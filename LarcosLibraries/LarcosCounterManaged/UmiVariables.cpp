#include "StdAfx.h"
#include "FirstIncludes.h"

#include <stdio.h>
#include <math.h>
#include <ctype.h>
#include <time.h>
#include <fstream>
#include <iostream>
#include <ostream>
#include <string>
#include <vector>
#include "MemoryDebug.h"


#include "KKBaseTypes.h"
using namespace KKB;


#include "LarcosVariables.h"
using  namespace  LarcosBase;


#include "UmiOSservices.h"
#include "UmiKKStr.h"
#include "UmiVariables.h"
using namespace  LarcosCounterManaged;


using namespace System;
using namespace System::IO;
using namespace System::Windows::Forms;
using namespace KKB;
using namespace System::Windows::Forms;




UmiVariables::UmiVariables (void)
{
}


void  UmiVariables::SetLarcosHomeDir ()
{
  LarcosVariables::SetLarcosHomeDir ();
}



void  UmiVariables::SetLarcosHomeDir (String^  _larcosHomeDir)
{
  LarcosVariables::SetLarcosHomeDir (UmiKKStr::SystemStringToKKStr (_larcosHomeDir));
}




String^  UmiVariables::LarcosHomeDir ()
{
  String^  result = UmiKKStr::KKStrToSystenStr (LarcosVariables::LarcosHomeDir ());
  return  result;
}


String^  UmiVariables::ConfigurationDir ()
{
  return  UmiKKStr::KKStrToSystenStr (LarcosVariables::ConfigurationDir ());
}



String^  UmiVariables::CmdLineProgDir ()
{
  return  UmiKKStr::KKStrToSystenStr (LarcosVariables::CmdLineProgDir ());
}



String^  UmiVariables::DataFilesRootDir ()
{
  return  UmiKKStr::KKStrToSystenStr (LarcosVariables::DataFilesRootDir ());
}



String^  UmiVariables::ReportDir ()
{
  return  UmiKKStr::KKStrToSystenStr (LarcosVariables::ReportDir ());
}



String^  UmiVariables::ScannerFilesDefaultDir ()
{
  return  UmiKKStr::KKStrToSystenStr (LarcosVariables::ScannerFilesDefaultDir ());
}



String^  UmiVariables::TempDir ()
{
  return  UmiKKStr::KKStrToSystenStr (LarcosVariables::TempDir ());
}



/* Directory where logging files can be written to. */
String^  UmiVariables::LoggingDir ()
{
  return  UmiKKStr::KKStrToSystenStr (LarcosVariables::LoggingDir ());
}



String^  UmiVariables::TrainingModelsDir ()
{
  return  UmiKKStr::KKStrToSystenStr (LarcosVariables::TrainingModelsDir ());
}
