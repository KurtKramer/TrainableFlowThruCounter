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


//#include "MLClass.h"

//#include "UmiKKStr.h"


#include "UmiClass.h"
#include "UmiPrediction.h"

using namespace CounterManaged;


using namespace System;
using namespace System::Threading;
using namespace System::Windows::Forms;

UmiPrediction::UmiPrediction  (UmiClass^ _mlClass,
                               int       _votes,
                               double    _probability
                              ):
      mlClass  (_mlClass),
      probability (_probability),
      votes       (_votes)
{}



String^  UmiPrediction::ClassName::get ()  
{
  if  (mlClass == nullptr)
    return String::Empty;
  else
    return  mlClass->Name;
}
    

void  UmiPrediction::Refresh (UmiPrediction^  p)
{
  mlClass     = p->MLClass;
  probability = p->Probability;
  votes       = p->Votes;
}
