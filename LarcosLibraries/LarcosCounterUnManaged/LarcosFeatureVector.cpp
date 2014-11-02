#include "FirstIncludes.h"

#include <ctype.h>
#include <math.h>
#include <time.h>

#include <string>
#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <string.h>
#include <typeinfo>

#include "MemoryDebug.h"

using namespace  std;


#include "KKBaseTypes.h"
#include "Blob.h"
#include "BMPImage.h"
#include "ContourFollower.h"
#include "ConvexHull.h"
#include "DateTime.h"
#include "ImageIO.h"
#include "KKException.h"
#include "OSservices.h"
#include "Raster.h"
#include "RunLog.h"
#include "KKStr.h"
using namespace  KKB;


#include "FeatureNumList.h"
#include "FeatureFileIO.h"
#include "FeatureFileIOKK.h"
#include "MLClass.h"
#include "ImageDirTree.h"
using namespace  KKMachineLearning;


#include "LarcosFeatureVector.h"
using namespace  LarcosCounterUnManaged;
                      




const  kkint32  LarcosFeatureVector::SizeThreshold = 10000;  /**< Size of example in num of pixels before we decide to reduce the
                                                     * example to improve feature calculation.
                                                     */


							
kkint16  LarcosFeatureVector::maxNumOfFeatures      = 56;

kkint16  LarcosFeatureVector::SizeIndex             = 0;
kkint16  LarcosFeatureVector::Moment1Index          = 1;
kkint16  LarcosFeatureVector::Moment2Index          = 2;
kkint16  LarcosFeatureVector::Moment3Index          = 3;
kkint16  LarcosFeatureVector::Moment4Index          = 4;
kkint16  LarcosFeatureVector::Moment5Index          = 5;
kkint16  LarcosFeatureVector::Moment6Index          = 6;
kkint16  LarcosFeatureVector::Moment7Index          = 7;
kkint16  LarcosFeatureVector::Moment8Index          = 8;

kkint16  LarcosFeatureVector::EdgeSizeIndex         = 9;
kkint16  LarcosFeatureVector::EdgeMoment1Index      = 10;
kkint16  LarcosFeatureVector::EdgeMoment2Index      = 11;	
kkint16  LarcosFeatureVector::EdgeMoment3Index      = 12;
kkint16  LarcosFeatureVector::EdgeMoment4Index      = 13;
kkint16  LarcosFeatureVector::EdgeMoment5Index      = 14;
kkint16  LarcosFeatureVector::EdgeMoment6Index      = 15;
kkint16  LarcosFeatureVector::EdgeMoment7Index      = 16;
kkint16  LarcosFeatureVector::EdgeMoment8Index      = 17;

kkint16  LarcosFeatureVector::TransparancyConvexHullIndex = 18;
kkint16  LarcosFeatureVector::TransparancyPixelCountIndex = 19;
kkint16  LarcosFeatureVector::TransparancyOpen3Index      = 20;
kkint16  LarcosFeatureVector::TransparancyOpen5Index      = 21;
kkint16  LarcosFeatureVector::TransparancyOpen7Index      = 22;
kkint16  LarcosFeatureVector::TransparancyOpen9Index      = 23;
kkint16  LarcosFeatureVector::TransparancyClose3Index     = 24;
kkint16  LarcosFeatureVector::TransparancyClose5Index     = 25;
kkint16  LarcosFeatureVector::TransparancyClose7Index     = 26;

kkint16  LarcosFeatureVector::ConvexAreaIndex       = 27;
kkint16  LarcosFeatureVector::TransparancySizeIndex = 28;
kkint16  LarcosFeatureVector::TransparancyWtdIndex  = 29;
kkint16  LarcosFeatureVector::WeighedMoment0Index   = 30;
kkint16  LarcosFeatureVector::WeighedMoment1Index   = 31;
kkint16  LarcosFeatureVector::WeighedMoment2Index   = 32;
kkint16  LarcosFeatureVector::WeighedMoment3Index   = 33;
kkint16  LarcosFeatureVector::WeighedMoment4Index   = 34;
kkint16  LarcosFeatureVector::WeighedMoment5Index   = 35;
kkint16  LarcosFeatureVector::WeighedMoment6Index   = 36;
kkint16  LarcosFeatureVector::WeighedMoment7Index   = 37;
kkint16  LarcosFeatureVector::WeighedMoment8Index   = 38;

kkint16  LarcosFeatureVector::IntensityHist1Index   = 39;
kkint16  LarcosFeatureVector::IntensityHist2Index   = 40;
kkint16  LarcosFeatureVector::IntensityHist3Index   = 41;
kkint16  LarcosFeatureVector::IntensityHist4Index   = 42;
kkint16  LarcosFeatureVector::IntensityHist5Index   = 43;
kkint16  LarcosFeatureVector::IntensityHist6Index   = 44;
kkint16  LarcosFeatureVector::IntensityHist7Index   = 45;

kkint16  LarcosFeatureVector::DarkSpotCount0        = 46;
kkint16  LarcosFeatureVector::DarkSpotCount1        = 47;
kkint16  LarcosFeatureVector::DarkSpotCount2        = 48;
kkint16  LarcosFeatureVector::DarkSpotCount3        = 49;
kkint16  LarcosFeatureVector::DarkSpotCount4        = 50;
kkint16  LarcosFeatureVector::DarkSpotCount5        = 51;
kkint16  LarcosFeatureVector::DarkSpotCount6        = 52;
kkint16  LarcosFeatureVector::DarkSpotCount7        = 53;
kkint16  LarcosFeatureVector::DarkSpotCount8        = 54;
kkint16  LarcosFeatureVector::DarkSpotCount9        = 55;


const  char*  LarcosFeatureVector::FeatureNames[] =
{
  "Size",                    //  0
  "Moment1",                 //  1 
  "Moment2",                 //  2
  "Moment3",                 //  3
  "Moment4",                 //  4
  "Moment5",                 //  5
  "Moment6",                 //  6
  "Moment7",                 //  7
  "Moment8",                 //  8
  
  "EdgeSize",                //  9
  "EdgeMoment1",             // 10
  "EdgeMoment2",             // 11
  "EdgeMoment3",             // 12
  "EdgeMoment4",             // 13
  "EdgeMoment5",             // 14
  "EdgeMoment6",             // 15
  "EdgeMoment7",             // 16
  "EdgeMoment8",             // 17

  "TransparancyConvexHull",  // 18
  "TransparancyPixelCount",  // 19
  
  "TransparancyOpen3",       // 20    Granulometric features
  "TransparancyOpen5",       // 21
  "TransparancyOpen7",       // 22
  "TransparancyOpen9",       // 23
  "TransparancyClose3",      // 24
  "TransparancyClose5",      // 25
  "TransparancyClose7",      // 26

  "ConvexArea",              // 27
  "TransparancySize",        // 28
  "TransparancyWtd",         // 29

  "WeighedMoment0",          // 30
  "WeighedMoment1",          // 31
  "WeighedMoment2",          // 32
  "WeighedMoment3",          // 33
  "WeighedMoment4",          // 34
  "WeighedMoment5",          // 35
  "WeighedMoment6",          // 36
  "WeighedMoment7",          // 37
  "WeighedMoment8",          // 38

  "IntensityHist1",          // 39
  "IntensityHist2",          // 40
  "IntensityHist3",          // 41 
  "IntensityHist4",          // 42
  "IntensityHist5",          // 43
  "IntensityHist6",          // 44
  "IntensityHist7",          // 45
  "DarkSpotCount0",          // 46
  "DarkSpotCount1",          // 47
  "DarkSpotCount2",          // 48
  "DarkSpotCount3",          // 49
  "DarkSpotCount4",          // 50
  "DarkSpotCount5",          // 51
  "DarkSpotCount6",          // 52
  "DarkSpotCount7",          // 53
  "DarkSpotCount8",          // 54
  "DarkSpotCount9"           // 55
};

RunLog  LarcosFeatureVector::runLog;



KKStr  LarcosFeatureVector::FeatureName (kkint32  fieldNum)
{
  if  ((fieldNum < 0)  ||  (fieldNum >= MaxNumOfFeatures ()))
  {
    // This should never happen,  if it does I want to know right away and fix
    // the problem, will probably be indicative a greater more involved 
    // problem.

    cerr << endl
         << "LarcosFeatureVector::::FeatureName   ***ERROR***  FeatureNum[" << fieldNum << "] is out of bounds." << endl
         << endl;
    return  KKStr::EmptyStr ();
  }
  
  return  FeatureNames [fieldNum];
}  /* FeatureName */



FileDescPtr  LarcosFeatureVector::postLarvaeFeaturesFileDesc = NULL;


FileDescPtr  LarcosFeatureVector::PostLarvaeFeaturesFileDesc ()
{
  if  (postLarvaeFeaturesFileDesc)
    return  postLarvaeFeaturesFileDesc;

  bool  alreadyExists = false;
  postLarvaeFeaturesFileDesc = new FileDesc ();
  for  (kkint32 fieldNum = 0;  fieldNum < MaxNumOfFeatures ();  fieldNum++)
  {
    postLarvaeFeaturesFileDesc->AddAAttribute (FeatureName (fieldNum), NumericAttribute, alreadyExists);
  }

  // Lets make sure that one was already created by opening up a data file.
  postLarvaeFeaturesFileDesc = FileDesc::GetExistingFileDesc (postLarvaeFeaturesFileDesc);

  return  postLarvaeFeaturesFileDesc;
}  /* PostLarvaeFeaturesFileDesc () */



LarcosFeatureVector::LarcosFeatureVector (kkint32  _numOfFeatures):
       FeatureVector (_numOfFeatures),

        centroidCol     (-1),
        centroidRow     (-1),
        numOfEdgePixels (-1)
{
}



LarcosFeatureVector::LarcosFeatureVector (const LarcosFeatureVector&  _image):
  FeatureVector (_image),

  centroidCol     (_image.centroidCol),
  centroidRow     (_image.centroidRow),
  numOfEdgePixels (_image.numOfEdgePixels)

{
}




LarcosFeatureVector::LarcosFeatureVector (const BmpImage&   _image,
                            const MLClassPtr  _mlClass,
                            RasterListPtr     _intermediateImages
                           ):
  FeatureVector (MaxNumOfFeatures ()),
  centroidCol      (-1),
  centroidRow      (-1),
  numOfEdgePixels  (-1)
{
  MLClass       (_mlClass);
  ImageFileName (_image.FileName ());

  RasterPtr raster = new Raster (_image);
  CalcFeatures (*raster, _intermediateImages);
  delete  raster;
}




LarcosFeatureVector::LarcosFeatureVector (      Raster&     _raster,
                            const MLClassPtr  _mlClass,
                            RasterListPtr     _intermediateImages
                           ):
  FeatureVector (MaxNumOfFeatures ()),
  centroidCol      (-1),
  centroidRow      (-1),
  numOfEdgePixels  (-1)
{
  MLClass    (_mlClass);
  ImageFileName (_raster.FileName ());

  CalcFeatures  (_raster, _intermediateImages);
}



LarcosFeatureVector::LarcosFeatureVector (KKStr          _fileName,
                            MLClassPtr  _mlClass,
                            bool&          _successfull,
                            RasterListPtr  _intermediateImages
                           ):

  FeatureVector (MaxNumOfFeatures ()),
  centroidCol      (-1),
  centroidRow      (-1),
  numOfEdgePixels  (-1)

{
  MLClass       (_mlClass);
  ImageFileName    (_fileName);

  RasterPtr  raster = ReadImage (_fileName);
  if  (raster == NULL)
  {
    _successfull = false;
    for  (kkint32 x = 0; x < MaxNumOfFeatures (); x++)
      featureData[x] = 0;
    cerr  << "LarcosFeatureVector::LarcosFeatureVector  ***ERROR***, Opening File[" << _fileName << "]." << endl;
    return;
  }

  if  (raster->Color ())
  {
    RasterPtr  temp = raster->CreateGrayScale ();
    temp->ReverseImage ();
    delete  raster;
    raster = temp;
    temp = NULL;
  }

  _successfull = true;
  //raster->ConnectedComponent ();
  CalcFeatures (*raster, _intermediateImages);
  delete  raster;
}



LarcosFeatureVector::LarcosFeatureVector (const FeatureVector&  featureVector):
   FeatureVector    (featureVector),
   centroidCol      (-1),
   centroidRow      (-1),
   numOfEdgePixels  (-1)

{
  //if  (strcmp (featureVector.UnderlyingClass (), "LarcosFeatureVector") == 0)
  if  (typeid (featureVector) == typeid(LarcosFeatureVector))
  {
    // The underlying class is another LarcosFeatureVector object.
    const LarcosFeatureVector&  example = dynamic_cast<const LarcosFeatureVector&>(featureVector);

    centroidCol      = example.CentroidCol     ();
    centroidRow      = example.CentroidRow     ();
    numOfEdgePixels  = example.NumOfEdgePixels ();
  }
}



LarcosFeatureVector::~LarcosFeatureVector ()
{
}






void  LarcosFeatureVector::SaveIntermediateImage (Raster&        raster, 
                                           const KKStr&   desc,
                                           RasterListPtr  intermediateImages
                                          )
{
  if  (!intermediateImages)
    return;

  RasterPtr  newRaster = NULL;

  kkint32  largestDim = Max (raster.Height (), raster.Width ());
  if  (largestDim < 300)
  {
    newRaster = new Raster (raster);
  }
  else
  {
    kkint32  reductionMultiple = 2;
    while  ((largestDim / reductionMultiple) > 300)
     reductionMultiple++;
    newRaster = raster.ReduceByEvenMultiple (reductionMultiple);
  }

  newRaster->FileName (desc);
  intermediateImages->PushOnBack (newRaster);
}




void  LarcosFeatureVector::ParseImageFileName (const KKStr&  fullFileName, 
                                        KKStr&        scannerFileName,
                                        kkuint32&     scanLineNum,
                                        kkuint32&     scanCol
                                       )
{
  scannerFileName = "";
  scanLineNum    = 0;
  scanCol        = 0;
  
  KKStr  rootName = osGetRootName (fullFileName);
  if  (rootName.Empty ())
    return;
  
  kkint32  x = rootName.LocateLastOccurrence ('_');
  if  (x > 0)
  {
    KKStr  colStr = rootName.SubStrPart (x + 1);
    KKStr  temp = rootName.SubStrPart (0, x - 1);
    x = temp.LocateLastOccurrence ('_');
    if  (x > 0)
    {
      scannerFileName = temp.SubStrPart (0, x - 1);
      KKStr  rowStr = temp.SubStrPart (x + 1);
      scanCol     = atoi (colStr.Str ());
      scanLineNum = atoi (rowStr.Str ());
    }
  }
}  /* ParseImageFileName */





//#define  DEBUB_CalcFeatures

void  LarcosFeatureVector::CalcFeatures (Raster&        srcRaster,
                                  RasterListPtr  intermediateImages
                                 )
{
  RunLog  log;

  Version (CurrentFeatureFileVersionNum);
  kkuint32  intensityHistBuckets[8];

  bool  weOwnRaster = false;
  RasterPtr  reversedImage = NULL;

  RasterPtr  origSizeImage = &srcRaster;

  if  (srcRaster.BackgroundPixelValue () > srcRaster.ForegroundPixelValue ())
  {
    weOwnRaster = true;
    reversedImage = srcRaster.ReversedImage ();
    origSizeImage = reversedImage;
  }

  Raster* raster = (weOwnRaster ? reversedImage : &srcRaster);

  kkint32 areaBeforeReduction = 0;
  float  weighedSizeBeforeReduction = 0.0f;
 
  raster->CalcAreaAndIntensityFeatures (areaBeforeReduction, 
                                        weighedSizeBeforeReduction,
                                        intensityHistBuckets
                                       );

  kkint32  reductionFactor = 1;
  kkint32  reductionFactorSquared = 1;

  if  (areaBeforeReduction > SizeThreshold)
  {
    kkint32  reducedMinDim = Min (raster->Height (), raster->Width ());

    kkint32  reducedSize = areaBeforeReduction;
    reductionFactor = 1;
    while  ((reducedSize > SizeThreshold)  &&  (reducedMinDim > 20))
    {
      reductionFactor = reductionFactor + 1;
      reductionFactorSquared = reductionFactor * reductionFactor;
      reducedSize = areaBeforeReduction / reductionFactorSquared;
      reducedMinDim = Min (raster->Height (), raster->Width ()) / reductionFactor;
    }

    RasterPtr  reducedRaster = raster->ReduceByEvenMultiple (reductionFactor);

    if  (intermediateImages)
      SaveIntermediateImage (*reducedRaster, "Reduced_" + StrFormatInt (reductionFactor, "ZZZZZZ0"), intermediateImages);

    if  (weOwnRaster)
    {
      delete  raster;
      raster = NULL;
    }

    raster = reducedRaster;
    weOwnRaster = true;
  }

  if  (areaBeforeReduction < 20)
  {
    for  (kkint32 tp = 0; tp < numOfFeatures; tp++)
      featureData[tp] = 9999999;
    
    if  (weOwnRaster)
    {
      delete  raster;
      raster = NULL;
    }
    return;
  }
  
  KKStr  driveLetter, path, root, extension;

  float  convexf = 0.0;

  osParseFileSpec (ImageFileName (), driveLetter, path, root, extension);
  KKStr  baseName = "c:\\Temp\\TestImages2\\" + root + "\\" + root;

  if  (intermediateImages)
    SaveIntermediateImage (*raster, "Original_" + StrFormatInt (areaBeforeReduction, "ZZZZZZ0"), intermediateImages);

  //raster->ConnectedComponent ();
  //SaveIntermediateImage (*raster, baseName, "ConnectedComponent", saveImage);

  float  momentf[9];
  raster->CentralMoments (momentf);

  float  momentfWeighted[9];
  raster->CentralMomentsWeighted (momentfWeighted);

  kkint32  height = raster->Height ();
  kkint32  width  = raster->Width  ();

  RasterPtr  wr1 = new Raster (height, width);
  RasterPtr  wr2 = new Raster (height, width);

  float  edgeMomentf[9];

  raster->Dialation (wr1);
  wr1->Dialation (wr2);
  wr2->FillHole (wr1);

  if  (false)
  {
    KKB::osCreateDirectoryPath ("c:\\Temp\\TestImages2\\" + root);
    SaveImage (*raster, baseName + "_01-Orig.bmp");
    SaveImage (*wr2,    baseName + "_02-HoleFilled.bmp");

    Raster  r1 (height, width, true);

    for  (int r = 0;  r < height;  ++r)
    {
      uchar**  mask = wr1->Green ();
      for  (int c = 0;  c < width;  ++c)
      {
        if  (mask[r][c] == 0)
          r1.SetPixelValue (r, c, PixelValue::Red);

        else if  (mask[r][c] == 1)
          r1.SetPixelValue (r, c, PixelValue::Green);

        else if  (mask[r][c] == 255)
          r1.SetPixelValue (r, c, PixelValue::Blue);

        else
          r1.SetPixelValue (r, c, PixelValue::Black);
      }
    }

    SaveImage (r1, baseName + "_02-Mask.bmp");
  }

  wr1->Erosion (wr2);
  wr2->Edge (wr1);
  wr1->CentralMoments (edgeMomentf);
  if  (intermediateImages)
  {
    kkint32  numEdgePixelsFound = (kkint32)(edgeMomentf[0]);
    SaveIntermediateImage (*wr2, "Edge_Image_" + StrFormatInt (numEdgePixelsFound, "ZZZZ0"), intermediateImages);
  }

  kkint32 area = (kkint32)(momentf[0] + 0.5f);  // Moment-0 is the same as the number of forground pixels in example.
  {
    ConvexHullPtr  ch = new ConvexHull ();
    ch->Filter (*raster, wr1);
    convexf = (float)ch->ConvexArea ();

    if  (intermediateImages)
    {
      KKStr convexImageFileName = "ConvexHull_" +
                                   StrFormatInt ((kkint32)convexf, "ZZZZZ0");
      SaveIntermediateImage (*wr1, convexImageFileName, intermediateImages);
    }

    //delete  convexImage;
    //convexImage = NULL;
    delete  ch;
    ch = NULL;
  }

  raster->Erosion (wr1);
  wr1->Dialation (wr2);
  float  areaOpen3 = (float)(wr2->ForegroundPixelCount());
  if  (intermediateImages)
    SaveIntermediateImage (*wr2, "Opening3_" + StrFormatInt ((kkint32)areaOpen3, "ZZZZZZ0"), intermediateImages);


  raster->Erosion (wr1, KKB::SQUARE5);
  wr1->Dialation (wr2, KKB::SQUARE5);
  float  areaOpen5 = (float)(wr2->ForegroundPixelCount ());
  if  (intermediateImages)
    SaveIntermediateImage (*wr2, "Opening5_" + StrFormatInt ((kkint32)areaOpen5, "ZZZZZZ0"), intermediateImages);

  raster->Erosion (wr1, KKB::SQUARE7);
  wr1->Dialation (wr2, KKB::SQUARE7);
  float  areaOpen7 = (float)(wr2->ForegroundPixelCount ());
  if  (intermediateImages)
    SaveIntermediateImage (*wr2, "Opening7_" + StrFormatInt ((kkint32)areaOpen7, "ZZZZZZ0"), intermediateImages);
  
  wr2->Erosion (wr1, KKB::SQUARE9);
  wr1->Dialation (wr2, KKB::SQUARE9);
  float  areaOpen9 = (float)(wr2->ForegroundPixelCount ());
  if  (intermediateImages)
    SaveIntermediateImage (*wr2, "Opening9_" + StrFormatInt ((kkint32)areaOpen9, "ZZZZZZ0"), intermediateImages);

  raster->Dialation (wr1);
  wr1->Erosion (wr2);
  float  areaClose3 = (float)(wr2->ForegroundPixelCount ());
  if  (intermediateImages)
    SaveIntermediateImage (*wr2, "Close3_" + StrFormatInt ((kkint32)areaClose3, "ZZZZZZ0"), intermediateImages);

  wr2->FillHole (wr1);
  float  tranf = (float)(wr2->ForegroundPixelCount ());
  if  (intermediateImages)
    SaveIntermediateImage (*wr2, "FillHole_" + StrFormatInt ((kkint32)tranf, "ZZZZZZ0"), intermediateImages);

  raster->Dialation (wr1, KKB::SQUARE5);
  wr1->Erosion (wr2, KKB::SQUARE5);
  float  areaClose5 = (float)(wr2->ForegroundPixelCount ());
  if  (intermediateImages)
    SaveIntermediateImage (*wr2, "Close5_" + StrFormatInt ((kkint32)areaClose5, "ZZZZZZ0"), intermediateImages);
  
  raster->Dialation (wr1, KKB::SQUARE7);
  wr1->Erosion   (wr2, KKB::SQUARE7);
  float  areaClose7 = float (wr2->ForegroundPixelCount ());
  if  (intermediateImages)
    SaveIntermediateImage (*wr2, "Close7_" + StrFormatInt ((kkint32)areaClose7, "ZZZZZZ0"), intermediateImages);

  {
    featureData[SizeIndex]    = float (areaBeforeReduction);
    featureData[Moment1Index] = float (momentf[1]);
    featureData[Moment2Index] = float (momentf[2]);
    featureData[Moment3Index] = float (momentf[3]);
    featureData[Moment4Index] = float (momentf[4]);
    featureData[Moment5Index] = float (momentf[5]);
    featureData[Moment6Index] = float (momentf[6]);
    featureData[Moment7Index] = float (momentf[7]);
    featureData[Moment8Index] = float (momentf[8]);

    featureData[WeighedMoment0Index] = momentfWeighted[0];
    featureData[WeighedMoment1Index] = momentfWeighted[1];
    featureData[WeighedMoment2Index] = momentfWeighted[2];
    featureData[WeighedMoment3Index] = momentfWeighted[3];
    featureData[WeighedMoment4Index] = momentfWeighted[4];
    featureData[WeighedMoment5Index] = momentfWeighted[5];
    featureData[WeighedMoment6Index] = momentfWeighted[6];
    featureData[WeighedMoment7Index] = momentfWeighted[7];
    featureData[WeighedMoment8Index] = momentfWeighted[8];

    featureData[EdgeSizeIndex]    = (float)edgeMomentf[0];
    featureData[EdgeMoment1Index] = (float)edgeMomentf[1];
    featureData[EdgeMoment2Index] = (float)edgeMomentf[2];
    featureData[EdgeMoment3Index] = (float)edgeMomentf[3];
    featureData[EdgeMoment4Index] = (float)edgeMomentf[4];
    featureData[EdgeMoment5Index] = (float)edgeMomentf[5];
    featureData[EdgeMoment6Index] = (float)edgeMomentf[6];
    featureData[EdgeMoment7Index] = (float)edgeMomentf[7];
    featureData[EdgeMoment8Index] = (float)edgeMomentf[8];

    // Need to adjust for any reduction in Image Size
    featureData[SizeIndex]           = float (areaBeforeReduction);
    featureData[EdgeSizeIndex]       = float (edgeMomentf[0] * (float)reductionFactor);
    featureData[WeighedMoment0Index] = weighedSizeBeforeReduction;
  }


  if ((area > convexf)  &&  (convexf > 0))
     featureData[TransparancyConvexHullIndex] = 1.0;
  else 
     featureData[TransparancyConvexHullIndex] = (float)area / (float)convexf;

  featureData[TransparancyPixelCountIndex] = (float)area / (float)tranf;
  featureData[TransparancyOpen3Index]      = (float)(area - areaOpen3)  / (float)area;
  featureData[TransparancyOpen5Index]      = (float)(area - areaOpen5)  / (float)area;
  featureData[TransparancyOpen7Index]      = (float)(area - areaOpen7)  / (float)area;                
  featureData[TransparancyOpen9Index]      = (float)(area - areaOpen9)  / (float)area;
  featureData[TransparancyClose3Index]     = (float)(area - areaClose3) / (float)area;
  featureData[TransparancyClose5Index]     = (float)(area - areaClose5) / (float)area;
  featureData[TransparancyClose7Index]     = (float)(area - areaClose7) / (float)area;
 
  {
    // This part has to be done after 'CalcOrientationAndEigerRatio' is called.  That is where the example centroid is calced.
    centroidCol = raster->CentroidCol () * reductionFactor;
    centroidRow = raster->CentroidRow () * reductionFactor;
  }

  featureData[ConvexAreaIndex]       = convexf * reductionFactorSquared;
  featureData[TransparancySizeIndex] = (float)(momentf[0] / convexf);
  featureData[TransparancyWtdIndex]  = (float)(momentfWeighted[0] / convexf);

  float  areaD = float (areaBeforeReduction);

  featureData[IntensityHist1Index] = ((float)intensityHistBuckets[1] / areaD);
  featureData[IntensityHist2Index] = ((float)intensityHistBuckets[2] / areaD);
  featureData[IntensityHist3Index] = ((float)intensityHistBuckets[3] / areaD);
  featureData[IntensityHist4Index] = ((float)intensityHistBuckets[4] / areaD);
  featureData[IntensityHist5Index] = ((float)intensityHistBuckets[5] / areaD);
  featureData[IntensityHist6Index] = ((float)intensityHistBuckets[6] / areaD);
  featureData[IntensityHist7Index] = ((float)intensityHistBuckets[7] / areaD);


  {
    RasterPtr  darkSpots = NULL;

    if  (false)
    {
      darkSpots = raster->BinarizeByThreshold (200, 255);
    }

    else
    {
      darkSpots = origSizeImage->BinarizeByThreshold (200, 255);
    }

    darkSpots->Erosion (SQUARE3);
    darkSpots->Erosion (SQUARE3);

    KKB::BlobListPtr  blobs = darkSpots->ExtractBlobs (3);

    int  darkSpotFreq[10];
    int x;
    for  (x = 0;  x < 10;  ++x)
      darkSpotFreq[x] = 0;

    KKB::BlobList::iterator  idx;
    for  (idx = blobs->begin ();  idx != blobs->end ();  ++idx)
    {
      KKB::BlobPtr  b = idx->second;

      double l =  ::log ((double)(b->PixelCount ()));

      double logOfThree = ::log (3.0);

      int  index = (int)floor (l / logOfThree);
      index = Min (Max (0, index), 9);
      ++(darkSpotFreq[index]);
    }

    featureData[DarkSpotCount0] = (float)darkSpotFreq[0];
    featureData[DarkSpotCount1] = (float)darkSpotFreq[1];
    featureData[DarkSpotCount2] = (float)darkSpotFreq[2];
    featureData[DarkSpotCount3] = (float)darkSpotFreq[3];
    featureData[DarkSpotCount4] = (float)darkSpotFreq[4];
    featureData[DarkSpotCount5] = (float)darkSpotFreq[5];
    featureData[DarkSpotCount6] = (float)darkSpotFreq[6];
    featureData[DarkSpotCount7] = (float)darkSpotFreq[7];
    featureData[DarkSpotCount8] = (float)darkSpotFreq[8];
    featureData[DarkSpotCount9] = (float)darkSpotFreq[9];

    delete  blobs;      blobs     = NULL;
    delete  darkSpots;  darkSpots = NULL;
  }

  if  (intermediateImages)
  {
    RasterPtr  thinnedImage = raster->ThinContour ();
    SaveIntermediateImage  (*thinnedImage, "_Thinned", intermediateImages);
    delete  thinnedImage;  
    thinnedImage = NULL;
  }

  featureData[0] = (float)areaBeforeReduction;  // Incase the example was reduced.
  OrigSize ((float)areaBeforeReduction);

  Probability (-1.0f);

  delete  wr1;  wr1 = NULL;
  delete  wr2;  wr2 = NULL;

  if  (weOwnRaster)
    delete  raster;
}  /* CalcFeatures */






LarcosFeatureVectorList::LarcosFeatureVectorList (FileDescPtr  _fileDesc,
                                    bool         _owner,
                                    RunLog&      _log
                                   ):

    FeatureVectorList (_fileDesc, _owner, _log)
{

}




LarcosFeatureVectorList::LarcosFeatureVectorList (RunLog&       _log,
                                    MLClassPtr _mlClass,
                                    KKStr         _dirName,
                                    KKStr         _fileName
                                   ):

  FeatureVectorList (LarcosFeatureVector::PostLarvaeFeaturesFileDesc (), true, _log)

{
  FeatureExtraction (_dirName, _fileName, _mlClass);
}




LarcosFeatureVectorList::LarcosFeatureVectorList (const LarcosFeatureVectorList&  examples):
   FeatureVectorList (examples.FileDesc (), examples.Owner (), examples.log)
{
  const_iterator  idx;
  for  (idx = examples.begin ();  idx != examples.end ();  idx++)
  {
    const LarcosFeatureVectorPtr  imageExample = *idx;
    if  (Owner ())
      PushOnBack (new LarcosFeatureVector (*imageExample));
    else
      PushOnBack (imageExample);
  }
}




LarcosFeatureVectorList::LarcosFeatureVectorList (const LarcosFeatureVectorList&  examples,
                                    bool                     _owner
                                   ):

   FeatureVectorList (examples.FileDesc (), _owner, examples.log)
{
  const_iterator  idx;
  for  (idx = examples.begin ();  idx != examples.end ();  idx++)
  {
    const LarcosFeatureVectorPtr  imageExample = *idx;
    if  (Owner ())
      PushOnBack (new LarcosFeatureVector (*imageExample));
    else
      PushOnBack (imageExample);
  }
}


LarcosFeatureVectorList::LarcosFeatureVectorList (const FeatureVectorList&  featureVectorList,
                                    bool                      _owner
                                   ):

  FeatureVectorList (featureVectorList.FileDesc (),
                     _owner, 
                     featureVectorList.log
                    )
{
  //if  (strcmp (featureVectorList.UnderlyingClass (), "LarcosFeatureVectorList") == 0)
  if  (typeid (featureVectorList) == typeid (LarcosFeatureVectorList))
  {
    const LarcosFeatureVectorList&  examples = dynamic_cast<const LarcosFeatureVectorList&> (featureVectorList);
  }


  if  (_owner)
  {
    FeatureVectorList::const_iterator  idx;
    for  (idx = featureVectorList.begin ();  idx != featureVectorList.end ();  idx++)
    {
      FeatureVectorPtr featureVector = *idx;
      
      // The constructor below will detect what the underlying type of 'featureVector' is.  
      // If (underlying type is a 'LarcosFeatureVector' object)  then
      //   | Information that is particular to a 'LarcosFeatureVector' object will be extracted
      //   | from the 'FeatureVector' object being passed in.
      // else
      //   | info that is particular to a 'LarcosFeatureVector' object will be set to
      //   | default values.
      LarcosFeatureVectorPtr  example = new LarcosFeatureVector (*featureVector);
      PushOnBack (example);
    }
  }
  else
  {
    // Since we will not own the contents but just point to an existing instances we will 
    // have to make sure that the existing instances of 'FeatureVector' objects have a 
    // underlying type of 'LarcosFeatureVector'.
    FeatureVectorList::const_iterator  idx;
    for  (idx = featureVectorList.begin ();  idx != featureVectorList.end ();  idx++)
    {
      FeatureVectorPtr featureVector = *idx;
      //if  (strcmp (featureVector->UnderlyingClass (), "LarcosFeatureVector") == 0)
      if  (typeid (*featureVector) == typeid (LarcosFeatureVector))
      {
        LarcosFeatureVectorPtr example = dynamic_cast<LarcosFeatureVectorPtr>(featureVector);
        PushOnBack (example);
      }
      else
      {
        // ****    ERROR  ****
        KKStr  errMSsg = "LarcosFeatureVectorList   One of the elements in 'featureVectorList' is not of 'LarcosFeatureVector'  type.";
        log.Level (-1) << endl << endl << endl
             << "LarcosFeatureVectorList::LarcosFeatureVectorList (const FeatureVectorList&  featureVectorList)   ***ERROR***" << endl
             << "        " << errMSsg  << endl
             << "       FileName[" << featureVector->ImageFileName () << "]"  << endl
             << endl;
        throw KKException (errMSsg);
      }
    }
  }
}









//****************************************************************************
//*  Will Create a list of examples that are a subset of the ones in _examples.  *
//* The subset will consist of the examples who's mlClass is one of the     *
//* ones in mlClasses.                                                    *
//****************************************************************************
LarcosFeatureVectorList::LarcosFeatureVectorList (MLClassList&     _mlClasses,
                                      LarcosFeatureVectorList&  _examples,
                                      RunLog&             _log
                                     ):
  FeatureVectorList (_mlClasses, _examples, _log)
  
{
}




LarcosFeatureVectorList::LarcosFeatureVectorList (const FeatureVectorList&  featureVectorList):
  FeatureVectorList (featureVectorList.FileDesc (),
                     featureVectorList.Owner (), 
                     featureVectorList.log
                    )
{
  //if  (strcmp (featureVectorList.UnderlyingClass (), "LarcosFeatureVectorList") == 0)
  if  (typeid (featureVectorList) == typeid (LarcosFeatureVectorList))
  {
    const LarcosFeatureVectorList&  examples = dynamic_cast<const LarcosFeatureVectorList&> (featureVectorList);
  }


  if  (featureVectorList.Owner ())
  {
    FeatureVectorList::const_iterator  idx;
    for  (idx = featureVectorList.begin ();  idx != featureVectorList.end ();  idx++)
    {
      FeatureVectorPtr featureVector = *idx;
      
      // The constructor below will detect what the underlying type of 'featureVector' is.  
      // If (underlying type is a 'LarcosFeatureVector' object)  then
      //   | Information that is particular to a 'LarcosFeatureVector' object will be extracted
      //   | from the 'FeatureVector' object being passed in.
      // else
      //   | info that is particular to a 'LarcosFeatureVector' object will be set to
      //   | default values.
      LarcosFeatureVectorPtr  example = new LarcosFeatureVector (*featureVector);
      PushOnBack (example);
    }
  }
  else
  {
    // Since we will not own the contents but just point to existing instances we will 
    // have to make sure that the existing instances of 'FeatureVector' objects have a 
    // underlying type of 'LarcosFeatureVector'.
    FeatureVectorList::const_iterator  idx;
    for  (idx = featureVectorList.begin ();  idx != featureVectorList.end ();  idx++)
    {
      FeatureVectorPtr featureVector = *idx;
      //if  (strcmp (featureVector->UnderlyingClass (), "LarcosFeatureVector") == 0)
      if  (typeid (*featureVector) == typeid (LarcosFeatureVector))
      {
        LarcosFeatureVectorPtr example = dynamic_cast<LarcosFeatureVectorPtr>(featureVector);
        PushOnBack (example);
      }
      else
      {
        // ****    ERROR  ****
        log.Level (-1) << endl << endl << endl
             << "LarcosFeatureVectorList ::LarcosFeatureVectorList (const FeatureVectorList&  featureVectorList)              ***ERROR***" << endl
             << endl
             << "One of the elements in 'featureVectorList' is not of 'LarcosFeatureVector'  type.  We can not  recast this element"
             << "FileName[" << featureVector->ImageFileName () << "]"  << endl
             << endl;
        osWaitForEnter ();
        exit (-1);
      }
    }
  }
}





LarcosFeatureVectorList::~LarcosFeatureVectorList ()
{
}







LarcosFeatureVectorPtr  LarcosFeatureVectorList::IdxToPtr (kkint32 idx)  const
{
  return  (LarcosFeatureVectorPtr)FeatureVectorList::IdxToPtr (idx);
}  /* IdxToPtr */




void  LarcosFeatureVectorList::AddSingleImageFeatures (LarcosFeatureVectorPtr  _imageFeatures)
{
  PushOnBack (_imageFeatures);
}



LarcosFeatureVectorPtr  LarcosFeatureVectorList::BackOfQueue ()
{
  if  (size () < 1)
    return NULL;

  FeatureVectorPtr  fv = back ();
  //if  (strcmp (fv->UnderlyingClass (), "LarcosFeatureVector") == 0)
  if  (typeid (*fv) == typeid (LarcosFeatureVector))
    return  dynamic_cast<LarcosFeatureVectorPtr> (fv);

  
  log.Level (-1) << endl << endl 
                 << "LarcosFeatureVectorList::BackOfQueue ()    ***ERROR***        Entry at back of Queue is not a 'LarcosFeatureVector' object." << endl
                 << endl;

  return NULL;
}  /* BackOfQueue */




LarcosFeatureVectorPtr  LarcosFeatureVectorList::PopFromBack ()
{
  if  (size () < 1)  return NULL;

  FeatureVectorPtr  fv = back ();
  //if  (strcmp (fv->UnderlyingClass (), "LarcosFeatureVector") != 0)
  if  (typeid (*fv) == typeid (LarcosFeatureVector))
  {
    log.Level (-1)  << endl << endl 
                    << "LarcosFeatureVectorList::BackOfQueue ()    ***ERROR***        Entry poped from back of Queue is not a 'LarcosFeatureVector' object." << endl
                    << endl;
    return NULL;
  }

  return  dynamic_cast<LarcosFeatureVectorPtr> (FeatureVectorList::PopFromBack ());
}  /* PopFromBack */






void  LarcosFeatureVectorList::AddQueue (LarcosFeatureVectorList&  imagesToAdd)
{
  FeatureVectorList::AddQueue (imagesToAdd);
}  /* AddQueue */






LarcosFeatureVectorPtr  LarcosFeatureVectorList::BinarySearchByName (const KKStr&  _imageFileName)  const
{
  return  (LarcosFeatureVectorPtr)FeatureVectorList::BinarySearchByName (_imageFileName);
}  /* BinarySearchByName */





LarcosFeatureVectorPtr  LarcosFeatureVectorList::LookUpByRootName (const KKStr&  _rootName)
{
  return  (LarcosFeatureVectorPtr)FeatureVectorList::LookUpByRootName (_rootName);
}  /* LookUpByRootName */






LarcosFeatureVectorPtr  LarcosFeatureVectorList::LookUpByImageFileName (const KKStr&  _imageFileName)  const
{
  return  (LarcosFeatureVectorPtr)FeatureVectorList::LookUpByImageFileName (_imageFileName);
}  /* LookUpByImageFileName */





LarcosFeatureVectorListPtr  LarcosFeatureVectorList::OrderUsingNamesFromAFile (const KKStr&  fileName)
{
  FeatureVectorListPtr  examples = FeatureVectorList::OrderUsingNamesFromAFile (fileName);
  examples->Owner (false);
  LarcosFeatureVectorListPtr  orderedImages = new LarcosFeatureVectorList (*examples);
  delete  examples;
  return  orderedImages;
}  /* OrderUsingNamesFromAFile */






void   LarcosFeatureVectorList::FeatureExtraction (KKStr       _dirName, 
                                            KKStr       _fileName, 
                                            MLClassPtr  _mlClass
                                           )
{
  KKStr  className = _mlClass->Name ();
  log.Level (10) << "FeatureExtraction,  dirName   [" << _dirName    << "]." << endl;
  log.Level (10) << "                    fileName  [" << _fileName   << "]." << endl;
  log.Level (10) << "                    className [" << className   << "]." << endl;

  bool  cancelFlag  = false;
  bool  successful  = false;

  if  (_dirName.LastChar () != DSchar)
    _dirName << DS;
  
  KKStr  fullFeatureFileName (_dirName);
  fullFeatureFileName << _fileName;

  KKStrListPtr   fileNameList;
  
  KKStr  fileSpec (_dirName);
  fileSpec << "*.*";

  fileNameList = osGetListOfFiles (fileSpec);
  if  (!fileNameList)
    return;

  KKStrList::iterator  fnIDX = fileNameList->begin ();

  bool  successfull;

  KKStrPtr imageFileName = NULL;

  kkint32  numOfImages = fileNameList->QueueSize ();
  kkint32  count = 0;

  for  (fnIDX = fileNameList->begin ();   fnIDX != fileNameList->end ();  ++fnIDX)
  {
    if  ((count % 100) == 0)
      cout << className << " " << count << " of " << numOfImages << endl;

    imageFileName = *fnIDX;

    bool validImageFileFormat = SupportedImageFileFormat (*imageFileName);
    
    if  (!validImageFileFormat)
      continue;

    KKStr  fullFileName = osAddSlash (_dirName) + (*imageFileName);
    LarcosFeatureVectorPtr example = new LarcosFeatureVector (fullFileName, _mlClass, successfull, NULL);
    if  (!example)
    {
      KKStr  msg (100);
      msg << "LarcosFeatureVectorList::FeatureExtraction   ***ERROR***   Could not Allocate LarcosFeatureVector object" << endl
          << "for FileName[" << fullFileName << "].";
      cerr << endl << msg << endl << endl;
      throw KKException (msg);
    }

    if  (!successfull)
    {
      log.Level (-1) << "LarcosFeatureVectorList::FeatureExtraction  ***ERROR***, Processing file[" << (*imageFileName) << "]." << endl;
      delete  example;
      example = NULL;
    }

    else
    {
      example->ImageFileName (*imageFileName);
      log.Level (30) << example->ImageFileName () << "  " << example->OrigSize () << endl;
      PushOnBack (example);
    }

    count++;
  }

  Version (CurrentFeatureFileVersionNum);

  kkuint32  numExamplesWritten = 0;

  // WriteImageFeaturesToFile (fullFeatureFileName, RawFormat, FeatureNumList::AllFeatures (fileDesc));
  FeatureFileIOKK::Driver ()->SaveFeatureFile (fullFeatureFileName, 
                                               FeatureNumList::AllFeatures (FileDesc ()), 
                                               *this, 
                                               numExamplesWritten,
                                               cancelFlag,
                                               successful,
                                               log
                                              );
  delete  fileNameList;
  fileNameList = NULL;
}  /* FeatureExtraction */






/**
 * @brief  Creates a duplicate of list and also dupliactes it contents.
 * @return Duplicated list with hardcopy of its contents.
 */
LarcosFeatureVectorListPtr  LarcosFeatureVectorList::DuplicateListAndContents ()  const
{
  LarcosFeatureVectorListPtr  copyiedList = new LarcosFeatureVectorList (FileDesc (), true, log);

  for  (kkint32 idx = 0;  idx < QueueSize ();  idx++)
  {
    LarcosFeatureVectorPtr  curImage = (LarcosFeatureVectorPtr)IdxToPtr (idx);
    copyiedList->AddSingleImageFeatures (new LarcosFeatureVector (*curImage));
  }
  
  copyiedList->Version (Version ());

  return  copyiedList;
}  /* DuplicateListAndContents */






void  LarcosFeatureVectorList::RecalcFeatureValuesFromImagesInDirTree (KKStr  rootDir,
                                                                bool&  successful
                                                               )
{
  log.Level (20) << "RecalcFeatureValuesFromImagesInDirTree   RootDir[" << rootDir << "]." << endl;

  successful = false;

  ImageDirTree   fileDirectory (rootDir);

  if  (QueueSize () < 1)
  {
    successful = true;
    return;
  }

  if  (fileDirectory.Size () < 1)
  {
    log.Level (10) << "RecalcFeatureValuesFromImagesInDirTree  No Image Files in[" << rootDir << "]" << endl;
    return;
  }

  KKStrConstPtr  dirPath = NULL;

  iterator  idx;
  LarcosFeatureVectorPtr  example = NULL;

  for  (idx = begin ();   idx != end ();  idx++)
  {
    example = *idx;
    dirPath = fileDirectory.LocateImage (example->ImageFileName ());
    if  (!dirPath)
    {
      log.Level (10) << "RecalcFeatureValuesFromImagesInDirTree  Could not locate Image[" << example->ImageFileName () << "] in Subdirectory Tree." << endl;
      return;
    }

    KKStr  fullFileName (dirPath);
    osAddLastSlash (fullFileName);
    fullFileName << example->ImageFileName ();

    bool   validFile;
    RasterPtr  raster = new Raster (fullFileName, validFile);
    if  (!validFile)
    {
      delete  raster;
      return;
    }
    example->CalcFeatures (*raster, NULL);
    delete  raster;  raster = NULL;
  }
}  /* RecalcFeatureValuesFromImagesInDirTree */





LarcosFeatureVectorListPtr  LarcosFeatureVectorList::ExtractDuplicatesByRootImageFileName ()
{
  FeatureVectorListPtr  duplicateFeatureVectorObjects = FeatureVectorList::ExtractDuplicatesByRootImageFileName ();
  LarcosFeatureVectorListPtr  duplicateImageFeaturesObjects = new LarcosFeatureVectorList (*duplicateFeatureVectorObjects);
  duplicateFeatureVectorObjects->Owner (false);
  delete  duplicateFeatureVectorObjects;  duplicateFeatureVectorObjects = NULL;
  return  duplicateImageFeaturesObjects;
}  /* ExtractDuplicatesByRootImageFileName */






LarcosFeatureVectorListPtr   LarcosFeatureVectorList::ExtractImagesForAGivenClass (MLClassPtr  _mlClass,
                                                                     kkint32        _maxToExtract,
                                                                     float          _minSize
                                                                    )  const
{
  FeatureVectorListPtr  featureVectorList = FeatureVectorList::ExtractImagesForAGivenClass (_mlClass, _maxToExtract, _minSize);
  LarcosFeatureVectorListPtr  imageFeaturesList = new LarcosFeatureVectorList (*featureVectorList);
  featureVectorList->Owner (false);
  delete  featureVectorList;  featureVectorList = NULL;
  return  imageFeaturesList;
}  /*  ExtractImagesForAGivenClass  */





LarcosFeatureVectorListPtr  LarcosFeatureVectorList::StratifyAmoungstClasses (MLClassListPtr  mlClasses,
                                                                kkint32            maxImagesPerClass,
                                                                kkint32            numOfFolds
                                                               )
{
  FeatureVectorListPtr  stratifiedFeatureVectors = FeatureVectorList::StratifyAmoungstClasses (mlClasses, maxImagesPerClass, numOfFolds);
  LarcosFeatureVectorListPtr  stratifiedImagefeatures  = new LarcosFeatureVectorList (*stratifiedFeatureVectors);
  stratifiedFeatureVectors->Owner (false);
  delete stratifiedFeatureVectors;  stratifiedFeatureVectors = NULL;
  return  stratifiedImagefeatures;
}  /* StratifyAmoungstClasses */




LarcosFeatureVectorListPtr  LarcosFeatureVectorList::StratifyAmoungstClasses (kkint32  numOfFolds)
{
  MLClassListPtr  classes = ExtractListOfClasses ();

  FeatureVectorListPtr  stratifiedFeatureVectors = FeatureVectorList::StratifyAmoungstClasses (classes, -1, numOfFolds);
  LarcosFeatureVectorListPtr  stratifiedImagefeatures  = new LarcosFeatureVectorList (*stratifiedFeatureVectors);
  
  stratifiedFeatureVectors->Owner (false);

  delete stratifiedFeatureVectors;  stratifiedFeatureVectors = NULL;
  delete  classes;                  classes                  = NULL;

  return  stratifiedImagefeatures;
}  /* StratifyAmoungstClasses */

