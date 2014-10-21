#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Diagnostics;


#include  "PostLarvaeFV.h"
using namespace  KKMachineLearning;

#include  "UmiClass.h"
#include  "UmiClassList.h"
#include  "UmiRaster.h"

#include  "UmiRunLog.h"


namespace LarcosCounterManaged {
  public  ref class  ParsedImageFileName
  {
  public:
    ParsedImageFileName (System::String^  _scannerFileName, 
                         kkuint32         _scanLineNum,  
                         kkuint32         _scanColNum
                        )
    {
      scannerFileName = _scannerFileName;
      scanLineNum    = _scanLineNum;
      scanColNum     = _scanColNum;
    }

     System::String^  scannerFileName;
     kkuint32         scanLineNum; 
     kkuint32         scanColNum;
  };



  public  ref class UmiFeatureVector
  {
  public:
    //**************************************************************************
    //* Will create a new instance of "features" that will be owned by the new *
    //* instance of "UmiFeatureVector" that is created.                      *
    //**************************************************************************
    UmiFeatureVector (UmiFeatureVector^  umiFeatureVector);  


    
    //**************************************************************************
    //* Will calculate features off the image that is in "raster"; see         *
    //* ScsLibrary::PostLarvaeFV::CalcFeatures.  Features that are            *
    //* based off instrumentation data will come from the IntrumentDataManager *
    //* the "imageFileName" will be parsed to get ScannerFileName and ScanLine. *
    //**************************************************************************
    UmiFeatureVector (System::Array^   raster,
                      System::String^  imageFileName,
                      UmiRunLog^       log                        
                     );


    //**************************************************************************
    //* Will calculate features off the image that is in "raster"; see         *
    //* ScsLibrary::PostLarvaeFV::CalcFeatures.  Features that are            *
    //* based off instrumentation data will come from the IntrumentDataManager *
    //* the "imageFileName" will be parsed to get ScannerFileName and ScanLine. *
    //**************************************************************************
    UmiFeatureVector (UmiRaster^       raster,
                      System::String^  imageFileName,
                      UmiRasterList^   intermediateImages,
                      UmiRunLog^       log
                     );

    UmiFeatureVector (PostLarvaeFVPtr  _features);  // will take ownership of '_features'  from caller

    
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
    property String^    ImageFileName      {String^     get ();  void  set (String^    _imageFileName);}
    property int        NumFeatures        {int         get ();}
    property float      OrigSize           {float       get ();}
    property float      Probability        {float       get ();  void  set (float      _probability);}
    property bool       Validated          {bool        get ();  void  set (bool       _validated);}


    PostLarvaeFVPtr  Features ()  {return  features;}


    String^  FeatureName (int featureNum);


    double  FeatureValue (int featureNum);


    static  ParsedImageFileName^  ParseImageFileName (System::String^  fullFileName);


    static  void  ParseImageFileName (String^   _fullFileName,
                                      String^%  _scannerFileName,
                                      kkuint32%     _scanLine,
                                      kkuint32%     _scanCol
                                     );

    PostLarvaeFVPtr  UnManagedClass ()  {return  features;}



  private:
    void  CleanUpUnmanagedResources ();


    PostLarvaeFVPtr  features;
    UmiClass^        mlClass;
  };  /* UmiFeatureVector */
}  /* LarcosCounterManaged */