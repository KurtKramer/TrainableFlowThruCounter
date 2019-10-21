#pragma once

#include "OSservices.h"
#include "MsgQueue.h"
//#include  "UmiRunLog.h"

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Collections::Generic;
using namespace System::Diagnostics;
using namespace System::Drawing;


#include "UmiScannerHeaderFields.h"
#include "CounterStats.h"
#include "UmiRaster.h"
#include "StatusSnapshotManaged.h"

// Problem is in include below
#include "CounterManager.h"
using namespace  CounterUnManaged;

#include "UmiCounterOperatingModes.h"
#include "StatusSnapshotManaged.h"

#include "UmiCounterState.h"
#include "UmiInstallationConfig.h"
#include "UmiOperatingParameters.h"
#include "UmiScannerFile.h"
#include "UmiSessionParameters.h"



/**
 *@namespace CounterManaged
 *@brief Provides a managed interface to the 'CounterUnManaged' library.
 *@details  The primary class of concern is 'CounterManagerWrapper' which provides communication to the 
 * unmanaged class 'CounterManager'.  
 */

namespace CounterManaged
{

  /**
   *@brief  This is a dot-net wrapper class for the unmanaged class 'CounterManager'.  
   *@details A ".net" application such as "CounterApplication" would create a single instance of this class
   * and communicate to it thru the methods defined below. Ex: to start a recording session it would 
   * call the method 'RecordButtonPressed' and then proceed to call in a time event 'CurState', 'GetStats',
   * and 'GetAllLoggedMsgs' for display to the screen.
   */

  public  ref class CounterManagerWrapper
  {
  public:
    typedef  UmiScannerFile::Format  ScannerFileFormat;
    typedef  StatusSnapshotManaged::FieldIdx   StatusSnapshotDataFieldIdx;

    String^  CounterStateToStr (CounterState  state);


    CounterManagerWrapper ();

    !CounterManagerWrapper ();
    ~CounterManagerWrapper ();

    ///<summary>
    ///Call this method after the instance is created and before any other method is called.
    ///Performs setup tasks such as loading CounterManager configuration file; makes 
    ///initial attempt to connect to camera, etc...
    ///</summary>
    void  Initialize ();

    String^  CounterVersionNumber ();

    UmiCounterOperatingModes  OperatingMode ();

    UmiCounterState  CurState ();

    ///<summary>
    ///Returns a copy of the Current InstallationConufig that CounterCameraManager instance is operating under.
    ///</summary>
    UmiInstallationConfig^  Installation (UmiRunLog^  runLog);


    //  Message managing methods.

    /**@brief  A copy of the message 'msg' and append to 'msgQueue'; where 'LoggerThread' will write to log file. */
    void  AddMsg (String^ msg);

    List<String^>^  GetAllLoggedMsgs ();

    /**@brief  Creates and returns a UmiRunLog instance that will add log messages to 'msgQueue' which will be added to Log file by 'LoggerThread'.  */
    UmiRunLog^  GetUmiRunLogInstance ();



    static  CameraParametersManaged^  PromptForCamera (UmiMsgQueue^  msgQueue);

    static  CameraParametersManagedList^  GetCameraList ();


    void  AutoGainButtonPressed (bool%     _successful,
                                 String^%  _errMsg
                                );

    /**
     *@brief  Will connect to specified line scan camera.
     *@details  Imagery will be available for display to the screen thru "SnapShotLatestFrame".
     */
    void  ConnectButtonPressed ();


    void  PlayBackButtonPressed (String^                 _srcScannerFileName,
                                 UmiSessionParameters^   _sessionParameters,
                                 UmiOperatingParameters^ _operatingParameters,
                                 bool%                   _successful,
                                 String^%                _errMsg
                                );

    void  RecordButtonPressed (UmiSessionParameters^   _sessionParameters,
                               UmiOperatingParameters^ _operatingParameters,
                               bool%                   _successful,
                               String^%                _errMsg
                              );

    void  StopButtonPressed (bool      _stopImmediately,
                             bool%     _successful,
                             String^%  _errMsg
                            );

    void  SetOperatingParameters (UmiOperatingParameters^  _operatingParameters);

    void  SetDefaultOperatingParameters (UmiOperatingParameters^  _defaultOperatingParameters);

    void  SetTrainingModel (String^                  _trainingModelName,
                            bool%                    _successful,
                            UmiOperatingParameters^  _opParms
                           );


    //**********************************************************************************


    void  PlayingBackRealTime (bool _PlayingBackRealTime);

    void  ThroughPutField (StatusSnapshotDataFieldIdx _throughPutField);


    /** 
     *@brief Specify the parameters that the camera should be using.
     *@details  Will be used the next time the camera is connected or if already connected will attempt 
     * to switch to these parameters.
     */
    void  RequestedCameraParameters (float   requestedAnalogGain,   /**< Will be used as scan rate when we connect to a camera (create an instance of 'CameraAquisitio')  */
                                     kkint32 requestedDigitalGain,
                                     float   requestedScanRate      /**< Will be used as scan rate when we connect to a camera (create an instance of 'CameraAquisitio')  */
                                    );

    void  RequestedAnalogGain (float  requestedAnalogGain);   /**< Will be used as scan rate when we connect to a camera (create an instance of 'CameraAquisitio')  */

    void  RequestedDigitalGain (kkint32  requestedDigitalGain);

    void  RequestedScanRate   (float  requestedScanRate);

    void  RequestedSensitivityMode (String^  sensitivityMode);

    void  ResetCounts  ();

    void  ControlNumValidityCheck (String^   controlNum,
                                   String^%  errMsg
                                  );

    void  SampleLastFrameBeforeFlatField (bool _sampleLastFrameBeforeFlatField);

    /**
     *@brief Signal all threads to stop at earliest convenience.
     *@details  For example finish whatever is in their queues before shutting down. 
     */
    void  ShutDownAllThreads ();


    /** @brief Signal all threads to stop processing immediately.  */
    void  TerminateAllThreads ();



    /*********************************************************************/
    /*                          Access Methods                           */
    /*********************************************************************/
    void  Status (System::String^%  _statusMsg,
                  System::String^%  _color,
                  System::String^%  _secondaryMsg,
                  float%            _analogGain,
                  Int32%            _digitalGain,
                  Int32%            _cameraTemparature,
                  float%            _flowRate
                 );

    uchar                      BackGroundPixelTH              ();
    bool                       CameraAutoGainThreadRunning    ();
    String^                    CameraMacAddress               ();
    CameraParametersManaged^   CameraParams                   ();
    String^                    CameraSerialNum                ();
    bool                       CameraThreadRunning            ();
    int                        CropLeft                       ();
    int                        CropRight                      ();
    float                      CurAnalogGain                  ();  /**< Gain reported from camera. */
    int                        CurDigitalGain                 ();  /**< Gain reported from camera. */
    float                      CurScanRate                    ();
    bool                       DiskWritingThreadRunning       ();
    String^                    DropFolderToRemote             ();
    bool                       EmbeddedFlowMeter              ();
    int                        FrameProcessorsCount           ();
    int                        FrameWidthInPixels             ();
    String^                    GetTrainingModelName           ();
    bool                       GenerateFinaleReport           ();
    float                      ImagingChamberWidth            ();
    int                        ImagingChamberWidthPixels      ();
    float                      ImagingChamberPixelsPerMM      ();
    bool                       IsRecording                    (); /**< Indicates if Scanner Data is being recorded at this time. */
    bool                       IsRunning                      (); /**< 'curState' indicates that we are running;  ex "csDroppedFrames", "csRunning", "csPlayingBack" */
    String^                    LastScannerFileCounted         ();
    String^                    NameLastRecordedFile           ();
    int                        ParticlesCounted               ();
    bool                       PlayingBackRealTime            ();
    float                      RequestedAnalogGain            ();
    int                        RequestedDigitalGain           ();
    float                      RequestedScanRate              ();
    String^                    RequestedSensitivityMode       ();
    bool                       SampleLastFrameBeforeFlatField ();
    kkint32                    SnapshotInterval               ();
    StatusSnapshotDataFieldIdx ThroughPutField                ();

    kkint64                    TotalBytesRead                 ();
    kkint64                    TotalBytesToRead               ();

    bool                       SaveDebugImages                ()  {return saveDebugImages;}

    void   SaveDebugImages (bool _saveDebugImages);


    void  GetStatistics (float%  frameRate,               float%  scanRate,
                         Int64%  bytesWritten,            Int32%  waitingToWriteToDisk,
                         Int32%  physicalSeqNumsSkipped,  Int32%  diskWritingSeqNumBreaks,
                         Int32%  totalLostPackets,        Int32%  scanLinesWritten,
                         Int32%  physicalFramesRead,      Int32%  physicalFramesDropped,
                         Int32%  physicalFramesWaitingToProcess,  
                         Int32%  physicalFramesProcessed,
                         Int32%  particlesExtracted,      Int32%  particlesWaitingProcessing,
                         Int32%  particlesCounted
                        );

    String^  GetNextControlNumber ();

    CounterStatsManaged^  GetStats ();

    CounterStatsManaged^  GetFinaleStats ();

    kkint32  GetLastSessionId ();

    UmiSessionParameters^  GetSessionParameters ();

    UmiOperatingParameters^  GetOperatingParameters ();

    UmiOperatingParameters^  GetDefaultOperatingParameters ();

    bool  StoppingIsInProgress ();


    /** 
     *@brief Returns an array representing the highest pixel values for each column over 
     * all the sampling lines maintained by the FlatFieldCorrection routine.
     *@details It ultimately calls the "CameraFlatFieldCorrection::CameraHighPoints" method. 
     * The array returned will be the same length as the width of the camera.
     */
    array<byte>^  CameraHighPoints ();


    /**
     *@brief Returns an array representing the highest pixel values for each column over
     * a specified number of the most recent sampling lines maintaining by the FlatFieldCorrection
     * routine.
     *@param[in] n  Number of sample lines to search;  The most recent sampling lines are searched.
     */
    array<byte>^  CameraHighPointsFromLastNSampleLines (kkint32 n);


    Bitmap^  GetLastParticle (kkint32   targetHeight,
                              kkint32   targetWidth,
                              String^%  classLabel
                             );


    void  GetGainSettings (bool%   _autoGainRunning,
                           float%  _analogGain,
                           kkint32%  _digitalGain
                          );


    StatusSnapshotManagedList^  SnapshotsRetrieve (System::DateTime  rangeStart,
                                                   System::DateTime  rangeEnd
                                                  );

    StatusSnapshotManaged^  SnapshotLastEntry ();

    Bitmap^  SnapShotLatestFrame (kkint32  targetHeight,
                                  kkint32  targetWidth
                                 );

    /**
     *@brief   Returns cropped part of last frame retrieved from acquisition device.
     */
    UmiRaster^  SnapShotLatestFrame ();

    /**
     *@brief Set the minimum number of scan lines that need to be maintained in 'lastFrames' in 'cameraFrameBuffer'
     * for a given height and width.
     */
    void  SetLiveVideoDimensions (kkint32  _liveVideoHeight,
                                  kkint32  _liveVideoWidth
                                 );

    // Access methods for run time parameters,
    bool  FlatFieldEnabled   ();
    int   MinSizeThreshold   ();
    bool  DataIsToBeCounted  ();
    bool  DataIsToBeRecorded ();

    void  BackGroundPixelTH          (uchar _backGroundPixelTH);

    void  CropLeft                   (int   _cropLeft);
    void  CropRight                  (int   _cropRight);

    void  CropSettingsChanged        (int   _cropLeft,
                                      int   _cropRight
                                     );

    void  FlatFieldEnabled           (bool _flatFieldEnabled);
    void  MinSizeThreshold           (int  _minSizeThreshold);

    void  LastScannerFileCounted (String^  _lastScannerFileCounted);

    void  NameLastRecordedFile (String^  _nameLastRecordedFile);

    void  SnapshotInterval (kkint32 _snapshotInterval);


  private:
    Bitmap^  BuildBitmap (RasterPtr  raster,
                          kkint32    targetHeight,
                          kkint32    targetWidth
                         );

    Bitmap^  BuildBitmapFillTargetWidth (RasterPtr  raster,
                                         kkint32    targetHeight,
                                         kkint32    targetWidth
                                        );


    /**
     *@brief Return Gray-scale 8 bits-per-pixel image;  the raster data is assumed to have (background == 0).
     */
    Bitmap^  BuildBitmapFillTargetWidth2 (RasterPtr  raster,
                                          kkint32    targetHeight,
                                          kkint32    targetWidth,
                                          bool       complementData
                                         );

    void     CleanUpMemory ();  /**< Called when u r done with this instance and u want to free its memory. */

    /**
     *@brief updates the memory pressure that this instance is exerting; this method should be called
     * periodically.
     */
    void     UpdateUnManagedConsumedMemory ();

    KKStr    SystemStringToKKStr (System::String^  s);

    String^  KKStrToSystenStr (const KKStr&  s);

    bool*               cancelFlag;
    CounterManagerPtr   counterManager;
    CounterStatsPtr     counterStats;
    kkint64             lastReportedMemoryPreasure;
    MsgQueuePtr         loggedMsgs;
    MsgQueuePtr         msgQueue;
    bool                saveDebugImages;  /**< Indicates that we are to save images after features have been computed along with their respective feature vectors. */
  };  /* CounterManagerWrapper */
}  /* CounterManaged */
