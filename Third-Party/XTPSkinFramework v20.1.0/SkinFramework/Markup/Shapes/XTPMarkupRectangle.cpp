// XTPMarkupRectangle.cpp: implementation of the CXTPMarkupRectangle class.
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

#include "GraphicLibrary/GdiPlus/XTPGdiPlus.h"

#include "Common/XTPTypeId.h"
#include "Common/XTPFramework.h"
#include "Common/XTPSystemHelpers.h"
#include "Common/XTPSynchro.h"
#include "Common/XTPApplication.h"
#include "Common/XTPSingleton.h"
#include "Common/XTPGdiObjects.h"
#include "Common/Base/Types/XTPSize.h"

#include "Markup/XTPMarkupContext.h"
#include "Markup/XTPMarkupObject.h"
#include "Markup/XTPMarkupInputElement.h"
#include "Markup/XTPMarkupDrawingContext.h"
#include "Markup/XTPMarkupUIElement.h"
#include "Markup/XTPMarkupFrameworkElement.h"
#include "Markup/Shapes/XTPMarkupShape.h"
#include "Markup/Shapes/XTPMarkupRectangle.h"
#include "Markup/XTPMarkupThickness.h"
#include "Markup/XTPMarkupIIDs.h"

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

#ifdef _DEBUG
#	undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#	define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////////
// CXTPMarkupRectangle

IMPLEMENT_MARKUPCLASS(L"Rectangle", CXTPMarkupRectangle, CXTPMarkupShape)

CXTPMarkupDependencyProperty* CXTPMarkupRectangle::m_pRadiusXProperty = NULL;
CXTPMarkupDependencyProperty* CXTPMarkupRectangle::m_pRadiusYProperty = NULL;

void CXTPMarkupRectangle::RegisterMarkupClass()
{
	m_pRadiusXProperty = CXTPMarkupDependencyProperty::Register(
		L"RadiusX", MARKUP_TYPE(CXTPMarkupInt), MARKUP_TYPE(CXTPMarkupRectangle),
		new CXTPMarkupPropertyMetadata(NULL,
									   CXTPMarkupPropertyMetadata::flagAffectsRender
										   | CXTPMarkupPropertyMetadata::flagHorzDpiSensible));

	m_pRadiusYProperty = CXTPMarkupDependencyProperty::Register(
		L"RadiusY", MARKUP_TYPE(CXTPMarkupInt), MARKUP_TYPE(CXTPMarkupRectangle),
		new CXTPMarkupPropertyMetadata(NULL,
									   CXTPMarkupPropertyMetadata::flagAffectsRender
										   | CXTPMarkupPropertyMetadata::flagVertDpiSensible));
}

CXTPMarkupRectangle::CXTPMarkupRectangle()
{
}

CXTPMarkupRectangle::~CXTPMarkupRectangle()
{
}

CSize CXTPMarkupRectangle::MeasureOverride(CXTPMarkupDrawingContext* /*pDC*/, CSize szAvailableSize)
{
	CSize szBound(0, 0);

	if (GetStretch() != xtpMarkupStretchNone)
	{
		if (szAvailableSize.cx < INT_MAX)
			szBound.cx = szAvailableSize.cx;

		if (szAvailableSize.cy < INT_MAX)
			szBound.cy = szAvailableSize.cy;
	}

	return szBound;
}

void CXTPMarkupRectangle::OnRender(CXTPMarkupDrawingContext* pDC)
{
	CXTPMarkupBrush* pStrokeBrush = GetStroke();
	CXTPMarkupBrush* pFillBrush	  = GetFill();

	if (!pStrokeBrush && !pFillBrush)
		return;

	int nStroke = pStrokeBrush ? GetStrokeThickness() : 0;

	CRect rc(0, 0, GetRenderSize().cx, GetRenderSize().cy);
	if (rc.Width() == 0 || rc.Height() == 0)
		return;

	double nRadiusX = GetRadiusX(), nRadiusY = GetRadiusY();
	nRadiusX = min((double)(rc.Width() - nStroke) / 2, nRadiusX);
	nRadiusY = min((double)(rc.Height() - nStroke) / 2, nRadiusY);

	double* pnCorners = NULL;
	if (nRadiusX > 0 && nRadiusY > 0)
	{
		pnCorners	 = new double[8];
		pnCorners[0] = pnCorners[3] = pnCorners[4] = pnCorners[7] = (double)nRadiusY;
		pnCorners[1] = pnCorners[2] = pnCorners[5] = pnCorners[6] = (double)nRadiusX;
	}

	XTPMarkupSmoothingMode oldSmoothingMode = pDC->SetSmoothingMode(GetSmoothingMode());

	if (pFillBrush)
	{
		CRect rcFill(rc);
		rcFill.DeflateRect(nStroke / 2, nStroke / 2);

		pDC->FillRectangle(rcFill, pFillBrush, pnCorners);
	}

	if (nStroke > 0)
	{
		CXTPMarkupThickness strokeThickness(nStroke);
		pDC->DrawRectangle(rc, pStrokeBrush, &strokeThickness, pnCorners);
	}

	pDC->SetSmoothingMode(oldSmoothingMode);

	if (pnCorners)
		delete[] pnCorners;
}

int CXTPMarkupRectangle::GetRadiusX() const
{
	CXTPMarkupInt* pValue = MARKUP_STATICCAST(CXTPMarkupInt, GetValue(m_pRadiusXProperty));
	return pValue ? (int)*pValue : 0;
}

int CXTPMarkupRectangle::GetRadiusY() const
{
	CXTPMarkupInt* pValue = MARKUP_STATICCAST(CXTPMarkupInt, GetValue(m_pRadiusYProperty));
	return pValue ? (int)*pValue : 0;
}

#include "Common/Base/Diagnostic/XTPBeginAfxMap.h"
BEGIN_DISPATCH_MAP(CXTPMarkupRectangle, CXTPMarkupShape)
	DISP_PROPERTY_EX_ID(CXTPMarkupRectangle, "RadiusX", 600, OleGetRadiusX, OleSetRadiusX, VT_I4)
	DISP_PROPERTY_EX_ID(CXTPMarkupRectangle, "RadiusY", 601, OleGetRadiusY, OleSetRadiusY, VT_I4)
END_DISPATCH_MAP()
#include "Common/Base/Diagnostic/XTPEndAfxMap.h"

#ifdef _XTP_ACTIVEX

BEGIN_INTERFACE_MAP(CXTPMarkupRectangle, CXTPMarkupFrameworkElement)
	INTERFACE_PART(CXTPMarkupRectangle, XTPDIID_MarkupRectangle, Dispatch)
END_INTERFACE_MAP()

IMPLEMENT_OLETYPELIB_EX(CXTPMarkupRectangle, XTPDIID_MarkupRectangle)
#endif

int CXTPMarkupRectangle::OleGetRadiusX()
{
	return GetRadiusX();
}

void CXTPMarkupRectangle::OleSetRadiusX(int nValue)
{
	SetRadiusX(nValue);
}

int CXTPMarkupRectangle::OleGetRadiusY()
{
	return GetRadiusY();
}

void CXTPMarkupRectangle::OleSetRadiusY(int nValue)
{
	SetRadiusY(nValue);
}
