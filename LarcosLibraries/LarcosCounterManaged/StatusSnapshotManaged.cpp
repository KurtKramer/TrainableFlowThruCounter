#include "StdAfx.h"
#include "FirstIncludes.h"

#include <stdio.h>
#include <math.h>

#include <ctype.h>
#include <time.h>
#include <string>
#include "MemoryDebug.h"
using namespace std;

#include "KKStr.h"
using namespace  KKB;

#include "StatusSnapshot.h"
using namespace LarcosCounterUnManaged;

#include "UmiKKStr.h"
#include "StatusSnapshotManaged.h"
using namespace  LarcosCounterManaged;


using namespace System;
using namespace System::Threading;
//using namespace System::Windows::Forms;



StatusSnapshotManaged::StatusSnapshotManaged (const StatusSnapshot&  stats):

    timeStamp                       (DateTimeKKBtoSystem (stats.TimeStamp ())),
    timeOffset                      (stats.TimeOffset                      ()),
    logicalFrameProcessorsAvailable (stats.LogicalFrameProcessorsAvailable ()),
    logicalFramesOnQueue            (stats.LogicalFramesOnQueue            ()),
    count                           (stats.Count                           ()),
    particles                       (stats.Particles                       ()),
    cpuUsage                        (stats.CpuUsage                        ()),
    availableCapacity               (stats.AvailableCapacity               ()),
    physicalFramesDropped           (stats.PhysicalFramesDropped           ()),
    logicalFramesDropped            (stats.LogicalFramesDropped            ()),
    scanLinesRead                   (stats.ScanLinesRead                   ()),
    scanLinesWritten                (stats.ScanLinesWritten                ()),
    particlesWaitingProcessing      (stats.ParticlesWaitingProcessing      ()),
    flowRate                        (stats.FlowRate                        ())
{
}



StatusSnapshotManaged::!StatusSnapshotManaged ()
{
  CleanUpMemory ();
}



StatusSnapshotManaged::~StatusSnapshotManaged ()
{
  this->!StatusSnapshotManaged ();
}


void  StatusSnapshotManaged::CleanUpMemory ()
{
}



System::DateTime   StatusSnapshotManaged::DateTimeKKBtoSystem (const  KKB::DateTime&  date)
{
  int  year  = date.Date ().Year  ();
  int  month = date.Date ().Month ();
  int  day   = date.Date ().Day   ();

  int  hours   = date.Time ().Hour   ();
  int  minutes = date.Time ().Minute ();
  int  seconds = date.Time ().Second ();

  bool valid = true;

  month   = Min (12, Max (1, month));
  day     = Min (31, Max (1, day));
  hours   = Min (24, Max (0, hours));
  minutes = Min (60, Max (0, minutes));

  System::DateTime  dt;
  try
  {
    dt = System::DateTime (year, month, day, hours, minutes, seconds);
  }
  catch (Exception^)
  {
    dt = System::DateTime (1900, 1, 1, 0, 0, 0);
  }

  return  dt;
}  /* DateTimeKKBtoSystem */



KKB::DateTime   StatusSnapshotManaged::DateTimeSystemToKKB (System::DateTime  date)
{
  return  KKB::DateTime (date.Year, date.Month, date.Day, date.Hour, date.Minute, date.Second);
}  /* DateTimeKKBtoSystem */



float  StatusSnapshotManaged::GetDataField (DataFieldIdx  dataField)
{
  float  data = 0.0f;

  switch  (dataField)
  {
  case  DataFieldIdx::LogicalFrameProcessorsAvailable:
    data = logicalFrameProcessorsAvailable;
    break;

  case  DataFieldIdx::LogicalFramesOnQueue:
    data = logicalFramesOnQueue;
    break;

  case  DataFieldIdx::Count:
    data = (float)count;
    break;

  case  DataFieldIdx::Particles:
    data = (float)particles;
    break;

  case  DataFieldIdx::PhysicalFramesDropped:
    data = (float)physicalFramesDropped;
    break;

  case  DataFieldIdx::LogicalFramesDropped:
    data = (float)logicalFramesDropped;
    break;

  case  DataFieldIdx::ScanLinesRead:
    data = (float)scanLinesRead;
    break;

  case  DataFieldIdx::ScanLinesWritten:
    data = (float)scanLinesWritten;
    break;

  case  DataFieldIdx::ParticlesWaitingProcessing:
    data = (float)particlesWaitingProcessing;
    break;

  case  DataFieldIdx::FlowRate:
    data = flowRate;
    break;
  }

  return data;
}



StatusSnapshotManaged::DataFieldIdx  StatusSnapshotManaged::DataFieldIdxFromStr (String^ s)
{
  return  (DataFieldIdx)StatusSnapshot::DataFieldIdxFromStr (UmiKKStr::SystemStringToKKStr (s));
}



String^  StatusSnapshotManaged::DataFieldIdxToStr (StatusSnapshotManaged::DataFieldIdx idx)
{
  return  UmiKKStr::KKStrToSystenStr (StatusSnapshot::DataFieldIdxToStr ((StatusSnapshot::DataFieldIdx)idx));
}



array<String^>^  StatusSnapshotManaged::DataFieldIdxNames ()
{
  array<String^>^  a = gcnew array<String^> (StatusSnapshot::dfiInvalid);
  for  (int  x = 0;  x < (int)StatusSnapshot::dfiInvalid;  ++x)
    a[x] = UmiKKStr::KKStrToSystenStr (StatusSnapshot::DataFieldIdxToStr ((StatusSnapshot::DataFieldIdx)x));
  return  a;
}




StatusSnapshotManagedList::StatusSnapshotManagedList ()
{
}



StatusSnapshotManagedList::StatusSnapshotManagedList (const StatusSnapshotList&  snapshots)
{
  StatusSnapshotList::const_iterator  idx;
  for  (idx = snapshots.begin ();  idx != snapshots.end ();  ++idx)
    Add (gcnew StatusSnapshotManaged (**idx));
}



StatusSnapshotManagedList::~StatusSnapshotManagedList ()
{
  this->!StatusSnapshotManagedList ();
}



StatusSnapshotManagedList::!StatusSnapshotManagedList ()
{
}
