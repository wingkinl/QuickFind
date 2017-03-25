#pragma once

#include "afxbutton.h"
#include "SearchComboBox.h"
#include "QuickFindInfo.h"

enum QuickFindCmd
{
	QuickFindCmdFind			= 0,
	QuickFindCmdFindAll			= 1,
	QuickFindCmdReplace			= 2,
	QuickFindCmdReplaceAll		= 3,
	QuickFindCmdFindTextChange	= 4,
	QuickFindCmdScopeSelChange	= 5,
	QuickFindCmdOptionsChange	= 6,

	QuickFindCmdTerminating		= 10,

	QuickFindCmdCustom			= 1024,
};
#define QUICKFINDMSGSTRING  _T("QuickFindReplace")

// CQuickFindWnd dialog

typedef CDialogEx	CQuickFindWndBase;

class QUICKFIND_EXT_CLASS CQuickFindWnd : public CQuickFindWndBase
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
	struct QNMHDR
	{
		CQuickFindWnd*	pNotifier;
		WPARAM			wParam;
		LPARAM			lParam;
	};

	BOOL		m_bCanShowReplaceUI;

	virtual BOOL Create(const QUICKFIND_INFO& info, CWnd* pParentWnd);

	virtual void SetNotifyOwner(CWnd* pWndOwner);

	BOOL SetScopeItems(const CStringArray& saItems, int nActiveIndex = 0);

	BOOL OnInitDialog() override;

	CString GetFindString() const;

	CString GetReplaceString() const;

	void SetFindString(LPCTSTR pszText);

	virtual void SetActiveShowWindow();

	// called when owner window's size has been changed
	virtual void UpdateWindowPos();

	inline DWORD GetFlags() const
	{
		return m_info.dwFlags;
	}

	inline BOOL IsMatchCase() const
	{
		return m_info.IsMatchCase();
	}

	inline BOOL IsMatchWholeWord() const
	{
		return m_info.IsMatchWholeWord();
	}

	inline BOOL IsUseRegEx() const
	{
		return m_info.IsUseRegEx();
	}

	inline BOOL IsFindReplacePrevious() const
	{
		return m_info.IsFindReplacePrevious();
	}

	inline BOOL IsFindReplaceNext() const
	{
		return m_info.IsFindReplaceNext();
	}

	void GetFindStringArray(CStringArray& sa) const;

	void GetReplaceStringArray(CStringArray& sa) const;
protected:
	virtual BOOL InitButton(CMFCButton& btn, UINT nID, HINSTANCE hResInst = nullptr, int nImgSize = 16) const;
	
	BOOL InitCombo(CComboBox& combo, const CStringArray& saText);

	BOOL PreTranslateMessage(MSG* pMsg) override;

	BOOL FindAccelerator(UINT uiCmd, CString& str) const;

	void PostNcDestroy() override;

	void OnCancel() override;

	BOOL GetMoveGripperRect(CRect& rectGripper) const;
	BOOL GetSizeGripperRect(CRect& rectGripper) const;

	CSize CalcWindowSizeFromClient(CSize szClient) const;

	void SwitchFloatDock(BOOL bFloat, CWnd* pWndOwner);

	virtual void OnUpdateActiveStatus();

	void InitFindActionMenu();

	void ShowReplaceUI(BOOL bShow);
	void ShowOptionsUI(BOOL bShow);

	void SwitchUI(BOOL bShowAsReplace, BOOL bShowOptions);

	LRESULT NotifyOwner(QuickFindCmd cmd, WPARAM wp = 0, LPARAM lp = 0);
	void UpdateFindResult(BOOL bFindOK);

	void PromoteFindTextItems();
	void PromoteReplaceTextItems();

	BOOL IsShowReplaceUI() const;
	BOOL IsShowOptionsUI() const;
protected:
	CSearchComboBox	m_wndFind;
	
	CMenu			m_menuFindAction;
	CMFCMenuButton	m_wndFindAction;
	CMFCButton		m_wndClose;

	CComboBox		m_wndReplace;
	CMFCButton		m_wndReplaceNext;
	CMFCButton		m_wndReplaceAll;

	class CCheckButton : public CMFCButton
	{
	public:
		void OnDrawBorder(CDC* pDC, CRect& rectClient, UINT uiState) override
		{
			//CMFCButton::OnDrawBorder(pDC, rectClient, uiState);
			UNREFERENCED_PARAMETER(uiState);
			rectClient.DeflateRect(1, 1);
			COLORREF clrPen = (m_bPushed || m_bChecked) ? RGB(0,84,153) : m_bHighlighted ? RGB(0,120,215) : RGB(173, 173, 173);
			CPen pen(PS_SOLID, 1, clrPen);
			CBrush br(m_bChecked ? RGB(204, 228, 247) : RGB(225, 225, 225));
			auto oldBrush = pDC->SelectObject(&br);
			auto oldPen = pDC->SelectObject(&pen);
			pDC->Rectangle(rectClient);
			pDC->SelectObject(oldPen);
			pDC->SelectObject(oldBrush);
			rectClient.DeflateRect(1, 1);
		}
	};
	CCheckButton	m_wndMatchCase;
	CCheckButton	m_wndMatchWord;
	CCheckButton	m_wndRegEx;
	CComboBox		m_wndScope;

	class CDockButton : public CMFCButton
	{
	public:
		void OnDrawBorder(CDC* /*pDC*/, CRect& /*rectClient*/, UINT /*uiState*/) override {}
	};
	CDockButton		m_wndDockFloat;

	CSize			m_szLastClientSize;

	BOOL			m_bShowReplaceUI;
	BOOL			m_bShowOptionsUI;

	struct Row
	{
		LONG top;
		LONG bottom;
	};
	Row				m_arrUIRows[3];

	CSize			m_szMaxDlgSize;	// assumption: the initial window is the maximum state
	CSize			m_szMaxClientSize;

	HACCEL			m_hAccel;
	ACCEL*			m_pAccelTable;
	int				m_nAccelSize;

	QUICKFIND_INFO	m_info;

	BOOL			m_bActive;

	UINT_PTR		m_nDockTimerID;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMoving(UINT nSide, LPRECT lpRect);
	afx_msg void OnWindowPosChanging(WINDOWPOS* lpwndpos);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	afx_msg void OnButtonFindAction();
	afx_msg void OnMatchCase();
	afx_msg void OnMatchWord();
	afx_msg void OnUseRegEx();	
	afx_msg void OnSelChangeFind();
	afx_msg void OnEditChangeFind();
	afx_msg void OnSelChangeScope();
	afx_msg void OnFindNext();
	afx_msg void OnFindPrevious();
	afx_msg void OnFindAll();
	afx_msg void OnReplaceNext();
	afx_msg void OnReplaceAll();
	afx_msg void OnSwitchFloatDock();
	afx_msg void OnEditFind();
	afx_msg void OnEditReplace();
	afx_msg void OnDestroy();
	afx_msg LRESULT OnIdleUpdateCmdUI(WPARAM wParam, LPARAM);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);

	DECLARE_MESSAGE_MAP()
};
