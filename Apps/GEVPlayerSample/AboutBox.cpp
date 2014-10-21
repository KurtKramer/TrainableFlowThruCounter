// *****************************************************************************
//
//     Copyright (c) 2007, Pleora Technologies Inc., All rights reserved.
//
// *****************************************************************************

// *****************************************************************************
//
//     Copyright (c) 2007, Pleora Technologies Inc., All rights reserved.
//
// *****************************************************************************

///
/// \file AboutBox.cpp
/// \class AboutBox
///
/// \brief Generic and brandable bout box dialog.
///

#include "stdafx.h"

#include "GEVPlayer.h"
#include "AboutBox.h"

static const wchar_t * BTN_OK = _T( "OK" );


BEGIN_DHTML_EVENT_MAP( AboutBox )
    DHTML_EVENT_ONCLICK( BTN_OK, OnOK )
END_DHTML_EVENT_MAP()


// ==========================================================================
void
AboutBox::Show()
{
    AboutBox lDlg;
    lDlg.DoModal( _T( "About.html" ) );
}


// ==========================================================================
AboutBox::AboutBox()
{
}


// ==========================================================================
AboutBox::~AboutBox()
{
}


// ==========================================================================
HRESULT
AboutBox::OnOK(
    IHTMLElement *
    )
{
    EndDialog( IDOK );
    return S_OK;
}

