// MyStatic.cpp : implementation file
//

#include "stdafx.h"
#include "SerialCtrlDemo.h"
#include "SerialCtrlDemoDlg.h"
#include "MyStatic.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMyStatic

CMyStatic::CMyStatic()
{	
}

CMyStatic::~CMyStatic()
{
}


BEGIN_MESSAGE_MAP(CMyStatic, CStatic)
	//{{AFX_MSG_MAP(CMyStatic)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
	// ON_STN_CLICKED(IDC_STATIC_INFO, &CSerialCtrlDemoDlg::OnStnClickedStaticInfo)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMyStatic message handlers

#define BUFFERSIZE 128
void CMyStatic::Tick(CStatic *ptrInfo)
{
	static int j = 0;
	char strTimer[16];
	char chrLoop[BUFFERSIZE];

	if (ptrInfo != NULL) {
		sprintf_s(chrLoop, "Seconds: #%d", j++);
		ptrInfo->SetWindowText(chrLoop);
	}

	sprintf_s(strTimer, "T: #%d", j);
	TRACE(strTimer);
	// Sleep(500);		
	//sprintf_s(chrLoop, "Try: #%d", i);
	//ptrInfo->SetWindowText(chrLoop);
}
