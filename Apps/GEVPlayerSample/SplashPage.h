// *****************************************************************************
//
//     Copyright (c) 2007, Pleora Technologies Inc., All rights reserved.
//
// *****************************************************************************

#pragma once

#include "HtmlDialog.h"


class SplashPage
    : public HtmlDialog
{
public:

    static void Show();

    SplashPage();
    virtual ~SplashPage();

protected:

    virtual void OnTimer( UINT_PTR aEvent );
    virtual BOOL OnInitDialog();

    DECLARE_MESSAGE_MAP()

};


