#if  !defined(_DATABASEIMAGEGROUPENTRY_)
#define  _DATABASEIMAGEGROUPENTRY_


#include  "Str.h"
#include  "KKQueue.h"



namespace LarcosDatabase {


#if  !defined(_DATABASEIMAGE_)
  class  DataBaseImage;
  typedef  DataBaseImage*  DataBaseImagePtr;
  class  DataBaseImageList;
  typedef  DataBaseImageList*  DataBaseImageListPtr;
#endif



class DataBaseImageGroupEntry
{
public:
  typedef  DataBaseImageGroupEntry*  DataBaseImageGroupEntryPtr;

  DataBaseImageGroupEntry (const DataBaseImageGroupEntry&  imageGroupEntry);

  DataBaseImageGroupEntry (int           _imageGroupId,
                           const KKStr&  _imageFileName
                          );

  ~DataBaseImageGroupEntry ();


  int           ImageGroupId  ()  const  {return  imageGroupId;}
  const KKStr&  ImageFileName ()  const  {return  imageFileName;}

  void    ImageGroupId  (int           _imageGroupId)   {imageGroupId  = _imageGroupId;}
  void    ImageFileName (const KKStr&  _imageFileName)  {imageFileName = _imageFileName;}

private:
  int    imageGroupId;
  KKStr  imageFileName;
};  /* DataBaseImageGroupEntry */

typedef  DataBaseImageGroupEntry::DataBaseImageGroupEntryPtr  DataBaseImageGroupEntryPtr;



class  DataBaseImageGroupEntryList:  public  KKQueue<DataBaseImageGroupEntry>
{
public:
  typedef  DataBaseImageGroupEntryList*  DataBaseImageGroupEntryListPtr;

  DataBaseImageGroupEntryList (bool  _owner);

  DataBaseImageGroupEntryList (const DataBaseImageGroupEntryList&  imageGroupEntries);

  DataBaseImageGroupEntryList (int                         groupId,
                               const DataBaseImageListPtr  images
                              );

  ~DataBaseImageGroupEntryList ();
};

typedef  DataBaseImageGroupEntryList::DataBaseImageGroupEntryListPtr  DataBaseImageGroupEntryListPtr;

}  /* namespace LarcosDatabase { */


#endif
