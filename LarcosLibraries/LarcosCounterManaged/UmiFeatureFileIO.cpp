#include  "StdAfx.h"
#include  "FirstIncludes.h"

#include <stdio.h>
#include <math.h>

#include <ctype.h>
#include <time.h>

#include <fstream>
#include <iostream>
#include <ostream>
#include <string>
#include <vector>
#include "MemoryDebug.h"
using namespace std;

#include "KKBaseTypes.h"
#include "..\\KKBase\\GoalKeeper.h"
#include "OSservices.h"

using namespace KKB;

#include "FeatureFileIo.h"
#include "FeatureVector.h"
#include "FeatureFileIOKK.h"
#include "FileDesc.h"
#include "MLClass.h"
using namespace  KKMLL;


#include "UmiKKStr.h"
#include "UmiRaster.h"
#include "UmiRunLog.h"
#include "UmiFeatureVector.h"
#include "UmiFeatureVectorList.h"
#include "UmiFeatureFileIO.h"
using namespace  LarcosCounterManaged;


using namespace System;
using namespace System::Threading;
using namespace System::Windows::Forms;




UmiFeatureFileIO::UmiFeatureFileIO (String^  driverName):
    cancelLoad          (NULL),
    changesMade         (NULL),
    driver              (NULL),
    lastError           (nullptr),
    numExamplesWritten  (NULL),
    successful          (NULL)
{
  if  (String::IsNullOrEmpty (driverName))
    driverName = "KK";

  driver = FeatureFileIO::FileFormatFromStr (UmiKKStr::SystemStringToKKStr (driverName));
  cancelLoad          = new bool;
  changesMade         = new bool;
  numExamplesWritten  = new kkuint32;
  successful          = new bool;

  *cancelLoad         = false;
  *changesMade        = false;
  *numExamplesWritten = 0;
  *successful = (driver != NULL);
}



UmiFeatureFileIO::~UmiFeatureFileIO ()
{
  this->!UmiFeatureFileIO ();
}



UmiFeatureFileIO::!UmiFeatureFileIO ()
{
  delete  cancelLoad;          cancelLoad         = NULL;
  delete  changesMade;         changesMade        = NULL;
  delete  numExamplesWritten;  numExamplesWritten = NULL;
  delete  successful;          successful         = NULL;
}



array<String^>^  UmiFeatureFileIO::RegisteredDriverNames (bool  canRead,
                                                          bool  canWrite
                                                         )
{
  VectorKKStr  names = FeatureFileIO::RegisteredDriverNames (canRead, canWrite);

  array<String^>^  results = gcnew array<String^> (names.size ());
  
  kkuint32  idx;
  for  (idx = 0;  idx < names.size ();  idx++)
  {
    results->SetValue (UmiKKStr::KKStrToSystenStr (names[idx]), (int)idx);
  }

  return  results;
}  /* RegisteredDriverNames */ 





bool  UmiFeatureFileIO::ValidDriverName (String^ _driverName)
{
  KKStr driverName = UmiKKStr::SystemStringToKKStr (_driverName);

  return  (FeatureFileIO::FileFormatFromStr (driverName) != NULL);
}




bool    UmiFeatureFileIO::CancelLoad::get ()  
{
  if  (cancelLoad)
    return  *cancelLoad;
  else
    return  false;
}




void UmiFeatureFileIO::CancelLoad::set (bool _cancelLoad)
{
  if  (!cancelLoad)
    cancelLoad = new bool;
  *cancelLoad = _cancelLoad;
}




bool  UmiFeatureFileIO::CanRead::get ()
{
  if  (!driver)
    return false;
  return driver->CanRead ();
}



bool  UmiFeatureFileIO::CanWrite::get ()
{
  if  (!driver)
    return false;
  return driver->CanWrite ();
}



bool    UmiFeatureFileIO::ChangesMade::get ()
{
  if  (changesMade)
    return *changesMade;
  else
    return  false;
}



String^  UmiFeatureFileIO::DriverName::get ()
{
  if  (driver)
    return  UmiKKStr::KKStrToSystenStr (driver->DriverName ());
  else
    return String::Empty;
}


kkuint32  UmiFeatureFileIO::NumExamplesWritten::get ()
{
  if  (numExamplesWritten)
    return  *numExamplesWritten;
  else
    return 0;
}



bool  UmiFeatureFileIO::Successful::get ()
{
  if  (successful)
    return  *successful;
  else
    return  false;
}




bool  UmiFeatureFileIO::Valid::get ()
{
  return  (driver != NULL);
}




void  UmiFeatureFileIO::SaveFeatureFile (String^                _fileName, 
                                         UmiFeatureVectorList^  _examples,
                                         UmiRunLog^             _runLog
                                        )
{
  if  (!driver)
  {
    _runLog->WriteLn (10, "UmiFeatureFileIO::SaveFeatureFile      Invalid Driver");
    *successful = false;
    return;
  }

  if  (!(driver->CanWrite ()))
  {
    _runLog->WriteLn (10, "UmiFeatureFileIO::SaveFeatureFile      Driver["    + 
                      UmiKKStr::KKStrToSystenStr (driver->DriverName ())  +
                      "] does not support reading");
    *successful = false;
    return;
  }

  _runLog->WriteLn (20, "UmiFeatureFileIO::SaveFeatureFile   FileName: " + _fileName);

  FeatureVectorListPtr  data = _examples->ToFeatureVectorList (_runLog);
  driver->SaveFeatureFile (UmiKKStr::SystemStringToKKStr (_fileName), 
                           data->AllFeatures (), 
                           *data, 
                           *numExamplesWritten, 
                           *cancelLoad, 
                           *successful, 
                           _runLog->Log ()
                          );

  delete  data;
  data = NULL;
}



UmiFeatureVectorList^  UmiFeatureFileIO::LoadFeatureFile (String^     _fileName, 
                                                          UmiClass^   _classes, 
                                                          UmiRunLog^  _runLog
                                                         )
{
  if  (!driver)
  {
    lastError = "Invalid Driver";
    _runLog->WriteLn (10, "UmiFeatureFileIO::LoadFeatureFile  " + lastError);
    *successful = false;
    return  nullptr;
  }

  if  (!(driver->CanWrite ()))
  {
    lastError = "Driver[" + UmiKKStr::KKStrToSystenStr (driver->DriverName ()) + "] does not support writing.";

    _runLog->WriteLn (10, "UmiFeatureFileIO::SaveFeatureFile  " + lastError);
    *successful = false;
    return  nullptr;
  }

  _runLog->WriteLn (20, "UmiFeatureFileIO::LoadFeatureFile   FileName: " + _fileName);

  MLClassListPtr  classes = new MLClassList ();
  FeatureVectorListPtr  data = driver->LoadFeatureFile (UmiKKStr::SystemStringToKKStr (_fileName), 
                                                        *classes,
                                                        -1,
                                                        *cancelLoad,
                                                        *successful,
                                                        *changesMade,
                                                        _runLog->Log ()
                                                       );
  if  (!data)
  {
    lastError = "Driver[" + UmiKKStr::KKStrToSystenStr (driver->DriverName ()) + "] LoadFeatureFile did not return any data.";
    _runLog->WriteLn (10, "UmiFeatureFileIO::SaveFeatureFile  " + lastError);
    *successful = false;
    return nullptr;
  }

  if  (!(*successful))
  {
    lastError = "Driver[" + UmiKKStr::KKStrToSystenStr (driver->DriverName ()) + "] LoadFeatureFile Not Successful.";
    _runLog->WriteLn (10, "UmiFeatureFileIO::SaveFeatureFile  " + lastError);
    delete  data;
    return nullptr;
  }

  UmiFeatureVectorList^  results = gcnew UmiFeatureVectorList (*data);
  data->Owner (false);
  delete  data;  data = NULL;

  return  results;
}
