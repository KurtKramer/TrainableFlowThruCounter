/* ReportWriterThread.h -- Tread that generates a report that is later used by the Audit function.
 * Copyright (C) 2011-2013  Kurt Kramer
 * For conditions of distribution and use, see copyright notice in LarcosCounterUnManaged.txt
 */
#if  !defined(_REPORTWRITERTHREAD_)
#define  _REPORTWRITERTHREAD_

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
   *@brief  This will manage report writing.
   *@details  This classes primary responsibility is to write particle extraction data to a report
   *  that will later be used by the ground-truth application.
   */
  class ReportWriterThread: public  CameraThread
  {
  public:

    /**
     *@param[in] _manager
     *@param[in] _particleEntryBuffer
     *@param[in] _initialScannerFileName The initial scanner file; there may be multiple files where 
     *           the following only differ by an additional seq number.
     *@param[in] _msgQueue
     *@param[in] _threadName
     */
    ReportWriterThread (LarcosCounterManagerPtr      _manager,
                        ParticleEntryBufferPtr       _particleEntryBuffer,
                        const KKStr&                 _initialScannerFileName,
                        const SessionParametersPtr   _sessionParameters,
                        const OperatingParametersPtr _operatingParameters,
                        MsgQueuePtr                  _msgQueue,
                        const KKStr&                 _threadName
                       );

    virtual
      ~ReportWriterThread ();


    /**
     *@brief  Returns an estimate of the amount of memory consumed in bytes.
     *@details This will help managed objects keep track of how much memory 
     * they are using in the unmanaged world.
     */
    kkint32  MemoryConsumedEstimated ();


    virtual void  GetStats (LarcosCounterStats&  stats)  const;


    void  AddDataLine (const KKStr& lineName,
                       const KKStr& lineValue
                      );


    virtual void  ResetCounts ();

    /**
     *@brief  Call this Method to start writing camera frame data to disk.  Will stay in loop until cancelFlag is true.
     */
    virtual void  Run ();

    void  WriteHeader ();


  private:
    DateTime                      dateTimeCompleted;
    DateTime                      dateTimeStarted;
    KKStr                         initialScannerFileName;
    const OperatingParametersPtr  operatingParameters;
    ParticleEntryBufferPtr        particleEntryBuffer;
    bool                          playingBack;
    ofstream*                     report;
    KKStr                         reportFileName;
    const SessionParametersPtr    sessionParameters;
    double                        systemTimeUsedCompleted;
    double                        systemTimeUsedStarted;
  };   /* ReportWriterThread */

  typedef  ReportWriterThread*  ReportWriterThreadPtr;
}


#endif
