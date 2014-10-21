// *****************************************************************************
//
//     Copyright (c) 2007, Pleora Technologies Inc., All rights reserved.
//
// *****************************************************************************

#pragma once

#include "Thread.h"

#include <PvDisplayWnd.h>
#include <PvBuffer.h>
#include <PvBufferConverter.h>
#include <PvDevice.h>
#include <PvFilterDeinterlace.h>

#include <math.h>

#include "FPSStabilizer.h"
#include "LogBuffer.h"


class GEVPlayerDlg;
class ImageSaveDlg;
class FilteringDlg;


class ThreadDisplay : public Thread, PvGenEventSink
{
public:

    ThreadDisplay( 
        GEVPlayerDlg *aDlg, PvDisplayWnd *aDisplayWnd, 
        PvDevice *aDevice, PvPipeline *aPipeline, 
        FilteringDlg *aFilteringDlg, ImageSaveDlg* aImageSaveDlg,
        LogBuffer *aLogBuffer );
    ~ThreadDisplay();

    bool GetEnabled() const { return mEnabled; }
    void SetEnabled( bool aEnabled ) { mEnabled = aEnabled; }

    bool GetVSync() const { return mVSync; }
    void SetVSync( bool aVSync ) { mVSync = aVSync; }

    PvUInt32 GetTargetFPS() const { return mTargetFPS; }
    void SetTargetFPS( PvUInt32 aTarget ) { mTargetFPS = aTarget; }

    PvBuffer* RetrieveLatestBuffer();
    void ReleaseLatestBuffer();

    void WhiteBalance( PvFilterRGB *aFilterRGB );

    int GetFrameRate() { return ( mEnabled ) ? mStabilizer.GetAvg() : 0; }

    void ResetStats();

    bool GetDeinterlacingEnabled() const { return mDeinterlacingEnabled; }

    bool GetKeepPartialImages() { return mKeepPartialImages; }
    void SetKeepPartialImages( bool aKeepPartialImages ) { mKeepPartialImages = aKeepPartialImages; }

protected:

    void RegisterInvalidators();
    void UnregisterInvalidators();
    void UpdateDeinterlacer();

    void ReleaseLatestBuffers();
    void ProcessBuffer( PvBuffer *aBuffer );
    bool AreBlockIDsConsecutive( PvBuffer *aFirst, PvBuffer *aSecond );
    void Display( PvBuffer *aBuffer );
    bool SetLatest( PvBuffer *aBuffer );
    void Log( PvBuffer *aBuffer );

    void OnParameterUpdate( PvGenParameter *aParameter );

    DWORD Function();

private:

    // We do not own these objects
    GEVPlayerDlg *mMainDlg;
    PvDisplayWnd *mDisplayWnd;
    PvDevice *mDevice;
    PvPipeline * mPipeline;

    PvFilterDeinterlace mFilterDeinterlace;

    CMutex mBufferMutex;
    PvBuffer *mLatestBuffer;
    PvBuffer *mLatestEvenBuffer;
    PvBuffer *mLatestOddBuffer;

    PvBuffer *mFinalBuffer;
    PvBuffer mDeinterlacedBuffer;

    FilteringDlg *mFilteringDlg;
    ImageSaveDlg* mImageSaveDlg;

    bool mEnabled;
    bool mVSync;
    bool mDeinterlacingEnabled;
    PvUInt32 mTargetFPS;

    PvUInt16 mLastBlockID;

    FPSStabilizer mStabilizer;
    LogBuffer *mLogBuffer;
    bool mKeepPartialImages;
};
