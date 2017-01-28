/* CameraAutoGainThread.h --
 * Copyright (C) 2011-2013  Kurt Kramer
 * For conditions of distribution and use, see copyright notice in LarcosCounterUnManaged.txt
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

#include "ScannerFile.h"
#include "ScannerFileEntry.h"
#include "Variables.h"
using namespace  KKLSC;

#include "CameraAcquisition.h"
#include "CameraFrameBuffer.h"
#include "LarcosCounterManager.h"
#include "CameraAutoGainThread.h"
using  namespace  LarcosCounterUnManaged;



CameraAutoGainThread::CameraAutoGainThread (LarcosCounterManagerPtr _manager,
                                            MsgQueuePtr             _msgQueue,
                                            CameraAcquisitionPtr    _camera,
                                            CameraFrameBufferPtr    _cameraFrameBuffer,
                                            const KKStr&            _threadName
                                           ):
    CameraThread (_manager, _threadName, _msgQueue),
    camera            (_camera),
    cameraFrameBuffer (_cameraFrameBuffer)
{
}



CameraAutoGainThread::~CameraAutoGainThread ()
{
}



kkMemSize  CameraAutoGainThread::MemoryConsumedEstimated ()
{
  return  sizeof (CameraAutoGainThread);
}



void  CameraAutoGainThread::Run ()
{
  Status (ThreadStatus::Running);
  log.Level (10) << "CameraAutoGainThread::Run    Thread started." << endl;

  if  (camera == NULL)
  {
    log.Level (-1) << endl
                   << "CameraAutoGainThread::Run   ***ERROR***  Camera is not defined !!!" << endl 
                   << endl;
  }

  else if  (!Manager ()->WeAreConnectedToCamera ())
  {
    log.Level (-1) << endl
                   << "CameraAutoGainThread::Run   ***ERROR***  Camera is not connected !!!" << endl 
                   << endl;
  }

  else
  {
    camera->PerformAutoGainProcedure ();
  }

  Status (ThreadStatus::Stopping);

  log.Level (10) << "CameraAutoGainThread::Run   Exiting   TerminateFlag: " << TerminateFlag () <<  "  ShutdownFlag: " << ShutdownFlag () << endl;

  return;
}  /* Run */




void  CameraAutoGainThread::ResetCounts ()
{
}
