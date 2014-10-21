// *****************************************************************************
//
//     Copyright (c) 2007, Pleora Technologies Inc., All rights reserved.
//
// *****************************************************************************

#include "stdafx.h"

#include "GEVPlayer.h"
#include "HtmlDialog.h"

#include <comutil.h>
#include <string>


#define STR_BUFFER_SIZE 2048


IMPLEMENT_DYNCREATE(HtmlDialog, CDHtmlDialog)


BEGIN_MESSAGE_MAP(HtmlDialog, CDHtmlDialog)
END_MESSAGE_MAP()

BEGIN_DHTML_EVENT_MAP(HtmlDialog)
END_DHTML_EVENT_MAP()


// ==========================================================================
HtmlDialog::HtmlDialog()
    : CDHtmlDialog( IDD, IDH, NULL )
{
}


// ==========================================================================
HtmlDialog::~HtmlDialog()
{
}


// ==========================================================================
BOOL HtmlDialog::DoModal( const CString &aHtml )
{
    ASSERT( aHtml.GetLength() > 0 );
    mHtml = aHtml;

    return ( CDHtmlDialog::DoModal() == 0 ) ? true : false;
}


// ==========================================================================
bool HtmlDialog::GetName( const CString &aId, wchar_t * aOut, unsigned int aOutSize )
{
    ASSERT( aId.GetLength() > 0 );
    ASSERT( aOut != NULL );
    ASSERT( aOutSize > 1 );

    CComPtr< IHTMLButtonElement > lBtn;
    HRESULT lHR;
    
    lHR = GetElementInterface(
        aId,
        IID_IHTMLButtonElement,
        reinterpret_cast< void ** >( & lBtn ) );
    if ( lHR != S_OK )
    {
        return false;
    }

    bstr_t lData;

    lHR = lBtn->get_name( lData.GetAddress() );
    if (    ( lHR != S_OK )
         || ( lData.length() == 0 )
         || ( lData.length() >= aOutSize ) )
    {
        return false;
    }

    wcscpy_s( aOut, aOutSize, lData );

    return true;
}


// ==========================================================================
void HtmlDialog::OnDocumentComplete( LPDISPATCH, LPCTSTR aUrl )
{
    ASSERT( aUrl != NULL );

    if ( wcslen( aUrl ) > 0 )
    {
        wchar_t lStr[ STR_BUFFER_SIZE ];

        // ===== Update window size =====

        unsigned int lSizeX = 640;
        unsigned int lSizeY = 480;

        if ( GetName( _T( "size" ), lStr, STR_BUFFER_SIZE ) )
        {
            swscanf_s( lStr, _T( "%u,%u" ), & lSizeX, & lSizeY );
        }

        POINT lPt;

        CDC * lDC = GetDC();

        lPt.x = lSizeX;
        lPt.y = lSizeY;

        lDC->DPtoLP( & lPt );

        ReleaseDC( lDC );

        SetWindowPos( NULL, 0, 0, lPt.x, lPt.y, SWP_NOMOVE );
        CenterWindow();

        // ===== Show window =====
    
        ShowWindow( SW_SHOW );
    }
}


// ==========================================================================
BOOL HtmlDialog::OnInitDialog()
{
    CDHtmlDialog::OnInitDialog();

    wchar_t lUrl[ STR_BUFFER_SIZE ];

    wcscpy_s( lUrl, STR_BUFFER_SIZE, _T( "file://" ) );

    DWORD lEr = GetModuleFileName( NULL, lUrl + 7, STR_BUFFER_SIZE - 7 );
    ASSERT( lEr != 0 );

    std::wstring lTempUrl(lUrl);
    std::wstring::size_type lPos = 0;
    lPos = lTempUrl.find( _T( "_Dbg.exe" ), 0 );
    if ( lPos != std::wstring::npos )
    {
        lTempUrl.erase( lPos, 4 );
    }

    lPos = lTempUrl.find( _T( "64_Dbg.exe" ), 0 );
    if ( lPos != std::wstring::npos )
    {
        lTempUrl.erase( lPos, 6 );
    }

    lPos = lTempUrl.find( _T( "64.exe" ), 0 );
    if ( lPos != std::wstring::npos )
    {
        lTempUrl.erase( lPos, 2 );
    }

    swprintf_s( lUrl, STR_BUFFER_SIZE, _T( "%s" ), lTempUrl.c_str() );
    
    wcscat_s( lUrl, STR_BUFFER_SIZE, _T( ".html/" ) );
    wcscat_s( lUrl, STR_BUFFER_SIZE, mHtml );

    if ( _waccess( lUrl + 7, 4 ) != 0 )
    {
        return FALSE;
    }

    Navigate( lUrl, navNoHistory | navNoReadFromCache | navNoWriteToCache );

    return TRUE;
}

