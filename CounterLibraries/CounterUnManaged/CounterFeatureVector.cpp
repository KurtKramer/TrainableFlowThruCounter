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

#include "FactoryFVProducer.h"
#include "FeatureNumList.h"
#include "FeatureFileIO.h"
#include "FeatureFileIOKK.h"
#include "CounterFVProducer.h"
#include "MLClass.h"
#include "ImageDirTree.h"
using namespace  KKMLL;


#include "CounterFeatureVector.h"
using namespace  CounterUnManaged;
                      


CounterFeatureVector::CounterFeatureVector (kkint32  _numOfFeatures):
       FeatureVector (_numOfFeatures),

        centroidCol     (-1),
        centroidRow     (-1),
        numOfEdgePixels (-1)
{
}



CounterFeatureVector::CounterFeatureVector (const CounterFeatureVector&  _fv):
  FeatureVector (_fv),

  centroidCol     (_fv.centroidCol),
  centroidRow     (_fv.centroidRow),
  numOfEdgePixels (_fv.numOfEdgePixels)
{
}





CounterFeatureVector::CounterFeatureVector (const FeatureVector&  featureVector):
   FeatureVector    (featureVector),
   centroidCol      (-1),
   centroidRow      (-1),
   numOfEdgePixels  (-1)

{
  if  (typeid (featureVector) == typeid(CounterFeatureVector))
  {
    // The underlying class is another CounterFeatureVector object.
    const CounterFeatureVector&  example = dynamic_cast<const CounterFeatureVector&>(featureVector);

    centroidCol      = example.CentroidCol     ();
    centroidRow      = example.CentroidRow     ();
    numOfEdgePixels  = example.NumOfEdgePixels ();
  }
}



CounterFeatureVector::~CounterFeatureVector ()
{
}




CounterFeatureVectorPtr  CounterFeatureVector::Duplicate ()  const
{
  return new CounterFeatureVector (*this);
}




CounterFeatureVectorList::CounterFeatureVectorList (FileDescConstPtr  _fileDesc,
                                                  bool              _owner
                                                 ):
    FeatureVectorList (_fileDesc, _owner)
{

}




CounterFeatureVectorList::CounterFeatureVectorList (FactoryFVProducerPtr  _fvProducerFactory,
                                                  MLClassPtr            _mlClass,
                                                  KKStr                 _dirName,
                                                  KKStr                 _fileName,
                                                  RunLog&               _log
                                                 ):

   FeatureVectorList (_fvProducerFactory->FileDesc (), true)

{
  FeatureExtraction (_fvProducerFactory, _dirName, _fileName, _mlClass, _log);
}




CounterFeatureVectorList::CounterFeatureVectorList (const CounterFeatureVectorList&  examples):
   FeatureVectorList (examples.FileDesc (), examples.Owner ())
{
  const_iterator  idx;
  for  (idx = examples.begin ();  idx != examples.end ();  idx++)
  {
    const CounterFeatureVectorPtr  imageExample = *idx;
    if  (Owner ())
      PushOnBack (new CounterFeatureVector (*imageExample));
    else
      PushOnBack (imageExample);
  }
}




CounterFeatureVectorList::CounterFeatureVectorList (const CounterFeatureVectorList&  examples,
                                                  bool                           _owner
                                                 ):

   FeatureVectorList (examples.FileDesc (), _owner)
{
  const_iterator  idx;
  for  (idx = examples.begin ();  idx != examples.end ();  idx++)
  {
    const CounterFeatureVectorPtr  imageExample = *idx;
    if  (Owner ())
      PushOnBack (new CounterFeatureVector (*imageExample));
    else
      PushOnBack (imageExample);
  }
}


CounterFeatureVectorList::CounterFeatureVectorList (const FeatureVectorList&  featureVectorList,
                                                    bool                      _owner
                                                   ):

  FeatureVectorList (featureVectorList.FileDesc (), _owner)
{
  if  (_owner)
  {
    for (auto idx: featureVectorList)
    {
      FeatureVectorPtr featureVector = idx;
      
      // The constructor below will detect what the underlying type of 'featureVector' is.  
      // If (underlying type is a 'CounterFeatureVector' object)  then
      //   | Information that is particular to a 'CounterFeatureVector' object will be extracted
      //   | from the 'FeatureVector' object being passed in.
      // else
      //   | info that is particular to a 'CounterFeatureVector' object will be set to
      //   | default values.
      CounterFeatureVectorPtr  example = new CounterFeatureVector (*featureVector);
      PushOnBack (example);
    }
  }
  else
  {
    // Since we will not own the contents but just point to an existing instances we will 
    // have to make sure that the existing instances of 'FeatureVector' objects have a 
    // underlying type of 'CounterFeatureVector'.
    for (auto idx : featureVectorList)
    {
      FeatureVectorPtr featureVector = idx;
      if  (typeid (*featureVector) == typeid (CounterFeatureVector))
      {
        CounterFeatureVectorPtr example = dynamic_cast<CounterFeatureVectorPtr>(featureVector);
        PushOnBack (example);
      }
      else
      {
        // ****    ERROR  ****
        KKStr  errMsg;
        errMsg << "CounterFeatureVectorList   ***ERROR***   One Elements not a 'CounterFeatureVector'  type  FileName: '" << featureVector->ExampleFileName () << "'.";
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
CounterFeatureVectorList::CounterFeatureVectorList (MLClassList&               _mlClasses,
                                                    CounterFeatureVectorList&  _examples
                                                   ):
  FeatureVectorList (_mlClasses, _examples)
{
}



CounterFeatureVectorList::CounterFeatureVectorList (const FeatureVectorList&  featureVectorList):
  FeatureVectorList (featureVectorList.FileDesc (),
                     featureVectorList.Owner ()
                    )
{
  if  (featureVectorList.Owner ())
  {
    for  (auto idx: featureVectorList)
    {
      FeatureVectorPtr featureVector = idx;
      
      // The constructor below will detect what the underlying type of 'FeatureVector' is.  
      // If (underlying type is a 'CounterFeatureVector' object)  then
      //   | Information that is particular to a 'CounterFeatureVector' object will be extracted
      //   | from the 'FeatureVector' object being passed in.
      // else
      //   | info that is particular to a 'CounterFeatureVector' object will be set to
      //   | default values.
      CounterFeatureVectorPtr  example = new CounterFeatureVector (*featureVector);
      PushOnBack (example);
    }
  }
  else
  {
    // Since we will not own the contents but just point to existing instances we will 
    // have to make sure that the existing instances of 'FeatureVector' objects have a 
    // underlying type of 'CounterFeatureVector'.
    for  (auto idx: featureVectorList)
    {
      FeatureVectorPtr featureVector = idx;
      if  (typeid (*featureVector) == typeid (CounterFeatureVector))
      {
        CounterFeatureVectorPtr example = dynamic_cast<CounterFeatureVectorPtr>(featureVector);
        PushOnBack (example);
      }
      else
      {
        // ****    ERROR  ****
        KKStr  errMsg;
        errMsg << "CounterFeatureVectorList (const FeatureVectorList&  featureVectorList)   ***ERROR***   "
             << "One element not a 'CounterFeatureVector' type can not recast:  FileName[" << featureVector->ExampleFileName () << "]";
        cerr << endl << errMsg << endl << endl;
        throw KKException (errMsg);
      }
    }
  }
}



CounterFeatureVectorList::~CounterFeatureVectorList ()
{
}



CounterFeatureVectorPtr  CounterFeatureVectorList::IdxToPtr (kkint32 idx)  const
{
  return  (CounterFeatureVectorPtr)FeatureVectorList::IdxToPtr (idx);
}  /* IdxToPtr */



CounterFeatureVectorPtr  CounterFeatureVectorList::BackOfQueue ()
{
  if  (size () < 1)
    return NULL;

  FeatureVectorPtr  fv = back ();
  if  (typeid (*fv) == typeid (CounterFeatureVector))
    return  dynamic_cast<CounterFeatureVectorPtr> (fv);

  cerr << endl 
       << "CounterFeatureVectorList::BackOfQueue ()   ***ERROR***   Entry at back of Queue is not a 'CounterFeatureVector' object." << endl
       << endl;

  return NULL;
}  /* BackOfQueue */



CounterFeatureVectorPtr  CounterFeatureVectorList::PopFromBack ()
{
  if  (size () < 1)  return NULL;

  FeatureVectorPtr  fv = back ();
  if  (typeid (*fv) == typeid (CounterFeatureVector))
  {
    cerr << endl
         << "CounterFeatureVectorList::BackOfQueue ()   ***ERROR***   Entry popped from back of Queue is not a 'CounterFeatureVector' object." << endl
         << endl;
    return NULL;
  }

  return  dynamic_cast<CounterFeatureVectorPtr> (FeatureVectorList::PopFromBack ());
}  /* PopFromBack */



void  CounterFeatureVectorList::AddQueue (CounterFeatureVectorList&  imagesToAdd)
{
  FeatureVectorList::AddQueue (imagesToAdd);
}  /* AddQueue */



CounterFeatureVectorPtr  CounterFeatureVectorList::BinarySearchByName (const KKStr&  _imageFileName)  const
{
  return  (CounterFeatureVectorPtr)FeatureVectorList::BinarySearchByName (_imageFileName);
}  /* BinarySearchByName */



CounterFeatureVectorListPtr   CounterFeatureVectorList::ManufactureEmptyList (bool _owner)  const
{
  return new CounterFeatureVectorList (FileDesc (), _owner);
}



CounterFeatureVectorPtr  CounterFeatureVectorList::LookUpByRootName (const KKStr&  _rootName)
{
  return  (CounterFeatureVectorPtr)FeatureVectorList::LookUpByRootName (_rootName);
}  /* LookUpByRootName */



CounterFeatureVectorPtr  CounterFeatureVectorList::LookUpByImageFileName (const KKStr&  _imageFileName)  const
{
  return  (CounterFeatureVectorPtr)FeatureVectorList::LookUpByImageFileName (_imageFileName);
}  /* LookUpByImageFileName */



CounterFeatureVectorListPtr  CounterFeatureVectorList::OrderUsingNamesFromAFile (const KKStr&  orderFileName,
                                                                                 RunLog&       log
                                                                                )
{
  FeatureVectorListPtr  examples = FeatureVectorList::OrderUsingNamesFromAFile (orderFileName, log);
  examples->Owner (false);
  CounterFeatureVectorListPtr  orderedImages = new CounterFeatureVectorList (*examples);
  delete  examples;
  return  orderedImages;
}  /* OrderUsingNamesFromAFile */



void   CounterFeatureVectorList::FeatureExtraction (FactoryFVProducerPtr  _fvProducerFactory,
                                                   KKStr                 _dirName, 
                                                   KKStr                 _fileName, 
                                                   MLClassPtr            _mlClass,
                                                   RunLog&               _log
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

  FeatureVectorProducerPtr  fvProducer = _fvProducerFactory->ManufactureInstance (_log);

  KKStrList::iterator  fnIDX = fileNameList->begin ();

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

    FeatureVectorPtr featureVector = fvProducer->ComputeFeatureVectorFromImage (fullFileName, _mlClass, NULL, _log);
    if  (!featureVector)
    {
      KKStr  msg (100);
      msg << "CounterFeatureVectorList::FeatureExtraction   ***ERROR***   Could not Allocate CounterFeatureVector object" << endl
          << "for FileName[" << fullFileName << "].";
      _log.Level (-1) << endl << msg << endl << endl;
    }
    else
    {
      CounterFeatureVectorPtr  counterFeatureVector = NULL;
      if  (typeid(*featureVector) == typeid(CounterFeatureVector))
      {
        counterFeatureVector = dynamic_cast<CounterFeatureVectorPtr>(featureVector);
        featureVector = NULL;
      }
      else
      {
        counterFeatureVector = new CounterFeatureVector (*featureVector);
        delete  featureVector;
        featureVector = NULL;
      }

      counterFeatureVector->ExampleFileName (*imageFileName);
      _log.Level (30) << counterFeatureVector->ExampleFileName () << "  " << counterFeatureVector->OrigSize () << endl;
      PushOnBack (counterFeatureVector);
      count++;
    }
  }

  delete  fvProducer;  fvProducer = NULL;

  Version (_CounterFVProducer_VersionNum_);

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



CounterFeatureVectorListPtr   CounterFeatureVectorList::Duplicate (bool _owner)  const
{
  return new CounterFeatureVectorList (*this, _owner);
}  /* Duplicate */



/**
 * @brief  Creates a duplicate of list and also duplicates it contents.
 * @return Duplicated list with hard copy of its contents.
 */
CounterFeatureVectorListPtr  CounterFeatureVectorList::DuplicateListAndContents ()  const
{
  CounterFeatureVectorListPtr  copyiedList = new CounterFeatureVectorList (FileDesc (), true);

  for  (kkuint32 idx = 0;  idx < QueueSize ();  idx++)
  {
    CounterFeatureVectorPtr  curImage = (CounterFeatureVectorPtr)IdxToPtr (idx);
    copyiedList->PushOnBack (new CounterFeatureVector (*curImage));
  }
  
  copyiedList->Version (Version ());

  return  copyiedList;
}  /* DuplicateListAndContents */



void  CounterFeatureVectorList::RecalcFeatureValuesFromImagesInDirTree (FactoryFVProducerPtr  fvProducerFactory,  
                                                                        const KKStr&          rootDir,
                                                                        bool&                 successful,
                                                                        RunLog&               log
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

  FeatureVectorProducerPtr  fvProducer = fvProducerFactory->ManufactureInstance (log);

  KKStrConstPtr  dirPath = NULL;

  iterator  idx;
  CounterFeatureVectorPtr  example = NULL;

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

    bool  validFile = false;
    RasterPtr  raster = new Raster (fullFileName, validFile);
    if  (!validFile)
    {
      delete  raster;  raster = NULL;
      log.Level (-1) << "CounterFeatureVectorList::RecalcFeatureValuesFromImagesInDirTree   ***ERROR***  Unable to load image: " << fullFileName << endl << endl;
    }
    else
    {
      FeatureVectorPtr fv = fvProducer->ComputeFeatureVector (*raster, example->MLClass (), NULL, 1.0, log);
      if  (fv)
      {
        kkint32  x;
        kkint32  featureCount = Min (fv->NumOfFeatures (), example->NumOfFeatures ());
        for (x = 0; x < featureCount;  ++x)
          example->FeatureData (x, fv->FeatureData (x));

        example->OrigSize (fv->OrigSize ());
        example->Version (fv->Version ());

        if  (typeid(*fv) == typeid(CounterFeatureVector))
        {
          CounterFeatureVectorPtr  lfv = dynamic_cast<CounterFeatureVectorPtr>(fv);
          example->CentroidCol (lfv->CentroidCol ());
          example->CentroidRow (lfv->CentroidRow ());
          example->NumOfEdgePixels (lfv->NumOfEdgePixels ());
        }

        delete  fv;
        fv = NULL;
      }
      delete  raster;  raster = NULL;
    }
  }

  delete  fvProducer;
  fvProducer = NULL;
}  /* RecalcFeatureValuesFromImagesInDirTree */



CounterFeatureVectorListPtr  CounterFeatureVectorList::ExtractDuplicatesByRootImageFileName ()
{
  FeatureVectorListPtr  duplicateFeatureVectorObjects = FeatureVectorList::ExtractDuplicatesByRootImageFileName ();
  CounterFeatureVectorListPtr  duplicateImageFeaturesObjects = new CounterFeatureVectorList (*duplicateFeatureVectorObjects);
  duplicateFeatureVectorObjects->Owner (false);
  delete  duplicateFeatureVectorObjects;  duplicateFeatureVectorObjects = NULL;
  return  duplicateImageFeaturesObjects;
}  /* ExtractDuplicatesByRootImageFileName */



CounterFeatureVectorListPtr   CounterFeatureVectorList::ExtractExamplesForAGivenClass (MLClassPtr  _mlClass,
                                                                                   kkint32     _maxToExtract,
                                                                                   float       _minSize
                                                                                  )  const
{
  FeatureVectorListPtr  featureVectorList = FeatureVectorList::ExtractExamplesForAGivenClass (_mlClass, _maxToExtract, _minSize);
  CounterFeatureVectorListPtr  imageFeaturesList = new CounterFeatureVectorList (*featureVectorList);
  featureVectorList->Owner (false);
  delete  featureVectorList;  featureVectorList = NULL;
  return  imageFeaturesList;
}  /*  ExtractExamplesForAGivenClass  */



CounterFeatureVectorListPtr  CounterFeatureVectorList::StratifyAmoungstClasses (kkint32  numOfFolds, 
                                                                              RunLog&  log
                                                                             )
{
  MLClassListPtr  classes = ExtractListOfClasses ();

  FeatureVectorListPtr  stratifiedFeatureVectors = FeatureVectorList::StratifyAmoungstClasses (classes, -1, numOfFolds, log);
  CounterFeatureVectorListPtr  stratifiedImagefeatures  = new CounterFeatureVectorList (*stratifiedFeatureVectors);
  
  stratifiedFeatureVectors->Owner (false);

  delete stratifiedFeatureVectors;  stratifiedFeatureVectors = NULL;
  delete  classes;                  classes                  = NULL;

  return  stratifiedImagefeatures;
}  /* StratifyAmoungstClasses */



CounterFeatureVectorListPtr  CounterFeatureVectorList::StratifyAmoungstClasses (MLClassListPtr  mlClasses,
                                                                              kkint32         maxImagesPerClass,
                                                                              kkint32         numOfFolds,
                                                                              RunLog&         log
                                                                             )
{
  FeatureVectorListPtr  stratifiedFeatureVectors = FeatureVectorList::StratifyAmoungstClasses (mlClasses, maxImagesPerClass, numOfFolds, log);
  CounterFeatureVectorListPtr  stratifiedImagefeatures  = new CounterFeatureVectorList (*stratifiedFeatureVectors);
  stratifiedFeatureVectors->Owner (false);
  delete stratifiedFeatureVectors;  stratifiedFeatureVectors = NULL;
  return  stratifiedImagefeatures;
}  /* StratifyAmoungstClasses */
