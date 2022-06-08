// XTPMarkupControl.cpp: implementation of the CXTPMarkupControl class.
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

#include "Common/Base/Diagnostic/XTPDisableAdvancedWarnings.h"
#include <ActivScp.h>
#include "Common/Base/Diagnostic/XTPEnableAdvancedWarnings.h"

#include "GraphicLibrary/GdiPlus/XTPGdiPlus.h"

#include "Common/XTPTypeId.h"
#include "Common/XTPCasting.h"
#include "Common/XTPFramework.h"
#include "Common/XTPSystemHelpers.h"
#include "Common/XTPSynchro.h"
#include "Common/XTPApplication.h"
#include "Common/XTPSingleton.h"
#include "Common/XTPGdiObjects.h"
#include "Common/Base/Types/XTPSize.h"

#include "Markup/XTPMarkupContext.h"
#include "Markup/XTPMarkupTools.h"
#include "Markup/XTPMarkupObject.h"
#include "Markup/XTPMarkupInputElement.h"
#include "Markup/XTPMarkupUIElement.h"
#include "Markup/XTPMarkupFrameworkElement.h"
#include "Markup/Controls/XTPMarkupDecorator.h"
#include "Markup/Controls/XTPMarkupBorder.h"
#include "Markup/Controls/XTPMarkupControl.h"
#include "Markup/Controls/XTPMarkupPanel.h"
#include "Markup/XTPMarkupFrameworkContentElement.h"
#include "Markup/Text/XTPMarkupTextElement.h"
#include "Markup/XTPMarkupDrawingContext.h"
#include "Markup/XTPMarkupThickness.h"
#include "Markup/XTPMarkupIIDs.h"

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

#ifdef _DEBUG
#	undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#	define new DEBUG_NEW
#endif

CXTPMarkupDependencyProperty* CXTPMarkupControl::m_pBackgroundProperty = NULL;
CXTPMarkupDependencyProperty* CXTPMarkupControl::m_pForegroundProperty = NULL;
CXTPMarkupDependencyProperty* CXTPMarkupControl::m_pPaddingProperty	   = NULL;

IMPLEMENT_MARKUPCLASS(L"Control", CXTPMarkupControl, CXTPMarkupFrameworkElement)

void CXTPMarkupControl::RegisterMarkupClass()
{
	CXTPMarkupBorder::RegisterType();
	CXTPMarkupPanel::RegisterType();
	CXTPMarkupTextElement::RegisterType();

	m_pPaddingProperty = CXTPMarkupBorder::m_pPaddingProperty->AddOwner(
		MARKUP_TYPE(CXTPMarkupControl));
	m_pBackgroundProperty = CXTPMarkupPanel::m_pBackgroundProperty->AddOwner(
		MARKUP_TYPE(CXTPMarkupControl));
	m_pForegroundProperty = CXTPMarkupTextElement::m_pForegroundProperty->AddOwner(
		MARKUP_TYPE(CXTPMarkupControl));
}

CSize CXTPMarkupControl::ArrangeOverride(CSize szFinalSize)
{
	if (GetVisualChildrenCount() > 0)
	{
		CXTPMarkupUIElement* pVisualChild = (CXTPMarkupUIElement*)GetVisualChild(0);

		pVisualChild->Arrange(CRect(0, 0, szFinalSize.cx, szFinalSize.cy));
	}
	return szFinalSize;
}

CSize CXTPMarkupControl::MeasureOverride(CXTPMarkupDrawingContext* pDC, CSize szAvailableSize)
{
	if (GetVisualChildrenCount() > 0)
	{
		CXTPMarkupUIElement* pVisualChild = (CXTPMarkupUIElement*)GetVisualChild(0);

		pVisualChild->Measure(pDC, szAvailableSize);
		return pVisualChild->GetDesiredSize();
	}

	return CSize(0, 0);
}

CXTPMarkupBrush* CXTPMarkupControl::GetBackground() const
{
	return MARKUP_STATICCAST(CXTPMarkupBrush, GetValue(m_pBackgroundProperty));
}

void CXTPMarkupControl::SetBackground(CXTPMarkupBrush* brush)
{
	SetValue(m_pBackgroundProperty, brush);
}

CXTPMarkupBrush* CXTPMarkupControl::GetForeground() const
{
	return MARKUP_STATICCAST(CXTPMarkupBrush, GetValue(m_pForegroundProperty));
}

void CXTPMarkupControl::SetForeground(CXTPMarkupBrush* brush)
{
	SetValue(m_pForegroundProperty, brush);
}

void CXTPMarkupControl::SetPadding(int nLeft, int nTop, int nRight, int nBottom)
{
	SetValue(m_pPaddingProperty, new CXTPMarkupThickness(nLeft, nTop, nRight, nBottom));
}

void CXTPMarkupControl::SetPadding(int padding)
{
	SetValue(m_pPaddingProperty, new CXTPMarkupThickness(padding));
}

CXTPMarkupThickness* CXTPMarkupControl::GetPadding() const
{
	return MARKUP_STATICCAST(CXTPMarkupThickness, GetValue(m_pPaddingProperty));
}

#include "Common/Base/Diagnostic/XTPBeginAfxMap.h"
BEGIN_DISPATCH_MAP(CXTPMarkupControl, CXTPMarkupFrameworkElement)
	DISP_PROPERTY_EX_ID(CXTPMarkupControl, "Background", 500, OleGetBackground, OleSetBackground,
						VT_DISPATCH)
	DISP_PROPERTY_EX_ID(CXTPMarkupControl, "Foreground", 501, OleGetForeground, OleSetForeground,
						VT_DISPATCH)
	DISP_PROPERTY_EX_ID(CXTPMarkupControl, "Padding", 502, OleGetPadding, OleSetPadding,
						VT_DISPATCH)
END_DISPATCH_MAP()
#include "Common/Base/Diagnostic/XTPEndAfxMap.h"

#ifdef _XTP_ACTIVEX

BEGIN_INTERFACE_MAP(CXTPMarkupControl, CXTPMarkupFrameworkElement)
	INTERFACE_PART(CXTPMarkupControl, XTPDIID_MarkupControl, Dispatch)
END_INTERFACE_MAP()

IMPLEMENT_OLETYPELIB_EX(CXTPMarkupControl, XTPDIID_MarkupControl)
#endif

LPDISPATCH CXTPMarkupControl::OleGetBackground()
{
	return XTPGetDispatch(GetBackground());
}

void CXTPMarkupControl::OleSetBackground(LPDISPATCH lpBackground)
{
	SetBackground(MARKUP_STATICCAST(CXTPMarkupBrush, FromDispatch(lpBackground)));
}

LPDISPATCH CXTPMarkupControl::OleGetForeground()
{
	return XTPGetDispatch(GetForeground());
}

void CXTPMarkupControl::OleSetForeground(LPDISPATCH lpForeground)
{
	SetForeground(MARKUP_STATICCAST(CXTPMarkupBrush, FromDispatch(lpForeground)));
}

LPDISPATCH CXTPMarkupControl::OleGetPadding()
{
	return XTPGetDispatch(GetPadding());
}

void CXTPMarkupControl::OleSetPadding(LPDISPATCH lpThickness)
{
	SetValue(m_pPaddingProperty, MARKUP_STATICCAST(CXTPMarkupThickness, FromDispatch(lpThickness)));
}
