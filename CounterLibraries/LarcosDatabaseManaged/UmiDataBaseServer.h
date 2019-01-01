#pragma once

#include  "DateTime.h"
#include  "Str.h"
using namespace  KKB;

#include  "DataBaseServer.h"
using namespace  KKMachineLearning;


#include  "UmiRunLog.h"

//  Job in Life:   This module is the Managed version of "DataBaseServer".


namespace LarcosDatabaseManaged 
{

  public  ref  class UmiDataBaseServer
  {
  public:
    UmiDataBaseServer ();
    
    UmiDataBaseServer (const DataBaseServerPtr  server);  // Will take ownership of 'server' 
    
    UmiDataBaseServer (UmiDataBaseServer^  umiServer);

  protected:
    ~UmiDataBaseServer ();

  private:
    !UmiDataBaseServer ();

  public:
    DataBaseServerPtr   Server ()  {return  server;}

    property  String^  Description  {String^   get ();   void set (String^ _description)  ;}
    property  String^  HostName     {String^   get ();   void set (String^ _hostName)     ;}
    property  String^  UserName     {String^   get ();   void set (String^ _userName)     ;}
    property  String^  PassWord     {String^   get ();   void set (String^ _passWord)     ;}
    property  String^  DataBaseName {String^   get ();   void set (String^ _dataBaseName) ;}

    void  CleanUpMemory ();

    DataBaseServerPtr   UnManagedClass ()   {return  server;}

  private:
    DataBaseServerPtr   server;
  };



  public ref class UmiDataBaseServerList:  List<UmiDataBaseServer^>
  {
  public:
    UmiDataBaseServerList (UmiRunLog^  _log);


    UmiDataBaseServerList (const DataBaseServerListPtr  _servers,
                           UmiRunLog^                 _log
                          );

    ~UmiDataBaseServerList ();

    property  String^  DefaultServerDescription  
                          {
                           String^  get ()  {return  defaultServerDescription;}  
                           void set (String^ _defaultServerDescription)  {defaultServerDescription = _defaultServerDescription;}
                          }

    property  UmiRunLog^  Log  {UmiRunLog^   get () {return  log;};}


    void  CleanUpMemory ();


    UmiDataBaseServer^  GetDefaultServer ();
    UmiDataBaseServer^  LookUpByDescription (String^ _description);

    void  WriteConfigFile ();


  private:
    String^        defaultServerDescription;
    UmiRunLog^   log;
  };
}
