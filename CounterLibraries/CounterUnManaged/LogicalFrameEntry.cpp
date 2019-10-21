/* LogicalFrameEntry.cpp -- Represents a single camera frame.
 * Copyright (C) 2011-2011  Kurt Kramer
 * For conditions of distribution and use, see copyright notice in KKLineScanner.h
 */
#include "FirstIncludes.h"

#include <deque>
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


#include "LogicalFrameEntry.h"
using  namespace  CounterUnManaged;



LogicalFrameEntry::LogicalFrameEntry ():
    frameNum         (0),
    height           (0),
    scannerFile      (NULL),
    scanLineNumStart (0),
    byteOffset       (0),
    count            (0),
    flowRateRatio    (0.0f),
    flowRate         (0.0f)
{
}



LogicalFrameEntry::LogicalFrameEntry (const LogicalFrameEntry&  _entry):
    frameNum         (_entry.frameNum),
    height           (_entry.height),
    scannerFile      (_entry.scannerFile),
    scanLineNumStart (_entry.scanLineNumStart),
    byteOffset       (_entry.byteOffset),
    count            (_entry.count),
    flowRateRatio    (_entry.flowRateRatio),
    flowRate         (_entry.flowRate)
{
}



LogicalFrameEntry::LogicalFrameEntry (kkint32              _frameNum,
                                      kkint32              _height,
                                      ScannerFileEntryPtr  _scannerFile,
                                      kkint32              _scanLineNumStart,
                                      kkint64              _byteOffset,
                                      kkint32              _count,
                                      float                _flowRateRatio,
                                      float                _flowRate
                                     ):
    frameNum         (_frameNum),
    height           (_height),
    scannerFile      (_scannerFile),
    scanLineNumStart (_scanLineNumStart),
    byteOffset       (_byteOffset),
    count            (_count),
    flowRateRatio    (_flowRateRatio),
    flowRate         (_flowRate)
{
}



LogicalFrameEntry::~LogicalFrameEntry ()
{
}



kkMemSize  LogicalFrameEntry::MemoryConsumedEstimated ()  const
{
  return sizeof (*this);
}



void  LogicalFrameEntry::Assign (const LogicalFrameEntry&   _entry)
{
  frameNum         = _entry.frameNum;
  height           = _entry.height;
  scannerFile      = _entry.scannerFile;
  scanLineNumStart = _entry.scanLineNumStart;
  byteOffset       = _entry.byteOffset;
  count            = _entry.count;
  flowRateRatio    = _entry.flowRateRatio;
  flowRate         = _entry.flowRate;
}



void  LogicalFrameEntry::Assign (kkint32              _frameNum,
                                 kkint32              _height,
                                 ScannerFileEntryPtr  _scannerFile,
                                 kkint32              _scanLineNumStart,
                                 kkint64              _byteOffset,
                                 kkint32              _count,
                                 float                _flowRateRatio,
                                 float                _flowRate
                                )
{
  frameNum         = _frameNum;
  height           = _height;
  scannerFile      = _scannerFile;
  scanLineNumStart = _scanLineNumStart;
  byteOffset       = _byteOffset;
  count            = _count;
  flowRateRatio    = _flowRateRatio;
  flowRate         = _flowRate;
}



LogicalFrameEntryList::LogicalFrameEntryList (bool _owner):
   KKQueue<LogicalFrameEntry> (_owner)
{
}



LogicalFrameEntryList::LogicalFrameEntryList (const LogicalFrameEntryList&  _list):
   KKQueue<LogicalFrameEntry> (_list)
{
}


 
LogicalFrameEntryList::~LogicalFrameEntryList ()
{
}



kkMemSize  LogicalFrameEntryList::MemoryConsumedEstimated ()  const
{
  kkMemSize  memoryConsumedEstimated = sizeof (*this);
  if  (Owner ())
  {
    const_iterator idx;
    for  (idx = begin ();  idx != end ();  ++idx)
      memoryConsumedEstimated += (*idx)->MemoryConsumedEstimated ();
  }
  return memoryConsumedEstimated;
}



LogicalFrameEntryQueue::LogicalFrameEntryQueue (bool           _owner,
                                                const  KKStr&  _queueName
                                               ):
   deque<LogicalFrameEntryPtr> (),
   goalie (NULL),
   owner  (_owner)
{
  GoalKeeper::Create ("LogicalFrameEntryQueue_" + _queueName, goalie);
}



LogicalFrameEntryQueue::~LogicalFrameEntryQueue ()
{
  if  (owner)
  {
    LogicalFrameEntryPtr  entry = PopFromBack ();
    while  (entry)
    {
      delete  entry;
      entry = NULL;
      entry = PopFromBack ();
    }
  }
  GoalKeeper::Destroy (goalie);
  goalie = NULL;
}



kkMemSize  LogicalFrameEntryQueue::MemoryConsumedEstimated ()  const
{
  kkMemSize  memoryConsumedEstimated = sizeof (*this);
  if  (owner)
  {
    const_iterator idx;
    for  (idx = begin ();  idx != end ();  ++idx)
      memoryConsumedEstimated += (*idx)->MemoryConsumedEstimated ();
  }
  return memoryConsumedEstimated;
}



void  LogicalFrameEntryQueue::PushOnFront (LogicalFrameEntryPtr  entry)
{
  goalie->StartBlock ();
  this->push_front (entry);
  goalie->EndBlock ();
}



LogicalFrameEntryPtr  LogicalFrameEntryQueue::PopFromBack ()
{
  LogicalFrameEntryPtr entry = NULL;

  goalie->StartBlock ();
  if  (size () > 0)
  {
    entry = this->back ();
    pop_back ();
  }
  goalie->EndBlock ();

  return  entry;
}



kkint32  LogicalFrameEntryQueue::QueueSize ()  const
{
  kkint32  queueSize = 0;
  goalie->StartBlock ();
  queueSize = (kkint32)size ();
  goalie->EndBlock ();
  return  queueSize;
}
