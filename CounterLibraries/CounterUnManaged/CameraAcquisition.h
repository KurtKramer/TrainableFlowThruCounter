/* CameraAcquisition.h -- Base class for the Acquisition threads.
 * Copyright (C) 2011-2013  Kurt Kramer
 * For conditions of distribution and use, see copyright notice in CounterUnManaged.txt
 */
#if  !defined(_CAMERAACQUISITION_)
#define  _CAMERAACQUISITION_


#include "MsgQueue.h"
#include "RunLog.h"
using namespace  KKB;

#include "FlowMeterTracker.h"
using namespace  KKLSC;

#include "CameraThread.h"
#include "PreClassDefinitions.h"

namespace CounterUnManaged
{
  class  CommandEntry;
  typedef  CommandEntry*  CommandEntryPtr;

  class  CommandEntryQueue;
  typedef  CommandEntryQueue*  CommandEntryQueuePtr;


  /**
   *@class  CameraAcquisition
   *@brief  Base class to be used by all devices that acquisition data such as cameras or even previously recoded data.
   *@see  CameraAcquisitionPleora
   *@see  CameraAcquisitionSimulator
   */
  class  CameraAcquisition:  public  CameraThread
  {
  public:
    enum  class  StartStatusType
                 {Null,
                  Connecting,
                  ConnectionFailed,
                  Connected,
                  Disconnected,
                  Stoped
                 };


    CameraAcquisition (CounterManagerPtr _manager,
                       CameraFrameBufferPtr    _frameBuffer,
                       FlowMeterTrackerPtr     _flowMeter,
                       MsgQueuePtr             _msgQueue,
                       const KKStr&            _threadName
                      );


    CameraAcquisition (CounterManagerPtr _manager,
                       CameraParametersPtr     _cameraParams,
                       CameraFrameBufferPtr    _frameBuffer,
                       FlowMeterTrackerPtr     _flowMeter,
                       MsgQueuePtr             _msgQueue,
                       const KKStr&            _threadName
                      );

    virtual
      ~CameraAcquisition ();

    /**
     *@brief Returns an estimate of the amount of memory consumed in bytes.
     *@details This will help managed objects keep track of how much memory they 
     * are using in the unmanaged world.
     */
    kkMemSize  MemoryConsumedEstimated () const;

    virtual  void  GetStats (CounterStats&  stats)  const;


    CameraParametersPtr  CameraParams                ()  const  {return cameraParams;}
    float                CurAnalogGain               ()  const  {return curAnalogGain;}           /**< Analog  gain that camera is currently functioning at. */
    kkint32              CurDigitalGain              ()  const  {return curDigitalGain;}          /**< Digital gain that camera is currently functioning at. */
    float                CurScanRate                 ()  const  {return curScanRate;}             /**< ScanRate that camera is currently functioning at. */
    const KKStr&         CurSensitivityMode          ()  const  {return curSensitivityMode;}
    kkuint32             FlowMeterCounter            ()  const  {return flowMeterCounter;}        /** Value of Flow-Meter-Counter as of scan line indicated by 'flowMeterCounterScanLine'. */
    kkuint32             FlowMeterCounterScanLine    ()  const  {return flowMeterCounterScanLine;}
    kkint32              FrameHeight                 ()  const;                                   /**< Height of Frames that is being delivered.             */
    float                FrameRate                   ()  const  {return frameRate;}               /**< Frames per second;  updated every tenth frame.        */
    kkint32              CameraTemparature           ()  const  {return cameraTemparature;}
    kkint32              FrameWidth                  ()  const;
    const KKStr&         IpAddress                   ()  const;
    kkuint64             LastBlockId                 ()  const  {return lastBlockId;}
    const KKStr&         MacAddress                  ()  const;
    kkint32              PhysicalFramesRead          ()  const  {return physicalFramesRead;}
    kkint32              PhysicalSeqNumsSkipped      ()  const  {return physicalSeqNumsSkipped;}  /**< Indicates the number of frames not retrieved from the Pleora driver (Dropped) since last call to "ResetCounts". */
    float                RequestedAnalogGain         ()  const  {return requestedAnalogGain;}     /**< Analog-Gain last requested; you need to check "CurAnalogGain" for current AnalogGain being used.*/
    kkint32              RequestedDigitalGain        ()  const  {return requestedDigitalGain;}
    const KKStr&         RequestedSensitivityMode    ()  const  {return requestedSensitivityMode;}
    kkint32              ScanLinesRead               ()  const  {return scanLinesRead;}           /**< Total number of scan-lines read since last call to "ResetCounts". */
    float                ScanRate                    ()  const;
    const KKStr&         SerialNumber                ()  const;
    StartStatusType      StartStatus                 ()  const  {return startStatus;}
    const KKStr&         StatusMsg                   ()  const  {return statusMsg;}
    kkint32              TotalLostPackets            ()  const  {return totalLostPackets;}



    /**
     *@brief  Called by 'LarcosCameraManager' whenever one of the Crop settings change.
     */
    virtual
    void  CropSettingsChanged (kkint32  _cropLeft,
                               kkint32  _cropRight
                              );

    /**
     *@brief  Called by 'LarcosCameraManager' whenever ScanRate changes.
     */
    virtual
    void   ScanRateChanged (float _newScanRate);



    void  AddAnalogGainCommand (float _analogGain);

    void  AddDigitalGainCommand (kkint32 _digitalGain);

    void  AddScanRateCommand (float _scanRate);

    void  AddSensitivityModeCommand (const KKStr&  _sensitivityMode);
 
    void  CameraParametersAddToHeaderFields ();

    bool  FlatFieldEnabled ()  const;

    void  FlatFieldEnabled (bool  _flatFieldEnabled);

    /**
     * Will check to see if height or width had changed, if so will inform the Manager object.
     */
    void  FrameHeightWidth (kkint32  _frameHeight,
                            kkint32  _frameWidth
                           );

    void  RequestedCameraParameters (OperatingParametersPtr  opParms);

    void  RequestedAnalogGain (float _requestedAnalogGain);

    void  RequestedDigitalGain (kkint32 _requestedDigitalGain);

    void  RequestedScanRate (float _requestedScanRate);

    void  RequestedSensitivityMode (const KKStr&  _requestedSensitivityMode);

    virtual  void  Run ();

    virtual  void  ResetCounts ();

    /**
     *@brief  Call this method to set Gain settings.  
     *@details  This method needs to be implemented by any class that is derived from 'CameraAcquisition'.
     */
    virtual  void  PerformAutoGainProcedure () = 0;


  protected:
    bool  QueueActive () const;  /**< Indicates that there might be entries in 'commandQueue'; that is you
                                  * should make a call to 'GetNextCommandEntry' to get any entries.
                                  */


    CommandEntryPtr  GetNextCommandEntry ();

    void  FlowMeterCounterUpdate (kkuint32 scanLineNum,
                                  kkuint32 counter
                                 );

    
    void  StartStatus (StartStatusType  _startStatus,
                       const KKStr&     _statusMsg
                      );


    float  ComputeFlowRateFromFlowRateRatio ()  const;


    CameraParametersPtr    cameraParams;
    kkint32                cameraTemparature;
    CommandEntryQueuePtr   commandQueue;           /**< Commands for the camera (ScanRate, AnalogGain, etc) */
    float                  curAnalogGain;          /**< Analog gain last reported by camera. */
    kkint32                curDigitalGain;         /**< Digital Gain last reported by camera. */
    float                  curScanRate;
    KKStr                  curSensitivityMode;

    FlowMeterTrackerPtr    flowMeter;              /**< Provided by CounterManager; DO NOT Own.  */

    kkuint32               flowMeterCounter;       /**< Flow Meter counter value as scan line indicated by 'flowMeterCounterScanLine'
                                                    * This value is currently coming thru the imagery data as the 1st four pixels in 
                                                    * each scan line.  In the future it may also come thru a separate I/O device.
                                                    */

    kkuint32               flowMeterCounterScanLine;

    CameraFrameBufferPtr   frameBuffer;            /**< We will not own this instance    */
    float                  frameRate;              /**< Frames per second;  updated every tenth frame. */  

    kkuint64               lastBlockId;            /**< Pleora assigns a sequential number to all frames that wraps around at 65535. */
    kkint32                outputQueueSize;
    kkint32                physicalFramesRead;
    kkint32                physicalSeqNumsSkipped; /**<  Frames that were skipped when retrieving from the camera. */
    float                  requestedAnalogGain;
    kkint32                requestedDigitalGain;
    float                  requestedScanRate;
    KKStr                  requestedSensitivityMode;
    kkint32                scanLinesRead;          /**< Total number of scan lines read. */
    StartStatusType        startStatus;
    KKStr                  statusMsg;
    kkint32                totalLostPackets;
  }; /* CameraAcquisition */

  typedef  CameraAcquisition*  CameraAcquisitionPtr;





  /**
   *@class CameraAcquisition::CommandEntry
   *@brief  Used to queue updates to the Camera that need to be done, such as request to update ScanRate.
   */
  class  CommandEntry
  {
  public:
    enum  class  EntryTypes {Null, ScanRate, AnalogGain, DigitalGain, SensitivityMode};

    CommandEntry (EntryTypes  _commandType,
                  float       _parameter
                 );

    CommandEntry (EntryTypes  _commandType,
                  kkint32     _parameter
                 );

    CommandEntry (EntryTypes    _commandType,
                  const KKStr&  _parameter
                 );

    float          AnalogGain      () const {return analogGain;}
    kkint32        DigitalGain     () const {return digitalGain;}
    EntryTypes     CommandType     () const {return commandType;}
    float          ScanRate        () const {return scanRate;}
    const KKStr&   SensitivityMode () const {return sensitivityMode;}

  private:
    EntryTypes  commandType;
    float       analogGain;
    kkint32     digitalGain;
    float       scanRate;
    KKStr       sensitivityMode;
  };  /* CommandEntry */



  class  CommandEntryQueue
  {
  public:
    CommandEntryQueue ();

    ~CommandEntryQueue ();

    void  AddAnalogGainCommand (float _analogGain);

    void  AddDigitalGainCommand (kkint32 _digitalGain);

    void  AddScanRateCommand (float _scanRate);

    void  AddSensitivityMode (const KKStr& _sensitivityMode);

    CommandEntryPtr  GetNextCommand ();

    void  PushOnBack (CommandEntryPtr  cmd);

    void  PushOnFront (CommandEntryPtr  cmd);

    CommandEntryPtr  PopFromFront ();

    bool  QueueActive () const {return queueActive;}

  private:
    KKQueue<CommandEntry>  cmdQueue;

    GoalKeeperPtr          goalie;

    volatile bool          queueActive; /**< Indicates that the 'CameraAquision' thread needs to check 'commandQueue'     *
                                         * for entries.  The 'CameraAcquisition' thread will set this variable to 'false' *
                                         * while other threads can set it to 'true'.  The idea is that I want to minimize *
                                         * calls to 'goalie' to reduce O/S overhead.                                      *
                                         */
  };  /* CommandEntryQueue */



}  /* CounterUnManaged */

#endif

