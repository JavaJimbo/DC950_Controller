#ifndef _TEST_ROUTINES_H
#define _TEST_ROUTINES_H
#include "stdafx.h"
#include "SerialCtrlDemoDlg.h"
#include "Definitions.h"

class TestApp : public CDialog
{
public:			
	BOOL TestApp::sendReceiveSerial(int targetDevice, char *outPacket, char *inPacket);
	void msDelay(int milliseconds);
	void ConfigureFont(int fontHeight, int fontWidth);	
	BOOL TestApp::InitializeHP34401(CSerialCtrlDemoDlg *ptrDialog);
	BOOL openTestSerialPort();
	BOOL closeTestSerialPort();	
	BOOL DisplayMessageBox(LPCTSTR strTopLine, LPCTSTR strBottomLine, int boxType);	
	BOOL ReadSerialPort(int targetDevice, char *ptrPacket);
	BOOL WriteSerialPort(int targetDevice, char *ptrPacket);
	BOOL InitializeDisplayText();
	BOOL flgMainPortOpen;
	BOOL flgHPmeterInitialized;
	BOOL flgIniFileOpen;		
	int	 Execute(int stepNumber, CSerialCtrlDemoDlg *ptrDialog);
	void DisplayIntructions(int stepNumber, CSerialCtrlDemoDlg *ptrDialog);
	void DisplayPassFailStatus(int stepNumber, CSerialCtrlDemoDlg *ptrDialog);
	void resetTestStatus();
	TestStep testStep[NUMBER_OF_STEPS];

	TestApp(CWnd* pParent = NULL);	// standard constructor
	~TestApp();
};

#endif
