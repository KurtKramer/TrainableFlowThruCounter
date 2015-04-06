/*
 * Copyright (C) 2011-2013  Kurt Kramer
 * For conditions of distribution and use, see copyright notice in LarcosCounterUnManaged.txt
 */


#if  !defined(_LARCOSCOUNTERSTATS_)
#define  _LARCOSCOUNTERSTATS_

#include "KKBaseTypes.h"
#include "KKStr.h"


namespace LarcosCounterUnManaged
{
  /** @brief  Used to communicate current LARCOS status.*/
  class LarcosCounterStats
  {
  public:
    LarcosCounterStats ();

    virtual ~LarcosCounterStats ();

    void  Reset ();

    void  Assign (const LarcosCounterStats& stats);

    kkint64  BytesWritten                      () const {return bytesWritten;}
    kkint64  BytesWrittenAllSegs               () const {return bytesWrittenAllSegs;}
    kkint32  CropLeft                          () const {return cropLeft;}
    kkint32  CropRight                         () const {return cropRight;}
    float  CurAnalogGain                       () const {return curAnalogGain;}
    kkint32  CurDigitalGain                    () const {return curDigitalGain;}
    const KKStr&  CurSensitivity               () const {return curSensitivity;}
    kkint32  DiskWritingSeqNumBreaks           () const {return diskWritingSeqNumBreaks;}
    float  FlowRate                            () const {return flowRate;}
    float  FrameRate                           () const {return frameRate;}
    kkint32  CameraTemparature                 () const {return cameraTemparature;}
    kkint32  LogicalFrameProcessorsAvailable   () const {return logicalFrameProcessorsAvailable;} /**< Number of FrameProcessors that are sleeping until available Frame to process. */
    kkint32  LogicalFramesCreated              () const {return logicalFramesCreated;}            /**< Number of logical frames created by the FrameBuilder thread. */
    kkint32  LogicalFramesDropped              () const {return logicalFramesDropped;}            /**< Number of logical frames that were lost because 'logicalFrames' queue was full. */
    kkint32  LogicalFramesOnQueue              () const {return logicalFramesOnQueue;}            /**< Number of logical frames awaiting processing. */
    kkint32  LogicalFramesProcessed            () const {return logicalFramesProcessed;}          /**< Number of logical frames that have been processed by ''LogicalFrameProcessor' threads. */
    kkint32  ParticlesCounted                  () const {return particlesCounted;}
    kkint32  ParticlesExtracted                () const {return particlesExtracted;}
    kkint32  ParticlesWaitingProcessing        () const {return particlesWaitingProcessing;}
    kkint32  PhysicalFramesDropped             () const {return physicalFramesDropped;}           /**< Number of physical frames dropped because the ran out of room in the buffer. */
    kkint32  PhysicalFramesProcessed           () const {return physicalFramesProcessed;}
    kkint32  PhysicalFramesRead                () const {return physicalFramesRead;}              /**< Number of frames read from the camera. */
    kkint32  PhysicalFramesWaitingToProcess    () const {return physicalFramesWaitingToProcess;}
    kkint32  PhysicalSeqNumsSkipped            () const {return physicalSeqNumsSkipped;}
    kkint32  ScanLinesRead                     () const {return scanLinesRead;}                   /**< Number of scan lines read by camera acquisition thread. */
    kkint32  ScanLinesWritten                  () const {return scanLinesWritten;}
    float    ScanRate                          () const {return scanRate;}
    kkint64  TotalBytesToRead                  () const {return totalBytesToRead;}
    kkint32  TotalLostPackets                  () const {return totalLostPackets;}
    kkint32  WaitingToWriteToDisk              () const {return waitingToWriteToDisk;}            /**< Number of physical frames retrieved from the camera that have not been written to disk. */


    void  BytesWritten                    (kkint64  _bytesWritten)                   {bytesWritten                   = _bytesWritten;}
    void  BytesWrittenAllSegs             (kkint64  _bytesWrittenAllSegs)            {bytesWrittenAllSegs            = _bytesWrittenAllSegs;}
    void  CurAnalogGain                   (float    _curAnalogGain)                  {curAnalogGain                  = _curAnalogGain;}
    void  CurDigitalGain                  (kkint32  _curDigitalGain)                 {curDigitalGain                 = _curDigitalGain;}
    void  CurSensitivity                  (const KKStr&  _curSensitivity)            {curSensitivity                 = _curSensitivity;}
    void  CropLeft                        (kkint32  _cropLeft)                       {cropLeft                       = _cropLeft;}
    void  CropRight                       (kkint32  _cropRight)                      {cropRight                      = _cropRight;}
    void  DiskWritingSeqNumBreaks         (kkint32  _diskWritingSeqNumBreaks)        {diskWritingSeqNumBreaks        = _diskWritingSeqNumBreaks;}
    void  FlowRate                        (float    _flowRate)                       {flowRate                       = _flowRate;}
    void  FrameRate                       (float    _frameRate)                      {frameRate                      = _frameRate;}
    void  CameraTemparature               (kkint32  _cameraTemparature)              {cameraTemparature              = _cameraTemparature;}
    void  LogicalFrameProcessorsAvailable (kkint32  _logicalFrameProcessorsAvailabe) {logicalFrameProcessorsAvailable = _logicalFrameProcessorsAvailabe;}
    void  LogicalFramesCreated            (kkint32  _logicalFramesCreated)           {logicalFramesCreated           = _logicalFramesCreated;}
    void  LogicalFramesDropped            (kkint32  _logicalFramesDropped)           {logicalFramesDropped           = _logicalFramesDropped;}
    void  LogicalFramesOnQueue            (kkint32  _logicalFramesOnQueue)           {logicalFramesOnQueue           = _logicalFramesOnQueue;}
    void  LogicalFramesProcessed          (kkint32  _logicalFramesProcessed)         {logicalFramesProcessed         = _logicalFramesProcessed;}
    void  ParticlesCounted                (kkint32  _particlesCounted)               {particlesCounted               = _particlesCounted;}
    void  ParticlesExtracted              (kkint32  _particlesExtracted)             {particlesExtracted             = _particlesExtracted;}
    void  ParticlesWaitingProcessing      (kkint32  _particlesWaitingProcessing)     {particlesWaitingProcessing     = _particlesWaitingProcessing;}
    void  PhysicalFramesRead              (kkint32  _physicalFramesRead)             {physicalFramesRead             = _physicalFramesRead;}
    void  PhysicalFramesDropped           (kkint32  _physicalFramesDropped)          {physicalFramesDropped          = _physicalFramesDropped;}
    void  PhysicalFramesProcessed         (kkint32  _physicalFramesProcessed)        {physicalFramesProcessed        = _physicalFramesProcessed;}
    void  PhysicalFramesWaitingToProcess  (kkint32  _physicalFramesWaitingToProcess) {physicalFramesWaitingToProcess = _physicalFramesWaitingToProcess;}
    void  PhysicalSeqNumsSkipped          (kkint32  _physicalSeqNumsSkipped)         {physicalSeqNumsSkipped         = _physicalSeqNumsSkipped;}
    void  ScanLinesRead                   (kkint32  _scanLinesRead)                  {scanLinesRead                  = _scanLinesRead;}
    void  ScanLinesWritten                (kkint32  _scanLinesWritten)               {scanLinesWritten               = _scanLinesWritten;}
    void  ScanRate                        (float    _scanRate)                       {scanRate                       = _scanRate;}
    void  TotalBytesToRead                (kkint64  _totalBytesToRead)               {totalBytesToRead               = _totalBytesToRead;}
    void  TotalLostPackets                (kkint32 _totalLostPackets)               {totalLostPackets               = _totalLostPackets;}
    void  WaitingToWriteToDisk            (kkint32  _waitingToWriteToDisk)           {waitingToWriteToDisk           = _waitingToWriteToDisk;}

  private:
    kkint64  bytesWritten;
    kkint64  bytesWrittenAllSegs;
    kkint32  cameraTemparature;
    kkint32  cropLeft;
    kkint32  cropRight;
    float    curAnalogGain;
    kkint32  curDigitalGain;
    KKStr    curSensitivity;
    kkint32  diskWritingSeqNumBreaks;
    float    flowRate;
    float    frameRate;
    kkint32  logicalFrameProcessorsAvailable; /**< Number of FraeProcessors that are sleeping until available Frame to process. */
    kkint32  logicalFramesCreated;            /**< Number of logical frames created by the FrameBuilder thread. */
    kkint32  logicalFramesDropped;            /**< Number of logical frames that were lost because 'logicalFrames' queue was full. */
    kkint32  logicalFramesOnQueue;            /**< Number of logical frames awaiting processing. */
    kkint32  logicalFramesProcessed;          /**< Number of logical frames that have been processed by ''LogicalFrameProcessor' threads. */
    kkint32  particlesCounted;                /**< Number of particles that have been identified as shrimp in 'LogicalFrameProcessor' threads. */
    kkint32  particlesExtracted;              /**< Total number of particles extracted. */
    kkint32  particlesWaitingProcessing;      /**< Particles that have been extracted in one of the 'LogicalFrameProcessor' that is but not yet processed. */
    kkint32  physicalFramesDropped;           /**< Number of physical frames dropped because the ran out of room in the buffer. */
    kkint32  physicalFramesProcessed;         /**< Number of physical frames processed by 'LogicalFrameBuilder'. */
    kkint32  physicalFramesRead;              /**< Number of frames read from the camera.  */
    kkint32  physicalFramesWaitingToProcess;  /**< Number of physical frames that are waiting to be processed by 'LogicalFrameBuilder' thread. */
    kkint32  physicalSeqNumsSkipped;          /**< Each buffer retrieved is assigned a sequence number by Pleora S/W; this represents the total number that were skipped indicating that frames were lost. */
    kkint32  scanLinesRead;                   /**< Number of scan lines read by camera acquisition thread. */
    kkint32  scanLinesWritten;                /**< Total number of scan lines written to Scanner Files. */
    float    scanRate;
    kkint64  totalBytesToRead;
    kkint32  totalLostPackets;                /**< As reported by 'PvBuffer::GetLostPacketCount' from after 'PvPipeLine::RetrieveNextBuffer' method is called. */
    kkint32  waitingToWriteToDisk;            /**< Number of physical frames retrieved from the camera that have not been written to disk. */
  };  /* LarcosCounterStats */

  typedef LarcosCounterStats*  LarcosCounterStatsPtr;

}  /* LarcosCounterUnManaged */



#endif

