// XTPMarkupString.h: interface for the CXTPMarkupString class.
//
// (c)1998-2023 Codejock Software, All Rights Reserved.
//
// THIS SOURCE FILE IS THE PROPERTY OF CODEJOCK SOFTWARE AND IS NOT TO BE
// RE-DISTRIBUTED BY ANY MEANS WHATSOEVER WITHOUT THE EXPRESSED WRITTEN
// CONSENT OF CODEJOCK SOFTWARE.
//
// THIS SOURCE CODE CAN ONLY BE USED UNDER THE TERMS AND CONDITIONS OUTLINED
// IN THE XTREME TOOLKIT PRO LICENSE AGREEMENT. CODEJOCK SOFTWARE GRANTS TO
// YOU (ONE SOFTWARE DEVELOPER) THE LIMITED RIGHT TO USE THIS SOFTWARE ON A
// SINGLE COMPUTER.
//
// CONTACT INFORMATION:
// support@codejock.com
// http://www.codejock.com
//
/////////////////////////////////////////////////////////////////////////////

//{{AFX_CODEJOCK_PRIVATE
#if !defined(__XTPMARKUPSTRING_H__)
#	define __XTPMARKUPSTRING_H__
//}}AFX_CODEJOCK_PRIVATE

#	if _MSC_VER > 1000
#		pragma once
#	endif // _MSC_VER > 1000

#	include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

class CXTPMarkupObject;

class _XTP_EXT_CLASS CXTPMarkupString : public CXTPMarkupObject
{
	DECLARE_MARKUPCLASS(CXTPMarkupString);

public:
	CXTPMarkupString(LPCSTR lpszValue);
	CXTPMarkupString(LPCWSTR lpszValue = NULL, int nLength = -1, int nObjFlags = 0);

	virtual ~CXTPMarkupString();

public:
	operator LPCWSTR() const;
	int GetLength() const;
	UINT GetIgnoreCaseHashKey() const;
	BOOL IsEqual(const CXTPMarkupString* pString, BOOL bCaseSensitive = TRUE) const;

public:
	static CXTPMarkupString* AFX_CDECL CreateValue(LPCSTR lpszString, int nLength = -1);
	static CXTPMarkupString* AFX_CDECL CreateValue(LPCWSTR lpszString, int nLength = -1);

public:
	virtual UINT GetHashKey() const;
	virtual BOOL IsEqual(const CXTPMarkupObject* pObject) const;

private:
	void Init(LPCWSTR lpszValue, int nLength);
	void CalcHash();

protected:
	LPWSTR m_lpszValue;
	int m_nLength;
	UINT m_nHash;
	UINT m_nCIHash;
};

AFX_INLINE CXTPMarkupString::operator LPCWSTR() const
{
	return m_lpszValue;
}

AFX_INLINE int CXTPMarkupString::GetLength() const
{
	return m_nLength;
}

AFX_INLINE BOOL IsStringObject(CXTPMarkupObject* pObject)
{
	return pObject && pObject->GetType() == MARKUP_TYPE(CXTPMarkupString);
}

#	include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPMARKUPSTRING_H__)
