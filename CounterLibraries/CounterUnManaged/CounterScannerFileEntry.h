#if  !defined(_LARCOSSCANNERFILEENTRY_)
#define  _LARCOSSCANNERFILEENTRY_


/**
 *@class
 *@brief  A Larcos specific implementation of a ScannerFileEntry
 *@details  CReated this class to contain not only ScannerFile data but also data that 
 * is relevant to a Larcos ScannerFile such as SessionParaeters and OperatingParameters.
 * As of 2014-06-02 this class is not in use yet.  Initial creation is meant to provoke
 * ideas about what I want in it and how to manage instances of it.
 */


#include  "ScannerFileEntry.h"



namespace CounterUnManaged
{
#if  !defined(_SessionParameters_Defined_)
  class  SessionParameters;
  typedef  SessionParameters*  SessionParametersPtr;
#endif


#if  !defined(_OperatingParameters_Defined_)
  class  OperatingParameters;
  typedef  OperatingParameters*  OperatingParametersPtr;
#endif


  class CounterScannerFileEntry:  public ScannerFileEntry
  {
  public:
    CounterScannerFileEntry ();

    virtual ~CounterScannerFileEntry ();

  private:
    ScannerFileEntryPtr     scannerFileEntry;
    SessionParametersPtr    sessionParameters;
    OperatingParametersPtr  operatingParameters;
  };  /* CounterScannerFileEntry */




}  /* CounterUnManaged */



#endif