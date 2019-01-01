/* CameraAcquisitionPleoraDalsa.h -- Represents a single camera frame.
 * Copyright (C) 2011-2013  Kurt Kramer
 * For conditions of distribution and use, see copyright notice in LarcosCounterUnManaged.txt
 */
#if  !defined(_CAMERAACQUISITIONPLEORADALSA_)
#define  _CAMERAACQUISITIONPLEORADALSA_


#include "CameraAcquisitionPleora.h"


namespace LarcosCounterUnManaged
{
  /**
   *@class  CameraAcquisitionPleoraDalsa
   *@brief  Specific implementation of a GIGE device for DALSA cameras using the Pleora interface.
   *@details  Inspiration comes from 'PvPipelineSample' from 'Pleora Technologies Inc.'
   */
  class CameraAcquisitionPleoraDalsa: public  CameraAcquisitionPleora
  {
  public:
    typedef  CameraAcquisitionPleoraDalsa*  CameraAcquisitionPleoraDalsaPtr;


    CameraAcquisitionPleoraDalsa (LarcosCounterManagerPtr _manager,
                                  const KKStr&            _macAddress,
                                  CameraFrameBufferPtr    _frameBuffer,
                                  FlowMeterTrackerPtr     _flowMeter,
                                  MsgQueuePtr             _msgQueue,
                                  const KKStr&            _threadName
                                 );

    CameraAcquisitionPleoraDalsa (LarcosCounterManagerPtr _manager,
                                  CameraParametersPtr     _cameraParams,
                                  CameraFrameBufferPtr    _frameBuffer,
                                  FlowMeterTrackerPtr     _flowMeter,
                                  MsgQueuePtr             _msgQueue,
                                  const KKStr&            _threadName
                                 );



    virtual
      ~CameraAcquisitionPleoraDalsa ();

    virtual  void  Run ();

    virtual  void  ResetCounts ();

    /**
     * @brief  Executes the "Auto-Gain" function for "Pleora" based cameras.
     */
    virtual  void  PerformAutoGainProcedureOld ();
    virtual  void  PerformAutoGainProcedure ();


  protected:
    static
    const char*  PvGenTypeToCStr (PvGenType  genType);
    
    void  AnalogGainBinarySearch (uint16 threshholdSaturationValue);

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


    void  ConnectToCamera (bool&  connectionSuccessful);
    void  DisconnectFromCamera ();

    static
    double  GetPvParameterFloat (PvGenParameterArray*  params,
                                 const KKStr&          paramName,
                                 double                defaultValue  /**< Value if parameter not available. */
                                );

    static
    int32  GetPvParameterEnum (PvGenParameterArray*  params,
                               const KKStr&          paramName,
                               int32                 defaultValue
                              );

    static
    KKStr  GetPvParameterEnumStr (PvGenParameterArray*  params,
                                  const KKStr&          paramName,
                                  const KKStr&          defaultValue
                                 );

    static
    int64  GetPvParameterInteger (PvGenParameterArray*  params,
                                  const KKStr&          paramName,
                                  int64                 defaultValue
                                 );


    static
    void   CameraParametersPopulate (PvGenParameterArray*  params,
                                     CameraParameters&     cameraParameters,
                                     RunLog&               runLog
                                    );

    /**********************************************/
    void  RaiseAnalogGainUntilSaturated2 (float  increments);

    void  SetAnalogGainToFirstHighInRange (float  analogGainStart, 
                                           float  analogGainEnd, 
                                           float  increment
                                          );

    void  RaiseDigitalGainUntilSaturated (int32  increments);

    void  RaiseDigitalGainUntilAtLeastHighValue (int  minHighValue);

    int32                 gainTapSel;  /**<  Currently selected Gain tap as reported by Pleora s/w */

    PvDevice*             lDevice;
    PvPipeline*           lPipeline;
    PvStreamPtr           lStream;
    PvGenParameterArray*  lDeviceParams;   /**< Parameters of currently selected camera.  */
  
    //  Stream Parameters
    PvGenParameterArray*  lStreamParams;
    PvGenFloat*           lFrameRate;
    PvGenInteger*         lCameraTemparature;

    //  Device Parameters
    PvGenInteger*         lIPAddressParam;
    PvGenInteger*         lTLLocked;
    PvGenInteger*         lPayloadSize;
    PvGenCommand*         lStart;
    PvGenCommand*         lStop;
    PvGenFloat*           lScanRate;
    PvGenEnum*            lGainSelector;
    PvGenFloat*           lAnalogGainAbs;
    PvGenInteger*         lDigitalGainRaw;

    PvGenCommand*         lReadVoltageAndTemperature;
    PvGenEnum*            lSensitivityMode;

    // For Other Camera
    PvGenInteger*         lGain;
    PvGenInteger*         lAcquisitionLinePeriod;
    PvGenInteger*         lExposureTime;

    int32                 oneSecFrameInterval;    /**< Number of frames per second. */ 
    int32                 twoSecFrameInterval; 
    int32                 threeSecFrameInterval;
    int32                 fiveSecFrameInterval;
    int32                 tenSecFrameInterval;
    int32                 voltTempReadInterval;   /**< Number of frames to read before reading voltage and temp again;
                                                   *   computed so that 5 secs between readings.
                                                   */

    bool                  embeddedFlowMeter;      /**< Indicates if FlowMeter counter will be communicated through 1st 4 pixels in scan line. */
    bool                  generateFlowRateReport; /**< If true will create a FlowRate report in C:\Temp\ */

    // Variables needed to support AutoGain.
    float                 agAnalogGain;
    float                 analogGainMin;
    float                 analogGainMax;
    int32                 agDigitalGain;
    int32                 digitalGainMax;
  };

  typedef  CameraAcquisitionPleoraDalsa::CameraAcquisitionPleoraDalsaPtr  CameraAcquisitionPleoraDalsaPtr;
}  /* LarcosCounterUnManaged */

#endif

