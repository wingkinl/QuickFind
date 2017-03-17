#pragma once

#include "afxbutton.h"
#include "SearchComboBox.h"

enum QuickFindCmd
{
	QuickFindCmdFind			= 0,
	QuickFindCmdFindAll			= 1,
	QuickFindCmdReplace			= 2,
	QuickFindCmdReplaceAll		= 3,
	QuickFindCmdFindTextChange	= 4,
	QuickFindCmdScopeSelChange	= 5,

	QuickFindCmdTerminating		= 10,
};
#define QUICKFINDMSGSTRING  _T("QuickFindReplace")

class QUICKFIND_INFO
{
public:
	enum Flags {
		FlagsInitShowReplace		= 0x00000001,
		FlagsInitShowOptions		= 0x00000002,
		FlagsShowScope				= 0x00000004,
		FlagsMatchCase				= 0x00000100,
		FlagsMatchWord				= 0x00000200,
		FlagsUseRegEx				= 0x00000400,
		FlagsFindReplacePrevious	= 0x00000800,
		FlagsNotifyFindTextChange	= 0x00010000,
		FlagsFreeMove				= 0x01000000,
	};
	DWORD			dwFlags;
	CStringArray	saSearch;
	CStringArray	saReplace;
	UINT			nMaxItems;

	QUICKFIND_INFO()
	{
		dwFlags = FlagsInitShowOptions|FlagsNotifyFindTextChange;
		nMaxItems = 20;
	}

	QUICKFIND_INFO& operator=(const QUICKFIND_INFO& rhs)
	{
		dwFlags = rhs.dwFlags;
		saSearch.Copy(rhs.saSearch);
		saReplace.Copy(rhs.saReplace);
		if (saSearch.GetSize() > nMaxItems)
			saSearch.SetSize(nMaxItems);
		if (saReplace.GetSize() > nMaxItems)
			saReplace.SetSize(nMaxItems);
		return *this;
	}

	inline BOOL IsInitShowAsReplace() const
	{
		return (dwFlags & FlagsInitShowReplace) != 0;
	}

	inline BOOL IsInitShowOptions() const
	{
		return (dwFlags & FlagsInitShowOptions) != 0;
	}

	inline BOOL IsShowScope() const
	{
		return (dwFlags & FlagsShowScope) != 0;
	}

	inline BOOL IsMatchCase() const
	{
		return (dwFlags & FlagsMatchCase) != 0;
	}

	inline BOOL IsMatchWholeWord() const
	{
		return (dwFlags & FlagsMatchWord) != 0;
	}

	inline BOOL IsUseRegEx() const
	{
		return (dwFlags & FlagsUseRegEx) != 0;
	}

	inline BOOL IsFindReplacePrevious() const
	{
		return (dwFlags & FlagsFindReplacePrevious) != 0;
	}

	inline BOOL IsFindReplaceNext() const
	{
		return !IsFindReplacePrevious();
	}

	inline BOOL IsNotifyFindTextChange() const
	{
		return (dwFlags & FlagsNotifyFindTextChange) != 0;
	}

	inline BOOL IsFreeMove() const
	{
		return (dwFlags & FlagsFreeMove) != 0;
	}

	void PromoteStringInArray(LPCTSTR pszText, BOOL bFindStringArray = TRUE)
	{
		CStringArray& sa = bFindStringArray ? saSearch : saReplace;
		PromoteStringInArray(sa, pszText);
		if (sa.GetSize() > nMaxItems)
			sa.SetSize(nMaxItems);
	}
protected:
	static void PromoteStringInArray(CStringArray& sa, LPCTSTR pszTextToMerge)
	{
		INT_PTR ii;
		for (ii = 0; ii < sa.GetSize(); ++ii)
		{
			if (sa[ii].Compare(pszTextToMerge) == 0)
			{
				if (ii)
					sa.RemoveAt(ii);
				break;
			}
		}
		if (ii || sa.GetSize() == 0)
			sa.InsertAt(0, pszTextToMerge);
	}
};

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

	virtual BOOL Create(const QUICKFIND_INFO& info, CWnd* pParentWnd);

	void SetNotifyOwner(CWnd* pWndOwner);
	CWnd* GetNotifyOwner() const;

	BOOL SetScopeItems(const CStringArray& saItems, int nActiveIndex = 0);

	BOOL OnInitDialog() override;

	CString GetFindString() const;

	CString GetReplaceString() const;

	void SetFindString(LPCTSTR pszText);

	void SetActiveShowWindow();

	// called when owner window's size has been changed
	void UpdateWindowPos();

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
	BOOL InitButton(CMFCButton& btn, UINT nID, HINSTANCE hResInst = nullptr) const;
	
	BOOL InitCombo(CComboBox& combo, const CStringArray& saText);

	BOOL PreTranslateMessage(MSG* pMsg) override;

	BOOL FindAccelerator(UINT uiCmd, CString& str) const;

	void PostNcDestroy() override;

	void OnCancel() override;

	BOOL GetMoveGripperRect(CRect& rectGripper);
	BOOL GetSizeGripperRect(CRect& rectGripper);

	CSize CalcWindowSizeFromClient(CSize szClient) const;
private:
	void InitFindActionMenu();

	void ShowReplaceUI(BOOL bShow);
	void ShowOptionsUI(BOOL bShow);

	void SwitchUI(BOOL bShowAsReplace, BOOL bShowOptions);

	LRESULT NotifyOwner(QuickFindCmd cmd, WPARAM wp = 0, LPARAM lp = 0);

	void PromoteFindTextItems();
	void PromoteReplaceTextItems();
protected:
	CSearchComboBox	m_wndFind;
	
	CMenu			m_menuFindAction;
	CMFCMenuButton	m_wndFindAction;
	CMFCButton		m_wndClose;

	CComboBox		m_wndReplace;
	CMFCButton		m_wndReplaceNext;
	CMFCButton		m_wndReplaceAll;

	CMFCButton		m_wndMatchCase;
	CMFCButton		m_wndMatchWord;
	CMFCButton		m_wndRegEx;
	CComboBox		m_wndScope;

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
	CWnd*			m_pWndOwner;
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
	afx_msg void OnEditFind();
	afx_msg void OnEditReplace();
	afx_msg void OnNcDestroy();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
	afx_msg LRESULT OnIdleUpdateCmdUI(WPARAM wParam, LPARAM);

	DECLARE_MESSAGE_MAP()
};
