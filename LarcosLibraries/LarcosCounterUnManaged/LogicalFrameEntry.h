/* LogicalFrameEntry.h -- 
 * Copyright (C) 2011-2013  Kurt Kramer
 * For conditions of distribution and use, see copyright notice in LarcosCounterUnManaged.txt
 */
#if  !defined(_LOGICALFRAMEENTRY_)
#define  _LOGICALFRAMEENTRY_

#include <queue>

#include "GoalKeeper.h"
#include "KKQueue.h"
#include "RunLog.h"


#include "ScannerFileEntry.h"
using namespace KKLSC;

namespace LarcosCounterUnManaged
{
  /**
   * @brief  Used to track a single LogicalFrame in a ScannerFile; Scan-Line and Byte-Offset to the start of the frame..
   * @details The 'LogicalFrameProcessor' will create a instance for each LogicalFrame dropped and after recording is
   *  stopped will use them re-load the dropped frames and process.
   */
  class  LogicalFrameEntry
  {
  public:
    typedef  LogicalFrameEntry*  LogicalFrameEntryPtr;

    LogicalFrameEntry ();

    LogicalFrameEntry (const LogicalFrameEntry&  _entry);

    LogicalFrameEntry (kkint32              _frameNum,
                       kkint32              _height,
                       ScannerFileEntryPtr  _scannerFile,
                       kkint32              _scanLineNumStart,
                       kkint64              _byteOffset,
                       kkint32              _count,
                       float                _flowRateRatio,
                       float                _flowRate
                      );

    ~LogicalFrameEntry ();

    kkMemSize  MemoryConsumedEstimated ()  const;


    float                FlowRate         () const {return flowRate;}
    float                FlowRateRatio    () const {return flowRateRatio;}
    kkint32              FrameNum         () const {return frameNum;}
    kkint32              Height           () const {return height;}
    ScannerFileEntryPtr  ScannerFile      () const {return scannerFile;}
    kkint32              ScanLineNumStart () const {return scanLineNumStart;}

    void  Assign (const LogicalFrameEntry&   _entry);

    void  Assign (kkint32              _frameNum,
                  kkint32              _height,
                  ScannerFileEntryPtr  _scannerFile,
                  kkint32              _scanLineNumStart,
                  kkint64              _byteOffset,
                  kkint32              _count,
                  float                _flowRateRatio,
                  float                _flowRate
                 );

  private:
    kkint32              frameNum;
    kkint32              height;
    ScannerFileEntryPtr  scannerFile;
    kkint32              scanLineNumStart;   /**< Scan-line in Scanner File that the 1st line is this frame comes from or was written to. */
    kkint64              byteOffset;         /**< Byte offset within Scanner file that this frame starts at; -1 indicates that we do not know. */
    kkint32              count;              /**< Number of shrimp counted in this frame   -1 indicates that no count was made. */
    float                flowRateRatio;
    float                flowRate;
  };

  typedef  LogicalFrameEntry*  LogicalFrameEntryPtr;



  class  LogicalFrameEntryList:  public KKQueue<LogicalFrameEntry>
  {
  public:
    LogicalFrameEntryList (bool _owner);

    LogicalFrameEntryList (const LogicalFrameEntryList&  list);

    ~LogicalFrameEntryList ();

    kkMemSize  MemoryConsumedEstimated ()  const;

  };

  typedef  LogicalFrameEntryList*  LogicalFrameEntryListPtr;



  class  LogicalFrameEntryQueue: protected deque<LogicalFrameEntryPtr>
  {
  public:
    typedef  LogicalFrameEntryQueue*  LogicalFrameEntryQueuePtr;

    LogicalFrameEntryQueue (bool           _owner,
                            const  KKStr&  _queueName
                           );

    ~LogicalFrameEntryQueue ();

    kkMemSize  MemoryConsumedEstimated ()  const;

    void  PushOnFront (LogicalFrameEntryPtr  entry);

    LogicalFrameEntryPtr  PopFromBack ();

    kkint32  QueueSize ()  const;

  private:
    GoalKeeperPtr  goalie;
    bool           owner;
  };  /* LogicalFrameEntryQueue */
  

}  /* LarcosCounterUnManaged */

#endif
