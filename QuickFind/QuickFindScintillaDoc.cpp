// QuickFindScintillaDoc.cpp : implementation file
//

#include "stdafx.h"
#ifdef _ENABLE_SCINTILLA_BUILD
#include "QuickFind.h"
#include "QuickFindScintillaDoc.h"


// CQuickFindScintillaDoc

IMPLEMENT_DYNCREATE(CQuickFindScintillaDoc, CQuickFindScintillaDocBase)

CQuickFindScintillaDoc::CQuickFindScintillaDoc()
{
}

BOOL CQuickFindScintillaDoc::OnNewDocument()
{
	if (!CQuickFindScintillaDocBase::OnNewDocument())
		return FALSE;
	return TRUE;
}

CQuickFindScintillaDoc::~CQuickFindScintillaDoc()
{
}


BEGIN_MESSAGE_MAP(CQuickFindScintillaDoc, CQuickFindScintillaDocBase)
END_MESSAGE_MAP()


// CQuickFindScintillaDoc diagnostics

#ifdef _DEBUG
void CQuickFindScintillaDoc::AssertValid() const
{
	CQuickFindScintillaDocBase::AssertValid();
}

#ifndef _WIN32_WCE
void CQuickFindScintillaDoc::Dump(CDumpContext& dc) const
{
	CQuickFindScintillaDocBase::Dump(dc);
}
#endif
#endif //_DEBUG

#ifndef _WIN32_WCE
// CQuickFindScintillaDoc serialization

void CQuickFindScintillaDoc::Serialize(CArchive& ar)
{
	CQuickFindScintillaDocBase::Serialize(ar);
}
#endif


// CQuickFindScintillaDoc commands


#endif // _ENABLE_SCINTILLA_BUILD