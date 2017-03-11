
// QuickFindView.h : interface of the CQuickFindView class
//

#pragma once
#include "QuickFindWnd.h"

class CQuickFindCntrItem;

class CQuickFindWndDemo : public CQuickFindWnd
{
public:
	CQuickFindWndDemo();
protected:
	DECLARE_MESSAGE_MAP()
};

class CQuickFindView : public CRichEditView
{
protected: // create from serialization only
	CQuickFindView();
	DECLARE_DYNCREATE(CQuickFindView)

// Attributes
public:
	CQuickFindDoc* GetDocument() const;

// Operations
public:

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void OnInitialUpdate(); // called first time after construct
	
	virtual CQuickFindWnd* CreateFindReplaceWindow();
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

	BOOL OnReplace(CQuickFindWndDemo* pQuickFindWnd);
	BOOL OnReplaceAll(CQuickFindWndDemo* pQuickFindWnd);
protected:

// Generated message map functions
protected:
	afx_msg void OnDestroy();
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnEditFind();
	afx_msg void OnEditReplace();
	afx_msg LRESULT OnQuickFindCmd(WPARAM wp, LPARAM lp);

	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in QuickFindView.cpp
inline CQuickFindDoc* CQuickFindView::GetDocument() const
   { return reinterpret_cast<CQuickFindDoc*>(m_pDocument); }
#endif

