/* DiskWriterThread.cpp -- Responsible for writing camera data to disk.
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
#include "ImageIO.h"
#include "OSservices.h"
#include "Raster.h"
using namespace KKB;

#include "ScannerFile.h"
#include "ScannerFileSimple.h"
#include "ScannerFile3BitEncoded.h"
#include "Variables.h"
using namespace  KKLSC;

#include "DiskWriterThread.h"
#include "CameraFrame.h"
#include "CameraFrameBuffer.h"
#include "CameraThread.h"
#include "LarcosCounterManager.h"
#include "LarcosCounterStats.h"
using  namespace  LarcosCounterUnManaged;



DiskWriterThread::DiskWriterThread (LarcosCounterManagerPtr  _manager,
                                    CameraFrameBufferPtr     _cameraBuffer,
                                    MsgQueuePtr              _msgQueue,
                                    const KKStr&             _scannerFileName,
                                    ScannerFile::Format      _format,
                                    kkint32                  _frameHeight,
                                    kkint32                  _frameWidth,
                                    const KKStr&             _threadName
                                   ):
    CameraThread (_manager, _threadName, _msgQueue),
    bytesWritten              (0),
    bytesWrittenAllSegs       (0),
    bytesWrittenCompletedSegs (0),
    cameraBuffer              (_cameraBuffer),
    curScannerFileRootName    (),
    dwStatus                  (DiskWritingStatus::Null),
    firstFrameYet             (false),
    format                    (_format),
    frameSeqNumLast           (0),
    frameHeight               (_frameHeight),
    frameWidth                (_frameWidth),
    framesWritten             (0),
    msgs                      (NULL),  /**< Messages that are to be added to the current scanner file position. */
    msgsAreQueued             (false),
    numFramesSinceLastSkip    (0),
    numSkippedFrames          (0),
    dataIsToBeRecorded        (true),
    scannerFileBaseCount      (0),
    scannerFileBaseName       (),
    scannerFileBaseExtension  (),
    scannerFileDir            (),
    scannerFileSize           (0),
    scannerFileSizeMax        ((kkint64)16 * (kkint64)(1024 * 1024 * 1024)),
    //scannerFileSizeMax        (10 * 1024 * 1024),
    scanLinesWritten          (0),
    startNewScannerFile       (false),
    startNewScannerFileName   ()
{
  osCreateDirectoryPath (Variables::ScannerFilesDefaultDir ());
  msgs = new KKB::MsgQueue ("DiskWriterThread");

  startNewScannerFileName  = _scannerFileName;
  scannerFileBaseCount     = 0;
  startNewScannerFile      = true;
  scannerFileDir           = osGetPathPartOfFile (_scannerFileName);
  scannerFileBaseName      = osGetRootName       (_scannerFileName);
  scannerFileBaseExtension = osGetFileExtension  (_scannerFileName);
}



DiskWriterThread::~DiskWriterThread ()
{
  delete  msgs;  
  msgs = NULL;
}



kkint32 DiskWriterThread::MemoryConsumedEstimated ()
{
  return  sizeof (DiskWriterThread);
}


void  DiskWriterThread::GetStats (LarcosCounterStats&  stats)  const
{
  bytesWrittenAllSegs = bytesWrittenCompletedSegs + bytesWritten;
  stats.BytesWritten            (bytesWritten);
  stats.BytesWrittenAllSegs     (bytesWrittenAllSegs);
  stats.ScanLinesWritten        (scanLinesWritten);
  stats.DiskWritingSeqNumBreaks (numSkippedFrames);
}



bool  DiskWriterThread::WeAreRecordingToDisk ()
{
  return ((Status () == ThreadStatus::Running)  &&  (DiskStatus () == DiskWritingStatus::Recording));
}  /* WeAreRecordingToDisk */




CameraFramePtr  DiskWriterThread::GetNextFrame ()
{
  kkuint32  numMiliSecsWaited = 0;
  CameraFramePtr  frame = cameraBuffer->GetNextFrameToWriteToDisk ();
  while   ((frame == NULL)  &&  (!TerminateFlag ())  &&  (!ShutdownFlag ()))
  {
    if  (numMiliSecsWaited > 1000)
      dwStatus = DiskWritingStatus::NotRecording;
    osSleep (0.1f);
    numMiliSecsWaited += 100;
    frame = cameraBuffer->GetNextFrameToWriteToDisk ();
  }

  if  (!frame)
    return NULL;

  if  (!firstFrameYet)
  {
    firstFrameYet = true;
  }
  else 
  {
    kkint64  numFramesSkipped = frame->SeqNum () - (frameSeqNumLast + 1);
    if  (numFramesSkipped < 1)
    {
      ++numFramesSinceLastSkip;
      if  ((dwStatus == DiskWritingStatus::DroppingFrames)  &&  (numFramesSkipped > 200))
        dwStatus = DiskWritingStatus::Recording;
    }
    else
    {
      KKStr  statusMsg (100);
      log.Level (-1) << endl
                     << "DiskWriterThread::GetNextFrame   ***ERROR**    Frames have been skipped." << endl
                     << "          LastFrameSeqNum: " << frameSeqNumLast << "] frame->SeqNum: " << frame->SeqNum () << endl
                     << endl;
      dwStatus = DiskWritingStatus::DroppingFrames;
      numSkippedFrames += (kkint32)numFramesSkipped;
      numFramesSinceLastSkip = 0;
    }
  }
  frameSeqNumLast = frame->SeqNum ();

  return  frame;
 }  /* GetNextFrame */



void  DiskWriterThread::AddCameraLineToHeaderFields (ScannerFilePtr  scannerFile)
{
  VectorUcharPtr  highPoints = Manager ()->CameraHighPoints ();
  if  ((!highPoints)  ||  (!scannerFile))
    return;

  kkint32  width = highPoints->size ();

  KKStr  highPointsStr (width * 4 + 10);  // 3 Bytes per pixel plus one byte for comma.
  highPointsStr << (*highPoints)[0];
  for  (kkint32 x = 1;  x < width;  ++x)
    highPointsStr << "," << (*highPoints)[x];
  scannerFile->AddHeaderField ("CameraLine", highPointsStr);

  delete  highPoints;
  highPoints = NULL;
}  /* AddCameraLineToHeaderFields */



void  DiskWriterThread::Run ()
{
  log.Level (10) << "DiskWriterThread::Run   Thread Started." << endl;
  Status (ThreadStatus::Running);
  if  (!cameraBuffer)
  {
    Status (ThreadStatus::Stopping);
    return;
  }


  ScannerFilePtr  scannerFile = NULL;
  scannerFileSize = 0;

  kkuint32  lastFlowMeterCounter = 0;

  curScannerFileName = "";

  ofstream*  flowMeterFile = NULL;

  CameraFramePtr  frame = GetNextFrame ();
  while  (!TerminateFlag ())
  {
    if  ((frame == NULL)  &&  (ShutdownFlag ()))
      break;

    if  (frame->ScannerFileRootName () != curScannerFileRootName)
    {
      if  (true)
      {
        delete  flowMeterFile;
        KKStr  flowMeterFileName =  KKB::osRemoveExtension (frame->ScannerFileFullName ()) + "_FlowMeterData.txt";
        flowMeterFile = new ofstream (flowMeterFileName.Str ());
      }

      startNewScannerFile = true;
      startNewScannerFileName = osAddSlash (scannerFileDir) + frame->ScannerFileRootName () + "." + scannerFileBaseExtension;

      bytesWrittenCompletedSegs += bytesWritten;
      delete  scannerFile;  scannerFile = NULL;
      scannerFile = ScannerFile::CreateScannerFileForOutput (startNewScannerFileName, 
                                                             format,
                                                             frameWidth,
                                                             frameHeight,  /**< Frame Height, we will make the same as image width. */
                                                             log
                                                            );
      if  ((!scannerFile)  ||  (!scannerFile->Opened ()))
      {
        log.Level (-1) << endl
                       << "DiskWriterThread::Run   ***ERROR***   Opening Scanner File: " << startNewScannerFileName << endl
                       << endl;
        delete  scannerFile;
        scannerFile = NULL;
        dwStatus = DiskWritingStatus::NotRecording;
        break;
      }
      else
      {
        log.Level (10) << "DiskWriterThread::Run   Started new Scanner-File: " << startNewScannerFileName << endl;
        scannerFile->AddHeaderFields (Manager ()->HeaderFields ());
        AddCameraLineToHeaderFields (scannerFile);
        scannerFile->InitiateWritting ();
        curScannerFileName = scannerFile->FileName ();
        curScannerFileRootName = osGetRootName (curScannerFileName);

        // At this point "LarcosCounterManager::StartNewScannerFile" should already have been called for this scanner file.
        // so no need to cal now.
        //Manager ()->StartNewScannerFile (ScannerFileEntry::GetOrCreateScannerFileEntry (scannerFile));

        startNewScannerFile = false;
      }
    }

    if  (msgsAreQueued)
    {
      msgsAreQueued = false;
      KKStrPtr  msg = msgs->GetNextMsg ();
      while  (msg)
      {
        scannerFile->WriteTextBlock ((uchar*)(msg->Str ()),  msg->Len ());
        delete  msg;
        msg = NULL;
        msg = msgs->GetNextMsg ();
      }
    }

    if  (frame)
    {
      if  (dataIsToBeRecorded  &&  (scannerFile != NULL))
      {
        kkint32 height = frame->FrameHeight ();
        kkint32 width  = frame->FrameWidth ();

        kkuint32  flowMeterCounter = frame->FlowMeterCounter ();
        
        if  (flowMeterCounter  != lastFlowMeterCounter)
        {
          WordFormat32Bits  dw (flowMeterCounter);
          
          if  (flowMeterFile)
            *flowMeterFile 
                 << "\t" << "ScannerFileScanLine"      << "\t" << frame->ScannerFileScanLine ()
                 << "\t" << "FlowMeterCounter"         << "\t" << flowMeterCounter 
                 << "\t" << "FlowMeterCounterScanLine" << "\t" << frame->FlowMeterCounterScanLine () 
                 << "\t" << "FlowRateRatio"            << "\t" << frame->FlowRateRatio ()
                 << endl;

          scannerFile->WriteInstrumentDataWord (0, scannerFile->LastScanLine (), dw);
          lastFlowMeterCounter = flowMeterCounter;
        }

        uchar** scanLines = frame->Frame ()->Green ();
        if  (scanLines == NULL)
        {
          // This is not good !!!
          log.Level (-1) << endl
                         << "DiskWriterThread::Run   ***ERROR***    Raster from frame == NULL  CAN NOT HAPPEN !!!" << endl
                         << endl;
        }
        else
        {
          if  (scannerFile)
          {
            for  (kkint32 row = 0;  row < height;  row++)
            {
              scannerFile->WriteScanLine (scanLines[row], width);
            }
          }
 
          dwStatus = DiskWritingStatus::Recording;        
          scannerFileSize = scannerFile->FileSizeInBytes ();
          bytesWritten = scannerFileSize;
        }

        framesWritten++;
        scanLinesWritten += height;
      }
     
      cameraBuffer->DoneWrittingFrameToDisk (frame);
    }

    if  ((!startNewScannerFile)  &&  (scannerFileSize > scannerFileSizeMax))
    {
      startNewScannerFile = true;
      while  (true)
      {
        startNewScannerFileName =  osAddSlash (scannerFileDir) + scannerFileBaseName + "_" + StrFormatInt (scannerFileBaseCount, "000") + "." + scannerFileBaseExtension;
        scannerFileBaseCount++;
        if  (!osFileExists (startNewScannerFileName))
          break;
      }

      log.Level (10) << "DiskWriterThread::Run   Requesting start new Scanner-File, New-Name: " << startNewScannerFileName << endl;

      ScannerFileEntryPtr  sfe = ScannerFileEntry::GetOrCreateScannerFileEntry (osGetRootName (startNewScannerFileName));
      sfe->FullName (startNewScannerFileName);
      sfe->PixelsPerScanLine (frameWidth);
      sfe->ScanRate (Manager ()->CurScanRate ());
      Manager ()->StartNewScannerFile (sfe);
    }

    frame = GetNextFrame ();
  }

  delete  flowMeterFile;
  flowMeterFile = NULL;

  dwStatus = DiskWritingStatus::NotRecording;

  delete  scannerFile;
  scannerFile = NULL;

  Status (ThreadStatus::Stopping);

  log.Level (10) << "DiskWriterThread::Run   Exiting:"
                 << "  No-More-Frames: " << (frame == NULL)
                 << "  TerminateFlag: "  << TerminateFlag ()
                 << "  ShutdownFlag: "   << ShutdownFlag ()
                 << endl;
  return;
}  /* Run */




void  DiskWriterThread::AddMessageToCurrentScannerFile (const KKStr&  msg)
{
  msgsAreQueued = true;
  msgs->AddMsg (msg);
}


void  DiskWriterThread::ResetCounts ()
{
  bytesWritten              = 0;
  bytesWrittenAllSegs       = 0;
  bytesWrittenCompletedSegs = 0;
  framesWritten             = 0;
  numSkippedFrames          = 0;
  numFramesSinceLastSkip    = 0;
  scannerFileBaseCount      = 0;
  scanLinesWritten          = 0;
}
