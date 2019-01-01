#include "StdAfx.h"
#include "FirstIncludes.h"
#include <stdio.h>
#include <math.h>
#include <ctype.h>
#include <time.h>
#include <fstream>
#include <iostream>
#include <map>
#include <ostream>
#include <string>
#include <vector>
#include "MemoryDebug.h"


#include "Application.h"
#include "KKBaseTypes.h"
#include "..\\KKBase\\GoalKeeper.h"
#include "ImageIO.h"
#include "Raster.h"
#include "RunLog.h"
#include "KKStr.h"
using namespace KKB;


//#include  "Variables.h"
//using namespace  KKLSC;


#include "Classifier2.h"
#include "FeatureFileIOKK.h"
#include "TrainingConfiguration2.h"
using namespace KKMLL;

#include "CounterVariables.h"
using namespace  CounterBase;


//#include "PostLarvaeFVProducer.h"
#include "CounterFVProducer.h"
using namespace CounterUnManaged;


#include "UmiFeatureVector.h"
#include "UmiFeatureVectorList.h"
#include "UmiVariables.h"
#include "UmiTrainingConfiguration.h"
#include "UmiKKStr.h"
#include "UmiOSservices.h"
#include "UmiRaster.h"

#include "UmiTrainingModel2.h"
using namespace CounterManaged;





using namespace System;
using namespace System::Drawing::Imaging;
using namespace System::IO;
using namespace System::Windows::Forms;
using namespace System::Threading;

  
  
UmiTrainingModel2::UmiTrainingModel2 (UmiRunLog^       _umiRunLog,
                                      System::String^  _modelName
                                     ):
              cancelFlag                 (new bool),
              classifier                 (NULL),
              classes                    (NULL),
              classList                  (gcnew  UmiClassList ()),
              colorValues                (nullptr),
              config                     (NULL),
              crossProbTable             (NULL),
              crossProbTableNumClasses   (0),
              factoryFVProducer          (NULL),
              fvProducer                 (NULL),
              loadTrainingLibraryRunning (false),
              modelName                  (System::String::Copy (_modelName)),
              umiRunLog                  (_umiRunLog),
              probabilities              (NULL),
              runLog                     (NULL),
              runLogFileName             (nullptr),
              runLogLastLineNum          (0),
              statusMsg                  (new KKStr ()),
              trainer                    (NULL),
              valid                      (new bool (true)),
              votes                      (NULL)
               
{
  if  (System::String::IsNullOrEmpty (_modelName))
  {
    *valid = false;
    return;
  }

  GC::Collect ();

  CreateRunLog ();

  osRunAsABackGroundProcess ();

  //factoryFVProducer =  LarcosFVProducerFactory::Factory (runLog);
}





UmiTrainingModel2::UmiTrainingModel2 (UmiRunLog^                 _umiRunLog,
                                      UmiTrainingConfiguration^  _config
                                     ):

              cancelFlag                 (new bool),
              classifier                 (NULL),
              classes                    (NULL),
              classList                  (gcnew  UmiClassList ()),
              colorValues                (nullptr),
              config                     (NULL),
              crossProbTable             (NULL),
              crossProbTableNumClasses   (0),
              factoryFVProducer          (NULL),
              fvProducer                 (NULL),
              loadTrainingLibraryRunning (false),
              modelName                  (_config->ModelName ()),
              umiRunLog                  (_umiRunLog),
              probabilities              (NULL),
              runLog                     (NULL),
              runLogFileName             (nullptr),
              runLogLastLineNum          (0),
              statusMsg                  (new KKStr ()),
              trainer                    (NULL),
              valid                      (new bool (true)),
              votes                      (NULL)
               
{
  if  (_config == nullptr)
  {
    *valid = false;
    return;
  }

  GC::Collect ();

  CreateRunLog ();

  osRunAsABackGroundProcess ();

  config = new CounterTrainingConfiguration (*(_config->Config ()));
  factoryFVProducer =  config->FvFactoryProducer (_umiRunLog->Log ());
}






UmiTrainingModel2::UmiTrainingModel2 (UmiRunLog^      _umiRunLog,
                                      DirectoryInfo^  _directoryInfo
                                     ):
              cancelFlag                 (new bool),
              classifier                 (NULL),
              classes                    (NULL),
              classList                  (gcnew  UmiClassList ()),
              config                     (NULL),
              crossProbTable             (NULL),
              crossProbTableNumClasses   (0),
              factoryFVProducer          (NULL),
              fvProducer                 (NULL),
              loadTrainingLibraryRunning (false),
              modelName                  (nullptr),
              umiRunLog                  (_umiRunLog),
              probabilities              (NULL),
              runLog                     (NULL),
              runLogFileName             (nullptr),
              runLogLastLineNum          (0),
              statusMsg                  (new KKStr ()),
              trainer                    (NULL),
              valid                      (new bool (true)),
              votes                      (NULL)

{
  osRunAsABackGroundProcess ();

  if  (_directoryInfo == nullptr)
  {
    *valid = false;
    return;
  }


  GC::Collect ();

  KKStr  errorMessage;

  KKStr  dirName    = UmiKKStr::SystemStringToKKStr (_directoryInfo->FullName);
  bool   successful = false;

  KKStr  modelNameKKStr = osGetRootNameOfDirectory (dirName) + ".cfg";
  modelName = UmiKKStr::KKStrToSystenStr (modelNameKKStr);

  CreateRunLog ();

  KKStr  configFileName = osAddSlash (CounterVariables::TrainingModelsDir ()) + modelNameKKStr;

  FileDescConstPtr fd = CounterFVProducer::DefineFileDescStatic ();

  try
  {
    config = CounterTrainingConfiguration::CreateFromDirectoryStructure 
                                          (configFileName,
                                           dirName,
                                           NULL,           // operatingParameters,
                                           *runLog,
                                           successful,
                                           errorMessage
                                          );
  }
  catch  (Exception^ e)
  {
    successful = false;
    errorMessage = UmiKKStr::SystemStringToKKStr (e->ToString ());
  }
  if  (!successful)
  {
    delete config; 
    config = NULL;
    *valid = false;
    return;
  }

  factoryFVProducer =  config->FvFactoryProducer (*runLog);
  delete  classes;  classes = NULL;
  classes = config->ExtractClassList ();
  PopulateCSharpClassList ();
}



UmiTrainingModel2::~UmiTrainingModel2 ()
{
  this->!UmiTrainingModel2 (); 
}



UmiTrainingModel2::!UmiTrainingModel2()
{
  CleanUpUnmanagedResources ();
}



void  UmiTrainingModel2::CleanUpUnmanagedResources ()
{
  delete  classifier;
  classifier = NULL;

  try
  {
    delete  trainer;         
    trainer = NULL;
  }
  catch  (const std::exception&  e1)
  {
    runLog->Level (-1) << std::endl
      << "UmiTrainingModel2::CleanUpUnmanagedResources  ***ERROR***     Exception destroying the 'Trainer Object." << std::endl
      << "                Exception[" << e1.what () << "]" << std::endl
      << std::endl;

  }

  catch  (const char* e2)
  {
    runLog->Level (-1) << std::endl
      << "UmiTrainingModel2::CleanUpUnmanagedResources  ***ERROR***     Exception destroying the 'Trainer Object." << std::endl
      << "                Exception[" << e2 << "]" << std::endl
      << std::endl;
  }

  catch  (...)
  {
    runLog->Level (-1) << std::endl
      << "UmiTrainingModel2::CleanUpUnmanagedResources  ***ERROR***     Exception destroying the 'Trainer Object." << std::endl
      << std::endl;

  }
  trainer = NULL;
 
  delete  cancelFlag;      cancelFlag     = NULL;
  delete  classes;         classes        = NULL;
  delete  config;          config         = NULL;
  delete  fvProducer;      fvProducer     = NULL;
  delete  statusMsg;       statusMsg      = NULL;
  delete  valid;           valid          = NULL;
  delete  probabilities;   probabilities  = NULL;
  delete  votes;           votes          = NULL;

  if  (crossProbTable)
  {
    for  (int x = 0;  x < crossProbTableNumClasses;  x++)
      delete  crossProbTable[x];
    delete  crossProbTable;
    crossProbTable = NULL;
  }


  if  (runLog)
  {
    (*runLog).Level (10) << "UmiTrainingModel2::CleanUpUnmanagedResources  Done Cleaning Up." << std::endl;
    delete  runLog;
    runLog = NULL;
  }

    GC::Collect ();
}  /* CleanUpUnmanagedResources */




void  UmiTrainingModel2::CreateRunLog ()
{
  if  (runLog == NULL)
  {
    String^  dir = UmiOSservices::AddSlash (UmiVariables::TempDir ()) + "RunLogs";
    UmiOSservices::CreateDirectoryPath (dir);

    bool fileNameAlreadyUsed = true;
    do  
    {
      runLogFileName = UmiOSservices::AddSlash (dir) + UmiOSservices::GetRootName (modelName) + "_RunLog_" + System::DateTime::Now.ToString ("yyyyMMdd-HHmmss-fff") + ".txt";
      fileNameAlreadyUsed  = File::Exists (runLogFileName);
      if  (fileNameAlreadyUsed)
        Thread::Sleep (100);
    }  
      while  (fileNameAlreadyUsed);

    runLog = new RunLog (UmiKKStr::SystemStringToKKStr (runLogFileName).Str ());
    runLog->SetLevel (CounterVariables::DebugLevel ());
  }
}  /* CreateRunLog */



String^  UmiTrainingModel2::LastRunLogTextLine ()
{
  if  (runLog == NULL)
    return nullptr;

  String^  lastLine = nullptr;

  if  (runLogLastLineNum != runLog->LineCount ())
  {
    lastLine = gcnew String (UmiKKStr::KKStrToSystenStr (runLog->LastLine ()));
    runLogLastLineNum = runLog->LineCount ();
  }

  return  lastLine;
}  /* LastRunLogTextLine */






System::DateTime  UmiTrainingModel2::BuildDateTime::get ()
{
  if  (!trainer)
    return System::DateTime (1900, 1, 1);

  return  UmiOSservices::KKuDateTimeToSystemDateTime (trainer->BuildDateTime ());
}





String^  UmiTrainingModel2::RootDirExpanded::get ()
{
  LarcosTrainingConfigurationConstPtr  configToUse = GetConfigToUse ();
  if  (configToUse)
    return UmiKKStr::KKStrToSystenStr (configToUse->RootDirExpanded ());
  else
    return  UmiOSservices::AddSlash (UmiVariables::CounterHomeDir ()) + UmiOSservices::GetRootName (modelName);
}



void  UmiTrainingModel2::SaveConfiguration ()
{
  LarcosTrainingConfigurationConstPtr  configToUse = GetConfigToUse ();
  KKStr  fileName = configToUse->FileName ();
  if  (fileName.Empty ())
    fileName = osAddSlash (CounterVariables::TrainingModelsDir ()) + UmiKKStr::SystemStringToKKStr (modelName);

  configToUse->Save (fileName);
}  /* SaveConfiguration*/





void  UmiTrainingModel2::PopulateCSharpClassList ()
{
  osRunAsABackGroundProcess ();

  if  (classList != nullptr)
  {
    classList = nullptr;

    delete  probabilities;  probabilities = NULL;
    delete  votes;          votes = NULL;
    
    if  (crossProbTable)
    {
      for  (int x = 0;  x < crossProbTableNumClasses;  x++)
        delete  crossProbTable[x];
      delete  crossProbTable;
      crossProbTable = NULL;
    }
  }

  classList = gcnew UmiClassList ();
  if  (classes != NULL)
  {
    crossProbTableNumClasses = classes->QueueSize ();
    crossProbTable = new double*[crossProbTableNumClasses];
    int  idx;
    for  (idx = 0;  idx < crossProbTableNumClasses;  idx++)
    {
      MLClassPtr  ic = classes->IdxToPtr (idx);
      System::String^  className = UmiKKStr::KKStrToSystenStr (ic->Name ());
      UmiClass^  cSharpClass = UmiClassList::GetUniqueClass (className, "");
      classList->Add (cSharpClass);

      crossProbTable[idx] = new double[crossProbTableNumClasses];

      for  (int  idx2 = 0;  idx2 < crossProbTableNumClasses;  idx2++)
         crossProbTable[idx][idx2] = 0.0;
    }

    probabilities = new double[classes->QueueSize () + 10];
    votes         = new int   [classes->QueueSize () + 10];
  }
}  /* PopulateCSharpClassList */





bool  UmiTrainingModel2::IncludesClass (UmiClass^  mlClass)
{
  if  (mlClass == nullptr)
    return false;

  LarcosTrainingConfigurationConstPtr  configToUse = GetConfigToUse ();
  if  (!configToUse)
    return false;

  KKStr  dirPath = configToUse->DirectoryPathForClass (mlClass->UnmanagedImageClass ());
  
  return  (!dirPath.Empty ());
}  /* IncludesClass */





// Returns a list of classes that belong to this model. It will be created from "classList"; so the caller 
// can do with it as they want.
UmiClassList^  UmiTrainingModel2::ImageClasses ()
{
  if  (classList == nullptr)
    return  nullptr;

  return gcnew UmiClassList (classList);
}




UmiOperatingParameters^  UmiTrainingModel2::GetOperatingParameters ()
{
  if  ((!config)  ||  (config->OperatingParms () == NULL))
    return nullptr;
  else
    return gcnew UmiOperatingParameters (new OperatingParameters (*(config->OperatingParms ())));
}




void  UmiTrainingModel2::LoadExistingModelOtherwiseBuild ()
{
  LoadExistingTrainedModel ();
  if  ((!trainer)  &&  (!(*cancelFlag)))
    LoadTrainigLibrary (true);

  if  ((!(*cancelFlag))  &&  (*valid))
  {
    delete  classes;  classes = NULL;
    if  (trainer->MLClasses ())
      classes = new MLClassList (*(trainer->MLClasses ()));
    else
      throw gcnew Exception ("UmiTrainingModel2::LoadExistingModelOtherwiseBuild     (trainer->MLClasses () == NULL)");

    PopulateCSharpClassList ();
  }
} /* LoadExistingModelOtherwiseBuild */




void  UmiTrainingModel2::LoadTrainingModelForGivenLevel (kkuint32 level)
{
  GC::Collect ();

  if  (!factoryFVProducer)
    factoryFVProducer = GetConfigToUse ()->FvFactoryProducer (*runLog);

  *cancelFlag = false;

  try
  {
    trainer = TrainingProcess2::CreateTrainingProcessForLevel (GetConfigToUse (), level, *cancelFlag, *runLog);
  }
  catch (System::AccessViolationException^ z)
  {
    (*runLog).Level (-1) << UmiKKStr::SystemStringToKKStr (z->ToString ()) << std::endl;
    delete  trainer;  trainer = NULL;
    delete  classes;  classes = NULL;
    *valid = false;
    return;
  }

  if  (trainer->Abort ())
  {
    *valid = false;
    delete  trainer;
    trainer = NULL;
  }
  else
  {
    *valid = true;
    delete  classes;  classes = NULL;

    if  (trainer->MLClasses ())
      classes = new MLClassList (*(trainer->MLClasses ()));
    else
      throw gcnew Exception ("UmiTrainingModel2::LoadTrainingModelForGivenLevel     (trainer->ImageClasses() == NULL)");

    classifier = new Classifier2 (trainer, *runLog);
    PopulateCSharpClassList ();
  }
}  /* LoadTrainingModelForGivenLevel*/




void  UmiTrainingModel2::LoadExistingTrainedModel ()

{
  int x = 0;

  GC::Collect ();

  FactoryFVProducer*  fvProducerFactory = GetConfigToUse ()->FvFactoryProducer (*runLog);

  KKB::KKStr  configFileName = UmiKKStr::SystemStringToKKStr (modelName);

  *cancelFlag = false;

  try
  {
    trainer = TrainingProcess2::LoadExistingTrainingProcess (configFileName, *cancelFlag, *runLog);
  }
  catch (System::AccessViolationException^ z)
  {
    (*runLog).Level (-1) << UmiKKStr::SystemStringToKKStr (z->ToString ()) << std::endl;
    delete  trainer;  trainer = NULL;
    delete  classes;  classes = NULL;
    *valid = false;
    return;
  }
  if  (!trainer)
  {
    delete  classes;  classes = NULL;
    *valid = false;
    return;
  }

  trainer->FeaturesAlreadyNormalized (false);
  if  (CancelFlag)
  {
    delete  trainer;  trainer = NULL;
    delete  classes;  classes = NULL;
    *valid = false;
    return;
  }

  if  (trainer->Abort ())
  {
    delete  trainer;  trainer = NULL;
    delete  classes;  classes = NULL;
    *valid = false;
    return;
  }
  else
  {
    *valid = true;
    delete  classes;  classes = NULL;

    if  (trainer->MLClasses ())
      classes = new MLClassList (*(trainer->MLClasses ()));
    else
      throw gcnew Exception ("LoadExistingTrainedModel    (trainer->ImageClasses() == NULL)");

    classifier = new Classifier2 (trainer, *runLog);
    PopulateCSharpClassList ();
  }

  GC::Collect ();

}  /* LoadExistingTrainedModel */



void  UmiTrainingModel2::LoadTrainigLibrary (bool  forceRebuild)
{
  ErrorMsgsClear ();

  int x = 0;

  GC::Collect ();

  //FactoryFVProducer*  fvProducerFactory =   PostLarvaeFVProducerFactory::Factory (runLog);

  if  (!factoryFVProducer)
    factoryFVProducer = GetConfigToUse ()->FvFactoryProducer (*runLog);
  
  FileDescConstPtr fd = factoryFVProducer->FileDesc ();

  KKB::KKStr  configFileName = UmiKKStr::SystemStringToKKStr (modelName);

  *cancelFlag = false;

  try
  {
    TrainingProcess2::WhenToRebuild   whenToRebuild = TrainingProcess2::WhenToRebuild::NotUpToDate;
    if  (forceRebuild)
      whenToRebuild = TrainingProcess2::WhenToRebuild::AlwaysRebuild;


    trainer = TrainingProcess2::CreateTrainingProcess 
                                     (GetConfigToUse (),
                                      false,              // false = DON't check-for-duplicates
                                      whenToRebuild,
                                      true,               // true = save training model if it had ti be rebuilt.
                                      *cancelFlag,
                                      *runLog
                                     );
  }
  catch (System::AccessViolationException^ z)
  {
    (*runLog) << UmiKKStr::SystemStringToKKStr (z->ToString ()) << std::endl;

    if  (trainer)
    {
      try
      {
        ErrorMsgsAdd (trainer->ConfigFileFormatErrors ());
      }
      catch  (Exception^)  
      {
        (*runLog) << "UmiTrainingModel2::LoadTrainigLibrary   ***ERROR***  Exception calling 'ErrorMsgsAdd'." << std::endl;
      }
    }

    delete  trainer;  trainer = NULL;
    *valid = false;
    return;
  }
  catch  (System::Exception^  z2)
  {
    (*runLog) << UmiKKStr::SystemStringToKKStr (z2->ToString ()) << std::endl;
    if  (trainer)
    {
      try{ErrorMsgsAdd (trainer->ConfigFileFormatErrors ());}
      catch  (Exception^)  
      {
      (*runLog) << "UmiTrainingModel2::LoadTrainigLibrary   ***ERROR***  Exception calling 'ErrorMsgsAdd'." << std::endl;
      }
    }
    delete  trainer;  trainer = NULL;
    *valid = false;
    return;
  }

  if  ((*cancelFlag)  ||  (!trainer))
  {
    *valid = false;
    delete  trainer;
    trainer = NULL;
  }

  else if  (trainer->Abort ())
  {
    try  {ErrorMsgsAdd (trainer->ConfigFileFormatErrors ());}  
    catch  (Exception^)  
    {
      (*runLog) << "UmiTrainingModel2::LoadTrainigLibrary   ***ERROR***  Exception calling 'ErrorMsgsAdd'." << std::endl;
    }
    *valid = false;
    delete  trainer;
    trainer = NULL;
  }
  else
  {
    *valid = true;
    delete  classes;  classes = NULL;
    if  (trainer->MLClasses ())
      classes = new MLClassList (*(trainer->MLClasses ()));
    else
      throw gcnew Exception ("UmiTrainingModel2::LoadTrainigLibrary    (trainer->ImageClasses() == NULL)");
    classifier = new Classifier2 (trainer, *runLog);
    PopulateCSharpClassList ();
  }

  GC::Collect ();
}  /* LoadTrainigLibrary */






void  UmiTrainingModel2::BuildTrainingModel (UmiFeatureVectorList^  umiTrainingData)
{
  GC::Collect ();

  PostLarvaeFVResetDarkSpotCounts ();

  FactoryFVProducer*  fvProducerFactory = LarcosFVProducerFactory::Factory (runLog);
  FileDescConstPtr fd = fvProducerFactory->FileDesc ();

  KKB::KKStr  configFileName = UmiKKStr::SystemStringToKKStr (modelName);
  *cancelFlag = false;

  FeatureVectorListPtr  trainingData = new FeatureVectorList (fd, false);
  for each (UmiFeatureVector^ pfv in umiTrainingData)
    trainingData->PushOnBack (pfv->Features ());

  try
  {
    trainer = TrainingProcess2::CreateTrainingProcessFromTrainingExamples
                   (config, 
                    trainingData,
                    false,         // false = DON't take ownership of 'trainingData'.
                    false,         // false = Features are NOT already normalized.
                    *cancelFlag,
                    *runLog
                   );
  }
  catch (System::AccessViolationException^ z)
  {
    (*runLog) << UmiKKStr::SystemStringToKKStr (z->ToString ()) << std::endl;
    delete  trainer;  trainer = NULL;
    *valid = false;
    return;
  }

  if  (trainer->Abort ())
  {
    *valid = false;
    delete  trainer;    trainer = NULL;
  }
  else
  {
    classes = new MLClassList (*trainer->MLClasses ());
    PopulateCSharpClassList ();
    *valid = true;
    classifier = new Classifier2 (trainer, *runLog);
  }

  //ofstream o ("C:\\Temp\\DarkSpots.txt");
  //PostLarvaeFVPrintReport (o);

  GC::Collect ();
}  /* BuildTrainingModel */




UmiPredictionList^   UmiTrainingModel2::BinaryProbailitiesForClass (UmiClass^  leftClass)
{
  if  (!crossProbTable)
    return  nullptr;

  int  idx = classList->LookUpIndex (leftClass);
  if  (idx < 0)
    return  nullptr;

  if  (idx >= crossProbTableNumClasses)
  {
    (*runLog).Level (-1) << "\n"
                         <<  "UmiTrainingModel2::BinaryProbailitiesForClass   ***ERROR***" << "\n" 
                         <<  "                   idx[" << idx << "] >= crossProbTableNumClasses[" << crossProbTableNumClasses << "]" << "\n"
                         << std::endl;
    return  nullptr;
  }

  if  (crossProbTableNumClasses != classList->Count)
    return  nullptr;

  UmiPredictionList^  predictions = gcnew UmiPredictionList ();
  for  (int x = 0;  x < crossProbTableNumClasses;  x++)
  {
    predictions->Add (gcnew UmiPrediction (classList[x], 1, crossProbTable[idx][x]));
  }

  return  predictions;
}  /* BinaryProbailitiesForClass */




UmiPredictionList^   UmiTrainingModel2::PredictProbabilities (UmiFeatureVector^  umiFeatureVector)
{
  LarcosFeatureVectorPtr  unKnownExample = new CounterFeatureVector (*(umiFeatureVector->Features ()));
  int  numClasses = classes->QueueSize ();

  try
  {
    classifier->ProbabilitiesByClass (*classes, unKnownExample, votes, probabilities);
  }
  catch  (std::exception e)
  {
    KKStr  errMsg = "Exception occurred calling 'ProbabilitiesByClass'.\n\n";
    errMsg << e.what ();
    System::Windows::Forms::MessageBox::Show (UmiKKStr::KKStrToSystenStr (errMsg), "UmiTrainingModel2::PredictProbabilities");
    delete  unKnownExample;  unKnownExample = NULL;
    return nullptr;
  }
  catch  (...)
  {
    System::Windows::Forms::MessageBox::Show ("Exception occurred calling 'ProbabilitiesByClass'."
                                              "UmiTrainingModel2::PredictProbabilities"
                                             );
    delete  unKnownExample;  unKnownExample = NULL;
    return nullptr;
  }

  delete  unKnownExample;  unKnownExample = NULL;
   

  try
  {
    classifier->RetrieveCrossProbTable (*classes, crossProbTable);
  }
  catch  (...)
  {
    System::Windows::Forms::MessageBox::Show ("Exception occurred calling 'RetrieveCrossProbTable'."
                                              "UmiTrainingModel2::PredictProbabilities"
                                             );
    return nullptr;
  }

  UmiPredictionList^  predictions = gcnew UmiPredictionList (numClasses);
  for  (int idx = 0;  idx < numClasses;  idx++)
    predictions->Add (gcnew UmiPrediction (classList[idx], votes[idx], probabilities[idx]));

  if  (classifier->SelectionMethod () == SVM_SelectionMethod::Voting)
     predictions->SortByVotingHighToLow ();
  else
     predictions->SortByProbabilityHighToLow ();

  return  predictions;
}  /* PredictProbabilities */





UmiPrediction^  UmiTrainingModel2::PredictClass (UmiFeatureVector^  example)
{
  FeatureVector  testExample (*(example->Features ()));  // We need a local copy; because the ClassifyImage function is going 
                                                         // to normalize the data.

  MLClassPtr  class1Pred     = NULL;
  MLClassPtr  class2Pred     = NULL;
  int         class1Votes    = -1;
  int         class2Votes    = -1;
  double      class1Prob     = -1.0f;
  double      class2Prob     = -1.0f;
  double      knownClassProb = -1.0f;
  int         numOfWinners   = -1;
  double      breakTie       = -1.0f;
  try
  {
    classifier->ClassifyAExample (testExample, 
                                  class1Pred, 
                                  class2Pred,
                                  class1Votes,
                                  class2Votes,
                                  knownClassProb,
                                  class1Prob,
                                  class2Prob, 
                                  numOfWinners,
                                  breakTie
                                 );
  }
  catch  (Exception^ e)
  {
    System::Windows::Forms::MessageBox::Show ("Exception occurred when calling 'Classifier::ClassifyAImage'  in  'UmiTrainingModel2::PredictClass'" + "\n\n" +
                                              e->ToString (),
                                              "UmiTrainingModel2::PredictClass"
                                             );
    class1Pred = nullptr;
  }

  UmiPrediction^  prediction = gcnew UmiPrediction (classList->LookUpByUnmanagedClass (class1Pred), class1Votes, class1Prob);

  return  prediction;
}  /* PredictClass */




void  UmiTrainingModel2::PredictClass (UmiFeatureVector^  featureVector,
                                       UmiPrediction^     prediction1,
                                       UmiPrediction^     prediction2
                                      )
{
  if  (!classifier)
    return;

  MLClassPtr  class1Pred     = NULL;
  MLClassPtr  class2Pred     = NULL;
  int         class1Votes    = -1;
  int         class2Votes    = -1;
  double      class1Prob     = -1.0f;
  double      class2Prob     = -1.0f;
  double      knownClassProb = -1.0f;
  int         numOfWinners   = -1;
  double      breakTie       = -1.0f;
  try
  {
    classifier->ClassifyAExample (*(featureVector->UnManagedClass ()),
                                  class1Pred, 
                                  class2Pred,
                                  class1Votes,
                                  class2Votes,
                                  knownClassProb,
                                  class1Prob,
                                  class2Prob, 
                                  numOfWinners,
                                  breakTie
                                 );
  }
  catch  (Exception^ e)
  {
    System::Windows::Forms::MessageBox::Show ("Exception occurred when calling 'Classifier::ClassifyAImage'  in  'UmiTrainingModel2::PredictClass'" + "\n\n" +
                                              e->ToString (),
                                              "UmiTrainingModel2::PredictClass"
                                             );
    class1Pred = nullptr;
  }


  if  (class1Pred != nullptr)
  {
    prediction1->MLClass     = UmiClassList::GetUniqueClass (class1Pred);
    prediction1->Probability = class1Prob;
    prediction1->Votes       = class1Votes;
  
    prediction2->MLClass     = UmiClassList::GetUniqueClass (class2Pred);
    prediction2->Probability = class2Prob;
    prediction2->Votes       = class2Votes;
  }

  return;
}   /* PredictClass */




void  UmiTrainingModel2::PredictClass (System::String^   imageFileName,
                                       System::Array^    raster,
                                       UmiPrediction^    prediction1,
                                       UmiPrediction^    prediction2
                                      )
{
  if  (!classifier)
    return;
  if  (!fvProducer)
    fvProducer = factoryFVProducer->ManufactureInstance (*runLog);
  UmiRaster^  r = gcnew UmiRaster (raster);
  r->FileName = imageFileName;
  MLClassPtr  unknownClass = classes->GetUnKnownClass ();
  FeatureVectorPtr  fvp = fvProducer->ComputeFeatureVector (*(r->UnmanagedClass ()), unknownClass, NULL, 1.0f, *runLog);
  UmiFeatureVector^  fv = gcnew UmiFeatureVector (fvp);
  PredictClass (fv, prediction1, prediction2);
  return;
}  /* PredictClass */




UmiPredictionList^   UmiTrainingModel2::PredictProbabilities (System::String^  imageFileName,
                                                              UmiRasterList^   intermediateImages
                                                             )
{
  bool  sucessful = true;
  if  (!fvProducer)
    fvProducer = factoryFVProducer->ManufactureInstance (*runLog);

  RasterListPtr  tempIntermediateImages = NULL;
  if  (intermediateImages != nullptr)
    tempIntermediateImages = new RasterList (true);
  MLClassPtr  unKnownClass = classes->IdxToPtr (0);
  FeatureVectorPtr  fv = NULL;
  try
  {
    fv = fvProducer->ComputeFeatureVectorFromImage (UmiKKStr::SystemStringToKKStr (imageFileName), unKnownClass, tempIntermediateImages, *runLog);
  }
  catch  (Exception^ e)
  {
    fv = NULL;
    sucessful = false;
    System::Windows::Forms::MessageBox::Show ("Exception occurred when Constructing a 'CounterFeatureVector' object" + "\n\n" +
                                              e->ToString (),
                                              "UmiTrainingModel2::PredictProbabilities"
                                             );
  }
  catch  (std::exception&  e2)
  {
    fv = NULL;
    sucessful = false;
    KKStr  msg = e2.what ();
    String^  exceptionMsg = UmiKKStr::KKStrToSystenStr (msg);
    System::Windows::Forms::MessageBox::Show ("Exception occurred calling 'ComputeFeatureVectorFromImage'.\n\n" + exceptionMsg, "TrainingModel2::PredictProbabilities");
  }
  catch  (...)
  {
    fv = NULL;
    sucessful = false;
    System::Windows::Forms::MessageBox::Show ("Exception occurred calling 'ComputeFeatureVectorFromImage'.", "TrainingModel2::PredictProbabilities");
  }

  UmiRasterList::CopyOverIntermediateImages (tempIntermediateImages, intermediateImages);

  if  (!sucessful)
  {
    delete  fv; 
    fv = NULL;
    return  nullptr;
  }

  UmiFeatureVector^  pfv = gcnew UmiFeatureVector (fv);  // 'pfv' will take ownership of 'fv'

  UmiPredictionList^  predictions = nullptr;
  try
  {
    predictions = PredictProbabilities (pfv);
  }
  catch (Exception^ e)
  {
    System::Windows::Forms::MessageBox::Show ("Exception occurred calling 'PredictProbabilities'" + "\n\n" +  e->ToString (),
                                              "UmiTrainingModel2::PredictProbabilities"
                                             );
    predictions = nullptr;
  }
  catch  (std::exception&  e3)
  {
    fv = NULL;
    sucessful = false;
    KKStr  msg = e3.what ();
    String^  exceptionMsg = UmiKKStr::KKStrToSystenStr (msg);
    System::Windows::Forms::MessageBox::Show ("Exception occurred calling 'PredictProbabilities'.\n\n" + exceptionMsg, "TrainingModel2::PredictProbabilities");
  }
  catch  (...)
  {
    System::Windows::Forms::MessageBox::Show ("Exception occurred calling 'PredictProbabilities'.",
                                              "UmiTrainingModel2::PredictProbabilities"
                                             );
    predictions = nullptr;
  }

  delete  pfv;
  pfv = nullptr;

  return  predictions;
}  /* PredictProbabilities */



array<String^>^  UmiTrainingModel2::SupportVectorNames (UmiClass^ c1,
                                                        UmiClass^ c2
                                                       )
{
  if  (!classifier)
    return nullptr;

  MLClassPtr  c1Scs = c1->UnmanagedImageClass ();
  MLClassPtr  c2Scs = c2->UnmanagedImageClass ();

  std::vector<KKStr> fvNames = classifier->SupportVectorNames (c1Scs, c2Scs);

  array<String^>^  results = gcnew array<String^> ((int)fvNames.size ());
  for  (kkuint32 zed = 0;  zed < fvNames.size ();  zed++)
    results[zed] = UmiKKStr::KKStrToSystenStr (fvNames[zed]);
    
  return  results;
}



array<CounterManaged::ProbNamePair^>^   
                     UmiTrainingModel2::FindWorstSupportVectors (UmiFeatureVector^  umiFeatureVector,
                                                                 int                numToFind,
                                                                 UmiClass^          c1,
                                                                 UmiClass^          c2
                                                                )
{
  if  (!classifier)
    return nullptr;

  MLClassPtr  c1Scs = c1->UnmanagedImageClass ();
  MLClassPtr  c2Scs = c2->UnmanagedImageClass ();

  // Will make duplicate of feature vector Because the Classifier will normalize the data.
  FeatureVectorPtr  dupFV = new FeatureVector (*umiFeatureVector->UnManagedClass ());

  std::vector<KKMLL::ProbNamePair> worstExamples 
    = classifier->FindWorstSupportVectors (dupFV, 
                                           numToFind, 
                                           c1Scs, 
                                           c2Scs
                                          );

  array<CounterManaged::ProbNamePair^>^  results = gcnew array<CounterManaged::ProbNamePair^> ((int)worstExamples.size ());
  for  (kkuint32 zed = 0;  zed < worstExamples.size ();  zed++)
    results[zed] = gcnew CounterManaged::ProbNamePair (worstExamples[zed].name, worstExamples[zed].probability);

  delete  dupFV;  dupFV = NULL;

  return  results;
}  /* FindWorstSupportVectors */





array<CounterManaged::ProbNamePair^>^   
                     UmiTrainingModel2::FindWorstSupportVectors2 (UmiFeatureVector^  umiFeatureVector,
                                                                  int                numToFind,
                                                                  UmiClass^          c1,
                                                                  UmiClass^          c2
                                                                 )
{
  if  (!classifier)
    return nullptr;

  MLClassPtr  c1Scs = c1->UnmanagedImageClass ();
  MLClassPtr  c2Scs = c2->UnmanagedImageClass ();

  // Will make duplicate of feature vector Because the Classifier will normalize the data.
  FeatureVectorPtr  dupFV = new FeatureVector (*umiFeatureVector->UnManagedClass ());

  std::vector<KKMLL::ProbNamePair> worstExamples 
    = classifier->FindWorstSupportVectors2 (dupFV, 
                                            numToFind, 
                                            c1Scs, 
                                            c2Scs
                                           );

  array<CounterManaged::ProbNamePair^>^  results = gcnew array<CounterManaged::ProbNamePair^> ((int)worstExamples.size ());
  for  (kkuint32 zed = 0;  zed < worstExamples.size ();  zed++)
    results[zed] = gcnew CounterManaged::ProbNamePair (worstExamples[zed].name, worstExamples[zed].probability);

  delete  dupFV;  dupFV = NULL;

  return  results;
}  /* FindWorstSupportVectors2 */





Bitmap^  UmiTrainingModel2::BuildBitmapFromRasterData (uchar**  r,
                                                       int      height,
                                                       int      width
                                                      )
{
  int  rowFirst = 99999;
  int  rowLast  = -1;
  int  colFirst = 99999;
  int  colLast  = -1;

  Bitmap^  image = gcnew Bitmap (width, height);
    

  // GDI+ return format is BGR, NOT RGB. 
  BitmapData^ bmData = image->LockBits (System::Drawing::Rectangle (0, 0, width, height),
                                        ImageLockMode::ReadWrite,
                                        PixelFormat::Format24bppRgb
                                       );
  int stride = bmData->Stride;
  System::IntPtr Scan0 = bmData->Scan0;

  {
    int  nOffset = stride - width * 3;
    int  bytesPerRow = width * 3 + nOffset;
    int  col = 0;
    int  row = 0;
    byte pixelValue = 0;
    byte lastPixelValue = 0;


    if  (colorValues == nullptr)
    {
      colorValues = gcnew array<Pen^>(256);
      for (int x = 0; x < 256; x++)
      {
        int y = 255 - x;
        colorValues[x] = gcnew Pen (Color::FromArgb(y, y, y));
      }
    }

    Pen^ curPen = colorValues[pixelValue];
    byte  red   = curPen->Color.R;
    byte  green = curPen->Color.G;
    byte  blue  = curPen->Color.B;

    byte* ptr = (byte*)(void*)Scan0;

    for  (row = 0;  row < height;  row++)
    {
      uchar*  datRow = r[row];
          
      for (col = 0;  col < width;  col++)
      {
        pixelValue = datRow[col];
        if  (pixelValue != lastPixelValue)
        {
          lastPixelValue = pixelValue;
          curPen = colorValues[lastPixelValue];
          red   = curPen->Color.R;
          green = curPen->Color.G;
          blue  = curPen->Color.B;
        }

        ptr[0] = blue;  ptr++;
        ptr[0] = green; ptr++;
        ptr[0] = red;   ptr++;

        if  (pixelValue < 255)
        {
          // We are looking at a foreground pixel.
          if  (row < rowFirst )
            rowFirst = row;
          rowLast = row;

          if  (col < colFirst)
            colFirst = col;

          if  (col > colLast)
            colLast = col;
        }
      }
          
      ptr += nOffset;
    }
  }  

  image->UnlockBits (bmData);
  return  image;
}  /* BuildBitmapFromRasterData */




UmiPredictionList^   UmiTrainingModel2::PredictProbabilities (System::String^  imageFileName,
                                                              System::Array^   raster
                                                             )
{
  if  (!classifier)
    return nullptr;

  UmiFeatureVector^  fv = gcnew UmiFeatureVector (raster, imageFileName, umiRunLog);
  UmiPredictionList^  predictions = this->PredictProbabilities (fv);
  delete  fv;  fv = nullptr;
  return  predictions;
}  /* PredictProbabilities */




String^   UmiTrainingModel2::RunLogFileName::get ()
{
  if  (runLog != NULL)
    return  UmiKKStr::KKStrToSystenStr (runLog->FileName ());
  else
    return  String::Empty;
}  /* RunLogFileName */






void  UmiTrainingModel2::CancelLoad ()   // Sets cancel flag to terminate loading of training model.
{
  *cancelFlag = true;
}



String^  UmiTrainingModel2::DirectoryPathForClass (UmiClass^  mlClass)
{
  LarcosTrainingConfigurationConstPtr  configToUse = GetConfigToUse ();
  if  (configToUse == NULL)
    return nullptr;

  KKStr  dirPath = configToUse->DirectoryPathForClass (mlClass->UnmanagedImageClass ());
  return  UmiKKStr::KKStrToSystenStr (dirPath);
}  /* DirectoryPathForClass */




array<String^>^  UmiTrainingModel2::GetListOfTrainingModels ()
{
  KKStr searchSpec = osAddSlash (CounterVariables::TrainingModelsDir ()) + "*.cfg";
  KKStrListPtr   fileNames = osGetListOfFiles (searchSpec);
  if  (fileNames == NULL)
    return  nullptr;

  int  count = fileNames->QueueSize ();  
  if  (count < 1)
  {
    delete  fileNames;
    return nullptr;
  }

  cli::array<String^>^  configFiles 
    = gcnew cli::array<String^> (count);

  KKStrList::iterator  idx;
  int x;
  for  (idx = fileNames->begin (), x = 0;  idx != fileNames->end ();  idx++, x++)
  {
    KKStrPtr s = *idx;
    configFiles[x] = gcnew String (s->Str ());
  }

  delete fileNames;
  fileNames = NULL;

  return  configFiles;
}  /* GetListOfTrainingModels */






kkuint32  UmiTrainingModel2::NumHierarchialLevels::get ()
{
  LarcosTrainingConfigurationConstPtr  configToUse = GetConfigToUse ();
  if  (configToUse)
    return  configToUse->NumHierarchialLevels ();
  else
    return 0;
}




int  UmiTrainingModel2::ImagesPerClass::get ()
{
  TrainingConfiguration2ConstPtr tConfig = trainer->Config ();
  if  (tConfig)
    return  tConfig->ImagesPerClass ();
  else
    return -1;
}



String^  UmiTrainingModel2::ParameterStr::get ()
{
  KKStr  parmStr;

  if  (trainer)
  {
    ModelParamPtr  parameters = trainer->Parameters ();
    if  (parameters)
      parmStr = parameters->ToCmdLineStr ();

    else
    {
      TrainingConfiguration2ConstPtr tConfig = trainer->Config ();
      if  (tConfig)
      {
        ModelParamPtr  parameters = tConfig->ModelParameters ();
        if  (parameters)
          parmStr = parameters->ToCmdLineStr ();
      }
    }
  }

  if  (parmStr.Empty ())
  {
    LarcosTrainingConfigurationConstPtr  configToUse = GetConfigToUse ();
    if  ((configToUse)  &&  (configToUse->ModelParameters ()))
      parmStr = configToUse->ModelParameters ()->ToCmdLineStr ();
  }

  return  UmiKKStr::KKStrToSystenStr (parmStr);
}  /* SvmParamStr::get () */




String^  UmiTrainingModel2::ConfigFileName::get ()
{
  LarcosTrainingConfigurationConstPtr  configToUse = GetConfigToUse ();
  if  (configToUse)
    return UmiKKStr::KKStrToSystenStr (configToUse->FileName ());
  else
    return String::Empty;
}



void  UmiTrainingModel2::AddClass (UmiClass^  newClass)
{
  CounterTrainingConfigurationPtr  configToUse = GetConfigToUse ();
  if  (configToUse == NULL)
    throw gcnew Exception ("Could not retrieve the configuration file.");

  MLClassPtr  c = newClass->UnmanagedImageClass ();

  KKStr  dirPath =  (configToUse->DirectoryPathForClass (c));
  if  (!dirPath.Empty ())
    throw gcnew Exception ("Class already in Training Model.");

  // kak 2010-05-09
  // We need to make sure the directory for this class exists.
  KKStr  classDirName = osAddSlash (configToUse->RootDirExpanded ()) + c->Name ();
  osCreateDirectoryPath (classDirName);

  configToUse->AddATrainingClass (c);
}   /* AddClass */





void  UmiTrainingModel2::AddImageToTrainingLibray (String^     imageFileName,
                                                   UmiRaster^  raster, 
                                                   UmiClass^   mlClass,
                                                   bool        onLine       /**< If set to true; will prompt user if they want to add new class if not part of Training Model. */
                                                  )
{
  if  (raster == nullptr)
    throw gcnew Exception ("No raster image provided;   \"raster == nullptr;\"  in method \"UmiTrainingModel2::AddImageToTrainingLibray\"");

  if  (mlClass == nullptr)
    throw gcnew Exception ("No class provided;   \"mlClass == nullptr;\"  in method \"UmiTrainingModel2::AddImageToTrainingLibray\"");

  LarcosTrainingConfigurationConstPtr  configToUse = GetConfigToUse ();
  if  (!configToUse)
    throw  gcnew Exception  ("No defined Configuration file available for ModelName[" + modelName + "]");

  if  (!this->IncludesClass (mlClass))
  {
    if  (onLine)
    {
      DialogResult dr = MessageBox::Show 
                            ("Class[" + mlClass->Name + "] not part of model[" + ModelName + "]" + "\n\n" +
                             "Do you want to add this Class to training model?",
                             "Add to Training Library",
                             MessageBoxButtons::YesNo
                            );
      if  (dr != DialogResult::Yes)
        return;
      AddClass (mlClass);
      SaveConfiguration ();
    }
    else
    {
      throw gcnew Exception ("Class[" + mlClass->Name + "] Not part of training model[" + ModelName + "]");
    }
  }

  KKStr  dirPathToCopyTo = configToUse->DirectoryPathForClass (mlClass->UnmanagedImageClass ());
  if  (dirPathToCopyTo.Empty ())
    throw gcnew Exception ("No Directory Path defined for Class[" + mlClass->Name + "]  Model[" + modelName + "]");

  if  (!osValidDirectory (dirPathToCopyTo))
    throw gcnew Exception ("UmiTrainingModel2::AddImageToTrainingLibray    Directory[" + UmiKKStr::KKStrToSystenStr (dirPathToCopyTo) + "] does not exist.");

  // make sure that target directory exists.

  KKStr  rootName = osGetRootName (UmiKKStr::SystemStringToKKStr (imageFileName));
  KKStr  destFileName = osAddSlash (dirPathToCopyTo) + rootName + ".bmp";

  if  (osFileExists (destFileName))
  {
    System::String^  errMsg = "Image[" + UmiKKStr::KKStrToSystenStr (rootName) + "] already exists in Class[" + mlClass->Name + "]";
    if  (!onLine)
    {
      throw gcnew Exception (errMsg);
    }
    else
    {
      MessageBox::Show (errMsg, "Already in Training Library", MessageBoxButtons::OK);
      return;
    }
  }

  try
  {
    raster->Save (UmiKKStr::KKStrToSystenStr (destFileName));
  }
  catch  (KKStr  errMsg)
  {
    throw gcnew Exception (UmiKKStr::KKStrToSystenStr (errMsg));
  }
}  /* AddImageToTrainingLibray */
 



CounterTrainingConfigurationPtr  UmiTrainingModel2::GetConfigToUse ()
{
  CounterTrainingConfigurationPtr  configToUse = NULL;
  if  (config)
    configToUse = config;

  if  (!configToUse)
  {
    config  = new CounterTrainingConfiguration ();
    config->Load (UmiKKStr::SystemStringToKKStr (modelName), 
                  NULL,   // initialOperatingParameters
                  false,  // false = Do Not Validate Directories.
                  *runLog
                 );
    // Don't need to delete 'fd'  'FileDesc' instances are kept in memory and shared when identical.
    configToUse = config;
  }
  return  configToUse;
}  /* GetConfigToUse */




void  UmiTrainingModel2::ErrorMsgsClear ()
{
  if  (errorMsgs == nullptr)
    errorMsgs = gcnew List<String^> ();
  else
    errorMsgs->Clear ();
}



void  UmiTrainingModel2::ErrorMsgsAdd (String^  errorMsg)
{
  if  (errorMsgs == nullptr)
    errorMsgs = gcnew List<String^> ();

  errorMsgs->Add (errorMsg);
}



void  UmiTrainingModel2::ErrorMsgsAdd (const VectorKKStr&  _errorMsgs)
{
  VectorKKStr::const_iterator  idx;
  for  (idx = _errorMsgs.begin ();  idx != _errorMsgs.end ();  idx++)
  {
    String^  msg = UmiKKStr::KKStrToSystenStr (*idx);
    ErrorMsgsAdd (msg);
  }
}  /* ErrorMsgsAdd */
