/* LogicalFrame.cpp -- Represents a single camera frame.
 * Copyright (C) 2011-2013  Kurt Kramer
 * For conditions of distribution and use, see copyright notice in CounterUnManaged.txt
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

#include "ScannerFileEntry.h"
using namespace  KKLSC;

#include "LogicalFrame.h"
using  namespace  CounterUnManaged;



LogicalFrame::LogicalFrame (kkint32  _frameHeight,
                            kkint32  _frameWidth,
                            RunLog&  _logger
                           ):

    logicalFrameEntry      (NULL),
    backGroundPixelTH      (31),
    bytesAllocated         (0),
    flowRateRatio          (1.0f),
    flowRate               (1.0f),

    frameHeight            (_frameHeight),
    frameWidth             (_frameWidth),
    frameHeightUsed        (0),
    frameAreaUsed          (0),
    frameLinesArea         (NULL),
    frameLines             (NULL),
    scanLineNumStart       (0),
    scanLineNumStartCurSF  (0),
    scannerFileEntry       (NULL),
    frameSeqNum            (0)

{
  AllocateMemmory ();
}




LogicalFrame::~LogicalFrame ()
{
  CleanUpMemory ();
}



void  LogicalFrame::AllocateMemmory ()
{
  bytesAllocated = frameWidth * frameHeight;
  frameLinesArea = new uchar[bytesAllocated];

  uchar*   frameLinesAreaPtr = frameLinesArea;
  frameLines = new uchar*[frameHeight];

  for  (kkint32 row = 0;  row < frameHeight;  ++row)
  {
    frameLines [row] = frameLinesAreaPtr;
    frameLinesAreaPtr += frameWidth;
  }
}  /* AllocateMemmory */



void  LogicalFrame::CleanUpMemory()
{
  delete  frameLinesArea;  frameLinesArea   = NULL;
  delete  frameLines;      frameLines       = NULL;
}



kkMemSize  LogicalFrame::MemoryConsumedEstimated ()  const
{
  //                                         frameLinesArea  +                  frameLines
  int  memConsumed = sizeof (LogicalFrame) + bytesAllocated * sizeof(uchar)   + frameHeight * sizeof(uchar*);
  return  memConsumed;
}



void  LogicalFrame::ReAllocateMemory (kkint32  _newFrameHeight,
                                      kkint32  _newFrameWidth
                                     )
{
  CleanUpMemory ();
  frameWidth  = _newFrameWidth;
  if  (_newFrameHeight > frameHeight)
    frameHeight = _newFrameHeight;
  AllocateMemmory ();
}


const KKStr&  LogicalFrame::ScannerFileRootName () const  
{
  if  (scannerFileEntry)
    return scannerFileEntry->RootName ();
  else
    return KKStr::EmptyStr ();
}




void  LogicalFrame::AssignAFrame (LogicalFrameEntryPtr _logicalFrameEntry,
                                  kkint32              _newFrameHeight,
                                  kkint32              _newFrameWidth,
                                  kkint32              _scanLineNumStart,
                                  ScannerFileEntryPtr  _scannerFileEntry,
                                  kkint32              _scanLineNumStartCurSF,
                                  float                _flowRateRatio,
                                  float                _flowRate,
                                  kkint64              _frameSeqNum,
                                  uchar*               _newFrameArea
                                 )
{
  if  ((_newFrameHeight > frameHeight)  ||  (_newFrameWidth != frameWidth))
    ReAllocateMemory (_newFrameHeight, _newFrameWidth);

  logicalFrameEntry     = _logicalFrameEntry;
  scanLineNumStart      = _scanLineNumStart;
  scannerFileEntry      = _scannerFileEntry;
  scanLineNumStartCurSF = _scanLineNumStartCurSF;
  flowRateRatio         = _flowRateRatio;
  flowRate              = _flowRate;
  frameSeqNum           = _frameSeqNum;
  frameHeightUsed       = _newFrameHeight;

  frameAreaUsed = frameHeightUsed * frameWidth;
  KKStr::MemCpy (frameLinesArea, _newFrameArea, frameAreaUsed);
}  /* AssignAFrame */





LogicalFrameList::LogicalFrameList (kkint32 _maxNumFramesAllowed,
                                    kkint32 _frameHeight,
                                    kkint32 _frameWidth,
                                    RunLog& _logger
                                   ):
 

    availableFrames      (),
    waitingToBeProcessed (),
    allFrames            (),
    numFramesAllocated   (0),
    maxNumFramesAllowed  (_maxNumFramesAllowed),
    goalKeeper           (NULL),
    frameHeight          (_frameHeight),
    frameWidth           (_frameWidth)

 {
   GoalKeeperSimple::Create ("LogicalFrameList", goalKeeper);
   goalKeeper->StartBlock ();
   AllocateInitialFrames (_logger);
   goalKeeper->EndBlock ();
 }




LogicalFrameList::~LogicalFrameList ()
{
  goalKeeper->StartBlock ();
  while  (allFrames.size () > 0)
  {
    LogicalFramePtr f = allFrames.back ();
    allFrames.pop_back ();
    delete  f;
    f = NULL;
  }
  goalKeeper->EndBlock ();

  GoalKeeperSimple::Destroy (goalKeeper);
  goalKeeper = NULL;
}



kkMemSize  LogicalFrameList::MemoryConsumedEstimated ()  const
{
  kkMemSize  memConsumed = sizeof (LogicalFrameList);
  LogicalFrameVector::const_iterator  idx;
  for  (idx = allFrames.begin ();  idx != allFrames.end ();  ++idx)
  {
    LogicalFramePtr  f = *idx;
    memConsumed += f->MemoryConsumedEstimated ();
  }
  return  memConsumed;
}



void  LogicalFrameList::AllocateInitialFrames (RunLog& _logger)
{
  while  (numFramesAllocated < maxNumFramesAllowed)
  {
    LogicalFramePtr  f = new LogicalFrame (frameHeight, frameWidth, _logger);
    allFrames.push_back (f);
    availableFrames.push (f);
    ++numFramesAllocated;
  }
}



LogicalFramePtr  LogicalFrameList::GetAvailableFrame (RunLog&  log)
{
  LogicalFramePtr  availableFrame = NULL;
  goalKeeper->StartBlock ();

  if  (availableFrames.size () > 0)
  {
    availableFrame = availableFrames.front ();
    availableFrames.pop ();
  }

  else if  (numFramesAllocated < maxNumFramesAllowed)
  {
    availableFrame = new LogicalFrame (frameHeight, frameWidth, log);
    allFrames.push_back (availableFrame);
    ++numFramesAllocated;
  }

  goalKeeper->EndBlock ();

  return  availableFrame;
}  /* GetAvailableFrame */



void  LogicalFrameList::QueueFrameForProcessing (LogicalFramePtr  _frame)
{
  goalKeeper->StartBlock ();
  waitingToBeProcessed.push (_frame);
  goalKeeper->EndBlock ();
}




void  LogicalFrameList::FrameProcessed (LogicalFramePtr  _frame)
{
  goalKeeper->StartBlock ();
  availableFrames.push (_frame);
  goalKeeper->EndBlock ();
}  /* FrameProcessed */




LogicalFramePtr  LogicalFrameList::GetNextFrameToProcess ()
{
  goalKeeper->StartBlock ();
  LogicalFramePtr  nextFrameToProcess = NULL;

  if  (waitingToBeProcessed.size () > 0)
  {
    nextFrameToProcess = waitingToBeProcessed.front ();
    waitingToBeProcessed.pop ();
  }

  goalKeeper->EndBlock ();

  return  nextFrameToProcess;
}



kkint32 LogicalFrameList::NumWaitingToBeProcessed ()  const
{
  return  (kkint32)waitingToBeProcessed.size ();
}
