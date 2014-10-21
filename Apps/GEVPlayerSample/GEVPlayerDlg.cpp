// *****************************************************************************
//
//     Copyright (c) 2007, Pleora Technologies Inc., All rights reserved.
//
// *****************************************************************************

#include "stdafx.h"

#include "GEVPlayer.h"
#include "GEVPlayerDlg.h"
#include "AboutBox.h"
#include "SplashPage.h"
#include "SetupDlg.h"
#include "ImageSaveDlg.h"
#include "LoadingThread.h"
#include "ConnectionThread.h"
#include "SavingThread.h"
#include "BufferOptionsDlg.h"
#include "PvMessageBox.h"

#include <PvConfigurationWriter.h>
#include <PvConfigurationReader.h>
#include <PvSerialPortIPEngine.h>

#include <string.h>
#include <limits.h>
#include <io.h>


#define DISPLAYSETUP_VERSION ( "1.0.0.0" )

#define TAG_VERSION ( "displaysetupversion" )
#define TAG_ACCELERATION ( "displayacceleration" )
#define TAG_VSYNC ( "displayvsync" )
#define TAG_KEEP_PARTIAL ( "keeppartialimages" )
#define TAG_ENABLED ( "displayenabled" )
#define TAG_FPS ( "displayframerate" )
#define TAG_STREAMINGBUFFERCOUNT ( "streamingbuffercount" )
static const int BUFFER_COUNT_DEFAULT = 16;
static const int BUFFER_COUNT_MIN = 4;


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


BEGIN_MESSAGE_MAP(GEVPlayerDlg, CDialog)
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_WM_SIZE()
    ON_WM_GETMINMAXINFO()
    ON_BN_CLICKED(IDC_DEVICE_BUTTON, &GEVPlayerDlg::OnBnClickedDeviceButton)
    ON_BN_CLICKED(IDC_LINK_BUTTON, &GEVPlayerDlg::OnBnClickedLinkButton)
    ON_BN_CLICKED(IDC_STREAMPARAMS_BUTTON, &GEVPlayerDlg::OnBnClickedStreamparamsButton)
    ON_BN_CLICKED(IDC_CONNECT_BUTTON, &GEVPlayerDlg::OnBnClickedConnectButton)
    ON_BN_CLICKED(IDC_DISCONNECT_BUTTON, &GEVPlayerDlg::OnBnClickedDisconnectButton)
    ON_WM_CLOSE()
    ON_BN_CLICKED(IDC_START, &GEVPlayerDlg::OnBnClickedStart)
    ON_BN_CLICKED(IDC_STOP, &GEVPlayerDlg::OnBnClickedStop)
    ON_CBN_SELCHANGE(IDC_MODE, &GEVPlayerDlg::OnCbnSelchangeMode)
    ON_MESSAGE(WM_IMAGEDISPLAYED, OnImageDisplayed)
    ON_BN_CLICKED(IDC_DEVICEEVENTS_BUTTON, &GEVPlayerDlg::OnBnClickedDeviceEvents)
    ON_COMMAND(ID_FILE_LOAD, &GEVPlayerDlg::OnFileLoad)
    ON_COMMAND(ID_FILE_SAVE, &GEVPlayerDlg::OnFileSave)
    ON_COMMAND(ID_FILE_EXIT, &GEVPlayerDlg::OnFileExit)
    ON_COMMAND(ID_HELP_EVENTMONITOR, &GEVPlayerDlg::OnHelpEventmonitor)
    ON_COMMAND(ID_HELP_ABOUT, &GEVPlayerDlg::OnHelpAbout)
    ON_COMMAND_RANGE(ID_FILE_RECENT1, ID_FILE_RECENT10, &GEVPlayerDlg::OnRecent)
    ON_WM_INITMENU()
    ON_WM_MOVE()
    ON_COMMAND(ID_TOOLS_DISPLAYVSYNC, &GEVPlayerDlg::OnToolsDisplaysync)
    ON_WM_CTLCOLOR()
    ON_MESSAGE(WM_DISCONNECT, OnDisconnect)
    ON_MESSAGE(WM_STOPSTREAMING, OnStopStreaming)
    ON_COMMAND(ID_FILE_SAVEAS, &GEVPlayerDlg::OnFileSaveAs)
    ON_COMMAND(ID_ACCELERATOR_SAVE, &GEVPlayerDlg::OnAcceleratorSave)
    ON_COMMAND(ID_ACCELERATOR_OPEN, &GEVPlayerDlg::OnAcceleratorOpen)
    ON_COMMAND(ID_TOOLS_SETUP, &GEVPlayerDlg::OnToolsSetup)
    ON_COMMAND(ID_TOOLS_IMAGEFILTERING, &GEVPlayerDlg::OnToolsImagefiltering)
    ON_COMMAND(ID_TOOLS_SAVEIMAGES, &GEVPlayerDlg::OnToolsSaveimages)
    ON_COMMAND(ID_TOOLS_SAVECURRENTIMAGE, &GEVPlayerDlg::OnToolsSavecurrentimage)
    ON_MESSAGE(WM_STREAM, OnStream)
    ON_COMMAND(ID_TOOLS_IPENGINESERIALCOMMUNICATION, &GEVPlayerDlg::OnToolsSerialCommunication)
    ON_WM_TIMER()
    ON_WM_DESTROY()
    ON_COMMAND(ID_REGISTERINTERFACE, &GEVPlayerDlg::OnRegisterInterface)
    ON_COMMAND(ID_DISPLAYIMAGES_DISABLED, &GEVPlayerDlg::OnDisplayDisabled)
    ON_COMMAND(ID_DISPLAYIMAGES_30FPS, &GEVPlayerDlg::OnDisplay30FPS)
    ON_COMMAND(ID_DISPLAYIMAGES_60FPS, &GEVPlayerDlg::OnDisplay60FPS)
    ON_COMMAND(ID_TOOLS_SAVEPREFERENCES, &GEVPlayerDlg::OnToolsSavepreferences)
    ON_COMMAND(ID_TOOLS_RESTOREDEFAULTPREFERENCES, &GEVPlayerDlg::OnToolsRestoredefaultpreferences)
    ON_COMMAND(ID_TOOLS_BUFFEROPTIONS, &GEVPlayerDlg::OnToolsBufferoptions)
    ON_COMMAND(ID_TOOLS_DISPLAYPARTIALIMAGES, &GEVPlayerDlg::OnToolsDisplaypartialimages)
END_MESSAGE_MAP()


// =============================================================================
GEVPlayerDlg::GEVPlayerDlg( CString aFileName, CWnd* pParent /* =NULL */ )
    : CDialog( GEVPlayerDlg::IDD, pParent )
    , mNeedInit( TRUE )
    , mThreadDisplay( NULL )
    , mEventMonitorDlg( NULL )
    , mAcquiringImages( false )
    , mMRUMenu( NULL )
    , mDeviceConnected( false )
    , mFileName( aFileName )
    , mPipeline( &mStream )
    , mDisconnectTime( 0 )
    , mNewIP( false )
    , mStatusColor( SCDefault )
    , mSerial0IPEngineSupported( false )
    , mTimer( 0 )
    , mTargetFPS( 30 )
    , mVSync( false )
    , mLogBuffer( NULL )
    , mKeepPartialImages( false )
{
    m_hIcon = AfxGetApp()->LoadIcon( IDR_MAINFRAME );

    mPlayButton.SetBitmap( IDB_PLAY );
    mStopButton.SetBitmap( IDB_STOP );

    mImageSaveDlg = new ImageSaveDlg( &mFilteringDlg, this );
    mLogBuffer = new LogBuffer;
}


// =============================================================================
GEVPlayerDlg::~GEVPlayerDlg()
{
    if ( mLogBuffer != NULL )
    {
        delete mLogBuffer;
        mLogBuffer = NULL;
    }

    if ( mMRUMenu != NULL )
    {
        delete mMRUMenu;
        mMRUMenu = NULL;
    }

    if ( mImageSaveDlg != NULL )
    {
        delete mImageSaveDlg;
        mImageSaveDlg = NULL;
    }
}


// =============================================================================
void GEVPlayerDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_MODE, mModeCombo);
    DDX_Control(pDX, IDC_CHANNEL, mChannelCombo);
    DDX_Control(pDX, IDC_START, mPlayButton);
    DDX_Control(pDX, IDC_STOP, mStopButton);
    DDX_Control(pDX, IDC_STATUS, mStatusTextBox);
    DDX_Control(pDX, IDC_IP_EDIT, mIPEdit);
    DDX_Control(pDX, IDC_MAC_EDIT, mMACEdit);
    DDX_Control(pDX, IDC_MODEL_EDIT, mModelEdit);
    DDX_Control(pDX, IDC_MANUFACTURER_EDIT, mManufacturerEdit);
    DDX_Control(pDX, IDC_NAME_EDIT2, mNameEdit);
}


// =============================================================================
BOOL GEVPlayerDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

#ifndef _DEBUG
#ifdef PT_INTERNAL
    SplashPage::Show();
#endif //PT_INTERNAL
#endif // _DEBUG

    mFilteringDlg.Create( IDD_FILTERING, this );

    mYellowColor = RGB( 0xFF, 0xFF, 0x00 );
    mYellowBrush.CreateSolidBrush( mYellowColor );

    mRedColor = RGB( 0x80, 0x00, 0x00 );
    mRedBrush.CreateSolidBrush( mRedColor );

    SetStatusColor( SCDefault );

    GetClientRect( mCrt );
    mNeedInit = FALSE;

    // Set the icon for this dialog.  The framework does this automatically
    //  when the application's main window is not a dialog
    SetIcon(m_hIcon, TRUE);			// Set big icon
    SetIcon(m_hIcon, FALSE);		// Set small icon

    // Right now we support a single, static channel
    mChannelCombo.ResetContent();
    mChannelCombo.AddString( _T( "Data Channel #0" ) );
    mChannelCombo.SetCurSel( 0 );

    CRect lDisplayRect;
    GetDlgItem( IDC_DISPLAYPOS )->GetClientRect( &lDisplayRect );
    GetDlgItem( IDC_DISPLAYPOS )->ClientToScreen( &lDisplayRect );
    ScreenToClient( &lDisplayRect );
    mDisplay.Create( GetSafeHwnd(), 10000 );
    mDisplay.SetPosition( lDisplayRect.left, lDisplayRect.top, lDisplayRect.Width(), lDisplayRect.Height() );
    mDisplay.SetBackgroundColor( 0x80, 0x80, 0x80 );
    
    EnableInterface();
    
    LoadMRUFromRegistry();
    UpdateMRUMenu();

    // create a defaults persistence file if it doesn't already exist
    CString lDefaultPath = GetDefaultPath();
    SaveConfig( lDefaultPath, false );

    if ( !mFileName.IsEmpty() )
    {
        OpenConfig( mFileName );
    }
    else
    {
        // check for the existence of the sticky configuration file. If it exists, load it.
        CString lStickyPath = GetStickyPath();
        if( _taccess( lStickyPath.GetBuffer(), 0 ) == 0 )
        {
            OpenConfig( lStickyPath );
        }
    }

    mAccel = ::LoadAccelerators(AfxGetResourceHandle(),
             MAKEINTRESOURCE(IDR_ACCELERATOR));

    // Create timer that will be used to periodically update the status bar
    mTimer = SetTimer( 1, 250, NULL );

    return TRUE;  // return TRUE  unless you set the focus to a control
}


// =============================================================================
void GEVPlayerDlg::OnPaint()
{
    if (IsIconic())
    {
        CPaintDC dc(this); // device context for painting

        SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

        // Center icon in client rectangle
        int cxIcon = GetSystemMetrics(SM_CXICON);
        int cyIcon = GetSystemMetrics(SM_CYICON);
        CRect rect;
        GetClientRect(&rect);
        int x = (rect.Width() - cxIcon + 1) / 2;
        int y = (rect.Height() - cyIcon + 1) / 2;

        // Draw the icon
        dc.DrawIcon(x, y, m_hIcon);
    }
    else
    {
        CDialog::OnPaint();
    }
}


// =============================================================================
HCURSOR GEVPlayerDlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}


// =============================================================================
void GEVPlayerDlg::OnClose()
{
    // Make sure we cleanup before we leave
    Disconnect();

    CDialog::OnClose();
}


// =============================================================================
void GEVPlayerDlg::OnBnClickedConnectButton()
{
    // create a device finder wnd and open the select device dialog
    PvDeviceFinderWnd lFinder;
    lFinder.SetTitle( _T( "GEV Device Selection" ) );

    PvResult lResult = lFinder.ShowModal();
    if ( !lResult.IsOK() || ( lFinder.GetSelected() == NULL ) )
    {
        return;
    }

    CWaitCursor lCursor;

    UpdateWindow();

    Connect( lFinder.GetSelected(), NULL );
}


// =============================================================================
void GEVPlayerDlg::OnBnClickedDisconnectButton()
{
    CWaitCursor lCursor;

    Disconnect();
}


// =============================================================================
void GEVPlayerDlg::OnSize(UINT nType, int cx, int cy)
{
    CDialog::OnSize(nType, cx, cy);

    CRect lR;
    this->GetWindowRect( &lR );
    // TRACE( "%i %i\n", lR.Width(), lR.Height() );

    if ( mNeedInit || cx == 0 || cy == 0 )
    {
        return;
    }

    int dx = cx - mCrt.Width();
    int dy = cy - mCrt.Height();

    GetClientRect(&mCrt);

    CRect r1;

    HDWP hDWP = ::BeginDeferWindowPos(20);

    //
    // Bottom left, just bring the group box down
    //

    int lBottomLeft[] =
    {
        IDC_CONTROL_GROUP
    };

    for ( int i = 0; i < sizeof( lBottomLeft ) / sizeof ( lBottomLeft[ 0 ] ); i++ )
    {
        CWnd *lChild = GetDlgItem( lBottomLeft[ i ] );
        if ( lChild != NULL )
        {
            lChild->GetWindowRect(&r1); ScreenToClient(&r1); 
            r1.bottom += dy;
            ::DeferWindowPos(
                hDWP, lChild->m_hWnd, NULL, 
                r1.left, r1.top, r1.Width(), r1.Height(),
                SWP_NOACTIVATE|SWP_NOZORDER);
        }
    }

    //
    // Right, make sure the display and group box fill the right part
    // of our window
    //

    int lDisplayGroup[] =
    {
        IDC_DISPLAY_GROUP, IDC_DISPLAYPOS
    };

    for ( int i = 0; i < sizeof( lDisplayGroup ) / sizeof ( lDisplayGroup[ 0 ] ); i++ )
    {
        CWnd *lChild = GetDlgItem( lDisplayGroup[ i ] );
        if ( lChild != NULL )
        {
            lChild->GetWindowRect(&r1); ScreenToClient(&r1); 
            r1.bottom += dy;
            r1.right += dx;
            ::DeferWindowPos(
                hDWP, lChild->m_hWnd, NULL, 
                r1.left, r1.top, r1.Width(), r1.Height(),
                SWP_NOACTIVATE|SWP_NOZORDER);

            if ( lDisplayGroup[i] == IDC_DISPLAYPOS )
            {
                mDisplay.SetPosition( 
                    r1.left, r1.top, 
                    r1.Width(), r1.Height() );
            }
        }
    }


    //
    // Last, the status box. Just like the display, but sticks to the bottom
    // and is not resized
    //

    CWnd *lBottomRight[] = 
    { 
        &mStatusTextBox
    };

    for ( int i = 0; i < sizeof( lBottomRight ) / sizeof ( lBottomRight[ 0 ] ); i++ )
    {
        CWnd *lChild = lBottomRight[ i ];
        if ( lChild != NULL )
        {
            lChild->GetWindowRect(&r1); ScreenToClient(&r1); 
            ::DeferWindowPos(
                hDWP, lChild->m_hWnd, NULL, 
                r1.left, r1.top + dy, r1.Width() + dx, r1.Height(),
                SWP_NOACTIVATE|SWP_NOZORDER);
        }
    }

    ::EndDeferWindowPos(hDWP);
}


// =============================================================================
void GEVPlayerDlg::OnGetMinMaxInfo( MINMAXINFO *lpMMI )
{
    lpMMI->ptMinTrackSize.x = 800;
    lpMMI->ptMinTrackSize.y = 517;
}

// =============================================================================
void GEVPlayerDlg::EnableInterface()
{
    // This method can be called really early or late when the window is not created
    if ( GetSafeHwnd() == 0 )
    {
        return;
    }

    GetDlgItem( IDC_CONNECT_BUTTON )->EnableWindow( !mDeviceConnected && !mStream.IsOpen() );
    GetDlgItem( IDC_DISCONNECT_BUTTON )->EnableWindow( mDeviceConnected || mStream.IsOpen() );

    GetDlgItem( IDC_DEVICE_BUTTON )->EnableWindow( mDeviceConnected );
    GetDlgItem( IDC_STREAMPARAMS_BUTTON )->EnableWindow( mStream.IsOpen() );

    // Special case, we want to allow setting communication paramters before we connect
    GetDlgItem( IDC_LINK_BUTTON )->EnableWindow( mSetupDlg.GetRole() != SetupDlg::RoleData  ); 

    mPlayButton.EnableWindow( mDeviceConnected && ( mSetupDlg.GetRole() != SetupDlg::RoleMonitor ) );

    // If not connected, disable the acquisition mode control. If enabled,
    // it will be managed automatically by events from the GenICam parameters
    if ( !mDeviceConnected )
    {
        mModeCombo.EnableWindow( FALSE );
        mStopButton.EnableWindow( FALSE );
    }
    else
    {
        PvGenCommand *lStart = dynamic_cast<PvGenCommand *>( mDevice.GetGenParameters()->Get( "AcquisitionStart" ) );
        PvGenCommand *lStop = dynamic_cast<PvGenCommand *>( mDevice.GetGenParameters()->Get( "AcquisitionStop" ) );

        mPlayButton.EnableWindow( lStart != NULL );
        mStopButton.EnableWindow( ( lStop != NULL ) && ( mSetupDlg.GetRole() != SetupDlg::RoleMonitor ) );
    }
}


// =============================================================================
void GEVPlayerDlg::Connect( PvDeviceInfo *aDI, PvConfigurationReader *aCR )
{
    ASSERT( aDI != NULL || aCR != NULL );
    if ( aDI == NULL && aCR == NULL )	
    {
        return;
    }

    if ( aDI != NULL )
    {
        PvDeviceClass lClass = aDI->GetClass();
        SetupDlg::Role lRole = mSetupDlg.GetRole();

		if ( ( lClass == PvDeviceClassReceiver ) || 
			 ( lClass == PvDeviceClassPeripheral ) ||
			 ( lClass == PvDeviceClassTransceiver ) )
        {
			// For receiver and peripheral, we make sure  role is controller only
            if ( lRole == SetupDlg::RoleCtrlData || lRole == SetupDlg::RoleData )
            {
                CString lMessage;

				if ( ( lClass == PvDeviceClassReceiver ) ||
					 ( lClass == PvDeviceClassPeripheral ) )
				{
					// Receiver and peripheral message
					lMessage = _T( "You have chosen to connect to a " );
					lMessage += ( lClass == PvDeviceClassReceiver ) ? _T( "receiver" ) : _T( "peripheral" );
					lMessage += _T( " however GEVPlayer is currently set up to connect as a " );
					lMessage += ( lRole == SetupDlg::RoleCtrlData ) ? _T( "controller and data receiver" ) : _T( "data receiver" );
					lMessage += _T( ". \r\n\r\nRe-attempt connection as a controller?" );
				}
				else if ( lClass == PvDeviceClassTransceiver )
				{
					// Transceiver message
					lMessage = _T( "You have chosen to connect to a transceiver. GEVPlayer does not fully\r\n" );
					lMessage += _T( "support transceivers in this version. Re-attempt connection as a \r\n" );
					lMessage += _T( "controller?" );
				}
				else
				{
					ASSERT( 0 );
				}

                if ( MessageBox( lMessage, _T( "GEVPlayer"), MB_YESNO | MB_ICONQUESTION ) == IDNO )
                {
                    return;
                }
                else
                {
                    mSetupDlg.SetRole( SetupDlg::RoleCtrl );
					mSetupDlg.SetDestination( SetupDlg::DestinationUnicastOther );
					mSetupDlg.SetUnicastIP( _T( "0.0.0.0" ) );
					mSetupDlg.SetUnicastPort( 0 );
                }
            }
        }
    }
    
    // Just in case we came here still connected...
    Disconnect();

    mDisconnectTime = 0;
    mNewIP = false;

    // Device connection, packet size negociation and stream opening
    // is performed in a separate thread while we display a progress
    // dialog to the user
    PvResult lResult;
    if ( aDI != NULL )
    {
        ConnectionThread lConnectionThread( &mSetupDlg, aDI, &mDevice, &mStream, this );
        lResult = lConnectionThread.Connect();
    }
    else if ( aCR != NULL )
    {
        LoadingThread lLoadingThread( &mSetupDlg, aCR, &mDevice, &mStream, this );
        lResult = lLoadingThread.Load();
    }

    if ( !lResult.IsOK() )
    {
        PvMessageBox( this, lResult );
        Disconnect();

        return;
    }

    mDeviceConnected = mDevice.IsConnected();

    if ( aDI != NULL )
    {
        mManufacturerEdit.SetWindowText( aDI->GetVendor().GetUnicode() );
        mModelEdit.SetWindowText( aDI->GetModel().GetUnicode() );
        mIPEdit.SetWindowText( aDI->GetIPAddress().GetUnicode() );
        mMACEdit.SetWindowText( aDI->GetMACAddress().GetUnicode() );
        mNameEdit.SetWindowText( aDI->GetUserDefinedName().GetUnicode() );
    }

    if ( mDeviceConnected )
    {
        // Create the event monitor *before* registering the device event sink
        ASSERT( mEventMonitorDlg == NULL );
        mEventMonitorDlg = new EventMonitorDlg( mLogBuffer, mDevice.GetGenParameters(), this );
        mEventMonitorDlg->Create( EventMonitorDlg::IDD, this );

        // Register device event sink
        mDevice.RegisterEventSink( this );

        // Register to all events of the parameters in the device's node map
        PvGenParameterArray *lGenDevice = mDevice.GetGenParameters();
        for ( PvUInt32 i = 0; i < lGenDevice->GetCount(); i++ )
        {
            lGenDevice->Get( i )->RegisterEventSink( this );
        }

        if ( aDI == NULL )
        {
            PvGenInteger *lIPAddressParam = dynamic_cast<PvGenInteger *>( lGenDevice->Get( "GevCurrentIPAddress" ) );
            PvGenInteger *lMACAddressParam = dynamic_cast<PvGenInteger *>( lGenDevice->Get( "GevMACAddress" ) );
            PvGenString *lManufacturer = dynamic_cast<PvGenString *>( lGenDevice->Get( "DeviceVendorName" ) );
            PvGenString *lModelName = dynamic_cast<PvGenString *>( lGenDevice->Get( "DeviceModelName" ) );
            PvGenString *lDeviceVersion = dynamic_cast<PvGenString *>( lGenDevice->Get( "DeviceVersion" ) );
            PvGenString *lNameParam = dynamic_cast<PvGenString *>( lGenDevice->Get( "GevDeviceUserID " ) );

            // Verify all params - except name which is optional
            ASSERT( lIPAddressParam != NULL );
            ASSERT( lMACAddressParam != NULL );
            ASSERT( lManufacturer != NULL );
            ASSERT( lModelName != NULL );
            ASSERT( lDeviceVersion != NULL );

            PvString lManufacturerStr, lModelNameStr, lDeviceVersionStr;
            lManufacturer->GetValue( lManufacturerStr );
            lModelName->GetValue( lModelNameStr );
            lDeviceVersion->GetValue( lDeviceVersionStr );

            // IP
            CString lIPStr;
            PvInt64 lIPAddress = 0;
            lIPAddressParam->GetValue( lIPAddress );
            unsigned char *lIPPtr = reinterpret_cast<unsigned char *>( &lIPAddress );
            lIPStr.Format( _T( "%i.%i.%i.%i" ), 
                lIPPtr[3], lIPPtr[2], lIPPtr[1], lIPPtr[0] );

            // MAC address
            CString lMACStr;
            PvInt64 lMACAddress;
            lMACAddressParam->GetValue( lMACAddress );
            unsigned char *lMACPtr = reinterpret_cast<unsigned char *>( &lMACAddress );
            lMACStr.Format( _T( "%02X:%02X:%02X:%02X:%02X:%02X" ), 
                lMACPtr[5], lMACPtr[4], lMACPtr[3], lMACPtr[2], lMACPtr[1], lMACPtr[0] );

            // Device name (User ID)
            CString lNameStr( "N/A" );
            if ( lNameParam != NULL )
            {
                PvString lStr;
                lNameParam->GetValue( lStr );
                lNameStr = lStr.GetUnicode();
            }

            mManufacturerEdit.SetWindowText( lManufacturerStr );
            mModelEdit.SetWindowText( lModelNameStr.GetUnicode() );
            mIPEdit.SetWindowText( lIPStr );
            mMACEdit.SetWindowText( lMACStr );
            mNameEdit.SetWindowText( lNameStr );
        }

        // Fill acquisition mode combo box
        mModeCombo.ResetContent();
        PvGenEnum *lMode = dynamic_cast<PvGenEnum *>( mDevice.GetGenParameters()->Get( "AcquisitionMode" ) );
        if ( lMode != NULL )
        {
            ASSERT( lMode != NULL ); // Mandatory parameter
            PvInt64 lEntriesCount = 0;
            VERIFY( lMode->GetEntriesCount( lEntriesCount ) ); // Failure here would be totally unexpected
            for ( PvUInt32 i = 0; i < lEntriesCount; i++ )
            {
                const PvGenEnumEntry *lEntry = NULL;
                VERIFY( lMode->GetEntryByIndex( i, &lEntry ) ); // Not expecting any failure on this one
                
                bool lAvailable = false;
                lEntry->IsAvailable( lAvailable );
                if ( lAvailable )
                {
                    PvString lEEName;
                    VERIFY( lEntry->GetName( lEEName ).IsOK() );

                    PvInt64 lEEValue;
                    VERIFY( lEntry->GetValue( lEEValue ).IsOK() );

                    int lIndex = mModeCombo.AddString( lEEName.GetUnicode() );
                    mModeCombo.SetItemData( lIndex, static_cast<DWORD_PTR>( lEEValue ) );
                }
            }

            // Set mode combo box to value currently used by the device
            PvInt64 lValue = 0;
            lMode->GetValue( lValue );
            for ( int i = 0; i < mModeCombo.GetCount(); i++ )
            {
                if ( lValue == mModeCombo.GetItemData( i ) )
                {
                    mModeCombo.SetCurSel( i );
                    break;
                }
            }
        }

        PvGenCommand *lStart = dynamic_cast<PvGenCommand *>( mDevice.GetGenParameters()->Get( "AcquisitionStart" ) );
        PvGenCommand *lStop = dynamic_cast<PvGenCommand *>( mDevice.GetGenParameters()->Get( "AcquisitionStop" ) );

        // Force an update on all the parameters we have shortcuts to
        if ( lMode != NULL ) OnParameterUpdate( lMode );
        if ( lStart != NULL ) OnParameterUpdate( lStart );
        if ( lStop != NULL ) OnParameterUpdate( lStop );

        mAcquiringImages = false;

        // Check whether IP Engine serial communication is supported or not
        mSerial0IPEngineSupported = PvSerialPortIPEngine::IsSupported( &mDevice, PvIPEngineSerial0 );
        mSerial0IPEngineSupported |= PvSerialPortIPEngine::IsSupported( &mDevice, PvIPEngineSerialBulk0 );
    }

    if ( mStream.IsOpen() )
    {
        // Ready image reception
        StartStreaming();
    }

    // Sync the UI with our new status
    EnableInterface();
}


// =============================================================================
void GEVPlayerDlg::Disconnect()
{
    mDisconnectTime = ::GetTickCount();
    mNewIP = false;
    
    // Close all configuration child windows
    CloseGenWindow( &mDeviceWnd );
    CloseGenWindow( &mStreamParametersWnd );
    mFilteringDlg.ShowWindow( SW_HIDE );
    if ( mTerminalIPEngineWnd.GetHandle() != 0 )
    {
        mTerminalIPEngineWnd.Close();
    }
    if ( mRegisterInterfaceDlg.GetSafeHwnd() != 0 )
    {
        mRegisterInterfaceDlg.EndDialog( IDOK );
    }

    if ( mDevice.IsConnected() || mDeviceConnected )
    {
        // Unregister device event sink
        mDevice.UnregisterEventSink( this );

        // Close event monitor
        if ( mEventMonitorDlg != NULL )
        {
            if ( mEventMonitorDlg->GetSafeHwnd() != 0 )
            {
                mEventMonitorDlg->DestroyWindow();
            }

            delete mEventMonitorDlg;
            mEventMonitorDlg = NULL;
        }

        mDevice.Disconnect();
    }

    // If streaming, stop streaming
    if ( mStream.IsOpen() )
    {
        StopStreaming();
        mStream.Close();
    }

    // Reset device ID - can be called by the destructor when the window
    // no longer exists, be careful...
    if ( GetSafeHwnd() != 0 )
    {
        mManufacturerEdit.SetWindowText( _T( "" ) );
        mModelEdit.SetWindowText( _T( "" ) );
        mIPEdit.SetWindowText( _T( "" ) );
        mMACEdit.SetWindowText( _T( "" ) );
        mNameEdit.SetWindowText( _T( "" ) );
    }

    mStatusTextBox.SetWindowText( _T( "" ) );
    SetStatusColor( SCDefault );

    mDeviceConnected = false;

    // Sync the UI with our new status
    EnableInterface();
    SetWindowText( L"GEVPlayer" );
}


// =============================================================================
void GEVPlayerDlg::OnBnClickedDeviceButton()
{
    if ( !mDeviceConnected )
    {
        return;
    }

    ShowGenWindow( 
        &mDeviceWnd, 
        mDevice.GetGenParameters(), 
        _T( "GEV Device Control" ) );
}


// =============================================================================
void GEVPlayerDlg::OnBnClickedDeviceEvents()
{
}


// =============================================================================
void GEVPlayerDlg::OnBnClickedLinkButton()
{
    ShowGenWindow( 
        &mCommunicationWnd, 
        mDevice.GetGenLink(), 
        _T( "Communication Control" ) );
}


// =============================================================================
void GEVPlayerDlg::OnBnClickedStreamparamsButton()
{
    if ( !mStream.IsOpen() )
    {
        return;
    }
    
    ShowGenWindow( 
        & mStreamParametersWnd, 
        mStream.GetParameters(), 
        _T( "Image Stream Control" ) );
}


// =============================================================================
void GEVPlayerDlg::ShowGenWindow( PvGenBrowserWnd *aWnd, PvGenParameterArray *aParams, const CString &aTitle )
{
    if ( aWnd->GetHandle() != 0 )
    {
        // If already open, just toggle to closed...     
        CloseGenWindow( aWnd );
        return;
    }
    
    // Create, assigne parameters, set title and show modeless
    aWnd->SetTitle( PvString( aTitle ) );
    aWnd->SetGenParameterArray( aParams );
    aWnd->ShowModeless( GetSafeHwnd() );
}


// =============================================================================
void GEVPlayerDlg::CloseGenWindow( PvGenBrowserWnd *aWnd )
{
    // If the window object is currently created (visible), close/destroy it
    if ( ( aWnd )->GetHandle() != 0 )
    {
        ( aWnd )->Close();
    }
}


// =============================================================================
void GEVPlayerDlg::StartStreaming()
{
    CSingleLock lLock( &mStartStreamingMutex );

    if ( mThreadDisplay == NULL )
    {
        // Create display thread
        mThreadDisplay = new ThreadDisplay( this, &mDisplay, &mDevice, &mPipeline, &mFilteringDlg, mImageSaveDlg, mLogBuffer );

        // Apply any cached display options
        ApplyDisplayOptions();

        // Start threads
        mThreadDisplay->Start();
        mThreadDisplay->SetPriority( THREAD_PRIORITY_ABOVE_NORMAL );
    }

    if ( !mPipeline.IsStarted() )
    {
        mPipeline.Start();
    }
}


// =============================================================================
void GEVPlayerDlg::StopStreaming()
{
    CSingleLock lLock( &mStartStreamingMutex );

    if ( mThreadDisplay != NULL )
    {
		// Signal the thread to stop the abort queued stream buffers
		mThreadDisplay->SignalStop();
		if ( mPipeline.IsStarted() )
		{
			mPipeline.Stop();
		}

	    // Wait on display thread to be completed
        mThreadDisplay->Stop();

        delete mThreadDisplay;
        mThreadDisplay = NULL;
    }

}

// =============================================================================
void GEVPlayerDlg::StartAcquisition()
{
    CSingleLock lLock( &mStartAcquisitionMutex );
    
    PvGenEnum *lMode = dynamic_cast<PvGenEnum *>( mDevice.GetGenParameters()->Get( "AcquisitionMode" ) );
    if ( lMode == NULL )
    {
        MessageBox( _T( "AcquisitionMode parameter missing." ), _T("GEVPlayer") );
        return;
    }

    PvGenCommand *lStart = dynamic_cast<PvGenCommand *>( mDevice.GetGenParameters()->Get( "AcquisitionStart" ) );
    if ( lStart == NULL )
    {
        MessageBox( _T( "AcquisitionStart parameter missing." ), _T("GEVPlayer") );
        return;
    }

    PvGenInteger *lTLParamsLocked = dynamic_cast<PvGenInteger *>( mDevice.GetGenParameters()->Get( "TLParamsLocked" ) );
    PvGenCommand *lResetStats = dynamic_cast<PvGenCommand *>( mStream.GetParameters()->Get( "Reset" ) );

    if ( mStream.IsOpen() )
    {
        PvUInt32 lPayloadSize = GetPayloadSize();
        if ( lPayloadSize > 0 )
        {
            mPipeline.SetBufferSize( lPayloadSize );
        }

        mPipeline.Reset();
    }

    lResetStats->Execute();

    if ( mThreadDisplay != NULL )
    {
        mThreadDisplay->ResetStats();
    }

    if ( mImageSaveDlg != NULL )
    {
        mImageSaveDlg->ResetStats();
    }

    PvResult lResult = PvResult::Code::NOT_INITIALIZED;

    PvString lStr;
    lMode->GetValue( lStr );
    CString lModeStr = lStr.GetUnicode();
    if ( lModeStr.Find( _T( "Continuous" ) ) >= 0 )
    {
        // We are streaming, lock the TL parameters
        if ( lTLParamsLocked != NULL ) 
        {
            lResult = lTLParamsLocked->SetValue( 1 );
        }

        lStart->Execute();
        mAcquiringImages = true;
    }
    else if ( ( lModeStr.Find( _T( "Multi" ) ) >= 0 ) || 
              ( lModeStr.Find( _T( "Single" ) ) >= 0 ) )
    {
        // We are streaming, lock the TL parameters
        if ( lTLParamsLocked != NULL ) 
        {
            lTLParamsLocked->SetValue( 1 );
        }

        lResult = lStart->Execute();

        // We are done streaming, unlock the TL parameters
        if ( lTLParamsLocked != NULL ) 
        {
            lTLParamsLocked->SetValue( 0 );
        }
    }
}


// =============================================================================
void GEVPlayerDlg::StopAcquisition()
{
    CSingleLock lLock( &mStartAcquisitionMutex );

    PvResult lResult;

    // Execute AcquisitionStop command, if it exists
    PvGenCommand *lStop = dynamic_cast<PvGenCommand *>( mDevice.GetGenParameters()->Get( "AcquisitionStop" ) );
    if ( lStop != NULL )
    {
        lResult = lStop->Execute();
    }

    // TLParamsLocked, if it exists
    PvGenInteger *lTLParamsLocked = dynamic_cast<PvGenInteger *>( mDevice.GetGenParameters()->Get( "TLParamsLocked" ) );
    if ( lTLParamsLocked != NULL )
    {
        lResult = lTLParamsLocked->SetValue( 0 );
    }
}


// =============================================================================
void GEVPlayerDlg::OnBnClickedStart()
{
    if ( !mDeviceConnected )
    {
        return;
    }

    PvGenBoolean *lRecoveryEnabled = dynamic_cast<PvGenBoolean *>( mDevice.GetGenLink()->Get( _T( "LinkRecoveryEnabled" ) ) );

    bool lEnabled = true;
    if ( lRecoveryEnabled != NULL )
    {
        lRecoveryEnabled->GetValue( lEnabled );
    }

    if ( lEnabled )
    {
        // Take a backup of the device configuration
        PvGenCommand *lRestore = dynamic_cast<PvGenCommand *>( mDevice.GetGenLink()->Get( _T( "StoreConfiguration" ) ) );
        if ( lRestore != NULL )
        {
            lRestore->Execute();
        }
    }

    // Reset image timeouts counters used for recovery
    mDevice.ResetStreamTimeoutCounter();

    StartAcquisition();

    EnableInterface();
}


// =============================================================================
void GEVPlayerDlg::OnBnClickedStop()
{
    if ( !mDeviceConnected )
    {
        return;
    }

    mAcquiringImages = false;
    SetStatusColor( SCDefault );
    mStatusTextBox.SetWindowText( _T( "" ) );

    PvGenCommand *lStop = dynamic_cast<PvGenCommand *>( mDevice.GetGenParameters()->Get( "AcquisitionStop" ) );
    ASSERT( lStop != NULL ); // Mandatory parameter

    bool lAvailable = false;
    if ( lStop->IsAvailable( lAvailable ).IsOK() )
    {
        StopAcquisition();
    }

    EnableInterface();
}


// =============================================================================
void GEVPlayerDlg::OnCbnSelchangeMode()
{
    if ( !mDeviceConnected )
    {
        return;
    }

    if ( mModeCombo.GetCurSel() < 0 )
    {
        return;
    }

    PvGenEnum *lMode = dynamic_cast<PvGenEnum *>( mDevice.GetGenParameters()->Get( "AcquisitionMode" ) );
    if ( lMode == NULL )
    {
        return;
    }

    PvUInt64 lValue = mModeCombo.GetItemData( mModeCombo.GetCurSel() );
    PvResult lResult = lMode->SetValue( lValue );
    if ( !lResult.IsOK() )
    {
        PvMessageBox( this, lResult );
    }
}


// =============================================================================
LRESULT GEVPlayerDlg::OnImageDisplayed( WPARAM wParam, LPARAM lParam )
{
    // Stream opened, image save dlg exists, thread display is up
    if ( ( !mStream.IsOpen() ) ||
         ( mImageSaveDlg == NULL ) ||
         ( mThreadDisplay == NULL ) )
    {
        return 0;
    }

    PvInt64 lCount = 0;
    CString lImagesCount = _T( "N/A" );
    CString lAvgFPS = _T( "N/A" );
    CString lBandwidth = _T( "N/A" );
    CString lStr;
    CString lDisplayFPS = _T( "N/A" );

    if ( mImageSaveDlg->GetSaveEnabled() )
    {
        SetStatusColor( SCRed );

        if ( mImageSaveDlg->GetFrames() > 0 )
        {
            lAvgFPS.Format( _T( "%.01f" ), mImageSaveDlg->GetFPS() );
        }

        lStr.Format( _T( "RECORDING %d images    %I64i MB    %s FPS    %.01f Mbps"), mImageSaveDlg->GetFrames(), 
            mImageSaveDlg->GetTotalSize(), lAvgFPS, mImageSaveDlg->GetMbps() );
        mStatusTextBox.SetWindowText( lStr );
    }
    else
    {
        PvGenInteger *lInteger = dynamic_cast<PvGenInteger *>(
            mStream.GetParameters()->Get( "ImagesCount" ) );
        if ( lInteger != NULL )
        {
            lInteger->GetValue( lCount );
            lImagesCount.Format( _T( "%i" ), lCount );
        }

        if ( lCount > 1 )
        {
            PvGenFloat *lFloat = dynamic_cast<PvGenFloat *>(
                mStream.GetParameters()->Get( "AcquisitionRate" ) );
            if ( lFloat != NULL )
            {
                double lValue = 0.0;
                lFloat->GetValue( lValue );
                lAvgFPS.Format( _T( "%.01f" ), lValue );
            }

            lFloat = dynamic_cast<PvGenFloat *>(  
                mStream.GetParameters()->Get( "Bandwidth" ) );
            if ( lFloat != NULL )
            {
                double lValueD = 0.0;
                lFloat->GetValue( lValueD );

                // Convert from bps to Mbps and display
                lBandwidth.Format( _T( "%.01f" ), lValueD / 1000000.0 );
            }

            int lRate = mThreadDisplay->GetFrameRate();
            if ( lRate > 0.0 )
            {
                lDisplayFPS.Format( _T( "%i" ), lRate );
            }
        }
    
        CString lStr;
        lStr.Format( _T( "Stream: %s images    %s FPS    %s Mbps     Display: %s FPS" ), 
            lImagesCount, lAvgFPS, lBandwidth, lDisplayFPS );

        SetStatusColor( SCDefault );
        mStatusTextBox.SetWindowText( lStr + _T( "\r\n" ) + GetErrorString() );
    }

    return 0;
}


// =============================================================================
CString GEVPlayerDlg::GetErrorString()
{
    CString lErrorCount = _T( "" );
    CString lLastError = _T( "" );

    // Error count
    PvGenInteger *lInteger = dynamic_cast<PvGenInteger *>(
        mStream.GetParameters()->Get( "ErrorCount" ) );
    if ( lInteger != NULL )
    {
        PvInt64 lCount;
        lInteger->GetValue( lCount );

        lErrorCount.Format( _T( "%I64i" ), lCount );
    }

    // Last error
    PvGenEnum *lEnum = dynamic_cast<PvGenEnum *>(
        mStream.GetParameters()->Get( "LastError" ) );
    if ( lEnum != NULL )
    {
        PvInt64 lValue;
        lEnum->GetValue( lValue );
        if ( lValue == PvResult::Code::OK )
        {
            lLastError = _T( "N/A" );
        }
        else
        {
            PvString lError;
            lEnum->GetValue( lError );

            lLastError = lError.GetUnicode();
        }
    }

    CString lErrorStr;
    lErrorStr.Format( _T( "Error count: %s    Last error: %s" ),
        lErrorCount, lLastError );

    std::vector<CString> lWarnings;

    // Packet resends have been used
    lInteger = dynamic_cast<PvGenInteger *>(
        mStream.GetParameters()->Get( "ExpectedResend" ) );
    if ( lInteger != NULL )
    {
        PvInt64 lCount;
        lInteger->GetValue( lCount );

        if ( lCount > 0 )
        {
            lWarnings.push_back( _T( "resends" ) );
        }
    }

    // There is at least one missing block ID
    lInteger = dynamic_cast<PvGenInteger *>(
        mStream.GetParameters()->Get( "BlockIDsMissing" ) );
    if ( ( lInteger != NULL ) && lInteger->IsReadable() )
    {
        PvInt64 lCount;
        lInteger->GetValue( lCount );

        if ( lCount > 0 )
        {
            lWarnings.push_back( _T( "missing block IDs" ) );
        }
    }
    
    // Buffers dropped from pipeline output queue?
    lInteger = dynamic_cast<PvGenInteger *>(
        mStream.GetParameters()->Get( "PipelineImagesDropped" ) );
    if ( ( lInteger != NULL ) && lInteger->IsReadable() )
    {
        PvInt64 lCount;
        lInteger->GetValue( lCount );

        if ( lCount > 0 )
        {
            lWarnings.push_back( _T( "pipeline dropped images" ) );
        }
    }
    
    // There is at least one image that was dropped on the device
    lInteger = dynamic_cast<PvGenInteger *>(
        mStream.GetParameters()->Get( "ImagesDropped" ) );
    if ( ( lInteger != NULL ) && lInteger->IsReadable() )
    {
        PvInt64 lCount;
        lInteger->GetValue( lCount );

        if ( lCount > 0 )
        {
            lWarnings.push_back( _T( "device dropped images" ) );
        }
    }

    // Partial line missing
    lInteger = dynamic_cast<PvGenInteger *>(
        mStream.GetParameters()->Get( "PartialLineMissing" ) );
    if ( ( lInteger != NULL ) && lInteger->IsReadable() )
    {
        PvInt64 lCount;
        lInteger->GetValue( lCount );

        if ( lCount > 0 )
        {
            lWarnings.push_back( _T( "partial lines missing" ) );
        }
    }

    // Full line missing
    lInteger = dynamic_cast<PvGenInteger *>(
        mStream.GetParameters()->Get( "FullLineMissing" ) );
    if ( ( lInteger != NULL ) && lInteger->IsReadable() )
    {
        PvInt64 lCount;
        lInteger->GetValue( lCount );

        if ( lCount > 0 )
        {
            lWarnings.push_back( _T( "full lines missing" ) );
        }
    }

    if ( lWarnings.size() <= 0 )
    {
        return lErrorStr;
    }

    // Retrieve status box size
    CRect lRect;
    mStatusTextBox.GetClientRect( &lRect );

    // Get current error string size
    CDC lDC;
    lDC.Attach( mStatusTextBox.GetDC()->m_hDC );
    lDC.SelectObject( mStatusTextBox.GetFont() );

    // Set text...

    CString lFinalStr = lErrorStr + _T( "    Warnings: " );
    std::vector<CString>::iterator lIt = lWarnings.begin();
    while ( lIt != lWarnings.end() )
    {
        CString lTestStr = lFinalStr + *( lIt ) + _T( ", ..." );
        CSize lSize = lDC.GetTextExtent( lTestStr );
        if ( ( lSize.cx + 10 ) <= lRect.Width() )
        {
            lFinalStr += *( lIt++ );
            if ( lIt != lWarnings.end() )
            {
                lFinalStr += _T( ", " );
            }
        }
        else
        {
            lFinalStr += _T( "... " );
            break;
        }
    }

    return lFinalStr;
}


// =============================================================================
void GEVPlayerDlg::OnParameterUpdate( PvGenParameter *aParameter )
{
    PvString lName;
    if ( !aParameter->GetName( lName ).IsOK() )
    {
        ASSERT( 0 ); // Totally unexpected	
        return;
    }
/*
    // If the AcquisitionStop node is updated, make sure to reflect its new state on the UI
    if ( ( lName == "AcquisitionStop" ) &&
         ( mStopButton.GetSafeHwnd() != 0 ) )
    {
        bool lAvailable = false;
        if ( aParameter->IsAvailable( lAvailable ).IsOK() )
        {
            mStopButton.EnableWindow( lAvailable );
        }
    }
    */
    if ( ( lName == "AcquisitionMode" ) &&
         ( mModeCombo.GetSafeHwnd() != 0 ) )
    {
        bool lAvailable = false, lWritable = false;
        VERIFY( aParameter->IsAvailable( lAvailable ).IsOK() );
        if ( lAvailable )
        {
            VERIFY( aParameter->IsWritable( lWritable ).IsOK() );
        }

        mModeCombo.EnableWindow( lAvailable && lWritable );

        PvGenEnum *lEnum = dynamic_cast<PvGenEnum *>( aParameter );
        ASSERT( lEnum != NULL );

        if ( lEnum != NULL )
        {
            PvInt64 lEEValue = 0;
            VERIFY( lEnum->GetValue( lEEValue ) );		

            for ( int i = 0; i < mModeCombo.GetCount(); i++ )
            {
                DWORD_PTR lData = mModeCombo.GetItemData( i );
                if ( lData == lEEValue )
                {
                    mModeCombo.SetCurSel( i );
                    break;
                }
            }
        }
    }
}


// =============================================================================
void GEVPlayerDlg::OnFileLoad()
{
    CFileDialog lFileDlg( TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, L"PureGEV Configuration Files (*.pvcfg)|*.pvcfg|All files (*.*)|*.*||", this);

    lFileDlg.m_ofn.lpstrTitle = L"Open PureGEV Project";

    if ( lFileDlg.DoModal() == IDOK)
    {
        mFileName = lFileDlg.GetPathName();
        OpenConfig( mFileName );  
    }
}


// =============================================================================
void GEVPlayerDlg::OnFileSave()
{
    if ( mFileName.IsEmpty() )
    {
        OnFileSaveAs();
        return;
    }
    
    SaveConfig( mFileName, true );

    ReportMRU( mFileName );
}


// =============================================================================
void GEVPlayerDlg::OnFileExit()
{
    Disconnect();
    EndDialog( IDOK );
}

void GEVPlayerDlg::OnRecent( UINT nID )
{
    unsigned int lIndex = nID - ID_FILE_RECENT1;
    std::list<CString>::iterator lIt;
    lIt = mRecentList.begin();
    unsigned int i = 0;
    while( lIt != mRecentList.end() && i <= lIndex )
    {
        if ( i == lIndex )
        {
            mFileName = *lIt;
        }
        lIt++;
        i++;
    }

    OpenConfig( mFileName );
}


// =============================================================================
void GEVPlayerDlg::OnHelpEventmonitor()
{
    if ( !mDeviceConnected )
    {
        return;
    }

    ASSERT( mEventMonitorDlg != NULL );
    if ( mEventMonitorDlg == NULL )
    {
        return;
    }

    ASSERT( mEventMonitorDlg->GetSafeHwnd() != 0 );
    if ( mEventMonitorDlg->GetSafeHwnd() == 0 )
    {
        return;
    }

    if ( mEventMonitorDlg->IsWindowVisible() )
    {
        mEventMonitorDlg->ShowWindow( SW_HIDE );
    }
    else
    {
        mEventMonitorDlg->ShowWindow( SW_SHOW );
        mEventMonitorDlg->BringWindowToTop();
        mEventMonitorDlg->SetFocus();
    }
}


// =============================================================================
void GEVPlayerDlg::OnHelpAbout()
{
#ifdef PT_INTERNAL
    AboutBox::Show();
#endif //PT_INTERNAL
}


// =============================================================================
void GEVPlayerDlg::OnMove(int x, int y)
{
    CDialog::OnMove(x, y);

    mDisplay.UpdateBounds();
}

    
// =============================================================================
void GEVPlayerDlg::OnInitMenu(CMenu* pMenu)
{
    CDialog::OnInitMenu(pMenu);

    for ( unsigned int i = 0; i < pMenu->GetMenuItemCount(); i++ )
    {
        UINT lID = pMenu->GetMenuItemID( i );
        if ( lID == -1 )
        {
            CMenu *lSubMenu = pMenu->GetSubMenu( i );
            for ( unsigned int j = 0; j < lSubMenu->GetMenuItemCount(); j++ )
            {  
                UINT lID = lSubMenu->GetMenuItemID( j );
                if ( lID != -1 )
                {
                    switch ( lID )
                    {
                    case ID_HELP_EVENTMONITOR:

                        lSubMenu->EnableMenuItem( j, MF_BYPOSITION | 
                            ( ( mEventMonitorDlg != NULL ) ? 
                                MF_ENABLED : 
                                MF_GRAYED ) );
                        lSubMenu->CheckMenuItem( j, MF_BYPOSITION |
                            ( ( ( mEventMonitorDlg != NULL ) && mEventMonitorDlg->IsWindowVisible() ) ? 
                                MF_CHECKED : 
                                MF_UNCHECKED ) );
                        break;

                    case ID_TOOLS_DISPLAYVSYNC:

                        lSubMenu->EnableMenuItem( j, MF_BYPOSITION | 
                            ( ( mThreadDisplay != NULL ) ? 
                                MF_ENABLED : 
                                MF_GRAYED ) );
                        lSubMenu->CheckMenuItem( j, MF_BYPOSITION | 
                                ( mVSync ? 
                                MF_CHECKED : 
                                MF_UNCHECKED ) );
                        break;

                    case ID_TOOLS_DISPLAYPARTIALIMAGES:
                        lSubMenu->EnableMenuItem( j, MF_BYPOSITION | 
                            ( ( mThreadDisplay != NULL ) ? 
                                MF_ENABLED : 
                                MF_GRAYED ) );
                        lSubMenu->CheckMenuItem( j, MF_BYPOSITION | 
                                ( mKeepPartialImages ? 
                                MF_CHECKED : 
                                MF_UNCHECKED ) );

                    case ID_TOOLS_SAVEIMAGES:

                        lSubMenu->EnableMenuItem( j, MF_BYPOSITION | 
                            ( ( mThreadDisplay != NULL ) ? 
                                MF_ENABLED : 
                                MF_GRAYED ) );
                        break;

                    case ID_TOOLS_SAVECURRENTIMAGE:
                        lSubMenu->EnableMenuItem( j, MF_BYPOSITION | 
                            ( ( mThreadDisplay != NULL ) ? 
                                MF_ENABLED : 
                                MF_GRAYED ) );
                        break;

                    case ID_TOOLS_BUFFEROPTIONS:
                        lSubMenu->EnableMenuItem( j, MF_BYPOSITION | 
                            ( ( mThreadDisplay != NULL ) ? 
                                MF_ENABLED : 
                                MF_GRAYED ) );
                        break;

                    case ID_FILE_SAVEAS:
                    case ID_FILE_SAVE:
                        lSubMenu->EnableMenuItem( j, MF_BYPOSITION | 
                            ( ( mDeviceConnected || mStream.IsOpen() ) ? 
                                MF_ENABLED : 
                                MF_GRAYED ) );
                        break;

                    case ID_FILE_LOAD:
                        lSubMenu->EnableMenuItem( j, MF_BYPOSITION | 
                            ( ( ( mCommunicationWnd.GetHandle() == 0 ) && ( mDeviceWnd.GetHandle() == 0 ) && ( mStreamParametersWnd.GetHandle() == 0 ) ) ? 
                                MF_ENABLED : 
                                MF_GRAYED ) );
                        break;

                    case ID_TOOLS_IMAGEFILTERING:
                        lSubMenu->EnableMenuItem( j, MF_BYPOSITION | 
                            ( ( mStream.IsOpen() ) ? 
                                MF_ENABLED : 
                                MF_GRAYED ) );
                        lSubMenu->CheckMenuItem( j, MF_BYPOSITION |
                            ( ( mFilteringDlg.IsWindowVisible() ) ? 
                                MF_CHECKED : 
                                MF_UNCHECKED ) );
                        break;

                    case ID_TOOLS_IPENGINESERIALCOMMUNICATION:
                        lSubMenu->EnableMenuItem( j, MF_BYPOSITION |
                            ( ( mDevice.IsConnected() && mSerial0IPEngineSupported ) ? 
                                MF_ENABLED : 
                                MF_GRAYED ) );
                        lSubMenu->CheckMenuItem( j, MF_BYPOSITION |
                            ( ( mTerminalIPEngineWnd.GetHandle() != 0 ) ?
                                MF_CHECKED : 
                                MF_UNCHECKED ) );
                        break;
                    }
                }
                else
                {
                    CMenu *lSubMenu2 = lSubMenu->GetSubMenu( j );
                    for ( unsigned int k = 0; k < lSubMenu2->GetMenuItemCount(); k++ )
                    {  
                        UINT lID = lSubMenu2->GetMenuItemID( k );
                        switch ( lID )
                        {
                        case ID_DISPLAYIMAGES_DISABLED:
                            lSubMenu2->EnableMenuItem( k, MF_BYPOSITION | 
                                ( ( mThreadDisplay != NULL ) ? 
                                    MF_ENABLED : 
                                    MF_GRAYED ) );
                            lSubMenu2->CheckMenuItem( k, MF_BYPOSITION |
                                    ( ( mTargetFPS == -1 ) ?
                                    MF_CHECKED : 
                                    MF_UNCHECKED ) );
                            break;

                        case ID_DISPLAYIMAGES_30FPS:
                            lSubMenu2->EnableMenuItem( k, MF_BYPOSITION | 
                                ( ( mThreadDisplay != NULL ) ? 
                                    MF_ENABLED : 
                                    MF_GRAYED ) );
                            lSubMenu2->CheckMenuItem( k, MF_BYPOSITION |
                                ( ( mTargetFPS == 30 ) ?
                                    MF_CHECKED : 
                                    MF_UNCHECKED ) );
                            break;

                        case ID_DISPLAYIMAGES_60FPS:
                            lSubMenu2->EnableMenuItem( k, MF_BYPOSITION | 
                                ( ( mThreadDisplay != NULL ) ? 
                                    MF_ENABLED : 
                                    MF_GRAYED ) );
                            lSubMenu2->CheckMenuItem( k, MF_BYPOSITION |
                                ( ( mTargetFPS == 60 ) ?
                                    MF_CHECKED : 
                                    MF_UNCHECKED ) );
                            break;
                        }
                    }
                }
            }
        }
    }
}


// =============================================================================
void GEVPlayerDlg::OnToolsDisplaysync()
{
    mVSync = !mVSync;
    ApplyDisplayOptions();
}


// =============================================================================
HBRUSH GEVPlayerDlg::OnCtlColor( CDC* pDC, CWnd* pWnd, UINT nCtlColor )
{
    switch ( pWnd->GetDlgCtrlID() )
    {
    case IDC_STATUS:    
        switch ( mStatusColor )
        {
        case SCDefault:
            break;

        case SCYellow:
            pDC->SetBkColor( mYellowColor );
            return mYellowBrush;

        case SCRed:
            pDC->SetTextColor( RGB( 0xFF, 0xFF, 0xFF ) );
            pDC->SetBkColor( mRedColor );
            return mRedBrush;
        }
        break;
    }

    return CDialog::OnCtlColor( pDC, pWnd, nCtlColor );
}


// =============================================================================
void GEVPlayerDlg::OnRestoreConfiguration( PvDevice *aDevice )
{
    TRACE( _T( "==> OnRestoreConfiguration\n" ) );
    
    SetStatusColor( SCYellow );
    mStatusTextBox.SetWindowText( _T( "Restoring configuration..." ) );

    // Important - this will, amongst other, make sure TLParamsLocked is set to 0!
    StopAcquisition();

    PvGenBoolean *lRecoveryEnabled = dynamic_cast<PvGenBoolean *>( mDevice.GetGenLink()->Get( _T( "LinkRecoveryEnabled" ) ) );

    bool lEnabled = true;
    if ( lRecoveryEnabled != NULL )
    {
        lRecoveryEnabled->GetValue( lEnabled );
    }

    if ( lEnabled )
    {
        PvGenCommand *lRestore = dynamic_cast<PvGenCommand *>( mDevice.GetGenLink()->Get( _T( "RestoreConfiguration" ) ) );
        if ( lRestore != NULL )
        {
            lRestore->Execute();
        }
    }
}


// =============================================================================
void GEVPlayerDlg::OnRestoreDevice( PvDevice *aDevice )
{
    TRACE( _T( "==> OnRestoreDevice\n" ) );
    
    SetStatusColor( SCYellow );
    mStatusTextBox.SetWindowText( _T( "Resetting device..." ) );

    // Try finding a DeviceReset command in the node map
    PvGenCommand *lReset = dynamic_cast<PvGenCommand *>( mDevice.GetGenParameters()->Get( "DeviceReset" ) );
    if ( lReset != NULL )
    {
        // Resetting the device
        lReset->Execute();
    }
}


// =============================================================================
void GEVPlayerDlg::OnRestoreAcquisition( PvDevice *aDevice )
{
    TRACE( _T( "==> OnRestoreAcquisition\n" ) );
    
    if ( mAcquiringImages )
    {
        SetStatusColor( SCYellow );
        mStatusTextBox.SetWindowText( _T( "Restarting image acquisition..." ) );
    }
    else
    {
        SetStatusColor( SCDefault );
        mStatusTextBox.SetWindowText( _T( "" ) );
    }

    PostMessage( WM_STREAM );
}    


// =============================================================================
void GEVPlayerDlg::OnLinkDisconnected( PvDevice *aDevice )
{
    TRACE( _T( "==> OnLinkDisconnected\n" ) );
    
    mDisconnectTime = ::GetTickCount();

    PostMessage( WM_STOPSTREAMING );
    PostMessage( WM_DISCONNECT );
}


// =============================================================================
void GEVPlayerDlg::OnLinkReconnected( PvDevice *aDevice )
{
    TRACE( _T( "==> OnLinkReconnected\n" ) );
    
    PvGenInteger *lInteger = dynamic_cast<PvGenInteger *>( mDevice.GetGenParameters()->Get( "GevCurrentIPAddress" ) );
    ASSERT( lInteger != NULL );

    PvInt64 lValue;
    lInteger->GetValue( lValue );

    CString lIP;
    lIP.Format( _T( "%I64i.%I64i.%I64i.%I64i" ),
        ( lValue & 0xFF000000 ) >> 24,
        ( lValue & 0x00FF0000 ) >> 16,
        ( lValue & 0x0000FF00 ) >> 8,
        ( lValue & 0x000000FF ) );

    CString lOldIP;
    mIPEdit.GetWindowText( lOldIP );
    mNewIP = ( lIP != lOldIP );

    mIPEdit.SetWindowText( lIP );
}


// =============================================================================
LRESULT GEVPlayerDlg::OnStream( WPARAM wParam, LPARAM lParam )
{
    TRACE( _T( "GEVPlayerDlg::OnStream( W, L )\n" ) );
    
    if ( !mDevice.IsConnected() )
    {
        return 0;
    }
    
    //
    // Only re-open stream if control and data.
    //     - If control only, no need for stream
    //     - If data only, there is no recovery required
    //     - If monitor, well there is no stream!
    //

    if ( mSetupDlg.GetRole() == SetupDlg::RoleCtrlData )
    {
        // Added for improved reliability
        if ( mStream.IsOpen() )
        {
            mDisconnectTime = ::GetTickCount();
            mStream.Close();
        }

        if ( mNewIP )
        {
            // Make sure we wait for the FDO to unload...
            while ( ( ::GetTickCount() - mDisconnectTime ) < 20000 )
            {
                ::Sleep( 250 );
            }
        }

        PvGenInteger *lInteger = dynamic_cast<PvGenInteger *>( mDevice.GetGenParameters()->Get( "GevCurrentIPAddress" ) );
        ASSERT( lInteger != NULL );

        PvInt64 lValue;
        lInteger->GetValue( lValue );

        CString lIP;
        lIP.Format( _T( "%I64i.%I64i.%I64i.%I64i" ),
            ( lValue & 0xFF000000 ) >> 24,
            ( lValue & 0x00FF0000 ) >> 16,
            ( lValue & 0x0000FF00 ) >> 8,
            ( lValue & 0x000000FF ) );

        PvResult lResult = PvResult::Code::GENERIC_ERROR;
        switch ( mSetupDlg.GetDestination() )
        {
            case SetupDlg::DestinationUnicastAuto:
                lResult = mStream.Open( (LPCTSTR)lIP );
                break;

            case SetupDlg::DestinationUnicastOther:
                // "Other" is not us, no need to open the stream
                break;

            case SetupDlg::DestinationMulticast:
                // Bug 2491, added device IP
                lResult = mStream.Open( (LPCTSTR)lIP, (LPCTSTR)mSetupDlg.GetMulticastIP(), mSetupDlg.GetMulticastPort() );
                break;

            default:
                ASSERT( 0 );
        }

        if ( lResult.IsOK() )
        {
            mDevice.SetStreamDestination( mStream.GetLocalIPAddress(), mStream.GetLocalPort() );
            StartStreaming();

            if ( mAcquiringImages )
            {
                StartAcquisition();
            }
        }
    }

    return 0;
}


// =============================================================================
LRESULT GEVPlayerDlg::OnDisconnect( WPARAM wParam, LPARAM lParam )
{
    TRACE( _T( "GEVPlayerDlg::OnDisconnect( W, L )\n" ) );
    
    // Is reconnecting enabled?
    PvGenBoolean *lLinkRecoveryEnabled = dynamic_cast<PvGenBoolean *>( mDevice.GetGenLink()->Get( "LinkRecoveryEnabled"  ) );
    ASSERT( lLinkRecoveryEnabled != NULL );

    bool lEnabled = false;
    lLinkRecoveryEnabled->GetValue( lEnabled );

    if ( lEnabled )
    {
        SetStatusColor( SCYellow );
        mStatusTextBox.SetWindowText( _T( "Connection to device lost..." ) );
    }
    else
    {
        SetStatusColor( SCDefault );
        mStatusTextBox.SetWindowText( _T( "" ) );

        CWaitCursor lCursor;
        Disconnect();

        MessageBox( _T( "Connection to device lost." ), _T( "GEVPlayer" ), MB_OK | MB_ICONINFORMATION );
    }

    return 0;
}


// =============================================================================
LRESULT GEVPlayerDlg::OnStopStreaming( WPARAM wParam, LPARAM lParam )
{
    TRACE( _T( "GEVPlayerDlg::OnStopStreaming( W, L )\n" ) );
    
    if ( mStream.IsOpen() )
    {
        StopStreaming();
        mStream.Close();
    }

    return 0;
}


// =============================================================================
void GEVPlayerDlg::OnToolsSetup()
{
    mSetupDlg.SetEnabled( !mDeviceConnected && !mStream.IsOpen() );
    mSetupDlg.DoModal();
    mSetupDlg.DestroyWindow();

    EnableInterface();
}


// =============================================================================
void GEVPlayerDlg::OnToolsImagefiltering()
{
    if ( mFilteringDlg.IsWindowVisible() )
    {
        mFilteringDlg.ShowWindow( SW_HIDE );
    }
    else
    {
        mFilteringDlg.ShowWindow( SW_SHOW );
        mFilteringDlg.BringWindowToTop();
        mFilteringDlg.SetFocus();
    }
}


// =============================================================================
void GEVPlayerDlg::OnFileSaveAs()
{
    CFileDialog lFileDlg( FALSE, L".pvcfg", NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, L"PureGEV Configuration Files (*.pvcfg)|*.pvcfg|All files (*.*)|*.*||", this);

    lFileDlg.m_ofn.lpstrTitle = L"Save PureGEV Project";

    if ( lFileDlg.DoModal() == IDOK)
    {
        mFileName = lFileDlg.GetPathName(); // This is your selected file name with path
        
        OnFileSave();
    }
}


// =============================================================================
void GEVPlayerDlg::OpenConfig( CString aFileName )
{
    if ( mCommunicationWnd.GetHandle() != 0 )
    {
        MessageBox( _T( "Please close the communication control window before loading a new configuration.") );
        return;
    }
    if ( mStreamParametersWnd.GetHandle() != 0 )
    {
        MessageBox( _T( "Please close the image stream control window before loading a new configuration.") );
        return;
    }
    if ( mDeviceWnd.GetHandle() != 0 )
    {
        MessageBox( _T( "Please close the GEV Device control window before loading a new configuration.") );
        return;
    }

    // Create reader, load file
    PvConfigurationReader lReader;
    PvResult lResult = lReader.Load( aFileName.GetBuffer() );
    if ( !lResult.IsOK() )
    {
        PvMessageBox( this, lResult );
        return;
    }

    // we're loading a new persistence file. Report it now so that it's visible in the title bar while we're loading.
    if ( !mFileName.IsEmpty() && aFileName == mFileName )
    {
        ReportMRU( aFileName );
    }

    // Check if roles are the same if not, we must disconnect first
    if ( ( mDeviceConnected || mStream.IsOpen() ) && 
         !mSetupDlg.IsTheSame( lReader ) )
    {
        if ( MessageBox( _T( "Setup of this configuration file does not match the current ")
                         _T( "connection.\r\n\r\nIn order to proceed with the operation, ")
                         _T( "the GigE Vision Device will be disconnected.\r\n\r\nContinue ")
                         _T( "restoring configuration?"), 
                         _T( "GEVPlayer" ), MB_YESNO | MB_ICONQUESTION ) == IDNO )
        {
            return;
        }    

        Disconnect();
    }

    mSetupDlg.Load( lReader );
    mFilteringDlg.Load( lReader );
    
    // Data is in log buffer, but the UI is event monitor. Once the data is loaded,
    // notify the event monitor to refresh itself
    mLogBuffer->Load( lReader );
    if ( mEventMonitorDlg != NULL )
    {
        mEventMonitorDlg->LoadData();
    }

    if ( lReader.GetGenParameterArrayCount() > 0 )
    {
        lReader.Restore( "communication", *mDevice.GetGenLink() );
    }

    if ( lReader.GetDeviceCount() > 0 || lReader.GetStreamCount() > 0 )
    {
        // If we're connected, just apply the settings. Otherwise connect from the settings in the persistence file.
        if ( mDeviceConnected )
        {
            if ( MessageBox( _T( "Opening a file now will apply the settings to the currently connected " )
                             _T( "GigE Vision Device. \r\n\r\nIf you wish to connect to a different GigEVision Device associated " )
                             _T( "with the file, disconnect first, then open the file. \r\n\r\nContinue opening file?" ), 
                             _T( "GEVPlayer"), MB_YESNO | MB_ICONQUESTION ) == IDNO )
            {
                return;
            }
            LoadingThread lLoadingThread( &mSetupDlg, &lReader, &mDevice, &mStream, this );
            lLoadingThread.Load();
        }
        else
        {
            Connect( NULL, &lReader );
        }

        mDeviceConnected = mDevice.IsConnected();
        mAcquiringImages = false;
    }

    LoadDisplayOptions( lReader );

    mImageSaveDlg->Load( lReader );

    // note that these items weren't saved in versions of GEVPlayer prior to 1.2 so we can't count on them being there
    PvUInt32 lValue;
    PvString lCommunicationVisibility;
    if ( lReader.Restore(PvString("communicationvisibility"), lCommunicationVisibility ).IsOK() )
    {
        sscanf_s( lCommunicationVisibility.GetAscii(), "%d", &lValue );
        mCommunicationWnd.SetVisibility( (PvGenVisibility) lValue );
    }
    PvString lDeviceVisibility;
    if ( lReader.Restore(PvString("devicevisibility"), lDeviceVisibility ).IsOK() )
    {
        sscanf_s( lDeviceVisibility.GetAscii(), "%d", &lValue );
        mDeviceWnd.SetVisibility( (PvGenVisibility) lValue );
    }
    PvString lStreamVisibility;
    if ( lReader.Restore(PvString("streamvisibility"), lStreamVisibility ).IsOK() )
    {
        sscanf_s( lStreamVisibility.GetAscii(), "%d", &lValue );
        mStreamParametersWnd.SetVisibility( (PvGenVisibility) lValue );
    }
    PvString lBufferCountStr;
    if ( lReader.Restore(PvString( TAG_STREAMINGBUFFERCOUNT ), lBufferCountStr ).IsOK() )
    {
        sscanf_s( lBufferCountStr.GetAscii(), "%d", &lValue );
        if ( lValue < BUFFER_COUNT_MIN )
        {
            lValue = BUFFER_COUNT_DEFAULT;
        }
        mPipeline.SetBufferCount( lValue );
    }

    EnableInterface();
}


// =============================================================================
void GEVPlayerDlg::ReportMRU(CString aFileName)
{

    //typedef CStringT< TCHAR, StrTraitATL< TCHAR > > CAtlString;
    CString resToken;
    int curPos= 0;
    CString lName;
    resToken= aFileName.Tokenize(_T("\\"),curPos);
    while (resToken != "")
    {
        lName = resToken;
        resToken= aFileName.Tokenize(_T("\\"),curPos);
    };
    SetWindowText( lName + _T(" - GEVPlayer") );

    // Try to find the item in our current list

    std::list<CString>::iterator lIt;
    std::list<CString>::iterator lToRemove;

    lIt = mRecentList.begin();
    bool lRemove = false;
    while (lIt != mRecentList.end())
    {
        if (*lIt == aFileName)
        {
            // If match, remove it, will be added as top latter
            lToRemove = lIt;
            lRemove = true;
        }
        lIt++;
    }

    if ( lRemove )
    {
        mRecentList.erase(lToRemove);
    }

    // Whether we removed already existing or not, insert at top
    mRecentList.push_front(aFileName);

    // trim the end (we don't want more than 10)
    if ( mRecentList.size() > 10 )
    {
        lIt = mRecentList.begin();
        std::list<CString>::iterator lLast;
        while (lIt != mRecentList.end())
        {
            lLast = lIt;
            lIt++;
        }
        // erase the back, not the front
        mRecentList.erase( lLast );
    }

    // Save current MRU to registry
    SaveMRUToRegistry();

    UpdateMRUMenu();
}


// =============================================================================
void GEVPlayerDlg::SaveMRUToRegistry()
{
    // iterate through the list

    std::list<CString>::iterator lIt;

    lIt = mRecentList.begin();

    CWinApp* pApp = AfxGetApp();

    UINT lCount = 0;
    while( lIt != mRecentList.end() ) 
    {

        wchar_t lNumber[4];
        swprintf_s(lNumber, 4, L"%d", lCount);

        wchar_t lToRegistry[MAX_PATH];
        swprintf_s(lToRegistry, MAX_PATH, L"%s", lIt->GetBuffer());
        BOOL lRet = pApp->WriteProfileString(_T("MRU"), lNumber, lToRegistry);
        lIt++;
        lCount++;
    }

}


// =============================================================================
void GEVPlayerDlg::LoadMRUFromRegistry()
{

    CString lData;
    DWORD lRet = 1;
    UINT lCount = 0;

    CWinApp* pApp = AfxGetApp();
    while( lData.Compare(_T("default")) != 0 )
    {
        wchar_t lNumber[4];
        swprintf_s(lNumber, 4, _T("%d"), lCount);
        lData = pApp->GetProfileString( _T("MRU"), lNumber, _T("default") );
        if ( lData.Compare(_T("default")) != 0 )
        {
           mRecentList.push_back( lData ); 
        }
        lCount++;
    }
}
 

// =============================================================================
void GEVPlayerDlg::UpdateMRUMenu()
{
    CMenu* mmenu = GetMenu();
    CMenu* lSubMenu = mmenu->GetSubMenu(0);
    if ( mMRUMenu == NULL )
    {
        mMRUMenu = new CMenu();
        mMRUMenu->CreatePopupMenu();
        lSubMenu->InsertMenu( 1, MF_BYPOSITION | MF_POPUP, (UINT_PTR)mMRUMenu->m_hMenu, L"Open Recent" );
    }
    if ( !mRecentList.empty() )
    {
        std::list<CString>::iterator lIt;
        lIt = mRecentList.begin();
        unsigned int i = 0;
        while( lIt != mRecentList.end() )
        {
            mMRUMenu->DeleteMenu( ID_FILE_RECENT1 + i, MF_BYCOMMAND );

            wchar_t lRecentString[MAX_PATH];
            swprintf_s( lRecentString, MAX_PATH, L"%d - %s", i+1, lIt->GetBuffer());

            mMRUMenu->AppendMenu(MF_STRING | MF_ENABLED, ID_FILE_RECENT1 + i, lRecentString );
            i++;
            lIt++;
        }
    }

}


// =============================================================================
BOOL GEVPlayerDlg::PreTranslateMessage(MSG* pMsg)
{
    if (WM_KEYFIRST <= pMsg->message && pMsg->message
                <= WM_KEYLAST)
        if (mAccel && ::TranslateAccelerator
            (m_hWnd, mAccel, pMsg))
            return TRUE;

    return CDialog::PreTranslateMessage(pMsg);
}


// =============================================================================
void GEVPlayerDlg::OnAcceleratorSave()
{
    OnFileSave();
}


// =============================================================================
void GEVPlayerDlg::OnAcceleratorOpen()
{
    OnFileLoad();
}


// =============================================================================
void GEVPlayerDlg::OnToolsSaveimages()
{
    ASSERT( mThreadDisplay != NULL );
    if ( mThreadDisplay == NULL )
    {
        return;
    }
    mImageSaveDlg->DoModal();
}


// =============================================================================
void GEVPlayerDlg::OnToolsSavecurrentimage()
{
    ASSERT( mThreadDisplay != NULL );
    if ( mThreadDisplay == NULL )
    {
        return;
    }
    mImageSaveDlg->SaveThisImage();
}


// =============================================================================
void GEVPlayerDlg::SetStatusColor( StatusColor aColor )
{
    mStatusColor = aColor;
    mStatusTextBox.Invalidate();
}


// =============================================================================
void GEVPlayerDlg::OnEvent( PvDevice *aDevice, PvUInt16 aEventID, PvUInt16 aChannel, PvUInt16 aBlockID, 
    PvUInt64 aTimestamp, const void *aData, PvUInt32 aDataLength )
{
/*
    FG 2009-06-17: Removed raw events from the monitor as it disrupted
                   serial communication...

    // Event monitor is expected to be created when the device is connected
    ASSERT( mEventMonitorDlg != NULL ); 

    mEventMonitorDlg->ReportEvent( aEventID, aChannel, aBlockID,
        aTimestamp, aData, aDataLength );
*/
}


// =============================================================================
void GEVPlayerDlg::OnToolsSerialCommunication()
{
    CloseGenWindow( &mCommunicationWnd );
    CloseGenWindow( &mDeviceWnd );
    CloseGenWindow( &mStreamParametersWnd );

    mTerminalIPEngineWnd.SetDevice( &mDevice );
    mTerminalIPEngineWnd.ShowModal( GetSafeHwnd() );
}


// ==============================================================================
void GEVPlayerDlg::OnTimer( UINT_PTR nIDEvent )
{
    if ( nIDEvent == 1 )
    {
        // Periodically refresh the status, helps catching
        // the last image in a sequence if it is filtered out
        // by the display frame rate limiter
        SendMessage( WM_IMAGEDISPLAYED );
    }

    CDialog::OnTimer( nIDEvent );
}


// ==========================================================================
void GEVPlayerDlg::OnDestroy()
{
    CDialog::OnDestroy();

    if ( mTimer != 0 )
    {
        KillTimer( 1 );
        mTimer = 0;
    }
}


// ==========================================================================
void GEVPlayerDlg::OnRegisterInterface()
{
    if ( !mDevice.IsConnected() )
    {
        return;
    }

    CloseGenWindow( &mCommunicationWnd );
    CloseGenWindow( &mDeviceWnd );
    CloseGenWindow( &mStreamParametersWnd );

    mRegisterInterfaceDlg.SetDevice( &mDevice );
    mRegisterInterfaceDlg.DoModal();
    mRegisterInterfaceDlg.DestroyWindow();
}


// ==========================================================================
PvUInt32 GEVPlayerDlg::GetPayloadSize()
{
    if ( !mDevice.IsConnected() )
    {
        return 0;
    }

    PvGenInteger *lPayloadSize = dynamic_cast<PvGenInteger *>( mDevice.GetGenParameters()->Get( "PayloadSize" ) );
    PvGenInteger *lWidth = dynamic_cast<PvGenInteger *>( mDevice.GetGenParameters()->Get( "Width" ) );
    PvGenInteger *lHeight = dynamic_cast<PvGenInteger *>( mDevice.GetGenParameters()->Get( "Height" ) );
    PvGenEnum *lPixelFormat = dynamic_cast<PvGenEnum *>( mDevice.GetGenParameters()->Get( "PixelFormat" ) );

    // Get payload size
    PvInt64 lPayloadSizeValue = 0;
    if ( lPayloadSize != NULL )
    {
        lPayloadSize->GetValue( lPayloadSizeValue );
    }

    // Compute poor man's payload size - for devices not maintaning PayloadSize properly
    PvInt64 lPoorMansPayloadSize = 0;
    if ( ( lWidth != NULL ) && ( lHeight != NULL ) && ( lPixelFormat != NULL ) )
    {
        PvInt64 lWidthValue, lHeightValue;
        lWidth->GetValue( lWidthValue );
        lHeight->GetValue( lHeightValue );

        PvInt64 lPixelFormatValue;
        lPixelFormat->GetValue( lPixelFormatValue );
        PvInt64 lPixelSizeInBits = PvGetPixelBitCount( static_cast<PvPixelType>( lPixelFormatValue ) );

        lPoorMansPayloadSize = ( lWidthValue * lHeightValue * lPixelSizeInBits ) / 8;
    }

    // Take max, let pipeline know what the payload size is
    PvInt64 lBestPayloadSize = max( lPayloadSizeValue, lPoorMansPayloadSize );
    if ( ( lBestPayloadSize > 0 ) && ( lBestPayloadSize < UINT_MAX ) )
    {
        return static_cast<PvUInt32>( lBestPayloadSize );
    }

    // Could not compute/retrieve payload size...
    return 0;
}


// ==========================================================================
void GEVPlayerDlg::OnDisplayDisabled()
{
    mTargetFPS = -1;
    ApplyDisplayOptions();
}


// ==========================================================================
void GEVPlayerDlg::OnDisplay30FPS()
{
    mTargetFPS = 30;
    ApplyDisplayOptions();
}


// ==========================================================================
void GEVPlayerDlg::OnDisplay60FPS()
{
    mTargetFPS = 60;
    ApplyDisplayOptions();
}

CString GEVPlayerDlg::GetStickyPath()
{
    CString lLocation;
    TCHAR lAppData[MAX_PATH];
    if ( SHGetSpecialFolderPath(      
        GetSafeHwnd(),
        lAppData,
        CSIDL_APPDATA,
        TRUE ) == TRUE )
    {
        lLocation += lAppData;
    }
    lLocation += "\\Pleora\\GEVPlayer.pvcfg";
    return lLocation;
}

CString GEVPlayerDlg::GetDefaultPath()
{
    CString lLocation;
    TCHAR lAppData[MAX_PATH];
    if ( SHGetSpecialFolderPath(      
        GetSafeHwnd(),
        lAppData,
        CSIDL_APPDATA,
        TRUE ) == TRUE )
    {
        lLocation += lAppData;
    }
    lLocation += "\\Pleora\\GEVPlayerDefaults.pvcfg";
    return lLocation;
}

void GEVPlayerDlg::OnToolsSavepreferences()
{
    SaveConfig( GetStickyPath(), false );
}



void GEVPlayerDlg::SaveConfig( CString aLocation, bool aSaveConnectedDevice )
{
    PvConfigurationWriter lWriter;
    lWriter.Store( "1", "gevplayerconfig" ); 

    SaveDisplayOptions( lWriter );

    mImageSaveDlg->Save( lWriter );
    mSetupDlg.Save( lWriter );
    mFilteringDlg.Save( lWriter );
    mLogBuffer->Save( lWriter );

    CString lCommunicationVisibility;
    CString lDeviceVisibility;
    CString lStreamVisibility;

    // save visibility settings for each PvGenBrowserWnd
    lCommunicationVisibility.Format( _T("%d"), mCommunicationWnd.GetVisibility() );
    lDeviceVisibility.Format( _T("%d"), mDeviceWnd.GetVisibility() );
    lStreamVisibility.Format( _T("%d"), mStreamParametersWnd.GetVisibility() );
    lWriter.Store( lCommunicationVisibility.GetBuffer(), "communicationvisibility" );
    lWriter.Store( lDeviceVisibility.GetBuffer(), "devicevisibility" );
    lWriter.Store( lStreamVisibility.GetBuffer(), "streamvisibility" );


    PvUInt32 lCount = mPipeline.GetBufferCount();
    
    if ( lCount < BUFFER_COUNT_MIN )
    {
        lCount = BUFFER_COUNT_DEFAULT;
    }

    CString lBufferCountStr;
    lBufferCountStr.Format( _T("%d"), lCount );
    lWriter.Store( lBufferCountStr.GetBuffer(), TAG_STREAMINGBUFFERCOUNT );

    if( aSaveConnectedDevice )
    {
		mEventMonitorDlg->EnableGenICamMonitoring( false );
		SavingThread lSavingThread( &mSetupDlg, &lWriter, &mDevice, &mStream, this );
        lSavingThread.Save();
		mEventMonitorDlg->EnableGenICamMonitoring( true );
    }
    else
    {
        lWriter.Store( mDevice.GetGenLink(), "communication" );
    }

    lWriter.Save( aLocation.GetBuffer() );
}



void GEVPlayerDlg::OnToolsRestoredefaultpreferences()
{
    // load the default file
    OpenConfig( GetDefaultPath() );

    // save the default settings so they are persisted next time
    SaveConfig( GetStickyPath(), false );
}


///
/// \brief Saves display configuration to file
///

void GEVPlayerDlg::SaveDisplayOptions( PvConfigurationWriter& aWriter )
{
    aWriter.Store( DISPLAYSETUP_VERSION, TAG_VERSION );
    aWriter.Store( mVSync ? "1" : "0", TAG_VSYNC );
    aWriter.Store( mKeepPartialImages ? "1" : "0", TAG_KEEP_PARTIAL );

    char lMaxFPS[512];
    sprintf_s( lMaxFPS, 512, "%d", mTargetFPS );
    aWriter.Store( lMaxFPS, TAG_FPS );
}


///
/// \brief Retrieves display configuration from file
///

void GEVPlayerDlg::LoadDisplayOptions( PvConfigurationReader& aReader )
{
	PvString lStr;

    if ( aReader.Restore( TAG_VSYNC, lStr ).IsOK() )
    {
		if ( strcmp( lStr.GetAscii(), "1" ) == 0 )
		{
			mVSync = true;
		}
		else
		{
			mVSync = false;
		}
    }

    if ( aReader.Restore( TAG_KEEP_PARTIAL, lStr ).IsOK() )
    {
		if ( strcmp( lStr.GetAscii(), "1" ) == 0 )
		{
			mKeepPartialImages = true;
		}
		else
		{
			mKeepPartialImages = false;
		}
    }

    if ( aReader.Restore( TAG_FPS, lStr ).IsOK() )
    {
    	int lFPS = atoi( lStr.GetAscii() );
    	switch ( lFPS )
    	{
		default:
			ASSERT( 0 );
			break; // Do not change anything...

		case 30:
		case 60:
		case -1:
            mTargetFPS = lFPS;
            break;
    	}
    }

    // in case the display currently exists, apply the cached values immediately
    ApplyDisplayOptions();
}

void GEVPlayerDlg::ApplyDisplayOptions()
{
    if ( mThreadDisplay == NULL )
    {
        return;
    }

    mThreadDisplay->SetEnabled( mTargetFPS != -1 );
    mThreadDisplay->SetTargetFPS( mTargetFPS );
    mThreadDisplay->SetVSync( mVSync );
    mThreadDisplay->SetKeepPartialImages( mKeepPartialImages );
    mThreadDisplay->ResetStats();
}


void GEVPlayerDlg::OnToolsBufferoptions()
{
    BufferOptionsDlg lBufferOptions( &mPipeline );
    lBufferOptions.DoModal();
}

void GEVPlayerDlg::OnToolsDisplaypartialimages()
{
    mKeepPartialImages = !mKeepPartialImages;
    ApplyDisplayOptions();
}
