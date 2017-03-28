#pragma once

class CQuickFindCntrItem;

// CQuickFindRichEditView view

typedef CRichEditView	CQuickFindRichEditViewBase;

class CQuickFindRichEditView : public CQuickFindRichEditViewBase
{
	DECLARE_DYNCREATE(CQuickFindRichEditView)

protected:
	CQuickFindRichEditView();           // protected constructor used by dynamic creation
	virtual ~CQuickFindRichEditView();
public:
	CQuickFindDoc* GetDocument() const;
public:
protected:
	virtual void OnInitialUpdate(); // called first time after construct

#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

	afx_msg void OnDestroy();
protected:
	DECLARE_MESSAGE_MAP()
};


