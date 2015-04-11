/* LarcosTrainingModel.h -- Represents a TraningModel Entry.
 * Copyright (C) 2011-2013  Kurt Kramer
 * For conditions of distribution and use, see copyright notice in LarcosCounterUnManaged.txt
 */
#if  !defined(_PARTICLEENTRY_)
#define  _PARTICLEENTRY_

#include <queue>

#include "GoalKeeper.h"
#include "KKQueue.h"
#include "MorphOpStretcher.h"
#include "Raster.h"
#include "RasterBuffer.h"
#include "RunLog.h"

#include "MLClass.h"

using namespace KKMLL;

namespace LarcosCounterUnManaged
{
  class  LarcosTrainingModel
  {
  public:
    typedef  LarcosTrainingModel*  LarcosTrainingModelPtr;

    LarcosTrainingModel ();

    LarcosTrainingModel (const LarcosTrainingModel&  _entry);

    LarcosTrainingModel (const KKStr&  subject,
                         const KKStr&  size,
                         const KKStr&  waterQuality,
                         const KKStr&  trainingModelName
                        );

    ~LarcosTrainingModel ();

    kkint32  MemoryConsumedEstimated ()  const;

    const KKStr&   Subject           () const {return subject;}
    const KKStr&   Size              () const {return size;}
    const KKStr&   WaterQuality      () const {return waterQuality;}
    const KKStr&   TrainingModelName () const {return trainingModelName;}

    void  Subject           (const KKStr&  _subject)            {subject           = _subject;}
    void  Size              (const KKStr&  _size)               {size              = _size;}
    void  WaterQuality      (const KKStr&  _waterQuality)       {waterQuality      = _waterQuality;}
    void  TrainingModelName (const KKStr&  _trainingModelName)  {trainingModelName = _trainingModelName;}

  private:
    KKStr   subject;
    KKStr   size;
    KKStr   waterQuality;
    KKStr   trainingModelName;
  };  /* LarcosTrainingModel */

  typedef  LarcosTrainingModel::LarcosTrainingModelPtr  LarcosTrainingModelPtr;



  class  LarcosTrainingModelList:  public KKQueue<LarcosTrainingModel>
  {
  public:
    typedef  LarcosTrainingModelList*  LarcosTrainingModelListPtr;

    LarcosTrainingModelList (bool _owner);

    LarcosTrainingModelList (const LarcosTrainingModelList&  list);

    static
      LarcosTrainingModelListPtr  BuildFromTrainigModelDir ();

    ~LarcosTrainingModelList ();

    KKStrListPtr  GetListOfSubjects ();

    KKStrListPtr  GetListOfSizes (const KKStr&  subject);

    KKStrListPtr  GetListOfWaterQualities (const KKStr&  subject, const KKStr&  size);

    bool          InList (const KKStr&  subject, const KKStr&  size,  const KKStr&  waterQuality);
  };

  typedef  LarcosTrainingModelList::LarcosTrainingModelListPtr  LarcosTrainingModelListPtr;

}  /* LarcosCounterUnManaged */

#endif
