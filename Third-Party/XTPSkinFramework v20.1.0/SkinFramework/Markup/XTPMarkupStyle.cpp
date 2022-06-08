// XTPMarkupStyle.cpp: implementation of the CXTPMarkupStyle class.
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
#include "Markup/XTPMarkupString.h"
#include "Markup/XTPMarkupStyle.h"
#include "Markup/XTPMarkupSetter.h"
#include "Markup/XTPMarkupSetterCollection.h"
#include "Markup/XTPMarkupTrigger.h"
#include "Markup/XTPMarkupTriggerCollection.h"
#include "Markup/XTPMarkupResourceDictionary.h"
#include "Markup/XTPMarkupBuilder.h"

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

#ifdef _DEBUG
#	define new DEBUG_NEW
#	undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////
// CXTPMarkupStyle

CXTPMarkupDependencyProperty* CXTPMarkupStyle::m_pTargetTypeProperty = NULL;
CXTPMarkupDependencyProperty* CXTPMarkupStyle::m_pBasedOnProperty	 = NULL;
CXTPMarkupDependencyProperty* CXTPMarkupStyle::m_pResourcesProperty	 = NULL;
CXTPMarkupDependencyProperty* CXTPMarkupStyle::m_pTriggersProperty	 = NULL;

IMPLEMENT_MARKUPCLASS(L"Style", CXTPMarkupStyle, CXTPMarkupObject)

void CXTPMarkupStyle::RegisterMarkupClass()
{
	CXTPMarkupSetter::RegisterType();
	CXTPMarkupTrigger::RegisterType();

	m_pResourcesProperty = CXTPMarkupDependencyProperty::Register(
		L"Resources", MARKUP_TYPE(CXTPMarkupResourceDictionary), MARKUP_TYPE(CXTPMarkupStyle));

	m_pTargetTypeProperty = CXTPMarkupDependencyProperty::Register(L"TargetType",
																   MARKUP_TYPE(CXTPMarkupType),
																   MARKUP_TYPE(CXTPMarkupStyle));
	m_pBasedOnProperty	  = CXTPMarkupDependencyProperty::Register(L"BasedOn",
																   MARKUP_TYPE(CXTPMarkupStyle),
																   MARKUP_TYPE(CXTPMarkupStyle));

	m_pTriggersProperty = CXTPMarkupDependencyProperty::Register(
		L"Triggers", MARKUP_TYPE(CXTPMarkupTriggerCollection), MARKUP_TYPE(CXTPMarkupStyle));
}

CXTPMarkupStyle::CXTPMarkupStyle()
{
	m_pSetters = new CXTPMarkupSetterCollection();
	m_pSetters->SetLogicalParent(this);

	m_bSealed = FALSE;

	m_pProperties = NULL;
}

CXTPMarkupStyle::~CXTPMarkupStyle()
{
	if (m_pSetters)
	{
		m_pSetters->SetLogicalParent(NULL);
		MARKUP_RELEASE(m_pSetters);
	}

	MARKUP_RELEASE(m_pProperties);
}

void CXTPMarkupStyle::ResolveTriggerProperty(CXTPMarkupBuilder* pBuilder,
											 CXTPMarkupTrigger* pTrigger)
{
	ResolveSetterProperty(pBuilder, pTrigger);

	CXTPMarkupSetterCollection* pSetters = pTrigger->GetSetters();
	int nCount							 = pSetters ? pSetters->GetCount() : 0;

	for (int i = 0; i < nCount; i++)
	{
		ResolveSetterProperty(pBuilder, pSetters->GetItem(i));
	}
}

void CXTPMarkupStyle::ResolveSetterProperty(CXTPMarkupBuilder* pBuilder, CXTPMarkupObject* pSetter)
{
	CXTPMarkupType* pTargetType = MARKUP_STATICCAST(CXTPMarkupType,
													GetValue(m_pTargetTypeProperty));

	CXTPMarkupDependencyProperty* pProperty = (CXTPMarkupDependencyProperty*)pSetter->GetValue(
		CXTPMarkupSetter::m_pPropertyProperty);
	if (!pProperty)
	{
		pBuilder->ThrowBuilderException(_T("Must specify both Property and Value for Setter."));
	}

	if (pProperty->GetType() == MARKUP_TYPE(CXTPMarkupDependencyProperty))
	{
	}
	else if (IsStringObject(pProperty))
	{
		LPCWSTR lpszTagName = *((CXTPMarkupString*)pProperty);

		pProperty = pBuilder->FindProperty(pTargetType, lpszTagName);

		if (pProperty == NULL)
		{
			pBuilder->ThrowBuilderException(
				pBuilder->FormatString(_T("Cannot find the Style Property '%ls'"),
									   (LPCTSTR)lpszTagName));
		}

		pProperty->AddRef();
		pSetter->SetValue(CXTPMarkupSetter::m_pPropertyProperty, pProperty);
	}
	else
	{
		pBuilder->ThrowBuilderException(_T("Must specify both Property and Value for Setter."));
	}

	CXTPMarkupObject* pValue = pSetter->GetValue(CXTPMarkupSetter::m_pValueProperty);
	if (!pValue)
	{
		pBuilder->ThrowBuilderException(_T("Must specify both Property and Value for Setter."));
	}

	if (!pValue->IsKindOf(pProperty->GetPropetyType()))
	{
		CXTPMarkupObject* pNewValue = pBuilder->ConvertValue(pProperty, pValue);
		pSetter->SetValue(CXTPMarkupSetter::m_pValueProperty, pNewValue);
	}
}

void CXTPMarkupStyle::SetContentObject(CXTPMarkupBuilder* pBuilder, CXTPMarkupObject* pContent)
{
	if (pContent->IsKindOf(MARKUP_TYPE(CXTPMarkupSetter)))
	{
		ResolveSetterProperty(pBuilder, (CXTPMarkupSetter*)pContent);
		m_pSetters->SetContentObject(pBuilder, pContent);
	}
	else
	{
		CXTPMarkupObject::SetContentObject(pBuilder, pContent);
	}
}

void CXTPMarkupStyle::SetPropertyObject(CXTPMarkupBuilder* pBuilder,
										CXTPMarkupDependencyProperty* pProperty,
										CXTPMarkupObject* pValue)
{
	if (pProperty == m_pTriggersProperty)
	{
		if (pValue && pValue->IsKindOf(MARKUP_TYPE(CXTPMarkupTriggerCollection)))
		{
			CXTPMarkupTriggerCollection* pTriggers = (CXTPMarkupTriggerCollection*)pValue;
			for (int i = 0; i < pTriggers->GetCount(); i++)
			{
				ResolveTriggerProperty(pBuilder, pTriggers->GetItem(i));
			}
		}
	}

	CXTPMarkupObject::SetPropertyObject(pBuilder, pProperty, pValue);
}

void CXTPMarkupStyle::Seal()
{
	if (m_bSealed)
		return;

	ASSERT(!m_pProperties);

	if (!m_pProperties)
		m_pProperties = new CXTPMarkupProperties(NULL);

	SetLogicalParent(NULL);

	CXTPMarkupStyle* pStyle = GetBasedStyle();
	if (pStyle)
	{
		pStyle->Seal();

		m_pProperties->Copy(pStyle->m_pProperties);
	}

	int nCount = m_pSetters->GetCount();
	for (int i = 0; i < nCount; i++)
	{
		CXTPMarkupSetter* pSetter = m_pSetters->GetItem(i);

		CXTPMarkupObject* pValue = pSetter->GetSetterValue();
		MARKUP_ADDREF(pValue);

		m_pProperties->Set(pSetter->GetSetterProperty(), pValue);
	}

	m_bSealed = TRUE;
}

CXTPMarkupObject* CXTPMarkupStyle::GetStyleValue(CXTPMarkupDependencyProperty* pProperty) const
{
	return m_pProperties ? m_pProperties->Lookup(pProperty) : NULL;
}

void CXTPMarkupType::SetTypeStyle(CXTPMarkupStyle* pStyle)
{
	if (m_pTypeStyle)
	{
		m_pTypeStyle->Release();
	}

	m_pTypeStyle = pStyle;

	if (pStyle)
	{
		pStyle->Seal();
	}
}

CXTPMarkupObject* CXTPMarkupStyle::FindResource(const CXTPMarkupObject* pKey) const
{
	return CXTPMarkupResourceDictionary::FindResource(this, pKey);
}

CXTPMarkupStyle* CXTPMarkupStyle::GetBasedStyle() const
{
	return MARKUP_STATICCAST(CXTPMarkupStyle, GetValue(m_pBasedOnProperty));
}

void CXTPMarkupStyle::SetBasedStyle(CXTPMarkupStyle* pStyle)
{
	SetValue(m_pBasedOnProperty, pStyle);
}

void CXTPMarkupStyle::SetTriggers(CXTPMarkupTriggerCollection* pTriggers)
{
	SetValue(m_pTriggersProperty, pTriggers);
}

CXTPMarkupSetterCollection* CXTPMarkupStyle::GetSetters() const
{
	return m_pSetters;
}

CXTPMarkupTriggerCollection* CXTPMarkupStyle::GetTriggers() const
{
	return MARKUP_STATICCAST(CXTPMarkupTriggerCollection, GetValue(m_pTriggersProperty));
}
