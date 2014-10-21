#if  !defined(_LarcosTrainingModelManaged_)
#define  _LarcosTrainingModelManaged_


using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Diagnostics;
using namespace System::Collections::Generic;
using namespace System::Threading;

#include "LarcosTrainingModel.h"
using  namespace LarcosCounterUnManaged;


#include "UmiRunLog.h"



namespace LarcosCounterManaged 
{
  public  ref  class  LarcosTrainingModelManaged
  {
  public:
    LarcosTrainingModelManaged (LarcosTrainingModelPtr  _larcosTraningModel);  /**< Takes ownership of '_larcosTraningModel'. */

    property  String^    Subject           {String^    get ();}
    property  String^    Size              {String^    get ();}
    property  String^    WaterQuality      {String^    get ();}
    property  String^    TrainingModelName {String^    get ();}
 
    LarcosTrainingModelPtr  UnManaged ();

  private:
    LarcosTrainingModelPtr  larcosTrainingModel;
  };  /* LarcosTrainingModelManaged */




  public  ref  class  LarcosTrainingModelListManaged
  {
  public:
    LarcosTrainingModelListManaged ();

    LarcosTrainingModelListManaged (LarcosTrainingModelListPtr  _larcosTrainingModels);  /**< Takes ownership of the contents of 'models'. */

    !LarcosTrainingModelListManaged ();

    ~LarcosTrainingModelListManaged ();

    array<String^>^  GetListOfSubjects ();

    array<String^>^  GetListOfSizes (String^ subject);

    array<String^>^  GetListOfWaterQualities (String^ subject, String^ size);

    bool  InList (String^ subject, String^ size, String^ waterQuality);

    static
      LarcosTrainingModelListManaged^  BuildFromTrainigModelDir ();

  private:
    LarcosTrainingModelListPtr  larcosTrainingModels;
  };  /* LarcosTrainingModelListManaged */

}  /* LarcosCounterManaged */


#endif
