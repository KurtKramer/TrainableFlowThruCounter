#ifndef  _UMICLASSSTATISTIC_
#define  _UMICLASSSTATISTIC_

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Collections::Generic;
using namespace System::Diagnostics;

#include "ClassStatistic.h"
#include "MLClass.h"

#include "UmiClass.h"


namespace LarcosCounterManaged
{
  /*!
   \class  UmiClassStatistic
   \brief  Used as a vehicle to return statistics for a goven class.
   */

  public ref  class  UmiClassStatistic
  {
  public:
    UmiClassStatistic (const ClassStatistic&  classStat);

    UmiClassStatistic (UmiClass^ _mlClass,
                         kkuint32    _count
                        );

    property  String^    ClassName  {String^    get ();}
    property  kkuint32   Count      {kkuint32   get ()  {return  count;}     void  set (kkuint32   _count)    {count   = _count;}}
    property  UmiClass^  MLClass    {UmiClass^  get ()  {return  mlClass;}   void  set (UmiClass^  _mlClass)  {mlClass = _mlClass;}}


  private:
    kkuint32     count;
    UmiClass^  mlClass;
  };




  public  ref  class  UmiClassStatisticList:  public  List<UmiClassStatistic^>
  {
  public:
    UmiClassStatisticList ();

    UmiClassStatisticList (const ClassStatisticList&  stats);

    void  Add (UmiClass^  mlClass,
               kkuint32     count
              );

    void  Add (UmiClassStatisticList^  stats);


    property  kkuint32  ImageCountTotal  {kkuint32  get ();}


    void  SortByClassName ();

  };
}
#endif
