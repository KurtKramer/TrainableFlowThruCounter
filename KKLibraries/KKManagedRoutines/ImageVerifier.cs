using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

using KKManagedRoutines;
using LarcosCounterManaged;


namespace KKManagedRoutines
{
  public partial class ImageVerifier : Form
  {
    LarcosCounterManagerWrapper  cameraManager = null;

    private  UmiRaster  raster    = null;
    private  String     fileName  = null;
    private  UmiRunLog  runLog    = null;

    private  UmiClass   classUserValidatesAs = null;  // Will only be set if USER validates during this Dialog session.

    private  int        lastHeight = 0;
    private  int        lastWidth  = 0;

    private  int        leftPanelLastHeight = 0;
    private  int        leftPanelLastWidth  = 0;

    private  String     trainingModelName = "";

    private  UmiTrainingModel2  trainingModel = null;

    private  float      zoomFactor = 1.0f;

    private  bool       displayGrid = false;

    private  bool       okPressed = false;

    private  float      pixelsPerMM = 1.0f / 0.034f;

    private  UmiShrimpLengthComputer  lengthComputer = null;


    private  UmiParticleEntry  particleEntry = null;

    private  ToolTip     toolTip = null;


    public  UmiClass  ClassUserValidatesAs  {get {return classUserValidatesAs;}}

    public  bool      OkPressed             {get {return okPressed;}}


    public ImageVerifier (LarcosCounterManagerWrapper  _cameraManager,
                          UmiParticleEntry             _particleEntry,
                          UmiRaster                    _raster,
                          String                       _fileName,
                          float                        _pixelsPerMM,
                          UmiRunLog                    _runLog
                         )
    {
      cameraManager     = _cameraManager;
      trainingModelName = ActiveTrainingLibraries.Model1Name;
      particleEntry     = _particleEntry;
      raster            = _raster;
      fileName          = _fileName;
      runLog            = _runLog;

      pixelsPerMM = cameraManager.ImagingChamberPixelsPerMM ();
      if  (pixelsPerMM <= 0.0f)
        pixelsPerMM = (1500 / (2.5f * 2.54f / 100.0f)) / 1000.0f;
      
      trainingModel = ActiveTrainingLibraries.Model1 ();

      if  (runLog == null)
        runLog = new UmiRunLog ();

      if  (raster == null)
      {
        MessageBox.Show ("The Raster image was not provided.");
        return;
      }

      InitializeComponent();

      ImageFileName.Text = fileName;

      SetUpToolTips ();
    }

    
    private void  InitializeScreenFields ()
    {
      if  (cameraManager.OperatingMode () == LarcosOperatingModes.User)
      {
        AddNewClassButtom.Enabled = false;
        AddNewClassButtom.Visible = false;
      }
      else
      {
        AddNewClassButtom.Enabled = true;
        AddNewClassButtom.Visible = true;
      }

      if  (particleEntry != null)
        OriginalClass.Text = particleEntry.PredClassName;
      UpdateVerifiedClassList ();
    }  /* PopulateClassChooser */




    private  void  UpdateVerifiedClassList ()
    {
      String[] names = null;
      if  (trainingModel == null)
      {
        TrainingModelName.Text = "";
        names = new String[]{"Shrimp_01", "Strimp_02", "Strimp_03", "Bubbles", "Detritus", "Noise"};
      }
      else
      {
        UmiTrainingConfiguration  config = new UmiTrainingConfiguration (trainingModel.ModelName, null, runLog);
        UmiClassList  classes = config.GetClassList ();
        if  (classes != null)
          names = classes.ToNameArray ();
        TrainingModelName.Text = trainingModel.ModelName;
      }

      VerifiedClass.DataSource = names;
      VerifiedClass.SelectedItem = particleEntry.PredClass.Name;
    }



    private  void  SetUpToolTips ()
    {
      toolTip = new ToolTip ();
      toolTip.SetToolTip (DisplayGridButton,               "Turns on Grid display;  each grid line represents 1 mm.");
      toolTip.SetToolTip (ComputeLengthButton,             "Will compute length of shrimp and display results in the form of red line segments.");
      toolTip.SetToolTip (LengthInMM,                      "Displays length of Shrimp, if computed, in mm's.");
      toolTip.SetToolTip (OrientationAngle,                "Displays orientation angle,in degrees, of shrimp if computed .");
      toolTip.SetToolTip (OriginalClass,                   "Class that was last assigned to image.");
      toolTip.SetToolTip (TrainingModelName,               "Currently Selected Classifier.");
      toolTip.SetToolTip (BreakDownTrainingLibrary1Button, "Press this button to see prediction break-down, ex: probability of each possible class.");
      toolTip.SetToolTip (Lib1Pred1Class,                  "Prediction made by current classifier.");
      toolTip.SetToolTip (Lib1Pred1Prob,                   "Probability assigned by current active classifier.");
      toolTip.SetToolTip (Lib1Pred1Votes,                  "Number of votes current classifier made for prediction.");
      toolTip.SetToolTip (VerifiedClass,                   "Class that user has verified this image belongs to.");
      toolTip.SetToolTip (UpdateTrainingLibrayButton,      "Press this button to add this image to the training library for currently active classifier.");
      toolTip.SetToolTip (AddNewClassButtom,               "Bring up Add-New-Class dialog to add a new class to training model.");
      toolTip.SetToolTip (OkButton,                        "Press this when done viewing this image.");
    }




    public  void  LengthComputer (UmiShrimpLengthComputer  _lengthComputer) 
    {
      lengthComputer = _lengthComputer;
    }


    private void MakePredictions()
    {
      if  (!ActiveTrainingLibraries.ModelsAreAvailable ())
      {
        // Since there are NO training models loaded;  there is no point doing the work for a prediction.
        return;
      }

      UmiPredictionList  predictions1 = null;

      ActiveTrainingLibraries.MakePredictions (raster, fileName, ref predictions1, runLog);

      if  ((predictions1 != null)  &&  (predictions1.Count > 1))
      {
        Lib1Pred1Class.Text = predictions1[0].ClassName;
        Lib1Pred1Prob.Text  = predictions1[0].Probability.ToString ("##0.00%");
        Lib1Pred1Votes.Text = predictions1[0].Votes.ToString ("#,##0");
      }

      return;
    }  /* MakePredictions */

    

    private  void  GetZoomFactor ()
    {
      float  origZoomFactor = zoomFactor;
      bool  divideBy100 = false;
      String s = ZoomFactor.Text;
      if  (s[s.Length - 1] == '%')
      {
        s = s.Substring (0, s.Length - 1);
        divideBy100 = true;
      }

      zoomFactor = UmiKKStr.StrToFloat (s);
      if  (zoomFactor <= 0.0f)
      {
        zoomFactor = origZoomFactor;
        divideBy100 = false;
      }

      if  (divideBy100)
        zoomFactor = zoomFactor / 100.0f;

      if  (zoomFactor <= 0.0f)
        zoomFactor = 1.0f;

      int  largestDim = Math.Max (raster.Height, raster.Width);
      int  zoomedLargestDim = (int)((float)largestDim * zoomFactor + 0.5f);

      if  (zoomedLargestDim > 4096)
      {
        zoomFactor = 4096.0f / (float)(zoomedLargestDim + 1);
      }

      int  smallestDim = Math.Min (raster.Height, raster.Width);
      int  zoomedSmalestDim = (int)((float)smallestDim * zoomFactor + 0.5f);
      if  (zoomedSmalestDim < 3)
      {
        zoomFactor = 3.0f / (float)(zoomedSmalestDim + 1);
      }

      ZoomFactor.Text = zoomFactor.ToString ("##0.0%");
    }  /* GetZoomFactor */




    private  Bitmap  ReSize (Bitmap  src,
                             float   factor
                            )
    {
      int nWidth   = (int)((float)src.Width  * factor + 0.5f);
      int nHeight  = (int)((float)src.Height * factor + 0.5f);

      Bitmap result = new Bitmap (nWidth, nHeight);
      Graphics g = Graphics.FromImage( (Image) result );
      g.DrawImage (src, 0, 0, nWidth, nHeight );
      return result;
    }




    private  void  LoadSubjectImage (bool  computeLength)
    {
      if  (particleEntry.Length > 0.0f)
      {
        LengthInMM.Text = particleEntry.Length.ToString ("##,##0.0") + " mm";
        OrientationAngle.Text = particleEntry.Orientation.ToString ("##0.0") + " deg";
      }

      GetZoomFactor ();

      UmiRaster  srcRaster = raster;

      if  (computeLength  &&  (lengthComputer != null))
      {
        lengthComputer.ProcessImage (raster, true);
        
        UmiRaster  demoImage = lengthComputer.GetDemoImage ();
        if  (demoImage == null)
        {
          if  (lengthComputer.ImageIsOccluded)
            LengthInMM.Text = "Occluded";
          
          else if  (lengthComputer.ImageIsDistorted)
            LengthInMM.Text = "Distorted";
          
          else
            LengthInMM.Text = "";

          OrientationAngle.Text = "";
        }
        else
        {
          srcRaster = demoImage;
          LengthInMM.Text = lengthComputer.LenInMiliMeters.ToString ("##,##0.0") + " mm";

          float  oa = lengthComputer.OrientationAngleInDegrees;
          OrientationAngle.Text = oa.ToString ("##0.0") + " deg";
        }
      }

      Bitmap  bm = srcRaster.Transpose ().BuildBitmap ();

      if  (zoomFactor != 1.0f)
        bm = ReSize (bm, zoomFactor);

      {
        // Lets add hash marks.
        Graphics  g = Graphics.FromImage (bm);
        float  zoomedPixelsPerMM = zoomFactor * pixelsPerMM;
        int  x = 0;
        Brush  hashMarkBrush = Brushes.Red;
        Pen    gridPen   = new Pen (Color.FromArgb (30, 255, 0, 0));

        // Vertical Hash Marks
        while  (true)
        {
          int  hashPos = (int)((float)x * zoomedPixelsPerMM + 0.5f);
          if  (hashPos >= bm.Height - 4)
            break;

          int hashLen = 4;
          if  ((x % 10) == 0)
            hashLen = 8;
          
          g.FillRectangle (hashMarkBrush, 0, hashPos, hashLen, 2);
          g.FillRectangle (hashMarkBrush, bm.Width - (hashLen + 1), hashPos, hashLen, 2);
          if  (displayGrid)
            g.DrawLine (gridPen, 0, hashPos, bm.Width - 1, hashPos);
          x++;
        }

        x = 0;

        // Horizontal Hash Marks
        while  (true)
        {
          int  hashPos = (int)((float)x * zoomedPixelsPerMM + 0.5f);
          if  (hashPos >= bm.Width)
            break;

          int hashLen = 4;
          if  ((x % 10) == 0)
            hashLen = 8;
          
          g.FillRectangle (hashMarkBrush, hashPos, 0, 2, hashLen);
          g.FillRectangle (hashMarkBrush, hashPos, bm.Height - (hashLen + 1), 2, hashLen);
          if  (displayGrid)
            g.DrawLine (gridPen, hashPos, 0, hashPos, bm.Height - 1);
          x++;
        }
      }

      SubjectImage.Height = bm.Height;
      SubjectImage.Width  = bm.Width;

      SubjectImage.Image = bm;
    }  /* LoadSubjectImage */


    
    private  void  LoadTrainingModel1 (String  modelName,
                                       bool    forceRebuild
                                      )
    {
      TrainingModelName.Enabled = false;
      RebuildTrainModelButton.Enabled = false;
      ActiveTrainingLibraries.SetModel1 (modelName, forceRebuild, Model1AlertCompletion);
    }  /* LoadTrainingModel1 */



    void  Model1AlertCompletion (UmiTrainingModel2  sender)
    {
      if  ((sender == null)  ||  (!sender.Valid))
      {
        TrainingModelName.Text = "";
      }

      TrainingModelName.Text = trainingModel.ModelName;
      TrainingModelName.Enabled = true;
      RebuildTrainModelButton.Enabled = true;

      MakePredictions ();
    }  /* Model1AlertCompletion*/



    private void  ImageVerifier_Load (object sender, EventArgs e)
    {
      InitializeScreenFields ();

      lastHeight = Height;
      lastWidth  = Width;

      leftPanelLastHeight = BottomPanel.Height;
      leftPanelLastWidth  = BottomPanel.Width;

      // We were going to rotate image so Width and Height must me swapped when comparing with window size.
      float  factorHeight = (float)SubjectImage.Height / (float)(raster.Width  + 1);
      float  factorWidth  = (float)SubjectImage.Width  / (float)(raster.Height + 1);

      float  smallestFactor = Math.Min (factorHeight, factorWidth);
      if  (smallestFactor < 1.0f)
      {
        zoomFactor = smallestFactor;
        ZoomFactor.Text = smallestFactor.ToString ("##0.0%");
      }

      MakePredictions ();

      LoadSubjectImage (false);

      // Because the raster will be transposed before we display it;  we need to compare Raster.Height with Panel.Width
      // Raster.Width with Panel.Height.
      if  (raster.Height < (BottomPanel.Width - 20))
      {
        int widthReduction = (BottomPanel.Width - 20) - raster.Height;
        int newWidth = this.Width - widthReduction;

        if  (newWidth < this.MinimumSize.Width)
          newWidth = this.MinimumSize.Width;

        Width = newWidth;
      }


      if  (raster.Width < (BottomPanel.Height - 20))
      {
        int heightReduction = (BottomPanel.Height - 20) - raster.Width;
        int newHeight = this.Height - heightReduction;

        if  (newHeight < this.MinimumSize.Height)
          newHeight = this.MinimumSize.Height;

        Height = newHeight;
      }

    }  /* ImageViewer_Load*/



    private void ImageVerifier_Resize (object sender, EventArgs e)
    {
      int  deltaHeight = Height - lastHeight;
      int  deltaWidth  = Width  - lastWidth;

      BottomPanel.Height += deltaHeight;
      BottomPanel.Width  += deltaWidth;

      TopPanel.Width     += deltaWidth;

      OkButton.Top       += deltaHeight;
      OkButton.Left      += (deltaWidth / 2);

      lastWidth  = Width;
      lastHeight = Height;
    }



    private void LeftPanel_SizeChanged (object sender, EventArgs e)
    {
      int  deltaHeight = BottomPanel.Height - leftPanelLastHeight;
      int  deltaWidth  = BottomPanel.Width  - leftPanelLastWidth;

      SubjectImage.Height += deltaHeight;
      SubjectImage.Width  += deltaWidth;

      leftPanelLastHeight = BottomPanel.Height;
      leftPanelLastWidth  = BottomPanel.Width;

      LoadSubjectImage (false);
    }



    private void ZoomFactor_SelectedIndexChanged(object sender, EventArgs e)
    {
      ZoomFactor.Text = ZoomFactor.Items[ZoomFactor.SelectedIndex].ToString ();
      LoadSubjectImage (false);
    }



    private void TrainingLibrary1_SelectedIndexChanged (object sender, EventArgs e)
    {
      String  newTrainingLibraryName = TrainingModelName.Text;
      if  (newTrainingLibraryName != ActiveTrainingLibraries.Model1Name)
        LoadTrainingModel1 (newTrainingLibraryName, false);
    }


    private void RebuildTrainLibrary1Button_Click(object sender, EventArgs e)
    {
      if  (!String.IsNullOrEmpty (TrainingModelName.Text))
      {
        runLog.WriteLn (10, "ImageViewer.RebuildTrainLibrary1Button_Click");
        LoadTrainingModel1 (TrainingModelName.Text, true);
      }
    }


    private void ZoomFactor_Leave(object sender, EventArgs e)
    {
      float  oldZoomFactor = zoomFactor;
      GetZoomFactor ();
      if  (zoomFactor != oldZoomFactor)
        LoadSubjectImage (false);
    } /* ZoomFactor_Leave */  


    private void BreakDownTrainingLibrary1Button_Click (object sender, EventArgs e)
    {
      PredictionBreakDownDisplay  pbdd = new PredictionBreakDownDisplay (raster, ActiveTrainingLibraries.Model1 (), runLog);
      pbdd.ShowDialog ();
      pbdd = null;
    }


    private void DisplayGridButton_Click(object sender, EventArgs e)
    {
      displayGrid = !displayGrid;
      LoadSubjectImage (false);
    }


    private void ImageVerifier_FormClosing (object sender, FormClosingEventArgs e)
    {
    }


    private  void  AddRasterImageToTrainingLibrary (String  className)
    {
      String  trainLibraryRootDir = trainingModel.RootDirExpanded;
      String  rootName            = UmiOSservices.GetRootName (fileName);
      String  rootNameWithExt     = rootName + ".bmp";

      bool  saveIntoTrainigLibrary = true;
      String  existingFileDir = UmiOSservices.LookForFile (rootNameWithExt, trainLibraryRootDir);
      if  (!String.IsNullOrEmpty (existingFileDir))
      {
        String  prevClassName = UmiOSservices.GetRootNameOfDirectory (existingFileDir);
        if  (String.Equals (prevClassName, className, StringComparison.CurrentCultureIgnoreCase))
        {
          saveIntoTrainigLibrary = false;

          DialogResult  dr = 
            MessageBox.Show (this,
                             "This example already in Training Library; do you want to overwrite existing example  Yes/No?", 
                             "Update Training Library", 
                             MessageBoxButtons.YesNo
                            );
          if  (dr == DialogResult.Yes)
          {
            saveIntoTrainigLibrary = true;
            String  fullNameOfExistingFile = UmiOSservices.AddSlash (existingFileDir) + rootNameWithExt;
            try  {System.IO.File.Delete (fullNameOfExistingFile);}  catch  (Exception)  {}
          }
        }
        else
        {
          DialogResult dr = 
            MessageBox.Show (this, 
                             "This example already exists in Class[" + prevClassName + "]" + "\n\n" + "Do you want to move to new Class Yes/No?", 
                             "Update Training Library", 
                             MessageBoxButtons.YesNo
                            );
          if  (dr == DialogResult.No)
          {
            saveIntoTrainigLibrary = false;
          }
          else
          {
            String  fullNameOfExistingFile = UmiOSservices.AddSlash (existingFileDir) + rootNameWithExt;
            try  {System.IO.File.Delete (fullNameOfExistingFile);}  catch  (Exception)  {}
            runLog.WriteLn (10, "ImageVerifier   Deleting from  Training-Library: " + trainingModel.ModelName  + "  Class: " + prevClassName + "  File-Name: " + rootName);
            saveIntoTrainigLibrary = true;
          }
        }
      }

      if  (saveIntoTrainigLibrary)
      {
         String  classRootDir = UmiOSservices.AddSlash (trainLibraryRootDir) +  className;
         UmiOSservices.CreateDirectoryPath (classRootDir);
         String  fullFileName = UmiOSservices.AddSlash (classRootDir) + fileName;
         raster.Save (fullFileName);
         runLog.WriteLn (10, "ImageVerifier   Adding image to Training-Library : " + trainingModel.ModelName  + "  Class :" + className + "  File-Name: " + rootName);
      }
    }  /* AddRasterImageToTrainingLibrary */



    private void UpdateTrainingLibrayButton_Click (object sender, EventArgs e)
    {
      if  (trainingModel == null)
        return;

      String  className = (String)VerifiedClass.SelectedItem;
      if  (String.IsNullOrEmpty (className))
      {
        MessageBox.Show (this, "No class specified", "Update Training Library");
      }
      else
      {
        AddRasterImageToTrainingLibrary (className);
      }
    }



    private void VerifiedClass_SelectedIndexChanged (object sender, EventArgs e)
    {
      classUserValidatesAs = UmiClassList.GetUniqueClass ((String)VerifiedClass.SelectedItem, "");
    }



    private void OkButton_Click (object sender, EventArgs e)
    {
      okPressed = true;
      Close ();
    }


    private void ComputeLengthButton_Click (object sender, EventArgs e)
    {
      LoadSubjectImage (true);
    }


    private void AddNewClassButtom_Click (object sender, EventArgs e)
    {
      AddAClass aac = new AddAClass (trainingModel, particleEntry, runLog);
      aac.ShowDialog (this);
      if  (aac.NewClassWasAdded)
      {
        classUserValidatesAs = particleEntry.PredClass;

        AddRasterImageToTrainingLibrary (classUserValidatesAs.Name);

        UpdateVerifiedClassList ();

        RebuildTrainLibrary1Button_Click (null, null);
      }
    }

  }  /* ImageVerifier*/
}
