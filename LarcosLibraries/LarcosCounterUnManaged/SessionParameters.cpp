/* SessionParameters.cpp -- Represents a Snapshot of stats being maintained by the Larcos Camera Manager.
 * Copyright (C) 2011-2014  Kurt Kramer
 * For conditions of distribution and use, see copyright notice in LarcosCounterUnManaged.txt
 */
#include "FirstIncludes.h"
#include <errno.h>
#include <istream>
#include <iostream>
#include <vector>
#include "MemoryDebug.h"
using namespace std;


#include "KKBaseTypes.h"
#include "OSservices.h"
using namespace KKB;

#include "ScannerHeaderFields.h"
#include "ScannerFile.h"
using namespace  KKLSC;

#include "SessionParameters.h"
using  namespace  LarcosCounterUnManaged;



SessionParameters::SessionParameters ():
     LarcosDataRec ("SP", "SessionParameters"),
     sessionId            (0),
     comments             (),
     controlNum           (),
     dateTimeRecorded     (),
     destination          (),
     destScannerFileName  (),
     hostName             (),
     personInCharge       (),
     sessionDescription   (),
     source               (),
     trainingModelName    ()
{
}



SessionParameters::~SessionParameters ()
{
}



int  SessionParameters::MemoryConsumedEstimated ()  const
{
  int  x = comments.MemoryConsumedEstimated            () + 
           destination.MemoryConsumedEstimated         () +
           destScannerFileName.MemoryConsumedEstimated () +
           hostName.MemoryConsumedEstimated            () +
           personInCharge.MemoryConsumedEstimated      () +         
           controlNum.MemoryConsumedEstimated          () +  
           sessionDescription.MemoryConsumedEstimated  () + 
           source.MemoryConsumedEstimated              () + 
           trainingModelName.MemoryConsumedEstimated   ();

  return  x;
}



void  SessionParameters::Assign (const SessionParameters& stats)
{
  comments            = stats.comments;
  dateTimeRecorded    = stats.dateTimeRecorded;
  destination         = stats.destination;
  destScannerFileName = stats.destScannerFileName;
  hostName            = stats.hostName;
  personInCharge      = stats.personInCharge;
  controlNum          = stats.controlNum;
  sessionDescription  = stats.sessionDescription;
  source              = stats.source;
  trainingModelName   = stats.trainingModelName;
}



const KKStr&  SessionParameters::GetScannerFileValue (ScannerFilePtr  sf, 
                                                      const KKStr&    fieldName
                                                     )
{
  KKStr  fullName = "SP:" + fieldName;
  const KKStr& fv1 = sf->GetValue (fullName);
  if  (!fv1.Empty ())
    return fv1;

  const KKStr&  fv2 = sf->GetValue (fieldName);

  return  fv2;
}




void  SessionParameters::UpdateFromScannerFile (ScannerFilePtr  sf)
{
  if  (sf == NULL)
    return;
  
  const KKStr&  sessionIdStr = GetScannerFileValue (sf, "SessionId");
  if  (!sessionIdStr.Empty ())
    sessionId = sessionIdStr.ToInt32 ();
    
  comments = GetScannerFileValue (sf, "Comments").DecodeQuotedStr ();

  dateTimeRecorded = GetScannerFileValueDateTime (sf, "DateTimeRecorded", dateTimeRecorded);

  destination = GetScannerFileValue (sf, "Destination");
  if  (destination.Empty ())
    destination = GetScannerFileValue (sf, "To");

  destScannerFileName = GetScannerFileValue (sf, "DestScannerFileName");
  hostName            = GetScannerFileValue (sf, "HostName");

  personInCharge = GetScannerFileValue (sf, "PersonInCharge");
  sessionDescription = GetScannerFileValue (sf, "SessionDescription").DecodeQuotedStr ();
  if  (sessionDescription.Empty ())
    sessionDescription  = GetScannerFileValue (sf, "Description").DecodeQuotedStr ();

  controlNum = GetScannerFileValue (sf, "ControlNum");
  if  (controlNum.Empty ())
    controlNum = GetScannerFileValue (sf, "RootName");


  source = GetScannerFileValue (sf, "Source");
  if  (source.Empty ())
    source = GetScannerFileValue (sf, "From");

  trainingModelName   = GetScannerFileValue (sf, "TrainingModelName");
  if  (trainingModelName.Empty ())
    trainingModelName = GetScannerFileValue (sf, "Classifier");

}  /* UpdateFromScannerFile */
 



void  SessionParameters::UpdateFromDataField (const KKStr&  fieldName,
                                              const KKStr&  fieldValue,
                                              bool&         fieldFound
                                             )
{
  fieldFound = false;
  if  (fieldName.EqualIgnoreCase ("Comments"))
  {
    comments = fieldValue.DecodeQuotedStr ();
    fieldFound = true;
  }

  else if  (fieldName.EqualIgnoreCase ("DateTimeRecorded"))
  {
    dateTimeRecorded = DateTime (fieldValue);
    fieldFound = true;
  }

  else if  (fieldName.EqualIgnoreCase ("Destination")  ||  fieldName.EqualIgnoreCase ("To"))
  {
    destination = fieldValue;
    fieldFound = true;
  }

  else if  (fieldName.EqualIgnoreCase ("DestScannerFileName"))
  {
    destScannerFileName = fieldValue;
    fieldFound = true;
  }

  else if  (fieldName.EqualIgnoreCase ("HostName"))
  {
    hostName = fieldValue;
    fieldFound = true;
  }

  else if  (fieldName.EqualIgnoreCase ("PersonInCharge"))
  {
    personInCharge = fieldValue;
    fieldFound = true;
  }

  else if  ((fieldName.EqualIgnoreCase ("ControlNum"))  ||  (fieldName.EqualIgnoreCase ("RootName")))
  {
    controlNum = fieldValue;
    fieldFound = true;
  }

  else if  (fieldName.EqualIgnoreCase ("SessionDescription")  ||  fieldName.EqualIgnoreCase ("Description"))
  {
    sessionDescription = fieldValue.DecodeQuotedStr ();
    fieldFound = true;
  }

  else if  (fieldName.EqualIgnoreCase ("Source")  ||  fieldName.EqualIgnoreCase ("From"))
  {
    source = fieldValue;
    fieldFound = true;
  }

  else if  (fieldName.EqualIgnoreCase ("TrainingModelName"))
  {
    trainingModelName = fieldValue;
    fieldFound = true;
  }
}  /* UpdateFromDataField */



void  SessionParameters::AddToHeaderFields (ScannerHeaderFieldsPtr  headerFields)
{
  headerFields->Add ("SP:SessionId",           sessionId);
  headerFields->Add ("SP:Comments",            comments.QuotedStr ());
  headerFields->Add ("SP:DateTimeRecorded",    dateTimeRecorded);
  headerFields->Add ("SP:Destination",         destination);
  headerFields->Add ("SP:DestScannerFileName", destScannerFileName);
  headerFields->Add ("SP:HostName",            hostName);
  headerFields->Add ("SP:PersonInCharge",      personInCharge);
  headerFields->Add ("SP:ControlNum",          controlNum);
  headerFields->Add ("SP:SessionDescription",  sessionDescription.QuotedStr ());
  headerFields->Add ("SP:Source",              source);
  headerFields->Add ("SP:TrainingModelName",   trainingModelName);
}



void  SessionParameters::WriteFieldValues (ostream&   o)  const
{
  o << "SessionId"           << "\t" << sessionId                       << endl
    << "Comments"            << "\t" << comments.QuotedStr ()           << endl
    << "DateTimeRecorded"    << "\t" << dateTimeRecorded                << endl
    << "Destination"         << "\t" << destination                     << endl
    << "DestScannerFileName" << "\t" << destScannerFileName             << endl
    << "HostName"            << "\t" << hostName                        << endl
    << "PersonInCharge"      << "\t" << personInCharge                  << endl
    << "ControlNum"          << "\t" << controlNum                      << endl
    << "SessionDescription"  << "\t" << sessionDescription.QuotedStr () << endl
    << "Source"              << "\t" << source                          << endl
    << "TrainingModelName"   << "\t" << trainingModelName               << endl;
}



void  SessionParameters::WriteXML (ostream&  o)  const
{
  o << "<SessionParameters>"  << endl;
  WriteFieldValues (o);
  o << "</SessionParameters>" << endl;
}  /* WriteXML */



void  SessionParameters::ReadXML (istream&  i)
{
  bool  eol = false;
  bool  eof = false;
  bool  fieldFound = false;

  KKStrPtr  restOfLine = NULL;
  KKStr  fieldName = osReadNextToken (i, "\t", eof, eol);
  while (!eof)
  {
    if  (!eol)
      restOfLine = osReadRestOfLine (i, eof);
    
    if  (fieldName.EqualIgnoreCase ("</SessionParameters>"))
      break;

    if  (!fieldName.StartsWith ("//"))
      UpdateFromDataField (fieldName, *restOfLine, fieldFound);

    delete  restOfLine;
    restOfLine = NULL;
    fieldName = osReadNextToken (i, "\t", eof, eol);
  }
  delete  restOfLine;
  restOfLine = NULL;
}  /* ReadXML */

