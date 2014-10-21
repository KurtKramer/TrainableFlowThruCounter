#if  !defined(_LARCOSTRAININGCONFIGURATION_)
#define  _LARCOSTRAININGCONFIGURATION_

#include "FileDesc.h"
#include "TrainingConfiguration2.h"

using namespace KKMachineLearning;

namespace  LarcosCounterUnManaged 
{
  #if  !defined(_OperatingParameters_Defined_)
    class  OperatingParameters;
    typedef  OperatingParameters*  OperatingParametersPtr;
  #endif


  class  LarcosTrainingConfiguration:  public  TrainingConfiguration2
  {
  public:
    typedef  LarcosTrainingConfiguration*  LarcosTrainingConfigurationPtr;


    LarcosTrainingConfiguration (FileDescPtr            _fileDesc,
                                 const KKStr&           _configFileName,
                                 OperatingParametersPtr _initialOperatingParameters,
                                 RunLog&                _log,
                                 bool                   validateDirectories = true
                                );


    LarcosTrainingConfiguration (const LarcosTrainingConfiguration&  tc);


    /**
     *@brief Use this one if you want to create a default Configuration object.
     */
    LarcosTrainingConfiguration (FileDescPtr             _fileDesc,
                                 MLClassListPtr          _mlClasses,
                                 KKStr                   _parameterStr,
                                 OperatingParametersPtr  _initialOperatingParameters,
                                 RunLog&                 _log
                                );

    ~LarcosTrainingConfiguration ();


    static
    LarcosTrainingConfiguration*  CreateFromDirectoryStructure 
                                            (FileDescPtr             _fileDesc,
                                             const KKStr&            _existingConfigFileName,
                                             const KKStr&            _subDir,
                                             OperatingParametersPtr  _initialOperatingParameters,
                                             RunLog&                 _log,
                                             bool&                   _successful,
                                             KKStr&                  _errorMessage
                                            );

    static
    LarcosTrainingConfiguration*  CreateFromFeatureVectorList (FeatureVectorList&      _examples,
                                                               MLClassListPtr          _mlClasses,
                                                               OperatingParametersPtr  _initialOperatingParameters,
                                                               RunLog&                 _log
                                                              );

    virtual
    void  Save (const KKStr&  fileName);

    const OperatingParameters*  OperatingParms ()  const  {return operatingParms;}


    /** @brief  Update 'operatingParms' from  '_operatingParms'.  */
    void  SetOperatingParms (const OperatingParameters&  _operatingParms);


  private:
    OperatingParametersPtr  operatingParms;

  };  /* LarcosTrainingConfiguration */


  #define  _LarcosTrainingConfigurationDefined_

  typedef  LarcosTrainingConfiguration::LarcosTrainingConfigurationPtr  LarcosTrainingConfigurationPtr;




  class  LarcosTrainingConfigurationList: public KKQueue<LarcosTrainingConfiguration>
  {
  public:
    LarcosTrainingConfigurationList (bool _owner):
        KKQueue<LarcosTrainingConfiguration> (_owner)
        {}

    ~LarcosTrainingConfigurationList ()
        {}

  };


  typedef  LarcosTrainingConfigurationList*  TrainingConfiguration2ListPtr;

}  /* namespace LarcosCounterUnManaged */

#endif
