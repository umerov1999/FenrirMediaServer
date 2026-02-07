// XTPMarkupContentControl.cpp : implementation file
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
#include "Common/Math/XTPMathUtils.h"
#include "Common/Base/Types/XTPPoint2.h"
#include "Common/Base/Types/XTPSize.h"
#include "Common/Base/Types/XTPRect.h"

#include "Markup/XTPMarkupTools.h"
#include "Markup/XTPMarkupBuilder.h"
#include "Markup/XTPMarkupObject.h"
#include "Markup/XTPMarkupString.h"
#include "Markup/XTPMarkupInputElement.h"
#include "Markup/XTPMarkupUIElement.h"
#include "Markup/XTPMarkupFrameworkElement.h"
#include "Markup/Controls/XTPMarkupControl.h"
#include "Markup/XTPMarkupContext.h"
#include "Markup/Controls/XTPMarkupContentControl.h"
#include "Markup/Controls/XTPMarkupTextBlock.h"
#include "Markup/XTPMarkupIIDs.h"

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

#ifdef _DEBUG
#	define new DEBUG_NEW
#	undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////
// CXTPMarkupContentControl

CXTPMarkupDependencyProperty* CXTPMarkupContentControl::m_pContentProperty = NULL;

IMPLEMENT_MARKUPCLASS(NULL, CXTPMarkupContentControl, CXTPMarkupControl)

void CXTPMarkupContentControl::RegisterMarkupClass()
{
	m_pContentProperty = CXTPMarkupDependencyProperty::Register(
		L"Content", MARKUP_TYPE(CXTPMarkupUIElement), MARKUP_TYPE(CXTPMarkupContentControl),
		new CXTPMarkupPropertyMetadata(NULL, &CXTPMarkupBuilder::ConvertContent,
									   CXTPMarkupPropertyMetadata::flagAffectsMeasure));
}

CXTPMarkupContentControl::CXTPMarkupContentControl()
{
}

CXTPMarkupContentControl::~CXTPMarkupContentControl()
{
}

CXTPMarkupUIElement* CXTPMarkupContentControl::GetContent() const
{
	return MARKUP_STATICCAST(CXTPMarkupUIElement, GetValue(m_pContentProperty));
}

void CXTPMarkupContentControl::SetContent(CXTPMarkupUIElement* pContent)
{
	SetValue(m_pContentProperty, pContent);
}

void CXTPMarkupContentControl::SetContentObject(CXTPMarkupBuilder* pBuilder,
												CXTPMarkupObject* pContent)
{
	ASSERT(NULL != pContent);

	if (GetContent() != NULL)
	{
		pBuilder->ThrowBuilderException(
			CXTPMarkupBuilder::FormatString(_T("'%ls' already has a child and cannot add '%ls'")
											_T(". '%ls' can accept only one child."),
											(LPCTSTR)GetType()->m_lpszClassName,
											(LPCTSTR)pContent->GetType()->m_lpszClassName,
											(LPCTSTR)GetType()->m_lpszClassName));
	}

	if (IsStringObject(pContent))
	{
		CXTPMarkupTextBlock* pTextBlock = MARKUP_CREATE(CXTPMarkupTextBlock, GetMarkupContext());
		pTextBlock->SetContentObject(pBuilder, (CXTPMarkupString*)pContent);
		SetContent(pTextBlock);
	}
	else if (pContent->IsKindOf(MARKUP_TYPE(CXTPMarkupUIElement)))
	{
		SetContent((CXTPMarkupUIElement*)pContent);
		MARKUP_ADDREF(pContent);
	}
	else
	{
		pBuilder->ThrowBuilderException(
			CXTPMarkupBuilder::FormatString(_T("'%ls' object cannot be added to '%ls'. Object ")
											_T("cannot be converted to type 'CXTPMarkupUIElement'"),
											(LPCTSTR)pContent->GetType()->m_lpszClassName,
											(LPCTSTR)GetType()->m_lpszClassName));
	}
}

BOOL CXTPMarkupContentControl::HasContentObject() const
{
	return GetContent() != NULL;
}

#include "Common/Base/Diagnostic/XTPBeginAfxMap.h"
BEGIN_DISPATCH_MAP(CXTPMarkupContentControl, CXTPMarkupControl)
	DISP_PROPERTY_EX_ID(CXTPMarkupContentControl, "Content", 600, OleGetContent, OleSetContent,
						VT_DISPATCH)
END_DISPATCH_MAP()
#include "Common/Base/Diagnostic/XTPEndAfxMap.h"

#ifdef _XTP_ACTIVEX

BEGIN_INTERFACE_MAP(CXTPMarkupContentControl, CXTPMarkupControl)
	INTERFACE_PART(CXTPMarkupContentControl, XTPDIID_MarkupContentControl, Dispatch)
END_INTERFACE_MAP()

IMPLEMENT_OLETYPELIB_EX(CXTPMarkupContentControl, XTPDIID_MarkupContentControl)
#endif

LPDISPATCH CXTPMarkupContentControl::OleGetContent()
{
	return XTPGetDispatch(GetContent());
}

void CXTPMarkupContentControl::OleSetContent(LPDISPATCH lpDisp)
{
	SetContent(MARKUP_STATICCAST(CXTPMarkupUIElement, FromDispatch(lpDisp)));
}
