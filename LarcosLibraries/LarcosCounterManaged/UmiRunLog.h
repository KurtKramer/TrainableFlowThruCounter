#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Diagnostics;

#include "RunLog.h"
#include "UmiMsgQueue.h"
using namespace  KKB;


namespace LarcosCounterManaged 
{

  public  ref class UmiRunLog
  {
  public:
    UmiRunLog ();
    UmiRunLog (String^  logFileName);
    UmiRunLog (MsgQueuePtr  _msgQueue);

    !UmiRunLog ();
    ~UmiRunLog ();

    void  AttachFile (String^            runLogFileName);
    void  AttachFile (const KKB::KKStr&  runLogFileName);


    /**
     *@brief  Will create an instance of UmiMsgQueue and attach to associated 'RunLog'.
     *@details  You need to call this method to be ale to make use of the methods
     *GetCopyOfLastMsg (),  GetAllMsgs (), and  GetNextMsg ().  This would be a simple way 
     * of creating an instance of UmiRunLog and passing to other objects under different 
     * threads and be able to collect all the message in this one central place.
     */
    void  AttachUmiMsgQueue ();

    void          Close ();

    void          DetachFile ();

    void          DisplayWithNoteBook ();

    String^       FileName ();

    void          Flush ();


    /** 
     *@brief Returns a duplicate of the Line written to log file;  only if AttachUmiMsgQueue was evoked. 
     *@details This will not effect the current copy of the message queue.  The returned string
     * will be owned by the caller who will be responsible for deleting it.
     */
    String^  GetCopyOfLastMsg ();



    /**
     *@brief  Returns all messages that are currently in the Attached MsgQueue.
     */
    List<String^>^  GetAllMsgs ();



    /**
     *@brief  If there is an attached MsgQueue (AttachUmiMsgQueue was evoked) will return oldest message in MsgQueue.
     */
    String^       GetNextMsg ();


    String^       LastLine ();

    int           LineCount ();

    RunLog&       Log ()  {return *runLog;}

    void          SetLoggingLevel (int _loggingLevel);

    String^       RunLogFileName ();

    void          WriteLine (String^  s);

    void          Write   (int      level,
                           String^  s
                          );

    void          WriteLn (int      level,
                           String^  s
                          );


    static  UmiRunLog^  CreateRunLog (String^  rootName);    /**< Will create a RunLog with a file in the 'StereoLogerTempDirectory' Starting 
                                                              * with 'rootName' with the Date and Time appended to end.
                                                              */

  private:
      RunLogPtr     runLog;
      UmiMsgQueue^  msgQueue;
      bool          weOwnRunLog;
  };

}  /* namespace  LarcosCounterManaged */

//	public ref class TrainingModel :  public System::ComponentModel::Component