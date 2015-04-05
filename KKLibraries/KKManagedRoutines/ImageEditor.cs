using System;
using System.IO;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Drawing.Imaging;
using System.Text;
using System.Windows.Forms;

using LarcosCounterManaged;


namespace KKManagedRoutines
{
  public partial class ImageEditor : Form
  {
    Color           backGroundColor = Color.FromArgb (255, 255, 255);
    Pen[]           colorValues    = null;
    bool            colorize       = false;
    string          fileName       = null;
    int             height         = 0;
    Bitmap          image          = null;
    
    float           flowRateFactor = 1.0f;
    float           sizeRatio      = 1.0f;
    byte[][]        raster         = null;
    int             width;

    float           lowerBound = 0.0f;
    float           upperBound = 1.0f;

    uint            thLowerBound = 0;
    uint            thUpperBound = 255;

    uint            backGroundTH = 31;

    String          imageFileName  = "";

    UmiRunLog       runLog = new UmiRunLog ();


    private  class  Operation
    {
      UmiRaster.OperationType  operation    = UmiRaster.OperationType.Null;
      UmiRaster.MaskType       mask         = UmiRaster.MaskType.SQUARE3;
      int                      maskSize     = 3;
      float                    lowerBound   = 0.0f;
      float                    upperBound   = 1.0f;
      uint                     thLowerBound = 0;
      uint                     thUpperBound = 255;
      uint                     backGroundTH = 31;


      public  Operation (UmiRaster.OperationType  _op,
                         uint                     _backGroundTH
                        )
      {
        operation    = _op;
        backGroundTH = _backGroundTH;
      }


      public  Operation (UmiRaster.OperationType  _op,
                         UmiRaster.MaskType       _mt,
                         uint                     _backGroundTH
                        )
      {
        operation    = _op;
        mask         = _mt;
        maskSize     = UmiRaster.MaskSize (_mt);
        backGroundTH = _backGroundTH;
      }

      
      public  Operation (UmiRaster.OperationType  _op,
                         UmiRaster.MaskType       _mt,
                         float                    _lowerBound,
                         float                    _upperBound,
                         uint                     _backGroundTH
                        )
      {
        operation    = _op;
        mask         = _mt;
        maskSize     = UmiRaster.MaskSize (_mt);
        lowerBound   = _lowerBound;
        upperBound   = _upperBound;
        backGroundTH = _backGroundTH;
      }


      public  Operation (UmiRaster.OperationType  _op,
                         UmiRaster.MaskType       _mt,
                         uint                     _thLowerBound,
                         uint                     _thUpperBound,
                         uint                     _backGroundTH
                        )
      {
        operation  = _op;
        mask       = _mt;
        maskSize   = UmiRaster.MaskSize (_mt);
        thLowerBound = _thLowerBound;
        thUpperBound = _thUpperBound;
        backGroundTH = _backGroundTH;
      }


      public  override  String  ToString ()
      {
        String  s = UmiRaster.OperationTypeToStr (operation);

        switch  (operation)
        {
          case  UmiRaster.OperationType.Closing:  
          case  UmiRaster.OperationType.Dilation:
          case  UmiRaster.OperationType.Erosion:
          case  UmiRaster.OperationType.Opening:
            {
              s =  s.PadRight (12);
              s += UmiRaster.MaskTypeToStr (mask);
              s += (", bg(" + backGroundTH + ")");
            }
            break;

          case  UmiRaster.OperationType.SmoothAveraging:
          case  UmiRaster.OperationType.SmoothMedium:
            {
              s = s.PadRight (12);
              s += UmiRaster.MaskSize (mask);
            }
            break;

          case  UmiRaster.OperationType.BandPass:
            {
              s = s.PadRight (12);
              s += ((float)(lowerBound * 100.0)).ToString ("##0.0") + " - " + 
                   ((float)(upperBound * 100.0)).ToString ("##0.0");
            }
            break;


          case  UmiRaster.OperationType.BinarizeTH:
            {
              s = s.PadRight (12);
              s += thLowerBound.ToString ("##0.0") + " - " + thUpperBound.ToString ("##0.0");
            }
            break;

          case UmiRaster.OperationType.ConnectedComponent:
            {
              s = s.PadRight (12);
              s += UmiRaster.MaskTypeToStr (mask);
              s += (", bg(" + backGroundTH + ")");
            }
            break;
        }

        return  s;
      }  /* ToString */



      public  UmiRaster  PerformOperation (UmiRaster  r,
                                           UmiRunLog  runLog
                                          )
      {
        UmiRaster  result = null;
        r.BackgroundPixelTH = (byte)backGroundTH;
        switch  (operation)
        {
          case  UmiRaster.OperationType.BandPass:           result = r.BandPass            (lowerBound,   upperBound, true); break;
          case  UmiRaster.OperationType.Binarize:           result = r.Binarize            ();             break;
          case  UmiRaster.OperationType.BinarizeTH:         result = r.BinarizeByThreshold (thLowerBound, thUpperBound); break;
          case  UmiRaster.OperationType.Closing:            result = r.CloseImage          (mask);         break;
          case  UmiRaster.OperationType.ConnectedComponent: result = r.ConnectedComponent  ();             break;
          case  UmiRaster.OperationType.ConvexHull:         result = r.ConvexHull          ();             break;
          case  UmiRaster.OperationType.Dilation:          result = r.DilateImage        (mask);         break;
          case  UmiRaster.OperationType.Edge:               result = r.EdgeImage           ();             break;
          case  UmiRaster.OperationType.Erosion:            result = r.ErodeImage          (mask);         break;
          case  UmiRaster.OperationType.FillHoles:          result = r.FillHoles           ();             break;
          case  UmiRaster.OperationType.FourierTransform:   result = r.FourierTransform    ();             break;
          case  UmiRaster.OperationType.MaskExclude:        result = r.MaskExclude         (mask);         break;
          case  UmiRaster.OperationType.Opening:            result = r.OpenImage           (mask);         break;
          case  UmiRaster.OperationType.SmoothAveraging:    result = r.SmoothAveraging     (maskSize);     break;
          case  UmiRaster.OperationType.SmoothMedium:       result = r.SmoothMedium        (maskSize);     break;
          case  UmiRaster.OperationType.Thinning:           result = r.Thinning            ();             break;

        }  /* operation */

        return  result;
      }  /* PerformOperation */
    }


    List<Operation>  operations = new List<Operation> ();




    public  ImageEditor (String _fileName)
    {
      InitializeComponent();

      imageFileName = _fileName;

      UmiRaster  pr = null;
      try
      {
        pr = new UmiRaster (_fileName);
      }
      catch (Exception e)
      {
        pr = null;
        throw new Exception ("ImageEditor", e);
     }

      raster         = pr.BuildRasterArray2 ();
      fileName       = _fileName;
      height         = pr.Height;
      width          = pr.Width;

      pr = null;

      BuildColorValues ();

      Scale.SelectedItem = "100%";

      image = BuildBitmapFromRasterData (raster); 
      
      pictureBox1.Height = image.Height;
      pictureBox1.Width  = image.Width;
      pictureBox1.Image  = image;

      ImagePanel.AutoScroll = true;
      //ImagePanel.SetStyle   (ControlStyles.ResizeRedraw, false);
      ImagePanel.AutoScroll = true;
      ImagePanel.AutoScrollMinSize = image.Size;
      ImagePanel.Invalidate();

      MakePredictions ();
    }
  



    public  ImageEditor (UmiRaster  _raster)
    {
      InitializeComponent();

      //raster         = (byte[][])_raster.BuildRasterArray ();
      raster         = _raster.BuildRasterArray2 ();
      imageFileName  = _raster.FileName;

      height         = _raster.Height;
      width          = _raster.Width;

      _raster = null;

      BuildColorValues ();

      Scale.SelectedItem = "100%";

      image = BuildBitmapFromRasterData (raster);
      
      pictureBox1.Height = image.Height;
      pictureBox1.Width  = image.Width;
      pictureBox1.Image  = image;

      ImagePanel.AutoScroll = true;
      //ImagePanel.SetStyle   (ControlStyles.ResizeRedraw, false);
      ImagePanel.AutoScroll = true;
      ImagePanel.AutoScrollMinSize = image.Size;
      ImagePanel.Invalidate();

      MakePredictions ();
    }
  



    public ImageEditor (string    _fileName,
                        int       _height,
                        int       _width,
                        byte[][]  _raster   // Will take ownership of '_raster'
                       )
    {
      InitializeComponent();
      
      fileName = _fileName;
      raster   = _raster;
      height   = _height;
      width    = _width;

      imageFileName = _fileName;;

      BuildColorValues ();

      Scale.SelectedItem = "100%";

      image = BuildBitmapFromRasterData (raster);
      
      pictureBox1.Height = image.Height;
      pictureBox1.Width  = image.Width;
      pictureBox1.Image  = image;

      ImagePanel.AutoScroll = true;
      //ImagePanel.SetStyle   (ControlStyles.ResizeRedraw, false);
      ImagePanel.AutoScroll = true;
      ImagePanel.AutoScrollMinSize = image.Size;
      ImagePanel.Invalidate();

      MakePredictions ();
    }



    private  void  AddOperation (UmiRaster.OperationType  _operation)
    {
      UmiRaster.MaskType  mt = UmiRaster.MaskTypeFromStr (MaskTypesComboBox.SelectedItem.ToString ());

      backGroundTH = (uint)BackGroundTH.Value;

      if  (_operation == UmiRaster.OperationType.BandPass)
        operations.Add (new Operation (_operation, mt, lowerBound, upperBound, backGroundTH));

      else if  (_operation == UmiRaster.OperationType.BinarizeTH)
        operations.Add (new Operation (_operation, mt, thLowerBound, thUpperBound, backGroundTH));

      else
        operations.Add (new Operation (_operation, mt, backGroundTH));
      UpdateOperationsList ();
      RePaintImage ();
    }  /* AddOperation */



    private  void  UndoOperation ()
    {
      if  (operations.Count < 1)
        return;
      
      Operation  lastOperation = operations[operations.Count - 1];
      operations.RemoveAt (operations.Count - 1);
      lastOperation = null;

      UpdateOperationsList ();

      RePaintImage ();
    }  /* UndoOperation */



    private  void  UpdateOperationsList ()
    {
      OperationsList.Items.Clear ();
      foreach  (Operation op in operations)
      {
        OperationsList.Items.Add (op);
      }
    }  /* UpdateOperationsList */




    private  byte[][]  PerformAllOperations (byte[][]  startRaster)
    {
      UmiRaster  lastRaster = new UmiRaster (startRaster);

      foreach  (Operation o in  operations)
      {
        UmiRaster  nextRaster = o.PerformOperation (lastRaster, runLog);
        if  (nextRaster == null)
        {
          MessageBox.Show ("Morphological operation[" + o.ToString () + "] failed.");
          break;
        }
        lastRaster = nextRaster;
      }

      byte[][]  finalResult = null;

      try
      {
        finalResult = lastRaster.BuildRasterArray2 ();
      }
      catch  (Exception)
      {
        // Lets try one more time.
        finalResult = lastRaster.BuildRasterArray2 ();
        MessageBox.Show ("PerformAllOperations     Called  'BuildRasterArray2'  Two Times.");
      }

      lastRaster = null;

      return  finalResult;
    }  /* PerformAllOperations */





    private  void  MakePredictions  ()
    {
      pred1ClassName.Text = "";
      lib2Pred1ClassName.Text = "";

      BlobDetector  bd = new BlobDetector (raster, 20);
      BlobList  blobs = bd.ExtractBlobs ();

      if  ((blobs == null)  ||  (blobs.Count < 1))
        return;

      Blob  largestBlob = blobs.LocateLargestBlob ();
      if  (largestBlob == null)
        return;

      byte[][]  largestRaster = bd.ExtractedBlobImage (largestBlob);

      UmiRunLog  runLog = new UmiRunLog ();
      UmiRaster  larcosRaster = new UmiRaster (largestRaster);

      UmiPredictionList  trainLibrary1Predictions = null;
      UmiPredictionList  trainLibrary2Predictions = null;

      ActiveTrainingLibraries.MakePredictions (larcosRaster,
                                               imageFileName, 
                                               ref trainLibrary1Predictions, 
                                               ref trainLibrary2Predictions, 
                                               runLog
                                              );
 
      if  (trainLibrary1Predictions != null)
      {
        TrainLib1Name.Text = ActiveTrainingLibraries.Model1Name;
        if  (trainLibrary1Predictions.Count > 0)
        {
          pred1Prob.Text      = trainLibrary1Predictions[0].Probability.ToString ("p");
          pred1ClassName.Text = trainLibrary1Predictions[0].ClassName;
          this.Text           = trainLibrary1Predictions[0].ClassName;

          if  (trainLibrary1Predictions.Count > 1)
          {
            lib2Pred1Prob.Text      = trainLibrary1Predictions[1].Probability.ToString ("p");
            lib2Pred1ClassName.Text = trainLibrary1Predictions[1].ClassName;
          }
        }
      }

      if  (trainLibrary2Predictions != null)
      {
        TrainLib2Name.Text = ActiveTrainingLibraries.Model2Name;
        if  (trainLibrary2Predictions.Count > 0)
        {
          lib2Pred1Prob.Text      = trainLibrary2Predictions[0].Probability.ToString ("p");
          lib2Pred1ClassName.Text = trainLibrary2Predictions[0].ClassName;
        }
      }

      larcosRaster = null;

      blobs.Dispose ();
      largestBlob.Dispose ();
      bd.Dispose ();
      bd = null;
    }  /* MakePredictions */  




    private void  ImageEditor_Load (object sender, EventArgs e)
    {
      MaskTypesComboBox.Items.Add (UmiRaster.MaskTypeToStr (UmiRaster.MaskType.CROSS3));
      MaskTypesComboBox.Items.Add (UmiRaster.MaskTypeToStr (UmiRaster.MaskType.CROSS5));
      MaskTypesComboBox.Items.Add (UmiRaster.MaskTypeToStr (UmiRaster.MaskType.SQUARE3));
      MaskTypesComboBox.Items.Add (UmiRaster.MaskTypeToStr (UmiRaster.MaskType.SQUARE5));
      MaskTypesComboBox.Items.Add (UmiRaster.MaskTypeToStr (UmiRaster.MaskType.SQUARE7));
      MaskTypesComboBox.Items.Add (UmiRaster.MaskTypeToStr (UmiRaster.MaskType.SQUARE9));
      MaskTypesComboBox.Items.Add (UmiRaster.MaskTypeToStr (UmiRaster.MaskType.SQUARE11));

      MaskTypesComboBox.SelectedIndex = 2;
    }


    private void BuildColorValues ()
    {
      if  (colorize)
        BuildColorizedPenValues ();
      else
        BuildGrayScaleValues ();
    }



    private void  BuildGrayScaleValues ()
    {
      int x;
      colorValues = new Pen[256];
      for (x = 0; x < 256; x++)
      {
        int y = 255 - x;
        colorValues[x] = new Pen (Color.FromArgb(y, y, y));
      }

      backGroundColor = Color.FromArgb (255, 255, 255);
    }  /* BuildColorValues */


    private  void  BuildColorizedPenValues ()
    {
      float  h;
      float  s = 220.0f / 255.0f;
      float  i = 144.0f / 255.0f;

      float  r, g, b;
      int    rInt, gInt, bInt;

      float  H;

      float  TwoPI        = (float)Math.PI * 2.0f;
      float  TwoThirdsPI  = (float)Math.PI * 2.0f / 3.0f;
      float  OneThirdPI   = (float)Math.PI / 3.0f;
      float  FourThirdsPI = (float)Math.PI * 4.0f / 3.0f;

      int  hInt;
      for  (hInt = 0;  hInt < 256;  hInt++)
      {
        h = (float)(255 - hInt) / 255.0f;

        //H = TwoPI * h;
        //H = TwoThirdsPI * h;
        H = (float)TwoPI * h;
        if  (H <= 0.0f)
          H = H + TwoPI;
      
        if  ((s < 0.00001) || (i < 0.00001))
        {
          r = g = b = i;          
        }   

        else if  ((H > 0.0f)  &&  (H <= TwoThirdsPI))
        {
          r = (float)(i * (1 + ((s * Math.Cos (H)) / Math.Cos (OneThirdPI - H))));
          b = i * (1 - s);
          g = 3 * i * (1 - (r + b)/(3 * i));
        }

        else if  ((H > TwoThirdsPI)  &&  (H <= (FourThirdsPI)))
        {
          H = H - TwoThirdsPI;
          
          g = (float)(i * (1 + ((s * Math.Cos (H)) / Math.Cos (OneThirdPI - H))));
          r = i * (1 - s);
          b = 3 * i * (1 - ((r + g) / (3 * i)));
        }

        else if  ((H > FourThirdsPI) && (H <= TwoPI))
        {
          H = H - FourThirdsPI;
          b = (float)(i * (1 + ((s * Math.Cos (H)) / Math.Cos (OneThirdPI - H))));
          g = i * (1 - s);
          r = 3 * i * (1 - ((g + b) / (3 * i)));
        }

        else
        {
          r = g = b = 0;
        }

        rInt = (int)(255.0f * r + 0.5f);
        if  (rInt > 255)
          rInt = 255;

        gInt = (int)(255.0f * g + 0.5f);
        if  (gInt > 255)
          gInt = 255;

        bInt = (int)(255.0f * b + 0.5f);
        if  (bInt > 255)
          bInt = 255;


        colorValues[hInt] = new Pen (Color.FromArgb (255, rInt, gInt, bInt));
      }

      colorValues[0] = new Pen (Color.FromArgb (255, 255, 255, 255));
    }  /* BuildColorValues */


		private void saveToolStripMenuItem_Click(object sender, EventArgs e)
		{
			SaveFileDialog saveDialog = new SaveFileDialog();
			saveDialog.InitialDirectory = "";
			saveDialog.Filter = "(Bitmaps)|*.bmp;";
			saveDialog.FilterIndex = 0;
			saveDialog.RestoreDirectory = true;
			saveDialog.FileName = fileName;
			if (saveDialog.ShowDialog() == DialogResult.OK)
			{
				fileName = saveDialog.FileName;
				image.Save(saveDialog.FileName, ImageFormat.Bmp);
			}
		}



		private void exitToolStripMenuItem_Click(object sender, EventArgs e)
		{

		}



    public  Bitmap  BuildBitmapFromRasterData (byte[][]  r)
    {
      int  rowFirst = 99999;
      int  rowLast  = -1;
      int  colFirst = 99999;
      int  colLast  = -1;

      Bitmap  image = new Bitmap (width, height);
    
    
      // GDI+ return format is BGR, NOT RGB. 
      BitmapData bmData = image.LockBits (new Rectangle(0, 0, width, height),
                                          ImageLockMode.ReadWrite,
                                          PixelFormat.Format24bppRgb
                                         );
      int stride = bmData.Stride;
      System.IntPtr Scan0 = bmData.Scan0;

      unsafe
      {
        int  nOffset = stride - width * 3;
        int  bytesPerRow = width * 3 + nOffset;
        int  col = 0;
        int  row = 0;
        byte pixelValue = 0;
        byte lastPixelValue = 0;
        Pen  curPen = colorValues[pixelValue];

        byte  red   = curPen.Color.R;
        byte  green = curPen.Color.G;
        byte  blue  = curPen.Color.B;


        byte* ptr = (byte*)(void*)Scan0;

        for  (row = 0;  row < height;  row++)
        {
          byte[]  datRow = r[row];
          
          for (col = 0;  col < width;  col++)
          {
            pixelValue = datRow[col];
            if  (pixelValue != lastPixelValue)
            {
              lastPixelValue = pixelValue;
              curPen = colorValues[lastPixelValue];
              red   = curPen.Color.R;
              green = curPen.Color.G;
              blue  = curPen.Color.B;
            }

            ptr[0] = blue;  ptr++;
            ptr[0] = green; ptr++;
            ptr[0] = red;   ptr++;

            if  (pixelValue < 255)
            {
              // We are looking at a foreground pixel.
              if  (row < rowFirst )
                rowFirst = row;
              rowLast = row;

              if  (col < colFirst)
                colFirst = col;

              if  (col > colLast)
                colLast = col;
            }
          }
          
          ptr += nOffset;
        }
      }  /* Unsafe */

      image.UnlockBits (bmData);


      {
        // We are now going to add some scale lines
        int scaleColRight = Math.Min (colLast + 5, width - 1);
        int scaleColLeft  = Math.Max (0,           colFirst - 5);

        int  scaleRowBot = Math.Min (rowLast + 5, height - 1);
        int  scaleRowTop = Math.Max (0          , rowFirst - 5);

        Pen  p = new Pen (Color.Red);
        Graphics  dc = Graphics.FromImage (image);
        dc.DrawLine (p, scaleColLeft,  rowFirst, scaleColLeft,  rowLast);
        dc.DrawLine (p, scaleColRight, rowFirst, scaleColRight, rowLast);

        dc.DrawLine (p, colFirst, scaleRowTop, colLast,  scaleRowTop);
        dc.DrawLine (p, colFirst, scaleRowBot, colLast,  scaleRowBot);

        int  x = rowFirst;
        while  (x < rowLast)
        {
          dc.DrawLine (p, scaleColLeft  + 4, x, scaleColLeft,  x);
          if  (x > rowFirst)
            dc.DrawLine (p, scaleColRight - 4, x, scaleColRight, x);
          x += 40;
        }
        //dc.DrawLine (p, scaleColRight - 5, rowLast, scaleColRight, rowLast);
 
        x = colFirst;
        while  (x < colLast)
        {
          dc.DrawLine (p, x,  scaleRowTop + 4, x, scaleRowTop);
          if  (x > colFirst)
            dc.DrawLine (p, x,  scaleRowBot - 4, x, scaleRowBot);
          x += 40;
        }
        //dc.DrawLine (p, colLast, scaleRowBot, colLast - 5, scaleRowBot);
      }
      
      return  image;
    }  /* BuildBitmapFromRasterData */





    private  void  ResizeWindow ()
    {
      int  availRows = Height - (ImagePanel.Top + 60);
      int  availCols = Width  - (ImagePanel.Left + 40);
      
      ImagePanel.Width  = availCols;
      ImagePanel.Height = availRows;

      pictureBox1.Height = ImagePanel.Height - 4;
      pictureBox1.Width  = ImagePanel.Width  - 4;

      int lastRowWeCanUse = ImagePanel.Bottom;
      int newOperationsListHeight = lastRowWeCanUse - OperationsList.Top;
      OperationsList.Height = newOperationsListHeight;

      Invalidate ();
    }  /* ResizeWindow */




    private  void  RePaintImage ()
    {
      if  ((sizeRatio <= 0.0f)  ||  (image == null))
        return;

      byte[][] processedRaster = PerformAllOperations (raster);


      Bitmap newImage = BuildBitmapFromRasterData (processedRaster);
      
      int  newWidth  = (int)((float)width  * sizeRatio + 0.5f);
      int  newHeight = (int)((float)height * sizeRatio * flowRateFactor + 0.5f);

      image = new Bitmap (newWidth, newHeight);
      Graphics imageDC = Graphics.FromImage (image);
      imageDC.Clear (backGroundColor);
      imageDC.ScaleTransform (sizeRatio, sizeRatio * flowRateFactor);
      imageDC.DrawImageUnscaled (newImage, 0, 0);
      
      newImage.Dispose ();
      newImage = null;
      
      pictureBox1.Height = newHeight;
      pictureBox1.Width  = newWidth;
      pictureBox1.Image  = image;

      ImagePanel.AutoScroll        = true;
      ImagePanel.AutoScrollMinSize = image.Size;
      ImagePanel.Invalidate ();
    }  /* RePaintImage */

       

    private void ImageEditor_SizeChanged(object sender, EventArgs e)
    {
      ResizeWindow ();
    }



    private void SizeOptions_SelectedOptionChanged ()
    {
      string  selOptStr = Scale.SelectedText.ToString ();
      if  (selOptStr.EndsWith ("%"))
      {
        selOptStr = selOptStr.Substring (0, selOptStr.Length - 1);
        sizeRatio = UmiKKStr.StrToFloat (selOptStr) / 100.0f;
        RePaintImage ();
      }
    }


		private void Scale_SelectedIndexChanged(object sender, EventArgs e)
		{
			String selScaleStr = (String)Scale.SelectedItem;
      if (selScaleStr.EndsWith ("%"))
			{
        selScaleStr = selScaleStr.Substring (0, selScaleStr.Length - 1);
        sizeRatio = UmiKKStr.StrToFloat (selScaleStr) / 100.0f;
				RePaintImage();
			}
		}


    private void ClassifyButton_Click (object sender, EventArgs e)
    {
    }




    private void Colorize_CheckedChanged(object sender, EventArgs e)
    {
      if  (Colorize.Checked == colorize)
        return;

      colorize = Colorize.Checked;
      BuildColorValues ();
      RePaintImage ();
    }


    private void ClossingButton_Click(object sender, EventArgs e)
    {
      AddOperation (UmiRaster.OperationType.Closing);
    }

    private void OpeningButton_Click(object sender, EventArgs e)
    {
      AddOperation (UmiRaster.OperationType.Opening);
    }

    private void ErosionButton_Click(object sender, EventArgs e)
    {
      AddOperation (UmiRaster.OperationType.Erosion);
    }

    private void DialationButton_Click(object sender, EventArgs e)
    {
      AddOperation (UmiRaster.OperationType.Dilation);
    }

    private void SmoothingButton_Click(object sender, EventArgs e)
    {
      AddOperation (UmiRaster.OperationType.SmoothAveraging);
    }

    private void MaskExcludeButton_Click (object sender, EventArgs e)
    {
      AddOperation (UmiRaster.OperationType.MaskExclude);
    }


    private void MediumButton_Click(object sender, EventArgs e)
    {
      AddOperation (UmiRaster.OperationType.SmoothMedium);
    }

    private void EdgeButton_Click(object sender, EventArgs e)
    {
      AddOperation (UmiRaster.OperationType.Edge);
    }

    private void FillHolesButton_Click(object sender, EventArgs e)
    {
      AddOperation (UmiRaster.OperationType.FillHoles);
    }

    private void ThinningButton_Click(object sender, EventArgs e)
    {
      AddOperation (UmiRaster.OperationType.Thinning);
    }

    private void UndoButton_Click(object sender, EventArgs e)
    {
      UndoOperation ();
    }

    private void ConnectedComponentButton_Click(object sender, EventArgs e)
    {
      AddOperation (UmiRaster.OperationType.ConnectedComponent);
    }

    private void BinarizeButton_Click(object sender, EventArgs e)
    {
      AddOperation (UmiRaster.OperationType.Binarize);
    }

    private void ConvexHullButon_Click(object sender, EventArgs e)
    {
      AddOperation (UmiRaster.OperationType.ConvexHull);
    }



    private void BandPassButton_Click(object sender, EventArgs e)
    {
      lowerBound = UmiKKStr.StrToFloat (LowerBound.Text) / 100.0f;
      upperBound = UmiKKStr.StrToFloat (UpperBound.Text) / 100.0f;

      if  (lowerBound < 0.0f)
        lowerBound = 0.0f;
      else if  (lowerBound > 1.0f)
        lowerBound = 1.0f;

      if  (upperBound < 0.0f)
        upperBound = 0.0f;
      else if  (upperBound > 1.0f)
        upperBound = 1.0f;

      if  (lowerBound > upperBound)
        lowerBound = upperBound;

      LowerBound.Text = ((float)(lowerBound * 100.0f)).ToString ("##0");
      UpperBound.Text = ((float)(upperBound * 100.0f)).ToString ("##0");

      AddOperation (UmiRaster.OperationType.BandPass);
    }



    private void BinarizeTHButton_Click(object sender, EventArgs e)
    {
      thLowerBound = (uint)UmiKKStr.StrToInt (ThLowerBound.Text);
      thUpperBound = (uint)UmiKKStr.StrToInt (ThUpperBound.Text);

      thLowerBound = Math.Max (0,   thLowerBound);
      thUpperBound = Math.Min (255, thUpperBound);
      
      if  (thLowerBound > thUpperBound)
        thLowerBound = thUpperBound - 1;

      ThLowerBound.Text = thLowerBound.ToString ("##0");
      ThUpperBound.Text = thUpperBound.ToString ("##0");

      AddOperation (UmiRaster.OperationType.BinarizeTH);
    }

    private void FlowRateFactor_ValueChanged (object sender, EventArgs e)
    {
      flowRateFactor = (float)FlowRateFactor.Value;
      RePaintImage();
    }

  }
}