
// QuickFindView.cpp : implementation of the CQuickFindView class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "QuickFind.h"
#endif

#include "QuickFindDoc.h"
#include "CntrItem.h"
#include "resource.h"
#include "QuickFindView.h"
#include "QuickFindWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

class _QUICKFIND_STATE
{
public:
	_QUICKFIND_STATE();

	CQuickFindWnd*	pQuickFindWnd;
	QUICKFIND_INFO	info;
};

_QUICKFIND_STATE::_QUICKFIND_STATE()
{
	pQuickFindWnd = nullptr;
}

_QUICKFIND_STATE _quickfindState;

// CQuickFindView

IMPLEMENT_DYNCREATE(CQuickFindView, CRichEditView)

BEGIN_MESSAGE_MAP(CQuickFindView, CRichEditView)
	ON_WM_DESTROY()
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_COMMAND(ID_EDIT_FIND, &CQuickFindView::OnEditFind)
	ON_COMMAND(ID_EDIT_REPLACE, &CQuickFindView::OnEditReplace)
END_MESSAGE_MAP()

// CQuickFindView construction/destruction

CQuickFindView::CQuickFindView()
{
	// TODO: add construction code here

}

CQuickFindView::~CQuickFindView()
{
}

BOOL CQuickFindView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CRichEditView::PreCreateWindow(cs);
}

void CQuickFindView::OnInitialUpdate()
{
	CRichEditView::OnInitialUpdate();

	CString strText;
	strText.Format(_T("Hello world, what a wonderful world! %p"), this);
	GetRichEditCtrl().SetWindowText(strText);

	// Set the printing margins (720 twips = 1/2 inch)
	SetMargins(CRect(720, 720, 720, 720));
}

void CQuickFindView::OnDestroy()
{
	// Deactivate the item on destruction; this is important
	// when a splitter view is being used
   COleClientItem* pActiveItem = GetDocument()->GetInPlaceActiveItem(this);
   if (pActiveItem != NULL && pActiveItem->GetActiveView() == this)
   {
      pActiveItem->Deactivate();
      ASSERT(GetDocument()->GetInPlaceActiveItem(this) == NULL);
   }
   CRichEditView::OnDestroy();
}


void CQuickFindView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CQuickFindView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CQuickFindView diagnostics

#ifdef _DEBUG
void CQuickFindView::AssertValid() const
{
	CRichEditView::AssertValid();
}

void CQuickFindView::Dump(CDumpContext& dc) const
{
	CRichEditView::Dump(dc);
}

CQuickFindDoc* CQuickFindView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CQuickFindDoc)));
	return (CQuickFindDoc*)m_pDocument;
}
#endif //_DEBUG


// CQuickFindView message handlers

void CQuickFindView::OnEditFind()
{
	ASSERT_VALID(this);
	OnEditFindReplace(TRUE);
}

void CQuickFindView::OnEditReplace()
{
	ASSERT_VALID(this);
	OnEditFindReplace(FALSE);
}

void CQuickFindView::OnEditFindReplace(BOOL bFind)
{
	if (_quickfindState.pQuickFindWnd)
	{
		// TODO: need to switch UI
		_quickfindState.pQuickFindWnd->SetActiveWindow();
		_quickfindState.pQuickFindWnd->ShowWindow(SW_SHOW);
		return;
	}
	CString strFind = GetRichEditCtrl().GetSelText();
	// if selection is empty or spans multiple lines use old find text
	if (strFind.IsEmpty() || (strFind.FindOneOf(_T("\n\r")) != -1))
	{
		
	}
	_quickfindState.pQuickFindWnd = new CQuickFindWnd;
	ASSERT(_quickfindState.pQuickFindWnd);
	_quickfindState.info.pWndOwner = this;
	if (!_quickfindState.pQuickFindWnd->Create(_quickfindState.info))
	{
		_quickfindState.pQuickFindWnd = nullptr;
		ASSERT_VALID(this);
		return;
	}
	_quickfindState.pQuickFindWnd->SetWindowPos(nullptr, 100, 100, -1, -1, SWP_NOSIZE);
	_quickfindState.pQuickFindWnd->SetActiveWindow();
	_quickfindState.pQuickFindWnd->ShowWindow(SW_SHOW);
	ASSERT_VALID(this);
}

