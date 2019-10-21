/* CameraThread.cpp -- Responsible for extracting individual images from frames.
 * Copyright (C) 2011-2013  Kurt Kramer
 */
#include "FirstIncludes.h"

#include <errno.h>
#include <istream>
#include <iostream>
#include <fstream>
#include <queue>
#include <map>
#include <vector>
#include "MemoryDebug.h"
using namespace std;


#include "KKBaseTypes.h"
#include "KKThread.h"
#include "OSservices.h"
using namespace KKB;


#include "CounterVariables.h"
using namespace  CounterBase ;


#include "CameraThread.h"
using  namespace  CounterUnManaged;




CameraThread::CameraThread (CounterManagerPtr _manager,
                            const KKStr&            _threadName,
                            MsgQueuePtr             _msgQueue
                           ):
    KKThread (_threadName, NULL, _msgQueue),
    log       (_msgQueue),
    manager   (_manager)
{
  log.SetLevel (CounterVariables::DebugLevel ());
}



CameraThread::~CameraThread ()
{
}



void  CameraThread::CropSettingsChanged (kkint32  _cropLeft,
                                         kkint32  _cropRight
                                        )
{
}



void   CameraThread::ScanRateChanged (float _newScanRate)
{
}



void  CameraThread::TerminateFlagChanged ()
{
  KKThread::TerminateFlagChanged ();
}




kkMemSize  CameraThread::MemoryConsumedEstimated ()  const
{
  return  KKThread::MemoryConsumedEstimated () + log.MemoryConsumedEstimated ();
}




void  CameraThread::Run ()
{
  // This method should have been over ridden by a derived class.
  log.Level (-1) << endl
                 << "CameraThread::Run   ***ERROR***    This method should have been over ridden by a derived class." << endl
                 << endl;
}




CameraThreadList::CameraThreadList (bool _owner):
   KKQueue<CameraThread> (_owner)
{
}


//CameraThreadList::CameraThreadList (const CameraThreadList&  list):
//  KKQueue<CameraThread> (list)
//{
//}



CameraThreadList::~CameraThreadList ()
{
}



kkMemSize  CameraThreadList::MemoryConsumedEstimated ()  const
{
  kkMemSize  memoryConsumedEstimated = sizeof (*this) + size () * sizeof (void*);
  if  (this->Owner ())
  {
    CameraThreadList::const_iterator idx;
    for  (idx = begin ();  idx != end ();  ++idx)
    {
      CameraThreadPtr  thread = *idx;
      memoryConsumedEstimated += thread->MemoryConsumedEstimated ();
    }
  }
  return  memoryConsumedEstimated;
}  /* MemoryConsumedEstimated */



void  CameraThreadList::SendTerminateCmdToAllThreads ()
{
  CameraThreadList::iterator idx;
  for  (idx = begin ();  idx != end ();  ++idx)
  {
    CameraThreadPtr  t = *idx;
    if  ((t->Status () == KKThread::ThreadStatus::Running)  ||  
         (t->Status () == KKThread::ThreadStatus::Starting)
        )
    {
      t->TerminateThread ();
    }
  }
}  /* SendTerminateCmdToAllThreads */




void  CameraThreadList::SendShutdownCmdToAllThreads ()
{
  CameraThreadList::iterator idx;
  for  (idx = begin ();  idx != end ();  ++idx)
  {
    CameraThreadPtr  t = *idx;
    if  ((t->Status () == KKThread::ThreadStatus::Running)  ||  
         (t->Status () == KKThread::ThreadStatus::Starting)
        )
    {
      t->ShutdownThread ();
    }
  }
}  /* SendShutdownCmdToAllThreads */



void  CameraThreadList::WaitForAllThreadsToStop (float  maxWaitTime,
                                                 bool&  allThreadsStopped
                                                )
{
  double  startTime = osGetSystemTimeUsed ();

  allThreadsStopped = false;
  float  timeWaitedSoFar = 0.0f;

  do  
  {
    allThreadsStopped = true;
    CameraThreadList::iterator idx;
    for  (idx = begin ();  idx != end ();  ++idx)
    {
      CameraThreadPtr  t = *idx;
      if  (t->Status () != KKThread::ThreadStatus::Stopped)
        allThreadsStopped = false;
    }

    double  timeNow = osGetSystemTimeUsed ();
    timeWaitedSoFar = (float)(timeNow - startTime);
  }  while  ((!allThreadsStopped)  &&  (timeWaitedSoFar < maxWaitTime));

  return;
}  /* WaitForAllTHreadsToStop */



void  CameraThreadList::TerminateAllButSpecifiedThread (CameraThreadPtr specifiedThread)
{
  CameraThreadList threadsToDelete (false);
  iterator  idx;
  for  (idx = begin ();  idx != end ();  ++idx)
  {
    CameraThreadPtr  ct = *idx;
    if  (ct != specifiedThread)
    {
      ct->TerminateThread ();
      threadsToDelete.PushOnBack (ct);
    }
  }

  while  (threadsToDelete.QueueSize () > 0)
  {
    CameraThreadPtr  ct = threadsToDelete.PopFromBack ();
    ct->WaitForThreadToStop (1);
    DeleteEntry (ct);
    delete ct;
    ct = NULL;
  }
}  /* TerminateAllButSpecifiedThread */




void  CameraThreadList::DeleteAllStoppedThreads ()
{
  CameraThreadList threadsToDelete (false);
  iterator  idx;
  for  (idx = begin ();  idx != end ();  ++idx)
  {
    CameraThreadPtr  ct = *idx;
    if  (ct->Status () == KKThread::ThreadStatus::Stopped)
      threadsToDelete.PushOnBack (ct);
  }

  while  (threadsToDelete.QueueSize () > 0)
  {
    CameraThreadPtr  ct = threadsToDelete.PopFromBack ();
    DeleteEntry (ct);
    delete ct;
    ct = NULL;
  }
}  /* DeleteAllStoppedThreads */


