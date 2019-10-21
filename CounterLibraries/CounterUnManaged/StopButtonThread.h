/* StopButtonThread.h -- Stops the processing of a recoding or playback session.
 * Copyright (C) 2011-2013  Kurt Kramer
 * For conditions of distribution and use, see copyright notice in CounterUnManaged.txt
 */
#if  !defined(_STOPBUTTONTHREAD_)
#define  _STOPBUTTONTHREAD_

#include  "GoalKeeper.h"
#include  "RunLog.h"

#include  "CameraThread.h"


namespace CounterUnManaged
{
  /**
   *@brief  manage thread that stops the counting and recording of data;  it performs the actions
   *  required when the 'StopButton' has been pressed by the user.
   */
  class StopButtonThread: public  CameraThread
  {
  public:
    StopButtonThread (CounterManagerPtr  _manager,
                      bool                     _stopImmediately,
                      MsgQueuePtr              _msgQueue,
                      const KKStr&             _threadName
                     );


    virtual
      ~StopButtonThread ();
 

    /**
     *@brief  Returns an estimate of the amount of memory consumed in bytes.
     *@details This will help managed objects keep track of how much memory 
     * they are using in the unmanaged world.
     */
    kkMemSize  MemoryConsumedEstimated ();
 
    
    virtual  void  ResetCounts ();


    /**
     *@brief  Call this Method to start writing camera frame data to disk; will stay in loop until cancelFlag is true.
     */
    virtual void  Run ();


  private:
    bool  stopImmediately;
  };   /* StopButtonThread */

  typedef  StopButtonThread*  StopButtonThreadPtr;
}


#endif
