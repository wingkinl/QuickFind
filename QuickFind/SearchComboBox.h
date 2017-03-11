#pragma once
#include "ComboBoxBrowseCtrl.h"

typedef CComboBoxBrowseCtrl	CSearchComboBoxBase;

class QUICKFIND_EXT_CLASS CSearchComboBox : public CSearchComboBoxBase
{
	DECLARE_DYNAMIC(CSearchComboBox)
public:
	CSearchComboBox();
	~CSearchComboBox();
public:
// Overrides
protected:
	void Init() override;

	std::unique_ptr<CComboBoxBrowseCtrlEdit> CreateEditControl() override;
protected:
	
	DECLARE_MESSAGE_MAP()
};

