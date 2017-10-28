#pragma once
// #include "Definitions.h"

#define MULTIMETER 1
#define BUFFERSIZE 128
#define FONTHEIGHT 26
#define FONTWIDTH 10
#define MAXTRIES 5
#define MULTIMETER 1

#define TOTAL_STEPS 13
#define FINAL_FAIL TOTAL_STEPS-1
#define FINAL_PASS (TOTAL_STEPS-2)
#define STARTUP 0
#define LEFTPANEL 0
#define RIGHTPANEL 1

#define DATAIN 0
#define DATAOUT 1

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

enum TEST_STATES {
	SUBSTEP = 0,
	NOT_DONE,	
	PASS,
	FAIL
};

enum TEST_TYPE {
	MANUAL = 0,
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
	UINT Result = NOT_DONE;
	int  testType = MANUAL;
	int  testID = 0;
	BOOL enableENTER = TRUE;
	BOOL enablePREVIOUS = TRUE;
	BOOL enableHALT = TRUE;
	BOOL enablePASS = TRUE;
	BOOL enableFAIL = TRUE;	
};
