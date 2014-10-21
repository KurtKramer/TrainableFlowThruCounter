// *****************************************************************************
//
//     Copyright (c) 2007, Pleora Technologies Inc., All rights reserved.
//
// *****************************************************************************

#pragma once

#include <string>


class HtmlDialog : public CDHtmlDialog
{

    DECLARE_DYNCREATE( HtmlDialog )

public:

    HtmlDialog();
    virtual ~HtmlDialog();
    virtual BOOL DoModal( const CString &aHtml );

protected :

    enum 
    {
        IDD = IDD_HTML,
        IDH = IDR_HTML,
    };

    bool GetName( const CString &aId, wchar_t * aOut, unsigned int aOutSize );

    virtual void OnDocumentComplete( LPDISPATCH, LPCTSTR aUrl );
    virtual BOOL OnInitDialog();

    DECLARE_MESSAGE_MAP()
    DECLARE_DHTML_EVENT_MAP()

private:

    CString mHtml;

};

