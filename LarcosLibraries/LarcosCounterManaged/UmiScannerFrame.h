#pragma once

#include  "UmiScannerClock.h"
using namespace System;


namespace LarcosCounterManaged 
{
  using namespace System;

  public  ref class  UmiScannerFrame
  {
  public:
    typedef  array<array<Byte>^ >   FrameArray;

    UmiScannerFrame (UmiScannerClock^ _clock,
                     int              _scanLinesPerFrame,
                     int              _pixelsPerScanLine
                    );

    property  FrameArray^   ScanLines      {FrameArray^  get ();   void  set (FrameArray^ _scanLines);}
    property  int           FrameNum       {int  get ()  {return frameNum;     } void  set (int  _frameNum)      {frameNum       = _frameNum;}}
    property  long          ScanLineFirst  {long get ()  {return scanLineFirst;} void  set (long _scanLineFirst) {scanLineFirst  = _scanLineFirst;}}
    property  long          ScanLineLast   {long get ()  {return scanLineLast; } void  set (long _scanLineLast)  {scanLineLast   = _scanLineLast;}}
    property  long          Time           {long get ()  {return time;         } void  set (long _time)          {time           = _time;}}

  private:
    FrameArray^  AllocateFrameArray (int  _scanLinesPerFrame,
                                     int  _pixelsPerScanLine
                                    );

    FrameArray^       scanLines;
    int               frameNum;
    long              scanLineFirst;
    long              scanLineLast;
    UmiScannerClock^  clock;
    long              time;
  };  /* UmiScannerFrame */
}
