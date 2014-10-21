#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;

#include  "DateTime.h"
#include  "UmiRunLog.h"


namespace LarcosCounterManaged {

  public ref class UmiMethods
  {
  public:
    UmiMethods ();

    static  System::DateTime   DateKKUtoSystem (const  KKB::DateType&  date);

    static  KKB::DateType      DateSystemToKKU (System::DateTime   dt);

    static  System::DateTime   DateTimeKKUtoSystem (const  KKB::DateTime&  date);

    static  KKB::DateTime      DateTimeSystemToKKU (System::DateTime  dt);

    static  kkint32            DebugLevel ();

    static  System::String^    LatitudeLongitudeToString (double latitude,
                                                          double longitude
                                                         );

    static  void               StartMemoryLeakDetection ();

  };  /* UmiMethods */


}  /* LarcosCounterManaged */