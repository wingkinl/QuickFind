
// QuickFindView.h : interface of the CQuickFindView class
//

#pragma once
#include "QuickFindWnd.h"

class CQuickFindWndDemo;

typedef CRichEditView	CQuickFindViewBase;

class CQuickFindView : public CQuickFindViewBase
{
protected: // create from serialization only
	CQuickFindView();
	DECLARE_DYNCREATE(CQuickFindView)

// Attributes
public:
	
// Operations
public:

// Overrides
public:
protected:
	virtual void OnInitialUpdate(); // called first time after construct
	
	virtual CQuickFindWnd* CreateFindReplaceWindow();

	CString GetSelectedOrCursorText() const;
	BOOL CanDoReplace() const;
	// Implementation
public:
	virtual ~CQuickFindView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	void OnEditFindReplace(BOOL bFind);

	BOOL OnFind(CQuickFindWndDemo* pQuickFindWnd);
	BOOL OnFindAll(CQuickFindWndDemo* pQuickFindWnd);
	BOOL OnIncrementalFind(CQuickFindWndDemo* pQuickFindWnd);

	BOOL OnReplace(CQuickFindWndDemo* pQuickFindWnd);
	BOOL OnReplaceAll(CQuickFindWndDemo* pQuickFindWnd);
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

	DECLARE_MESSAGE_MAP()
};


