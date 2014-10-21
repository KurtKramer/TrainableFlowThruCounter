#include "StdAfx.h"
#include "FirstIncludes.h"

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
#include "UmiKKStr.h"
#include "UmiOSservices.h"


using namespace  LarcosCounterManaged;



using namespace System;
using namespace System::IO;
using namespace KKB;
using namespace System::Windows::Forms;



UmiOSservices::UmiOSservices (void)
{
}



String^  UmiOSservices::AddSlash (String^ s)
{
  if  (s->Length == 0)
    return  String::Empty;
      
  char lastCh = (char)s[s->Length - 1];
  if   ((lastCh == '/')  ||  (lastCh == '\\'))
    return  s;
        
  return  (s + "\\");
}   /* AddSlash */



bool  UmiOSservices::DirectoryExists (String^  dirName)
{
  bool  directoryExists = osValidDirectory (UmiKKStr::SystemStringToKKStr (dirName));
  return  directoryExists;
}  /* DirectoryExists */



bool  UmiOSservices::FileExists (String^ fileName)
{
  return  File::Exists (fileName);
}  /* FileExists */




bool   UmiOSservices::CreateDirectory (String^  dirName)
{
  bool  created = osCreateDirectory (UmiKKStr::SystemStringToKKStr (dirName));
  return  created;
}  /* CreateDirectory */



bool   UmiOSservices::CreateDirectoryPath (String^  _pathName)  // Will create the whole Directory Path,
{                                                            // not just the final part of the path.

  return  osCreateDirectoryPath (UmiKKStr::SystemStringToKKStr (_pathName));
}




long  UmiOSservices::HexStrToLong (String^  argument)
{
  unsigned __int64  uiHex = 0;

  try
  {
    // Convert hex string to unsigned integer
    uiHex = System::Convert::ToUInt64 (argument, 16);
    // Format it and show as a string				
  }
  catch (Exception^ exception)
  {
    // Show overflow message and return
    MessageBox::Show ("Overflow\n" + exception->ToString ());
  }

  return  (long)uiHex;
}  /* HexStrToLong */







void  UmiOSservices::RenameFile (String^ oldFileName,
                                 String^ newFileName
                                )
{
  // First delete new file if it exists; to get it out of the way
  try
  {
    FileInfo^ f = gcnew FileInfo (newFileName);
    if  (f->Exists)
    {
      f->Delete ();
    }
  }
  catch (Exception^ e1)
  {
    MessageBox::Show ("Error trying to rename '" + 
                      oldFileName + 
                      "'" +
                      "to '" + 
                      newFileName +
                      "'  " + 
                      e1->ToString ()
                     );
  }

  // We can now rename the oldFileName to the newFileName
  try
  {
    FileInfo^ f = gcnew FileInfo (oldFileName);
    if (f->Exists)
    {
      f->MoveTo (newFileName);
    }
  }
  catch (Exception^ e1)
  {
    MessageBox::Show ("Error trying to rename '" + oldFileName + "'" +
                      "to '" + newFileName +"'  " + e1->ToString ()
                     );
  }
}  /* RenameFile */



String^  UmiOSservices::RemoveExtension (String^  fileName)
{
  return  UmiKKStr::KKStrToSystenStr (osRemoveExtension (UmiKKStr::SystemStringToKKStr (fileName)));
}  /* RemoveExtension */





String^ UmiOSservices::GetRootName (String^ fileName)
{
  return  UmiKKStr::KKStrToSystenStr (osGetRootName (UmiKKStr::SystemStringToKKStr (fileName)));
}


String^   UmiOSservices::GetRootNameWithExtension (String^  fullFileName)
{
  return  UmiKKStr::KKStrToSystenStr (osGetRootNameWithExtension (UmiKKStr::SystemStringToKKStr (fullFileName)));
}  /* GetRootNameWithExtension */



String^  UmiOSservices::GetFileExtension (String^  fullFileName)
{
  return  UmiKKStr::KKStrToSystenStr (osGetFileExtension (UmiKKStr::SystemStringToKKStr (fullFileName)));
}




String^  UmiOSservices::GetPathPartOfFile (String^  fullFileName)
{
  return UmiKKStr::KKStrToSystenStr (osGetPathPartOfFile (UmiKKStr::SystemStringToKKStr (fullFileName)));
}




String^   UmiOSservices::GetParentDirectoryOfDirPath (String^ path)
{
  KKStr  s = UmiKKStr::SystemStringToKKStr (path);
  KKStr  parentDir = osGetParentDirectoryOfDirPath (s);
  return  UmiKKStr::KKStrToSystenStr (parentDir);
}




String^   UmiOSservices::GetRootNameOfDirectory (String^ path)
{
  // Strating with last chracter in string searcgh back until we find a seperating
  // character such as a slash or ":".

  KKStr  s = UmiKKStr::SystemStringToKKStr (path);
  KKStr  rootName = osGetRootNameOfDirectory (s);
  return  UmiKKStr::KKStrToSystenStr (rootName);
}




KKB::DateTime  UmiOSservices::SystemDateTimeToKKuDateTime (System::DateTime  sytemDateTime)
{
  return  KKB::DateTime   (sytemDateTime.Year,
                           sytemDateTime.Month,
                           sytemDateTime.Day,
                           sytemDateTime.Hour,
                           sytemDateTime.Minute,
                           sytemDateTime.Second
                          );
}



System::DateTime  UmiOSservices::KKuDateTimeToSystemDateTime (const KKB::DateTime&  dateTime)
{
  const DateType&  d (dateTime.Date ());
  const TimeType&  t (dateTime.Time ());


  System::DateTime dt;
  if  (d.Year () < 1)
  {
    return System::DateTime (1900, 1, 1);
  }

  try 
  {
    dt = System::DateTime (d.Year (), d.Month (), d.Day (), t.Hour (), t.Minute (), t.Second ());
  }
  catch  (Exception^  )
  {
    dt = System::DateTime (1900, 1, 1);
  }

  return  dt;

  //if  ((d.Year () < 1980)  ||  (d.Month < 1)  ||  (d.Month > 12)  ||  (d.Day < 1)  || (d.Day > 31))
  //  return  System::DateTime (0, 1, 1);
  //
  //return  System::DateTime (d.Year (), d.Month (), d.Day (), t.Hour (), t.Minute (), t.Second ());
}


String^  UmiOSservices::LookForFile (String^  fileName,
                                     String^  srcDir
                                    )
{
  if  ((fileName == nullptr)  ||  (srcDir == nullptr))
    return nullptr;

  KKStr  location = osLookForFile (UmiKKStr::SystemStringToKKStr (fileName), UmiKKStr::SystemStringToKKStr (srcDir));
  return  UmiKKStr::KKStrToSystenStr (location);
}  /* LookForFile */




array<String^>^   UmiOSservices::GetListOfFiles (String^  dirPath,
                                              String^  fileSpec
                                             )
{
  DirectoryInfo^ di = nullptr;
  try
  {
    di = gcnew DirectoryInfo(dirPath);
  }
  catch (Exception ^)
  {
    di = nullptr;
  }

  if  (di == nullptr)
    return  nullptr;


  array<FileInfo^>^  files = di->GetFiles(fileSpec);
  if  (files == nullptr)
    return  nullptr;

  array<String^>^  results = gcnew array<String^> (files->Length);
  for  (int  x = 0; x < files->Length;  x++)
  {
    results[x] = files[x]->Name;
  }

  return  results;
}  /* GetListOfFiles */




