#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Diagnostics;
using namespace System::IO;

#include "MLClass.h"
using namespace  KKMLL;

#include "CounterTrainingConfiguration.h"
using namespace  CounterUnManaged;

#include "UmiFeatureVectorList.h"
#include "UmiOperatingParameters.h"
#include "UmiTrainingClass.h"
#include "UmiRunLog.h"


namespace CounterManaged 
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

    CounterTrainingConfigurationPtr Config ()  {return config;};

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

    UmiFeatureVectorList^     LoadFeatureDataFromTrainingLibraries (bool^ changesMadeToTrainingLibraries,
                                                                    UmiRunLog^ _log
                                                                   );

    String^                   ModelName ();

    kkuint32                  NumHierarchialLevels ();  /**< returns back the number of hierarchical levels there.  */

    String^                   RootDirExpanded ();

    void                      Save (String^  configFileName);

    void                      SetOperatingParms (UmiOperatingParameters^  _operatingParms);

    UmiTrainingClassList^     TrainingClasses ();

    Boolean                   Valid ();

  private:
    bool*                           cancelFlag;
    CounterTrainingConfigurationPtr  config;
    MLClassListPtr                  classes;
    String^                         loadLogStream;
    UmiRunLog^                      log;
    Boolean^                        valid;
  };  /* UmiTrainingConfiguration */
}
