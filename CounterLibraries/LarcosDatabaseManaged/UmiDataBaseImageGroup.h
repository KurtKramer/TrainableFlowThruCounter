#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Collections::Generic;
using namespace System::Diagnostics;
using namespace System::Drawing;
using namespace System::Windows::Forms;

#include  "DataBaseImageGroup.h"
using namespace  KKMachineLearning;


#include  "UmiRunLog.h"
#include  "UmiKKStr.h"


namespace LarcosDatabaseManaged 
{
  public  ref class UmiDataBaseImageGroup
  {
  public:
    UmiDataBaseImageGroup ();

    UmiDataBaseImageGroup (DataBaseImageGroupPtr  _imageGroup);  // Will take ownership of _imageGroup

    UmiDataBaseImageGroup (int      _groupId,
                             String^  _name,
                             String^  _description,
                             uint     _count
                            );


  protected:
    !UmiDataBaseImageGroup ();

  private:
    ~UmiDataBaseImageGroup (); 

  public:
    property  int        ImageGroupId  {int        get ();   void  set (int      _groupId);}
    property  String^    Name          {String^    get ();   void  set (String^  _name);}
    property  String^    Description   {String^    get ();   void  set (String^  _description);}
    property  uint       Count         {uint       get ();   void  set (uint     _count);}

    DataBaseImageGroupPtr  UnManagedClass ()  {return imageGroup;}

    static  void  ValidName (String^%  _name,
                             String^%  _errorDesc,
                             bool%     _valid
                            );

  private:
    DataBaseImageGroupPtr  imageGroup;
  };




  public  ref  class  UmiDataBaseImageGroupList:  List<UmiDataBaseImageGroup^>
  {
  public:
    UmiDataBaseImageGroupList ();

    /// Will take ownership of contents of 'entries';  if entries does not own its contents then 
    /// will create new instances.  The entries->Owner () flag will be set to 'false' 
    UmiDataBaseImageGroupList (DataBaseImageGroupListPtr  entries);

  protected:
    !UmiDataBaseImageGroupList ();

  private:
    ~UmiDataBaseImageGroupList ();
  };  /* UmiDataBaseImageGroupList */

};
