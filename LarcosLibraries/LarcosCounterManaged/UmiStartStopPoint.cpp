#include "StdAfx.h"
#include "FirstIncludes.h"

#include <stdio.h>
#include <ctype.h>
#include <vector>
#include "MemoryDebug.h"


#include "KKBaseTypes.h"
using namespace KKB;


#include "StartStopPoint.h"
using namespace  KKLSC;

#include "UmiKKStr.h"
#include "UmiStartStopPoint.h"

using namespace LarcosCounterManaged;


using namespace System;
using namespace System::Threading;
using namespace System::Windows::Forms;


UmiStartStopPoint::UmiStartStopPoint  (StartStopPointPtr  _startStopPoint):
    scanLineNum  (_startStopPoint->ScanLineNum ()),
    type         ((StartStopType)(_startStopPoint->Type ()))
{
}



UmiStartStopPoint::UmiStartStopPoint  (const StartStopPoint&  _startStopPoint):
    scanLineNum  (_startStopPoint.ScanLineNum ()),
    type         ((StartStopType)(_startStopPoint.Type ()))
{
}




UmiStartStopPoint::UmiStartStopPoint  (Int32          _scanLineNum,
                                       StartStopType  _type
                                      ):
      scanLineNum  (_scanLineNum),
      type         (_type)
{
}



String^   UmiStartStopPoint::TypeStr::get ()     
{
  return  UmiKKStr::KKStrToSystenStr (StartStopPoint::StartStopTypeToStr ((StartStopPoint::StartStopType)type));
}


UmiStartStopRegion::UmiStartStopRegion (const StartStopRegion&  region):
  start (region.Start ()),
  end   (region.End ())
{
}




UmiStartStopPointList::UmiStartStopPointList (const StartStopPointList&  list)
{
  StartStopPointList::const_iterator  idx;

  for  (idx = list.begin ();  idx != list.end ();  ++idx)
  {
    StartStopPointPtr  p = *idx;
    Add (gcnew UmiStartStopPoint (*p));
  }
}




UmiStartStopRegionList::UmiStartStopRegionList (const StartStopRegionList&  list)
{
  StartStopRegionList::const_iterator  idx;

  for  (idx = list.begin ();  idx != list.end ();  ++idx)
  {
    const StartStopRegion*  startStopPoint = *idx;
    Add (gcnew UmiStartStopRegion (*startStopPoint));
  }
}
 


bool  UmiStartStopRegionList::ScanLineIncluded (Int32  scanLine)
{
  for each  (UmiStartStopRegion^  r in *this)
  {
    if  ((scanLine >= r->Start)  &&  (scanLine <= r->End))
      return true;
  }

  return  false;
}
