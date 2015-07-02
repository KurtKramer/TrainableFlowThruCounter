#if  !defined(_LARCOSOPERATINGMODES_)
#define  _LARCOSOPERATINGMODES_
using namespace System;

#include "LarcosCounterManager.h"
using namespace  LarcosCounterUnManaged;

#include "LarcosOperatingModes.h"


namespace LarcosCounterManaged
{
  public enum class  LarcosOperatingModes: kkint16
  {
    _NULL         = LarcosCounterUnManaged::LarcosCounterManager::LarcosOperatingModes::Null,
    User          = LarcosCounterUnManaged::LarcosCounterManager::LarcosOperatingModes::User,
    Advanced      = LarcosCounterUnManaged::LarcosCounterManager::LarcosOperatingModes::Advanced,
    Invalid       = LarcosCounterUnManaged::LarcosCounterManager::LarcosOperatingModes::Invalid
  };
}  /* LarcosCounterManaged */


#endif