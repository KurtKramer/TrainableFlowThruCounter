#include "StdAfx.h"
#include "FirstIncludes.h"

#include <stdio.h>
#include <math.h>


#include  <ctype.h>
#include  <time.h>

#include "StdAfx.h"
#include "FirstIncludes.h"

#include <fstream>
#include <iostream>
#include <ostream>
#include <string>
#include <vector>
#include "MemoryDebug.h"
using namespace std;

#include  "KKBaseTypes.h"

using namespace System;
using namespace KKB;


#include "UmiOSservices.h"
#include "UmiKKStr.h"

using namespace  LarcosCounterManaged;

UmiKKStr::UmiKKStr ()
{
}



KKStr  UmiKKStr::SystemStringToKKStr (System::String^  s)
{
  if  (s == nullptr)
    return KKStr::EmptyStr ();

  KKB::KKStr  kkStr (s->Length);
  for  (int x = 0;  x < s->Length;  x++)
    kkStr.Append ((char)s[x]);
  return  kkStr;
}


KKStrPtr  UmiKKStr::SystemStringToKKStrPtr (System::String^  s)
{
  if  (s == nullptr)
    return new KKB::KKStr ();

  KKB::KKStrPtr  kkStr = new KKStr (s->Length);
  for  (int x = 0;  x < s->Length;  x++)
    kkStr->Append ((char)s[x]);
  return  kkStr;
}




System::String^  UmiKKStr::KKStrToSystenStr (const KKStr&  s)
{
  if  (s.Str () == NULL)
    return gcnew String ("");
  System::String^  sysStr = gcnew System::String (s.Str ());
  return  sysStr;
}



array<String^>^  UmiKKStr::KKStrToListToStringArray (KKStrListPtr list)
{
  if  (list == NULL)
    return nullptr;

  array<String^>^  managedStrings = gcnew array<String^> (list->QueueSize ());
  KKStrList::const_iterator idx;

  int x = 0;
  for  (idx = list->begin ();  idx != list->end ();  ++idx, ++x)
    managedStrings[x] = KKStrToSystenStr (**idx);

  return  managedStrings;
}




Byte  UmiKKStr::StrToByte (String^  s)
{
  Byte i = 0;
  try  {i = System::Byte::Parse (s);}
  catch (Exception^){i = 0;}
  return  i;
}  /* StrToByte */



char*  UmiKKStr::StrToCharStar (String^ s)
{
  if  (s == nullptr)
    return _strdup ("");

  int len = s->Length;
  char*  result = new char[len + 1];
  for  (int x = 0;  x < len;  x++)
    result[x] = (char)s[x];
  result[len] = 0;
  return  result;
}  /* StrToCharStar*/




String^  StripOutCommas (String^ s)
{
  if  (s == nullptr)
    return  String::Empty;

  int  zed = s->IndexOf (',');
  while  (zed >= 0)
  {
    s = s->Remove (zed, 1);
    zed = s->IndexOf (',');
  }

  return  s;
}  /* StripOutCommas */




    
kkuint32  UmiKKStr::StrToUint (String^  s)
{
  kkuint32 i = 0;
  try {i = (kkuint32)System::UInt32::Parse (StripOutCommas (s));}
  catch  (Exception^) {i = 0;}
  return  i;
}  /* StrToUint */


int   UmiKKStr::StrToInt (String^  s)
{
  int i = 0;

  try {i = int::Parse (StripOutCommas (s));}
  catch  (Exception^) {i = 0;}
  return  i;
}  /* StrToInt */



double  UmiKKStr::StrToLatitude  (String^ s)
{
  KKStr kkStr = UmiKKStr::SystemStringToKKStr (s);
  return  kkStr.ToLatitude ();
}


double  UmiKKStr::StrToLongitude (String^ s)
{
  KKStr kkStr = UmiKKStr::SystemStringToKKStr (s);
  return  kkStr.ToLongitude ();
}



long  UmiKKStr::StrToLong (String^ s)
{
  long i = 0;
  try  {i = long::Parse(StripOutCommas (s)); }
  catch (Exception^) { i = 0; }
  return i;
}  /* StrToInt */



bool  UmiKKStr::StrToBool (String^ s)
{
  String^ u = s->ToUpper ();
  if  ((u == "T")  ||  (u == "TRUE")  ||  (u == "Y")  ||  (u == "YES")  ||  (u =="ON") ||  (u == "1"))
    return true;
  else
    return false;
}  /* StrToBool */



float  UmiKKStr::StrToFloat (String^ s)
{
  bool  percentage = false;
  int  perectIndex = s->LastIndexOf ("%");
  if  (perectIndex >= 0)
  {
    percentage = true;
    s = s->Remove (perectIndex, 1);
  }  

  float  f = 0.0f;
  try  {f = float::Parse (StripOutCommas (s)); }
  catch (Exception^) {f = 0.0f;}

  if  (percentage)
    f = f / 100.0f;
  return f ;
}  /* StrToFloat */



double  UmiKKStr::StrToDouble (String^ s)
{
  double v = 0.0;
  try {v = double::Parse (StripOutCommas (s));}
  catch (Exception^){v = 0.0;}
  return v;
}  /* StrToDouble */




System::DateTime  UmiKKStr::StrToDateTime (String^ s)
{
  System::DateTime t = System::DateTime::Now;
  try {t = System::DateTime::Parse (s);}
  catch (Exception^)
  {
    KKStr  ds = UmiKKStr::SystemStringToKKStr (s);
    KKB::DateTime  dt (ds);
    t = LarcosCounterManaged::UmiOSservices::KKuDateTimeToSystemDateTime (dt);
  }
  return t;
}  /* StrToDateTime */





System::String^  UmiKKStr::StrToQuotedStr (String^ s)
{
  KKStr  kkS = SystemStringToKKStr (s);
  KKStr  quotedStr = kkS.QuotedStr ();
  return KKStrToSystenStr (quotedStr);
}




System::String^  UmiKKStr::StrFromQuotedStr (String^ s)
{
  KKStr  kkS = SystemStringToKKStr (s);
  KKStr  unQuotedStr = kkS.ExtractQuotedStr ("", true);
  return KKStrToSystenStr (unQuotedStr);
}





KKStr  operator+ (const KKStr&     left,
                  System::String^  right
                 )
{
  KKStr result (left);

  if  (right != nullptr)
  {
    for  (int x = 0;  x < right->Length;  x++)
      result.Append ((char)right[x]);
  }

  return  result;
}  /* operator+ */





KKStr  operator+ (System::String^  left,
                  const KKStr&     right
                 )
{
  if  (left == nullptr)
    return right;

  KKStr result (left->Length + right.Len () + 1);
  for  (int x = 0;  x < left->Length;  x++)
    result.Append ((char)left[x]);
  
  result += right;
  return result;
}  /* operator+ */



KKStr&  LarcosCounterManaged::operator<< (KKStr&           left,
                                        System::String^  right
                                       )
{
  if  (right != nullptr)
  {
    for  (int x = 0;  x < right->Length;  x++)
      left.Append ((char)right[x]);
  }

  return  left;
}  /* operator<< */





String^  LarcosCounterManaged::operator<< (String^  left,
                                         KKStr&   right
                                        )
{
  left = left + UmiKKStr::KKStrToSystenStr (right);
  return left;
}




String^  UmiKKStr::LatitudeToStr  (double latitude)
{
  bool  north = (latitude >= 0.0);
  latitude = fabs (latitude);

  while  (latitude > 90)
  {
    latitude = 180 - latitude;
    if  (latitude < 0.0)
    {
      latitude = fabs (latitude);
      north = !north;
    }
  }

  double  latitudeDegrees = 0.0;
  double  latitudeMinutes = modf (latitude, &latitudeDegrees) * 60.0;

  System::String^  latStr  = latitudeDegrees.ToString  ("#0")      + 
                             gcnew String (":")                    +
                             latitudeMinutes.ToString  ("00.0000") +
                             gcnew String ((north ? "N" : "S"));

  return  latStr;
}  /* LatitudeToStr */



String^  UmiKKStr::LongitudeToStr  (double longitude)
{
  bool  east = longitude >= 0.0;
  longitude = fabs (longitude);
  while  (longitude > 180.0)
  {
    longitude = longitude - 360.0;
    if  (longitude < 0.0)
    {
      longitude = fabs (longitude);
      east = !east;
    }
  }

  double  longitudeDegrees = 0.0;
  double  longitudeMinutes = modf (longitude, &longitudeDegrees) * 60.0;

  System::String^  longStr = longitudeDegrees.ToString ("##0") + 
                             gcnew String (":")                       + 
                             longitudeMinutes.ToString ("00.0000") + 
                             gcnew String (east  ? "E" : "W");

  return  longStr;
}  /* LongitudeToStr */


