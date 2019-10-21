#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Collections::Generic;
using namespace System::Diagnostics;
using namespace System::Drawing;
using namespace System::Windows::Forms;

#include  "DataBaseImageGroupEntry.h"
using namespace  KKMachineLearning;

#include  "UmiRunLog.h"
#include  "UmiKKStr.h"




namespace LarcosDatabaseManaged 
{
  public  ref class UmiDataBaseImageGroupEntry
  {
  public:
    UmiDataBaseImageGroupEntry ();
    UmiDataBaseImageGroupEntry (DataBaseImageGroupEntryPtr  _entry);

  protected:
    !UmiDataBaseImageGroupEntry ();

  private:
    ~UmiDataBaseImageGroupEntry ();

  public:
    property  int      ImageGroupId  {int      get ();   void  set (int      _groupId);}
    property  String^  ImageFileName {String^  get ();   void  set (String^  _name);}

    DataBaseImageGroupEntryPtr   UnManagedClass ()  {return entry;}

  private:
    DataBaseImageGroupEntryPtr  entry;
  };



  public  ref  class  UmiDataBaseImageGroupEntryList:  List<UmiDataBaseImageGroupEntry^>
  {
  public:
    UmiDataBaseImageGroupEntryList ();

    /// Will take ownership of contents of 'entries';  if entries does not own its contents then 
    /// will create new instances.  The entries->Owner () flag will be set to 'false' 
    UmiDataBaseImageGroupEntryList (DataBaseImageGroupEntryListPtr  entries);

  protected:
    !UmiDataBaseImageGroupEntryList ();

  private:
    ~UmiDataBaseImageGroupEntryList ();
  };  /* UmiDataBaseImageGroupEntryList */


};
