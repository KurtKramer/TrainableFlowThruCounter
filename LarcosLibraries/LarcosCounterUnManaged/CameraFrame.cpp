/* CameraFrame.cpp -- Represents a single camera frame.
 * Copyright (C) 2011-2013  Kurt Kramer
 */
#include "FirstIncludes.h"
#include <errno.h>
#include <istream>
#include <iostream>
#include <vector>
#include "MemoryDebug.h"
using namespace std;

#include "KKBaseTypes.h"
#include "KKException.h"
#include "OSservices.h"
#include "Raster.h"
using namespace KKB;

#include "ScannerFileEntry.h"
using namespace  KKLSC;

#include "CameraFrame.h"
#include "CameraFlatFieldCorrection.h"
using  namespace  LarcosCounterUnManaged;



CameraFrame::CameraFrame (kkint32 _frameHeight,
                          kkint32 _frameWidth
                         ):
    flowMeterCounter         (0),
    flowMeterCounterScanLine (0),
    flowRate                 (1.0f),
    flowRateRatio            (1.0f),
    frame                    (NULL),
    processed                (false),
    seqNum                   (0),
    scannerFileEntry         (NULL),
    scannerFileScanLine      (0),
    waitingToProcess         (false),
    waitingToWriteToDisk     (false),
    writtenToDisk            (false) 
{
  frame = new Raster (_frameHeight, _frameWidth, false);
}



CameraFrame::~CameraFrame ()
{
  delete  frame;
  frame = NULL;
}



uchar  CameraFrame::operator() (kkint32 row,  kkint32 col)  const
{
  if  (!frame)
  {
    KKStr  errMsg = "No frame is allocated.";
    cerr << endl << endl << errMsg << endl << endl;
    throw KKException (errMsg);
  }

  if  ((row < 0)  ||  (row >= frame->Height ())  ||  
       (col < 0)  ||  (col >= frame->Width  ())
      )
  {
    KKStr  errMsg;
    errMsg << "CameraFrame::operator()   row[" << row << "]  or  col[" << col << "] are out of bounds.";
    cerr << endl << endl << errMsg << endl << endl;
    throw KKException (errMsg);
  }

  return  frame->GetPixelValue (row, col);
}


kkint32 CameraFrame::FrameHeight ()  const
{
  if  (frame)
    return frame->Height ();
  else
    return 0;
}



kkint32 CameraFrame::FrameWidth ()  const
{
  if  (frame)
    return frame->Width ();
  else
    return 0;
}


const KKStr&  CameraFrame::ScannerFileRootName  ()  const
{
  if  (scannerFileEntry)
    return scannerFileEntry->RootName ();
  else
    return KKStr::EmptyStr ();

}


const KKStr&  CameraFrame::ScannerFileFullName  ()  const
{
  if  (scannerFileEntry)
    return scannerFileEntry->FullName ();
  else
    return KKStr::EmptyStr ();

}



void  CameraFrame::ReSizeFrame (kkint32 newHeight,
                                kkint32 newWidth
                               )
{
  delete  frame;
  frame = new Raster (newHeight, newWidth);
}  /* ReSizeFrame */



RasterPtr  CameraFrame::ToRaster ()  const
{
  if  (frame)
    return new Raster (*frame);
  else
    return NULL;
}



void  CameraFrame::PopualteFrameFromCameraData (uchar*               _cameraData,
                                                kkint32              _cameraDataSize,
                                                bool                 _complementData,
                                                ScannerFileEntryPtr  _scannerFileEntry,
                                                kkint32              _scannerFileScanLine,
                                                kkuint32             _flowMeterCounter,
                                                kkuint32             _flowMeterCounterScanLine,
                                                float                _flowRateRatio,
                                                float                _flowRate
                                               )
{
  if  (_cameraDataSize > frame->TotPixels ())
  {
    KKStr  errMsg;
    errMsg << "CameraFrame::PopualteFrameFromCameraData    cameraDataSize[" << _cameraDataSize << "]"
           << "  and   frame->TotPixels[" << frame->TotPixels () << "]   are not in agreement.";
    cerr << endl << endl << errMsg << endl << endl;
    throw  KKException (errMsg);
  }

  flowMeterCounter          = _flowMeterCounter;
  flowMeterCounterScanLine  = _flowMeterCounterScanLine;
  flowRateRatio             = _flowRateRatio;
  flowRate                  = _flowRate;
  scannerFileEntry          = _scannerFileEntry;
  scannerFileScanLine       = _scannerFileScanLine;

  if  (_complementData)
  {
    kkuint64*  srcPtr  = (kkuint64*)_cameraData;
    kkuint64*  destPtr = (kkuint64*)frame->GreenArea ();

    kkuint32 num8ByteBlocksLeft = _cameraDataSize / 8;
    kkuint32 numLeftOverBytes   = _cameraDataSize % 8;

    while  (num8ByteBlocksLeft)
    {
      *destPtr = ~(*srcPtr);
      ++destPtr;
      ++srcPtr;
      --num8ByteBlocksLeft;
    }

    if  (numLeftOverBytes)
    {
      uchar* srcUcharPtr  = (uchar*)srcPtr;
      uchar* destUcharPtr = (uchar*)destPtr;
      while  (numLeftOverBytes)
      {
        *destUcharPtr = ~(*srcUcharPtr);
        ++destUcharPtr;
        ++srcUcharPtr;
        --numLeftOverBytes;
      }
    }
  }
  else
  {
    uchar*  dest = frame->GreenArea ();
    KKStr::MemCpy (dest, _cameraData, _cameraDataSize);
  }
}  /* PopualteFrameFromCameraData */



CameraFrameList::CameraFrameList (bool _owner):
  KKQueue<CameraFrame> (_owner)
{
}


CameraFrameList::~CameraFrameList ()
{
}

