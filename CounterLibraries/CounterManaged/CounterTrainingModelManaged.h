#if  !defined(_COUNTERTRAININGNODEL_)
#define  _COUNTERTRAININGNODEL_


using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Diagnostics;
using namespace System::Collections::Generic;
using namespace System::Threading;

#include "CounterTrainingModel.h"
using  namespace CounterUnManaged;


#include "UmiRunLog.h"



namespace CounterManaged 
{
  public  ref  class  CounterTrainingModelManaged
  {
  public:
    CounterTrainingModelManaged (CounterTrainingModelPtr  _counterTrainingModel);  /**< Takes ownership of '_counterTrainingModel'. */

    property  String^    Subject           {String^    get ();}
    property  String^    Size              {String^    get ();}
    property  String^    WaterQuality      {String^    get ();}
    property  String^    TrainingModelName {String^    get ();}
 
    CounterTrainingModelPtr  UnManaged ();

  private:
    CounterTrainingModelPtr  counterTrainingModel;
  };  /* CounterTrainingModelManaged */




  public  ref  class  CounterTrainingModelListManaged
  {
  public:
    CounterTrainingModelListManaged ();

    CounterTrainingModelListManaged (CounterTrainingModelListPtr  _counterTrainingModels);  /**< Takes ownership of the contents of 'models'. */

    !CounterTrainingModelListManaged ();

    ~CounterTrainingModelListManaged ();

    array<String^>^  GetListOfSubjects ();

    array<String^>^  GetListOfSizes (String^ subject);

    array<String^>^  GetListOfWaterQualities (String^ subject, String^ size);

    bool  InList (String^ subject, String^ size, String^ waterQuality);

    static
      CounterTrainingModelListManaged^  BuildFromTrainigModelDir ();

  private:
    CounterTrainingModelListPtr  counterTrainingModels;
  };  /* CounterTrainingModelListManaged */

}  /* CounterManaged */


#endif
