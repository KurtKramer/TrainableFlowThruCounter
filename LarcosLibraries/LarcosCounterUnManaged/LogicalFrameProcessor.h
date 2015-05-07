/* LogicalFrameProcessor.h -- Will process logical frames produced by 'LogicalFrameBuilder'.
 * Copyright (C) 2011-2013  Kurt Kramer
 * For conditions of distribution and use, see copyright notice in LarcosCounterUnManaged.txt
 */
#if  !defined(_LOGICALFRAMEPROCESSOR_)
#define  _LOGICALFRAMEPROCESSOR_

#include "GoalKeeper.h"
#include "KKQueue.h"
#include "RunLog.h"
#include "Raster.h"
#include "GoalKeeper.h"

#include "Classifier2.h"

#include "LogicalFrame.h"
#include "CameraThread.h"

#include "PreClassDefinitions.h"

namespace LarcosCounterUnManaged
{


  /**
   *@brief  Processes logical frames from the "logicalFrames" queue  extracting discrete particles
   * and adding them to the "detectedROIs" queue.
   */
  class  LogicalFrameProcessor:  public  CameraThread
  {
  public:
    typedef  kkint16  BlobIdType;
    typedef  vector<BlobIdType>  BlobIdVectorType;

    typedef  enum  {cmNULL,
                    cmStraightCount,
                    cmPostLarvae,
                    cmClassier
                   }  CountingMethods;
     

    LogicalFrameProcessor (LarcosCounterManagerPtr _manager,
                           const KKStr&            _classifierName,
                           OperatingParametersPtr  _operatingParameters,
                           RasterBufferPtr         _lastParticlesProcessed,
                           LogicalFrameListPtr     _logicalFrames,
                           ParticleEntryBufferPtr  _particleEntryBuffer,
                           const KKStr&            _particlesDirName,
                           kkint32                 _frameHeight,     /**< Max logical frameHeight expected. */
                           kkint32                 _frameWidth,      /**< Length of scan line.              */
                           kkint32                 _cropLeft,
                           kkint32                 _cropRight,
                           MsgQueuePtr             _msgQueue,
                           const KKStr&            _threadName
                          );

    virtual
      ~LogicalFrameProcessor ();

    /**
     *@brief Returns an estimate of the amount of memory consumed in bytes.  
     *@details This will help managed objects keep track of how much memory they are using in the 
     * unmanaged world.
     */
    virtual  kkint32  MemoryConsumedEstimated ()  const;


    // Access Methods
    VolConstBool&   Available           () const {return available;}
    kkint32  MinSizeThreshold           () const {return minSizeThreshold;}
    kkint32  LogicalFramesProcessed     () const {return logicalFramesProcessed;}
    kkint32  ParticlesExtracted         () const {return particlesExtracted;}
    kkint32  ParticlesCounted           () const {return particlesCounted;}      /**< Number of particles of interest that were counted; the number of shrimp.  */
    kkint32  ParticlesWaitingProcessing () const {return particlesWaitingProcessing;}
    kkint32  SizingSamplingInterval     () const {return sizingSamplingInterval;}

    void   BackGroundPixelTH (uchar _backGroundPixelTH)  {backGroundPixelTH = _backGroundPixelTH;}

    virtual
    void   CropSettingsChanged (kkint32  _cropLeft,
                                kkint32  _cropRight
                               );

    void   MinSizeThreshold    (kkint32 _minSizeThreshold)     {minSizeThreshold  = _minSizeThreshold;}

    virtual  void   ResetCounts ();

    void  SaveParticleImages     (bool  _saveParticleImages)      {saveParticleImages     = _saveParticleImages;}
    void  SizingSamplingInterval (kkint32 _sizingSamplingInterval)  {sizingSamplingInterval = _sizingSamplingInterval;}


    /** @brief  This version will count individual connected components. */
    virtual  void   Run ();



    virtual
    void  TerminateFlagChanged ();



  private:
    class  Blob;
    typedef  Blob*  BlobPtr;
    class  BlobList;
    typedef  BlobList*  BlobListPtr;

    void  AllocateMemmory ();
    void  CleanUpMemory();

    void  AddParticleEntry (kkint32     scanRow,
                            kkint32     scanCol,
                            RasterPtr   particle,
                            MLClassPtr  ic,
                            BlobPtr     blob,
                            float       probability,
                            float       breakTie
                           );

    void  LoadClassifer ();

    
    void  ReAllocateMemory (kkint32  _newFrameHeight,
                            kkint32  _newFrameWidth
                           );

    void  ErodeImage (uchar**  srcRaster,
                      uchar*   destRasterArea,
                      uchar**  destRaster,
                      kkint32  maskBias
                     );


    // Connected Component methods.
    BlobIdType  NearestNeighborUpperLeft (kkint32  row,
                                          kkint32  col
                                         );

    BlobIdType  NearestNeighborUpperRight (kkint32  row,
                                           kkint32  col
                                          );

    void   ExtractBlobs (uchar*  rowsArea,
                         uchar** rows
                        );

    RasterPtr  ExtractABlob (uchar*         rowsArea,
                             uchar**        rows,
                             const BlobPtr  blob
                            );

    RasterPtr  ExtractABlob2 (uchar*         rowsArea,
                              uchar**        rows,
                              const BlobPtr  blob,
                              kkint32        divisor
                             );


    LogicalFramePtr  GetNextFrame ();



    void  AnalyseParticleStraightCount (RasterPtr  particle,
                                        kkint32    scanRow,
                                        kkint32    scanCol,
                                        BlobPtr    blob
                                       );

    void  AnalyseParticlePostLarvae (RasterPtr  particle,
                                     kkint32    scanRow,
                                     kkint32    scanCol,
                                     BlobPtr    blob
                                    );

    void  AnalyseParticleUsingClassifier (RasterPtr  particle,
                                          kkint32    scanRow,
                                          kkint32    scanCol,
                                          BlobPtr    blob
                                         );


    void  SaveParticle (RasterPtr   particle,
                        kkint32     countThisParticle,
                        MLClassPtr  mlClass,
                        kkint32     scanRow,
                        kkint32     scanCol
                       );
                                                
    void  ProcessFrame ();

    void  UpdateCounts (kkint32  size,
                        float  numCounted
                       );


    volatile bool           available;                 /**< Indicates that currently not processing a logical frame and is available. */
    uchar                   backGroundPixelTH;
    Classifier2Ptr          classifier;
    KKStr                   classifierName;
    kkint32                 connectedComponentDist;
    CountingMethods         countingMethod;
    bool                    countParticles;
    kkint32                 cropLeft;
    kkint32                 cropRight;
    kkint32                 erosionStructSize;
    ParticleEntryListPtr    extractedParticles;        /**< List of particles extracted while processing a frame. */
    float                   flowRate;                  /**< From LogicalFrame::FlowRate      which is from CameraFrame::FlowRate.      */
    float                   flowRateRatio;             /**< From LogicalFrame::FlowRateRatio which is from CameraFrame::FlowRateRatio. */
    RasterBufferPtr         lastParticlesProcessed;
    ShrimpLengthComputerPtr lenComputer;
    LogicalFrameListPtr     logicalFrames;
    kkint32                 logicalFramesProcessed;    /**< Number of logical frames processed by this instance. */
    kkint32                 minSizeThreshold;
    ParticleEntryBufferPtr  particleEntryBuffer;
    kkuint32                particleEntryProcCount;
    kkint32                 particlesExtracted;
    kkint32                 particlesCounted;
    KKStr                   particlesDirName;          /**<  Sub-directory where images that get filtered out need to be written to when SaveImages is true. */
    kkint32                 particlesWaitingProcessing;
    bool                    playingBack;               /**< Indicates that we are playing back a previously recorded file.                                   */
    bool                    playingBackRealTime;       /**< When playing back simulate the ScanRate of the camera; aids in testing application; that means   *
                                                        * buffers can be overflowed and frames dropped. */
    bool                    saveParticleImages;        /**<  Indicates that individual particle images are to be saved.  Images are typically saved in
                                                        *    Images that are not added to the 'detectedROIs' will be written to 'particlesDirName' by this object.
                                                        */
    KKStr                   scannerFileRootName;       /**< Name of scanner file where the Logical frame comes from or is being written to. */
    kkint32                 sizingSamplingInterval;

    MorphOpStretcherPtr     stretcher;        /**< Used to adjust images by flowRate;  when there is a FlowMeter flowRateRatio will
                                               * vary; but we do not want to create a new "MorphOpStretcher" instance unless the change
                                               * is significant.  The logic is that computing the adjustment factors is expensive
                                               * and do not want to for every single example.  A compromise is to only build a
                                               * new "MorphOpStretcher" instance when the FlowRateRatio changes by more than 5%.
                                               */

    set<KKStr>              subDirsCreated;
    TrainingProcess2Ptr     trainer;
    MLClassPtr              unknownClass;

    // Structures needed for processing a single frame.
    BlobListPtr   blobs;
    BlobIdType**  blobIds;
    BlobIdType*   blobIdsArea;
    BlobIdType*   blobIdsMinusOnes;           /**<  Used to help quickly initialize one row of 'blobIds'  data array one row at a time.    */
    kkint32       bytesAllocated;             /**< Total number of pixels in work frame area.                                              */
    kkint32       frameAreaUsed;              /**< frameHeightUsed * frameWidth */
    kkint32       frameHeight;                /**< Total number of rows(scan lines) allocated for 'ScanLines', 'blobIds', and 'workArea'.  */
    kkint32       frameWidth;
    kkint32       frameHeightUsed;

    uchar**       frameLines;                 /**< Will point to 'frameLines'     in the 'LogicalFrame' instance currently being proceed.  */
    uchar*        frameLinesArea;             /**< Will point to 'frameLinesArea' in the 'LogicalFrame' instance currently being proceed.  */
    kkuint32      frameStartScanLineNum;      /**< The scan line that the logical frame starts at.                                         */

    kkint32       logFrameScannerFileScanRow; /**< Scan Row from 'scannerFileRootName' that the logical frame starts at.           */
    uchar*        workLinesArea;
    uchar**       workLines;


    static  volatile
    GoalKeeperPtr  classifierBuildBlocker;

  };   /* LogicalFrameProcessor */

  typedef  LogicalFrameProcessor*  LogicalFrameProcessorPtr;




  class  LogicalFrameProcessorList:  public KKQueue<LogicalFrameProcessor>  
  {
  public:
    LogicalFrameProcessorList (bool _owner = true);

    ~LogicalFrameProcessorList ();


    kkint32  GetNumLogicalFrameProcessorsAvailable ();

    void  GetStats (LarcosCounterStats&  larcosCameraStats)  const;

    void  ShutdownThreads ();

    void  TerminateThreads ();

    bool  ThreadsStillRunning ()  const;

    void  WaitForThreadsToStop (kkint32  secsToWait);
  };

  typedef  LogicalFrameProcessorList*  LogicalFrameProcessorListPtr;
}


#endif
