#include "StdAfx.h"
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

#include "KKException.h"
#include "KKStr.h"
#include "OSservices.h"
#include "Raster.h"
#include "RasterBuffer.h"
#include "RunLog.h"
using namespace  KKB;


#include "CounterVariables.h"
using namespace  CounterBase;


#include "CameraParametersManaged.h"
#include "LarcosCounterStatsManaged.h"
#include "UmiKKStr.h"


#include "LarcosCounterManagerWrapper.h"


using  namespace  LarcosCounterManaged;


using namespace System;
using namespace System::Threading;
//using namespace System::Windows::Forms;



LarcosCounterManagerWrapper::LarcosCounterManagerWrapper ():
    larcosCounterManager       (NULL),
    larcosCounterStats         (NULL),
    lastReportedMemoryPreasure (0),
    loggedMsgs                 (NULL),
    msgQueue                   (),
    saveDebugImages            (false)
{
  CounterBase::CounterVariables::SetCounterHomeDir ();

  loggedMsgs = new MsgQueue ("LarcosCounterManagerWrapper::LoggedMsgs");
  msgQueue = new MsgQueue ("LarcosCounterManagerWrapper");

  int maxNumOfThreads = System::Environment::ProcessorCount;
  larcosCounterManager = new LarcosCounterManager (msgQueue, loggedMsgs, maxNumOfThreads);
  larcosCounterManager->SaveDebugImages (saveDebugImages);
  UpdateUnManagedConsumedMemory ();
}




LarcosCounterManagerWrapper::!LarcosCounterManagerWrapper ()
{
  CleanUpMemory ();
}



LarcosCounterManagerWrapper::~LarcosCounterManagerWrapper ()
{
  this->!LarcosCounterManagerWrapper ();
}



void  LarcosCounterManagerWrapper::Initialize ()
{
  larcosCounterManager->Initialize ();
}



void  LarcosCounterManagerWrapper::SaveDebugImages (bool _saveDebugImages)
{
  saveDebugImages = _saveDebugImages;
  larcosCounterManager->SaveDebugImages (saveDebugImages);
}


String^  LarcosCounterManagerWrapper::CounterStateToStr (LarcosCounterState  state)
{

  LarcosCounterManager::CounterState  unmanagedState = (LarcosCounterManager::CounterState)state;

  KKStr counterStateStr = LarcosCounterManager::CounterStateToStr (unmanagedState);
  return  KKStrToSystenStr (counterStateStr);
}



void  LarcosCounterManagerWrapper::CleanUpMemory ()
{
  delete  larcosCounterManager;   larcosCounterManager = NULL;
  delete  larcosCounterStats;     larcosCounterStats   = NULL;
  delete  msgQueue;               msgQueue             = NULL;
  delete  loggedMsgs;             loggedMsgs           = NULL;

  if  (lastReportedMemoryPreasure > 0)
  {
    GC::RemoveMemoryPressure (lastReportedMemoryPreasure);
    lastReportedMemoryPreasure = 0;
  }
}



float  LarcosCounterManagerWrapper::ImagingChamberWidth ()
{
  return larcosCounterManager->ImagingChamberWidth ();
}



int  LarcosCounterManagerWrapper::ImagingChamberWidthPixels ()
{
  return larcosCounterManager->ImagingChamberWidthPixels ();
}



float  LarcosCounterManagerWrapper::ImagingChamberPixelsPerMM ()
{
  return larcosCounterManager->ImagingChamberPixelsPerMM ();
}



KKStr  LarcosCounterManagerWrapper::SystemStringToKKStr (System::String^  s)
{
  if  (s == nullptr)
    return KKStr::EmptyStr ();

  KKB::KKStr  kkStr (s->Length);
  for  (int x = 0;  x < s->Length;  x++)
    kkStr.Append ((char)s[x]);
  return  kkStr;
}



System::String^  LarcosCounterManagerWrapper::KKStrToSystenStr (const KKStr&  s)
{
  System::String^  sysStr = gcnew System::String (s.Str ());
  return  sysStr;
}



String^  LarcosCounterManagerWrapper::LarcosVersionNumber ()
{
  return UmiKKStr::KKStrToSystenStr (larcosCounterManager->LarcosVersionNumber ());
}



LarcosOperatingModes  LarcosCounterManagerWrapper::OperatingMode ()
{
  return  (LarcosOperatingModes)(larcosCounterManager->OperatingMode ());
}



UmiInstallationConfig^  LarcosCounterManagerWrapper::Installation (UmiRunLog^  runLog)
{
  const InstallationConfigPtr  installation = larcosCounterManager->Installation ();
  if  (installation == NULL)
    return  nullptr;

  if  (runLog == nullptr)
    runLog = gcnew UmiRunLog ();


  InstallationConfigPtr  newInstallation = new InstallationConfig (*installation, runLog->Log ());
  
  return gcnew UmiInstallationConfig (newInstallation);
}



LarcosCounterState  LarcosCounterManagerWrapper::CurState ()  
{
  return  (LarcosCounterState)(larcosCounterManager->CurState ());
}




LarcosCounterManagerWrapper::StatusSnapshotDataFieldIdx   LarcosCounterManagerWrapper::ThroughPutField ()
{
  return  (StatusSnapshotDataFieldIdx)(larcosCounterManager->ThroughPutField ());
}



void  LarcosCounterManagerWrapper::ThroughPutField (StatusSnapshotDataFieldIdx _throughPutField)
{
  larcosCounterManager->ThroughPutField ((StatusSnapshot::FieldIdx)_throughPutField);
}



CameraParametersManagedList^  LarcosCounterManagerWrapper::GetCameraList ()
{
  CameraParametersManagedList^  results = nullptr;

  CameraParametersListPtr  cameras = LarcosCounterManager::GetCameraList (NULL);
  if  (cameras)
    results = gcnew CameraParametersManagedList (*cameras);

  // The managed Constructor 'CameraParametersManagedList' will make its own copy of the camera data;  that
  // is it is not taking ownership of the contents of 'cameras'.
  delete  cameras;
  cameras = NULL;
  return  results;
}



CameraParametersManaged^  LarcosCounterManagerWrapper::PromptForCamera (UmiMsgQueue^  msgQueue)
{
  if  (msgQueue == nullptr)
    throw gcnew Exception ("LarcosCounterManagerWrapper::PromptForCamera    msgQueue == NULL");

  RunLog  runLog (msgQueue->MsgQueue ());
  runLog.SetLevel (CounterVariables::DebugLevel ());

  CameraParametersPtr  parameters = LarcosCounterManager::PromptForCamera (runLog);
  if  (parameters)
    return gcnew CameraParametersManaged (parameters);
  else
    return nullptr;
}



void  LarcosCounterManagerWrapper::AutoGainButtonPressed (bool%     _successful,
                                                          String^%  _errMsg
                                                         )
{
  bool  successful = false;
  KKStr  errMsg = "";

  larcosCounterManager->AutoGainButtonPressed (successful, errMsg);

  _errMsg = KKStrToSystenStr (errMsg);
  _successful = successful;
}



void  LarcosCounterManagerWrapper::ConnectButtonPressed ()
{
  larcosCounterManager->ConnectButtonPressed ();
}




void  LarcosCounterManagerWrapper::PlayBackButtonPressed 
                         (String^                 _srcScannerFileName,
                          UmiSessionParameters^   _sessionParameters,
                          UmiOperatingParameters^ _operatingParameters,
                          bool%                   _successful,
                          String^%                _errMsg
                         )
{
  KKStr  srcScannerFileName = SystemStringToKKStr (_srcScannerFileName);

  bool  successful = _successful;

  KKStr errMsg;

  larcosCounterManager->PlayBackButtonPressed 
                         (srcScannerFileName,
                          _sessionParameters->UnManagedClass (),
                          _operatingParameters->UnManagedClass (),
                          successful,
                          errMsg
                         );

  _errMsg = KKStrToSystenStr (errMsg);
  _successful = successful;
}  /* PlayBackButtonPressed */




void  LarcosCounterManagerWrapper::RecordButtonPressed 
                         (UmiSessionParameters^   _sessionParameters,
                          UmiOperatingParameters^ _operatingParameters,
                          bool%                   _successful,
                          String^%                _errMsg
                         )
{
  bool   successful = _successful;
  KKStr  errMsg;

  larcosCounterManager->RecordButtonPressed 
                         (_sessionParameters->UnManagedClass (),
                          _operatingParameters->UnManagedClass (),
                          successful,
                          errMsg
                         );

  _errMsg = KKStrToSystenStr (errMsg);
  _successful = successful;
}




void  LarcosCounterManagerWrapper::StopButtonPressed (bool      _stopImmediately,
                                                      bool%     _successful,
                                                      String^%  _errMsg
                                                     )
{
  bool  successful = _successful;
  KKStr  errMsg = SystemStringToKKStr (_errMsg);
  larcosCounterManager->StopButtonPressed (_stopImmediately, successful, errMsg);
  _errMsg = KKStrToSystenStr (errMsg);
  _successful = successful;
}
 



String^   LarcosCounterManagerWrapper::GetTrainingModelName ()
{
  return  KKStrToSystenStr (larcosCounterManager->TrainingModelName ());
}



void  LarcosCounterManagerWrapper::SetOperatingParameters (UmiOperatingParameters^  _operatingParameters)
{
  larcosCounterManager->SetOperatingParameters (_operatingParameters->UnManagedClass ());
}



void  LarcosCounterManagerWrapper::SetDefaultOperatingParameters (UmiOperatingParameters^  _defaultOperatingParameters)
{
  larcosCounterManager->SetDefaultOperatingParameters (_defaultOperatingParameters->UnManagedClass ());
}



void  LarcosCounterManagerWrapper::SetTrainingModel (String^                  _trainingModelName,
                                                     bool%                    _successful,
                                                     UmiOperatingParameters^  _opParms
                                                    )
{
  bool  successful       = _successful;

  // Assuming that the '_trainingModelName' is good and the respective configuration file contains
  // the parameters the "RequestedScanRate", "RequestedAnalogGain", and "RequestedDigitalGain" will
  // be updated in 'larcosCameramanager'.
  larcosCounterManager->SetTrainingModel 
        (SystemStringToKKStr (_trainingModelName),
         successful,
         _opParms->UnManagedClass ()
        );

  _successful = successful;
}



void  LarcosCounterManagerWrapper::PlayingBackRealTime (bool _PlayingBackRealTime)
{
  larcosCounterManager->PlayingBackRealTime (_PlayingBackRealTime);
}


void  LarcosCounterManagerWrapper::RequestedCameraParameters (float   requestedAnalogGain,
                                                              kkint32 requestedDigitalGain,
                                                              float   requestedScanRate
                                                             )
{
  larcosCounterManager->RequestedAnalogGain  (requestedAnalogGain);
  larcosCounterManager->RequestedDigitalGain (requestedDigitalGain);
  larcosCounterManager->RequestedScanRate    (requestedScanRate);
}


void  LarcosCounterManagerWrapper::RequestedAnalogGain (float  requestedAnalogGain)
{
  larcosCounterManager->RequestedAnalogGain (requestedAnalogGain);
}


void  LarcosCounterManagerWrapper::RequestedDigitalGain (kkint32  requestedDigitalGain)
{
  larcosCounterManager->RequestedDigitalGain (requestedDigitalGain);
}


void  LarcosCounterManagerWrapper::RequestedScanRate (float  requestedScanRate)
{
  larcosCounterManager->RequestedScanRate (requestedScanRate);
}


void  LarcosCounterManagerWrapper::RequestedSensitivityMode (String^  sensitivityMode)
{
  larcosCounterManager->RequestedSensitivityMode (SystemStringToKKStr (sensitivityMode));
}



void  LarcosCounterManagerWrapper::SampleLastFrameBeforeFlatField (bool _sampleLastFrameBeforeFlatField)
{
  larcosCounterManager->SampleLastFrameBeforeFlatField (_sampleLastFrameBeforeFlatField);
}




bool  LarcosCounterManagerWrapper::SampleLastFrameBeforeFlatField ()
{
  return  larcosCounterManager->SampleLastFrameBeforeFlatField ();
}




void  LarcosCounterManagerWrapper::ControlNumValidityCheck (String^   controlNum,
                                                            String^%  errMsg
                                                           )
{
  KKStr  unmanagedErrMsg = "";
  larcosCounterManager->ControlNumValidityCheck (UmiKKStr::SystemStringToKKStr (controlNum), unmanagedErrMsg);
  errMsg = UmiKKStr::KKStrToSystenStr (unmanagedErrMsg);
  return;
}




array<byte>^  LarcosCounterManagerWrapper::CameraHighPoints ()
{
  VectorUcharPtr  cameraHighs  = larcosCounterManager->CameraHighPoints ();
  if  (!cameraHighs)
    return nullptr;
  array<byte>^  results = gcnew array<byte> ((int)cameraHighs->size ());
  for  (uint x = 0;  x < cameraHighs->size ();  x++)
    results[x] = (*cameraHighs)[x];
  delete  cameraHighs;   cameraHighs = NULL;
  return  results;
}



array<byte>^    LarcosCounterManagerWrapper::CameraHighPointsFromLastNSampleLines (kkint32 n)
{
  VectorUcharPtr  cameraHighs  = larcosCounterManager->CameraHighPointsFromLastNSampleLines (n);
  if  (!cameraHighs)
    return nullptr;
  array<byte>^  results = gcnew array<byte> ((int)cameraHighs->size ());
  for  (uint x = 0;  x < cameraHighs->size ();  x++)
    results[x] = (*cameraHighs)[x];
  delete  cameraHighs;   cameraHighs = NULL;
  return  results;
}


bool  LarcosCounterManagerWrapper::StoppingIsInProgress ()
{
  return larcosCounterManager->StoppingIsInProgress ();
}



bool  LarcosCounterManagerWrapper::PlayingBackRealTime ()
{
  return larcosCounterManager->PlayingBackRealTime ();
}



float  LarcosCounterManagerWrapper::RequestedAnalogGain ()
{
  return larcosCounterManager->RequestedAnalogGain ();
}


int  LarcosCounterManagerWrapper::RequestedDigitalGain ()
{
  return larcosCounterManager->RequestedDigitalGain ();
}


float  LarcosCounterManagerWrapper::RequestedScanRate ()
{
  return  larcosCounterManager->RequestedScanRate ();
}


String^  LarcosCounterManagerWrapper::RequestedSensitivityMode ()
{
  return  KKStrToSystenStr (larcosCounterManager->RequestedSensitivityMode ());
}



uchar  LarcosCounterManagerWrapper::BackGroundPixelTH ()
{
  return  larcosCounterManager->BackGroundPixelTH ();
}




bool  LarcosCounterManagerWrapper::CameraAutoGainThreadRunning ()
{
  return  larcosCounterManager->CameraAutoGainThreadRunning ();
}




String^  LarcosCounterManagerWrapper::CameraMacAddress ()
{
  return  KKStrToSystenStr (larcosCounterManager->CameraMacAddress ());
}


CameraParametersManaged^  LarcosCounterManagerWrapper::CameraParams ()
{
  return gcnew CameraParametersManaged (larcosCounterManager->CameraParams ());
}


String^  LarcosCounterManagerWrapper::CameraSerialNum ()
{
  return  KKStrToSystenStr (larcosCounterManager->CameraSerialNum ());
}


bool  LarcosCounterManagerWrapper::CameraThreadRunning ()
{
  return  larcosCounterManager->CameraThreadRunning ();
}



void  LarcosCounterManagerWrapper::ResetCounts  ()
{
  larcosCounterManager->ResetCounts ();
}



int   LarcosCounterManagerWrapper::ParticlesCounted ()
{
  return  larcosCounterManager->ParticlesCounted ();
}  /* ParticlesCounted */



float  LarcosCounterManagerWrapper::CurScanRate ()
{
  return  larcosCounterManager->CurScanRate ();
}


kkint32 LarcosCounterManagerWrapper::SnapshotInterval ()
{
  return  larcosCounterManager->SnapshotInterval ();
}


void  LarcosCounterManagerWrapper::SnapshotInterval (kkint32 _snapshotInterval)
{
  larcosCounterManager->SnapshotInterval (_snapshotInterval);
}




kkint64   LarcosCounterManagerWrapper::TotalBytesRead  ()
{
  return larcosCounterManager->TotalBytesRead ();
}


kkint64   LarcosCounterManagerWrapper::TotalBytesToRead  ()
{
  return larcosCounterManager->TotalBytesToRead ();
}


void  LarcosCounterManagerWrapper::GetStatistics (float%  frameRate,               float%  scanRate,
                                                  Int64%  bytesWritten,            Int32%  waitingToWriteToDisk,
                                                  Int32%  physicalSeqNumsSkipped,  Int32%  diskWritingSeqNumBreaks,
                                                  Int32%  totalLostPackets,        Int32%  scanLinesWritten,
                                                  Int32%  physicalFramesRead,      Int32%  physicalFramesDropped,
                                                  Int32%  physicalFramesWaitingToProcess,  Int32%  physicalFramesProcessed,
                                                  Int32%  particlesExtracted,      Int32%  particlesWaitingProcessing,
                                                  Int32%  particleCount
                                                 )
{
  if  (!larcosCounterStats)
    larcosCounterStats = new LarcosCounterStats ();
  larcosCounterManager->GetStats (*larcosCounterStats);

  bytesWritten                    = larcosCounterStats->BytesWritten               ();
  frameRate                       = larcosCounterStats->FrameRate                  ();
  physicalFramesRead              = larcosCounterStats->PhysicalFramesRead         ();
  physicalFramesDropped           = larcosCounterStats->PhysicalFramesDropped      ();
  physicalFramesWaitingToProcess  = larcosCounterStats->PhysicalFramesWaitingToProcess     ();
  particlesExtracted              = larcosCounterStats->ParticlesExtracted         ();
  particlesWaitingProcessing      = larcosCounterStats->ParticlesWaitingProcessing ();
  particleCount                   = larcosCounterStats->ParticlesCounted           ();
  physicalFramesProcessed         = larcosCounterStats->PhysicalFramesProcessed    ();
  physicalSeqNumsSkipped          = larcosCounterStats->PhysicalSeqNumsSkipped     ();
  scanRate                        = larcosCounterStats->ScanRate                   ();
  scanLinesWritten                = larcosCounterStats->ScanLinesWritten           ();
  totalLostPackets                = larcosCounterStats->TotalLostPackets           ();
  waitingToWriteToDisk            = larcosCounterStats->WaitingToWriteToDisk       ();
}  /* GetStatistics */



void  LarcosCounterManagerWrapper::GetGainSettings (bool%   _autoGainRunning,
                                                    float%  _analogGain,
                                                    kkint32%  _digitalGain
                                                   )
{
  _autoGainRunning = larcosCounterManager->CameraAutoGainThreadRunning ();
  _analogGain      = larcosCounterManager->CurAnalogGain ();
  _digitalGain     = larcosCounterManager->CurDigitalGain ();
}



String^  LarcosCounterManagerWrapper::GetNextControlNumber ()
{
  return UmiKKStr::KKStrToSystenStr (larcosCounterManager->GetNextControlNumber ());
}


LarcosCounterStatsManaged^  LarcosCounterManagerWrapper::GetStats ()
{
  if  (!larcosCounterStats)
    larcosCounterStats = new LarcosCounterStats ();

  larcosCounterManager->GetStats (*larcosCounterStats);
  return gcnew LarcosCounterStatsManaged (larcosCounterStats);
}



LarcosCounterStatsManaged^  LarcosCounterManagerWrapper::GetFinaleStats ()
{
  const LarcosCounterStatsPtr  finaleStats = larcosCounterManager->GetFinaleStats ();
  return gcnew LarcosCounterStatsManaged (finaleStats);
}



kkint32  LarcosCounterManagerWrapper::GetLastSessionId ()
{
  return  larcosCounterManager->GetLastSessionId ();
}



UmiSessionParameters^  LarcosCounterManagerWrapper::GetSessionParameters ()
{
  SessionParametersPtr  sessionParms = new SessionParameters ();
  larcosCounterManager->GetSessionParameters (*sessionParms);
  return gcnew UmiSessionParameters (sessionParms);
}




UmiOperatingParameters^  LarcosCounterManagerWrapper::GetOperatingParameters ()
{
  OperatingParametersPtr  opParms = new OperatingParameters ();
  larcosCounterManager->GetOperatingParameters (*opParms);
  return gcnew UmiOperatingParameters (opParms);
}





UmiOperatingParameters^  LarcosCounterManagerWrapper::GetDefaultOperatingParameters ()
{
  OperatingParametersPtr  opParms = new OperatingParameters ();
  larcosCounterManager->GetDefaultOperatingParameters (*opParms);
  return gcnew UmiOperatingParameters (opParms);
}



String^  LarcosCounterManagerWrapper::DropFolderToLarcos ()
{
  return  UmiKKStr::KKStrToSystenStr (larcosCounterManager->DropFolderToLarcos ());
}



bool  LarcosCounterManagerWrapper::DiskWritingThreadRunning ()
{
  return  larcosCounterManager->DiskWritingThreadRunning ();
}


bool  LarcosCounterManagerWrapper::EmbeddedFlowMeter ()
{
  return  larcosCounterManager->EmbeddedFlowMeter ();
}



int  LarcosCounterManagerWrapper::FrameProcessorsCount ()
{
  return  larcosCounterManager->FrameProcessorsCount ();
}



bool  LarcosCounterManagerWrapper::GenerateFinaleReport ()
{
  return  larcosCounterManager->GenerateFinaleReport ();
}


bool  LarcosCounterManagerWrapper::IsRecording ()
{
  return  larcosCounterManager->IsRecording ();
}


bool  LarcosCounterManagerWrapper::IsRunning ()
{
  return  larcosCounterManager->IsRunning ();
}



String^  LarcosCounterManagerWrapper::LastScannerFileCounted ()
{
  return  KKStrToSystenStr (larcosCounterManager->LastScannerFileCounted ());
}


void  LarcosCounterManagerWrapper::LastScannerFileCounted (String^  _lastScannerFileCounted)
{
  larcosCounterManager->LastScannerFileCounted (SystemStringToKKStr (_lastScannerFileCounted));
}



String^  LarcosCounterManagerWrapper::NameLastRecordedFile ()
{
  return  KKStrToSystenStr (larcosCounterManager->NameLastRecordedFile ());
}


void  LarcosCounterManagerWrapper::NameLastRecordedFile (String^  _nameLastRecordedFile)
{
  larcosCounterManager->NameLastRecordedFile (SystemStringToKKStr (_nameLastRecordedFile));
}




void  LarcosCounterManagerWrapper::UpdateUnManagedConsumedMemory ()
{
  kkint64  unManagedMemoryInUse = larcosCounterManager->MemoryConsumedEstimated ();
  kkint64  deltaMemory = unManagedMemoryInUse - lastReportedMemoryPreasure;

  if  (deltaMemory > 0)
      GC::AddMemoryPressure (deltaMemory);
  else if  (deltaMemory < 0)
    GC::RemoveMemoryPressure (0 - deltaMemory);

  lastReportedMemoryPreasure = unManagedMemoryInUse;
}  /* UpdateUnManagedConsumedMemory */




void  LarcosCounterManagerWrapper::ShutDownAllThreads ()
{
  larcosCounterManager->ShutDownAllThreads ();
}  /* CancelProcesses */




//  Will stay in a loop until all threads that this object controls are stopped.
void  LarcosCounterManagerWrapper::TerminateAllThreads ()
{
  larcosCounterManager->TerminateAllThreads ();
}  /* StopAllThreads*/



void  LarcosCounterManagerWrapper::AddMsg (String^ msg)
{
  if  (msg == nullptr)
    return;
  larcosCounterManager->AddMsg (UmiKKStr::SystemStringToKKStrPtr (msg));
}



List<String^>^  LarcosCounterManagerWrapper::GetAllLoggedMsgs ()
{
  KKStrListPtr  msgs = larcosCounterManager->GetAllLoggedMsgs ();
  if  (!msgs)
    return nullptr;

  List<String^>^  results = gcnew List<String^> ();
  KKStrPtr  msg = msgs->PopFromFront();
  while  (msg)
  {
    results->Add (KKStrToSystenStr (*msg));
    delete  msg;
    msg = msgs->PopFromFront ();
  }
  delete  msgs;  msgs = NULL;
  return results;
}  /* GetAllLoggedMsgs */



UmiRunLog^  LarcosCounterManagerWrapper::GetUmiRunLogInstance ()
{
  return gcnew UmiRunLog (msgQueue);
}



Bitmap^  LarcosCounterManagerWrapper::GetLastParticle (kkint32   targetHeight,
                                                       kkint32   targetWidth,
                                                       String^%  classLabel
                                                      )
{
  Bitmap^ bm = nullptr;
  classLabel = nullptr;
  RasterPtr  lastParticle = larcosCounterManager->GetLastParticle ();
  if  (lastParticle)
  {
    bm = BuildBitmap (lastParticle, targetHeight, targetWidth);
    classLabel = KKStrToSystenStr (lastParticle->Title ());
    delete  lastParticle;
    lastParticle = NULL;
  }
  return bm;
}  /* GetLastParticle */





StatusSnapshotManagedList^  LarcosCounterManagerWrapper::SnapshotsRetrieve (System::DateTime  rangeStart,
                                                                            System::DateTime  rangeEnd
                                                                           )
{
  StatusSnapshotManagedList^  snapshotsManaged = nullptr;

  StatusSnapshotListPtr  snapshots 
    = larcosCounterManager->SnapshotsRetrieve (StatusSnapshotManaged::DateTimeSystemToKKB (rangeStart), 
                                               StatusSnapshotManaged::DateTimeSystemToKKB (rangeEnd)
                                              );
  if  (snapshots)
  {
    snapshotsManaged = gcnew StatusSnapshotManagedList (*snapshots);
    delete snapshots;
    snapshots = NULL;
  }

  return snapshotsManaged;
}



StatusSnapshotManaged^  LarcosCounterManagerWrapper::SnapshotLastEntry ()
{
  StatusSnapshotManaged^  snapshotManaged = nullptr;

  const StatusSnapshotPtr  snapshot =  larcosCounterManager->SnapshotLastEntry ();
  if  (snapshot)
    snapshotManaged = gcnew StatusSnapshotManaged (*snapshot);

  return snapshotManaged;
}  /* SnapshotLastEntry */



Bitmap^  LarcosCounterManagerWrapper::SnapShotLatestFrame (kkint32  targetHeight,
                                                           kkint32  targetWidth
                                                          )
{
  Bitmap^ bm = nullptr;
  RasterPtr  lastFrame = larcosCounterManager->SnapShotLatestFrame ();
  if  (lastFrame)
  {
    //bm = BuildBitmapFillTargetWidth (lastFrame, targetHeight, targetWidth);
    bm = BuildBitmapFillTargetWidth2 (lastFrame, targetHeight, targetWidth, (lastFrame->BackgroundPixelValue () == 0));
    delete  lastFrame;
    lastFrame = NULL;
  }
  return bm;
}  /* SnapShotLatestFrame */





UmiRaster^  LarcosCounterManagerWrapper::SnapShotLatestFrame ()
{
  RasterPtr  lastFrame = larcosCounterManager->SnapShotLatestFrame ();
  if  (lastFrame == NULL)
    return nullptr;
  else
    return gcnew UmiRaster (lastFrame);
}




void  LarcosCounterManagerWrapper::SetLiveVideoDimensions (kkint32  _liveVideoHeight,
                                                           kkint32  _liveVideoWidth
                                                          )
{
  larcosCounterManager->SetLiveVideoDimensions (_liveVideoHeight, _liveVideoWidth);
}




void  LarcosCounterManagerWrapper::BackGroundPixelTH (uchar  _backGroundPixelTH)
{
  larcosCounterManager->BackGroundPixelTH (_backGroundPixelTH);
}



int  LarcosCounterManagerWrapper::CropLeft ()
{
  return  larcosCounterManager->CropLeft ();
}


int  LarcosCounterManagerWrapper::CropRight ()
{
  return  larcosCounterManager->CropRight ();
}



void  LarcosCounterManagerWrapper::CropLeft (int _cropLeft)
{
  larcosCounterManager->CropSettingsChanged (_cropLeft, larcosCounterManager->CropRight ());
}



void  LarcosCounterManagerWrapper::CropRight (int _cropRight)
{
  larcosCounterManager->CropSettingsChanged (larcosCounterManager->CropLeft (), _cropRight);
}



int   LarcosCounterManagerWrapper::FrameWidthInPixels ()
{
  return  larcosCounterManager->FrameWidthInPixels ();
}



void  LarcosCounterManagerWrapper::CropSettingsChanged (int _cropLeft,
                                                        int _cropRight
                                                       )
{
  larcosCounterManager->CropSettingsChanged (_cropLeft, _cropRight);
}


float  LarcosCounterManagerWrapper::CurAnalogGain ()
{
  return larcosCounterManager->CurAnalogGain ();
}



int  LarcosCounterManagerWrapper::CurDigitalGain ()
{
  return larcosCounterManager->CurDigitalGain ();
}



bool  LarcosCounterManagerWrapper::FlatFieldEnabled ()  
{
  return  larcosCounterManager->FlatFieldEnabled ();
}


void  LarcosCounterManagerWrapper::FlatFieldEnabled (bool  _flatFieldEnabled)
{
  larcosCounterManager->FlatFieldEnabled (_flatFieldEnabled);
}


int   LarcosCounterManagerWrapper::MinSizeThreshold ()
{
  return  larcosCounterManager->MinSizeThreshold ();
}


void  LarcosCounterManagerWrapper::MinSizeThreshold (int  _minSizeThreshold)
{
  larcosCounterManager->MinSizeThreshold (_minSizeThreshold);
}


bool  LarcosCounterManagerWrapper::DataIsToBeCounted ()
{
  return  larcosCounterManager->DataIsToBeCounted ();
}


bool  LarcosCounterManagerWrapper::DataIsToBeRecorded ()
{
  return  larcosCounterManager->DataIsToBeRecorded ();
}



Bitmap^  LarcosCounterManagerWrapper::BuildBitmap (RasterPtr  raster,
                                                   kkint32    targetHeight,
                                                   kkint32    targetWidth
                                                  )
{
  RasterPtr  targetRaster = raster;
  bool  deleteTargetRaster = false;

  if  ((targetHeight > 0)  &&  (targetWidth > 0))
  {
    float  ratioHeight = 1.0f;
    float  ratioWidth  = 1.0f;
    if  (raster->Height () >= targetHeight)
      ratioHeight = (float)(targetHeight + 1) / (float)raster->Height ();

    if  (raster->Width () >= targetWidth)
      ratioWidth = (float)(targetWidth) / (float)(raster->Width ());

    float ratio = Min (ratioHeight, ratioWidth);
    if  (ratio < 1.0f)
    {
      targetRaster = raster->ReduceByFactor (ratio);
      deleteTargetRaster = true;
    }
  }

  int  rowFirst = 99999;
  int  rowLast  = -1;
  int  colFirst = 99999;
  int  colLast  = -1;

  int  width  = targetRaster->Width ();
  int  height = targetRaster->Height ();

  bool  color = targetRaster->Color ();

  uchar**  red   = NULL;
  uchar**  green = targetRaster->Green ();
  uchar**  blue  = NULL;

  if  (color)
  {
    red   = targetRaster->Red   ();
    blue  = targetRaster->Blue  ();
  }
  else
  {
    red  = green;
    blue = green;
  }

  Bitmap^  image = gcnew Bitmap (width, height, System::Drawing::Imaging::PixelFormat::Format24bppRgb);
    
  // GDI+ return format is BGR, NOT RGB. 
  System::Drawing::Imaging::BitmapData^ bmData 
    = image->LockBits (System::Drawing::Rectangle (0, 0, width, height),
                       System::Drawing::Imaging::ImageLockMode::ReadWrite,
                       image->PixelFormat
                      );
  int stride = bmData->Stride;
  System::IntPtr  Scan0 = bmData->Scan0;

  {
    int  nOffset = stride - width * 3;
    int  bytesPerRow = width * 3 + nOffset;
    int  col = 0;
    int  row = 0;
    byte greenValue = 0;

    byte* ptr = (byte*)(void*)Scan0;

    for  (row = 0;  row < height;  row++)
    {
      uchar*  redDataRow   = red  [row];
      uchar*  greenDataRow = green[row];
      uchar*  blueDataRow  = blue [row];

      for (col = 0;  col < width;  col++)
      {
        if  (color)
        {
          ptr[0] = blueDataRow  [col];   ptr++;    // kak
          ptr[0] = greenDataRow [col];   ptr++;
          ptr[0] = redDataRow   [col];   ptr++;
        }
        else
        {
          greenValue = 255 - greenDataRow[col];
          ptr[0] = greenValue;  ptr++;
          ptr[0] = greenValue;  ptr++;
          ptr[0] = greenValue;  ptr++;
        }
      }

      ptr += nOffset;
    }
  }   /* Unsafe */
  image->UnlockBits (bmData);

  if  (deleteTargetRaster)
    delete targetRaster;
  targetRaster = NULL;

  return  image;
}  /* BuildBitmap */




Bitmap^  LarcosCounterManagerWrapper::BuildBitmapFillTargetWidth (RasterPtr  raster,
                                                                  kkint32    targetHeight,
                                                                  kkint32    targetWidth
                                                                 )
{
  RasterPtr  targetRaster = raster;
  bool  deleteTargetRaster = false;

  if  (targetWidth > 0)
  {
    float  ratioWidth  = 1.0f;
    if  (raster->Width () >= targetWidth)
      ratioWidth = (float)(targetWidth) / (float)(raster->Width ());

    if  (ratioWidth < 1.0f)
    {
      targetRaster = raster->ReduceByFactor (ratioWidth);
      deleteTargetRaster = true;
    }
  }
  
  int  bitMapHeght = Min (targetHeight, targetRaster->Height ());

  int  rowFirst = 99999;
  int  rowLast  = -1;
  int  colFirst = 99999;
  int  colLast  = -1;

  int  width  = targetRaster->Width ();
  int  height = bitMapHeght;

  bool  color = targetRaster->Color ();

  uchar**  red   = NULL;
  uchar**  green = targetRaster->Green ();
  uchar**  blue  = NULL;

  if  (color)
  {
    red   = targetRaster->Red   ();
    blue  = targetRaster->Blue  ();
  }
  else
  {
    red  = green;
    blue = green;
  }

  Bitmap^  image = gcnew Bitmap (width, bitMapHeght, System::Drawing::Imaging::PixelFormat::Format24bppRgb);
    
  // GDI+ return format is BGR, NOT RGB. 
  System::Drawing::Imaging::BitmapData^ bmData 
    = image->LockBits (System::Drawing::Rectangle (0, 0, width, bitMapHeght),
                       System::Drawing::Imaging::ImageLockMode::ReadWrite,
                       image->PixelFormat
                      );
  int stride = bmData->Stride;
  System::IntPtr  Scan0 = bmData->Scan0;

  {
    int  nOffset = stride - width * 3;
    int  bytesPerRow = width * 3 + nOffset;
    int  col = 0;
    int  row = 0;
    byte greenValue = 0;

    byte* ptr = (byte*)(void*)Scan0;

    for  (row = 0;  row < bitMapHeght;  row++)
    {
      uchar*  redDataRow   = red  [row];
      uchar*  greenDataRow = green[row];
      uchar*  blueDataRow  = blue [row];

      for (col = 0;  col < width;  col++)
      {
        if  (color)
        {
          ptr[0] = blueDataRow  [col];   ptr++;    // kak
          ptr[0] = greenDataRow [col];   ptr++;
          ptr[0] = redDataRow   [col];   ptr++;
        }
        else
        {
          //greenValue = 255 - greenDataRow[col];
          greenValue = greenDataRow[col];
          ptr[0] = greenValue;  ptr++;
          ptr[0] = greenValue;  ptr++;
          ptr[0] = greenValue;  ptr++;
        }
      }

      ptr += nOffset;
    }
  }   /* Unsafe */
  image->UnlockBits (bmData);

  if  (deleteTargetRaster)
    delete targetRaster;
  targetRaster = NULL;

  return  image;
}  /* BuildBitmapFillTargetWidths */








Bitmap^  LarcosCounterManagerWrapper::BuildBitmapFillTargetWidth2 (RasterPtr  raster,
                                                                   kkint32    targetHeight,
                                                                   kkint32    targetWidth,
                                                                   bool       complementData
                                                                  )
{
  RasterPtr  targetRaster = raster;
  bool  deleteTargetRaster = false;

  if  (targetHeight < 0)
    return nullptr;

  if  (targetWidth > 0)
  {
    float  ratioWidth  = 1.0f;
    if  (raster->Width () >= targetWidth)
      ratioWidth = (float)(targetWidth) / (float)(raster->Width ());

    if  (ratioWidth < 1.0f)
    {
      targetRaster = raster->ReduceByFactor (ratioWidth);
      deleteTargetRaster = true;
    }
  }
  
  int  bitMapHeght = Min (targetHeight, targetRaster->Height ());

  int  rowFirst = 99999;
  int  rowLast  = -1;
  int  colFirst = 99999;
  int  colLast  = -1;

  int  width  = targetRaster->Width ();
  int  height = bitMapHeght;

  bool  color = targetRaster->Color ();

  uchar**  red   = NULL;
  uchar**  green = targetRaster->Green ();
  uchar**  blue  = NULL;

  if  (color)
  {
    red   = targetRaster->Red   ();
    blue  = targetRaster->Blue  ();
  }
  else
  {
    red  = green;
    blue = green;
  }

  int  zed = 0;
  Bitmap^  image = gcnew Bitmap (width, bitMapHeght, System::Drawing::Imaging::PixelFormat::Format8bppIndexed);

  System::Drawing::Imaging::ColorPalette^  pal = image->Palette;
  for  (int xx = 0, yy = 255;  xx < 256;  ++xx, --yy)
  {
    zed = complementData ? (255 - xx) : xx;
    pal->Entries[xx] = Color::FromArgb (zed, zed, zed);
  }
  image->Palette = pal;
    
  // GDI+ return format is BGR, NOT RGB. 
  System::Drawing::Imaging::BitmapData^ bmData 
    = image->LockBits (System::Drawing::Rectangle (0, 0, width, bitMapHeght),
                       System::Drawing::Imaging::ImageLockMode::ReadWrite,
                       image->PixelFormat
                      );
  int stride = bmData->Stride;
  System::IntPtr  Scan0 = bmData->Scan0;

  {
    int  nOffset = stride - width;
    int  bytesPerRow = width + nOffset;
    int  col = 0;
    int  row = 0;
    byte greenValue = 0;

    byte* ptr = (byte*)(void*)Scan0;

    for  (row = 0;  row < bitMapHeght;  row++)
    {
      uchar*  greenDataRow = green[row];

      for (col = 0;  col < width;  col++)
      {
        //greenValue = 255 - greenDataRow[col];
        greenValue = greenDataRow[col];
        ptr[0] = greenValue;  ptr++;
      }

      ptr += nOffset;
    }
  }   /* Unsafe */
  image->UnlockBits (bmData);

  if  (deleteTargetRaster)
    delete targetRaster;
  targetRaster = NULL;

  return  image;
}  /* BuildBitmapFillTargetWidths2 */







void  LarcosCounterManagerWrapper::Status (System::String^% _statusMsg,
                                           System::String^% _color,
                                           System::String^% _secondaryMsg,
                                           float%           _analogGain,
                                           Int32%           _digitalGain,
                                           Int32%           _cameraTemparature,
                                           float%           _flowRate
                                          )
{
  KKStr  statusMsg;
  KKStr  color;
  KKStr  secondaryMsg;

  float analogGain  = _analogGain;
  kkint32 digitalGain = _digitalGain;
  kkint32 cameraTemparature = -1;
  float flowRate = 1.0f;

  if  (larcosCounterManager)
  {
    larcosCounterManager->Status (statusMsg, color, secondaryMsg, analogGain, digitalGain, cameraTemparature, flowRate);
  }

  _statusMsg         = KKStrToSystenStr (statusMsg);
  _color             = KKStrToSystenStr (color);
  _secondaryMsg      = KKStrToSystenStr (secondaryMsg);
  _analogGain        = analogGain;
  _digitalGain       = digitalGain;
  _cameraTemparature = cameraTemparature;
  _flowRate          = flowRate;
  return;
}