#pragma once

using namespace System;
using namespace System::Drawing;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Diagnostics;


#include "TrainingClass.h"
using namespace KKB;
using namespace KKMachineLearning;

#include "UmiClass.h"
#include "UmiKKStr.h"
#include "UmiRunLog.h"


namespace LarcosCounterManaged {
  public  ref  class UmiTrainingClass
  {
  public:
    UmiTrainingClass (UmiClass^  _class,
                      String^    _directory
                     );

    UmiTrainingClass (TrainingClassPtr  _trainingClass);

    property  float      CountFactor {float      get ();   void  set (float      _countFactor);}
    property  String^    Directory   {String^    get ();   void  set (String^    _directory);}
    property  UmiClass^  MLClass     {UmiClass^  get ();   void  set (UmiClass^  _mlClass);}

    String^   ExpandedDirectory (String^  rootDir);


    TrainingClassPtr  UnManagedClass ();
  private:


    TrainingClassPtr  trainingClass;
  };



  public  ref  class  UmiTrainingClassList:  public  List<UmiTrainingClass^>
  {
  public:
    UmiTrainingClassList ();

    UmiTrainingClassList (TrainingClassListPtr  trainingClasses);

  private:
  };  /* UmiTrainingClassList */
}
