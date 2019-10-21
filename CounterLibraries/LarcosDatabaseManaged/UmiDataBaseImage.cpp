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

#include  "..\KKBaseLibrery\GoalKeeper.h"
#include  "Raster.h"
#include  "OSservices.h"
using namespace KKB;


#include  "UmiKKStr.h"
#include  "UmiRaster.h"

#include  "UmiDataBaseImage.h"




using namespace  LarcosDatabaseManaged;

using namespace System;
using namespace System::Windows::Forms;




UmiDataBaseImage::UmiDataBaseImage ():
   image (NULL)
{
}




UmiDataBaseImage::UmiDataBaseImage (DataBaseImagePtr  _image):
   image (_image)
{
}




UmiDataBaseImage::~UmiDataBaseImage ()
{
  this->!UmiDataBaseImage ();
}



UmiDataBaseImage::!UmiDataBaseImage ()
{
  delete  image;
  image = NULL;
}






Image^  UmiDataBaseImage::Thumbnail::get ()
{
  if  (!image->EncodedThumbnail ())
     return  nullptr;

  // See "SimpleCompressor" 

  UmiRaster^  pr = nullptr;
  try
  {
    pr = gcnew UmiRaster (Raster::FromSimpleCompression (image->EncodedThumbnail (), image->EncodedThumbnailLen ()));
  }
  catch  (Exception^  e)
  {
    System::Windows::Forms::MessageBox::Show ("Exception creating Thumbnail Image.\n\n" +
                                              "ImageFileName[" + ImageFileName+ "]  Height[" + Height + "]  Width[" + Width + "]\n\n" +
                                              e->ToString (),
                                              "UmiDataBaseImage::Thumbnail"
                                             );
    pr = gcnew UmiRaster (50, 50);
  }

  Image^ thumbNail = pr->BuildBitmap ();
  delete  pr;
  pr = nullptr;

  return  thumbNail;
}





String^   UmiDataBaseImage::Class1Name::get ()
{
  return  UmiKKStr::KKStrToSystenStr (image->Class1Name ());
}




void  UmiDataBaseImage::ValidatedClass::set (UmiClass^ _validatedCass)
{
  if  (_validatedCass == nullptr)
    image->ValidatedClass (NULL);
  else
    image->ValidatedClass (_validatedCass->UnmanagedImageClass ());
}



String^  UmiDataBaseImage::ValidatedClassName::get ()
{
  return  UmiKKStr::KKStrToSystenStr (image->ValidatedClassName ());
}



String^   UmiDataBaseImage::ImageFileName::get ()
{
  return  UmiKKStr::KKStrToSystenStr (image->ImageFileName ());
}




UmiDataBaseImageList::UmiDataBaseImageList ()
{
}



UmiDataBaseImageList::UmiDataBaseImageList (DataBaseImageListPtr  unmanagedImages)
{
  if  (unmanagedImages == NULL)
    return;

  DataBaseImageList::iterator  idx;
  for  (idx = unmanagedImages->begin ();  idx != unmanagedImages->end ();  idx++)
  {
    DataBaseImagePtr  i = *idx;
    UmiDataBaseImage^ pi = gcnew UmiDataBaseImage (i);
    Add (pi);
  }
  unmanagedImages->Owner (false);
}



UmiDataBaseImageList::~UmiDataBaseImageList ()
{
  CleanUpMemory ();
}


UmiDataBaseImageList::!UmiDataBaseImageList ()
{
  CleanUpMemory ();
}



void  UmiDataBaseImageList::CleanUpMemory ()
{
  Clear ();
}



void  UmiDataBaseImageList::AddList (UmiDataBaseImageList^ list)
{
  for each  (UmiDataBaseImage^ i in list)
  {
    Add (i);
  }
}  /* AddList */




UmiDataBaseImageList::UmiDataBaseImageComparer::UmiDataBaseImageComparer 
           (SortOrderType  _sortOrder, 
            bool           _reverseSort
           ):
      sortOrder   (_sortOrder),
      reverseSort (_reverseSort)
  {}
                                   




int  UmiDataBaseImageList::UmiDataBaseImageComparer::Compare 
     (UmiDataBaseImage^ x, 
      UmiDataBaseImage^ y
     )
{
  int  result = 0;

  if  (x == nullptr)
    result = (y == nullptr) ? 0 : -1;

  else if  (y == nullptr)
    result = 1;

  else
  {
    switch  (sortOrder)
    {
    case  SortOrderType::soNULL:                result = 0;                                                         break;
    case  SortOrderType::soBreakTie:            result = x->BreakTie.CompareTo    (y->BreakTie);                    break;
    case  SortOrderType::soClassName:           result = x->Class1Name->CompareTo (y->Class1Name);                  break;
    case  SortOrderType::soDepth:               result = x->Depth.CompareTo       (y->Depth);                       break;
    case  SortOrderType::soProbability:         result = x->Class1Prob.CompareTo  (y->Class1Prob);                  break;
    case  SortOrderType::soSize:                result = x->PixelCount.CompareTo  (y->PixelCount);                  break;
    case  SortOrderType::soSpatial:             result = 0;                                                         break;
    case  SortOrderType::soValidatedClassName:  result = x->ValidatedClassName->CompareTo (y->ValidatedClassName);  break;
    }
  }

  if  (result == 0)
  {
    // Since we are still tied;  we will compre in temporal order.
    result = x->ScannerFileName->CompareTo (y->ScannerFileName);
    if  (result == 0)
      result = x->TopLeftRow.CompareTo (y->TopLeftRow);
  }

  if  (reverseSort)
    result = 0 - result;  // Revese the result of th ecomparision.

  return  result;
}  /* Compare */




void  UmiDataBaseImageList::Sort (SortOrderType  _sortOrder, 
                                    bool           _reverseSort
                                   )
{
  UmiDataBaseImageComparer^  comparer = gcnew UmiDataBaseImageComparer (_sortOrder, _reverseSort);

  List<UmiDataBaseImage^>::Sort (comparer);
}  /* Sort */




String^  UmiDataBaseImageList::SortOrderTypeToStr (SortOrderType  so)
{
  String^  result = nullptr;

  switch (so)
  {
  case  SortOrderType::soNULL:                result = "Null";         break;
  case  SortOrderType::soBreakTie:            result = "BreakTie";     break;
  case  SortOrderType::soClassName:           result = "ClassName";    break;
  case  SortOrderType::soDepth:               result = "Depth";        break;
  case  SortOrderType::soProbability:         result = "Probability";  break;
  case  SortOrderType::soSize:                result = "Size";         break;
  case  SortOrderType::soSpatial:             result = "Spatial";      break;
  case  SortOrderType::soValidatedClassName:  result = "ValidatedName";    break;
   
  }
  return  result;
}  /* SortOrderTypeToStr */



UmiDataBaseImageList::SortOrderType  UmiDataBaseImageList::SortOrderTypeFromStr (String^  s)
{
  s = s->ToUpper ();

  if  (s == "NULL")           return  SortOrderType::soNULL;
  if  (s == "BREAKTIE")       return  SortOrderType::soBreakTie;
  if  (s == "CLASSNAME")      return  SortOrderType::soClassName;
  if  (s == "DEPTH")          return  SortOrderType::soDepth;
  if  (s == "PROBABILITY")    return  SortOrderType::soProbability;
  if  (s == "SIZE")           return  SortOrderType::soSize;
  if  (s == "SPATIAL")        return  SortOrderType::soSpatial;
  if  (s == "VALIDATEDNAME")  return  SortOrderType::soValidatedClassName;

  return  SortOrderType::soNULL;
}  /* SortOrderTypeFromStr */



array<UmiDataBaseImageList::SortOrderType>^    UmiDataBaseImageList::SortOrderTypeList ()
{
  array<SortOrderType>^  sortOrderTypeList = gcnew array<SortOrderType> (7);
  sortOrderTypeList[0] = SortOrderType::soNULL;
  sortOrderTypeList[1] = SortOrderType::soBreakTie;
  sortOrderTypeList[2] = SortOrderType::soClassName;
  sortOrderTypeList[3] = SortOrderType::soDepth;
  sortOrderTypeList[4] = SortOrderType::soProbability;
  sortOrderTypeList[5] = SortOrderType::soSize;
  sortOrderTypeList[6] = SortOrderType::soSpatial;
  sortOrderTypeList[7] = SortOrderType::soValidatedClassName;

  return  sortOrderTypeList;
}  /* SortOrderTypeList */



void  UmiDataBaseImageList::PopulateComboBoxWithSortOrderOptions (ComboBox^  cb)
{
  cb->Items->Clear ();
  cb->Items->Add (SortOrderTypeToStr (SortOrderType::soSpatial));
  cb->Items->Add (SortOrderTypeToStr (SortOrderType::soBreakTie));
  cb->Items->Add (SortOrderTypeToStr (SortOrderType::soClassName));
  cb->Items->Add (SortOrderTypeToStr (SortOrderType::soValidatedClassName));
  cb->Items->Add (SortOrderTypeToStr (SortOrderType::soDepth));
  cb->Items->Add (SortOrderTypeToStr (SortOrderType::soSize));
  cb->Items->Add (SortOrderTypeToStr (SortOrderType::soProbability));
}
