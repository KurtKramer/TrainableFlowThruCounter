/* LarcosCounterStats.cpp -- Represents a Snapshot of stats being maintained by the Larcos Camera Manager.
 * Copyright (C) 2011-2013  Kurt Kramer
 * For conditions of distribution and use, see copyright notice in LarcosCounterUnManaged.txt
 */
#include "FirstIncludes.h"
#include <errno.h>
#include <istream>
#include <iostream>
#include <vector>
#include "MemoryDebug.h"
using namespace std;


#include "KKBaseTypes.h"
using namespace KKB;



#include "LarcosCounterStats.h"
using  namespace  LarcosCounterUnManaged;

LarcosCounterStats::LarcosCounterStats ():
    bytesWritten                    (0),
    bytesWrittenAllSegs             (0),
    cameraTemparature               (-1),
    cropLeft                        (0),
    cropRight                       (0),
    curAnalogGain                   (0.0),
    curDigitalGain                  (0),
    diskWritingSeqNumBreaks         (0),
    flowRate                        (0.0f),
    frameRate                       (0.0f),
    logicalFrameProcessorsAvailable (0),
    logicalFramesCreated            (0),
    logicalFramesDropped            (0),
    logicalFramesOnQueue            (0),
    logicalFramesProcessed          (0),
    particlesCounted                (0),
    particlesExtracted              (0),
    particlesWaitingProcessing      (0),
    physicalFramesDropped           (0),
    physicalFramesProcessed         (0),
    physicalFramesRead              (0),
    physicalFramesWaitingToProcess  (0),
    physicalSeqNumsSkipped          (0),
    scanLinesRead                   (0),
    scanLinesWritten                (0),
    scanRate                        (0.0f),
    totalBytesToRead                (0),
    totalLostPackets                (0),
    waitingToWriteToDisk            (0)
{
}


LarcosCounterStats::~LarcosCounterStats ()
{
}


void  LarcosCounterStats::Reset ()
{
  bytesWritten                    = 0;
  bytesWrittenAllSegs             = 0;
  cropLeft                        = 0;
  cropRight                       = 0;
  curAnalogGain                   = 0.0;
  curDigitalGain                  = 0;
  diskWritingSeqNumBreaks         = 0;
  flowRate                        = 0.0f;
  frameRate                       = 0.0f;
  cameraTemparature               = -1;
  logicalFrameProcessorsAvailable = 0;
  logicalFramesCreated            = 0;
  logicalFramesDropped            = 0;
  logicalFramesOnQueue            = 0;
  logicalFramesProcessed          = 0;
  particlesCounted                = 0;
  particlesExtracted              = 0;
  particlesWaitingProcessing      = 0;
  physicalFramesDropped           = 0;
  physicalFramesProcessed         = 0;
  physicalFramesRead              = 0;
  physicalFramesWaitingToProcess  = 0;
  physicalSeqNumsSkipped          = 0;
  scanLinesWritten                = 0;
  scanRate                        = 0.0f;
  totalBytesToRead                = 0;
  totalLostPackets                = 0;
  waitingToWriteToDisk            = 0;
}



void  LarcosCounterStats::Assign (const LarcosCounterStats& stats)
{
  bytesWritten                    = stats.bytesWritten;
  bytesWrittenAllSegs             = stats.bytesWrittenAllSegs;
  cropLeft                        = stats.cropLeft;
  cropRight                       = stats.cropRight;
  curAnalogGain                   = stats.curAnalogGain;
  curDigitalGain                  = stats.curDigitalGain;
  diskWritingSeqNumBreaks         = stats.diskWritingSeqNumBreaks;
  flowRate                        = stats.flowRate;
  frameRate                       = stats.frameRate;
  cameraTemparature               = stats.cameraTemparature;
  logicalFrameProcessorsAvailable = stats.logicalFrameProcessorsAvailable;
  logicalFramesCreated            = stats.logicalFramesCreated;
  logicalFramesDropped            = stats.logicalFramesDropped;
  logicalFramesOnQueue            = stats.logicalFramesOnQueue;
  logicalFramesProcessed          = stats.logicalFramesProcessed;
  particlesCounted                = stats.particlesCounted;
  particlesExtracted              = stats.particlesExtracted;
  particlesWaitingProcessing      = stats.particlesWaitingProcessing;
  physicalFramesDropped           = stats.physicalFramesDropped;
  physicalFramesProcessed         = stats.physicalFramesProcessed;
  physicalFramesRead              = stats.physicalFramesRead;
  physicalFramesWaitingToProcess  = stats.physicalFramesWaitingToProcess;
  physicalSeqNumsSkipped          = stats.physicalSeqNumsSkipped;
  scanLinesRead                   = stats.scanLinesRead;
  scanLinesWritten                = stats.scanLinesWritten;
  scanRate                        = stats.scanRate;
  totalBytesToRead                = stats.totalBytesToRead;
  totalLostPackets                = stats.totalLostPackets;
  waitingToWriteToDisk            = stats.waitingToWriteToDisk;
}
