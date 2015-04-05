#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Diagnostics;
using namespace System::IO;

#include "MLClass.h"
using namespace  KKMachineLearning;

#include "LarcosTrainingConfiguration.h"
using namespace  LarcosCounterUnManaged;

#include "UmiFeatureVectorList.h"
#include "UmiOperatingParameters.h"
#include "UmiTrainingClass.h"
#include "UmiRunLog.h"


namespace LarcosCounterManaged 
{
  public ref class UmiTrainingConfiguration
  {
  public:
    UmiTrainingConfiguration (UmiRunLog^  _log);

    UmiTrainingConfiguration (String^                  _configFileName,
                              UmiOperatingParameters^  _initialOperatingParameters,
                              UmiRunLog^               _log 
                             );

    UmiTrainingConfiguration (DirectoryInfo^           di,
                              UmiOperatingParameters^  initialOperatingParameters,
                              UmiRunLog^               log
                             );

  private:
    ~UmiTrainingConfiguration  ();

  protected:
    !UmiTrainingConfiguration  ();

  public:
    void                      CancelLoad ();  /**< Call this method to set the 'canclFlag' to true.  */

    LarcosTrainingConfigurationPtr Config ()  {return config;};

    String^                   FullFileName ();




    void                      AddATrainingClass (UmiTrainingClass^  _trainClass);

    static
    String^                   GetEffectiveConfigFileName (String^  configFileName);

    UmiClassList^             GetClassList ();

    String^                   GetSettingValue (String^ sectionName,
                                               String^ settingName
                                              );

    UmiOperatingParameters^   GetOperatingParameters ();

    Dictionary<String^,String^>^  GetSettingValues (String^ _sectionName);


    String^                   LoadLogStream ()  {return loadLogStream;}

    UmiFeatureVectorList^     LoadFeatureDataFromTrainingLibraries (bool^ changesMadeToTrainingLibraries);

    String^                   ModelName ();

    kkuint32                  NumHierarchialLevels ();  /**< returns back the number of hierarchical levels there.  */

    String^                   RootDirExpanded ();

    void                      Save (String^  configFileName);

    void                      SetOperatingParms (UmiOperatingParameters^  _operatingParms);

    UmiTrainingClassList^     TrainingClasses ();

    Boolean                   Valid ();

  private:
    bool*                           cancelFlag;
    LarcosTrainingConfigurationPtr  config;
    MLClassListPtr                  classes;
    String^                         loadLogStream;
    UmiRunLog^                      log;
    Boolean^                        valid;
  };  /* UmiTrainingConfiguration */
}
