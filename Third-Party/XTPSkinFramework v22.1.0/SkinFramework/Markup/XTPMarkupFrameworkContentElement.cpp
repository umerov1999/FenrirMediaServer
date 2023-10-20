// XTPMarkupFrameworkContentElement.cpp: implementation of the CXTPMarkupFrameworkContentElement
// class.
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
#include "Markup/XTPMarkupInputElement.h"
#include "Markup/XTPMarkupUIElement.h"
#include "Markup/XTPMarkupFrameworkElement.h"
#include "Markup/XTPMarkupFrameworkContentElement.h"

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

#ifdef _DEBUG
#	undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#	define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////////
// CXTPMarkupFrameworkContentElement

CXTPMarkupDependencyProperty* CXTPMarkupFrameworkContentElement::m_pTagProperty = NULL;

IMPLEMENT_MARKUPCLASS(L"FrameworkContentElement", CXTPMarkupFrameworkContentElement,
					  CXTPMarkupInputElement);

void CXTPMarkupFrameworkContentElement::RegisterMarkupClass()
{
	CXTPMarkupFrameworkElement::RegisterType();

	m_pTagProperty = CXTPMarkupFrameworkElement::m_pTagProperty->AddOwner(
		MARKUP_TYPE(CXTPMarkupFrameworkContentElement));
}

CXTPMarkupFrameworkContentElement::CXTPMarkupFrameworkContentElement()
{
}

void CXTPMarkupFrameworkContentElement::OnPropertyChanged(CXTPMarkupDependencyProperty* pProperty,
														  CXTPMarkupObject* pOldValue,
														  CXTPMarkupObject* pNewValue)
{
	CXTPMarkupInputElement::OnPropertyChanged(pProperty, pOldValue, pNewValue);

	FireTriggers(pProperty, pNewValue);

	if (pProperty->GetFlags() & CXTPMarkupPropertyMetadata::flagAffectsMeasure)
	{
		CXTPMarkupFrameworkElement* pParent = GetParent();
		if (pParent)
			pParent->InvalidateMeasure();
	}

	if (pProperty->GetFlags() & CXTPMarkupPropertyMetadata::flagAffectsArrange)
	{
		CXTPMarkupFrameworkElement* pParent = GetParent();
		if (pParent)
			pParent->InvalidateArrange();
	}

	if (pProperty->GetFlags() & CXTPMarkupPropertyMetadata::flagAffectsRender)
	{
		CXTPMarkupFrameworkElement* pParent = GetParent();
		if (pParent)
			pParent->InvalidateVisual();
	}
}

CXTPMarkupFrameworkElement* CXTPMarkupFrameworkContentElement::GetParent() const
{
	CXTPMarkupObject* pParent = m_pLogicalParent;
	while (pParent)
	{
		if (pParent->IsKindOf(MARKUP_TYPE(CXTPMarkupFrameworkElement)))
			return (CXTPMarkupFrameworkElement*)pParent;

		pParent = pParent->GetLogicalParent();
	}

	return NULL;
}

CXTPMarkupInputElement* CXTPMarkupFrameworkContentElement::InputHitTest(CPoint /*point*/) const
{
	return (CXTPMarkupInputElement*)this;
}

CRect CXTPMarkupFrameworkContentElement::GetBoundRect() const
{
	return GetParent()->GetBoundRect();
}

CRect CXTPMarkupFrameworkContentElement::GetUpdateRect() const
{
	return GetParent()->GetUpdateRect();
}
