// XTPMarkupDecorator.cpp: implementation of the CXTPMarkupDecorator class.
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

#include "stdafx.h"

#include "Common/XTPFramework.h"

#include "Markup/XTPMarkupObject.h"
#include "Markup/XTPMarkupInputElement.h"
#include "Markup/XTPMarkupUIElement.h"
#include "Markup/XTPMarkupFrameworkElement.h"
#include "Markup/Controls/XTPMarkupDecorator.h"
#include "Markup/XTPMarkupBuilder.h"

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

#ifdef _DEBUG
#	undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#	define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CXTPMarkupDecorator::CXTPMarkupDecorator()
{
	m_pChild = NULL;
}

CXTPMarkupDecorator::~CXTPMarkupDecorator()
{
	SetChild(NULL);
}

CXTPMarkupUIElement* CXTPMarkupDecorator::GetChild() const
{
	return m_pChild;
}

void CXTPMarkupDecorator::SetChild(CXTPMarkupUIElement* pChild)
{
	if (m_pChild)
	{
		m_pChild->SetLogicalParent(NULL);
		MARKUP_RELEASE(m_pChild);
	}

	m_pChild = pChild;

	if (m_pChild)
	{
		m_pChild->SetLogicalParent(this);
	}
}

void CXTPMarkupDecorator::SetContentObject(CXTPMarkupBuilder* pBuilder, CXTPMarkupObject* pContent)
{
	ASSERT(NULL != pContent);

	if (!pContent->IsKindOf(MARKUP_TYPE(CXTPMarkupUIElement)))
	{
		pBuilder->ThrowBuilderException(
			CXTPMarkupBuilder::FormatString(_T("'%ls' object cannot be added to '%ls'. Object ")
											_T("cannot be converted to type 'CXTPMarkupUIElement'"),
											(LPCTSTR)pContent->GetType()->m_lpszClassName,
											(LPCTSTR)GetType()->m_lpszClassName));
	}

	if (m_pChild != NULL)
	{
		pBuilder->ThrowBuilderException(
			CXTPMarkupBuilder::FormatString(_T("'%ls' already has a child and cannot add ")
											_T("'%ls'. '%ls' can accept only one child."),
											(LPCTSTR)GetType()->m_lpszClassName,
											(LPCTSTR)pContent->GetType()->m_lpszClassName,
											(LPCTSTR)GetType()->m_lpszClassName));
	}

	SetChild((CXTPMarkupUIElement*)pContent);
	MARKUP_ADDREF(pContent);
}

BOOL CXTPMarkupDecorator::HasContentObject() const
{
	return m_pChild != NULL;
}
