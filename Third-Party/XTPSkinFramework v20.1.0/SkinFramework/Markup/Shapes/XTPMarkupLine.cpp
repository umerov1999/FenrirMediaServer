// XTPMarkupLine.cpp: implementation of the CXTPMarkupLine class.
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
#include <math.h>
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
#include "Markup/XTPMarkupUIElement.h"
#include "Markup/XTPMarkupFrameworkElement.h"
#include "Markup/Shapes/XTPMarkupShape.h"
#include "Markup/Shapes/XTPMarkupLine.h"
#include "Markup/XTPMarkupDrawingContext.h"
#include "Markup/XTPMarkupIIDs.h"

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

#ifdef _DEBUG
#	undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#	define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////////
// CXTPMarkupLine

CXTPMarkupDependencyProperty* CXTPMarkupLine::m_pX1Property = NULL;
CXTPMarkupDependencyProperty* CXTPMarkupLine::m_pY1Property = NULL;
CXTPMarkupDependencyProperty* CXTPMarkupLine::m_pX2Property = NULL;
CXTPMarkupDependencyProperty* CXTPMarkupLine::m_pY2Property = NULL;

IMPLEMENT_MARKUPCLASS(L"Line", CXTPMarkupLine, CXTPMarkupShape)

void CXTPMarkupLine::RegisterMarkupClass()
{
	m_pX1Property = CXTPMarkupDependencyProperty::Register(
		L"X1", MARKUP_TYPE(CXTPMarkupInt), MARKUP_TYPE(CXTPMarkupLine),
		new CXTPMarkupPropertyMetadata(NULL,
									   CXTPMarkupPropertyMetadata::flagAffectsMeasure
										   | CXTPMarkupPropertyMetadata::flagHorzDpiSensible));

	m_pY1Property = CXTPMarkupDependencyProperty::Register(
		L"Y1", MARKUP_TYPE(CXTPMarkupInt), MARKUP_TYPE(CXTPMarkupLine),
		new CXTPMarkupPropertyMetadata(NULL,
									   CXTPMarkupPropertyMetadata::flagAffectsMeasure
										   | CXTPMarkupPropertyMetadata::flagVertDpiSensible));

	m_pX2Property = CXTPMarkupDependencyProperty::Register(
		L"X2", MARKUP_TYPE(CXTPMarkupInt), MARKUP_TYPE(CXTPMarkupLine),
		new CXTPMarkupPropertyMetadata(NULL,
									   CXTPMarkupPropertyMetadata::flagAffectsMeasure
										   | CXTPMarkupPropertyMetadata::flagHorzDpiSensible));

	m_pY2Property = CXTPMarkupDependencyProperty::Register(
		L"Y2", MARKUP_TYPE(CXTPMarkupInt), MARKUP_TYPE(CXTPMarkupLine),
		new CXTPMarkupPropertyMetadata(NULL,
									   CXTPMarkupPropertyMetadata::flagAffectsMeasure
										   | CXTPMarkupPropertyMetadata::flagVertDpiSensible));
}

CXTPMarkupLine::CXTPMarkupLine()
{
}

CXTPMarkupLine::~CXTPMarkupLine()
{
}

int CXTPMarkupLine::GetX1() const
{
	CXTPMarkupInt* pValue = MARKUP_STATICCAST(CXTPMarkupInt, GetValue(m_pX1Property));
	return pValue != NULL ? (int)*pValue : 0;
}

int CXTPMarkupLine::GetX2() const
{
	CXTPMarkupInt* pValue = MARKUP_STATICCAST(CXTPMarkupInt, GetValue(m_pX2Property));
	return pValue != NULL ? (int)*pValue : 0;
}

int CXTPMarkupLine::GetY1() const
{
	CXTPMarkupInt* pValue = MARKUP_STATICCAST(CXTPMarkupInt, GetValue(m_pY1Property));
	return pValue != NULL ? (int)*pValue : 0;
}

int CXTPMarkupLine::GetY2() const
{
	CXTPMarkupInt* pValue = MARKUP_STATICCAST(CXTPMarkupInt, GetValue(m_pY2Property));
	return pValue != NULL ? (int)*pValue : 0;
}

CSize CXTPMarkupLine::MeasureOverride(CXTPMarkupDrawingContext* /*pDC*/, CSize szAvailableSize)
{
	int x1 = GetX1();
	int x2 = GetX2();
	int y1 = GetY1();
	int y2 = GetY2();

	int x = max(x1, x2);
	int y = max(y1, y2);

	if ((x1 == x2 || y1 == y2) && GetStroke())
	{
		int nStroke = (GetStrokeThickness() + 1) / 2;

		if (x1 == x2)
			x += nStroke;
		if (y1 == y2)
			y += nStroke;
	}

	CSize szBound(x, y);

	if (GetStretch() != xtpMarkupStretchNone)
	{
		if (szAvailableSize.cx < INT_MAX)
			szBound.cx = szAvailableSize.cx;

		if (szAvailableSize.cy < INT_MAX)
			szBound.cy = szAvailableSize.cy;
	}
	return szBound;
}

void CXTPMarkupLine::GetPoints(int& x1, int& y1, int& x2, int& y2) const
{
	x1 = GetX1();
	x2 = GetX2();
	y1 = GetY1();
	y2 = GetY2();

	if (GetStretch() != xtpMarkupStretchNone)
	{
		int x = max(x1, x2);
		int y = max(y1, y2);

		CSize sz = GetRenderSize();
		if (x1 == x2)
			x1 = x2 = sz.cx / 2;
		else
		{
			x1 = (x1 == x ? sz.cx : 0);
			x2 = (x2 == x ? sz.cx : 0);
		}

		if (y1 == y2)
			y1 = y2 = sz.cy / 2;
		else
		{
			y1 = (y1 == y ? sz.cy : 0);
			y2 = (y2 == y ? sz.cy : 0);
		}
	}
}

void CXTPMarkupLine::OnRender(CXTPMarkupDrawingContext* pDC)
{
	CXTPMarkupStrokeStyle strokeStyle;
	GetStrokeStyle(&strokeStyle);

	if (strokeStyle.nStrokeThickness <= 0)
		return;

	int x1, x2, y1, y2;
	GetPoints(x1, y1, x2, y2);

	if (x1 == 0 && x2 == 0 && y1 == 0 && y2 == 0)
		return;

	XTPMarkupSmoothingMode oldSmoothingMode = pDC->SetSmoothingMode(GetSmoothingMode());
	pDC->DrawLine(x1, y1, x2, y2, &strokeStyle);
	pDC->SetSmoothingMode(oldSmoothingMode);
}

CXTPMarkupInputElement* CXTPMarkupLine::InputHitTestOverride(CPoint point) const
{
	if (!GetStroke())
		return NULL;

	int nStroke = GetStrokeThickness();
	if (nStroke == 0)
		return NULL;

	int x1, x2, y1, y2;
	GetPoints(x1, y1, x2, y2);

	double x = x2 - x1;
	double y = y2 - y1;

	double xp = point.x - x1;
	double yp = point.y - y1;

	double s = (double)nStroke / 2.0;

	if (xp + s < min(x, 0) || xp - s > max(x, 0) || yp + s < min(y, 0) || yp - s > max(y, 0))
		return NULL;

	if (x == 0)
		return fabs(xp) <= s ? (CXTPMarkupInputElement*)this : NULL;

	double yStroke = fabs(s / cos(atan(y / x)));

	double y0 = y * xp / (double)x;

	return fabs(y0 - yp) <= yStroke ? (CXTPMarkupInputElement*)this : NULL;
}

/////////////////////////////////////////////////////////////////////////
// Line

#include "Common/Base/Diagnostic/XTPBeginAfxMap.h"
BEGIN_DISPATCH_MAP(CXTPMarkupLine, CXTPMarkupShape)
	DISP_PROPERTY_EX_ID(CXTPMarkupLine, "X1", 600, OleGetX1, OleSetX1, VT_I4)
	DISP_PROPERTY_EX_ID(CXTPMarkupLine, "X2", 601, OleGetX2, OleSetX2, VT_I4)
	DISP_PROPERTY_EX_ID(CXTPMarkupLine, "Y1", 602, OleGetY1, OleSetY1, VT_I4)
	DISP_PROPERTY_EX_ID(CXTPMarkupLine, "Y2", 603, OleGetY2, OleSetY2, VT_I4)
END_DISPATCH_MAP()
#include "Common/Base/Diagnostic/XTPEndAfxMap.h"

#ifdef _XTP_ACTIVEX

BEGIN_INTERFACE_MAP(CXTPMarkupLine, CXTPMarkupFrameworkElement)
	INTERFACE_PART(CXTPMarkupLine, XTPDIID_MarkupLine, Dispatch)
END_INTERFACE_MAP()

IMPLEMENT_OLETYPELIB_EX(CXTPMarkupLine, XTPDIID_MarkupLine)
#endif

int CXTPMarkupLine::OleGetX1()
{
	return GetX1();
}

int CXTPMarkupLine::OleGetX2()
{
	return GetX2();
}

int CXTPMarkupLine::OleGetY1()
{
	return GetY1();
}

int CXTPMarkupLine::OleGetY2()
{
	return GetY2();
}

void CXTPMarkupLine::OleSetX1(int nValue)
{
	SetX1(nValue);
}

void CXTPMarkupLine::OleSetX2(int nValue)
{
	SetX2(nValue);
}

void CXTPMarkupLine::OleSetY1(int nValue)
{
	SetY1(nValue);
}

void CXTPMarkupLine::OleSetY2(int nValue)
{
	SetY2(nValue);
}
