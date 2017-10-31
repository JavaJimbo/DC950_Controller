
/* TestFunctions.cpp
 * Written in Visual C++ for DC950 test system
 *
 * 10-28-17: Testhandler() fully implemented, test sequence flow works well.
 */
// handleInterfaceBoard, handleHPmultiMeter, handleACpowerSupply;
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
		for (int i = 0; i < FINAL_PASS; i++) 
			testStep[i].Status = NOT_DONE_YET;		
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
		ptrDialog->m_static_optFilter.EnableWindow(TRUE);
		ptrDialog->m_static_optStandard.EnableWindow(TRUE);
		ptrDialog->m_static_modelGroup.EnableWindow(TRUE);
		ptrDialog->m_static_BarcodeLabel.ShowWindow(TRUE);
		enableBarcodeScan(ptrDialog);
		InitializeDisplayText();
	}

	TestApp::TestApp(CWnd* pParent) {
		
		// handleInterfaceBoard = handleHPmultiMeter = handleACpowerSupply = NULL;
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
		testStep[i].stepType = 0;
		testStep[i].enableENTER = 0;
		testStep[i].enablePASS = 0;
		testStep[i].enableFAIL = 0;
		testStep[i].enableHALT = 0;
		testStep[i].enablePREVIOUS = 0;
		i++;

		// 1 SCAN BARCODE
		testStep[i].testName = "New Unit";
		testStep[i].lineOne = "Scan barcode or type serial number";
		testStep[i].lineTwo = "in box above. Also select Model.";				
		testStep[i].lineThree = "Then press ENTER to begin tests.";
		testStep[i].lineFour = "";
		testStep[i].lineFive = "";
		testStep[i].lineSix = "";
		testStep[i].stepType = MANUAL;
		testStep[i].enableENTER = TRUE;
		testStep[i].enablePASS = FALSE;
		testStep[i].enableFAIL = FALSE;
		testStep[i].enableHALT = FALSE;
		testStep[i].enablePREVIOUS = FALSE;		
		i++;

		// 2 HI POT TEST
		testStep[i].testName = testStep[i].lineOne = "Hi-Pot Test:";
		testStep[i].lineTwo = "Connect tester, then run test.";
		testStep[i].lineThree = "Click PASS or FAIL to indicate results";
		testStep[i].lineFour = "Then click ENTER for next test.";
		testStep[i].lineFive = "";
		testStep[i].lineSix = "";
		testStep[i].stepType = MANUAL;
		testStep[i].enableENTER = FALSE;
		testStep[i].enablePASS = TRUE;
		testStep[i].enableFAIL = TRUE;		
		testStep[i].enableHALT = FALSE;
		testStep[i].enablePREVIOUS = TRUE;
		i++;

		// 3 GROUND BOND TEST
		testStep[i].testName = testStep[i].lineOne = "Ground Bond Test:";
		testStep[i].lineTwo = "Connect tester, then run test.";
		testStep[i].lineThree = "Click PASS or FAIL to indicate results";
		testStep[i].lineFour = "Then click ENTER for next test.";
		testStep[i].lineFive = "";
		testStep[i].lineSix = "";
		testStep[i].stepType = MANUAL;
		testStep[i].enableENTER = FALSE;
		testStep[i].enablePASS = TRUE;
		testStep[i].enableFAIL = TRUE;
		testStep[i].enableHALT = FALSE;
		testStep[i].enablePREVIOUS = TRUE;
		i++;

		// 4 POT TEST LOW
		testStep[i].testName = testStep[i].lineOne = "Pot Test LOW:";
		testStep[i].lineTwo = "Connect test leads to DC950 lamp.";
		testStep[i].lineThree = "Set switch on back to LOCAL";
		testStep[i].lineFour = "Turn POT to zero, then turn ON DC950";
		testStep[i].lineFive = "Press ENTER to run test";
		testStep[i].lineSix = "";
		testStep[i].stepType = MANUAL;
		testStep[i].enableENTER = TRUE;
		testStep[i].enablePASS = FALSE;
		testStep[i].enableFAIL = FALSE;
		testStep[i].enableHALT = FALSE;
		testStep[i].enablePREVIOUS = TRUE;
		i++;

		// 5 POT TEST HIGH
		testStep[i].testName = testStep[i].lineOne = "Pot Test HIGH:";
		testStep[i].lineTwo = "Turn POT to MAX power";
		testStep[i].lineThree = "Press ENTER to run test";
		testStep[i].lineFour = "";
		testStep[i].lineFive = "";
		testStep[i].lineSix = "";
		testStep[i].stepType = MANUAL;
		testStep[i].enableENTER = TRUE;
		testStep[i].enablePASS = FALSE;
		testStep[i].enableFAIL = FALSE;
		testStep[i].enableHALT = FALSE;
		testStep[i].enablePREVIOUS = TRUE;
		i++;

		// 6 REMOTE TEST SETUP
		testStep[i].testName = "Remote Pot Test:";
		testStep[i].lineOne = "Remote test setup:";
		testStep[i].lineTwo = "Turn pot back to zero";
		testStep[i].lineThree = "Set switch on back to REMOTE";
		testStep[i].lineFour = "Plug in DB9 connectors on back of DC950";
		testStep[i].lineFive = "Press ENTER when ready";
		testStep[i].lineSix = "";
		testStep[i].stepType = MANUAL; //  SUBSTEP;
		testStep[i].enableENTER = TRUE;
		testStep[i].enablePASS = FALSE;
		testStep[i].enableFAIL = FALSE;
		testStep[i].enableHALT = FALSE;
		testStep[i].enablePREVIOUS = TRUE;
		i++;
		
		// 7 REMOTE POT TEST
		testStep[i].testName = "Remote Pot Test:";
		testStep[i].lineOne = "Running Remote test";
		testStep[i].lineTwo = "Please wait...";
		testStep[i].lineThree = "";
		testStep[i].lineFour = "";
		testStep[i].lineFive = "";
		testStep[i].lineSix = "";
		testStep[i].stepType = AUTO;
		testStep[i].enableENTER = FALSE;
		testStep[i].enablePASS = FALSE;
		testStep[i].enableFAIL = FALSE;
		testStep[i].enableHALT = FALSE;
		testStep[i].enablePREVIOUS = FALSE;
		i++;

		// 8 AC POWER SWEEP
		testStep[i].testName = "AC Power Sweep:";
		testStep[i].lineOne = "Running Remote test";
		testStep[i].lineTwo = "Please wait...";
		testStep[i].lineThree = "";
		testStep[i].lineFour = "";
		testStep[i].lineFive = "";
		testStep[i].lineSix = "";
		testStep[i].stepType = AUTO;
		testStep[i].enableENTER = FALSE;
		testStep[i].enablePASS = FALSE;
		testStep[i].enableFAIL = FALSE;
		testStep[i].enableHALT = FALSE;
		testStep[i].enablePREVIOUS = TRUE;
		i++;

		// 9 ACTUATOR OPEN TEST
		testStep[i].testName = "Actuator OPEN Test:";
		testStep[i].lineOne = "Actuator should be all the way open,";
		testStep[i].lineTwo = "and light should should be fully visible.";
		testStep[i].lineThree = "";
		testStep[i].lineFour = "Click PASS if actuator is OPEN";
		testStep[i].lineFive = "Click FAIL if it is closed";
		testStep[i].lineSix = "";
		testStep[i].stepType = MANUAL;
		testStep[i].enableENTER = FALSE;
		testStep[i].enablePASS = TRUE;
		testStep[i].enableFAIL = TRUE;
		testStep[i].enableHALT = FALSE;
		testStep[i].enablePREVIOUS = TRUE;
		i++;

		// 10 ACTUATOR CLOSED TEST
		testStep[i].testName = "Actuator CLOSED Test:";
		testStep[i].lineOne = "Actuator should be completed closed,";
		testStep[i].lineTwo = "and no light should be shining";
		testStep[i].lineThree = "through hole.";
		testStep[i].lineFour = "Click PASS if actuator is CLOSED";
		testStep[i].lineFive = "Click FAIL if it is open";
		testStep[i].lineSix = "";
		testStep[i].stepType = MANUAL;
		testStep[i].enableENTER = FALSE;
		testStep[i].enablePASS = TRUE;
		testStep[i].enableFAIL = TRUE;
		testStep[i].enableHALT = FALSE;
		testStep[i].enablePREVIOUS = TRUE;
		i++;

		// 11 SPECTROMETER TEST
		testStep[i].testName = "Spectrometer Test:";
		testStep[i].lineOne = "Running Remote test";
		testStep[i].lineTwo = "Please wait...";
		testStep[i].lineThree = "";
		testStep[i].lineFour = "";
		testStep[i].lineFive = "";
		testStep[i].lineSix = "";
		testStep[i].stepType = AUTO;
		testStep[i].enableENTER = FALSE;
		testStep[i].enablePASS = FALSE;
		testStep[i].enableFAIL = FALSE;
		testStep[i].enableHALT = FALSE;
		testStep[i].enablePREVIOUS = FALSE;
		i++;

		// 12 TEST COMPLETE
		testStep[i].testName = "Spectrometer Test:";
		testStep[i].lineOne = "All tests PASSED";
		testStep[i].lineTwo = "Turn off DC950";
		testStep[i].lineThree = "Disconnect test leads";
		testStep[i].lineFour = "Then press ENTER to test next unit";
		testStep[i].lineFive = "";
		testStep[i].lineSix = "";
		testStep[i].stepType = MANUAL;
		testStep[i].enableENTER = TRUE;
		testStep[i].enablePASS = FALSE;
		testStep[i].enableFAIL = FALSE;
		testStep[i].enableHALT = FALSE;
		testStep[i].enablePREVIOUS = TRUE;
		i++;

		// 13 FAILED UNIT		
		testStep[i].testName = NULL;
		testStep[i].lineOne = "UNIT FAILED";
		testStep[i].lineTwo = "Turn off DC950";
		testStep[i].lineThree = "Disconnect test leads";
		testStep[i].lineFour = "Attach FAIL tag";
		testStep[i].lineFive = "Press ENTER to test new unit.";
		testStep[i].lineSix = "";
		testStep[i].stepType = MANUAL;
		testStep[i].enableENTER = TRUE;
		testStep[i].enablePASS = FALSE;
		testStep[i].enableFAIL = FALSE;
		testStep[i].enableHALT = FALSE;
		testStep[i].enablePREVIOUS = FALSE;
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
		char strReset[BUFFERSIZE] = "*RST\r\n";
		char strEnableRemote[BUFFERSIZE] = ":SYST:REM\r\n";
		char strMeasure[BUFFERSIZE] = ":MEAS?\r\n";		

		// 1) Send RESET command to HP34401:
		if (!sendReceiveSerial(HP_METER, ptrDialog, strReset, NULL, FALSE)) {
			DisplayMessageBox("HP MULTIMETER COM ERROR", "Make sure meter power is ON, \r\nCheck RS232 and USB cables", 1);
			return FALSE;
		}

		msDelay(500);

		// 2) Enable RS232 remote control on HP34401:
		if (!sendReceiveSerial(HP_METER, ptrDialog, strEnableRemote, NULL, FALSE)) {
			DisplayMessageBox("HP MULTIMETER COM ERROR", "Make sure meter power is ON, \r\nCheck RS232 and USB cables", 1);
			return FALSE;
		}

		msDelay(500);

		// Now try getting a measurement from the HP34401:
		if (!sendReceiveSerial(HP_METER, ptrDialog, strMeasure, NULL, TRUE)) {
			DisplayMessageBox("HP MULTIMETER COM ERROR", "Make sure meter power is ON, \r\nCheck RS232 and USB cables", 1);
			return FALSE;
		}

		return TRUE;
	}

	int TestApp::Execute(int stepNumber, CSerialCtrlDemoDlg *ptrDialog){		
		int testStatus = 0; // testStep[stepNumber].Status;		
		static BOOL runFilterActuatorTest = FALSE;

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
			runFilterActuatorTest = (BOOL)ptrDialog->m_static_optFilter.GetCheck();
			ptrDialog->m_static_optFilter.EnableWindow(FALSE);
			ptrDialog->m_static_optStandard.EnableWindow(FALSE);
			break;
		case 2:			// 2 HI POT TEST			
		case 3:			// 3 GROUND BOND TEST		
			testStatus = testStep[stepNumber].Status;
			break;
		case 4:			// 4 POT TEST LOW
			testStatus = PASS;
			break;
		case 5:			// 5 POT TEST HIGH
			testStatus = PASS;
			break;
		case 6:			// 6 REMOTE TEST SETUP
			testStatus = PASS;
			break;
		case 7:			// 7 REMOTE POT TEST
			testStatus = PASS;
			break;
		case 8:			// 8 AC POWER SWEEP
			testStatus = PASS;
			break;
		case 9:			// 9 ACTUATOR OPEN TEST			
			testStatus = PASS;
			break;
		case 10:		// 10 ACTUATOR CLOSED TEST			
			testStatus = PASS;
			break;
		case 11:		// 11 SPECTROMETER TEST
			testStatus = PASS;
			break;
		case 12:		// 12 TEST COMPLETE
			resetTestData();
			resetDisplays(ptrDialog);			
			testStatus = PASS;
			break;
		case 13:		// 13 UNIT FAILED
			resetTestData();
			resetDisplays(ptrDialog);			
			testStatus = FAIL;
		default:
			break;
		}
		testStep[stepNumber].Status = testStatus;
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
			if (testStep[stepNumber].Status == PASS) {
				ptrDialog->m_static_TestName.SetWindowText(testStep[stepNumber].testName);
				ptrDialog->m_static_TestResult.SetWindowText("PASS");
			}
			else if (testStep[stepNumber].Status == FAIL) {
				ptrDialog->m_static_TestName.SetWindowText(testStep[stepNumber].testName);
				ptrDialog->m_static_TestResult.SetWindowText("FAIL");
			}	
			else if (testStep[stepNumber].Status == NOT_DONE_YET) {
				ptrDialog->m_static_TestName.SetWindowText(testStep[stepNumber].testName);
				ptrDialog->m_static_TestResult.SetWindowText("PLEASE WAIT...");
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
			return (PASS);
		}
	}



	BOOL TestApp::InitializeSystem(CSerialCtrlDemoDlg *ptrDialog) {
		if (openTestSerialPort(portNameInterfaceBoard, &handleInterfaceBoard)
			&& openTestSerialPort(portNameMultiMeter, &handleHPmultiMeter)
				&& openTestSerialPort(portNameACpowerSupply, &handleACpowerSupply)){

			ptrDialog->m_static_DataOut.SetWindowText("COM Data Out");
			ptrDialog->m_static_DataIn.SetWindowText("COM Data In");
			if (InitializeHP34401(ptrDialog)) 
				return TRUE;
			else return FALSE;
		}
		else return FALSE;		
	}

	TestApp::~TestApp() {
		//closeSerialPort(ptrDialog->handleInterfaceBoard);
		//closeSerialPort(ptrDialog->handleHPmultiMeter);
		//closeSerialPort(ptrDialog->handleACpowerSupply);
	}
	