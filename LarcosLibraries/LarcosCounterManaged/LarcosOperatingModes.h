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
    _NULL         = LarcosCounterUnManaged::LarcosCounterManager::lomNULL,
    User          = LarcosCounterUnManaged::LarcosCounterManager::lomUser,
    Advanced      = LarcosCounterUnManaged::LarcosCounterManager::lomAdvanced,
    Invalid       = LarcosCounterUnManaged::LarcosCounterManager::lomInvalid
  };
}  /* LarcosCounterManaged */


#endif