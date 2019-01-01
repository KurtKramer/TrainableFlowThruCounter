#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Collections::Generic;
using namespace System::Diagnostics;

#include "SessionParameters.h"
using namespace  CounterUnManaged;

#include "UmiScannerFile.h"


namespace CounterManaged
{
  public  ref  class  UmiSessionParameters
  {
  public:
    typedef  UmiScannerFile::Format  ScannerFileFormat;
    typedef  System::DateTime  DateTime;

    UmiSessionParameters  ();


    /// <summary>
    /// Creates a duplicate of '_parms'.
    /// </summary>
    UmiSessionParameters  (UmiSessionParameters^  _parms);


    /// <summary>
    /// Will take ownership '_parns';  caller BETTER NOT DELETE this instance.
    /// </summary>
    /// <param name="_parms">Parameters being given to this managed instance.</param>
    UmiSessionParameters  (SessionParametersPtr  _parms);

    /// <summary>
    /// Will allocate/ create a duplicate of '_parms' and own it.
    /// </summary>
    /// <param name="_parms">Parameters that are to be duplicated.</param>
    UmiSessionParameters  (const SessionParameters&  _parms);

  private:
    ~UmiSessionParameters ();

  public:
    !UmiSessionParameters ();


    SessionParametersPtr  UnManagedClass ()  {return sessionParameters;}

    void  Assign (UmiSessionParameters^  parms);

    property  int      SessionId          {int      get ();    void  set  (int     _sessionId);}

    property  String^  Comments           {String^  get ();    void  set  (String^  _comments);}
    property  DateTime DateTimeRecorded   {DateTime get ();    void  set  (DateTime _dateTimeRecorded);}
    property  String^  Destination        {String^  get ();    void  set  (String^  _destination);}
    property  String^  PersonInCharge     {String^  get ();    void  set  (String^  _personInCharge);}
    property  String^  ControlNum         {String^  get ();    void  set  (String^  _rootName);}
    property  String^  SessionDescription {String^  get ();    void  set  (String^  _sessionDescription);}
    property  String^  Source             {String^  get ();    void  set  (String^  _comments);}
    property  String^  TrainingModelName  {String^  get ();    void  set  (String^  _trainingModelName);}


    /// <summary>
    /// Used to update appropriate member field from name/value pair as would be retrieved from a configuration file.
    /// </summary>
    /// <param name="fieldName">Name of field/member to be updated; should be same name as member name.</param>
    /// <param name="fieldValue">Value of field that is being updated.</param>
    /// <param name="fieldFound">Indicates if 'fieldName' is one of the members of this class.</param>
    void  UpdateFromDataField (String^  fieldName,
                               String^  fieldValue,
                               bool%    fieldFound
                              );

    /// <summary>
    /// Updates member fields from header Fields in specified ScannerFile(sf).
    /// details Earlier version of Larcos did not proceed Session Parameter fields with "SP:" prefix so if 
    /// a given field is not found with the prefix will then try looking for field value without prefix.
    /// </summary>
    void  UpdateFromScannerFile (UmiScannerFile^ sf);

    void  WriteFieldValues (System::IO::StreamWriter^  sr);


  private:
    SessionParametersPtr  sessionParameters;
  };  /* UmiSessionParameters */



  public  ref  class  UmiSessionParametersList: public List<UmiSessionParameters^>
  {
  public:
    UmiSessionParametersList ();
  };  /* UmiSessionParametersList */

}
