/* LogicalFrameBuilder.h -- Will process frames coming from the camera and create logical frames to be processed by a 'LogicalGFrameProcessor'
 * Copyright (C) 2011-2013  Kurt Kramer
 * For conditions of distribution and use, see copyright notice in CounterUnManaged.txt
 */
#include "FirstIncludes.h"

#include <errno.h>
#include <istream>
#include <iostream>
#include <fstream>
#include <deque>
#include <queue>
#include <map>
#include <memory.h>
#include <vector>
#include "MemoryDebug.h"
using namespace std;

#include "KKBaseTypes.h"
#include "Blob.h"
#include "ImageIO.h"
#include "OSservices.h"
#include "Raster.h"
#include "RasterBuffer.h"
using namespace KKB;

#include "ScannerFileEntry.h"
using  namespace  KKLSC;

#include "CounterVariables.h"
using namespace  CounterBase;

#include "LogicalFrameBuilder.h"

#include "CameraFrame.h"
#include "CameraFrameBuffer.h"
#include "CounterManager.h"
#include "CounterStats.h"
#include "LogicalFrame.h"
#include "LogicalFrameEntry.h"
using  namespace  CounterUnManaged;

/**
 *@brief  Takes the physical frame data from the camera via 'cameraBuffer' and breaks it up into 
 *        logical frames.  The logical frames are then placed into 'logicalFrames' to be further 
 *        processed.
 */
LogicalFrameBuilder::LogicalFrameBuilder 
                          (CounterManagerPtr   _manager,
                           CameraFrameBufferPtr      _cameraBuffer,
                           LogicalFrameListPtr       _logicalFrames,
                           LogicalFrameEntryListPtr  _logicalFrameEntries,
                           LogicalFrameEntryQueuePtr _droppedFrames,
                           kkint32                   _scanLineLen,
                           kkint32                   _scanLinesToAllocate,
                           uchar                     _backGroundPixelTH,
                           MsgQueuePtr               _msgQueue,
                           const KKStr&              _threadName
                          ):
    CameraThread  (_manager, _threadName, _msgQueue),

    backGroundPixelTH              (_backGroundPixelTH),
    totalWorkArea                  (0),
    scanLines                      (NULL),
    scanLinesArea                  (NULL),
    scanLinesAllocated             (0),
    scanLinesSeqNumOfFisrtScanLine (0),
    cropLeft                       (0),
    cropRight                      (2047),
    dontDropFrames                 (false),
    frameSeqNumLast                (0),
    lastScanLineToProcess          (-1),
    lastScanLineWithNoPixels       (-1),
    minSizeThreshold               (300),
    nextRowToUse                   (0),
    pixelsOnScanLine               (NULL),
    flowRateOnScanLine             (NULL),
    flowRateRatioOnScanLine        (NULL),
    logicalFrames                  (_logicalFrames),
    logicalFrameEntries            (_logicalFrameEntries),
    droppedFrames                  (_droppedFrames),
    droppedFramesBeingProcessed    (false),
    logicalFramesCreated           (0),
    logicalFramesDropped           (0),
    logicalFramesSeqNum            (1),
    frameWidth                     (_scanLineLen),
    physicalFramesProcessed        (0),
    playingBack                    (false),
    playingBackRealTime            (false),
    curScannerFileEntry            (NULL),
    curScannerFileScanLineRow      (-1),
    cameraBuffer                   (_cameraBuffer)

{
  log.Level (20) << "LogicalFrameBuilder::LogicalFrameBuilder" << endl;
  totalWorkArea = _scanLinesToAllocate * frameWidth;
  scanLinesAllocated = _scanLinesToAllocate;

  scanLinesArea           = new uchar [totalWorkArea];
  scanLines               = new uchar*[scanLinesAllocated];
  pixelsOnScanLine        = new kkuint16[scanLinesAllocated];
  flowRateOnScanLine      = new float[scanLinesAllocated];
  flowRateRatioOnScanLine = new float[scanLinesAllocated];

  uchar*  scanLinesAreaPtr   = scanLinesArea;
  for  (kkint32 row = 0;  row < scanLinesAllocated;  row++)
  {
    pixelsOnScanLine       [row] = 0;
    flowRateOnScanLine     [row] = 1.0f;
    flowRateRatioOnScanLine[row] = 1.0f;
    scanLines              [row] = scanLinesAreaPtr;
    scanLinesAreaPtr += frameWidth;
  }

  playingBack = Manager ()->PlayingBack ();
  playingBackRealTime = Manager ()->PlayingBackRealTime ();

  dontDropFrames = (playingBack  &&  (!playingBackRealTime));
}



LogicalFrameBuilder::~LogicalFrameBuilder ()
{
  CleanUpMemory ();
  log.Level (20) << "LogicalFrameBuilder::~LogicalFrameBuilder" << endl;
}



void  LogicalFrameBuilder::CleanUpMemory ()
{
  delete  scanLinesArea;            scanLinesArea           = NULL;
  delete  scanLines;                scanLines               = NULL;
  delete  pixelsOnScanLine;         pixelsOnScanLine        = NULL;
  delete  flowRateOnScanLine;       flowRateOnScanLine      = NULL;
  delete  flowRateRatioOnScanLine;  flowRateRatioOnScanLine = NULL;
}



kkMemSize LogicalFrameBuilder::MemoryConsumedEstimated ()
{
  return  sizeof (LogicalFrameBuilder);
}



void  LogicalFrameBuilder::GetStats (CounterStats&  stats) const
{
  stats.LogicalFramesCreated    (logicalFramesCreated);
  stats.LogicalFramesDropped    (logicalFramesDropped);
  stats.PhysicalFramesProcessed (physicalFramesProcessed);
}



void  LogicalFrameBuilder::CropSettings (kkint32  _cropLeft,
                                         kkint32  _cropRight
                                        )
{
  cropLeft  = _cropLeft;
  cropRight = _cropRight;
}



CameraFramePtr  LogicalFrameBuilder::GetNextFrame ()
{
  CameraFramePtr  frame = cameraBuffer->GetNextFrameToProcess ();
  while   ((frame == NULL)  &&  (!TerminateFlag ())  &&  (!ShutdownFlag ()))
  {
    osSleepMiliSecs (10);
    frame = cameraBuffer->GetNextFrameToProcess ();
  }

  if  (frame != NULL)
  {
    if  (frame->SeqNum () != (frameSeqNumLast + 1))
    {
      KKStr  statusMsg (100);
      statusMsg << ThreadName () << "::" << "GetNextFrame   SeqNum Break."
                << " LastFrameSeqNum[" << frameSeqNumLast << "]  frame->SeqNum[" << frame->SeqNum () << "].";
      AddMsg (statusMsg);
    }
    frameSeqNumLast = frame->SeqNum ();
  }

  return  frame;
}  /* GetNextFrame */



// This thread will include counting.
void  LogicalFrameBuilder::Run ()
{
  log.Level (10) << "LogicalFrameBuilder::Run" << endl;
  Status (ThreadStatus::Running);
  if  (!cameraBuffer)
  {
    AddMsg ("LogicalFrameBuilder::Run     Missing Camera");
    Crashed (true);
    Status (ThreadStatus::Stopping);
  }

  CameraFramePtr  frame = NULL;

  if  (!(Manager ()->PlayingBack ()))
  {
    // Skip the first 20 frames to get Past Flat Field Correction.
    for  (kkint32 x = 0;  (x < 20  &&  (!TerminateFlag ()));  x++)
    {
      frame = GetNextFrame ();
      if  (frame)
        cameraBuffer->DoneProcessingFrame (frame);
      frame = NULL;
    }
  }

  if  (!TerminateFlag ())
  {
    frame = GetNextFrame ();
    curScannerFileEntry = frame->ScannerFile ();
    bool  sameScannerFile = (frame->ScannerFile () == curScannerFileEntry);

    while  ((frame != NULL)  &&   (!TerminateFlag ())  &&  (!ShutdownFlag ()))
    {
      // We are starting a brand new logical frame such that there are no leftover scan lines.
      // We need to do this when ever we start processing data that is meant for a new Scanner File or
      // was read from a new Scanner file.
      curScannerFileEntry       = frame->ScannerFile ();
      curScannerFileScanLineRow = frame->ScannerFileScanLine ();
      sameScannerFile = true;

      while  ((frame != NULL)  &&  sameScannerFile  &&  (!TerminateFlag ())  &&  (!ShutdownFlag ()))
      {
        if  (!frame)
          break;
        kkint32 numEmptyScanLines = scanLinesAllocated - nextRowToUse;

        while  ((frame != NULL)        &&  
                sameScannerFile        &&  
                (!TerminateFlag ())    &&  
                (!ShutdownFlag  ())    &&  
                (numEmptyScanLines > frame->FrameHeight ())
               )
        {
          AddFrameToScanLineArea (frame);
          cameraBuffer->DoneProcessingFrame (frame);
          frame = NULL;
          ++physicalFramesProcessed;

          numEmptyScanLines = scanLinesAllocated - nextRowToUse;

          frame = GetNextFrame ();
          if  (frame)
            sameScannerFile = (frame->ScannerFile () == curScannerFileEntry);
        }
      
        FindLastScanLineToProcess ();
        PlaceRasterDataIntoLogicalFrameList (curScannerFileEntry);
        MoveOutProcessedScanLines ();
      }

      if  ((!TerminateFlag ())  &&  (nextRowToUse > 0))
      {
        // We want to process what ever if left in the work area before we start adding
        // data from another scanner file.
        SetLastUsedRowAsLastScanLineToProcess ();
        PlaceRasterDataIntoLogicalFrameList (curScannerFileEntry);
        MoveOutProcessedScanLines ();
      }
    }
  
    if  (frame)
    {
      cameraBuffer->DoneProcessingFrame (frame);
      frame = NULL;
    }

    if  (!TerminateFlag ())
      ProcessDroppedFrames ();
  }

  CleanUpMemory ();

  Status (ThreadStatus::Stopping);

  log.Level (10) << "LogicalFrameBuilder::Run   Exiting:"
                 << "  No-More-Frames: " << (frame == NULL)
                 << "  TerminateFlag: "  << TerminateFlag ()
                 << "  ShutdownFlag: "   << ShutdownFlag ()
                 << endl;
}  /* Run */



void  LogicalFrameBuilder::AddFrameToScanLineArea (CameraFramePtr f)
{
  kkint32 scanLinesLeft = scanLinesAllocated - nextRowToUse;
  if  (f->FrameHeight () > scanLinesLeft)
  {
    // There is no room to fit this frame.
    KKStr  m(100);
    m << ThreadName () << "::" << "AddFrameToScanLineArea   ***ERROR***  No room to add Frame[" << f->SeqNum () << "].";
    AddMsg (m);
    return;
  }

  const RasterPtr  r = f->Frame ();
  
  kkint32 frameHeight = r->Height ();
  uchar*  dest = scanLines[nextRowToUse];
  memcpy (dest, r->GreenArea (), (r->Height () * r->Width ()));
  kkint32 newNextRowToUse = nextRowToUse + frameHeight;

  while  (nextRowToUse < newNextRowToUse)
  {
    uchar*  scanLineRow = scanLines[nextRowToUse];
    pixelsOnScanLine        [nextRowToUse] = 0;
    flowRateOnScanLine      [nextRowToUse] = 0.0f;
    flowRateRatioOnScanLine [nextRowToUse] = 0.0f;
    kkint32 pixelCount = 0;
    for  (kkint32 col = cropLeft;  col <= cropRight;  col++)
    {
      if  (scanLineRow[col] > backGroundPixelTH)
        pixelCount++;
    }

    pixelsOnScanLine         [nextRowToUse] = pixelCount;
    flowRateOnScanLine       [nextRowToUse] = f->FlowRate ();
    flowRateRatioOnScanLine  [nextRowToUse] = f->FlowRateRatio ();

    if  (pixelCount < 1)
      lastScanLineWithNoPixels = nextRowToUse;
    nextRowToUse++;
  }
}  /* AddFrameToScanLineArea */



void  LogicalFrameBuilder::FindLastScanLineToProcess ()
{
  kkint32 oneForthRow = scanLinesAllocated / 4;
  if  ((lastScanLineWithNoPixels > oneForthRow)  &&  (lastScanLineWithNoPixels < nextRowToUse))
  {
    lastScanLineToProcess = lastScanLineWithNoPixels;
    return;
  }
  
  kkint32 row = nextRowToUse - 1;
  kkint32 leastNumPixels = pixelsOnScanLine[row];
  kkint32 rowWithLeastNumPixels = row;
  while  (row > oneForthRow)
  {
    if  (pixelsOnScanLine[row] < leastNumPixels)
    {
      leastNumPixels = pixelsOnScanLine[row];
      rowWithLeastNumPixels = row;
    }
    row--;
  }

  lastScanLineToProcess = rowWithLeastNumPixels;
  return;
}  /* FindLastScanLineToProcess */



void  LogicalFrameBuilder::SetLastUsedRowAsLastScanLineToProcess ()
{
  lastScanLineToProcess = nextRowToUse - 1;
  return;
}  /* FindLastScanLineToProcess */



void  LogicalFrameBuilder::PlaceRasterDataIntoLogicalFrameList (ScannerFileEntryPtr  scannerFileEntry)
{
  kkint32 height = lastScanLineToProcess + 1;
  kkint32 width  = frameWidth;

  LogicalFramePtr  frame = NULL;
  
  ++logicalFramesCreated;

  // If we are playing back a existing Scanner file and we are not simulating real 
  // time then we do not want to drop any data.  So when 'dontDropFrames' is true we 
  // will stay in loop until a logical from is available.
  frame = logicalFrames->GetAvailableFrame (log);
  while  (dontDropFrames  &&  (frame == NULL)  &&  (!TerminateFlag ()))
  {
    osSleepMiliSecs (10);
    frame = logicalFrames->GetAvailableFrame (log);
  }

  float  flowRate      = (flowRateOnScanLine      [0] + flowRateOnScanLine      [height - 1]) / 2.0f;
  float  flowRateRatio = (flowRateRatioOnScanLine [0] + flowRateRatioOnScanLine [height - 1]) / 2.0f;

  LogicalFrameEntryPtr  entry = new LogicalFrameEntry (logicalFramesSeqNum, 
                                                       height, 
                                                       scannerFileEntry,
                                                       scanLinesSeqNumOfFisrtScanLine,   /**< Scan-Line in destination or source Scanner-File that this logical frame starts at. */
                                                       -1,                               /**< byteOffset */ 
                                                       -1,                               /**< count      */
                                                       flowRateRatio, 
                                                       flowRate
                                                      );
  logicalFrameEntries->PushOnBack (entry);
  if  (!frame)
  {
    droppedFrames->PushOnFront (entry);
    ++logicalFramesDropped;
  }
  else
  {
    frame->AssignAFrame (entry,
                         height, width, scanLinesSeqNumOfFisrtScanLine,
                         scannerFileEntry,
                         curScannerFileScanLineRow,   /**< Row in the Current scanner file that this logical frame derives from.  */
                         flowRateRatio,
                         flowRate,
                         logicalFramesSeqNum, scanLinesArea
                        );
    logicalFrames->QueueFrameForProcessing (frame);
  }

  ++logicalFramesSeqNum;
  return ;
}  /* PlaceRasterDataIntoLogicalFrameList */



void  LogicalFrameBuilder::MoveOutProcessedScanLines ()
{
  // 'scanLines' represent the working set of scanLines being processed,  but in the global sense of
  // total number of scanLines processed the first row in 'scanLines' is the 'scanLinesSeqNumOfFisrtScanLine'
  // scan line retrieved from the 'CameraAcquisition' instance.  We will need to increment this value by the
  // number of rows just processed '(lastScanLineToProcess + 1)'.
  scanLinesSeqNumOfFisrtScanLine += (lastScanLineToProcess + 1);

  // Similar to 'scanLinesSeqNumOfFisrtScanLine' we need to adjust 'curScannerFileScanLineRow' by the number of
  // scan lines that are being added to this LogicalFrame.
  curScannerFileScanLineRow      += (lastScanLineToProcess + 1);
  
  // At this point all scan lines up to and including 'lastScanLineToProcess'  have been selected to add 
  // to a logical frame.  We will now create that logical frame add it to the logicalFrame queue for 
  // later processing.

  kkint32 linesLeftToProcess = (nextRowToUse - lastScanLineToProcess) - 1;
  if  (linesLeftToProcess > 0)
  {
    kkint32 scanLineToMoveFrom = lastScanLineToProcess + 1;
    memmove (scanLinesArea, scanLines[scanLineToMoveFrom], linesLeftToProcess * frameWidth);

    kkint32 destRow = 0;
    kkint32 srcRow = lastScanLineToProcess + 1;
    for  (destRow = 0;  destRow < linesLeftToProcess;  ++destRow, ++srcRow)
    {
      pixelsOnScanLine        [destRow] = pixelsOnScanLine        [srcRow];
      flowRateOnScanLine      [destRow] = flowRateOnScanLine      [srcRow];
      flowRateRatioOnScanLine [destRow] = flowRateRatioOnScanLine [srcRow];
    }
  }

  nextRowToUse = linesLeftToProcess;
  lastScanLineWithNoPixels = -1;

  kkint32 linesNeedingToBeBlankedOut = scanLinesAllocated - linesLeftToProcess;
  if  (linesNeedingToBeBlankedOut > 0)
  {
    memset (scanLines[nextRowToUse], 0, linesNeedingToBeBlankedOut * frameWidth);
  }
}  /* MoveOutProcessedScanLines */



void  LogicalFrameBuilder::ProcessDroppedFrames ()
{
  /**@todo   We need to wait until writing to the ScannerFile is completed and it is closed */
  kkint32 numFramesDropped = droppedFrames->QueueSize ();

  log.Level (10) << "LogicalFrameBuilder::ProcessDroppedFrames    numFramesDropped: " << numFramesDropped << endl;

  if  (numFramesDropped < 1)
  {
    log.Level (10) << "ProcessDroppedFrames   No Frames Were Dropped!" << endl;
    return;
  }

  droppedFramesBeingProcessed = true;

  KKStr numFramesDroppedStr = StrFormatInt (numFramesDropped, "##0");
  AddMsg ("Processing Dropped Frames [" + numFramesDroppedStr + "]");

  // We will now process all the frames that have been dropped.
  LogicalFrameEntryPtr  frameEntry = droppedFrames->PopFromBack ();

  while  (frameEntry  &&  (!TerminateFlag ()))
  {
    ScannerFileEntryPtr  curScannerFile = frameEntry->ScannerFile ();
    
    log.Level (20) << "ProcessDroppedFrames   Opening Scanner-File: " << curScannerFile->RootName () << endl;
    ScannerFilePtr sf = ScannerFile::CreateScannerFile (curScannerFile->FullName (), log);
    if  (sf->Opened ())
      sf->BuildFrameOffsets (this->TerminateFlag ());

    kkint32  droppedFramesLeft = numFramesDropped;

    while  (frameEntry  &&  (!TerminateFlag ())  &&  (curScannerFile == frameEntry->ScannerFile ()))
    {
      log.Level (20) << "ProcessDroppedFrames     droppedFramesLeft: " << droppedFramesLeft << "  FrameNum: " << frameEntry->FrameNum () << endl;
      LoadLogicalFrame (sf, frameEntry);
      frameEntry = droppedFrames->PopFromBack ();
      --droppedFramesLeft;
    }

    delete  sf;
    sf = NULL;
  }

  droppedFramesBeingProcessed = false;

  log.Level (10) << "LogicalFrameBuilder::ProcessDroppedFrames   Exiting" << endl;

}  /* ProcessDroppedFrames */



void  LogicalFrameBuilder::LoadLogicalFrame (ScannerFilePtr        sf,
                                             LogicalFrameEntryPtr  frameEntry
                                            )
{
  // We will load the logical frame into the normal FramArea that would have been used in 
  // 'AddFrameToScanLineArea' as if there is no other data there.

  log.Level (20) << "LogicalFrameBuilder::LoadLogicalFrame   FrameNum: " << frameEntry->FrameNum () << "  ScanLine: " << frameEntry->ScanLineNumStart () << "  FrameHeight: " << frameEntry->Height () << endl;

  sf->SkipToScanLine (frameEntry->ScanLineNumStart ());
  kkint32  linesRead = 0;
  kkuint32  lineSize = 0;
  kkuint32*  colCount = new kkuint32[frameWidth];
  for  (kkint32  x = 0;  x < frameWidth;  ++x)
    colCount[x] = 0;
  kkuint32  pixelsInRow = 0;

  uchar*  nextDestRow = scanLinesArea;
  while  ((linesRead < frameEntry->Height ())  &&  (!sf->Eof ()))
  {
    sf->GetNextLine (nextDestRow, 
                     frameWidth, 
                     lineSize,     //  Number pixels read into nextDestRow.
                     colCount,
                     pixelsInRow   // number of foreground pixels in row.
                    );
    nextDestRow += frameWidth;
    ++linesRead;
  }
  nextRowToUse = linesRead;

  LogicalFramePtr  frame = logicalFrames->GetAvailableFrame (log);
  while  ((frame == NULL)  &&  (!TerminateFlag ()))
  {
    osSleepMiliSecs (10);
    frame = logicalFrames->GetAvailableFrame (log);
  }

  log.Level (20) << "LoadLogicalFrame  Queuing FrameNum: " << frameEntry->FrameNum () << endl;

  frame->AssignAFrame (frameEntry,
                       frameEntry->Height (),
                       frameWidth,
                       frameEntry->ScanLineNumStart (),
                       frameEntry->ScannerFile (),
                       frameEntry->ScanLineNumStart (),
                       frameEntry->FlowRateRatio (),
                       frameEntry->FlowRate (),
                       frameEntry->FrameNum (),
                       scanLinesArea
                      );
  logicalFrames->QueueFrameForProcessing (frame);
  delete[]  colCount;
}  /* LoadLogicalFrame */



void  LogicalFrameBuilder::ResetCounts ()
{
  logicalFramesCreated    = 0;
  logicalFramesDropped    = 0;
  physicalFramesProcessed = 0;
}
