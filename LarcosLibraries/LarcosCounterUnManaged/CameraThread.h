/* CameraThread.h -- Base class fir all other LarcosManager threads.
 * Copyright (C) 2011-2013  Kurt Kramer
 * For conditions of distribution and use, see copyright notice in LarcosCounterUnManaged.txt
 */
#if  !defined(_CAMERAPATHREAD_)
#define  _CAMERAPATHREAD_

#include "GoalKeeper.h"
#include "KKQueue.h"
#include "MsgQueue.h"
#include "RunLog.h"
#include "KKThread.h"



namespace  LarcosCounterUnManaged
{
  #if  !defined(_LARCOSCOUNTERMANAGER_)
  class  LarcosCounterManager;
  typedef  LarcosCounterManager*  LarcosCounterManagerPtr;
  #endif


  /**
   *@brief  Base class for all Thread specific classes used by the LarcosCounter.
   *@details  This is a abstract class that is a specialization of the general purpose 
   * class "KKTHread".  It contains data fields  and methods specific to the Larcos Counter.  
   * Each individual thread  in the LarcosCounter will be managed by a instance of a class
   * that was derived from this class (CameraAcquisition, LogicalFrameBuilder, LogicalPrameProcessor,
   * ).
   */
  class CameraThread: public  KKThread
  {
  public:
    CameraThread (LarcosCounterManagerPtr _manager,
                  const KKStr&            _threadName,
                  MsgQueuePtr             _msgQueue
                 );

    virtual
    ~CameraThread ();

    //typedef  KKThread::ThreadStatus  ThreadStatus;


    LarcosCounterManagerPtr  Manager () {return   manager;}


    const LarcosCounterManagerPtr  ManagerConst () const {return  (const LarcosCounterManagerPtr)manager;}


    /**  Returns an estimate of the amount of memory consumed in bytes.  This will 
      * help managed objects keep track of how much memory they are using in the 
      * unmanaged world.
      */
    kkMemSize  MemoryConsumedEstimated ()  const;


    virtual  void  Run ();

    virtual
    void  ResetCounts () = 0;


    /**
     *@brief  The LarcosCameraManager will call this method for all 'CameraThread' based instances 
     * one of the Crop Settings Change.
     */
    virtual
    void  CropSettingsChanged (kkint32  _cropLeft,
                               kkint32  _cropRight
                              );

    /**
     *@brief  The LarcosCameraManager will call this method for all 'CameraThread' based instances whenever the 
     * acquisition thread detects a change in the scan-rate.
     *@details If a particular sub-classed object needs to know when the scan-rate changes it will overload
     * this method.
     */
    virtual
    void   ScanRateChanged (float _newScanRate);

    /**
     *@brief  A 'KKTread' event method.  Will be called whenever the status of 'terminateFlag' has changed.
     */
    virtual
    void  TerminateFlagChanged ();


  protected:
    RunLog  log;

  private:
    LarcosCounterManagerPtr   manager;
  };   /* CameraThread */

  typedef  CameraThread*  CameraThreadPtr;




  class  CameraThreadList:  public  KKQueue<CameraThread>
  {
  public:
    CameraThreadList (bool _owner = true);
    //CameraThreadList (const CameraThreadList&  list);
    ~CameraThreadList ();

    kkMemSize  MemoryConsumedEstimated ()  const;

    void  DeleteAllStoppedThreads ();

    void  SendTerminateCmdToAllThreads ();

    void  SendShutdownCmdToAllThreads ();

    void  TerminateAllButSpecifiedThread (CameraThreadPtr specifiedThread);

    void  WaitForAllThreadsToStop (float  maxWaitTime,
                                   bool&  allThradsStopped
                                  );
  };

  typedef  CameraThreadList*  CameraThreadListPtr;

}  /* LarcosCounterUnManaged */


#endif
