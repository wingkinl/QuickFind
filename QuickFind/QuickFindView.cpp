
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
#include "QuickFindWndDemo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CQuickFindView

IMPLEMENT_DYNCREATE(CQuickFindView, CQuickFindViewBase)

static const UINT _QUICKFINDMSG = ::RegisterWindowMessage(QUICKFINDMSGSTRING);

BEGIN_MESSAGE_MAP(CQuickFindView, CQuickFindViewBase)
	ON_WM_DESTROY()
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_COMMAND(ID_EDIT_FIND, &CQuickFindView::OnEditFind)
	ON_COMMAND(ID_EDIT_REPLACE, &CQuickFindView::OnEditReplace)
	ON_REGISTERED_MESSAGE(_QUICKFINDMSG, &CQuickFindView::OnQuickFindCmd)
	ON_WM_SIZE()
	ON_MESSAGE_VOID(WM_IDLEUPDATECMDUI, CQuickFindView::OnIdleUpdateCmdUI)
END_MESSAGE_MAP()

// CQuickFindView construction/destruction

CQuickFindView::CQuickFindView()
{
	
}

CQuickFindView::~CQuickFindView()
{
}

void CQuickFindView::OnInitialUpdate()
{
	CQuickFindViewBase::OnInitialUpdate();

	CString strText;
	strText.Format(_T("Hello World, what a wonderful world! %p"), this);
	strText += _T("What's WhatsApp? That is a cat wearing a white Hat.");
	GetRichEditCtrl().SetWindowText(strText);
}

void CQuickFindView::CheckCloseOwnedFindWindow()
{
	if (::IsWindow(_quickfindState.pQuickFindWnd->GetSafeHwnd()) && _quickfindState.pQuickFindWnd->GetOwner() == this)
	{
		_quickfindState.pQuickFindWnd->SendMessage(WM_CLOSE);
		_quickfindState.pQuickFindWnd->SetOwner(nullptr);
		_quickfindState.pQuickFindWnd = nullptr;
	}
}

BOOL CQuickFindView::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_ESCAPE)
		{
			CheckCloseOwnedFindWindow();
		}
	}
	return __super::PreTranslateMessage(pMsg);
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
   CheckCloseOwnedFindWindow();
   CQuickFindViewBase::OnDestroy();
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
	CQuickFindViewBase::AssertValid();
}

void CQuickFindView::Dump(CDumpContext& dc) const
{
	CQuickFindViewBase::Dump(dc);
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

CQuickFindWnd* CQuickFindView::CreateFindReplaceWindow()
{
	return new CQuickFindWndDemo;
}

CString CQuickFindView::GetSelectedOrCursorText() const
{
	return GetRichEditCtrl().GetSelText();
}

BOOL CQuickFindView::CanDoReplace() const
{
	return (GetRichEditCtrl().GetStyle() & ES_READONLY) == 0;
}

void CQuickFindView::OnEditFindReplace(BOOL bFind)
{
	CString strFind = GetSelectedOrCursorText();
	// if selection is empty or spans multiple lines use old find text
	if (strFind.IsEmpty() || strFind.FindOneOf(_T("\n\r")) != -1)
	{
		if (_quickfindState.info.saSearch.IsEmpty())
			strFind.Empty();
		else
			strFind = _quickfindState.info.saSearch[0];
	}
	else
		_quickfindState.info.PromoteStringInArray(strFind);
	if (_quickfindState.pQuickFindWnd)
	{
		_quickfindState.pQuickFindWnd->SetNotifyOwner(this);
		_quickfindState.pQuickFindWnd->m_bCanShowReplaceUI = CanDoReplace();
		_quickfindState.pQuickFindWnd->SendMessage(WM_COMMAND, bFind ? ID_EDIT_FIND : ID_EDIT_REPLACE);
		_quickfindState.pQuickFindWnd->SetFindString(strFind);
		_quickfindState.pQuickFindWnd->SetActiveShowWindow();
		return;
	}	
	_quickfindState.pQuickFindWnd = CreateFindReplaceWindow();
	ASSERT(_quickfindState.pQuickFindWnd);
	_quickfindState.pQuickFindWnd->m_bCanShowReplaceUI = CanDoReplace();
	if (!_quickfindState.pQuickFindWnd->Create(_quickfindState.info, this))
	{
		_quickfindState.pQuickFindWnd = nullptr;
		ASSERT_VALID(this);
		return;
	}
	CStringArray saScopes;
	saScopes.Add(_T("Current Block"));
	saScopes.Add(_T("Current Document"));
	saScopes.Add(_T("All Open Documents"));
	_quickfindState.pQuickFindWnd->SetScopeItems(saScopes, 1);
	_quickfindState.pQuickFindWnd->SetActiveShowWindow();
	ASSERT_VALID(this);
}

BOOL CQuickFindView::OnFind(CQuickFindWndDemo* pQuickFindWnd)
{
	ASSERT_VALID(this);
	ASSERT_VALID(pQuickFindWnd);
	_quickfindState.info.dwFlags = pQuickFindWnd->GetFlags();
	pQuickFindWnd->GetFindStringArray(_quickfindState.info.saSearch);
	BOOL bRet = FindText(pQuickFindWnd->GetFindString(), pQuickFindWnd->IsMatchCase(), 
		pQuickFindWnd->IsMatchWholeWord(), pQuickFindWnd->IsFindReplaceNext());
	return bRet;
}

BOOL CQuickFindView::OnFindAll(CQuickFindWndDemo* pQuickFindWnd)
{
	return OnIncrementalFind(pQuickFindWnd);
}

BOOL CQuickFindView::OnIncrementalFind(CQuickFindWndDemo* pQuickFindWnd)
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

BOOL CQuickFindView::OnReplace(CQuickFindWndDemo* pQuickFindWnd)
{
	ASSERT_VALID(this);
	ASSERT_VALID(pQuickFindWnd);
	_quickfindState.info.dwFlags = pQuickFindWnd->GetFlags();
	pQuickFindWnd->GetReplaceStringArray(_quickfindState.info.saReplace);
	OnReplaceSel(pQuickFindWnd->GetFindString(), pQuickFindWnd->IsFindReplaceNext(), 
		pQuickFindWnd->IsMatchCase(), pQuickFindWnd->IsMatchWholeWord(), pQuickFindWnd->GetReplaceString());
	return TRUE;
}

BOOL CQuickFindView::OnReplaceAll(CQuickFindWndDemo* pQuickFindWnd)
{
	ASSERT_VALID(this);
	ASSERT_VALID(pQuickFindWnd);
	_quickfindState.info.dwFlags = pQuickFindWnd->GetFlags();
	pQuickFindWnd->GetReplaceStringArray(_quickfindState.info.saReplace);
	__super::OnReplaceAll(pQuickFindWnd->GetFindString(), pQuickFindWnd->GetReplaceString(), 
		pQuickFindWnd->IsMatchCase(), pQuickFindWnd->IsMatchWholeWord());
	return TRUE;
}

LRESULT CQuickFindView::OnQuickFindCmd(WPARAM wp, LPARAM lp)
{
	QuickFindCmd cmd = static_cast<QuickFindCmd>(wp);
	CQuickFindWnd::QNMHDR* pQNMHDR = reinterpret_cast<CQuickFindWnd::QNMHDR*>(lp);
	CQuickFindWndDemo* pQuickFindWnd = static_cast<CQuickFindWndDemo*>(pQNMHDR->pNotifier);
	ASSERT_VALID(pQuickFindWnd);
	ASSERT(!_quickfindState.pQuickFindWnd || pQuickFindWnd == _quickfindState.pQuickFindWnd);
	switch (cmd)
	{
	case QuickFindCmdTerminating:
		_quickfindState.pQuickFindWnd = nullptr;
		break;
	case QuickFindCmdFind:
		return (LRESULT)OnFind(pQuickFindWnd);
	case QuickFindCmdFindAll:
		return (LRESULT)OnFindAll(pQuickFindWnd);
	case QuickFindCmdReplace:
		return (LRESULT)OnReplace(pQuickFindWnd);
	case QuickFindCmdReplaceAll:
		return (LRESULT)OnReplaceAll(pQuickFindWnd);
	case QuickFindCmdFindTextChange:
		return (LRESULT)OnIncrementalFind(pQuickFindWnd);
	case QuickFindCmdOptionsChange:
		return (LRESULT)OnFind(pQuickFindWnd);
	default:
		break;
	}
	return 0;
}

void CQuickFindView::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);
	if (_quickfindState.pQuickFindWnd)
		_quickfindState.pQuickFindWnd->UpdateWindowPos();
}

void CQuickFindView::OnIdleUpdateCmdUI()
{
	if ( _quickfindState.pQuickFindWnd && (_quickfindState.pQuickFindWnd->GetStyle() & WS_POPUP) )
		_quickfindState.pQuickFindWnd->SendMessage(WM_IDLEUPDATECMDUI);
}

