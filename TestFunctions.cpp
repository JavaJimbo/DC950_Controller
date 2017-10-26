
/* TestFunctions.cpp
 * Written in Visual C++ for DC950
 *
 */

// NOTE: INCUDES MUST BE IN THIS ORDER!!!
#include "stdafx.h"
#include "SerialCtrlDemo.h"
#include "SerialCtrlDemoDlg.h"
#include <string.h>
#include "TestApp.h"
#include "Definitions.h"



int intError = 0;
extern int stepNumber;

HANDLE gDoneEvent;
HANDLE hTimer = NULL;
HANDLE hTimerQueue = NULL;


void TestApp::resetTestStatus() {
	for (int i = 0; i < NUMBER_OF_STEPS; i++) {
		if (i == 4 || i == 6) testStep[i].status = NO_TEST;
		else testStep[i].status = NOT_DONE;
	}
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
			intError = SYSTEM_ERROR;
		}		
	}

	BOOL TestApp::InitializeDisplayText() {	
		int i = 0;
		// 0		
		testStep[i].lineOne = "Prepare DC950 for testing:";					
		testStep[i].lineTwo = "Scan barcode or type serial number";				
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
		
		i++;
		// 1
		testStep[i].lineOne = "Hi-Pot Test:";		
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
		i++;
		// 2
		testStep[i].lineOne = "Ground Bond Test:";		
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
		i++;

		// 3
		testStep[i].lineOne = "Pot Test LOW:";
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
		i++;

		// 4
		testStep[i].lineOne = "Pot Test LOW:";
		testStep[i].lineTwo = "";
		testStep[i].lineThree = "";
		testStep[i].lineFour = "";
		testStep[i].lineFive = "Press ENTER for next test";
		testStep[i].lineSix = "";
		testStep[i].testType = MANUAL;
		testStep[i].enableENTER = TRUE;
		testStep[i].enablePASS = FALSE;
		testStep[i].enableFAIL = FALSE;
		testStep[i].enableHALT = FALSE;
		testStep[i].enablePREVIOUS = TRUE;
		i++;

		// 5
		testStep[i].lineOne = "Pot Test HIGH:";
		testStep[i].lineTwo = "Turn POT to MAX power";
		testStep[i].lineThree = "Press ENTER to run test";
		testStep[i].lineFour = "";
		testStep[i].lineFive = "";
		testStep[i].lineSix = "";
		testStep[i].testType = AUTO;
		testStep[i].enableENTER = TRUE;
		testStep[i].enablePASS = FALSE;
		testStep[i].enableFAIL = FALSE;
		testStep[i].enableHALT = FALSE;
		testStep[i].enablePREVIOUS = TRUE;
		i++;

		// 6
		testStep[i].lineOne = "Pot Test HIGH:";
		testStep[i].lineTwo = "Turn POT to MAX power";
		testStep[i].lineThree = "Press ENTER for next test";
		testStep[i].lineFour = "";
		testStep[i].lineFive = "";
		testStep[i].lineSix = "";
		testStep[i].testType = AUTO;
		testStep[i].enableENTER = TRUE;
		testStep[i].enablePASS = FALSE;
		testStep[i].enableFAIL = FALSE;
		testStep[i].enableHALT = FALSE;
		testStep[i].enablePREVIOUS = TRUE;
		i++;

		// 7
		testStep[i].lineOne = "Remote Test:";
		testStep[i].lineTwo = "Turn pot back to zero";
		testStep[i].lineThree = "Set switch on back to REMOTE";
		testStep[i].lineFour = "Plug in DB9 connectors on back of DC950";
		testStep[i].lineFive = "Press ENTER when ready";
		testStep[i].lineSix = "";
		testStep[i].testType = AUTO;
		testStep[i].enableENTER = TRUE;
		testStep[i].enablePASS = FALSE;
		testStep[i].enableFAIL = FALSE;
		testStep[i].enableHALT = FALSE;
		testStep[i].enablePREVIOUS = TRUE;
		i++;

		// 8
		testStep[i].lineOne = "AC Power Sweep:";
		testStep[i].lineTwo = "Turn pot back to zero";
		testStep[i].lineThree = "Set switch on back to REMOTE";
		testStep[i].lineFour = "";
		testStep[i].lineFive = "Press ENTER when ready";
		testStep[i].lineSix = "";
		testStep[i].testType = AUTO;
		testStep[i].enableENTER = FALSE;
		testStep[i].enablePASS = FALSE;
		testStep[i].enableFAIL = FALSE;
		testStep[i].enableHALT = TRUE;
		testStep[i].enablePREVIOUS = FALSE;
		i++;

		// 9
		testStep[i].lineOne = "Actuator Test:";	
		testStep[i].lineTwo = "Running test - please wait";
		testStep[i].lineThree = "";
		testStep[i].lineFour = "";
		testStep[i].lineFive = "";
		testStep[i].lineSix = "";
		testStep[i].testType = AUTO;
		testStep[i].enableENTER = FALSE;
		testStep[i].enablePASS = FALSE;
		testStep[i].enableFAIL = FALSE;
		testStep[i].enableHALT = TRUE;
		testStep[i].enablePREVIOUS = FALSE;
		i++;

		// 10
		testStep[i].lineOne = "Spectrometer Test:";
		testStep[i].lineTwo = "Running test - please wait";
		testStep[i].lineThree = "";
		testStep[i].lineFour = "";
		testStep[i].lineFive = "";
		testStep[i].lineSix = "";
		testStep[i].testType = AUTO;
		testStep[i].enableENTER = FALSE;
		testStep[i].enablePASS = FALSE;
		testStep[i].enableFAIL = FALSE;
		testStep[i].enableHALT = TRUE;
		testStep[i].enablePREVIOUS = FALSE;
		i++;

		// 11
		testStep[i].lineOne = "Test Complete:";
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
		return TRUE;
	}

	void TestApp::DisplayPassFailStatus(int stepNumber, CSerialCtrlDemoDlg *ptrDialog) {
		if (testStep[stepNumber].status == PASS) 
			ptrDialog->m_static_passfail_status.SetWindowText("PASS");
		else if (testStep[stepNumber].status == FAIL) 
			ptrDialog->m_static_passfail_status.SetWindowText("FAIL");
		else ptrDialog->m_static_passfail_status.SetWindowText("");
	}
		
	void TestApp::DisplayIntructions(int stepNumber, CSerialCtrlDemoDlg *ptrDialog){	
		ptrDialog->m_static_ButtonRun.SetWindowText("ENTER");

		char *ptrText = NULL;


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

		ptrText = testStep[stepNumber].lineSix;
		if (ptrText != NULL) ptrDialog->m_static_Line6.SetWindowText(ptrText);

		ptrDialog->m_static_ButtonPass.EnableWindow(testStep[stepNumber].enablePASS);
		ptrDialog->m_static_ButtonFail.EnableWindow(testStep[stepNumber].enableFAIL);
		ptrDialog->m_static_ButtonRun.EnableWindow(testStep[stepNumber].enableENTER);
		ptrDialog->m_static_ButtonPrevious.EnableWindow(testStep[stepNumber].enablePREVIOUS);
		ptrDialog->m_static_ButtonHalt.EnableWindow(testStep[stepNumber].enableHALT);
	}
	
	int TestApp::Execute(int stepNumber, CSerialCtrlDemoDlg *ptrDialog){		
		switch(stepNumber){
		case 0:
			break;
		case 1:			
			break;
		case 2:			
			break;
		case 3:
			break;
		case 4:
			break;
		case 5:
			break;
		case 6:
			break;
		case 7:
			break;
		case 8:
			break;
		case 9:
			break;
		case 10:
			break;
		case 11:
			break;
		case 12:
			break;
		default:
			break;
		}		
		return intError;
	}

	/*
	BOOL TestApp::GetTimerState(int stepNumber) {
		return(testStep[stepNumber].runTimer);
	}
	*/

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
	 
	

	BOOL TestApp::InitializeHP34401(CSerialCtrlDemoDlg *ptrDialog) {
		char strReset[BUFFERSIZE] = "$RESET";
		char strEnableRemote[BUFFERSIZE] = "$REMOTE";
		char strResponse[BUFFERSIZE];
				
		if (flgHPmeterInitialized) return (TRUE);

		// 1) Send RESET command to HP34401:
		if (!sendReceiveSerial(1, strReset, strResponse)) return (FALSE);
		ptrDialog->m_static_Line6.SetWindowText((LPCTSTR)strResponse);

		msDelay(500);

		// 2) Enable RS232 remote control : ":SYST:REM\r\n"
		if (!sendReceiveSerial(1, strEnableRemote, strResponse)) return (FALSE);
		ptrDialog->m_static_Line6.SetWindowText((LPCTSTR)strResponse);

		flgHPmeterInitialized = TRUE;
		return(TRUE);
	}
