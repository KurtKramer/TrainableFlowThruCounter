#ifndef  _EXTRACTFEATURES_
#define  _EXTRACTFEATURES_


#include "Application.h"
#include "Raster.h"
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
  void  ProcessDirectory (const KKStr&  subDirName);
  void  AddImageToScannerFile (const KKStr&  fileName);
  void  AddImageToScannerFile2 (const KKStr&  fileName);
  void  SortByNumberAtEnd (KKStrListPtr  list);

  
  FlatFieldCorrectionPtr  flatField;
  ostream*                report;
  ofstream*               reportFile;
  KKStr                   reportFileName;
  ScannerFilePtr          outScannerFile;
  KKStr                   rootDir;
  kkint32                 scanRowsAdded;

};  /* ImportSipper4Images */





#endif