/* LarcosDataRec.cpp -- Represents the operating parameters used by Larcos; ex's:  BackGroundThreshold, FlowRateFactor.
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


#include "Configuration.h"
#include "KKBaseTypes.h"
#include "OSservices.h"
using namespace KKB;


#include "ScannerFile.h"
using namespace  KKLSC;

#include "LarcosDataRec.h"
#include "LarcosTrainingConfiguration.h"
using  namespace  LarcosCounterUnManaged;



LarcosDataRec::LarcosDataRec (const KKStr&   _scannerFilePrefix,
                              const KKStr&   _sectionName
                             ):
    scannerFilePrefix (_scannerFilePrefix),
    sectionName       (_sectionName)
{
}


LarcosDataRec::LarcosDataRec (const LarcosDataRec&   _larcosDataRec):
    scannerFilePrefix (_larcosDataRec.scannerFilePrefix),
    sectionName       (_larcosDataRec.sectionName)
{
}



    
LarcosDataRec::~LarcosDataRec ()
{
}



int  LarcosDataRec::MemoryConsumedEstimated ()  const
{
  return sizeof (*this);
}



KKStrConstPtr LarcosDataRec::GetSettingValue (const LarcosTrainingConfiguration&  c,
                                              const KKStr&                        fieldName
                                             )
{
  kkint32  lineNum = -1;
  KKStrConstPtr  x = c.SettingValue (sectionName, fieldName, lineNum);
  return x;
}



void  LarcosDataRec::UpdateFromConfigurationUchar (const LarcosTrainingConfiguration&  c,
                                                   const KKStr&                        fieldName,
                                                   uchar&                              fieldValue
                                                  )
{
  KKStrConstPtr  x = GetSettingValue (c, fieldName);
  if  (x)
    fieldValue = (uchar)x->ToInt ();
}




void  LarcosDataRec::UpdateFromConfigurationInt32 (const LarcosTrainingConfiguration&  c,
                                                   const KKStr&                        fieldName,
                                                   kkint32&                            fieldValue
                                                  )
{
  KKStrConstPtr  x = GetSettingValue (c, fieldName);
  if  (x)
    fieldValue = x->ToInt32 ();
}



void  LarcosDataRec::UpdateFromConfigurationFloat (const LarcosTrainingConfiguration&  c,
                                                   const KKStr&                        fieldName,
                                                   float&                              fieldValue
                                                  )
{
  KKStrConstPtr  x = GetSettingValue (c, fieldName);
  if  (x)
    fieldValue = x->ToFloat ();
}



void  LarcosDataRec::UpdateFromConfigurationBool (const LarcosTrainingConfiguration&  c,
                                                  const KKStr&                        fieldName,
                                                  bool&                               fieldValue
                                                 )
{
  KKStrConstPtr  x = GetSettingValue (c, fieldName);
  if  (x)
    fieldValue = x->ToBool ();
}



void  LarcosDataRec::UpdateFromConfigurationKKStr (const LarcosTrainingConfiguration&  c,
                                                   const KKStr&                        fieldName,
                                                   KKStr&                              fieldValue
                                                  )
{
  KKStrConstPtr  x = GetSettingValue (c, fieldName);
  if  (x)
    fieldValue = *x;
}




void  LarcosDataRec::UpdateFromConfigurationFileFormat (const LarcosTrainingConfiguration&  c,
                                                        const KKStr&                        fieldName,
                                                        ScannerFile::ScannerFileFormat&     fieldValue
                                                       )
{
  KKStrConstPtr  x = GetSettingValue (c, fieldName);
  if  (x)
  {
    ScannerFile::ScannerFileFormat t = ScannerFile::ScannerFileFormatFromStr (*x);
    if  (t != ScannerFile::sfUnKnown)
      fieldValue = t;
  }
}



const KKStr&  LarcosDataRec::GetScannerFileValue (ScannerFilePtr  sf, 
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




DateTime  LarcosDataRec::GetScannerFileValueDateTime (ScannerFilePtr  sf, 
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




kkint32  LarcosDataRec::GetScannerFileValueInt32 (ScannerFilePtr  sf, 
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



float  LarcosDataRec::GetScannerFileValueFloat (ScannerFilePtr  sf, 
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





bool  LarcosDataRec::GetScannerFileValueBool (ScannerFilePtr  sf, 
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



KKStr  LarcosDataRec::GetScannerFileValueKKStr (ScannerFilePtr  sf, 
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



const char*  LarcosDataRec::BoolToStr (bool b)  const
{
  return  (b ? "True" : "False");
}

