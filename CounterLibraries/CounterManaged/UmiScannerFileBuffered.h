#if  !defined(_UmiScannerFileBuffered_)
#define  _UmiScannerFileBuffered_

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Diagnostics;
using namespace System::Collections::Generic;
using namespace System::Threading;



#include "ScannerFile.h"

#include "UmiRaster.h"
#include "UmiRunLog.h"
#include "UmiScannerFrame.h"
#include "UmiScannerFile.h"


namespace CounterManaged 
{
  #if  !defined(_UmiParticleEntry_)
    ref class  UmiParticleEntry;
    ref class  UmiParticleEntryList;
  #endif


  public  ref  class  UmiScannerFileBuffered
  {
  public:
    UmiScannerFileBuffered (UmiScannerFile^  _scannerFile);
    
    UmiScannerFileBuffered (String^     _fileName,
                           UmiRunLog^  _runLog
                          );

    !UmiScannerFileBuffered ();

    ~UmiScannerFileBuffered ();


    bool                   BuildFrameOffsetsRunning ();
    kkint32                CropColLeft              ()  {return  cropColLeft;}
    kkint32                CropColRight             ()  {return  cropColRight;}
    kkint64                FileSizeInBytes          ()  {return  scannerFile->FileSizeInBytes;}
    int                    LargestKnowmFrameNum     ()  {return  scannerFile->LargestKnowmFrameNum;}
    long                   LargestKnownScanLine     ()  {return  scannerFile->LargestKnownScanLine;}
    UmiParticleEntryList^  ParticleEntries          ()  {return  particleEntries;}
    bool                   ParticleEntriesLoaded    ()  {return  particleEntriesLoaded;}
    kkint32                PixelsPerDisLine         ()  {return  pixelsPerDisLine;}
    UmiScannerFile^        ScannerFile              ()  {return  scannerFile;}

    property  String^  FileName            {String^ get ();}
    property  bool     FrameIndexBuilt     {bool    get ();}
    property  bool     LoadThreadsRunning  {bool    get ();}  /**< Returns true if either FrameOffSets are running or ParticleEntry is running. */
    property  kkint32  PixelsPerScanLine   {kkint32 get ();}
    property  float    Ratio               {float   get ()  {return ratio;};}
    property  kkint32  ScanLinesPerFrame   {kkint32 get ();}
    property  float    ScanRate            {float   get ();}

    array<UmiScannerFile::HeaderField^>^  GetHeaderFields ();

    String^  GetValue (String^  key);

    void  FrameBytesOffsetsUpdateThreadStart ();

    void  ParticleEntryDataLoadThreadStart (UmiRunLog^  runLog);

    void  FrameBytesOffsetsUpdateThreadTerminate ();
    

    /**
     *@details  Ratio will be computed from these parameters;  ratio = (1 + _cropColRight - _cropColLeft) / _pixelsPerDisLine
     *@param[in]  _pixelsPerDisLine  Number of displayable pixels
     *@param[in]  _cropColLeft  Column in Scanner file that 'LeftCrop' will be set at.
     *@param[in]  _cropColRight Column in Scanner file that 'RightCrop' will be set at.
     */
    void  SetPixelsPerDisplayLine (kkint32 _pixelsPerDisLine,
                                   kkint32 _cropColLeft,
                                   kkint32 _cropClRight
                                 );


    array<array<byte>^>^   GetScanLines  (kkint32  scanLineStart,
                                          kkint32  scanLineEnd
                                         );
  
    UmiRaster^  GetImage (UmiParticleEntry^  pe);
  
    UmiParticleEntryList^  GetParticlesForScanLineRange (kkint32  displayRowStart,
                                                         kkint32  displayRowEnd
                                                        );

    void  GetDisplayRows  (kkint32               displayRowStart,
                           kkint32               displayRowEnd,
                           array<array<byte>^>^  raster
                          );

    ///
    /// <summary>
    /// Will populate 'raster' with scan-row data such that each row in raster represents a column in the 
    /// scanner file and each column in 'raster' represents the appropriate 'scannerFile' rows.  This function 
    /// will take into count the crop settings and 'ratio' factor.
    /// </summary>
    /// <param name="displayRowStart">This is the first displayable row to retrieve; the actual scanner file row will be 'displayRowStart' * 'ratio'.</param>
    /// <param name="displayRowEnd">This is the last displayable row to retrieve;  the actual scanner file row will be 'displayRowEnd' * 'ratio'.</param>
    /// <param name="raster">This is the column in the panel that we wish to display; it will be added to 'displayScanRow' to get the relative displayable scan row.</param>
    ///
    void  GetDisplayRowsScanColsByScanRows  (kkint32               displayRowStart,
                                             kkint32               displayRowEnd,
                                             array<array<byte>^>^  raster
                                            );



    ///
    /// <summary>
    /// Will extract a Raster image from the scanner file as it was extracted by the "LogicalFrameProcessor" object;  this 
    /// way we can get the exact same image that was used during the original processing of scanner data.  This includes
    /// the padding by 2 columns and rows of the image;  this is important so that the feature computation routines will 
    /// work correctly.  some of the routines will be tripped up if they fun into foreground pixels that are on the edge 
    /// of the image.
    /// </summary>
    /// <param name="pe">Particle entry we are to create a raster for.</param>
    ///
    UmiRaster^  GetRasterForParticle (UmiParticleEntry^ pe);


    ///
    /// <summary>
    /// Will extract a Raster image from the scanner file as it would have been extracted during the counting session.
    /// </summary>
    /// <param name="pe">Particle entry we are to create a raster for.</param>
    /// <param name="operatingParameters">Parameters used to extract image with; number erosions etc....</param>
    ///
    UmiRaster^  GetRasterForParticleAsCounted (UmiParticleEntry^        pe,
                                               UmiOperatingParameters^  operatingParameters
                                              );


    ///
    /// <summary>
    /// Gets a thumbnail version of the particle specified;  will make sure it fits with in the specified dimensions.
    /// </summary>
    /// <param name="pe">Particle entry we are to create a raster for.</param>
    /// <param name="maxHeight">Maximum number of rows.</param>
    /// <param name="maxWidth">Maximum number of cols.</param>
    ///
    UmiRaster^  GetThumbNailForParticle (UmiParticleEntry^        pe,
                                         UmiOperatingParameters^  operatingParameters,
                                         kkint32                  maxHeight,
                                         kkint32                  maxWidth
                                        );




    kkint32  ScanColumnFromDisplayCol (kkint32 displayColumn);



    // next group of methods are dedicated to the StopStart points in a scanner file. They are 
    // Managed wrappers of the unmanaged version in "ScannerFile" via the "UmiScannerFile" object. 

    void  AddStartPoint (kkint32  _scanLineNum);

    void  AddStopPoint (kkint32  _scanLineNum);

    void  StartStopPointDelete (kkint32 _scanLineNum);

    UmiStartStopPoint^  StartStopPointNearestEntry (kkint32 _scanLineNum);

    UmiStartStopPoint^  StartStopPointPrevEntry (kkint32 _scanLineNum);

    UmiStartStopPoint^  StartStopPointSuccEntry (kkint32 _scanLineNum);

    UmiStartStopPointList^  StartStopPoints ();

    const StartStopPointList&   StartStopPointsUnManaged ();

    /*
     *@brief  Returns an array indication the record rate in bytes/sec for specified time-intervals.
     *@details  Each element in the returned array will give the average number of bytes recorded for the time
     * the corresponding time interval. Array element 0 starts at the beginning of the Scanner file and covers 
     * the number of scan lines required to account for 'intervalSecs' seconds.
     */    
    array<float>^  RecordRateByTimeIntervals (int intervalSecs);


  private:
    void  Initialization ();

    void  LoadParticleEntryData (Object^ data);

    void  UpdateDisplayRow (array<byte>^    scanLine,
                            array<ushort>^  displayRow,
                            double          scanLineFrac  /**< Percentage of this row that should contribute to the displayRow.  */
                           );

    void  CalculateColumnRatios ();

    UmiScannerFile^  OpenAppropriateFormat (String^     _fileName,
                                            UmiRunLog^  _runLog
                                           );

    UmiScannerFrame^  OldestFrame ();
 
    UmiScannerFrame^  GetFrameWithScanLine (kkint32  scanLine);
  
  
    UmiScannerClock^          clock;
    array<UmiScannerFrame^>^  frames;
    UmiScannerFile^           scannerFile;
    bool                      weOwnScannerFile;

    //  Used to describe which logical column
    kkint32                   cropColLeft;             /**< Scan Column that we start with                                */
    kkint32                   cropColRight;            /**< Scan Column that we end with; other columns will be ignored.  */
    kkint32                   croppedWidth;
    float                     dispColsPerScanCol;      /**< Should be inverse of ratio.                                   */
    float                     dispRowsPerScanRow;
    UmiParticleEntryList^     particleEntries;
    bool                      particleEntriesLoaded;
    kkint32                   pixelsPerDisLine;
    float                     ratio;
    float*                    scanColPerDispColRatio;
    kkint16*                  scanColPerDispColTarget;
    kkint32                   scanLineSize;
  };  /* UmiScannerFileBuffered */

}  /* CounterManaged */

#endif
