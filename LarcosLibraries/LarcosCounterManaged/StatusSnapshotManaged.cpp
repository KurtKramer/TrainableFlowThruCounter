#include "StdAfx.h"
#include "FirstIncludes.h"

#include <stdio.h>
#include <math.h>

#include <ctype.h>
#include <time.h>
#include <string>
#include "MemoryDebug.h"


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



float  StatusSnapshotManaged::GetDataField (FieldIdx  dataField)
{
  float  data = 0.0f;

  switch  (dataField)
  {
  case  FieldIdx::LogicalFrameProcessorsAvailable:
    data = logicalFrameProcessorsAvailable;
    break;

  case  FieldIdx::LogicalFramesOnQueue:
    data = logicalFramesOnQueue;
    break;

  case  FieldIdx::Count:
    data = (float)count;
    break;

  case  FieldIdx::Particles:
    data = (float)particles;
    break;

  case  FieldIdx::PhysicalFramesDropped:
    data = (float)physicalFramesDropped;
    break;

  case  FieldIdx::LogicalFramesDropped:
    data = (float)logicalFramesDropped;
    break;

  case  FieldIdx::ScanLinesRead:
    data = (float)scanLinesRead;
    break;

  case  FieldIdx::ScanLinesWritten:
    data = (float)scanLinesWritten;
    break;

  case  FieldIdx::ParticlesWaitingProcessing:
    data = (float)particlesWaitingProcessing;
    break;

  case  FieldIdx::FlowRate:
    data = flowRate;
    break;
  }

  return data;
}



StatusSnapshotManaged::FieldIdx  StatusSnapshotManaged::SnapShotFieldIdxFromStr (String^ s)
{
  return  (FieldIdx)StatusSnapshot::SnapShotFieldIdxFromStr (UmiKKStr::SystemStringToKKStr (s));
}



String^  StatusSnapshotManaged::SnapShotFieldIdxToStr (StatusSnapshotManaged::FieldIdx idx)
{
  return  UmiKKStr::KKStrToSystenStr (StatusSnapshot::SnapShotFieldIdxToStr ((StatusSnapshot::FieldIdx)idx));
}



array<String^>^  StatusSnapshotManaged::DataFieldIdxNames ()
{
  array<String^>^  a = gcnew array<String^> ((int)StatusSnapshot::FieldIdx::Invalid);
  for  (int  x = 0;  x < (int)StatusSnapshot::FieldIdx::Invalid;  ++x)
    a[x] = UmiKKStr::KKStrToSystenStr (StatusSnapshot::SnapShotFieldIdxToStr ((StatusSnapshot::FieldIdx)x));
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
