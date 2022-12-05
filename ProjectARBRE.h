
// ProjectARBRE.h : main header file for the ProjectARBRE application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CProjectARBREApp:
// See ProjectARBRE.cpp for the implementation of this class
//

class CProjectARBREApp : public CWinApp
{
public:
	CProjectARBREApp() noexcept;


// Overrides
public:
	virtual BOOL InitInstance();

// Implementation
	UINT  m_nAppLook;
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CProjectARBREApp theApp;
