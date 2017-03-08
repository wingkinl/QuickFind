#pragma once

#include "afxbutton.h"
#include "SearchComboBox.h"

#define QUICKFINDMSGSTRING  _T("QuickFindReplace")

struct QUICKFIND_INFO
{
	enum Flags {
		FlagsInitReplace	= 0x00000001,
		FlagsMatchCase		= 0x00000002,
		FlagsMatchWord		= 0x00000004,
		FlagsUseRegEx		= 0x00000008,
		FlagsShowScope		= 0x00000010,
	};
	DWORD			dwFlags;
	CWnd*			pWndOwner;
	CStringArray	saSearch;
	CStringArray	saReplace;
};

// CQuickFindWnd dialog

typedef CDialog	CQuickFindWndBase;

class CQuickFindWnd : public CQuickFindWndBase
{
	DECLARE_DYNAMIC(CQuickFindWnd)

public:
	CQuickFindWnd();
	virtual ~CQuickFindWnd();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_QUICK_FIND_REPLACE };
#endif
public:
	
public:
	virtual BOOL Create(QUICKFIND_INFO* pInfo);

	BOOL OnInitDialog() override;
protected:
	BOOL InitButton(CMFCButton& btn, UINT nID, HINSTANCE hResInst = nullptr) const;

	BOOL PreTranslateMessage(MSG* pMsg) override;

	BOOL FindAccelerator(UINT uiCmd, CString& str) const;
protected:
	CSearchComboBox	m_wndFind;
	CComboBox		m_wndReplace;
	
	CMenu			m_menuFindAction;
	CMFCMenuButton	m_wndFindAction;
	int				m_nCurFindActionID;
	CMFCButton		m_wndClose;
	CMFCButton		m_wndReplaceNext;
	CMFCButton		m_wndReplaceAll;

	CMFCButton		m_wndMatchCase;
	CMFCButton		m_wndMatchWord;
	CMFCButton		m_wndRegEx;

	CComboBox		m_wndScope;

	CSize			m_szLastClientSize;

	BOOL			m_bShowReplaceUI;
	BOOL			m_bShowOptionsUI;

	int				m_nSecondRowCtrlsTop;
	int				m_nThirdRowCtrlsTop;

	CSize			m_wndMaxDlgSize;

	HACCEL			m_hAccel;
	ACCEL*			m_pAccelTable;
	int				m_nAccelSize;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	afx_msg void OnPaint();
	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	afx_msg void OnButtonFindActionMenu();
	afx_msg void OnFindNext();
	afx_msg void OnFindPrevious();
	afx_msg void OnFindAll();
	afx_msg void OnReplaceNext();
	afx_msg void OnReplaceAll();

	DECLARE_MESSAGE_MAP()
};
