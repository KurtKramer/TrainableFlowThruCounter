// *****************************************************************************
//
//     Copyright (c) 2007, Pleora Technologies Inc., All rights reserved.
//
// *****************************************************************************

#include "stdafx.h"

#include "GEVPlayer.h"
#include "SplashPage.h"


#ifdef _PT_DEBUG_
#pragma warning ( push )
#pragma warning ( disable : 4100 )
#endif // _PT_DEBUG_


BEGIN_MESSAGE_MAP( SplashPage, HtmlDialog )
    ON_WM_TIMER()
END_MESSAGE_MAP()


// ==========================================================================
void
SplashPage::Show()
{
// #ifndef _PT_DEBUG_

    SplashPage lDlg;
    lDlg.DoModal( _T( "Splash.html" ) );

// #endif // _PT_DEBUG_
}


#ifdef _PT_DEBUG_
#pragma warning( pop )
#endif // _PT_DEBUG_


// ==========================================================================
SplashPage::SplashPage()
{
}


// ==========================================================================
SplashPage::~SplashPage()
{
}


// ==========================================================================
void
SplashPage::OnTimer(
    UINT_PTR aEvent
    )
{
    KillTimer( aEvent );
    ShowWindow( SW_HIDE );

    EndDialog( IDOK );
}


// ==========================================================================
BOOL 
SplashPage::OnInitDialog()
{
    HtmlDialog::OnInitDialog();

    SetTimer( 0, 2500, NULL );

    return TRUE;
}
