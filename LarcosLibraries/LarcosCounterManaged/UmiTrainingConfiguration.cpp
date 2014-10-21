#include "StdAfx.h"
#include  "FirstIncludes.h"

#include <stdio.h>
#include <math.h>


#include <ctype.h>
#include <time.h>

#include <fstream>
#include <iostream>
#include <ostream>
#include <string>
#include <strstream>
#include <vector>
#include "MemoryDebug.h"
using namespace std;

#include "KKBaseTypes.h"
#include "..\\KKBase\\GoalKeeper.h"
#include "KKStr.h"
#include "OSservices.h"
using namespace KKB;

#include "FileDesc.h"
#include "FeatureFileIOKK.h"
#include "PostLarvaeFV.h"
#include "TrainingConfiguration2.h"
using namespace  KKMachineLearning;

#include "LarcosVariables.h"
using namespace  LarcosBase;

#include "OperatingParameters.h"

#include "UmiTrainingConfiguration.h"

#include "UmiKKStr.h"
#include "UmiRunLog.h"
#include "UmiVariables.h"
using namespace LarcosCounterManaged;


using namespace System;
using namespace System::Threading;


UmiTrainingConfiguration::UmiTrainingConfiguration (UmiRunLog^  _log):
  cancelFlag    (new bool (false)),
  classes       (NULL),
  config        (NULL),
  loadLogStream (nullptr),
  log           (_log),
  valid         (gcnew System::Boolean (false))

{
}



UmiTrainingConfiguration::UmiTrainingConfiguration (String^                  _configFileName,
                                                    UmiOperatingParameters^  _initialOperatingParameters,
                                                    UmiRunLog^               _log
                                                   ):
  cancelFlag    (new bool (false)),
  classes       (NULL),
  config        (NULL),
  loadLogStream (nullptr),
  log           (_log),
  valid         (gcnew System::Boolean (false))
{
  OperatingParametersPtr  op = NULL;
  if  (_initialOperatingParameters != nullptr)
    op = _initialOperatingParameters->UnManagedClass ();

  strstream  logStr;
  KKStr  configFileName = UmiKKStr::SystemStringToKKStr (_configFileName);
  FileDescPtr  fileDesc = PostLarvaeFV::PostLarvaeFeaturesFileDesc ();
  config = new LarcosTrainingConfiguration (fileDesc, configFileName, op, log->Log (), true);
  valid = gcnew System::Boolean (config->FormatGood ());
  if  (!config->FormatGood ())
     loadLogStream = gcnew String (logStr.str ());
}





UmiTrainingConfiguration::UmiTrainingConfiguration (DirectoryInfo^            di,
                                                    UmiOperatingParameters^  _initialOperatingParameters,
                                                    UmiRunLog^              _log
                                                   ):
  cancelFlag    (new bool (false)),
  classes       (NULL),
  config        (NULL),
  loadLogStream (nullptr),
  log           (_log),
  valid         (gcnew Boolean (false))

{
  osRunAsABackGroundProcess ();

  OperatingParametersPtr  op = NULL;
  if  (_initialOperatingParameters != nullptr)
    op = _initialOperatingParameters->UnManagedClass ();

  KKStr  errorMessage;

  KKStr  dirName    = UmiKKStr::SystemStringToKKStr (di->FullName);
  bool   successful = false;

  KKStr  modelNameKKStr = osGetRootNameOfDirectory (dirName) + ".cfg";
  KKStr  configFileName = osAddSlash (LarcosVariables::TrainingModelsDir ()) + modelNameKKStr;
    
  FileDescPtr fd = PostLarvaeFV::PostLarvaeFeaturesFileDesc ();
  delete  classes;  classes = NULL;

  config = LarcosTrainingConfiguration::CreateFromDirectoryStructure 
                                        (fd,
                                         configFileName,
                                         dirName,
                                         op,
                                         log->Log (),
                                         successful,
                                         errorMessage
                                        );

  valid = gcnew System::Boolean (successful);

  classes = config->ExtractClassList ();

  // We are nvere ever suposed to delete a 'FileDesc' object.  they are managed wholly in the "FileDesc" class.  
  // Other parts of the app may and probably will be using this same instance.
  //delete  fd;  fd = NULL;
}  /* CreateTrainingModelFromDirectory */
  




UmiTrainingConfiguration::~UmiTrainingConfiguration ()
{
  this->!UmiTrainingConfiguration ();
}



UmiTrainingConfiguration::!UmiTrainingConfiguration ()
{
  delete  cancelFlag; cancelFlag = NULL;
  delete  config;     config     = NULL;
  delete  classes;    classes    = NULL;
}





void  UmiTrainingConfiguration::CancelLoad ()  // Call this methid to set the 'canclFlag' to true.
{
  *cancelFlag = true;
}


String^   UmiTrainingConfiguration::RootDirExpanded ()
{
  return  UmiKKStr::KKStrToSystenStr (config->RootDirExpanded ());
}


String^   UmiTrainingConfiguration::FullFileName ()
{
  KKStr  fullFileName = TrainingConfiguration2::GetEffectiveConfigFileName (config->ConfigFileNameSpecified ());
  return UmiKKStr::KKStrToSystenStr (fullFileName);
}



String^   UmiTrainingConfiguration::ModelName ()
{
  if  (!config)
    return  System::String::Empty;

  return  UmiKKStr::KKStrToSystenStr (osGetRootNameWithExtension (config->ConfigFileNameSpecified ()));
}  /* ModelName */





System::Boolean  UmiTrainingConfiguration::Valid ()
{
  return  *valid;
}  /* Valid */




kkuint32  UmiTrainingConfiguration::NumHierarchialLevels ()
{
  if  (config)
    return  config->NumHierarchialLevels ();
  else
    return 0;
}




String^  UmiTrainingConfiguration::GetEffectiveConfigFileName (String^  configFileName)
{
  KKStr  effectiveFileName = TrainingConfiguration2::GetEffectiveConfigFileName (UmiKKStr::SystemStringToKKStr (configFileName));
  return  UmiKKStr::KKStrToSystenStr (effectiveFileName);
}


void  UmiTrainingConfiguration::AddATrainingClass (UmiTrainingClass^  _trainClass)
{
  TrainingClassPtr  utc = _trainClass->UnManagedClass ();
  TrainingClassPtr  tc = new TrainingClass (*utc);
  config->AddATrainingClass (tc);
}






UmiFeatureVectorList^  UmiTrainingConfiguration::LoadFeatureDataFromTrainingLibraries (bool^ _changesMadeToTrainingLibraries)
{
  if  (!config)
    return  nullptr;

  *cancelFlag = false;

  bool  changesMadeToTrainingLibraries = false;

  KKB::DateTime  latestImageTimeStamp;

  FeatureVectorListPtr  data = config->LoadFeatureDataFromTrainingLibraries 
         (latestImageTimeStamp,
          changesMadeToTrainingLibraries,
          *cancelFlag
         );

  if  (data == NULL)
    return nullptr;

  if  (*cancelFlag)
  {
    delete  data;
    data=NULL;
   return nullptr;
  }

  PostLarvaeFVListPtr  imageData = new PostLarvaeFVList (*data, false);
  imageData->Owner (true);
  data->Owner (false);
  delete  data;
  data = NULL;

  UmiFeatureVectorList^ pfvl = gcnew UmiFeatureVectorList (*imageData);
  imageData->Owner (false);
  delete  imageData;  imageData = NULL;

  _changesMadeToTrainingLibraries = changesMadeToTrainingLibraries;

  //*_changesMadeToTrainingLibraries = changesMadeToTrainingLibraries;

  return  pfvl;
}  /* LoadFeatureDataFromTrainingLibraries */




UmiClassList^  UmiTrainingConfiguration::GetClassList ()
{
  MLClassListPtr  classes = config->ExtractClassList ();

  UmiClassList^  results = gcnew UmiClassList (classes);
  delete  classes;
  classes = NULL;
  return  results;
}




String^   UmiTrainingConfiguration::GetSettingValue (String^ sectionName,
                                                     String^ settingName
                                                    )
{
  kkint32  lineNum = 0;
  KKStrConstPtr  value = config->SettingValue (UmiKKStr::SystemStringToKKStr (sectionName), UmiKKStr::SystemStringToKKStr (settingName), lineNum);
  if  (value == NULL)
    return nullptr;
  else
    return UmiKKStr::KKStrToSystenStr (*value);
}



Dictionary<String^,String^>^   UmiTrainingConfiguration::GetSettingValues (String^  _sectionName)
{
  kkint32  sectionNum =  config->SectionNum (UmiKKStr::SystemStringToKKStr (_sectionName));
  if  (sectionNum < 0)
    return nullptr;

  Dictionary<String^,String^>^ settings = gcnew Dictionary<String^,String^> ();

  kkint32 settingNum = 0;
  do
  {
    kkint32  lineNum = 0;
    KKStrConstPtr   settingValue = config->SettingValue (sectionNum, settingNum, lineNum);
    if  (settingValue == NULL)
      break;

    KKStrConstPtr   settingName = config->SettingName (sectionNum, settingNum);
    if  (settingName == NULL)
      break;

    try  {settings->Add (UmiKKStr::KKStrToSystenStr (*settingName), UmiKKStr::KKStrToSystenStr (*settingValue));}   catch (...) {}

    ++settingNum;
  }  while  (true);

  return  settings;

}  /* GetSettingValues */



UmiOperatingParameters^   UmiTrainingConfiguration::GetOperatingParameters ()
{
  const OperatingParameters*  opParms = NULL;
  if  (config)
    opParms = config->OperatingParms ();


  OperatingParametersPtr  resultParms = NULL;
  if  (opParms)
    resultParms = new OperatingParameters (*opParms);
  else
    resultParms = new OperatingParameters ();

  return  gcnew UmiOperatingParameters (resultParms);
}



void  UmiTrainingConfiguration::SetOperatingParms (UmiOperatingParameters^  _operatingParms)
{
  config->SetOperatingParms (*(_operatingParms->UnManagedClass ()));
}



UmiTrainingClassList^   UmiTrainingConfiguration::TrainingClasses ()
{
  UmiTrainingClassList^  results = gcnew UmiTrainingClassList ();

  const TrainingClassList&  trainingClasses = config->TrainingClasses ();
  TrainingClassList::const_iterator  idx;
  for  (idx = trainingClasses.begin ();  idx != trainingClasses.end ();  idx++)
  {
    const TrainingClassPtr  tc = *idx;
    TrainingClassPtr  newTc = new TrainingClass (*tc);
    results->Add (gcnew UmiTrainingClass (newTc));
  }

  return  results;
}  /* TrainingClasses */




void   UmiTrainingConfiguration::Save (String^  configFileName)
{
  KKStr  fileName = UmiKKStr::SystemStringToKKStr (configFileName);
  config->Save (fileName);
}  /* Save */
