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
	m_bShowReplaceUI = FALSE;
	m_bShowOptionsUI = FALSE;
	m_nSecondRowCtrlsTop = -1;
	m_nThirdRowCtrlsTop = -1;
}

CQuickFindWnd::~CQuickFindWnd()
{
}

void CQuickFindWnd::DoDataExchange(CDataExchange* pDX)
{
	CQuickFindWndBase::DoDataExchange(pDX);	
	DDX_Control(pDX, IDOK, m_wndFindAction);
	DDX_Control(pDX, IDCANCEL, m_wndClose);
	DDX_Control(pDX, IDC_QUICK_FIND, m_wndFind);
	DDX_Control(pDX, IDC_QUICK_REPLACE, m_wndReplace);
	DDX_Control(pDX, IDC_QUICK_REPLACE_NEXT, m_wndReplaceNext);
	DDX_Control(pDX, IDC_QUICK_REPLACE_ALL, m_wndReplaceAll);
	DDX_Control(pDX, IDC_QUICK_FIND_MATCHCASE, m_wndMatchCase);
	DDX_Control(pDX, IDC_QUICK_FIND_MATCHWORD, m_wndMatchWord);
	DDX_Control(pDX, IDC_QUICK_FIND_REGEX, m_wndRegEx);
	DDX_Control(pDX, IDC_QUICK_FIND_SCOPE, m_wndScope);
}


BEGIN_MESSAGE_MAP(CQuickFindWnd, CQuickFindWndBase)
	ON_WM_PAINT()
	ON_WM_NCHITTEST()
	ON_WM_SIZE()
	ON_WM_GETMINMAXINFO()
END_MESSAGE_MAP()


// CQuickFindWnd message handlers

BOOL CQuickFindWnd::InitButton(CMFCButton& btn, UINT nResID, HINSTANCE hResInst) const
{
	auto resName = MAKEINTRESOURCE(nResID);
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
	strTooltip.LoadString(nResID);
	btn.SetTooltip(strTooltip);
	return TRUE;
}

BOOL CQuickFindWnd::Create(LPCTSTR lpszFindWhat, LPCTSTR lpszReplaceWith, DWORD dwFlags, CWnd* pOwnerWnd)
{
	CQuickFindWndBase::Create(IDD_QUICK_FIND_REPLACE, pOwnerWnd);
	return TRUE;
}

BOOL CQuickFindWnd::OnInitDialog()
{
	CQuickFindWndBase::OnInitDialog();
	CRect rect;
	GetClientRect(rect);
	m_szLastClientSize = rect.Size();

	GetWindowRect(rect);
	m_wndMaxDlgSize = rect.Size();

	m_wndReplace.GetWindowRect(rect);
	ScreenToClient(rect);
	m_nSecondRowCtrlsTop = rect.top;

	m_wndMatchCase.GetWindowRect(rect);
	ScreenToClient(rect);
	m_nThirdRowCtrlsTop = rect.top;

	//SetBackgroundColor(RGB(45,45,48), FALSE);

	m_wndFindAction.m_bDefaultClick = TRUE;
	VERIFY(InitButton(m_wndFindAction, IDR_QUICKREPLACE_NEXT));
	VERIFY(InitButton(m_wndClose, IDR_QUICKFIND_CLOSE));
	VERIFY(InitButton(m_wndReplaceNext, IDR_QUICKREPLACE_NEXT));
	VERIFY(InitButton(m_wndReplaceAll, IDR_QUICKREPLACE_ALL));
	VERIFY(InitButton(m_wndMatchCase, IDR_QUICKFIND_MATCHCASE));
	VERIFY(InitButton(m_wndMatchWord, IDR_QUICKFIND_MATCHWORD));
	VERIFY(InitButton(m_wndRegEx, IDR_QUICKFIND_REGEX));

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

	if (szDiff.cy)
	{
	}

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
	CRect rect;
	m_wndFind.GetWindowRect(rect);
	ScreenToClient(rect);
	lpMMI->ptMinTrackSize.y = rect.bottom + rect.top;

	lpMMI->ptMaxTrackSize.y = m_wndMaxDlgSize.cy;
}
