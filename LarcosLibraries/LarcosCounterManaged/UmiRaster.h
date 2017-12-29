#pragma once

using namespace System;
using namespace System::Drawing;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Diagnostics;
using namespace System::Windows::Forms;


#include  "Raster.h"

#include  "UmiKKStr.h"
#include  "UmiRunLog.h"


namespace LarcosCounterManaged 
{

  public  ref class UmiRaster
  {
  public:
    UmiRaster (int  height,
               int  width
              );

    UmiRaster (System::Array^   _raster);

    UmiRaster (array<uchar, 2>^ _raster);

    UmiRaster  (RasterPtr  _raster);   // Will take ownership of "_raster"  and delete it when done.

    UmiRaster  (String^   _fileName);

    !UmiRaster ();
    ~UmiRaster ();


    System::Array^  BuildRasterArray ();

    array<array<System::Byte>^ >^  BuildRasterArray2 ();

    static  RasterPtr  BuildRasterObj (System::Array^  raster);

    static  RasterPtr  BuildRasterObj (array<uchar, 2>^ raster);

    property uchar    BackgroundPixelTH
    {
      uchar  get ()                         {return  raster->BackgroundPixelTH ();}
      void   set (uchar _backgroundPixelTH) {raster->BackgroundPixelTH (_backgroundPixelTH);}
    }

    property String^  FileName   {String^  get () {return  UmiKKStr::KKStrToSystenStr (raster->FileName ());}     void  set (String^  _fileName)  {raster->FileName (UmiKKStr::SystemStringToKKStr (_fileName));}}
    property int      Height     {int      get () {return  raster->Height ();}}
    property bool     IsColor    {bool     get () {return  raster->Color  ();}}
    property int      Width      {int      get () {return  raster->Width  ();}}


    enum  class  OperationType  
     {
       Null,
       BandPass,
       Binarize,
       BinarizeTH,
       Closing,
       ConnectedComponent,
       ConvexHull,
       Dilation, 
       Edge,
       Erosion,
       FillHoles,
       FourierTransform,
       MaskExclude,
       Opening, 
       SmoothAveraging,
       SmoothMedium,
       Stretching,
       Thinning
      };


     enum  class  MaskType: int  
     {
       CROSS3   = 0,
       CROSS5   = 1,
       SQUARE3  = 2,
       SQUARE5  = 3,
       SQUARE7  = 4,
       SQUARE9  = 5,
       SQUARE11 = 6
     };


     static  System::String^  OperationTypeToStr   (OperationType    ot);
     static  OperationType    OperationTypeFromStr (System::String^  otStr);

     static  System::String^      MaskTypeToStr   (MaskType         mt);
     static  UmiRaster::MaskType  MaskTypeFromStr (System::String^  mtStr);

     static  int              MaskSize (MaskType  mt);


     UmiRaster^   BandPass (float  lowerFreqBound,
                            float  upperFreqBound,
                            bool   retainBackground
                           );

     UmiRaster^   Binarize ();

     UmiRaster^   BinarizeByThreshold (kkuint32  min,
                                       kkuint32  max
                                      );

     Bitmap^      BuildBitmap ();

     float        CalcWeightedArea ();

     UmiRaster^   CloseImage (MaskType  mt);

     UmiRaster^   ConnectedComponent ();  // returns the largest connected component in the image.

     UmiRaster^   ConvexHull ();

     UmiRaster^   CreateColor ();

     UmiRaster^   DilateImage (MaskType  mt);

     void   DrawLine (int bpRow,    int bpCol,
                      int epRow,    int epCol,
                      Color^  pv
                     );


     void   DrawCircle (float   centerRow, 
                        float   centerCol, 
                        float   radius,
                        Color^  color
                       );


     void   DrawCircle (float   centerRow, 
                        float   centerCol, 
                        float   radius,
                        float   startAngle,   /*!< Start and End angles should be given in radians */
                        float   endAngle,     /*!< Where the angles are with respect to the compass */
                        Color^  color
                       );

     void  DrawGrid (float                   pixelsPerMinor,
                     int                     minorsPerMajor,
                     System::Drawing::Color  hashColor,
                     System::Drawing::Color  gridColor
                    );

     UmiRaster^   EdgeImage ();

     UmiRaster^   ErodeImage (MaskType  mt);

     UmiRaster^   FillHoles ();

     /**
      *@brief Returns an image that reflects the differences between this image and the image supplied in the parameter.
      *@details  Each pixel will represent the magnitude of the difference between the two raster instances for that 
      * pixel location.  If there are no differences than a raster of all 0's will be returned. If dimensions are different
      * then the largest dimensions will be sued.
      *@param[in]  r  Raster to compare with.
      *@returns A raster that will reflect the differences between the two instances where each pixel will represent 
      * the magnitude of the differences.
      */
     UmiRaster^   FindMagnitudeDifferences (UmiRaster^  r);

     UmiRaster^   FourierTransform ();

     Color  GetPixelValue (int r,  int c);

     UmiRaster^   HistogramEqualizedImage ();

     UmiRaster^   MaskExclude (MaskType    mt);


     void   SetPixelValue (int r, int c, 
                           uchar red,  uchar green,  uchar blue
                          );

     void   SetPixelValue (int r,  int c, 
                           Color^  pv
                          );

     UmiRaster^   OpenImage (MaskType  mt);

     UmiRaster^   ReduceByFactor (float factor);  //  0 < factor <= 1.0  ex: 0.5 = Make raster half size

     UmiRaster^   ReversedImage ();  /**<  Produces image with Foreground and Background reversed. */

     void         Save (String^  fileName);

     UmiRaster^   SmoothAveraging (int  maskSize);

     UmiRaster^   SmoothMedium (int  maskSize);

     UmiRaster^   StreatchImage (float  rowFactor,
                                 float  colFactor
                                );

     UmiRaster^   Thinning ();

     UmiRaster^   ToColor (); /**< Converts gray-scale image into equivalent color image; that is all
                               * three color channels will now exists but still appear as gray-scale.
                               */

     UmiRaster^   Transpose ();

     RasterPtr    UnmanagedClass ()   {return  raster;}


  private:
     RasterPtr  raster;
     kkMemSize  memoryPreasure;  /**< used by GC::RemoveMemoryPressure  to know how much memory pressure to remove. */
  };  /* UmiRaster */



  public  ref  class  UmiRasterList: public List<UmiRaster^>
  {
  public:
    UmiRasterList ();

    /**
     *@brief  'UmiRasterList' will be taking ownership of 'images' and its contents.
     *@details If images does not own its contents then we will create new instances that
     * this instance will then own.  The "Owner" flag on 'images' will be set to 'false' and
     * images will be deleted and set to NULL.
     */
    UmiRasterList (RasterListPtr&  images);


    /**
     *@brief Gives ownership of contents of 'images' to this instance 'UmiRasterList'.
     *@details If images does not own its contents then we will create new instances that
     * this instance will then own.  The "Owner" flag on 'images' will be set to 'false'
     * upon return because 'image' will no longer own its contents.  The caller will still
     * be responsible for 'images' itself.
     */
    void  GiveOwnership (RasterList&   images);


    static
    void  CopyOverIntermediateImages (RasterListPtr&  src,
                                      UmiRasterList^  dest
                                     );


  };  /* UmiStartStopPointList */



}
