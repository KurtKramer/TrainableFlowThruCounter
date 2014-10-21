#pragma once

#include "ProgressDlg.h"
#include "SetupDlg.h"
#include <PvConfigurationReader.h>
#include <PvDevice.h>
#include <PvStream.h>

// =============================================================================
// Thread used to bring back a persisted state
//
class LoadingThread : public Thread
{
public:

    LoadingThread( SetupDlg *aSetupDlg, PvConfigurationReader *aReader, PvDevice *aDevice, PvStream *aStream, CWnd* aParent = NULL );
    virtual ~LoadingThread();
    
    PvResult Load();

protected:

    DWORD Function();


private:

    SetupDlg *mSetupDlg;
    PvConfigurationReader *mReader;
    PvDevice *mDevice;
    PvStream *mStream;
    ProgressDlg *mDlg;
    PvResult mResult;
};

