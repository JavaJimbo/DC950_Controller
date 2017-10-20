// Timer.h: interface for the Timer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TIMELIMIT_H__D2107E6A_33E5_11D3_8D14_00E0980636C2__INCLUDED_)
#define AFX_TIMELIMIT_H__D2107E6A_33E5_11D3_8D14_00E0980636C2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class Timer  
{
public:
	void StopTicking();
	void StartTicking(CStatic *ptrDisplayInfo);
	Timer();
	virtual ~Timer();
	int GetTimeout(){return m_msTimeout;}
	void SetTimeout(int t){m_msTimeout=t;}	
	CStatic *m_ptrInfo;
protected:
	int m_msTimeout;
	virtual void Tick(CStatic *ptrInfo);
private:
	HANDLE m_hThreadDone;
	bool m_bStop;
	static UINT TickerThread(LPVOID pParam);
};

#endif // !defined(AFX_TIMELIMIT_H__D2107E6A_33E5_11D3_8D14_00E0980636C2__INCLUDED_)
