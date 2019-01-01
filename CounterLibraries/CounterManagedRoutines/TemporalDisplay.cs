using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Windows.Forms.DataVisualization.Charting;



namespace CounterManagedRoutines
{
  public partial class TemporalDisplay : Form
  {
    private  byte[][]  raster = null;
    private  int       col  = 0;
    private  int       width = 2048;

    private  int       cropLeft  = 0;
    private  int       cropRight = 2047;

    private  int       lastHeight = 0;
    private  int       lastWidth = 0;

    private  bool      complementData = false;


    public TemporalDisplay (byte[][]  _raster,
                            int       _col,
                            int       _cropLeft,
                            int       _cropRight,
                            bool      _complementData
                           )
    {
      col            = _col;
      cropLeft       = _cropLeft;
      cropRight      = _cropRight;
      complementData = _complementData;

      raster = _raster;
      if  ((raster != null)  &&  (raster.Length > 0))
        width = raster[0].Length;
      else
        width = 0;

      InitializeComponent ();
      
      Column.Value = (decimal)col;
    }

    
    private  void    ComputeStats (ref double  mean,
                                   ref double  stdDev,
                                   int         c
                                  )
    {
      double  sum       = 0;
      double  sumSquare = 0;
      int  n            = 0;

      int  height = Math.Min (300, raster.Length);

      for  (int r = 0;  r < height;  ++r)
      {
        int  zed = 0;
        if  (raster[r].Length > c)
        {
          zed = raster[r][c];
          if  (complementData)
            zed = 255 - zed;
          ++n;
          sum += (double)zed;
          sumSquare += (double)(zed * zed);
        }
        else
        {
          zed = 0;
        }
      }

      mean   = sum / (double)n;
      double  var    = (sumSquare - (sum * sum) / (double)n) / (double)(n - 1);
      stdDev = Math.Sqrt (var); 
    }  /* ComputeStats */



    private  void  ComputeAvgStats (ref double  avgMean,
                                    ref double  avgStdDev
                                   )
    {
      cropRight = Math.Min (cropRight, width);

      int  n = 0;
      double  totalMean = 0.0;
      double  totalStdDev = 0.0;

      for  (int c = cropLeft;  c <= cropRight;  ++c)
      {
        double  mean = 0.0;
        double  stdDev = 0.0;
        ComputeStats (ref mean, ref stdDev, c);
        totalMean    += mean;
        totalStdDev  += stdDev;
        ++n;
      }

      avgMean = totalMean     / (double)n;
      avgStdDev = totalStdDev / (double)n;
    }  /* ComputeAvgStats */



    private  void  PopulateChart ()
    {
      ColIntensityChart.Series[0].Points.Clear ();

      if  ((raster == null)  &&  (width < 1))
        return;

      int  height = Math.Min (300, raster.Length);

      int  minValue = 99999;
      int  maxValue = 0;

      double  sum       = 0;
      double  sumSquare = 0;
      int  n            = 0;

      for  (int r = 0;  r < height;  ++r)
      {
        int  zed = 0;
        if  (raster[r].Length > col)
        {
          zed = raster[r][col];
          if  (complementData)
            zed = 255 - zed;

          minValue = Math.Min (zed, minValue);
          maxValue = Math.Max (zed, maxValue);

          ++n;
          sum += (double)zed;
          sumSquare += (double)(zed * zed);
        }
        else
        {
          zed = 0;
        }
        DataPoint  dp = new DataPoint (r, zed);

        ColIntensityChart.Series[0].Points.Add (dp);
      }

      ColIntensityChart.ChartAreas[0].AxisY.Minimum = minValue;
      ColIntensityChart.ChartAreas[0].AxisY.Maximum = 255;

      double  mean   = sum / n;
      double  var    = (sumSquare - (sum * sum) / n) / (n - 1);
      double  stdDev = Math.Sqrt (var); 

      Mean.Text   = mean.ToString ("##0.000");
      StdDev.Text = stdDev.ToString ("##0.000");


      double  avgMean = 0.0;
      double  avgStdDev = 0.0;

      ComputeAvgStats (ref avgMean, ref avgStdDev);
      AvgStdDev.Text = avgStdDev.ToString ("##0.0000");

    }





    private void TemporalDisplay_Load (object sender, EventArgs e)
    {
      lastHeight = Height;
      lastWidth  = Width;

      PopulateChart ();
    }




    private void Column_ValueChanged (object sender, EventArgs e)
    {
      int  zed = (int)Column.Value;
      if  ((zed >= 0)  &&  (zed < width))
      {
        col = zed;
        PopulateChart ();
      }
    }



    private void TemporalDisplay_SizeChanged (object sender, EventArgs e)
    {
      if  (WindowState == FormWindowState.Minimized)
        return;

      if  (lastHeight > 0)
      {
        int  deltaHeight = Height - lastHeight;
        int  deltaWidth  = Width  - lastWidth;

        ColIntensityChart.Height += deltaHeight;
        ColIntensityChart.Width  += deltaWidth;

        Column.Top      += deltaHeight;
        ColLabel.Top    += deltaHeight;

        MeanLabel.Top   += deltaHeight;
        Mean.Top        += deltaHeight;

        StdDev.Top      += deltaHeight;
        StdDevLabel.Top += deltaHeight;

        AvgStdDev.Top      += deltaHeight;
        AvgStdDevLabel.Top += deltaHeight;
      }

      lastHeight = Height;
      lastWidth  = Width;
    }

    private void label1_Click (object sender, EventArgs e)
    {

    }




  }
}
