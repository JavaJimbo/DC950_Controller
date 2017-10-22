#ifndef _TEST_ROUTINES_H
#define _TEST_ROUTINES_H
// TestRoutines.h
#include "stdafx.h"

#define PASS 1
#define FAIL -1


class TestApp : public CDialog
{
public:			
	BOOL TestApp::sendReceiveSerial(int targetDevice, char *outPacket, char *inPacket);
	void msDelay(int milliseconds);
	void ConfigureFont(int fontHeight, int fontWidth);
	BOOL InitializeHP34401();	
	BOOL openTestSerialPort();
	BOOL closeTestSerialPort();	
	BOOL DisplayMessageBox(LPCTSTR strTopLine, LPCTSTR strBottomLine, int boxType);
	// BOOL isSystemOK();	
	BOOL ReadSerialPort(int targetDevice, char *ptrPacket);
	BOOL WriteSerialPort(int targetDevice, char *ptrPacket);

	BOOL flgMainPortOpen;
	BOOL flgHPmeterInitialized;
	BOOL flgIniFileOpen;	

	TestApp(CWnd* pParent = NULL);	// standard constructor
	~TestApp();
};

class DUT
{
public:
	int hiPotTest;
	int groundBondTest;
	int potAdjustTest;
	int remoteTest;
	int ACpowerTest;
	int ActuiatorTest;
	int SpectrometerTest;

	DUT();
	// ~DUT();
};

class Test
{
public:
	char *lineOne;
	Test();
};
#endif
