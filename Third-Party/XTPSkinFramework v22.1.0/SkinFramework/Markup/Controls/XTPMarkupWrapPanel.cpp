// XTPMarkupWrapPanel.cpp: implementation of the CXTPMarkupWrapPanel class.
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
#include "Common/Base/cxminmax.h"

#include "Common/XTPTypeId.h"
#include "Common/XTPCasting.h"
#include "Common/XTPFramework.h"
#include "Common/XTPSystemHelpers.h"

#include "Markup/XTPMarkupTools.h"
#include "Markup/XTPMarkupObject.h"
#include "Markup/XTPMarkupInputElement.h"
#include "Markup/XTPMarkupUIElement.h"
#include "Markup/XTPMarkupFrameworkElement.h"
#include "Markup/Controls/XTPMarkupUIElementCollection.h"
#include "Markup/Controls/XTPMarkupPanel.h"
#include "Markup/Controls/XTPMarkupWrapPanel.h"
#include "Markup/XTPMarkupBuilder.h"
#include "Markup/XTPMarkupIIDs.h"

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

#ifdef _DEBUG
#	undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#	define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////////
// CXTPMarkupWrapPanel

CXTPMarkupDependencyProperty* CXTPMarkupWrapPanel::m_pOrientationProperty = NULL;
CXTPMarkupDependencyProperty* CXTPMarkupWrapPanel::m_pItemHeightProperty  = NULL;
CXTPMarkupDependencyProperty* CXTPMarkupWrapPanel::m_pItemWidthProperty   = NULL;

IMPLEMENT_MARKUPCLASS(L"WrapPanel", CXTPMarkupWrapPanel, CXTPMarkupPanel)

void CXTPMarkupWrapPanel::RegisterMarkupClass()
{
	m_pOrientationProperty = CXTPMarkupDependencyProperty::Register(
		L"Orientation", MARKUP_TYPE(CXTPMarkupEnum), MARKUP_TYPE(CXTPMarkupWrapPanel),
		new CXTPMarkupPropertyMetadata(NULL, &CXTPMarkupBuilder::ConvertOrientation,
									   CXTPMarkupPropertyMetadata::flagAffectsMeasure));

	m_pItemHeightProperty = CXTPMarkupDependencyProperty::Register(
		L"ItemHeight", MARKUP_TYPE(CXTPMarkupInt), MARKUP_TYPE(CXTPMarkupWrapPanel),
		new CXTPMarkupPropertyMetadata(NULL,
									   CXTPMarkupPropertyMetadata::flagAffectsMeasure
										   | CXTPMarkupPropertyMetadata::flagVertDpiSensible));

	m_pItemWidthProperty = CXTPMarkupDependencyProperty::Register(
		L"ItemWidth", MARKUP_TYPE(CXTPMarkupInt), MARKUP_TYPE(CXTPMarkupWrapPanel),
		new CXTPMarkupPropertyMetadata(NULL,
									   CXTPMarkupPropertyMetadata::flagAffectsMeasure
										   | CXTPMarkupPropertyMetadata::flagHorzDpiSensible));
}

CXTPMarkupWrapPanel::CXTPMarkupWrapPanel()
{
}

CXTPMarkupWrapPanel::~CXTPMarkupWrapPanel()
{
}

XTPMarkupOrientation CXTPMarkupWrapPanel::GetOrientation() const
{
	CXTPMarkupEnum* pOrientation = MARKUP_STATICCAST(CXTPMarkupEnum,
													 GetValue(m_pOrientationProperty));
	return pOrientation ? (XTPMarkupOrientation)(int)(*pOrientation)
						: xtpMarkupOrientationHorizontal;
}

void CXTPMarkupWrapPanel::SetOrientation(XTPMarkupOrientation orientation)
{
	SetValue(m_pOrientationProperty, CXTPMarkupEnum::CreateValue(orientation));
}

inline int OrientationWidth(BOOL bHorizontal, const CSize& sz)
{
	return bHorizontal ? sz.cx : sz.cy;
};
inline int OrientationHeight(BOOL bHorizontal, const CSize& sz)
{
	return bHorizontal ? sz.cy : sz.cx;
};

CSize CXTPMarkupWrapPanel::MeasureOverride(CXTPMarkupDrawingContext* pDC, CSize constraint)
{
	BOOL bHorizontal = GetOrientation() == xtpMarkupOrientationHorizontal;

	CXTPMarkupInt* pItemWidth  = MARKUP_STATICCAST(CXTPMarkupInt, GetValue(m_pItemWidthProperty));
	CXTPMarkupInt* pItemHeight = MARKUP_STATICCAST(CXTPMarkupInt, GetValue(m_pItemHeightProperty));

	CSize availableSize(pItemWidth != NULL ? (int)*pItemWidth : constraint.cx,
						pItemHeight != NULL ? (int)*pItemHeight : constraint.cy);

	int nTotalWidth = OrientationWidth(bHorizontal, constraint);

	int nLineWidth  = 0;
	int nLineHeight = 0;

	int nPanelWidth  = 0;
	int nPanelHeight = 0;

	int nCount = m_pChildren->GetCount();
	for (int i = 0; i < nCount; i++)
	{
		CXTPMarkupUIElement* pElement = m_pChildren->GetItem(i);
		if (pElement == NULL)
			continue;

		pElement->Measure(pDC, availableSize);
		CSize desiredSize(pItemWidth != NULL ? (int)*pItemWidth : pElement->GetDesiredSize().cx,
						  pItemHeight != NULL ? (int)*pItemHeight : pElement->GetDesiredSize().cy);

		int nDesiredWidth  = OrientationWidth(bHorizontal, desiredSize);
		int nDesiredHeight = OrientationHeight(bHorizontal, desiredSize);

		if (nLineWidth + nDesiredWidth > nTotalWidth)
		{
			nPanelWidth = CXTP_max(nPanelWidth, nLineWidth);
			nPanelHeight += nLineHeight;

			nLineWidth  = nDesiredWidth;
			nLineHeight = nDesiredHeight;
		}
		else
		{
			nLineWidth += nDesiredWidth;
			nLineHeight = CXTP_max(nLineHeight, nDesiredHeight);
		}
	}

	nPanelWidth = CXTP_max(nPanelWidth, nLineWidth);
	nPanelHeight += nLineHeight;

	return bHorizontal ? CSize(nPanelWidth, nPanelHeight) : CSize(nPanelHeight, nPanelWidth);
}

void CXTPMarkupWrapPanel::ArrangeLine(int nPanelHeight, int nLineHeight, int nFirstElement,
									  int nLastElement)
{
	BOOL bHorizontal = GetOrientation() == xtpMarkupOrientationHorizontal;

	CXTPMarkupInt* pItemWidth = MARKUP_STATICCAST(CXTPMarkupInt,
												  GetValue(bHorizontal ? m_pItemWidthProperty
																	   : m_pItemHeightProperty));

	int nTotalWidth = 0;

	for (int i = nFirstElement; i < nLastElement; i++)
	{
		CXTPMarkupUIElement* pElement = m_pChildren->GetItem(i);
		if (pElement == NULL)
			continue;

		int nWidth = pItemWidth != NULL ? (int)*pItemWidth
										: (bHorizontal ? pElement->GetDesiredSize().cx
													   : pElement->GetDesiredSize().cy);

		if (bHorizontal)
		{
			pElement->Arrange(
				CRect(nTotalWidth, nPanelHeight, nTotalWidth + nWidth, nPanelHeight + nLineHeight));
		}
		else
		{
			pElement->Arrange(
				CRect(nPanelHeight, nTotalWidth, nPanelHeight + nLineHeight, nTotalWidth + nWidth));
		}

		nTotalWidth += nWidth;
	}
}

CSize CXTPMarkupWrapPanel::ArrangeOverride(CSize arrangeSize)
{
	BOOL bHorizontal = GetOrientation() == xtpMarkupOrientationHorizontal;

	CXTPMarkupInt* pItemWidth  = MARKUP_STATICCAST(CXTPMarkupInt, GetValue(m_pItemWidthProperty));
	CXTPMarkupInt* pItemHeight = MARKUP_STATICCAST(CXTPMarkupInt, GetValue(m_pItemHeightProperty));

	int nTotalWidth = OrientationWidth(bHorizontal, arrangeSize);

	int nLineWidth  = 0;
	int nLineHeight = 0;

	int nPanelHeight = 0;

	int nFirstElement = 0;

	int nCount = m_pChildren->GetCount();
	for (int i = 0; i < nCount; i++)
	{
		CXTPMarkupUIElement* pElement = m_pChildren->GetItem(i);
		if (pElement == NULL)
			continue;

		CSize desiredSize(pItemWidth != NULL ? (int)*pItemWidth : pElement->GetDesiredSize().cx,
						  pItemHeight != NULL ? (int)*pItemHeight : pElement->GetDesiredSize().cy);

		int nDesiredWidth  = OrientationWidth(bHorizontal, desiredSize);
		int nDesiredHeight = OrientationHeight(bHorizontal, desiredSize);

		if (nLineWidth + nDesiredWidth > nTotalWidth)
		{
			ArrangeLine(nPanelHeight, nLineHeight, nFirstElement, i);

			nPanelHeight += nLineHeight;

			nLineWidth  = nDesiredWidth;
			nLineHeight = nDesiredHeight;

			nFirstElement = i;
		}
		else
		{
			nLineWidth += nDesiredWidth;
			nLineHeight = CXTP_max(nLineHeight, nDesiredHeight);
		}
	}

	ArrangeLine(nPanelHeight, nLineHeight, nFirstElement, nCount);

	return arrangeSize;
}

//////////////////////////////////////////////////////////////////////////
// CXTPMarkupWrapPanel

#include "Common/Base/Diagnostic/XTPBeginAfxMap.h"
BEGIN_DISPATCH_MAP(CXTPMarkupWrapPanel, CXTPMarkupPanel)
	DISP_PROPERTY_EX_ID(CXTPMarkupWrapPanel, "Orientation", 600, OleGetOrientation,
						OleSetOrientation, VT_I4)
END_DISPATCH_MAP()
#include "Common/Base/Diagnostic/XTPEndAfxMap.h"

#ifdef _XTP_ACTIVEX

BEGIN_INTERFACE_MAP(CXTPMarkupWrapPanel, CXTPMarkupPanel)
	INTERFACE_PART(CXTPMarkupWrapPanel, XTPDIID_MarkupWrapPanel, Dispatch)
END_INTERFACE_MAP()

IMPLEMENT_OLETYPELIB_EX(CXTPMarkupWrapPanel, XTPDIID_MarkupWrapPanel)
#endif

long CXTPMarkupWrapPanel::OleGetOrientation()
{
	return GetOrientation();
}

void CXTPMarkupWrapPanel::OleSetOrientation(long nValue)
{
	SetOrientation((XTPMarkupOrientation)nValue);
}
