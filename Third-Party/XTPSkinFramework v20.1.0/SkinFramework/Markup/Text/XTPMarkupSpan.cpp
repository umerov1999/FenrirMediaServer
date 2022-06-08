// XTPMarkupSpan.cpp: implementation of the CXTPMarkupSpan class.
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

#include "Common/XTPTypeId.h"
#include "Common/XTPCasting.h"
#include "Common/XTPFramework.h"
#include "Common/Base/Types/XTPSize.h"

#include "Markup/XTPMarkupTools.h"
#include "Markup/XTPMarkupObject.h"
#include "Markup/XTPMarkupInputElement.h"
#include "Markup/XTPMarkupFrameworkContentElement.h"
#include "Markup/Text/XTPMarkupTextElement.h"
#include "Markup/Text/XTPMarkupInline.h"
#include "Markup/Text/XTPMarkupSpan.h"
#include "Markup/Text/XTPMarkupInlineCollection.h"
#include "Markup/XTPMarkupIIDs.h"

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

#ifdef _DEBUG
#	undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#	define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////////
// CXTPMarkupSpan

IMPLEMENT_MARKUPCLASS(L"Span", CXTPMarkupSpan, CXTPMarkupInline);

void CXTPMarkupSpan::RegisterMarkupClass()
{
}

CXTPMarkupSpan::CXTPMarkupSpan()
{
	m_pInlines = new CXTPMarkupInlineCollection();
	m_pInlines->SetLogicalParent(this);
}

CXTPMarkupSpan::~CXTPMarkupSpan()
{
	if (m_pInlines)
	{
		m_pInlines->SetLogicalParent(NULL);
		m_pInlines->Release();
	}
}

void CXTPMarkupSpan::SetContentObject(CXTPMarkupBuilder* pBuilder, CXTPMarkupObject* pContent)
{
	m_pInlines->SetContentObject(pBuilder, pContent);
}
BOOL CXTPMarkupSpan::HasContentObject() const
{
	return m_pInlines->HasContentObject();
}

BOOL CXTPMarkupSpan::AllowWhiteSpaceContent() const
{
	return TRUE;
}

CXTPMarkupInline* CXTPMarkupSpan::GetFirstInline() const
{
	return m_pInlines->GetCount() > 0 ? m_pInlines->GetInline(0)->GetFirstInline()
									  : (CXTPMarkupInline*)this;
}

/////////////////////////////////////////////////////////////////////////
// Span

#include "Common/Base/Diagnostic/XTPBeginAfxMap.h"
BEGIN_DISPATCH_MAP(CXTPMarkupSpan, CXTPMarkupInline)
	DISP_PROPERTY_EX_ID(CXTPMarkupSpan, "Inlines", 700, OleGetInlines, SetNotSupported, VT_DISPATCH)
END_DISPATCH_MAP()
#include "Common/Base/Diagnostic/XTPEndAfxMap.h"

#ifdef _XTP_ACTIVEX

BEGIN_INTERFACE_MAP(CXTPMarkupSpan, CXTPMarkupInline)
	INTERFACE_PART(CXTPMarkupSpan, XTPDIID_MarkupSpan, Dispatch)
END_INTERFACE_MAP()

IMPLEMENT_OLETYPELIB_EX(CXTPMarkupSpan, XTPDIID_MarkupSpan)
#endif

LPDISPATCH CXTPMarkupSpan::OleGetInlines()
{
	return XTPGetDispatch(GetInlines());
}
