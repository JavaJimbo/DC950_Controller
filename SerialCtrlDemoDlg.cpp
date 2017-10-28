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
*	10-28-17: Testhandler() fully implemented, test sequence flow works well.
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

CFont BigFont, SmallFont, MidFont;

char strSendMeasureCommand[] = ":MEAS?\r\n";

TestApp MyTestApp;		
UINT  stepNumber = 0;
BOOL previousButtonPushed = FALSE;


// Dialog constructor
CSerialCtrlDemoDlg::CSerialCtrlDemoDlg(CWnd* pParent /*=NULL*/)	: CDialog(CSerialCtrlDemoDlg::IDD, pParent)	// , bPortOpened(FALSE)
{
	
}

// Dialog deconstructor: shut down system 
CSerialCtrlDemoDlg::~CSerialCtrlDemoDlg() {
	if (MyTestApp.flgMainPortOpen) {
		MyTestApp.closeTestSerialPort();
	}
}

static UINT BASED_CODE indicators[] =
{
	ID_INDICATOR_STATUSBAR1,
	ID_INDICATOR_STATUSBAR2
};

void CSerialCtrlDemoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_STATIC_PASSFAIL_STATUS, m_static_TestResult);
	DDX_Control(pDX, IDC_STATIC_LINE1, m_static_Line1);
	DDX_Control(pDX, IDC_STATIC_LINE2, m_static_Line2);
	DDX_Control(pDX, IDC_STATIC_LINE3, m_static_Line3);
	DDX_Control(pDX, IDC_STATIC_LINE4, m_static_Line4);
	DDX_Control(pDX, IDC_STATIC_LINE5, m_static_Line5);	
	DDX_Control(pDX, IDC_STATIC_LINE6, m_static_Line6);
	DDX_Control(pDX, IDC_STATIC_LINE7, m_static_DataOut);
	DDX_Control(pDX, IDC_STATIC_LINE8, m_static_DataIn);
	DDX_Control(pDX, IDC_STATIC_RESULT, m_static_TestName);
	DDX_Control(pDX, IDC_STATIC_COM_OUT, m_static_ComOut);
	DDX_Control(pDX, IDC_STATIC_COM_IN, m_static_ComIn);
	
	DDX_Control(pDX, IDC_RUN_BUTTON, m_static_ButtonEnter);
	DDX_Control(pDX, IDC_BUTTON_PASS, m_static_ButtonPass);
	DDX_Control(pDX, IDC_BUTTON_FAIL, m_static_ButtonFail);
	
	DDX_Control(pDX, IDC_BUTTON_PREVIOUS, m_static_ButtonPrevious);
	DDX_Control(pDX, IDC_BUTTON_HALT, m_static_ButtonHalt);
	DDX_Control(pDX, IDC_EDIT1, m_BarcodeEditBox);
	DDX_Control(pDX, IDC_STATIC_LABEL, m_static_BarcodeLabel);
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
	
	m_static_TestResult.SetWindowText("");
	m_static_Line1.SetWindowText("Test system preparation:");
	m_static_Line2.SetWindowText("Before beginning, make sure test");
	m_static_Line3.SetWindowText("equipment is plugged in and turned ON,");
	m_static_Line4.SetWindowText("and INTERFACE BOX is ON");
	m_static_Line5.SetWindowText("Then click READY to begin.");
	m_static_Line6.SetWindowText("");
	

	ConfigureFont(BigFont, 24, 12, TRUE);	
	CWnd* pCtrlWnd = GetDlgItem(IDC_STATIC_LINE1);	
	pCtrlWnd->SetFont(&BigFont, TRUE);

	ConfigureFont(MidFont, 20, 8, TRUE);
	pCtrlWnd = GetDlgItem(IDC_STATIC_PASSFAIL_STATUS);
	pCtrlWnd->SetFont(&MidFont, TRUE);

	ConfigureFont(SmallFont, 15, 5, TRUE);
	pCtrlWnd = GetDlgItem(IDC_STATIC_LINE6);
	pCtrlWnd->SetFont(&SmallFont, TRUE);
	
	MyTestApp.InitializeDisplayText();	
	previousButtonPushed = FALSE;
	stepNumber = 0;	
	CreateStatusBars();
	MyTestApp.DisplayStepNumber(this, stepNumber);		
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CSerialCtrlDemoDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	CDialog::OnSysCommand(nID, lParam);
}


void CSerialCtrlDemoDlg::OnBnClickedButtonPass()
{
	MyTestApp.testStep[stepNumber].Result = PASS;
	MyTestApp.DisplayPassFailStatus(stepNumber, this);
	m_static_ButtonEnter.EnableWindow(TRUE);
}


void CSerialCtrlDemoDlg::OnBnClickedButtonFail()
{
	MyTestApp.testStep[stepNumber].Result = FAIL;
	MyTestApp.DisplayPassFailStatus(stepNumber, this);
	m_static_ButtonEnter.EnableWindow(TRUE);
}


// PREVIOUS BUTTON
void CSerialCtrlDemoDlg::OnBtnClickedPrevious()
{
	UINT stepStatus;
	int  testType;
	do {
		if (stepNumber <= BARCODE_SCAN) break;
		stepNumber--;
		if (stepNumber == REMOTE_TEST) break;

		stepStatus = MyTestApp.testStep[stepNumber].Result;
		testType = MyTestApp.testStep[stepNumber].testType;
	} while (stepStatus == SUBSTEP || testType == AUTO);

	if (stepNumber == 1) MyTestApp.enableBarcodeScan(this);
	
	MyTestApp.DisplayStepNumber(this, stepNumber);
	MyTestApp.DisplayIntructions(stepNumber, this);
	MyTestApp.DisplayPassFailStatus(stepNumber, this);
}


void CSerialCtrlDemoDlg::CreateStatusBars() {

	m_StatusBar.Create(this); //We create the status bar
	m_StatusBar.SetIndicators(indicators, 2); //Set the number of panes 	

	CRect rect;
	GetClientRect(&rect);
	//Size the two panes
	m_StatusBar.SetPaneInfo(0, ID_INDICATOR_STATUSBAR1,	SBPS_NORMAL, rect.Width() - 300);
	m_StatusBar.SetPaneInfo(1, ID_INDICATOR_STATUSBAR2, SBPS_STRETCH, 0);

	//This is where we actually draw it on the screen
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, ID_INDICATOR_STATUSBAR2);
	m_StatusBar.GetStatusBarCtrl().SetBkColor(RGB(180, 180, 180));
	m_StatusBar.SetPaneText(0, "No file loaded");
	m_StatusBar.SetPaneText(1, "Step #");
}


// HALT TIMER
void CSerialCtrlDemoDlg::OnBtnClickedHalt()
{
	StopTimer();
	previousButtonPushed = FALSE;
}


void CSerialCtrlDemoDlg::ConfigureFont(CFont &ptrFont, int fontHeight, int fontWidth, BOOL flgBold) {
	if (flgBold) {
		ptrFont.CreateFont(
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
	else
	{
		ptrFont.CreateFont(
			fontHeight,
			fontWidth,
			0,
			FW_NORMAL,
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
}

void CALLBACK TimerProc(void* lpParametar, BOOLEAN TimerOrWaitFired)
{
	// This is used only to call QueueTimerHandler
	// Typically, this function is static member of CTimersDlg
	CSerialCtrlDemoDlg* obj = (CSerialCtrlDemoDlg*)lpParametar;
	obj->timerHandler();	
}

void CSerialCtrlDemoDlg::StopTimer() {
	if (m_timerHandle != NULL) {
		DeleteTimerQueueTimer(NULL, m_timerHandle, NULL);
		m_timerHandle = NULL;
	}
}


void CSerialCtrlDemoDlg::StartTimer() {
	if (m_timerHandle == NULL) {
		DWORD elapsedTime = 2000;
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

void CSerialCtrlDemoDlg::OnBtnClickedRun() {
	Testhandler();
}

void CSerialCtrlDemoDlg::timerHandler() {
	Testhandler();
}

void CSerialCtrlDemoDlg::Testhandler()
{
		// Execute test step and display result:
		int testResult = MyTestApp.Execute(stepNumber, this);
		MyTestApp.DisplayPassFailStatus(stepNumber, this);

		// Step #0 checks RS232 communication.
		// If system fails this step,
		// the interface board power probably isn't on.
		// For this case, quit routine here:
		if (stepNumber == 0 && testResult != PASS) return;		

		// Otherwise if a unit has completed full test sequence,
		// set step back to 1 for testing next unit:
		else if (stepNumber == FINAL_PASS || stepNumber == FINAL_FAIL) 
			stepNumber = 1;

		// If unit passed or there was no test for this step, 
		// then advance to next step.	
		// If all tests are completed, reset system
		// and set step back to 1 for testing next unit:
		else if (testResult == PASS || testResult == SUBSTEP) 			
			stepNumber++;		
		// Otherwise if unit failed, stop timer (it it's running)
		// then query user to retry or quit testing unit:
		else {
			StopTimer();
			if (stepNumber == 1) MyTestApp.DisplayMessageBox("BARCODE ERROR", "Please scan label again", 1);
			else {
				BOOL retry = MyTestApp.DisplayMessageBox("UNIT FAILED TEST", "Retry test or Cancel?", 2);
				// If user doesn't wish to retry any failed test, jump to FAIL TEST:
				if (!retry) stepNumber = FINAL_FAIL;
			}
		}
		// Now display next step:
		MyTestApp.DisplayIntructions(stepNumber, this);
		MyTestApp.DisplayStepNumber(this, stepNumber);
		// If this is the start of an automated sequence
		// or if sequence is already in progress, start timer.
		// If next step is manual, make sure timer is off:
		if (MyTestApp.testStep[stepNumber].testType == AUTO) StartTimer();		
		else StopTimer();
		
		if (stepNumber == 1) MyTestApp.enableBarcodeScan(this);
		else MyTestApp.disableBarcodeScan(this);

		// Now wait for user to hit ENTER
		// or timer to call this routine and execute new step
		return;
}
