/* CameraAcquisitionSimulator.h --
 * Copyright (C) 2011-2013  Kurt Kramer
 * For conditions of distribution and use, see copyright notice in CounterUnManaged.txt
 */
#if  !defined(_CAMERASIMULATOR_)
#define  _CAMERASIMULATOR_

#include "RunLog.h"

#include "CameraAcquisition.h"

namespace CounterUnManaged
{
  #if  !defined(_CAMERAFRAME_)
  class  CameraFrame;
  typedef  CameraFrame*  CameraFramePtr;
  #endif

  #if  !defined(_CAMERAFRAMEBUFFER_)
  class  CameraFrameBuffer;
  typedef  CameraFrameBuffer*  CameraFrameBufferPtr;
  #endif

  #if  !defined(_SCANNERFILE_)
  class  ScannerFile;
  typedef  ScannerFile*  ScannerFilePtr;
  #endif
  

  /**
   *@class  CameraAcquisitionSimulator
   *@brief  Simulates the function of class '.
   */
  class CameraAcquisitionSimulator: public  CameraAcquisition
  {
  public:
    CameraAcquisitionSimulator 
                    (CounterManagerPtr     _manager,
                     bool                  _playingBackRealTime,
                     CameraFrameBufferPtr  _frameBuffer,
                     const KKStr&          _fileName,
                     FlowMeterTrackerPtr   _flowMeter,
                     MsgQueuePtr           _msgQueue,
                     const KKStr&          _threadName
                    );

    virtual
    ~CameraAcquisitionSimulator ();

    virtual  void  Run ();

    virtual  void  ResetCounts ();

    virtual  void  PerformAutoGainProcedure ();

  private:
    void  AllocateDynamicMemory ();

    void  ComplementDataBlock (void*   block,
                               kkint32 blockSize
                              );

    void  GetNextFrame2 (uchar*  buffer,
                         bool&   eof
                        );

    void  BuildListOfScannerFilesToProcess ();

    void  OpenNextScannerFile ();


    kkuint32*         colCount;
    KKStr           curFileName;             /**< One scanning session could of resulted in multiple Scanner Files being created.
                                              *   This variable will reflect the one that is currently open.
                                              */
    KKStr           fileName;
    kkuint32        lineSize;
    kkuint32        pixelsInRow;
    bool            playingBackRealTime;

    KKStrList       scannerFilesToProcess;  /**<  List of scanner files that we need to process yet.  When we are done
                                              * processing one scanner file we will retrieve the next one by popping off
                                              * its name from the front of 'scannerFilesToProcess'.   Because Counter starts
                                              * a new ScannerFile at 1GBboundaries.  There can be several physical files
                                              * that represent one scanning session.
                                              */

    ScannerFilePtr  sf;
    kkint64         totalBytesRead;
    kkint64         totalBytesToRead;        /**< Represents the total bytes to read from all Scanner Files that were located in
                                              * 'BuildListOfScannerFilesToProcess'.
                                              */
  };

  typedef  CameraAcquisitionSimulator*  CameraAcquisitionSimulatorPtr;
}  /* CounterUnManaged */

#endif

