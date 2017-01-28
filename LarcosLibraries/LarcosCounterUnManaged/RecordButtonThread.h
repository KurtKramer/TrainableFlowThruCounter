/* RecordButtonThread.h --
 * Copyright (C) 2011-2013  Kurt Kramer
 * For conditions of distribution and use, see copyright notice in LarcosCounterUnManaged.txt
 */
#if  !defined(_RECORDBUTTONTHREAD_)
#define  _RECORDBUTTONTHREAD_

#include "GoalKeeper.h"
#include "RunLog.h"

#include "ScannerFile.h"
using namespace  KKLSC;

#include "CameraThread.h"


namespace LarcosCounterUnManaged
{
  /**
   *@brief  manage thread that stops the counting and recording of data;  it performs the actions
   *  required when the 'StopButton' has been pressed by the user.
   */
  class RecordButtonThread: public  CameraThread
  {
  public:
    typedef  ScannerFile::Format   ScannerFileFormat;


    RecordButtonThread (LarcosCounterManagerPtr _manager,
                        MsgQueuePtr             _msgQueue,
                        const KKStr&            _threadName
                       );

    virtual
      ~RecordButtonThread ();
 
    const KKStr&  ErrMsg     ()  const  {return errMsg;}
    bool          Successful ()  const  {return successful;}

    /**
     *@brief  Returns an estimate of the amount of memory consumed in bytes.
     *@details This will help managed objects keep track of how much memory 
     * they are using in the unmanaged world.
     */
    kkMemSize  MemoryConsumedEstimated ();
 
    
    virtual  void  ResetCounts ();


    /**
     *@brief  Call this Method to start writing camera frame data to disk.  Will stay in loop until cancelFlag is true.
     */
    virtual void  Run ();


  private:
    bool   successful;
    KKStr  errMsg;
  };   /* RecordButtonThread */

  typedef  RecordButtonThread*  RecordButtonThreadPtr;
}


#endif

