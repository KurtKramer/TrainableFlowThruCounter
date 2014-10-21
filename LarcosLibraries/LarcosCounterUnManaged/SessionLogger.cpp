/* SessionLogger.cpp -- Represents a Snapshot of stats being maintained by the Larcos Camera Manager.
 * Copyright (C) 2011-2013  Kurt Kramer
 * For conditions of distribution and use, see copyright notice in LarcosCounterUnManaged.txt
 */
#include "FirstIncludes.h"
#include <errno.h>
#include <fstream>
#include <istream>
#include <iostream>
#include "MemoryDebug.h"
using namespace std;

#include "GlobalGoalKeeper.h"
#include "KKBaseTypes.h"
#include "OSservices.h"
using namespace KKB;

#include "LarcosVariables.h"
using namespace  LarcosBase;

#include "SessionLogger.h"
using  namespace  LarcosCounterUnManaged;


KKStr  SessionLogger::lastSessionIdFileName  = KKB::osAddSlash (LarcosVariables::ConfigurationDir ()) + "LastSessionId.txt";
KKStr  SessionLogger::sessionLogFileName     = KKB::osAddSlash (LarcosVariables::LoggingDir       ()) + "SessionLog.txt";


kkint32  SessionLogger::GetNextSessionId ()
{
  GlobalGoalKeeper::StartBlock ();

  kkint32  lastSessionId = GetLastSessionId ();

  kkint32  nextSessionId = lastSessionId + 1;

  ofstream  o (lastSessionIdFileName.Str ());
  if  (o.is_open ())
  {
    o << "// NextSessionId control file." << endl
      << "//"                             << endl
      << "// This file contains only one field that represents the lastSessionId assigned to a ScannerFile."      << endl
      << "// It can only be updated by the \"LarcosCounterUnManaged::SessionLogger::GetNextSessionId\" method in" << endl
      << "// the \"SessionLogger.cpp\" module."                                                                   << endl
      << endl
      << "LastSessionId" << "\t" << nextSessionId << endl
      << "EndOfFile" << endl;

    o.close ();
  }

  GlobalGoalKeeper::EndBlock ();

  return  nextSessionId;
}  /* GetNextSessionId */



kkint32  SessionLogger::GetLastSessionId ()
{
  GlobalGoalKeeper::StartBlock ();

  int  lastSessionId = 0;

  ifstream i (lastSessionIdFileName.Str ());
  if  (i.is_open ())
  {
    bool eof = false;

    KKStr  l =  osReadRestOfLine (i, eof);
    while  (!eof)
    {
      if  (l.StartsWith ("//"))
      {
        // We will ignore coment line.
      }
      else
      {
        KKStr  fieldName = l.ExtractToken2 ("\n\r\t");
        if  (fieldName.EqualIgnoreCase ("LastSessionId"))
          lastSessionId = l.ExtractTokenInt ("\n\r\t");
      }

      l =  osReadRestOfLine (i, eof);
    }

    i.close ();
  }

  GlobalGoalKeeper::EndBlock ();

  return  lastSessionId;
}




void  SessionLogger::AddSessionEntry (SessionParametersPtr    sessionParameters,
                                      OperatingParametersPtr  operatorParameters
                                     )
{
  GlobalGoalKeeper::StartBlock ();

  ofstream o (sessionLogFileName.Str (), ios_base::ate);

  o << "<SessionLogEntry>" << endl;
  sessionParameters->WriteXML (o);
  operatorParameters->WriteXML (o);
  o << "</SessionLogEntry>" << endl;

  o.close ();

  GlobalGoalKeeper::EndBlock ();
}  /* AddSessionEntry */




