#include "StdAfx.h"
#include "FirstIncludes.h"

#include <stdio.h>
#include <math.h>

#include <ctype.h>
#include <time.h>
#include <string>
#include "MemoryDebug.h"
using namespace std;

#include "KKStr.h"
using namespace  KKB;

#include "LarcosCounterStats.h"
using namespace LarcosCounterUnManaged;


#include "LarcosCounterStatsManaged.h"
#include "UmiKKStr.h"
using namespace  LarcosCounterManaged;


using namespace System;
using namespace System::Threading;
//using namespace System::Windows::Forms;



LarcosCounterStatsManaged::LarcosCounterStatsManaged (LarcosCounterStatsPtr  stats):
    bytesWritten                   (0),
    bytesWrittenAllSegs            (0),
    cropLeft                       (0),
    cropRight                      (0),
    curAnalogGain                  (0.0f),
    curDigitalGain                 (0),
    curSensitivity                 (nullptr),
    diskWritingSeqNumBreaks        (0),
    frameRate                      (0),
    cameraTemparature              (-1),
    logicalFrameProcessorsAvailable (0),
    logicalFramesCreated           (0),
    logicalFramesDropped           (0),
    logicalFramesOnQueue           (0),
    logicalFramesProcessed         (0),
    particlesCounted               (0),
    particlesExtracted             (0),
    particlesWaitingProcessing     (0),
    physicalSeqNumsSkipped         (0),
    physicalFramesDropped          (0),
    physicalFramesProcessed        (0),
    physicalFramesRead             (0),
    physicalFramesWaitingToProcess (0),
    scanLinesRead                  (0),
    scanLinesWritten               (0),
    scanRate                       (0),
    totalBytesToRead               (0),
    totalLostPackets               (0),
    waitingToWriteToDisk           (0)


{
  if  (stats)
  {
    bytesWritten                    = stats->BytesWritten                   ();
    bytesWrittenAllSegs             = stats->BytesWrittenAllSegs            ();
    cropLeft                        = stats->CropLeft                       ();
    cropRight                       = stats->CropRight                      ();
    curAnalogGain                   = stats->CurAnalogGain                  ();
    curDigitalGain                  = stats->CurDigitalGain                 ();
    curSensitivity                  = UmiKKStr::KKStrToSystenStr (stats->CurSensitivity ());
    frameRate                       = stats->FrameRate                      ();
    cameraTemparature               = stats->CameraTemparature              ();
    logicalFrameProcessorsAvailable = stats->LogicalFrameProcessorsAvailable ();
    logicalFramesCreated            = stats->LogicalFramesCreated           ();
    logicalFramesDropped            = stats->LogicalFramesDropped           ();
    logicalFramesOnQueue            = stats->LogicalFramesOnQueue           ();
    logicalFramesProcessed          = stats->LogicalFramesProcessed         ();
    diskWritingSeqNumBreaks         = stats->DiskWritingSeqNumBreaks        ();
    particlesExtracted              = stats->ParticlesExtracted             ();
    particlesWaitingProcessing      = stats->ParticlesWaitingProcessing     ();
    particlesCounted                = stats->ParticlesCounted               ();
    physicalFramesDropped           = stats->PhysicalFramesDropped          ();
    physicalFramesProcessed         = stats->PhysicalFramesProcessed        ();
    physicalFramesRead              = stats->PhysicalFramesRead             ();
    physicalFramesWaitingToProcess  = stats->PhysicalFramesWaitingToProcess ();
    physicalSeqNumsSkipped          = stats->PhysicalSeqNumsSkipped         ();
    scanLinesRead                   = stats->ScanLinesRead                  ();
    scanLinesWritten                = stats->ScanLinesWritten               ();
    scanRate                        = stats->ScanRate                       ();
    totalBytesToRead                = stats->TotalBytesToRead               ();
    totalLostPackets                = stats->TotalLostPackets               ();
    waitingToWriteToDisk            = stats->WaitingToWriteToDisk           ();
  }
}



LarcosCounterStatsManaged::!LarcosCounterStatsManaged ()
{
  CleanUpMemory ();
}



LarcosCounterStatsManaged::~LarcosCounterStatsManaged ()
{
  this->!LarcosCounterStatsManaged ();
}


void  LarcosCounterStatsManaged::CleanUpMemory ()
{
}
