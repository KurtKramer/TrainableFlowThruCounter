#if  !defined(_LARCOSCOUNTERSTATE_)
#define  _LARCOSCOUNTERSTATE_
using namespace System;

#include "LarcosCounterManager.h"
using namespace  LarcosCounterUnManaged;


namespace LarcosCounterManaged
{
  public enum class  LarcosCounterState: kkint16
  {
    _NULL              = LarcosCounterUnManaged::LarcosCounterManager::csNULL, 
    Stopped            = LarcosCounterUnManaged::LarcosCounterManager::csStopped, 
    Stopping           = LarcosCounterUnManaged::LarcosCounterManager::csStopping,
    Connected          = LarcosCounterUnManaged::LarcosCounterManager::csConnected,
    Connecting         = LarcosCounterUnManaged::LarcosCounterManager::csConnecting,
    Starting           = LarcosCounterUnManaged::LarcosCounterManager::csStarting,
    BuildingClassifier = LarcosCounterUnManaged::LarcosCounterManager::csBuildingClassifier,
    Running            = LarcosCounterUnManaged::LarcosCounterManager::csRunning,
    PlayingBack        = LarcosCounterUnManaged::LarcosCounterManager::csPlayingBack,
    DroppedFrames      = LarcosCounterUnManaged::LarcosCounterManager::csDroppedFrames
  };
}  /* LarcosCounterManaged */


#endif