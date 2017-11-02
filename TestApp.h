#ifndef _TEST_ROUTINES_H
#define _TEST_ROUTINES_H
#include "stdafx.h"
#include "SerialCtrlDemoDlg.h"
#include "Definitions.h"

class TestApp : public CDialog
{
public:					
	BOOL TestApp::sendReceiveSerial (int COMdevice, CSerialCtrlDemoDlg *ptrDialog, char *outPacket, char *inPacket, BOOL useCRC);
	void msDelay (int milliseconds);	
	BOOL InitializeHP34401(CSerialCtrlDemoDlg *ptrDialog);	
	BOOL InitializeInterfaceBoard(CSerialCtrlDemoDlg *ptrDialog);
	BOOL InitializePowerSupply(CSerialCtrlDemoDlg *ptrDialog);
	BOOL openTestSerialPort(const char *ptrPortName, HANDLE *ptrPortHandle);
	BOOL closeSerialPort(HANDLE ptrPortHandle);
	void closeAllSerialPorts();
	BOOL DisplayMessageBox(LPCTSTR strTopLine, LPCTSTR strBottomLine, int boxType);	
	BOOL ReadSerialPort(HANDLE ptrPortHandle, char *ptrPacket);
	BOOL WriteSerialPort(HANDLE ptrPortHandle, char *ptrPacket);
	BOOL InitializeDisplayText();
	UINT SystemStartUp(CSerialCtrlDemoDlg *ptrDialog);
	void DisplayIntructions(int stepNumber, CSerialCtrlDemoDlg *ptrDialog);
	// void DisplayPassFailStatus(int stepNumber, CSerialCtrlDemoDlg *ptrDialog);
	void DisplayPassFailStatus(int stepNumber, int subStepNumber, CSerialCtrlDemoDlg *ptrDialog);
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
	BOOL SetPowerSupplyVoltage(CSerialCtrlDemoDlg *ptrDialog, int voltage);	
	BOOL SetInterfaceBoardMulitplexer(CSerialCtrlDemoDlg *ptrDialog, int multiplexerSelect);
	BOOL ReadVoltage(CSerialCtrlDemoDlg *ptrDialog, int multiplexerSelect, float *ptrLampVoltage);
	BOOL SetInterfaceBoardPWM(CSerialCtrlDemoDlg *ptrDialog, int PWMvalue);
	int  RunRemoteTests(CSerialCtrlDemoDlg *ptrDialog);
	int  RunPowerSupplyTest(CSerialCtrlDemoDlg *ptrDialog);
	BOOL TestApp::SetInhibitRelay(CSerialCtrlDemoDlg *ptrDialog, BOOL flagON);
	BOOL TestApp::SetInterfaceBoardActuatorOutput(CSerialCtrlDemoDlg *ptrDialog, BOOL actuatorOpen);

	TestStep testStep[TOTAL_STEPS];
	CSerialCtrlDemoDlg *ptrDialog;
	
	HANDLE handleInterfaceBoard, handleHPmultiMeter, handleACpowerSupply;
	//const char *portNameInterfaceBoard = "COM8";
	//const char *portNameMultiMeter = "\\\\.\\COM17";
	//const char *portNameACpowerSupply = "\\\\.\\COM19";

	const char *portNameInterfaceBoard = "COM3";
	const char *portNameACpowerSupply = "COM4";
	const char *portNameMultiMeter = "COM5";

	TestApp(CWnd* pParent = NULL);	// standard constructor
	~TestApp();
};

#endif
