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

#include "MemoryDebug.h"

#include "KKBaseTypes.h"
#include "Raster.h"
#include "OSservices.h"
using namespace  KKB;


#include "UmiDataBaseImageGroup.h"
#include "UmiKKStr.h"


using namespace  LarcosDatabaseManaged;


using namespace System;
using namespace System::Windows::Forms;


UmiDataBaseImageGroup::UmiDataBaseImageGroup ():
   imageGroup (NULL)
{
  GC::AddMemoryPressure (200);
}


UmiDataBaseImageGroup::UmiDataBaseImageGroup (DataBaseImageGroupPtr  _imageGroup):
  imageGroup (new DataBaseImageGroup (*_imageGroup))
{
  GC::AddMemoryPressure (200);
}


UmiDataBaseImageGroup::UmiDataBaseImageGroup (int      _groupId,
                                                  String^  _name,
                                                  String^  _description,
                                                  uint     _count
                                                 ):
  imageGroup (NULL)
{
  imageGroup = new DataBaseImageGroup (_groupId,
                                       UmiKKStr::SystemStringToKKStr (_name),
                                       UmiKKStr::SystemStringToKKStr (_description),
                                       _count
                                      );
  GC::AddMemoryPressure (200);
}



  
UmiDataBaseImageGroup::!UmiDataBaseImageGroup ()
{
  delete  imageGroup;
  imageGroup = NULL;
  GC::RemoveMemoryPressure (200);
}


UmiDataBaseImageGroup::~UmiDataBaseImageGroup ()
{
  this->!UmiDataBaseImageGroup ();
}


int  UmiDataBaseImageGroup::ImageGroupId::get ()
{
  if  (!imageGroup)
    return -1;
  else
    return  imageGroup->ImageGroupId ();
}


void  UmiDataBaseImageGroup::ImageGroupId::set (int _groupId)
{
  if  (!imageGroup)
    imageGroup = new DataBaseImageGroup (_groupId, "DummyName_" + StrFormatInt (_groupId, "ZZZZZZZ0"), "", 0);
  else
    imageGroup->ImageGroupId (_groupId);
}



String^  UmiDataBaseImageGroup::Name::get ()
{
  if  (!imageGroup)
    return String::Empty;
  else
    return UmiKKStr::KKStrToSystenStr (imageGroup->Name ());
}




void   UmiDataBaseImageGroup::Name::set (String^  _name)
{
  if  (!imageGroup)
    imageGroup = new DataBaseImageGroup (-1, UmiKKStr::SystemStringToKKStr (_name), "", 0);
  else
    imageGroup->Name (UmiKKStr::SystemStringToKKStr (_name));
}



String^  UmiDataBaseImageGroup::Description::get ()
{
  if  (!imageGroup)
    return String::Empty;
  else
    return UmiKKStr::KKStrToSystenStr (imageGroup->Description ());
}




void   UmiDataBaseImageGroup::Description::set (String^  _description)
{
  if  (!imageGroup)
    imageGroup = new DataBaseImageGroup (-1, "", UmiKKStr::SystemStringToKKStr (_description), 0);
  else
    imageGroup->Description (UmiKKStr::SystemStringToKKStr (_description));
}



uint  UmiDataBaseImageGroup::Count::get ()
{
  if  (!imageGroup)
    return 0;
  else
    return  imageGroup->Count ();
}


void  UmiDataBaseImageGroup::Count::set (uint _count)
{
  if  (!imageGroup)
    imageGroup = new DataBaseImageGroup (-1, "DummyName_Count[" + StrFormatInt (_count, "ZZZZZZZ0") + "]", "", _count);
  else
    imageGroup->Count (_count);
}





void  UmiDataBaseImageGroup::ValidName (String^%  _name,
                                          String^%  _errorDesc,
                                          bool%     _valid
                                         )
{
  KKStr  name = UmiKKStr::SystemStringToKKStr (_name);
  KKStr  errorDesc = "";
  bool   valid = false;
  DataBaseImageGroup::ValidName (name, errorDesc, valid);

  _errorDesc = UmiKKStr::KKStrToSystenStr (errorDesc);
  _valid = valid;
}  /* ValidName */








UmiDataBaseImageGroupList::UmiDataBaseImageGroupList ()
{
}




UmiDataBaseImageGroupList::UmiDataBaseImageGroupList (DataBaseImageGroupListPtr  groups)
{
  if  (!groups)
    return;

  // If the provided list is not the owner of its contentes;  then we will need to create new instances.
  bool  createNewInstance = !(groups->Owner ());

  DataBaseImageGroupList::iterator  idx;
  for  (idx = groups->begin ();  idx != groups->end ();  idx++)
  {
    DataBaseImageGroupPtr  group = *idx;
    if  (createNewInstance)
      group = new DataBaseImageGroup (**idx);
    Add (gcnew UmiDataBaseImageGroup (group));
  }
  groups->Owner (false);
}




UmiDataBaseImageGroupList::!UmiDataBaseImageGroupList ()
{
  Clear ();
}



UmiDataBaseImageGroupList::~UmiDataBaseImageGroupList ()
{
  this->!UmiDataBaseImageGroupList ();
}
