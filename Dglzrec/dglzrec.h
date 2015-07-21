// dglzrec.h : main header file for the DGLZREC application
//

#if !defined(AFX_DGLZREC_H__1EECE531_0301_4110_A0BE_74C9D32B646C__INCLUDED_)
#define AFX_DGLZREC_H__1EECE531_0301_4110_A0BE_74C9D32B646C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CDglzrecApp:
// See dglzrec.cpp for the implementation of this class
//

class CDglzrecApp : public CWinApp
{
public:
	CDglzrecApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDglzrecApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CDglzrecApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DGLZREC_H__1EECE531_0301_4110_A0BE_74C9D32B646C__INCLUDED_)
