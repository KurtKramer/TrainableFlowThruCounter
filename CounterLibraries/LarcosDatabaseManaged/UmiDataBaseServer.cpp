#include  "StdAfx.h"
#include  "FirstIncludes.h"

#include  <stdio.h>
#include  <math.h>
#include  <ctype.h>
#include  <time.h>
#include  <fstream>
#include  <iostream>
#include  <map>
#include  <ostream>
#include  <string>
#include  <vector>
using namespace std;

#include  "MemoryDebug.h"
#include  "KKBaseTypes.h"
#include  "..\KKBaseLibrery\GoalKeeper.h"


using namespace System;
using namespace System::Collections::Generic;
using namespace System::IO;
using namespace System::Windows::Forms;
using namespace KKB;

#include  "UmiKKStr.h"
#include  "UmiScannerFileEntry.h"

#include  "UmiDataBaseServer.h"

using namespace  LarcosDatabaseManaged;



UmiDataBaseServer::UmiDataBaseServer ():
  server (NULL)
{
  server = new DataBaseServer ();
}




UmiDataBaseServer::UmiDataBaseServer (UmiDataBaseServer^  umiServer):
  server (NULL)
{
  server = new DataBaseServer (*(umiServer->server));
}



UmiDataBaseServer::UmiDataBaseServer (const DataBaseServerPtr  _server):
  server (NULL)
{
  server = new DataBaseServer (*_server);
}




UmiDataBaseServer::~UmiDataBaseServer ()
{
  this->!UmiDataBaseServer ();
}


UmiDataBaseServer::!UmiDataBaseServer ()
{
  delete  server;
  server = NULL;
}



String^  UmiDataBaseServer::Description::get () 
{
  if  (!server)
    return  String::Empty;

  return  UmiKKStr::KKStrToSystenStr (server->Description ());
}


void  UmiDataBaseServer::Description::set (String^  _description)
{
  if  (!server)
    server = new DataBaseServer ();
  server->Description (UmiKKStr::SystemStringToKKStr (_description));
}





String^  UmiDataBaseServer::HostName::get () 
{
  if  (!server)
    return  String::Empty;

  return  UmiKKStr::KKStrToSystenStr (server->HostName ());
}


void  UmiDataBaseServer::HostName::set (String^ _hostName)
{
  if  (!server)
    server = new DataBaseServer ();
  server->HostName (UmiKKStr::SystemStringToKKStr (_hostName));
}



String^  UmiDataBaseServer::UserName::get () 
{
  if  (!server)
    return  String::Empty;

  return  UmiKKStr::KKStrToSystenStr (server->UserName ());
}


void  UmiDataBaseServer::UserName::set (String^  _userName)
{
  if  (!server)
    server = new DataBaseServer ();
  server->UserName (UmiKKStr::SystemStringToKKStr (_userName));
}




String^  UmiDataBaseServer::PassWord::get () 
{
  if  (!server)
    return  String::Empty;

  return  UmiKKStr::KKStrToSystenStr (server->PassWord ());
}


void  UmiDataBaseServer::PassWord::set (String^  _passWord)
{
  if  (!server)
    server = new DataBaseServer ();
  server->PassWord (UmiKKStr::SystemStringToKKStr (_passWord));
}



String^  UmiDataBaseServer::DataBaseName::get () 
{
  if  (!server)
    return  String::Empty;

  return  UmiKKStr::KKStrToSystenStr (server->DataBaseName ());
}




void  UmiDataBaseServer::DataBaseName::set (String^  _dataBaseName)
{
  if  (!server)
    server = new DataBaseServer ();
  server->DataBaseName (UmiKKStr::SystemStringToKKStr (_dataBaseName));
}




void  UmiDataBaseServer::CleanUpMemory ()
{
}




UmiDataBaseServerList::UmiDataBaseServerList (UmiRunLog^  _log)
{
  log = _log;
  DataBaseServerListPtr  servers = new DataBaseServerList (log->Log ());

  DataBaseServerList::const_iterator  idx;

  for  (idx = servers->begin ();  idx != servers->end ();  idx++)
  {
    DataBaseServerPtr  s = *idx;
    Add (gcnew UmiDataBaseServer (s));
  }

  defaultServerDescription = UmiKKStr::KKStrToSystenStr (servers->DefaultServerDescription ());

  // Since 'UmiDataBaseServer' takes ownership of the 'DataBaseServer' object being passed in
  // we don't want the 'servers' list to own them;  this way when we delete 'servers' we do
  // not delete the instances of 'DataBaseServer' that it references.
  servers->Owner (false);
  delete  servers;
  servers = NULL;
}





UmiDataBaseServerList::UmiDataBaseServerList (const DataBaseServerListPtr  _servers,
                                                  UmiRunLog^                 _log
                                                 )
{
  log = _log;

  DataBaseServerList::const_iterator  idx;

  for  (idx = _servers->begin ();  idx != _servers->end ();  idx++)
  {
    DataBaseServerPtr  s = *idx;
    Add (gcnew UmiDataBaseServer (new DataBaseServer (*s)));
  }

  defaultServerDescription = UmiKKStr::KKStrToSystenStr (_servers->DefaultServerDescription ());
}



UmiDataBaseServerList::~UmiDataBaseServerList ()
{
  CleanUpMemory ();
}



void  UmiDataBaseServerList::CleanUpMemory ()
{
}  /* CleanUpMemory */




UmiDataBaseServer^   UmiDataBaseServerList::GetDefaultServer ()
{
  UmiDataBaseServer^  defaultServer = LookUpByDescription (defaultServerDescription);
  if  (defaultServer == nullptr) 
  {
    defaultServer = LookUpByDescription ("Default");
    if  (defaultServer == nullptr)
    {
      if  (Count > 0)
        defaultServer = (*this)[0];
    }
  }
  return  defaultServer;
}  /* GetDefaultServer*/




UmiDataBaseServer^  UmiDataBaseServerList::LookUpByDescription (String^ _description)
{
  UmiDataBaseServer^ s = nullptr;

  for each  (UmiDataBaseServer^  server in (*this))
  {
    if  (String::Compare (_description, server->Description, true) == 0)
    {
      s = server;
      break;
    }
  }

  return  s;
}  /* LookUpByDescription */




void  UmiDataBaseServerList::WriteConfigFile ()
{
  DataBaseServerListPtr  servers = new DataBaseServerList (log->Log (), 
                                                           false        // false = Do not load from config file
                                                          );


  for each  (UmiDataBaseServer^  pdbs  in  (*this))
  {
    DataBaseServerPtr dbs = new DataBaseServer (*(pdbs->UnManagedClass ()));
    servers->PushOnBack (dbs);
  }

  servers->DefaultServerDescription (UmiKKStr::SystemStringToKKStr (defaultServerDescription));
  servers->WriteConfigFile ();
  delete  servers;
  servers = NULL;
}  /* WriteConfigFile */
