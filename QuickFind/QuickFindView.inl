#pragma once
// QuickFindView.cpp : implementation of the CQuickFindView class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "QuickFind.h"
#endif

#include "QuickFindView.h"
#include "QuickFindWndDemo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CQuickFindView

// CQuickFindView construction/destruction


template <typename CQuickFindViewBase>
CQuickFindView<CQuickFindViewBase>::CQuickFindView()
{

}

template <typename CQuickFindViewBase>
CQuickFindView<CQuickFindViewBase>::~CQuickFindView()
{
	
}

template <typename CQuickFindViewBase>
void CQuickFindView<CQuickFindViewBase>::OnInitialUpdate()
{
	CQuickFindViewBase::OnInitialUpdate();

	CString strText;
	strText.Format(_T("Hello World, what a wonderful world! %p"), this);
	strText += _T("\r\nWhat's WhatsApp? That is a cat wearing a white Hat.");
	SetText(strText);
}

template <typename CQuickFindViewBase>
void CQuickFindView<CQuickFindViewBase>::CheckCloseOwnedFindWindow()
{
	if (::IsWindow(_quickfindState.pQuickFindWnd->GetSafeHwnd()) && _quickfindState.pQuickFindWnd->GetOwner() == this)
	{
		_quickfindState.pQuickFindWnd->SendMessage(WM_CLOSE);
		_quickfindState.pQuickFindWnd->SetOwner(nullptr);
		_quickfindState.pQuickFindWnd = nullptr;
	}
}

template <typename CQuickFindViewBase>
BOOL CQuickFindView<CQuickFindViewBase>::PreTranslateMessage(MSG* pMsg)
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

template <typename CQuickFindViewBase>
void CQuickFindView<CQuickFindViewBase>::OnDestroy()
{
   CheckCloseOwnedFindWindow();
   CQuickFindViewBase::OnDestroy();
}


// CQuickFindView diagnostics

#ifdef _DEBUG
template <typename CQuickFindViewBase>
void CQuickFindView<CQuickFindViewBase>::AssertValid() const
{
	CQuickFindViewBase::AssertValid();
}

template <typename CQuickFindViewBase>
void CQuickFindView<CQuickFindViewBase>::Dump(CDumpContext& dc) const
{
	CQuickFindViewBase::Dump(dc);
}
#endif //_DEBUG


// CQuickFindView message handlers

template <typename CQuickFindViewBase>
void CQuickFindView<CQuickFindViewBase>::OnEditFind()
{
	ASSERT_VALID(this);
	OnEditFindReplace(TRUE);
}

template <typename CQuickFindViewBase>
void CQuickFindView<CQuickFindViewBase>::OnEditReplace()
{
	ASSERT_VALID(this);
	OnEditFindReplace(FALSE);
}

template <typename CQuickFindViewBase>
CQuickFindWnd* CQuickFindView<CQuickFindViewBase>::CreateFindReplaceWindow()
{
	return new CQuickFindWndDemo;
}

template <typename CQuickFindViewBase>
void CQuickFindView<CQuickFindViewBase>::OnEditFindReplace(BOOL bFind)
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

template <typename CQuickFindViewBase>
BOOL CQuickFindView<CQuickFindViewBase>::OnFind(CQuickFindWndDemo* pQuickFindWnd)
{
	ASSERT_VALID(this);
	ASSERT_VALID(pQuickFindWnd);
	_quickfindState.info.dwFlags = pQuickFindWnd->GetFlags();
	pQuickFindWnd->GetFindStringArray(_quickfindState.info.saSearch);
	return TRUE;
}

template <typename CQuickFindViewBase>
BOOL CQuickFindView<CQuickFindViewBase>::OnFindAll(CQuickFindWndDemo* /*pQuickFindWnd*/)
{
	return TRUE;
}

template <typename CQuickFindViewBase>
BOOL CQuickFindView<CQuickFindViewBase>::OnIncrementalFind(CQuickFindWndDemo* /*pQuickFindWnd*/)
{
	return TRUE;
}

template <typename CQuickFindViewBase>
BOOL CQuickFindView<CQuickFindViewBase>::OnReplace(CQuickFindWndDemo* pQuickFindWnd)
{
	ASSERT_VALID(this);
	ASSERT_VALID(pQuickFindWnd);
	_quickfindState.info.dwFlags = pQuickFindWnd->GetFlags();
	pQuickFindWnd->GetReplaceStringArray(_quickfindState.info.saReplace);
	return TRUE;
}

template <typename CQuickFindViewBase>
BOOL CQuickFindView<CQuickFindViewBase>::OnReplaceAll(CQuickFindWndDemo* pQuickFindWnd)
{
	ASSERT_VALID(this);
	ASSERT_VALID(pQuickFindWnd);
	_quickfindState.info.dwFlags = pQuickFindWnd->GetFlags();
	pQuickFindWnd->GetReplaceStringArray(_quickfindState.info.saReplace);
	return TRUE;
}

template <typename CQuickFindViewBase>
LRESULT CQuickFindView<CQuickFindViewBase>::OnQuickFindCmd(WPARAM wp, LPARAM lp)
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

template <typename CQuickFindViewBase>
void CQuickFindView<CQuickFindViewBase>::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);
	if (_quickfindState.pQuickFindWnd)
		_quickfindState.pQuickFindWnd->UpdateWindowPos();
}

template <typename CQuickFindViewBase>
void CQuickFindView<CQuickFindViewBase>::OnIdleUpdateCmdUI()
{
	if ( _quickfindState.pQuickFindWnd && (_quickfindState.pQuickFindWnd->GetStyle() & WS_POPUP) )
		_quickfindState.pQuickFindWnd->SendMessage(WM_IDLEUPDATECMDUI);
}

