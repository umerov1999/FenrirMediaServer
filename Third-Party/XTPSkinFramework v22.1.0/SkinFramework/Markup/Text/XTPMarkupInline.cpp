// XTPMarkupInline.cpp: implementation of the CXTPMarkupInline class.
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
#include "Common/Math/XTPMathUtils.h"
#include "Common/XTPCasting.h"
#include "Common/XTPFramework.h"
#include "Common/XTPSystemHelpers.h"
#include "Common/Base/Types/XTPPoint2.h"
#include "Common/Base/Types/XTPSize.h"
#include "Common/Base/Types/XTPRect.h"

#include "Markup/XTPMarkupTools.h"
#include "Markup/XTPMarkupObject.h"
#include "Markup/XTPMarkupInputElement.h"
#include "Markup/XTPMarkupUIElement.h"
#include "Markup/XTPMarkupFrameworkElement.h"
#include "Markup/XTPMarkupBuilder.h"
#include "Markup/Controls/XTPMarkupTextBlock.h"
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
// CXTPMarkupInline

CXTPMarkupDependencyProperty* CXTPMarkupInline::m_pBaselineAlignmentProperty = NULL;

IMPLEMENT_MARKUPCLASS(NULL, CXTPMarkupInline, CXTPMarkupTextElement);

void CXTPMarkupInline::RegisterMarkupClass()
{
	m_pBaselineAlignmentProperty = CXTPMarkupDependencyProperty::Register(
		L"BaselineAlignment", MARKUP_TYPE(CXTPMarkupEnum), MARKUP_TYPE(CXTPMarkupInline),
		new CXTPMarkupPropertyMetadata(NULL, &CXTPMarkupBuilder::ConvertBaselineAlignment,
									   CXTPMarkupPropertyMetadata::flagAffectsArrange));
}

CXTPMarkupInline::CXTPMarkupInline()
{
	m_nIndex = -1;
}

CXTPMarkupInline::~CXTPMarkupInline()
{
}

POSITION CXTPMarkupInline::GetContentStartPosition() const
{
	return MARKUP_POSITION_EOF;
};

void CXTPMarkupInline::GetContentNextPosition(POSITION& pos) const
{
	pos = NULL;
}

BOOL CXTPMarkupInline::IsWordBreakPosition(POSITION /*pos*/) const
{
	return FALSE;
}

BOOL CXTPMarkupInline::IsLineBreakPosition(POSITION /*pos*/) const
{
	return FALSE;
}

BOOL CXTPMarkupInline::IsCaretReturnPosition(POSITION /*pos*/) const
{
	return FALSE;
}

BOOL CXTPMarkupInline::IsWhiteSpacePosition(POSITION /*pos*/) const
{
	return FALSE;
}

void CXTPMarkupInline::PrepareMeasure(CXTPMarkupDrawingContext* pDC)
{
	UNREFERENCED_PARAMETER(pDC);
}

CSize CXTPMarkupInline::Measure(CXTPMarkupDrawingContext* /*pDC*/, POSITION /*posStart*/,
								POSITION /*posEnd*/)
{
	return CSize(0, 0);
}

void CXTPMarkupInline::Measure(CXTPMarkupDrawingContext* /*pDC*/, POSITION /*posStart*/,
							   POSITION /*posEnd*/, CXTPSizeF& size)
{
	size = CXTPSizeF();
}

int CXTPMarkupInline::GetBaseline() const
{
	return 0;
}

const XTPMarkupFontMetrics* CXTPMarkupInline::GetFontMetrics() const
{
	return NULL;
}

void CXTPMarkupInline::Arrange(CRect /*rcFinalRect*/, POSITION /*posStart*/, POSITION /*posEnd*/)
{
}

void CXTPMarkupInline::Render(CXTPMarkupDrawingContext* /*pDC*/, CRect /*rc*/,
							  POSITION /*posStart*/, POSITION /*posEnd*/)
{
}

CXTPMarkupInline* CXTPMarkupInline::GetFirstInline() const
{
	return (CXTPMarkupInline*)this;
}

CXTPMarkupInline* CXTPMarkupInline::GetNextInline() const
{
	CXTPMarkupInlineCollection* pOwner = MARKUP_DYNAMICCAST(CXTPMarkupInlineCollection,
															m_pLogicalParent);

	if (!pOwner)
		return NULL;

	if (pOwner->GetCount() > m_nIndex + 1)
		return pOwner->GetInline(m_nIndex + 1)->GetFirstInline();

	if (MARKUP_DYNAMICCAST(CXTPMarkupSpan, pOwner->GetLogicalParent()))
		return ((CXTPMarkupSpan*)(pOwner->GetLogicalParent()))->GetNextInline();

	return NULL;
}

CXTPMarkupTextBlock* CXTPMarkupInline::GetTextBlock() const
{
	return MARKUP_STATICCAST(CXTPMarkupTextBlock, GetParent());
}

XTPMarkupBaselineAlignment CXTPMarkupInline::GetBaselineAlignment() const
{
	CXTPMarkupEnum* pBaseline = MARKUP_STATICCAST(CXTPMarkupEnum,
												  GetValue(m_pBaselineAlignmentProperty));
	return pBaseline ? (XTPMarkupBaselineAlignment)(int)*pBaseline : xtpMarkupBaseline;
}

#include "Common/Base/Diagnostic/XTPBeginAfxMap.h"
BEGIN_DISPATCH_MAP(CXTPMarkupInline, CXTPMarkupTextElement)
	DISP_PROPERTY_EX_ID(CXTPMarkupInline, "BaselineAlignment", 600, OleGetBaselineAlignment,
						OleSetBaselineAlignment, VT_I4)
END_DISPATCH_MAP()
#include "Common/Base/Diagnostic/XTPEndAfxMap.h"

#ifdef _XTP_ACTIVEX

BEGIN_INTERFACE_MAP(CXTPMarkupInline, CXTPMarkupTextElement)
	INTERFACE_PART(CXTPMarkupInline, XTPDIID_MarkupInline, Dispatch)
END_INTERFACE_MAP()

IMPLEMENT_OLETYPELIB_EX(CXTPMarkupInline, XTPDIID_MarkupInline)
#endif

int CXTPMarkupInline::OleGetBaselineAlignment()
{
	return GetBaselineAlignment();
}

void CXTPMarkupInline::OleSetBaselineAlignment(int nValue)
{
	SetBaselineAlignment((XTPMarkupBaselineAlignment)nValue);
}
