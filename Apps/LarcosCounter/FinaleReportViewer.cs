using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.IO;
using System.Text;
using System.Windows.Forms;
using System.Drawing.Printing;
using System.Windows.Forms.DataVisualization.Charting;

//using Microsoft.Reporting.WinForms;

using  CounterManaged;





namespace LarcosCounter
{
  public partial class FinaleReportViewer : Form
  {

    private  CounterManagerWrapper  cameraManager    = null;
    private  FinaleReportDataSource       reportDataSource = null;

    private  int  oldHeight = 0;
    private  int  oldWidth  = 0;


    public  FinaleReportViewer (CounterManagerWrapper  _cameraManager,
                                FinaleReportDataSource       _reportDataSource
                               )
    {
      cameraManager    = _cameraManager;
      reportDataSource = _reportDataSource;
      InitializeComponent ();
    }


    private void FinaleReportViewer_Load (object sender, EventArgs e)
    {
      FinaleReportDataSourceBindingSource.DataSource = reportDataSource;

      List<FinaleReportChartDataSource>  chartSource = new List<FinaleReportChartDataSource> ();
      chartSource.Add (new FinaleReportChartDataSource ("Size Distribution", reportDataSource.SizeChart));
      chartSource.Add (new FinaleReportChartDataSource ("Temporal Distribution", reportDataSource.TemporalChart));
      chartsBindingSource.DataSource = chartSource;
      reportViewer1.RefreshReport ();
    }


    private void FinaleReportDataSourceBindingSource_CurrentChanged (object sender, EventArgs e)
    {

    }


    private void FinaleReportViewer_SizeChanged (object sender, EventArgs e)
    {
      if  (oldHeight != 0)
      {
        int  deltaHeight = Height - oldHeight;
        int  deltaWidth  = Width  - oldWidth;

        reportViewer1.Height += deltaHeight;
        reportViewer1.Width  += deltaWidth;
      }

      oldHeight = Height;
      oldWidth  = Width;
    }

    private void chartsBindingSource_CurrentChanged(object sender, EventArgs e)
    {

    }
  }
}
