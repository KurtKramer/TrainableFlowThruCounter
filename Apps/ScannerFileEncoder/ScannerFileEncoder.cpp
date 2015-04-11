#include  "FirstIncludes.h"

#include  <stdlib.h>
#include  <memory.h>
#include  <math.h>

#include  <map>
#include  <string>
#include  <iostream>
#include  <fstream>
#include  <queue>
#include  <vector>
using namespace std;



// -src D:\Users\kkramer\AEOS\ShrimpImages  -SFN Shrimp.Spr
// -src C:\Users\kkramer\Desktop\AEOS\Shrimp images  -sfn Shrimp2.spr


#include  "MemoryDebug.h"
#include  "KKBaseTypes.h"


#include  "ImageIO.h"
#include  "OSservices.h"
#include  "Raster.h"
#include  "RandomNumGenerator.h"
#include  "KKStr.h"
using namespace KKB;


#include "ScannerFile.h"
#include "ScannerFile3BitEncoded.h"
using namespace KKLSC;



//using namespace KKMLL;


#include "ScannerFileEncoder.h"

using  namespace  ScannerFileEncoderApp;



ScannerFileEncoder::ScannerFileEncoder (int     argc, 
                                        char**  argv
                                       ):

   Application (argc, argv),
   destFileName              (),
   imagesPerSec              (10),
   randGen                   ((long)osGetLocalDateTime ().Seconds ()),
   scanRate                  (20000.0),
   scanLineBuffer            (true),
   scanLineBufferStart       (0),
   scanLineBufferEnd         (0),
   scanLineWidth             (2048),
   scannerFile               (NULL),
   scannerFileLenInSecs      (600),
   scannerFileLenInScanLines (0),
   srcDir                    (),
   srcImages                 (NULL),
   totalImages               (0)

{
  if  (argc < 2)
  {
    log.Level (-1) << endl << endl 
                   << "No Command Line Parameters were provided." << endl
                   << endl;

    DisplayCommandLineParameters ();
    Abort (true);
    return;
  }

  ProcessCmdLineParameters (argc, argv);
  if  (Abort ())
  {
    DisplayCommandLineParameters ();
    return;
  }

  if  (srcDir.Empty ())
  {
    srcDir = osGetCurrentDirectory ();
    log.Level (10) << endl << endl 
                   << "ScannerFileEncoder   No source directory specified defaulting to current directory." << endl
                   << endl
                   << "                    SrcDir = CurrentDir[" << srcDir << "]" << endl
                   << endl;
  }

  if  (destFileName.Empty ())
  {
    destFileName = osGetRootNameOfDirectory (srcDir) + ".spr";
    log.Level (10) << endl << endl 
                   << "ScannerFileEncoder   No destination file name specified; defaulting to directory name." << endl
                   << endl
                   << "                    DestFileName[" << destFileName << "]" << endl
                   << endl;
  }


  if  (osFileExists (destFileName))
  {
    log.Level (-1) << endl << endl
      << "ScannerFileEncoder   ***ERROR***    Destination File[" << destFileName << "]  Already Exists." << endl
      << endl;
    Abort (true);
    DisplayCommandLineParameters ();
    return;
  }

  totalImages = (int)(imagesPerSec * scannerFileLenInSecs);
  scannerFileLenInScanLines = (long)(scannerFileLenInSecs * scanRate);

  // Initialize ScanLineBuffer
  ScanLinePtr  sl = new ScanLine (scanLineWidth);
  scanLineBuffer.PushOnBack (sl);
  scanLineBufferStart = 0;
  scanLineBufferEnd   = 0;
}





ScannerFileEncoder::~ScannerFileEncoder ()
{
  delete  srcImages;  srcImages = NULL;
}






bool  ScannerFileEncoder::ProcessCmdLineParameter (char    parmSwitchCode, 
                                                   KKStr   parmSwitch, 
                                                   KKStr   parmValue
                                                  )
{
  KKStr  parmValueUpper (parmValue);
  parmValueUpper.Upper ();

  parmSwitch.Upper ();

  if  ((parmSwitch == "-S")  ||  (parmSwitch == "-SRC")||  (parmSwitch == "-SOURCE")  ||  (parmSwitch == "-SRCDIR"))
  {
    srcDir = parmValue;
    if  (!osValidDirectory (srcDir))
    {
      log.Level (-1) << "ProcessCmdLineParameter   ***ERROR***    Invalid Source Directory Specified [" << srcDir << "]." << endl;
      Abort (true);
    }
  }

  else if  ((parmSwitch == "-DEST")            ||  
            (parmSwitch == "-D")               ||  
            (parmSwitch == "-DESTINATION")     ||
            (parmSwitch == "-SCANNERFILENAME") ||
            (parmSwitch == "-SFN")
           )
  {
    if  (!destFileName.Empty ())
    {
      log.Level (-1) << "ProcessCmdLineParameter    ***ERROR***    More than one destination file was specified." << endl;
      Abort (true);
    }
    
    destFileName = parmValue;
    if  (osFileExists (destFileName))
    {
      log.Level (-1) << "ProcessCmdLineParameter   ***ERROR***    Destination File[" << destFileName << "] already exists." << endl;
      Abort (true);
    }
  }

  else
  {
    log.Level (-1) << endl << endl
                   << "ScannerFileEncoder::ProcessCmdLineParameter    ***ERROR***" << endl
                   << endl
                   << "             Invalid Parameter[" << parmSwitch << "]" << endl
                   << endl;
    Abort (true);
  }


	return  !Abort ();
}  /* ProcessCmdLineParameter */




/******************************************************************************
 * DisplayCommandLineParameters()
 * DESC: Displays a help message to the user
 ******************************************************************************/
void   ScannerFileEncoder::DisplayCommandLineParameters ()
{
  log.Level (0) << "ScannerFileEncoder"                                                               << endl;
  log.Level (0)                                                                                      << endl;
  log.Level (0) << "    -src   <Source Directroy>  Specify source directory where to retiev BMP"     << endl;
  log.Level (0) << "           from."                                                                << endl;
  log.Level (0)                                                                                      << endl;
  log.Level (0) << "    -dest  <Scanner File>  Name of Scanner-file to create."                        << endl;
  log.Level (0)                                                                                      << endl;
}  /* DisplayCommandLineParameters */






void  ScannerFileEncoder::AddScanLine (uchar* scanLine, 
                                       int    width
                                      )
{
  const int  lineLen = 2048;
  int  leftPad = (lineLen - width) / 2;

  uchar  buff[lineLen];

  memset (buff, 0, sizeof (buff));
  for  (int x = 0;  x < width;  x++)
    buff[x + leftPad] = 255 - scanLine[x];

  scannerFile->WriteScanLine (buff, width);
}  /* AddScanLine */



void  ScannerFileEncoder::ProcessFile (const KKStr&  fileName)
{
  log.Level (10) << "ScannerFileEncoder::ProcessFile   FileName[" << fileName << "]" << endl;

  bool  validFile = false;
  RasterPtr image = new Raster (fileName, validFile);
  if  (!validFile)
  {
    log.Level (-1) 
      << endl 
      << "ScannerFileEncoder::ProcessFile   ***ERROR***   File[" << fileName << "]  Can not be read." << endl
      << endl;
    delete  image;
    image = NULL;
    return;
  }

  int  width = image->Width ();
  int  height = image->Height ();
  if  (width > 4096)
  {
    log.Level (-1) 
      << endl 
      << "ScannerFileEncoder::ProcessFile   ***ERROR***   File[" << fileName << "]  Image Width[" << width << "] is too wide." << endl
      << endl;
    delete  image;
    image = NULL;
    return;
  }

  int  row;
  for  (row = 0;  row < height;  row++)
  {
    AddScanLine (image->Green ()[row], width);
  }

  {
    KKStr  saveImageFileName = "C:\\Temp\\Shrimp\\" + osGetRootName (fileName) + ".bmp";
    SaveImage (*image, saveImageFileName);
  }




  delete  image;
  image = NULL;
}  /* ProcessFile */



void  ScannerFileEncoder::ProcessDirectory (KKStr  dirName)
{
  osAddLastSlash (dirName);
  log.Level (10) << "ScannerFileEncoder::ProcessDirectory [" << dirName << "]" << endl;

  KKStrListPtr  fileNames = osGetListOfFiles (osAddSlash (dirName) + "*.*");
  if  ((fileNames == NULL)  ||  (fileNames->QueueSize () < 1))
  {
    log.Level (10) << "ProcessDirectory  No files found." << endl;
    return;
  }

  int numFilesProcessed = 0;


  KKStrList::iterator  idx;
  for  (idx = fileNames->begin ();  idx != fileNames->end ();  idx++)
  {
    KKStrPtr  fileName = *idx;
    KKStr extension = osGetFileExtension (*fileName);
    if  (extension.EqualIgnoreCase ("bmp"))
    {
      ProcessFile (dirName + *fileName);
      numFilesProcessed++;
    }
  }

  KKStrListPtr  subDirectories = osGetListOfDirectories (dirName);
  for  (idx = subDirectories->begin ();  idx != subDirectories->end ();  idx++)
  {
    KKStrPtr  subDirName = *idx;
    ProcessDirectory (dirName + *subDirName);
  }
  delete  subDirectories;  subDirectories = NULL;

  delete  fileNames;       fileNames = NULL;
}  /* ProcessDirectory */






RasterListPtr  ScannerFileEncoder::GetImagesInSubDirectory (KKStr  dirName)
{
  osAddLastSlash (dirName);
  log.Level (10) << "ScannerFileEncoder::ProcessDirectory [" << dirName << "]" << endl;

  RasterListPtr  results = new RasterList (true);
  
  KKStrListPtr  fileNames = osGetListOfFiles (osAddSlash (dirName) + "*.*");
  if  ((fileNames == NULL)  ||  (fileNames->QueueSize () < 1))
  {
    log.Level (10) << "ProcessDirectory  No files found." << endl;
    return NULL;
  }

  int numFilesProcessed = 0;


  KKStrList::iterator  idx;
  for  (idx = fileNames->begin ();  idx != fileNames->end ();  idx++)
  {
    KKStrPtr  fileName = *idx;
    KKStr extension = osGetFileExtension (*fileName);
    if  (extension.EqualIgnoreCase ("bmp"))
    {
      KKStr  fullFileName = dirName + *fileName;
      RasterPtr i = ReadImage (fullFileName);
      if  (i)
        results->PushOnBack (i);
    }
  }

  KKStrListPtr  subDirectories = osGetListOfDirectories (dirName);
  for  (idx = subDirectories->begin ();  idx != subDirectories->end ();  idx++)
  {
    KKStrPtr  subDirName = *idx;
    RasterListPtr  imagesInSubDir = GetImagesInSubDirectory (dirName + *subDirName);
    if  (imagesInSubDir)
    {
      results->AddQueue (*imagesInSubDir);
      imagesInSubDir->Owner (false);
      delete  imagesInSubDir;  
      imagesInSubDir = NULL;
    }
  }

  delete  subDirectories;  subDirectories = NULL;
  return  results;
}  /* GetImagesInSubDirectory */




void  ScannerFileEncoder::PopulateScanLinePositions ()
{
  int  x = 0;

  for  (x = 0;  x < totalImages;  x++)
  {
    ulong  scanLine = randGen.Next () % scannerFileLenInScanLines;
    scanLinePositions.push_back (scanLine);
  }

  sort (scanLinePositions.begin (), scanLinePositions.end ());
}  /* PopulateScanLinePositions */





RasterPtr  ScannerFileEncoder::GenerateRandomImage ()
{
  int  imageIdx = randGen.Next () % srcImages->QueueSize ();

  float  rotateAngle = (float)(randGen.Next () % 90) / 10;

  RasterPtr  rotatedImage = srcImages->IdxToPtr (imageIdx)->Rotate (rotateAngle);

  RasterPtr  resultImage = rotatedImage->TightlyBounded (3);

  delete  rotatedImage;  rotatedImage = NULL;


  if  (resultImage->Width () > 2048)
  {
    float   redFactor = 2000.0f / (float)resultImage->Width ();
    RasterPtr  reducedImage = resultImage->ReduceByFactor (redFactor);
    delete  resultImage;
    resultImage = reducedImage;
    reducedImage = NULL;
  }

  return  resultImage;
}  /* GenerateRandomImage */






void  ScannerFileEncoder::AddScanLineToBuffer ()
{
  ScanLinePtr scanLine = new ScanLine (scanLineWidth);
  scanLineBuffer.PushOnBack (scanLine);
  scanLineBufferEnd++;
}  /* AddScanLineToBuffer */



void  ScannerFileEncoder::FlushScanLineBuffer (ulong scanRow)
{
  while  (scanLineBufferStart < scanRow)
  {
    if  (scanLineBufferEnd <= scanRow)
      AddScanLineToBuffer ();

    ScanLinePtr  scanLine = scanLineBuffer.PopFromFront ();
    scanLineBufferStart++;

    scannerFile->WriteScanLine (scanLine->scanLine, scanLineWidth);

    delete  scanLine;  scanLine = NULL;
  }
}  /* FlushScanLineBuffer */




void  ScannerFileEncoder::WritePixelToScannerFile (ulong  scanLineRow, 
                                                  uint   col,
                                                  uchar  pixel
                                                 )
{
  if  (scanLineRow < scanLineBufferStart)
  {
    log.Level (-1) << endl << endl
      << "ScannerFileEncoder::WritePixelToScannerFile   ***ERROR***    ScanLineRow[" << (uint64)scanLineRow << "] preceeds burrenty uffer position." << endl
      << endl;
    return;
  }

  while  (scanLineRow > scanLineBufferEnd)
    AddScanLineToBuffer ();

  uint  bufferRow = scanLineRow - scanLineBufferStart;

  if  (col >= scanLineWidth)
  {
    log.Level (-1) << endl << endl
      << "ScannerFileEncoder::WritePixelToScannerFile   ***ERROR***    Col[" << col << "] Excceds scan line width." << endl
      << endl;
    return;
  }

  scanLineBuffer[bufferRow][col] = 255 - pixel;
}  /* WritePixelToScannerFile */



void  ScannerFileEncoder::BuildScannerFile ()
{
  delete  srcImages;  
  
  srcImages = GetImagesInSubDirectory (srcDir);
  
  PopulateScanLinePositions ();

  RasterPtr  i = NULL;

  int  imageIdx = 0;
  for  (imageIdx = 0;  imageIdx < totalImages;  imageIdx++)
  {
    ulong  startScannerRow = scanLinePositions[imageIdx];

    FlushScanLineBuffer (Max ((ulong)0, startScannerRow - 1));

    i = GenerateRandomImage ();

    cout << "Idx"      << "\t" << imageIdx       << "\t" 
         << "ScanLine" << "\t" << startScannerRow << "\t" 
         << "Image"    << "\t" << i->FileName () 
         << endl;

    uint  leeWay = 2048 - i->Width ();

    uint  randOffset = 0;
    if  (leeWay >= 1)
       randOffset = randGen.Next () % leeWay;
    uint startCol = 1024 + randOffset;

    int  row, col;
    uchar**  green = i->Green ();
    for  (row = 0;  row < i->Height ();  row++)
    {
      uchar*  greenRow = green[row];
      ulong  sipperRow = startScannerRow + row;
      uint   scannerCol = startCol;
      for  (col = 0;  col < i->Width ();  col++)
      {
        if  (greenRow[col] > 0)
          WritePixelToScannerFile (sipperRow, scannerCol, greenRow[col]);
        scannerCol++;
      }
    }

    delete  i;
    i = NULL;
  }

  FlushScanLineBuffer (scanLineBufferEnd);
}  /* BuildScannerFile */





void   ScannerFileEncoder::Main ()
{
  if  (Abort ())
    return;

  scannerFile =  new ScannerFile3BitEncoded (destFileName.Str (), log);
  if  (!scannerFile->Opened ())
  {
    log.Level (-1) << endl << endl
      << "ScannerFileEncoder::Main   Error opening file[" << destFileName << "]" << endl
      << endl;
    return;
  }

  BuildScannerFile ();

  delete  scannerFile;
  scannerFile = NULL;
}  /* Main */






int  main (int     argc,
           char**  argv
          )
{
  ScannerFileEncoder  scannerFileEncoder (argc, argv);
  if  (scannerFileEncoder.Abort ())
    return 1;

  scannerFileEncoder.Main ();
  if  (scannerFileEncoder.Abort ())
    return 1;
  else
    return 0;
}
