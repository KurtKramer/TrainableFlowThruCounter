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


#include "PostLarvaeFVProducer.h"
#include "PostLarvaeFV.h"
using  namespace  LarcosCounterUnManaged;



PostLarvaeFVProducer::PostLarvaeFVProducer ():
    FeatureVectorComputer ("PostLarvaeFV",
                           PostLarvaeFV::PostLarvaeFeaturesFileDesc ()
                          ),
    totPixsForMorphOps (100000)
{
}




PostLarvaeFVProducer::~PostLarvaeFVProducer ()
{
}





const type_info*   PostLarvaeFVProducer::FeatureVectorTypeId () const
{
  return  &(typeid (PostLarvaeFV));
}





FeatureVectorPtr  PostLarvaeFVProducer::ComputeFeatureVector (RasterPtr  image,
                                                              RunLog&    runLog
                                                             )
{
  return 
  

}

