// *****************************************************************************
//
//     Copyright (c) 2010, Pleora Technologies Inc., All rights reserved.
//
// *****************************************************************************

#pragma once

#include <PvConfigurationReader.h>
#include <PvConfigurationWriter.h>


class LogBuffer
{
public:

	LogBuffer();
	~LogBuffer();

	void Log( const CString &aItem );
	CString Empty();

	void Reset();

	void SetGenICamEnabled( bool aEnabled ) { mGenICamEnabled = aEnabled; }
	void SetBufferErrorEnabled( bool aEnabled ) { mBufferErrorEnabled = aEnabled; }
	void SetBufferAllEnabled( bool aEnabled ) { mBufferAllEnabled = aEnabled; }
	void SetFilename( const CString &aFilename );
	void SetWriteToFileEnabled( bool aEnabled );

	bool IsGenICamEnabled() const { return mGenICamEnabled; }
	bool IsBufferErrorEnabled() const { return mBufferErrorEnabled; }
	bool IsBufferAllEnabled() const { return mBufferAllEnabled; }
	const CString &GetFilename() const { return mFilename; }
	bool IsWriteToFileEnabled() const { return mWriteToFileEnabled; }

    // Persistence
    void Save( PvConfigurationWriter &aWriter );
    void Load( PvConfigurationReader &aReader );

protected:

	void ResetConfig();

private:

	CMutex mMutex;

	DWORD mStartTimestamp;
	CString mBuffer;

	bool mGenICamEnabled;
	bool mBufferErrorEnabled;
	bool mBufferAllEnabled;

	CString mFilename;
	bool mWriteToFileEnabled;
};

