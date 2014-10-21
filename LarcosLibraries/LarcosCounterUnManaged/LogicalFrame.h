/* LogicalFrame.h -- Represents a single logical frame.
 * Copyright (C) 2011-2013  Kurt Kramer
 * For conditions of distribution and use, see copyright notice in LarcosCounterUnManaged.txt
 */
#if  !defined(_LOGICALFRAME_)
#define  _LOGICALFRAME_

#include <queue>

#include "GoalKeeperSimple.h"
#include "KKQueue.h"
#include "MorphOpStretcher.h"
#include "Raster.h"
#include "RasterBuffer.h"
#include "RunLog.h"

#include "PreClassDefinitions.h"

namespace LarcosCounterUnManaged
{
  class  LogicalFrame
  {
  public:
    typedef  LogicalFrame*  LogicalFramePtr;

    /** @brief  Constructs an instance of a logical frame. */
    LogicalFrame (kkint32  _frameHeight,
                  kkint32  _frameWidth,
                  RunLog&  _log
                 );

    ~LogicalFrame ();

    void  AssignAFrame (LogicalFrameEntryPtr _logicalFrameEntry,
                        kkint32              _newFrameHeight,
                        kkint32              _newFrameWidth,
                        kkint32              _scanLineNumStart,
                        ScannerFileEntryPtr  _scannerFileEntry,
                        kkint32              _scanLineNumStartCurSF,
                        float                _flowRateRatio,
                        float                _flowRate,
                        kkint64              _frameSeqNum,
                        uchar*               _newFrameArea
                       );

    float        FlowRate                () const  {return flowRate;}
    float        FlowRateRatio           () const  {return flowRateRatio;}
    kkint32      FrameAreaUsed           () const  {return frameAreaUsed;}
    kkint32      FrameHeight             () const  {return frameHeight;}
    kkint32      FrameWidth              () const  {return frameWidth;}
    kkint32      FrameHeightUsed         () const  {return frameHeightUsed;}
    kkint32      MemoryConsumedEstimated () const;
    kkint64      FrameSeqNum             () const  {return frameSeqNum;}
    kkint32      ScanLineNumStart        () const  {return scanLineNumStart;}
    kkint32      ScanLineNumStartCurSF   () const  {return scanLineNumStartCurSF;}

    const KKStr& ScannerFileRootName     () const;

    uchar*       FrameLinesArea          ()        {return frameLinesArea;}
    uchar**      FrameLines              ()        {return frameLines;}


  private:
    void  AllocateMemmory ();
    void  CleanUpMemory();
    
    void  ReAllocateMemory (kkint32  _newFrameHeight,
                            kkint32  _newFrameWidth
                           );


    LogicalFrameEntryPtr   logicalFrameEntry;          /**< Assigned a value in 'LogicalFrame::AssignAFrame'  which is called from 
                                                        * 'LogicalFrameBuilder::PlaceRasterDataIntoLogicalFrameList' which is 
                                                        * called from 'LogicalFrameBuilder::Run'.
                                                        */

    uchar                  backGroundPixelTH;

    kkint32                bytesAllocated;             /**< Total number of pixels in work frame area.                                              */
    
    float                  flowRateRatio;              /**< If no FlowMeter will thne be the one provided by LarcsCounterManager otherwise it will
                                                        * be the one compted by 'FlowMeterTracker'.
                                                        */

    float                  flowRate;                   /**< Meters/Sec as determied by FlowMeter;  if no flow meter present will be computed from 
                                                        * FlowRateFactor(FlowRateRatio).
                                                        */

    kkint32                frameHeight;                /**< Total number of rows(scan lines) allocated for 'ScanLines', 'blobIds', and 'workArea'.  */
    kkint32                frameWidth;
    kkint32                frameHeightUsed;            /**< The number of scan lines that are actually populated with data.                         */
    kkint32                frameAreaUsed;              /**< The number of pixels in 'frameLinesArea' that are actually used.                        */

    uchar*                 frameLinesArea;
    uchar**                frameLines;

    kkint32                scanLineNumStart;           /**< The scan-line that the first row in 'frameLines' represents. */

    kkint32                scanLineNumStartCurSF;      /**< The scan-line that the first row in 'framesLines represents with *
                                                        * respect to the ScannerFile it was written to or read from.
                                                        */

    ScannerFileEntryPtr    scannerFileEntry;           /* Root name of scanner file that this frame is being written to or the
                                                        * frame that it was read from.
                                                        */

    kkint64                frameSeqNum;                /**< sequential number assigned to each frame grabbed from camera. */
  };  /* LogicalFrame */

  typedef  LogicalFrame*  LogicalFramePtr;



  /** 
   *@brief  Container object for holding a list of frames.  
   *@details Will contain a list of logical frames up to a maximum number specified.  When the maximum is
   * reached it will delete(dropped frame) from the list the oldest logical frame when ever a new one is
   * added. 
   */
  class  LogicalFrameList
  {
  public:
    /**
     *@brief  Constructs a new Logical Frame list.
     *@param[in] _maxSize  Maximum number of frames contained in list; if list contains this
     *           many entries when a new entry is added it will delete the oldest entry.
     */
    LogicalFrameList (kkint32 _maxNumFramesAllowed,
                      kkint32 _frameHeight,
                      kkint32 _frameWidth,
                      RunLog& _logger
                     );

    ~LogicalFrameList ();

    LogicalFramePtr  GetAvailableFrame (RunLog&  log);

    void  QueueFrameForProcessing (LogicalFramePtr  _frame);

    void  FrameProcessed (LogicalFramePtr  _frame);

    LogicalFramePtr  GetNextFrameToProcess ();

    kkint32    MemoryConsumedEstimated ()  const;

    kkint32    NumWaitingToBeProcessed ()  const;



  private:
    typedef  std::queue<LogicalFramePtr>   LogicalFrameQueue;
    typedef  std::vector<LogicalFramePtr>  LogicalFrameVector;

    void  AllocateInitialFrames (RunLog& _logger);


    LogicalFrameQueue    availableFrames;
    LogicalFrameQueue    waitingToBeProcessed;
    LogicalFrameVector   allFrames;     /**<  Will own all frames created by this instance of 'LogicalFrameList' */

    kkint32              numFramesAllocated;
    kkint32              maxNumFramesAllowed;

    GoalKeeperSimplePtr  goalKeeper;

    // Variables needed to create a new frame.
    kkint32             frameHeight;
    kkint32             frameWidth;
  };  /* LogicalFrameList */

  typedef  LogicalFrameList* LogicalFrameListPtr;

}  /* LarcosCounterUnManaged */

#endif
