/* ReportWriterThread.cpp -- Responsible for writing camera data to disk.
 * Copyright (C) 2011-2011  Kurt Kramer
 * For conditions of distribution and use, see copyright notice in KKLineScanner.h
 */
#include "FirstIncludes.h"

#include <errno.h>
#include <istream>
#include <iostream>
#include <fstream>
#include <deque>
#include <map>
#include <vector>
#include "MemoryDebug.h"
using namespace std;


#include "KKBaseTypes.h"
#include "ImageIO.h"
#include "OSservices.h"
using namespace KKB;

#include "Variables.h"
using namespace  KKLSC;

#include "ReportWriterThread.h"
#include "CameraThread.h"
#include "InstallationConfig.h"
#include "CounterManager.h"
#include "SessionParameters.h"
#include "OperatingParameters.h"
#include "ParticleEntry.h"
using  namespace  CounterUnManaged;



ReportWriterThread::ReportWriterThread (CounterManagerPtr      _manager,
                                        ParticleEntryBufferPtr       _particleEntryBuffer,
                                        const KKStr&                 _initialScannerFileName,
                                        const SessionParametersPtr   _sessionParameters,
                                        const OperatingParametersPtr _operatingParameters,
                                        MsgQueuePtr                  _msgQueue,
                                        const KKStr&                 _threadName
                                       ):

    CameraThread (_manager, _threadName, _msgQueue),
    dateTimeCompleted       (),
    dateTimeStarted         (),
    initialScannerFileName  (_initialScannerFileName),
    operatingParameters     (_operatingParameters),
    particleEntryBuffer     (_particleEntryBuffer),
    playingBack             (false),
    report                  (NULL),
    reportFileName          (),
    sessionParameters       (_sessionParameters),
    systemTimeUsedCompleted (0.0),
    systemTimeUsedStarted   (0.0)
{
  log.Level (10) << "ReportWriterThread::ReportWriterThread" << endl;
  playingBack = Manager ()->PlayingBack ();
  reportFileName = osRemoveExtension (initialScannerFileName) + ".rpt";

  dateTimeStarted       = osGetLocalDateTime ();
  systemTimeUsedStarted = osGetSystemTimeUsed ();

  report = new ofstream (reportFileName.Str ());
}



ReportWriterThread::~ReportWriterThread ()
{
  delete  report;
  report = NULL;
  log.Level (10) << "ReportWriterThread::~ReportWriterThread" << endl;
}



kkMemSize  ReportWriterThread::MemoryConsumedEstimated ()
{
  return  sizeof (ReportWriterThread);
}


void  ReportWriterThread::GetStats (CounterStats&  stats)  const
{
}



void  ReportWriterThread::WriteHeader ()
{
  *report << "RunDateTime"            << "\t" << osGetLocalDateTime ()              << endl
          << "InitialScannerFileName" << "\t" << initialScannerFileName             << endl
          << "SrcScannerFileName"     << "\t" << Manager ()->SrcScannerFileName ()  << endl
          << "CameraMacAddress"       << "\t" << Manager ()->CameraMacAddress   ()  << endl
          << "CameraFrameHeight"      << "\t" << Manager ()->CameraFrameHeight  ()  << endl
          << "CameraFrameWidth"       << "\t" << Manager ()->CameraFrameWidth   ()  << endl
          << "CropLeft"               << "\t" << Manager ()->CropLeft           ()  << endl
          << "CropRight"              << "\t" << Manager ()->CropRight          ()  << endl;

  sessionParameters->WriteXML (*report);
  operatingParameters->WriteXML (*report);
  InstallationConfigPtr  instalation = Manager ()->Installation ();
  if  (instalation)
    instalation->WriteXML (*report);
}


void  ReportWriterThread::AddDataLine (const KKStr& lineName,
                                       const KKStr& lineValue
                                      )
{
  particleEntryBuffer->StartBlock ();
  *report << lineName << "\t" << lineValue << endl;
  particleEntryBuffer->EndBlock ();
}


void  ReportWriterThread::Run ()
{
  Status (ThreadStatus::Running);
  log.Level (10) << "ReportWriterThread::Run" << endl;
  if  (!particleEntryBuffer)
  {
    Status (ThreadStatus::Stopping);
    return;
  }

  WriteHeader ();
  particleEntryBuffer->PrintImageEntryDefinition (*report);

  while  ((!TerminateFlag ())  &&  (!ShutdownFlag ()))
  {
    if  (particleEntryBuffer->DataAvailable ())
      particleEntryBuffer->AddToReport (*report);
    osSleepMiliSecs (10);
  }

  particleEntryBuffer->AddToReport (*report);

  dateTimeCompleted       = osGetLocalDateTime ();
  systemTimeUsedCompleted = osGetSystemTimeUsed ();


  *report << "DateTimeStarted"   << "\t" << dateTimeStarted   << endl;
  *report << "DateTimeCompleted" << "\t" << dateTimeCompleted << endl;

  kkuint64  elaspedTimeInSecs = dateTimeCompleted.Seconds () - dateTimeStarted.Seconds ();
 
  *report << "SystemTimeUsedStarted"    << "\t" << systemTimeUsedStarted   << endl;
  *report << "SystemTimeUsedCompleted"  << "\t" << systemTimeUsedCompleted << endl;
  *report << "ElaspedTimeInSecs"        << "\t" << elaspedTimeInSecs       << endl;
  *report << "SystemTimeUsedTotal"      << "\t" << (systemTimeUsedCompleted - systemTimeUsedStarted) << endl;

  delete  report;
  report = NULL;

  Status (ThreadStatus::Stopping);

  log.Level (10) << "ReportWriterThread::Run   Exiting   TerminateFlag: " << TerminateFlag () <<  "  ShutdownFlag: " << ShutdownFlag () << endl;

  return;
}  /* Run */







void  ReportWriterThread::ResetCounts ()
{
}
