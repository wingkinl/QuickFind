
// CntrItem.cpp : implementation of the CQuickFindCntrItem class
//

#include "stdafx.h"
#include "QuickFind.h"

#include "QuickFindRichEditDoc.h"
#include "QuickFindView.h"
#include "CntrItem.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CQuickFindCntrItem implementation

IMPLEMENT_SERIAL(CQuickFindCntrItem, CRichEditCntrItem, 0)

CQuickFindCntrItem::CQuickFindCntrItem(REOBJECT* preo, CQuickFindRichEditDoc* pContainer)
	: CRichEditCntrItem(preo, pContainer)
{
	// TODO: add one-time construction code here
}

CQuickFindCntrItem::~CQuickFindCntrItem()
{
	// TODO: add cleanup code here
}


// CQuickFindCntrItem diagnostics

#ifdef _DEBUG
void CQuickFindCntrItem::AssertValid() const
{
	CRichEditCntrItem::AssertValid();
}

void CQuickFindCntrItem::Dump(CDumpContext& dc) const
{
	CRichEditCntrItem::Dump(dc);
}
#endif

