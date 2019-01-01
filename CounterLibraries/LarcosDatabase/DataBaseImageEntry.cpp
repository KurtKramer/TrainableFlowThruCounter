#include  "FirstIncludes.h"
#include  <stdlib.h>
#include  <memory.h>

#include  <string>
#include  <iostream>
#include  <fstream>
#include  <vector>
#include  <map>

#include  "MemoryDebug.h"

using namespace std;

#include  "KKBaseTypes.h"
#include  "OSservices.h"
#include  "Str.h"
using namespace KKB;


#include  "DataBaseImageEntry.h"
using namespace  LarcosDatabase;


DataBaseImageEntry::DataBaseImageEntry (const KKStr&  _imageFileName,
                                        float         _depth
                                        ):
    imageFileName (_imageFileName),
    depth         (_depth)
{
}



DataBaseImageEntry::~DataBaseImageEntry ()
{
}




DataBaseImageEntryList::DataBaseImageEntryList (bool _owner):
      KKQueue<DataBaseImageEntry> (_owner)
{
}


DataBaseImageEntryList::~DataBaseImageEntryList ()
{
}


