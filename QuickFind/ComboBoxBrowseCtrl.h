// most code were taken from CMFCEditBrowseCtrl

#pragma once

#ifndef _MEMORY_
#include <memory>
#endif //#ifndef _MEMORY_

// CComboBoxBrowseCtrl

typedef CComboBox	CComboBoxBrowseCtrlBase;

class CComboBoxBrowseCtrl : public CComboBoxBrowseCtrlBase
{
	DECLARE_DYNAMIC(CComboBoxBrowseCtrl)

public:
	CComboBoxBrowseCtrl();
	virtual ~CComboBoxBrowseCtrl();
public:
	CMFCEditBrowseCtrl*	GetEditControl() const;
protected:
	virtual void Init();

	std::unique_ptr<CMFCEditBrowseCtrl> m_pEdit;

	virtual std::unique_ptr<CMFCEditBrowseCtrl> CreateEditControl();
// Operations
public:
	void PreSubclassWindow() override;
// Overrides
public:
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

	DECLARE_MESSAGE_MAP()
};


