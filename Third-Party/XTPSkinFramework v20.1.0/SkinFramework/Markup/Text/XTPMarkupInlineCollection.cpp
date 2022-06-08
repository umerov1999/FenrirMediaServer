// XTPMarkupInlineCollection.cpp: implementation of the CXTPMarkupInlineCollection class.
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

#include "Common/Base/Diagnostic/XTPDisableAdvancedWarnings.h"
#include <ActivScp.h>
#include "Common/Base/Diagnostic/XTPEnableAdvancedWarnings.h"

#include "Common/XTPCasting.h"
#include "Common/XTPFramework.h"
#include "Common/Base/Types/XTPSize.h"

#include "Markup/XTPMarkupTools.h"
#include "Markup/XTPMarkupObject.h"
#include "Markup/XTPMarkupString.h"
#include "Markup/XTPMarkupInputElement.h"
#include "Markup/XTPMarkupUIElement.h"
#include "Markup/XTPMarkupBuilder.h"
#include "Markup/XTPMarkupContext.h"

#include "Markup/XTPMarkupFrameworkContentElement.h"
#include "Markup/Text/XTPMarkupTextElement.h"
#include "Markup/Text/XTPMarkupInline.h"
#include "Markup/Text/XTPMarkupRun.h"
#include "Markup/Text/XTPMarkupInlineCollection.h"
#include "Markup/Text/XTPMarkupInlineUIContainer.h"
#include "Markup/XTPMarkupIIDs.h"

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

#ifdef _DEBUG
#	undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#	define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////////
// CXTPMarkupInlineCollection

IMPLEMENT_MARKUPCLASS(NULL, CXTPMarkupInlineCollection, CXTPMarkupCollection);

void CXTPMarkupInlineCollection::RegisterMarkupClass()
{
}
CXTPMarkupInlineCollection::CXTPMarkupInlineCollection()
{
	m_pElementType = MARKUP_TYPE(CXTPMarkupInline);
}

CXTPMarkupInlineCollection::~CXTPMarkupInlineCollection()
{
}

void CXTPMarkupInlineCollection::OnItemAdded(CXTPMarkupObject* pItem, int nIndex)
{
	((CXTPMarkupInline*)pItem)->m_nIndex = nIndex;
}

void CXTPMarkupInlineCollection::SetContentObject(CXTPMarkupBuilder* pBuilder,
												  CXTPMarkupObject* pContent)
{
	ASSERT(NULL != pContent);

	CXTPMarkupInputElement* pParent = MARKUP_DYNAMICCAST(CXTPMarkupInputElement, m_pLogicalParent);
	ASSERT(pParent);
	if (!pParent)
	{
		pBuilder->ThrowBuilderException(
			CXTPMarkupBuilder::FormatString(_T("'%ls' object cannot be added to '%ls'. Object ")
											_T("cannot be converted to type 'CXTPMarkupInline'"),
											(LPCTSTR)pContent->GetType()->m_lpszClassName,
											(LPCTSTR)GetType()->m_lpszClassName));
	}

	if (IsStringObject(pContent))
	{
		CXTPMarkupRun* pRun = MARKUP_CREATE(CXTPMarkupRun, pParent->GetMarkupContext());
		pRun->SetText((CXTPMarkupString*)pContent);
		Add(pRun);
		MARKUP_ADDREF(pContent);
	}
	else if (pContent->IsKindOf(MARKUP_TYPE(CXTPMarkupInline)))
	{
		Add((CXTPMarkupInline*)pContent);
		MARKUP_ADDREF(pContent);
	}
	else if (pContent->IsKindOf(MARKUP_TYPE(CXTPMarkupUIElement)))
	{
		CXTPMarkupInlineUIContainer* pInlineUIContainer =
			MARKUP_CREATE(CXTPMarkupInlineUIContainer, pParent->GetMarkupContext());
		pInlineUIContainer->SetChild((CXTPMarkupUIElement*)pContent);
		Add(pInlineUIContainer);
		MARKUP_ADDREF(pContent);
	}
	else
	{
		pBuilder->ThrowBuilderException(
			CXTPMarkupBuilder::FormatString(_T("'%ls' object cannot be added to '%ls'. Object ")
											_T("cannot be converted to type 'CXTPMarkupInline'"),
											(LPCTSTR)pContent->GetType()->m_lpszClassName,
											(LPCTSTR)GetType()->m_lpszClassName));
	}
}

//////////////////////////////////////////////////////////////////////////
// CXTPMarkupInlineCollection

#include "Common/Base/Diagnostic/XTPBeginAfxMap.h"
BEGIN_DISPATCH_MAP(CXTPMarkupInlineCollection, CXTPMarkupObject)
	DISP_FUNCTION_ID(CXTPMarkupInlineCollection, "Count", 1, OleGetItemCount, VT_I4, VTS_NONE)
	DISP_FUNCTION_ID(CXTPMarkupInlineCollection, "Item", DISPID_VALUE, OleGetItem, VT_DISPATCH,
					 VTS_I4)
	DISP_FUNCTION_ID(CXTPMarkupInlineCollection, "_NewEnum", DISPID_NEWENUM, OleNewEnum, VT_UNKNOWN,
					 VTS_NONE)
	DISP_FUNCTION_ID(CXTPMarkupInlineCollection, "Add", 2, OleAdd, VT_EMPTY, VTS_DISPATCH)
	DISP_FUNCTION_ID(CXTPMarkupInlineCollection, "Insert", 3, OleInsert, VT_EMPTY,
					 VTS_I4 VTS_DISPATCH)
	DISP_FUNCTION_ID(CXTPMarkupInlineCollection, "Clear", 4, RemoveAll, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION_ID(CXTPMarkupInlineCollection, "RemoveAt", 5, Remove, VT_EMPTY, VTS_I4)
END_DISPATCH_MAP()
#include "Common/Base/Diagnostic/XTPEndAfxMap.h"

#ifdef _XTP_ACTIVEX

BEGIN_INTERFACE_MAP(CXTPMarkupInlineCollection, CXTPMarkupObject)
	INTERFACE_PART(CXTPMarkupInlineCollection, XTPDIID_MarkupInlineCollection, Dispatch)
END_INTERFACE_MAP()

IMPLEMENT_OLETYPELIB_EX(CXTPMarkupInlineCollection, XTPDIID_MarkupInlineCollection)
#endif

IMPLEMENT_ENUM_VARIANT(CXTPMarkupInlineCollection)

long CXTPMarkupInlineCollection::OleGetItemCount()
{
	return (long)GetCount();
}

LPDISPATCH CXTPMarkupInlineCollection::OleGetItem(long nIndex)
{
	return XTPGetDispatch(GetInline(nIndex));
}

void CXTPMarkupInlineCollection::OleAdd(LPDISPATCH lpElementDisp)
{
	if (!lpElementDisp)
		AfxThrowOleException(E_INVALIDARG);

	Add(MARKUP_STATICCAST(CXTPMarkupInline, FromDispatch(lpElementDisp)));
}

void CXTPMarkupInlineCollection::OleInsert(long Index, LPDISPATCH lpElementDisp)
{
	if (!lpElementDisp)
		AfxThrowOleException(E_INVALIDARG);

	Insert(Index, MARKUP_STATICCAST(CXTPMarkupInline, FromDispatch(lpElementDisp)));
}
