#include  "FirstIncludes.h"

#include <stdlib.h>
#include <memory>
#include <string>
#include <iostream>
#include <fstream>
#include <map>
#include <vector>


#include "MemoryDebug.h"


using namespace std;

#include "KKBaseTypes.h"
#include "ImageIO.h"
#include "KKStr.h"
#include "MorphOpStretcher.h"
#include "OSservices.h"
using namespace  KKB;




#include  "DiversifyFlowRateTrainingLibrary.h"



DiversifyFlowRateTrainingLibrary::DiversifyFlowRateTrainingLibrary (int     argc,
                                                                    char**  argv
                                                                   ):
  Application (),
  report             (NULL),
  reportFile         (NULL),
  reportFileName     (),
  trainLibRootDir    ()

{
  this->InitalizeApplication(argc, argv);

  if  (Abort ())
  {
    DisplayCommandLineParameters ();
    return;
  }

  if  (trainLibRootDir.Empty ())
  {
    trainLibRootDir = osGetCurrentDirectory ();
  }

  if  (reportFileName.Empty ())
  {
    reportFile = NULL;
    report = &cout;
  }
  else
  {
    reportFile = new ofstream (reportFileName.Str ());
    report = reportFile;
  }

  *report << "Creating Diversified Trauining Images by Flow Rate Factor." << endl;

  *report << "Report File Name           [" << reportFileName      << "]."  << endl;
  *report << "Source Root Directory      [" << trainLibRootDir   << "]."  << endl;
}  /* DiversifyFlowRateTrainingLibrary */



DiversifyFlowRateTrainingLibrary::~DiversifyFlowRateTrainingLibrary ()
{
  if  (reportFile)
  {
    reportFile->close ();
    delete  reportFile;
  }
}




bool  DiversifyFlowRateTrainingLibrary::ProcessCmdLineParameter (char   parmSwitchCode, 
                                                                 KKStr  parmSwitch, 
                                                                 KKStr  parmValue
                                                                )
{
  if  (parmSwitch.EqualIgnoreCase ("-Report")      ||
       parmSwitch.EqualIgnoreCase ("-ReportFile")  ||
       parmSwitch.EqualIgnoreCase ("-R")
      )
    reportFileName = parmValue;
 
  else 
  if  (parmSwitch.EqualIgnoreCase ("-RootDir")      ||
       parmSwitch.EqualIgnoreCase ("-RD")  ||
       parmSwitch.EqualIgnoreCase ("-TrainLibDir")
      )
    trainLibRootDir = parmValue;

  return  !Abort ();
}  /* ProcessCmdLineParameter */


// -RootDir E:\TFTC\Classifier\TrainingLibraries\VaShrimp2.5g_Struct5_FR-Diversified


void   DiversifyFlowRateTrainingLibrary::DisplayCommandLineParameters ()
{
  log.Level (0) << "DiversifyFlowRateTrainingLibrary  -RootDir <xxx> -Report <xxxx>" << endl;
  log.Level (0) << endl;
  log.Level (0) << "    -RootDir  Root directory of training library." << endl;
  log.Level (0)                                                         << endl;
  log.Level (0) << "    -Report  Report File."                          << endl;
  log.Level (0)                                                         << endl;
}  /* DisplayCommandLineParameters */



void  DiversifyFlowRateTrainingLibrary::ProcessTrainingLibrary ()
{
  ProcessDirectory (trainLibRootDir);
}  /* ProcessTrainingLibrary */



void  DiversifyFlowRateTrainingLibrary::ProcessDirectory (const KKStr&  subDirName)
{
  log.Level (10) << "DiversifyFlowRateTrainingLibrary::ProcessDirectory  " << subDirName << endl;
  KKStr  fileSpec = osAddSlash (subDirName) + "*.bmp";
  KKStrListPtr  files = osGetListOfFiles (fileSpec);
  if  ((files != NULL)  &&  (files->QueueSize () > 0))
  {
    KKStrList::iterator idx;
    for  (idx = files->begin ();  idx != files->end ();  ++idx)
    {
      KKStrPtr  fileName = *idx;
      cout << *fileName << endl;
      KKStr  fullFileName = osAddSlash (subDirName) + *fileName;
      DiversifyImage (fullFileName);
    }
  }

  KKStrListPtr  subDirNames = osGetListOfDirectories (subDirName);
  if  ((subDirNames != NULL)  &&  (subDirNames->QueueSize () > 0))
  {
    KKStrList::iterator idx;
    for  (idx = subDirNames->begin ();  idx != subDirNames->end ();  ++idx)
    {
      KKStrPtr  dirName = *idx;
      KKStr  fullSubDirName = osAddSlash (subDirName) + *dirName;
      ProcessDirectory (fullSubDirName);
    }
  }

}  /* ProcessDirectory */




void  DiversifyFlowRateTrainingLibrary::DiversifyImage (const KKStr& fullFileName)
{
  RasterPtr i = ReadImage (fullFileName);
  if  (!i)
  {
    log.Level (-1) << "DiversifyImage   File[" << fullFileName << "]" << endl;
    return;
  }

  KKStr baseName = osRemoveExtension (fullFileName);

  StretchImage (i, baseName, 0.80f);
  StretchImage (i, baseName, 0.90f);
  StretchImage (i, baseName, 1.10f);
  StretchImage (i, baseName, 1.20f);
}  /* DiversifyImage */



void  DiversifyFlowRateTrainingLibrary::StretchImage (RasterPtr     i,
                                                      const KKStr&  baseName,
                                                      float         flowRateFactor
                                                     )
{
  MorphOpStretcher  stretcher (flowRateFactor, 1.0f);

  RasterPtr  stretchedImage = stretcher.PerformOperation (i);


  kkint32  x = (kkint32)(0.5f + flowRateFactor * 100);
  KKStr  newFileName = baseName + "_FR" + StrFormatInt (x, "000") + ".bmp";

  SaveImageGrayscaleInverted8Bit (*stretchedImage, newFileName);


  delete  stretchedImage;
  stretchedImage = NULL;
}  /* StretchImage */



void  main (int  argc,  char** argv)
{
  DiversifyFlowRateTrainingLibrary app (argc, argv);

  app.ProcessTrainingLibrary ();
}





