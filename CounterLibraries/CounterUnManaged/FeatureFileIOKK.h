#ifndef  _FEATUREFILEIOKK_
#define  _FEATUREFILEIOKK_

#include "FeatureFileIO.h"
#include "FeatureNumList.h"

#include "CounterFeatureVector.h"


namespace CounterUnManaged 
{

  /**
    @class  FeatureFileIOKK
    @brief  Supports the reading and writing of feature data from "KK" feature data files.
    @details
    @code
    * *************************************************************************************************
    * *  The KK feature data file is a very simple format where the first few rows contain a          *
    * *  description of the rest of the data file.  Any row that starts with "//" will be treated as  *
    * *  comment line.                                                                                *
    * *  First Row:  Identifies the file as a feature Data File.  An example on the next line.        *
    * *              "FEATURE_DATA_FILE Version	101	NumOfFeatures	20	NumOfExamples  258"   *
    * *              Each field will be separated by a tab character.  This line says that the        *
    * *              version number is 101. There are 20 features and 258 examples or feature vectors.*
    * *              The version number gets incremented whenever there is a change in the way        *
    * *              that feature data is computed.                                                   *
    * *                                                                                               *
    * *  Second Row: List all the fields in the feature data file.  The first fields are feature      *
    * *              data.  The first non feature field will be: "ClassName".  There are several non  *
    * *              feature data fields.                                                             *
    * *************************************************************************************************
    @endcode
    @see  FeatureFileIO
    */
  class  FeatureFileIOKK:  public  FeatureFileIO
  {
  public:
    typedef  FeatureFileIOKK*  FeatureFileIOKKPtr;

    FeatureFileIOKK ();
    virtual ~FeatureFileIOKK ();

    virtual  FileDescConstPtr  GetFileDesc (const KKStr&    _fileName,
                                            std::istream&   _in,
                                            MLClassListPtr  _classes,
                                            kkint32&        _estSize,
                                            KKStr&          _errorMessage,
                                            RunLog&         _runLog
                                           ) override;


    virtual  LarcosFeatureVectorListPtr  LoadFile (const KKStr&      _fileName,
                                                   FileDescConstPtr  _fileDesc,
                                                   MLClassList&      _classes, 
                                                   std::istream&     _in,
                                                   OptionUInt32      _maxCount,    // Maximum # images to load.
                                                   VolConstBool&     _cancelFlag,
                                                   bool&             _changesMade,
                                                   KKStr&            _errorMessage,
                                                   RunLog&           _log
                                                  )  override;


    virtual  void   SaveFile (FeatureVectorList&    _data,
                              const KKStr&          _fileName,
                              FeatureNumListConst&  _selFeatures,
                              std::ostream&         _out,
                              kkuint32&             _numExamplesWritten,
                              VolConstBool&         _cancelFlag,
                              bool&                 _successful,
                              KKStr&                _errorMessage,
                              RunLog&               _log
                             )  override;


    static  FeatureFileIOKKPtr  Driver () {return driver;}
   


  private:
    static  FeatureFileIOKKPtr  CreateAndRegisterInstance ();


    /**
     *@brief  Will point to instance of 'FeatureFileIOKK' that is created when static members 
     * are initialized during module load.  The instance will be Registered with 'FeatureFileIO::RegisterDriver'
     * by call 'CreateAndRegisterInstance'.
     */
    static FeatureFileIOKKPtr  driver;


    enum  class FieldTypes: int
                   {FeatureData,
                    ClassName,
                    ImageClassIDX,
                    ImageFileName, 
                    OrigSize,
                    NumOfEdgePixels,
                    Probability,
                    CentroidRow,
                    CentroidCol,
                    PredictedClass,
                    PredictedClassIDX,
                    BreakTie,
                    UnKnown
                   };



    void  Parse_FEATURE_DATA_FILE_Line (KKStr&    line,
                                        kkint32&  version,
                                        kkint32&  numOfFields,
                                        kkint32&  numOfExamples
                                       );


    VectorInt  CreateIndirectionTable (const VectorKKStr&  fields,
                                       kkint32             numOfFeatures
                                      );

  };

  typedef  FeatureFileIOKK::FeatureFileIOKKPtr  FeatureFileIOKKPtr;

#define  _FeatureFileIOKK_Defined_

}  /* namespace CounterUnManaged */


#endif
