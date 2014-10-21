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
using namespace std;

#include "KKBaseTypes.h"
#include "..\KKBase\GoalKeeper.h"
using namespace KKB;

#include "UmiClass.h"
#include "UmiClassList.h"
#include "UmiClassStatistic.h"
using namespace LarcosCounterManaged;

using namespace System;
using namespace System::Windows::Forms;



UmiClassStatistic::UmiClassStatistic (const ClassStatistic&  classStat)
{
  mlClass = UmiClassList::GetUniqueClass (classStat.MLClass ());
  count      = classStat.Count ();
}



UmiClassStatistic::UmiClassStatistic (UmiClass^  _mlClass,
                                          kkuint32     _count
                                         )
{
  mlClass = _mlClass;
  count      = _count;
}



String^  UmiClassStatistic::ClassName::get ()
{
  if  (mlClass == nullptr)
    return String::Empty;

  return  mlClass->Name;
}



UmiClassStatisticList::UmiClassStatisticList ()
{
}



UmiClassStatisticList::UmiClassStatisticList (const ClassStatisticList&  stats)
{
  ClassStatisticList::const_iterator  idx;
  for  (idx = stats.begin ();  idx != stats.end ();  idx++)
  {
    Add (gcnew UmiClassStatistic (**idx));
  }
}



int  CompareUmiClassStatistic (UmiClassStatistic^ left,
                               UmiClassStatistic^ right
                              )
{
  if  (left == nullptr)
  {
    if  (right == nullptr)
      return 0;
    else
      return 1;
  }

  else if  (right == nullptr)
    return -1;

  else
    return  String::Compare(left->ClassName, right->ClassName);
}  /* CompareUmiClassStatistic*/




void  UmiClassStatisticList::SortByClassName ()
{
  Sort(gcnew Comparison<UmiClassStatistic^>(CompareUmiClassStatistic));
}  /* SortByClassName */




void  UmiClassStatisticList::Add (UmiClass^  mlClass,
                                    kkuint32     count
                                   )
{
  for each  (UmiClassStatistic^  stat  in  (*this))
  {
    if  (stat->MLClass == mlClass)
    {
      stat->Count = stat->Count + count;
      return;
    }
  }

  this->Add (gcnew UmiClassStatistic (mlClass, count));
}  /* AddInAClass */




void  UmiClassStatisticList::Add (UmiClassStatisticList^  stats)
{
  for each  (UmiClassStatistic^  stat  in  stats)
  {
    Add (stat->MLClass, stat->Count);
  }
}



kkuint32  UmiClassStatisticList::ImageCountTotal::get ()  
{
  kkuint32  imageCountTotal = 0;
  for each  (UmiClassStatistic^ stat in (*this))
    imageCountTotal += stat->Count;
  return  imageCountTotal;
}
