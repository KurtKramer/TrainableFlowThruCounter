#if  !defined(_LARCOSCOUNTERSTATE_)
#define  _LARCOSCOUNTERSTATE_
using namespace System;

#include "LarcosCounterManager.h"
using namespace  LarcosCounterUnManaged;


namespace LarcosCounterManaged
{
  typedef  LarcosCounterUnManaged::LarcosCounterManager::CounterState  CounterState;
  
  public enum class  LarcosCounterState: kkint16
  {
    _NULL              = (kkint16)CounterState::Null, 
    Stopped            = (kkint16)CounterState::Stopped, 
    Stopping           = (kkint16)CounterState::Stopping,
    Connected          = (kkint16)CounterState::Connected,
    Connecting         = (kkint16)CounterState::Connecting,
    Starting           = (kkint16)CounterState::Starting,
    BuildingClassifier = (kkint16)CounterState::BuildingClassifier,
    Running            = (kkint16)CounterState::Running,
    PlayingBack        = (kkint16)CounterState::PlayingBack,
    DroppedFrames      = (kkint16)CounterState::DroppedFrames
  };
  
}  /* LarcosCounterManaged */


#endif