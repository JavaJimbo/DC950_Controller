
/* TestFunctions.cpp
 * Written in Visual C++ for DC950 test system
 *
 * 10-28-17: Testhandler() fully implemented, test sequence flow works well.
 * 11-1-17: All tests up and running except spectrometer. Spreadsheet not done yet.
 *	$$$$ comment out power supply commands
 */

// NOTE: INCUDES MUST BE IN THIS ORDER!!!
#include "stdafx.h"
#include "SerialCtrlDemo.h"
#include "SerialCtrlDemoDlg.h"
#include <string.h>
#include "TestApp.h"
#include "Definitions.h"

// int intError = 0;

// extern int stepNumber, subStepNumber;
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
		handleInterfaceBoard = handleHPmultiMeter = handleACpowerSupply = NULL;
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
		testStep[i].lineOne = "Scan barcode and select model";
		testStep[i].lineTwo = "Set switch on DC950 to LOCAL";
		testStep[i].lineThree = "Plug it into power supply";
		testStep[i].lineFour = "Flip DC950 power switch to ON";
		testStep[i].lineFive = "Click ENTER to begin tests.";
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
		testStep[i].lineThree = "Click PASS or FAIL";
		testStep[i].lineFour = "";
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
		testStep[i].lineThree = "Click PASS or FAIL";
		testStep[i].lineFour = "";
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
		testStep[i].lineTwo = "Connect leads to lamp.";
		testStep[i].lineThree = "Set switch to LOCAL";
		testStep[i].lineFour = "Turn POT to zero, turn ON the DC950";
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
		testStep[i].testName = "Remote Control Tests:";
		testStep[i].lineOne = "Remote test setup:";
		testStep[i].lineTwo = "Turn pot back to zero";
		testStep[i].lineThree = "Switch DC950 to REMOTE";
		testStep[i].lineFour = "Plug in DB9 connectors";
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
		testStep[i].lineOne = "Running AC Power Sweep";
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
		testStep[i].testName = "Actuator OPEN test:";
		testStep[i].lineOne = "Actuator should be OPEN";
		testStep[i].lineTwo = "Lamp should be ON";
		testStep[i].lineThree = "";
		testStep[i].lineFour = "Click PASS if OPEN";
		testStep[i].lineFive = "Click FAIL if CLOSED";
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
		testStep[i].lineOne = "Actuator should be CLOSED";
		testStep[i].lineTwo = "Lamp should be ON";
		testStep[i].lineThree = "";
		testStep[i].lineFour = "Click PASS if CLOSED";
		testStep[i].lineFive = "Click FAIL if OPEN";
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
		testStep[i].lineFour = "Click ENTER to test next unit";
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
	
	TestApp::~TestApp() {
		closeSerialPort(handleInterfaceBoard);
		closeSerialPort(handleHPmultiMeter);
		closeSerialPort(handleACpowerSupply);
	}
	

	
	
	void TestApp::DisplayPassFailStatus(int stepNumber, int subStepNumber, CSerialCtrlDemoDlg *ptrDialog) {
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
				if (!subStepNumber) {
					ptrDialog->m_static_TestName.SetWindowText(testStep[stepNumber].testName);
					ptrDialog->m_static_TestResult.SetWindowText("PLEASE WAIT");
				}
				else {
					ptrDialog->m_static_TestName.SetWindowText(testStep[stepNumber].testName);
					char strSubsStep[32];
					sprintf_s(strSubsStep, "Substep #%d", subStepNumber);
					ptrDialog->m_static_TestResult.SetWindowText(strSubsStep);
				}
			}
		}
	}

	void TestApp::DisplayStepNumber(CSerialCtrlDemoDlg *ptrDialog, int stepNumber) {
		char strStepNumber[16];
		sprintf_s(strStepNumber, "Step #%d", stepNumber);
		ptrDialog->m_static_Line6.SetWindowText(strStepNumber);
	}

	BOOL TestApp::InitializeSystem(CSerialCtrlDemoDlg *ptrDialog) {
		if (openTestSerialPort(portNameInterfaceBoard, &handleInterfaceBoard)
			&& openTestSerialPort(portNameMultiMeter, &handleHPmultiMeter)
			&& openTestSerialPort(portNameACpowerSupply, &handleACpowerSupply)) {
			ptrDialog->m_static_DataOut.SetWindowText("COM Data Out");
			ptrDialog->m_static_DataIn.SetWindowText("COM Data In");			
			if (InitializeHP34401(ptrDialog) && InitializeInterfaceBoard(ptrDialog) && InitializePowerSupply(ptrDialog))			
				return TRUE;
			else return FALSE;
			
			return TRUE;
		}
		else return FALSE;
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

	BOOL TestApp::InitializeInterfaceBoard(CSerialCtrlDemoDlg *ptrDialog) {
		char strReset[BUFFERSIZE] = "$RESET";
		char strResponse[BUFFERSIZE] = "";

		// Send RESET command to interface board:
		if (!sendReceiveSerial(INTERFACE_BOARD, ptrDialog, strReset, strResponse, TRUE)) {
			DisplayMessageBox("INTERFACE BOARD COM ERROR", "Make sure Interface board power is ON, \r\nCheck RS232 and USB cables", 1);
			return FALSE;
		}
		else if (!strstr(strResponse, "OK")) {
			DisplayMessageBox("INTERFACE BOARD COMMUNICATION ERROR", "Check interface board cables and power", 1);
			return FALSE;
		}
		return TRUE;
	}

	
		
	BOOL TestApp::ReadVoltage(CSerialCtrlDemoDlg *ptrDialog, int multiplexerSelect, float *ptrLampVoltage)
	{
		char strResponse[BUFFERSIZE] = "";
		char strReadVoltage[BUFFERSIZE] = ":MEAS?\r\n";		

		// Set relays on interface board for desired voltage source:
		if (!SetInterfaceBoardMulitplexer(ptrDialog, multiplexerSelect)) return FALSE;

		msDelay(500);

		// Send READ VOLTAGE command to multimeter and get response:
		if (!sendReceiveSerial(HP_METER, ptrDialog, strReadVoltage, strResponse, TRUE)) {
			DisplayMessageBox("HP METER COM ERROR", "Make sure AC power supply is ON, \r\nCheck RS232 and USB cables", 1);
			return FALSE;
		}
		// If communication with multimeter was successful, convert response string to float value:
		else *ptrLampVoltage = (float) atof(strResponse);

		return TRUE;
	}
		
	BOOL TestApp::SetInterfaceBoardMulitplexer(CSerialCtrlDemoDlg *ptrDialog, int multiplexerSelect)
	{
		char strCommand[BUFFERSIZE];
		char strResponse[BUFFERSIZE];

		switch (multiplexerSelect) {
		default:
		case LAMP:
			strcpy_s(strCommand, BUFFERSIZE, "$LAMP");
			break;
		case VREF:
			strcpy_s(strCommand, BUFFERSIZE, "$VREF");
			break;
		case CONTROL_VOLTAGE:		
			strcpy_s(strCommand, BUFFERSIZE, "$CTRL");
			break;
		}

		// Send RESET command to interface board:
		if (!sendReceiveSerial(INTERFACE_BOARD, ptrDialog, strCommand, strResponse, TRUE)) {
			DisplayMessageBox("INTERFACE BOARD COM ERROR", "Make sure Interface board power is ON, \r\nCheck RS232 and USB cables", 1);
			return FALSE;
		}
		else if (!strstr(strResponse, "OK")) {
			DisplayMessageBox("INTERFACE BOARD COMMUNICATION ERROR", "Check interface board cables and power", 1);
			return FALSE;
		}
		return TRUE;
	}

	BOOL TestApp::SetInterfaceBoardPWM(CSerialCtrlDemoDlg *ptrDialog, int PWMvalue)
	{
		char strCommand[BUFFERSIZE] = "$PWM>";
		char strValue[BUFFERSIZE], strResponse[BUFFERSIZE];

		sprintf_s(strValue, BUFFERSIZE, "%d", PWMvalue);
		strcat_s(strCommand, BUFFERSIZE, strValue);

		// Send SET PWM command to interface board:
		if (!sendReceiveSerial(INTERFACE_BOARD, ptrDialog, strCommand, strResponse, TRUE)) {
			DisplayMessageBox("INTERFACE BOARD COM ERROR", "Make sure Interface board power is ON, \r\nCheck RS232 and USB cables", 1);
			return FALSE;
		}
		else if (!strstr(strResponse, "OK")) {
			DisplayMessageBox("INTERFACE BOARD COMMUNICATION ERROR", "Check interface board cables and power", 1);
			return FALSE;
		}
		return TRUE;
	}
	
	// This routine executes the following sequence:
	// 0) The PWM is set to 0, the relays are set to read the control voltage
	// 1) The control voltage is measured, then relays are set to monitor lamp voltage
	// 2) The lamp voltage is measured, the PWM is set to the next setpoint, and the relays are set to read control voltage
	// 3) Steps 1) and 2) are repeated for 1,2,3,4,5 volts
	// 4) The linearity error is calculated for each lamp voltage
	// 5) The PWM is set to full and the relays are set to monitor lamp voltage
	// 6) Lamp voltage is measured and the inhibit relay is turned on
	// 7) Lamp voltage is measured, then the PWM is turned to 0
	// 8) If any of the above steps fails, the failure is reported and the test is halted, otherwise it passes.
	int TestApp::RunRemoteTests(CSerialCtrlDemoDlg *ptrDialog)
	{
		#define NUM_TEST_VALUES 7
		int testPWM;
		int PWMvalues[NUM_TEST_VALUES] = { 0, 1243, 2486, 3729, 4972, 6215, 6215};		
		float lampVoltage;
		long lampVoltageX10, expectedVoltageX10, errorVoltageX10;

		if (ptrDialog->subStepNumber > NUM_TEST_VALUES) return FAIL;
		if (ptrDialog->subStepNumber < 0) return FAIL;			
		testPWM = PWMvalues[ptrDialog->subStepNumber];
		
		expectedVoltageX10 = (testPWM * 210) / 6215;
		if (ptrDialog->subStepNumber == (NUM_TEST_VALUES-1)) {
			SetInhibitRelay(ptrDialog, TRUE);
			expectedVoltageX10 = 0;
		}
		else SetInhibitRelay(ptrDialog, FALSE);		

		msDelay(100);
		if (!SetInterfaceBoardPWM(ptrDialog, testPWM)) return FAIL;	
		msDelay(100);
		if (!ReadVoltage(ptrDialog, LAMP, &lampVoltage)) return FAIL;

		lampVoltageX10 = abs((long)(lampVoltage * 10.0));		
		errorVoltageX10 = abs(lampVoltageX10 - expectedVoltageX10);

		if (errorVoltageX10 > MAX_LAMP_x10_VOLTAGE_ERROR) return FAIL;

		ptrDialog->subStepNumber++;
		if (ptrDialog->subStepNumber >= NUM_TEST_VALUES){			
			InitializeInterfaceBoard(ptrDialog);
			return (PASS);
		}
		else return NOT_DONE_YET;
	}

	BOOL TestApp::SetInhibitRelay(CSerialCtrlDemoDlg *ptrDialog, BOOL flagON)
	{
		char strCommand[BUFFERSIZE];
		char strResponse[BUFFERSIZE];

		if (flagON) strcpy_s(strCommand, BUFFERSIZE, "$INHIBIT>ON");
		else strcpy_s(strCommand, BUFFERSIZE, "$INHIBIT>OFF");

		// Send RESET command to interface board:
		if (!sendReceiveSerial(INTERFACE_BOARD, ptrDialog, strCommand, strResponse, TRUE)) {
			DisplayMessageBox("INTERFACE BOARD COM ERROR", "Make sure Interface board power is ON, \r\nCheck RS232 and USB cables", 1);
			return FALSE;
		}
		else if (!strstr(strResponse, "OK")) {
			DisplayMessageBox("INTERFACE BOARD COMMUNICATION ERROR", "Check interface board cables and power", 1);
			return FALSE;
		}
		return TRUE;
	}
	

	int TestApp::Execute(int stepNumber, CSerialCtrlDemoDlg *ptrDialog) {
		int testStatus = PASS; // testStep[stepNumber].Status;				
		float fltVoltage;
		int intVoltageX10;
		static BOOL flagStartRemoteTest = TRUE;

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
			ptrDialog->runFilterActuatorTest = (BOOL)ptrDialog->m_static_optFilter.GetCheck();
			ptrDialog->m_static_optFilter.EnableWindow(FALSE);
			ptrDialog->m_static_optStandard.EnableWindow(FALSE);
			ptrDialog->subStepNumber = 0;
			InitializeInterfaceBoard(ptrDialog);
			break;
		case 2:			// 2 HI POT TEST			
		case 3:			// 3 GROUND BOND TEST		
			testStatus = testStep[stepNumber].Status;
			break;
		case 4:			// 4 POT TEST LOW			
			ReadVoltage(ptrDialog, LAMP, &fltVoltage);
			if (abs(((int)(fltVoltage * 10)) - 0)  > MAX_LAMP_x10_VOLTAGE_ERROR) testStatus = FAIL;
			break;
		case 5:			// 5 POT TEST HIGH
			ReadVoltage(ptrDialog, LAMP, &fltVoltage);
			intVoltageX10 = abs((int)(fltVoltage * 10.0));
			if (abs(intVoltageX10 - 210)  > MAX_LAMP_x10_VOLTAGE_ERROR) testStatus = FAIL;
			break;
		case 6:			// 6 REMOTE TEST SETUP
			flagStartRemoteTest = TRUE;
			break;
		case 7:			// 7 REMOTE POT TEST
			testStatus = RunRemoteTests(ptrDialog);
			if (testStatus != NOT_DONE_YET) {
				testStatus = testStatus;
			}
			break;
		case 8:			// 8 AC POWER SWEEP
			testStatus = RunPowerSupplyTest(ptrDialog);
			if (testStatus == PASS && ptrDialog->runFilterActuatorTest){
				if (!SetInterfaceBoardActuatorOutput(ptrDialog, TRUE)) testStatus = FAIL;
			}
			break;
		case 9:			// 9 ACTUATOR OPEN TEST			
			if (!SetInterfaceBoardActuatorOutput(ptrDialog, FALSE)) testStatus = FAIL;
			else testStatus = testStep[stepNumber].Status;
			break;
		case 10:		// 10 ACTUATOR CLOSED TEST				
			testStatus = testStep[stepNumber].Status;
			break;
		case 11:		// 11 SPECTROMETER TEST
			testStatus = PASS;
			break;
		case 12:		// 12 TEST COMPLETE
			resetTestData();
			resetDisplays(ptrDialog);
			testStatus = PASS;
			InitializeInterfaceBoard(ptrDialog);
			break;
		case 13:		// 13 UNIT FAILED
			resetTestData();
			resetDisplays(ptrDialog);
			testStatus = FAIL;
			InitializeInterfaceBoard(ptrDialog);
		default:
			break;
		}
		testStep[stepNumber].Status = testStatus;
		return testStatus;
	}


	BOOL TestApp::SetInterfaceBoardActuatorOutput(CSerialCtrlDemoDlg *ptrDialog, BOOL actuatorOpen)
	{
		char strCommand[BUFFERSIZE] = "$TTL_HIGH";	
		char strInputCommand[BUFFERSIZE] = "$TTL_IN";
		char strResponse[BUFFERSIZE];

		SetInterfaceBoardPWM(ptrDialog, MAX_PWM);
		msDelay(100);

		if (!actuatorOpen) strcpy_s(strCommand, BUFFERSIZE, "$TTL_LOW");

		// Send SET PWM command to interface board:
		if (!sendReceiveSerial(INTERFACE_BOARD, ptrDialog, strCommand, strResponse, TRUE)) {
			DisplayMessageBox("INTERFACE BOARD COM ERROR", "Make sure Interface board power is ON, \r\nCheck RS232 and USB cables", 1);
			return FALSE;
		}
		else if (!strstr(strResponse, "OK")) {
			DisplayMessageBox("INTERFACE BOARD COMMUNICATION ERROR", "Check interface board cables and power", 1);
			return FALSE;
		}
		
		// Check TTL input for actuator fault detect:
		if (!sendReceiveSerial(INTERFACE_BOARD, ptrDialog, strInputCommand, strResponse, TRUE)) {
			DisplayMessageBox("INTERFACE BOARD COM ERROR", "Make sure Interface board power is ON, \r\nCheck RS232 and USB cables", 1);
			return FALSE;
		}
		else if (!strstr(strResponse, "OK")) {
			DisplayMessageBox("DC950 TTL INPUT ERROR", "The DC950 has failed the TTL input test", 1);
			return FALSE;
		}

		return TRUE; // SUCCESS! Both the interface board and the DC950 are responding to commands
	}

	BOOL TestApp::SetPowerSupplyVoltage(CSerialCtrlDemoDlg *ptrDialog, int commandVoltage) {
		char strCommand[BUFFERSIZE];
		char strResponse[BUFFERSIZE] = "";
		char strReadVoltage[BUFFERSIZE] = "MEAS:VOLT?\r\n";		

		return TRUE; // $$$$

    	  // Send SET VOLTAGE command:
		sprintf_s(strCommand, "VOLT %d\r\n", commandVoltage);

		if (!sendReceiveSerial(AC_POWER_SUPPLY, ptrDialog, strCommand, NULL, FALSE)) {
			DisplayMessageBox("AC POWER SUPPLY COM ERROR", "Make sure AC power supply is ON, \r\nCheck RS232 and USB cables", 1);
			return FALSE;
		}
				
		msDelay(5000);

		// Send READ VOLTAGE command and get response:
		if (!sendReceiveSerial(AC_POWER_SUPPLY, ptrDialog, strReadVoltage, strResponse, TRUE)) {
			DisplayMessageBox("AC POWER SUPPLY COM ERROR", "Make sure AC power supply is ON, \r\nCheck RS232 and USB cables", 1);
			return FALSE;
		}
		
		// Now check response from power supply. 		
		else {
			int actualVoltage = atoi(strResponse);
			int errorVoltage = abs(actualVoltage - commandVoltage);

			if (errorVoltage > MAX_SUPPLY_VOLTAGE_ERROR) {
				char strError[BUFFERSIZE];
				sprintf_s(strError, BUFFERSIZE, "AC POWER SUPPLY ERROR: voltage = %d VAC, should be %d", actualVoltage, commandVoltage);
				DisplayMessageBox(strError, "Shut off supply and consult Engineering", 1);
				return FALSE;
			}
		}
		
		// Otherwise, return TRUE to indicate power supply is communicating and functioning:
		return TRUE;
	}

	int TestApp::RunPowerSupplyTest(CSerialCtrlDemoDlg *ptrDialog)
	{
#define NUM_TEST_VOLTAGES 6
		int supplyVoltage;
		int arrSupplyVoltage[NUM_TEST_VOLTAGES] = { 90, 115, 132, 254, 230, 180 };
		float lampVoltage;
		long lampVoltageX10, errorVoltageX10;

		if (ptrDialog->subStepNumber == 0) {
			if (!InitializeInterfaceBoard(ptrDialog)) return FAIL;
			msDelay(100);
			if (!SetInterfaceBoardPWM(ptrDialog, MAX_PWM)) return FAIL;
			msDelay(100);
		}

		if (ptrDialog->subStepNumber > NUM_TEST_VOLTAGES) return FAIL;
		if (ptrDialog->subStepNumber < 0) return FAIL;

		supplyVoltage = arrSupplyVoltage[ptrDialog->subStepNumber];
		if (!SetPowerSupplyVoltage(ptrDialog, supplyVoltage)) return FAIL;

		msDelay(400);

		if (!ReadVoltage(ptrDialog, LAMP, &lampVoltage)) return FAIL;  
		lampVoltageX10 = abs((long)(lampVoltage * 10.0));
		errorVoltageX10 = abs(lampVoltageX10 - 210);     // 210 is 21.0 volts x 10

		if (errorVoltageX10 > MAX_LAMP_x10_VOLTAGE_ERROR) return FAIL;

		ptrDialog->subStepNumber++;
		if (ptrDialog->subStepNumber >= NUM_TEST_VOLTAGES) {
			InitializeInterfaceBoard(ptrDialog);
			return (PASS);
		}
		else return NOT_DONE_YET;
	}

	BOOL TestApp::InitializePowerSupply(CSerialCtrlDemoDlg *ptrDialog) {
		char strCommand[BUFFERSIZE] = "*RST\r\n";

		return TRUE; // $$$$

		// Send RESET commands to programmable power supply:
		strcpy_s(strCommand, BUFFERSIZE, "*RST\r\n");
		if (!sendReceiveSerial(AC_POWER_SUPPLY, ptrDialog, strCommand, NULL, FALSE)) {
			DisplayMessageBox("AC POWER SUPPLY COM ERROR", "Make sure AC power supply is ON, \r\nCheck RS232 and USB cables", 1);
			return FALSE;
		}		
		msDelay(500);

		strcpy_s(strCommand, BUFFERSIZE, "*CLS\r\n");
		if (!sendReceiveSerial(AC_POWER_SUPPLY, ptrDialog, strCommand, NULL, FALSE)) {
			DisplayMessageBox("AC POWER SUPPLY COM ERROR", "Make sure AC power supply is ON, \r\nCheck RS232 and USB cables", 1);
			return FALSE;
		}
		msDelay(500);

		strcpy_s(strCommand, BUFFERSIZE, "*SRE 128\r\n");
		if (!sendReceiveSerial(AC_POWER_SUPPLY, ptrDialog, strCommand, NULL, FALSE)) {
			DisplayMessageBox("AC POWER SUPPLY COM ERROR", "Make sure AC power supply is ON, \r\nCheck RS232 and USB cables", 1);
			return FALSE;
		}
		msDelay(500);

		strcpy_s(strCommand, BUFFERSIZE, "*ESE 0\r\n");
		if (!sendReceiveSerial(AC_POWER_SUPPLY, ptrDialog, strCommand, NULL, FALSE)) {
			DisplayMessageBox("AC POWER SUPPLY COM ERROR", "Make sure AC power supply is ON, \r\nCheck RS232 and USB cables", 1);
			return FALSE;
		}
		msDelay(500);

		strcpy_s(strCommand, BUFFERSIZE, "OUTP 1\r\n");
		if (!sendReceiveSerial(AC_POWER_SUPPLY, ptrDialog, strCommand, NULL, FALSE)) {
			DisplayMessageBox("AC POWER SUPPLY COM ERROR", "Make sure AC power supply is ON, \r\nCheck RS232 and USB cables", 1);
			return FALSE;
		}
				
		msDelay(500);

		strcpy_s(strCommand, BUFFERSIZE, "VOUT 0\r\n");
		sendReceiveSerial(AC_POWER_SUPPLY, ptrDialog, strCommand, NULL, FALSE);

		msDelay(500);

		// Send READ VOLTAGE command and get response:
		char strResponse[BUFFERSIZE];
		if (!sendReceiveSerial(AC_POWER_SUPPLY, ptrDialog, strCommand, strResponse, TRUE)) {
			DisplayMessageBox("AC POWER SUPPLY COM ERROR", "Make sure AC power supply is ON, \r\nCheck RS232 and USB cables", 1);
			return FALSE;
		}
		// Now check response from power supply. 
		// The voltage should be more or less zero.
		// If it reads much more than one volt, 
		// something is definitely wrong.
		// Return FALSE to indicate failure:
		else {
			int supplyVoltage = atoi(strResponse);
			if (supplyVoltage > 1) {
				char strError[BUFFERSIZE];
				sprintf_s(strError, BUFFERSIZE, "AC POWER SUPPLY ERROR: voltage = %d VAC, should be 0", supplyVoltage);
				DisplayMessageBox(strError, "Shut off supply and consult Engineering", 1);
				return FALSE;
			}
		}		

		// Return TRUE to indicate power supply is communicating and functioning:		
		return TRUE;
	}
