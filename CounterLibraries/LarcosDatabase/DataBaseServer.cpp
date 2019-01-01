#include  "FirstIncludes.h"

#include  <ctype.h>
#include  <time.h>

#include  <fstream>
#include  <iostream>
#include  <map>
#include  <string>
#include  <vector>

#include  "MemoryDebug.h"
#include  "KKBaseTypes.h"


using namespace std;

#include "OSservices.h"
using namespace KKB;

#include "Variables.h"
using namespace  KKLSC;


#include "DataBaseServer.h"
using  namespace  LarcosDatabase;


DataBaseServer::DataBaseServer ():
  description   ("Default"),
  hostName      ("localhost"),
  userName      ("root"),
  passWord      ("dasani20"),
  dataBaseName  ("larcos")
{
}



DataBaseServer::DataBaseServer (const DataBaseServer&  dataBaseServer):
  description   (dataBaseServer.description),
  hostName      (dataBaseServer.hostName),
  userName      (dataBaseServer.userName),
  passWord      (dataBaseServer.passWord),
  dataBaseName  (dataBaseServer.dataBaseName)
{
}



DataBaseServer::DataBaseServer (const KKStr&  parameterStr):
  description   ("Default"),
  hostName      ("localhost"),
  userName      ("root"),
  passWord      ("dasani20"),
  dataBaseName  ("pices")
{
  ParseParameterStr (parameterStr);
}

  

DataBaseServer::~DataBaseServer ()
{

}



KKStr  DataBaseServer::ServerDescription ()  const  // Get description of server for info display
{
  return  description + "  Host[" + hostName + "]  User[" + userName + "]  Database[" + dataBaseName + "]";
}



void  DataBaseServer::ParseParameterStr (const  KKStr&  parameterStr)
{
  VectorKKStr  parameterPairs = parameterStr.Split ("\t");

  VectorKKStr::iterator  idx;
  for  (idx = parameterPairs.begin ();  idx != parameterPairs.end ();  idx++)
  {
    VectorKKStr  fields = (*idx).Split (":=");   // Split by either ':'  or  '='
    if  (fields.size () < 2)
    {
      // Should be two fields;  line must be malformed.
      continue;
    }

    KKStr  parameterName = fields[0].ToUpper ();

    if      ((parameterName == "DESCRIPTION")   ||  (parameterName == "DESC")      ||  (parameterName == "D"))   description  = fields[1];
    else if ((parameterName == "HOSTNAME")      ||  (parameterName == "HOST")      ||  (parameterName == "H"))   hostName     = fields[1];
    else if ((parameterName == "USERNAME")      ||  (parameterName == "USER")      ||  (parameterName == "U"))   userName     = fields[1];
    else if ((parameterName == "PASSWORD")      ||  (parameterName == "PW")        ||  (parameterName == "P"))   passWord     = fields[1];
    else if ((parameterName == "DATABASENAME")  ||  (parameterName == "DATABASE")  ||  (parameterName == "DB"))  dataBaseName = fields[1];
  }
}  /* ParseParameterStr */





KKStr  DataBaseServer::ToParameterStr ()  const
{
  KKStr   parameterStr (1024);

  parameterStr = "Description:"   + description   + "\t"  +
                 "HostName:"      + hostName      + "\t"  +
                 "UserName:"      + userName      + "\t"  +
                 "PassWord:"      + passWord      + "\t"  +
                 "DataBaseName:"  + dataBaseName;

  return  parameterStr;

}  /* ToParameterStr */





DataBaseServerList::DataBaseServerList (RunLog&  _log,
                                        bool     _loadFromConfigFile
                                       ):  
  KKQueue<DataBaseServer> (true, 10),   // true = We will own contents,  10 = Initial capacity set to 10
  defaultServerDescription ("Default"),
  log                      (_log)
{
  if  (_loadFromConfigFile)
    ReadConfigFile ();
}



DataBaseServerList::DataBaseServerList (const DataBaseServerList&  serverList):
    KKQueue<DataBaseServer> (true, QueueSize ()),
    defaultServerDescription  (serverList.defaultServerDescription),
    log                       (serverList.log)
{
  DataBaseServerList::const_iterator  idx;
  for  (idx = begin ();  idx != end ();  idx++)
  {
    DataBaseServerPtr  dbs = *idx;
    PushOnBack (new DataBaseServer (*dbs));
  }
}


DataBaseServerList::~DataBaseServerList ()
{
}






void  DataBaseServerList::WriteConfigFile ()  const
{
  osCreateDirectoryPath (Variables::ScsConfigurationDirectory ());
  KKStr  configFileName = osAddSlash (Variables::ScsConfigurationDirectory ()) + "MySql.cfg";
 
  log.Level (10) << "DataBaseServerList::WriteConfigFile    Writing ConfigFileName[" << configFileName << "]" << endl;
  
  ofstream  o (configFileName.Str ());

  if  (!o.is_open ())
  {
    log.Level (-1) << endl << endl
                   << "DataBaseServerList::WriteConfigFile     ***ERROR***   Opening ConfigFile["  <<  configFileName << "]" << endl
                   << endl;
    return;
  }

  o << "//  PICES  MySQL  configuration file.  There will be one entry for each known MySQL dtabase server." << std::endl
    << "//"  << std::endl
    << "//   Date Writen: " << osGetLocalDateTime () << std::endl
    << "//"  << std::endl
    << "//  Num Entries: " << QueueSize () << std::endl
    << "//" << std::endl;

  DataBaseServerList::const_iterator  idx;
  for  (idx = begin ();  idx != end ();  idx++)
    o << "Server" << "\t" << (*idx)->ToParameterStr () << endl;

  o << "//"  << std::endl
    << "DefaultServer" << "\t" << defaultServerDescription << std::endl;

  o.close ();
}  /* WriteConfigFile */




void  DataBaseServerList::ReadConfigFile ()
{
  KKStr  configFileName = osAddSlash (Variables::ScsConfigurationDirectory ()) + "MySql.cfg";

  log.Level (10) << "DataBaseServerList::ReadConfigFile      ConfigFileName[" <<  configFileName << "]" << endl;
  FILE* in = osFOPEN (configFileName.Str (), "r");
  if  (!in)
  {
    log.Level (-1) << "DataBaseServerList::ReadConfigFile    Error loading ConfigFileName[" <<  configFileName << "]" << endl;
    return;
  }

  DataBaseServerPtr  defaultServer = new DataBaseServer ();
  bool  defaultServerFound = false;

  char  buff[1024];
  while  (fgets (buff, sizeof (buff), in))
  {
    buff[sizeof(buff) - 1] = 0;  // Just to make sure that bufer not comepletely full
    KKStr  ln (buff);

    if  (ln.Len () < 3)
      continue;

    if  ((ln[0] == '/')  &&  (ln[1] == '/'))
      continue;

    KKStr  fieldName  = ln.ExtractToken ("\t\n\r=").ToLower ();
    KKStr  fieldValue = ln.ExtractToken ("\n\r");

    if  ((fieldName.Empty ())  ||  (fieldValue.Empty ()))
      continue;

    if       ((fieldName == "databasename")  ||  (fieldName == "database")  ||  (fieldName == "d"))   defaultServer->DataBaseName (fieldValue);
    else if  ((fieldName == "hostname")      ||  (fieldName == "host")      ||  (fieldName == "h"))   defaultServer->HostName     (fieldValue);
    else if  ((fieldName == "password")      ||  (fieldName == "pass")      ||  (fieldName == "p"))   defaultServer->PassWord     (fieldValue);
    else if  ((fieldName == "username")      ||  (fieldName == "user")      ||  (fieldName == "u"))   defaultServer->UserName     (fieldValue);

    else if  ((fieldName == "defaultserver") ||  (fieldName == "default"))        
      defaultServerDescription = fieldValue;
    
    else if  (fieldName == "server")
    {
      DataBaseServerPtr  server = new  DataBaseServer (fieldValue);

      if  (server->Description ().EqualIgnoreCase ("Default"))
        defaultServerFound = true;

      PushOnBack (server);
    }

    else
    {
      cerr << std::endl << std::endl
           << "DataBase::ReadConfigFile     ***ERROR***       Invalid FieldName[" << fieldName << "]" << std::endl
           << std::endl;
    }
  }

  if  (!defaultServerFound)
  {
    PushOnBack (defaultServer);
  }
  else
  {
    delete  defaultServer;
    defaultServer = NULL;
  }

  fclose (in);
}  /* ReadConfigFile */




DataBaseServerPtr  DataBaseServerList::LookUpByDescription (const KKStr&  _description)
{
  DataBaseServerList::iterator  idx;

  DataBaseServerPtr  dabaseServer = NULL;

  for  (idx = begin ();  idx != end ();  idx++)
  {
     DataBaseServerPtr  dbs = *idx;
     if  (dbs->Description ().EqualIgnoreCase (_description))
     {
       dabaseServer = dbs;
       break;
     }
  }

  return  dabaseServer;
}  /* LookUpByDescription */




DataBaseServerPtr   DataBaseServerList::GetDefaultServer ()
{
  DataBaseServerPtr  defaultServer = LookUpByDescription (defaultServerDescription);
  if  (!defaultServer)
  {
    defaultServer = LookUpByDescription ("Default");
    if  (!defaultServer)
    {
      if  (QueueSize () > 0)
        defaultServer = IdxToPtr (0);   // Just return the first one in the list.

    }
  }
  return  defaultServer;
}  /* GetDefaultServer */



