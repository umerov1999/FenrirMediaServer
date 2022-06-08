// XTPMarkupResourceDictionary.h: interface for the CXTPMarkupResourceDictionary class.
//
// (c)1998-2021 Codejock Software, All Rights Reserved.
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
#if !defined(__XTPMARKUPRESOURCEDICTIONARY_H__)
#	define __XTPMARKUPRESOURCEDICTIONARY_H__
//}}AFX_CODEJOCK_PRIVATE

#	if _MSC_VER > 1000
#		pragma once
#	endif // _MSC_VER > 1000

#	include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

class CXTPMarkupObject;
class CXTPMarkupBuilder;

class _XTP_EXT_CLASS CXTPMarkupResourceDictionary : public CXTPMarkupObject
{
	DECLARE_MARKUPCLASS(CXTPMarkupResourceDictionary);

public:
	CXTPMarkupResourceDictionary();
	virtual ~CXTPMarkupResourceDictionary();

public:
	void Add(CXTPMarkupObject* pKey, CXTPMarkupObject* pValue);

	CXTPMarkupObject* operator[](LPCWSTR lpszKey) const;
	CXTPMarkupObject* Lookup(LPCWSTR lpszKey) const;
	CXTPMarkupObject* Lookup(const CXTPMarkupObject* pKey) const;

	static CXTPMarkupObject* AFX_CDECL FindResource(const CXTPMarkupObject* pElement,
													const CXTPMarkupObject* pKey);

protected:
	void SetContentObject(CXTPMarkupBuilder* pBuilder, CXTPMarkupObject* pContent);
	CXTPMarkupObject* GetObjectKey(CXTPMarkupObject* pContent) const;

protected:
	CMap<CXTPMarkupObjectPtr, CXTPMarkupObjectPtr, CXTPMarkupObjectPtr, CXTPMarkupObjectPtr>
		m_mapDictionary;
};

#	include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPMARKUPRESOURCEDICTIONARY_H__)
