#include  "FirstIncludes.h"


//-d E:\Larcos\ScannerFiles\2013-01-30_SIS-PL8-Run3\SIS-PL8-Run3_20130130-145605_SIS-PL8-Run3-1000_00\Shrimp_01


#include <stdlib.h>
#include <memory>
#include <string>
#include <iostream>
#include <fstream>
#include <map>
#include <vector>


#include "MemoryDebug.h"


using namespace std;

#include "KKBaseTypes.h"
#include "ImageIO.h"
#include "KKStr.h"
#include "MorphOpStretcher.h"
#include "MsgQueue.h"
#include "OSservices.h"
using namespace  KKB;

#include "LarcosCounterManager.h"
using namespace   LarcosCounterUnManaged;

#include  "HistogramSize.h"



HistogramSize::HistogramSize (int     argc,
                              char**  argv
                             ):
  Application (),
  distordedDirName      (),
  occludedDirName       (),
  occlusiionLimit       (50),
  occlusionPixelValueTH (180),
  report                (NULL),
  reportFile            (NULL),
  reportFileName        (),
  saveDirName           (),
  targetDir             (),
  workDir               (),
  histogram             (),
  histogramBy5          (),

  //pixelsPerScanLine    (1832 - 365),
  pixelsPerScanLine     (1673 - 410),
  mmPerScanLine         (72.0f),
  mmPerPixel            (0.0f),
  scanRate              (20000.0f)


{
  InitalizeApplication (argc, argv);

  if  (Abort ())
  {
    DisplayCommandLineParameters ();
    return;
  }

  if  (targetDir.Empty ())
  {
    targetDir = osGetCurrentDirectory ();
  }

  if  (saveDirName.Empty ())
    saveDirName = osAddSlash (targetDir) + "ImagesWithLinesDrawm";
  osCreateDirectoryPath (saveDirName);

  if  (occludedDirName.Empty ())
    occludedDirName = osAddSlash (targetDir) + "ImagesOcluded";
  osCreateDirectoryPath (occludedDirName);

  if  (distordedDirName.Empty ())
    distordedDirName = osAddSlash (targetDir) + "ImagesDistoted";
  osCreateDirectoryPath (distordedDirName);

  workDir = osAddSlash (targetDir) + "ImagesWork";
  osCreateDirectoryPath (workDir);

  if  (reportFileName.Empty ())
  {
    reportFileName = osAddSlash (saveDirName) + osGetDirNameFromPath (targetDir) + ".txt";
    reportFile = NULL;
    report = &cout;
  }

  reportFile = new ofstream (reportFileName.Str ());
  report = reportFile;

  mmPerPixel = mmPerScanLine / pixelsPerScanLine;


  *report << "Creating Diversified Training Images by Flow Rate Factor." << endl;

  *report << "Report File Name"      << "\t" << reportFileName     << endl;
  *report << "Target Directory"      << "\t" << targetDir          << endl;
  *report << "Scan Rate"             << "\t" << scanRate           << endl;
  *report << "Pixels/ScanLine"       << "\t" << pixelsPerScanLine  << endl;
  *report << "Mili-Meters/ScanLine"  << "\t" << mmPerScanLine      << endl;
  *report << "Scan Rate"             << "\t" << scanRate           << endl;
}  /* HistogramSize */



HistogramSize::~HistogramSize ()
{
  if  (reportFile)
  {
    reportFile->close ();
    delete  reportFile;
  }
}



bool  HistogramSize::ProcessCmdLineParameter (char   parmSwitchCode, 
                                              KKStr  parmSwitch, 
                                              KKStr  parmValue
                                             )
{
  if  (parmSwitch.EqualIgnoreCase ("-Report")      ||
       parmSwitch.EqualIgnoreCase ("-ReportFile")  ||
       parmSwitch.EqualIgnoreCase ("-R")
      )
    reportFileName = parmValue;
 
  else 
  if  (parmSwitch.EqualIgnoreCase ("-Dir")      ||
       parmSwitch.EqualIgnoreCase ("-Directory")  ||
       parmSwitch.EqualIgnoreCase ("-D")
      )
    targetDir = parmValue;

  else 
  if  (parmSwitch.EqualIgnoreCase ("-ScanRate")      ||
       parmSwitch.EqualIgnoreCase ("-SR")
      )
    scanRate = parmValue.ToFloat ();


  return  !Abort ();
}  /* ProcessCmdLineParameter */


// -RootDir E:\Larcos\Classifier\TrainingLibraries\VaShrimp2.5g_Struct5_FR-Diversified


void   HistogramSize::DisplayCommandLineParameters ()
{
  log.Level (0) << "HistogramSize  -RootDir <xxx> -Report <xxxx>" << endl;
  log.Level (0) << endl;
  log.Level (0) << "    -RootDir  Root directory of traininig library." << endl;
  log.Level (0)                                                         << endl;
  log.Level (0) << "    -Report  Report File."                          << endl;
  log.Level (0)                                                         << endl;
}  /* DisplayCommandLineParameters */





void  HistogramSize::ProcessDirectory ()
{
  if  (report)
  {
    *report << "RootName"         << "\t"  
            << "Len(Pixels)"      << "\t" 
            << "Len(mm)"          << "\t" 
            << "Area"             << "\t" 
            << "heightWidthRatio" << "\t" 
            << "Height"           << "\t" 
            << "Width"
            << endl;
  }


  KKStr  fileSpec = osAddSlash (targetDir) + "*.bmp";
  KKStrListPtr  files = osGetListOfFiles (fileSpec);
  if  ((files != NULL)  &&  (files->QueueSize () > 0))
  {
    KKStrList::iterator idx;
    for  (idx = files->begin ();  idx != files->end ();  ++idx)
    {
      KKStr  fileName = *idx;
      cout << fileName << endl;
      KKStr  fullFileName = osAddSlash (targetDir) + fileName;
      ProcessImage (fullFileName);
    }
  }


  if  (report)
  {
    *report << endl << endl
      << "Length(mm)" << "\t" << "count" << endl;
    for  (kkuint32 x = 0;  x < histogram.size ();  ++x)
    {
      *report << x << "\t" << histogram[x] << endl;
    }


    *report << endl << endl
      << "Length(mm)" << "\t" << "count" << endl;
    for  (kkuint32 x = 0;  x < histogramBy5.size ();  ++x)
    {
      *report << ((x + 1) * 5) << "\t" << histogramBy5[x] << endl;
    }
  }

}  /* ProcessDirectory */





Point  HistogramSize::DerivePreRotatedPoint (kkint32 height,
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




bool  HistogramSize::OccludedImage (RasterPtr  i)
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

  bool  imageOccluded = ((pixelCountTopRow   > occlusiionLimit)  ||
                         (pixelCountBotRow   > occlusiionLimit)  ||
                         (pixelCountLeftCol  > heightTH)        ||
                         (pixelCountRightCol > heightTH)
                        );

  return  imageOccluded;
}  /* OccludedImage */






float  HistogramSize::CalcHeightWidthRatio (RasterPtr  i)
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

  // Find row ranethat encompases 80% of pixels of image.
  kkint32  topRow = (int)floor (centroidRow);
  kkint32  botRow = topRow + 1;

  kkint32  pixelsAcountedFor = 0;
  kkint32  pixelsThatNeedToBeAccountedFor = (kkint32)((float)size * 0.8f);
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



void  HistogramSize::Experiment (RasterPtr     i,
                                 const KKStr&  rootName
                                )
{
  kkint32  area = i->CalcArea ();
  KKStr  fullName = osAddSlash (workDir) +  rootName + "__" + StrFormatInt (area, "000000");
  KKB::SaveImageGrayscaleInverted4Bit (*i, fullName + "__0-Orig.bmp");

  {
    RasterPtr  thImage = i->BinarizeByThreshold (100, 255);
    KKB::SaveImageGrayscaleInverted4Bit (*thImage, fullName + "__1-TH.bmp");
    delete  thImage;
  }


  //ErodeBySize (i);
  //KKB::SaveImageGrayscaleInverted4Bit (*erodedImage, fullName + "__2-E.bmp");
  //delete  erodedImage;
  //erodedImage = NULL;
}  /* Experiment */




void  HistogramSize::ErodeBySize (Raster& i)
{
  kkint32  numOfErosionsToDo = 1;

  kkint32  area = i.CalcArea ();


  if  (area < 5000)
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
    i.Erosion (SQUARE3);

  return;
}  /* ErodeBySize */





void  HistogramSize::ProcessImage (const KKStr& fullFileName)
{
  RasterPtr origImage = ReadImage (fullFileName);
  if  (!origImage)
  {
    log.Level (-1) << "ProcessImage   File[" << fullFileName << "]   failed to load." << endl;
    return;
  }

  KKStr  rootName = osGetRootName (fullFileName);
  bool  imageOccluded = OccludedImage (origImage);
  if  (imageOccluded)
  {
    KKStr  saveFileName = osAddSlash (occludedDirName) + osGetRootName (fullFileName) + ".bmp";
    KKB::SaveImageGrayscaleInverted4Bit (*origImage, saveFileName);
    return;
  }

  PointListPtr  results = NULL;

  float  eigenRatio;
  float  orientationAngle;

  kkint32  area = origImage->CalcArea ();

  RasterPtr  workRaster = origImage->BinarizeByThreshold (100, 255);
  ErodeBySize (*workRaster);
  workRaster->FillHole ();
  workRaster->ConnectedComponent (1);

  workRaster->CalcOrientationAndEigerRatio (eigenRatio,
                                            orientationAngle
                                           );
  if  ((orientationAngle > TwoPie)  ||  (orientationAngle < -TwoPie))
  {
    orientationAngle = 0.0;
    eigenRatio = 1.0;
  }

  float orientationAngleInDegrees = orientationAngle * 180.0f / (float)PIE;

  while  (orientationAngleInDegrees > 90.0f)
    orientationAngleInDegrees -= 180;

  while  (orientationAngleInDegrees < -90.0f)
    orientationAngleInDegrees += 180;

  RasterPtr rotatedImage = workRaster->Rotate (orientationAngle);
  kkint32  tlRow;
  kkint32  tlCol;
  kkint32  brRow;
  kkint32  brCol;

  rotatedImage->FindBoundingBox (tlRow, tlCol, brRow, brCol);
  if  (tlRow >= 0)
  {
    kkint32  boxWidth  = brCol - tlCol;
    kkint32  boxHeight = brRow - tlRow;

    float heightWidthRatio = CalcHeightWidthRatio (rotatedImage);

    bool  imageIsDistorted = false;
    if  (heightWidthRatio > 0.37f)
      imageIsDistorted = true;

    else if  ((fabs (orientationAngleInDegrees) > 20.0f)  &&  (heightWidthRatio < 0.09f))
      imageIsDistorted = true;

    if  (imageIsDistorted)
    {
      int  heightWidthRatioInt = (int)(0.5f + 1000.0f * heightWidthRatio);
      log.Level (10) << "Image" << "\t" << rootName << "\t" << " is to narrow  or fat." << endl;
      KKStr  saveFileName = osAddSlash (distordedDirName) + 
                            osGetRootName (fullFileName) + "___" + 
                            StrFormatDouble (heightWidthRatioInt, "000") + 
                            ".bmp";
      KKB::SaveImageGrayscaleInverted4Bit (*origImage, saveFileName);

      KKStr  rotatedFileName = osAddSlash (distordedDirName) + osGetRootName (fullFileName) + "_Rotated.bmp";
      KKB::SaveImageGrayscaleInverted4Bit (*rotatedImage, rotatedFileName);
    }
    else
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

      results = new PointList (true);
      results->PushOnBack (new Point (p1Orig));
      results->PushOnBack (new Point (p2Orig));
      results->PushOnBack (new Point (p3Orig));
      results->PushOnBack (new Point (p4Orig));

      float  lenInPixels = results->ComputeSegmentLens (1.0f, 1.0f);
      float  lenInMiliMeters = lenInPixels * mmPerPixel;

      RasterPtr  imageWithLine = origImage->ToColor ();


      for  (kkint32 x = 1;  x < results->QueueSize ();  ++x)
      {
        PointPtr p1 = results->IdxToPtr (x - 1);
        PointPtr p2 = results->IdxToPtr (x);
        imageWithLine->DrawFatLine (*p1, *p2, PixelValue::Red, 0.80f);
      }

      int  heightWidthRatioInt = (int)(0.5f + 1000.0f * heightWidthRatio);
      int  sizeOfImage = origImage->CalcArea ();
      KKStr  saveFileName = osAddSlash (saveDirName) + 
                            osGetRootName (fullFileName) + 
                            "__" + StrFormatInt (heightWidthRatioInt, "000") +
                            ".bmp";
      SaveImage (*imageWithLine, saveFileName);

      if  (report)
      {
        *report << rootName << "\t"  << lenInPixels << "\t" << lenInMiliMeters << "\t" << area << "\t" << heightWidthRatio << "\t" << boxHeight << "\t" << boxWidth  << endl;
      }

      kkuint32  histogramIdx = (int)(lenInMiliMeters);
      while  (histogramIdx >= histogram.size ())
        histogram.push_back (0);
      (histogram[histogramIdx])++;


      kkuint32  histogram5Idx = (int)(lenInMiliMeters / 5.0f);
      while  (histogram5Idx >= histogramBy5.size ())
        histogramBy5.push_back (0);
      (histogramBy5[histogram5Idx])++;

      delete  imageWithLine;  imageWithLine = NULL;
    }
  }

  delete rotatedImage;  rotatedImage = NULL;
  delete workRaster;    workRaster   = NULL;

  delete  results;        results       = NULL;
  delete  origImage;      origImage     = NULL;
}  /* ProcessImage */



#include "ScannerFile.h"

using namespace  KKLSC;






bool  OkToRenameFile (const KKStr&  fn)
{
  kkint32 idx = fn.Find ("_2013");
  if  (idx < 0)
    idx = fn.Find ("_2012");

  if  (idx < 0)  return false;

  KKStr  part1 = fn.SubStrPart (0, idx - 1);
  KKStr  rest = fn.SubStrPart (idx + 1);

  if  (rest[8] != '-')
    return false;

  KKStr  dateStr = rest.SubStrPart (0, 7);
  rest = rest.SubStrPart (9);

  if  (rest.SubStrPart (6) == ".lsc")
    return true;

  if  (rest[6] != '_')
    return false;

  KKStr  timeStr = rest.SubStrPart (0, 5);
  rest = rest.SubStrPart (7);

  if  (isdigit (rest[0])  &&  isdigit (rest[1])  &&  (rest.SubStrPart (2) == ".lsc"))
    return true;

  if  (isdigit (rest[0])  &&  isdigit (rest[1])  &&  isdigit (rest[2])  &&  (rest.SubStrPart (3) == ".lsc"))
    return true;

  return false;
}  /* OkToRenameFile */




KKStr  MakeNewName (const KKStr&  fn,
                    float         scanRate, 
                    KKStr         sensitivityMode, 
                    const KKStr&  flatField,
                    kkint32       bitDepth
                   )
{
  // Example Names
  //   "FGCU_ModelT_20130705-132437.lsc"
  //   "FGCU_ModelT_20130705-132437_00.lsc"  // Recorded File Played Back
  //   "PI_PL8_20130208-104448_000.lsc"      // Contunuation

  if  (sensitivityMode.Empty ())
    sensitivityMode = "Low";

  KKStr  infoStr (32);
  infoStr << "SR-"   << StrFormatDouble (scanRate / 1000, "0") << "k" << "_" 
          << "Sen-"  << sensitivityMode                               << "_"
          << "FF-"   << flatField                                    << "_"
          << "BD-"   << StrFormatInt (bitDepth, "0");

  kkint32  len = fn.Len ();

  KKStr part1 = "";
  KKStr part2 = "";

  if  (fn.Tail (4) != ".lsc")
    return fn;

  if  (fn[len - 8] == '_')
  {
    part1 = fn.SubStrPart (0, len - 9);
    part2 = fn.SubStrPart (len - 8);
  }

  else if  (fn[len - 7] == '_')
  {
    part1 = fn.SubStrPart (0, len - 5);
    part2 = fn.SubStrPart (len - 4);
  }

  else
  {
    part1 = fn.SubStrPart (0, len - 5);
    part2 = fn.SubStrPart (len - 4);
  }

  KKStr  newName = part1 + "_" + infoStr + part2;

  return  newName;
}




void  ScannerFileCharacteristics_ProcessOneDir (const KKStr&  dir,
                                                const KKStr&  title,
                                                ostream&      o
                                               )
{ 
  KKStrListPtr  scannerFileNames = KKB::osGetListOfFiles (osAddSlash (dir) + "*.lsc");

  if  (!scannerFileNames)
  {
    o << endl
      << "ERROR   retrieving file names from Directory[" + dir + "]" << endl
      << endl;
    return;
  }
  RunLog  runLog;

  o << endl << endl << dir << endl;


  KKStrList::iterator  idx;
  for  (idx = scannerFileNames->begin ();  idx != scannerFileNames->end ();  ++idx)
  {
    KKStr  fn = **idx;
    KKStr  fullName = osAddSlash (dir) + fn;

    kkint64  fileSizeBytes = KKB::osGetFileSize (fullName);

    ScannerFilePtr  sf = ScannerFile::CreateScannerFile (fullName, runLog);
    if  (!sf)
    {
      o    << fn << "\t" << "Error Opening File" << endl;
      cout << fn << "\t" << "Error Opening File" << endl;
    }
     
    else
    {
      KKStr  fileFormatStr = sf->FileFormatStr ();
      kkuint32  bitDepth = 8;

      bool  cancelFlag = false;
      sf->BuildFrameOffsets (cancelFlag);
      if  (sf->LargestKnownScanLine () < 1)
      {
        cout << endl << "Could not retrieve number of scan lines." << endl << endl;
      }

      switch  (sf->FileFormat ())
      {
      case  ScannerFile::sfSimple:          bitDepth = 8;   break;
      case  ScannerFile::sf3BitEncoded:     bitDepth = 3;   break;
      case  ScannerFile::sf4BitEncoded:     bitDepth = 4;   break;
      case  ScannerFile::sfZlib3BitEncoded: bitDepth = 8;   break;
      }

      KKStr  dateTimeRecordedStr = "";
      KKStr  sensitivityMode = "";
      float  analogGain      = 0.0f;
      kkuint32 digitalGain     = 0;
      KKStr  flatFieldCorrectionEnabled = "DK";
      ScannerHeaderFieldsPtr  headerFields = sf->HeaderFields ();
      KKStr  playingBack = "DK";
      KKStr  description = "";
      kkint32  largestKnownScanLine = sf->LargestKnownScanLine ();
      float  scanRate = sf->ScanRate ();

      if  (headerFields)
      {
        fileFormatStr              = sf->FileFormatStr ();
        sensitivityMode            = headerFields->GetValue ("SensitivityMode");
        analogGain                 = headerFields->GetValue ("AnalogGain").ToFloat ();
        digitalGain                = headerFields->GetValue ("DigitalGain").ToInt32 ();
        sensitivityMode            = headerFields->GetValue ("SensitivityMode");
        flatFieldCorrectionEnabled = headerFields->GetValue ("FlatFieldCorrectionEnabled");
        playingBack                = headerFields->GetValue ("PlayingBack");
        description                = headerFields->GetValue ("Description");
        dateTimeRecordedStr        = headerFields->GetValue ("DateTimeRecorded");
      }

      if  (dateTimeRecordedStr.Empty ())
      {
        DateTime dt = KKB::osGetFileDateTime (fullName);
        dateTimeRecordedStr = dt.Date ().YYYY_MMM_DD () + "-" + dt.Time ().HH_MM_SS ();
      }

      float  sizeInSecs = 0.0f;
      if  (scanRate != 0.0f)
        sizeInSecs = (float)largestKnownScanLine / scanRate;

      delete  sf;
      sf = NULL;

      KKStr  oldName = fn;
      if  (OkToRenameFile (fn))
      {
        fn = MakeNewName (fn, scanRate, sensitivityMode, flatFieldCorrectionEnabled, bitDepth);
        KKB::osRenameFile (osAddSlash (dir) + oldName, osAddSlash (dir) + fn);
      }

      o << title                      << "\t"
        << fn                         << "\t"
        << dateTimeRecordedStr        << "\t"
        << fileSizeBytes              << "\t"
        << largestKnownScanLine       << "\t"
        << sizeInSecs                 << "\t"
        << fileFormatStr              << "\t"
        << bitDepth                   << "\t"
        << sensitivityMode            << "\t"
        << analogGain                 << "\t"
        << digitalGain                << "\t"
        << scanRate                   << "\t"
        << flatFieldCorrectionEnabled << "\t"
        << playingBack                << "\t"
        << description
        << endl;

      cout << fn << "\t" << largestKnownScanLine << "\t" << fileSizeBytes << "\t" << sizeInSecs << endl;
    }
  }
}  /* ScannerFileCharacteristics_ProcessOneDir */




void  ScannerFileCharacteristics_ProcessDirTree (const KKStr&  baseDir,
                                                 const KKStr&  subDir,
                                                 ostream&      o
                                                )
{
  KKStr  dir = "";
  KKStr  title = "Base";
  if  (subDir.Empty ())
  {
    dir = baseDir;
    title = "Basse";
  }
  else
  {
    title = subDir;
    dir = osAddSlash (baseDir) + subDir;
  }

  ScannerFileCharacteristics_ProcessOneDir (dir, title, o);

  KKStrListPtr  subDirectories = KKB::osGetListOfDirectories (dir);
  if  (subDirectories)
  {
    KKStrList::iterator  idx;
    for  (idx = subDirectories->begin ();  idx != subDirectories->end ();  ++idx)
    {
      KKStr nextSubDir = "";

      if  (subDir.Empty ())
        nextSubDir = *idx;
      else
        nextSubDir = osAddSlash (subDir) + (*idx);

      ScannerFileCharacteristics_ProcessDirTree (baseDir, nextSubDir, o);
    }

    delete  subDirectories;
    subDirectories = NULL;
  }
}  /* ScannerFileCharacteristics_ProcessDirTree */







#include "ScannerFile.h"
#include "StartStopPoint.h"
#include "InstallationConfig.h"
#include "OperatingParameters.h"
#include "SessionParameters.h"

using namespace LarcosCounterUnManaged;

class  Region
{
public:
  Region (kkint32  _start,  kkint32  _end): 
       start(_start), 
       end (_end) 
     {}

  kkint32  start;
  kkint32  end;
};



vector<Region>  BuildRegionList  (const  StartStopPointList&  list)
{
  vector<Region>  regions;

  if  (list.size () < 1)
  {
    regions.push_back (Region (0, int32_max));
    return  regions;
  }

  kkint32  prevLineNum = 0;
  StartStopPoint::StartStopType  prevType = StartStopPoint::sspStartPoint;

  StartStopPointList::const_iterator  idx = list.begin ();
  while  (idx != list.end ())
  {
    kkint32  nextLineNum = (*idx)->ScanLineNum ();
    StartStopPoint::StartStopType  nextType = (*idx)->Type ();

    if  (prevType == StartStopPoint::sspStartPoint)
    {
      if  (nextType == StartStopPoint::sspStopPoint)
      {
        regions.push_back (Region (prevLineNum, nextLineNum));
        cout << nextLineNum << "\t"  << "New region: " << prevLineNum << " - " << nextLineNum << endl;
        prevType = nextType;
        prevLineNum = nextLineNum;

      }
      else
      {
        // Since we appear to have two start points in a row we will ignore the second one.
        cout << nextLineNum << "\t"  << "Two Start Points in a row." << endl;
      }
    }
    else
    {
      // previous point was StopPoint.
      if  (nextType == StartStopPoint::sspStopPoint)
      {
        // We have two stop points in a row;  will ignore
        cout << nextLineNum << "\t"  << "Two Stop points in row." << endl;
      }
      else
      {
        cout << nextLineNum << "\t"  << "New Start point." << endl;
        prevType = nextType;
        prevLineNum = nextLineNum;
      }
    }


    ++idx;
  }

  if  (prevType == StartStopPoint::sspStartPoint)
  {
    regions.push_back (Region (prevLineNum, int32_max));
  }

  return  regions;
}  /* BuildRegionList */







void  CombineScannerFiles ()
{
  RunLog  runLog;

  /*
  char*  scannerFiles[] = {
                     "F:\\Larcos\\ScannerFiles\\2013-11-20_FGCU_Vaki\\2013-11-21_HighDensity3\\HighDensity3_20131121-113122.lsc",
                     "F:\\Larcos\\ScannerFiles\\2013-11-20_FGCU_Vaki\\2013-11-21_dense1\\dense1_20131121-101703.lsc",
                     "F:\\Larcos\\ScannerFiles\\2013-11-20_FGCU_Vaki\\2013-11-21_sh-gsh\\sh-gsh_20131121-094205.lsc",
                     "F:\\Larcos\\ScannerFiles\\2013-11-20_FGCU_Vaki\\2013-11-21_sh-gsh-test\\sh-gsh-test_20131121-094029.lsc",
                     "F:\\Larcos\\ScannerFiles\\2013-11-20_FGCU_Vaki\\2013-11-21_test2\\test2_20131121-095915.lsc",
                     "F:\\Larcos\\ScannerFiles\\2013-11-20_FGCU_Vaki\\2013-11-21_test3\\test3_20131121-101026.lsc",
                     "F:\\Larcos\\ScannerFiles\\2013-11-20_FGCU_Vaki\\2013-11-21_Density2\\Density2_20131121-104854.lsc",
                     "F:\\Larcos\\ScannerFiles\\2013-11-20_FGCU_Vaki\\2013-11-21_HighDensity4\\HighDensity4_20131121-114413.lsc",
                     "F:\\Larcos\\ScannerFiles\\2013-11-20_FGCU_Vaki\\2013-11-21_HighDensity\\highdens_20131121-104222.lsc",
                      NULL
                    };
  */
  /*
  char*  scannerFiles[] = {
                     "F:\\Larcos\\ScannerFiles\\2013-11-20_FGCU_Vaki\\2013-11-21_HighDensity4\\HighDensity4_20131121-114413.lsc",
                     "F:\\Larcos\\ScannerFiles\\2013-11-20_FGCU_Vaki\\2013-11-21_sh-gsh\\sh-gsh_20131121-094205.lsc",
                     "F:\\Larcos\\ScannerFiles\\2013-11-20_FGCU_Vaki\\2013-11-21_Density2\\Density2_20131121-104854.lsc",
                     "F:\\Larcos\\ScannerFiles\\2013-11-20_FGCU_Vaki\\2013-11-21_HighDensity\\highdens_20131121-104222.lsc",
                     "F:\\Larcos\\ScannerFiles\\2013-11-20_FGCU_Vaki\\2013-11-21_Density2\\Density2_20131121-104854.lsc",
                     "F:\\Larcos\\ScannerFiles\\2013-11-20_FGCU_Vaki\\2013-11-21_HighDensity3\\HighDensity3_20131121-113122.lsc",
                     "F:\\Larcos\\ScannerFiles\\2013-11-20_FGCU_Vaki\\2013-11-21_sh-gsh\\sh-gsh_20131121-094205.lsc",
                     "F:\\Larcos\\ScannerFiles\\2013-11-20_FGCU_Vaki\\2013-11-21_HighDensity4\\HighDensity4_20131121-114413.lsc",
                     "F:\\Larcos\\ScannerFiles\\2013-11-20_FGCU_Vaki\\2013-11-21_Density2\\Density2_20131121-104854.lsc",
                     "F:\\Larcos\\ScannerFiles\\2013-11-20_FGCU_Vaki\\2013-11-21_HighDensity4\\HighDensity4_20131121-114413.lsc",
                     "F:\\Larcos\\ScannerFiles\\2013-11-20_FGCU_Vaki\\2013-11-21_HighDensity\\highdens_20131121-104222.lsc",
                     "F:\\Larcos\\ScannerFiles\\2013-11-20_FGCU_Vaki\\2013-11-21_HighDensity3\\HighDensity3_20131121-113122.lsc",
                     "F:\\Larcos\\ScannerFiles\\2013-11-20_FGCU_Vaki\\2013-11-21_Density2\\Density2_20131121-104854.lsc",
                     "F:\\Larcos\\ScannerFiles\\2013-11-20_FGCU_Vaki\\2013-11-21_HighDensity4\\HighDensity4_20131121-114413.lsc",
                     "F:\\Larcos\\ScannerFiles\\2013-11-20_FGCU_Vaki\\2013-11-21_HighDensity\\highdens_20131121-104222.lsc",
                     "F:\\Larcos\\ScannerFiles\\2013-11-20_FGCU_Vaki\\2013-11-21_HighDensity3\\HighDensity3_20131121-113122.lsc",
                     "F:\\Larcos\\ScannerFiles\\2013-11-20_FGCU_Vaki\\2013-11-21_Density2\\Density2_20131121-104854.lsc",
                     "F:\\Larcos\\ScannerFiles\\2013-11-20_FGCU_Vaki\\2013-11-21_HighDensity4\\HighDensity4_20131121-114413.lsc",
                     "F:\\Larcos\\ScannerFiles\\2013-11-20_FGCU_Vaki\\2013-11-21_HighDensity3\\HighDensity3_20131121-113122.lsc",
                     "F:\\Larcos\\ScannerFiles\\2013-11-20_FGCU_Vaki\\2013-11-21_HighDensity\\highdens_20131121-104222.lsc",
                     "F:\\Larcos\\ScannerFiles\\2013-11-20_FGCU_Vaki\\2013-11-21_sh-gsh\\sh-gsh_20131121-094205.lsc",
                     "F:\\Larcos\\ScannerFiles\\2013-11-20_FGCU_Vaki\\2013-11-21_Density2\\Density2_20131121-104854.lsc",
                     "F:\\Larcos\\ScannerFiles\\2013-11-20_FGCU_Vaki\\2013-11-21_HighDensity\\highdens_20131121-104222.lsc",
                     "F:\\Larcos\\ScannerFiles\\2013-11-20_FGCU_Vaki\\2013-11-21_HighDensity3\\HighDensity3_20131121-113122.lsc",
                     "F:\\Larcos\\ScannerFiles\\2013-11-20_FGCU_Vaki\\2013-11-21_Density2\\Density2_20131121-104854.lsc",
                     "F:\\Larcos\\ScannerFiles\\2013-11-20_FGCU_Vaki\\2013-11-21_HighDensity4\\HighDensity4_20131121-114413.lsc",
                     "F:\\Larcos\\ScannerFiles\\2013-11-20_FGCU_Vaki\\2013-11-21_Density2\\Density2_20131121-104854.lsc",
                     "F:\\Larcos\\ScannerFiles\\2013-11-20_FGCU_Vaki\\2013-11-21_HighDensity3\\HighDensity3_20131121-113122.lsc",
                     "F:\\Larcos\\ScannerFiles\\2013-11-20_FGCU_Vaki\\2013-11-21_HighDensity\\highdens_20131121-104222.lsc",
                     "F:\\Larcos\\ScannerFiles\\2013-11-20_FGCU_Vaki\\2013-11-21_HighDensity4\\HighDensity4_20131121-114413.lsc",
                     "F:\\Larcos\\ScannerFiles\\2013-11-20_FGCU_Vaki\\2013-11-21_Density2\\Density2_20131121-104854.lsc",
                     "F:\\Larcos\\ScannerFiles\\2013-11-20_FGCU_Vaki\\2013-11-21_HighDensity3\\HighDensity3_20131121-113122.lsc",
                     "F:\\Larcos\\ScannerFiles\\2013-11-20_FGCU_Vaki\\2013-11-21_sh-gsh\\sh-gsh_20131121-094205.lsc",
                     "F:\\Larcos\\ScannerFiles\\2013-11-20_FGCU_Vaki\\2013-11-21_HighDensity\\highdens_20131121-104222.lsc",
                     "F:\\Larcos\\ScannerFiles\\2013-11-20_FGCU_Vaki\\2013-11-21_HighDensity4\\HighDensity4_20131121-114413.lsc",
                     "F:\\Larcos\\ScannerFiles\\2013-11-20_FGCU_Vaki\\2013-11-21_Density2\\Density2_20131121-104854.lsc",
                     "F:\\Larcos\\ScannerFiles\\2013-11-20_FGCU_Vaki\\2013-11-21_HighDensity\\highdens_20131121-104222.lsc",
                     "F:\\Larcos\\ScannerFiles\\2013-11-20_FGCU_Vaki\\2013-11-21_Density2\\Density2_20131121-104854.lsc",
                      NULL
                    };


  KKStr  outFileName = "F:\\Larcos\\ScannerFiles\\2013-11-20_FGCU_Vaki\\2013-11-21-SIS-PL-HighdensityLongRun\\2013-11-21-SIS-PL-HighdensityLongRun.lsc";
*/

/*
  char*  scannerFiles[] = {
                     "D:\\Larcos\\ScannerFiles\\2012-02-03_TX\\SCS_20120203101106_TrackB3_Run1.lsc",
                     "D:\\Larcos\\ScannerFiles\\2012-02-03_TX\\SCS_20120203104424_TrackB3_Run2.lsc",
                     "D:\\Larcos\\ScannerFiles\\2012-02-03_TX\\SCS_20120203104424_TrackB3_Run2_000.lsc",
                     "D:\\Larcos\\ScannerFiles\\2012-02-03_TX\\SCS_20120203104424_TrackB3_Run2_001.lsc",
                     "D:\\Larcos\\ScannerFiles\\2012-02-03_TX\\SCS_20120203115957_TrackC3_Run2.lsc",
                     "D:\\Larcos\\ScannerFiles\\2012-02-03_TX\\SCS_20120203121522_TrackC3_Run3.lsc",
                     "D:\\Larcos\\ScannerFiles\\2012-02-03_TX\\SCS_20120203141137_TrackD3_Run1.lsc",
                     "D:\\Larcos\\ScannerFiles\\2012-02-03_TX\\SCS_20120203142620_TrackD3_Run2.lsc",
                     "D:\\Larcos\\ScannerFiles\\2012-02-03_TX\\SCS_20120203151240_TrackD3_Run3.lsc",
                     "D:\\Larcos\\ScannerFiles\\2012-02-03_TX\\SCS_20120203161055_TrackA3_Run1.lsc",
                     "D:\\Larcos\\ScannerFiles\\2012-02-03_TX\\SCS_20120203162615_TrackA3_Run2.lsc",
                     "D:\\Larcos\\ScannerFiles\\2012-02-03_TX\\SCS_20120203164608_TrackA3_Run3.lsc",
                     "D:\\Larcos\\ScannerFiles\\2012-02-03_TX\\SCS_20120203170302_TrackA3_Run4.lsc",
                     NULL
                    };


  KKStr  outFileName = "C:\\Larcos\\ScannerFiles\\2014-02-09_WAS-2014\\TAM_LargeShrimp\\TAM-12G-Shrimp.lsc";
*/


  char*  scannerFiles[] = {
                     "C:\\Larcos\\ScannerFiles\\TAM-12G-Shrimp-Dense.lsc",
                     "C:\\Larcos\\ScannerFiles\\TAM-12G-Shrimp-Dense.lsc",
                     "C:\\Larcos\\ScannerFiles\\TAM-12G-Shrimp-Dense.lsc",
                     "C:\\Larcos\\ScannerFiles\\TAM-12G-Shrimp-Dense.lsc",
                     "C:\\Larcos\\ScannerFiles\\TAM-12G-Shrimp-Dense.lsc",
                     "C:\\Larcos\\ScannerFiles\\TAM-12G-Shrimp-Dense.lsc",
//                     "F:\\Larcos\\ScannerFiles\\2014-02-09_WAS-2014\\TAM-12G-Shrimp.lsc",
//                     "F:\\Larcos\\ScannerFiles\\2014-02-09_WAS-2014\\TAM-12G-Shrimp.lsc",
//                     "F:\\Larcos\\ScannerFiles\\2014-02-09_WAS-2014\\TAM-12G-Shrimp.lsc",
                     NULL
                    };


  KKStr  outFileName = "F:\\Larcos\\ScannerFiles\\2014-02-09_WAS-2014\\TAM-12G-Shrimp-Dense-x6.lsc";




  ScannerFilePtr  out = ScannerFile::CreateScannerFileForOutput (outFileName, ScannerFile::sf4BitEncoded, 2048, 480, runLog);
  out->AddHeaderField ("DestScannerFileFormat","4BitEncoded");
  if  (!out)
  {
    cerr << endl << endl << "***ERROR***  opening output file." << endl;
    exit (-1);
  }

  bool  firstScannerFile = true;

  uchar  lineBuff[20480];
  kkuint32  lineBuffSize = 20480;
  kkuint32  lineSize = 0;
  kkuint32*  colCount = new kkuint32[lineBuffSize];
  kkuint32  pixelsInLine = 0;


  int  nameIdx = 0;
  while  (scannerFiles[nameIdx] != NULL)
  {
    KKStr  scannerFileName = scannerFiles[nameIdx];

    KKStr  scannerFileRootName = osGetRootName (scannerFileName);

    ScannerFilePtr  in = ScannerFile::CreateScannerFile (scannerFiles[nameIdx], runLog);
    if  (!in)
    {
      cerr << endl << "***ERROR***    Could not open ScannerFile: " << scannerFiles[nameIdx] << endl;
    }
    else
    {
      cout << "Starting Scanner File: " << scannerFiles[nameIdx] << endl;
      if  (firstScannerFile)
      {
        KKStr  cameraLineStr = in->GetValue ("CameraLine");
        KKStr  modelStr      = in->GetValue ("Model");
        KKStr  macAddress    = in->GetValue ("MacAddress");
        KKStr  ipAddress     = in->GetValue ("IpAddress");

        out->AddHeaderField ("CameraLine", cameraLineStr);
        out->AddHeaderField ("CP:Model",      modelStr);
        out->AddHeaderField ("CP:MacAddress", macAddress);
        out->AddHeaderField ("CP:IpAddress",  ipAddress);

        OperatingParameters  op;
        op.UpdateFromScannerFile (in);

        op.RequestedScanRate (20000);
        op.ConnectedComponentDist (1);
        op.ErosionStructSize (2);
        op.FlatFieldEnabled (true);
        op.MinSizeThreshold (8000);
        op.FlowRateFactor (3.0f);

        op.AddToHeaderFields (out->HeaderFields ());

        SessionParameters  sp;
        sp.UpdateFromScannerFile (in);
        sp.TrainingModelName ("Shrimp_12G_Murkey");
        sp.SessionDescription ("TAM  Several runs combined;  \"2012-02-03_TX\", Tracks A3, B3, C3, and D3.");
        sp.DateTimeRecorded (DateTime (2012, 2, 3, (uchar)10, (uchar)13, (uchar)0));
        sp.ControlNum ("TAM10");
        sp.AddToHeaderFields (out->HeaderFields ());

        bool   successful = false;
        InstallationConfig  instalalationConfig ("D:\\Larcos\\Configurations\\Installations\\ModelA.cfg", successful, runLog);
        if  (successful)
          instalalationConfig.AddToHeaderFields (out->HeaderFields ());

        out->InitiateWritting ();
      }

      volatile bool  cancelFlag = false;
      in->BuildFrameOffsets (cancelFlag);
      vector<Region> regions = BuildRegionList  (in->StartStopPoints ());

      vector<Region>::iterator  idx = regions.begin ();
      while  (idx != regions.end ())
      {
        // Skip to start of region
        in->SkipToScanLine (idx->start);
        while  (in->LastScanLine () < idx->end)
        {
          in->GetNextLine (lineBuff, lineBuffSize, lineSize, colCount, pixelsInLine); 
          if  (in->Eof ())
            break;
          out->WriteScanLine (lineBuff, lineSize);
          if  ((in->NextScanLine () % 100000) == 0)
            cout << scannerFileRootName << "\t" << in->NextScanLine () << endl;
        }
        ++idx;
      }

      delete  in;
      in = NULL;

      firstScannerFile = false;
    }

    ++nameIdx;
  }

  delete  out;
  out = NULL;

}  /* CombineScannerFiles */





struct  FMEntry
{
  FMEntry (kkuint32  _scanLineNum,
           kkuint32  _counter
           ):
      scanLineNum (_scanLineNum),
      counter     (_counter)
      {}

  kkuint32  scanLineNum;
  kkuint32  counter;
};


typedef  vector<FMEntry>  FMEntryList;

typedef  FMEntryList*   FMEntryListPtr;


FMEntryListPtr   LoadFlowMeterData (const KKStr&  flowMeterDataFileName)
{
  ifstream  frd (flowMeterDataFileName.Str ());
  if  (!frd.is_open ())
    return NULL;

  FMEntryListPtr  data = new FMEntryList ();

  bool  eof = false;
  bool  eol = false;

  KKStr  line = KKB::osReadRestOfLine (frd, eof);
  while  (!eof)
  {
    kkuint32  scanLineNum = line.ExtractTokenUint ("\t");
    kkuint32  counter     = line.ExtractTokenUint ("\t");
    data->push_back (FMEntry (scanLineNum, counter));
    line = KKB::osReadRestOfLine (frd, eof);
  }

  frd.close ();

  return  data;
}  /* LoadFlowMeterData */




void  MergeInFlowRateData (const KKStr&  scannerFileName)
{
  RunLog  runLog;

  KKStr  scannerFileRootName = KKB::osGetRootName (scannerFileName);

  ScannerFilePtr  in =  ScannerFile::CreateScannerFile (scannerFileName, runLog);
  if  (!in)
  {
    runLog.Level (-1) << "MergeInFlowRateData   ***ERROR***  Could not open Scanner File: \"" << scannerFileName << "\"." << endl;
    return;
  }

  KKStr  flowRateFileName = KKB::osRemoveExtension (scannerFileName) + "_FlowMeterData.txt";

  FMEntryListPtr  flowRateData = LoadFlowMeterData (flowRateFileName);
  if  (!flowRateData)
  {
    runLog.Level (-1) << "MergeInFlowRateData    ***ERROR***   FlowMeterData file: \"" << flowRateFileName << "\"  does noy exist." << endl;
    return;
  }

  KKStr  outFileName = KKB::osRemoveExtension (scannerFileName) + "_WithFlowRateData.lsc";

  kkuint32  pixelsPerScanLine = in->PixelsPerScanLine ();


  KKLSC::ScannerHeaderFieldsPtr  headerFields = in->HeaderFields ();

  osDeleteFile (outFileName);

  ScannerFilePtr  out 
     = ScannerFile::CreateScannerFileForOutput (outFileName, 
                                                ScannerFile::sfSimple,
                                                pixelsPerScanLine,
                                                in->FrameHeight (),
                                                runLog
                                               );

  out->AddHeaderFields (headerFields);

  out->InitiateWritting ();

  kkint32  flowMeterCount = 0;

  kkuint32  lineLenMax = pixelsPerScanLine + 100;
  kkuint32  lineSize = 0;

  uchar*  lineBuff = new uchar[lineLenMax];

  kkuint32*  colCount = new kkuint32[lineLenMax];

  kkuint32  pixelsInRow = 0;  // Number of forground pixels in scanLine.
  
  in->GetNextLine (lineBuff, lineLenMax, lineSize, colCount, pixelsInRow);
  kkuint32  scanLineNum = 0;

  kkuint32  curCounterIdx = 0;
  kkuint32  curCounterScanLine  = (*flowRateData)[curCounterIdx].scanLineNum;
  kkuint32  curCounter          = (*flowRateData)[curCounterIdx].counter;
  kkuint32  nextCounterScanLine = (*flowRateData)[curCounterIdx + 1].scanLineNum;

  while  (!in->Eof ())
  {
    if  ((scanLineNum % 100000) == 0)
    {
      cout << scannerFileRootName << "\t" << StrFormatInt (scanLineNum, "ZZZ,ZZZ,ZZ0") << "\t" << curCounter << endl;
    }

    while  ((curCounterIdx < (flowRateData->size () - 1))  &&  (nextCounterScanLine < scanLineNum))
    {
      ++curCounterIdx;
      curCounterScanLine  = (*flowRateData)[curCounterIdx].scanLineNum;
      curCounter          = (*flowRateData)[curCounterIdx].counter;
      if  (curCounterIdx < (flowRateData->size () - 1))
        nextCounterScanLine = (*flowRateData)[curCounterIdx + 1].scanLineNum;
      else
        nextCounterScanLine = 999999999;
    }

    WordFormat32Bits  dataWord (curCounter);
    lineBuff[0] = dataWord.fourBytes.byte3;
    lineBuff[1] = dataWord.fourBytes.byte2;
    lineBuff[2] = dataWord.fourBytes.byte1;
    lineBuff[3] = dataWord.fourBytes.byte0;

    out->WriteScanLine (lineBuff, lineSize);

    in->GetNextLine (lineBuff, lineLenMax, lineSize, colCount, pixelsInRow);
    ++scanLineNum;
  }

  out->Close ();

  delete  out;           out          = NULL;
  delete  colCount;      colCount     = NULL;
  delete  lineBuff;      lineBuff     = NULL;
  delete  flowRateData;  flowRateData = NULL;
  delete  in;            in = NULL;

}  /* MergeInFlowRateData */



void  MergeInFlowRateDataForAllFiles ()
{
  MergeInFlowRateData ("E:\\Larcos\\ScannerFiles\\2014-03-02_Larcos_FM\\Larcos_FM_20140302-144248.lsc");
  MergeInFlowRateData ("E:\\Larcos\\ScannerFiles\\2014-03-02_Larcos_FM_001\\Larcos_FM_001_20140302-144525.lsc");
  MergeInFlowRateData ("E:\\Larcos\\ScannerFiles\\2014-03-02_Larcos_FM_002\\Larcos_FM_002_20140302-144942.lsc");
  MergeInFlowRateData ("E:\\Larcos\\ScannerFiles\\2014-03-02_Larcos_FM_003\\Larcos_FM_003_20140302-145652.lsc");
  MergeInFlowRateData ("E:\\Larcos\\ScannerFiles\\2014-03-02_Larcos_FM_004\\Larcos_FM_004_20140302-150009.lsc");
  MergeInFlowRateData ("E:\\Larcos\\ScannerFiles\\2014-03-02_Larcos_FM_005\\Larcos_FM_005_20140302-150241.lsc");
}  /* MergeInFlowRateDataForAllFiles */













void  main (int  argc,  char** argv)
{
  {
    MsgQueuePtr  msgQueue = new MsgQueue ("Test1");
    MsgQueuePtr  msgQueue2 = new MsgQueue ("Test2");

    LarcosCounterManager*  manager = new LarcosCounterManager (msgQueue, msgQueue2, 4);
  }

  {
    MergeInFlowRateDataForAllFiles ();
    exit (-1);
  }


  {
    CombineScannerFiles ();
    exit (-1);
  }


  {
    ofstream o ("c:\\Temp\\ScannerFileAttributes2.txt");

    o << "Root"        << "\t"
      << "File"        << "\t"
      << "Date-Time"   << "\t"
      << "Size"        << "\t"
      << "Scan"        << "\t"
      << ""            << "\t"
      << "Format"      << "\t"
      << "Bit"         << "\t"
      << "Sensitivity" << "\t"
      << "Analog"      << "\t"
      << "Digital"     << "\t"
      << "Scan"        << "\t"
      << "Flad"        << "\t"
      << "Playing"     << "\t"
      << ""
      << endl;

    o << "Dir"         << "\t"
      << "Name"        << "\t"
      << "Recorded"    << "\t"
      << "Bytes"       << "\t"
      << "Lines"       << "\t"
      << "Seconds"     << "\t"
      << "Format"      << "\t"
      << "Depth"       << "\t"
      << "Mode"        << "\t"
      << "Gain"        << "\t"
      << "Gain"        << "\t"
      << "Rate"        << "\t"
      << "Field"       << "\t"
      << "Back"        << "\t"
      << "Description"
      << endl;

    ScannerFileCharacteristics_ProcessDirTree ("F:\\Larcos\\ScannerFiles\\2013-07-02_FGCU_ModelT", "", o);
    ScannerFileCharacteristics_ProcessDirTree ("F:\\Larcos\\ScannerFiles\\2013-07-03_FGCU_ModelT", "", o);
    ScannerFileCharacteristics_ProcessDirTree ("F:\\Larcos\\ScannerFiles\\2013-07-05_FGCU_ModelT", "", o);
    //ScannerFileCharacteristics_ProcessDirTree ("E:\\Larcos\\ScannerFiles", "", o);
    o.close ();
    exit (-1);
  }

  HistogramSize app (argc, argv);
  app.InitalizeApplication (argc, argv);
  app.ProcessDirectory ();
}

