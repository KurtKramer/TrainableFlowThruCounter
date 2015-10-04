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
  VectorKKStr  directories;
  directories.push_back (UmiKKStr::SystemStringToKKStr (_directory));

  trainingClass = new TrainingClass (directories, 
                                     UmiKKStr::SystemStringToKKStr (_class->Name),
                                     1.0f,
                                     1.0f,   // CountFactor
                                     NULL,   // subClassifier
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
 




UmiClass^  UmiTrainingClass::MLClass::get ()
{
  return   UmiClassList::GetUniqueClass (trainingClass->MLClass ());
}



void  UmiTrainingClass::MLClass::set (UmiClass^  _mlClass)
{
  trainingClass->MLClass (_mlClass->UnmanagedImageClass());
}




String^  UmiTrainingClass::Directory (int  idx)
{
  const KKStr& dirName = trainingClass->Directory (idx);
  return  UmiKKStr::KKStrToSystenStr (dirName);
}



void  UmiTrainingClass::Directory (int      idx, 
                                   String^  directory
                                  )
{
  trainingClass->Directory (idx, UmiKKStr::SystemStringToKKStr (directory));
}





String^  UmiTrainingClass::ExpandedDirectory (String^  rootDir,
                                              int      idx
                                             )
{
  if  (rootDir == nullptr)
    rootDir = String::Empty;
  return  UmiKKStr::KKStrToSystenStr (trainingClass->ExpandedDirectory (UmiKKStr::SystemStringToKKStr (rootDir), idx));
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
