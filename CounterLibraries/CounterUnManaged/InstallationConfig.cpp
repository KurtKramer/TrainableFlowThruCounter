/* InstallationConfig.cpp -- Represents a single Installation of Larcos
 * Copyright (C) 2011-2013  Kurt Kramer
 * For conditions of distribution and use, see copyright notice in CounterUnManaged.txt
 */
#include "FirstIncludes.h"

#include <errno.h>
#include <fstream>
#include <istream>
#include <iostream>
#include <vector>
#include "MemoryDebug.h"
using namespace std;


#include "KKBaseTypes.h"
#include "KKStrParser.h"
#include "OSservices.h"
using namespace KKB;


#include "ScannerFile.h"
#include "ScannerHeaderFields.h"
using namespace  KKLSC;

#include "CounterVariables.h"
using namespace  CounterBase;



#include "InstallationConfig.h"
using  namespace  CounterUnManaged;


KKStr  InstallationConfig::flowMeterMethodStrs[] = {"NULL", "Embedded", "EndOfList"};



const KKStr&  InstallationConfig::FlowMeterMethodToStr (FlowMeterMethods  method)
{
  if  ((method < FlowMeterMethods::Null)  ||  (method >= FlowMeterMethods::EndOfList ))
    return KKStr::EmptyStr();
  else
    return flowMeterMethodStrs[(int)method];
}



InstallationConfig::FlowMeterMethods  InstallationConfig::FlowMeterMethodFromStr (const KKStr&  s)
{
  FlowMeterMethods  method = FlowMeterMethods::Null;
  while  (method < FlowMeterMethods::EndOfList)
  {
    if  (s.EqualIgnoreCase (flowMeterMethodStrs[(int)method]))
      break;
    method = (FlowMeterMethods)((int)method + 1);
  }
  if  (method >= FlowMeterMethods::EndOfList)
    return FlowMeterMethods::Null;
  else
    return method;
}



InstallationConfig::InstallationConfig (RunLog&  runLog):
    CounterDataRec ("Installation", "LarcosInstallation"),
    description               (),
    flowMeterMethod           (FlowMeterMethods::Null),
    flowMeterTicsPerMeter     (0.0f),
    imagingChamberWidth       (0.0f),
    imagingChamberWidthPixels (),
    cameraMacAddress          (),
    name                      ()
{
  runLog.Level (40) << "InstallationConfig::InstallationConfig   Empty instance created." << endl;
}



InstallationConfig::InstallationConfig (const InstallationConfig&  _entry,
                                        RunLog&                    _runLog
                                       ):
    CounterDataRec ("Installation", "LarcosInstallation"),
    description               (_entry.description),
    flowMeterMethod           (_entry.flowMeterMethod),
    flowMeterTicsPerMeter     (_entry.flowMeterTicsPerMeter),
    imagingChamberWidth       (_entry.imagingChamberWidth),
    imagingChamberWidthPixels (_entry.imagingChamberWidthPixels),
    cameraMacAddress          (_entry.cameraMacAddress),
    name                      (_entry.name)
{
  _runLog.Level (40) << "InstallationConfig::InstallationConfig   Copy Constructor  Name: " << name << endl;
}



InstallationConfig::InstallationConfig (const KKB::KKStr&  _name,
                                        bool&              _successful,
                                        RunLog&            _log
                                       ):
    CounterDataRec ("Installation", "LarcosInstallation"),
    description               (),
    flowMeterMethod           (FlowMeterMethods::Null),
    flowMeterTicsPerMeter     (0.0f),
    imagingChamberWidth       (0.0f),
    imagingChamberWidthPixels (2048),
    cameraMacAddress          (),
    name                      ()
{
  _log.Level (10) << "InstallationConfig::InstallationConfig   Create from file;  Name: " << name << endl;
  Load (_name, _successful, _log);
}





InstallationConfig::~InstallationConfig ()
{
}



const KKStr&  InstallationConfig::FlowMeterMethodStr () const
{
  return FlowMeterMethodToStr (flowMeterMethod);
}


void  InstallationConfig::AddToHeaderFields (ScannerHeaderFieldsPtr  headerFields)
{
  headerFields->Add ("Installation:Description",                 description);
  headerFields->Add ("Installation:FlowMeterMethod",             FlowMeterMethodStr ());
  headerFields->Add ("Installation:FlowMeterTicsPerMeter",       flowMeterTicsPerMeter);
  headerFields->Add ("Installation:ImagingChamberWidth",         imagingChamberWidth);
  headerFields->Add ("Installation:ImagingChamberWidthPixels",   imagingChamberWidthPixels);
  headerFields->Add ("Installation:CameraMacAddress",            cameraMacAddress);
  headerFields->Add ("Installation:Name",                        name);
}






void  InstallationConfig::UpdateFromScannerFile (ScannerFilePtr  sf)
{
  if  (sf == NULL)
    return;

  KKStr  flowMeterMethodStr = FlowMeterMethodStr ();

  cameraMacAddress          = GetScannerFileValueKKStr (sf, "CameraMacAddress",          cameraMacAddress);
  description               = GetScannerFileValueKKStr (sf, "Description",               description);
  flowMeterMethodStr        = GetScannerFileValueKKStr (sf, "FlowMeterMethod",           flowMeterMethodStr);
  flowMeterTicsPerMeter     = GetScannerFileValueFloat (sf, "FlowMeterTicsPerMeter",     flowMeterTicsPerMeter);
  imagingChamberWidth       = GetScannerFileValueFloat (sf, "ImagingChamberWidth",       imagingChamberWidth);
  imagingChamberWidthPixels = GetScannerFileValueInt32 (sf, "ImagingChamberWidthPixels", imagingChamberWidthPixels);
  name                      = GetScannerFileValueKKStr (sf, "Name",                      name);

  flowMeterMethod = FlowMeterMethodFromStr (flowMeterMethodStr);
}  /* UpdateFromScannerFile */
 



bool  InstallationConfig::FlowMeterPresent () const
{
  return  ((flowMeterMethod == FlowMeterMethods::Embedded));
}




kkMemSize  InstallationConfig::MemoryConsumedEstimated ()  const
{
  return  sizeof (*this) + description.MemoryConsumedEstimated ()  + name.MemoryConsumedEstimated ();
}


void  InstallationConfig::Assign (const InstallationConfig&   config)
{
  cameraMacAddress            = config.CameraMacAddress          ();
  description                 = config.Description               ();
  flowMeterMethod             = config.FlowMeterMethod           ();
  flowMeterTicsPerMeter       = config.FlowMeterTicsPerMeter     ();
  imagingChamberWidth         = config.ImagingChamberWidth       ();
  imagingChamberWidthPixels   = config.ImagingChamberWidthPixels ();
  name                        = config.Name                      ();
}



KKStr  InstallationConfig::installationDirPath = osAddSlash (CounterVariables::ConfigurationDir ()) + "Installations";


KKStr  InstallationConfig::InstallationDirPath ()
{
  return  installationDirPath;
}


KKStr  InstallationConfig::DeriveFullFleName (const KKStr& _name)
{
  return  osAddSlash (InstallationDirPath ()) + _name + ".cfg";
}




void  InstallationConfig::UpdateFromDataField (const KKStr&  fieldName,
                                               const KKStr&  fieldValue,
                                               bool&         fieldFound
                                              )
{
  fieldFound = true;

  if  (fieldName.EqualIgnoreCase ("CameraMacAddress"))
    cameraMacAddress = fieldValue;

  else if  (fieldName.EqualIgnoreCase ("Description"))
    description = fieldValue;

  else if  (fieldName.EqualIgnoreCase ("FlowMeterMethod"))
    flowMeterMethod = FlowMeterMethodFromStr (fieldValue);

  else if  (fieldName.EqualIgnoreCase ("FlowMeterTicsPerMeter"))
    flowMeterTicsPerMeter = fieldValue.ToFloat ();

  else if  (fieldName.EqualIgnoreCase ("ImagingChamberWidth"))
    imagingChamberWidth = fieldValue.ToFloat ();

  else if  (fieldName.EqualIgnoreCase ("ImagingChamberWidthPixels"))
    imagingChamberWidthPixels = fieldValue.ToInt32 ();

  else if  (fieldName.EqualIgnoreCase ("Name"))
    name = fieldValue;

  else
    fieldFound = false;
}



void  InstallationConfig::WriteFieldValues (ostream&   o)  const
{
  o << "CameraMacAddress"          << "\t" << cameraMacAddress           << endl
    << "Description"               << "\t" << description                << endl
    << "FlowMeterMethod"           << "\t" << FlowMeterMethodStr ()      << endl
    << "FlowMeterTicsPerMeter"     << "\t" << flowMeterTicsPerMeter      << endl
    << "ImagingChamberWidth"       << "\t" << imagingChamberWidth        << endl
    << "ImagingChamberWidthPixels" << "\t" << imagingChamberWidthPixels  << endl
    << "Name"                      << "\t" << name                       << endl;
}




void  InstallationConfig::WriteXML (ostream&  o)  const
{
  o << "<InstallationConfig>"  << endl;
  WriteFieldValues (o);
  o << "</InstallationConfig>"  << endl;
}  /* WriteXML */




void  InstallationConfig::ReadXML (istream&  i)
{
  bool  eol = false;
  bool  eof = false;
  bool  fieldFound = false;

  while (!eof)
  {
    KKStr  fieldName = osReadNextToken (i, "\t", eof, eol);
    if  (eof)
      break;

    KKStr restOfLine = "";
    if  (!eol)
      restOfLine = osReadRestOfLine2 (i, eof);
    
    if  (fieldName.EqualIgnoreCase ("</InstallationConfig>"))
      break;

    if  (!fieldName.StartsWith ("//"))
      UpdateFromDataField (fieldName, restOfLine, fieldFound);
  }
}  /*   ReadXML  */








void  InstallationConfig::Load (const KKStr&  _name,
                                bool&         _successful,
                                RunLog        _log
                               )
{
  _log.Level (30) << "InstallationConfig::Load   name: " << _name << endl;
  name = _name;
  _successful = false;
  KKStr  fullFileName = DeriveFullFleName (_name);
  FILE*  f = osFOPEN (fullFileName.Str (), "r");
  if  (f == NULL)
    return;

  KKStrPtr  l = osReadNextLine (f);
  while  (l != NULL)
  {
    if  ((!l->StartsWith ("//"))  &&  (l->Len () > 0))
    {
      KKStrParser  parser (*l);
      parser.TrimWhiteSpace (" \n\r");
      KKStr  fieldName  = parser.GetNextToken ("\t=");
      KKStr  fieldValue = parser.GetNextToken ("\t=");

      if  (!fieldName.Empty ())
      {
        if  (fieldName.EqualIgnoreCase ("EndOfInstallationConfig"))
          break;
 
        bool  fieldFound = false;
        UpdateFromDataField (fieldName, fieldValue, fieldFound);

        if  (!fieldFound)
        {
          _log.Level (-1) << "InstallationConfig::Load   Unknown FieldName: " << l << endl;
        }
      }
    }

    delete  l;
    l = osReadNextLine (f);
  }

  delete  l;
  l = NULL;

  _successful = true;

  _log.Level (50) << "InstallationConfig::Load   name                     : " << name                           << endl
                  << "                           description              : " << description                    << endl
                  << "                           flowMeterMethod          : " << FlowMeterMethodToStr (flowMeterMethod) << endl
                  << "                           flowMeterTicsPerMeter    : " << flowMeterTicsPerMeter          << endl
                  << "                           ImagingChamberWidth      : " << imagingChamberWidth            << endl
                  << "                           ImagingChamberWidthPixels: " << imagingChamberWidthPixels      << endl;

  fclose (f);
}  /* Load */



void  InstallationConfig::Save (RunLog& _log)  const
{
  osCreateDirectoryPath (InstallationDirPath ());

  KKStr  fullFileName = DeriveFullFleName (name);

  ofstream f (fullFileName.Str ());

  f << "//InstallationConfig  for " << name    << endl
    << "//SaveDate: " << osGetLocalDateTime () << endl
    << "//" << endl
    << endl;

  f << "Name"                      << "\t" << name                                    << endl
    << "Description"               << "\t" << description                             << endl
    << "FlowMeterMethod"           << "\t" << FlowMeterMethodToStr (flowMeterMethod)  << endl
    << "FlowMeterTicsPerMeter"     << "\t" << flowMeterTicsPerMeter                   << endl
    << "ImagingChamberWidth"       << "\t" << imagingChamberWidth                     << endl
    << "ImagingChamberWidthPixels" << "\t" << imagingChamberWidthPixels               << endl
    << "CameraMacAddress"          << "\t" << cameraMacAddress                        << endl;

  f << "EndOfInstallationConfig" << endl;
}  /* Save */


/*
  typedef  InstallationConfig*  InstallationConfigPtr;

  class  InstallationConfigList: public KKQueue<InstallationConfig>
  {
  public:
    typedef  InstallationConfigList* InstallationConfigListPtr;
*/

InstallationConfigList::InstallationConfigList (bool  _owner):
    KKQueue<InstallationConfig> (_owner)
{
}


InstallationConfigList::~InstallationConfigList ()
{
}

 
InstallationConfigListPtr  InstallationConfigList::GetListOfInstallationConfig (RunLog&  _log)
{
  KKStr  dirPath = osAddSlash (InstallationConfig::InstallationDirPath ()) + "*.cfg";

  KKStrListPtr  fileNames = osGetListOfFiles (dirPath);
  if  (!fileNames)
    return NULL;

  InstallationConfigListPtr  installations = new InstallationConfigList (true);

  KKStrList::iterator  idx;

  for  (idx = fileNames->begin ();  idx != fileNames->end ();  ++idx)
  {
    KKStrPtr  fn = *idx;

    bool  successful = false;

    InstallationConfigPtr  c = new InstallationConfig (*fn, successful, _log);

    installations->PushOnBack (c);
  }

  return  installations;
}  /* GetListOfInstallationConfig */
