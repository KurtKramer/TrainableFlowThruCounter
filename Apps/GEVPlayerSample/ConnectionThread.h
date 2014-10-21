#pragma once

#include "ProgressDlg.h"
#include "SetupDlg.h"
#include <PvDeviceInfo.h>
#include <PvDevice.h>
#include <PvStream.h>

// =============================================================================
// Thread used to setup the system on a connect operation
//
class ConnectionThread : public Thread
{
public:

    ConnectionThread( SetupDlg *aSetupDlg, PvDeviceInfo *aDeviceInfo, PvDevice *aDevice, PvStream *aStream, CWnd* aParent );
    virtual ~ConnectionThread();
    
    PvResult Connect();

protected:

	void SetStreamDestination( const CString &aIPAddress, PvUInt16 aPort );

    DWORD Function();

private:

    SetupDlg *mSetupDlg;
    PvDeviceInfo *mDeviceInfo;
    PvDevice *mDevice;
    PvStream *mStream;
    ProgressDlg *mDlg;
    PvResult mResult;
};

