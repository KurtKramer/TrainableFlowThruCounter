using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
//using UnManagedInterface;
using CounterManaged;


namespace CounterControls
{
  public partial class CameraLine : UserControl
  {
    bool                         autoRefresh      = false;
    CounterManagerWrapper  cameraManager    = null;
    private  byte[]              cameraData       = null;
    private  byte[]              cameraDataPrev1  = null;
    private  byte[]              cameraDataPrev2  = null;
    private  bool                embeddedFlowMeter = false;
    private  int                 firstCameraCol   = 0;
    private  int                 horzCoarsness    = 2;
    private  int                 oldScallingRatio = 0;
    private  float               vertCoarsness    = 2.0f;
    
    private  Bitmap              chartBuff   = null;
    private  Graphics            chartBuffDC = null;

    private  decimal             numSampleLines = 3;

    //private  UmiRunLog           runLog = null;


    public  CameraLine ()
    {
      //runLog = new UmiRunLog ();

      //runLog.WriteLn (10, "CameraLine");
      //Load += new System.EventHandler (this.OnLoad);
      InitializeComponent ();

      SizeChanged += new System.EventHandler (this.OnSizeChanged);
    }



    public String RequestedSensitivityMode
    {
      get  {return (String)RequestedSensitivityModeCB.SelectedItem;}
      set  {RequestedSensitivityModeCB.Text = value;}
    }
    

    public  float  GetRequestedAnalogGain ()
    {
      return  (float)RequestedAnalogGain.Value;
    }


    public  int  GetRequestedDigitalGain ()
    {
      return  (int)RequestedDigitalGain.Value;
    }


    public  void  SetCameraManager (CounterManagerWrapper  _cameraManager)
    {
      cameraManager = _cameraManager;
      //runLog = cameraManager.GetUmiRunLogInstance ();
      embeddedFlowMeter = cameraManager.EmbeddedFlowMeter ();

      decimal  rag = (Decimal)cameraManager.RequestedAnalogGain ();
      if  (rag < RequestedAnalogGain.Minimum)
        rag = RequestedAnalogGain.Minimum;

      else if (rag > RequestedAnalogGain.Maximum)
        rag = RequestedAnalogGain.Maximum;

      RequestedAnalogGain.Value       = rag;
      RequestedDigitalGain.Value      = (Decimal)Math.Max (100, cameraManager.RequestedDigitalGain ());
      RequestedSensitivityModeCB.Text = cameraManager.RequestedSensitivityMode ();

      ReFreshCameraData ();
      
      oldScallingRatio = ScallingRatio ();
      firstCameraCol   = 0;

      numSampleLines = 3;
      NumSampleLines.Value = (Decimal)numSampleLines;

      ScalingRatio.Value = 8;
      oldScallingRatio = 8;
      
      //refreshButton.Enabled = true;
    }



    private  int  ScallingRatio ()
    {
      return (int)ScalingRatio.Value;
    }  /* ScallingRatio */
    


    private  void  PaintNoCameraDataAvailable ()
    {
      Graphics dc = cameraLineWindow.CreateGraphics ();
      Font  font = new Font ("Arial", 24);
      Brush  b = new SolidBrush (Color.Red);
      
      string  msg = "NO Camera Data Available";
      Size  spaceNeeded = TextRenderer.MeasureText (msg, font);
      
      int  h = cameraLineWindow.Height;
      int  w = cameraLineWindow.Width;
      
      int  row = (h - spaceNeeded.Height) / 2;
      int  col = (w - spaceNeeded.Width)  / 2;
      
      dc.Clear (Color.Black);
      TextRenderer.DrawText (dc, msg, font, new Point (col, row), Color.Red, Color.Black);
    }  /* PaintWaitingForCameraLineData */

    

    private  void  PaintImage ()
    {
      if  (cameraData == null)
      {
        PaintNoCameraDataAvailable ();
        return;
      }

      SuspendLayout ();

      firstCameraCol = pictureScrollBar.Value;

      byte[] dataToUseSmoothed = cameraData;
      byte[] cameraDataPrev1Smoothed = cameraDataPrev1;
      if  (MediumSmootingWindow.Value > 0)
      {
        dataToUseSmoothed       = MediumSmoothArray (cameraData,      (int)MediumSmootingWindow.Value);
        cameraDataPrev1Smoothed = MediumSmoothArray (cameraDataPrev1, (int)MediumSmootingWindow.Value);
      }

      int  cropLeftCol  = cameraManager.CropLeft  ();
      int  cropRightCol = cameraManager.CropRight ();

      int  scallingRatio = ScallingRatio ();
      int  scanLineWidth = cameraData.Length;
      
      // firstCameraCol = 0;
      if  (firstCameraCol < 0)
        firstCameraCol = 0;

      firstCameraCol = ((int)(firstCameraCol / scallingRatio)) * scallingRatio;

      if  (chartBuff != null)
      {
        chartBuffDC.Dispose ();
        chartBuff.Dispose ();
      }

      chartBuff = new Bitmap (cameraLineWindow.Width, cameraLineWindow.Height);
      chartBuffDC = Graphics.FromImage (chartBuff);

      int  height = cameraLineWindow.Height - 4;
      int  width  = cameraLineWindow.Width  - 4;
      int  topRow = 2;
      int  botRow = topRow + height - 1;
      
      int  leftCol = 2;
      int  rightCol = leftCol + width - 1;
      
      int  numCameraCols = dataToUseSmoothed.Length;

      int  numPointsToDisplay = scanLineWidth / scallingRatio;
      horzCoarsness = Math.Max (2, (int)(0.5f + (float)(width) / (float)numPointsToDisplay));
      
      //horzCoarsness = (int)(0.5f + (float)(scallingRatio * width) / (float)numCameraCols);
      //horzCoarsness = Math.Max (2, horzCoarsness);
      int numDisplableCameraCols = scallingRatio * width / horzCoarsness;

      vertCoarsness = (float)height / 256.0f;
      if  (vertCoarsness < 1.0f)
        vertCoarsness = 1.0f;
        
      Graphics  g = cameraLineWindow.CreateGraphics ();
      g.Clear (cameraLineWindow.BackColor);
      chartBuffDC.Clear (cameraLineWindow.BackColor);
     
      int  availLogicalCols = width / horzCoarsness;
      int  availLogicalRows = (int)Math.Floor((float)height / vertCoarsness);

      int  numCameraColsThatCouldFit = Math.Min (availLogicalCols * scallingRatio, numCameraCols);
      numCameraColsThatCouldFit = ((int)(numCameraColsThatCouldFit / scallingRatio)) * scallingRatio;

      int  numCamColsLeftToDisplay = (numCameraCols - firstCameraCol);
      if  (numCamColsLeftToDisplay < numCameraColsThatCouldFit)
        firstCameraCol = Math.Max (0, numCameraCols - numCameraColsThatCouldFit);

      int lastCameraCol = firstCameraCol + availLogicalCols * scallingRatio;
      if  (lastCameraCol >= numCameraCols)
        lastCameraCol = numCameraCols - 1;

      int  totalCameraColsDisplayed = lastCameraCol - firstCameraCol;
      int  cropLeftDisplayCol = -1;
      int  cropRightDisplayCol = -1;

      if  ((cropLeftCol >= firstCameraCol)  &&  (cropLeftCol <= lastCameraCol))
        cropLeftDisplayCol  = (int)(leftCol + (float)horzCoarsness  * ((float)(cropLeftCol - firstCameraCol)) / (float)scallingRatio);

      if  ((cropRightCol >= firstCameraCol)  &&  (cropRightCol <= lastCameraCol))
        cropRightDisplayCol = (int)(leftCol + (float)horzCoarsness * ((float)(cropRightCol - firstCameraCol)) / (float)scallingRatio);

      {
        // Draw Axis's
        Pen p = new Pen (Color.GreenYellow, 1);
        
        int row255 = botRow - (int)(0.5f + 255.0f * vertCoarsness);
        
        g.DrawLine (p,  leftCol,  botRow,  rightCol,  botRow);
        g.DrawLine (p,  leftCol,  row255,  leftCol,   botRow);
        g.DrawLine (p,  rightCol, row255,  rightCol,  botRow);
        g.DrawLine (p,  leftCol,  row255,  rightCol,  row255);

        chartBuffDC.DrawLine (p,  leftCol,  botRow,  rightCol,  botRow);
        chartBuffDC.DrawLine (p,  leftCol,  row255,  leftCol,   botRow);
        chartBuffDC.DrawLine (p,  rightCol, row255,  rightCol,  botRow);
        chartBuffDC.DrawLine (p,  leftCol,  row255,  rightCol,  row255);

        p.Color = Color.GreenYellow;

        int row100 = botRow - (int)(0.5f + 100.0f * vertCoarsness);
        int row200 = botRow - (int)(0.5f + 200.0f * vertCoarsness);
        
        g.DrawLine (p, leftCol, row100, rightCol, row100);
        g.DrawLine (p, leftCol, row200, rightCol, row200);

        chartBuffDC.DrawLine (p, leftCol, row100, rightCol, row100);
        chartBuffDC.DrawLine (p, leftCol, row200, rightCol, row200);

        Font f = AnalogGainLabel.Font;
        p.Color = Color.Cyan;
        Brush cropBrush = new SolidBrush  (Color.Cyan);

        if  (cropLeftDisplayCol > 0)
        {
          g.DrawLine           (p, cropLeftDisplayCol, row255, cropLeftDisplayCol, botRow);
          chartBuffDC.DrawLine (p, cropLeftDisplayCol, row255, cropLeftDisplayCol, botRow);
          g.DrawString (cropLeftCol.ToString (), f, cropBrush, cropLeftDisplayCol + 2, row255 + 2);
        }

        if  (cropRightDisplayCol > 0)
        {
          String cropRightColStr = cropRightCol.ToString ();
          int  cropRightColStrLen = (int)(g.MeasureString (cropRightColStr, f).Width + 2.0f);
          g.DrawLine           (p, cropRightDisplayCol, row255, cropRightDisplayCol, botRow);
          chartBuffDC.DrawLine (p, cropRightDisplayCol, row255, cropRightDisplayCol, botRow);
          g.DrawString (cropRightCol.ToString (), f, cropBrush, cropRightDisplayCol - cropRightColStrLen, row255 + 2);
        }
      }

      Brush b  = new SolidBrush (Color.Red);
      Brush b2 = new SolidBrush (Color.Blue);

      int  logicalCol = 0;
      int  imageCol = leftCol;
      int  cameraCol = firstCameraCol;
      for  (logicalCol = 0;  logicalCol < availLogicalCols;  logicalCol++)
      {
        if  (cameraCol > lastCameraCol)
          break;

        int x = 0;
        int minCameraValue = 99999;
        int maxCameraValue = 0;

        int minCameraValuePrev1 = 99999;
        int maxCameraValuePrev1 = 0;
        
        for (x = 0; x < scallingRatio; x++)
        {
          int colToCheck = cameraCol + x;
          if  (colToCheck < dataToUseSmoothed.Length)
          {
            int cameraValue = dataToUseSmoothed[cameraCol + x];
            if (cameraValue < minCameraValue)
              minCameraValue = cameraValue;

            if (cameraValue > maxCameraValue)
              maxCameraValue = cameraValue;

            if  (cameraDataPrev1Smoothed != null)
            {
              cameraValue = cameraDataPrev1Smoothed[cameraCol + x];
              if (cameraValue < minCameraValuePrev1)
                minCameraValuePrev1 = cameraValue;

              if (cameraValue > maxCameraValuePrev1)
                maxCameraValuePrev1 = cameraValue;
            }
          }
        }

        int endImageRow   = botRow - (int)(0.5f + (float)minCameraValue * vertCoarsness);
        int startImageRow = botRow - (int)(0.5f + (float)maxCameraValue * vertCoarsness);

        int endImageRowPrev1   = botRow - (int)(0.5f + (float)minCameraValuePrev1 * vertCoarsness);
        int startImageRowPrev1 = botRow - (int)(0.5f + (float)maxCameraValuePrev1 * vertCoarsness);

        if  (cameraDataPrev1Smoothed != null)
        {
          g.FillRectangle           (b2, imageCol, startImageRowPrev1, horzCoarsness, (1 + endImageRowPrev1 - startImageRowPrev1));
          chartBuffDC.FillRectangle (b2, imageCol, startImageRowPrev1, horzCoarsness, (1 + endImageRowPrev1 - startImageRowPrev1));
        }
        
        g.FillRectangle           (b, imageCol, startImageRow, horzCoarsness, (1   + endImageRow - startImageRow));
        chartBuffDC.FillRectangle (b, imageCol, startImageRow, horzCoarsness, (1 + endImageRow - startImageRow));
        
        cameraCol += scallingRatio;
        imageCol  += horzCoarsness;
      }

      pictureScrollBar.Maximum = dataToUseSmoothed.Length;

      pictureScrollBar.LargeChange = availLogicalCols * scallingRatio;
      pictureScrollBar.SmallChange = scallingRatio;
      pictureScrollBar.Value = firstCameraCol;

      ResumeLayout (false);
    }  /* PaintImage */



    /// <summary>
    /// Smooth the input array by using a sliding window
    /// </summary>
    /// <param name="A">How many cells before and after to include in window.</param>
    /// <param name="windowSize"></param>
    /// <returns></returns>
    byte[]  MediumSmoothArray (byte[] A,
                               int    windowSize
                              )
    {
      int  x  = 0;
      int  x2 = 0;
      int  y  = 0;

      int  firstAvalCol = 0;
      if  (embeddedFlowMeter)
        firstAvalCol = 4;
      
      byte[]  r = new byte[A.Length];
      for  (x = 0;  x < r.Length;  ++x)
        r[x] = 0;

      for  (y = firstAvalCol;  y < A.Length;  ++y)
      {
        int sI = Math.Max (firstAvalCol, y - windowSize);
        int eI = Math.Min (y + windowSize - 1, A.Length - 1);
        int  winSize = 1 + eI - sI;
        int  midIdx = winSize / 2;
        byte[] wa = new byte[winSize];
        for  (x = sI, x2 = 0; x <= eI;  ++x, ++x2)
          wa[x2] = A[x];
        Array.Sort (wa);
        r[y] = wa[midIdx];
      }
      return r;
    }  /* MediumSmoothArray */


    byte  FindMediumPixelValue (byte[] A)
    {
      if  (A.Length < 1)
        return 0;

      int  sortedALen = A.Length;
      int  firstCol = 0;
      if  (embeddedFlowMeter)
      {
        sortedALen = sortedALen - 4;
        firstCol = 3;
      }

      byte[]  sortedA = new byte[sortedALen];
      for  (int toIdx = 0, fromIdx = firstCol;  toIdx < sortedA.Length;  ++toIdx, ++fromIdx)
        sortedA[toIdx] = A[fromIdx];

      Array.Sort (sortedA);

      return  sortedA[sortedA.Length / 2];
    } /* FindMediumPixelValue */



    private  void  AutoSetCropSettings ()
    {
      // Logic will be simple.
      // 1) Smooth camera data using medium filter with currently selected window size.
      // 2) Mind medium value.
      // 3) Select the 1st columns coming from left and right ends  that are greater that 50% of medium value.

      if  (cameraManager == null)
        return;

      cameraData = cameraManager.CameraHighPointsFromLastNSampleLines ((int)numSampleLines);
      if  (cameraData == null)
        return;

      byte[]  cameraDataSmoothed = MediumSmoothArray (cameraData, Math.Max (1, (int)MediumSmootingWindow.Value));
      byte  medium = FindMediumPixelValue (cameraDataSmoothed);

      byte  threshold = (byte)(medium / 2);

      int  cropLeftMin = 0;
      if  (embeddedFlowMeter)
      {
        // If the Flow meter is embedded then the columns 0 thru 3 will contain Flow-Meter-Counter data.
        cropLeftMin = 4;
      }

      int middleIdx = cameraData.Length / 2;
      int cropLeft  = middleIdx;
      int cropRight = middleIdx;

      while  ((cropLeft > cropLeftMin)  &&  (cameraDataSmoothed[cropLeft] >= threshold))
        --cropLeft;
      ++cropLeft;

      while  ((cropRight < cameraDataSmoothed.Length)  &&  (cameraDataSmoothed[cropRight] >= threshold))
        ++cropRight;
      --cropRight;

      cameraManager.CropSettingsChanged (cropLeft, cropRight);

      PaintImage ();
    }  /* AutoSetCropSettings */


    //private void refreshButton_Click (object sender, EventArgs e)
    //{
    //  ReFreshCameraData ();
    //}



    private void pictureScrollBar_ValueChanged (object sender, EventArgs e)
    {
      firstCameraCol = pictureScrollBar.Value;
      PaintImage ();
    }

    
    private void CameraLineDisplay_SizeChanged (object sender, EventArgs e)
    {
      int  newCameraLineWidowHeight = Height - (pictureScrollBar.Height + BottomControlsPannel.Height + 4);
      cameraLineWindow.Height = newCameraLineWidowHeight;
      cameraLineWindow.Width  = Width - 4;

      pictureScrollBar.Top = newCameraLineWidowHeight + 2;
      pictureScrollBar.Width = Width - 4;

      BottomControlsPannel.Top = pictureScrollBar.Top + pictureScrollBar.Height + 2;
      BottomControlsPannel.Width = Width - 4;

      int  halfDW = 

      SensitivityModeLabel.Left = (int)(0.5f + (float)Width * 301.0f / 837.0f);
      RequestedSensitivityModeCB.Left = (int)(0.5f + (float)Width * 302.0f / 837.0f);

      AnalogGainLabel.Left  = SensitivityModeLabel.Left + 85;
      DigitalGainLabel.Left = AnalogGainLabel.Left      + 95;

      RequestedAnalogGain.Left = RequestedSensitivityModeCB.Left + 85;
      RequestedDigitalGain.Left = RequestedAnalogGain.Left + 95;

      AutoCropButton.Left = BottomControlsPannel.Width - (AutoCropButton.Width + AutoGainButton.Width + 20 + 15);
      AutoGainButton.Left = BottomControlsPannel.Width - (AutoGainButton.Width + 15);

      //PaintImage ();
    }  /* CameraLineDisplay_SizeChanged */



    private void  SaveImageInLogDirectory ()
    {
      if  (chartBuff != null)
      {
        DateTime  d = DateTime.Now;
        string logFileDir = UmiOSservices.AddSlash (UmiVariables.LoggingDir ()) + "CameraLines";
        string logFileName = UmiOSservices.AddSlash (logFileDir) + "CameraLine_" + d.ToString ("yyyy-MM-dd_HHmmss") + ".jpg";
        UmiOSservices.CreateDirectory (logFileDir);
        chartBuff.Save (logFileName);
      }
    }  /* SaveImageInLogDirectory */


   
    
    private void  ReFreshCameraData ()
    {
      cameraDataPrev2 = cameraDataPrev1;
      cameraDataPrev1 = cameraData;
      if  (cameraManager == null)
        cameraData = null;
      else
        cameraData = cameraManager.CameraHighPointsFromLastNSampleLines ((int)numSampleLines);
      PaintImage ();
      //SaveImageInLogDirectory ();
      //refreshButton.Enabled = true;
    }



//    private void refreshButton_Click_1 (object sender, EventArgs e)
//    {
//      ReFreshCameraData ();
//    }



    private void AutoRefreshStart ()
    {
      //runLog.WriteLn (20, "AutoRefreshStart");
      autoRefreshTimer.Enabled = true;
      autoRefreshTimer.Interval = 1000;
    }



    private void  AutoRefreshStop ()
    {
      //runLog.WriteLn (20, "AutoRefreshStop");
      autoRefreshTimer.Enabled = false;
    }



//    private void AutoRefreshButton_Click (object sender, EventArgs e)
//    {
//      autoRefresh = !autoRefresh;
//      if  (!autoRefresh)
//        AutoRefreshStop ();
//      else
//        AutoRefreshStart ();
//    }

    bool  autoGainWasRunning = false;

    private void autoRefreshTimer_Tick (object sender, EventArgs e)
    {
      if  (cameraManager == null)
        return;

      if  (Visible)
      {
        cameraDataPrev2 = cameraDataPrev1;
        cameraDataPrev1 = cameraData;
        if  (cameraManager == null)
          cameraData = null;
        else
          cameraData = cameraManager.CameraHighPointsFromLastNSampleLines ((int)numSampleLines);
        if  (cameraData != null)
        {
          PaintImage ();
          //SaveImageInLogDirectory ();
          //refreshButton.Enabled = true;
        }

        bool  autoGainRunning = false;
        float  analogGain = 0.0f;
        Int32  digitalGain = 1000;
        cameraManager.GetGainSettings (ref autoGainRunning, ref analogGain, ref digitalGain);
        AutoGainButton.Enabled = !autoGainRunning;

        if  ((analogGain < (float)RequestedAnalogGain.Minimum)  ||  (analogGain > (float)RequestedAnalogGain.Maximum))
        {
          RequestedAnalogGain.Visible = false;
        }
        else
        {
          RequestedAnalogGain.Visible = true;
          RequestedAnalogGain.Value  = (decimal)analogGain;
        }
        RequestedDigitalGain.Value = (decimal)digitalGain;
        
        if  (autoGainRunning)
        {
          autoGainWasRunning = true;
        }
        else if  (autoGainWasRunning)
        {
          autoGainWasRunning = false;
        }
      }
    }



    private void CameraLineDisplay_FormClosing (object sender, FormClosingEventArgs e)
    {
      autoRefreshTimer.Enabled = false;
    }



    private void RequestedAnalogGain_ValueChanged (object sender, EventArgs e)
    {
      cameraManager.RequestedAnalogGain ((float)RequestedAnalogGain.Value);
    }



    private void RequestedDigitalGain_ValueChanged (object sender, EventArgs e)
    {
      cameraManager.RequestedDigitalGain ((Int32)RequestedDigitalGain.Value);
    }



    private void NumSampleLines_ValueChanged (object sender, EventArgs e)
    {
      numSampleLines = NumSampleLines.Value;
    }



    private void CameraLineDisplay_Load (object sender, EventArgs e)
    {

    }


    private void ScalingRatio_ValueChanged (object sender, EventArgs e)
    {
      int  newScallingRatio = ScallingRatio ();
      if  (newScallingRatio != oldScallingRatio)
      {
        PaintImage ();
        oldScallingRatio = newScallingRatio;
      }
    }


    private void AutoCropButton_Click (object sender, EventArgs e)
    {
      //runLog.WriteLn (10, "AutoCropButton_Click");
      AutoSetCropSettings ();
    }
    
    
    private void pictureScrollBar_ValueChanged_1 (object sender, EventArgs e)
    {
      PaintImage ();
    }


    protected override void OnVisibleChanged(System.EventArgs e)
    {
      if  (Visible)
      {
        if  (!autoRefresh)
          AutoRefreshStart ();
        ReFreshCameraData ();
      }
      else
      {
        autoRefreshTimer.Enabled = false;
        AutoRefreshStop ();
      }
      base.OnVisibleChanged (e);
    }



    protected override void OnPaint (PaintEventArgs e)
    {
      //PaintImage ();
    }



    private void OnSizeChanged (object sender, EventArgs e)
    {
      //runLog.WriteLn (40, "OnSizeChanged    *** Entering ***");
      // Place code that is to be executed when ever this control is resized.

      CameraLineDisplay_SizeChanged (sender, e);
 
      //runLog.WriteLn (40, "OnSizeChanged    *** Exiting ***");
    }


    private  void  SetVisibilityOfAdvancedControls (bool  v)
    {
      ScalingRatio.Enabled               = v;
      ScalingRatio.Visible               = v;
      NumSampleLines.Enabled             = v;
      NumSampleLines.Visible             = v;
      MediumSmootingWindow.Enabled       = v;
      MediumSmootingWindow.Visible       = v;
      RequestedSensitivityModeCB.Enabled = v;
      RequestedSensitivityModeCB.Visible = v;
      RequestedAnalogGain.Enabled        = v;
      RequestedAnalogGain.Visible        = v;
      RequestedDigitalGain.Enabled       = v;
      RequestedDigitalGain.Visible       = v;
      ScalingRationLabel.Visible         = v;
      NumSampleLinesLabel.Visible        = v;
      MediumSmootingWindowLabel.Visible  = v;
      SensitivityModeLabel.Visible       = v;
      AnalogGainLabel.Visible            = v;
      DigitalGainLabel.Visible           = v;
    }


    private void CameraLine_Load (object sender, EventArgs e)
    {
      CameraLineDisplay_SizeChanged (sender, e);

      if  ((cameraManager == null)  ||  (cameraManager.OperatingMode () == UmiCounterOperatingModes.Advanced))
        SetVisibilityOfAdvancedControls (true);
      else
        SetVisibilityOfAdvancedControls (false);
    }


    int  suspendLevel = 0;

    new void  SuspendLayout ()
    {
      suspendLevel++;
      if  (suspendLevel < 2)
        base.SuspendLayout ();
    }  /* SuspendLayout */


    new void  ResumeLayout ()
    {
      if  (suspendLevel <= 0)
        return;
      suspendLevel--;
      if (suspendLevel < 1)
        base.ResumeLayout ();
    }  /* ResumeLayout */



    new void ResumeLayout (bool perFormLayout)
    {
      if (suspendLevel <= 0)
        return;
      suspendLevel--;
      if (suspendLevel < 1)
        base.ResumeLayout (perFormLayout);
    }


    private void cameraLineWindow_Paint (object sender, PaintEventArgs e)
    {
      PaintImage ();
    }


    private void  AutoGainButton_Click (object sender, EventArgs e)
    {
      //runLog.WriteLn (10, "AutoGainButton_Click ");
      if  (cameraManager.CameraAutoGainThreadRunning ())
      {
        //runLog.WriteLn (10, "AutoGainButton_Click   Auto-Gain is already running.");
        MessageBox.Show (this, "Auto-Gain is already running.", "Auto-Gain", MessageBoxButtons.OK);
        return;
      }

      if  (!cameraManager.CameraThreadRunning ())
      {
        //runLog.WriteLn (10, "AutoGainButton_Click   Camera is not connected.");
        MessageBox.Show (this, "Camera is not connected.", "Auto-Gain", MessageBoxButtons.OK);
        return;
      }
      
      bool  successful = false;
      String  errMsg = "";
      cameraManager.AutoGainButtonPressed (ref successful, ref errMsg);
      if  (!successful)
      {
        MessageBox.Show (this, "Auto Gain Failed to Start" + "\n" + errMsg, "Camera Auto Gain", MessageBoxButtons.OK);
      }
      else
      {
        if  (!autoRefresh)
        {
          //AutoRefreshButton_Click (this, null);
        }
      }
    }


    private void RequestedSensitivityModeCB_SelectedIndexChanged (object sender, EventArgs e)
    {
      cameraManager.RequestedSensitivityMode (RequestedSensitivityModeCB.Text);
    }

    private void MediumSmootingWindow_ValueChanged (object sender, EventArgs e)
    {

    }

    private void CameraLine_VisibleChanged (object sender, EventArgs e)
    {
      if  (cameraManager == null)
        return;
      RequestedAnalogGain.Value = (Decimal)cameraManager.RequestedAnalogGain ();
      RequestedDigitalGain.Value = (Decimal)Math.Max (100, cameraManager.RequestedDigitalGain ());
      RequestedSensitivityModeCB.Text = cameraManager.RequestedSensitivityMode ();
    } 
  }
}
