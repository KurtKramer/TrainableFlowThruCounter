// *****************************************************************************
//
//     Copyright (c) 2010, Pleora Technologies Inc., All rights reserved.
//
// *****************************************************************************

#include "stdafx.h"

#include "logbuffer.h"

#ifdef WIN32
#include <shlobj.h>
#endif


#define LOGBUFFER_VERSION ( "1.0.0.0" )

#define TAG_VERSION ( "logbufferversion" )
#define TAG_GENICAMENABLED ( "loggenicamenabled" )
#define TAG_BUFFERALLENABLED ( "logbufferallenabled" )
#define TAG_BUFFERERRORENABLED ( "logbuffererrorenabled" )
#define TAG_LOGFILENAME ( "logfilename" )
#define TAG_WRITETOFILEENABLED ( "logwritetofileenabled" )

#define VAL_TRUE ( "true" )
#define VAL_FALSE ( "false" )


#define MAX_LOG_SIZE ( 256 * 1024 )


// =============================================================================
LogBuffer::LogBuffer()
	: mGenICamEnabled( true )
	, mBufferAllEnabled( false )
	, mBufferErrorEnabled( true )
	, mWriteToFileEnabled( false )
{
    ResetConfig();
    Reset();
}


// =============================================================================
LogBuffer::~LogBuffer()
{
}


// =============================================================================
void LogBuffer::ResetConfig()
{
	mGenICamEnabled = true;
	mBufferAllEnabled = false;
	mBufferErrorEnabled = true;
	mWriteToFileEnabled = false;

#ifdef WIN32
    wchar_t lDesktop[ MAX_PATH ];
	SHGetSpecialFolderPath( NULL, lDesktop, CSIDL_DESKTOP, true );

    mFilename = CString( lDesktop ) + _T( "\\EventMonitor.txt" );
#endif //WIN32

#ifdef _LINUX_
    //TODO: Find API call to get the proper default folder
    mFilename = "EventMonitor.txt";
#endif //_LINUX_
}


// =============================================================================
void LogBuffer::Reset()
{
	mMutex.Lock();
	//////////////////////////////////

    mStartTimestamp = ::GetTickCount();
	mBuffer = "";

    //////////////////////////////////
    mMutex.Unlock();
}


// =============================================================================
void LogBuffer::Log( const CString &aItem )
{
    DWORD lElapsed = ::GetTickCount() - mStartTimestamp;
    
    CString lLast = _T( "0" );
	if ( lElapsed >= 0 )
	{
		lLast.Format( _T( "%04i.%03i" ),
			lElapsed / 1000, lElapsed % 1000 );
	}

	CString lStr = lLast + _T( "    " ) + aItem;

	mMutex.Lock();
	//////////////////////////////////

    FILE *lFile = NULL;
	if ( mWriteToFileEnabled )
	{
		// Open file
		_wfopen_s( &lFile, (LPCTSTR)mFilename, _T( "at" ) );
	}

	if ( mBuffer.GetLength() < MAX_LOG_SIZE )
	{
		mBuffer += lStr + _T( "\r\n" );
	}

	if ( lFile != NULL )
	{
		fwprintf( lFile, _T( "%s\n" ), (LPCTSTR)lStr );
        fclose( lFile );
	}

	//////////////////////////////////
	mMutex.Unlock();
}


// =============================================================================
CString LogBuffer::Empty()
{
	mMutex.Lock();
	//////////////////////////////////

	CString lTemp = mBuffer;
	mBuffer = "";

	//////////////////////////////////
	mMutex.Unlock();

	return lTemp;
}


// =============================================================================
void LogBuffer::SetFilename( const CString &aFilename ) 
{ 
	mMutex.Lock();
	//////////////////////////////////

	mFilename = aFilename; 

	//////////////////////////////////
	mMutex.Unlock();
}


// =============================================================================
void LogBuffer::SetWriteToFileEnabled( bool aEnabled ) 
{ 
	mMutex.Lock();
	//////////////////////////////////

	mWriteToFileEnabled = aEnabled; 

	//////////////////////////////////
	mMutex.Unlock();
}


// =============================================================================
void LogBuffer::Save( PvConfigurationWriter &aWriter )
{
    // Save a version string, just in case we need it in the future
    aWriter.Store( LOGBUFFER_VERSION, TAG_VERSION );

	// bool mGenICamEnabled;
    aWriter.Store( mGenICamEnabled ? VAL_TRUE : VAL_FALSE, TAG_GENICAMENABLED );

	// bool mBufferAllEnabled;
    aWriter.Store( mBufferAllEnabled ? VAL_TRUE : VAL_FALSE, TAG_BUFFERALLENABLED );

	// bool mBufferErrorEnabled;
    aWriter.Store( mBufferErrorEnabled ? VAL_TRUE : VAL_FALSE, TAG_BUFFERERRORENABLED );

	// CString mFilename;
	aWriter.Store( (LPCTSTR)mFilename, TAG_LOGFILENAME );

	// bool mWriteToFileEnabled;
    aWriter.Store( mWriteToFileEnabled ? VAL_TRUE : VAL_FALSE, TAG_WRITETOFILEENABLED );
}


// =============================================================================
void LogBuffer::Load( PvConfigurationReader &aReader )
{
    PvResult lResult;
    PvString lPvStr;

	// Always load from a blank setup!
	ResetConfig();

	// bool mGenICamEnabled;
    lResult = aReader.Restore( TAG_GENICAMENABLED, lPvStr );
    if ( lResult.IsOK() )
    {
        mGenICamEnabled = ( lPvStr == VAL_TRUE );
    }

	// bool mBufferAllEnabled;
    lResult = aReader.Restore( TAG_BUFFERALLENABLED, lPvStr );
    if ( lResult.IsOK() )
    {
        mBufferAllEnabled = ( lPvStr == VAL_TRUE );
    }

	// bool mBufferErrorEnabled;
    lResult = aReader.Restore( TAG_BUFFERERRORENABLED, lPvStr );
    if ( lResult.IsOK() )
    {
        mBufferErrorEnabled = ( lPvStr == VAL_TRUE );
    }

	// CString mFilename;
    lResult = aReader.Restore( TAG_LOGFILENAME, lPvStr );
    if ( lResult.IsOK() )
	{
		SetFilename( lPvStr.GetUnicode() );
	}

	// bool mWriteToFileEnabled;
    lResult = aReader.Restore( TAG_WRITETOFILEENABLED, lPvStr );
    if ( lResult.IsOK() )
    {
        SetWriteToFileEnabled( lPvStr == VAL_TRUE );
    }
}


