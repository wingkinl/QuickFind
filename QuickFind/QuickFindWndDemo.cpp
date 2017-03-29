#include "stdafx.h"
#include "QuickFindWndDemo.h"

IMPLEMENT_DYNAMIC(CQuickFindWndDemo, CQuickFindWndDemoBase)

CQuickFindWndDemo::CQuickFindWndDemo(void)
{
	m_bPOSIXRegEx = FALSE;
	m_nSelBegin = m_nSelEnd = 0;
}


CQuickFindWndDemo::~CQuickFindWndDemo(void)
{
}

BEGIN_MESSAGE_MAP(CQuickFindWndDemo, CQuickFindWndDemoBase)
END_MESSAGE_MAP()

BOOL CQuickFindWndDemo::OnInitDialog()
{
	BOOL bRet = CQuickFindWndDemoBase::OnInitDialog();
	CStringArray saScopes;
	saScopes.SetSize(FindScopeSize);
	saScopes[FindScopeSelection] = _T("Selection");
	saScopes[FindScopeCurDoc] = _T("Current Document");
	SetScopeItems(saScopes, FindScopeCurDoc);
	return bRet;
}

auto CQuickFindWndDemo::GetCurScope() const -> FindScope
{
	return (FindScope)m_wndScope.GetCurSel();
}

void CQuickFindWndDemo::OnUpdateActiveStatus()
{
	if (m_bActive)
	{
		NotifyOwner(QuickFindCmdCustom, CustomCmdOnUpdateActiveStatus);
	}
	CQuickFindWndDemoBase::OnUpdateActiveStatus();
}

void CQuickFindWndDemo::UpdateScope(LONG nSelStart, LONG nSelEnd, FindScope scope)
{
	m_nSelBegin = nSelStart;
	m_nSelEnd = nSelEnd;
	if (m_nSelBegin == m_nSelEnd && scope == FindScopeSelection)
		scope = FindScopeCurDoc;
	m_wndScope.SetCurSel(scope);
}
