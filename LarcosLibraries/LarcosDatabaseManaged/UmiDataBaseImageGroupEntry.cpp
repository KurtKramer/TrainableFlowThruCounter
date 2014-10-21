#include  "StdAfx.h"
#include  "FirstIncludes.h"

#include  <stdio.h>
#include  <math.h>
#include  <ctype.h>
#include  <time.h>
#include  <fstream>
#include  <iostream>
#include  <map>
#include  <ostream>
#include  <string>
#include  <vector>
using namespace std;

#include  "MemoryDebug.h"
#include  "KKBaseTypes.h"
#include  "Raster.h"
using namespace  KKB;

#include  "UmiDataBaseImageGroupEntry.h"
using namespace  LarcosDatabaseManaged;


using namespace System;
using namespace System::Windows::Forms;




UmiDataBaseImageGroupEntry::UmiDataBaseImageGroupEntry ()
{
  entry = new DataBaseImageGroupEntry (-1, "");
  GC::AddMemoryPressure (40);
}




UmiDataBaseImageGroupEntry::UmiDataBaseImageGroupEntry (DataBaseImageGroupEntryPtr  _entry):  
    entry (new DataBaseImageGroupEntry (*_entry))  
{
  GC::AddMemoryPressure (40);
}



UmiDataBaseImageGroupEntry::!UmiDataBaseImageGroupEntry ()
{
  delete  entry;
  entry = NULL;
  GC::RemoveMemoryPressure (40);
}



UmiDataBaseImageGroupEntry::~UmiDataBaseImageGroupEntry ()
{
  this->!UmiDataBaseImageGroupEntry ();
}



int  UmiDataBaseImageGroupEntry::ImageGroupId::get ()
{
  if  (!entry)
    return -1;
  else
    return entry->ImageGroupId ();
}



void  UmiDataBaseImageGroupEntry::ImageGroupId::set (int groupId)
{
  if  (!entry)
    entry = new DataBaseImageGroupEntry  (groupId, "");
  else
    entry->ImageGroupId (groupId);
}



String^  UmiDataBaseImageGroupEntry::ImageFileName::get ()
{
  if  (!entry)
    return String::Empty;
  else
    return UmiKKStr::KKStrToSystenStr (entry->ImageFileName ());
}



void  UmiDataBaseImageGroupEntry::ImageFileName::set (String^  imageFileName)
{
  if  (!entry)
    entry = new DataBaseImageGroupEntry  (-1, UmiKKStr::SystemStringToKKStr (imageFileName));
  else
    entry->ImageFileName (UmiKKStr::SystemStringToKKStr (imageFileName));
}





UmiDataBaseImageGroupEntryList::UmiDataBaseImageGroupEntryList ()
{
}




UmiDataBaseImageGroupEntryList::UmiDataBaseImageGroupEntryList (DataBaseImageGroupEntryListPtr  entries)
{
  if  (!entries)
    return;

  // If the provided list is not the owner of its contentes;  then we will need to create new instances.
  bool  createNewInstance = !(entries->Owner ());

  DataBaseImageGroupEntryList::iterator  idx;
  for  (idx = entries->begin ();  idx != entries->end ();  idx++)
  {
    DataBaseImageGroupEntryPtr  entry = *idx;
    if  (createNewInstance)
      entry = new DataBaseImageGroupEntry (**idx);
    Add (gcnew UmiDataBaseImageGroupEntry (entry));
  }
  entries->Owner (false);
}




UmiDataBaseImageGroupEntryList::!UmiDataBaseImageGroupEntryList ()
{
  Clear ();
}



UmiDataBaseImageGroupEntryList::~UmiDataBaseImageGroupEntryList ()
{
  this->!UmiDataBaseImageGroupEntryList ();
}
