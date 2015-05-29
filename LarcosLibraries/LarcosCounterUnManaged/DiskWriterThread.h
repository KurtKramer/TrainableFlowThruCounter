/* DiskWriterThread.h -- 
 * Copyright (C) 2011-2013  Kurt Kramer
 * For conditions of distribution and use, see copyright notice in LarcosCounterUnManaged.txt
 */
#if  !defined(_DISKWRITERTHREAD_)
#define  _DISKWRITERTHREAD_

#include "GoalKeeper.h"
#include "KKQueue.h"
#include "MsgQueue.h"
#include "Raster.h"
#include "RunLog.h"

#include "ScannerFile.h"



#include "CameraThread.h"


namespace LarcosCounterUnManaged
{
  #if  !defined(_CAMERAFRAMEBUFFER_)
  class  CameraFrameBuffer;
  typedef  CameraFrameBuffer*  CameraFrameBufferPtr;
  #endif

  #if  !defined(_CAMERAFRAME_)
  class  CameraFrame;
  typedef  CameraFrame*  CameraFramePtr;
  #endif

  #if  !defined(_LARCOSCOUNTERSTATS_)
    class  LarcosCounterStats;
    typedef  LarcosCounterStats*  LarcosCounterStatsPtr;
  #endif


  /**
   *@brief  This will manage the writing of raw camera frame data to a disk.
   *@details  This classes primary responsibility is to grab frames of data from
   *          the provided 'CameraFrameBuffer' instance and write them to disk.
   */
  class DiskWriterThread: public  CameraThread
  {
  public:
    typedef  KKLSC::ScannerFile::Format  ScannerFileFormat;

    enum  class  DiskWritingStatus  
                   {dwNULL,
                    Recording,
                    NotRecording,
                    DroppingFrames
                   };

    DiskWriterThread (LarcosCounterManagerPtr  _manager,
                      CameraFrameBufferPtr     _cameraBuffer,
                      MsgQueuePtr              _msgQueue,
                      const KKStr&             _scannerFileName,
                      ScannerFile::Format      _format,
                      kkint32                  _frameWidth,
                      kkint32                  _frameHeight,
                      const KKStr&             _threadName
                     );

    virtual
      ~DiskWriterThread ();
 
    /**
     *@brief  Returns an estimate of the amount of memory consumed in bytes.
     *@details This will help managed objects keep track of how much memory 
     * they are using in the unmanaged world.
     */
    kkint32  MemoryConsumedEstimated ();
     
    virtual
      void  GetStats (LarcosCounterStats&  stats)  const;

    kkint64            BytesWritten        ()  const  {return  bytesWritten;}
    kkint64            BytesWrittenAllSegs ()  const  {return  bytesWrittenAllSegs;}
    const KKStr&       CurScannerFileName  ()  const  {return  curScannerFileName;}
    DiskWritingStatus  DiskStatus          ()  const  {return  dwStatus;}
    ScannerFileFormat  Format              ()  const  {return  format;}
    kkint32            FrameSeqNumBreaks   ()  const  {return  numSkippedFrames;}
    kkint32            NumSkippedFrames    ()  const  {return  numSkippedFrames;}
    bool               DataIsToBeRecorded  ()  const  {return  dataIsToBeRecorded;}
    long               ScanLinesWritten    ()  const  {return  scanLinesWritten;}

    //void  DataIsToBeRecorded (bool               _dataIsToBeRecorded)  {dataIsToBeRecorded  = _dataIsToBeRecorded;}

    void  AddHeaderField (const KKStr&  _fieldName,
                          const KKStr&  _fieldValue
                         );

    void  AddHeaderField (const KKStr&  _fieldName,
                          kkint32       _fieldValue
                         );

    void  AddHeaderField (const KKStr&  _fieldName,
                          kkint64       _fieldValue
                         );

    void  AddHeaderField (const KKStr&  _fieldName,
                          double        _fieldValue
                         );

    /** 
     *@brief  Will queue a message to be written to the current Scanner File.
     *@details This assumes that the format of the current Scanner file supports text messages.
     */
    void  AddMessageToCurrentScannerFile (const KKStr&  msg);


    virtual  void  ResetCounts ();


    /**
     *@brief  Call this Method to start writing camera frame data to disk; will stay in loop until cancelFlag is true.
     */
    virtual void  Run ();

    bool   WeAreRecordingToDisk ();

  private:
    void  AddCameraLineToHeaderFields (ScannerFilePtr  scannerFile);

    CameraFramePtr  GetNextFrame ();



    kkint64               bytesWritten;
    mutable  kkint64      bytesWrittenAllSegs;
    kkint64               bytesWrittenCompletedSegs;
    CameraFrameBufferPtr  cameraBuffer;
    KKStr                 curScannerFileName;          /**< Current file that we are writing scanner data to. */
    KKStr                 curScannerFileRootName;
    DiskWritingStatus     dwStatus;
    bool                  firstFrameYet;
    ScannerFileFormat     format;
    kkint64               frameSeqNumLast;
    kkint32               frameHeight;
    kkint32               frameWidth;
    long                  framesWritten;
    MsgQueuePtr           msgs;                        /**< Messages that need to be written to the Scanner file will be queued in this structure. */
    bool                  msgsAreQueued;               /**< Flag to let the 'WrittingThread' loop know that there are messages queued to be written to the scanner file. */
    kkint32               numFramesSinceLastSkip;
    kkint32               numSkippedFrames;
    bool                  dataIsToBeRecorded;
    kkint32               scannerFileBaseCount;        /**< Number of times current Base Scanner File was restarted when old one exceeded 'scannerFileSizeMax'. */
    KKStr                 scannerFileBaseName;
    KKStr                 scannerFileBaseExtension;
    KKStr                 scannerFileDir;
    kkint64               scannerFileSize;             /**< Number bytes written to current ScannerFile   */
    kkint64               scannerFileSizeMax;          /**< Maximum number of bytes to write to a scanner file before starting a new one. */
    long                  scanLinesWritten;
    bool                  startNewScannerFile;
    KKStr                 startNewScannerFileName;
  };   /* DiskWriterThread */

  typedef  DiskWriterThread*  DiskWriterThreadPtr;
}


#endif
