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
m_staticText.SetWindowText((LPCTSTR)txtSerialNumber);
*/
#include "stdafx.h"
#include "SerialCtrlDemo.h"
#include "SerialCtrlDemoDlg.h"
#include "TestRoutines.h"
#include "Definitions.h"
#include <windows.h>
#include <iostream>
#include <sstream>
#include <string.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


char strSendMeasureCommand[] = ":MEAS?\r\n";

enum {
	STANDBY = 0,
	SEND,
	RECEIVE
};

int state = STANDBY;
int tickCounter = 0;

static TestApp MyTestApp;		
int intError = 0;

struct {
	BOOL portError = FALSE;
} error;

void CALLBACK TimerProc(void* lpParametar,
	BOOLEAN TimerOrWaitFired)
{
	// This is used only to call QueueTimerHandler
	// Typically, this function is static member of CTimersDlg
	CSerialCtrlDemoDlg* obj = (CSerialCtrlDemoDlg*)lpParametar;
	obj->timerHandler();
	
}

// CAboutDlg dialog used for App About
class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CSerialCtrlDemoDlg dialog
CSerialCtrlDemoDlg::CSerialCtrlDemoDlg(CWnd* pParent /*=NULL*/)	: CDialog(CSerialCtrlDemoDlg::IDD, pParent)	// , bPortOpened(FALSE)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CSerialCtrlDemoDlg::~CSerialCtrlDemoDlg() {
	if (MyTestApp.flgMainPortOpen) {
		MyTestApp.closeTestSerialPort();
	}
}

void CSerialCtrlDemoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_INFO, m_staticInfo);
	DDX_Control(pDX, IDC_STATIC_NEW, m_staticText);	
	DDX_Control(pDX, IDC_STATIC4, m_static4Info);
	DDX_Control(pDX, IDC_EDIT1, m_EditSerialNumber);
}


BEGIN_MESSAGE_MAP(CSerialCtrlDemoDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_RUN_BUTTON, &CSerialCtrlDemoDlg::OnBnClickedRunButton)
	ON_STN_CLICKED(IDC_STATIC_INFO, &CSerialCtrlDemoDlg::OnStnClickedStaticInfo)
	ON_STN_CLICKED(IDC_STATIC_NEW, &CSerialCtrlDemoDlg::OnStnClickedStaticNew)
	ON_BN_CLICKED(IDC_BUTTON2, &CSerialCtrlDemoDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON1, &CSerialCtrlDemoDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// CSerialCtrlDemoDlg message handlers
BOOL CSerialCtrlDemoDlg::OnInitDialog()
{	
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here	
	intError = NO_ERRORS;
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CSerialCtrlDemoDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CSerialCtrlDemoDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CSerialCtrlDemoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CSerialCtrlDemoDlg::OnBnClickedRunButton()
{
	StartTimer();
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


void CSerialCtrlDemoDlg::OnStnClickedStaticInfo()
{
	// TODO: Add your control notification handler code here
}


void CSerialCtrlDemoDlg::OnStnClickedStaticNew()
{
	// TODO: Add your control notification handler code here
}


void CSerialCtrlDemoDlg::OnLbnSelchangeList1()
{
	// TODO: Add your control notification handler code here
}


// START: INITIALIZE HP34401
void CSerialCtrlDemoDlg::OnBnClickedButton1()
{		
	if (MyTestApp.openTestSerialPort()){
		//m_staticInfo.SetWindowText("Initializing meter. Please wait...");
		if (MyTestApp.InitializeHP34401()) // $$$$
		{
			if (m_timerHandle == NULL) {
				// MyTestApp.msDelay(2000);
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
	}
}


// HALT timer
void CSerialCtrlDemoDlg::OnBnClickedButton2()
{	
	DeleteTimerQueueTimer(NULL, m_timerHandle, NULL);  // destroy the timer
	m_timerHandle = NULL;	
}

#define MAX_ERROR 3
void CSerialCtrlDemoDlg::timerHandler() {
	static int errorCounter = 0;
	int dummy = 0;
	char outPacket[BUFFERSIZE] = "$MEAS?";
	char inPacket[BUFFERSIZE];

	// sprintf_s(outPacket, BUFFERSIZE, "$TEST>And this is my count: %d", counter++);
	// sprintf_s(outPacket, BUFFERSIZE, "$MEAS?");

	// MyTestApp.sendReceiveSerial(&m_staticInfo, TRUE);	
	// if (!MyTestApp.sendReceiveSerial(1, strSendMeasureCommand, inPacket)) {

	if (!MyTestApp.sendReceiveSerial(1, outPacket, inPacket)) {
		errorCounter++;
		if (errorCounter > MAX_ERROR) {
			DeleteTimerQueueTimer(NULL, m_timerHandle, NULL);  // destroy the timer
			m_timerHandle = NULL;
			MyTestApp.closeTestSerialPort();			 
			char strError[32];
			sprintf_s(strError, "Error code: %d", intError);
			MyTestApp.DisplayMessageBox("COM PORT OR SYSTEM ERROR:", strError, 1);
		}
	}
	else {
		m_staticInfo.SetWindowText(inPacket);
		intError = NO_ERRORS;
		errorCounter = 0;
	}
	dummy++;
}

void CSerialCtrlDemoDlg::OnBnClickedMfcmenubutton1()
{
}

