#if  !defined(_UmiParticleEntry_)
#define  _UmiParticleEntry_


using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Diagnostics;
using namespace System::Collections::Generic;
using namespace System::Threading;


#include "OperatingParameters.h"
#include "ParticleEntry.h"
#include "SessionParameters.h"
using  namespace LarcosCounterUnManaged;

#include "UmiClass.h"
#include "UmiClassList.h"
#include "UmiRunLog.h"
#include "UmiOperatingParameters.h"
#include "UmiSessionParameters.h"
#include "UmiStartStopPoint.h"


namespace LarcosCounterManaged 
{
  #if  !defined(_UmiScannerFileBuffered_)
    ref  class  UmiScannerFileBuffered;
  #endif


  public  ref  class  UmiParticleEntry
  {
  public:
    UmiParticleEntry ();

    UmiParticleEntry (String^    _scannerFileRootName,
                      kkint32    _scannerRow,
                      kkint16    _scannerCol,
                      kkint16    _height,
                      kkint16    _width,
                      UmiClass^  _predClass,
                      float      _orientation,
                      float      _length,
                      float      _flowRateRatio,
                      float      _flowRate,
                      float      _probability,
                      float      _breakTie
                     );
  
    UmiParticleEntry (const ParticleEntry&  pe);


    property  String^    ScannerFileRootName {String^    get ();}
    property  float      BreakTie            {float      get ();}
    property  float      CountFactor         {float      get ();}
    property  String^    FileName            {String^    get ();}
    property  float      FlowRate            {float      get ();}
    property  float      FlowRateRatio       {float      get ();}
    property  kkint16    Height              {kkint16    get ();}
    property  float      Length              {float      get ();  void  set  (float _length)         {length    = _length;}}
    property  float      Orientation         {float      get ();}
    property  UmiClass^  PredClass           {UmiClass^  get ();  void  set  (UmiClass^ _predClass)  {predClass = _predClass;}}
    property  String^    PredClassName       {String^    get ();}
    property  float      Probability         {float      get ();}
    property  kkint32    ScannerRow          {kkint32    get ();}
    property  kkint32    ScannerRowOverAll   {kkint32    get ();  void  set (kkint32 _scannerRowOverAll);}
    property  kkint16    ScannerCol          {kkint16    get ();}
    property  kkint16    Width               {kkint16    get ();}

    bool  ExactMatch (kkint32  scannerRow,
                      kkint16  scannerCol,
                      kkint16  height,
                      kkint16  width
                     );

    ParticleEntryPtr  UnManaged ();

  private:
    String^    scannerFileRootName;
    kkint32    scannerRow;
    kkint32    scannerRowOverAll;  /**< A single recording session can be broken into multiple ScannerFiles. This
                                    * data member specifies the scanner row from the beginning of the recording session
                                    * while 'scannerRow' is relative to the scanner-file that contains this particle.
                                    */
    kkint16    scannerCol;
    kkint16    height;
    kkint16    width;
    UmiClass^  predClass;
    float      length;
    float      orientation;
    float      flowRateRatio;
    float      flowRate;
    float      probability;
    float      breakTie;
  };  /* UmiParticleEntry */




  public  ref  class  UmiParticleEntryList: List<UmiParticleEntry^>
  {
  public:

    enum  class  SortOrder: int  {Null = 0, ScanLine = 1,  Probability = 2,  BreakTie = 3};

    static  UmiParticleEntryList^  LoadMultipleScannerFiles (array<String^>^  scannerFileNames,
                                                             UmiRunLog^       runLog
                                                            );

    UmiParticleEntryList ();

    UmiParticleEntryList (String^     _scannerFileName,
                          bool        _loadHeaderOnly,
                          UmiRunLog^  _runLog
                         );

    !UmiParticleEntryList ();

    ~UmiParticleEntryList ();

    void  Assign (UmiParticleEntryList^  pel);

    property  bool      HeaderInfoLoaded   {bool     get () {return headerInfoLoaded;}}

    property  int       BackGroundPixelTH  {int      get ();}
    property  String^   ControlNum         {String^  get ();}
    property  kkint32   CropLeft           {kkint32  get () {return cropLeft;}}
    property  kkint32   CropRight          {kkint32  get () {return cropRight;}}
    property  String^   SessionDescription {String^  get ();}
    property  kkint16   ErosionStructSize  {kkint16  get ();}
    property  float     FlowRateFactor     {float    get ();}
    property  kkint32   PixelsPerScanLine  {kkint32  get () {return (1 + cropRight - cropLeft);}}
    property  bool      SaveParticleImages {bool     get ();}
    property  String^   TrainingModelName  {String^  get ();}


    /**
     *@brief Append a modified ParticleEntry to end of report file.
     */
    void  AddUpdateToReportFile (UmiParticleEntry^  pe);


    void  LoadFile (bool        _loadHeaderOnly,
                    UmiRunLog^  _runLog
                   );


    /// <summary>
    /// Locate the Particle that encompasses the specified pixel location.
    /// </summary>
    UmiParticleEntry^  FindParticle (kkint32  scanRow,
                                     kkint16  scanCol
                                    );

    UmiClassList^  ExtractListOfClasses ();

    UmiParticleEntryList^  GetParticlesForScanLineRange (kkint32 scanRowStart,
                                                         kkint32 scanRowEnd
                                                        );


    UmiParticleEntryList^  GetParticlesByPredictedClass (UmiClass^  predClass);


    /// <summary>
    /// Returns a sub list of particles that belong to the specified regions as driven by StartStop points.
    /// </summary>
    UmiParticleEntryList^  GetParticlesByStartStopRegions (UmiStartStopRegionList^  regions);

    UmiParticleEntryList^  GetParticlesByCountRange (kkint32  minCount,
                                                     kkint32  maxCount
                                                    );


    /**
     *@param[in]  minSize  Particles must be Greater-or-Equal to this parameter.
     *@param[in]  maxSize  Particles must be Less-Than to this parameter.
     */
    UmiParticleEntryList^  GetParticlesBySizeRange (float  minSize,
                                                    float  maxSize
                                                   );


    array<float>^  CountFrequencyByTimeIntervals (int    interval,
                                                  float  scanRate
                                                 );

    array<float>^  FlowRateRatioByTimeIntervals (int    interval,
                                                 float  scanRate
                                                );

    array<float>^  FlowRateByTimeIntervals (int    interval,
                                            float  scanRate
                                           );

    array<float>^  ParticleFrequencyByTimeIntervals (int    interval,
                                                     float  scanRate
                                                    );

    UmiOperatingParameters^  GetOperatingParameters ();
    UmiSessionParameters^    GetSessionParameters ();



    static
    array<String^>^  SortOrderOptions ();


    static
    SortOrder   UmiParticleEntryList::SortOrderFromStr (String^ s);


    void  Sort (String^  sortOrderStr);

    void  Sort (UmiParticleEntryList::SortOrder  sortOrder);

    void  SortByScanLine ();

    int  SubjectCount (UmiScannerFileBuffered^  scannerFile);


  private:
    ref class  ParticleEntryComparer: System::Collections::Generic::IComparer<UmiParticleEntry^>
    {
    public:
      ParticleEntryComparer (): sortOrder (SortOrder::ScanLine)   {}

      ParticleEntryComparer (SortOrder  _sortOrder):  sortOrder (_sortOrder)   {}


      virtual  kkint32 Compare (UmiParticleEntry^  x,   
                              UmiParticleEntry^  y
                             );

    private:
      SortOrder  sortOrder;
    };


    /**
     *@brief  Used by 'LoadFile' to locate entries already loaded such as when there was
     * a class correction and an update was appended to report file.
     *@details  If the contents of the list are sorted by ScannerRow as indicated by 
     * 'sortedByScanLine' will perform a binary search to locate entry; otherwise will
     * perform a sequential search.
     */
    UmiParticleEntry^  LocateParticle (kkint32  scannerRow,
                                       kkint16  scannerCol,
                                       kkint16  height,
                                       kkint16  width
                                      );

    UmiParticleEntry^  LocateParticle (UmiParticleEntry^  pe);


    void  SetDefaultFieldIndexs ();


    void  SplitIntoNameAndData (String^   line,
                                String^%  name,
                                String^%  value
                               );


    void  ProcessFieldDefinitions (const KKStr&  value);


    UmiParticleEntry^  ParseParticleImageLine (const KKStr&  value);
   

    kkint32  FindGreaterOrEqual (kkint32 scanRow);

    bool     loaded;   /**< Indicates if loading from file has been completed. */

    String^  baseScannerName;   /**< This will be the same as the Initial Scanner Files root name.
                                 * If the scanner file had multiple parts we will need to adjust 
                                 * Scanner File Number when plotting time based off the initial 
                                 * scanner file.
                                 */

    
    bool  headerInfoLoaded;   /**< Indicates that "UmiParticleEntryList::LoadFile: method has read past all the  header fields in 
                               * the report file but may not have finished reading the entire report file.  It will be initialized 
                               * to false upon construction and set to "true" when the first detail line is read or end of file is
                               * reached.
                               */


    OperatingParametersPtr  operatingParameters;
    SessionParametersPtr    sessionParameters;


    kkint32  cropLeft;
    kkint32  cropRight;
    String^  reportFileName;
    String^  scannerFileName;
    String^  scannerFileRootName;
    bool     sortedByScanLine;     /**< Indicates that the contents of list have been sorted by 'SortByScanLine' */

    kkint32  numFields;
    kkint16  scannerFileRootNameIDX;
    kkint16  scannerRowIDX;
    kkint16  scannerColIDX;
    kkint16  heightIDX;
    kkint16  widthIDX;
    kkint16  classNameIDX;
    kkint16  orientationIDX;
    kkint16  lengthIDX;
    kkint16  flowRateRatioIDX;
    kkint16  flowRateIDX;
    kkint16  probabilityIDX;
    kkint16  breakTieIDX;
  };  /* UmiParticleEntryList */

}  /* LarcosCounterManaged */


#endif
