
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
#include <..\src\mfc\afximpl.h>

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

CQuickFindWndDemo::CQuickFindWndDemo()
{

}

BEGIN_MESSAGE_MAP(CQuickFindWndDemo, CQuickFindWnd)
END_MESSAGE_MAP()


// CQuickFindView

IMPLEMENT_DYNCREATE(CQuickFindView, CRichEditView)

static const UINT _QUICKFINDMSG = ::RegisterWindowMessage(QUICKFINDMSGSTRING);

BEGIN_MESSAGE_MAP(CQuickFindView, CRichEditView)
	ON_WM_DESTROY()
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_COMMAND(ID_EDIT_FIND, &CQuickFindView::OnEditFind)
	ON_COMMAND(ID_EDIT_REPLACE, &CQuickFindView::OnEditReplace)
	ON_REGISTERED_MESSAGE(_QUICKFINDMSG, &CQuickFindView::OnQuickFindCmd)
	ON_WM_SIZE()
END_MESSAGE_MAP()

// CQuickFindView construction/destruction

CQuickFindView::CQuickFindView()
{
	
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
   if (::IsWindow(_quickfindState.pQuickFindWnd->GetSafeHwnd()) && _quickfindState.pQuickFindWnd->GetOwner() == this)
   {
	   _quickfindState.pQuickFindWnd->SendMessage(WM_CLOSE);
	   _quickfindState.pQuickFindWnd = nullptr;
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

CQuickFindWnd* CQuickFindView::CreateFindReplaceWindow()
{
	return new CQuickFindWndDemo;
}

CString CQuickFindView::GetSelText() const
{
	return GetRichEditCtrl().GetSelText();
}

void CQuickFindView::OnEditFindReplace(BOOL bFind)
{
	CString strFind = GetSelText();
	// if selection is empty or spans multiple lines use old find text
	if (!strFind.IsEmpty() && strFind.FindOneOf(_T("\n\r")) == -1)
	{
		_quickfindState.info.PromoteStringInArray(strFind);
	}
	if (_quickfindState.pQuickFindWnd)
	{
		_quickfindState.pQuickFindWnd->SetNotifyOwner(this);
		_quickfindState.pQuickFindWnd->SendMessage(WM_COMMAND, bFind ? ID_EDIT_FIND : ID_EDIT_REPLACE);
		_quickfindState.pQuickFindWnd->SetFindString(strFind);
		_quickfindState.pQuickFindWnd->SetActiveShowWindow();
		return;
	}	
	_quickfindState.pQuickFindWnd = CreateFindReplaceWindow();
	ASSERT(_quickfindState.pQuickFindWnd);
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
	return OnHighlightFind(pQuickFindWnd);
}

BOOL CQuickFindView::OnHighlightFind(CQuickFindWndDemo* pQuickFindWnd)
{
	ASSERT_VALID(this);
	ASSERT_VALID(pQuickFindWnd);
	ASSERT_VALID(this);
	_AFX_RICHEDIT_STATE* pEditState = _afxRichEditState;
	pEditState->strFind = pQuickFindWnd->GetFindString();	
	pEditState->bCase = pQuickFindWnd->IsMatchCase();
	pEditState->bWord = pQuickFindWnd->IsMatchWholeWord();
	pEditState->bNext = pQuickFindWnd->IsFindReplaceNext();

	CWaitCursor wait;

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
	if (!pEditState->strFind.IsEmpty())
	{
		CHARFORMAT2 cfSel;
		cfSel.cbSize = sizeof(cfSel);
		cfSel.dwMask = CFM_BACKCOLOR;
		cfSel.dwEffects = 0;
		cfSel.crBackColor = RGB(246, 185, 77);

		bFound = FindText(pEditState);
		if (bFound)
		{
			do
			{
				GetRichEditCtrl().GetSel(ft.chrg);
				GetRichEditCtrl().SetSelectionCharFormat(cfSel);
				if (ft.chrg.cpMin < lInitialCursorPosition)
				{
					lInitialCursorPosition += pEditState->strReplace.GetLength() - pEditState->strFind.GetLength();
				}
			} while (FindText(pEditState));
		}
	}

	GetRichEditCtrl().SetSel(lInitialCursorPosition, lInitialCursorPosition);
	GetRichEditCtrl().HideSelection(FALSE, FALSE);
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
		return (LRESULT)OnHighlightFind(pQuickFindWnd);
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

