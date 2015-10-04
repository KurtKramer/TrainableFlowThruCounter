#include "StdAfx.h"
#include "FirstIncludes.h"

#include <stdio.h>
#include <ctype.h>
#include <sstream>
#include <vector>
#include "MemoryDebug.h"


#include "KKBaseTypes.h"
using namespace KKB;


#include "ScannerFile.h"
using namespace  KKLSC;


#include "InstallationConfig.h"
using namespace  LarcosCounterUnManaged;



#include "UmiKKStr.h"
#include "UmiInstallationConfig.h"
#include "UmiRunLog.h"
#include "UmiScannerFile.h"
using namespace LarcosCounterManaged;

#include <msclr\marshal_cppstd.h>


using namespace System;



String^  UmiInstallationConfig::FlowMeterMethodToStr (FlowMeterMethods  method)
{
  return  UmiKKStr::KKStrToSystenStr (InstallationConfig::FlowMeterMethodToStr ((InstallationConfig::FlowMeterMethods)method));
}


UmiInstallationConfig::FlowMeterMethods   UmiInstallationConfig::FlowMeterMethodFromStr (String^  s)
{
  return  (UmiInstallationConfig::FlowMeterMethods)InstallationConfig::FlowMeterMethodFromStr (UmiKKStr::SystemStringToKKStrPtr (s));
}



String^    UmiInstallationConfig::InstallationDirPath ()
{
  return  UmiKKStr::KKStrToSystenStr (InstallationConfig::InstallationDirPath ());
}





UmiInstallationConfig::UmiInstallationConfig  (UmiRunLog^  runLog):
  installationConfig (NULL)
{
  if  (runLog == nullptr)
    runLog = gcnew UmiRunLog ();
  installationConfig = new InstallationConfig (runLog->Log ());
}



UmiInstallationConfig::UmiInstallationConfig  (UmiInstallationConfig^  _parms,
                                               UmiRunLog^              runLog
                                              ):
  installationConfig (NULL)
{
  if  (runLog == nullptr)
    runLog = gcnew UmiRunLog ();
  installationConfig = new InstallationConfig (*(_parms->UnManagedClass ()),  runLog->Log ());
}




UmiInstallationConfig::UmiInstallationConfig  (InstallationConfigPtr  _parms):
  installationConfig (_parms)
{
}



UmiInstallationConfig::UmiInstallationConfig  (const InstallationConfig&  _parms,
                                               UmiRunLog^                 runLog
                                              ):
  installationConfig (NULL)
{
  if  (runLog == nullptr)
    runLog = gcnew UmiRunLog ();

  installationConfig = new InstallationConfig (_parms, runLog->Log ());
}


  
UmiInstallationConfig::~UmiInstallationConfig ()
{
  this->!UmiInstallationConfig ();
}



UmiInstallationConfig::!UmiInstallationConfig ()
{
  delete  installationConfig;
  installationConfig = NULL;
}


void  UmiInstallationConfig::Assign (UmiInstallationConfig^  parms)
{
  installationConfig->Assign (*(parms->UnManagedClass ()));
}



String^   UmiInstallationConfig::CameraMacAddress::get ()
{
  return  UmiKKStr::KKStrToSystenStr (installationConfig->CameraMacAddress ());
}


String^   UmiInstallationConfig::Description::get ()
{
  return  UmiKKStr::KKStrToSystenStr (installationConfig->Description ());
}


UmiInstallationConfig::FlowMeterMethods  UmiInstallationConfig::FlowMeterMethod::get ()
{
  return  (UmiInstallationConfig::FlowMeterMethods)installationConfig->FlowMeterMethod ();
}



String^   UmiInstallationConfig::FlowMeterMethodStr::get ()
{
  return  UmiKKStr::KKStrToSystenStr (installationConfig->FlowMeterMethodStr ());
}


String^   UmiInstallationConfig::Name::get ()
{
  return  UmiKKStr::KKStrToSystenStr (installationConfig->Name ());
}




void  UmiInstallationConfig::UpdateFromDataField (String^  fieldName,
                                                  String^  fieldValue,
                                                  bool%    fieldFound
                                                 )
{
  KKStr  fn = UmiKKStr::SystemStringToKKStr (fieldName);
  KKStr  fv = UmiKKStr::SystemStringToKKStr (fieldValue);
  bool  ff = false;
  installationConfig->UpdateFromDataField (fn, fv, ff);
  fieldFound = ff;
}



void  UmiInstallationConfig::UpdateFromScannerFile (UmiScannerFile^ sf)
{
  if  (sf == nullptr)
    return;

  installationConfig->UpdateFromScannerFile (sf->UnManagedClass ());
}




void  UmiInstallationConfig::WriteFieldValues (System::IO::StreamWriter^  sr)
{
  std::ostringstream o;
  installationConfig->WriteFieldValues (o);
  String^ s = msclr::interop::marshal_as<String^> (o.str ());
  sr->WriteLine (s);
}




UmiInstallationConfigList::UmiInstallationConfigList ()
{
}



