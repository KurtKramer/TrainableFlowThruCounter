#include "StdAfx.h"
#include "FirstIncludes.h"
#include <stdio.h>
#include <math.h>
#include <ctype.h>
#include <time.h>
#include <fstream>
#include <iostream>
#include <map>
#include <ostream>
#include <string>
#include <vector>
#include "MemoryDebug.h"

#include "KKBaseTypes.h"
#include "..\\KKBase\\GoalKeeper.h"
#include "OSservices.h"
using namespace  KKB;


#include "UmiOSservices.h"
#include "UmiKKStr.h"
#include "UmiScannerHeaderFields.h"
using namespace  LarcosCounterManaged;

UmiScannerHeaderFields::UmiScannerHeaderFields ():
  scannerHeaderFields (new ScannerHeaderFields ())
{
}


UmiScannerHeaderFields::UmiScannerHeaderFields (ScannerHeaderFieldsPtr  fields):
   scannerHeaderFields (fields)
{
}



UmiScannerHeaderFields::!UmiScannerHeaderFields ()
{
  delete  scannerHeaderFields;
  scannerHeaderFields = NULL;
}

UmiScannerHeaderFields::~UmiScannerHeaderFields ()
{
  this->!UmiScannerHeaderFields ();
}


kkint32  UmiScannerHeaderFields::MemoryConsumedEstimated ()
{
  return  scannerHeaderFields->MemoryConsumedEstimated ();
}



void  UmiScannerHeaderFields::Add (UmiScannerHeaderFields^  fields)
{
  scannerHeaderFields->Add (fields->Unmanaged ());
}



void  UmiScannerHeaderFields::Add (String^ fieldName,
                                   String^ fieldValue
                                  )
{
  scannerHeaderFields->Add (UmiKKStr::SystemStringToKKStr (fieldName), UmiKKStr::SystemStringToKKStr (fieldValue));
}



void  UmiScannerHeaderFields::Add (String^  fieldName,
                                   bool     fieldValue
                                  )
{
  scannerHeaderFields->Add (UmiKKStr::SystemStringToKKStr (fieldName), fieldValue);
}



void UmiScannerHeaderFields::Add (String^  fieldName,
                                  kkint32  fieldValue
                                 )
{
  scannerHeaderFields->Add (UmiKKStr::SystemStringToKKStr (fieldName), fieldValue);
}



void  UmiScannerHeaderFields::Add (String^  fieldName,
                                   kkint64  fieldValue
                                  )
{
  scannerHeaderFields->Add (UmiKKStr::SystemStringToKKStr (fieldName), fieldValue);
}


void  UmiScannerHeaderFields::Add (String^  fieldName,
                                   double   fieldValue
                                  )
{
  scannerHeaderFields->Add (UmiKKStr::SystemStringToKKStr (fieldName), fieldValue);
}


void  UmiScannerHeaderFields::Clear ()
{
  scannerHeaderFields->Clear ();
}



bool  UmiScannerHeaderFields::FieldExists (String^  fieldName)
{
  return  scannerHeaderFields->FieldExists (UmiKKStr::SystemStringToKKStr (fieldName));
}



String^  UmiScannerHeaderFields::GetValue (String^  fieldName)
{
  return  UmiKKStr::KKStrToSystenStr (scannerHeaderFields->GetValue (UmiKKStr::SystemStringToKKStr (fieldName)));
}

