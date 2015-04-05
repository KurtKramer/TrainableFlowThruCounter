/* PlayBackButtonThread.h -- Controls a single thread that will process a PlayBackButton request.
 * Copyright (C) 2011-2013  Kurt Kramer
 * For conditions of distribution and use, see copyright notice in LarcosCounterUnManaged.txt
 */
#if  !defined(_PLAYBACKBUTTONTHREAD_)
#define  _PLAYBACKBUTTONTHREAD_

#include  "GoalKeeper.h"
#include  "RunLog.h"

#include  "ScannerFile.h"
using namespace  KKLSC;

#include  "CameraThread.h"

namespace LarcosCounterUnManaged
{
  /**
   *@brief  A thread that will create and start the threads necessary for playing back a existing scanner file.
   */
  class PlayBackButtonThread: public  CameraThread
  {
  public:
    typedef  ScannerFile::ScannerFileFormat  ScannerFileFormat;

    PlayBackButtonThread (LarcosCounterManagerPtr _manager,
                          const KKStr&            _srcScannerFileName,
                          MsgQueuePtr             _msgQueue,
                          const KKStr&            _threadName
                         );

    virtual
      ~PlayBackButtonThread ();
 
    const KKStr&  ErrMsg     ()  const  {return errMsg;}
    bool          Successful ()  const  {return successful;}

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
    KKStr  srcScannerFileName;
    bool   successful;
    KKStr  errMsg;
  };   /* PlayBackButtonThread */

  typedef  PlayBackButtonThread*  PlayBackButtonThreadPtr;
}


#endif
