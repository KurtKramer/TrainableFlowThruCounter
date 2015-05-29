/*
 * Copyright (C) 2011-2014  Kurt Kramer
 * For conditions of distribution and use, see copyright notice in LarcosCounterUnManaged.txt
 */


#if  !defined(_SESSIONPARAMETERS_)
#define  _SESSIONPARAMETERS_

#include "KKBaseTypes.h"
using namespace  KKB;

#include "ScannerFile.h"
using namespace  KKLSC;

#include "LarcosDataRec.h"


namespace LarcosCounterUnManaged
{
  /** 
   *@brief  Parameters that are specific to a single recording session such as a Control Number, SessionId,
   * From(Source) To(destination), etc.  This data will be recorded in the header fields of the respective
   * Scanner File and in the "LarcosCounterManager" config file.  The one in "LarcosCounterManager" is supposed
   * to be the values selected for the last recording session started.
   */
  class SessionParameters: public LarcosDataRec
  {
  public:
    typedef  ScannerFile::Format  ScannerFileFormat;

    SessionParameters ();

    virtual ~SessionParameters ();

    virtual  int  MemoryConsumedEstimated ()  const;

    void  Assign (const SessionParameters&  parms);

    void  SessionId            (kkint32        _sessionId)            {sessionId           = _sessionId;}
    void  Comments             (const KKStr&   _comments)             {comments            = _comments;}
    void  DateTimeRecorded     (KKB::DateTime  _dateTimeRecorded)     {dateTimeRecorded    = _dateTimeRecorded;}
    void  Destination          (const KKStr&   _destination)          {destination         = _destination;}
    void  DestScannerFileName  (const KKStr&   _destScannerFileName)  {destScannerFileName = _destScannerFileName;}
    void  HostName             (const KKStr&   _hostName)             {hostName            = _hostName;}
    void  PersonInCharge       (const KKStr&   _personInCharge)       {personInCharge      = _personInCharge;}
    void  SessionDescription   (const KKStr&   _sessionDescription)   {sessionDescription  = _sessionDescription;}
    void  ControlNum           (const KKStr&   _controlNum)           {controlNum          = _controlNum;}
    void  Source               (const KKStr&   _source)               {source              = _source;}
    void  TrainingModelName    (const KKStr&   _trainingModelName)    {trainingModelName   = _trainingModelName;}

    kkint32               SessionId            ()  const  {return sessionId;}

    const KKStr&          Comments             ()  const  {return comments;}
    const KKB::DateTime&  DateTimeRecorded     ()  const  {return dateTimeRecorded;}
    const KKStr&          Destination          ()  const  {return destination;}
    const KKStr&          DestScannerFileName  ()  const  {return destScannerFileName;}
    const KKStr&          HostName             ()  const  {return hostName;}
    const KKStr&          PersonInCharge       ()  const  {return personInCharge;}
    const KKStr&          SessionDescription   ()  const  {return sessionDescription;}
    const KKStr&          ControlNum           ()  const  {return controlNum;}
    const KKStr&          Source               ()  const  {return source;}
    const KKStr&          TrainingModelName    ()  const  {return trainingModelName;}



    /**
     *@brief Adds the data fields from this instance to the specified header fields which will be included
     * in a recorded ScannerFile.
     */
    virtual
    void  AddToHeaderFields (ScannerHeaderFieldsPtr  headerFields);


    /**
     *@brief Updates member fields from header Fields in specified ScannerFile(sf).
     *@details Earlier version of Larcos did not Precede Session Parameter fields with "SP:" prefix so if 
     * a given field is not found with the prefix will then try looking for field value without prefix.
     */
    virtual
    void  UpdateFromScannerFile (ScannerFilePtr  sf);


    /**
     *@brief  Used to update appropriate member field from name/value pair as would be retrieved from a configuration file.
     *@param[in]  fieldName   Name of field/member to be updated; should be same name as member name.
     *@param[in]  fieldValue  Value of field that is being updated.
     *@param[out] fieldFound  Indicates if 'fieldName' is one of the members of this class.
     */
    virtual
    void  UpdateFromDataField (const KKStr&  fieldName,
                               const KKStr&  fieldValue,
                               bool&         fieldFound
                              );

    /**
     *@brief  Writes out FieldName/Value pairs to specified output sstream; there will be one line
     * per data field with Field-Name and its respective value separated by tab(\t) characters.
     */
    virtual
    void  WriteFieldValues (ostream&   o)  const;


    /**
     *@brief  Writes a very simple xml record with the 1st and last lines consisting of <SessionParameters>
     * and </SessionParameters> with the data fields in-between  written by a call to 'WriteFieldValues'.
     */
    void  WriteXML (ostream&  o)  const;

    /**
     *@brief  Will update the data fields in this instance by reading lines from provided input stream until
     * the line containing "</SessionParameters>" is encountered.
     *@details Each line that is read will be divided into FieldName separated by Field-Value where the 1st
     * tab('\t') character encounter will separate the two fields.  Field-Value may contain multiple instances of
     * tab characters.
     */
    void  ReadXML (istream&  i);


  private:
     /**
      *@brief Will retrieve value of 'fieldName' fist as a SessionParameter field ("SP:") and if not found then as 
      * generic field value.
      *@details  In earlier version of Larcos recorded each field in header with just field name;  but later on we started to add
      * prefix to denote section such as 'SP:' for Session Parameters.
      */
     virtual
     const KKStr&  GetScannerFileValue (ScannerFilePtr  sf, 
                                        const KKStr&    fieldName
                                       );



     kkint32         sessionId;                /**< This is a sequence number that will be assigned when a
                                                * new recording session is started; its value comes from the 
                                                * "LastSessionId.txt" file in the configuration directory.
                                                */
    
     KKStr           comments;
     KKStr           controlNum;               // *** Rename to short description
     KKB::DateTime   dateTimeRecorded;         /**< Date and Time that Scanner file was recorded.       */
     KKStr           destination;              /**< Destination for Shrimp being counted.               */
     KKStr           destScannerFileName;      /**< Where the Scanner file was written to.              */
     KKStr           hostName;                 /**< Name of computer that recording session was ran on. */
     KKStr           personInCharge;           /**< Who is responsible for the movement of shrimp.      */
     KKStr           sessionDescription;
     KKStr           source;                   /**< Where are the shrimp being moved from.              */
     KKStr           trainingModelName;
  };  /* SessionParameters */

  typedef SessionParameters*  SessionParametersPtr;

#define  _SessionParameters_Defined_

}  /* LarcosCounterUnManaged */



#endif

