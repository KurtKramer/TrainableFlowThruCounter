/* RecordButtonThread.cpp -- Manages the actions required when the Stop button is pressed.
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
#include "RecordButtonThread.h"
using  namespace  LarcosCounterUnManaged;



RecordButtonThread::RecordButtonThread (LarcosCounterManagerPtr _manager,
                                        MsgQueuePtr             _msgQueue,
                                        const KKStr&            _threadName
                                       ):
    CameraThread (_manager, _threadName, _msgQueue),
    successful   (false),
    errMsg       ()

{
  log.Level (20) << "RecordButtonThread::RecordButtonThread" << endl;
}



RecordButtonThread::~RecordButtonThread ()
{
  log.Level (20) << "RecordButtonThread::~RecordButtonThread" << endl;
}



kkint32  RecordButtonThread::MemoryConsumedEstimated ()
{
  return  sizeof (RecordButtonThread);
}



void  RecordButtonThread::Run ()
{
  log.Level (10) << "RecordButtonThread::Run" << endl;
  Status (KKThread::tsRunning);
  Manager ()->StartRecordingAndOrCounting (successful,
                                           errMsg
                                          );

  log.Level (10) << "RecordButtonThread::Run   Exiting   TerminateFlag: " << TerminateFlag () <<  "  ShutdownFlag: " << ShutdownFlag () << endl;

  Status (KKThread::tsStopping);
  return;
}  /* Run */




void  RecordButtonThread::ResetCounts ()
{
}
