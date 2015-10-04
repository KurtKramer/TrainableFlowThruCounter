#include "StdAfx.h"
#include "FirstIncludes.h"

#include <stdio.h>
#include <ctype.h>
#include <sstream>
#include <vector>
#include "MemoryDebug.h"


#include "KKBaseTypes.h"
using namespace KKB;


#include "ScannerFile.h"
using namespace  KKLSC;


#include "OperatingParameters.h"
using namespace  LarcosCounterUnManaged;



#include "UmiKKStr.h"
#include "UmiOperatingParameters.h"
#include "UmiScannerFile.h"
using namespace LarcosCounterManaged;

#include <msclr\marshal_cppstd.h>


using namespace System;



UmiOperatingParameters::UmiOperatingParameters  ():
  operatingParameters (new OperatingParameters ())
{
}



UmiOperatingParameters::UmiOperatingParameters  (UmiOperatingParameters^  _parms):
  operatingParameters (NULL)
{
  operatingParameters = new OperatingParameters (*(_parms->operatingParameters));
}




UmiOperatingParameters::UmiOperatingParameters  (OperatingParametersPtr  _parms):
  operatingParameters (_parms)
{
}



UmiOperatingParameters::UmiOperatingParameters  (const OperatingParameters&  _parms):
  operatingParameters (new OperatingParameters (_parms))
{
}


  
UmiOperatingParameters::~UmiOperatingParameters ()
{
  this->!UmiOperatingParameters ();
}



UmiOperatingParameters::!UmiOperatingParameters ()
{
  delete  operatingParameters;
  operatingParameters = NULL;
}


void  UmiOperatingParameters::Assign (UmiOperatingParameters^  parms)
{
  operatingParameters->Assign (*(parms->UnManagedClass ()));
}



UmiScannerFile::Format  UmiOperatingParameters::DestScannerFileFormat::get ()
{
  return  (UmiScannerFile::Format)operatingParameters->DestScannerFileFormat ();
}



String^   UmiOperatingParameters::DestScannerFileFormatStr::get ()
{
  return  UmiKKStr::KKStrToSystenStr (operatingParameters->DestScannerFileFormatStr ());
}



void  UmiOperatingParameters::DestScannerFileFormat::set (UmiScannerFile::Format  _destScannerFileFormat)
{
  operatingParameters->DestScannerFileFormat ((ScannerFile::Format)_destScannerFileFormat);
}




String^  UmiOperatingParameters::RequestedSensitivityMode::get ()
{
  return  UmiKKStr::KKStrToSystenStr (operatingParameters->RequestedSensitivityMode ());
}



void  UmiOperatingParameters::RequestedSensitivityMode::set  (String^ _requestedSensitivityMode)
{
  operatingParameters->RequestedSensitivityMode (UmiKKStr::SystemStringToKKStr (_requestedSensitivityMode));
}



void  UmiOperatingParameters::UpdateFromDataField (String^  fieldName,
                                                   String^  fieldValue,
                                                   bool%    fieldFound
                                                  )
{
  KKStr  fn = UmiKKStr::SystemStringToKKStr (fieldName);
  KKStr  fv = UmiKKStr::SystemStringToKKStr (fieldValue);
  bool  ff = false;
  operatingParameters->UpdateFromDataField (fn, fv, ff);
  fieldFound = ff;
}



void  UmiOperatingParameters::UpdateFromScannerFile (UmiScannerFile^ sf)
{
  if  (sf == nullptr)
    return;

  operatingParameters->UpdateFromScannerFile (sf->UnManagedClass ());
}




void  UmiOperatingParameters::WriteFieldValues (System::IO::StreamWriter^  sr)
{
  std::ostringstream o;
  operatingParameters->WriteFieldValues (o);
  String^ s = msclr::interop::marshal_as<String^> (o.str ());
  sr->WriteLine (s);
}




UmiOperatingParametersList::UmiOperatingParametersList ()
{
}



