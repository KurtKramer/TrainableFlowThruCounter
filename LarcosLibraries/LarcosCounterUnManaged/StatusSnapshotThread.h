/* StatusSnapshotThread.h -- Takes at fixed intervals a snapshot of recording stats.
 * Copyright (C) 2011-2013  Kurt Kramer
 * For conditions of distribution and use, see copyright notice in LarcosCounterUnManaged.txt
 */
#if  !defined(_STATUSSNAPSHOTTHREAD_)
#define  _STATUSSNAPSHOTTHREAD_

#include "GoalKeeper.h"
#include "KKQueue.h"
#include "MsgQueue.h"
#include "Raster.h"
#include "RunLog.h"

#include "CameraThread.h"
#include "ScannerFile.h"

#include "PreClassDefinitions.h"


namespace LarcosCounterUnManaged
{
  /**
   *@brief  This thread will capture StatusSnapShot's at defined intervals.
   *@details  Will be started whenever we are processing data; whether from the camera or playing-back a
   * previously recorded file.
   */
  class StatusSnapshotThread: public  CameraThread
  {
  public:

    /**
     *@param[in] _manager
     *@param[in] _snapshotBuffer  Buffer where statusSnapshot entries will be saved.
     *@param[in] _snapshotInterval Interval in seconds between grabbing SnapShots of the counters current status.
     *@param[in] _msgQueue
     *@param[in] _threadName
     */
    StatusSnapshotThread (LarcosCounterManagerPtr  _manager,
                          StatusSnapshotBufferPtr  _snapshotBuffer,
                          kkint32                  _snapshotInterval,
                          MsgQueuePtr              _msgQueue,
                          const KKStr&             _threadName
                         );

    virtual
      ~StatusSnapshotThread ();


    /**
     *@brief  Returns an estimate of the amount of memory consumed in bytes.
     *@details This will help managed objects keep track of how much memory 
     * they are using in the unmanaged world.
     */
    kkMemSize  MemoryConsumedEstimated ();

    void  SnapshotInterval (kkint32  _snapshotInterval)  {snapshotInterval = _snapshotInterval;}


    virtual void  ResetCounts ();

    /**  @brief  Call this Method to start the StatusSnapshot  thread. */
    virtual void  Run ();

  private:
    kkint32                   snapshotInterval;  /**< Number of seconds in between snapShots. */
    StatusSnapshotBufferPtr   snapshotBuffer;
    LarcosCounterStats        lastStats;
    LarcosCounterStats        nextStats;
  };   /* StatusSnapshotThread */

  typedef  StatusSnapshotThread*  StatusSnapshotThreadPtr;
}


#endif
