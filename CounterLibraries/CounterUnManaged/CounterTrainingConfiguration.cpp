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

#include "CounterVariables.h"
using namespace  CounterBase;

#include "CounterTrainingConfiguration.h"
#include "CounterFVProducer.h"
#include "OperatingParameters.h"
#include "PostLarvaeFVProducer.h"
using namespace  CounterUnManaged;



CounterTrainingConfiguration::CounterTrainingConfiguration ():
  TrainingConfiguration2 (),
  operatingParms (NULL)
{
}



CounterTrainingConfiguration::CounterTrainingConfiguration (MLClassListPtr          _mlClasses,
                                                          KKStr                   _parameterStr,
                                                          OperatingParametersPtr  _initialOperatingParameters,
                                                          RunLog&                 _log
                                                         ):
  TrainingConfiguration2 (_mlClasses, CounterFVProducerFactory::Factory (&_log), _parameterStr, _log),
  operatingParms (NULL)
{
  if  (_initialOperatingParameters)
    operatingParms = new OperatingParameters (*_initialOperatingParameters);
  else
    operatingParms = new OperatingParameters ();

  operatingParms->UpdateFromConfiguration (*this);
}



CounterTrainingConfiguration::CounterTrainingConfiguration (const CounterTrainingConfiguration&  tc):
  TrainingConfiguration2 (tc),
  operatingParms (NULL)
{
  if  (tc.operatingParms)
    operatingParms = new OperatingParameters (*(tc.operatingParms));
  else
    operatingParms = new OperatingParameters ();
} 



CounterTrainingConfiguration::~CounterTrainingConfiguration ()
{
  delete  operatingParms;
  operatingParms = NULL;
}



void  CounterTrainingConfiguration::Load (const KKStr&           _configFileName,
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



CounterTrainingConfigurationPtr  CounterTrainingConfiguration::Duplicate ()  const
{
  return new CounterTrainingConfiguration (*this);
}



FactoryFVProducerPtr   CounterTrainingConfiguration::DefaultFeatureVectorProducer (RunLog&  runLog)  const
{
  return CounterFVProducerFactory::Factory (&runLog);
}



void  CounterTrainingConfiguration::Save (const KKStr& fileNameToSaveTo)  const
{
  ofstream  o (fileNameToSaveTo.Str ());

  TrainingConfiguration2::Save (o);

  if  (operatingParms)
    operatingParms->WriteConfigSection (o);

  o.close ();
}  /* Save */



void  CounterTrainingConfiguration::SetOperatingParms (const OperatingParameters&  _operatingParms)
{
  operatingParms->Assign (_operatingParms);
}



CounterTrainingConfigurationPtr  CounterTrainingConfiguration::CreateFromFeatureVectorList
                                                    (FeatureVectorList&      _examples,
                                                     OperatingParametersPtr  _initialOperatingParameters,
                                                     RunLog&                 _log
                                                    )
{
  _log.Level (10) << "CounterTrainingConfiguration::CreateFromFeatureVectorList" << endl;

  MLClassListPtr  mlClasses = _examples.ExtractListOfClasses ();
  mlClasses->SortByName ();

  CounterTrainingConfigurationPtr  config 
      = new CounterTrainingConfiguration (mlClasses, 
                                         "-m 200 -s 0 -n 0.11 -t 2 -g 0.024717  -c 10  -u 100  -up  -mt OneVsOne  -sm P", 
                                         _initialOperatingParameters,
                                         _log
                                        );

  config->SetFeatureNums (_examples.AllFeatures ());

  delete  mlClasses;  mlClasses = NULL;

  return  config;
}  /* CreateFromFeatureVectorList */



CounterTrainingConfigurationPtr  CounterTrainingConfiguration::CreateFromDirectoryStructure 
                                                    (const KKStr&            _existingConfigFileName,
                                                     const KKStr&            _subDir,
                                                     OperatingParametersPtr  _initialOperatingParameters,
                                                     RunLog&                 _log,
                                                     bool&                   _successful,
                                                     KKStr&                  _errorMessage
                                                    )
{
  _log.Level (10) << "CounterTrainingConfiguration::CreateFromDirectoryStructure"  << endl
                  << "                       Feature Data from SubDir[" << _subDir << "]." << endl
                  << endl;

  _successful = true;

  KKStr  directoryConfigFileName = osGetRootNameOfDirectory (_subDir);
  if  (directoryConfigFileName.Empty ())
    directoryConfigFileName = osAddSlash (CounterVariables::TrainingModelsDir ()) + "Root.cfg";
  else
    directoryConfigFileName = osAddSlash (CounterVariables::TrainingModelsDir ()) + directoryConfigFileName + ".cfg";

  CounterTrainingConfigurationPtr  config = NULL;

  if  (!_existingConfigFileName.Empty ())
  {
    if  (osFileExists (_existingConfigFileName))
    {
      config = new CounterTrainingConfiguration ();
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
      config = new CounterTrainingConfiguration ();
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
    config = new CounterTrainingConfiguration (NULL,      // Not supplying the MLClassList
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



void  CounterTrainingConfiguration::WriteXML (const KKStr&  varName,
                                             ostream&      o
                                            )  const
{
  XmlTag  startTag ("CounterTrainingConfiguration", XmlTag::TagTypes::tagStart);
  if  (!varName.Empty ())
    startTag.AddAtribute ("VarName", varName);
  startTag.WriteXML (o);
  o << endl;

  WriteXMLFields (o);

  if  (operatingParms)
    operatingParms->WriteXML ("OperatingParms", o);

  XmlTag  endTag ("CounterTrainingConfiguration", XmlTag::TagTypes::tagEnd);
  endTag.WriteXML (o);
  o << endl;
}  /* WriteXML */



void   CounterTrainingConfiguration::ReadXML (XmlStream&      s,
                                              XmlTagConstPtr  tag,
                                              VolConstBool&   cancelFlag,
                                              RunLog&         log
                                             )

{
  log.Level (30) << "CounterTrainingConfiguration::ReadXML   tag->name: '" << tag->Name () << "'." << endl;
  XmlTokenPtr t = s.GetNextToken (cancelFlag, log);
  while  (t  &&  (!cancelFlag))
  {
    t = ReadXMLBaseToken (t, cancelFlag, log);
    if  (t)
    {
      const KKStr&  varName = t->VarName ();
      if  (varName.EqualIgnoreCase ("OperatingParms")  &&  (typeid (*t) == typeid(XmlElementOperatingParameters)))
        operatingParms = dynamic_cast<XmlElementOperatingParametersPtr> (t)->TakeOwnership ();
    }
    delete  t;
    t = s.GetNextToken (cancelFlag, log);
  }
  delete  t;
  t = NULL;
  if (!cancelFlag)
    ReadXMLPost (cancelFlag, log);
}  /* ReadXML */



XmlFactoryMacro(CounterTrainingConfiguration)
