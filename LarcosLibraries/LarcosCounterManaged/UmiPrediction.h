#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Collections::Generic;
using namespace System::Diagnostics;

#include  "UmiClass.h"

namespace LarcosCounterManaged
{
  public  ref  class  UmiPrediction
  {
  public:
    UmiPrediction  (UmiClass^  _mlClass,
                    int          _votes,
                    double       _probability
                   );

    property UmiClass^  MLClass      {UmiClass^   get ()  {return  mlClass;}      void  set (UmiClass^  _mlClass)     {mlClass     = _mlClass;}}
    property double     Probability  {double      get ()  {return  probability;}  void  set (double     _probability) {probability = _probability;}}
    property String^    ClassName    {String^     get ();}
    property int        Votes        {int         get ()  {return  votes;}        void  set (int        _votes)       {votes       = _votes;}}
   
    void  Refresh (UmiPrediction^  p);


  private:
    UmiClass^  mlClass;
    double     probability;
    int        votes;

  };
}
