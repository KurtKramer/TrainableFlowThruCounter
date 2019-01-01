//  because <my_global.h> needs to define some Macro's before windows.h  and stdlib.h  we need t include
// befire  "FirstIncludes.h"
#include <my_global.h>
#include <mysql.h>
#include <errmsg.h>

#include  "FirstIncludes.h"

#include  <ctype.h>
#include  <time.h>
#include  <fstream>
#include  <iostream>
#include  <map>
#include  <string>
#include  <vector>

#include "MemoryDebug.h"
using namespace std;


// Base Library
#include "KKBaseTypes.h"
#include "OSservices.h"
#include "Raster.h"
using namespace KKB;

#include "ScannerFileEntry.h"
using  namespace  KKLSC;

#include "FileDesc.h"
#include "FeatureFileIOKK.h"
#include "PostLarvaeFV.h"
using namespace KKMachineLearning;

#include "DataBase.h"
#include "DataBaseServer.h"
using  namespace  LarcosDatabase;



KKB::uint   CharStarToUint (const char* s)
{
  if  (s == NULL)
    return 0;
  return (KKB::uint)atol (s);
}  /* CharStarToUint */




int   CharStarToInt (const char* s)
{
  if  (s == NULL)
    return 0;
  return (int)atoi (s);
}  /* CharStarToInt */



float  CharStarToFloat (const char*  s)
{
  if  (s == NULL)
    return 0.0f;
  return  (float)atof (s);
}  /* CharStarToFloat */



double  CharStarToDouble (const char*  s)
{
  if  (s == NULL)
    return 0.0;
  return  atof (s);
}  /* CharStarToDouble */




KKB::ulong  CharStarToUlong (const char*  s)
{
  if  (s == NULL)
    return 0;
  KKB::ulong  l =  (KKB::ulong)atof(s);
  return l;
}  /* CharStarToUlong */



GoalKeeperPtr    DataBase::blocker = NULL;


// Will instiate an instance of "GoalKeeper" if "blocker" does not already 
// point one.
void  DataBase::CreateBlocker ()
{
  if  (!blocker)
    GoalKeeper::Create ("DataBaseBlocker", blocker);  // Will handle Race condition.
}




int  DataBase::numDataBaseInstances = 0;



DataBase::DataBase (RunLog&  _log):
  conn            (NULL),
  featureFileDesc (NULL),
  resultSet       (NULL),
  server          (NULL),
  log             (_log)

{
  CreateBlocker ();
  blocker->StartBlock ();

  InitilizeMySqlLibrary ();

  server = GetDefaultMySqlParameters ();
  Connect ();

  numDataBaseInstances++;
  blocker->EndBlock ();
}





DataBase::DataBase (const DataBaseServerPtr  _server,
                    RunLog&                  _log
                   ):

  conn            (NULL),
  featureFileDesc (NULL),
  resultSet       (NULL),
  server          (NULL),
  log             (_log)

{
  CreateBlocker ();
  blocker->StartBlock ();

  InitilizeMySqlLibrary ();

  if  (_server)
  {
    server = new DataBaseServer (*_server);
  }
  else
  {
    log.Level (-1) << endl << endl << endl
                   << "DataBase::DataBase  ***ERROR***       No Server Parameters provided;  will use default ones." << endl
                   << endl;
    server = new DataBaseServer ();
  }

  Connect ();
  numDataBaseInstances++;
  blocker->EndBlock ();
}




DataBase::~DataBase ()
{
  CreateBlocker ();
  blocker->StartBlock ();

  if  (conn)
  {
    mysql_close (conn);
    conn = NULL;
  }

  delete  server;
  server = NULL;
  numDataBaseInstances--;
  if  (numDataBaseInstances == 0)
  {
    //mysql_library_end ();
  }
  blocker->EndBlock ();
}



void  DataBase::InitilizeMySqlLibrary ()
{
  if  (numDataBaseInstances == 0)
    mysql_library_init (0, NULL, NULL);


  bool  threadSafe = mysql_thread_safe () != 0;
  if  (!threadSafe)
  {
    ofstream  o ("c:\\Temp\\QueryStatement_ErrorLog.txt", ios::app);
    o << endl
      << osGetLocalDateTime () << " : " << "DataBase::DataBase     ***NOT THREAD SAVE ***" << endl
      << endl;
    o.close ();
  }
}  /* InitilizeMySqlLibrary */



bool  DataBase::ThreadInit ()
{
  valid = (bool)(mysql_thread_init () != 0);
  return  valid;
}  /* ThreadInit */



void  DataBase::ThreadEnd ()
{
  mysql_thread_end ();
}  /* ThreadEnd */



DataBaseServerListPtr  DataBase::GetListOfServers (RunLog&  _log)
{
  CreateBlocker ();
  blocker->StartBlock ();

  DataBaseServerListPtr  list = new DataBaseServerList (_log);

  blocker->EndBlock ();
  return  list;
}



KKStr  DataBase::ServerDescription ()  const
{
  if  (server == NULL)
    return  "*** no server paranmeters selected ***";

  KKStr  description = server->ServerDescription ();
  if  (!Valid ())
    description << "  ***ERROR***";

  return  description;
}  /* ServerDescription */




int  DataBase::Connect ()
{
  if  (conn)
  {
    mysql_close (conn);
    conn = NULL;
  }

  conn = mysql_init (NULL);

  if  (conn == NULL)
  {
    log.Level (-1) << endl << endl
                   << "DataBase     ***ERROR***" << endl
                   << endl
                   << "       mysql_init ()    Failed    No database activity can work without it" << endl
                   << endl;
    valid = false;
    return - 1;
  }


  //uint  timeOut = 120;
  //int returnCd = mysql_options (conn, MYSQL_OPT_CONNECT_TIMEOUT, (const char*)(&timeOut)); 

  MYSQL*  returnMYSQLstruct = 
        mysql_real_connect (conn,                /* pointer to connection handler */
                            server->HostName     ().Str (),
                            server->UserName     ().Str (),
                            server->PassWord     ().Str (),
                            server->DataBaseName ().Str (), /* Database Name                 */
                            0,                        /* port (use default)            */
                            NULL,                     /* socket (use default)          */
                            CLIENT_MULTI_STATEMENTS   /* flags (none)                  */
                           );

  if  (returnMYSQLstruct == NULL)
  {
    // The connection failed.
    valid = false;
    lastMySqlErrorDesc = mysql_error (conn);
    lastMySqlErrorNo   = mysql_errno (conn);
    log.Level (-1) << endl 
                   << endl
                   << "DataBase     ***ERROR***" << endl
                   << endl
                   << "       ErrorCd      [" << lastMySqlErrorNo        << "]" << endl
                   << "       Error        [" << lastMySqlErrorDesc      << "]" << endl
                   << "       HostName     [" << server->HostName     () << "]" << endl
                   << "       UserName     [" << server->UserName     () << "]" << endl
                   << "       DataBaseName [" << server->DataBaseName () << "]" << endl
                   << endl;
    return  lastMySqlErrorNo;
  }
  else
  {
    valid = true;
    lastMySqlErrorNo = 0;
    lastMySqlErrorDesc = "";
    return 0;
  }
}  /* Connect */






DataBaseServerPtr  DataBase::GetDefaultMySqlParameters ()
{
  DataBaseServerPtr  defaultParameters = NULL;
  {
    DataBaseServerListPtr servers = this->GetListOfServers (log);
    if  (servers)
    {
      DataBaseServerPtr entryInList = servers->GetDefaultServer ();
      if  (entryInList)
        defaultParameters = new DataBaseServer (*entryInList);
      delete  servers;
    }
  }

  if  (!defaultParameters)
  {
    defaultParameters = new DataBaseServer ();
  }

  return  defaultParameters;
}  /* GetDefaultMySqlParameters */





bool  DataBase::ResultSetLoad (char**   fieldNames)
{
  resultSetFieldDefs  = NULL;
  resultSetNumRows    = 0;
  resultSetNumFields  = 0;
  resultSetMore       = false;
  resultSet           = NULL;
  resultSetNextRow    = NULL;
  resultSetLengths    = NULL;
  resultSetFieldIndex.clear ();
 
  resultSet = mysql_use_result (conn); /* generate result set */
  if  (resultSet == NULL)
  {
    resultSetMore = false;
    lastMySqlErrorDesc = mysql_error (conn);
    lastMySqlErrorNo   = mysql_errno (conn);

    log.Level (-1) << endl << endl 
                   << "DataBase::ResultSetLoad    ***ERROR***     occured when retrieving result set." << endl
                   << endl
                   << "ErrorDesc[" << lastMySqlErrorDesc << "]"  << endl
                   << endl;
    return  false;
  }

  resultSetMore = true;
  resultSetNumRows    = (uint)mysql_num_rows   (resultSet);
  resultSetNumFields  = (uint)mysql_num_fields (resultSet);
  resultSetFieldDefs  = mysql_fetch_fields (resultSet);

  if  (fieldNames)
  {
    ResultSetBuildFieldIndexTable (fieldNames);
  }
  else
  {
    for (uint x = 0; x < resultSetNumFields;  x++)
      resultSetFieldIndex.push_back (x);
  }

  return  resultSetMore;
}  /* ResultSetLoad */




bool   DataBase::ResultSetGetNext (char**  fieldNames)
{
  int  returnCd = mysql_next_result (conn);
  if  (returnCd == 0)
  {
    resultSetMore = true;
    ResultSetLoad (fieldNames);
  }
  else
  {
    resultSetMore= false;
  }

  return  resultSetMore;
}  /* ResultSetGetNext */




void DataBase::ResulSetFree ()
{
  if  (resultSet)
    mysql_free_result (resultSet);
  resultSet = NULL;
  resultSetFieldDefs = NULL;
  resultSetNumFields = 0;
  resultSetNumRows   = 0;
  resultSetNextRow   = NULL;
  resultSetLengths   = NULL;
  resultSetFieldIndex.clear ();
}


/**
 @brief  Call this method after you are done proessing the results of the last query.  If you do not 
       do this the next call may fail.
 */
void  DataBase::ResultSetsClear ()
{
  if  (resultSet)
    ResulSetFree ();

  int returnCd = mysql_next_result (conn);
  while  (returnCd == 0)
  {
    resultSet = mysql_use_result (conn); /* generate result set */
    if  (resultSet)
    {
      do  
      {
        resultSetNextRow = mysql_fetch_row (resultSet);
      } while  (resultSetNextRow);
      ResulSetFree ();
    }
    returnCd = mysql_next_result (conn);
  }
}  /* ResultSetsClear*/




/**
 @brief  Returns an array with the indirection index for each field listed in fieldNames.
 @details
  Will return an array that has the indirection index for each field in fieldNames.
  ex   results[2]  specifies which entry in the results row contains the value for 
  the field name in 'fieldNames[2]'.
 @param[in]  fieldNames  List of field names to get indirection matrix.  The last enry should be NULL to mark end of list.
 */
void  DataBase::ResultSetBuildFieldIndexTable (char** fieldNames)
{
  if  ((resultSet == NULL)  ||  (fieldNames == NULL))
    return;

  resultSetFieldIndex.clear ();

  if  (resultSetNumFields < 1)
    return;

  int  fieldNum = 0;

  while  (fieldNames[fieldNum])
  {
    const char* fieldName = fieldNames[fieldNum];
    int idx = -1;
    for  (uint x = 0;  x < resultSetNumFields;  x++)
    {
      if  (_stricmp (resultSetFieldDefs[x].name, fieldName) == 0)
      {
        idx = x;
        break;
      }
    }
    resultSetFieldIndex.push_back (idx);
    fieldNum++;
  }

  return;
}  /* ResultSetBuildFieldIndexTable */



  
bool  DataBase::ResultSetFetchNextRow ()
{
  if  (resultSet == NULL)
  {
    log.Level (-1) << endl << endl
                   << "DataBase::ResultSetFetchNextRow : 'resultSet' == NULL   ***ERROR***" << endl
                   << "                   Prev SQL Cmd : " << prevQueryStatement << endl
                   << endl;

    ofstream  l ("c:\\Temp\\QueryStatement_ErrorLog.txt", ios::app);
    l << endl 
      << osGetLocalDateTime () << " : " << "DataBase::ResultSetFetchNextRow   'resultSet' == NULL   ***ERROR***" << endl
      << "             Prev SQL Cmd :" << prevQueryStatement << endl
      << "           LastMySQLError :" << lastMySqlErrorNo   << "\t" << lastMySqlErrorDesc << endl
      << endl;
    l.close ();
    return false;
  }

  resultSetLengths = NULL;
  resultSetNextRow = mysql_fetch_row (resultSet);
  return  resultSetNextRow != NULL;
}



const char*  DataBase::ResultSetGetField (uint fieldIdx)
{
  if  ((fieldIdx < 0)  ||  (fieldIdx >= (int)resultSetFieldIndex.size ()))
    return  "";

  uint  rowFieldIndex = resultSetFieldIndex[fieldIdx];
  if  ((rowFieldIndex < 0)  ||  (rowFieldIndex >= resultSetNumFields))
    return  "";

  if  (!resultSetNextRow)
  {
    log.Level (-1) << endl << endl
                   << "DataBase::ResultSetGetField     ***ERROR***" << endl
                   << "                             There is no active ResultSetRow." << endl
                   << endl;
    return  NULL;
  }

  //if  (resultSetNextRow[rowFieldIndex] == NULL)
  //  return  "";
  
  return  resultSetNextRow[rowFieldIndex];
}



KKB::ulong  DataBase::ResultSetGetFieldLen (uint  fieldIdx)
{
  if  (!resultSetLengths)
    resultSetLengths = mysql_fetch_lengths (resultSet);

  if  (!resultSetLengths)
    return  0;

  if  ((fieldIdx < 0)  ||  (fieldIdx >= (int)resultSetFieldIndex.size ()))
    return  0;

  uint  rowFieldIndex = resultSetFieldIndex[fieldIdx];
  if  ((rowFieldIndex < 0)  ||  (rowFieldIndex >= resultSetNumFields))
    return  0;

  return  resultSetLengths[rowFieldIndex];
}



RasterPtr  DataBase::ResultSetGetRasterField (uint fieldIdx)
{
  const char*  buffer = ResultSetGetField (fieldIdx);
  if  (buffer == NULL)
    return NULL;
  ulong  bufferLen = ResultSetGetFieldLen (fieldIdx);
  RasterPtr r = Raster::FromCompressor ((uchar*)buffer, bufferLen);
  return  r;
}



KKStr  DataBase::ResultSetGetKKStrField (uint fieldIdx)
{
  return  ResultSetGetField (fieldIdx);
}



bool  DataBase::ResultSetGetBool (uint fieldIdx)
{
  KKStr s = ResultSetGetField (fieldIdx);
  s.Upper ();
  return  ((s == "1")  ||  (s == "TRUE")  ||  (s == "YES")  ||  (s == "T")  ||  (s == "Y"));
}




float  DataBase::ResultSetGetFloatField (uint fieldIdx)
{
  return  CharStarToFloat (ResultSetGetField (fieldIdx));
}



DateTime  DataBase::ResultSetGetDateTimeField (uint fieldIdx)
{
  return  DateTimeFromMySqlDateTimeField (ResultSetGetField (fieldIdx));
}



double  DataBase::ResultSetGetDoubleField (uint fieldIdx)
{
  return  CharStarToDouble (ResultSetGetField (fieldIdx));
}


int   DataBase::ResultSetGetIntField (uint fieldIdx)
{
  return  CharStarToInt (ResultSetGetField (fieldIdx));
}


KKB::uint   DataBase::ResultSetGetUintField (uint fieldIdx)
{
  return  CharStarToUint (ResultSetGetField (fieldIdx));
}


int   DataBase::ResultSetGetUlongField (uint fieldIdx)
{
  return  CharStarToUlong (ResultSetGetField (fieldIdx));
}



KKStr  DataBase::LastErrorDesc ()  const
{
  return  lastMySqlErrorDesc;
}



KKB::DateType  DataBase::DateFromMySqlDateTimeField (KKStr  field)
{
  int  year  = field.ExtractTokenInt ("-");
  int  month = field.ExtractTokenInt ("-");
  int  day   = field.ToInt ();

  if  ((year < 1900)  || (year > 2199)  ||  (month < 1)  ||  (month > 12)  || (day < 1)  || (day > 31))
    return DateType (1900, 1, 1);

  return  DateType (year, month, day);
}  /* DateFromMySqlDateTimeField */





DateTime  DataBase::DateTimeFromMySqlDateTimeField (KKStr  field)
{
  KKStr  yearStr   = field.ExtractToken ("-");
  KKStr  monthStr  = field.ExtractToken ("-");
  KKStr  dayStr    = field.ExtractToken (" ");
  KKStr  hourStr   = field.ExtractToken (":");
  KKStr  minStr    = field.ExtractToken (":");
  
  DateTime result ((uint) yearStr.ToInt (),  (uchar)monthStr.ToInt (), (uchar)dayStr.ToInt (), 
                   (uchar)hourStr.ToInt (),  (uchar)minStr.ToInt (),   (uchar)field.ToInt ());

  return  result;
}  /* DateTimeFromMySqlField */



KKStr  DataBase::DateTimeToQuotedStr (const DateTime&  dt)
{
  KKStr  result = "\"" + dt.Date ().YYYY_MM_DD () + " " + dt.Time ().HH_MM_SS () + "\"";
  return  result;
}




KKStr  DateToQuotedStr (const DateType&  d)
{
  KKStr  result = "\"" + d.YYYY_MM_DD () + "\"";
  return  result;
}





int  DataBase::QueryStatement (const char* queryStr,
                               int         queryStrLen
                              )
{
  if  (resultSet != NULL)
  {
    // This should not happen,  We will add warning to Sql ERRO File.
    log.Level (-1) << endl
      << "DataBase::QueryStatement   ***ERROR***    'resultSet != NULL'   This should never happen." << endl
      << "     Prev Statement[" << prevQueryStatement << "]" << endl
      << "     new Statement [" << queryStr           << "]" << endl
      << endl;

    ofstream  l ("c:\\Temp\\QueryStatement_ErrorLog.txt", ios::app);
    l << endl
      << "'resultSet != NULL'   This should never happen."   << endl
      << "     Prev Statement[" << prevQueryStatement << "]" << endl
      << "     new Statement [" << queryStr           << "]" << endl
      << endl;
    l.close ();

    // Since we do not know the status of teh Prebvious ResultSet we will set it to NULL.
    resultSet = NULL;
  }

  int  outOfSeqLoopCount = 0;

  int  attempt = 1;


  while  (true)
  {
    int returnCd = mysql_real_query (conn, queryStr, queryStrLen);
    if  (returnCd == 0)
      break;

    lastMySqlErrorDesc = mysql_error (conn);
    lastMySqlErrorNo   = mysql_errno (conn);

    if  ((lastMySqlErrorNo == CR_SERVER_GONE_ERROR)  ||  (lastMySqlErrorNo == CR_SERVER_LOST))
    {
      // We will attempt to reconnect to the server 
      KKStr  queryStrMsg;
      if  (queryStrLen > 256)  queryStrMsg.Append (queryStr, 255);
      else  queryStrMsg = queryStr;

      log.Level (-1) << endl << endl << endl
                     << "DataBase::QueryStatement    Server Connection has been Lost;  will retry in 2 secs." << endl
                     << endl
                     << "          lastMySqlErrorDesc[" << lastMySqlErrorDesc << "]" << endl
                     << "          lastMySqlErrorNo  [" << lastMySqlErrorNo   << "]" << endl
                     << endl;

      ofstream  l ("c:\\Temp\\QueryStatement_ErrorLog.txt", ios::app);
      l << osGetLocalDateTime () << " : " << "Server Connection has been Lost" << endl
        << osGetLocalDateTime () << " : " << lastMySqlErrorNo << "\t" << lastMySqlErrorDesc << endl
        << queryStrMsg << endl 
        << endl;
      l.close ();

      returnCd = Connect ();
      while  (returnCd != 0)
      {
        if  (attempt > 30)
        {
          log.Level (-1) << endl 
                         << "DataBase::QueryStatement  30 Attepts at reconnectionhave failed." << endl
                         << endl;
          
          ofstream  l ("c:\\Temp\\QueryStatement_ErrorLog.txt", ios::app);
          l << endl
            << osGetLocalDateTime () << " : " << "Server Connection could   NOT  be reestablished." << endl
            << endl;
          l.close ();

          return  CR_SERVER_LOST;
        }

        if  (attempt > 10)
          osSleep (10);
        else if  (attempt > 3)
          osSleep (5);
        else
          osSleep (2.0f);

        attempt++;
        log.Level (-1) << endl 
                       << endl << "DataBase::QueryStatement    Attempt[" << attempt << "]"  << endl
                       << endl;
                          
        {
          ofstream  l ("c:\\Temp\\QueryStatement_ErrorLog.txt", ios::app);
          l << endl
            << osGetLocalDateTime () << " : " << "ReConnection Attempt[" << attempt << "]." << endl
            << endl;
          l.close ();
        }

        returnCd = Connect ();
      }

      log.Level (-1) << endl
                     << "DataBase::QueryStatement    Connection reestablished after [" << attempt << "] attempts." << endl
                     << endl;

      ofstream  dbLogFile ("c:\\Temp\\QueryStatement_ErrorLog.txt", ios::app);
      dbLogFile << endl
                << osGetLocalDateTime () << " : " << "Server Connection   *** Reestablished ***." << endl
                << endl;
      dbLogFile.close ();

      continue;
    }

    if  (lastMySqlErrorNo == 2014)
    {
      // Calls are out of sequence;  

      KKStr  curStatement;
      if  (queryStrLen > 512)  curStatement.Append (queryStr, 512);
      else  curStatement.Append (queryStr, queryStrLen);

      log.Level (-1) << endl << endl << endl
                     << "DataBase::QueryStatement    MySQL calls out of sequence." << endl
                     << endl
                     << "          lastMySqlErrorDesc[" << lastMySqlErrorDesc << "]" << endl
                     << "          lastMySqlErrorNo  [" << lastMySqlErrorNo   << "]" << endl
                     << endl;

      ofstream  l ("c:\\Temp\\QueryStatement_ErrorLog.txt", ios::app);
      l << osGetLocalDateTime () << " : " << "MySQL Calls out of sequience" << endl
        << osGetLocalDateTime () << " : " << lastMySqlErrorNo << "\t" << lastMySqlErrorDesc << endl
        << "Prev Statement : " << prevQueryStatement << endl 
        << "Cur  Statement : " << curStatement       << endl 
        << "Loop Count     : " << outOfSeqLoopCount  << endl
        << endl;
      l.close ();
      if  (outOfSeqLoopCount == 0)
      {
        ResultSetsClear ();
      }

      else if  (outOfSeqLoopCount == 1)
      {
        // Clearing result sets did not work so lets try closing and opening connection
        returnCd = Connect ();
      }

      else if  (outOfSeqLoopCount == 1)
      {
        return  lastMySqlErrorNo;
      }

      outOfSeqLoopCount++;
    }

    duplicateKey = ((lastMySqlErrorNo == 1022)  ||  (lastMySqlErrorNo == 1062));

    if  (returnCd != 0)
    {
      valid = false;

      ofstream  l ("c:\\Temp\\QueryStatement_ErrorLog.txt", ios::app);
      l << osGetLocalDateTime () << " : " << lastMySqlErrorNo << "\t" << lastMySqlErrorDesc << endl
        << queryStr << endl 
        << endl;
      l.close ();

      return  lastMySqlErrorNo;
    }
  }  /* End of while(true)  loop */

  valid = true;

  prevQueryStatement = "";
  if  (queryStrLen > 512)
    prevQueryStatement.Append (queryStr, 512);
  else
    prevQueryStatement.Append (queryStr, queryStrLen);

  return 0;
}  /* QueryStatement */




int  DataBase::QueryStatement2 (const char* queryStr,
                                int         queryStrLen
                               )
{
  if  (resultSet != NULL)
  {
    // This should not happen,  We will add warning to Sql ERRO File.
    log.Level (-1) << endl
      << "DataBase::QueryStatement   ***ERROR***    'resultSet != NULL'   This should never happen." << endl
      << "     Prev Statement[" << prevQueryStatement << "]" << endl
      << "     new Statement [" << queryStr           << "]" << endl
      << endl;

    ofstream  l ("c:\\Temp\\QueryStatement_ErrorLog.txt", ios::app);
    l << endl
      << "'resultSet != NULL'   This should never happen."   << endl
      << "     Prev Statement[" << prevQueryStatement << "]" << endl
      << "     new Statement [" << queryStr           << "]" << endl
      << endl;
    l.close ();

    // Since we do not know the status of teh Prebvious ResultSet we will set it to NULL.
    resultSet = NULL;
  }

  int  outOfSeqLoopCount = 0;

  int  attempt = 1;


  int returnCd = mysql_real_query (conn, queryStr, queryStrLen);
  if  (returnCd != 0)
  {
    lastMySqlErrorDesc = mysql_error (conn);
    lastMySqlErrorNo   = mysql_errno (conn);

    if  ((lastMySqlErrorNo == CR_SERVER_GONE_ERROR)  ||  (lastMySqlErrorNo == CR_SERVER_LOST))
    {
      // We will attempt to reconnect to the server 
      KKStr  queryStrMsg;
      if  (queryStrLen > 256)  queryStrMsg.Append (queryStr, 255);
      else  queryStrMsg = queryStr;

      log.Level (-1) << endl << endl << endl
                     << "DataBase::QueryStatement    Server Connection has been Lost;  will retry in 2 secs." << endl
                     << endl
                     << "          lastMySqlErrorDesc[" << lastMySqlErrorDesc << "]" << endl
                     << "          lastMySqlErrorNo  [" << lastMySqlErrorNo   << "]" << endl
                     << endl;

      ofstream  l ("c:\\Temp\\QueryStatement_ErrorLog.txt", ios::app);
      l << osGetLocalDateTime () << " : " << "Server Connection has been Lost" << endl
        << osGetLocalDateTime () << " : " << lastMySqlErrorNo << "\t" << lastMySqlErrorDesc << endl
        << queryStrMsg << endl 
        << endl;
      l.close ();

      Connect ();
      return  returnCd;
    }
    else
    {
      duplicateKey = ((lastMySqlErrorNo == 1022)  ||  (lastMySqlErrorNo == 1062));
      if  (returnCd != 0)
      {
        valid = false;

        ofstream  l ("c:\\Temp\\QueryStatement_ErrorLog.txt", ios::app);
        l << osGetLocalDateTime () << " : " << lastMySqlErrorNo << "\t" << lastMySqlErrorDesc << endl
          << queryStr << endl 
          << endl;
        l.close ();

        return  lastMySqlErrorNo;
      }
    }
  }  /* End of while(true)  loop */

  valid = true;

  prevQueryStatement = "";
  if  (queryStrLen > 512)
    prevQueryStatement.Append (queryStr, 512);
  else
    prevQueryStatement.Append (queryStr, queryStrLen);

  return 0;
}  /* QueryStatement2 */





int  DataBase::QueryStatement (const KKStr& statement)
{
  int  outOfSeqLoopCount = 0;
  while  (true)
  {
    int returnCd = mysql_real_query (conn, statement.Str (), statement.Len ());
    if  (returnCd == 0)
      break;

    lastMySqlErrorDesc = mysql_error (conn);
    lastMySqlErrorNo   = mysql_errno (conn);

    if  ((lastMySqlErrorNo == CR_SERVER_GONE_ERROR)  ||  (lastMySqlErrorNo == CR_SERVER_LOST))
    {
      // We will attempt to reconnect to the server 
      log.Level (-1) << endl << endl << endl
                     << "DataBase::QueryStatement    Server Connection has been Lost;  will retry in 2 secs." << endl
                     << endl
                     << "          lastMySqlErrorDesc[" << lastMySqlErrorDesc << "]" << endl
                     << "          lastMySqlErrorNo  [" << lastMySqlErrorNo   << "]" << endl
                     << endl;

      ofstream  l ("c:\\Temp\\QueryStatement_ErrorLog.txt", ios::app);
      l << osGetLocalDateTime () << " : " << "Server Connection has been Lost" << endl
        << osGetLocalDateTime () << " : " << lastMySqlErrorNo << "\t" << lastMySqlErrorDesc << endl
        << statement << endl 
        << endl;
      l.close ();

      int  attempt = 1;
      returnCd = Connect ();
      while  (returnCd != 0)
      {
        if  (attempt > 30)
        {
          log.Level (-1) << endl 
                         << "DataBase::QueryStatement  30 Attepts at reconnectionhave failed." << endl
                         << endl;
          
          ofstream  l ("c:\\Temp\\QueryStatement_ErrorLog.txt", ios::app);
          l << endl
            << osGetLocalDateTime () << " : " << "Server Connection could   NOT  be reestablished." << endl
            << endl;
          l.close ();

          return  CR_SERVER_LOST;
        }

        osSleep (2.0f);
        attempt++;
        log.Level (-1) << endl 
                       << endl << "DataBase::QueryStatement    Attempt[" << attempt << "]"  << endl
                       << endl;
                          
        {
          ofstream  l ("c:\\Temp\\QueryStatement_ErrorLog.txt", ios::app);
          l << endl
            << osGetLocalDateTime () << " : " << "ReConnection Attempt[" << attempt << "]." << endl
            << endl;
          l.close ();
        }

        returnCd = Connect ();
      }

      log.Level (-1) << endl
                     << "DataBase::QueryStatement    Connection reestablished after [" << attempt << "] attempts." << endl
                     << endl;

      ofstream  dbLogFile ("c:\\Temp\\QueryStatement_ErrorLog.txt", ios::app);
      dbLogFile << endl
                << osGetLocalDateTime () << " : " << "Server Connection   *** Reestablished ***." << endl
                << endl;
      dbLogFile.close ();

      continue;
    }

    if  (lastMySqlErrorNo == 2014)
    {
      // Calls are out of sequence;  
      log.Level (-1) << endl << endl << endl
                     << "DataBase::QueryStatement    MySQL calls out of sequence." << endl
                     << endl
                     << "          lastMySqlErrorDesc[" << lastMySqlErrorDesc << "]" << endl
                     << "          lastMySqlErrorNo  [" << lastMySqlErrorNo   << "]" << endl
                     << endl;

      ofstream  l ("c:\\Temp\\QueryStatement_ErrorLog.txt", ios::app);
      l << osGetLocalDateTime () << " : " << "MySQL Calls out of sequience" << endl
        << osGetLocalDateTime () << " : " << lastMySqlErrorNo << "\t" << lastMySqlErrorDesc << endl
        << "Prev Statement : " << prevQueryStatement << endl 
        << "Cur  Statement : " << statement          << endl 
        << "Loop Count     : " << outOfSeqLoopCount  << endl
        << endl;
      l.close ();
      if  (outOfSeqLoopCount == 0)
      {
        ResultSetsClear ();
      }

      else if  (outOfSeqLoopCount == 1)
      {
        // Clearing result sets did not work so lets try closing and opening connection
        returnCd = Connect ();
      }

      else if  (outOfSeqLoopCount == 1)
      {
        return  lastMySqlErrorNo;
      }

      outOfSeqLoopCount++;
    }

    duplicateKey = ((lastMySqlErrorNo == 1022)  ||  (lastMySqlErrorNo == 1062));

    if  (returnCd != 0)
    {
      valid = false;

      ofstream  l ("c:\\Temp\\QueryStatement_ErrorLog.txt", ios::app);
      l << osGetLocalDateTime () << " : " << lastMySqlErrorNo << "\t" << lastMySqlErrorDesc << endl
        << "Cur  Statement : " << statement          << endl
        << "Prev Statement : " << prevQueryStatement << endl
        << endl;
      l << endl;
      l.close ();

      return  lastMySqlErrorNo;
    }
  }

  valid = true;

  prevQueryStatement = statement;

  return 0;
}  /* QueryStatement */





KKStrMatrixPtr  DataBase::QueryStatement (const KKStr&  statement,
                                          char**        colsToReturn
                                         )
{
  int  returnCd = QueryStatement (statement.Str (), statement.Len ());
  if  (returnCd != 0)
    return NULL;

  if  (!ResultSetLoad (colsToReturn))
    return NULL;

  uint numCols = resultSetNumFields;
  uint col = 0;

  KKStrMatrixPtr  results = new KKStrMatrix (numCols);
  while  (ResultSetFetchNextRow ())
  {
    KKStrListPtr  newResultRow = new KKStrList (true);
    for  (col = 0;  col < numCols;  col++)
    {
      newResultRow->PushOnBack (new KKStr (ResultSetGetField (col)));
    }
    results->AddRow (newResultRow);
  }

  ResulSetFree ();
  ResultSetsClear ();

  return  results;
}  /* QueryStatement */





//********************************************************************************************
//*                                 FeatureData   Routines                                   *
//********************************************************************************************

void  DataBase::FeatureDataCreateTable ()
{
  KKStr  createStatement;

  createStatement << "create table  FeatureData"         << endl
                  << "("                                 << endl
                  << "  ImageFileName    varchar(32)   not null,"   << endl
                  << "  ScannerFileName   char (14)     not null,"   << endl;

  int  fieldNum = 0;
  for  (fieldNum = 0;  fieldNum < PostLarvaeFV::MaxNumOfFeatures ();  fieldNum++)
  {
    KKStr  fieldName = PostLarvaeFV::FeatureName (fieldNum);
    if  (!fieldName.Empty ())
      createStatement << "  " << PostLarvaeFV::FeatureName (fieldNum) << "  real  null," << endl;
  }

  createStatement << endl
                  << "  Primary Key        (ImageFileName   ASC),"  << endl
                  << "  Key ScannerFileKey (ScannerFileName  ASC)"  << endl
                  << ")";

  int  returnCd = QueryStatement (createStatement);

  if  (returnCd != 0)
  {
    log.Level (-1) << endl << endl
                   << "DataBase::CreateFeatureDataTable    ***ERROR***" << endl
                   << endl
                   << "         Error[" << lastMySqlErrorDesc << "] occured trying to create FeatureData table." << endl
                   << endl;
    valid = false;
    return;
  }

  ResultSetsClear ();
}  /* CreateFeatureDataTable */




void  DataBase::FeatureDataInsertRow (const KKStr&          _scannerFileName,
                                      const PostLarvaeFV&  example
                                     )
{
  KKStr  scannerFileName = osGetRootName (_scannerFileName);
  KKStr  imageFileName  = osGetRootName (example.ImageFileName ());

  KKStr  insertStatement (3000);
  insertStatement << "call  FeatureDataInsert(" << imageFileName.QuotedStr () << ", " << scannerFileName.QuotedStr ();
  for  (int fieldNum = 0;  fieldNum < example.NumOfFeatures ();  fieldNum++)
  {
    double f = example.FeatureData (fieldNum);
    if  ((_isnan (f))  ||  (!_finite (f)))
      insertStatement << ", " << "NULL";
    else
      insertStatement << ", " << example.FeatureData (fieldNum);
  }
  insertStatement << ")";

  int  returnCd = QueryStatement (insertStatement);
  ResultSetsClear ();
}  /* FeatureDataInsertRow */




int  LookUpFeatureDataField (const KKStr& fieldName)
{
  int  fieldNum = 0;
  for  (fieldNum = 0;  fieldNum < PostLarvaeFV::MaxNumOfFeatures ();  fieldNum++)
  {
    if  (fieldName == PostLarvaeFV::FeatureName (fieldNum))
      return  fieldNum;
  }
  return -1;
}  /* LookUpFeatureDataField */




char**   DataBase::featureDataFieldNames = NULL;



char**  DataBase::GetFeatureDataFieldNames ()
{
  if  (featureDataFieldNames != NULL)
    return  featureDataFieldNames;

  CreateBlocker ();
  blocker->StartBlock ();

  if  (featureDataFieldNames != NULL)
  {
    // Looks like another thread must have been in th emiddle of creating this list.
    blocker->EndBlock ();
    return  featureDataFieldNames;
  }

  char**  fieldNames = NULL;
  int  fieldNamesSize = 15 + PostLarvaeFV::MaxNumOfFeatures ();
  fieldNames = new char*[fieldNamesSize];
  fieldNames[ 0] = "ImageId";
  fieldNames[ 1] = "ImageFileName";
  fieldNames[ 2] = "TopLeftRow";
  fieldNames[ 3] = "TopLeftCol";
  fieldNames[ 4] = "PixelCount";
  fieldNames[ 5] = "CentroidRow";
  fieldNames[ 6] = "CentroidCol";
  fieldNames[ 7] = "Class1Id";
  fieldNames[ 8] = "Class1Name";
  fieldNames[ 9] = "Class1Prob";
  fieldNames[10] = "Class2Prob";
  fieldNames[11] = "ClassValidatedId";
  fieldNames[12] = "ClassNameValidated";
  fieldNames[13] = "ImagesDepth";
   
  int  fieldNum = 0;
  for  (fieldNum = 0;  fieldNum < PostLarvaeFV::MaxNumOfFeatures ();  fieldNum++)
    fieldNames[14 + fieldNum] = STRDUP (PostLarvaeFV::FeatureName (fieldNum).Str ());
  fieldNames[14 + fieldNum] = NULL;

  featureDataFieldNames = fieldNames;
  blocker->EndBlock ();
  return  featureDataFieldNames;
}  /* GetFeatureDataFieldNames */





PostLarvaeFVListPtr   DataBase::FeatureDataProcessResults ()
{
  char**  fieldNames = GetFeatureDataFieldNames ();

  if  (!featureFileDesc)
    featureFileDesc = PostLarvaeFV::PostLarvaeFeaturesFileDesc (log);

  ResultSetLoad (fieldNames);
  if  (!resultSetMore)
    return  NULL;

  PostLarvaeFVListPtr  results = new PostLarvaeFVList (featureFileDesc, true, log, resultSetNumRows);

  uint  topLeftRow  = 0;
  uint  topLeftCol  = 0;

  KKStr  class1Name         (64);
  int    class1Id = -1;
  KKStr  classNameValidated (64);
  int    classValidatedId = -1;
  float  class1Prob = 0.0f;
  float  class2Prob = 0.0f;

  float  imagesDepth = 0.0f;

  while  (ResultSetFetchNextRow ())
  {
    PostLarvaeFVPtr  i = new PostLarvaeFV (featureFileDesc->NumOfFields ());

    i->ImageFileName (ResultSetGetField      (1));
    topLeftRow = ResultSetGetUintField       (2);
    topLeftCol = ResultSetGetUintField       (3);
    i->OrigSize (ResultSetGetFloatField      (4));
    i->CentroidRow (ResultSetGetFloatField   (5));
    i->CentroidCol (ResultSetGetFloatField   (6));
    class1Id   = ResultSetGetIntField        (7);
    class1Name = ResultSetGetField           (8);
    class1Prob = ResultSetGetFloatField      (9);
    class2Prob = ResultSetGetFloatField      (10);
    classValidatedId = ResultSetGetIntField  (11);
    classNameValidated = ResultSetGetField   (12);
    imagesDepth = ResultSetGetFloatField     (13);


    for  (int fieldNum = 0;  fieldNum < PostLarvaeFV::MaxNumOfFeatures ();  fieldNum++)
      i->FeatureData (fieldNum, ResultSetGetFloatField (14 + fieldNum));

    if  (class1Id == 0)
      class1Id = -1;

    if  (classValidatedId == 0)
      classValidatedId = -1;

    if  (!class1Name.Empty ())
      i->PredictedClass (ImageClass::CreateNewImageClass (class1Name, class1Id));

    if  (!classNameValidated.Empty ())
    {
      i->ImageClass (ImageClass::CreateNewImageClass (classNameValidated, classValidatedId));
      i->Validated (true);
    }
    else
    {
      if  (!class1Name.Empty ())
        i->ImageClass (ImageClass::CreateNewImageClass (class1Name, class1Id));
      else
        i->ImageClass (ImageClass::GetUnKnownClassStatic ());
    }

    i->Probability (class1Prob);
    i->BreakTie (fabs (class1Prob - class2Prob));
    i->TrainWeight (1.0f);

    i->SfCentroidCol (topLeftCol + i->CentroidCol ());
    i->SfCentroidRow (topLeftRow + i->CentroidRow ());

    results->PushOnBack (i);
  }

  ResulSetFree ();

  return  results;
}  /* FeatureDataProcessResults */




PostLarvaeFVPtr    DataBase::FeatureDataRecLoad (const KKStr&  imageFileName)
{
  KKStr  rootName = osGetRootName (imageFileName);

  KKStr  selectStr (256);

  selectStr << "call  FeatureDataLoadByImageFileName(" << rootName.QuotedStr () << ")";
  int  returnCd = QueryStatement (selectStr);
  if  (returnCd != 0)
  {
    log.Level (-1) << endl << endl 
                   << "DataBase::FeatureDataRecLoad     ***ERROR***" << endl
                   << endl
                   << "Error[" << lastMySqlErrorDesc << "]" << endl
                   << endl;
    return NULL;
  }

  PostLarvaeFVListPtr  results = FeatureDataProcessResults ();
  ResultSetsClear ();
  if  (results == NULL)  
    return NULL;

  if  (results->QueueSize () < 1)
  {
    delete  results;  results = NULL;
    return NULL;
  }

  PostLarvaeFVPtr  result = results->PopFromBack ();
  delete results;  results = NULL;

  ResultSetsClear ();
  return  result;
}  /* FeatureDataRecLoad */




/// Retrieves FeatureData for a given 'DataBaseImage' object.
PostLarvaeFVPtr   DataBase::FeatureDataRecLoad (DataBaseImagePtr  image)
{
  KKStr  selectStr (256);

  selectStr << "call  FeatureDataLoadByImageFileName(" <<image->ImageFileName ().QuotedStr () << ")";
  int  returnCd = QueryStatement (selectStr);
  if  (returnCd != 0)
  {
    log.Level (-1) << endl << endl 
                   << "DataBase::FeatureDataRecLoad     ***ERROR***" << endl
                   << endl
                   << "Error[" << lastMySqlErrorDesc << "]" << endl
                   << endl;
    return NULL;
  }

  PostLarvaeFVListPtr  results = FeatureDataProcessResults ();
  ResultSetsClear ();
  if  (results == NULL)  
    return NULL;

  if  (results->QueueSize () < 1)
  {
    delete  results;  results = NULL;
    return NULL;
  }

  PostLarvaeFVPtr  result = results->PopFromBack ();
  delete results;  results = NULL;

  ResultSetsClear ();
  return  result;
}  /* FeatureDataRecLoad */





/// Loades all Images with related Feature data for the specified 'ScannerFileEntry'
/// imageClass:               You are only interested in feature data for a specific class
PostLarvaeFVListPtr  DataBase::FeatureDataGetOneScannerFile (const KKStr&         scannerFileName,
                                                             const ImageClassPtr  imageClass,
                                                             char                 classKeyToUse,
                                                             bool&                cancelFlag
                                                            )
{
  return  NULL;
}  /* FeatureDataGetOneScannerFile */






PostLarvaeFVListPtr  DataBase::FeatureDataForImageGroup (const DataBaseImageGroupPtr  imageGroup,
                                                          const ImageClassPtr          imageClass,
                                                          char                         classKeyToUse,
                                                          const bool&                  cancelFlag
                                                         )
{
  return  NULL;
}  /* FeatureDataForImageGroup */





void   DataBase::FeatureDataUpdate (PostLarvaeFVPtr  example)
{
  KKStr  updateStr (3072);

  updateStr << "update FeatureData set ";
  for  (int featureNum = 0;  featureNum < example->NumOfFeatures ();  featureNum++)
  {
    if  (featureNum > 0)
      updateStr << ", ";
    updateStr << PostLarvaeFV::FeatureName (featureNum) << " = " << example->FeatureData (featureNum);
  }

  updateStr << "  where ImageFileName = " << example->ImageFileName ().QuotedStr ();

  int  returnCd = QueryStatement (updateStr);
}  /* FeatureDataUpdate */




//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************
//*                                 ImageClass Rotines                                      *
//*******************************************************************************************
//*******************************************************************************************
//*******************************************************************************************



ImageClassPtr  DataBase::ImageClassLoad (const KKStr&  className)
{
  KKStr  selectStr (128);

  selectStr << "call ClassesRetrieveByName(" << className.QuotedStr () << ")";
  int  returnCd = QueryStatement (selectStr);
  if  (returnCd != 0)
  {
    log.Level (-1) << endl << endl << "DataBase::ImageClassLoad     ***ERROR***" << endl << endl
                   << "Error[" << lastMySqlErrorDesc << "]" << endl << endl;
    return NULL;
  }

  ResultSetLoad (NULL);
  if  (!resultSetMore)
    return  NULL;

  ImageClassPtr  imageClass = NULL;

  while  (ResultSetFetchNextRow ())
  {
    //delete  imageClass;   //We can never delete an instance of ImageClass because there is only 
                            //one global instance of each class that everone has to share.
    imageClass = NULL;
    
    int   classId     = CharStarToInt (resultSetNextRow[0]);
    KKStr className   = resultSetNextRow[1];
    int   parentId    = CharStarToInt (resultSetNextRow[2]);
    KKStr parentName  = resultSetNextRow[3];
    KKStr description = resultSetNextRow[4];

    if  (!className.Empty ())
    {
      imageClass = ImageClass::CreateNewImageClass (className, classId);
      imageClass->Description (description);

      parentName.TrimLeft ();
      parentName.TrimRight ();
      if  (!parentName.Empty ())
        imageClass->Parent (ImageClass::CreateNewImageClass (parentName, parentId));
    }
  }

  ResulSetFree ();

  ResultSetsClear ();
  return  imageClass;
}  /* ImageClassLoad */




ImageClassListPtr  DataBase::ImageClassLoadList ()
{
  KKStr  selectStr = "call  ClassesRetrieveAll()";
  int  returnCd = QueryStatement (selectStr);
  if  (returnCd != 0)
  {
    log.Level (-1) << endl << endl << "DataBase::ImageClassLoadList     ***ERROR***" << endl << endl
                   << "Error[" << lastMySqlErrorDesc << "]" << endl << endl;
    return NULL;
  }

  ResultSetLoad (NULL);
  if  (!resultSetMore)
    return  NULL;

  ImageClassListPtr  classes = new ImageClassList ();

  vector<ImageClassPtr>  classesWithParents;
  vector<KKStr>          classesWithParentsNames;

  while  (ResultSetFetchNextRow ())
  {
    int   classId     = CharStarToInt (resultSetNextRow[0]);
    KKStr className   = resultSetNextRow[1];
    int   parentId    = CharStarToInt (resultSetNextRow[2]);
    KKStr parentName  = resultSetNextRow[3];
    KKStr description = resultSetNextRow[4];

    if  (!className.Empty ())
    {
      ImageClassPtr  c = ImageClass::CreateNewImageClass (className, classId);
      c->Description (description);
      classes->PushOnBack (c);
      c->StoredOnDataBase (true);

      if  (!parentName.Empty ())
      {
        classesWithParents.push_back (c);
        classesWithParentsNames.push_back (parentName);
      }
    }
  }

  ResulSetFree ();

  for  (uint x = 0;  x < classesWithParents.size ();  x++)
  {
    ImageClassPtr parent = classes->LookUpByName (classesWithParentsNames[x]);
    if  (parent)
    {
      if  (parent->IsAnAncestor (classesWithParents[x]))
      {
        log.Level (-1) << endl << endl << "DataBase::ImageClassLoadList    ***ERROR***     occured when retrieving result set." << endl << endl
                       << "    There is a Circular Parent Assignment" << endl 
                       << "    Class[" << classesWithParents[x]->Name () << "]   Parent[" << parent->Name () << "]" << endl
                       << endl;
        // We can not make this parent assignment;  otherwise we will end up crating a circular data structure.
      }
      else
      {
        classesWithParents[x]->Parent (parent);
      }
    }
  }

  ResultSetsClear ();

  return  classes;
}  /* ImageClassLoadList */




void  DataBase::ImageClassInsert (ImageClass&  imageClass,
                                  bool&        successful
                                 )
{
  KKStr  insertStr = "call ImageClassInsert(" + 
                     imageClass.Name ().QuotedStr ()        + ", " + 
                     imageClass.ParentName  ().QuotedStr () + ", " +
                     imageClass.Description ().QuotedStr () +
                     ")";
	
  int  returnCd = QueryStatement (insertStr);
  successful = (returnCd == 0);
  if  (successful)
    imageClass.ClassId ((int)mysql_insert_id (conn));

  ResultSetsClear ();
}  /* ImageClassInsert */






void  DataBase::ImageClassUpdate (const KKStr&       oldClassName,
                                  const ImageClass&  imageClass,
                                  bool&              successful
                                 )
{
  KKStr updateStr = "Call ImageClassUpdate(" +
                          oldClassName.QuotedStr ()              + ", " +
                          imageClass.Name ().QuotedStr ()        + ", " +
                          imageClass.ParentName  ().QuotedStr () + ", " +
                          imageClass.Description ().QuotedStr () +
                          ")";

  int  returnCd = QueryStatement (updateStr);
  successful = (returnCd == 0);
  ResultSetsClear ();
}  /* ImageClassUpdate */




void  DataBase::ImageClassDelete (const KKStr&  imageClassName)
{
  KKStr  deleteStr = "Call ImageClassDelete(" + imageClassName.QuotedStr () + ")";
  int  returnCd = QueryStatement (deleteStr);
  ResultSetsClear ();
}






//*****************************************************************************************
//*****************************************************************************************
//*****************************************************************************************
//*                                   Image  Rotines                                      *
//*****************************************************************************************
//*****************************************************************************************
//*****************************************************************************************


KKB::uchar*  DataBase::EncodeARasterImageIntoAThumbNail (const Raster&  image,
                                                         uint           maxDimSizeAllowed,
                                                         uint&          buffLen
                                                        )
{
  RasterPtr  thumbNail = NULL;
  uint  maxDim = Max (image.Height (), image.Width ());

  if  (maxDim > maxDimSizeAllowed)
  {
    float  reductionFactor = (float)maxDimSizeAllowed / (float)maxDim;
    //thumbNail = image.ReduceByEvenMultiple (multiple);
    thumbNail = image.ReduceByFactor (reductionFactor);
  }

  uchar*  compressedBuff = NULL;
  if  (thumbNail)
  {
    compressedBuff = thumbNail->SimpleCompression (buffLen);
    delete  thumbNail; 
    thumbNail = NULL;
  }
  else
  {
    compressedBuff = image.SimpleCompression (buffLen);
  }
  
  return  compressedBuff;
}  /* EncodeARasterImageIntoAThumbNail */





void  DataBase::ImageInsert (const Raster&         image,
                             const KKStr&          imageFileName,
                             const KKStr&          scannerFileName,
                                   osFilePos       byteOffset,     // byteOffset of "ScannerRow" containing TopLeftRow
                                   uint            topLeftRow,
                                   uint            topLeftCol,
                                   uint            height,
                                   uint            width,
                                   uint            pixelCount,
                                   uint            centroidRow,
                                   uint            centroidCol,
                                   ImageClassPtr   class1,
                                   float           class1Prob,
                                   ImageClassPtr   class2,
                                   float           class2Prob,
                                   ImageClassPtr   validatedClass,
                                   float           depth,
                                   int&            imageId,
                                   bool&           successful
                            )
{
  KKStr  insertStatement (3000);
  KKStr  class1Name = "";
  KKStr  class2Name = "";
  KKStr  validatedClassName = "";

  if  (class1)
    class1Name = class1->Name ();

  if  (class2)
    class2Name = class2->Name ();

  if  (validatedClass)
    validatedClassName = validatedClass->Name ();

  uint           imageCompressedBuffLen      = 0;
  uchar*         mySqlImageCompressedBuff    = NULL;
  unsigned long  mySqlImageCompressedBuffLen = 0;

  uchar* imageCompressedBuff = EncodeARasterImageIntoAThumbNail (image, 100, imageCompressedBuffLen);
  if  (imageCompressedBuff == NULL)
  {
    mySqlImageCompressedBuff = NULL;
    mySqlImageCompressedBuffLen = 0;
  }
  else
  {
    mySqlImageCompressedBuff = new uchar[imageCompressedBuffLen * 2 + 5];
    mySqlImageCompressedBuffLen = mysql_real_escape_string (conn, (char*)mySqlImageCompressedBuff, (char*)imageCompressedBuff, imageCompressedBuffLen);
  }

  insertStatement << "call ImagesInsert("
                  << imageFileName.QuotedStr ()       << ", "
                  << scannerFileName.QuotedStr ()      << ", "
                  << byteOffset                       << ", "
                  << topLeftRow                       << ", "
                  << topLeftCol                       << ", "
                  << height                           << ", "
                  << width                            << ", "
                  << pixelCount                       << ", "
                  << centroidRow                      << ", "
                  << centroidCol                      << ", "
                  << class1Name.QuotedStr ()          << ", "
                  << class1Prob                       << ", "
                  << class2Name.QuotedStr ()          << ", "
                  << class2Prob                       << ", "
                  << validatedClassName.QuotedStr ()  << ", "
                  << depth                            << ", ";

  if  (mySqlImageCompressedBuff == NULL)
  {
    insertStatement << "NULL";
  }
  else
  {
    insertStatement.Append ('"');
    insertStatement.Append ((char*)mySqlImageCompressedBuff, mySqlImageCompressedBuffLen);
    insertStatement.Append ('"');
  }

  insertStatement << ")";

  int  returnCd = QueryStatement (insertStatement);
  successful = (returnCd == 0);
  delete  mySqlImageCompressedBuff;
  mySqlImageCompressedBuff = NULL;
  
  delete  imageCompressedBuff;
  imageCompressedBuff = NULL;

  if  (successful)
  {
    imageId = (int)mysql_insert_id (conn);

    ResultSetsClear ();

    int  maxDim = Max (image.Height (), image.Width ());
    if  (maxDim > 100)
    {
      // The image had to be reduced to a thumbnail size so we want to store a copy of the original in the ImagesFullSize table.
      ImageFullSizeSave (imageFileName, image);
    }
  }
}  /* ImageInsert */





void  DataBase::ImagesEraseScannerFileEntry (const KKStr&  _scannerFileName)
{
  KKStr  scannerFileName = osGetRootName (_scannerFileName);
  int  returnCd = 0;

  KKStr  imageDeleteStatement = "call ImagesEraseScannerFileEntry(" + scannerFileName.QuotedStr () + ")";
  returnCd = QueryStatement (imageDeleteStatement);
  ResultSetsClear ();
}  /* ImagesEraseScannerFileEntry */





ClassStatisticListPtr  DataBase::ImageProcessClassStaticsResults ()
{
  ResultSetLoad (NULL);
  if  (!resultSetMore)
    return  NULL;

  ClassStatisticListPtr  stats = new ClassStatisticList (true);

  int    classId;
  KKStr  className;
  uint   count;

  while  (ResultSetFetchNextRow ())
  {
    classId   = CharStarToInt (resultSetNextRow[0]);
    className = resultSetNextRow[1];
    count     = CharStarToInt (resultSetNextRow[2]);

    if  (count == 0)
    {
      int ggg = 7767;
    }

    if  (count > 0)
    {
      ClassStatisticPtr  stat = new ClassStatistic (ImageClass::CreateNewImageClass (className, classId), count);
      stats->PushOnBack (stat);
    }
  }

  ResulSetFree ();

  ResultSetsClear ();

  return  stats;
}  /* ImageProcessClassStaticsResults */




ClassStatisticListPtr  DataBase::ImageGetClassStatistics (DataBaseImageGroupPtr  imageGroup,
                                                          const KKStr&           scannerFileName,
                                                          ImageClassPtr          imageClass,
                                                          char                   classKeyToUse,
                                                          float                  minProb,
                                                          float                  maxProb,
                                                          int                    minSize,
                                                          int                    maxSize,
                                                          float                  minDepth,
                                                          float                  maxDepth
                                                         )
{
  KKStr  selectStr (1024);

  KKStr  groupNameStr = ((imageGroup == NULL) ? "" : imageGroup->Name ());

  KKStr  classKeyToUseStr = "";
  classKeyToUseStr.Append (classKeyToUse);


  KKStr  className = "";
  if  (imageClass)
    className = imageClass->Name ();

  selectStr << "call ImagesGetClassStatistics("
            << groupNameStr.QuotedStr      () << ", "
            << scannerFileName.QuotedStr    () << ", "
            << className.QuotedStr         () << ", "
            << classKeyToUseStr.QuotedStr  () << ", "
            << minProb                        << ", "
            << maxProb                        << ", "
            << minSize                        << ", "
            << maxSize                        << ", "
            << minDepth                       << ", "
            << maxDepth
            << ")";

  int  returnCd = QueryStatement (selectStr);
  if  (returnCd != 0)
    return NULL;

  ClassStatisticListPtr  stats = ImageProcessClassStaticsResults ();
  ResultSetsClear ();
 
  return  stats;
}  /* ImageGetClassStatistics */



ClassStatisticListPtr  DataBase::ImageGetClassStatistics (DataBaseImageGroupPtr  imageGroup,
                                                          const KKStr&           cruiseName,
                                                          const KKStr&           stationName,
                                                          const KKStr&           deploymentNum,
                                                          ImageClassPtr          imageClass,
                                                          char                   classKeyToUse,
                                                          float                  minProb,
                                                          float                  maxProb,
                                                          int                    minSize,
                                                          int                    maxSize,
                                                          float                  minDepth,
                                                          float                  maxDepth
                                                         )
{
  if  (cruiseName.Empty ()  &&  stationName.Empty ()  &&  deploymentNum.Empty ())
  {
    return  ImageGetClassStatistics (imageGroup, "", imageClass, classKeyToUse, minProb, maxProb, minSize, maxSize, minDepth, maxDepth);
  }

  VectorKKStr*  scannerFileEntries = ScannerFileEntriesGetList (cruiseName, stationName, deploymentNum);

  if  (!scannerFileEntries)
    return NULL;

  ClassStatisticListPtr  stats = new ClassStatisticList (true);

  VectorKKStr::iterator  idx;
  for  (idx = scannerFileEntries->begin ();  idx != scannerFileEntries->end ();  idx++)
  {
    ClassStatisticListPtr  statsOneFile = ImageGetClassStatistics (imageGroup, *idx, imageClass, classKeyToUse, minProb, maxProb, minSize, maxSize, minDepth, maxDepth);
    if  (statsOneFile)
    {
      (*stats) += (*statsOneFile);
      delete  statsOneFile;
      statsOneFile = NULL;
    }
  }

  delete  scannerFileEntries;

  return  stats;
}  /* ImageGetClassStatistics */







VectorUint*  DataBase::ImageGetDepthStatistics (DataBaseImageGroupPtr  imageGroup,
                                                const KKStr&           scannerFileName,
                                                float                  depthIncrements,
                                                ImageClassPtr          imageClass,
                                                char                   classKeyToUse,
                                                float                  minProb,
                                                float                  maxProb,
                                                int                    minSize,
                                                int                    maxSize
                                               )
{
  KKStr  selectStr (1024);

  KKStr  groupNameStr = ((imageGroup == NULL) ? "" : imageGroup->Name ());
  KKStr  classNameStr = ((imageClass == NULL) ? "" : imageClass->Name ());

  KKStr  classKeyToUseStr = "";
  classKeyToUseStr.Append (classKeyToUse);

  VectorUint*  counts = new VectorUint;

  if  ((depthIncrements <= 0.0f)  ||  (depthIncrements > 1000.0f))
  {
    log.Level (-1) << endl << endl << "DataBase::ImageGetDepthStatistics    ***ERROR***    depthIncrements[" << depthIncrements << "] is unrealistic,  defaulting to 10.0 meters." << endl << endl;
    depthIncrements = 10.0f;
  }
  

  selectStr << "call ImagesGetDepthStatistics("
            << groupNameStr.QuotedStr      () << ", "
            << scannerFileName.QuotedStr   ()  << ", "
            << depthIncrements                << ", "
            << classNameStr.QuotedStr     ()  << ", "
            << classKeyToUseStr.QuotedStr ()  << ", "
            << minProb                        << ", "
            << maxProb                        << ", "
            << minSize                        << ", "
            << maxSize
            << ")";

  int  returnCd = QueryStatement (selectStr);
  if  (returnCd != 0)
    return NULL;

  ResultSetLoad (NULL);
  if  (!resultSetMore)
    return  counts;

  int   incIndex;
  uint  count;

  while  (ResultSetFetchNextRow ())
  {
    incIndex = atoi (resultSetNextRow[0]);
    if  (incIndex < 0)
      incIndex = 0;

    count = CharStarToUint (resultSetNextRow[1]);

    while  (counts->size () <= (uint)incIndex)
      counts->push_back (0);

    (*counts)[incIndex] = count;
  }

  ResulSetFree ();

  ResultSetsClear ();

  return  counts;
}  /* ImageGetDepthStatistics */




VectorUint*  DataBase::ImageGetDepthStatistics (DataBaseImageGroupPtr  imageGroup,
                                                const KKStr&           cruiseName,
                                                const KKStr&           stationName,
                                                const KKStr&           deploymentNum,
                                                float                  depthIncrements,
                                                ImageClassPtr          imageClass,
                                                char                   classKeyToUse,
                                                float                  minProb,
                                                float                  maxProb,
                                                int                    minSize,
                                                int                    maxSize
                                               )
{
  if  ((depthIncrements <= 0.0f)  ||  (depthIncrements > 1000.0f))
  {
    log.Level (-1) << endl
                   << endl
                   << "DataBase::ImageGetDepthStatistics    ***ERROR***    depthIncrements[" << depthIncrements << "] is unrealistic,  defaulting to 10.0 meters." << endl
                   << endl;
    depthIncrements = 10.0f;
  }


  bool  allScannerFileEntries = cruiseName.Empty ()  &&  stationName.Empty ()  &&  deploymentNum.Empty ();
  if  (allScannerFileEntries)
  {
    return ImageGetDepthStatistics (imageGroup, "", depthIncrements, imageClass, classKeyToUse, minProb, maxProb, minSize, maxSize);
  }

  VectorKKStr*  scannerFileEntries = ScannerFileEntriesGetList (cruiseName, stationName, deploymentNum);
  if  (!scannerFileEntries)
    return  NULL;

  VectorUint* counts = new VectorUint ();

  VectorKKStr::iterator  idx;
  for  (idx = scannerFileEntries->begin ();  idx != scannerFileEntries->end ();  idx++)
  {
    VectorUint*  countsThisScannerFileEntry = ImageGetDepthStatistics (imageGroup, *idx, depthIncrements, imageClass, classKeyToUse, minProb, maxProb, minSize, maxSize);
    if  (countsThisScannerFileEntry)
    {
      while  (counts->size () <= countsThisScannerFileEntry->size ())
        counts->push_back (0);
      for  (uint x = 0;  x < countsThisScannerFileEntry->size ();  x++)
        (*counts)[x] += (*countsThisScannerFileEntry)[x];

      delete  countsThisScannerFileEntry;
      countsThisScannerFileEntry = NULL;
    }
  }

  delete  scannerFileEntries;

  return  counts;
}  /* ImageGetDepthStatistics */







DataBaseImageListPtr  DataBase::ImageQueryProcessResults ()
{
  ResultSetLoad (NULL);
  if  (!resultSetMore)
    return  NULL;

  DataBaseImageListPtr  results = new DataBaseImageList (true);
 
  int  imageIdIdx            = -1;
  int  scannerFileNameIdx    = -1;
  int  class1IdIdx           = -1;
  int  class1NameIdx         = -1; 
  int  class1ProbIdx         = -1;
  int  class2IdIdx           = -1;
  int  class2NameIdx         = -1;
  int  class2ProbIdx         = -1;
  int  ClassValidatedIdIdx   = -1;
  int  classNameValidatedIdx = -1;
  int  topLeftRowIdx         = -1;
  int  topLeftColIdx         = -1;
  int  pixelCountIdx         = -1;
  int  centroidRowIdx        = -1;
  int  centroidColIdx        = -1;
  int  heightIdx             = -1;
  int  widthIdx              = -1;
  int  byteOffsetIdx         = -1;
  int  depthIdx              = -1;
  int  thumbnailIdx          = -1;

  {
    // Build Field Index
    MYSQL_FIELD *field;
    int  idx = 0;
    while  ((field = mysql_fetch_field (resultSet)))
    {
      KKStr  fieldName = field->name;
      if  (fieldName.CompareIgnoreCase ("ImageId") == 0)
        imageIdIdx = idx;
      
      else if  (fieldName.CompareIgnoreCase ("scannerFileName") == 0)
        scannerFileNameIdx = idx;

      else if  (fieldName.CompareIgnoreCase ("class1Id") == 0)
        class1IdIdx = idx;

      else if  (fieldName.CompareIgnoreCase ("class1Name") == 0)
        class1NameIdx = idx;

      else if  (fieldName.CompareIgnoreCase ("class1Prob") == 0)
        class1ProbIdx = idx;

      else if  (fieldName.CompareIgnoreCase ("class2Id") == 0)
        class2IdIdx = idx;

      else if  (fieldName.CompareIgnoreCase ("class2Name") == 0)
        class2NameIdx = idx;

      else if  (fieldName.CompareIgnoreCase ("class2Prob") == 0)
        class2ProbIdx = idx;

      else if  (fieldName.CompareIgnoreCase ("ClassValidatedId") == 0)
        ClassValidatedIdIdx = idx;

      else if  (fieldName.CompareIgnoreCase ("ClassNameValidated") == 0)
        classNameValidatedIdx = idx;

      else if  (fieldName.CompareIgnoreCase ("topLeftRow") == 0)
        topLeftRowIdx = idx;

      else if  (fieldName.CompareIgnoreCase ("topLeftCol") == 0)
        topLeftColIdx = idx;

      else if  (fieldName.CompareIgnoreCase ("pixelCount") == 0)
        pixelCountIdx = idx;

      else if  (fieldName.CompareIgnoreCase ("CentroidRow") == 0)
        centroidRowIdx = idx;

      else if  (fieldName.CompareIgnoreCase ("CentroidCol") == 0)
        centroidColIdx = idx;

      else if  (fieldName.CompareIgnoreCase ("height") == 0)
        heightIdx = idx;

      else if  (fieldName.CompareIgnoreCase ("width") == 0)
        widthIdx = idx;

      else if  (fieldName.CompareIgnoreCase ("byteOffset") == 0)
        byteOffsetIdx = idx;

      else if  (fieldName.CompareIgnoreCase ("depth") == 0)
        depthIdx = idx;

      else if  (fieldName.CompareIgnoreCase ("thumbnail") == 0)
        thumbnailIdx = idx;

      idx++;
    }
  }

  uint    numFields  = resultSetNumFields;

  uint   imageId            = 0;
  int    class1Id           = -1;
  KKStr  class1Name         = "";
  int    class2Id           = -1;
  KKStr  class2Name         = "";
  int    classValidatedId   = -1;
  KKStr  classNameValidated = "";

  while  (ResultSetFetchNextRow ())
  {
    ulong*  lengths    = mysql_fetch_lengths (resultSet);
    DataBaseImagePtr  i = new DataBaseImage ();

    class1Id           = -1;
    class1Name         = "";
    class2Id           = -1;
    class2Name         = "";
    classValidatedId   = -1;
    classNameValidated = "";
    imageId            = 0;

    if  (imageIdIdx >= 0)
      i->ImageId (CharStarToUint (resultSetNextRow[imageIdIdx]));

    if  (scannerFileNameIdx >= 0)
      i->ScannerFileName (resultSetNextRow[scannerFileNameIdx]);

    if  (class1IdIdx >= 0)
    {
      if  (resultSetNextRow[class1IdIdx])
        class1Id = CharStarToInt (resultSetNextRow[class1IdIdx]);
    }

    if  (class1NameIdx >= 0)
    {
      KKStr  class1Name = resultSetNextRow[class1NameIdx];
      if  (!class1Name.Empty ())
        i->Class1 (ImageClass::CreateNewImageClass (class1Name, class1Id));
    }

    if  (class1ProbIdx >= 0)
      i->Class1Prob (CharStarToFloat (resultSetNextRow[class1ProbIdx]));

    if  (class2IdIdx >= 0)
    {
      if  (resultSetNextRow[class2IdIdx])
        class2Id = CharStarToInt (resultSetNextRow[class2IdIdx]);
    }

    if  (class2NameIdx >= 0)
    {
      KKStr  class2Name = resultSetNextRow[class2NameIdx];
      if  (!class2Name.Empty ())
        i->Class2 (ImageClass::CreateNewImageClass (resultSetNextRow[class2NameIdx], class2Id));
    }

    if  (class2ProbIdx >= 0)
      i->Class2Prob (CharStarToFloat (resultSetNextRow[class2ProbIdx]));

    if  (ClassValidatedIdIdx >= 0)
    {
      if  (resultSetNextRow[ClassValidatedIdIdx])
        classValidatedId = CharStarToInt (resultSetNextRow[ClassValidatedIdIdx]);
    }

    if  (classNameValidatedIdx >= 0)
    {
      KKStr classNameValidated = resultSetNextRow[classNameValidatedIdx];
      if  (!classNameValidated.Empty ())
        i->ValidatedClass (ImageClass::CreateNewImageClass (classNameValidated, classValidatedId));
    }

    if  (topLeftRowIdx >= 0)
      i->TopLeftRow (CharStarToUint  (resultSetNextRow[topLeftRowIdx]));

    if  (topLeftColIdx >= 0)
      i->TopLeftCol (CharStarToUint  (resultSetNextRow[topLeftColIdx]));

    if  (pixelCountIdx >= 0)
      i->PixelCount (CharStarToUint  (resultSetNextRow[pixelCountIdx]));

    if  (centroidRowIdx >= 0)
      i->CentroidRow (CharStarToUint (resultSetNextRow[centroidRowIdx]));

    if  (centroidColIdx >= 0)
      i->CentroidCol (CharStarToUint (resultSetNextRow[centroidColIdx]));

    if  (heightIdx >= 0)
      i->Height (CharStarToUint  (resultSetNextRow[heightIdx]));

    if  (widthIdx >= 0)
      i->Width (CharStarToUint  (resultSetNextRow[widthIdx]));

    if  (byteOffsetIdx >= 0)
      i->ByteOffset (CharStarToUlong (resultSetNextRow[byteOffsetIdx]));

    if  (depthIdx >= 0)
      i->Depth (CharStarToFloat (resultSetNextRow[depthIdx]));

    if  (thumbnailIdx >= 0)
    {
      int blockLen = lengths[thumbnailIdx];

      uchar*  encodedThumbNail = new uchar[blockLen];
      memcpy (encodedThumbNail, resultSetNextRow[thumbnailIdx], blockLen);

      int encBlockLen = encodedThumbNail[0] * 255 + encodedThumbNail[1];  // This should be the same as 'blockLen'

      if  (encBlockLen != blockLen)
      {
        log.Level (-1)  << "DataBase::ImageQueryProcessResults     ***ERROR***"  << endl
                        << endl
                        << "DataBase::ImageQueryProcessResults      There is an error in Compress Block Length data." << endl
                        << endl;
      }

      i->EncodedThumbnail    (encodedThumbNail);
      i->EncodedThumbnailLen (blockLen);
    }

    results->PushOnBack (i);
  }

  ResulSetFree ();

  return  results;
}  /* ImageQueryProcessResults */





DataBaseImagePtr  DataBase::ImagesLocateClosestImage (const KKStr&  imageFileName)
{
  KKStr  scannerFileName (32);
  uint   scanLineNum = 0;
  uint  scanCol      = 0;
  PostLarvaeFV::ParseImageFileName (imageFileName, scannerFileName, scanLineNum, scanCol);
  return  ImagesLocateClosestImage (scannerFileName, scanLineNum, scanCol);
}  /* ImagesLocateClosestImage */





DataBaseImagePtr  DataBase::ImagesLocateClosestImage (const KKStr&  scannerFileName,
                                                      uint          scanLine,
                                                      uint          scanCol
                                                     )
{
  KKStr  sqlStr = "call  ImagesLocateClosestImage(" + scannerFileName.QuotedStr ()         + ", " + 
                                                      StrFormatInt (scanLine, "ZZZZZZZ0") + ", " +
                                                      StrFormatInt (scanCol,  "ZZZZZZZ0") + 
                                                 ")";
  

  KKStrMatrixPtr  results = QueryStatement (sqlStr, NULL);
  if  (results == NULL)
    return NULL;

  if  ((results->NumRows () < 1)  ||  (results->NumCols () < 2))
  {
    delete  results;
    return NULL;
  }

  KKStr  imageFileNameInDataBase = (*results)[0][1];
  delete  results;  results = NULL;

  return  ImageLoad (imageFileNameInDataBase);
}  /* ImagesLocateClosestImage */




DataBaseImagePtr  DataBase::ImageLoad (uint  imageId)
{
  KKStr selectStr (100);
  selectStr << "call ImagesLoadByImageId(" << imageId << ")";
  int  returnCd = QueryStatement (selectStr);
  if  (returnCd != 0)
    return NULL;

  DataBaseImageListPtr  images =  ImageQueryProcessResults ();

  ResultSetsClear ();
 
  if  ((images == NULL)  ||  (images->QueueSize () < 1))
  {
    delete  images;  images = NULL;
    return NULL;
  }
  DataBaseImagePtr  dbImage = images->PopFromBack ();
  delete  images; images = NULL;

  return  dbImage;
}  /* ImageLoad */





DataBaseImagePtr  DataBase::ImageLoad (const KKStr&   imageFileName)
{
  KKStr selectStr = "call ImagesLoadByImageFileName(" + imageFileName.QuotedStr () + ")";
  int  returnCd = QueryStatement (selectStr);
  if  (returnCd != 0)
    return NULL;

  DataBaseImageListPtr  images =  ImageQueryProcessResults ();

  ResultSetsClear ();
 
  if  ((images == NULL)  ||  (images->QueueSize () < 1))
  {
    delete  images;  images = NULL;
    return NULL;
  }
  DataBaseImagePtr  dbImage = images->PopFromBack ();
  delete  images; images = NULL;

  return  dbImage;
}  /* ImageLoad */






DataBaseImageListPtr  DataBase::ImageQuery (DataBaseImageGroupPtr  group,
                                            bool                   includeThumbnail,
                                            const bool&            cancelFlag
                                           )
{
  if  (!group)
    return NULL;

  KKStr  selectStr = "call  ImagesQueryByImageGroup(" + 
                     group->Name ().QuotedStr () + ", " +
                     (includeThumbnail ? "true":"false") +
                     ")";

  int  returnCd = QueryStatement (selectStr);
  if  (returnCd != 0)
    return NULL;

  DataBaseImageListPtr results = ImageQueryProcessResults ();

  ResultSetsClear ();

  return  results;
}  /* ImageQuery */
    





DataBaseImageListPtr  DataBase::ImageQuery (DataBaseImageGroupPtr  imageGroup,
                                            const KKStr&           scannerFileName,
                                            ImageClassPtr          imageClass,
                                            char                   classKeyToUse,
                                            float                  probMin,
                                            float                  probMax,
                                            int                    sizeMin,
                                            int                    sizeMax,
                                            float                  depthMin,
                                            float                  depthMax,
                                            uint                   restartImageId,
                                            int                    limit,            // Max # of rows 2 return.  -1 idicates no limit.
                                            bool                   includeThumbnail,
                                            const bool&            cancelFlag
                                           )
{
  KKStr  selectStr(512);

  KKStr  emptyStr = "\"\"";

  KKStr  groupNameStr = ((imageGroup == NULL) ? "" : imageGroup->Name ());
  KKStr  classNameStr = ((imageClass == NULL) ? "" : imageClass->Name ());
  KKStr  classKeyToUseStr = "";
  classKeyToUseStr.Append (classKeyToUse);


  selectStr << "call  ImagesQuery("  
            << groupNameStr.QuotedStr     ()  << ", "
            << scannerFileName.QuotedStr   ()  << ", " 
            << classNameStr.QuotedStr     ()  << ", "
            << classKeyToUseStr.QuotedStr ()  << ", "
            << probMin  << ", " << probMax    << ", "
            << sizeMin  << ", " << sizeMax    << ", "
            << depthMin << ", " << depthMax   << ", "
            << restartImageId                 << ", "
            << limit                          << ", "  
            << (includeThumbnail ? "true" : "false")
            << ")";
   
  int  returnCd = QueryStatement (selectStr);
  if  (returnCd != 0)
    return NULL;

  DataBaseImageListPtr results = ImageQueryProcessResults ();

  ResultSetsClear ();

  return  results;
}  /* ImageQuery */






DataBaseImageListPtr  DataBase::ImageQuery (DataBaseImageGroupPtr  imageGroup,
                                            const KKStr&           cruiseName,
                                            const KKStr&           stationName,
                                            const KKStr&           deploymentNum,
                                            ImageClassPtr          imageClass,
                                            char                   classKeyToUse,
                                            float                  probMin,
                                            float                  probMax,
                                            int                    sizeMin,
                                            int                    sizeMax,
                                            float                  depthMin,
                                            float                  depthMax,
                                            uint                   restartImageId,
                                            int                    limit,            // Max # of rows to return.  -1 indicates no limit.,
                                            bool                   includeThumbnail,
                                            const bool&            cancelFlag
                                           )
{
  KKStr  selectStr (4096);
  VectorKKStr*  scannerFileEntries = ScannerFileEntriesGetList (cruiseName, stationName, deploymentNum);

  if  (!scannerFileEntries)
    return NULL;

  DataBaseImageListPtr  images = new DataBaseImageList (true);

  VectorKKStr::iterator  idx;
  for  (idx = scannerFileEntries->begin ();  (idx != scannerFileEntries->end ())  &&  (!cancelFlag);  idx++)
  {
    DataBaseImageListPtr  imagesForOneScannerFile = ImageQuery (imageGroup,
                                                                *idx,           imageClass,  classKeyToUse,
                                                                probMin,        probMax, 
                                                                sizeMin,        sizeMax, 
                                                                depthMin,       depthMax, 
                                                                restartImageId, limit,
                                                                includeThumbnail,        //  true = include thumbNail
                                                                cancelFlag
                                                               );
    if  (imagesForOneScannerFile)
    {
      imagesForOneScannerFile->Owner (false);
      images->AddQueue (*imagesForOneScannerFile);
      delete  imagesForOneScannerFile;
      imagesForOneScannerFile = NULL;
    }
  }

  if  (cancelFlag)
  {
    delete  images;
    images = NULL;
  }

  delete  scannerFileEntries;

  return  images;
}  /* ImageQuery */



DataBaseImageListPtr  DataBase::ImageQueryByGrouop 
                                  (DataBaseImageGroupPtr  imageGroup,
                                   const KKStr&           cruiseName,
                                   const KKStr&           stationName,
                                   const KKStr&           deploymentNum,
                                   const KKStr&           scannerFileName,
                                   ImageClassPtr          imageClass,
                                   char                   classKeyToUse,
                                   float                  probMin,
                                   float                  probMax,
                                   int                    sizeMin,
                                   int                    sizeMax,
                                   float                  depthMin,
                                   float                  depthMax,
                                   uint                   restartImageId,
                                   int                    limit,            // Max # of rows to return.  -1 indicates no limit.,
                                   bool                   includeThumbnail
                                  )
{
  KKStr  sqlStr(512);

  KKStr  imageGroupName (32);
  if  (imageGroup)  imageGroupName = imageGroup->Name ();

  KKStr  className (32);
  if  (imageClass)  className = imageClass->Name ();

  KKStr  classKeyToUseStr = "";
  classKeyToUseStr.Append (classKeyToUse);

  sqlStr    << "call  ImagesQueryByGrouop("  
            << imageGroupName.QuotedStr   ()         << ", "
            << cruiseName.QuotedStr       ()         << ", "
            << stationName.QuotedStr      ()         << ", "
            << deploymentNum.QuotedStr    ()         << ", "
            << scannerFileName.QuotedStr   ()         << ", "
            << className.QuotedStr        ()         << ", "
            << classKeyToUseStr.QuotedStr ()         << ", "
            << probMin  << ", " << probMax           << ", "
            << sizeMin  << ", " << sizeMax           << ", "
            << depthMin << ", " << depthMax          << ", "
            << restartImageId                        << ", "
            << limit                                 << ", "  
            << (includeThumbnail ? "true" : "false")
            << ")";
   
  int  returnCd = QueryStatement (sqlStr);
  if  (returnCd != 0)
    return NULL;

  DataBaseImageListPtr results = ImageQueryProcessResults ();

  ResultSetsClear ();

  return  results;
}  /* ImageQuery */









DataBaseImageListPtr  DataBase::ImageQueryForScanLineRange (const KKStr&   scannerFileName,
                                                            uint           scanLineStart,
                                                            uint           scanLineEnd
                                                           )
{
  KKStr selectStr = "call ImagesQueryForScanLineRange(" + scannerFileName.QuotedStr ()            + ", "
                                                        + StrFormatInt (scanLineStart, "ZZZZZ0") + ", "
                                                        + StrFormatInt (scanLineStart, "ZZZZZ0") +
                                                      ")";

  int  returnCd = QueryStatement (selectStr);
  if  (returnCd != 0)
    return NULL;

  DataBaseImageListPtr  results = ImageQueryProcessResults ();
  ResultSetsClear ();
  return  results;
}  /* ImageQueryForScanLineRange */




VectorKKStr*   DataBase::ImageListOfImageFileNamesByScanLineRange (const KKStr&   scannerFileName,
                                                                   uint           scanLineStart,
                                                                   uint           scanLineEnd
                                                                  )
{
  KKStr selectStr = "call ImagesImageFileNamesByScanLineRange(" + scannerFileName.QuotedStr ()            + ", "
                                                                + StrFormatInt (scanLineStart, "ZZZZZ0") + ", "
                                                                + StrFormatInt (scanLineStart, "ZZZZZ0") +
                                                             ")";

  int  returnCd = QueryStatement (selectStr);
  if  (returnCd != 0)
    return NULL;

  char*  fieldNames[] = {"ImageId", "ImageFileName", NULL};
  ResultSetLoad (fieldNames);
  if  (!resultSetMore)
    return  NULL;

  VectorKKStr*  results = new VectorKKStr ();

  MYSQL_ROW  row = NULL;
  while  (ResultSetFetchNextRow ())
    results->push_back (ResultSetGetField (1));

  ResulSetFree ();

  ResultSetsClear ();

  return  results;
}  /* ImageListOfImageFileNamesByScanLineRange */





void  DataBase::ImageUpdatePredictions (const KKStr&   imageFileName,
                                        ImageClassPtr  class1Pred,
                                        float          class1Prob,
                                        ImageClassPtr  class2Pred,
                                        float          class2Prob
                                       )
{
  KKStr  updateStr (512);
  if  ((class1Pred == NULL)  &&  (class2Pred == NULL))
    return;

  KKStr  emptyStr = "";
  KKStr  c1Name = ((class1Pred) ?  class1Pred->Name () : emptyStr);
  KKStr  c2Name = ((class2Pred) ?  class2Pred->Name () : emptyStr);

  updateStr << "call ImagesUpdatePredictions("  << imageFileName.QuotedStr () << ", "
                                                << c1Name.QuotedStr ()        << ", "
                                                << class1Prob                 << ", "
                                                << c2Name.QuotedStr ()        << ", "
                                                << class2Prob
                                                << ")";
  int  returnCd = QueryStatement (updateStr);

  ResultSetsClear ();

}  /* ImageUpdatePredictions */






void  DataBase::ImageUpdateValidatedClass (const KKStr&   imageFileName, 
                                           ImageClassPtr  imageClass
                                          )
{
  if  (!imageClass)
    return;

  KKStr  updateStr (512);

  updateStr << "call  ImagesUpdateValidatedClass(" << imageFileName.QuotedStr ()       << ", " 
            <<                                        imageClass->Name ().QuotedStr () << ", "
            <<                                        "1.0"                                                   
            <<                                 ")";
  int  returnCd = QueryStatement (updateStr);
  ResultSetsClear ();
}  /* ImageUpdateValidatedClass */




void  DataBase::ImageUpdate (DataBaseImage&  dbImage,
                             Raster&         image
                            )
{
  KKStr  sqlStr (3000);
  KKStr  class1Name = "";
  KKStr  class2Name = "";
  KKStr  validatedClassName = "";

  class1Name = dbImage.Class1Name ();
  class2Name = dbImage.Class2Name ();
  validatedClassName = dbImage.ValidatedClassName ();

  uint           imageCompressedBuffLen      = 0;
  uchar*         mySqlImageCompressedBuff    = NULL;
  unsigned long  mySqlImageCompressedBuffLen = 0;

  uchar* imageCompressedBuff = EncodeARasterImageIntoAThumbNail (image, 100, imageCompressedBuffLen);
  if  (imageCompressedBuff == NULL)
  {
    mySqlImageCompressedBuff = NULL;
    mySqlImageCompressedBuffLen = 0;
  }
  else
  {
    // In a worst case situation the esape string could be twoce as long as the source string plus 5 bytes overhead.
    mySqlImageCompressedBuff = new uchar[imageCompressedBuffLen * 2 + 5];
    mySqlImageCompressedBuffLen = mysql_real_escape_string (conn, (char*)mySqlImageCompressedBuff, (char*)imageCompressedBuff, imageCompressedBuffLen);
  }

  sqlStr << "call ImagesUpdate("
         << dbImage.ImageId        ()               << ", "
         << dbImage.ImageFileName  ().QuotedStr ()  << ", "
         << dbImage.ScannerFileName ().QuotedStr ()  << ", "
         << dbImage.ByteOffset     ()               << ", "
         << dbImage.TopLeftRow     ()               << ", "
         << dbImage.TopLeftCol     ()               << ", "
         << dbImage.Height         ()               << ", "
         << dbImage.Width          ()               << ", "
         << dbImage.PixelCount     ()               << ", "
         << dbImage.CentroidRow    ()               << ", "
         << dbImage.CentroidCol    ()               << ", "
         << class1Name.QuotedStr   ()               << ", "
         << dbImage.Class1Prob     ()               << ", "
         << class2Name.QuotedStr ()                 << ", "
         << dbImage.Class2Prob     ()               << ", "
         << validatedClassName.QuotedStr ()         << ", "
         << dbImage.Depth          ()               << ", ";

  if  (mySqlImageCompressedBuff == NULL)
  {
    sqlStr << "NULL";
  }
  else
  {
    sqlStr.Append ('"');
    sqlStr.Append ((char*)mySqlImageCompressedBuff, mySqlImageCompressedBuffLen);
    sqlStr.Append ('"');
  }

  sqlStr << ")";

  int  returnCd = QueryStatement (sqlStr);
  bool  successful = (returnCd == 0);
  delete  mySqlImageCompressedBuff;
  mySqlImageCompressedBuff = NULL;
  
  delete  imageCompressedBuff;
  imageCompressedBuff = NULL;

  if  (successful)
  {
    ResultSetsClear ();
    sqlStr = "delete  from  ImagesFullSize  where  ImageId = " + StrFormatInt (dbImage.ImageId (), "ZZZZZZZZ0");
    int  returnCd = QueryStatement (sqlStr);
    ResultSetsClear ();

    int  maxDim = Max (dbImage.Height (), dbImage.Width ());
    if  (maxDim > 100)
    {
      // The image had to be reduced to a thumbnail size so we want to store a copy of the original in the ImagesFullSize table.
      ImageFullSizeSave (dbImage.ImageFileName (), image);
    }
  }
}  /* ImageUpdate */




void  DataBase::ImageUpdateValidatedAndPredictClass (const KKStr&   imageFileName, 
                                                     ImageClassPtr  imageClass, 
                                                     float          class1Prob
                                                    )
{
  if  (!imageClass)
    return;

  KKStr  updateStr (512);
  updateStr << "call  ImagesUpdateValidatedClass(" << imageFileName.QuotedStr ()       << ", " 
            <<                                        imageClass->Name ().QuotedStr () << ", "
            <<                                        class1Prob                                                   
            <<                                 ")";
  int  returnCd = QueryStatement (updateStr);
  ResultSetsClear ();
}  /* ImageUpdateValidatedAndPredictClass*/








//*************************************************************************************
//*                             Image Full SizeRoutines                               *
//*************************************************************************************
RasterPtr  DataBase::ImageFullSizeLoad (const KKStr&  imageFileName)
{
  RasterPtr  fullSizeImage = NULL;

  char*  fieldNames[] = {"ImageId", "ImageFileName", "FullSizeImage", NULL};

  KKStr  selectStr = "call ImagesFullSizeLoad(" + osGetRootName (imageFileName).QuotedStr () + ")";
  int  returnCd = QueryStatement (selectStr);

  ResultSetLoad (fieldNames);

  if  (ResultSetFetchNextRow ())
    fullSizeImage = ResultSetGetRasterField (2);

  ResulSetFree ();
  ResultSetsClear ();

  return  fullSizeImage;

}  /* ImageFullSizeLoad */




void   DataBase::ImageFullSizeSave (const KKStr&    imageFileName,
                                    const Raster&   raster
                                   )
{
  uint compressedBuffLen = 0;
  uchar*  compressedBuff = raster.ToCompressor (compressedBuffLen);
  if  (!compressedBuff)
  {
    log.Level (-1) << endl << endl << "DataBase::ImageFullSizeSave   ***ERROR***    Compressing Raster  ImageFileName[" << imageFileName << "]" << endl << endl;
    return;
  }

  char*          queryStr  = NULL;
  int            queryStrLen = 0;
  unsigned long  mySqlImageCompressedBuffLen = 0;

  uint  queryStrSize = compressedBuffLen * 2 + 300;
  queryStr = new char[queryStrSize];
  if  (!queryStr)
  {
    log.Level (-1) << endl << endl << "DataBase::ImageFullSizeSave   ***ERROR***    ImageFileName[" << imageFileName << "]  Could not allocate 'QueryStr'.  Length[" << queryStrSize << "]" << endl << endl;
    delete  compressedBuff;
    compressedBuff = NULL;
    return;
  }

  char*  queryStrPtr = queryStr;
  strcpy (queryStrPtr, "call ImagesFullSizeSave(");
  queryStrPtr += strlen (queryStrPtr);

  strcpy (queryStrPtr, osGetRootName (imageFileName).QuotedStr ().Str ());
  queryStrPtr += strlen (queryStrPtr);

  strcpy (queryStrPtr, ", \"");
  queryStrPtr += strlen (queryStrPtr);
  queryStrLen = (int)strlen (queryStr); 

  mySqlImageCompressedBuffLen = mysql_real_escape_string (conn, (char*)queryStrPtr, (char*)compressedBuff, compressedBuffLen);
  queryStrLen += mySqlImageCompressedBuffLen;
  queryStrPtr += mySqlImageCompressedBuffLen;

  // We no longer need 'compressedBuff' so lets delete it.
  delete  compressedBuff;  compressedBuff = NULL;
  
  strcat (queryStrPtr, "\")");
  queryStrLen += (int)strlen (queryStrPtr);
  queryStrPtr += strlen (queryStrPtr);

  int  returnCd = QueryStatement (queryStr, queryStrLen);
  if  (returnCd == 0)
  {
    int  imageId = -1;
    ResultSetLoad (NULL);
    if  (resultSetMore)
    {
      if  (ResultSetFetchNextRow ())
        imageId = ResultSetGetIntField (0);
      ResulSetFree ();
    }
    ResultSetsClear ();
  }

  delete  queryStr; 
  queryStr = NULL;

  delete  compressedBuff;
  compressedBuff = NULL;
}  /* ImageFullSizeSave*/






RasterPtr  DataBase::ImageFullSizeFind (const KKStr&  imageFileName)
{
  DataBaseImagePtr i = this->ImageLoad (imageFileName);
  if  (!i)
  {
    // Without this entry there is no way to locate the image.
    return NULL;
  }

  RasterPtr  r = NULL;
  uint  maxDim = Max (i->Height (), i->Width ());
  if  (maxDim <= 100)
    r = i->ThumbNail ();

  if  (!r)
  {
    // Will now try the ImagesFullSize table
    r = ImageFullSizeLoad (imageFileName);
  }

  return  r;
}  /* ImageFullSizeFind */




//*************************************************************************************
//*                             Image Group Routines                                  *
//*************************************************************************************
void  DataBase::ImageGroupInsert (DataBaseImageGroup&  imageGroup)
{
  KKStr  insertStr (256);

  insertStr << "CALL ImageGroupInsert(" 
            <<                         imageGroup.Name       ().QuotedStr () << ", "
            <<                         imageGroup.Description().QuotedStr ()
            << ")";
    
  int  returnCd = QueryStatement (insertStr.Str ());
  if  (returnCd != 0)
    return;

  ResultSetLoad (NULL);
  if  (!resultSetMore)
    return;

  if  (resultSetNumFields < 1)
  {
    lastMySqlErrorDesc = "ImageGroupId was not returnd.";
    lastMySqlErrorNo   = -1;
  }
  else
  {
    while  (ResultSetFetchNextRow ())
      imageGroup.ImageGroupId (ResultSetGetIntField (0));
  }

  ResulSetFree ();
  ResultSetsClear ();

  return;
}  /* ImageGroupInsert */




DataBaseImageGroupPtr  DataBase::ImageGroupLoad (const KKStr&  imageGroupName)   // returns a list of all Group Assignments
{
  DataBaseImageGroupPtr  group = NULL;

  KKStr  selectCmd = "call ImageGroupLoad(" + imageGroupName.QuotedStr () + ")";
  int  returnCd = QueryStatement (selectCmd);
  if  (returnCd != 0)
    return  NULL;

  char*  fieldNames[] = {"ImageGroupId", "ImageGroupName", "Description", "GroupCount", NULL};
  ResultSetLoad (fieldNames);
  if  (!resultSetMore)
    return  NULL;

  int    groupId = 0;
  KKStr  name    = "";
  KKStr  desc    = "";
  int    count   = 0;
  while  (ResultSetFetchNextRow ())
  {
    if  (group)
    {
      delete  group;  group = NULL;
    }
    groupId = ResultSetGetIntField (0);
    name    = ResultSetGetField    (1);
    desc    = ResultSetGetField    (2);
    count   = ResultSetGetIntField (3);
    group = new DataBaseImageGroup (groupId, name, desc, count);
  }

  ResulSetFree ();
  ResultSetsClear ();

  return  group;
}  /* ImageGroupLoad */






DataBaseImageGroupListPtr  DataBase::ImageGroupLoad ()   // returns a list of all Group Assignments
{
  DataBaseImageGroupListPtr  groups = NULL;

  KKStr  selectCmd = "call ImageGroupLoadAll()";
  int  returnCd = QueryStatement (selectCmd);
  if  (returnCd != 0)
    return  groups;

  ResultSetLoad (NULL);
  if  (!resultSetMore)
    return  NULL;

  if  (resultSetNumFields < 4)
  {
    lastMySqlErrorDesc = mysql_error (conn);
    lastMySqlErrorNo   = mysql_errno (conn);

    log.Level (-1) << endl << endl 
                   << "DataBase::ImageGroupLoad    ***ERROR***     Not enough fields returned." << endl
                   << endl
                   << "ErrorDesc[" << lastMySqlErrorDesc << "]"  << endl
                   << endl;
    return  NULL;
  }

  groups = new DataBaseImageGroupList (true);

  while  (ResultSetFetchNextRow ())
  {
    int    groupId = ResultSetGetIntField (0);
    KKStr  name    = ResultSetGetField (1);
    KKStr  desc    = ResultSetGetField (2);
    uint   count   = (uint)ResultSetGetUlongField (3);
    groups->PushOnBack (new DataBaseImageGroup (groupId, name, desc, count));
  }

  ResulSetFree ();
  ResultSetsClear  ();
  return  groups;
}  /* ImageGroupLoad */




void  DataBase::ImageGroupDelete (int imageGroupId)
{
  KKStr  deleteStr (1000);
  deleteStr << "CALL ImageGroupDelete(" << imageGroupId << ")";
    
  int  returnCd = QueryStatement (deleteStr.Str ());
  if  (returnCd != 0)
    return;

  ResultSetsClear  ();

  return;
}  /* ImageGroupDelete */




//*************************************************************************************
//*                               ImageGroupEntries                                   *
//*************************************************************************************

void  DataBase::ImageGroupEntriesInsert (const DataBaseImageGroupEntry&  imageGroupEntry)
{
  KKStr  sqlStr (256);
  sqlStr << "call ImageGroupEntryInsert(" <<  imageGroupEntry.ImageGroupId () << ", " << imageGroupEntry.ImageFileName ().QuotedStr () << ")";
  int  returnCd = QueryStatement (sqlStr.Str ());

  return;
}  /* ImageGroupEntriesInsert */





void  DataBase::ImageGroupEntriesInsert (const DataBaseImageGroupEntryList&  imageGroupList)
{
  DataBaseImageGroupEntryList::const_iterator idx;
  for  (idx = imageGroupList.begin ();   idx != imageGroupList.end ();  idx++)
  {
    DataBaseImageGroupEntryPtr ie = *idx;
    ImageGroupEntriesInsert (*ie);
  }
}  /* ImageGroupEntriesInsert */





KKStrMatrixPtr  DataBase::ImageGroupEntriesInsert (int                 groupId,
                                                   const VectorKKStr&  imageFileNames
                                                  )
{
  int  x = 0;
  int  maxParmLen = 60000;  // The stored procedure parameter is limited to 60,000 characters.
  int  sqlStrLen = Min (maxParmLen, (int)(49 * imageFileNames.size ()));  
  sqlStrLen = Max (sqlStrLen, 100);
  
  KKStr  sqlStr (sqlStrLen);

  sqlStr = "";
  sqlStr << "call  ImageGroupEntryInsertList(" << groupId << ", \"";

  uint  zed = 0;

  while  (zed < imageFileNames.size ())
  {
    if  (zed > 0)
      sqlStr << "\t";
    sqlStr << imageFileNames[zed];
    zed++;
  }
  sqlStr << "\")";

  if  (zed < imageFileNames.size ())
  {
    log.Level (-1) << endl
                   << "DataBase::ImageGroupEntriesInsert    ***ERROR***" << endl
                   << endl
                   << "      Parameter for ImageFileNames was to large." << endl
                   << endl;
    return  NULL;
  }

  KKStrMatrixPtr results = new KKStrMatrix (2);  // 2 Columns

  int  returnCd = QueryStatement (sqlStr);
  if  (returnCd != 0)
  {
    // Since we failed to process anything we will return 'NULL' indicating total failure.
    delete  results;
    return  NULL;
  }
  
  // Process results of procedure call.
  char*  fieldNames[] = {"ImageId", "ImageFileName", "Successful", "ErrorDesc", NULL};
  ResultSetLoad (fieldNames);
  while  (ResultSetFetchNextRow ())
  {
    bool  successful    = ResultSetGetBool (2);
    if  (!successful)
    {
      KKStrPtr  imageFileName = new KKStr (ResultSetGetKKStrField (1));
      KKStrPtr  errorDesc     = new KKStr (ResultSetGetKKStrField (3));
      KKStrListPtr  row = new KKStrList (true, 2);
      row->push_back (imageFileName);
      row->push_back (errorDesc);
      results->AddRow (row);
    }
  }

  ResulSetFree ();
  ResultSetsClear  ();

  return  results;
}  /* ImageGroupEntriesInsert */





DataBaseImageGroupEntryListPtr  DataBase::ImageGroupEntriesLoad (int groupId)
{
  KKStr  selectCmd (200);

  selectCmd << "select  ImageFileName  from ImageGroupEntries  where  ImageGroupId = " << groupId << endl;

  int  returnCd = QueryStatement (selectCmd);
  if  (returnCd != 0)
    return  NULL;

  MYSQL_RES*  resSet;
  resSet = mysql_use_result (conn); /* generate result set */
  if  (resSet == NULL)
  {
    lastMySqlErrorDesc = mysql_error (conn);
    lastMySqlErrorNo   = mysql_errno (conn);

    log.Level (-1) << endl << endl 
                   << "DataBase::ImageGroupEntriesLoad    ***ERROR***     occured when retrieving result set." << endl
                   << endl
                   << "ErrorDesc[" << lastMySqlErrorDesc << "]"  << endl
                   << endl;
    return  NULL;
  }

  DataBaseImageGroupEntryListPtr  entries = new DataBaseImageGroupEntryList (true);
  int  numFields  = mysql_num_fields (resSet);
  if  (numFields < 1)
  {
    lastMySqlErrorDesc = mysql_error (conn);
    lastMySqlErrorNo   = mysql_errno (conn);

    log.Level (-1) << endl << endl 
                   << "DataBase::ImageGroupEntriesLoad    ***ERROR***     Not enough cllumns ." << endl
                   << endl
                   << "ErrorDesc[" << lastMySqlErrorDesc << "]"  << endl
                   << endl;
    return  NULL;
  }

  MYSQL_ROW  row;
  while  (row = mysql_fetch_row (resSet))
  {
    KKStr  imageFileName = row[0];
    DataBaseImageGroupEntryPtr  entry = new DataBaseImageGroupEntry (groupId, imageFileName);
    entries->PushOnBack (entry);
  }

  mysql_free_result (resSet);

  return  entries;
}  /* ImageGroupEntriesLoad */



/********************************************************************************************/
/*                                ScannerFileEntry Routines                                 */
/********************************************************************************************/
char*  DataBase::scannerFileEntryFieldNames[] = 
                       {"ScannerFileId",   "ScannerFileName", "CruiseName",     "StationName",    
                        "DeploymentNum",   "Description",     "DateTimeStart",  "SizeInBytes",
                        "NumScanLines",    "ScanRate",        "Depth",          NULL
                       };


ScannerFileEntryListPtr  DataBase::ScannerFileEntryProcessResults ()
{
  ResultSetLoad (scannerFileEntryFieldNames);
  if  (!resultSetMore)
    return NULL;

  ScannerFileEntryListPtr  results = new ScannerFileEntryList (true);
  while  (ResultSetFetchNextRow ())
  {
    ScannerFileEntryPtr  sf = new ScannerFileEntry (ResultSetGetField (1));
    sf->ScannerFileId   (ResultSetGetIntField (0));
    sf->CruiseName     (ResultSetGetField (2));
    sf->StationName    (ResultSetGetField (3));
    sf->DeploymentNum  (ResultSetGetField (4));
    sf->Description    (ResultSetGetField (5));
    sf->DateTimeStart  (DateTimeFromMySqlDateTimeField (ResultSetGetField (8))); 
    sf->SizeInBytes    (ResultSetGetIntField    (17));
    sf->NumScanLines   (ResultSetGetIntField    (18));
    sf->ScanRate       (ResultSetGetFloatField  (19));
    sf->Depth          (ResultSetGetFloatField  (20));
    results->PushOnBack (sf);
  }

  ResulSetFree ();
  return  results;
}  /* ScannerFileEntryProcessResults */






ScannerFileEntryPtr  DataBase::ScannerFileEntryLoad (const KKStr&  _scannerFileName)
{
  KKStr scannerFileName = osGetRootName (_scannerFileName);

  KKStr  queryStr = "select *  from  ScannerFileEntries  " 
                    "where ScannerFileName = " + scannerFileName.QuotedStr ();

  int  returnCd = QueryStatement (queryStr);
  if  (returnCd != 0)
    return  NULL;

  ScannerFileEntryListPtr  results = ScannerFileEntryProcessResults ();
  if  (results == NULL)
    return NULL;

  ScannerFileEntryPtr  result = results->PopFromBack ();
  delete  results;
  results = NULL;
  ResultSetsClear ();
  return  result;
}  /* SiperFileRecLoad */





VectorKKStr*  DataBase::ScannerFileEntriesGetList (const KKStr& cruiseName,
                                                   const KKStr& stationName,
                                                   const KKStr& deploymentNum
                                                  )
{
  KKStr  selectCmd (4096);

  selectCmd << "select  sf.ScannerFileName  from  ScannerFileEntries  sf ";

  VectorKKStr  conditions;

  if  (!cruiseName.Empty ())
    conditions.push_back ("CruiseName = " + cruiseName.QuotedStr ());

  if  (!stationName.Empty ())
    conditions.push_back ("StationName = " + stationName.QuotedStr ());

  if  (!deploymentNum.Empty ())
    conditions.push_back ("DeploymentNum = " + deploymentNum.QuotedStr ());

  if  (conditions.size () > 0)
  {
    selectCmd << "  where  ";
    for  (uint x = 0;  x < conditions.size ();  x++)
    {
      if  (x > 0)
        selectCmd << "  and  ";
      selectCmd << conditions[x];
    }
  }

  selectCmd << "    order by CruiseName, StationName, DeploymentNum, DateTimeStart";

  int  returnCd = QueryStatement (selectCmd);
  if  (returnCd != 0)
    return  NULL;


  MYSQL_RES*  resSet;
  resSet = mysql_use_result (conn);
  if  (resSet == NULL)
  {
    lastMySqlErrorDesc = mysql_error (conn);
    lastMySqlErrorNo   = mysql_errno (conn);

    log.Level (-1) << endl << endl 
                   << "DataBase::ScannerFileEntriesGetList    ***ERROR***     occured when retrieving result set." << endl
                   << endl
                   << "ErrorDesc[" << lastMySqlErrorDesc << "]"  << endl
                   << endl;
    return  NULL;
  }


  VectorKKStr*  scannerFileEntries = new VectorKKStr ();

  MYSQL_ROW  row;
  while  (row = mysql_fetch_row (resSet))
  {
    KKStr  scannerFileName (row[0]);
    scannerFileEntries->push_back (scannerFileName);
  }

  mysql_free_result (resSet);

  return  scannerFileEntries;
}  /* ScannerFileEntriesGetList */





ScannerFileEntryListPtr  DataBase::ScannerFileEntryLoad (const KKStr& cruiseName,
                                                         const KKStr& stationName,
                                                         const KKStr& deploymentNum
                                                        )
{
  KKStr  selectCmd (200);

  selectCmd << "select * from  ScannerFileEntries  "  << endl;

  if  (!cruiseName.Empty ())
  {
    selectCmd << "  where  CruiseName = " << cruiseName.QuotedStr ();
    if  (!stationName.Empty ())
    {
      selectCmd  << "  and  StationName = " << stationName.QuotedStr ();
      if  (!deploymentNum.Empty ())
      {
        selectCmd  << "  and  DeploymentNum = " << deploymentNum.QuotedStr ();
      }
    }
  }

  selectCmd << "  order by CruiseName, StationName, DeploymentNum, DateTimeStart";

  int  returnCd = QueryStatement (selectCmd);
  if  (returnCd != 0)
    return  NULL;

  return  ScannerFileEntryProcessResults ();
}  /* ScannerFileEntryLoad */






void  DataBase::ScannerFileEntryInsert (ScannerFileEntry&  scannerFileEntry)
{
  KKStr  insertStr (1000);
  insertStr <<  "insert into  ScannerFileEntries ("
                                      "ScannerFileName, CruiseName, StationName, DeploymentNum, Description"      
                                      "DateTimeStart, SizeInBytes, NumScanLines, Scanrate, Depth"
                                    ")" << endl
            <<      "values(" 
            <<         scannerFileEntry.ScannerFileName ().QuotedStr ()          << ", "
            <<         scannerFileEntry.CruiseName      ().QuotedStr ()          << ", "
            <<         scannerFileEntry.StationName     ().QuotedStr ()          << ", "
            <<         scannerFileEntry.DeploymentNum   ().QuotedStr ()          << ", "
            <<         scannerFileEntry.Description     ().QuotedStr ()          << ", "
            <<         DateTimeToQuotedStr (scannerFileEntry.DateTimeStart  ())  << ", "
            <<         scannerFileEntry.SizeInBytes    ()                        << ", "
            <<         scannerFileEntry.NumScanLines   ()                        << ", "
            <<         scannerFileEntry.ScanRate       ()                        << ", "
            <<         scannerFileEntry.Depth          ()
            <<       ")";

  int  returnCd = QueryStatement (insertStr.Str ());
  if  (returnCd == 0)
     scannerFileEntry.ScannerFileId ((int)mysql_insert_id (conn));

  return;
}  /* ScannerFileEntryInsert */



void   DataBase::ScannerFileEntryUpdate (ScannerFileEntry&   scannerFileEntry)
{
  KKStr  updateStr(1024);
  updateStr << "update  ScannerFileEntries  sf"  << endl
            << "  set  CruiseName = "     << scannerFileEntry.CruiseName    ().QuotedStr ()          << ", " << endl
            << "       StationName = "    << scannerFileEntry.StationName   ().QuotedStr ()          << ", " << endl
            << "       DeploymentNum = "  << scannerFileEntry.DeploymentNum ().QuotedStr ()          << ", " << endl
            << "       Description = "    << scannerFileEntry.Description   ().QuotedStr ()          << ", " << endl
            << "       DateTimeStart = "  << DateTimeToQuotedStr (scannerFileEntry.DateTimeStart ()) << ", " << endl
            << "       SizeInBytes = "    << scannerFileEntry.SizeInBytes   ()                       << ", " << endl
            << "       NumScanLines = "   << scannerFileEntry.NumScanLines  ()                       << ", " << endl
            << "       ScanRate = "       << scannerFileEntry.ScanRate      ()                       << ", " << endl
            << "       Depth = "          << scannerFileEntry.Depth         ()                               << endl
            << "  where  sf.ScannerFileName = " << scannerFileEntry.ScannerFileName ().QuotedStr () << endl;

  int  returnCd = QueryStatement (updateStr.Str ());
}  /* ScannerFileEntryUpdate */





void   DataBase::ScannerFileEntryUpdateFileSizeStats (const KKStr&  _scannerFileName, 
                                                long long     _sizeInBytes, 
                                                uint          _numScanLines
                                               )
{
  KKStr  updateStr (512);
  
  updateStr << "update  ScannerFileEntries  set SizeInBytes = " << _sizeInBytes << ", NumScanLines = " << _numScanLines << endl 
            << "        where ScannerFileName = " << _scannerFileName.QuotedStr ();

  int  returnCd = QueryStatement (updateStr.Str ());
}  /* ScannerFileEntryUpdateFileSizeStats */




/**
 *@brief  Delete the contents of a single Scanner File Entry from the database from all tables such as Images, FeatureData, etc.
 *@param[in]  _scannerFileName  Name of Scanner file to delete.
 */
void   DataBase::ScannerFileEntryDelete (const KKStr&  _scannerFileName)
{
  KKStr  updateStr (512);
  updateStr << "call  ScannerFileEntryDelete (" << _scannerFileName.QuotedStr () << ")";
  int  returnCd = QueryStatement (updateStr.Str ());
}





