using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO;
using System.Text;
using System.Windows.Forms;

using LarcosCounterManaged;

namespace LarcosManagedRoutines
{
  public partial class DisplayParticleImages : Form
  {
    private  int                   lastParticleIndexLoaded = 0;
    private  List<FlowLayoutPanel> thumbNails = null;

    private  int                   lastHeight = 0;
    private  int                   lastWidth  = 0;

    private  bool                  updatesWereMade = false;

    private  UmiParticleEntryList         particles      = null;
    private  UmiScannerFileBuffered       scannerFile    = null;
    private  LarcosCounterManagerWrapper  cameraManager  = null;
    private  UmiRunLog                    runLog         = null;
    private  float                        pixelsPerMM    = 23.6f;
    private  UmiShrimpLengthComputer      lengthComputer = null;
    private  UmiOperatingParameters       operatingParameters = null;

    private  bool  saveDebugImages = false;


    public  bool  UpdatesWereMade  {get {return updatesWereMade;}}


    public DisplayParticleImages (String                       _title,
                                  LarcosCounterManagerWrapper  _cameraManager,
                                  UmiScannerFileBuffered       _scannerFile,
                                  UmiParticleEntryList         _particles,
                                  bool                         _saveDebugImages,
                                  UmiRunLog                    _runLog
                                 )
    {
      InitializeComponent ();

      Text            = _title;
      cameraManager   = _cameraManager;
      scannerFile     = _scannerFile;
      particles       = _particles;
      saveDebugImages = _saveDebugImages;
      runLog          = _runLog;

      operatingParameters = particles.GetOperatingParameters ();

      int  backgroundPixelTH = 31;
      if  (operatingParameters != null)
        backgroundPixelTH = operatingParameters.BackGroundPixelTH;

      float  width = cameraManager.ImagingChamberWidth ();
      if  (width == 0.0f)
        width = 2.5f * 2.54f / 100.0f;   // (2.5 inches) * (2.54 cm's/inch)  /  (100 cm's/m)

      int  pixelsPerScanLine = cameraManager.ImagingChamberWidthPixels ();
      pixelsPerMM = (float)pixelsPerScanLine /  width;
      int  erosionStructSize = particles.ErosionStructSize;

      lengthComputer = new UmiShrimpLengthComputer (pixelsPerScanLine, cameraManager.ImagingChamberWidth (), erosionStructSize, backgroundPixelTH);

      thumbNails = new List<FlowLayoutPanel> ();
      if  (particles != null)
        UpdateDisplayTimer.Enabled = true;
    }

    

    public bool GetThumbnailImageAbort()
    {
      return true;
    }  /* GetThumbnailImageAbort */



    private  System.IntPtr  thumbNailCallBackData = IntPtr.Zero;




    private  void  LoadNextParticleFromScannerFile ()
    {
      if  (particles == null)
        return;

      if  (lastParticleIndexLoaded >= particles.Count)
      {
        UpdateDisplayTimer.Enabled = false;
        return;
      }

      Font font = new System.Drawing.Font ("Courier New", 8.0f, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));

      Bitmap  image = null;

      int  particleIndex = lastParticleIndexLoaded;
      
      UmiParticleEntry  particleEntry = particles[lastParticleIndexLoaded];
      lastParticleIndexLoaded++;
      UmiRaster  particle = scannerFile.GetRasterForParticleAsCounted (particleEntry, operatingParameters);
      particle = particle.Transpose ();

      try  {image = particle.BuildBitmap ();}
      catch (Exception) {image = null;}
      if  (image == null)
         return;

      float  ratio = 1.0f;
      int  maxDim = Math.Max (image.Height, image.Width);
      if  (maxDim > 150)
         ratio = 150.0f / (float)maxDim;

      PictureBox pb = new PictureBox ();
      pb.BorderStyle = BorderStyle.FixedSingle;

      int h = (int)((float)image.Height * ratio);
      int w = (int)((float)image.Width  * ratio);

      pb.Height = h + 2;
      pb.Width  = w + 2;
      pb.MouseDoubleClick += new System.Windows.Forms.MouseEventHandler(ThumbNail_MouseDoubleClick);
      pb.Name = particleIndex.ToString ();

      Image thumbNail = image.GetThumbnailImage (w, h, GetThumbnailImageAbort, thumbNailCallBackData);
      pb.Image = thumbNail;

      FlowLayoutPanel pan = new FlowLayoutPanel ();
      pan.Size = new Size (Math.Max (170, w + 10), 200);
      pan.Height = h + 80;
      pan.Width  = Math.Max (160, w + 10);
      pan.Controls.Add (pb);
      //TextBox tb = new TextBox ();
      //tb.Width = pan.Width - 8;
      //tb.Text = rootName;
      //tb.Font = font;
      //pan.Controls.Add (tb);

      pan.BorderStyle = BorderStyle.FixedSingle;

      pan.BackColor = Color.White;
      
      thumbNails.Add (pan);
      ImageDisplayPanel.Controls.Add (pan);
    }  /* LoadNextParticleFromScannerFile */



    private  void  ThumbNail_MouseDoubleClick (object sender, MouseEventArgs e)
    {
      if  (e.Button == MouseButtons.Left)
      {
        PictureBox pb = (PictureBox)sender;
        String  particleEntryIndexStr = pb.Name;
        int  particleEntryIndex = UmiKKStr.StrToInt (particleEntryIndexStr);
        if  ((particleEntryIndex >= 0)  &&  (particleEntryIndex < particles.Count))
        {
          UmiParticleEntry pe = particles[particleEntryIndex];
          if  (pe != null)
          {
            String  fileName = UmiOSservices.GetRootName (scannerFile.FileName) + "_" +
                               pe.ScannerRow.ToString ("00000000")              + "_" +
                               pe.ScannerCol.ToString ("0000") +
                              ".bmp";
            UmiRaster  pr = scannerFile.GetRasterForParticle (particles[particleEntryIndex]);
            pr.BackgroundPixelTH = (byte)particles.BackGroundPixelTH;

            if  (particles.ErosionStructSize > 0)
            {
              UmiRaster.MaskType  mt = UmiRaster.MaskType.SQUARE3;
              switch  (particles.ErosionStructSize)
              {
                case  1: mt = UmiRaster.MaskType.SQUARE3;  break;
                case  2: mt = UmiRaster.MaskType.SQUARE5;  break;
                case  3: mt = UmiRaster.MaskType.SQUARE7;  break;
                case  4: mt = UmiRaster.MaskType.SQUARE9;  break;
                case  5: mt = UmiRaster.MaskType.SQUARE11; break;
              }
              pr = pr.ErodeImage (mt);
            }

            pr = pr.ConnectedComponent ();
            pr = pr.StreatchImage (particles.FlowRateFactor, 1.0f);

            ImageVerifier  iv = new ImageVerifier (cameraManager, particles[particleEntryIndex], pr, fileName, pixelsPerMM, saveDebugImages, runLog);
            iv.LengthComputer (lengthComputer);
            iv.ShowDialog (this);
    
            if  (iv.OkPressed  &&  (iv.ClassUserValidatesAs != null))
            {
              pe.PredClass = iv.ClassUserValidatesAs;
              particles.AddUpdateToReportFile (pe);
              updatesWereMade = true;
            }
          }
        }
      }
    }


    private void  UpdateDisplayTimer_Tick (object sender, EventArgs e)
    {
      if  (particles != null)
        LoadNextParticleFromScannerFile ();
      else
        LoadNextParticleFromScannerFile ();
    }


    private void DisplayScsImages_Load(object sender, EventArgs e)
    {
      lastHeight = Height;
      lastWidth  = Width;
    }



    private void  DisplayLarcosImages_Resize (object sender, EventArgs e)
    {
    }




    private void DisplayLarcosImages_SizeChanged(object sender, EventArgs e)
    {
    }


    private void ImageDisplayPanel_Paint(object sender, PaintEventArgs e)
    {

    }

    private void DisplayLarcosImages_ResizeEnd(object sender, EventArgs e)
    {
      int  deltaWidth  = Width  - lastWidth;
      int  deltaHeight = Height - lastHeight;

      if  ((deltaHeight == 0)  &&  (deltaWidth == 0))
        return;

      ImageDisplayPanel.Height += deltaHeight;
      ImageDisplayPanel.Width  += deltaWidth;

      ImageDisplayPanel.Controls.Clear ();

      foreach  (FlowLayoutPanel  flp  in  thumbNails)
      {
        ImageDisplayPanel.Controls.Add (flp);
      }

      ImageDisplayPanel.AutoScroll = true;
      ImageDisplayPanel.PerformLayout ();
    } 
  }
}
