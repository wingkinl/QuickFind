#pragma once
#ifdef _ENABLE_SCINTILLA_BUILD
#include "ScintillaWrapper\ScintillaDocView.h"
// CQuickFindScintillaDoc document

typedef CScintillaDoc	CQuickFindScintillaDocBase;

class CQuickFindScintillaDoc : public CQuickFindScintillaDocBase
{
	DECLARE_DYNCREATE(CQuickFindScintillaDoc)

public:
	CQuickFindScintillaDoc();
	virtual ~CQuickFindScintillaDoc();
#ifndef _WIN32_WCE
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
#endif
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual BOOL OnNewDocument();

	DECLARE_MESSAGE_MAP()
};

#endif // _ENABLE_SCINTILLA_BUILD