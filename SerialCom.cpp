/* SerialCom.cpp
 * Written in Visual C++ for DC950
 *
 * 10-12-17: Sends, receives commands via RS232
 *	1) Send RESET command:			"*RST\r\n"
 *  2) Enable RS232 remote control: ":SYST:REM\r\n"
 *  3) Send measurement command:	":MEAS?\r\n"
 *	10-15-17: Added pop up message boxes for error handling 
 *	10-20-71: Created ReadSerialPort() and WriteSerialPort() and simpified sendReceiveSerial()
 *  10-21-17: Added CRC
 *	10-22-17: Cleaned up error handling, use intError. Works well with interface board->meter
 */

// NOTE: INCUDES MUST BE IN THIS ORDER!!!
#include "stdafx.h"
#include "SerialCtrlDemo.h"
#include "SerialCtrlDemoDlg.h"
#include <string.h>
#include "TestApp.h"
#include "Definitions.h"

const char *testPortName = "COM5";
HANDLE m_testPortHandle = NULL;

extern UINT16  CRCcalculate(char *ptrPacket, BOOL addCRCtoPacket);
extern BOOL CRCcheck(char *ptrPacket);
// extern int intError;
	
BOOL TestApp::openTestSerialPort() {
		DCB m_testPortConfig;
		if (!flgMainPortOpen){							// Get current configuration of serial communication port.
														// if (GetCommState(m_NewPortHandle, &m_testPortConfig) == 0)			 
			int tryAgain;
			do {
				m_testPortHandle = CreateFile(testPortName,  // Specify port device: default "COM1"		
					GENERIC_READ | GENERIC_WRITE,       // Specify mode that open device.
					0,                                  // the devide isn't shared.
					NULL,                               // the object gets a default security.
					OPEN_EXISTING,                      // Specify which action to take on file. 
					0,                                  // default.
					NULL);                              // default.

				if (GetCommState(m_testPortHandle, &m_testPortConfig) == 0) {
					flgMainPortOpen = FALSE;
					tryAgain = DisplayMessageBox("Serial Port Error", "Check USB connections and try again.", 1);
					closeTestSerialPort();
				}
				else flgMainPortOpen = TRUE;
				msDelay(100);
			} while (!flgMainPortOpen && tryAgain);

			if (!flgMainPortOpen) {
				// intError = PORT_ERROR;
				return (FALSE);
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
				flgMainPortOpen = FALSE;
				// intError = PORT_ERROR;
				AfxMessageBox("PROGRAM ERROR: Set configuration port has problem.");
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
			return TRUE;
		}
		return TRUE;  // Return true if port is already open
	}

	BOOL TestApp::closeTestSerialPort() {
		if (flgMainPortOpen)               // Port need to be open before.
		{
			flgMainPortOpen = FALSE;              // Update status
			flgHPmeterInitialized = FALSE;
			if (!CloseHandle(m_testPortHandle))   // Call this function to close port.
			{
				AfxMessageBox("Port Closing failed.");
				return FALSE;
			}
		}
		return(TRUE);
	}
	
	
	BOOL TestApp::WriteSerialPort (int targetDevice, char *ptrPacket){
		int length;
		int trial = 0;
		int numBytesWritten = 0;		

		if (ptrPacket == NULL) {
			// intError = SYSTEM_ERROR;
			return (FALSE);
		}

		length = strlen(ptrPacket);					
		if (length >= BUFFERSIZE) {
			// intError = SYSTEM_ERROR;
			return (FALSE);
		}		

		do {
			trial++;
			if (WriteFile(m_testPortHandle, ptrPacket, length, (LPDWORD) &numBytesWritten, NULL)) break;
			msDelay(100);
		} while (trial < MAXTRIES && numBytesWritten == 0);

		if (trial >= MAXTRIES) {
			// intError = TIMEOUT_ERROR;
			if (targetDevice == MULTIMETER)	flgHPmeterInitialized = FALSE;
			return (FALSE);
		}
		return (TRUE);		
	}
	
		
	BOOL TestApp::ReadSerialPort(int targetDevice, char *ptrPacket) {
		int totalBytesRead = 0;
		DWORD numBytesRead;
		char inBytes[BUFFERSIZE];
		ptrPacket[0] = '\0';
		int trial = 0;

		if (ptrPacket == NULL) {			
			// intError = SYSTEM_ERROR;
			return (FALSE);
		}

		// Wait for response:
		trial = 0;
		ptrPacket[0] = '\0';
		do {
			trial++;
			msDelay(100);
			if (ReadFile(m_testPortHandle, inBytes, BUFFERSIZE, &numBytesRead, NULL)) {
				if (numBytesRead > 0 && numBytesRead < BUFFERSIZE) {
					inBytes[numBytesRead] = '\0';
					strcat_s(ptrPacket, BUFFERSIZE, inBytes);
					if (strchr(inBytes, '\r')) break;					
				}
			}
		} while (trial < MAXTRIES);

		if (trial >= MAXTRIES) {
			// intError = TIMEOUT_ERROR;
			return (FALSE);
		}
		return (TRUE);		
	}
	
	// All serial communication goes through a single USB serial port to an interface board.	
	BOOL TestApp::sendReceiveSerial(CSerialCtrlDemoDlg *ptrDialog, int targetDevice, char *outPacket, char *inPacket) {
		if (outPacket == NULL) {
			// intError = SYSTEM_ERROR;
			return (FALSE);
		}

		if (inPacket == NULL) return (TRUE);
		inPacket[0] = '\0';

		if (!openTestSerialPort()) return (FALSE);

		DisplaySerialComData(ptrDialog, DATAOUT, outPacket);
		CRCcalculate(outPacket, TRUE);
		if (outPacket == NULL) return (FALSE);
		
		DisplaySerialComData(ptrDialog, DATAIN, "");

		if (!WriteSerialPort(0, outPacket)) {
			DisplaySerialComData(ptrDialog, DATAOUT, "COM PORT ERROR");
			return (FALSE);
		}

		if (!ReadSerialPort(0, inPacket)) {
			DisplaySerialComData(ptrDialog, DATAIN, "COM PORT ERROR");
			return (FALSE);
		}

		if (!CRCcheck(inPacket)) {
			DisplaySerialComData(ptrDialog, DATAIN, "CRC ERROR");
			return (FALSE);
		}
		else DisplaySerialComData(ptrDialog, DATAIN, inPacket);

		return (TRUE);		
	}


	