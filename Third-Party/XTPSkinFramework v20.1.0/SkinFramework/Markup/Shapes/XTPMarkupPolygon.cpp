// XTPMarkupPolygon.cpp: implementation of the CXTPMarkupPolygon class.
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
#include "Markup/XTPMarkupUIElement.h"
#include "Markup/XTPMarkupFrameworkElement.h"
#include "Markup/Shapes/XTPMarkupShape.h"
#include "Markup/Shapes/XTPMarkupPolygon.h"
#include "Markup/XTPMarkupDrawingContext.h"
#include "Markup/XTPMarkupIIDs.h"

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

#ifdef _DEBUG
#	undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#	define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////////
// CXTPMarkupPolygon

CXTPMarkupDependencyProperty* CXTPMarkupPolygon::m_pPointsProperty = NULL;

IMPLEMENT_MARKUPCLASS(L"Polygon", CXTPMarkupPolygon, CXTPMarkupShape)

void CXTPMarkupPolygon::RegisterMarkupClass()
{
	m_pPointsProperty = CXTPMarkupDependencyProperty::Register(
		L"Points", MARKUP_TYPE(CXTPMarkupPointCollection), MARKUP_TYPE(CXTPMarkupPolygon),
		new CXTPMarkupPropertyMetadata(NULL, CXTPMarkupPropertyMetadata::flagAffectsMeasure));
}

CXTPMarkupPolygon::CXTPMarkupPolygon()
{
}

CXTPMarkupPolygon::~CXTPMarkupPolygon()
{
}

void CXTPMarkupPolygon::SetPoints(CXTPMarkupPointCollection* pPoints)
{
	SetValue(m_pPointsProperty, pPoints);
}

CXTPMarkupPointCollection* CXTPMarkupPolygon::GetPoints() const
{
	return MARKUP_STATICCAST(CXTPMarkupPointCollection, GetValue(m_pPointsProperty));
}

CSize CXTPMarkupPolygon::MeasureOverride(CXTPMarkupDrawingContext* /*pDC*/, CSize szAvailableSize)
{
	CXTPMarkupPointCollection* pPoints = GetPoints();
	if (!pPoints)
		return CSize(0, 0);
	CRect rcBounds = pPoints->GetBounds();

	CSize szBound(max(rcBounds.Width(), rcBounds.right), max(rcBounds.Height(), rcBounds.bottom));

	if (GetStretch() != xtpMarkupStretchNone)
	{
		if (szAvailableSize.cx < INT_MAX)
			szBound.cx = szAvailableSize.cx;

		if (szAvailableSize.cy < INT_MAX)
			szBound.cy = szAvailableSize.cy;
	}
	return szBound;
}

void CXTPMarkupPolygon::OnRender(CXTPMarkupDrawingContext* pDC)
{
	CXTPMarkupStrokeStyle strokeStyle;
	GetStrokeStyle(&strokeStyle);

	CXTPMarkupBrush* pFillBrush = GetFill();

	if (!strokeStyle.pStrokeBrush && !pFillBrush)
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

		pDC->Polygon(arrStretch.GetData(), (int)arrStretch.GetSize(), &strokeStyle, pFillBrush);
	}
	else
	{
		pDC->Polygon(arr.GetData(), (int)arr.GetSize(), &strokeStyle, pFillBrush);
	}

	pDC->SetSmoothingMode(oldSmoothingMode);
}

/////////////////////////////////////////////////////////////////////////
// Polygon

BEGIN_DISPATCH_MAP(CXTPMarkupPolygon, CXTPMarkupShape)
END_DISPATCH_MAP()

#ifdef _XTP_ACTIVEX

BEGIN_INTERFACE_MAP(CXTPMarkupPolygon, CXTPMarkupFrameworkElement)
	INTERFACE_PART(CXTPMarkupPolygon, XTPDIID_MarkupPolygon, Dispatch)
END_INTERFACE_MAP()

IMPLEMENT_OLETYPELIB_EX(CXTPMarkupPolygon, XTPDIID_MarkupPolygon)
#endif
