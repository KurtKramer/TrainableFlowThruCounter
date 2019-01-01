#if  !defined(_COUNTERSTATE_)
#define  _COUNTERSTATE_
using namespace System;

#include "CounterManager.h"
using namespace  CounterUnManaged;


namespace CounterManaged
{
  typedef  CounterUnManaged::CounterManager::CounterState  CounterState;
  
  public enum class  UmiCounterState: kkint16
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
  
}  /* CounterManaged */


#endif