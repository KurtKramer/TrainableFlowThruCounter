/* PlayBackButtonThread.cpp -- Manages the actions required when the Stop button is pressed.
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
#include "PlayBackButtonThread.h"
using  namespace  LarcosCounterUnManaged;



PlayBackButtonThread::PlayBackButtonThread (LarcosCounterManagerPtr _manager,
                                            const KKStr&            _srcScannerFileName,
                                            MsgQueuePtr             _msgQueue,
                                            const KKStr&            _threadName
                                           ):
    CameraThread (_manager, _threadName, _msgQueue),
    srcScannerFileName (_srcScannerFileName),
    successful         (false),
    errMsg             ()

{
  log.Level (10) << "PlayBackButtonThread::PlayBackButtonThread" << endl;
}



PlayBackButtonThread::~PlayBackButtonThread ()
{
  log.Level (10) << "PlayBackButtonThread::~PlayBackButtonThread" << endl;
}



kkint32  PlayBackButtonThread::MemoryConsumedEstimated ()
{
  return  sizeof (PlayBackButtonThread);
}



void  PlayBackButtonThread::Run ()
{
  log.Level (10) << "PlayBackButtonThread::Run" << endl;
  Status (KKThread::tsRunning);
  Manager ()->PlayBackScannerFile (srcScannerFileName,
                                   successful,
                                   errMsg
                                  );

  log.Level (10) << "PlayBackButtonThread::Run   Exiting   TerminateFlag: " << TerminateFlag () <<  "  ShutdownFlag: " << ShutdownFlag () << endl;

  Status (KKThread::tsStopping);
  return;
}  /* Run */




void  PlayBackButtonThread::ResetCounts ()
{
}
