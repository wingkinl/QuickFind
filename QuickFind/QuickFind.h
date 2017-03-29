
// QuickFind.h : main header file for the QuickFind application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols
#include "QuickFindInfo.h"

class CQuickFindWnd;

class _QUICKFIND_STATE
{
public:
	_QUICKFIND_STATE();

	CQuickFindWnd*	pQuickFindWnd;
	QUICKFIND_INFO	info;
};

extern _QUICKFIND_STATE _quickfindState;

// CQuickFindApp:
// See QuickFind.cpp for the implementation of this class
//

class CQuickFindApp : public CWinAppEx
{
public:
	CQuickFindApp();


// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation
	UINT  m_nAppLook;
	BOOL  m_bHiColorIcons;
#ifdef _ENABLE_SCINTILLA_BUILD
	HINSTANCE m_hSciDLL;
#endif // _ENABLE_SCINTILLA_BUILD

	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();

	afx_msg void OnFileNew();
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CQuickFindApp theApp;
