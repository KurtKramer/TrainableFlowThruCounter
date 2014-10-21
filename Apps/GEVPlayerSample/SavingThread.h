#pragma once

#include "ProgressDlg.h"
#include "SetupDlg.h"
#include <PvConfigurationWriter.h>
#include <PvDevice.h>
#include <PvStream.h>


// =============================================================================
// Thread used to save a persisted state
//
class SavingThread : public Thread
{
public:

    SavingThread( SetupDlg *aSetupDlg, PvConfigurationWriter *aWriter, PvDevice *aDevice, PvStream *aStream, CWnd* aParent = NULL );
    virtual ~SavingThread();

    PvResult Save();

protected:

    DWORD Function();

private:

    SetupDlg *mSetupDlg;
    PvConfigurationWriter *mWriter;
    PvDevice *mDevice;
    PvStream *mStream;
    ProgressDlg *mDlg;

};