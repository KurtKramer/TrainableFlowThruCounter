#include "StdAfx.h"
#include "FirstIncludes.h"

#include <stdio.h>
#include <math.h>
#include <ctype.h>
#include <time.h>
#include <fstream>
#include <iostream>
#include <istream>
#include <map>
#include <ostream>
#include <string>
#include <vector>
#include "MemoryDebug.h"
using namespace std;


#include  "KKBaseTypes.h"
#include  "..\\KKBase\\GoalKeeper.h"

#include "OSservices.h"
using namespace  KKB;



#include "TrainingClass.h"
using namespace  KKMLL;



#include "UmiTrainingClass.h"
#include "UmiClassList.h"
#include "UmiKKStr.h"
#include "UmiRunLog.h"
using namespace  LarcosCounterManaged;


using namespace System;
using namespace System::Threading;
using namespace System::Windows::Forms;



UmiTrainingClass::UmiTrainingClass (UmiClass^  _class,
                                    String^    _directory
                                   )
{
  trainingClass = new TrainingClass (UmiKKStr::SystemStringToKKStr (_directory), 
                                     UmiKKStr::SystemStringToKKStr (_class->Name),
                                     1.0f,
                                     1.0f,   // CountFactor
                                     *(::MLClass::GlobalClassList ())
                                    );
}



UmiTrainingClass::UmiTrainingClass (TrainingClassPtr  _trainingClass):
  trainingClass (_trainingClass)
{
}




TrainingClassPtr  UmiTrainingClass::UnManagedClass ()
{
  if  (trainingClass)
    return trainingClass;
  return  NULL;
}



float  UmiTrainingClass::CountFactor::get ()
{
  if  (trainingClass == NULL)
    return 1.0f;
  else
    return trainingClass->CountFactor ();
}



void  UmiTrainingClass::CountFactor::set (float _countFactor)
{
  trainingClass->CountFactor (_countFactor);
}
 



String^  UmiTrainingClass::Directory::get ()
{
  if (trainingClass == NULL)
    return  gcnew String ("");

  return  UmiKKStr::KKStrToSystenStr (trainingClass->Directory ());
}




void  UmiTrainingClass::Directory::set (String^  _directory)
{
  trainingClass->Directory (UmiKKStr::SystemStringToKKStr (_directory));
}



UmiClass^  UmiTrainingClass::MLClass::get ()
{
  return   UmiClassList::GetUniqueClass (trainingClass->MLClass ());
}


void  UmiTrainingClass::MLClass::set (UmiClass^  _mlClass)
{
  trainingClass->MLClass (_mlClass->UnmanagedImageClass());
}



String^   UmiTrainingClass::ExpandedDirectory (String^  rootDir)
{
  if  (rootDir == nullptr)
    rootDir = String::Empty;
  return  UmiKKStr::KKStrToSystenStr (trainingClass->ExpandedDirectory (UmiKKStr::SystemStringToKKStr (rootDir)));
}  /* ExpandedDirectory */




UmiTrainingClassList::UmiTrainingClassList ()
{
}



///<summary>
/// Will take ownership of the contents of 'trainingClasses'.  If 'trainingClasses'
/// does not own its own contents; will then create new instances of 'TrainingClass' 
/// objects.
///</summary>
UmiTrainingClassList::UmiTrainingClassList (TrainingClassListPtr  trainingClasses)
{
  TrainingClassList::iterator  idx;
  for  (idx = trainingClasses->begin ();  idx != trainingClasses->end ();  idx++)
  {
    TrainingClassPtr  tc = *idx;
    if  (!trainingClasses->Owner ())
      tc = new TrainingClass (*tc);
    Add (gcnew UmiTrainingClass (tc));
  }

  trainingClasses->Owner (false);
}
