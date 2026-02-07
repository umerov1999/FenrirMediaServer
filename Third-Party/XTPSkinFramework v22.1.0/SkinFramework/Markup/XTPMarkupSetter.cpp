// XTPMarkupSetter.cpp: implementation of the CXTPMarkupSetter class.
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

#include "Markup/XTPMarkupObject.h"
#include "Markup/XTPMarkupSetter.h"

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

#ifdef _DEBUG
#	define new DEBUG_NEW
#	undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////
// CXTPMarkupSetter

CXTPMarkupDependencyProperty* CXTPMarkupSetter::m_pPropertyProperty = NULL;
CXTPMarkupDependencyProperty* CXTPMarkupSetter::m_pValueProperty	= NULL;

IMPLEMENT_MARKUPCLASS(L"Setter", CXTPMarkupSetter, CXTPMarkupObject);

void CXTPMarkupSetter::RegisterMarkupClass()
{
	m_pPropertyProperty = CXTPMarkupDependencyProperty::Register(L"Property",
																 MARKUP_TYPE(CXTPMarkupObject),
																 MARKUP_TYPE(CXTPMarkupSetter));
	m_pValueProperty	= CXTPMarkupDependencyProperty::Register(L"Value",
																 MARKUP_TYPE(CXTPMarkupObject),
																 MARKUP_TYPE(CXTPMarkupSetter));
}

CXTPMarkupSetter::CXTPMarkupSetter()
{
}

CXTPMarkupSetter::CXTPMarkupSetter(CXTPMarkupDependencyProperty* pProperty,
								   CXTPMarkupObject* pValue)
{
	ASSERT(pProperty && pValue);

	pProperty->AddRef();
	SetValue(m_pPropertyProperty, pProperty);
	SetValue(m_pValueProperty, pValue);
}

CXTPMarkupDependencyProperty* CXTPMarkupSetter::GetSetterProperty() const
{
	return MARKUP_STATICCAST(CXTPMarkupDependencyProperty, GetValue(m_pPropertyProperty));
}

CXTPMarkupObject* CXTPMarkupSetter::GetSetterValue() const
{
	return CXTPMarkupObject::GetValue(m_pValueProperty);
}
