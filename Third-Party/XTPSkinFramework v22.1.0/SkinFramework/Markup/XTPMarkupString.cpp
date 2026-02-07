// XTPMarkupString.cpp: implementation of the CXTPMarkupString class.
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

#include "StdAfx.h"

#include "Common/XTPTypeId.h"
#include "Common/XTPCasting.h"
#include "Common/XTPFramework.h"
#include "Common/XTPSystemHelpers.h"
#include "Common/XTPSynchro.h"
#include "Common/XTPApplication.h"
#include "Common/XTPSingleton.h"
#include "Common/XTPResourceManager.h"

#include "Markup/XTPMarkupObject.h"
#include "Markup/XTPMarkupString.h"

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

#ifdef _DEBUG
#	define new DEBUG_NEW
#	undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////
// CXTPMarkupString

IMPLEMENT_MARKUPCLASS(NULL, CXTPMarkupString, CXTPMarkupObject);

void CXTPMarkupString::RegisterMarkupClass()
{
}

CXTPMarkupString::CXTPMarkupString(LPCSTR lpszValue)
	: m_lpszValue(NULL)
	, m_nHash(0)
	, m_nCIHash(0)
{
	if (lpszValue != NULL)
	{
		Init(XTP_CT2CW(lpszValue), -1);
	}
}

CXTPMarkupString::CXTPMarkupString(LPCWSTR lpszValue, int nLength /*= -1*/, int nObjFlags /*= 0*/)
	: CXTPMarkupObject(nObjFlags)
	, m_lpszValue(NULL)
	, m_nHash(0)
	, m_nCIHash(0)
{
	if (lpszValue != NULL)
	{
		Init(lpszValue, nLength);
	}
}

void CXTPMarkupString::Init(LPCWSTR lpszValue, int nLength)
{
	m_nLength = !lpszValue ? 0 : nLength == -1 ? (int)wcslen(lpszValue) : nLength;

	m_lpszValue = new WCHAR[XTPToUIntChecked(m_nLength + 1)];

	if (!lpszValue)
	{
		m_lpszValue[0] = 0;
	}
	else if (nLength == -1)
	{
		memcpy(m_lpszValue, lpszValue, (m_nLength + 1) * sizeof(WCHAR));
	}
	else
	{
		memcpy(m_lpszValue, lpszValue, nLength * sizeof(WCHAR));
		m_lpszValue[nLength] = 0;
	}
}

void CXTPMarkupString::CalcHash()
{
	LPCWSTR key  = m_lpszValue;
	UINT nHash   = 0;
	UINT nCIHash = 0;
	while (L'\0' != *key)
	{
		WCHAR ch = *key++;
		nCIHash  = (nCIHash << 5) + nCIHash + XTP_CHARTOUPPERW(ch);
		nHash	= nCIHash + ch;
	}

	m_nHash   = nHash;
	m_nCIHash = nCIHash;
}

CXTPMarkupString* CXTPMarkupString::CreateValue(LPCSTR lpszString, int nLength)
{
	USES_CONVERSION;

	CXTPMarkupString* pString = CreateValue(A2W(lpszString), nLength);

	return pString;
}

CXTPMarkupString* CXTPMarkupString::CreateValue(LPCWSTR lpszString, int nLength)
{
	CXTPMarkupString* pValue = NULL;

	if (lpszString == 0 || nLength == 0 || lpszString[0] == 0)
	{
		pValue = new CXTPMarkupString(L"", 0);
	}
	else if (lpszString[1] != 0)
	{
		pValue = new CXTPMarkupString(lpszString, nLength);
	}
	else if (lpszString[0] == L' ')
	{
		pValue = new CXTPMarkupString(L" ", 1);
	}
	else if (lpszString[0] == L'\n')
	{
		pValue = new CXTPMarkupString(L"\n", 1);
	}
	else
	{
		pValue = new CXTPMarkupString(lpszString, nLength);
	}

	return pValue;
}

UINT CXTPMarkupString::GetIgnoreCaseHashKey() const
{
	if (0 == m_nCIHash)
	{
		const_cast<CXTPMarkupString*>(this)->CalcHash();
	}

	return m_nCIHash;
}

UINT CXTPMarkupString::GetHashKey() const
{
	if (0 == m_nHash)
	{
		const_cast<CXTPMarkupString*>(this)->CalcHash();
	}

	return m_nHash;
}

BOOL CXTPMarkupString::IsEqual(const CXTPMarkupString* pString,
							   BOOL bCaseSensitive /*= TRUE*/) const
{
	ASSERT(NULL != pString);

	if (bCaseSensitive ? GetHashKey() != pString->GetHashKey()
					   : GetIgnoreCaseHashKey() != pString->GetIgnoreCaseHashKey())
		return FALSE;

	if (m_nLength != pString->m_nLength)
		return FALSE;

	return (bCaseSensitive ? wcscmp(m_lpszValue, pString->m_lpszValue) == 0
						   : _wcsicmp(m_lpszValue, pString->m_lpszValue) == 0);
}

BOOL CXTPMarkupString::IsEqual(const CXTPMarkupObject* pObject) const
{
	if (!pObject)
		return FALSE;

	if (pObject->GetType() != MARKUP_TYPE(CXTPMarkupString))
		return FALSE;

	return IsEqual(static_cast<const CXTPMarkupString*>(pObject));
}

CXTPMarkupString::~CXTPMarkupString()
{
	SAFE_DELETE_AR(m_lpszValue);
}
