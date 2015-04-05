#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Diagnostics;
using namespace System::Collections::Generic;
using namespace System::Threading;

#include "OSservices.h"

#include "ScannerFile.h"
#include "StartStopPoint.h"

#include "UmiRunLog.h"
#include "UmiStartStopPoint.h"
#include "UmiScannerFrame.h"


namespace LarcosCounterManaged 
{
  public ref class UmiScannerFile
  {
  public:
    typedef  array<array<byte>^ >  FrameArray;
    typedef  osFilePos             FilePos;

    enum  class  ScannerFileFormat: int  
    {
      sfSimple          = ScannerFile::sfSimple,
      sf3BitEncoded     = ScannerFile::sf3BitEncoded,
      sf4BitEncoded     = ScannerFile::sf4BitEncoded,
      sfZlib3BitEncoded = ScannerFile::sfZlib3BitEncoded,
      sfUnKnown         = ScannerFile::sfUnKnown
    };

    static
    ScannerFileFormat  ScannerFileFormatFromStr (String^ s);

    static
    String^  ScannerFileFormatToStr (ScannerFileFormat  f);


    ref class  HeaderField
    {
    public:
      HeaderField (String^ _key,
                   String^ _value
                  )
      {key = _key;  value = _value;}

      String^  key;
      String^  value;
    };

    UmiScannerFile (ScannerFilePtr  _scannerFile,
                    UmiRunLog^      _log
                   );  

    
    /** @brief  Creates a new instance 'UmiScannerFile' for an existing Scanner-File. */
    static  
    UmiScannerFile^  CreateScannerFile (String^     _fileName,
                                        UmiRunLog^  _log
                                       );


    static  
    UmiScannerFile^  CreateScannerFileForOutput (String^     _fileName,
                                                 String^     _formatStr,
                                                 UInt32      _pixelsPerScanLine,
                                                 UInt32      _frameHeight,
                                                 UmiRunLog^  _log
                                                );


    // next group of methods are dedicated to the StopStart points in a scanner file.  They are 
    // Managed wrappers of the unmanaged version in "ScannerFile" via the "UmiScannerFile" object. 
    void  AddStartPoint (kkint32  _scanLineNum);

    void  AddStopPoint (kkint32  _scanLineNum);


    void  StartStopPointDelete (kkint32 _scanLineNum);

    UmiStartStopPoint^  StartStopPointNearestEntry (kkint32 _scanLineNum);

    UmiStartStopPoint^  StartStopPointPrevEntry (kkint32 _scanLineNum);

    UmiStartStopPoint^  StartStopPointSuccEntry (kkint32 _scanLineNum);

    UmiStartStopPointList^  StartStopPoints ();

    UmiStartStopRegionList^  StartStopRegions ();

    const StartStopPointList&    StartStopPointsUnManaged ();

    /*
     *@brief  Returns an array indication the record rate in bytes/sec for specified time-intervals.
     *@details  Each element in the returned array will give the average number of bytes recorded for the time
     * the corresponding time interval.  Array element 0 starts at the beginning of the Scanner file and covers 
     * the number of scan lines required to account for 'intervalSecs' seconds.
     */    
    array<float>^  RecordRateByTimeIntervals (int intervalSecs);



  private:
    ~UmiScannerFile ();

  protected:
    !UmiScannerFile ();


  public:
    void  UpdateMemoryPressure ();

    ScannerFilePtr  UnManagedClass ()  {return  scannerFile;}

    property  bool       BuildFrameOffsetsRunning    {bool       get ();}
    property  bool       Eof                         {bool       get ();}
    property  String^    FileFormatStr               {String^    get ();}
    property  String^    FileName                    {String^    get ();}
    property  long long  FileSizeInBytes             {long long  get ();}
    property  FilePos    FrameBufferFileOffsetNext   {long long  get ();}   /**<  Byte offset of next scan line.          */
    property  FilePos    FrameBufferFileOffsetLast   {long long  get ();}   /**<  Byte offset of start of last scan-line. */
    property  bool       FrameIndexBuilt             {bool       get ();}
    property  bool       FrameOffsetsLoaded          {bool       get ();}   /**< Same as 'FrameIndexBuilt'                */
    property  kkuint32   LastScanLine                {kkuint32   get ();}   /**<  Last Scan line read or written.         */
    property  kkint32    LargestKnowmFrameNum        {kkint32    get ();}
    property  kkuint32   LargestKnownScanLine        {kkuint32   get ();}
    property  kkuint32   NextScanLine                {kkuint32   get ();}   /**<  Next scan-line to be read.              */
    property  bool       Opened                      {bool       get ();}
    property  kkint32    PixelsPerScanLine           {kkint32    get ();}
    property  kkint32    ScanLinesPerFrame           {kkint32    get ();}
    property  float      ScanRate                    {float      get ();}


    void  AddHeaderField (String^  _fieldName,
                          String^  _fieldValue
                         );

    void  Close ();


    FrameArray^  CreateRatser (kkuint32  scanLineStart,
                               kkuint32  numScanLines
                              );
   
    static
    array<String^>^  FileFormatOptions ();              /**<  Returns back an array of strings that lists the available Scanner File Formats supported. */

    static
    bool  FileFormatStrValid (String^  _formatStr);

    void  FrameBytesOffsetsUpdateThreadStart ();        /**<  This method starts the thread that updates the 'frameOffsets' table in the
                                                         * underlying ScannerFile instance.
                                                         */

    void  FrameBytesOffsetsUpdateThreadTerminate ();    /**<  Call this method to terminate the 'updateFrameByteOffsetsThread'.   */


    /**
     *@brief  Call this method to Load the FrameOffsets and StartStop points from the index file.
     *@details  This method is also called from 'BuildFrameOffsets'. DOnot call this method if you have also called
     * 'FrameBytesOffsetsUpdateThreadStart'.
     */
    void  LoadIndexFile (bool%       successful,
                         UmiRunLog^  runLog
                        );


    void  FrameRead (kkint32          frameNum,
                     UmiScannerFrame^ frame,
                     bool%            found
                    );

    String^  GetValue (String^  fieldName);             /**< Returns the value of the header field defined by 'fieldName'. */
    
    array<HeaderField^>^  GetHeaderFields ();

  private:
    void  UpdateFrameByteOffsets ();

    kkint32           memoryPreasureAdded;

    bool              frameIndexBuilt;                   /**<  Indicates if 'frameByteOffsets' has been completely built. */
    kkint32           frameNumLastRead;
    kkint32           frameNumNextRead;
    kkint32           largestKnownScanLine;
    UmiRunLog^        log;
    kkint32           scanLinesPerFrame;
    ScannerFilePtr    scannerFile;                       /**<  The underlying unmanaged ScannerFile instance. */
    Thread^           updateFrameByteOffsetsThread;
    bool              updateFrameByteOffsetsRunning;     /**<  Indicates if the thread that updates the frameByesOffsets is running.   */
    bool*             updateFrameByteOffsetsCancelFlag;  /**<  'true' indicates that the frameByesOffsets load thread should terminate. */

    // variables needed to read a single scan line.  We declare a single instance of these variables to avoid the overhead of 
    // dynamically allocating and deallocating them every time we need them.
    kkuint32          pixelsPerScanLine;
    uchar*            lineBuff;
    kkuint32          lineSize;
    kkuint32*          colCount;
    kkuint32          pixesInRow;;
  };  /* UmiScannerFile */
}  /* LarcosCounterManaged */
