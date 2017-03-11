// ComboBoxWithBtn.cpp : implementation file
//

#include "stdafx.h"
#include "ComboBoxBrowseCtrl.h"


// CComboBoxBrowseCtrl

IMPLEMENT_DYNAMIC(CComboBoxBrowseCtrl, CComboBoxBrowseCtrlBase)

CComboBoxBrowseCtrl::CComboBoxBrowseCtrl()
{
	
}

CComboBoxBrowseCtrl::~CComboBoxBrowseCtrl()
{
}

BEGIN_MESSAGE_MAP(CComboBoxBrowseCtrl, CComboBoxBrowseCtrlBase)
	ON_WM_CREATE()
END_MESSAGE_MAP()


// CComboBoxBrowseCtrl message handlers

void CComboBoxBrowseCtrl::PreSubclassWindow()
{
	CComboBoxBrowseCtrlBase::PreSubclassWindow();

	_AFX_THREAD_STATE* pThreadState = AfxGetThreadState();
	if (pThreadState->m_pWndInit == NULL)
	{
		Init();
	}
}

int CComboBoxBrowseCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CComboBoxBrowseCtrlBase::OnCreate(lpCreateStruct) == -1)
		return -1;

	Init();
	return 0;
}

void CComboBoxBrowseCtrl::Init()
{
	COMBOBOXINFO info;
	memset(&info, 0, sizeof(COMBOBOXINFO));
	info.cbSize = sizeof(COMBOBOXINFO);

	GetComboBoxInfo(&info);
	if (info.hwndItem)
	{
		if (!m_pEdit)
			m_pEdit = CreateEditControl();
		m_pEdit->SubclassWindow(info.hwndItem);
	}
}

CComboBoxBrowseCtrlEdit* CComboBoxBrowseCtrl::GetEditControl() const
{
	return m_pEdit.get();
}

CComboBoxBrowseCtrlEdit::CComboBoxBrowseCtrlEdit()
{

}

CComboBoxBrowseCtrlEdit::~CComboBoxBrowseCtrlEdit()
{

}

BOOL CComboBoxBrowseCtrlEdit::PreTranslateMessage(MSG* pMsg)
{
	// bypass CMFCEditBrowseCtrl's code that handles Alt+Down and Alt+Right
	return CEdit::PreTranslateMessage(pMsg);
}

std::unique_ptr<CComboBoxBrowseCtrlEdit> CComboBoxBrowseCtrl::CreateEditControl()
{
#if _MSC_VER >= 1800
	return std::make_unique<CComboBoxBrowseCtrlEdit>();
#else
	return std::unique_ptr<CComboBoxBrowseCtrlEdit>(new CComboBoxBrowseCtrlEdit());
#endif
}
