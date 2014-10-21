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
using namespace std;

#include "KKBaseTypes.h"
using namespace  System::Diagnostics;
using namespace System;


#include "UmiOSservices.h"
#include "UmiKKStr.h"
#include "UmiVariables.h"

#include "UmiMsgQueue.h"



using namespace  LarcosCounterManaged;



UmiMsgQueue::UmiMsgQueue (String^  logFileName)
{
  KKStr s = UmiKKStr::SystemStringToKKStr (logFileName);
  msgQueue = new KKB::MsgQueue (s);
}



UmiMsgQueue::!UmiMsgQueue ()
{
  delete msgQueue;
  msgQueue = NULL;
}


UmiMsgQueue::~UmiMsgQueue ()
{
  this->!UmiMsgQueue ();
}




void  UmiMsgQueue::AddMsg (String^  msg)
{
  if  (msg == nullptr)
    msg = "UmiMsgQueue::AddMsg    msg == nullptr";

  msgQueue->AddMsg (UmiKKStr::SystemStringToKKStrPtr (msg));
}  /* AddFrame */






String^  UmiMsgQueue::GetNextMsg ()
{
  String^  result = nullptr;

  KKStrPtr  m = msgQueue->GetNextMsg ();
  if  (m == NULL)
    result = nullptr;
  else
    result = UmiKKStr::KKStrToSystenStr (*m);

  delete  m;
  m = NULL;

  return  result;
}  /* GetNextMsg */



List<String^>^  UmiMsgQueue::GetAllMsgs ()
{
  if  (msgQueue == NULL)
    return nullptr;

  List<String^>^  result = gcnew List<String^> ();


  KKStrListPtr  msgs = msgQueue->GetAllMsgs ();
  if  (msgs == NULL)
    return nullptr;

  KKStrList::iterator  idx;
  for  (idx = msgs->begin ();  idx != msgs->end ();  ++idx)
  {
    KKStrPtr  s = *idx;
    result->Add (UmiKKStr::KKStrToSystenStr (*s));
  }

  delete  msgs;
  msgs = NULL;

  return  result;
}  /* GetAllMsgs */




String^  UmiMsgQueue::GetCopyOfLastMsg ()
{
  if  (msgQueue == NULL)
    return  nullptr;

  KKStrPtr  lastMsg = msgQueue->GetCopyOfLastMsg ();
  if  (lastMsg == NULL)
    return  nullptr;

  String^  result = UmiKKStr::KKStrToSystenStr (*lastMsg);
  delete  lastMsg;
  lastMsg = NULL;

  return  result;
}



