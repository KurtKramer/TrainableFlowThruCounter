#include  "StdAfx.h"
#include  "FirstIncludes.h"

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
#include "..\\KKBase\\GoalKeeper.h"
#include "OSservices.h"
#include "Raster.h"
using namespace KKB;


#include "FactoryFVProducer.h"
#include "FeatureFileIo.h"
#include "FeatureFileIOKK.h"
#include "FileDesc.h"
#include "MLClass.h"
using namespace  KKMLL;

#include "PostLarvaeFVProducer.h"
using namespace  CounterUnManaged;

#include "UmiClassList.h"
#include "UmiKKStr.h"
#include "UmiRaster.h"
#include "UmiRunLog.h"
#include "UmiFeatureVector.h"
#include "UmiFeatureVectorList.h"
#include "UmiOSservices.h"
using namespace  CounterManaged;


using namespace System;
using namespace System::Threading;
using namespace System::Windows::Forms;



UmiFeatureVectorList::UmiFeatureVectorList ():
  cancelFlag            (new bool (false)),
  classes               (NULL),
  numExamplesWritten    (NULL),
  sortedByImageFileName (false)
{
}




UmiFeatureVectorList::UmiFeatureVectorList (CounterFeatureVectorList&  examples): // Will take ownership of "examples"  contents.
  cancelFlag            (new bool (false)),
  classes               (NULL),
  numExamplesWritten    (NULL),
  sortedByImageFileName (false)
{
  PostLarvaeFVList::iterator  idx;
  for  (idx = examples.begin ();  idx != examples.end ();  idx++)
  {
    PostLarvaeFVPtr  example = *idx;
    UmiFeatureVector^  pfv = gcnew UmiFeatureVector (example);
    Add (pfv);
  }

  examples.Owner (false);
}




UmiFeatureVectorList::UmiFeatureVectorList (FeatureVectorList&  examples)
{
  UmiFeatureVector^  pfv = nullptr;

  while  (examples.size () > 0)
  {
    FeatureVectorPtr  fv = examples.PopFromFront ();
    //if  (strcmp (fv->UnderlyingClass (), "PostLarvaeFV") == 0)
    if  (typeid (*fv) == typeid (PostLarvaeFV))
    {
      pfv = gcnew UmiFeatureVector (dynamic_cast<PostLarvaeFVPtr> (fv));
      fv = NULL;
    }
    else
    {
      PostLarvaeFVPtr  featureVector = new PostLarvaeFV (*fv);
      delete  fv;  fv = NULL;
      pfv = gcnew UmiFeatureVector (featureVector);  // 'pfv' will take ownership of 'featureVector'.
    }

    Add (pfv);
  }
}






UmiFeatureVectorList::~UmiFeatureVectorList ()
{
  Clear ();
  this->!UmiFeatureVectorList ();
}



UmiFeatureVectorList::!UmiFeatureVectorList ()
{
  CleanUpUnmanagedResources ();
}



void  UmiFeatureVectorList::CleanUpUnmanagedResources ()
{
  delete  classes;             classes    = NULL;
  delete  cancelFlag;          cancelFlag = NULL;
  delete  numExamplesWritten;  numExamplesWritten = NULL;
}  /* CleanUpUnmanagedResources */




void  UmiFeatureVectorList::CancelLoad ()  
{
  if  (!cancelFlag)
    cancelFlag = new bool (true);
  *cancelFlag = true;
}



kkuint32  UmiFeatureVectorList::NumExamplesWritten::get ()
{
  if  (numExamplesWritten)
    return *numExamplesWritten;
  else
    return 0;
}




UmiFeatureVectorList^   UmiFeatureVectorList::CreateListForAGivenLevel (kkuint32      level,
                                                                        UmiRunLog^  runLog
                                                                       )
{
  UmiFeatureVectorList^  specificLevelList = gcnew UmiFeatureVectorList ();

  for  (int x = 0;  x < this->Count;  x++)
  {
    UmiFeatureVector^  pfv = (*this)[x];

    PostLarvaeFVPtr  fvForLevel = new PostLarvaeFV (*(pfv->Features ()));
    MLClassPtr  classForLevel = fvForLevel->MLClass ()->MLClassForGivenHierarchialLevel (level);
    fvForLevel->MLClass (classForLevel);
    specificLevelList->Add (gcnew UmiFeatureVector (fvForLevel));
  }
  return  specificLevelList;
}  /* CreateListForAGivenLevel */





void  UmiFeatureVectorList::LoadInSubDirectoryTree 
                                       (String^     rootDir,
                                        bool        useDirectoryNameForClassName,
                                        UmiRunLog^  log,
                                        bool        rewiteRootFeatureFile
                                       )
{
  FactoryFVProducerPtr  fvProducerFactory = PostLarvaeFVProducerFactory::Factory (&(log->Log ()));
  FeatureFileIOPtr  ioDriver = fvProducerFactory->DefaultFeatureFileIO ();

  if  (classes)
  {
    delete  classes;
    classes = NULL;
  }

  classes = new MLClassList ();

  if  (!cancelFlag)
    cancelFlag = new bool (false);

  FeatureVectorListPtr  featureData = ioDriver->LoadInSubDirectoryTree 
                                     (fvProducerFactory,
                                      UmiKKStr::SystemStringToKKStr (rootDir),
                                      *classes,
                                      useDirectoryNameForClassName,
                                      *cancelFlag,
                                      rewiteRootFeatureFile,
                                      log->Log ()
                                     );

  if  (*cancelFlag)
  {
    delete  featureData;
    featureData = NULL;
    return;
  }

  PostLarvaeFVListPtr  larcosFeatureData = NULL;

  if  (typeid (featureData) == typeid (PostLarvaeFVList))
  {
    larcosFeatureData = dynamic_cast<PostLarvaeFVList*>(featureData);
    featureData= NULL;
  }
  else
  {
    larcosFeatureData = new PostLarvaeFVList (*featureData, true);
    delete  featureData;
    featureData= NULL;
  }

  featureData->Owner (false);  // The ownership of the individual ImageFeature objects will be taken over by 
                               // this container object "UmiFeatureVectorList".

  PostLarvaeFVList::iterator  idx;
  for  (idx = larcosFeatureData->begin ();  idx != larcosFeatureData->end ();  idx++)
  {
    PostLarvaeFVPtr fv = *idx;
    UmiFeatureVector^   pfv = gcnew UmiFeatureVector (fv);  // We just turned ownership of 'fv' over to 'UmiFeatureVectorList'
    Add (pfv);
  }
  delete  larcosFeatureData;
  larcosFeatureData = NULL;
  return;
}  /* LoadInSubDirectoryTree */



UmiClassList^  UmiFeatureVectorList::ExtractListOfClasses ()
{
  UmiClassList^  classes = gcnew UmiClassList ();
  for each (UmiFeatureVector^ pfv in (*this))
  {
    if  (classes->LookUpIndex (pfv->MLClass) < 0)
      classes->Add (pfv->MLClass);
  }

  return  classes;
} /* ExtractListOfClasses */



void  UmiFeatureVectorList::RandomizeOrder ()
{
  int  numExamples = Count;

  Random^  randGen = gcnew Random ();
  for  (int x = 0;  x < Count;  x++)
  {
    int y = randGen->Next (numExamples);
    UmiFeatureVector^  temp = (*this)[x];
    (*this)[x] = (*this)[y];
    (*this)[y] = temp;
  }
}  /* RandomizeOrder */



UmiFeatureVectorList^  UmiFeatureVectorList::ExtractExamplesForAGivenClass (UmiClass^  mlClass)
{
  UmiFeatureVectorList^  extractedExamples = gcnew UmiFeatureVectorList ();

  for each (UmiFeatureVector^  pfv  in (*this))
  {
    if  (pfv->MLClass == mlClass)
      extractedExamples->Add (pfv);
  }

  return  extractedExamples;
}  /* ExtractExamplesForAGivenClass */




void  UmiFeatureVectorList::AddQueue (UmiFeatureVectorList^ subQueue)
{
  for each (UmiFeatureVector^  pfv in subQueue)
    Add (pfv);
}  /* AddQueue */




UmiFeatureVectorList^  UmiFeatureVectorList::StratifyAmoungstClasses (int numOfFolds)
{
  int  foldNum = 0;
  UmiFeatureVectorList^  stratifiedExamples = gcnew UmiFeatureVectorList ();

  UmiClassList^  classes = ExtractListOfClasses ();

  array<UmiFeatureVectorList^>^ examplesEachFold = gcnew array<UmiFeatureVectorList^> (numOfFolds);
  for  (foldNum = 0;  foldNum < numOfFolds;  foldNum++)
   examplesEachFold[foldNum] = gcnew UmiFeatureVectorList ();

  for each (UmiClass^  pc in classes)
  {
    UmiFeatureVectorList^  examplesThisClass = ExtractExamplesForAGivenClass (pc);
    examplesThisClass->RandomizeOrder ();
    foldNum = 0;
    for each (UmiFeatureVector^  pfv in examplesThisClass)
    {
      if  (foldNum >= numOfFolds)
        foldNum = 0;
      examplesEachFold[foldNum]->Add (pfv);
      foldNum++;
    }

    examplesThisClass = nullptr;
  }

  for  (foldNum = 0;  foldNum < numOfFolds;  foldNum++)
  {
    UmiFeatureVectorList^ examplesThisFold = examplesEachFold[foldNum];
    examplesThisFold->RandomizeOrder ();
    stratifiedExamples->AddQueue (examplesThisFold);
    examplesThisFold = nullptr;
    examplesEachFold[foldNum] = nullptr;
  }

  examplesEachFold = nullptr;

  return  stratifiedExamples;
}  /* StratifyAmoungstClasses */




ref class  UmiFeatureVectorList::FeatureVectorComparitorByImageFileName : public System::Collections::Generic::IComparer<UmiFeatureVector^>
{
public:
  virtual  int  Compare (UmiFeatureVector^ a, 
                         UmiFeatureVector^ b
                        )
  {
    String^ sA = UmiOSservices::GetRootName (a->ExampleFileName);
    String^ sB = UmiOSservices::GetRootName (b->ExampleFileName);
    return  sA->CompareTo (sB);
  }
};





void  UmiFeatureVectorList::SortByImageFileName ()
{
  FeatureVectorComparitorByImageFileName^ c = gcnew FeatureVectorComparitorByImageFileName ();
  Sort (c);
  sortedByImageFileName = true;
}




UmiFeatureVector^  UmiFeatureVectorList::LookUpByImageFileName (String^  imageFileName)
{
  if  (!sortedByImageFileName)
    SortByImageFileName ();

  String^  imageFileRootName = UmiOSservices::GetRootName (imageFileName);

  int  left = 0;
  int  right = Count - 1;
  int  middle = -1;

  UmiFeatureVector^  fv = nullptr;

  FeatureVectorComparitorByImageFileName^  comparer = gcnew FeatureVectorComparitorByImageFileName ();

  while  (left < right)
  {
    middle = (left + right) / 2;
    String^  middleRootName = UmiOSservices::GetRootName ((this)[middle]->ExampleFileName);
    int  x = middleRootName->CompareTo (imageFileRootName);

    if  (x < 0)
      left = middle + 1;

    else if  (x > 0)
      right = middle - 1;

    else
    {
      fv = (this)[middle];
      break;
    }
  }

  return  fv;
}  /* LookUpByImageFileName */



// Creates an unmanaged list of feature vectors.
FeatureVectorListPtr  UmiFeatureVectorList::ToFeatureVectorList (UmiRunLog^  runLog)
{
  FeatureVectorListPtr  fvl = new FeatureVectorList (PostLarvaeFV::PostLarvaeFeaturesFileDesc (), false);

  for each (UmiFeatureVector^ fv in *this)
    fvl->PushOnBack (fv->UnManagedClass ());

  return  fvl;
}  /* ToFeatureVectorList */




void  UmiFeatureVectorList::Save (String^       fileName,
                                  UmiRunLog^  runLog
                                 )
{
  FeatureVectorListPtr  fvl = ToFeatureVectorList (runLog);
  bool  successful  = false;

  if  (!numExamplesWritten)
    numExamplesWritten = new kkuint32;
  *numExamplesWritten = 0;

  try
  {
    FeatureFileIOKK::Driver ()->SaveFeatureFile 
                  (UmiKKStr::SystemStringToKKStr (fileName), 
                   fvl->AllFeatures (), 
                   *fvl,
                   *numExamplesWritten,
                   *cancelFlag,
                   successful,
                   runLog->Log ()
                  );
  }
  catch  (Exception^  e)  
  {
    throw  gcnew Exception ("UmiFeatureVectorList::Save", e);
  }

  if (!successful)
    throw  gcnew Exception ("UmiFeatureVectorList::Save");

}  /* Save */

