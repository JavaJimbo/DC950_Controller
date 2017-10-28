#ifndef _TEST_ROUTINES_H
#define _TEST_ROUTINES_H
#include "stdafx.h"
#include "SerialCtrlDemoDlg.h"
#include "Definitions.h"

class TestApp : public CDialog
{
public:				
	BOOL sendReceiveSerial(CSerialCtrlDemoDlg *ptrDialog, int targetDevice, char *outPacket, char *inPacket);	
	void msDelay(int milliseconds);	
	BOOL InitializeHP34401(CSerialCtrlDemoDlg *ptrDialog);
	BOOL openTestSerialPort();
	BOOL closeTestSerialPort();	
	BOOL DisplayMessageBox(LPCTSTR strTopLine, LPCTSTR strBottomLine, int boxType);	
	BOOL ReadSerialPort(int targetDevice, char *ptrPacket);
	BOOL WriteSerialPort(int targetDevice, char *ptrPacket);
	BOOL InitializeDisplayText();
	UINT SystemStartUp(CSerialCtrlDemoDlg *ptrDialog);
	BOOL flgMainPortOpen;
	BOOL flgHPmeterInitialized;
	BOOL flgIniFileOpen;				
	void DisplayIntructions(int stepNumber, CSerialCtrlDemoDlg *ptrDialog);
	void DisplayPassFailStatus(int stepNumber, CSerialCtrlDemoDlg *ptrDialog);
	void DisplayStatusBarText(CSerialCtrlDemoDlg *ptrDialog, int panel, LPCTSTR strText);
	void DisplaySerialComData(CSerialCtrlDemoDlg *ptrDialog, int comDirection, char *strData);
	void DisplayText(CSerialCtrlDemoDlg *ptrDialog, int lineNumber, LPCTSTR strText);	
	void DisplayStepNumber(CSerialCtrlDemoDlg *ptrDialog, int stepNumber);	
	void resetDisplays(CSerialCtrlDemoDlg *ptrDialog);
	void resetTestData();	
	int  Execute(int stepNumber, CSerialCtrlDemoDlg *ptrDialog);
	void enableBarcodeScan(CSerialCtrlDemoDlg *ptrDialog);
	void disableBarcodeScan(CSerialCtrlDemoDlg *ptrDialog);
	BOOL InitializeSystem(CSerialCtrlDemoDlg *ptrDialog);
	TestStep testStep[TOTAL_STEPS];

	TestApp(CWnd* pParent = NULL);	// standard constructor
	~TestApp();
};

#endif
