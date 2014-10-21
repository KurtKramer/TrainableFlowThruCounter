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


#include "ScannerFile.h"
using namespace  KKLSC;


#include  "ImportSipper4Images.h"


// -rd "E:\Pices\SipperFiles\WB0813-08_Sipper4\lir.20.SIPPER 4 August 2013 Trials.DSH09.UDR 1"
// -rd "F:\Pices\SipperFiles\USF\WB0813_S4\lir.21.SIPPER 4 August 2013 Trials.DSH10.UDR 1"
// -rd "F:\Pices\SipperFiles\USF\WB0813S4\lir.21.SIPPER 4 August 2013 Trials.DSH10.UDR 1"

// -rd "E:\Pices\lir.2.August 2014.TESTGoM.UDR 1"


ImportSipper4Images::ImportSipper4Images (int     argc,
                                          char**  argv
                                         ):
  Application (argc, argv),
  flatField      (NULL),
  outScannerFile (NULL),
  report         (NULL),
  reportFile     (NULL),
  reportFileName (),
  rootDir        (),
  scanRowsAdded  (0)

{
  ProcessCmdLineParameters (argc, argv);

  //flatField = new FlatFieldCorrection (20, 4096, ScannerFile4BitEncoded::CompensationTable ());
  //flatField = new FlatFieldCorrection (20, 4096, NULL);

  if  (Abort ())
  {
    DisplayCommandLineParameters ();
    return;
  }

  //if  (rootDir.Empty ())
  //{
  //  rootDir = osGetCurrentDirectory ();
  //}

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

  *report << "Creating Scanner File from SIPPER4 images." << endl;

  *report << "Report File Name"       << "\t" << reportFileName        << endl;
  *report << "Source Root Directory"  << "\t" << rootDir               << endl;
  *report << "CurrentDataTime"        << "\t" << osGetLocalDateTime () << endl;
}  /* ImportSipper4Images */



ImportSipper4Images::~ImportSipper4Images ()
{
  delete  flatField;
  flatField = NULL;

  if  (reportFile)
  {
    reportFile->close ();
    delete  reportFile;
  }
}




bool  ImportSipper4Images::ProcessCmdLineParameter (char   parmSwitchCode, 
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
    rootDir = parmValue;

  return  !Abort ();
}  /* ProcessCmdLineParameter */


// -RootDir E:\Larcos\Classifier\TrainingLibraries\VaShrimp2.5g_Struct5_FR-Diversified


void   ImportSipper4Images::DisplayCommandLineParameters ()
{
  log.Level (0) << "ImportSipper4Images  -RootDir <xxx> -Report <xxxx>" << endl;
  log.Level (0) << endl;
  log.Level (0) << "    -RootDir  Root directory of traininig library." << endl;
  log.Level (0)                                                         << endl;
  log.Level (0) << "    -Report  Report File."                          << endl;
  log.Level (0)                                                         << endl;
}  /* DisplayCommandLineParameters */


class  ComparerLastColNum
{
public:
  bool  operator () (KKStr* s1,
                     KKStr* s2
                    )
  {
    kkint32  n1 = s1->ToInt32 ();
    kkint32  n2 = s2->ToInt32 ();

    kkint32  lastDash1 = s1->LocateLastOccurrence ('-');
    if  (lastDash1 >= 0)
      n1 = s1->SubStrPart (lastDash1 + 1).ToInt32 ();

    kkint32  lastDash2 = s2->LocateLastOccurrence ('-');
    if  (lastDash2 >= 0)
      n2 = s2->SubStrPart (lastDash2 + 1).ToInt32 ();

    return n1 < n2;
  }
};  /* ComparerLastColNum */



void  ImportSipper4Images::SortByNumberAtEnd (KKStrListPtr  list)
{
  ComparerLastColNum  comparer;
  sort (list->begin (), list->end (), comparer);
}

//


void  ImportSipper4Images::ProcessDirectory (const KKStr&  subDirName)
{
  log.Level (10) << "ImportSipper4Images::ProcessDirectory  " << subDirName << endl;

  *report << endl << "Starting Directory: " << subDirName << endl;

  if  (outScannerFile)
  {
    outScannerFile->Close ();
    delete  outScannerFile;
    outScannerFile = NULL;
  }

  KKStr  rootDirName = KKB::osGetRootNameOfDirectory (subDirName);
  rootDirName.LeftPad (4, '0');

  KKStr  scannerFileName = osAddSlash (rootDir) + "WB0814S4-DSH10_" + rootDirName + ".lsc";
  if  (KKB::osFileExists (scannerFileName))
    KKB::osDeleteFile (scannerFileName);

  outScannerFile = ScannerFile::CreateScannerFileForOutput 
    (scannerFileName, ScannerFile::sf4BitEncoded, 4096, 420, log);

  outScannerFile->InitiateWritting ();


  KKStr textBlock (128);
  textBlock << "Directory" << "\t" << subDirName << endl;
  outScannerFile->WriteTextBlock ((uchar*)textBlock.Str (), textBlock.Len ());

  KKStr  fileSpec = osAddSlash (subDirName) + "*.tif";
  KKStrListPtr  files = osGetListOfFiles (fileSpec);

  if  ((files != NULL)  &&  (files->QueueSize () > 0))
  {
    SortByNumberAtEnd (files);
    KKStrList::iterator idx;
    for  (idx = files->begin ();  idx != files->end ();  ++idx)
    {
      KKStr  fileName = *idx;
      cout << fileName << endl;
      *report << fileName << endl;
      KKStr  fullFileName = osAddSlash (subDirName) + fileName;
      AddImageToScannerFile2 (fullFileName);
    }
  }

  KKStrListPtr  subDirNames = osGetListOfDirectories (subDirName);
  if  ((subDirNames != NULL)  &&  (subDirNames->QueueSize () > 0))
  {
    SortByNumberAtEnd (subDirNames);

    KKStrList::iterator idx;
    for  (idx = subDirNames->begin ();  idx != subDirNames->end ();  ++idx)
    {
      KKStr  dirName = *idx;
      KKStr  fullSubDirName = osAddSlash (subDirName) + dirName;
      ProcessDirectory (fullSubDirName);
    }
  }

}  /* ProcessDirectory */




void  ImportSipper4Images::AddImageToScannerFile (const KKStr&  fileName)
{
  RasterPtr i = ReadImage (fileName);
  if  (!i)
  {
    log.Level (-1) << endl 
      << "ImportSipper4Images::AddImageToScannerFile  ***ERROR***   Could not open image[" << fileName << "]." << endl
      << endl;
    return;
  }

  {
    // Remove edge noise from left side of image.
    for  (kkint32  row = 0;  row < i->Height ();  ++row)
    {
      uchar*  rowData = (i->Green ())[row];
      for  (kkint32  col = 0;  col < 500;  ++col)
        rowData[col] = 255;
    }
  }

  //KKB::SaveImage (*i, "C:\\Temp\\Sipper4Images\\" + KKB::osGetRootName (fileName) + "_01-AsRead.bmp");


  //i->ReverseImage ();
  //i->BackgroundPixelValue (0);
  //i->BackgroundPixelTH (31);
  //i->ForegroundPixelValue (255);

  //KKB::SaveImage (*i, "C:\\Temp\\Sipper4Images\\" + KKB::osGetRootName (fileName) + "_02-Reversed.bmp");


  if  (!flatField)
  {
    flatField = new FlatFieldCorrection (20, i->Width (), ScannerFile4BitEncoded::CompensationTable ());
  }
  
  else if  (flatField->LineWidth () != i->Width ())
  {
    delete  flatField;
    flatField = new FlatFieldCorrection (20, i->Width (), ScannerFile4BitEncoded::CompensationTable ());
  }

  kkint32  row = 0;

  {
    // Apply Flat Field.
    for  (row = 0;  row < i->Height ();  ++row)
    {
      uchar*  rowData = (i->Green ())[row];
      if  ((scanRowsAdded % 200) ==  0)
        flatField->AddSampleLine (rowData);
      flatField->ApplyFlatFieldCorrection (rowData);
      ++scanRowsAdded;
    }
  }
  // Flat Field will reverse the image.
  //i->BackgroundPixelValue (255);
  //i->BackgroundPixelTH (255 - 31);
  //i->ForegroundPixelValue (0);

  i->BackgroundPixelTH (17);

  //KKB::SaveImage (*i, "C:\\Temp\\Sipper4Images\\" + KKB::osGetRootName (fileName) +"_03-FlatFielded.bmp");

  //RasterPtr j = i->BandPass (0.0f, 0.75f, false);
  RasterPtr j = i->CreateSmoothedMediumImage (5);
  {
    uchar  bgth = j->BackgroundPixelTH ();
    uchar*  rawData = j->GreenArea ();
    for  (kkint32 x = 0;  x < j->TotPixels ();  ++x)
    {
      if  (rawData[x] > bgth)
        rawData[x] = 255;
      else
        rawData[x] = 0;
    }
  }
  //RasterPtr j = new Raster (*i);

  //j->BackgroundPixelTH (255 - 110);
  //KKB::SaveImage (*j, "C:\\Temp\\Sipper4Images\\" + KKB::osGetRootName (fileName) + "_04-Smoothed.bmp");

  
  j->Dialation (SQUARE7);
  //KKB::SaveImage (*j, "C:\\Temp\\Sipper4Images\\" + KKB::osGetRootName (fileName) + "_05-Dialated01.bmp");

  //j->FillHole ();

  RasterPtr  k = i->ExtractUsingMask (j);
  //KKB::SaveImage (*k, "C:\\Temp\\Sipper4Images\\" + KKB::osGetRootName (fileName) + "_07-MaskApplied.bmp");
  //KKB::SaveImage (*i, "C:\\Temp\\Sipper4Images\\" + KKB::osGetRootName (fileName) + "_08-OrigSrcForMask.bmp");

  for  (row = 0;  row < k->Height ();  ++row)
  {
    uchar*  rowData = (k->Green ())[row];
    outScannerFile->WriteScanLine (rowData, k->Width ());
  }

  delete k;  k = NULL;
  delete j;  j = NULL;
  delete i;  i = NULL;

  outScannerFile->Flush ();
}  /* AddImageToScannerFile */



void  ImportSipper4Images::AddImageToScannerFile2 (const KKStr&  fileName)
{
  RasterPtr i = ReadImage (fileName);
  if  (!i)
  {
    log.Level (-1) << endl 
      << "ImportSipper4Images::AddImageToScannerFile  ***ERROR***   Could not open image[" << fileName << "]." << endl
      << endl;
    return;
  }

  {
    // Remove edge noise from right size of image.
    for  (kkint32  row = 0;  row < i->Height ();  ++row)
    {
      uchar*  rowData = (i->Green ())[row];
      for  (kkint32  col = 3388;  col < i->Width ();  ++col)
        rowData[col] = 255;
    }
  }

  if  (!flatField)
  {
    flatField = new FlatFieldCorrection (20, i->Width (), ScannerFile4BitEncoded::CompensationTable ());
  }
  
  else if  (flatField->LineWidth () != i->Width ())
  {
    delete  flatField;
    flatField = new FlatFieldCorrection (20, i->Width (), ScannerFile4BitEncoded::CompensationTable ());
  }

  kkint32  row = 0;

  {
    // Apply Flat Field.
    for  (row = 0;  row < i->Height ();  ++row)
    {
      uchar*  rowData = (i->Green ())[row];
      if  ((scanRowsAdded % 200) ==  0)
        flatField->AddSampleLine (rowData);
      flatField->ApplyFlatFieldCorrection (rowData);
      ++scanRowsAdded;
    }
  }
  // Flat Field will reverse the image.
  //i->BackgroundPixelValue (255);
  //i->BackgroundPixelTH (255 - 31);
  //i->ForegroundPixelValue (0);

  i->BackgroundPixelTH (17);

  for  (row = 0;  row < i->Height ();  ++row)
  {
    uchar*  rowData = (i->Green ())[row];
    outScannerFile->WriteScanLine (rowData, i->Width ());
  }

  delete i;  i = NULL;

  outScannerFile->Flush ();
}  /* AddImageToScannerFile2 */




void  ImportSipper4Images::Main ()
{
  if  (rootDir.Empty ())
  {
    log.Level (-1) << endl
      << "ImportSipper4Images::Main   ***ERROR***   Root Directory is not defined." << endl
      << endl;
    return;
  }

  if  (!osValidDirectory (rootDir))
  {
    log.Level (-1) << endl
      << "ImportSipper4Images::Main   ***ERROR***   Root Directory: " << rootDir << "Does not exist." << endl
      << endl;
    return;
  }

  ProcessDirectory (rootDir);

  outScannerFile->Close ();
}  /* Main */



void  main (int  argc,  char** argv)
{
  ImportSipper4Images app (argc, argv);

  app.Main ();
}





