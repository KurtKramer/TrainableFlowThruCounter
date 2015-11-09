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

#include "ScannerFile4BitEncoded.h"
#include "ImportSipper4Images.h"


// -rd "E:\Pices\SipperFiles\WB0813-08_Sipper4\lir.20.SIPPER 4 August 2013 Trials.DSH09.UDR 1"
// -rd "F:\Pices\SipperFiles\USF\WB0813_S4\lir.21.SIPPER 4 August 2013 Trials.DSH10.UDR 1"
// -rd "F:\Pices\SipperFiles\USF\WB0813S4\lir.21.SIPPER 4 August 2013 Trials.DSH10.UDR 1"

// -rd "E:\Pices\lir.2.August 2014.TESTGoM.UDR 1"


ImportSipper4Images::ImportSipper4Images (int     argc,
                                          char**  argv
                                         ):
  Application (),
  rng ((long)(KKB::osGetLocalDateTime().Seconds () % (1024 * 1024))),

  flatField      (NULL),
  outScannerFile (NULL),
  report         (NULL),
  reportFile     (NULL),
  reportFileName (),
  rootDir        (),
  scanRowsAdded  (0),
  imagesPerFrame (50)

{
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
    (scannerFileName, ScannerFile::Format::sf4BitEncoded, 4096, 420, log);

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
      KKStr  fileName = **idx;
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
      KKStr  dirName = **idx;
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

  
  j->Dilation (MaskTypes::SQUARE7);
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


RasterListPtr ImportSipper4Images::LoadSampleImages (const KKStr&  rootDir)
{
  RasterListPtr sampleImages = new RasterList (true);
  KKStrListPtr  names =  KKB::osGetListOfFiles(osAddSlash (rootDir) + "*.bmp");
  for (auto idx: *names) {
    RasterPtr image = KKB::ReadImage (osAddSlash(rootDir) + *idx);
    if  (image != NULL)
      sampleImages->PushOnBack (image);
  }
  return  sampleImages;
}



RasterPtr  ImportSipper4Images::RandomizeImage (const Raster&  i)
{
  float heightAdjRatio = 1.3f  - (float)(0.4f * ((float)(rng.Next () % 100) / 99.0f));
  float widthAdjRatio  = 1.3f  - (float)(0.4f * ((float)(rng.Next () % 100) / 99.0f));

  RasterPtr resizedImage = i.StreatchImage (heightAdjRatio, widthAdjRatio);
  RasterPtr rotatedImage = NULL; 

  float  randRotationAngle = (float)(rng.Next () % 90);
  if  (randRotationAngle == 0.0f)
  {
    rotatedImage = resizedImage;
    resizedImage = NULL;
  }
  else
  {
    rotatedImage = resizedImage->Rotate (randRotationAngle);
    delete  resizedImage;
    resizedImage = NULL;
  }

  RasterPtr  resultImage = rotatedImage->TightlyBounded (1);

  delete  rotatedImage;  rotatedImage = NULL;
  delete  resizedImage;  resizedImage = NULL;

  return  resultImage;
}  /* RandomizeImage */



bool  ImportSipper4Images::DoesImageFit (Raster&  dest,  const Raster&  src, int row, int col)
{
  kkint32  collisions = 0;

  kkint32  srcRow = 0, srcCol = 0;
  kkint32  destRow = row, destCol = col;

  for  (srcRow = 0;  (srcRow < src.Height ())  &&  (destRow < dest.Height ());  ++srcRow, ++destRow)
  {
    for  (srcCol = 0;  (srcCol < src.Width ())  &&  (destCol < dest.Width ());  ++srcCol, ++destCol)
    {
      if  (src.ForegroundPixel (srcRow, srcCol)  &&  dest.ForegroundPixel (destRow, destCol))
        ++collisions;
    }
  }
  return (collisions < 1);
}




void  ImportSipper4Images::PaintImage (Raster&  dest,  const Raster&  src, int row, int col)
{
  kkint32  srcRow = 0, srcCol = 0;
  kkint32  destRow = row, destCol = col;
  for  (srcRow = 0;  (srcRow < src.Height ())  &&  (destRow < dest.Height ());  ++srcRow, ++destRow)
  {
    destCol = col;
    for  (srcCol = 0;  (srcCol < src.Width ())  &&  (destCol < dest.Width ());  ++srcCol, ++destCol)
    {
      if  (src.ForegroundPixel (srcRow, srcCol))
        dest.SetPixelValue (destRow, destCol, src.GetPixelValue (srcRow, srcCol));
    }
  }
}



kkint32  rcount = 0;


bool  ImportSipper4Images::FitImageRandomly (Raster&  dest,  const Raster&  src)
{
  kkint32  size = 0, weight = 0;
  float rowCenter = 0.0f, colCenter = 0.0f;
  float rowCenterWeidght = 0.0f, colCenterWeight = 0.0f;

  src.CalcCentroid (size, weight, rowCenter, colCenter, rowCenterWeidght, colCenterWeight);

  bool  imageFits = false;
  RasterPtr  candidateImage = NULL;

  kkint32  randomTries = 0;

  while  ((!imageFits)  &&  (randomTries < 10))
  {
    RasterPtr  rImage = RandomizeImage (src);

    kkint32  rangeColLeft  = 110;
    kkint32  rangeColRight = dest.Width  () - (rImage->Width () + 100);
    kkint32  rangeRowTop   = 0;
    kkint32  rangeRowBot   = dest.Height () - (rImage->Height () + 1);
    kkint32  rangeWidth    = 1 + rangeColRight - rangeColLeft;
    kkint32  rangeHeight   = 1 + rangeRowBot   - rangeRowTop;


    kkint32  attempts = 0;
    while  ((!imageFits)  &&  (attempts < 10))
    {
      kkint32  randomRow = rng.Next () % rangeHeight;
      kkint32  randomCol = rangeColLeft + (rng.Next () % rangeWidth);

      if  (DoesImageFit (dest, *rImage, randomRow,  randomCol))
      {

        KKStr  randomImageName = "E:\\ThePlanktonCounter\\ScannerFiles\\DemoScannerFiles\\EricsBAckYard_" + StrFormatInt(rcount, "000000") + ".bmp";
        //KKB::SaveImageGrayscaleInverted8Bit (*rImage, randomImageName);

        PaintImage (dest, *rImage, randomRow,  randomCol);
        imageFits = true;
      }

      ++attempts;
    }
    delete  rImage;
    rImage = NULL;
    ++randomTries;
  }

  return  imageFits;
}



bool  ImportSipper4Images::FitRandomImageRandomly (RasterListPtr  candidateImages, Raster&  dest)
{
  kkint32 randomIdx = (rng.Next () % candidateImages->QueueSize ());
  return  FitImageRandomly (dest, *(candidateImages->IdxToPtr (randomIdx)));
}




void  ImportSipper4Images::PopulateWithRandomImagesRandomly (RasterListPtr  candidateImages, Raster&  dest, int numPerFrame)
{
  kkint32 numImagesToFit =  (numPerFrame / 2) +  (rng.Next () % numPerFrame);
  kkint32 numImagesFitted = 0; 

  for  (kkint32 idx = 0;  idx < numImagesToFit;  ++idx)
  {
    if  (FitRandomImageRandomly(candidateImages, dest))
      ++numImagesFitted;
  }
}



void  ImportSipper4Images::Main ()
{
  RasterListPtr  candidateImages = LoadSampleImages ("E:\\ThePlanktonCounter\\InterestingImages");
  RasterListPtr  tinyImages = LoadSampleImages ("E:\\ThePlanktonCounter\\TinyImages");

  KKStr  scannerFileName = "E:\\ThePlanktonCounter\\ScannerFiles\\DemoScannerFiles\\EricsBAckYard.lsc";

  KKB::osDeleteFile (scannerFileName);
  ScannerFilePtr  sf = ScannerFile::CreateScannerFileForOutput (scannerFileName, ScannerFile::Format::sf4BitEncoded, 2048, 2048, log); 
  sf->AddHeaderField ("Description", "Created this file from contents of several scanner files collected from Canal");

  sf->InitiateWritting ();


  kkint32  frameHeight = 2048 * 12;

  kkint64  numScanLines = 20000 * 3600 * 2;
  kkint32  numFrames = (kkint32)(numScanLines / frameHeight);

  kkint32  numFramesCreated = 0;
  while  (numFramesCreated < numFrames)
  {
    cout << "numFramesCreated: " << "\t" << numFramesCreated << endl;
    RasterPtr  frame = new Raster (frameHeight, 2048);
    PopulateWithRandomImagesRandomly (candidateImages, *frame, 25);

    PopulateWithRandomImagesRandomly (tinyImages, *frame, 800);


    uchar** frameData = frame->Green ();

    for (kkint32 idx = 0;  idx < frame->Height ();  ++idx)
    {
      sf->WriteScanLine (frameData[idx], frame->Width ());
    }

    delete  frame;
    frame = NULL;
    ++numFramesCreated;
  }

  sf->Close ();
  delete sf;
  sf = NULL;

  delete  candidateImages;
  candidateImages = NULL;
}  /* Main */



void  main (int  argc,  char** argv)
{
  ImportSipper4Images app (argc, argv);
  app.InitalizeApplication (argc, argv);
  app.Main ();
}





