/* CameraFrameBuffer.h -- Implements Camera Frame Buffering allowing a camera thread 
 *                        to unload frames quickly for a separate thread to process.
 * Copyright (C) 2011-2013  Kurt Kramer
 */
#include "FirstIncludes.h"

#include <errno.h>
#include <istream>
#include <iostream>
#include <queue>
#include <vector>
#include <math.h>
#include "MemoryDebug.h"
using namespace std;


#include "KKBaseTypes.h"
#include "KKException.h"
#include "GoalKeeperSimple.h"
#include "OSservices.h"
using namespace KKB;

#include "ScannerFile.h"
#include "ScannerFileEntry.h"
using namespace KKLSC;

#include "CameraFrameBuffer.h"
#include "CameraFlatFieldCorrection.h"
#include "CameraFrame.h"
#include "LarcosCounterStats.h"
#include "LarcosCounterManager.h"
using  namespace  LarcosCounterUnManaged;



CameraFrameBuffer::CameraFrameBuffer (LarcosCounterManagerPtr _manager,
                                      const KKStr&            _name,        /* Name of buffer, must be unique */
                                      kkint32                 _bufferSize,
                                      kkint32                 _maxNumOfBuffers,
                                      kkint32                 _frameHeight,
                                      kkint32                 _frameWidth,
                                      bool                    _dataIsToBeCounted,
                                      bool                    _dataIsToBeRecorded,
                                      bool                    _sampleLastFrameBeforeFlatField
                                     ):

    available                      (NULL),
    buffer                         (NULL),
    compensationTable              (NULL),
    curScannerFileEntry            (NULL),
    dataIsToBeCounted              (_dataIsToBeCounted),
    dataIsToBeRecorded             (_dataIsToBeRecorded),
    flatFieldCorrection            (NULL),
    flatFeildSamplingInterval      (2),
    frameHeight                    (_frameHeight),
    frameWidth                     (_frameWidth),
    gateKeeper                     (NULL),
    lastFrames                     (NULL),
    lastFramesIdx                  (0),
    lastFramesMinScanLines         (0),
    lastFramesSize                 (0),
    manager                        (_manager),
    maxNumOfBuffers                (_maxNumOfBuffers),
    name                           (_name),
    nextFrameSeqNum                (0),
    physicalFramesDropped          (0),
    sampleLastFrameBeforeFlatField (_sampleLastFrameBeforeFlatField),
    scannerFileNumScanLines        (0),
    snapShotLatestFrameWorkArea    (NULL),
    waitingToProcess               (NULL),
    waitingToWriteToDisk           (NULL)
{
  available            = new CameraFrameList (false);
  buffer               = new CameraFrameList (true);
  waitingToProcess     = new CameraFrameList (false);
  waitingToWriteToDisk = new CameraFrameList (false);

  if  (maxNumOfBuffers < _bufferSize)
    maxNumOfBuffers = _bufferSize;

  GoalKeeper::Create ("CameraFrameBuffer_" + name, gateKeeper);
  IncreaseBufferSize (_bufferSize);

  flatFieldCorrection = new CameraFlatFieldCorrection (20, 
                                                       frameWidth, 
                                                       compensationTable
                                                      );

  LastFramesAllocate ();
}



CameraFrameBuffer::~CameraFrameBuffer ()
{
  delete  available;             available             = NULL;
  delete  waitingToProcess;      waitingToProcess      = NULL;
  delete  waitingToWriteToDisk;  waitingToWriteToDisk  = NULL;
  delete  buffer;                buffer                = NULL;
  delete  flatFieldCorrection;   flatFieldCorrection   = NULL;

  GoalKeeper::Destroy (gateKeeper);
  gateKeeper = NULL;

  LastFramesCleanUp ();

  compensationTable = NULL;
}



void  CameraFrameBuffer::LastFramesAllocate ()
{
  kkint32  newLastFramesSize = (kkint32)ceil ((float)lastFramesMinScanLines / (float)frameHeight);
  if  (newLastFramesSize == lastFramesSize)
    return;

  if  (lastFrames)
    LastFramesCleanUp ();

  lastFramesSize = newLastFramesSize;

  lastFrames = new RasterPtr[lastFramesSize];
  for  (int x = 0;  x < lastFramesSize;  ++x)
    lastFrames[x] = new Raster (frameHeight, frameWidth);
  lastFramesIdx = lastFramesSize - 1;

  snapShotLatestFrameWorkArea = new Raster (lastFramesSize * frameHeight, frameWidth);
}  /* AllocateLastFrames */



void  CameraFrameBuffer::LastFramesCleanUp ()
{
  if  (lastFrames == NULL)
    return;

  for  (kkint32 x = 0;  x < lastFramesSize;  ++x)
  {
    delete  lastFrames[x];  
    lastFrames[x] = NULL;
  }

  delete[]  lastFrames;
  lastFrames = NULL;
  lastFramesSize = 0;

  delete  snapShotLatestFrameWorkArea;
  snapShotLatestFrameWorkArea = NULL;
}



bool  CameraFrameBuffer::FlatFieldEnabled  () const
{
  if  (flatFieldCorrection)
    return flatFieldCorrection->Enabled ();
  else
    return false;
}



void  CameraFrameBuffer::FlatFieldEnabled (bool _enabled)
{
  if  (flatFieldCorrection)
    flatFieldCorrection->Enabled (_enabled);
}


void  CameraFrameBuffer::SetCompensationTable (const uchar* _compensationTable)  
{
  compensationTable = _compensationTable;
  if  (flatFieldCorrection)
    flatFieldCorrection->CompensationTable (compensationTable);
}



void  CameraFrameBuffer::SetLastFramesMinScanLines (kkint32 _lastFramesMinScanLines)
{
  gateKeeper->StartBlock ();
  lastFramesMinScanLines = Max ((kkint32)1, _lastFramesMinScanLines);
  LastFramesCleanUp ();
  LastFramesAllocate ();
  gateKeeper->EndBlock ();
}



void  CameraFrameBuffer::DataIsToBeCounted  (bool _dataIsToBeCounted)
{
  gateKeeper->StartBlock ();
  dataIsToBeCounted  = _dataIsToBeCounted;
  gateKeeper->EndBlock ();
}


void  CameraFrameBuffer::DataIsToBeRecorded (bool _dataIsToBeRecorded)
{
  gateKeeper->StartBlock ();
  dataIsToBeRecorded = _dataIsToBeRecorded;
  gateKeeper->EndBlock ();
}



void  CameraFrameBuffer::DataIsToBeCountedAndOrRecorded (bool _dataIsToBeCounted,
                                                         bool _dataIsToBeRecorded
                                                        )
{
  gateKeeper->StartBlock ();
  dataIsToBeCounted  = _dataIsToBeCounted;
  dataIsToBeRecorded = _dataIsToBeRecorded;
  gateKeeper->EndBlock ();
}



/** @brief  Remove the oldest frame in the 'waitingToProcess' queue and has already been written to disk. */
void  CameraFrameBuffer::ThrowOutOldestOccupiedBuffer ()
{
  gateKeeper->StartBlock ();

  CameraFrameList::iterator  idx;
  for  (idx = waitingToProcess->begin ();  idx != waitingToProcess->end ();  ++idx)
  {
    CameraFramePtr  frame = *idx;
    if  (frame->WaitingToProcess ()  &&  (frame->WrittenToDisk ()))
    {
      // This frame has already been written to disk,  but is still waiting to be processed.
      // Since it is more important that it be written to disk it is Okay to throw out.  
      waitingToProcess->DeleteEntry (frame);
      //waitingToWriteToDisk->DeleteEntry (frame);
      available->PushOnBack (frame);
      physicalFramesDropped++;
      break;
    }
  }

  gateKeeper->EndBlock ();
  return;
}  /* ThrowOutOldestOccupiedBuffer */



void  CameraFrameBuffer::IncreaseBufferSize (kkint32 numNewFrameBuffers)
{
  for  (kkint32 x = 0;  x < numNewFrameBuffers;  ++x)
  {
    CameraFramePtr  frame = new CameraFrame (frameHeight, frameWidth);
    frame->Processed            (false);
    frame->WrittenToDisk        (false);
    frame->WaitingToWriteToDisk (false);
    frame->WaitingToProcess     (false);

    buffer->PushOnBack (frame);
    available->PushOnBack (frame);
  }
}  /* IncreaseBufferSize */



kkint32 CameraFrameBuffer::NumAvailable () const 
{
  if  (available)
    return available->QueueSize ();
  else
    return 0;
}  /* NumAvailable */



kkint32 CameraFrameBuffer::NumWaitingToProcess () const 
{
  if  (waitingToProcess)
    return waitingToProcess->QueueSize ();
  else
    return 0;
}



void  CameraFrameBuffer::GetStats (LarcosCounterStats&  larcosCameraStats) const
{
  larcosCameraStats.PhysicalFramesWaitingToProcess (NumWaitingToProcess ());
  larcosCameraStats.PhysicalFramesDropped          (physicalFramesDropped);
  larcosCameraStats.WaitingToWriteToDisk           (NumWaitingToWriteToDisk ());
}



kkint32 CameraFrameBuffer::NumWaitingToWriteToDisk () const 
{
  if  (waitingToWriteToDisk)
    return waitingToWriteToDisk->QueueSize ();
  else
    return 0;
}




void  CameraFrameBuffer::UpdateLastFrame (uchar*  cameraData, 
                                          kkint32 cameraDataSize,
                                          kkint32 height,
                                          kkint32 width
                                         )
{
  // Update LastFrame window.
  // Update last frames.

  if  (!lastFrames)
    LastFramesAllocate ();

  if  ((height != lastFrames[lastFramesIdx]->Height ())  ||
       (width  != lastFrames[lastFramesIdx]->Width  ())
      )
  {
    LastFramesCleanUp ();
    LastFramesAllocate ();
  }
  
  ++lastFramesIdx;
  if  (lastFramesIdx >= lastFramesSize)
    lastFramesIdx = 0;

  KKStr::MemCpy (lastFrames[lastFramesIdx]->GreenArea (), cameraData, cameraDataSize);
}






/**
 * This method creates a copy of the most current frame retrieved from the camera.
 * We are not using yet;  not sure this is the best way to do it.
 */
RasterPtr  CameraFrameBuffer::SnapShotLatestFrame (kkint32  cropLeft,
                                                   kkint32  cropRight
                                                  )
{
  gateKeeper->StartBlock ();

  if  (lastFrames == NULL)
  {
    gateKeeper->EndBlock ();
    return NULL;
  }

  kkint32  width = frameWidth;

  cropLeft   = Max ((kkint32)0, cropLeft);
  cropRight  = Min (width - 1, cropRight);

  if  ((cropRight - cropLeft) < 50)
  {
    cropLeft = 0;
    cropRight = width - 1;
  }

  uchar**  destRows = snapShotLatestFrameWorkArea->Green ();
  kkint32  destRowIdx = snapShotLatestFrameWorkArea->Height () - 1;

  kkint32  nextIdx = lastFramesIdx + 1;  // "nextIdx" = oldest frame in "lastFrames".
  if  (nextIdx >= lastFramesSize)
    nextIdx = 0;

  for  (int x = 0;  x < lastFramesSize;  ++x)
  {
    kkint32  frameHeight = lastFrames[nextIdx]->Height ();
    kkint32  frameWidth  = lastFrames[nextIdx]->Width  ();
    uchar** srcRows = lastFrames[nextIdx]->Green ();
    for  (kkint32  y = 0;  y < frameHeight;  ++y, --destRowIdx)
    {
      KKStr::MemCpy (destRows[destRowIdx], srcRows[y], frameWidth);
    }

    ++nextIdx;
    if  (nextIdx >= lastFramesSize)
      nextIdx = 0;
  }
     
  gateKeeper->EndBlock ();

  kkint32 newWidth = 1 + cropRight - cropLeft;

  RasterPtr  result = new Raster (*snapShotLatestFrameWorkArea, 0, cropLeft, snapShotLatestFrameWorkArea->Height (), newWidth);

  if  (sampleLastFrameBeforeFlatField)
  {
    result->BackgroundPixelValue (255);
    result->ForegroundPixelValue (0);
  }
  else
  {
    result->BackgroundPixelValue (0);
    result->ForegroundPixelValue (255);
  }

  return result;
}  /* SnapShotLatestFrame */




VectorUcharPtr CameraFrameBuffer::LastFrameAverageScanLine()  const
{
  gateKeeper->StartBlock ();

  if  (lastFrames == NULL)
  {
    gateKeeper->EndBlock ();
    return NULL;
  }

  kkint32  row = 0;
  kkint32  col = 0;

  kkint32  frameHeight = lastFrames[lastFramesIdx]->Height ();
  kkint32  frameWidth  = lastFrames[lastFramesIdx]->Width  ();
  kkuint32*  colTotals = new kkuint32[frameWidth];
  for  (col = 0;  col < frameWidth;  ++col)
    colTotals[col] = 0;

  uchar** srcData = lastFrames[lastFramesIdx]->Green ();

  for  (row = 0;  row < frameHeight;  ++row)
  {
    uchar*  srcRow = srcData[row];
    for  (col = 0;  col < frameWidth;  ++col)
      colTotals[col] += srcRow[col];
  }

  gateKeeper->EndBlock ();

  VectorUcharPtr result = new VectorUchar (frameWidth, 0);
  for  (col = 0;  col < frameWidth;  ++col)
    (*result)[col] = (int)(0.5f + (float)(colTotals[col]) / (float)frameHeight);

  delete  colTotals;
  colTotals = NULL;

  return result;
}  /* LastFrameAverageScanLine */



VectorUcharPtr CameraFrameBuffer::LastFrameHighValuesScanLine()  const
{
  gateKeeper->StartBlock ();

  if  (lastFrames == NULL)
  {
    gateKeeper->EndBlock ();
    return NULL;
  }

  kkint32  row = 0;
  kkint32  col = 0;

  kkint32  frameHeight = lastFrames[lastFramesIdx]->Height ();
  kkint32  frameWidth  = lastFrames[lastFramesIdx]->Width  ();
  uchar*  colValues = new uchar[frameWidth];
  for  (col = 0;  col < frameWidth;  ++col)
    colValues[col] = 0;

  uchar** srcData = lastFrames[lastFramesIdx]->Green ();

  for  (row = 0;  row < frameHeight;  ++row)
  {
    uchar*  srcRow = srcData[row];
    for  (col = 0;  col < frameWidth;  ++col)
    {
      uchar  b = srcRow[col];
      if  (b > colValues[col])
        colValues[col] = b;
    }
  }

  gateKeeper->EndBlock ();

  VectorUcharPtr result = new VectorUchar (frameWidth, 0);
  for  (col = 0;  col < frameWidth;  ++col)
    (*result)[col] = colValues[col];

  delete  colValues;
  colValues = NULL;

  return result;
}  /* LastFrameHighValuesScanLine */





void  CameraFrameBuffer::StartNewScannerFile (ScannerFileEntryPtr  _scannerFileEntry)
{
  curScannerFileEntry = _scannerFileEntry;
  scannerFileNumScanLines = 0;
}



void  CameraFrameBuffer::AddFrame (uchar*   cameraData,
                                   kkint32  cameraDataSize,
                                   kkint32  height,
                                   kkint32  width,
                                   kkuint32 flowMeterCounter,
                                   kkuint32 flowMeterCounterScanLine,
                                   float    flowRateRatio,
                                   float    flowRate
                                  )
{
  if  (cameraDataSize != height * width)
  {
    KKStr  errMsg;
    errMsg << "CameraFrameBuffer::AddFrame    cameraDataSize[" << cameraDataSize << "] and "
           << "height[" << height << "]  Width[" << width << "]  are not in agreement.";
    cerr << endl << endl << errMsg << endl << endl;
    throw KKException (errMsg);
  }

  flatFieldCorrection->AddSampleLine (cameraData);

  gateKeeper->StartBlock ();

  if  (sampleLastFrameBeforeFlatField)
    UpdateLastFrame (cameraData, cameraDataSize, height, width);

  uchar*   cameraDataRow = cameraData;
  for  (kkint32 row = 0;  row < height;  ++row)
  {
    flatFieldCorrection->ApplyFlatFieldCorrection (cameraDataRow);
    cameraDataRow += width;
  }

  // 2014-03-15
  // The 'compensationTable' will now be applied during 'CameraFlatFieldCorrection::ApplyFlatFieldCorrection'
  // This will result in considerable CPU time savings.
  //if  (compensationTable)
  //{
  //  for  (int xx = 0;  xx < cameraDataSize; ++xx)
  //    cameraData[xx] = compensationTable[cameraData[xx]];
  //}

  if  (!sampleLastFrameBeforeFlatField)
    UpdateLastFrame (cameraData, cameraDataSize, height, width);

  if  (dataIsToBeCounted  ||  dataIsToBeRecorded)
  {
    if  (available->QueueSize () < 1)
    {
      // There are no available frame buffers to use.
      if  (buffer->QueueSize () < maxNumOfBuffers)
      {
        // We will add a new one or now.
        IncreaseBufferSize (1);
      }
      else
      {
        ThrowOutOldestOccupiedBuffer ();
      }
    }

    CameraFramePtr  frame = available->PopFromBack ();

    if  (frame == NULL)
    {
      physicalFramesDropped++;
    }
    else
    {
      if  ((height != frame->FrameHeight ())  ||  (width !=  frame->FrameWidth ()))
      {
        // We will need to resize the frame; what a pain in the ass.
        frame->ReSizeFrame (height, width);
      }
      frame->PopualteFrameFromCameraData (cameraData, 
                                          cameraDataSize, 
                                          false,                      /**<  false = Do NOT complement data. */
                                          curScannerFileEntry,
                                          scannerFileNumScanLines,
                                          flowMeterCounter,
                                          flowMeterCounterScanLine,
                                          flowRateRatio,
                                          flowRate
                                         );
      frame->SeqNum (nextFrameSeqNum);

      scannerFileNumScanLines += height;

      frame->Processed        (!dataIsToBeCounted);
      frame->WaitingToProcess (dataIsToBeCounted);
      if  (dataIsToBeCounted)
        waitingToProcess->PushOnBack (frame);

      frame->WrittenToDisk        (!dataIsToBeRecorded);
      frame->WaitingToWriteToDisk (dataIsToBeRecorded);
      if  (dataIsToBeRecorded)
        waitingToWriteToDisk->PushOnBack (frame);
    }
  }

  ++nextFrameSeqNum;

  gateKeeper->EndBlock ();
}  /* AddFrame */



CameraFramePtr  CameraFrameBuffer::GetNextFrameToWriteToDisk ()
{
  CameraFramePtr  result = NULL;

  gateKeeper->StartBlock ();

  if  (waitingToWriteToDisk->QueueSize () > 0)
  {
    result = waitingToWriteToDisk->PopFromFront ();
    result->WaitingToWriteToDisk (false);  // No longer in the 'WaitigToWriteToDisk' queue.  Its now in the middle of processing. */
  }

  gateKeeper->EndBlock ();
  return result;
}  /* GetNextFrameToWriteToDisk */



void  CameraFrameBuffer::DoneWrittingFrameToDisk (CameraFramePtr  frame)
{
  gateKeeper->StartBlock ();

  frame->WrittenToDisk (true);
  if  (frame->Processed ())
    available->PushOnBack (frame);

  gateKeeper->EndBlock ();
}  /* DoneWrittingFrameToDisk*/



CameraFramePtr  CameraFrameBuffer::GetNextFrameToProcess ()
{
  CameraFramePtr  result = NULL;

  gateKeeper->StartBlock ();

  if  (waitingToProcess->QueueSize () > 0)
  {
    result = waitingToProcess->PopFromFront ();
    result->WaitingToProcess (false);  // No longer in the 'WaitigToProcess' queue.  Its now in the middle of processing. */
  }

  gateKeeper->EndBlock ();
  return result;
}  /* GetNextFrameToProcess */



void  CameraFrameBuffer::DoneProcessingFrame (CameraFramePtr  frame)
{
  gateKeeper->StartBlock ();

  frame->Processed (true);
  if  (frame->WrittenToDisk ())
    available->PushOnBack (frame);

  gateKeeper->EndBlock ();
}  /* DoneProcessingFrame */



kkint32 CameraFrameBuffer::MemoryConsumedEstimated ()
{
  gateKeeper->StartBlock ();
  kkint32 memoryConsumed = sizeof (CameraFrameBuffer) + buffer->size () * frameHeight * frameWidth;
  gateKeeper->EndBlock ();
  return  memoryConsumed;
}  /* MemoryConsumedEstimated */



VectorUcharPtr  CameraFrameBuffer::CameraHighPoints ()  const
{
  if  (flatFieldCorrection)
    return flatFieldCorrection->CameraHighPoints ();
  else
    return NULL;
}  /* CameraHighPoints */



VectorUcharPtr  CameraFrameBuffer::CameraHighPointsFromLastNSampleLines (kkint32 n)  const
{
  if  (flatFieldCorrection)
    return flatFieldCorrection->CameraHighPointsFromLastNSampleLines (n);
  else
    return NULL;
}


void   CameraFrameBuffer::ScanRateChanged (float _newScanRate)
{
  ComputeFlatFeildSamplingInterval ();
}



void  CameraFrameBuffer::ComputeFlatFeildSamplingInterval ()
{
  float  scanRate = manager->RequestedScanRate ();
  if  ((scanRate < 100.0f)  ||  (scanRate > 50000))
    scanRate = 10000.0f;

  int  numSampleLines = 20;
  if  (flatFieldCorrection != NULL)
    numSampleLines = flatFieldCorrection->NumSampleLines ();

  flatFeildSamplingInterval = (kkint32)(0.5f + ((20.0f * scanRate) / numSampleLines) / frameHeight);
}
