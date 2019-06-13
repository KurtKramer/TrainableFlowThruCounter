/* CameraAcquisition.cpp -- Base class for the Acquisition threads.
 * Copyright (C) 2011-2013  Kurt Kramer
 * For conditions of distribution and use, see copyright notice in CounterUnManaged.txt
 */
#include "FirstIncludes.h"

#include <errno.h>
#include <istream>
#include <iostream>
#include <queue>
#include <vector>
#if  defined(WIN32)
#include <windows.h>
#endif
#include "MemoryDebug.h"
using namespace std;


#include "KKBaseTypes.h"
#include "GoalKeeper.h"
#include "OSservices.h"
#include "RunLog.h"
using namespace KKB;

#include "ScannerFile.h"
#include "ScannerFileEntry.h"
using namespace  KKLSC;

#include "CameraAcquisition.h"

#include "CameraFrame.h"
#include "CameraFrameBuffer.h"
#include "CameraAcquisition.h"
#include "CameraParameters.h"
#include "InstallationConfig.h"
#include "CounterManager.h"
#include "CounterStats.h"
using  namespace  CounterUnManaged;



CommandEntry::CommandEntry (EntryTypes  _commandType,
                            float       _parameter
                           ):
  commandType (_commandType),
  analogGain  (0.0f),
  digitalGain (0),
  scanRate    (0.0f)
{
  switch  (commandType)
  {
  case  EntryTypes::Null:
    break;

  case  EntryTypes::AnalogGain:
    analogGain = _parameter;
    break;

  case  EntryTypes::DigitalGain:
    digitalGain = (kkint32)_parameter;
    break;

  case  EntryTypes::ScanRate:  
    scanRate = _parameter;
    break;

  case  EntryTypes::SensitivityMode:
    sensitivityMode = "Low";
    break;
  }
}



CommandEntry::CommandEntry (EntryTypes  _commandType,
                            kkint32     _parameter
                           ):
  commandType (_commandType),
  analogGain  (0.0f),
  digitalGain (0),
  scanRate    (0.0f)
{
  switch  (commandType)
  {
  case  EntryTypes::Null:
    break;

  case  EntryTypes::AnalogGain:
    analogGain = (float)_parameter;
    break;

  case  EntryTypes::DigitalGain:
    digitalGain = _parameter;
    break;

  case  EntryTypes::ScanRate:  
    scanRate = (float)_parameter;
    break;

  case  EntryTypes::SensitivityMode:
    sensitivityMode = "Low";
    break;
  }
}



CommandEntry::CommandEntry (EntryTypes    _commandType,
                            const KKStr&  _parameter
                           ):
  commandType (_commandType),
  analogGain  (0.0f),
  digitalGain (0),
  scanRate    (0.0f),
  sensitivityMode ("Low")
{
  switch  (commandType)
  {
  case  EntryTypes::Null:
    break;

  case  EntryTypes::AnalogGain:
    analogGain = _parameter.ToFloat ();
    break;

  case  EntryTypes::DigitalGain:
    digitalGain = _parameter.ToInt32 ();
    break;

  case  EntryTypes::ScanRate:
    scanRate = (float)_parameter.ToFloat ();
    break;

  case  EntryTypes::SensitivityMode:
    sensitivityMode = _parameter;
    break;
  }
}

  
  
  CommandEntryQueue::CommandEntryQueue ():
  cmdQueue    (),
  goalie      (NULL),
  queueActive (false)
{
  GoalKeeper::Create ("CameraAcquisition::CommandEntryQueue", goalie);
}



CommandEntryQueue::~CommandEntryQueue ()
{
  GoalKeeper::Destroy (goalie);
  goalie = NULL;
}



void  CommandEntryQueue::AddScanRateCommand (float _scanRate)
{
  PushOnBack (new CommandEntry (CommandEntry::EntryTypes::ScanRate, _scanRate));
}



void  CommandEntryQueue::AddAnalogGainCommand (float _analogGain)
{
  PushOnBack (new CommandEntry (CommandEntry::EntryTypes::AnalogGain, _analogGain));
}



void  CommandEntryQueue::AddDigitalGainCommand (kkint32 _digitalGain)
{
  PushOnBack (new CommandEntry (CommandEntry::EntryTypes::DigitalGain, _digitalGain));
}



void  CommandEntryQueue::AddSensitivityMode (const KKStr&  _sensitivityMode)
{
  PushOnBack (new CommandEntry (CommandEntry::EntryTypes::SensitivityMode, _sensitivityMode));
}



CommandEntryPtr  CommandEntryQueue::GetNextCommand ()
{
  return  PopFromFront ();
}



void  CommandEntryQueue::PushOnBack (CommandEntryPtr  cmd)
{
  goalie->StartBlock ();
  cmdQueue.PushOnBack (cmd);
  queueActive = true;
  goalie->EndBlock ();
}



void  CommandEntryQueue::PushOnFront (CommandEntryPtr  cmd)
{
  goalie->StartBlock ();
  cmdQueue.PushOnBack (cmd);
  goalie->EndBlock ();
}



CommandEntryPtr  CommandEntryQueue::PopFromFront ()
{
  CommandEntryPtr  c = NULL;
  goalie->StartBlock ();
  c = cmdQueue.PopFromFront ();
  queueActive = (cmdQueue.QueueSize () > 0);
  goalie->EndBlock ();
  return  c;
}



CameraAcquisition::CameraAcquisition (CounterManagerPtr    _manager,
                                      CameraFrameBufferPtr _frameBuffer,
                                      FlowMeterTrackerPtr  _flowMeter,
                                      MsgQueuePtr          _msgQueue,
                                      const KKStr&         _threadName
                                    ):
    CameraThread (_manager, _threadName, _msgQueue),
    cameraParams             (NULL),
    cameraTemparature        (-1),
    commandQueue             (NULL),
    curAnalogGain            (0.0f),
    curDigitalGain           (0),
    curScanRate              (0.0),
    curSensitivityMode       ("Low"),
    flowMeter                (_flowMeter),
    flowMeterCounter         (0),
    flowMeterCounterScanLine (0),
    frameBuffer              (_frameBuffer),
    frameRate                (0.0f),
    lastBlockId              (-1),
    outputQueueSize          (0),
    physicalFramesRead       (0),
    physicalSeqNumsSkipped   (0),
    requestedAnalogGain      (0.0f),
    requestedDigitalGain     (0),
    requestedScanRate        (0.0f),
    scanLinesRead            (0),
    startStatus              (StartStatusType::Null),
    statusMsg                (),
    totalLostPackets         (0)
{
  cameraParams = new CameraParameters ();
  commandQueue = new CommandEntryQueue ();
}



CameraAcquisition::CameraAcquisition (CounterManagerPtr       _manager,
                                      CameraParametersPtr     _cameraParams,
                                      CameraFrameBufferPtr    _frameBuffer,
                                      FlowMeterTrackerPtr     _flowMeter,
                                      MsgQueuePtr             _msgQueue,
                                      const KKStr&            _threadName
                                    ):
    CameraThread (_manager, _threadName, _msgQueue),
    cameraParams             (_cameraParams),
    cameraTemparature        (-1),
    commandQueue             (NULL),
    curAnalogGain            (0.0f),
    curDigitalGain           (0),
    curScanRate              (0.0),
    curSensitivityMode       ("Low"),
    flowMeter                (_flowMeter),
    flowMeterCounter         (0),
    flowMeterCounterScanLine (0),
    frameBuffer              (_frameBuffer),
    frameRate                (0.0f),
    lastBlockId              (-1),
    outputQueueSize          (0),
    physicalFramesRead       (0),
    physicalSeqNumsSkipped   (0),
    requestedAnalogGain      (0.0f),
    requestedDigitalGain     (0),
    requestedScanRate        (0.0f),
    scanLinesRead            (0),
    startStatus              (StartStatusType::Null),
    statusMsg                (),
    totalLostPackets         (0)
{
  commandQueue = new CommandEntryQueue ();
}



CameraAcquisition::~CameraAcquisition ()
{
  delete  cameraParams;   cameraParams = NULL;
  delete  commandQueue;   commandQueue = NULL;
}



kkMemSize CameraAcquisition::MemoryConsumedEstimated () const
{
  kkMemSize frameBufferSize = 0;
  if  (frameBuffer)
    frameBufferSize = frameBuffer->MemoryConsumedEstimated ();

  kkMemSize memConsumed =  CameraThread::MemoryConsumedEstimated ()        + 
                           6 * sizeof (kkint32)                              + 
                           sizeof (CameraFrameBufferPtr) + frameBufferSize +
                           sizeof (frameRate)                              +
                           sizeof (cameraTemparature);

  if  (cameraParams)
    memConsumed += cameraParams->MemoryConsumedEstimated ();

  return  memConsumed;
}  /* MemoryConsumedEstimated */



void  CameraAcquisition::FlowMeterCounterUpdate (kkuint32 scanLineNum,
                                                 kkuint32 counter
                                                )
{
  flowMeter->AddEntry (scanLineNum, counter);
}



void  CameraAcquisition::CropSettingsChanged (kkint32  _cropLeft,
                                              kkint32  _cropRight
                                             )
{
  CameraThread::CropSettingsChanged (_cropLeft, _cropRight);
}



void  CameraAcquisition::ScanRateChanged (float _newScanRate)
{
  CameraThread::ScanRateChanged (_newScanRate);
}



void  CameraAcquisition::AddAnalogGainCommand (float _analogGain)
{
  commandQueue->AddAnalogGainCommand (_analogGain);
}



void  CameraAcquisition::AddDigitalGainCommand (kkint32 _digitalGain)
{
  commandQueue->AddDigitalGainCommand (_digitalGain);
}



void  CameraAcquisition::AddScanRateCommand (float _scanRate)
{
  commandQueue->AddScanRateCommand (_scanRate);
}



void  CameraAcquisition::AddSensitivityModeCommand (const KKStr&  _sensitivityMode)
{
  commandQueue->AddSensitivityMode (_sensitivityMode);
}



bool  CameraAcquisition::QueueActive () const
{
  return commandQueue->QueueActive ();
}



CommandEntryPtr  CameraAcquisition::GetNextCommandEntry ()
{
  return  commandQueue->GetNextCommand ();
}



const KKStr&  CameraAcquisition::SerialNumber ()  const
{
  if  (cameraParams)
    return cameraParams->SerialNumber ();
  else
    return KKStr::EmptyStr ();
}



const KKStr&  CameraAcquisition::MacAddress ()  const
{
  if  (cameraParams)
    return cameraParams->MacAddress ();
  else
    return  KKStr::EmptyStr ();
}



const KKStr&  CameraAcquisition::IpAddress ()  const
{
  if  (cameraParams)
    return cameraParams->IpAddress ();
  else
    return  KKStr::EmptyStr ();
}



kkint32  CameraAcquisition::FrameHeight ()  const  
{
  if  (cameraParams)
    return cameraParams->FrameHeight ();
  else
    return  0;
}



kkint32  CameraAcquisition::FrameWidth ()  const
{
  if  (cameraParams)
    return cameraParams->FrameWidth ();
  else
    return  0;
}



void  CameraAcquisition::FrameHeightWidth (kkint32  _frameHeight,
                                           kkint32  _frameWidth
                                          )
{
  if  (!cameraParams)
    cameraParams = new CameraParameters ();

  if  ((_frameWidth  != cameraParams->FrameWidth ())  ||
       (_frameHeight != cameraParams->FrameHeight ())  
      )
  {
    Manager ()->FrameHeightWidthChanged (_frameHeight, _frameWidth);
    cameraParams->FrameHeight (_frameHeight);
    cameraParams->FrameWidth (_frameWidth);
  }
}



float  CameraAcquisition::ScanRate ()  const
{
  if  (cameraParams)
    return cameraParams->ScanRate ();
  else
    return  0.0f;
}



float  CameraAcquisition::ComputeFlowRateFromFlowRateRatio ()  const
{
  const CounterManagerPtr  manager = ManagerConst ();
  const InstallationConfigPtr  instalation = manager->Installation ();

  float  chamberWidthSpatialy = instalation->ImagingChamberWidth ();
  float  scanRate             = ScanRate ();
  float  flowRateRatio        = manager->FlowRateFactor ();
  float  chamberWidthPixels   = (float)(1 + manager->CropRight () - manager->CropLeft ());

  float  flowRate = (chamberWidthSpatialy * scanRate) / (flowRateRatio * chamberWidthPixels);
 
  return  flowRate;
}



void  CameraAcquisition::GetStats (CounterStats&  counterStats) const
{
  counterStats.CurAnalogGain          (curAnalogGain);
  counterStats.CurDigitalGain         (curDigitalGain);
  counterStats.CurSensitivity         (curSensitivityMode);
  counterStats.PhysicalFramesRead     (physicalFramesRead);
  counterStats.FrameRate              (frameRate);
  counterStats.CameraTemparature      (cameraTemparature);
  counterStats.TotalLostPackets       (totalLostPackets);
  counterStats.ScanRate               (curScanRate);
  counterStats.PhysicalSeqNumsSkipped (physicalSeqNumsSkipped);
  counterStats.ScanLinesRead          (scanLinesRead);
}



void  CameraAcquisition::CameraParametersAddToHeaderFields ()
{
  Manager ()->AddHeaderField ("CP:AnalogGain",       cameraParams->AnalogGain           ());
  Manager ()->AddHeaderField ("CP:DeviceVendorName", cameraParams->DeviceVendorName     ());
  Manager ()->AddHeaderField ("CP:DeviceModelName",  cameraParams->DeviceModelName      ());
  Manager ()->AddHeaderField ("CP:DeviceVersion",    cameraParams->DeviceVersion        ());
  Manager ()->AddHeaderField ("CP:DigitalGain",      cameraParams->DigitalGain          ());
  Manager ()->AddHeaderField ("CP:SensitivityMode",  cameraParams->SensitivityMode      ());
  Manager ()->AddHeaderField ("CP:FrameHeight",      (kkint64)cameraParams->FrameHeight ());
  Manager ()->AddHeaderField ("CP:FrameWidth",       (kkint64)cameraParams->FrameWidth  ());
  Manager ()->AddHeaderField ("CP:IpAddress",        cameraParams->IpAddress            ());
  Manager ()->AddHeaderField ("CP:MacAddress",       cameraParams->MacAddress           ());
  Manager ()->AddHeaderField ("CP:Model",            cameraParams->Model                ());
  Manager ()->AddHeaderField ("CP:ScanRate",         cameraParams->ScanRate             ());
  Manager ()->AddHeaderField ("CP:SerialNumber",     cameraParams->SerialNumber         ());
}



void  CameraAcquisition::StartStatus (StartStatusType  _startStatus,
                                      const KKStr&     _statusMsg
                                     )
{
  startStatus = _startStatus;
  statusMsg   = _statusMsg;
}



bool  CameraAcquisition::FlatFieldEnabled ()  const
{
  if  (frameBuffer)
    return  frameBuffer->FlatFieldEnabled ();
  else
    return false;
}



void  CameraAcquisition::FlatFieldEnabled (bool  _flatFieldEnabled)
{
  if  (frameBuffer)
    frameBuffer->FlatFieldEnabled (_flatFieldEnabled);
}



void  CameraAcquisition::RequestedCameraParameters (OperatingParametersPtr  opParms)
{
  if  (!opParms->RequestedSensitivityMode ().Empty ())  AddSensitivityModeCommand (opParms->RequestedSensitivityMode ());
  if  (opParms->RequestedAnalogGain  () > -10000.0f)    AddAnalogGainCommand      (opParms->RequestedAnalogGain      ());
  if  (opParms->RequestedDigitalGain () > 0)            AddDigitalGainCommand     (opParms->RequestedDigitalGain     ());
  if  (opParms->RequestedScanRate    () > 0.0f)         AddScanRateCommand        (opParms->RequestedScanRate        ());
}



void  CameraAcquisition::RequestedAnalogGain (float _requestedAnalogGain)
{
  if  ((StartStatus () == StartStatusType::Connected)  ||  (StartStatus () == StartStatusType::Connecting))
    AddAnalogGainCommand (_requestedAnalogGain);
  requestedAnalogGain = _requestedAnalogGain;
}



void  CameraAcquisition::RequestedDigitalGain (kkint32 _requestedDigitalGain)
{
  if  ((StartStatus () == StartStatusType::Connected)  ||  (StartStatus () == StartStatusType::Connecting))
    AddDigitalGainCommand (_requestedDigitalGain);
  requestedDigitalGain = _requestedDigitalGain;
}



void  CameraAcquisition::RequestedScanRate (float _requestedScanRate)
{
  if  ((StartStatus () == StartStatusType::Connected)  ||  (StartStatus () == StartStatusType::Connecting))
    AddScanRateCommand (_requestedScanRate);
  requestedScanRate = _requestedScanRate;
}  /* RequestedScanRate */



void  CameraAcquisition::RequestedSensitivityMode (const KKStr& _requestedSensitivityMode)
{
  if  ((StartStatus () == StartStatusType::Connected)  ||  (StartStatus () == StartStatusType::Connecting))
    AddSensitivityModeCommand (_requestedSensitivityMode);
    
  requestedSensitivityMode = _requestedSensitivityMode;
}  /* RequestedSensitivityMode */



void  CameraAcquisition::Run ()
{
  // This method should have been over ridden by a derived class.
  log.Level (-1) << endl 
                 <<"CameraAcquisition::Run   ***ERROR***       This method should have been over-ridden by a derived class." << endl 
                 << endl;
}



void  CameraAcquisition::ResetCounts ()
{
  scanLinesRead          = 0;
  physicalFramesRead     = 0;
  totalLostPackets       = 0;
  physicalSeqNumsSkipped = 0;
}
