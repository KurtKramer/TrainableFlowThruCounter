/* ShrimpLengthComputer.cpp -- Routines to compute the length of a shrimp.
 * Copyright (C) 2011-2013  Kurt Kramer
 * For conditions of distribution and use, see copyright notice in CounterUnManaged.txt
 */
#include "FirstIncludes.h"

#include <errno.h>
#include <istream>
#include <iostream>
#include <vector>
#include <math.h>
#include "MemoryDebug.h"
using namespace std;


#include "KKBaseTypes.h"
#include "ImageIO.h"
#include "OSservices.h"
#include "Raster.h"
using namespace KKB;

#include "MLClass.h"
using namespace KKMLL;


#include "ShrimpLengthComputer.h"
using  namespace  CounterUnManaged;


ShrimpLengthComputer::ShrimpLengthComputer (kkint32  _pixelsPerScanLine,
                                            float  _imageChamberWidth,
                                            kkint32  _erosionStructSize,
                                            uchar  _backgroundPixelTH
                                           ):
  backgroundPixelTH         (_backgroundPixelTH),
  centerPoints              (NULL),
  eigenRatio                (0.0f),
  erosionStructSize         (_erosionStructSize),
  imageIsOccluded           (false),
  imageIsDistorted          (false),
  demoImage                 (NULL),
  imageChamberWidth         (_imageChamberWidth),
  lenInPixels               (0.0f),
  lenInMiliMeters           (0.0f),
  mmPerScanLine             (63.5f),
  mmPerPixel                (0.0f),
  occlusiionLimit           (50),
  occlusionPixelValueTH     (180),
  orientationAngle          (0.0f),
  orientationAngleInDegrees (0.0f),
  pixelsPerScanLine         (_pixelsPerScanLine),
  rotatedImage              (NULL),
  saveDebugImages           (false),
  scanRate                  (20000.0f),
  workRaster                (NULL)
{
  if  (imageChamberWidth <= 0.0f)
    imageChamberWidth = 0.0635f;
  mmPerScanLine = imageChamberWidth * 1000.0f;
  mmPerPixel = mmPerScanLine / pixelsPerScanLine;
}



ShrimpLengthComputer::~ShrimpLengthComputer ()
{
  delete centerPoints;   centerPoints  = NULL;
  delete demoImage;      demoImage     = NULL;
  delete rotatedImage;   rotatedImage  = NULL;
  delete workRaster;     workRaster    = NULL;
}



kkMemSize  ShrimpLengthComputer::MemoryConsumedEstimated ()  const
{
  return  sizeof (*this);
}



void  ShrimpLengthComputer::PixelsPerScanLine (kkint32 _pixelsPerScanLine)
{
  pixelsPerScanLine = _pixelsPerScanLine;
  mmPerPixel = mmPerScanLine / (float)pixelsPerScanLine;
}



Point  ShrimpLengthComputer::DerivePreRotatedPoint (kkint32 height,
                                                    kkint32 width,
                                                    Point&  rotatedPoint, 
                                                    float   turnAngle
                                                   )
{
  kkint32  diag = (kkint32)sqrt ((float)(height * height + width * width)) + 10;

  float  a11 = (float)(cos (-turnAngle));
  float  a12 = (float)(sin (-turnAngle));
  float  b1  = width  * 0.5f;

  float  a21 = -a12;
  float  a22 = a11;
  float  b2  = height * 0.5f;

  kkint32  halfDiag = (diag + 1) / 2;

  kkint32  centDestRow = rotatedPoint.Row () - halfDiag;
  kkint32  centDestCol = rotatedPoint.Col () - halfDiag;

  kkint32  srcY = (kkint32)((float)(a21 * centDestCol) + (float)(a22 * centDestRow) + b2 + 0.5);
  kkint32  srcX = (kkint32)((float)(a11 * centDestCol) + (float)(a12 * centDestRow) + b1 + 0.5);

  return  Point (srcY, srcX);
}  /* DerivePreRotatedPoint */




bool  ShrimpLengthComputer::OccludedImage (RasterPtr  i)
{
  kkint32  tlRow;
  kkint32  tlCol;
  kkint32  brRow;
  kkint32  brCol;

  kkint32  pixelCountTopRow = 0;
  kkint32  pixelCountBotRow = 0;

  kkint32  pixelCountLeftCol = 0;
  kkint32  pixelCountRightCol = 0;

  i->FindBoundingBox (tlRow, tlCol, brRow, brCol);

  uchar**  data = i->Green ();
  uchar*   topRowData = data[tlRow];
  uchar*   botRowData = data[brRow];

  for  (kkint32 col = tlCol;  col < brCol;  ++col)
  {
    if  (topRowData[col] > occlusionPixelValueTH)
      ++pixelCountTopRow;
    if  (botRowData[col] > occlusionPixelValueTH)
      ++pixelCountBotRow;
  }

  for  (kkint32 row = tlRow;  row <= brRow;  ++row)
  {
    uchar*  rowData = data[row];
    if  (rowData[tlCol] > occlusionPixelValueTH)
      ++pixelCountLeftCol;

    if  (rowData[brCol] > occlusionPixelValueTH)
      ++pixelCountRightCol;
  }

  kkint32  height = 1 + brRow - tlRow;

  int  heightTH = Min ((int)((float)height * 0.2f), occlusiionLimit);

  bool  imageIsOccluded = ((pixelCountTopRow   > occlusiionLimit)  ||
                           (pixelCountBotRow   > occlusiionLimit)  ||
                           (pixelCountLeftCol  > heightTH)        ||
                           (pixelCountRightCol > heightTH)
                          );

  return  imageIsOccluded;
}  /* OccludedImage */



float  ShrimpLengthComputer::CalcHeightWidthRatio (RasterPtr  i)
{
  kkint32  size                = 0;
  kkint32  weight              = 0;
  float  centroidRow         = 0.0f;
  float  centroidCol         = 0.0f;
  float  centroidRowWeighted = 0.0f;
  float  centroidColWeighted = 0.0f;

  i->CalcCentroid (size, weight, centroidRow, centroidCol, centroidRowWeighted, centroidColWeighted);

  uchar**  data = i->Green  ();
  kkint32  height = i->Height ();
  kkint32  width  = i->Width  ();

  // Find row range that encompass 80% of pixels of image.
  kkint32  topRow = (int)floor (centroidRow);
  kkint32  botRow = topRow + 1;

  kkint32  pixelsAcountedFor = 0;
  kkint32  pixelsThatNeedToBeAccountedFor = (kkint32)((float)size * 0.85f);
  while  (pixelsAcountedFor < pixelsThatNeedToBeAccountedFor)
  {
    for (kkint32 col = 0;  col < width;  ++col)
    {
      if  (topRow >= 0)
      {
        if  (data[topRow][col] > 2)
          ++pixelsAcountedFor;
      }
      if  (botRow < height)
      {
        if  (data[botRow][col] > 2)
          ++pixelsAcountedFor;
      }
    }

    --topRow;
    ++botRow;
  }

  float  heightBulk = 1.0f + (float)botRow - (float)topRow;

  kkint32  leftCol = (int)floor (centroidCol);
  kkint32  rightCol = leftCol + 1;

  pixelsAcountedFor = 0;

  while  (pixelsAcountedFor < pixelsThatNeedToBeAccountedFor)
  {
    for (kkint32 row = 0;  row < height;  ++row)
    {
      if  (leftCol >= 0)
      {
        if  (data[row][leftCol] > 2)
          ++pixelsAcountedFor;
      }
      if  (rightCol < width)
      {
        if  (data[row][rightCol] > 2)
          ++pixelsAcountedFor;
      }
    }

    --leftCol;
    ++rightCol;
  }

  float  widthBulk = 1.0f + (float)rightCol - (float)leftCol;

  return  heightBulk / widthBulk;
}  /* CalcHeightWidthRatio */




int  ShrimpLengthComputer::ErodeBySize (Raster& i)
{
  kkint32  numOfErosionsToDo = 1;

  kkint32  area = i.CalcArea ();

  if  (area < 1000)
    numOfErosionsToDo = 0;

  else if  (area < 5000)
    numOfErosionsToDo = 1;

  else if  (area < 10000)
    numOfErosionsToDo = 2;

  else if  (area < 20000)
    numOfErosionsToDo = 3;

  else if  (area < 40000)
    numOfErosionsToDo = 4;

  else if  (area < 80000)
    numOfErosionsToDo = 5;

  else if  (area < 120000)
    numOfErosionsToDo = 5;

  else
    numOfErosionsToDo = 6;

  for (int x = 0;  x < numOfErosionsToDo;  ++x)
    i.Erosion (MorphOp::MaskTypes::SQUARE3);

  return  numOfErosionsToDo;
}  /* ErodeBySize */



void  ShrimpLengthComputer::ErodeImageByStructSize (Raster& i)
{
  for (int x = 0;  x < erosionStructSize;  ++x)
    i.Erosion (MorphOp::MaskTypes::SQUARE3);
}


void  ShrimpLengthComputer::ProcessImage (RasterPtr  origImage,
                                          bool       buildDemoImage
                                         )
{
  delete  workRaster;    workRaster    = NULL;
  delete  rotatedImage;  rotatedImage  = NULL;
  delete  demoImage;     demoImage     = NULL;
  delete  centerPoints;  centerPoints  = NULL;

  KKStr  baseDir = "C:\\Temp\\LengthComputerDir\\" + osGetRootName (origImage->FileName ());
  if  (saveDebugImages)
    osCreateDirectoryPath (baseDir);
  KKStr  baseName = osAddSlash (baseDir) + osGetRootName (origImage->FileName ());

  if  (saveDebugImages)
    SaveImageGrayscaleInverted8Bit (*origImage, baseName + "_00_BaseName.bmp");

  lenInPixels               = 0.0f;
  lenInMiliMeters           = 0.0f;
  imageIsDistorted          = false;
  eigenRatio                = 0.0f;
  orientationAngle          = 0.0f;
  orientationAngleInDegrees = 0.0f;

  imageIsOccluded = OccludedImage (origImage);
  if  (imageIsOccluded)
    return;

  workRaster = origImage->BinarizeByThreshold (backgroundPixelTH, 255);
  if  (saveDebugImages)
    SaveImageGrayscaleInverted8Bit (*workRaster, baseName + "_01_Binarize.bmp");

  if  (erosionStructSize >= 0)
  {
    ErodeImageByStructSize (*workRaster);
    if  (saveDebugImages)
      SaveImageGrayscaleInverted8Bit (*workRaster, baseName + "_02_ErodedByStructSize_" + StrFormatInt (erosionStructSize, "00") + ".bmp");
  }
  else
  {
    int numErosions = ErodeBySize (*workRaster);
    if  (saveDebugImages)
      SaveImageGrayscaleInverted8Bit (*workRaster, baseName + "_03_ErodeBySize_" + StrFormatInt (numErosions, "00") +  "_" + StrFormatInt (erosionStructSize, "00") + ".bmp");
  }

  workRaster->FillHole ();
  if  (saveDebugImages)
    SaveImageGrayscaleInverted8Bit (*workRaster, baseName + "_04_FillHole.bmp");

  workRaster->ConnectedComponent (1);
  if  (saveDebugImages)
    SaveImageGrayscaleInverted8Bit (*workRaster, baseName + "_05_ConnectedComponent.bmp");

  workRaster->CalcOrientationAndEigerRatio (eigenRatio, orientationAngle);
  if  ((orientationAngle > TwoPie)  ||  (orientationAngle < -TwoPie))
  {
    orientationAngle = 0.0;
    eigenRatio = 1.0;
  }

  orientationAngleInDegrees = orientationAngle * (float)RadToDeg;
  float  origOrientationInDeg = orientationAngleInDegrees;

  while  (orientationAngleInDegrees > 90.0f)
    orientationAngleInDegrees -= 180;

  while  (orientationAngleInDegrees < -90.0f)
    orientationAngleInDegrees += 180;


  if  ((orientationAngleInDegrees < -30.0f)  ||  (orientationAngleInDegrees > 30.0f))
  {
    if  (saveDebugImages)
      SaveImageGrayscaleInverted8Bit (*workRaster, baseName + "05_Distorted_" + StrFormatDouble (orientationAngleInDegrees, "-000.0") + ".bmp");
    imageIsDistorted = true;
    return;
  }

  rotatedImage = workRaster->Rotate (orientationAngle);
  if  (saveDebugImages)
    SaveImageGrayscaleInverted8Bit (*rotatedImage, baseName + "05_Rotated_" + StrFormatDouble (origOrientationInDeg, "-000.0") + ".bmp");
  kkint32  tlRow;
  kkint32  tlCol;
  kkint32  brRow;
  kkint32  brCol;

  rotatedImage->FindBoundingBox (tlRow, tlCol, brRow, brCol);
  if  (tlRow >= 0)
  {
    kkint32  boxWidth  = brCol - tlCol;

    float heightWidthRatio = CalcHeightWidthRatio (rotatedImage);

    imageIsDistorted = false;
    if  (heightWidthRatio > 0.25f)
      imageIsDistorted = true;

    else if  ((fabs (orientationAngleInDegrees) > 20.0f)  &&  (heightWidthRatio < 0.09f))
      imageIsDistorted = true;

    if  (!imageIsDistorted)
    {
      uchar**  imageData = rotatedImage->Green ();

      kkint32  mark1Col = (kkint32)((float)boxWidth * 0.10f + 0.5f) + tlCol;
      kkint32  mark2Col = (kkint32)((float)boxWidth * 0.50f + 0.5f) + tlCol;
      kkint32  mark3Col = (kkint32)((float)boxWidth * 0.90f + 0.5f) + tlCol;

      kkint32  a1RowTot   = 0;
      kkint32  a1ColTot   = 0;
      kkint32  a1PixCount = 0;

      kkint32  a2RowTot   = 0;
      kkint32  a2ColTot   = 0;
      kkint32  a2PixCount = 0;

      kkint32  a3RowTot   = 0;
      kkint32  a3ColTot   = 0;
      kkint32  a3PixCount = 0;

      kkint32  a4RowTot   = 0;
      kkint32  a4ColTot   = 0;
      kkint32  a4PixCount = 0;

      for  (kkint32 row = tlRow;  row <= brRow;  ++row)
      {
        kkint32 col = 0;

        uchar*  rowData = imageData[row];

        for  (col = tlCol;  col <= mark1Col;  ++col)
        {
          if  (rowData[col] > 2)
          {
            ++a1PixCount;
            a1RowTot += row;
            a1ColTot += col;
          }
        }

        for  (col = mark1Col + 1;  col <= mark2Col;  ++col)
        {
          if  (rowData[col] > 2)
          {
            ++a2PixCount;
            a2RowTot += row;
            a2ColTot += col;
          }
        }

        for  (col = mark2Col + 1;  col <= mark3Col;  ++col)
        {
          if  (rowData[col] > 2)
          {
            ++a3PixCount;
            a3RowTot += row;
            a3ColTot += col;
          }
        }

        for  (col = mark3Col + 1;  col <= brCol;  ++col)
        {
          if  (rowData[col] > 2)
          {
            ++a4PixCount;
            a4RowTot += row;
            a4ColTot += col;
          }
        }
      }

      a1PixCount = Max (1, a1PixCount);
      a2PixCount = Max (1, a2PixCount);
      a3PixCount = Max (1, a3PixCount);
      a4PixCount = Max (1, a4PixCount);

      Point p1 ((int)(0.5f + a1RowTot / a1PixCount), (int)(0.5f + a1ColTot / a1PixCount));
      Point p2 ((int)(0.5f + a2RowTot / a2PixCount), (int)(0.5f + a2ColTot / a2PixCount));
      Point p3 ((int)(0.5f + a3RowTot / a3PixCount), (int)(0.5f + a3ColTot / a3PixCount));
      Point p4 ((int)(0.5f + a4RowTot / a4PixCount), (int)(0.5f + a4ColTot / a4PixCount));

      Point p1Orig = DerivePreRotatedPoint (origImage->Height (), origImage->Width (), p1, orientationAngle);
      Point p2Orig = DerivePreRotatedPoint (origImage->Height (), origImage->Width (), p2, orientationAngle);
      Point p3Orig = DerivePreRotatedPoint (origImage->Height (), origImage->Width (), p3, orientationAngle);
      Point p4Orig = DerivePreRotatedPoint (origImage->Height (), origImage->Width (), p4, orientationAngle);

      centerPoints = new PointList (true);
      centerPoints->PushOnBack (new Point (p1Orig));
      centerPoints->PushOnBack (new Point (p2Orig));
      centerPoints->PushOnBack (new Point (p3Orig));
      centerPoints->PushOnBack (new Point (p4Orig));

      lenInPixels = centerPoints->ComputeSegmentLens (1.0f, 1.0f);
      lenInMiliMeters = lenInPixels * mmPerPixel;

      delete  demoImage;
      demoImage = NULL;
      if  (buildDemoImage)
      {
        demoImage = origImage->ToColor ();

        for  (kkuint32 x = 1;  x < centerPoints->QueueSize ();  ++x)
        {
          PointPtr p1 = centerPoints->IdxToPtr (x - 1);
          PointPtr p2 = centerPoints->IdxToPtr (x);
          demoImage->DrawFatLine (*p1, *p2, PixelValue::Red, 0.80f);
        }

        if  (saveDebugImages)
          SaveImageGrayscaleInverted8Bit (*demoImage, baseName + "07_Rotated_DemoImage.bmp");
      }
    }
  }
}  /* ProcessImage */




