using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

using CounterManagedRoutines;
using CounterManaged;

namespace CounterApplication
{
  public partial class VisibilityDialog : Form
  {
    private  int  lastHeight = 0;
    private  int  lastWidth  = 0;

    private  CounterManagerWrapper   cameraManager = null;

    public  float  GetRequestedAnalogGain  ()  {return  cameraLine.GetRequestedAnalogGain  ();}
    public  int    GetRequestedDigitalGain ()  {return  cameraLine.GetRequestedDigitalGain ();}


    public VisibilityDialog (CounterManagerWrapper  _cameraManager)
    {
      InitializeComponent ();
      cameraManager = _cameraManager;
      cameraLine.SetCameraManager (/* cameraManager */ null);
    }


    private void VisibilityDialog_SizeChanged (object sender, EventArgs e)
    {
      if  (lastHeight > 0)
      {
        int  deltaHeight = Height - lastHeight;
        int  deltaWidth  = Width  - lastWidth;

        cameraLine.Width  += deltaWidth;
        cameraLine.Height += deltaHeight;
      }

      lastHeight = Height;
      lastWidth  = Width;
    }



    private void VisibilityDialog_Load (object sender, EventArgs e)
    {
    }



    private void VisibilityDialog_FormClosing (object sender, FormClosingEventArgs e)
    {
      if  (cameraManager.CameraAutoGainThreadRunning ())
      {
        MessageBox.Show (this, "Auto Gain is still running.", "Visibility", MessageBoxButtons.OK);
        e.Cancel = true;
      }
      return;
    }
  }
}
