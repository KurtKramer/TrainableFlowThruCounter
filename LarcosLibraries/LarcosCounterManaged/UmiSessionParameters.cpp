#include "StdAfx.h"
#include "FirstIncludes.h"

#include <stdio.h>
#include <ctype.h>
#include <vector>
#include <sstream>
#include "MemoryDebug.h"
using namespace std;

#include "KKBaseTypes.h"
using namespace KKB;


#include "ScannerFile.h"
using namespace  KKLSC;


#include "SessionParameters.h"
using namespace  LarcosCounterUnManaged;



#include "UmiKKStr.h"
#include "UmiMethods.h"
#include "UmiSessionParameters.h"
#include "UmiScannerFile.h"
using namespace LarcosCounterManaged;


#include <msclr\marshal_cppstd.h>

using namespace System;


UmiSessionParameters::UmiSessionParameters  ():
   sessionParameters (new SessionParameters ())
{
}


   
UmiSessionParameters::UmiSessionParameters  (UmiSessionParameters^  _parms):
   sessionParameters (NULL)
{
  if  (_parms == nullptr)
    sessionParameters = new SessionParameters ();

  else if  (_parms->sessionParameters == NULL)
    sessionParameters = new SessionParameters ();

  else
    sessionParameters = new SessionParameters (*(_parms->sessionParameters));
}



UmiSessionParameters::UmiSessionParameters  (SessionParametersPtr  _parms):
  sessionParameters (_parms)
{
}



UmiSessionParameters::UmiSessionParameters  (const SessionParameters&  _parms):
  sessionParameters (new SessionParameters (_parms))
{
}

UmiSessionParameters::~UmiSessionParameters ()
{
  this->!UmiSessionParameters ();
}



UmiSessionParameters::!UmiSessionParameters ()
{
  delete  sessionParameters;
  sessionParameters = NULL;
}



void  UmiSessionParameters::Assign (UmiSessionParameters^  parms)
{
  sessionParameters->Assign (*(parms->UnManagedClass ()));
}




int  UmiSessionParameters::SessionId::get () {return sessionParameters->SessionId ();}

String^           UmiSessionParameters::Comments::get           () {return UmiKKStr::KKStrToSystenStr  (sessionParameters->Comments           ());}
System::DateTime  UmiSessionParameters::DateTimeRecorded::get   () {return UmiMethods::DateTimeKKUtoSystem (sessionParameters->DateTimeRecorded ());}
String^           UmiSessionParameters::Destination::get        () {return UmiKKStr::KKStrToSystenStr  (sessionParameters->Destination        ());}
String^           UmiSessionParameters::PersonInCharge::get     () {return UmiKKStr::KKStrToSystenStr  (sessionParameters->PersonInCharge     ());}
String^           UmiSessionParameters::ControlNum::get         () {return UmiKKStr::KKStrToSystenStr  (sessionParameters->ControlNum         ());}
String^           UmiSessionParameters::SessionDescription::get () {return UmiKKStr::KKStrToSystenStr  (sessionParameters->SessionDescription ());}
String^           UmiSessionParameters::Source::get             () {return UmiKKStr::KKStrToSystenStr  (sessionParameters->Source             ());}
String^           UmiSessionParameters::TrainingModelName::get  () {return UmiKKStr::KKStrToSystenStr  (sessionParameters->TrainingModelName  ());}




void  UmiSessionParameters::SessionId::set (int _sessionId) {sessionParameters->SessionId (_sessionId);}

void  UmiSessionParameters::Comments::set           (String^   _comments)           {sessionParameters->Comments           (UmiKKStr::SystemStringToKKStr   (_comments));}
void  UmiSessionParameters::DateTimeRecorded::set   (DateTime  _dateTimeRecorded)   {sessionParameters->DateTimeRecorded   (UmiMethods::DateTimeSystemToKKU (_dateTimeRecorded));}
void  UmiSessionParameters::Destination::set        (String^   _destination)        {sessionParameters->Destination        (UmiKKStr::SystemStringToKKStr   (_destination));}
void  UmiSessionParameters::PersonInCharge::set     (String^   _personInCharge)     {sessionParameters->PersonInCharge     (UmiKKStr::SystemStringToKKStr   (_personInCharge));}
void  UmiSessionParameters::ControlNum::set         (String^   _rootName)           {sessionParameters->ControlNum         (UmiKKStr::SystemStringToKKStr   (_rootName));}
void  UmiSessionParameters::SessionDescription::set (String^   _sessionDescription) {sessionParameters->SessionDescription (UmiKKStr::SystemStringToKKStr   (_sessionDescription));}
void  UmiSessionParameters::Source::set             (String^   _source)             {sessionParameters->Source             (UmiKKStr::SystemStringToKKStr   (_source));}
void  UmiSessionParameters::TrainingModelName::set  (String^   _trainingModelName)  {sessionParameters->TrainingModelName  (UmiKKStr::SystemStringToKKStr   (_trainingModelName));}




void  UmiSessionParameters::UpdateFromDataField (String^  fieldName,
                                                 String^  fieldValue,
                                                 bool%    fieldFound
                                                )
{
  KKStr  fn = UmiKKStr::SystemStringToKKStr (fieldName);
  KKStr  fv = UmiKKStr::SystemStringToKKStr (fieldValue);
  bool  ff = false;
  sessionParameters->UpdateFromDataField (fn, fv, ff);
  fieldFound = ff;
}



void  UmiSessionParameters::UpdateFromScannerFile (UmiScannerFile^ sf)
{
  if  (sf == nullptr)
    return;

  sessionParameters->UpdateFromScannerFile (sf->UnManagedClass ());
}




void  UmiSessionParameters::WriteFieldValues (System::IO::StreamWriter^  sr)
{
  ostringstream o;
  sessionParameters->WriteFieldValues (o);
  String^ s = msclr::interop::marshal_as<String^> (o.str ());
  sr->WriteLine (s);
}



UmiSessionParametersList::UmiSessionParametersList ()
{
}
