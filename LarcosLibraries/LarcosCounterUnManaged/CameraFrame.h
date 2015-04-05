/* CameraFrame.h -- Represents a single camera frame.
 * Copyright (C) 2011-2013  Kurt Kramer
 * For conditions of distribution and use, see copyright notice in LarcosCounterUnManaged.txt
 */
#if  !defined(_CAMERAFRAME_)
#define  _CAMERAFRAME_

#include "KKQueue.h"
#include "Raster.h"

#include "ScannerFileEntry.h"


namespace LarcosCounterUnManaged
{
  #if  !defined(_CAMERAFLATFIELDCORRECTION_)
  class  CameraFlatFieldCorrection;
  typedef  CameraFlatFieldCorrection*  CameraFlatFieldCorrectionPtr;
  #endif

  class  CameraFrame
  {
  public:
    typedef  CameraFrame*  CameraFramePtr;

    CameraFrame (kkint32 _frameHeight,
                 kkint32 _frameWidth
                );

    ~CameraFrame ();

    kkuint32             FlowMeterCounter         ()  const    {return flowMeterCounter;}
    kkuint32             FlowMeterCounterScanLine ()  const    {return flowMeterCounterScanLine;}
    float                FlowRate                 ()  const    {return flowRate;}
    float                FlowRateRatio            ()  const    {return flowRateRatio;}
    RasterPtr            Frame                    ()  const    {return frame;}
    kkint32              FrameHeight              ()  const;
    kkint32              FrameWidth               ()  const;
    bool                 Processed                ()  const    {return processed;}
    ScannerFileEntryPtr  ScannerFile              ()  const    {return scannerFileEntry;}
    const KKStr&         ScannerFileRootName      ()  const;
    const KKStr&         ScannerFileFullName      ()  const;
    kkint32              ScannerFileScanLine      ()  const    {return scannerFileScanLine;}
    kkint64              SeqNum                   ()  const    {return seqNum;}
    bool                 WaitingToWriteToDisk     ()  const    {return waitingToWriteToDisk;}
    bool                 WaitingToProcess         ()  const    {return waitingToProcess;}
    bool                 WrittenToDisk            ()  const    {return writtenToDisk;}
 
    void  FlowMeterCounter         (kkuint32  _flowMeterCounter)          {flowMeterCounter         = _flowMeterCounter;}
    void  FlowMeterCounterScanLine (kkuint32  _flowMeterCounterScanLine)  {flowMeterCounterScanLine = _flowMeterCounterScanLine;}
    void  Processed                (bool      _processed)                 {processed                = _processed;}
    void  SeqNum                   (kkint64   _seqNum)                    {seqNum                   = _seqNum;}
    void  WaitingToProcess         (bool      _waitingToProcess)          {waitingToProcess         = _waitingToProcess;}
    void  WaitingToWriteToDisk     (bool      _waitingToWriteToDisk)      {waitingToWriteToDisk     = _waitingToWriteToDisk;}
    void  WrittenToDisk            (bool      _writtenToDisk)             {writtenToDisk            = _writtenToDisk;}

    uchar  operator() (kkint32 row,  kkint32 col)  const;


    void  PopualteFrameFromCameraData (uchar*              _cameraData,
                                       kkint32             _cameraDataSize,
                                       bool                _complementData,
                                       ScannerFileEntryPtr _scannerFileEntry,
                                       kkint32             _scannerFileScanLine,
                                       kkuint32            _flowMeterCounter,
                                       kkuint32            _flowMeterCounterScanLine,
                                       float               _flowRateRatio,
                                       float               _flowRate
                                      );

    void  ReSizeFrame (kkint32 newHeight,
                       kkint32 newWidth
                      );

    RasterPtr  ToRaster ()  const;  /**<  Will return a Duplicate of the frames Raster */

  private:
    kkuint32             flowMeterCounter;     /**< Flow-Meter-Counter;  the counter value as of the scan line indicated by 
                                                * 'flowMeterCounterScanLine'.  On some installations this value will be transmitted 
                                                * via the imagery data in the first 4 pixel columns on each scan line.
                                                */

    kkuint32             flowMeterCounterScanLine;

    float                flowRate;             /**< Meters per second as derived from FlowMeter;  if no FlowMeter present will then compute from 
                                                * FlowRateRatio.
                                                */

    float                flowRateRatio;        /**< Computed Flow-Rate-Factor from 'FlowMeterTracker::FlowRateToRatio'; this value is only useful if there
                                                * is a Flow-Meter connected to Larcos.  The initial version will transmit the FlowMeter counter valued as
                                                * embedded data in the imagery; 1st 4 pixel columns in each scan-line.
                                                */

    RasterPtr            frame;
    bool                 processed;            /**< Indicates if contents have been processed.                                                             */
    kkint64              seqNum;               /**< sequential number assigned to each frame grabbed from camera.                                          */

    ScannerFileEntryPtr  scannerFileEntry;     /**< Scanner file that frame is to be written to or was read from depending on whether we are recording   *
                                                *  or Playing-back
                                                */

    kkint32              scannerFileScanLine;  /**< Scan-line that the 1st scan-line in 'frame' represents with respect to 'scannerFileRootName' scanner file. */


    bool                 waitingToProcess;     /**< Indicates that frame is in the 'waitingToProcess'      queue of the CameraFrameBuffer object.              */
    bool                 waitingToWriteToDisk; /**< Indicates that frame is in the 'waitingToWrirteToDisk' queue of the CameraFrameBuffer object.              */
    bool                 writtenToDisk;        /**< Indicates if frame has been written to disk yet.  @see CameraFrameBuffer::ThrowOutOldestOccupiedBuffer     */
  };  /* CameraFrame */


  typedef  CameraFrame::CameraFramePtr  CameraFramePtr;


  class  CameraFrameList:  public KKQueue<CameraFrame> 
  {
  public:
    CameraFrameList (bool  _owner);
    ~CameraFrameList ();
  };

  typedef  CameraFrameList* CameraFrameListPtr;

}  /* LarcosCounterUnManaged */

#endif
