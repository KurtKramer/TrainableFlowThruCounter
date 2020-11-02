/* CameraThread.h -- Base class for all other CounterManager threads.
 * Copyright (C) 2011-2019  Kurt Kramer
 * For conditions of distribution and use, see copyright notice in CounterUnManaged.txt
 */
#if  !defined(_CAMERAPATHREAD_)
#define  _CAMERAPATHREAD_

#include "GoalKeeper.h"
#include "KKQueue.h"
#include "MsgQueue.h"
#include "RunLog.h"
#include "KKThread.h"



namespace  CounterUnManaged
{
  #if  !defined(_COUNTERMANAGER_)
  class  CounterManager;
  typedef  CounterManager*  CounterManagerPtr;
  typedef  const CounterManager CounterManagerConst;
  typedef  CounterManagerConst* CounterManagerConstPtr;
  #endif


  /**
   *@brief  Base class for all Thread specific classes used by the CounterApplication.
   *@details  This is a abstract class that is a specialization of the general purpose 
   * class "KKTHread".  It contains data fields  and methods specific to the Trainable Flow-Thru Counter.  
   * Each individual thread  in the CounterApplication will be managed by a instance of a class
   * that was derived from this class (CameraAcquisition, LogicalFrameBuilder, LogicalPrameProcessor,
   * ).
   */
  class CameraThread: public  KKThread
  {
  public:
    CameraThread (CounterManagerPtr _manager,
                  const KKStr&      _threadName,
                  MsgQueuePtr       _msgQueue
                 );

    virtual
    ~CameraThread ();

    
    CounterManagerPtr  Manager () {return   manager;}


    CounterManagerConstPtr  ManagerConst () const {return  (CounterManagerConstPtr)manager;}


    /**  Returns an estimate of the amount of memory consumed in bytes.  This will 
      * help managed objects keep track of how much memory they are using in the 
      * unmanaged world.
      */
    kkMemSize  MemoryConsumedEstimated ()  const;


    virtual  void  Run ();

    virtual
    void  ResetCounts () = 0;


    /**
     *@brief  The CounterManager will call this method for all 'CameraThread' based instances when one of the Crop Settings Change.
     */
    virtual
    void  CropSettingsChanged (kkint32  _cropLeft,
                               kkint32  _cropRight
                              );

    /**
     *@brief  The CounterManager will call this method for all 'CameraThread' based instances whenever the 
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
    CounterManagerPtr   manager;
  };   /* CameraThread */

  typedef  CameraThread*  CameraThreadPtr;




  class  CameraThreadList:  public  KKQueue<CameraThread>
  {
  public:
    CameraThreadList (bool _owner = true);
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

}  /* CounterUnManaged */


#endif
