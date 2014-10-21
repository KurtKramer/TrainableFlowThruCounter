/* ShrimpLengthComputer.h -- Routines to compute the length of a shrimp.
 * Copyright (C) 2011-2013  Kurt Kramer
 * For conditions of distribution and use, see copyright notice in LarcosCounterUnManaged.txt
 */
#if  !defined(_SHRIMPLENGTHCOMPUTER_)
#define  _SHRIMPLENGTHCOMPUTER_

#define  _ShrimpLengthComputerDebug_


#include <queue>

#include "GoalKeeper.h"
#include "KKQueue.h"
#include "MorphOpStretcher.h"
#include "Raster.h"
#include "RunLog.h"

#include "MLClass.h"

using namespace KKMachineLearning;

namespace LarcosCounterUnManaged
{
  class  ShrimpLengthComputer
  {
  public:
    typedef  ShrimpLengthComputer*  ShrimpLengthComputerPtr;

    ShrimpLengthComputer (kkint32  _pixelsPerScanLine,
                          float  _imageChamberWidth,
                          kkint32  _erosionStructSize,
                          uchar  _backgroundPixelTH
                         );

    virtual
    ~ShrimpLengthComputer ();

    virtual  kkint32  MemoryConsumedEstimated ()  const;

    RasterPtr  DemoImage                 () const  {return demoImage;}
    kkint32    ErosionStructSize         () const  {return erosionStructSize;}
    bool       ImageIsOccluded           () const  {return imageIsOccluded;}
    bool       ImageIsDistorted          () const  {return imageIsDistorted;}
    float      LenInMiliMeters           () const  {return lenInMiliMeters;}
    float      LenInPixels               () const  {return lenInPixels;}
    float      MmPerScanLine             () const  {return mmPerScanLine;}
    kkint32    PixelsPerScanLine         () const  {return pixelsPerScanLine;}
    float      OrientationAngle          () const  {return orientationAngle;}
    float      OrientationAngleInDegrees () const  {return orientationAngleInDegrees;}


    void  ErosionStructSize (kkint32 _erosionStructSize)  {erosionStructSize = _erosionStructSize;}

    void  PixelsPerScanLine (kkint32 _pixelsPerScanLine);


    Point  DerivePreRotatedPoint (kkint32 height,
                                  kkint32 width,
                                  Point&  rotatedPoint, 
                                  float   turnAngle
                                 );

    void  ProcessImage (RasterPtr  origImage,
                        bool       buildDemoImage
                       );

  private:
    float  CalcHeightWidthRatio (RasterPtr  i);

    /**
     *@brief  Number of erosions will be function of size of image.
     *@returns Number of erosions performed.
     */
    int   ErodeBySize (Raster& i);
    
    void   ErodeImageByStructSize (Raster& i);

    /** returns 'true' if image is to be considered occluded. */
    bool  OccludedImage (RasterPtr  i);

    uchar          backgroundPixelTH;
    PointListPtr   centerPoints;
    float          eigenRatio;
    kkint32        erosionStructSize;
    bool           imageIsOccluded;
    bool           imageIsDistorted;
    RasterPtr      demoImage;
    float          imageChamberWidth;
    float          lenInPixels;
    float          lenInMiliMeters;
    float          mmPerScanLine;
    float          mmPerPixel;
    kkint32        occlusiionLimit;
    uchar          occlusionPixelValueTH;
    float          orientationAngle;
    float          orientationAngleInDegrees;
    kkint32        pixelsPerScanLine;
    RasterPtr      rotatedImage;
    bool           saveDebugImages;
    float          scanRate;
    RasterPtr      workRaster;
  };  /* ShrimpLengthComputer */

  typedef  ShrimpLengthComputer*  ShrimpLengthComputerPtr;



}  /* LarcosCounterUnManaged */

#endif
