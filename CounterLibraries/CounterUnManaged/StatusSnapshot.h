/* StatusSnapshot.h -- Represents a single snapshot.
 * Copyright (C) 2011-2013  Kurt Kramer
 * For conditions of distribution and use, see copyright notice in CounterUnManaged.txt
 */
#if  !defined(_STATUSSNAPSHOT_)
#define  _STATUSSNAPSHOT_

#include <map>

#include "DateTime.h"
#include "GoalKeeper.h"
#include "KKQueue.h"
#include "MorphOpStretcher.h"
#include "Raster.h"
#include "RasterBuffer.h"
#include "RunLog.h"

#include "MLClass.h"

using namespace KKMLL;

#include "PreClassDefinitions.h"


namespace CounterUnManaged
{


  /**
   *@brief  Stores a snapshot of CounterApplication dynamic data such as Flow-rate, Shrimp/Min, Particles/Min Logical-Frame-Processors In use, etc.
   *@details  CounterManager will maintain a historical status of each session.  This will be done by taking a snapshot
   * of current status at specified intervals.<p>
   * Note that all accumulator variables (count, particles, logicalFramesOnQueue, ... etc) are for a single time interval!
   */
  class  StatusSnapshot
  {
  public:
    typedef  StatusSnapshot*  StatusSnapshotPtr;


    enum  class FieldIdx: int 
                   {Null,
                    LogicalFrameProcessorsAvailable,
                    LogicalFramesOnQueue,
                    Count,
                    Particles,
                    CpuUsage,
                    AvailableCapacity,
                    PhysicalFramesDropped,
                    LogicalFramesDropped,
                    ScanLinesRead,
                    ScanLinesWritten,
                    ParticlesWaitingProcessing,
                    FlowRate,
                    Invalid
                   };

    static  const KKStr&  SnapShotFieldIdxToStr   (FieldIdx i);
    static  FieldIdx  SnapShotFieldIdxFromStr (const KKStr&  i);


    StatusSnapshot ();

    StatusSnapshot (const StatusSnapshot&  _entry);

    StatusSnapshot (KKB::DateTime _timeStamp,
                    kkint32       _timeOffset,
                    kkint16       _logicalFrameProcessorsAvailable,
                    kkint16       _logicalFramesOnQueue,
                    kkint32       _count,
                    kkint32       _particles,
                    float         _cpuUsage,
                    float         _availableCapacity,
                    kkint32       _cameraTemparature,
                    kkint32       _physicalFramesDropped,
                    kkint32       _logicalFramesDropped,
                    kkint32       _scanLinesRead,
                    kkint32       _scanLinesWritten,
                    kkint32       _particlesWaitingProcessing,
                    float         _flowRate
                   );


    ~StatusSnapshot ();

    kkMemSize  MemoryConsumedEstimated ()  const;

    void  Assign (const StatusSnapshot&   _entry);

    /**
     *@brief  Will assign the change(difference) between two successive 'CounterStats'. 
     *@details This is meant to look for what has changed in the counters status since the last 
     * call to CounterManager::GetStats. For example the call to "GetStats" will return the total
     * number of dropped frames; but what we are interested in is whether frames are still being dropped.
     */
    void  Assign (const CounterStats&  _lastEntry,
                  const CounterStats&  _nextEntry
                 );


    const KKB::DateTime&  TimeStamp           ()  const  {return  timeStamp;}
    kkint32   TimeOffset                      ()  const  {return  timeOffset;}
    float     LogicalFrameProcessorsAvailable ()  const  {return  logicalFrameProcessorsAvailable;}
    float     LogicalFramesOnQueue            ()  const  {return  logicalFramesOnQueue;}
    kkint32   Count                           ()  const  {return  count;}
    kkint32   Particles                       ()  const  {return  particles;}
    float     CpuUsage                        ()  const  {return  cpuUsage;}
    float     AvailableCapacity               ()  const  {return  availableCapacity;}
    kkint32   CameraTemparature               ()  const  {return  cameraTemparature;}
    kkint32   PhysicalFramesDropped           ()  const  {return  physicalFramesDropped;}
    kkint32   LogicalFramesDropped            ()  const  {return  logicalFramesDropped;}
    kkint32   ScanLinesRead                   ()  const  {return  scanLinesRead;}
    kkint32   ScanLinesWritten                ()  const  {return  scanLinesWritten;}
    kkint32   ParticlesWaitingProcessing      ()  const  {return  particlesWaitingProcessing;}
    float     FlowRate                        ()  const  {return  flowRate;}

    void      TimeStamp                       (KKB::DateTime _timeStamp)                        {timeStamp                       = _timeStamp;}
    void      TimeOffset                      (kkint32       _timeOffset)                       {timeOffset                      = _timeOffset;}
    void      LogicalFrameProcessorsAvailable (float         _logicalFrameProcessorsAvailable)  {logicalFrameProcessorsAvailable = _logicalFrameProcessorsAvailable;}
    void      LogicalFramesOnQueue            (float         _logicalFramesOnQueue)             {logicalFramesOnQueue            = _logicalFramesOnQueue;}
    void      Count                           (kkint32       _count)                            {count                           = _count;}
    void      Particles                       (kkint32       _particles)                        {particles                       = _particles;}
    void      CpuUsage                        (float         _cpuUsage)                         {cpuUsage                        = _cpuUsage;}
    void      AvailableCapacity               (float         _availableCapacity)                {availableCapacity               = _availableCapacity;}
    void      CameraTemparature               (kkint32       _cameraTemparature)                {cameraTemparature               = _cameraTemparature;}
    void      PhysicalFramesDropped           (kkint32       _physicalFramesDropped)            {physicalFramesDropped           = _physicalFramesDropped;}      
    void      LogicalFramesDropped            (kkint32       _logicalFramesDropped)             {logicalFramesDropped            = _logicalFramesDropped;}
    void      ScanLinesRead                   (kkint32       _scanLinesRead)                    {scanLinesRead                   = _scanLinesRead;}
    void      ScanLinesWritten                (kkint32       _scanLinesWritten)                 {scanLinesWritten                = _scanLinesWritten;}
    void      ParticlesWaitingProcessing      (kkint32       _particlesWaitingProcessing)       {particlesWaitingProcessing      = _particlesWaitingProcessing;}
    void      FlowRate                        (float         _flowRate)                         {flowRate                        = _flowRate;}

    void  UpdateTimeStamp ();


  private:
    static  const KKStr  dataFieldIdxStrs[];

    KKB::DateTime  timeStamp;                         /**< Time that snapshot was taken.                */
    kkint32        timeOffset;                        /**< Offset in seconds from start of collection.  */
    float          logicalFrameProcessorsAvailable;
    float          logicalFramesOnQueue;
    kkint32        count;                             /**< Shrimp counted this interval.          */
    kkint32        particles;                         /**< Particles counted       this interval. */
    float          cpuUsage;
    float          availableCapacity;
    kkint32        cameraTemparature;
    kkint32        physicalFramesDropped;             /**< Physical Frames dropped this interval. */
    kkint32        logicalFramesDropped;              /**< Logical Frames dropped  this interval. */
    kkint32        scanLinesRead;                     /**< Scan-Lines read         this interval. */
    kkint32        scanLinesWritten;                  /**< Scan Lines written      this interval. */
    kkint32        particlesWaitingProcessing;        /**< Number of particles identified by FrameProcessors but have not yet been processed. */
    float          flowRate;
  };  /* StatusSnapshot */

  typedef  StatusSnapshot*  StatusSnapshotPtr;



  class  StatusSnapshotList:  public  KKQueue<StatusSnapshot>
  {
  public:
    StatusSnapshotList (bool _owner);
    ~StatusSnapshotList ();

    StatusSnapshotList (const StatusSnapshotList&  list);
  };  /* StatusSnapshotList */


  typedef  StatusSnapshotList*  StatusSnapshotListPtr;



  /**
   *@brief  Used by "CounterManager" and  "StatusSnapshotThread"  to keep track of snapshots of Counter Status at defined intervals of time.
   *@details The "StatusSnapshotThread" thread will awake at intervals and take a snapshot of Counter activity and add to end of this buffer.
   * These snapshots will reflect the change in same variables such as dropped frames.
   */
  class  StatusSnapshotBuffer:  public  std::vector<StatusSnapshotPtr>
  {
  public:
    typedef  StatusSnapshotBuffer*  StatusSnapshotBufferPtr;

    typedef  std::pair<KKB::DateTime, StatusSnapshotPtr>  KeyRecPair;

    StatusSnapshotBuffer ();

    StatusSnapshotBuffer (const StatusSnapshotList&  list);

    ~StatusSnapshotBuffer ();

    void  AddEntry (StatusSnapshotPtr  snapShot);

    const StatusSnapshotPtr  LastEntry ();

	  /**@brief  Empties the buffer; you would call this before you start a recording/ counting  session.  */
	  void  Reset ();

    StatusSnapshotListPtr  Retrieve (KKB::DateTime  rangeStart,
                                     KKB::DateTime  rangeEnd
                                    );
  private:
    void  DeleteContents ();

    GoalKeeperPtr  goalie;
  };  /* StatusSnapshotBuffer */

  typedef  StatusSnapshotBuffer::StatusSnapshotBufferPtr  StatusSnapshotBufferPtr;


}  /* CounterUnManaged */

#endif
