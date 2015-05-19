/* OperatingParameters.cpp -- Represents the operating parameters used by Larcos; ex's:  BackGroundThreshold, FlowRateFactor.
 * Copyright (C) 2011-2013  Kurt Kramer
 * For conditions of distribution and use, see copyright notice in LarcosCounterUnManaged.txt
 */


#if  !defined(_OPERATINGPARAMETERS_)
#define  _OPERATINGPARAMETERS_

#include "KKBaseTypes.h"
using namespace  KKB;

#include "ScannerFile.h"
using namespace  KKLSC;


#include "LarcosDataRec.h"


namespace LarcosCounterUnManaged
{

  #if  !defined(_LARCOSTRAININGCONFIGURATION_)
    class  LarcosTrainingConfiguration;
    typedef  LarcosTrainingConfiguration*  LarcosTrainingConfigurationPtr;
  #endif


  /** @brief  Parameters required to perform either a recording or play-back session .*/
  class  OperatingParameters:  public  LarcosDataRec
  {
  public:
    typedef  ScannerFile::ScannerFileFormat  ScannerFileFormat;

    OperatingParameters ();

    OperatingParameters (const OperatingParameters&  opParms);

    virtual ~OperatingParameters ();

    virtual  int  MemoryConsumedEstimated ()  const;

    void  Assign (const OperatingParameters&  parms);

    void  UpdateFromConfiguration (const LarcosTrainingConfiguration& c);


    void  BackGroundPixelTH        (uchar              _backGroundPixelTH)        {backGroundPixelTH        = _backGroundPixelTH;}
    void  ConnectedComponentDist   (kkint32            _connectedComponentDist)   {connectedComponentDist   = _connectedComponentDist;}
    void  DataIsToBeCounted        (bool               _dataIsToBeCounted)        {dataIsToBeCounted        = _dataIsToBeCounted;}
    void  DataIsToBeRecorded       (bool               _dataIsToBeRecorded)       {dataIsToBeRecorded       = _dataIsToBeRecorded;}
    void  DestScannerFileFormat    (ScannerFileFormat  _destScannerFileFormat)    {destScannerFileFormat    = _destScannerFileFormat;}
    void  ErosionStructSize        (kkint32            _erosionStructSize)        {erosionStructSize        = _erosionStructSize;}
    void  FlatFieldEnabled         (bool               _flatFieldEnabled)         {flatFieldEnabled         = _flatFieldEnabled;}
    void  FlowRateFactor           (float              _flowRateFactor)           {flowRateFactor           = _flowRateFactor;}
    void  MinSizeThreshold         (kkint32            _minSizeThreshold)         {minSizeThreshold         = _minSizeThreshold;}
    void  PlayingBack              (bool               _playingBack)              {playingBack              = _playingBack;}
    void  SaveParticleImages       (bool               _saveParticleImages)       {saveParticleImages       = _saveParticleImages;}
    void  RequestedAnalogGain      (float              _requestedAnalogGain)      {requestedAnalogGain      = _requestedAnalogGain;}
    void  RequestedDigitalGain     (kkint32            _requestedDigitalGain)     {requestedDigitalGain     = _requestedDigitalGain;}
    void  RequestedSensitivityMode (const KKStr&       _requestedSensitivityMode) {requestedSensitivityMode = _requestedSensitivityMode;}
    void  RequestedScanRate        (float              _requestedScanRate)        {requestedScanRate        = _requestedScanRate;}

    uchar              BackGroundPixelTH        ()  const  {return backGroundPixelTH;}
    kkint32            ConnectedComponentDist   ()  const  {return connectedComponentDist;}
    bool               DataIsToBeCounted        ()  const  {return dataIsToBeCounted;}
    bool               DataIsToBeRecorded       ()  const  {return dataIsToBeRecorded;}
    ScannerFileFormat  DestScannerFileFormat    ()  const  {return destScannerFileFormat;}
    const KKStr&       DestScannerFileFormatStr ()  const;
    kkint32            ErosionStructSize        ()  const  {return erosionStructSize;}
    bool               FlatFieldEnabled         ()  const  {return flatFieldEnabled;}
    float              FlowRateFactor           ()  const  {return flowRateFactor;}
    kkint32            MinSizeThreshold         ()  const  {return minSizeThreshold;}
    bool               PlayingBack              ()  const  {return playingBack;}
    bool               SaveParticleImages       ()  const  {return saveParticleImages;}
    float              RequestedAnalogGain      ()  const  {return requestedAnalogGain;}
    kkint32            RequestedDigitalGain     ()  const  {return requestedDigitalGain;}
    const KKStr&       RequestedSensitivityMode ()  const  {return requestedSensitivityMode;}
    float              RequestedScanRate        ()  const  {return requestedScanRate;}

    virtual
    void  AddToHeaderFields (ScannerHeaderFieldsPtr  headerFields);

    /**
     *@brief Updates member fields from header Fields in specified ScannerFile(sf).
     *@details Earlier version of Larcos did not proceed Session Parameter fields with "SP:" prefix so if 
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
     *@brief  Validates all the fields;  errors found will be appended to '_errMsg' with a '\n' between each error.
     */
    void  ValidateParameters (KKStr&  _errMsg);


    virtual
    void  WriteFieldValues (ostream&   o)  const;


    /**
     *@brief  Will write the 'OperatingParameters' section of the Training Model Config section.
     */
    void  WriteConfigSection (ostream&   o)  const;



    void  WriteToRunLog (const KKStr&  prefix,
                         RunLog&       runLog
                        )  const;

    /**
     *@brief  Will write the contents of this instance in the specified output stream such that the ReadXML method will be able to read.
     */
    void  WriteXML (ostream&  o)  const;

    void  WriteXML (const KKStr&  varName,
                    ostream&      o
                   )  const;

    void  ReadXML (istream&  i);

    void  ReadXML (XmlStream&      s,
                   XmlTagConstPtr  tag,
                   RunLog&         log
                  );

  private:
    KKStrConstPtr GetSettingValue (const LarcosTrainingConfiguration&  c,
                                   const KKStr&                        fieldName
                                  );


    /**
     *@brief Will retrieve value of 'fieldName' fist as a SessionParameter field then as generic field value.
     *@details  In earlier version of Larcos recorded each field in header with just name;  but later on we started to add
     * prefix to denote section such as 'SP:' for Session Parameters.
     */
    const KKStr&  GetScannerFileValue (ScannerFilePtr  sf, 
                                       const KKStr&    fieldName
                                      );

    uchar              backGroundPixelTH;
    kkint32            connectedComponentDist;
    bool               dataIsToBeCounted;       /**< When adding data to frame buffer flag it to be processed for counting.  */
    bool               dataIsToBeRecorded;      /**< When adding data to frame buffer flag it to be written to disk.         */
    ScannerFileFormat  destScannerFileFormat;
    kkint32            erosionStructSize;
    bool               flatFieldEnabled;
    float              flowRateFactor;
    kkint32            minSizeThreshold;
    bool               playingBack;
    bool               saveParticleImages;
    float              requestedAnalogGain;
    kkint32            requestedDigitalGain;
    KKStr              requestedSensitivityMode;
    float              requestedScanRate;
  };  /* OperatingParameters */

  typedef OperatingParameters*  OperatingParametersPtr;

#define  _OperatingParameters_Defined_


  typedef  XmlElementModelTemplate<OperatingParameters>  XmlElementOperatingParameters;
  typedef  XmlElementOperatingParameters*  XmlElementOperatingParametersPtr;
}  /* LarcosCounterUnManaged */



#endif

