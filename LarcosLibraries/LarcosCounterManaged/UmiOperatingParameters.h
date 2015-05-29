#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Collections::Generic;
using namespace System::Diagnostics;

#include "OperatingParameters.h"
using namespace  LarcosCounterUnManaged;

#include "UmiScannerFile.h"


namespace LarcosCounterManaged
{
  public  ref  class  UmiOperatingParameters
  {
  public:
    typedef  UmiScannerFile::Format  ScannerFileFormat;

    UmiOperatingParameters  ();

    /// <summary>
    /// Will duplicate contents of _parms.
    /// </summary>
    UmiOperatingParameters  (UmiOperatingParameters^  _parms);

    /// <summary>
    /// Will take ownership '_parns';  caller BETTER NOT DELETE this instance.
    /// </summary>
    /// <param name="_parms">Parameters being given to this managed instance.</param>
    UmiOperatingParameters  (OperatingParametersPtr  _parms);

    /// <summary>
    /// Will allocate/ create a duplicate of '_parms' and own it.
    /// </summary>
    /// <param name="_parms">Parameters that are to be duplicated.</param>
    UmiOperatingParameters  (const OperatingParameters&  _parms);

  private:
    ~UmiOperatingParameters ();

  public:
    !UmiOperatingParameters ();

    void  Assign (UmiOperatingParameters^  parms);

    OperatingParametersPtr  UnManagedClass ()  {return operatingParameters;}

    property  int                BackGroundPixelTH        {int                get () {return operatingParameters->BackGroundPixelTH      ();}  void  set  (int                _backGroundPixelTH)        {operatingParameters->BackGroundPixelTH      (_backGroundPixelTH);}}
    property  int                ConnectedComponentDist   {int                get () {return operatingParameters->ConnectedComponentDist ();}  void  set  (int                _connectedComponentDist)   {operatingParameters->ConnectedComponentDist (_connectedComponentDist);}}
    property  bool               DataIsToBeCounted        {bool               get () {return operatingParameters->DataIsToBeCounted      ();}  void  set  (bool               _dataIsToBeCounted)        {operatingParameters->DataIsToBeCounted      (_dataIsToBeCounted);}}
    property  bool               DataIsToBeRecorded       {bool               get () {return operatingParameters->DataIsToBeRecorded     ();}  void  set  (bool               _dataIsToBeRecorded)       {operatingParameters->DataIsToBeRecorded     (_dataIsToBeRecorded);}}
    property  ScannerFileFormat  DestScannerFileFormat    {ScannerFileFormat  get ();                                                          void  set  (ScannerFileFormat  _destScannerFileFormat);}
    property  String^            DestScannerFileFormatStr {String^            get ();}
    property  int                ErosionStructSize        {int                get () {return operatingParameters->ErosionStructSize      ();}  void  set  (int                _erosionStructSize)        {operatingParameters->ErosionStructSize      (_erosionStructSize);}}
    property  bool               FlatFieldEnabled         {bool               get () {return operatingParameters->FlatFieldEnabled       ();}  void  set  (bool               _flatFieldEnabled)         {operatingParameters->FlatFieldEnabled       (_flatFieldEnabled);}}
    property  float              FlowRateFactor           {float              get () {return operatingParameters->FlowRateFactor         ();}  void  set  (float              _flowRateFactor)           {operatingParameters->FlowRateFactor         (_flowRateFactor);}}
    property  int                MinSizeThreshold         {int                get () {return operatingParameters->MinSizeThreshold       ();}  void  set  (int                _minSizeThreshold)         {operatingParameters->MinSizeThreshold       (_minSizeThreshold);}}
    property  bool               PlayingBack              {bool               get () {return operatingParameters->PlayingBack            ();}  void  set  (bool               _playingBack)              {operatingParameters->PlayingBack            (_playingBack);}}
    property  bool               SaveParticleImages       {bool               get () {return operatingParameters->SaveParticleImages     ();}  void  set  (bool               _saveParticleImages)       {operatingParameters->SaveParticleImages     (_saveParticleImages);}}

    property  float              RequestedAnalogGain      {float              get () {return operatingParameters->RequestedAnalogGain    ();}  void  set  (float              _requestedAnalogGain)      {operatingParameters->RequestedAnalogGain    (_requestedAnalogGain);}}
    property  int                RequestedDigitalGain     {int                get () {return operatingParameters->RequestedDigitalGain   ();}  void  set  (int                _requestedDigitalGain)     {operatingParameters->RequestedDigitalGain   (_requestedDigitalGain);}}
    property  String^            RequestedSensitivityMode {String^            get ();                                                          void  set  (String^            _requestedSensitivityMode);}
    property  float              RequestedScanRate        {float              get () {return operatingParameters->RequestedScanRate      ();}  void  set  (float              _requestedScanRate)        {operatingParameters->RequestedScanRate      (_requestedScanRate);}}

    void  UpdateFromDataField (String^  fieldName,
                               String^  fieldValue,
                               bool%    fieldFound
                              );

    void  UpdateFromScannerFile (UmiScannerFile^ sf);


    void  WriteFieldValues (System::IO::StreamWriter^  sr);

  private:
    OperatingParametersPtr  operatingParameters;
  };  /* UmiOperatingParameters */



  public  ref  class  UmiOperatingParametersList: public List<UmiOperatingParameters^>
  {
  public:
    UmiOperatingParametersList ();
  };  /* UmiOperatingParametersList */

}
