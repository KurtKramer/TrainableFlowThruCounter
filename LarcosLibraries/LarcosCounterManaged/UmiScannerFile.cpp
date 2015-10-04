#include "StdAfx.h"
#include "FirstIncludes.h"
#include <stdio.h>
#include <math.h>
#include <ctype.h>
#include <time.h>
#include <fstream>
#include <iostream>
#include <map>
#include <ostream>
#include <string>
#include <vector>
#include "MemoryDebug.h"


#include "KKBaseTypes.h"
#include "..\\KKBase\\GoalKeeper.h"
#include "OSservices.h"
using namespace  KKB;

#include "ScannerFile.h"
using namespace  KKLSC;

#include "UmiScannerFile.h"

#include "UmiKKStr.h"
using namespace  LarcosCounterManaged;


using namespace System;
using namespace System::Threading;
using namespace System::Windows::Forms;
using namespace System::Runtime::InteropServices;

#define  MemPreasPerScannerFile  10000



UmiScannerFile::Format  UmiScannerFile::ScannerFileFormatFromStr (String^ s)
{
  KKStr  kkS = UmiKKStr::SystemStringToKKStr (s);
  ScannerFile::Format  
    sf = ScannerFile::ScannerFileFormatFromStr (kkS);

  return  (Format)sf;
}



String^  UmiScannerFile::ScannerFileFormatToStr (Format  f)
{
  ScannerFile::Format  sff = (ScannerFile::Format)f;
  KKStr  s = ScannerFile::ScannerFileFormatToStr (sff);
  return UmiKKStr::KKStrToSystenStr (s);
}



UmiScannerFile::UmiScannerFile (ScannerFilePtr  _scannerFile,
                                UmiRunLog^      _log
                               ):
    frameNumLastRead                 (-1),
    frameNumNextRead                 (0),
    log                              (_log),
    memoryPreasureAdded              (0),
    scanLinesPerFrame                (_scannerFile->FrameHeight ()),
    scannerFile                      (_scannerFile),
    updateFrameByteOffsetsRunning    (false),
    updateFrameByteOffsetsThread     (nullptr),
    updateFrameByteOffsetsCancelFlag (NULL),

    // variables needed to read a single scan line.
    pixelsPerScanLine                (0),
    lineBuff                         (NULL),
    colCount                         (NULL)
{
  updateFrameByteOffsetsCancelFlag = new bool ();

  if  (log == nullptr)
    log = gcnew UmiRunLog ();

  pixelsPerScanLine = (kkuint32)scannerFile->PixelsPerScanLine ();
  lineBuff = new uchar[pixelsPerScanLine + 100];
  colCount = new kkuint32[pixelsPerScanLine + 100];

  UpdateMemoryPressure ();
}



UmiScannerFile::~UmiScannerFile ()
{
  this->!UmiScannerFile ();
}



UmiScannerFile::!UmiScannerFile ()
{
  Close ();
  GC::RemoveMemoryPressure (memoryPreasureAdded);
  delete  updateFrameByteOffsetsCancelFlag;
  updateFrameByteOffsetsCancelFlag = NULL;
}



void  UmiScannerFile::UpdateMemoryPressure ()
{
  kkint32  mem = 0;
  if  (lineBuff)  mem += ((100 + pixelsPerScanLine) * sizeof (uchar));
  if  (colCount)  mem += ((100 + pixelsPerScanLine) * sizeof (kkuint32));

  if  (scannerFile)  mem += scannerFile->MemoryConsumedEstimated ();

  kkint32  deltaMemory = mem - memoryPreasureAdded;

  if  (deltaMemory > 0)
    GC::AddMemoryPressure (deltaMemory);
  
  else if  (deltaMemory < 0)
    GC::RemoveMemoryPressure (0 - deltaMemory);

  memoryPreasureAdded = mem;
}




void  UmiScannerFile::Close ()
{
  if  (updateFrameByteOffsetsRunning)
     FrameBytesOffsetsUpdateThreadTerminate ();

  delete  scannerFile;  scannerFile = NULL;
  delete  lineBuff;     lineBuff = NULL;
  delete  colCount;     colCount = NULL;
}



UmiScannerFile^  UmiScannerFile::CreateScannerFile (String^     _fileName,
                                                    UmiRunLog^  _log
                                                   )
{
  if  (_log == nullptr)
    _log = gcnew UmiRunLog ();

  ScannerFilePtr  scannerFile = ScannerFile::CreateScannerFile (UmiKKStr::SystemStringToKKStr (_fileName), _log->Log ());
  if  (!scannerFile)
    return nullptr;
  else
    return gcnew UmiScannerFile (scannerFile, _log);
}



UmiScannerFile^  UmiScannerFile::CreateScannerFileForOutput (String^     _fileName,
                                                             String^     _formatStr,
                                                             UInt32      _pixelsPerScanLine,
                                                             UInt32      _frameHeight,
                                                             UmiRunLog^  _log
                                                            )
{
  ScannerFilePtr  scannerFile = ScannerFile::CreateScannerFileForOutput 
                                (UmiKKStr::SystemStringToKKStr (_fileName),
                                 UmiKKStr::SystemStringToKKStr (_formatStr),
                                 _pixelsPerScanLine,
                                 _frameHeight,
                                 _log->Log ()
                                );
  if  (scannerFile)
    return gcnew UmiScannerFile (scannerFile, _log);
  else
    return nullptr;
}  /* CreateScannerFileForOutput */




bool  UmiScannerFile::Eof::get ()
{
  return  (scannerFile != NULL) ? scannerFile->Eof () : true;
}



String^  UmiScannerFile::FileFormatStr::get ()
{
  return  (scannerFile != NULL) ? UmiKKStr::KKStrToSystenStr (scannerFile->FileFormatStr ()) : nullptr;
}



String^  UmiScannerFile::FileName::get ()
{
  return  (scannerFile != NULL) ? UmiKKStr::KKStrToSystenStr (scannerFile->FileName ()) : nullptr;
}



long long  UmiScannerFile::FileSizeInBytes::get ()
{
  return  (scannerFile != NULL) ? scannerFile->FileSizeInBytes () : 0;
}  



bool  UmiScannerFile::FrameIndexBuilt::get ()
{
  if  (scannerFile)
    return scannerFile->FrameOffsetsLoaded ();
  else
    return false;
}



bool  UmiScannerFile::FrameOffsetsLoaded::get ()
{
  if  (scannerFile)
    return scannerFile->FrameOffsetsLoaded ();
  else
    return false;
}



kkuint32  UmiScannerFile::LastScanLine::get ()
{
  return  (scannerFile != NULL) ? scannerFile->LastScanLine () : 0;
}


UmiScannerFile::FilePos   UmiScannerFile::FrameBufferFileOffsetLast::get ()
{
  return  (scannerFile != NULL) ? scannerFile->FrameBufferFileOffsetLast () : -1;
}


kkint32 UmiScannerFile::LargestKnowmFrameNum::get ()
{
  if  (scannerFile)
    return scannerFile->LargestKnowmFrameNum ();
  else
    return -1;
}


kkuint32  UmiScannerFile::LargestKnownScanLine::get ()
{
  if  (scannerFile)
    return scannerFile->LargestKnownScanLine ();
  else
    return 0;
}



kkuint32  UmiScannerFile::NextScanLine::get ()
{
  return  (scannerFile != NULL) ? scannerFile->NextScanLine () : 0;
}


bool   UmiScannerFile::BuildFrameOffsetsRunning::get ()
{
  return  (scannerFile != NULL) ? scannerFile->BuildFrameOffsetsRunning () : false;
}


UmiScannerFile::FilePos  UmiScannerFile::FrameBufferFileOffsetNext::get ()
{
  return  (scannerFile != NULL) ? scannerFile->FrameBufferFileOffsetNext () : -1;
}


bool  UmiScannerFile::Opened::get ()
{
  return  (scannerFile != NULL) ? scannerFile->Opened () : false;
}



kkint32  UmiScannerFile::PixelsPerScanLine::get ()
{
  return  (scannerFile != NULL) ? scannerFile->PixelsPerScanLine () : -1;
}


kkint32 UmiScannerFile::ScanLinesPerFrame::get ()
{
  return  scanLinesPerFrame;
}



float  UmiScannerFile::ScanRate::get ()
{
  return  (scannerFile != NULL) ? scannerFile->ScanRate () : 0.0f;
}



array<String^>^  UmiScannerFile::FileFormatOptions ()
{
  int  numOfOptions = (int)ScannerFile::Format::sfUnKnown;
  array<String^>^ optionsArray = gcnew array<String^> (numOfOptions);
  kkint32 x = 0;
  for  (x = 0;  x < numOfOptions;  ++x)
    optionsArray[x] = UmiKKStr::KKStrToSystenStr (ScannerFile::ScannerFileFormatToStr ((ScannerFile::Format)x));

  return  optionsArray;
}  /* FileFormatOptions */



bool  UmiScannerFile::FileFormatStrValid (String^  _formatStr)
{
  ScannerFile::Format  format = ScannerFile::ScannerFileFormatFromStr (UmiKKStr::SystemStringToKKStr (_formatStr));
  return  (format != ScannerFile::Format::sfUnKnown);
}  /* FileFormatStrValid */




void  UmiScannerFile::UpdateFrameByteOffsets ()
{
  updateFrameByteOffsetsRunning = true;
  scannerFile->BuildFrameOffsets (*updateFrameByteOffsetsCancelFlag);
  updateFrameByteOffsetsRunning = false;
  UpdateMemoryPressure ();
}  /* UpdateFrameByteOffsets */




void  UmiScannerFile::FrameBytesOffsetsUpdateThreadStart ()
{
  if  ((updateFrameByteOffsetsRunning) || (updateFrameByteOffsetsThread != nullptr))
  {
    String^  msg = "Attempted to start 'UpdateFrameByteOffsets'  but already running";
    MessageBox::Show (msg, "FrameBytesOffsetsUpdateThreadStart");
    return;
  }

  updateFrameByteOffsetsThread = gcnew Thread (gcnew ThreadStart (this, &UmiScannerFile::UpdateFrameByteOffsets));
  updateFrameByteOffsetsThread->Name = "UpdateFrameByteOffsets";
  updateFrameByteOffsetsThread->Start ();
}  /* FrameBytesOffsetsUpdateThreadStart */




void  UmiScannerFile::FrameBytesOffsetsUpdateThreadTerminate ()
{
  if  ((!updateFrameByteOffsetsRunning)  ||  (updateFrameByteOffsetsThread == nullptr))
    return;
      
  *updateFrameByteOffsetsCancelFlag = true;
      
  kkint32  loopCount = 0;
  while  ((updateFrameByteOffsetsRunning)  &&  (loopCount < 20))
    Thread::Sleep (10);

  if ((updateFrameByteOffsetsRunning) || (updateFrameByteOffsetsThread != nullptr))
    updateFrameByteOffsetsThread->Abort ();
      
  updateFrameByteOffsetsThread    = nullptr;
  updateFrameByteOffsetsRunning   = false;
  *updateFrameByteOffsetsCancelFlag = false;
}  /* FrameBytesOffsetsUpdateThreadTerminate */



void  UmiScannerFile::LoadIndexFile (bool%       successful,
                                     UmiRunLog^  runLog
                                    )
{
  if  ((updateFrameByteOffsetsRunning) || (updateFrameByteOffsetsThread != nullptr))
  {
    runLog->WriteLn (-1, "UmiScannerFile::LoadIndexFile   ***ERROR***    'updateFrameByteOffsets'  is  Running.");
    successful = false;
    return;
  }

  bool  s = false;
  scannerFile->LoadIndexFile (s);
  successful = s;
}



void  UmiScannerFile::FrameRead (kkint32          frameNum,
                                 UmiScannerFrame^ frame,
                                 bool%            found
                               )
{
  bool  frameFound = false;
  scannerFile->FrameRead (frameNum, frameFound);
  found = frameFound;
  if  (found)
  {
    uchar*  frameBuffer = scannerFile->FrameBuffer ();

    frame->FrameNum      = frameNum;
    frame->ScanLineFirst = frameNum * scanLinesPerFrame;
    frame->ScanLineLast  = frame->ScanLineFirst + scanLinesPerFrame - 1;

    FrameArray^  frameArray = frame->ScanLines;

    int  frameHeight = scannerFile->FrameHeight ();
    int  frameWidth  = scannerFile->PixelsPerScanLine ();

    uchar*  frameBufferPtr = frameBuffer;

    for  (int row = 0;  row < frameHeight;  ++row)
    {
      Marshal::Copy ((System::IntPtr)frameBufferPtr, frameArray[row], 0, frameWidth);
      frameBufferPtr += frameWidth;
    }
    frameNumLastRead = frameNum;
    frameNumNextRead = frameNum + 1;
  }
}  /* FrameRead */                   



void  UmiScannerFile::AddHeaderField (String^  _fieldName,
                                      String^  _fieldValue
                                     )
{
  if  (scannerFile)
    scannerFile->AddHeaderField (UmiKKStr::SystemStringToKKStr (_fieldName), UmiKKStr::SystemStringToKKStr (_fieldValue));
}  /* AddHeaderField */



String^  UmiScannerFile::GetValue (String^  fieldName)
{
  if  (!scannerFile)
    return nullptr;
  const KKStr&  s = scannerFile->GetValue (UmiKKStr::SystemStringToKKStr (fieldName));
  return  UmiKKStr::KKStrToSystenStr (s);
}




UmiScannerFile::FrameArray^  UmiScannerFile::CreateRatser (kkuint32  scanLineStart,
                                                           kkuint32  numScanLines
                                                          )
{
  kkuint32  lineSize = 0;
  kkuint32  forgroundPixels = 0;

  kkint32  frameNumToRead = scanLineStart / scanLinesPerFrame;
  if  (frameNumToRead > scannerFile->LargestKnowmFrameNum ())
    return nullptr;

  scannerFile->SkipToScanLine (scanLineStart);
  if  (scannerFile->Eof ())
    return nullptr;

  FrameArray^ raster = gcnew FrameArray (scannerFile->FrameHeight ());
  kkuint32  row = 0;
  for  (row = 0;  (row < numScanLines)  &&  (!scannerFile->Eof ());  ++row)
  {
    scannerFile->GetNextLine (lineBuff, pixelsPerScanLine, lineSize,  colCount, lineSize); 
    if  (scannerFile->Eof ())
      break;

    kkint32  thisRowLen = Min (pixelsPerScanLine, lineSize);

    array<byte>^  rasterRow = gcnew array<byte> (pixelsPerScanLine);
    Marshal::Copy ((System::IntPtr)lineBuff, rasterRow, 0, thisRowLen);
    raster[row] = rasterRow;
  }

  while  (row < numScanLines)
  {
    array<byte>^  rasterRow = gcnew array<byte> (pixelsPerScanLine);
    for  (kkuint32 x = 0;  x < pixelsPerScanLine;  x++)
      rasterRow[x] = 0;
    ++row;
  }

  return  raster;
}  /* CreateRatser */




array<UmiScannerFile::HeaderField^>^  UmiScannerFile::GetHeaderFields ()
{
  if  (scannerFile == NULL)
    return nullptr;

  ScannerHeaderFieldsPtr  headerFields = scannerFile->HeaderFields ();
  if  (headerFields == NULL)
    return nullptr;

  array<HeaderField^>^  results = gcnew array<HeaderField^>(headerFields->size ());

  kkint32  resultsIDX = 0;
  ScannerHeaderFields::iterator  idx;
  for  (idx = headerFields->begin ();  idx != headerFields->end ();  ++idx)
  {
    results[resultsIDX] = gcnew HeaderField (UmiKKStr::KKStrToSystenStr (idx->first), UmiKKStr::KKStrToSystenStr (idx->second));
    resultsIDX++;
  }

  return  results;
}  /* GetHeaderFields */





void  UmiScannerFile::AddStartPoint (kkint32  _scanLineNum)
{
  scannerFile->AddStartPoint (_scanLineNum);
}



void  UmiScannerFile::AddStopPoint (kkint32  _scanLineNum)
{
  scannerFile->AddStopPoint (_scanLineNum);
}


void  UmiScannerFile::StartStopPointDelete (kkint32 _scanLineNum)
{
  scannerFile->StartStopPointDelete (_scanLineNum);
}


UmiStartStopPoint^  UmiScannerFile::StartStopPointNearestEntry (kkint32 _scanLineNum)
{
  StartStopPointPtr  ssp = scannerFile->StartStopPointNearestEntry (_scanLineNum);
  if  (ssp == NULL)
    return nullptr;
  else
    return gcnew UmiStartStopPoint (*ssp);
}



UmiStartStopPoint^  UmiScannerFile::StartStopPointPrevEntry (kkint32 _scanLineNum)
{
  StartStopPointPtr  ssp = scannerFile->StartStopPointPrevEntry (_scanLineNum);
  if  (ssp == NULL)
    return nullptr;
  else
    return gcnew UmiStartStopPoint (*ssp);
}




UmiStartStopPoint^  UmiScannerFile::StartStopPointSuccEntry (kkint32 _scanLineNum)
{
  StartStopPointPtr  ssp = scannerFile->StartStopPointSuccEntry (_scanLineNum);
  if  (ssp == NULL)
    return nullptr;
  else
    return gcnew UmiStartStopPoint (*ssp);
}


UmiStartStopPointList^  UmiScannerFile::StartStopPoints ()
{
  return gcnew UmiStartStopPointList (scannerFile->StartStopPoints ());
}



UmiStartStopRegionList^  UmiScannerFile::StartStopRegions ()
{
  const  StartStopPointList&  startStopList =  scannerFile->StartStopPoints ();

  StartStopRegionListPtr  unmanagedRegions = new StartStopRegionList (startStopList);

  UmiStartStopRegionList^  regions = gcnew UmiStartStopRegionList (*unmanagedRegions);

  delete  unmanagedRegions;
  unmanagedRegions = NULL;

  return  regions;
}



const StartStopPointList&   UmiScannerFile::StartStopPointsUnManaged ()
{
  return  scannerFile->StartStopPoints ();
}



array<float>^  UmiScannerFile::RecordRateByTimeIntervals (int intervalSecs)
{
  if  (!scannerFile)
    return nullptr;
  VectorFloatPtr  scanRates = scannerFile->RecordRateByTimeIntervals (intervalSecs);
  if  (scanRates == NULL)
    return nullptr;

  kkuint32  size = scanRates->size ();
  array<float>^  scanRates2 = gcnew array<float> (size);
  for  (kkuint32 x = 0;  x < size;  ++x)
    scanRates2[x] = (*scanRates)[x];
  delete  scanRates;
  scanRates = NULL;
  return scanRates2;
}



