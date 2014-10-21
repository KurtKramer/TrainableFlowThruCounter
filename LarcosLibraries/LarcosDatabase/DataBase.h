#if  !defined(_DATABASE_)
#define  _DATABASE_



#include  "DateTime.h"
#include  "GoalKeeper.h"
#include  "Raster.h"
#include  "RunLog.h"
#include  "Str.h"
using namespace KKB;


#include  "ScannerFileEntry.h"

#include  "ClassStatistic.h"
#include  "FileDesc.h"
#include  "ImageClass.h"

#include  "PostLarvaeFV.h"

#include  "DataBaseImage.h"
#include  "DataBaseImageGroup.h"
#include  "DataBaseImageGroupEntry.h"


#ifndef  _mysql_h
struct  MYSQL;
struct  MYSQL_RES;
struct  MSQL_STRUCT; 
typedef  MSQL_STRUCT* MYSQL_ROW;
struct  MYSQL_FIELD;
#endif

namespace LarcosDatabase
{

  #ifndef  _POSTLARVAEFV_
  class  PostLarvaeFV;
  typedef  PostLarvaeFV*  PostLarvaeFVPtr;

  class  PostLarvaeFVList;
  typedef  PostLarvaeFVList*  PostLarvaeFVListPtr;
  #endif


  #if  !defined(_DATABASESERVER_)
  class  DataBaseServer;
  typedef  DataBaseServer*  DataBaseServerPtr;

  class  DataBaseServerList;
  typedef  DataBaseServerList*  DataBaseServerListPtr;
  #endif


  /**
   @class  DataBase
   @brief DataBase connection management object.  All database comnunications will go through this class.
   @author Kurt Kramer
   @date Dec/01/2008
   */

  class  DataBase
  {
  public:
    typedef  KKB::uchar  uchar;
    typedef  KKB::uint   uint;
    typedef  KKB::ulong  ulong;

    DataBase (RunLog&  _log);

    DataBase (const DataBaseServerPtr  _server,   // Will make own local copy of "_server"
              RunLog&                  _log
             );

    ~DataBase ();


    KKStr  ServerDescription ()  const;   // return description of server that this instance is connected to.

    KKStr  LastErrorDesc ()  const;

    bool                     DuplicateKey () const {return duplicateKey;}
    const DataBaseServerPtr  Server       () const {return server;}
    bool                     Valid        () const {return valid;}

    static
    DataBaseServerListPtr  GetListOfServers (RunLog&  _log);

    int  QueryStatement (const char* queryStr,
                         int         queryStrLen
                        );

    int  QueryStatement (const KKStr&  statement);



    ///  Will make MySQL call contained in 'statement'.  and return back results as a Vector List of KKStr objects.
    ///  colsToReturn  - A list of columns to retirn from the result set.  
    KKStrMatrixPtr  QueryStatement (const KKStr&  statement,
                                    char**        colsToReturn
                                   );

    /// Wil make SQL Query call; if there is a doisconnect from the database;  it will not try to 
    /// recconnect like the other 'QueryStatement2' calls.
    int  QueryStatement2 (const char* queryStr,
                          int         queryStrLen
                         );

    bool  ThreadInit ();  // Call at start of new thread.
    void  ThreadEnd ();   // Call just before thread termonates.


    //***********************************************************************************
    void  FeatureDataCreateTable ();

    void  FeatureDataInsertRow (const KKStr&         _scannerFileName,
                                const PostLarvaeFV&  example
                               );
    

    /// Retrieves FeatureData for a given 'DataBaseImage' object.
    PostLarvaeFVPtr      FeatureDataRecLoad (DataBaseImagePtr  image);


    /// Retrieves FeatureData for a given ImageFileName.
    PostLarvaeFVPtr      FeatureDataRecLoad (const KKStr&  imageFileName);



    PostLarvaeFVListPtr  FeatureDataGetOneScannerFile (const KKStr&         sipperFileRootName,
                                                       const ImageClassPtr  imageClass,
                                                       char                 classKeyToUse,
                                                       bool&                cancelFlag
                                                      );

    PostLarvaeFVListPtr  FeatureDataForImageGroup (const DataBaseImageGroupPtr  imageGroup,
                                                   const ImageClassPtr          imageClass,
                                                   char                         classKeyToUse,
                                                   const bool&                  cancelFlag
                                                  );

    void   FeatureDataUpdate (PostLarvaeFVPtr  example);





    //***********************************************************************************



    ///  Creates and entry in the Images table for 'image',
    ///  if either dimension of 'image' exceeds 100 it will save a reduced version of it in the thumbNail field of 'Images' 
    ///  and a full size version in the ImagesFullSize table.
    void  ImageInsert (const Raster&         image,
                       const KKStr&          imageFileName,
                       const KKStr&          scannerFileName,
                             osFilePos       byteOffset,     // byteOffset of SipperRow containing TopLeftRow
                             uint            topLeftRow,
                             uint            topLeftCol,
                             uint            height,
                             uint            width,
                             uint            pixelCount,
                             uint            centroidRow,
                             uint            centroidCol,
                             ImageClassPtr   class1,
                             float           class1Prob,
                             ImageClassPtr   class2,
                             float           class2Prob,
                             ImageClassPtr   validatedClass,
                             float           depth,
                             int&            imageId,
                             bool&           successful
                       );

    void  ImagesEraseSipperFile (const KKStr&  _scannerFileName);


    DataBaseImagePtr      ImageLoad (uint  imageId);  

    DataBaseImagePtr      ImageLoad (const KKStr&   imageFileName);  // Root name of image.

    DataBaseImagePtr      ImagesLocateClosestImage (const KKStr&  imageFileName);

    DataBaseImagePtr      ImagesLocateClosestImage (const KKStr&  scannerFileName,
                                                    uint          scanLine,
                                                    uint          scanCol
                                                   );



    DataBaseImageListPtr  ImageQuery (DataBaseImageGroupPtr  group,
                                      bool                   includeThumbnail,
                                      const bool&            cancelFlag
                                     );
      

   
    DataBaseImageListPtr  ImageQuery (DataBaseImageGroupPtr  imageGroup,
                                      const KKStr&           scannerFileName,
                                      ImageClassPtr          imageClass,
                                      char                   classKeyToUse,
                                      float                  probMin,
                                      float                  probMax,
                                      int                    sizeMin,
                                      int                    sizeMax,
                                      float                  depthMin,
                                      float                  depthMax,
                                      uint                   restartImageId,
                                      int                    limit,            // Max # of rows to return.  -1 indicates no limit.
                                      bool                   includeThumbnail,
                                      const bool&            cancelFlag
                                     );



    DataBaseImageListPtr  ImageQuery (DataBaseImageGroupPtr imageGroup,
                                      const KKStr&          cruiseName,
                                      const KKStr&          stationName,
                                      const KKStr&          deploymentNum,
                                      ImageClassPtr         imageClass,
                                      char                  classKeyToUse,
                                      float                 probMin,
                                      float                 probMax,
                                      int                   sizeMin,
                                      int                   sizeMax,
                                      float                 depthMin,
                                      float                 depthMax,
                                      uint                  restartImageId,
                                      int                   limit,            // Max # of rows to return.  -1 indicates no limit.
                                      bool                  includeThumbnail,
                                      const bool&           cancelFlag
                                     );


    DataBaseImageListPtr  ImageQuery (const KKStr&   cruiseName,
                                      const KKStr&   stationName,
                                      const KKStr&   deploymentNum,
                                      ImageClassPtr  imageClass,
                                      char           classKeyToUse,
                                      float          minProb,
                                      float          minSize,
                                      const KKStr&   dataField1Name,
                                      float          dataField1Min,
                                      float          dataField1Max,
                                      const KKStr&   dataField2Name,
                                      float          dataField2Min,
                                      float          dataField2Max,
                                      const KKStr&   dataField3Name,
                                      float          dataField3Min,
                                      float          dataField3Max,
                                      uint           restartImageId,
                                      int            limit            // Max # of rows 2 return.  -1 idicates no limit.
                                     );


    DataBaseImageListPtr  ImageQueryByGrouop 
                                    (DataBaseImageGroupPtr  imageGroup,
                                     const KKStr&           cruiseName,
                                     const KKStr&           stationName,
                                     const KKStr&           deploymentNum,
                                     const KKStr&           scannerFileName,
                                     ImageClassPtr          imageClass,
                                     char                   classKeyToUse,
                                     float                  probMin,
                                     float                  probMax,
                                     int                    sizeMin,
                                     int                    sizeMax,
                                     float                  depthMin,
                                     float                  depthMax,
                                     uint                   restartImageId,
                                     int                    limit,            // Max # of rows to return.  -1 indicates no limit.,
                                     bool                   includeThumbnail
                                    );
    

    DataBaseImageListPtr  ImageQueryForScanLineRange (const KKStr&   scannerFileName,
                                                      uint           scanLineStart,
                                                      uint           scanLineEnd
                                                     );


    VectorKKStr*          ImageListOfImageFileNamesByScanLineRange (const KKStr&   scannerFileName,
                                                                    uint           scanLineStart,
                                                                    uint           scanLineEnd
                                                                   );




    void  ImageUpdatePredictions (const KKStr&   imageFileName,
                                  ImageClassPtr  class1Pred,
                                  float          class1Prob,
                                  ImageClassPtr  class2Pred,
                                  float          class2Prob
                                 );


    void  ImageUpdateValidatedClass (const KKStr&   imageFileName, 
                                     ImageClassPtr  imageClass
                                    );

    /// Update both the Validates and Class1Name
    void  ImageUpdateValidatedAndPredictClass (const KKStr&   imageFileName, 
                                               ImageClassPtr  imageClass, 
                                               float          class1Prob
                                              );

    void  ImageUpdate (DataBaseImage&  dbImage,
                       Raster&         image
                      );

    ClassStatisticListPtr  ImageGetClassStatistics (DataBaseImageGroupPtr  imageGroup,
                                                    const KKStr&           scannerFileName,
                                                    ImageClassPtr          imageClass,
                                                    char                   classKeyToUse,
                                                    float                  minProb,
                                                    float                  maxProb,
                                                    int                    minSize,
                                                    int                    maxSize,
                                                    float                  minDepth,
                                                    float                  maxDepth
                                                   );


    ClassStatisticListPtr  ImageGetClassStatistics (DataBaseImageGroupPtr  imageGroup,
                                                    const KKStr&           cruiseName,
                                                    const KKStr&           stationName,
                                                    const KKStr&           deploymentNum,
                                                    ImageClassPtr          imageClass,
                                                    char                   classKeyToUse,
                                                    float                  minProb,
                                                    float                  maxProb,
                                                    int                    minSize,
                                                    int                    maxSize,
                                                    float                  minDepth,
                                                    float                  maxDepth
                                                   );



    /// Will return a pointer to a vector<int> instance that will contain counts of images
    /// by depth.  Each ealement in the vector will represent a depth range such that 
    /// index 0 will be for depth range (0 <= images.Depth < depthIncrements)
    /// index 1 will ""     ""      ""  (depthIncrements <= images.Depth < (2 * depthIncrements))
    VectorUint*  ImageGetDepthStatistics (DataBaseImageGroupPtr  imageGroup,
                                          const KKStr&           scannerFileName,
                                          float                  depthIncrements,
                                          ImageClassPtr          imageClass,
                                          char                   classKeyToUse,
                                          float                  minProb,
                                          float                  maxProb,
                                          int                    minSize,
                                          int                    maxSize
                                         );


    VectorUint*  ImageGetDepthStatistics (DataBaseImageGroupPtr  imageGroup,
                                          const KKStr&           cruiseName,
                                          const KKStr&           stationName,
                                          const KKStr&           deploymentNum,
                                          float                  depthIncrements,
                                          ImageClassPtr          imageClass,
                                          char                   classKeyToUse,
                                          float                  minProb,
                                          float                  maxProb,
                                          int                    minSize,
                                          int                    maxSize
                                         );



    
    
    //***********************************************************************************
    RasterPtr  ImageFullSizeLoad (const KKStr&  imageFileName);


    void       ImageFullSizeSave (const KKStr&    imageFileName,
                                  const Raster&   raster
                                 );

    /// Will locate a original size version of the image; it will try in order 3 diferent sources.
    /// 1) ThumbNail in Images Table.
    /// 2) FullSizeImage in ImagesFullSize table
    /// 3) will go to ScannerFile
    RasterPtr  ImageFullSizeFind (const KKStr&  imageFileName);




    //***********************************************************************************
    ImageClassPtr      ImageClassLoad (const KKStr&  className);

    ImageClassListPtr  ImageClassLoadList ();

    void  ImageClassInsert (ImageClass&  imageClass,
                            bool&        successful
                           );

    void  ImageClassUpdate (const KKStr&       oldClassName,
                            const ImageClass&  imageClass,
                            bool&              successful
                           );

    void  ImageClassDelete (const KKStr&  imageClassName);

    void  ImagesEraseScannerFileEntry (const KKStr&  _scannerFileName);



    //***********************************************************************************
    void  ImageGroupInsert (DataBaseImageGroup&  imageGroup);

    DataBaseImageGroupListPtr  ImageGroupLoad ();   // returns a list of all Group Assignments

    DataBaseImageGroupPtr      ImageGroupLoad (const KKStr&  name);  

    void  ImageGroupDelete (int imageGroupId);



    //***********************************************************************************
    void  ImageGroupEntriesInsert (const DataBaseImageGroupEntry&  imageGroupEntry);

    void  ImageGroupEntriesInsert (const DataBaseImageGroupEntryList&  imageGroupList);

    // Will insert a list of images into the same group in ImageGroupEntries.  A list of ImageFileNames
    // that failed to be inserted with a description of the error will be returned.  If NULL is returned
    // the the nsertion failed completely.   A 2 x n matrix will be returend, One row for each image that 
    // failed insertion.  [0,0] = ImageFileName for row '0' that failed insertion, 
    //                    [0,1] = Description of error that caused insertion to fail.
    //
    // Since there is a limitation on the length ogf the MySQL parameter of 64K  we need to make sure that the
    // length of all the imageFileNames will be less than 64K  that is  sum_i (ImageFileNamess[i].Len ()) + imageFileNams.size () < 64000.
    KKStrMatrixPtr  ImageGroupEntriesInsert (int                 groupId,         // Will insert a list of images into the same 
                                             const VectorKKStr&  imageFileNames   // group
                                            );

    DataBaseImageGroupEntryListPtr  ImageGroupEntriesLoad (int groupId);




    //********************************************************************************************
    KKLSC::ScannerFileEntryListPtr  ScannerFileEntryProcessResults ();


    KKLSC::ScannerFileEntryPtr  ScannerFileEntryLoad (const KKStr&  _scannerFileName);


    VectorKKStr*  ScannerFileEntriesGetList (const KKStr& cruiseName,
                                             const KKStr& stationName,
                                             const KKStr& deploymentNum
                                            );


    KKLSC::ScannerFileEntryListPtr  ScannerFileEntryLoad (const KKStr& cruiseName,
                                                          const KKStr& stationName,
                                                          const KKStr& deploymentNum
                                                       );


    void  ScannerFileEntryInsert (KKLSC::ScannerFileEntry&  scannerFileEntry);



    void   ScannerFileEntryUpdate (KKLSC::ScannerFileEntry&   scannerFileEntry);


    void   ScannerFileEntryUpdateFileSizeStats (const KKStr&  _scannerFileName, 
                                                long long     _sizeInBytes, 
                                                uint          _numScanLines
                                               );


    /**
     *@brief  Delete the contents of a single Scanner File Entry from the database from all tables such as Images, FeatureData, etc.
     *@param[in]  _scannerFileName  Name of Scanner file to delete.
     */
    void   ScannerFileEntryDelete (const KKStr&  _scannerFileName);

    void   ResultSetsClear ();

  private:
    static  GoalKeeperPtr  blocker;   /*!< used to handle multithreading issues. */

    static  void   CreateBlocker ();  /*!< Call this to make sure that Blocker exists.  Will handle race conditions. */

    void    InitilizeMySqlLibrary ();

    DataBaseServerPtr  GetDefaultMySqlParameters ();


    int  Connect ();



    // Generalized routines
    KKB::DateTime  DateTimeFromMySqlDateTimeField (KKStr  field);

    KKB::DateType  DateFromMySqlDateTimeField (KKStr  field);

    KKStr   DateTimeToQuotedStr (const KKB::DateTime&  dt);

    

    static char**       featureDataFieldNames;
    static char**       GetFeatureDataFieldNames ();

    PostLarvaeFVListPtr  FeatureDataProcessResults ();  // Used by the FeatureData retrieval routines.



    uchar*  EncodeARasterImageIntoAThumbNail (const Raster&  image,
                                              uint           maxDimSizeAllowed,
                                              uint&          buffLen
                                             );

    DataBaseImageListPtr   ImageQueryProcessResults ();

    ClassStatisticListPtr  ImageProcessClassStaticsResults ();

    static  char*  scannerFileEntryFieldNames[];



    RunLog&  log;

    MYSQL*             conn;
    DataBaseServerPtr  server;              // We will own this instance
    bool               duplicateKey;        // Set to true juring insert if a duplicate key error ccurs
    bool               valid;               // Will nbe set to false if cnection is bad
    uint               lastMySqlErrorNo;
    KKStr              lastMySqlErrorDesc;
    KKStr              prevQueryStatement;

    FileDescPtr        featureFileDesc;

    static int         numDataBaseInstances;  // Will be incremented when the contructor is called and
                                              // decremented when the desructor is called.  This way the
                                              // Constructor and Destrctors will know if they should call
                                              // "mysql_library_init" and "mysql_library_end" respectively.


    MYSQL_FIELD*       resultSetFieldDefs;
    MYSQL_RES*         resultSet;
    uint               resultSetNumFields;
    uint               resultSetNumRows;
    bool               resultSetMore;   
    MYSQL_ROW          resultSetNextRow;
    VectorInt          resultSetFieldIndex;
    ulong*             resultSetLengths;


    void   ResultSetBuildFieldIndexTable (char**  fieldNames);
    bool   ResultSetLoad    (char**  fieldNames);
    bool   ResultSetGetNext (char**  fieldNames);
    bool   ResultSetFetchNextRow ();
    void   ResulSetFree ();

    
    bool           ResultSetGetBool          (uint fieldIdx);
    const char*    ResultSetGetField         (uint fieldIdx);
    ulong          ResultSetGetFieldLen      (uint fieldIdx);
    KKStr          ResultSetGetKKStrField    (uint fieldIdx);
    KKB::DateTime  ResultSetGetDateTimeField (uint fieldIdx);
    double         ResultSetGetDoubleField   (uint fieldIdx);
    float          ResultSetGetFloatField    (uint fieldIdx);
    int            ResultSetGetIntField      (uint fieldIdx);
    RasterPtr      ResultSetGetRasterField   (uint fieldIdx);
    uint           ResultSetGetUintField     (uint fieldIdx);
    int            ResultSetGetUlongField    (uint fieldIdx);
  };


  typedef  DataBase*  DataBasePtr;

}  /* namespace LarcosDatabase */


#endif
