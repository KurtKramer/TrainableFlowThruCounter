#if  !defined(_LARCOSCOUNTERSTATSMANAGED_)
#define  _LARCOSCOUNTERSTATSMANAGED_

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Collections::Generic;
using namespace System::Diagnostics;
using namespace System::Drawing;


#include "CameraParameters.h"
#include "LarcosCounterStats.h"
using namespace  LarcosCounterUnManaged;

#include  "CameraParametersManaged.h"

namespace LarcosCounterManaged
{
  public ref class LarcosCounterStatsManaged
  {
  public:
    LarcosCounterStatsManaged (LarcosCounterStatsPtr  stats);

    !LarcosCounterStatsManaged ();

    ~LarcosCounterStatsManaged ();

    property  kkint64  BytesWritten                    {kkint64  get ()  {return bytesWritten;};}
    property  kkint64  BytesWrittenAllSegs             {kkint64  get ()  {return bytesWrittenAllSegs;};}
    property  kkint32  CropLeft                        {kkint32  get ()  {return cropLeft;};}
    property  kkint32  CropRight                       {kkint32  get ()  {return cropRight;};}
    property  float    CurAnalogGain                   {float    get ()  {return curAnalogGain;};}
    property  kkint32  CurDigitalGain                  {kkint32  get ()  {return curDigitalGain;};}
    property  String^  CurSensitivity                  {String^  get ()  {return curSensitivity;};}
    property  kkint32  DiskWritingSeqNumBreaks         {kkint32  get ()  {return diskWritingSeqNumBreaks;};}
    property  float    FrameRate                       {float    get ()  {return frameRate;};}
    property  kkint32  CameraTemparature               {kkint32  get ()  {return cameraTemparature;};}
    property  kkint32  LogicalFrameProcessorsAvailable {kkint32  get ()  {return logicalFrameProcessorsAvailable;};}  /**< Number of FrameProcessors that are sleeping until available Frame to process. */
    property  kkint32  LogicalFramesCreated            {kkint32  get ()  {return logicalFramesCreated;};}
    property  kkint32  LogicalFramesDropped            {kkint32  get ()  {return logicalFramesDropped;};}         
    property  kkint32  LogicalFramesOnQueue            {kkint32  get ()  {return logicalFramesOnQueue;};}     
    property  kkint32  LogicalFramesProcessed          {kkint32  get ()  {return logicalFramesProcessed;};}       
    property  kkint32  PhysicalSeqNumsSkipped          {kkint32  get ()  {return physicalSeqNumsSkipped;};}
    property  kkint32  ParticlesCounted                {kkint32  get ()  {return particlesCounted;};}
    property  kkint32  ParticlesExtracted              {kkint32  get ()  {return particlesExtracted;};}
    property  kkint32  ParticlesWaitingProcessing      {kkint32  get ()  {return particlesWaitingProcessing;};}
    property  kkint32  PhysicalFramesDropped           {kkint32  get ()  {return physicalFramesDropped;};}
    property  kkint32  PhysicalFramesProcessed         {kkint32  get ()  {return physicalFramesProcessed;};}
    property  kkint32  PhysicalFramesRead              {kkint32  get ()  {return physicalFramesRead;};}
    property  kkint32  PhysicalFramesWaitingToProcess  {kkint32  get ()  {return physicalFramesWaitingToProcess;};}
    property  kkint32  ScanLinesRead                   {kkint32  get ()  {return scanLinesRead;};}                   /**< Number of scan lines read by camera acquisition thread. */
    property  kkint32  ScanLinesWritten                {kkint32  get ()  {return scanLinesWritten;};}
    property  float    ScanRate                        {float    get ()  {return scanRate;};}
    property  kkint64  TotalBytesToRead                {kkint64  get ()  {return totalBytesToRead;};}
    property  kkint32  TotalLostPackets                {kkint32  get ()  {return totalLostPackets;};}
    property  kkint32  WaitingToWriteToDisk            {kkint32  get ()  {return waitingToWriteToDisk;};}


  private:
    void  CleanUpMemory ();

    kkint64  bytesWritten;
    kkint64  bytesWrittenAllSegs;
    kkint32  cropLeft;
    kkint32  cropRight;
    float    curAnalogGain;
    kkint32  curDigitalGain;
    String^  curSensitivity;
    kkint32  diskWritingSeqNumBreaks;    
    float    frameRate;
    kkint32  cameraTemparature;
    kkint32  logicalFrameProcessorsAvailable; /**< Number of FraeProcessors that are sleeping until available Frame to process. */
    kkint32  logicalFramesCreated;           /**< Number of logical frames created by the FrameBuilder thread. */
    kkint32  logicalFramesDropped;           /**< Number of logical frames that were lost because 'logicalFrames' queue was full. */
    kkint32  logicalFramesOnQueue;           /**< Number of logical frames awaiting processing. */
    kkint32  logicalFramesProcessed;         /**< Number of logical frames that have been processed by ''LogicalFrameProcessor' threads. */
    kkint32  particlesCounted;               /**< Number of particles that have been identified as shrimp in 'LogicalFrameProcessor' threads. */
    kkint32  particlesExtracted;             /**< Total number of particles extracted. */
    kkint32  particlesWaitingProcessing;     /**< Particles that have been extracted in one of the 'LogicalFrameProcessor' but not yet processed. */
    kkint32  physicalFramesDropped;          /**< Number of physical frames dropped because the ran out of room in the buffer. */
    kkint32  physicalFramesProcessed;        /**< Number of physical frames processed by 'LogicalFrameBuilder'. */
    kkint32  physicalFramesRead;             /**< Number of frames read from the camera. */
    kkint32  physicalFramesWaitingToProcess; /**< Number of physical frames that are waiting to be processed by 'LogicalFrameBuilder' thread. */
    kkint32  physicalSeqNumsSkipped;         /**< Each buffer retrieved is assigned a seq number by pleora S/W; this represents the total number that were skipped indicating that frames were lost. */
    kkint32  scanLinesRead;                  /**< Number of scan lines read by camera acquisition thread. */
    kkint32  scanLinesWritten;               /**< Total number of scan lines written to Scanner Files. */
    float    scanRate;
    kkint64  totalBytesToRead;
    kkint32  totalLostPackets;               /**< As reported by 'PvBuffer::GetLostPacketCount' from after 'PvPipeLine::RetrieveNextBuffer' method is called. */
    kkint32  waitingToWriteToDisk;           /**< Number of physical frames retrieved from the camera that have not been written to disk. */
  };  /* LarcosCounterManagerWrapper */
}  /* LarcosCounterManaged */


#endif