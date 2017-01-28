/*
 * Copyright (C) 2011-2013  Kurt Kramer
 * For conditions of distribution and use, see copyright notice in LarcosCounterUnManaged.txt
 */
#if   !defined(_CAMERAPARAMETERS_)
#define  _CAMERAPARAMETERS_

#include "KKStr.h"

namespace LarcosCounterUnManaged
{
  class CameraParameters
  {
  public:
    CameraParameters ();
    ~CameraParameters ();

    kkMemSize  MemoryConsumedEstimated ();

    float         AnalogGain       () const {return analogGain;}
    const KKStr&  DeviceVendorName () const {return deviceVendorName;}
    const KKStr&  DeviceModelName  () const {return deviceModelName;}
    const KKStr&  DeviceVersion    () const {return deviceVersion;}
    kkint32       DigitalGain      () const {return digitalGain;}
    kkint32       FrameHeight      () const {return frameHeight;}
    kkint32       FrameWidth       () const {return frameWidth;}
    kkint32       LinePeriod       () const {return linePeriod;}
    const KKStr&  Model            () const {return model;}
    float         ScanRate         () const {return scanRate;}
    const KKStr&  SerialNumber     () const {return serialNumber;}
    const KKStr&  SensitivityMode  () const {return sensitivityMode;}

    const KKStr&  MacAddress              () const {return macAddress;}
    const KKStr&  IpAddress               () const {return ipAddress;}
    const KKStr&  InterfaceSubnetMask     () const {return interfaceSubnetMask;}
    const KKStr&  InterfaceDefaultGateway () const {return interfaceDefaultGateway;}


    void  AnalogGain       (float         _analogGain)       {analogGain       = _analogGain;}
    void  DeviceVendorName (const KKStr&  _deviceVendorName) {deviceVendorName = _deviceVendorName;}
    void  DeviceModelName  (const KKStr&  _deviceModelName)  {deviceModelName  = _deviceModelName;}
    void  DeviceVersion    (const KKStr&  _deviceVersion)    {deviceVersion    = _deviceVersion;}
    void  DigitalGain      (kkint32        _digitalGain)     {digitalGain      = _digitalGain;}
    void  FrameHeight      (kkint32       _frameHeight)      {frameHeight      = _frameHeight;}
    void  FrameWidth       (kkint32       _frameWidth)       {frameWidth       = _frameWidth;}
    void  LinePeriod       (kkint32       _linePeriod)       {linePeriod       = _linePeriod;}
    void  Model            (const KKStr&  _model)            {model            = _model;}
    void  ScanRate         (float         _scanRate)         {scanRate         = _scanRate;}
    void  SerialNumber     (const KKStr&  _serialNumber)     {serialNumber     = _serialNumber;}
    void  SensitivityMode  (const KKStr&  _sensitivityMode)  {sensitivityMode  = _sensitivityMode;}

    void  MacAddress              (const KKStr&  _macAddress)              {macAddress              = _macAddress;}
    void  IpAddress               (const KKStr&  _ipAddress)               {ipAddress               = _ipAddress;}
    void  InterfaceSubnetMask     (const KKStr&  _interfaceSubnetMask)     {interfaceSubnetMask     = _interfaceSubnetMask;}
    void  InterfaceDefaultGateway (const KKStr&  _interfaceDefaultGateway) {interfaceDefaultGateway = _interfaceDefaultGateway;}


  private:
    float   analogGain;         /**< Will reflect the value reported by the camera, not what we would like it to be. */
    KKStr   deviceVendorName;
    KKStr   deviceModelName;
    KKStr   deviceVersion;
    kkint32 digitalGain;        /**< Will reflect the value reported by the camera, not what we would like it to be. */
    kkint32 frameHeight;
    kkint32 frameWidth;
    kkint32 linePeriod;         /**< 'acquisitionLinePeriod' from e2v;  Number microseconds per scan line.  */
    KKStr   model;
    float   scanRate;
    KKStr   serialNumber;    
    KKStr   sensitivityMode;

    KKStr   macAddress;
    KKStr   ipAddress;
    KKStr   interfaceSubnetMask;
    KKStr   interfaceDefaultGateway;
  };  /* CameraParameters */

  typedef  CameraParameters*  CameraParametersPtr;


  class  CameraParametersList:  public  KKQueue<CameraParameters>
  {
  public:
    CameraParametersList (bool  _owner): KKQueue<CameraParameters> (_owner) {}

    ~CameraParametersList ()  {}
  };

  typedef  CameraParametersList*  CameraParametersListPtr;

}   /* LarcosCounterUnManaged */
#endif
