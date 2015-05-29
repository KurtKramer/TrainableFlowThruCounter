/* OperatingParameters.cpp -- Represents the operating parameters used by Larcos; ex's:  BackGroundThreshold, FlowRateFactor.
 * Copyright (C) 2011-2013  Kurt Kramer
 * For conditions of distribution and use, see copyright notice in LarcosCounterUnManaged.txt
 */
#include "FirstIncludes.h"
#include <errno.h>
#include <istream>
#include <iostream>
#include <vector>
#include "MemoryDebug.h"
using namespace std;


#include "GlobalGoalKeeper.h"
#include "Configuration.h"
#include "KKBaseTypes.h"
#include "OSservices.h"
using namespace KKB;


#include "ScannerFile.h"
using namespace  KKLSC;

#include "OperatingParameters.h"
#include "LarcosTrainingConfiguration.h"
using  namespace  LarcosCounterUnManaged;



OperatingParameters::OperatingParameters ():
     LarcosDataRec ("OP", "LarcosOperatingParameters"),
     backGroundPixelTH        (31),
     connectedComponentDist   (1),
     dataIsToBeCounted        (true),
     dataIsToBeRecorded       (true),
     destScannerFileFormat    (ScannerFile::Format::sf4BitEncoded),
     erosionStructSize        (1),
     flatFieldEnabled         (true),
     flowRateFactor           (1.0f),
     minSizeThreshold         (300),
     playingBack              (false),
     saveParticleImages       (false),
     requestedAnalogGain      (2.5f),
     requestedDigitalGain     (1000),
     requestedSensitivityMode ("Low"),
     requestedScanRate        (10000.0f)
{
}


     
OperatingParameters::OperatingParameters (const OperatingParameters&  opParms):
     LarcosDataRec ("OP", "OperatingParaneters"),
     backGroundPixelTH        (opParms.backGroundPixelTH),
     connectedComponentDist   (opParms.connectedComponentDist),
     dataIsToBeCounted        (opParms.dataIsToBeCounted),
     dataIsToBeRecorded       (opParms.dataIsToBeRecorded),
     destScannerFileFormat    (opParms.destScannerFileFormat),
     erosionStructSize        (opParms.erosionStructSize),
     flatFieldEnabled         (opParms.flatFieldEnabled),
     flowRateFactor           (opParms.flowRateFactor),
     minSizeThreshold         (opParms.minSizeThreshold),
     playingBack              (opParms.playingBack),
     saveParticleImages       (opParms.saveParticleImages),
     requestedAnalogGain      (opParms.requestedAnalogGain),
     requestedDigitalGain     (opParms.requestedDigitalGain),
     requestedSensitivityMode (opParms.requestedSensitivityMode),
     requestedScanRate        (opParms.requestedScanRate)
{
}



OperatingParameters::~OperatingParameters ()
{
}



int  OperatingParameters::MemoryConsumedEstimated ()  const
{
  return sizeof (*this);
}



const KKStr&  OperatingParameters::DestScannerFileFormatStr ()  const
{
  return  ScannerFile::ScannerFileFormatToStr (destScannerFileFormat);
}



KKStrConstPtr OperatingParameters::GetSettingValue (const LarcosTrainingConfiguration&  c,
                                                    const KKStr&                        fieldName
                                                   )
{
  kkint32  lineNum = -1;
  KKStrConstPtr  x = c.SettingValue ("LarcosOperatingParameters", fieldName, lineNum);
  if  (x == NULL)
    x = c.SettingValue ("Larcos", fieldName, lineNum);
  return x;
}




void  OperatingParameters::UpdateFromConfiguration (const LarcosTrainingConfiguration&  c)
{
  UpdateFromConfigurationUchar      (c, "BackGroundPixelTH",        backGroundPixelTH);
  UpdateFromConfigurationInt32      (c, "ConnectedComponentDist",   connectedComponentDist);
  UpdateFromConfigurationBool       (c, "DataIsToBeCounted",        dataIsToBeCounted);
  UpdateFromConfigurationBool       (c, "DataIsToBeRecorded",       dataIsToBeRecorded);
  UpdateFromConfigurationFileFormat (c, "DestScannerFileFormat",    destScannerFileFormat);
  UpdateFromConfigurationInt32      (c, "ErosionStructSize",        erosionStructSize);
  UpdateFromConfigurationBool       (c, "FlatFieldEnabled",         flatFieldEnabled);
  UpdateFromConfigurationFloat      (c, "FlowRateFactor",           flowRateFactor);
  UpdateFromConfigurationInt32      (c, "MinSizeThreshold",         minSizeThreshold);
  UpdateFromConfigurationBool       (c, "PlayingBack",              playingBack);
  UpdateFromConfigurationBool       (c, "SaveParticleImages",       saveParticleImages);
  UpdateFromConfigurationFloat      (c, "RequestedAnalogGain",      requestedAnalogGain);
  UpdateFromConfigurationInt32      (c, "RequestedDigitalGain",     requestedDigitalGain);
  UpdateFromConfigurationKKStr      (c, "RequestedSensitivityMode", requestedSensitivityMode);
  UpdateFromConfigurationFloat      (c, "RequestedScanRate",        requestedScanRate);
}



void  OperatingParameters::Assign (const OperatingParameters& stats)
{
  backGroundPixelTH        = stats.backGroundPixelTH;
  connectedComponentDist   = stats.connectedComponentDist;
  dataIsToBeCounted        = stats.dataIsToBeCounted;
  dataIsToBeRecorded       = stats.dataIsToBeRecorded;
  destScannerFileFormat    = stats.destScannerFileFormat;
  erosionStructSize        = stats.erosionStructSize;
  flatFieldEnabled         = stats.flatFieldEnabled;
  flowRateFactor           = stats.flowRateFactor;
  minSizeThreshold         = stats.minSizeThreshold;
  playingBack              = stats.playingBack;
  saveParticleImages       = stats.saveParticleImages;
  requestedAnalogGain      = stats.requestedAnalogGain;
  requestedDigitalGain     = stats.requestedDigitalGain;
  requestedSensitivityMode = stats.requestedSensitivityMode;
  requestedScanRate        = stats.requestedScanRate;
}


void  OperatingParameters::AddToHeaderFields (ScannerHeaderFieldsPtr  headerFields)
{
  headerFields->Add ("OP:BackGroundPixelTH",        backGroundPixelTH);
  headerFields->Add ("OP:ConnectedComponentDist",   connectedComponentDist);
  headerFields->Add ("OP:DataIsToBeCounted",        dataIsToBeCounted);
  headerFields->Add ("OP:DataIsToBeRecorded",       dataIsToBeRecorded);
  headerFields->Add ("OP:DestScannerFileFormat",    DestScannerFileFormatStr ());
  headerFields->Add ("OP:ErosionStructSize",        erosionStructSize);
  headerFields->Add ("OP:FlatFieldEnabled",         flatFieldEnabled);
  headerFields->Add ("OP:FlowRateFactor",           flowRateFactor);
  headerFields->Add ("OP:MinSizeThreshold",         minSizeThreshold);
  headerFields->Add ("OP:PlayingBack",              playingBack);
  headerFields->Add ("OP:SaveParticleImages",       saveParticleImages);
  headerFields->Add ("OP:RequestedAnalogGain",      requestedAnalogGain);
  headerFields->Add ("OP:RequestedDigitalGain",     requestedDigitalGain);
  headerFields->Add ("OP:RequestedSensitivityMode", requestedSensitivityMode);
  headerFields->Add ("OP:RequestedScanRate",        requestedScanRate);
}



const KKStr&  OperatingParameters::GetScannerFileValue (ScannerFilePtr  sf, 
                                                        const KKStr&    fieldName
                                                       )
{
  KKStr  fullName = "OP:" + fieldName;
  const KKStr& fv1 = sf->GetValue (fullName);
  if  (!fv1.Empty ())
    return fv1;

  const KKStr&  fv2 = sf->GetValue (fieldName);
  if  (!fv2.Empty ())
    return fv2;

  if  (!fieldName.StartsWith ("Requested"))
    return KKStr::EmptyStr ();

  const KKStr&  fv3 = sf->GetValue (fieldName.SubStrPart (9));
  return  fv3;
}



void  OperatingParameters::UpdateFromScannerFile (ScannerFilePtr  sf)
{
  if  (sf == NULL)
    return;
  
  backGroundPixelTH       = GetScannerFileValueInt32 (sf, "BackGroundPixelTH",      backGroundPixelTH);
  connectedComponentDist  = GetScannerFileValueInt32 (sf, "ConnectedComponentDist", connectedComponentDist);
  erosionStructSize       = GetScannerFileValueInt32 (sf, "ErosionStructSize",      erosionStructSize);
  minSizeThreshold        = GetScannerFileValueInt32 (sf, "MinSizeThreshold",       minSizeThreshold);
  
  dataIsToBeCounted       = GetScannerFileValueBool  (sf, "DataIsToBeCounted",      dataIsToBeCounted);
  dataIsToBeRecorded      = GetScannerFileValueBool  (sf, "DataIsToBeRecorded",     dataIsToBeRecorded);

  // In earlier version this field was called 'FlatFieldCorrectionEnabled'  we call the following method 
  // twice; 1st with old name and then with new name to make sure that we capture its value.
  flatFieldEnabled        = GetScannerFileValueBool  (sf, "FlatFieldCorrectionEnabled",  flatFieldEnabled);
  flatFieldEnabled        = GetScannerFileValueBool  (sf, "FlatFieldEnabled",            flatFieldEnabled);

  playingBack             = GetScannerFileValueBool  (sf, "PlayingBack",            playingBack);
  saveParticleImages      = GetScannerFileValueBool  (sf, "SaveParticleImages",     saveParticleImages );
  
  const KKStr&  destScannerFileFormatStr = GetScannerFileValue (sf, "DestScannerFileFormat");
  if  (!destScannerFileFormatStr.Empty ())
    destScannerFileFormat = ScannerFile::ScannerFileFormatFromStr (destScannerFileFormatStr);

  flowRateFactor           = GetScannerFileValueFloat (sf, "FlowRateFactor",           flowRateFactor );
  requestedAnalogGain      = GetScannerFileValueFloat (sf, "RequestedAnalogGain",      requestedAnalogGain );
  requestedDigitalGain     = GetScannerFileValueInt32 (sf, "RequestedDigitalGain",     requestedDigitalGain );
  requestedSensitivityMode = GetScannerFileValueKKStr (sf, "RequestedSensitivityMode", requestedSensitivityMode );
  requestedScanRate        = GetScannerFileValueFloat (sf, "RequestedScanRate",        requestedScanRate );
}  /* UpdateFromScannerFile */
 



void  OperatingParameters::UpdateFromDataField (const KKStr&  fieldName,
                                                const KKStr&  fieldValue,
                                                bool&         fieldFound
                                               )
{
  fieldFound = false;
  if  (fieldName.EqualIgnoreCase ("BackGroundPixelTH"))
  {
    backGroundPixelTH = fieldValue.ToInt32 ();
    fieldFound = true;
  }

  else if  (fieldName.EqualIgnoreCase ("ConnectedComponentDist"))
  {
    connectedComponentDist = fieldValue.ToInt32 ();
    fieldFound = true;
  }

  else if  (fieldName.EqualIgnoreCase ("DataIsToBeCounted"))
  {
    dataIsToBeCounted = fieldValue.ToBool ();
    fieldFound = true;
  }

  else if  (fieldName.EqualIgnoreCase ("DataIsToBeRecorded"))
  {
    dataIsToBeRecorded = fieldValue.ToBool ();
    fieldFound = true;
  }

  else if  (fieldName.EqualIgnoreCase ("DestScannerFileFormat"))
  {
    destScannerFileFormat = ScannerFile::ScannerFileFormatFromStr (fieldValue);
    fieldFound = true;
  }

  else if  (fieldName.EqualIgnoreCase ("ErosionStructSize"))
  {
    erosionStructSize = fieldValue.ToInt32 ();
    fieldFound = true;
  }

  else if  (fieldName.EqualIgnoreCase ("FlatFieldEnabled"))
  {
    flatFieldEnabled = fieldValue.ToBool ();
    fieldFound = true;
  }

  else if  (fieldName.EqualIgnoreCase ("FlowRateFactor"))
  {
    flowRateFactor = fieldValue.ToFloat ();
    fieldFound = true;
  }

  else if  (fieldName.EqualIgnoreCase ("MinSizeThreshold"))
  {
    minSizeThreshold = fieldValue.ToInt32 ();
    fieldFound = true;
  }

  else if  (fieldName.EqualIgnoreCase ("PlayingBack"))
  {
    playingBack = fieldValue.ToBool ();
    fieldFound = true;
  }

  else if  (fieldName.EqualIgnoreCase ("SaveParticleImages"))
  {
    saveParticleImages = fieldValue.ToBool ();
    fieldFound = true;
  }

  else if  (fieldName.EqualIgnoreCase ("RequestedAnalogGain"))
  {
    requestedAnalogGain = fieldValue.ToFloat ();
    fieldFound = true;
  }

  else if  (fieldName.EqualIgnoreCase ("RequestedDigitalGain"))
  {
    requestedDigitalGain = fieldValue.ToInt32 ();
    fieldFound = true;
  }

  else if  (fieldName.EqualIgnoreCase ("RequestedSensitivityMode"))
  {
    requestedSensitivityMode = fieldValue;
    fieldFound = true;
  }

  else if  (fieldName.EqualIgnoreCase ("RequestedScanRate"))
  {
    requestedScanRate = fieldValue.ToFloat ();
    fieldFound = true;
  }
}  /* UpdateFromDataField */



void  OperatingParameters::WriteFieldValues (ostream&   o)  const
{
  o << "BackGroundPixelTH"        << "\t" << (int)backGroundPixelTH          << endl
    << "ConnectedComponentDist"   << "\t" << connectedComponentDist          << endl
    << "DataIsToBeCounted"        << "\t" << BoolToStr (dataIsToBeCounted)   << endl
    << "DataIsToBeRecorded"       << "\t" << BoolToStr (dataIsToBeRecorded)  << endl
    << "DestScannerFileFormat"    << "\t" << ScannerFile::ScannerFileFormatToStr (destScannerFileFormat) << endl
    << "ErosionStructSize"        << "\t" << erosionStructSize               << endl
    << "FlatFieldEnabled"         << "\t" << BoolToStr (flatFieldEnabled)    << endl
    << "FlowRateFactor"           << "\t" << flowRateFactor                  << endl
    << "MinSizeThreshold"         << "\t" << minSizeThreshold                << endl
    << "PlayingBack"              << "\t" << BoolToStr (playingBack)         << endl
    << "SaveParticleImages"       << "\t" << BoolToStr (saveParticleImages)  << endl
    << "RequestedAnalogGain"      << "\t" << requestedAnalogGain             << endl
    << "RequestedDigitalGain"     << "\t" << requestedDigitalGain            << endl
    << "RequestedSensitivityMode" << "\t" << requestedSensitivityMode        << endl
    << "RequestedScanRate"        << "\t" << requestedScanRate               << endl;
}



void  OperatingParameters::WriteConfigSection (ostream&  o)  const
{
  o << "[LarcosOperatingParameters]" << endl;

  o << "BackGroundPixelTH = "      << (int)backGroundPixelTH           << endl;
  o << "ConnectedComponentDist = " << connectedComponentDist           << endl;
  o << "DataIsToBeCounted = "      << BoolToStr (dataIsToBeCounted)    << endl;
  o << "DataIsToBeRecorded = "     << BoolToStr (dataIsToBeRecorded)   << endl;
  o << "DestScannerFileFormat = "  << DestScannerFileFormatStr ()      << endl;
  o << "ErosionStructSize = "      << erosionStructSize                << endl;
  o << "FlatFieldEnabled = "       << BoolToStr (flatFieldEnabled)     << endl;
  if  (flowRateFactor > 0.0f)
    o << "FlowRateFactor = " << flowRateFactor << endl;

  if  (minSizeThreshold > 0)
    o << "MinSizeThreshold = " << minSizeThreshold << endl;

  o << "PlayingBack = "              << BoolToStr (playingBack)        << endl;
  o << "SaveParticleImages = "       << BoolToStr (saveParticleImages) << endl;
  o << "RequestedScanRate = "        << requestedScanRate              << endl;
  o << "RequestedAnalogGain = "      << requestedAnalogGain            << endl;
  o << "RequestedDigitalGain = "     << requestedDigitalGain           << endl;
  o << "RequestedSensitivityMode = " << requestedSensitivityMode       << endl;
  o << "RequestedScanRate = "        << requestedScanRate              << endl;
}



void  OperatingParameters::WriteToRunLog (const KKStr&  prefix,
                                          RunLog&       runLog
                                         )  const
{
  runLog << prefix << "dataIsToBeCounted      : " << dataIsToBeCounted           << endl
         << prefix << "erosionStructSize      : " << erosionStructSize           << endl
         << prefix << "connectedComponentDist : " << connectedComponentDist      << endl
         << prefix << "minSizeThreshold       : " << minSizeThreshold            << endl
         << prefix << "flowRateFactor         : " << flowRateFactor              << endl
         << prefix << "saveParticleImages     : " << saveParticleImages          << endl
         << prefix << "dataIsToBeRecorded     : " << dataIsToBeRecorded          << endl
         << prefix << "destScannerFileFormat  : " << DestScannerFileFormatStr () << endl;
}



void  OperatingParameters::ValidateParameters (KKStr&  _errMsg)
{
  if  (minSizeThreshold < 1)
    _errMsg << "\n" << "Min Size Threshold[" << minSizeThreshold << "]  must be greater than 1";
    
  if  ((flowRateFactor < 0.1)  ||  (flowRateFactor >= 10.0))
    _errMsg << "\n" << "Flow Rate Factor is out of range (0.1 thru 10.0).";

  if  ((this->erosionStructSize < 0)  ||  (erosionStructSize > 8))
    _errMsg << "\n" << "Erosion Structure Size[" << erosionStructSize << "] is out of range (0 thru 8).";
      
  if  (destScannerFileFormat == ScannerFile::Format::sfUnKnown)
    _errMsg << "\n" << "Invalid File Format specified.";

  if  ((!flatFieldEnabled)  &&  (saveParticleImages || dataIsToBeCounted))
    _errMsg << "\n" << "To count or save images you must enable Flat-Field-Correction";

  if  (requestedScanRate < 1000)  
    _errMsg << "\n" << "Scan rate must be at least 1000 lpn";
}



void  OperatingParameters::WriteXML (ostream&  o)  const
{
  o << "<OperatingParameters>"  << endl;
  WriteFieldValues (o);
  o << "</OperatingParameters>"  << endl;
}  /* WriteXML */



void  OperatingParameters::WriteXML (const KKStr&  varName,
                                     ostream&      o
                                    )  const
{
  XmlTag  startTag ("OperatingParameters",  XmlTag::TagTypes::tagStart);
  if  (!varName.Empty ())
    startTag.AddAtribute ("VarName", varName);
  startTag.WriteXML (o);

  WriteFieldValues (o);

  XmlTag  endTag ("OperatingParameters", XmlTag::TagTypes::tagEnd);
  endTag.WriteXML (o);
  o << endl;
}  /* WriteXML */



void  OperatingParameters::ReadXML (istream&  i)
{
  bool  eol = false;
  bool  eof = false;
  bool  fieldFound = false;

  while (!eof)
  {
    KKStr  fieldName = osReadNextToken (i, "\t", eof, eol);
    if  (eof)
      break;

    KKStr  restOfLine = "";
    if  (!eol)
      restOfLine = osReadRestOfLine (i, eof);
    
    if  (fieldName.EqualIgnoreCase ("</OperatingParameters>"))
      break;

    if  (!fieldName.StartsWith ("//"))
      UpdateFromDataField (fieldName, restOfLine, fieldFound);
  }
}  /*   ReadXML  */




void  OperatingParameters::ReadXML (XmlStream&      s,
                                    XmlTagConstPtr  tag,
                                    RunLog&         log
                                   )
{
  KKStr  svmParametersStr;
  XmlTokenPtr  t = s.GetNextToken (log);
  while  (t)
  {
    if  (t->TokenType () == XmlToken::TokenTypes::tokContent)
    {
      XmlContentPtr c = dynamic_cast<XmlContentPtr> (t);
      if  (c  &&  c->Content ())
      {
        KKStrParser parser (*(c->Content ()));
        parser.TrimWhiteSpace (" ");

        KKStr  fieldName  = parser.GetNextToken ("\t\n\r");
        KKStr  fieldValue = parser.GetNextToken ("\t\n\r");
        bool  fieldFound = false;

        if  (!fieldName.StartsWith ("//"))
          UpdateFromDataField (fieldName, fieldValue, fieldFound);
      }
    }
    delete  t;
    t = s.GetNextToken (log);
  }
}  /* ReadXML */

 

XmlFactoryMacro(OperatingParameters)
