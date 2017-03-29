// QuickFindScintillaView.cpp : implementation file
//

#include "stdafx.h"
#ifdef _ENABLE_SCINTILLA_BUILD
#include "QuickFind.h"
#include "QuickFindScintillaView.h"


// CQuickFindScintillaView

IMPLEMENT_DYNCREATE(CQuickFindScintillaView, CQuickFindScintillaViewBase)

CQuickFindScintillaView::CQuickFindScintillaView()
{

}

CQuickFindScintillaView::~CQuickFindScintillaView()
{
}

BEGIN_MESSAGE_MAP(CQuickFindScintillaView, CQuickFindScintillaViewBase)
END_MESSAGE_MAP()


// CQuickFindScintillaView diagnostics

#ifdef _DEBUG
void CQuickFindScintillaView::AssertValid() const
{
	CQuickFindScintillaViewBase::AssertValid();
}

#ifndef _WIN32_WCE
void CQuickFindScintillaView::Dump(CDumpContext& dc) const
{
	CQuickFindScintillaViewBase::Dump(dc);
}
#endif
#endif //_DEBUG


// CQuickFindScintillaView message handlers

void CQuickFindScintillaView::SetText(LPCTSTR pszText)
{
	GetCtrl().SetText(pszText);
}

CString CQuickFindScintillaView::GetSelectedOrCursorText() const
{
	auto pThis = const_cast<CQuickFindScintillaView*>(this);
	auto& ctrl = pThis->GetCtrl();
	CString strText = ctrl.GetSelText();
	return strText;
}

BOOL CQuickFindScintillaView::CanDoReplace() const
{
	return !const_cast<CQuickFindScintillaView*>(this)->GetCtrl().GetReadOnly();
}

#endif // _ENABLE_SCINTILLA_BUILD