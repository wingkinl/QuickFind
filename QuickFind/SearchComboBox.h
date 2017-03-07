#pragma once
#include "ComboBoxBrowseCtrl.h"

typedef CComboBoxBrowseCtrl	CSearchComboBoxBase;

class CSearchComboBox : public CSearchComboBoxBase
{
	DECLARE_DYNAMIC(CSearchComboBox)
public:
	CSearchComboBox();
	~CSearchComboBox();
public:
// Overrides
protected:
	void Init() override;

	std::unique_ptr<CMFCEditBrowseCtrl> CreateEditControl() override;
protected:
	
	DECLARE_MESSAGE_MAP()
};

