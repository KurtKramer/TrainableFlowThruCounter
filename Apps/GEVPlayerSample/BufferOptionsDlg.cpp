// BufferOptionsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "GEVPlayer.h"
#include "BufferOptionsDlg.h"


// BufferOptionsDlg dialog

IMPLEMENT_DYNAMIC(BufferOptionsDlg, CDialog)

BufferOptionsDlg::BufferOptionsDlg( PvPipeline *aPipeline, CWnd* pParent /*=NULL*/)
	: CDialog(BufferOptionsDlg::IDD, pParent)
    , mPipeline( aPipeline )
{

}

BufferOptionsDlg::~BufferOptionsDlg()
{
}

void BufferOptionsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(BufferOptionsDlg, CDialog)
    ON_BN_CLICKED(IDOK, &BufferOptionsDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// BufferOptionsDlg message handlers

void BufferOptionsDlg::OnBnClickedOk()
{
    PvUInt32 lCount = 4;
    switch( GetCheckedRadioButton( IDC_4BUFFERS, IDC_256BUFFERS ) )
    {   
        default:
        case IDC_4BUFFERS:
            lCount = 4;
            break;
        case IDC_8BUFFERS:
            lCount = 8;
            break;
        case IDC_16BUFFERS:
            lCount = 16;
            break;
        case IDC_32BUFFERS:
            lCount = 32;
            break;
        case IDC_64BUFFERS:
            lCount = 64;
            break;
        case IDC_128BUFFERS:
            lCount = 128;
            break;
        case IDC_256BUFFERS:
            lCount = 256;
            break;
    }

    PvResult lResult = mPipeline->SetBufferCount( lCount );
    if ( !lResult.IsOK() )
    {   
        CString lMessage;
        lMessage.Format( _T( "%s\r\n\r\nCould not change buffer count to requested value.\r\nOnly %i buffers could be allocated." ), 
            lResult.GetCodeString().GetUnicode(), mPipeline->GetBufferCount() );

        MessageBox( lMessage, _T( "Error" ), MB_ICONERROR | MB_OK );

        return;
    }

    OnOK();
}

BOOL BufferOptionsDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    PvUInt32 lCount = mPipeline->GetBufferCount();
	switch ( lCount )
	{
    default:
    case 16: // Default, fall through
        CheckRadioButton( IDC_4BUFFERS, IDC_256BUFFERS, IDC_16BUFFERS );
        break;
    case 4:
        CheckRadioButton( IDC_4BUFFERS, IDC_256BUFFERS, IDC_4BUFFERS );
        break;
    case 8:
        CheckRadioButton( IDC_4BUFFERS, IDC_256BUFFERS, IDC_8BUFFERS );
        break;
    case 32:
        CheckRadioButton( IDC_4BUFFERS, IDC_256BUFFERS, IDC_32BUFFERS );
        break;
    case 64:
        CheckRadioButton( IDC_4BUFFERS, IDC_256BUFFERS, IDC_64BUFFERS );
        break;
    case 128:
        CheckRadioButton( IDC_4BUFFERS, IDC_256BUFFERS, IDC_128BUFFERS );
        break;
    case 256:
        CheckRadioButton( IDC_4BUFFERS, IDC_256BUFFERS, IDC_256BUFFERS );
        break;
    }

    return TRUE;  // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX Property Pages should return FALSE
}
