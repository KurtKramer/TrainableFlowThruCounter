/* CameraAcquisitionPleora.h -- Represents a single camera frame.
 * Copyright (C) 2011-2013  Kurt Kramer
 * For conditions of distribution and use, see copyright notice in LarcosCounterUnManaged.txt
 */
#if  !defined(_CAMERAACQUISITIONPLEORA_)
#define  _CAMERAACQUISITIONPLEORA_

#include <PvDeviceInfo.h>
#include <PvDevice.h>
#include <PvPipeline.h>
#include <PvBuffer.h>
#include <PvStream.h>

#include "MsgQueue.h"

#include "CameraAcquisition.h"
#include "PreClassDefinitions.h"


namespace LarcosCounterUnManaged
{
//  #ifndef __PV_DEVICEINFO_H__
//  class  __declspec( dllimport ) PvDeviceInfo;
//  #endif
  typedef  PvDeviceInfo*  PvDeviceInfoPtr;
  
  //#ifndef __PV_DEVICE_H__
  //class  PvDevice;
  //#endif

  typedef  PvDevice*    PvDevicePtr;
  typedef  PvStream*    PvStreamPtr;
  typedef  PvPipeline*  PvPipelinePtr;
  

  /**
   *@class  CameraAcquisitionPleora
   *@brief  Manages the retrieval of image frames from a Pleora driven camera.
   *@details  Inspiration comes from 'PvPipelineSample' from 'Pleora Technologies Inc.'
   */
  class CameraAcquisitionPleora: public  CameraAcquisition
  {
  public:
    typedef  CameraAcquisitionPleora*  CameraAcquisitionPleoraPtr;


    static
    CameraParametersListPtr  GetCameraList (MsgQueuePtr  _msgQueue);



    static
    CameraParametersPtr  GetDeviceInfo (const KKStr&  _keyValue,
                                        char          _keyType, /* 'S' = Serial Number, 'M' = MAC Address */
                                        RunLog&       _runLog
                                       );


    static
    CameraAcquisitionPleoraPtr  CreateFromSerialNumber 
           (LarcosCounterManagerPtr _manager,
            const KKStr             _serialNumber,
            CameraFrameBufferPtr    _frameBuffer,
            FlowMeterTrackerPtr     _flowMeter,
            MsgQueuePtr             _msgQueue,
            const KKStr&            _threadName
           );


    static
    CameraAcquisitionPleoraPtr  CreateFromMacAddress 
           (LarcosCounterManagerPtr _manager,
            const KKStr&            _macAddress,
            CameraFrameBufferPtr    _frameBuffer,
            FlowMeterTrackerPtr     _flowMeter,
            MsgQueuePtr             _msgQueue,
            const KKStr&            _threadName
           );


    static
    CameraAcquisitionPleoraPtr  CreateFromCameraParameters
           (LarcosCounterManagerPtr _manager,
            CameraParametersPtr     _cameraParameters,
            CameraFrameBufferPtr    _frameBuffer,
            FlowMeterTrackerPtr     _flowMeter,
            MsgQueuePtr             _msgQueue,
            const KKStr&            _threadName
           );



    
    static
    CameraParametersPtr  GetCameraParameters (const KKStr&  _macAddress,
                                              MsgQueuePtr   _msgQueue,
                                              RunLog&       _runLog
                                             );



    static
    KKStr  PromptForCameraMacAddress ();

    CameraAcquisitionPleora (LarcosCounterManagerPtr _manager,
                             const KKStr&            _macAddress,
                             CameraFrameBufferPtr    _frameBuffer,
                             FlowMeterTrackerPtr     _flowMeter,
                             MsgQueuePtr             _msgQueue,
                             const KKStr&            _threadName
                            );

    CameraAcquisitionPleora (LarcosCounterManagerPtr _manager,
                             CameraParametersPtr     _cameraParams,
                             CameraFrameBufferPtr    _frameBuffer,
                             FlowMeterTrackerPtr     _flowMeter,
                             MsgQueuePtr             _msgQueue,
                             const KKStr&            _threadName
                            );



    virtual
      ~CameraAcquisitionPleora ();

    virtual  void  Run ();

    virtual  void  ResetCounts ();

    /**
     * @brief  Executes the "Auto-Gain" function for "Pleora" based cameras.
     */
    virtual  void  PerformAutoGainProcedureOld ();
    virtual  void  PerformAutoGainProcedure ();


  private:
    
    static
    const char*  PvGenTypeToCStr (PvGenType  genType);
    
    void  AddToHeaderField (PvGenParameter*  p);

    void  AddPleoraVariableToHeaderField (const KKStr&  varName);

    void  AnalogGainBinarySearch (kkuint16 threshholdSaturationValue);

    void  ApplyCommandEntries ();

    void  ApplyCommandNextEntry ();

    /**
     *@brief Computes Frame Intervals from Time Intervals used to perform expensive tasks during camera processing.
     *@details should be called when ever the ScanRate or FrameSize is changed.
     */
    void  ComputeFramneIntervals ();

    void  DigitalGainBinarySearch ();

    void  InitializeDeviceParameterAccessVariables ();
    void  InitializeStreamParameterAccessVariables ();

    void  PerformAutoCrop ();

    void  SetCameraParametersForFlowMeter ();

    void  SetGainTap         (kkint64 gainTap);
    void  SetAnalogGain      (float   requestedAnalogGain);
    void  SetDigitalGain     (kkint32 requestedDigitalGain);
    void  SetScanRate        (float   requestedScanRate);
    void  SetSensitivityMode (const   KKStr& requestedSensitivityMode);
    
    void  SetPvParameterEnum (const KKStr&  paramName,
                              const KKStr&  value,
                              bool&         successful,
                              KKStr&        errMsg
                             );

    void  SetPvParameterBool (const KKStr&  paramName,
                              bool          value,
                              bool&         successful,
                              KKStr&        errMsg
                             );

    static
    KKStr  Int64ToIpAddress (PvInt64 i);

    void  ConnectToCamera (bool&  connectionSuccessful);
    void  DisconnectFromCamera ();

    static
    double  GetPvParameterFloat (PvGenParameterArray*  params,
                                 const KKStr&          paramName,
                                 double                defaultValue  /**< Value if parameter not available. */
                                );

    static
    kkint32  GetPvParameterEnum (PvGenParameterArray*  params,
                                 const KKStr&          paramName,
                                 kkint32               defaultValue
                                );

    static
    KKStr  GetPvParameterEnumStr (PvGenParameterArray*  params,
                                  const KKStr&          paramName,
                                  const KKStr&          defaultValue
                                 );

    static
    kkint64  GetPvParameterInteger (PvGenParameterArray*  params,
                                    const KKStr&          paramName,
                                    kkint64               defaultValue
                                   );

    static
    KKStr  GetPvParameterString (PvGenParameterArray*  params,
                                 const KKStr&          paramName,
                                 const KKStr&          defaultValue
                                );

    static
    void  PrintDeviceParameters (const KKStr&          subName,
                                 PvGenParameterArray*  lDeviceParams
                                );
    static
    KKStr  GetPvGenParameterDesc (PvGenParameter*  p,
                                  bool             tabDelStr
                                 );

    static
    void   CameraParametersPopulate (PvGenParameterArray*  params,
                                     CameraParameters&     cameraParameters,
                                     RunLog&               runLog
                                    );

    static
    KKStr  PvResultToStr (PvResult&  r);


    /**********************************************/
    kkuint16  CameraHighValue ();

    bool  CameraSomePixelsSaturated (float percentThreshold);

    kkuint16  CameraHighValueOf3Frames ();

    /** @brief  Return s 'true' in more than 1/40th of pixels in scan line have a value greater than 253.  */
    bool  DigitalGainIsMaxed ();

    void  WaitForOneFrame ();

    void  MakeAnalogGainRequest (float  _gain);

    void  MakeDigitalGainRequest (kkint32  _gain);

    void  RaiseAnalogGainUntilSaturated2 (float  increments);

    void  SetAnalogGainToFirstHighInRange (float  analogGainStart, 
                                           float  analogGainEnd, 
                                           float  increment
                                          );

    void  RaiseDigitalGainUntilSaturated (kkint32  increments);

    void  RaiseDigitalGainUntilAtLeastHighValue (int  minHighValue);


    PvDevice*             lDevice;
    PvGenParameterArray*  lDeviceParams;   /**< Parameters of currently selected camera.  */
    PvPipeline*           lPipeline;
    PvStreamPtr           lStream;
    PvGenParameterArray*  lStreamParams;

    PvGenInteger*         lAcquisitionLinePeriod;
    PvGenFloat*           lAnalogGainAbs;
    PvGenInteger*         lCameraTemparature;
    PvGenInteger*         lDigitalGainRaw;
    PvGenInteger*         lExposureTime;
    PvGenFloat*           lFrameRate;
    PvGenInteger*         lGain;
    PvGenEnum*            lGainSelector;
    PvGenInteger*         lIPAddressParam;
    PvGenInteger*         lPayloadSize;
    PvGenCommand*         lReadVoltageAndTemperature;
    PvGenFloat*           lScanRate;
    PvGenEnum*            lSensitivityMode;
    PvGenCommand*         lStart;
    PvGenCommand*         lStop;
    PvGenInteger*         lTLLocked;


    kkint32               oneSecFrameInterval;    /**< Number of frames per second. */ 
    kkint32               twoSecFrameInterval; 
    kkint32               threeSecFrameInterval;
    kkint32               fiveSecFrameInterval;
    kkint32               tenSecFrameInterval;
    kkint32               voltTempReadInterval;   /**< Number of frames to read before reading voltage and temp again;
                                                   *   computed so that 5 sec's between readings.
                                                   */

    bool                  embeddedFlowMeter;      /**< Indicates if FlowMeter counter will be communicated through 1st 4 pixels in scan line. */
    bool                  generateFlowRateReport; /**< If true will create a FlowRate report in C:\Temp\ */

    // Variables needed to support AutoGain.
    float                 agAnalogGain;
    float                 analogGainMin;
    float                 analogGainMax;
    kkint32               agDigitalGain;
    kkint32               digitalGainMax;
    kkint32               gainTapSel;  /**<  Currently selected Gain tap as reported by Pleora s/w */
  };

  typedef  CameraAcquisitionPleora::CameraAcquisitionPleoraPtr  CameraAcquisitionPleoraPtr;
}  /* LarcosCounterUnManaged */

#endif

