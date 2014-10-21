// *****************************************************************************
//
//     Copyright (c) 2007, Pleora Technologies Inc., All rights reserved.
//
// *****************************************************************************

#pragma once
#include "afxcmn.h"
#include "afxwin.h"

#include <map>

#include "ParameterInfo.h"
#include "LogBuffer.h"

#include "resource.h"


class EventMonitorDlg : public CDialog
{
    DECLARE_DYNAMIC(EventMonitorDlg)

public:

    EventMonitorDlg( LogBuffer *aLogBuffer, PvGenParameterArray *aParameters, CWnd* pParent = NULL );   // standard constructor
    virtual ~EventMonitorDlg();

    enum { IDD = IDD_EVENTMONITOR };

    void LoadData();

	// Controls GenICam monitoring. Different than logging control, for performance purpose
	void EnableGenICamMonitoring( bool aEnabled );

protected:

    virtual void DoDataExchange( CDataExchange* pDX );    // DDX/DDV support
    BOOL OnInitDialog();
    afx_msg void OnBnClickedSelectall();
    afx_msg void OnBnClickedAddButton();
    afx_msg void OnBnClickedRemoveButton();
    afx_msg void OnBnClickedClearButton();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
    afx_msg void OnTimer( UINT_PTR nIDEvent );
    afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
    afx_msg void OnDestroy();
    afx_msg void OnOK();
    afx_msg void OnCancel();
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnBnClickedGenICamCheckBox();
    afx_msg void OnBnClickedBufferAllCheckBox();
    afx_msg void OnBnClickedBufferErrorCheckBox();
    afx_msg void OnBnClickedSaveLogCheckBox();
    afx_msg void OnBnClickedSaveLogButton();
    DECLARE_MESSAGE_MAP()

    void EnableInterface();

private:

    PvGenParameterArray *mParameters;
    std::map<CString, ParameterInfo> mInfoMap;

    CBrush mWindowBackgroundBrush;

    CEdit mLogEdit;
    CButton mSaveLogCheckBox;
    CEdit mSaveLogEdit;
    CButton mSaveLogButton;
    CButton mGenICamCheckBox;
    CButton mBufferAllCheckBox;
    CButton mBufferErrorCheckBox;

    UINT_PTR mTimer;

    int mLogEditTextLength;
    int mLastUpdateTime;

    CRect mCrt;
    BOOL mNeedInit;

    LogBuffer *mLogBuffer;
};
