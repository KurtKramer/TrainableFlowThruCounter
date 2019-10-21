#if  !defined(_DATABASEIMAGEGROUP_)
#define  _DATABASEIMAGEGROUP_

#include  "Str.h"
#include  "KKQueue.h"



namespace  LarcosDatabase
{

class  DataBaseImageGroup
{
public:
  typedef  DataBaseImageGroup*  DataBaseImageGroupPtr;
  typedef  KKB::uint   uint;

  DataBaseImageGroup (int           _imageGroupId,
                      const KKStr&  _name,
                      const KKStr&  _description,
                      uint          _count
                     );

  DataBaseImageGroup (const DataBaseImageGroup&  _dataBaseImageGroup);


  ~DataBaseImageGroup ();


  int           ImageGroupId () const  {return  imageGroupId;}
  const KKStr&  Name         () const  {return  name;}
  const KKStr&  Description  () const  {return  description;}
  uint          Count        () const  {return  count;}

  void  ImageGroupId (int           _imageGroupId)  {imageGroupId = _imageGroupId;}
  void  Name         (const KKStr&  _name)          {name         = _name;        }
  void  Description  (const KKStr&  _description)   {description  = _description; }
  void  Count        (uint          _count)         {count        = _count;       }

  static  bool  ValidName (const KKStr&  name);  // Returns back true if 'name' is formatted ok.


  ///  determines if '_name' is a valid ImageGroupName,  will return back description of any errors 
  static  void  ValidName (KKStr&  _name,
                           KKStr&  _errorDesc,
                           bool&   _valid
                          );  // Returns back true if 'name' is formatted ok.

private:
  int     imageGroupId;
  KKStr   name;
  KKStr   description;
  uint    count;        // number of entries in GroupEntries table for this group
                        // will come from count(g.GroupId) during Select statement
};


typedef  DataBaseImageGroup::DataBaseImageGroupPtr  DataBaseImageGroupPtr;



class  DataBaseImageGroupList: public  KKQueue<DataBaseImageGroup>
{
public:
  typedef  DataBaseImageGroupList*  DataBaseImageGroupListPtr;

  DataBaseImageGroupList (bool _owner);
  DataBaseImageGroupList (const  DataBaseImageGroupList&  _dataBaseImageGroupList);

  ~DataBaseImageGroupList ();



};

typedef   DataBaseImageGroupList::DataBaseImageGroupListPtr  DataBaseImageGroupListPtr;


}  /* namespace  LarcosDatabase */


#endif
