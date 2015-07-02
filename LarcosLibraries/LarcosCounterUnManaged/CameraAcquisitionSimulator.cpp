/* CameraAcquisitionSimulator.cpp --
 * Copyright (C) 2011-2013  Kurt Kramer
 * For conditions of distribution and use, see copyright notice in LarcosCounterUnManaged.txt
 */
#include "FirstIncludes.h"

#include <errno.h>
#include <istream>
#include <iostream>
#include <queue>
#include <map>
#include <vector>
#if  defined(OS_WINDOWS)
#include <windows.h>
#endif
#include "MemoryDebug.h"
using namespace std;


#include "KKBaseTypes.h"
#include "GoalKeeper.h"
#include "OSservices.h"
#include "Raster.h"
#include "RunLog.h"
using namespace KKB;


#include "ScannerFile.h"
using namespace  KKLSC;

#include "CameraAcquisitionSimulator.h"
#include "CameraFrame.h"
#include "CameraFrameBuffer.h"
#include "CameraAcquisitionSimulator.h"
#include "CameraParameters.h"
#include "LarcosCounterManager.h"
using  namespace  LarcosCounterUnManaged;



CameraAcquisitionSimulator::CameraAcquisitionSimulator 
                                 (LarcosCounterManagerPtr _manager,
                                  bool                    _playingBackRealTime,
                                  CameraFrameBufferPtr    _frameBuffer,
                                  const KKStr&            _fileName,
                                  FlowMeterTrackerPtr     _flowMeter,
                                  MsgQueuePtr             _msgQueue,
                                  const KKStr&            _threadName
                                 ):

   CameraAcquisition (_manager, _frameBuffer, _flowMeter, _msgQueue, _threadName),
   colCount              (NULL),
   curFileName           (),
   fileName              (_fileName),
   lineSize              (0),
   pixelsInRow           (0),
   playingBackRealTime   (false),
   scannerFilesToProcess (true),
   sf                    (NULL),
   totalBytesRead        (0),
   totalBytesToRead      (0)

{
  AllocateDynamicMemory ();
  playingBackRealTime = _playingBackRealTime;
}




CameraAcquisitionSimulator::~CameraAcquisitionSimulator ()
{
  delete  colCount;
  colCount = NULL;
}



void  CameraAcquisitionSimulator::AllocateDynamicMemory ()
{
  delete  colCount;
  if  (FrameWidth () > 0)
    colCount = new kkuint32 [FrameWidth () + 10];
  else
    colCount = NULL;
}



void  CameraAcquisitionSimulator::BuildListOfScannerFilesToProcess ()
{
  totalBytesToRead = 0;
  
  scannerFilesToProcess.DeleteContents ();
  scannerFilesToProcess.PushOnBack (new KKStr (fileName));

  KKStr  baseNameInterestedIn = osRemoveExtension (fileName);
  KKStr  baseNameExt = osGetFileExtension (fileName);
  KKStr  dirName = osGetPathPartOfFile (fileName);

  kkint32  numSeqSkipped = 0;

  for  (int x = 0;  (x < 999)  &&  (numSeqSkipped < 10);  ++x)
  {
    KKStr  candidateName = baseNameInterestedIn + "_" + StrFormatInt (x, "000") + "." + baseNameExt;
    if  (osFileExists (candidateName))
    {
      scannerFilesToProcess.PushOnBack (new KKStr (candidateName));
      numSeqSkipped = 0;
    }
    else
    {
      ++numSeqSkipped;
    }
  }

  {
    KKStrList::const_iterator  idx;
    for  (idx = scannerFilesToProcess.begin ();  idx != scannerFilesToProcess.end ();  ++idx)
    {
      KKStr  fn = *idx;
      totalBytesToRead += osGetFileSize (fn);
    }
  }
}  /* BuildListOfScannerFilesToProcess */



void  CameraAcquisitionSimulator::OpenNextScannerFile ()
{
  delete  sf;  sf = NULL;

  while  (sf == NULL)
  {
    KKStrPtr  nextFileName = scannerFilesToProcess.PopFromFront ();
    if  (nextFileName == NULL)
      break;

    curFileName = *nextFileName;
    sf = ScannerFile::CreateScannerFile (curFileName, log);
    if  (sf == NULL)
    {
      log.Level (-1) << endl << "CameraAcquisitionSimulator::OpenNextScannerFile   ***ERROR***    Could not open Scanner File: " << curFileName << endl << endl;
    }
    else
    {
      log.Level (10) << endl << "CameraAcquisitionSimulator::OpenNextScannerFile    Opened Scanner File: " << curFileName << endl << endl;
      RequestedScanRate (sf->ScanRate ());
    }

    delete  nextFileName;
    nextFileName = NULL;
  }

  if  (sf)
  {
    FrameHeightWidth (sf->FrameHeight (), sf->PixelsPerScanLine ());
    curScanRate = sf->ScanRate ();
    cameraParams->ScanRate (curScanRate);
    AllocateDynamicMemory ();
  }
}  /* OpenNextScannerFile */





void  CameraAcquisitionSimulator::GetNextFrame2 (uchar*  buffer,
                                                 bool&   eof
                                                )
{
  if  ((sf == NULL)  ||  (sf->Eof ()))
  {
    eof = true;
    return;
  }

  kkint32 totalArea = FrameHeight () * FrameWidth ();
  KKStr::MemSet (buffer, 0, totalArea);

  uchar*  bufferPtr = buffer;
  kkint32 row = 0;

  kkint32  frameHeight = FrameHeight ();
  kkint32  frameWidth  = FrameWidth  ();

  sf->GetNextLine (bufferPtr, frameWidth, lineSize, colCount, pixelsInRow);
  if  (sf->Eof ())
  {
    eof = true;
    return;
  }
  ++row;

  eof = false;

  while  ((row < frameHeight)  &&  (!sf->Eof ()))
  {
    bufferPtr += frameWidth;
    ++row;
    sf->GetNextLine (bufferPtr, frameWidth, lineSize, colCount, pixelsInRow);
  }

  scanLinesRead += row;

  // Because we are simulating a line scan camera we need to complement the data such that BackGround = 255; and Foreground = 0.
  if  (Manager ()->EmbeddedFlowMeter ())
  {
    bufferPtr = buffer;
    for  (kkint32  z = 0;  z < frameHeight;  ++z)
    {
      uchar*  bufferEndPtr = bufferPtr + frameWidth;
      bufferPtr  += 4;
      while  (bufferPtr != bufferEndPtr)
      {
        *bufferPtr = (255 - (*bufferPtr));
        ++bufferPtr;
      }
    }
  }
  else
  {
    bufferPtr = buffer;
    uchar*  bufferEndPtr = bufferPtr + totalArea;
    while  (bufferPtr != bufferEndPtr)
    {
      *bufferPtr = (255 - (*bufferPtr));
      ++bufferPtr;
    }
  }
}  /* GetNextFrame2 */




void  CameraAcquisitionSimulator::ComplementDataBlock (void*  block,
                                                       kkint32  blockSize
                                                      )
{
  kkint32  num8ByteBlocks = blockSize / 8;
  kkint32  num8ByteArea = 8 * num8ByteBlocks;
  kkint32  numLeftOver = blockSize - num8ByteArea;

  kkint32 x = 0;
  kkuint64*  ptr = (kkuint64*)block;
  for  (x = 0;  x < num8ByteBlocks;  ++x)
    *ptr = ~(*ptr);


  uchar*  ptr2 = (uchar*)block + num8ByteArea;
  for  (x = 0;  x < numLeftOver;  ++x)
    *ptr2 = ~(*ptr2);
}  /* ComplementDataBlock */



void  CameraAcquisitionSimulator::Run ()
{
  log.Level (10) << "CameraAcquisitionSimulator::Run" << endl;
  Status (ThreadStatus::Running);
  StartStatus (StartStatusType::Connecting, "");

  bool  embeddedFlowMeter = Manager ()->EmbeddedFlowMeter ();

  // We will sleep for 1 seconds to give the rest of the application a 
  // chance to get setup before we start sending then scanner data.
  osSleep (1.0f);

  totalBytesRead = 0;

  // As we complete scanner files we will add their file size to 'totalBytesPrevScannerFiles'; and 
  // 'totalBytesRead' will equal totalBytesPrevScannerFiles + sf->
  kkint64  totalBytesPrevScannerFiles = 0;

  BuildListOfScannerFilesToProcess ();

  Manager ()->TotalBytesToRead (totalBytesToRead);

  OpenNextScannerFile ();

  totalLostPackets = 0;
  // Acquire images until the user instructs us to stop
  log.Level (10) << "CameraAcquisitionSimulator::Run   Acquisition Loop" << endl;

  bool  embeddedFlowMeterDataPresent = false;
  if  (sf->FileFormat ()  == ScannerFile::Format::sfSimple)
  {
    if  (!sf->FlatFieldEnabled ())
    {
      if  (Manager ()->EmbeddedFlowMeter ())
        embeddedFlowMeterDataPresent = true;
    }
  }


  Status (ThreadStatus::Running);

  float     pausePerFrameInSecs     = (float)((double)sf->FrameHeight () / curScanRate); 
  kkuint32  pausePerFrameInMiliSecs = (kkuint32)(pausePerFrameInSecs * 1000) - 1;
  kkuint64  startOfPeriodMiliSecs   = osGetSystemTimeInMiliSecs ();

  while  ((sf != NULL)  &&  (!TerminateFlag ())  &&  (!ShutdownFlag ()))
  {
    if  ((sf->FrameHeight () != FrameHeight ())  ||  ((int)(sf->PixelsPerScanLine ()) != FrameWidth ()))
    {
      FrameHeightWidth (sf->FrameHeight (), sf->PixelsPerScanLine ());
      AllocateDynamicMemory ();
    }

    StartStatus (StartStatusType::Connected, "Connected to Scanner File[" + sf->FileName () + "]");

    Manager ()->StartingToReadNewScannerFile (ScannerFileEntry::GetOrCreateScannerFileEntry (sf));

    if  (curScanRate == 0)
      curScanRate = 20000;

    kkint32  bufferArea = FrameHeight () * FrameWidth ();
    uchar*  buffer = new uchar[bufferArea + 2048];

    bool  eof = false;

    flowMeterCounter = 0;
    flowMeterCounterScanLine = 0;

    kkint32  lastFlowRateCount = 0;
    kkint32  scanLinesCounted = 0;

    //// For building a PL demo file we want to skip first 150K scan lines to get past FlatField correction
    //for  (int xxx = 0;  (xxx < (150000 / 2048))  &&  (!eof);  ++xxx)
    //  GetNextFrame2 (buffer, eof);

    GetNextFrame2 (buffer, eof);
    while  ((!TerminateFlag ())  &&  (!ShutdownFlag ())  &&  (!eof))
    {
      // because data is stored with (backGround == 0) and (foreground == 255) in scanner files we need to
      // complement it all to simulate the data hat a camera would provide.
      if  (embeddedFlowMeter)
      {
        uchar*  bufferLinePtr = buffer;
        for  (kkint32 r = 0;  r < FrameHeight ();  ++r)
        {
          ComplementDataBlock (bufferLinePtr + 4, FrameWidth () - 4);
          bufferLinePtr += FrameWidth ();
        }
      }
      else
      {
        ComplementDataBlock (buffer, bufferArea);
      }

      kkuint32  tempFlowMeterCounter =  sf->FlowMeterCounter ();
      if  (tempFlowMeterCounter != flowMeterCounter)
      {
        flowMeterCounter = tempFlowMeterCounter;
        flowMeterCounterScanLine = sf->FlowMeterCounterScanLine ();
        FlowMeterCounterUpdate (flowMeterCounterScanLine, flowMeterCounter);
      }

      ++physicalFramesRead;

      float  flowRate = 0.0f;
      float  flowRateRatio = 0.0f;

      flowMeter->GetFlowRateTrend (flowRate, flowRateRatio);

      if  (playingBackRealTime)
      {

        MMRESULT zed = timeBeginPeriod (1);
        if  (zed == TIMERR_NOCANDO)
        {
          kkuint64  xxx = zed;
        }

        // We need to sleep long enough to emulate frame rate of camera,
        kkuint64  endOfLastFrameInMiliSecs = (kkuint32)(osGetSystemTimeInMiliSecs ());
        kkuint64  miliSecsSoFar = (kkuint32)(endOfLastFrameInMiliSecs - startOfPeriodMiliSecs);
        while  (pausePerFrameInMiliSecs > miliSecsSoFar)
        {
          kkuint32 timeLeft = (kkint32)(pausePerFrameInMiliSecs - miliSecsSoFar);
          osSleepMiliSecs (1);
          endOfLastFrameInMiliSecs = osGetSystemTimeInMiliSecs ();
          miliSecsSoFar = (kkuint32)(endOfLastFrameInMiliSecs - startOfPeriodMiliSecs);
        }
        startOfPeriodMiliSecs = endOfLastFrameInMiliSecs;

        timeEndPeriod (1);

      }
      else
      {
        while  ((frameBuffer->NumAvailable () < 2)  && (!TerminateFlag ()))
          osSleepMiliSecs (10);
      }

      frameBuffer->AddFrame ((uchar*)buffer, bufferArea, FrameHeight (), FrameWidth (), flowMeterCounter, flowMeterCounterScanLine, flowRateRatio, flowRate);
      totalBytesRead = totalBytesPrevScannerFiles + sf->FrameBufferFileOffsetNext ();
      Manager ()->TotalBytesRead (totalBytesRead);

      GetNextFrame2 (buffer, eof);

      if  (!eof)
      {
        if  (embeddedFlowMeterDataPresent)
        {
          const uchar* sld = buffer;
          for (int rrr = 0;  rrr < FrameHeight ();  ++rrr)
          {
            kkint32 flowRateCount = sld[0] + sld[1] * 256 + (sld[2] * 256 * 256) + (sld[3] * 256 * 256 * 256);
            if  (flowRateCount != lastFlowRateCount)
            {
              FlowMeterCounterUpdate (scanLinesCounted, flowRateCount);

              lastFlowRateCount = flowRateCount;
            }

            sld += FrameWidth ();
            ++scanLinesCounted;
          }
        }
      }
    }

    totalBytesPrevScannerFiles += sf->FileSizeInBytes ();

    delete  buffer;  buffer = NULL;
    delete  sf;      sf = NULL;

    if  ((!TerminateFlag ())  &&  (!ShutdownFlag ()))
      OpenNextScannerFile ();
  }

  Status (ThreadStatus::Stopping);
  
  delete  sf;
  sf = NULL;

  StartStatus (StartStatusType::Disconnected, "Simulated Camera Stopped.");

  if  ((!TerminateFlag ())  &&  (!ShutdownFlag ()) &&  (Manager ()->CurState () == LarcosCounterManager::CounterState::PlayingBack))
  {
    bool  successful = false;
    KKStr  errMsg = "";
    Manager ()->StopButtonPressed (TerminateFlag (), successful, errMsg);
  }

  log.Level (10) << "CameraAcquisitionSimulator::Run   Exiting  TerminateFlag: " << TerminateFlag () <<  "  ShutdownFlag: " << ShutdownFlag () << endl;

  return;
}  /* Run */






void  CameraAcquisitionSimulator::ResetCounts ()
{
}



void  CameraAcquisitionSimulator::PerformAutoGainProcedure ()
{
}
