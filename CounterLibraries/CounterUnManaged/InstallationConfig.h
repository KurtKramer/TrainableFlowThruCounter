/* InstallationConfig.h -- Represents a single Installation of Trainable Flow-Thru
 * Copyright (C) 2011-2019  Kurt Kramer
 * For conditions of distribution and use, see copyright notice in CounterUnManaged.txt
 */
#if  !defined(_INSTALLATIONCONFIG_)
#define  _INSTALLATIONCONFIG_

#include "RunLog.h"

#if  !defined(_SCANNERFILE_)
namespace  KKLSC
{
  class  ScannerFile;
  typedef  ScannerFile*  ScannerFilePtr;
}
#endif


#if  !defined(_SCANNERHEADERFIELDS_)
namespace  KKLSC
{
  class  ScannerHeaderFields;
  typedef  ScannerHeaderFields*  ScannerHeaderFieldsPtr;
}
#endif

#include "CounterDataRec.h"



namespace CounterUnManaged
{
  /**
   * @class  InstallationConfig
   */
  class  InstallationConfig: public  CounterDataRec
  {
  public:
    typedef  InstallationConfig*  InstallationConfigPtr;

    enum  class  FlowMeterMethods: int {Null, Embedded, EndOfList};

    static const KKStr&      FlowMeterMethodToStr   (FlowMeterMethods  method);
    static FlowMeterMethods  FlowMeterMethodFromStr (const KKStr&  s);

    static  KKStr  InstallationDirPath ();

    InstallationConfig (RunLog&  runLog);

    InstallationConfig (const KKB::KKStr&  _name,
                        bool&              _successful,
                        RunLog&            _log
                       );

    InstallationConfig (const InstallationConfig&  _entry,
                        RunLog&                    _runLog
                       );

    virtual
      ~InstallationConfig ();

    virtual  kkMemSize  MemoryConsumedEstimated ()  const;

    void  Assign (const InstallationConfig&  parms);

    const KKStr&      Description               () const {return description;}
    bool              FlowMeterPresent          () const;
    FlowMeterMethods  FlowMeterMethod           () const {return flowMeterMethod;}
    const KKStr&      FlowMeterMethodStr        () const;
    float             FlowMeterTicsPerMeter     () const {return flowMeterTicsPerMeter;}
    float             ImagingChamberWidth       () const {return imagingChamberWidth;}
    kkint32           ImagingChamberWidthPixels () const {return imagingChamberWidthPixels;}
    const KKStr&      CameraMacAddress          () const {return cameraMacAddress;}
    const KKStr&      Name                      () const {return name;}
 

    /**
     * @brief  Used to add Installation specific fields to a ScannerFile header fields.
     * @details Will be called by "CounterManager::AddHeaderFields" to add header fields for the specific installation 
     *  that is building a ScannerFile.  When playing back a Scanner file "CounterManager" will be able to reconstruct
     *  a instance of 'InstallationConfig' from these header fields.
     * @param[in] headerFields  The header Fields structure that header fields are to be added to.
     * @sa CounterManager::AddHeaderFields
     * @sa ScannerHeaderFields
     */
    virtual
    void  AddToHeaderFields (ScannerHeaderFieldsPtr  headerFields);


    /**
     *@brief Updates member fields from header Fields in specified ScannerFile(sf).
     *@details Earlier version of Counter did not process Session Parameter fields with "SP:" prefix so if 
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


    virtual
    void  WriteFieldValues (std::ostream&   o)  const;

    void  WriteXML (std::ostream&  o)  const;

    void  ReadXML (std::istream&  i);


    void  Load (const KKStr&  _name,
                bool&         _successful,
                RunLog        _log
               );

    void  Save (RunLog& _log)  const;


  private:
    static  KKStr  flowMeterMethodStrs[];
    static  KKStr  installationDirPath;
    static  KKStr  DeriveFullFleName (const KKStr& _name);

    KKStr             cameraMacAddress;
    KKStr             description;
    FlowMeterMethods  flowMeterMethod;
    float             flowMeterTicsPerMeter;
    float             imagingChamberWidth;
    kkint32           imagingChamberWidthPixels;
    KKStr             name;
  };  /* InstallationConfig */



  typedef  InstallationConfig*  InstallationConfigPtr;

  class  InstallationConfigList: public KKQueue<InstallationConfig>
  {
  public:
    typedef  InstallationConfigList* InstallationConfigListPtr;

    InstallationConfigList (bool  owner);
    ~InstallationConfigList ();

    static  InstallationConfigListPtr  GetListOfInstallationConfig (RunLog&  _log);

  };  /* InstallationConfigList */

  typedef  InstallationConfigList::InstallationConfigListPtr   InstallationConfigListPtr;
  


}  /* CounterUnManaged */

#endif
