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


#include "KKBaseTypes.h"
#include "..\\KKBase\\GoalKeeper.h"
#include "OSservices.h"

using namespace KKB;


#include "MLClass.h"

//#include "UmiKKStr.h"


#include "UmiPredictionList.h"


using namespace  LarcosCounterManaged;


using namespace System;
using namespace System::Threading;
using namespace System::Windows::Forms;


UmiPredictionList::UmiPredictionList (): List<UmiPrediction^> ()  {}

UmiPredictionList::UmiPredictionList (int  size): List<UmiPrediction^> (size) {}

    
ref class UmiPredictionList::SortPredictionsByProbability: public System::Collections::Generic::IComparer<UmiPrediction^>
{
public:
  virtual  int  Compare (UmiPrediction^ p1, UmiPrediction^ p2)
  {
    double  prob1 = p1->Probability;
    double  prob2 = p2->Probability;
        
    if  (prob1 < prob2)
      return 1;
    else if  (prob1 > prob2)
      return -1;
    else
      return 0;
  }
};  /* SortPredictionsByProbability */



ref class UmiPredictionList::SortPredictionsByVotes : public System::Collections::Generic::IComparer<UmiPrediction^>
{
public:
  virtual  int  Compare (UmiPrediction^ p1, UmiPrediction^ p2)
  {
    int     votes1 = p1->Votes;
    int     votes2 = p2->Votes;

    double  prob1 = p1->Probability;
    double  prob2 = p2->Probability;
        
    if  (votes1 < votes2)
      return 1;

    if  (votes1 > votes2)
      return -1;

    else if  (prob1 < prob2)
      return 1;

    else if  (prob1 > prob2)
      return -1;

    else
      return 0;
  }
};  /* SortPredictionsByVotes */



void  UmiPredictionList::SortByProbabilityHighToLow ()
{
  SortPredictionsByProbability^  comp = gcnew SortPredictionsByProbability ();
  Sort (comp);
}



void  UmiPredictionList::SortByVotingHighToLow ()
{
  SortPredictionsByVotes^  comp = gcnew SortPredictionsByVotes ();
  Sort (comp);
}
