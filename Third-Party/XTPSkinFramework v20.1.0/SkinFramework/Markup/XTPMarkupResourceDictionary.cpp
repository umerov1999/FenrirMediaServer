// XTPMarkupResourceDictionary.cpp: implementation of the CXTPMarkupResourceDictionary class.
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

#include "StdAfx.h"

#include "Common/XTPFramework.h"

#include "Markup/XTPMarkupObject.h"
#include "Markup/XTPMarkupStyle.h"
#include "Markup/XTPMarkupString.h"
#include "Markup/XTPMarkupInputElement.h"
#include "Markup/XTPMarkupUIElement.h"
#include "Markup/XTPMarkupFrameworkElement.h"
#include "Markup/XTPMarkupResourceDictionary.h"
#include "Markup/XTPMarkupBuilder.h"

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

#ifdef _DEBUG
#	undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#	define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////////
// CXTPMarkupResourceDictionary

IMPLEMENT_MARKUPCLASS(L"ResourceDictionary", CXTPMarkupResourceDictionary, CXTPMarkupObject)

void CXTPMarkupResourceDictionary::RegisterMarkupClass()
{
}

CXTPMarkupResourceDictionary::CXTPMarkupResourceDictionary()
{
}

CXTPMarkupResourceDictionary::~CXTPMarkupResourceDictionary()
{
	CXTPMarkupObject* pKey;
	CXTPMarkupObject* pValue;

	POSITION pos = m_mapDictionary.GetStartPosition();
	while (pos)
	{
		m_mapDictionary.GetNextAssoc(pos, pKey, pValue);

		MARKUP_RELEASE(pKey);
		MARKUP_RELEASE(pValue);
	}
}

CXTPMarkupObject* CXTPMarkupResourceDictionary::operator[](LPCWSTR lpszKey) const
{
	return Lookup(lpszKey);
}

CXTPMarkupObject* CXTPMarkupResourceDictionary::Lookup(LPCWSTR lpszKey) const
{
	CXTPMarkupString* pKey	 = CXTPMarkupString::CreateValue(lpszKey);
	CXTPMarkupObject* pValue = Lookup(pKey);
	MARKUP_RELEASE(pKey);
	return pValue;
}

CXTPMarkupObject* CXTPMarkupResourceDictionary::Lookup(const CXTPMarkupObject* pKey) const
{
	CXTPMarkupObject* pValue;

	if (m_mapDictionary.Lookup((CXTPMarkupObject*)pKey, pValue))
		return pValue;

	return NULL;
}

void CXTPMarkupResourceDictionary::Add(CXTPMarkupObject* pKey, CXTPMarkupObject* pValue)
{
	m_mapDictionary.SetAt(pKey, pValue);
}

CXTPMarkupObject* CXTPMarkupResourceDictionary::GetObjectKey(CXTPMarkupObject* pContent) const
{
	if (!pContent)
		return NULL;

	CXTPMarkupObject* pKey = pContent->GetValue(m_pKeyProperty);
	if (pKey)
		return pKey;

	if (MARKUP_DYNAMICCAST(CXTPMarkupStyle, pContent))
	{
		return pContent->GetValue(CXTPMarkupStyle::m_pTargetTypeProperty);
	}

	return NULL;
}

void CXTPMarkupResourceDictionary::SetContentObject(CXTPMarkupBuilder* pBuilder,
													CXTPMarkupObject* pContent)
{
	ASSERT(NULL != pContent);

	CXTPMarkupObject* pKey = GetObjectKey(pContent);

	if (pKey == NULL)
	{
		pBuilder->ThrowBuilderException(_T("Objects added to Dictionary must have Key attribute ")
										_T("or some other type of associated Key"));
	}

	if (Lookup(pKey))
	{
		pBuilder->ThrowBuilderException(
			_T("Dictionary key is already used. Key attributes must be unique."));
	}

	Add(pKey, pContent);
	MARKUP_ADDREF(pKey);
	MARKUP_ADDREF(pContent);
}

CXTPMarkupObject* CXTPMarkupResourceDictionary::FindResource(const CXTPMarkupObject* pElement,
															 const CXTPMarkupObject* pKey)
{
	if (pElement->IsKindOf(MARKUP_TYPE(CXTPMarkupResourceDictionary)))
	{
		return ((CXTPMarkupResourceDictionary*)pElement)->Lookup(pKey);
	}

	while (pElement)
	{
		CXTPMarkupResourceDictionary* pResources =
			MARKUP_STATICCAST(CXTPMarkupResourceDictionary,
							  pElement->GetValue(CXTPMarkupStyle::m_pResourcesProperty));

		if (pResources)
		{
			CXTPMarkupObject* pValue = pResources->Lookup(pKey);
			if (pValue)
				return pValue;
		}

		CXTPMarkupStyle* pStyle = MARKUP_STATICCAST(
			CXTPMarkupStyle, pElement->GetValue(CXTPMarkupFrameworkElement::m_pStyleProperty));
		if (pStyle)
		{
			CXTPMarkupObject* pValue = pStyle->FindResource(pKey);
			if (pValue)
				return pValue;
		}

		if (pElement->GetType() == MARKUP_TYPE(CXTPMarkupStyle))
			return NULL;

		pElement = pElement->GetLogicalParent();
	}

	return NULL;
}
