#include "StdAfx.h"
#include  "FirstIncludes.h"

#include <stdio.h>
#include <math.h>

#include <ctype.h>
#include <time.h>
#include <string>
#include "MemoryDebug.h"
using namespace std;

#include "KKStr.h"
using namespace  KKB;


#include  "LarcosVariables.h"
using namespace  LarcosBase;



#include "CameraParametersManaged.h"
using  namespace  LarcosCounterManaged;


using namespace System;
using namespace System::Threading;
//using namespace System::Windows::Forms;

/*
// %SystemRoot%\system32;
   %SystemRoot%;
   %SystemRoot%\System32\Wbem;
   %SYSTEMROOT%\System32\WindowsPowerShell\v1.0\;
   C:\Program Files\WIDCOMM\Bluetooth Software\;
   C:\Program Files\WIDCOMM\Bluetooth Software\syswow64;
   C:\Program Files (x86)\Microsoft SQL Server\90\Tools\binn\;
   C:\Program Files (x86)\Common Files\Pleora\eBUS SDK;
   C:\Program Files\Common Files\Pleora\eBUS SDK;
   C:\Program Files\TortoiseSVN\bin;
   C:\Windows\Microsoft.NET\Framework\v2.0.50727
*/


CameraParametersManaged::CameraParametersManaged (CameraParametersPtr  parameters):
    analogGain       (0),
    deviceVendorName (nullptr),
    deviceModelName  (nullptr),
    deviceVersion    (nullptr),
    digitalGain      (0),
    frameHeight      (0),
    frameWidth       (0),
    macAddress       (nullptr),
    model            (nullptr),
    scanRate         (0.0f),
    serialNumber     (nullptr),
    sensitivityMode  (nullptr)
{
  if  (parameters)
  {
    deviceVendorName  = KKStrToSystenStr (parameters->DeviceVendorName ());
    deviceModelName   = KKStrToSystenStr (parameters->DeviceModelName  ());
    deviceVersion     = KKStrToSystenStr (parameters->DeviceVersion    ());
    macAddress        = KKStrToSystenStr (parameters->MacAddress       ());
    model             = KKStrToSystenStr (parameters->Model            ());
    serialNumber      = KKStrToSystenStr (parameters->SerialNumber     ());
    sensitivityMode   = KKStrToSystenStr (parameters->SensitivityMode  ());

    analogGain        = parameters->AnalogGain  ();
    digitalGain       = parameters->DigitalGain ();
    frameHeight       = parameters->FrameHeight ();
    frameWidth        = parameters->FrameWidth  ();
    scanRate          = parameters->ScanRate    ();
  }
}



CameraParametersManaged::CameraParametersManaged (CameraParameters&  parameters):
    analogGain       (0),
    deviceVendorName (nullptr),
    deviceModelName  (nullptr),
    deviceVersion    (nullptr),
    digitalGain      (0),
    frameHeight      (0),
    frameWidth       (0),
    macAddress       (nullptr),
    model            (nullptr),
    scanRate         (0.0f),
    serialNumber     (nullptr),
    sensitivityMode  (nullptr)
{
    deviceVendorName  = KKStrToSystenStr (parameters.DeviceVendorName ());
    deviceModelName   = KKStrToSystenStr (parameters.DeviceModelName  ());
    deviceVersion     = KKStrToSystenStr (parameters.DeviceVersion    ());
    macAddress        = KKStrToSystenStr (parameters.MacAddress       ());
    model             = KKStrToSystenStr (parameters.Model            ());
    serialNumber      = KKStrToSystenStr (parameters.SerialNumber     ());
    sensitivityMode   = KKStrToSystenStr (parameters.SensitivityMode  ());

    analogGain        = parameters.AnalogGain  ();
    digitalGain       = parameters.DigitalGain ();
    frameHeight       = parameters.FrameHeight ();
    frameWidth        = parameters.FrameWidth  ();
    scanRate          = parameters.ScanRate    ();
}


CameraParametersManaged::!CameraParametersManaged ()
{
  CleanUpMemory ();
}


CameraParametersManaged::~CameraParametersManaged ()
{
  this->!CameraParametersManaged ();
}


void  CameraParametersManaged::CleanUpMemory ()
{
}


KKStr  CameraParametersManaged::SystemStringToKKStr (String^  s)
{
  if  (s == nullptr)
    return KKStr::EmptyStr ();

  KKB::KKStr  kkStr (s->Length);
  for  (int x = 0;  x < s->Length;  x++)
    kkStr.Append ((char)s[x]);
  return  kkStr;
}



String^  CameraParametersManaged::KKStrToSystenStr (const KKStr&  s)
{
  System::String^  sysStr = gcnew System::String (s.Str ());
  return  sysStr;
}




CameraParametersManagedList::CameraParametersManagedList (const CameraParametersList&  cameras)
{
  CameraParametersList::const_iterator  idx;
  for  (idx = cameras.begin ();  idx != cameras.end ();  ++idx)
  {
    Add (gcnew CameraParametersManaged (*idx));
  }
}



CameraParametersManagedList::~CameraParametersManagedList ()
{
  this->!CameraParametersManagedList ();
}


CameraParametersManagedList::!CameraParametersManagedList ()
{
}
