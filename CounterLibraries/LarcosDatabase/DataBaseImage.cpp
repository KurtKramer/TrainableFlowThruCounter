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

#include  "ImageClass.h"
using namespace KKMachineLearning;

#include  "DataBaseImage.h"
using namespace LarcosDatabase;




DataBaseImage::DataBaseImage ():
    byteOffset       (0),
    centroidCol      (0),
    centroidRow      (0),
    class1           (NULL),
    class1Prob       (0.0f),
    class2           (NULL),
    class2Prob       (0.0f),
    depth            (0.0f),
    encodedThumbnail (NULL),
    height           (0),
    imageId          (0),
    pixelCount       (0),
    scannerFileId     (-1),
    scannerFileName   (),
    topLeftRow       (0),
    topLeftCol       (0),
    validatedClass   (NULL),
    width            (0)
{
}



DataBaseImage::~DataBaseImage ()
{
  delete  encodedThumbnail;
  encodedThumbnail = NULL;
}


float  DataBaseImage::BreakTie () const  
{
  if  (class1Prob > class2Prob)
    return  (class1Prob - class2Prob);
  else
    return  (class2Prob - class1Prob);
}


KKStr  DataBaseImage::ImageFileName () const
{
  return  osGetRootName (scannerFileName) + "_" + StrFormatInt (topLeftRow, "00000000") + "_" + StrFormatInt (topLeftCol, "0000");
}



const KKStr&  DataBaseImage::Class1Name ()  const
{
  if  (class1)
    return class1->Name ();
  else
    return KKStr::EmptyStr ();
}



const KKStr&  DataBaseImage::Class2Name ()  const
{
  if  (class2)
    return class2->Name ();
  else
    return KKStr::EmptyStr ();
}



void  DataBaseImage::Class1Name (const char* _class1Name)
{
  if  (!_class1Name)
    class1 = NULL;
  
  else if  (strlen (_class1Name) == 0)
    class1 = NULL;

  else
    class1 = ImageClass::CreateNewImageClass (_class1Name);
}  /* Class1Name */




void  DataBaseImage::Class2Name (const char*  _class2Name)
{
  if  (!_class2Name)
     class2 = NULL;

  else if  (strlen (_class2Name) == 0)
     class2 = NULL;

  else
    class2 = ImageClass::CreateNewImageClass (_class2Name);
}  /* Class2Name */



const  KKStr&   DataBaseImage::ValidatedClassName () const
{
  if  (validatedClass)
    return  validatedClass->Name ();
  else
    return  KKStr::EmptyStr ();
}



void  DataBaseImage::ValidatedClassName (const char*  _validatedClassName)
{
  if  (!_validatedClassName)
    validatedClass = NULL;

  else if  (strlen (_validatedClassName) == 0)
    validatedClass = NULL;

  else
    validatedClass = ImageClass::CreateNewImageClass (_validatedClassName);
}  /* ValidatedClassName */




RasterPtr  DataBaseImage::ThumbNail () const
{
  if  (!encodedThumbnail)
    return  NULL;
  return  Raster::FromSimpleCompression (this->encodedThumbnail, encodedThumbnailLen);
}  /* ThumbNail */




DataBaseImageList::DataBaseImageList (bool _owner):
      KKQueue<DataBaseImage> (_owner)
{
}


DataBaseImageList::~DataBaseImageList ()
{
}




ImageClassListPtr  DataBaseImageList::ExtractListOfClasses ()
{
  ImageClassListPtr  classes = new ImageClassList ();
  DataBaseImageList::iterator  idx;

  for  (idx = begin ();  idx != end ();  idx++)
  {
    DataBaseImagePtr  i = *idx;

    ImageClassPtr  ic = i->ValidatedClass ();
    if  (ic == NULL)
      ic = i->Class1 ();

    if  (ic)
    {
      if  (classes->PtrToIdx (ic) < 0)
      {
        classes->PushOnBack (ic);
      }
    }
  }

  return  classes;
}  /* ExtractListOfClasses */




DataBaseImageListPtr  DataBaseImageList::ExtractImagesForAGivenClass (ImageClassPtr  _imageClass)
{ 
  DataBaseImageListPtr  images = new DataBaseImageList (false);
  DataBaseImageList::iterator  idx;

  for  (idx = begin ();  idx != end ();  idx++)
  {
    DataBaseImagePtr  i = *idx;
    ImageClassPtr  ic = i->ValidatedClass ();
    if  (ic == NULL)
      ic = i->Class1 ();

    if  (ic == _imageClass)
      images->PushOnBack (i);
  }

  return   images;
}  /* ExtractImagesForAGivenClass */




