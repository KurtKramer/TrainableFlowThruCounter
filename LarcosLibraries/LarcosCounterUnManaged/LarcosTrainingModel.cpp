/* LarcosTrainingModel.cpp -- Represents a single camera frame.
 * Copyright (C) 2011-2013  Kurt Kramer
 * For conditions of distribution and use, see copyright notice in LarcosCounterUnManaged.txt
 */
#include "FirstIncludes.h"

#include <errno.h>
#include <istream>
#include <iostream>
#include <vector>
#include "MemoryDebug.h"
using namespace std;


#include "KKBaseTypes.h"
#include "OSservices.h"
using namespace KKB;

#include "KKMLVariables.h"
using namespace KKMachineLearning;


#include "LarcosTrainingModel.h"
using  namespace  LarcosCounterUnManaged;


LarcosTrainingModel::LarcosTrainingModel ():
   subject           (),
   size              (),
   waterQuality      (),
   trainingModelName ()
{
}


LarcosTrainingModel::LarcosTrainingModel (const LarcosTrainingModel&  _entry):

    subject           (_entry.subject),
    size              (_entry.size),
    waterQuality      (_entry.waterQuality),
    trainingModelName (_entry.trainingModelName)
{
}



LarcosTrainingModel::LarcosTrainingModel (const KKStr&  _subject,
                                          const KKStr&  _size,
                                          const KKStr&  _waterQuality,
                                          const KKStr&  _trainingModelName
                                         ):
    subject           (_subject),
    size              (_size),
    waterQuality      (_waterQuality),
    trainingModelName (_trainingModelName)
{
}


LarcosTrainingModel::~LarcosTrainingModel ()
{
}



kkint32  LarcosTrainingModel::MemoryConsumedEstimated ()  const
{
  return  sizeof (*this) + 
          subject.MemoryConsumedEstimated           () +
          size.MemoryConsumedEstimated              () +
          waterQuality.MemoryConsumedEstimated      () +
          trainingModelName.MemoryConsumedEstimated ();
}




LarcosTrainingModelList::LarcosTrainingModelList (bool _owner):
   KKQueue<LarcosTrainingModel> (_owner)
{
}



LarcosTrainingModelList::LarcosTrainingModelList (const LarcosTrainingModelList&  _list):
   KKQueue<LarcosTrainingModel> (_list)
{
}



LarcosTrainingModelList::~LarcosTrainingModelList ()
{
}



LarcosTrainingModelListPtr  LarcosTrainingModelList::BuildFromTrainigModelDir ()
{
  KKStr  trainModelDir = KKMLVariables::TrainingModelsDir ();
  KKStrListPtr  modelFileNames = osGetListOfFiles (osAddSlash (trainModelDir) + "*.cfg");
  if  (modelFileNames == NULL)
    return NULL;

  LarcosTrainingModelListPtr  modelList = new LarcosTrainingModelList (true);

  KKStrList::const_iterator idx;
  for  (idx = modelFileNames->begin ();  idx != modelFileNames->end ();  ++idx)
  {
    KKStrPtr name = *idx;

    KKStr  fileRootName = osGetRootName (*name);
    if  (fileRootName.InstancesOfChar ('_') != 2)
      continue;

    KKStr  subject      = fileRootName.ExtractToken2 ("_");
    KKStr  size         = fileRootName.ExtractToken2 ("_");
    KKStr  waterQuality = fileRootName.ExtractToken2 ("_");

    modelList->PushOnBack (new LarcosTrainingModel (subject, size, waterQuality, name));
  }

  delete  modelFileNames;
  modelFileNames = NULL;
  return  modelList;
}  /* BuildFromTrainigModelDir */



KKStrListPtr  LarcosTrainingModelList::GetListOfSubjects ()
{
  map<KKStr,KKStr>  uniqueSubjects;
  map<KKStr,KKStr>::const_iterator  uniqueSubjectsIdx;

  KKStrListPtr uniqueSubjectsList = new KKStrList (true);

  LarcosTrainingModelList::const_iterator  idx;
  for  (idx = begin ();  idx != end ();  ++idx)
  {
    LarcosTrainingModelPtr ltm = *idx;
    KKStr  subjectLower = ltm->Subject ().ToLower ();
    uniqueSubjectsIdx = uniqueSubjects.find (subjectLower);
    if  (uniqueSubjectsIdx == uniqueSubjects.end ())
    {
      uniqueSubjectsList->PushOnBack (new KKStr (ltm->Subject ()));
      uniqueSubjects.insert (pair<KKStr,KKStr> (subjectLower, ltm->Subject ()));
    }
  }

  return  uniqueSubjectsList;
}  /* GetListOfSubjects */



KKStrListPtr  LarcosTrainingModelList::GetListOfSizes (const KKStr&  subject)
{
  map<KKStr,KKStr>  uniqueSizes;
  map<KKStr,KKStr>::const_iterator  uniqueSizesIdx;

  KKStrListPtr uniqueSizesList = new KKStrList (true);

  LarcosTrainingModelList::const_iterator  idx;
  for  (idx = begin ();  idx != end ();  ++idx)
  {
    LarcosTrainingModelPtr ltm = *idx;
    if  (subject.EqualIgnoreCase (ltm->Subject ()))
    {
      KKStr  sizeLower = ltm->Size ().ToLower ();
      uniqueSizesIdx = uniqueSizes.find (sizeLower);
      if  (uniqueSizesIdx == uniqueSizes.end ())
      {
        uniqueSizesList->PushOnBack (new KKStr (ltm->Size ()));
        uniqueSizes.insert (pair<KKStr,KKStr> (sizeLower, ltm->Size ()));
      }
    }
  }

  return  uniqueSizesList;
}  /* GetListOfSizes */



KKStrListPtr  LarcosTrainingModelList::GetListOfWaterQualities (const KKStr&  subject, const KKStr&  size)
{
  KKStrListPtr waterQualityList = new KKStrList (true);

  LarcosTrainingModelList::const_iterator  idx;
  for  (idx = begin ();  idx != end ();  ++idx)
  {
    LarcosTrainingModelPtr ltm = *idx;
    if  (subject.EqualIgnoreCase (ltm->Subject ())  &&   size.EqualIgnoreCase (ltm->Size ()))
      waterQualityList->PushOnBack (new KKStr (ltm->WaterQuality ()));
  }

  return  waterQualityList;
}  /* GetListOfWaterQualities */



bool  LarcosTrainingModelList::InList (const KKStr&  subject, 
                                       const KKStr&  size,  
                                       const KKStr&  waterQuality
                                      )
{
  LarcosTrainingModelList::const_iterator  idx;
  for  (idx = begin ();  idx != end ();  ++idx)
  {
    LarcosTrainingModelPtr ltm = *idx;
    if  (subject.EqualIgnoreCase      (ltm->Subject      ())  &&   
         size.EqualIgnoreCase         (ltm->Size         ())  &&
         waterQuality.EqualIgnoreCase (ltm->WaterQuality ())
        )
      return  true;
  }

  return  false;
}
