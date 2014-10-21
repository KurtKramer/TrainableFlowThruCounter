// *****************************************************************************
//
//     Copyright (c) 2007, Pleora Technologies Inc., All rights reserved.
//
// *****************************************************************************

#include "stdafx.h"

#include "ThreadDisplay.h"
#include "GEVPlayerDlg.h"
#include "ImageSaveDlg.h"

#include <limits>
#include <assert.h>


#define GEN_INTERLACED ( "Interlaced" )
#define GEN_DEINTERLACING ( "Deinterlacing" )
#define GEN_INVERSION ( "DeinterlacingFieldInversion" )


// ==========================================================================
ThreadDisplay::ThreadDisplay( 
        GEVPlayerDlg *aDlg, PvDisplayWnd *aDisplayWnd, 
        PvDevice *aDevice, PvPipeline *aPipeline, 
        FilteringDlg *aFilteringDlg, ImageSaveDlg* aImageSaveDlg, 
        LogBuffer *aLogBuffer )
    : mEnabled( true )
    , mTargetFPS( 30 )
    , mVSync( false )
    , mMainDlg( aDlg )
    , mDisplayWnd( aDisplayWnd )
    , mDevice( aDevice )
    , mPipeline( aPipeline )
    , mLatestBuffer( NULL )
    , mLatestOddBuffer( NULL )
    , mLatestEvenBuffer( NULL )
    , mImageSaveDlg( aImageSaveDlg )
    , mFilteringDlg( aFilteringDlg )
    , mDeinterlacingEnabled( false )
    , mFinalBuffer( NULL )
    , mLogBuffer( aLogBuffer )
    , mLastBlockID( 0 )
    , mKeepPartialImages( false )
{
    mFilteringDlg->SetThreadDisplay( this );
    mImageSaveDlg->SetThreadDisplay( this );

    RegisterInvalidators();
    UpdateDeinterlacer(); 
    ResetStats();
}


// ==========================================================================
ThreadDisplay::~ThreadDisplay()
{
    mFilteringDlg->SetThreadDisplay( NULL );
    mImageSaveDlg->SetThreadDisplay( NULL );

    UnregisterInvalidators();
}


// ==========================================================================
DWORD ThreadDisplay::Function()
{
    ASSERT( mDisplayWnd != NULL );
    ASSERT( mMainDlg != NULL );

    for ( ;; )
    {
        // Check if we were signaled to terminate
        if ( IsStopping() )
        {
            CSingleLock lLock( &mBufferMutex );
            ReleaseLatestBuffers();
            break;
        }

        PvBuffer *lBuffer = NULL;

        // Try retrieving a buffer, using default timeout
        PvResult lResult = mPipeline->RetrieveNextBuffer( &lBuffer, -1 );

		// Perfrom logging on the just retrieved buffer
		Log( lBuffer );

        if ( lResult.IsOK() )
        {
            // Report image acquisition to PvDevice (for recovery management)
            mDevice->Reset ();

            if ( lBuffer->GetOperationResult().IsOK() || mKeepPartialImages )
            {
                ProcessBuffer( lBuffer );
            }
            else
            {
                mPipeline->ReleaseBuffer( lBuffer );

                // Report failure (timeout) to PvDevice (for recovery management)
                if ( ( mMainDlg->IsAcquiringImages() ) && 
                     ( lResult == PvResult::Code::TIMEOUT ) )
                {
                    mDevice->ReportStreamTimeout();
                }
            }
        }
    }

    return 0;
}


// ==========================================================================
PvBuffer* ThreadDisplay::RetrieveLatestBuffer()
{
    mBufferMutex.Lock();
    return mFinalBuffer;
}


// ==========================================================================
void ThreadDisplay::ReleaseLatestBuffer()
{
    mBufferMutex.Unlock();
}


// ==========================================================================
void ThreadDisplay::WhiteBalance( PvFilterRGB *aFilterRGB )
{
    if ( mFinalBuffer == NULL )
    {
        return;
    }

    CSingleLock lLock( &mBufferMutex );
    
    PvBuffer lBufferDisplay;
    lBufferDisplay.Alloc( mFinalBuffer->GetWidth(), mFinalBuffer->GetHeight(), PvPixelWinRGB32 );

    // Convert last good buffer to RGB, one-time use converter
    PvBufferConverter lConverter;
    lConverter.Convert( mFinalBuffer, &lBufferDisplay );

    aFilterRGB->WhiteBalance( &lBufferDisplay );

    Display( mFinalBuffer );
}


// ==========================================================================
// This method is called everytime the Interlaced, Deinterlacing or the
// DeinterlactingFieldInversion parameters are invalidated. 
//
// We simply lookup these parameters and refresh the de-interlacing 
// configuration.
//
// Right now we only subscribe to interlacing parameter invalidators. If
// this would change, we should look into only updating the de-interlacing
// configuraiton if the de-interlacing parameters are actually invalidated.
//
void ThreadDisplay::OnParameterUpdate( PvGenParameter *aParameter )
{
    UpdateDeinterlacer();
}


// ==========================================================================
void ThreadDisplay::UpdateDeinterlacer()
{
    // Get all related interlacing parameters
    PvGenBoolean *lInterlaced = dynamic_cast<PvGenBoolean *>( mDevice->GetGenParameters()->Get( GEN_INTERLACED ) );
    PvGenEnum *lDeinterlacing = dynamic_cast<PvGenEnum *>( mDevice->GetGenParameters()->Get( GEN_DEINTERLACING ) );
    PvGenBoolean *lInversion = dynamic_cast<PvGenBoolean *>( mDevice->GetGenParameters()->Get( GEN_INVERSION ) );

    // If the Interlaced and Deinterlacing features exist then
    mDeinterlacingEnabled = false;
    if ( ( lInterlaced != NULL ) && ( lDeinterlacing != NULL ) && ( lInversion != NULL ) )
    {
        // Read Interlaced value
        bool lInterlacedValue = false;
        PvResult lResult = lInterlaced->GetValue( lInterlacedValue );
        if ( !lResult.IsOK() )
        {
            return;
        }

        // Read Deinterlacing value
        PvString lDeinterlacingValue;
        lResult = lDeinterlacing->GetValue( lDeinterlacingValue );
        if ( !lResult.IsOK() )
        {
            return;
        
        }

        // If Interlaced = True and Deinterlacing = False then
        if ( lInterlacedValue && ( lDeinterlacingValue == "Off" ) )
        {
            // Perform de-interlacing... 
            mDeinterlacingEnabled = true;

            // Read DeinterlacingFieldInversionValue
            bool lInversionValue = false;
            lResult = lInversion->GetValue( lInversionValue );
            if ( lResult.IsOK() )
            {
                // ... according to the DeinterlacingFieldInversion feature
                if ( lInversionValue != mFilterDeinterlace.GetFieldInversion() )
                {
                    mFilterDeinterlace.SetFieldInversion( lInversionValue );
                }
            }
        }
    }
}


// ==========================================================================
void ThreadDisplay::RegisterInvalidators()
{
    // Register invalidators for interlacing support
    if ( mDevice != NULL )
    {
        PvGenParameter *lParam = mDevice->GetGenParameters()->Get( GEN_INTERLACED );
        if ( lParam != NULL )
        {
            lParam->RegisterEventSink( this );
        }

        lParam = mDevice->GetGenParameters()->Get( GEN_DEINTERLACING );
        if ( lParam != NULL )
        {
            lParam->RegisterEventSink( this );
        }

        lParam = mDevice->GetGenParameters()->Get( GEN_INVERSION );
        if ( lParam != NULL )
        {
            lParam->RegisterEventSink( this );
        }
    }
}


// ==========================================================================
void ThreadDisplay::UnregisterInvalidators()
{
    // Register invalidators for interlacing support
    if ( mDevice != NULL )
    {
        PvGenParameter *lParam = mDevice->GetGenParameters()->Get( GEN_INTERLACED );
        if ( lParam != NULL )
        {
            lParam->UnregisterEventSink( this );
        }

        lParam = mDevice->GetGenParameters()->Get( GEN_DEINTERLACING );
        if ( lParam != NULL )
        {
            lParam->UnregisterEventSink( this );
        }

        lParam = mDevice->GetGenParameters()->Get( GEN_INVERSION );
        if ( lParam != NULL )
        {
            lParam->UnregisterEventSink( this );
        }
    }
}


// ==========================================================================
void ThreadDisplay::ResetStats()
{
    mLastBlockID = 0;
    mStabilizer.Reset();
}


// ==========================================================================
void ThreadDisplay::ReleaseLatestBuffers()
{
    if ( mLatestBuffer != NULL )
    {
        mPipeline->ReleaseBuffer( mLatestBuffer );
        mLatestBuffer = NULL;
    }

    if ( mLatestOddBuffer != NULL )
    {
        mPipeline->ReleaseBuffer( mLatestOddBuffer );
        mLatestOddBuffer = NULL;
    }

    if ( mLatestEvenBuffer != NULL )
    {
        mPipeline->ReleaseBuffer( mLatestEvenBuffer );
        mLatestEvenBuffer = NULL;
    }
}


// ==========================================================================
bool ThreadDisplay::AreBlockIDsConsecutive( PvBuffer *aFirst, PvBuffer *aSecond )
{
    PvUInt16 lFirst = aFirst->GetBlockID();
    PvUInt16 lSecond = aSecond->GetBlockID();

    PvUInt16 lExpected = lFirst + 1;
    if ( lExpected == 0 )
    {
        lExpected = 1;
    }

    return ( lSecond == lExpected );
}


// ==========================================================================
void ThreadDisplay::ProcessBuffer( PvBuffer *aBuffer )
{
    // Based on regular or interlaced status, set latest buffer
    if ( !SetLatest( aBuffer ) )
    {
        return;
    }

    // Display and main dialog refresh throttled by FPS stabilizer
    if ( mStabilizer.IsTimeToDisplay( mTargetFPS ) ) 
    {
        // Only display if the display option is enabled!
        if ( mEnabled )
        {
            CSingleLock lLock( &mBufferMutex );
            Display( mFinalBuffer );
        }

        // Send notification to main dialog to update status
        // mMainDlg->PostMessage( WM_IMAGEDISPLAYED );
    }

    // Give the buffer to the image saving dialog - will save if necessary
    mImageSaveDlg->SaveIfNecessary( mFinalBuffer );
}


// ==========================================================================
void ThreadDisplay::Display( PvBuffer *aBuffer )
{
    // Display buffer
    mFilteringDlg->ConfigureConverter( mDisplayWnd->GetConverter() );
    mDisplayWnd->Display( *aBuffer, mVSync );
}


// ==========================================================================
bool ThreadDisplay::SetLatest( PvBuffer *aBuffer )
{
    CSingleLock lLock( &mBufferMutex );

    // since we've added the "keep partial images" option, this is a safeguard to make sure
    // we don't try to display a buffer with 0 width or height
	if ( ( aBuffer->GetWidth() == 0 ) || 
		 ( aBuffer->GetHeight() == 0 ) || 
		 ( ( aBuffer->GetAcquiredSize() - aBuffer->GetPaddingY() ) > aBuffer->GetSize() ) ||
		 ( ( aBuffer->GetRequiredSize() - aBuffer->GetPaddingY() ) > aBuffer->GetSize() ) )
    {
        mPipeline->ReleaseBuffer( aBuffer );
        return false;
    }

    if ( aBuffer->IsInterlacedOdd() )
    {
        // Save/replace Odd temp buffer, wait for matching Even
        ReleaseLatestBuffers();
        mLatestOddBuffer = aBuffer;
        return false;
    }
    else if ( aBuffer->IsInterlacedEven() )
    {
        // Do we have an Odd buffer? Is the Odd buffer matching this one?
        if ( ( mLatestOddBuffer != NULL ) && ( AreBlockIDsConsecutive( mLatestOddBuffer, aBuffer ) ) )
        {
            mLatestEvenBuffer = aBuffer;
        }
        else
        {
            mPipeline->ReleaseBuffer( aBuffer );
            return false;
        }
    }
    else
    {
        // No interlacing, just keep this buffer
        ReleaseLatestBuffers();
        mLatestBuffer = aBuffer;
    }

    if ( mLatestBuffer != NULL )
    {
        // Single buffer: progressive scan or old iPORT ANL de-interlacing
        mFinalBuffer = mLatestBuffer;
        if ( mDeinterlacingEnabled )
        {
            // De-interlacing required for old iPORT ANL
            mFilterDeinterlace.Apply( mFinalBuffer, &mDeinterlacedBuffer );
            mFinalBuffer = &mDeinterlacedBuffer;
            
            // Final pointing on de-interlaced buffer, we can release
            ReleaseLatestBuffers();
        }
    }
    else if ( ( mLatestOddBuffer != NULL ) && ( mLatestEvenBuffer != NULL ) )
    {
        // Odd and Even buffers available: new iPORT ANL-Pro de-interlacing
        mFilterDeinterlace.Apply( mLatestOddBuffer, mLatestEvenBuffer, &mDeinterlacedBuffer );
        mFinalBuffer = &mDeinterlacedBuffer;

        // Final pointing on de-interlaced buffer, we can release
        ReleaseLatestBuffers();
    }

    return true;
}


// ==========================================================================
void ThreadDisplay::Log( PvBuffer *aBuffer )
{
	if ( aBuffer == NULL )
	{
		return;
	}

	PvUInt32 lBlockID = aBuffer->GetBlockID();
    PvResult lResult = aBuffer->GetOperationResult();

    if ( mLogBuffer->IsBufferErrorEnabled() || mLogBuffer->IsBufferAllEnabled() )
	{
		PvUInt32 lBlockIDsMissing = 0;
		if ( ( mLastBlockID != 0 ) && ( lBlockID != 0 ) )
		{
			if  ( lBlockID > mLastBlockID )
			{
				// Easy
				lBlockIDsMissing = lBlockID - mLastBlockID - 1;
			}
			else
			{
				// Handle wrap-around
				lBlockIDsMissing =
					( USHRT_MAX - mLastBlockID ) +
					( lBlockID - 1 );
			}

			if ( lBlockIDsMissing > 0 )
			{
				CString lStr;
				lStr.Format( _T( "Block IDs between %04X and %04X missing" ),
					mLastBlockID, lBlockID );

				mLogBuffer->Log( lStr );
			}
		}

        if ( !lResult.IsOK() || mLogBuffer->IsBufferAllEnabled() )
        {
		    CString lStr;
		    lStr.Format( _T( "Result: %s BlockID: %04X Timestamp: %016llX" ), 
                PvString( lResult.GetCodeString() ).GetUnicode(), lBlockID, aBuffer->GetTimestamp() );

		    mLogBuffer->Log( lStr );
        }
	}

	// Save for next run...
	mLastBlockID = lBlockID;
}

