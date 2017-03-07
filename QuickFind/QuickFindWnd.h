#pragma once

#include "afxbutton.h"
#include "SearchComboBox.h"


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
	enum Flags {
		FlagsInitReplace	= 0x00000001,
		FlagsMatchCase		= 0x00000002,
		FlagsMatchWord		= 0x00000004,
		FlagsUseRegEx		= 0x00000008,
		FlagsShowScope		= 0x00000010,
	};
public:
	virtual BOOL Create(LPCTSTR lpszFindWhat, LPCTSTR lpszReplaceWith, DWORD dwFlags, CWnd* pOwnerWnd);

	BOOL OnInitDialog() override;
protected:
	BOOL InitButton(CMFCButton& btn, UINT nResID, HINSTANCE hResInst = nullptr) const;
protected:
	CSearchComboBox	m_wndFind;
	CComboBox		m_wndReplace;
	
	CMFCMenuButton	m_wndFindAction;
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
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	afx_msg void OnPaint();
	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);

	DECLARE_MESSAGE_MAP()
};
