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


#include "CounterVariables.h"
using  namespace  CounterBase;


#include "UmiOSservices.h"
#include "UmiKKStr.h"
#include "UmiVariables.h"
using namespace  CounterManaged;

using namespace System;
using namespace System::IO;
using namespace System::Windows::Forms;
using namespace KKB;
using namespace System::Windows::Forms;



UmiVariables::UmiVariables (void)
{
}



void  UmiVariables::SetCounterHomeDir ()
{
  CounterVariables::SetCounterHomeDir ();
}



void  UmiVariables::SetCounterHomeDir (String^  _counterHomeDir)
{
  CounterVariables::SetCounterHomeDir (UmiKKStr::SystemStringToKKStr (_counterHomeDir));
}



String^  UmiVariables::CounterHomeDir ()
{
  String^  result = UmiKKStr::KKStrToSystenStr (CounterVariables::CounterHomeDir ());
  return  result;
}



String^  UmiVariables::ConfigurationDir ()
{
  return  UmiKKStr::KKStrToSystenStr (CounterVariables::ConfigurationDir ());
}



String^  UmiVariables::CmdLineProgDir ()
{
  return  UmiKKStr::KKStrToSystenStr (CounterVariables::CmdLineProgDir ());
}



String^  UmiVariables::DataFilesRootDir ()
{
  return  UmiKKStr::KKStrToSystenStr (CounterVariables::DataFilesRootDir ());
}



String^  UmiVariables::ReportDir ()
{
  return  UmiKKStr::KKStrToSystenStr (CounterVariables::ReportDir ());
}



String^  UmiVariables::ScannerFilesDefaultDir ()
{
  return  UmiKKStr::KKStrToSystenStr (CounterVariables::ScannerFilesDefaultDir ());
}



String^  UmiVariables::TempDir ()
{
  return  UmiKKStr::KKStrToSystenStr (CounterVariables::TempDir ());
}



/* Directory where logging files can be written to. */
String^  UmiVariables::LoggingDir ()
{
  return  UmiKKStr::KKStrToSystenStr (CounterVariables::LoggingDir ());
}



String^  UmiVariables::TrainingModelsDir ()
{
  return  UmiKKStr::KKStrToSystenStr (CounterVariables::TrainingModelsDir ());
}
