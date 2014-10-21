#ifndef  _EXTRACTFEATURES_
#define  _EXTRACTFEATURES_


#include "Application.h"
#include "Raster.h"


class  DiversifyFlowRateTrainingLibrary: public  Application
{
public:
  DiversifyFlowRateTrainingLibrary (int     argc,
                                    char**  argv
                                   );
  
  ~DiversifyFlowRateTrainingLibrary ();

  void  ProcessTrainingLibrary ();


  virtual const 
  char*          ApplicationName ()  {return  "DiversifyFlowRateTrainingLibrary";}

  void           DisplayCommandLineParameters ();


  virtual 
  bool           ProcessCmdLineParameter (char    parmSwitchCode, 
                                          KKStr  parmSwitch, 
                                          KKStr  parmValue
                                         );


private:
  void  DiversifyImage (const KKStr& fullFileName);

  void  ProcessDirectory (const KKStr&  subDirName);

  void  StretchImage (RasterPtr     i,
                      const KKStr&  baseName,
                      float         flowRateFactor
                     );


  ostream*  report;
  ofstream* reportFile;
  KKStr     reportFileName;
  KKStr     trainLibRootDir;
};





#endif