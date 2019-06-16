using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO;
using System.Text;
using System.Windows.Forms;


using CounterControls;
using CounterManaged;


namespace CounterApplication
{
  public partial class  CameraLineDisplay : Form
  {
    bool                   autoRefresh      = false;
    CounterManaged.CounterManagerWrapper cameraManager    = null;
    private  byte[]        cameraData       = null;
    private  byte[]        cameraDataPrev1  = null;
    private  byte[]        cameraDataPrev2  = null;
    private  int           firstCameraCol   = 0;
    private  int           horzCoarsness    = 2;
    private  int           oldScallingRatio = 0;
    private  float         vertCoarsness    = 2.0f;
    
    private  bool          weAreWaitingForCameraLineData = false;
  
    private  bool          errorLastTimeTryingToRetrieveData = false;

    private  Bitmap        chartBuff   = null;
    private  Graphics      chartBuffDC = null;

    private  decimal       numSampleLines = 3;



    public  CameraLineDisplay (CounterManaged.CounterManagerWrapper _cameraManager)
    {
      cameraManager = _cameraManager;

      InitializeComponent ();

      RequestedAnalogGain.Value  = (Decimal)_cameraManager.RequestedAnalogGain ();
      RequestedDigitalGain.Value = _cameraManager.RequestedDigitalGain ();

      ReFreshCameraData ();
      
      oldScallingRatio = ScallingRatio ();
      firstCameraCol   = 0;

      numSampleLines = 3;
      NumSampleLines.Value = (Decimal)numSampleLines;

      ScalingRatio.Value = 8;
      oldScallingRatio = 8;
      
      refreshButton.Enabled = true;
    }



    private  int  ScallingRatio ()
    {
      return (int)ScalingRatio.Value;
    }  /* ScallingRatio */
    
    

    private  void  PaintImage ()
    {
      if  (errorLastTimeTryingToRetrieveData)
      {
        PaintErrorRetreivingCameraLineData ();
        return;
      }
        
      if  ((cameraData == null)  &&  weAreWaitingForCameraLineData)
      {
        PaintWaitingForCameraLineData ();
        return;        
      }
    
      if  (cameraData == null)
        return;

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
        
        g.FillRectangle           (b, imageCol, startImageRow, horzCoarsness, (1 + endImageRow - startImageRow));
        chartBuffDC.FillRectangle (b, imageCol, startImageRow, horzCoarsness, (1 + endImageRow - startImageRow));
        
        cameraCol += scallingRatio;
        imageCol  += horzCoarsness;
      }

      pictureScrollBar.Maximum = dataToUseSmoothed.Length;

      pictureScrollBar.LargeChange = availLogicalCols * scallingRatio;
      pictureScrollBar.SmallChange = scallingRatio;
      pictureScrollBar.Value = firstCameraCol;
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

      byte[]  r = new byte[A.Length];
      for  (x = 0;  x < r.Length;  ++x)
        r[x] = 0;

      for  (y = 0;  y < A.Length;  ++y)
      {
        int sI = Math.Max (0, y - windowSize);
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
    }  /* SmoothArray */


    byte  FindMediumPixelValue (byte[] A)
    {
      if  (A.Length < 1)
        return 0;
      byte[]  sortedA = new byte[A.Length];
      for  (int x = 0;  x < A.Length;  ++x)
        sortedA[x] = A[x];

      Array.Sort (sortedA);

      return  sortedA[sortedA.Length / 2];
    } /* FindMediumPixelValue */



    private  void  AutoSetCropSettings ()
    {
      // Logic will be simple.
      // 1) Smooth camera data using medium filter with currently selected window size.
      // 2) Mind medium value.
      // 3) Select the 1st columns going from left and right edges that are greater that 50% of medium value.

      cameraData = cameraManager.CameraHighPointsFromLastNSampleLines ((int)numSampleLines);

      byte[]  cameraDataSmoothed = MediumSmoothArray (cameraData, Math.Max (1, (int)MediumSmootingWindow.Value));
      byte  medium = FindMediumPixelValue (cameraDataSmoothed);

      byte  threshold = (byte)(medium / 2);

      int  cropLeft = 0;
      int  cropLeftMax = cameraDataSmoothed.Length * 2 / 3;
      while   ((cameraDataSmoothed[cropLeft] < threshold) &&  (cropLeft < cropLeftMax))
        ++cropLeft;

      int  cropRight = cameraDataSmoothed.Length - 1;
      int  cropRightMin = cropLeft + 1;

      while   ((cameraDataSmoothed[cropRight] < threshold) &&  (cropRight >  cropRightMin))
        --cropRight;

      cameraManager.CropSettingsChanged (cropLeft, cropRight);

      PaintImage ();
    }  /* AutoSetCropSettings */




    private void exitButton_Click (object sender, EventArgs e)
    {
      autoRefreshTimer.Enabled = false;
      Close ();
    }


    private void refreshButton_Click (object sender, EventArgs e)
    {
      ReFreshCameraData ();
    }


    private void cameraLineWindow_Resize (object sender, EventArgs e)
    {
      PaintImage ();
    }


    private void pictureScrollBar_ValueChanged (object sender, EventArgs e)
    {
      firstCameraCol = pictureScrollBar.Value;
      PaintImage ();
    }



    private void CameraLineDisplay_SizeChanged (object sender, EventArgs e)
    {
      int newTop = Height - BottomControlsPannel.Height - 41;
      BottomControlsPannel.Top = newTop;

      int  newWidth = Width - 23;
      int  changeInWidth = newWidth - BottomControlsPannel.Width;
      BottomControlsPannel.Width = newWidth;

      exitButton.Left        = exitButton.Left        + changeInWidth;
      AutoRefreshButton.Left = AutoRefreshButton.Left + changeInWidth;
      refreshButton.Left     = refreshButton.Left     + changeInWidth;
      
      // ScrollBar
      pictureScrollBar.Width = Width - 23;
      newTop =  BottomControlsPannel.Top - pictureScrollBar.Height - 20;
      pictureScrollBar.Top = newTop;

      // cameraLineWindow
      cameraLineWindow.Width  = Width - 23;
      cameraLineWindow.Height = pictureScrollBar.Top - cameraLineWindow.Top;
      
      PaintImage ();
    }  /* CameraLineDisplay_SizeChanged */




    private  void  PaintWaitingForCameraLineData ()
    {
      Graphics dc = cameraLineWindow.CreateGraphics ();
      Font  font = new Font ("Arial", 24);
      Brush  b = new SolidBrush (Color.Red);
      
      string  msg = "Waiting For Camera Line Data";
      Size  spaceNeeded = TextRenderer.MeasureText (msg, font);
      
      int  h = cameraLineWindow.Height;
      int  w = cameraLineWindow.Width;
      
      int  row = (h - spaceNeeded.Height) / 2;
      int  col = (w - spaceNeeded.Width)  / 2;
      
      dc.Clear (Color.Black);
      TextRenderer.DrawText (dc, msg, font, new Point (col, row), Color.Red, Color.Black);
    }  /* PaintWaitingForCameraLineData */
 


    private  void  PaintErrorRetreivingCameraLineData ()
    {
      Graphics dc = cameraLineWindow.CreateGraphics ();
      Font  font = new Font ("Arial", 24);
      Brush  b = new SolidBrush (Color.Red);
      
      string  msg = "Error Retrieving Camera Line Data";
      Size  spaceNeeded = TextRenderer.MeasureText (msg, font);
      
      int  h = cameraLineWindow.Height;
      int  w = cameraLineWindow.Width;
      
      
      int  row = (h - spaceNeeded.Height) / 2;
      int  col = (w - spaceNeeded.Width)  / 2;
      
      dc.Clear (Color.Black);
      TextRenderer.DrawText (dc, msg, font, new Point (col, row), Color.Red, Color.Black);
    }  /* PaintErrorRetreivingCameraLineData   */



    private void  SaveImageInLogDirectory ()
    {
      if  (chartBuff != null)
      {
        DateTime  d = DateTime.Now;
        string logFileDir = CounterManaged.UmiOSservices.AddSlash (CounterManaged.UmiVariables.LoggingDir ()) + "CameraLines";
        string logFileName = CounterManaged.UmiOSservices.AddSlash (logFileDir) + "CameraLine_" + d.ToString ("yyyy-MM-dd_HHmmss") + ".jpg";
        CounterManaged.UmiOSservices.CreateDirectory (logFileDir);
        chartBuff.Save (logFileName);
      }
    }  /* SaveImageInLogDirectory */



    private void cameraLineWindow_Paint (object sender, PaintEventArgs e)
    {
      PaintImage ();
    }

    
    
    private void  ReFreshCameraData ()
    {
      cameraDataPrev2 = cameraDataPrev1;
      cameraDataPrev1 = cameraData;
      cameraData = cameraManager.CameraHighPointsFromLastNSampleLines ((int)numSampleLines);
      PaintImage ();
      SaveImageInLogDirectory ();
      refreshButton.Enabled = true;
    }



    private void refreshButton_Click_1 (object sender, EventArgs e)
    {
      ReFreshCameraData ();
    }



    private void AutoRefreshButton_Click (object sender, EventArgs e)
    {
      autoRefresh = !autoRefresh;
      if  (!autoRefresh)
      {
        AutoRefreshButton.Text = "Auto Refresh";
        autoRefreshTimer.Enabled = false;
      }
      else
      {
        AutoRefreshButton.Text = "Disable Auto Refresh";
        autoRefreshTimer.Enabled = true;
        autoRefreshTimer.Interval = 1000;
      }
    }



    private void autoRefreshTimer_Tick (object sender, EventArgs e)
    {
      cameraDataPrev2 = cameraDataPrev1;
      cameraDataPrev1 = cameraData;
      cameraData = cameraManager.CameraHighPointsFromLastNSampleLines ((int)numSampleLines);
      PaintImage ();
      //SaveImageInLogDirectory ();
      refreshButton.Enabled = true;
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
      AutoSetCropSettings ();
    }

    private void exitButton_Click_1 (object sender, EventArgs e)
    {
      Close ();
    }

    private void pictureScrollBar_ValueChanged_1 (object sender, EventArgs e)
    {
      PaintImage ();
    }
  }
}