#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Collections::Generic;
using namespace System::Diagnostics;

#include "MsgQueue.h"
using namespace  KKB;


namespace LarcosCounterManaged 
{

  public  ref class UmiMsgQueue
  {
  public:
    UmiMsgQueue (String^  logFileName);

    !UmiMsgQueue ();
    ~UmiMsgQueue ();


    MsgQueuePtr  MsgQueue ()  {return msgQueue;}

    /**
     *@brief  Take ownership of 'msg' and add to end of the queue.
     *@param[in]  msg  Pointer to message that is to be added to end of queue.  The caller will 
     *            pass ownership of this string to this instance of MsgQueue.
     */
    void  AddMsg (String^  msg);     /**<  Taking ownership of 'msg'        */


      
    /** 
     *@brief Returns a duplicate of the last string added to the message queue.
     *@details This will not effect the current copy of the message queue.  he returned string
     * will be owned by the caller who will be responsible for deleting it.
     */
    String^  GetCopyOfLastMsg ();



    /**
     *@brief  Returns all messages that are currently in the queue.
     */
    List<String^>^  GetAllMsgs ();



    /**
     *@brief  Removes from the queue the oldest message added to the queue that has not been removed.
     *@details The caller will get ownership of the string and be responsible for deleting it.
     */
    String^  GetNextMsg ();

  private:
      MsgQueuePtr  msgQueue;
  };

}  /* namespace  LarcosCounterManaged */

//	public ref class TrainingModel :  public System::ComponentModel::Component