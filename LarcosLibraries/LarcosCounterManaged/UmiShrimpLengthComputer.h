#pragma once

#include  "ShrimpLengthComputer.h"

using namespace System;

#include  "UmiRaster.h"

namespace LarcosCounterManaged 
{
  using namespace System;

  public  ref class  UmiShrimpLengthComputer
  {
  public:
    UmiShrimpLengthComputer (ShrimpLengthComputerPtr _lengthComputer);

    UmiShrimpLengthComputer (kkint32 _pixelsPerScanLine,
                             float _imageChamberWidth,
                             kkint32 _erosionStructSize,
                             kkint32 _backGroundPixelTH
                            );

    !UmiShrimpLengthComputer ();

    ~UmiShrimpLengthComputer ();



    property  kkint32 ErosionStructSize         {kkint32 get ()  {return  lengthComputer->ErosionStructSize         ();}   void  set (kkint32 _erosionStructSize);}
    property  bool    ImageIsOccluded           {bool  get ()  {return  lengthComputer->ImageIsOccluded           ();}}
    property  bool    ImageIsDistorted          {bool  get ()  {return  lengthComputer->ImageIsDistorted          ();}}
    property  float   LenInMiliMeters           {float get ()  {return  lengthComputer->LenInMiliMeters           ();}}
    property  float   LenInPixels               {float get ()  {return  lengthComputer->LenInPixels               ();}}
    property  float   MmPerScanLine             {float get ()  {return  lengthComputer->MmPerScanLine             ();}}
    property  kkint32 PixelsPerScanLine         {kkint32 get ()  {return  lengthComputer->PixelsPerScanLine         ();}   void  set (kkint32 _pixelsPerScanLine);}
    property  float   OrientationAngle          {float get ()  {return  lengthComputer->OrientationAngle          ();}}
    property  float   OrientationAngleInDegrees {float get ()  {return  lengthComputer->OrientationAngleInDegrees ();}}

    UmiRaster^  GetDemoImage ();

    void  ProcessImage (UmiRaster^  origImage,
                        bool        buildDemoImage
                       );

  private:
    ShrimpLengthComputerPtr   lengthComputer;
    int                       allocatedMemory;
  };  /* UmiShrimpLengthComputer */
}
