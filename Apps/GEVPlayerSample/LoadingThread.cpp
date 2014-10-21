#include "stdafx.h"

#include "LoadingThread.h"

LoadingThread::LoadingThread( SetupDlg *aSetupDlg, PvConfigurationReader *aReader, PvDevice *aDevice, PvStream *aStream, CWnd* aParent )
    : mSetupDlg( aSetupDlg )
    , mReader( aReader )
    , mDevice( aDevice )
    , mStream( aStream )
{
    mDlg = new ProgressDlg( this, aParent );
}

LoadingThread::~LoadingThread()
{
    if ( mDlg != NULL )
    {
        delete mDlg;
        mDlg = NULL;
    }
}

PvResult LoadingThread::Load()
{
    ASSERT( mDlg != NULL );
    mDlg->DoModal();
    return mResult;
}

DWORD LoadingThread::Function()
{
    PvResult lResult = PvResult::Code::NOT_CONNECTED;

    try
    {
        ASSERT( mDevice != NULL );
        ASSERT( mStream != NULL );

        if ( ( mSetupDlg->GetRole() == SetupDlg::RoleCtrlData ) ||
             ( mSetupDlg->GetRole() == SetupDlg::RoleCtrl ) ||
             ( mSetupDlg->GetRole() == SetupDlg::RoleMonitor ) )
        {
            if ( mDevice->IsConnected() )
            {
                mDlg->SetStatus( _T( "Restoring device parameters from file..." ) );
            }
            else
            {
                mDlg->SetStatus( _T( "Connecting to device and restoring it from file..." ) );
            }

            lResult = mReader->Restore( 0, mDevice );
            if ( !lResult.IsOK() )
            {
                mResult = lResult;
                return lResult.GetCode();
            }
        }

        if ( ( mSetupDlg->GetRole() == SetupDlg::RoleCtrlData ) ||
            ( mSetupDlg->GetRole() == SetupDlg::RoleData ) )
        {
            if ( mStream->IsOpen() )
            {
                mDlg->SetStatus( _T( "Restoring stream parameters from file..." ) );
            }
            else
            {
                mDlg->SetStatus( _T( "Opening a stream and restoring it from file..." ) );
            }

            lResult = mReader->Restore( 0, *mStream );
        }

        if ( !lResult.IsOK() )
        {
            mResult = lResult;
            return lResult.GetCode();
        }

        // Now that the stream is opened, set the destination on the device
        if ( ( mSetupDlg->GetRole() == SetupDlg::RoleCtrlData ) ||
             ( mSetupDlg->GetRole() == SetupDlg::RoleCtrl ) )
        {
            if ( mStream->IsOpen() && 
                 ( ( mSetupDlg->GetDestination() == SetupDlg::DestinationUnicastAuto ) ||
                   ( mSetupDlg->GetDestination() == SetupDlg::DestinationUnicastSpecific ) ) )
            {
                mDevice->SetStreamDestination( 
                    mStream->GetLocalIPAddress(), 
                    mStream->GetLocalPort() );
            }
            else if ( mSetupDlg->GetDestination() == SetupDlg::DestinationUnicastOther )
            {
                // Bug 2132: only set destination if different than 0.0.0.0:0
                if ( ( mSetupDlg->GetUnicastIP() != "0.0.0.0" ) &&
                     ( mSetupDlg->GetUnicastPort() != 0 ) )
                {
                    mDevice->SetStreamDestination( 
                        (LPCTSTR)mSetupDlg->GetUnicastIP(), 
                        mSetupDlg->GetUnicastPort() );
                }
            }
            else if ( mSetupDlg->GetDestination() == SetupDlg::DestinationMulticast )
            {
                mDevice->SetStreamDestination( 
                    (LPCTSTR)mSetupDlg->GetMulticastIP(), 
                    mSetupDlg->GetMulticastPort() );
            }
        }
    }
    catch ( ... )
    {
        lResult = PvResult( PvResult::Code::ABORTED, "Unexpected error loading persistence file" );
    }

    mResult = lResult;
    return lResult.GetCode();
}
