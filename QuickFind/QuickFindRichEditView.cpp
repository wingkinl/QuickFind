// QuickFindRichEditView.cpp : implementation file
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "QuickFind.h"
#endif
#include "QuickFindRichEditDoc.h"
#include "QuickFindRichEditView.h"
#include "CntrItem.h"


// CQuickFindRichEditView

IMPLEMENT_DYNCREATE(CQuickFindRichEditView, CQuickFindRichEditViewBase)

CQuickFindRichEditView::CQuickFindRichEditView()
{

}

CQuickFindRichEditView::~CQuickFindRichEditView()
{
}

BEGIN_MESSAGE_MAP(CQuickFindRichEditView, CQuickFindRichEditViewBase)
	ON_WM_DESTROY()
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
END_MESSAGE_MAP()

void CQuickFindRichEditView::OnInitialUpdate()
{
	CQuickFindRichEditViewBase::OnInitialUpdate();

	// Set the printing margins (720 twips = 1/2 inch)
	SetMargins(CRect(720, 720, 720, 720));
}

void CQuickFindRichEditView::OnDestroy()
{
	// Deactivate the item on destruction; this is important
	// when a splitter view is being used
	COleClientItem* pActiveItem = GetDocument()->GetInPlaceActiveItem(this);
	if (pActiveItem != NULL && pActiveItem->GetActiveView() == this)
	{
		pActiveItem->Deactivate();
		ASSERT(GetDocument()->GetInPlaceActiveItem(this) == NULL);
	}
	CQuickFindRichEditViewBase::OnDestroy();
}

void CQuickFindRichEditView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CQuickFindRichEditView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}

// CQuickFindRichEditView diagnostics

#ifdef _DEBUG
void CQuickFindRichEditView::AssertValid() const
{
	CQuickFindRichEditViewBase::AssertValid();
}

#ifndef _WIN32_WCE
void CQuickFindRichEditView::Dump(CDumpContext& dc) const
{
	CQuickFindRichEditViewBase::Dump(dc);
}
#endif

CQuickFindRichEditDoc* CQuickFindRichEditView::GetDocument() const
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CQuickFindRichEditDoc)));
	return (CQuickFindRichEditDoc*)m_pDocument;
}
#endif //_DEBUG


// CQuickFindRichEditView message handlers
