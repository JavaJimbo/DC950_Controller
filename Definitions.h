#pragma once
// #include "Definitions.h"
//#define SUCCESS	1
//#define SERIAL_PORT_ERROR 0
#define MULTIMETER 1
#define BUFFERSIZE 128
#define FONTHEIGHT 26
#define FONTWIDTH 10
#define MAXTRIES 5
#define MULTIMETER 1

enum ERROR_CODES {
	NO_ERRORS = 0,
	PORT_ERROR,
	TIMEOUT_ERROR,
	RESPONSE_ERROR,
	CRC_ERROR,
	SYSTEM_ERROR
};

