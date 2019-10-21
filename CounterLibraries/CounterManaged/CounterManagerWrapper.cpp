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
#include "CounterStatsManaged.h"
#include "UmiKKStr.h"
#include "CounterManagerWrapper.h"
using  namespace  CounterManaged;


using namespace System;
using namespace System::Threading;
//using namespace System::Windows::Forms;



CounterManagerWrapper::CounterManagerWrapper ():
    counterManager             (NULL),
    counterStats               (NULL),
    lastReportedMemoryPreasure (0),
    loggedMsgs                 (NULL),
    msgQueue                   (),
    saveDebugImages            (false)
{
  CounterBase::CounterVariables::SetCounterHomeDir ();

  loggedMsgs = new MsgQueue ("CounterManagerWrapper::LoggedMsgs");
  msgQueue = new MsgQueue ("CounterManagerWrapper");

  int maxNumOfThreads = System::Environment::ProcessorCount;
  counterManager = new CounterManager (msgQueue, loggedMsgs, maxNumOfThreads);
  counterManager->SaveDebugImages (saveDebugImages);
  UpdateUnManagedConsumedMemory ();
}



CounterManagerWrapper::!CounterManagerWrapper ()
{
  CleanUpMemory ();
}



CounterManagerWrapper::~CounterManagerWrapper ()
{
  this->!CounterManagerWrapper ();
}



void  CounterManagerWrapper::Initialize ()
{
  counterManager->Initialize ();
}



void  CounterManagerWrapper::SaveDebugImages (bool _saveDebugImages)
{
  saveDebugImages = _saveDebugImages;
  counterManager->SaveDebugImages (saveDebugImages);
}



String^  CounterManagerWrapper::CounterStateToStr (CounterState  state)
{

  CounterManager::CounterState  unmanagedState = (CounterManager::CounterState)state;

  KKStr counterStateStr = CounterManager::CounterStateToStr (unmanagedState);
  return  KKStrToSystenStr (counterStateStr);
}



void  CounterManagerWrapper::CleanUpMemory ()
{
  delete  counterManager;         counterManager = NULL;
  delete  counterStats;     counterStats   = NULL;
  delete  msgQueue;               msgQueue             = NULL;
  delete  loggedMsgs;             loggedMsgs           = NULL;

  if  (lastReportedMemoryPreasure > 0)
  {
    GC::RemoveMemoryPressure (lastReportedMemoryPreasure);
    lastReportedMemoryPreasure = 0;
  }
}



float  CounterManagerWrapper::ImagingChamberWidth ()
{
  return counterManager->ImagingChamberWidth ();
}



int  CounterManagerWrapper::ImagingChamberWidthPixels ()
{
  return counterManager->ImagingChamberWidthPixels ();
}



float  CounterManagerWrapper::ImagingChamberPixelsPerMM ()
{
  return counterManager->ImagingChamberPixelsPerMM ();
}



KKStr  CounterManagerWrapper::SystemStringToKKStr (System::String^  s)
{
  if  (s == nullptr)
    return KKStr::EmptyStr ();

  KKB::KKStr  kkStr (s->Length);
  for  (int x = 0;  x < s->Length;  x++)
    kkStr.Append ((char)s[x]);
  return  kkStr;
}



System::String^  CounterManagerWrapper::KKStrToSystenStr (const KKStr&  s)
{
  System::String^  sysStr = gcnew System::String (s.Str ());
  return  sysStr;
}



String^  CounterManagerWrapper::CounterVersionNumber ()
{
  return UmiKKStr::KKStrToSystenStr (counterManager->CounterVersionNumber ());
}



UmiCounterOperatingModes  CounterManagerWrapper::OperatingMode ()
{
  return  (UmiCounterOperatingModes)(counterManager->OperatingMode ());
}



UmiInstallationConfig^  CounterManagerWrapper::Installation (UmiRunLog^  runLog)
{
  const InstallationConfigPtr  installation = counterManager->Installation ();
  if  (installation == NULL)
    return  nullptr;

  if  (runLog == nullptr)
    runLog = gcnew UmiRunLog ();


  InstallationConfigPtr  newInstallation = new InstallationConfig (*installation, runLog->Log ());
  
  return gcnew UmiInstallationConfig (newInstallation);
}



UmiCounterState  CounterManagerWrapper::CurState ()  
{
  return  (UmiCounterState)(counterManager->CurState ());
}



CounterManagerWrapper::StatusSnapshotDataFieldIdx   CounterManagerWrapper::ThroughPutField ()
{
  return  (StatusSnapshotDataFieldIdx)(counterManager->ThroughPutField ());
}



void  CounterManagerWrapper::ThroughPutField (StatusSnapshotDataFieldIdx _throughPutField)
{
  counterManager->ThroughPutField ((StatusSnapshot::FieldIdx)_throughPutField);
}



CameraParametersManagedList^  CounterManagerWrapper::GetCameraList ()
{
  CameraParametersManagedList^  results = nullptr;

  CameraParametersListPtr  cameras = CounterManager::GetCameraList (NULL);
  if  (cameras)
    results = gcnew CameraParametersManagedList (*cameras);

  // The managed Constructor 'CameraParametersManagedList' will make its own copy of the camera data;  that
  // is it is not taking ownership of the contents of 'cameras'.
  delete  cameras;
  cameras = NULL;
  return  results;
}



CameraParametersManaged^  CounterManagerWrapper::PromptForCamera (UmiMsgQueue^  msgQueue)
{
  if  (msgQueue == nullptr)
    throw gcnew Exception ("CounterManagerWrapper::PromptForCamera    msgQueue == NULL");

  RunLog  runLog (msgQueue->MsgQueue ());
  runLog.SetLevel (CounterVariables::DebugLevel ());

  CameraParametersPtr  parameters = CounterManager::PromptForCamera (runLog);
  if  (parameters)
    return gcnew CameraParametersManaged (parameters);
  else
    return nullptr;
}



void  CounterManagerWrapper::AutoGainButtonPressed (bool%     _successful,
                                                          String^%  _errMsg
                                                         )
{
  bool  successful = false;
  KKStr  errMsg = "";

  counterManager->AutoGainButtonPressed (successful, errMsg);

  _errMsg = KKStrToSystenStr (errMsg);
  _successful = successful;
}



void  CounterManagerWrapper::ConnectButtonPressed ()
{
  counterManager->ConnectButtonPressed ();
}



void  CounterManagerWrapper::PlayBackButtonPressed 
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

  counterManager->PlayBackButtonPressed 
                         (srcScannerFileName,
                          _sessionParameters->UnManagedClass (),
                          _operatingParameters->UnManagedClass (),
                          successful,
                          errMsg
                         );

  _errMsg = KKStrToSystenStr (errMsg);
  _successful = successful;
}  /* PlayBackButtonPressed */



void  CounterManagerWrapper::RecordButtonPressed 
                         (UmiSessionParameters^   _sessionParameters,
                          UmiOperatingParameters^ _operatingParameters,
                          bool%                   _successful,
                          String^%                _errMsg
                         )
{
  bool   successful = _successful;
  KKStr  errMsg;

  counterManager->RecordButtonPressed 
                         (_sessionParameters->UnManagedClass (),
                          _operatingParameters->UnManagedClass (),
                          successful,
                          errMsg
                         );

  _errMsg = KKStrToSystenStr (errMsg);
  _successful = successful;
}



void  CounterManagerWrapper::StopButtonPressed (bool      _stopImmediately,
                                                bool%     _successful,
                                                String^%  _errMsg
                                               )
{
  bool  successful = _successful;
  KKStr  errMsg = SystemStringToKKStr (_errMsg);
  counterManager->StopButtonPressed (_stopImmediately, successful, errMsg);
  _errMsg = KKStrToSystenStr (errMsg);
  _successful = successful;
}
 



String^   CounterManagerWrapper::GetTrainingModelName ()
{
  return  KKStrToSystenStr (counterManager->TrainingModelName ());
}



void  CounterManagerWrapper::SetOperatingParameters (UmiOperatingParameters^  _operatingParameters)
{
  counterManager->SetOperatingParameters (_operatingParameters->UnManagedClass ());
}



void  CounterManagerWrapper::SetDefaultOperatingParameters (UmiOperatingParameters^  _defaultOperatingParameters)
{
  counterManager->SetDefaultOperatingParameters (_defaultOperatingParameters->UnManagedClass ());
}



void  CounterManagerWrapper::SetTrainingModel (String^                  _trainingModelName,
                                               bool%                    _successful,
                                               UmiOperatingParameters^  _opParms
                                              )
{
  bool  successful = _successful;

  // Assuming that the '_trainingModelName' is good and the respective configuration file contains
  // the parameters the "RequestedScanRate", "RequestedAnalogGain", and "RequestedDigitalGain" will
  // be updated in 'CounterManager'.
  counterManager->SetTrainingModel 
        (SystemStringToKKStr (_trainingModelName),
         successful,
         _opParms->UnManagedClass ()
        );

  _successful = successful;
}



void  CounterManagerWrapper::PlayingBackRealTime (bool _PlayingBackRealTime)
{
  counterManager->PlayingBackRealTime (_PlayingBackRealTime);
}



void  CounterManagerWrapper::RequestedCameraParameters (float   requestedAnalogGain,
                                                              kkint32 requestedDigitalGain,
                                                              float   requestedScanRate
                                                             )
{
  counterManager->RequestedAnalogGain  (requestedAnalogGain);
  counterManager->RequestedDigitalGain (requestedDigitalGain);
  counterManager->RequestedScanRate    (requestedScanRate);
}



void  CounterManagerWrapper::RequestedAnalogGain (float  requestedAnalogGain)
{
  counterManager->RequestedAnalogGain (requestedAnalogGain);
}


void  CounterManagerWrapper::RequestedDigitalGain (kkint32  requestedDigitalGain)
{
  counterManager->RequestedDigitalGain (requestedDigitalGain);
}



void  CounterManagerWrapper::RequestedScanRate (float  requestedScanRate)
{
  counterManager->RequestedScanRate (requestedScanRate);
}



void  CounterManagerWrapper::RequestedSensitivityMode (String^  sensitivityMode)
{
  counterManager->RequestedSensitivityMode (SystemStringToKKStr (sensitivityMode));
}



void  CounterManagerWrapper::SampleLastFrameBeforeFlatField (bool _sampleLastFrameBeforeFlatField)
{
  counterManager->SampleLastFrameBeforeFlatField (_sampleLastFrameBeforeFlatField);
}



bool  CounterManagerWrapper::SampleLastFrameBeforeFlatField ()
{
  return  counterManager->SampleLastFrameBeforeFlatField ();
}



void  CounterManagerWrapper::ControlNumValidityCheck (String^   controlNum,
                                                            String^%  errMsg
                                                           )
{
  KKStr  unmanagedErrMsg = "";
  counterManager->ControlNumValidityCheck (UmiKKStr::SystemStringToKKStr (controlNum), unmanagedErrMsg);
  errMsg = UmiKKStr::KKStrToSystenStr (unmanagedErrMsg);
  return;
}



array<byte>^  CounterManagerWrapper::CameraHighPoints ()
{
  VectorUcharPtr  cameraHighs  = counterManager->CameraHighPoints ();
  if  (!cameraHighs)
    return nullptr;
  array<byte>^  results = gcnew array<byte> ((int)cameraHighs->size ());
  for  (uint x = 0;  x < cameraHighs->size ();  x++)
    results[x] = (*cameraHighs)[x];
  delete  cameraHighs;   cameraHighs = NULL;
  return  results;
}



array<byte>^    CounterManagerWrapper::CameraHighPointsFromLastNSampleLines (kkint32 n)
{
  VectorUcharPtr  cameraHighs  = counterManager->CameraHighPointsFromLastNSampleLines (n);
  if  (!cameraHighs)
    return nullptr;
  array<byte>^  results = gcnew array<byte> ((int)cameraHighs->size ());
  for  (uint x = 0;  x < cameraHighs->size ();  x++)
    results[x] = (*cameraHighs)[x];
  delete  cameraHighs;   cameraHighs = NULL;
  return  results;
}



bool  CounterManagerWrapper::StoppingIsInProgress ()
{
  return counterManager->StoppingIsInProgress ();
}



bool  CounterManagerWrapper::PlayingBackRealTime ()
{
  return counterManager->PlayingBackRealTime ();
}



float  CounterManagerWrapper::RequestedAnalogGain ()
{
  return counterManager->RequestedAnalogGain ();
}



int  CounterManagerWrapper::RequestedDigitalGain ()
{
  return counterManager->RequestedDigitalGain ();
}



float  CounterManagerWrapper::RequestedScanRate ()
{
  return  counterManager->RequestedScanRate ();
}


String^  CounterManagerWrapper::RequestedSensitivityMode ()
{
  return  KKStrToSystenStr (counterManager->RequestedSensitivityMode ());
}



uchar  CounterManagerWrapper::BackGroundPixelTH ()
{
  return  counterManager->BackGroundPixelTH ();
}



bool  CounterManagerWrapper::CameraAutoGainThreadRunning ()
{
  return  counterManager->CameraAutoGainThreadRunning ();
}



String^  CounterManagerWrapper::CameraMacAddress ()
{
  return  KKStrToSystenStr (counterManager->CameraMacAddress ());
}



CameraParametersManaged^  CounterManagerWrapper::CameraParams ()
{
  return gcnew CameraParametersManaged (counterManager->CameraParams ());
}



String^  CounterManagerWrapper::CameraSerialNum ()
{
  return  KKStrToSystenStr (counterManager->CameraSerialNum ());
}



bool  CounterManagerWrapper::CameraThreadRunning ()
{
  return  counterManager->CameraThreadRunning ();
}



void  CounterManagerWrapper::ResetCounts  ()
{
  counterManager->ResetCounts ();
}



int   CounterManagerWrapper::ParticlesCounted ()
{
  return  counterManager->ParticlesCounted ();
}  /* ParticlesCounted */



float  CounterManagerWrapper::CurScanRate ()
{
  return  counterManager->CurScanRate ();
}



kkint32 CounterManagerWrapper::SnapshotInterval ()
{
  return  counterManager->SnapshotInterval ();
}



void  CounterManagerWrapper::SnapshotInterval (kkint32 _snapshotInterval)
{
  counterManager->SnapshotInterval (_snapshotInterval);
}



kkint64   CounterManagerWrapper::TotalBytesRead  ()
{
  return counterManager->TotalBytesRead ();
}



kkint64   CounterManagerWrapper::TotalBytesToRead  ()
{
  return counterManager->TotalBytesToRead ();
}



void  CounterManagerWrapper::GetStatistics (float%  frameRate,               float%  scanRate,
                                            Int64%  bytesWritten,            Int32%  waitingToWriteToDisk,
                                            Int32%  physicalSeqNumsSkipped,  Int32%  diskWritingSeqNumBreaks,
                                            Int32%  totalLostPackets,        Int32%  scanLinesWritten,
                                            Int32%  physicalFramesRead,      Int32%  physicalFramesDropped,
                                            Int32%  physicalFramesWaitingToProcess,  Int32%  physicalFramesProcessed,
                                            Int32%  particlesExtracted,      Int32%  particlesWaitingProcessing,
                                            Int32%  particleCount
                                           )
{
  if  (!counterStats)
    counterStats = new CounterStats ();
  counterManager->GetStats (*counterStats);

  bytesWritten                    = counterStats->BytesWritten               ();
  frameRate                       = counterStats->FrameRate                  ();
  physicalFramesRead              = counterStats->PhysicalFramesRead         ();
  physicalFramesDropped           = counterStats->PhysicalFramesDropped      ();
  physicalFramesWaitingToProcess  = counterStats->PhysicalFramesWaitingToProcess ();
  particlesExtracted              = counterStats->ParticlesExtracted         ();
  particlesWaitingProcessing      = counterStats->ParticlesWaitingProcessing ();
  particleCount                   = counterStats->ParticlesCounted           ();
  physicalFramesProcessed         = counterStats->PhysicalFramesProcessed    ();
  physicalSeqNumsSkipped          = counterStats->PhysicalSeqNumsSkipped     ();
  scanRate                        = counterStats->ScanRate                   ();
  scanLinesWritten                = counterStats->ScanLinesWritten           ();
  totalLostPackets                = counterStats->TotalLostPackets           ();
  waitingToWriteToDisk            = counterStats->WaitingToWriteToDisk       ();
}  /* GetStatistics */



void  CounterManagerWrapper::GetGainSettings (bool%     _autoGainRunning,
                                              float%    _analogGain,
                                              kkint32%  _digitalGain
                                             )
{
  _autoGainRunning = counterManager->CameraAutoGainThreadRunning ();
  _analogGain      = counterManager->CurAnalogGain ();
  _digitalGain     = counterManager->CurDigitalGain ();
}



String^  CounterManagerWrapper::GetNextControlNumber ()
{
  return UmiKKStr::KKStrToSystenStr (counterManager->GetNextControlNumber ());
}



CounterStatsManaged^  CounterManagerWrapper::GetStats ()
{
  if  (!counterStats)
    counterStats = new CounterStats ();

  counterManager->GetStats (*counterStats);
  return gcnew CounterStatsManaged (counterStats);
}



CounterStatsManaged^  CounterManagerWrapper::GetFinaleStats ()
{
  const CounterStatsPtr  finaleStats = counterManager->GetFinaleStats ();
  return gcnew CounterStatsManaged (finaleStats);
}



kkint32  CounterManagerWrapper::GetLastSessionId ()
{
  return  counterManager->GetLastSessionId ();
}



UmiSessionParameters^  CounterManagerWrapper::GetSessionParameters ()
{
  SessionParametersPtr  sessionParms = new SessionParameters ();
  counterManager->GetSessionParameters (*sessionParms);
  return gcnew UmiSessionParameters (sessionParms);
}



UmiOperatingParameters^  CounterManagerWrapper::GetOperatingParameters ()
{
  OperatingParametersPtr  opParms = new OperatingParameters ();
  counterManager->GetOperatingParameters (*opParms);
  return gcnew UmiOperatingParameters (opParms);
}



UmiOperatingParameters^  CounterManagerWrapper::GetDefaultOperatingParameters ()
{
  OperatingParametersPtr  opParms = new OperatingParameters ();
  counterManager->GetDefaultOperatingParameters (*opParms);
  return gcnew UmiOperatingParameters (opParms);
}



String^  CounterManagerWrapper::DropFolderToRemote ()
{
  return  UmiKKStr::KKStrToSystenStr (counterManager->DropFolderToRemote ());
}



bool  CounterManagerWrapper::DiskWritingThreadRunning ()
{
  return  counterManager->DiskWritingThreadRunning ();
}



bool  CounterManagerWrapper::EmbeddedFlowMeter ()
{
  return  counterManager->EmbeddedFlowMeter ();
}



int  CounterManagerWrapper::FrameProcessorsCount ()
{
  return  counterManager->FrameProcessorsCount ();
}



bool  CounterManagerWrapper::GenerateFinaleReport ()
{
  return  counterManager->GenerateFinaleReport ();
}



bool  CounterManagerWrapper::IsRecording ()
{
  return  counterManager->IsRecording ();
}



bool  CounterManagerWrapper::IsRunning ()
{
  return  counterManager->IsRunning ();
}



String^  CounterManagerWrapper::LastScannerFileCounted ()
{
  return  KKStrToSystenStr (counterManager->LastScannerFileCounted ());
}



void  CounterManagerWrapper::LastScannerFileCounted (String^  _lastScannerFileCounted)
{
  counterManager->LastScannerFileCounted (SystemStringToKKStr (_lastScannerFileCounted));
}



String^  CounterManagerWrapper::NameLastRecordedFile ()
{
  return  KKStrToSystenStr (counterManager->NameLastRecordedFile ());
}



void  CounterManagerWrapper::NameLastRecordedFile (String^  _nameLastRecordedFile)
{
  counterManager->NameLastRecordedFile (SystemStringToKKStr (_nameLastRecordedFile));
}



void  CounterManagerWrapper::UpdateUnManagedConsumedMemory ()
{
  kkint64  unManagedMemoryInUse = counterManager->MemoryConsumedEstimated ();
  kkint64  deltaMemory = unManagedMemoryInUse - lastReportedMemoryPreasure;

  if  (deltaMemory > 0)
      GC::AddMemoryPressure (deltaMemory);
  else if  (deltaMemory < 0)
    GC::RemoveMemoryPressure (0 - deltaMemory);

  lastReportedMemoryPreasure = unManagedMemoryInUse;
}  /* UpdateUnManagedConsumedMemory */



void  CounterManagerWrapper::ShutDownAllThreads ()
{
  counterManager->ShutDownAllThreads ();
}  /* CancelProcesses */



//  Will stay in a loop until all threads that this object controls are stopped.
void  CounterManagerWrapper::TerminateAllThreads ()
{
  counterManager->TerminateAllThreads ();
}  /* StopAllThreads*/



void  CounterManagerWrapper::AddMsg (String^ msg)
{
  if  (msg == nullptr)
    return;
  counterManager->AddMsg (UmiKKStr::SystemStringToKKStrPtr (msg));
}



List<String^>^  CounterManagerWrapper::GetAllLoggedMsgs ()
{
  KKStrListPtr  msgs = counterManager->GetAllLoggedMsgs ();
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



UmiRunLog^  CounterManagerWrapper::GetUmiRunLogInstance ()
{
  return gcnew UmiRunLog (msgQueue);
}



Bitmap^  CounterManagerWrapper::GetLastParticle (kkint32   targetHeight,
                                                 kkint32   targetWidth,
                                                 String^%  classLabel
                                                )
{
  Bitmap^ bm = nullptr;
  classLabel = nullptr;
  RasterPtr  lastParticle = counterManager->GetLastParticle ();
  if  (lastParticle)
  {
    bm = BuildBitmap (lastParticle, targetHeight, targetWidth);
    classLabel = KKStrToSystenStr (lastParticle->Title ());
    delete  lastParticle;
    lastParticle = NULL;
  }
  return bm;
}  /* GetLastParticle */



StatusSnapshotManagedList^  CounterManagerWrapper::SnapshotsRetrieve (System::DateTime  rangeStart,
                                                                      System::DateTime  rangeEnd
                                                                     )
{
  StatusSnapshotManagedList^  snapshotsManaged = nullptr;

  StatusSnapshotListPtr  snapshots 
    = counterManager->SnapshotsRetrieve (StatusSnapshotManaged::DateTimeSystemToKKB (rangeStart), 
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



StatusSnapshotManaged^  CounterManagerWrapper::SnapshotLastEntry ()
{
  StatusSnapshotManaged^  snapshotManaged = nullptr;

  const StatusSnapshotPtr  snapshot =  counterManager->SnapshotLastEntry ();
  if  (snapshot)
    snapshotManaged = gcnew StatusSnapshotManaged (*snapshot);

  return snapshotManaged;
}  /* SnapshotLastEntry */



Bitmap^  CounterManagerWrapper::SnapShotLatestFrame (kkint32  targetHeight,
                                                     kkint32  targetWidth
                                                    )
{
  Bitmap^ bm = nullptr;
  RasterPtr  lastFrame = counterManager->SnapShotLatestFrame ();
  if  (lastFrame)
  {
    //bm = BuildBitmapFillTargetWidth (lastFrame, targetHeight, targetWidth);
    bm = BuildBitmapFillTargetWidth2 (lastFrame, targetHeight, targetWidth, (lastFrame->BackgroundPixelValue () == 0));
    delete  lastFrame;
    lastFrame = NULL;
  }
  return bm;
}  /* SnapShotLatestFrame */



UmiRaster^  CounterManagerWrapper::SnapShotLatestFrame ()
{
  RasterPtr  lastFrame = counterManager->SnapShotLatestFrame ();
  if  (lastFrame == NULL)
    return nullptr;
  else
    return gcnew UmiRaster (lastFrame);
}



void  CounterManagerWrapper::SetLiveVideoDimensions (kkint32  _liveVideoHeight,
                                                     kkint32  _liveVideoWidth
                                                    )
{
  counterManager->SetLiveVideoDimensions (_liveVideoHeight, _liveVideoWidth);
}



void  CounterManagerWrapper::BackGroundPixelTH (uchar  _backGroundPixelTH)
{
  counterManager->BackGroundPixelTH (_backGroundPixelTH);
}



int  CounterManagerWrapper::CropLeft ()
{
  return  counterManager->CropLeft ();
}



int  CounterManagerWrapper::CropRight ()
{
  return  counterManager->CropRight ();
}



void  CounterManagerWrapper::CropLeft (int _cropLeft)
{
  counterManager->CropSettingsChanged (_cropLeft, counterManager->CropRight ());
}



void  CounterManagerWrapper::CropRight (int _cropRight)
{
  counterManager->CropSettingsChanged (counterManager->CropLeft (), _cropRight);
}



int   CounterManagerWrapper::FrameWidthInPixels ()
{
  return  counterManager->FrameWidthInPixels ();
}



void  CounterManagerWrapper::CropSettingsChanged (int _cropLeft,
                                                  int _cropRight
                                                 )
{
  counterManager->CropSettingsChanged (_cropLeft, _cropRight);
}



float  CounterManagerWrapper::CurAnalogGain ()
{
  return counterManager->CurAnalogGain ();
}



int  CounterManagerWrapper::CurDigitalGain ()
{
  return counterManager->CurDigitalGain ();
}



bool  CounterManagerWrapper::FlatFieldEnabled ()  
{
  return  counterManager->FlatFieldEnabled ();
}



void  CounterManagerWrapper::FlatFieldEnabled (bool  _flatFieldEnabled)
{
  counterManager->FlatFieldEnabled (_flatFieldEnabled);
}



int   CounterManagerWrapper::MinSizeThreshold ()
{
  return  counterManager->MinSizeThreshold ();
}



void  CounterManagerWrapper::MinSizeThreshold (int  _minSizeThreshold)
{
  counterManager->MinSizeThreshold (_minSizeThreshold);
}



bool  CounterManagerWrapper::DataIsToBeCounted ()
{
  return  counterManager->DataIsToBeCounted ();
}



bool  CounterManagerWrapper::DataIsToBeRecorded ()
{
  return  counterManager->DataIsToBeRecorded ();
}



Bitmap^  CounterManagerWrapper::BuildBitmap (RasterPtr  raster,
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




Bitmap^  CounterManagerWrapper::BuildBitmapFillTargetWidth (RasterPtr  raster,
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



Bitmap^  CounterManagerWrapper::BuildBitmapFillTargetWidth2 (RasterPtr  raster,
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



void  CounterManagerWrapper::Status (System::String^% _statusMsg,
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

  if  (counterManager)
  {
    counterManager->Status (statusMsg, color, secondaryMsg, analogGain, digitalGain, cameraTemparature, flowRate);
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