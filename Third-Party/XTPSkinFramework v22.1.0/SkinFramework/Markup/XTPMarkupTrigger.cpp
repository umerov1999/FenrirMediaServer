// XTPMarkupTrigger.cpp: implementation of the CXTPMarkupTrigger class.
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
#include "Markup/XTPMarkupTrigger.h"
#include "Markup/XTPMarkupSetter.h"
#include "Markup/XTPMarkupSetterCollection.h"

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

#ifdef _DEBUG
#	define new DEBUG_NEW
#	undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////
// CXTPMarkupTrigger

CXTPMarkupDependencyProperty* CXTPMarkupTrigger::m_pPropertyProperty = NULL;
CXTPMarkupDependencyProperty* CXTPMarkupTrigger::m_pSettersProperty  = NULL;
CXTPMarkupDependencyProperty* CXTPMarkupTrigger::m_pValueProperty	= NULL;

IMPLEMENT_MARKUPCLASS(L"Trigger", CXTPMarkupTrigger, CXTPMarkupObject)

void CXTPMarkupTrigger::RegisterMarkupClass()
{
	CXTPMarkupSetter::RegisterType();

	m_pPropertyProperty = CXTPMarkupSetter::m_pPropertyProperty->AddOwner(
		MARKUP_TYPE(CXTPMarkupTrigger));
	m_pSettersProperty = CXTPMarkupDependencyProperty::Register(
		L"Setters", MARKUP_TYPE(CXTPMarkupSetterCollection), MARKUP_TYPE(CXTPMarkupTrigger));
	m_pValueProperty = CXTPMarkupSetter::m_pValueProperty->AddOwner(MARKUP_TYPE(CXTPMarkupTrigger));
}

CXTPMarkupTrigger::CXTPMarkupTrigger()
{
}

CXTPMarkupTrigger::CXTPMarkupTrigger(CXTPMarkupDependencyProperty* pProperty,
									 CXTPMarkupObject* pValue)
{
	ASSERT(pProperty && pValue);

	pProperty->AddRef();
	SetValue(m_pPropertyProperty, pProperty);
	SetValue(m_pValueProperty, pValue);
}

CXTPMarkupSetterCollection* CXTPMarkupTrigger::GetSetters() const
{
	return MARKUP_STATICCAST(CXTPMarkupSetterCollection, GetValue(m_pSettersProperty));
}

CXTPMarkupDependencyProperty* CXTPMarkupTrigger::GetTriggerProperty() const
{
	return MARKUP_STATICCAST(CXTPMarkupDependencyProperty, GetValue(m_pPropertyProperty));
}

void CXTPMarkupTrigger::SetSetters(CXTPMarkupSetterCollection* pSetters)
{
	SetValue(m_pSettersProperty, pSetters);
}

CXTPMarkupObject* CXTPMarkupTrigger::GetTriggerValue() const
{
	return CXTPMarkupObject::GetValue(m_pValueProperty);
}
