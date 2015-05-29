/*
 * LarcosCounterManager.h
 * Copyright (C) 2011-2013  Kurt Kramer
 * For conditions of distribution and use, see copyright notice in LarcosCounterUnManaged.txt
 */

#if  !defined(_LARCOSCOUNTERMANAGER_)
#define  _LARCOSCOUNTERMANAGER_


#include "GoalKeeper.h"
#include "KKThread.h"
#include "MsgQueue.h"
#include "KKObservable.h"
#include "OSservices.h"
#include "Raster.h"
#include "RasterBuffer.h"

#include "Classifier2.h"
#include "TrainingProcess2.h"
using namespace  KKMLL;

#include "FlowMeterTracker.h"
#include "ScannerHeaderFields.h"
#include "ScannerFile.h"
using namespace KKLSC;

#include "OperatingParameters.h"
#include "StatusSnapshot.h"

#include "PreClassDefinitions.h"

/**
 *@namespace  LarcosCounterUnManaged
 *@brief  This namespace contains the code that is specific to the LARCOS Counter application.
 *@details  This namespace is meant to be a library that is O/S neutral; that is it has no O/S specific 
 * code.  It has all the data structures necessary to support the LARCOS counter.  The main class
 * in this routine is 'LarcosCounterManager'; all communication to and from this library should be
 * through a instance of that class.
 */


namespace LarcosCounterUnManaged 
{
  /**
   *@brief  This is the main class used to manage the LARCOS counter application.
   *@details  A  GUI based application would create one instance of the class and communicate to it
   * thru the various methods.  As the user interacts with the application by pressing buttons such as
   * a request to connect to the camera the GUI application would call the "ConnectButtonPressed" method.
   * This function would then fire off a thread that would manage the connection to the camera.  The user 
   * application would then monitor the current status of the Counter by calling methods such as
   * 'GetStats' which utilizes an instance of 'LarcosCounterStats' to pass back current Counter Status.
   */
  class  LarcosCounterManager: public  KKObservable
  {
  public:
    typedef  ScannerFile::Format  ScannerFileFormat;

    typedef  KKThread::ThreadStatus  ThreadStatus;

    typedef  KKThread::ThreadPriority  ThreadPriority;

    enum class CounterState: kkint16 
                 {csNULL, 
                  Stopped, 
                  Stopping, 
                  Connected, 
                  Connecting, 
                  Starting,
                  BuildingClassifier,  // This step is done when ever we Start a new session or replay a previous one.
                  Running,
                  PlayingBack,
                  DroppedFrames
                 };

    static
    KKStr  CounterStateToStr (CounterState  state);


    enum  class LarcosOperatingModes
                  {lomNULL,
                   User,
                   Advanced,
                   Invalid
                  };

    static 
    const KKStr&  LarcosOperatingModeToStr (LarcosOperatingModes om);

    static
    LarcosOperatingModes  LarcosOperatingModeFromStr (const KKStr&  s);


    LarcosCounterManager (MsgQueuePtr  _msgQueue,
                          MsgQueuePtr  _loggedMsgs,
                          int          _maxNumOfThreads
                         );

    ~LarcosCounterManager ();


    /**
     *@brief  Returns an estimate of the amount of memory consumed in bytes.
     *@details This will help managed objects keep track of how much memory 
     * they are using in the unmanaged world.
     */
    kkint64   MemoryConsumedEstimated ();


    /**
     *@brief  Call this method after the instance is created and before any other method is called.
     *@details  Performs setup tasks such as loading LarcosCounterManaer configuration file; makes 
     * initial attempt to connect to camera, etc...
     */
    void  Initialize ();

    KKStr  LarcosVersionNumber ();

    CounterState  CurState ()  {return curState;}

    LarcosOperatingModes  OperatingMode ()  {return   operatingMode;}

    
    StatusSnapshot::FieldIdx  ThroughPutField () const  {return throughPutField;}


    /** Returns the current Installation Configuration. */
    const InstallationConfigPtr  Installation ()  const  {return  installation;}



    //************************************************************************************************************************************
    // Messaging Methods.

    /**@brief   Will take ownership of 'msg' and append to 'msgQueue'; where 'LoggerThread' will write to log file. */
    void  AddMsg (KKStrPtr  msg);

    /**@brief  A copy of the message 'msg' and append to 'msgQueue'; where 'LoggerThread' will write to log file. */
    void  AddMsg (const KKStr&  msg);  

    /**
     * Returns a list of all messages, since last call, that have been added by 'AddMsg' or 'RunLog' from all threads.
     */
    KKStrListPtr  GetAllLoggedMsgs ();


    //************************************************************************************************************************************
    // Methods for retrieving Camera info, etc.

    static  CameraParametersListPtr  GetCameraList (MsgQueuePtr  _msgQueue);


    static  CameraParametersPtr  PromptForCamera (RunLog&  runLog);




    //************************************************************************************************************************************
    // Methods that are called in response to user requests such as when a button is pressed; such as "Start-A-Recording-Session"

    void  AutoGainButtonPressed (bool&   _successful,
                                 KKStr&  _errMsg
                                ); 


    void  ConnectButtonPressed ();


    /** 
     *@brief  Call this method when the user presses the Record button.
     *@details  This will cause the 'RecordButtonThread' to be started. That thread
     * will then manage a recording session until it is requested to stop. It will 
     * initiate several other threads that perform the actual recording and counting of shrimp.
     */
    void  RecordButtonPressed (SessionParametersPtr     _sessionParameters,
                               OperatingParametersPtr   _operatingParameters,
                               bool&                    _successful,
                               KKStr&                   _errMsg
                              );

    /**
     *@brief The user has requested to play back a previously recorded ScannerFile.
     *@details This will cause the 'PlayBackButtonThread' to be started.  It will perform similar
     * actions as that of the 'RecordButtonPressed' method.
     */
    void  PlayBackButtonPressed (const KKStr&           _srcScannerFileName,
                                 SessionParametersPtr   _sessionParameters,
                                 OperatingParametersPtr _operatingParameters,
                                 bool&                  _successful,
                                 KKStr&                 _errMsg
                                );

    void  StopButtonPressed (bool    _stopImmediately,
                             bool&   _successful,
                             KKStr&  _errMsg
                            );





    //************************************************************************************************************************************

    void  ShutDownAllThreads ();   /**< Signal all threads to stop at earliest convenience; for ex: finish whatever is 
                                    * in their queues before shutting down. 
                                    */


    void  TerminateAllThreads ();  /**< Shutdown all threads immediately;  */


    void  SetOperatingParameters (const OperatingParametersPtr  _operatingParameters);

    void  SetDefaultOperatingParameters (const OperatingParametersPtr  _defaultOperatingParameters);


    /**
     *@brief  Informs that a new TrainingModel is to be used; model will be loaded and OperatingParaneters specified will be
     * update from any provided parameters from the Specified configuration file.
     *@details  This method will load the configuration file specified by '_trainingModelName' and retrieve Larcos specific
     * variables and update the appropriate fields in 'operatingParameters'; If '_opParms is provided it will be refreshed 
     * with the latest values retrieved from the config file.
     *
     *@param[in]     _trainingModelName  The name of configuration file that contains the Training Model parameters.
     *@param[out]    _successful         Indicates if a valid Training Model.
     *@param[out]    _opParms            Will be refreshed with new OperatingParameters after retrieving any settings from the config file.
     */
    void  SetTrainingModel (const KKStr&            _trainingModelName,
                            bool&                   _successful,
                            OperatingParametersPtr  _opParms
                           );


    // A call back method that can be called by one of the managed threads; typically a CameraAcquision thread.
    void  FrameHeightWidthChanged (kkint32  _newFrameHeight,
                                   kkint32  _newFrameWidth
                                  );

    void  ResetCounts  ();

    void  HeaderFieldsClear ();

    void  AddHeaderField (const KKStr&  _fieldName,
                          const KKStr&  _fieldValue
                         );

    void  AddHeaderField (const KKStr&  _fieldName,
                          bool          _fieldValue
                         );

    void  AddHeaderField (const KKStr&  _fieldName,
                          kkint32       _fieldValue
                         );

    void  AddHeaderField (const KKStr&  _fieldName,
                          kkint64       _fieldValue
                         );

    void  AddHeaderField (const KKStr&  _fieldName,
                          float         _fieldValue
                         );

    void  AddHeaderField (const KKStr&  _fieldName,
                          double        _fieldValue
                         );

    ScannerHeaderFieldsPtr  HeaderFields ()  {return headerFields;}


    void   AddMessageToCurrentScannerFile (const KKStr&  msg);

    void   BackGroundPixelTH          (uchar _backGroundPixelTH);

    void   FlatFieldEnabled           (bool  _flatFieldEnabled);

    void   LastScannerFileCounted     (const KKStr& _lastScannerFileCounted)  {lastScannerFileCounted = _lastScannerFileCounted;}

    void   MinSizeThreshold           (kkint32  _minSizeThreshold);

    void   NameLastRecordedFile       (const KKStr&  _nameLastRecordedFile)   {nameLastRecordedFile = _nameLastRecordedFile;}

    void   PlayingBackRealTime        (bool   _playingBackRealTime);

    void   RequestedAnalogGain        (float  _requestedAnalogGain);

    void   RequestedDigitalGain       (kkint32  _requestedDigitalGain);

    void   RequestedScanRate          (float  _requestedScanRate);

    void   RequestedSensitivityMode   (const KKStr&  _ensitivityMode);

    void   SampleLastFrameBeforeFlatField (bool _sampleLastFrameBeforeFlatField);

    void   SnapshotInterval           (kkint32 _snapshotInterval);
    
    void   ThroughPutField            (StatusSnapshot::FieldIdx  _throughPutField)  {throughPutField = _throughPutField;}

    void   TotalBytesRead             (kkint64 _totalBytesRead)   {totalBytesRead   = _totalBytesRead;}

    void   TotalBytesToRead           (kkint64 _totalBytesToRead)   {totalBytesToRead = _totalBytesToRead;}


    /***********************************************************************************/
    /*                                Access Methods                                   */
    /***********************************************************************************/
    uchar         BackGroundPixelTH              () const {return operatingParameters->BackGroundPixelTH ();}
    KKStr         CameraSerialNum                ();
    KKStr         CameraMacAddress               ();
    bool          CameraThreadRunning            () const;
    bool          CameraAutoGainThreadRunning    () const;
    kkint32       CameraFrameHeight              () const {return cameraFrameHeight;}
    kkint32       CameraFrameWidth               () const {return cameraFrameWidth;}
    kkint32       ConnectedComponentDist         () const {return operatingParameters->ConnectedComponentDist ();}
    kkint32       CropLeft                       ();
    kkint32       CropRight                      ();
    float         CurAnalogGain                  () const;
    kkint32       CurDigitalGain                 () const;
    float         CurScanRate                    () const;
    bool          DataIsToBeRecorded             () const {return operatingParameters->DataIsToBeRecorded ();}
    bool          DataIsToBeCounted              () const {return operatingParameters->DataIsToBeCounted  ();}
    const KKStr&  Description                    () const;                                 /**< Same as SessionDescription  */
    bool          DiskWritingThreadRunning       () const;
    const KKStr&  DropFolderToLarcos             () const {return dropFolderToLarcos;}
    bool          EmbeddedFlowMeter              () const;                                 /**< Indicates if 1st 4 bytes of scan line will contain flow-meter counter value. */
    kkint32       ErosionStructSize              () const {return operatingParameters->ErosionStructSize ();}
    bool          FlatFieldEnabled               () const {return operatingParameters->FlatFieldEnabled  ();}
    float         FlowRateFactor                 () const {return operatingParameters->FlowRateFactor    ();}
    int           FrameProcessorsCount           () const;
    kkint32       FrameWidthInPixels             () const;
    bool          GenerateFinaleReport           ();                                       /**< Indicates that Counting Session completed normally and it is Okay to generate Finale report.  */
    float         ImagingChamberWidth            () const;                                 /**< Width of imaging chamber in meters.  */
    kkint32       ImagingChamberWidthPixels      () const;
    float         ImagingChamberPixelsPerMM      () const;
    bool          IsRecording                    () const;                                 /**< Indicates if we are currently recording data.                                                 */
    bool          IsRunning                      () const;                                 /**< 'curState' indicates that we are running;  ex "DroppedFrames", "Running", "PlayingBack" */
    KKStr         LastScannerFileCounted         () const {return lastScannerFileCounted;}
    bool          LogicalFrameBuilderRunning     ();
    kkint32       LogicalFrameQueueSizeMax       () const {return logicalFrameQueueSizeMax;}
    kkint32       MinSizeThreshold               () const {return operatingParameters->MinSizeThreshold ();}
    const KKStr&  NameLastRecordedFile           () const {return nameLastRecordedFile;}   /**< Name of last Scanner File recorded from the camera. */
    kkint32       ParticlesCounted               ();
    bool          PlayingBack                    () const {return operatingParameters->PlayingBack ();}
    bool          PlayingBackRealTime            () const {return playingBackRealTime;}
    float         RequestedAnalogGain            () const {return operatingParameters->RequestedAnalogGain      ();};
    kkint32       RequestedDigitalGain           () const {return operatingParameters->RequestedDigitalGain     ();}
    float         RequestedScanRate              () const {return operatingParameters->RequestedScanRate        ();}
    const KKStr&  RequestedSensitivityMode       () const {return operatingParameters->RequestedSensitivityMode ();}
    const KKStr&  ControlNum                     ();
    bool          SampleLastFrameBeforeFlatField () const {return sampleLastFrameBeforeFlatField;}
    bool          SaveParticleImages             () const {return operatingParameters->SaveParticleImages ();}
    const KKStr&  SessionDescription             () const {return Description ();}
    const KKStr&  SrcScannerFileName             () const {return srcScannerFileName;}
    kkint32       SnapshotInterval               () const {return snapshotInterval;}
    bool          StoppingIsInProgress           () const;
    kkint64       TotalBytesRead                 () const {return totalBytesRead;}
    kkint64       TotalBytesToRead               () const {return totalBytesToRead;}
    const KKStr&  TrainingModelName              () const;
    bool          WeAreConnectedToCamera         () const;  /**< Returns true is the acquisition thread exists and it is connected to a camera or reading from a file. */
    bool          WeAreConnectingToCamera        () const;  /**< Returns true is the acquisition thread exists and it is connecting to a camera or getting ready to read from a file. */

    
    const CameraParametersPtr     CameraParams ()  const;  /**< Return pointer to current camera parameters of currently connected camera. */

    VectorUcharPtr  CameraHighPoints ();

    VectorUcharPtr  CameraHighPointsFromLastNSampleLines (kkint32 n)  const;

    VectorUcharPtr  LastFrameAverageScanLine()  const;  /**< Returns a vector representing the average value of each column in the last frame to have been read. */

    VectorUcharPtr  LastFrameHighValuesScanLine()  const;

    const LarcosCounterStatsPtr  GetFinaleStats () {return finaleStats;}

    kkint32 GetLastSessionId ();

    void  GetLogicalFrameStats (kkint32&  _logicalFramesOnQueue,
                                kkint32&  _logicalFrameProcessorsAvailable
                               );

    /**
     *@brief Will derive the next control number by adding one to the training sequence number of the previous control number recorded.
     *@details Control number is also known as ControlNum and Short-Description
     */
    KKStr  GetNextControlNumber ();

    void  GetStats (LarcosCounterStats&  stats);

    void  GetSessionParameters (SessionParameters&  _sessionParameters);

    void  GetOperatingParameters (OperatingParameters&  _operatingParameters);

    void  GetDefaultOperatingParameters (OperatingParameters&  _defaultOperatingParameters);

    void  AddSecondaryMsg (const KKStr&  msg);

    void  ControlNumValidityCheck (const KKStr&  controlNum,
                                   KKStr&        errMsg
                                  );

    StatusSnapshotListPtr  SnapshotsRetrieve (KKB::DateTime  rangeStart,
                                              KKB::DateTime  rangeEnd
                                             );

    const StatusSnapshotPtr  SnapshotLastEntry ();

    /**
     *@brief  Returns back a image of one of the last particles processed by one of the instances of 'LogicalFrameProcessor'.
     *@details  Caller will have ownership and will be responsible for deleting when done with instance.
     */
    RasterPtr  GetLastParticle ();

    void  SetLiveVideoDimensions (kkint32  liveVideoHeight,
                                  kkint32  liveVideoWidth
                                 );


    /**
     *@brief This will inform the 'CameraFrameBuffer' instance know that future camera frames are to be written to the
     * Scanner file indicated by '_scannerFileRootName'.
     *@details Future 'CameraFrame' instances added to 'CameraFrameBuffer' will have '_scannerFileRootName' attached to them.
     * This way the 'DiskWriterThread' will know which scanner file to add the frame to; and the 'LogicalFrameBuilder' thread 
     * will be able to pass on the Scanner File name and scan line to the LogicalFrame instances that it builds.  Ultimately
     * this will allow the 'LogicalFrameProcessor' threads know where extracted images come from.
     */
    void  StartNewScannerFile (ScannerFileEntryPtr  _scannerFileEntry);


    /**
     *@brief Call this method when playing back existing ScannerFiles; if we are not recording a new one will then inform 
     * the 'CameraFrameBuffer' instance.
     *@details Future 'CameraFrame' instances added to 'CameraFrameBuffer' will have '_scannerFileRootName' attached to them.
     * This way the 'DiskWriterThread' will know which scanner file to add the frame to; and the 'LogicalFrameBuilder' thread 
     * will be able to pass on the Scanner File name and scan line to the LogicalFrame instances that it builds.  Ultimately
     * this will allow the 'LogicalFrameProcessor' threads know where extracted images come from.
     */
    void  StartingToReadNewScannerFile (ScannerFileEntryPtr  _scannerFileEntry);


    void  Status (KKStr&  statusMsg,
                  KKStr&  color,
                  KKStr&  secondaryMsg,
                  float&  analogGain,
                  kkint32&  digitalGain,
                  kkint32&  cameraTemparature,
                  float&  flowRate
                 );

    /**  @brief  Returns a copy of the most current frame retrieved from the camera('CameraFrameBuffer').*/
    RasterPtr  SnapShotLatestFrame ();


    // Following methods are events that all other objects owned by this instance need to be informed about.

    /**@brief  Will inform all objects owned by this instance of the new crop settings. */
    void   CropSettingsChanged  (kkint32  _cropLeft,
                                 kkint32  _cropRight
                                );

    /**@brief  This method called by Acquisition thread whenever the Scan-Rate changes all other
     * objects owned by this manager will be informed of the change.
     */
    void   ScanRateChanged (float _newScanRate);



  private:

    /**
     *@brief Called by PlayBack and StartRecordingAndOrCounting to add all pertinent header fields to scanner files.
     */
    void  AddHeaderFields ();

    void  BuildDestScannerFileName (bool playingBackExistingFile);

    bool  LogicalFrameBuffersAreEmpty ();

    bool  CameraFrameBufferEmpty ();

    void  CleanUpMemory ();    /**< Called when u r done with this instance and u want to free its memory. */

    void  InitializeFlowMeterTracker ();

    void  ValidateLarcosInstallation ();


    /**
     *@brief  Will stop the counting and recording related threads.
     *@details  Will stop sending data from the camera acquisition thread to the recording and
     * counting related threads.  What ever imagery data that is already in the buffers will
     * be processed.  Upon return from this method the only thread that should still be running
     * will be the CameraAquistion thread.  If we were performing a PlayBack of a previously 
     * recorded scanner file the Acquisition thread will also be shutdown.
     *
     * This method will be called by the 'StopButtonThread' class.
     */
    void  CloseOutCountingAndOrRecording (VolConstBool&  terminateFlag,
                                          bool           stopImmediately
                                         );

    /**
     *@brief Set the minimum number of scan lines that need to be maintained in 'lastFrames' in 'cameraFrameBuffer'
     * for a given height and width.
     */
    void  ComputeLastFramesMinScanLines ();


    /**
     *@brief  Will connect to line-scan camera.
     *@details  Imagery will be available for display to the screen thru "SnapShotLatestFrame".
     */
    void  ConnectToCamera (bool&  _successful);


    /**
     *@brief Starts up all threads required for Recording and counting data.
     *@details called from a special 'RecordButtonThread' instance this way the process
     * of starting to record rows not hang the screen.
     */
    void  StartRecordingAndOrCounting (bool&   _successful,
                                       KKStr&  _errMsg
                                      );


    void  PlayBackScannerFile (const KKStr&  _srcScannerFileName,
                               bool&         _successful,
                               KKStr&        _errMsg
                              );


    void  ValidateCommonParameters (SessionParametersPtr   _sessionParameters,
                                    OperatingParametersPtr _operatingParameters,
                                    KKStr&                 _errMsg
                                   );


    void  ValidateTrainingModel (const KKStr&  trainingModelName,
                                 bool&         successful,
                                 KKStr&        errMsg
                                );

    static
    void  CreateGoalie ();

    static
    void  CleanUp ();

    void  DeleteAllThreads ();

    void  DeleteAllStoppedThreads ();

    void  DeleteOneThread (CameraThreadPtr &t);

    void  DeleteAcquisitionThread ();

    void  DeleteCameraAutoGainThread ();

    void  DeleteConnectButtonThread ();

    void  DeleteDiskWriterThread ();

    void  DeleteReportWriterThread ();

    void  DeleteSnapshotThread ();

    void  DeleteLogicalFrameBuilderThread ();

    void  DeleteLogicalFrameProcessorThreads ();

    void  DeletePlayBackButtonThread ();

    void  DeleteStopButtonThread ();

    void  DeleteStartButtonThread ();

    /**   Returns 'true' if pressing the 'Stop' button a valid action. */
    bool  OkToPressStop (KKStr&  errMsg);

    void  ShutdownOneThread (CameraThreadPtr  t);

    void  ShutDownCameraAutoGainThread ();

    void  ShutDownDiskWriterThread ();

    void  ShutDownSnapshotThread ();

    void  ShutDownReportWriterThread ();

    void  ShutDownLogicalFrameBuilderThread (bool stopImmediately);

    void  ShutDownLogicalFrameProcessorThreads (bool stopImmediately);

    void  StartCameraDiskWriterThread (bool&  _successful);

    void  StartReportWriterThread (bool&  _successful);

    void  StartSnapshotThread (bool&  _successful);

    void  StartFrameBuilderAndProcessingThreads (bool&  _successful);

    void  StartCameraAcquisitionThread (CameraAcquisitionPtr  acquisitionThread, 
                                        bool&                 _successful
                                       );

    void  TerminateAndDeleteAllButCameraAndStartButtonThreads ();

    void  TerminateAndDeleteAllButConnectButtonThread ();

    void  TerminateAndDeleteAllButPlayBackButtonThread ();

    void  WaitForAllButCameraThreadsToStop (kkint32  maxSecsToWait,
                                            bool&  allThreadsStopped
                                           );

    /**@brief returns true if the camera is connected and running and the CameraDiskWritting object is recoding to disk. */
    bool   WeAreRecordingToDisk       ();


    void  SaveConfiguration ();
    void  ReadConfiguration ();

    static  const KKStr  larcosOperatingModeStrs[];   /**<  Displayable strings of the different LarcosOperatingModes values. */

    KKStr                         configurationFileName;

    CounterState                  curState;
    LarcosOperatingModes          operatingMode;
    MsgQueue                      secondaryMsgs;           /**< Add messages that are to be displayed to the screen  on next call to 'Status' */
    KKStr                         dropFolderToLarcos;      /**< Root directory where files are deposited that need to be transmitted to Larcos.
                                                            * The concept is that this is going to be a drop-box or google-drive folder. 
                                                            */

    StatusSnapshot::FieldIdx      throughPutField;

    InstallationConfigPtr         installation;            /**< Represents the physical parameters of the installation that we are recording on or
                                                            * playing back from.   It will initially be created when 'LarcosCounterManager' is 
                                                            * created from "ModelA" configuration file;  When the Play-back function "PlayBackScannerFile"
                                                            * is ran for a Scanner-File that contains 'InstallationConfig' parameters in its header fields
                                                            * 'installation' will be re-created using those Header-Field parameters.
                                                            */
    kkint32                       cropLeft;
    kkint32                       cropRight;
    kkint32                       frameHeightMax;          /**<  The maximum number of rows that will be processed by the 'LogicalFrameProcessor'
                                                            * threads.  This parameter as a VERY VERY large impact on memory since each 
                                                            * 'LogicalFrame' and 'LogicalFrameProcessor.h' instances will allocate memory to 
                                                            * handle frames of these lengths.  When we switch over to 64 bit will be able to 
                                                            * increase dramatically.
                                                            */
    kkint32                       liveVideoHeight;
    kkint32                       liveVideoWidth;

    static
    GoalKeeperPtr                 goalie;                  /**< used to make sure that calls made to this object from different threads
                                                            * do not collide.  For example when the user presses the "Stop" button and the
                                                            * 'StopButtonPressed' method is called we do not want another method
                                                            * that can create, shutdown, cancel, or destroy a thread to be called.
                                                            */

    kkint16                       logicalFrameQueueSizeMax;

    // Thread MNanagement variables.
    kkint32                       maxNumOfThreads;
    CameraThreadListPtr           allThreads;
    CameraAcquisitionPtr          acquisitionThread;
    CameraAutoGainThreadPtr       cameraAutoGainThread;
    ConnectButtonThreadPtr        connectButtonThread;
    DiskWriterThreadPtr           diskWriterThread;
    LoggerThreadPtr               loggerThread;
    LogicalFrameBuilderPtr        logicalFrameBuilderThread;
    LogicalFrameProcessorListPtr  frameProcessors;
    PlayBackButtonThreadPtr       playBackButtonThread;
    ReportWriterThreadPtr         reportWriterThread;
    StatusSnapshotThreadPtr       snapshotThread;
    RecordButtonThreadPtr         startButtonThread;
    StopButtonThreadPtr           stopButtonThread;


    KKStr                         cameraMacAddress;

    KKLSC::ScannerHeaderFieldsPtr headerFields;            /**< Keep track of fields that need to be added to every ScannerFile. */


    StatusSnapshotBufferPtr       snapshotBuffer;          /**<  buffer of all snapshots taken during last run to be started. */
    kkint32                       snapshotInterval;        /**< Interval in seconds between grabbing StatusSnapshop's by 'snapshotThread'. */

    bool                          playingBackRealTime;     /**< When playing back simulate the ScanRate of the camera; aids in testing application. */
    bool                          sampleLastFrameBeforeFlatField;

    FlowMeterTrackerPtr           flowMeter;

    CameraFrameBufferPtr          cameraFrameBuffer;       /**< Buffer used to communicate from camera thread to the disk and Frame processing  *
                                                            *   threads.  It will be created and owned by the CameraAcquision thread.           *
                                                            */

    LogicalFrameListPtr           logicalFrames;
    LogicalFrameEntryListPtr      logicalFrameEntries;     /**< List of all logical frames created. */
    LogicalFrameEntryQueuePtr     droppedFrames;           /**< Subset of "logicalFrameEntries"; only logical frames that were dropped. */
    RasterBufferPtr               lastParticlesProcessed;  /**<  When 'CameraParticleProcessor' is done with a particle it will move it to this queue.
                                                            *    If there is not enough room for them on this queue it will then be deleted,
                                                            */

    ParticleEntryBufferPtr        particleEntryBuffer;     /**< Where entries that need to be written to the report are buffered. */

    LarcosCounterStatsPtr         finaleStats;             /**< Stat's at time of last PlayBack or recording session.  */

    kkint64                       totalBytesToRead;        /**< Comes from 'CameraAcquisitionSimulator' and represent the total *
                                                            * number of bytes that need to be processed during a play-back.
                                                            */

    kkint64                       totalBytesRead;          /**< Comes from 'CameraAcquisitionSimulator' and represent the number of bytes read so far. */

    TrainingProcess2Ptr           trainer;
    volatile bool                 trainerCancelFlag;
    KKStr                         trainerStatusMsg;
    Classifier2Ptr                classifier;

    kkint32                       memoryPreasure;
    KKStr                         destScannerFileName;
    KKStr                         destScannerFileDir;
    KKStr                         destScannerFileRootName;
    KKStr                         srcScannerFileName;
    ScannerFileFormat             srcScannerFileFormat;
    KKStr                         particlesDirName;
    SessionParametersPtr          sessionParameters;
    OperatingParametersPtr        operatingParameters;
    OperatingParametersPtr        defaultOperatingParameters;
    KKStr                         lastControlNumber;      /**< This will be the control number specified in the last RecordingSession. */

    KKStr                         lastScannerFileCounted; /**< Full name of the last Scanner file that was counted. */

    KKStr                         nameLastRecordedFile;   /**< Name of last Scanner File recorded from the camera. */

    bool                          stoppingIsInProgress;
    bool                          generateFinaleReport;   /**< Indicates that all counting processing is done and okay to generate final report. */

    kkint32                       cameraBuffSizeMax;
    kkint32                       cameraFrameHeight;
    kkint32                       cameraFrameWidth;

    MsgQueuePtr                   msgQueue;         /**< Messages that are to be logged are to be added to this MsgQueue; after they are written to Log file in 'LoggerThread' will be moved to 'loggedMsgs'. */
    MsgQueuePtr                   loggedMsgs;       /**< Messages that have been written to the log file in 'LoggerThread' are copied to this MsgQueue and are removed by owner of 'LarcosCounterManager' when needed for display. */

    RunLogPtr                     runLog;

    friend  class  StopButtonThread;
    friend  class  ConnectButtonThread;
    friend  class  RecordButtonThread;
    friend  class  PlayBackButtonThread;

  };  /* LarcosCounterManager */

  typedef  LarcosCounterManager*  LarcosCounterManagerPtr;

}  /* LarcosCounterUnManaged */

#endif
