#pragma once
#ifdef _ENABLE_SCINTILLA_BUILD
#include "QuickFindView.inl"
#include "ScintillaWrapper\ScintillaDocView.h"

// CQuickFindScintillaView view

typedef CQuickFindView<CScintillaView>	CQuickFindScintillaViewBase;

class CQuickFindScintillaView : public CQuickFindScintillaViewBase
{
	DECLARE_DYNCREATE(CQuickFindScintillaView)

protected:
	CQuickFindScintillaView();           // protected constructor used by dynamic creation
	virtual ~CQuickFindScintillaView();

public:
	void SetText(LPCTSTR pszText) override;
	CString GetSelectedOrCursorText() const override;
	BOOL CanDoReplace() const override;

	BOOL OnFind(CQuickFindWndDemo* pQuickFindWnd) override;
	BOOL OnFindAll(CQuickFindWndDemo* pQuickFindWnd) override;
	BOOL OnIncrementalFind(CQuickFindWndDemo* pQuickFindWnd) override;

	BOOL OnReplace(CQuickFindWndDemo* pQuickFindWnd) override;
	BOOL OnReplaceAll(CQuickFindWndDemo* pQuickFindWnd) override;

	virtual BOOL FindTextSimple(LPCTSTR lpszFind, BOOL bNext, DWORD dwSCFlags, const Sci_CharacterRange* pCharRange = nullptr);
	virtual BOOL SameAsSelected(LPCTSTR lpszCompare, DWORD dwSCFlags);
	virtual Sci_Position FindAndSelect(Sci_TextToFind& ft, DWORD dwSCFlags, Sci_Position nActualStartPos);

#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	DECLARE_MESSAGE_MAP()
};

#endif // _ENABLE_SCINTILLA_BUILD
