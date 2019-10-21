/* 
 * Copyright (C) 2011-2013  Kurt Kramer
 * For conditions of distribution and use, see copyright notice in CounterUnManaged.txt
 */
#if  !defined(_PRECLASSDEFINITIONS_)
#define  _PRECLASSDEFINITIONS_



namespace CounterUnManaged 
{
  #if  !defined(_CAMERAFRAME_)
  class  CameraFrame;
  typedef  CameraFrame*  CameraFramePtr;
  #endif


  #if   !defined(_CAMERAPARAMETERS_)
    class  CameraParameters;
    typedef  CameraParameters*  CameraParametersPtr;
    class  CameraParametersList;
    typedef  CameraParametersList*  CameraParametersListPtr;
  #endif


  #if  !defined(_CAMERAFRAMEBUFFER_)
    class  CameraFrameBuffer;
    typedef  CameraFrameBuffer*  CameraFrameBufferPtr;
  #endif


  #if  !defined(_CAMERAACQUISITION_)
    class  CameraAcquisition;
    typedef  CameraAcquisition*  CameraAcquisitionPtr;
  #endif


  #if  !defined(_CAMERAAUTOGAINTHREAD_)
    class  CameraAutoGainThread;
    typedef  CameraAutoGainThread*  CameraAutoGainThreadPtr;
  #endif


  #if  !defined(_DISKWRITERTHREAD_)
    class  DiskWriterThread;
    typedef  DiskWriterThread*  DiskWriterThreadPtr;
  #endif


  #if  !defined(_CAMERAPATHREAD_)
    class CameraThread;
    typedef  CameraThread*  CameraThreadPtr;
    class  CameraThreadList;
    typedef  CameraThreadList*  CameraThreadListPtr;
  #endif


  #if  !defined(_CONNECTBUTTONTHREAD_)
    class  ConnectButtonThread;
    typedef  ConnectButtonThread*  ConnectButtonThreadPtr;
  #endif

  #if  !defined(_INSTALLATIONCONFIG_)
    class  InstallationConfig;
    typedef  InstallationConfig*  InstallationConfigPtr;
  #endif

  #if  !defined(_LOGGERTHREAD_)
    class  LoggerThread;
    typedef  LoggerThread*  LoggerThreadPtr;
  #endif


  #if  !defined(_LOGICALFRAMEENTRY_)
    class  LogicalFrameEntry;
    typedef  LogicalFrameEntry*  LogicalFrameEntryPtr;
    class  LogicalFrameEntryList;
    typedef  LogicalFrameEntryList*  LogicalFrameEntryListPtr;
    class  LogicalFrameEntryQueue;
    typedef  LogicalFrameEntryQueue*  LogicalFrameEntryQueuePtr;
  #endif


  #if  !defined(_LOGICALFRAMEBUILDER_)
    class LogicalFrameBuilder;
    typedef  LogicalFrameBuilder* LogicalFrameBuilderPtr;
  #endif

  
  #if  !defined(_LOGICALFRAME_)
    class  LogicalFrame;
    typedef  LogicalFrame*  LogicalFramePtr;
    class  LogicalFrameList;
    typedef  LogicalFrameList*  LogicalFrameListPtr;
  #endif


  #if  !defined(_LOGICALFRAMEPROCESSOR_)
    class  LogicalFrameProcessor;
    typedef  LogicalFrameProcessor*  LogicalFrameProcessorPtr;
    class  LogicalFrameProcessorList;
    typedef  LogicalFrameProcessorList*  LogicalFrameProcessorListPtr;
  #endif


  #if  !defined(_COUNTERSTATS_)
    class  CounterStats;
    typedef  CounterStats*  CounterStatsPtr;
  #endif

  #if  !defined(_OPERATINGPARAMETERS_)
    class  OperatingParameters;
    typedef  OperatingParameters*  OperatingParametersPtr;
  #endif
  

  #if  !defined(_PARTICLEENTRY_)
    class  ParticleEntry;
    typedef  ParticleEntry*  ParticleEntryPtr;

    class  ParticleEntryList;
    typedef  ParticleEntryList*  ParticleEntryListPtr;

    class  ParticleEntryBuffer;
    typedef  ParticleEntryBuffer*  ParticleEntryBufferPtr;
  #endif


  #if  !defined(_PLAYBACKBUTTONTHREAD_)
    class  PlayBackButtonThread;
    typedef  PlayBackButtonThread*  PlayBackButtonThreadPtr;
  #endif


  #if  !defined(_RECORDBUTTONTHREAD_)
    class  RecordButtonThread;
    typedef  RecordButtonThread*  RecordButtonThreadPtr;
  #endif


  #if  !defined(_REPORTWRITERTHREAD_)
    class ReportWriterThread;
    typedef  ReportWriterThread*  ReportWriterThreadPtr;
  #endif

  #if  !defined(_SESSIONPARAMETERS_)
    class  SessionParameters;
    typedef  SessionParameters*  SessionParametersPtr;
  #endif



  #if  !defined(_STOPBUTTONTHREAD_)
    class  StopButtonThread;
    typedef  StopButtonThread*  StopButtonThreadPtr;
  #endif

  #if  !defined(_SHRIMPLENGTHCOMPUTER_)
    class  ShrimpLengthComputer;
    typedef  ShrimpLengthComputer*  ShrimpLengthComputerPtr;
  #endif

  #if  !defined(_STATUSSNAPSHOT_)
    class  StatusSnapshot;
    typedef  StatusSnapshot*  StatusSnapshotPtr;
    class  StatusSnapshotList;
    typedef  StatusSnapshotList*  StatusSnapshotListPtr;
    class  StatusSnapshotBuffer;
    typedef  StatusSnapshotBuffer*  StatusSnapshotBufferPtr;
  #endif


  #if  !defined(_STATUSSNAPSHOTTHREAD_)
    class  StatusSnapshotThread;
    typedef  StatusSnapshotThread*  StatusSnapshotThreadPtr;
  #endif


}  /* CounterUnManaged */


#endif
