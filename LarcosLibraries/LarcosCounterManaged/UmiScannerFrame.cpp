#include "StdAfx.h"
#include "FirstIncludes.h"

#include <stdio.h>
#include <vector>
#include "MemoryDebug.h"
using  namespace  std;


#include "KKBaseTypes.h"
using namespace  KKB;


#include "UmiScannerFrame.h"
using namespace  LarcosCounterManaged;


UmiScannerFrame::UmiScannerFrame (UmiScannerClock^ _clock,
                                  int              _scanLinesPerFrame,
                                  int              _pixelsPerScanLine
                                 ):
  
  clock         (_clock),
  frameNum      (-1),
  scanLines     (nullptr),
  scanLineFirst (-1),
  scanLineLast  (-1),
  time          (0)

{
  clock = _clock;
  int x = 0;
  scanLines = AllocateFrameArray (_scanLinesPerFrame, _pixelsPerScanLine);
}




UmiScannerFrame::FrameArray^  UmiScannerFrame::AllocateFrameArray (int  _scanLinesPerFrame,
                                                                   int  _pixelsPerScanLine
                                                                  )
{
  FrameArray^  a = gcnew array<array<Byte>^ >  (_scanLinesPerFrame);
  for  (int r = 0;  r < _scanLinesPerFrame;  r++)
  {
    a[r] = gcnew array<Byte> (_pixelsPerScanLine);
  }

  return a;
}



UmiScannerFrame::FrameArray^  UmiScannerFrame::ScanLines::get ()
{
  time = clock->Time;
  return scanLines;
} 



void  UmiScannerFrame::ScanLines::set (FrameArray^ _scanLines)
{
  time = clock->Time;
  scanLines = _scanLines;
}
