#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Collections::Generic;
using namespace System::Diagnostics;
using namespace System::Drawing;
using namespace System::Windows::Forms;

#include  "OSservices.h"
#include  "DataBaseImage.h"

#include  "UmiRunLog.h"
#include  "UmiKKStr.h"
#include  "UmiClass.h"
#include  "UmiClassList.h"
#include  "UmiRaster.h"


namespace LarcosDatabaseManaged 
{
  public ref class UmiDataBaseImage
  {
  
  public:
    UmiDataBaseImage ();
    UmiDataBaseImage (DataBaseImagePtr  _image);

  protected:
    !UmiDataBaseImage ();

  private:
    ~UmiDataBaseImage ();

  public:
    property  float        BreakTie           {float        get () {return  image->BreakTie ();}}
    property  osFilePos    ByteOffset         {osFilePos    get () {return  image->ByteOffset ();}}
    property  UmiClass^    Class1             {UmiClass^    get () {return  UmiClassList::GetUniqueClass (image->Class1 ());}}
    property  String^      Class1Name         {String^      get ();}
    property  float        Class1Prob         {float        get () {return  image->Class1Prob ();}}
    property  UmiClass^    Class2             {UmiClass^    get () {return  UmiClassList::GetUniqueClass (image->Class2 ());}}
    property  float        Class2Prob         {float        get () {return  image->Class2Prob ();}}
    property  float        Depth              {float        get () {return  image->Depth();}}
    property  uint         Height             {uint         get () {return  image->Height ();}}
    property  String^      ImageFileName      {String^      get ();}
    property  uint         ImageId            {uint         get () {return  image->ImageId    ();}}
    property  uint         TopLeftRow         {uint         get () {return  image->TopLeftRow ();}}
    property  uint         TopLeftCol         {uint         get () {return  image->TopLeftCol ();}}
    property  uint         PixelCount         {uint         get () {return  image->PixelCount ();}}
    property  String^      ScannerFileName    {String^      get () {return  UmiKKStr::KKStrToSystenStr (image->ScannerFileName ());}}
    property  Image^       Thumbnail          {Image^       get ();}
    property  UmiClass^    ValidatedClass     {UmiClass^    get () {return  UmiClassList::GetUniqueClass (image->ValidatedClass ());}  void set (UmiClass^ _validatedCass);}
    property  String^      ValidatedClassName {String^      get ();}
    property  uint         Width              {uint         get () {return  image->Width ();}}

    DataBaseImagePtr  UnManagedDataBaseImage ()   {return  image;}


  private:
    DataBaseImagePtr   image;
  };  /* UmiDataBaseImage */



  public  ref  class  UmiDataBaseImageList:  List<UmiDataBaseImage^>
  {
  public:
    UmiDataBaseImageList ();

    UmiDataBaseImageList (DataBaseImageListPtr  unmanagedImages);

    ~UmiDataBaseImageList ();
    !UmiDataBaseImageList ();

    enum  class  SortOrderType {soNULL, soBreakTie, soClassName, soDepth, soProbability, soSize, soSpatial, soValidatedClassName};

    static  String^                SortOrderTypeToStr   (SortOrderType  so);
    static  SortOrderType          SortOrderTypeFromStr (String^  s);
    static  array<SortOrderType>^  SortOrderTypeList    ();
    static  void                   PopulateComboBoxWithSortOrderOptions (ComboBox^  cb);

    void  AddList (UmiDataBaseImageList^ list);

    void  CleanUpMemory ();

    void  Sort (SortOrderType  _sortOrder, 
                bool           _reverseSort
               );

  private:
    ref class UmiDataBaseImageComparer: System::Collections::Generic::IComparer<UmiDataBaseImage^>
    {
    public:
      UmiDataBaseImageComparer (SortOrderType  _sortOrder, 
                                bool           _reverseSort
                               );

      virtual int Compare (UmiDataBaseImage^ x, UmiDataBaseImage^ y);

    private:
      SortOrderType  sortOrder;
      bool           reverseSort;
    };  /* UmiDataBaseImageComparer */

  };  /* UmiDataBaseImageList */
};

