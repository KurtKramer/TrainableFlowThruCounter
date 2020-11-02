using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using System.Drawing;
using System.Drawing.Imaging;
using System.Threading;

using CounterManaged;
using CounterManagedRoutines;


 
namespace CounterFileViewer
{

  public  class  CounterFileViewerParameters 
  {
    private  Panel       panel    = null;
    private  Graphics    panelDC  = null;
    private  Bitmap      buffer   = null;
    private  Graphics    bufferDC = null;
    private  Color       backGroundColor;

    private  Int32       width = 0;
    private  Int32       height = 0;
    
    private  Int32       displayRowsToDisplay = 4096;
    
    private  Int32       displayRowTop = 0;
    private  Int32       displayRowBot = 4096;
    
    private  Int32       numPixelsOnEachRow = 0;  // will be width or smaller that allows a proper mutiple of ratio
    
    private  byte[]      displayRow = null;
    private  byte[][]    raster     = null;
    
    private  Int32       cropColumnLeft  = 0;
    private  Int32       cropColumnRight = 4095;

    private  UmiScannerFileBuffered  scannerFile = null;

    private  Pen[]                  colorValues = null;   // Pen color to use for each pixel value;  normaly 
                                                          // each value is set to the appropriate grayscale value.
                                                          // except when (colorize == true) selectd then Colors
                                                          // are used for the 7 levels of grayscale that the Scanner
                                                          // camera produces.
      
    private  BlobList               blobs = null;
    private  bool                   extractBlobs = false;
    private  int                    blobMinSize  = 250;
    
    private  bool                   colorize = false;

    private  ContextMenu            scannerPanelContextMenu = null;

    private  UmiParticleEntryList   particlesOnCurrentScreen = null;
  

    Int32  rightMouseRow = 0;
    Int32  rightMouseCol = 0;
        
    public   int    BlobMinSize          ()  {return  blobMinSize;}
    public   Int32  CropColemnLeft       ()  {return  cropColumnLeft;}
    public   Int32  CropColemnRight      ()  {return  cropColumnRight;}
    public   Int32  DisplayRowsToDisplay ()  {return  displayRowsToDisplay;}
    public   bool   ExtractBlobs         ()  {return  extractBlobs;}
    public   long   DisplayRowBot        ()  {return  displayRowBot;}
    public   long   DisplayRowTop        ()  {return  displayRowTop;}
    
    public   void  BlobMinSize  (int   _blobMinSize)   {blobMinSize  = _blobMinSize;}
    public   void  ExtractBlobs (bool  _extractBlobs)  {extractBlobs = _extractBlobs;}



    public  float  Ratio ()  
    {
      if  (scannerFile == null)
        return 1.0f;
      else
        return scannerFile.Ratio;
    }



    public  CounterFileViewerParameters (Panel  _panel)
    {
      panel = _panel;
      panel.MouseClick += new MouseEventHandler (MonitorMouseClick);
      panelDC = panel.CreateGraphics ();
      BuildPenValues ();
      ComputeParameters ();
    }



    public  void  Dispose ()
    {
      if  (buffer != null)
      {
        buffer.Dispose ();
        buffer = null;
        bufferDC.Dispose ();
        bufferDC = null;
      }
      
      raster      = null;
      displayRow  = null;
      colorValues = null;
    }  /* Dispose */



    private  void  BuildPenValues ()
    {
      if  (colorize)
      {
        BuildColorizedPenValues ();
        return;
      }

      int x;

      colorValues = new Pen[256];
      for (x = 0; x < 256; x++)
      {
        int y = 255 - x;

        //// KKKK  get rid of the next few lines of code;  they were only meant to  h
        //int  zed = Math.Max (0, y - (256 - 16));
        //zed = zed * 16;
        //colorValues[x] = new Pen(Color.FromArgb(zed, zed, zed));

        colorValues[x] = new Pen(Color.FromArgb(y, y, y));
      }

      backGroundColor = Color.FromArgb (255, 255, 255);
    }  /* BuildPenValues */



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
    }



    public void   ComputeParameters ()
    {
      int  x = 0;

      double  oldRatio = 1.0;

      int  pixelsPerScanLine = 2048;
      if  (scannerFile != null)
        pixelsPerScanLine = scannerFile.PixelsPerScanLine;

      if  (numPixelsOnEachRow != 0)
        oldRatio = (double)pixelsPerScanLine / (double)numPixelsOnEachRow;
      if  (scannerFile != null)
        oldRatio = scannerFile.Ratio;
      
      Int32  actualRowTop = (Int32)((double)displayRowTop * oldRatio);
   
      width   = panel.Width;
      height  = panel.Height;

      numPixelsOnEachRow = width;

      if  (scannerFile != null)
        pixelsPerScanLine = scannerFile.PixelsPerScanLine;
      float  ratio = pixelsPerScanLine / numPixelsOnEachRow;    
      if  (scannerFile != null)
      {
        scannerFile.SetPixelsPerDisplayLine (numPixelsOnEachRow, cropColumnLeft, cropColumnRight);
        ratio = scannerFile.Ratio;
      }
       
      if  (buffer != null)
        buffer.Dispose();

      panelDC = panel.CreateGraphics ();
      buffer = new Bitmap (panel.Width, panel.Height);
      bufferDC = Graphics.FromImage (buffer);
      
      displayRowsToDisplay = height;
      displayRowBot = displayRowTop + displayRowsToDisplay - 1;
      
      int  scannerPixelsThatCanFitOnARow = (int)Math.Ceiling ((float)numPixelsOnEachRow * ratio);
      displayRow = new byte[numPixelsOnEachRow];      
      
      raster = new byte[height][];
      for  (x = 0;  x < height;  x++)
        raster[x] = new byte[width];

      SetUpScannerPanelContectMenu ();

      Int32  newDisplayRowTop = (Int32)((double)actualRowTop / (double)ratio);
      SetNewDisplayRowTop (newDisplayRowTop);
    }  /* ComputeParameters */
  
  
  
    public  void  SetNewDisplayRowTop (Int32 _displayRowTop)
    {
      displayRowTop = _displayRowTop;
      displayRowBot = displayRowTop + displayRowsToDisplay - 1;
    }  /* SetNewDisplayRowTop */


    
    public  bool  Colorize
    {
      get  {return  colorize;}
      set  
      {
        colorize = value;
        BuildPenValues ();
      }
    }  /* Colorize */

    

    public  void  SetScannerFile (UmiScannerFileBuffered   _scannerFile)
    {
      scannerFile = _scannerFile;
      displayRowTop = 0;

      if  (scannerFile == null)
      {
        cropColumnLeft  = 0;
        cropColumnRight = 4095;
      }
      else
      {
        cropColumnLeft  = scannerFile.CropColLeft ();
        cropColumnRight = scannerFile.CropColRight ();
      }

      ComputeParameters ();
    }  /* SetScannerFile */


  
    private  void  ComputeARowToDisplay (Int32   displayRowNum,
                                         byte[]  displayRow
                                        )
    {
      if  (scannerFile == null)
      {
        Int32  col = 0;
        for  (col = 0;  col < displayRow.Length;  col++)
          displayRow[col] = 0;
        return;
      }
      
      byte[][]  displayRows = new byte[1][];
      displayRows[0] = new byte[numPixelsOnEachRow];
      
      scannerFile.GetDisplayRows (displayRowNum, displayRowNum, displayRows);
      byte[]    firstDisplayRow = displayRows[0];
      
      int  lineLen = Math.Min (displayRow.Length, firstDisplayRow.Length);
      
      for  (int x = 0;  x < lineLen;  x++)
        displayRow[x] = (byte)firstDisplayRow[x];
    }  /* ComputeARowToDisplay */
  
  
  
    private  void  PaintARow (int       screenRow,
                              Graphics  dc
                             )
    {
      ComputeARowToDisplay (screenRow + displayRowTop, displayRow);
      
      int  col = 0;
      
      byte  oldColor = 0;
      byte  curColor = 0;
      int   startCol = 0;
      
      while  (col < (displayRow.Length - 1))
      {
        oldColor = displayRow[col];
        curColor = oldColor;
        startCol =  col;
        
        while  ((oldColor == curColor)  &&  (col < (displayRow.Length - 1)))
        {
          col++;
          curColor = displayRow[col];
        }
        
        int  endCol = col - 1;

        if  (oldColor != 0)
        {
          //Brush b = new SolidBrush (colorValues[oldColor].Color);
          //dc.FillRectangle (b, startCol, screenRow, 1 + (endCol - startCol), 1);
          dc.DrawLine (colorValues[oldColor], startCol, screenRow, endCol + 1, screenRow);
        }
      }
    }  /* PaintARow */


    
    public static bool  PaintARow2 (Bitmap  b,
                                    int     row,
                                    byte[]  rowData
                                   )
    {
      // GDI+ return format is BGR, NOT RGB. 
      BitmapData bmData = b.LockBits (new Rectangle(0, 0, b.Width, b.Height),
                                      ImageLockMode.ReadWrite,
                                      PixelFormat.Format24bppRgb
                                     );

      int stride = bmData.Stride;
      System.IntPtr Scan0 = bmData.Scan0;

      unsafe
      {
        int nOffset = stride - b.Width * 3;
        int bytesPerRow = b.Width * 3 + nOffset;
        int col = 0;
        byte  pixelValue;

        byte* ptr = (byte*)(void*)Scan0 + bytesPerRow * row;

        for (col = 0;  col < rowData.Length;  col++)
        {
          pixelValue = (byte)((byte)255 - rowData[col]);
          ptr[0] = pixelValue; ptr++;
          ptr[0] = pixelValue; ptr++;
          ptr[0] = pixelValue; ptr++;
        }
      }  /* Unsafe */

      b.UnlockBits(bmData);

      return true;
    }  /* PaintARow2 */
    


    public void  PaintWholePanel ()
    {
      //panelDC.Clear (backGroundColor);
      bufferDC.Clear(backGroundColor);
      
      if  (scannerFile != null)
      {
        scannerFile.GetDisplayRows (displayRowTop, displayRowBot, raster);

        particlesOnCurrentScreen = scannerFile.GetParticlesForScanLineRange (displayRowTop, displayRowBot);

        /*
        try
        {
          scannerFile.GetDisplayRows (displayRowTop, displayRowBot, raster);
        }
        catch  (Exception  e)
        {
          MessageBox.Show (e.ToString (), "PaintWholePanel   Error calling 'GetDisplayRows'");
        }
        */
      }
      
      // GDI+ return format is BGR, NOT RGB. 
      BitmapData bmData = buffer.LockBits (new Rectangle(0, 0, width, height),
                                           ImageLockMode.ReadWrite,
                                           PixelFormat.Format24bppRgb
                                          );

      int stride = bmData.Stride;
      System.IntPtr Scan0 = bmData.Scan0;

      unsafe
      {
        int  nOffset = stride - width * 3;
        int  bytesPerRow = width * 3 + nOffset;

        byte pixelValue    = 0;
        byte oldPixelValue = 0;

        byte  red   = 255;
        byte  green = 255;
        byte  blue  = 255;
        
        int  row, col;

        byte* ptr = (byte*)(void*)Scan0;

        for  (row = 0;  row < height;  row++)
        {
          byte[]  rowData = raster[row];
          
          for  (col = 0;  col < rowData.Length;  col++)
          {
            //pixelValue = (byte)((byte)255 - rowData[col]);
            //ptr[0] = pixelValue; ptr++;
            //ptr[0] = pixelValue; ptr++;
            //ptr[0] = pixelValue; ptr++;

            pixelValue = (byte)rowData[col];
            if  (oldPixelValue != pixelValue)
            {
              oldPixelValue = pixelValue;
              red   = colorValues[pixelValue].Color.R;
              green = colorValues[pixelValue].Color.G;
              blue  = colorValues[pixelValue].Color.B;
            }

            ptr[0] = blue;   ptr++;
            ptr[0] = green;  ptr++;
            ptr[0] = red;    ptr++;
          }
          
          ptr += nOffset;
        }
      }  /* Unsafe */

      buffer.UnlockBits (bmData);

      panelDC.DrawImageUnscaled (buffer, 0, 0);

      DisplayAllParticles (panelDC);

      if  (extractBlobs)
        ExtractBlobsFromCurrentScreen ();
      else
        ClearBobs ();

    }  /* PaintWholePanel*/



    public  void  ClearBobs ()
    {
      if  (blobs != null)
      {
        blobs.Dispose ();
        blobs = null;
      }
    }  /* ClearBobs */



    private  void  ExtractBlobsFromCurrentScreen ()
    {
      if  (blobs != null)
      {
        blobs.Dispose ();
        blobs = null;
      }

      if  (raster == null)
        return;

      BlobDetector bd = new BlobDetector (raster, (int)((float)blobMinSize / Math.Ceiling (scannerFile.Ratio * scannerFile.Ratio)));
      blobs = bd.ExtractBlobs ();
      bd.Dispose ();

      //Lets Paint Boxes for each Blob
      foreach (Blob b in blobs)  
      {
        PaintRectangle (panelDC,  Color.Red, b);
        PaintRectangle (bufferDC, Color.Red, b);
      }
    }  /* ExtractBlobsFromCurrentScreen */



    public static bool  ScrollUp (Bitmap  b, 
                                  int     numRows
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
      
        byte*  destPtr = (byte*)(void*)Scan0;
        byte*  srcPtr =  ((byte*)(void*)Scan0) + bytesPerRow * numRows;

        int nWidth = b.Width * 3;

        for (int y = 0; y < (b.Height - numRows); ++y)
        {
          for (int x = 0; x < nWidth; ++x)
          {
            destPtr[0] = srcPtr[0];
            ++srcPtr;
            ++destPtr;
          }
          
          srcPtr  += nOffset;
          destPtr += nOffset;
        }
      }  /* Unsafe */

      b.UnlockBits (bmData);
      
      return true;
    }  /* ScrollUp */



    public  void  ScrollRows2 (int  numRows)
    {
      int  row = 0;
      
      if  (numRows < 0)
      {
        // Scroll down
        Bitmap    newBuffer   = new Bitmap (buffer.Width, buffer.Height);
        Graphics  newBufferDC = Graphics.FromImage (newBuffer);
        newBufferDC.Clear (backGroundColor);
        newBufferDC.DrawImageUnscaled (buffer, 0, -numRows);
        
        SetNewDisplayRowTop (displayRowTop + numRows);
        
        for  (row = 0;  row < (-numRows);  row++)
          PaintARow (row, newBufferDC);

        buffer.Dispose ();
        bufferDC.Dispose ();
      
        buffer   = newBuffer;
        bufferDC = newBufferDC;
      }
      
      else
      {
        // Scroll Up
        ScrollUp (buffer, numRows);
        bufferDC.FillRectangle (new SolidBrush (backGroundColor),
                                new Rectangle (0, (height - numRows), buffer.Width, numRows)
                               );
        
        SetNewDisplayRowTop (displayRowTop + numRows);
        for (row = height - numRows;  row < height;  row++)
          PaintARow (row, bufferDC);
      }

      panelDC.DrawImageUnscaled (buffer, 0, 0);
      {
        // Scroll Blobs
        if  (blobs != null)
        {
          foreach  (Blob  b in blobs)
          {
            b.Scroll (numRows);
            PaintRectangle (panelDC, Color.Red, b);
          }
        }

        DisplayAllParticles (panelDC);
      }

    }  /* ScrollRows2 */



    public  void  AddUserSpecifiedBlob (int tlCol,  int tlRow,
                                        int brCol,  int brRow
                                       )
    {
      if  (blobs == null)
        blobs = new BlobList (true);
      

      Blob b = new Blob (-1, tlCol, tlRow, brCol, brRow);
      blobs.Add (b);
    }



    public  UmiParticleEntry  SelectParticle (Int32  displayCol,
                                              Int32  displayRow
                                             )
    {
      if  (particlesOnCurrentScreen == null)
        return null;

      Int32  scannerRow = (Int32)((float)((displayRowTop + displayRow) * Ratio ()));
      //int scannerRow = (int)((float)(displayRow) * Ratio ());
      Int32 scannerCol = (Int32)((Int32)cropColumnLeft + (Int32)(0.5f + (float)displayCol * Ratio ()));

      return particlesOnCurrentScreen.FindParticle (scannerRow, (Int16)scannerCol);
    }



    public  UmiRaster  GetRasterForParticle (UmiParticleEntry pe)
    {
      if  (pe == null)
        return null;

       Int32  scanLineStart = pe.ScannerRow;
       Int32  scanLineEnd   = pe.ScannerRow + pe.Height - 1;

       byte[][] rows = scannerFile.GetScanLines (scanLineStart, scanLineEnd);
       if (rows == null)
         return null;
       
       Int32  height = pe.Height;
       Int32  width  = pe.Width;
       
       byte[][]  raster = new byte[height][];

       Int32 row, col, scannerCol;
       for  (row = 0;  row < height;  row++)
       {
         byte[]  rowData = rows[row];
         
         raster[row] = new byte[width];
         
         scannerCol = pe.ScannerCol;
         col = 0;;
         
         while  (col < width)
         {
           raster[row][col] = rowData[scannerCol];
           col++;
           scannerCol++;
         }
       }

       return new UmiRaster (raster); 
    } 



    public  Blob  SelectedBlob (int col,
                                int row
                               )
    {
      if  (blobs == null)
        return  null;

      foreach  (Blob  b in blobs)
      {
        if  ((col >= b.ColLeft)  &&  (col <= b.ColRight)  &&  (row >= b.RowBot)  &&  (row <= b.RowTop))
          return  b;
      }

      return  null;
    }



    public  void  PaintFromBuffer ()
    {
      if  (buffer == null)
        PaintWholePanel ();
      else
        panelDC.DrawImageUnscaled (buffer, 0, 0);
    }



    public  void  PaintAllBlobs ()
    {
      if  (blobs == null)
        return;

      foreach  (Blob b in blobs)
      {
        if  (b != null)
          PaintRectangle (panelDC, Color.Red, b);
      }
    }  /* PaintAllBlobs */



    private  void  PaintRectangle  (Graphics  g,
                                    Color     lineColor,
                                    Blob      b
                                   )
    {
      int  colLeft  = Math.Max (b.ColLeft,  0);
      int  colRight = Math.Min (b.ColRight, width  - 1);
      int  rowTop   = Math.Min (b.RowTop,   height - 1);
      int  rowBot   = Math.Max (b.RowBot,   0);
      
      Pen p = new Pen (lineColor);

      g.DrawLine (p, colLeft,  rowTop, colRight, rowTop);
      g.DrawLine (p, colRight, rowTop, colRight, rowBot);
      g.DrawLine (p, colRight, rowBot, colLeft,  rowBot);
      g.DrawLine (p, colLeft,  rowBot, colLeft,  rowTop);
    }  /* PaintRectangle */

    

    private  void  DisplayAllParticles (Graphics  g)
    {
      if  (particlesOnCurrentScreen == null)
        return;

      foreach  (UmiParticleEntry pe in particlesOnCurrentScreen)
        PaintParticle  (g, pe);
    }



    private  void  PaintParticle  (Graphics          g,
                                   UmiParticleEntry  pe
                                  )
    {
      float ratio = Ratio ();

      int  rowTop = (int)((long)(0.5f + (float)pe.ScannerRow / ratio) - displayRowTop);
      int  rowBot =  rowTop + (int)(pe.Height / ratio);

      int  colLeft  = (int)(0.5f + (float)(pe.ScannerCol - cropColumnLeft) /ratio);
      int  colRight = colLeft + (int)((float)pe.Width / ratio);
      
      Pen p = null;

      String  name = pe.PredClass.Name;
      if  ((name.Length > 5)  &&  (name.Substring (0, 6) == "Shrimp"))
        p = new Pen (Color.Green);
      else
        p = new Pen (Color.Red);

      g.DrawLine (p, colLeft,  rowTop, colRight, rowTop);
      g.DrawLine (p, colRight, rowTop, colRight, rowBot);
      g.DrawLine (p, colRight, rowBot, colLeft,  rowBot);
      g.DrawLine (p, colLeft,  rowBot, colLeft,  rowTop);
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
                                               ref Int32  colLeft
                                              )

    {
      if  (scannerFile == null)
        return  null;

      int  x1 = Math.Max (0, blob.ColLeft - 1);
      int  y1 = Math.Max (0, blob.RowBot  - 1);
      int  x2 = Math.Min (panel.Width - 1,  blob.ColRight + 1);
      int  y2 = Math.Min (panel.Height - 1, blob.RowTop   + 1);
    
       firstScanLine = (Int32)((displayRowTop + y1) * scannerFile.Ratio);
       colLeft = (Int32)((Int32)(Math.Floor ((float)x1 * scannerFile.Ratio)) + (Int32)(scannerFile.CropColLeft ()));

       Int32  lastScanLine = (Int32)((float)(displayRowTop + y2 + 1) * scannerFile.Ratio + 0.5f);

       Int32  colRight     = (Int32)(((float)(x2 + 1) * scannerFile.Ratio) + (Int32)(scannerFile.CropColLeft ()));
       if  (colRight >= scannerFile.PixelsPerScanLine)
         colRight = (Int32)(scannerFile.PixelsPerScanLine - 1);
       
       if  (colLeft >= colRight)
         return null;

       byte[][] rows = scannerFile.GetScanLines (firstScanLine, lastScanLine);
       
       int  height = (int)(1 + lastScanLine - firstScanLine);
       int  width  = (int)(1 + colRight     - colLeft);
       
       byte[][]  raster = new byte[height][];

       int row, col, scannerCol;
       for  (row = 0;  row < height;  row++)
       {
         byte[]  rowData = rows[row];
         
         raster[row] = new byte[width];
         
         scannerCol = colLeft;
         col = 0;;
         
         while  (col < width)
         {
           raster[row][col] = rowData[scannerCol];
           col++;
           scannerCol++;
         }
       }

       return  raster;
    }   /* ExtractImageFromScanner */                                
                         
    
        
    private  void  SetUpScannerPanelContectMenu ()
    {
      scannerPanelContextMenu = new ContextMenu ();

      if  (scannerFile != null)
      {
        scannerPanelContextMenu.MenuItems.Add ("Set LeftCrop", new EventHandler (SetLeftCrop));
        if  (scannerFile.CropColLeft () > 0)
          scannerPanelContextMenu.MenuItems.Add ("Release LeftCrop", new EventHandler (ReleaseLeftCrop));
          
        scannerPanelContextMenu.MenuItems.Add ("Set RightCrop", new EventHandler (SetRightCrop));
        if  (scannerFile.CropColRight () < (scannerFile.PixelsPerScanLine - 1))
          scannerPanelContextMenu.MenuItems.Add ("Release RightCrop", new EventHandler (ReleaseRightCrop));

        scannerPanelContextMenu.MenuItems.Add ("Display Column", new EventHandler (DisplayColumn));
      }

      panel.ContextMenu = scannerPanelContextMenu;
    }



    private  void  ReleaseLeftCrop (object sender, System.EventArgs e)
    {
      if  (scannerFile != null)
      {
        cropColumnLeft = 0;
        ComputeParameters ();
        PaintWholePanel ();
      }
    } /* ReleaseLeftCrop */



    private  void  SetLeftCrop (object sender, System.EventArgs e)
    {
      if  (scannerFile != null)
      {
        Int32  scanLineLen = scannerFile.PixelsPerDisLine ();
        cropColumnLeft = scannerFile.ScanColumnFromDisplayCol (rightMouseCol);
        if  ((1 + cropColumnRight - cropColumnLeft) <= scanLineLen)
        {
          cropColumnRight = (Int32)(cropColumnLeft + scanLineLen - 1);
          
          if  (cropColumnRight >= scannerFile.PixelsPerScanLine)
          {
            cropColumnRight = (Int32)(scannerFile.PixelsPerScanLine - 1);
            cropColumnLeft  = (Int32)(cropColumnRight - scanLineLen + 1);
          }
        }

        ComputeParameters ();
        PaintWholePanel ();
      }
    } /* SetLeftCrop */



    private  void  ReleaseRightCrop (object sender, System.EventArgs e)
    {
      if  (scannerFile != null)
      {
        cropColumnRight = (Int32)(scannerFile.PixelsPerScanLine - 1);
        ComputeParameters ();
        PaintWholePanel ();
      }
    } /* ReleaseRightCrop */



    private  void  DisplayColumn (object sender, System.EventArgs e)
    {
      int  col = scannerFile.ScanColumnFromDisplayCol (rightMouseCol);
    
      int  actualRowTop = (Int32)((double)displayRowTop * Ratio ());

      byte[][]  raster = scannerFile.GetScanLines  (actualRowTop, actualRowTop + 512);
      TemporalDisplay  td = new TemporalDisplay (raster, col, cropColumnLeft, cropColumnRight, true);
      td.ShowDialog (panel);
    }



    private  void  SetRightCrop (object sender, System.EventArgs e)
    {
      if  (scannerFile != null)
      {
        Int32  scanLineLen = scannerFile.PixelsPerDisLine ();
        cropColumnRight = scannerFile.ScanColumnFromDisplayCol (rightMouseCol);
        if  ((1 + cropColumnRight - cropColumnLeft) <= scanLineLen)
        {
          cropColumnLeft = (Int32)(cropColumnRight - scanLineLen + 1);
          if  (cropColumnLeft < 0)
          {
            Int32  one = 1;
            cropColumnLeft = 0;
            cropColumnRight = scanLineLen - one;
          }
        }

        ComputeParameters ();
        PaintWholePanel ();
      }
    } /* SetRightCrop */



    private  void  MonitorMouseClick (object  sender, MouseEventArgs mea)
    {
      if  (mea.Button == MouseButtons.Right)
      {
        rightMouseCol = (Int32)mea.X;
        rightMouseRow = (Int32)mea.Y;
      }
    }

  }  /* CounterFileViewerParameters */
}
