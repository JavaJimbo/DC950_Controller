// SerialCtrlDemoDlg.h : header file
//

#pragma once

#ifndef __AFXWIN_H__
#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols

class CSerialCtrlDemoDlg : public CDialog
{
// Construction
public:
	CSerialCtrlDemoDlg(CWnd* pParent = NULL);	// standard constructor
	~CSerialCtrlDemoDlg();						// destructor

// Dialog Data
	enum { IDD = IDD_SERIALCTRLDEMO_DIALOG };
	//CMyStatic	m_static;

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	// HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
public:
	//CComboBox m_comboSN;
	//CComboBox m_comboBR;
	CStatic m_static_Line5;
	CStatic m_static_Line3;
	CStatic m_static_Line2;
	CStatic m_static_Line4;
	CStatic m_static_Line1;	
	
	CEdit m_EditSerialNumber;
	
	HANDLE m_timerHandle = NULL;
	void timerHandler();
	void StartTimer();
public:
	afx_msg void OnBtnClickedRun();	
	afx_msg void OnBtnClickedHalt();
	afx_msg void OnBtnClickedPrevious();	
};
