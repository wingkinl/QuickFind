/*
MIT License

Copyright(c) 2017 wingkinl (Kenny Lau)

https://github.com/wingkinl/QuickFind

History: 3/21/2017 Initial version

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files(the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

// QuickFindWnd.cpp : implementation file
//

#include "stdafx.h"
#include "QuickFindWnd.h"
#include "afxdialogex.h"
#include "Resource.h"

// CQuickFindWnd dialog

#define QFTIMER_ID_DOCK_EVENT	1000
#define QFTIMER_DOCK_TIMEOUT	500

IMPLEMENT_DYNAMIC(CQuickFindWnd, CQuickFindWndBase)

CQuickFindWnd::CQuickFindWnd()
	: CQuickFindWndBase(IDD_QUICK_FIND_REPLACE)
{
	m_bCanShowReplaceUI = TRUE;
	m_szLastClientSize.SetSize(0, 0);
	m_bShowReplaceUI = FALSE;
	m_bShowOptionsUI = TRUE;
	ZeroMemory(&m_arrUIRows, sizeof(m_arrUIRows));
	m_hAccel = nullptr;
	m_pAccelTable = nullptr;
	m_nAccelSize = 0;
	m_bActive = FALSE;
	m_nDockTimerID = 0;
}

CQuickFindWnd::~CQuickFindWnd()
{
	delete[] m_pAccelTable;
	m_pAccelTable = nullptr;
}

void CQuickFindWnd::DoDataExchange(CDataExchange* pDX)
{
	CQuickFindWndBase::DoDataExchange(pDX);	
	DDX_Control(pDX, IDOK, m_wndFindAction);
	DDX_Control(pDX, IDCANCEL, m_wndClose);
	DDX_Control(pDX, ID_QUICKFIND_FINDWHAT, m_wndFind);
	DDX_Control(pDX, ID_QUICKFIND_REPLACEWITH, m_wndReplace);
	DDX_Control(pDX, ID_QUICKFIND_REPLACENEXT, m_wndReplaceNext);
	DDX_Control(pDX, ID_QUICKFIND_REPLACEALL, m_wndReplaceAll);
	DDX_Control(pDX, ID_QUICKFIND_MATCHCASE, m_wndMatchCase);
	DDX_Control(pDX, ID_QUICKFIND_MATCHWORD, m_wndMatchWord);
	DDX_Control(pDX, ID_QUICKFIND_REGEX, m_wndRegEx);
	DDX_Control(pDX, ID_QUICKFIND_SCOPE, m_wndScope);
	DDX_Control(pDX, ID_QUICKFIND_FLOAT, m_wndDockFloat);
}

static const UINT _QUICKFINDMSG = ::RegisterWindowMessage(QUICKFINDMSGSTRING);

BEGIN_MESSAGE_MAP(CQuickFindWnd, CQuickFindWndBase)
	ON_WM_DESTROY()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_NCHITTEST()
	ON_WM_SETCURSOR()
	ON_WM_SIZING()
	ON_WM_SIZE()
	ON_WM_MOVING()
	ON_WM_WINDOWPOSCHANGING()
	ON_WM_GETMINMAXINFO()
	ON_BN_CLICKED(IDOK, &OnButtonFindAction)
	ON_COMMAND(ID_QUICKFIND_MATCHCASE, &OnMatchCase)
	ON_COMMAND(ID_QUICKFIND_MATCHWORD, &OnMatchWord)
	ON_COMMAND(ID_QUICKFIND_REGEX, &OnUseRegEx)
	ON_CBN_SELCHANGE(ID_QUICKFIND_FINDWHAT, &OnSelChangeFind)
	ON_CBN_EDITCHANGE(ID_QUICKFIND_FINDWHAT, &OnEditChangeFind)
	ON_CBN_SELCHANGE(ID_QUICKFIND_SCOPE, &OnSelChangeScope)
	ON_COMMAND(ID_QUICKFIND_NEXT, &OnFindNext)
	ON_COMMAND(ID_QUICKFIND_PREVIOUS, &OnFindPrevious)
	ON_COMMAND(ID_QUICKFIND_ALL, &OnFindAll)
	ON_COMMAND(ID_QUICKFIND_REPLACENEXT, &OnReplaceNext)
	ON_COMMAND(ID_QUICKFIND_REPLACEALL, &OnReplaceAll)
	ON_COMMAND(ID_EDIT_FIND, &OnEditFind)
	ON_COMMAND(ID_EDIT_REPLACE, &OnEditReplace)
	ON_COMMAND(ID_QUICKFIND_FLOAT, &OnSwitchFloatDock)
	ON_MESSAGE(WM_IDLEUPDATECMDUI, &OnIdleUpdateCmdUI)
	ON_WM_TIMER()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
END_MESSAGE_MAP()


// CQuickFindWnd message handlers

BOOL CQuickFindWnd::FindAccelerator(UINT uiCmd, CString& str) const
{
	if (!m_pAccelTable)
		return FALSE;
	BOOL bFound = FALSE;
	for (int i = 0; i < m_nAccelSize; i++)
	{
		if (m_pAccelTable[i].cmd == uiCmd)
		{
			bFound = TRUE;

			CMFCAcceleratorKey helper(&m_pAccelTable[i]);

			CString strKey;
			helper.Format(strKey);

			if (!str.IsEmpty())
			{
				str += _T("; ");
			}

			str += strKey;

			break;
		}
	}
	return bFound;
}

void CQuickFindWnd::PostNcDestroy()
{
	ASSERT(m_hWnd == NULL);
	delete this;
}

void CQuickFindWnd::OnCancel()
{
	auto pWndOwner = GetOwner();
	if (pWndOwner->GetSafeHwnd())
	{
		pWndOwner->SetFocus();
	}
	CQuickFindWndBase::OnCancel();
}

BOOL CQuickFindWnd::InitButton(CMFCButton& btn, UINT nID, HINSTANCE hResInst, int nImgSize) const
{
	auto resName = MAKEINTRESOURCE(nID);
	if (!hResInst)
	{
		hResInst = AfxFindResourceHandle(resName, RT_GROUP_ICON);
		if (!hResInst)
		{
			return FALSE;
		}
	}
	btn.m_nFlatStyle = CMFCButton::BUTTONSTYLE_FLAT;
	//btn.m_bTransparent = TRUE;
	btn.SetWindowText(_T(""));

	auto hIcon = (HICON)LoadImage(hResInst, resName, IMAGE_ICON, nImgSize, nImgSize, LR_SHARED);
	btn.SetImage(hIcon);

	CString strTooltip;
	strTooltip.LoadString(nID);

 	CString strAccel;

	BOOL bHasAccel = FindAccelerator(nID, strAccel);
	if (bHasAccel)
	{
		strTooltip += _T(" (");
		strTooltip += strAccel;
		strTooltip += _T(')');
	}

	btn.SetTooltip(strTooltip);
	return TRUE;
}

BOOL CQuickFindWnd::InitCombo(CComboBox& combo, const CStringArray& saText)
{
	combo.ResetContent();
	for (INT_PTR ii = 0; ii < saText.GetSize(); ++ii)
	{
		combo.AddString(saText[ii]);
	}
	if (!saText.IsEmpty())
	{
		combo.SetCurSel(0);
	}
	return TRUE;
}

BOOL CQuickFindWnd::Create(const QUICKFIND_INFO& info, CWnd* pParentWnd)
{
	m_info = info;	
	// left vs right, top vs bottom, they should be mutually exclusive.
	if (m_info.IsDockLeft() && m_info.IsDockRight())
		m_info.dwFlags &= ~QUICKFIND_INFO::FlagsDockLeft;
	if (m_info.IsDockTop() && m_info.IsDockBottom())
		m_info.dwFlags &= ~QUICKFIND_INFO::FlagsDockBottom;
	BOOL bRet = CQuickFindWndBase::Create(IDD_QUICK_FIND_REPLACE, pParentWnd);
	if (!bRet)
		return FALSE;
	SwitchFloatDock(m_info.IsFloating(), pParentWnd);
	return TRUE;
}

const int g_szDockFloatBtnSize = 10;

void CQuickFindWnd::OnSwitchFloatDock()
{
	BOOL bFloat = m_wndDockFloat.GetCheck() != BST_UNCHECKED;
	SwitchFloatDock(bFloat, GetOwner());
}

void CQuickFindWnd::SwitchFloatDock(BOOL bFloat, CWnd* pWndOwner)
{
	VERIFY(InitButton(m_wndDockFloat, bFloat ? ID_QUICKFIND_DOCK : ID_QUICKFIND_FLOAT, nullptr, g_szDockFloatBtnSize));
	if (bFloat)
		m_info.dwFlags |= QUICKFIND_INFO::FlagsFloat;
	else
		m_info.dwFlags &= ~QUICKFIND_INFO::FlagsFloat;

	// SetParent below triggers OnWindowPosChanging, so need to backup it for restoring
	CRect rect;
	GetWindowRect(rect);
	DWORD dwOldDockFlags = m_info.dwFlags & QUICKFIND_INFO::FlagsDockMask;
	SetNotifyOwner(pWndOwner);
	m_info.dwFlags &= ~QUICKFIND_INFO::FlagsDockMask;
	m_info.dwFlags |= dwOldDockFlags;

	UINT nFlags = SWP_NOZORDER | SWP_NOSIZE | SWP_NOOWNERZORDER;
	if (bFloat)
	{
		SetWindowPos(&wndTop, rect.left, rect.top, rect.Width(), rect.Height(), nFlags);
	}
	else
	{
		pWndOwner->ScreenToClient(rect);
		SetWindowPos(nullptr, rect.left, rect.top, -1, -1, nFlags);
		UpdateWindowPos();
	}
}

// Setting owner as the parent window has many problems, some edit controls
// (RichEdit, Scintilla) are not designed to work with child window properly:
// 1, RichEdit scrolls this window along with its text
// 2, Scintilla causes flashing/flickering of the caret in find edit control
// 3, Context menu shows on this control
// To work around this, create this window as a sibling window instead of a
// a child.

CWnd* CQuickFindWnd::GetDockParent(CWnd* pWndOwner) const
{
#ifdef _QF_USE_OWNER_AS_PARENT
	CWnd* pWndParent = pWndOwner;
#else
	CWnd* pWndParent = pWndOwner->GetParent();
#endif // _QF_USE_OWNER_AS_PARENT
	return pWndParent;
}

void CQuickFindWnd::SetNotifyOwner(CWnd* pWndOwner)
{
	ASSERT_VALID(pWndOwner);
	SetOwner(pWndOwner);
	if (m_info.IsFloating())
	{
		// SetParent below will hide this window, so check this style first
		// to avoid flickering, no need to do this if already a popup
		if (GetStyle() & WS_CHILD)
			SetParent(nullptr);
		ModifyStyle(WS_CHILD, WS_POPUP);
	}
	else
	{
		CWnd* pWndParent = GetDockParent(pWndOwner);
		pWndParent->ModifyStyle(0, WS_CLIPCHILDREN);
		CWnd* pWndSibling = pWndParent->GetWindow(GW_CHILD);
		while (pWndSibling)
		{
			pWndSibling->ModifyStyle(0, WS_CLIPSIBLINGS);
			pWndSibling = pWndSibling->GetNextWindow();
		}
		SetParent(pWndParent);
		ModifyStyle(WS_POPUP, WS_CHILD);
	}
}

BOOL CQuickFindWnd::SetScopeItems(const CStringArray& saItems, int nActiveIndex)
{
	if (!m_wndScope.GetSafeHwnd())
	{
		ASSERT(0);
		return FALSE;
	}
	m_wndScope.ResetContent();
	for (INT_PTR ii = 0; ii < saItems.GetSize(); ++ii)
	{
		m_wndScope.AddString(saItems[ii]);
	}
	if (!saItems.IsEmpty())
		m_wndScope.SetCurSel(nActiveIndex);
	return TRUE;
}

void CQuickFindWnd::InitFindActionMenu()
{
	//m_menuFindAction.LoadMenu(IDR_QUICKFIND_ACTION);
	//m_wndFindAction.m_hMenu = m_menuFindAction.GetSubMenu(0)->GetSafeHmenu();
	m_menuFindAction.CreatePopupMenu();

	UINT arrIDS[] = { ID_QUICKFIND_NEXT, ID_QUICKFIND_PREVIOUS, ID_QUICKFIND_ALL };
	for (auto nID : arrIDS)
	{
		CString strText;
		strText.LoadString(nID);
		m_menuFindAction.AppendMenu(MF_STRING, nID, strText);
	}

	m_wndFindAction.m_hMenu = m_menuFindAction.GetSafeHmenu();
	//m_wndFindAction.m_bOSMenu = FALSE;
}

static void _CenterAlignControls(CWnd* pParent, CWnd* arrWnds[], UINT nCount)
{
	CRect rectAlignWith;
	CWnd* pWndAlignWith = arrWnds[0];
	pWndAlignWith->GetWindowRect(rectAlignWith);
	pParent->ScreenToClient(rectAlignWith);
	UINT nFlags = SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOZORDER;
	for (UINT ii = 1; ii < nCount; ++ii)
	{
		CWnd* pWnd = arrWnds[ii];
		CRect rect;
		pWnd->GetWindowRect(rect);
		pParent->ScreenToClient(rect);
		auto nHeightDiff = rect.Height() - rectAlignWith.Height();
		if (nHeightDiff == 0)
		{
			if (rect.top != rectAlignWith.top)
			{
				pWnd->SetWindowPos(nullptr, rect.left, rectAlignWith.top, -1, -1, nFlags);
			}
		}
		else
		{
			rect.top = rectAlignWith.top - nHeightDiff / 2;
			pWnd->SetWindowPos(nullptr, rect.left, rect.top, -1, -1, nFlags);
		}
	}
}

BOOL CQuickFindWnd::OnInitDialog()
{
	CQuickFindWndBase::OnInitDialog();

	// WS_CLIPCHILDREN to avoid flickering
	ModifyStyle(0, WS_CLIPCHILDREN | WS_CLIPSIBLINGS);

	CRect rect;
	GetClientRect(rect);
	m_szMaxClientSize = rect.Size();
	m_szLastClientSize = m_szMaxClientSize;

	CRect rectDlg;
	GetWindowRect(rectDlg);
	m_szMaxDlgSize = rectDlg.Size();

	CWnd* arrWndRows[] = {&m_wndFind, &m_wndReplace, &m_wndScope};
	static_assert(_countof(m_arrUIRows) == _countof(arrWndRows), "UI design breaking change?");
	for (int nRow = 0; nRow < _countof(arrWndRows); ++nRow)
	{
		arrWndRows[nRow]->GetWindowRect(rect);
		ScreenToClient(rect);
		m_arrUIRows[nRow].top = rect.top;
		m_arrUIRows[nRow].bottom = rect.bottom;
	}

	auto resAccel = MAKEINTRESOURCE(IDR_QUICKFIND_ACCEL);
	auto hInstance = AfxFindResourceHandle(resAccel, RT_ACCELERATOR);
	m_hAccel = LoadAccelerators(hInstance, resAccel);
	ASSERT(m_hAccel != NULL);
	if (m_hAccel)
	{
		m_nAccelSize = ::CopyAcceleratorTable(m_hAccel, NULL, 0);

		m_pAccelTable = new ACCEL[m_nAccelSize];
		ENSURE(m_pAccelTable != NULL);

		::CopyAcceleratorTable(m_hAccel, m_pAccelTable, m_nAccelSize);
	}

	m_wndFindAction.m_bDefaultClick = TRUE;
	InitFindActionMenu();
	int nCurFindActionID = m_info.IsFindReplaceNext() ? ID_QUICKFIND_NEXT : ID_QUICKFIND_PREVIOUS;
	VERIFY(InitButton(m_wndFindAction, nCurFindActionID, hInstance));
	VERIFY(InitButton(m_wndClose, IDR_QUICKFIND_CLOSE, hInstance));
	VERIFY(InitButton(m_wndReplaceNext, ID_QUICKFIND_REPLACENEXT, hInstance));
	VERIFY(InitButton(m_wndReplaceAll, ID_QUICKFIND_REPLACEALL, hInstance));
	VERIFY(InitButton(m_wndMatchCase, ID_QUICKFIND_MATCHCASE, hInstance));
	VERIFY(InitButton(m_wndMatchWord, ID_QUICKFIND_MATCHWORD, hInstance));
	VERIFY(InitButton(m_wndRegEx, ID_QUICKFIND_REGEX, hInstance));	
	VERIFY(InitButton(m_wndDockFloat, ID_QUICKFIND_FLOAT, hInstance, g_szDockFloatBtnSize));
	rect.SetRect(1, 1, 1+g_szDockFloatBtnSize, 1+g_szDockFloatBtnSize);
	m_wndDockFloat.MoveWindow(rect);

	VERIFY(InitCombo(m_wndFind, m_info.saSearch));
	VERIFY(InitCombo(m_wndReplace, m_info.saReplace));
	CString strCue;
	strCue.LoadString(ID_QUICKFIND_REPLACEWITH);
	m_wndReplace.SetCueBanner(strCue);

	m_wndScope.ShowWindow(m_info.IsShowScope() ? SW_SHOWNORMAL : SW_HIDE);
	
	SwitchUI(m_info.IsInitShowAsReplace(), m_info.IsInitShowOptions());

	// fix the minor alignment issue
	CWnd* arrWndAlignFind[] = { &m_wndFind, &m_wndFindAction, &m_wndClose };
	_CenterAlignControls(this, arrWndAlignFind, _countof(arrWndAlignFind));

	return TRUE;  // return TRUE  unless you set the focus to a control
}

CString CQuickFindWnd::GetFindString() const
{
	CString strText;
	int nCurSel = m_wndFind.GetCurSel();
	if (nCurSel >= 0)
		m_wndFind.GetLBText(nCurSel, strText);
	else
		m_wndFind.GetWindowText(strText);
	return strText;
}

CString CQuickFindWnd::GetReplaceString() const
{
	CString strText;
	int nCurSel = m_wndReplace.GetCurSel();
	if (nCurSel >= 0)
		m_wndReplace.GetLBText(nCurSel, strText);
	else
		m_wndReplace.GetWindowText(strText);
	return strText;
}

void CQuickFindWnd::SetActiveShowWindow()
{
	m_wndMatchCase.SetCheck(IsMatchCase());
	m_wndMatchWord.SetCheck(IsMatchWholeWord());
	m_wndRegEx.SetCheck(IsUseRegEx());
	SetActiveWindow();
	UpdateWindowPos();
	m_wndFind.SetFocus();
	ShowWindow(SW_SHOW);
}

enum {
	QuickFindMinTrackWidth = 200,
	QuickFindSizeAndFocusIndicatorHeight = 2,
};

void CQuickFindWnd::UpdateWindowPos()
{
	if (m_info.IsFloating())
		return;
	// be aware that the owner and parent can be different
	auto pWndOwner = GetOwner();
	auto pWndParent = GetParent();
	ASSERT_VALID(pWndOwner);
	ASSERT_VALID(pWndParent);
	// owner window is the targeted restrain window
	CRect rectOwner;
	pWndOwner->GetClientRect(rectOwner);
	CRect rectDlg;
	GetWindowRect(rectDlg);	
	pWndOwner->ScreenToClient(rectDlg);
	// Don't send WM_WINDOWPOSCHANGING to avoid auto snapping to parent during this procedure.
	if (m_info.IsDocked())
	{
		// left vs right, top vs bottom, they should be mutually exclusive.
		CSize szDlg = rectDlg.Size();
		if (m_info.IsDockLeft())
		{
			rectDlg.left = rectOwner.left;
		}
		else if (m_info.IsDockRight())
		{
			rectDlg.left = max(rectOwner.right - szDlg.cx, rectOwner.left);
			szDlg.cx = max(rectOwner.right - rectDlg.left, QuickFindMinTrackWidth);
		}
		if (m_info.IsDockTop())
		{
			rectDlg.top = rectOwner.top;
		}
		else if (m_info.IsDockBottom())
		{
			rectDlg.top = rectOwner.bottom - szDlg.cy;
		}
		rectDlg.right = rectDlg.left + szDlg.cx;
		rectDlg.bottom = rectDlg.top + szDlg.cy;
		// restrain in owner client
		if (rectDlg.left < rectOwner.left)
		{
			rectDlg.left = rectOwner.left;
			rectDlg.right = rectDlg.left + szDlg.cx;
		}
		if (rectDlg.top < rectOwner.top)
		{
			rectDlg.top = rectOwner.top;
			rectDlg.bottom = rectDlg.top + szDlg.cy;
		}
		if (rectDlg.right > rectOwner.right)
		{
			rectDlg.right = rectOwner.right;
			rectDlg.left = rectDlg.right - szDlg.cx;
		}
		if (rectDlg.bottom > rectOwner.bottom)
		{
			rectDlg.bottom = rectOwner.bottom;
			rectDlg.top = rectDlg.bottom - szDlg.cy;
		}

		if (pWndOwner->GetSafeHwnd() != pWndParent->GetSafeHwnd())
		{
			pWndOwner->MapWindowPoints(pWndParent, rectDlg);
		}
		UINT nFlags = SWP_NOSENDCHANGING | SWP_NOZORDER | SWP_NOACTIVATE;
		SetWindowPos(nullptr, rectDlg.left, rectDlg.top, szDlg.cx, szDlg.cy, nFlags);
	}
	else
	{
		// in case the window was moved outside the client area, move it to proper place		
		CSize szDiff(rectDlg.right - rectOwner.right, rectDlg.bottom - rectOwner.bottom);
		if (szDiff.cx > 0)
			rectDlg.left = max(rectDlg.left - szDiff.cx, rectOwner.left);
		if (szDiff.cy > 0)
			rectDlg.top = max(rectDlg.top - szDiff.cy, rectOwner.top);
		if (szDiff.cx > 0 || szDiff.cy > 0)
		{
			if (pWndOwner->GetSafeHwnd() != pWndParent->GetSafeHwnd())
			{
				pWndOwner->MapWindowPoints(pWndParent, rectDlg);
			}
			UINT nFlags = SWP_NOSIZE | SWP_NOSENDCHANGING | SWP_NOZORDER | SWP_NOACTIVATE;
			SetWindowPos(nullptr, rectDlg.left, rectDlg.top, -1, -1, nFlags);
		}
	}
}

static void _GetComboStringArray(const CComboBox& combo, CStringArray& sa, int nMaxCount)
{
	nMaxCount = min(nMaxCount, combo.GetCount());
	sa.SetSize(nMaxCount);
	for (int ii = 0; ii < nMaxCount; ++ii)
	{
		combo.GetLBText(ii, sa[ii]);
	}
}

void CQuickFindWnd::GetFindStringArray(CStringArray& sa) const
{
	_GetComboStringArray(m_wndFind, sa, (int)m_info.nMaxItems);
}

void CQuickFindWnd::GetReplaceStringArray(CStringArray& sa) const
{
	_GetComboStringArray(m_wndReplace, sa, (int)m_info.nMaxItems);
}

BOOL CQuickFindWnd::OnEraseBkgnd(CDC* /*pDC*/)
{
	return TRUE;
}

void CQuickFindWnd::OnPaint()
{
	CPaintDC dc(this);
	CMemDC memDC(dc, this);
	CDC* pDC = &memDC.GetDC();
	CRect rectClient, rectGripper;
	GetClientRect(rectClient);
	pDC->FillSolidRect(rectClient, GetSysColor(COLOR_3DFACE));

	GetSizeGripperRect(rectGripper);
	int nGripCY = GetSystemMetrics(SM_CYVSCROLL);
	rectGripper.bottom -= QuickFindSizeAndFocusIndicatorHeight;
	rectGripper.top = rectGripper.bottom - nGripCY;
	HTHEME ht = OpenThemeData(m_hWnd, L"SCROLLBAR");
	if (ht)
	{
		DrawThemeBackground(ht, pDC->GetSafeHdc(), SBP_SIZEBOX, SZB_HALFBOTTOMLEFTALIGN, rectGripper, nullptr);
		CloseThemeData(ht);
	}

	GetMoveGripperRect(rectGripper);
	int nBoxSize = 4;
	bool bHorz = true;
	const int nBoxesNumber = bHorz ? (rectGripper.Height() - nBoxSize) / nBoxSize : (rectGripper.Width() - nBoxSize) / nBoxSize;
	int nOffset = bHorz ? (rectGripper.Height() - nBoxesNumber * nBoxSize) / 2 : (rectGripper.Width() - nBoxesNumber * nBoxSize) / 2;

	for (int nBox = 0; nBox < nBoxesNumber; nBox++)
	{
		int x = bHorz ? rectGripper.left : rectGripper.left + nOffset;
		int y = bHorz ? rectGripper.top + nOffset : rectGripper.top;

		pDC->FillSolidRect(x + 1, y + 1, nBoxSize / 2, nBoxSize / 2, GetGlobalData()->clrBtnHilite);
		pDC->FillSolidRect(x, y, nBoxSize / 2, nBoxSize / 2, RGB(0xa0,0xa0,0xa0));

		nOffset += nBoxSize;
	}

	rectGripper = rectClient;
	rectGripper.top = rectGripper.bottom - QuickFindSizeAndFocusIndicatorHeight;
	COLORREF crfActiveIndicate;
	if (m_bActive)
	{
		if (m_info.IsFloating())
			crfActiveIndicate = RGB(34, 177, 76);
		else if (m_info.IsDocked())
			crfActiveIndicate = RGB(0, 122, 204);
		else
			crfActiveIndicate = RGB(255, 127, 39);
	}
	else
		crfActiveIndicate = RGB(204, 206, 219);
	pDC->FillSolidRect(rectGripper, crfActiveIndicate);

	if (m_nDockTimerID && m_info.IsDocked())
	{
		CPoint ptOld = pDC->GetCurrentPosition();
		CPen pen(PS_SOLID, 1, RGB(255, 0, 128));
		auto pOldPen = pDC->SelectObject(&pen);
		if (m_info.IsDockTop())
		{
			pDC->MoveTo(rectClient.left, rectClient.top);
			pDC->LineTo(rectClient.right, rectClient.top);
		}
		if (m_info.IsDockBottom())
		{
			pDC->MoveTo(rectClient.left, rectClient.bottom-1);
			pDC->LineTo(rectClient.right, rectClient.bottom-1);
		}
		if (m_info.IsDockLeft())
		{
			pDC->MoveTo(rectClient.left, rectClient.top);
			pDC->LineTo(rectClient.left, rectClient.bottom);
		}
		if (m_info.IsDockRight())
		{
			pDC->MoveTo(rectClient.right - 1, rectClient.top);
			pDC->LineTo(rectClient.right - 1, rectClient.bottom);
		}
		pDC->SelectObject(pOldPen);
		pDC->MoveTo(ptOld);
	}
}

BOOL CQuickFindWnd::GetMoveGripperRect(CRect& rectGripper) const
{
	GetClientRect(rectGripper);
	rectGripper.left = rectGripper.right - 6;
	rectGripper.top += 5;
	rectGripper.bottom -= 5;
	return TRUE;
}

BOOL CQuickFindWnd::GetSizeGripperRect(CRect& rectGripper) const
{
	int nGripCX = GetSystemMetrics(SM_CXHSCROLL);
	GetClientRect(rectGripper);
	rectGripper.right = rectGripper.left + nGripCX;
	return TRUE;
}

CSize CQuickFindWnd::CalcWindowSizeFromClient(CSize szClient) const
{
	szClient.cx += m_szMaxDlgSize.cx - m_szMaxClientSize.cx;
	szClient.cy += m_szMaxDlgSize.cy - m_szMaxClientSize.cy;
	return szClient;
}

LRESULT CQuickFindWnd::OnNcHitTest(CPoint point)
{
 	CPoint ptHitClient = point;
 	ScreenToClient(&ptHitClient);
	CRect rect;
	if ( GetSizeGripperRect(rect) && rect.PtInRect(ptHitClient) )
	{
		int nGripCY = GetSystemMetrics(SM_CYVSCROLL);
		if (ptHitClient.y >= rect.bottom - nGripCY)
			return HTBOTTOMLEFT;
		return HTLEFT;
	}
	else if ( GetMoveGripperRect(rect) && rect.PtInRect(ptHitClient) )
	{
		return HTCAPTION;
	}
	else
	{
		CRect rcClient;
		GetClientRect(rcClient);
		if (ptHitClient.y >= rcClient.bottom - 5)
			return HTBOTTOM;
	}
	return CQuickFindWndBase::OnNcHitTest(point);
}

BOOL CQuickFindWnd::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	CPoint ptCursor;
	::GetCursorPos(&ptCursor);

	CPoint ptCursorClient = ptCursor;
	ScreenToClient(&ptCursorClient);

	CRect rectGripper;
	GetMoveGripperRect(rectGripper);

	if (rectGripper.PtInRect(ptCursorClient))
	{
		SetCursor(GetGlobalData()->m_hcurSizeAll);
		return TRUE;
	}
	return CQuickFindWndBase::OnSetCursor(pWnd, nHitTest, message);
}

void CQuickFindWnd::OnSizing(UINT fwSide, LPRECT pRect)
{
	CQuickFindWndBase::OnSizing(fwSide, pRect);

	if (fwSide == WMSZ_LEFT)
		return;

	CRect rectOldWnd;
	GetWindowRect(rectOldWnd);

	CRect rect = *pRect;
	int nYDelta = rect.Height() - rectOldWnd.Height();
	if (nYDelta == 0)
		return;
	
	// snap to the last row of controls
	ScreenToClient(rect);
	for (int nRow = _countof(m_arrUIRows)-1; nRow >= 0; --nRow)
	{
		if (rect.bottom >= m_arrUIRows[nRow].bottom)
		{
			BOOL bShowReplaceUI = FALSE;
			BOOL bShowOptionsUI = FALSE;
			switch (nRow)
			{
			case 0:
				break;
			case 1:
				bShowReplaceUI = m_info.IsInitShowAsReplace();
				bShowOptionsUI = !bShowReplaceUI;
				break;
			case 2:
				bShowReplaceUI = bShowOptionsUI = TRUE;
				break;
			}
			bShowReplaceUI = bShowReplaceUI && m_bCanShowReplaceUI;
			nRow = (bShowReplaceUI ? 1 : 0) + (bShowOptionsUI ? 1 : 0);
			int nNewClientHeight = m_arrUIRows[nRow].bottom + m_arrUIRows[0].top + 1;
			pRect->bottom = pRect->top + CalcWindowSizeFromClient(CSize(0, nNewClientHeight)).cy;

			if (bShowReplaceUI ^ IsShowReplaceUI() || bShowOptionsUI ^ IsShowOptionsUI())
			{
				SwitchUI(bShowReplaceUI, bShowOptionsUI);
			}
			return;
		}
	}
}

void CQuickFindWnd::ShowReplaceUI(BOOL bShow)
{
	CWnd* arrWnds[] = { &m_wndReplace, &m_wndReplaceNext, &m_wndReplaceAll };
	for (auto pWnd : arrWnds)
	{
		pWnd->ShowWindow(bShow);
	}
}

void CQuickFindWnd::ShowOptionsUI(BOOL bShow)
{
	CWnd* arrWnds[] ={ &m_wndScope, &m_wndMatchCase, &m_wndMatchWord, &m_wndRegEx };
	if (bShow)
	{
		CRect rect;
		m_wndScope.GetWindowRect(rect);
		ScreenToClient(rect);
		rect.top = IsShowReplaceUI() ? m_arrUIRows[2].top : m_arrUIRows[1].top;
		m_wndScope.SetWindowPos(nullptr, rect.left, rect.top, -1, -1, SWP_NOSIZE|SWP_NOZORDER);
		_CenterAlignControls(this, arrWnds, _countof(arrWnds));
	}
	for (auto pWnd : arrWnds)
	{
		pWnd->ShowWindow(bShow ? SW_SHOWNA : SW_HIDE);
	}
}

void CQuickFindWnd::OnSize(UINT nType, int cx, int cy)
{
	CQuickFindWndBase::OnSize(nType, cx, cy);

	m_bActive = TRUE;
	CRect rect;
	Invalidate();

	if (!m_wndFind.GetSafeHwnd())
		return;

	CSize szDlgNewSize(cx, cy);
	CSize szDiff = szDlgNewSize - m_szLastClientSize;

	CWnd* arrWndResizeWidth[] = {&m_wndFind, &m_wndReplace, &m_wndScope};
	CWnd* arrWndAnchorRight[] = { &m_wndFindAction, &m_wndClose, &m_wndReplaceNext, &m_wndReplaceAll };

	HDWP hdwp = BeginDeferWindowPos(_countof(arrWndResizeWidth) + _countof(arrWndAnchorRight));
	if (!hdwp)
	{
		ASSERT(0);
	}
	for (auto pResizeWnd : arrWndResizeWidth)
	{
		pResizeWnd->GetWindowRect(rect);
		ScreenToClient(rect);
		rect.right += szDiff.cx;
		HWND hWnd = pResizeWnd->GetSafeHwnd();
		UINT nFlags = SWP_NOMOVE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE;
		if (hdwp)
			hdwp = DeferWindowPos(hdwp, hWnd, NULL, -1, -1, rect.Width(), rect.Height(), nFlags);
		else
			pResizeWnd->SetWindowPos(NULL, -1, -1, rect.Width(), rect.Height(), nFlags);
	}	
	for (auto pAnchorWnd : arrWndAnchorRight)
	{
		pAnchorWnd->GetWindowRect(rect);
		ScreenToClient(rect);
		rect.OffsetRect(szDiff.cx, 0);
		HWND hWnd = pAnchorWnd->GetSafeHwnd();
		UINT nFlags = SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE;
		if (hdwp)
			hdwp = DeferWindowPos(hdwp, hWnd, NULL, rect.left, rect.top, -1, -1, nFlags);
		else
			pAnchorWnd->SetWindowPos(NULL, rect.left, rect.top, -1, -1, nFlags);
	}
	if (hdwp)
		EndDeferWindowPos(hdwp);

	m_szLastClientSize = szDlgNewSize;
}

void CQuickFindWnd::OnMoving(UINT nSide, LPRECT lpRect)
{
	CQuickFindWndBase::OnMoving(nSide, lpRect);
	if (!m_bActive)
	{
		m_bActive = TRUE;
		Invalidate();
	}
	if (m_info.IsFloating())
		return;
	auto pWndOwner = GetOwner();
	if (!pWndOwner->GetSafeHwnd())
		return;

	CRect rectNew = *lpRect;
	CRect rectOldWnd;
	GetWindowRect(rectOldWnd);
	if (rectOldWnd == rectNew)
		return;

	DWORD dwOldDock = m_info.dwFlags & QUICKFIND_INFO::FlagsDockMask;

	// restrain in owner client
	CSize szNew = rectNew.Size();
	CRect rectOwner;
	pWndOwner->GetClientRect(rectOwner);
	pWndOwner->ClientToScreen(rectOwner);
	if (rectNew.left < rectOwner.left)
	{
		rectNew.left = rectOwner.left;
		rectNew.right = rectNew.left + szNew.cx;
		m_info.dwFlags |= QUICKFIND_INFO::FlagsDockLeft;
	}
	if (rectNew.top < rectOwner.top)
	{
		rectNew.top = rectOwner.top;
		rectNew.bottom = rectNew.top + szNew.cy;
		m_info.dwFlags |= QUICKFIND_INFO::FlagsDockTop;
	}
	if (rectNew.right > rectOwner.right)
	{
		rectNew.right = rectOwner.right;
		rectNew.left = rectNew.right - szNew.cx;
		m_info.dwFlags |= QUICKFIND_INFO::FlagsDockRight;
	}
	if (rectNew.bottom > rectOwner.bottom)
	{
		rectNew.bottom = rectOwner.bottom;
		rectNew.top = rectNew.bottom - szNew.cy;
		m_info.dwFlags |= QUICKFIND_INFO::FlagsDockBottom;
	}
	*lpRect = rectNew;

	if (m_info.IsDocked())
	{
		if (rectNew.left != rectOwner.left)
			m_info.dwFlags &= ~QUICKFIND_INFO::FlagsDockLeft;
		if (rectNew.top != rectOwner.top)
			m_info.dwFlags &= ~QUICKFIND_INFO::FlagsDockTop;
		if (rectNew.right != rectOwner.right)
			m_info.dwFlags &= ~QUICKFIND_INFO::FlagsDockRight;
		if (rectNew.bottom != rectOwner.bottom)
			m_info.dwFlags &= ~QUICKFIND_INFO::FlagsDockBottom;
	}
	DWORD dwNewDock = m_info.dwFlags & QUICKFIND_INFO::FlagsDockMask;
	if (dwOldDock != dwNewDock)
	{
		if (dwNewDock)
			m_nDockTimerID = SetTimer(QFTIMER_ID_DOCK_EVENT, QFTIMER_DOCK_TIMEOUT, nullptr);
		else if (m_nDockTimerID)
		{
			KillTimer(m_nDockTimerID);
			m_nDockTimerID = 0;
		}
		Invalidate();
	}
}

void CQuickFindWnd::OnWindowPosChanging(WINDOWPOS* lpwndpos)
{
	CQuickFindWndBase::OnWindowPosChanging(lpwndpos);
	if (m_info.IsFloating())
		return;
	if ((lpwndpos->flags & SWP_NOMOVE) == 0 && !m_info.IsDocked())
	{
		// snap to owner window
		auto pWndOwner = GetOwner();
		if (!pWndOwner->GetSafeHwnd())
			return;
		auto pWndParent = GetParent();
		DWORD dwOldDock = m_info.dwFlags & QUICKFIND_INFO::FlagsDockMask;
		enum {SnapSize = 15};
		CRect rectOwner;
		pWndOwner->GetClientRect(rectOwner);
		pWndOwner->MapWindowPoints(pWndParent, rectOwner);
		CRect rect;
		if (lpwndpos->flags & SWP_NOSIZE)
		{
			CRect rectWnd;
			GetWindowRect(&rectWnd);
			rect.SetRect(lpwndpos->x, lpwndpos->y, lpwndpos->x + rectWnd.Width(), lpwndpos->y + rectWnd.Height());
		}
		else
		{
			rect.SetRect(lpwndpos->x, lpwndpos->y, lpwndpos->x + lpwndpos->cx, lpwndpos->y + lpwndpos->cy);
		}
		if (rect.left - rectOwner.left < SnapSize)
		{
			lpwndpos->x = rectOwner.left;
			lpwndpos->cx = rect.Width();
			m_info.dwFlags |= QUICKFIND_INFO::FlagsDockLeft;
		}
		else if (rectOwner.right - rect.right < SnapSize)
		{
			lpwndpos->x = rectOwner.right - rect.Width();
			lpwndpos->cx = rect.Width();
			m_info.dwFlags |= QUICKFIND_INFO::FlagsDockRight;
		}
		if (rect.top - rectOwner.top < SnapSize)
		{
			lpwndpos->y = rectOwner.top;
			lpwndpos->cy = rect.Height();
			m_info.dwFlags |= QUICKFIND_INFO::FlagsDockTop;
		}
		else if (rectOwner.bottom - rect.bottom < SnapSize)
		{
			lpwndpos->y = rectOwner.bottom - rect.Height();
			lpwndpos->cy = rect.Height();
			m_info.dwFlags |= QUICKFIND_INFO::FlagsDockBottom;
		}
		DWORD dwNewDock = m_info.dwFlags & QUICKFIND_INFO::FlagsDockMask;
		if (dwOldDock != dwNewDock)
		{
			if (dwNewDock)
				m_nDockTimerID = SetTimer(QFTIMER_ID_DOCK_EVENT, QFTIMER_DOCK_TIMEOUT, nullptr);
			else if (m_nDockTimerID)
			{
				KillTimer(m_nDockTimerID);
				m_nDockTimerID = 0;
			}
			Invalidate();
		}
	}
}

void CQuickFindWnd::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	CQuickFindWndBase::OnGetMinMaxInfo(lpMMI);
	lpMMI->ptMinTrackSize.x = QuickFindMinTrackWidth;
	int nMaxClientHeight = m_arrUIRows[0].top + m_arrUIRows[0].bottom + 1;
	lpMMI->ptMinTrackSize.y = CalcWindowSizeFromClient(CSize(0, nMaxClientHeight)).cy;
	if (m_szMaxDlgSize.cy)
		lpMMI->ptMaxTrackSize.y = m_szMaxDlgSize.cy;
}

BOOL CQuickFindWnd::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message >= WM_KEYFIRST && pMsg->message <= WM_KEYLAST)
	{
		if (m_hAccel != NULL)
		{
			if (::TranslateAccelerator(m_hWnd, m_hAccel, pMsg))
				return TRUE;
		}
	}

	return CQuickFindWndBase::PreTranslateMessage(pMsg);
}

void CQuickFindWnd::OnButtonFindAction()
{
	BOOL bChangeButton = FALSE;
	int nCmd = m_wndFindAction.m_nMenuResult;
	if (!nCmd)
		nCmd = m_info.IsFindReplaceNext() ? ID_QUICKFIND_NEXT : ID_QUICKFIND_PREVIOUS;
	switch (nCmd)
	{
	case ID_QUICKFIND_NEXT:
		bChangeButton = m_info.IsFindReplacePrevious();
		m_info.dwFlags &= ~QUICKFIND_INFO::FlagsFindReplacePrevious;
		OnFindNext();
		break;
	case ID_QUICKFIND_PREVIOUS:
		bChangeButton = m_info.IsFindReplaceNext();
		m_info.dwFlags |= QUICKFIND_INFO::FlagsFindReplacePrevious;
		OnFindPrevious();
		break;
	case ID_QUICKFIND_ALL:
		m_wndFindAction.m_nMenuResult = 0;
		OnFindAll();
		break;
	}
	if (bChangeButton)
		VERIFY(InitButton(m_wndFindAction, m_wndFindAction.m_nMenuResult));
}

void CQuickFindWnd::OnMatchCase()
{
	if (m_info.IsMatchCase())
		m_info.dwFlags &= ~QUICKFIND_INFO::FlagsMatchCase;
	else
		m_info.dwFlags |= QUICKFIND_INFO::FlagsMatchCase;
	BOOL bBtnChecked = m_wndMatchCase.GetCheck() != BST_UNCHECKED;
	if (m_info.IsMatchCase() != bBtnChecked)
		m_wndMatchCase.SetCheck(m_info.IsMatchCase());
	UpdateFindResult( NotifyOwner(QuickFindCmdOptionsChange) != 0 );
}

void CQuickFindWnd::OnMatchWord()
{
	if (m_info.IsMatchWholeWord())
		m_info.dwFlags &= ~QUICKFIND_INFO::FlagsMatchWord;
	else
		m_info.dwFlags |= QUICKFIND_INFO::FlagsMatchWord;
	BOOL bBtnChecked = m_wndMatchWord.GetCheck() != BST_UNCHECKED;
	if (m_info.IsMatchWholeWord() != bBtnChecked)
		m_wndMatchWord.SetCheck(m_info.IsMatchWholeWord());
	UpdateFindResult( NotifyOwner(QuickFindCmdOptionsChange) != 0 );
}

void CQuickFindWnd::OnUseRegEx()
{
	if (m_info.IsUseRegEx())
		m_info.dwFlags &= ~QUICKFIND_INFO::FlagsUseRegEx;
	else
		m_info.dwFlags |= QUICKFIND_INFO::FlagsUseRegEx;
	BOOL bBtnChecked = m_wndRegEx.GetCheck() != BST_UNCHECKED;
	if (m_info.IsUseRegEx() != bBtnChecked)
		m_wndRegEx.SetCheck(m_info.IsUseRegEx());
	UpdateFindResult( NotifyOwner(QuickFindCmdOptionsChange) != 0 );
}

void CQuickFindWnd::OnSelChangeFind()
{
	if (m_info.IsNotifyFindTextChange())
	{
		UpdateFindResult( NotifyOwner(QuickFindCmdFindTextChange) != 0 );
	}
}

void CQuickFindWnd::OnEditChangeFind()
{
	if (m_info.IsNotifyFindTextChange())
	{
		UpdateFindResult( NotifyOwner(QuickFindCmdFindTextChange) != 0 );
	}
}

void CQuickFindWnd::OnSelChangeScope()
{
	UpdateFindResult( NotifyOwner(QuickFindCmdScopeSelChange, (WPARAM)m_wndScope.GetCurSel()) != 0 );
}

static void _PromoteTextInComboEx(CComboBox& combo, LPCTSTR pszText, int nMaxCount)
{
	ASSERT(combo.m_hWnd);
	// FindStringExact is not case sensitive
	//int nIndex = combo.FindStringExact(-1, pszText);
	int nIndex = -1, nCount = combo.GetCount();
	for (int ii = 0; ii < nCount; ii++)
	{
		CString strTemp;
		combo.GetLBText(ii, strTemp);
		if (!strTemp.Compare(pszText))
		{
			nIndex = ii;
			break;
		}
	}
	if (nIndex < 0)
	{
		combo.InsertString(0, pszText);
		if (combo.GetCount() > nMaxCount)
		{
			combo.DeleteString(combo.GetCount() - 1);
		}
	}
	else if (nIndex > 0)
	{
		combo.DeleteString(nIndex);
		combo.InsertString(0, pszText);
	}
	combo.SetCurSel(0);
}

static void _PromoteCurTextInComboList(CComboBox& combo, int nMaxCount)
{
	// they have to be in sync!
	int nCurSel = combo.GetCurSel();
	if (nCurSel >= 0)
	{
		// no need to promote if already the first one
		if (nCurSel > 0)
		{
			CString strItem;
			combo.GetLBText(nCurSel, strItem);
			combo.DeleteString(nCurSel);
			combo.InsertString(0, strItem);
			combo.SetCurSel(0);
		}
	}
	else if (combo.GetWindowTextLength())
	{
		CString strText;
		combo.GetWindowText(strText);
		_PromoteTextInComboEx(combo, strText, nMaxCount);
	}
}

void CQuickFindWnd::SetFindString(LPCTSTR pszText)
{
	if (pszText && *pszText)
		_PromoteTextInComboEx(m_wndFind, pszText, (int)m_info.nMaxItems);
	else
		m_wndFind.SetWindowText(_T(""));
}

void CQuickFindWnd::PromoteFindTextItems()
{
	_PromoteCurTextInComboList(m_wndFind, (int)m_info.nMaxItems);
}

void CQuickFindWnd::PromoteReplaceTextItems()
{
	_PromoteCurTextInComboList(m_wndReplace, (int)m_info.nMaxItems);
}

BOOL CQuickFindWnd::IsShowReplaceUI() const
{
	return m_bCanShowReplaceUI && m_bShowReplaceUI;
}

BOOL CQuickFindWnd::IsShowOptionsUI() const
{
	return m_bShowOptionsUI;
}

void CQuickFindWnd::OnFindNext()
{
	PromoteFindTextItems();
	BOOL bWasFindPrevious = m_info.IsFindReplacePrevious();
	m_info.dwFlags &= ~QUICKFIND_INFO::FlagsFindReplacePrevious;
	UpdateFindResult( NotifyOwner(QuickFindCmdFind) != 0 );
	if (bWasFindPrevious)
		m_info.dwFlags |= QUICKFIND_INFO::FlagsFindReplacePrevious;
}

void CQuickFindWnd::OnFindPrevious()
{
	PromoteFindTextItems();
	BOOL bWasFindNext = m_info.IsFindReplaceNext();
	m_info.dwFlags |= QUICKFIND_INFO::FlagsFindReplacePrevious;
	UpdateFindResult( NotifyOwner(QuickFindCmdFind) != 0 );
	if (bWasFindNext)
		m_info.dwFlags &= ~QUICKFIND_INFO::FlagsFindReplacePrevious;
}

void CQuickFindWnd::OnFindAll()
{
	PromoteFindTextItems();
	UpdateFindResult( NotifyOwner(QuickFindCmdFindAll) != 0 );
}

void CQuickFindWnd::OnReplaceNext()
{
	PromoteReplaceTextItems();
	UpdateFindResult( NotifyOwner(QuickFindCmdReplace) != 0 );
}

void CQuickFindWnd::OnReplaceAll()
{
	PromoteReplaceTextItems();
	UpdateFindResult( NotifyOwner(QuickFindCmdReplaceAll) != 0 );
}

void CQuickFindWnd::SwitchUI(BOOL bShowAsReplace, BOOL bShowOptions)
{
	m_bShowReplaceUI = bShowAsReplace;
	m_bShowOptionsUI = bShowOptions;
	CRect rectDlg;
	GetWindowRect(rectDlg);
	CSize szDlg = rectDlg.Size();
	int nRows = (IsShowOptionsUI() ? 1 : 0) + (IsShowReplaceUI() ? 1 : 0);
	LONG nNewClientHeight = m_arrUIRows[nRows].bottom + m_arrUIRows[0].top + 1;
	szDlg.cy = CalcWindowSizeFromClient(CSize(0, nNewClientHeight)).cy;

	ShowReplaceUI(IsShowReplaceUI());
	ShowOptionsUI(IsShowOptionsUI());
	SetWindowPos(nullptr, -1, -1, szDlg.cx, szDlg.cy, SWP_NOMOVE | SWP_NOZORDER);
}

LRESULT CQuickFindWnd::NotifyOwner(QuickFindCmd cmd, WPARAM wp, LPARAM lp)
{
	auto pWndOwner = GetOwner();
	if (pWndOwner->GetSafeHwnd())
	{
		QNMHDR nmhdr = {this, wp, lp};
		return pWndOwner->SendMessage(_QUICKFINDMSG, (WPARAM)cmd, (LPARAM)&nmhdr);
	}
	return 0;
}

void CQuickFindWnd::UpdateFindResult(BOOL bFindOK)
{
	m_wndFind.m_bSearchOK = bFindOK;
	m_wndFind.RedrawWindow(NULL, NULL, RDW_FRAME | RDW_INVALIDATE);
}

void CQuickFindWnd::OnEditFind()
{
	m_info.dwFlags &= ~QUICKFIND_INFO::FlagsInitShowReplace;
	SwitchUI(FALSE, TRUE);
	m_wndFind.SetFocus();
}

void CQuickFindWnd::OnEditReplace()
{
	m_info.dwFlags |= QUICKFIND_INFO::FlagsInitShowReplace;
	SwitchUI(TRUE, TRUE);
	m_wndFind.SetFocus();
}

void CQuickFindWnd::OnDestroy()
{
	KillTimer(m_nDockTimerID);
	m_nDockTimerID = 0;
	NotifyOwner(QuickFindCmdTerminating);
	CQuickFindWndBase::OnDestroy();
}

LRESULT CQuickFindWnd::OnIdleUpdateCmdUI(WPARAM /*wParam*/, LPARAM)
{
	if ((GetStyle() & WS_VISIBLE))
	{
		CWnd* pFocus = GetFocus();
		BOOL bActiveOld = m_bActive;

		m_bActive = (pFocus->GetSafeHwnd() != NULL && (IsChild(pFocus) || pFocus->GetSafeHwnd() == GetSafeHwnd()));

		if (m_bActive != bActiveOld)
		{
			OnUpdateActiveStatus();
		}
	}
	return 0L;
}

void CQuickFindWnd::OnUpdateActiveStatus()
{
	Invalidate();
}

void CQuickFindWnd::OnTimer(UINT_PTR nIDEvent)
{
	switch (nIDEvent)
	{
	case QFTIMER_ID_DOCK_EVENT:
		KillTimer(m_nDockTimerID);
		m_nDockTimerID = 0;
		Invalidate();
		break;
	}
	CQuickFindWndBase::OnTimer(nIDEvent);
}

void CQuickFindWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
	SetFocus();
	CQuickFindWndBase::OnLButtonDown(nFlags, point);
}

void CQuickFindWnd::OnRButtonDown(UINT nFlags, CPoint point)
{
	SetFocus();
	CQuickFindWndBase::OnRButtonDown(nFlags, point);
}

