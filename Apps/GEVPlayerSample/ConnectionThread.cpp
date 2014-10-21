#include "stdafx.h"
#include "ConnectionThread.h"
#include <PvInterface.h>


ConnectionThread::ConnectionThread( SetupDlg *aSetupDlg, PvDeviceInfo *aDeviceInfo, PvDevice *aDevice, PvStream *aStream, CWnd* aParent )
        : mSetupDlg( aSetupDlg )
        , mDeviceInfo( aDeviceInfo )
        , mDevice( aDevice )
        , mStream( aStream )
{
    mDlg = new ProgressDlg( this, aParent );
}

ConnectionThread::~ConnectionThread()
{
    if ( mDlg != NULL )
    {
        delete mDlg;
        mDlg = NULL;
    }
}

PvResult ConnectionThread::Connect()
{
    ASSERT( mDlg != NULL );
    mDlg->DoModal();

    return mResult;
}

void ConnectionThread::SetStreamDestination( const CString &aIpAddress, PvUInt16 aPort )
{
	ASSERT( mDevice != NULL );
	ASSERT( mStream != NULL );
	ASSERT( mDeviceInfo != NULL );

    // Bug 2132: only set destination if different than 0.0.0.0:0
    if ( ( aIpAddress == _T( "0.0.0.0" ) ) &&
         ( aPort == 0 ) )
    {
		return;
	}

	// Bug 3202: don't set destination on receiver, peripheral or transceiver (for now)
	if ( ( mDeviceInfo->GetClass() == PvDeviceClassReceiver ) ||
		 ( mDeviceInfo->GetClass() == PvDeviceClassPeripheral ) ||
		 ( mDeviceInfo->GetClass() == PvDeviceClassTransceiver ) )
	{
		return;
	}

	mDevice->SetStreamDestination( (LPCTSTR)aIpAddress, aPort );
}

DWORD ConnectionThread::Function()
{
    PvResult lResult = PvResult::Code::NOT_CONNECTED;

    try
    {
        if ( ( mSetupDlg->GetRole() == SetupDlg::RoleCtrlData ) ||
             ( mSetupDlg->GetRole() == SetupDlg::RoleCtrl ) ||
             ( mSetupDlg->GetRole() == SetupDlg::RoleMonitor ) )
        {
            // Connect device
            mDlg->SetStatus( _T( "Building GenICam interface..." ) );
            lResult = mDevice->Connect( mDeviceInfo, 
                ( mSetupDlg->GetRole() != SetupDlg::RoleMonitor ) ? 
                    PvAccessControl : 
                    PvAccessReadOnly );
            if ( !lResult.IsOK() )
            {
                mResult = lResult;
                return lResult.GetCode();
            }

            if ( mSetupDlg->GetRole() == SetupDlg::RoleCtrlData )
            {
                bool lEnabledValue = true;
                PvGenBoolean *lEnabled = dynamic_cast<PvGenBoolean *>( mDevice->GetGenLink()->Get( "AutoNegotiation" ) );
                if ( lEnabled != NULL )
                {
                    lEnabled->GetValue( lEnabledValue );
                }

                PvInt64 lUserPacketSizeValue = 1476;
                PvGenInteger *lUserPacketSize = dynamic_cast<PvGenInteger *>( mDevice->GetGenLink()->Get( "DefaultPacketSize" ) );
                if ( ( lUserPacketSize != NULL ) && lUserPacketSize->IsReadable() )
                {
                    lUserPacketSize->GetValue( lUserPacketSizeValue );
                }

                if ( lEnabledValue )
                {
                    // Perform automatic packet size negociation
                    mDlg->SetStatus( _T( "Optimizing streaming packet size..." ) );
                    lResult = mDevice->NegotiatePacketSize( 0, 1476 );
                    if ( !lResult.IsOK() )
                    {
                        mDlg->SetStatus( _T( "WARNING: streaming packet size optimization failure, using 1476 bytes!" ) );
                        ::Sleep( 3000 );
                    }
                }
                else
                {
                    bool lManualPacketSizeSuccess = false;

                    // Start by figuring out if we can use GenICam to set the packet size
                    bool lUseGenICam = false;
                    PvGenInteger *lPacketSize = dynamic_cast<PvGenInteger *>( mDevice->GetGenLink()->Get( "GevSCPSPacketSize" ) );
                    if ( lPacketSize != NULL )
                    {
                        bool lWritable = false;
                        PvResult lResult = lPacketSize->IsWritable( lWritable );
                        if ( lResult.IsOK() && lWritable )
                        {
                            lUseGenICam = true;
                        }
                    }
                    
                    if ( lUseGenICam )
                    {
                        // Setting packet size through the GenICam interface
                        PvResult lResult = lPacketSize->SetValue( lUserPacketSizeValue );
                        if ( lResult.IsOK() )
                        {
                            lManualPacketSizeSuccess = true;
                        }
                        else
                        {
                            // Last resort default...
                            lPacketSize->SetValue( 576 );
                            lManualPacketSizeSuccess = false;
                        }
                    }
                    else
                    {
                        // Direct write, for GenICam challenged devices...
                        PvResult lResult = mDevice->WriteRegister( 0x0D04, (PvUInt32)lUserPacketSizeValue );
                        if ( lResult.IsOK() )
                        {
                            lManualPacketSizeSuccess = true;
                        }
                        else
                        {
                            // Last resort default...
                            mDevice->WriteRegister( 0x0D04, 576 );
                            lManualPacketSizeSuccess = false;
                        }
                    }
                    
                    CString lNewStr;
                    if ( lManualPacketSizeSuccess )
                    {
                        lNewStr.Format( 
                            _T( "A packet of size of %i bytes was configured for streaming. You may experience issues " )
                            _T( "if your system configuration cannot support this packet size." ), 
                            lUserPacketSizeValue);
                    }
                    else
                    {                        
                        lNewStr.Format( _T( "WARNING: could not set streaming packet size to %i bytes, using %i bytes!" ),
                                       lUserPacketSizeValue, 576 );
                    }
                    mDlg->SetStatus( lNewStr );
                    ::Sleep( 3000 );
                }
            }
        }

        // Open stream - and retry if it fails
        if ( ( mSetupDlg->GetRole() == SetupDlg::RoleCtrlData ) ||
             ( mSetupDlg->GetRole() == SetupDlg::RoleData ) )
        {
            mDlg->SetStatus( _T( "Opening eBUS stream to device..." ) );

            int lCount = 0;
            for ( ;; )
            {
                if ( IsStopping() )
                {
                    return PvResult::Code::ABORTED;
                }

                bool lIsDriverInstalled = false;
                lResult = mStream->IsDriverInstalled( mDeviceInfo->GetIPAddress(), lIsDriverInstalled, 
                    mDeviceInfo->GetInterface()->GetIPAddress() );
                if ( !lResult.IsOK() )
                {
                    mResult = lResult;
                    return lResult.GetCode();
                }

                if ( !lIsDriverInstalled )
                {
                    if ( lCount < 3 )
                    {
                        mDlg->SetStatus( _T( "Device used for the first time, installing \r\neBUS protocol FDO..." ) );
                    }
                    else
                    {
                        mDlg->SetStatus( _T( "Opening eBUS stream to device..." ) );
                        mDlg->SetCanCancel( true );
                    }
                }

                // Open stream
                if ( mSetupDlg->GetDestination() == SetupDlg::DestinationUnicastAuto )
                {
                    lResult = mStream->Open( 
                        mDeviceInfo->GetIPAddress(), 0, 0, mDeviceInfo->GetInterface()->GetIPAddress() );
                }
                else if ( mSetupDlg->GetDestination() == SetupDlg::DestinationUnicastSpecific )
                {
                    lResult = mStream->Open(
                        mDeviceInfo->GetIPAddress(), mSetupDlg->GetUnicastSpecificPort(), 0, mDeviceInfo->GetInterface()->GetIPAddress() );
                }
                else if ( mSetupDlg->GetDestination() == SetupDlg::DestinationMulticast )
                {
                    lResult = mStream->Open(
                        mDeviceInfo->GetIPAddress(),
                        (LPCTSTR)mSetupDlg->GetMulticastIP(), 
                        mSetupDlg->GetMulticastPort(), 0, mDeviceInfo->GetInterface()->GetIPAddress() );
                }
                else
                {
                    ASSERT( 0 );
                }

                if ( lResult.IsOK() )
                {
                    break;
                }

                if ( lIsDriverInstalled )
                {
                    // The FDO is installed and we can't open the stream. Just fail.
                    break;
                }

                ::Sleep( 1000 );

                lCount++;
            }
        }

        // Now that the stream is opened, set the destination on the device
        if ( ( mSetupDlg->GetRole() == SetupDlg::RoleCtrlData ) ||
             ( mSetupDlg->GetRole() == SetupDlg::RoleCtrl ) )
        {
            if ( ( mSetupDlg->GetDestination() == SetupDlg::DestinationUnicastAuto ) ||
                 ( mSetupDlg->GetDestination() == SetupDlg::DestinationUnicastSpecific ) )
            {
				SetStreamDestination( mStream->GetLocalIPAddress().GetUnicode(), mStream->GetLocalPort() );
            }
            else if ( mSetupDlg->GetDestination() == SetupDlg::DestinationUnicastOther )
            {
                SetStreamDestination( mSetupDlg->GetUnicastIP(), mSetupDlg->GetUnicastPort() );
            }
            else if ( mSetupDlg->GetDestination() == SetupDlg::DestinationMulticast )
            {
                SetStreamDestination( (LPCTSTR)mSetupDlg->GetMulticastIP(), mSetupDlg->GetMulticastPort() );
            }
        }
    }
    catch ( ... )
    {
        lResult = PvResult( PvResult::Code::ABORTED, "Unexpected error" );
    }

    mResult = lResult;
    return lResult.GetCode();
}


