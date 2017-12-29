#if  !defined(_UmiScannerHeaderFields_)
#define  _UmiScannerHeaderFields_


using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Diagnostics;
using namespace System::Collections::Generic;
using namespace System::Threading;

#include "ScannerHeaderFields.h"
using  namespace KKLSC;

#include "UmiRunLog.h"


namespace LarcosCounterManaged 
{
  public  ref  class  UmiScannerHeaderFields
  {
  public:
    UmiScannerHeaderFields ();

    UmiScannerHeaderFields (ScannerHeaderFieldsPtr  fields);  /**< Will take ownership of 'fields' */

    !UmiScannerHeaderFields ();

    ~UmiScannerHeaderFields ();

    kkMemSize  MemoryConsumedEstimated ();

    void  Add (UmiScannerHeaderFields^  fields);

    void  Add (String^ fieldName,
               String^  fieldValue
              );

    void  Add (String^  fieldName,
               bool     fieldValue
              );

    void  Add (String^  fieldName,
               kkint32  fieldValue
              );

    void  Add (String^  fieldName,
               kkint64  fieldValue
              );

    void  Add (String^  fieldName,
               double   fieldValue
              );

    void  Clear ();  /**< Erases contents */

    bool  FieldExists (String^  fieldName);

    String^  GetValue (String^  fieldName);

    ScannerHeaderFieldsPtr  Unmanaged ()  {return scannerHeaderFields;}

  private:
    ScannerHeaderFieldsPtr  scannerHeaderFields;
  };  /* UmiScannerHeaderFields */
}  /* LarcosCounterManaged */
#endif
