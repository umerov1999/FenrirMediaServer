// XTPMarkupRenderTransform.cpp: implementation of the CXTPMarkupRenderTransform class.
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
#include "Markup/XTPMarkupBuilder.h"
#include "Markup/Transform/XTPMarkupTransform.h"
#include "Markup/Transform/XTPMarkupRenderTransform.h"
#include "Markup/Transform/XTPMarkupRotateTransform.h"
#include "Markup/Transform/XTPMarkupScaleTransform.h"
#include "Markup/Transform/XTPMarkupTranslateTransform.h"
#include "Markup/Transform/XTPMarkupSkewTransform.h"

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

#ifdef _DEBUG
#	define new DEBUG_NEW
#	undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////
// CXTPMarkupRenderTransform

CXTPMarkupDependencyProperty* CXTPMarkupRenderTransform::m_pPropertyRotateTransform	   = NULL;
CXTPMarkupDependencyProperty* CXTPMarkupRenderTransform::m_pPropertyScaleTransform	   = NULL;
CXTPMarkupDependencyProperty* CXTPMarkupRenderTransform::m_pPropertyTranslateTransform = NULL;
CXTPMarkupDependencyProperty* CXTPMarkupRenderTransform::m_pPropertySkewTransform	   = NULL;

IMPLEMENT_MARKUPCLASS(L"RenderTransform", CXTPMarkupRenderTransform, CXTPMarkupObject);

CXTPMarkupRenderTransform::CXTPMarkupRenderTransform()
	: m_pContentProperty(NULL)
	, m_pTargetObject(NULL)
{
}

CXTPMarkupRenderTransform::~CXTPMarkupRenderTransform()
{
}

void CXTPMarkupRenderTransform::RegisterMarkupClass()
{
	m_pPropertyRotateTransform = CXTPMarkupDependencyProperty::Register(
		L"RotateTransform", MARKUP_TYPE(CXTPMarkupRotateTransform),
		MARKUP_TYPE(CXTPMarkupRenderTransform));

	m_pPropertyScaleTransform = CXTPMarkupDependencyProperty::Register(
		L"ScaleTransform", MARKUP_TYPE(CXTPMarkupScaleTransform),
		MARKUP_TYPE(CXTPMarkupRenderTransform));

	m_pPropertyTranslateTransform = CXTPMarkupDependencyProperty::Register(
		L"TranslateTransform", MARKUP_TYPE(CXTPMarkupTranslateTransform),
		MARKUP_TYPE(CXTPMarkupRenderTransform));

	m_pPropertySkewTransform = CXTPMarkupDependencyProperty::Register(
		L"SkewTransform", MARKUP_TYPE(CXTPMarkupSkewTransform),
		MARKUP_TYPE(CXTPMarkupRenderTransform));
}

void CXTPMarkupRenderTransform::SetContentObject(CXTPMarkupBuilder* pBuilder,
												 CXTPMarkupObject* pContent)
{
	ASSERT(NULL != pBuilder);
	ASSERT(NULL != pContent);

	if (NULL != m_pContentProperty)
	{
		pBuilder->ThrowBuilderException(
			CXTPMarkupBuilder::FormatString(_T("'%ls' already has a child and cannot add ")
											_T("'%ls'. '%ls' can accept only one child."),
											(LPCTSTR)GetType()->m_lpszClassName,
											(LPCTSTR)pContent->GetType()->m_lpszClassName,
											(LPCTSTR)GetType()->m_lpszClassName));
	}

	if (pContent->IsKindOf(MARKUP_TYPE(CXTPMarkupRotateTransform)))
	{
		SetValue(m_pPropertyRotateTransform, pContent);
		m_pContentProperty = m_pPropertyRotateTransform;
		MARKUP_ADDREF(pContent);
	}
	else if (pContent->IsKindOf(MARKUP_TYPE(CXTPMarkupScaleTransform)))
	{
		SetValue(m_pPropertyScaleTransform, pContent);
		m_pContentProperty = m_pPropertyScaleTransform;
		MARKUP_ADDREF(pContent);
	}
	else if (pContent->IsKindOf(MARKUP_TYPE(CXTPMarkupTranslateTransform)))
	{
		SetValue(m_pPropertyTranslateTransform, pContent);
		m_pContentProperty = m_pPropertyTranslateTransform;
		MARKUP_ADDREF(pContent);
	}
	else if (pContent->IsKindOf(MARKUP_TYPE(CXTPMarkupSkewTransform)))
	{
		SetValue(m_pPropertySkewTransform, pContent);
		m_pContentProperty = m_pPropertySkewTransform;
		MARKUP_ADDREF(pContent);
	}
	else
	{
		CXTPMarkupObject::SetContentObject(pBuilder, pContent);
	}
}

void CXTPMarkupRenderTransform::OnSetAsProperty(CXTPMarkupDependencyProperty* pProperty,
												CXTPMarkupObject* pTargetObject)
{
	UNREFERENCED_PARAMETER(pProperty);

	ASSERT(NULL != pTargetObject);
	m_pTargetObject = pTargetObject;
}

CXTPMarkupTransform* CXTPMarkupRenderTransform::GetTransformObject() const
{
	CXTPMarkupTransform* pTransform = NULL;
	if (NULL != m_pContentProperty)
	{
		pTransform = MARKUP_STATICCAST(CXTPMarkupTransform, GetValue(m_pContentProperty));
	}

	return pTransform;
}
