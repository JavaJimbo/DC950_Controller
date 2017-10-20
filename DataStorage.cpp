#include "stdafx.h"
#include "TestRoutines.h"
#include "SerialCtrlDemo.h"
#include "SerialCtrlDemoDlg.h"
#include "SerialCtrl.h"
#include <windows.h>
#include <iostream>
#include <sstream>
#include <fstream>


class DUT {
public:
	DUT *next;
	BOOL passStatus;

	DUT() { next = NULL; passStatus = TRUE; };
	~DUT() {};
};

class TestList {
	DUT *ptrHead, *ptrTail, *ptrCurrent;

	TestList();

	void addDUT() {
		ptrHead = ptrTail = ptrCurrent = NULL;
	}
};


