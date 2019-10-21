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
#include "Option.h"
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
using namespace  KKMLL;


#include "PostLarvaeFV.h"
using namespace  CounterUnManaged;


// Will be used to histogram the average size of dark spots encountered.
class  DarkSpotStats
{
public:
  DarkSpotStats (MLClassPtr  _c,
                 kkuint32    _bucketSize
                ):
      bucketSize (_bucketSize),
      c          (_c),
      count      (0),
      sizeHist   (20, 0)
  {
  }


  void  AddDarkSpotsList (KKB::BlobListPtr  blobs)
  {
    ++count;
    for  (auto idx: *blobs)
    {
      KKB::BlobPtr  b = idx.second;
      uint  sizeIdx = b->PixelCount () / bucketSize;
      if  (sizeIdx < sizeHist.size ())
        sizeHist[sizeIdx] += 1;
    }
  }



  void  PrintStatsLine (ostream& o)
  {
    uint  x;
    o << c->Name () << "\t" << count;

    int  oldPrecision = (int)o.precision ();
    o.precision (3);

    for  (x = 0;  x < sizeHist.size ();  ++x)
    {
      float zed = 0.0f;
      if  (count > 0)
        zed = (float)sizeHist[x] / (float)count;
      o << "\t" << zed;
    }

    o << endl;
    
    o.precision (oldPrecision);
  }  /* PrintStatsLine */

  kkuint32       bucketSize;
  MLClassPtr  c;
  int            count;        /**< number of images processed. */
  vector<int>    sizeHist;     
};  /* DarkSpotStats */



class  DarkSpotsStatsList:  map<MLClassPtr, DarkSpotStats*>
{
public:
  DarkSpotsStatsList (kkuint32 _bucketSize):
      bucketSize (_bucketSize)
  {
  }

  

  ~DarkSpotsStatsList ()
  {
    for  (idx = begin ();  idx != end ();  ++idx)
      delete  idx->second;
  }



  void  AddDarkSpotsList (MLClassPtr       _c,
                          KKB::BlobListPtr  blobs 
                         )
  {
    DarkSpotStats*  stats = NULL;

    idx = this->find (_c);
    if  (idx != end ())
    {
      stats = idx->second;
    }
    else
    {
      stats = new DarkSpotStats (_c, bucketSize);
      insert(pair<MLClassPtr, DarkSpotStats*>(_c, stats));
    }

    stats->AddDarkSpotsList (blobs);
  }



  void  PrintHeaderLine (ostream&  o)
  {
    o << "ClassName" << "\t" << "Count";
    for  (int x = 0;  x < 20;  ++x)
    {
      int  start = x * bucketSize;
      int  end   = start + bucketSize - 1;
      o << "\t" << "\"" << start << "-" << "\"" << end;
    }
    o << endl;
    return;
  }



  struct  SortKey
  {
    SortKey (MLClassPtr   _c,
             DarkSpotStats*  _ds
            ):
        countFactor (_c->CountFactor ()),
        c           (_c),
        ds          (_ds)
    {}


    bool  operator<(const SortKey& right)  const
    {
      float  x = (countFactor - right.countFactor);
      if  (x == 0.0f)
        x = (float)(c->Name ().CompareTo (right.c->Name ()));
      return (x < 0.0f);
    }

    float           countFactor;
    MLClassPtr   c;
    DarkSpotStats*  ds;
  };

 

  void  PrintReport (ostream&  o)
  {
    vector<SortKey>  sortKeys;
    for  (idx = begin ();  idx != end ();  ++idx)
      sortKeys.push_back (SortKey(idx->first, idx->second));

    sort (sortKeys.begin (), sortKeys.end ());

    o << "BucketSize" << "\t" << bucketSize << endl << endl;
    PrintHeaderLine (o);

    for  (auto idx2: sortKeys)
    {
      idx2.ds->PrintStatsLine (o);
    }
    o << endl << endl;
  }  /* PrintReport */


  private:
    kkuint32  bucketSize;
    iterator  idx;
};  /* DarkSpotsStatsList */



DarkSpotsStatsList*  darkSportStats001 = NULL;
DarkSpotsStatsList*  darkSportStats010 = NULL;
DarkSpotsStatsList*  darkSportStats050 = NULL;
DarkSpotsStatsList*  darkSportStats100 = NULL;
DarkSpotsStatsList*  darkSportStats500 = NULL;



void  CounterUnManaged::PostLarvaeFVResetDarkSpotCounts ()
{
  delete  darkSportStats001;  darkSportStats001 = NULL;
  delete  darkSportStats010;  darkSportStats010 = NULL;
  delete  darkSportStats050;  darkSportStats050 = NULL;
  delete  darkSportStats100;  darkSportStats100 = NULL;
  delete  darkSportStats500;  darkSportStats500 = NULL;

  darkSportStats001 = new DarkSpotsStatsList (1);
  darkSportStats010 = new DarkSpotsStatsList (10);
  darkSportStats050 = new DarkSpotsStatsList (50);
  darkSportStats100 = new DarkSpotsStatsList (100);
  darkSportStats500 = new DarkSpotsStatsList (500);
}  /* PostLarvaeFVResetDarkSpotCounts */



void  CounterUnManaged::PostLarvaeFVPrintReport (ostream& o)
{
  if  (darkSportStats001 != NULL)
    darkSportStats001->PrintReport (o);

  if  (darkSportStats010 != NULL)
    darkSportStats010->PrintReport (o);

  if  (darkSportStats050 != NULL)
    darkSportStats050->PrintReport (o);

  if  (darkSportStats100 != NULL)
    darkSportStats100->PrintReport (o);

  if  (darkSportStats500 != NULL)
    darkSportStats500->PrintReport (o);
}



void  CounterUnManaged::PostLarvaeFVAddBlobList (MLClassPtr     c,
                                                  KKB::BlobListPtr  blobs 
                                                 )
{
  if  (darkSportStats001)
    darkSportStats001->AddDarkSpotsList (c, blobs);

  if  (darkSportStats010)
    darkSportStats010->AddDarkSpotsList (c, blobs);

  if  (darkSportStats050)
    darkSportStats050->AddDarkSpotsList (c, blobs);

  if  (darkSportStats100)
    darkSportStats100->AddDarkSpotsList (c, blobs);

  if  (darkSportStats500)
    darkSportStats500->AddDarkSpotsList (c, blobs);
}
                         


const  kkint32  PostLarvaeFV::SizeThreshold = 10000;  /**< Size of example in number of pixels before we decide to reduce the
                                                       * example to improve feature calculation.
                                                       */


							
kkuint32  PostLarvaeFV::maxNumOfFeatures      = 56;

kkint16  PostLarvaeFV::SizeIndex             = 0;
kkint16  PostLarvaeFV::Moment1Index          = 1;
kkint16  PostLarvaeFV::Moment2Index          = 2;
kkint16  PostLarvaeFV::Moment3Index          = 3;
kkint16  PostLarvaeFV::Moment4Index          = 4;
kkint16  PostLarvaeFV::Moment5Index          = 5;
kkint16  PostLarvaeFV::Moment6Index          = 6;
kkint16  PostLarvaeFV::Moment7Index          = 7;
kkint16  PostLarvaeFV::Moment8Index          = 8;

kkint16  PostLarvaeFV::EdgeSizeIndex         = 9;
kkint16  PostLarvaeFV::EdgeMoment1Index      = 10;
kkint16  PostLarvaeFV::EdgeMoment2Index      = 11;	
kkint16  PostLarvaeFV::EdgeMoment3Index      = 12;
kkint16  PostLarvaeFV::EdgeMoment4Index      = 13;
kkint16  PostLarvaeFV::EdgeMoment5Index      = 14;
kkint16  PostLarvaeFV::EdgeMoment6Index      = 15;
kkint16  PostLarvaeFV::EdgeMoment7Index      = 16;
kkint16  PostLarvaeFV::EdgeMoment8Index      = 17;

kkint16  PostLarvaeFV::TransparancyConvexHullIndex = 18;
kkint16  PostLarvaeFV::TransparancyPixelCountIndex = 19;
kkint16  PostLarvaeFV::TransparancyOpen3Index      = 20;
kkint16  PostLarvaeFV::TransparancyOpen5Index      = 21;
kkint16  PostLarvaeFV::TransparancyOpen7Index      = 22;
kkint16  PostLarvaeFV::TransparancyOpen9Index      = 23;
kkint16  PostLarvaeFV::TransparancyClose3Index     = 24;
kkint16  PostLarvaeFV::TransparancyClose5Index     = 25;
kkint16  PostLarvaeFV::TransparancyClose7Index     = 26;

kkint16  PostLarvaeFV::ConvexAreaIndex       = 27;
kkint16  PostLarvaeFV::TransparancySizeIndex = 28;
kkint16  PostLarvaeFV::TransparancyWtdIndex  = 29;
kkint16  PostLarvaeFV::WeighedMoment0Index   = 30;
kkint16  PostLarvaeFV::WeighedMoment1Index   = 31;
kkint16  PostLarvaeFV::WeighedMoment2Index   = 32;
kkint16  PostLarvaeFV::WeighedMoment3Index   = 33;
kkint16  PostLarvaeFV::WeighedMoment4Index   = 34;
kkint16  PostLarvaeFV::WeighedMoment5Index   = 35;
kkint16  PostLarvaeFV::WeighedMoment6Index   = 36;
kkint16  PostLarvaeFV::WeighedMoment7Index   = 37;
kkint16  PostLarvaeFV::WeighedMoment8Index   = 38;

kkint16  PostLarvaeFV::IntensityHist1Index   = 39;
kkint16  PostLarvaeFV::IntensityHist2Index   = 40;
kkint16  PostLarvaeFV::IntensityHist3Index   = 41;
kkint16  PostLarvaeFV::IntensityHist4Index   = 42;
kkint16  PostLarvaeFV::IntensityHist5Index   = 43;
kkint16  PostLarvaeFV::IntensityHist6Index   = 44;
kkint16  PostLarvaeFV::IntensityHist7Index   = 45;

kkint16  PostLarvaeFV::DarkSpotCount0        = 46;
kkint16  PostLarvaeFV::DarkSpotCount1        = 47;
kkint16  PostLarvaeFV::DarkSpotCount2        = 48;
kkint16  PostLarvaeFV::DarkSpotCount3        = 49;
kkint16  PostLarvaeFV::DarkSpotCount4        = 50;
kkint16  PostLarvaeFV::DarkSpotCount5        = 51;
kkint16  PostLarvaeFV::DarkSpotCount6        = 52;
kkint16  PostLarvaeFV::DarkSpotCount7        = 53;
kkint16  PostLarvaeFV::DarkSpotCount8        = 54;
kkint16  PostLarvaeFV::DarkSpotCount9        = 55;


const  char*  PostLarvaeFV::FeatureNames[] =
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
  
  "TransparancyOpen3",       // 20    Gramulmetric features
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

RunLog  PostLarvaeFV::runLog;



KKStr  PostLarvaeFV::FeatureName (kkuint32  fieldNum)
{
  KKCheck (fieldNum < MaxNumOfFeatures (), "PostLarvaeFV::::FeatureName   ***ERROR***  FeatureNum: " << fieldNum << " is out of bounds.")
  return  FeatureNames [fieldNum];
}  /* FeatureName */



FileDescConstPtr  PostLarvaeFV::postLarvaeFeaturesFileDesc = NULL;


FileDescConstPtr  PostLarvaeFV::PostLarvaeFeaturesFileDesc ()
{
  if  (postLarvaeFeaturesFileDesc)
    return  postLarvaeFeaturesFileDesc;

  bool  alreadyExists = false;
  auto tempFileDesc = new FileDesc ();
  for  (kkuint32 fieldNum = 0;  fieldNum < MaxNumOfFeatures ();  ++fieldNum)
  {
    tempFileDesc->AddAAttribute (FeatureName (fieldNum),  AttributeType::Numeric, alreadyExists);
  }

  // Lets make sure that one was already created by opening up a data file.
  postLarvaeFeaturesFileDesc = FileDesc::GetExistingFileDesc (tempFileDesc);

  return  postLarvaeFeaturesFileDesc;
}  /* PostLarvaeFeaturesFileDesc () */



PostLarvaeFV::PostLarvaeFV (kkuint32  _numOfFeatures):
       FeatureVector (_numOfFeatures),

        centroidCol     (-1),
        centroidRow     (-1),
        numOfEdgePixels (-1)
{
}



PostLarvaeFV::PostLarvaeFV (const PostLarvaeFV&  _image):
  FeatureVector (_image),

  centroidCol     (_image.centroidCol),
  centroidRow     (_image.centroidRow),
  numOfEdgePixels (_image.numOfEdgePixels)

{
}



PostLarvaeFV::PostLarvaeFV (const BmpImage&   _image,
                            const MLClassPtr  _mlClass,
                            RasterListPtr     _intermediateImages
                           ):
  FeatureVector (MaxNumOfFeatures ()),
  centroidCol      (-1),
  centroidRow      (-1),
  numOfEdgePixels  (-1)
{
  MLClass       (_mlClass);
  ExampleFileName (_image.FileName ());

  RasterPtr raster = new Raster (_image);
  CalcFeatures (*raster, _intermediateImages);
  delete  raster;
}



PostLarvaeFV::PostLarvaeFV (      Raster&     _raster,
                            const MLClassPtr  _mlClass,
                            RasterListPtr     _intermediateImages
                           ):
  FeatureVector (MaxNumOfFeatures ()),
  centroidCol      (-1),
  centroidRow      (-1),
  numOfEdgePixels  (-1)
{
  MLClass    (_mlClass);
  ExampleFileName (_raster.FileName ());

  CalcFeatures  (_raster, _intermediateImages);
}



PostLarvaeFV::PostLarvaeFV (KKStr          _fileName,
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
  ExampleFileName    (_fileName);

  RasterPtr  raster = ReadImage (_fileName);
  if  (raster == NULL)
  {
    _successfull = false;
    for  (kkuint32 x = 0;  x < MaxNumOfFeatures ();  ++x)
      featureData[x] = 0.0f;
    cerr  << "PostLarvaeFV::PostLarvaeFV  ***ERROR***, Opening File[" << _fileName << "]." << endl;
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



PostLarvaeFV::PostLarvaeFV (const FeatureVector&  featureVector):
   FeatureVector    (featureVector),
   centroidCol      (-1),
   centroidRow      (-1),
   numOfEdgePixels  (-1)

{
  //if  (strcmp (featureVector.UnderlyingClass (), "PostLarvaeFV") == 0)
  if  (typeid (featureVector) == typeid(PostLarvaeFV))
  {
    // The underlying class is another PostLarvaeFV object.
    const PostLarvaeFV&  example = dynamic_cast<const PostLarvaeFV&>(featureVector);

    centroidCol      = example.CentroidCol     ();
    centroidRow      = example.CentroidRow     ();
    numOfEdgePixels  = example.NumOfEdgePixels ();
  }
}



PostLarvaeFV::~PostLarvaeFV ()
{
}



PostLarvaeFVPtr  PostLarvaeFV::Duplicate ()  const
{
  return new PostLarvaeFV (*this);
}



void  PostLarvaeFV::SaveIntermediateImage (Raster&        raster, 
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



void  PostLarvaeFV::ParseImageFileName (const KKStr&  fullFileName, 
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
  
  auto  x = rootName.LocateLastOccurrence ('_');
  if  (x  &&  (x.value () > 0))
  {
    KKStr  colStr = rootName.SubStrPart (x);
    KKStr  temp = rootName.SubStrSeg (0, x);
    x = temp.LocateLastOccurrence ('_');
    if  (x  &&  (x.value () > 0))
    {
      scannerFileName = temp.SubStrSeg (0, x);
      KKStr  rowStr = temp.SubStrPart (x + 1);
      scanCol     = atoi (colStr.Str ());
      scanLineNum = atoi (rowStr.Str ());
    }
  }
}  /* ParseImageFileName */



void  PostLarvaeFV::CalcFeatures (Raster&        srcRaster,
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
    for  (kkuint32 tp = 0;  tp < numOfFeatures;  ++tp)
      featureData[tp] = FLT_MAX;
    
    if  (weOwnRaster)
    {
      delete  raster;
      raster = NULL;
    }
    return;
  }
  
  KKStr  driveLetter, path, root, extension;

  float  convexf = 0.0;

  osParseFileSpec (ExampleFileName (), driveLetter, path, root, extension);
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

  raster->Dilation (wr1);
  wr1->Dilation (wr2);
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

  kkint32 area = (kkint32)(momentf[0] + 0.5f);  // Moment-0 is the same as the number of foreground pixels in example.
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
  wr1->Dilation (wr2);
  float  areaOpen3 = (float)(wr2->ForegroundPixelCount());
  if  (intermediateImages)
    SaveIntermediateImage (*wr2, "Opening3_" + StrFormatInt ((kkint32)areaOpen3, "ZZZZZZ0"), intermediateImages);

  raster->Erosion (wr1, MorphOp::MaskTypes::SQUARE5);
  wr1->Dilation (wr2, MorphOp::MaskTypes::SQUARE5);
  float  areaOpen5 = (float)(wr2->ForegroundPixelCount ());
  if  (intermediateImages)
    SaveIntermediateImage (*wr2, "Opening5_" + StrFormatInt ((kkint32)areaOpen5, "ZZZZZZ0"), intermediateImages);

  raster->Erosion (wr1, MorphOp::MaskTypes::SQUARE7);
  wr1->Dilation (wr2, MorphOp::MaskTypes::SQUARE7);
  float  areaOpen7 = (float)(wr2->ForegroundPixelCount ());
  if  (intermediateImages)
    SaveIntermediateImage (*wr2, "Opening7_" + StrFormatInt ((kkint32)areaOpen7, "ZZZZZZ0"), intermediateImages);
  
  wr2->Erosion (wr1, MorphOp::MaskTypes::SQUARE9);
  wr1->Dilation (wr2, MorphOp::MaskTypes::SQUARE9);
  float  areaOpen9 = (float)(wr2->ForegroundPixelCount ());
  if  (intermediateImages)
    SaveIntermediateImage (*wr2, "Opening9_" + StrFormatInt ((kkint32)areaOpen9, "ZZZZZZ0"), intermediateImages);

  raster->Dilation (wr1);
  wr1->Erosion (wr2);
  float  areaClose3 = (float)(wr2->ForegroundPixelCount ());
  if  (intermediateImages)
    SaveIntermediateImage (*wr2, "Close3_" + StrFormatInt ((kkint32)areaClose3, "ZZZZZZ0"), intermediateImages);

  wr2->FillHole (wr1);
  float  tranf = (float)(wr2->ForegroundPixelCount ());
  if  (intermediateImages)
    SaveIntermediateImage (*wr2, "FillHole_" + StrFormatInt ((kkint32)tranf, "ZZZZZZ0"), intermediateImages);

  raster->Dilation (wr1, MorphOp::MaskTypes::SQUARE5);
  wr1->Erosion (wr2, MorphOp::MaskTypes::SQUARE5);
  float  areaClose5 = (float)(wr2->ForegroundPixelCount ());
  if  (intermediateImages)
    SaveIntermediateImage (*wr2, "Close5_" + StrFormatInt ((kkint32)areaClose5, "ZZZZZZ0"), intermediateImages);
  
  raster->Dilation (wr1, MorphOp::MaskTypes::SQUARE7);
  wr1->Erosion   (wr2, MorphOp::MaskTypes::SQUARE7);
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
    // This part has to be done after 'CalcOrientationAndEigerRatio' is called.  That is where the example centroid is calculated.
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

    darkSpots->Erosion (MorphOp::MaskTypes::SQUARE3);
    darkSpots->Erosion (MorphOp::MaskTypes::SQUARE3);

    KKB::BlobListPtr  blobs = darkSpots->ExtractBlobs (3);

    int  darkSpotFreq[10];
    int x;
    for  (x = 0;  x < 10;  ++x)
      darkSpotFreq[x] = 0;

    for  (auto idx: *blobs)
    {
      KKB::BlobPtr  b = idx.second;

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

  featureData[0] = (float)areaBeforeReduction;  // In case the example was reduced.
  OrigSize ((float)areaBeforeReduction);

  Probability (-1.0f);

  delete  wr1;  wr1 = NULL;
  delete  wr2;  wr2 = NULL;

  if  (weOwnRaster)
    delete  raster;
}  /* CalcFeatures */



PostLarvaeFVList::PostLarvaeFVList (FileDescConstPtr  _fileDesc,
                                    bool             _owner
                                   ):

    FeatureVectorList (_fileDesc, _owner)
{

}



PostLarvaeFVList::PostLarvaeFVList (MLClassPtr _mlClass,
                                    KKStr      _dirName,
                                    KKStr      _fileName,
                                    RunLog&    _log                                    
                                   ):

  FeatureVectorList (PostLarvaeFV::PostLarvaeFeaturesFileDesc (), true)

{
  FeatureExtraction (_dirName, _fileName, _mlClass, _log);
}



PostLarvaeFVList::PostLarvaeFVList (const PostLarvaeFVList&  examples):
   FeatureVectorList (examples.FileDesc (), examples.Owner ())
{
  const_iterator  idx;
  for  (idx = examples.begin ();  idx != examples.end ();  idx++)
  {
    const PostLarvaeFVPtr  imageExample = *idx;
    if  (Owner ())
      PushOnBack (new PostLarvaeFV (*imageExample));
    else
      PushOnBack (imageExample);
  }
}



PostLarvaeFVList::PostLarvaeFVList (const PostLarvaeFVList&  examples,
                                    bool                     _owner
                                   ):

   FeatureVectorList (examples.FileDesc (), _owner)
{
  const_iterator  idx;
  for  (idx = examples.begin ();  idx != examples.end ();  idx++)
  {
    const PostLarvaeFVPtr  imageExample = *idx;
    if  (Owner ())
      PushOnBack (new PostLarvaeFV (*imageExample));
    else
      PushOnBack (imageExample);
  }
}



PostLarvaeFVList::PostLarvaeFVList (const FeatureVectorList&  featureVectorList,
                                    bool                      _owner
                                   ):

  FeatureVectorList (featureVectorList.FileDesc (),
                     _owner
                    )
{
  //if  (strcmp (featureVectorList.UnderlyingClass (), "PostLarvaeFVList") == 0)
  if  (typeid (featureVectorList) == typeid (PostLarvaeFVList))
  {
    const PostLarvaeFVList&  examples = dynamic_cast<const PostLarvaeFVList&> (featureVectorList);
  }


  if  (_owner)
  {
    FeatureVectorList::const_iterator  idx;
    for  (idx = featureVectorList.begin ();  idx != featureVectorList.end ();  idx++)
    {
      FeatureVectorPtr featureVector = *idx;
      
      // The constructor below will detect what the underlying type of 'featureVector' is.  
      // If (underlying type is a 'PostLarvaeFV' object)  then
      //   | Information that is particular to a 'PostLarvaeFV' object will be extracted
      //   | from the 'FeatureVector' object being passed in.
      // else
      //   | info that is particular to a 'PostLarvaeFV' object will be set to
      //   | default values.
      PostLarvaeFVPtr  example = new PostLarvaeFV (*featureVector);
      PushOnBack (example);
    }
  }
  else
  {
    // Since we will not own the contents but just point to an existing instances we will 
    // have to make sure that the existing instances of 'FeatureVector' objects have a 
    // underlying type of 'PostLarvaeFV'.
    FeatureVectorList::const_iterator  idx;
    for  (idx = featureVectorList.begin ();  idx != featureVectorList.end ();  idx++)
    {
      FeatureVectorPtr featureVector = *idx;
      //if  (strcmp (featureVector->UnderlyingClass (), "PostLarvaeFV") == 0)
      if  (typeid (*featureVector) == typeid (PostLarvaeFV))
      {
        PostLarvaeFVPtr example = dynamic_cast<PostLarvaeFVPtr>(featureVector);
        PushOnBack (example);
      }
      else
      {
        // ****    ERROR  ****
        KKStr  errMsg = "PostLarvaeFVList   One of the elements in 'featureVectorList' is not of 'PostLarvaeFV'  type.";
        cerr << endl << errMsg << endl << endl;
        throw KKException (errMsg);
      }
    }
  }
}



//****************************************************************************
//*  Will Create a list of examples that are a subset of the ones in _examples.  *
//* The subset will consist of the examples who's mlClass is one of the     *
//* ones in mlClasses.                                                    *
//****************************************************************************
PostLarvaeFVList::PostLarvaeFVList (MLClassList&       _mlClasses,
                                    PostLarvaeFVList&  _examples
                                   ):
  FeatureVectorList (_mlClasses, _examples)
  
{
}



PostLarvaeFVList::PostLarvaeFVList (const FeatureVectorList&  featureVectorList):
  FeatureVectorList (featureVectorList.FileDesc (),
                     featureVectorList.Owner ()
                    )
{
  //if  (strcmp (featureVectorList.UnderlyingClass (), "PostLarvaeFVList") == 0)
  if  (typeid (featureVectorList) == typeid (PostLarvaeFVList))
  {
    const PostLarvaeFVList&  examples = dynamic_cast<const PostLarvaeFVList&> (featureVectorList);
  }

  if  (featureVectorList.Owner ())
  {
    FeatureVectorList::const_iterator  idx;
    for  (idx = featureVectorList.begin ();  idx != featureVectorList.end ();  idx++)
    {
      FeatureVectorPtr featureVector = *idx;
      
      // The constructor below will detect what the underlying type of 'featureVector' is.  
      // If (underlying type is a 'PostLarvaeFV' object)  then
      //   | Information that is particular to a 'PostLarvaeFV' object will be extracted
      //   | from the 'FeatureVector' object being passed in.
      // else
      //   | info that is particular to a 'PostLarvaeFV' object will be set to
      //   | default values.
      PostLarvaeFVPtr  example = new PostLarvaeFV (*featureVector);
      PushOnBack (example);
    }
  }
  else
  {
    // Since we will not own the contents but just point to existing instances we will 
    // have to make sure that the existing instances of 'FeatureVector' objects have a 
    // underlying type of 'PostLarvaeFV'.
    FeatureVectorList::const_iterator  idx;
    for  (idx = featureVectorList.begin ();  idx != featureVectorList.end ();  idx++)
    {
      FeatureVectorPtr featureVector = *idx;
      if  (typeid (*featureVector) == typeid (PostLarvaeFV))
      {
        PostLarvaeFVPtr example = dynamic_cast<PostLarvaeFVPtr>(featureVector);
        PushOnBack (example);
      }
      else
      {
        // ****    ERROR  ****
        KKStr  errMsg;
        errMsg << "PostLarvaeFVList ::PostLarvaeFVList  ***ERROR***   One elements is not 'PostLarvaeFV'  type; can not  recast: " 
             << "FileName[" << featureVector->ExampleFileName () << "]";
        cerr << errMsg;
        throw KKException (errMsg);
      }
    }
  }
}



PostLarvaeFVList::~PostLarvaeFVList ()
{
}



PostLarvaeFVPtr  PostLarvaeFVList::IdxToPtr (kkint32 idx)  const
{
  return  (PostLarvaeFVPtr)FeatureVectorList::IdxToPtr (idx);
}  /* IdxToPtr */



void  PostLarvaeFVList::AddSingleImageFeatures (PostLarvaeFVPtr  _imageFeatures)
{
  PushOnBack (_imageFeatures);
}



PostLarvaeFVPtr  PostLarvaeFVList::BackOfQueue ()
{
  if  (size () < 1)
    return NULL;

  FeatureVectorPtr  fv = back ();
  if  (typeid (*fv) == typeid (PostLarvaeFV))
    return  dynamic_cast<PostLarvaeFVPtr> (fv);
  
  cerr << endl 
       << "PostLarvaeFVList::BackOfQueue   ***ERROR***   Entry at back of Queue is not a 'PostLarvaeFV' object." << endl
       << endl;

  return NULL;
}  /* BackOfQueue */



PostLarvaeFVPtr  PostLarvaeFVList::PopFromBack ()
{
  if  (size () < 1)  return NULL;

  FeatureVectorPtr  fv = back ();
  if  (typeid (*fv) == typeid (PostLarvaeFV))
  {
  cerr << endl 
       << "PostLarvaeFVList::BackOfQueue   ***ERROR***   Entry at Front of Queue is not a 'PostLarvaeFV' object." << endl
       << endl;
    return NULL;
  }

  return  dynamic_cast<PostLarvaeFVPtr> (FeatureVectorList::PopFromBack ());
}  /* PopFromBack */



void  PostLarvaeFVList::AddQueue (PostLarvaeFVList&  imagesToAdd)
{
  FeatureVectorList::AddQueue (imagesToAdd);
}  /* AddQueue */



PostLarvaeFVPtr  PostLarvaeFVList::BinarySearchByName (const KKStr&  _imageFileName)  const
{
  return  (PostLarvaeFVPtr)FeatureVectorList::BinarySearchByName (_imageFileName);
}  /* BinarySearchByName */



PostLarvaeFVPtr  PostLarvaeFVList::LookUpByRootName (const KKStr&  _rootName)
{
  return  (PostLarvaeFVPtr)FeatureVectorList::LookUpByRootName (_rootName);
}  /* LookUpByRootName */



PostLarvaeFVListPtr   PostLarvaeFVList::ManufactureEmptyList (bool _owner)  const
{
  return new PostLarvaeFVList (FileDesc (), _owner);
}



PostLarvaeFVPtr  PostLarvaeFVList::LookUpByImageFileName (const KKStr&  _imageFileName)  const
{
  return  (PostLarvaeFVPtr)FeatureVectorList::LookUpByImageFileName (_imageFileName);
}  /* LookUpByImageFileName */



PostLarvaeFVListPtr  PostLarvaeFVList::OrderUsingNamesFromAFile (const KKStr&  fileName,
                                                                 RunLog&       log
                                                                )
{
  FeatureVectorListPtr  examples = FeatureVectorList::OrderUsingNamesFromAFile (fileName, log);
  examples->Owner (false);
  PostLarvaeFVListPtr  orderedImages = new PostLarvaeFVList (*examples);
  delete  examples;
  return  orderedImages;
}  /* OrderUsingNamesFromAFile */



void   PostLarvaeFVList::FeatureExtraction (KKStr       _dirName, 
                                            KKStr       _fileName, 
                                            MLClassPtr  _mlClass,
                                            RunLog&     _log
                                           )
{
  KKStr  className = _mlClass->Name ();
  _log.Level (10) << "FeatureExtraction,  dirName   [" << _dirName    << "]." << endl;
  _log.Level (10) << "                    fileName  [" << _fileName   << "]." << endl;
  _log.Level (10) << "                    className [" << className   << "]." << endl;

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
    PostLarvaeFVPtr example = new PostLarvaeFV (fullFileName, _mlClass, successfull, NULL);
    if  (!example)
    {
      KKStr  msg (100);
      msg << "PostLarvaeFVList::FeatureExtraction   ***ERROR***   Could not Allocate PostLarvaeFV object" << endl
          << "for FileName[" << fullFileName << "].";
      _log.Level (-1) << endl << msg << endl << endl;
      throw KKException (msg);
    }

    if  (!successfull)
    {
      _log.Level (-1) << "PostLarvaeFVList::FeatureExtraction  ***ERROR***, Processing file[" << (*imageFileName) << "]." << endl;
      delete  example;
      example = NULL;
    }

    else
    {
      example->ExampleFileName (*imageFileName);
      _log.Level (30) << example->ExampleFileName () << "  " << example->OrigSize () << endl;
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
                                               _log
                                              );
  delete  fileNameList;
  fileNameList = NULL;
}  /* FeatureExtraction */



PostLarvaeFVListPtr  PostLarvaeFVList::Duplicate (bool _owner)  const
{
  return new PostLarvaeFVList (*this, _owner);
}



/**
 * @brief  Creates a duplicate of list and also duplicates it contents.
 * @return Duplicated list with hard-copy of its contents.
 */
PostLarvaeFVListPtr  PostLarvaeFVList::DuplicateListAndContents ()  const
{
  PostLarvaeFVListPtr  copyiedList = new PostLarvaeFVList (FileDesc (), true);

  for  (kkuint32 idx = 0;  idx < QueueSize ();  idx++)
  {
    PostLarvaeFVPtr  curImage = (PostLarvaeFVPtr)IdxToPtr (idx);
    copyiedList->AddSingleImageFeatures (new PostLarvaeFV (*curImage));
  }
  
  copyiedList->Version (Version ());

  return  copyiedList;
}  /* DuplicateListAndContents */



void  PostLarvaeFVList::RecalcFeatureValuesFromImagesInDirTree (KKStr    rootDir,
                                                                bool&    successful,
                                                                RunLog&  log
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
  PostLarvaeFVPtr  example = NULL;

  for  (idx = begin ();   idx != end ();  idx++)
  {
    example = *idx;
    dirPath = fileDirectory.LocateImage (example->ExampleFileName ());
    if  (!dirPath)
    {
      log.Level (10) << "RecalcFeatureValuesFromImagesInDirTree  Could not locate Image[" << example->ExampleFileName () << "] in Subdirectory Tree." << endl;
      return;
    }

    KKStr  fullFileName (*dirPath);
    osAddLastSlash (fullFileName);
    fullFileName << example->ExampleFileName ();

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



PostLarvaeFVListPtr  PostLarvaeFVList::ExtractDuplicatesByRootImageFileName ()
{
  FeatureVectorListPtr  duplicateFeatureVectorObjects = FeatureVectorList::ExtractDuplicatesByRootImageFileName ();
  PostLarvaeFVListPtr  duplicateImageFeaturesObjects = new PostLarvaeFVList (*duplicateFeatureVectorObjects);
  duplicateFeatureVectorObjects->Owner (false);
  delete  duplicateFeatureVectorObjects;  duplicateFeatureVectorObjects = NULL;
  return  duplicateImageFeaturesObjects;
}  /* ExtractDuplicatesByRootImageFileName */



PostLarvaeFVListPtr   PostLarvaeFVList::ExtractExamplesForAGivenClass (MLClassPtr  _mlClass,
                                                                        kkint32     _maxToExtract,
                                                                        float       _minSize
                                                                       )  const
{
  FeatureVectorListPtr  featureVectorList = FeatureVectorList::ExtractExamplesForAGivenClass (_mlClass, _maxToExtract, _minSize);
  PostLarvaeFVListPtr  imageFeaturesList = new PostLarvaeFVList (*featureVectorList);
  featureVectorList->Owner (false);
  delete  featureVectorList;  featureVectorList = NULL;
  return  imageFeaturesList;
}  /*  ExtractExamplesForAGivenClass  */



PostLarvaeFVListPtr  PostLarvaeFVList::StratifyAmoungstClasses (MLClassListPtr  mlClasses,
                                                                kkint32         maxImagesPerClass,
                                                                kkint32         numOfFolds,
                                                                RunLog&         log
                                                               )
{
  FeatureVectorListPtr  stratifiedFeatureVectors = FeatureVectorList::StratifyAmoungstClasses (mlClasses, maxImagesPerClass, numOfFolds, log);
  PostLarvaeFVListPtr  stratifiedImagefeatures  = new PostLarvaeFVList (*stratifiedFeatureVectors);
  stratifiedFeatureVectors->Owner (false);
  delete stratifiedFeatureVectors;  stratifiedFeatureVectors = NULL;
  return  stratifiedImagefeatures;
}  /* StratifyAmoungstClasses */



PostLarvaeFVListPtr  PostLarvaeFVList::StratifyAmoungstClasses (kkint32  numOfFolds,
                                                                RunLog&  log
                                                               )
{
  MLClassListPtr  classes = ExtractListOfClasses ();

  FeatureVectorListPtr  stratifiedFeatureVectors = FeatureVectorList::StratifyAmoungstClasses (classes, -1, numOfFolds, log);
  PostLarvaeFVListPtr  stratifiedImagefeatures  = new PostLarvaeFVList (*stratifiedFeatureVectors);
  
  stratifiedFeatureVectors->Owner (false);

  delete stratifiedFeatureVectors;  stratifiedFeatureVectors = NULL;
  delete  classes;                  classes                  = NULL;

  return  stratifiedImagefeatures;
}  /* StratifyAmoungstClasses */
