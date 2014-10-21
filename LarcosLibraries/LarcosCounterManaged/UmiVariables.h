#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Collections::Generic;
using namespace System::Diagnostics;

//using namespace UmiCSharpClasses;


#include  "..\\KKBase\\OSservices.h"
#include  "UmiRunLog.h"


namespace LarcosCounterManaged 
{
  public ref class UmiVariables
  {
  public:
    UmiVariables ();

    static  void  SetLarcosHomeDir ();
    static  void  SetLarcosHomeDir (String^  _larcosHomeDir);

    static  String^  ConfigurationDir       ();  /**< Where application configuration files go;  NOT training models.                  */
    static  String^  CmdLineProgDir         ();  /**< Directory where you can find Command Line programs such as "CrossValidation.exe" */
    static  String^  LoggingDir             ();  /**< Directory where logging files can be written to.                                 */
    static  String^  DataFilesRootDir       ();
    static  String^  LarcosHomeDir          ();
    static  String^  TempDir                ();
    static  String^  TrainingModelsDir      ();
    static  String^  ReportDir              ();
    static  String^  ScannerFilesDefaultDir ();
  };
}