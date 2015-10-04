#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;

#include "DateTime.h"
#include "KKStr.h"

using  namespace  KKB;


namespace LarcosCounterManaged 
{
  public  ref class UmiKKStr
  {
  public:
    UmiKKStr ();

    static
      KKStr  SystemStringToKKStr (System::String^  s);

    static
      KKStrPtr  SystemStringToKKStrPtr (System::String^  s);
  
    static
      System::String^  KKStrToSystenStr (const KKStr&  s);

    static
      array<String^>^  KKStrToListToStringArray (KKStrListPtr list);


    static System::Byte      StrToByte        (String^ s);
    static bool              StrToBool        (String^ s);
    static char*             StrToCharStar    (String^ s);
    static System::DateTime  StrToDateTime    (String^ s);
    static double            StrToDouble      (String^ s);
    static float             StrToFloat       (String^ s);
    static int               StrToInt         (String^ s);
    static double            StrToLatitude    (String^ s);
    static long              StrToLong        (String^ s);
    static double            StrToLongitude   (String^ s);
    static kkuint32          StrToUint        (String^ s);
    static String^           StrToQuotedStr   (String^ s);
    static String^           StrFromQuotedStr (String^ s);

    static String^           LatitudeToStr  (double latitude);
    static String^           LongitudeToStr (double longitude);
  };


  KKStr&  operator<< (KKStr&           left,
                      System::String^  right
                     );


  String^ operator<< (String^  left,
                      KKStr&   right
                     );
}  /* LarcosCounterManaged */
