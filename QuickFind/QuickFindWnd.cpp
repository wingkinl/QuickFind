// QuickFindWnd.cpp : implementation file
//

#include "stdafx.h"
#include "QuickFindWnd.h"
#include "afxdialogex.h"
#include "Resource.h"


QUICKFIND_INFO::QUICKFIND_INFO()
{
	dwFlags = FlagsInitShowOptions;
	pWndOwner = nullptr;
}

QUICKFIND_INFO& QUICKFIND_INFO::operator=(const QUICKFIND_INFO& rhs)
{
	dwFlags = rhs.dwFlags;
	pWndOwner = rhs.pWndOwner;
	saSearch.Copy(rhs.saSearch);
	saReplace.Copy(rhs.saReplace);
	return *this;
}

// CQuickFindWnd dialog

IMPLEMENT_DYNAMIC(CQuickFindWnd, CQuickFindWndBase)

CQuickFindWnd::CQuickFindWnd()
	: CQuickFindWndBase(IDD_QUICK_FIND_REPLACE)
{
	m_szLastClientSize.SetSize(0, 0);
	m_nLastDlgHeight = 0;
	m_bShowReplaceUI = FALSE;
	m_bShowOptionsUI = TRUE;
	m_nSecondRowCtrlsTop = -1;
	m_nThirdRowCtrlsTop = -1;
	m_hAccel = nullptr;
	m_pAccelTable = nullptr;
	m_nAccelSize = 0;
	m_nCurFindActionID = 0;
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
	DDX_Control(pDX, IDC_QUICK_FIND, m_wndFind);
	DDX_Control(pDX, IDC_QUICK_REPLACE, m_wndReplace);
	DDX_Control(pDX, ID_QUICKFIND_REPLACENEXT, m_wndReplaceNext);
	DDX_Control(pDX, ID_QUICKFIND_REPLACEALL, m_wndReplaceAll);
	DDX_Control(pDX, ID_QUICKFIND_MATCHCASE, m_wndMatchCase);
	DDX_Control(pDX, ID_QUICKFIND_MATCHWORD, m_wndMatchWord);
	DDX_Control(pDX, ID_QUICKFIND_REGEX, m_wndRegEx);
	DDX_Control(pDX, ID_QUICKFIND_SCOPE, m_wndScope);
}


BEGIN_MESSAGE_MAP(CQuickFindWnd, CQuickFindWndBase)
	ON_WM_PAINT()
	ON_WM_NCHITTEST()
	ON_WM_SIZING()
	ON_WM_SIZE()
	ON_WM_GETMINMAXINFO()
	ON_BN_CLICKED(IDOK, &OnButtonFindActionMenu)
	ON_COMMAND(ID_QUICKFIND_NEXT, &OnFindNext)
	ON_COMMAND(ID_QUICKFIND_PREVIOUS, &OnFindPrevious)
	ON_COMMAND(ID_QUICKFIND_ALL, &OnFindAll)
	ON_COMMAND(ID_QUICKFIND_REPLACENEXT, &OnReplaceNext)
	ON_COMMAND(ID_QUICKFIND_REPLACEALL, &OnReplaceAll)
	ON_COMMAND(ID_EDIT_FIND, &OnEditFind)
	ON_COMMAND(ID_EDIT_REPLACE, &OnEditReplace)
END_MESSAGE_MAP()


// CQuickFindWnd message handlers

const QUICKFIND_INFO& CQuickFindWnd::GetParams()
{
	return m_info;
}

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

BOOL CQuickFindWnd::Create(const QUICKFIND_INFO& info)
{
	ASSERT(info.pWndOwner);
	m_info = info;
	BOOL bRet = CQuickFindWndBase::Create(IDD_QUICK_FIND_REPLACE, m_info.pWndOwner);
	if (!bRet)
		return FALSE;
	return TRUE;
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

	m_wndReplace.GetWindowRect(rect);
	ScreenToClient(rect);
	m_nSecondRowCtrlsTop = rect.top;

	m_wndMatchCase.GetWindowRect(rect);
	ScreenToClient(rect);
	m_nThirdRowCtrlsTop = rect.top;

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
	m_menuFindAction.LoadMenu(IDR_QUICKFIND_ACTION);
	m_wndFindAction.m_hMenu = m_menuFindAction.GetSubMenu(0)->GetSafeHmenu();
	m_wndFindAction.m_bOSMenu = FALSE;
	m_nCurFindActionID = ID_QUICKFIND_NEXT;
	VERIFY(InitButton(m_wndFindAction, m_nCurFindActionID));
	VERIFY(InitButton(m_wndClose, IDR_QUICKFIND_CLOSE));
	VERIFY(InitButton(m_wndReplaceNext, ID_QUICKFIND_REPLACENEXT));
	VERIFY(InitButton(m_wndReplaceAll, ID_QUICKFIND_REPLACEALL));
	VERIFY(InitButton(m_wndMatchCase, ID_QUICKFIND_MATCHCASE));
	VERIFY(InitButton(m_wndMatchWord, ID_QUICKFIND_MATCHWORD));
	VERIFY(InitButton(m_wndRegEx, ID_QUICKFIND_REGEX));

	m_bShowReplaceUI = !!(m_info.dwFlags & QUICKFIND_INFO::FlagsInitShowReplace);
	m_bShowOptionsUI = !!(m_info.dwFlags & QUICKFIND_INFO::FlagsInitShowOptions);
	if (!m_bShowReplaceUI || !m_bShowOptionsUI)
	{
		CalcLayout(&rectDlg);
		CSize szDlg = rectDlg.Size();
		SetWindowPos(nullptr, -1, -1, szDlg.cx, szDlg.cy, SWP_NOMOVE | SWP_NOZORDER);
		m_nLastDlgHeight = szDlg.cy;
	}

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CQuickFindWnd::OnPaint()
{
	CPaintDC dc(this);
	CRect rect;
	GetClientRect(&rect);
	rect.right = rect.left + GetSystemMetrics(SM_CXHSCROLL);
	rect.top = rect.bottom - GetSystemMetrics(SM_CYVSCROLL);
	HTHEME ht = OpenThemeData(m_hWnd, L"SCROLLBAR");
	if (ht)
	{
		DrawThemeBackground(ht, dc.GetSafeHdc(), SBP_SIZEBOX, SZB_HALFBOTTOMLEFTALIGN, rect, nullptr);
		CloseThemeData(ht);
	}
}

LRESULT CQuickFindWnd::OnNcHitTest(CPoint point)
{
	CPoint ptHitClient = point;
	ScreenToClient(&ptHitClient);

	int nGripCX = GetSystemMetrics(SM_CXHSCROLL);
	int nGripCY = GetSystemMetrics(SM_CYVSCROLL);
	CRect rcClient;
	GetClientRect(rcClient);
	CRect rcLeft = rcClient;
	rcLeft.right = rcLeft.left + nGripCX;
	if (rcLeft.PtInRect(ptHitClient))
	{
		if (ptHitClient.y >= rcLeft.bottom - nGripCY)
			return HTBOTTOMLEFT;
		return HTLEFT;
	}
	else if (ptHitClient.y >= rcClient.bottom - 5)
		return HTBOTTOM;

	return CQuickFindWndBase::OnNcHitTest(point);
}

void CQuickFindWnd::CalcLayout(LPRECT pRect)
{
	if (m_bShowReplaceUI && m_bShowOptionsUI)
	{
		pRect->bottom = pRect->top + m_szMaxDlgSize.cy;
	}
	else if (m_bShowReplaceUI || m_bShowOptionsUI)
	{
		CWnd* pWnd = m_bShowReplaceUI ? (CWnd*)&m_wndReplace : &m_wndMatchCase;
		CRect rect;
		pWnd->GetWindowRect(rect);
		int nHeight = rect.Height();
		GetClientRect(rect);
		rect.bottom = m_nSecondRowCtrlsTop + nHeight;
		ClientToScreen(rect);
		pRect->bottom = rect.bottom + 3;
	}
	else
	{
		CRect rect;
		m_wndFind.GetWindowRect(rect);
		pRect->bottom = rect.bottom + 3;
	}
	ShowReplaceUI(m_bShowReplaceUI);
	ShowOptionsUI(m_bShowOptionsUI);
}

BOOL CQuickFindWnd::IsInitShowAsReplace() const
{
	return !!(m_info.dwFlags & QUICKFIND_INFO::FlagsInitShowReplace);
}

void CQuickFindWnd::OnSizing(UINT fwSide, LPRECT pRect)
{
	CQuickFindWndBase::OnSizing(fwSide, pRect);
	if (fwSide == WMSZ_LEFT)
		return;

	CRect rectOldWnd;
	GetWindowRect(rectOldWnd);

	CRect rect(pRect);
	//int nYDelta = rect.Height() - rectOldWnd.Height();
	//if (nYDelta == 0)
	//	return;
	int nYDelta = rect.Height() - m_nLastDlgHeight;
	if (abs(nYDelta) < 5)
		return;
	if (m_bShowReplaceUI && m_bShowOptionsUI)
	{
		// you can't be any larger in height
		ASSERT(nYDelta < 0);
		if (IsInitShowAsReplace())
			m_bShowOptionsUI = FALSE;
		else
			m_bShowReplaceUI = FALSE;
	}
	else if (m_bShowReplaceUI)
	{
		if (nYDelta < 0)
			m_bShowReplaceUI = FALSE;
		else
			m_bShowOptionsUI = TRUE;
	}
	else if (m_bShowOptionsUI)
	{
		if (nYDelta < 0)
			m_bShowOptionsUI = FALSE;
		else
			m_bShowReplaceUI = TRUE;
	}
	else
	{
		// you can't be any smaller in height
		ASSERT(nYDelta > 0);
		if (IsInitShowAsReplace())
			m_bShowReplaceUI = TRUE;
		else
			m_bShowOptionsUI = TRUE;
	}
	CalcLayout(pRect);
	m_nLastDlgHeight = pRect->bottom - pRect->top;
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
	BOOL bMoveWnd = bShow && !IsInitShowAsReplace();
	for (auto pWnd : arrWnds)
	{
		if (bMoveWnd)
		{
			CRect rect;
			pWnd->GetWindowRect(rect);
			ScreenToClient(rect);
			rect.top = m_bShowReplaceUI ? m_nThirdRowCtrlsTop : m_nSecondRowCtrlsTop;
			pWnd->SetWindowPos(nullptr, rect.left, rect.top, -1, -1, SWP_NOSIZE|SWP_NOZORDER);
		}
		pWnd->ShowWindow(bShow);
	}
}

void CQuickFindWnd::OnSize(UINT nType, int cx, int cy)
{
	CQuickFindWndBase::OnSize(nType, cx, cy);

	CRect rect;
	GetClientRect(&rect);
	rect.right = rect.left + GetSystemMetrics(SM_CXHSCROLL);
	//rect.top = rect.bottom - GetSystemMetrics(SM_CYVSCROLL);
	InvalidateRect(rect);

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

void CQuickFindWnd::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	CQuickFindWndBase::OnGetMinMaxInfo(lpMMI);
	lpMMI->ptMinTrackSize.x = 200;
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

void CQuickFindWnd::OnButtonFindActionMenu()
{
	if (m_wndFindAction.m_nMenuResult && m_wndFindAction.m_nMenuResult != m_nCurFindActionID)
	{
		m_nCurFindActionID = m_wndFindAction.m_nMenuResult;
		VERIFY(InitButton(m_wndFindAction, m_nCurFindActionID));
	}
	switch (m_nCurFindActionID)
	{
	case ID_QUICKFIND_NEXT:
		OnFindNext();
		break;
	case ID_QUICKFIND_PREVIOUS:
		OnFindPrevious();
		break;
	case ID_QUICKFIND_ALL:
		OnFindAll();
		break;
	default:
		ASSERT(0);
		break;
	}
}

void CQuickFindWnd::OnFindNext()
{

}

void CQuickFindWnd::OnFindPrevious()
{

}

void CQuickFindWnd::OnFindAll()
{

}

void CQuickFindWnd::OnReplaceNext()
{

}

void CQuickFindWnd::OnReplaceAll()
{

}

void CQuickFindWnd::SwitchUI(BOOL bShowAsReplace)
{
	m_bShowReplaceUI = bShowAsReplace;
	m_bShowOptionsUI = TRUE;
	CRect rectDlg;
	GetWindowRect(rectDlg);

	if (bShowAsReplace)
		m_info.dwFlags |= QUICKFIND_INFO::FlagsInitShowReplace;
	else
		m_info.dwFlags &= ~QUICKFIND_INFO::FlagsInitShowReplace;

	CalcLayout(&rectDlg);
	CSize szDlg = rectDlg.Size();
	SetWindowPos(nullptr, -1, -1, szDlg.cx, szDlg.cy, SWP_NOMOVE | SWP_NOZORDER);
}

void CQuickFindWnd::OnEditFind()
{
	if (!IsInitShowAsReplace() && m_bShowOptionsUI)
		return;
	SwitchUI(FALSE);
}

void CQuickFindWnd::OnEditReplace()
{
	if (IsInitShowAsReplace() && m_bShowOptionsUI)
		return;
	SwitchUI(TRUE);
}
