#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Collections::Generic;
using namespace System::Diagnostics;

#include "InstallationConfig.h"
using namespace  LarcosCounterUnManaged;

#include "UmiScannerFile.h"


namespace LarcosCounterManaged
{
  public  ref  class  UmiInstallationConfig
  {
  public:
    enum  class  FlowMeterMethods: int  
    {
      Null      = InstallationConfig::FlowMeterMethods::Null,
      Embedded  = InstallationConfig::FlowMeterMethods::Embedded,
      EndOfList = InstallationConfig::FlowMeterMethods::EndOfList,
    };

    static String^            FlowMeterMethodToStr   (FlowMeterMethods  method);
    static FlowMeterMethods   FlowMeterMethodFromStr (String^  s);
    static String^            InstallationDirPath    ();


    UmiInstallationConfig  (UmiRunLog^  runLog);

    /// <summary>
    /// Will duplicate contents of _parms.
    /// <param name="_parms">Parameters being given to this managed instance.</param>
    /// <param name="runLog">Run Log to report to;  if st to nullptr will create temporary instance to use.</param>
    /// </summary>
    UmiInstallationConfig  (UmiInstallationConfig^  _parms,
                            UmiRunLog^              runLog
                           );

    /// <summary>
    /// Will take ownership '_parns';  caller BETTER NOT DELETE this instance.
    /// </summary>
    /// <param name="_parms">Installation configuration Being Given Ownership to this managed instance.</param>
    UmiInstallationConfig  (InstallationConfigPtr  _parms);

    /// <summary>
    /// Will allocate create a duplicate of '_parms' and own it.
    /// </summary>
    /// <param name="_parms">Parameters that are to create duplicated.</param>
    /// <param name="runLog">Run Log to report to;  if st to nullptr will create temporary instance to use.</param>
    UmiInstallationConfig  (const InstallationConfig&  _parms,
                            UmiRunLog^                 runLog
                           );

  private:
    ~UmiInstallationConfig ();

  public:
    !UmiInstallationConfig ();

    void  Assign (UmiInstallationConfig^  parms);

    InstallationConfigPtr  UnManagedClass ()  {return installationConfig;}


    property  String^            CameraMacAddress          {String^            get ();}
    property  String^            Description               {String^            get ();}
    property  FlowMeterMethods   FlowMeterMethod           {FlowMeterMethods   get ();}
    property  String^            FlowMeterMethodStr        {String^            get ();}
    property  bool               FlowMeterPresent          {bool               get () {return installationConfig->FlowMeterPresent          ();}}
    property  float              FlowMeterTicsPerMeter     {float              get () {return installationConfig->FlowMeterTicsPerMeter     ();}}
    property  float              ImagingChamberWidth       {float              get () {return installationConfig->ImagingChamberWidth       ();}}
    property  int                ImagingChamberWidthPixels {int                get () {return installationConfig->ImagingChamberWidthPixels ();}}
    property  String^            Name                      {String^            get ();}


    void  UpdateFromDataField (String^  fieldName,
                               String^  fieldValue,
                               bool%    fieldFound
                              );

    void  UpdateFromScannerFile (UmiScannerFile^ sf);

    void  WriteFieldValues (System::IO::StreamWriter^  sr);

  private:
    InstallationConfigPtr  installationConfig;
  };  /* UmiInstallationConfig */



  public  ref  class  UmiInstallationConfigList: public List<UmiInstallationConfig^>
  {
  public:
    UmiInstallationConfigList ();
  };  /* UmiInstallationConfigList */

}
