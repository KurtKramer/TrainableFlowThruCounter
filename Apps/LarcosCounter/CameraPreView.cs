using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
//using System.Linq;
using System.Text;
using System.Windows.Forms;

//using  UnManagedInterface;
using  CounterManaged;

namespace LarcosCounter
{
  public partial class CameraPreView : Form
  {
    private  CounterManagerWrapper  cameraManager = null;
    private  int                          oldHeight = 0;
    private  int                          oldWidth  = 0;

    public CameraPreView (CounterManagerWrapper  _cameraManager)
    {
      cameraManager = _cameraManager;
      InitializeComponent();
    }

    int  cameraFrameCount = 0;

    private void RefreshTimer_Tick(object sender, EventArgs e)
    {
      Count.Text = cameraManager.ParticlesCounted ().ToString ("###,###,##0");
      String  classLabel = null;
      Bitmap  bm = cameraManager.GetLastParticle (CameraFrame.Height, CameraFrame.Width, ref classLabel);
      if  (bm == null)
      {
        RefreshTimer.Interval = 100;
        return;
      }

      if  (classLabel == null)
        classLabel = "";

      switch  (cameraFrameCount % 6)
      {
        case 0:  CameraFrame.SizeMode = PictureBoxSizeMode.CenterImage;
                 CameraFrame.Image = bm;
                 CameraFrame0label.Text = classLabel;
                 break;
        case 1:  CameraFrame1.SizeMode = PictureBoxSizeMode.CenterImage;
                 CameraFrame1.Image = bm;
                 CameraFrame1label.Text = classLabel;
                 break;
        case 2:  CameraFrame2.SizeMode = PictureBoxSizeMode.CenterImage;
                 CameraFrame2.Image = bm;
                 CameraFrame2label.Text = classLabel;
                 break;
        case 3:  CameraFrame3.SizeMode = PictureBoxSizeMode.CenterImage;
                 CameraFrame3.Image = bm;
                 CameraFrame3label.Text = classLabel;
                 break;
        case 4:  CameraFrame4.SizeMode = PictureBoxSizeMode.CenterImage;
                 CameraFrame4.Image = bm;
                 CameraFrame4label.Text = classLabel;
                 break;
        case 5:  CameraFrame5.SizeMode = PictureBoxSizeMode.CenterImage;
                 CameraFrame5.Image = bm;
                 CameraFrame5label.Text = classLabel;
                 break;
      }

      RefreshTimer.Interval = 20;
      cameraFrameCount++;
    }


    private void ResetButton_Click(object sender, EventArgs e)
    {
      DialogResult  dr = MessageBox.Show (this, "Do you want to reset the counter?", "Larcos Shrimp Counter", MessageBoxButtons.YesNo);
      if  (dr == DialogResult.Yes)
        cameraManager.ResetCounts ();
    }


    private void  ComputeFramePositions ()
    {
      int h = ResetButton.Top - Count.Bottom - 12;
      int w = Width  - 30;

      int rowsPerFrame = (int)(0.5 + h / 2.0);
      int colsPerFrame = (int)(0.5 + w / 3.0);

      int  frameHeight = rowsPerFrame - (12 + 22);
      int  frameWidth  = colsPerFrame - 10;

      int  frameTopRow0 = 97 + (0 * rowsPerFrame);
      int  frameTopRow1 = 97 + (1 * rowsPerFrame);

      int  classLabelTopRow0 = frameTopRow0 + frameHeight;
      int  classLabelTopRow1 = frameTopRow1 + frameHeight;

      int  col0 = 10 + (0 * colsPerFrame);
      int  col1 = 10 + (1 * colsPerFrame);
      int  col2 = 10 + (2 * colsPerFrame);

      CameraFrame.Height = frameHeight;
      CameraFrame.Width  = frameWidth;
      CameraFrame.Left   = col0;
      CameraFrame.Top    = frameTopRow0;
      CameraFrame0label.Top = classLabelTopRow0;
      CameraFrame0label.Width = frameWidth;
      CameraFrame0label.Left  = col0;

      CameraFrame1.Height = frameHeight;
      CameraFrame1.Width  = frameWidth;
      CameraFrame1.Left   = col1;
      CameraFrame1.Top    = frameTopRow0;
      CameraFrame1label.Top = classLabelTopRow0;
      CameraFrame1label.Width = frameWidth;
      CameraFrame1label.Left  = col1;

      CameraFrame2.Height = frameHeight;
      CameraFrame2.Width  = frameWidth;
      CameraFrame2.Left   = col2;
      CameraFrame2.Top    = frameTopRow0;
      CameraFrame2label.Top = classLabelTopRow0;
      CameraFrame2label.Width = frameWidth;
      CameraFrame2label.Left  = col2;

      CameraFrame3.Height = frameHeight;
      CameraFrame3.Width  = frameWidth;
      CameraFrame3.Left   = col0;
      CameraFrame3.Top    = frameTopRow1;
      CameraFrame3label.Top = classLabelTopRow1;
      CameraFrame3label.Width = frameWidth;
      CameraFrame3label.Left  = col0;

      CameraFrame4.Height   = frameHeight;
      CameraFrame4.Width    = frameWidth;
      CameraFrame4.Left     = col1;
      CameraFrame4.Top      = frameTopRow1;
      CameraFrame4label.Top = classLabelTopRow1;
      CameraFrame4label.Width = frameWidth;
      CameraFrame4label.Left  = col1;

      CameraFrame5.Height = frameHeight;
      CameraFrame5.Width  = frameWidth;
      CameraFrame5.Left   = col2;
      CameraFrame5.Top    = frameTopRow1;
      CameraFrame5label.Top = classLabelTopRow1;
      CameraFrame5label.Width = frameWidth;
      CameraFrame5label.Left  = col2;
    }  /* ComputeFramePositions */



    private void CameraPreView_SizeChanged (object sender, EventArgs e)
    {
      if  (oldHeight == 0)
        return;

      int  deltaHeight = Height - oldHeight;
      int  deltaWidth  = Width  - oldWidth;

      ComputeFramePositions ();

      int halfDeltaWidth  = (int)(0.5 + deltaWidth  / 2.0);

      Count.Left = (Width - Count.Width) / 2;


      int  rowsResetButonWasFromBottom = (oldHeight - ResetButton.Top);
      ResetButton.Top = Height - rowsResetButonWasFromBottom;
      ResetButton.Left = (Width - ResetButton.Width) / 2;
      
      oldHeight  = Height;
      oldWidth   = Width;
    }



    private void CameraPreView_Load (object sender, EventArgs e)
    {
      oldHeight  = Height;
      oldWidth   = Width;
    }

    private void CameraFrame1label_TextChanged (object sender, EventArgs e)
    {

    }

    private void CameraFrame0label_TextChanged (object sender, EventArgs e)
    {

    }
  }
}
