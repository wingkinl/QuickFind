// QuickFindRichEditView.cpp : implementation file
//

#include "stdafx.h"
#include "QuickFindDoc.h"
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

CQuickFindDoc* CQuickFindRichEditView::GetDocument() const
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CQuickFindDoc)));
	return (CQuickFindDoc*)m_pDocument;
}
#endif //_DEBUG


// CQuickFindRichEditView message handlers
