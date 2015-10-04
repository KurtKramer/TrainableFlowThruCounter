#include "StdAfx.h"
#include "FirstIncludes.h"

#include <stdio.h>
#include <vector>
#include "MemoryDebug.h"


#include "KKBaseTypes.h"
using namespace  KKB;

#include "ShrimpLengthComputer.h"
using namespace  LarcosCounterUnManaged;


#include "UmiShrimpLengthComputer.h"
using namespace  LarcosCounterManaged;


UmiShrimpLengthComputer::UmiShrimpLengthComputer (ShrimpLengthComputerPtr _lengthComputer):
  allocatedMemory (0),
  lengthComputer  (_lengthComputer)
{
  allocatedMemory = lengthComputer->MemoryConsumedEstimated ();
  GC::AddMemoryPressure (allocatedMemory);
}



UmiShrimpLengthComputer::UmiShrimpLengthComputer (kkint32 _pixelsPerScanLine,
                                                  float _imageChamberWidth,
                                                  kkint32 _erosionStructSize,
                                                  kkint32 _backGroundPixelTH
                                                 ):
    allocatedMemory (0),
    lengthComputer  (NULL)
{
  lengthComputer = new ShrimpLengthComputer (_pixelsPerScanLine, _imageChamberWidth, _erosionStructSize, (uchar)_backGroundPixelTH);
  allocatedMemory = lengthComputer->MemoryConsumedEstimated ();
  GC::AddMemoryPressure (allocatedMemory);
}


UmiShrimpLengthComputer::!UmiShrimpLengthComputer ()

{
  delete  lengthComputer;
  lengthComputer = NULL;
  GC::RemoveMemoryPressure (allocatedMemory);
}



UmiShrimpLengthComputer::~UmiShrimpLengthComputer ()
{
  this->!UmiShrimpLengthComputer ();
}




void  UmiShrimpLengthComputer::ErosionStructSize::set (kkint32 _erosionStructSize)
{
  lengthComputer->ErosionStructSize (_erosionStructSize);
}




void  UmiShrimpLengthComputer::PixelsPerScanLine::set (kkint32 _pixelsPerScanLine)
{
  lengthComputer->PixelsPerScanLine (_pixelsPerScanLine);
}



UmiRaster^  UmiShrimpLengthComputer::GetDemoImage ()
{
  if  ((lengthComputer == NULL)  ||  (lengthComputer->DemoImage () == NULL))
    return nullptr;
  return gcnew UmiRaster (new Raster (*(lengthComputer->DemoImage ())));
}




void  UmiShrimpLengthComputer::ProcessImage (UmiRaster^  origImage,
                                             bool        buildDemoImage
                                            )
{
  if  (origImage == nullptr)
    return;
  lengthComputer->ProcessImage (origImage->UnmanagedClass (), buildDemoImage);
}

