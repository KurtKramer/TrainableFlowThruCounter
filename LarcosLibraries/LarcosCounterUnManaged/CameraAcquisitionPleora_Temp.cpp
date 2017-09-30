/* CameraAcquisitionPleora.cpp -- Represents a single camera frame.
 * Copyright (C) 2011-2013  Kurt Kramer
 * For conditions of distribution and use, see copyright notice in LarcosCounterUnManaged.txt
 */
#include "FirstIncludes.h"

#include <errno.h>
#include <istream>
#include <iostream>
#include <fstream>
#include <queue>
#include <vector>
#if  defined(OS_WINDOWS)
#include <windows.h>
#endif
#include <math.h>
#include "MemoryDebug.h"
using namespace std;


#include <PvDeviceFinderWnd.h>
#include <PvDeviceGEV.h>
#include <PvPipeline.h>
#include <PvBuffer.h>
#include <PvStreamGEV.h>
//#include <PvStreamRaw.h>
#include <PvSystem.h>

#include "KKBaseTypes.h"
#include "GoalKeeper.h"
#include "OSservices.h"
#include "RunLog.h"
using namespace KKB;


#include "ScannerFile.h"
#include "ScannerFileEntry.h"
using namespace  KKLSC;


#include "LarcosVariables.h"
using namespace  LarcosBase;


#include "CameraAcquisitionPleora.h"
#include "CameraAcquisition.h"
#include "CameraFrame.h"
#include "CameraFrameBuffer.h"
#include "CameraParameters.h"
#include "InstallationConfig.h"


#include "LarcosCounterManager.h"


using  namespace  LarcosCounterUnManaged;



// 2013-10-19,   KAK,  Will learned possible way to get to talk thru camera to get flow data,
//                     Seems bug in firmware where we have to set format to "NoConnect"  then
//                     to "TTL";  this will force the command to propagate thru the rest if the 
//                     camera.
//
// LineFormat  turn to "NoConnect(0)"  then back to "TTL(1):
//  Input to lineSelector  to Line0, followed by NoConnect,  then TTL

// OutPut
// OutputSelector to Line1
// OutputFormat set to "NoConnect"  followed by  "TTL".



CameraAcquisitionPleora::CameraAcquisitionPleora 
                                     (LarcosCounterManagerPtr _manager,
                                      const KKStr&            _macAddress,
                                      CameraFrameBufferPtr    _frameBuffer,
                                      FlowMeterTrackerPtr     _flowMeter,
                                      MsgQueuePtr             _msgQueue,
                                      const KKStr&            _threadName
                                     ):
    CameraAcquisition (_manager, _frameBuffer, _flowMeter, _msgQueue, _threadName),

    oneSecFrameInterval        (41),    /**< Number of frames at 480 Scan Lines per frame 20K scan rate for  1 second interval. */
    twoSecFrameInterval        (82),
    threeSecFrameInterval      (123),
    fiveSecFrameInterval       (205),
    tenSecFrameInterval        (410),
    voltTempReadInterval       (416),   /**< Number of frames at 480 Scan Lines per frame 20K scan rate for 10 second interval.  */

    embeddedFlowMeter          (false),
    generateFlowRateReport     (false),

    // Variables needed to support AutoGain.
    agAnalogGain               (-5.0),
    analogGainMin              (-10.0f),
    analogGainMax              (10.0f),
    agDigitalGain              (1000),
    digitalGainMax             (10000),
    gainTapSel                 (-1)

{
  cameraParams->MacAddress (_macAddress);
  if  (Manager ()->Installation ()->FlowMeterMethod () == InstallationConfig::FlowMeterMethods::Embedded)
    embeddedFlowMeter = true;
  else
    embeddedFlowMeter = false;
}



CameraAcquisitionPleora::CameraAcquisitionPleora 
                                     (LarcosCounterManagerPtr _manager,
                                      CameraParametersPtr     _cameraParams,
                                      CameraFrameBufferPtr    _frameBuffer,
                                      FlowMeterTrackerPtr     _flowMeter,
                                      MsgQueuePtr             _msgQueue,
                                      const KKStr&            _threadName
                                     ):
    CameraAcquisition (_manager, _cameraParams, _frameBuffer, _flowMeter, _msgQueue, _threadName),

    oneSecFrameInterval        (41),    /**< Number of frames at 480 Scan Lines per frame 20K scan rate for  1 second interval. */
    twoSecFrameInterval        (82),
    threeSecFrameInterval      (123),
    fiveSecFrameInterval       (205),
    tenSecFrameInterval        (410),
    voltTempReadInterval       (416),   /**< Number of frames at 480 Scan Lines per frame 20K scan rate for 10 second interval.  */

    embeddedFlowMeter          (false),
    generateFlowRateReport     (false),

    // Variables needed to support AutoGain.
    agAnalogGain               (-5.0),
    analogGainMin              (-10.0f),
    analogGainMax              (10.0f),
    agDigitalGain              (1000),
    digitalGainMax             (10000),
    gainTapSel                 (-1)

{
  if  (Manager ()->Installation ()->FlowMeterMethod () == InstallationConfig::FlowMeterMethods::Embedded)
    embeddedFlowMeter = true;
  else
    embeddedFlowMeter = false;
}



CameraAcquisitionPleora::~CameraAcquisitionPleora ()
{
}



void  CameraAcquisitionPleora::InitializeDeviceParameterAccessVariables ()
{
}  /* InitializeDeviceParameterAccessVariables */



void  CameraAcquisitionPleora::InitializeStreamParameterAccessVariables ()
{
}




const char*  CameraAcquisitionPleora::PvGenTypeToCStr (PvGenType  genType)
{
  const char* s = "UnKnown";
  return s;
}  /* PvGenTypeToCStr */



double  CameraAcquisitionPleora::GetPvParameterFloat (PvGenParameterArray*  params,
                                                      const KKStr&          paramName,
                                                      double                defaultValue  /**< Value if parameter not available. */
                                                     )
{
  return 0.0;
}  /* GetPvParameterFloat */



kkint64  CameraAcquisitionPleora::GetPvParameterInteger (PvGenParameterArray*  params,
                                                         const KKStr&          paramName,
                                                         kkint64               defaultValue
                                                        )
{

  return 0;
}  /* GetPvParameterInteger */



kkint32  CameraAcquisitionPleora::GetPvParameterEnum (PvGenParameterArray*  params,
                                                    const KKStr&          paramName,
                                                    kkint32               defaultValue
                                                   )
{
  return 0;
}  /* GetPvParameterEnum */




KKStr  CameraAcquisitionPleora::GetPvParameterEnumStr (PvGenParameterArray*  params,
                                                       const KKStr&          paramName,
                                                       const KKStr&          defaultValue
                                                      )
{
  KKStr  result = defaultValue;
  return  result;
}  /* GetPvParameterEnumStr */






KKStr  CameraAcquisitionPleora::GetPvParameterString (PvGenParameterArray*  params,
                                                      const KKStr&          paramName,
                                                      const KKStr&          defaultValue
                                                     )
{
  KKStr result = defaultValue;
  return result;
}  /* GetPvParameterString */





void  CameraAcquisitionPleora::SetPvParameterEnum (const KKStr&  paramName,
                                                   const KKStr&  value,
                                                   bool&         successful,
                                                   KKStr&        errMsg
                                                  )
{
  successful = false;
  return;
}  /* SetPvParameterEnum */



void  CameraAcquisitionPleora::SetPvParameterBool (const KKStr&  paramName,
                                                   bool          value,
                                                   bool&         successful,
                                                   KKStr&        errMsg
                                                  )
{
  errMsg = "";
  successful = true;
  return;
}  /* SetPvParameterBool */




KKStr  CameraAcquisitionPleora::Int64ToIpAddress (int64_t i)
{
  kkint32 p1 = i % 256;  i /= 256;
  kkint32 p2 = i % 256;  i /= 256;
  kkint32 p3 = i % 256;  i /= 256;
  kkint32 p4 = i % 256;  i /= 256;
  KKStr ipStr = StrFormatInt (p4, "000") + "."  +
                StrFormatInt (p3, "000") + "."  +
                StrFormatInt (p2, "000") + "."  +
                StrFormatInt (p1, "000");

  return  ipStr;
}  /* Int64ToIpAddress */





void  CameraAcquisitionPleora::AddToHeaderField (PvGenParameter*  p)
{
}  /* AddToHeaderField */




void   CameraAcquisitionPleora::AddPleoraVariableToHeaderField (const KKStr&  varName)
{
}




void   CameraAcquisitionPleora::CameraParametersPopulate (PvGenParameterArray*  params,
                                                          CameraParameters&     cameraParams,
                                                          RunLog&               runLog
                                                         )
{
  runLog.Level (40) << "CameraAcquisitionPleora::CameraParametersPopulate " << ((params == NULL) ? "***  params == NULL  ***" : "") << endl;
  return;
}  /* CameraParametersPopulate */



void  CameraAcquisitionPleora::SetGainTap (kkint64 gainTap)
{
}


KKStr  CameraAcquisitionPleora::PvResultToStr (PvResult&  r)
{
  KKStr s = "";

  return  s;
}  /* PvResultToStr */




void  CameraAcquisitionPleora::SetSensitivityMode (const KKStr& requestedSensitivityMode)
{
}   /* SetSensitivityMode */



void  CameraAcquisitionPleora::SetScanRate  (float requestedScanRate)
{}  /* SetScanRate */



void  CameraAcquisitionPleora::SetAnalogGain  (float requestedAnalogGain)
{
}  /* SetAnalogGain */



void  CameraAcquisitionPleora::SetDigitalGain  (kkint32 requestedDigitalGain)
{
}  /* SetDigitalGain */



void  CameraAcquisitionPleora::ApplyCommandEntries ()
{
  log.Level (10) << "CameraAcquisitionPleora::ApplyCommandEntries" << endl;
  while  (QueueActive ())
    ApplyCommandNextEntry ();
}  /* ApplyCommandEntries */



void  CameraAcquisitionPleora::ApplyCommandNextEntry ()
{
  if  (!QueueActive ())
    return;

  CommandEntryPtr  cmd = GetNextCommandEntry ();
  if  (cmd)
  {
    switch  (cmd->CommandType ())
    {
    case  CommandEntry::EntryTypes::Null:
      break;

    case  CommandEntry::EntryTypes::AnalogGain:
      SetAnalogGain (cmd->AnalogGain ());
      break;

    case  CommandEntry::EntryTypes::DigitalGain:
      SetDigitalGain (cmd->DigitalGain ());
      break;

    case  CommandEntry::EntryTypes::ScanRate:
      SetScanRate (cmd->ScanRate ());
      break;

    case  CommandEntry::EntryTypes::SensitivityMode:
      SetSensitivityMode (cmd->SensitivityMode ());
      break;
    }

    delete  cmd;
    cmd = NULL;
  }
}  /* ApplyCommandNextEntry */






void  CameraAcquisitionPleora::SetCameraParametersForFlowMeter ()
{
  //KKKK  I want to move this code to a point where the variables are writable.
  bool  successful = false;
  KKStr  errMsg;

  SetPvParameterEnum ("PLC_Q1_Variable0", "One", successful, errMsg);
  if  (!successful)
  {
    log.Level (-1) << "SetCameraParametersForFlowMeter    ***ERROR***  setting 'PLC_Q1_Variable0'." << endl
                   << errMsg << endl;
  }

  SetPvParameterEnum ("PLC_Q1_Operator0", "Or", successful, errMsg);
  if  (!successful)
  {
    log.Level (-1) << "SetCameraParametersForFlowMeter    ***ERROR***  setting 'PLC_Q1_Operator0'." << endl
                   << errMsg << endl;
  }


  SetPvParameterEnum ("PLC_Q1_Variable1", "One", successful, errMsg);
  if  (!successful)
  {
    log.Level (-1) << "SetCameraParametersForFlowMeter    ***ERROR***  setting 'PLC_Q1_Variable1'." << endl
                   << errMsg << endl;
  }

  SetPvParameterEnum ("OutputSelector", "Line1", successful, errMsg);
  if  (!successful)
  {
    log.Level (-1) << "SetCameraParametersForFlowMeter    ***ERROR***  setting 'OutputSelector'." << endl
                   << errMsg << endl;
  }


  SetPvParameterEnum ("OutputFormat", "NoConnect", successful, errMsg);
  if  (!successful)
  {
    log.Level (-1) << "SetCameraParametersForFlowMeter    ***ERROR***  setting 'OutputFormat' to 'NoConnect'." << endl
                   << errMsg << endl;
  }

  SetPvParameterEnum ("OutputFormat", "TTL", successful, errMsg);
  if  (!successful)
  {
    log.Level (-1) << "SetCameraParametersForFlowMeter    ***ERROR***  setting 'OutputFormat' to 'TTL'." << endl
                   << errMsg << endl;
  }

  SetPvParameterEnum ("LineSelector", "Line0", successful, errMsg);
  if  (!successful)
  {
    log.Level (-1) << "SetCameraParametersForFlowMeter    ***ERROR***  setting 'LineSelector' to 'Line0'." << endl
                   << errMsg << endl;
  }

  SetPvParameterEnum ("LineFormat", "NoConnect", successful, errMsg);
  if  (!successful)
  {
    log.Level (-1) << "SetCameraParametersForFlowMeter    ***ERROR***  setting 'LineFormat' to 'NoConnect'." << endl
                   << errMsg << endl;
  }

  SetPvParameterEnum ("LineFormat", "TTL", successful, errMsg);
  if  (!successful)
  {
    log.Level (-1) << "SetCameraParametersForFlowMeter    ***ERROR***  setting 'LineFormat' to 'TTL'." << endl
                   << errMsg << endl;
  }

  SetPvParameterEnum ("CounterSelector", "Counter1", successful, errMsg);
  if  (!successful)
  {
    log.Level (-1) << "SetCameraParametersForFlowMeter    ***ERROR***  setting 'CounterSelector' to 'Counter1'." << endl
                   << errMsg << endl;
  }

  SetPvParameterEnum ("PLC_I0", "Line0", successful, errMsg);
  if  (!successful)
  {
    log.Level (-1) << "SetCameraParametersForFlowMeter    ***ERROR***  setting 'PLC_I0' to 'Line0'." << endl
                   << errMsg << endl;
  }

  SetPvParameterEnum ("PLC_Q17_Variable0", "PLC_I0", successful, errMsg);
  if  (!successful)
  {
    log.Level (-1) << "SetCameraParametersForFlowMeter    ***ERROR***  setting 'PLC_Q17_Variable0' to 'PLC_I0'." << endl
                   << errMsg << endl;
  }

  SetPvParameterEnum ("PLC_Q17_Operator0", "Or", successful, errMsg);
  if  (!successful)
  {
    log.Level (-1) << "SetCameraParametersForFlowMeter    ***ERROR***  setting 'PLC_Q17_Variable0' to 'PLC_I0'." << endl
                   << errMsg << endl;
  }

  SetPvParameterEnum ("PLC_Q17_Variable1", "Zero", successful, errMsg);
  if  (!successful)
  {
    log.Level (-1) << "SetCameraParametersForFlowMeter    ***ERROR***  setting 'PLC_Q17_Variable1' to 'Zero'." << endl
                   << errMsg << endl;
  }

  SetPvParameterEnum ("GevTimestampCounterSelector", "Counter1", successful, errMsg);
  if  (!successful)
  {
    log.Level (-1) << "SetCameraParametersForFlowMeter    ***ERROR***  setting 'GevTimestampCounterSelector' to 'Counter1'." << endl
                   << errMsg << endl;
  }

  SetPvParameterBool ("GrbCh0AcqCfgIncludeMetadataInImage", true, successful, errMsg);
  if  (!successful)
  {
    log.Level (-1) << "SetCameraParametersForFlowMeter    ***ERROR***  setting 'GrbCh0AcqCfgIncludeMetadataInImage' to 'True'." << endl
                   << errMsg << endl;
  }

  SetPvParameterEnum ("GrbCh0MetadataSourceSelection", "GevTimestampValueLow", successful, errMsg);
  if  (!successful)
  {
    log.Level (-1) << "SetCameraParametersForFlowMeter    ***ERROR***  setting 'GrbCh0MetadataSourceSelection' to 'GevTimestampValueLow'." << endl
                   << errMsg << endl;
  }

  SetPvParameterEnum ("CounterEventSource", "PLC_Q17_RisingEdge", successful, errMsg);
  if  (!successful)
  {
    log.Level (-1) << "SetCameraParametersForFlowMeter    ***ERROR***  setting 'CounterEventSource' to 'PLC_Q17_RisingEdge'." << endl
                   << errMsg << endl;
  }
}  /* SetCameraParametersForFlowMeter */





void  CameraAcquisitionPleora::ConnectToCamera (bool&  connectionSuccessful)
{
  return;
}  /* ConnectToCamera */




void  CameraAcquisitionPleora::DisconnectFromCamera ()
{
  return;
}  /* DisconnectFromCamera */




void  CameraAcquisitionPleora::ComputeFramneIntervals ()
{
  if  (curScanRate <= 1.0f)
    return;

  // Compute the number of frames that occur in a 5 second period.
  oneSecFrameInterval   = (kkint32)(0.5f + curScanRate *  1.0f / (float)FrameHeight ());
  twoSecFrameInterval   = oneSecFrameInterval *  2;
  threeSecFrameInterval = oneSecFrameInterval *  3;
  fiveSecFrameInterval  = oneSecFrameInterval *  5;
  tenSecFrameInterval   = oneSecFrameInterval * 10;
  voltTempReadInterval  = oneSecFrameInterval * 10;
}


void  CameraAcquisitionPleora::Run ()
{
  return;
}  /* Run */



CameraParametersListPtr  CameraAcquisitionPleora::GetCameraList (MsgQueuePtr  _msgQueue)
{

  return NULL;
}  /* GetCameraList */



CameraParametersPtr  CameraAcquisitionPleora::GetDeviceInfo (const KKStr& _keyValue,
                                                             char         _keyType, /* 'S' = Serial Number, 'M' = MAC Address */
                                                             RunLog&      _runLog
                                                            )
{
  return NULL;
}  /* GetDeviceInfo */




CameraParametersPtr  CameraAcquisitionPleora::GetCameraParameters (const KKStr&  _macAddress,
                                                                   MsgQueuePtr   _msgQueue,
                                                                   RunLog&       _runLog
                                                                  )
{
  return  NULL;
}  /* GetCameraParameters */





CameraAcquisitionPleoraPtr  CameraAcquisitionPleora::CreateFromSerialNumber 
           (LarcosCounterManagerPtr _manager,
            const KKStr             _serialNumber,
            CameraFrameBufferPtr    _frameBuffer,
            FlowMeterTrackerPtr     _flowMeter,
            MsgQueuePtr             _msgQueue,
            const KKStr&            _threadName
           )
{
  RunLog  runLog (_msgQueue);
  runLog.SetLevel (LarcosVariables::DebugLevel ());

  runLog.Level (20) << "CameraAcquisitionPleora::CreateFromSerialNumber   serialNumber :" << _serialNumber << endl;

  CameraParametersPtr cameraParams = GetDeviceInfo (_serialNumber, 'S', runLog);
  if  (!cameraParams)
    return NULL;

  KKStr macAddress = cameraParams->MacAddress ();
  delete  cameraParams;
  cameraParams = NULL;

  if  (macAddress.Empty ())
    return NULL;

  return new  CameraAcquisitionPleora (_manager, 
                                       macAddress,
                                       _frameBuffer,
                                       _flowMeter,
                                       _msgQueue,
                                       _threadName
                                     );
}  /* CreateFromSerialNumber */




CameraAcquisitionPleoraPtr  CameraAcquisitionPleora::CreateFromMacAddress 
           (LarcosCounterManagerPtr _manager,
            const KKStr&            _macAddress,
            CameraFrameBufferPtr    _frameBuffer,
            FlowMeterTrackerPtr     _flowMeter,
            MsgQueuePtr             _msgQueue,
            const KKStr&            _threadName
           )
{
  RunLog  runLog (_msgQueue);
  runLog.SetLevel (LarcosVariables::DebugLevel ());

  runLog.Level (20) << "CameraAcquisitionPleora::CreateFromMacAddress   MacAddress :" << _macAddress << endl;
  return new  CameraAcquisitionPleora (_manager,
                                       _macAddress,
                                       _frameBuffer,
                                       _flowMeter,
                                       _msgQueue,
                                       _threadName
                                     );
}  /* CreateFromMacAddress */




CameraAcquisitionPleoraPtr  CameraAcquisitionPleora::CreateFromCameraParameters
               (LarcosCounterManagerPtr _manager,
                CameraParametersPtr     _cameraParameters,
                CameraFrameBufferPtr    _frameBuffer,
                FlowMeterTrackerPtr     _flowMeter,
                MsgQueuePtr             _msgQueue,
                const KKStr&            _threadName
               )
{
  RunLog  runLog (_msgQueue);
  runLog.SetLevel (LarcosVariables::DebugLevel ());

  runLog.Level (20) << "CameraAcquisitionPleora::CreateFromCameraParameters   MacAddress :" << _cameraParameters->MacAddress () << endl;

  CameraAcquisitionPleoraPtr  acquisition =  new CameraAcquisitionPleora (_manager, _cameraParameters, _frameBuffer, _flowMeter, _msgQueue, _threadName);
  return  acquisition;
}



KKStr  CameraAcquisitionPleora::PromptForCameraMacAddress ()
{
  KKStr  macAddress = "";
  return  macAddress;
}  /* PromptForCameraMacAddress */




KKStr  CameraAcquisitionPleora::GetPvGenParameterDesc (PvGenParameter*  p,
                                                       bool             tabDelStr
                                                      )
{
  KKStr returnStr = "";
  return  returnStr;
}  /* GetPvGenParameterDesc */



void  CameraAcquisitionPleora::PrintDeviceParameters (const KKStr&          subName,
                                                      PvGenParameterArray*  lDeviceParams
                                                     )
{
}  /* PrintDeviceParameters */


void  CameraAcquisitionPleora::ResetCounts ()
{
  CameraAcquisition::ResetCounts ();
}



/******************************************************************************/
/*                             Auto gain Procedure                            */
/******************************************************************************/





kkuint16 CameraAcquisitionPleora::CameraHighValue ()
{
  return  255;
}  /* CameraHighValue */




bool  CameraAcquisitionPleora::CameraSomePixelsSaturated (float percentThreshold)
{

  return  true;
}  /* CameraSomePixelsSaturated */




kkuint16 CameraAcquisitionPleora::CameraHighValueOf3Frames ()
{
  WaitForOneFrame ();
  kkuint16  hv1 = CameraHighValue ();

  WaitForOneFrame ();
  kkuint16  hv2 = CameraHighValue ();

  WaitForOneFrame ();
  kkuint16  hv3 = CameraHighValue ();

  return  (Max (hv1, hv2, hv3));
}  /* CameraHighValueOf3Frames */




bool  CameraAcquisitionPleora::DigitalGainIsMaxed ()
{
  bool  digitalGainIsMaxed = false;

  VectorUcharPtr  scanLine = Manager ()->LastFrameAverageScanLine ();
  if  (!scanLine)
    return  false;

  kkuint16  numPixelsGreaterThan253 = 0;

  // We start from the 4th column because the first 4 columns may be carrying the "Flow-Meter" counter value.
  for  (kkuint32 x = 4;  x < scanLine->size ();  ++x)
  {
    uchar v = (*scanLine)[x];
    if  (v > 253)
      ++numPixelsGreaterThan253;
  }

  kkuint16  threshold = (kkuint16)(0.5f + scanLine->size () / 40.0f);
  if  (numPixelsGreaterThan253 > threshold)
    digitalGainIsMaxed = true;
  else
    digitalGainIsMaxed = false;

  delete  scanLine;
  scanLine = NULL;

  return  digitalGainIsMaxed;
}  /* DigitalGainIsMaxed */





void  CameraAcquisitionPleora::WaitForOneFrame ()
{
  kkint64  lastNextFrameSeqNum = frameBuffer->NextFrameSeqNum ();
  while  ((!TerminateFlag ())  &&  (!ShutdownFlag ()))
  {
    kkint64  diff = frameBuffer->NextFrameSeqNum () - lastNextFrameSeqNum;
    if  (diff < 0)  diff = 0 - diff;
    if  (diff > 1) 
      break;
    osSleepMiliSecs (20);
  }
}  /* WaitForOneFrame */




void  CameraAcquisitionPleora::MakeAnalogGainRequest (float  _gain)
{
  log.Level (50) << "CameraAcquisitionPleora::MakeAnalogGainRequest  Gain [" << _gain << "]." << endl;

  Manager ()->RequestedAnalogGain (_gain);

  int loopCount = 0;

  while  ((!TerminateFlag ())  &&  (!ShutdownFlag ()))
  {
    if  ((loopCount % 20) == 19)
      Manager ()->RequestedAnalogGain (_gain);

    osSleepMiliSecs (20);
    float  diff = fabs (Manager ()->CurAnalogGain () - _gain);
    if  (diff < 0.01f)
      break;

    if  (RequestedAnalogGain () != _gain)
       Manager ()->RequestedAnalogGain (_gain);

    ++loopCount;
  }
}  /* MakeAnalogGainRequest */



void  CameraAcquisitionPleora::MakeDigitalGainRequest (kkint32  _gain)
{
  log.Level (50) << "CameraAcquisitionPleora::MakeDigitalGainRequest  Gain [" << _gain << "]." << endl;

  Manager ()->RequestedDigitalGain (_gain);
  while  ((!TerminateFlag ())  &&  (!ShutdownFlag ()))
  {
    osSleepMiliSecs (20);
    kkint32  diff = Manager ()->CurDigitalGain () - _gain;
    if  (diff < 0)
      diff = 0 - diff;
    if  (diff < 1)
      break;

    if  (RequestedDigitalGain () != _gain)
       Manager ()->RequestedDigitalGain (_gain);
  }
}  /* MakeDigitalGainRequest */






void  CameraAcquisitionPleora::RaiseAnalogGainUntilSaturated2 (float  increments)
{
  // AnalogGain saturation occurs when the high values no longer increase as you increase Analog Gain.
  // Because of noise we define Not Growing and not increasing by more than 2%.
  log.Level (40) << "CameraAcquisitionPleora::RaiseAnalogGainUntilSaturated2  increments[" << increments << "]." << endl;

  while  ((!TerminateFlag ())  &&  (!ShutdownFlag ()))
  {
    float  nextAnalogGain = agAnalogGain + increments;
    if  (nextAnalogGain > analogGainMax)
    {
      log.Level (30) << "RaiseAnalogGainUntilSaturated2   Reached end of Analog-Gain range." << endl;
      break;
    }

    MakeAnalogGainRequest (nextAnalogGain);
    agAnalogGain = nextAnalogGain;
    WaitForOneFrame ();

    // When increments are at 2.0f we want a saturation threshold count of 10% of total number of pixels in a scan line.
    float  th = Max (0.3f, 0.1f * (increments / 2.0f));

    if  (CameraSomePixelsSaturated (th))
    {
      agAnalogGain = agAnalogGain - increments;
      agAnalogGain = max (analogGainMin, agAnalogGain);
      MakeAnalogGainRequest (agAnalogGain);
      WaitForOneFrame ();
      log.Level (30) << "RaiseAnalogGainUntilSaturated2   Camera values have not gained  New analogGain[" << KKB::StrFormatDouble (agAnalogGain, "##0.00") << "]." << endl;
      break;
    }
  }
}  /* RaiseAnalogGainUntilSaturated2 */




void    CameraAcquisitionPleora::SetAnalogGainToFirstHighInRange (float  analogGainStart, 
                                                                  float  analogGainEnd, 
                                                                  float  increment
                                                                 )
{
  log.Level (40) << "SetAnalogGainToFirstHighInRange  AnalogGainStart: " << analogGainStart << "  AnalogGainEnd: " << analogGainEnd << "  Increment: " << increment << endl;

  VectorUint16 highValues;

  kkuint16  allTimeHigh = 0;

  float  nextAnalogGain = analogGainStart;
  while  (nextAnalogGain <= analogGainEnd)
  {
    agAnalogGain = nextAnalogGain;
    MakeAnalogGainRequest (agAnalogGain);
    WaitForOneFrame ();
    //osSleepMiliSecs (1000);
    kkuint16  hv = CameraHighValueOf3Frames ();
    allTimeHigh = Max (hv, allTimeHigh);
    highValues.push_back (hv);
    nextAnalogGain = nextAnalogGain + increment;
  }

  agAnalogGain = analogGainStart;
  for  (kkuint16 x = 0;  x < highValues.size ();  ++x)
  {
    if  (highValues[x] == allTimeHigh)
      break;
    agAnalogGain += increment;
  }

  MakeAnalogGainRequest (agAnalogGain);
}  /* SetAnalogGainToFirstHighInRange */




void  CameraAcquisitionPleora::RaiseDigitalGainUntilSaturated (kkint32  increments)
{
  log.Level (40) << "CameraAcquisitionPleora::RaiseDigitalGainUntilSaturated [" << increments << "]." << endl;
  int loopCount = 0;

  while  ((!TerminateFlag ())  &&  (!ShutdownFlag ()))
  {
    kkint32  nextDigitalGain = agDigitalGain + increments;
    if  (nextDigitalGain > digitalGainMax)
      break;

    MakeDigitalGainRequest (nextDigitalGain);
    agDigitalGain = nextDigitalGain;
    WaitForOneFrame ();
    if  (DigitalGainIsMaxed ())
    {
      agDigitalGain = Max ((kkint32)0, (agDigitalGain - increments));
      MakeDigitalGainRequest (nextDigitalGain);
      break;
    }
    ++loopCount;
    if  (loopCount > 10)
      break;
  }
}  /* RaiseDigitalGainUntilSaturated */




void  CameraAcquisitionPleora::RaiseDigitalGainUntilAtLeastHighValue (int  minHighValue)
{
  log.Level (40) << "CameraAcquisitionPleora::RaiseDigitalGainUntilAtLeastHighValue   MinHighValue[" << minHighValue << "]." << endl;
  WaitForOneFrame ();
  kkuint16  highValue = CameraHighValue ();
  if  (highValue >= minHighValue)
    return;

  kkint32  bestGuess = agDigitalGain * minHighValue / highValue;
  kkint32  increment = (bestGuess - agDigitalGain) / 3;

  while  ((!TerminateFlag ())  &&  (!ShutdownFlag ()) &&  (highValue < minHighValue))
  {
    if  (agDigitalGain >= digitalGainMax)
      break;
    agDigitalGain += increment;
    agDigitalGain = Min (agDigitalGain, digitalGainMax);
    MakeDigitalGainRequest (agDigitalGain);
    WaitForOneFrame ();
    highValue = CameraHighValue ();
  }

}  /* RaiseDigitalGainUntilAtLeastHighValue */





/*
 *@details
 *  This code was originally implemented for a Dalsa SG-11-02K80.
 *@code
 * Background:
 *   Camera: Dalsa SG-11-02K80
 *   Analog Gain (-10.0f thru 10.0f)
 *   Digital Gain (0 thru 5000)
 *
 *   Preference is to maximize Analog gain over Digital gain.
 *
 *  General Flow.
 *  1) Initialize Analog-Gain to known minimum value and Digital-Gain to 2000.
 *  2) Raise Digital Gain until High-Value in Scan line is at least 80.
 *  3) Raise Analog-Gain by increments of 2.0f until saturated.
 *  4) Raise Digital-Gain until high value in scan line is at least 200.
 *  5) Raise Analog-Gain by increments of 0.2f until saturated.
 *  6) Search for Analog-Gain that is in range of cur value -0.5 to + 1.0 selecting the 1st one
 *     to have "High-Value".
 *
 *    At this point we have the Analog-Gain value we want to live with; we now only need to 
 *    search for the Digital-Gain.
 *  
 *  7) Search for Digital-Gain start with Coarse(increments of a 1000) to fine using increment 
 *     of 100, and then 20.
 *@endcode
 */
void  CameraAcquisitionPleora::PerformAutoGainProcedureOld ()
{
  log.Level (10) << "CameraAcquisitionPleora::PerformAutoGainProcedure" << endl;

  bool sampleBeforeFlatField = Manager ()->SampleLastFrameBeforeFlatField ();
  Manager ()->SampleLastFrameBeforeFlatField (true);

  agAnalogGain  = analogGainMin;
  agDigitalGain = 2000;

  MakeAnalogGainRequest (agAnalogGain);
  MakeDigitalGainRequest (agDigitalGain);

  RaiseDigitalGainUntilAtLeastHighValue (80);
  if  ((TerminateFlag ())  ||  (ShutdownFlag ()))
    return;

  RaiseAnalogGainUntilSaturated2 (2.0f);
  if  ((TerminateFlag ())  ||  (ShutdownFlag ()))
    return;

  // Now that we should have a reasonable value for Analog-Gain we will start the Digital Gain at low value again
  agDigitalGain = 1000;
  MakeDigitalGainRequest (agDigitalGain);
  WaitForOneFrame ();

  RaiseDigitalGainUntilAtLeastHighValue (200);
  if  ((TerminateFlag ())  ||  (ShutdownFlag ()))
    return;

  agDigitalGain = (kkint32)(0.6f * (float)agDigitalGain);
  MakeDigitalGainRequest (agDigitalGain);
  WaitForOneFrame ();

  RaiseAnalogGainUntilSaturated2 (0.5f);
  if  ((TerminateFlag ())  ||  (ShutdownFlag ()))
    return;

  float  analogGainStart = Max (analogGainMin, (agAnalogGain - 0.5f));
  float  analogGainEnd   = Min (analogGainMax, (agAnalogGain + 0.5f));
  SetAnalogGainToFirstHighInRange (analogGainStart, analogGainEnd, 0.1f);

  RaiseDigitalGainUntilSaturated (1000);
  if  ((TerminateFlag ())  ||  (ShutdownFlag ()))
    return;

  RaiseDigitalGainUntilSaturated (100);
  if  ((TerminateFlag ())  ||  (ShutdownFlag ()))
    return;

  RaiseDigitalGainUntilSaturated (20);
  if  ((TerminateFlag ())  ||  (ShutdownFlag ()))
    return;

  Manager ()->SampleLastFrameBeforeFlatField (sampleBeforeFlatField);
}  /* PerformAutoGainProcedure */





void  CameraAcquisitionPleora::AnalogGainBinarySearch (kkuint16 threshholdSaturationValue)
{
  log.Level (20) << "CameraAcquisitionPleora::AnalogGainBinarySearch    " << endl;

  float  low  = Max (-10000.0f, analogGainMin);
  float  high = Min (10000.0f,  analogGainMax);

  float  deltaLowHigh = high - low;

  while  (deltaLowHigh > 0.1f)
  {
    float   mid = (low + high) / 2.0f;
    agAnalogGain = mid;
    MakeAnalogGainRequest (agAnalogGain);
    WaitForOneFrame ();
    kkuint16  cameraHighValue = CameraHighValue ();
    if  (cameraHighValue < threshholdSaturationValue)
      low = mid;
    else
      high = mid;
    deltaLowHigh = high - low;
  }

  agAnalogGain = low;
  MakeAnalogGainRequest (agAnalogGain);
  WaitForOneFrame ();

  log.Level (20) << "CameraAcquisitionPleora::AnalogGainBinarySearch   Done   AnalogGain[" << agAnalogGain << "]" << endl;
}   /* AnalogGainBinarySearch */





void  CameraAcquisitionPleora::DigitalGainBinarySearch ()
{
  log.Level (20) << "CameraAcquisitionPleora::DigitalGainBinarySearch" << endl;

  kkint32  low  = 1;
  kkint32  high = digitalGainMax;
  kkint32  deltaLowHigh = high - low;

  while  (deltaLowHigh > 10)
  {
    kkint32 mid = (low + high) / 2;
    agDigitalGain = mid;
    MakeDigitalGainRequest (agDigitalGain);
    WaitForOneFrame ();

    if  (DigitalGainIsMaxed ())
      high = mid;
    else
      low = mid;

    deltaLowHigh = high - low;
  }

  agDigitalGain = low;
  MakeDigitalGainRequest (agDigitalGain);
  WaitForOneFrame ();

  log.Level (20) << "CameraAcquisitionPleora::DigitalGainBinarySearch   Done   DigitalGain[" << agDigitalGain << "]" << endl;

}   /* DigitalGainBinarySearch */



/**
 *@details   Perform this with Analog-Gain maxed out so that area being imaged is obvious.
 */
void  CameraAcquisitionPleora::PerformAutoCrop ()
{
  log.Level (20) << "CameraAcquisitionPleora::PerformAutoCrop" << endl;
  VectorUcharPtr  scanLine = Manager ()->LastFrameHighValuesScanLine ();
  if  (!scanLine)
  {
    log.Level (-1) << "CameraAcquisitionPleora::PerformAutoCrop    ***ERROR***    No  'LastFrameHighValuesScanLine'  returned." << endl;
    return;
  }

  kkuint16  highPoint = 0;

  // We start from the 4th column because the first 4 columns may be carrying the "Flow-Meter" counter value.
  kkuint16  x = 4;
  for  (x = 4;  x < scanLine->size ();  ++x)
  {
    uchar v = (*scanLine)[x];
    if  (v > highPoint)
      highPoint = v;
  }

  kkuint16  threshhold = highPoint * 1 / 2;
  
  kkint32  midPoint = (kkint32)scanLine->size () / 2;

  kkint32  cropLeft = midPoint;
  while  ((cropLeft > 6)  && 
          (((*scanLine)[cropLeft] > threshhold)  ||  ((*scanLine)[cropLeft - 1] > threshhold))
         )
    --cropLeft;
  cropLeft += 2;

  kkint32  rightSide = (kkint32)scanLine->size ();
  kkint32  cropRight = midPoint;
  while  ((cropRight <  rightSide)  && 
          (((*scanLine)[cropRight] > threshhold)  ||  ((*scanLine)[cropRight - 1] > threshhold))
         )
    ++cropRight;
  cropRight -= 1;

  Manager ()->CropSettingsChanged (cropLeft, cropRight);

  delete  scanLine;
  scanLine = NULL;

  log.Level (50) << "CameraAcquisitionPleora::PerformAutoCrop    Exiting." << endl;

  return;
} /* PerformAutoCrop */



void  CameraAcquisitionPleora::PerformAutoGainProcedure ()
{
  log.Level (10) << "CameraAcquisitionPleora::PerformAutoGainProcedure" << endl;

  bool sampleBeforeFlatField = Manager ()->SampleLastFrameBeforeFlatField ();
  Manager ()->SampleLastFrameBeforeFlatField (true);

  // 1) Set Analog Gain to Max Value for a Fixed Digital-Gain value 
  //    a. The current high Image value will be used as the Threshold-Saturation-Value; we will use this value to search for Analog value to use.
  //    b. Determine Crop Settings.
  //
  // 2) Perform Binary Search for Analog-Gain value that is just below "


  agAnalogGain  = Min (10000.0f, analogGainMax);
  agDigitalGain = 2000;

  MakeAnalogGainRequest (agAnalogGain);
  MakeDigitalGainRequest (agDigitalGain);
  WaitForOneFrame ();
  WaitForOneFrame ();
  kkuint16  threshholdSaturationValue = CameraHighValue ();
  if  ((!TerminateFlag ())  &&  (!ShutdownFlag ()))
  {

    AnalogGainBinarySearch (threshholdSaturationValue);
    if  ((!TerminateFlag ())  &&  (!ShutdownFlag ()))
    {
      PerformAutoCrop ();
      DigitalGainBinarySearch ();
    }
  }

  Manager ()->SampleLastFrameBeforeFlatField (sampleBeforeFlatField);
}  /* PerformAutoGainProcedure */






