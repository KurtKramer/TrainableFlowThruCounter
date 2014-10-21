/* CameraAcquisitionPleoraDalsa.cpp -- Represents a single camera frame.
 * Copyright (C) 2011-2013  Kurt Kramer
 * For conditions of distribution and use, see copyright notice in LarcosCounterUnManaged.txt
 */
#include  "FirstIncludes.h"

#include <errno.h>
#include <istream>
#include <iostream>
#include <fstream>
#include <queue>
#include <vector>

#if  defined(OS_WINDOWS)
#include <windows.h>
#endif
#include "MemoryDebug.h"
using namespace std;


#include <PvDeviceFinderWnd.h>
#include <PvDevice.h>
#include <PvPipeline.h>
#include <PvBuffer.h>
#include <PvStream.h>
#include <PvStreamRaw.h>
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


#include "CameraAcquisitionPleoraDalsa.h"
#include "CameraAcquisition.h"
#include "CameraFrame.h"
#include "CameraFrameBuffer.h"
#include "CameraAcquisitionPleoraDalsa.h"
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



CameraAcquisitionPleoraDalsa::CameraAcquisitionPleoraDalsa 
                                     (LarcosCounterManagerPtr _manager,
                                      const KKStr&            _macAddress,
                                      CameraFrameBufferPtr    _frameBuffer,
                                      FlowMeterTrackerPtr     _flowMeter,
                                      MsgQueuePtr             _msgQueue,
                                      const KKStr&            _threadName
                                     ):
    CameraAcquisitionPleora (_manager, _macAddress, _frameBuffer, _flowMeter, _msgQueue, _threadName),

    lDevice                    (NULL),
    lDeviceParams              (NULL),
    lPipeline                  (NULL),
    lStream                    (NULL),
    lStreamParams              (NULL),

    lAcquisitionLinePeriod     (NULL),
    lAnalogGainAbs             (NULL),
    lCameraTemparature         (NULL),
    lDigitalGainRaw            (NULL),
    lExposureTime              (NULL),
    lFrameRate                 (NULL),
    lGain                      (NULL),
    lGainSelector              (NULL),
    lIPAddressParam            (NULL),
    lPayloadSize               (NULL),
    lReadVoltageAndTemperature (NULL),
    lScanRate                  (NULL),
    lSensitivityMode           (NULL),
    lStart                     (NULL),
    lStop                      (NULL),
    lTLLocked                  (NULL),


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
  if  (Manager ()->Installation ()->FlowMeterMethod () == InstallationConfig::fmmEmbedded)
    embeddedFlowMeter = true;
  else
    embeddedFlowMeter = false;
}



CameraAcquisitionPleoraDalsa::CameraAcquisitionPleoraDalsa 
                                     (LarcosCounterManagerPtr _manager,
                                      CameraParametersPtr     _cameraParams,
                                      CameraFrameBufferPtr    _frameBuffer,
                                      FlowMeterTrackerPtr     _flowMeter,
                                      MsgQueuePtr             _msgQueue,
                                      const KKStr&            _threadName
                                     ):
    CameraAcquisitionPleora (_manager, _cameraParams, _frameBuffer, _flowMeter, _msgQueue, _threadName),

    lDevice                    (NULL),
    lDeviceParams              (NULL),
    lPipeline                  (NULL),
    lStream                    (NULL),
    lStreamParams              (NULL),

    lAcquisitionLinePeriod     (NULL),
    lAnalogGainAbs             (NULL),
    lCameraTemparature         (NULL),
    lDigitalGainRaw            (NULL),
    lExposureTime              (NULL),
    lFrameRate                 (NULL),
    lGain                      (NULL),
    lGainSelector              (NULL),
    lIPAddressParam            (NULL),
    lPayloadSize               (NULL),
    lReadVoltageAndTemperature (NULL),
    lScanRate                  (NULL),
    lSensitivityMode           (NULL),
    lStart                     (NULL),
    lStop                      (NULL),
    lTLLocked                  (NULL),


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
  if  (Manager ()->Installation ()->FlowMeterMethod () == InstallationConfig::fmmEmbedded)
    embeddedFlowMeter = true;
  else
    embeddedFlowMeter = false;
}



CameraAcquisitionPleoraDalsa::~CameraAcquisitionPleoraDalsa ()
{
}



void  CameraAcquisitionPleoraDalsa::InitializeDeviceParameterAccessVariables ()
{
  lDeviceParams = lDevice->GetGenParameters();
  lIPAddressParam            = dynamic_cast<PvGenInteger *> (lDeviceParams->Get ("IPAddressParam"));
  lTLLocked                  = dynamic_cast<PvGenInteger *> (lDeviceParams->Get ("TLParamsLocked"));
  lPayloadSize               = dynamic_cast<PvGenInteger *> (lDeviceParams->Get ("PayloadSize"));
  lStart                     = dynamic_cast<PvGenCommand *> (lDeviceParams->Get ("AcquisitionStart"));
  lStop                      = dynamic_cast<PvGenCommand *> (lDeviceParams->Get ("AcquisitionStop"));
  lReadVoltageAndTemperature = dynamic_cast<PvGenCommand *> (lDeviceParams->Get ("ReadVoltageAndTemperature"));
  lScanRate                  = dynamic_cast<PvGenFloat   *> (lDeviceParams->Get ("AcquisitionLineRateAbs"));
  lGainSelector              = dynamic_cast<PvGenEnum    *> (lDeviceParams->Get ("GainSelector"));
  lAnalogGainAbs             = dynamic_cast<PvGenFloat   *> (lDeviceParams->Get ("GainAbs"));
  lDigitalGainRaw            = dynamic_cast<PvGenInteger *> (lDeviceParams->Get ("DigitalGainRaw"));

  lSensitivityMode           = dynamic_cast<PvGenEnum    *> (lDeviceParams->Get ("SensitivityMode"));
  lGain                      = dynamic_cast<PvGenInteger *> (lDeviceParams->Get ("Gain"));
  lAcquisitionLinePeriod     = dynamic_cast<PvGenInteger *> (lDeviceParams->Get ("AcquisitionLinePeriod"));
  lExposureTime              = dynamic_cast<PvGenInteger *> (lDeviceParams->Get ("ExposureTime"));
  lCameraTemparature         = dynamic_cast<PvGenInteger *> (lDeviceParams->Get ("CameraTemparature"));

  if  (!lCameraTemparature)
    lCameraTemparature = dynamic_cast<PvGenInteger *>(lDeviceParams->Get ("DeviceTemperature"));
}  /* InitializeDeviceParameterAccessVariables */



void  CameraAcquisitionPleoraDalsa::InitializeStreamParameterAccessVariables ()
{
  lStreamParams = lStream->GetParameters ();
  lFrameRate    = dynamic_cast<PvGenFloat *>  (lStreamParams->Get ("AcquisitionRateAverage"));
}




const char*  CameraAcquisitionPleoraDalsa::PvGenTypeToCStr (PvGenType  genType)
{
  const char* s = "UnKnown";
  switch  (genType)
  {
  case  PvGenTypeInteger:    s = "Integer";    break;
  case  PvGenTypeEnum:       s = "Enum";       break;
  case  PvGenTypeBoolean:    s = "Boolean";    break;
  case  PvGenTypeString:     s = "String";     break;
  case  PvGenTypeCommand:    s = "Command";    break;
  case  PvGenTypeFloat:      s = "Float";      break;
  case  PvGenTypeRegister:   s = "regisetr";   break;
  case  PvGenTypeUndefined:  s = "Undefined";  break;
  default:
    s = "UnKnown";
  }

  return s;
}  /* PvGenTypeToCStr */



double  CameraAcquisitionPleoraDalsa::GetPvParameterFloat (PvGenParameterArray*  params,
                                                      const KKStr&          paramName,
                                                      double                defaultValue  /**< Value if parameter not available. */
                                                     )
{
  if  (!params)
    return defaultValue;

  double result = defaultValue;

  PvGenParameter*  param = params->Get (paramName.Str ());
  if  (param)
  {
    PvGenType  pvGenType;
    if  (param->GetType (pvGenType).IsOK ())
    {
      if  (pvGenType == PvGenTypeFloat)
      {
        PvGenFloat*  pvGenFloat = dynamic_cast<PvGenFloat *> (param);
        double  pvFloat;
        if  (pvGenFloat->GetValue (pvFloat).IsOK ())
          result = pvFloat;
      }
    }
    else
    {
      PvString  pvString;
      if  (param->ToString (pvString).IsOK ())
      {
        KKStr  s = pvString.GetAscii ();
        result = s.ToDouble ();
      }
    }
  }

  return result;
}  /* GetPvParameterFloat */



int64  CameraAcquisitionPleoraDalsa::GetPvParameterInteger (PvGenParameterArray*  params,
                                                       const KKStr&          paramName,
                                                       int64                 defaultValue
                                                      )
{
  if  (!params)
    return defaultValue;

  int64  result = defaultValue;

  PvGenParameter*  param = params->Get (paramName.Str ());
  if  (param)
  {
    PvGenType  pvGenType;
    if  (param->GetType (pvGenType).IsOK ())
    {
      if  (pvGenType == PvGenTypeInteger)
      {
        PvGenInteger*  pvGenInteger = dynamic_cast<PvGenInteger *> (param);
        PvInt64  pvInt;
        if  (pvGenInteger->GetValue (pvInt).IsOK ())
          result = pvInt;
      }
    }
    else
    {
      PvString  pvString;
      if  (param->ToString (pvString).IsOK ())
      {
        KKStr  s = pvString.GetAscii ();
        result = s.ToInt64 ();
      }
    }
  }

  return result;
}  /* GetPvParameterInteger */



int32  CameraAcquisitionPleoraDalsa::GetPvParameterEnum (PvGenParameterArray*  params,
                                                    const KKStr&          paramName,
                                                    int32                 defaultValue
                                                   )
{
  if  (!params)
    return defaultValue;

  int64  result = defaultValue;

  PvGenParameter*  param = params->Get (paramName.Str ());
  if  (param)
  {
    PvGenType  pvGenType;
    if  (param->GetType (pvGenType).IsOK ())
    {
      if  (pvGenType == PvGenTypeEnum)
      {
        PvGenEnum*  pvGenEnum = dynamic_cast<PvGenEnum *> (param);
        PvInt64  pvInt;
        if  (pvGenEnum->GetValue (pvInt).IsOK ())
          result = pvInt;
      }
    }
    else
    {
      PvString  pvString;
      if  (param->ToString (pvString).IsOK ())
      {
        KKStr  s = pvString.GetAscii ();
        result = s.ToInt64 ();
      }
    }
  }

  return (int32)result;
}  /* GetPvParameterEnum */




KKStr  CameraAcquisitionPleoraDalsa::GetPvParameterEnumStr (PvGenParameterArray*  params,
                                                       const KKStr&          paramName,
                                                       const KKStr&          defaultValue
                                                      )
{
  if  (!params)
    return defaultValue;

  KKStr  result = defaultValue;

  PvGenParameter*  param = params->Get (paramName.Str ());
  if  (param)
  {
    PvString  pvString;
    if  (param->ToString (pvString).IsOK ())
     result  = pvString.GetAscii ();
  }

  return  result;
}  /* GetPvParameterEnumStr */



void   CameraAcquisitionPleoraDalsa::CameraParametersPopulate (PvGenParameterArray*  params,
                                                          CameraParameters&     cameraParams,
                                                          RunLog&               runLog
                                                         )
{
  runLog.Level (40) << "CameraAcquisitionPleoraDalsa::CameraParametersPopulate " << ((params == NULL) ? "***  params == NULL  ***" : "") << endl;

  if  (!params)
    return;

  int32  acquisitionLinePeriod = 0;
  int32  exposureTime = 0;
  float  scanRate = 0.0f;
  float  analogGain = 0.0f;

  analogGain = (float)GetPvParameterFloat (params, "GainAbs", -1.0);
  scanRate = (float)GetPvParameterFloat  (params, "AcquisitionLineRateAbs", 0.0);

  cameraParams.DeviceVendorName (GetPvParameterString         (params, "DeviceVendorName",       ""));
  cameraParams.DeviceModelName  (GetPvParameterString         (params, "DeviceModelName",        ""));
  cameraParams.DeviceVersion    (GetPvParameterString         (params, "DeviceVersion",          ""));
  cameraParams.DigitalGain      ((int32)GetPvParameterInteger (params, "DigitalGainRaw",         -1));
  cameraParams.FrameHeight      ((int32)GetPvParameterInteger (params, "Height",                  0));
  cameraParams.FrameWidth       ((int32)GetPvParameterInteger (params, "Width",                   0));
  cameraParams.Model            (GetPvParameterString         (params, "DeviceModelName",        ""));
  cameraParams.SensitivityMode  (GetPvParameterEnumStr        (params, "SensitivityMode",        -1));

  int64  ipAddr = GetPvParameterInteger (params, "GevCurrentIPAddress",    0);
  cameraParams.IpAddress (Int64ToIpAddress (ipAddr));

  if  (cameraParams.DeviceModelName ().EqualIgnoreCase ("DiviinaLM1GE"))
  {
    acquisitionLinePeriod = (int32)GetPvParameterInteger (params, "AcquisitionLinePeriod", -1);
    exposureTime          = (int32)GetPvParameterInteger (params, "ExposureTime",          -1);
    analogGain            = (float)GetPvParameterInteger (params, "Gain",                  -1);
  }

  if  ((scanRate <= 0.0f)  &&  (acquisitionLinePeriod > 0))
    scanRate = 1000000.0f / (float)acquisitionLinePeriod;

  else if  ((scanRate > 0.0f)  &&  (acquisitionLinePeriod <= 0))
    acquisitionLinePeriod = (int32)(0.5f + 1000000.0f / scanRate);

  cameraParams.AnalogGain (analogGain);
  cameraParams.LinePeriod (acquisitionLinePeriod);
  cameraParams.ScanRate   (scanRate);


  //result->serialNumber     = GetPvParameterString (lDeviceParams, "???????????????");
  //result->macAddress       = GetPvParameterString (lDeviceParams, "DeviceModelName");
  return;
}  /* CameraParametersPopulate */




void  CameraAcquisitionPleoraDalsa::ApplyCommandEntries ()
{
  log.Level (10) << "CameraAcquisitionPleoraDalsa::ApplyCommandEntries" << endl;
  while  (QueueActive ())
    ApplyCommandNextEntry ();
}  /* ApplyCommandEntries */



void  CameraAcquisitionPleoraDalsa::ApplyCommandNextEntry ()
{
  if  (!QueueActive ())
    return;

  CommandEntryPtr  cmd = GetNextCommandEntry ();
  if  (cmd)
  {
    switch  (cmd->CommandType ())
    {
    case  CommandEntry::ceAnalogGain:
      SetAnalogGain (cmd->AnalogGain ());
      break;

    case  CommandEntry::ceDigitalGain:
      SetDigitalGain (cmd->DigitalGain ());
      break;

    case  CommandEntry::ceScanRate:
      SetScanRate (cmd->ScanRate ());
      break;

    case  CommandEntry::ceSensitivityMode:
      SetSensitivityMode (cmd->SensitivityMode ());
      break;
    }

    delete  cmd;
    cmd = NULL;
  }
}  /* ApplyCommandNextEntry */






void  CameraAcquisitionPleoraDalsa::SetCameraParametersForFlowMeter ()
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





void  CameraAcquisitionPleoraDalsa::ConnectToCamera (bool&  connectionSuccessful)
{
  log.Level (10) << "ConnectToCamera" << endl;
  connectionSuccessful = false;
  StartStatus (ssConnecting, "");

  Manager ()->AddSecondaryMsg ("Connecting to Camera.");

  if  (lPipeline  ||  lStream  ||  lDevice)
  {
    log.Level (10) << "ConnectToCamera  Previous connection not cleaned up need to call 'DisconnectFromCamera'." << endl;
    DisconnectFromCamera ();
    log.Level (40) << "ConnectToCamera    back from 'DisconnectFromCamera' can now proceed to connect to camera." << endl;
  }

  // Connect to the GEV Device
  lDevice = new PvDevice ();

  KKStr  status;
  status << "Connecting to Mac Address: " << MacAddress ();
  log.Level (10) << status << endl;

  PvString  pcMacAddress = MacAddress ().Str ();

  log.Level (40) << "ConnectToCamera   Establishing connection to :" << MacAddress () << endl;
  PvResult  pvConnectResult = lDevice->Connect (pcMacAddress);
  if   (!pvConnectResult.IsOK ())
  {
    status = "";
    status << "Unable to connect to Mac-Address: " << MacAddress ();
    log.Level (-1) << "ConnectToCamera   ***ERROR*** Tryng to connect :" << PvResultToStr (pvConnectResult) << endl
                   << "   " << status << endl;
    StartStatus (ssConnectionFailed, status);

    PvString  pvIpAddress               = cameraParams->IpAddress  ().Str ();
    PvString  pvMacAddress              = cameraParams->MacAddress ().Str ();
    PvString  pvInterfaceSubnetMask     = cameraParams->InterfaceSubnetMask ().Str ();
    PvString  pvInterfaceDefaultGateway = cameraParams->InterfaceDefaultGateway ().Str ();

    log.Level (40) << "ConnectToCamera  Attempting to configure IP address of Mac :" << cameraParams->MacAddress () << "  to IP :" << cameraParams->IpAddress () << endl;

    PvResult pvr = PvDevice::SetIPConfiguration (pvMacAddress, 
                                                 pvIpAddress, 
                                                 pvInterfaceSubnetMask, 
                                                 pvInterfaceDefaultGateway
                                                );
    if  (!pvr.IsOK ())
    {
      log.Level (-1) << "ConnectToCamera  ***ERROR***  Setting IP Address  " << PvResultToStr (pvr) << endl;
    }
    else
    {
      log.Level (40) << "ConnectToCamera  IP Address succesfully cnfigured  will now try connecting again." << endl;
      pvConnectResult = lDevice->Connect (pvMacAddress);
    }

    if  (pvConnectResult.IsOK ())
    {
      log.Level (30) << "ConnectToCamera   Connection successful :" << PvResultToStr (pvConnectResult) << endl;
    }
    else
    {
      log.Level (30) << "Failed to connect after setting IP :" << PvResultToStr (pvConnectResult) << endl;
      delete lDevice;
      lDevice = NULL;
      return;
    }
  }

  status = "";
  status << "Successfully connected to MacAddress: " << MacAddress ();
  log.Level (10) << status << endl;

  //PrintDeviceParameters ();

  // Get device parameters needed to control streaming
  InitializeDeviceParameterAccessVariables ();

  if  (embeddedFlowMeter)
    SetCameraParametersForFlowMeter ();

  CameraParametersPopulate (lDeviceParams, *cameraParams, log);
  Manager ()->FrameHeightWidthChanged (cameraParams->FrameHeight (), cameraParams->FrameWidth ());
  curScanRate    = cameraParams->ScanRate ();
  curAnalogGain  = cameraParams->AnalogGain ();
  curDigitalGain = cameraParams->DigitalGain ();

  if  ((requestedScanRate > 1)  &&  (requestedScanRate < 36000))
    AddScanRateCommand (requestedScanRate);

  if  ((requestedAnalogGain >= analogGainMin)  &&  (requestedAnalogGain <= analogGainMax))
    AddAnalogGainCommand (requestedAnalogGain);

  if  ((requestedDigitalGain >= 0)  &&  (requestedDigitalGain <= digitalGainMax))
    AddDigitalGainCommand (requestedDigitalGain);

  analogGainMin = -7.0f;
  digitalGainMax = 10000;
  if  (lAnalogGainAbs)
  {
    // Retrieve Min-Max analogGain values.
    analogGainMin = -7.0f;
    double  min = 0.0;
  
    PvResult  getMinResult = lAnalogGainAbs->GetMin (min);
    if  (getMinResult.IsOK ())
    {
      log.Level (50) << "ConnectToCamera  AnalogGainAbs::Min :" << min << endl;
      if  (min < -1000.0f)
        analogGainMin = -1000.0f;
      else
        analogGainMin =  (float)min;
    }
    else
    {
      log.Level (40) << "ConnectToCamera   ***ERROR***  AnalogGainAbs.GetMin() :" << PvResultToStr (getMinResult) << endl;
    }

    double  max = 0.0;
    PvResult getMaxResult = lAnalogGainAbs->GetMax (max);
    if  (getMaxResult.IsOK ())
    {
      log.Level (50) << "ConnectToCamera   AnalogGainAbs::Max :" << max << endl;
      if  (max > 1000.0f)
        analogGainMax = 1000.0f;
      else
        analogGainMax = (float)max;
    }
    else
    {
      log.Level (40) << "ConnectToCamera   *** ERROR ***  AnalogGainAbs.GetMin()[" << PvResultToStr (getMaxResult) << "]." << endl;
    }
  }

  // Negotiate streaming packet size
  log.Level (20) << "ConnectToCamera   Negotiating Streaming Packet Size." << endl;
  PvResult  negotiateResult = lDevice->NegotiatePacketSize ();
  if  (!negotiateResult.IsOK ())
    log.Level (-1) << "ConnectToCamera  ***ERROR***   NegotiatePacketSize failed :" << PvResultToStr (negotiateResult) << endl;
  else
    log.Level (40) << "ConnectToCamera  NegotiatePacketSize Successful :" << PvResultToStr (negotiateResult) << endl;

  log.Level (20) << "ConnectToCamera   Opening stream to device." << endl;
  lStream = new PvStream ();
  PvResult  streamOpenResult = lStream->Open (IpAddress ().Str ());
  if  (!streamOpenResult.IsOK ())
  { 
    log.Level (-1) << "ConnectToCamera   ***ERROR***    Unable to open Camera Stream :" << PvResultToStr (streamOpenResult) << endl;
    delete  lStream;
    lStream = NULL;
    return;
  }
  else
  {
    log.Level (40) << "ConnectToCamera  Camera Stream Opened :" << PvResultToStr (streamOpenResult) << endl;
  }


  lPipeline = new PvPipeline (lStream);

  PvInt64 payloadSize = 0;
  
  PvResult getPayloadSizeResult = lPayloadSize->GetValue (payloadSize);
  if  (!getPayloadSizeResult.IsOK ())
    log.Level (-1) << "ConnectToCamera   ***ERROR***  Error retrieving PayLoadSize :" << PvResultToStr (getPayloadSizeResult) << endl;
  else
    log.Level (40) << "ConnectToCamera   PayLoadSize Retrieved :" << (int64)payloadSize << " :" << PvResultToStr (getPayloadSizeResult) << endl;


  lPipeline->SetBufferSize (static_cast<PvUInt32> (payloadSize));

  PvResult  bufferCountResult = lPipeline->SetBufferCount (64); // Increase for high frame rate without missing block IDs
  if  (bufferCountResult.IsOK ())
    log.Level (40) << "ConnectToCamera   BufferCount successfuly set to :" << 64 << " :" << PvResultToStr (bufferCountResult) << endl;
  else
    log.Level (-1) << "ConnectToCamera   ***ERROR***   Setting BufferCount :" << PvResultToStr (bufferCountResult) << endl;
 
  // Have to set the Device IP destination to the Stream

  PvString localIPAddress = lStream->GetLocalIPAddress ();
  PvUInt16 localPort      = lStream->GetLocalPort ();

  PvResult streamDestResult = lDevice->SetStreamDestination (localIPAddress, localPort);
  if  (streamDestResult.IsOK ())
    log.Level (40) << "ConnectToCamera   Device StreamDestination set to :" << localIPAddress.GetAscii () << " : " << localPort << endl;
  else
    log.Level (-1) << "ConnectToCamera   ***ERROR***   Setting Device-Stream-Destination :" << PvResultToStr (streamDestResult) << endl;


  // TLParamsLocked is optional but when present, it MUST be set to 1 before sending the AcquisitionStart command
  if  (lTLLocked != NULL)
  {
    log.Level (40) << "ConnectToCamera   Setting TLParamsLocked to 1" << endl;
    PvResult tlParamsLockedResult = lTLLocked->SetValue (1);
    if  (tlParamsLockedResult.IsOK ())
      log.Level (40) << "ConnectToCamera  'tlParamsLocked' successfully set to '1' :" << PvResultToStr (tlParamsLockedResult) << endl;
    else
      log.Level (-1) << "ConnectToCamera  ***ERROR***  Failure setting 'tlParamsLocked' to 1 :" << PvResultToStr (tlParamsLockedResult) << endl;
  }


  // IMPORTANT: the pipeline needs to be "armed", or started before we instruct the device to send us images
  log.Level (20) << "ConnectToCamera   Starting pipeline" << endl;
  PvResult pipeLineStartResult = lPipeline->Start ();
  if  (pipeLineStartResult.IsOK ())
    log.Level (40) << "ConnectToCamera   Pipeline Started Successfully." << endl;
  else
    log.Level (-1) << "ConnectToCamera   ***ERROR*** Starting Pipeline :" << PvResultToStr (pipeLineStartResult) << endl;
 
  // Get stream parameters/stats
  InitializeStreamParameterAccessVariables ();


  log.Level (40) << "ConnectToCamera   Retrieving  'GevTimestampControlReset' parameter." << endl;
  PvGenCommand *lResetTimestamp = dynamic_cast<PvGenCommand *>(lDeviceParams->Get ("GevTimestampControlReset"));
  if  (!lResetTimestamp)
  {
    log.Level (40) << "ConnectToCamera   Parameter 'GevTimestampControlReset'  not retrieved." << endl;
  }
  else
  {
    log.Level (40) << "ConnectToCamera  Executing 'GevTimestampControlReset'." << endl;
    PvResult timestampControlResetResult = lResetTimestamp->Execute ();
    if  (timestampControlResetResult.IsOK ())
      log.Level (40) << "ConnectToCamera  'GevTimestampControlReset' successfully executed :" << PvResultToStr (timestampControlResetResult) << endl;
    else
      log.Level (-1) << "ConnectToCamera  ***ERROR***  'GevTimestampControlReset' execute failed :" << PvResultToStr (timestampControlResetResult) << endl;
  }


  {
    // Setting "MaximumResendGroupSize" to 30;  this helps to reduce if not eliminate 
    // "2819,TOO_MANY_CONSECUTIVE_RESENDS"  errors when retrieving camera frame buffers.
    PvGenInteger*  maxResendGroupSize = dynamic_cast<PvGenInteger *> (lStreamParams->Get ("MaximumResendGroupSize"));
    if  (maxResendGroupSize)
    {
      PvResult maxResendGroupSizeResult;

      PvInt64  curValue = 0;
      maxResendGroupSizeResult = maxResendGroupSize->GetValue (curValue);
      if  (!maxResendGroupSizeResult.IsOK ())
      {
        log.Level (-1) << "ConnectToCamera   ***ERROR***   Unable to get current 'MaximumResendGroupSize' value." << endl
                       << "                  " << PvResultToStr (maxResendGroupSizeResult) << endl;
        curValue = 0;
      }
       
      if  (curValue >= 30)
      {
        log.Level (10) << "ConnectToCamera   MaximumResendGroupSize: " << curValue << "  Not going to override." << endl;
      }
      else
      {
        maxResendGroupSizeResult = maxResendGroupSize->SetValue (30);
        if  (!maxResendGroupSizeResult.IsOK ())
        {
          log.Level (-1) << "ConnectToCamera   ***ERROR***   Unable to set 'MaximumResendGroupSize' to 30" << endl
                         << "                  " << PvResultToStr (maxResendGroupSizeResult) << endl;
        }
        else
        {
          log.Level (10) << "ConnectToCamera   MaximumResendGroupSize: Set to 30." << endl;
        }
      }
    }
  }

  if  (Manager ()->OperatingMode () == LarcosCounterManager::lomAdvanced)
    Manager ()->AddSecondaryMsg ("Setting Camera Parameters.");
  ApplyCommandEntries ();

  // The pipeline is already "armed", we just have to tell the device to start sending us images
  log.Level (10) << "ConnectToCamera   Sending Start command to device." << endl;
  PvResult deviveStartResult = lStart->Execute();
  if  (deviveStartResult.IsOK ())
    log.Level (40) << "ConnectToCamera   Start command successful :" << PvResultToStr (deviveStartResult) << endl;
  else
    log.Level (-1) << "ConnectToCamera   ***ERROR***   Start Command failed :" << PvResultToStr (deviveStartResult) << endl;

  if  (embeddedFlowMeter)
  {
    AddPleoraVariableToHeaderField ("PLC_Q1_Variable0");
    AddPleoraVariableToHeaderField ("PLC_Q1_Operator0");
    AddPleoraVariableToHeaderField ("PLC_Q1_Variable1");
    AddPleoraVariableToHeaderField ("LineSelector");
    AddPleoraVariableToHeaderField ("LineFormat");
    AddPleoraVariableToHeaderField ("OutputSelector");
    AddPleoraVariableToHeaderField ("OutputFormat");
    AddPleoraVariableToHeaderField ("CounterSelector");
    AddPleoraVariableToHeaderField ("PLC_I0");
    AddPleoraVariableToHeaderField ("PLC_Q17_Variable0");
    AddPleoraVariableToHeaderField ("PLC_Q17_Operator0");
    AddPleoraVariableToHeaderField ("PLC_Q17_Variable1");
    AddPleoraVariableToHeaderField ("GevTimestampCounterSelector");
    AddPleoraVariableToHeaderField ("GrbCh0AcqCfgIncludeMetadataInImage");
    AddPleoraVariableToHeaderField ("GrbCh0MetadataInsertionMode");
  }

  CameraParametersAddToHeaderFields ();

  connectionSuccessful = true;

  Manager ()->AddSecondaryMsg ("Camera Connection Established!");

  log.Level (40) << "CameraAcquisitionPleoraDalsa::ConnectToCamera    Exiting." << endl;

  return;
}  /* ConnectToCamera */




void  CameraAcquisitionPleoraDalsa::DisconnectFromCamera ()
{
  log.Level (10) << endl << "DisconnectFromCamera" << endl;
  Manager ()->AddSecondaryMsg ("Disconnecting from Camera.");
  if  (lStop)
  {
    log.Level (20) << "DisconnectFromCamera   Executing Stop Command." << endl;
    PvResult stopResult = lStop->Execute();
    log.Level (40) << "DisconnectFromCamera   Stop Command Results :" << this->PvResultToStr (stopResult) << endl;
  }

  // If present reset TLParamsLocked to 0. Must be done AFTER the 
  // streaming has been stopped
  if  (lTLLocked != NULL)
  {
    log.Level (20) << "DisconnectFromCamera   Resetting TLParamsLocked to 0." << endl;
    PvResult lockResult = lTLLocked->SetValue (0);
    log.Level (40) << "DisconnectFromCamera   Resetting Results :" << PvResultToStr (lockResult) << endl;
  }
  
  if  (lPipeline)
  {
    // We stop the pipeline - letting the object lapse out of 
    // scope would have had the destructor do the same, but we do it anyway

    log.Level (20) << "DisconnectFromCamera   Stopping pipeline." <<endl;
    PvResult pipeLineResult = lPipeline->Stop();
    log.Level (40) << "DisconnectFromCamera   Stopping Results :" <<  PvResultToStr (pipeLineResult) << endl;
    delete  lPipeline;
    lPipeline = NULL;
  }

  if  (lStream)
  {
    // Now close the stream. Also optionnal but nice to have
    log.Level (20) << "DisconnectFromCamera   Closing Stream." << endl;
    PvResult streamResult = lStream->Close();
    log.Level (40) << "DisconnectFromCamera   Closing Stream Result :" << PvResultToStr (streamResult) << endl;
    delete  lStream;
    lStream = NULL;
  }

  if  (lDevice)
  {
    // Finally disconnect the device. Optional, still nice to have
    log.Level (20) << "DisconnectFromCamera    Disconnecting From Device." << endl;
    PvResult deviceResult = lDevice->Disconnect();
    log.Level (40) << "DisconnectFromCamera    Disconnecting From Device :" << PvResultToStr (deviceResult) << endl;
    delete  lDevice;
    lDevice = NULL;
  }

  lStreamParams              = NULL;
  lFrameRate                 = NULL;
  lCameraTemparature         = NULL;
  lTLLocked                  = NULL;
  lPayloadSize               = NULL;
  lStart                     = NULL;
  lReadVoltageAndTemperature = NULL;
  lStop                      = NULL;
  lScanRate                  = NULL;
  lGainSelector              = NULL;
  lAnalogGainAbs             = NULL;
  lDigitalGainRaw            = NULL;
  lGain                      = NULL;
  lExposureTime              = NULL;
  lAcquisitionLinePeriod     = NULL;

  Manager ()->AddSecondaryMsg ("Disconnected From Camera.");

  log.Level (40) << "CameraAcquisitionPleoraDalsa::DisconnectFromCamera    Exiting." << endl;

  return;
}  /* DisconnectFromCamera */




void  CameraAcquisitionPleoraDalsa::ComputeFramneIntervals ()
{
  if  (curScanRate <= 1.0f)
    return;

  // Compoute the number of frames that occur in a 5 second period.
  oneSecFrameInterval   = (int32)(0.5f + curScanRate *  1.0f / (float)FrameHeight ());
  twoSecFrameInterval   = oneSecFrameInterval *  2;
  threeSecFrameInterval = oneSecFrameInterval *  3;
  fiveSecFrameInterval  = oneSecFrameInterval *  5;
  tenSecFrameInterval   = oneSecFrameInterval * 10;
  voltTempReadInterval  = oneSecFrameInterval * 10;
}


void  CameraAcquisitionPleoraDalsa::Run ()
{
  Status (KKThread::tsRunning);

  log.Level (10) << "CameraAcquisitionPleoraDalsa::Run" << endl;

  KKStr  status (128);

  StartStatus (this->ssConnecting, "Connecting to Camera.");

  bool  connectionSuccessful = false;
  ConnectToCamera (connectionSuccessful);
  if  (!connectionSuccessful)
  {
    StartStatus (ssConnectionFailed, "Connection FAILED!!!.");
    Manager ()->AddSecondaryMsg ("");
    status = "";
    status << "Unable to connect to Mac-Address: " << MacAddress ();
    log.Level (10) << status << endl;
    DisconnectFromCamera ();
    Status (KKThread::tsStopping);
    Crashed (true);
    return;
  }

  int32  loopCount = 0;
  int32  whosTurn = 0;  /**< Will be used to determine who's turn to talk to camera;  FrameRate == 0;  CameraTemperature == 1 */

  lastBlockId = 0;

  totalLostPackets = 0;

  StartStatus (ssConnected, "Camera Connected.");

  ComputeFramneIntervals ();

  ofstream* flowRate = NULL;
  int32  flowRateScanLineCount = 0;
  int32  lastFlowRateCount = -1;
  if  (generateFlowRateReport)
  {
    DateTime dt = KKB::osGetLocalDateTime ();
    KKStr  flowMeterName = "C:\\Temp\\FlowMeter_" + dt.Date ().YYYYMMDD () + "-" + dt.Time ().HHMMSS () + ".txt";
    flowRate = new ofstream (flowMeterName.Str ());
    *flowRate << "Frame"  << "\t" << "Scan"  << "\t"  << ""    << "\t"  << ""    << "\t"  << ""    << "\t"  << ""    << "\t"  << ""    << "\t"  << "Total" << endl;
    *flowRate << "num"    << "\t" << "Line"  << "\t"  << "P0"  << "\t"  << "P1"  << "\t"  << "P2"  << "\t"  << "P3"  << "\t"  << "P4"  << "\t"  << "Count" << endl;
  }

  // Acquire images until the user instructs us to stop
  log.Level (10) << "CameraAcquisitionPleoraDalsa::Run   Image Acquisition Loop Started." << endl;
  while  (!TerminateFlag ()  &&  !ShutdownFlag ())
  {
    if  (!lPipeline)
    {
      // We are not communicating to the camera lets try reconnecting.
      StartStatus (ssDisconnected, "Camera Disconnected !!!  Attempting to reconect.");

      log.Level (-1) << endl << "CameraAcquisitionPleoraDalsa::Run    Will Sleep for 2 Seconds." << endl << endl;

      osSleepMiliSecs (1000);
      ConnectToCamera (connectionSuccessful);
      if  (connectionSuccessful)
      {
        StartStatus (ssConnected, "Camera Connected.");
        if  (Manager ()->OperatingMode () == LarcosCounterManager::lomAdvanced)
          Manager ()->AddSecondaryMsg ("Re-connected to Camera.");

        log.Level (-1) << endl << "CameraAcquisitionPleoraDalsa::Run    *** Conection Re-Established  ***" << endl << endl;

        lastBlockId = 0;
      }
      else
      {
        log.Level (-1) << endl << "CameraAcquisitionPleoraDalsa::Run    ***ERROR***    Camera is not talking to us." << endl << endl;
        DisconnectFromCamera ();
      }
    }
    if  (lPipeline)
    {
      if  (QueueActive ())
        ApplyCommandEntries ();

      // Retrieve next buffer		
      PvBuffer *lBuffer = NULL;
      PvResult  lOperationResult;

      PvResult lResult = lPipeline->RetrieveNextBuffer (&lBuffer, 2000, &lOperationResult);
      if  (!lResult.IsOK ())
      {
        KKStr  kkMsg (100);
        kkMsg << "lPipeline->RetrieveNextBuffer  ***ERROR***   Retrieving NextBuffer: " <<  PvResultToStr (lResult);
        log.Level (-1) << kkMsg << endl;
        StartStatus (ssDisconnected, kkMsg);
        if  (Manager ()->OperatingMode () == LarcosCounterManager::lomAdvanced)
          Manager ()->AddSecondaryMsg ("Error Retrieving Frame from Camera.");
        DisconnectFromCamera ();
      }
      else
      {
        uint32  pvOutputQueueSize = lPipeline->GetOutputQueueSize ();

        if  (!lOperationResult.IsOK ())
        {
          KKStr  resultStr = PvResultToStr (lOperationResult);
          log.Level (-1) << "lPipeline->RetrieveNextBuffer   ***WARNING***  :" << resultStr << endl
                         << "    pvOutputQueueSize: " << pvOutputQueueSize << endl;
          if  (Manager ()->OperatingMode () == LarcosCounterManager::lomAdvanced)
            Manager ()->AddSecondaryMsg ("Camera Timed Out.");
        }
        else
        {
          const PvUInt8*  rawData = lBuffer->GetDataPointer ();

          int32   lostPacketCount = lBuffer->GetLostPacketCount ();
          PvImage* i = lBuffer->GetImage ();
          uint32  imageSize = i->GetImageSize ();

          uint64  blockId = lBuffer->GetBlockID ();
          int64  numBlocksSkipped = blockId - (lastBlockId + 1);
          if  (numBlocksSkipped < 0)
            numBlocksSkipped += 65535;

          if  (numBlocksSkipped > 0)
          {
            log.Level (-1) << "CameraAcquisitionPleoraDalsa::Run   ***ERROR***  'blockId's Skipped   LastBlockId: " << lastBlockId << "   BlockId: " << blockId << "   Approx ScanLine: " << scanLinesRead << endl;
            physicalSeqNumsSkipped += (int32)numBlocksSkipped;
          }

          lastBlockId = blockId;

          totalLostPackets += lostPacketCount;

          loopCount++;
          if  ((loopCount % fiveSecFrameInterval) == 0)
          {
            switch  (whosTurn)
            {
            case 0:
              {
                if  (lFrameRate)
                {
                  double  frameRateD = 0.0;
                  lFrameRate->GetValue (frameRateD);
                  frameRate = (float)frameRateD;
                }
              }
              break;

            case 1:
              {
                if  (lCameraTemparature  &&  lReadVoltageAndTemperature)
                {
                  PvResult tempReadResult = lReadVoltageAndTemperature->Execute ();
                  if  (tempReadResult.IsOK ())
                  {
                    PvInt64  pvInt = -1;
                    lCameraTemparature->GetValue (pvInt);
                    cameraTemparature = (int32)pvInt;
                    log.Level (50) << "CameraAcquisitionPleoraDalsa::Run   lCameraTemparature :" << cameraTemparature << endl;
                  }
                  else
                  {
                    log.Level (-1) << "CameraAcquisitionPleoraDalsa::Run   ***ERROR***  executing 'ReadVoltageAndTemperature :" << PvResultToStr (tempReadResult) << endl;
                  }
                }
              }
              break;
            }

            ++whosTurn;
            if  (whosTurn > 1)
              whosTurn = 0;
          }

          if  (embeddedFlowMeter)
          {
            // Flow Counter as beginning of frame.
            uint32  tempFlowMeterCounter = rawData[0] + rawData[1] * 256 + (rawData[2] * 256 * 256) + (rawData[3] * 256 * 256 * 256);
            if  (tempFlowMeterCounter != flowMeterCounter)
            {
              // At some point in the future we might want to call "Call-Back" function from here to let other objects 
              // know that the flow meter changed value.
              flowMeterCounter = tempFlowMeterCounter;
              flowMeterCounterScanLine = scanLinesRead;
              FlowMeterCounterUpdate (scanLinesRead, flowMeterCounter);
            }

            if  (generateFlowRateReport)
            {
              // This code was added to help get diagnostic info on the 1ast 4 columns of imagery which
              // are being used to comunicate FlowRate meter pulse count.
              const PvUInt8* sld = rawData;
              for (int rrr = 0;  rrr < FrameHeight ();  ++rrr)
              {
                int32 flowRateCount = sld[0] + sld[1] * 256 + (sld[2] * 256 * 256) + (sld[3] * 256 * 256 * 256);
                if  (flowRateCount != lastFlowRateCount)
                {
                  *flowRate << physicalFramesRead     << "\t" 
                            << flowRateScanLineCount  << "\t"
                            << (int)(sld[0])          << "\t" 
                            << (int)(sld[1])          << "\t" 
                            << (int)(sld[2])          << "\t" 
                            << (int)(sld[3])          << "\t" 
                            << flowRateCount
                            << endl;
                  lastFlowRateCount = flowRateCount;
                }

                sld += FrameWidth ();
                ++flowRateScanLineCount;
              }
            }
          }

          ++physicalFramesRead;
          scanLinesRead += FrameHeight ();

          float flowRate = 0.0f;
          float flowRateRatio = -1.0f;
          flowMeter->GetFlowRateTrend (flowRate, flowRateRatio);
          frameBuffer->AddFrame ((uchar*)rawData, imageSize, FrameHeight (), FrameWidth (), flowMeterCounter, flowMeterCounterScanLine, flowRateRatio, flowRate);

          PvResult releaseBufferResult = lPipeline->ReleaseBuffer (lBuffer);
          if  (!releaseBufferResult.IsOK ())
            log.Level (-1) << "CameraAcquisitionPleoraDalsa::Run   ***ERROR***  ReleaseBuffer failed :" << PvResultToStr (releaseBufferResult) << endl;
        }
      }
    }
  }

  DisconnectFromCamera ();

  delete  flowRate;
  flowRate = NULL;

  Manager ()->AddSecondaryMsg ("Cameras Acquision Stopped.");

  StartStatus (ssDisconnected, "Camera Stopped.");
  Status (KKThread::tsStopping);

  log.Level (10) << "CameraAcquisitionPleoraDalsa::Run   Exiting   TerminateFlag: " << TerminateFlag () <<  "  ShutdownFlag: " << ShutdownFlag () << endl;

  return;
}  /* Run */









void  CameraAcquisitionPleoraDalsa::ResetCounts ()
{
  CameraAcquisition::ResetCounts ();
}



/******************************************************************************/
/*                             Auto gain Procedure                            *
/******************************************************************************/




void  CameraAcquisitionPleoraDalsa::RaiseAnalogGainUntilSaturated2 (float  increments)
{
  // AnalogGain saturation occurs when the high values no longer increase as you increase Analog Gain.
  // Because of noise we define Not Growing and not increasing by more than 2%.
  log.Level (40) << "CameraAcquisitionPleoraDalsa::RaiseAnalogGainUntilSaturated2  increments[" << increments << "]." << endl;

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




void    CameraAcquisitionPleoraDalsa::SetAnalogGainToFirstHighInRange (float  analogGainStart, 
                                                                  float  analogGainEnd, 
                                                                  float  increment
                                                                 )
{
  log.Level (40) << "SetAnalogGainToFirstHighInRange  AnalogGainStart: " << analogGainStart << "  AnalogGainEnd: " << analogGainEnd << "  Increment: " << increment << endl;

  VectorUint16 highValues;

  uint16  allTimeHigh = 0;

  float  nextAnalogGain = analogGainStart;
  while  (nextAnalogGain <= analogGainEnd)
  {
    agAnalogGain = nextAnalogGain;
    MakeAnalogGainRequest (agAnalogGain);
    WaitForOneFrame ();
    //osSleepMiliSecs (1000);
    uint16  hv = CameraHighValueOf3Frames ();
    allTimeHigh = Max (hv, allTimeHigh);
    highValues.push_back (hv);
    nextAnalogGain = nextAnalogGain + increment;
  }

  agAnalogGain = analogGainStart;
  for  (uint16 x = 0;  x < highValues.size ();  ++x)
  {
    if  (highValues[x] == allTimeHigh)
      break;
    agAnalogGain += increment;
  }

  MakeAnalogGainRequest (agAnalogGain);
}  /* SetAnalogGainToFirstHighInRange */




void  CameraAcquisitionPleoraDalsa::RaiseDigitalGainUntilSaturated (int32  increments)
{
  log.Level (40) << "CameraAcquisitionPleoraDalsa::RaiseDigitalGainUntilSaturated [" << increments << "]." << endl;
  int loopCount = 0;

  while  ((!TerminateFlag ())  &&  (!ShutdownFlag ()))
  {
    int32  nextDigitalGain = agDigitalGain + increments;
    if  (nextDigitalGain > digitalGainMax)
      break;

    MakeDigitalGainRequest (nextDigitalGain);
    agDigitalGain = nextDigitalGain;
    WaitForOneFrame ();
    if  (DigitalGainIsMaxed ())
    {
      agDigitalGain = Max ((int32)0, (agDigitalGain - increments));
      MakeDigitalGainRequest (nextDigitalGain);
      break;
    }
    ++loopCount;
    if  (loopCount > 10)
      break;
  }
}  /* RaiseDigitalGainUntilSaturated */




void  CameraAcquisitionPleoraDalsa::RaiseDigitalGainUntilAtLeastHighValue (int  minHighValue)
{
  log.Level (40) << "CameraAcquisitionPleoraDalsa::RaiseDigitalGainUntilAtLeastHighValue   MinHighValue[" << minHighValue << "]." << endl;
  WaitForOneFrame ();
  uint16  highValue = CameraHighValue ();
  if  (highValue >= minHighValue)
    return;

  int32  bestGuess = agDigitalGain * minHighValue / highValue;
  int32  increment = (bestGuess - agDigitalGain) / 3;

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
 *  This cocde was originally implemented for a Dalsa SG-11-02K80.
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
 *    At this point we have the Analog-Gain value we want to live with; we now only neeed to 
 *    search for the Digital-Gain.
 *  
 *  7) Search for Digital-Gain start with Coarse(increments of a 1000) to fine using incremenst 
 *     of 100, and then 20.
 *@endcode
 */
void  CameraAcquisitionPleoraDalsa::PerformAutoGainProcedureOld ()
{
  log.Level (10) << "CameraAcquisitionPleoraDalsa::PerformAutoGainProcedure" << endl;

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

  agDigitalGain = (int32)(0.6f * (float)agDigitalGain);
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





void  CameraAcquisitionPleoraDalsa::AnalogGainBinarySearch (uint16 threshholdSaturationValue)
{
  log.Level (20) << "CameraAcquisitionPleoraDalsa::AnalogGainBinarySearch    " << endl;

  float  low  = Max (-10000.0f, analogGainMin);
  float  high = Min (10000.0f,  analogGainMax);

  float  deltaLowHigh = high - low;

  while  (deltaLowHigh > 0.1f)
  {
    float   mid = (low + high) / 2.0f;
    agAnalogGain = mid;
    MakeAnalogGainRequest (agAnalogGain);
    WaitForOneFrame ();
    uint16  cameraHighValue = CameraHighValue ();
    if  (cameraHighValue < threshholdSaturationValue)
      low = mid;
    else
      high = mid;
    deltaLowHigh = high - low;
  }

  agAnalogGain = low;
  MakeAnalogGainRequest (agAnalogGain);
  WaitForOneFrame ();

  log.Level (20) << "CameraAcquisitionPleoraDalsa::AnalogGainBinarySearch   Done   AnalogGain[" << agAnalogGain << "]" << endl;
}   /* AnalogGainBinarySearch */





void  CameraAcquisitionPleoraDalsa::DigitalGainBinarySearch ()
{
  log.Level (20) << "CameraAcquisitionPleoraDalsa::DigitalGainBinarySearch" << endl;

  int32  low  = 1;
  int32  high = digitalGainMax;
  int32  deltaLowHigh = high - low;

  while  (deltaLowHigh > 10)
  {
    int32   mid = (low + high) / 2;
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

  log.Level (20) << "CameraAcquisitionPleoraDalsa::DigitalGainBinarySearch   Done   DigitalGain[" << agDigitalGain << "]" << endl;

}   /* DigitalGainBinarySearch */



/**
 *@details   Perform this with Analog-Gain maxed out so that area being imaged is obvious.
 */
void  CameraAcquisitionPleoraDalsa::PerformAutoCrop ()
{
  log.Level (20) << "CameraAcquisitionPleoraDalsa::PerformAutoCrop" << endl;
  VectorUcharPtr  scanLine = Manager ()->LastFrameHighValuesScanLine ();
  if  (!scanLine)
  {
    log.Level (-1) << "CameraAcquisitionPleoraDalsa::PerformAutoCrop    ***ERROR***    No  'LastFrameHighValuesScanLine'  returned." << endl;
    return;
  }

  uint16  highPoint = 0;

  // We start from the 4th column because the first 4 columns may be carrying the "Flow-Meter" counter value.
  uint16  x = 4;
  for  (x = 4;  x < scanLine->size ();  ++x)
  {
    uchar v = (*scanLine)[x];
    if  (v > highPoint)
      highPoint = v;
  }

  uint16  threshhold = highPoint * 1 / 2;
  
  int32  midPoint = scanLine->size () / 2;

  int32  cropLeft = midPoint;
  while  ((cropLeft > 6)  && 
          (((*scanLine)[cropLeft] > threshhold)  ||  ((*scanLine)[cropLeft - 1] > threshhold))
         )
    --cropLeft;
  cropLeft += 2;

  int32  rightSide = scanLine->size ();
  int32  cropRight = midPoint;
  while  ((cropRight <  rightSide)  && 
          (((*scanLine)[cropRight] > threshhold)  ||  ((*scanLine)[cropRight - 1] > threshhold))
         )
    ++cropRight;
  cropRight -= 1;

  Manager ()->CropSettingsChanged (cropLeft, cropRight);

  delete  scanLine;
  scanLine = NULL;

  log.Level (50) << "CameraAcquisitionPleoraDalsa::PerformAutoCrop    Exiting." << endl;

  return;
} /* PerformAutoCrop */



void  CameraAcquisitionPleoraDalsa::PerformAutoGainProcedure ()
{
  log.Level (10) << "CameraAcquisitionPleoraDalsa::PerformAutoGainProcedure" << endl;

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
  uint16  threshholdSaturationValue = CameraHighValue ();
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






