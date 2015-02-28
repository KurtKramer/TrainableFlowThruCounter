/* LoggerThread.cpp -- Responsible for writing camera data to disk.
 * Copyright (C) 2011-2013  Kurt Kramer
 * For conditions of distribution and use, see copyright notice in LarcosCounterUnManaged.txt
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
#include "MsgQueue.h"
#include "OSservices.h"
using namespace KKB;

#include "Variables.h"
using namespace  KKLSC;

#include "LarcosVariables.h"
using namespace  LarcosBase;

#include "LarcosCounterManager.h"
#include "LoggerThread.h"
using  namespace  LarcosCounterUnManaged;


LoggerThread::LoggerThread (MsgQueuePtr  _msgQueue,
                            MsgQueuePtr  _loggedMsgs,
                            const KKStr& _threadName
                           ):

    KKThread (_threadName, NULL, _msgQueue),
    logFile         (NULL),
    logFileName     (),
    loggedMsgs      (_loggedMsgs),
    weOwnLoggedMsgs (false)
{
  if  (loggedMsgs == NULL)
  {
    loggedMsgs = new KKB::MsgQueue (_threadName);
    weOwnLoggedMsgs = true;
  }

  KKStr logFileDir = osAddSlash (LarcosVariables::LoggingDir ()) + "LarcosCounter";
  osCreateDirectoryPath (logFileDir);
  DateTime now = osGetLocalDateTime ();
  logFileName = osAddSlash (logFileDir) + "LarcosCounter_" + now.Date ().YYYYMMDD () + "-" + now.Time ().HHMMSS () + ".txt";
  logFile = new ofstream (logFileName.Str ());

  *logFile << "LoggerThread::LoggerThread    Logger File Created" << endl;
}



LoggerThread::~LoggerThread ()
{
  ProcessMsgQueue ();

  if  ((Status () == KKThread::tsStarting)  ||  (Status () == KKThread::tsRunning))
  {
    TerminateThread ();
    osSleepMiliSecs (500);
  }

  if  (weOwnLoggedMsgs)
  {
    delete  loggedMsgs;
    loggedMsgs = NULL;
  }

  *logFile << "LoggerThread::~LoggerThread" << endl;

  delete  logFile;
  logFile = NULL;
}



kkint32  LoggerThread::MemoryConsumedEstimated ()
{
  return  sizeof (LoggerThread);
}



KKStrListPtr  LoggerThread::GetAllLoggedMsgs ()
{
  if  (loggedMsgs)
    return loggedMsgs->GetAllMsgs ();
  else
    return NULL;
}


KKStrPtr  LoggerThread::GetNextLoggedMsg ()
{
  if  (loggedMsgs)
    return loggedMsgs->GetNextMsg ();
  else
    return NULL;
}


void  LoggerThread::ProcessMsgQueue ()
{
  KKStrListPtr msgs = MsgQueue ()->GetAllMsgs ();
  if  (msgs == NULL)
    return;

  KKStrList::iterator idx;
  for (idx = msgs->begin ();  idx != msgs->end ();  ++idx)
  {
    KKStrPtr  msg = *idx;
    (*logFile) << (*msg) << endl;
  }

  loggedMsgs->AddMsgs (msgs, true);
  msgs->Owner (false);
  delete  msgs;
  msgs = NULL;
}  /* ProcessMsgQueue */



void  LoggerThread::Run ()
{
  Status (KKThread::tsRunning);

  *logFile << "LoggerThread::Run" << endl;

  //KKB::DateTime  timeStart = KKB::osGetLocalDateTime ();
  //KKB::DateTime  timeNow   = timeStart;

  while  ((!TerminateFlag ())  &&  (!ShutdownFlag ()))
  {
    ProcessMsgQueue ();
    osSleepMiliSecs (200);
  }

  *logFile << "LoggerThread::Run   Exiting" << endl;
  Status (tsStopping);
  return;
}  /* Run */




LoggerThreadPtr  LoggerThread::CreateAndStartInstance (MsgQueuePtr  _msgQueue,
                                                       MsgQueuePtr  _loggedMsgs,
                                                       const KKStr& _threadName
                                                      )
{
  bool  successful = false;
  LoggerThreadPtr  logger = new LoggerThread (_msgQueue, _loggedMsgs, _threadName);
  logger->Start (KKThread::tpLow, successful);
  return logger;
}
                           
