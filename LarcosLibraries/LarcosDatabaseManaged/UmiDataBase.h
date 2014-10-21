#ifndef  _UMIDATABASE_
#define  _UMIDATABASE_

#include  "DataBase.h"
#include  "DateTime.h"
#include  "UmiClassList.h"
#include  "UmiClassStatistic.h"
#include  "UmiDataBaseServer.h"
#include  "UmiDataBaseImage.h"
#include  "UmiDataBaseImageGroup.h"
#include  "UmiDataBaseImageGroupEntry.h"
#include  "UmiFeatureVector.h"
#include  "UmiFeatureVectorList.h"
#include  "UmiScannerFileEntry.h"
#include  "UmiRunLog.h"



namespace LarcosDatabaseManaged 
{
  /*!
   \class UmiDataBase
   All access to the MySQL database is done through this class.  Each one of these methods calls
   the matching method in the unmanaged class DataBase in the ScsLibrary.
   */

  public ref class UmiDataBase
  {
  public:
    static   UmiDataBase^  globaDbConn = nullptr;

    static   UmiDataBase^  GetGlobalDatabaseManager (UmiRunLog^  _runLog);

    static   UmiDataBase^  GetGlobalDatabaseManagerNewInstance (UmiRunLog^  _runLog);  // Will uinstatiate a new instance of the current GlobalDatabase Connecetion.

    static   UmiDataBase^  SelectNewDataBaseServer (UmiRunLog^  _runLog);


    UmiDataBase (UmiRunLog^  _runLog);

    UmiDataBase (UmiDataBaseServer^  _server,
                   UmiRunLog^          _log
                  );
    
  protected:
    !UmiDataBase ();

  private:
    ~UmiDataBase ();

  public:

    array<array<String^>^ >^  QueryStatement (String^          statement,
                                              array<String^>^  colsToReturn
                                             );

    property  bool                  CancelFlag  {bool  get ();  void  set (bool  _cancelFlag);}

    property  UmiDataBaseServer^  Server      {UmiDataBaseServer^  get ();}   //  Setting this property to 'True' causes what ever db operartion to cancel.

    bool  DuplicateKey ()  {return  dbConn->DuplicateKey ();}

    String^  ServerDescription ();

    String^  LastErrorDesc ();

    bool  Successful ()  {return  lastOpSuccessful;}

    bool  ThreadInit ();  /*!< Call at start of new thread.         */
    void  ThreadEnd ();   /*!< Call just before thread termonates.  */


    bool  Valid ();


    //*******************************************************************************************
    void  FeatureDataInsertRow (String^              _scannerFileName,
                                UmiFeatureVector^  _example
                               );
  

    UmiFeatureVector^  FeatureDataRecLoad (String^  imageFileName);

    UmiFeatureVector^  FeatureDataRecLoad (UmiDataBaseImage^  image);


    UmiFeatureVectorList^  FeatureDataGetOneScannerFile (String^        scannerFileName,
                                                          UmiClass^    imageClass,
                                                          System::Char   classKeyToUse,
                                                          bool           reExtractInstrumentData
                                                         );

    
    UmiFeatureVectorList^  FeatureDataForImageGroup (UmiDataBaseImageGroup^  imageGroup,
                                                     UmiClass^               imageClass,
                                                     System::Char              classKeyToUse
                                                    );


    void   FeatureDataUpdate (UmiFeatureVector^  example);






    //*******************************************************************************************
    UmiDataBaseImage^      ImageLoad (uint  imageId);


    UmiDataBaseImage^      ImageLoad (String^   imageFileName);  // Root name of image.


    UmiDataBaseImageList^  ImageQuery (UmiDataBaseImageGroup^  group,
                                       bool                      includeThumbnail
                                      );

    UmiDataBaseImageList^  ImageQuery (UmiDataBaseImageGroup^  imageGroup,
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
                                      );


    UmiDataBaseImageList^  ImageQuery (UmiDataBaseImageGroup^  imageGroup,
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
                                      );



    UmiDataBaseImageList^  ImageQueryByGrouop 
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
                                  );



    UmiClassStatisticList^  ImageGetClassStatistics (UmiDataBaseImageGroup^  imageGroup,
                                                     String^                   scannerFileName,
                                                     UmiClass^               imageClass,
                                                     System::Char              classKeyToUse,
                                                     float                     minProb,
                                                     float                     maxProb,
                                                     int                       minSize,
                                                     int                       maxSize,
                                                     float                     minDepth,
                                                     float                     maxDepth
                                                    );


    UmiClassStatisticList^  ImageGetClassStatistics (UmiDataBaseImageGroup^  imageGroup,
                                                     String^                   cruiseName,
                                                     String^                   stationName,
                                                     String^                   deploymentNum,
                                                     UmiClass^                 imageClass,
                                                     System::Char              classKeyToUse,
                                                     float                     minProb,
                                                     float                     maxProb,
                                                     int                       minSize,
                                                     int                       maxSize,
                                                     float                     minDepth,
                                                     float                     maxDepth
                                                    );



    array<uint>^  ImageGetDepthStatistics (UmiDataBaseImageGroup^  imageGroup,
                                           String^                   scannerFileName,
                                           float                     depthIncrements,
                                           UmiClass^               imageClass,
                                           System::Char              classKeyToUse,
                                           float                     minProb,
                                           float                     maxProb,
                                           int                       minSize,
                                           int                       maxSize
                                          );


    array<uint>^  ImageGetDepthStatistics (UmiDataBaseImageGroup^  imageGroup,
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
                                          );



    void  ImageUpdatePredictions (String^      imageFileName,
                                  UmiClass^  class1Pred,
                                  float        class1Prob,
                                  UmiClass^  class2Pred,
                                  float        class2Prob
                                 );



    void  ImageUpdateValidatedClass (String^      imageFileName, 
                                     UmiClass^  imageClass
                                    );

    void  ImageUpdateValidatedAndPredictClass (String^     imageFileName, 
                                               UmiClass^ imageClass, 
                                               float       class1Prob
                                              );




    //***********************************************************************************
    UmiRaster^  ImageFullSizeLoad (String^  imageFileName);


    void       ImageFullSizeSave (String^       imageFileName,
                                  UmiRaster^  raster
                                 );

    /// Will locate a original size version of the image; it will try in orger 3 diferent sources.
    /// 1) ThumbNail in Images Table.
    /// 2) FullSizeImage in ImagesFullSize table
    /// 3) will go to ScannerFile
    UmiRaster^  ImageFullSizeFind (String^  imageFileName);




    //*******************************************************************************************
    UmiClass^              ImageClassLoad (String^  className);  // Load instance on UmiClass with name = "ClassName"  
                                                                 // if not in database will return nullptr

    UmiClassList^          ImageClassLoadList ();  // Will load all class'es from "Scs.Classes"  table.

    void                   ImageClassInsert (UmiClass^  umiClass);

    void                   ImageClassUpdate (String^      oldClassName,   // if == nullptr  assumed taht name has not changed.
                                             UmiClass^  umiClass
                                            );

    void                   ImageClassDelete (String^  className);



    //***********************************************************************************
    void  ImageGroupInsert (UmiDataBaseImageGroup^  imageGroup);

    UmiDataBaseImageGroupList^  ImageGroupLoad ();   // returns a list of all Group Assignments

    UmiDataBaseImageGroup^  ImageGroupLoad (String^  name);
    
    void  ImageGroupDelete (int imageGroupId);







    //***********************************************************************************
    void  ImageGroupEntriesInsert (UmiDataBaseImageGroupEntry^  imageGroupEntry);

    void  ImageGroupEntriesInsert (UmiDataBaseImageGroupEntryList^  imageGroupList);


    array<array<String^>^ >^  ImageGroupEntriesInsert (int             groupId,
                                                       List<String^>^  imageFileNames
                                                      );

    void  ImageGroupEntriesInsert (int                    groupId, 
                                   UmiDataBaseImageList^  images
                                  );

    UmiDataBaseImageGroupEntryList^  ImageGroupEntriesLoad (int groupId);





    //*******************************************************************************************



   

    //`*******************************************************************************************
    array<String^>^           ScannerFileEntriesGetList 
                                        (String^  cruiseName,
                                         String^  stationName,
                                         String^  deploymentNum
                                        );

    UmiScannerFileEntryList^  ScannerFileEntryLoad 
                                        (String^  cruiseName,
                                         String^  stationName,
                                         String^  deploymentNum
                                        );

    UmiScannerFileEntry^      ScannerFileEntryLoad  (String^ _scannerFileName);

    void                      ScannerFileEntryInsert (UmiScannerFileEntry^  scannerFileEntry);

    void                      ScannerFileEntryUpdate (UmiScannerFileEntry^  scannerFileEntry);

    String^                   GetFullScannerFileName (String^ _scannerFileName);



  private:
    DataBasePtr  dbConn;
    UmiRunLog^   runLog;

    bool*        cancelFlag;   // Some operations need a cancelFlag;  for example "ImageQuery".

    bool  lastOpSuccessful;
  };  /* UmiDataBase */
}
#endif

