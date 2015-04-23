/* ConnectButtonThread.cpp -- Manages the actions required when the Connect button is pressed.
 * Copyright (C) 2011-2013  Kurt Kramer
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
#include "ConnectButtonThread.h"
using  namespace  LarcosCounterUnManaged;



ConnectButtonThread::ConnectButtonThread (LarcosCounterManagerPtr _manager,
                                          MsgQueuePtr             _msgQueue,
                                          const KKStr&            _threadName
                                          ):
    CameraThread (_manager, _threadName, _msgQueue)
{
}



ConnectButtonThread::~ConnectButtonThread ()
{
}



kkint32  ConnectButtonThread::MemoryConsumedEstimated ()
{
  return  sizeof (ConnectButtonThread);
}




void  ConnectButtonThread::Run ()
{
  log.Level (40) << "ConnectButtonThread::Run" << endl;
  Status (ThreadStatus::tsRunning);
  bool  successful = false;
  Manager ()->ConnectToCamera (successful);
  Status (ThreadStatus::tsStopping);
  log.Level (10) << "ConnectButtonThread::Run   Exiting   TerminateFlag: " << TerminateFlag () <<  "  ShutdownFlag: " << ShutdownFlag () << endl;
  return;
}  /* Run */




void  ConnectButtonThread::ResetCounts ()
{
}
