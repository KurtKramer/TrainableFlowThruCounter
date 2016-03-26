using System;
using System.Collections;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Diagnostics;
using System.Drawing;
using System.IO;
using System.Text;
using System.Threading;
using System.Windows.Forms;
using System.Runtime.InteropServices;

using LarcosCounterManaged;
using LarcosManagedRoutines;



namespace LarcosFileViewer
{
  public partial class LarcosFileViewer : Form
  {
    private  int  zed = 0;
       
    private  Queue<String>               alarms                = new Queue<string> ();
    private  Queue<String>               alarmFileNames        = new Queue<string> ();
                              
    private  int                         autoScrollRate        = 0;
    private  int                         timerTicCount         = 0;
    
    private  UmiScannerFileBuffered      file                  = null;
    private  string                      lastFileOpenedName    = "";
    private  string                      scannerFileName       = "";
    private  LarcosFileViewerParameters  parameters            = null;

    private  ArrayList                   imageEditors          = null;
    
    private  int                         rightMargin           = 0;

    private  string                      initialFileName       = null; 
    private  string                      windowTitle           = "";    // When not empty will replace Window Title.

    private  string                      LarcosFileViewerConfigFileName = "";

    private  UmiRunLog                   runLog                = new UmiRunLog();

    private  int                         oldWidth              = 0;
    private  int                         oldHeight             = 0;

    private  bool                        saveDebugImages       = false;


    public  int  BlobMinSize  
    {
      get {return parameters.BlobMinSize ();}  
      set {parameters.BlobMinSize (value); }
    }


    public  LarcosFileViewer ()
    {
      SplashScreen sc = new SplashScreen ();
      sc.Show (this);

      LarcosFileViewerConfigFileName = UmiOSservices.AddSlash (UmiVariables.ConfigurationDir ()) + "LarcosFileViewer.txt";

      try
      {
        InitializeComponent ();
      }
      catch  (Exception e)
      {
        MessageBox.Show ("Exception running function 'InitializeComponent'");
        throw new Exception ("Excpetion calling 'InitializeComponent'", e);
      }
      

      try
      {
        Initialization ();
      }
      catch  (Exception e2)
      {
        MessageBox.Show ("Exception running function 'Initialization'");
        throw new Exception ("Excpetion calling 'Initialization'", e2);
      }
    }



    public LarcosFileViewer (string  fileName)
    {
      SplashScreen sc = new SplashScreen ();
      sc.Show (this);

      LarcosFileViewerConfigFileName = UmiOSservices.AddSlash (UmiVariables.ConfigurationDir ()) + "LarcosFileViewer.txt";
      initialFileName = fileName;
      InitializeComponent ();
      Initialization ();
    }



    public LarcosFileViewer (Stream  stream,
                             string  _windowTitle,
                             string  _scannerFileName
                            )
    {
      LarcosFileViewerConfigFileName = UmiOSservices.AddSlash (UmiVariables.ConfigurationDir ()) + "LarcosFileViewer.txt";
      windowTitle     = _windowTitle;
      scannerFileName = _scannerFileName;
      
      InitializeComponent ();
      Initialization ();
    }



    private  void  Initialization ()
    {
      ScannerPanel.MouseWheel += new System.Windows.Forms.MouseEventHandler (this.ScannerPanel_MouseWheel);

      oldWidth  = Width;
      oldHeight = Height;

      parameters = new LarcosFileViewerParameters (ScannerPanel);
      imageEditors = new ArrayList ();
      
      rightMargin = this.Width - FileName.Right;

      LoadLarcosFileViewerConfiuration ();
    }  /* Initialization */



    private  void  LoadLarcosFileViewerConfiuration ()
    {
      StreamReader f = null;

      try
      {
        f = new StreamReader (LarcosFileViewerConfigFileName);
      }
      catch (Exception)
      {
        return;
      }

      string txtLine = f.ReadLine ();
      while (txtLine != null)
      {
        if (txtLine.Substring (0, 2) != "//")
        {
          String[]  fields = txtLine.Split ('\t');
          if  (fields.Length > 1)
          {
            string   rowType = fields[0].ToLower ().Trim ();
          
            if  (rowType == "lastfileopenedname")
            {
              if  (fields.Length >= 2)
                lastFileOpenedName = fields[1];
            }

            else if  (rowType == "endoffile")
              break;
          }
        }
        
        txtLine = f.ReadLine();
      }

			if  (String.IsNullOrEmpty(initialFileName) && (!String.IsNullOrEmpty(lastFileOpenedName)))
				initialFileName = lastFileOpenedName;

      f.Close();
    }  /* LoadLarcosFileViewerConfiuration */


    
    private  void  SaveLarcosFileViewerConfiguration ()
    {
      StreamWriter w = null;
      try
      {
        w = new StreamWriter (LarcosFileViewerConfigFileName);
      }
      catch (Exception e)
      {
        MessageBox.Show ("Exception Writing File Viewer Configuration.  [" + e.ToString() + "]");
        return;
      }

      w.WriteLine ("// LarcosFileViewer Configuration");
      w.WriteLine ("// DateTime" + "\t" + DateTime.Now.ToString());
      w.WriteLine ("//");
      w.WriteLine ("//");
      w.WriteLine ("//  The next line specifies the last Scanner-File opened.");
      w.WriteLine ("LastFileOpenedName" + "\t" + lastFileOpenedName);
      w.WriteLine ("//");
      w.WriteLine ("//");

      w.WriteLine ("EndOfFile");

      w.Close();
      
      return;
    }  /* SaveConfiguration */



    private  void  SetScannerFileName (string  fileName)
    {
      SetUpDataStructuresForNewFile ();
    
      if  (!File.Exists (fileName))
      {
        MessageBox.Show ("File [" + fileName + "] does not exist");
      }
      else
      {
        scannerFileName = fileName;
        InitializeForNewDataFile (fileName);
        lastFileOpenedName = fileName;
        SaveLarcosFileViewerConfiguration ();
      }
    }  /* SetScannerFileName */




    private  void  SetUpDataStructuresForNewFile ()
    {
      DisposeExistingDataStructures ();
      parameters.SetNewDisplayRowTop (0);
      parameters.SetScannerFile (null);
      parameters.ComputeParameters ();
    
      FileName.Text = "";
      
      imageEditors = new ArrayList ();
    } /* SetUpDataStructuresForNewFile */



    
    private  void  DisposeExistingDataStructures ()
    {
      if  (imageEditors != null)
      {
        int  x;  
        for  (x = 0;  x < imageEditors.Count;  x++)
        {
          ImageEditor  ie = (ImageEditor)(imageEditors[x]);
          if  (!ie.IsDisposed)
            ie.Dispose ();
          ie = null;
          imageEditors[x] = null;
        }
        
        imageEditors = null;
      }


      if  (file != null)
      {
        file.Dispose ();
        file = null;
      }
      
    } /* DisposeExistingDataStructures */

  

    private  void  InitializeForNewDataFile (string  fileName)
    {
      try
      {
        file = new UmiScannerFileBuffered (fileName, null);
      }
      catch  (Exception e)
      {
        MessageBox.Show (e.ToString (), "Not a valid a Scanner File Format[" + fileName + "]");
        file = null;
      }
      
      if  (file != null)
      {
        file.FrameBytesOffsetsUpdateThreadStart ();
        file.ParticleEntryDataLoadThreadStart (runLog);
        FileName.Text = scannerFileName;
        parameters.SetScannerFile (file);
        parameters.PaintWholePanel ();
        UpdateStats ();
        UpdateVerticleScrollBar ();
      }
    }  /* InitializeForNewDataFile */



    private void  UpdateStats ()
    {
      if  (file == null)
        return;

      float  ratio = parameters.Ratio ();
    
      int  scanLinesPerFrame = file.ScanLinesPerFrame;

      int  scanLineTop = (int)Math.Floor ((float)parameters.DisplayRowTop () * ratio);
      int  frameNumTop = scanLineTop / scanLinesPerFrame;
      
      int  scanLineBot = (int)Math.Ceiling ((float)(parameters.DisplayRowBot () + 1) * ratio) - 1;
      int  frameNumBot = (int)Math.Ceiling ((double)scanLineBot / (double)scanLinesPerFrame);
      if  (frameNumBot > file.LargestKnowmFrameNum ())
        frameNumBot = file.LargestKnowmFrameNum ();

      ScanLinesDisplayed.Text = scanLineTop.ToString ("###,###,##0") + " - " + scanLineBot.ToString ("###,###,##0");
      
      cropColLeft.Text  = parameters.CropColemnLeft  ().ToString ("#,##0");
      cropColRight.Text = parameters.CropColemnRight ().ToString ("#,##0");

      if  (file != null)
          SanLinesTotal.Text = file.LargestKnownScanLine ().ToString ("###,###,##0");
    } /* UpdateStats */




    private  void  UpdateScreenForNewSize ()
    {
      if  (parameters == null)
        return;

      if  ((this.Height < 10)  ||  (this.Width < 10))
        return;
    
      int  changeInWidth  = Width  - oldWidth;
      int  changeInHeight = Height - oldHeight;
      int  changeInWidthHalf = (changeInWidth / 2);
    
      oldWidth  = Width;
      oldHeight = Height;
    
      int  newWidth  = this.Width;
      int  newHeight = this.Height;
      
      int  panelLeftCol = ScannerPanel.Left;
      int  panelTopRow  = ScannerPanel.Top;
      int  scrollBarWidth = vScrollBar1.Width;
      
      int  avaiableColsToWorkWith = Math.Max (100, newWidth  - (panelLeftCol + scrollBarWidth + 20));
      int  avaiableRowsToWorkWith = Math.Max (100, (newHeight - (panelTopRow + 50)));
      
      ScannerPanel.Width  = avaiableColsToWorkWith;
      ScannerPanel.Height = avaiableRowsToWorkWith;
      
      vScrollBar1.Left   = panelLeftCol + avaiableColsToWorkWith;
      vScrollBar1.Height = avaiableRowsToWorkWith;
      
      FileName.Width =  FileName.Width + changeInWidth;
      ScrollingPannel.Left += changeInWidthHalf;

      MouseCoordinates.Left      += changeInWidthHalf;
      MouseCoordinatesLabel.Left += changeInWidthHalf;

      ScanLinesDisplayed.Left      += changeInWidth;
      ScanLinesDisplayedLabel.Left += changeInWidth;

      cropColRight.Left = cropColRight.Left + changeInWidth;
            
      ClearRectangle ();

      parameters.ComputeParameters ();
      parameters.PaintWholePanel ();

      UpdateStats ();
      UpdateVerticleScrollBar ();
    }  /* UpdateScreenForNewSize */





    private  void  UpdateVerticleScrollBar ()
    {
      int   maxNumDisplayRows = 20000;
      if  (file != null)
        maxNumDisplayRows = (int)((float)file.LargestKnownScanLine() / parameters.Ratio ());
      
      vScrollBar1.Minimum = 0;
      vScrollBar1.Maximum = maxNumDisplayRows;

      int  value = (int)parameters.DisplayRowTop ();
      if  (value > maxNumDisplayRows)
      {
        value = maxNumDisplayRows - parameters.DisplayRowsToDisplay ();
        if  (value < 0)
          value = 0;
      }

      vScrollBar1.Value       = value;
      vScrollBar1.LargeChange = parameters.DisplayRowsToDisplay ();
      vScrollBar1.SmallChange = 2;
    }  /* UpdateVerticleScrollBar */




    private  void  ScrollToPropperPlaceInScannerFile (int  displayRow)
    {
      if  (file == null)
        return;
        
      int  largestKnownDisplayRow =  (int)((float)file.LargestKnownScanLine () / parameters.Ratio ());
        
      if  (file.FrameIndexBuilt &&  (displayRow > (largestKnownDisplayRow - parameters.DisplayRowsToDisplay ())))
        return;
    
      int  deltaDisplayRows = (int)(displayRow - parameters.DisplayRowTop ());
     
      if  (Math.Abs (deltaDisplayRows) < 200)
      {
        parameters.ScrollRows2 (deltaDisplayRows);
      }
      else
      {
        parameters.SetNewDisplayRowTop (displayRow);
        parameters.PaintWholePanel ();
      }
    }  /* ScrollToPropperPlaceInScannerFile */




    private void openScannerFileToolStripMenuItem_Click (object sender, EventArgs e)
    {
      OpenFileDialog  fileDialog = new OpenFileDialog ();
      fileDialog.InitialDirectory = "";
      fileDialog.FileName = "";

      if  (lastFileOpenedName != "")
      {
        FileInfo fi = new FileInfo (lastFileOpenedName);
        fileDialog.InitialDirectory = fi.Directory.ToString ();

        fileDialog.FileName = fi.Name;
      }

      if  (fileDialog.InitialDirectory == "")
      {
        fileDialog.InitialDirectory = UmiOSservices.AddSlash (UmiVariables.ScannerFilesDefaultDir ());
      }

      fileDialog.Filter = "Scanner Files (*.txt)|*.lsc|All files (*.*)|*.*";

      fileDialog.FilterIndex = 0;
      fileDialog.RestoreDirectory = true;
      fileDialog.CheckFileExists = true;
      if (fileDialog.ShowDialog() == DialogResult.OK)
      {
        SetScannerFileName (fileDialog.FileName);
      }
    }  /* openScannerFileToolStripMenuItem_Click */



    private void ScannerPanel_Paint (object sender, PaintEventArgs e)
    {
      parameters.PaintFromBuffer ();
    }



    private void LarcosFileViewer_Load (object sender, EventArgs e)
    {
      StatusUpdateTimer.Enabled = true;
      
      if  ((initialFileName != null)  &&  (initialFileName != ""))
      {
        SetScannerFileName (initialFileName);
      }
      
      autoDetect.Checked = parameters.ExtractBlobs ();
    }  /* LarcosFileViewer_Load */




    private void StatusUpdateTimer_Tick (object sender, EventArgs e)
    {
      timerTicCount++;
      AutoScrollScannerData ();
      if  ((timerTicCount % 10) == 0)
      {
        UpdateStats ();
        UpdateVerticleScrollBar ();
      }

      if  (alarms.Count > 0)
      {
        StatusUpdateTimer.Enabled = false;

        while  (alarms.Count > 0)
        {
          Thread.Sleep (100);
          String  alarmMsg = "";
          String  alarmLogFileName = "";
          lock  (alarms)
          {
            alarmMsg = alarms.Dequeue ();
            alarmLogFileName = alarmFileNames.Dequeue ();
          }

          if  (String.IsNullOrEmpty (alarmLogFileName))
          {
            MessageBox.Show (this, alarmMsg, "Alarm",  MessageBoxButtons.OK);
          }
          else
          {
            alarmMsg += "\n";
            alarmMsg += "\n";
            alarmMsg += "Click on Yes to see log file.";

            DialogResult  dr = MessageBox.Show (this, alarmMsg, "View Log File",  MessageBoxButtons.YesNo);
            if  (dr == DialogResult.Yes)
            {
              Process proc = new Process();
     
              proc.StartInfo.WorkingDirectory = @"";
              proc.StartInfo.FileName = "notepad.exe";
              proc.StartInfo.Arguments = alarmLogFileName;
              proc.StartInfo.UseShellExecute = false;
              proc.StartInfo.RedirectStandardOutput = false;
              proc.StartInfo.RedirectStandardError = true;
              proc.Start();
              proc.WaitForExit();
              proc.Close();
            }
          }
        }

        StatusUpdateTimer.Enabled = true;
      }
    }



    private void LarcosFileViewer_SizeChanged(object sender, EventArgs e)
    {
      UpdateScreenForNewSize();
    }


    int  x1, y1, x2, y2;
    bool  definingRectangle = false;
    
    
    
    private  void  ClearRectangle ()
    {
      definingRectangle = false;
    }
    
    
    
    private  void  PaintRectangle ()
    {
      float  ratio = parameters.Ratio ();
      float  oneHalfRatio = (ratio / 2.0f);
      int  middleRow = (int)(0.5f + oneHalfRatio + ratio * ((y1 + y2) / 2.0f));
      int  middleCol = (int)(0.5f + oneHalfRatio + ratio * ((x1 + x2) / 2.0f));
      int  scanLine =  (int)((float)parameters.DisplayRowTop () * ratio) + middleRow;
      MouseCoordinates.Text = scanLine.ToString("###,###,##0") + " - " + middleCol.ToString ("#,##0");
      parameters.PaintTemporayRectangle (Color.Red, x1, y1, x2, y2);
    }  /* PaintRectangle */




//*********************************************************************************************
//
//  Folowing code was lifted from web site below
// http://groups.google.com/group/microsoft.public.dotnet.languages.csharp/browse_thread/thread/4c541e081043cf4b/99e885ef8f09cc78?lnk=st&q=c%23++MouseWheel++Event++Panel&rnum=2&hl=en#99e885ef8f09cc78
//
//  It intercepts the Wheel mouse movements and forces them to generate an
//  event.
//
//
//*********************************************************************************************
    bool sendingMessage = false; 
    protected override void WndProc (ref Message m) 
    { 
      Point mp = MousePosition; 
      if (m.Msg == 522) 
      { 
         if (sendingMessage) 
         { 
           m.Result = (IntPtr)0; 
          return; 
         } 
          
         //Debug.Print("Mouse Wheel: lparam = {0},WParam = {1}, Result ={2}", 
         //            m.LParam, m.WParam, m.Result
         //           );
          
         try
         { 
           sendingMessage = true; 
           SendMessage (GetChildAtPoint (PointToClient(mp)).Handle, 
                        m.Msg, 
                        m.WParam.ToInt32(), 
                        m.LParam.ToString()
                       ); 
           sendingMessage = false; 
         } 
         catch  (Exception exception)
         { 
           string s = exception.ToString (); 
           sendingMessage = false; 
         } 
         m.Result = (IntPtr)0; 
      } 
      base.WndProc(ref m);
    } /* WndProc */



    [DllImport("user32.dll")] 
      public static extern int SendMessage (IntPtr hwnd, 
                                            [MarshalAs(UnmanagedType.U4)] 
                                                       int Msg, 
                                            int wParam, 
                                            [MarshalAs(System.Runtime.InteropServices.UnmanagedType.LPStr)] 
                                                      string lParam
                                           );
//*********************************************************************************************



 
    private void  ScannerPanel_MouseWheel (object sender, MouseEventArgs e)
    {
      int delta = e.Delta / 3;
      
      int  newTopDisplayRow = (int)(parameters.DisplayRowTop () - delta);
      if  (newTopDisplayRow < 0)
        newTopDisplayRow = 0;
      
      ScrollToPropperPlaceInScannerFile (newTopDisplayRow);
      ClearRectangle ();
      UpdateVerticleScrollBar ();
    }  /* ScannerPanel_MouseWheel */



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

      int  newTopDisplayRow = (int)(parameters.DisplayRowTop () + autoScrollRate);
      if  (newTopDisplayRow < 0)
      {
        autoScrollRate = 0;
        newTopDisplayRow = 0;
        AutoScrollingRateTextSet ();
      }

      ScrollToPropperPlaceInScannerFile (newTopDisplayRow);
    }  /* AutoScrollScannerData */


    
    private void LarcosFileViewer_KeyDown (object sender, KeyEventArgs e)
    {
      Keys k = e.KeyCode;

      int  newTopDisplayRow = -1;
      if  (k == Keys.PageDown)
        newTopDisplayRow = (int)(parameters.DisplayRowTop () + parameters.DisplayRowsToDisplay ());

      else if (k == Keys.PageUp)
        newTopDisplayRow = (int)(parameters.DisplayRowTop () - parameters.DisplayRowsToDisplay ());

      else if  ((Control.ModifierKeys == Keys.Control)  &&  (k == Keys.G))
        ProcessGotoScanLine ();

      else if  ((Control.ModifierKeys == Keys.Control)  &&  (k == Keys.Home))
        newTopDisplayRow = 1;

      else if  ((Control.ModifierKeys == Keys.Control)  &&  (k == Keys.End))
        newTopDisplayRow = (int)((float)file.LargestKnownScanLine() / parameters.Ratio ()) - parameters.DisplayRowsToDisplay ();

      if  (newTopDisplayRow > 0)
      {
        ScrollToPropperPlaceInScannerFile (newTopDisplayRow);
        ClearRectangle();
        UpdateVerticleScrollBar();
      }
    }


    private  void  ProcessGotoScanLine ()
    {
      GoToScanLinePrompt  gotoPrompt = new GoToScanLinePrompt ((int)file.LargestKnownScanLine());
      DialogResult dr = gotoPrompt.ShowDialog ();
      if  (dr == DialogResult.OK)
      {
        int  newTopDisplayRow = (int)((float)gotoPrompt.SelectedScanLine / parameters.Ratio ());
        ScrollToPropperPlaceInScannerFile (newTopDisplayRow);
        ClearRectangle();
        UpdateVerticleScrollBar();
      }
    }
    


    private void ScannerPanel_MouseDown (object sender, MouseEventArgs e)
    {
      if  (parameters.SelectedBlob (e.X, e.Y) == null)
      {
        x1 = e.X;
        y1 = e.Y;
        definingRectangle = true;
      }
    }


    private void ScannerPanel_MouseDoubleClick (object sender, MouseEventArgs e)
    {
      UmiParticleEntry  pe = parameters.SelectParticle (e.X, e.Y);
      if  (pe != null)
      {
        string  fileName = UmiOSservices.GetRootName (scannerFileName) + "_" +
                           pe.ScannerRow.ToString ("00000000")         + "_" +
                           pe.ScannerCol.ToString ("0000") +
                           ".bmp";

        UmiRaster r = parameters.GetRasterForParticle (pe);
        if  (r != null)
        {
          //r.FileName = fileName;
          ImageEditor  imageEditor = new ImageEditor (r);
          imageEditor.Show ();
          imageEditors.Add (imageEditor);
          ClearRectangle ();
        }
        return;
      }

      Blob  selectedBlob = parameters.SelectedBlob (e.X, e.Y);
      if  (selectedBlob != null)
      {
        // User has selected region to work on seperatly.
        
        Int32  startScanLine = 0;
        Int32  startCol      = 0;
        
        byte[][]  selectedRegion = parameters.ExtractImageFromScanner (selectedBlob,
                                                                       ref startScanLine,
                                                                       ref startCol
                                                                      );
        if  (selectedRegion == null)
          return;

        string  fileName = UmiOSservices.GetRootName (scannerFileName) + "_" +
                           startScanLine.ToString ("00000000")     + "_" +
                           startCol.ToString ("0000") +
                           ".bmp";
                           
        int  height = selectedRegion.Length;
        int  width  = selectedRegion[0].Length;
        
        ImageEditor  imageEditor = new ImageEditor (fileName, 
                                                    height, 
                                                    width, 
                                                    selectedRegion,
                                                    saveDebugImages
                                                   );
        
        imageEditor.Show ();
        imageEditors.Add (imageEditor);
        ClearRectangle ();
      }
    }                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          
   


    private void ScannerPanel_MouseUp (object sender, MouseEventArgs e)
    {
      if  (definingRectangle)
      {
        x2 = e.X;
        y2 = e.Y;
        
        int  top, bot, left, right;
         
        top   = Math.Max (y1, y2);
        bot   = Math.Min (y1, y2);
        left  = Math.Min (x1, x2);
        right = Math.Max (x1, x2);

        parameters.AddUserSpecifiedBlob (top, left, bot, right);
        definingRectangle = false;

        parameters.PaintAllBlobs ();
      }
    }



    private void  ScannerPanel_MouseMove (object sender, MouseEventArgs e)
    {
      if  (definingRectangle)
      {
        x2 = e.X;
        y2 = e.Y;
        PaintRectangle ();
      }
    }


    private void exitToolStripMenuItem_Click(object sender, EventArgs e)
    {
      if  (file != null)
        file.FrameBytesOffsetsUpdateThreadTerminate();

      Close();
    
//      file.FrameBytesOffsetsUpdateThreadTerminate ();
//      Application.Exit();
    }
    
    

    private void LarcosFileViewer_FormClosing (object sender, FormClosingEventArgs e)
    {
      if  (file != null)
        file.FrameBytesOffsetsUpdateThreadTerminate();
      SaveLarcosFileViewerConfiguration ();

      DisposeExistingDataStructures ();
      
      if  (parameters != null)
      {
        parameters.Dispose ();
        parameters = null;
      }
    }



    private void AutoScrollUp_Click (object sender, EventArgs e)
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



    private void AutoScrollDown_Click (object sender, EventArgs e)
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



    private void AutoScrollStop_Click (object sender, EventArgs e)
    {
      autoScrollRate = 0;
      AutoScrollingRateTextSet ();
    }



    private void vScrollBar1_Scroll (object sender, ScrollEventArgs e)
    {
      autoScrollRate = 0;
      AutoScrollingRateTextSet ();
      ScrollToPropperPlaceInScannerFile (vScrollBar1.Value);
      ClearRectangle ();
    }


    private void autoDetect_CheckedChanged (object sender, EventArgs e)
    {
      parameters.ExtractBlobs (autoDetect.Checked);
    }





    public  void  CancelLoadingModels ()
    {
      ActiveTrainingLibraries.CancelLoads ();
    }  /* CancelLoadingModels */




    private  void  RightMouseMenu (int  displayRow,
                                   int  displayCol
                                  )
    {
    }  /* RightMouseMenu */




    private void colorize_CheckedChanged(object sender, EventArgs e)
    {
      if  (parameters.Colorize != colorize.Checked)
      {
        parameters.Colorize = colorize.Checked;
        parameters.PaintWholePanel ();
      }
    }


    private void headerInfoToolStripMenuItem_Click (object sender, EventArgs e)
    {
      if  (file == null)
        return;

      UmiScannerFile.HeaderField[] headerFields = file.GetHeaderFields ();
      HeaderFields  headerFieldsDialog = new HeaderFields (FileName.Text, headerFields);
      headerFieldsDialog.ShowDialog (this);
    }



    private void cameraLineToolStripMenuItem_Click (object sender, EventArgs e)
    {
      if  (file == null)
        return;

      String  cameraLineStr = file.GetValue ("CameraLine");
      if  (!String.IsNullOrEmpty (cameraLineStr))
      {
        String[]  cameraLineFields = cameraLineStr.Split (',');
        if  (cameraLineFields != null)
        {
          byte[]  cameraData = new byte[cameraLineFields.Length];
          for  (int x = 0;  x < cameraLineFields.Length;  ++x)
            cameraData[x] = UmiKKStr.StrToByte (cameraLineFields[x]);

          CameraLineDisplay  cld = new CameraLineDisplay (cameraData);
          cld.ShowDialog (this);
        }
      }
    }

    private void autoDetectionToolStripMenuItem1_Click (object sender, EventArgs e)
    {
      if  (String.IsNullOrEmpty (scannerFileName))
        return;

      if  ((file != null)  &&  (!file.ParticleEntriesLoaded ()))
        return;
    }

  }  /* LarcosFileViewer */
}
