#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Collections::Generic;
using namespace System::Diagnostics;

#include "StartStopPoint.h"


namespace LarcosCounterManaged
{
  public  ref  class  UmiStartStopPoint
  {
  public:
    enum  class  StartStopType: int  
    {
      sspNULL       = StartStopPoint::sspNULL,
      sspStartPoint = StartStopPoint::sspStartPoint,
      sspStopPoint  = StartStopPoint::sspStopPoint,
      sspInvalid    = StartStopPoint::sspInvalid,
    };


    UmiStartStopPoint  (Int32          _scanLineNum,
                        StartStopType  _type
                       );

    UmiStartStopPoint  (StartStopPointPtr  _startStopPoint);

    UmiStartStopPoint  (const StartStopPoint&  _startStopPoint);

    property Int32           ScanLineNum {Int32          get ()  {return scanLineNum;}  void  set (Int32          _scanLineNum)  {scanLineNum  = _scanLineNum;}}
    property StartStopType   Type        {StartStopType  get ()  {return type;}         void  set (StartStopType  _type)         {type         = _type;}}
    property String^         TypeStr     {String^        get ();}

  private:
    kkint32        scanLineNum;
    StartStopType  type;
  };  /* UmiStartStopPoint */



  public  ref  class  UmiStartStopPointList: public List<UmiStartStopPoint^>
  {
  public:
    UmiStartStopPointList (const StartStopPointList&  list);


  };  /* UmiStartStopPointList */




  public  ref  class  UmiStartStopRegion
  {
  public:
    UmiStartStopRegion (const StartStopRegion&  region);

    property  Int32     Start  {Int32    get ()  {return start;}   void  set (Int32  _start)  {start = _start;}}
    property  Int32     End    {Int32    get ()  {return end;}     void  set (Int32  _end)    {end   = _end;}}

  private:
    kkint32  start;
    kkint32  end;
  };



  public  ref  class  UmiStartStopRegionList: public List<UmiStartStopRegion^>
  {
  public:
    UmiStartStopRegionList (const StartStopRegionList&  list);

    bool  ScanLineIncluded (Int32  scanLine);

  };  /* UmiStartStopRegionList */




}
