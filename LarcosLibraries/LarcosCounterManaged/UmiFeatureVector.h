#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Diagnostics;


//#include "PostLarvaeFV.h"
#include "FeatureVector.h"
using namespace  KKMLL ;


#include "UmiClass.h"
#include "UmiClassList.h"
#include "UmiRaster.h"

#include "UmiRunLog.h"


namespace LarcosCounterManaged 
{
  public  ref class  ParsedImageFileName
  {
  public:
    ParsedImageFileName (System::String^  _scannerFileName, 
                         kkuint32         _scanLineNum,  
                         kkuint32         _scanColNum
                        )
    {
      scannerFileName = _scannerFileName;
      scanLineNum     = _scanLineNum;
      scanColNum      = _scanColNum;
    }

     System::String^  scannerFileName;
     kkuint32         scanLineNum; 
     kkuint32         scanColNum;
  };



  public  ref class UmiFeatureVector
  {
  public:

    ///<summary>
    /// Will create a new instance of "features" that will be owned by the new *
    /// instance of "UmiFeatureVector" that is created.                      *
    ///</summary>
    UmiFeatureVector (UmiFeatureVector^  umiFeatureVector);  



    ///<summary>
    /// Will calculate features off the image that is in "raster"; see
    /// LarcosFVProducer::ComputeFeatureVector.  Features that are
    /// based off instrumentation data will come from the IntrumentDataManager
    /// the "imageFileName" will be parsed to get ScannerFileName and ScanLine.
    ///</summary>
    UmiFeatureVector (System::Array^   raster,
                      System::String^  imageFileName,
                      UmiRunLog^       log                        
                     );


    ///<summary>
    /// Will calculate features off the image that is in "raster"; see
    /// LarcosFVProducer::ComputeFeatureVector.  Features that are
    /// based off instrumentation data will come from the IntrumentDataManager
    /// the "imageFileName" will be parsed to get ScannerFileName and ScanLine.
    ///</summary>
    UmiFeatureVector (UmiRaster^       raster,
                      System::String^  imageFileName,
                      UmiRasterList^   intermediateImages,
                      bool             saveDebugImages,
                      UmiRunLog^       log
                     );

    UmiFeatureVector (FeatureVectorPtr  _features);  /**< will take ownership of '_features'  from caller  */

    
    UmiFeatureVector (String^         _imageFileName,
                      UmiClass^       _mlClass,
                      UmiRasterList^  _intermediateImages,
                      UmiRunLog^      log
                     );

  private:
    ~UmiFeatureVector ();

  protected:
    !UmiFeatureVector ();


  public:
    property String^    ClassName          {String^     get ();}
    property bool       FeatureDataMissing {bool        get ();}  // If feature values are 0.0f then will return 'true' else 'false'.
    property UmiClass^  MLClass            {UmiClass^   get ();  void  set (UmiClass^  _mlClass);}
    property String^    ExampleFileName    {String^     get ();  void  set (String^    _imageFileName);}
    property uint       NumFeatures        {uint        get ();}
    property float      OrigSize           {float       get ();}
    property float      Probability        {float       get ();  void  set (float      _probability);}
    property bool       Validated          {bool        get ();  void  set (bool       _validated);}

    FeatureVectorPtr  Features ()  {return  features;}


    String^  FeatureName (uint featureNum);


    float  FeatureValue (kkuint32 featureNum);


    static  ParsedImageFileName^  ParseImageFileName (System::String^  fullFileName);


    static  void  ParseImageFileName (String^    _fullFileName,
                                      String^%   _scannerFileName,
                                      kkuint32%  _scanLine,
                                      kkuint32%  _scanCol
                                     );

    FeatureVectorPtr  UnManagedClass ()  {return  features;}

  private:
    void  CleanUpUnmanagedResources ();

    FeatureVectorPtr  features;
    UmiClass^         mlClass;
  };  /* UmiFeatureVector */
}  /* LarcosCounterManaged */
