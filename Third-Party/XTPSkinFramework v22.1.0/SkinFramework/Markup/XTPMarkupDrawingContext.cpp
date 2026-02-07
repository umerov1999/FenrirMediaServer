// XTPMarkupDrawingContext.cpp: implementation of the CXTPMarkupDrawingContext class.
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

#include "GraphicLibrary/GdiPlus/XTPGdiPlus.h"

#include "Common/XTPTypeId.h"
#include "Common/XTPCasting.h"
#include "Common/XTPFramework.h"
#include "Common/XTPSystemHelpers.h"
#include "Common/XTPSynchro.h"
#include "Common/XTPApplication.h"
#include "Common/XTPSingleton.h"
#include "Common/XTPGdiObjects.h"
#include "Common/XTPIntel80Helpers.h"
#include "Common/XTPDrawHelpers.h"
#include "Common/Math/XTPMathUtils.h"
#include "Common/Base/Types/XTPSize.h"

using namespace Gdiplus;
using namespace Gdiplus::DllExports;

#include "Markup/XTPMarkupContext.h"
#include "Markup/XTPMarkupObject.h"
#include "Markup/XTPMarkupDrawingContext.h"
#include "Markup/XTPMarkupBuilder.h"
#include "Markup/XTPMarkupThickness.h"
#include "Markup/XTPMarkupString.h"
#include "Markup/XTPMarkupTypedSimpleStack.h"
#include "Markup/DeviceContext/XTPMarkupDeviceContext.h"
#include "Markup/DeviceContext/XTPGdiPlus.h"
#include "Markup/DeviceContext/XTPMarkupGdiPlusDeviceContext.h"
#include "Markup/XTPMarkupIIDs.h"

#pragma warning(disable : 4510)
#pragma warning(disable : 4610)

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

#ifdef _DEBUG
#	undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#	define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////////
// CXTPMarkupBrushKey

IMPLEMENT_MARKUPCLASS(NULL, CXTPMarkupBrushKey, CXTPMarkupObject);

void CXTPMarkupBrushKey::RegisterMarkupClass()
{
}

//////////////////////////////////////////////////////////////////////////
// CXTPMarkupBrush

IMPLEMENT_MARKUPCLASS(NULL, CXTPMarkupBrush, CXTPMarkupObject);

void CXTPMarkupBrush::RegisterMarkupClass()
{
}

COLORREF CXTPMarkupBrush::GetHintColor() const
{
	return 0;
}

CXTPMarkupObject* CXTPMarkupBrush::ConvertFrom(CXTPMarkupBuilder* pBuilder,
											   CXTPMarkupObject* pObject) const
{
	UNREFERENCED_PARAMETER(pBuilder);

	if (IsStringObject(pObject))
	{
		COLORREF clr;
		if (CXTPMarkupColor::ConvertFromString(*((CXTPMarkupString*)pObject), clr))
		{
			return new CXTPMarkupSolidColorBrush((BYTE)(clr >> 24), clr & 0xFFFFFF);
		}
	}

	return NULL;
}

BEGIN_DISPATCH_MAP(CXTPMarkupBrush, CXTPMarkupObject)
END_DISPATCH_MAP()

#ifdef _XTP_ACTIVEX

BEGIN_INTERFACE_MAP(CXTPMarkupBrush, CXTPMarkupObject)
	INTERFACE_PART(CXTPMarkupBrush, XTPDIID_MarkupBrush, Dispatch)
END_INTERFACE_MAP()

IMPLEMENT_OLETYPELIB_EX(CXTPMarkupBrush, XTPDIID_MarkupBrush)
#endif

//////////////////////////////////////////////////////////////////////////
// CXTPMarkupGradientStop

CXTPMarkupDependencyProperty* CXTPMarkupGradientStop::m_pColorProperty  = NULL;
CXTPMarkupDependencyProperty* CXTPMarkupGradientStop::m_pOffsetProperty = NULL;

IMPLEMENT_MARKUPCLASS(L"GradientStop", CXTPMarkupGradientStop, CXTPMarkupObject);

void CXTPMarkupGradientStop::RegisterMarkupClass()
{
	m_pColorProperty  = CXTPMarkupDependencyProperty::Register(L"Color",
															   MARKUP_TYPE(CXTPMarkupColor),
															   MARKUP_TYPE(CXTPMarkupGradientStop));
	m_pOffsetProperty = CXTPMarkupDependencyProperty::Register(L"Offset",
															   MARKUP_TYPE(CXTPMarkupDouble),
															   MARKUP_TYPE(CXTPMarkupGradientStop));
}

CXTPMarkupGradientStop::CXTPMarkupGradientStop()
{
}

CXTPMarkupGradientStop::CXTPMarkupGradientStop(COLORREF clr, double dOffset)
{
	SetValue(m_pColorProperty, new CXTPMarkupColor(clr));
	SetValue(m_pOffsetProperty, new CXTPMarkupDouble(dOffset));
}

COLORREF CXTPMarkupGradientStop::GetColor() const
{
	CXTPMarkupColor* pColor = MARKUP_STATICCAST(CXTPMarkupColor, GetValue(m_pColorProperty));
	return pColor != NULL ? (COLORREF)*pColor : 0;
}

double CXTPMarkupGradientStop::GetOffset() const
{
	CXTPMarkupDouble* pOffset = MARKUP_STATICCAST(CXTPMarkupDouble, GetValue(m_pOffsetProperty));
	return pOffset != NULL ? (double)*pOffset : 0.0;
}

IMPLEMENT_MARKUPCLASS(L"", CXTPMarkupGradientStops, CXTPMarkupCollection);

void CXTPMarkupGradientStops::RegisterMarkupClass()
{
}

CXTPMarkupGradientStops::CXTPMarkupGradientStops()
{
	m_pElementType = MARKUP_TYPE(CXTPMarkupGradientStop);
}

//////////////////////////////////////////////////////////////////////////
// CXTPMarkupLinearGradientBrush

CXTPMarkupDependencyProperty* CXTPMarkupLinearGradientBrush::m_pStartPointProperty	= NULL;
CXTPMarkupDependencyProperty* CXTPMarkupLinearGradientBrush::m_pEndPointProperty	  = NULL;
CXTPMarkupDependencyProperty* CXTPMarkupLinearGradientBrush::m_pGradientStopsProperty = NULL;

IMPLEMENT_MARKUPCLASS(L"LinearGradientBrush", CXTPMarkupLinearGradientBrush, CXTPMarkupBrush);

void CXTPMarkupLinearGradientBrush::RegisterMarkupClass()
{
	m_pStartPointProperty = CXTPMarkupDependencyProperty::Register(
		L"StartPoint", MARKUP_TYPE(CXTPMarkupPoint), MARKUP_TYPE(CXTPMarkupLinearGradientBrush));
	m_pEndPointProperty = CXTPMarkupDependencyProperty::Register(
		L"EndPoint", MARKUP_TYPE(CXTPMarkupPoint), MARKUP_TYPE(CXTPMarkupLinearGradientBrush));

	m_pGradientStopsProperty = CXTPMarkupDependencyProperty::Register(
		L"GradientStops", MARKUP_TYPE(CXTPMarkupGradientStops),
		MARKUP_TYPE(CXTPMarkupLinearGradientBrush));
}

CXTPMarkupLinearGradientBrush::CXTPMarkupLinearGradientBrush()
{
	SetValue(m_pGradientStopsProperty, new CXTPMarkupGradientStops());
}

CXTPMarkupLinearGradientBrush::CXTPMarkupLinearGradientBrush(CXTPMarkupGradientStops* pGradientStops)
{
	SetValue(m_pGradientStopsProperty, pGradientStops);
}

COLORREF CXTPMarkupLinearGradientBrush::GetHintColor() const
{
	CXTPMarkupGradientStops* pGradientStops = GetGradientStops();

	if (!pGradientStops || pGradientStops->GetCount() == 0)
		return 0;

	return pGradientStops->GetItem(0)->GetColor() & 0xFFFFFF;
}

CXTPMarkupLinearGradientBrush::~CXTPMarkupLinearGradientBrush()
{
}

void CXTPMarkupLinearGradientBrush::SetStartPoint(double x, double y)
{
	SetValue(m_pStartPointProperty, new CXTPMarkupPoint(x, y));
}

void CXTPMarkupLinearGradientBrush::SetEndPoint(double x, double y)
{
	SetValue(m_pEndPointProperty, new CXTPMarkupPoint(x, y));
}

void CXTPMarkupLinearGradientBrush::SetContentObject(CXTPMarkupBuilder* pBuilder,
													 CXTPMarkupObject* pContent)
{
	CXTPMarkupGradientStops* pGradientStops = GetGradientStops();
	if (NULL != pGradientStops)
	{
		pGradientStops->SetContentObject(pBuilder, pContent);
	}
}

//////////////////////////////////////////////////////////////////////////
// CXTPMarkupPoint

IMPLEMENT_MARKUPCLASS(NULL, CXTPMarkupPoint, CXTPMarkupObject);

void CXTPMarkupPoint::RegisterMarkupClass()
{
}

CXTPMarkupPoint::CXTPMarkupPoint()
	: CXTPMarkupObject(CXTPMarkupObject::flagNoCom)
{
	x = 0;
	y = 0;
}

CXTPMarkupPoint::CXTPMarkupPoint(double x, double y)
	: CXTPMarkupObject(CXTPMarkupObject::flagNoCom)
{
	this->x = x;
	this->y = y;
}

CXTPMarkupPoint& CXTPMarkupPoint::operator=(const CXTPMarkupPoint& srcPoint)
{
	x = srcPoint.x;
	y = srcPoint.y;
	return *this;
}

CXTPMarkupObject* CXTPMarkupPoint::ConvertFrom(CXTPMarkupBuilder* pBuilder,
											   CXTPMarkupObject* pObject) const
{
	ASSERT(NULL != pBuilder);
	ASSERT(NULL != pObject);

	if (IsStringObject(pObject))
	{
		LPCWSTR lpszValue = *(CXTPMarkupString*)pObject;

		double dX = 0, dY = 0;
		if (!CXTPMarkupBuilder::ConvertDouble(lpszValue, dX, _T(','), FALSE))
			return NULL;

		if (!CXTPMarkupBuilder::ConvertDouble(lpszValue, dY))
			return NULL;

		if (NULL != m_pAssociatedProperty)
		{
			if (0
				!= (m_pAssociatedProperty->GetFlags()
					& CXTPMarkupPropertyMetadata::flagHorzDpiSensible))
			{
				dX = pBuilder->GetMarkupContext()->ScaleX(dX);
			}

			if (0
				!= (m_pAssociatedProperty->GetFlags()
					& CXTPMarkupPropertyMetadata::flagVertDpiSensible))
			{
				dY = pBuilder->GetMarkupContext()->ScaleY(dY);
			}
		}

		return new CXTPMarkupPoint(dX, dY);
	}

	return NULL;
}

//////////////////////////////////////////////////////////////////////////
// CXTPMarkupPointCollection

IMPLEMENT_MARKUPCLASS(NULL, CXTPMarkupPointCollection, CXTPMarkupObject)

void CXTPMarkupPointCollection::RegisterMarkupClass()
{
}

CXTPMarkupPointCollection::CXTPMarkupPointCollection()
	: CXTPMarkupObject(CXTPMarkupObject::flagNoCom)
{
	m_rcBounds.SetRectEmpty();
}

CXTPMarkupPointCollection::CXTPMarkupPointCollection(CPointArray& arr)
{
	m_rcBounds.SetRectEmpty();

	m_arrPoints.Copy(arr);

	int nCount = (int)m_arrPoints.GetSize();

	if (nCount > 0)
	{
		m_rcBounds.SetRect(INT_MAX, INT_MAX, -INT_MAX, -INT_MAX);

		for (int i = 0; i < nCount; i++)
		{
			Gdiplus::GpPointF& pt = m_arrPoints[i];

			if (pt.X > static_cast<REAL>(m_rcBounds.right))
				m_rcBounds.right = CXTPMathUtils::Round(pt.X);
			if (pt.Y > static_cast<REAL>(m_rcBounds.bottom))
				m_rcBounds.bottom = CXTPMathUtils::Round(pt.Y);
			if (pt.X < static_cast<REAL>(m_rcBounds.left))
				m_rcBounds.left = CXTPMathUtils::Round(pt.X);
			if (pt.Y < static_cast<REAL>(m_rcBounds.top))
				m_rcBounds.top = CXTPMathUtils::Round(pt.Y);
		}
	}
}

BOOL CXTPMarkupPointCollection::GetNextValue(LPCWSTR& lpszValue, float& fValue)
{
	while (*lpszValue == ' ')
		lpszValue++;
	LPCWSTR lpszNext = lpszValue;

	fValue	= 0;
	int nSign = 1;
	if (*lpszNext == '-')
	{
		nSign = -1;
		lpszNext++;
	}

	if (*lpszNext == 0)
		return FALSE;

	BOOL bDecimal  = 0;
	float dDecimal = 0;

	while (*lpszNext != 0)
	{
		WCHAR c = *lpszNext;

		if (c == ' ' || c == ',')
		{
			if (lpszNext == lpszValue)
				return FALSE;
			lpszNext++;
			break;
		}

		if (c == '.')
		{
			if (bDecimal)
				return FALSE;
			bDecimal = TRUE;
			dDecimal = 1;

			if (lpszValue == lpszNext)
				lpszValue = ++lpszNext;
			else
				lpszNext++;

			continue;
		}

		if (c < '0' || c > '9')
			return FALSE;

		if (!bDecimal)
		{
			fValue = 10 * fValue + static_cast<float>(c - '0');
		}
		else
		{
			dDecimal /= 10;
			fValue += dDecimal * static_cast<float>(c - '0');
		}

		lpszNext++;
	}

	lpszValue = lpszNext;
	fValue *= static_cast<float>(nSign);

	return TRUE;
}

BOOL CXTPMarkupPointCollection::ConvertFromString(CXTPMarkupBuilder* pBuilder, LPCWSTR lpszValue,
												  CPointArray& arr)
{
	ASSERT(NULL != pBuilder);

	if (!lpszValue)
		return FALSE;

	while (*lpszValue != 0)
	{
		float x, y;
		if (!GetNextValue(lpszValue, x))
			return FALSE;

		if (!GetNextValue(lpszValue, y))
			return FALSE;

		while (*lpszValue == ' ')
			lpszValue++;

		Gdiplus::GpPointF pt;
		pt.X = pBuilder->GetMarkupContext()->ScaleX(x);
		pt.Y = pBuilder->GetMarkupContext()->ScaleY(y);
		arr.Add(pt);
	}

	return TRUE;
}

CXTPMarkupObject* CXTPMarkupPointCollection::ConvertFrom(CXTPMarkupBuilder* pBuilder,
														 CXTPMarkupObject* pObject) const
{
	if (IsStringObject(pObject))
	{
		CPointArray arr;
		if (ConvertFromString(pBuilder, *((CXTPMarkupString*)pObject), arr))
		{
			return new CXTPMarkupPointCollection(arr);
		}
	}

	return NULL;
}

void CXTPMarkupPointCollection::Stretch(CPointArray& arrStretch, CSize sz)
{
	const CPointArray& arr = GetPoints();
	arrStretch.Copy(arr);
	int nCount = (int)arrStretch.GetSize();

	CRect rc = GetBounds();
	if (rc.Width() == 0 && rc.Height() == 0)
		return;

	for (int i = 0; i < nCount; i++)
	{
		Gdiplus::GpPointF& pt = arrStretch[i];
		if (rc.Width() != 0)
			pt.X = (pt.X - static_cast<REAL>(rc.left)) * static_cast<REAL>(sz.cx)
				   / static_cast<REAL>(rc.Width());
		if (rc.Height() != 0)
			pt.Y = (pt.Y - static_cast<REAL>(rc.top)) * static_cast<REAL>(sz.cy)
				   / static_cast<REAL>(rc.Height());
	}
}

CRect CXTPMarkupPointCollection::GetBounds() const
{
	return m_rcBounds;
}

const CXTPMarkupPointCollection::CPointArray& CXTPMarkupPointCollection::GetPoints() const
{
	return m_arrPoints;
}

//////////////////////////////////////////////////////////////////////////
// CXTPMarkupSolidColorBrush

CXTPMarkupDependencyProperty* CXTPMarkupSolidColorBrush::m_pColorProperty = NULL;

IMPLEMENT_MARKUPCLASS(L"SolidColorBrush", CXTPMarkupSolidColorBrush, CXTPMarkupBrush);

void CXTPMarkupSolidColorBrush::RegisterMarkupClass()
{
	m_pColorProperty = CXTPMarkupDependencyProperty::Register(
		L"Color", MARKUP_TYPE(CXTPMarkupColor), MARKUP_TYPE(CXTPMarkupSolidColorBrush));
}

CXTPMarkupSolidColorBrush::CXTPMarkupSolidColorBrush()
{
}

CXTPMarkupSolidColorBrush::CXTPMarkupSolidColorBrush(COLORREF clr)
{
	SetValue(m_pColorProperty, new CXTPMarkupColor(clr | 0xFF000000));
}

CXTPMarkupSolidColorBrush::CXTPMarkupSolidColorBrush(BYTE nAlpha, COLORREF clr)
{
	SetValue(m_pColorProperty, new CXTPMarkupColor(nAlpha, clr));
}

BOOL CXTPMarkupSolidColorBrush::IsEqual(const CXTPMarkupObject* pObject) const
{
	if (!pObject)
		return FALSE;

	if (pObject->GetType() != MARKUP_TYPE(CXTPMarkupSolidColorBrush))
		return FALSE;

	return GetHintColor() == ((CXTPMarkupSolidColorBrush*)pObject)->GetHintColor();
}

COLORREF CXTPMarkupSolidColorBrush::GetHintColor() const
{
	CXTPMarkupColor* pColor = MARKUP_STATICCAST(CXTPMarkupColor, GetValue(m_pColorProperty));

	if (pColor)
	{
		return (*pColor) & 0xFFFFFF;
	}

	return 0;
}

CXTPMarkupColor* CXTPMarkupSolidColorBrush::GetColor() const
{
	return MARKUP_STATICCAST(CXTPMarkupColor, GetValue(m_pColorProperty));
}

#include "Common/Base/Diagnostic/XTPBeginAfxMap.h"
BEGIN_DISPATCH_MAP(CXTPMarkupSolidColorBrush, CXTPMarkupBrush)
	DISP_PROPERTY_EX_ID(CXTPMarkupSolidColorBrush, "Color", 100, OleGetColor, OleSetColor, VT_COLOR)
END_DISPATCH_MAP()
#include "Common/Base/Diagnostic/XTPEndAfxMap.h"

#ifdef _XTP_ACTIVEX

BEGIN_INTERFACE_MAP(CXTPMarkupSolidColorBrush, CXTPMarkupBrush)
	INTERFACE_PART(CXTPMarkupSolidColorBrush, XTPDIID_MarkupSolidColorBrush, Dispatch)
END_INTERFACE_MAP()

IMPLEMENT_OLETYPELIB_EX(CXTPMarkupSolidColorBrush, XTPDIID_MarkupSolidColorBrush)
#endif

COLORREF CXTPMarkupSolidColorBrush::OleGetColor()
{
	CXTPMarkupColor* pColor = MARKUP_STATICCAST(CXTPMarkupColor, GetValue(m_pColorProperty));
	return pColor != NULL ? (COLORREF)*pColor : 0xFF000000;
}

void CXTPMarkupSolidColorBrush::OleSetColor(COLORREF clr)
{
	int nAlpha = static_cast<int>(clr >> 24);

	if (nAlpha == 0)
		nAlpha = 0xFF;

	SetValue(m_pColorProperty, new CXTPMarkupColor((BYTE)nAlpha, clr));
}

//////////////////////////////////////////////////////////////////////////
// CXTPMarkupDrawingContext

void AFX_CDECL CXTPMarkupDrawingContext::Register(BOOL bInit)
{
#ifndef _XTP_EXCLUDE_GDIPLUS
	CXTPMarkupGdiPlusDeviceContext::Register(bInit);
#endif
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CXTPMarkupDrawingContext::CXTPMarkupDrawingContext(CXTPMarkupContext* pMarkupContext, HDC hDC,
												   HDC hAttribDC /* = 0 */)
	: m_hDC(hDC)
	, m_hAttribDC(hAttribDC ? hAttribDC : hDC)
	, m_bDeleteDC(FALSE)
	, m_pDeviceContext(NULL)
	, m_bUseTextSmoothing(FALSE)
{
	Init(pMarkupContext);
}

CXTPMarkupDrawingContext::CXTPMarkupDrawingContext(CXTPMarkupContext* pMarkupContext)
	: m_hDC(CreateCompatibleDC(0))
	, m_hAttribDC(m_hDC)
	, m_bDeleteDC(TRUE)
	, m_pDeviceContext(NULL)
	, m_bUseTextSmoothing(FALSE)
{
	Init(pMarkupContext);
}

CXTPMarkupDrawingContext::CXTPMarkupDrawingContext(HDC hDC)
	: m_hDC(hDC)
	, m_hAttribDC(m_hDC)
	, m_bDeleteDC(FALSE)
	, m_pDeviceContext(NULL)
	, m_bUseTextSmoothing(FALSE)
{
	Init(NULL);
}

CXTPMarkupDrawingContext::CXTPMarkupDrawingContext()
	: m_hDC(CreateCompatibleDC(0))
	, m_hAttribDC(m_hDC)
	, m_bDeleteDC(TRUE)
	, m_pDeviceContext(NULL)
	, m_bUseTextSmoothing(FALSE)
{
	Init(NULL);
}

CXTPMarkupDrawingContext::CXTPMarkupDrawingContext(CXTPMarkupContext* pMarkupContext,
												   GpGraphics* pGpGraphics)
{
	m_hDC		= NULL;
	m_hAttribDC = NULL;
	m_bDeleteDC = FALSE;

	m_pDeviceContext = new CXTPMarkupGdiPlusDeviceContext(pGpGraphics);

	m_pSelectedFont = NULL;
	m_hOldFont		= NULL;

	m_hDWP = NULL;

	m_rcClipBox.SetRectEmpty();
	m_pDeviceContext->GetClipBox(m_rcClipBox);

	m_pMarkupContext	= pMarkupContext;
	m_bUseTextSmoothing = FALSE;
}

CXTPMarkupDrawingContext::CXTPMarkupDrawingContext(CXTPMarkupContext* pMarkupContext,
												   Graphics* pGraphics)
{
	_XTP_USES_PROTECTED_ACCESS_ARGS(CXTPMarkupDrawingContext, Graphics, (HDC hDC), (hDC),
									GpGraphics, nativeGraphics);

	GpGraphics* pGpGraphics = _XTP_PROTECTED_ACCESS(Graphics, pGraphics, nativeGraphics);

	m_hDWP = NULL;

	m_hDC		= NULL;
	m_hAttribDC = NULL;
	m_bDeleteDC = FALSE;

	m_pDeviceContext = new CXTPMarkupGdiPlusDeviceContext(pGpGraphics);

	m_pSelectedFont = NULL;
	m_hOldFont		= NULL;

	m_rcClipBox.SetRectEmpty();
	m_pDeviceContext->GetClipBox(m_rcClipBox);

	m_pMarkupContext	= pMarkupContext;
	m_bUseTextSmoothing = FALSE;
}

void CXTPMarkupDrawingContext::Init(CXTPMarkupContext* pMarkupContext)
{
	ASSERT(NULL == m_pDeviceContext);

#ifndef _XTP_EXCLUDE_GDIPLUS
	if ((!pMarkupContext || pMarkupContext->IsGdiPlusEnabled())
		&& !XTPDrawHelpers()->IsContextRTL(m_hDC))
	{
		Gdiplus::GpGraphics* pGpGraphics =
			CXTPMarkupGdiPlusDeviceContext::CreateGraphics(m_hDC, m_hAttribDC);
		if (NULL != pGpGraphics)
		{
			m_pDeviceContext = new CXTPMarkupGdiPlusDeviceContext(pGpGraphics, FALSE);
		}
	}
#endif

	if (NULL == m_pDeviceContext)
	{
		m_pDeviceContext = new CXTPMarkupDeviceContext(m_hDC, m_hAttribDC);
	}

	if (pMarkupContext)
	{
		SetSmoothingMode(pMarkupContext->GetDefaultSmoothingMode(),
						 pMarkupContext->GetDefaultTextSmoothing());
	}

	m_pSelectedFont = NULL;
	m_hOldFont		= NULL;

	m_rcClipBox.SetRectEmpty();
	m_pDeviceContext->GetClipBox(m_rcClipBox);

	m_pMarkupContext = pMarkupContext;

	m_hDWP = NULL;

	::SetBkMode(m_hDC, TRANSPARENT);
}

CXTPMarkupDrawingContext::~CXTPMarkupDrawingContext()
{
	SetFont(NULL);

	delete m_pDeviceContext;

	if (m_bDeleteDC)
	{
		DeleteDC(m_hDC);
	}
}

HDC CXTPMarkupDrawingContext::Detach()
{
	SetFont(NULL);

	HDC hDC = m_hDC;
	m_hDC   = NULL;
	return hDC;
}

CXTPMarkupTransformationMatrix* CXTPMarkupDrawingContext::GetTransformationMatrix() const
{
	return m_pDeviceContext->GetTransformationMatrix();
}

void CXTPMarkupDrawingContext::SetTransformationMatrix(CXTPMarkupTransformationMatrix* pMatrix)
{
	m_pDeviceContext->SetTransformationMatrix(pMatrix);
}

void CXTPMarkupDrawingContext::OffsetViewport(const POINT& ptVisualOffset)
{
	m_pDeviceContext->OffsetViewport(ptVisualOffset);

	m_pDeviceContext->GetClipBox(m_rcClipBox);
}

XTPMarkupSmoothingMode
	CXTPMarkupDrawingContext::SetSmoothingMode(XTPMarkupSmoothingMode smoothingMode)
{
	return m_pDeviceContext->SetSmoothingMode(smoothingMode, m_bUseTextSmoothing);
}

XTPMarkupSmoothingMode CXTPMarkupDrawingContext::SetSmoothingMode(
	XTPMarkupSmoothingMode smoothingMode, BOOL bUseTextSmoothing)
{
	SetUseTextSmoothing(bUseTextSmoothing);
	return m_pDeviceContext->SetSmoothingMode(smoothingMode, GetUseTextSmoothing());
}

void CXTPMarkupDrawingContext::IntersectClipRect(const RECT& rcLayoutClip)
{
	m_pDeviceContext->IntersectClipRect(rcLayoutClip);

	m_pDeviceContext->GetClipBox(m_rcClipBox);
}

HRGN CXTPMarkupDrawingContext::SaveClipRegion()
{
	return m_pDeviceContext->SaveClipRegion();
}

void CXTPMarkupDrawingContext::RestoreClipRegion(HRGN hrgnClip)
{
	if (hrgnClip != NULL)
	{
		m_pDeviceContext->RestoreClipRegion(hrgnClip);

		m_pDeviceContext->GetClipBox(m_rcClipBox);
	}
}

void CXTPMarkupDrawingContext::FillSolidRect(CRect rc, COLORREF clr)
{
	CXTPMarkupSolidColorBrush brush(clr);
	m_pDeviceContext->FillRectangle(rc, &brush);
}

void CXTPMarkupDrawingContext::FillRectangle(CRect rc, CXTPMarkupBrush* pBrush,
											 double* pCornerRadius)
{
	if (pCornerRadius)
		m_pDeviceContext->FillRoundRectangle(rc, pBrush, pCornerRadius);
	else
		m_pDeviceContext->FillRectangle(rc, pBrush);
}

void CXTPMarkupDrawingContext::DrawFrame(CRect rc, CXTPMarkupBrush* pBrush)
{
	long xs = 1, ys = 1;

	if (NULL != m_pMarkupContext)
	{
		xs = m_pMarkupContext->ScaleX(static_cast<int>(xs));
		ys = m_pMarkupContext->ScaleY(static_cast<int>(ys));
	}

	CXTPMarkupThickness thickness(xs, ys);
	DrawRectangle(rc, pBrush, &thickness);
}

void CXTPMarkupDrawingContext::DrawTextLine(LPCWSTR lpszText, UINT nCount, LPCRECT lpRect)
{
	m_pDeviceContext->DrawString(lpszText, nCount, lpRect);
}

void CXTPMarkupDrawingContext::SetFont(CXTPMarkupFont* pFont)
{
	if (m_pSelectedFont == pFont)
		return;

	m_pDeviceContext->SetTextFont(pFont);

	MARKUP_RELEASE(m_pSelectedFont);
	m_pSelectedFont = pFont;
	MARKUP_ADDREF(m_pSelectedFont);
}

BOOL CXTPMarkupDrawingContext::GetFontMetrics(CXTPMarkupFont* pFont, XTPMarkupFontMetrics* pMetrics)
{
	return m_pDeviceContext->GetFontMetrics(pFont, pMetrics);
}

SIZE CXTPMarkupDrawingContext::GetTextExtent(LPCWSTR lpszText, int nCount) const
{
	return m_pDeviceContext->MeasureString(lpszText, nCount);
}

void CXTPMarkupDrawingContext::GetTextExtent(LPCWSTR lpszText, int nCount, CXTPSizeF& size) const
{
	m_pDeviceContext->MeasureString(lpszText, nCount, size);
}

void CXTPMarkupDrawingContext::DrawLine(int x1, int y1, int x2, int y2,
										CXTPMarkupStrokeStyle* pStrokeStyle)
{
	if (!pStrokeStyle || pStrokeStyle->nStrokeThickness <= 0)
		return;

	m_pDeviceContext->DrawLine(x1, y1, x2, y2, pStrokeStyle);
}

void CXTPMarkupDrawingContext::Polygon(const POINT* points, int nCount,
									   CXTPMarkupStrokeStyle* pStrokeStyle,
									   CXTPMarkupBrush* pFillBrush)
{
	m_pDeviceContext->Polygon(points, nCount, pStrokeStyle, pFillBrush);
}

void CXTPMarkupDrawingContext::Polygon(const void* points, int nCount,
									   CXTPMarkupStrokeStyle* pStrokeStyle,
									   CXTPMarkupBrush* pFillBrush)
{
	m_pDeviceContext->Polygon(points, nCount, pStrokeStyle, pFillBrush);
}

void CXTPMarkupDrawingContext::Polyline(const POINT* points, int nCount,
										CXTPMarkupStrokeStyle* pStrokeStyle)
{
	m_pDeviceContext->Polyline(points, nCount, pStrokeStyle);
}

void CXTPMarkupDrawingContext::Polyline(const void* points, int nCount,
										CXTPMarkupStrokeStyle* pStrokeStyle)
{
	m_pDeviceContext->Polyline(points, nCount, pStrokeStyle);
}

void CXTPMarkupDrawingContext::Ellipse(CRect rc, CXTPMarkupStrokeStyle* pStrokeStyle,
									   CXTPMarkupBrush* pFillBrush)
{
	m_pDeviceContext->Ellipse(rc, pStrokeStyle, pFillBrush);
}

void CXTPMarkupDrawingContext::DrawRectangle(CRect rc, CXTPMarkupBrush* pBrush,
											 CXTPMarkupThickness* pThickness, double* pCornerRadius)
{
	if (!pThickness || !pBrush)
		return;

	if (pCornerRadius)
		m_pDeviceContext->DrawRoundRectangle(rc, pBrush, pThickness, pCornerRadius);
	else
		m_pDeviceContext->DrawRectangle(rc, pBrush, pThickness);
}

void CXTPMarkupDrawingContext::DrawPath(CXTPMarkupPathData* pData,
										CXTPMarkupStrokeStyle* pStrokeStyle,
										CXTPMarkupBrush* pFillBrush)
{
	m_pDeviceContext->DrawPath(pData, pStrokeStyle, pFillBrush);
}

void CXTPMarkupDrawingContext::DrawImage(CXTPImageManagerIcon* pIcon, CRect rc)
{
	m_pDeviceContext->DrawImage(pIcon, rc);
}

void CXTPMarkupDrawingContext::DrawImage(CXTPMarkupDeviceDependentImage* pImage, CRect rc)
{
	m_pDeviceContext->DrawImage(pImage, rc);
}

HDC CXTPMarkupDrawingContext::GetDC()
{
	return m_pDeviceContext->GetDC();
}

void CXTPMarkupDrawingContext::ReleaseDC(HDC hDC)
{
	m_pDeviceContext->ReleaseDC(hDC);
}

void CXTPMarkupDrawingContext::SetTextColor(CXTPMarkupBrush* pBrush)
{
	m_pDeviceContext->SetTextColor(pBrush);
}

double CXTPMarkupDrawingContext::GetOpacity() const
{
	ASSERT(NULL != m_pDeviceContext);
	return m_pDeviceContext->GetOpacity();
}

void CXTPMarkupDrawingContext::SetOpacity(double dOpacity) const
{
	ASSERT(NULL != m_pDeviceContext);
	m_pDeviceContext->SetOpacity(dOpacity);
}

//////////////////////////////////////////////////////////////////////////
// CXTPMarkupFont

CXTPMarkupFont::CXTPMarkupFont()
{
	m_pMarkupContext = NULL;
	m_pNextChain	 = NULL;
}

CXTPMarkupFont::~CXTPMarkupFont()
{
	if (m_pMarkupContext)
	{
		VERIFY(m_pMarkupContext->m_pFonts->Remove(this));
	}
}

//////////////////////////////////////////////////////////////////////////
// CXTPMarkupPrintingContext

CXTPMarkupPrintingContext::CXTPMarkupPrintingContext(CXTPMarkupContext* pMarkupContext, HDC hDC,
													 HDC hAttribDC)
	: CXTPMarkupDrawingContext(pMarkupContext, hDC, hAttribDC)
{
	CXTPMarkupGdiPlusDeviceContext* pGdiPlusDC = DYNAMIC_DOWNCAST(CXTPMarkupGdiPlusDeviceContext,
																  m_pDeviceContext);
	if (pGdiPlusDC)
	{
		GdipSetPageUnit(pGdiPlusDC->GetGraphics(), UnitPixel);
	}
}
CXTPMarkupPrintingContext::CXTPMarkupPrintingContext(HDC hDC, HDC hAttribDC)
	: CXTPMarkupDrawingContext(NULL, hDC, hAttribDC)
{
	CXTPMarkupGdiPlusDeviceContext* pGdiPlusDC = DYNAMIC_DOWNCAST(CXTPMarkupGdiPlusDeviceContext,
																  m_pDeviceContext);
	if (pGdiPlusDC)
	{
		GdipSetPageUnit(pGdiPlusDC->GetGraphics(), UnitPixel);
	}
}
