#if  !defined(_COUNTERTRAININGCONFIGURATION_)
#define  _COUNTERTRAININGCONFIGURATION_

#include "XmlStream.h"

#include "FileDesc.h"
#include "TrainingConfiguration2.h"

using namespace KKMLL;

namespace  CounterUnManaged 
{
  #if  !defined(_OperatingParameters_Defined_)
    class  OperatingParameters;
    typedef  OperatingParameters*  OperatingParametersPtr;
  #endif


  class  CounterTrainingConfiguration:  public  TrainingConfiguration2
  {
  public:
    typedef  CounterTrainingConfiguration*  CounterTrainingConfigurationPtr;
    typedef  CounterTrainingConfiguration const  CounterTrainingConfigurationConst;
    typedef  CounterTrainingConfigurationConst  *  CounterTrainingConfigurationConstPtr;


    CounterTrainingConfiguration ();


    CounterTrainingConfiguration (const CounterTrainingConfiguration&  tc);


    /**
     *@brief Use this one if you want to create a default Configuration object.
     */
    CounterTrainingConfiguration (MLClassListPtr          _mlClasses,
                                  KKStr                   _parameterStr,
                                  OperatingParametersPtr  _initialOperatingParameters,
                                  RunLog&                 _log
                                 );

    ~CounterTrainingConfiguration ();


    virtual
    void  Load (const KKStr&           _configFileName,
                OperatingParametersPtr _initialOperatingParameters,
                bool                   _validateDirectories,  /**<  Used to default to 'true'. */
                RunLog&                _log
               );


    virtual
    CounterTrainingConfigurationPtr  Duplicate ()  const;


    virtual
    FactoryFVProducerPtr   DefaultFeatureVectorProducer (RunLog&  runLog)  const;

    static
    CounterTrainingConfiguration*  CreateFromDirectoryStructure 
                                            (const KKStr&            _existingConfigFileName,
                                             const KKStr&            _subDir,
                                             OperatingParametersPtr  _initialOperatingParameters,
                                             RunLog&                 _log,
                                             bool&                   _successful,
                                             KKStr&                  _errorMessage
                                            );

    static
    CounterTrainingConfiguration*  CreateFromFeatureVectorList (FeatureVectorList&      _examples,
                                                                OperatingParametersPtr  _initialOperatingParameters,
                                                                RunLog&                 _log
                                                               );

    virtual
    void  Save (const KKStr&  fileNameToSaveTo)  const;

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

  };  /* CounterTrainingConfiguration */


  #define  _CounterTrainingConfigurationDefined_

  typedef  CounterTrainingConfiguration::CounterTrainingConfigurationPtr       CounterTrainingConfigurationPtr;
  typedef  CounterTrainingConfiguration::CounterTrainingConfigurationConst     CounterTrainingConfigurationConst;
  typedef  CounterTrainingConfiguration::CounterTrainingConfigurationConstPtr  CounterTrainingConfigurationConstPtr;



  class  CounterTrainingConfigurationList: public KKQueue<CounterTrainingConfiguration>
  {
  public:
    CounterTrainingConfigurationList (bool _owner):
        KKQueue<CounterTrainingConfiguration> (_owner)
        {}

    ~CounterTrainingConfigurationList ()
        {}

  };


  typedef  CounterTrainingConfigurationList*  TrainingConfiguration2ListPtr;


  class  XmlElementCounterTrainingConfiguration : public XmlElementTrainingConfiguration2
  {
  public:
    XmlElementCounterTrainingConfiguration (XmlTagPtr      tag,
                                            XmlStream&     s,
                                            VolConstBool&  cancelFlag,
                                            RunLog&        log
                                           ):
          XmlElementTrainingConfiguration2 (tag, s, cancelFlag, log)
    {}

  };
  
  typedef  XmlElementCounterTrainingConfiguration*  XmlElementCounterTrainingConfigurationPtr;

}  /* namespace CounterUnManaged */

#endif
