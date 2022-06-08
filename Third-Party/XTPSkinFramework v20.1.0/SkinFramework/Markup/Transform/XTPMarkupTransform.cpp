// XTPMarkupTransform.cpp: implementation of the CXTPMarkupTransform class.
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
#include "Markup/XTPMarkupInputElement.h"
#include "Markup/XTPMarkupUIElement.h"
#include "Markup/Transform/XTPMarkupTransform.h"
#include "Markup/Transform/XTPMarkupRenderTransform.h"

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

#ifdef _DEBUG
#	undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#	define new DEBUG_NEW
#endif

IMPLEMENT_MARKUPCLASS(NULL, CXTPMarkupTransform, CXTPMarkupObject);

CXTPMarkupTransform::CXTPMarkupTransform()
	: m_pRenderTransform(NULL)
{
}

void CXTPMarkupTransform::RegisterMarkupClass()
{
}

CXTPMarkupTransformationMatrix*
	CXTPMarkupTransform::ApplyTransform(const CXTPMarkupTransformationMatrix* pMatrix)
{
	ASSERT(NULL != pMatrix);
	UNREFERENCED_PARAMETER(pMatrix);
	return NULL;
}

void CXTPMarkupTransform::OnPropertyChanged(CXTPMarkupDependencyProperty* pProperty,
											CXTPMarkupObject* pOldValue,
											CXTPMarkupObject* pNewValue)
{
	if (NULL != m_pRenderTransform)
	{
		CXTPMarkupObject* pTargetObject = m_pRenderTransform->GetTargetObject();
		if (NULL != pTargetObject && pTargetObject->IsKindOf(MARKUP_TYPE(CXTPMarkupUIElement)))
		{
			CXTPMarkupUIElement* pUIElement = MARKUP_STATICCAST(CXTPMarkupUIElement, pTargetObject);
			pUIElement->InvalidateMeasure();
		}
	}

	CXTPMarkupObject::OnPropertyChanged(pProperty, pOldValue, pNewValue);
}

void CXTPMarkupTransform::OnSetAsProperty(CXTPMarkupDependencyProperty* pProperty,
										  CXTPMarkupObject* pTargetObject)
{
	UNREFERENCED_PARAMETER(pProperty);

	ASSERT(NULL != pTargetObject);
	ASSERT(pTargetObject->IsKindOf(MARKUP_TYPE(CXTPMarkupRenderTransform)));
	m_pRenderTransform = MARKUP_STATICCAST(CXTPMarkupRenderTransform, pTargetObject);
}
