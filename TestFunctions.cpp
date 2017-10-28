
/* TestFunctions.cpp
 * Written in Visual C++ for DC950 test system
 *
 * 10-28-17: Testhandler() fully implemented, test sequence flow works well.
 */

// NOTE: INCUDES MUST BE IN THIS ORDER!!!
#include "stdafx.h"
#include "SerialCtrlDemo.h"
#include "SerialCtrlDemoDlg.h"
#include <string.h>
#include "TestApp.h"
#include "Definitions.h"

// int intError = 0;
extern int stepNumber;
CString strSerialNumber;

HANDLE gDoneEvent;
HANDLE hTimer = NULL;
HANDLE hTimerQueue = NULL;

	void TestApp::resetTestData() {
		for (int i = 0; i < FINAL_PASS; i++) {
			if (testStep[i].Result != SUBSTEP)
				testStep[i].Result = NOT_DONE;
		}
	}

	void TestApp::resetDisplays(CSerialCtrlDemoDlg *ptrDialog) {
		ptrDialog->m_BarcodeEditBox.SetWindowText("");
		ptrDialog->m_static_TestResult.SetWindowText("");
		ptrDialog->m_static_TestName.SetWindowText("New Unit");
		ptrDialog->m_static_ComOut.SetWindowText("Serial Com Out");
		ptrDialog->m_static_ComIn.SetWindowText("Serial Com In");
		ptrDialog->m_static_Line6.SetWindowText("");
		ptrDialog->m_BarcodeEditBox.ShowWindow(TRUE);
		ptrDialog->m_static_BarcodeLabel.ShowWindow(TRUE);
		ptrDialog->m_static_DataIn.SetWindowText("");
		ptrDialog->m_static_DataOut.SetWindowText("");		
	}

	TestApp::TestApp(CWnd* pParent) {		
		flgIniFileOpen = TRUE;
		flgHPmeterInitialized = FALSE;				

		TCHAR* pFileName = _T("INIfile.txt");
		try
		{
			CStdioFile fileHandle(pFileName, CFile::modeRead | CFile::typeText);
		}
		catch (CFileException* pe)
		{
			MessageBox("Cannot open INIfile.txt", "FILE ERROR", MB_OK);
			flgIniFileOpen = FALSE;
			pe->Delete();
			// intError = SYSTEM_ERROR;
		}		
	}

	BOOL TestApp::InitializeDisplayText() {	
		int i = 0;
		// 0 PRESET TEXT doesn't get used, but initialize it anyway: 
		testStep[i].testName = NULL;
		testStep[i].lineOne = NULL;
		testStep[i].lineTwo = NULL;
		testStep[i].lineThree = NULL;
		testStep[i].lineFour = NULL;
		testStep[i].lineFive = NULL;
		testStep[i].lineSix = NULL;
		testStep[i].testType = 0;
		testStep[i].enableENTER = 0;
		testStep[i].enablePASS = 0;
		testStep[i].enableFAIL = 0;
		testStep[i].enableHALT = 0;
		testStep[i].enablePREVIOUS = 0;
		testStep[i].Result = 0;
		i++;

		// 1 SCAN BARCODE
		testStep[i].testName = "New Unit";
		testStep[i].lineOne = "Scan barcode";
		testStep[i].lineTwo = "or type serial number";				
		testStep[i].lineThree = "in box above. Then click ENTER";
		testStep[i].lineFour = "";
		testStep[i].lineFive = "";
		testStep[i].lineSix = "";
		testStep[i].testType = MANUAL;
		testStep[i].enableENTER = TRUE;
		testStep[i].enablePASS = FALSE;
		testStep[i].enableFAIL = FALSE;
		testStep[i].enableHALT = FALSE;
		testStep[i].enablePREVIOUS = FALSE;		
		testStep[i].Result = NOT_DONE;
		i++;

		// 2 HI POT TEST
		testStep[i].testName = testStep[i].lineOne = "Hi-Pot Test:";
		testStep[i].lineTwo = "Connect tester, then run test.";
		testStep[i].lineThree = "Click PASS or FAIL to indicate results";
		testStep[i].lineFour = "Then click ENTER for next test.";
		testStep[i].lineFive = "";
		testStep[i].lineSix = "";
		testStep[i].testType = MANUAL;
		testStep[i].enableENTER = FALSE;
		testStep[i].enablePASS = TRUE;
		testStep[i].enableFAIL = TRUE;		
		testStep[i].enableHALT = FALSE;
		testStep[i].enablePREVIOUS = TRUE;
		testStep[i].Result = NOT_DONE;
		i++;

		// 3 GROUND BOND TEST
		testStep[i].testName = testStep[i].lineOne = "Ground Bond Test:";
		testStep[i].lineTwo = "Connect tester, then run test.";
		testStep[i].lineThree = "Click PASS or FAIL to indicate results";
		testStep[i].lineFour = "Then click ENTER for next test.";
		testStep[i].lineFive = "";
		testStep[i].lineSix = "";
		testStep[i].testType = MANUAL;
		testStep[i].enableENTER = FALSE;
		testStep[i].enablePASS = TRUE;
		testStep[i].enableFAIL = TRUE;
		testStep[i].enableHALT = FALSE;
		testStep[i].enablePREVIOUS = TRUE;
		testStep[i].Result = NOT_DONE;
		i++;

		// 4 POT TEST LOW
		testStep[i].testName = testStep[i].lineOne = "Pot Test LOW:";
		testStep[i].lineTwo = "Connect test leads to DC950 lamp.";
		testStep[i].lineThree = "Set switch on back to LOCAL";
		testStep[i].lineFour = "Turn POT to zero, then turn ON DC950";
		testStep[i].lineFive = "Press ENTER to run test";
		testStep[i].lineSix = "";
		testStep[i].testType = MANUAL;
		testStep[i].enableENTER = TRUE;
		testStep[i].enablePASS = FALSE;
		testStep[i].enableFAIL = FALSE;
		testStep[i].enableHALT = FALSE;
		testStep[i].enablePREVIOUS = TRUE;
		testStep[i].Result = NOT_DONE;
		i++;

		// 5 POT TEST HIGH
		testStep[i].testName = testStep[i].lineOne = "Pot Test HIGH:";
		testStep[i].lineTwo = "Turn POT to MAX power";
		testStep[i].lineThree = "Press ENTER to run test";
		testStep[i].lineFour = "";
		testStep[i].lineFive = "";
		testStep[i].lineSix = "";
		testStep[i].testType = MANUAL;
		testStep[i].enableENTER = TRUE;
		testStep[i].enablePASS = FALSE;
		testStep[i].enableFAIL = FALSE;
		testStep[i].enableHALT = FALSE;
		testStep[i].enablePREVIOUS = TRUE;
		testStep[i].Result = NOT_DONE;
		i++;

		// 6 REMOTE TEST SETUP
		testStep[i].testName = "Remote Pot Test:";
		testStep[i].lineOne = "Remote test setup:";
		testStep[i].lineTwo = "Turn pot back to zero";
		testStep[i].lineThree = "Set switch on back to REMOTE";
		testStep[i].lineFour = "Plug in DB9 connectors on back of DC950";
		testStep[i].lineFive = "Press ENTER when ready";
		testStep[i].lineSix = "";
		testStep[i].testType = MANUAL;
		testStep[i].enableENTER = TRUE;
		testStep[i].enablePASS = FALSE;
		testStep[i].enableFAIL = FALSE;
		testStep[i].enableHALT = FALSE;
		testStep[i].enablePREVIOUS = TRUE;
		testStep[i].Result = NOT_DONE;
		i++;
		
		// 7 REMOTE POT TEST
		testStep[i].testName = "Remote Pot Test:";
		testStep[i].lineOne = "Running Remote test";
		testStep[i].lineTwo = "Please wait...";
		testStep[i].lineThree = "";
		testStep[i].lineFour = "";
		testStep[i].lineFive = "";
		testStep[i].lineSix = "";
		testStep[i].testType = AUTO;
		testStep[i].enableENTER = FALSE;
		testStep[i].enablePASS = FALSE;
		testStep[i].enableFAIL = FALSE;
		testStep[i].enableHALT = FALSE;
		testStep[i].enablePREVIOUS = FALSE;
		testStep[i].Result = NOT_DONE;
		i++;

		// 8 AC POWER SWEEP
		testStep[i].testName = "AC Power Sweep:";
		testStep[i].lineOne = "Running Remote test";
		testStep[i].lineTwo = "Please wait...";
		testStep[i].lineThree = "";
		testStep[i].lineFour = "";
		testStep[i].lineFive = "";
		testStep[i].lineSix = "";
		testStep[i].testType = AUTO;
		testStep[i].enableENTER = FALSE;
		testStep[i].enablePASS = FALSE;
		testStep[i].enableFAIL = FALSE;
		testStep[i].enableHALT = FALSE;
		testStep[i].enablePREVIOUS = TRUE;
		testStep[i].Result = NOT_DONE;
		i++;

		// 9 ACTUATOR TEST
		testStep[i].testName = "Actuator Test:";
		testStep[i].lineOne = "Running Remote test";
		testStep[i].lineTwo = "Please wait...";
		testStep[i].lineThree = "";
		testStep[i].lineFour = "";
		testStep[i].lineFive = "";
		testStep[i].lineSix = "";
		testStep[i].testType = AUTO;
		testStep[i].enableENTER = FALSE;
		testStep[i].enablePASS = FALSE;
		testStep[i].enableFAIL = FALSE;
		testStep[i].enableHALT = FALSE;
		testStep[i].enablePREVIOUS = TRUE;
		testStep[i].Result = NOT_DONE;
		i++;

		// 10 SPECTROMETER TEST
		testStep[i].testName = "Spectrometer Test:";
		testStep[i].lineOne = "Running Remote test";
		testStep[i].lineTwo = "Please wait...";
		testStep[i].lineThree = "";
		testStep[i].lineFour = "";
		testStep[i].lineFive = "";
		testStep[i].lineSix = "";
		testStep[i].testType = AUTO;
		testStep[i].enableENTER = FALSE;
		testStep[i].enablePASS = FALSE;
		testStep[i].enableFAIL = FALSE;
		testStep[i].enableHALT = FALSE;
		testStep[i].enablePREVIOUS = FALSE;
		testStep[i].Result = NOT_DONE;
		i++;

		// 11 TEST COMPLETE
		testStep[i].testName = "Spectrometer Test:";
		testStep[i].lineOne = "All tests PASSED";
		testStep[i].lineTwo = "Turn off DC950";
		testStep[i].lineThree = "Disconnect test leads";
		testStep[i].lineFour = "Then press ENTER to test next unit";
		testStep[i].lineFive = "";
		testStep[i].lineSix = "";
		testStep[i].testType = MANUAL;
		testStep[i].enableENTER = TRUE;
		testStep[i].enablePASS = FALSE;
		testStep[i].enableFAIL = FALSE;
		testStep[i].enableHALT = FALSE;
		testStep[i].enablePREVIOUS = TRUE;
		testStep[i].Result = SUBSTEP;
		i++;

		// 12 FAILED UNIT		
		testStep[i].testName = NULL;
		testStep[i].lineOne = "UNIT FAILED";
		testStep[i].lineTwo = "Turn off DC950";
		testStep[i].lineThree = "Disconnect test leads";
		testStep[i].lineFour = "Attach FAIL tag";
		testStep[i].lineFive = "Press ENTER to test new unit.";
		testStep[i].lineSix = "";
		testStep[i].testType = MANUAL;
		testStep[i].enableENTER = TRUE;
		testStep[i].enablePASS = FALSE;
		testStep[i].enableFAIL = FALSE;
		testStep[i].enableHALT = FALSE;
		testStep[i].enablePREVIOUS = FALSE;
		testStep[i].Result = SUBSTEP;
		return TRUE;
	}

	void TestApp::DisplayStepNumber(CSerialCtrlDemoDlg *ptrDialog, int stepNumber) {
		char strStepNumber[16];
		sprintf_s(strStepNumber, "Step #%d", stepNumber);
		ptrDialog->m_static_Line6.SetWindowText(strStepNumber);
	}

	void TestApp::DisplaySerialComData(CSerialCtrlDemoDlg *ptrDialog, int comDirection, char *strData) {
		if (comDirection == DATAIN) ptrDialog->m_static_DataIn.SetWindowText((LPCTSTR) strData);
		else ptrDialog->m_static_DataOut.SetWindowText((LPCTSTR)strData);
	}

	// Displays user instructions for each test, and enables/disables buttons as needed		
	void TestApp::DisplayIntructions(int stepNumber, CSerialCtrlDemoDlg *ptrDialog){	
		char *ptrText = NULL;

		// Once STARTUP is done, change "READY" to "ENTER" text on button:
		if (stepNumber == 1) ptrDialog->m_static_ButtonEnter.SetWindowText("ENTER"); 

		// Display user instructions on five text lines:
		ptrText = testStep[stepNumber].lineOne;
		if (ptrText!= NULL) ptrDialog->m_static_Line1.SetWindowText(ptrText);

		ptrText = testStep[stepNumber].lineTwo;
		if (ptrText != NULL) ptrDialog->m_static_Line2.SetWindowText(ptrText);

		ptrText = testStep[stepNumber].lineThree;
		if (ptrText != NULL) ptrDialog->m_static_Line3.SetWindowText(ptrText);

		ptrText = testStep[stepNumber].lineFour;
		if (ptrText != NULL) ptrDialog->m_static_Line4.SetWindowText(ptrText);

		ptrText = testStep[stepNumber].lineFive;
		if (ptrText != NULL) ptrDialog->m_static_Line5.SetWindowText(ptrText);

		// Enable or disable buttons depending on which step will execute next:
		ptrDialog->m_static_ButtonPass.EnableWindow(testStep[stepNumber].enablePASS);
		ptrDialog->m_static_ButtonFail.EnableWindow(testStep[stepNumber].enableFAIL);
		ptrDialog->m_static_ButtonEnter.EnableWindow(testStep[stepNumber].enableENTER);
		ptrDialog->m_static_ButtonPrevious.EnableWindow(testStep[stepNumber].enablePREVIOUS);
		ptrDialog->m_static_ButtonHalt.EnableWindow(testStep[stepNumber].enableHALT);
	}
	

	TestApp::~TestApp() {
		TestApp::closeTestSerialPort();
	}

	VOID CALLBACK TimerRoutine(PVOID lpParam, BOOLEAN TimerOrWaitFired)
	{
		SetEvent(gDoneEvent);
	}

	void TestApp::msDelay(int milliseconds) {		
		gDoneEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
		hTimerQueue = CreateTimerQueue();
		int arg = 123;
		CreateTimerQueueTimer(&hTimer, hTimerQueue, (WAITORTIMERCALLBACK)TimerRoutine, &arg, milliseconds, 0, 0);
		WaitForSingleObject(gDoneEvent, INFINITE);
		CloseHandle(gDoneEvent);
	}
	

	BOOL TestApp::InitializeHP34401(CSerialCtrlDemoDlg *ptrDialog) {
		char strReset[BUFFERSIZE] = "$RESET";
		char strEnableRemote[BUFFERSIZE] = "$REMOTE";
		char strResponse[BUFFERSIZE];
				
		if (flgHPmeterInitialized) return (TRUE);

		// 1) Send RESET command to HP34401:
		if (!sendReceiveSerial(ptrDialog, 1, strReset, strResponse)) {
			DisplayMessageBox("Inteface board COM error", "Check RS232 connections and POWER ON LED", 1);
			return (FALSE);
		}

		msDelay(500);

		// 2) Enable RS232 remote control:
		if (!sendReceiveSerial(ptrDialog, 1, strEnableRemote, strResponse)) {
			DisplayMessageBox("Inteface board COM error", "Check RS232 connections and POWER ON LED", 1);
			return (FALSE);
		}

		flgHPmeterInitialized = TRUE;
		return(TRUE);
	}

	int TestApp::Execute(int stepNumber, CSerialCtrlDemoDlg *ptrDialog){
		
		int testStatus = 0; // testStep[stepNumber].Result;

		switch (stepNumber) {
		case 0:			// 0 Start Up
			testStatus = SystemStartUp(ptrDialog);
			break;
		case 1:			// 1 SCAN BARCODE			
			ptrDialog->m_BarcodeEditBox.GetWindowText(strSerialNumber);
			if (0 == strcmp(strSerialNumber, "")) {
				testStatus = FAIL;				
				break;
			}
			testStatus = PASS;						
			break;
		case 2:			// 2 HI POT TEST			
		case 3:			// 3 GROUND BOND TEST		
			testStatus = testStep[stepNumber].Result;
			break;
		case 4:			// 4 POT TEST LOW
			testStatus = PASS;
			break;
		case 5:			// 5 POT TEST HIGH
			testStatus = PASS;
			break;
		case 6:			// 6 REMOTE TEST SETUP
			testStatus = SUBSTEP;
			break;
		case 7:			// 6 REMOTE POT TEST
			testStatus = PASS;
			break;
		case 8:			// 5 AC POWER SWEEP
			testStatus = PASS;
			break;
		case 9:		// 8 ACTUATOR TEST
			testStatus = PASS;
			break;
		case 10:		// 9 SPECTROMETER TEST
			testStatus = PASS;
			break;
		case 11:		// 10 TEST COMPLETE
			resetTestData();
			resetDisplays(ptrDialog);			
			testStatus = PASS;
			break;
		case 12:		// 11 UNIT FAILED
			resetTestData();
			resetDisplays(ptrDialog);			
			testStatus = FAIL;
		default:
			break;
		}
		testStep[stepNumber].Result = testStatus;
		return testStatus;
	}
	
	void TestApp::DisplayStatusBarText(CSerialCtrlDemoDlg *ptrDialog, int panel, LPCTSTR strText) {
		ptrDialog->m_StatusBar.SetPaneText(panel, (LPCTSTR)strText);
	}
		
	void TestApp::DisplayText(CSerialCtrlDemoDlg *ptrDialog, int lineNumber, LPCTSTR strText) {
		switch (lineNumber) {
		case 1:
			ptrDialog->m_static_Line1.SetWindowText(strText);
			break;
		case 2:
			ptrDialog->m_static_Line2.SetWindowText(strText);
			break;
		case 3:
			ptrDialog->m_static_Line3.SetWindowText(strText);
			break;
		case 4:
			ptrDialog->m_static_Line4.SetWindowText(strText);
			break;
		case 5:
			ptrDialog->m_static_Line5.SetWindowText(strText);
			break;
		case 6:
			ptrDialog->m_static_Line6.SetWindowText(strText);
			break;
		default:
			break;
		}		
	}


	BOOL TestApp::DisplayMessageBox(LPCTSTR strTopLine, LPCTSTR strBottomLine, int boxType)
	{
		BOOL tryAgain = false;
		int msgBoxID;

		if (boxType == 3) msgBoxID = MessageBox(strBottomLine, strTopLine, MB_ICONWARNING | MB_CANCELTRYCONTINUE | MB_DEFBUTTON2);
		else if (boxType == 2) msgBoxID = MessageBox(strBottomLine, strTopLine, MB_ICONWARNING | MB_RETRYCANCEL | MB_DEFBUTTON2);
		else msgBoxID = MessageBox(strBottomLine, strTopLine, MB_ICONWARNING | MB_OK | MB_DEFBUTTON2);

		switch (msgBoxID)
		{
		case IDTRYAGAIN:
		case IDRETRY:
			tryAgain = true;
			break;
		case IDCANCEL:
		case IDOK:
		case IDCONTINUE:
		default:
			tryAgain = false;
			break;
		}
		return tryAgain;
	}
		
	void TestApp::DisplayPassFailStatus(int stepNumber, CSerialCtrlDemoDlg *ptrDialog) {
		if (stepNumber > STARTUP && stepNumber < FINAL_PASS) {
			if (testStep[stepNumber].Result == PASS) {
				ptrDialog->m_static_TestName.SetWindowText(testStep[stepNumber].testName);
				ptrDialog->m_static_TestResult.SetWindowText("PASS");
			}
			else if (testStep[stepNumber].Result == FAIL) {
				ptrDialog->m_static_TestName.SetWindowText(testStep[stepNumber].testName);
				ptrDialog->m_static_TestResult.SetWindowText("FAIL");
			}			
		}
	}
	
	void TestApp::enableBarcodeScan(CSerialCtrlDemoDlg *ptrDialog) {		
		ptrDialog->m_BarcodeEditBox.SetFocus();
		ptrDialog->m_BarcodeEditBox.ShowCaret();
		ptrDialog->m_BarcodeEditBox.SendMessage(EM_SETREADONLY, 0, 0);
	}

	void TestApp::disableBarcodeScan(CSerialCtrlDemoDlg *ptrDialog) {
		ptrDialog->m_BarcodeEditBox.SendMessage(EM_SETREADONLY, 1, 0);
	}

	
	// This is called once, when ENTER button
	// has been pushed the first time.
	UINT TestApp::SystemStartUp(CSerialCtrlDemoDlg *ptrDialog)
	{
		if (!InitializeSystem(ptrDialog)) return (FAIL);		
		else {			
			resetDisplays(ptrDialog);
			ptrDialog->m_static_BarcodeLabel.ShowWindow(TRUE);
			return (PASS);
		}
	}

	BOOL TestApp::InitializeSystem(CSerialCtrlDemoDlg *ptrDialog) {
		if (openTestSerialPort()) {
			ptrDialog->m_static_DataOut.SetWindowText("COM Data Out");
			ptrDialog->m_static_DataIn.SetWindowText("COM Data In");
			if (InitializeHP34401(ptrDialog)) return TRUE;
			else return FALSE;
		}
		else return FALSE;		
	}

	