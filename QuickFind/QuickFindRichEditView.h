#pragma once
#include "QuickFindView.inl"

class CQuickFindCntrItem;

// CQuickFindRichEditView view

typedef CQuickFindView<CRichEditView>	CQuickFindRichEditViewBase;

class CQuickFindRichEditView : public CQuickFindRichEditViewBase
{
	DECLARE_DYNCREATE(CQuickFindRichEditView)

protected:
	CQuickFindRichEditView();           // protected constructor used by dynamic creation
	virtual ~CQuickFindRichEditView();
public:
	CQuickFindRichEditDoc* GetDocument() const;

	void SetText(LPCTSTR pszText) override;
	CString GetSelectedOrCursorText() const override;
	BOOL CanDoReplace() const override;
public:
	BOOL OnFind(CQuickFindWndDemo* pQuickFindWnd) override;
	BOOL OnFindAll(CQuickFindWndDemo* pQuickFindWnd) override;
	BOOL OnIncrementalFind(CQuickFindWndDemo* pQuickFindWnd) override;

	BOOL OnReplace(CQuickFindWndDemo* pQuickFindWnd) override;
	BOOL OnReplaceAll(CQuickFindWndDemo* pQuickFindWnd) override;
protected:
	virtual void OnInitialUpdate(); // called first time after construct

#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnDestroy();
protected:
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in QuickFindView.cpp
inline CQuickFindRichEditDoc* CQuickFindRichEditView::GetDocument() const
{ return reinterpret_cast<CQuickFindRichEditDoc*>(m_pDocument); }
#endif

