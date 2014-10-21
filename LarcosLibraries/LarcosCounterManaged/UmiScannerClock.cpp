#include "StdAfx.h"
#include "FirstIncludes.h"
#include <stdio.h>
#include "MemoryDebug.h"

using namespace System;

#include "UmiScannerClock.h"
using namespace  LarcosCounterManaged;


UmiScannerClock::UmiScannerClock ():
   time (0)
{
}



long  UmiScannerClock::Time::get ()
{
  long x = time;
  time++;
  return  x;
}



void   UmiScannerClock::Time::set (long _time)
{
  time = _time;
}
