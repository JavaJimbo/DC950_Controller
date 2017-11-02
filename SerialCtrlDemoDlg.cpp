/*	SerialCtrlDemoDlg.cpp : implementation file
*
*	9-13-17 JBS: in OnBnClickedButtonWr() made two changes:
*	1) Append carriage return & linefeed to end of command for HP 34401a
*	2) Clear receive listbox before sending command
*	10-15-17: Added pop up message boxes for error handling
*	10-20-71: Created ReadSerialPort() and WriteSerialPort() and simpified sendReceiveSerial()
*	10-21-17: Added CRC
*	10-22-17: Cleaned up error handling, use intError. Works well with interface board->meter
*	10-23-17: More cleanup, eliminated unnecessary statements. Renamed text labels.
*	10-28-17: Testhandler() fully implemented, test sequence flow works well.
*	10-30-17: Model number select radio buttons were added. 
*			  Also changes made for barcode scanner to prevent CR exiting program.
*   10-31-17: Converted RS232 communication to three COM ports.
*	11-1-17: All tests up and running except spectrometer. Spreadsheet not done yet.
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

TestApp MyTestApp;		

//UINT  stepNumber = 0;
//UINT  subStepNumber = 0;
// BOOL previousButtonPushed = FALSE;


// Dialog constructor
CSerialCtrlDemoDlg::CSerialCtrlDemoDlg(CWnd* pParent /*=NULL*/)	: CDialog(CSerialCtrlDemoDlg::IDD, pParent)	// , bPortOpened(FALSE)
{
	
}

// Dialog deconstructor: shut down system 
CSerialCtrlDemoDlg::~CSerialCtrlDemoDlg() {
	MyTestApp.closeAllSerialPorts();
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
	DDX_Control(pDX, IDC_BUTTON_TEST, m_static_ButtonTest);
	
	DDX_Control(pDX, IDC_BUTTON_PREVIOUS, m_static_ButtonPrevious);
	DDX_Control(pDX, IDC_BUTTON_HALT, m_static_ButtonHalt);
	DDX_Control(pDX, IDC_EDIT1, m_BarcodeEditBox);
	DDX_Control(pDX, IDC_STATIC_LABEL, m_static_BarcodeLabel);
	DDX_Control(pDX, IDC_RADIO_STD, m_static_optStandard);
	DDX_Control(pDX, IDC_RADIO_FILTER, m_static_optFilter);
	DDX_Control(pDX, IDC_STATIC_MODELGROUP, m_static_modelGroup);
}


BEGIN_MESSAGE_MAP(CSerialCtrlDemoDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_BN_CLICKED(IDC_BUTTON_TEST, &CSerialCtrlDemoDlg::OnBtnClickedTest)
	ON_BN_CLICKED(IDC_RUN_BUTTON, &CSerialCtrlDemoDlg::OnBtnClickedRun)
	ON_BN_CLICKED(IDC_BUTTON_HALT, &CSerialCtrlDemoDlg::OnBtnClickedHalt)
	ON_BN_CLICKED(IDC_BUTTON_PREVIOUS, &CSerialCtrlDemoDlg::OnBtnClickedPrevious)
	ON_BN_CLICKED(IDC_BUTTON_PASS, &CSerialCtrlDemoDlg::OnBnClickedButtonPass)
	ON_BN_CLICKED(IDC_BUTTON_FAIL, &CSerialCtrlDemoDlg::OnBnClickedButtonFail)
	ON_BN_CLICKED(IDC_RADIO_STD, &CSerialCtrlDemoDlg::OnBtnClickedRadioSTD)
	ON_BN_CLICKED(IDC_RADIO_FILTER, &CSerialCtrlDemoDlg::OnBtnClickedRadioFilter)	
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
	// previousButtonPushed = FALSE;
	stepNumber = 0;	
	subStepNumber = 0;
	CreateStatusBars();
	MyTestApp.DisplayStepNumber(this, stepNumber);		
	m_static_optStandard.SetCheck(TRUE);
	m_static_optFilter.SetCheck(FALSE);
	// runFilterActuatorTest = (BOOL) m_static_optFilter.GetCheck();
	// TestApp MyTestApp = new TestApp(this);
	MyTestApp.ptrDialog = this;
		 	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CSerialCtrlDemoDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	CDialog::OnSysCommand(nID, lParam);
}


// PREVIOUS BUTTON
void CSerialCtrlDemoDlg::OnBtnClickedPrevious()
{
	int  stepType;
	do {
		if (stepNumber <= BARCODE_SCAN) break;
		stepNumber--;
		if (stepNumber == REMOTE_TEST) break;
		
		stepType = MyTestApp.testStep[stepNumber].stepType;
	} while (stepType == AUTO);

	if (stepNumber == 1) MyTestApp.resetDisplays(this); //  enableBarcodeScan(this);
	
	MyTestApp.DisplayStepNumber(this, stepNumber);
	MyTestApp.DisplayIntructions(stepNumber, this);
	MyTestApp.DisplayPassFailStatus(stepNumber, 0, this);
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
	// previousButtonPushed = FALSE;
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

void CSerialCtrlDemoDlg::OnBnClickedButtonPass()
{
	MyTestApp.testStep[stepNumber].Status = PASS;
	Testhandler();
	//MyTestApp.DisplayPassFailStatus(stepNumber, this);
	//m_static_ButtonEnter.EnableWindow(TRUE);
}

void CSerialCtrlDemoDlg::OnBnClickedButtonFail()
{
	MyTestApp.testStep[stepNumber].Status = FAIL;
	Testhandler();
	//MyTestApp.DisplayPassFailStatus(stepNumber, this);
	//m_static_ButtonEnter.EnableWindow(TRUE);
}

void CSerialCtrlDemoDlg::Testhandler()
{
		// Execute test step and display result:
		int testResult = MyTestApp.Execute(stepNumber, this);
		MyTestApp.DisplayPassFailStatus(stepNumber, subStepNumber, this);

		if (testResult == PASS || testResult == FAIL) {
			subStepNumber = 0;

			int stepType = MyTestApp.testStep[stepNumber].stepType;

			// Step #0 checks RS232 communication.
			// If system fails this step,
			// the interface board power probably isn't on.
			// For this case, quit routine here:
			if (stepNumber == 0 && testResult != PASS) return;

			// Otherwise if a unit has completed full test sequence,
			// set step back to 1 for testing next unit:
			else if (stepNumber == FINAL_PASS || stepNumber == FINAL_FAIL)
				stepNumber = 1;

			// If unit passedd, then advance to next step.
			// If unit isn't the filter actuator model, 
			// then skip the ACTUATOR and SPECTROMETER tests and go to FINAL PASS:
			else if (testResult == PASS) {
				stepNumber++; 
				if (runFilterActuatorTest == FALSE && stepNumber == ACTUATOR_TEST) stepNumber = FINAL_PASS;
			}
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
			// If this is the start of an automated sequence
			// or if sequence is already in progress, start timer.
			// If next step is manual, make sure timer is off:
			if (MyTestApp.testStep[stepNumber].stepType == AUTO) StartTimer();
			else StopTimer();
		}

		// Now display next step:
		MyTestApp.DisplayIntructions(stepNumber, this);
		MyTestApp.DisplayStepNumber(this, stepNumber);		

		if (stepNumber == 1) MyTestApp.resetDisplays(this);// MyTestApp.enableBarcodeScan(this);
		else MyTestApp.disableBarcodeScan(this);

		// Now wait for user to hit ENTER
		// or timer to call this routine and execute new step
		return;
}



//void CSerialCtrlDemoDlg::OnBnClickedRadio1()
//{
	// TODO: Add your control notification handler code here
//}


void CSerialCtrlDemoDlg::OnBtnClickedTest() {
	static int voltage = 10;
	//char strMeasure[BUFFERSIZE] = ":MEAS?\r\n";				
	//if (!MyTestApp.sendReceiveSerial (HP_METER, this, strMeasure, NULL, TRUE))
	//	MyTestApp.DisplayMessageBox("HP COM error", "Check RS232 connections and POWER button", 1);			
	//voltage++;
	//char strCommand[BUFFERSIZE];
	//sprintf_s(strCommand, BUFFERSIZE, "VOLT %d\r\n", voltage);
	//MyTestApp.sendReceiveSerial(AC_POWER_SUPPLY, this, strCommand, NULL, FALSE);
	MyTestApp.RunPowerSupplyTest(this);
}


void CSerialCtrlDemoDlg::OnBtnClickedRadioSTD()
{
	MyTestApp.enableBarcodeScan(this);
}

void CSerialCtrlDemoDlg::OnBtnClickedRadioFilter()
{
	MyTestApp.enableBarcodeScan(this);
}


BOOL CSerialCtrlDemoDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE)
		{
			return TRUE;                // Do not process further
		}
	}

	return CWnd::PreTranslateMessage(pMsg);
}
