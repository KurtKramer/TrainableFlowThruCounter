/*
 * Copyright (C) 2011-2014  Kurt Kramer
 * For conditions of distribution and use, see copyright notice in LarcosCounterUnManaged.txt
 */


#if  !defined(_SESSIONLOGGER_)
#define  _SESSIONLOGGER_

#include "KKBaseTypes.h"
using namespace  KKB;

#include "OperatingParameters.h"
#include "SessionParameters.h"


namespace LarcosCounterUnManaged
{
  /** @brief  Maintains a log of all recoding sessions */
  class  SessionLogger
  {
  public:
    static  kkint32  GetLastSessionId ();
    static  kkint32  GetNextSessionId ();
    static  void  AddSessionEntry (SessionParametersPtr    sessionParameters,
                                   OperatingParametersPtr  operatorParameters
                                  );

  private:
    static  KKStr  lastSessionIdFileName;
    static  KKStr  sessionLogFileName;
  };  /* SessionLogger */


}  /* LarcosCounterUnManaged */



#endif

