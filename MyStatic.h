#if !defined(AFX_MYSTATIC_H__23523D6F_4906_11D3_B793_0080C8FC4417__INCLUDED_)
#define AFX_MYSTATIC_H__23523D6F_4906_11D3_B793_0080C8FC4417__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MyStatic.h : header file
//
#include "Timer.h"
/////////////////////////////////////////////////////////////////////////////
// CMyStatic window

class CMyStatic : public CStatic, public Timer
{
// Construction
public:
	CMyStatic();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMyStatic)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMyStatic();

	// Generated message map functions
protected:
	virtual void Tick(CStatic *ptrInfo);
	//{{AFX_MSG(CMyStatic)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MYSTATIC_H__23523D6F_4906_11D3_B793_0080C8FC4417__INCLUDED_)
