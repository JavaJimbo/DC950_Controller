/*	SerialCtrlDemoDlg.cpp : implementation file
*
*	9-13-17 JBS: $$$$ in OnBnClickedButtonWr() made two changes:
*	1) Append carriage return & linefeed to end of command for HP 34401a
*	2) Clear receive listbox before sending command
*	10-15-17: Added pop up message boxes for error handling
*	10-20-71: Created ReadSerialPort() and WriteSerialPort() and simpified sendReceiveSerial()
*	10-21-17: Added CRC
*	10-22-17: Cleaned up error handling, use intError. Works well with interface board->meter
*/

/*
m_EditSerialNumber.SetFocus();   
m_EditSerialNumber.ShowCaret();

CString txtSerialNumber;   
m_EditSerialNumber.GetWindowText(txtSerialNumber);
m_static_Line1.SetWindowText((LPCTSTR)txtSerialNumber);
*/
#include "stdafx.h"
#include "SerialCtrlDemo.h"
#include "SerialCtrlDemoDlg.h"
#include "SerialCom.h"
#include "Definitions.h"
#include <windows.h>
#include <iostream>
#include <sstream>
#include <string.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const char *strErrorCodes[] = { NULL, "PORT_ERROR", "TIMEOUT_ERROR", "RESPONSE_ERROR", "CRC_ERROR", "SYSTEM_ERROR" };
#define MAXERROR 5


char strSendMeasureCommand[] = ":MEAS?\r\n";

enum {
	STANDBY = 0,
	SEND,
	RECEIVE
};


TestApp MyTestApp;		
int intError = 0;


void CALLBACK TimerProc(void* lpParametar,
	BOOLEAN TimerOrWaitFired)
{
	// This is used only to call QueueTimerHandler
	// Typically, this function is static member of CTimersDlg
	CSerialCtrlDemoDlg* obj = (CSerialCtrlDemoDlg*)lpParametar;
	obj->timerHandler();
	
}


// CSerialCtrlDemoDlg dialog
CSerialCtrlDemoDlg::CSerialCtrlDemoDlg(CWnd* pParent /*=NULL*/)	: CDialog(CSerialCtrlDemoDlg::IDD, pParent)	// , bPortOpened(FALSE)
{
	
}

CSerialCtrlDemoDlg::~CSerialCtrlDemoDlg() {
	if (MyTestApp.flgMainPortOpen) {
		MyTestApp.closeTestSerialPort();
	}
}

void CSerialCtrlDemoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_STATIC_LINE1, m_static_Line1);
	DDX_Control(pDX, IDC_STATIC_LINE2, m_static_Line2);
	DDX_Control(pDX, IDC_STATIC_LINE3, m_static_Line3);
	DDX_Control(pDX, IDC_STATIC_LINE4, m_static_Line4);
	DDX_Control(pDX, IDC_STATIC_LINE5, m_static_Line5);	
	DDX_Control(pDX, IDC_EDIT1, m_EditSerialNumber);
}

BEGIN_MESSAGE_MAP(CSerialCtrlDemoDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_BN_CLICKED(IDC_RUN_BUTTON, &CSerialCtrlDemoDlg::OnBtnClickedRun)
	ON_BN_CLICKED(IDC_BUTTON_HALT, &CSerialCtrlDemoDlg::OnBtnClickedHalt)
	ON_BN_CLICKED(IDC_BUTTON_PREVIOUS, &CSerialCtrlDemoDlg::OnBtnClickedPrevious)
END_MESSAGE_MAP()


// CSerialCtrlDemoDlg message handlers
BOOL CSerialCtrlDemoDlg::OnInitDialog()
{	
	CDialog::OnInitDialog();

	 m_static_Line5.SetWindowText("THIS IS STATIC LINE 5");
	 m_static_Line3.SetWindowText("THIS IS STATIC LINE 3");
	 m_static_Line2.SetWindowText("THIS IS STATIC LINE 2");
	 m_static_Line4.SetWindowText("THIS IS STATIC LINE 4");
	 m_static_Line1.SetWindowText("THIS IS STATIC LINE 1");

	// TODO: Add extra initialization here	
	intError = NO_ERRORS;

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CSerialCtrlDemoDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	CDialog::OnSysCommand(nID, lParam);
}




void CSerialCtrlDemoDlg::StartTimer() {
	if (m_timerHandle == NULL) {
		DWORD elapsedTime = 1000;
		BOOL success = ::CreateTimerQueueTimer(  // Create new timer
			&m_timerHandle,
			NULL,
			TimerProc,
			this,
			0,
			elapsedTime,
			WT_EXECUTEINTIMERTHREAD);
	}
}


// START: INITIALIZE HP34401
void CSerialCtrlDemoDlg::OnBtnClickedPrevious()
{		
	if (MyTestApp.openTestSerialPort()){			
		if (MyTestApp.InitializeHP34401(this)) 
		{
			GetDlgItem(IDC_RUN_BUTTON)->EnableWindow(TRUE);
			GetDlgItem(IDC_BUTTON_HALT)->EnableWindow(TRUE);
			m_static_Line5.SetWindowText("Initializing meter. Please wait...");
			if (m_timerHandle == NULL) {
				MyTestApp.msDelay(1000);
				DWORD elapsedTime = 1000;				
				BOOL success = ::CreateTimerQueueTimer(  
					&m_timerHandle,
					NULL,
					TimerProc,
					this,
					0,
					elapsedTime,
					WT_EXECUTEINTIMERTHREAD);
			}
		}
		else MyTestApp.DisplayMessageBox("Serial communication error", "Check interface board connections", 1);
	}
}

void CSerialCtrlDemoDlg::OnBtnClickedRun()
{
	StartTimer();
}

// HALT timer
void CSerialCtrlDemoDlg::OnBtnClickedHalt()
{	
	DeleteTimerQueueTimer(NULL, m_timerHandle, NULL);  // destroy the timer
	m_timerHandle = NULL;	
}


// TIMER HANDLER - TEST SEQUENCE EXECUTES HERE
void CSerialCtrlDemoDlg::timerHandler() {	
	char outPacket[BUFFERSIZE] = "$MEAS?";
	char inPacket[BUFFERSIZE];

	if (!MyTestApp.sendReceiveSerial(1, outPacket, inPacket)) {
			DeleteTimerQueueTimer(NULL, m_timerHandle, NULL);  // destroy the timer
			m_timerHandle = NULL;
			MyTestApp.closeTestSerialPort();			 
			if (intError <= MAXERROR) MyTestApp.DisplayMessageBox("ERROR:", strErrorCodes[intError], 1);
	}
	else {
		m_static_Line5.SetWindowText(inPacket);		
		intError = NO_ERRORS;		
	}	
}



