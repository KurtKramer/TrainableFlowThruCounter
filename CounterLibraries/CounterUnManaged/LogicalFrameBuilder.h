/* LogicalFrameBuilder.h -- Will process frames coming from the camera and create logical frames to be processed by a 'LogicalGFrameProcessor'
 * Copyright (C) 2011-2013  Kurt Kramer
 * For conditions of distribution and use, see copyright notice in CounterUnManaged.txt
 */
#if  !defined(_LOGICALFRAMEVUILDERPROCESSOR_)
#define  _LOGICALFRAMEVUILDERPROCESSOR_

#include "GoalKeeper.h"
#include "KKQueue.h"
#include "RunLog.h"
#include "Raster.h"
#include "RasterBuffer.h"

#include "ScannerFile.h"

#include "CameraThread.h"
#include "CameraFrameBuffer.h"
#include "LogicalFrame.h"


//***************************************************************************************************
//  This class is meant as a replacement for "CameraImageProcessor". It is meant to create logical
//  Frames and add them to a queue,
//
// A new class called "LogicalFrameProcessor" is meant to remove entries from the logical 
// from queue and break them into ROI's.
//***************************************************************************************************

#include "PreClassDefinitions.h"

namespace CounterUnManaged
{
  #if  !defined(_CAMERAFRAMEBUFFER_)
  class  CameraFrameBuffer;
  typedef  CameraFrameBuffer*  CameraFrameBufferPtr;
  #endif

  #if  !defined(_CAMERAFRAME_)
  class  CameraFrame;
  typedef  CameraFrame*  CameraFramePtr;
  #endif

  #if  !defined(_LARCOSCOUNTERSTATS_)
    class  CounterStats;
    typedef  CounterStats*  CounterStatsPtr;
  #endif



  /**
   *@brief  Manages the thread that creates logical frames from physical frames.
   *@details Retrieves Frames from "cameraBuffer"(an instance of "CameraFrameBuffer"), which is populated by an
   *         instance of "CameraAcquisition", and creates logical frames ("LogicalFrame") which will then be inserted
   *         into "logicalFrames",an instance of "LogicalFrameList".  Logical frames are created by scanning physical
   *         frames for 3 blank scan line rows. If no such break is detected within a specified number of scan-lines
   *         then the scan-line with the least number of foreground pixels will be selected as a breaking point
   *         for the logical frame.
   */
  class LogicalFrameBuilder:  public  CameraThread
  {
  public:
    LogicalFrameBuilder (CounterManagerPtr   _manager,
                         CameraFrameBufferPtr      _cameraBuffer,
                         LogicalFrameListPtr       _logicalFrames,
                         LogicalFrameEntryListPtr  _logicalFrameEntries,
                         LogicalFrameEntryQueuePtr _droppedFrames,
                         kkint32                   _scanLineLen,
                         kkint32                   _scanLinesToAllocate,
                         uchar                     _backGroundPixelTH,
                         MsgQueuePtr               _msgQueue,
                         const KKStr&              _threadName
                        );

    virtual
      ~LogicalFrameBuilder ();
    
    // Access Methods
    kkint32 CropLeft                    () const  {return cropLeft;}
    kkint32 CropRight                   () const  {return cropRight;}
    bool    DroppedFramesBeingProcessed () const  {return droppedFramesBeingProcessed;}
    kkint32 LogicalFramesCreated        () const  {return logicalFramesCreated;}
    kkint32 LogicalFramesDropped        () const  {return logicalFramesDropped;}
    kkint32 LogicalFramesSeqNum         () const  {return logicalFramesSeqNum;}
    kkint32 MinSizeThreshold            () const  {return minSizeThreshold;}
    kkint32 PhysicalFramesProcessed     () const  {return physicalFramesProcessed;}

    virtual
    void  CropSettings (kkint32  _cropLeft,
                        kkint32  _cropRight
                       );

    void   BackGroundPixelTH    (uchar _backGroundPixelTH)     {backGroundPixelTH    = _backGroundPixelTH;}
    void   LogicalFramesDropped (kkint32 _logicalFramesDropped)  {logicalFramesDropped = _logicalFramesDropped;}
    void   LogicalFramesSeqNum  (kkint32 _logicalFramesSeqNum)   {logicalFramesSeqNum  = _logicalFramesSeqNum;}
    void   MinSizeThreshold     (kkint32 _minSizeThreshold)      {minSizeThreshold     = _minSizeThreshold;}

    void  GetStats (CounterStats&  stats)  const;


    /**
     * @brief  Returns an estimate of the amount of memory consumed in bytes.
     * @details This will help Managed-Objects keep track of how much memory they are using in the unmanaged world.
     */
    kkMemSize  MemoryConsumedEstimated ();

    virtual  void  ResetCounts ();

    virtual  void  Run ();


  private:
    void  CleanUpMemory ();

    CameraFramePtr  GetNextFrame ();

    void  AddFrameToScanLineArea (CameraFramePtr f);
    void  FindLastScanLineToProcess  ();
    void  MoveOutProcessedScanLines  ();
    void  PlaceRasterDataIntoLogicalFrameList (ScannerFileEntryPtr  scannerFileEntry);
    void  ProcessDroppedFrames ();
    void  SetLastUsedRowAsLastScanLineToProcess ();

    void  LoadLogicalFrame (ScannerFilePtr         sf,
                            LogicalFrameEntryPtr  frameEntry
                           );


    uchar                     backGroundPixelTH;  
    kkint32                   totalWorkArea;             /**< Total number of pixels in work frame area. */
    uchar**                   scanLines;
    uchar*                    scanLinesArea;
    kkint32                   scanLinesAllocated;
    kkint32                   scanLinesSeqNumOfFisrtScanLine;

    kkint32                   cropLeft;
    kkint32                   cropRight;
    bool                      dontDropFrames;            /**< Indicates that we can not drop logical frames; this would occur when we are *
                                                          * performing a play-back and 'playingBackRealTime' is set false.
                                                          */
    kkint64                   frameSeqNumLast;
    kkint32                   lastScanLineToProcess;     /**< Points to a break in ScanLines; the idea is we do not want to split an image. */
    kkint32                   lastScanLineWithNoPixels;
    kkint32                   minSizeThreshold;
    kkint32                   nextRowToUse;              /**< Next row in 'scanLines' to add next frame to.       */
    ushort*                   pixelsOnScanLine;
    float*                    flowRateOnScanLine;
    float*                    flowRateRatioOnScanLine;
    LogicalFrameListPtr       logicalFrames;               /**< Where ROI's that need to be processed are put into. */
    LogicalFrameEntryListPtr  logicalFrameEntries;         /**< All Logical Frames will have an entry in this list. Owned by 'CounterManager' */
    LogicalFrameEntryQueuePtr droppedFrames;               /**< Whatever logical frames have to be dropped will have an entry added to this queue.  Owned by 'CounterManager'    */

    bool                      droppedFramesBeingProcessed; /**< Indicates if Dropped Frames are being processed at this time;
                                                            * This is done after the "ShutdownFlag" flag is true.
                                                            */

    kkint32                   logicalFramesCreated;
    kkint32                   logicalFramesDropped;
    kkint32                   logicalFramesSeqNum;
    kkint32                   frameWidth;
    kkint32                   physicalFramesProcessed;
    bool                      playingBack;               /**< Indicates that we are playing back a previously recoded Scanner file.   */
    bool                      playingBackRealTime;       /**< When playing back simulate the ScanRate of the camera; aids in testing application; that means   *
                                                          * buffers can be overflowed and frames dropped. 
                                                          */

    ScannerFileEntryPtr       curScannerFileEntry;       /**< The scanner file that is currently providing the imaging data that we are
                                                          * processing.  If we are recording then it represents the scanner file that 
                                                          * we are writing to.
                                                          */

    kkint32                   curScannerFileScanLineRow; /** Represents the row from the current Scanner-File(curScannerFileRootName)  *
                                                          * where the current logical frame starts from.
                                                          */

    CameraFrameBufferPtr      cameraBuffer;              /**< Buffer where physical frames come from.  */
  };   /* LogicalFrameBuilder */

  typedef  LogicalFrameBuilder*  LogicalFrameBuilderPtr;
}


#endif
