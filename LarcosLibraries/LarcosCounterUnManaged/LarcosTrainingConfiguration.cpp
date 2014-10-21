#include "FirstIncludes.h"

#include <stdio.h>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "MemoryDebug.h"

using namespace  std;


#include "KKBaseTypes.h"
#include "KKException.h"
#include "OSservices.h"
#include "RunLog.h"
using namespace  KKB;

//#include "Variables.h"
//using namespace  KKLSC;

#include "FeatureVector.h"
#include "FileDesc.h"
#include "MLClass.h"
#include "PostLarvaeFV.h"
#include "TrainingConfiguration2.h"
using namespace  KKMachineLearning;

#include "LarcosVariables.h"
using namespace  LarcosBase;

#include "LarcosTrainingConfiguration.h"
#include "OperatingParameters.h"
using namespace  LarcosCounterUnManaged;



LarcosTrainingConfiguration::LarcosTrainingConfiguration (FileDescPtr            _fileDesc,
                                                          const KKStr&           _configFileName, 
                                                          OperatingParametersPtr _initialOperatingParameters,
                                                          RunLog&                _log,
                                                          bool                   _validateDirectories
                                                         ):
  TrainingConfiguration2 (_fileDesc, _configFileName, _log, _validateDirectories),
  operatingParms (NULL)

{
  if  (_initialOperatingParameters)
    operatingParms = new OperatingParameters (*_initialOperatingParameters);
  else
    operatingParms = new OperatingParameters ();
  operatingParms->UpdateFromConfiguration (*this);
}




LarcosTrainingConfiguration::LarcosTrainingConfiguration (FileDescPtr             _fileDesc,
                                                          MLClassListPtr          _mlClasses,
                                                          KKStr                   _parameterStr,
                                                          OperatingParametersPtr  _initialOperatingParameters,
                                                          RunLog&                 _log
                                                         ):
  TrainingConfiguration2 (_fileDesc, _mlClasses, _parameterStr, _log),
  operatingParms (NULL)
{
  if  (_initialOperatingParameters)
    operatingParms = new OperatingParameters (*_initialOperatingParameters);
  else
    operatingParms = new OperatingParameters ();

  operatingParms->UpdateFromConfiguration (*this);
}



LarcosTrainingConfiguration::LarcosTrainingConfiguration (const LarcosTrainingConfiguration&  tc):
  TrainingConfiguration2 (tc),
  operatingParms (NULL)
{
  if  (tc.operatingParms)
    operatingParms = new OperatingParameters (*(tc.operatingParms));
  else
    operatingParms = new OperatingParameters ();
} 




LarcosTrainingConfiguration::~LarcosTrainingConfiguration ()
{
  delete  operatingParms;
  operatingParms = NULL;
}



void  LarcosTrainingConfiguration::Save (const KKStr& fileName)
{
  ofstream  o (fileName.Str ());

  TrainingConfiguration2::Save (o);

  if  (operatingParms)
    operatingParms->WriteConfigSection (o);

  o.close ();
}  /* Save */




void  LarcosTrainingConfiguration::SetOperatingParms (const OperatingParameters&  _operatingParms)
{
  operatingParms->Assign (_operatingParms);
}




LarcosTrainingConfigurationPtr  LarcosTrainingConfiguration::CreateFromFeatureVectorList
                                                    (FeatureVectorList&      _examples,
                                                     MLClassListPtr          _mlClasses,
                                                     OperatingParametersPtr  _initialOperatingParameters,
                                                     RunLog&                 _log
                                                    )
{
  _log.Level (10) << "LarcosTrainingConfiguration::CreateFromFeatureVectorList" << endl;
  FileDescPtr  fileDesc = _examples.FileDesc ();

  MLClassListPtr  mlClasses = _examples.ExtractListOfClasses ();
  mlClasses->SortByName ();

  LarcosTrainingConfigurationPtr  config 
      = new LarcosTrainingConfiguration (fileDesc,
                                         mlClasses, 
                                         "-m 200 -s 0 -n 0.11 -t 2 -g 0.024717  -c 10  -u 100  -up  -mt OneVsOne  -sm P", 
                                         _initialOperatingParameters,
                                         _log
                                        );

  config->SetFeatureNums (_examples.AllFeatures ());

  delete  mlClasses;  mlClasses = NULL;

  return  config;
}  /* CreateFromFeatureVectorList */





LarcosTrainingConfigurationPtr  LarcosTrainingConfiguration::CreateFromDirectoryStructure 
                                                    (FileDescPtr             _fileDesc,
                                                     const KKStr&            _existingConfigFileName,
                                                     const KKStr&            _subDir,
                                                     OperatingParametersPtr  _initialOperatingParameters,
                                                     RunLog&                 _log,
                                                     bool&                   _successful,
                                                     KKStr&                  _errorMessage
                                                    )
{
  _log.Level (10) << "LarcosTrainingConfiguration::CreateFromDirectoryStructure"  << endl
                  << "                       Feature Data from SubDir[" << _subDir << "]." << endl
                  << endl;

  _successful = true;

  if  (_fileDesc == NULL)
    _fileDesc = PostLarvaeFV::PostLarvaeFeaturesFileDesc ();

  KKStr  directoryConfigFileName = osGetRootNameOfDirectory (_subDir);
  if  (directoryConfigFileName.Empty ())
    directoryConfigFileName = osAddSlash (LarcosVariables::TrainingModelsDir ()) + "Root.cfg";
  else
    directoryConfigFileName = osAddSlash (LarcosVariables::TrainingModelsDir ()) + directoryConfigFileName + ".cfg";

  LarcosTrainingConfigurationPtr  config = NULL;

  if  (!_existingConfigFileName.Empty ())
  {
    if  (osFileExists (_existingConfigFileName))
    {
      config = new LarcosTrainingConfiguration (_fileDesc,
                                                _existingConfigFileName,
                                                _initialOperatingParameters,
                                                _log, 
                                                false
                                               );
      config->RootDir (_subDir);
      if  (!(config->FormatGood ()))
      {
        delete  config;
        config = NULL;
      }
    }
  }

  if  (!config)
  {
    if  (osFileExists (directoryConfigFileName))
    {
      config = new LarcosTrainingConfiguration (_fileDesc,
                                                directoryConfigFileName,
                                                _initialOperatingParameters,
                                                _log,
                                                false
                                               );
      config->RootDir (_subDir);
      if  (!(config->FormatGood ()))
      {
        delete  config;
        config = NULL;
      }
    }
  }

  if  (!config)
  {
    config = new LarcosTrainingConfiguration (_fileDesc,
                                              NULL,      // Not supplying the MLClassList
                                              "=-s 0 -n 0.11 -t 2 -g 0.01507  -c 12  -u 100  -up  -mt OneVsOne  -sm P",
                                              _initialOperatingParameters,
                                              _log
                                             );
    config->RootDir (_subDir);
  }

  config->BuildTrainingClassListFromDirectoryStructure (_subDir,
                                                        _successful, 
                                                        _errorMessage
                                                       );

  config->Save (directoryConfigFileName);

  return  config;
} /* CreateFromDirectoryTree */


