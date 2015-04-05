/* ConnectButtonThread.h -- Represents a single camera frame.
 * Copyright (C) 2011-2013  Kurt Kramer
 * For conditions of distribution and use, see copyright notice in LarcosCounterUnManaged.txt
 */
#if  !defined(_CONNECTBUTTONTHREAD_)
#define  _CONNECTBUTTONTHREAD_

#include "GoalKeeper.h"
#include "RunLog.h"

#include "CameraThread.h"


namespace LarcosCounterUnManaged
{
  /**
   *@brief  manage thread that stops the counting and recording of data; it performs the actions
   *  required when the 'StopButton' has been pressed by the user.
   */
  class ConnectButtonThread: public  CameraThread
  {
  public:
    ConnectButtonThread (LarcosCounterManagerPtr  _manager,
                         MsgQueuePtr              _msgQueue,
                         const KKStr&             _threadName
                        );


    ~ConnectButtonThread ();
 

    /**
     *@brief  Returns an estimate of the amount of memory consumed in bytes.
     *@details This will help managed objects keep track of how much memory 
     * they are using in the unmanaged world.
     */
    kkint32  MemoryConsumedEstimated ();
 
    
    virtual  void  ResetCounts ();


    /**
     *@brief  Call this Method to start writing camera frame data to disk; will stay in loop until cancelFlag is true.
     */
    virtual void  Run ();


  private:
  };   /* ConnectButtonThread */

  typedef  ConnectButtonThread*  ConnectButtonThreadPtr;
}


#endif
