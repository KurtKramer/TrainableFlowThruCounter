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
#include "LarcosFVProducer.h"
#include "MLClass.h"
#include "ImageDirTree.h"
using namespace  KKMLL;


#include "LarcosFeatureVector.h"
using namespace  LarcosCounterUnManaged;
                      





LarcosFeatureVector::LarcosFeatureVector (kkint32  _numOfFeatures):
       FeatureVector (_numOfFeatures),

        centroidCol     (-1),
        centroidRow     (-1),
        numOfEdgePixels (-1)
{
}



LarcosFeatureVector::LarcosFeatureVector (const LarcosFeatureVector&  _fv):
  FeatureVector (_fv),

  centroidCol     (_fv.centroidCol),
  centroidRow     (_fv.centroidRow),
  numOfEdgePixels (_fv.numOfEdgePixels)
{
}





LarcosFeatureVector::LarcosFeatureVector (const FeatureVector&  featureVector):
   FeatureVector    (featureVector),
   centroidCol      (-1),
   centroidRow      (-1),
   numOfEdgePixels  (-1)

{
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









LarcosFeatureVectorList::LarcosFeatureVectorList (FileDescPtr  _fileDesc,
                                                  bool         _owner
                                                 ):
    FeatureVectorList (_fileDesc, _owner)
{

}




LarcosFeatureVectorList::LarcosFeatureVectorList (FactoryFVProducerPtr  _fvProducerFactory,
                                                  MLClassPtr            _mlClass,
                                                  KKStr                 _dirName,
                                                  KKStr                 _fileName,
                                                  RunLog&               _log
                                                 ):

   FeatureVectorList (_fvProducerFactory->FileDesc (), true)

{
  FeatureExtraction (_fvProducerFactory, _dirName, _fileName, _mlClass, _log);
}




LarcosFeatureVectorList::LarcosFeatureVectorList (const LarcosFeatureVectorList&  examples):
   FeatureVectorList (examples.FileDesc (), examples.Owner ())
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
                                                  bool                           _owner
                                                 ):

   FeatureVectorList (examples.FileDesc (), _owner)
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

  FeatureVectorList (featureVectorList.FileDesc (), _owner)
{
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
      if  (typeid (*featureVector) == typeid (LarcosFeatureVector))
      {
        LarcosFeatureVectorPtr example = dynamic_cast<LarcosFeatureVectorPtr>(featureVector);
        PushOnBack (example);
      }
      else
      {
        // ****    ERROR  ****
        KKStr  errMsg;
        errMsg << "LarcosFeatureVectorList   ***ERROR***   One Elements not a 'LarcosFeatureVector'  type  FileName[" << featureVector->ExampleFileName () << "]";
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
LarcosFeatureVectorList::LarcosFeatureVectorList (MLClassList&              _mlClasses,
                                                  LarcosFeatureVectorList&  _examples
                                                 ):
  FeatureVectorList (_mlClasses, _examples)
  
{
}




LarcosFeatureVectorList::LarcosFeatureVectorList (const FeatureVectorList&  featureVectorList):
  FeatureVectorList (featureVectorList.FileDesc (),
                     featureVectorList.Owner ()
                    )
{
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
      
      // The constructor below will detect what the underlying type of 'FeatureVector' is.  
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
      if  (typeid (*featureVector) == typeid (LarcosFeatureVector))
      {
        LarcosFeatureVectorPtr example = dynamic_cast<LarcosFeatureVectorPtr>(featureVector);
        PushOnBack (example);
      }
      else
      {
        // ****    ERROR  ****
        KKStr  errMsg;
        errMsg << "LarcosFeatureVectorList (const FeatureVectorList&  featureVectorList)   ***ERROR***   "
             << "One element not a 'LarcosFeatureVector' type can not recast:  FileName[" << featureVector->ExampleFileName () << "]";
        cerr << endl << errMsg << endl << endl;
        throw KKException (errMsg);
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





LarcosFeatureVectorPtr  LarcosFeatureVectorList::BackOfQueue ()
{
  if  (size () < 1)
    return NULL;

  FeatureVectorPtr  fv = back ();
  if  (typeid (*fv) == typeid (LarcosFeatureVector))
    return  dynamic_cast<LarcosFeatureVectorPtr> (fv);

  cerr << endl 
       << "LarcosFeatureVectorList::BackOfQueue ()   ***ERROR***   Entry at back of Queue is not a 'LarcosFeatureVector' object." << endl
       << endl;

  return NULL;
}  /* BackOfQueue */




LarcosFeatureVectorPtr  LarcosFeatureVectorList::PopFromBack ()
{
  if  (size () < 1)  return NULL;

  FeatureVectorPtr  fv = back ();
  if  (typeid (*fv) == typeid (LarcosFeatureVector))
  {
    cerr << endl
         << "LarcosFeatureVectorList::BackOfQueue ()   ***ERROR***   Entry popped from back of Queue is not a 'LarcosFeatureVector' object." << endl
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




LarcosFeatureVectorListPtr   LarcosFeatureVectorList::ManufactureEmptyList (bool _owner)  const
{
  return new LarcosFeatureVectorList (FileDesc (), _owner);
}



LarcosFeatureVectorPtr  LarcosFeatureVectorList::LookUpByRootName (const KKStr&  _rootName)
{
  return  (LarcosFeatureVectorPtr)FeatureVectorList::LookUpByRootName (_rootName);
}  /* LookUpByRootName */






LarcosFeatureVectorPtr  LarcosFeatureVectorList::LookUpByImageFileName (const KKStr&  _imageFileName)  const
{
  return  (LarcosFeatureVectorPtr)FeatureVectorList::LookUpByImageFileName (_imageFileName);
}  /* LookUpByImageFileName */





LarcosFeatureVectorListPtr  LarcosFeatureVectorList::OrderUsingNamesFromAFile (const KKStr&  fileName,
                                                                               RunLog&       log
                                                                              )
{
  FeatureVectorListPtr  examples = FeatureVectorList::OrderUsingNamesFromAFile (fileName, log);
  examples->Owner (false);
  LarcosFeatureVectorListPtr  orderedImages = new LarcosFeatureVectorList (*examples);
  delete  examples;
  return  orderedImages;
}  /* OrderUsingNamesFromAFile */






void   LarcosFeatureVectorList::FeatureExtraction (FactoryFVProducerPtr  _fvProducerFactory,
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
      msg << "LarcosFeatureVectorList::FeatureExtraction   ***ERROR***   Could not Allocate LarcosFeatureVector object" << endl
          << "for FileName[" << fullFileName << "].";
      _log.Level (-1) << endl << msg << endl << endl;
    }
    else
    {
      LarcosFeatureVectorPtr  larcosFeatureVector = NULL;
      if  (typeid(*featureVector) == typeid(LarcosFeatureVector))
      {
        larcosFeatureVector = dynamic_cast<LarcosFeatureVectorPtr>(featureVector);
        featureVector = NULL;
      }
      else
      {
        larcosFeatureVector = new LarcosFeatureVector (*featureVector);
        delete  featureVector;
        featureVector = NULL;
      }

      larcosFeatureVector->ExampleFileName (*imageFileName);
      _log.Level (30) << larcosFeatureVector->ExampleFileName () << "  " << larcosFeatureVector->OrigSize () << endl;
      PushOnBack (larcosFeatureVector);
      count++;
    }
  }

  delete  fvProducer;  fvProducer = NULL;

  Version (_LarcosFVProducer_VersionNum_);

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




LarcosFeatureVectorListPtr   LarcosFeatureVectorList::Duplicate (bool _owner)  const
{
  return new LarcosFeatureVectorList (*this, _owner);
}  /* Duplicate */




/**
 * @brief  Creates a duplicate of list and also duplicates it contents.
 * @return Duplicated list with hard copy of its contents.
 */
LarcosFeatureVectorListPtr  LarcosFeatureVectorList::DuplicateListAndContents ()  const
{
  LarcosFeatureVectorListPtr  copyiedList = new LarcosFeatureVectorList (FileDesc (), true);

  for  (kkint32 idx = 0;  idx < QueueSize ();  idx++)
  {
    LarcosFeatureVectorPtr  curImage = (LarcosFeatureVectorPtr)IdxToPtr (idx);
    copyiedList->PushOnBack (new LarcosFeatureVector (*curImage));
  }
  
  copyiedList->Version (Version ());

  return  copyiedList;
}  /* DuplicateListAndContents */






void  LarcosFeatureVectorList::RecalcFeatureValuesFromImagesInDirTree (FactoryFVProducerPtr  fvProducerFactory,  
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
  LarcosFeatureVectorPtr  example = NULL;

  for  (idx = begin ();   idx != end ();  idx++)
  {
    example = *idx;
    dirPath = fileDirectory.LocateImage (example->ExampleFileName ());
    if  (!dirPath)
    {
      log.Level (10) << "RecalcFeatureValuesFromImagesInDirTree  Could not locate Image[" << example->ExampleFileName () << "] in Subdirectory Tree." << endl;
      return;
    }

    KKStr  fullFileName (dirPath);
    osAddLastSlash (fullFileName);
    fullFileName << example->ExampleFileName ();

    bool   validFile;
    RasterPtr  raster = new Raster (fullFileName, validFile);
    if  (!validFile)
    {
      delete  raster;  raster = NULL;
      log.Level (-1) << "LarcosFeatureVectorList::RecalcFeatureValuesFromImagesInDirTree   ***ERROR***  Unable to load image: " << fullFileName << endl << endl;
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

        if  (typeid(*fv) == typeid(LarcosFeatureVector))
        {
          LarcosFeatureVectorPtr  lfv = dynamic_cast<LarcosFeatureVectorPtr>(fv);
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





LarcosFeatureVectorListPtr  LarcosFeatureVectorList::ExtractDuplicatesByRootImageFileName ()
{
  FeatureVectorListPtr  duplicateFeatureVectorObjects = FeatureVectorList::ExtractDuplicatesByRootImageFileName ();
  LarcosFeatureVectorListPtr  duplicateImageFeaturesObjects = new LarcosFeatureVectorList (*duplicateFeatureVectorObjects);
  duplicateFeatureVectorObjects->Owner (false);
  delete  duplicateFeatureVectorObjects;  duplicateFeatureVectorObjects = NULL;
  return  duplicateImageFeaturesObjects;
}  /* ExtractDuplicatesByRootImageFileName */






LarcosFeatureVectorListPtr   LarcosFeatureVectorList::ExtractImagesForAGivenClass (MLClassPtr  _mlClass,
                                                                                   kkint32     _maxToExtract,
                                                                                   float       _minSize
                                                                                  )  const
{
  FeatureVectorListPtr  featureVectorList = FeatureVectorList::ExtractImagesForAGivenClass (_mlClass, _maxToExtract, _minSize);
  LarcosFeatureVectorListPtr  imageFeaturesList = new LarcosFeatureVectorList (*featureVectorList);
  featureVectorList->Owner (false);
  delete  featureVectorList;  featureVectorList = NULL;
  return  imageFeaturesList;
}  /*  ExtractImagesForAGivenClass  */



LarcosFeatureVectorListPtr  LarcosFeatureVectorList::StratifyAmoungstClasses (kkint32  numOfFolds, 
                                                                              RunLog&  log
                                                                             )
{
  MLClassListPtr  classes = ExtractListOfClasses ();

  FeatureVectorListPtr  stratifiedFeatureVectors = FeatureVectorList::StratifyAmoungstClasses (classes, -1, numOfFolds, log);
  LarcosFeatureVectorListPtr  stratifiedImagefeatures  = new LarcosFeatureVectorList (*stratifiedFeatureVectors);
  
  stratifiedFeatureVectors->Owner (false);

  delete stratifiedFeatureVectors;  stratifiedFeatureVectors = NULL;
  delete  classes;                  classes                  = NULL;

  return  stratifiedImagefeatures;
}  /* StratifyAmoungstClasses */



LarcosFeatureVectorListPtr  LarcosFeatureVectorList::StratifyAmoungstClasses (MLClassListPtr  mlClasses,
                                                                              kkint32         maxImagesPerClass,
                                                                              kkint32         numOfFolds,
                                                                              RunLog&         log
                                                                             )
{
  FeatureVectorListPtr  stratifiedFeatureVectors = FeatureVectorList::StratifyAmoungstClasses (mlClasses, maxImagesPerClass, numOfFolds, log);
  LarcosFeatureVectorListPtr  stratifiedImagefeatures  = new LarcosFeatureVectorList (*stratifiedFeatureVectors);
  stratifiedFeatureVectors->Owner (false);
  delete stratifiedFeatureVectors;  stratifiedFeatureVectors = NULL;
  return  stratifiedImagefeatures;
}  /* StratifyAmoungstClasses */



