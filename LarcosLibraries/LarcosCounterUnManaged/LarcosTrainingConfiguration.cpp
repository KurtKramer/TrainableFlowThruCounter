#include "FirstIncludes.h"
#include <stdio.h>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include "MemoryDebug.h"

using namespace  std;


#include "GlobalGoalKeeper.h"
#include "KKBaseTypes.h"
#include "KKException.h"
#include "OSservices.h"
#include "RunLog.h"
using namespace  KKB;


#include "FeatureVector.h"
#include "FileDesc.h"
#include "MLClass.h"
#include "PostLarvaeFV.h"
#include "TrainingConfiguration2.h"
using namespace  KKMLL;

#include "LarcosVariables.h"
using namespace  LarcosBase;

#include "LarcosTrainingConfiguration.h"
#include "LarcosFVProducer.h"
#include "OperatingParameters.h"
#include "PostLarvaeFVProducer.h"
using namespace  LarcosCounterUnManaged;



LarcosTrainingConfiguration::LarcosTrainingConfiguration ():
  TrainingConfiguration2 (),
  operatingParms (NULL)
{
}



LarcosTrainingConfiguration::LarcosTrainingConfiguration (MLClassListPtr          _mlClasses,
                                                          KKStr                   _parameterStr,
                                                          OperatingParametersPtr  _initialOperatingParameters,
                                                          RunLog&                 _log
                                                         ):
  TrainingConfiguration2 (_mlClasses, LarcosFVProducerFactory::Factory (&_log), _parameterStr, _log),
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



void  LarcosTrainingConfiguration::Load (const KKStr&           _configFileName,
                                         OperatingParametersPtr _initialOperatingParameters,
                                         bool                   _validateDirectories,  /**<  Used to default to 'true'. */
                                         RunLog&                _log
                                        )
{
  TrainingConfiguration2::Load (_configFileName, _validateDirectories, _log);
  if  (_initialOperatingParameters)
    operatingParms = new OperatingParameters (*_initialOperatingParameters);
  else
    operatingParms = new OperatingParameters ();
  operatingParms->UpdateFromConfiguration (*this);
}




LarcosTrainingConfigurationPtr  LarcosTrainingConfiguration::Duplicate ()  const
{
  return new LarcosTrainingConfiguration (*this);
}





FactoryFVProducerPtr   LarcosTrainingConfiguration::DefaultFeatureVectorProducer (RunLog&  runLog)  const
{
  return LarcosFVProducerFactory::Factory (&runLog);
}



void  LarcosTrainingConfiguration::Save (const KKStr& fileName)  const
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
                                                     OperatingParametersPtr  _initialOperatingParameters,
                                                     RunLog&                 _log
                                                    )
{
  _log.Level (10) << "LarcosTrainingConfiguration::CreateFromFeatureVectorList" << endl;
  FileDescPtr  fileDesc = _examples.FileDesc ();

  MLClassListPtr  mlClasses = _examples.ExtractListOfClasses ();
  mlClasses->SortByName ();

  LarcosTrainingConfigurationPtr  config 
      = new LarcosTrainingConfiguration (mlClasses, 
                                         "-m 200 -s 0 -n 0.11 -t 2 -g 0.024717  -c 10  -u 100  -up  -mt OneVsOne  -sm P", 
                                         _initialOperatingParameters,
                                         _log
                                        );

  config->SetFeatureNums (_examples.AllFeatures ());

  delete  mlClasses;  mlClasses = NULL;

  return  config;
}  /* CreateFromFeatureVectorList */





LarcosTrainingConfigurationPtr  LarcosTrainingConfiguration::CreateFromDirectoryStructure 
                                                    (const KKStr&            _existingConfigFileName,
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
      config = new LarcosTrainingConfiguration ();
      config->Load (_existingConfigFileName, 
                    _initialOperatingParameters, 
                    false,   // false = Don't validate directories.
                    _log
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
      config = new LarcosTrainingConfiguration ();
      config->Load (directoryConfigFileName,
                    _initialOperatingParameters,
                    false,  // false = Dont validate directories.
                    _log
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
    config = new LarcosTrainingConfiguration (NULL,      // Not supplying the MLClassList
                                              "=-s 0 -n 0.11 -t 2 -g 0.01507  -c 12  -u 100  -up  -mt OneVsOne  -sm P",
                                              _initialOperatingParameters,
                                              _log
                                             );
    config->RootDir (_subDir);
  }

  config->BuildTrainingClassListFromDirectoryStructure (_subDir,
                                                        _successful, 
                                                        _errorMessage,
                                                        _log
                                                       );

  config->Save (directoryConfigFileName);

  return  config;
} /* CreateFromDirectoryTree */





void  LarcosTrainingConfiguration::WriteXML (const KKStr&  varName,
                                             ostream&      o
                                            )  const
{
  XmlTag  startTag ("LarcosTrainingConfiguration", XmlTag::TagTypes::tagStart);
  if  (!varName.Empty ())
    startTag.AddAtribute ("VarName", varName);
  startTag.WriteXML (o);
  o << endl;

  WriteXMLFields (o);

  if  (operatingParms)
    operatingParms->WriteXML ("OperatingParms", o);

  XmlTag  endTag ("LarcosTrainingConfiguration", XmlTag::TagTypes::tagEnd);
  endTag.WriteXML (o);
  o << endl;
}  /* WriteXML */




void   LarcosTrainingConfiguration::ReadXML (XmlStream&      s,
                                             XmlTagConstPtr  tag,
                                             RunLog&         log
                                            )

{
  XmlTokenPtr t = s.GetNextToken (log);
  while  (t)
  {
    t = ReadXMLBaseToken (t, log);
    if  (t)
    {
      const KKStr&  varName = t->VarName ();
      if  (varName.EqualIgnoreCase ("OperatingParms")  &&  (typeid (*t) == typeid(XmlElementOperatingParameters)))
        operatingParms = dynamic_cast<XmlElementOperatingParametersPtr> (t)->TakeOwnership ();
    }
    delete  t;
    t = s.GetNextToken (log);
  }
  ReadXMLPost (log);
}  /* ReadXML */



XmlFactoryMacro(LarcosTrainingConfiguration)
