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
	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
public:
	CStatic m_static_BarcodeLabel;;
	CStatic m_static_TestResult;
	CStatic m_static_ComOut;
	CStatic m_static_ComIn;
	CStatic m_static_Line1;
	CStatic m_static_Line2;
	CStatic m_static_Line3;
	CStatic m_static_Line4;
	CStatic m_static_Line5;	
	CStatic m_static_Line6;
	CStatic m_static_DataIn;
	CStatic m_static_DataOut;
	CStatic m_static_TestName;
	CButton m_static_ButtonEnter;	
	CButton m_static_ButtonPass;
	CButton m_static_ButtonFail;
	CButton m_static_ButtonHalt;
	CButton m_static_ButtonPrevious;
	CEdit   m_BarcodeEditBox;
	CStatusBar m_StatusBar;
	CStatusBar m_StatusBarLeft;
	
	HANDLE m_timerHandle = NULL;
	void timerHandler();
	void StartTimer();
	void StopTimer();				
	void CSerialCtrlDemoDlg::ConfigureFont(CFont &ptrFont, int fontHeight, int fontWidth, BOOL flgBold);
	void CreateStatusBars();	
	void Testhandler();	
public:
	afx_msg void OnBtnClickedRun();	
	afx_msg void OnBtnClickedHalt();
	afx_msg void OnBtnClickedPrevious();	
	afx_msg void OnBnClickedButtonPass();
	afx_msg void OnBnClickedButtonFail();
};
