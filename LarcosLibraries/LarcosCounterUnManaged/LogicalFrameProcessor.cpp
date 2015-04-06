/* LogicalFrameProcessor.cpp -- Responsible for extracting individual images from logical frames.
 * Copyright (C) 2011-2013  Kurt Kramer
 * For conditions of distribution and use, see copyright notice in LarcosCounterUnManaged.h
 */
#include "FirstIncludes.h"

#include <errno.h>
#include <istream>
#include <iostream>
#include <fstream>
#include <deque>
#include <queue>
#include <map>
#include <vector>
#include <math.h>
#include "MemoryDebug.h"
using namespace std;

#include "KKBaseTypes.h"
#include "Blob.h"
#include "ImageIO.h"
#include "OSservices.h"
#include "Raster.h"
#include "RasterBuffer.h"
using namespace KKB;

#include "ScannerFile.h"
#include "ScannerFileEntry.h"
using namespace  KKLSC;

#include "Classifier2.h"
#include "PostLarvaeFV.h"
#include "TrainingProcess2.h"
using  namespace KKMachineLearning;


#include "LogicalFrameProcessor.h"

#include "CameraFrame.h"
#include "CameraFrameBuffer.h"
#include "LarcosCounterManager.h"
#include "LarcosCounterStats.h"
#include "OperatingParameters.h"
#include "ParticleEntry.h"
#include "PostLarvaeFVProducer.h"
#include "ShrimpLengthComputer.h"
using  namespace  LarcosCounterUnManaged;


typedef  LogicalFrameProcessor::BlobIdType  BlobIdType;

volatile  GoalKeeperPtr  LogicalFrameProcessor::classifierBuildBlocker = NULL;




/*************************************************************************************/
/*  We will first define the Blob and BlobList class structures necessary to support */
/*  Connected component analysis.                                                    */
/*************************************************************************************/


/**
 * @class  LogicalFrameProcessor::Blob
 * @details  Meant to track a single detected blob in the 'logicalFrame'. A instance of 'LogicalFrameProcessor::BlobList' will
 *   keep track of all 'Blob' instances in a single logical frame.
 */
class  LogicalFrameProcessor::Blob
{
public:
  Blob ()
  {
    Reset ();
    id = 0;
  }

  ~Blob () {}

  BlobIdType  Id ()  {return  id;}

  kkint32  ColLeft    ()  {return colLeft;}
  kkint32  ColRight   ()  {return colRight;}
  kkint32  Height     ()  {return (1 + abs (rowBot - rowTop));}
  kkint32  RowBot     ()  {return rowBot;}
  kkint32  RowTop     ()  {return rowTop;}
  kkint32  Width      ()  {return (1 + abs (colLeft - colRight));}
  kkint32  PixelCount ()  {return pixelCount;}

  void  Reset ()
  {
    colLeft = colRight = pixelCount = rowBot = rowTop = 0;  inUse = false;
  }

  kkint32    colLeft;
  kkint32    colRight;
  BlobIdType id;
  bool       inUse;    /**<  Indicates whether this blob is in use;  false indicates available to use.*/
  kkint32    pixelCount;
  kkint32    rowBot;
  kkint32    rowTop;
};  /* Blob */




class  LogicalFrameProcessor::BlobList
{
public:
  typedef  Blob*  BlobPtr;

  BlobList (kkint32  _size):
     blobList          (NULL),
     blobListAllocated (0),
     blobsInUse        (0),
     freedUpBlobs      (),
     largestKnownRow   (0),
     nextBlobId        (0)
  {
    if  (_size > 30000)
    {
      cerr << endl << endl << "*** ****      _size > 30000       *** ***" << endl << endl;
    }

    blobList = new Blob[_size];
    blobListAllocated = _size;
    for  (kkint32 x = 0;  x < _size;  ++x)
      blobList[x].id = x;
  }
  
  ~BlobList ()
  {
    delete[] blobList;
    blobList = NULL;
  }


  kkint32  MemoryConsumedEstimated ()
  {
    kkint32  memoryConsumedEstimated = sizeof (BlobList) + sizeof (kkint32) * freedUpBlobs.size ();

    if  (blobList)
      memoryConsumedEstimated += blobListAllocated * sizeof (Blob);
    return  memoryConsumedEstimated;
  }  /* MemoryConsumedEstimated */



  void  GrowBlobList (kkint32 _increase)
  {
    kkint32  x = 0;
    kkint32  newBlobListAllocated = blobListAllocated + _increase;

    if  (newBlobListAllocated > 30000)
      cerr << endl << endl << "*** ****      _size > 30000       *** ***" << endl << endl;

    Blob*  newBlobList = new Blob[newBlobListAllocated];
    for  (x = 0;  x < blobListAllocated;  x++)
       newBlobList[x] = blobList[x];
    for  (x = blobListAllocated;  x < newBlobListAllocated;  ++x)
      newBlobList[x].id = x;
    delete[]  blobList;
    blobList = newBlobList;
    newBlobList = NULL;
    blobListAllocated = newBlobListAllocated;
  }


  void  Reset ()
  {
    nextBlobId = 0;
    freedUpBlobs.clear ();
    blobsInUse = 0;
    largestKnownRow = 0;
  }


  BlobPtr  NextAvailableBlob (BlobIdType**  blobIds)
  {
    BlobPtr  result = NULL;
    if  (nextBlobId < blobListAllocated)
    {
      result = &(blobList[nextBlobId]);
      result->id = nextBlobId;
      result->inUse = true;
      ++nextBlobId;
    }
    else
    {
      if  (freedUpBlobs.size () < 1)
        SearchForBlobsToDelete (blobIds);

      if  (freedUpBlobs.size () > 0)
      {
        // There are blobs available for reuse.
        kkint32  blobIdToReuse = freedUpBlobs.back ();
        freedUpBlobs.pop_back ();
        result = &(blobList[blobIdToReuse]);
        result->id = blobIdToReuse;
        result->inUse = true;
      }
    }
    if  (!result)
    {
      // Will have to expand blobList
      GrowBlobList (1000);
      result = &(blobList[nextBlobId]);
      result->id = nextBlobId;
      nextBlobId++;
    }

    ++blobsInUse;
    result->inUse = true;
    return result;
  }  /* NextAvailableBlob */



  void  BlobAvailableForReuse (BlobPtr       blob,
                               BlobIdType**  blobIds
                              )
  {
    BlobIdType  blobId = blob->id;
    kkint32  colRight = blob->colRight;
    kkint32  colLeft  = blob->colLeft;

    for  (kkint32 r = blob->rowTop;  r <= blob->rowBot;  ++r)
    {
      for  (kkint32 c = colLeft;  c <= colRight;  ++c)
        if  (blobIds[r][c] == blobId)
          blobIds[r][c] = -1;
    }

    freedUpBlobs.push_back (blobId);
    blob->rowTop = blob->rowBot = blob->colLeft = blob->colRight = 0;
    blob->inUse = false;
  }  /* BlobAvailableForReuse */



  void  SearchForBlobsToDelete (BlobIdType**  blobIds)
  {
    kkint32  rowThreshHold = largestKnownRow - 100;

    for  (kkint32 x = 0;  x < nextBlobId;  ++x)
    {
      if  (blobList[x].inUse  &&  (blobList[x].pixelCount < 100))
      {
        if  (blobList[x].rowBot < rowThreshHold)
          BlobAvailableForReuse (&(blobList[x]), blobIds);
      }
    }
  }  /* SearchForBlobsTodelete */



  BlobPtr  NewBlob (kkint32       _row,
                    kkint32       _col,
                    BlobIdType**  blobIds
                   )
  {
    BlobPtr  result = NextAvailableBlob (blobIds);
    result->rowTop   = _row;
    result->rowBot   = _row;
    result->colLeft  = _col;
    result->colRight = _col;
    result->inUse    = true;
    result->pixelCount = 0;

    if  (_row > largestKnownRow)
      largestKnownRow = _row;

    return  result;
  }  /* NewBlob */



  BlobPtr  LookUpByBlobId (kkint32  blobId)
  {
    if  (blobId < nextBlobId)
      return  &(blobList[blobId]);
    else
    {
      KKStr  errMsg;
      errMsg << "BlobList::LookUpByBlobId    ***ERROR***     Invalid BlobId[" << blobId << "]";
      cerr << errMsg << endl;
      return NULL;
    }
  }


  /** @brief  Merges the smaller blob into the larger blob.   */
  BlobPtr  MergeBlobIds (BlobIdType    blob1Id,
                         BlobIdType    blob2Id,
                         BlobIdType**  blobIds
                        )
  {
    BlobPtr blob1 = &(blobList[blob1Id]);
    BlobPtr blob2 = &(blobList[blob2Id]);

    BlobPtr  srcBlob  = NULL;
    BlobPtr  destBlob = NULL;
    if  (blob1->pixelCount > blob2->pixelCount)
    {
      srcBlob  = blob2;
      destBlob = blob1;
    }
    else
    {
      srcBlob  = blob1;
      destBlob = blob2;
    }

    BlobIdType  srcBlobId  = srcBlob->id;
    BlobIdType  destBlobId = destBlob->id;

    kkint32  col = 0;
    kkint32  row = 0;

    kkint32  rowBot   = srcBlob->rowBot;
    kkint32  colRight = srcBlob->colRight;

    for  (row = srcBlob->rowTop;  row <= rowBot;  ++row)
    {
      BlobIdType*  srcRow = blobIds[row];
      for  (col = srcBlob->colLeft;  col <= colRight;  ++col)
      {
        if  (srcRow[col] == srcBlobId)
          srcRow[col] = destBlobId;
      }
    }

    destBlob->rowTop   = Min (destBlob->rowTop,   srcBlob->rowTop);
    destBlob->rowBot   = Max (destBlob->rowBot,   srcBlob->rowBot);
    destBlob->colLeft  = Min (destBlob->colLeft,  srcBlob->colLeft);
    destBlob->colRight = Max (destBlob->colRight, srcBlob->colRight);
    destBlob->pixelCount = destBlob->pixelCount + srcBlob->pixelCount;

    --blobsInUse;

    BlobAvailableForReuse (srcBlob, blobIds);

    return  destBlob;
  }  /* MergeBlobIds */


  kkint32  NextBlobId ()  const  {return nextBlobId;}
  kkint32  BlobsInUse ()  const  {return blobsInUse;}


private:
  Blob*             blobList;
  kkint32           blobListAllocated;
  kkint32           blobsInUse;
  BlobIdVectorType  freedUpBlobs;        /**< Stack of blobs that have been deleted and are available for reuse; */

  kkint32           largestKnownRow;

  BlobIdType        nextBlobId;
};  /* BlobList */





LogicalFrameProcessor::LogicalFrameProcessor 
                                (LarcosCounterManagerPtr _manager,
                                 const KKStr&            _classifierName,
                                 OperatingParametersPtr  _operatingParameters,
                                 RasterBufferPtr         _lastParticlesProcessed,
                                 LogicalFrameListPtr     _logicalFrames,
                                 ParticleEntryBufferPtr  _particleEntryBuffer,
                                 const KKStr&            _particlesDirName,
                                 kkint32                 _frameHeight,     /**< Max logical frameHeight expected. */
                                 kkint32                 _frameWidth,      /**< Length of scan line.              */
                                 kkint32                 _cropLeft,
                                 kkint32                 _cropRight,
                                 MsgQueuePtr             _msgQueue,
                                 const KKStr&            _threadName
                                ):
    CameraThread  (_manager, _threadName,_msgQueue),

    available                  (false),
    backGroundPixelTH          (_operatingParameters->BackGroundPixelTH ()),
    classifier                 (NULL),
    classifierName             (_classifierName),
    connectedComponentDist     (_operatingParameters->ConnectedComponentDist ()),
    countingMethod             (cmStraightCount),
    countParticles             (true),
    cropLeft                   (_cropLeft),
    cropRight                  (_cropRight),
    erosionStructSize          (_operatingParameters->ErosionStructSize ()),
    extractedParticles         (NULL),
    flowRate                   (1.0f),
    flowRateRatio              (_operatingParameters->FlowRateFactor ()),
    lastParticlesProcessed     (_lastParticlesProcessed),
    lenComputer                (NULL),
    logicalFrames              (_logicalFrames),
    logicalFramesProcessed     (0),
    minSizeThreshold           (_operatingParameters->MinSizeThreshold ()),
    particleEntryBuffer        (_particleEntryBuffer),
    particleEntryProcCount     (0),
    particlesExtracted         (0),
    particlesCounted           (0),
    particlesDirName           (_particlesDirName),
    particlesWaitingProcessing (0),
    playingBack                (false),
    saveParticleImages         (_operatingParameters->SaveParticleImages ()),
    sizingSamplingInterval     (10),
    stretcher                  (NULL),
    subDirsCreated             (),
    trainer                    (NULL),
    unknownClass               (MLClass::GetUnKnownClassStatic ()),

    blobs                      (NULL),
    blobIds                    (NULL),
    blobIdsArea                (NULL),
    blobIdsMinusOnes           (NULL),
    bytesAllocated             (0),
    frameAreaUsed              (0),
    frameHeight                (_frameHeight),
    frameWidth                 (_frameWidth),
    frameHeightUsed            (0),
    frameLines                 (NULL),
    frameLinesArea             (NULL),
    frameStartScanLineNum      (0),
    logFrameScannerFileScanRow (0),
    workLinesArea              (NULL),
    workLines                  (NULL)


{
  log.Level (10) << ThreadName () << "::LogicalFrameProcessor" << endl;
  particlesDirName   = osAddSlash (particlesDirName);
  stretcher = new MorphOpStretcher (flowRateRatio, 1.0f);
  AllocateMemmory ();
  playingBack         = Manager ()->PlayingBack ();
  playingBackRealTime = Manager ()->PlayingBackRealTime ();

  kkint32  pixelsPerScanLine = Manager ()->ImagingChamberWidthPixels ();
  if  (pixelsPerScanLine < 1)
    pixelsPerScanLine = 1500;

  lenComputer = new ShrimpLengthComputer (pixelsPerScanLine, Manager ()->ImagingChamberWidth (), erosionStructSize, backGroundPixelTH);
  if  (playingBack  &&  (!playingBackRealTime))
    sizingSamplingInterval = 1;
  else
    sizingSamplingInterval = 30;
}



LogicalFrameProcessor::~LogicalFrameProcessor ()
{
  CleanUpMemory ();

  delete  lenComputer;        lenComputer        = NULL;
  delete  extractedParticles; extractedParticles = NULL;
  delete  stretcher;          stretcher          = NULL;
  delete  classifier;         classifier         = NULL;
  delete  trainer;            trainer            = NULL;
  log.Level (10) << ThreadName () << "::~LogicalFrameProcessor" << endl;
}



void  LogicalFrameProcessor::AllocateMemmory ()
{
  blobs = new BlobList (1000);
  //blobs = new BlobList (10000);
  bytesAllocated = frameWidth * frameHeight;
  blobIdsArea      = new BlobIdType[bytesAllocated];
  frameLinesArea   = NULL;  // Will point to 'frameLinesArea' in the 'LogicalFrame' instance currently being processed. */
  workLinesArea    = new uchar[bytesAllocated];

  extractedParticles = new ParticleEntryList (true);

  BlobIdType*  blobIdsAreaPtr   = blobIdsArea;
  uchar*       workLinesAreaPtr = workLinesArea;

  blobIds    = new BlobIdType* [frameHeight];
  workLines  = new uchar*[frameHeight];
  frameLines = NULL;       // Will point to 'frameLines' in the 'LogicalFrame' instance currently being processed. */

  blobIdsMinusOnes = new BlobIdType[frameWidth];
  for  (kkint32 col = 0;  col < frameWidth;  ++col)
    blobIdsMinusOnes[col] = -1;

  for  (kkint32 row = 0;  row < frameHeight;  ++row)
  {
    blobIds    [row] = blobIdsAreaPtr;
    workLines  [row] = workLinesAreaPtr;

    blobIdsAreaPtr   += frameWidth;
    workLinesAreaPtr += frameWidth;
  }

  cropLeft = Max (0, cropLeft);
  if   ((cropRight < 1)  ||  (cropRight >= frameWidth))
    cropRight = frameWidth - 1;

  if  (lenComputer)
    lenComputer->PixelsPerScanLine (1 + cropRight - cropLeft);
}  /* AllocateMemmory */



void  LogicalFrameProcessor::CleanUpMemory()
{
  delete   blobs;              blobs              = NULL;
  delete   blobIdsArea;        blobIdsArea        = NULL;
  delete   blobIds;            blobIds            = NULL;
  delete   workLinesArea;      workLinesArea      = NULL;
  delete   workLines;          workLines          = NULL;
  delete   blobIdsMinusOnes;   blobIdsMinusOnes   = NULL;
  delete   extractedParticles; extractedParticles = NULL;

  // We will not delete the 'frameLinesArea' and 'frameLines';  they will be owned by the instances of 'LogicalFrame';
  frameLinesArea   = NULL;
  frameLines       = NULL;
}



kkint32  LogicalFrameProcessor::MemoryConsumedEstimated ()  const
{
  int  memConsumed = sizeof (LogicalFrameProcessor);

  if  (blobs)
    memConsumed += blobs->MemoryConsumedEstimated ();

  if  (blobIdsArea)
  {
    //                                blobIdsArea          + workLinesArea;
    memConsumed += (bytesAllocated * (sizeof (BlobIdType)  + sizeof (uchar)));
    memConsumed += (frameHeight    * (sizeof (BlobIdType*) + sizeof (char*)));
  }

  if  (blobIdsMinusOnes)
     memConsumed += (frameWidth * sizeof (BlobIdType));

  if  (classifier)
    memConsumed += classifier->MemoryConsumedEstimated ();

  if  (trainer)
    memConsumed += trainer->MemoryConsumedEstimated ();

  return  memConsumed;
}



void  LogicalFrameProcessor::ReAllocateMemory (kkint32  _newFrameHeight,
                                               kkint32  _newFrameWidth
                                              )
{
  CleanUpMemory ();
  frameWidth  = _newFrameWidth;
  if  (_newFrameHeight > frameHeight)
    frameHeight = _newFrameHeight;
  AllocateMemmory ();
}



void  LogicalFrameProcessor::CropSettingsChanged (kkint32  _cropLeft,
                                                  kkint32  _cropRight
                                                 )
{
  cropLeft  = _cropLeft;
  cropRight = _cropRight;
  if  (lenComputer)
    lenComputer->PixelsPerScanLine (1 + cropRight - cropLeft);
}



LogicalFramePtr  LogicalFrameProcessor::GetNextFrame ()
{
  LogicalFramePtr  frame = logicalFrames->GetNextFrameToProcess ();
  while   ((frame == NULL)  &&  (!TerminateFlag ())  &&  (!ShutdownFlag ()))
  {
    available = true;
    osSleep (0.01f);
    frame = logicalFrames->GetNextFrameToProcess ();
  }
  available = false;

  if  (!frame)
  {
    frameLines     = NULL;
    frameLinesArea = NULL;
  }

  return  frame;
 }  /* GetNextFrame */



void  LogicalFrameProcessor::LoadClassifer ()
{
  log.Level (20) << ThreadName () << "::LoadClassifer  classifierName: " << classifierName << endl;
  delete  classifier;   classifier = NULL;
  delete  trainer;      trainer    = NULL;

  if  ((classifierName.EqualIgnoreCase ("PostLarvae"))  ||  (classifierName.EqualIgnoreCase ("Post Larvae")))
  {
    countingMethod = cmPostLarvae;
    log.Level (20) << "LoadClassifer   Utilizing PostLarvae Counting Method." << endl;
  }

  else if  (classifierName.EqualIgnoreCase ("None")  ||  classifierName.Empty ())
  {
    countingMethod = cmStraightCount;
    log.Level (20) << "LoadClassifer   Utilizing Straight-Count Counting Method." << endl;
  }
  else
  {
    if  (classifierBuildBlocker == NULL)
      GoalKeeper::Create ("ClassifierBuildBlocker", classifierBuildBlocker);
    classifierBuildBlocker->StartBlock ();

    KKStr  statusMsg = " Loading Training Library[" + classifierName + "]";
    log.Level (10) << "LoadClassifer   " << statusMsg << endl;

    countingMethod = cmClassier;

    // Since the 'LarcosCounterManager' has already built and saved this classifier we already know 
    // that it is good and we can go ahead and load the saved copy.
    trainer = new TrainingProcess2 (classifierName,
                                    PostLarvaeFVProducerFactory::Factory (&log),
                                    log,
                                    false,              /**<  'false' = Features are already normalized. */
                                    TerminateFlag (),
                                    statusMsg
                                   );

    classifierBuildBlocker->EndBlock ();

    if  (trainer->Abort ())
    {
      log.Level (-1) << endl
                     << ThreadName () << "::LoadClassifer   ***ERROR***  Loading Training-Model: " << classifierName << endl
                     << "                                   Reason: " << statusMsg << endl
                     << endl;
      Crashed (true);
      return;
    }

    classifier = new Classifier2 (trainer, log);
  }
}  /* LoadClassifer */




// This thread will include counting.
void  LogicalFrameProcessor::Run ()
{
  log.Level (10) << ThreadName () << "::Run" << endl;
  Status (tsRunning);

  LoadClassifer ();
  if  (Crashed ())
  {
    Status (tsStopping);
    return;
  }

  LogicalFramePtr  frame = GetNextFrame ();
  while  ((!TerminateFlag ())   &&  (frame != NULL))
  {
    flowRate      = frame->FlowRate ();
    flowRateRatio = frame->FlowRateRatio ();

    if  ((frame->FrameHeightUsed () > frameHeight)  ||  (frame->FrameWidth () != frameWidth))
      ReAllocateMemory (frame->FrameHeightUsed (), frame->FrameWidth ());

    frameStartScanLineNum      = frame->ScanLineNumStart      ();
    scannerFileRootName        = frame->ScannerFileRootName   ();
    logFrameScannerFileScanRow = frame->ScanLineNumStartCurSF ();

    frameLines     = frame->FrameLines     ();
    frameLinesArea = frame->FrameLinesArea ();

    frameHeightUsed = frame->FrameHeightUsed ();
    frameAreaUsed   = frame->FrameAreaUsed ();

    if  (countParticles)
      ProcessFrame ();

    // we do not own 'frame', 'frameLines', or 'frameLinesArea',
    logicalFrames->FrameProcessed (frame);
    frame = NULL;
    frameLines = NULL;
    frameLinesArea = NULL;

    frame = GetNextFrame ();
  }

  CleanUpMemory ();

  log.Level (10) << ThreadName () << "::Run    EXITING"
                 << "   No-More-Frames: " << (frame == NULL)
                 << "   TerminateFlag: "  << TerminateFlag ()
                 << "   ShutdownFlag: "   << ShutdownFlag ()
                 << endl;

  Status (tsStopping);
}  /* Run */



void  LogicalFrameProcessor::ProcessFrame ()
{
  uchar** scanLines = NULL;
  uchar*  scanLinesArea = NULL;

  if  (erosionStructSize > 0)
  {
    ErodeImage (frameLines, workLinesArea, workLines, erosionStructSize);
    scanLines     = workLines;
    scanLinesArea = workLinesArea;
  }
  else
  {
    scanLines     = frameLines;
    scanLinesArea = frameLinesArea;
  }

  //ExtractBlobs (workLinesArea, workLines);
  ExtractBlobs (scanLinesArea, scanLines);

  kkint32  idx;

  particlesWaitingProcessing = blobs->BlobsInUse ();

  for  (idx = 0;  idx < blobs->NextBlobId ();  ++idx)
  {
    BlobPtr  blob = blobs->LookUpByBlobId (idx);

    if  (!blob->inUse)
      continue;

    --particlesWaitingProcessing;

    if  ((!blob)  ||  ((int)(0.5f + blob->PixelCount () * flowRateRatio) < minSizeThreshold)  ||  (!blob->inUse))
      continue;

    if  (TerminateFlag ())
      break;

    bool  noise = false;
    {
      // Some artifact Line Line Detection 
      if  (blob->Width () < 20)
        noise = true;
      else
      {
        float  heightToWidth = (float)blob->Height () / (float)blob->Width ();
        if  (heightToWidth > 25)
          noise = true;
      }
    }

    kkint32  scanRow = logFrameScannerFileScanRow + blob->rowTop;
    kkint32  scanCol = blob->colLeft;

    kkint32  divisor = 1;

    if  (noise)
    {
      if  (saveParticleImages)
      {
        RasterPtr  i = NULL;

        if  (blob->Height () > 2000)
        {
          divisor = (kkint32)ceil ((float)blob->Height () / (float)2000);
          i = ExtractABlob2 (scanLinesArea, scanLines, blob, divisor);
        }
        else
        {
          i = ExtractABlob (scanLinesArea, scanLines, blob);
        }

        KKStr  subDirName = osAddSlash (particlesDirName + "NoiseParticles");
        /**@todo  I should create the 'NoiseParticles' sub-directory in the controlling object.  */
        osCreateDirectoryPath (subDirName);
        KKStr  fullFileName = subDirName + scannerFileRootName + 
                                        "_"  + StrFormatInt64 (scanRow, "00000000")  +
                                        "_"  + StrFormatInt64 (scanCol, "0000")      +
                                        ".bmp";
        SaveImageGrayscaleInverted8Bit (*i, fullFileName);
        delete  i;   i  = NULL;
      }
    }
    else
    {
      RasterPtr  i = NULL;

      if  (blob->Height () > 8000)
      {
        divisor = (kkint32)ceil ((float)blob->Height () / (float)8000);
        i = ExtractABlob2 (scanLinesArea, scanLines, blob, divisor);
      }
      else
      {
        i = ExtractABlob (scanLinesArea, scanLines, blob);
      }

      i->BackgroundPixelTH (backGroundPixelTH);

      KKStr  imageRootName = scannerFileRootName  + 
                             "_"  + StrFormatInt64 (scanRow, "00000000")  +
                             "_"  + StrFormatInt64 (scanCol, "0000")      +
                             ".bmp";
      i->FileName (imageRootName);
      i->Divisor (divisor);

      if  (divisor > 1)
        log.Level (20) << ThreadName () << "::ProcessFrame  Reduce Image: " << imageRootName << "  by: " << divisor << endl;

      switch  (countingMethod)
      {
      case  cmStraightCount:
      case  cmNULL:
        AnalyseParticleStraightCount (i, scanRow, scanCol, blob);
        break;

      case  cmPostLarvae:
        AnalyseParticlePostLarvae (i, scanRow, scanCol, blob);
        break;

      case  cmClassier:
        AnalyseParticleUsingClassifier (i, scanRow, scanCol, blob);
        break;
      }

      if  (lastParticlesProcessed)
      {
        lastParticlesProcessed->AddRaster (i);
        i = NULL;
      }
      else
      {
        delete  i;  
        i = NULL;
      }
    }
    ++particlesExtracted;
  }

  particleEntryBuffer->Add (*extractedParticles);

  ++logicalFramesProcessed;
}  /* ProcessFrame */




void  LogicalFrameProcessor::AnalyseParticleStraightCount (RasterPtr  particle,
                                                           kkint32    scanRow,
                                                           kkint32    scanCol,
                                                           BlobPtr    blob
                                                          )
{
  if  (saveParticleImages)
    SaveParticle (particle, 1, unknownClass, scanRow, scanCol);

  AddParticleEntry (scanRow, scanCol, particle, unknownClass, blob, 1.0f, 0.0f);

  kkint32 particleSize = particle->ForegroundPixelCount ();
  if  (particleSize <= 0)
    particleSize = particle->CalcArea ();

  UpdateCounts (particleSize, 1);
}  /* AnalyseParticle */



void  LogicalFrameProcessor::AnalyseParticlePostLarvae (RasterPtr  particle,
                                                        kkint32    scanRow,
                                                        kkint32    scanCol,
                                                        BlobPtr    blob
                                                       )
{
  RasterPtr  darkSpots = particle->BinarizeByThreshold (200, 255);
  darkSpots->Erosion (MorphOp::SQUARE3);
  darkSpots->Erosion (MorphOp::SQUARE3);

  KKB::BlobListPtr  blobs = darkSpots->ExtractBlobs (1);
  kkint32 countThisParticle = 0;
  {
    int  th = 4;
    KKB::BlobList::iterator  idx;
    for  (idx = blobs->begin ();  idx != blobs->end ();  ++idx)
    {
      KKB::BlobPtr  blob = idx->second;
      if  (blob->PixelCount () > th)
        ++countThisParticle;
    }
  }

  //kkint32 countThisParticle = blobs->QueueSize ();
  if  (countThisParticle < 1)
    countThisParticle = 1;

  delete  blobs;      blobs     = NULL;
  delete  darkSpots;  darkSpots = NULL;

  KKStr  className = "Shrimp_" + StrFormatInt (countThisParticle, "000");

  MLClassPtr  ic = MLClass::CreateNewMLClass (className, 01);
  if  (ic)
    particle->Title (ic->Name ());

  if  (saveParticleImages)
    SaveParticle (particle, countThisParticle, ic, scanRow, scanCol);
  AddParticleEntry (scanRow, scanCol, particle, ic, blob, 1.0f, 0.0f);

  kkint32 particleSize = particle->ForegroundPixelCount ();
  if  (particleSize <= 0)
    particleSize = particle->CalcArea ();

  UpdateCounts (particleSize, (float)countThisParticle);
}  /* AnalyseParticlePostLarvae */





void  LogicalFrameProcessor::AnalyseParticleUsingClassifier (RasterPtr  particle,
                                                             kkint32    scanRow,
                                                             kkint32    scanCol,
                                                             BlobPtr    blob
                                                            )
{
  PostLarvaeFVPtr  fv = new PostLarvaeFV (*particle, unknownClass, NULL);

  double  probability  = 0.0;
  double  breakTie     = 0.0;
  int     numOfWinners = 0;
  bool    knownClassOnOfWinners = false;

  MLClassPtr  predictedClass = classifier->ClassifyAImage (*fv, probability, numOfWinners, knownClassOnOfWinners, breakTie);
  if  (predictedClass)
    particle->Title (predictedClass->Name ());
  UpdateCounts ((kkint32)fv->OrigSize (),  predictedClass->CountFactor ());
  delete  fv;   fv = NULL;
  if  (saveParticleImages)
    SaveParticle (particle, (kkint32)(predictedClass->CountFactor ()), predictedClass, scanRow, scanCol);
  AddParticleEntry (scanRow, scanCol, particle, predictedClass, blob, (float)probability, (float)breakTie);
}  /* AnalyseParticleUsingClassifier */



void  LogicalFrameProcessor::AddParticleEntry (kkint32     scanRow,
                                               kkint32     scanCol,
                                               RasterPtr   particle,
                                               MLClassPtr  ic,
                                               BlobPtr     blob,
                                               float       probability,
                                               float       breakTie
                                              )
{
  // because the images were adjusted by a FlowRate factor I need to reverse it to get the 
  // correct height with respect to the scanner file.
  //kkint32  origHeight = (kkint32)(0.5f + (float)particle->Height () / flowRateRatio);

  float  orientation = 0.0f;
  float  length = 0.0f;

  if  (lenComputer  &&  (ic->CountFactor () == 1))
  {
    if  ((particle->Height () < 1000)  && (particle->Width () < 1000))
    {
      if  ((particleEntryProcCount % sizingSamplingInterval) == 0)
      {
        lenComputer->ProcessImage (particle, false);
        orientation = lenComputer->OrientationAngleInDegrees ();
        if  ((!lenComputer->ImageIsDistorted ())  &&  (!lenComputer->ImageIsOccluded ()))
          length = lenComputer->LenInMiliMeters ();
      }
      ++particleEntryProcCount;
    }
  }

  extractedParticles->PushOnBack (new ParticleEntry (scannerFileRootName, 
                                                     scanRow,
                                                     scanCol,
                                                     blob->Height (),
                                                     blob->Width (),
                                                     ic,
                                                     orientation,
                                                     length,
                                                     flowRateRatio,
                                                     flowRate,
                                                     probability,
                                                     breakTie
                                                    )
                                 );
}




void  LogicalFrameProcessor::SaveParticle (RasterPtr   particle,
                                           kkint32     countThisParticle,
                                           MLClassPtr  mlClass,
                                           kkint32     scanRow,
                                           kkint32     scanCol
                                          )
{
  KKStr  subDirPath = osAddSlash (particlesDirName) + mlClass->Name ();

  set<KKStr>::iterator  idx;
  idx = subDirsCreated.find (subDirPath);
  if  (idx == subDirsCreated.end ())
  {
    subDirsCreated.insert (subDirPath);
    osCreateDirectoryPath (subDirPath);
  }
  
  KKStr  particleFileRootName = scannerFileRootName + "_" + StrFormatInt64 (scanRow, "00000000") + "-" + StrFormatInt (scanCol, "0000");
  KKStr  fullFileName = osAddSlash (subDirPath) + particleFileRootName + ".bmp";

  try
  {
    SaveImageGrayscaleInverted8Bit (*particle, fullFileName); 
  }
  catch  (const KKException&  e)
  {
    log.Level (-1) << endl
                   << "LogicalFrameProcessor::SaveParticle   ***ERROR***   Exception saving image: " << particleFileRootName << endl
                   << "          Error description " << e.ToString () << endl
                   << endl;
  }
  catch  (...)
  {
    log.Level (-1) << endl
                   << "LogicalFrameProcessor::SaveParticle   ***ERROR***   Exception thrown calling 'SaveImageGrayscaleInverted8Bit' for: " << particleFileRootName << endl
                   << endl;
  }
}  /* SaveParticle */




void  LogicalFrameProcessor::UpdateCounts (kkint32  size,
                                           float  numCounted
                                          )
{
  particlesCounted += (kkint32)(numCounted + 0.5f);
}





//========================================================================================================
//========================================================================================================
//========================================================================================================
//========================================================================================================


BlobIdType  LogicalFrameProcessor::NearestNeighborUpperLeft (kkint32  row,
                                                             kkint32  col
                                                            )
{
  BlobIdType  nearestBlob = -1;
  kkint32  c = 0, r = 0, startCol = 0;

  startCol = Max (col - 1, cropLeft);

  for (r = row - connectedComponentDist;  r < row;  ++r)
  {
    if  (r >= 0)
    {
      BlobIdType*  blobRow = blobIds[r];
      for  (c = startCol;  c <= col;  ++c)
        nearestBlob = Max (blobRow[c], nearestBlob);
    }

    if  (startCol > cropLeft)
      --startCol;
  }

  return  nearestBlob;
}  /* NearestNeighborUpperLeft */




BlobIdType  LogicalFrameProcessor::NearestNeighborUpperRight (kkint32  row,
                                                              kkint32  col
                                                             )

{
  BlobIdType  nearestBlobId = -1;
  kkint32     r = 0, c = 0, endCol = 0;

  endCol = Min (col + 1, cropRight);
  for  (r = row - connectedComponentDist;  r < row;  ++r)
  {
    if  (r >= 0)
    {
      BlobIdType*  blobRow = blobIds[r];
      for  (c = col + 1;  c <= endCol;  ++c)
      {
        nearestBlobId = Max (nearestBlobId, blobRow[c]);
      }
    }

    if  (endCol < cropRight)
      ++endCol;
  }

  return  nearestBlobId;
}  /* NearestNeighborUpperRight */
   




void   LogicalFrameProcessor::ExtractBlobs (uchar*  rowsArea,
                                            uchar** rows
                                           )
{
  uchar*        curRow         = NULL;
  BlobIdType*   curRowBlobIds  = NULL;

  kkint32  col = cropLeft + 2;
  kkint32  row = 0;

  BlobPtr     curBlob    = NULL;
  BlobIdType  curBlobId  = 0;
  BlobIdType  nearBlobId = 0;

  kkint32  blankColsInARow = 0;

  blobs->Reset ();

  for  (row = 0;  row < frameHeightUsed;  ++row)
  {
    curRow         = rows[row];
    curRowBlobIds  = blobIds[row];
    KKStr::MemCpy (curRowBlobIds, blobIdsMinusOnes, frameWidth * sizeof (blobIdsMinusOnes[0]));

    curBlob = NULL;

    col = cropLeft;
    while  (col <= cropRight)
    {
      if  (curRow[col] > backGroundPixelTH)
      {
        blankColsInARow = 0;

        nearBlobId = NearestNeighborUpperLeft (row, col);
        if  (nearBlobId < 0)
          nearBlobId = NearestNeighborUpperRight (row, col);

        if  (curBlob)
        {
          if  (nearBlobId >= 0)
          {
            if  (nearBlobId != curBlobId)
            {
              curBlob = blobs->MergeBlobIds (nearBlobId, curBlobId, blobIds);
              curBlobId = curBlob->Id ();
            }
          }

          curRowBlobIds[col] = curBlobId;
          curBlob->colRight  = Max (curBlob->colRight, col);
          curBlob->rowBot    = Max (curBlob->rowBot,   row);
          curBlob->pixelCount++;
        }
        else
        {
          // No Current Blob
          if  (nearBlobId >= 0)
          {
            curBlob   = blobs->LookUpByBlobId (nearBlobId);
            curBlobId = curBlob->id;
          }
          else
          {
            curBlob = blobs->NewBlob (row, col, blobIds);
            curBlobId = curBlob->id;
          }

          curRowBlobIds[col] = curBlobId;
          curBlob->colLeft   = Min (curBlob->colLeft,  col);
          curBlob->colRight  = Max (curBlob->colRight, col);
          curBlob->rowBot    = Max (curBlob->rowBot,   row);
          curBlob->rowTop    = Min (curBlob->rowTop,   row);
          curBlob->pixelCount++;
        }
      }
      else
      {
        // Background Pixel
        if  (curBlob)
        {
          nearBlobId = NearestNeighborUpperLeft (row, col);

          if  (nearBlobId >= 0)
          {
            if  (nearBlobId != curBlobId)
            {
              curBlob = blobs->MergeBlobIds (nearBlobId, curBlobId, blobIds);
              curBlobId = curBlob->Id ();
            }
          }
        }
        
        blankColsInARow++;
        if  (blankColsInARow > connectedComponentDist)
        {
          curBlob = NULL;
          curBlobId = -1;
        }
      }

      col++;
    }
  }

  return ;
}  /* ExtractBlobs */




RasterPtr  LogicalFrameProcessor::ExtractABlob (uchar*         rowsArea,
                                                uchar**        rows,
                                                const BlobPtr  blob
                                               )
{
  kkint32  height = blob->Height () + 4;
  kkint32  width  = blob->Width  () + 4;

  RasterPtr  blobRaster = new Raster (height, width, false);
  kkint32  blobId = blob->id;

  kkint32  row;
  kkint32  col;

  kkint32  rasterRow = 2;
  kkint32  rowBot = blob->rowBot;
  for  (row = blob->rowTop;  row <= rowBot;  ++row)
  {
    kkint32      rasterCol = 2;
    uchar*       destRow   = blobRaster->Green ()[rasterRow];
    uchar*       srcRow    = rows[row];
    BlobIdType*  blobIdRow = blobIds[row];

    kkint32  colRight = blob->colRight;

    for  (col = blob->colLeft;  col <= colRight;  ++col)
    {
      if  (blobIdRow[col] == blobId)
      {
        destRow[rasterCol] = srcRow[col];
      }
      rasterCol++;
    }
    rasterRow++;
  }

  {
    //  Make sure that curent stretcher is within 5% of latest flowRateRatio; if not create a new one.
    float deltaRowFactor = (float)fabs (flowRateRatio - stretcher->RowFactor ());
    if  ((deltaRowFactor / flowRateRatio) > 0.05)
    {
      delete  stretcher;
      stretcher = new MorphOpStretcher (flowRateRatio, 1.0f);
    }
  }

  RasterPtr  stretchedRaster = stretcher->PerformOperation (blobRaster);
  delete  blobRaster;
  blobRaster = NULL;

  return  stretchedRaster;
}  /* ExtractABlob */



RasterPtr  LogicalFrameProcessor::ExtractABlob2 (uchar*         rowsArea,
                                                 uchar**        rows,
                                                 const BlobPtr  blob,
                                                 kkint32        divisor
                                                )
{
  kkint32  heightReduced = blob->Height () / divisor;
  kkint32  widthReduced  = blob->Width  () / divisor;

  if  (heightReduced < 1)   heightReduced = 1;
  if  (widthReduced  < 1)   widthReduced  = 1;

  kkint32  height = heightReduced + 4;
  kkint32  width  = widthReduced  + 4;

  RasterPtr  blobRaster = new Raster (height, width, false);
  kkint32  blobId = blob->id;

  kkint32  row;
  kkint32  col;

  kkint32  rasterRow = 2;

  for  (row = 0;  row < heightReduced;  ++row)
  {
    kkint32 rasterCol = 2;
    uchar*  destRow   = blobRaster->Green ()[rasterRow];

    kkint32 srcRowTop = blob->rowTop + row * divisor;
    kkint32 srcRowBot = srcRowTop + divisor - 1;

    for  (col = 0;  col < widthReduced;  ++col)
    {

      kkint32  srcColLeft  = blob->colLeft + col * divisor;
      kkint32  srcColRight = Max (srcColLeft + divisor - 1, blob->colRight);

      kkint32  totPV = 0;
      kkint32  totPVcount = 0;

      for  (kkint32  srcRow = srcRowTop;  srcRow <= srcRowBot;  ++srcRow)
      {
        for  (kkint32  srcCol = srcColLeft;  srcCol <= srcColRight;  ++srcCol)
        {
          if  (blobIds[srcRow][srcCol] == blobId)
          {
            totPV += rows[srcRow][srcCol];
            ++totPVcount;
          }
        }
      }

      if  (totPVcount > 0)
        destRow[rasterCol] = (totPV / totPVcount);
      else
        destRow[rasterCol] = 0;

      rasterCol++;
    }
    rasterRow++;
  }

  {
    //  Make sure that curent stretcher is within 5% of latest flowRateRatio; if not create a new one.
    float deltaRowFactor = (float)fabs (flowRateRatio - stretcher->RowFactor ());
    if  ((deltaRowFactor / flowRateRatio) > 0.05)
    {
      delete  stretcher;
      stretcher = new MorphOpStretcher (flowRateRatio, 1.0f);
    }
  }

  RasterPtr  stretchedRaster = stretcher->PerformOperation (blobRaster);
  delete  blobRaster;
  blobRaster = NULL;

  return  stretchedRaster;
}  /* ExtractABlob2 */




void  LogicalFrameProcessor::ErodeImage (uchar**  srcRaster,
                                         uchar*   destRasterArea,
                                         uchar**  destRaster,
                                         kkint32  maskBias
                                        )
{
  kkint32  height = frameHeightUsed;

  KKStr::MemSet (destRasterArea, 0, frameAreaUsed);
  uchar*  destRow = NULL;
  uchar*  srcRow  = NULL;
  
  kkint32  row, col;

  kkint32  maskRowStart = 0;
  kkint32  maskRowEnd   = 0;
  kkint32  maskColStart = 0;
  kkint32  maskColEnd   = 0;
  kkint32  maskRow = 0;
  kkint32  maskCol = 0;
  bool  fit = false;

  for  (row = 0;  row < height;  ++row)
  {
    destRow = destRaster[row];
    srcRow  = srcRaster[row];

    maskRowStart = Max (0, row - maskBias);
    maskRowEnd   = Min (height - 1, row + maskBias);

    for  (col = 0;  col < frameWidth;  ++col)
    {
      if  (srcRow[col] > backGroundPixelTH)
      {
        fit = true;
        maskColStart = Max (cropLeft,  col - maskBias);
        maskColEnd   = Min (cropRight, col + maskBias);
        for  (maskRow = maskRowStart;  (maskRow <= maskRowEnd)  &&  fit;  ++maskRow)
        {
          for  (maskCol = maskColStart;  maskCol <= maskColEnd;  ++maskCol)
          {
            if  (srcRaster[maskRow][maskCol] <= backGroundPixelTH)
            {
              fit = false;
              break;
            }
          }
        }
        if  (fit)
          destRow[col] = srcRow[col];
      }
    }  /* for (col) */
  }  /* for (row) */
}  /* ErodeImage */




void  LogicalFrameProcessor::ResetCounts ()
{
  logicalFramesProcessed     = 0;
  particlesExtracted         = 0;
  particlesCounted           = 0;
  particlesWaitingProcessing = 0;
}




LogicalFrameProcessorList::LogicalFrameProcessorList (bool _owner):
   KKQueue<LogicalFrameProcessor>  (_owner)
{
}



LogicalFrameProcessorList::~LogicalFrameProcessorList ()
{
}



kkint32  LogicalFrameProcessorList::GetNumLogicalFrameProcessorsAvailable ()
{
  kkint32  logicalFrameProcessorsAvailable = 0;
  LogicalFrameProcessorList::const_iterator  idx;
  for  (idx = begin ();  idx != end ();  ++idx)
  {
    if  ((*idx)->Available ())
      ++logicalFrameProcessorsAvailable;
  }
  return  logicalFrameProcessorsAvailable;
}



void  LogicalFrameProcessorList::GetStats (LarcosCounterStats&  larcosCameraStats) const
{
  kkint32 logicalFrameProcessorsAvailable = 0;
  kkint32 logicalFramesProcessed         = 0;
  kkint32 particlesExtracted             = 0;
  kkint32 particlesCounted               = 0;
  kkint32 particlesWaitingProcessing     = 0;

  LogicalFrameProcessorList::const_iterator  idx;
  for  (idx = begin ();  idx != end ();  ++idx)
  {
    LogicalFrameProcessorPtr  lfp = *idx;
    logicalFramesProcessed     += lfp->LogicalFramesProcessed     ();
    particlesExtracted         += lfp->ParticlesExtracted         ();
    particlesCounted           += lfp->ParticlesCounted           ();
    particlesWaitingProcessing += lfp->ParticlesWaitingProcessing ();
    if  (lfp->Available ())
      ++logicalFrameProcessorsAvailable;
  }

  larcosCameraStats.LogicalFrameProcessorsAvailable (logicalFrameProcessorsAvailable);
  larcosCameraStats.LogicalFramesProcessed          (logicalFramesProcessed);
  larcosCameraStats.ParticlesExtracted              (particlesExtracted);
  larcosCameraStats.ParticlesCounted                (particlesCounted);
  larcosCameraStats.ParticlesWaitingProcessing      (particlesWaitingProcessing);

  return;
}  /* GetStats */


void  LogicalFrameProcessorList::ShutdownThreads ()
{
  LogicalFrameProcessorList::iterator  idx;
  for  (idx = begin ();  idx != end ();  ++idx)
  {
    LogicalFrameProcessorPtr  lfp = *idx;
    lfp->ShutdownThread ();
  }
  return;
}  /* ShutDownThreads */



void  LogicalFrameProcessorList::TerminateThreads ()
{
  LogicalFrameProcessorList::iterator  idx;
  for  (idx = begin ();  idx != end ();  ++idx)
  {
    LogicalFrameProcessorPtr  lfp = *idx;
    lfp->TerminateThread ();
  }

  for  (idx = begin ();  idx != end ();  ++idx)
  {
    LogicalFrameProcessorPtr  lfp = *idx;
    lfp->WaitForThreadToStop (1);
  }

  return;
}  /* TerminateThreads */



bool  LogicalFrameProcessorList::ThreadsStillRunning ()  const
{
  bool  threadsStillRunning = false;

  LogicalFrameProcessorList::const_iterator  idx;
  for  (idx = begin ();  idx != end ();  ++idx)
  {
    LogicalFrameProcessorPtr  lfp = *idx;
    if  (lfp->ThreadStillProcessing ())
    {
      threadsStillRunning = true;
      break;
    }
  }
  return  threadsStillRunning;
}  /* ThreadsStillRunning */




void  LogicalFrameProcessorList::WaitForThreadsToStop (kkint32  secsToWait)
{
  kkuint32  maxCount = secsToWait * 5;
  if  (secsToWait == 0)
    maxCount = 10000000;

  kkuint32  count = 0;
  while ((count < maxCount)  &&   ThreadsStillRunning ())
  {
    osSleep (0.2f);
    ++count;
  }

  LogicalFrameProcessorList::iterator  idx;
  for  (idx = begin ();  idx != end ();  ++idx)
  {
    LogicalFrameProcessorPtr  lfp = *idx;
    lfp->WaitForThreadToStop (1);
  }
}  /* WaitForThreadsToStop */
