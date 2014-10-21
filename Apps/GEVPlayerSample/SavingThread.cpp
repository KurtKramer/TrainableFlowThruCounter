#include "stdafx.h"

#include "SavingThread.h"


SavingThread::SavingThread( SetupDlg *aSetupDlg, PvConfigurationWriter *aWriter, PvDevice *aDevice, PvStream *aStream, CWnd* aParent )
    : mSetupDlg( aSetupDlg )
    , mWriter( aWriter )
    , mDevice( aDevice )
    , mStream( aStream )
{
    mDlg = new ProgressDlg( this, aParent );
}

SavingThread::~SavingThread()
{
    if ( mDlg != NULL )
    {
        delete mDlg;
        mDlg = NULL;
    }
}

PvResult SavingThread::Save()
{
    mDlg->DoModal();
    return GetReturnValue();
}

DWORD SavingThread::Function()
{
    PvResult lResult = PvResult::Code::NOT_CONNECTED;

    try
    {
        ASSERT( mDevice != NULL );
        ASSERT( mStream != NULL );
        ASSERT( mDlg != NULL );

        if ( ( mSetupDlg->GetRole() == SetupDlg::RoleCtrlData ) ||
             ( mSetupDlg->GetRole() == SetupDlg::RoleCtrl ) ||
             ( mSetupDlg->GetRole() == SetupDlg::RoleMonitor ) )
        {
            mDlg->SetStatus( _T( "Saving the device parameters..." ) );

            lResult = mWriter->Store(mDevice);
            if ( !lResult.IsOK() )
            {
                return lResult.GetCode();
            }
        }

        if ( ( mSetupDlg->GetRole() == SetupDlg::RoleCtrlData ) ||
            ( mSetupDlg->GetRole() == SetupDlg::RoleData ) )
        {
            mDlg->SetStatus( _T( "Saving the stream parameters..." ) );

            lResult = mWriter->Store(mStream);
            if ( !lResult.IsOK() )
            {
                return lResult.GetCode();
            }
        }
    }
    catch ( ... )
    {
        lResult = PvResult::Code::ABORTED;
    }

    return lResult.GetCode();
}
