/* StatusSnapshot.cpp -- Represents a snapshot of Status.
 * Copyright (C) 2011-2011  Kurt Kramer
 * For conditions of distribution and use, see copyright notice in KKLineScanner.h
 */
#include "FirstIncludes.h"

#include <errno.h>
#include <istream>
#include <iostream>
#include <vector>
#include "MemoryDebug.h"
using namespace std;


#include "KKBaseTypes.h"
#include "ImageIO.h"
#include "OSservices.h"
#include "Raster.h"
using namespace KKB;

#include "MLClass.h"
using namespace KKMLL;

#include "LarcosCounterStats.h"
#include "StatusSnapshot.h"
using  namespace  LarcosCounterUnManaged;



const KKStr  StatusSnapshot::dataFieldIdxStrs[] 
                 = {"NULL",
                    "LogicalFrameProcessorsAvailable",
                    "LogicalFramesOnQueue",
                    "Count",
                    "Particles",
                    "CpuUsage",
                    "AvaialbleCapacity",
                    "PhysicalFramesDropped",
                    "LogicalFramesDropped",
                    "ScanLinesRead",
                    "ScanLinesWritten",
                    "ParticlesWaitingProcessing",
                    "FlowRate",
                    "Invalid"
                   };


const KKStr&  StatusSnapshot::SnapShotFieldIdxToStr (FieldIdx i)
{
  if  ((i < FieldIdx::Null)  ||  (i > FieldIdx::Invalid))
    return dataFieldIdxStrs[(int)FieldIdx::Null];
  else
    return dataFieldIdxStrs[(int)i];
}




StatusSnapshot::FieldIdx  StatusSnapshot::SnapShotFieldIdxFromStr (const KKStr&  s)
{
  int  x = 0;
  while  (x <= (int)FieldIdx::Invalid)
  {
    if  (s.EqualIgnoreCase (dataFieldIdxStrs[x]))
      return  (FieldIdx)x;
    ++x;
  }

  return  FieldIdx::Null;
}




StatusSnapshot::StatusSnapshot ():
  timeStamp                       (),
  timeOffset                      (0),
  logicalFrameProcessorsAvailable (0.0f),
  logicalFramesOnQueue            (0.0f),
  count                           (0),
  particles                       (0),
  cpuUsage                        (0.0f),
  availableCapacity               (0.0f),
  cameraTemparature               (-1),
  physicalFramesDropped           (0),
  logicalFramesDropped            (0),
  scanLinesRead                   (0),
  scanLinesWritten                (0),
  particlesWaitingProcessing      (0),
  flowRate                        (0.0f)
{
}




StatusSnapshot::StatusSnapshot (const StatusSnapshot&  _entry):
  timeStamp                       (_entry.timeStamp),
  timeOffset                      (_entry.timeOffset),
  logicalFrameProcessorsAvailable (_entry.logicalFrameProcessorsAvailable),
  logicalFramesOnQueue            (_entry.logicalFramesOnQueue),
  count                           (_entry.count),
  particles                       (_entry.particles),
  cpuUsage                        (_entry.cpuUsage),
  availableCapacity               (_entry.availableCapacity),
  cameraTemparature               (_entry.cameraTemparature),
  physicalFramesDropped           (_entry.physicalFramesDropped),
  logicalFramesDropped            (_entry.logicalFramesDropped),
  scanLinesRead                   (_entry.scanLinesRead),
  scanLinesWritten                (_entry.scanLinesWritten),
  particlesWaitingProcessing      (_entry.particlesWaitingProcessing),
  flowRate                        (_entry.flowRate)
{
}



StatusSnapshot::StatusSnapshot (KKB::DateTime  _timeStamp,
                                kkint32        _timeOffset,
                                kkint16        _logicalFrameProcessorsAvailable,
                                kkint16        _logicalFramesOnQueue,
                                kkint32        _count,
                                kkint32        _particles,
                                float          _cpuUsage,
                                float          _availableCapacity,
                                kkint32        _cameraTemparature,
                                kkint32        _physicalFramesDropped,
                                kkint32        _logicalFramesDropped,
                                kkint32        _scanLinesRead,
                                kkint32        _scanLinesWritten,
                                kkint32        _particlesWaitingProcessing,
                                float          _flowRate
                               ):
  timeStamp                       (_timeStamp),
  timeOffset                      (_timeOffset),
  logicalFrameProcessorsAvailable (_logicalFrameProcessorsAvailable),
  logicalFramesOnQueue            (_logicalFramesOnQueue),
  count                           (_count),
  particles                       (_particles),
  cpuUsage                        (_cpuUsage),
  availableCapacity               (_availableCapacity),
  cameraTemparature               (_cameraTemparature),
  physicalFramesDropped           (_physicalFramesDropped),
  logicalFramesDropped            (_logicalFramesDropped),
  scanLinesRead                   (_scanLinesRead),
  scanLinesWritten                (_scanLinesWritten),
  particlesWaitingProcessing      (_particlesWaitingProcessing),
  flowRate                        (_flowRate)
{
}



StatusSnapshot::~StatusSnapshot ()
{
}



kkMemSize  StatusSnapshot::MemoryConsumedEstimated ()  const
{
  return  sizeof (*this);
}




void  StatusSnapshot::Assign (const StatusSnapshot&  _entry)
{
  timeStamp                       = _entry.timeStamp;
  timeOffset                      = _entry.timeOffset;
  logicalFrameProcessorsAvailable = _entry.logicalFrameProcessorsAvailable;
  logicalFramesOnQueue            = _entry.logicalFramesOnQueue;
  count                           = _entry.count;
  particles                       = _entry.particles;
  cpuUsage                        = _entry.cpuUsage;
  availableCapacity               = _entry.availableCapacity;
  cameraTemparature               = _entry.cameraTemparature;
  physicalFramesDropped           = _entry.physicalFramesDropped;
  logicalFramesDropped            = _entry.logicalFramesDropped;
  scanLinesRead                   = _entry.scanLinesRead;
  scanLinesWritten                = _entry.scanLinesWritten;
  particlesWaitingProcessing      = _entry.particlesWaitingProcessing;
  flowRate                        = _entry.flowRate;
}



void  StatusSnapshot::Assign (const LarcosCounterStats&  _lastEntry,
                              const LarcosCounterStats&  _nextEntry
                             )
{
  cpuUsage = 0.0;
  timeStamp = osGetLocalDateTime ();
  logicalFrameProcessorsAvailable = (float)_nextEntry.LogicalFrameProcessorsAvailable ();
  logicalFramesOnQueue            = (float)_nextEntry.LogicalFramesOnQueue            ();
  cameraTemparature               = _nextEntry.CameraTemparature                      ();
  availableCapacity               = 0.0;

  count                           = _nextEntry.ParticlesCounted           () - _lastEntry.ParticlesCounted      ();
  particles                       = _nextEntry.ParticlesExtracted         () - _lastEntry.ParticlesExtracted    ();
  physicalFramesDropped           = _nextEntry.PhysicalFramesDropped      () - _lastEntry.PhysicalFramesDropped ();
  logicalFramesDropped            = _nextEntry.LogicalFramesDropped       () - _lastEntry.LogicalFramesDropped  ();
  scanLinesRead                   = _nextEntry.ScanLinesRead              () - _lastEntry.ScanLinesRead         ();
  scanLinesWritten                = _nextEntry.ScanLinesWritten           () - _lastEntry.ScanLinesWritten      ();
  particlesWaitingProcessing      = _nextEntry.ParticlesWaitingProcessing ();
  flowRate                        = _nextEntry.FlowRate ();
}  /* Assign */




void  StatusSnapshot::UpdateTimeStamp ()
{
  timeStamp = osGetLocalDateTime ();
}



StatusSnapshotList::StatusSnapshotList (bool _owner):
   KKQueue<StatusSnapshot> (_owner)
{
}



StatusSnapshotList::StatusSnapshotList (const StatusSnapshotList&  _list):
   KKQueue<StatusSnapshot> (_list)
{
}



StatusSnapshotList::~StatusSnapshotList ()
{
}





StatusSnapshotBuffer::StatusSnapshotBuffer ():
  goalie (NULL)
{
  GoalKeeper::Create ("StatusSnapshotList", goalie);
}



StatusSnapshotBuffer::StatusSnapshotBuffer (const StatusSnapshotList&  list):
  goalie (NULL)
{
  GoalKeeper::Create ("StatusSnapshotList", goalie);
}



StatusSnapshotBuffer::~StatusSnapshotBuffer ()
{
  DeleteContents ();
  GoalKeeper::Destroy (goalie);
  goalie = NULL;
}



void  StatusSnapshotBuffer::DeleteContents ()
{
  goalie->StartBlock ();
  iterator  idx;
  for  (idx = begin ();  idx != end ();  ++idx)
    delete (*idx);
  clear ();
  goalie->EndBlock ();
}



void  StatusSnapshotBuffer::AddEntry (StatusSnapshotPtr  snapShot)
{
  goalie->StartBlock ();
  push_back (snapShot);
  goalie->EndBlock ();
}



void  StatusSnapshotBuffer::Reset ()
{
  DeleteContents ();
}



StatusSnapshotListPtr  StatusSnapshotBuffer::Retrieve (KKB::DateTime  rangeStart,
                                                       KKB::DateTime  rangeEnd
                                                      )
{
  goalie->StartBlock ();

  StatusSnapshotListPtr  results = new StatusSnapshotList (false);

  iterator  idx;
  for  (idx = begin ();  idx != end ();  ++idx)
  {
    StatusSnapshotPtr snapshot = *idx;
    if  ((snapshot->TimeStamp () >= rangeStart)  && (snapshot->TimeStamp () <= rangeEnd))
      results->PushOnBack (snapshot);
  }
  goalie->EndBlock ();

  return  results;
}  /* Retrieve */




const StatusSnapshotPtr  StatusSnapshotBuffer::LastEntry ()
{
  StatusSnapshotPtr  result = NULL;
  goalie->StartBlock ();
  int  c = size ();
  if  (c > 0)
    result = (*this)[c-1];
  goalie->EndBlock ();
  return  result;
}  /* LastEntry */






