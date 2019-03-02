using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace CounterFileViewer
{
  public partial class SplashScreen : Form
  {
    int  ticks = 0;

    public SplashScreen ()
    {
      InitializeComponent ();
    }

    private void SplashScreenTimer_Tick (object sender, EventArgs e)
    {
      ++ticks;
      if  (ticks > 30)
        this.Close ();
    }

    private void pictureBox1_Click(object sender, EventArgs e)
    {

    }
  }
}
