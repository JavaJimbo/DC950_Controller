/* TestRoutines.cpp
 * Written in Visual C++ for DC950
 *
 * 10-12-17: Sends, receives commands via RS232
 *	1) Send RESET command:			"*RST\r\n"
 *  2) Enable RS232 remote control: ":SYST:REM\r\n"
 *  3) Send measurement command:	":MEAS?\r\n"
 *	10-15-17: Added pop up message boxes for error handling 
 */

// NOTE: INCUDES MUST BE IN THIS ORDER!!!
#include "stdafx.h"
#include "SerialCtrlDemo.h"
#include <string.h>
#include "TestRoutines.h"

#define FONTHEIGHT 26
#define FONTWIDTH 10
#define BUFFERSIZE 128

const char *testPortName = "COM8";

HANDLE m_testPortHandle = NULL;

HANDLE gDoneEvent;
HANDLE hTimer = NULL;
HANDLE hTimerQueue = NULL;
CFont font;

int arg = 123;

enum {
	STANDBY = 0,
	SEND,
	RECEIVE
};
int state = STANDBY;

	DUT::DUT() {
		groundBondTest = hiPotTest = groundBondTest = potAdjustTest = remoteTest = ACpowerTest = ActuiatorTest = SpectrometerTest = 0;

	}

	BOOL TestApp::isSystemOK() {
		if (!flgSerialPortError && flgIniFileOpen)
			return(TRUE);
		else return (FALSE);
	}

	TestApp::TestApp(CWnd* pParent) {
		flgSerialPortError = FALSE;
		flgIniFileOpen = TRUE;
		flgHPmeterInitialized = FALSE;
		flgMainPortOpen = FALSE;

		TCHAR* pFileName = _T("INIfile.txt");
		try
		{
			CStdioFile fileHandle(pFileName, CFile::modeRead | CFile::typeText);
		}
		catch (CFileException* pe)
		{
			MessageBox("Cannot open file", "FILE ERROR", MB_OK);
			flgIniFileOpen = FALSE;
			pe->Delete();
		}
	}

	TestApp::~TestApp() {
		TestApp::closeTestSerialPort();
	}


	VOID CALLBACK TimerRoutine(PVOID lpParam, BOOLEAN TimerOrWaitFired)
	{
		if (lpParam == NULL)
		{
			//TRACE("\nTimerRoutine lpParam is NULL\n");
		}
		else
		{
			// lpParam points to the argument; in this case it is an int
			// TRACE("\nTimer routine called.");

			if (TimerOrWaitFired)
			{
				; // TRACE("\nThe wait timed out.\n");
			}
			else
			{
				; // TRACE("\nThe wait event was signaled.\n");
			}
		}

		SetEvent(gDoneEvent);
	}



	void TestApp::msDelay(int milliseconds) {
		// Use an event object to track the TimerRoutine execution
		gDoneEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
		if (NULL == gDoneEvent)
		{
			//TRACE("\nCreateEvent failed \n");
		}

		// Create the timer queue.
		hTimerQueue = CreateTimerQueue();
		if (NULL == hTimerQueue)
		{
			//TRACE("\nCreateTimerQueue failed \n");
		}

		// Set a timer to call the timer routine in milliseconds
		if (!CreateTimerQueueTimer(&hTimer, hTimerQueue, (WAITORTIMERCALLBACK)TimerRoutine, &arg, milliseconds, 0, 0))
		{
			//TRACE("\nCreateTimerQueueTimer failed \n");
		}

		// TODO: Do other useful work here 
		// TRACE("\nCall timer routine in milliseconds...\n");

		// Wait for the timer-queue thread to complete using an event 
		// object. The thread will signal the event at that time.
		if (WaitForSingleObject(gDoneEvent, INFINITE) != WAIT_OBJECT_0);
		//TRACE("\nWaitForSingleObject failed \n");

		CloseHandle(gDoneEvent);

		// Delete all timers in the timer queue.
		if (!DeleteTimerQueue(hTimerQueue));
		//TRACE("\nDeleteTimerQueue failed \n");

	}

	void TestApp::ConfigureFont(int fontHeight, int fontWidth) {
		font.CreateFont(
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


	BOOL TestApp::openTestSerialPort() {
		DCB m_testPortConfig;
		if (!flgMainPortOpen) {

			m_testPortHandle = CreateFile(testPortName,  // Specify port device: default "COM1"		
				GENERIC_READ | GENERIC_WRITE,       // Specify mode that open device.
				0,                                  // the devide isn't shared.
				NULL,                               // the object gets a default security.
				OPEN_EXISTING,                      // Specify which action to take on file. 
				0,                                  // default.
				NULL);                              // default.

													// Get current configuration of serial communication port.

													//if (GetCommState(m_NewPortHandle, &m_testPortConfig) == 0)
			if (GetCommState(m_testPortHandle, &m_testPortConfig) == 0)
			{

				AfxMessageBox("Get configuration port has problem.");
				return FALSE;
			}

			DCB dcb;
			dcb.BaudRate = CBR_9600;			// $$$$ Fix baud rate at 9600, 1 stop bit, no parity, 8 data bits
			dcb.StopBits = ONESTOPBIT;
			dcb.Parity = NOPARITY;
			dcb.ByteSize = DATABITS_8;

			// Assign user parameter.
			m_testPortConfig.BaudRate = dcb.BaudRate;    // Specify buad rate of communicaiton.
			m_testPortConfig.StopBits = dcb.StopBits;    // Specify stopbit of communication.
			m_testPortConfig.Parity = dcb.Parity;        // Specify parity of communication.
			m_testPortConfig.ByteSize = dcb.ByteSize;    // Specify  byte of size of communication.

														 // Set current configuration of serial communication port.		
														 // if (SetCommState(m_NewPortHandle, &m_testPortConfig) == 0)
			if (SetCommState(m_testPortHandle, &m_testPortConfig) == 0)
			{
				AfxMessageBox("Set configuration port has problem.");
				return FALSE;
			}

			// instance an object of COMMTIMEOUTS.
			COMMTIMEOUTS comTimeOut;
			comTimeOut.ReadIntervalTimeout = 3;
			comTimeOut.ReadTotalTimeoutMultiplier = 3;
			comTimeOut.ReadTotalTimeoutConstant = 2;
			comTimeOut.WriteTotalTimeoutMultiplier = 3;
			comTimeOut.WriteTotalTimeoutConstant = 2;

			SetCommTimeouts(m_testPortHandle, &comTimeOut);		// set the time-out parameter into device control.

															//m_NewPortStatus = TRUE;
			flgMainPortOpen = TRUE;
			return TRUE;
		}
		return TRUE;  // Return true if port is already open
	}

	BOOL TestApp::closeTestSerialPort() {
		if (flgMainPortOpen)               // Port need to be open before.
		{
			flgMainPortOpen = FALSE;                 // Update status
			if (!CloseHandle(m_testPortHandle))  // Call this function to close port.
			{
				AfxMessageBox("Port Closing failed.");
				return FALSE;
			}
		}
		return(TRUE);
	}

#define MAXTRIES 5
	BOOL TestApp::sendReceiveSerial(CStatic *ptrInfo, BOOL getReply) {
		char outputData[BUFFERSIZE] = "\rRead meter voltage\r";
		const char strSendMeasureCommand[] = ":MEAS?\r\n";
		char inBytes[BUFFERSIZE];
		int length;
		DWORD numBytesWritten = 0;
		DWORD numBytesRead = 0;
		static BOOL startFlag = TRUE;
		static int timeSeconds = 0;
		static int trial = 0;
		char inPacket[BUFFERSIZE] = { '\0' };

		length = strlen(strSendMeasureCommand);
		
		if (!openTestSerialPort()) return (FALSE);
		if (!InitializeHP34401()) return (FALSE);		

		// SEND measure command		
		if (!WriteFile(m_testPortHandle, strSendMeasureCommand, length, &numBytesWritten, NULL)) {
			TRACE("\nERROR: PORT WRITE ERROR");
			state = STANDBY;
			closeTestSerialPort();
			return(FALSE);
		}

		if (!getReply) return(TRUE);

		// Wait for response:
		trial = 0;
		inPacket[0] = '\0';
		do {
			trial++;
			msDelay(100);
			if (ReadFile(m_testPortHandle, inBytes, BUFFERSIZE, &numBytesRead, NULL)) {
				if (numBytesRead > 0 && numBytesRead < BUFFERSIZE) {
					inBytes[numBytesRead] = '\0';
					//sprintf_s(strTrial, "\r%d bytes received: %s", numBytesRead, inBytes);
					//RACE(strTrial);
					strcat_s(inPacket, inBytes);
					if (strchr(inBytes, '\r')) {
						ptrInfo->SetWindowText(inPacket);
						break;
					}
				}
			}
		} while (trial < MAXTRIES);

		if (trial >= MAXTRIES) {
			state = STANDBY;
			closeTestSerialPort();
			flgHPmeterInitialized = FALSE;
			// ptrInfo->SetWindowText("TIMEOUT");
			return (FALSE);
		}
		else return(TRUE);
	}


	BOOL TestApp::InitializeHP34401() {
		int length;
		DWORD numBytesWritten = 0;
		const char strReset[] = "*RST\r\n";
		const char strEnableRemote[] = ":SYST:REM\r\n";

		if (flgHPmeterInitialized) return (TRUE);
			
		// 1) Send RESET command to HP34401:
		length = strlen(strReset);
		if (!WriteFile(m_testPortHandle, strReset, length, &numBytesWritten, NULL)) {
			TRACE("\nERROR: PORT WRITE ERROR");
			state = STANDBY;
			closeTestSerialPort();
			return(FALSE);
		}

		msDelay(1000);

		// 2) Enable RS232 remote control : ":SYST:REM\r\n"
		length = strlen(strEnableRemote);
		if (!WriteFile(m_testPortHandle, strEnableRemote, length, &numBytesWritten, NULL)) {
			TRACE("\nERROR: PORT WRITE ERROR");
			state = STANDBY;
			closeTestSerialPort();
			return(FALSE);
		}
		flgHPmeterInitialized = TRUE;

		return(TRUE);
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

	#define MAXRETRIES 5

	/*
	BOOL WriteSerialPort (char *ptrPacket) {
		int length;
		int retries = 0;
		DWORD numBytesWritten = 0;		

		if (ptrPacket == NULL) return (FALSE);
		length = strlen(ptrPacket);
		if (length < MAXBUFFER) {
			do {
				if (WriteFile(m_testPortHandle, ptrPacket, length, &numBytesWritten, NULL)) break;
				msDelay(1000);
			} while (++retries < MAXRETRIES && numBytesWritten == 0);
		}
		else return (FALSE);

		return (TRUE);
	}

	BOOL ReadSerialPort(char *ptrPacket) {
		int length;
		int retries = 0;
		DWORD numBytesWritten = 0;

		if (ptrPacket == NULL) return (FALSE);
		length = strlen(ptrPacket);
		if (length < MAXBUFFER) {
			do {
				if (WriteFile(m_testPortHandle, ptrPacket, length, &numBytesWritten, NULL)) break;
				msDelay(1000);
			} while (++retries < MAXRETRIES && numBytesWritten == 0);
		}
		else return (FALSE);

		if (retries < MAXRETRIES) return (TRUE);
		else return (FALSE);
	}

	BOOL ReadSerialPort(char *ptrPacket) {
		int totalBytesRead = 0, numBytesRead = 0;
		int retries = 0;

		do {
			if (ReadFile(m_testPortHandle, inBytes, BUFFERSIZE, &numBytesRead, NULL)) {
				if (numBytesRead > 0 && numBytesRead < BUFFERSIZE) {
					inBytes[numBytesRead] = '\0';
					strcat_s(inPacket, inBytes);
					if (strchr(inBytes, '\r')) {
						ptrInfo->SetWindowText(inPacket);
						break;
					}
				}
			}
		} while (++retries < MAXRETRIES);

		if (retries < MAXRETRIES) return (TRUE);
		else return (FALSE);
	}
	*/