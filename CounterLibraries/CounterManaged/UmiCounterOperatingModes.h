#if  !defined(_COUNTEROPERATINGMODES_)
#define  _COUNTEROPERATINGMODES_
using namespace System;

#include "CounterManager.h"
using namespace  CounterUnManaged;

#include "UmiCounterOperatingModes.h"


namespace CounterManaged
{
  public enum class  UmiCounterOperatingModes: kkint16
  {
    _NULL         = CounterUnManaged::CounterManager::CounterOperatingModes::Null,
    User          = CounterUnManaged::CounterManager::CounterOperatingModes::User,
    Advanced      = CounterUnManaged::CounterManager::CounterOperatingModes::Advanced,
    Invalid       = CounterUnManaged::CounterManager::CounterOperatingModes::Invalid
  };
}  /* CounterManaged */


#endif