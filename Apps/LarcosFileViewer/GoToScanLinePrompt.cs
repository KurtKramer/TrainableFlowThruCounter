using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

using LarcosCounterManaged;
using LarcosManagedRoutines;


namespace LarcosFileViewer
{
  public partial class GoToScanLinePrompt : Form
  {
    private  int  maxNumScanLines  = 0;
    private  int  selectedScanLine = 0;

    public   int  SelectedScanLine  {get {return selectedScanLine;}}


    public GoToScanLinePrompt (int  _maxNumScanLines)
    {
      maxNumScanLines = _maxNumScanLines;
      InitializeComponent ();
    }
    

    private void GotoButton_Click (object sender, EventArgs e)
    {
      selectedScanLine = UmiKKStr.StrToInt (ScanLine.Text);
      DialogResult = DialogResult.OK;
      Close ();
    }



    private void CancelButton_Click (object sender, EventArgs e)
    {
      DialogResult = DialogResult.Cancel;
      Close ();
    }
  }
}
