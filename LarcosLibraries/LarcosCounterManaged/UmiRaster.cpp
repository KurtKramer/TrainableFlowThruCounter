#include "StdAfx.h"
#include "FirstIncludes.h"
#include <stdio.h>
#include <math.h>
#include <ctype.h>
#include <time.h>
#include <fstream>
#include <iostream>
#include <istream>
#include <map>
#include <ostream>
#include <string>
#include <vector>
#include "MemoryDebug.h"
using namespace std;


#include "KKBaseTypes.h"
#include "..\\KKBase\\GoalKeeper.h"
#include "UmiOSservices.h"

#include "Blob.h"
#include "ContourFollower.h"
#include "ConvexHull.h"
#include "ImageIO.h"
#include "MorphOpMaskExclude.h"
#include "PixelValue.h"
#include "Raster.h"
using namespace KKB;


#include "UmiOSservices.h"

#include "UmiKKStr.h"
#include "UmiRunLog.h"
#include "UmiRaster.h"


using namespace  LarcosCounterManaged;


using namespace System;
using namespace System::Threading;
using namespace System::Windows::Forms;


UmiRaster::UmiRaster  (String^  _fileName):
    memoryPreasure (0),
    raster (NULL)
{
  if  (UmiOSservices::GetFileExtension (_fileName)->ToLower () == "bmp")
  {
    bool valid = false;
    raster = new Raster (UmiKKStr::SystemStringToKKStr (_fileName), valid);
    if  (!valid)
    {
      delete  raster;
      raster = NULL;
    }
  }

  if  (!raster)
  {
    System::Drawing::Bitmap^ i = nullptr;
    raster = NULL;
    try
    {
      i = gcnew System::Drawing::Bitmap (_fileName);
    }
    catch (Exception^ e)
    { 
      throw gcnew Exception ("UmiRaster::UmiRaster", e);
    }

    System::Drawing::Color   color;

    int  r, c;

    int h = i->Height;
    int w = i->Width;

    raster = new Raster (h, w);

    for  (r = 0;  r < h;  r++)
    {
      for  (c = 0;  c < w;  c++)
      {
        color = i->GetPixel (c, r);
        raster->SetPixelValue (r, c, 255 - color.G);
      }
    }
  }
  raster->FileName (UmiKKStr::SystemStringToKKStr (_fileName));

  memoryPreasure = raster->MemoryConsumedEstimated ();
  GC::AddMemoryPressure (memoryPreasure);
}



UmiRaster::UmiRaster (int  height,
                      int  width
                     ):
    memoryPreasure (0)
{
  raster = new Raster (height, width);
  memoryPreasure = raster->Width () * raster->Height () * (raster->Color () ? 3 : 1);
  GC::AddMemoryPressure (memoryPreasure);
}




UmiRaster::UmiRaster (System::Array^ _raster):
    memoryPreasure (0)
{
  raster = BuildRasterObj (_raster);
  memoryPreasure = raster->Width () * raster->Height () * (raster->Color () ? 3 : 1);
  GC::AddMemoryPressure (memoryPreasure);
}



UmiRaster::UmiRaster (array<uchar, 2>^  _raster):
    memoryPreasure (0),
    raster (NULL)
{
  int  height = _raster->GetLength (0);
  int  width  = _raster->GetLength (1);

  raster = new Raster (height, width);
  uchar*  rasterArea = raster->GreenArea ();
  for  (int row = 0;  row < height;  ++row)
  {
    for  (int col = 0;  col < width;  ++col)
    {
      *rasterArea = _raster[row, col];
      ++rasterArea;
    }
  }
}



UmiRaster::UmiRaster (RasterPtr  _raster):
   memoryPreasure (0),
   raster         (_raster)
{
  memoryPreasure =  raster->MemoryConsumedEstimated ();
  GC::AddMemoryPressure (memoryPreasure);
}



UmiRaster::!UmiRaster ()
{
  delete  raster;
  raster = NULL;

  if  (memoryPreasure > 0)
    GC::RemoveMemoryPressure (memoryPreasure);
}



UmiRaster::~UmiRaster ()
{
  this->!UmiRaster ();
}



System::String^  UmiRaster::MaskTypeToStr (MaskType  mt)
{
  System::String^  result = nullptr;
  switch  (mt)
  {
  case  MaskType::CROSS3:   result = "Cross-3";     break;
  case  MaskType::CROSS5:   result = "Cross-5";     break;
  case  MaskType::SQUARE3:  result = "Square-3";    break;
  case  MaskType::SQUARE5:  result = "Square-5";    break;
  case  MaskType::SQUARE7:  result = "Square-7";    break;
  case  MaskType::SQUARE9:  result = "Square-9";    break;
  case  MaskType::SQUARE11: result = "Square-11";   break;
  default:                  result = "UnKnown";     break;
  }  /* end of switch (ot) */

  return  result;
}  /* MaskTypeToStr */



UmiRaster::MaskType  UmiRaster::MaskTypeFromStr (System::String^  mtStr)
{
  mtStr = mtStr->ToUpper ();

  if  (mtStr == "CROSS-3")
    return  MaskType::CROSS3;

  if  (mtStr == "CROSS-5")
    return  MaskType::CROSS5;

  if  (mtStr == "SQUARE-3")
    return  MaskType::SQUARE3;

  if  (mtStr == "SQUARE-5")
    return  MaskType::SQUARE5;

  if  (mtStr == "SQUARE-7")
    return  MaskType::SQUARE7;

  if  (mtStr == "SQUARE-9")
    return  MaskType::SQUARE9;

  if  (mtStr == "SQUARE-11")
    return  MaskType::SQUARE11;

  return  MaskType::SQUARE3;
}  /* MaskTypeFromStr */





System::String^  UmiRaster::OperationTypeToStr (OperationType  ot)
{
  System::String^  result = nullptr;
  switch  (ot)
  {
  case  OperationType::BandPass:            result = "BandPass";            break;
  case  OperationType::Binarize:            result = "Binarize";            break;
  case  OperationType::BinarizeTH:          result = "BinarizeTH";          break;
  case  OperationType::Closing:             result = "Closing";             break;
  case  OperationType::ConnectedComponent:  result = "ConnectedComponent";  break;
  case  OperationType::ConvexHull:          result = "ConvexHull";          break;
  case  OperationType::Dialation:           result = "Dialation";           break;
  case  OperationType::Edge:                result = "Edge";                break;
  case  OperationType::Erosion:             result = "Erosion";             break;
  case  OperationType::FillHoles:           result = "FillHoles";           break;
  case  OperationType::FourierTransform:    result = "FourierTransform";    break;
  case  OperationType::Opening:             result = "Opening";             break;
  case  OperationType::SmoothAveraging:     result = "SmoothAveraging";     break;
  case  OperationType::SmoothMedium:        result = "SmoothMedium";        break;
  case  OperationType::Streatching:         result = "Streatching";         break;
  case  OperationType::Thinning:            result = "Thinning";            break;
  default:                                  result = "UnKnown";             break;
  }  /* end of switch (ot) */

  return  result;
}  /* OperationTypeToStr */




UmiRaster::OperationType  UmiRaster::OperationTypeFromStr (System::String^  otStr)
{
  otStr = otStr->ToUpper ();

  if  (otStr == "BANDPASS")
    return  OperationType::BandPass;
  
  if  (otStr == "BINARIZE")
    return  OperationType::Binarize;
  
  if  (otStr == "BINARIZETH")
    return  OperationType::BinarizeTH;
  
  if  (otStr == "CLOSING")
    return  OperationType::Closing;

  if  (otStr == "CONNECTEDCOMPONENT")
    return  OperationType::ConnectedComponent;

  if  (otStr == "CONVEXHULL")
    return  OperationType::ConvexHull;

  if  (otStr == "DIALATION")
    return  OperationType::Dialation;

  if  (otStr == "EDGE")
    return  OperationType::Edge;

  if  (otStr == "EROSION")
    return  OperationType::Erosion;
  
  if  (otStr == "FILLHOLES")
    return  OperationType::FillHoles;

  if  (otStr == "FOURIERTRANSFORM")
    return  OperationType::FourierTransform;

  if  (otStr == "OPENING")
    return  OperationType::Opening;

  if  (otStr == "SMOOTHAVERAGING")
    return  OperationType::SmoothAveraging;

  if  (otStr == "SMOOTHMEDIUM")
    return  OperationType::SmoothMedium;

  if  (otStr == "STREATCHING")
    return  OperationType::Streatching;

  if  (otStr == "THINNING")
    return  OperationType::Thinning;

  return  OperationType::Null;
}  /* OperationTypeFromStr */




int   UmiRaster::MaskSize (MaskType  mt)
{
  int  size = 3;
  switch  (mt)
  {
  case  MaskType::CROSS3:   size = 3;   break;
  case  MaskType::CROSS5:   size = 5;   break;
  case  MaskType::SQUARE3:  size = 3;   break;
  case  MaskType::SQUARE5:  size = 5;   break;
  case  MaskType::SQUARE7:  size = 7;   break;
  case  MaskType::SQUARE9:  size = 9;   break;
  case  MaskType::SQUARE11: size = 11;  break;
  }
  return  size;
}



Color  UmiRaster::GetPixelValue  (int r,  int c)
{
  uchar  red, green, blue;
  raster->GetPixelValue (r, c, red, green, blue);
  return Color::FromArgb (255, red, green, blue);
}  /* GetPixelValue */




void  UmiRaster::DrawLine (int bpRow,    int bpCol,
                           int epRow,    int epCol,
                           Color^  pv
                          )
{
  raster->DrawLine (bpRow, bpCol, epRow, epCol, pv->R, pv->G, pv->B);
}




void   UmiRaster::DrawCircle (float   centerRow, 
                              float   centerCol, 
                              float   radius,
                              Color^  color
                             )
{
  PixelValue pv (color->R, color->G, color->B);
  raster->DrawCircle (centerRow, centerCol, radius, pv);
}  /* DrawCircle */




void   UmiRaster::DrawCircle (float   centerRow, 
                              float   centerCol, 
                              float   radius,
                              float   startAngle,   /*!< Start and End angles should be given in degrees  */
                              float   endAngle,     /*!< Where the angles are with respect to the compass */
                              Color^  color
                             )
{
  PixelValue pv (color->R, color->G, color->B);
  raster->DrawCircle (centerRow, centerCol, radius, startAngle, endAngle, pv);
}  /* DrawCircle */




void  UmiRaster::DrawGrid (float                   pixelsPerMinor,
                           int                     minorsPerMajor,
                           System::Drawing::Color  hashColor,
                           System::Drawing::Color  gridColor
                          )
{
  PixelValue  hashPv (hashColor.R, hashColor.G, hashColor.B);
  PixelValue  gridPv (gridColor.R, gridColor.G, gridColor.B);
  raster->DrawGrid (pixelsPerMinor, minorsPerMajor, hashPv, gridPv);
}



void  UmiRaster::SetPixelValue  (int r,  int c, 
                                 uchar red,  uchar green,  uchar blue
                                )
{
  raster->SetPixelValue (r, c, red, green, blue);
}  /* SetPixelValue */




void  UmiRaster::SetPixelValue  (int r,  int c, 
                                 Color^  pv
                                )
{
  raster->SetPixelValue (r, c, pv->R, pv->G, pv->B);
}  /* SetPixelValue */





RasterPtr  UmiRaster::BuildRasterObj (System::Array^  _raster)
{
  if  (_raster == nullptr)
    return  NULL;

  int  h = _raster->Length;
  if  (h < 1)
    return  NULL;

  int w = ((System::Array^)(_raster->GetValue (0)))->Length;

  RasterPtr  r = new Raster (h, w, false);

  uchar*  greenArea = r->GreenArea ();

  int  row, col;

  int  z = 0;

  for (row = 0;  row < h;  row++)
  {
    System::Array^  rowData = (System::Array^)_raster->GetValue (row);
    for  (col = 0;  col < w;  col++)
    {
      greenArea[z] = (uchar)rowData->GetValue (col);
      z++;
    }
  }
 
  return  r;
}  /* BuildRasterObj */



RasterPtr  UmiRaster::BuildRasterObj (array<uchar, 2>^ raster)
{
  if  (raster == nullptr)
    return  NULL;

  int h = raster->GetLength (0);
  int w = raster->GetLength (1);
  if  ((h < 1)  ||  (w < 1))
    return  NULL;

  RasterPtr  r = new Raster (h, w, false);

  uchar*  greenArea = r->GreenArea ();

  int  row, col;

  int  z = 0;

  for (row = 0;  row < h;  row++)
  {
    for  (col = 0;  col < w;  col++)
    {
      greenArea[z] = raster[row,col]; 
      z++;
    }
  }
 
  return  r;
}  /* BuildRasterObj */






System::Array^  UmiRaster::BuildRasterArray ()
{
  if  (!raster)
    return  nullptr;

  int  width  = raster->Width ();
  int  height = raster->Height ();

  uchar**  srcRows = raster->Green ();
  uchar*   srcRow  = NULL;

  typedef  cli::array<Byte > ^   RowType;

  cli::array<RowType> ^  destRows  = gcnew array<RowType> (height);
  RowType   destRow = nullptr;
  
  for  (int row = 0;  row < height;  row++)
  {
    destRow = gcnew cli::array<Byte > (width);
      
    destRows[row] = destRow;
    srcRow        = srcRows[row];
    for  (int  col = 0;  col < width;  col++)
      destRow[col] = srcRow[col];
  }

  return  destRows;
}  /* BuildRasterArray */



array<array<System::Byte>^ >^  UmiRaster::BuildRasterArray2 ()
{
  if  (!raster)
    return  nullptr;

  int  row, col;

  int  width  = raster->Width ();
  int  height = raster->Height ();

  uchar**  srcRows = raster->Green ();
  if  (srcRows == NULL)
    throw gcnew Exception ("UmiRaster::BuildRasterArray2     GreenArea == NULL");

  uchar*   srcRow  = NULL;

  array<array<System::Byte>^ >^  destRows = gcnew array<array<System::Byte>^ > (height);
  
  array<System::Byte>^   destRow = nullptr;

  for  (row = 0;  row < height;  row++)
  {
    destRow = gcnew array<System::Byte> (width);

    destRows->SetValue (destRow, row);
    srcRow        = srcRows[row];
    for  (col = 0;  col < width;  col++)
    { 
      try
      {
        destRow[col] = srcRow[col];
      }
      catch  (Exception^ e3)
      {
        throw gcnew Exception ("UmiRaster::BuildRasterArray2   Error Processing Row[" + row.ToString () + "]  Col[" + col.ToString () + "]", e3);
      }
    }
  }

  return  destRows;
}







UmiRaster^   UmiRaster::BandPass (float  lowerFreqBound,
                                  float  upperFreqBound,
                                  bool   retainBackground
                                 )
{
  return  gcnew UmiRaster (raster->BandPass (lowerFreqBound, upperFreqBound, retainBackground));
}  /* BandPass */



UmiRaster^   UmiRaster::Binarize ()
{
  RasterPtr  result = raster->BinarizeByThreshold (2, 255);
  return  gcnew UmiRaster (result);
}  /* Binarize */





UmiRaster^   UmiRaster::BinarizeByThreshold (kkuint32  min,
                                             kkuint32  max
                                            )
{
  RasterPtr  result = raster->BinarizeByThreshold (min, max);
  return  gcnew UmiRaster (result);
}  /* BinarizeByThreshold */




UmiRaster^   UmiRaster::ErodeImage (MaskType  mt)
{
  RasterPtr  erodedRaster = raster->CreateErodedImage ((KKB::MaskTypes)mt);
  return  gcnew UmiRaster (erodedRaster);
}  /* ErodeImage */




UmiRaster^   UmiRaster::DialateImage (MaskType  mt)
{
  RasterPtr  dialatedRaster = raster->CreateDialatedRaster ((KKB::MaskTypes)mt);

  return  gcnew UmiRaster (dialatedRaster);
}   /* DialateImage */



UmiRaster^   UmiRaster::MaskExclude (MaskType    mt)
{
  MorphOpMaskExclude  mo ((KKB::MorphOp::MaskTypes)mt);
  RasterPtr  result = mo.PerformOperation (raster);
  return  gcnew UmiRaster (result);
}  /* OpenImage */





UmiRaster^   UmiRaster::OpenImage (MaskType  mt)
{
  RasterPtr  erodedImage = raster->CreateErodedImage         ((KKB::MaskTypes)mt);
  RasterPtr  opendImage  = erodedImage->CreateDialatedRaster ((KKB::MaskTypes)mt);
  delete  erodedImage;  erodedImage = NULL;
  return  gcnew UmiRaster (opendImage);
}  /* OpenImage */




UmiRaster^  UmiRaster::ReduceByFactor (float factor) //  0 < factor <= 1.0  ex: 0.5 = Make raster half size
{
  if  (!raster)
    return nullptr;

  RasterPtr  reducedRaster = raster->ReduceByFactor (factor);
  return gcnew UmiRaster (reducedRaster);
}




void   UmiRaster::Save (String^  fileName)
{
  if  (!raster)
     throw gcnew Exception ("UmiRaster::Save  'raster' is not defined.");

  String^  extension = UmiOSservices::GetFileExtension (fileName)->ToLower ();

  if  ((extension == "bmp")  &&  (!raster->Color ()))
  {
    // We want to use the routine in ImageIO from "KKBaseLibrery" to save this image because it is much more efficient in space ussage.
    //SaveImage (*raster, UmiKKStr::SystemStringToKKStr (fileName));
    SaveImageGrayscaleInverted8Bit (*raster, UmiKKStr::SystemStringToKKStr (fileName));
  }
  else
  {
    Bitmap^  bm = BuildBitmap ();
    bm->Save (fileName);
  }
}  /* Save */



float  UmiRaster::CalcWeightedArea ()
{
  return raster->CalcWeightedArea ();
}



UmiRaster^   UmiRaster::CloseImage (MaskType  mt)
{
  RasterPtr  dialatedImage = raster->CreateDialatedRaster     ((KKB::MaskTypes)mt);
  RasterPtr  closedImage   = dialatedImage->CreateErodedImage ((KKB::MaskTypes)mt);
  delete  dialatedImage;  dialatedImage = NULL;
  return  gcnew UmiRaster (closedImage);
}  /* CloseImage */




UmiRaster^   UmiRaster::ConvexHull ()
{
  KKB::ConvexHullPtr  ch = new KKB::ConvexHull ();
  RasterPtr  convexImage = ch->PerformOperation (raster);
  delete  ch; ch = NULL;
  return  gcnew UmiRaster (convexImage);
}  /* ConvexHull */



UmiRaster^  UmiRaster::ConnectedComponent ()
{
  RasterPtr  connectedRaster = new Raster (*raster);
  connectedRaster->ConnectedComponent (1);
  return  gcnew UmiRaster (connectedRaster);
}  /* ConnectedComponent */



UmiRaster^  UmiRaster::CreateColor ()
{
  RasterPtr  colorImage = raster->CreateColor ();
  return gcnew UmiRaster (colorImage);
}



UmiRaster^  UmiRaster::FillHoles ()
{
  RasterPtr  filledImage = new Raster (*raster);
  filledImage->FillHole ();
  return  gcnew UmiRaster (filledImage);
}  /* FillHoles */



UmiRaster^  UmiRaster::EdgeImage ()
{
  RasterPtr  edgeImage = new Raster (*raster);
  edgeImage->Edge ();
  return  gcnew UmiRaster (edgeImage);
}  /* FillHoles */



UmiRaster^  UmiRaster::FindMagnitudeDifferences (UmiRaster^  r)
{
  RasterPtr deltaRaster = raster->FindMagnitudeDifferences (*(r->raster));
  if  (!deltaRaster)
    return nullptr;
  else
    return gcnew UmiRaster (deltaRaster);
}



UmiRaster^  UmiRaster::FourierTransform ()
{
  RasterPtr  result = raster->FastFourier ();
  return  gcnew UmiRaster (result);
}  /* FourierTransform */



UmiRaster^  UmiRaster::HistogramEqualizedImage ()
{
  return gcnew UmiRaster (raster->HistogramEqualizedImage ());
}


UmiRaster^   UmiRaster::ReversedImage ()
{
  return gcnew UmiRaster (raster->ReversedImage ());
}



UmiRaster^   UmiRaster::SmoothAveraging (int  maskSize)
{
  RasterPtr  smoothedImage = raster->CreateSmoothImage (maskSize);
  return  gcnew UmiRaster (smoothedImage);
}  /* SmoothAveraging  */
   


UmiRaster^   UmiRaster::SmoothMedium (int  maskSize)
{
  RasterPtr   smoothedImage = raster->CreateSmoothedMediumImage (maskSize);
  return  gcnew UmiRaster (smoothedImage);
}  /* SmoothMedium  */



 UmiRaster^   UmiRaster::StreatchImage (float  rowFactor,
                                        float  colFactor
                                       )
 {
   RasterPtr  i = raster->StreatchImage (rowFactor, colFactor);
   return gcnew UmiRaster (i);
 }



UmiRaster^   UmiRaster::Thinning ()
{
  RasterPtr  thinnedImage = NULL;
  try
  {
    thinnedImage = raster->ThinContour ();
  }
  catch  (Exception^)
  {
    thinnedImage = NULL;
  }

  if  (thinnedImage == NULL)
    return  nullptr;

  return  gcnew UmiRaster (thinnedImage);
}  /* Thinning */



UmiRaster^  UmiRaster::ToColor ()
{
  return gcnew UmiRaster (raster->ToColor ());
}




UmiRaster^   UmiRaster::Transpose ()
{
  RasterPtr  result = NULL;
  try
  {
    result = raster->Transpose ();
  }
  catch  (Exception^)
  {
    result = NULL;
  }

  if  (result == NULL)
    return  nullptr;

  return  gcnew UmiRaster (result);
}  /* Transpose */



RasterPtr  ApplyMask (RasterPtr  origImage,
                      RasterPtr  maskImage
                     )
{
  int  row, col;
  RasterPtr  result = new Raster (origImage->Height (), origImage->Width ());
  for  (row = 0;  row < origImage->Height ();  row++)
  {
    for  (col = 0;  col < origImage->Width ();  col++)
    {
      if  (maskImage->ForegroundPixel (row, col))
        result->SetPixelValue (row, col, origImage->GetPixelValue (row, col));
    }
  }

  return  result;
}  /* ApplyMask*/




void  SaveRaster (RasterPtr     r,
                  const KKStr&  fileName
                 )
{
  UmiRaster^  pr = gcnew UmiRaster (new Raster (*r));

  Bitmap  bm = pr->BuildBitmap ();
  bm.Save (UmiKKStr::KKStrToSystenStr (fileName));

  pr = nullptr;
}






Bitmap^  UmiRaster::BuildBitmap ()
{
  int  rowFirst = 99999;
  int  rowLast  = -1;
  int  colFirst = 99999;
  int  colLast  = -1;

  int  width = raster->Width ();
  int  height = raster->Height ();

  bool  color = raster->Color ();

  uchar**  red   = NULL;
  uchar**  green = raster->Green ();
  uchar**  blue  = NULL;

  if  (color)
  {
    red   = raster->Red   ();
    blue  = raster->Blue  ();
  }
  else
  {
    red  = green;
    blue = green;
  }

  Bitmap^  image = gcnew Bitmap (width, height, System::Drawing::Imaging::PixelFormat::Format24bppRgb);
    
  // GDI+ return format is BGR, NOT RGB. 
  System::Drawing::Imaging::BitmapData^ bmData 
    = image->LockBits (System::Drawing::Rectangle (0, 0, width, height),
                       System::Drawing::Imaging::ImageLockMode::ReadWrite,
                       image->PixelFormat
                      );
  int stride = bmData->Stride;
  System::IntPtr  Scan0 = bmData->Scan0;

  {
    int  nOffset = stride - width * 3;
    int  bytesPerRow = width * 3 + nOffset;
    int  col = 0;
    int  row = 0;
    byte greenValue = 0;

    byte* ptr = (byte*)(void*)Scan0;

    for  (row = 0;  row < height;  row++)
    {
      uchar*  redDataRow   = red  [row];
      uchar*  greenDataRow = green[row];
      uchar*  blueDataRow  = blue [row];
          
      for (col = 0;  col < width;  col++)
      {
        if  (color)
        {
          ptr[0] = blueDataRow  [col];   ptr++;    // kak
          ptr[0] = greenDataRow [col];   ptr++;
          ptr[0] = redDataRow   [col];   ptr++;
        }
        else
        {
          greenValue = 255 - greenDataRow[col];
          ptr[0] = greenValue;  ptr++;
          ptr[0] = greenValue;  ptr++;
          ptr[0] = greenValue;  ptr++;
        }
      }

      ptr += nOffset;
    }  /* Unsafe */

    image->UnlockBits (bmData);
    return  image;
  }   /* Unsafe */
}  /* BuildBitmap */




UmiRasterList::UmiRasterList ()
{
}



UmiRasterList::UmiRasterList (RasterListPtr&  images)
{
  if  (images == NULL)
    return;

  GiveOwnership (*images);
  delete  images;
  images = NULL;
}



void  UmiRasterList::GiveOwnership (RasterList&  images)
{
  RasterList::iterator  idx;
  for  (idx = images.begin ();  idx != images.end ();  ++idx)
  {
    RasterPtr i = *idx;
    if  (!images.Owner ())
      i = new Raster (*i);
    Add (gcnew UmiRaster (i));
  }

  images.Owner (false);
}



void  UmiRasterList::CopyOverIntermediateImages (RasterListPtr&  src,
                                                 UmiRasterList^  dest
                                                )
{
  if  ((src == NULL)  ||  (dest == nullptr))
    return;

  while  (src->QueueSize () > 0)
  {
    RasterPtr  nextRaster = src->PopFromFront ();
    dest->Add (gcnew UmiRaster (nextRaster));
  }

  delete  src;
  src = NULL;
}



