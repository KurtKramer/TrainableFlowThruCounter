using System;
using System.Collections.Generic;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

using LarcosCounterManaged;
using LarcosManagedRoutines;


namespace LarcosCounter
{
  class ThumbNailImageCell: DataGridViewCell
  {

    public override  Type FormattedValueType { get  {return  typeof (UmiParticleEntry);} } 


    public bool GetThumbnailImageAbort()
    {
      return true;
    }  /* GetThumbnailImageAbort */


    private  System.IntPtr  thumbNailCallBackData = IntPtr.Zero;

    private  static  UmiScannerFileBuffered  curScannerFile      = null;
    private  static  UmiOperatingParameters  operatingParameters = null;
    private  static  UmiClass                baseClass           = null;



    public  static  void  SetCurScannerFile  (UmiScannerFileBuffered  _curScannerFile)
    {
      curScannerFile = _curScannerFile;
    }


    public  static  void  SetOperatingParameters (UmiOperatingParameters  _operatingParameters)
    {
      operatingParameters = _operatingParameters;
    }


    public  static  void  SetBaseClass (UmiClass  _baseClass)
    {
      baseClass = _baseClass;
    }


    private  Image  MakeImageFit (Image  src, 
                                  int    maxHeight,
                                  int    maxWidth
                                 )
    {
      int  destHeight = src.Height;
      int  destWidth  = src.Width;

      if  ((destHeight <= maxHeight)  &&  (destWidth <= maxWidth))
        return  src;

      float  heightFact = (float)destHeight / (float)maxHeight;
      float  widthFact  = (float)destWidth  / (float)maxWidth;

      float  factor = 0.0f;
      if  (heightFact < widthFact)
        factor = heightFact;
      else
        factor = widthFact;

      destWidth  = Math.Min (maxWidth,  (int)((float)destWidth  * factor + 0.5f));
      destHeight = Math.Min (maxHeight, (int)((float)destHeight * factor + 0.5f));

      return  src.GetThumbnailImage (destWidth, destHeight, GetThumbnailImageAbort, thumbNailCallBackData);
    }  /* MakeImageFit */



    //********************************************************************************************************
    //*                        Code to support the creation of a single thumbnail image.                     *
    //********************************************************************************************************
    static Font   thumbNailFont            = new Font (FontFamily.GenericSansSerif, 8.25f, FontStyle.Regular, GraphicsUnit.Point);
    static Brush  thumbNailLabelBrush      = new SolidBrush  (Color.Gray);
    static Brush  thumbNailDataBrush       = new SolidBrush  (Color.Black);
    static Brush  thumbNailSelectedBrush   = new SolidBrush  (Color.Red);
    static Pen    thumbNailSelectedPen     = new Pen         (new SolidBrush (Color.LightGray));
    static Brush  thumbNailBackGround      = new SolidBrush  (Color.White);
    static Pen    thumbNailBorderPen       = new Pen         (Color.Black);


    static StringFormat  stringFormat = new StringFormat ();



    protected override void Paint (Graphics                  graphics,
                                   Rectangle                 clipBounds,
                                   Rectangle                 cellBounds,
                                   int                       rowIndex,
                                   DataGridViewElementStates cellState,
                                   object                    value,
                                   object                    formattedValue,
                                   string                    errorText,
                                   DataGridViewCellStyle     cellStyle,
                                   DataGridViewAdvancedBorderStyle advancedBorderStyle,
                                   DataGridViewPaintParts    paintParts
                                  )
    {
      // Call the base class method to paint the default cell appearance.
      base.Paint (graphics,   
                  clipBounds,     
                  cellBounds, 
                  rowIndex,   
                  cellState,  
                  value,      
                  formattedValue, 
                  errorText,  
                  cellStyle,
                  advancedBorderStyle, 
                  paintParts
                );

      bool  selected = ((cellState & DataGridViewElementStates.Selected) == 
                       DataGridViewElementStates.Selected);
                       
      int  borderWidth = 1;
      graphics.FillRectangle (selected ? thumbNailSelectedBrush : thumbNailBackGround, cellBounds);
      
      if  ((value == null)  ||  (value.GetType () != typeof (UmiParticleEntry)))
      {
        graphics.DrawRectangle (thumbNailBorderPen, cellBounds);
        return;
      } 

      UmiParticleEntry  pe = (UmiParticleEntry)value;

      int  availWidth  = cellBounds.Width  - 2 * borderWidth - 2;
      int  availHeight = cellBounds.Height - 2 * borderWidth;
      
      int  heightNeededForText = thumbNailFont.Height;  // (Probability and Break-Tie) on one line
      if  (pe.PredClass != baseClass)
        heightNeededForText += thumbNailFont.Height;

      int  heightAvailForImage = availHeight - heightNeededForText;
 
      Image displayImage = null;

      lock  (curScannerFile)
      {
        if  (operatingParameters == null)
        {
          operatingParameters = new UmiOperatingParameters ();
          operatingParameters.UpdateFromScannerFile (curScannerFile.ScannerFile ());
        }

        displayImage = curScannerFile.GetThumbNailForParticle (pe, operatingParameters, heightAvailForImage, availWidth).BuildBitmap ();
      }

      // we want to paint the image in the center
      int  paintRow = cellBounds.Y + borderWidth + (heightAvailForImage - displayImage.Height) / 2;
      int  paintCol = cellBounds.X + borderWidth + (availWidth          - displayImage.Width)  / 2;

      graphics.DrawImage (displayImage, paintCol, paintRow, displayImage.Width, displayImage.Height);

      int  curPixelRow = heightAvailForImage + cellBounds.Y;  // This is where 1st line of text must go
      int  leftCol = cellBounds.X + borderWidth;

      graphics.DrawString ("P:", thumbNailFont, thumbNailLabelBrush, leftCol + 1, curPixelRow);
      graphics.DrawString ((pe.Probability).ToString ("##0.0%"), thumbNailFont, thumbNailDataBrush, leftCol  + 20, curPixelRow);

      graphics.DrawString ("B:", thumbNailFont, thumbNailLabelBrush, leftCol + 71, curPixelRow);
      graphics.DrawString ((pe.BreakTie).ToString ("##0.0%"), thumbNailFont, thumbNailDataBrush, leftCol + 90, curPixelRow);
      curPixelRow += thumbNailFont.Height;
      if  (pe.PredClass != baseClass)
      {
        graphics.DrawString ("C:", thumbNailFont, thumbNailLabelBrush, leftCol + 1, curPixelRow);
        graphics.DrawString (pe.PredClass.Name, thumbNailFont, thumbNailLabelBrush, leftCol + 20, curPixelRow);
      }

      graphics.DrawRectangle (thumbNailBorderPen, cellBounds);
      for  (int  zed = 1;  zed < borderWidth;  zed++)
      {
        Rectangle  r = new Rectangle (cellBounds.X + borderWidth, cellBounds.Y + borderWidth, cellBounds.Width - 2 * borderWidth, cellBounds.Height - 2 * borderWidth);
        graphics.DrawRectangle (thumbNailBorderPen, cellBounds);
      }

      return;
    }  /* Paint */




    
    protected  override  void  OnDoubleClick (DataGridViewCellEventArgs e)
    {
      if  (DataGridView == null)
        return;

      if  (e.ColumnIndex >= DataGridView.Columns.Count)
        return;

      AuditorByClass.ThumbNailImageColumn  tnic = ((AuditorByClass.ThumbNailImageColumn)(this.DataGridView.Columns[e.ColumnIndex]));

      IWin32Window parent = tnic.Parent;
      if  (parent.GetType () != typeof (AuditorByClass))
        return;
      
      AuditorByClass  pc = (AuditorByClass)parent;
      if  (pc == null)
        return;

      pc.ThumbnailGrid_CellContentDoubleClick (DataGridView, e);
    }  /* OnDoubleClick */


    protected override void OnMouseDoubleClick(DataGridViewCellMouseEventArgs e)
    {
      base.OnMouseDoubleClick(e);
    }

  }  /* ThumbNailImageCell */
}
