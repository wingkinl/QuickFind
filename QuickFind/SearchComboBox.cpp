#include "stdafx.h"
#include "SearchComboBox.h"
#include "Resource.h"

IMPLEMENT_DYNAMIC(CSearchComboBox, CSearchComboBoxBase)

CSearchComboBox::CSearchComboBox()
{
}


CSearchComboBox::~CSearchComboBox()
{
}

BEGIN_MESSAGE_MAP(CSearchComboBox, CSearchComboBoxBase)
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
	SetCueBanner(L"Find...");
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
	CSearchComboBox* pParent = STATIC_DOWNCAST(CSearchComboBox, GetParent());
	auto pComboParent = pParent->GetParent();
	if (pComboParent->GetSafeHwnd())
		pComboParent->SendMessage(WM_COMMAND, MAKEWPARAM(pParent->GetDlgCtrlID(), CBN_EDITCHANGE), (LPARAM)pParent->GetSafeHwnd());
}

BOOL CSearchComboBoxEdit::OnEnChange()
{
	RedrawWindow(NULL, NULL, RDW_FRAME | RDW_INVALIDATE);
	return FALSE;	// allow parent to handle it too
}

std::unique_ptr<CComboBoxBrowseCtrlEdit> CSearchComboBox::CreateEditControl()
{
#if _MSC_VER >= 1800
	return std::make_unique<CSearchComboBoxEdit>();
#else
	return std::unique_ptr<CSearchComboBoxEdit>(new CSearchComboBoxEdit());
#endif
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


