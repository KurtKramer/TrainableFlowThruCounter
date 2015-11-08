#include "StdAfx.h"
#include "FirstIncludes.h"
#include <stdio.h>
#include <math.h>
#include <ctype.h>
#include <time.h>
#include <fstream>
#include <iostream>
#include <map>
#include <ostream>
#include <string>
#include <vector>
#include "MemoryDebug.h"


#include "KKBaseTypes.h"
#include "..\\KKBase\\GoalKeeper.h"
#include "OSservices.h"
using namespace  KKB;

#include "ScannerFile.h"
using namespace  KKLSC;

#include "InstallationConfig.h"
#include "SessionParameters.h"
#include "OperatingParameters.h"


#include "UmiOSservices.h"
#include "UmiKKStr.h"
#include "UmiParticleEntry.h"
#include "UmiScannerFileBuffered.h"
#include "UmiTrainingConfiguration.h"
using namespace  LarcosCounterManaged;




UmiParticleEntry::UmiParticleEntry ():
  scannerFileRootName (""),
  scannerRow          (0),
  scannerRowOverAll   (0),
  scannerCol          (0),
  height              (0),
  width               (0),
  predClass           (nullptr),
  length              (0.0f),
  orientation         (0.0f),
  flowRateRatio       (1.0f),
  flowRate            (1.0f),
  probability         (0.0f),
  breakTie            (0.0f)
{
}




UmiParticleEntry::UmiParticleEntry (String^    _scannerFileRootName,
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
                                   ):

  scannerFileRootName (_scannerFileRootName),
  scannerRow          (_scannerRow),
  scannerRowOverAll   (0),
  scannerCol          (_scannerCol),
  height              (_height),
  width               (_width),
  predClass           (_predClass),
  length              (_length),
  orientation         (_orientation),
  flowRateRatio       (_flowRateRatio),
  flowRate            (_flowRate),
  probability         (_probability),
  breakTie            (_breakTie)
{
}  /* ParticleEntry */  
  



UmiParticleEntry::UmiParticleEntry (const ParticleEntry&  pe):
    scannerFileRootName (UmiKKStr::KKStrToSystenStr (pe.ScannerFileRootName ())),
    scannerRow          (pe.ScannerRow        ()),
    scannerRowOverAll   (0),
    scannerCol          (pe.ScannerCol        ()),
    height              (pe.Height            ()),
    width               (pe.Width             ()),
    predClass           (UmiClassList::GetUniqueClass (pe.PredClass ())),
    length              (pe.Length            ()),
    orientation         (pe.Orientation       ()),
    flowRateRatio       (pe.FlowRateRatio     ()),
    flowRate            (pe.FlowRate          ()),
    probability         (pe.Probability       ()),
    breakTie            (pe.BreakTie          ())
{
}



String^     UmiParticleEntry::ScannerFileRootName::get ()   {return scannerFileRootName;}
float       UmiParticleEntry::FlowRate::get            ()   {return flowRate;}
float       UmiParticleEntry::FlowRateRatio::get       ()   {return flowRateRatio;}
kkint16     UmiParticleEntry::Height::get              ()   {return height;}
float       UmiParticleEntry::Length::get              ()   {return length;}
float       UmiParticleEntry::Orientation::get         ()   {return orientation;}
UmiClass^   UmiParticleEntry::PredClass::get           ()   {return predClass;}
kkint32     UmiParticleEntry::ScannerRow::get          ()   {return scannerRow;}
kkint32     UmiParticleEntry::ScannerRowOverAll::get   ()   {return scannerRowOverAll;}  
kkint16     UmiParticleEntry::ScannerCol::get          ()   {return scannerCol;}
kkint16     UmiParticleEntry::Width::get               ()   {return width;}
float       UmiParticleEntry::Probability::get         ()   {return probability;}
float       UmiParticleEntry::BreakTie::get            ()   {return breakTie;}




String^  UmiParticleEntry::PredClassName::get ()
{
  if  (predClass == nullptr)
    return "";
  else
    return predClass->Name;
}




float  UmiParticleEntry::CountFactor::get ()
{
  if  (predClass == nullptr)
    return 0.0f;
  else
    return predClass->CountFactor;
}




String^  UmiParticleEntry::FileName::get ()
{
  String^  sfrn = scannerFileRootName;
  if  (sfrn == nullptr)
    sfrn = "";

  String^  fileName = sfrn + "_" + this->scannerRow.ToString ("00000000") + "-" + scannerCol.ToString ("0000") + ".bmp";

  return  fileName;
}




void  UmiParticleEntry::ScannerRowOverAll::set (kkint32  _scannerRowOverAll) {scannerRowOverAll = _scannerRowOverAll;}





bool  UmiParticleEntry::ExactMatch (kkint32  _scannerRow,
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


ParticleEntryPtr  UmiParticleEntry::UnManaged ()
{
  MLClassPtr ic = NULL;
  if  (predClass != nullptr)
    ic = predClass->UnmanagedImageClass ();

  ParticleEntryPtr pe 
    = new ParticleEntry (UmiKKStr::SystemStringToKKStr (scannerFileRootName),
                         scannerRow, 
                         scannerCol,
                         height,
                         width, 
                         ic,
                         orientation,
                         length,
                         flowRateRatio,
                         flowRate,
                         probability,
                         breakTie
                        );
  return  pe;
}
  






UmiParticleEntryList::UmiParticleEntryList (): 
   List<UmiParticleEntry^> (),
   sessionParameters       (NULL),
   operatingParameters     (NULL),
   baseScannerName         (""),
   headerInfoLoaded        (false),
   loaded                  (false),
   reportFileName          (nullptr),
   scannerFileName         (""),
   scannerFileRootName     (""),
   sortedByScanLine        (false)
{
  sessionParameters   = new SessionParameters   ();
  operatingParameters = new OperatingParameters ();

  SetDefaultFieldIndexs ();
}



UmiParticleEntryList::UmiParticleEntryList (String^     _scannerFileName,
                                            bool        _loadHeaderOnly,
                                            UmiRunLog^  _runLog
                                           ):
  List<UmiParticleEntry^> (),
  sessionParameters       (NULL),
  operatingParameters     (NULL),
  baseScannerName         (""),
  headerInfoLoaded        (false),
  loaded                  (false),
  reportFileName          (nullptr),
  scannerFileName         (_scannerFileName),
  scannerFileRootName     (""),
  sortedByScanLine        (false)
{
  _runLog->WriteLn (40, "UmiParticleEntryList   ScannerFileName: " + _scannerFileName);
  sessionParameters   = new SessionParameters   ();
  operatingParameters = new OperatingParameters ();
  SetDefaultFieldIndexs ();
  scannerFileRootName = UmiOSservices::GetRootName (_scannerFileName);
  reportFileName = UmiOSservices::RemoveExtension (_scannerFileName) + ".rpt";
  LoadFile (_loadHeaderOnly, _runLog);
}



UmiParticleEntryList::!UmiParticleEntryList ()
{
  delete  sessionParameters;    sessionParameters   = NULL;
  delete  operatingParameters;  operatingParameters = NULL;
}





UmiParticleEntryList::~UmiParticleEntryList ()
{
  this->!UmiParticleEntryList ();
}



UmiParticleEntryList^  UmiParticleEntryList::LoadMultipleScannerFiles (array<String^>^  scannerFileNames,
                                                                       UmiRunLog^       runLog
                                                                      )
{
  if  (runLog == nullptr)
    runLog = gcnew UmiRunLog ();

  UmiParticleEntryList^  combinedParticles = nullptr;

  if  (scannerFileNames == nullptr)
    return gcnew  UmiParticleEntryList ();

  for each  (String^  sfn in scannerFileNames)
  {
    UmiParticleEntryList^   particles = gcnew UmiParticleEntryList (sfn, 
                                                                    false,   // false = Load All data not just Header data.
                                                                    runLog
                                                                   );
    if  (combinedParticles == nullptr)
      combinedParticles = particles;
    else
      combinedParticles->AddRange (particles);
  }
  
  return  combinedParticles;
}  /* LoadMultipleScannerFiles */




void  UmiParticleEntryList::Assign (UmiParticleEntryList^  pel)
{
  sessionParameters->Assign   (*(pel->sessionParameters));
  operatingParameters->Assign (*(pel->operatingParameters));

  cropLeft            = pel->cropLeft;
  cropRight           = pel->cropRight;
  reportFileName      = pel->reportFileName;
  scannerFileName     = pel->scannerFileName;
  scannerFileRootName = pel->scannerFileRootName;
  sortedByScanLine    = pel->sortedByScanLine;
}



int  UmiParticleEntryList::BackGroundPixelTH::get ()
{
  if  (operatingParameters != nullptr)
    return operatingParameters->BackGroundPixelTH ();
  else
    return 31;
}



String^   UmiParticleEntryList::SessionDescription::get ()
{
  return UmiKKStr::KKStrToSystenStr (sessionParameters->SessionDescription ());
}



kkint16  UmiParticleEntryList::ErosionStructSize::get  () 
{
  if  (operatingParameters)
    return operatingParameters->ErosionStructSize ();
  else
    return 0;
}



float  UmiParticleEntryList::FlowRateFactor::get () 
{
  if  (operatingParameters)
    return operatingParameters->FlowRateFactor ();
  else
    return 1.0f;
}



String^  UmiParticleEntryList::ControlNum::get () 
{
  return UmiKKStr::KKStrToSystenStr (sessionParameters->ControlNum ());
}



bool  UmiParticleEntryList::SaveParticleImages::get ()
{
  return operatingParameters->SaveParticleImages ();
}



String^  UmiParticleEntryList::TrainingModelName::get () 
{
  return UmiKKStr::KKStrToSystenStr (sessionParameters->TrainingModelName ());
}



UmiOperatingParameters^  UmiParticleEntryList::GetOperatingParameters ()
{
  if  (operatingParameters)
    return gcnew UmiOperatingParameters (new OperatingParameters (*operatingParameters));
  else
    return gcnew UmiOperatingParameters ();
}



UmiSessionParameters^   UmiParticleEntryList::GetSessionParameters ()
{
  if  (sessionParameters)
    return gcnew UmiSessionParameters (new SessionParameters (*sessionParameters));
  else
    return gcnew UmiSessionParameters ();
}



void  UmiParticleEntryList::SetDefaultFieldIndexs ()
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




kkint32  UmiParticleEntryList::ParticleEntryComparer::Compare (UmiParticleEntry^ x,  UmiParticleEntry^ y)
{
  int  result = 0;

  switch  (sortOrder)
  {
  case  SortOrder::ScanLine:
    {
      kkint32 zed = x->ScannerFileRootName->CompareTo (y->ScannerFileRootName);
      if  (zed == 0)
      {
        zed = (x->ScannerRow - y->ScannerRow);
        if  (zed == 0)
          zed = (x->ScannerCol - y->ScannerCol);
      }

      result =  Math::Sign (zed);
    }
    break;

    case  SortOrder::Probability:
    {
      result = Math::Sign (x->Probability - y->Probability);
    }
    break;

    case  SortOrder::BreakTie:
    {
      result = Math::Sign (x->BreakTie - y->BreakTie);
    }
    break;

  }

  return  result;
}




void  UmiParticleEntryList::SplitIntoNameAndData (String^   line,
                                                  String^%  name,
                                                  String^%  value
                                                 )
{
  int idx = line->IndexOf ('\t');
  if  (idx < 0)
  {
    name = line;
    value = "";
    return;
  }

  if  (idx < 1)
    name = "";
  else
    name = line->Substring (0, idx);

  if  (idx == (line->Length - 1))
    value = "";
  else
    value = line->Substring (idx + 1);

}  /* SplitIntoNameAndData */


array<String^>^  UmiParticleEntryList::SortOrderOptions ()
{
  array<String^>^  options = gcnew array<String^> {"Scan-Line", "Probability", "Break-Tie"};

  return  options;
}



UmiParticleEntryList::SortOrder   UmiParticleEntryList::SortOrderFromStr (String^ s)
{
  if  (s->StartsWith ("Scan", true, System::Globalization::CultureInfo::CurrentCulture))
    return  UmiParticleEntryList::SortOrder::ScanLine;

  if  (s->StartsWith ("Prob", true, System::Globalization::CultureInfo::CurrentCulture))
    return  UmiParticleEntryList::SortOrder::Probability;

  if  (s->StartsWith ("Break", true, System::Globalization::CultureInfo::CurrentCulture))
    return  UmiParticleEntryList::SortOrder::BreakTie;

  return  UmiParticleEntryList::SortOrder::Null;
}



void  UmiParticleEntryList::Sort (String^  sortOrderStr)
{
  SortOrder so = SortOrderFromStr (sortOrderStr);
  if  (so == SortOrder::Null)
    so = SortOrder::Null;

  Sort (so);
}



void  UmiParticleEntryList::Sort (UmiParticleEntryList::SortOrder  sortOrder)
{
  Sort (gcnew ParticleEntryComparer (sortOrder));
  sortedByScanLine = true;
}  /* SortByScanLine */



void  UmiParticleEntryList::SortByScanLine ()
{
  Sort (gcnew ParticleEntryComparer (SortOrder::ScanLine));
  sortedByScanLine = true;
}  /* SortByScanLine */




UmiParticleEntry^  UmiParticleEntryList::LocateParticle (UmiParticleEntry^  pe)
{
  if  (pe == nullptr)
    return nullptr;
  else
    return LocateParticle (pe->ScannerRow, pe->ScannerCol, pe->Height, pe->Width);
}



UmiParticleEntry^  UmiParticleEntryList::LocateParticle (kkint32  scannerRow,
                                                         kkint16  scannerCol,
                                                         kkint16  height,
                                                         kkint16  width
                                                        )
{
  if  (sortedByScanLine)
  {
    kkint32  b = 0;
    kkint32  e = Count - 1;
    kkint32  m = -1;
    UmiParticleEntry^  pe = nullptr;

    while  (b <= e)
    {
      m = (b + e) / 2;
      pe  = (*this)[m];
      if  (pe->ScannerRow > scannerRow)
      {
        e = m - 1;
      }
      else if  (pe->ScannerRow < scannerRow)
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
      pe = (*this)[x];
      if  (pe->ScannerRow != scannerRow)
        break;

      if  (pe->ExactMatch (scannerRow, scannerCol, height, width))
        return pe;
      --x;
    }

    x = m + 1;
    while  (x < Count)
    {
      pe = (*this)[x];
      if  (pe->ScannerRow != scannerRow)
        break;

      if  (pe->ExactMatch (scannerRow, scannerCol, height, width))
        return pe;
      ++x;
    }

    // At this point none of the entries were a match; returning a nullptr indicates that there were no matches.
    return  nullptr;
  }
  else
  {
    for each  (UmiParticleEntry^ pe in *this)
    {
      if  (pe->ExactMatch (scannerRow, scannerCol, height, width))
        return pe;
    }
    return  nullptr;
  }
}  /* LocateParticle */



void  UmiParticleEntryList::LoadFile (bool        _loadHeaderOnly,
                                      UmiRunLog^  _runLog
                                     )
{
  scannerFileRootName = UmiOSservices::GetRootName (scannerFileName);

  baseScannerName = scannerFileRootName;

  // Total number of scanner rows read;  since sometimes a recording session can span multiple scanner files
  // we will accumulate scanner rows of all prev read scanner files.
  kkint32  scannerRowsTotal = 0;
  kkint32  scannerRowLargestCurrentScannerFile = 0;
  String^  lastScannerFileRootName = "";

  KKStr  fn = UmiKKStr::SystemStringToKKStr (reportFileName);

  std::ifstream i (fn.Str ());
  if  (!i.is_open ())
    return;

  bool  eof = false;
  bool  eol = false;
  KKStrPtr  value = NULL;

  while  (!eof)
  {
    KKStr  name = osReadNextToken (i, "\t", eof, eol);
    if  (eof)
      break;

    if  (eol  || (name.StartsWith ("//")))
      continue;

    delete  value;
    value = NULL;
    if  (!eol)
      value = osReadRestOfLine (i, eof);

    if  (name.EqualIgnoreCase ("<SessionParameters>"))
      sessionParameters->ReadXML (i);

    else if  (name.EqualIgnoreCase ("<OperatingParameters>"))
      operatingParameters->ReadXML (i);

    else if  (name.EqualIgnoreCase ("<InstallationConfig>"))
    {
      InstallationConfigPtr  installConfig = new InstallationConfig (_runLog->Log ());
      installConfig->ReadXML (i);      
      delete  installConfig;
    }

    else if  (name.EqualIgnoreCase ("BackGroundPixelTH"))
      operatingParameters->BackGroundPixelTH ((uchar)value->ToInt32 ());

    else if  ((name.EqualIgnoreCase ("ControlNum"))  ||  (name.EqualIgnoreCase ("RootName")))
      sessionParameters->ControlNum (*value);

    else if  (name.EqualIgnoreCase ("ConnectedComponentDist"))
      operatingParameters->ConnectedComponentDist (value->ToInt32 ());

    else if  (name.EqualIgnoreCase ("CropLeft"))
      cropLeft = value->ToInt32 ();

    else if  (name.EqualIgnoreCase ("CropRight"))
      cropRight = value->ToInt32 ();

    else if  (name.EqualIgnoreCase ("DataIsToBeRecorded"))
      operatingParameters->DataIsToBeRecorded (value->ToBool ());

    else if  (name.EqualIgnoreCase ("DataIsToBeCounted"))
      operatingParameters->DataIsToBeCounted (value->ToBool ());

    else if  (name.EqualIgnoreCase ("Define"))
      ProcessFieldDefinitions (*value);

    else if  ((name.EqualIgnoreCase ("Description"))  ||  (name.EqualIgnoreCase ("SessionDescription")))
      sessionParameters->SessionDescription (*value);

    else if  (name.EqualIgnoreCase ("ErosionStructSize"))
      operatingParameters->ErosionStructSize (value->ToInt32 ());

    else if  (name.EqualIgnoreCase ("FlowRateFactor"))
      operatingParameters->FlowRateFactor (value->ToFloat ());

    else if  (name.EqualIgnoreCase ("MinSizeThreshold"))
      operatingParameters->MinSizeThreshold (value->ToInt32 ());

    else if  (name.EqualIgnoreCase ("PlayingBack"))
      operatingParameters->PlayingBack (value->ToBool ());

    else if  (name.EqualIgnoreCase ("SaveParticleImages"))
      operatingParameters->SaveParticleImages (value->ToBool ());

    else if  (name.EqualIgnoreCase ("ScanRate"))
      operatingParameters->RequestedScanRate (value->ToFloat ());

    else if  (name.EqualIgnoreCase ("TrainingModelName"))
      sessionParameters->TrainingModelName (*value);

    else if  (name.EqualIgnoreCase ("pe"))
    {
      UmiParticleEntry^  pe = ParseParticleImageLine (*value);
      if  (pe != nullptr)
      {
        Add (pe);
        headerInfoLoaded = true;
        if  (_loadHeaderOnly)
          break;
      }
    }

    else if  (name == "pe_update")
    {
      // This will be an update to an existing particle in table.
      headerInfoLoaded = true;
      if  (_loadHeaderOnly)
        break;

      if  (!sortedByScanLine)
        SortByScanLine ();

      UmiParticleEntry^  newPE = ParseParticleImageLine (*value);
      if  (newPE != nullptr)
      {
        UmiParticleEntry^  existingPE = LocateParticle (newPE);
        if  (existingPE != nullptr)
        {
          existingPE->PredClass = newPE->PredClass;
        }
      }
    }
  }
  delete  value;
  value = NULL;


  i.close ();

  SortByScanLine ();

  for each  (UmiParticleEntry^  pe in this)
  {
    if  (pe->ScannerFileRootName != lastScannerFileRootName)
    {
      scannerRowsTotal += scannerRowLargestCurrentScannerFile;
      scannerRowLargestCurrentScannerFile = 0;
      lastScannerFileRootName = pe->ScannerFileRootName;
    }
    scannerRowLargestCurrentScannerFile = Math::Max (scannerRowLargestCurrentScannerFile, pe->ScannerRow);
    pe->ScannerRowOverAll = scannerRowsTotal + pe->ScannerRow;
  }

  if  (!sessionParameters->TrainingModelName ().Empty ())
  {
    String^  tmn = UmiKKStr::KKStrToSystenStr (sessionParameters->TrainingModelName ());
    UmiTrainingConfiguration^  config = gcnew UmiTrainingConfiguration  (tmn, nullptr, _runLog);
  }
  loaded = true;
}  /* LoadFile */



void  UmiParticleEntryList::AddUpdateToReportFile (UmiParticleEntry^  pe)
{
  if  (String::IsNullOrEmpty (reportFileName))
    return;

  StreamWriter^  sw = nullptr;
  try  
  {
    sw = gcnew StreamWriter (reportFileName, 
                             true   // true = Append-to-End of file.
                            );
  }
  catch  (...)  {sw = nullptr;}
  if  (sw == nullptr)
    return;

  sw->WriteLine ("pe_update"
                 + "\t" + pe->ScannerFileRootName 
                 + "\t" + pe->ScannerRow
                 + "\t" + pe->ScannerCol
                 + "\t" + pe->Height
                 + "\t" + pe->Width
                 + "\t" + pe->PredClassName
                 + "\t" + pe->Orientation
                 + "\t" + pe->Length
                 + "\t" + pe->FlowRateRatio
                 + "\t" + pe->FlowRate
                 + "\t" + pe->Probability
                 + "\t" + pe->BreakTie
                );
  sw->Close ();
}  /* AddUpdateToReportFile */




void  UmiParticleEntryList::ProcessFieldDefinitions (const KKStr&  value)
{
  VectorKKStr  fields = value.Split ('\t');
 
  if  (fields.size () < 2)
    return;

  if  (!fields[0].EqualIgnoreCase ("PE"))
    return;

  numFields = (kkint32)fields.size () - 1;

  kkint32  idx = 1;
  kkint32  defIDX = 0;
  while  (idx < (kkint32)fields.size ())
  {
    KKStr  fieldName = fields[idx].ToLower ();
     
    if       (fieldName == "scannerfilerootname") scannerFileRootNameIDX = defIDX;

    else if  ((fieldName == "scanrow")     ||  
              (fieldName == "scannerrow"))        scannerRowIDX          = defIDX;

    else if  ((fieldName == "scancol")     ||
              (fieldName == "scannercol"))        scannerColIDX          = defIDX;

    else if  (fieldName == "height")              heightIDX              = defIDX;
    else if  (fieldName == "width")               widthIDX               = defIDX;
    else if  (fieldName == "classname")           classNameIDX           = defIDX;
    else if  (fieldName == "orientation")         orientationIDX         = defIDX;
    else if  (fieldName == "length")              lengthIDX              = defIDX;
    else if  (fieldName == "flowrateratio")       flowRateRatioIDX       = defIDX;
    else if  (fieldName == "flowrate")            flowRateIDX            = defIDX;
    else if  (fieldName == "probability")         probabilityIDX         = defIDX;
    else if  (fieldName == "breakTie")            breakTieIDX            = defIDX;
    
    ++idx;
    ++defIDX;
  }
}  /* ProcessFieldDefinitions */



UmiParticleEntry^  UmiParticleEntryList::ParseParticleImageLine (const KKStr&  value)
{
  VectorKKStr  fields = value.Split ('\t');

  if  (fields.size () < (uint)numFields)
    return nullptr;

  String^  scannerFileRootName = "";

  kkint32  scanRow       = 0;
  kkint16  scanCol       = 0;
  kkint16  height        = 0;
  kkint16  width         = 0;
  String^  className     = "";
  float    orientation   = 0.0f;
  float    length        = 0.0f;
  float    flowRateRatio = 0.0f;
  float    flowRate      = 0.0f;

  float    probability   = 0.0f;
  float    breakTie      = 0.0f;

  UmiClass^  ic = nullptr;

  scannerFileRootName = UmiKKStr::KKStrToSystenStr (fields[scannerFileRootNameIDX]);
  scanRow             = fields[scannerRowIDX].ToInt32 ();
  scanCol             = (short)fields[scannerColIDX].ToInt32 ();
  height              = (short)fields[heightIDX].ToInt32 ();
  width               = (short)fields[widthIDX].ToInt32 ();
  className           = UmiKKStr::KKStrToSystenStr (fields[classNameIDX]);
  ic = UmiClassList::GetUniqueClass (className, "");

  if  ((orientationIDX   >= 0)  &&  (orientationIDX   < numFields))  orientation   = fields[orientationIDX].ToFloat ();
  if  ((lengthIDX        >= 0)  &&  (lengthIDX        < numFields))  length        = fields[lengthIDX].ToFloat ();
  if  ((flowRateRatioIDX >= 0)  &&  (flowRateRatioIDX < numFields))  flowRateRatio = fields[flowRateRatioIDX].ToFloat ();
  if  ((flowRateIDX      >= 0)  &&  (flowRateIDX      < numFields))  flowRate      = fields[flowRateIDX].ToFloat ();
  if  ((probabilityIDX   >= 0)  &&  (probabilityIDX   < numFields))  probability   = fields[probabilityIDX].ToFloat ();
  if  ((breakTieIDX      >= 0)  &&  (breakTieIDX      < numFields))  breakTie      = fields[breakTieIDX].ToFloat ();

  UmiParticleEntry^  pe = gcnew UmiParticleEntry (scannerFileRootName, scanRow, scanCol, height, width, ic, orientation, length, flowRateRatio, flowRate, probability, breakTie);

  return pe;
}  /* ParseParticleImageLine */


   

UmiParticleEntry^  UmiParticleEntryList::FindParticle (kkint32  scanRow,
                                                       kkint16  scanCol
                                                      )
{
  if  (Count < 1)
    return nullptr;

  UmiParticleEntry^  result = nullptr;

  kkint32  pIdx = FindGreaterOrEqual (scanRow);
  if  (pIdx < 0)
    pIdx = Count - 1;

  kkint32 vertThreshold = 4096;

  while  (pIdx >= 0)
  {
    UmiParticleEntry^  pe = (this)[pIdx];
    kkint32  particleBot = pe->ScannerRowOverAll + pe->Height - 1;
    if  ((scanRow - particleBot) > vertThreshold)
      break;

    if  ((pe->ScannerRowOverAll <= scanRow)  &&  (particleBot >= scanRow))
    {
      kkint32  particleRight = pe->ScannerCol + pe->Width - 1;
      if  ((scanCol >= pe->ScannerCol)  &&  (scanCol <= particleRight))
      {
        result = pe;
        break;
      }
    }

    --pIdx;
  }

  return result;
}  /* FindParticle */




kkint32  UmiParticleEntryList::FindGreaterOrEqual (kkint32 scanRow)
{
  if  (Count < 1)
    return -1;

  if  (this[Count - 1]->ScannerRowOverAll < scanRow)
    return -1;
  
  int  b = 0;
  int  e = Count - 1;

  while  (true)
  {
    int m = (b + e) / 2;

    UmiParticleEntry^  pe = (this)[m];
    if  (pe->ScannerRowOverAll < scanRow)
      b = m + 1;

    else if  (pe->ScannerRowOverAll > scanRow)
      e = m - 1;

    else if  (pe->ScannerRowOverAll == scanRow)
      e = m;

    if  (b >= e)
      break;
  }

  return  b;
}   /* FindGreaterOrEqual*/





UmiParticleEntryList^  UmiParticleEntryList::GetParticlesForScanLineRange (kkint32  scanRowStart,
                                                                           kkint32  scanRowEnd
                                                                          )
{
  kkint32  idx = FindGreaterOrEqual (scanRowStart);
  if  (idx < 0)
    return nullptr;

  UmiParticleEntry^  pe = this[idx];
  kkint32  scanRow = pe->ScannerRowOverAll;

  if  (scanRow > scanRowEnd)
    return nullptr;

  UmiParticleEntryList^  results = gcnew UmiParticleEntryList ();
  results->sessionParameters->Assign   (*sessionParameters);
  results->operatingParameters->Assign (*operatingParameters);

  results->cropLeft             = cropLeft;
  results->cropRight            = cropRight;
  results->reportFileName       = reportFileName;
  results->scannerFileName      = scannerFileName;
  results->scannerFileRootName  = scannerFileRootName;
  results->sortedByScanLine     = sortedByScanLine;

  while  (scanRow <= scanRowEnd)
  {
    if  (scanRow >= scanRowStart)
      results->Add (pe);
    ++idx;
    if  (idx >= Count)
      break;

    pe = this[idx];
    scanRow = pe->ScannerRowOverAll;
  }

  return  results;
}  /* GetParticlesForScanLineRange */




UmiParticleEntryList^  UmiParticleEntryList::GetParticlesByPredictedClass (UmiClass^  predClass)
{
  UmiParticleEntryList^  results = gcnew UmiParticleEntryList ();
  results->sessionParameters->Assign   (*sessionParameters);
  results->operatingParameters->Assign (*operatingParameters);

  for each  (UmiParticleEntry^  pe  in  (*this))
  {
    if  (Object::ReferenceEquals (pe->PredClass, predClass))
      results->Add (pe);
  }
  return  results;
}





UmiParticleEntryList^  UmiParticleEntryList::GetParticlesByStartStopRegions (UmiStartStopRegionList^  regions)
{
  UmiParticleEntryList^  results = gcnew UmiParticleEntryList ();
  results->Assign (this);

  for each  (UmiStartStopRegion^ region in regions)
  {
    UmiParticleEntryList^  subset = GetParticlesForScanLineRange (region->Start, region->End);
    if  (subset != nullptr)
    {
      for each  (UmiParticleEntry^  entry in  subset)
        results->Add (entry);
    }
  }
  return  results;
}



UmiParticleEntryList^  UmiParticleEntryList::GetParticlesByCountRange (kkint32  minCount,
                                                                       kkint32  maxCount
                                                                      )
{
  UmiParticleEntryList^  results = gcnew UmiParticleEntryList ();
  results->Assign (this);

  for each  (UmiParticleEntry^  pe  in  this)
  {
    int  countFactor = (int)(pe->PredClass->CountFactor);
    if  ((countFactor >= minCount)  &&  (countFactor <= maxCount))
      results->Add (pe);
  }
  return  results;
}  /* GetParticlesByCountRange */




UmiParticleEntryList^  UmiParticleEntryList::GetParticlesBySizeRange (float  minSize,
                                                                      float  maxSize
                                                                     )
{
  UmiParticleEntryList^  results = gcnew UmiParticleEntryList ();
  results->Assign (this);

  for each  (UmiParticleEntry^  pe  in  this)
  {
    float   len = pe->Length;
    if  ((len >= minSize)   &&   (len < maxSize)  &&  (pe->CountFactor == 1.0f))
      results->Add (pe);
  }
  return  results;
}  /* GetParticlesBySizeRange */




UmiClassList^  UmiParticleEntryList::ExtractListOfClasses ()
{
  UmiClassList^  classes = gcnew UmiClassList ();

  Dictionary<UmiClass^,UmiClass^>^  classesFound = gcnew Dictionary<UmiClass^,UmiClass^> ();

  for each  (UmiParticleEntry^ pe in this)
  {
    if  (!classesFound->ContainsKey (pe->PredClass))
    {
      classesFound->Add (pe->PredClass, pe->PredClass);
      classes->Add (pe->PredClass);
    }
  }
  return  classes;
}



array<float>^  UmiParticleEntryList::CountFrequencyByTimeIntervals (int    interval,
                                                                    float  scanRate
                                                                   )
{
  if  (Count < 1)
    return nullptr;

  if  (!loaded)
  {
    // We do not want to scan this array while loading of data is still going on.
    return nullptr;
  }

  UmiParticleEntry^ lastEntry = (*this)[Count - 1];
  int  maxNumSecs   = (int)(0.5f + (float)(lastEntry->ScannerRow) / scanRate);
  int  maxIntervals = (int)Math::Ceiling ((float)maxNumSecs / (float)interval) + 1;

  array<float>^  freqHistogram = gcnew array<float> (maxIntervals);

  for  (int x = 0;  x < Count;  ++x)
  {
    UmiParticleEntry^ pe = (*this)[x];
    float  count = pe->PredClass->CountFactor;
    if  (count > 0)
    {
      int  particleOffsetSecs = (int)(0.5f + (float)(pe->ScannerRow) / scanRate);
      int  particleInterval = (int)(0.5f + (float)particleOffsetSecs / (float)interval);

      // because this method can get called before the entire list is done loading 
      // we can not depend on the order or maybe more items added after we got the count.
      if  (particleInterval < maxIntervals)
        freqHistogram[particleInterval] += count;
    }
  }

  return  freqHistogram;
}  /* CountFrequencyByTimeIntervals  */



array<float>^  UmiParticleEntryList::FlowRateRatioByTimeIntervals (int    interval,
                                                                   float  scanRate
                                                                  )
{
  if  (Count < 1)
    return nullptr;

  if  (!loaded)
  {
    // We do not want to scan this array while loading of data is still going on.
    return nullptr;
  }

  UmiParticleEntry^ lastEntry = (*this)[Count - 1];
  int  maxNumSecs   = (int)(0.5f + (float)(lastEntry->ScannerRow) / scanRate);
  int  maxIntervals = (int)Math::Ceiling ((float)maxNumSecs / (float)interval) + 1;

  array<float>^  flowRateRatioByIntervals  = gcnew array<float> (maxIntervals);
  array<int>^    patrticleCountByIntervals = gcnew array<int>   (maxIntervals);

  for  (int x = 0;  x < Count;  ++x)
  {
    UmiParticleEntry^ pe = (*this)[x];
    int  particleOffsetSecs = (int)(0.5f + (float)(pe->ScannerRow) / scanRate);
    int  particleInterval = (int)(0.5f + (float)particleOffsetSecs / (float)interval);

    // because this method can get called before the entire list is done loading 
    // we can not depend on the order or maybe more items added after we got the count.
    if  (particleInterval < maxIntervals)
    {
      flowRateRatioByIntervals[particleInterval]  += pe->FlowRateRatio;
      patrticleCountByIntervals[particleInterval] += 1;
    }
  }

  for  (int z = 0;  z < maxIntervals;  ++z)
  {
    int  c = patrticleCountByIntervals[z];
    if  (c > 0)
      flowRateRatioByIntervals[z] /= (float)c;
  }

  return  flowRateRatioByIntervals;
}  /* FlowRateRatioByTimeIntervals  */




array<float>^  UmiParticleEntryList::FlowRateByTimeIntervals (int    interval,
                                                              float  scanRate
                                                             )
{
  if  (Count < 1)
    return nullptr;

  if  (!loaded)
  {
    // We do not want to scan this array while loading of data is still going on.
    return nullptr;
  }

  UmiParticleEntry^ lastEntry = (*this)[Count - 1];
  int  maxNumSecs   = (int)(0.5f + (float)(lastEntry->ScannerRow) / scanRate);
  int  maxIntervals = (int)Math::Ceiling ((float)maxNumSecs / (float)interval) + 1;

  array<float>^  flowRateByIntervals       = gcnew array<float> (maxIntervals);
  array<int>^    patrticleCountByIntervals = gcnew array<int>   (maxIntervals);

  for  (int x = 0;  x < Count;  ++x)
  {
    UmiParticleEntry^ pe = (*this)[x];
    int  particleOffsetSecs = (int)(0.5f + (float)(pe->ScannerRow) / scanRate);
    int  particleInterval = (int)(0.5f + (float)particleOffsetSecs / (float)interval);

    // because this method can get called before the entire list is done loading 
    // we can not depend on the order or maybe more items added after we got the count.
    if  (particleInterval < maxIntervals)
    {
      flowRateByIntervals[particleInterval]       += pe->FlowRateRatio;
      patrticleCountByIntervals[particleInterval] += 1;
    }
  }

  for  (int z = 0;  z < maxIntervals;  ++z)
  {
    int  c = patrticleCountByIntervals[z];
    if  (c > 0)
      flowRateByIntervals[z] /= (float)c;
  }

  return  flowRateByIntervals;
}  /* FlowRateByTimeIntervals  */






array<float>^  UmiParticleEntryList::ParticleFrequencyByTimeIntervals (int    interval,
                                                                       float  scanRate
                                                                      )
{
  if  (Count < 1)
    return nullptr;

  if  (!loaded)
  {
    // We do not want to scan this array while loading of data is still going on.
    return nullptr;
  }

  UmiParticleEntry^ lastEntry = (*this)[Count - 1];
  int  maxNumSecs   = (int)(0.5f + (float)(lastEntry->ScannerRow) / scanRate);
  int  maxIntervals = (int)Math::Ceiling ((float)maxNumSecs / (float)interval) + 1;

  array<float>^  freqHistogram = gcnew array<float> (maxIntervals);
  int maxIntervalsIdx = maxIntervals - 1;

  for  (int x = 0;  x < Count;  ++x)
  {
    UmiParticleEntry^ pe = (*this)[x];
    float  count = pe->PredClass->CountFactor;
    int  particleOffsetSecs = (int)(0.5f + (float)(pe->ScannerRow) / scanRate);
    int  particleInterval = Math::Min ((int)(0.5f + (float)particleOffsetSecs / (float)interval), maxIntervalsIdx);
    freqHistogram[particleInterval] += 1.0f;
  }

  return  freqHistogram;
}  /* ParticleFrequencyByTimeIntervals  */





int  UmiParticleEntryList::SubjectCount (UmiScannerFileBuffered^  scannerFile)
{
  const StartStopPointList&  startStopPoints = scannerFile->StartStopPointsUnManaged ();

  StartStopPoint::StartStopType curType = StartStopPoint::StartStopType::StartPoint;
  kkint32  curTypeStartLineNum = 0;

  if  ((startStopPoints.size () > 0)  &&  (startStopPoints[0]->ScanLineNum () == 0))
  {
    curType             = startStopPoints[0]->Type ();
    curTypeStartLineNum = startStopPoints[0]->ScanLineNum ();
  }

  float  subjectCount = 0.0f;

  int  x = 0;
  while  (x < Count)
  {
    StartStopPoint::StartStopType nextType = StartStopPoint::StartStopType::StartPoint;
    kkint32                       nextTypeStartScanLineNum = 999999999;

    StartStopPointPtr  next = startStopPoints.SuccEntry (curTypeStartLineNum + 1);
    if  (next != NULL)
    {
      nextType                 = next->Type ();
      nextTypeStartScanLineNum = next->ScanLineNum ();
    }

    UmiParticleEntry^  pe = (*this)[x];
    while  ((x < Count)  &&  (pe->ScannerRow < nextTypeStartScanLineNum))
    {
      if  (curType == StartStopPoint::StartStopType::StartPoint)
        subjectCount += pe->PredClass->CountFactor;
      ++x;
      if  (x < Count)
        pe = (*this)[x];
    }

    curType              = nextType;
    curTypeStartLineNum  = nextTypeStartScanLineNum;
  }

  return  (int)(subjectCount + 0.5f);
}  /* SubjectCount */
