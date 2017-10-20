/* SerialCtrlDemo.cpp : Defines the class behaviors for the application.
 *
 * $$$$ Modified to work with HP 34401A multimeter
 *  For commands see HP 344A Multimeter User's Guide 
 *  Part Number 34401-90004
 *
 *  Minimum command sequence to communicate with HP meter:
 *	1) Send RESET command:			"*RST\r\n"
 *  2) Enable RS232 remote control: ":SYST:REM\r\n"
 *  3) Send measurement command:	":MEAS?\r\n"
 *
 *	9-30-17 JBS: Compiled and checked 
 *	10-10-17 JBS: Removed unused combo boxes, list boxes, buttons, and text from main form
 *		Eliminated files SerialCtl.cpp and SerialCtl.h
 *  10-12-17: sendReceiveSerial() sends and receives commands via RS232. 
 *				Non-blocking timer added to SerialCtrlDemoDlg 
 *				sendReceiveSerial() displays text on form.
 *	10-15-17: Added pop up message boxes for error handling
 */

#include "stdafx.h"
#include "SerialCtrlDemo.h"
#include "SerialCtrlDemoDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


BEGIN_MESSAGE_MAP(CSerialCtrlDemoApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CSerialCtrlDemoApp construction
CSerialCtrlDemoApp::CSerialCtrlDemoApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CSerialCtrlDemoApp object
CSerialCtrlDemoApp theApp;


// CSerialCtrlDemoApp initialization
BOOL CSerialCtrlDemoApp::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	CSerialCtrlDemoDlg dlg;		
	m_pMainWnd = &dlg;	
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
