#if  !defined(_HISTOGRAMSIZE_)
#define  _HISTOGRAMSIZE_


#include "Application.h"
#include "Raster.h"
#include "Point.h"


class  HistogramSize: public  Application
{
public:
  HistogramSize (int     argc,
                 char**  argv
                );
  
  ~HistogramSize ();

  void  ProcessDirectory ();


  virtual const 
  char*          ApplicationName ()  {return  "HistogramSize";}

  void           DisplayCommandLineParameters ();


  virtual 
  bool           ProcessCmdLineParameter (char    parmSwitchCode, 
                                          KKStr  parmSwitch, 
                                          KKStr  parmValue
                                         );


private:
  float  CalcHeightWidthRatio (RasterPtr  i);


  Point  DerivePreRotatedPoint (kkint32 height,
                                kkint32 width,
                                Point&  rotatedPoint, 
                                float   turnAngle
                               );

  void  ErodeBySize (Raster& i);

  void  Experiment (RasterPtr     i,
                    const KKStr&  rootName
                   );

  bool   OccludedImage (RasterPtr  i);

  
  void   ProcessImage (const KKStr& fullFileName);



  kkint32   pixelsPerScanLine;
  float     mmPerScanLine;
  float     mmPerPixel;
  kkint32   occlusiionLimit;
  float     scanRate;

  VectorInt  histogram;
  VectorInt  histogramBy5;

  KKStr     occludedDirName;
  KKStr     distordedDirName;
  KKStr     workDir;
  kkint32   occlusionPixelValueTH;
  ostream*  report;
  ofstream* reportFile;
  KKStr     reportFileName;
  KKStr     saveDirName;
  KKStr     targetDir;
};





#endif