/* LarcosCounterManager.h
 * Copyright (C) 2011-2013  Kurt Kramer
 * For conditions of distribution and use, see copyright notice in LarcosCounterUnManaged.txt
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
#include "LarcosFeatureVector.h"
#include "LarcosFVProducer.h"
using namespace KKMachineLearning;

#include "LarcosVariables.h"
using namespace  LarcosBase;


#include "LarcosCounterManager.h"


#include "DiskWriterThread.h"

#include "CameraFrameBuffer.h"
#include "CameraAcquisitionPleora.h"
#include "CameraAcquisitionSimulator.h"
#include "CameraAutoGainThread.h"
#include "CameraParameters.h"
#include "ConnectButtonThread.h"
#include "InstallationConfig.h"
#include "LarcosTrainingConfiguration.h"
#include "LoggerThread.h"
#include "LogicalFrameEntry.h"
#include "LogicalFrameBuilder.h"
#include "LogicalFrameProcessor.h"
#include "LarcosCounterStats.h"
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


using  namespace  LarcosCounterUnManaged;


#define  MemoryWillingToAllocateToCameraFrameBuffers 251658240


GoalKeeperPtr  LarcosCounterManager::goalie = NULL;

KKStr  LarcosCounterManager::CounterStateToStr (CounterState  state)
{
  switch  (state)
  {
  case  csNULL:               return "NULL";
  case  csStopped:            return "Stopped";
  case  csStopping:           return "Stopping";
  case  csConnected:          return "Connected";
  case  csConnecting:         return "Connecting";
  case  csStarting:           return "Starting";
  case  csBuildingClassifier: return "BuildingClassifier";
  case  csRunning:            return "Running";
  case  csPlayingBack:        return "PlayingBack";
  case  csDroppedFrames:      return "DroppedFrames";
  }
  return  "NULL";
}


const  KKStr  LarcosCounterManager::larcosOperatingModeStrs[] = {"NULL", "User", "Advanced", "Invalid"};


const KKStr&  LarcosCounterManager::LarcosOperatingModeToStr (LarcosOperatingModes om)
{
  if  (((int)om < 0)  ||  (om >= (int)lomInvalid))
    return  larcosOperatingModeStrs[lomInvalid];
  else
    return  larcosOperatingModeStrs[om];
}



LarcosCounterManager::LarcosOperatingModes  LarcosCounterManager::LarcosOperatingModeFromStr (const KKStr&  s)
{
  for  (kkint32  x = 0;  x < (kkint32)lomInvalid;  ++x)
  {
    if  (s.EqualIgnoreCase (larcosOperatingModeStrs[x]))
      return  (LarcosOperatingModes)x;
  }

  return  lomNULL;
}





LarcosCounterManager::LarcosCounterManager (MsgQueuePtr  _msgQueue,
                                            MsgQueuePtr  _loggedMsgs,
                                            int          _maxNumOfThreads
                                           ):
    configurationFileName          (),
    curState                       (csStopped),
    operatingMode                  (lomUser),
    throughPutField                (StatusSnapshot::dfiCount),
    secondaryMsgs                  ("SecondaryMsgs"),

    installation                   (NULL),

    maxNumOfThreads                (_maxNumOfThreads),
    allThreads                     (NULL),
    acquisitionThread              (NULL),
    cameraAutoGainThread           (NULL),
    connectButtonThread            (NULL),
    diskWriterThread               (NULL),
    frameProcessors                (NULL),
    reportWriterThread             (NULL),
    loggerThread                   (NULL),
    logicalFrameBuilderThread      (NULL),
    playBackButtonThread           (NULL),
    startButtonThread              (NULL),
    snapshotThread                 (NULL),
    stopButtonThread               (NULL),

    cameraMacAddress               (),

    headerFields                   (NULL),

    cropLeft                       (0),
    cropRight                      (2047),

    frameHeightMax                 (14000),
    logicalFrameQueueSizeMax       (10),

    liveVideoHeight                (848),
    liveVideoWidth                 (400),

    playingBackRealTime            (true),
    sampleLastFrameBeforeFlatField (false),

    snapshotBuffer                 (NULL),
    snapshotInterval               (5),

    flowMeter                      (NULL),
    cameraFrameBuffer              (NULL),
    classifier                     (NULL),
    lastParticlesProcessed         (NULL),
    logicalFrames                  (NULL),
    logicalFrameEntries            (NULL),
    droppedFrames                  (NULL),
    particleEntryBuffer            (NULL),
    finaleStats                    (NULL),
    totalBytesToRead               (0),
    totalBytesRead                 (0),
    trainer                        (NULL),
    trainerCancelFlag              (false),
    trainerStatusMsg               (),
    memoryPreasure                 (0),
    destScannerFileName            (),
    destScannerFileDir             (),
    destScannerFileRootName        (),
    lastControlNumber              (),
    srcScannerFileName             (),
    srcScannerFileFormat           (ScannerFile::sfUnKnown),
    particlesDirName               (),
    sessionParameters              (NULL),
    operatingParameters            (NULL),
    defaultOperatingParameters     (NULL),
    cameraBuffSizeMax              (256),
    cameraFrameHeight              (480),
    cameraFrameWidth               (2048),
    stoppingIsInProgress           (false),
    generateFinaleReport           (false),
    msgQueue                       (_msgQueue),
    loggedMsgs                     (_loggedMsgs),
    runLog                         (NULL)
{
  ValidateLarcosInstallation ();

  configurationFileName = osAddSlash (LarcosVariables::ConfigurationDir ()) + "LarcosCounterManager.txt";

  loggerThread = LoggerThread::CreateAndStartInstance (msgQueue, loggedMsgs, "LoggerThread");
  runLog = new RunLog (msgQueue);
  runLog->SetLevel (LarcosVariables::DebugLevel ());
  runLog->Level (40) << "LarcosCounterManager::LarcosCounterManager   MaxNumThreads: " << _maxNumOfThreads << endl;

  allThreads                 = new CameraThreadList (true);
  frameProcessors            = new LogicalFrameProcessorList (false);
  sessionParameters          = new SessionParameters ();
  operatingParameters        = new OperatingParameters ();
  defaultOperatingParameters = new OperatingParameters ();
  headerFields               = new ScannerHeaderFields ();

  InitializeFlowMeterTracker ();

  CreateGoalie ();
}

 

LarcosCounterManager::~LarcosCounterManager ()
{
  SaveConfiguration ();
  runLog->Level (40) << "LarcosCounterManager::~LarcosCounterManager" << endl;
  CleanUpMemory ();
}




void  LarcosCounterManager::Initialize ()
{
  // 1) Get Camera mac Address is order of:
  //    a. Use last one used from "LarcosCounterManager" configuration file.
  //    b. Installation configuraion.
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
      CameraParametersListPtr  cameras = LarcosCounterManager::GetCameraList (msgQueue);
      if  ((cameras != NULL)  &&  (cameras->QueueSize () == 1))
        cameraMacAddress = (*cameras)[0].MacAddress ();
      delete  cameras;
      cameras = NULL;
    }
  }

  if  (!cameraMacAddress.Empty ())
    ConnectButtonPressed ();

  if  (!sessionParameters->TrainingModelName ().Empty ())
  {
    bool  successful = false;
    SetTrainingModel (sessionParameters->TrainingModelName (), successful, NULL);
  }
}  /* Initialize  */




KKStr  LarcosCounterManager::LarcosVersionNumber ()
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
  verNum << _SVNVERSION_WITH_QUOTES_;

  return  verNum;
}




void  LarcosCounterManager::CleanUpMemory ()
{
  if  (allThreads)
  {
    bool  allThreadsStopped = false;
    allThreads->SendTerminateCmdToAllThreads ();
    allThreads->WaitForAllThreadsToStop (10.0f, allThreadsStopped);
  }

  // 'allThreads'  owns 'acquisitionThread', 'diskWriterThread', 'logicalFrameBuilderThread' and 'frameProcessors' contents.

  delete  allThreads;                  allThreads                 = NULL;
                                       acquisitionThread          = NULL;
                                       cameraAutoGainThread       = NULL;
                                       connectButtonThread        = NULL;
                                       diskWriterThread           = NULL;
                                       logicalFrameBuilderThread  = NULL;
                                       reportWriterThread         = NULL;
                                       startButtonThread          = NULL;
                                       playBackButtonThread       = NULL;
                                       stopButtonThread           = NULL;
                                       snapshotThread             = NULL;

  delete  flowMeter;                   flowMeter                  = NULL;
  delete  frameProcessors;             frameProcessors            = NULL;
  delete  cameraFrameBuffer;           cameraFrameBuffer          = NULL;
  delete  logicalFrames;               logicalFrames              = NULL;
  delete  logicalFrameEntries;         logicalFrameEntries        = NULL;
  delete  droppedFrames;               droppedFrames              = NULL;
  delete  lastParticlesProcessed;      lastParticlesProcessed     = NULL;
  delete  particleEntryBuffer;         particleEntryBuffer        = NULL;
  delete  snapshotBuffer;              snapshotBuffer             = NULL;
  delete  trainer;                     trainer                    = NULL;
  delete  classifier;                  classifier                 = NULL;
  delete  headerFields;                headerFields               = NULL;
  delete  runLog;                      runLog                     = NULL;
  delete  finaleStats;                 finaleStats                = NULL;

  delete  sessionParameters;           sessionParameters          = NULL;
  delete  operatingParameters;         operatingParameters        = NULL;
  delete  defaultOperatingParameters;  defaultOperatingParameters = NULL;

  delete  installation;                installation               = NULL;

  // loggerThread is not derived from CameraThread and is not managed by 'allThreads' ;  we want to make this the last thread to be deleted.
  delete  loggerThread;                loggerThread               = NULL; 
}  /* CleanUpMemory */





void  LarcosCounterManager::ValidateLarcosInstallation ()
{
  if  (!osValidDirectory (LarcosVariables::LarcosHomeDir ()))
    osCreateDirectoryPath (LarcosVariables::LarcosHomeDir ());

  if  (!osValidDirectory (LarcosVariables::ConfigurationDir ()))
    osCreateDirectoryPath (LarcosVariables::ConfigurationDir ());

  bool  successful = false;

  delete  installation;
  RunLog  runLog;
  installation = new InstallationConfig ("ModelA", successful, runLog);
  if  (!successful)
    installation->Save (runLog);
}  /* ValidateLarcosInstallation */




float  LarcosCounterManager::ImagingChamberWidth ()  const
{
  if  (installation)
    return  installation->ImagingChamberWidth ();
  else
    return  0.0f;
}



kkint32  LarcosCounterManager::ImagingChamberWidthPixels ()  const
{
  if  (installation)
    return  installation->ImagingChamberWidthPixels ();
  else
    return  0;
}



float  LarcosCounterManager::ImagingChamberPixelsPerMM ()  const
{
  if  ((!installation)  ||  (installation->ImagingChamberWidth () == 0.0f))
    return 0.0f;
  else
    return  ((float)(installation->ImagingChamberWidthPixels ()) / installation->ImagingChamberWidth ()) / 1000.0f;
}



void  LarcosCounterManager::InitializeFlowMeterTracker ()
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



kkint64  LarcosCounterManager::MemoryConsumedEstimated ()
{
  kkint32  memoryConsumed =  sizeof (LarcosCounterManager);

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



void  LarcosCounterManager::CreateGoalie ()
{
  bool didNotExist = false;
  GoalKeeper::CreateAndStartBlock ("LarcosCounterManager", goalie, didNotExist);
  if  (didNotExist)
     atexit (LarcosCounterManager::CleanUp);
  goalie->EndBlock ();
}



void  LarcosCounterManager::CleanUp ()
{
  GoalKeeper::Destroy (goalie);
  goalie = NULL;
}


void  LarcosCounterManager::AddMsg (KKStrPtr  msg)
{
  if  (msgQueue == NULL)
  {
    cerr << endl 
         << "LarcosCounterManager::AddMsg   ***ERROR***    msgQuue is not defined."  << endl
         << "                               Msg[" << *msg << "]." << endl
         << endl;
    delete  msg;
    msg = NULL;
  }
  else
  {
    KKStrPtr  s = new KKStr (msg->Len () + 22);
    *s << osGetThreadId () << " - " << osGetLocalDateTime ().Time () << "->" << (*msg);
    msgQueue->AddMsg (s);
    delete  msg;
    msg = NULL;
  }
}  /* AddMsg */




void  LarcosCounterManager::AddMsg (const KKStr&  msg)
{
  if  (msgQueue == NULL)
  {
    cerr << endl 
         << "LarcosCounterManager::AddMsg   ***ERROR***    msgQuue is not defined."  << endl
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



KKStrListPtr  LarcosCounterManager::GetAllLoggedMsgs ()
{
  if  (loggedMsgs)
    return loggedMsgs->GetAllMsgs ();
  else
    return new KKStrList ();
}



void   LarcosCounterManager::ControlNumValidityCheck (const KKStr&  controlNum,
                                                      KKStr&        errMsg
                                                     )
{
  const char* validChars = "ABCDEFGHIJKLMNOPQRSTUVWXRZabcdefghijklmnopqrstuvwxyz0123456789_-";

  kkint32 len = controlNum.Len ();
  for  (int idx = 0;  idx < len;  ++idx)
  {
    char c = controlNum[idx];
    if  (KKStr::StrChr (validChars, c) == NULL)
       errMsg << "\n" << "ControlNum Invalid Character[" << c << "]";
  }
}  /* ControlNumValidityCheck */




KKStr  LarcosCounterManager::GetNextControlNumber ()
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

    KKStr  firstPart = lastControlNumber.SubStrPart (0, x - 1);
    if  (firstPart.LastChar () == '_')
      firstPart.ChopLastChar ();

    nextControlNumber = firstPart + "_" + StrFormatInt ((lastSeqNum + 1), "ZZ000");
  }

  return nextControlNumber;
}




void  LarcosCounterManager::ValidateCommonParameters (SessionParametersPtr    _sessionParameters,
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



CameraParametersListPtr  LarcosCounterManager::GetCameraList (MsgQueuePtr  _msgQueue)
{
  return  CameraAcquisitionPleora::GetCameraList (_msgQueue);
}


CameraParametersPtr  LarcosCounterManager::PromptForCamera (RunLog&  runLog)
{
  runLog.Level (10) << "LarcosCounterManager::PromptForCamera" << endl;
  CreateGoalie ();
  goalie->StartBlock ();
  CameraParametersPtr  result = NULL;

  KKStr  macAddress = CameraAcquisitionPleora::PromptForCameraMacAddress ();
  if  (macAddress.Empty ())
  {
    runLog.Level (10) << "LarcosCounterManager::PromptForCamera   No Camera Selected." << endl;
    result = NULL;
  }
  else
  {
    result = CameraAcquisitionPleora::GetCameraParameters (macAddress, NULL, runLog);
    runLog.Level (10) << "LarcosCounterManager::PromptForCamera   MacAddress :" << macAddress.QuotedStr () << " selected." << endl;
  }

  goalie->EndBlock ();

  return  result;
}  /* PromptForCamera */



bool  LarcosCounterManager::IsRecording () const
{
  bool  isRecording = false;
  goalie->StartBlock ();

  if  (diskWriterThread)
    isRecording = diskWriterThread->DiskStatus () == DiskWriterThread::dwRecording;
  
  goalie->EndBlock ();
  return  isRecording;
}



bool  LarcosCounterManager::IsRunning () const
{
  bool  isRunning = false;
  goalie->StartBlock ();

  isRunning = ((curState == LarcosCounterManager::csDroppedFrames)  ||
               (curState == LarcosCounterManager::csRunning)        ||
               (curState == LarcosCounterManager::csPlayingBack)
              );

  goalie->EndBlock ();
  return  isRunning;
}





void  LarcosCounterManager::AutoGainButtonPressed (bool&   _successful,
                                                   KKStr&  _errMsg
                                                  )
{
  runLog->Level (40) << "LarcosCounterManager::AutoGainButtonPressed" << endl;
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
  cameraAutoGainThread->Start (KKThread::tpNormal, _successful);

  goalie->EndBlock ();
}



void  LarcosCounterManager::ConnectButtonPressed ()
{
  runLog->Level (40) << "LarcosCounterManager::ConnectButtonPressed" << endl;

  goalie->StartBlock ();

  if  (curState == LarcosCounterManager::csStopped)
  {
    DeleteConnectButtonThread ();
    connectButtonThread = new ConnectButtonThread (this, msgQueue,  "ConnectButtonThread");
    allThreads->PushOnBack (connectButtonThread);
    bool  successful = false;
    runLog->Level (40) << "ConnectButtonPressed   Starting 'ConnectButtonThread' Thread." << endl;
    connectButtonThread->Start (KKThread::tpNormal, successful);
  }
  goalie->EndBlock ();
}




void  LarcosCounterManager::ConnectToCamera (bool&  _successful)
{
  _successful = false;

  runLog->Level (10) << "LarcosCounterManager::ConnectToCamera   CameraMacAddress: " << cameraMacAddress << endl;

  if  (curState != csStopped)
  {
    _successful = false;
    runLog->Level (10) << "LarcosCounterManager::ConnectToCamera   We need to be in the Stooped-State before we can connect to camera." << endl;
    return;
  }

  goalie->StartBlock ();
  curState = csConnecting;
  
  TerminateAndDeleteAllButConnectButtonThread ();

  delete  cameraFrameBuffer;
  cameraFrameBuffer = NULL;

  goalie->EndBlock ();

  KKStr  macAddress = "";

  CameraParametersPtr  cameraParameters = NULL;

  if  (cameraMacAddress.Empty ())
  {
    runLog->Level (10) << "ConnectToCamera   Prompting For Camera" << endl;
    secondaryMsgs.AddMsg ("Prompting For Camera");
    cameraMacAddress = CameraAcquisitionPleora::PromptForCameraMacAddress ();
    if  (cameraMacAddress.Empty ())
    {
      runLog->Level (10) << "ConnectToCamera   No camera selected." << endl;
      curState = csStopped;
      _successful = false;
      //goalie->EndBlock ();
      return;
    }
  }

  runLog->Level (20) << "ConnectToCamera   Retrieving camera parameters from: " << cameraMacAddress << endl;
  secondaryMsgs.AddMsg ("Retrieving Camera Parameters.");
  
  delete  cameraParameters;
  cameraParameters = NULL;
  cameraParameters = CameraAcquisitionPleora::GetDeviceInfo (cameraMacAddress, 'M', *runLog);
  if  (cameraParameters)
  {
    macAddress = cameraParameters->MacAddress ();
    cameraFrameHeight = cameraParameters->FrameHeight ();
    cameraFrameWidth  = cameraParameters->FrameWidth ();
  }
  else
  {
    runLog->Level (10) << "ConnectToCamera   Camera not avaialble." << endl;
    secondaryMsgs.AddMsg ("Camera not Connected!!!");
    curState = csStopped;
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

  runLog->Level (10) << "ConnectToCamera   Starting Acquision Thread" << endl;
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
    curState = csConnected;
  }
  else
  {
    curState = csStopped;
    runLog->Level (-1) << "ConnectToCamera   Could not connect to camera." << endl;
  }


  SaveConfiguration ();

  //goalie->EndBlock ();
  return;
}  /* ConnectToCamera*/



void  LarcosCounterManager::BuildDestScannerFileName (bool playingBackExistingFile)
{
  KKStr  controlNum = sessionParameters->ControlNum ();

  DateTime  dt = osGetLocalDateTime ();
  KKStr  dateStr = StrFormatInt (dt.Year (), "0000") + "-" + StrFormatInt (dt.Month (), "00") + "-" + StrFormatInt (dt.Day (), "00");

  if  (playingBackExistingFile)
  {
    KKStr  srcFileRootName = osGetRootName (srcScannerFileName);
    destScannerFileDir = osGetPathPartOfFile (srcScannerFileName);
    KKStr  baseRootName = srcFileRootName;
    if  (baseRootName.Find (controlNum, 0) < 0)
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
      kkint32  seqNum = 0;
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
    destScannerFileDir      = osAddSlash (LarcosVariables::ScannerFilesDefaultDir ()) + dateStr + "_" + controlNum;
    destScannerFileRootName = controlNum + "_" + dt.Date ().YYYYMMDD () + "-" + dt.Time ().HHMMSS ();
    destScannerFileName     = osAddSlash (destScannerFileDir) + destScannerFileRootName + ".lsc";
    particlesDirName        = osAddSlash (destScannerFileDir) + destScannerFileRootName;
  }

  if  (operatingParameters->DataIsToBeRecorded ())
    osCreateDirectoryPath (destScannerFileDir);

  if  (operatingParameters->SaveParticleImages ())
    osCreateDirectoryPath (particlesDirName);
}  /* BuildDestScannerFileName */





void  LarcosCounterManager::AddHeaderFields ()
{
  if  (operatingParameters->DataIsToBeRecorded ())
  {
    sessionParameters->DestScannerFileName (destScannerFileName);
    AddHeaderField ("DestScannerFileDir",       destScannerFileDir);
    AddHeaderField ("DestScannerFileRootName",  destScannerFileRootName);
  }

  sessionParameters->DateTimeRecorded (osGetLocalDateTime ());
  sessionParameters->HostName (osGetHostName ());

  sessionParameters->AddToHeaderFields (headerFields);
  operatingParameters->AddToHeaderFields (headerFields);

  AddHeaderField ("ParticlesDirName", particlesDirName);
  AddHeaderField ("CropLeft",         cropLeft);
  AddHeaderField ("CropRight",        cropRight);

  if  (installation)
    installation->AddToHeaderFields (HeaderFields ());
}  /* AddHeaderFields */



void  LarcosCounterManager::ValidateTrainingModel (const KKStr&  trainingModelName,
                                                   bool&         successful,
                                                   KKStr&        errMsg
                                                  )

{
  runLog->Level (10) << "LarcosCounterManager::ValidateTrainingModel  Model: " + trainingModelName << endl;

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
    // KKKK  Building Training Model
    //

    CounterState  origState = curState;
    curState = this->csBuildingClassifier;

    if  (operatingMode == lomAdvanced)
      secondaryMsgs.AddMsg ("Validating Training Model");
    trainerCancelFlag = false;
    trainer = new TrainingProcess2 (trainingModelName,
                                    NULL,                // _excludeList,
                                    LarcosFVProducerFactory::Factory (runLog),
                                    *runLog,
                                    NULL,               // _report,
                                    false,              // _forceRebuild,
                                    true,               // _checkForDuplicates,
                                    trainerCancelFlag,
                                    trainerStatusMsg
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
      trainer = NULL;
      secondaryMsgs.AddMsg ("Error Building Classifier");
    }
    else
    {
      successful = true;
      runLog->Level (40) << "ValidateTrainingModel   Valid Format." << endl;
    }

    delete  trainer;
    trainer = NULL;

    curState = origState;
  }

}  /* ValidateTrainingModel */




void  LarcosCounterManager::StartRecordingAndOrCounting (bool&   _successful,
                                                         KKStr&  _errMsg
                                                        )
{
  goalie->StartBlock ();

  if  (LarcosVariables::DebugLevel () >= 40)
    runLog->Level (40) << "LarcosCounterManager::StartRecordingAndOrCounting "      << endl
                       << "  trainingModelName      : " << TrainingModelName ()     << endl
                       << "  controlNum             : " << ControlNum ()            << endl
                       << "  description            : " << Description ()           << endl;
  operatingParameters->WriteToRunLog ("  OP:", *runLog);

  _successful = true;
  CounterState  origState = curState;

  operatingParameters->PlayingBack (false);

  if  (curState != csConnected)
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

  curState = csStarting;
  SaveConfiguration ();

  DeleteAllStoppedThreads ();

  TerminateAndDeleteAllButCameraAndStartButtonThreads ();

  ResetCounts ();

  delete  classifier;           classifier          = NULL;
  delete  trainer;              trainer             = NULL;
  delete  logicalFrames;        logicalFrames       = NULL;
  delete  logicalFrameEntries;  logicalFrameEntries = NULL;
  delete  droppedFrames;        droppedFrames       = NULL;
  delete  particleEntryBuffer;  particleEntryBuffer = NULL;
  goalie->EndBlock ();

  BuildDestScannerFileName (false);  // false = We are NOT playng back an existing Scanner-File.

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
    curState = csRunning;
    secondaryMsgs.AddMsg ("Larcos Started Successfully.");
  }
  else
  {
    runLog->Level (-1) << "StartRecordingAndOrCounting   ***ERROR***  Failed to Start!!!" << endl;
    secondaryMsgs.AddMsg ("Failed to Start!!!");

    TerminateAndDeleteAllButCameraAndStartButtonThreads ();
    if  (WeAreConnectedToCamera ())
      curState = csConnected;

    else if  (WeAreConnectingToCamera ())
      curState = csConnecting;

    else
      curState = csStopped;
  }

  SaveConfiguration ();

  //goalie->EndBlock ();
}  /* StartRecordingAndOrCounting */




void  LarcosCounterManager::RecordButtonPressed 
                      (SessionParametersPtr     _sessionParameters,
                       OperatingParametersPtr   _operatingParameters,
                       bool&                    _successful,
                       KKStr&                   _errMsg
                      )
{
  goalie->StartBlock ();
  
  _operatingParameters->PlayingBack (false);

  if  (LarcosVariables::DebugLevel () >= 40)
  {
    runLog->Level (40) << "LarcosCounterManager::RecordButtonPressed  "      << endl
                       << "  trainingModelName      : " << TrainingModelName ()     << endl
                       << "  controlNum             : " << ControlNum ()            << endl
                       << "  description            : " << Description ()           << endl;

    _operatingParameters->WriteToRunLog ("  OP:", *runLog);
  }

  _successful = true;

  _errMsg = "";

  if  (curState != csConnected)
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

    startButtonThread->Start (KKThread::tpNormal, _successful);
  }

  goalie->EndBlock ();
}  /* RecordButtonPressed */





void  LarcosCounterManager::PlayBackScannerFile (const KKStr&  _srcScannerFileName,
                                                 bool&         _successful,
                                                 KKStr&        _errMsg
                                                )
{
  goalie->StartBlock ();

  operatingParameters->PlayingBack (true);

  if  (LarcosVariables::DebugLevel () >= 40)
  {
    runLog->Level (40) << "LarcosCounterManager::PlayBackScannerFile  "      << endl
                       << "  srcScannerFileName     : " << _srcScannerFileName      << endl
                       << "  trainingModelName      : " <<  TrainingModelName ()    << endl
                       << "  controlNum             : " << ControlNum ()            << endl
                       << "  description            : " << Description ()           << endl;
    operatingParameters->WriteToRunLog ("  OP:", *runLog);
  }

  _successful = true;

  CounterState  origState = curState;

  DeleteAllStoppedThreads ();

  if  ((curState == csStopped)  ||  (curState == csConnected))
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

  curState = csStarting;

  TerminateAndDeleteAllButPlayBackButtonThread ();

  ResetCounts ();

  srcScannerFileName     = _srcScannerFileName;
  srcScannerFileFormat   = ScannerFile::sfUnKnown;

  BuildDestScannerFileName (true);

  if  (operatingParameters->DataIsToBeRecorded ())
  {
    if  (operatingParameters->DestScannerFileFormat () == ScannerFile::sfUnKnown)
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

  delete trainer;             trainer             = NULL;
  delete classifier;          classifier          = NULL;
  delete cameraFrameBuffer;   cameraFrameBuffer   = NULL;
  delete logicalFrames;       logicalFrames       = NULL;
  delete logicalFrameEntries; logicalFrameEntries = NULL;
  delete droppedFrames;       droppedFrames       = NULL;
  delete particleEntryBuffer; particleEntryBuffer = NULL;

  float scanRate = 0.0f;

  ScannerHeaderFieldsPtr scannerFileHeaderFields = NULL;

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
    scannerFileHeaderFields = NULL;
    goalie->EndBlock ();
    return;
  }

  //if  ((scannerFileHeaderFields != NULL)  &&  scannerFileHeaderFields->FieldExists ("Installation:Name"))
  //{
  //  // Since the specified Scanner-File has 'Installation' paraeters defined we will build a new instance 
  //  // of 'installation' from these header fields.
  //  delete  installation;
  //  installation = new InstallationConfig (scannerFileHeaderFields, *runLog);
  //  delete  scannerFileHeaderFields;
  //  scannerFileHeaderFields = NULL;
  //}

  delete  scannerFileHeaderFields;
  scannerFileHeaderFields = NULL;

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
                         "LarcosCounterManager",
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
  // is set to csStarting so we should not be able to have any conflicts during rest of this method 
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
    curState = csStopped;
  }
  else
  {
    curState = csPlayingBack;
    runLog->Level (40) << "PlayBackScannerFile   Successfuly started." << endl;
    secondaryMsgs.AddMsg ("Play-Back Successfully Started.");
  }

  SaveConfiguration ();

  //goalie->EndBlock ();
  return;
}  /* PlayBackScannerFile*/




void  LarcosCounterManager::PlayBackButtonPressed 
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

  if  (LarcosVariables::DebugLevel () >= 40)
  {
    runLog->Level (40) << "LarcosCounterManager::PlayBackButtonPressed  "       << endl
                       << "  srcScannerFileName     : " << _srcScannerFileName  << endl
                       << "  trainingModelName      : " << TrainingModelName () << endl
                       << "  controlNum             : " << ControlNum ()        << endl
                       << "  description            : " << Description ()       << endl;
    _operatingParameters->WriteToRunLog ("  OP:", *runLog);
  }

  _errMsg = "";

  if  ((curState != csConnected)  &&  (curState != this->csStopped))
  {
    _errMsg = "Can only start PlayBack when in Connected ot Stopped state!";
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

    playBackButtonThread->Start (KKThread::tpNormal, _successful);
  }

  runLog->Level (40) << "PlayBackButtonPressed   Exiting." << endl;

  goalie->EndBlock ();
}  /* PlayBackButtonPressed */




bool  LarcosCounterManager::OkToPressStop (KKStr&  errMsg)
{
  bool  okToPressStop = false;
  errMsg = "";
  if  ((stopButtonThread != NULL)  &&  (stopButtonThread->Status () != CameraThread::tsStopped))
  {
    errMsg = "Larcos is already Stopping.";
  }

  else if  (startButtonThread  &&  startButtonThread->ThreadStillProcessing ())
  {
    errMsg = "Larcos is still starting up;  wait for it to stop before pressing 'Stop'";
  }


  {
    switch  (curState)
    {
      case csStarting:
      case csBuildingClassifier:
        errMsg = "Larcos is still starting up;  wait for it to stop before pressing 'Stop'";
        break;

      case csStopping:
        errMsg = "Larcos is already 'Stopping'.";
        break;

      case  csDroppedFrames:
        errMsg = "Larcos is processing dropped frames.";
        break;

      case csConnected:
      case csConnecting:
      {
        errMsg = "Larcos is Not Running.";
        break;
      }

      case csRunning:
      case csPlayingBack:
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




void  LarcosCounterManager::StopButtonPressed (bool    _stopImmediately,
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

    stopButtonThread->Start (KKThread::tpNormal, _successful);
    if  (!_successful)
      _errMsg = "Could not start the STOP process.";
  }

  goalie->EndBlock ();
}  /* StopButtonPressed */




/**
 *@brief  Will stop grabbing data from the camera and finish counting what in in the buffers.
 */
void  LarcosCounterManager::CloseOutCountingAndOrRecording (VolConstBool&  terminateFlag,
                                                            bool           stopImmediately
                                                           )
{
  goalie->StartBlock ();

  runLog->Level (40) << "LarcosCounterManager::CloseOutCountingAndOrRecording   stopImmediately: " << stopImmediately << endl;

  secondaryMsgs.AddMsg ("Stopping Larcos.");

  // In case the StartButton thread is still running because it is building a classifier, especially 
  // a new one with lots of examples; We need to set the cancel flag so tha the training process 
  // stops asap.
  trainerCancelFlag = true;

  if  ((curState != csRunning)  &&  (curState != csPlayingBack))
  {
    runLog->Level (10) << "CloseOutCountingAndOrRecording   We are not Running or playimg back a recorded file." << endl;
    goalie->EndBlock ();
    return;
  }

  // We will first let the 'CameraAcquision' thread know that it does not need to
  // save data to disk anymore.  Then we will shutdown the other threads after they are finished
  // processing what is in the buffers.
  curState = csStopping;
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

  if  (PlayingBack ()  &&  (acquisitionThread != NULL))
  {
    acquisitionThread->TerminateThread ();
    acquisitionThread->WaitForThreadToStop (10);
    if  (operatingMode == lomAdvanced)
      secondaryMsgs.AddMsg ("Stopped Reading Scanner File.");
  }

  // Now that we stopped collecting data from the camera (Acquision Device) we need to
  // wait until all the physical frames in the buffer have been processed.
  while  (!CameraFrameBufferEmpty ())
    osSleepMiliSecs (20);
  if  (operatingMode == lomAdvanced)
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
      curState = csDroppedFrames;
      logicalFrameBuilderThread->ShutdownThread ();
      while  (!LogicalFrameBuffersAreEmpty ()  &&  (!terminateFlag))
        osSleepMiliSecs (50);
    }
  }

  ShutDownLogicalFrameBuilderThread (stopImmediately);
  ShutDownLogicalFrameProcessorThreads (stopImmediately);
  ShutDownSnapshotThread ();
  ShutDownReportWriterThread ();

  if  (operatingMode == lomAdvanced)
    secondaryMsgs.AddMsg ("Processing Last Frames.");
  // Will now wait until all threads have stopped.
  bool  allThreadsStopped = false;
  WaitForAllButCameraThreadsToStop (120, allThreadsStopped);

  // Taking a snapshot of statistics at time the processing was stopped.
  if  (!finaleStats)
    finaleStats = new LarcosCounterStats ();
  GetStats (*finaleStats);

  operatingParameters->PlayingBack (false);

  if  ((acquisitionThread != NULL)  &&  (acquisitionThread->StartStatus () == CameraAcquisition::ssConnected))
    curState = csConnected;
  else
    curState = csStopped;

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

  secondaryMsgs.AddMsg ("Larcos Has Been Stopped.");

  runLog->Level (40) << "CloseOutCountingAndOrRecording   Exiting." << endl;

  stoppingIsInProgress = false;
}  /* CloseOutCountingAndOrRecording */




void  LarcosCounterManager::DeleteAllStoppedThreads ()
{
  runLog->Level (40) << "LarcosCounterManager::DeleteAllStoppedThreads." << endl;

  if  ((acquisitionThread)  &&  (acquisitionThread->Status () == CameraThread::tsStopped))
    DeleteAcquisitionThread ();

  if  ((logicalFrameBuilderThread)  &&  (logicalFrameBuilderThread->Status () == CameraThread::tsStopped))
    DeleteLogicalFrameBuilderThread ();

  if  (frameProcessors)
  {
    LogicalFrameProcessorList  threadsToDelete (false);

    LogicalFrameProcessorList::iterator  idx;
    for  (idx = frameProcessors->begin ();  idx != frameProcessors->end ();  ++idx)
    {
      LogicalFrameProcessorPtr  lfp = *idx;
      if  (lfp->Status () == CameraThread::tsStopped)
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

  if  ((cameraAutoGainThread)  &&  (cameraAutoGainThread->Status () == CameraThread::tsStopped))
    DeleteCameraAutoGainThread ();

  if  ((diskWriterThread)  &&  (diskWriterThread->Status () == CameraThread::tsStopped))
    DeleteDiskWriterThread ();

  if  ((reportWriterThread)  &&  (reportWriterThread->Status () == CameraThread::tsStopped))
    DeleteReportWriterThread ();

  if  ((snapshotThread)  &&  (snapshotThread->Status () == CameraThread::tsStopped))
    DeleteSnapshotThread ();

  if  ((connectButtonThread)  &&  (connectButtonThread->Status () == CameraThread::tsStopped))
    DeleteConnectButtonThread ();

  if  ((startButtonThread)  &&  (startButtonThread->Status () == CameraThread::tsStopped))
    DeleteStartButtonThread ();

  if  ((playBackButtonThread)  &&  (playBackButtonThread->Status () == CameraThread::tsStopped))
    DeletePlayBackButtonThread ();

  if  ((stopButtonThread)  &&  (stopButtonThread->Status () == CameraThread::tsStopped))
    DeleteStopButtonThread ();

  runLog->Level (40) << "DeleteAllStoppedThreads   Exiting." << endl;

}  /* DeleteAllStoppedThreads */




void  LarcosCounterManager::WaitForAllButCameraThreadsToStop (kkint32  maxSecsToWait,
                                                              bool&  allThreadsStopped
                                                             )
{
  if  (allThreads != NULL)
    return;

  runLog->Level (40) << "LarcosCounterManager::WaitForAllButCameraThreadsToStop." << endl
                     << "     maxSecsToWait: " << maxSecsToWait << endl;

  CameraThreadListPtr  tempList = new CameraThreadList (false);

  CameraThreadList::const_iterator  idx;
  for  (idx = allThreads->begin ();   idx != allThreads->end ();  ++idx)
  {
    CameraThreadPtr ct = *idx;
    if  ((ct != acquisitionThread)  &&  (ct != stopButtonThread))
    {
      runLog->Level (50) << "WaitForAllButCameraThreadsToStop   Thread: " << ct->ThreadName () << " Stll Running." << endl;
      tempList->PushOnBack (ct);
    }
  }

  tempList->WaitForAllThreadsToStop ((float)maxSecsToWait, allThreadsStopped);

  delete  tempList;
  tempList = NULL;

  if  (!allThreadsStopped)
    runLog->Level (-1) << "WaitForAllButCameraThreadsToStop   ***ERROR***   Not all threads stopped." << endl;

  runLog->Level (40) << "WaitForAllButCameraThreadsToStop   Exiting     allThreadsStopped: " << allThreadsStopped << endl;
}  /* WaitForAllButCameraThreadsToStop */



void  LarcosCounterManager::ShutdownOneThread (CameraThreadPtr  t)
{
  if  (t == NULL)
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


void  LarcosCounterManager::ShutDownCameraAutoGainThread ()
{
  ShutdownOneThread (cameraAutoGainThread);
}  /* ShutDownCameraAutoGainThread */




void  LarcosCounterManager::ShutDownDiskWriterThread ()
{
  ShutdownOneThread (diskWriterThread);
}  /* ShutDownDiskWriterThread */



void  LarcosCounterManager::ShutDownSnapshotThread ()
{
  ShutdownOneThread (snapshotThread);
}  /* ShutDownSnapshotThread */




void  LarcosCounterManager::ShutDownReportWriterThread ()
{
  ShutdownOneThread (reportWriterThread);
}  /* ShutDownReportWriterThread */



void  LarcosCounterManager::ShutDownLogicalFrameBuilderThread (bool stopImmediately)
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



void  LarcosCounterManager::ShutDownLogicalFrameProcessorThreads (bool stopImmediately)
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



void  LarcosCounterManager::DeleteOneThread (CameraThreadPtr &t)
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
      t = NULL;
    }
  }
}  /* DeleteOneThread */



void  LarcosCounterManager::DeleteAcquisitionThread ()
{
  DeleteOneThread ((CameraThreadPtr&)acquisitionThread);
}  /* DeleteAcquisitionThread */



void  LarcosCounterManager::DeleteCameraAutoGainThread ()
{
   DeleteOneThread ((CameraThreadPtr&)cameraAutoGainThread);
}  /* DeletecameraAutoGainThread */



void  LarcosCounterManager::DeleteDiskWriterThread ()
{
  DeleteOneThread ((CameraThreadPtr&)diskWriterThread);
}  /* DeleteDiskWriterThread */



void  LarcosCounterManager::DeleteReportWriterThread ()
{
  DeleteOneThread ((CameraThreadPtr&)reportWriterThread);
}  /* DeleteReportWriterThread */



void  LarcosCounterManager::DeleteSnapshotThread ()
{
  DeleteOneThread ((CameraThreadPtr&)snapshotThread);
}  /* DeleteSnapshotThread */



void  LarcosCounterManager::DeleteLogicalFrameBuilderThread ()
{
  DeleteOneThread ((CameraThreadPtr&)logicalFrameBuilderThread);
}  /* DeleteLogicalFrameBuilderThread */



void  LarcosCounterManager::DeleteLogicalFrameProcessorThreads ()
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
  while  (fp != NULL)
  {
    if  (fp->ThreadStillProcessing ())
      runLog->Level (-1) << "DeleteLogicalFrameProcessorThreads   ***ERROR***  LogicalFrameProcessor: " << fp->ThreadName () << "  Status: " << fp->StatusStr () << "   Still processing." << endl;

    /**@todo  If Still processing should not just delete instance of thread.  */

    allThreads->DeleteEntry (fp);
    delete  fp;
    fp = NULL;
    fp = frameProcessors->PopFromBack ();
  }

  delete  frameProcessors;
  frameProcessors = NULL;
  runLog->Level (20) << "DeleteLogicalFrameProcessorThreads   Exiting." << endl;
}  /* DeleteLogicalFrameProcessorThreads */




void  LarcosCounterManager::DeleteConnectButtonThread ()
{
  DeleteOneThread ((CameraThreadPtr&)connectButtonThread);
}



void  LarcosCounterManager::DeleteStartButtonThread ()
{
  DeleteOneThread ((CameraThreadPtr&)startButtonThread);
}



void  LarcosCounterManager::DeletePlayBackButtonThread ()
{
  DeleteOneThread ((CameraThreadPtr&)playBackButtonThread);
}



void  LarcosCounterManager::DeleteStopButtonThread ()
{
  DeleteOneThread ((CameraThreadPtr&)stopButtonThread);
}


//  Will stay in a loop until all threads that this object controls are stopped.
void  LarcosCounterManager::ShutDownAllThreads ()
{
  // We will first cancel the camera/ datasource thread.  Then wait for the other threads to
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



void  LarcosCounterManager::TerminateAllThreads ()
{
  bool  allThreadsStopped = false;
  if  (allThreads)
  {
    allThreads->SendTerminateCmdToAllThreads ();
    allThreads->WaitForAllThreadsToStop (10.0f, allThreadsStopped);
  }
}  /* TerminateAllThreads */



void  LarcosCounterManager::TerminateAndDeleteAllButPlayBackButtonThread ()
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



void  LarcosCounterManager::TerminateAndDeleteAllButCameraAndStartButtonThreads ()
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



void  LarcosCounterManager::TerminateAndDeleteAllButConnectButtonThread ()
{
  DeleteLogicalFrameBuilderThread    ();
  DeleteLogicalFrameProcessorThreads ();
  DeleteCameraAutoGainThread         ();
  DeleteDiskWriterThread             ();
  DeleteAcquisitionThread              ();
  DeleteReportWriterThread           ();
  DeleteStartButtonThread            ();
  DeletePlayBackButtonThread         ();
  DeleteStopButtonThread             ();
}  /* TerminateAndDeleteAllButConnectButtonThread */




void   LarcosCounterManager::DeleteAllThreads ()
{
  if  (allThreads)
  {
    TerminateAllThreads ();
    while  (allThreads->size () > 0)
    {
      CameraThreadPtr  t = allThreads->PopFromFront ();
      delete  t;
      t = NULL;
    }
  }

  delete  frameProcessors;
  frameProcessors = new LogicalFrameProcessorList (false);

  acquisitionThread         = NULL;
  cameraAutoGainThread      = NULL;
  diskWriterThread          = NULL;
  reportWriterThread        = NULL;
  logicalFrameBuilderThread = NULL;
  startButtonThread         = NULL;
  playBackButtonThread      = NULL;
  stopButtonThread          = NULL;
  connectButtonThread       = NULL;
}  /* DeleteAllThreads */





void  LarcosCounterManager::StartCameraAcquisitionThread (CameraAcquisitionPtr  acquisitionThread, 
                                                          bool&                 _successful
                                                         )
{
  secondaryMsgs.AddMsg ("Starting Camera Acquisition.");

  acquisitionThread->RequestedCameraParameters (operatingParameters);

  acquisitionThread->CropSettingsChanged (cropLeft, cropRight);

  acquisitionThread->Start (KKThread::tpHigh, _successful);
  if  (_successful)
  {
    while  (acquisitionThread->StartStatus() != CameraAcquisition::ssConnected)
    {
      osSleep (0.1f);
      if  (acquisitionThread->Crashed ())
      {
        _successful = false;
        break;
      }
    }
  }
}  /* StartCameraAcquisitionThread*/



void  LarcosCounterManager::StartCameraDiskWriterThread (bool&  _successful)
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

  diskWriterThread->Start (KKThread::tpNormal, _successful);
  allThreads->PushOnBack (diskWriterThread);
}




void  LarcosCounterManager::StartReportWriterThread (bool&  _successful)
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

  reportWriterThread->Start (KKThread::tpNormal, _successful);
  allThreads->PushOnBack (reportWriterThread);
}  /* StartReportWriterThread */




void  LarcosCounterManager::StartSnapshotThread (bool&  _successful)
{
  _successful = false;
  DeleteSnapshotThread ();

  if  (snapshotBuffer)
    snapshotBuffer->Reset ();
  else
    snapshotBuffer = new StatusSnapshotBuffer ();
  
  delete  snapshotThread;
  snapshotThread = new StatusSnapshotThread (this, snapshotBuffer, snapshotInterval, msgQueue, "StatusSnapshotThread");

  snapshotThread->Start (KKThread::tpNormal, _successful);
  allThreads->PushOnBack (snapshotThread);
}




void   LarcosCounterManager::StartFrameBuilderAndProcessingThreads (bool&  _successful)
{
  _successful= true;

  cameraFrameBuffer->SetCompensationTable (ScannerFile::ConpensationTable (operatingParameters->DestScannerFileFormat ()));

  if  (operatingMode == lomAdvanced)
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
    droppedFrames = new LogicalFrameEntryQueue ("DroppedFrames", true);

  if  (!particleEntryBuffer)
    particleEntryBuffer = new ParticleEntryBuffer ();

  if  (!lastParticlesProcessed)
    lastParticlesProcessed = new RasterBuffer ("LastParticlesProcessed", 8);

  //int numFrameProcessesToStart = Min (5, Max (1, (maxNumOfThreads - 2)));

  int  maxNumFrameildsers = 5;
  if  (this->cameraFrameBuffer->FrameWidth () > 2048)
    maxNumFrameildsers = 2;

  int numFrameProcessesToStart = Min (maxNumFrameildsers, Max (1, (maxNumOfThreads - 3)));
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
    fp->Start (KKThread::tpNormal, _successful);
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
    logicalFrameBuilderThread->Start (KKThread::tpNormal, _successful);

    if  (!_successful)
    {
      runLog->Level (-1) << "LogicalFrameBuilder Thread failed to start." << endl;
      return;
    }
    else
    {
      if  (operatingMode == lomAdvanced)
        secondaryMsgs.AddMsg ("Frame Processors Started.");
    }
  }
}  /* StartFrameBuilderAndProcessingThreads */



void  LarcosCounterManager::HeaderFieldsClear ()
{
  headerFields->Clear ();
}


void  LarcosCounterManager::AddHeaderField (const KKStr&  _fieldName,
                                            const KKStr&  _fieldValue
                                           )
{
  headerFields->Add (_fieldName, _fieldValue);
}


void  LarcosCounterManager::AddHeaderField (const KKStr&  _fieldName,
                                            bool          _fieldValue
                                           )
{
  headerFields->Add (_fieldName, _fieldValue);
}


void  LarcosCounterManager::AddHeaderField (const KKStr&  _fieldName,
                                            kkint32       _fieldValue
                                           )
{
  headerFields->Add (_fieldName, _fieldValue);
}


void  LarcosCounterManager::AddHeaderField (const KKStr&  _fieldName,
                                            kkint64       _fieldValue
                                           )
{
  headerFields->Add (_fieldName, _fieldValue);
}


void  LarcosCounterManager::AddHeaderField (const KKStr&  _fieldName,
                                            float         _fieldValue
                                           )
{
  headerFields->Add (_fieldName, _fieldValue);
}


void  LarcosCounterManager::AddHeaderField (const KKStr&  _fieldName,
                                            double        _fieldValue
                                           )
{
  headerFields->Add (_fieldName, _fieldValue);
}


void  LarcosCounterManager::AddMessageToCurrentScannerFile (const KKStr&  msg)
{
  if  (diskWriterThread)
    diskWriterThread->AddMessageToCurrentScannerFile (msg);
}

 

VectorUcharPtr  LarcosCounterManager::CameraHighPoints ()
{
  if  (!cameraFrameBuffer)
    return NULL;
  VectorUcharPtr  cameraHighs = cameraFrameBuffer->CameraHighPoints ();
  return  cameraHighs;
}


VectorUcharPtr  LarcosCounterManager::CameraHighPointsFromLastNSampleLines (kkint32 n)  const
{
  if  (!cameraFrameBuffer)
    return NULL;
  VectorUcharPtr  cameraHighs = cameraFrameBuffer->CameraHighPointsFromLastNSampleLines (n);
  return  cameraHighs;
}



VectorUcharPtr  LarcosCounterManager::LastFrameAverageScanLine()  const
{
  if  (!cameraFrameBuffer)
    return NULL;
  VectorUcharPtr  avgScanLine = cameraFrameBuffer->LastFrameAverageScanLine ();
  return  avgScanLine;
}



VectorUcharPtr  LarcosCounterManager::LastFrameHighValuesScanLine()  const
{
  if  (!cameraFrameBuffer)
    return NULL;
  VectorUcharPtr  avgScanLine = cameraFrameBuffer->LastFrameHighValuesScanLine ();
  return  avgScanLine;
}



bool  LarcosCounterManager::StoppingIsInProgress ()  const
{
  return stoppingIsInProgress;
}



KKStr  LarcosCounterManager::CameraSerialNum ()
{
  if  (acquisitionThread)
    return  acquisitionThread->SerialNumber ();
  else  
    return KKStr::EmptyStr ();
}


KKStr  LarcosCounterManager::CameraMacAddress ()
{
  if  (acquisitionThread)
    return  acquisitionThread->MacAddress ();
  else
    return  KKStr::EmptyStr ();
}



void  LarcosCounterManager::ResetCounts  ()
{
  if  (allThreads)
  {
    CameraThreadList::iterator  idx;
    for  (idx = allThreads->begin ();  idx != allThreads->end ();  ++idx)
      (*idx)->ResetCounts ();
  }
}



kkint32  LarcosCounterManager::ParticlesCounted ()
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



void   LarcosCounterManager::PlayingBackRealTime (bool  _playingBackRealTime)
{
  playingBackRealTime = _playingBackRealTime;
}



const CameraParametersPtr   LarcosCounterManager::CameraParams ()  const
{
  if  (acquisitionThread)
    return acquisitionThread->CameraParams ();
  else
    return NULL;
}



void  LarcosCounterManager::FrameHeightWidthChanged (kkint32  _newFrameHeight,
                                                     kkint32  _newFrameWidth
                                                    )
{
  cameraFrameHeight = _newFrameHeight;
  cameraFrameWidth  = _newFrameWidth;
}


void  LarcosCounterManager::AddSecondaryMsg (const KKStr&  msg)
{
  secondaryMsgs.AddMsg (msg);
}


/**
 *@brief  retrieves run time variables that would typically be displayed allowing technician to see what is going on.
 *@param[out]  stat's
 */
void  LarcosCounterManager::GetStats (LarcosCounterStats&  stats)
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




void  LarcosCounterManager::GetSessionParameters (SessionParameters&  _sessionParameters)
{
  _sessionParameters.Assign (*sessionParameters);
}



void  LarcosCounterManager::GetOperatingParameters (OperatingParameters&  _operatingParameters)
{
  _operatingParameters.Assign (*operatingParameters);
}


void  LarcosCounterManager::GetDefaultOperatingParameters (OperatingParameters&  _defaultOperatingParameters)
{
  _defaultOperatingParameters.Assign (*defaultOperatingParameters);
}



kkint32  LarcosCounterManager::GetLastSessionId ()
{
  return  SessionLogger::GetLastSessionId ();
}



void  LarcosCounterManager::GetLogicalFrameStats (kkint32&  _logicalFramesOnQueue,
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



bool  LarcosCounterManager::GenerateFinaleReport ()
{
  goalie->StartBlock ();
  bool  ok = generateFinaleReport;
  if  (ok)
    generateFinaleReport = false;
  goalie->EndBlock ();
  return  ok;
}






int  LarcosCounterManager::FrameProcessorsCount () const
{
  if  (frameProcessors)
    return frameProcessors->QueueSize ();
  else
    return 0;
}


bool  LarcosCounterManager::LogicalFrameBuilderRunning ()
{
  return  ((logicalFrameBuilderThread != NULL)  &&  (logicalFrameBuilderThread->Status () == KKThread::tsRunning));
}



const KKStr&  LarcosCounterManager::ControlNum ()
{
  if  (sessionParameters == NULL)
    return KKStr::EmptyStr ();
  else
    return sessionParameters->ControlNum ();
}


const KKStr&  LarcosCounterManager::Description () const
{
  if  (sessionParameters == NULL)
    return KKStr::EmptyStr ();
  else
    return sessionParameters->SessionDescription ();
}


bool  LarcosCounterManager::EmbeddedFlowMeter () const
{
  if  (installation)
    return  (installation->FlowMeterMethod () == InstallationConfig::fmmEmbedded);
  else
    return false;
}


const KKStr&  LarcosCounterManager::TrainingModelName () const
{
  if  (sessionParameters == NULL)
    return KKStr::EmptyStr ();
  else
    return sessionParameters->TrainingModelName ();
}


bool  LarcosCounterManager::DiskWritingThreadRunning ()  const
{
  return  ((diskWriterThread != NULL)  &&  (diskWriterThread->Status () == KKThread::tsRunning));
}


bool  LarcosCounterManager::CameraAutoGainThreadRunning ()  const
{
  return  ((cameraAutoGainThread != NULL)  &&  (cameraAutoGainThread->Status () == KKThread::tsRunning));
}


bool  LarcosCounterManager::CameraFrameBufferEmpty ()
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


bool  LarcosCounterManager::LogicalFrameBuffersAreEmpty ()
{
  if  (((logicalFrames       != NULL)  &&  (logicalFrames->NumWaitingToBeProcessed () > 0))  ||
       ((droppedFrames       != NULL)  &&  (droppedFrames->QueueSize               () > 0))  ||
       ((particleEntryBuffer != NULL)  &&  (particleEntryBuffer->DataAvailable     ()))
      )
  {
    return false;
  }
  else
  {
    return  true;
  }
}



StatusSnapshotListPtr  LarcosCounterManager::SnapshotsRetrieve (KKB::DateTime  rangeStart,
                                                                KKB::DateTime  rangeEnd
                                                               )
{
  if  (snapshotBuffer)
    return snapshotBuffer->Retrieve (rangeStart, rangeEnd);
  else
    return NULL;
}



const StatusSnapshotPtr  LarcosCounterManager::SnapshotLastEntry ()
{
  if  (snapshotBuffer)
    return snapshotBuffer->LastEntry ();
  else
    return NULL;
}



RasterPtr  LarcosCounterManager::GetLastParticle ()
{
  RasterPtr  lastParticle = NULL;

  if  (lastParticlesProcessed)
    lastParticle = lastParticlesProcessed->GetNextRaster ();
  return  lastParticle;
}



void  LarcosCounterManager::StartNewScannerFile (ScannerFileEntryPtr  _scannerFileEntry)
{
  if  (cameraFrameBuffer)
  {
    cameraFrameBuffer->StartNewScannerFile (_scannerFileEntry);
  }
}




void  LarcosCounterManager::StartingToReadNewScannerFile (ScannerFileEntryPtr  _scannerFileEntry)
{
  if  (!operatingParameters->DataIsToBeRecorded ())
  {
    // Since we are not recording a new Scanner File we need to use the name of the scanner files that we
    // are reading from to drive the name of the extracted images.
    if  (cameraFrameBuffer)
      cameraFrameBuffer->StartNewScannerFile (_scannerFileEntry);
  }
}




void  LarcosCounterManager::ComputeLastFramesMinScanLines ()
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




void  LarcosCounterManager::SetLiveVideoDimensions (kkint32  _liveVideoHeight,
                                                    kkint32  _liveVideoWidth
                                                   )
{
  liveVideoHeight = _liveVideoHeight;
  liveVideoWidth  = _liveVideoWidth;
  ComputeLastFramesMinScanLines ();
}




RasterPtr  LarcosCounterManager::SnapShotLatestFrame ()
{
  goalie->StartBlock ();
  RasterPtr  lastVideoFrame = NULL;
  if  (cameraFrameBuffer != NULL)
    lastVideoFrame = cameraFrameBuffer->SnapShotLatestFrame (CropLeft (), CropRight ());
  goalie->EndBlock ();
  return  lastVideoFrame;
}  /* SnapShotLatestFrame */




bool  LarcosCounterManager::CameraThreadRunning ()  const
{
  bool  cameraThreadRunning = false;
  goalie->StartBlock ();
  cameraThreadRunning = ((acquisitionThread != NULL)  &&  acquisitionThread->ThreadStillProcessing ());
  goalie->EndBlock ();
  return  cameraThreadRunning;
}



void  LarcosCounterManager::BackGroundPixelTH (uchar _backGroundPixelTH)
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



kkint32 LarcosCounterManager::CropLeft ()
{
  return  cropLeft;
}



kkint32  LarcosCounterManager::CropRight ()
{
  return  cropRight;
}



kkint32  LarcosCounterManager::FrameWidthInPixels () const
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



float   LarcosCounterManager::CurAnalogGain () const
{
  if  (acquisitionThread)
    return  acquisitionThread->CurAnalogGain ();
  else
    return 0.0;
}



kkint32 LarcosCounterManager::CurDigitalGain () const
{
  if  (acquisitionThread)
    return  acquisitionThread->CurDigitalGain ();
  else
    return 0;
}



float  LarcosCounterManager::CurScanRate () const
{
  if  (acquisitionThread)
    return  acquisitionThread->CurScanRate ();
  else
    return 0;
}



void  LarcosCounterManager::CropSettingsChanged (kkint32  _cropLeft,
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



void   LarcosCounterManager::ScanRateChanged (float _newScanRate)
{
  CameraThreadList::iterator idx;

  if  (flowMeter)
    flowMeter->ScanRateChanged (_newScanRate);
  for  (idx = allThreads->begin ();  idx != allThreads->end ();  ++idx)
  {
    CameraThreadPtr  ct = *idx;
    ct->ScanRateChanged (_newScanRate);
  }
}



void  LarcosCounterManager::FlatFieldEnabled (bool  _flatFieldEnabled)
{
  operatingParameters->FlatFieldEnabled (_flatFieldEnabled);
  AddHeaderField ("OP:FlatFieldEnabled", _flatFieldEnabled);

  if  (cameraFrameBuffer)
    cameraFrameBuffer->FlatFieldEnabled (_flatFieldEnabled);

  if  (acquisitionThread)
    acquisitionThread->FlatFieldEnabled (_flatFieldEnabled);
}



void  LarcosCounterManager::MinSizeThreshold (kkint32 _minSizeThreshold)
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



bool  LarcosCounterManager::WeAreRecordingToDisk ()
{
  return  (((curState == csRunning)  ||  (curState == csPlayingBack))             &&
           (diskWriterThread  != NULL)  &&  (acquisitionThread != NULL)           &&
           (diskWriterThread->WeAreRecordingToDisk ())                            &&
           (acquisitionThread->StartStatus () == CameraAcquisition::ssConnected)  &&
           (acquisitionThread->Status ()      == CameraThread::tsRunning)
          );
}



void  LarcosCounterManager::RequestedAnalogGain (float _requestedAnalogGain)
{
  operatingParameters->RequestedAnalogGain (_requestedAnalogGain);
  if  (acquisitionThread)
    acquisitionThread->RequestedAnalogGain (_requestedAnalogGain);
}



void  LarcosCounterManager::RequestedDigitalGain (kkint32 _requestedDigitalGain)
{
  operatingParameters->RequestedDigitalGain (_requestedDigitalGain);
  if  (acquisitionThread)
    acquisitionThread->RequestedDigitalGain (_requestedDigitalGain);
}



void  LarcosCounterManager::RequestedScanRate (float _requestedScanRate)
{
  operatingParameters->RequestedScanRate (_requestedScanRate);
  if  (acquisitionThread)
    acquisitionThread->RequestedScanRate (_requestedScanRate);
}


void  LarcosCounterManager::RequestedSensitivityMode (const KKStr&  _requestedSensitivityMode)
{
  operatingParameters->RequestedSensitivityMode (_requestedSensitivityMode);
  if  (acquisitionThread)
    acquisitionThread->RequestedSensitivityMode (_requestedSensitivityMode);
}




void   LarcosCounterManager::SampleLastFrameBeforeFlatField (bool _sampleLastFrameBeforeFlatField)
{
  sampleLastFrameBeforeFlatField = _sampleLastFrameBeforeFlatField;
  if  (cameraFrameBuffer)
    cameraFrameBuffer->SampleLastFrameBeforeFlatField (_sampleLastFrameBeforeFlatField);
}



void   LarcosCounterManager::SnapshotInterval (kkint32 _snapshotInterval)   
{
  snapshotInterval = _snapshotInterval;
  if  (snapshotThread)
    snapshotThread->SnapshotInterval (snapshotInterval);
}



void  LarcosCounterManager::Status (KKStr&  statusMsg,
                                    KKStr&  color,
                                    KKStr&  secondaryMsg,
                                    float&  analogGain,
                                    kkint32&  digitalGain,
                                    kkint32&  cameraTemparature,
                                    float&  flowRate
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
    nextMsg = NULL;
  }
  else
  {
    secondaryMsg = "";
  }

  if  (curState == csPlayingBack)
  {
    statusMsg = "Playing Back";
    color = "White";
    goalie->EndBlock ();
    return;
  }

  else if  (curState == csDroppedFrames)
  {
    if  (this->operatingMode == lomUser)
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

  KKThread::ThreadStatus               cameraStatus      = KKThread::tsNULL;
  CameraAcquisition::StartStatusType   cameraStartStatus = CameraAcquisition::ssNULL;
  DiskWriterThread::DiskWritingStatus  diskWritingStatus = DiskWriterThread::dwNULL;
  KKStr  cameraMsg = "";

  if  (acquisitionThread)
  {
    cameraStatus      = acquisitionThread->Status      ();
    cameraStartStatus = acquisitionThread->StartStatus ();
    cameraMsg         = acquisitionThread->StatusMsg   ();
  }
  else
  {
    cameraStatus      = CameraAcquisition::tsNULL;
    cameraStartStatus = CameraAcquisition::ssDisconnected;
    cameraMsg = "";
  }

  if  (diskWriterThread)
    diskWritingStatus = diskWriterThread->DiskStatus ();
  else
    diskWritingStatus = DiskWriterThread::dwNULL;

  color = "White";

  if  (acquisitionThread == NULL)
  {
    color = "White";
    statusMsg = "Not Connected";
    goalie->EndBlock ();
    return;
  }

  if  (cameraStatus == CameraAcquisition::tsRunning)
  {
    switch  (cameraStartStatus)
    {
    case  CameraAcquisition::ssConnecting:
      color = "Orange";
      statusMsg = "Connecting";
      break;

    case  CameraAcquisition::ssConnected:
      color = "White";
      statusMsg = "Connected";
      if  (curState == csStarting)
        statusMsg = "Starting";

      else if  (curState == csBuildingClassifier)
        statusMsg = "Building Classifier";
      break;

    case  CameraAcquisition::ssConnectionFailed:
      color = "Red";
      statusMsg = "Connecteion Error";
      break;

    case  CameraAcquisition::ssDisconnected:
      color = "Orange";
      statusMsg = "Disconnected";
      break;

    case  CameraAcquisition::ssStoped:
      color = "White";
      statusMsg = "Stopped";
      break;
    }
  }

  goalie->EndBlock ();
}  /* Status */



bool   LarcosCounterManager::WeAreConnectedToCamera ()  const
{
  if  (!acquisitionThread)
    return false;
  else
    return  (acquisitionThread->StartStatus () == CameraAcquisition::ssConnected);
}



bool   LarcosCounterManager::WeAreConnectingToCamera ()  const
{
  if  (!acquisitionThread)
    return false;
  else
    return  (acquisitionThread->StartStatus () == CameraAcquisition::ssConnecting);
}




void  LarcosCounterManager::SetOperatingParameters (const OperatingParametersPtr  _operatingParameters)
{
  operatingParameters->Assign (*_operatingParameters);
}



void  LarcosCounterManager::SetDefaultOperatingParameters (const OperatingParametersPtr  _defaultOperatingParameters)
{
  defaultOperatingParameters->Assign (*_defaultOperatingParameters);
  SaveConfiguration ();
}



void  LarcosCounterManager::SetTrainingModel (const KKStr&            _trainingModelName,
                                              bool&                   _successful,
                                              OperatingParametersPtr  _opParms
                                             )
{
  goalie->StartBlock ();

  runLog->Level (40) << "LarcosCounterManager::SetTrainingModel   TrainingModelName: " << _trainingModelName << endl;

  LarcosTrainingConfigurationPtr  config = NULL;

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
    config = new LarcosTrainingConfiguration (_trainingModelName, operatingParameters, *runLog, false);
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
  config = NULL;

  goalie->EndBlock ();
}  /* SetTrainingModel */





void  LarcosCounterManager::SaveConfiguration ()
{
  goalie->StartBlock ();

  ofstream o (configurationFileName.Str ());
  if  (!o.is_open ())
  {
    runLog->Level (-1) << endl
      << "LarcosCounterManager::SaveConfiguration   ***ERROR***   Failed to write ConfiguratioFile" << endl
      << endl;
    goalie->EndBlock ();
    return;
  }

  o << "<LarcosCounterManager>" << endl
    << "// LarcosCounter Configuration"  << endl
    << "// DateTime" << "\t" << osGetLocalDateTime () << endl
    << endl
    << endl;

  o << "SrcScannerFileName"             << "\t" << srcScannerFileName                                  << endl;
  o << "NameLastRecordedFile"           << "\t" << nameLastRecordedFile                                << endl;
  o << "DropFolderToLarcos"             << "\t" << dropFolderToLarcos                                  << endl;
  o << "OperatingMode"                  << "\t" << LarcosOperatingModeToStr (operatingMode)            << endl;
  o << "ThroughPutField"                << "\t" << StatusSnapshot::DataFieldIdxToStr (throughPutField) << endl;
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
  o << "</LarcosCounterManager>"        << endl;
  o.close ();

  goalie->EndBlock ();
  return;
}  /* SaveConfiguration */




void  LarcosCounterManager::ReadConfiguration ()
{
  goalie->StartBlock ();

  ifstream i (configurationFileName.Str ());
  if  (!i.is_open ())
  {
    runLog->Level (-1) << endl
      << "LarcosCounterManager::ReadConfiguration   ***ERROR***      Failed to Read ConfiguratioFile" << endl
      << endl;
    goalie->EndBlock ();
    return;
  }

  bool  eol = false;
  bool  eof = false;
  bool  fieldFound = false;

  while (!eof)
  {
    KKStr  fieldName = osReadNextToken (i, "\t", eof, eol);
    if  (eof)
      break;

    if  (fieldName.EqualIgnoreCase ("</LarcosCounterManager>"))
      break;

    if  (eol)
      continue;

    if  (fieldName.StartsWith ("//"))
    {
      osReadRestOfLine (i, eof);
    }

    else
    {
      KKStr  restOfLine = osReadRestOfLine (i, eof).TrimRight ("\n\r");

      if  (fieldName.EqualIgnoreCase ("<SessionParameters>"))
        sessionParameters->ReadXML (i);

      else if  (fieldName.EqualIgnoreCase ("<OperatingParameters>"))
        defaultOperatingParameters->ReadXML (i);

      else if  (fieldName.EqualIgnoreCase ("DropFolderToLarcos"))
        dropFolderToLarcos = restOfLine.TrimRight ();

      else if  (fieldName.EqualIgnoreCase ("OperatingMode"))
      {
        operatingMode = LarcosOperatingModeFromStr (restOfLine);
        if  ((operatingMode == lomNULL)  ||  (operatingMode == lomInvalid))
          operatingMode = lomUser;
      }

      else if  (fieldName.EqualIgnoreCase ("ThroughPutField"))
        throughPutField = StatusSnapshot::DataFieldIdxFromStr (restOfLine);

      else if  (fieldName.EqualIgnoreCase ("SrcScannerFileName"))
        srcScannerFileName = restOfLine;

      else if  (fieldName.EqualIgnoreCase ("NameLastRecordedFile"))
        nameLastRecordedFile = restOfLine;

      else if  (fieldName.EqualIgnoreCase ("LastControlNumber"))
        lastControlNumber = restOfLine;

      else if  (fieldName.EqualIgnoreCase ("LastScannerFileCounted"))
        lastScannerFileCounted = restOfLine;

      else if  (fieldName.EqualIgnoreCase ("CropLeft"))
        cropLeft = restOfLine.ToInt32 ();

      else if  (fieldName.EqualIgnoreCase ("CropRight"))
        cropRight = restOfLine.ToInt32 ();

      else if  (fieldName.EqualIgnoreCase ("CameraMacAddress"))
        cameraMacAddress = restOfLine;

      else if  (fieldName.EqualIgnoreCase ("SampleLastFrameBeforeFlatField"))
        sampleLastFrameBeforeFlatField = restOfLine.ToBool ();

      else if  (fieldName.EqualIgnoreCase ("SnapshotInterval"))
        snapshotInterval = restOfLine.ToInt32 ();

      else if  (fieldName.EqualIgnoreCase ("PlayingBackRealTime"))
        playingBackRealTime = restOfLine.ToBool ();
    }
  }

  i.close ();

  const KKStr&  trainModelName = sessionParameters->TrainingModelName ();

  operatingParameters->Assign (*defaultOperatingParameters);

  if  (!trainModelName.Empty ())
  {
    // retrieve any parameters specified in the default training model.
    FileDescPtr  fileDesc = LarcosFVProducer::DefineFileDescStatic ();
    LarcosTrainingConfigurationPtr  config = new LarcosTrainingConfiguration (trainModelName, operatingParameters, *runLog, false);
    if  (!config->FormatGood ())
    {
      runLog->Level (-1) << "ReadConfiguration   ***ERROR***  Default Training Model: " << trainModelName << "  is Invalid." << endl;
    }
    else
    {
      operatingParameters->Assign (*(config->OperatingParms ()));
    }
    delete  config;
    config = NULL;
  }

  if  ((operatingMode != lomAdvanced)  ||  (throughPutField < 0)  ||  (throughPutField >= StatusSnapshot::dfiInvalid))
    throughPutField = StatusSnapshot::dfiCount;

  if  (snapshotInterval < 1)
    snapshotInterval = 5;

  else if  (snapshotInterval > 900)
    snapshotInterval = 5;

  if  (flowMeter)
    flowMeter->FlowRateRatioDefaultChanged (defaultOperatingParameters->FlowRateFactor ());

  goalie->EndBlock ();

}  /* ReadConfiguration */
