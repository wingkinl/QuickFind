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

static const UINT _QUICKFINDMSG = ::RegisterWindowMessage(QUICKFINDMSGSTRING);

BEGIN_MESSAGE_MAP(CQuickFindScintillaView, CQuickFindScintillaViewBase)
	ON_WM_DESTROY()
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_COMMAND(ID_EDIT_FIND, &CQuickFindScintillaView::OnEditFind)
	ON_COMMAND(ID_EDIT_REPLACE, &CQuickFindScintillaView::OnEditReplace)
	ON_REGISTERED_MESSAGE(_QUICKFINDMSG, &CQuickFindScintillaView::OnQuickFindCmd)
	ON_WM_SIZE()
	ON_MESSAGE_VOID(WM_IDLEUPDATECMDUI, CQuickFindScintillaView::OnIdleUpdateCmdUI)
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

static DWORD GetSCSearchFlags(CQuickFindWndDemo* pQuickFindWnd)
{
	DWORD dwSearchFlags = 0;
	if (pQuickFindWnd->IsMatchCase())
		dwSearchFlags |= SCFIND_MATCHCASE;

	if (pQuickFindWnd->IsMatchWholeWord())
		dwSearchFlags |= SCFIND_WHOLEWORD;

	if (pQuickFindWnd->IsUseRegEx())
		dwSearchFlags |= SCFIND_REGEXP | SCFIND_CXX11REGEX;
	return dwSearchFlags;
}

BOOL CQuickFindScintillaView::OnFind(CQuickFindWndDemo* pQuickFindWnd)
{
	ASSERT_VALID(this);
	ASSERT(pQuickFindWnd);
	if ( !CQuickFindScintillaViewBase::OnFind(pQuickFindWnd) )
		return FALSE;

	CString strFind = pQuickFindWnd->GetFindString();
	if (strFind.IsEmpty())
		return TRUE;
	auto nScope = pQuickFindWnd->GetCurScope();
	Sci_CharacterRange range;
	const Sci_CharacterRange* pCharRange = nullptr;
	BOOL bFindInSel = nScope == CQuickFindWndDemo::FindScopeSelection;
	if (bFindInSel)
	{
		range.cpMin = pQuickFindWnd->m_nSelBegin;
		range.cpMax = pQuickFindWnd->m_nSelEnd;
		pCharRange = &range;
	}
	BOOL bNext = pQuickFindWnd->IsFindReplaceNext();
	DWORD dwSCFlags = GetSCSearchFlags(pQuickFindWnd);
	BOOL bRet = FindTextSimple(strFind, bNext, dwSCFlags, pCharRange);
	return bRet;
}

BOOL CQuickFindScintillaView::OnFindAll(CQuickFindWndDemo* pQuickFindWnd)
{
	return CQuickFindScintillaViewBase::OnFindAll(pQuickFindWnd);
}

BOOL CQuickFindScintillaView::OnIncrementalFind(CQuickFindWndDemo* pQuickFindWnd)
{
	if (!CQuickFindScintillaViewBase::OnIncrementalFind(pQuickFindWnd))
		return FALSE;
	CString strFind = pQuickFindWnd->GetFindString();
	if (strFind.IsEmpty())
		return TRUE;
	auto& ctrl = GetCtrl();
	auto nFindBeginPos = min(pQuickFindWnd->m_nSelBegin, pQuickFindWnd->m_nSelEnd);
	Sci_TextToFind ft;
#ifdef _UNICODE
	CStringA sUTF8Text(CScintillaCtrl::W2UTF8(strFind, -1));
	ft.lpstrText = sUTF8Text.GetBuffer(sUTF8Text.GetLength());
#else
	CStringA sAsciiText(lpszFind);
	ft.lpstrText = sAsciiText.GetBuffer(sAsciiText.GetLength());
#endif //#ifdef _UNICODE
	ft.chrg.cpMin = 0;
	ft.chrg.cpMax = ctrl.GetLength();
	DWORD dwSCFlags = GetSCSearchFlags(pQuickFindWnd);
	BOOL bRet = FindAndSelect(ft, dwSCFlags, nFindBeginPos) != -1;
	return bRet;
}

BOOL CQuickFindScintillaView::OnReplace(CQuickFindWndDemo* pQuickFindWnd)
{
	if (!CQuickFindScintillaViewBase::OnReplace(pQuickFindWnd))
		return FALSE;
	CString strFind = pQuickFindWnd->GetFindString();
	if (strFind.IsEmpty())
		return TRUE;
	auto& ctrl = GetCtrl();
	CString strReplace = pQuickFindWnd->GetReplaceString();

	auto nScope = pQuickFindWnd->GetCurScope();
	Sci_CharacterRange range;
	const Sci_CharacterRange* pCharRange = nullptr;
	BOOL bFindInSel = nScope == CQuickFindWndDemo::FindScopeSelection;
	if (bFindInSel)
	{
		range.cpMin = pQuickFindWnd->m_nSelBegin;
		range.cpMax = pQuickFindWnd->m_nSelEnd;
		pCharRange = &range;
	}
	BOOL bNext = pQuickFindWnd->IsFindReplaceNext();
	DWORD dwSCFlags = GetSCSearchFlags(pQuickFindWnd);
	if (!SameAsSelected(strFind, dwSCFlags))
	{
		return FindTextSimple(strFind, bNext, dwSCFlags, pCharRange);
	}

	if (pQuickFindWnd->IsUseRegEx())
	{
		ctrl.TargetFromSelection();
		ctrl.ReplaceTargetRE(strReplace.GetLength(), strReplace);
	}
	else
		ctrl.ReplaceSel(strReplace);
	return FindTextSimple(strFind, bNext, dwSCFlags, pCharRange);
}

BOOL CQuickFindScintillaView::OnReplaceAll(CQuickFindWndDemo* pQuickFindWnd)
{
	if (!CQuickFindScintillaViewBase::OnReplaceAll(pQuickFindWnd))
		return FALSE;
	ASSERT(pQuickFindWnd);
	auto& ctrl = GetCtrl();
	DWORD dwSCFlags = GetSCSearchFlags(pQuickFindWnd);

	CString strFind = pQuickFindWnd->GetFindString();
	if (strFind.IsEmpty())
		return TRUE;

	CString strReplace = pQuickFindWnd->GetReplaceString();

	auto nScope = pQuickFindWnd->GetCurScope();
	Sci_CharacterRange range;
	const Sci_CharacterRange* pCharRange = nullptr;
	BOOL bFindInSel = nScope == CQuickFindWndDemo::FindScopeSelection;
	if (bFindInSel)
	{
		range.cpMin = pQuickFindWnd->m_nSelBegin;
		range.cpMax = pQuickFindWnd->m_nSelEnd;
		pCharRange = &range;
	}

	CWaitCursor wait;

	//Set the selection to the beginning of the document to ensure all text is replaced in the document
	ctrl.SetSel(0, 0);

	//Do the replacements
	ctrl.HideSelection(TRUE);
	ctrl.BeginUndoAction();
	BOOL bFoundSomething = FALSE;
	while (FindTextSimple(strFind, TRUE, dwSCFlags, pCharRange))
	{
		bFoundSomething = TRUE;
		if (pQuickFindWnd->IsUseRegEx())
		{
			ctrl.TargetFromSelection();
			ctrl.ReplaceTargetRE(strReplace.GetLength(), strReplace);
		}
		else
			ctrl.ReplaceSel(strReplace);
	}
	ctrl.EndUndoAction();
	//Restore the old selection
	ctrl.HideSelection(FALSE);
	return bFoundSomething;
}

BOOL CQuickFindScintillaView::FindTextSimple(LPCTSTR lpszFind, BOOL bNext, DWORD dwSCFlags, const Sci_CharacterRange* pCharRange /*= nullptr*/)
{
	ASSERT(lpszFind != nullptr);
	auto& ctrl = GetCtrl();
	auto nSelStart = ctrl.GetSelectionStart();
	auto nSelEnd = ctrl.GetSelectionEnd();

	Sci_TextToFind ft;
#ifdef _UNICODE
	CStringA sUTF8Text(CScintillaCtrl::W2UTF8(lpszFind, -1));
	ft.lpstrText = sUTF8Text.GetBuffer(sUTF8Text.GetLength());
#else
	CStringA sAsciiText(lpszFind);
	ft.lpstrText = sAsciiText.GetBuffer(sAsciiText.GetLength());
#endif //#ifdef _UNICODE
	Sci_Position nActualStartPos = nSelStart;
	if (nSelStart != nSelEnd) // i.e. there is a selection
	{
		if (bNext)
			nActualStartPos = nSelStart + 1;
		else
			nActualStartPos = nSelStart - 1;
	}
	if (pCharRange)
	{
		ft.chrg = *pCharRange;
	}
	else
	{
		ft.chrg.cpMin = 0;
		ft.chrg.cpMax = ctrl.GetLength();
	}

	if (nActualStartPos < ft.chrg.cpMin)
		nActualStartPos = ft.chrg.cpMax;
	else if (nActualStartPos > ft.chrg.cpMax)
		nActualStartPos = ft.chrg.cpMin;

	if (!bNext)
	{
		//Swap the start and end positions which Scintilla uses to flag backward searches
		std::swap(ft.chrg.cpMin, ft.chrg.cpMax);
	}

	BOOL bRet = FindAndSelect(ft, dwSCFlags, nActualStartPos) != -1;
	return bRet;
}

BOOL CQuickFindScintillaView::SameAsSelected(LPCTSTR lpszCompare, DWORD dwSCFlags)
{
	BOOL bCase = (dwSCFlags & SCFIND_MATCHCASE) != 0;
	BOOL bWord = (dwSCFlags & SCFIND_WHOLEWORD) != 0;
	BOOL bRegEx = (dwSCFlags & SCFIND_REGEXP) != 0;
	return CQuickFindScintillaViewBase::SameAsSelected(lpszCompare, bCase, bWord, bRegEx);
}

Sci_Position CQuickFindScintillaView::FindAndSelect(Sci_TextToFind& ft, DWORD dwSCFlags, Sci_Position nActualStartPos)
{
	Sci_CharacterRange range = ft.chrg;
	bool bWrapAround = ft.chrg.cpMin != nActualStartPos;
	ft.chrg.cpMin = nActualStartPos;

	auto& ctrl = GetCtrl();

	Sci_Position index = ctrl.FindText(dwSCFlags, &ft);
	if (index == -1 && bWrapAround)
	{
		//ft.chrg.cpMin = range.cpMin;
		//ft.chrg.cpMax = nActualStartPos;
		// the search above in defined range fails, but the reason may be
		// the starting position is in the middle of the targeted text, so
		// do a complete search again to see if we have any luck.
		ft.chrg = range;
		index = ctrl.FindText(dwSCFlags, &ft);
	}
	if (index != -1) // i.e. we found something
		ctrl.SetSel(ft.chrgText.cpMin, ft.chrgText.cpMax);
	return index;
}

#endif // _ENABLE_SCINTILLA_BUILD