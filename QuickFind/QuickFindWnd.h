#pragma once

#include "afxbutton.h"
#include "SearchComboBox.h"

#define QUICKFINDMSGSTRING  _T("QuickFindReplace")

class QUICKFIND_INFO
{
public:
	enum Flags {
		FlagsInitShowReplace	= 0x00000001,
		FlagsInitShowOptions	= 0x00000002,
		FlagsShowScope			= 0x00000004,
		FlagsMatchCase			= 0x00010000,
		FlagsMatchWord			= 0x00020000,
		FlagsUseRegEx			= 0x00040000,
	};
	DWORD			dwFlags;
	CWnd*			pWndOwner;
	CStringArray	saSearch;
	CStringArray	saReplace;

	QUICKFIND_INFO();
	
	QUICKFIND_INFO& operator=(const QUICKFIND_INFO& rhs);
};

// CQuickFindWnd dialog

typedef CDialogEx	CQuickFindWndBase;

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
	virtual BOOL Create(const QUICKFIND_INFO& info);

	BOOL OnInitDialog() override;

	const QUICKFIND_INFO& GetParams();
protected:
	BOOL InitButton(CMFCButton& btn, UINT nID, HINSTANCE hResInst = nullptr) const;

	BOOL PreTranslateMessage(MSG* pMsg) override;

	BOOL FindAccelerator(UINT uiCmd, CString& str) const;

	void CalcLayout(LPRECT pRect);

	BOOL IsInitShowAsReplace() const;
private:
	void ShowReplaceUI(BOOL bShow);
	void ShowOptionsUI(BOOL bShow);

	void SwitchUI(BOOL bShowAsReplace);
protected:
	CSearchComboBox	m_wndFind;	
	
	CMenu			m_menuFindAction;
	CMFCMenuButton	m_wndFindAction;
	int				m_nCurFindActionID;
	CMFCButton		m_wndClose;

	CComboBox		m_wndReplace;
	CMFCButton		m_wndReplaceNext;
	CMFCButton		m_wndReplaceAll;

	CMFCButton		m_wndMatchCase;
	CMFCButton		m_wndMatchWord;
	CMFCButton		m_wndRegEx;
	CComboBox		m_wndScope;

	CSize			m_szLastClientSize;
	int				m_nLastDlgHeight;

	BOOL			m_bShowReplaceUI;
	BOOL			m_bShowOptionsUI;

	int				m_nSecondRowCtrlsTop;
	int				m_nThirdRowCtrlsTop;

	CSize			m_szMaxDlgSize;

	HACCEL			m_hAccel;
	ACCEL*			m_pAccelTable;
	int				m_nAccelSize;

	QUICKFIND_INFO	m_info;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	afx_msg void OnPaint();
	afx_msg LRESULT OnNcHitTest(CPoint point);		
	afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	afx_msg void OnButtonFindActionMenu();
	afx_msg void OnFindNext();
	afx_msg void OnFindPrevious();
	afx_msg void OnFindAll();
	afx_msg void OnReplaceNext();
	afx_msg void OnReplaceAll();
	afx_msg void OnEditFind();
	afx_msg void OnEditReplace();

	DECLARE_MESSAGE_MAP()
};
