#ifndef  _EXTRACTFEATURES_
#define  _EXTRACTFEATURES_


#include "Application.h"
#include "Raster.h"
#include "RandomNumGenerator.h"
using namespace KKB;

#include "FlatFieldCorrection.h"
#include "ScannerFile.h"
#include "ScannerFile4BitEncoded.h"
using namespace KKLSC;



class  ImportSipper4Images: public  Application
{
public:
  ImportSipper4Images (int     argc,
                       char**  argv
                      );
  
  ~ImportSipper4Images ();

  void  Main ();


  virtual const 
  char*    ApplicationName ()  {return  "ImportSipper4Images";}

  void     DisplayCommandLineParameters ();


  virtual 
  bool     ProcessCmdLineParameter (char    parmSwitchCode, 
                                    KKStr  parmSwitch, 
                                    KKStr  parmValue
                                   );


private:
  RasterListPtr  LoadSampleImages (const KKStr&  rootDir);
  RasterPtr  RandomizeImage (const Raster&  i);
  bool  FitImageRandomly (Raster&  dest,  const Raster&  src);

  bool  FitRandomImageRandomly (RasterListPtr  candidateImages, Raster&  dest);
  void  PopulateWithRandomImagesRandomly (RasterListPtr  candidateImages, Raster&  dest, int numPerFrame);



  bool  DoesImageFit (Raster&  dest,  const Raster&  src, int row, int col);
  void  PaintImage (Raster&  dest,  const Raster&  src, int row, int col);

  void  ProcessDirectory (const KKStr&  subDirName);
  void  AddImageToScannerFile (const KKStr&  fileName);
  void  AddImageToScannerFile2 (const KKStr&  fileName);
  void  SortByNumberAtEnd (KKStrListPtr  list);

  
  RandomNumGenerator rng;
  int                imagesPerFrame;

  FlatFieldCorrectionPtr  flatField;
  ostream*                report;
  ofstream*               reportFile;
  KKStr                   reportFileName;
  ScannerFilePtr          outScannerFile;
  KKStr                   rootDir;
  kkint32                 scanRowsAdded;

};  /* ImportSipper4Images */





#endif