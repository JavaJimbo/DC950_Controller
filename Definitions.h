#pragma once
// #include "Definitions.h"

#define MULTIMETER 1
#define BUFFERSIZE 128
#define FONTHEIGHT 26
#define FONTWIDTH 10
#define MAXTRIES 5
#define MULTIMETER 1

#define TOTAL_STEPS 14
#define FINAL_FAIL TOTAL_STEPS-1
#define FINAL_PASS (TOTAL_STEPS-2)
#define STARTUP 0
#define LEFTPANEL 0
#define RIGHTPANEL 1

#define DATAIN 0
#define DATAOUT 1

enum COM_PORTS {
	INTERFACE_BOARD = 0,
	HP_METER,
	AC_POWER_SUPPLY
};

enum ERROR_CODES {
	NO_ERRORS = 0,
	PORT_ERROR,
	TIMEOUT_ERROR,
	RESPONSE_ERROR,
	CRC_ERROR,
	SYSTEM_ERROR	
};

enum TIMER_STATES {
	TIMER_PAUSED = 0,
	TIMER_RUN
};

enum STATUS {
	NOT_DONE_YET,	
	PASS,
	FAIL
};

enum STEP_TYPE {	
	NOT_USED = 0,	
	MANUAL,
	AUTO
};

#define REMOTE_TEST 6
#define BARCODE_SCAN 1

struct TestStep
{
public:
	char *lineOne = NULL;
	char *lineTwo = NULL;
	char *lineThree = NULL;
	char *lineFour = NULL;
	char *lineFive = NULL;
	char *lineSix = NULL;
	char *testName = NULL;
	UINT Status = NOT_DONE_YET;
	int  stepType = MANUAL;
	int  testID = 0;
	BOOL enableENTER = TRUE;
	BOOL enablePREVIOUS = TRUE;
	BOOL enableHALT = TRUE;
	BOOL enablePASS = TRUE;
	BOOL enableFAIL = TRUE;	
};
