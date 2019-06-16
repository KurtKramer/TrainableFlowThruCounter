using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;

using System.Windows.Forms.DataVisualization.Charting;
using System.Drawing;


using CounterManaged;



namespace CounterApplication
{
  public  class FinaleReportDataSource
  {
    public  FinaleReportDataSource  (CounterManagerWrapper  _cameraManager,
                                     UmiSessionParameters   _sessionParameters,
                                     UmiOperatingParameters _operatingParameters,
                                     int                    _count,

                                     int                    _sampleSize,
                                     float                  _sizeMean,
                                     float                  _sizeStdDev,

                                     byte[]                 _sizeChart,
                                     byte[]                 _temporalChart
                                    )
    {
      cameraManager       = _cameraManager;
      sessionParameters   = _sessionParameters;
      operatingParameters = _operatingParameters;
      count               = _count;
      sampleSize          = _sampleSize;
      sizeMean            = _sizeMean;
      sizeStdDev          = _sizeStdDev;
      sizeChart           = _sizeChart;
      temporalChart       = _temporalChart;
    }

    public  String    ControlNum          {get  {return sessionParameters.ControlNum;}}
    public  int       Count               {get  {return count;}}
    
    public  int       SampleSize          {get  {return  sampleSize;}  set  {sampleSize = value;}}
    public  float     SizeMean            {get  {return  sizeMean;}    set  {sizeMean   = value;}}
    public  float     SizeStdDev          {get  {return  sizeStdDev;}  set  {sizeStdDev = value;}}

    
    public  DateTime  DateTimeRecorded    {get  {return sessionParameters.DateTimeRecorded;}}
    public  String    Destination         {get  {return sessionParameters.Destination;}}
    public  String    PersonInCharge      {get  {return sessionParameters.PersonInCharge;}}
    public  String    CounterVersionNum   {get  {return cameraManager.CounterVersionNumber ();}}
    public  String    SessionDescription  {get  {return sessionParameters.SessionDescription;}}
    public  float     ScanRate            {get  {return operatingParameters.RequestedScanRate;}}
    public  int       SessionId           {get  {return sessionParameters.SessionId;}}
    public  byte[]    SizeChart           {get  {return sizeChart;}}
    public  String    Source              {get  {return sessionParameters.Source;}}
    public  byte[]    TemporalChart       {get  {return temporalChart;}}
    public  String    TrainingModelName   {get  {return sessionParameters.TrainingModelName;}}

    public  byte[]    Logo
    {
      get
      {
        Bitmap counterLogo = global::CounterApplication.Properties.Resources.ZOIS_Logo_Small;
        MemoryStream counterLogoStream = new MemoryStream ();
        counterLogo.Save (counterLogoStream, System.Drawing.Imaging.ImageFormat.Bmp);
        return  counterLogoStream.ToArray ();
      }
    }

    public  List<byte[]>   Charts
    {
      get
      {
        List<byte[]>  charts = new List<byte[]> ();
        charts.Add (sizeChart);
        charts.Add (temporalChart);
        return  charts;
      }
    }

    private  CounterManagerWrapper   cameraManager       = null;
    private  int                     count               = 0;
    private  int                     sampleSize          = 0;
    private  float                   sizeMean            = 0.0f;
    private  float                   sizeStdDev          = 0.0f;

    private  UmiOperatingParameters  operatingParameters = null;
    private  UmiSessionParameters    sessionParameters   = null;
    private  byte[]                  sizeChart           = null;
    private  byte[]                  temporalChart       = null;
  }
}
