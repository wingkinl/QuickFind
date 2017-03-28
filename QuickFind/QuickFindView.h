
// QuickFindView.h : interface of the CQuickFindView class
//

#pragma once
#include "QuickFindWnd.h"

class CQuickFindWndDemo;

template <typename CQuickFindViewBase>
class CQuickFindView : public CQuickFindViewBase
{
protected: // create from serialization only
	CQuickFindView();

// Attributes
public:
	
// Operations
public:

// Overrides
public:
protected:
	virtual void OnInitialUpdate(); // called first time after construct
	
	virtual CQuickFindWnd* CreateFindReplaceWindow();

	virtual void SetText(LPCTSTR pszText) = 0;
	virtual CString GetSelectedOrCursorText() const = 0;
	virtual BOOL CanDoReplace() const = 0;
	// Implementation
public:
	virtual ~CQuickFindView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	void OnEditFindReplace(BOOL bFind);

	virtual BOOL OnFind(CQuickFindWndDemo* pQuickFindWnd);
	virtual BOOL OnFindAll(CQuickFindWndDemo* pQuickFindWnd);
	virtual BOOL OnIncrementalFind(CQuickFindWndDemo* pQuickFindWnd);

	virtual BOOL OnReplace(CQuickFindWndDemo* pQuickFindWnd);
	virtual BOOL OnReplaceAll(CQuickFindWndDemo* pQuickFindWnd);
protected:
	void CheckCloseOwnedFindWindow();
	BOOL PreTranslateMessage(MSG* pMsg) override;
// Generated message map functions
protected:
	afx_msg void OnDestroy();
	afx_msg void OnEditFind();
	afx_msg void OnEditReplace();
	afx_msg LRESULT OnQuickFindCmd(WPARAM wp, LPARAM lp);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnIdleUpdateCmdUI();
};


