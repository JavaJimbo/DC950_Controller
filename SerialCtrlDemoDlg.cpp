/*	SerialCtrlDemoDlg.cpp : implementation file
*
*	9-13-17 JBS: $$$$ in OnBnClickedButtonWr() made two changes:
*	1) Append carriage return & linefeed to end of command for HP 34401a
*	2) Clear receive listbox before sending command
*	10-15-17: Added pop up message boxes for error handling
*	10-20-71: Created ReadSerialPort() and WriteSerialPort() and simpified sendReceiveSerial()
*	10-21-17: Added CRC
*	10-22-17: Cleaned up error handling, use intError. Works well with interface board->meter
*	10-23-17: More cleanup, eliminated unnecessary statements. Renamed text labels.
*	GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(FALSE);
*/


#include "stdafx.h"
#include "SerialCtrlDemo.h"
#include "SerialCtrlDemoDlg.h"
#include "TestApp.h"
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

CFont NewFont;

char strSendMeasureCommand[] = ":MEAS?\r\n";

TestApp MyTestApp;		
int  stepNumber = -1;
BOOL previousButtonPushed = FALSE;

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

	DDX_Control(pDX, IDC_STATIC_PASSFAIL_STATUS, m_static_passfail_status);
	DDX_Control(pDX, IDC_STATIC_LINE1, m_static_Line1);
	DDX_Control(pDX, IDC_STATIC_LINE2, m_static_Line2);
	DDX_Control(pDX, IDC_STATIC_LINE3, m_static_Line3);
	DDX_Control(pDX, IDC_STATIC_LINE4, m_static_Line4);
	DDX_Control(pDX, IDC_STATIC_LINE5, m_static_Line5);	
	DDX_Control(pDX, IDC_STATIC_LINE6, m_static_Line6);
	DDX_Control(pDX, IDC_RUN_BUTTON, m_static_ButtonRun);
	DDX_Control(pDX, IDC_BUTTON_PASS, m_static_ButtonPass);
	DDX_Control(pDX, IDC_BUTTON_FAIL, m_static_ButtonFail);
	DDX_Control(pDX, IDC_BUTTON_PREVIOUS, m_static_ButtonPrevious);
	DDX_Control(pDX, IDC_BUTTON_HALT, m_static_ButtonHalt);
	DDX_Control(pDX, IDC_EDIT1, m_EditSerialNumber);
}

BEGIN_MESSAGE_MAP(CSerialCtrlDemoDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_BN_CLICKED(IDC_RUN_BUTTON, &CSerialCtrlDemoDlg::OnBtnClickedRun)
	ON_BN_CLICKED(IDC_BUTTON_HALT, &CSerialCtrlDemoDlg::OnBtnClickedHalt)
	ON_BN_CLICKED(IDC_BUTTON_PREVIOUS, &CSerialCtrlDemoDlg::OnBtnClickedPrevious)
	ON_BN_CLICKED(IDC_BUTTON_PASS, &CSerialCtrlDemoDlg::OnBnClickedButtonPass)
	ON_BN_CLICKED(IDC_BUTTON_FAIL, &CSerialCtrlDemoDlg::OnBnClickedButtonFail)
END_MESSAGE_MAP()


// CSerialCtrlDemoDlg message handlers
BOOL CSerialCtrlDemoDlg::OnInitDialog()
{	
	CDialog::OnInitDialog();
	
	m_static_passfail_status.SetWindowText("");
	m_static_Line1.SetWindowText("Test system preparation:");
	m_static_Line2.SetWindowText("Before beginning, make sure test");
	m_static_Line3.SetWindowText("equipment is plugged in and turned ON,");
	m_static_Line4.SetWindowText("and INTERFACE BOX is ON");
	m_static_Line5.SetWindowText("Then click READY to begin.");
	m_static_Line6.SetWindowText("");
	

	ConfigureFont(24, 12);
	/* Get the handle of a control*/
	CWnd* pCtrlWnd = GetDlgItem(IDC_STATIC_LINE1);	
	pCtrlWnd->SetFont(&NewFont, TRUE);
	pCtrlWnd = GetDlgItem(IDC_STATIC_PASSFAIL_STATUS);
	pCtrlWnd->SetFont(&NewFont, TRUE);


	MyTestApp.InitializeDisplayText();
	stepNumber = -1;
	
	previousButtonPushed = FALSE;
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CSerialCtrlDemoDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	CDialog::OnSysCommand(nID, lParam);
}


void CALLBACK TimerProc(void* lpParametar, BOOLEAN TimerOrWaitFired)
{
	// This is used only to call QueueTimerHandler
	// Typically, this function is static member of CTimersDlg
	CSerialCtrlDemoDlg* obj = (CSerialCtrlDemoDlg*)lpParametar;
	obj->timerHandler();
	// obj->testHandler();

}





// HALT TIMER
void CSerialCtrlDemoDlg::OnBtnClickedHalt()
{	
	StopTimer();
	previousButtonPushed = FALSE;
}

// TIMER HANDLER
void CSerialCtrlDemoDlg::timerHandler() {
	stepNumber++;
	if (stepNumber < NUMBER_OF_TESTS) {
		MyTestApp.Execute(stepNumber, this);
		if (MyTestApp.testStep[stepNumber].testType == MANUAL) StopTimer();
		MyTestApp.DisplayIntructions(stepNumber, this);
		MyTestApp.DisplayPassFailStatus(stepNumber, this);
		previousButtonPushed = FALSE;
	}
	else {
		stepNumber = 0;
		StopTimer();
	}
}

void CSerialCtrlDemoDlg::enableSerialNumberEntry() {
	m_EditSerialNumber.SetFocus();
	m_EditSerialNumber.ShowCaret();
	m_EditSerialNumber.SendMessage(EM_SETREADONLY, 0, 0);
}

void CSerialCtrlDemoDlg::disableSerialNumberEntry() {
	m_EditSerialNumber.SendMessage(EM_SETREADONLY, 1, 0);
}

void CSerialCtrlDemoDlg::StopTimer() {
	GetDlgItem(IDC_RUN_BUTTON)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_PREVIOUS)->EnableWindow(TRUE);
	if (m_timerHandle != NULL) {
		DeleteTimerQueueTimer(NULL, m_timerHandle, NULL);
		m_timerHandle = NULL;
	}
}


void CSerialCtrlDemoDlg::StartTimer() {
	GetDlgItem(IDC_RUN_BUTTON)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_PREVIOUS)->EnableWindow(FALSE);
	if (m_timerHandle == NULL) {
		DWORD elapsedTime = 10000;
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


// This routine is called once after start up,
// when the START/PREVIOUS button ispushed for the first time.
BOOL CSerialCtrlDemoDlg::InitializeSystem() {
	if (MyTestApp.openTestSerialPort()) {
		if (MyTestApp.InitializeHP34401(this)) {
			return TRUE;
		}
		else {
			MyTestApp.DisplayMessageBox("Serial communication error", "Check interface board power", 1);
			return FALSE;
		}
	}
	else return FALSE;
}


void CSerialCtrlDemoDlg::ConfigureFont(int fontHeight, int fontWidth){
	NewFont.CreateFont(
		fontHeight,
		fontWidth,
		0,
		FW_BOLD,
		FW_DONTCARE,
		FALSE,
		FALSE,
		FALSE,
		DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY,
		DEFAULT_PITCH,
		NULL
	);
}

void CSerialCtrlDemoDlg::OnBnClickedButtonPass()
{
	MyTestApp.testStep[stepNumber].status = PASS;
	MyTestApp.DisplayPassFailStatus(stepNumber, this);
	m_static_ButtonRun.EnableWindow(TRUE);
}


void CSerialCtrlDemoDlg::OnBnClickedButtonFail()
{
	MyTestApp.testStep[stepNumber].status = FAIL;
	MyTestApp.DisplayPassFailStatus(stepNumber, this);
	m_static_ButtonRun.EnableWindow(TRUE);
}

/*
if (stepNumber == 1 || stepNumber == 2) {
waitState = WAIT_FOR_PASSFAIL_ENTRY;
m_static_ButtonPass.EnableWindow(TRUE);
m_static_ButtonFail.EnableWindow(TRUE);
}
else {
waitState = 0;
m_static_ButtonPass.EnableWindow(FALSE);
m_static_ButtonFail.EnableWindow(FALSE);

//GetDlgItem(IDC_BUTTON_FAIL)->EnableWindow(FALSE);
}
*/

// PREVIOUS BUTTON
void CSerialCtrlDemoDlg::OnBtnClickedPrevious()
{
	stepNumber--;
	if (stepNumber < 0) stepNumber = 0;
	if (stepNumber == 0) enableSerialNumberEntry();
	else disableSerialNumberEntry();

	MyTestApp.DisplayIntructions(stepNumber, this);
	MyTestApp.DisplayPassFailStatus(stepNumber, this);
}

// ENTER BUTTON
void CSerialCtrlDemoDlg::OnBtnClickedRun()
{
	if (stepNumber < 0) {
		if (!InitializeSystem()) return;
	}	
	else if (stepNumber == 0) MyTestApp.resetTestStatus();
	else {
		// EXECUTE TASKS FOR THE CURRENTLY STEP	
		MyTestApp.Execute(stepNumber, this);
		if (MyTestApp.testStep[stepNumber].testType == AUTO) 
			StartTimer();
	}
	
	// ADVANCE TO NEXT STEP
	if (MyTestApp.testStep[stepNumber].testType == MANUAL) stepNumber++;
	if (stepNumber > LASTSTEP) stepNumber = 0;

	// DISPLAY INSTRUCTIONS FOR NEXT STEP
	MyTestApp.DisplayIntructions(stepNumber, this);
	MyTestApp.DisplayPassFailStatus(stepNumber, this);

	// IF THIS IS STEP 0 IN SEQUENCE, ENABLE SERIAL NUMBER ENTRY:
	if (stepNumber == 0) {
		enableSerialNumberEntry();
		MyTestApp.resetTestStatus();
	}
	else disableSerialNumberEntry();
}

/*
GetDlgItem(IDC_BUTTON_PREVIOUS)->EnableWindow(TRUE);
GetDlgItem(IDC_BUTTON_HALT)->EnableWindow(TRUE);


if (stepNumber == 1 || stepNumber == 2) {
GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(TRUE);
GetDlgItem(IDC_BUTTON_FAIL)->EnableWindow(TRUE);
}
else {
GetDlgItem(IDC_BUTTON_PASS)->EnableWindow(FALSE);
GetDlgItem(IDC_BUTTON_FAIL)->EnableWindow(FALSE);
}

*/