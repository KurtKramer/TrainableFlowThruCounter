﻿using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;

using System.Windows.Forms.DataVisualization.Charting;
using System.Drawing;


using CounterManaged;



namespace CounterApplication
{
  public  class FinaleReportChartDataSource
  {
    public  FinaleReportChartDataSource  (String  _chartTitle,
                                          byte[]  _chart
                                         )
    {
      chart      = _chart;
      chartTitle = _chartTitle;
    }

    public  byte[]    Chart         {get  {return chart;}}
    public  string    ChartTitle    {get  {return chartTitle;}}

    private  byte[]   chart       = null;
    private  string   chartTitle  = null;
  }
}
