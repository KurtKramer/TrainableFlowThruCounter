#ifndef  _POSTLARVAEFV_
#define  _POSTLARVAEFV_

/**
 *@class CounterUnManaged::PostLarvaeFV
 *@brief  Specialized version of KKMLL::FeatureVector that will be used 
 *to represent the features of a POST Larvae image.
 *@author  Kurt Kramer
 *@details
 * Used for the representation of a Single Plankton Image. You create an instance of this object for 
 * each single image you need to keep track of. There is a specialized version of KKMLL::FeatureFileIO 
 * called  KKMLL::FeatureFileIOKK that is used to write and read feature Data files. What makes this 
 * class of KKMLL::FeatureVector special are the additional fields that are Plankton specific such as 
 * centroidCol, centroidRow, latitude, longitude, numOfEdgePixels, Centroid within SIPPEER file 
 * sfCentroidCol, sfCentroidRow and version.<p>
 *
 * The version number field is supposed to indicate which feature calculation routines were used.  This 
 * way if there are changes the way features are calculated it can be detected during runtime if the features
 * are up to date they need to be recomputed.
*/


#include "BMPImage.h"
#include "KKQueue.h"
#include "Raster.h"
#include "RunLog.h"
#include "KKStr.h"
using namespace KKB;
            

#include "Attribute.h"
#include "ClassStatistic.h"
#include "FeatureNumList.h"
#include "FeatureVector.h"
#include "FileDesc.h"
#include "MLClass.h"
using namespace  KKMLL;

#define   CurrentFeatureFileVersionNum  316


namespace CounterUnManaged 
{
  void  PostLarvaeFVResetDarkSpotCounts ();

  void  PostLarvaeFVPrintReport (std::ostream& o);

  void  PostLarvaeFVAddBlobList (MLClassPtr        c,
                                 KKB::BlobListPtr  blobs 
                                );


  class  PostLarvaeFV:  public  FeatureVector 
  {
  public:
    typedef  PostLarvaeFV*  PostLarvaeFVPtr;
    typedef  KKB::uchar  uchar;

    //PostLarvaeFV (MLClassPtr  mlClass);

    PostLarvaeFV (kkuint32  _numOfFeatures);

    PostLarvaeFV (const PostLarvaeFV&  _image);


    PostLarvaeFV (      Raster&     _raster,
                  const MLClassPtr  _mlClass,
                  RasterListPtr     _intermediateImages
                 );

    PostLarvaeFV (const BmpImage&   _image,
                  const MLClassPtr  _mlClass,
                  RasterListPtr     _intermediateImages
                 );


    PostLarvaeFV (KKStr          _fileName,
                  MLClassPtr     _mlClass,
                  bool&          _successfull,
                  RasterListPtr  _intermediateImages
                 );


    /**
     *@brief  Smart copy constructor that will detect the underlying type of the source instance.
     *@details
     *@code
     *************************************************************************************
     ** This constructor will detect what the underlying type of 'featureVector' is.     *
     ** If (underlying type is a 'PostLarvaeFV' object)  then                            *
     **   | Information that is particular to a 'PostLarvaeFV' object will be extracted  *
     **   | from the 'FeatureVector' object.                                             *
     ** else                                                                             *
     **   | Info that is particular to a 'PostLarvaeFV' object will be set to default    *
     **   | values.                                                                      *
     *************************************************************************************
     *@endcode
     */
    PostLarvaeFV (const FeatureVector&  featureVector);

    virtual  ~PostLarvaeFV ();

    virtual  PostLarvaeFVPtr  Duplicate ()  const;

    // Access Methods.
    void  CentroidCol      (float    _centroidCol)      {centroidCol      = _centroidCol;}
    void  CentroidRow      (float    _centroidRow)      {centroidRow      = _centroidRow;}
    void  NumOfEdgePixels  (kkint32  _numOfEdgePixels)  {numOfEdgePixels  = _numOfEdgePixels;}


    float   CentroidCol        () const  {return  centroidCol;}    // Centroid with respect to image
    float   CentroidRow        () const  {return  centroidRow;}    //  ""    ""    ""    ""    ""
    kkint32 NumOfEdgePixels    () const  {return  numOfEdgePixels;}

    void    CalcFeatures (Raster&        raster,
                          RasterListPtr  intermediateImages
                         );


    static  void  ParseImageFileName (const KKStr&  fullFileName, 
                                      KKStr&        scannerFileName,
                                      kkuint32&     scanLineNum,
                                      kkuint32&     scanCol
                                     );


    static  FileDescConstPtr    postLarvaeFeaturesFileDesc;
    
    static  FileDescConstPtr    PostLarvaeFeaturesFileDesc ();

    static  KKStr     FeatureName (kkuint32  fieldNum);
    static  kkuint32  MaxNumOfFeatures () {return maxNumOfFeatures;}



  private:
    static  RunLog  runLog;

  
    void   SaveIntermediateImage (Raster&        raster, 
                                  const KKStr&   desc,
                                  RasterListPtr  intermediateImages
                                 );

    static  RasterListPtr  calcImages;
    static  VectorKKStr*   calcImagesDescs;

    float     centroidCol;     /**<  centroid Column with just respect to image. */
    float     centroidRow;     /**<  centroid Row with just respect to image. */
    kkint32   numOfEdgePixels;

    static  kkuint32  maxNumOfFeatures;
    static  const     kkint32  SizeThreshold;

    static  const    char*  FeatureNames[];


    static  kkint16  SizeIndex;                   // 0;
    static  kkint16  Moment1Index;                // 1;
    static  kkint16  Moment2Index;                // 2;
    static  kkint16  Moment3Index;                // 3;
    static  kkint16  Moment4Index;                // 4;
    static  kkint16  Moment5Index;                // 5;
    static  kkint16  Moment6Index;                // 6;
    static  kkint16  Moment7Index;                // 7;
    static  kkint16  Moment8Index;                // 8;

    static  kkint16  EdgeSizeIndex;               // 9;
    static  kkint16  EdgeMoment1Index;            // 10;
    static  kkint16  EdgeMoment2Index;            // 11;
    static  kkint16  EdgeMoment3Index;            // 12;
    static  kkint16  EdgeMoment4Index;            // 13;
    static  kkint16  EdgeMoment5Index;            // 14;
    static  kkint16  EdgeMoment6Index;            // 15;
    static  kkint16  EdgeMoment7Index;            // 16;
    static  kkint16  EdgeMoment8Index;            // 17;

    static  kkint16  TransparancyConvexHullIndex; // 18;
    static  kkint16  TransparancyPixelCountIndex; // 19;
    static  kkint16  TransparancyOpen3Index;      // 20;
    static  kkint16  TransparancyOpen5Index;      // 21;
    static  kkint16  TransparancyOpen7Index;      // 22;
    static  kkint16  TransparancyOpen9Index;      // 23;
    static  kkint16  TransparancyClose3Index;     // 24;
    static  kkint16  TransparancyClose5Index;     // 25;
    static  kkint16  TransparancyClose7Index;     // 26;

    static  kkint16  ConvexAreaIndex;             // 27
    static  kkint16  TransparancySizeIndex;       // 28
    static  kkint16  TransparancyWtdIndex;        // 29

    static  kkint16  WeighedMoment0Index;         // 30
    static  kkint16  WeighedMoment1Index;         // 31
    static  kkint16  WeighedMoment2Index;         // 32
    static  kkint16  WeighedMoment3Index;         // 33
    static  kkint16  WeighedMoment4Index;         // 34
    static  kkint16  WeighedMoment5Index;         // 35
    static  kkint16  WeighedMoment6Index;         // 36
    static  kkint16  WeighedMoment7Index;         // 37
    static  kkint16  WeighedMoment8Index;         // 38

    static  kkint16  IntensityHist1Index;         // 39
    static  kkint16  IntensityHist2Index;         // 40
    static  kkint16  IntensityHist3Index;         // 41
    static  kkint16  IntensityHist4Index;         // 42
    static  kkint16  IntensityHist5Index;         // 43
    static  kkint16  IntensityHist6Index;         // 44
    static  kkint16  IntensityHist7Index;         // 45
    static  kkint16  DarkSpotCount0;              // 46
    static  kkint16  DarkSpotCount1;              // 47
    static  kkint16  DarkSpotCount2;              // 48
    static  kkint16  DarkSpotCount3;              // 49
    static  kkint16  DarkSpotCount4;              // 50
    static  kkint16  DarkSpotCount5;              // 51
    static  kkint16  DarkSpotCount6;              // 52
    static  kkint16  DarkSpotCount7;              // 53
    static  kkint16  DarkSpotCount8;              // 54
    static  kkint16  DarkSpotCount9;              // 55
  };


  typedef  PostLarvaeFV::PostLarvaeFVPtr  PostLarvaeFVPtr;

#define  _PostLarvaeFV_Defined_



  class  PostLarvaeFVList:  public FeatureVectorList
  {
  public: 
    typedef  PostLarvaeFVList*  PostLarvaeFVListPtr;

    PostLarvaeFVList (FileDescConstPtr  _fileDesc,
                      bool              _owner
                     );

  private:
    /**
     *@brief  Will create a duplicate List of examples, in the same order.  If the source 
     *        'examples' owns its entries, then new duplicate entries will be created, and will 
     *        own them otherwise will only get pointers to existing instances in 'examples'.
     */
    PostLarvaeFVList (const PostLarvaeFVList&  examples);


  public:

    /**
     *@brief Creates a duplicate List of examples, in the same order, and depending on '_owner' will 
     *       create new instances or just point to the existing one.
     *@details 
     *@code 
     *  If '_owner' = true 
     *     Create new instances of contents and own them.  
     *  else if  'owner' = false, 
     *     Copy over pointers to existing instances.  
     *@endcode
     *@param[in]  _examples   The list of 'PostLarvaeFV' that is to be copied.
     *@param[in]  _owner      If 'true' new instances of 'PostLarvaeFV' instances will be created
     *                        and this new list will own them and if 'false' will just point to
     *                        the existing instances and not own the.
     */
    PostLarvaeFVList (const PostLarvaeFVList&  _examples,
                      bool                     _owner
                     );


    /**
     *@brief Creates a duplicate List of examples, in the same order, and depending on '_owner' will 
     *       create new instances or just point to the existing one.
     *@details 
     *@code 
     *  If '_owner' = true 
     *     Create new instances of contents and own them.  
     *  else if  'owner' = false, 
     *     Copy over pointers to existing instances.  
     *@endcode
     * If any of the existing instances do not have an underlying class of PostLarvaeFV;  
     * the function will throw an exception.
     *
     *@param[in]  _examples   The list of 'PostLarvaeFV' that is to be copied.
     *@param[in]  _owner      If 'true' new instances of 'PostLarvaeFV' instances will be created
     *                        and this new list will own them and if 'false' will just point to
     *                        the existing instances and not own the.
     */
    PostLarvaeFVList (const FeatureVectorList&  featureVectorList,
                      bool                      _owner
                     );


    /**
     *@brief  Constructor that will extract a list of feature vectors for all the image files in the 
     *        specified directory.
     *@details
     * Will scan the directory _dirName for any image files. For each image found a new instance of PostLarvaeFV
     * will be created who's features will be derived from the image. These PostLarvaeFV' objects will be 
     * assigned the class specified by '_mlClass'.  A new data file containing the extracted features will be 
     * saved in fileName.
     *
     *@param _log[in]      Log file to write messages to.
     *@param _mlClass[in]  Class to assign to new 'PostLarvaeFV' objects.
     *@param _dirName[in]  Directory to scan for examples.
     *@param _fileName     Name of file to contain the extracted feature data.
     */
    PostLarvaeFVList (MLClassPtr  _mlClass,
                      KKStr       _dirName,
                      KKStr       _fileName,
                      RunLog&     _log
                     );



    /**
     *@brief  constructor that will create a list of examples from _examples that are assigned one of the 
     *        classes listed in _mlClasses.
     *@details
     *   Will Create a list of examples that are a subset of the ones in _examples.  The subset will
     *   consist of the examples who's mlClass is one of the  ones in mlClasses.  We will not own
     *   any the contents only point to the ones already in _examples.
     *@param[in] _mlClasses  List of classes that we are interested in.
     *@param[in] _examples        Source examples that we want to scan.
     *@param[in] _log           
     */
    PostLarvaeFVList (MLClassList&       _mlClasses,
                      PostLarvaeFVList&  _examples
                     );


    /**
     @brief
     @details
        This constructor is meant to create a list of 'PostLarvaeFV' objects from the FeatureVector
        objects contained in featureVectorList.
     @code
     If  'featureVectorList'  owns its contents (that is 'featureVectorList.Owner () == true'  then
        |  We will create new Instances of 'PostLarvaeFV' objects that we will own.
        |  The underlying class of the 'FeatureVector' objects will be converted to a
        |  'PostLarvaeFV'  class.
     else
        |  all the 'FeatureVector' objects in 'featureVectorList' must have an underlying class of
        |  'PostLarvaeFV'.  If one or more do not then the program will halt with a message to
        |  the log.
    @endcode
    */
    PostLarvaeFVList (const FeatureVectorList&  featureVectorList);




    virtual  ~PostLarvaeFVList ();


    void                   AddSingleImageFeatures (PostLarvaeFVPtr  _imageFeatures);  // Same as PushOnBack

    void                   AddQueue (PostLarvaeFVList&  imagesToAdd);

    PostLarvaeFVPtr        BackOfQueue ();

    PostLarvaeFVPtr        BinarySearchByName (const KKStr&  _imageFileName)  const;

    VectorFloat            CalculateDensitesByQuadrat (float        scanRate,         // Scan Lines per Sec.
                                                       float        quadratSize,      // Meters.
                                                       float        defaultFlowRate,  // Meters per Sec
                                                       const bool&  cancelFlag,
                                                       RunLog&      log
                                                      );

    /***@brief  Creates a duplicate of list using the same container. */
    virtual
    PostLarvaeFVListPtr    Duplicate (bool _owner)  const;

    PostLarvaeFVListPtr    DuplicateListAndContents ()  const;


    /**
     *@brief Returns: a list of 'PostLarvaeFV' objects that have duplicate root file names.
     *@details
     *@code
     ***************************************************************************************************
     ** Returns: a list of 'PostLarvaeFV' objects that have duplicate root file names.  The returned   *
     ** list will not own these items.  All instances of the duplicate objects will be returned.       *
     ** Ex:  if three instances have the same ExampleFileName all three will be returned.                * 
     ***************************************************************************************************
     *@endcode
     */
    PostLarvaeFVListPtr   ExtractDuplicatesByRootImageFileName ();

    PostLarvaeFVListPtr   ExtractExamplesForAGivenClass (MLClassPtr  _mlClass,
                                                         kkint32     _maxToExtract,
                                                         float       _minSize
                                                        )  const;


    void                  FeatureExtraction (KKStr       _dirName, 
                                             KKStr       _fileName, 
                                             MLClassPtr  _mlClass,
                                             RunLog&     _log
                                            );

    PostLarvaeFVPtr       IdxToPtr (kkint32 idx) const;

    PostLarvaeFVPtr       LookUpByImageFileName (const KKStr&  _imageFileName)  const;

    PostLarvaeFVPtr       LookUpByRootName (const KKStr&  _rootName);

    /**@brief Creates an instance of a Empty FeatureVectorList. */
    virtual
    PostLarvaeFVListPtr   ManufactureEmptyList (bool _owner)  const;

    
    /**
     *@brief  Using list of ImageFileNames in a file('fileName') create a new PostLarvaeFVList instance 
     * with examples in order based off contents of file. If error occurs will return NULL.
     */
    PostLarvaeFVListPtr    OrderUsingNamesFromAFile (const KKStr&  namesFileName,
                                                     RunLog&       log
                                                    );

    PostLarvaeFVPtr        PopFromBack ();

    void                   RecalcFeatureValuesFromImagesInDirTree (KKStr    rootDir,
                                                                   bool&    successful,
                                                                   RunLog&  log
                                                                  );

    PostLarvaeFVListPtr    StratifyAmoungstClasses (kkint32  numOfFolds,
                                                    RunLog&  log
                                                   );


    PostLarvaeFVListPtr    StratifyAmoungstClasses (MLClassListPtr  mlClasses,
                                                    kkint32         maxImagesPerClass,
                                                    kkint32         numOfFolds,
                                                    RunLog&         log
                                                   );



    //virtual  const char*   UnderlyingClass ()  const  {return  "PostLarvaeFVList";}


    class  const_iterator
    {
    private:
      FeatureVectorList::const_iterator  idx;
    
    public:
      const_iterator ():
          idx ()
      {
      }


      const_iterator (const const_iterator&  ivConst_Iterator):
          idx (ivConst_Iterator.idx)
      {
      }


      const_iterator (const FeatureVectorList::const_iterator&  fvConst_Iterator):
          idx (fvConst_Iterator)
      {
      }


      const_iterator (const FeatureVectorList::iterator&  fvIterator):
          idx (fvIterator)
      {
      }


      const PostLarvaeFVPtr  operator*()
      {
        return  (const PostLarvaeFVPtr)*idx;
      }


      const_iterator&   operator= (const const_iterator&  right)
      {
        idx = right.idx;
        return  *this;
      }



      const_iterator&   operator= (const FeatureVectorList::iterator&  right)  
      {
        idx = right;
        return *this;
      }


      const_iterator&   operator= (const FeatureVectorList::const_iterator&  right)  
      {
        idx = right;
        return *this;
      }

      
      bool  operator!= (const const_iterator&  right)  const
      {
        return  idx != right.idx;
      }


      bool  operator!= (const FeatureVectorList::iterator&  right)  const
      {
        return  idx != (FeatureVectorList::const_iterator)right;
      }


      bool  operator!= (const FeatureVectorList::const_iterator&  right)  const
      {
        return  idx != right;
      }


      bool  operator== (const const_iterator&  right)  const
      {
        return  idx == right.idx;
      }


      bool  operator== (const FeatureVectorList::iterator&  right)  const
      {
        return  (idx == right);
      }


      const_iterator&   operator++ (int)
      {
        idx++;
        return  *this;
      }
    };  /* const_iterator */





    class  iterator
    {
    private:
      FeatureVectorList::iterator  idx;
    
    public:
      iterator ():
          idx ()
      {
      }

      iterator (const iterator&  idx):
          idx (idx.idx)
      {
      }


      iterator (const FeatureVectorList::iterator&  idx):
          idx (idx)
      {
      }


      PostLarvaeFVPtr  operator*()
      {
        return  (PostLarvaeFVPtr)*idx;
      }

      iterator&   operator= (const iterator&  right)
      {
        idx = right.idx;
        return  *this;
      }

      bool  operator!= (const iterator&  right)  const
      {
        return  idx != right.idx;
      }

      bool  operator== (const iterator&  right)  const
      {
        return  idx == right.idx;
      }

      iterator&   operator++ (int)
      {
        idx++;
        return  *this;
      }
    };


    
    //iterator  begin ()  {return  KKQueue<FeatureVector>::begin ();}


  private:
  };  /* PostLarvaeFVList */


  typedef  PostLarvaeFVList::PostLarvaeFVListPtr  PostLarvaeFVListPtr;

#define  _PostLarvaeFVList_Defined_


}  /* namespace CounterUnManaged */

#endif

