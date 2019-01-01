using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

using CounterManaged;


namespace CounterManagedRoutines
{
  public partial class ImageViewer : Form
  {
    private  UmiRaster         raster          = null;
    private  UmiFeatureVector  featureVector   = null;
    private  UmiRunLog         runLog          = null;

    private  UmiClass          classUserValidatesAs = null;  // Will only be set if USER validates during this Dialog session.

    private  int      lastHeight = 0;
    private  int      lastWidth  = 0;

    private  int      leftPanelLastHeight = 0;
    private  int      leftPanelLastWidth  = 0;

    private  int      rightPanelLastHeight = 0;
    private  int      rightPanelLastWidth  = 0;

    private  float    zoomFactor = 1.0f;

    private  bool     displayGrid = false;

    private  bool     saveDebugImages = false;

    
    public  UmiClass  ClassUserValidatesAs  {get {return classUserValidatesAs;}}




    public ImageViewer (UmiRaster     _raster,
                        bool          _saveDebugImages,
                        UmiRunLog     _runLog
                       )
    {
      raster  = _raster;
      runLog  = _runLog;
      saveDebugImages = _saveDebugImages;

      if  (raster == null)
      {
        MessageBox.Show ("The Raster image was not provoded.");
        return;
      }

      InitializeComponent();

      ExampleFileName.Text = raster.FileName;
    }



    private void  InitializeScreenFields ()
    {
      String[]  trainingModels = UmiTrainingModel2.GetListOfTrainingModels ();
      if  (trainingModels != null)
      {
        foreach  (String modelName in trainingModels)
        {
          TrainingLibrary1.Items.Add (modelName);
          TrainingLibrary2.Items.Add (modelName);
        }
      }

      if  (!String.IsNullOrEmpty (ActiveTrainingLibraries.Model1Name))
        TrainingLibrary1.Text = ActiveTrainingLibraries.Model1Name;

      if  (!String.IsNullOrEmpty (ActiveTrainingLibraries.Model2Name))
        TrainingLibrary2.Text = ActiveTrainingLibraries.Model2Name;

    }  /* PopulateClassChooser */




    private void MakePredictions()
    {
      if  (!ActiveTrainingLibraries.ModelsAreAvailable ())
      {
        // Since there are NO training models loaded;  there is no point doing the work for a prediction.
        return;
      }

      UmiPredictionList  predictions1 = null;
      UmiPredictionList  predictions2 = null;

      ActiveTrainingLibraries.MakePredictions (featureVector, ref predictions1, ref predictions2, runLog);

      if  ((predictions1 != null)  &&  (predictions1.Count > 1))
      {
        Lib1Pred1Class.Text = predictions1[0].ClassName;
        Lib1Pred1Prob.Text  = predictions1[0].Probability.ToString ("##0.00%");
        Lib1Pred1Votes.Text = predictions1[0].Votes.ToString ("#,##0");

        Lib1Pred2Class.Text = predictions1[1].ClassName;
        Lib1Pred2Prob.Text  = predictions1[1].Probability.ToString ("##0.00%");
        Lib1Pred2Votes.Text = predictions1[1].Votes.ToString ("#,##0");
      }

      if  ((predictions2 != null)  &&  (predictions2.Count > 1))
      {
        Lib2Pred1Class.Text = predictions2[0].ClassName;
        Lib2Pred1Prob.Text  = predictions2[0].Probability.ToString ("##0.00%");
        Lib2Pred1Votes.Text = predictions2[0].Votes.ToString ("#,##0");

        Lib2Pred2Class.Text = predictions2[1].ClassName;
        Lib2Pred2Prob.Text  = predictions2[1].Probability.ToString ("##0.00%");
        Lib2Pred2Votes.Text = predictions2[1].Votes.ToString ("#,##0");
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




    private  void  LoadPlanktonImage ()
    {
      GetZoomFactor ();
      Bitmap  bm = raster.BuildBitmap ();

      if  (zoomFactor != 1.0f)
        bm = ReSize (bm, zoomFactor);

      {
        // Lets add hash marks.
        Graphics  g = Graphics.FromImage (bm);
        float  pixesPerMM = zoomFactor * (1.0f / 0.034f);
        int  x = 0;
        Brush  hashMarkBrush = Brushes.Red;
        Pen    gridPen   = new Pen (Color.FromArgb (30, 255, 0, 0));

        // Verticle Hash Marks
        while  (true)
        {
          int  hashPos = (int)((float)x * pixesPerMM + 0.5f);
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
          int  hashPos = (int)((float)x * pixesPerMM + 0.5f);
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

      PlanktonImage.Height = bm.Height;
      PlanktonImage.Width  = bm.Width;

      PlanktonImage.Image = bm;
    }  /* LoadPlanktonImage */


    
    private  void  LoadTrainingModel1 (String  modelName,
                                       bool    forceRebuild
                                      )
    {
      TrainingLibrary1.Enabled = false;
      TrainingLibrary2.Enabled = false;
      RebuildTrainLibrary1Button.Enabled = false;
      RebuildTrainLibrary2Button.Enabled = false;
      ActiveTrainingLibraries.SetModel1 (modelName, forceRebuild, Model1AlertCompletion);
    }  /* LoadTrainingModel1 */



    void  Model1AlertCompletion (UmiTrainingModel2  sender)
    {
      if  ((sender == null)  ||  (!sender.Valid))
      {
        TrainingLibrary1.Text = "";
      }

      TrainingLibrary1.Enabled = true;
      TrainingLibrary2.Enabled = true;
      RebuildTrainLibrary1Button.Enabled = true;
      RebuildTrainLibrary2Button.Enabled = true;

      MakePredictions ();
    }  /* Model1AlertCompletion*/




    private  void  LoadTrainingModel2 (String  modelName,
                                       bool    forceRebuild
                                      )
    {
      TrainingLibrary1.Enabled = false;
      TrainingLibrary2.Enabled = false;
      RebuildTrainLibrary1Button.Enabled = false;
      RebuildTrainLibrary2Button.Enabled = false;
      ActiveTrainingLibraries.SetModel2 (modelName, forceRebuild, Model1AlertCompletion);
    }  /* LoadTrainingModel2 */

    


    void  Model2AlertCompletion (UmiTrainingModel2 sender)
    {
      if  ((sender == null)  ||  (!sender.Valid))
      {
        TrainingLibrary2.Text = "";
      }

      TrainingLibrary1.Enabled = true;
      TrainingLibrary2.Enabled = true;
      RebuildTrainLibrary1Button.Enabled = true;
      RebuildTrainLibrary2Button.Enabled = true;
      
      MakePredictions ();
    }  /* Model2AlertCompletion*/

        




    private void  ImageViewer_Load (object sender, EventArgs e)
    {
      InitializeScreenFields ();

      lastHeight = Height;
      lastWidth  = Width;

      leftPanelLastHeight = LeftPanel.Height;
      leftPanelLastWidth  = LeftPanel.Width;

      rightPanelLastHeight = RightPanel.Height;
      rightPanelLastWidth  = RightPanel.Width;

      float  factorHeight = (float)PlanktonImage.Height / (float)(raster.Height + 1);
      float  factorWidth  = (float)PlanktonImage.Width  / (float)(raster.Width  + 1);

      float  smallestFactor = Math.Min (factorHeight, factorWidth);
      if  (smallestFactor < 1.0f)
      {
        zoomFactor = smallestFactor;
        ZoomFactor.Text = smallestFactor.ToString ("##0.0%");
      }

      MakePredictions ();

      LoadPlanktonImage ();

      if  (raster.Width < (LeftPanel.Width - 20))
      {
        int widthReduction = (LeftPanel.Width - 20) - raster.Width;
        int newWidth = this.Width - widthReduction;

        if  (newWidth < this.MinimumSize.Width)
          newWidth = this.MinimumSize.Width;

        Width = newWidth;
      }


      if  (raster.Height < (LeftPanel.Height - 20))
      {
        int heightReduction = (LeftPanel.Height - 20) - raster.Height;
        int newHeight = this.Height - heightReduction;

        if  (newHeight < this.MinimumSize.Height)
          newHeight = this.MinimumSize.Height;

        Height = newHeight;
      }

    }  /* ImageViewer_Load*/



    private void ImageViewer_Resize (object sender, EventArgs e)
    {
      int  deltaHeight = Height - lastHeight;
      int  deltaWidth  = Width  - lastWidth;

      LeftPanel.Height   += deltaHeight;
      RightPanel.Height  += deltaHeight;

      LeftPanel.Width += deltaWidth;
      RightPanel.Left += deltaWidth;

      lastWidth = Width;
      lastHeight = Height;
    }



    private void LeftPanel_SizeChanged (object sender, EventArgs e)
    {
      int  deltaHeight = LeftPanel.Height - leftPanelLastHeight;
      int  deltaWidth  = LeftPanel.Width  - leftPanelLastWidth;

      PlanktonImage.Height += deltaHeight;
      PlanktonImage.Width  += deltaWidth;

      leftPanelLastHeight = LeftPanel.Height;
      leftPanelLastWidth  = LeftPanel.Width;

      LoadPlanktonImage ();
    }



    private void RightPanel_SizeChanged(object sender, EventArgs e)
    {
      int  deltaHeight = RightPanel.Height - rightPanelLastHeight;
      int  deltaWidth  = RightPanel.Width  - rightPanelLastWidth;

      rightPanelLastHeight = RightPanel.Height;
      rightPanelLastWidth  = RightPanel.Width;
    }



    private void ZoomFactor_SelectedIndexChanged(object sender, EventArgs e)
    {
      ZoomFactor.Text = ZoomFactor.Items[ZoomFactor.SelectedIndex].ToString ();
      LoadPlanktonImage ();
    }



    private void TrainingLibrary1_SelectedIndexChanged (object sender, EventArgs e)
    {
      String  newTrainingLibraryName = TrainingLibrary1.Text;
      if  (newTrainingLibraryName != ActiveTrainingLibraries.Model1Name)
        LoadTrainingModel1 (newTrainingLibraryName, false);
    }



    private void RebuildTrainLibrary1Button_Click(object sender, EventArgs e)
    {
      if  (!String.IsNullOrEmpty (TrainingLibrary1.Text))
      {
        LoadTrainingModel1 (TrainingLibrary1.Text, true);
      }
    }


    private void TrainingLibrary2_SelectedIndexChanged(object sender, EventArgs e)
    {
      String  newTrainingLibraryName = TrainingLibrary2.Text;
      if  (newTrainingLibraryName != ActiveTrainingLibraries.Model2Name)
        LoadTrainingModel2 (newTrainingLibraryName, false);
    } 



    private void RebuildTrainLibrary2Button_Click(object sender, EventArgs e)
    {
      if  (!String.IsNullOrEmpty (TrainingLibrary2.Text))
      {
        LoadTrainingModel2 (TrainingLibrary2.Text, true);
      }
    }




    private void ImageViewer_FormClosing (object sender, FormClosingEventArgs e)
    {
    }



    private void ZoomFactor_Leave(object sender, EventArgs e)
    {
      float  oldZoomFactor = zoomFactor;
      GetZoomFactor ();
      if  (zoomFactor != oldZoomFactor)
        LoadPlanktonImage ();
    } /* ZoomFactor_Leave */  


    private void BreakDownTrainingLibrary1Button_Click (object sender, EventArgs e)
    {
      PredictionBreakDownDisplay  pbdd = new PredictionBreakDownDisplay (raster, ActiveTrainingLibraries.Model1 (), saveDebugImages, runLog);
      pbdd.ShowDialog ();
      pbdd = null;
    }


    private void BreakDownTrainingLibrary2Button_Click (object sender, EventArgs e)
    {
      PredictionBreakDownDisplay  pbdd = new PredictionBreakDownDisplay (raster, ActiveTrainingLibraries.Model2 (), saveDebugImages, runLog);
      pbdd.ShowDialog ();
      pbdd = null;
    }

    
    private void DisplayGridButton_Click(object sender, EventArgs e)
    {
      displayGrid = !displayGrid;
      LoadPlanktonImage ();
    }  /* DataLabel_MouseClick */

  }  /* ImageViewer*/
}
