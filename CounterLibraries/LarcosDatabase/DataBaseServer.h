#if  !defined(_DATABASESERVER_)
#define  _DATABASESERVER_


/**
 @class LarcosDatabase::DataBaseServer
 @brief  Represents the parameters for a single Database Server.  
 @details  Things like username, hostname, etc.   The "DataBase"  class will use one instance
           of this object to use as connection parameters.
*/


#include  "KKQueue.h"
#include  "RunLog.h"
#include  "Str.h"
using namespace KKB;

namespace LarcosDatabase 
{
  class  DataBaseServer
  {
  public:
    typedef  DataBaseServer*    DataBaseServerPtr;

    DataBaseServer ();
    DataBaseServer (const DataBaseServer&  dataBaseServer);
    DataBaseServer (const KKStr&           parameterStr);

    ~DataBaseServer ();

    const KKStr&  Description  ()   {return  description;}
    const KKStr&  HostName     ()   {return  hostName;}
    const KKStr&  UserName     ()   {return  userName;}
    const KKStr&  PassWord     ()   {return  passWord;}
    const KKStr&  DataBaseName ()   {return  dataBaseName;}

    void  Description  (const KKStr&  _description)   { description  = _description;}
    void  HostName     (const KKStr&  _hostName)      { hostName     = _hostName;}
    void  UserName     (const KKStr&  _userName)      { userName     = _userName;}
    void  PassWord     (const KKStr&  _passWord)      { passWord     = _passWord;}
    void  DataBaseName (const KKStr&  _dataBaseName)  { dataBaseName = _dataBaseName;}

    KKStr  ServerDescription ()  const;  // Get description of server for info display

    KKStr  ToParameterStr ()  const;

  private:
    void  ParseParameterStr (const  KKStr&  parameterStr);

    KKStr     description;
    KKStr     hostName;
    KKStr     userName;
    KKStr     passWord;
    KKStr     dataBaseName;
  };  /* DataBaseServer */


  typedef  DataBaseServer::DataBaseServerPtr  DataBaseServerPtr;



  class  DataBaseServerList: public  KKQueue<DataBaseServer>
  {
  public:
    typedef  DataBaseServerList*  DataBaseServerListPtr;

    DataBaseServerList (RunLog&  _log,                         // Will load lost from "MySql.cfg" file in "%PicesHomeDir%\Configurations" directory
                        bool     _loadFromConfigFile = true    // if set to false will create an empty list.
                       );

    DataBaseServerList (const DataBaseServerList&  serverList);

   ~DataBaseServerList ();


    const KKStr&        DefaultServerDescription ()  const  {return defaultServerDescription;}
    void                DefaultServerDescription (const KKStr&   _defaultServerDescription)  {defaultServerDescription = _defaultServerDescription;}


    DataBaseServerPtr   LookUpByDescription (const KKStr&  _description);

    DataBaseServerPtr   GetDefaultServer ();   // Will return a pointer to the DataBaseServer instance that
                                               // has the same decription as "defaultServerDescription".
  


    RunLog&   Log ()   {return  log;}

    void  WriteConfigFile ()  const;

  private:
    void  ReadConfigFile ();

    KKStr  defaultServerDescription;

    RunLog&  log;
  };


  typedef  DataBaseServerList::DataBaseServerListPtr  DataBaseServerListPtr;

}  /* namespace LarcosDatabase */

#endif
