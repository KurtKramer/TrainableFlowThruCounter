#include  "FirstIncludes.h"
#include  <stdlib.h>
#include  <memory.h>

#include  <string>
#include  <iostream>
#include  <fstream>
#include  <vector>
#include  <map>
#include "MemoryDebug.h"

using namespace std;

#include "KKBaseTypes.h"
#include "RunLog.h"
using namespace KKB;

#include "DataBaseImageGroupEntry.h"
#include "DataBaseImage.h"
using namespace  LarcosDatabase;



DataBaseImageGroupEntry::DataBaseImageGroupEntry (const DataBaseImageGroupEntry&  imageGroupEntry):
  imageGroupId  (imageGroupEntry.imageGroupId),
  imageFileName (imageGroupEntry.imageFileName)
{
}


DataBaseImageGroupEntry::DataBaseImageGroupEntry (int           _imageGroupId,
                                                  const KKStr&  _imageFileName
                                                 ):
  imageGroupId  (_imageGroupId),
  imageFileName (_imageFileName)
{
}


DataBaseImageGroupEntry::~DataBaseImageGroupEntry ()
{
}


DataBaseImageGroupEntryList::DataBaseImageGroupEntryList (bool _owner):
  KKQueue<DataBaseImageGroupEntry> (_owner)
{
}




DataBaseImageGroupEntryList::DataBaseImageGroupEntryList (const  DataBaseImageGroupEntryList&  _dataBaseImageGroupEntryList):
  KKQueue<DataBaseImageGroupEntry> (_dataBaseImageGroupEntryList)
{
}



DataBaseImageGroupEntryList::DataBaseImageGroupEntryList (int                         groupId,
                                                          const DataBaseImageListPtr  images
                                                         ):
  KKQueue<DataBaseImageGroupEntry> (true)
{
  if  (!images)
    return;

  DataBaseImageList::const_iterator  idx;
  for  (idx = images->begin ();  idx != images->end ();  idx++)
    PushOnBack (new DataBaseImageGroupEntry (groupId, (*idx)->ImageFileName ()));
}




DataBaseImageGroupEntryList::~DataBaseImageGroupEntryList ()
{
}



