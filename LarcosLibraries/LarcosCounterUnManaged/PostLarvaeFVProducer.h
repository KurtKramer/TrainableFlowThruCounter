#if  !defined(_POSTLARVAEFVPRODUCER_)
#define  _POSTLARVAEFVPRODUCER_

#include "KKBaseTypes.h"
#include "RunLog.h"
using namespace  KKB;

#include "FeatureVectorProducer.h"
using namespace  KKMachineLearning;

#include "CameraThread.h"
#include "PreClassDefinitions.h"

namespace LarcosCounterUnManaged
{
  class PostLarvaeFVProducer:  public FeatureVectorProducer
  {
  public:
    PostLarvaeFVProducer ();
    virtual ~PostLarvaeFVProducer ();


    virtual  FeatureVectorPtr  ComputeFeatureVector (RasterPtr  image,
                                                     RunLog&    runLog
                                                    );

    /**
     *@brief  Returns the 'type_info' of the Feature Vector that this instance of 'FeatureComputer' creates.
     */
    virtual  const type_info*  FeatureVectorTypeId () const;



  private:
    


    KKStr     name;

    kkuint32  totPixsForMorphOps;  /**<  When this instance is created this is the amount of memory each 
                                    * raster work area will be restricted to.  The Height and width will 
                                    * be adjusted such that the resultant dimensions will fit within this
                                    * constraint.
                                    */
  };  /* PostLarvaeFVProducer */


}  /* LarcosCounterUnManaged */


#endif
