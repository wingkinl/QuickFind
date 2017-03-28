
// CntrItem.h : interface of the CQuickFindCntrItem class
//

#pragma once

class CQuickFindRichEditDoc;
class CQuickFindView;

class CQuickFindCntrItem : public CRichEditCntrItem
{
	DECLARE_SERIAL(CQuickFindCntrItem)

// Constructors
public:
	CQuickFindCntrItem(REOBJECT* preo = NULL, CQuickFindRichEditDoc* pContainer = NULL);
		// Note: pContainer is allowed to be NULL to enable IMPLEMENT_SERIALIZE
		//  IMPLEMENT_SERIALIZE requires the class have a constructor with
		//  zero arguments.  Normally, OLE items are constructed with a
		//  non-NULL document pointer

// Attributes
public:
	CQuickFindRichEditDoc* GetDocument()
		{ return reinterpret_cast<CQuickFindRichEditDoc*>(CRichEditCntrItem::GetDocument()); }
	CQuickFindView* GetActiveView()
		{ return reinterpret_cast<CQuickFindView*>(CRichEditCntrItem::GetActiveView()); }

// Implementation
public:
	~CQuickFindCntrItem();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
};

