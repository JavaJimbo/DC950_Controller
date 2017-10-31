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
 *  10-31-17: Converted RS232 communication to three COM ports.
 */

// NOTE: INCUDES MUST BE IN THIS ORDER!!!
#include "stdafx.h"
#include "SerialCtrlDemo.h"
#include "SerialCtrlDemoDlg.h"
#include <string.h>
#include "TestApp.h"
#include "Definitions.h"


// const char portNamemultiMeter
// const char portNamepowerSupply

extern UINT16  CRCcalculate(char *ptrPacket, BOOL addCRCtoPacket);
extern BOOL CRCcheck(char *ptrPacket);
// extern int intError;

// hPort = CreateFile("\\\\.\\COM10", GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, 0);

BOOL TestApp::openTestSerialPort(const char *ptrPortName, HANDLE *ptrPortHandle) {
		DCB serialPortConfig;
		BOOL tryAgain = FALSE;		

			do {
					*ptrPortHandle = CreateFile(ptrPortName,  // Specify port device: default "COM1"		
					GENERIC_READ | GENERIC_WRITE,       // Specify mode that open device.
					0,                                  // the devide isn't shared.
					NULL,                               // the object gets a default security.
					OPEN_EXISTING,                      // Specify which action to take on file. 
					0,                                  // default.
					NULL);                              // default.

					char errorMessage[BUFFERSIZE];
					sprintf_s(errorMessage, "ERROR: cannot open %s port", ptrPortName);

				if (GetCommState(*ptrPortHandle, &serialPortConfig) == 0) {
					tryAgain = DisplayMessageBox(errorMessage, "Check USB connections and try again.", 2);
					if (!tryAgain) return FALSE;
				}
				else break;   // SUCCESS! Port opened OK, no need to try opening it again
				msDelay(100);
			} while (tryAgain);

			DCB dcb;
			dcb.BaudRate = CBR_9600;			// $$$$ Fix baud rate at 9600, 1 stop bit, no parity, 8 data bits
			dcb.StopBits = ONESTOPBIT;
			dcb.Parity = NOPARITY;
			dcb.ByteSize = DATABITS_8;

			// Assign user parameter.
			serialPortConfig.BaudRate = dcb.BaudRate;    // Specify buad rate of communicaiton.
			serialPortConfig.StopBits = dcb.StopBits;    // Specify stopbit of communication.
			serialPortConfig.Parity = dcb.Parity;        // Specify parity of communication.
			serialPortConfig.ByteSize = dcb.ByteSize;    // Specify  byte of size of communication.

														 // Set current configuration of serial communication port.		
														 // if (SetCommState(m_NewPortHandle, &serialPortConfig) == 0)
			if (SetCommState(*ptrPortHandle, &serialPortConfig) == 0)
			{
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

			SetCommTimeouts(*ptrPortHandle, &comTimeOut);		// set the time-out parameter into device control.												
			return TRUE;  // Return TRUE to indicate port is successfully opened and configured
	}

	BOOL TestApp::closeSerialPort(HANDLE ptrPortHandle) {
		if (ptrPortHandle == NULL) return TRUE;

		if (!CloseHandle(ptrPortHandle))
		{
			AfxMessageBox("Port Closing failed.");
			return FALSE;
		}
		ptrPortHandle = NULL;
		return(TRUE);
	}

	void TestApp::closeAllSerialPorts() {
		closeSerialPort(handleInterfaceBoard);
		closeSerialPort(handleHPmultiMeter);
		closeSerialPort(handleACpowerSupply);
	}
	
	
	BOOL TestApp::WriteSerialPort (HANDLE ptrPortHandle, char *ptrPacket) {
		int length;
		int trial = 0;
		int numBytesWritten = 0;		

		if (ptrPacket == NULL || ptrPortHandle == NULL) return (FALSE);
		
		length = strlen(ptrPacket);					
		if (length >= BUFFERSIZE) {
			// intError = SYSTEM_ERROR;
			return (FALSE);
		}		

		do {
			trial++;
			if (WriteFile(ptrPortHandle, ptrPacket, length, (LPDWORD) &numBytesWritten, NULL)) break;
			msDelay(100);
		} while (trial < MAXTRIES && numBytesWritten == 0);

		if (trial >= MAXTRIES) return FALSE;
		else return TRUE;
	}
	
		
	BOOL TestApp::ReadSerialPort(HANDLE ptrPortHandle, char *ptrPacket) {
		int totalBytesRead = 0;
		DWORD numBytesRead;
		char inBytes[BUFFERSIZE];
		ptrPacket[0] = '\0';
		int trial = 0;

		if (ptrPacket == NULL) {			
			return (FALSE);
		}

		trial = 0;
		ptrPacket[0] = '\0';
		do {
			trial++;
			msDelay(100);
			if (ReadFile(ptrPortHandle, inBytes, BUFFERSIZE, &numBytesRead, NULL)) {
				if (numBytesRead > 0 && numBytesRead < BUFFERSIZE) {
					inBytes[numBytesRead] = '\0';
					strcat_s(ptrPacket, BUFFERSIZE, inBytes);
					if (strchr(inBytes, '\r')) break;					
				}
			}
		} while (trial < MAXTRIES);

		if (trial >= MAXTRIES) return (FALSE);		
		else return (TRUE);		
	}
	
	BOOL TestApp::sendReceiveSerial(int COMdevice, CSerialCtrlDemoDlg *ptrDialog, char *outPacket, char *inPacket, BOOL expectReply) {

		HANDLE ptrPortHandle = NULL;

		switch (COMdevice) {
		case HP_METER:
			ptrPortHandle = handleHPmultiMeter;
			break;
		case AC_POWER_SUPPLY:
			ptrPortHandle = handleACpowerSupply;
			break;
		case INTERFACE_BOARD:
			ptrPortHandle = handleInterfaceBoard;
		default:
			break;
		}		

		if (ptrPortHandle == NULL || outPacket == NULL) {
			return (FALSE);
		}

		DisplaySerialComData(ptrDialog, DATAOUT, outPacket);
		if (COMdevice == INTERFACE_BOARD) CRCcalculate(outPacket, TRUE);

		if (outPacket == NULL) return (FALSE);

		DisplaySerialComData(ptrDialog, DATAIN, "");

		if (!WriteSerialPort(ptrPortHandle, outPacket)) {
			DisplaySerialComData(ptrDialog, DATAOUT, "COM PORT ERROR");
			return (FALSE);
		}

		if (!expectReply) return TRUE;

		if (inPacket == NULL) {
			char temp[BUFFERSIZE];
			inPacket = temp;
		}

		if (!ReadSerialPort(ptrPortHandle, inPacket)) {
			DisplaySerialComData(ptrDialog, DATAIN, "COM PORT ERROR");
			return (FALSE);
		}

		if (COMdevice == INTERFACE_BOARD) {
			if (!CRCcheck(inPacket)) {
				DisplaySerialComData(ptrDialog, DATAIN, "CRC ERROR");
				return (FALSE);
			}			
		}
		DisplaySerialComData(ptrDialog, DATAIN, inPacket);
		return (TRUE);		
	}


	