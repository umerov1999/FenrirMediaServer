// XTPMarkupPolyline.cpp: implementation of the CXTPMarkupPolyline class.
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
#include "Markup/Shapes/XTPMarkupPolyline.h"
#include "Markup/XTPMarkupIIDs.h"

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

#ifdef _DEBUG
#	undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#	define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////////
// CXTPMarkupPolyline

CXTPMarkupDependencyProperty* CXTPMarkupPolyline::m_pPointsProperty = NULL;

IMPLEMENT_MARKUPCLASS(L"Polyline", CXTPMarkupPolyline, CXTPMarkupShape)

void CXTPMarkupPolyline::RegisterMarkupClass()
{
	m_pPointsProperty = CXTPMarkupDependencyProperty::Register(
		L"Points", MARKUP_TYPE(CXTPMarkupPointCollection), MARKUP_TYPE(CXTPMarkupPolyline),
		new CXTPMarkupPropertyMetadata(NULL, CXTPMarkupPropertyMetadata::flagAffectsMeasure));
}

CXTPMarkupPolyline::CXTPMarkupPolyline()
{
}

CXTPMarkupPolyline::~CXTPMarkupPolyline()
{
}

void CXTPMarkupPolyline::SetPoints(CXTPMarkupPointCollection* pPoints)
{
	SetValue(m_pPointsProperty, pPoints);
}

CXTPMarkupPointCollection* CXTPMarkupPolyline::GetPoints() const
{
	return MARKUP_STATICCAST(CXTPMarkupPointCollection, GetValue(m_pPointsProperty));
}

CSize CXTPMarkupPolyline::MeasureOverride(CXTPMarkupDrawingContext* /*pDC*/, CSize szAvailableSize)
{
	CXTPMarkupPointCollection* pPoints = GetPoints();
	if (!pPoints)
		return CSize(0, 0);
	CRect rcBounds = pPoints->GetBounds();

	CSize szBound(max(rcBounds.Width(), rcBounds.right), max(rcBounds.Height(), rcBounds.bottom));

	if ((szBound.cx == 0 || szBound.cy == 0) && GetStroke())
	{
		int nStroke = (GetStrokeThickness() + 1) / 2;

		if (szBound.cy == 0)
			szBound.cy += nStroke;
		if (szBound.cx == 0)
			szBound.cx += nStroke;
	}

	if (GetStretch() != xtpMarkupStretchNone)
	{
		if (szAvailableSize.cx < INT_MAX)
			szBound.cx = szAvailableSize.cx;

		if (szAvailableSize.cy < INT_MAX)
			szBound.cy = szAvailableSize.cy;
	}
	return szBound;
}

void CXTPMarkupPolyline::OnRender(CXTPMarkupDrawingContext* pDC)
{
	CXTPMarkupStrokeStyle strokeStyle;
	GetStrokeStyle(&strokeStyle);

	CXTPMarkupBrush* pFullBrush = GetFill();

	if (strokeStyle.nStrokeThickness <= 0 && NULL == pFullBrush)
		return;

	CXTPMarkupPointCollection* pPoints = GetPoints();
	if (!pPoints)
		return;

	const CXTPMarkupPointCollection::CPointArray& arr = pPoints->GetPoints();

	XTPMarkupSmoothingMode oldSmoothingMode = pDC->SetSmoothingMode(GetSmoothingMode());

	if (GetStretch() != xtpMarkupStretchNone)
	{
		CXTPMarkupPointCollection::CPointArray arrStretch;
		pPoints->Stretch(arrStretch, GetRenderSize());

		if (0 < strokeStyle.nStrokeThickness)
		{
			pDC->Polyline(arrStretch.GetData(), (int)arrStretch.GetSize(), &strokeStyle);
		}

		if (NULL == pFullBrush)
		{
			pDC->Polygon(arrStretch.GetData(), (int)arrStretch.GetSize(), NULL, pFullBrush);
		}
	}
	else
	{
		if (0 < strokeStyle.nStrokeThickness)
		{
			pDC->Polyline(arr.GetData(), (int)arr.GetSize(), &strokeStyle);
		}

		if (NULL != pFullBrush)
		{
			pDC->Polygon(arr.GetData(), (int)arr.GetSize(), NULL, pFullBrush);
		}
	}

	pDC->SetSmoothingMode(oldSmoothingMode);
}

/////////////////////////////////////////////////////////////////////////
// Polyline

BEGIN_DISPATCH_MAP(CXTPMarkupPolyline, CXTPMarkupShape)
END_DISPATCH_MAP()

#ifdef _XTP_ACTIVEX

BEGIN_INTERFACE_MAP(CXTPMarkupPolyline, CXTPMarkupFrameworkElement)
	INTERFACE_PART(CXTPMarkupPolyline, XTPDIID_MarkupPolyline, Dispatch)
END_INTERFACE_MAP()

IMPLEMENT_OLETYPELIB_EX(CXTPMarkupPolyline, XTPDIID_MarkupPolyline)
#endif
