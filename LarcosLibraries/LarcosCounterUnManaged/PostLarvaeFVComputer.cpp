/* CameraAcquisition.cpp -- Base class for the Acquisition threads.
 * Copyright (C) 2011-2013  Kurt Kramer
 * For conditions of distribution and use, see copyright notice in LarcosCounterUnManaged.txt
 */
#include "FirstIncludes.h"

#include <errno.h>
#include <istream>
#include <iostream>
#include <queue>
#include <vector>
#if  defined(WIN32)
#include <windows.h>
#endif
#include "MemoryDebug.h"
using namespace std;


#include "KKBaseTypes.h"
#include "GoalKeeper.h"
#include "OSservices.h"
#include "Raster.h"
#include "RunLog.h"
using namespace KKB;


#include "PostLarvaeFVComputer.h"
#include "PostLarvaeFV.h"
using  namespace  LarcosCounterUnManaged;



PostLarvaeFVComputer::PostLarvaeFVComputer ():
    FeatureVectorComputer ("PostLarvaeFV",
                           PostLarvaeFV::PostLarvaeFeaturesFileDesc ()
                          ),
    totPixsForMorphOps (100000)
{
}




PostLarvaeFVComputer::~PostLarvaeFVComputer ()
{
}





const type_info*   PostLarvaeFVComputer::FeatureVectorTypeId () const
{
  return  &(typeid (PostLarvaeFV));
}





FeatureVectorPtr  PostLarvaeFVComputer::ComputefeatureVector (RasterPtr  image,
                                                              RunLog&    runLog
                                                             )
{
  return NULL;
}

