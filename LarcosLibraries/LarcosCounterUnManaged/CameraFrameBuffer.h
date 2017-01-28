/* CameraFrameBuffer.h -- Implements Camera Frame Buffering allowing a camera thread to unload frames quickly for a separate thread to process.
 * Copyright (C) 2011-2013  Kurt Kramer
 * For conditions of distribution and use, see copyright notice in LarcosCounterUnManaged.txt
 */
#if  !defined(_CAMERAFRAMEBUFFER_)
#define  _CAMERAFRAMEBUFFER_

#include  "GoalKeeperSimple.h"
#include  "Raster.h"

#if  !defined(_FLOWMETERTRACKER_)
namespace  KKLSC
{
  class  FlowMeterTracker;
  typedef  FlowMeterTracker*  FlowMeterTrackerPtr;
}
#endif


namespace LarcosCounterUnManaged
{
  #if  !defined(_LARCOSCOUNTERMANAGER_)
  class  LarcosCounterManager;
  typedef  LarcosCounterManager*  LarcosCounterManagerPtr;
  #endif

  #if  !defined(_CAMERAFRAME_)
  class  CameraFrame;
  typedef  CameraFrame*  CameraFramePtr;

  class  CameraFrameList;
  typedef  CameraFrameList* CameraFrameListPtr;
  #endif

  #if  !defined(_CAMERAFLATFIELDCORRECTION_)
  class  CameraFlatFieldCorrection;
  typedef  CameraFlatFieldCorrection*  CameraFlatFieldCorrectionPtr;
  #endif

  #if  !defined(_LARCOSCOUNTERSTATS_)
    class  LarcosCounterStats;
    typedef  LarcosCounterStats*  LarcosCounterStatsPtr;
  #endif

  /**
   *@class  CameraFrameBuffer
   *@brief  Will manage buffer that will allow two separate threads, one reading form the camera 
   * and the other processing the frames. 
   *@details  You will need one instance of this class for each Camera that you want to buffer data for.
   * It is meant to be used in a multi-threaded environment, specifically three threads, but more or
   * less can always access it.
   * 1) A thread that will acquire data from the camera and insert it into this buffer one frame 
   *    at a time.  It will use the 'AddFrame' method to do this.
   * 2) A thread that will process these frames by calling 'GetNextFrameToProcess' to get the next 
   *    frame in the stream and 'DoneProcessingFrame' when it is done processing the frame.
   * 3) A thread that will write the camera data to disk.  It will call 'GetNextFrameToWriteToDisk'
   *    to get the next frame and then call 'DoneWrittingFrameToDisk'  after it is done writing
   *    data to disk.
   *  
   * A 'GoalKeepeer' object 'gateKeeper' will be used to enforce integrity in the Multi-Threaded
   * environment.  It will guarantee that only one thread at a time can access the four queues
   * listed below.
   *
   * There are 4 CameraFrameList instances that will keep the frames organized.  
   *  buffer               - Maintains a list of all CameraFrames that have been allocated.  It will own 
   *                         these frames.  
   *  available            - This is the list of frames that are empty and available for new data from 
   *                         the camera.  The 'AddFrame' method will remove frame from this and populate
   *                         with camera data.
   *  waitingToProcess     - This is the list of frames that still need to be processed.  Entries are 
   *                         put in by the 'AddFrame' method and removed by the 'GetNextFrameToProcess'
   *                         'GetNextFrameToProcess'.
   *  waitingToWriteToDisk - The list of frames that need to be written to disk.  'AddFrame' will add
   *                         frames to the end of this list while 'GetNextFrameToProcess' will remove 
   *                         from the front.
   * 
   */
  class  CameraFrameBuffer 
  {
  public:
    typedef  CameraFrameBuffer*  CameraFrameBufferPtr;

    CameraFrameBuffer (LarcosCounterManagerPtr _manager,
                       const KKStr&            _name,
                       kkint32                 _bufferSize,
                       kkint32                 _maxNumOfBuffers,
                       kkint32                 _frameHeight,
                       kkint32                 _frameWidth,
                       bool                    _dataIsToBeCounted,
                       bool                    _dataIsToBeRecorded,
                       bool                    _sampleLastFrameBeforeFlatField
                      );

    ~CameraFrameBuffer ();


    ScannerFileEntryPtr  CurScannerFileEntry            () const {return curScannerFileEntry;}
    bool                 DataIsToBeRecorded             () const {return dataIsToBeRecorded;}
    bool                 DataIsToBeCounted              () const {return dataIsToBeCounted;}
    kkint32              FrameHeight                    () const {return frameHeight;}
    kkint32              FrameWidth                     () const {return frameWidth;}
    kkint32              MaxNumOfBuffers                () const {return maxNumOfBuffers;}
    kkint64              NextFrameSeqNum                () const {return nextFrameSeqNum;}
    kkint32              NumAvailable                   () const;
    kkint32              NumWaitingToProcess            () const;
    kkint32              NumWaitingToWriteToDisk        () const;
    kkint32              PhysicalFramesDropped          () const {return physicalFramesDropped;}
    kkint32              SampleLastFrameBeforeFlatField () const {return sampleLastFrameBeforeFlatField;}

    void  GetStats (LarcosCounterStats&  larcosCameraStats) const;


    bool  FlatFieldEnabled  () const;
    void  FlatFieldEnabled (bool _enabled);

    VectorUcharPtr  CameraHighPoints ()  const;

    VectorUcharPtr  CameraHighPointsFromLastNSampleLines (kkint32 n)  const;


    // Returns an estimate of the amount of memory consumed in bytes.  This will 
    // help managed objects keep track of how much memory they are using in the 
    // unmanaged world.
    kkMemSize  MemoryConsumedEstimated ();  


    void  MaxNumOfBuffers (kkint32 _maxNumOfBuffers)  {maxNumOfBuffers = _maxNumOfBuffers;}


   /**
     *@brief Informs us that a new scanner file is to be created or we are starting to read from a new one.
     *@details The next 'CameraFrame' instance that is added to this buffer will be flagged with
     * the ScannerFileEntry that goes with "_scannerFile".  This instance of "ScannerFileEntry" will
     * act as a flag to the 'DiskWriterThread' thread that it needs to close out the current scanner file 
     * and start a new one.
     */
    void  StartNewScannerFile (ScannerFileEntryPtr  _scannerFileEntry);

    void  AddFrame (uchar*   cameraData,
                    kkint32  cameraDataSize,
                    kkint32  height,
                    kkint32  width,
                    kkuint32 flowMeterCounter,
                    kkuint32 flowMeterCounterScanLine,
                    float    flowRateRatio,
                    float    flowRate
                   );

    void  DataIsToBeCounted  (bool _dataIsToBeCounted);
    void  DataIsToBeRecorded (bool _dataIsToBeRecorded);

    void  DataIsToBeCountedAndOrRecorded (bool _dataIsToBeCounted,
                                          bool _dataIsToBeRecorded
                                         );

    CameraFramePtr  GetNextFrameToWriteToDisk ();
    void  DoneWrittingFrameToDisk (CameraFramePtr  fram);

    CameraFramePtr  GetNextFrameToProcess ();
    void  DoneProcessingFrame (CameraFramePtr  frame);

    void  SampleLastFrameBeforeFlatField (bool _sampleLastFrameBeforeFlatField)  {sampleLastFrameBeforeFlatField = _sampleLastFrameBeforeFlatField;}

    void  SetCompensationTable (const uchar* _compensationTable);

    void  SetLastFramesMinScanLines (kkint32 _lastFramesMinScanLines);

    RasterPtr  SnapShotLatestFrame (kkint32 cropLeft,
                                    kkint32 cropRight
                                   );

    VectorUcharPtr  LastFrameAverageScanLine()  const;

    VectorUcharPtr  LastFrameHighValuesScanLine()  const;

    /**
     *@brief  The LarcosCameraManager  that owns this instance will call this method when the Acquisition Thread
     * informs it that the Scan-Rate has changed.
     */
    void   ScanRateChanged (float _newScanRate);


  private:
    /**@brief  Call this method when ever FlatField is enabled and/or ScanRate is changed. */
    void  ComputeFlatFeildSamplingInterval ();

    void  IncreaseBufferSize (kkint32 numNewFrameBuffers);

    void  LastFramesAllocate ();

    void  LastFramesCleanUp ();

    /** @brief  Remove the oldest frame in the 'waitingToProcess' queue and has already been written to disk.     */
    void  ThrowOutOldestOccupiedBuffer ();

    void  UpdateLastFrame (uchar*  cameraData, 
                           kkint32 cameraDataSize,
                           kkint32 height,
                           kkint32 width
                          );


    CameraFrameListPtr      available;             /**< Empty frames that are available to be populated from camera. */

    CameraFrameListPtr      buffer;                /**< All frames will have an entry in this list.                   */


    const uchar*            compensationTable;          /**< From ScannerFile::ConpensationTable(); used to compensate for the effects of ScannerFile compression. */
    ScannerFileEntryPtr     curScannerFileEntry;
    bool                    dataIsToBeCounted;          /**< When adding data to frame buffer flag it to be processed for counting.  */
    bool                    dataIsToBeRecorded;         /**< When adding data to frame buffer flag it to be written to disk.         */

    CameraFlatFieldCorrectionPtr flatFieldCorrection;

    kkint32                 flatFeildSamplingInterval; /**< Sampling interval in frames fort Flat-Field correction
                                                        * This value will be computed such that we sample 10 seconds
                                                        * of imagery; ex if FlatField is maintaining a history of 20 sampling lines
                                                        * and scan rate is 10,000 lps  we would need to sample once every
                                                        * 5,000 scan lines.
                                                        */
    kkint32                 frameHeight;
    kkint32                 frameWidth;
    GoalKeeperPtr           gateKeeper;
    RasterPtr*              lastFrames;                 /**< An array that implements a wrap around buffer that will contain a copy of the
                                                         * most current frames added to the 'cameraFrameBuffer'.
                                                         */

    kkint32                 lastFramesIdx;              /**< Index of last frame that has been added to 'lastFrames' */

    kkint32                 lastFramesMinScanLines;     /**<  The minimum number of total scan lines that we want to retain in 'lastFrames'. */

    kkint32                 lastFramesSize;             /**< Size of the 'lastFrames' array; each element will have a 'Raster' instance
                                                         * assign to it.
                                                         */

    LarcosCounterManagerPtr manager;

    kkint32                 maxNumOfBuffers;
    KKStr                   name;                       /**< Name of camera buffer, needs to be unique amongst all camera buffer instances. */
    kkint64                 nextFrameSeqNum;
    kkint32                 physicalFramesDropped;
    bool                    sampleLastFrameBeforeFlatField;

    kkint32                 scannerFileNumScanLines;     /**< The number of Scan-Lines that have been added to the
                                                          * current ScannerFile.   Since the flow of image data must all go
                                                          * thru a 'CameraFrameBuffer' instance I decided to keep track of the
                                                          * current scanner file that is being Read from or Written to here.
                                                          * Ex: when the 'DiskWriterThread' decides to start a new Scanner
                                                          * file; it will send a message to this object thru 'LarcosCounterManager'
                                                          * and then this instance will flag all following frames with the
                                                          * new ScannerFile name and start the line count back at 0.  This will
                                                          * also allow the 'LogicalFrameBuilder' thread to attach scan lines to
                                                          * 'LogicalFrame' instances it crates allowing the 'LogicalFrameProcessor'
                                                          *  threads to know the ScannerFile and Scan-Line that each particle
                                                          *  it creates exists at.
                                                          */

    RasterPtr               snapShotLatestFrameWorkArea; /**< Work area to copy lastFarme0 & 1 over to. */

    CameraFrameListPtr      waitingToProcess;
    CameraFrameListPtr      waitingToWriteToDisk;
  };  /* CameraFrameBuffer */


  typedef  CameraFrameBuffer::CameraFrameBufferPtr  CameraFrameBufferPtr;
}  /* LarcosCounterUnManaged */

#endif

