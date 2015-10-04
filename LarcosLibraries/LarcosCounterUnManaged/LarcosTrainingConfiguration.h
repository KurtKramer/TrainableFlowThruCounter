#if  !defined(_LARCOSTRAININGCONFIGURATION_)
#define  _LARCOSTRAININGCONFIGURATION_

#include "XmlStream.h"

#include "FileDesc.h"
#include "TrainingConfiguration2.h"

using namespace KKMLL;

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
    typedef  LarcosTrainingConfiguration const  LarcosTrainingConfigurationConst;
    typedef  LarcosTrainingConfigurationConst  *  LarcosTrainingConfigurationConstPtr;


    LarcosTrainingConfiguration ();


    LarcosTrainingConfiguration (const LarcosTrainingConfiguration&  tc);


    /**
     *@brief Use this one if you want to create a default Configuration object.
     */
    LarcosTrainingConfiguration (MLClassListPtr          _mlClasses,
                                 KKStr                   _parameterStr,
                                 OperatingParametersPtr  _initialOperatingParameters,
                                 RunLog&                 _log
                                );

    ~LarcosTrainingConfiguration ();


    virtual
    void  Load (const KKStr&           _configFileName,
                OperatingParametersPtr _initialOperatingParameters,
                bool                   _validateDirectories,  /**<  Used to default to 'true'. */
                RunLog&                _log
               );


    virtual
    LarcosTrainingConfigurationPtr  Duplicate ()  const;


    virtual
    FactoryFVProducerPtr   DefaultFeatureVectorProducer (RunLog&  runLog)  const;

    static
    LarcosTrainingConfiguration*  CreateFromDirectoryStructure 
                                            (const KKStr&            _existingConfigFileName,
                                             const KKStr&            _subDir,
                                             OperatingParametersPtr  _initialOperatingParameters,
                                             RunLog&                 _log,
                                             bool&                   _successful,
                                             KKStr&                  _errorMessage
                                            );

    static
    LarcosTrainingConfiguration*  CreateFromFeatureVectorList (FeatureVectorList&      _examples,
                                                               OperatingParametersPtr  _initialOperatingParameters,
                                                               RunLog&                 _log
                                                              );

    virtual
    void  Save (const KKStr&  fileName)  const;

    const OperatingParameters*  OperatingParms ()  const  {return operatingParms;}

    virtual void  ReadXML (XmlStream&      s,
                           XmlTagConstPtr  tag,
                           VolConstBool&   cancelFlag,
                           RunLog&         log
                          );

    /** @brief  Update 'operatingParms' from  '_operatingParms'.  */
    void  SetOperatingParms (const OperatingParameters&  _operatingParms);

    void  WriteXML (const KKStr&   varName,
                    std::ostream&  o
                   )  const;


  private:
    OperatingParametersPtr  operatingParms;

  };  /* LarcosTrainingConfiguration */


  #define  _LarcosTrainingConfigurationDefined_

  typedef  LarcosTrainingConfiguration::LarcosTrainingConfigurationPtr       LarcosTrainingConfigurationPtr;
  typedef  LarcosTrainingConfiguration::LarcosTrainingConfigurationConst     LarcosTrainingConfigurationConst;
  typedef  LarcosTrainingConfiguration::LarcosTrainingConfigurationConstPtr  LarcosTrainingConfigurationConstPtr;



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


  class  XmlElementLarcosTrainingConfiguration: public XmlElementTrainingConfiguration2
  {
  public:
    XmlElementLarcosTrainingConfiguration (XmlTagPtr      tag,
                                           XmlStream&     s,
                                           VolConstBool&  cancelFlag,
                                           RunLog&        log
                                          ):
          XmlElementTrainingConfiguration2 (tag, s, cancelFlag, log)
    {}

  };
  
  typedef  XmlElementLarcosTrainingConfiguration*  XmlElementLarcosTrainingConfigurationPtr;

}  /* namespace LarcosCounterUnManaged */

#endif
