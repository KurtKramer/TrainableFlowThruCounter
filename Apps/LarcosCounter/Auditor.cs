using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.IO;
using System.Text;
using System.Windows.Forms;
using System.Windows.Forms.DataVisualization.Charting;

using LarcosCounterManaged;
using KKManagedRoutines;


namespace LarcosCounter
{ 
  public partial class Auditor : Form
  {
    private  LarcosCounterManagerWrapper  cameraManager     = null;
    private  int                          autoScrollRate    = 0;
    private  int                          erosionStructSize = -1;
    private  int                          timerTicCount     = 0;
    private  String                       scannerFileName   = "";
    private  UmiShrimpLengthComputer      lengthComputer    = null;
    private  UmiScannerFileBuffered       scannerFile       = null;
    private  List<String>                 errorMessages     = new List<string> ();
    private  bool                         scannerFileLoadThreadsStillRunning = false;

    private  String[]                     plotOptions = {"Shrimp Count","Particle Count", "Bytes", "Flow Ratio", "Flow Rate"};
    private  int                          plottingIntervalSecs = 5;
    private  float                        scanRate = 20000.0f;

    private  int                          oldWidth  = 0;
    private  int                          oldHeight = 0;

    private  float                        pixelsPerMM = 23.6f;

    private  bool                         runPlayBack = false;

    private  String                       trainingModelName = "";
    private  bool                         trainingModelLoaded = false;

    private  int                          scannerFileCropLeft = -1;
    private  int                          scannerFileCropRight = -1;

    private  int                          backgroundPixelTH = 31;

    private  UmiOperatingParameters       opParametersToExtractParticles = null;

    
    enum   PlottingOptions  {ShrimpCount = 0, ParticleCount = 1, Bytes = 2, FlowRateRatio = 3, FlowRate = 4};

    UmiRunLog  runLog = null;


    public bool RunPlayBack  {get {return runPlayBack;}}

    
    public  Auditor (LarcosCounterManagerWrapper  _cameraManager,
                     String                       _scannerFileName,
                     UmiRunLog                    _runLog
                    )
    {
      cameraManager   = _cameraManager;
      scannerFileName = _scannerFileName;

      runLog = _runLog;
      if  (runLog == null)
        runLog = cameraManager.GetUmiRunLogInstance ();

      InitializeComponent ();
      PlotOptions.DataSource   = plotOptions;
      PlotOptions.SelectedItem = plotOptions[0];
      oldWidth  = Width;
      oldHeight = Height;

      Text = "Auditor          " + UmiOSservices.GetRootName (scannerFileName);

      InitialDataLoad ();
    }



    private  void  InitialDataLoad ()
    {
      runLog.WriteLn (20, "Auditor::InitialDataLoad");
      try
      {
        scannerFile = new UmiScannerFileBuffered (scannerFileName, runLog);
      }
      catch  (Exception)
      {
        scannerFile = null;
        errorMessages.Add ("Exception opening scanner file.");
        return;
      }

      String  trainModelNameActive      = ActiveTrainingLibraries.Model1Name;
      String  trainModelNameReportFile  = "";
      String  trainModelNameScannerFile = "";

      int  particleCropLeft = -1;
      int  particleCropRight = -1;

      UmiSessionParameters    particlesSessionParameters   = null;
      UmiOperatingParameters  particlesOperatingParameters = null;

      opParametersToExtractParticles = cameraManager.GetOperatingParameters ();

      if  (scannerFile != null)
      {
        try
        {
          // Will load header fields from Report ile/ ParticleEntries
          UmiParticleEntryList  particleEntriesHeaderFields = new UmiParticleEntryList (scannerFileName, true, runLog);
          trainModelNameReportFile       = particleEntriesHeaderFields.TrainingModelName;
          particlesSessionParameters     = particleEntriesHeaderFields.GetSessionParameters ();
          particlesOperatingParameters   = particleEntriesHeaderFields.GetOperatingParameters ();
          opParametersToExtractParticles = particleEntriesHeaderFields.GetOperatingParameters ();

          if  (particlesOperatingParameters != null)
            backgroundPixelTH = particlesOperatingParameters.BackGroundPixelTH;

          particleCropLeft = particleEntriesHeaderFields.CropLeft;
          particleCropRight = particleEntriesHeaderFields.CropRight;
        }
        catch  (Exception)
        {}

        UmiSessionParameters  scannerFileSessionParameters = new UmiSessionParameters ();
        scannerFileSessionParameters.UpdateFromScannerFile (scannerFile.ScannerFile ());
        trainModelNameScannerFile = scannerFileSessionParameters.TrainingModelName;
      }

      trainingModelName = trainModelNameReportFile;
      if  (String.IsNullOrEmpty (trainingModelName))
        trainingModelName = trainModelNameScannerFile;

      if  (String.IsNullOrEmpty (trainingModelName))
        cameraManager.GetTrainingModelName ();

      if  (String.IsNullOrEmpty (trainingModelName))
        trainingModelName = trainModelNameActive;

      if  (!String.IsNullOrEmpty (trainingModelName))
      {
        ActiveTrainingLibraries.SetModel1 (trainingModelName, false, DoneLoadingTrainModel);
      }
      else
      {
        if  (!String.IsNullOrEmpty (ActiveTrainingLibraries.Model1Name))
          ActiveTrainingLibraries.LoadModels (DoneLoadingTrainModel);
      }

      if  (scannerFile != null)
      {
        scannerFile.FrameBytesOffsetsUpdateThreadStart ();

        //  Because 'ParticleEntryDataLoadThreadStart' will startits own thread we want to giove it its own instance of a UmiRunLog.
        scannerFile.ParticleEntryDataLoadThreadStart (cameraManager.GetUmiRunLogInstance ());
        ScannerFilePanel.SetScannerFile (cameraManager, scannerFile, trainingModelName);
        ScannerFilePanel.PaintWholePanel ();
        scannerFileLoadThreadsStillRunning = true;
        scanRate = scannerFile.ScanRate;
        if  (particlesSessionParameters != null)
        {
          Description.Text = particlesSessionParameters.SessionDescription;
        }
        else
        {
          Description.Text = scannerFile.GetValue ("Description");
        }

        scannerFileCropLeft  = UmiKKStr.StrToInt (scannerFile.GetValue ("CropLeft"));
        scannerFileCropRight = UmiKKStr.StrToInt (scannerFile.GetValue ("CropRight"));

        if  ((particleCropLeft >= 0)  &&  (particleCropLeft < particleCropRight)  &&  (particleCropRight < 2048))
          ScannerFilePanel.SetCropColumns (particleCropLeft, particleCropRight);

        else if  ((scannerFileCropLeft > 0)  &&  (scannerFileCropLeft < scannerFileCropRight)  &&  (scannerFileCropRight < 2048))
          ScannerFilePanel.SetCropColumns (scannerFileCropLeft, scannerFileCropRight);
      }

      if  (cameraManager.OperatingMode () == LarcosOperatingModes.Advanced)
      {
        ValidateByClassButton.Visible = true;
        ValidateByClassButton.Enabled = true;
      }
      else
      {
        ValidateByClassButton.Visible = true;
        ValidateByClassButton.Enabled = true;
      }
    }


    private  void  DoneLoadingTrainModel (UmiTrainingModel2  loadedModel)
    {
      trainingModelLoaded = true;
      UmiTrainingModel2  tm = ActiveTrainingLibraries.Model1 ();
      if  (tm != null)
      {
        UmiOperatingParameters  opParms = tm.GetOperatingParameters ();
        if  (opParms != null)
          opParametersToExtractParticles = opParms;
      }
    }


    private  PlottingOptions  PlottingOptionFromString (String s)
    {
      for  (int x = 0;  x < plotOptions.Length;  ++x)
        if  (plotOptions[x].Equals (s))
          return (PlottingOptions)x;
      return PlottingOptions.ShrimpCount;
    }



    private  void  UpdateScrollBar ()
    {
      int   maxNumDisplayRows = 20000;
      if  (scannerFile != null)
        maxNumDisplayRows = ScannerFilePanel.ScanLineRowToDispalyRow (scannerFile.LargestKnownScanLine());
      
      ScannerFileScrollBar.Minimum = 0;
      ScannerFileScrollBar.Maximum = maxNumDisplayRows;

      int  value = (int)ScannerFilePanel.DisplayScanRowLeft ();
      if  (value > maxNumDisplayRows)
      {
        value = maxNumDisplayRows - ScannerFilePanel.DisplayRowsToDisplay ();
        if  (value < 0)
          value = 0;
      }

      ScannerFileScrollBar.Value   = value;
      ScannerFileScrollBar.LargeChange = ScannerFilePanel.DisplayRowsToDisplay ();
      ScannerFileScrollBar.SmallChange = 2;

      AuditorChartAddCurrentPosition ();
    }  /* UpdateScrollBar */




    private  String  SecsToHHMMSS (int secs)
    {
      int hh = secs / 3600;
      secs = secs - (hh * 3600);
      int  mm = secs / 60;
      int  ss = secs % 60;
      return hh.ToString ("00") + ":" + mm.ToString ("00") + ":" + ss.ToString ("00");
    }




    // While PartcleEntries and FrameOffsets are still loading we need to call this method periodily to 
    // refresh ScannerFile stats.
    private  void  DisplayScannerFileStats ()
    {
      if  (scannerFile == null)
      {
        ScannerFileLenInSecs.Text = "";
        ScannerFileLenInScanLines.Text = "0";
        ScannerFileScrollBar.Minimum = 0;
        ScannerFileScrollBar.Maximum = 100;
        ScannerFileScrollBar.Value = 0;
      }
      else
      {
        int  scannerFileLenInSecs = (int)((float)scannerFile.LargestKnownScanLine () / scanRate);
        ScannerFileLenInSecs.Text = SecsToHHMMSS (scannerFileLenInSecs);

        int  numScanLines = scannerFile.LargestKnownScanLine ();
        ScannerFileLenInScanLines.Text = numScanLines.ToString ("###,###,##0");

        ScannerFileScrollBar.Minimum = 0;
        ScannerFileScrollBar.Maximum = numScanLines;
        ScannerFileScrollBar.Value = ScannerFilePanel.ScanLineLeft ();
      }
    }  /* DisplayScannerFileStats */





    int  auditorChartStripLineCurPosIdx = -1;

    private  void  AuditorChartAddCurrentPosition ()
    {
      double  offsetInSecs = ((float)(ScannerFilePanel.ScanLineLeft ()) / scanRate);

      if (auditorChartStripLineCurPosIdx < 0)
      {
        StripLine  sl = new StripLine ();
        sl.BorderColor = Color.Black;
        sl.Interval = 1000000;
        sl.IntervalOffset = offsetInSecs;
        sl.IntervalType = DateTimeIntervalType.Seconds;
        sl.StripWidth = 0.0f;
        sl.BorderWidth = 2;
        sl.StripWidthType = DateTimeIntervalType.Seconds;
        auditorChartStripLineCurPosIdx = ScannerFileChart.ChartAreas[0].AxisX.StripLines.Count;
        ScannerFileChart.ChartAreas[0].AxisX.StripLines.Add (sl);
      }
      else
      {
        ScannerFileChart.ChartAreas[0].AxisX.StripLines[auditorChartStripLineCurPosIdx].IntervalOffset = offsetInSecs;
      }
    }


    private  void  AuditorChartAddStartStopPoints ()
    {
      runLog.WriteLn (40, "Auditor::AuditorChartAddStartStopPoints");
      ScannerFileChart.ChartAreas[0].AxisX.StripLines.Clear ();
      auditorChartStripLineCurPosIdx = -1;

      UmiStartStopPointList startStopPoints = scannerFile.StartStopPoints ();
      foreach  (UmiStartStopPoint  p  in  startStopPoints)
      {
        StripLine  sl = new StripLine ();
        if  (p.Type == UmiStartStopPoint.StartStopType.sspStartPoint)
          sl.BorderColor = Color.Green;
        else
          sl.BorderColor = Color.Red;

        double  offsetInSecs = ((float)p.ScanLineNum / scanRate);
        sl.Interval = 1000000;
        sl.IntervalOffset = offsetInSecs;
        sl.IntervalType   = DateTimeIntervalType.Seconds;
        sl.StripWidth     = 0.0f;
        sl.BorderWidth    = 3;
        sl.StripWidthType = DateTimeIntervalType.Seconds;
        ScannerFileChart.ChartAreas[0].AxisX.StripLines.Add (sl);
      }

      AuditorChartAddCurrentPosition ();
    }  /* AuditorChartAddStartStopPoints */



    private  void  PopulateAuditorChart ()
    {
      runLog.WriteLn (40, "Auditor::PopulateAuditorChart   :" + (String)PlotOptions.SelectedItem);
      
      ScannerFileChart.Series[0].Points.Clear ();
      if  (scannerFile == null)
        return;

      PlottingOptions  plotOptionSel = PlottingOptionFromString ((String)PlotOptions.SelectedItem);

      float[]  plotHist = null;

      switch  (plotOptionSel)
      {
        case PlottingOptions.ShrimpCount:
          if  (scannerFile.ParticleEntries () != null)
            plotHist = scannerFile.ParticleEntries ().CountFrequencyByTimeIntervals (plottingIntervalSecs, scanRate);
          break;

        case PlottingOptions.ParticleCount:
          if  (scannerFile.ParticleEntries () != null)
            plotHist = scannerFile.ParticleEntries ().ParticleFrequencyByTimeIntervals (plottingIntervalSecs, scanRate);
          break;

        case PlottingOptions.Bytes:
            plotHist = scannerFile.RecordRateByTimeIntervals (plottingIntervalSecs);
          break;

        case PlottingOptions.FlowRateRatio:
            plotHist = scannerFile.ParticleEntries ().FlowRateRatioByTimeIntervals (plottingIntervalSecs, scanRate);
          break;

        case PlottingOptions.FlowRate:
            plotHist = scannerFile.ParticleEntries ().FlowRateByTimeIntervals (plottingIntervalSecs, scanRate);
          break;
      }

      int  timeOffset = 0;

      PopulateCount ();

      if  (plotHist != null)
      {
        for (int x = 0; x < plotHist.Length; ++x)
        {
          DateTime d = new DateTime (2000, 1, 1);
          d = d.AddSeconds (timeOffset);
          ScannerFileChart.Series[0].Points.AddXY (d, plotHist[x]);
          timeOffset += plottingIntervalSecs;
        }
      }

      ScannerFileChart.ChartAreas[0].RecalculateAxesScale ();
    }  /* PopulateAuditorChart */



    private  void  PopulateCount ()
    {
      if  ((scannerFile != null)  &&  (scannerFile.ParticleEntries () != null))
        Count.Text = scannerFile.ParticleEntries ().SubjectCount (scannerFile).ToString ("#,###,##0");
      else
        Count.Text = "0";
    }
    

    private  void  AutoScrollingRateTextSet ()
    {
      AutoScrollingRate.Text = autoScrollRate.ToString ();
      if  (autoScrollRate >= 0)
        AutoScrollingRate.ForeColor = Color.Blue;
      else
        AutoScrollingRate.ForeColor = Color.Red;
    }  /* AutoScrollingRateTextSet */



    private  void  AutoScrollScannerData ()
    {
      if  (autoScrollRate == 0)
        return;

      int  newTopDisplayRow = (int)(ScannerFilePanel.DisplayScanRowLeft () + autoScrollRate);
      if  (newTopDisplayRow < 0)
      {
        autoScrollRate = 0;
        newTopDisplayRow = 0;
        AutoScrollingRateTextSet ();
      }

      ScannerFilePanel.ScrollToPropperPlaceInScannerFile (newTopDisplayRow);
      AuditorChartAddCurrentPosition ();
    }  /* AutoScrollScannerData */



    private  void  UpdateScreenForNewSize ()
    {
      if  ((this.Height < 10)  ||  (this.Width < 10))
        return;
    
      int  deltaWidth  = Width  - oldWidth;
      int  deltaHeight = Height - oldHeight;
    
      oldWidth  = Width;
      oldHeight = Height;

      ScannerFileChart.Width      += deltaWidth;
      ScannerFilePanel.Width      += deltaWidth;
      ScannerFilePanel.Height     += deltaHeight;
      ScannerFileScrollBar.Top    += deltaHeight;
      ScannerFileScrollBar.Width  += deltaWidth;
      ScannerDisplayControls.Top  += deltaHeight;
      ScannerDisplayControls.Left += (deltaWidth / 2);
      RunRecountButton.Top        += deltaHeight;
      ValidateByClassButton.Top   += deltaHeight;
      ValidateByClassButton.Left  += deltaWidth;
      GenerateReportButton.Top    += deltaHeight;
      GenerateReportButton.Left   += deltaWidth;
      UpdateScrollBar ();
    }  /* UpdateScreenForNewSize */


    
    private void  AuditorTimer_Tick (object sender, EventArgs e)
    {
      AutoScrollScannerData ();
      if  (((timerTicCount % 10) == 0)  &&  scannerFileLoadThreadsStillRunning)
      {
        DisplayScannerFileStats ();
        PopulateAuditorChart ();
        AuditorChartAddStartStopPoints ();
        UpdateScrollBar ();
        if  (!scannerFile.LoadThreadsRunning)
        {
          scannerFileLoadThreadsStillRunning = false;

          if  (scannerFile.ParticleEntries ()  != null)
          {
            float  widthInMeters = cameraManager.ImagingChamberWidth ();
            if  (widthInMeters == 0.0f)
              widthInMeters = 2.5f * 2.54f / 100.0f;   // (2.5 inches) * (2.54 cm's/inch)  /  (100 cm's/m)

            int  pixelsPerScanLine = cameraManager.ImagingChamberWidthPixels ();
            pixelsPerMM = (float)pixelsPerScanLine /  (1000.0f  * widthInMeters);
            erosionStructSize = scannerFile.ParticleEntries ().ErosionStructSize;
            lengthComputer = new UmiShrimpLengthComputer (pixelsPerScanLine, cameraManager.ImagingChamberWidth (),  erosionStructSize, backgroundPixelTH);
          }
        }
      }

      ++timerTicCount;
    }


    private void Auditor_SizeChanged (object sender, EventArgs e)
    {
      if  (WindowState != FormWindowState.Minimized)
        UpdateScreenForNewSize ();
    }



    private void ScannerFileScrollBar_Scroll (object sender, ScrollEventArgs e)
    {
      autoScrollRate = 0;
      AutoScrollingRateTextSet ();
      ScannerFilePanel.ScrollToPropperPlaceInScannerFile (ScannerFileScrollBar.Value);
      AuditorChartAddCurrentPosition ();
    }


    private void FastBackwardButon_Click (object sender, EventArgs e)
    {
      int  magnitude = Math.Abs (autoScrollRate);
      if  (magnitude > 400)
          autoScrollRate -= 100;
      else if  (magnitude > 200)
          autoScrollRate -= 50;
      else if  (magnitude > 50)
          autoScrollRate -= 25;
      else
          autoScrollRate -= 10;
      AutoScrollingRateTextSet ();
    }



    private void FastForwardButton_Click (object sender, EventArgs e)
    {
      int  magnitude = Math.Abs (autoScrollRate);
      if  (magnitude >= 400)
          autoScrollRate += 100;
      else if  (magnitude >= 200)
          autoScrollRate += 50;
      else if  (magnitude >= 50)
          autoScrollRate += 25;
      else
          autoScrollRate += 10;
      AutoScrollingRateTextSet ();
    }



    private void StopAutoScrol_Click (object sender, EventArgs e)
    {
      autoScrollRate = 0;
      AutoScrollingRateTextSet ();
    }



    private void SkipToPrevButton_Click (object sender, EventArgs e)
    {
      if (scannerFile == null)
        return;
      ScannerFilePanel.SkipToPrevStartStopPoint ();
      UpdateScrollBar ();
    }



    private void  SkipToNextButton_Click (object sender, EventArgs e)
    {
      if  (scannerFile == null)
        return;
      ScannerFilePanel.SkipToNextStartStopPoint ();
      UpdateScrollBar ();
    }

   
    private void Colorize_CheckedChanged (object sender, EventArgs e)
    {
      ScannerFilePanel.Colorize = Colorize.Checked;
      ScannerFilePanel.PaintWholePanel ();
    }


    private void Auditor_FormClosing (object sender, FormClosingEventArgs e)
    {
      if  (scannerFile != null)
        scannerFile.FrameBytesOffsetsUpdateThreadTerminate ();
      AuditorTimer.Enabled = false;
    }


    private void RunRecountButton_Click (object sender, EventArgs e)
    {
      runPlayBack = true;
      Close ();
    }


    private void GenerateReportButton_Click (object sender, EventArgs e)
    {
      FinaleReport  fr = new FinaleReport (cameraManager, scannerFileName, true, runLog);
      fr.ShowDialog (this);
    }



    private void ScannerFileChart_MouseClick (object sender, MouseEventArgs e)
    {
      Point  p = e.Location;
      ChartArea  ca = ScannerFileChart.ChartAreas[0];
      double xValue = ca.AxisX.PixelPositionToValue ((double)p.X);

      double  offSetInSecs = DateTime.FromOADate (xValue).TimeOfDay.TotalSeconds;

      int  scanLineNum = (int)(0.5f + offSetInSecs * scanRate);

      autoScrollRate = 0;
      AutoScrollingRateTextSet ();

      int  displayRow = (int)(0.5f + (float)scanLineNum / scannerFile.Ratio);
      if  (displayRow > ScannerFileScrollBar.Maximum)
        displayRow = ScannerFileScrollBar.Maximum;

      ScannerFileScrollBar.Value = displayRow;
      ScannerFilePanel.ScrollToPropperPlaceInScannerFile (displayRow);
      AuditorChartAddCurrentPosition ();
    }


    private  void  ScannerFilePanel_StartStopPointChange (int scanLine)
    {
      AuditorChartAddStartStopPoints ();
      PopulateCount ();
    }

    private void ScannerFilePanel_CropSettingsChanged ()
    {
      UpdateScrollBar ();
      AuditorChartAddStartStopPoints ();
    }



    private void PlotOptions_SelectedIndexChanged (object sender, EventArgs e)
    {
      PopulateAuditorChart ();
    }



    private void Auditor_Load (object sender, EventArgs e)
    {

    }



    private void ScannerFilePanel_Load (object sender, EventArgs e)
    {

    }



    private  void  ScannerFilePanel_ParticleSelected (object sender, 
                                                      LarcosControls.ScannerFileViewerPanel.ParticleSelectedEventArgs e
                                                     )
    {
      if  (!trainingModelLoaded)
      {
        MessageBox.Show (this, "Training Model not loaded yet.", "", MessageBoxButtons.OK);
        return;
      }

      UmiParticleEntryList  particles = scannerFile.ParticleEntries ();

      UmiParticleEntry pe = e.ParticleEntry;

      String  fileName = pe.FileName;
      UmiRaster r = scannerFile.GetRasterForParticleAsCounted (pe, opParametersToExtractParticles);
      if  (r != null)
      {
        ImageVerifier  imageVerifier = new ImageVerifier (cameraManager, pe, r, fileName, pixelsPerMM, runLog);
        imageVerifier.LengthComputer (this.lengthComputer);
        imageVerifier.ShowDialog (this);

        if  (imageVerifier.OkPressed  &&  (imageVerifier.ClassUserValidatesAs != null))
        {
          pe.PredClass = imageVerifier.ClassUserValidatesAs;
          scannerFile.ParticleEntries ().AddUpdateToReportFile (pe);
          PopulateCount ();
          //ScannerFilePanel.PaintParticle (pe);
          ScannerFilePanel.PaintWholePanel ();
        }
      }
      return;
    }


    private void ValidateByClassButton_Click (object sender, EventArgs e)
    {
      if  (!trainingModelLoaded)
      {
        MessageBox.Show (this, "Training Model not loaded yet.", "", MessageBoxButtons.OK);
        return;
      }

      AuditorByClass  abc = new AuditorByClass (cameraManager, scannerFile, runLog);
      abc.ShowDialog (this);
      scannerFile.ParticleEntries ().SortByScanLine ();
      PopulateCount ();
      ScannerFilePanel.PaintWholePanel ();
    }

  }
}
