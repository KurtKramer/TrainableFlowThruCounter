#include "FirstIncludes.h"
#include <stdlib.h>
#include <memory>
#include "MemoryDebug.h"
using namespace  std;


#include "KKBaseTypes.h"
#include "OSservices.h"
using namespace  KKB;

#include "Variables.h"
using namespace  KKLSC;


#include "KKMLVariables.h"
using namespace  KKMLL;

#include "LarcosVariables.h"
using namespace  LarcosBase;



LarcosVariables::LarcosVariables ()
{
}



LarcosVariables::~LarcosVariables ()
{
}



KKStr  LarcosVariables::larcosHomeDir = "";


kkint32  LarcosVariables::debugLevel = -1;



KKStr  LarcosVariables::LarcosHomeDir ()
{
  if  (!larcosHomeDir.Empty ())
    return  larcosHomeDir;

  // We will first try and get directory from the LarcosHomeDir environment variable.
  KKStrPtr homeDir = osGetEnvVariable ("ZoisHomeDir");
  if  (homeDir != NULL)
  {
    larcosHomeDir = *homeDir;
    delete  homeDir;
    homeDir = NULL;
  }

  if  (larcosHomeDir.Empty ())
  {
    #if  defined(KKOS_WINDOWS)
       larcosHomeDir = "C:\\Larcos";
       cout << endl << "larcosHomeDir" << "\t" << larcosHomeDir << endl << endl;
    #else
       larcosHomeDir = "/Larcos";
    #endif
  }

  return  larcosHomeDir; 
}  /* LarcosHomeDir */



KKStr  LarcosVariables::ScannerFilesDefaultDir ()
{
  return osAddSlash (LarcosHomeDir ()) + "ScannerFiles";
}  /* ScannerFilesDefaultDir */



KKStr  LarcosVariables::TempDir ()
{
  return  osAddSlash (LarcosHomeDir ()) + "Temp";
}


void   LarcosVariables::SetLarcosHomeDir ()
{
  // When this method is called the caller wants to set the Larcos home directory to the default location.
  // so we set 'larcosHomeDir' to a empty string and then call the method 'LarcosHomeDir' which will initialize
  // 'larcosHomeDir' to the default directory.
  larcosHomeDir = "";
  LarcosHomeDir ();
  KKMLVariables::SetMachineLearninigHomeDir (osAddSlash (LarcosHomeDir ()) + "Classifier");
  KKLSC::Variables::SetHomeDir (LarcosHomeDir ());
}



void   LarcosVariables::SetLarcosHomeDir (const KKStr&  _larcosHomeDir)
{
  larcosHomeDir = _larcosHomeDir;
  KKMLVariables::SetMachineLearninigHomeDir (osAddSlash (LarcosHomeDir ()) + "Classifier");
  KKLSC::Variables::SetHomeDir (LarcosHomeDir ());
}



/* Directory where logging files can be written to. */
KKStr  LarcosVariables::LoggingDir ()
{
  return osAddSlash (LarcosHomeDir ()) + "LoggingFiles";
}


KKStr  LarcosVariables::CmdLineProgDir ()
{
  KKStr  s = osAddSlash (LarcosHomeDir ()) + "exe";
  return s;
}



KKStr  LarcosVariables::ConfigurationDir ()
{
  KKStr  configDir = osAddSlash (osAddSlash (LarcosHomeDir ()) + "Configurations");
  return  configDir;
}



kkint32  LarcosVariables::DebugLevel ()
{
  if  (debugLevel < 0)
  {
    KKStrPtr s = osGetEnvVariable ("LarcosDebugLevel");
    if  (s)
    {
      debugLevel = s->ToInt32 ();
      delete  s;
      s = NULL;
    }
    else
    {
      debugLevel  = 10;
    }
  }
  return  debugLevel;
}



KKStr  LarcosVariables::DataFilesRootDir ()
{
  KKStr  dataFilesRootDir = osAddSlash (osAddSlash (LarcosHomeDir ()) + "Configurations");
  return  dataFilesRootDir;
}



KKStr  LarcosVariables::ReportDir ()
{
  KKStr  reportDir = osAddSlash (osAddSlash (LarcosHomeDir ()) + "Reports");
  return  reportDir;
}




KKStr  LarcosVariables::TrainingLibrariesDir ()
{
  return  KKMLVariables::TrainingLibrariesDir ();
}



KKStr  LarcosVariables::TrainingModelsDir ()
{
  return  KKMLVariables::TrainingModelsDir ();
}
