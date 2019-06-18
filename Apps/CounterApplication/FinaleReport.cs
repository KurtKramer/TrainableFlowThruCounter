using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Drawing.Printing;
using System.Windows.Forms.DataVisualization.Charting;
using System.IO;

//using Microsoft.Reporting.WinForms;

using CounterManaged;
using CounterManagedRoutines;


namespace CounterApplication
{
  public partial class FinaleReport : Form
  {
    private  UmiParticleEntryList         particles       = null;
    private  UmiRunLog                    runLog          = null;
    private  string                       scannerFileName = "";
    private  UmiScannerFile               scannerFile     = null;

    private  UmiSessionParameters         sessionParameters   = new UmiSessionParameters ();
    private  UmiOperatingParameters       operatingParameters = new UmiOperatingParameters ();

    private  string    sessionDescription = "";
    private  string    controlNum         = "";
    private  float     scanRate           = 20000.0f;
    private  DateTime  dateTimeRecorded;
    private  int       totalCount         = 0;
    private  int       totalRecordTime    = 0;
    private  string    trainingModelName  = "";

    private  int       sampleSize  = 0;
    private  float     sizeMean    = 0.0f;
    private  float     sizeStdDev  = 0.0f;


    private  string    configFileName     = "";
    private  bool      formIsMaximized    = false;

    private  int  heightLast = 0;
    private  int  widthLast  = 0;

    private  string  lastDirectorySavedTo = "";

    private  bool    autoSaveReport = false;

    private  List<int>     sizeHistogram  = null;
    private  List<float>   countHistogram = null;

    private  CounterManagerWrapper  cameraManager = null;

    private  FinaleReportDataSource  reportDataSource = null;

    private  bool  saveDebugImages = false;


    
    public  UmiOperatingParameters  OperatingParameters  {get {return operatingParameters;}}

    public  Chart   SizeChart  {get  {return SizeDistributionChart;}}



    public FinaleReport (CounterManagerWrapper  _cameraManager,
                         string                 _scannerFileName,
                         bool                   _autoSaveReport,
                         bool                   _saveDebugImages,
                         UmiRunLog              _runLog
                        )
    {
      cameraManager = _cameraManager;
      saveDebugImages = _saveDebugImages;
      runLog = _runLog;
      if  (runLog == null)
        runLog = new UmiRunLog ();

      scannerFileName = _scannerFileName;

      autoSaveReport = _autoSaveReport;
      configFileName = UmiOSservices.AddSlash (UmiVariables.ConfigurationDir ()) + "FinaleReport.cfg";

      InitializeComponent ();
    }


    private void FinaleReport_Load (object sender, EventArgs e)
    {
      heightLast = Height;
      widthLast  = Width;

      LoadConfigurationFile ();

      string  scannerFileComments = "";

      sessionDescription  = "";
      controlNum          = "";
      trainingModelName   = "";
      scanRate            = 0.0f;
      dateTimeRecorded    = DateTime.MinValue;

      scannerFile = UmiScannerFile.CreateScannerFile (scannerFileName, runLog);
      if  (scannerFile != null)
      {
        sessionParameters.UpdateFromScannerFile (scannerFile);
        operatingParameters.UpdateFromScannerFile (scannerFile);
        bool  successful = false;
        scannerFile.LoadIndexFile (ref successful, runLog);
      }

      try  {
        {
          particles = new UmiParticleEntryList (scannerFileName, 
                                                false,               /**< false = go ahead and load the whole report file. */
                                                runLog
                                               );

          //foreach  (UmiParticleEntry pe in particles)
          //{
          //  if  (pe.Length > 0.0f)
          //  {
          //    //pe.Length = pe.Length * 0.0635f / 0.0889f;
          //    //pe.Length = pe.Length * 0.055947137f / 0.068595602f;  // AM1
          //    //pe.Length = pe.Length * 0.055947137f / 0.048696319f;    // AM2
          //  }
          //}
        }
      }
      catch (Exception)  {particles = null;}
      if  (particles != null)
      {
        DateTime  origRecordDateTime = sessionParameters.DateTimeRecorded;
        UmiSessionParameters    particlesSessionParameters   = particles.GetSessionParameters ();
        UmiOperatingParameters  particlesOperatingParameters = particles.GetOperatingParameters ();

        if  (particlesSessionParameters != null)
          sessionParameters.Assign (particlesSessionParameters);
      
        if  (particlesOperatingParameters != null)
          operatingParameters.Assign (particlesOperatingParameters);

        sessionParameters.DateTimeRecorded = origRecordDateTime;
      }


      sessionDescription      = sessionParameters.SessionDescription;
      controlNum              = sessionParameters.ControlNum;
      trainingModelName       = sessionParameters.TrainingModelName;
      TrainingModelName2.Text = trainingModelName;
      scanRate                = operatingParameters.RequestedScanRate;
      dateTimeRecorded        = sessionParameters.DateTimeRecorded;
      scannerFileComments     = sessionParameters.Comments;
      personInCharge.Text     = sessionParameters.PersonInCharge;
      source.Text             = sessionParameters.Source;
      destination.Text        = sessionParameters.Destination;

      if  ((scannerFile != null)  &&  (particles != null))
      {
        UmiStartStopRegionList  regions = scannerFile.StartStopRegions ();
        if  (regions != null)
          particles = particles.GetParticlesByStartStopRegions (regions);
      }

      ControlNumField.Text = controlNum;
      SessionDescription2.Text = sessionDescription;
      comments.Text = scannerFileComments;
      recordDateTime.Text = dateTimeRecorded.ToString ("yyyy-MMM-dd HH:mm:ss");

      PopulateCountsChart ();
      PopulateSizeHistogramChart ();

      BuildReportDataSource ();

      if  (autoSaveReport)
        AutoSaveReport ();
    }



    private  void  SaveConfiguration ()
    {
      System.IO.StreamWriter  o = null;
      try{o = new System.IO.StreamWriter (configFileName);}  catch  (Exception){o = null; return;}
      
      o.WriteLine ("//FinaleReport Configuration File");
      o.WriteLine ("//");
      o.WriteLine ("//DateTime Saved" + "\t" + DateTime.Now.ToString ("F"));
      o.WriteLine ("//");
      o.WriteLine ("WidthLast"            + "\t" + widthLast);
      o.WriteLine ("HeightLast"           + "\t" + heightLast);
      o.WriteLine ("Maximized"            + "\t" + (formIsMaximized ? "YES":"NO"));

      o.WriteLine ("TimeIntervals"        + "\t" + TimeIntervals.Value.ToString ());
      o.WriteLine ("SizeInterval"         + "\t" + SizeInterval.Value.ToString ());

      o.WriteLine ("LastDirectorySavedTo" + "\t" + lastDirectorySavedTo);

      o.Close ();
      o = null;
    }



    private  void   LoadConfigurationFile ()
    {
      System.IO.StreamReader i = null;

      try {i = new System.IO.StreamReader (configFileName);}  catch  (Exception) {i = null;}
      if  (i == null)
        return;

      int  savedWidth  = 0;
      int  savedHeight = 0;
      bool screenWasMaximized = false;

      string  nextLine = null;

      while  (true)
      {
        try  {nextLine = i.ReadLine ();}  catch (Exception) {break;}
        if  (nextLine == null)
          break;

        nextLine = nextLine.Trim ();
        
        if  ((nextLine.Length < 3)  ||  (nextLine.Substring (0, 2) == "//"))
          continue;

        string[] fields = nextLine.Split ('\t');
        if  (fields.Length < 2)
          continue;

        string  fieldName  = fields[0];
        string  fieldValue = fields[1];

        switch  (fieldName)
        { 
          case  "WidthLast":
            savedWidth = UmiKKStr.StrToInt (fieldValue);
            break;

          case  "HeightLast":
            savedHeight = UmiKKStr.StrToInt (fieldValue);
            break;

          case  "LastDirectorySavedTo":
            lastDirectorySavedTo = fieldValue;
            break;

          case  "Maximized":
            screenWasMaximized  = (fieldValue.ToUpper () == "YES");
            break;

          case  "TimeIntervals":
            TimeIntervals.Value = (decimal)UmiKKStr.StrToFloat (fieldValue);
            break;

          case  "SizeInterval":
            SizeInterval.Value = (decimal)UmiKKStr.StrToFloat (fieldValue);
            break;
        }
      }

      i.Close ();

      if  (savedWidth > Screen.PrimaryScreen.Bounds.Width)
        savedWidth = Screen.PrimaryScreen.Bounds.Width;

      if  (savedHeight > Screen.PrimaryScreen.Bounds.Height)
        savedHeight = Screen.PrimaryScreen.Bounds.Height;

      if  (screenWasMaximized)
      {
        //savedWidth = Screen.PrimaryScreen.Bounds.Width;
        //savedHeight = savedHeight = Screen.PrimaryScreen.Bounds.Height;
        this.WindowState = FormWindowState.Maximized; 
      }
      else
      {
        Height = Math.Max (savedHeight, MinimumSize.Height);
        Width  = Math.Max (savedWidth,  MinimumSize.Width);
      }

      OnResizeEnd (new EventArgs ());
    }  /* LoadConfigurationFile */




    private  List<int>  BuildSizeHistogram (ref int    sampleSize,
                                            ref float  mean,
                                            ref float  stdDev
                                           )
    {
      sampleSize = 0;
      mean = 0.0f;
      stdDev = 0.0f;

      if  (particles == null)
        return  null;

      float  sum = 0.0f;
      float  sumSquare = 0.0f;

      float  sizeInterval = (float)SizeInterval.Value;
      if  (sizeInterval <= 0.0f)
        sizeInterval = 1.0f;

      List<int> histogram = new List<int> ();

      foreach  (UmiParticleEntry pe in particles)
      {
        if  ((pe.Length > 0.0f)  &&  (pe.CountFactor == 1.0f))
        {
          ++sampleSize;
          sum += pe.Length;
          sumSquare += pe.Length * pe.Length;

          int  idx = (int)(pe.Length / sizeInterval);
          while  (histogram.Count <= idx)
            histogram.Add (0);
          histogram[idx] += 1;
        }
      }

      if  (sampleSize > 0)
      {
        if  (sampleSize == 1)
        {
          mean = sum;
          stdDev = 0.0f;
        }
        else
        {
          mean = sum / sampleSize;
          float  variance = (sumSquare - (sum * sum) / sampleSize) / (sampleSize - 1);
          stdDev = (float)Math.Sqrt (variance);
        }
      }

      return  histogram;
    }  /* BuildSizeHistogram */



    private  void  PopulateSizeHistogramChart ()
    {
      sampleSize = 0;
      sizeMean   = 0.0f;
      sizeStdDev = 0.0f;

      sizeHistogram = BuildSizeHistogram (ref sampleSize, ref sizeMean, ref sizeStdDev);
      if  ((sizeHistogram == null)  ||  (sizeHistogram.Count < 1))
        return;

      if  (reportDataSource != null)
      {
        reportDataSource.SampleSize = sampleSize;
        reportDataSource.SizeMean   = sizeMean;
        reportDataSource.SizeStdDev = sizeStdDev;
      }

      SampleSize.Text = sampleSize.ToString ("###,##0");
      SizeMean.Text   = sizeMean.ToString ("##,##0.00") + "mm";
      SizeStdDev.Text = sizeStdDev.ToString ("##,##0.00") + "mm";
      
      float  sizeInterval = (float)SizeInterval.Value;

      SizeDistributionChart.Titles.Clear ();
      SizeDistributionChart.Titles.Add ("Size Distribution");
      //SizeDistributionChart.Titles.Add (controlNum + "  " + sessionDescription + "   " +  "Date: " + dateTimeRecorded.ToString ("yyyy-MMM-dd hh:mm:ss"));

      ChartArea ca = SizeDistributionChart.ChartAreas[0];
      ca.AxisY.Minimum = 0;
      ca.AxisY.Title = "Count";
      ca.AxisX.Minimum = 0;
      ca.AxisX.Title = "Size (mm)";
      ca.AxisY.Minimum = 0;

      SizeDistributionChart.Series[0].Points.Clear ();

      for  (int idx = 0;  idx < sizeHistogram.Count;  ++idx)
      {
        DataPoint dp = new DataPoint (idx, sizeHistogram[idx]);
        string spLabel = "";
        if  (sizeInterval == 1.0f)
        {
          spLabel = (idx * sizeInterval).ToString ();
        }
        else
        {
          float  start = (float)idx * sizeInterval;
          float  end   = (float)(idx + 1) * sizeInterval;
          spLabel = start.ToString () + "≤X<" + end.ToString ();
        }

        dp.AxisLabel = spLabel;
        SizeDistributionChart.Series[0].Points.Add (dp);
      }

      SizeDistributionChart.ChartAreas[0].RecalculateAxesScale ();
    }




    private  List<float>   BuildCountHistogram ()
    {
      totalCount = 0;
      if  (particles == null)
        return  null;
      float  timeInterval = (float)TimeIntervals.Value;
      List<float>  histogram = new List<float> ();
      foreach  (UmiParticleEntry pe in particles)
      {
        float  deltaTime = pe.ScannerRowOverAll / scanRate;

        int  timeIDX = (int)(deltaTime / timeInterval);
        while  (histogram.Count <= timeIDX)
          histogram.Add (0);

        histogram[timeIDX] += pe.PredClass.CountFactor;
      }

      foreach (int zed in histogram)
        totalCount += zed;

      return  histogram;
    }  /* BuildCountHistogram */



    private  List<float>   BuildNonShrimpCount ()
    {
      totalCount = 0;
      if  (particles == null)
        return  null;
      float  timeInterval = (float)TimeIntervals.Value;
      List<float>  histogram = new List<float> ();
      foreach  (UmiParticleEntry pe in particles)
      {
        if  (pe.PredClass.CountFactor == 0.0)
        {
          float  deltaTime = pe.ScannerRowOverAll / scanRate;

          int  timeIDX = (int)(deltaTime / timeInterval);
          while  (histogram.Count <= timeIDX)
            histogram.Add (0);
          histogram[timeIDX] += 1.0f;
        }
      }

      return  histogram;
    }  /* BuildNonShrimpCount */



    private  string  SecondsToTimeStr (int  secs)
    {
      int  hours = secs / 3600;
      secs = secs % 3600;

      int  mins = secs / 60;
      secs = secs % 60;

      return  hours.ToString ("00") + ":" + mins.ToString ("00") + ":" + secs.ToString ("00");
    }


    private  void  PopulateCountsChart ()
    {
      float  timeInterval = (float)TimeIntervals.Value;
      countHistogram = BuildCountHistogram ();

      TotalCount.Text = totalCount.ToString ("###,##0");

      CountsChart.Titles.Clear ();
      CountsChart.Titles.Add ("Count Distribution by Time");
      //CountsChart.Titles.Add (controlNum + "  " + sessionDescription + "   " +  "Date: " + dateTimeRecorded.ToString ("yyyy-MMM-dd hh:mm:ss"));

      ChartArea ca = SizeDistributionChart.ChartAreas[0];
      ca.AxisY.Minimum = 0;
      ca.AxisY.Title = "Count";
      ca.AxisX.Minimum = 0;
      ca.AxisX.Title = "Time (sec's)";
      ca.AxisY.Minimum = 0;

      CountsChart.Series[0].Points.Clear ();

      for  (int idx = 0;  idx < countHistogram.Count;  ++idx)
      {
        DataPoint dp = new DataPoint (idx, countHistogram[idx]);
        double  secs = idx * timeInterval;
        dp.AxisLabel = SecondsToTimeStr ((int)secs);
        CountsChart.Series[0].Points.Add (dp);
      }

      totalRecordTime = (int)(0.5f + (float)countHistogram.Count * timeInterval);

      CountsChart.ChartAreas[0].RecalculateAxesScale ();
    }  /* PopulateCountsChart */ 
    


    private void FinaleReport_Resize (object sender, EventArgs e)
    {
      int  heightDelta = Height - heightLast;
      int  widthDelta  = Width  - widthLast;

      int  heightDeltaTop = heightDelta / 2;
      int  heightDeltaBot = heightDelta - heightDeltaTop;

      ChartTabs.Height += heightDelta;
      ChartTabs.Width  += widthDelta;

      SizeDistributionChart.Height += heightDelta;
      SizeDistributionChart.Width  += widthDelta;

      CountsChart.Height += heightDelta;
      CountsChart.Width  += widthDelta;

      PrintButton.Left += widthDelta;

      heightLast = Height;
      widthLast  = Width;
    }



    private void FinaleReport_SizeChanged (object sender, EventArgs e)
    {
      if  (WindowState == FormWindowState.Maximized)
      {
        // Looks like user has pressed the Maximized button.  We have to trap it here because
        // the ResizeEnd event does not trap when form is Maximized.
        //sCommanderFormResized ();
        FinaleReport_Resize (sender, e);
        formIsMaximized = true;
      }

      else if  (WindowState == FormWindowState.Normal)
      {
        if  (formIsMaximized)
        {
          // We normally trap the ResizeEnd event; but when the form was already maximized and the user
          // presses the button to un-maximize. the ResizeEnd does not trap that.  So we check to 
          // see if the form was already maximize. If so then we resized the form.
          //CommanderFormResized ();
          FinaleReport_Resize (sender, e);
          formIsMaximized = false;
        }
      }
    }



    private  void   BuildReportDataSource ()
    {
      MemoryStream  sizeDistributionChartMemory = new MemoryStream ();
      SizeDistributionChart.SaveImage (sizeDistributionChartMemory, ChartImageFormat.Png);

      MemoryStream  temporalChartStream = new MemoryStream ();
      CountsChart.SaveImage (temporalChartStream, ChartImageFormat.Png);

      reportDataSource = new FinaleReportDataSource 
        (cameraManager, 
         sessionParameters, 
         operatingParameters, 
         totalCount, 
         sampleSize,
         sizeMean,
         sizeStdDev,
         sizeDistributionChartMemory.ToArray (), 
         temporalChartStream.ToArray ()
        );
    }



    private  void  AutoSaveReport ()
    {
      string  reportDir = UmiOSservices.AddSlash (UmiVariables.ReportDir ()) + "SessionReports";
      UmiOSservices.CreateDirectoryPath (reportDir);

      int  seqNum = 0;
      bool  uniqueNameFound = false;
      string  reportName = "";
      while  (!uniqueNameFound)
      {
        reportName = UmiOSservices.AddSlash (reportDir) + UmiOSservices.GetRootName (scannerFileName) + "_" + seqNum.ToString ("000") + ".pdf";
        uniqueNameFound = !UmiOSservices.FileExists (reportName);
        ++seqNum;
      }

      SaveRepordAsPDF (reportName, false);
    }  /* AutoSaveReport */



    private  void  SaveRepordAsPDF (string  fileName,
                                    bool    aknowledgeSuccessfulSave
                                   )
    {
      byte[]  renderedReport = null;

      try
      {
        Microsoft.Reporting.WinForms.ReportDataSource reportDataSource1 = new Microsoft.Reporting.WinForms.ReportDataSource ();
        Microsoft.Reporting.WinForms.ReportDataSource reportDataSource2 = new Microsoft.Reporting.WinForms.ReportDataSource ();

        System.Windows.Forms.BindingSource FinaleReportDataSourceBindingSource = new System.Windows.Forms.BindingSource ();
        System.Windows.Forms.BindingSource chartsBindingSource = new System.Windows.Forms.BindingSource ();

        Microsoft.Reporting.WinForms.ReportViewer reportViewer1 = new Microsoft.Reporting.WinForms.ReportViewer ();

        ((System.ComponentModel.ISupportInitialize)(FinaleReportDataSourceBindingSource)).BeginInit ();
        ((System.ComponentModel.ISupportInitialize)(chartsBindingSource)).BeginInit ();

        SuspendLayout ();

        FinaleReportDataSourceBindingSource.DataSource = typeof (global::CounterApplication.FinaleReportDataSource);
        chartsBindingSource.DataMember = "Charts";
        chartsBindingSource.DataSource = FinaleReportDataSourceBindingSource;

        reportViewer1.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
        reportDataSource1.Name  = "FinaleReportDataSource";
        reportDataSource1.Value = FinaleReportDataSourceBindingSource;
        reportDataSource2.Name  = "FinaleReportChartDataSource";
        reportDataSource2.Value = chartsBindingSource;
        reportViewer1.LocalReport.DataSources.Add (reportDataSource1);
        reportViewer1.LocalReport.DataSources.Add (reportDataSource2);
        reportViewer1.LocalReport.ReportEmbeddedResource = "CounterApplication.CounterSessionReport.rdlc";
        reportViewer1.Location = new System.Drawing.Point (2, 2);
        reportViewer1.Name = "reportViewer1";
        reportViewer1.Size = new System.Drawing.Size (700, 700);
        reportViewer1.TabIndex = 0;
        reportViewer1.ZoomMode = Microsoft.Reporting.WinForms.ZoomMode.PageWidth;
        
        ((System.ComponentModel.ISupportInitialize)(FinaleReportDataSourceBindingSource)).EndInit ();
        ((System.ComponentModel.ISupportInitialize)(chartsBindingSource)).EndInit ();
        ResumeLayout (false);

        FinaleReportDataSourceBindingSource.DataSource = reportDataSource;

        List<FinaleReportChartDataSource>  chartSource = new List<FinaleReportChartDataSource> ();
        chartSource.Add (new FinaleReportChartDataSource ("Size Distribution", reportDataSource.SizeChart));
        chartSource.Add (new FinaleReportChartDataSource ("Temporal Distribution", reportDataSource.TemporalChart));
        chartsBindingSource.DataSource = chartSource;
        reportViewer1.RefreshReport ();

        string mimeType = string.Empty;
        string encoding = string.Empty;
        string fileExt  = string.Empty;
        string[] streams = null;
        Microsoft.Reporting.WinForms.Warning[] warnings = null;
        renderedReport = reportViewer1.LocalReport.Render ("PDF", null, out mimeType, out encoding, out fileExt, out streams, out warnings);
      }
      catch  (Exception e1)
      {
        MessageBox.Show (this, "Exception occurred rendering Session Report\n\n" + e1.ToString (), "Save Session Report");
        renderedReport = null;
      }

      if  (renderedReport != null)
      {
        try
        {
          FileStream  fs = new FileStream (fileName, FileMode.Create);
          fs.Write (renderedReport, 0, renderedReport.Length);
          fs.Close ();
          if  (aknowledgeSuccessfulSave)
            MessageBox.Show (this, "Session report saved successfully", "Save Session Report");
        }
        catch (Exception e2)
        {
          MessageBox.Show (this, "Exception occurred writing Session Report.\n\n" + e2.ToString (), "Save Session Report");
        }
      }

    }  /* SaveRepordAsPDF */



    private void PlotButton_Click (object sender, EventArgs e)
    {
      SaveConfiguration ();
      PopulateCountsChart ();
      PopulateSizeHistogramChart ();
      BuildReportDataSource ();
    }



    private void FinaleReport_FormClosing (object sender, FormClosingEventArgs e)
    {
      SaveConfiguration ();
    }



    private void PrintButton_Click (object sender, EventArgs e)
    {
      //PrintFinaleReport ();

      FinaleReportViewer  frv = new FinaleReportViewer (cameraManager, reportDataSource);
      frv.ShowDialog (this);
    }



    private void SavePDFButton_Click (object sender, EventArgs e)
    {
      if  (string.IsNullOrEmpty (lastDirectorySavedTo))
      {
        lastDirectorySavedTo = UmiOSservices.AddSlash (UmiVariables.ReportDir ()) + "SessionReports";
        UmiOSservices.CreateDirectoryPath (lastDirectorySavedTo);
      }

      SaveFileDialog  sfd = new SaveFileDialog ();
      sfd.CheckPathExists = true;
      sfd.Filter = "*PDF files (*.pdf)|*.pdf";
      sfd.RestoreDirectory = true;
      sfd.InitialDirectory = lastDirectorySavedTo;
      sfd.Title = "Save Session Report as PDF";
      sfd.FileName = UmiOSservices.GetRootName (scannerFileName) + ".pdf";

      DialogResult  dr = sfd.ShowDialog (this);
      if  (dr == DialogResult.OK)
      {
        lastDirectorySavedTo = UmiOSservices.GetPathPartOfFile (sfd.FileName);
        SaveRepordAsPDF (sfd.FileName, true);
      }
    }



    private void SizeDistributionChart_Click (object sender, EventArgs e)
    {

    }



    private void SizeDistributionChart_MouseClick (object sender, MouseEventArgs e)
    {
      Point  p = e.Location;
      ChartArea  ca = SizeDistributionChart.ChartAreas[0];
      double  xValue = ca.AxisX.PixelPositionToValue ((double)p.X);
      int  xIdx = (int)(xValue + 0.5);

      float  minSize = (float)(xIdx)     * (float)SizeInterval.Value;
      float  maxSize = (float)(xIdx + 1) * (float)SizeInterval.Value;
     
      UmiParticleEntryList  particlesForSizeRange = particles.GetParticlesBySizeRange (minSize, maxSize);
      if  (particlesForSizeRange != null)
      {
        UmiScannerFileBuffered  scannerFileBuffered = new UmiScannerFileBuffered (scannerFile);
        DisplayParticleImages  dpi = new DisplayParticleImages ("Size " +  minSize + " to " + maxSize,
                                                                cameraManager,
                                                                scannerFileBuffered,
                                                                particlesForSizeRange,
                                                                saveDebugImages,
                                                                runLog
                                                               );
        dpi.ShowDialog (this);

        if  (dpi.UpdatesWereMade)
        {
          PlotButton_Click (this, new EventArgs ());
        }
      }
    }

  }
}
