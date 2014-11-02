/* CameraAcquisition.cpp -- Base class for the Acquisition threads.
 * Copyright (C) 2011-2013  Kurt Kramer
 * For conditions of distribution and use, see copyright notice in LarcosCounterUnManaged.txt
 */
#include "FirstIncludes.h"

#include <errno.h>
#include <istream>
#include <iostream>
#include <queue>
#include <vector>
#if  defined(WIN32)
#include <windows.h>
#endif
#include "MemoryDebug.h"
using namespace std;


#include "KKBaseTypes.h"
#include "GlobalGoalKeeper.h"
#include "Raster.h"
#include "RunLog.h"
using namespace KKB;


#include "FactoryFVProducer.h"
#include "FeatureVectorProducer.h"
using namespace  KKMachineLearning;

#include "LarcosFVProducer.h"

#include "FeatureFileIOKK.h"
#include "LarcosFeatureVector.h"
using  namespace  LarcosCounterUnManaged;





kkint16  LarcosFVProducer::maxNumOfFeatures      = 56;

kkint16  LarcosFVProducer::SizeIndex             = 0;
kkint16  LarcosFVProducer::Moment1Index          = 1;
kkint16  LarcosFVProducer::Moment2Index          = 2;
kkint16  LarcosFVProducer::Moment3Index          = 3;
kkint16  LarcosFVProducer::Moment4Index          = 4;
kkint16  LarcosFVProducer::Moment5Index          = 5;
kkint16  LarcosFVProducer::Moment6Index          = 6;
kkint16  LarcosFVProducer::Moment7Index          = 7;
kkint16  LarcosFVProducer::Moment8Index          = 8;

kkint16  LarcosFVProducer::EdgeSizeIndex         = 9;
kkint16  LarcosFVProducer::EdgeMoment1Index      = 10;
kkint16  LarcosFVProducer::EdgeMoment2Index      = 11;	
kkint16  LarcosFVProducer::EdgeMoment3Index      = 12;
kkint16  LarcosFVProducer::EdgeMoment4Index      = 13;
kkint16  LarcosFVProducer::EdgeMoment5Index      = 14;
kkint16  LarcosFVProducer::EdgeMoment6Index      = 15;
kkint16  LarcosFVProducer::EdgeMoment7Index      = 16;
kkint16  LarcosFVProducer::EdgeMoment8Index      = 17;

kkint16  LarcosFVProducer::TransparancyConvexHullIndex = 18;
kkint16  LarcosFVProducer::TransparancyPixelCountIndex = 19;
kkint16  LarcosFVProducer::TransparancyOpen3Index      = 20;
kkint16  LarcosFVProducer::TransparancyOpen5Index      = 21;
kkint16  LarcosFVProducer::TransparancyOpen7Index      = 22;
kkint16  LarcosFVProducer::TransparancyOpen9Index      = 23;
kkint16  LarcosFVProducer::TransparancyClose3Index     = 24;
kkint16  LarcosFVProducer::TransparancyClose5Index     = 25;
kkint16  LarcosFVProducer::TransparancyClose7Index     = 26;

kkint16  LarcosFVProducer::ConvexAreaIndex       = 27;
kkint16  LarcosFVProducer::TransparancySizeIndex = 28;
kkint16  LarcosFVProducer::TransparancyWtdIndex  = 29;
kkint16  LarcosFVProducer::WeighedMoment0Index   = 30;
kkint16  LarcosFVProducer::WeighedMoment1Index   = 31;
kkint16  LarcosFVProducer::WeighedMoment2Index   = 32;
kkint16  LarcosFVProducer::WeighedMoment3Index   = 33;
kkint16  LarcosFVProducer::WeighedMoment4Index   = 34;
kkint16  LarcosFVProducer::WeighedMoment5Index   = 35;
kkint16  LarcosFVProducer::WeighedMoment6Index   = 36;
kkint16  LarcosFVProducer::WeighedMoment7Index   = 37;
kkint16  LarcosFVProducer::WeighedMoment8Index   = 38;

kkint16  LarcosFVProducer::IntensityHist1Index   = 39;
kkint16  LarcosFVProducer::IntensityHist2Index   = 40;
kkint16  LarcosFVProducer::IntensityHist3Index   = 41;
kkint16  LarcosFVProducer::IntensityHist4Index   = 42;
kkint16  LarcosFVProducer::IntensityHist5Index   = 43;
kkint16  LarcosFVProducer::IntensityHist6Index   = 44;
kkint16  LarcosFVProducer::IntensityHist7Index   = 45;

kkint16  LarcosFVProducer::DarkSpotCount0        = 46;
kkint16  LarcosFVProducer::DarkSpotCount1        = 47;
kkint16  LarcosFVProducer::DarkSpotCount2        = 48;
kkint16  LarcosFVProducer::DarkSpotCount3        = 49;
kkint16  LarcosFVProducer::DarkSpotCount4        = 50;
kkint16  LarcosFVProducer::DarkSpotCount5        = 51;
kkint16  LarcosFVProducer::DarkSpotCount6        = 52;
kkint16  LarcosFVProducer::DarkSpotCount7        = 53;
kkint16  LarcosFVProducer::DarkSpotCount8        = 54;
kkint16  LarcosFVProducer::DarkSpotCount9        = 55;


const  KKStr  LarcosFVProducer::FeatureNames[] =
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







LarcosFVProducer::LarcosFVProducer (FactoryFVProducerPtr  factory):
    FeatureVectorProducer ("PostLarvaeFV",
                           factory,
                           PostLarvaeFV::PostLarvaeFeaturesFileDesc ()
                          ),
    totPixsForMorphOps (4000000),
    workRaster1Area (NULL),
    workRaster2Area (NULL),
    workRaster1Rows (NULL),
    workRaster2Rows (NULL)
{
  workRaster1Area = new uchar[totPixsForMorphOps];
  workRaster2Area = new uchar[totPixsForMorphOps];
}




LarcosFVProducer::~LarcosFVProducer ()
{
  delete  workRaster1Area;  workRaster1Area = NULL;
  delete  workRaster2Area;  workRaster2Area = NULL;
  delete  workRaster1Rows;  workRaster1Rows = NULL;
  delete  workRaster2Rows;  workRaster2Rows = NULL;
}



PostLarvaeFVPtr  LarcosFVProducer::ComputeFeatureVector (const Raster&     srcImage,
                                                         const MLClassPtr  knownClass,
                                                         RasterListPtr     intermediateImages,
                                                         RunLog&           runLog
                                                        )
{


  LarcosFeatureVectorPtr  fv = new LarcosFeatureVector (maxNumOfFeatures);
  float*  featureData = fv->FeatureDataAlter ();

  fv->Version (254);

  kkint32 areaBeforeReduction = 0;
  float  weighedSizeBeforeReduction = 0.0f;

  kkuint32 row = 0;
  kkuint32 row = 0;

  kkuint32  intensityHistBuckets[8];
  srcRaster->CalcAreaAndIntensityFeatures (areaBeforeReduction, 
                                           weighedSizeBeforeReduction,
                                           intensityHistBuckets
                                          );

  kkuint32 srcHeight = srcImage->Height ();
  kkuint32 srcWidth  = srcImage->Width  ();

  kkuint32 reducedHeight = srcHeight;
  kkuint32 reducedWidth  = srcWidth;

  kkuint32 reducedSquareArea = reducedHeight * reducedWidth;

  kkuint32 reductionMultiple = 1;

  while  (reducedSquareArea > totPixsForMorphOps)
  {
    ++reductionMultiple;
    reducedHeight = srcHeight    / reductionMultiple;
    reducedWidth  = reducedWidth / reductionMultiple;
    reducedSquareArea = reducedHeight * reducedWidth;
  }

  delete  workRaster1Rows;  workRaster1Rows = new uchar[reducedHeight];
  delete  workRaster2Rows;  workRaster2Rows = new uchar[reducedHeight];

  uchar*  wp1 = workRaster1Area;
  uchar*  wp2 = workRaster2Area;

  for  (row = 0;  row < reducedHeight; ++row)
  {
    workRaster1Rows[row] = wp1;
    workRaster2Rows[row] = wp2;

    wp1 += reducedWidth;
    wp2 += reducedWidth;

  }

  

 







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



  return fv;



}




const type_info*   LarcosFVProducer::FeatureVectorTypeId () const
{
  return  &(typeid (PostLarvaeFV));
}





LarcosFVProducerFactory::LarcosFVProducerFactory ():
   FactoryFVProducer("LarcosFVProducerFactory", "PostLarvaeFV", "Post-Larvae FeatureVector Producer Factory")
{
}



LarcosFVProducerFactory::~LarcosFVProducerFactory ()
{
}



FeatureFileIOPtr  LarcosFVProducerFactory::DefaultFeatureFileIO ()  const
{
  return  FeatureFileIOKK::Driver ();
}


const type_info*  LarcosFVProducerFactory::FeatureVectorTypeId ()  const  
{
  return  &typeid (PostLarvaeFV);
}



const type_info*  LarcosFVProducerFactory::FeatureVectorListTypeId  () const  
{
  return  &typeid (PostLarvaeFVList);
}



FileDescPtr  LarcosFVProducerFactory::FileDesc ()  const
{
  return  PostLarvaeFV::PostLarvaeFeaturesFileDesc ();
}









LarcosFVProducerPtr  LarcosFVProducerFactory::ManufactureInstance (RunLog&  runLog)
{
  



}  /* ManufactureInstance */














PostLarvaeFVListPtr  LarcosFVProducerFactory::ManufacturFeatureVectorList (bool     owner,
                                                                               RunLog&  runLog
                                                                              )
{
  return new PostLarvaeFVList (PostLarvaeFV::PostLarvaeFeaturesFileDesc (), owner, runLog);
}



LarcosFVProducerFactory*  LarcosFVProducerFactory::factory = Factory (NULL);




LarcosFVProducerFactory*  LarcosFVProducerFactory::Factory (RunLog*  runLog)
{
  GlobalGoalKeeper::StartBlock ();

  if  (factory == NULL)
  {
    factory = new LarcosFVProducerFactory ();
    FactoryFVProducer::RegisterFactory (factory, runLog);
  }
  GlobalGoalKeeper::EndBlock ();

  return  factory;
}

