// *****************************************************************************
//
//     Copyright (c) 2007, Pleora Technologies Inc., All rights reserved.
//
// *****************************************************************************

#pragma once

#include "HtmlDialog.h"


class AboutBox
    : public HtmlDialog
{

public:

    static void Show();

    AboutBox();
    virtual ~AboutBox();

protected:

    virtual HRESULT OnOK( IHTMLElement * );
    DECLARE_DHTML_EVENT_MAP()

private:

};

