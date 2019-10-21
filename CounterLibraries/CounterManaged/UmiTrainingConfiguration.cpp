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
#include "..\\KKBase\\GoalKeeper.h"
#include "KKStr.h"
#include "OSservices.h"
using namespace KKB;

#include "FileDesc.h"
#include "FeatureFileIOKK.h"
#include "PostLarvaeFV.h"
#include "TrainingConfiguration2.h"
using namespace  KKMLL;

#include "CounterVariables.h"
using namespace  CounterBase;

#include "OperatingParameters.h"

#include "UmiTrainingConfiguration.h"

#include "UmiKKStr.h"
#include "UmiRunLog.h"
#include "UmiVariables.h"
using namespace CounterManaged;


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

  std::stringstream  logStr;
  KKStr  configFileName = UmiKKStr::SystemStringToKKStr (_configFileName);
  config = new CounterTrainingConfiguration ();
  config->Load (configFileName, op, true, log->Log ());
  valid = gcnew System::Boolean (config->FormatGood ());
  if  (!config->FormatGood ())
     loadLogStream = gcnew String (logStr.str ().c_str ());
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
  KKStr  configFileName = osAddSlash (CounterVariables::TrainingModelsDir ()) + modelNameKKStr;
    
  delete  classes;  classes = NULL;

  config = CounterTrainingConfiguration::CreateFromDirectoryStructure 
                                        (configFileName,
                                         dirName,
                                         op,
                                         log->Log (),
                                         successful,
                                         errorMessage
                                        );

  valid = gcnew System::Boolean (successful);

  classes = config->ExtractClassList ();

  // We are never ever supposed to delete a 'FileDesc' object.  they are managed wholly in the "FileDesc" class.  
  // Other parts of the application may and probably will be using this same instance.
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





void  UmiTrainingConfiguration::CancelLoad ()  // Call this method to set the 'canclFlag' to true.
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






UmiFeatureVectorList^  UmiTrainingConfiguration::LoadFeatureDataFromTrainingLibraries (bool^      _changesMadeToTrainingLibraries,
                                                                                       UmiRunLog^ _log
                                                                                      )
{
  if  (!config)
    return  nullptr;

  if  (_log == nullptr)
    _log = gcnew UmiRunLog ();

  *cancelFlag = false;

  bool  changesMadeToTrainingLibraries = false;

  KKB::DateTime  latestImageTimeStamp;

  FeatureVectorListPtr  data = config->LoadFeatureDataFromTrainingLibraries 
         (latestImageTimeStamp,
          changesMadeToTrainingLibraries,
          *cancelFlag,
          _log->Log ()
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
  OptionUInt32  lineNum = {};
  KKStrConstPtr  value = config->SettingValue (UmiKKStr::SystemStringToKKStr (sectionName), UmiKKStr::SystemStringToKKStr (settingName), lineNum);
  if  (value == NULL)
    return nullptr;
  else
    return UmiKKStr::KKStrToSystenStr (*value);
}



Dictionary<String^,String^>^   UmiTrainingConfiguration::GetSettingValues (String^  _sectionName)
{
  auto  sectionNum =  config->SectionNum (UmiKKStr::SystemStringToKKStr (_sectionName));
  if  (!sectionNum)
    return nullptr;

  Dictionary<String^,String^>^ settings = gcnew Dictionary<String^,String^> ();

  kkuint32 settingNum = 0;
  do
  {
    OptionUInt32  lineNum = {};
    KKStrConstPtr   settingValue = config->SettingValue (sectionNum.value (), settingNum, lineNum);
    if  (settingValue == NULL)
      break;

    KKStrConstPtr   settingName = config->SettingName (sectionNum.value (), settingNum);
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
