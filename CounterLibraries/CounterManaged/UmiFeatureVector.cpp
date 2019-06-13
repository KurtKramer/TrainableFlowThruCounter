#include "StdAfx.h"
#include "FirstIncludes.h"
#include <stdio.h>
#include <math.h>
#include <ctype.h>
#include <time.h>
#include <fstream>
#include <iostream>
#include <ostream>
#include <string>
#include <vector>
#include "MemoryDebug.h"

#include "KKBaseTypes.h"
#include "GoalKeeper.h"
#include "ImageIO.h"
#include "OSservices.h"
#include "Raster.h"
using namespace KKB;

#include "FeatureVector.h"
#include "FileDesc.h"
#include "MLClass.h"
using namespace  KKMLL;

#include "FeatureFileIOKK.h"
#include "CounterFVProducer.h"
using namespace  CounterUnManaged;

#include "UmiFeatureVector.h"
#include "UmiClassList.h"
#include "UmiKKStr.h"
#include "UmiRaster.h"
#include "UmiRunLog.h"
using namespace  CounterManaged;


using namespace System;
using namespace System::Threading;
using namespace System::Windows::Forms;


#define  MemPreasPerFV  430


UmiFeatureVector::UmiFeatureVector (UmiFeatureVector^  umiFeatureVector):
    features (NULL),
    mlClass (nullptr)
{
  features = new CounterFeatureVector (*(umiFeatureVector->Features ()));
  mlClass  = UmiClassList::GetUniqueClass (features->MLClass ());

  GC::AddMemoryPressure (MemPreasPerFV);
}



UmiFeatureVector::UmiFeatureVector (UmiRaster^       raster,
                                    System::String^  imageFileName,
                                    UmiRasterList^   intermediateImages,
                                    bool             saveDebugImages,
                                    UmiRunLog^       log
                                   ):
   mlClass (nullptr)
{
  mlClass = UmiClassList::GetUnKnownClassStatic ();
  RasterPtr r = raster->UnmanagedClass ();
  if  (r == NULL)
  {
    features = new CounterFeatureVector (CounterFVProducer::MaxNumOfFeatures ());
    return;
  }
  if  (imageFileName != nullptr)
    r->FileName (UmiKKStr::SystemStringToKKStr (imageFileName));
  RasterListPtr  tempIntermediateImages = NULL;
  if  (intermediateImages != nullptr)
    tempIntermediateImages = new RasterList (true);
  CounterFVProducerPtr fvp = CounterFVProducerFactory::Factory (&(log->Log ()))->ManufactureInstance (log->Log ());
  features = fvp->ComputeFeatureVector (*r, mlClass->UnmanagedImageClass (), tempIntermediateImages, 1.0, log->Log ());

  if  (saveDebugImages)
  {
    try {
      FileDescConstPtr  fd = fvp->FileDesc ();
      KKStr  rootDir  = "C:\\Temp\\CounterFeatureComputationDebugging\\";
      KKStr  rootName = osGetRootName (UmiKKStr::SystemStringToKKStr (imageFileName)) + "_ScannerFile";
      KKStr  fullFN = rootDir + rootName + ".bmp";
      KKB::SaveImageGrayscaleInverted8Bit (*r, fullFN);

      KKStr  fdFN = rootDir + rootName + ".txt";
      std::ofstream o (fdFN.Str ());
      o << "rootName" << "\t" << "priorReductionFactor" << "\t" << "predictedClass"        << "\t" << "probability";
      for  (uint zed = 0;  zed < fd->NumOfFields ();  ++zed)
        o << "\t" << fd->FieldName (zed);
      o << std::endl;
      o <<  rootName  << "\t" <<  ""  << "\t" << "" << "\t" << "0.0";
      for  (kkuint32 zed2 = 0;  zed2 < features->NumOfFeatures ();  ++zed2)
        o << "\t" << features->FeatureData (zed2);
      o << std::endl;
      o.close();
    }
    catch (const std::exception& e)  {
      String^ errMsg = gcnew String(e.what());
      log->WriteLine("\nUmiFeatureVector::UmiFeatureVector  ***ERROR***   Exception: " + errMsg + "\n");
    }
    catch (...) {
      log->WriteLine("\nUmiFeatureVector::UmiFeatureVector  ***ERROR***   Exception Thrown\n");
    }
  }

  UmiRasterList::CopyOverIntermediateImages (tempIntermediateImages, intermediateImages);
  delete  fvp;
  fvp = NULL;
  GC::AddMemoryPressure (MemPreasPerFV);
}



UmiFeatureVector::UmiFeatureVector (System::Array^   raster,
                                    System::String^  imageFileName,
                                    UmiRunLog^       log
                                   ):
   mlClass (nullptr)
{
  mlClass = UmiClassList::GetUnKnownClassStatic ();
  RasterPtr r = UmiRaster::BuildRasterObj (raster);
  if  (r == NULL)
  {
    features = new CounterFeatureVector (CounterFVProducer::MaxNumOfFeatures ());
    return;
  }
  if  (imageFileName != nullptr)
    r->FileName (UmiKKStr::SystemStringToKKStr (imageFileName));
  CounterFVProducerPtr fvp = CounterFVProducerFactory::Factory (&(log->Log ()))->ManufactureInstance (log->Log ());
  features = fvp->ComputeFeatureVector (*r, mlClass->UnmanagedImageClass (), NULL, 1.0f, log->Log ());
  delete  fvp;  fvp = NULL;
  delete  r;    r   = NULL;
  GC::AddMemoryPressure (MemPreasPerFV);
}



UmiFeatureVector::UmiFeatureVector (FeatureVectorPtr  _features):
   mlClass (nullptr)
{
  if  (_features == NULL)
    throw gcnew Exception ("UmiFeatureVector   NULL passed into constructor.");

  features = _features;
  mlClass = UmiClassList::GetUniqueClass (UmiKKStr::KKStrToSystenStr (features->MLClass ()->Name ()), String::Empty);
  GC::AddMemoryPressure (MemPreasPerFV);
}



UmiFeatureVector::UmiFeatureVector (String^         _imageFileName,
                                    UmiClass^       _mlClass,
                                    UmiRasterList^  _intermediateImages,
                                    UmiRunLog^      log
                                   )
{
  bool  successful = false;
  if  (mlClass == nullptr)
    mlClass = UmiClassList::GetUnKnownClassStatic ();

  RasterListPtr  tempIntermediateImages = NULL;
  if  (_intermediateImages != nullptr)
    tempIntermediateImages = new RasterList (true);
  CounterFVProducerPtr fvp = CounterFVProducerFactory::Factory (&(log->Log ()))->ManufactureInstance (log->Log ());
  features = fvp->ComputeFeatureVectorFromImage (UmiKKStr::SystemStringToKKStr (_imageFileName), mlClass->UnmanagedImageClass (), tempIntermediateImages, log->Log ());
  UmiRasterList::CopyOverIntermediateImages (tempIntermediateImages, _intermediateImages);

  delete  fvp;
  fvp = NULL;
  GC::AddMemoryPressure (MemPreasPerFV);
}



UmiFeatureVector::~UmiFeatureVector ()
{
  mlClass = nullptr;
  this->!UmiFeatureVector ();
}



UmiFeatureVector::!UmiFeatureVector ()
{
  CleanUpUnmanagedResources ();
}



void  UmiFeatureVector::CleanUpUnmanagedResources ()
{    
  delete  features;  features = NULL;
  GC::RemoveMemoryPressure (MemPreasPerFV);
}



String^  UmiFeatureVector::ClassName::get ()  
{
  if  (mlClass != nullptr)
    return  mlClass->Name;
  if  (features == NULL)
    return  "";
  return  UmiKKStr::KKStrToSystenStr (features->MLClassName ());
}



UmiClass^   UmiFeatureVector::MLClass::get ()
{
  if  (mlClass != nullptr)
    return mlClass;
  if  (!features)
    mlClass = UmiClassList::GetUnKnownClassStatic ();
  else
    mlClass = UmiClassList::GetUniqueClass (features->MLClass ());
  return  mlClass;
}



String^  UmiFeatureVector::ExampleFileName::get ()  
{
  if  (!features)
    return "";
  return  UmiKKStr::KKStrToSystenStr (features->ExampleFileName ());
}



uint  UmiFeatureVector::NumFeatures::get ()
{
  if  (!features)
    return 0;
  return  features->NumOfFeatures ();
}



float  UmiFeatureVector::OrigSize::get ()
{
  if  (!features)
    return 0.0f;
  return  features->OrigSize ();
}



float  UmiFeatureVector::Probability::get ()
{
  if  (!features)
    return 0.0f;
  return  features->Probability ();
}



bool   UmiFeatureVector::Validated::get ()
{
  if  (!features)
    return false;
  return features->Validated ();
}



void  UmiFeatureVector::MLClass::set (UmiClass^  umiClass)
{
  if  (umiClass == nullptr)
    umiClass = UmiClassList::GetUnKnownClassStatic ();
  mlClass = umiClass;
  if  (!features)
    return;
   features->MLClass (umiClass->UnmanagedImageClass ());
}



void  UmiFeatureVector::ExampleFileName::set (String^ _imageFileName)
{
  if  (!features)
    return;

  features->ExampleFileName (UmiKKStr::SystemStringToKKStr (_imageFileName));
}



void  UmiFeatureVector::Probability::set (float _probability)
{
  if  (features)
  {
    features->Probability (_probability);
  }
}



void   UmiFeatureVector::Validated::set (bool  _validated)
{
  if  (features)
    features->Validated (_validated);
}



bool   UmiFeatureVector::FeatureDataMissing::get ()
{
  return  (features->FeatureData (0) == 0.0f);   // Feature 0 is the Size feature;  this feature has to have a value greater than 0.0
}



String^  UmiFeatureVector::FeatureName (uint featureNum)
{
  return  UmiKKStr::KKStrToSystenStr (PostLarvaeFV::FeatureName (featureNum));
}



float  UmiFeatureVector::FeatureValue (kkuint32 featureNum)
{
  KKCheck (features, "UmiFeatureVector::FeatureValue    'features' is undefined!!!")

  KKCheck (featureNum < features->NumOfFeatures (), "UmiFeatureVector::FeatureValue   featureNum: " << featureNum << " exceeds NumOfFeatures: " << features->NumOfFeatures ())
    return 0.0;

  return  features->FeatureData (featureNum);
}



ParsedImageFileName^  UmiFeatureVector::ParseImageFileName (System::String^  _fullFileName)
{
  KKStr  fullFileName = UmiKKStr::SystemStringToKKStr (_fullFileName);

  KKStr  scannerFileName (30);
  kkuint32 scanLineNum;
  kkuint32 scanColNum;

  PostLarvaeFV::ParseImageFileName (fullFileName, scannerFileName, scanLineNum, scanColNum);


  System::String^ _scannerFileName = UmiKKStr::KKStrToSystenStr (scannerFileName);

  return  gcnew ParsedImageFileName (_scannerFileName, scanLineNum, scanColNum);

}  /* ParseImageFileName */



void  UmiFeatureVector::ParseImageFileName (String^    _fullFileName,
                                            String^%   _scannerFileName,
                                            kkuint32%  _scanLine,
                                            kkuint32%  _scanCol
                                           )
{
  KKStr  fullFileName = UmiKKStr::SystemStringToKKStr (_fullFileName);

  KKStr  scannerFileName (64);
  kkuint32 scanLine;
  kkuint32 scanCol;

  PostLarvaeFV::ParseImageFileName (fullFileName, scannerFileName, scanLine, scanCol);

  _scannerFileName = UmiKKStr::KKStrToSystenStr (scannerFileName);
  _scanLine = scanLine;
  _scanCol  = scanCol;
}  /* ParseImageFileName */
