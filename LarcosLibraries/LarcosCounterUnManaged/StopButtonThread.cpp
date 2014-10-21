/* StopButtonThread.cpp -- Manages the actions required when the Stop button is pressed.
 * Copyright (C) 2011-2011  Kurt Kramer
 * For conditions of distribution and use, see copyright notice in KKLineScanner.h
 */
#include "FirstIncludes.h"

#include <errno.h>
#include <istream>
#include <iostream>
#include <fstream>
#include <deque>
#include <map>
#include <vector>
#include "MemoryDebug.h"
using namespace std;


#include "KKBaseTypes.h"
using namespace KKB;

#include "Variables.h"
using namespace  KKLSC;

#include "LarcosCounterManager.h"
#include "StopButtonThread.h"
using  namespace  LarcosCounterUnManaged;



StopButtonThread::StopButtonThread (LarcosCounterManagerPtr _manager,
                                    bool                    _stopImmediately,
                                    MsgQueuePtr             _msgQueue,
                                    const KKStr&            _threadName
                                    ):
    CameraThread (_manager, _threadName, _msgQueue),
    stopImmediately (_stopImmediately)
{
  log.Level (10) << "StopButtonThread::StopButtonThread" << endl;
}



StopButtonThread::~StopButtonThread ()
{
  log.Level (10) << "StopButtonThread::~StopButtonThread" << endl;
}



kkint32  StopButtonThread::MemoryConsumedEstimated ()
{
  return  sizeof (StopButtonThread);
}




void  StopButtonThread::Run ()
{
  Status (KKThread::tsRunning);
  log.Level (10) << "StopButtonThread::Run" << endl;
  Manager ()->CloseOutCountingAndOrRecording (TerminateFlag (), stopImmediately);
  
  log.Level (10) << "StopButtonThread::Run   Exiting   TerminateFlag: " << TerminateFlag () <<  "  ShutdownFlag: " << ShutdownFlag () << endl;

  Status (KKThread::tsStopping);
  return;
}  /* Run */




void  StopButtonThread::ResetCounts ()
{
}