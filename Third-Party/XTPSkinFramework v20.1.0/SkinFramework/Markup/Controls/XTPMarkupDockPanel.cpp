// XTPMarkupDockPanel.cpp: implementation of the CXTPMarkupDockPanel class.
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

#include "Common/XTPCasting.h"
#include "Markup/XTPMarkupTools.h"
#include "Markup/XTPMarkupObject.h"
#include "Markup/XTPMarkupString.h"
#include "Markup/XTPMarkupInputElement.h"
#include "Markup/XTPMarkupUIElement.h"
#include "Markup/XTPMarkupFrameworkElement.h"
#include "Markup/Controls/XTPMarkupUIElementCollection.h"
#include "Markup/Controls/XTPMarkupPanel.h"
#include "Markup/Controls/XTPMarkupDockPanel.h"
#include "Markup/XTPMarkupIIDs.h"

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

#ifdef _DEBUG
#	undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#	define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////////
// CXTPMarkupDockPanel

CXTPMarkupDependencyProperty* CXTPMarkupDockPanel::m_pLastChildFillProperty = NULL;
CXTPMarkupDependencyProperty* CXTPMarkupDockPanel::m_pDockProperty			= NULL;

IMPLEMENT_MARKUPCLASS(L"DockPanel", CXTPMarkupDockPanel, CXTPMarkupPanel)

void CXTPMarkupDockPanel::RegisterMarkupClass()
{
	m_pLastChildFillProperty = CXTPMarkupDependencyProperty::Register(
		L"LastChildFill", MARKUP_TYPE(CXTPMarkupBool), MARKUP_TYPE(CXTPMarkupDockPanel),
		new CXTPMarkupPropertyMetadata(CXTPMarkupBool::CreateTrueValue(),
									   CXTPMarkupPropertyMetadata::flagAffectsArrange));

	m_pDockProperty = CXTPMarkupDependencyProperty::RegisterAttached(
		L"Dock", MARKUP_TYPE(CXTPMarkupEnum), MARKUP_TYPE(CXTPMarkupDockPanel),
		new CXTPMarkupPropertyMetadata(CXTPMarkupEnum::CreateValue(xtpMarkupDockLeft),
									   &CXTPMarkupDockPanel::ConvertDock,
									   &CXTPMarkupDockPanel::OnDockChanged));
}

CXTPMarkupDockPanel::CXTPMarkupDockPanel()
{
}

CXTPMarkupDockPanel::~CXTPMarkupDockPanel()
{
}

CXTPMarkupObject*
	CXTPMarkupDockPanel::ConvertDock(CXTPMarkupBuilder* /*pBuilder*/, CXTPMarkupObject* pObject,
									 CXTPMarkupDependencyProperty* /*pAssociatedProperty = NULL*/)
{
	if (IsStringObject(pObject))
	{
		LPCWSTR lpszValue = *((CXTPMarkupString*)pObject);
		int nLength		  = ((CXTPMarkupString*)pObject)->GetLength();

		if (nLength == 4 && _wcsicmp(lpszValue, L"Left") == 0)
			return CXTPMarkupEnum::CreateValue(xtpMarkupDockLeft);

		if (nLength == 5 && _wcsicmp(lpszValue, L"Right") == 0)
			return CXTPMarkupEnum::CreateValue(xtpMarkupDockRight);

		if (nLength == 3 && _wcsicmp(lpszValue, L"Top") == 0)
			return CXTPMarkupEnum::CreateValue(xtpMarkupDockTop);

		if (nLength == 6 && _wcsicmp(lpszValue, L"Bottom") == 0)
			return CXTPMarkupEnum::CreateValue(xtpMarkupDockBottom);
	}

	return NULL;
}

void CXTPMarkupDockPanel::OnDockChanged(CXTPMarkupObject* d,
										CXTPMarkupPropertyChangedEventArgs* /*e*/)
{
	CXTPMarkupUIElement* pVisual = MARKUP_DYNAMICCAST(CXTPMarkupUIElement, d);
	if (!pVisual)
		return;

	CXTPMarkupDockPanel* pPanel = MARKUP_DYNAMICCAST(CXTPMarkupDockPanel,
													 pVisual->GetVisualParent());
	if (!pPanel)
		return;

	pPanel->InvalidateMeasure();
}

XTPMarkupDock CXTPMarkupDockPanel::GetDock(CXTPMarkupUIElement* pElement)
{
	CXTPMarkupEnum* pDock = MARKUP_STATICCAST(CXTPMarkupEnum, pElement->GetValue(m_pDockProperty));
	return pDock != NULL ? (XTPMarkupDock)(int)*pDock : xtpMarkupDockLeft;
}

void CXTPMarkupDockPanel::SetDock(CXTPMarkupUIElement* pElement, XTPMarkupDock dock)
{
	pElement->SetValue(m_pDockProperty, new CXTPMarkupEnum(dock));
}

CSize CXTPMarkupDockPanel::MeasureOverride(CXTPMarkupDrawingContext* pDC, CSize constraint)
{
	int nMaxWidth	 = 0;
	int nMaxHeight	 = 0;
	int nTotalWidth	 = 0;
	int nTotalHeight = 0;

	int nCount = m_pChildren->GetCount();
	for (int i = 0; i < nCount; i++)
	{
		CXTPMarkupUIElement* pElement = m_pChildren->GetItem(i);
		if (pElement == NULL)
			continue;

		CSize availableSize(max(0, constraint.cx - nTotalWidth),
							max(0, constraint.cy - nTotalHeight));
		pElement->Measure(pDC, availableSize);

		CSize desiredSize = pElement->GetDesiredSize();
		switch (GetDock(pElement))
		{
			case xtpMarkupDockLeft:
			case xtpMarkupDockRight:
				nMaxHeight = max(nMaxHeight, nTotalHeight + desiredSize.cy);
				nTotalWidth += desiredSize.cx;
				break;

			case xtpMarkupDockTop:
			case xtpMarkupDockBottom:
				nMaxWidth = max(nMaxWidth, nTotalWidth + desiredSize.cx);
				nTotalHeight += desiredSize.cy;
				break;
		}
	}

	return CSize(max(nMaxWidth, nTotalWidth), max(nMaxHeight, nTotalHeight));
}

CSize CXTPMarkupDockPanel::ArrangeOverride(CSize arrangeSize)
{
	int nCount = m_pChildren->GetCount();

	int nLastChild = nCount - (GetLastChildFill() ? 1 : 0);
	int xLeft = 0, yTop = 0, xRight = 0, yBottom = 0;

	for (int i = 0; i < nCount; i++)
	{
		CXTPMarkupUIElement* pElement = m_pChildren->GetItem(i);
		if (pElement == NULL)
			continue;

		CSize desiredSize = pElement->GetDesiredSize();
		CRect rcFinalRect(xLeft, yTop, xLeft + max(0, arrangeSize.cx - (xLeft + xRight)),
						  yTop + max(0, arrangeSize.cy - (yTop + yBottom)));

		if (i < nLastChild)
		{
			switch (GetDock(pElement))
			{
				case xtpMarkupDockLeft:
					xLeft += desiredSize.cx;
					rcFinalRect.right = rcFinalRect.left + desiredSize.cx;
					break;

				case xtpMarkupDockTop:
					yTop += desiredSize.cy;
					rcFinalRect.bottom = rcFinalRect.top + desiredSize.cy;
					break;

				case xtpMarkupDockRight:
					xRight += desiredSize.cx;
					rcFinalRect.left  = max(0, arrangeSize.cx - xRight);
					rcFinalRect.right = rcFinalRect.left + desiredSize.cx;
					break;

				case xtpMarkupDockBottom:
					yBottom += desiredSize.cy;
					rcFinalRect.top	   = max(0, arrangeSize.cy - yBottom);
					rcFinalRect.bottom = rcFinalRect.top + desiredSize.cy;
					break;
			}
		}
		pElement->Arrange(rcFinalRect);
	}
	return arrangeSize;
}

void CXTPMarkupDockPanel::SetLastChildFill(BOOL bLastChildFill)
{
	SetValue(m_pLastChildFillProperty, CXTPMarkupBool::CreateValue(bLastChildFill));
}

BOOL CXTPMarkupDockPanel::GetLastChildFill() const
{
	CXTPMarkupBool* pLastChildFill = MARKUP_STATICCAST(CXTPMarkupBool,
													   GetValue(m_pLastChildFillProperty));
	return pLastChildFill != NULL ? (BOOL)*pLastChildFill : TRUE;
}

//////////////////////////////////////////////////////////////////////////
// CXTPMarkupDockPanel

#include "Common/Base/Diagnostic/XTPBeginAfxMap.h"
BEGIN_DISPATCH_MAP(CXTPMarkupDockPanel, CXTPMarkupPanel)
	DISP_PROPERTY_EX_ID(CXTPMarkupDockPanel, "LastChildFill", 600, OleGetLastChildFill,
						OleSetLastChildFill, VT_BOOL)
	DISP_FUNCTION_ID(CXTPMarkupDockPanel, "GetDock", 601, OleGetDock, VT_I4, VTS_DISPATCH)
	DISP_FUNCTION_ID(CXTPMarkupDockPanel, "SetDock", 602, OleSetDock, VT_EMPTY, VTS_DISPATCH VTS_I4)
END_DISPATCH_MAP()
#include "Common/Base/Diagnostic/XTPEndAfxMap.h"

#ifdef _XTP_ACTIVEX

BEGIN_INTERFACE_MAP(CXTPMarkupDockPanel, CXTPMarkupPanel)
	INTERFACE_PART(CXTPMarkupDockPanel, XTPDIID_MarkupDockPanel, Dispatch)
END_INTERFACE_MAP()

IMPLEMENT_OLETYPELIB_EX(CXTPMarkupDockPanel, XTPDIID_MarkupDockPanel)
#endif

BOOL CXTPMarkupDockPanel::OleGetLastChildFill()
{
	return GetLastChildFill();
}

void CXTPMarkupDockPanel::OleSetLastChildFill(BOOL nValue)
{
	SetLastChildFill(nValue);
}

int CXTPMarkupDockPanel::OleGetDock(LPDISPATCH lpElementDisp)
{
	return GetDock(MARKUP_STATICCAST(CXTPMarkupUIElement, FromDispatch(lpElementDisp, FALSE)));
}

void CXTPMarkupDockPanel::OleSetDock(LPDISPATCH lpElementDisp, int nValue)
{
	SetDock(MARKUP_STATICCAST(CXTPMarkupUIElement, FromDispatch(lpElementDisp, FALSE)),
			(XTPMarkupDock)nValue);
}
