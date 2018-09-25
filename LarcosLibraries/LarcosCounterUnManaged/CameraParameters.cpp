/*
 * Copyright (C) 2011-2013  Kurt Kramer
 * For conditions of distribution and use, see copyright notice in LarcosCounterUnManaged.txt
 */
#include "FirstIncludes.h"
#include <errno.h>
#include <istream>
#include <iostream>
#include <fstream>
#include <queue>
#include <vector>
#if  defined(KKOS_WINDOWS)
#include <windows.h>
#endif
#include "MemoryDebug.h"
using namespace std;


#include "KKBaseTypes.h"
using namespace KKB;


#include "CameraParameters.h"
using  namespace  LarcosCounterUnManaged;


float   analogGain;         /**< Will reflect the value reported by the camera, not what we would like it to be. */
KKStr   deviceVendorName;
KKStr   deviceModelName;
KKStr   deviceVersion;
kkint32 digitalGain;        /**< Will reflect the value reported by the camera, not what we would like it to be. */
kkint32 frameHeight;
kkint32 frameWidth;
kkint32 linePeriod;         /**< 'acquisitionLinePeriod' from e2v;  Number microseconds per scan line.  */
KKStr   model;
float   scanRate;
KKStr   serialNumber;
KKStr   sensitivityMode;

KKStr   macAddress;
KKStr   ipAddress;
KKStr   interfaceSubnetMask;
KKStr   interfaceDefaultGateway;



CameraParameters::CameraParameters ():
   analogGain              (0.0),
   deviceVendorName        (),
   deviceModelName         (),
   deviceVersion           (),
   digitalGain             (0),
   frameHeight             (0),
   frameWidth              (0),
   linePeriod              (0),
   model                   (),
   scanRate                (0.0f),
   serialNumber            (),
   sensitivityMode         (),
   macAddress              (),
   ipAddress               (),
   interfaceSubnetMask     (),
   interfaceDefaultGateway ()

{
}



CameraParameters::~CameraParameters ()
{
}



kkMemSize  CameraParameters::MemoryConsumedEstimated ()
{
  return  sizeof (CameraParameters) + 
    deviceVendorName.MemoryConsumedEstimated ()  +
    deviceModelName.MemoryConsumedEstimated  ()  +
    deviceVersion.MemoryConsumedEstimated    ()  +
    ipAddress.MemoryConsumedEstimated        ()  +
    macAddress.MemoryConsumedEstimated       ()  +
    model.MemoryConsumedEstimated            ()  +
    serialNumber.MemoryConsumedEstimated     ();
}
