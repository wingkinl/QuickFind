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
#include "QuickFindWnd.h"


// CQuickFindRichEditView

IMPLEMENT_DYNCREATE(CQuickFindRichEditView, CQuickFindRichEditViewBase)

CQuickFindRichEditView::CQuickFindRichEditView()
{

}

CQuickFindRichEditView::~CQuickFindRichEditView()
{
}

static const UINT _QUICKFINDMSG = ::RegisterWindowMessage(QUICKFINDMSGSTRING);

BEGIN_MESSAGE_MAP(CQuickFindRichEditView, CQuickFindRichEditViewBase)
	ON_WM_DESTROY()
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_COMMAND(ID_EDIT_FIND, &CQuickFindRichEditViewBase::OnEditFind)
	ON_COMMAND(ID_EDIT_REPLACE, &CQuickFindRichEditViewBase::OnEditReplace)
	ON_REGISTERED_MESSAGE(_QUICKFINDMSG, &CQuickFindRichEditViewBase::OnQuickFindCmd)
	ON_WM_SIZE()
	ON_MESSAGE_VOID(WM_IDLEUPDATECMDUI, CQuickFindRichEditViewBase::OnIdleUpdateCmdUI)
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

void CQuickFindRichEditView::SetText(LPCTSTR pszText)
{
	GetRichEditCtrl().SetWindowText(pszText);
}

CString CQuickFindRichEditView::GetSelectedOrCursorText() const
{
	return GetRichEditCtrl().GetSelText();
}

BOOL CQuickFindRichEditView::CanDoReplace() const
{
	return (GetRichEditCtrl().GetStyle() & ES_READONLY) == 0;
}

BOOL CQuickFindRichEditView::OnFind(CQuickFindWndDemo* pQuickFindWnd)
{
	if ( !CQuickFindRichEditViewBase::OnFind(pQuickFindWnd) )
		return FALSE;
	BOOL bRet = FindText(pQuickFindWnd->GetFindString(), pQuickFindWnd->IsMatchCase(), 
		pQuickFindWnd->IsMatchWholeWord(), pQuickFindWnd->IsFindReplaceNext());
	return bRet;
}

BOOL CQuickFindRichEditView::OnFindAll(CQuickFindWndDemo* pQuickFindWnd)
{
	ASSERT_VALID(this);
	ASSERT_VALID(pQuickFindWnd);
	ASSERT_VALID(this);
	CString strFind = pQuickFindWnd->GetFindString();
	CString strReplace = pQuickFindWnd->GetReplaceString();
	BOOL bCase = pQuickFindWnd->IsMatchCase();
	BOOL bWord = pQuickFindWnd->IsMatchWholeWord();
	BOOL bNext = pQuickFindWnd->IsFindReplaceNext();

	CWaitCursor wait;

	SetRedraw(FALSE);
	int nOldFirstVisibleLine = GetRichEditCtrl().GetFirstVisibleLine();

	FINDTEXTEX ft;
	GetRichEditCtrl().GetSel(ft.chrg);
	long lInitialCursorPosition = ft.chrg.cpMin;

	GetRichEditCtrl().HideSelection(TRUE, FALSE);
	CHARFORMAT2 cf;
	cf.cbSize = sizeof(cf);
	GetRichEditCtrl().GetDefaultCharFormat(cf);
	GetRichEditCtrl().SetSel(0, -1);
	GetRichEditCtrl().SetSelectionCharFormat(cf);
	GetRichEditCtrl().SetSel(0, 0);

	BOOL bFound = TRUE;
	if (!strFind.IsEmpty())
	{
		CHARFORMAT2 cfSel;
		cfSel.cbSize = sizeof(cfSel);
		cfSel.dwMask = CFM_BACKCOLOR;
		cfSel.dwEffects = 0;
		cfSel.crBackColor = RGB(246, 185, 77);

		bFound = FindText(strFind, bCase, bWord, bNext);
		if (bFound)
		{
			do
			{
				GetRichEditCtrl().GetSel(ft.chrg);
				GetRichEditCtrl().SetSelectionCharFormat(cfSel);
				if (ft.chrg.cpMin < lInitialCursorPosition)
				{
					lInitialCursorPosition += strReplace.GetLength() - strFind.GetLength();
				}
			} while (FindText(strFind, bCase, bWord, bNext));
		}
	}

	GetRichEditCtrl().SetSel(lInitialCursorPosition, lInitialCursorPosition);
	GetRichEditCtrl().HideSelection(FALSE, FALSE);

	// Prevent the auto-scroll of the control when calling SetSel()
	int nNewFirstVisibleLine = GetRichEditCtrl().GetFirstVisibleLine();

	if (nOldFirstVisibleLine != nNewFirstVisibleLine)
		GetRichEditCtrl().LineScroll(nOldFirstVisibleLine - nNewFirstVisibleLine);

	SetRedraw(TRUE);

	RedrawWindow();
	return bFound;
}

BOOL CQuickFindRichEditView::OnIncrementalFind(CQuickFindWndDemo* pQuickFindWnd)
{
	return OnFindAll(pQuickFindWnd);
}

BOOL CQuickFindRichEditView::OnReplace(CQuickFindWndDemo* pQuickFindWnd)
{
	if ( !CQuickFindRichEditViewBase::OnReplace(pQuickFindWnd) )
		return FALSE;
	OnReplaceSel(pQuickFindWnd->GetFindString(), pQuickFindWnd->IsFindReplaceNext(), 
		pQuickFindWnd->IsMatchCase(), pQuickFindWnd->IsMatchWholeWord(), pQuickFindWnd->GetReplaceString());
	return TRUE;
}

BOOL CQuickFindRichEditView::OnReplaceAll(CQuickFindWndDemo* pQuickFindWnd)
{
	if ( !CQuickFindRichEditViewBase::OnReplaceAll(pQuickFindWnd) )
		return FALSE;
	CRichEditView::OnReplaceAll(pQuickFindWnd->GetFindString(), pQuickFindWnd->GetReplaceString(), 
		pQuickFindWnd->IsMatchCase(), pQuickFindWnd->IsMatchWholeWord());
	return TRUE;
}
