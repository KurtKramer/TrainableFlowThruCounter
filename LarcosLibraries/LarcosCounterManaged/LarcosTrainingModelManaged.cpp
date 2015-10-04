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
using namespace  KKB;


#include "UmiOSservices.h"
#include "UmiKKStr.h"
#include "LarcosTrainingModelManaged.h"
using namespace  LarcosCounterManaged;



LarcosTrainingModelManaged::LarcosTrainingModelManaged (LarcosTrainingModelPtr  _larcosTrainingModel):
   larcosTrainingModel (_larcosTrainingModel)
 {
   int zed = 100;
 }


 String^     LarcosTrainingModelManaged::Subject::get ()
 {
   if  (larcosTrainingModel)
     return  UmiKKStr::KKStrToSystenStr (larcosTrainingModel->Subject ());
   else
     return  "";
 }


 String^     LarcosTrainingModelManaged::Size::get ()
 {
   if  (larcosTrainingModel)
     return  UmiKKStr::KKStrToSystenStr (larcosTrainingModel->Size ());
   else
     return  "";
 }


 String^     LarcosTrainingModelManaged::WaterQuality::get ()
 {
   if  (larcosTrainingModel)
     return  UmiKKStr::KKStrToSystenStr (larcosTrainingModel->WaterQuality ());
   else
     return  "";
 }



 String^  LarcosTrainingModelManaged::TrainingModelName::get ()
 {
   if  (larcosTrainingModel)
     return  UmiKKStr::KKStrToSystenStr (larcosTrainingModel->TrainingModelName ());
   else
     return  "";
 }


 
LarcosTrainingModelPtr  LarcosTrainingModelManaged::UnManaged ()
{
  return larcosTrainingModel;
}




LarcosTrainingModelListManaged::LarcosTrainingModelListManaged ()
{
}



LarcosTrainingModelListManaged::LarcosTrainingModelListManaged (LarcosTrainingModelListPtr  _larcosTrainingModels):
   larcosTrainingModels (_larcosTrainingModels)
{
}



LarcosTrainingModelListManaged::!LarcosTrainingModelListManaged ()
{
  delete  larcosTrainingModels;
  larcosTrainingModels = NULL;
}



LarcosTrainingModelListManaged::~LarcosTrainingModelListManaged ()
{
  this->!LarcosTrainingModelListManaged ();
}



LarcosTrainingModelListManaged^  LarcosTrainingModelListManaged::BuildFromTrainigModelDir ()
{
  LarcosTrainingModelListPtr models = LarcosTrainingModelList::BuildFromTrainigModelDir ();
  LarcosTrainingModelListManaged^  results = gcnew LarcosTrainingModelListManaged (models);
  // 'results' now owns 'models' so we do not need to delete it.
  models = NULL;
  return  results;
}



array<String^>^  LarcosTrainingModelListManaged::GetListOfSubjects ()
{
  KKStrListPtr  subjects = larcosTrainingModels->GetListOfSubjects ();
  array<String^>^  managedSubjects =  UmiKKStr::KKStrToListToStringArray (subjects);
  delete  subjects;
  subjects = NULL;
  return  managedSubjects;
}


array<String^>^  LarcosTrainingModelListManaged::GetListOfSizes (String^ subject)
{
  KKStrListPtr  sizes = larcosTrainingModels->GetListOfSizes (UmiKKStr::SystemStringToKKStr (subject));
  array<String^>^  managedSizes =  UmiKKStr::KKStrToListToStringArray (sizes);
  delete  sizes;
  sizes = NULL;
  return  managedSizes;
}



array<String^>^  LarcosTrainingModelListManaged::GetListOfWaterQualities (String^ subject, String^ size)
{
  KKStrListPtr  waterQualities = larcosTrainingModels->GetListOfWaterQualities (UmiKKStr::SystemStringToKKStr (subject), UmiKKStr::SystemStringToKKStr (size));
  array<String^>^  managedWaterQualities =  UmiKKStr::KKStrToListToStringArray (waterQualities);
  delete  waterQualities;
  waterQualities = NULL;
  return  managedWaterQualities;
}



bool  LarcosTrainingModelListManaged::InList (String^ subject, String^ size, String^ waterQuality)
{
  return  larcosTrainingModels->InList (UmiKKStr::SystemStringToKKStr (subject), 
                                        UmiKKStr::SystemStringToKKStr (size),
                                        UmiKKStr::SystemStringToKKStr (waterQuality)
                                       );
}
