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
using namespace KKB;

#include "CounterVariables.h"
using namespace CounterBase;

#include "UmiMethods.h"
#include "UmiKKStr.h"
using namespace LarcosCounterManaged;


UmiMethods::UmiMethods ()
{
}



System::DateTime   UmiMethods::DateKKUtoSystem (const  KKB::DateType&  date)
{
  return  System::DateTime (date.Year (), date.Month (),  date.Day ());
}


KKB::DateType   UmiMethods::DateSystemToKKU (System::DateTime   dt)
{
  
  return  KKB::DateType (dt.Year, dt.Month, dt.Day);
}



System::DateTime   UmiMethods::DateTimeKKUtoSystem (const  KKB::DateTime&  date)
{
  int  year  = date.Date ().Year  ();
  int  month = date.Date ().Month ();
  int  day   = date.Date ().Day   ();

  int  hours   = date.Time ().Hour   ();
  int  minutes = date.Time ().Minute ();
  int  seconds = date.Time ().Second ();

  bool valid = true;

  month   = Min (12, Max (1, month));
  day     = Min (31, Max (1, day));
  hours   = Min (24, Max (0, hours));
  minutes = Min (60, Max (0, minutes));

  System::DateTime  dt;
  try
  {
    dt = System::DateTime (year, month, day, hours, minutes, seconds);
  }
  catch (Exception^)
  {
    dt = System::DateTime (1900, 1, 1, 0, 0, 0);
  }

  return  dt;
}  /* DateTimeKKUtoSystem */



KKB::DateTime   UmiMethods::DateTimeSystemToKKU (System::DateTime   dt)
{
  
  return  KKB::DateTime (dt.Year, dt.Month, dt.Day, dt.Hour, dt.Minute, dt.Second);
}


kkint32  UmiMethods::DebugLevel ()
{
  return  CounterVariables::DebugLevel ();
}



System::String^   UmiMethods::LatitudeLongitudeToString (double latitude,
                                                         double longitude
                                                        )
{
  // It s assumed that both latitude and longitude are expressed in degrees

  bool  north = (latitude >= 0.0);
  latitude = fabs (latitude);

  while  (longitude > 180.0)
    longitude = longitude - 360.0;

  while  (longitude <= -180.0)
    longitude = longitude + 360;
  bool  west  = (longitude < 0.0);

  longitude = fabs (longitude);

  double  longitudeDegrees = 0.0;
  double  longitudeMinutes = modf (longitude, &longitudeDegrees);

  double  latitudeDegrees = 0.0;
  double  latitudeMinutes = modf (latitude, &latitudeDegrees) * 60.0;

  System::String^  latStr  = latitudeDegrees.ToString  ("00") + ":" + latitudeMinutes.ToString  ("00.0000") + (north ? "N" : "S");
  System::String^  longStr = longitudeDegrees.ToString ("00") + ":" + longitudeMinutes.ToString ("00.0000") + (west  ? "W" : "E");

  return latStr + " " + longStr;
}  /* LatitudeLongitudeToString */



void  UmiMethods::StartMemoryLeakDetection ()
{
  _CrtSetDbgFlag (_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
}