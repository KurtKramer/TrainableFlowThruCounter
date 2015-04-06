using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

using LarcosCounterManaged;
using KKManagedRoutines;

namespace LarcosCounter
{
  public partial class AuditorByClass : Form
  {
    private  int                          backgroundPixelTH      = 31;    
    private  LarcosCounterManagerWrapper  cameraManager          = null;
    private  UmiClassList                 classesInParticleList  = null;
    private  UmiClassList                 classesInTrainingModel = null;
    private  UmiClassList                 classesAvailableForSelection = null;
    private  int                          erosionStructSize      = 0;
    private  bool                         formIsMaximized        = false;
    private  UmiShrimpLengthComputer      lengthComputer         = null;
    private  UmiInstallationConfig        installationConfig     = null;
    private  List<UmiClass>               lastClassesValidated   = new List<UmiClass> ();
    private  int                          lastWidth              = 0;
    private  int                          lastHeight             = 0;
    private  UmiOperatingParameters       operatingParameters    = null;
    private  UmiParticleEntryList         particles              = null;
    private  UmiParticleEntryList         particlesInSelectedClass = null;
    private  float                        pixelsPerMM            = 23.6f;
    private  UmiRunLog                    runLog                 = null;
    private  UmiScannerFileBuffered       scannerFile            = null;
    private  float                        scanRate               = 20000.0f;
    private  UmiClass                     selectedClass          = null;
    private  UmiTrainingConfiguration     trainingConfiguration  = null;
    private  string                       trainingModelName      = "";


    private  int   thumbNailHeight = 150;
    private  int   thumbNailWidth  = 150;
    private  int   thumbnailGridCurRow = 0;
    private  int   thumbnailGridCurCol = 0;
    private  int   thumbnailGridLastHeight = 0;
    private  int   thumbnailGridLastWidth  = 0;
    

    public  enum  Tasks {NULL,  DisplaySelectedClass};

    private  Queue<Tasks>  taskQueue = new Queue<Tasks> ();


    public class ThumbNailImageColumn: DataGridViewColumn
    {
      private  IWin32Window  parent = null;

      public  IWin32Window  Parent  {get {return  parent;}}

      public ThumbNailImageColumn (IWin32Window  _parent)
      {
        parent = _parent;
        this.CellTemplate = new ThumbNailImageCell ();
      }
    }    /* ThumbNailImageColumn */




    /// <summary>
    /// 
    /// </summary>
    /// <param name="_cameraManager"></param>
    /// <param name="_scannerFile">Scanner File that we are auditing;  before calling this constructor the 
    /// loading of Particles must have been completed.</param>
    public AuditorByClass (LarcosCounterManagerWrapper  _cameraManager,
                           UmiScannerFileBuffered       _scannerFile,
                           UmiRunLog                    _runLog
                          )
    {
      cameraManager = _cameraManager;
      scannerFile   = _scannerFile;
      scanRate      = scannerFile.ScanRate;
      runLog        = _runLog;

      if  (runLog == null)
        runLog = new UmiRunLog ();

      ThumbNailImageCell.SetCurScannerFile (scannerFile);
 
      InitializeDataStructures ();

      InitializeComponent ();
    }


    private  void  TaskAdd (Tasks  t)
    {
      lock  (taskQueue)
      {
        taskQueue.Enqueue (t);
      }
    }


    private  Tasks  TaskNext ()
    {
      Tasks  t = Tasks.NULL;

      lock  (taskQueue)
      {
        if  (taskQueue.Count > 0)
          t = taskQueue.Dequeue ();
      }
      return t;
    }


    private  void  InitializeDataStructures ()
    {
      installationConfig = cameraManager.Installation (runLog);
      if  (installationConfig == null)
        installationConfig = new UmiInstallationConfig (runLog);

      installationConfig.UpdateFromScannerFile (scannerFile.ScannerFile ());

      float  widthInMeters = installationConfig.ImagingChamberWidth;
      if  (widthInMeters == 0.0f)
        widthInMeters = 2.5f * 2.54f / 100.0f;   // (2.5 inches) * (2.54 cm's/inch)  /  (100 cm's/m)

      int  pixelsPerScanLine = installationConfig.ImagingChamberWidthPixels;
      pixelsPerMM = (float)pixelsPerScanLine /  (1000.0f  * widthInMeters);
      lengthComputer = new UmiShrimpLengthComputer (pixelsPerScanLine, cameraManager.ImagingChamberWidth (),  erosionStructSize, backgroundPixelTH);

      operatingParameters = cameraManager.GetOperatingParameters ();
      if  (operatingParameters != null)
        erosionStructSize = operatingParameters.ErosionStructSize;

      particles = scannerFile.ParticleEntries ();
      if  (particles != null)
      {
        erosionStructSize = particles.ErosionStructSize;
        trainingModelName = particles.TrainingModelName;
        classesInParticleList = particles.ExtractListOfClasses ();
        operatingParameters = particles.GetOperatingParameters ();
      }

      if  (String.IsNullOrEmpty (trainingModelName))
        trainingModelName = cameraManager.GetTrainingModelName ();

      if  (!String.IsNullOrEmpty (trainingModelName))
      {
        ActiveTrainingLibraries.SetModel1 (trainingModelName, false, DoneLoadingTrainModel);
      }
      else
      {
        if  (!String.IsNullOrEmpty (ActiveTrainingLibraries.Model1Name))
        {
          ActiveTrainingLibraries.LoadModels (DoneLoadingTrainModel);
        }
      }

      trainingConfiguration = new UmiTrainingConfiguration (trainingModelName, operatingParameters, runLog);
      
      classesInTrainingModel = trainingConfiguration.GetClassList ();

      classesAvailableForSelection = classesInTrainingModel + classesInParticleList;
      classesAvailableForSelection.SortByName ();

      int  x = classesAvailableForSelection.Count;
      while  (x > 0)
      {
        --x;
        UpdateLastClassesValidated (classesAvailableForSelection[x]);
      }

      UmiOperatingParameters  trainModelOpParms = trainingConfiguration.GetOperatingParameters ();
      if  (trainModelOpParms != null)
        operatingParameters = trainModelOpParms;

      ThumbNailImageCell.SetOperatingParameters (operatingParameters);
    }  /* InitializeDataStructures */



    private  void  BuildClassButtonList ()
    {
      int  tabIndex = 0;

      Font  f = new System.Drawing.Font ("Microsoft Sans Serif", 16F);
      Size  s = new Size (ClassButonListPanel.Width - 4, f.Height + 8);

      foreach  (UmiClass c in classesAvailableForSelection)
      {
        Button b = new Button ();
        b.Name = c.Name + "_Button";
        b.Text = c.Name;
        b.Size = s;
        b.Click += new System.EventHandler (SelectAClass_Click);
        b.Font = f;
        b.TabIndex = tabIndex;
        ++tabIndex;
        b.UseVisualStyleBackColor = true;
        try
        {
          ClassButonListPanel.Controls.Add (b);
        }
        catch  (Exception e)
        {
          MessageBox.Show (this, "Adding Button to ClassList", e.ToString (), MessageBoxButtons.OK);
        }
      }
    }  /* BuildClassButtonList */



    private void SelectAClass_Click (object            sender, 
                                     System.EventArgs  e
                                    )
    {
      if  (sender == null)
        return;

      string className = ((Button)sender).Text;
      UmiClass  c = UmiClassList.GetUniqueClass (className, "");
      UpdateSelectedClass (c);
    }  /* DisplayWorstTrainingImages2 */



    private  void  UpdateSelectedClass (UmiClass  _selectedClass)
    {
      if  (_selectedClass== null)
        return;

      selectedClass = _selectedClass;
      ThumbNailImageCell.SetBaseClass (selectedClass);
      CurrentClass.Text = selectedClass.Name;
      particlesInSelectedClass = particles.GetParticlesByPredictedClass (selectedClass);
      if  (particlesInSelectedClass != null)
      {
        ClassCount.Text = particlesInSelectedClass.Count.ToString ("###,##0");
        String  sortOrderReq = (String)(SortOrder.SelectedItem);
        particlesInSelectedClass.Sort (sortOrderReq);
      }

      AddToThumbnailGridPanel (particlesInSelectedClass);
    }



    private void  AddToThumbnailGridPanel (UmiParticleEntryList  particles)
    {
      DefineThumbnailGridViewRowAndColumns ();

      if  (particles == null)
        return;

      thumbnailGridCurRow = 0;
      thumbnailGridCurCol = 0;

      int  numCols = ThumbnailGrid.Columns.Count;

      thumbnailGridCurRow = ThumbnailGrid.Rows.Count - 1;
      if  (ThumbnailGrid.Rows.Count == 0)
        thumbnailGridCurCol = numCols;

      int  idx = 0;
      while  (idx < particles.Count)
      {
        UmiParticleEntry  pe = particles[idx];
         
        if  (thumbnailGridCurCol >= numCols)
        {
          object[] row = new object[numCols];
          ThumbnailGrid.Rows.Add (row);
          thumbnailGridCurRow = ThumbnailGrid.Rows.Count - 1;
          thumbnailGridCurCol = 0;
          ThumbnailGrid.Rows[thumbnailGridCurRow].Height = thumbNailHeight + 2;
        }
      
        ThumbnailGrid.Rows[thumbnailGridCurRow].Cells[thumbnailGridCurCol].Value = pe;
        thumbnailGridCurCol++;
        idx++;
      }
    }  /* AddToThumbnailGridPanel */




    private  void  DefineThumbnailGridViewRowAndColumns ()
    {
      int  width  = ThumbnailGrid.Width;
      int  height = ThumbnailGrid.Height;

      int  deltaHeight = height - thumbnailGridLastHeight;
      int  deltaWidth  = width  - thumbnailGridLastWidth;

      thumbnailGridLastHeight = ThumbnailGrid.Height;
      thumbnailGridLastWidth  = ThumbnailGrid.Width;

      int  requiredHeight = thumbNailHeight;
      int  requiredWidth  = thumbNailWidth;

      int  numCols = (int)Math.Floor ((float)(width - 15)  / (float)requiredWidth);
      if  (numCols < 1)  
        numCols = 1;

      int  numRows = (int)Math.Floor ((float)height / (float)requiredHeight);
      if  (numRows < 1)
        numRows = 1;

      ThumbnailGrid.Rows.Clear ();
      ThumbnailGrid.Columns.Clear ();
      //ThumbnailDataGridView.ColumnCount = numCols;

      for  (int colIDX = 0;  colIDX < numCols;  colIDX++)
      {
        //DataGridViewImageColumn  dgvic = new DataGridViewImageColumn ();
        ThumbNailImageColumn  dgvic = new ThumbNailImageColumn (this);
        
        dgvic.Name = "ThumbCol" + colIDX;
        dgvic.HeaderText = "";
        dgvic.Width = thumbNailWidth;
        ThumbnailGrid.Columns.Add (dgvic);
      }

      {
        // Define Row Info
        DataGridViewCellStyle  dgvcs = new DataGridViewCellStyle ();
        dgvcs.Alignment = DataGridViewContentAlignment.MiddleCenter;
        ThumbnailGrid.RowsDefaultCellStyle = dgvcs;
        //try  {ThumbnailGrid.RowCount = numRows;}
        //catch  (Exception e)
        //{
        //  runLog.WriteLine ("DefineThumbnailGridViewRowAndColumns  \n" + e.ToString ());
        //}
      }

      //AddToThumbnailGridPanel (particlesInSelectedClass);

      BuildThumbnailGridConetextMenu ();
    }  /* DefineThumbnailGridViewRowAndColumns */




    private  UmiParticleEntryList  GetSelectedParticles ()
    {
      UmiParticleEntryList  selectedParticles = new UmiParticleEntryList ();
      foreach  (DataGridViewCell  dgvc in  ThumbnailGrid.SelectedCells)
      {
        int  colIndex = dgvc.ColumnIndex;
        int  rowIndex = dgvc.RowIndex;

        UmiParticleEntry  pe = (UmiParticleEntry)ThumbnailGrid.Rows[rowIndex].Cells[colIndex].Value;
        if  (pe != null)
          selectedParticles.Add (pe);
      }

      return  selectedParticles;         
    }  /* GetSelectedParticles */




    private  void  BuildThumbnailGridConetextMenu ()
    {
      ContextMenuStrip cms = new ContextMenuStrip ();

      cms.Items.Add ("Add to Training Library",      null, AddToTrainingLibrary);
      cms.Items.Add ("Select Class",                 null, ValidateSelectAClass);
      cms.Items.Add ("Copy to Clipboard",            null, CopyParticleToClipboard);
      cms.Items.Add ("Copy to Clipboard with Grid",  null, CopyImageToClipboardWithGrid);

      foreach  (UmiClass  c in lastClassesValidated)
      {
        cms.Items.Add (c.Name, null, ValidateTheseClasses);
        if  (cms.Items.Count > 10)
          break;
      }
 
      foreach  (DataGridViewColumn  dgvc in ThumbnailGrid.Columns)
        dgvc.ContextMenuStrip = cms;
    }  /* BuildThumbnailGridConetextMenu */



    private  void  UpdateLastClassesValidated (UmiClass  updatedClass)
    {
      int  idx  = 0;

      UmiClass next2Add = updatedClass;

      idx = 0;
      while  (true)
      {
        if  (idx >= lastClassesValidated.Count)
        {
          lastClassesValidated.Add (next2Add);
          break;
        }

        UmiClass temp = lastClassesValidated[idx];
        lastClassesValidated[idx] = next2Add;
        if  (temp == updatedClass)
          break;
        next2Add = temp;
        idx++;
      }
    }   /* UpdateLastClassesValidated */


    
    private  void  AddParticlesToTrainingLibrary (UmiParticleEntryList  particles)
    {
      UmiTrainingModel2  tm1 = ActiveTrainingLibraries.Model1 ();
      if  (tm1 == null)
         tm1 = ActiveTrainingLibraries.Model2 ();

      if  (tm1 == null)
      {
        MessageBox.Show (this, "There is no Active Training Library specified.");
        return;
      }

      foreach  (UmiParticleEntry  pe  in  particles)
      {
        if  (pe == null)
          continue;

        UmiRaster  particle = scannerFile.GetRasterForParticleAsCounted (pe, operatingParameters);
        if  (particle == null)
        {
          MessageBox.Show (this, "Was unable to get Particle from Scanner File[" + pe.FileName + "]" );
          continue;
        }

        try 
        {
          tm1.AddImageToTrainingLibray (pe.FileName, particle, pe.PredClass, true);
        }
        catch (Exception e)
        {
          MessageBox.Show (this, e.ToString (), "Adding Particle to Training Library");
        }
      }
    }  /* AddParticlesToTrainingLibrary */



   
    
    private  void  AddToTrainingLibrary (Object sender, EventArgs e)
    {
      if  (ActiveTrainingLibraries.Model1 () == null)
      {
        MessageBox.Show (this, "There is no currently selected training library.", "AddToTrainingLibrary", MessageBoxButtons.OK);
        return;
      }

      UmiParticleEntryList  particles = GetSelectedParticles ();
    
      if  (particles.Count < 1)
      {
        MessageBox.Show (this, "There are no selected particles.");
        return;
      }

      {
        DialogResult dr = MessageBox.Show (this, "Add the selected Particles to Training Library[" + ActiveTrainingLibraries.Model1Name + "]", "AddToTrainingLibrary", MessageBoxButtons.YesNo);
        if  (dr == DialogResult.Yes)
          AddParticlesToTrainingLibrary (particles);
      }
    }  /* AddToTrainingLibrary */



    private  void  ValidateSelectAClass (Object    sender, 
                                         EventArgs e
                                        )
    {
      String  rootDir = "";

      if  (trainingConfiguration != null)
        rootDir = trainingConfiguration.RootDirExpanded ();

      UmiClass likelyClass = null;
      UmiParticleEntryList  selectedParticles = GetSelectedParticles ();
      if  ((selectedParticles != null)  &&  (selectedParticles.Count > 0))
      {
        UmiParticleEntry  pe = selectedParticles[0];
        likelyClass = pe.PredClass;
      }
      
      SelectAClass  sac = new SelectAClass (classesInTrainingModel, rootDir, classesInTrainingModel);
      sac.SelectedClass = likelyClass;
      sac.ShowDialog ();
      if  (sac.SelectionMade  &&  (sac.SelectedClass != null))
      {
        ValidateSelectedParticlesWithThisClass (sac.SelectedClass);
      }
    }  /*  ValidateSelectAClass */



    private  void  ValidateTheseClasses (Object sender, EventArgs e)
    {
      ToolStripItem item = (ToolStripItem)sender;
      String  validatedClassName = item.Text;
      UmiClass  validatedClass = UmiClassList.GetUniqueClass (validatedClassName, "");
      ValidateSelectedParticlesWithThisClass (validatedClass);
    }


    private  void  ValidateSelectedParticlesWithThisClass (UmiClass  validatedClass)
    {
      UmiParticleEntryList  selectedParticles = GetSelectedParticles ();

      foreach  (UmiParticleEntry  pe  in  selectedParticles)
      {
        if  (pe == null)
          return;

        pe.PredClass = validatedClass;
        particles.AddUpdateToReportFile (pe);
      }

      UpdateLastClassesValidated (validatedClass);
      BuildThumbnailGridConetextMenu ();
      PopulateCount ();
    }  /*  ValidateSelectedParticlesWithThisClass */



    private  void  CopyParticleToClipboard (Object sender, EventArgs e)
    {
      UmiParticleEntryList  selectedParticles = GetSelectedParticles ();
      if  (selectedParticles == null)
        return;

      if  (selectedParticles.Count != 1)
      {
        MessageBox.Show (this, "Only one particles can be copied to clipboard.");
        return;
      }
      
      UmiRaster  r =  scannerFile.GetRasterForParticle (selectedParticles[0]);
      if  (r == null)
      {
        MessageBox.Show (this, "Could not load particle[" + selectedParticles[0].FileName + "].");
        return;
      }

      Image  i = r.BuildBitmap ();
      if  (i != null)
        Clipboard.SetImage (i);
    }  /* CopyParticleToClipboard */





    private  void  CopyImageToClipboardWithGrid (Object sender, EventArgs e)
    {
      UmiParticleEntryList  selectedParticles = GetSelectedParticles ();
      if  (selectedParticles == null)
        return;

      if  (selectedParticles.Count != 1)
      {
        MessageBox.Show (this, "Only one particles can be copied to clipboard.");
        return;
      }
      
      UmiRaster  r =  scannerFile.GetRasterForParticle (selectedParticles[0]);
      if  (r == null)
      {
        MessageBox.Show (this, "Could not load particle[" + selectedParticles[0].FileName + "].");
        return;
      }

      if  (!r.IsColor)
      {
        r = r.ReversedImage ();
        r = r.ToColor ();
      }

      r.DrawGrid (pixelsPerMM, 10, Color.Red, Color.FromArgb (30, 255, 0, 0));
      
      Image  i = r.BuildBitmap ();
      if  (i != null)
        Clipboard.SetImage (i);
    }  /* CopyImageToClipboardWithGrid */




    private  void  DisplayPaticleImage (UmiParticleEntry  pe)
    { 
      String  fileName = pe.FileName;

      UmiRaster  r = scannerFile.GetRasterForParticleAsCounted (pe, operatingParameters);
      if  (r != null)
      {
        ImageVerifier iv = new ImageVerifier (cameraManager, pe, r, fileName, pixelsPerMM, runLog);
        iv.ShowDialog (this);
        if  (iv.ClassUserValidatesAs != pe.PredClass)
        {
          pe.PredClass = iv.ClassUserValidatesAs;
          particles.AddUpdateToReportFile (pe);
        }
      }
    }  /* DisplayPaticleImage */

    


    public void  ThumbnailGrid_CellContentDoubleClick (object                    sender, 
                                                       DataGridViewCellEventArgs e
                                                      )
    {
      int  colIDX = e.ColumnIndex;
      int  rowIDX = e.RowIndex;

      UmiParticleEntry  pe = (UmiParticleEntry)ThumbnailGrid.Rows[rowIDX].Cells[colIDX].Value;
      if  (pe == null)
        return;
      DisplayPaticleImage (pe);
    }  /* ThumbnailGrid_CellContentDoubleClick */




    private  void  DoneLoadingTrainModel (UmiTrainingModel2  loadedModel)
    {
      if  (loadedModel != null)
      {
        UmiOperatingParameters  opParms = loadedModel.GetOperatingParameters ();
        if  (opParms != null)
        {
          operatingParameters = opParms;
          ThumbNailImageCell.SetOperatingParameters (operatingParameters);
        }
      }
    }



    private void AuditorByClass_Load (object sender, EventArgs e)
    {
      thumbnailGridLastHeight = ThumbnailGrid.Height;
      thumbnailGridLastWidth  = ThumbnailGrid.Width;

      lastHeight = Height;
      lastWidth  = Width;

      BuildClassButtonList ();

      String[]  sortOptions = UmiParticleEntryList.SortOrderOptions ();
      SortOrder.DataSource = sortOptions;
      SortOrder.SelectedItem = sortOptions[0];

      UmiClass  initialClass = null;
      if  ((classesAvailableForSelection != null)  &&  (classesAvailableForSelection.Count > 0))
      {
        initialClass = classesAvailableForSelection[0];
        foreach  (UmiClass  c  in  classesAvailableForSelection)
        {
          if  (c.Name.StartsWith ("Shrimp", true, System.Globalization.CultureInfo.CurrentCulture))
          {
            initialClass = UmiClassList.GetUniqueClass (c.Name, "");
            break;
          }
        }

        UpdateSelectedClass (initialClass);
      }
    }


    
    private  void  PopulateCount ()
    {
      if  (particles != null)
        Count.Text = particles.SubjectCount (scannerFile).ToString ("#,###,##0");
      else
        Count.Text = "0";
    }
    

    private  void  AuditorByClassFormResized ()
    {
      int  deltaHeight = Height - lastHeight;
      int  deltaWidth  = Width  - lastWidth;

      lastHeight = Height;
      lastWidth  = Width;

      if  (lastHeight == 0)
        return;

      ClassButonListPanel.Height += deltaHeight;

      SortOrderLabel.Left += deltaWidth;
      SortOrder.Left   += deltaWidth;

      ThumbnailGrid.Height  += deltaHeight;
      ThumbnailGrid.Width   += deltaWidth;

      UpdateSelectedClass (selectedClass);
    }



    private void AuditorByClass_Resize (object sender, EventArgs e)
    {
      if  (WindowState == FormWindowState.Maximized)
      {
        // Looks like user has pressed the Maximized button.  We have to trap it here because
        // the ResizeEnd event does not trap when form is Maximized.
        AuditorByClassFormResized ();
        formIsMaximized = true;
      }
      else if  (WindowState == FormWindowState.Normal)
      {
        if  (formIsMaximized)
        {
          // We normally trap the ResizeEnd event;  but when the form was already maximized and the user
          // presses the button to un-maximize.  the ResizeEnd does not trap that.  So we check to 
          // see if the form was already maximize.  If so then we resized the form.
          AuditorByClassFormResized ();
          formIsMaximized = false;
        }
      }
    }



    private void AuditorByClass_ResizeEnd (object sender, EventArgs e)
    {
       AuditorByClassFormResized ();
    }


    private void ThumbnailGrid_CellFormatting (object sender, DataGridViewCellFormattingEventArgs e)
    {
    }


    private void ThumbnailGrid_DataError (object sender, DataGridViewDataErrorEventArgs e)
    {
      int  colIdx = e.ColumnIndex;
      int  rowIdx = e.RowIndex;
    }



    private void timer1_Tick (object sender, EventArgs e)
    {
      Tasks  t = TaskNext ();
      if  (t == Tasks.NULL)
        return;

      switch  (t)
      {
        case  Tasks.DisplaySelectedClass:
          break;
      }


    }

    private void SortOrder_SelectedIndexChanged (object sender, EventArgs e)
    {
      if  (selectedClass != null)
        UpdateSelectedClass (selectedClass);
    }

  }
 
}
