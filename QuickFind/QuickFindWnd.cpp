// QuickFindWnd.cpp : implementation file
//

#include "stdafx.h"
#include "QuickFindWnd.h"
#include "afxdialogex.h"
#include "Resource.h"

// CQuickFindWnd dialog

IMPLEMENT_DYNAMIC(CQuickFindWnd, CQuickFindWndBase)

CQuickFindWnd::CQuickFindWnd()
	: CQuickFindWndBase(IDD_QUICK_FIND_REPLACE)
{
	m_szLastClientSize.SetSize(0, 0);
	m_nLastDlgHeight = 0;
	m_bShowReplaceUI = FALSE;
	m_bShowOptionsUI = TRUE;
	ZeroMemory(&m_arrUIRows, sizeof(m_arrUIRows));
	m_hAccel = nullptr;
	m_pAccelTable = nullptr;
	m_nAccelSize = 0;
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
}

static const UINT _QUICKFINDMSG = ::RegisterWindowMessage(QUICKFINDMSGSTRING);

BEGIN_MESSAGE_MAP(CQuickFindWnd, CQuickFindWndBase)
	ON_WM_NCDESTROY()
	//ON_WM_ERASEBKGND()
	//ON_WM_PAINT()
	ON_WM_NCHITTEST()
	ON_WM_SETCURSOR()
	ON_WM_SIZING()
	ON_WM_SIZE()
	ON_WM_MOVING()
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
END_MESSAGE_MAP()


// CQuickFindWnd message handlers

BOOL CQuickFindWnd::FindAccelerator(UINT uiCmd, CString& str) const
{
	ASSERT(m_pAccelTable);
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

BOOL CQuickFindWnd::InitButton(CMFCButton& btn, UINT nID, HINSTANCE hResInst) const
{
	auto resName = MAKEINTRESOURCE(nID);
	if (!hResInst)
	{
		hResInst = AfxFindResourceHandle(resName, RT_ICON);
		if (!hResInst)
		{
			return FALSE;
		}
	}
	btn.m_nFlatStyle = CMFCButton::BUTTONSTYLE_FLAT;
	btn.m_bTransparent = TRUE;
	btn.SetWindowText(_T(""));

	auto hIcon = (HICON)LoadImage(hResInst, resName, IMAGE_ICON, 16, 16, LR_SHARED);
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
	BOOL bRet = CQuickFindWndBase::Create(IDD_QUICK_FIND_REPLACE, pParentWnd);
	if (!bRet)
		return FALSE;
	return TRUE;
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
	m_wndFindAction.m_bOSMenu = FALSE;
}

BOOL CQuickFindWnd::OnInitDialog()
{
	CQuickFindWndBase::OnInitDialog();
	CRect rect;
	GetClientRect(rect);
	m_szLastClientSize = rect.Size();

	CRect rectDlg;
	GetWindowRect(rectDlg);
	m_szMaxDlgSize = rectDlg.Size();
	m_nLastDlgHeight = m_szMaxDlgSize.cy;

	CWnd* arrWndRows[] = {&m_wndFind, &m_wndReplace, &m_wndMatchCase};
	static_assert(_countof(m_arrUIRows) == _countof(arrWndRows), "UI design breaking change?");
	for (int nRow = 0; nRow < _countof(arrWndRows); ++nRow)
	{
		arrWndRows[nRow]->GetWindowRect(rect);
		ScreenToClient(rect);
		m_arrUIRows[nRow].top = rect.top;
		m_arrUIRows[nRow].bottom = rect.bottom;
	}

	m_hAccel = LoadAccelerators(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_QUICKFIND_ACCEL));
	ASSERT(m_hAccel != NULL);
	if (!m_pAccelTable)
	{
		m_nAccelSize = ::CopyAcceleratorTable(m_hAccel, NULL, 0);

		m_pAccelTable = new ACCEL[m_nAccelSize];
		ENSURE(m_pAccelTable != NULL);

		::CopyAcceleratorTable(m_hAccel, m_pAccelTable, m_nAccelSize);
	}

	m_wndFindAction.m_bDefaultClick = TRUE;
	InitFindActionMenu();
	int nCurFindActionID = m_info.IsFindReplaceNext() ? ID_QUICKFIND_NEXT : ID_QUICKFIND_PREVIOUS;
	VERIFY(InitButton(m_wndFindAction, nCurFindActionID));
	VERIFY(InitButton(m_wndClose, IDR_QUICKFIND_CLOSE));
	VERIFY(InitButton(m_wndReplaceNext, ID_QUICKFIND_REPLACENEXT));
	VERIFY(InitButton(m_wndReplaceAll, ID_QUICKFIND_REPLACEALL));
	VERIFY(InitButton(m_wndMatchCase, ID_QUICKFIND_MATCHCASE));
	VERIFY(InitButton(m_wndMatchWord, ID_QUICKFIND_MATCHWORD));
	VERIFY(InitButton(m_wndRegEx, ID_QUICKFIND_REGEX));

	VERIFY(InitCombo(m_wndFind, m_info.saSearch));
	VERIFY(InitCombo(m_wndReplace, m_info.saReplace));
	CString strCue;
	strCue.LoadString(ID_QUICKFIND_REPLACEWITH);
	m_wndReplace.SetCueBanner(strCue);

	m_wndScope.ShowWindow(m_info.IsShowScope() ? SW_SHOWNORMAL : SW_HIDE);
	SwitchUI(m_info.IsInitShowAsReplace(), m_info.IsInitShowOptions());

	return TRUE;  // return TRUE  unless you set the focus to a control
}

CString CQuickFindWnd::GetFindString() const
{
	CString strText;
	m_wndFind.GetWindowText(strText);
	return strText;
}

CString CQuickFindWnd::GetReplaceString() const
{
	CString strText;
	m_wndReplace.GetWindowText(strText);
	return strText;
}

void CQuickFindWnd::SetActiveShowWindow()
{
	SetActiveWindow();
	UpdateWindowPos();
	m_wndFind.SetFocus();
	ShowWindow(SW_SHOW);
}

enum {
	QuickFindMinTrackWidth = 200,
};

void CQuickFindWnd::UpdateWindowPos()
{
	auto pWndOwner = GetOwner();
	ASSERT_VALID(pWndOwner);
	CRect rectOwnerClient;
	pWndOwner->GetClientRect(rectOwnerClient);
	CRect rectDlg;
	GetWindowRect(rectDlg);
	if (m_info.IsFreeMove())
	{
		// in case the window was moved outside the client area, move it to proper place
		pWndOwner->ScreenToClient(rectDlg);
		CSize szDiff(rectDlg.right - rectOwnerClient.right, rectDlg.bottom - rectOwnerClient.bottom);
		if (szDiff.cx > 0)
			rectDlg.left = max(rectDlg.left - szDiff.cx, rectOwnerClient.left);
		if (szDiff.cy > 0)
			rectDlg.top = max(rectDlg.top - szDiff.cy, rectOwnerClient.top);
		if (szDiff.cx > 0 || szDiff.cy > 0)
			SetWindowPos(nullptr, rectDlg.left, rectDlg.top, -1, -1, SWP_NOSIZE);
	}
	else
	{
		int nLeft = max(rectOwnerClient.right - rectDlg.Width(), rectOwnerClient.left);
		int nWidth = max(rectOwnerClient.right - nLeft, QuickFindMinTrackWidth);
		SetWindowPos(nullptr, nLeft, rectOwnerClient.top, nWidth, rectDlg.Height(), 0);
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
	_GetComboStringArray(m_wndFind, sa, m_info.nMaxItems);
}

void CQuickFindWnd::GetReplaceStringArray(CStringArray& sa) const
{
	_GetComboStringArray(m_wndReplace, sa, m_info.nMaxItems);
}

BOOL CQuickFindWnd::OnEraseBkgnd(CDC* pDC)
{
	CRect rect;
	GetClientRect(rect);
	pDC->FillSolidRect(rect, GetSysColor(COLOR_3DFACE));

	rect.right = rect.left + GetSystemMetrics(SM_CXHSCROLL);
	rect.top = rect.bottom - GetSystemMetrics(SM_CYVSCROLL);
	HTHEME ht = OpenThemeData(m_hWnd, L"SCROLLBAR");
	if (ht)
	{
		DrawThemeBackground(ht, pDC->GetSafeHdc(), SBP_SIZEBOX, SZB_HALFBOTTOMLEFTALIGN, rect, nullptr);
		CloseThemeData(ht);
	}

	if ( GetMoveGripperRect(rect) )
	{
		for (LONG yy = rect.top; yy <= rect.bottom; yy += 2)
		{
			for (LONG xx = rect.left + (yy & 1) ? 2 : 0; xx <= rect.right; xx += 4)
			{
				pDC->SetPixel(xx, yy, RGB(153, 153, 153));
			}
		}
	}
	return TRUE;
}

void CQuickFindWnd::OnPaint()
{
	CPaintDC dc(this);
	CMemDC memDC(dc, this);
	CDC* pDC = &memDC.GetDC();
	//CDC* pDC = &dc;
	CRect rect;
	GetClientRect(rect);
	pDC->FillSolidRect(rect, GetSysColor(COLOR_3DFACE));

	rect.right = rect.left + GetSystemMetrics(SM_CXHSCROLL);
	rect.top = rect.bottom - GetSystemMetrics(SM_CYVSCROLL);
	HTHEME ht = OpenThemeData(m_hWnd, L"SCROLLBAR");
	if (ht)
	{
		DrawThemeBackground(ht, pDC->GetSafeHdc(), SBP_SIZEBOX, SZB_HALFBOTTOMLEFTALIGN, rect, nullptr);
		CloseThemeData(ht);
	}

// 	GetMoveGripperRect(rect);
// 	for (LONG yy = rect.top; yy <= rect.bottom; yy += 2)
// 	{
// 		for (LONG xx = rect.left + (yy & 1) ? 2 : 0; xx <= rect.right; xx += 4)
// 		{
// 			pDC->SetPixel(xx, yy, RGB(153,153,153));
// 		}
// 	}
}

BOOL CQuickFindWnd::GetMoveGripperRect(CRect& rectGripper)
{
	if (!m_wndReplaceAll.m_hWnd)
		return FALSE;
	GetClientRect(rectGripper);
	CRect rect;
	m_wndReplaceAll.GetWindowRect(rect);
	ScreenToClient(rect);
	rectGripper.left = rect.right;
	rectGripper.top = rect.top;
	rectGripper.DeflateRect(5, 5);
	return TRUE;
}

BOOL CQuickFindWnd::GetSizeGripperRect(CRect& rectGripper)
{
	int nGripCX = GetSystemMetrics(SM_CXHSCROLL);
	int nGripCY = GetSystemMetrics(SM_CYVSCROLL);
	CRect rcClient;
	GetClientRect(rcClient);
	rectGripper = rcClient;
	rectGripper.right = rectGripper.left + nGripCX;
	return TRUE;
}

LRESULT CQuickFindWnd::OnNcHitTest(CPoint point)
{
 	CPoint ptHitClient = point;
 	ScreenToClient(&ptHitClient);
	CRect rectGripper;
	if ( GetSizeGripperRect(rectGripper) && rectGripper.PtInRect(ptHitClient) )
	{
		int nGripCY = GetSystemMetrics(SM_CYVSCROLL);
		if (ptHitClient.y >= rectGripper.bottom - nGripCY)
			return HTBOTTOMLEFT;
		return HTLEFT;
	}
	else if ( GetMoveGripperRect(rectGripper) && rectGripper.PtInRect(ptHitClient) )
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
			rect.bottom = m_arrUIRows[nRow].bottom + m_arrUIRows[0].top;
			ClientToScreen(rect);
			*pRect = rect;
			BOOL bShowReplaceUI = m_bShowReplaceUI;
			BOOL bShowOptionsUI = m_bShowOptionsUI;
			switch (nRow)
			{
			case 0:
				bShowReplaceUI = bShowOptionsUI = FALSE;
				break;
			case 1:
				bShowReplaceUI = m_info.IsInitShowAsReplace();
				bShowOptionsUI = !bShowReplaceUI;
				break;
			case 2:
				bShowReplaceUI = bShowOptionsUI = TRUE;
				break;
			}
			if (bShowReplaceUI ^ m_bShowReplaceUI || bShowOptionsUI ^ m_bShowOptionsUI )
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
	CWnd* arrWnds[] ={ &m_wndMatchCase, &m_wndMatchWord, &m_wndRegEx, &m_wndScope };
	BOOL bMoveWnd = bShow;
	for (auto pWnd : arrWnds)
	{
		if (bMoveWnd)
		{
			CRect rect;
			pWnd->GetWindowRect(rect);
			ScreenToClient(rect);
			rect.top = m_bShowReplaceUI ? m_arrUIRows[2].top : m_arrUIRows[1].top;
			pWnd->SetWindowPos(nullptr, rect.left, rect.top, -1, -1, SWP_NOSIZE|SWP_NOZORDER);
		}
		pWnd->ShowWindow(bShow ? SW_SHOWNA : SW_HIDE);
	}
}

void CQuickFindWnd::OnSize(UINT nType, int cx, int cy)
{
	CQuickFindWndBase::OnSize(nType, cx, cy);

	CRect rect;
	//if (GetSizeGripperRect(rect))
	//	InvalidateRect(rect);

	//if (GetMoveGripperRect(rect))
	//	InvalidateRect(rect);

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
		if (hdwp)
			hdwp = DeferWindowPos(hdwp, hWnd, NULL, -1, -1, rect.Width(), rect.Height(),
				SWP_NOMOVE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE);
		else
			pResizeWnd->MoveWindow(&rect);
	}	
	for (auto pAnchorWnd : arrWndAnchorRight)
	{
		pAnchorWnd->GetWindowRect(rect);
		ScreenToClient(rect);
		rect.OffsetRect(szDiff.cx, 0);
		HWND hWnd = pAnchorWnd->GetSafeHwnd();
		if (hdwp)
			hdwp = DeferWindowPos(hdwp, hWnd, NULL, rect.left, rect.top, -1, -1,
				SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE);
		else
			pAnchorWnd->MoveWindow(&rect);
	}
	if (hdwp)
		EndDeferWindowPos(hdwp);

	m_szLastClientSize = szDlgNewSize;
}

void CQuickFindWnd::OnMoving(UINT nSide, LPRECT lpRect)
{
	CQuickFindWndBase::OnMoving(nSide, lpRect);

	if (m_info.IsFreeMove())
	{
		// restrain in owner
		auto pWndOwner = GetOwner();
		if (!pWndOwner->GetSafeHwnd())
			return;
		CRect rectNew = *lpRect;
		CSize szNew = rectNew.Size();
		CRect rectOwner;
		pWndOwner->GetClientRect(rectOwner);
		pWndOwner->ClientToScreen(rectOwner);
		if (rectNew.left < rectOwner.left)
		{
			rectNew.left = rectOwner.left;
			rectNew.right = rectNew.left + szNew.cx;
		}
		if (rectNew.top < rectOwner.top)
		{
			rectNew.top = rectOwner.top;
			rectNew.bottom = rectNew.top + szNew.cy;
		}
		if (rectNew.right > rectOwner.right)
		{
			rectNew.right = rectOwner.right;
			rectNew.left = rectNew.right - szNew.cx;
		}
		if (rectNew.bottom > rectOwner.bottom)
		{
			rectNew.bottom = rectOwner.bottom;
			rectNew.top = rectNew.bottom - szNew.cy;
		}
		*lpRect = rectNew;
	}
	else
	{
		m_info.dwFlags |= QUICKFIND_INFO::FlagsFreeMove;
	}
}

void CQuickFindWnd::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	CQuickFindWndBase::OnGetMinMaxInfo(lpMMI);
	lpMMI->ptMinTrackSize.x = QuickFindMinTrackWidth;
	if (m_wndFind.GetSafeHwnd())
	{
		CRect rect;
		m_wndFind.GetWindowRect(rect);
		ScreenToClient(rect);
		lpMMI->ptMinTrackSize.y = rect.bottom + rect.top;
	}
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
	switch (m_wndFindAction.m_nMenuResult)
	{
	default:
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
		OnFindAll();
		break;
	}
	if (bChangeButton)
		VERIFY(InitButton(m_wndFindAction, m_wndFindAction.m_nMenuResult));
}

void CQuickFindWnd::OnMatchCase()
{
	if (m_wndMatchCase.GetCheck() != BST_UNCHECKED)
		m_info.dwFlags |= QUICKFIND_INFO::FlagsMatchCase;
	else
		m_info.dwFlags &= ~QUICKFIND_INFO::FlagsMatchCase;
}

void CQuickFindWnd::OnMatchWord()
{
	if (m_wndMatchWord.GetCheck() != BST_UNCHECKED)
		m_info.dwFlags |= QUICKFIND_INFO::FlagsMatchWord;
	else
		m_info.dwFlags &= ~QUICKFIND_INFO::FlagsMatchWord;
}

void CQuickFindWnd::OnUseRegEx()
{
	if (m_wndRegEx.GetCheck() != BST_UNCHECKED)
		m_info.dwFlags |= QUICKFIND_INFO::FlagsUseRegEx;
	else
		m_info.dwFlags &= ~QUICKFIND_INFO::FlagsUseRegEx;
}

void CQuickFindWnd::OnSelChangeFind()
{
	if (m_info.IsNotifyFindTextChange())
		NotifyOwner(QuickFindCmdFindTextChange);
}

void CQuickFindWnd::OnEditChangeFind()
{
	if (m_info.IsNotifyFindTextChange())
		NotifyOwner(QuickFindCmdFindTextChange);
}

void CQuickFindWnd::OnSelChangeScope()
{
	NotifyOwner(QuickFindCmdScopeSelChange, (WPARAM)m_wndScope.GetCurSel());
}

static void _PromoteTextInComboEx(CComboBox& combo, LPCTSTR pszText, UINT nMaxCount)
{
	ASSERT(combo.m_hWnd);
	int nIndex = combo.FindStringExact(-1, pszText);
	if (nIndex < 0)
	{
		combo.InsertString(0, pszText);
		if ((UINT)combo.GetCount() > nMaxCount)
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

static void _PromoteCurTextInComboList(CComboBox& combo, UINT nMaxCount)
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
	_PromoteTextInComboEx(m_wndFind, pszText, m_info.nMaxItems);
	OnSelChangeFind();
}

void CQuickFindWnd::PromoteFindTextItems()
{
	_PromoteCurTextInComboList(m_wndFind, m_info.nMaxItems);
}

void CQuickFindWnd::PromoteReplaceTextItems()
{
	_PromoteCurTextInComboList(m_wndReplace, m_info.nMaxItems);
}

void CQuickFindWnd::OnFindNext()
{
	PromoteFindTextItems();
	NotifyOwner(QuickFindCmdFind);
}

void CQuickFindWnd::OnFindPrevious()
{
	PromoteFindTextItems();
	NotifyOwner(QuickFindCmdFind);
}

void CQuickFindWnd::OnFindAll()
{
	PromoteFindTextItems();
	NotifyOwner(QuickFindCmdFindAll);
}

void CQuickFindWnd::OnReplaceNext()
{
	PromoteReplaceTextItems();
	NotifyOwner(QuickFindCmdReplace);
}

void CQuickFindWnd::OnReplaceAll()
{
	PromoteReplaceTextItems();
	NotifyOwner(QuickFindCmdReplaceAll);
}

void CQuickFindWnd::SwitchUI(BOOL bShowAsReplace, BOOL bShowOptions)
{
	m_bShowReplaceUI = bShowAsReplace;
	m_bShowOptionsUI = bShowOptions;
	CRect rectDlg, rectClient;
	GetWindowRect(rectDlg);
	GetClientRect(rectClient);
	int nRows = (m_bShowOptionsUI ? 1 : 0) + (m_bShowReplaceUI ? 1 : 0);
	rectClient.bottom = m_arrUIRows[nRows].bottom + m_arrUIRows[0].top;
	ClientToScreen(rectClient);
	rectDlg.bottom = rectClient.bottom;

	ShowReplaceUI(m_bShowReplaceUI);
	ShowOptionsUI(m_bShowOptionsUI);	
	CSize szDlg = rectDlg.Size();
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

void CQuickFindWnd::OnEditFind()
{
	if (!m_info.IsInitShowAsReplace() && m_bShowOptionsUI)
		return;
	m_info.dwFlags &= ~QUICKFIND_INFO::FlagsInitShowReplace;
	SwitchUI(FALSE, TRUE);
	m_wndFind.SetFocus();
}

void CQuickFindWnd::OnEditReplace()
{
	if (m_info.IsInitShowAsReplace() && m_bShowOptionsUI)
		return;
	m_info.dwFlags |= QUICKFIND_INFO::FlagsInitShowReplace;
	SwitchUI(TRUE, TRUE);
	m_wndFind.SetFocus();
}

void CQuickFindWnd::OnNcDestroy()
{
	NotifyOwner(QuickFindCmdTerminating);
	CQuickFindWndBase::OnNcDestroy();
}
