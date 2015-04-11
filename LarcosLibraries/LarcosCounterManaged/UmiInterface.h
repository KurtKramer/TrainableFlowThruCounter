#ifndef  _UMIINTERFACE_
#define  _UMIINTERFACE_
#pragma once

using namespace System;

/**
 * @namespace  LarcosCounterManaged
 * @brief Classes that encapsulate Unmanaged code for use in the managed world of c# are located in this namepace.
 * @details The c# Managed world can not make calls to unmanaged code.  So that unmanaged classes that are useful 
 *  in KKBase (KKB), KKLineScanner (KKLSC), and KKMLL (KKMLL) can not be directly used.  A 
 *  separate set of classes need to be defined that will encapsulate the unmanaged classes providing a managed 
 *  interface need to be provided.  Ex:  'UmiFeatureVector' encapsulates an instance of the unmanaged class 
 *  'FeatureVector.h'.
*/
namespace LarcosCounterManaged
{

	public ref class Class1
	{
		// TODO: Add your methods for this class here.
	};
}

#endif
