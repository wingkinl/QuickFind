#include "stdafx.h"
#include "SearchComboBox.h"
#include "Resource.h"

IMPLEMENT_DYNAMIC(CSearchComboBox, CSearchComboBoxBase)

CSearchComboBox::CSearchComboBox()
{
	m_bSearchOK = TRUE;
}


CSearchComboBox::~CSearchComboBox()
{
}

BEGIN_MESSAGE_MAP(CSearchComboBox, CSearchComboBoxBase)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()

typedef CComboBoxBrowseCtrlEdit	CSearchComboBoxEditBase;

class CSearchComboBoxEdit : public CSearchComboBoxEditBase
{
public:
	void OnBrowse() override;

	void OnDrawBrowseButton(CDC* pDC, CRect rect, BOOL bIsButtonPressed, BOOL bIsButtonHot) override;

	void Init();
protected:
	void InitImages();

	void OnAfterUpdate() override;

	afx_msg BOOL OnEnChange();

	DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(CSearchComboBoxEdit, CSearchComboBoxEditBase)
	ON_CONTROL_REFLECT_EX(EN_CHANGE, OnEnChange)
END_MESSAGE_MAP()

void CSearchComboBoxEdit::OnBrowse()
{
	ASSERT_VALID(this);
	ENSURE(GetSafeHwnd() != NULL);
	SetWindowText(_T(""));
	OnAfterUpdate();
	CSearchComboBoxEditBase::OnBrowse();
}

void CSearchComboBoxEdit::OnDrawBrowseButton(CDC* pDC, CRect rect, BOOL bIsButtonPressed, BOOL bIsButtonHot)
{
	ASSERT(m_Mode != BrowseMode_None);
	ASSERT_VALID(pDC);
	COLORREF clrBk = GetGlobalData()->clrWindow;
	if (bIsButtonPressed)
	{
		clrBk = RGB(0x00, 0x7A, 0xCC);
	}
	else if (bIsButtonHot)
	{
		clrBk = RGB(0xC9, 0xDE, 0xF5);
	}
	
	pDC->FillSolidRect(rect, clrBk);

	if (m_ImageBrowse.GetSafeHandle() != NULL)
	{
		int iImage = GetWindowTextLength() ? 0 : 1;
		CPoint ptImage;
		ptImage.x = rect.CenterPoint().x - m_sizeImage.cx / 2;
		ptImage.y = rect.CenterPoint().y - m_sizeImage.cy / 2;

		m_ImageBrowse.Draw(pDC, iImage, ptImage, ILD_NORMAL);
	}
}

void CSearchComboBoxEdit::Init()
{
	CString strCue;
	strCue.LoadString(ID_QUICKFIND_FINDWHAT);
	SetCueBanner(strCue);
	InitImages();

	m_Mode = BrowseMode_Default;

	OnChangeLayout();
}

void CSearchComboBoxEdit::InitImages()
{
	if (m_ImageBrowse.GetSafeHandle() != NULL)
	{
		m_ImageBrowse.DeleteImageList();
	}

	m_sizeImage.cx = 16;
	m_sizeImage.cy = 16;

	UINT nFlags = ILC_MASK | ILC_COLOR32;
	m_ImageBrowse.Create(m_sizeImage.cx, m_sizeImage.cy, nFlags, 0, 0);

	m_ImageBrowse.Add(AfxGetApp()->LoadIcon(ID_QUICKFIND_CLEAR));
	m_ImageBrowse.Add(AfxGetApp()->LoadIcon(IDR_QUICKFIND_MAG));

	m_bDefaultImage = FALSE;
}

void CSearchComboBoxEdit::OnAfterUpdate()
{
	CSearchComboBoxEditBase::OnAfterUpdate();
	RedrawWindow(NULL, NULL, RDW_FRAME | RDW_INVALIDATE);
	CSearchComboBox* pComboBox = STATIC_DOWNCAST(CSearchComboBox, GetParent());
	if (pComboBox)
		pComboBox->OnAfterClearText();
}

BOOL CSearchComboBoxEdit::OnEnChange()
{
	RedrawWindow(NULL, NULL, RDW_FRAME | RDW_INVALIDATE);
	return FALSE;	// allow parent to handle it too
}

void CSearchComboBox::OnAfterClearText()
{
	m_bSearchOK = TRUE;
	RedrawWindow(NULL, NULL, RDW_FRAME | RDW_INVALIDATE);
	auto pParent = GetParent();
	if (pParent->GetSafeHwnd())
		pParent->SendMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), CBN_EDITCHANGE), (LPARAM)GetSafeHwnd());
}

std::unique_ptr<CComboBoxBrowseCtrlEdit> CSearchComboBox::CreateEditControl()
{
#if _MSC_VER >= 1800
	return std::make_unique<CSearchComboBoxEdit>();
#else
	return std::unique_ptr<CSearchComboBoxEdit>(new CSearchComboBoxEdit());
#endif
}

HBRUSH CSearchComboBox::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	if (CTLCOLOR_EDIT == nCtlColor && !m_bSearchOK)
	{
		//COLORREF crfBK = RGB(0xf9, 0xf2, 0xf4);
		//pDC->SetTextColor(RGB(0xc7,0x25,0x4e));
		COLORREF crfBK = RGB(0xf2, 0xde, 0xde);
		pDC->SetTextColor(RGB(0xa9,0x44,0x42));
		pDC->SetBkColor(crfBK);
		pDC->SetBkMode(TRANSPARENT);
		if (!m_brErr.GetSafeHandle())
			m_brErr.CreateSolidBrush(crfBK);
		return (HBRUSH)m_brErr.GetSafeHandle();
	}
	return CSearchComboBoxBase::OnCtlColor(pDC, pWnd, nCtlColor);
}

void CSearchComboBox::Init()
{
	CSearchComboBoxBase::Init();
	CSearchComboBoxEdit* pEditCtrl = static_cast<CSearchComboBoxEdit*>(GetEditControl());
	if (pEditCtrl->GetSafeHwnd())
	{
		pEditCtrl->Init();
	}
}


