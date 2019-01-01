#include  "StdAfx.h"
#include  "FirstIncludes.h"

#include  <stdio.h>
#include  <math.h>
#include  <ctype.h>
#include  <time.h>
#include  <fstream>
#include  <iostream>
#include  <map>
#include  <ostream>
#include  <string>
#include  <vector>
using namespace std;


#include  "MemoryDebug.h"
#include  "KKBaseTypes.h"
#include  "..\KKBaseLibrery\GoalKeeper.h"
using namespace KKB;

#include  "ScannerFileEntry.h"
using namespace  KKLSC;


#include  "UmiKKStr.h"
#include  "UmiScannerFileEntry.h"
#include  "UmiDataBaseServer.h"
#include  "UmiOSservices.h"

#include  "UmiDataBase.h"

#include  "UmiDataBaseServerPrompter.h"

using namespace  LarcosDatabaseManaged;


using namespace System;
using namespace System::Collections::Generic;
using namespace System::IO;
using namespace System::Windows::Forms;



UmiDataBase::UmiDataBase (UmiRunLog^  _runLog)
{
  runLog = _runLog;

  dbConn = new DataBase (runLog->Log ());
  cancelFlag = new bool;
  *cancelFlag = false;
};



UmiDataBase::UmiDataBase (UmiDataBaseServer^  _server,
                              UmiRunLog^          _log
                             )
{
  runLog = _log;
  dbConn = new DataBase (_server->Server (), runLog->Log ());
  cancelFlag = new bool;
  *cancelFlag = false;
}



UmiDataBase::!UmiDataBase ()
{
  delete  dbConn;
  dbConn = NULL;

  delete  cancelFlag;
  cancelFlag = NULL;
}




UmiDataBase::~UmiDataBase ()
{
  this->!UmiDataBase ();
}




bool  UmiDataBase::CancelFlag::get ()
{
  if  (cancelFlag)
    return  *cancelFlag;
  else
    return false;
}


void  UmiDataBase::CancelFlag::set (bool _cancelFlag)
{
  if  (!cancelFlag)
    cancelFlag = new bool;
  *cancelFlag = _cancelFlag;
}
 


UmiDataBaseServer^  UmiDataBase::Server::get ()
{
  if  (dbConn == NULL)
    return nullptr;

  DataBaseServerPtr  server = dbConn->Server ();
  if  (server == NULL)
    return gcnew UmiDataBaseServer ();

  return  gcnew UmiDataBaseServer (server);
}





UmiDataBase^  UmiDataBase::GetGlobalDatabaseManager (UmiRunLog^ _runLog)
{
  if  (globaDbConn == nullptr)
  {
    globaDbConn = gcnew UmiDataBase (_runLog);
    if  (!globaDbConn->Valid ())
      globaDbConn = nullptr;
  }

  return  globaDbConn;
}  /* GetGlobalDatabaseManager */





UmiDataBase^  UmiDataBase::GetGlobalDatabaseManagerNewInstance (UmiRunLog^ _runLog)
{
  UmiDataBase^  curGlobalConection = GetGlobalDatabaseManager (_runLog);
  if  (curGlobalConection == nullptr)
    return  nullptr;
  UmiDataBaseServer^ curServer = curGlobalConection->Server;
  UmiDataBase^  newInstance = gcnew UmiDataBase (curServer, _runLog);
  return  newInstance;
}  /* GetGlobalDatabaseManagerNewInstance */





UmiDataBase^  UmiDataBase::SelectNewDataBaseServer (UmiRunLog^  _log)
{
  UmiDataBaseServerPrompter^ dataBasePrompter = gcnew UmiDataBaseServerPrompter (_log);

  dataBasePrompter->ShowDialog ();

  UmiDataBaseServer^ newServer = dataBasePrompter->SelectedServer;

  if  (newServer != nullptr)
  {
    globaDbConn = nullptr;
    globaDbConn = gcnew UmiDataBase (newServer, _log);
  }
  
  return  globaDbConn;
}  /* SelectNewDataBaseServer */







String^  UmiDataBase::ServerDescription ()
{
  if  (dbConn)
    return  UmiKKStr::KKStrToSystenStr (dbConn->ServerDescription ());
  else
    return  "***  not connected  ***";
}  /* ServerDescription */




String^  UmiDataBase::LastErrorDesc ()
{
  return  UmiKKStr::KKStrToSystenStr (dbConn->LastErrorDesc ());
}




bool  UmiDataBase::Valid ()
{
  if  (!dbConn)
    return  false;
  return  dbConn->Valid ();
}



bool  UmiDataBase::ThreadInit ()
{
  lastOpSuccessful = false;
  if  (dbConn)
    lastOpSuccessful = dbConn->ThreadInit ();
  return  lastOpSuccessful;
}



void  UmiDataBase::ThreadEnd ()
{
  if  (dbConn)
    dbConn->ThreadEnd ();
}



 
array<array<String^>^ >^  UmiDataBase::QueryStatement (String^          statement,
                                                       array<String^>^  colsToReturn
                                                      )
{
  char**  colsToReturnCS = NULL;
  uint    col = 0;
  uint    numCols = 0;

  if  (colsToReturn != nullptr)
  {
    numCols = colsToReturn->Length;
    colsToReturnCS = new char*[numCols + 1];

    for (col = 0;  col < numCols;  col++)
      colsToReturnCS[col] = UmiKKStr::StrToCharStar (colsToReturn[col]);
    colsToReturnCS[numCols] = NULL;
  }

  KKStrMatrixPtr resultsKKStr = dbConn->QueryStatement (UmiKKStr::SystemStringToKKStr (statement), colsToReturnCS);

  if  (colsToReturnCS)
  {
    for  (col = 0;  col < numCols;  col++)
      delete  colsToReturnCS[col];
    delete  colsToReturnCS;
    colsToReturnCS = NULL;
  }

  if  (resultsKKStr == NULL)
    return nullptr;

  int  numRows = resultsKKStr->NumRows ();
  array<array<String^>^ >^ results = gcnew array<array<String^>^ >(numRows);

  for  (int row = 0;  row < numRows;  row++)
  {
    KKStrList&  rowDataKKStr = (*resultsKKStr)[row];
    uint  numCols = rowDataKKStr.QueueSize ();

    array<String^>^  rowData = gcnew array<String^> (numCols);

    for  (col = 0;  col < numCols;  col++)
    {
      rowData[col] = UmiKKStr::KKStrToSystenStr (rowDataKKStr[col]);
    }

    results[row] = rowData;
  }

  delete  resultsKKStr;
  resultsKKStr = NULL;

  return  results;
}  /* QueryStatement*/








//***************************************************************************************
//*                               Feature Data Routines                                 *
//***************************************************************************************
void  UmiDataBase::FeatureDataInsertRow (String^              _scannerFileName,
                                           UmiFeatureVector^  _example
                                          )
{
  dbConn->FeatureDataInsertRow (UmiKKStr::SystemStringToKKStr (_scannerFileName), *(_example->Features ()));
  lastOpSuccessful = dbConn->Valid ();
}  /* FeatureDataInsertRow*/




char  ClassKeyToUse (System::Char  classKeyToUse)
{
  if  ((classKeyToUse == 'V')  ||  (classKeyToUse == 'v'))
    return 'V';
  else
    return 'P';
}




UmiFeatureVector^  UmiDataBase::FeatureDataRecLoad (String^  imageFileName)
{
  KKStr fn = UmiKKStr::SystemStringToKKStr (imageFileName);
  PostLarvaeFVPtr  fv = dbConn->FeatureDataRecLoad (fn);
  lastOpSuccessful = dbConn->Valid ();
  if  ((!lastOpSuccessful)  ||  (fv == NULL))
    return nullptr;
  else
    return  gcnew UmiFeatureVector (fv);
}  /* FeatureDataRecLoad */




//***************************************************************************************
//*   Will use ImageFilenName, ScannerFileName, and TopLeftRow to load in ImageFeatures  *
//*  data from 'FeatureData'  and  'InstrumentData' tables.                             *
//***************************************************************************************
UmiFeatureVector^  UmiDataBase::FeatureDataRecLoad (UmiDataBaseImage^  image)
{
  PostLarvaeFVPtr  fv = dbConn->FeatureDataRecLoad (image->UnManagedDataBaseImage ());
  lastOpSuccessful = dbConn->Valid ();
  if  ((!lastOpSuccessful)  ||  (fv == NULL))
    return nullptr;
  else
    return  gcnew UmiFeatureVector (fv);
}



UmiFeatureVectorList^  UmiDataBase::FeatureDataGetOneScannerFile (String^       scannerFileName,
                                                                     UmiClass^   imageClass,
                                                                     System::Char  classKeyToUse,
                                                                     bool          reExtractInstrumentData
                                                                    )
{
  ImageClassPtr  ic = NULL;
  if  (imageClass != nullptr)
    ic = imageClass->UnmanagedImageClass ();

  PostLarvaeFVListPtr  examples = dbConn->FeatureDataGetOneScannerFile (UmiKKStr::SystemStringToKKStr (scannerFileName), 
                                                                         ic, 
                                                                         ClassKeyToUse (classKeyToUse),
                                                                         *cancelFlag
                                                                        );
  if  (examples == NULL)
    return nullptr;

  UmiFeatureVectorList^  results = gcnew UmiFeatureVectorList (*examples);
  // "results"  will own the individual instances in "examples" so we need
  // to set the "Owner" property of "examples" to false so taht when it gets
  // deleted it does not delete them.
  examples->Owner (false);  
  delete  examples;  examples = NULL;

  return  results;
}  /* FeatureDataGetOneScannerFile */




UmiFeatureVectorList^  UmiDataBase::FeatureDataForImageGroup (UmiDataBaseImageGroup^  imageGroup,
                                                              UmiClass^               imageClass,
                                                              System::Char              classKeyToUse
                                                             )
{
  if  (imageGroup == nullptr)
    return nullptr;

  ImageClassPtr  ic = NULL;
  if  (imageClass != nullptr)
    ic = imageClass->UnmanagedImageClass ();

  PostLarvaeFVListPtr  examples = dbConn->FeatureDataForImageGroup (imageGroup->UnManagedClass (), 
                                                                     ic,  
                                                                     ClassKeyToUse (classKeyToUse), 
                                                                     *cancelFlag
                                                                    );
  if  (examples == NULL)
    return nullptr;

  UmiFeatureVectorList^  results = gcnew UmiFeatureVectorList (*examples);
  // "results"  will own the individual instances in "examples" so we need
  // to set the "Owner" property of "examples" to false so taht when it gets
  // deleted it does not delete them.
  examples->Owner (false);  
  delete  examples;  examples = NULL;

  return  results;
}  /* FeatureDataForImageGroup */




void   UmiDataBase::FeatureDataUpdate (UmiFeatureVector^  example)
{
  dbConn->FeatureDataUpdate (example->UnManagedClass ());
  lastOpSuccessful = dbConn->Valid ();
}



//**********************************************************************************************
//****************************     UmiDataBaseImage  routines     ****************************
//**********************************************************************************************

UmiDataBaseImage^  UmiDataBase::ImageLoad (uint  imageId)
{
  DataBaseImagePtr image = dbConn->ImageLoad (imageId);
  lastOpSuccessful = (image != NULL);
  if  (image == NULL)
    return nullptr;

  return gcnew UmiDataBaseImage (image);
}  /* ImageLoad*/



UmiDataBaseImage^  UmiDataBase::ImageLoad (String^   imageFileName)
{
  DataBaseImagePtr image = dbConn->ImageLoad (UmiKKStr::SystemStringToKKStr (imageFileName));
  lastOpSuccessful = (image != NULL);
  if  (image == NULL)
    return nullptr;

  return gcnew UmiDataBaseImage (image);
}  /* ImageLoad*/



UmiDataBaseImageList^  UmiDataBase::ImageQuery (UmiDataBaseImageGroup^  group,
                                                bool                    includeThumbnail
                                               )
{
  DataBaseImageListPtr images = dbConn->ImageQuery (group->UnManagedClass (), includeThumbnail, *cancelFlag);
  lastOpSuccessful = dbConn->Valid ();
  if  (!images)
    return nullptr;

  UmiDataBaseImageList^  managedImages = gcnew UmiDataBaseImageList (images);
  images->Owner (false);
  delete  images;
  return  managedImages;
}  /* ImageQuery */





UmiDataBaseImageList^  UmiDataBase::ImageQuery (UmiDataBaseImageGroup^  imageGroup,
                                                    String^                   scannerFileName,
                                                    UmiClass^               imageClass,
                                                    System::Char              classKeyToUse,
                                                    float                     probMin,
                                                    float                     probMax,
                                                    int                       sizeMin,
                                                    int                       sizeMax,
                                                    float                     depthMin,
                                                    float                     depthMax,
                                                    uint                      restartImageId,
                                                    int                       limit,             // Max # of rows to return.  -1 indicates no limit.
                                                    bool                      includeThumbnail
                                                   )
{
  *cancelFlag = false;

  DataBaseImageGroupPtr  ig = NULL;
  if  (imageGroup != nullptr)
    ig = imageGroup->UnManagedClass ();

  ImageClassPtr  unmanagedClass = NULL;
  if  (imageClass != nullptr)
    unmanagedClass = imageClass->UnmanagedImageClass ();

  
  DataBaseImageListPtr  
            images = dbConn->ImageQuery (ig,
                                         UmiKKStr::SystemStringToKKStr (scannerFileName),
                                         unmanagedClass,  ClassKeyToUse (classKeyToUse),
                                         probMin,   probMax,
                                         sizeMin,   sizeMax,
                                         depthMin,  depthMax,
                                         restartImageId,
                                         limit,
                                         includeThumbnail,
                                         *cancelFlag
                                        );

  lastOpSuccessful = dbConn->Valid ();

  if  (images == NULL)
    return  nullptr;

  if  (*cancelFlag)
  {
    delete  images;
    return  nullptr;
  }

  UmiDataBaseImageList^  umiImages = gcnew UmiDataBaseImageList (images);
  images->Owner (false);
  delete  images;
  images = NULL;
  return  umiImages;
}  /* ImageQuery */





UmiDataBaseImageList^  UmiDataBase::ImageQuery (UmiDataBaseImageGroup^  imageGroup,
                                                    String^                   cruiseName,
                                                    String^                   stationName,
                                                    String^                   deploymentNum,
                                                    UmiClass^               imageClass,
                                                    System::Char              classKeyToUse,
                                                    float                     probMin,
                                                    float                     probMax,
                                                    int                       sizeMin,
                                                    int                       sizeMax,
                                                    float                     depthMin,
                                                    float                     depthMax,
                                                    uint                      restartImageId,
                                                    int                       limit,             // Max # of rows to return.  -1 indicates no limit.
                                                    bool                      includeThumbnail
                                                   )
{
  *cancelFlag = false;
  ImageClassPtr  unmanagedClass = NULL;
  if  (imageClass != nullptr)
    unmanagedClass = imageClass->UnmanagedImageClass ();

  DataBaseImageGroupPtr  ig = NULL;
  if  (imageGroup != nullptr)
    ig = imageGroup->UnManagedClass ();

  DataBaseImageListPtr  
            images = dbConn->ImageQuery (ig,
                                         UmiKKStr::SystemStringToKKStr (cruiseName),
                                         UmiKKStr::SystemStringToKKStr (stationName),
                                         UmiKKStr::SystemStringToKKStr (deploymentNum),
                                         unmanagedClass,  ClassKeyToUse (classKeyToUse), 
                                         probMin,   probMax,
                                         sizeMin,   sizeMax,
                                         depthMin,  depthMax,
                                         restartImageId,
                                         limit,
                                         includeThumbnail,
                                         *cancelFlag
                                        );

  lastOpSuccessful = dbConn->Valid ();

  if  (images == NULL)
    return  nullptr;

  if  (*cancelFlag)
  {
    delete  images;
    return  nullptr;
  }

  UmiDataBaseImageList^  umiImages = gcnew UmiDataBaseImageList (images);
  images->Owner (false);
  delete  images;
  images = NULL;
  return  umiImages;
}  /* ImageQuery */







UmiDataBaseImageList^  UmiDataBase::ImageQueryByGrouop 
                                  (UmiDataBaseImageGroup^   imageGroup,
                                   String^                    cruiseName,
                                   String^                    stationName,
                                   String^                    deploymentNum,
                                   String^                    scannerFileName,
                                   UmiClass^                imageClass,
                                   System::Char               classKeyToUse,
                                   float                      probMin,
                                   float                      probMax,
                                   int                        sizeMin,
                                   int                        sizeMax,
                                   float                      depthMin,
                                   float                      depthMax,
                                   uint                       restartImageId,
                                   int                        limit,            // Max # of rows to return.  -1 indicates no limit.,
                                   bool                       includeThumbnail
                                  )
{
  if  (imageGroup == nullptr)
    return  nullptr;

  DataBaseImageListPtr  images = dbConn->ImageQueryByGrouop 
                              (imageGroup->UnManagedClass (),
                               UmiKKStr::SystemStringToKKStr (cruiseName), 
                               UmiKKStr::SystemStringToKKStr (stationName),
                               UmiKKStr::SystemStringToKKStr (deploymentNum),
                               UmiKKStr::SystemStringToKKStr (scannerFileName),
                               ((imageClass == nullptr) ? NULL : imageClass->UnmanagedImageClass ()),
                               ClassKeyToUse (classKeyToUse), 
                               probMin,  probMax, 
                               sizeMin,  sizeMax,
                               depthMin, depthMax,
                               restartImageId,
                               limit,
                               includeThumbnail
                              );
  lastOpSuccessful = dbConn->Valid ();

  if  (images == NULL)
    return  nullptr;

  UmiDataBaseImageList^  umiImages = gcnew UmiDataBaseImageList (images);
  images->Owner (false);
  delete  images;
  images = NULL;
  return  umiImages;
}  /* ImageQueryByGrouop */






UmiClassStatisticList^  UmiDataBase::ImageGetClassStatistics 
                                (UmiDataBaseImageGroup^  imageGroup,
                                 String^                   scannerFileName,
                                 UmiClass^               imageClass,
                                 System::Char              classKeyToUse,
                                 float                     minProb,
                                 float                     maxProb,
                                 int                       minSize,
                                 int                       maxSize,
                                 float                     minDepth,
                                 float                     maxDepth
                                )
{
  DataBaseImageGroupPtr  ig = NULL;
  if  (imageGroup != nullptr)
    ig = imageGroup->UnManagedClass ();

  ImageClassPtr  ic = NULL;
  if  (imageClass != nullptr)
    ic = imageClass->UnmanagedImageClass ();

  ClassStatisticListPtr  stats 
       = dbConn->ImageGetClassStatistics (ig,
                                          UmiKKStr::SystemStringToKKStr (scannerFileName),
                                          ic,
                                          ClassKeyToUse (classKeyToUse),
                                          minProb,  maxProb,
                                          minSize,  maxSize,
                                          minDepth, maxDepth
                                         );
  if   (stats == NULL)
    return nullptr;

  UmiClassStatisticList^ results = gcnew UmiClassStatisticList (*stats);
  delete  stats;

  return  results;
}  /* ImageGetClassStatistics */





UmiClassStatisticList^  UmiDataBase::ImageGetClassStatistics 
                               (UmiDataBaseImageGroup^  imageGroup,
                                String^                   cruiseName,
                                String^                   stationName,
                                String^                   deploymentNum,
                                UmiClass^               imageClass,
                                System::Char              classKeyToUse,
                                float                     minProb,
                                float                     maxProb,
                                int                       minSize,
                                int                       maxSize,
                                float                     minDepth,
                                float                     maxDepth
                               )
{
  DataBaseImageGroupPtr  ig = NULL;
  if  (imageGroup != nullptr)
    ig = imageGroup->UnManagedClass ();

  ImageClassPtr  ic = NULL;
  if  (imageClass != nullptr)
    ic = imageClass->UnmanagedImageClass ();

  ClassStatisticListPtr  stats 
       = dbConn->ImageGetClassStatistics (ig,
                                          UmiKKStr::SystemStringToKKStr (cruiseName),
                                          UmiKKStr::SystemStringToKKStr (stationName),
                                          UmiKKStr::SystemStringToKKStr (deploymentNum),
                                          ic,
                                          ClassKeyToUse (classKeyToUse),
                                          minProb,  maxProb,
                                          minSize,  maxSize,
                                          minDepth, maxDepth
                                         );
  if   (stats == NULL)
    return nullptr;

  UmiClassStatisticList^  results = gcnew UmiClassStatisticList (*stats);
  delete  stats;

  return  results;
}  /* ImageGetClassStatistics */





array<uint>^  UmiDataBase::ImageGetDepthStatistics (UmiDataBaseImageGroup^  imageGroup,
                                                      String^                   scannerFileName,
                                                      float                     depthIncrements,
                                                      UmiClass^               imageClass,
                                                      System::Char              classKeyToUse,
                                                      float                     probMin,
                                                      float                     probMax,
                                                      int                       sizeMin,
                                                      int                       sizeMax
                                                     )
{
  DataBaseImageGroupPtr  ig = NULL;
  if  (imageGroup != nullptr)
    ig = imageGroup->UnManagedClass ();

  ImageClassPtr  ic = NULL;
  if  (imageClass != nullptr)
    ic = imageClass->UnmanagedImageClass ();

  VectorUint*  depthStats 
    = dbConn->ImageGetDepthStatistics (ig,
                                       UmiKKStr::SystemStringToKKStr (scannerFileName),
                                       depthIncrements,
                                       ic,
                                       ClassKeyToUse (classKeyToUse),
                                       probMin,
                                       probMax,
                                       sizeMin,
                                       sizeMax
                                      );

  if  ((depthStats == NULL)  ||  (depthStats->size () < 1))
  {
    delete  depthStats;  depthStats = NULL;
    return  nullptr;
  }
  
  array<uint>^ stats = gcnew array<uint> (depthStats->size ());

  for  (int x = 0;  x < (int)depthStats->size ();  x++)
    stats->SetValue ((*depthStats)[x], x);

  delete  depthStats;  depthStats = NULL;
  return  stats;
}  /* ImageGetDepthStatistics */





array<uint>^  UmiDataBase::ImageGetDepthStatistics (UmiDataBaseImageGroup^  imageGroup,
                                                      String^                   cruiseName,
                                                      String^                   stationName,
                                                      String^                   deploymentNum,
                                                      float                     depthIncrements,
                                                      UmiClass^               imageClass,
                                                      System::Char              classKeyToUse,
                                                      float                     probMin,
                                                      float                     probMax,
                                                      int                       sizeMin,
                                                      int                       sizeMax
                                                     )
{
  DataBaseImageGroupPtr  ig = NULL;
  if  (imageGroup != nullptr)
    ig = imageGroup->UnManagedClass ();

  ImageClassPtr  ic = NULL;
  if  (imageClass != nullptr)
    ic = imageClass->UnmanagedImageClass ();

  VectorUint*  depthStats 
    = dbConn->ImageGetDepthStatistics (ig,
                                       UmiKKStr::SystemStringToKKStr (cruiseName),
                                       UmiKKStr::SystemStringToKKStr (stationName),
                                       UmiKKStr::SystemStringToKKStr (deploymentNum),
                                       depthIncrements,
                                       ic,
                                       ClassKeyToUse (classKeyToUse),
                                       probMin,
                                       probMax,
                                       sizeMin,
                                       sizeMax
                                      );

  if  ((depthStats == NULL)  ||  (depthStats->size () < 1))
  {
    delete  depthStats;  depthStats = NULL;
    return  nullptr;
  }
  
  array<uint>^ stats = gcnew array<uint> (depthStats->size ());

  for  (int x = 0;  x < (int)depthStats->size ();  x++)
    stats->SetValue ((*depthStats)[x], x);

  delete  depthStats;  depthStats = NULL;
  return  stats;
}   /* ImageGetDepthStatistics */






void  UmiDataBase::ImageUpdatePredictions (String^      imageFileName,
                                             UmiClass^  class1Pred,
                                             float        class1Prob,
                                             UmiClass^  class2Pred,
                                             float        class2Prob
                                            )
{
  if  (class1Pred == nullptr)
    class1Pred = UmiClassList::GetUnKnownClassStatic ();

  if  (class2Pred == nullptr)
    class2Pred = UmiClassList::GetUnKnownClassStatic ();

  dbConn->ImageUpdatePredictions (UmiKKStr::SystemStringToKKStr (imageFileName),
                                  class1Pred->UnmanagedImageClass (),
                                  class1Prob,
                                  class2Pred->UnmanagedImageClass (),
                                  class2Prob
                                 );
  lastOpSuccessful = dbConn->Valid ();
}  /* ImageUpdatePredictions*/





void  UmiDataBase::ImageUpdateValidatedClass (String^      imageFileName, 
                                                UmiClass^  imageClass
                                               )
{
  dbConn->ImageUpdateValidatedClass (UmiKKStr::SystemStringToKKStr (imageFileName), imageClass->UnmanagedImageClass ());
  lastOpSuccessful = dbConn->Valid ();
}




void  UmiDataBase::ImageUpdateValidatedAndPredictClass (String^     imageFileName, 
                                                          UmiClass^ imageClass, 
                                                          float       class1Prob
                                                         )
{
  if  (imageClass == nullptr)
    imageClass = UmiClassList::GetUnKnownClassStatic ();

  dbConn->ImageUpdateValidatedAndPredictClass (UmiKKStr::SystemStringToKKStr (imageFileName),
                                               imageClass->UnmanagedImageClass (),
                                               class1Prob
                                              );
  lastOpSuccessful = dbConn->Valid ();
}  /* ImageUpdateValidatedAndPredictClass */




//***************************************************************************************
//*                           ImagesFullSize  Routines                                  *
//***************************************************************************************
UmiRaster^  UmiDataBase::ImageFullSizeLoad (String^  imageFileName)
{
  RasterPtr  r = dbConn->ImageFullSizeLoad (UmiKKStr::SystemStringToKKStr (imageFileName));
  if  (!r)
    return  nullptr;
  else
    return  gcnew UmiRaster (r);
}




void  UmiDataBase::ImageFullSizeSave (String^       imageFileName,
                                        UmiRaster^  raster
                                       )
{
  dbConn->ImageFullSizeSave (UmiKKStr::SystemStringToKKStr (imageFileName), *(raster->UnmanagedClass ()));
  lastOpSuccessful = dbConn->Valid ();
}  /* ImageFullSizeSave */





UmiRaster^  UmiDataBase::ImageFullSizeFind (String^  imageFileName)
{
  RasterPtr  r = dbConn->ImageFullSizeFind (UmiKKStr::SystemStringToKKStr (imageFileName));
  if  (!r)
    return nullptr;
  return  gcnew UmiRaster (r);
}






//***************************************************************************************
//*                             ImageClass  Routines                                    *
//***************************************************************************************

UmiClass^  UmiDataBase::ImageClassLoad (String^  className)  // Load instance on UmiClass with name = "ClassName"  
                                                                     // if not in database will return nullptr
{
  ImageClassPtr  imageClass = dbConn->ImageClassLoad (UmiKKStr::SystemStringToKKStr (className));
  if  (imageClass == NULL)
    return  nullptr;
  else
    return   UmiClassList::GetUniqueClass (imageClass);
}  /* ImageClassLoad */





UmiClassList^  UmiDataBase::ImageClassLoadList ()
{
  ImageClassListPtr  classes = dbConn->ImageClassLoadList ();
  if  (!classes)
    return  nullptr;


  UmiClass^ rootUmiClass = nullptr;

  UmiClassList^  umiClasses = gcnew UmiClassList ();

  ImageClassList::iterator  idx;
  for  (idx = classes->begin ();  idx != classes->end ();  idx++)
  {
    ImageClassPtr  ic = *idx;

    UmiClass^ c = UmiClassList::GetUniqueClass (UmiKKStr::KKStrToSystenStr (ic->Name ()), nullptr);
    umiClasses->Add (c);
    if  (ic->Name ().EqualIgnoreCase ("ALLCLASSES"))
    {
      rootUmiClass = c;
      umiClasses->RootNode = c;
    }
  }

  if  (rootUmiClass == nullptr)
  {
    // We never loaded the 'ALLCLASSES' root class.
    rootUmiClass = UmiClassList::GetUniqueClass ("AllClasses", nullptr);
    umiClasses->Add (rootUmiClass);
  }

  for  (idx = classes->begin ();  idx != classes->end ();  idx++)
  {
    ImageClassPtr  ic = *idx;

    UmiClass^ pc = umiClasses->LookUpByUnmanagedClass (ic);

    if  (pc == nullptr)
    {
      // This should not have been able to have happened;  but it did;  so lets have a breal point here
      // there must be something wrong with my program logic.
      runLog->Log ().Level (-1) << endl << endl 
                                << "UmiDataBase::ImageClassLoadList   ***ERROR***" << endl
                                << "                                    The Parent Child Relationship is Broken" << endl
                                << "                                    ClassName[" << ic->Name () << "]" << endl
                                << endl;
    }
    else if  (ic->Parent () == NULL)
    {
      if  (pc != rootUmiClass)
      {
        pc->Parent = rootUmiClass;
        rootUmiClass->AddAChild (pc);
      }
    }
    else
    {
      UmiClass^  parent = umiClasses->LookUpByUnmanagedClass (ic->Parent ());
      if  (parent == nullptr)
      {
        pc->Parent = rootUmiClass;
        rootUmiClass->AddAChild (pc);
      }
      else
      {
        pc->Parent = parent;
        parent->AddAChild (pc);
      }
    }
  }

  umiClasses->RootNode = rootUmiClass;

  return  umiClasses;
}  /* ImageClassLoadList */




void  UmiDataBase::ImageClassDelete (String^  className)
{
  dbConn->ImageClassDelete (UmiKKStr::SystemStringToKKStr (className));
}  /* ImageClassDelete */




void  UmiDataBase::ImageClassUpdate (String^    oldClassName,
                                     UmiClass^  umiClass
                                    )

{
  bool b = false; 

  KKStr  oldClassNameKKStr;

  if  (String::IsNullOrEmpty (oldClassName))
    oldClassNameKKStr = UmiKKStr::SystemStringToKKStr (umiClass->Name);
  else
    oldClassNameKKStr = UmiKKStr::SystemStringToKKStr (oldClassName);


  dbConn->ImageClassUpdate (oldClassNameKKStr, *(umiClass->UnmanagedImageClass ()), b);
  lastOpSuccessful = b;
  return;
}



void  UmiDataBase::ImageClassInsert (UmiClass^  umiClass)
{
  bool b = false; 
  dbConn->ImageClassInsert (*(umiClass->UnmanagedImageClass ()), b);
  lastOpSuccessful = b;
}




//*************************************************************************************************
//*                                ImageGroup Routines.                                           *
//*************************************************************************************************
UmiDataBaseImageGroup^  UmiDataBase::ImageGroupLoad (String^  name)
{
  DataBaseImageGroupPtr group = dbConn->ImageGroupLoad (UmiKKStr::SystemStringToKKStr (name));
  if  (!group)
    return nullptr;
  return  gcnew UmiDataBaseImageGroup (group);
}  /* ImageGroupQueryByName */


void  UmiDataBase::ImageGroupInsert (UmiDataBaseImageGroup^  imageGroup)
{
  dbConn->ImageGroupInsert (*(imageGroup->UnManagedClass ()));
  lastOpSuccessful = dbConn->Valid ();
}



UmiDataBaseImageGroupList^  UmiDataBase::ImageGroupLoad ()
{
  DataBaseImageGroupListPtr  imageGroups = dbConn->ImageGroupLoad ();
  if  (!imageGroups)
  {
    lastOpSuccessful = false;
    return  nullptr;
  }

  UmiDataBaseImageGroupList^ managedGroupsList = gcnew UmiDataBaseImageGroupList (imageGroups);
  imageGroups->Owner (false);  // 'managedGroupsList' now owns the contents of 'imageGroups'.
  delete  imageGroups;
  imageGroups = NULL;
  return  managedGroupsList;
}



void  UmiDataBase::ImageGroupDelete (int imageGroupId)
{
  dbConn->ImageGroupDelete (imageGroupId);
  lastOpSuccessful = dbConn->Valid ();
}





//*************************************************************************************************
//*                             ImageGroupEntries Routines.                                       *
//*************************************************************************************************
void  UmiDataBase::ImageGroupEntriesInsert (UmiDataBaseImageGroupEntry^  imageGroupEntry)
{
  dbConn->ImageGroupEntriesInsert (*(imageGroupEntry->UnManagedClass ()));
  lastOpSuccessful = dbConn->Valid ();
}



void  UmiDataBase::ImageGroupEntriesInsert (UmiDataBaseImageGroupEntryList^  imageGroupList)
{
  DataBaseImageGroupEntryListPtr  unmagedList = new DataBaseImageGroupEntryList (false); // will not own contents
  for each (UmiDataBaseImageGroupEntry^ entry in imageGroupList)
  {
    unmagedList->PushOnBack (entry->UnManagedClass ());
  }
  dbConn->ImageGroupEntriesInsert (*unmagedList);
  lastOpSuccessful = dbConn->Valid ();

  delete  unmagedList;
  unmagedList = NULL;
}  /* ImageGroupEntriesInsert */



array<array<String^>^ >^  UmiDataBase::ImageGroupEntriesInsert (int              groupId,
                                                                  List<String^>^   imageFileNames
                                                                 )
{
  VectorKKStr  names;
  for each  (String^ name  in imageFileNames)
    names.push_back (UmiKKStr::SystemStringToKKStr (name));

  KKStrMatrixPtr  resultsKKStr = dbConn->ImageGroupEntriesInsert (groupId, names);
  if  (resultsKKStr == NULL)
    return  nullptr;

  int  numRows = resultsKKStr->NumRows ();
  array<array<String^>^ >^ results = gcnew array<array<String^>^ >(numRows);

  for  (int row = 0;  row < numRows;  row++)
  {
    KKStrList&  rowDataKKStr = (*resultsKKStr)[row];
    uint  numCols = rowDataKKStr.QueueSize ();

    array<String^>^  rowData = gcnew array<String^> (numCols);

    for  (uint col = 0;  col < numCols;  col++)
    {
      rowData[col] = UmiKKStr::KKStrToSystenStr (rowDataKKStr[col]);
    }

    results[row] = rowData;
  }

  lastOpSuccessful = dbConn->Valid ();

  delete  resultsKKStr;  resultsKKStr = NULL;

  return  results;
}  /* ImageGroupEntriesInsert */




void  UmiDataBase::ImageGroupEntriesInsert (int                      groupId, 
                                            UmiDataBaseImageList^  images
                                           )
{
  VectorKKStr  names;
  for each  (UmiDataBaseImage^ image  in images)
    names.push_back (UmiKKStr::SystemStringToKKStr (image->ImageFileName));

  dbConn->ImageGroupEntriesInsert (groupId, names);
  lastOpSuccessful = dbConn->Valid ();
}  /* ImageGroupEntriesInsert */




UmiDataBaseImageGroupEntryList^  UmiDataBase::ImageGroupEntriesLoad (int groupId)
{
  DataBaseImageGroupEntryListPtr  entries = dbConn->ImageGroupEntriesLoad (groupId);
  if  (!entries)
  {
    lastOpSuccessful = false;
    return  nullptr;
  }

  UmiDataBaseImageGroupEntryList^  managedEntries = gcnew UmiDataBaseImageGroupEntryList (entries);
  entries->Owner (false);  // 'managedEntries' now own the contents of 'entries'
  delete  entries;
  entries = NULL;

  return  managedEntries;
}  /* ImageGroupEntriesLoad */




//****************************************************************************************************
//*                                   ScannerFile  routines.                                         *
//****************************************************************************************************


array<String^>^  UmiDataBase::ScannerFileEntriesGetList (String^ cruiseName,
                                                         String^ stationName,
                                                         String^ deploymentNum
                                                        )
{
  VectorKKStr*  scannerFileNames = dbConn->ScannerFileEntriesGetList
       (UmiKKStr::SystemStringToKKStr (cruiseName),
        UmiKKStr::SystemStringToKKStr (stationName),
        UmiKKStr::SystemStringToKKStr (deploymentNum)
       );
     
  if  (!scannerFileNames)
    return nullptr;

  array<String^>^ scannerFileNamesManaged = gcnew array<String^>(scannerFileNames->size ());

  VectorKKStr::iterator  idx;
  int                    idxMan = 0;
  for  (idx = scannerFileNames->begin ();  idx != scannerFileNames->end ();  idx++)
  {
    scannerFileNamesManaged->SetValue (UmiKKStr::KKStrToSystenStr (*idx), idxMan);
    idxMan++;
  }

  delete  scannerFileNames;  scannerFileNames = NULL;
  return  scannerFileNamesManaged;
}  /* ScannerFileEntriesGetList */





UmiScannerFileEntryList^  UmiDataBase::ScannerFileEntryLoad (String^  cruiseName,
                                                             String^  stationName,
                                                             String^  deploymentNum
                                                            )
{
  ScannerFileEntryListPtr  scannerFileEntries 
    = dbConn->ScannerFileEntryLoad (UmiKKStr::SystemStringToKKStr (cruiseName),
                                    UmiKKStr::SystemStringToKKStr (stationName),
                                    UmiKKStr::SystemStringToKKStr (deploymentNum)
                                   );
  if  (scannerFileEntries == NULL)
    return  nullptr;

  UmiScannerFileEntryList^  umiScannerFileEntries = gcnew UmiScannerFileEntryList (scannerFileEntries);

  scannerFileEntries->Owner (false);
  delete  scannerFileEntries;
  scannerFileEntries = NULL;

  return   umiScannerFileEntries;
}  /* ScannerFileEntryLoad */




UmiScannerFileEntry^  UmiDataBase::ScannerFileEntryLoad  (String^ _scannerFileName)
{
  ScannerFileEntryPtr  scannerFileEntry = dbConn->ScannerFileEntryLoad (UmiKKStr::SystemStringToKKStr (_scannerFileName));
  if  (!scannerFileEntry)
    return  nullptr;

  return  gcnew UmiScannerFileEntry (scannerFileEntry);
}





void  UmiDataBase::ScannerFileEntryInsert (UmiScannerFileEntry^  scannerFileEntry)
{
  if  (scannerFileEntry == nullptr)
  {
    lastOpSuccessful  = false;
    throw gcnew Exception ("UmiDataBase::ScannerFileEntryInsert   'nullptr' was passed in.");
  }

  if  ((scannerFileEntry->UnmanagedClass ()))
  {
    lastOpSuccessful  = false;
    throw gcnew Exception ("UmiDataBase::ScannerFileEntryInsert   The Unmanaged Class wass 'NULL'.");
  }

  dbConn->ScannerFileEntryInsert (*(scannerFileEntry->UnmanagedClass ()));
  lastOpSuccessful = dbConn->Valid ();
  return;
}  /* ScannerFileEntryInsert */





void   UmiDataBase::ScannerFileEntryUpdate (UmiScannerFileEntry^  scannerFileEntry)
{
  if  (scannerFileEntry == nullptr)
  {
    lastOpSuccessful  = false;
    throw gcnew Exception ("UmiDataBase::ScannerFileEntryUpdate   'nullptr' was passed in.");
  }

  if  (!(scannerFileEntry->UnmanagedClass ()))
  {
    lastOpSuccessful  = false;
    throw gcnew Exception ("UmiDataBase::ScannerFileEntryUpdate   The Unmanaged Class wass 'NULL'.");
  }

  dbConn->ScannerFileEntryUpdate (*(scannerFileEntry->UnmanagedClass ()));
}





String^  UmiDataBase::GetFullScannerFileName (String^ _scannerFileName)
{
  String^  rootName = UmiOSservices::GetRootName (_scannerFileName);

  KKStr  fullPathName =  ScannerFileEntry::GetFullScannerFileName (UmiKKStr::SystemStringToKKStr (rootName));
  if  (fullPathName.Empty ())
    return nullptr;

  return  UmiKKStr::KKStrToSystenStr (fullPathName);
}  /* GetFullScannerFileName */




