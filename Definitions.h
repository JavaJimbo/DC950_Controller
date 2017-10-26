#pragma once
// #include "Definitions.h"

#define MULTIMETER 1
#define BUFFERSIZE 128
#define FONTHEIGHT 26
#define FONTWIDTH 10
#define MAXTRIES 5
#define MULTIMETER 1

#define NUMBER_OF_STEPS 12

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
	NO_TEST = 0,
	NOT_DONE,	
	PASS,
	FAIL
};

enum TEST_TYPE {
	MANUAL = 0,
	AUTO
};


struct TestStep
{
public:
	char *lineOne = NULL;
	char *lineTwo = NULL;
	char *lineThree = NULL;
	char *lineFour = NULL;
	char *lineFive = NULL;
	char *lineSix = NULL;
	int status = NOT_DONE;
	int testType = MANUAL;
	BOOL enableENTER = TRUE;
	BOOL enablePREVIOUS = TRUE;
	BOOL enableHALT = TRUE;
	BOOL enablePASS = TRUE;
	BOOL enableFAIL = TRUE;	
};
