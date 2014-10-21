#ifndef  _DATABASEIMAGE_
#define  _DATABASEIMAGE_

/**
 @class  LarcosDatabase::DataBaseImage
 @details 
 @code
 //***********************************************************************************************
 //*   Used for returning results of Queries against the Images table in the pices database (See *
 //*   DataBase::ImageQuery).                                                                    *
 //***********************************************************************************************
 @endcode
 */

#ifndef  _mysql_h
struct  MYSQL;
struct  MYSQL_RES;
struct  MSQL_STRUCT; 
typedef  MSQL_STRUCT* MYSQL_ROW;
#endif

#include  "OSservices.h"
#include  "Raster.h"
#include  "RunLog.h"
using namespace KKB;

#include  "ImageClass.h"
using namespace  KKMachineLearning;

namespace LarcosDatabase
{
  class  DataBaseImage
  {
  public:
    typedef  KKB::uint  uint;

    typedef  DataBaseImage*  DataBaseImagePtr;

    typedef  KKB::uint   uint;
    typedef  KKB::ulong  ulong;
    typedef  KKB::uchar  uchar;

    DataBaseImage ();
    ~DataBaseImage ();

    float           BreakTie            () const;
    osFilePos       ByteOffset          () const  {return  byteOffset;}
    uint            CentroidCol         () const  {return  centroidCol;}
    uint            CentroidRow         () const  {return  centroidRow;}
    ImageClassPtr   Class1              () const  {return  class1;}
    const KKStr&    Class1Name          () const;
    float           Class1Prob          () const  {return  class1Prob;}
    ImageClassPtr   Class2              () const  {return  class2;}
    const KKStr&    Class2Name          () const;
    float           Class2Prob          () const  {return  class2Prob;}
    float           Depth               () const  {return  depth;}
    const uchar*    EncodedThumbnail    () const  {return  encodedThumbnail;}
    uint            EncodedThumbnailLen () const  {return  encodedThumbnailLen;}
    uint            Height              () const  {return  height;}
    KKStr           ImageFileName       () const;
    int             ImageId             () const  {return  imageId;}
    uint            PixelCount          () const  {return  pixelCount;}
    int             ScannerFileId       () const  {return  scannerFileId;}
    const KKStr&    ScannerFileName     () const  {return  scannerFileName;}
    KKB::RasterPtr  ThumbNail           () const;
    uint            TopLeftRow          () const  {return  topLeftRow;}
    uint            TopLeftCol          () const  {return  topLeftCol;}
    ImageClassPtr   ValidatedClass      () const  {return  validatedClass;}
    const KKStr&    ValidatedClassName  () const;
    uint            Width               () const  {return  width;}



    void    ByteOffset          (osFilePos       _byteOffset)          {byteOffset          = _byteOffset;}
    void    CentroidCol         (uint            _centroidCol)         {centroidCol         = _centroidCol;}
    void    CentroidRow         (uint            _centroidRow)         {centroidRow         = _centroidRow;}
    void    Class1              (ImageClassPtr   _class1)              {class1              = _class1;}
    void    Class1Name          (const char*     _class1Name);
    void    Class1Prob          (float           _class1Prob)          {class1Prob          = _class1Prob;}
    void    Class2              (ImageClassPtr   _class2)              {class2              = _class2;}
    void    Class2Name          (const char*     _class2Name);
    void    Class2Prob          (float           _class2Prob)          {class2Prob          = _class2Prob;}
    void    Depth               (float           _depth)               {depth               = _depth;}
    void    EncodedThumbnail    (uchar*          _encodedThumbnail)    {encodedThumbnail    = _encodedThumbnail;}
    void    EncodedThumbnailLen (uint            _encodedThumbnailLen) {encodedThumbnailLen = _encodedThumbnailLen;}
    void    Height              (uint            _height)              {height              = _height;}
    void    ImageId             (int             _imageId)             {imageId             = _imageId;}
    void    ScannerFileId       (int             _scannerFileId)        {scannerFileId        = _scannerFileId;}
    void    ScannerFileName     (const KKStr&    _scannerFileName)      {scannerFileName      = _scannerFileName;}  
    void    TopLeftRow          (uint            _topLeftRow)          {topLeftRow          = _topLeftRow;}
    void    TopLeftCol          (uint            _topLeftCol)          {topLeftCol          = _topLeftCol;}
    void    PixelCount          (uint            _pixelCount)          {pixelCount          = _pixelCount;}
    void    ValidatedClass      (ImageClassPtr   _validatedClass)      {validatedClass      = _validatedClass;}
    void    ValidatedClassName  (const char*     _validatedClassName);
    void    Width               (uint            _width)               {width               = _width;}


  private:
    osFilePos       byteOffset;
    uint            centroidRow;
    uint            centroidCol;
    ImageClassPtr   class1;
    float           class1Prob;
    ImageClassPtr   class2;
    float           class2Prob;
    float           depth;
    uchar*          encodedThumbnail; 
    uint            encodedThumbnailLen; 
    uint            height;
    int             imageId;
    uint            pixelCount;
    int             scannerFileId;
    KKStr           scannerFileName;
    uint            topLeftRow;
    uint            topLeftCol;
    ImageClassPtr   validatedClass;
    uint            width;
  };  /* DataBaseImage */



  typedef  DataBaseImage::DataBaseImagePtr  DataBaseImagePtr;



  class  DataBaseImageList: public  KKQueue<DataBaseImage>
  {
  public:
    typedef  DataBaseImageList*  DataBaseImageListPtr;

    DataBaseImageList (bool  _owner = true);
    ~DataBaseImageList ();

    ImageClassListPtr  ExtractListOfClasses ();

    DataBaseImageListPtr  ExtractImagesForAGivenClass (ImageClassPtr  _imageClass);


  private:
  };  /* DataBaseImageList */



  typedef  DataBaseImageList::DataBaseImageListPtr  DataBaseImageListPtr;


}  /* namespace LarcosDatabase */


#endif
