#if  !defined(_CAMERAPARAMETERSMANAGD_)
#define  _CAMERAPARAMETERSMANAGD_

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Collections::Generic;
using namespace System::Diagnostics;
using namespace System::Drawing;


#include "CameraParameters.h"
using namespace  LarcosCounterUnManaged;

#include  "CameraParametersManaged.h"

namespace LarcosCounterManaged
{
  public ref class CameraParametersManaged
  {
  public:
    CameraParametersManaged (CameraParametersPtr  parameters);

    CameraParametersManaged (CameraParameters&  parameters);

    !CameraParametersManaged ();

    ~CameraParametersManaged ();

    property   float     AnalogGain       {float    get ()  {return analogGain;};}
    property   String^   DeviceModelName  {String^  get ()  {return deviceModelName;};}
    property   String^   DeviceVendorName {String^  get ()  {return deviceVendorName;};}
    property   kkint32   DigitalGain      {kkint32  get ()  {return digitalGain;};}
    property   kkint32   FrameHeight      {kkint32  get ()  {return frameHeight;};}
    property   kkint32   FrameWidth       {kkint32  get ()  {return frameWidth;};}
    property   String^   IpAddress        {String^  get ()  {return ipAddress;};}
    property   String^   MacAddress       {String^  get ()  {return macAddress;};}
    property   String^   Model            {String^  get ()  {return model;};}
    property   float     ScanRate         {float    get ()  {return scanRate;};}
    property   String^   SensitivityMode  {String^  get ()  {return sensitivityMode;};}

  private:
    void  CleanUpMemory ();

    KKStr  SystemStringToKKStr (String^  s);

    String^  KKStrToSystenStr (const KKStr&  s);

    float     analogGain;
    String^   deviceModelName;
    String^   deviceVendorName;
    String^   deviceVersion;
    kkint32   digitalGain;
    kkint32   frameHeight;
    kkint32   frameWidth;
    String^   ipAddress;
    String^   macAddress;
    String^   model;
    float     scanRate;
    String^   serialNumber;
    String^   sensitivityMode;
  };  /* LarcosCounterManagerWrapper */



  public  ref class  CameraParametersManagedList: public List<CameraParametersManaged^>
  {
  public:
    CameraParametersManagedList (const CameraParametersList&  cameras);

    ~CameraParametersManagedList ();

    !CameraParametersManagedList ();
  };
}  /* LarcosCounterManaged */


#endif