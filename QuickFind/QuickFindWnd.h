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

	QUICKFIND_INFO()
	{
		dwFlags = FlagsInitShowOptions|FlagsNotifyFindTextChange;
	}

	QUICKFIND_INFO& operator=(const QUICKFIND_INFO& rhs)
	{
		dwFlags = rhs.dwFlags;
		saSearch.Copy(rhs.saSearch);
		saReplace.Copy(rhs.saReplace);
		return *this;
	}

	inline BOOL IsInitShowAsReplace() const
	{
		return !!(dwFlags & FlagsInitShowReplace);
	}

	inline BOOL IsInitShowOptions() const
	{
		return !!(dwFlags & FlagsInitShowOptions);
	}

	inline BOOL IsShowScope() const
	{
		return !!(dwFlags & FlagsShowScope);
	}

	inline BOOL IsMatchCase() const
	{
		return !!(dwFlags & FlagsMatchCase);
	}

	inline BOOL IsMatchWholeWord() const
	{
		return !!(dwFlags & FlagsMatchWord);
	}

	inline BOOL IsUseRegEx() const
	{
		return !!(dwFlags & FlagsUseRegEx);
	}

	inline BOOL IsFindReplacePrevious() const
	{
		return !!(dwFlags & FlagsFindReplacePrevious);
	}

	inline BOOL IsFindReplaceNext() const
	{
		return !IsFindReplacePrevious();
	}

	inline BOOL IsNotifyFindTextChange() const
	{
		return !!(dwFlags & FlagsNotifyFindTextChange);
	}

	inline BOOL IsFreeMove() const
	{
		return !!(dwFlags & FlagsFreeMove);
	}

	void PromoteStringInArray(LPCTSTR pszText, BOOL bFindStringArray = TRUE)
	{
		PromoteStringInArray(bFindStringArray ? saSearch : saReplace, pszText);
	}
protected:
	static void PromoteStringInArray(CStringArray& sa, LPCTSTR pszTextToMerge)
	{
		for (INT_PTR ii = 0; ii < sa.GetSize(); ++ii)
		{
			if (sa[ii].Compare(pszTextToMerge) == 0)
			{
				sa.RemoveAt(ii);
				break;
			}
		}
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

	BOOL SetScopeItems(const CStringArray& saItems, int nActiveIndex = 0);

	BOOL OnInitDialog() override;

	const QUICKFIND_INFO& GetParams();

	CString GetFindString() const;

	CString GetReplaceString() const;

	void SetFindString(LPCTSTR pszText);

	void SetActiveShowWindow();

	// called when owner window's size has been changed
	void UpdateWindowPos();
protected:
	BOOL InitButton(CMFCButton& btn, UINT nID, HINSTANCE hResInst = nullptr) const;
	
	BOOL InitCombo(CComboBox& combo, const CStringArray& saText);

	BOOL PreTranslateMessage(MSG* pMsg) override;

	BOOL FindAccelerator(UINT uiCmd, CString& str) const;

	void PostNcDestroy() override;

	void OnCancel() override;

	void GetMoveGripperRect(CRect& rectGripper);
	void GetSizeGripperRect(CRect& rectGripper);
private:
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
	int				m_nLastDlgHeight;

	BOOL			m_bShowReplaceUI;
	BOOL			m_bShowOptionsUI;

	struct Row
	{
		LONG top;
		LONG bottom;
	};
	Row				m_arrUIRows[3];

	CSize			m_szMaxDlgSize;

	HACCEL			m_hAccel;
	ACCEL*			m_pAccelTable;
	int				m_nAccelSize;

	QUICKFIND_INFO	m_info;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	afx_msg void OnPaint();	
	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
	afx_msg void OnSize(UINT nType, int cx, int cy);
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

	DECLARE_MESSAGE_MAP()
};
