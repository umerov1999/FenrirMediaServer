// XTPMarkupStackPanel.cpp: implementation of the CXTPMarkupStackPanel class.
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

#include "Common/XTPCasting.h"
#include "Markup/XTPMarkupTools.h"
#include "Markup/XTPMarkupObject.h"
#include "Markup/XTPMarkupInputElement.h"
#include "Markup/XTPMarkupUIElement.h"
#include "Markup/XTPMarkupFrameworkElement.h"
#include "Markup/Controls/XTPMarkupUIElementCollection.h"
#include "Markup/Controls/XTPMarkupPanel.h"
#include "Markup/Controls/XTPMarkupStackPanel.h"
#include "Markup/XTPMarkupBuilder.h"
#include "Markup/XTPMarkupIIDs.h"

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

#ifdef _DEBUG
#	undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#	define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CXTPMarkupDependencyProperty* CXTPMarkupStackPanel::m_pOrientationProperty = NULL;

IMPLEMENT_MARKUPCLASS(L"StackPanel", CXTPMarkupStackPanel, CXTPMarkupPanel)

void CXTPMarkupStackPanel::RegisterMarkupClass()
{
	m_pOrientationProperty = CXTPMarkupDependencyProperty::Register(
		L"Orientation", MARKUP_TYPE(CXTPMarkupEnum), MARKUP_TYPE(CXTPMarkupStackPanel),
		new CXTPMarkupPropertyMetadata(NULL, &CXTPMarkupBuilder::ConvertOrientation,
									   CXTPMarkupPropertyMetadata::flagAffectsMeasure));
}

CXTPMarkupStackPanel::CXTPMarkupStackPanel()
{
}

CXTPMarkupStackPanel::~CXTPMarkupStackPanel()
{
}

XTPMarkupOrientation CXTPMarkupStackPanel::GetOrientation() const
{
	CXTPMarkupEnum* pOrientation = MARKUP_STATICCAST(CXTPMarkupEnum,
													 GetValue(m_pOrientationProperty));
	return pOrientation ? (XTPMarkupOrientation)(int)(*pOrientation) : xtpMarkupOrientationVertical;
}

void CXTPMarkupStackPanel::SetOrientation(XTPMarkupOrientation orientation)
{
	SetValue(m_pOrientationProperty, CXTPMarkupEnum::CreateValue(orientation));
}

CSize CXTPMarkupStackPanel::MeasureOverride(CXTPMarkupDrawingContext* pDC, CSize constraint)
{
	CSize size(0, 0);
	CSize availableSize = constraint;

	bool bHorizontal = GetOrientation() == xtpMarkupOrientationHorizontal;

	if (bHorizontal)
	{
		availableSize.cx = INT_MAX;
	}
	else
	{
		availableSize.cy = INT_MAX;
	}

	int nCount = m_pChildren->GetCount();
	for (int i = 0; i < nCount; i++)
	{
		CXTPMarkupUIElement* pElement = m_pChildren->GetItem(i);
		if (pElement == NULL)
			continue;

		pElement->Measure(pDC, availableSize);
		CSize desiredSize = pElement->GetDesiredSize();

		if (bHorizontal)
		{
			size.cx += desiredSize.cx;
			size.cy = max(size.cy, desiredSize.cy);
		}
		else
		{
			size.cx = max(size.cx, desiredSize.cx);
			size.cy += desiredSize.cy;
		}
	}

	return size;
}

CSize CXTPMarkupStackPanel::ArrangeOverride(CSize arrangeSize)
{
	bool bHorizontal = GetOrientation() == xtpMarkupOrientationHorizontal;

	CRect finalRect(CPoint(0), arrangeSize);
	int width = 0;

	int nCount = m_pChildren->GetCount();
	for (int i = 0; i < nCount; i++)
	{
		CXTPMarkupUIElement* pElement = m_pChildren->GetItem(i);
		if (pElement == NULL)
			continue;

		if (bHorizontal)
		{
			finalRect.left += width;
			width			 = pElement->GetDesiredSize().cx;
			finalRect.right	 = finalRect.left + width;
			finalRect.bottom = max(arrangeSize.cy, pElement->GetDesiredSize().cy);
		}
		else
		{
			finalRect.top += width;
			width			 = pElement->GetDesiredSize().cy;
			finalRect.bottom = finalRect.top + width;
			finalRect.right	 = max(arrangeSize.cx, pElement->GetDesiredSize().cx);
		}

		pElement->Arrange(finalRect);
	}

	return arrangeSize;
}

#include "Common/Base/Diagnostic/XTPBeginAfxMap.h"
BEGIN_DISPATCH_MAP(CXTPMarkupStackPanel, CXTPMarkupPanel)
	DISP_PROPERTY_EX_ID(CXTPMarkupStackPanel, "Orientation", 600, OleGetOrientation,
						OleSetOrientation, VT_I4)
END_DISPATCH_MAP()
#include "Common/Base/Diagnostic/XTPEndAfxMap.h"

#ifdef _XTP_ACTIVEX

BEGIN_INTERFACE_MAP(CXTPMarkupStackPanel, CXTPMarkupPanel)
	INTERFACE_PART(CXTPMarkupStackPanel, XTPDIID_MarkupStackPanel, Dispatch)
END_INTERFACE_MAP()

IMPLEMENT_OLETYPELIB_EX(CXTPMarkupStackPanel, XTPDIID_MarkupStackPanel)
#endif

long CXTPMarkupStackPanel::OleGetOrientation()
{
	return GetOrientation();
}

void CXTPMarkupStackPanel::OleSetOrientation(long nValue)
{
	SetOrientation((XTPMarkupOrientation)nValue);
}
