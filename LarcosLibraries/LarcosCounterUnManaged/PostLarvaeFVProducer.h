#if  !defined(_POSTLARVAEFVPRODUCER_)
#define  _POSTLARVAEFVPRODUCER_

#include "KKBaseTypes.h"
#include "Raster.h"
#include "RunLog.h"
using namespace  KKB;

#include "FactoryFVProducer.h"
#include "FeatureVector.h"
#include "FeatureVectorProducer.h"
using namespace  KKMachineLearning;

#include "CameraThread.h"
#include "PostLarvaeFV.h"
#include "PreClassDefinitions.h"


namespace LarcosCounterUnManaged
{
  class PostLarvaeFVProducer:  public FeatureVectorProducer
  {
  public:
    PostLarvaeFVProducer ();

    virtual ~PostLarvaeFVProducer ();


    virtual  PostLarvaeFVPtr  ComputeFeatureVector (Raster&           image,
                                                    const MLClassPtr  knownClass,
                                                    RasterListPtr     intermediateImages,
                                                    RunLog&           runLog
                                                   );


    /**
     *@brief  Returns the 'type_info' of the Feature Vector that this instance of 'FeatureComputer' creates.
     */
    virtual  const type_info*  FeatureVectorTypeId () const;



  private:
    kkuint32  totPixsForMorphOps;  /**<  When this instance is created this is the amount of memory each 
                                    * raster work area will be restricted to.  The Height and width will 
                                    * be adjusted such that the resultant dimensions will fit within this
                                    * constraint.
                                    */
  };  /* PostLarvaeFVProducer */

  typedef  PostLarvaeFVProducer*  PostLarvaeFVProducerPtr;

#define  _PostLarvaeFVProducer_Defined_






  class  PostLarvaeFVProducerFactory:  public  FactoryFVProducer
  {
    typedef  PostLarvaeFVProducerFactory*  PostLarvaeFVProducerFactoryPtr;

    PostLarvaeFVProducerFactory ();

  protected:
    /**
     *@brief  A Factory can neer be deleted until the application terminates;  the atexit method will perform the deletes.
     */
    virtual ~PostLarvaeFVProducerFactory ();

  public:

    virtual  FeatureFileIOPtr  DefaultFeatureFileIO ()  const;

    virtual  FileDescPtr  FileDesc ()  const;

    virtual  PostLarvaeFVProducerPtr  ManufactureInstance (RunLog&  runLog);


    /**
     *@brief Manufactures a instance of a 'PostLarvaeFVList' class that will own its contents.
     */
    virtual  PostLarvaeFVListPtr  ManufacturFeatureVectorList (bool     owner,
                                                               RunLog&  runLog
                                                              );

    /**
     *@brief  Returns instance of "PostLarvaeFVProducerFactory"  that is registered with "FactoryFVProducer::RegisterFactory".
     *@details If instance does not exist yet oit will create an instance and register it.
     */
    static  PostLarvaeFVProducerFactory*  Factory (RunLog*  runLog);


  private:
    /**
     *@brief  This instance of 'PostLarvaeFVProducerFactory'  will be registerd with "FactoryFVProducer::RegisterFactory".  
     *@details  It will deleyed by the "FactoryFVProducer::FinaleCleanUp" upon application shutdown.
     */
    static  PostLarvaeFVProducerFactory*  factory;

  };  /* PostLarvaeFVProducerFactory */

#define  _PostLarvaeFVProducerFactory_Defined_


}  /* LarcosCounterUnManaged */


#endif
