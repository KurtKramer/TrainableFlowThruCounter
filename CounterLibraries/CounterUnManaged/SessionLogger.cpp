/* SessionLogger.cpp -- .
 * Copyright (C) 2011-2019  Kurt Kramer
 * For conditions of distribution and use, see copyright notice in CounterUnManaged.txt
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

#include "CounterVariables.h"
using namespace  CounterBase;

#include "SessionLogger.h"
using  namespace  CounterUnManaged;

KKStr  SessionLogger::lastSessionIdFileName  = KKB::osAddSlash (CounterVariables::ConfigurationDir ()) + "LastSessionId.txt";
KKStr  SessionLogger::sessionLogFileName     = KKB::osAddSlash (CounterVariables::LoggingDir       ()) + "SessionLog.txt";



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
      << "// It can only be updated by the \"CounterUnManaged::SessionLogger::GetNextSessionId\" method in" << endl
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

    KKStr  l =  osReadRestOfLine2 (i, eof);
    while  (!eof)
    {
      if  (l.StartsWith ("//"))
      {
        // We will ignore comment line.
      }
      else
      {
        KKStr  fieldName = l.ExtractToken2 ("\n\r\t");
        if  (fieldName.EqualIgnoreCase ("LastSessionId"))
          lastSessionId = l.ExtractTokenInt ("\n\r\t");
      }

      l =  osReadRestOfLine2 (i, eof);
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
