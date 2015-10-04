#include "StdAfx.h"

#include "FirstIncludes.h"
#include <stdio.h>
#include <math.h>
#include <ctype.h>
#include <time.h>
#include <fstream>
#include <iostream>
#include <ostream>
#include <string>
#include <vector>
#include "MemoryDebug.h"


#include "KKBaseTypes.h"
using namespace  KKB;

#include "LarcosVariables.h"
using namespace  LarcosBase;


using namespace  System::Diagnostics;


#include "UmiOSservices.h"
#include "UmiKKStr.h"
#include "UmiVariables.h"

#include "UmiRunLog.h"



using namespace  LarcosCounterManaged;



UmiRunLog::UmiRunLog ():
  msgQueue    (nullptr),
  runLog      (NULL),
  weOwnRunLog (false)
{
  runLog = new RunLog ();
  runLog->SetLevel (LarcosVariables::DebugLevel ());
  weOwnRunLog = true;
}



UmiRunLog::UmiRunLog (String^  logFileName):
  msgQueue    (nullptr),
  runLog      (NULL),
  weOwnRunLog (false)
{
  KKStr s = UmiKKStr::SystemStringToKKStr (logFileName);
  runLog = new RunLog (s.Str ());
  runLog->SetLevel (LarcosVariables::DebugLevel ());
  weOwnRunLog = true;
}



UmiRunLog::UmiRunLog (MsgQueuePtr  _msgQueue):
  msgQueue    (nullptr),
  runLog      (NULL),
  weOwnRunLog (false)
{
  runLog = new RunLog (_msgQueue);
  runLog->SetLevel (LarcosVariables::DebugLevel ());
  weOwnRunLog = true;
}



UmiRunLog::!UmiRunLog ()
{
  Close ();
}


UmiRunLog::~UmiRunLog ()
{
  this->!UmiRunLog ();
}



// Will create a RunLog with a file in the 'UmiVariables::LoggingDir ()' Starting 
// with 'rootName' with the Date and Time appended to the end.
UmiRunLog^  UmiRunLog::CreateRunLog (String^  rootName)
{
  String^  dir = UmiOSservices::AddSlash (UmiVariables::LoggingDir ()) + "RunLogs";
  UmiOSservices::CreateDirectoryPath (dir);

  String^  fileName = UmiOSservices::AddSlash (dir) + rootName + "_RunLog_" + System::DateTime::Now.ToString ("yyyyMMdd-HHmmss") + ".txt";
  return  gcnew  UmiRunLog (fileName);
}  /* CreateRunLog */




void  UmiRunLog::Close ()
{
  if  (weOwnRunLog)
  {
    delete runLog;
    runLog = NULL;
  }
};



void  UmiRunLog::AttachFile (String^  runLogFileName)
{
  KKStr  kkStr = UmiKKStr::SystemStringToKKStr (runLogFileName);
  runLog->AttachFile (kkStr);
}



void  UmiRunLog::AttachFile (const KKStr&  runLogFileName)
{
  runLog->AttachFile (runLogFileName);
}


void  UmiRunLog::AttachUmiMsgQueue ()
{
  if  (msgQueue != nullptr)
    return;
  msgQueue = gcnew UmiMsgQueue ("UmiRunLog");
  runLog->AttachMsgQueue (msgQueue->MsgQueue ());
}



void  UmiRunLog::DetachFile ()
{
  runLog->DetachFile ();
}




String^  UmiRunLog::FileName ()
{
  if  (runLog == NULL)
    return  "";

  return  UmiKKStr::KKStrToSystenStr (runLog->FileName ());
}  /* FileName */



String^  UmiRunLog::LastLine ()
{
  if  (runLog == NULL)
    return  System::String::Empty;

  return  UmiKKStr::KKStrToSystenStr (runLog->LastLine ());
}  /* LastLine */



String^  UmiRunLog::RunLogFileName ()
{
  return  UmiKKStr::KKStrToSystenStr (runLog->FileName ());
}  /* RunLogFileName */



void  UmiRunLog::SetLoggingLevel (int _loggingLevel)
{
 if  (runLog)
   runLog->SetLoggingLevel (_loggingLevel);
}  /* SetLevel */



int  UmiRunLog::LineCount ()
{
  if  (runLog == NULL)
    return 0;
  return  runLog->LineCount ();
}



void  UmiRunLog::Write (int      level,
                        String^  s
                       )
{
  if  (runLog)
  {
    runLog->SetLevel (level);
    runLog->Append (UmiKKStr::SystemStringToKKStr (s).Str ());
  }
}


void  UmiRunLog::WriteLn (int      level,
                          String^  s
                         )
{
  if  (runLog)
  {
    runLog->SetLevel (level);
    runLog->Append (UmiKKStr::SystemStringToKKStr (s).Str ());
    runLog->Append ("\n");
  }
}


void  UmiRunLog::WriteLine (String^  s)
{
  if  (runLog)
    runLog->WriteLine (UmiKKStr::SystemStringToKKStr (s));
}



void  UmiRunLog::DisplayWithNoteBook ()
{
  if  (!runLog)
    throw gcnew Exception ("UmiRunLog::DisplayWithNoteBook    *** runLog not defined ***");

  KKStr  curFileName = runLog->FileName ();
  if  (curFileName.Empty ())
  {
    // Logging was to memory and not DISK
    throw gcnew Exception ("UmiRunLog::DisplayWithNoteBook    *** Logging was not to a Disk File can't display contents ***");
  }

  runLog->DetachFile ();

  Process^  proc = gcnew Process();
  proc->StartInfo->WorkingDirectory = "";
  proc->StartInfo->FileName = "notepad.exe";
  proc->StartInfo->Arguments = UmiKKStr::KKStrToSystenStr (curFileName);
  proc->StartInfo->UseShellExecute = false;
  proc->StartInfo->RedirectStandardOutput = false;
  proc->StartInfo->RedirectStandardError = true;
  proc->Start();
  proc->WaitForExit();
  proc->Close();

  runLog->AttachFileAppend (curFileName);
}  /* DisplayWithNoteBook */



String^  UmiRunLog::GetCopyOfLastMsg ()
{
  if  (msgQueue == nullptr)
    return nullptr;
  else
    return msgQueue->GetCopyOfLastMsg ();
}



List<String^>^  UmiRunLog::GetAllMsgs ()
{
  if  (msgQueue == nullptr)
    return nullptr;
  else
    return msgQueue->GetAllMsgs ();
}


String^  UmiRunLog::GetNextMsg ()
{
  if  (msgQueue == nullptr)
    return nullptr;
  else
    return msgQueue->GetNextMsg ();
}



void  UmiRunLog::Flush ()
{
  if (runLog != NULL)
    runLog->Flush ();
}
