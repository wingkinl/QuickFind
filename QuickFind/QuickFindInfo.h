#pragma once

class QUICKFIND_INFO
{
public:
	enum Flags {
		FlagsInitShowReplace		= 0x00000001,
		FlagsInitShowOptions		= 0x00000002,
		FlagsShowScope				= 0x00000004,
		FlagsMatchCase				= 0x00000100,
		FlagsMatchWord				= 0x00000200,
		FlagsUseRegEx				= 0x00000400,
		FlagsFindReplacePrevious	= 0x00000800,
		FlagsNotifyFindTextChange	= 0x00010000,
		FlagsFloat					= 0x10000000,
		FlagsDockLeft				= 0x01000000,
		FlagsDockTop				= 0x02000000,
		FlagsDockRight				= 0x04000000,
		FlagsDockBottom				= 0x08000000,
		FlagsDockMask				= 0x0f000000,
	};
	DWORD			dwFlags;
	CStringArray	saSearch;
	CStringArray	saReplace;
	INT_PTR			nMaxItems;

	QUICKFIND_INFO()
	{
		dwFlags = FlagsInitShowOptions | FlagsNotifyFindTextChange;
		dwFlags |= FlagsDockTop | FlagsDockRight;
		nMaxItems = 20;
	}

	QUICKFIND_INFO& operator=(const QUICKFIND_INFO& rhs)
	{
		dwFlags = rhs.dwFlags;
		saSearch.Copy(rhs.saSearch);
		saReplace.Copy(rhs.saReplace);
		if (saSearch.GetSize() > nMaxItems)
			saSearch.SetSize(nMaxItems);
		if (saReplace.GetSize() > nMaxItems)
			saReplace.SetSize(nMaxItems);
		return *this;
	}

	inline BOOL IsInitShowAsReplace() const
	{
		return (dwFlags & FlagsInitShowReplace) != 0;
	}

	inline BOOL IsInitShowOptions() const
	{
		return (dwFlags & FlagsInitShowOptions) != 0;
	}

	inline BOOL IsShowScope() const
	{
		return (dwFlags & FlagsShowScope) != 0;
	}

	inline BOOL IsMatchCase() const
	{
		return (dwFlags & FlagsMatchCase) != 0;
	}

	inline BOOL IsMatchWholeWord() const
	{
		return (dwFlags & FlagsMatchWord) != 0;
	}

	inline BOOL IsUseRegEx() const
	{
		return (dwFlags & FlagsUseRegEx) != 0;
	}

	inline BOOL IsFindReplacePrevious() const
	{
		return (dwFlags & FlagsFindReplacePrevious) != 0;
	}

	inline BOOL IsFindReplaceNext() const
	{
		return !IsFindReplacePrevious();
	}

	inline BOOL IsNotifyFindTextChange() const
	{
		return (dwFlags & FlagsNotifyFindTextChange) != 0;
	}

	inline BOOL IsDocked() const
	{
		return (dwFlags & FlagsDockMask) != 0;
	}

	inline BOOL IsDockLeft() const
	{
		return (dwFlags & FlagsDockLeft) != 0;
	}

	inline BOOL IsDockTop() const
	{
		return (dwFlags & FlagsDockTop) != 0;
	}

	inline BOOL IsDockRight() const
	{
		return (dwFlags & FlagsDockRight) != 0;
	}

	inline BOOL IsDockBottom() const
	{
		return (dwFlags & FlagsDockBottom) != 0;
	}

	inline BOOL IsFloating() const
	{
		return (dwFlags & FlagsFloat) != 0;
	}

	void PromoteStringInArray(LPCTSTR pszText, BOOL bFindStringArray = TRUE)
	{
		CStringArray& sa = bFindStringArray ? saSearch : saReplace;
		PromoteStringInArray(sa, pszText);
		if (sa.GetSize() > nMaxItems)
			sa.SetSize(nMaxItems);
	}
protected:
	static void PromoteStringInArray(CStringArray& sa, LPCTSTR pszTextToMerge)
	{
		INT_PTR ii;
		for (ii = 0; ii < sa.GetSize(); ++ii)
		{
			if (sa[ii].Compare(pszTextToMerge) == 0)
			{
				if (ii)
					sa.RemoveAt(ii);
				break;
			}
		}
		if (ii || sa.GetSize() == 0)
			sa.InsertAt(0, pszTextToMerge);
	}
};
