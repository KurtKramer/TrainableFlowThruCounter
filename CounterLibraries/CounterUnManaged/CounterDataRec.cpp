/* CounterDataRec.cpp -- Represents the operating parameters used by Larcos; ex's:  BackGroundThreshold, FlowRateFactor.
 * Copyright (C) 2011-2013  Kurt Kramer
 * For conditions of distribution and use, see copyright notice in CounterUnManaged.txt
 */
#include "FirstIncludes.h"
#include <errno.h>
#include <istream>
#include <iostream>
#include <vector>
#include "MemoryDebug.h"
using namespace std;


#include "Configuration.h"
#include "KKBaseTypes.h"
#include "OSservices.h"
using namespace KKB;


#include "ScannerFile.h"
using namespace  KKLSC;

#include "CounterDataRec.h"
#include "CounterTrainingConfiguration.h"
using  namespace  CounterUnManaged;



CounterDataRec::CounterDataRec (const KKStr&   _scannerFilePrefix,
                              const KKStr&   _sectionName
                             ):
    scannerFilePrefix (_scannerFilePrefix),
    sectionName       (_sectionName)
{
}


CounterDataRec::CounterDataRec (const CounterDataRec&   _larcosDataRec):
    scannerFilePrefix (_larcosDataRec.scannerFilePrefix),
    sectionName       (_larcosDataRec.sectionName)
{
}



    
CounterDataRec::~CounterDataRec ()
{
}



kkMemSize  CounterDataRec::MemoryConsumedEstimated ()  const
{
  return sizeof (*this);
}



KKStrConstPtr CounterDataRec::GetSettingValue (const CounterTrainingConfiguration&  c,
                                               const KKStr&                        fieldName
                                              )
{
  OptionUInt32  lineNum = {};
  KKStrConstPtr  x = c.SettingValue (sectionName, fieldName, lineNum);
  return x;
}



void  CounterDataRec::UpdateFromConfigurationUchar (const CounterTrainingConfiguration&  c,
                                                   const KKStr&                        fieldName,
                                                   uchar&                              fieldValue
                                                  )
{
  KKStrConstPtr  x = GetSettingValue (c, fieldName);
  if  (x)
    fieldValue = (uchar)x->ToInt ();
}




void  CounterDataRec::UpdateFromConfigurationInt32 (const CounterTrainingConfiguration&  c,
                                                   const KKStr&                        fieldName,
                                                   kkint32&                            fieldValue
                                                  )
{
  KKStrConstPtr  x = GetSettingValue (c, fieldName);
  if  (x)
    fieldValue = x->ToInt32 ();
}



void  CounterDataRec::UpdateFromConfigurationFloat (const CounterTrainingConfiguration&  c,
                                                   const KKStr&                        fieldName,
                                                   float&                              fieldValue
                                                  )
{
  KKStrConstPtr  x = GetSettingValue (c, fieldName);
  if  (x)
    fieldValue = x->ToFloat ();
}



void  CounterDataRec::UpdateFromConfigurationBool (const CounterTrainingConfiguration&  c,
                                                  const KKStr&                        fieldName,
                                                  bool&                               fieldValue
                                                 )
{
  KKStrConstPtr  x = GetSettingValue (c, fieldName);
  if  (x)
    fieldValue = x->ToBool ();
}



void  CounterDataRec::UpdateFromConfigurationKKStr (const CounterTrainingConfiguration&  c,
                                                   const KKStr&                        fieldName,
                                                   KKStr&                              fieldValue
                                                  )
{
  KKStrConstPtr  x = GetSettingValue (c, fieldName);
  if  (x)
    fieldValue = *x;
}




void  CounterDataRec::UpdateFromConfigurationFileFormat (const CounterTrainingConfiguration&  c,
                                                        const KKStr&                        fieldName,
                                                        ScannerFile::Format&                fieldValue
                                                       )
{
  KKStrConstPtr  x = GetSettingValue (c, fieldName);
  if  (x)
  {
    ScannerFile::Format t = ScannerFile::ScannerFileFormatFromStr (*x);
    if  (t != ScannerFile::Format::sfUnKnown)
      fieldValue = t;
  }
}



const KKStr&  CounterDataRec::GetScannerFileValue (ScannerFilePtr  sf, 
                                                  const KKStr&    fieldName
                                                 )
{
  KKStr  fullName = scannerFilePrefix + ":" + fieldName;
  const KKStr& fv1 = sf->GetValue (fullName);
  if  (!fv1.Empty ())
    return fv1;

  const KKStr&  fv2 = sf->GetValue (fieldName);
  if  (!fv2.Empty ())
    return fv2;

  return  KKStr::EmptyStr ();
}




DateTime  CounterDataRec::GetScannerFileValueDateTime (ScannerFilePtr  sf, 
                                                      const KKStr&    fieldName,
                                                      DateTime        defaultValue
                                                     )
{

  const KKStr&  dateTimeStr = GetScannerFileValue (sf, fieldName);
  if  (dateTimeStr.Empty ())
    return defaultValue;
  else
    return DateTime (dateTimeStr);
}




kkint32  CounterDataRec::GetScannerFileValueInt32 (ScannerFilePtr  sf, 
                                                const KKStr&    fieldName,
                                                kkint32         curValue
                                               )
{
  const KKStr&  fv1 = GetScannerFileValue (sf, fieldName);
  if  (!fv1.Empty ())
    return  fv1.ToInt32 ();
  else
    return curValue;
}



float  CounterDataRec::GetScannerFileValueFloat (ScannerFilePtr  sf, 
                                                const KKStr&    fieldName,
                                                float           curValue
                                               )
{
  const KKStr&  fv1 = GetScannerFileValue (sf, fieldName);
  if  (!fv1.Empty ())
    return  fv1.ToFloat ();
  else
    return curValue;
}





bool  CounterDataRec::GetScannerFileValueBool (ScannerFilePtr  sf, 
                                              const KKStr&    fieldName,
                                              bool            curValue
                                             )
{
  const KKStr&  fv1 = GetScannerFileValue (sf, fieldName);
  if  (!fv1.Empty ())
    return  fv1.ToBool ();
  else
    return curValue;
}



KKStr  CounterDataRec::GetScannerFileValueKKStr (ScannerFilePtr  sf, 
                                                const KKStr&    fieldName,
                                                KKStr           curValue
                                               )
{
  const KKStr&  fv1 = GetScannerFileValue (sf, fieldName);
  if  (!fv1.Empty ())
    return  fv1;
  else
    return curValue;
}



const char*  CounterDataRec::BoolToStr (bool b)  const
{
  return  (b ? "True" : "False");
}

