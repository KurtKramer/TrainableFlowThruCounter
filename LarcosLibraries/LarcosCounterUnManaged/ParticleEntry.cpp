/* ParticleEntry.cpp -- Represents a single camera frame.
 * Copyright (C) 2011-2011  Kurt Kramer
 * For conditions of distribution and use, see copyright notice in KKLineScanner.h
 */
#include "FirstIncludes.h"

#include <errno.h>
#include <fstream>
#include <istream>
#include <iostream>
#include <vector>
#include <math.h>
#include "MemoryDebug.h"
using namespace std;


#include "KKBaseTypes.h"
#include "ImageIO.h"
#include "OSservices.h"
#include "Raster.h"
using namespace KKB;

#include "MLClass.h"
using namespace KKMLL;


#include "ParticleEntry.h"
using  namespace  LarcosCounterUnManaged;


ParticleEntry::ParticleEntry ():
    scannerFileRootName (),
    scannerRow          (0),
    scannerCol          (0),
    height              (0),
    width               (0),
    predClass           (NULL),
    orientation         (0.0f),
    length              (0.0f),
    flowRateRatio       (1.0f),
    flowRate            (1.0f),
    probability         (0.0f),
    breakTie            (0.0f),
    featureVector       (NULL)
{
}


ParticleEntry::ParticleEntry (const ParticleEntry&  _entry):

    scannerFileRootName (_entry.scannerFileRootName),
    scannerRow          (_entry.scannerRow),
    scannerCol          (_entry.scannerCol),
    height              (_entry.height),
    width               (_entry.width ),
    predClass           (_entry.predClass),
    orientation         (_entry.orientation),
    length              (_entry.length),
    flowRateRatio       (_entry.flowRateRatio),
    flowRate            (_entry.flowRate),
    probability         (_entry.probability),
    breakTie            (_entry.breakTie),
    featureVector       (_entry.featureVector)
{
}



ParticleEntry::ParticleEntry (const KKStr&  _scannerFileRootName,
                              kkint32       _scannerRow,
                              kkint32       _scannerCol,
                              kkint32       _height,
                              kkint32       _width,
                              MLClassPtr    _predClass,
                              float         _orientation,
                              float         _length,
                              float         _flowRateRatio,
                              float         _flowRate,
                              float         _probability,
                              float         _breakTie
                             ):
    scannerFileRootName (_scannerFileRootName ),
    scannerRow          (_scannerRow),
    scannerCol          (_scannerCol),
    height              (_height),
    width               (_width ),
    predClass           (_predClass),
    orientation         (_orientation),
    length              (_length),
    flowRateRatio       (_flowRateRatio),
    flowRate            (_flowRate),
    probability         (_probability),
    breakTie            (_breakTie),
    featureVector       (NULL)
{
}


ParticleEntry::~ParticleEntry ()
{
  delete  featureVector;
  featureVector = NULL;
}



kkint32  ParticleEntry::MemoryConsumedEstimated ()  const
{
  return 4 * sizeof(kkint32) + sizeof (MLClassPtr) + scannerFileRootName.MemoryConsumedEstimated ();
}



const KKStr&   ParticleEntry::PredClassName () const
{
  if  (predClass)
    return predClass->Name ();
  else
    return KKStr::EmptyStr ();
}



void  ParticleEntry::Assign (const ParticleEntry&   _entry)
{
  scannerFileRootName = _entry.scannerFileRootName;
  scannerRow          = _entry.scannerRow;
  scannerCol          = _entry.scannerCol;
  height              = _entry.height;
  width               = _entry.width;
  predClass           = _entry.predClass;
  orientation         = _entry.orientation;
  length              = _entry.length;
  flowRateRatio       = _entry.flowRateRatio;
  flowRate            = _entry.flowRate;
  probability         = _entry.probability;
  breakTie            = _entry.breakTie;
  featureVector       = _entry.featureVector;
}




bool  ParticleEntry::ExactMatch (kkint32  _scannerRow,
                                 kkint16  _scannerCol,
                                 kkint16  _height,
                                 kkint16  _width
                                )
{
  return  (_scannerRow == scannerRow)  &&  
          (_scannerCol == scannerCol)  &&
          (_height     == height)      &&
          (_width      == width);
}




FeatureVectorPtr  ParticleEntry::TakeOwnershipOfFeatureVector ()
{
  FeatureVectorPtr  tempFV = featureVector;
  featureVector = NULL;
  return  tempFV;
}




void  ParticleEntry::Assign (const KKStr& _scannerFileRootName,
                             kkint32      _scannerRow,
                             kkint32      _scannerCol,
                             kkint32      _height,
                             kkint32      _width,
                             MLClassPtr   _predClass,
                             float        _orientation,
                             float        _length,
                             float        _flowRateRatio,
                             float        _flowRate,
                             float        _probability,
                             float        _breakTie
                            )
{
  scannerFileRootName = _scannerFileRootName;
  scannerRow          = _scannerRow;
  scannerCol          = _scannerCol;
  height              = _height;
  width               = _width;
  predClass           = _predClass;
  orientation         = _orientation;
  length              = _length;
  flowRateRatio       = _flowRateRatio;
  flowRate            = _flowRate;
  probability         = _probability;
  breakTie            = _breakTie;
}


ParticleEntryList::ParticleEntryList (bool _owner):
   KKQueue<ParticleEntry> (_owner),
   baseScannerName        (""),
   controlNum             (),
   description            (),
   erosionStructSize      (1),
   flowRateFactor         (1.0f),
   loaded                 (false),
   reportFileName         (),
   saveParticleImages     (false),
   scannerFileName        (),
   scannerFileRootName    (),
   sortedByScanLine       (false),
   trainingModelName      (),
   numFields              (8),
   scannerFileRootNameIDX (0),
   scannerRowIDX          (1),
   scannerColIDX          (2),
   heightIDX              (3),
   widthIDX               (4),
   classNameIDX           (5),
   orientationIDX         (6),
   lengthIDX              (7),
   flowRateRatioIDX       (8),
   flowRateIDX            (9),
   probabilityIDX         (10),
   breakTieIDX            (11)
{ 
  SetDefaultFieldIndexs ();
}


ParticleEntryList::ParticleEntryList (const ParticleEntryList&  _list):
   KKQueue<ParticleEntry> (_list),
   baseScannerName        (""),
   controlNum             (_list.controlNum),
   description            (_list.description),
   erosionStructSize      (_list.erosionStructSize),
   flowRateFactor         (1.0f),
   loaded                 (_list.loaded),
   reportFileName         (_list.reportFileName),
   saveParticleImages     (false),
   scannerFileName        (_list.scannerFileName),
   scannerFileRootName    (_list.scannerFileRootName),
   sortedByScanLine       (_list.sortedByScanLine),
   trainingModelName      (_list.trainingModelName),
   numFields              (_list.numFields),
   scannerFileRootNameIDX (_list.scannerFileRootNameIDX),
   scannerRowIDX          (_list.scannerRowIDX),
   scannerColIDX          (_list.scannerColIDX),
   heightIDX              (_list.heightIDX),
   widthIDX               (_list.widthIDX),
   classNameIDX           (_list.classNameIDX),
   orientationIDX         (_list.orientationIDX),
   lengthIDX              (_list.lengthIDX),
   flowRateRatioIDX       (_list.flowRateRatioIDX),
   flowRateIDX            (_list.flowRateIDX),
   probabilityIDX         (_list.probabilityIDX),
   breakTieIDX            (_list.breakTieIDX)

{
}



ParticleEntryList::ParticleEntryList (const KKStr&  _scannerFileName,
                                      RunLog&       _log
                                     ):
   KKQueue<ParticleEntry> (true),
   baseScannerName        (""),
   controlNum             (),
   description            (""),
   erosionStructSize      (1),
   flowRateFactor         (1.0f),
   loaded                 (false),
   reportFileName         (),
   saveParticleImages     (false),
   scannerFileName        (_scannerFileName),
   scannerFileRootName    (),
   sortedByScanLine       (false),
   trainingModelName      (),
   numFields              (8),
   scannerFileRootNameIDX (0),
   scannerRowIDX          (1),
   scannerColIDX          (2),
   heightIDX              (3),
   widthIDX               (4),
   classNameIDX           (5),
   orientationIDX         (6),
   lengthIDX              (7),
   flowRateRatioIDX       (8),
   flowRateIDX            (9),
   probabilityIDX         (10),
   breakTieIDX            (11)
{
  _log.Level (20) << "ParticleEntryList::ParticleEntryList   ScannerFileName: " << _scannerFileName << endl;
  SetDefaultFieldIndexs ();
  scannerFileRootName = osGetRootName (_scannerFileName);
  reportFileName = osRemoveExtension (_scannerFileName) + ".rpt";
  LoadFile (_log);
}




 
ParticleEntryList::~ParticleEntryList ()
{
}




void  ParticleEntryList::SetDefaultFieldIndexs ()
{
  numFields               = 6;
  scannerFileRootNameIDX  = 0;
  scannerRowIDX           = 1;
  scannerColIDX           = 2;
  heightIDX               = 3;
  widthIDX                = 4;
  classNameIDX            = 5;
  orientationIDX          = 6;
  lengthIDX               = 7;
  flowRateRatioIDX        = 8;
  flowRateIDX             = 9;
  probabilityIDX          = 10;
  breakTieIDX             = 11;
}  /* SetDefaultFieldIndexs */



void  ParticleEntryList::SplitIntoNameAndData (const KKStr&  line,
                                               KKStr&        name,
                                               KKStr&        value
                                              )
{
  int idx = line.LocateCharacter ('\t');
  if  (idx < 0)
  {
    name = line;
    value = "";
    return;
  }

  name  = line.SubStrPart (0, idx - 1);
  value = line.SubStrPart (idx + 1);
}  /* SplitIntoNameAndData */




class  ParticleEntryList::ScanLinePredecessor
{
public:
  bool  operator() (ParticleEntryPtr  left,
                    ParticleEntryPtr  right
                   )
  {
    if  (left->ScannerRow () < right->ScannerRow ())
      return true;
    
    else if (left->ScannerRow () > right->ScannerRow ())
      return false;

    else
      return  left->ScannerCol () < right->ScannerCol ();
  }
};  /* ScanLinePredecessor */




void  ParticleEntryList::SortByScanLine ()
{
  ScanLinePredecessor  pr;
  sort (begin (), end (), pr);
  sortedByScanLine = true;
}  /* SortByScanLine */






ParticleEntryPtr  ParticleEntryList::LocateParticle (ParticleEntryPtr  pe)
{
  if  (pe == NULL)
    return NULL;
  else
    return LocateParticle (pe->ScannerRow (), pe->ScannerCol (), pe->Height (), pe->Width ());
}



ParticleEntryPtr  ParticleEntryList::LocateParticle (kkint32  scannerRow,
                                                     kkint16  scannerCol,
                                                     kkint16  height,
                                                     kkint16  width
                                                    )
{
  if  (sortedByScanLine)
  {
    kkint32  b = 0;
    kkint32  e = (kkint32)size () - 1;
    kkint32  m = -1;
    ParticleEntryPtr  pe = NULL;

    while  (b <= e)
    {
      m = (b + e) / 2;
      pe  = at(m);
      if  (pe->ScannerRow () > scannerRow)
      {
        e = m - 1;
      }
      else if  (pe->ScannerRow () < scannerRow)
      {
        b = m + 1;
      }
      else
      {
        break;
      }
    }

    // We found an entry with the same ScannerRow;  there can be several Particles that start on the 
    // same ScannerRow so we have to search sequentially both forward and backward from current position 'm'.

    int  x = m;
    while  (x >= 0)  
    {
      pe = at (x);
      if  (pe->ScannerRow () != scannerRow)
        break;

      if  (pe->ExactMatch (scannerRow, scannerCol, height, width))
        return pe;
      --x;
    }

    x = m + 1;
    while  (x < (kkint32)size ())
    {
      pe = at (x);
      if  (pe->ScannerRow () != scannerRow)
        break;

      if  (pe->ExactMatch (scannerRow, scannerCol, height, width))
        return pe;
      ++x;
    }

    // At this point none of the entries were a match.  This indicates that no entries
    // match the criteria.
    return  NULL;
  }
  else
  {
    ParticleEntryList::const_iterator  idx;
    for  (idx = begin ();  idx != end ();  ++idx)
    {
      ParticleEntryPtr pe = *idx;
      if  (pe->ExactMatch (scannerRow, scannerCol, height, width))
        return pe;
    }
    return  NULL;
  }
}  /* LocateParticle */



kkint32  ParticleEntryList::FindGreaterOrEqual (kkint32 scanRow)
{
  if  (size () < 1)
    return -1;

  if  (at (size () - 1)->ScannerRow () < scanRow)
    return -1;
  
  kkint32  b = 0;
  kkint32  e = (kkint32)size () - 1;

  while  (true)
  {
    int m = (b + e) / 2;

    ParticleEntryPtr  pe = at (m);
    if  (pe->ScannerRow () < scanRow)
      b = m + 1;

    else if  (pe->ScannerRow () > scanRow)
      e = m - 1;

    else if  (pe->ScannerRow () == scanRow)
      e = m;

    if  (b >= e)
      break;
  }

  return  b;
}   /* FindGreaterOrEqual*/



void  ParticleEntryList::ProcessFieldDefinitions (const KKStr&  value)
{
  VectorKKStr  fields = value.Split ('\t');
 
  if  (fields.size () < 2)
    return;

  if  (!fields[0].EqualIgnoreCase ("PE"))
    return;

  numFields = fields.size () - 1;

  kkuint32  idx = 1;
  kkuint32  defIDX = 0;
  while  (idx < fields.size ())
  {
    KKStr  fieldName = fields[idx];
     
    if  (fieldName.EqualIgnoreCase ("scannerfilerootname")) 
      scannerFileRootNameIDX = defIDX;

    else if  (fieldName.EqualIgnoreCase ("scanrow")  ||  fieldName.EqualIgnoreCase ("scannerrow"))
      scannerRowIDX = defIDX;

    else if  (fieldName.EqualIgnoreCase ("scancol")  ||  fieldName.EqualIgnoreCase ("scannercol"))
      scannerColIDX = defIDX;

    else if  (fieldName.EqualIgnoreCase ("height"))
      heightIDX = defIDX;

    else if  (fieldName.EqualIgnoreCase ("width"))
      widthIDX = defIDX;

    else if  (fieldName.EqualIgnoreCase ("classname"))
      classNameIDX = defIDX;

    else if  (fieldName.EqualIgnoreCase ("orientation"))
      orientationIDX = defIDX;

    else if  (fieldName.EqualIgnoreCase ("length"))
      lengthIDX = defIDX;

    else if  (fieldName.EqualIgnoreCase ("FlowRateRatio"))
      flowRateRatioIDX = defIDX;

    else if  (fieldName.EqualIgnoreCase ("FlowRate"))
      flowRateIDX = defIDX;

    else if  (fieldName.EqualIgnoreCase ("Probability"))
      flowRateIDX = defIDX;

    else if  (fieldName.EqualIgnoreCase ("BreakTie"))
      flowRateIDX = defIDX;

    ++idx;
    ++defIDX;
  }
}  /* ProcessFieldDefinitions */



ParticleEntryPtr  ParticleEntryList::ParseParticleImageLine (const KKStr&  value)
{
  VectorKKStr  fields = value.Split ('\t');
  if  ((kkint32)fields.size () < numFields)
    return NULL;

  KKStr  scannerFileRootName = "";

  kkint32  scanRow       = 0;
  kkint16  scanCol       = 0;
  kkint16  height        = 0;
  kkint16  width         = 0;
  float    orientation   = 0.0f;
  float    length        = 0.0f;
  float    flowRateRatio = 1.0f;
  float    flowRate      = 1.0f;

  float    probability   = 0.0f;
  float    breakTie      = 0.0f;

  MLClassPtr  ic = NULL;

  scannerFileRootName = fields[scannerFileRootNameIDX];
  scanRow             = fields[scannerRowIDX].ToInt ();
  scanCol             = (short)fields[scannerColIDX].ToInt32 ();
  height              = (short)fields[heightIDX].ToInt32 ();
  width               = (short)fields[widthIDX].ToInt32 ();

  ic = MLClass::CreateNewMLClass (fields[classNameIDX], -1);

  if  ((orientationIDX   >= 0)  &&  (orientationIDX   < numFields))  orientation   = fields[orientationIDX].ToFloat ();
  if  ((lengthIDX        >= 0)  &&  (lengthIDX        < numFields))  length        = fields[lengthIDX].ToFloat ();
  if  ((flowRateRatioIDX >= 0)  &&  (flowRateRatioIDX < numFields))  flowRateRatio = fields[flowRateRatioIDX].ToFloat ();
  if  ((flowRateIDX      >= 0)  &&  (flowRateIDX      < numFields))  flowRate      = fields[flowRateIDX].ToFloat ();

  if  ((probabilityIDX   >= 0)  &&  (probabilityIDX   < numFields))  probability    = fields[probabilityIDX].ToFloat ();
  if  ((breakTieIDX      >= 0)  &&  (breakTieIDX      < numFields))  breakTie       = fields[breakTieIDX].ToFloat ();

  ParticleEntryPtr  pe = new ParticleEntry (scannerFileRootName, scanRow, scanCol, height, width, ic, orientation, length, flowRateRatio, flowRate, probability, breakTie);

  return pe;
}  /* ParseParticleImageLine */


   

void  ParticleEntryList::LoadFile (RunLog&  _log)
{
  scannerFileRootName = osGetRootName (scannerFileName);

  _log.Level (30) << "ParticleEntryList::LoadFile   scannerFileRootName: " << scannerFileRootName << endl;

  baseScannerName = scannerFileRootName;

  // Total number of scanner rows read;  since sometimes a recording session can span multiple scanner files
  // we will accumulate scanner rows of all prev read scanner files.
  kkint32  scannerRowsTotal = 0;
  kkint32  scannerRowLargestCurrentScannerFile = 0;

  ifstream*  sr = NULL;
  sr = new ifstream (reportFileName.Str (), ios_base::in);
  if  ((sr == NULL)  ||  (!sr->is_open ()))
    return;

  char buff[10240];
  sr->getline (buff, sizeof (buff));
  while  (!sr->eof ())
  {
    KKStr l = buff;
    if  ((l.Len () < 3)  ||  (l.SubStrPart (0, 1) == "//"))
      continue;

    KKStr  name = "";
    KKStr  value = "";
    SplitIntoNameAndData (l, name, value);

    if  (name.EqualIgnoreCase ("define"))
      ProcessFieldDefinitions (value);

    else if  (name.EqualIgnoreCase ("description"))
      description = value.ExtractQuotedStr ("", true);

    else if  (name.EqualIgnoreCase ("erosionstructsize"))
      erosionStructSize = value.ToInt32 ();

    else if  (name.EqualIgnoreCase ("flowratefactor"))
      flowRateFactor = value.ToFloat ();

    else if  ((name.EqualIgnoreCase ("controlNum"))  ||  (name.EqualIgnoreCase ("rootname")))
      controlNum = value;

    else if  (name.EqualIgnoreCase ("saveparticleimages"))
      saveParticleImages = value.ToBool ();

    else if  (name.EqualIgnoreCase ("trainingmodelname"))
      trainingModelName = value;

    else if  (name.EqualIgnoreCase ("pe"))
    {
      ParticleEntryPtr  pe = ParseParticleImageLine (value);
      if  (pe != NULL)
        PushOnBack (pe);
    }

    else if  (name.EqualIgnoreCase ("pe_update"))
    {
      // This will be an update to an existing particle in table.

      if  (!sortedByScanLine)
        SortByScanLine ();

      ParticleEntryPtr  newPE = ParseParticleImageLine (value);
      if  (newPE != NULL)
      {
        ParticleEntryPtr  existingPE = LocateParticle (newPE);
        if  (existingPE != NULL)
        {
          existingPE->PredClass (newPE->PredClass ());
        }
      }
    }

    sr->getline (buff, sizeof (buff));
  }

  sr->close ();

  SortByScanLine ();
  // We originally planned on having multiple scanner files per Recording Session; but that has changed and we now only have one.
  // as a result the following code is no longer necessary.

  //KKStr  lastScannerFileRootName = "";
  //
  //ParticleEntryList::iterator idx;
  //for  (idx = begin ();  idx != end ();  ++idx)
  //{
  //  ParticleEntryPtr  pe = *idx;
  //  if  (pe->ScannerFileRootName () != lastScannerFileRootName)
  //  {
  //    scannerRowsTotal += scannerRowLargestCurrentScannerFile;
  //    scannerRowLargestCurrentScannerFile = 0;
  //    lastScannerFileRootName = pe->ScannerFileRootName ();
  //  }
  //  scannerRowLargestCurrentScannerFile = max (scannerRowLargestCurrentScannerFile, pe->ScannerRow ());
  //
  //  pe->ScannerRowOverAll (scannerRowsTotal + pe->ScannerRow ());
  //}

  loaded = true;

  _log.Level (30) << "ParticleEntryList::LoadFile   Exiting" << endl;

}  /* LoadFile */




MLClassListPtr  ParticleEntryList::ExtractListOfClasses ()  const
{
  MLClassListPtr  classes = new MLClassList ();

  map<MLClassPtr,MLClassPtr>  classesFound;

  ParticleEntryList::const_iterator  idx;
  for  (idx = begin ();  idx != end ();  ++idx)
  {
    ParticleEntryPtr  pe = *idx;
    if  (classesFound.find (pe->PredClass ()) == classesFound.end ())
    {
      classesFound.insert (pair<MLClassPtr,MLClassPtr> (pe->PredClass (), pe->PredClass()));
      classes->PushOnBack (pe->PredClass ());
    }
  }
  return  classes;
}



VectorFloatPtr  ParticleEntryList::CountFrequencyByTimeIntervals (int    interval,
                                                                  float  scanRate
                                                                 )
{
  if  (size () < 1)
    return NULL;

  if  (!loaded)
  {
    // We do not want to scan this array while loading of data is still going on.
    return NULL;
  }

  ParticleEntryPtr lastEntry = back ();
  int  maxNumSecs   = (int)(0.5f + (float)(lastEntry->ScannerRow ()) / scanRate);
  int  maxIntervals = (int)ceil ((float)maxNumSecs / (float)interval) + 1;

  VectorFloatPtr  freqHistogram = new VectorFloat (maxIntervals, 0);

  ParticleEntryList::const_iterator  idx;
  for  (idx = begin ();  idx != end ();  ++idx)
  {
    ParticleEntryPtr  pe = *idx;
    float  count = pe->PredClass ()->CountFactor ();
    int  particleOffsetSecs = (int)(0.5f + (float)(pe->ScannerRow ()) / scanRate);
    int  particleInterval = (int)(0.5f + (float)particleOffsetSecs / (float)interval);
    if  (particleInterval < maxIntervals)
      (*freqHistogram)[particleInterval] += count;
  }

  return  freqHistogram;
}  /* CountFrequencyByTimeIntervals  */




VectorFloatPtr  ParticleEntryList::FlowRateRatioByTimeIntervals (int    interval,
                                                                 float  scanRate
                                                                )
{
  if  (size () < 1)
    return NULL;

  if  (!loaded)
  {
    // We do not want to scan this array while loading of data is still going on.
    return NULL;
  }

  ParticleEntryPtr lastEntry = back ();
  int  maxNumSecs   = (int)(0.5f + (float)(lastEntry->ScannerRow ()) / scanRate);
  int  maxIntervals = (int)ceil ((float)maxNumSecs / (float)interval) + 1;

  VectorFloatPtr  flowRateRatioByIntervals  = new VectorFloat (maxIntervals, 0.0f);
  VectorInt32Ptr  patrticleCountByIntervals = new VectorInt32 (maxIntervals, 0);

  ParticleEntryList::const_iterator  idx;
  for  (idx = begin ();  idx != end ();  ++idx)
  {
    ParticleEntryPtr  pe = *idx;
    int  particleOffsetSecs = (int)(0.5f + (float)(pe->ScannerRow ()) / scanRate);
    int  particleInterval = (int)(0.5f + (float)particleOffsetSecs / (float)interval);
    if  (particleInterval < maxIntervals)
    {
      (*flowRateRatioByIntervals) [particleInterval] += pe->FlowRateRatio ();
      (*patrticleCountByIntervals)[particleInterval] += 1;
    }
  }

  for  (kkint32  interval = 0;  interval < maxIntervals;  ++interval)
  {
    kkint32 z = (*patrticleCountByIntervals)[interval];
    if  (z > 0)
     (*flowRateRatioByIntervals)[interval] /= z;
  }

  delete  patrticleCountByIntervals;   patrticleCountByIntervals = NULL;

  return  flowRateRatioByIntervals;
}  /* FlowRateRatioByTimeIntervals  */





VectorFloatPtr  ParticleEntryList::FlowRateByTimeIntervals (int    interval,
                                                            float  scanRate
                                                           )
{
  if  (size () < 1)
    return NULL;

  if  (!loaded)
  {
    // We do not want to scan this array while loading of data is still going on.
    return NULL;
  }

  ParticleEntryPtr lastEntry = back ();
  int  maxNumSecs   = (int)(0.5f + (float)(lastEntry->ScannerRow ()) / scanRate);
  int  maxIntervals = (int)ceil ((float)maxNumSecs / (float)interval) + 1;

  VectorFloatPtr  flowRateByIntervals       = new VectorFloat (maxIntervals, 0.0f);
  VectorInt32Ptr  patrticleCountByIntervals = new VectorInt32 (maxIntervals, 0);

  ParticleEntryList::const_iterator  idx;
  for  (idx = begin ();  idx != end ();  ++idx)
  {
    ParticleEntryPtr  pe = *idx;
    int  particleOffsetSecs = (int)(0.5f + (float)(pe->ScannerRow ()) / scanRate);
    int  particleInterval = (int)(0.5f + (float)particleOffsetSecs / (float)interval);
    if  (particleInterval < maxIntervals)
    {
      (*flowRateByIntervals)      [particleInterval] += pe->FlowRate ();
      (*patrticleCountByIntervals)[particleInterval] += 1;
    }
  }

  for  (kkint32  interval = 0;  interval < maxIntervals;  ++interval)
  {
    kkint32 z = (*patrticleCountByIntervals)[interval];
    if  (z > 0)
     (*flowRateByIntervals)[interval] /= z;
  }

  delete  patrticleCountByIntervals;   patrticleCountByIntervals = NULL;

  return  flowRateByIntervals;
}  /* FlowRateByTimeIntervals  */








VectorFloatPtr  ParticleEntryList::ParticleFrequencyByTimeIntervals (int    interval,
                                                                     float  scanRate
                                                                    )
{
  if  (size () < 1)
    return NULL;

  if  (!loaded)
  {
    // We do not want to scan this array while loading of data is still going on.
    return NULL;
  }

  ParticleEntryPtr lastEntry = back ();
  int  maxNumSecs   = (int)(0.5f + (float)(lastEntry->ScannerRow ()) / scanRate);
  int  maxIntervals = (int)ceil ((float)maxNumSecs / (float)interval) + 1;

  VectorFloatPtr  freqHistogram = new VectorFloat (maxIntervals, 0);

  ParticleEntryList::const_iterator  idx;
  for  (idx = begin ();  idx != end ();  ++idx)
  {
    ParticleEntryPtr  pe = *idx;
    int  particleOffsetSecs = (int)(0.5f + (float)(pe->ScannerRow ()) / scanRate);
    int  particleInterval = (int)(0.5f + (float)particleOffsetSecs / (float)interval);
    if  (particleInterval < maxIntervals)
      (*freqHistogram)[particleInterval] += 1.0f;
  }

  return  freqHistogram;
}  /* ParticleFrequencyByTimeIntervals  */





int  ParticleEntryList::SubjectCount (const StartStopPointList&   startStopPoints)
{
  StartStopPoint::StartStopType curType = StartStopPoint::StartStopType::StartPoint;
  kkint32  curTypeStartLineNum = 0;

  if  ((startStopPoints.size () > 0)  &&  (startStopPoints[0]->ScanLineNum () == 0))
  {
    curType             = startStopPoints[0]->Type ();
    curTypeStartLineNum = startStopPoints[0]->ScanLineNum ();
  }

  float  subjectCount = 0.0f;

  kkuint32  x = 0;
  while  (x < size ())
  {
    StartStopPoint::StartStopType nextType = StartStopPoint::StartStopType::StartPoint;
    kkint32                       nextTypeStartScanLineNum = 999999999;

    StartStopPointPtr  next = startStopPoints.SuccEntry (curTypeStartLineNum + 1);
    if  (next != NULL)
    {
      nextType                 = next->Type ();
      nextTypeStartScanLineNum = next->ScanLineNum ();
    }

    ParticleEntryPtr  pe = at (x);
    while  ((x < size ())  &&  (pe->ScannerRow () < nextTypeStartScanLineNum))
    {
      if  (curType == StartStopPoint::StartStopType::StartPoint)
        subjectCount += pe->PredClass ()->CountFactor ();
      ++x;
      if  (x < size ())
        pe = at(x);
    }

    curType              = nextType;
    curTypeStartLineNum  = nextTypeStartScanLineNum;
  }

  return  (int)(subjectCount + 0.5f);
}  /* SubjectCount */








ParticleEntryBuffer::ParticleEntryBuffer ():
      buff          (),
      dataAvailable (false),
      goalie        (NULL)
{
  GoalKeeperSimple::Create ("ParticleEntryBuffer", goalie);
}





ParticleEntryBuffer::~ParticleEntryBuffer ()
{
  Clear ();
  GoalKeeperSimple::Destroy (goalie);  
  goalie = NULL;
}


kkint32  ParticleEntryBuffer::MemoryConsumedEstimated ()  const
{
  return buff.size () * 80 + goalie->MemoryConsumedEstimated ();
}



void  ParticleEntryBuffer::Clear ()
{
  goalie->StartBlock ();

  while  (buff.size () > 0)
  {
    ParticleEntryPtr  e = buff.front ();
    buff.pop ();
    delete  e;
  }

  goalie->EndBlock ();
}  /* Clear */



void  ParticleEntryBuffer::Add (ParticleEntryList&   list)
{
  if  (list.QueueSize () < 1)
    return;

  goalie->StartBlock ();

  bool  origListOwner = list.Owner ();
  list.Owner (false);

  ParticleEntryList::iterator  idx;
  for  (idx = list.begin ();  idx != list.end ();  ++idx)
    buff.push (*idx);

  list.DeleteContents ();

  dataAvailable = true;

  list.Owner (origListOwner);
  goalie->EndBlock ();
}






void  ParticleEntryBuffer::PrintImageEntryDefinition (ostream& o)
{
  o << "Define" << "\t" << "PE" 
                << "\t" << "ScannerFileRootName"
                << "\t" << "ScannerRow"
                << "\t" << "ScannerCol"
                << "\t" << "Height"
                << "\t" << "Width"
                << "\t" << "PredClassName"
                << "\t" << "Orientation"
                << "\t" << "Length"
                << "\t" << "FlowRateRatio"
                << "\t" << "FlowRate"
                << "\t" << "Probability"
                << "\t" << "BreakTie"
                << endl;
}



void  ParticleEntryBuffer::AddToReport (ostream& o)
{
  goalie->StartBlock ();

  while  (buff.size () > 0)
  {
    ParticleEntryPtr  e = buff.front ();
    buff.pop ();

    o << "PE" << "\t" << e->ScannerFileRootName ()
              << "\t" << e->ScannerRow    ()
              << "\t" << e->ScannerCol    ()
              << "\t" << e->Height        ()
              << "\t" << e->Width         ()
              << "\t" << e->PredClassName ()
              << "\t" << e->Orientation   ()
              << "\t" << e->Length        ()
              << "\t" << e->FlowRateRatio ()
              << "\t" << e->FlowRate      ()
              << "\t" << e->Probability   ()
              << "\t" << e->BreakTie      ()
              << endl;
    
    FeatureVectorPtr  fv = e->TakeOwnershipOfFeatureVector ();
    if  (fv)
    {
      o << "FV" << "\t" << fv->ExampleFileName ();
      for  (kkint32 idx = 0;  idx < fv->NumOfFeatures ();  ++idx)
        o << "\t" << fv->FeatureData (idx);
      o << endl;
    }
    delete  e;
    e = NULL;
  }

  dataAvailable = false;
 
  goalie->EndBlock ();
}  /* AddToReport */



void  ParticleEntryBuffer::StartBlock ()
{
  goalie->StartBlock ();
}


void  ParticleEntryBuffer::EndBlock ()
{
  goalie->EndBlock ();
}



