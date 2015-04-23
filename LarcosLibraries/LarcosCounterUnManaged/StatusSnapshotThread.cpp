/* StatusSnapshotThread.cpp -- Takes at fixed intervals a snapshot of recording stats.
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
#include "ImageIO.h"
#include "OSservices.h"
using namespace KKB;

#include "Variables.h"
using namespace  KKLSC;

#include "LarcosCounterManager.h"
#include "LarcosCounterStats.h"
#include "StatusSnapshotThread.h"
#include "StatusSnapshot.h"
using  namespace  LarcosCounterUnManaged;



StatusSnapshotThread::StatusSnapshotThread (LarcosCounterManagerPtr  _manager,
                                            StatusSnapshotBufferPtr  _snapshotBuffer,
                                            kkint32                  _snapshotInterval,
                                            MsgQueuePtr              _msgQueue,
                                            const KKStr&             _threadName
                                           ):

    CameraThread (_manager, _threadName, _msgQueue),
    snapshotInterval (_snapshotInterval),
    snapshotBuffer   (_snapshotBuffer),
    lastStats        (),
    nextStats        ()
{
  log.Level (10) << "StatusSnapshotThread::StatusSnapshotThread" << endl;
}



StatusSnapshotThread::~StatusSnapshotThread ()
{
  snapshotBuffer = NULL;
  log.Level (10) << "StatusSnapshotThread::~StatusSnapshotThread" << endl;
}



kkint32  StatusSnapshotThread::MemoryConsumedEstimated ()
{
  return  sizeof (StatusSnapshotThread);
}





void  StatusSnapshotThread::Run ()
{
  Status (ThreadStatus::tsRunning);
  log.Level (10) << "StatusSnapshotThread::Run" << endl;

  if  (!snapshotBuffer)
  {
    log.Level (-1) << endl
                   << "StatusSnapshotThread::Run   ***ERROR***   Snapshot Buffer is not defined." << endl
                   << endl;
    Status (ThreadStatus::tsStopping);
    return;
  }

  KKB::DateTime  timeStart = KKB::osGetLocalDateTime ();
  KKB::DateTime  timeNow   = timeStart;

  kkint32  numSubSamplesToTake = snapshotInterval * 2;

  kkint32  logicalFrameQueueSizeMax = Max (1, Manager ()->LogicalFrameQueueSizeMax ());

  lastStats.Reset ();
  while  ((!TerminateFlag ())  &&  (!ShutdownFlag ()))
  {
    // We will take sub-samplings every 0.5 seconds of LogicalFrames on queue and Processors in use.
    // The mean average will then be used for the snapshot taken at the defined interval of 'snapshotInterval'.

    kkint32  numSubSamplingsTaken                 = 0;
    kkint32  totalLogicalFrameProcessorsAvailable = 0;
    kkint32  totalLogicalFramesOnQueue            = 0;

    while  ((!TerminateFlag ())  &&  (!ShutdownFlag ())  &&  (numSubSamplingsTaken < numSubSamplesToTake))
    {
      kkint32  logicalFrameProcessorsAvailable = 0;
      kkint32  logicalFramesOnQueue            = 0;
      Manager ()->GetLogicalFrameStats (logicalFramesOnQueue, logicalFrameProcessorsAvailable);
      totalLogicalFramesOnQueue            += logicalFramesOnQueue;
      totalLogicalFrameProcessorsAvailable += logicalFrameProcessorsAvailable;
      ++numSubSamplingsTaken;
      osSleepMiliSecs (500);
    }

    {
      Manager ()->GetStats (nextStats);
      StatusSnapshotPtr  snapshot = new StatusSnapshot ();
      snapshot->Assign (lastStats, nextStats);

      if  (numSubSamplingsTaken > 0)
      {
        float  avgLogicalFrameProcessorsAvailable = (float)totalLogicalFrameProcessorsAvailable / (float)numSubSamplingsTaken;
        float  avgLogicalFramesOnQueue            = (float)totalLogicalFramesOnQueue            / (float)numSubSamplingsTaken;

        snapshot->LogicalFrameProcessorsAvailable (avgLogicalFrameProcessorsAvailable);
        snapshot->LogicalFramesOnQueue            (avgLogicalFramesOnQueue);
      }

      kkint32  frameProcessorsCount = Max (1, Manager ()->FrameProcessorsCount ());

      float  availableCapacity = (0.8f * (float)snapshot->LogicalFrameProcessorsAvailable () / (float)frameProcessorsCount) + 
                                 (0.2f * (float)((float)logicalFrameQueueSizeMax - (float)(snapshot->LogicalFramesOnQueue () + frameProcessorsCount)) / (float)logicalFrameQueueSizeMax);

      snapshot->AvailableCapacity (availableCapacity);

      timeNow = KKB::osGetLocalDateTime ();
      snapshot->TimeOffset ((kkint32)((timeNow - timeStart).Seconds ()));
      snapshotBuffer->AddEntry (snapshot);
      lastStats.Assign (nextStats);
    }
  }

  log.Level (10) << "StatusSnapshotThread::Run   Exiting   TerminateFlag: " << TerminateFlag () <<  "  ShutdownFlag: " << ShutdownFlag () << endl;

  Status (ThreadStatus::tsStopping);
  return;
}  /* Run */



void  StatusSnapshotThread::ResetCounts ()
{
  lastStats.Reset ();
}
