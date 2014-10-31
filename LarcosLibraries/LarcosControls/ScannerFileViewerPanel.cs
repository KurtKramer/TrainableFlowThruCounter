using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Drawing.Imaging;
using System.Text;
using System.Windows.Forms;
using LarcosCounterManaged;
using KKManagedRoutines;

namespace LarcosControls
{
  public partial class ScannerFileViewerPanel : UserControl
  {

    private  Graphics  panelDC  = null;
    private  Bitmap    buffer   = null;
    private  Graphics  bufferDC = null;
    private  Color     backGroundColor;
    private  int       suspendLevel = 0;

    private  Int32     oldHeight = 0;
    private  Int32     oldWidth  = 0;

    private  Int32     panelWidth2  = 0;
    private  Int32     panelHeight2 = 0;

    private  Int32     displayRowsToDisplay = 2048;  /**<  Number of Display Rows that can be displayed;
                                                      *  The number of Scanner Rows = ratio * displayRowsToDisplay
                                                      */
    
    // The next two variables are with respect to the whole scanner file but their values are 
    // adjusted by 'ratio'.
    private  Int32     displayScanRowLeft  = 0;
    private  Int32     displayScanRowRight = 2047;

    //private  Int32     panelHeight2 = 0;  // Will be <= panelHeight; such that allows a proper mutiple of ratio

    private  byte[]    displayRow = null;
    private  byte[][]  raster     = null;

    private  Int32     cropColumnLeft  = 0;
    private  Int32     cropColumnRight = 2047;

    private  UmiScannerFileBuffered  scannerFile = null;

    private  String    trainingModelName = "";

    private  Pen[]                  colorValues = null;   // Pen color to use for each pixel value;  normaly 
    // each value is set to the appropriate grayscale value.
    // except when (colorize == true) selectd then Colors
    // are used for the 7 levels of grayscale that the Scanner
    // camera produces.

    private  BlobList               blobs = null;

    private  bool                   colorize = false;

    private  ContextMenu            scannerPanelContextMenu = null;

    private  UmiParticleEntryList   particlesOnCurrentScreen = null;

    private  LarcosCounterManagerWrapper     cameraManager = null;

    private  UmiRunLog              runLog = null;

    private  Font                   countFont = null;



    // The last column and row clicked on Panel.
    private  Int32  rightMouseRow = 0;
    private  Int32  rightMouseCol = 0;

    public  Int32  CropColemnLeft       () {return cropColumnLeft;}
    public  Int32  CropColemnRight      () {return cropColumnRight;}
    public  Int32  DisplayRowsToDisplay () {return displayRowsToDisplay;}
    public  int    DisplayScanRowRight  () {return displayScanRowRight;}
    public  int    DisplayScanRowLeft   () {return displayScanRowLeft;}


   
    public  void  SetCropColumns (int _cropColumnLeft,
                                  int _cropColumnRight
                                 )
    {
      cropColumnLeft  = _cropColumnLeft;
      cropColumnRight = _cropColumnRight;
      
      ComputeParameters ();
      PaintWholePanel ();
      OnCropSettingsChanged ();
    }


    public  int  ScanLineLeft ()  {return DisplayRowToScanLineRow (displayScanRowLeft);}


    public float  Ratio ()
    {
      if  (scannerFile == null)
        return 1.0f;
      else
        return scannerFile.Ratio;
    }


    public  int  DisplayRowToScanLineRow (int displayRow)
    {
      return (int)(0.5f + (float)displayRow * Ratio ());
    }


    public  int  ScanLineRowToDispalyRow (int scanLineRow)
    {
      return (int)(0.5f + (float)scanLineRow / Ratio ());
    }


    public ScannerFileViewerPanel ()
    {
      //scannerFile = _scannerFile;

      runLog = new UmiRunLog ();

      SizeChanged += new System.EventHandler (this.OnSizeChanged);
      InitializeComponent ();
      
      oldHeight = Height;
      oldWidth  = Width;

      // We want to add the 'OnSizeChanged' even after we Initialize all components so that 
      // 'oldHeight' and 'oldWidth' are updated correctly.
      SizeChanged += new System.EventHandler (this.OnSizeChanged);

      SetUpScannerPanelContectMenu ();
      panel.MouseClick += new MouseEventHandler (MonitorMouseClick);

      BuildCountFont ();
    }


    private  void  BuildCountFont ()
    {
      // A panel height of 400 pixels would get a "Courier New" font size of 10.  We will scale 
      // linearly frolm that.
      float  fontSize = panel.Height * 10.0f / 400.0f;
      countFont = new Font ("Courier New", fontSize, FontStyle.Bold);
    }


    private void BuildPenValues ()
    {
      if (colorize)
      {
        BuildColorizedPenValues ();
        return;
      }

      int x;

      colorValues = new Pen[256];
      for (x = 0; x < 256; x++)
      {
        int y = 255 - x;
        colorValues[x] = new Pen (Color.FromArgb (y, y, y));
      }

      backGroundColor = Color.FromArgb (255, 255, 255);
    }  /* BuildPenValues */



    new void  SuspendLayout ()
    {
      suspendLevel++;
      if (suspendLevel < 2)
      {
        base.SuspendLayout ();
      }
    }  /* SuspendLayout */


    new void ResumeLayout ()
    {
      if (suspendLevel <= 0)
        return;

      suspendLevel--;
      if  (suspendLevel < 1)
      {
        base.ResumeLayout ();
      }
    }  /* ResumeLayout */


    new void  ResumeLayout (bool perFormLayout)
    {
      if  (suspendLevel <= 0)
        return;

      suspendLevel--;
      if (suspendLevel < 1)
      {
        base.ResumeLayout (perFormLayout);
      }
    }  /* ResumeLayout */



    private void  BuildColorizedPenValues ()
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
      for (hInt = 0; hInt < 256; hInt++)
      {
        h = (float)(255 - hInt) / 255.0f;

        //H = TwoPI * h;
        //H = TwoThirdsPI * h;
        H = (float)TwoPI * h;
        if (H <= 0.0f)
          H = H + TwoPI;

        if ((s < 0.00001) || (i < 0.00001))
        {
          r = g = b = i;
        }

        else if ((H > 0.0f) && (H <= TwoThirdsPI))
        {
          r = (float)(i * (1 + ((s * Math.Cos (H)) / Math.Cos (OneThirdPI - H))));
          b = i * (1 - s);
          g = 3 * i * (1 - (r + b) / (3 * i));
        }

        else if ((H > TwoThirdsPI) && (H <= (FourThirdsPI)))
        {
          H = H - TwoThirdsPI;

          g = (float)(i * (1 + ((s * Math.Cos (H)) / Math.Cos (OneThirdPI - H))));
          r = i * (1 - s);
          b = 3 * i * (1 - ((r + g) / (3 * i)));
        }

        else if ((H > FourThirdsPI) && (H <= TwoPI))
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
        if (rInt > 255)
          rInt = 255;

        gInt = (int)(255.0f * g + 0.5f);
        if (gInt > 255)
          gInt = 255;

        bInt = (int)(255.0f * b + 0.5f);
        if (bInt > 255)
          bInt = 255;


        colorValues[hInt] = new Pen (Color.FromArgb (255, rInt, gInt, bInt));
      }

      colorValues[0] = new Pen (Color.FromArgb (255, 255, 255, 255));
    }



    public void ComputeParameters ()
    {
      int  x = 0;

      double  oldRatio = 1.0;

      int  pixelsPerScanLine = 2048;
      if (scannerFile != null)
        pixelsPerScanLine = scannerFile.PixelsPerScanLine;

      if (panelHeight2 != 0)
        oldRatio = (double)pixelsPerScanLine / (double)panelHeight2;
      if (scannerFile != null)
        oldRatio = scannerFile.Ratio;

      Int32  actualScannerRowThatIsLeft = (Int32)((double)displayScanRowLeft * oldRatio);

      panelWidth2  = panel.Width;
      panelHeight2 = panel.Height;

      float  ratio = pixelsPerScanLine / panelHeight2;
      if (scannerFile != null)
      {
        scannerFile.SetPixelsPerDisplayLine (panelHeight2, cropColumnLeft, cropColumnRight);
        ratio = scannerFile.Ratio;
      }

      if (buffer != null)
        buffer.Dispose ();

      panelDC = panel.CreateGraphics ();
      buffer = new Bitmap (panel.Width, panel.Height);
      bufferDC = Graphics.FromImage (buffer);

      displayRowsToDisplay = panelWidth2;
      displayScanRowRight = displayScanRowLeft + displayRowsToDisplay - 1;

      int  scannerPixelsThatCanFitOnARow = (int)Math.Ceiling ((float)panelHeight2 * ratio);
      displayRow = new byte[panelHeight2];

      //  Raster will be Displayable Scanner (Columns x Rows).
      raster = new byte[panelHeight2][];
      for (x = 0; x < panelHeight2; x++)
        raster[x] = new byte[panelWidth2];

      //SetUpScannerPanelContectMenu ();

      Int32  newDisplayRowTop = (Int32)((double)actualScannerRowThatIsLeft / (double)ratio);
      SetNewDisplayScanRowLeft (newDisplayRowTop);
    }  /* ComputeParameters */




    public void SetNewDisplayScanRowLeft (Int32 _displayScanRowLeft)
    {
      displayScanRowLeft = _displayScanRowLeft;
      displayScanRowRight = displayScanRowLeft + displayRowsToDisplay - 1;
    }  /* SetNewDisplayScanRowLeft */



    public bool Colorize
    {
      get { return colorize; }
      set
      {
        colorize = value;
        BuildPenValues ();
      }
    }  /* Colorize */




    public void SetScannerFile (LarcosCounterManagerWrapper _cameraManager,
                                UmiScannerFileBuffered      _scannerFile,
                                String                      _trainingModelName
                               )
    {
      cameraManager     = _cameraManager;
      scannerFile       = _scannerFile;
      trainingModelName = _trainingModelName;
      runLog = cameraManager.GetUmiRunLogInstance ();
      displayScanRowLeft = 0;

      runLog.WriteLn (40, "ScannerFileViewerPanel.SetScannerFile   _scannerFile :" + _scannerFile.FileName);

      if (scannerFile == null)
      {
        cropColumnLeft = 0;
        cropColumnRight = 2047;
      }
      else
      {
        cropColumnLeft  = scannerFile.CropColLeft ();
        cropColumnRight = scannerFile.CropColRight ();
      }

      ComputeParameters ();
    }  /* SetScannerFile */




    private  void  ComputeARowToDisplay (Int32  displayRowNum,
                                         byte[] displayRow
                                        )
    {
      if (scannerFile == null)
      {
        Int32  col = 0;
        for (col = 0; col < displayRow.Length; col++)
          displayRow[col] = 0;
        return;
      }

      byte[][]  displayRows = new byte[1][];
      displayRows[0] = new byte[panelHeight2];

      scannerFile.GetDisplayRows (displayRowNum, displayRowNum, displayRows);
      byte[]    firstDisplayRow = displayRows[0];

      int  lineLen = Math.Min (displayRow.Length, firstDisplayRow.Length);

      for (int x = 0; x < lineLen; x++)
        displayRow[x] = (byte)firstDisplayRow[x];
    }  /* ComputeARowToDisplay */



    /// <summary>
    /// 
    /// </summary>
    /// <param name="displayScanRow">This is the column in the panel that we wish to display; it will be added to 'displayScanRow' to get the relative displayable scan row.</param>
    /// <param name="dc">Device contents to display to.</param>
    private  void  PaintAScannerRow (int      displayScanRow,
                                     Graphics dc
                                    )
    {
      ComputeARowToDisplay (displayScanRow + displayScanRowLeft, displayRow);

      int  panelRow = 0;

      byte  oldColor = 0;
      byte  curColor = 0;
      int   startPanelRow = 0;

      while (panelRow < (displayRow.Length - 1))
      {
        oldColor = displayRow[panelRow];
        curColor = oldColor;
        startPanelRow = panelRow;

        while ((oldColor == curColor) && (panelRow < (displayRow.Length - 1)))
        {
          ++panelRow;
          curColor = displayRow[panelRow];
        }

        int  endPanelRow = panelRow - 1;

        if (oldColor != 0)
        {
          //Brush b = new SolidBrush (colorValues[oldColor].Color);
          //dc.FillRectangle (b, startCol, screenRow, 1 + (endCol - startCol), 1);
          dc.DrawLine (colorValues[oldColor], displayScanRow, startPanelRow, displayScanRow, endPanelRow + 1);
        }
      }
    }  /* PaintAScannerRow */



    private  void  UpdateIncludeExcludePanel ()
    {
      if  (scannerFile == null)
        return;

      Graphics  dc = IncludeExcludePanel.CreateGraphics ();

      Int32  leftScanRow = DisplayRowToScanLineRow (displayScanRowLeft);
      Int32  lastScanRow = DisplayRowToScanLineRow (displayScanRowRight);

      UmiStartStopPoint prevPoint = scannerFile.StartStopPointPrevEntry (leftScanRow);
      UmiStartStopPoint nextPoint = scannerFile.StartStopPointSuccEntry (leftScanRow + 1);

      UmiStartStopPoint.StartStopType  curType = UmiStartStopPoint.StartStopType.sspNULL;

      Int32  nextDisplayRow = displayScanRowLeft;

      while  (nextDisplayRow < displayScanRowRight)
      {
        if  (prevPoint == null)
          curType = UmiStartStopPoint.StartStopType.sspStartPoint;
        else
          curType = prevPoint.Type;

        Brush b = null;
        if  (curType == UmiStartStopPoint.StartStopType.sspStartPoint)
          b = new SolidBrush (Color.Green);
        else
          b = new SolidBrush (Color.Red);

        Int32 curDisplayRow = nextDisplayRow;
        if  (nextPoint == null)
          nextDisplayRow = Int32.MaxValue;
        else
          nextDisplayRow = ScanLineRowToDispalyRow (nextPoint.ScanLineNum);

        int  fromCol = curDisplayRow - displayScanRowLeft;
        int  toCol = nextDisplayRow - displayScanRowLeft;
        if  (toCol >= IncludeExcludePanel.Width)
        {
          toCol = IncludeExcludePanel.Width - 1;
        }
        else
        {
          panelDC.DrawLine (new Pen (b), toCol, 1, toCol, panel.Height - 2);
        }

        int  width = 1 + toCol - fromCol;

        dc.FillRectangle (b, fromCol, 0, width, IncludeExcludePanel.Height);

        prevPoint = nextPoint;
        if  (nextPoint != null)
          nextPoint = scannerFile.StartStopPointSuccEntry (nextPoint.ScanLineNum + 1);
      }
    }  /* UpdateIncludeExcludePanel */



    public  void  PaintWholePanel ()
    {
      //panelDC.Clear (backGroundColor);
      bufferDC.Clear (backGroundColor);

      if  (scannerFile != null)
      {
        // The next function call will populate 'rasetr' with scan-row data such that each row in raster will reprsent
        // a column in the scanner file and each column in 'raster' will represent the apropriate 'scannerFile' rows.
        // this function should take into count the crop settings and 'ratio' factor.
        scannerFile.GetDisplayRowsScanColsByScanRows (displayScanRowLeft, displayScanRowRight, raster);
        particlesOnCurrentScreen = scannerFile.GetParticlesForScanLineRange (displayScanRowLeft, displayScanRowRight);
      }

      // GDI+ return format is BGR, NOT RGB. 
      BitmapData bmData = buffer.LockBits (new Rectangle (0, 0, panelWidth2, panelHeight2),
                                           ImageLockMode.ReadWrite,
                                           PixelFormat.Format24bppRgb
                                          );

      int stride = bmData.Stride;
      System.IntPtr Scan0 = bmData.Scan0;

      unsafe
      {
        int  nOffset = stride - panelHeight2 * 3;
        int  bytesPerRow = panelWidth2 * 3 + nOffset;

        byte pixelValue    = 0;
        byte oldPixelValue = 0;

        byte  red   = 255;
        byte  green = 255;
        byte  blue  = 255;

        int  panelRow, panelCol;


        for  (panelRow = 0;  panelRow < panelHeight2;  ++panelRow)
        {
          // Each row in 'raster' represents a column in the scanner file that makes up one displayable column.
          byte[]  displayableColumn = raster[panelRow];

          byte* ptr = (byte*)(void*)Scan0 + stride * panelRow;

          for  (panelCol = 0;  panelCol < displayableColumn.Length;  ++panelCol)
          {
            //pixelValue = (byte)((byte)255 - rowData[col]);
            //ptr[0] = pixelValue; ptr++;
            //ptr[0] = pixelValue; ptr++;
            //ptr[0] = pixelValue; ptr++;

            pixelValue = (byte)displayableColumn [panelCol];
            if (oldPixelValue != pixelValue)
            {
              oldPixelValue = pixelValue;
              red   = colorValues[pixelValue].Color.R;
              green = colorValues[pixelValue].Color.G;
              blue  = colorValues[pixelValue].Color.B;
            }

            ptr[0] = blue;  ptr++;
            ptr[0] = green; ptr++;
            ptr[0] = red;   ptr++;
          }

          ptr += nOffset;
        }
      }  /* Unsafe */

      buffer.UnlockBits (bmData);

      panelDC.DrawImageUnscaled (buffer, 0, 0);

      UpdateIncludeExcludePanel ();

      DisplayAllParticles (panelDC);

      ClearBobs ();
    }  /* PaintWholePanel*/




    public void ClearBobs ()
    {
      if (blobs != null)
      {
        blobs.Dispose ();
        blobs = null;
      }
    }  /* ClearBobs */





    public static bool  ScrollLeft (Bitmap  b,
                                    int     numCols
                                   )
    {
      // GDI+ return format is BGR, NOT RGB. 
      BitmapData bmData = b.LockBits (new Rectangle (0, 0, b.Width, b.Height),
                                      ImageLockMode.ReadWrite,
                                      PixelFormat.Format24bppRgb
                                     );

      int stride = bmData.Stride;
      System.IntPtr  Scan0 = bmData.Scan0;

      unsafe
      {
        int  nOffset     = stride - b.Width * 3;
        int  bytesPerRow = b.Width * 3 + nOffset;
        int  bytesToMove = (b.Width - numCols) * 3;

        for  (int row = 0;  row < b.Height;  ++row)
        {
          byte*  destPtr = ((byte*)(void*)Scan0) + bytesPerRow * row;
          byte*  srcPtr =  ((byte*)(void*)Scan0) + bytesPerRow * row + numCols * 3;

          for (int  x = 0; x < bytesToMove; ++x)
          {
            destPtr[0] = srcPtr[0];
            ++srcPtr;
            ++destPtr;
          }
        }

      }  /* Unsafe */

      b.UnlockBits (bmData);

      return true;
    }  /* ScrollLeft */



    /// <summary>
    /// Scroll the Panel Display (Displayable-Scanner-File ) left by 'numDisplayableScanRows'.  That is shift pixels
    /// right by 'numDisplayableScanRows' and fill in missing pannel columns with appropriate scanner-rows.
    /// </summary>
    /// <param name="numDisplayableScanRows">Number of displayable scanner rows to shift right;  a negative value indicates shifting pixels left.</param>
    public void  ScrollRows2 (int numDisplayableScanRows)
    {
      int  displayableScanRow = 0;

      if  (numDisplayableScanRows < 0)
      {
        // KKKK
        // particlesOnCurrentScreen = scannerFile.GetParticlesForScanLineRange (displayScanRowLeft, displayScanRowRight);

        // 1) Get list of particles scrolled away  (From Right Side) and remove from list
        // 2) Get list of particles scrolled into; (To Left Side) add to list.


        // Scroll down
        Bitmap    newBuffer   = new Bitmap (buffer.Width, buffer.Height);
        Graphics  newBufferDC = Graphics.FromImage (newBuffer);
        newBufferDC.Clear (backGroundColor);
        newBufferDC.DrawImageUnscaled (buffer, -numDisplayableScanRows, 0);

        SetNewDisplayScanRowLeft (displayScanRowLeft + numDisplayableScanRows);

        for (displayableScanRow = 0; displayableScanRow < (-numDisplayableScanRows);  ++displayableScanRow)
          PaintAScannerRow (displayableScanRow, newBufferDC);

        buffer.Dispose ();
        bufferDC.Dispose ();

        buffer = newBuffer;
        bufferDC = newBufferDC;
      }

      else
      {
        ScrollLeft (buffer, numDisplayableScanRows);
        bufferDC.FillRectangle (new SolidBrush (backGroundColor),
                                new Rectangle ((panelWidth2 - numDisplayableScanRows), 0, numDisplayableScanRows, buffer.Height)
                               );

        SetNewDisplayScanRowLeft (displayScanRowLeft + numDisplayableScanRows);
        for (displayableScanRow = panelWidth2 - numDisplayableScanRows; displayableScanRow < panelWidth2; displayableScanRow++)
          PaintAScannerRow (displayableScanRow, bufferDC);
      }

      panelDC.DrawImageUnscaled (buffer, 0, 0);
      {
        // Scroll Blobs
        if (blobs != null)
        {
          foreach (Blob  b in blobs)
          {
            b.Scroll (numDisplayableScanRows);
            PaintRectangle (panelDC, Color.Red, b);
          }
        }

        particlesOnCurrentScreen = scannerFile.GetParticlesForScanLineRange (displayScanRowLeft, displayScanRowRight);

        DisplayAllParticles (panelDC);
      }

      UpdateIncludeExcludePanel ();
    }  /* ScrollRows2 */



    public void AddUserSpecifiedBlob (int tlCol, int tlRow,
                                      int brCol, int brRow
                                     )
    {
      if (blobs == null)
        blobs = new BlobList (true);


      Blob b = new Blob (-1, tlCol, tlRow, brCol, brRow);
      blobs.Add (b);
    }



    /// <summary>
    /// Returns a pointer to the particle pointed to by 'displayCol' and 'displayRow'.
    /// </summary>
    /// <param name="displayCol"></param>
    /// <param name="displayRow"></param>
    /// <returns>Particle being pinted to or null if not pointing to particle.</returns>
    public  UmiParticleEntry  SelectParticle (Int32 displayCol,
                                              Int32 displayRow
                                             )
    {
      if  (particlesOnCurrentScreen == null)
        return null;

      Int32  scannerFileRow = (Int32)(0.5f + (float)((displayScanRowLeft + displayCol) * Ratio ()));
      Int32  scannerFileCol = (Int32)((Int32)cropColumnLeft + (Int32)(0.5f + (float)displayRow * Ratio ()));

      return  particlesOnCurrentScreen.FindParticle (scannerFileRow, (Int16)scannerFileCol);
    }




    public  UmiRaster  GetRasterForParticle (UmiParticleEntry pe)
    {
      return  scannerFile.GetRasterForParticle (pe);
    }  /* GetRasterForParticle */



    public  Blob  SelectedBlob (int col,
                                int row
                               )
    {
      if (blobs == null)
        return null;

      foreach (Blob  b in blobs)
      {
        if ((col >= b.ColLeft) && (col <= b.ColRight) && (row >= b.RowBot) && (row <= b.RowTop))
          return b;
      }

      return null;
    }




    public void  PaintFromBuffer ()
    {
      if (buffer == null)
        PaintWholePanel ();
      else
        panelDC.DrawImageUnscaled (buffer, 0, 0);
    }



    public void  PaintAllBlobs ()
    {
      if (blobs == null)
        return;

      foreach (Blob b in blobs)
      {
        if (b != null)
          PaintRectangle (panelDC, Color.Red, b);
      }
    }  /* PaintAllBlobs */




    private void PaintRectangle (Graphics g,
                                 Color    lineColor,
                                 Blob     b
                                )
    {
      int  colLeft  = Math.Max (b.ColLeft,  0);
      int  colRight = Math.Min (b.ColRight, panelHeight2 - 1);
      int  rowTop   = Math.Min (b.RowTop,   panelWidth2 - 1);
      int  rowBot   = Math.Max (b.RowBot,   0);

      Pen p = new Pen (lineColor);

      g.DrawLine (p, colLeft,  rowTop, colRight, rowTop);
      g.DrawLine (p, colRight, rowTop, colRight, rowBot);
      g.DrawLine (p, colRight, rowBot, colLeft,  rowBot);
      g.DrawLine (p, colLeft,  rowBot, colLeft,  rowTop);
    }  /* PaintRectangle */



    private  void  DisplayAllParticles (Graphics g)
    {
      if (particlesOnCurrentScreen == null)
        return;

      foreach  (UmiParticleEntry pe in particlesOnCurrentScreen)
        PaintParticle (g, pe);
    }



    public  void  PaintParticle (UmiParticleEntry  pe)
    {
      Graphics g = panel.CreateGraphics ();
      PaintParticle (g, pe);
    }


    private  void  PaintParticle (Graphics          g,
                                  UmiParticleEntry  pe
                                 )
    {
      float ratio = Ratio ();

      int  colLeft  = (int)((long)(0.5f + (float)pe.ScannerRow / ratio) - displayScanRowLeft);
      int  colRight =  colLeft + (int)(pe.Height / ratio);

      int  rowTop = (int)(0.5f + (float)(pe.ScannerCol - cropColumnLeft) / ratio);
      int  rowBot = rowTop + (int)((float)pe.Width / ratio);

      Pen p = null;

      //String  name = pe.PredClass.Name;
      //if ((name.Length > 5) && (name.Substring (0, 6) == "Shrimp"))

      float  cf = pe.PredClass.CountFactor;

      if  (cf > 0.0f)
        p = new Pen (Color.Green);
      else
        p = new Pen (Color.Red);

      g.DrawLine (p, colLeft,  rowTop, colRight, rowTop);
      g.DrawLine (p, colRight, rowTop, colRight, rowBot);
      g.DrawLine (p, colRight, rowBot, colLeft,  rowBot);
      g.DrawLine (p, colLeft,  rowBot, colLeft,  rowTop);

      if  (cf > 1)
      {
        String s = cf.ToString ();
        int  sLen  = (int)(0.5f + g.MeasureString (s, countFont).Width);
        int  sHeight = (int)(0.5f + countFont.GetHeight (g));

        int c = (colLeft + colRight - sLen) / 2;
        int r = 2 + rowTop - sHeight;
        g.DrawString (s, countFont, Brushes.Blue, c, r);
      }

    }  /* PaintParticle */




    public  void  PaintTemporayRectangle (Color  lineColor,
                                          int    x1,
                                          int    y1,
                                          int    x2,
                                          int    y2
                                         )
    {
      Bitmap    tempBuffer = new Bitmap (buffer, buffer.Width, buffer.Height);
      Graphics  tempBufferDC = Graphics.FromImage (tempBuffer);

      Pen p = new Pen (lineColor);

      tempBufferDC.DrawLine (p, x1, y1, x2, y1);
      tempBufferDC.DrawLine (p, x2, y1, x2, y2);
      tempBufferDC.DrawLine (p, x2, y2, x1, y2);
      tempBufferDC.DrawLine (p, x1, y2, x1, y1);

      panelDC.DrawImageUnscaled (tempBuffer, 0, 0);
    }  /* PaintTemporayRectangle */



    public  byte[][]  ExtractImageFromScanner (Blob       blob,
                                               ref Int32  firstScanLine,
                                               ref Int32  scannerFileColLeft
                                              )
    {
      if (scannerFile == null)
        return null;

      int  colLeft  = Math.Max (0, blob.ColLeft - 1);
      int  rowBot   = Math.Max (0, blob.RowBot  - 1);
      int  colRight = Math.Min (panel.Width  - 1, blob.ColRight + 1);
      int  rowTop   = Math.Min (panel.Height - 1, blob.RowTop   + 1);

      firstScanLine = (Int32)((displayScanRowLeft + colLeft) * scannerFile.Ratio);
      scannerFileColLeft = (Int32)((Int32)(Math.Floor (0.5f + (float)colLeft * scannerFile.Ratio)) + (Int32)(scannerFile.CropColLeft ()));

      Int32  lastScanLine = (Int32)((float)(displayScanRowLeft + colRight) * scannerFile.Ratio + 0.5f);

      Int32  scannerFileColRight = (Int32)(((float)rowTop  * scannerFile.Ratio) + (Int32)(scannerFile.CropColLeft ()));
      if (scannerFileColRight >= scannerFile.PixelsPerScanLine)
        scannerFileColRight = (Int32)(scannerFile.PixelsPerScanLine - 1);

      if (scannerFileColLeft >= scannerFileColRight)
        return null;

      byte[][] scannerRows = scannerFile.GetScanLines (firstScanLine, lastScanLine);

      int  width  = (int)(1 + lastScanLine - firstScanLine);
      int  height = (int)(1 + scannerFileColRight - scannerFileColLeft);
      if  (scannerRows.Length < width)
        width = scannerRows.Length ;

      byte[][]  raster = new byte[height][];

      for  (int imageRow = 0, scannerCol = scannerFileColLeft;  imageRow < height;  ++imageRow, ++scannerCol)
      {
        raster[imageRow] = new byte[width];

        for  (int imageCol = 0,  scannerRow = 0;  imageCol < width;  ++imageCol, ++scannerRow)
        {
          raster[imageRow][imageCol] = scannerRows[scannerRow][scannerCol];
        }
      }

      return raster;
    }   /* ExtractImageFromScanner */


    private  void  SetUpScannerPanelContectMenu ()
    {
      scannerPanelContextMenu = new ContextMenu ();

      if (scannerFile != null)
      {
        scannerPanelContextMenu.MenuItems.Add ("Set Top-Crop", new EventHandler (SetLeftCrop));
        if (scannerFile.CropColLeft () > 0)
          scannerPanelContextMenu.MenuItems.Add ("Release Top-Crop", new EventHandler (ReleaseLeftCrop));

           scannerPanelContextMenu.MenuItems.Add ("Set Bottom Crop", new EventHandler (SetRightCrop));
        if (scannerFile.CropColRight () < (scannerFile.PixelsPerScanLine - 1))
          scannerPanelContextMenu.MenuItems.Add ("Release Bottom Crop", new EventHandler (ReleaseRightCrop));

        scannerPanelContextMenu.MenuItems.Add ("Start Counting", new EventHandler (SetStartCountingPoint));
        scannerPanelContextMenu.MenuItems.Add ("Stop Counting", new EventHandler (SetStopCountingPoint));
        scannerPanelContextMenu.MenuItems.Add ("Remove Counting Point", new EventHandler (RemoveCountingPoint));
      }

      scannerPanelContextMenu.Popup += new EventHandler (ScannerPanelContectMenuPopupEvent);

      panel.ContextMenu = scannerPanelContextMenu;
    }


    private  void  ScannerPanelContectMenuPopupEvent (System.Object sender, System.EventArgs e)
    {
      if (scannerFile == null)
        return;

      ContextMenu cm = (ContextMenu)sender;
      cm.MenuItems.Clear ();

      Point screenPoint = Cursor.Position;
      Point panelCursonPos = panel.PointToClient (screenPoint);
      int  rightMouseCol = panelCursonPos.X;

      cm.MenuItems.Add ("Set Top-Crop", new EventHandler (SetLeftCrop));
      if (scannerFile.CropColLeft () > 0)
        cm.MenuItems.Add ("Release Top-Crop", new EventHandler (ReleaseLeftCrop));

      cm.MenuItems.Add ("Set Bottom Crop", new EventHandler (SetRightCrop));
      if (scannerFile.CropColRight () < (scannerFile.PixelsPerScanLine - 1))
        cm.MenuItems.Add ("Release Bottom Crop", new EventHandler (ReleaseRightCrop));

      Int32 curScanRow = DisplayRowToScanLineRow (displayScanRowLeft + rightMouseCol);
      UmiStartStopPoint p = scannerFile.StartStopPointPrevEntry (curScanRow);
      if  ((p == null)  ||  (p.Type == UmiStartStopPoint.StartStopType.sspStartPoint))
      {
        cm.MenuItems.Add ("Stop Counting", new EventHandler (SetStopCountingPoint));
      }
      else
      {
        cm.MenuItems.Add ("Start Counting", new EventHandler (SetStartCountingPoint));
      }

      p = scannerFile.StartStopPointNearestEntry (curScanRow);
      if  (p != null)
      {
        Int32  deltaRows = Math.Abs (curScanRow - p.ScanLineNum);
        Int32  deltaDiaplayRows = ScanLineRowToDispalyRow (deltaRows);
        if (deltaDiaplayRows < 200)
          cm.MenuItems.Add ("Remove Counting Point", new EventHandler (RemoveCountingPoint));
      }
    }


    private  void  SetStartCountingPoint (object sender, System.EventArgs e)
    {
      if  (scannerFile == null)
        return;

      Int32  startScanLine = (int)(0.5f + (displayScanRowLeft + rightMouseCol) * Ratio ());
      runLog.WriteLn (10, "ScannerFileViewerPanel.SetStartCountingPoint   startScanLine :" + startScanLine.ToString ());
      scannerFile.AddStartPoint (startScanLine);
      UpdateIncludeExcludePanel ();
      OnStartPointAdded (startScanLine);
      OnStartStopPointChange (startScanLine);
    }


    private  void  SetStopCountingPoint (object sender, System.EventArgs e)
    {
      Int32  stopScanLine = (int)(0.5f + (displayScanRowLeft + rightMouseCol) * Ratio ());
      runLog.WriteLn (10, "ScannerFileViewerPanel.SetStopCountingPoint   stopScanLine :" + stopScanLine.ToString ());
      scannerFile.AddStopPoint (stopScanLine);
      UpdateIncludeExcludePanel ();
      OnStopPointAdded (stopScanLine);
      OnStartStopPointChange (stopScanLine);
    }


    private  void  RemoveCountingPoint (object sender, System.EventArgs e)
    {
      Int32  scanLineOfPointToSDelete = -1;
      Int32  scanLine = (int)(0.5f + (displayScanRowLeft + rightMouseCol) * Ratio ());

      UmiStartStopPoint nearestPoint = scannerFile.StartStopPointNearestEntry (scanLine);
      if  (nearestPoint == null)
      {
        MessageBox.Show (this, "No Start or Stop point nearby.", "Remove Counting Point", MessageBoxButtons.OK);
      }
      else
      {
        runLog.WriteLn (10, "ScannerFileViewerPanel.RemoveCountingPoint   scanLine :" + scanLine.ToString ());
        scanLineOfPointToSDelete = nearestPoint.ScanLineNum;
        scannerFile.StartStopPointDelete (scanLineOfPointToSDelete);
        UpdateIncludeExcludePanel ();
        OnStartStopPointDeleted (nearestPoint.ScanLineNum);
        OnStartStopPointChange (nearestPoint.ScanLineNum);
      }
    }


   
    private void ReleaseLeftCrop (object sender, System.EventArgs e)
    {
      if (scannerFile != null)
      {
        cropColumnLeft = 0;
        ComputeParameters ();
        PaintWholePanel ();
        OnCropSettingsChanged ();
      }
    } /* ReleaseLeftCrop */




    private void SetLeftCrop (object sender, System.EventArgs e)
    {
      if (scannerFile != null)
      {
        Int32  scanLineLen = scannerFile.PixelsPerDisLine ();
        cropColumnLeft = scannerFile.ScanColumnFromDisplayCol (rightMouseRow);
        if ((1 + cropColumnRight - cropColumnLeft) <= scanLineLen)
        {
          cropColumnRight = (Int32)(cropColumnLeft + scanLineLen - 1);

          if (cropColumnRight >= scannerFile.PixelsPerScanLine)
          {
            cropColumnRight = (Int32)(scannerFile.PixelsPerScanLine - 1);
            cropColumnLeft = (Int32)(cropColumnRight - scanLineLen + 1);
          }
        }

        ComputeParameters ();
        PaintWholePanel ();
        OnCropSettingsChanged ();
      }
    } /* SetLeftCrop */




    private void ReleaseRightCrop (object sender, System.EventArgs e)
    {
      if (scannerFile != null)
      {
        cropColumnRight = (Int32)(scannerFile.PixelsPerScanLine - 1);
        ComputeParameters ();
        PaintWholePanel ();
        OnCropSettingsChanged ();
      }
    } /* ReleaseLeftCrop */



    private void SetRightCrop (object sender, System.EventArgs e)
    {
      if (scannerFile != null)
      {
        Int32  scanLineLen = scannerFile.PixelsPerDisLine ();
        cropColumnRight = scannerFile.ScanColumnFromDisplayCol (rightMouseRow);
        if ((1 + cropColumnRight - cropColumnLeft) <= scanLineLen)
        {
          cropColumnLeft = (Int32)(cropColumnRight - scanLineLen + 1);
          if (cropColumnLeft < 0)
          {
            Int32  one = 1;
            cropColumnLeft = 0;
            cropColumnRight = scanLineLen - one;
          }
        }

        ComputeParameters ();
        PaintWholePanel ();
        OnCropSettingsChanged ();
      }
    } /* SetLeftCrop */



    public  void  ScrollToPropperPlaceInScannerFile (int  displayRow)
    {
      if  ((scannerFile == null)  ||  (displayRow == displayScanRowLeft))
        return;

      displayRow = Math.Max (0, displayRow);
        
      int  largestKnownDisplayRow =  ScanLineRowToDispalyRow (scannerFile.LargestKnownScanLine ());
        
      if  (scannerFile.FrameIndexBuilt  &&  (displayRow > (largestKnownDisplayRow - DisplayRowsToDisplay ())))
        displayRow = largestKnownDisplayRow - DisplayRowsToDisplay ();
    
      int  deltaDisplayRows = displayRow - displayScanRowLeft;
     
      if  (Math.Abs (deltaDisplayRows) < 200)
      {
        ScrollRows2 (deltaDisplayRows);
      }
      else
      {
        SetNewDisplayScanRowLeft (displayRow);
        PaintWholePanel ();
      }
    }  /* ScrollToPropperPlaceInScannerFile */



    public  void  SkipToNextStartStopPoint ()
    {
      if  (scannerFile == null)
        return;

      Int32  curScanLine = DisplayRowToScanLineRow (displayScanRowRight);
      Int32  targetDisplayRow = 0;
      UmiStartStopPoint nextStartStopPoint = scannerFile.StartStopPointSuccEntry (curScanLine);
      if (nextStartStopPoint == null)
        targetDisplayRow = ScanLineRowToDispalyRow (scannerFile.LargestKnownScanLine ()) - displayRowsToDisplay;
      else
        targetDisplayRow = ScanLineRowToDispalyRow (nextStartStopPoint.ScanLineNum);

      ScrollToPropperPlaceInScannerFile (targetDisplayRow - (displayRowsToDisplay / 2));
    }  /* SkipToNextStartStopPoint */



    public void SkipToPrevStartStopPoint ()
    {
      if (scannerFile == null)
        return;

      Int32  curScanLine = DisplayRowToScanLineRow (displayScanRowLeft);
      UmiStartStopPoint  prevStartStopPoint = scannerFile.StartStopPointPrevEntry (curScanLine);
      if  (prevStartStopPoint == null)
        ScrollToPropperPlaceInScannerFile (0);
      else
        ScrollToPropperPlaceInScannerFile (ScanLineRowToDispalyRow (prevStartStopPoint.ScanLineNum) - (displayRowsToDisplay / 2));
    }  /* SkipToPrevStartStopPoint */



    private void  panel_MouseDoubleClick (object sender, MouseEventArgs e)
    {
      if  (e.Button != MouseButtons.Left)
        return;

      ActiveTrainingLibraries.Model1ListOfClasses ();
      UmiParticleEntry  pe = SelectParticle (e.X, e.Y);
      if  (pe != null)
        OnParticleSelected (pe);
    }  /* panel_MouseDoubleClick */

    
    
    private void MonitorMouseClick (object sender, MouseEventArgs mea)
    {
      if  (mea.Button == MouseButtons.Right)
      {
        rightMouseCol = (Int32)mea.X;
        rightMouseRow = (Int32)mea.Y;
      }
    }


    private void OnSizeChanged (object sender, EventArgs e)
    {
      // Place code that is to be executed when ever this control is resized.

      ScannerFileViewerPanel_SizeChanged (sender, e);
    }



    private void ScannerFileViewerPanel_SizeChanged (object sender, EventArgs e)
    {
      if  (oldHeight == 0)
      {
        oldHeight = Height;
        oldWidth  = Width;
      }
      else
      {
        Int32  deltaHeight = Height - oldHeight;
        Int32  deltaWidth  = Width  - oldWidth;

        oldHeight = Height;
        oldWidth  = Width;

        panel.Height = panel.Height + deltaHeight;
        panel.Width  = panel.Width  + deltaWidth;

        IncludeExcludePanel.Top   += deltaHeight;
        IncludeExcludePanel.Width += deltaWidth;

        ComputeParameters ();
        BuildCountFont ();
        PaintWholePanel ();
      }
    }



    private void ScannerFileViewerPanel_Load (object sender, EventArgs e)
    {
      ScannerFileViewerPanel_SizeChanged (sender, e);
    }



    private void ScannerFileViewerPanel_VisibleChanged (object sender, EventArgs e)
    {

    }



    public  class  ParticleSelectedEventArgs: EventArgs
    {
      public  ParticleSelectedEventArgs (UmiParticleEntry  _particleEntry)
      {
        particleEntry = _particleEntry;
      }

      public  UmiParticleEntry  ParticleEntry  {get {return particleEntry;}}

      private  UmiParticleEntry  particleEntry;
    };



    public delegate void  StartPointAddedHandler       (int scanLine);
    public delegate void  StopPointAddedHandler        (int scanLine);
    public delegate void  StartStopPointDeletedHandler (int scanLine);
    public delegate void  StartStopPointChangeHandler  (int scanLine);
    public delegate void  CropSettingsChangedHandler   ();
    public delegate void  ParticleSelectedHandler      (object sender, ParticleSelectedEventArgs e);


    [Category ("Action")]
    [Description ("Occurs when user double clicks on particle.")]
    public event  ParticleSelectedHandler  ParticleSelected;

    protected virtual  void  OnParticleSelected (UmiParticleEntry  particleEntry)
    {
      if  (ParticleSelected != null)
        ParticleSelected (this, new ParticleSelectedEventArgs (particleEntry));  // Notify Subscribers
    }


    [Category ("Action")]
    [Description ("Fires when Start Point Added.")]
    public event  StartPointAddedHandler  StartPointAdded;

    protected virtual  void  OnStartPointAdded (int scanLine)
    {
      if  (StartPointAdded != null)
        StartPointAdded (scanLine);  // Notify Subscribers
    }



    [Category ("Action")]
    [Description ("Fires when Stop Point Added.")]
    public event  StopPointAddedHandler  StopPointAdded;

    protected virtual  void  OnStopPointAdded (int scanLine)
    {
      if  (StopPointAdded != null)
        StopPointAdded (scanLine);  // Notify Subscribers
    }



    [Category ("Action")]
    [Description ("Fires when Start/Stop Point Deleted.")]
    public event  StartStopPointDeletedHandler  StartStopPointDeleted;

    protected virtual  void  OnStartStopPointDeleted (int scanLine)
    {
      if  (StartStopPointDeleted != null)
        StartStopPointDeleted (scanLine);
    }



    [Category ("Action")]
    [Description ("Fires when Start or Stop Point is added or deleted.")]
    public event  StartStopPointChangeHandler  StartStopPointChange;

    protected virtual  void  OnStartStopPointChange (int scanLine)
    {
      if  (StartStopPointChange != null)
        StartStopPointChange (scanLine);
    }


    [Category ("Action")]
    [Description ("Occurs when ther is a change to crop setrtings.")]
    public event  CropSettingsChangedHandler  CropSettingsChanged;

    protected virtual  void  OnCropSettingsChanged ()
    {
      if  (CropSettingsChanged != null)
        CropSettingsChanged ();
    }

  }
}
