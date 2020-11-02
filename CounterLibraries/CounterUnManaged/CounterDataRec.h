/* CounterDataRec.cpp -- Represents the operating parameters used by Counter; ex's:  BackGroundThreshold, FlowRateFactor.
 * Copyright (C) 2011-2013  Kurt Kramer
 * For conditions of distribution and use, see copyright notice in CounterUnManaged.txt
 */


#if  !defined(_COUNTERDATAREC_)
#define  _COUNTERDATAREC_

#include "KKBaseTypes.h"
using namespace  KKB;

#include "ScannerFile.h"
using namespace  KKLSC;

#include "CounterTrainingConfiguration.h"

namespace CounterUnManaged
{
  /** @brief  Provides methods for adding and getting data-fields from Scanner File Header Fields .*/
  class  CounterDataRec
  {
  public:
    typedef  ScannerFile::Format  ScannerFileFormat;

    CounterDataRec (const KKStr&  _scannerFilePrefix,
                    const KKStr&  _sectionName
                   );

     CounterDataRec (const CounterDataRec&  _counterDataRec);


    virtual ~CounterDataRec ();

    virtual  kkMemSize  MemoryConsumedEstimated ()  const;

    virtual
    void  AddToHeaderFields (ScannerHeaderFieldsPtr  headerFields) = 0;

    /**
     *@brief Updates member fields from header Fields in specified ScannerFile(sf).
     *@details Earlier version of Counter did not proceed Session Parameter fields with "SP:" prefix so if 
     * a given field is not found with the prefix will then try looking for field value without prefix.
     */
    virtual
    void  UpdateFromScannerFile (ScannerFilePtr  sf) = 0;


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
                              ) = 0;


    virtual
    void  WriteFieldValues (std::ostream&   o)  const = 0;


  protected:
    KKStrConstPtr GetSettingValue (const CounterTrainingConfiguration&  c,
                                   const KKStr&                         fieldName
                                  );


    void  UpdateFromConfigurationUchar (const CounterTrainingConfiguration&  c,
                                        const KKStr&                         fieldName,
                                        uchar&                               fieldValue
                                       );


    void  UpdateFromConfigurationInt32 (const CounterTrainingConfiguration&  c,
                                        const KKStr&                         fieldName,
                                        kkint32&                             fieldValue
                                       );


    void  UpdateFromConfigurationFloat (const CounterTrainingConfiguration&  c,
                                        const KKStr&                         fieldName,
                                        float&                               fieldValue
                                       );


    void  UpdateFromConfigurationBool (const CounterTrainingConfiguration&  c,
                                       const KKStr&                         fieldName,
                                       bool&                                fieldValue
                                      );


    void  UpdateFromConfigurationKKStr (const CounterTrainingConfiguration&  c,
                                        const KKStr&                         fieldName,
                                        KKStr&                               fieldValue
                                       );


    void  UpdateFromConfigurationFileFormat (const CounterTrainingConfiguration&  c,
                                             const KKStr&                         fieldName,
                                             ScannerFile::Format&                 fieldValue
                                            );


     /**
      *@brief Will retrieve value of 'fieldName' first with 'scannerFilePrefix' field then as generic field value.
      *@details  In earlier version of Counter recorded each field in header with just name;  but later on we started to add
      * prefix to denote section such as 'SP:' for Session Parameters.
      */
     virtual
     const KKStr&  GetScannerFileValue (ScannerFilePtr  sf, 
                                        const KKStr&    fieldName
                                       );


     /**
      *@brief Will retrieve value of 'fieldName' fist as a SessionParameter field ("SP:")  and if not found then as 
      * generic field value.
      *@details  In earlier version of Counter recorded each field in header with just name;  but later on we started to add
      * prefix to denote section such as 'SP:' for Session Parameters.
      */
     KKB::DateTime  GetScannerFileValueDateTime (ScannerFilePtr  sf, 
                                                 const KKStr&    fieldName,
                                                 KKB::DateTime   defaultValue
                                                );

     uchar    GetScannerFileValueUchar (ScannerFilePtr  sf,
                                        const KKStr&    fieldName,
                                        uchar           curValue
                                       );

     kkint32  GetScannerFileValueInt32 (ScannerFilePtr  sf, 
                                        const KKStr&    fieldName,
                                        kkint32         curValue
                                       );

     float    GetScannerFileValueFloat (ScannerFilePtr  sf,
                                        const KKStr&    fieldName,
                                        float           curValue
                                       );

     bool     GetScannerFileValueBool  (ScannerFilePtr  sf, 
                                        const KKStr&    fieldName,
                                        bool            curValue
                                       );

     KKStr GetScannerFileValueKKStr    (ScannerFilePtr  sf, 
                                        const KKStr&    fieldName,
                                        KKStr           curValue
                                       );
     
     const char*  BoolToStr (bool b)  const;

  private:
    KKStr  scannerFilePrefix;
    KKStr  sectionName;
  };  /* CounterDataRec */

}  /* CounterUnManaged */



#endif

