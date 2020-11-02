/* CounterManager.h
 * Copyright (C) 2011-2013  Kurt Kramer
 * For conditions of distribution and use, see copyright notice in CounterUnManaged.txt
 */
#include "FirstIncludes.h"

#include <stdio.h>
#include <math.h>

#include <ctype.h>
#include <time.h>

#include <deque>
#include <fstream>
#include <iostream>
#include <istream>
#include <map>
#include <ostream>
#include <queue>
#include <string>
#include <vector>
#include "MemoryDebug.h"
using namespace std;

#include "OSservices.h"
#include "RasterBuffer.h"
#include "RunLog.h"
using namespace  KKB;

#include "FlowMeterTracker.h"
#include "ScannerFile.h"
#include "ScannerFileEntry.h"
using namespace  KKLSC;

#include "Classifier2.h"
#include "TrainingProcess2.h"
#include "CounterFeatureVector.h"
#include "CounterFVProducer.h"
using namespace KKMLL;

#include "CounterVariables.h"
using namespace  CounterBase;


#include "CounterManager.h"

#include "CameraFrameBuffer.h"
#include "CameraAcquisitionPleora.h"
#include "CameraAcquisitionSimulator.h"
#include "CameraAutoGainThread.h"
#include "CameraParameters.h"
#include "ConnectButtonThread.h"
#include "CounterTrainingConfiguration.h"
#include "DiskWriterThread.h"
#include "InstallationConfig.h"
#include "LoggerThread.h"
#include "LogicalFrameEntry.h"
#include "LogicalFrameBuilder.h"
#include "LogicalFrameProcessor.h"
#include "CounterStats.h"
#include "OperatingParameters.h"
#include "RecordButtonThread.h"
#include "ParticleEntry.h"
#include "PlayBackButtonThread.h"
#include "ReportWriterThread.h"
#include "SessionLogger.h"
#include "SessionParameters.h"
#include "StatusSnapshotThread.h"
#include "StatusSnapshot.h"
#include "StopButtonThread.h"
#include "SvnVersion.h"

#define Q(x) #x
#define QUOTE(x) Q(x)
#define _SVNVERSION_WITH_QUOTES_  QUOTE(_SVNVERSION_)


using  namespace  CounterUnManaged;


#define  MemoryWillingToAllocateToCameraFrameBuffers  1024 * 1024 * 256


GoalKeeperPtr  CounterManager::goalie = nullptr;

KKStr  CounterManager::CounterStateToStr (CounterState  state)
{
  switch  (state)
  {
  case  CounterState::Null:               return "nullptr";
  case  CounterState::Stopped:            return "Stopped";
  case  CounterState::Stopping:           return "Stopping";
  case  CounterState::Connected:          return "Connected";
  case  CounterState::Connecting:         return "Connecting";
  case  CounterState::Starting:           return "Starting";
  case  CounterState::BuildingClassifier: return "BuildingClassifier";
  case  CounterState::Running:            return "Running";
  case  CounterState::PlayingBack:        return "PlayingBack";
  case  CounterState::DroppedFrames:      return "DroppedFrames";
  }
  return  "nullptr";
}


const  KKStr  CounterManager::counterOperatingModeStrs[] = {"nullptr", "User", "Advanced", "Invalid"};


const KKStr&  CounterManager::CounterOperatingModeToStr (CounterOperatingModes om)
{
  if  (((int)om < 0)  ||  ((int)om >= (int)CounterOperatingModes::Invalid))
    return  counterOperatingModeStrs[(int)CounterOperatingModes::Invalid];
  else
    return  counterOperatingModeStrs[(int)om];
}



CounterManager::CounterOperatingModes  CounterManager::CounterOperatingModeFromStr (const KKStr&  s)
{
  for  (kkint32  x = 0;  x < (int)CounterOperatingModes::Invalid;  ++x)
  {
    if  (s.EqualIgnoreCase (counterOperatingModeStrs[x]))
      return  (CounterOperatingModes)x;
  }

  return  CounterOperatingModes::Null;
}



CounterManager::CounterManager (MsgQueuePtr  _msgQueue,
                                MsgQueuePtr  _loggedMsgs,
                                int          _maxNumOfThreads
                               ):
    configurationFileName          (),
    curState                       (CounterState::Stopped),
    operatingMode                  (CounterOperatingModes::User),
    throughPutField                (StatusSnapshot::FieldIdx::Count),
    secondaryMsgs                  ("SecondaryMsgs"),

    installation                   (nullptr),

    maxNumOfThreads                (_maxNumOfThreads),
    allThreads                     (nullptr),
    acquisitionThread              (nullptr),
    cameraAutoGainThread           (nullptr),
    connectButtonThread            (nullptr),
    diskWriterThread               (nullptr),
    frameProcessors                (nullptr),
    reportWriterThread             (nullptr),
    loggerThread                   (nullptr),
    logicalFrameBuilderThread      (nullptr),
    playBackButtonThread           (nullptr),
    startButtonThread              (nullptr),
    snapshotThread                 (nullptr),
    stopButtonThread               (nullptr),

    cameraMacAddress               (),

    headerFields                   (nullptr),

    cropLeft                       (0),
    cropRight                      (2047),

    frameHeightMax                 (14000),
    logicalFrameQueueSizeMax       (20),

    liveVideoHeight                (848),
    liveVideoWidth                 (400),

    playingBackRealTime            (true),
    sampleLastFrameBeforeFlatField (false),

    snapshotBuffer                 (nullptr),
    snapshotInterval               (5),

    flowMeter                      (nullptr),
    cameraFrameBuffer              (nullptr),
    classifier                     (nullptr),
    lastParticlesProcessed         (nullptr),
    logicalFrames                  (nullptr),
    logicalFrameEntries            (nullptr),
    droppedFrames                  (nullptr),
    particleEntryBuffer            (nullptr),
    finaleStats                    (nullptr),
    totalBytesToRead               (0),
    totalBytesRead                 (0),
    trainer                        (nullptr),
    trainerCancelFlag              (false),
    trainerStatusMsg               (),
    memoryPreasure                 (0),
    destScannerFileName            (),
    destScannerFileDir             (),
    destScannerFileRootName        (),
    lastControlNumber              (),
    srcScannerFileName             (),
    srcScannerFileFormat           (ScannerFile::Format::sfUnKnown),
    particlesDirName               (),
    sessionParameters              (nullptr),
    operatingParameters            (nullptr),
    defaultOperatingParameters     (nullptr),
    cameraBuffSizeMax              (256),
    cameraFrameHeight              (480),
    cameraFrameWidth               (2048),
    stoppingIsInProgress           (false),
    generateFinaleReport           (false),
    msgQueue                       (_msgQueue),
    loggedMsgs                     (_loggedMsgs),
    runLog                         (nullptr),
    saveDebugImages                (false)
{
  configurationFileName = osAddSlash (CounterVariables::ConfigurationDir ()) + "CounterManager.txt";

  loggerThread = LoggerThread::CreateAndStartInstance (msgQueue, loggedMsgs, "LoggerThread");
  runLog = new RunLog (msgQueue);
  runLog->SetLevel (CounterVariables::DebugLevel ());
  runLog->Level (40) << "CounterManager::CounterManager   MaxNumThreads: " << _maxNumOfThreads << endl;

  ValidateCounterInstallation ();

  allThreads                 = new CameraThreadList (true);
  frameProcessors            = new LogicalFrameProcessorList (false);
  sessionParameters          = new SessionParameters ();
  operatingParameters        = new OperatingParameters ();
  defaultOperatingParameters = new OperatingParameters ();
  headerFields               = new ScannerHeaderFields ();

  InitializeFlowMeterTracker ();

  CreateGoalie ();
}



CounterManager::~CounterManager ()
{
  SaveConfiguration ();
  runLog->Level (40) << "CounterManager::~CounterManager" << endl;
  CleanUpMemory ();
}



void  CounterManager::Initialize ()
{
  // 1) Get Camera mac Address is order of:
  //    a. Use last one used from "CounterManager" configuration file.
  //    b. Installation configuration.
  //    c. Look for attached camera.
  //
  // 2) If camera is present start connect procedure.
  //
  // 3) Set Active Training Model to last one used.

  ReadConfiguration ();
  if  (cameraMacAddress.Empty ())
  {
    cameraMacAddress = installation->CameraMacAddress ();
    if  (cameraMacAddress.Empty ())
    {
      CameraParametersListPtr  cameras = CounterManager::GetCameraList (msgQueue);
      if  ((cameras != nullptr)  &&  (cameras->QueueSize () == 1))
        cameraMacAddress = (*cameras)[0].MacAddress ();
      delete  cameras;
      cameras = nullptr;
    }
  }

  if  (!cameraMacAddress.Empty ())
    ConnectButtonPressed ();

  if  (!sessionParameters->TrainingModelName ().Empty ())
  {
    bool  successful = false;
    SetTrainingModel (sessionParameters->TrainingModelName (), successful, nullptr);
  }
}  /* Initialize  */



KKStr  CounterManager::CounterVersionNumber ()
{
/*  KKStr  dateStr = __DATE__;
  KKStr  timeStr = __TIME__;
  DateType  d (dateStr);
  TimeType  t (timeStr);

  DateType  majorVersionData (2014, 2, 6);

  kkint32  daysSinceMajorVersion = d.Days () - majorVersionData.Days ();
  kkint32  hourMinInDay = t.Hour () * 100 + t.Minute ();

  KKStr  verNum (20);
  verNum << "1.2." << daysSinceMajorVersion << "(p)";
  */

  KKStr  verNum = "1.2.";
  //verNum << _SVNVERSION_WITH_QUOTES_;

  return  verNum;
}



void  CounterManager::CleanUpMemory ()
{
  if  (allThreads)
  {
    bool  allThreadsStopped = false;
    allThreads->SendTerminateCmdToAllThreads ();
    allThreads->WaitForAllThreadsToStop (10.0f, allThreadsStopped);
  }

  // 'allThreads'  owns 'acquisitionThread', 'diskWriterThread', 'logicalFrameBuilderThread' and 'frameProcessors' contents.

  delete  allThreads;                  allThreads                 = nullptr;
                                       acquisitionThread          = nullptr;
                                       cameraAutoGainThread       = nullptr;
                                       connectButtonThread        = nullptr;
                                       diskWriterThread           = nullptr;
                                       logicalFrameBuilderThread  = nullptr;
                                       reportWriterThread         = nullptr;
                                       startButtonThread          = nullptr;
                                       playBackButtonThread       = nullptr;
                                       stopButtonThread           = nullptr;
                                       snapshotThread             = nullptr;

  delete  flowMeter;                   flowMeter                  = nullptr;
  delete  frameProcessors;             frameProcessors            = nullptr;
  delete  cameraFrameBuffer;           cameraFrameBuffer          = nullptr;
  delete  logicalFrames;               logicalFrames              = nullptr;
  delete  logicalFrameEntries;         logicalFrameEntries        = nullptr;
  delete  droppedFrames;               droppedFrames              = nullptr;
  delete  lastParticlesProcessed;      lastParticlesProcessed     = nullptr;
  delete  particleEntryBuffer;         particleEntryBuffer        = nullptr;
  delete  snapshotBuffer;              snapshotBuffer             = nullptr;
  delete  trainer;                     trainer                    = nullptr;
  delete  classifier;                  classifier                 = nullptr;
  delete  headerFields;                headerFields               = nullptr;
  delete  runLog;                      runLog                     = nullptr;
  delete  finaleStats;                 finaleStats                = nullptr;

  delete  sessionParameters;           sessionParameters          = nullptr;
  delete  operatingParameters;         operatingParameters        = nullptr;
  delete  defaultOperatingParameters;  defaultOperatingParameters = nullptr;

  delete  installation;                installation               = nullptr;

  // loggerThread is not derived from CameraThread and is not managed by 'allThreads' ;  we want to make this the last thread to be deleted.
  delete  loggerThread;                loggerThread               = nullptr; 
}  /* CleanUpMemory */



void  CounterManager::ValidateCounterInstallation ()
{
  if  (!osValidDirectory (CounterVariables::CounterHomeDir ()))
    osCreateDirectoryPath (CounterVariables::CounterHomeDir ());

  if  (!osValidDirectory (CounterVariables::ConfigurationDir ()))
    osCreateDirectoryPath (CounterVariables::ConfigurationDir ());

  bool  successful = false;

  delete  installation;
  
  installation = new InstallationConfig ("ModelA", successful, *runLog);
  if  (!successful)
    installation->Save (*runLog);
}  /* ValidateCounterInstallation */



float  CounterManager::ImagingChamberWidth ()  const
{
  if  (installation)
    return  installation->ImagingChamberWidth ();
  else
    return  0.0f;
}



kkint32  CounterManager::ImagingChamberWidthPixels ()  const
{
  if  (installation)
    return  installation->ImagingChamberWidthPixels ();
  else
    return  0;
}



float  CounterManager::ImagingChamberPixelsPerMM ()  const
{
  if  ((!installation)  ||  (installation->ImagingChamberWidth () == 0.0f))
    return 0.0f;
  else
    return  ((float)(installation->ImagingChamberWidthPixels ()) / installation->ImagingChamberWidth ()) / 1000.0f;
}



void  CounterManager::InitializeFlowMeterTracker ()
{
  if   (!flowMeter)
    flowMeter = new FlowMeterTracker ();

  flowMeter->Initialize (installation->FlowMeterPresent (),
                         operatingParameters->FlowRateFactor (),
                         20,     // History Table Size
                         10000,  // ScanRate
                         installation->ImagingChamberWidth (),
                         installation->ImagingChamberWidthPixels (),
                         installation->FlowMeterTicsPerMeter ()
                        );
}



kkMemSize  CounterManager::MemoryConsumedEstimated ()
{
  kkMemSize  memoryConsumed =  sizeof (CounterManager);

  CameraThreadList::const_iterator  idx;
  if  (allThreads)
  {
    for  (idx = allThreads->begin ();  idx != allThreads->end ();  ++idx)
      memoryConsumed += (*idx)->MemoryConsumedEstimated ();
  }

  // LoggerThread is not kept in 'allThreads' so needs to be accounted for separately.
  if  (loggerThread)             memoryConsumed += loggerThread->MemoryConsumedEstimated ();

  if  (installation)             memoryConsumed += installation->MemoryConsumedEstimated ();
  if  (logicalFrames)            memoryConsumed += logicalFrames->MemoryConsumedEstimated ();
  if  (logicalFrameEntries)      memoryConsumed += logicalFrameEntries->MemoryConsumedEstimated ();
  if  (droppedFrames)            memoryConsumed += droppedFrames->MemoryConsumedEstimated ();
  if  (lastParticlesProcessed)   memoryConsumed += lastParticlesProcessed->MemoryConsumedEstimated ();
  if  (particleEntryBuffer)      memoryConsumed += particleEntryBuffer->MemoryConsumedEstimated ();
  if  (trainer)                  memoryConsumed += trainer->MemoryConsumedEstimated ();
  if  (classifier)               memoryConsumed += classifier->MemoryConsumedEstimated ();
  if  (sessionParameters)        memoryConsumed += sessionParameters->MemoryConsumedEstimated ();
  if  (operatingParameters)      memoryConsumed += operatingParameters->MemoryConsumedEstimated ();

  memoryConsumed += cameraMacAddress.MemoryConsumedEstimated         ();
  memoryConsumed += destScannerFileName.MemoryConsumedEstimated      ();
  memoryConsumed += srcScannerFileName.MemoryConsumedEstimated       ();
  memoryConsumed += destScannerFileDir.MemoryConsumedEstimated       ();
  memoryConsumed += destScannerFileRootName.MemoryConsumedEstimated  ();
  memoryConsumed += particlesDirName.MemoryConsumedEstimated         ();

  return  memoryConsumed;
}  /* MemoryConsumedEstimated */



void  CounterManager::CreateGoalie ()
{
  bool didNotExist = false;
  GoalKeeper::CreateAndStartBlock ("CounterManager", goalie, didNotExist);
  if  (didNotExist)
     atexit (CounterManager::CleanUp);
  goalie->EndBlock ();
}



void  CounterManager::CleanUp ()
{
  GoalKeeper::Destroy (goalie);
  goalie = nullptr;
}



void  CounterManager::AddMsg (KKStrPtr  msg)
{
  if  (msgQueue == nullptr)
  {
    cerr << endl 
         << "CounterManager::AddMsg   ***ERROR***    msgQuue is not defined."  << endl
         << "                               Msg[" << *msg << "]." << endl
         << endl;
    delete  msg;
    msg = nullptr;
  }
  else
  {
    KKStrPtr  s = new KKStr (msg->Len () + 22);
    *s << osGetThreadId () << " - " << osGetLocalDateTime ().Time () << "->" << (*msg);
    msgQueue->AddMsg (s);
    delete  msg;
    msg = nullptr;
  }
}  /* AddMsg */



void  CounterManager::AddMsg (const KKStr&  msg)
{
  if  (msgQueue == nullptr)
  {
    cerr << endl 
         << "CounterManager::AddMsg   ***ERROR***    msgQuue is not defined."  << endl
         << "                               Msg[" << msg << "]." << endl
         << endl;
  }
  else
  {
    KKStr s (msg.Len () + 22);
    s <<  osGetThreadId () << " - " << osGetLocalDateTime ().Time () << "->" << msg;
    msgQueue->AddMsg (s);
  }
}  /* AddMsg */



KKStrListPtr  CounterManager::GetAllLoggedMsgs ()
{
  if  (loggedMsgs)
    return loggedMsgs->GetAllMsgs ();
  else
    return new KKStrList ();
}



void   CounterManager::ControlNumValidityCheck (const KKStr&  controlNum,
                                                      KKStr&        errMsg
                                                     )
{
  const char* validChars = "ABCDEFGHIJKLMNOPQRSTUVWXRZabcdefghijklmnopqrstuvwxyz0123456789_-";

  kkint32 len = controlNum.Len ();
  for  (int idx = 0;  idx < len;  ++idx)
  {
    char c = controlNum[idx];
    if  (KKStr::StrChr (validChars, c) == nullptr)
       errMsg << "\n" << "ControlNum Invalid Character[" << c << "]";
  }
}  /* ControlNumValidityCheck */



KKStr  CounterManager::GetNextControlNumber ()
{
  KKStr  nextControlNumber;

  if  (lastControlNumber.Empty ())
  {
    nextControlNumber = "SI_" + StrFormatInt (SessionLogger::GetLastSessionId () + 1, "ZZ000");
  }
  else
  {
    // Determine any trailing number of last Control-Number.
    int  x = lastControlNumber.Len () - 1;
    while  (x >= 0)
    {
      if  ((lastControlNumber[x] < '0')  ||  (lastControlNumber[x] > '9'))
        break;
      --x;
    }
    ++x;

    kkint32  lastSeqNum = lastControlNumber.SubStrPart (x).ToInt32 ();

    KKStr  firstPart = lastControlNumber.SubStrSeg (0, x);
    if  (firstPart.LastChar () == '_')
      firstPart.ChopLastChar ();

    nextControlNumber = firstPart + "_" + StrFormatInt ((lastSeqNum + 1), "ZZ000");
  }

  return nextControlNumber;
}



void  CounterManager::ValidateCommonParameters (SessionParametersPtr    _sessionParameters,
                                                OperatingParametersPtr  _operatingParameters,
                                                KKStr&                  _errMsg
                                               )
{
  _operatingParameters->ValidateParameters (_errMsg);

  if  (cropLeft > cropRight)
    _errMsg << "\n" << "CropLeft[" << cropLeft <<  "] greater than CropRight[" << cropRight << "].";

  if  (cropLeft < 0)
    _errMsg << "\n" << "CropLeft[" << cropLeft <<  "] can not be negative." << endl;

  if  (cropRight >= cameraFrameWidth)
    _errMsg << "\n" << "CropRight[" << cropRight << "] Can not be greater than FrameWidth[" << cameraFrameWidth << "]";
    

  if  (_operatingParameters->DataIsToBeRecorded ()  ||  _operatingParameters->SaveParticleImages ())
  {
    if  (_sessionParameters->ControlNum ().Empty ())
       _errMsg << "\n" << "Control-Num is empty";
    else
        ControlNumValidityCheck (_sessionParameters->ControlNum (), _errMsg);
  }
}  /* ValidateCommonParameters */



CameraParametersListPtr  CounterManager::GetCameraList (MsgQueuePtr  _msgQueue)
{
  return  CameraAcquisitionPleora::GetCameraList (_msgQueue);
}



CameraParametersPtr  CounterManager::PromptForCamera (RunLog&  runLog)
{
  runLog.Level (10) << "CounterManager::PromptForCamera" << endl;
  CreateGoalie ();
  goalie->StartBlock ();
  CameraParametersPtr  result = nullptr;

  KKStr  macAddress = CameraAcquisitionPleora::PromptForCameraMacAddress ();
  if  (macAddress.Empty ())
  {
    runLog.Level (10) << "CounterManager::PromptForCamera   No Camera Selected." << endl;
    result = nullptr;
  }
  else
  {
    result = CameraAcquisitionPleora::GetCameraParameters (macAddress, nullptr, runLog);
    runLog.Level (10) << "CounterManager::PromptForCamera   MacAddress :" << macAddress.QuotedStr () << " selected." << endl;
  }

  goalie->EndBlock ();

  return  result;
}  /* PromptForCamera */



bool  CounterManager::IsRecording () const
{
  bool  isRecording = false;
  goalie->StartBlock ();

  if  (diskWriterThread)
    isRecording = diskWriterThread->DiskStatus () == DiskWriterThread::DiskWritingStatus::Recording;
  
  goalie->EndBlock ();
  return  isRecording;
}



bool  CounterManager::IsRunning () const
{
  bool  isRunning = false;
  goalie->StartBlock ();

  isRunning = ((curState == CounterState::DroppedFrames)  ||
               (curState == CounterState::Running)        ||
               (curState == CounterState::PlayingBack)
              );

  goalie->EndBlock ();
  return  isRunning;
}



void  CounterManager::AutoGainButtonPressed (bool&   _successful,
                                             KKStr&  _errMsg
                                            )
{
  runLog->Level (40) << "CounterManager::AutoGainButtonPressed" << endl;
  goalie->StartBlock ();

  if  (!CameraThreadRunning ())
  {
    _successful = false;
    _errMsg = "***  Camera thread has not been started  ***";
    runLog->Level (-1) << endl << "AutoGainButtonPressed   ***ERROR***   " << _errMsg << endl << endl;
    goalie->EndBlock ();
    return;
  }

  if  (CameraAutoGainThreadRunning ())
  {
    _successful = false;
    _errMsg = "***  Auto-Gain is already running  ***";
    runLog->Level (-1) << endl << "AutoGainButtonPressed   ***ERROR***   " << _errMsg << endl << endl;
    goalie->EndBlock ();
    return;
  }

  DeleteCameraAutoGainThread ();
  cameraAutoGainThread 
    = new CameraAutoGainThread (this, msgQueue, acquisitionThread, cameraFrameBuffer, "CameraAutoGainThread");
  allThreads->PushOnBack (cameraAutoGainThread);

  runLog->Level (40) << "AutoGainButtonPressed   Starting 'CameraAutoGainThread' Thread." << endl;
  cameraAutoGainThread->Start (ThreadPriority::Normal, _successful);

  goalie->EndBlock ();
}



void  CounterManager::ConnectButtonPressed ()
{
  runLog->Level (40) << "CounterManager::ConnectButtonPressed" << endl;

  goalie->StartBlock ();

  if  (curState == CounterManager::CounterState::Stopped)
  {
    DeleteConnectButtonThread ();
    connectButtonThread = new ConnectButtonThread (this, msgQueue,  "ConnectButtonThread");
    allThreads->PushOnBack (connectButtonThread);
    bool  successful = false;
    runLog->Level (40) << "ConnectButtonPressed   Starting 'ConnectButtonThread' Thread." << endl;
    connectButtonThread->Start (ThreadPriority::Normal, successful);
  }
  goalie->EndBlock ();
}



void  CounterManager::ConnectToCamera (bool&  _successful)
{
  _successful = false;

  runLog->Level (10) << "CounterManager::ConnectToCamera   CameraMacAddress: " << cameraMacAddress << endl;

  if  (curState != CounterState::Stopped)
  {
    _successful = false;
    runLog->Level (10) << "CounterManager::ConnectToCamera   We need to be in the Stooped-State before we can connect to camera." << endl;
    return;
  }

  goalie->StartBlock ();
  curState = CounterState::Connecting;
  
  TerminateAndDeleteAllButConnectButtonThread ();

  delete  cameraFrameBuffer;
  cameraFrameBuffer = nullptr;

  goalie->EndBlock ();

  KKStr  macAddress = "";

  CameraParametersPtr  cameraParameters = nullptr;

  if  (cameraMacAddress.Empty ())
  {
    runLog->Level (10) << "ConnectToCamera   Prompting For Camera" << endl;
    secondaryMsgs.AddMsg ("Prompting For Camera");
    cameraMacAddress = CameraAcquisitionPleora::PromptForCameraMacAddress ();
    if  (cameraMacAddress.Empty ())
    {
      runLog->Level (10) << "ConnectToCamera   No camera selected." << endl;
      curState = CounterState::Stopped;
      _successful = false;
      //goalie->EndBlock ();
      return;
    }
  }

  runLog->Level (20) << "ConnectToCamera   Retrieving camera parameters from: " << cameraMacAddress << endl;
  secondaryMsgs.AddMsg ("Retrieving Camera Parameters.");
  
  delete  cameraParameters;
  cameraParameters = nullptr;
  cameraParameters = CameraAcquisitionPleora::GetDeviceInfo (cameraMacAddress, 'M', *runLog);
  if  (cameraParameters)
  {
    macAddress = cameraParameters->MacAddress ();
    cameraFrameHeight = cameraParameters->FrameHeight ();
    cameraFrameWidth  = cameraParameters->FrameWidth ();
  }
  else
  {
    runLog->Level (10) << "ConnectToCamera   Camera not available." << endl;
    secondaryMsgs.AddMsg ("Camera not Connected!!!");
    curState = CounterState::Stopped;
    _successful = false;
    return;
  }

  {
    kkint32  bytesPerFrame = cameraFrameHeight * cameraFrameWidth;
    int  maxNumOfBuffs = MemoryWillingToAllocateToCameraFrameBuffers / bytesPerFrame;
    int  initailNumOfBuffs = maxNumOfBuffs / 2;

    cameraFrameBuffer = new CameraFrameBuffer (this, 
                                               "FrameBuffer",
                                               initailNumOfBuffs,    // 128, 
                                               maxNumOfBuffs,        // cameraBuffSizeMax, 
                                               cameraFrameHeight, 
                                               cameraFrameWidth,
                                               false,                // _dataIsToBeCounted
                                               false,                // _dataIsToBeRecorded
                                               sampleLastFrameBeforeFlatField
                                              );

    cameraFrameBuffer->FlatFieldEnabled (operatingParameters->FlatFieldEnabled ());

    ComputeLastFramesMinScanLines ();
  }

  acquisitionThread = CameraAcquisitionPleora::CreateFromCameraParameters
                  (this, 
                   cameraParameters, 
                   cameraFrameBuffer,
                   flowMeter,
                   msgQueue,
                   "CameraAcquisitionPleora"
                  );

  allThreads->PushOnBack (acquisitionThread);

  runLog->Level (10) << "ConnectToCamera   Starting Acquisition Thread" << endl;
  StartCameraAcquisitionThread (acquisitionThread, _successful);
  if  (_successful)
  {
    runLog->Level (40) << "ConnectToCamera   Acquisition Thread started;  now will set:" << endl
                       << "     Sensitivity : " << RequestedSensitivityMode () << endl
                       << "     analogGain  : " << RequestedAnalogGain      () << endl
                       << "     digitalGain : " << RequestedDigitalGain     () << endl
                       << "     ScanRate    : " << RequestedScanRate        () << endl
                       << endl;

    osSleepMiliSecs (100);
    acquisitionThread->RequestedCameraParameters (operatingParameters);
    curState = CounterState::Connected;
  }
  else
  {
    curState = CounterState::Stopped;
    runLog->Level (-1) << "ConnectToCamera   Could not connect to camera." << endl;
  }

  SaveConfiguration ();

  //goalie->EndBlock ();
  return;
}  /* ConnectToCamera*/



void  CounterManager::BuildDestScannerFileName (bool playingBackExistingFile)
{
  KKStr  controlNum = sessionParameters->ControlNum ();

  DateTime  dt = osGetLocalDateTime ();
  KKStr  dateStr = StrFormatInt (dt.Year (), "0000") + "-" + StrFormatInt (dt.Month (), "00") + "-" + StrFormatInt (dt.Day (), "00");

  if  (playingBackExistingFile)
  {
    KKStr  srcFileRootName = osGetRootName (srcScannerFileName);
    destScannerFileDir = osGetPathPartOfFile (srcScannerFileName);
    KKStr  baseRootName = srcFileRootName;
    if  (!baseRootName.Find (controlNum, 0))
      baseRootName << "_" << controlNum;

    kkint32  seqNum = 0;
    
    while  (true)
    {
      destScannerFileRootName = baseRootName + "_"+ StrFormatInt (seqNum, "00");
      destScannerFileName = osAddSlash (destScannerFileDir) + destScannerFileRootName + ".lsc";
      if  (!osFileExists (destScannerFileName))
        break;
      ++seqNum;
    }

    if  (operatingParameters->DataIsToBeRecorded ())
    {
      particlesDirName = osAddSlash (destScannerFileDir) + destScannerFileRootName;
    }
    else
    {
      seqNum = 0;
      while  (true)
      {
        particlesDirName = osAddSlash (destScannerFileDir) +  baseRootName + "_" + StrFormatInt (seqNum, "00");
        if  ((!osFileExists (particlesDirName))  &&  (!osValidDirectory (particlesDirName)))
          break;
        ++seqNum;
      }
    }
  }
  else
  {
    // Recording new scanner file.
    destScannerFileDir      = osAddSlash (CounterVariables::ScannerFilesDefaultDir ()) + dateStr + "_" + controlNum;
    destScannerFileRootName = controlNum + "_" + dt.Date ().YYYYMMDD () + "-" + dt.Time ().HHMMSS ();
    destScannerFileName     = osAddSlash (destScannerFileDir) + destScannerFileRootName + ".lsc";
    particlesDirName        = osAddSlash (destScannerFileDir) + destScannerFileRootName;
  }

  if  (operatingParameters->DataIsToBeRecorded ())
    osCreateDirectoryPath (destScannerFileDir);

  if  (operatingParameters->SaveParticleImages ())
    osCreateDirectoryPath (particlesDirName);
}  /* BuildDestScannerFileName */



void  CounterManager::AddHeaderFields ()
{
  if  (operatingParameters->DataIsToBeRecorded ())
  {
    sessionParameters->DestScannerFileName (destScannerFileName);
    AddHeaderField ("DestScannerFileDir",       destScannerFileDir);
    AddHeaderField ("DestScannerFileRootName",  destScannerFileRootName);
  }

  sessionParameters->DateTimeRecorded (osGetLocalDateTime ());

  auto hostName = osGetHostName ();
  sessionParameters->HostName (hostName.value_or ("Failed to retrieve HostName"));

  sessionParameters->AddToHeaderFields (headerFields);
  operatingParameters->AddToHeaderFields (headerFields);

  AddHeaderField ("ParticlesDirName", particlesDirName);
  AddHeaderField ("CropLeft",         cropLeft);
  AddHeaderField ("CropRight",        cropRight);

  if  (installation)
    installation->AddToHeaderFields (HeaderFields ());
}  /* AddHeaderFields */



void  CounterManager::ValidateTrainingModel (const KKStr&  trainingModelName,
                                             bool&         successful,
                                             KKStr&        errMsg
                                            )

{
  runLog->Level (10) << "CounterManager::ValidateTrainingModel  Model: " + trainingModelName << endl;

  secondaryMsgs.AddMsg ("Validating Training Model: " + trainingModelName);

  if  ((trainingModelName.EqualIgnoreCase ("PostLarvae"))  ||  (trainingModelName.EqualIgnoreCase ("Post Larvae")))
  {
    successful = true;
  }

  else if  (trainingModelName.EqualIgnoreCase ("None")  ||  trainingModelName.Empty ())
  {
    successful = true;
  }

  else
  {
    CounterTrainingConfigurationPtr config = new CounterTrainingConfiguration ();
    config->Load (trainingModelName,
                  operatingParameters,
                  true,  // true = Validate Directories.
                  *runLog
                 );
                                                                             
    if  (!config->FormatGood ())
    {
      runLog->Level (-1) << endl << endl
                         << "ValidateTrainingModel   ***ERROR***   Training Model: " << trainingModelName.QuotedStr () << " Configuration file is invalid." << endl
                         << config->FormatErrorsWithLineNumbers ()
                         << endl;
      successful = false;
      secondaryMsgs.AddMsg ("Error Building Classifier");
    }
    else
    {
      CounterState  origState = curState;
      curState = CounterState::BuildingClassifier;

      if  (operatingMode == CounterOperatingModes::Advanced)
        secondaryMsgs.AddMsg ("Validating Training Model");
      trainerCancelFlag = false;

      /**@todo Add  a Cancel Option when 1st starting a record session if the training model has to
       * be rebuilt. At this point the user has requested to start recording/ counting there will be
       * no option to cancel.  If the training library is out of date and a new one needs to be trained; 
       * it can take a while;  we should consider adding the logic to allow a cancel at this point.
       */
      trainer = TrainingProcess2::CreateTrainingProcess (config,
                                                         true,   //  CheckForDuplicates
                                                         TrainingProcess2::WhenToRebuild::NotUpToDate,
                                                         true,   // true = Save Trained Model if needed to be rebuilt.
                                                         trainerCancelFlag,
                                                         *runLog
                                                        );
      if  (trainer->Abort ())
      {
        runLog->Level (-1) << endl << endl
                           << "ValidateTrainingModel   ***ERROR***   Training Model: " << trainingModelName.QuotedStr () << " is invalid." << endl
                           << "             trainerStatusMsg: " << trainerStatusMsg << endl   
                           << endl;
        successful = false;
        errMsg = trainerStatusMsg;
        delete  trainer;
        trainer = nullptr;
        secondaryMsgs.AddMsg ("Error Building Classifier");
      }
      else
      {
        successful = true;
        runLog->Level (40) << "ValidateTrainingModel   Valid Format." << endl;
      }

      delete  trainer;
      trainer = nullptr;
      curState = origState;
    }
    delete  config;
    config = nullptr;
  }
}  /* ValidateTrainingModel */



void  CounterManager::StartRecordingAndOrCounting (bool&   _successful,
                                                         KKStr&  _errMsg
                                                        )
{
  goalie->StartBlock ();

  if  (CounterVariables::DebugLevel () >= 40)
    runLog->Level (40) << "CounterManager::StartRecordingAndOrCounting "      << endl
                       << "  trainingModelName      : " << TrainingModelName ()     << endl
                       << "  controlNum             : " << ControlNum ()            << endl
                       << "  description            : " << Description ()           << endl;
  operatingParameters->WriteToRunLog ("  OP:", *runLog);

  _successful = true;
  CounterState  origState = curState;

  operatingParameters->PlayingBack (false);

  if  (curState != CounterState::Connected)
  {
    _errMsg = "We are not connected to the camera!!!";
    runLog->Level (-1) << "StartRecordingAndOrCounting   ***ERROR***   " << _errMsg << endl;
    _successful = false;
    goalie->EndBlock ();
    return;
  }

  if  (!CameraThreadRunning ())
  {
    _successful = false;
    _errMsg = "Camera thread has not been started.";
    runLog->Level (-1) << "StartRecordingAndOrCounting   ***ERROR***   " << _errMsg << endl;
    goalie->EndBlock ();
    return;
  }

  curState = CounterState::Starting;
  SaveConfiguration ();

  DeleteAllStoppedThreads ();

  TerminateAndDeleteAllButCameraAndStartButtonThreads ();

  ResetCounts ();

  delete  classifier;           classifier          = nullptr;
  delete  trainer;              trainer             = nullptr;
  delete  logicalFrames;        logicalFrames       = nullptr;
  delete  logicalFrameEntries;  logicalFrameEntries = nullptr;
  delete  droppedFrames;        droppedFrames       = nullptr;
  delete  particleEntryBuffer;  particleEntryBuffer = nullptr;
  goalie->EndBlock ();

  BuildDestScannerFileName (false);  // false = We are NOT playing back an existing Scanner-File.

  cameraFrameBuffer->DataIsToBeCountedAndOrRecorded (operatingParameters->DataIsToBeCounted (), 
                                                     operatingParameters->DataIsToBeRecorded ()
                                                    );

  cameraFrameBuffer->FlatFieldEnabled (operatingParameters->FlatFieldEnabled ());

  // We need to let the 'cameraFrameBuffer' know what the 1st Scanner file is going to be.

  ScannerFileEntryPtr  sfe = ScannerFileEntry::GetOrCreateScannerFileEntry (destScannerFileRootName);
  sfe->ScanRate (CurScanRate ());
  sfe->FullName (destScannerFileName);
  sfe->PixelsPerScanLine (acquisitionThread->FrameWidth ());
  cameraFrameBuffer->StartNewScannerFile (sfe);

  HeaderFieldsClear ();
  AddHeaderFields ();
  acquisitionThread->CameraParametersAddToHeaderFields ();
  
  particleEntryBuffer = new ParticleEntryBuffer ();

  StartReportWriterThread (_successful);
  if  (!_successful)
  {
    _errMsg = "Error starting 'ReportWriter' thread.";
    runLog->Level (-1) << "StartRecordingAndOrCounting   ***ERROR***  " << _errMsg << endl;
    curState = origState;
    //goalie->EndBlock ();
    return;
  }

  StartSnapshotThread (_successful);
  if  (!_successful)
  {
    _errMsg = "Error starting 'StatusSnapshot' thread.";
    runLog->Level (-1) << "StartRecordingAndOrCounting   ***ERROR***  " << _errMsg << endl;
    curState = origState;
    //goalie->EndBlock ();
    return;
  }

  if  (operatingParameters->DataIsToBeRecorded ())
  {
    StartCameraDiskWriterThread (_successful);
    if  (!_successful)
    {
      _errMsg = "Could not start the DiskWriter thread.";
      runLog->Level (-1) << "StartRecordingAndOrCounting   ***ERROR***  " << _errMsg << endl;
      TerminateAndDeleteAllButCameraAndStartButtonThreads ();
      //goalie->EndBlock ();
      return;
    }
  }

  if  (operatingParameters->DataIsToBeCounted ())
  {
    ValidateTrainingModel (sessionParameters->TrainingModelName (), _successful, _errMsg);

    if  (!_successful)
    {
      curState = origState;
      //goalie->EndBlock ();
      return;
    }
    else
    {
      StartFrameBuilderAndProcessingThreads (_successful);
      if  (!_successful)
      {
        _errMsg = "Could not start the FrameBuilder and FrameProcessing threads.";
        runLog->Level (-1) << "StartRecordingAndOrCounting   ***ERROR***  " << _errMsg << endl;
        TerminateAndDeleteAllButCameraAndStartButtonThreads ();
        //goalie->EndBlock ();
        return;
      }
    }
  }

  if  (_successful)
  {
    if  (operatingParameters->DataIsToBeRecorded ())
      lastControlNumber = sessionParameters->ControlNum ();
    nameLastRecordedFile = destScannerFileName;
    curState = CounterState::Running;
    secondaryMsgs.AddMsg ("Counter Started Successfully.");
  }
  else
  {
    runLog->Level (-1) << "StartRecordingAndOrCounting   ***ERROR***  Failed to Start!!!" << endl;
    secondaryMsgs.AddMsg ("Failed to Start!!!");

    TerminateAndDeleteAllButCameraAndStartButtonThreads ();
    if  (WeAreConnectedToCamera ())
      curState = CounterState::Connected;

    else if  (WeAreConnectingToCamera ())
      curState = CounterState::Connecting;

    else
      curState = CounterState::Stopped;
  }

  SaveConfiguration ();

  //goalie->EndBlock ();
}  /* StartRecordingAndOrCounting */



void  CounterManager::RecordButtonPressed 
                      (SessionParametersPtr     _sessionParameters,
                       OperatingParametersPtr   _operatingParameters,
                       bool&                    _successful,
                       KKStr&                   _errMsg
                      )
{
  goalie->StartBlock ();
  
  _operatingParameters->PlayingBack (false);

  if  (CounterVariables::DebugLevel () >= 40)
  {
    runLog->Level (40) << "CounterManager::RecordButtonPressed  "      << endl
                       << "  trainingModelName      : " << TrainingModelName ()     << endl
                       << "  controlNum             : " << ControlNum ()            << endl
                       << "  description            : " << Description ()           << endl;

    _operatingParameters->WriteToRunLog ("  OP:", *runLog);
  }

  _successful = true;

  _errMsg = "";

  if  (curState != CounterState::Connected)
  {
    _errMsg = "We are not connected to the camera!!!";
    runLog->Level (-1) << "RecordButtonPressed   ***ERROR***   " << _errMsg << endl;
    _successful = false;
    goalie->EndBlock ();
    return;
  }

  if  (!CameraThreadRunning ())
  {
    _successful = false;
    _errMsg = "Camera thread has not been started!!!";
    runLog->Level (-1) << "RecordButtonPressed   ***ERROR***   " << _errMsg << endl;
    goalie->EndBlock ();
    return;
  }

  ValidateCommonParameters (_sessionParameters,
                            _operatingParameters,
                            _errMsg
                           );

  if  (!_errMsg.Empty ())
     _successful = false;

  if  (_sessionParameters)
    sessionParameters->Assign (*_sessionParameters);

  if  (_operatingParameters)
  {
    operatingParameters->Assign (*_operatingParameters);
    flowMeter->FlowRateRatioDefaultChanged (operatingParameters->FlowRateFactor ());
  }

  if  (startButtonThread  &&  _successful)
    DeleteStartButtonThread ();

  SaveConfiguration ();

  if  (_successful)
  {
    runLog->Level (40) << "RecordButtonPressed   Creating and starting StartButtonThread. " << endl;

    startButtonThread = new RecordButtonThread (this,
                                                msgQueue, 
                                                "RecordButtonThread"
                                               );
    allThreads->PushOnBack (startButtonThread);

    startButtonThread->Start (ThreadPriority::Normal, _successful);
  }

  goalie->EndBlock ();
}  /* RecordButtonPressed */



void  CounterManager::PlayBackScannerFile (const KKStr&  _srcScannerFileName,
                                           bool&         _successful,
                                           KKStr&        _errMsg
                                          )
{
  goalie->StartBlock ();

  operatingParameters->PlayingBack (true);

  if  (CounterVariables::DebugLevel () >= 40)
  {
    runLog->Level (40) << "CounterManager::PlayBackScannerFile  "      << endl
                       << "  srcScannerFileName     : " << _srcScannerFileName      << endl
                       << "  trainingModelName      : " <<  TrainingModelName ()    << endl
                       << "  controlNum             : " << ControlNum ()            << endl
                       << "  description            : " << Description ()           << endl;
    operatingParameters->WriteToRunLog ("  OP:", *runLog);
  }

  _successful = true;

  CounterState  origState = curState;

  DeleteAllStoppedThreads ();

  if  ((curState == CounterState::Stopped)  ||  (curState == CounterState::Connected))
  {
    runLog->Level (10) << "PlayBackScannerFile  File: " << _srcScannerFileName << endl;
  }
  else
  {
    _successful = false;
    _errMsg = "We must be Stopped before you can Play Back a previous recording.";
    runLog->Level (-1) << "PlayBackScannerFile   ***ERROR***  " << _errMsg << endl
                       << "    Current Status: " + CounterStateToStr (curState) << endl
                       << endl;
    goalie->EndBlock ();
    return;
  }

  curState = CounterState::Starting;

  TerminateAndDeleteAllButPlayBackButtonThread ();

  ResetCounts ();

  srcScannerFileName     = _srcScannerFileName;
  srcScannerFileFormat   = ScannerFile::Format::sfUnKnown;

  BuildDestScannerFileName (true);

  if  (operatingParameters->DataIsToBeRecorded ())
  {
    if  (operatingParameters->DestScannerFileFormat () == ScannerFile::Format::sfUnKnown)
    {
      _errMsg = "Invalid destination File-Format selected.";
      secondaryMsgs.AddMsg (_errMsg);
      runLog->Level (-1) << "PlayBackScannerFile   ***ERROR***  " << _errMsg << endl;
      _successful = false;
      curState = origState;
      goalie->EndBlock ();
      return;
    }

    if  (destScannerFileName.Empty ())
    {
      _errMsg = "No Destination file specified.";
      runLog->Level (-1) << "PlayBackScannerFile   ***ERROR***  " << _errMsg << endl;
      _successful = false;
      curState = origState;
      goalie->EndBlock ();
      return;
    }
  }

  HeaderFieldsClear ();
  AddHeaderField ("SrcScannerFileName", srcScannerFileName);
  AddHeaderFields ();

  delete trainer;             trainer             = nullptr;
  delete classifier;          classifier          = nullptr;
  delete cameraFrameBuffer;   cameraFrameBuffer   = nullptr;
  delete logicalFrames;       logicalFrames       = nullptr;
  delete logicalFrameEntries; logicalFrameEntries = nullptr;
  delete droppedFrames;       droppedFrames       = nullptr;
  delete particleEntryBuffer; particleEntryBuffer = nullptr;

  float scanRate = 0.0f;

  ScannerHeaderFieldsPtr scannerFileHeaderFields = nullptr;

  ScannerFile::GetScannerFileParameters (srcScannerFileName,
                                         scannerFileHeaderFields,
                                         srcScannerFileFormat,
                                         cameraFrameHeight,
                                         cameraFrameWidth,
                                         scanRate,
                                         _successful,
                                         *runLog
                                        );
  if  (!_successful)
  {
    _errMsg = "Can not retrieve parameters from Scanner-File: " + srcScannerFileName;
    runLog->Level (-1) << "PlayBackScannerFile   ***ERROR***  " << _errMsg << endl;
    _successful = false;
    curState = origState;
    delete  scannerFileHeaderFields;
    scannerFileHeaderFields = nullptr;
    goalie->EndBlock ();
    return;
  }

  //if  ((scannerFileHeaderFields != nullptr)  &&  scannerFileHeaderFields->FieldExists ("Installation:Name"))
  //{
  //  // Since the specified Scanner-File has 'Installation' parameters defined we will build a new instance 
  //  // of 'installation' from these header fields.
  //  delete  installation;
  //  installation = new InstallationConfig (scannerFileHeaderFields, *runLog);
  //  delete  scannerFileHeaderFields;
  //  scannerFileHeaderFields = nullptr;
  //}

  delete  scannerFileHeaderFields;
  scannerFileHeaderFields = nullptr;

  if  ((cameraFrameWidth < 1)  ||  (cameraFrameWidth > 10000))
  {
    _errMsg = "Frame-Width: " + StrFromInt32 (cameraFrameWidth) + " from Scanner file is not reasonable.";
    runLog->Level (-1) << "PlayBackScannerFile   ***ERROR***  " << _errMsg << endl;
    _successful = false;
    curState = origState;
    goalie->EndBlock ();
    return;
  }

  {
    kkint32  bytesPerFrame = cameraFrameHeight * cameraFrameWidth;
    int  maxNumOfBuffs = MemoryWillingToAllocateToCameraFrameBuffers / bytesPerFrame;
    int  initailNumOfBuffs = maxNumOfBuffs / 2;

    delete  cameraFrameBuffer;
    cameraFrameBuffer = new 
      CameraFrameBuffer (this,
                         "CounterManager",
                         initailNumOfBuffs,        // 128,                      // _buffSize  Initial number of buffers.
                         maxNumOfBuffs,            // cameraBuffSizeMax,        // Maximum number of buffers.
                         cameraFrameHeight,
                         cameraFrameWidth,
                         operatingParameters->DataIsToBeCounted (),
                         operatingParameters->DataIsToBeRecorded (),
                         sampleLastFrameBeforeFlatField
                        );

    cameraFrameBuffer->FlatFieldEnabled (operatingParameters->FlatFieldEnabled ());
  }

  if  (operatingParameters->DataIsToBeRecorded ())
  {
    ScannerFileEntryPtr  sfe = ScannerFileEntry::GetOrCreateScannerFileEntry (destScannerFileRootName);
    sfe->ScanRate (operatingParameters->RequestedScanRate ());
    sfe->FullName (destScannerFileName);
    sfe->PixelsPerScanLine (cameraFrameWidth);
    cameraFrameBuffer->StartNewScannerFile (sfe);
  }

  ComputeLastFramesMinScanLines ();

  // At this point we have deleted all data structures that we are going to.  The 'curState' flag
  // is set to Starting so we should not be able to have any conflicts during rest of this method 
  // call.
  goalie->EndBlock ();

  if  (operatingParameters->DataIsToBeCounted ())
  {
    ValidateTrainingModel (TrainingModelName (), _successful, _errMsg);
    if  (!_successful)
    {
      runLog->Level (-1) << "PlayBackScannerFile   ***ERROR***  Validating training model: " << _errMsg << endl;
      curState = origState;
      //goalie->EndBlock ();
    }
  }

  if  (_successful)
  {
    StartReportWriterThread (_successful);
    if  (!_successful)
    {
      _errMsg = "Error starting 'ReportWriter' thread.";
      runLog->Level (-1) << "PlayBackScannerFile   ***ERROR***  " << _errMsg << endl;
      curState = origState;
      //goalie->EndBlock ();
    }
  }

  if  (_successful)
  {
    StartSnapshotThread (_successful);
    if  (!_successful)
    {
      _errMsg = "Error starting 'StatusSnapshot' thread.";
      runLog->Level (-1) << "PlayBackScannerFile   ***ERROR***  " << _errMsg << endl;
      curState = origState;
      //goalie->EndBlock ();
    }
  }

  if  (_successful  &&  operatingParameters->DataIsToBeRecorded ())
    StartCameraDiskWriterThread (_successful);

  if  (_successful  &&  operatingParameters->DataIsToBeCounted ())
    StartFrameBuilderAndProcessingThreads (_successful);

  if  (_successful)
  {
    acquisitionThread = 
      new CameraAcquisitionSimulator (this,
                                      playingBackRealTime,
                                      cameraFrameBuffer,
                                      srcScannerFileName,
                                      flowMeter,
                                      msgQueue,
                                      "CameraAcquisitionSimulator"
                                      );

    allThreads->PushOnBack (acquisitionThread);
    StartCameraAcquisitionThread (acquisitionThread, _successful);
    if  (!_successful)
    {
      _errMsg = "Error starting 'CameraAcquisitionSimulator' thread.";
      runLog->Level (-1) << "PlayBackScannerFile   ***ERROR***  " << _errMsg << endl;
    }
  }

  if  (!_successful)
  {
    runLog->Level (-1) << "PlayBackScannerFile   ***ERROR***  Playback failed to Start! Terminating all threads." << endl;
    secondaryMsgs.AddMsg ("Play-Back Failed to Start!!!");
    TerminateAndDeleteAllButPlayBackButtonThread ();
    curState = CounterState::Stopped;
  }
  else
  {
    curState = CounterState::PlayingBack;
    runLog->Level (40) << "PlayBackScannerFile   Successfully started." << endl;
    secondaryMsgs.AddMsg ("Play-Back Successfully Started.");
  }

  SaveConfiguration ();

  //goalie->EndBlock ();
  return;
}  /* PlayBackScannerFile*/



void  CounterManager::PlayBackButtonPressed 
                      (const KKStr&           _srcScannerFileName,
                       SessionParametersPtr   _sessionParameters,
                       OperatingParametersPtr _operatingParameters,
                       bool&                  _successful,
                       KKStr&                 _errMsg
                      )
{
  goalie->StartBlock ();
  _successful = true;

  _operatingParameters->PlayingBack (false);

  if  (CounterVariables::DebugLevel () >= 40)
  {
    runLog->Level (40) << "CounterManager::PlayBackButtonPressed  "       << endl
                       << "  srcScannerFileName     : " << _srcScannerFileName  << endl
                       << "  trainingModelName      : " << TrainingModelName () << endl
                       << "  controlNum             : " << ControlNum ()        << endl
                       << "  description            : " << Description ()       << endl;
    _operatingParameters->WriteToRunLog ("  OP:", *runLog);
  }

  _errMsg = "";

  if  ((curState != CounterState::Connected)  &&  (curState != CounterState::Stopped))
  {
    _errMsg = "Can only start PlayBack when in Connected or Stopped state!";
    runLog->Level (-1) << "PlayBackButtonPressed   ***ERROR***   " << _errMsg << endl
                       << "    Current State: " << this->CounterStateToStr (curState).QuotedStr () << endl;
    _successful = false;
    goalie->EndBlock ();
    return;
  }

  if  (!osFileExists (_srcScannerFileName))
  {
    _errMsg << "Scanner File: " << _srcScannerFileName.QuotedStr () << " Does not exist.";
    runLog->Level (-1) << "PlayBackButtonPressed   ***ERROR***   " << _errMsg << endl;
    _successful = false;
    goalie->EndBlock ();
    return;
  }

  ValidateCommonParameters (_sessionParameters,
                            _operatingParameters,
                            _errMsg
                           );

  if  (!_errMsg.Empty ())
  {
    _successful = false;
    runLog->Level (-1) << "PlayBackButtonPressed   ***ERROR***   Validating Parameters " << _errMsg << endl;
  }

  sessionParameters->Assign (*_sessionParameters);
  operatingParameters->Assign (*_operatingParameters);
  flowMeter->FlowRateRatioDefaultChanged (operatingParameters->FlowRateFactor ());

  SaveConfiguration ();

  if  (_successful)
  {
    playBackButtonThread 
      = new PlayBackButtonThread (this,
                                  _srcScannerFileName,
                                  msgQueue, 
                                  "PlayBackButtonThread"
                                 );

    allThreads->PushOnBack (playBackButtonThread);

    playBackButtonThread->Start (ThreadPriority::Normal, _successful);
  }

  runLog->Level (40) << "PlayBackButtonPressed   Exiting." << endl;

  goalie->EndBlock ();
}  /* PlayBackButtonPressed */



bool  CounterManager::OkToPressStop (KKStr&  errMsg)
{
  bool  okToPressStop = false;
  errMsg = "";
  if  ((stopButtonThread != nullptr)  &&  (stopButtonThread->Status () != CameraThread::ThreadStatus::Stopped))
  {
    errMsg = "Counter is already Stopping.";
  }

  else if  (startButtonThread  &&  startButtonThread->ThreadStillProcessing ())
  {
    errMsg = "Counter is still starting up;  wait for it to stop before pressing 'Stop'";
  }

  {
    switch  (curState)
    {
      case  CounterState::Starting:
      case  CounterState::BuildingClassifier:
        errMsg = "Counter is still starting up;  wait for it to stop before pressing 'Stop'";
        break;

      case  CounterState::Stopping:
        errMsg = "Counter is already 'Stopping'.";
        break;

      case  CounterState::DroppedFrames:
        errMsg = "Counter is processing dropped frames.";
        break;

      case  CounterState::Connected:
      case  CounterState::Connecting:
      {
        errMsg = "Counter is Not Running.";
        break;
      }

      case  CounterState::Running:
      case  CounterState::PlayingBack:
      {
        okToPressStop = true;
        break;
      }

      default:
      {
        break;
      }
    }
  }

  return  okToPressStop;
}  /* OkToPressStop */



void  CounterManager::StopButtonPressed (bool    _stopImmediately,
                                               bool&   _successful,
                                               KKStr&  _errMsg
                                              )
{
  goalie->StartBlock ();
  if  (OkToPressStop (_errMsg))
  {
    DeleteStopButtonThread ();

    stopButtonThread = new StopButtonThread (this, _stopImmediately, msgQueue, "StopButtonThread");
    allThreads->PushOnBack (stopButtonThread);

    stopButtonThread->Start (ThreadPriority::Normal, _successful);
    if  (!_successful)
      _errMsg = "Could not start the STOP process.";
  }

  goalie->EndBlock ();
}  /* StopButtonPressed */



/**
 *@brief  Will stop grabbing data from the camera and finish counting what in in the buffers.
 */
void  CounterManager::CloseOutCountingAndOrRecording (VolConstBool&  terminateFlag,
                                                            bool           stopImmediately
                                                           )
{
  goalie->StartBlock ();

  runLog->Level (40) << "CounterManager::CloseOutCountingAndOrRecording   stopImmediately: " << stopImmediately << endl;

  secondaryMsgs.AddMsg ("Stopping Counter.");

  // In case the StartButton thread is still running because it is building a classifier, especially 
  // a new one with lots of examples; We need to set the cancel flag so that the training process 
  // stops asap.
  trainerCancelFlag = true;

  if  ((curState != CounterState::Running)  &&  (curState != CounterState::PlayingBack)  &&  (curState != CounterState::Starting))
  {
    runLog->Level (10) << "CloseOutCountingAndOrRecording   We are not Running or playing back a recorded file." << endl;
    goalie->EndBlock ();
    return;
  }

  // We will first let the 'CameraAcquision' thread know that it does not need to
  // save data to disk anymore.  Then we will shutdown the other threads after they are finished
  // processing what is in the buffers.
  curState = CounterState::Stopping;
  stoppingIsInProgress = true;

  goalie->EndBlock ();

  if  (cameraFrameBuffer)
  {
    // When data is added to the 'cameraFrameBuffer' it will not be added to any queue now but it
    // will still be available for display to the screen.
    cameraFrameBuffer->DataIsToBeCountedAndOrRecorded (false,   /**< false = Do not count particles.        */
                                                       false    /**< false = Do not record imagery to disk. */
                                                      );

    // When new imagery data is added to the 'cameraFrameBuffer' it will not be flagged 
    // to be counted and/or written to disk.  Data that is already in the buffer will still 
    // be processed. 
  }

  osSleep (0.2f);

  ShutDownCameraAutoGainThread ();

  if  (PlayingBack ()  &&  (acquisitionThread != nullptr))
  {
    acquisitionThread->TerminateThread ();
    acquisitionThread->WaitForThreadToStop (10);
    if  (operatingMode == CounterOperatingModes::Advanced)
      secondaryMsgs.AddMsg ("Stopped Reading Scanner File.");
  }

  // Now that we stopped collecting data from the camera (Acquisition Device) we need to
  // wait until all the physical frames in the buffer have been processed.
  while  (!CameraFrameBufferEmpty ())
    osSleepMiliSecs (20);
  if  (operatingMode == CounterOperatingModes::Advanced)
    secondaryMsgs.AddMsg ("Camera Frame Buffers Empty.");


  // Now that there are no physical frames waiting to be processed (by both the "DiskWriterThread"
  // and the "LogicalFrameBuilder" threads) we can shutdown the "DiskWriterThread".
  //
  // We need to shutdown the "DiskWriterThread" before the "LogicalFrameBuilderThread" because
  // the last thing the "LogicalFrameBuilderThread" does when flagged to shutdown is process any
  // logical frames that have been dropped.  Before it can do that the Scanner File needs to be 
  // closed by the "DiskWriterThread".
  ShutDownDiskWriterThread ();

  // We set the "ShutDownFlag" to true by calling "ShutdownThread ()", this method only sets 
  // the flag to true.  Depending on how many logical frames have been dropped the "LogicalFrameBuilder"
  // may take a while to stop.

  if  (droppedFrames  &&  (!stopImmediately))
  {
    if  (droppedFrames->QueueSize () > 0)
    {
      curState = CounterState::DroppedFrames;
      logicalFrameBuilderThread->ShutdownThread ();
      while  (!LogicalFrameBuffersAreEmpty ()  &&  (!terminateFlag))
        osSleepMiliSecs (50);
    }
  }

  ShutDownLogicalFrameBuilderThread (stopImmediately);
  ShutDownLogicalFrameProcessorThreads (stopImmediately);
  ShutDownSnapshotThread ();
  ShutDownReportWriterThread ();

  if  (operatingMode == CounterOperatingModes::Advanced)
    secondaryMsgs.AddMsg ("Processing Last Frames.");
  // Will now wait until all threads have stopped.
  bool  allThreadsStopped = false;
  WaitForAllButCameraThreadsToStop (120, allThreadsStopped);

  // Taking a snapshot of statistics at time the processing was stopped.
  if  (!finaleStats)
    finaleStats = new CounterStats ();
  GetStats (*finaleStats);

  operatingParameters->PlayingBack (false);

  if  ((acquisitionThread != nullptr)  &&  (acquisitionThread->StartStatus () == CameraAcquisition::StartStatusType::Connected))
    curState = CounterState::Connected;
  else
    curState = CounterState::Stopped;

  goalie->StartBlock ();

  if  (operatingParameters->DataIsToBeCounted ()  &&  (!terminateFlag))
  {
    if  (operatingParameters->DataIsToBeRecorded ())
      lastScannerFileCounted = destScannerFileName;
    else
      lastScannerFileCounted = srcScannerFileName;

    if  (!stopImmediately) 
      generateFinaleReport = true;
  }

  SaveConfiguration ();

  goalie->EndBlock ();

  secondaryMsgs.AddMsg ("Counter Has Been Stopped.");

  runLog->Level (40) << "CloseOutCountingAndOrRecording   Exiting." << endl;

  stoppingIsInProgress = false;
}  /* CloseOutCountingAndOrRecording */



void  CounterManager::DeleteAllStoppedThreads ()
{
  runLog->Level (40) << "CounterManager::DeleteAllStoppedThreads." << endl;

  if  ((acquisitionThread)  &&  (acquisitionThread->Status () == ThreadStatus::Stopped))
    DeleteAcquisitionThread ();

  if  ((logicalFrameBuilderThread)  &&  (logicalFrameBuilderThread->Status () == ThreadStatus::Stopped))
    DeleteLogicalFrameBuilderThread ();

  if  (frameProcessors)
  {
    LogicalFrameProcessorList  threadsToDelete (false);

    LogicalFrameProcessorList::iterator  idx;
    for  (idx = frameProcessors->begin ();  idx != frameProcessors->end ();  ++idx)
    {
      LogicalFrameProcessorPtr  lfp = *idx;
      if  (lfp->Status () == ThreadStatus::Stopped)
        threadsToDelete.PushOnBack (lfp);
    }

    while  (threadsToDelete.QueueSize () > 0)
    {
      LogicalFrameProcessorPtr  lfp = threadsToDelete.PopFromBack ();
      frameProcessors->DeleteEntry (lfp);
      allThreads->DeleteEntry (lfp);
      delete  lfp;
    }
  }

  if  ((cameraAutoGainThread)  &&  (cameraAutoGainThread->Status () == ThreadStatus::Stopped))
    DeleteCameraAutoGainThread ();

  if  ((diskWriterThread)  &&  (diskWriterThread->Status () == ThreadStatus::Stopped))
    DeleteDiskWriterThread ();

  if  ((reportWriterThread)  &&  (reportWriterThread->Status () == ThreadStatus::Stopped))
    DeleteReportWriterThread ();

  if  ((snapshotThread)  &&  (snapshotThread->Status () == ThreadStatus::Stopped))
    DeleteSnapshotThread ();

  if  ((connectButtonThread)  &&  (connectButtonThread->Status () == ThreadStatus::Stopped))
    DeleteConnectButtonThread ();

  if  ((startButtonThread)  &&  (startButtonThread->Status () == ThreadStatus::Stopped))
    DeleteStartButtonThread ();

  if  ((playBackButtonThread)  &&  (playBackButtonThread->Status () == ThreadStatus::Stopped))
    DeletePlayBackButtonThread ();

  if  ((stopButtonThread)  &&  (stopButtonThread->Status () == ThreadStatus::Stopped))
    DeleteStopButtonThread ();

  runLog->Level (40) << "DeleteAllStoppedThreads   Exiting." << endl;

}  /* DeleteAllStoppedThreads */



void  CounterManager::WaitForAllButCameraThreadsToStop (kkint32  maxSecsToWait,
                                                              bool&  allThreadsStopped
                                                             )
{
  if  (allThreads != nullptr)
    return;

  runLog->Level (40) << "CounterManager::WaitForAllButCameraThreadsToStop." << endl
                     << "     maxSecsToWait: " << maxSecsToWait << endl;

  CameraThreadListPtr  tempList = new CameraThreadList (false);

  CameraThreadList::const_iterator  idx;
  for  (idx = allThreads->begin ();   idx != allThreads->end ();  ++idx)
  {
    CameraThreadPtr ct = *idx;
    if  ((ct != acquisitionThread)  &&  (ct != stopButtonThread))
    {
      runLog->Level (50) << "WaitForAllButCameraThreadsToStop   Thread: " << ct->ThreadName () << " Still Running." << endl;
      tempList->PushOnBack (ct);
    }
  }

  tempList->WaitForAllThreadsToStop ((float)maxSecsToWait, allThreadsStopped);

  delete  tempList;
  tempList = nullptr;

  if  (!allThreadsStopped)
    runLog->Level (-1) << "WaitForAllButCameraThreadsToStop   ***ERROR***   Not all threads stopped." << endl;

  runLog->Level (40) << "WaitForAllButCameraThreadsToStop   Exiting     allThreadsStopped: " << allThreadsStopped << endl;
}  /* WaitForAllButCameraThreadsToStop */



void  CounterManager::ShutdownOneThread (CameraThreadPtr  t)
{
  if  (t == nullptr)
    return;

  runLog->Level (20) << "ShutDownOneThread   Thread: " << t->ThreadName () << "  Status: " << t->StatusStr () << endl;
  if  (t->ThreadStillProcessing ())
  {
    runLog->Level (20) << "ShutDownOneThread   Issuing Shutdown to Thread: " << t->ThreadName () << endl;
    t->ShutdownThread ();
    t->WaitForThreadToStop (20);
  }
  if  (t->ThreadStillProcessing ())
    runLog->Level (-1) << "ShutDownOneThread   ***ERROR***   Thread: " << t->ThreadName () << "Still Running;  Status: " << t->StatusStr () << endl;
}



void  CounterManager::ShutDownCameraAutoGainThread ()
{
  ShutdownOneThread (cameraAutoGainThread);
}  /* ShutDownCameraAutoGainThread */



void  CounterManager::ShutDownDiskWriterThread ()
{
  ShutdownOneThread (diskWriterThread);
}  /* ShutDownDiskWriterThread */



void  CounterManager::ShutDownSnapshotThread ()
{
  ShutdownOneThread (snapshotThread);
}  /* ShutDownSnapshotThread */



void  CounterManager::ShutDownReportWriterThread ()
{
  ShutdownOneThread (reportWriterThread);
}  /* ShutDownReportWriterThread */



void  CounterManager::ShutDownLogicalFrameBuilderThread (bool stopImmediately)
{
  if  (!logicalFrameBuilderThread)
    return;

  runLog->Level (20) << "ShutDownLogicalFrameBuilderThread   stopImmediately: " << stopImmediately << endl;
  if  (stopImmediately)
  {
    logicalFrameBuilderThread->TerminateThread ();
    logicalFrameBuilderThread->WaitForThreadToStop (20);
  }
  else
  {
    ShutdownOneThread (logicalFrameBuilderThread);
  }
}  /* ShutDownLogicalFrameBuilderThread */



void  CounterManager::ShutDownLogicalFrameProcessorThreads (bool stopImmediately)
{
  if  (!frameProcessors)
    return;

  runLog->Level (20) << "ShutDownLogicalFrameProcessorThreads   stopImmediately: " << stopImmediately << endl;

  if  (stopImmediately)
  {
    frameProcessors->TerminateThreads ();
    frameProcessors->WaitForThreadsToStop (120);
  }
  else
  {
    frameProcessors->ShutdownThreads ();
    frameProcessors->WaitForThreadsToStop (120);
  }

  if  (frameProcessors->ThreadsStillRunning ())
    runLog->Level (-1) << "ShutDownLogicalFrameProcessorThreads   ***ERROR***   Not all logical processors shutdown or terminated." << endl;
}  /* ShutDownLogicalFrameProcessorThreads */



void  CounterManager::DeleteOneThread (CameraThreadPtr &t)
{
  if  (t)
  {
    runLog->Level (20) << "DeleteOneThread   Thread: " << t->ThreadName () << "  Status: " << t->StatusStr () << endl;
    
    if  (t->ThreadStillProcessing ())
    {
      t->TerminateThread ();
      t->WaitForThreadToStop (10);
    }

    if  (t->ThreadStillProcessing ())
    {
      runLog->Level (-1) << "DeleteOneThread   ***ERROR***   Thread: " << t->ThreadName () << "  Still Running;  Status: " << t->StatusStr () << endl;
      runLog->Flush ();
    }
    else
    {
      allThreads->DeleteEntry (t);
      delete  t;
      t = nullptr;
    }
  }
}  /* DeleteOneThread */



void  CounterManager::DeleteAcquisitionThread ()
{
  DeleteOneThread ((CameraThreadPtr&)acquisitionThread);
}  /* DeleteAcquisitionThread */



void  CounterManager::DeleteCameraAutoGainThread ()
{
   DeleteOneThread ((CameraThreadPtr&)cameraAutoGainThread);
}  /* DeletecameraAutoGainThread */



void  CounterManager::DeleteDiskWriterThread ()
{
  DeleteOneThread ((CameraThreadPtr&)diskWriterThread);
}  /* DeleteDiskWriterThread */



void  CounterManager::DeleteReportWriterThread ()
{
  DeleteOneThread ((CameraThreadPtr&)reportWriterThread);
}  /* DeleteReportWriterThread */



void  CounterManager::DeleteSnapshotThread ()
{
  DeleteOneThread ((CameraThreadPtr&)snapshotThread);
}  /* DeleteSnapshotThread */



void  CounterManager::DeleteLogicalFrameBuilderThread ()
{
  DeleteOneThread ((CameraThreadPtr&)logicalFrameBuilderThread);
}  /* DeleteLogicalFrameBuilderThread */



void  CounterManager::DeleteLogicalFrameProcessorThreads ()
{
  runLog->Level (20) << "DeleteLogicalFrameProcessorThreads" << endl;
  if  (!frameProcessors)
    return;

  if  (frameProcessors->ThreadsStillRunning ())
  {
    frameProcessors->TerminateThreads ();
    frameProcessors->WaitForThreadsToStop (1);
  }

  LogicalFrameProcessorPtr  fp = frameProcessors->PopFromBack ();
  while  (fp != nullptr)
  {
    if  (fp->ThreadStillProcessing ())
      runLog->Level (-1) << "DeleteLogicalFrameProcessorThreads   ***ERROR***  LogicalFrameProcessor: " << fp->ThreadName () << "  Status: " << fp->StatusStr () << "   Still processing." << endl;

    /**@todo  If Still processing should not just delete instance of thread.  */

    allThreads->DeleteEntry (fp);
    delete  fp;
    fp = nullptr;
    fp = frameProcessors->PopFromBack ();
  }

  delete  frameProcessors;
  frameProcessors = nullptr;
  runLog->Level (20) << "DeleteLogicalFrameProcessorThreads   Exiting." << endl;
}  /* DeleteLogicalFrameProcessorThreads */



void  CounterManager::DeleteConnectButtonThread ()
{
  DeleteOneThread ((CameraThreadPtr&)connectButtonThread);
}



void  CounterManager::DeleteStartButtonThread ()
{
  DeleteOneThread ((CameraThreadPtr&)startButtonThread);
}



void  CounterManager::DeletePlayBackButtonThread ()
{
  DeleteOneThread ((CameraThreadPtr&)playBackButtonThread);
}



void  CounterManager::DeleteStopButtonThread ()
{
  DeleteOneThread ((CameraThreadPtr&)stopButtonThread);
}



//  Will stay in a loop until all threads that this object controls are stopped.
void  CounterManager::ShutDownAllThreads ()
{
  // We will first cancel the camera/ data-source thread.  Then wait for the other threads to
  // finish processing what is left in buffers.

  stoppingIsInProgress = true;

  if  (acquisitionThread  &&  acquisitionThread->ThreadStillProcessing ())
  {
    acquisitionThread->ShutdownThread ();
    acquisitionThread->WaitForThreadToStop (10);
  }

  ShutDownLogicalFrameBuilderThread (false);
  ShutDownCameraAutoGainThread ();
  ShutDownDiskWriterThread ();
  ShutDownLogicalFrameProcessorThreads (false);
  ShutDownSnapshotThread ();
  ShutDownReportWriterThread ();
  allThreads->SendShutdownCmdToAllThreads ();

  bool  allThreadsStopped = false;
  allThreads->WaitForAllThreadsToStop (120.0f, allThreadsStopped);

  stoppingIsInProgress = false;
}  /* ShutDownAllThreads */



void  CounterManager::TerminateAllThreads ()
{
  bool  allThreadsStopped = false;
  if  (allThreads)
  {
    allThreads->SendTerminateCmdToAllThreads ();
    allThreads->WaitForAllThreadsToStop (10.0f, allThreadsStopped);
  }
}  /* TerminateAllThreads */



void  CounterManager::TerminateAndDeleteAllButPlayBackButtonThread ()
{
  DeleteLogicalFrameBuilderThread    ();
  DeleteLogicalFrameProcessorThreads ();
  DeleteCameraAutoGainThread         ();
  DeleteDiskWriterThread             ();
  DeleteAcquisitionThread            ();
  DeleteConnectButtonThread          ();
  DeleteReportWriterThread           ();
  DeleteStartButtonThread            ();
  DeleteStopButtonThread             ();
}  /* TerminateAndDeleteAllButPlayBackButtonThread */



void  CounterManager::TerminateAndDeleteAllButCameraAndStartButtonThreads ()
{
  DeleteLogicalFrameBuilderThread    ();
  DeleteLogicalFrameProcessorThreads ();
  DeleteCameraAutoGainThread         ();
  DeleteDiskWriterThread             ();
  DeleteReportWriterThread           ();
  DeleteConnectButtonThread          ();
  DeletePlayBackButtonThread         ();
  DeleteStopButtonThread             ();
}  /* TerminateAndDeleteAllButCameraAndStartButtonThreads */



void  CounterManager::TerminateAndDeleteAllButConnectButtonThread ()
{
  DeleteLogicalFrameBuilderThread    ();
  DeleteLogicalFrameProcessorThreads ();
  DeleteCameraAutoGainThread         ();
  DeleteDiskWriterThread             ();
  DeleteAcquisitionThread            ();
  DeleteReportWriterThread           ();
  DeleteStartButtonThread            ();
  DeletePlayBackButtonThread         ();
  DeleteStopButtonThread             ();
}  /* TerminateAndDeleteAllButConnectButtonThread */



void   CounterManager::DeleteAllThreads ()
{
  if  (allThreads)
  {
    TerminateAllThreads ();
    while  (allThreads->size () > 0)
    {
      CameraThreadPtr  t = allThreads->PopFromFront ();
      delete  t;
      t = nullptr;
    }
  }

  delete  frameProcessors;
  frameProcessors = new LogicalFrameProcessorList (false);

  acquisitionThread         = nullptr;
  cameraAutoGainThread      = nullptr;
  diskWriterThread          = nullptr;
  reportWriterThread        = nullptr;
  logicalFrameBuilderThread = nullptr;
  startButtonThread         = nullptr;
  playBackButtonThread      = nullptr;
  stopButtonThread          = nullptr;
  connectButtonThread       = nullptr;
}  /* DeleteAllThreads */



void  CounterManager::StartCameraAcquisitionThread (CameraAcquisitionPtr  acquisitionThreadToStart, 
                                                    bool&                 _successful
                                                   )
{
  secondaryMsgs.AddMsg ("Starting Camera Acquisition.");

  acquisitionThreadToStart->RequestedCameraParameters (operatingParameters);

  acquisitionThreadToStart->CropSettingsChanged (cropLeft, cropRight);

  acquisitionThreadToStart->Start (ThreadPriority::High, _successful);
  if  (_successful)
  {
    while  (acquisitionThreadToStart->StartStatus() != CameraAcquisition::StartStatusType::Connected)
    {
      osSleep (0.1f);
      if  (acquisitionThreadToStart->Crashed ())
      {
        _successful = false;
        break;
      }
    }
  }
}  /* StartCameraAcquisitionThread*/



void  CounterManager::StartCameraDiskWriterThread (bool&  _successful)
{
  _successful = false;
  DeleteDiskWriterThread ();

  // For the currently selected FileFormat we want to set the compensation-table that will
  // compensate the camera data for the effect of the compression performed by the specified
  // ScannerFileFormat.  The motivation is to make sure that the classifier receives images 
  // that are identical from playing back a existing Scanner-File as from the camera.
  cameraFrameBuffer->SetCompensationTable (ScannerFile::ConpensationTable (operatingParameters->DestScannerFileFormat ()));


  // Since we are going to create a new ScannerFile we need to get a new SessionId to assign to it.
  sessionParameters->SessionId (SessionLogger::GetNextSessionId ());
  sessionParameters->AddToHeaderFields (headerFields);
  SessionLogger::AddSessionEntry (sessionParameters, operatingParameters);

  diskWriterThread  = new DiskWriterThread  (this, 
                                             cameraFrameBuffer, 
                                             msgQueue,
                                             destScannerFileName,
                                             operatingParameters->DestScannerFileFormat (),
                                             cameraFrameHeight,
                                             cameraFrameWidth,
                                             "DiskWriterThread"
                                            );

  diskWriterThread->CropSettingsChanged (cropLeft, cropRight);

  diskWriterThread->Start (ThreadPriority::Normal, _successful);
  allThreads->PushOnBack (diskWriterThread);
}



void  CounterManager::StartReportWriterThread (bool&  _successful)
{
  _successful = false;
  DeleteReportWriterThread ();

  if  (!particleEntryBuffer)
    particleEntryBuffer = new ParticleEntryBuffer ();
  else
    particleEntryBuffer->Clear ();

  KKStr  initialScannerFileName = destScannerFileName;
  if  (!operatingParameters->DataIsToBeRecorded ())
    initialScannerFileName = srcScannerFileName;

  reportWriterThread = new ReportWriterThread (this,
                                               particleEntryBuffer,
                                               initialScannerFileName,
                                               sessionParameters,
                                               operatingParameters,
                                               msgQueue,
                                               "ReportWriterThread"
                                              );

  reportWriterThread->Start (ThreadPriority::Normal, _successful);
  allThreads->PushOnBack (reportWriterThread);
}  /* StartReportWriterThread */



void  CounterManager::StartSnapshotThread (bool&  _successful)
{
  _successful = false;
  DeleteSnapshotThread ();

  if  (snapshotBuffer)
    snapshotBuffer->Reset ();
  else
    snapshotBuffer = new StatusSnapshotBuffer ();
  
  delete  snapshotThread;
  snapshotThread = new StatusSnapshotThread (this, snapshotBuffer, snapshotInterval, msgQueue, "StatusSnapshotThread");

  snapshotThread->Start (ThreadPriority::Normal, _successful);
  allThreads->PushOnBack (snapshotThread);
}



void   CounterManager::StartFrameBuilderAndProcessingThreads (bool&  _successful)
{
  _successful= true;

  cameraFrameBuffer->SetCompensationTable (ScannerFile::ConpensationTable (operatingParameters->DestScannerFileFormat ()));

  if  (operatingMode == CounterOperatingModes::Advanced)
    secondaryMsgs.AddMsg ("Starting up Frame Processors.");

  delete  frameProcessors;
  frameProcessors = new LogicalFrameProcessorList (false);

  if  (!logicalFrames)
    logicalFrames = new LogicalFrameList (logicalFrameQueueSizeMax, 
                                          frameHeightMax, 
                                          cameraFrameWidth, 
                                          *runLog
                                         );

  if  (!logicalFrameEntries)
    logicalFrameEntries = new LogicalFrameEntryList (true);
  
  if  (!droppedFrames)
    droppedFrames = new LogicalFrameEntryQueue (true, "DroppedFrames");

  if  (!particleEntryBuffer)
    particleEntryBuffer = new ParticleEntryBuffer ();

  if  (!lastParticlesProcessed)
    lastParticlesProcessed = new RasterBuffer ("LastParticlesProcessed", 8);

  //int numFrameProcessesToStart = Min (5, Max (1, (maxNumOfThreads - 2)));

  int  maxNumFrameHandelers = maxNumOfThreads - 4;
  if  (this->cameraFrameBuffer->FrameWidth () > 2048)
    maxNumFrameHandelers /= 2;

  int numFrameProcessesToStart = Min (maxNumFrameHandelers, Max (1, (maxNumOfThreads - 3)));
  //int numFrameProcessesToStart = 1;

  for  (kkint32 x = 0;  x < numFrameProcessesToStart;  ++x)
  {
    KKStr  threadName = "LogicalFrameProcessor_" + StrFormatInt (x, "00");
    LogicalFrameProcessorPtr  fp = new LogicalFrameProcessor (this, 
                                                              TrainingModelName (),
                                                              operatingParameters,
                                                              lastParticlesProcessed,
                                                              logicalFrames,
                                                              particleEntryBuffer,
                                                              particlesDirName,
                                                              frameHeightMax,      /**< Max Logical Frame Height */
                                                              cameraFrameWidth,
                                                              cropLeft,
                                                              cropRight,
                                                              msgQueue,
                                                              threadName
                                                             );
    frameProcessors->PushOnBack (fp);
    allThreads->PushOnBack (fp);
    fp->Start (ThreadPriority::Normal, _successful);
    if  (!_successful)
    {
      runLog->Level (-1) << "StartFrameBuilderAndProcessingThreads   ***ERROR***   Thread failed to start." << endl;
      break;
    }
  }

  if  (_successful)
  {
    logicalFrameBuilderThread = new LogicalFrameBuilder (this, 
                                                         cameraFrameBuffer, 
                                                         logicalFrames,
                                                         logicalFrameEntries,
                                                         droppedFrames,
                                                         cameraFrameWidth,
                                                         frameHeightMax,           // Scan lines to allocate.
                                                         operatingParameters->BackGroundPixelTH (),
                                                         msgQueue, 
                                                         "LogicalFrameBuilder"
                                                        );

    logicalFrameBuilderThread->CropSettings (cropLeft, cropRight);

    allThreads->PushOnBack (logicalFrameBuilderThread);
    logicalFrameBuilderThread->Start (ThreadPriority::Normal, _successful);

    if  (!_successful)
    {
      runLog->Level (-1) << "LogicalFrameBuilder Thread failed to start." << endl;
      return;
    }
    else
    {
      if  (operatingMode == CounterOperatingModes::Advanced)
        secondaryMsgs.AddMsg ("Frame Processors Started.");
    }
  }
}  /* StartFrameBuilderAndProcessingThreads */



void  CounterManager::HeaderFieldsClear ()
{
  headerFields->Clear ();
}



void  CounterManager::AddHeaderField (const KKStr&  _fieldName,
                                            const KKStr&  _fieldValue
                                           )
{
  headerFields->Add (_fieldName, _fieldValue);
}



void  CounterManager::AddHeaderField (const KKStr&  _fieldName,
                                      bool          _fieldValue
                                     )
{
  headerFields->Add (_fieldName, _fieldValue);
}



void  CounterManager::AddHeaderField (const KKStr&  _fieldName,
                                      kkint32       _fieldValue
                                     )
{
  headerFields->Add (_fieldName, _fieldValue);
}



void  CounterManager::AddHeaderField (const KKStr&  _fieldName,
                                      kkint64       _fieldValue
                                     )
{
  headerFields->Add (_fieldName, _fieldValue);
}



void  CounterManager::AddHeaderField (const KKStr&  _fieldName,
                                      float         _fieldValue
                                     )
{
  headerFields->Add (_fieldName, _fieldValue);
}



void  CounterManager::AddHeaderField (const KKStr&  _fieldName,
                                      double        _fieldValue
                                     )
{
  headerFields->Add (_fieldName, _fieldValue);
}



void  CounterManager::AddMessageToCurrentScannerFile (const KKStr&  msg)
{
  if  (diskWriterThread)
    diskWriterThread->AddMessageToCurrentScannerFile (msg);
}

 

VectorUcharPtr  CounterManager::CameraHighPoints ()
{
  if  (!cameraFrameBuffer)
    return nullptr;
  VectorUcharPtr  cameraHighs = cameraFrameBuffer->CameraHighPoints ();
  return  cameraHighs;
}



VectorUcharPtr  CounterManager::CameraHighPointsFromLastNSampleLines (kkint32 n)  const
{
  if  (!cameraFrameBuffer)
    return nullptr;
  VectorUcharPtr  cameraHighs = cameraFrameBuffer->CameraHighPointsFromLastNSampleLines (n);
  return  cameraHighs;
}



VectorUcharPtr  CounterManager::LastFrameAverageScanLine()  const
{
  if  (!cameraFrameBuffer)
    return nullptr;
  VectorUcharPtr  avgScanLine = cameraFrameBuffer->LastFrameAverageScanLine ();
  return  avgScanLine;
}



VectorUcharPtr  CounterManager::LastFrameHighValuesScanLine()  const
{
  if  (!cameraFrameBuffer)
    return nullptr;
  VectorUcharPtr  avgScanLine = cameraFrameBuffer->LastFrameHighValuesScanLine ();
  return  avgScanLine;
}



bool  CounterManager::StoppingIsInProgress ()  const
{
  return stoppingIsInProgress;
}



KKStr  CounterManager::CameraSerialNum ()
{
  if  (acquisitionThread)
    return  acquisitionThread->SerialNumber ();
  else  
    return KKStr::EmptyStr ();
}



KKStr  CounterManager::CameraMacAddress ()
{
  if  (acquisitionThread)
    return  acquisitionThread->MacAddress ();
  else
    return  KKStr::EmptyStr ();
}



void  CounterManager::ResetCounts  ()
{
  if  (allThreads)
  {
    CameraThreadList::iterator  idx;
    for  (idx = allThreads->begin ();  idx != allThreads->end ();  ++idx)
      (*idx)->ResetCounts ();
  }
}



kkint32  CounterManager::ParticlesCounted ()
{
  //
  kkint32  particlesCounted = 0;
  if  (frameProcessors)
  {
    LogicalFrameProcessorList::iterator  idx;
    for  (idx = frameProcessors->begin ();  idx != frameProcessors->end ();  ++idx)
      particlesCounted += (*idx)->ParticlesCounted ();
  }

  return  particlesCounted;
}  /* ParticlesCounted */



void   CounterManager::PlayingBackRealTime (bool  _playingBackRealTime)
{
  playingBackRealTime = _playingBackRealTime;
}



const CameraParametersPtr   CounterManager::CameraParams ()  const
{
  if  (acquisitionThread)
    return acquisitionThread->CameraParams ();
  else
    return nullptr;
}



void  CounterManager::FrameHeightWidthChanged (kkint32  _newFrameHeight,
                                                     kkint32  _newFrameWidth
                                                    )
{
  cameraFrameHeight = _newFrameHeight;
  cameraFrameWidth  = _newFrameWidth;
}


void  CounterManager::AddSecondaryMsg (const KKStr&  msg)
{
  secondaryMsgs.AddMsg (msg);
}



/**
 *@brief  retrieves run time variables that would typically be displayed allowing technician to see what is going on.
 *@param[out]  stat's
 */
void  CounterManager::GetStats (CounterStats&  stats)
{
  stats.Reset ();

  if  (cameraFrameBuffer)
    cameraFrameBuffer->GetStats (stats);

  if  (acquisitionThread)
    acquisitionThread->GetStats (stats);

  if  (logicalFrameBuilderThread)
    logicalFrameBuilderThread->GetStats (stats);

  if  (frameProcessors)
    frameProcessors->GetStats (stats);

  if  (logicalFrames)
    stats.LogicalFramesOnQueue (logicalFrames->NumWaitingToBeProcessed ());

  if  (droppedFrames)
    stats.LogicalFramesDropped (droppedFrames->QueueSize ());

  if  (diskWriterThread)
    diskWriterThread->GetStats (stats);

  if  (reportWriterThread)
    reportWriterThread->GetStats (stats);

  if  (flowMeter)
    stats.FlowRate (flowMeter->FlowRateTrend ());

  stats.CropLeft  (cropLeft);
  stats.CropRight (cropRight);

  stats.TotalBytesToRead (totalBytesToRead);
}  /* GetStats */



void  CounterManager::GetSessionParameters (SessionParameters&  _sessionParameters)
{
  _sessionParameters.Assign (*sessionParameters);
}



void  CounterManager::GetOperatingParameters (OperatingParameters&  _operatingParameters)
{
  _operatingParameters.Assign (*operatingParameters);
}



void  CounterManager::GetDefaultOperatingParameters (OperatingParameters&  _defaultOperatingParameters)
{
  _defaultOperatingParameters.Assign (*defaultOperatingParameters);
}



kkint32  CounterManager::GetLastSessionId ()
{
  return  SessionLogger::GetLastSessionId ();
}



void  CounterManager::GetLogicalFrameStats (kkint32&  _logicalFramesOnQueue,
                                            kkint32&  _logicalFrameProcessorsAvailable
                                           )
{
  if  (logicalFrames)
    _logicalFramesOnQueue = logicalFrames->NumWaitingToBeProcessed ();
  else
    _logicalFramesOnQueue = 0;

  if  (frameProcessors)
    _logicalFrameProcessorsAvailable = frameProcessors->GetNumLogicalFrameProcessorsAvailable ();

}  /* GetLogicalFrameStats */



bool  CounterManager::GenerateFinaleReport ()
{
  goalie->StartBlock ();
  bool  ok = generateFinaleReport;
  if  (ok)
    generateFinaleReport = false;
  goalie->EndBlock ();
  return  ok;
}



int  CounterManager::FrameProcessorsCount () const
{
  if  (frameProcessors)
    return frameProcessors->QueueSize ();
  else
    return 0;
}



bool  CounterManager::LogicalFrameBuilderRunning ()
{
  return  ((logicalFrameBuilderThread != nullptr)  &&  (logicalFrameBuilderThread->Status () == ThreadStatus::Running));
}



const KKStr&  CounterManager::ControlNum ()
{
  if  (sessionParameters == nullptr)
    return KKStr::EmptyStr ();
  else
    return sessionParameters->ControlNum ();
}



const KKStr&  CounterManager::Description () const
{
  if  (sessionParameters == nullptr)
    return KKStr::EmptyStr ();
  else
    return sessionParameters->SessionDescription ();
}



bool  CounterManager::EmbeddedFlowMeter () const
{
  if  (installation)
    return  (installation->FlowMeterMethod () == InstallationConfig::FlowMeterMethods::Embedded);
  else
    return false;
}



const KKStr&  CounterManager::TrainingModelName () const
{
  if  (sessionParameters == nullptr)
    return KKStr::EmptyStr ();
  else
    return sessionParameters->TrainingModelName ();
}



bool  CounterManager::DiskWritingThreadRunning ()  const
{
  return  ((diskWriterThread != nullptr)  &&  (diskWriterThread->Status () == ThreadStatus::Running));
}



bool  CounterManager::CameraAutoGainThreadRunning ()  const
{
  return  ((cameraAutoGainThread != nullptr)  &&  (cameraAutoGainThread->Status () == ThreadStatus::Running));
}



bool  CounterManager::CameraFrameBufferEmpty ()
{
  if  (cameraFrameBuffer)
  {
    if  ((cameraFrameBuffer->NumWaitingToWriteToDisk () > 0)  ||
         (cameraFrameBuffer->NumWaitingToProcess     () > 0)
        )
     return false;
  }
  return  true;
}



bool  CounterManager::LogicalFrameBuffersAreEmpty ()
{
  if  (((logicalFrames       != nullptr)  &&  (logicalFrames->NumWaitingToBeProcessed () > 0))  ||
       ((droppedFrames       != nullptr)  &&  (droppedFrames->QueueSize               () > 0))  ||
       ((particleEntryBuffer != nullptr)  &&  (particleEntryBuffer->DataAvailable     ()))
      )
  {
    return false;
  }
  else
  {
    return  true;
  }
}



StatusSnapshotListPtr  CounterManager::SnapshotsRetrieve (KKB::DateTime  rangeStart,
                                                                KKB::DateTime  rangeEnd
                                                               )
{
  if  (snapshotBuffer)
    return snapshotBuffer->Retrieve (rangeStart, rangeEnd);
  else
    return nullptr;
}



const StatusSnapshotPtr  CounterManager::SnapshotLastEntry ()
{
  if  (snapshotBuffer)
    return snapshotBuffer->LastEntry ();
  else
    return nullptr;
}



RasterPtr  CounterManager::GetLastParticle ()
{
  RasterPtr  lastParticle = nullptr;

  if  (lastParticlesProcessed)
    lastParticle = lastParticlesProcessed->GetNextRaster ();
  return  lastParticle;
}



void  CounterManager::StartNewScannerFile (ScannerFileEntryPtr  _scannerFileEntry)
{
  if  (cameraFrameBuffer)
  {
    cameraFrameBuffer->StartNewScannerFile (_scannerFileEntry);
  }
}



void  CounterManager::StartingToReadNewScannerFile (ScannerFileEntryPtr  _scannerFileEntry)
{
  if  (!operatingParameters->DataIsToBeRecorded ())
  {
    // Since we are not recording a new Scanner File we need to use the name of the scanner files that we
    // are reading from to drive the name of the extracted images.
    if  (cameraFrameBuffer)
      cameraFrameBuffer->StartNewScannerFile (_scannerFileEntry);
  }
}



void  CounterManager::ComputeLastFramesMinScanLines ()
{
  kkint32  srcWidth = cameraFrameWidth;
  kkint32  cropWidth = 1 + cropRight - cropLeft;

  if  ((cropWidth < 20)  ||  (cropWidth > cameraFrameWidth))
    srcWidth = cameraFrameWidth;
  else
    srcWidth = cropWidth;
    

  kkint32  srcHeight = (kkint32)(0.5f + (float)srcWidth * (float)liveVideoHeight / (float)liveVideoWidth);
  if  (cameraFrameBuffer)
    cameraFrameBuffer->SetLastFramesMinScanLines (srcHeight);
}



void  CounterManager::SetLiveVideoDimensions (kkint32  _liveVideoHeight,
                                                    kkint32  _liveVideoWidth
                                                   )
{
  liveVideoHeight = _liveVideoHeight;
  liveVideoWidth  = _liveVideoWidth;
  ComputeLastFramesMinScanLines ();
}



RasterPtr  CounterManager::SnapShotLatestFrame ()
{
  goalie->StartBlock ();
  RasterPtr  lastVideoFrame = nullptr;
  if  (cameraFrameBuffer != nullptr)
    lastVideoFrame = cameraFrameBuffer->SnapShotLatestFrame (CropLeft (), CropRight ());
  goalie->EndBlock ();
  return  lastVideoFrame;
}  /* SnapShotLatestFrame */



bool  CounterManager::CameraThreadRunning ()  const
{
  bool  cameraThreadRunning = false;
  goalie->StartBlock ();
  cameraThreadRunning = ((acquisitionThread != nullptr)  &&  acquisitionThread->ThreadStillProcessing ());
  goalie->EndBlock ();
  return  cameraThreadRunning;
}



void  CounterManager::BackGroundPixelTH (uchar _backGroundPixelTH)
{
  operatingParameters->BackGroundPixelTH (_backGroundPixelTH);
  AddHeaderField ("OP:BackGroundPixelTH", (kkint32)_backGroundPixelTH);
  if  (logicalFrameBuilderThread)
    logicalFrameBuilderThread->BackGroundPixelTH (_backGroundPixelTH);

  if  (frameProcessors)
  {
    LogicalFrameProcessorList::const_iterator  idx;
    for  (idx = frameProcessors->begin ();  idx != frameProcessors->end ();  ++idx)
    {
      LogicalFrameProcessorPtr  fp = *idx;
      fp->BackGroundPixelTH (_backGroundPixelTH);
    }
  }
}  /* BackGroundPixelTH */



kkint32 CounterManager::CropLeft () const
{
  return  cropLeft;
}



kkint32  CounterManager::CropRight () const
{
  return  cropRight;
}



kkint32  CounterManager::FrameWidthInPixels () const
{
  kkint32  fw = 0;
  if  (acquisitionThread)
    fw = acquisitionThread->FrameWidth ();

  if  ((fw == 0)  &&  cameraFrameBuffer)
    fw = cameraFrameBuffer->FrameWidth ();

  if  (fw == 0)
    fw = 2048;

  return fw;
}



float   CounterManager::CurAnalogGain () const
{
  if  (acquisitionThread)
    return  acquisitionThread->CurAnalogGain ();
  else
    return 0.0;
}



kkint32 CounterManager::CurDigitalGain () const
{
  if  (acquisitionThread)
    return  acquisitionThread->CurDigitalGain ();
  else
    return 0;
}



float  CounterManager::CurScanRate () const
{
  if  (acquisitionThread)
    return  acquisitionThread->CurScanRate ();
  else
    return 0;
}



void  CounterManager::CropSettingsChanged (kkint32  _cropLeft,
                                           kkint32  _cropRight
                                          )
{
  cropLeft  = _cropLeft;
  cropRight = _cropRight;

  CameraThreadList::iterator idx;
  for  (idx = allThreads->begin ();  idx != allThreads->end ();  ++idx)
  {
    CameraThreadPtr  ct = *idx;
    ct->CropSettingsChanged (_cropLeft, _cropRight);
  }

  ComputeLastFramesMinScanLines ();
}  /* CropSettings */



void  CounterManager::ScanRateChanged (float _newScanRate)
{
  if  (flowMeter)
    flowMeter->ScanRateChanged (_newScanRate);

  if  (cameraFrameBuffer)
    cameraFrameBuffer->ScanRateChanged (_newScanRate);

  for  (auto idx: *allThreads)
  {
    CameraThreadPtr  ct = idx;
    ct->ScanRateChanged (_newScanRate);
  }
}



void  CounterManager::FlatFieldEnabled (bool  _flatFieldEnabled)
{
  operatingParameters->FlatFieldEnabled (_flatFieldEnabled);
  AddHeaderField ("OP:FlatFieldEnabled", _flatFieldEnabled);

  if  (cameraFrameBuffer)
    cameraFrameBuffer->FlatFieldEnabled (_flatFieldEnabled);

  if  (acquisitionThread)
    acquisitionThread->FlatFieldEnabled (_flatFieldEnabled);
}



void  CounterManager::MinSizeThreshold (kkint32 _minSizeThreshold)
{
  operatingParameters->MinSizeThreshold (_minSizeThreshold);
  AddHeaderField ("OP:MinSizeThreshold", _minSizeThreshold);

  if  (frameProcessors)
  {
    LogicalFrameProcessorList::iterator  idx;
    for  (idx = frameProcessors->begin ();  idx != frameProcessors->end ();  ++idx)
      (*idx)->MinSizeThreshold (_minSizeThreshold);
  }
}



bool  CounterManager::WeAreRecordingToDisk ()
{
  return  (((curState == CounterState::Running)  ||  (curState == CounterState::PlayingBack))             &&
           (diskWriterThread  != nullptr)  &&  (acquisitionThread != nullptr)           &&
           (diskWriterThread->WeAreRecordingToDisk ())                            &&
           (acquisitionThread->StartStatus () == CameraAcquisition::StartStatusType::Connected)  &&
           (acquisitionThread->Status ()      == ThreadStatus::Running)
          );
}



void  CounterManager::RequestedAnalogGain (float _requestedAnalogGain)
{
  operatingParameters->RequestedAnalogGain (_requestedAnalogGain);
  if  (acquisitionThread)
    acquisitionThread->RequestedAnalogGain (_requestedAnalogGain);
}



void  CounterManager::RequestedDigitalGain (kkint32 _requestedDigitalGain)
{
  operatingParameters->RequestedDigitalGain (_requestedDigitalGain);
  if  (acquisitionThread)
    acquisitionThread->RequestedDigitalGain (_requestedDigitalGain);
}



void  CounterManager::RequestedScanRate (float _requestedScanRate)
{
  operatingParameters->RequestedScanRate (_requestedScanRate);
  if  (acquisitionThread)
    acquisitionThread->RequestedScanRate (_requestedScanRate);
}


void  CounterManager::RequestedSensitivityMode (const KKStr&  _requestedSensitivityMode)
{
  operatingParameters->RequestedSensitivityMode (_requestedSensitivityMode);
  if  (acquisitionThread)
    acquisitionThread->RequestedSensitivityMode (_requestedSensitivityMode);
}



void   CounterManager::SampleLastFrameBeforeFlatField (bool _sampleLastFrameBeforeFlatField)
{
  sampleLastFrameBeforeFlatField = _sampleLastFrameBeforeFlatField;
  if  (cameraFrameBuffer)
    cameraFrameBuffer->SampleLastFrameBeforeFlatField (_sampleLastFrameBeforeFlatField);
}



void   CounterManager::SnapshotInterval (kkint32 _snapshotInterval)   
{
  snapshotInterval = _snapshotInterval;
  if  (snapshotThread)
    snapshotThread->SnapshotInterval (snapshotInterval);
}



void  CounterManager::Status (KKStr&    statusMsg,
                              KKStr&    color,
                              KKStr&    secondaryMsg,
                              float&    analogGain,
                              kkint32&  digitalGain,
                              kkint32&  cameraTemparature,
                              float&    flowRate
                             )
{
  goalie->StartBlock ();

  analogGain  = CurAnalogGain ();
  digitalGain = CurDigitalGain ();

  if  (acquisitionThread)
    cameraTemparature = acquisitionThread->CameraTemparature ();
 
  flowRate = flowMeter->FlowRateTrend ();
 
  KKStrPtr  nextMsg = secondaryMsgs.GetNextMsg ();
  if  (nextMsg)
  {
    secondaryMsg = *nextMsg;
    delete  nextMsg;
    nextMsg = nullptr;
  }
  else
  {
    secondaryMsg = "";
  }

  if  (curState == CounterState::PlayingBack)
  {
    statusMsg = "Playing Back";
    color = "White";
    goalie->EndBlock ();
    return;
  }

  else if  (curState == CounterState::DroppedFrames)
  {
    if  (this->operatingMode == CounterOperatingModes::User)
      statusMsg = "Finishing Up Counting";
    else
      statusMsg = "Processing Dropped Frames";
    color = "White";
    goalie->EndBlock ();
    return;
  }

  statusMsg = "";
  color = "Green";

  if  (WeAreRecordingToDisk ())
  {
    statusMsg = "Recording";
    color = "Green";
    goalie->EndBlock ();
    return;
  }

  statusMsg = CounterStateToStr (curState);

  KKThread::ThreadStatus               cameraStatus      = ThreadStatus::Null;
  CameraAcquisition::StartStatusType   cameraStartStatus = CameraAcquisition::StartStatusType::Null;
  DiskWriterThread::DiskWritingStatus  diskWritingStatus = DiskWriterThread::DiskWritingStatus::Null;
  KKStr  cameraMsg = "";

  if  (acquisitionThread)
  {
    cameraStatus      = acquisitionThread->Status      ();
    cameraStartStatus = acquisitionThread->StartStatus ();
    cameraMsg         = acquisitionThread->StatusMsg   ();
  }
  else
  {
    cameraStatus      = ThreadStatus::Null;
    cameraStartStatus = CameraAcquisition::StartStatusType::Disconnected;
    cameraMsg = "";
  }

  if  (diskWriterThread)
    diskWritingStatus = diskWriterThread->DiskStatus ();
  else
    diskWritingStatus = DiskWriterThread::DiskWritingStatus::Null;

  color = "White";

  if  (acquisitionThread == nullptr)
  {
    color = "White";
    statusMsg = "Not Connected";
    goalie->EndBlock ();
    return;
  }

  if  (cameraStatus == ThreadStatus::Running)
  {
    switch  (cameraStartStatus)
    {
    case  CameraAcquisition::StartStatusType::Connecting:
      color = "Orange";
      statusMsg = "Connecting";
      break;

    case  CameraAcquisition::StartStatusType::Connected:
      color = "White";
      statusMsg = "Connected";
      if  (curState == CounterState::Starting)
        statusMsg = "Starting";

      else if  (curState == CounterState::BuildingClassifier)
        statusMsg = "Building Classifier";
      break;

    case  CameraAcquisition::StartStatusType::ConnectionFailed:
      color = "Red";
      statusMsg = "Connection Error";
      break;

    case  CameraAcquisition::StartStatusType::Disconnected:
      color = "Orange";
      statusMsg = "Disconnected";
      break;

    case  CameraAcquisition::StartStatusType::Stoped:
      color = "White";
      statusMsg = "Stopped";
      break;
    }
  }

  goalie->EndBlock ();
}  /* Status */



bool   CounterManager::WeAreConnectedToCamera ()  const
{
  if  (!acquisitionThread)
    return false;
  else
    return  (acquisitionThread->StartStatus () == CameraAcquisition::StartStatusType::Connected);
}



bool   CounterManager::WeAreConnectingToCamera ()  const
{
  if  (!acquisitionThread)
    return false;
  else
    return  (acquisitionThread->StartStatus () == CameraAcquisition::StartStatusType::Connecting);
}



void  CounterManager::SetOperatingParameters (const OperatingParametersPtr  _operatingParameters)
{
  operatingParameters->Assign (*_operatingParameters);
}



void  CounterManager::SetDefaultOperatingParameters (const OperatingParametersPtr  _defaultOperatingParameters)
{
  defaultOperatingParameters->Assign (*_defaultOperatingParameters);
  SaveConfiguration ();
}



void  CounterManager::SetTrainingModel (const KKStr&            _trainingModelName,
                                        bool&                   _successful,
                                        OperatingParametersPtr  _opParms
                                       )
{
  goalie->StartBlock ();

  runLog->Level (40) << "CounterManager::SetTrainingModel   TrainingModelName: " << _trainingModelName << endl;

  CounterTrainingConfigurationPtr  config = nullptr;

  if  (_trainingModelName.EqualIgnoreCase ("PostLarvae")    ||  
       _trainingModelName.EqualIgnoreCase ("Post Larvae")   ||
       _trainingModelName.EqualIgnoreCase ("None")          ||
       _trainingModelName.Empty ()
      )
  {
    sessionParameters->TrainingModelName (_trainingModelName);
    _successful = true;

    //CropSettingsChanged (326, 1980);
    MinSizeThreshold (400);
 
    goalie->EndBlock ();
  }
  else
  {
    config = new CounterTrainingConfiguration ();
    config->Load (_trainingModelName, operatingParameters, false, *runLog);

    if  (!config->FormatGood ())
    {
      runLog->Level (-1) << "SetTrainingModel   ***ERROR***  Model: " << _trainingModelName << "  Invalid Format." << endl;
      _successful = false;
    }
    else
    {
      _successful = true;

      sessionParameters->TrainingModelName (_trainingModelName);
      if  (config->OperatingParms ())
      {
        operatingParameters->Assign (*config->OperatingParms ());
        MinSizeThreshold         (operatingParameters->MinSizeThreshold ());

        if  (operatingParameters->RequestedSensitivityMode () != operatingParameters->RequestedSensitivityMode ())
          RequestedSensitivityMode (operatingParameters->RequestedSensitivityMode ());
  
        if  (operatingParameters->RequestedScanRate () != operatingParameters->RequestedScanRate ())
           RequestedScanRate (operatingParameters->RequestedScanRate ());

        if  (operatingParameters->RequestedAnalogGain () != operatingParameters->RequestedAnalogGain ())
          RequestedAnalogGain (operatingParameters->RequestedAnalogGain ());
  
        if  (operatingParameters->RequestedDigitalGain () != operatingParameters->RequestedDigitalGain ())
          RequestedDigitalGain (operatingParameters->RequestedDigitalGain ());
      }
    }
  }

  if  (_successful  &&  operatingParameters  &&  _opParms)
  {
    _opParms->Assign (*operatingParameters);
  }

  delete  config;
  config = nullptr;

  goalie->EndBlock ();
}  /* SetTrainingModel */



void  CounterManager::SaveConfiguration ()
{
  goalie->StartBlock ();

  ofstream o (configurationFileName.Str ());
  if  (!o.is_open ())
  {
    runLog->Level (-1) << endl
      << "CounterManager::SaveConfiguration   ***ERROR***   Failed to write ConfiguratioFile" << endl
      << endl;
    goalie->EndBlock ();
    return;
  }

  o << "<CounterManager>" << endl
    << "// Counter Configuration"  << endl
    << "// DateTime" << "\t" << osGetLocalDateTime () << endl
    << endl
    << endl;

  o << "SrcScannerFileName"             << "\t" << srcScannerFileName                                  << endl;
  o << "NameLastRecordedFile"           << "\t" << nameLastRecordedFile                                << endl;
  o << "DropFolderToRemote"             << "\t" << dropFolderToRemote                                  << endl;
  o << "OperatingMode"                  << "\t" << CounterOperatingModeToStr (operatingMode)            << endl;
  o << "ThroughPutField"                << "\t" << StatusSnapshot::SnapShotFieldIdxToStr (throughPutField) << endl;
  o << "LastScannerFileCounted"         << "\t" << lastScannerFileCounted                              << endl;
  o << "LastControlNumber"              << "\t" << lastControlNumber                                   << endl;
  o << "CropLeft"                       << "\t" << cropLeft                                            << endl;
  o << "CropRight"                      << "\t" << cropRight                                           << endl;
  o << "CameraMacAddress"               << "\t" << CameraMacAddress ()                                 << endl;
  o << "SampleLastFrameBeforeFlatField" << "\t" << (sampleLastFrameBeforeFlatField ? "y" : "n")        << endl;
  o << "SnapshotInterval"               << "\t" << snapshotInterval                                    << endl;
  o << "PlayingBackRealTime"            << "\t" << (playingBackRealTime ? "Y" : "N")                   << endl;
  sessionParameters->WriteXML (o);
  defaultOperatingParameters->WriteXML (o);
  o << "</CounterManager>"        << endl;
  o.close ();

  goalie->EndBlock ();
  return;
}  /* SaveConfiguration */



void  CounterManager::ReadConfiguration ()
{
  goalie->StartBlock ();

  ifstream i (configurationFileName.Str ());
  if  (!i.is_open ())
  {
    runLog->Level (-1) << endl
      << "CounterManager::ReadConfiguration   ***ERROR***      Failed to Read ConfiguratioFile" << endl
      << endl;
    goalie->EndBlock ();
    return;
  }

  bool  eol = false;
  bool  eof = false;

  while (!eof)
  {
    KKStr  fieldName = osReadNextToken (i, "\t", eof, eol);
    if  (eof)
      break;

    if  (fieldName.EqualIgnoreCase ("</CounterManager>"))
      break;

    if  (eol)
      continue;

    if  (fieldName.StartsWith ("//"))
    {
      KKStrPtr restOfLine = osReadRestOfLine (i, eof);
      delete  restOfLine;
      restOfLine = nullptr;
    }

    else
    {
      KKStrPtr  restOfLine = osReadRestOfLine (i, eof);
      restOfLine->TrimRight ("\r\n\t ");

      if  (fieldName.EqualIgnoreCase ("<SessionParameters>"))
        sessionParameters->ReadXML (i);

      else if  
        (fieldName.EqualIgnoreCase ("<OperatingParameters>")  || 
         fieldName.EqualIgnoreCase ("<TFTCOperatingParameters>") ||
         fieldName.EqualIgnoreCase ("<LarcosOperatingParameters>"))
        defaultOperatingParameters->ReadXML (i);

      else if  (fieldName.EqualIgnoreCase ("DropFolderToRemote"))
        dropFolderToRemote = *restOfLine;

      else if  (fieldName.EqualIgnoreCase ("OperatingMode"))
      {
        operatingMode = CounterOperatingModeFromStr (*restOfLine);
        if  ((operatingMode == CounterOperatingModes::Null)  ||  (operatingMode == CounterOperatingModes::Invalid))
          operatingMode = CounterOperatingModes::User;
      }

      else if  (fieldName.EqualIgnoreCase ("ThroughPutField"))
        throughPutField = StatusSnapshot::SnapShotFieldIdxFromStr (*restOfLine);

      else if  (fieldName.EqualIgnoreCase ("SrcScannerFileName"))
        srcScannerFileName = *restOfLine;

      else if  (fieldName.EqualIgnoreCase ("NameLastRecordedFile"))
        nameLastRecordedFile = *restOfLine;

      else if  (fieldName.EqualIgnoreCase ("LastControlNumber"))
        lastControlNumber = *restOfLine;

      else if  (fieldName.EqualIgnoreCase ("LastScannerFileCounted"))
        lastScannerFileCounted = *restOfLine;

      else if  (fieldName.EqualIgnoreCase ("CropLeft"))
        cropLeft = restOfLine->ToInt32 ();

      else if  (fieldName.EqualIgnoreCase ("CropRight"))
        cropRight = restOfLine->ToInt32 ();

      else if  (fieldName.EqualIgnoreCase ("CameraMacAddress"))
        cameraMacAddress = *restOfLine;

      else if  (fieldName.EqualIgnoreCase ("SampleLastFrameBeforeFlatField"))
        sampleLastFrameBeforeFlatField = restOfLine->ToBool ();

      else if  (fieldName.EqualIgnoreCase ("SnapshotInterval"))
        snapshotInterval = restOfLine->ToInt32 ();

      else if  (fieldName.EqualIgnoreCase ("PlayingBackRealTime"))
        playingBackRealTime = restOfLine->ToBool ();

      delete restOfLine;
      restOfLine = nullptr;
    }
  }

  i.close ();

  const KKStr&  trainModelName = sessionParameters->TrainingModelName ();

  operatingParameters->Assign (*defaultOperatingParameters);

  if  (!trainModelName.Empty ())
  {
    // retrieve any parameters specified in the default training model.
    CounterTrainingConfigurationPtr  config = new CounterTrainingConfiguration ();
    config->Load (trainModelName, operatingParameters, false, *runLog);
    if  (!config->FormatGood ())
    {
      runLog->Level (-1) << "ReadConfiguration   ***ERROR***  Default Training Model: '" << trainModelName << "'  is Invalid." << endl;
    }
    else
    {
      operatingParameters->Assign (*(config->OperatingParms ()));
    }
    delete  config;
    config = nullptr;
  }

  if  ((operatingMode != CounterOperatingModes::Advanced)  ||  
       ((int)throughPutField < 0)  ||  
       (throughPutField >= StatusSnapshot::FieldIdx::Invalid)
      )
    throughPutField = StatusSnapshot::FieldIdx::Count;

  if  (snapshotInterval < 1)
    snapshotInterval = 5;

  else if  (snapshotInterval > 900)
    snapshotInterval = 5;

  if  (flowMeter)
    flowMeter->FlowRateRatioDefaultChanged (defaultOperatingParameters->FlowRateFactor ());

  goalie->EndBlock ();

}  /* ReadConfiguration */
