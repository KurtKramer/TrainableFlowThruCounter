using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace LarcosCounter
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
      if  (ticks > 20)
        this.Close ();
    }
  }
}
