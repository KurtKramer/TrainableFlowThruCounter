#pragma once

using namespace System;
using namespace System::Drawing;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Diagnostics;


#include "Classifier2.h"
#include "FactoryFVProducer.h"
#include "FeatureVectorProducer.h"
#include "TrainingProcess2.h"

#include "LarcosTrainingConfiguration.h"

#include "UmiClass.h"
#include "UmiClassList.h"
#include "UmiFeatureVector.h"
#include "UmiOperatingParameters.h"
#include "UmiPrediction.h"
#include "UmiPredictionList.h"
#include "UmiRunLog.h"
#include "UmiTrainingConfiguration.h"




namespace LarcosCounterManaged 
{
  public ref class  ProbNamePair
  {
  public:
    ProbNamePair (String^  _name,
                  double   _probability
                 ):
        name (_name),  probability (_probability)
        {}

    ProbNamePair (const KKStr&  _name,
                  double        _probability
                 ):
        name (nullptr),  probability (0.0f)
    {
      name = UmiKKStr::KKStrToSystenStr (_name);
      probability = _probability;
    }

    String^  name;
    double   probability;
  };



	public ref class  UmiTrainingModel2 
	{
	public:
    UmiTrainingModel2 (UmiRunLog^       _umiRunLog,
                       System::String^  _modelName
                      );

    UmiTrainingModel2 (UmiRunLog^                  _umiRunLog,
                       System::IO::DirectoryInfo^  _directoryInfo
                      );

    UmiTrainingModel2 (UmiRunLog^                 _umiRunLog,
                       UmiTrainingConfiguration^  _config
                      );

  protected:
    !UmiTrainingModel2 ();

  private:
    ~UmiTrainingModel2 ();

    void  CleanUpUnmanagedResources ();


  public:
    property  System::DateTime  BuildDateTime            {System::DateTime  get ();}
    property  bool              CancelFlag               {bool              get ()  {return *cancelFlag;}}
    property  String^           ConfigFileName           {String^           get ();}
    property  List<String^>^    ErrorMsgs                {List<String^>^    get ()  {return  errorMsgs;}}
    property  int               ImagesPerClass           {int               get ();}
    property  String^           ModelName                {String^           get ()  {return  modelName;}}
    property  kkuint32          NumHierarchialLevels     {kkuint32          get ();}     // returns back the number of hierarchail levels there
    property  String^           ParameterStr             {String^           get ();}
    property  String^           RootDirExpanded          {String^           get ();}
    property  String^           RunLogFileName           {String^           get ();}
    property  bool              Valid                    {bool              get ()  {return  *valid;}}


    void  AddClass (UmiClass^  newClass);

    void  AddImageToTrainingLibray (String^     imageFileName,
                                    UmiRaster^  raster, 
                                    UmiClass^   mlClass,
                                    bool        onLine  
                                   );

    void  BuildTrainingModel (UmiFeatureVectorList^  umiTrainingData);


    void  CancelLoad ();   // Sets cancel flag to terminate loading of training model.

    UmiClassList^  ImageClasses ();  // Will return a list of classes that belong to this model. 
                                       // It will be created from "classList".  So the the caller 
                                       // can do with it as they want.

    String^  DirectoryPathForClass (UmiClass^  mlClass);


    static
    array<String^>^  GetListOfTrainingModels ();

    UmiOperatingParameters^  GetOperatingParameters ();

    bool  IncludesClass (UmiClass^  mlClass); 

    String^  LastRunLogTextLine ();

    void  LoadExistingTrainedModel ();

    void  LoadExistingModelOtherwiseBuild ();

    void  LoadTrainigLibrary (bool  forceRebuild);


    void  LoadTrainingModelForGivenLevel (kkuint32 level);

    UmiPrediction^  PredictClass (UmiFeatureVector^  example);

    void  PredictClass (UmiFeatureVector^  featureVector,
                        UmiPrediction^     prediction1,
                        UmiPrediction^     prediction2
                       );

    void  PredictClass (System::String^  imageFileName,
                        System::Array^   raster,
                        UmiPrediction^   prediction1,
                        UmiPrediction^   prediction2
                       );


    // This call asasumes that here wea as sucessfull call to PredictProbabilities before.
    UmiPredictionList^   BinaryProbailitiesForClass (UmiClass^  leftClass);

    array<LarcosCounterManaged::ProbNamePair^>^  
                           FindWorstSupportVectors (UmiFeatureVector^  umiFeatureVector,
                                                    int                numToFind,
                                                    UmiClass^          c1,
                                                    UmiClass^          c2
                                                   );

    array<LarcosCounterManaged::ProbNamePair^>^  
                           FindWorstSupportVectors2 (UmiFeatureVector^  umiFeatureVector,
                                                     int                numToFind,
                                                     UmiClass^          c1,
                                                     UmiClass^          c2
                                                    );

    UmiPredictionList^   PredictProbabilities (System::String^  imageFileName,
                                               System::Array^   raster
                                              );

    UmiPredictionList^   PredictProbabilities (System::String^  imageFileName,
                                               UmiRasterList^   intermediateImages
                                              );

    UmiPredictionList^   PredictProbabilities (UmiFeatureVector^  umiFeatureVector);


    void  SaveConfiguration ();

    array<String^>^   SupportVectorNames (UmiClass^ c1,
                                          UmiClass^ c2
                                         );




	private:

    Bitmap^  BuildBitmapFromRasterData (uchar**  r,
                                        int      height,
                                        int      width
                                       );

    void     CreateRunLog ();

    void     ErrorMsgsClear ();
    void     ErrorMsgsAdd (String^ errorMsg);
    void     ErrorMsgsAdd (const VectorKKStr&  _errorMsgs);


    TrainingConfiguration2Ptr   GetConfigToUse ();

    void  PopulateCSharpClassList ();

    bool*                           cancelFlag;
    Classifier2Ptr                  classifier;
    MLClassListPtr                  classes;
    LarcosTrainingConfigurationPtr  config;
    double**                        crossProbTable;
    int                             crossProbTableNumClasses;
    List<String^>^                  errorMsgs;     /**< Error messages for later recall are added to this list. */
    FactoryFVProducerPtr            factoryFVProducer;
    FeatureVectorProducerPtr        fvProducer;
    bool                            loadTrainingLibraryRunning;
    System::String^                 modelName;
    UmiRunLog^                      umiRunLog;
    double*                         probabilities;
    RunLogPtr                       runLog;
    String^                         runLogFileName;
    int                             runLogLastLineNum;
    KKStrPtr                        statusMsg;
    TrainingProcess2Ptr             trainer;
    bool*                           valid;     // True if Training Library Valid
    int*                            votes;

    UmiClassList^                   classList;
    array<Pen^>^                    colorValues;
	};
}
