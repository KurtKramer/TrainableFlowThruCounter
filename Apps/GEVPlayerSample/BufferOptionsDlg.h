#pragma once

#include <PvPipeline.h>

// BufferOptionsDlg dialog

class BufferOptionsDlg : public CDialog
{
	DECLARE_DYNAMIC(BufferOptionsDlg)

public:
	BufferOptionsDlg( PvPipeline *aPipeline, CWnd* pParent = NULL );   // standard constructor
	virtual ~BufferOptionsDlg();

// Dialog Data
	enum { IDD = IDD_BUFFER_OPTIONS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedOk();
private:
    PvPipeline* mPipeline;
public:
    virtual BOOL OnInitDialog();
};
