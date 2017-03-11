// most code were taken from CMFCEditBrowseCtrl

#pragma once

#ifndef _MEMORY_
#include <memory>
#endif //#ifndef _MEMORY_

#ifndef QUICKFIND_EXT_CLASS
	#define QUICKFIND_EXT_CLASS
#endif

typedef CMFCEditBrowseCtrl	CComboBoxBrowseCtrlEditBase;

class QUICKFIND_EXT_CLASS CComboBoxBrowseCtrlEdit : public CComboBoxBrowseCtrlEditBase
{
public:
	CComboBoxBrowseCtrlEdit();
	~CComboBoxBrowseCtrlEdit();
public:
	BOOL PreTranslateMessage(MSG* pMsg);
};

// CComboBoxBrowseCtrl

typedef CComboBox	CComboBoxBrowseCtrlBase;

class QUICKFIND_EXT_CLASS CComboBoxBrowseCtrl : public CComboBoxBrowseCtrlBase
{
	DECLARE_DYNAMIC(CComboBoxBrowseCtrl)

public:
	CComboBoxBrowseCtrl();
	virtual ~CComboBoxBrowseCtrl();
public:
	CComboBoxBrowseCtrlEdit*	GetEditControl() const;
protected:
	virtual void Init();

	std::unique_ptr<CComboBoxBrowseCtrlEdit> m_pEdit;

	virtual std::unique_ptr<CComboBoxBrowseCtrlEdit> CreateEditControl();
// Operations
public:
	void PreSubclassWindow() override;
// Overrides
public:
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

	DECLARE_MESSAGE_MAP()
};


