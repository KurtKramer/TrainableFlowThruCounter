/* CameraAutoGainThread.h --
 * Copyright (C) 2011-2013  Kurt Kramer
 * For conditions of distribution and use, see copyright notice in LarcosCounterUnManaged.txt
 */
#if  !defined(_CAMERAAUTOGAINTHREAD_)
#define  _CAMERAAUTOGAINTHREAD_

#include "GoalKeeper.h"
#include "RunLog.h"

#include "CameraThread.h"


namespace LarcosCounterUnManaged
{
  /**
   *@brief  Thread that manages the Auto Gain function.
   *@details This thread will be fired up by the "LarcosCounterManager" instance as a result of the
   * method "AutoGainButtonPressed" being called.  The idea is that when the user presses the "Auto-Gain"
   * button the GUI application calls the "AutoGainButtonPressed" method which then creates an instance
   * of this (CameraAutoGainThread) and starts it up
   *
   * The actual code for performing Auto-Gain is implemented in the "CameraAcquisition" instance that is
   * communicating with the camera. This thread will call the derived virtual method "PerformAutoGainProcedure"
   * to perform the "Auto-Gain" function.
   *

   * Upon starting this thread will manage the Gain function by making requests to adjust both "Analog"
   * gain and "Digital" gain and based off the response to these requests will keep on making appropriate
   * changes.
   *
   * Background:
   *   Camera: Dalsa SG-11-02K80
   *   Analog Gain (-10.0f thru 10.0f)
   *   Digital Gain (0 thru 5000)
   *
   *   Preference is to maximize Analog gain over Digital gain.
   *
   *  General Flow.
   *  1) Initialize Analog And Digital Gain to -8.0f  and  2000  respectively.
   *  2) a. Raise Analog gain by increments 2.0f until the camera line no longer rises.
   *     b. Select the Analog gain that 4.0f below the 1st line where it was detected that 
   *        there was no gain in the camera.  The logic here is that when no rise in gain 
   *        is detected two increments must have already occurred.  In that case we need to
   *        go back 2 increments (4.0f) to get to point below where the camera gets
   *        saturated.
   *  4) Before fine tuning the Analog gain we raise the digital gain until the camera
   *     high value is 220 out of 255.  The intuition is that this provides greater 
   *     dynamic range for the smaller increments required for the fine tuning of the
   *     analog gain.  
   *  5) Raise Analog gain by increments of 0.5f until the camera line no longer rises; 
   *     This is done the same way as step "1)" except increments of 0.5f followed by
   *     reduction by 2 * 0.5f = 1.0f.
   *  6) It is likely that the best Analog gain is some where between -0.5f and +1.0f of
   *     the current Analog gain found in step "5)" above.  We now step thru that range at 
   *     increments of 0.1f and record the high values for each increment.  The increment
   *     that has the 1st occurrence of the highest value detected is selected as the
   *     best Analog Gain.
   *  7) We now tune the Digital Gain looking for Digital Gain just before the point where 
   *     we consider the Digital Gain-Maxed.  Digital Gain-Maxed is defined as when more
   *     than 2.5% of the pixels (columns) in the scan-line are above 253.  This is done in
   *     a coarse manner first at increments of 1000 then 200 and finally 50.
   */
  class CameraAutoGainThread: public  CameraThread
  {
  public:
    CameraAutoGainThread (LarcosCounterManagerPtr _manager,
                          MsgQueuePtr             _msgQueue,
                          CameraAcquisitionPtr    _camera,
                          CameraFrameBufferPtr    _cameraFrameBuffer,
                          const KKStr&            _threadName
                         );


    ~CameraAutoGainThread ();
 

    /**
     *@brief  Returns an estimate of the amount of memory consumed in bytes.
     *@details This will help managed objects keep track of how much memory 
     * they are using in the unmanaged world.
     */
    kkMemSize  MemoryConsumedEstimated ();
 
    
    virtual  void  ResetCounts ();


    /**
     *@brief  Call this Method to start writing camera frame data to disk.  Will stay in loop until cancelFlag is true.
     */
    virtual void  Run ();


  private:
    CameraAcquisitionPtr    camera;
    CameraFrameBufferPtr    cameraFrameBuffer;
  };   /* CameraAutoGainThread */

  typedef  CameraAutoGainThread*  CameraAutoGainThreadPtr;
}


#endif
