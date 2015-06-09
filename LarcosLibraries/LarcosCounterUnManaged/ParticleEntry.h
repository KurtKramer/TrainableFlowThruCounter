/* ParticleEntry.h -- Represents a single identified particle in a Scanner File.
 * Copyright (C) 2011-2013  Kurt Kramer
 * For conditions of distribution and use, see copyright notice in LarcosCounterUnManaged.txt
 */
#if  !defined(_PARTICLEENTRY_)
#define  _PARTICLEENTRY_

#include <queue>

#include "GoalKeeperSimple.h"
#include "KKQueue.h"
#include "MorphOpStretcher.h"
#include "Raster.h"
#include "RasterBuffer.h"
#include "RunLog.h"

#include "FeatureVector.h"
#include "MLClass.h"
using namespace KKMLL;

#include "StartStopPoint.h"
using  namespace  KKLSC;



namespace LarcosCounterUnManaged
{
  class  ParticleEntry
  {
  public:
    typedef  ParticleEntry*  ParticleEntryPtr;

    ParticleEntry ();

    ParticleEntry (const ParticleEntry&  _entry);

    ParticleEntry (const KKStr& _scannerFileRootName,
                   kkint32      _scannerRow,
                   kkint32      _scannerCol,
                   kkint32      _height,
                   kkint32      _width,
                   MLClassPtr   _mlClass,
                   float        _orientation,
                   float        _length,
                   float        _flowRateRatio,
                   float        _flowRate,
                   float        _probability,
                   float        _breakTie
                  );

    ~ParticleEntry ();

    kkint32  MemoryConsumedEstimated ()  const;


    const KKStr&  ScannerFileRootName () const {return scannerFileRootName;}
    kkint32       ScannerRow          () const {return scannerRow;}
    kkint32       ScannerCol          () const {return scannerCol;}
    kkint32       Height              () const {return height;}
    kkint32       Width               () const {return width;}
    MLClassPtr    PredClass           () const {return predClass;}
    const KKStr&  PredClassName       () const;
    float         Orientation         () const {return orientation;}
    float         Length              () const {return length;}
    float         FlowRateRatio       () const {return flowRateRatio;}
    float         FlowRate            () const {return flowRate;}

    float         Probability         () const {return probability;}
    float         BreakTie            () const {return breakTie;}

    FeatureVectorPtr  FeatureVector       () const {return featureVector;}

    void  PredClass (MLClassPtr _predClass)  {predClass = _predClass;}


    void  Assign (const ParticleEntry&   _entry);

    void  Assign (const KKStr&  _scannerFileRootName,
                  kkint32       _scannerRow,
                  kkint32       _scannewCol,
                  kkint32       _height,
                  kkint32       _width,
                  MLClassPtr    _mlClass,
                  float         _orientation,
                  float         _length,
                  float         _flowRateRatio,
                  float         _flowRate,
                  float         _probability,
                  float         _breakTie
                 );

    bool  ExactMatch (kkint32  _scannerRow,
                      kkint16  _scannerCol,
                      kkint16  _height,
                      kkint16  _width
                     );

    void  GivingOwnershipOfFeatureVector (FeatureVectorPtr  _featureVector) {featureVector = _featureVector;}

    FeatureVectorPtr  TakeOwnershipOfFeatureVector ();

  private:
    KKStr             scannerFileRootName;
    kkint32           scannerRow;
    kkint32           scannerCol;
    kkint32           height;
    kkint32           width;
    MLClassPtr        predClass;
    float             orientation;      /**< In Degrees */
    float             length;
    float             flowRateRatio;    /**< Number of Temporal Pixels/ Spatial Pixel. */
    float             flowRate;         /**< Meters/Sec .                              */
    float             probability;
    float             breakTie;
    FeatureVectorPtr  featureVector;    /**<  If not null;  add to end of FeatureVectr list
                                         * that is being built for the entire run.
                                         *  This was done so ewe could get the FeatureVectors
                                         * of all particles in a ScannerFile for test purposes.
                                         * In production this should always be NULL.
                                         */
  };  /* ParticleEntry */

  typedef  ParticleEntry*  ParticleEntryPtr;



  class  ParticleEntryList:  public KKQueue<ParticleEntry>
  {
  public:
    ParticleEntryList (bool _owner);

    ParticleEntryList (const ParticleEntryList&  list);

    ParticleEntryList (const KKStr&  _scannerFileName,
                       RunLog&       _log
                      );

    ~ParticleEntryList ();

    MLClassListPtr  ExtractListOfClasses ()  const;

    kkint32  FindGreaterOrEqual (kkint32 scanRow);

    ParticleEntryPtr  LocateParticle (ParticleEntryPtr  pe);

    ParticleEntryPtr  LocateParticle (kkint32  scannerRow,
                                      kkint16  scannerCol,
                                      kkint16  height,
                                      kkint16  width
                                     );

    VectorFloatPtr  CountFrequencyByTimeIntervals (int    interval,
                                                   float  scanRate
                                                  );

    VectorFloatPtr  FlowRateRatioByTimeIntervals (int    interval,
                                                  float  scanRate
                                                 );

    VectorFloatPtr  FlowRateByTimeIntervals (int    interval,
                                             float  scanRate
                                            );

    VectorFloatPtr  ParticleFrequencyByTimeIntervals (int    interval,
                                                      float  scanRate
                                                     );

    int  SubjectCount (const StartStopPointList&   startStopPoints);

    void  SortByScanLine ();


  private:
    void     LoadFile (RunLog&  _log);

    ParticleEntryPtr  ParseParticleImageLine (const KKStr&  value);

    void     ProcessFieldDefinitions (const KKStr&  value);

    void     SetDefaultFieldIndexs ();

    void     SplitIntoNameAndData (const KKStr&  line,
                                   KKStr&        name,
                                   KKStr&        value
                                  );

    
    KKStr    baseScannerName;      /**< This will be the same as the Initial Scanner Files root name.
                                    * If the scanner file had multiple parts we will need to adjust
                                    * Scanner File Number when plotting time based off the initial
                                    * scanner file.
                                    */
    KKStr    controlNum;
    KKStr    description;
    kkint16  erosionStructSize;
    float    flowRateFactor;
    bool     loaded;               /**< Indicates if loading from file has been completed. */
    KKStr    reportFileName;
    bool     saveParticleImages;
    KKStr    scannerFileName;
    KKStr    scannerFileRootName;
    bool     sortedByScanLine;     /**< Indicates if the current contents of this list are sorted by ScanLine */
    KKStr    trainingModelName;

    kkint16  numFields;
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
        class  ScanLinePredecessor;

 };

  typedef  ParticleEntryList*  ParticleEntryListPtr;



  /**
   *@brief Will allow for communications between threads of ImageEntry data.
   *@details  Specifically designed to allow the 'LogicalFrameProcessor' threads to pass information about the
   * extracted particles to the report writer thread.
   */
  class  ParticleEntryBuffer
  {
  public:
    ParticleEntryBuffer ();

    ~ParticleEntryBuffer ();

    kkint32  MemoryConsumedEstimated ()  const;

    void  Add (ParticleEntryList&   list);

    /**@brief  Removes(Clears) all entries from the buffer. */
    void  Clear ();

    volatile bool  DataAvailable ()  const  {return dataAvailable;}

    void  EndBlock ();

    /**
     *@brief Writes the definition of one ParticleEntry to the specified output stream.
     */
    void  PrintImageEntryDefinition (ostream& o);

    void  StartBlock ();

    void  AddToReport (ostream& o);

  private:
    queue<ParticleEntryPtr>   buff;
    volatile bool             dataAvailable;
    GoalKeeperSimplePtr       goalie;
  };  /* ParticleEntryBuffer */


  typedef  ParticleEntryBuffer*  ParticleEntryBufferPtr;
}  /* LarcosCounterUnManaged */

#endif
