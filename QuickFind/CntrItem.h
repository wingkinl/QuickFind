
// CntrItem.h : interface of the CQuickFindCntrItem class
//

#pragma once

class CQuickFindDoc;
class CQuickFindView;

class CQuickFindCntrItem : public CRichEditCntrItem
{
	DECLARE_SERIAL(CQuickFindCntrItem)

// Constructors
public:
	CQuickFindCntrItem(REOBJECT* preo = NULL, CQuickFindDoc* pContainer = NULL);
		// Note: pContainer is allowed to be NULL to enable IMPLEMENT_SERIALIZE
		//  IMPLEMENT_SERIALIZE requires the class have a constructor with
		//  zero arguments.  Normally, OLE items are constructed with a
		//  non-NULL document pointer

// Attributes
public:
	CQuickFindDoc* GetDocument()
		{ return reinterpret_cast<CQuickFindDoc*>(CRichEditCntrItem::GetDocument()); }
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

