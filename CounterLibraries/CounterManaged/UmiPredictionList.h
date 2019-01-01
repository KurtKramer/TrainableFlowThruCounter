#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Collections::Generic;
using namespace System::Diagnostics;


#include  "UmiPrediction.h"

namespace CounterManaged
{
  public ref  class UmiPredictionList:  List<UmiPrediction^>
  {
  public:
    UmiPredictionList ();

    UmiPredictionList (int  size);

    void  SortByProbabilityHighToLow ();
    void  SortByVotingHighToLow ();

  private:
    ref class SortPredictionsByProbability;
    ref class SortPredictionsByVotes;
  };
}