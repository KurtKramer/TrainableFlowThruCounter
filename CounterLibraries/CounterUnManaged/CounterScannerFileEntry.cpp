/* OperatingParameters.cpp -- Represents the operating parameters used by Larcos; ex's:  BackGroundThreshold, FlowRateFactor.
 * Copyright (C) 2011-2013  Kurt Kramer
 * For conditions of distribution and use, see copyright notice in CounterUnManaged.txt
 */
#include "FirstIncludes.h"
#include <errno.h>
#include <istream>
#include <iostream>
#include <vector>
#include "MemoryDebug.h"
using namespace std;


#include "KKBaseTypes.h"
#include "OSservices.h"
using namespace KKB;


#include "ScannerFile.h"
using namespace  KKLSC;

#include "OperatingParameters.h"
#include "SessionParameters.h"
#include "CounterScannerFileEntry.h"


using  namespace  CounterUnManaged;

