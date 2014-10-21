using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;

using System.Windows.Forms.DataVisualization.Charting;
using System.Drawing;


using LarcosCounterManaged;



namespace LarcosCounter
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
    public  String    ChartTitle    {get  {return chartTitle;}}

    private  byte[]   chart       = null;
    private  String   chartTitle  = null;
  }
}
