#pragma once
#include "quickfindwnd.h"

typedef CQuickFindWnd	CQuickFindWndDemoBase;

class CQuickFindWndDemo : public CQuickFindWndDemoBase
{
	DECLARE_DYNAMIC(CQuickFindWndDemo)
public:
	CQuickFindWndDemo();
	~CQuickFindWndDemo();
public:
	enum FindScope {
		FindScopeSelection,
		FindScopeCurDoc,

		FindScopeSize,
		// do not append!
	};

	LONG			m_nSelBegin;
	LONG			m_nSelEnd;

	enum CustomCmd {
		CustomCmdOnUpdateActiveStatus,

		CustomCmdLast,
	};
public:
	BOOL OnInitDialog() override;

	FindScope GetCurScope() const;

	void UpdateScope(LONG nSelStart, LONG nSelEnd, FindScope scope);
protected:
	void OnUpdateActiveStatus() override;

	DECLARE_MESSAGE_MAP()
};

