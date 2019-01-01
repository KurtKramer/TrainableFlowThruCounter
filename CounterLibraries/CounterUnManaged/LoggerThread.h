/* LoggerThread.h -- Represents a single camera frame.
 * Copyright (C) 2011-2013  Kurt Kramer
 * For conditions of distribution and use, see copyright notice in CounterUnManaged.txt
 */
#if  !defined(_LOGGERTHREAD_)
#define  _LOGGERTHREAD_

#include "GoalKeeper.h"
#include "KKQueue.h"
#include "MsgQueue.h"
#include "Raster.h"
#include "RunLog.h"
#include "KKThread.h"

#include "PreClassDefinitions.h"


namespace CounterUnManaged
{
  /**
   *@brief  This thread will manage all RunLog activity for the CounterManager.
   *@details  .
   */
  class LoggerThread: public  KKThread
  {
  public:
    typedef  LoggerThread*  LoggerThreadPtr;

    static
    LoggerThreadPtr  CreateAndStartInstance (MsgQueuePtr  _msgQueue,
                                             MsgQueuePtr  _loggedMsgs,
                                             const KKStr& _threadName
                                            );


    /**
     *@param[in] _msgQueue     Messages in this queue will be written to a log file and the added to '_loggedMsgs'.
     *@param[in] _loggedMsgs   Messages that have been written to the log file will be added to this MsgQueue.
     *@param[in] _threadName
     */
    LoggerThread (MsgQueuePtr  _msgQueue,
                  MsgQueuePtr  _loggedMsgs,
                  const KKStr& _threadName
                 );


    virtual
      ~LoggerThread ();


    /**
     *@brief  Returns an estimate of the amount of memory consumed in bytes.
     *@details This will help managed objects keep track of how much memory 
     * they are using in the unmanaged world.
     */
    kkMemSize  MemoryConsumedEstimated ();


    /** 
     *@brief  Returns all logged messages that are currently in the queue. 
     *@details Caller gets ownership of the list and its contents and is responsible for deleting.
     */
    KKStrListPtr  GetAllLoggedMsgs ();


    /**
     *@brief  Removes from the LoggedMsgs queue the oldest message added to the queue that has not been removed.
     *@details The caller will get ownership of the string and be responsible for deleting it.
     */
    KKStrPtr  GetNextLoggedMsg ();



    /**  @brief  Call this Method to start the Log Output. */
    virtual void  Run ();

  private:
    void  ProcessMsgQueue ();

    ofstream*    logFile;
    KKStr        logFileName;
    MsgQueuePtr  loggedMsgs;  /**< Will contain messages that have already been written to 'logFile' but have not been sent on to owner of 'CounterManager'. */
    bool         weOwnLoggedMsgs;
  };   /* LoggerThread */

  typedef  LoggerThread*  LoggerThreadPtr;
}


#endif
