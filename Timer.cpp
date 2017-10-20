 // Timer.cpp: implementation of the Timer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Timer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Timer::Timer()
{
	m_bStop=true;
	m_msTimeout=-1;
	m_hThreadDone = NULL;
	m_hThreadDone = CreateEvent(NULL,FALSE, FALSE, NULL);
	m_ptrInfo = NULL;
	ASSERT (m_hThreadDone);
	SetEvent(m_hThreadDone);

}

Timer::~Timer()
{
	//dont destruct until the thread is done
	DWORD ret=WaitForSingleObject(m_hThreadDone,INFINITE);
	ASSERT(ret==WAIT_OBJECT_0);
	Sleep(500);
}

void Timer::Tick(CStatic *ptrInfo)
{
	//Will be overriden by subclass

}

void Timer::StartTicking(CStatic *ptrDisplayInfo)
{	
	if (m_bStop==false)
		return; ///ignore, it is already ticking...	
	if (m_ptrInfo == NULL) m_ptrInfo = ptrDisplayInfo;
	m_bStop=false;
	ResetEvent(m_hThreadDone);
	AfxBeginThread(TickerThread, this);	
}

UINT Timer::TickerThread(LPVOID pParam)
{	
	Timer* me=(Timer*) pParam;
	ASSERT (me->m_msTimeout!=-1);
	while (!me->m_bStop)
	{
		Sleep (me->GetTimeout());
		me->Tick(me->m_ptrInfo);
	}
	SetEvent(me->m_hThreadDone);
	return 0;
}

void Timer::StopTicking()
{
	if (m_bStop==true)
		return; ///ignore, it is not ticking...

	m_bStop=true; //ok make it stop
	// WaitForSingleObject(m_hThreadDone,INFINITE); 
	//The above wait ensures that we do not return UNTIL the thread
	//has finished. This way we dont allow the user to start multiple
	//threads that will execute Tick() at the same time	
}
