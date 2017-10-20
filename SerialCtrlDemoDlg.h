// SerialCtrlDemoDlg.h : header file
//

#pragma once
// #include "SerialCtrl.h"
// #include "afxwin.h"
// #include "Timer.h"

// #include "mystatic.h"
// CSerialCtrlDemoDlg dialog
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
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_comboSN;
	CComboBox m_comboBR;
	CButton m_btnOpen;
	//CEdit m_editWrite;	 
	CStatic m_staticInfo;
	CStatic m_static4Info;
	CStatic m_staticText;	
	CEdit m_EditSerialNumber;
	
	// int m_timeElapsed;
	// BOOL bPortOpened;
	//BOOL bRunMode;
	HANDLE m_timerHandle = NULL;
	void timerHandler();
	void StartTimer();
public:
	afx_msg void OnBnClickedRunButton();	
	afx_msg void OnStnClickedStaticInfo();
	afx_msg void OnStnClickedStaticNew();
	afx_msg void OnLbnSelchangeList1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton1();	
	
	afx_msg void OnBnClickedMfcmenubutton1();
};
