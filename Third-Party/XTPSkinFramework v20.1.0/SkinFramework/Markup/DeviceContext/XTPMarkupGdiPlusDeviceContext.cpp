// XTPMarkupGdiPlusDeviceContext.cpp: implementation of the CXTPMarkupGdiPlusDeviceContext class.
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
#include "float.h"

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
#include "Common/XTPDrawHelpers.h"
#include "Common/XTPImageManager.h"
#include "Common/XTPMathUtils.h"
#include "Common/Base/Types/XTPSize.h"

using namespace Gdiplus;
using namespace Gdiplus::DllExports;

#include "Markup/XTPMarkupContext.h"
#include "Markup/XTPMarkupObject.h"
#include "Markup/XTPMarkupDrawingContext.h"
#include "Markup/DeviceContext/XTPMarkupDeviceContext.h"
#include "Markup/XTPMarkupThickness.h"
#include "Markup/Shapes/XTPMarkupPathData.h"
#include "Markup/DeviceContext/XTPGdiPlus.h"
#include "Markup/DeviceContext/XTPMarkupGdiPlusDeviceContext.h"
#include "Markup/DeviceContext/XTPMarkupDeviceDependentImage.h"
#include "Markup/DeviceContext/XTPMarkupGdiPlusImage.h"
#include "Markup/Transform/XTPMarkupTransformationMatrix.h"
#include "Markup/Transform/XTPMarkupGdiPlusTransformationMatrix.h"

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

#ifdef _DEBUG
#	undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#	define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////////
//  CXTPMarkupGdiPlusDeviceContext::DedicatedDC

class CXTPMarkupGdiPlusDeviceContext::DedicatedDC : public CXTPMarkupDedicatedDC
{
public:
	DedicatedDC(CXTPMarkupGdiPlusDeviceContext* pDC, const RECT& rect)
		: CXTPMarkupDedicatedDC(pDC, rect)
		, m_pDCEx(pDC)
		, m_pOffscreenImage(NULL)
		, m_pOffscreenGraphics(NULL)
		, m_hOffscreenDC(NULL)
	{
		if (Gdiplus::Ok
			== GdipCreateBitmapFromScan0(m_rect.Width(), m_rect.Height(), 0, PixelFormat32bppPARGB,
										 NULL, reinterpret_cast<GpBitmap**>(&m_pOffscreenImage)))
		{
			if (Gdiplus::Ok
				== GdipGetImageGraphicsContext(m_pOffscreenImage, &m_pOffscreenGraphics))
			{
				GdipGetDC(m_pOffscreenGraphics, &m_hOffscreenDC);
			}
		}
	}

	~DedicatedDC()
	{
		if (NULL != m_pOffscreenGraphics)
		{
			if (NULL != m_hOffscreenDC)
			{
				GdipReleaseDC(m_pOffscreenGraphics, m_hOffscreenDC);
			}

			GdipDeleteGraphics(m_pOffscreenGraphics);
		}

		if (NULL != m_pOffscreenImage)
		{
			GdipDisposeImage(m_pOffscreenImage);
		}
	}

	virtual HDC GetDC() const
	{
		return m_hOffscreenDC;
	}

	virtual void Commit()
	{
		ASSERT(NULL != m_hOffscreenDC && "Already committed");

		if (NULL != m_hOffscreenDC)
		{
			GdipReleaseDC(m_pOffscreenGraphics, m_hOffscreenDC);
			m_hOffscreenDC = NULL;

			GdipDrawImageRect(m_pDCEx->GetGraphics(), m_pOffscreenImage,
							  static_cast<REAL>(m_rect.left), static_cast<REAL>(m_rect.top),
							  static_cast<REAL>(m_rect.Width()),
							  static_cast<REAL>(m_rect.Height()));
		}
	}

private:
	CXTPMarkupGdiPlusDeviceContext* m_pDCEx;
	GpImage* m_pOffscreenImage;
	GpGraphics* m_pOffscreenGraphics;
	HDC m_hOffscreenDC;
};

//////////////////////////////////////////////////////////////////////////
// CXTPMarkupGdiPlusDeviceContext

IMPLEMENT_DYNAMIC(CXTPMarkupGdiPlusDeviceContext, CXTPMarkupDeviceContext);

CXTPGdiPlus* AFX_CDECL CXTPMarkupGdiPlusDeviceContext::GetGdiPlus()
{
	return NULL;
}

void AFX_CDECL CXTPMarkupGdiPlusDeviceContext::Register(BOOL bInit)
{
	UNREFERENCED_PARAMETER(bInit);
}

CXTPMarkupGdiPlusDeviceContext::CXTPMarkupGdiPlusDeviceContext(GpGraphics* pGpGraphics,
															   BOOL bSharedAccess /*= TRUE*/)
	: CXTPMarkupDeviceContext(NULL, NULL)
	, m_defaultPixelOffsetMode(PixelOffsetModeDefault)
	, m_gdipSmoothingMode(SmoothingModeAntiAlias)
	, m_gdipInterpolationMode(InterpolationModeDefault)
	, m_gdipTextRenderingHint(TextRenderingHintSystemDefault)
	, m_bGpGraphicsSharedAccess(bSharedAccess)
	, m_pGraphics(pGpGraphics)
	, m_pGpFont(NULL)
	, m_pGpTextBrush(NULL)
	, m_pGpStringFormat(NULL)
{
	ASSERT(NULL != m_pGraphics);

	ASSERT("Currently RTL is not supported by GDI+ DC." && !IsRTL());

	Init();
}

CXTPMarkupGdiPlusDeviceContext::~CXTPMarkupGdiPlusDeviceContext()
{
	if (NULL != m_pGpFont)
	{
		GdipDeleteFont(m_pGpFont);
		m_pGpFont = NULL;
	}

	if (NULL != m_pGpTextBrush)
	{
		GdipDeleteBrush(m_pGpTextBrush);
		m_pGpTextBrush = NULL;
	}

	if (NULL != m_pGpStringFormat)
	{
		GdipDeleteStringFormat(m_pGpStringFormat);
		m_pGpStringFormat = NULL;
	}

	if (!m_bGpGraphicsSharedAccess)
	{
		GdipDeleteGraphics(m_pGraphics);
		m_pGraphics = NULL;
	}
}

void CXTPMarkupGdiPlusDeviceContext::Init(BOOL bReInit /*= FALSE*/)
{
	if (bReInit)
	{
		HDC hDC = GetDC();
		ReleaseDC(hDC);

		if (m_bGpGraphicsSharedAccess)
			GdipDeleteGraphics(m_pGraphics);

		m_pGraphics = CreateGraphics(hDC, hDC);
	}

	if (NULL != m_pGpStringFormat)
	{
		GdipDeleteStringFormat(m_pGpStringFormat);
		m_pGpStringFormat = NULL;
	}

	SetSmoothingMode(xtpMarkupSmoothingDefault, TRUE);
	ApplySmoothingMode();

	GpStringFormat* pGpGenericTypographicStringFormat = NULL;
	GdipStringFormatGetGenericTypographic(&pGpGenericTypographicStringFormat);
	GdipCloneStringFormat(pGpGenericTypographicStringFormat, &m_pGpStringFormat);
	GdipSetStringFormatFlags(m_pGpStringFormat, StringFormatFlagsNoWrap | StringFormatFlagsNoClip
													| StringFormatFlagsMeasureTrailingSpaces
													| StringFormatFlagsNoFitBlackBox);
}

BOOL CXTPMarkupGdiPlusDeviceContext::IsRTL() const
{
	CXTPMarkupGdiPlusDeviceContext* This = const_cast<CXTPMarkupGdiPlusDeviceContext*>(this);
	HDC hDC								 = This->GetDC();
	BOOL bIsRTL							 = XTPDrawHelpers()->IsContextRTL(hDC);
	This->ReleaseDC(hDC);
	return bIsRTL;
}

GpGraphics* AFX_CDECL CXTPMarkupGdiPlusDeviceContext::CreateGraphics(HDC hDC, HDC hAttribDC)
{
	GpGraphics* pGpGraphics = NULL;

	if (hDC != hAttribDC)
	{
		GdipCreateFromHDC2(hDC, hAttribDC, &pGpGraphics);
	}
	else
	{
		GdipCreateFromHDC(hDC, &pGpGraphics);
	}

	if (NULL != pGpGraphics)
	{
		GdipSetPageUnit(pGpGraphics, UnitPixel);
	}

	return pGpGraphics;
}

GpGraphics* CXTPMarkupGdiPlusDeviceContext::GetGraphics() const
{
	ASSERT("Currently RTL is not supported by GDI+ DC." && !IsRTL());
	return m_pGraphics;
}

void CXTPMarkupGdiPlusDeviceContext::GetClipBox(LPRECT lpRect)
{
	GpRect rect;
	GdipGetClipBoundsI(GetGraphics(), &rect);

	lpRect->left   = rect.GetLeft();
	lpRect->top	   = rect.GetTop();
	lpRect->right  = rect.GetRight();
	lpRect->bottom = rect.GetBottom();
}

HRGN CXTPMarkupGdiPlusDeviceContext::SaveClipRegion()
{
	GpRegion* pGpRegion = NULL;

	GdipCreateRegion(&pGpRegion);
	GdipGetClip(GetGraphics(), pGpRegion);

	return (HRGN)pGpRegion;
}

void CXTPMarkupGdiPlusDeviceContext::RestoreClipRegion(HRGN hrgnClip)
{
	GpRegion* pGpRegion = (GpRegion*)hrgnClip;
	if (pGpRegion != NULL)
	{
		GdipSetClipRegion(GetGraphics(), pGpRegion, CombineModeReplace);
		GdipDeleteRegion(pGpRegion);
	}
}

void CXTPMarkupGdiPlusDeviceContext::IntersectClipRect(const RECT& rcLayoutClip)
{
	GdipSetClipRectI(GetGraphics(), rcLayoutClip.left, rcLayoutClip.top,
					 rcLayoutClip.right - rcLayoutClip.left, rcLayoutClip.bottom - rcLayoutClip.top,
					 CombineModeIntersect);
}

void CXTPMarkupGdiPlusDeviceContext::OffsetViewport(const POINT& ptViewortOrg)
{
	GdipTranslateWorldTransform(GetGraphics(), (REAL)ptViewortOrg.x, (REAL)ptViewortOrg.y,
								MatrixOrderPrepend);

	HDC hDC	 = GetDC();
	POINT pt = TranslatePoint(ptViewortOrg);
	OffsetViewportOrgEx(hDC, pt.x, pt.y, NULL);
	ReleaseDC(hDC);
}

GpFont* CXTPMarkupGdiPlusDeviceContext::AccessFont()
{
	GpFont* pFont = m_pGpFont;
	if (NULL == pFont)
	{
		// Not font set, obtain from current DC.
		HDC hDC;
		if (Gdiplus::Ok == GdipGetDC(GetGraphics(), &hDC))
		{
			HFONT hFont = reinterpret_cast<HFONT>(::GetCurrentObject(hDC, OBJ_FONT));
			if (NULL != hFont)
			{
				LOGFONT lf;
				if (0 < GetObject(hFont, sizeof(LOGFONT), &lf))
				{
#ifdef _UNICODE
					GdipCreateFontFromLogfontW(hDC, &lf, &m_pGpFont);
#else
					GdipCreateFontFromLogfontA(hDC, &lf, &m_pGpFont);
#endif
				}
			}

			if (NULL == m_pGpFont)
			{
				// Failed to obtain from DC, obtain default GUI font.
				hFont = reinterpret_cast<HFONT>(::GetStockObject(DEFAULT_GUI_FONT));

				LOGFONT lf;
				if (0 < GetObject(hFont, sizeof(LOGFONT), &lf))
				{
#ifdef _UNICODE
					GdipCreateFontFromLogfontW(hDC, &lf, &m_pGpFont);
#else
					GdipCreateFontFromLogfontA(hDC, &lf, &m_pGpFont);
#endif
				}
			}

			pFont = m_pGpFont;

			GdipReleaseDC(GetGraphics(), hDC);
		}
	}

	return pFont;
}

const GpFont* CXTPMarkupGdiPlusDeviceContext::AccessFont() const
{
	return const_cast<CXTPMarkupGdiPlusDeviceContext*>(this)->AccessFont();
}

void CXTPMarkupGdiPlusDeviceContext::ApplySmoothingMode()
{
	GdipSetSmoothingMode(GetGraphics(), m_gdipSmoothingMode);
	GdipSetPixelOffsetMode(GetGraphics(), m_defaultPixelOffsetMode);
	GdipSetInterpolationMode(GetGraphics(), m_gdipInterpolationMode);
	GdipSetTextRenderingHint(GetGraphics(), m_gdipTextRenderingHint);
}

XTPMarkupSmoothingMode CXTPMarkupGdiPlusDeviceContext::SetSmoothingMode(
	XTPMarkupSmoothingMode smoothingMode, BOOL bUseTextSmoothing)
{
	BOOL bValid = TRUE;
	switch (smoothingMode)
	{
		case xtpMarkupSmoothingNone:
			m_gdipSmoothingMode		 = SmoothingModeNone;
			m_defaultPixelOffsetMode = PixelOffsetModeNone;
			m_gdipInterpolationMode	 = InterpolationModeLowQuality;
			m_gdipTextRenderingHint	 = TextRenderingHintSystemDefault;
			break;
		case xtpMarkupSmoothingHighSpeed:
			m_gdipSmoothingMode		 = SmoothingModeHighSpeed;
			m_defaultPixelOffsetMode = PixelOffsetModeHighSpeed;
			m_gdipInterpolationMode	 = InterpolationModeLowQuality;
			m_gdipTextRenderingHint	 = TextRenderingHintSystemDefault;
			break;
		case xtpMarkupSmoothingHighQuality:
			m_gdipSmoothingMode		 = SmoothingModeHighQuality;
			m_defaultPixelOffsetMode = PixelOffsetModeHighQuality;
			m_gdipInterpolationMode	 = InterpolationModeHighQuality;
			m_gdipTextRenderingHint	 = bUseTextSmoothing ? TextRenderingHintAntiAlias
														 : TextRenderingHintSystemDefault;
			break;
		case xtpMarkupSmoothingAntiAlias:
			m_gdipSmoothingMode		 = SmoothingModeAntiAlias;
			m_defaultPixelOffsetMode = PixelOffsetModeHighQuality;
			m_gdipInterpolationMode	 = InterpolationModeHighQualityBicubic;
			m_gdipTextRenderingHint	 = bUseTextSmoothing ? TextRenderingHintAntiAlias
														 : TextRenderingHintSystemDefault;
			break;
		case xtpMarkupSmoothingDefault:
			m_gdipSmoothingMode		 = SmoothingModeAntiAlias;
			m_defaultPixelOffsetMode = PixelOffsetModeHighQuality;
			m_gdipInterpolationMode	 = InterpolationModeHighQualityBicubic;
			m_gdipTextRenderingHint	 = TextRenderingHintSystemDefault;
			break;
		default: bValid = FALSE; break;
	}

	XTPMarkupSmoothingMode oldSmoothingMode = m_smoothingMode;
	if (bValid)
	{
		m_smoothingMode = smoothingMode;
	}

	if (oldSmoothingMode != smoothingMode)
	{
		ApplySmoothingMode();
	}
	return oldSmoothingMode;
}

CXTPMarkupTransformationMatrix* CXTPMarkupGdiPlusDeviceContext::GetTransformationMatrix() const
{
	GpMatrix* pMatrix = NULL;
	GdipCreateMatrix(&pMatrix);
	GdipGetWorldTransform(GetGraphics(), pMatrix);
	return new CXTPMarkupGdiPlusTransformationMatrix(NULL, pMatrix);
}

void CXTPMarkupGdiPlusDeviceContext::SetTransformationMatrix(CXTPMarkupTransformationMatrix* pMatrix)
{
	ASSERT(NULL != pMatrix);
	ASSERT_KINDOF(CXTPMarkupGdiPlusTransformationMatrix, pMatrix);

	CXTPMarkupGdiPlusTransformationMatrix* pGdiPlusMatrix =
		static_cast<CXTPMarkupGdiPlusTransformationMatrix*>(pMatrix);
	GdipSetWorldTransform(GetGraphics(), pGdiPlusMatrix->GetMatrix());
}

CXTPMarkupDedicatedDC* CXTPMarkupGdiPlusDeviceContext::GetDedicatedDC(const RECT& rect)
{
	return new DedicatedDC(this, rect);
}

CXTPMarkupDeviceDependentImage* CXTPMarkupGdiPlusDeviceContext ::CreateDeviceDependentImage(
	IXTPMarkupDeviceDependentImageSite* pSite) const
{
	return new CXTPMarkupGdiPlusImage(NULL, pSite);
}

void CXTPMarkupGdiPlusDeviceContext::Ellipse(CRect rc, CXTPMarkupStrokeStyle* pStrokeStyle,
											 CXTPMarkupBrush* pFillBrush)
{
	int nStrokeThickness = pStrokeStyle->nStrokeThickness > 0 ? pStrokeStyle->nStrokeThickness : 0;

	RectF rcBound((REAL)rc.left, (REAL)rc.top, (REAL)rc.Width(), (REAL)rc.Height());
	rcBound.Inflate(-(REAL)((REAL)nStrokeThickness / 2.0), -(REAL)((REAL)nStrokeThickness / 2.0));

	if (pFillBrush)
	{
		GpBrush* pGpBrush = CreateGpBrush(pFillBrush, &rc);
		GdipFillEllipse(GetGraphics(), pGpBrush, rcBound.X, rcBound.Y, rcBound.Width,
						rcBound.Height);
		GdipDeleteBrush(pGpBrush);
	}

	if (nStrokeThickness > 0)
	{
		GpPen* pGpPen = CreateGpPen(pStrokeStyle, rc);
		GdipDrawEllipse(GetGraphics(), pGpPen, rcBound.X, rcBound.Y, rcBound.Width, rcBound.Height);
		GdipDeletePen(pGpPen);
	}
}

void CXTPMarkupGdiPlusDeviceContext::Polyline(const POINT* points, int nCount,
											  CXTPMarkupStrokeStyle* pStrokeStyle)
{
	GpPen* pGpPen = CreateGpPen(pStrokeStyle, GetPointsBoundRect(points, nCount));

	if (pGpPen)
	{
		GdipDrawLinesI(GetGraphics(), pGpPen, (GpPoint*)points, nCount);
		GdipDeletePen(pGpPen);
	}
}

void CXTPMarkupGdiPlusDeviceContext::Polyline(const void* points, int nCount,
											  CXTPMarkupStrokeStyle* pStrokeStyle)
{
	const Gdiplus::GpPointF* pointsF = static_cast<const Gdiplus::GpPointF*>(points);

	GpPen* pGpPen = CreateGpPen(pStrokeStyle, GetPointsBoundRect(pointsF, nCount));
	if (pGpPen)
	{
		GdipDrawLines(GetGraphics(), pGpPen, pointsF, nCount);
		GdipDeletePen(pGpPen);
	}
}

CRect CXTPMarkupGdiPlusDeviceContext::GetPointsBoundRect(const POINT* points, int nCount) const
{
	CRect rc(INT_MAX, INT_MAX, -INT_MAX, -INT_MAX);
	for (int i = 0; i < nCount; i++)
	{
		const POINT& pt = points[i];
		rc.left			= min(rc.left, pt.x);
		rc.top			= min(rc.top, pt.y);
		rc.right		= max(rc.right, pt.x);
		rc.bottom		= max(rc.bottom, pt.y);
	}

	return rc;
}
Gdiplus::GpRectF CXTPMarkupGdiPlusDeviceContext::GetPointsBoundRect(const Gdiplus::GpPointF* points,
																	int nCount) const
{
	Gdiplus::GpRectF rc(FLT_MAX, FLT_MAX, -FLT_MAX, -FLT_MAX);
	for (int i = 0; i < nCount; i++)
	{
		const Gdiplus::GpPointF& pt = points[i];
		rc.X						= min(rc.GetLeft(), pt.X);
		rc.Y						= min(rc.GetTop(), pt.Y);
		rc.Width					= max(rc.GetRight(), pt.X) - rc.X;
		rc.Height					= max(rc.GetBottom(), pt.Y) - rc.Y;
	}

	return rc;
}

void CXTPMarkupGdiPlusDeviceContext::Polygon(const POINT* points, int nCount,
											 CXTPMarkupStrokeStyle* pStrokeStyle,
											 CXTPMarkupBrush* pFillBrush)
{
	CRect rcBound = GetPointsBoundRect(points, nCount);
	if (pFillBrush)
	{
		GpBrush* pGpBrush = CreateGpBrush(pFillBrush, rcBound);

		if (pGpBrush)
		{
			GdipSetPixelOffsetMode(GetGraphics(), m_defaultPixelOffsetMode);
			GdipFillPolygonI(GetGraphics(), pGpBrush, (GpPoint*)points, nCount, FillModeAlternate);
			GdipDeleteBrush(pGpBrush);
		}
	}

	if (NULL != pStrokeStyle && 0 < pStrokeStyle->nStrokeThickness)
	{
		GpPen* pGpPen = CreateGpPen(pStrokeStyle, rcBound);
		if (pGpPen)
		{
			GdipSetPixelOffsetMode(GetGraphics(), (pStrokeStyle->nStrokeThickness % 2)
													  ? PixelOffsetModeDefault
													  : m_defaultPixelOffsetMode);
			GdipDrawPolygonI(GetGraphics(), pGpPen, (GpPoint*)points, nCount);
			GdipDeletePen(pGpPen);
		}
	}
}

void CXTPMarkupGdiPlusDeviceContext::Polygon(const void* points, int nCount,
											 CXTPMarkupStrokeStyle* pStrokeStyle,
											 CXTPMarkupBrush* pFillBrush)
{
	const Gdiplus::GpPointF* pointsF = static_cast<const Gdiplus::GpPointF*>(points);
	Gdiplus::GpRectF rcBound		 = GetPointsBoundRect(pointsF, nCount);
	if (pFillBrush)
	{
		GpBrush* pGpBrush = CreateGpBrush(pFillBrush, rcBound);

		if (pGpBrush)
		{
			GdipSetPixelOffsetMode(GetGraphics(), m_defaultPixelOffsetMode);
			GdipFillPolygon(GetGraphics(), pGpBrush, pointsF, nCount, FillModeAlternate);
			GdipDeleteBrush(pGpBrush);
		}
	}

	if (NULL != pStrokeStyle && 0 < pStrokeStyle->nStrokeThickness)
	{
		GpPen* pGpPen = CreateGpPen(pStrokeStyle, rcBound);
		if (pGpPen)
		{
			GdipSetPixelOffsetMode(GetGraphics(), (pStrokeStyle->nStrokeThickness % 2)
													  ? PixelOffsetModeDefault
													  : m_defaultPixelOffsetMode);
			GdipDrawPolygon(GetGraphics(), pGpPen, pointsF, nCount);
			GdipDeletePen(pGpPen);
		}
	}
}

void CXTPMarkupGdiPlusDeviceContext::DrawPath(CXTPMarkupPathData* pData,
											  CXTPMarkupStrokeStyle* pStrokeStyle,
											  CXTPMarkupBrush* pFillBrush)
{
	if (pData->GetCount() > 0)
	{
		GpPath* pGpPath = NULL;
		GdipCreatePath2((GpPointF*)pData->GetPoints(), pData->GetTypes(), pData->GetCount(),
						FillModeAlternate, &pGpPath);

		if (pGpPath)
		{
			if (pFillBrush)
			{
				GdipSetPixelOffsetMode(GetGraphics(), m_defaultPixelOffsetMode);
				GpBrush* pGpBrush = CreateGpBrush(pFillBrush, pData->GetBounds());
				GdipFillPath(GetGraphics(), pGpBrush, pGpPath);
				GdipDeleteBrush(pGpBrush);
			}

			if (pStrokeStyle && pStrokeStyle->nStrokeThickness > 0)
			{
				if (pData->m_nPixelOffsetMode == 0)
				{
					GdipSetPixelOffsetMode(GetGraphics(), m_defaultPixelOffsetMode);
				}
				else
				{
					GdipSetPixelOffsetMode(GetGraphics(),
										   (PixelOffsetMode)pData->m_nPixelOffsetMode);
				}

				GpPen* pGpPen = CreateGpPen(pStrokeStyle, pData->GetBounds());
				GdipDrawPath(GetGraphics(), pGpPen, pGpPath);
				GdipDeletePen(pGpPen);
			}

			GdipDeletePath(pGpPath);
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// The following function (GdipCreateBitmapFromHICON_Fixed_Native)
// is a fixed version of Microsoft one
// The problem of the original code not saving of transparency
// of HICON (alpha channel). The bug dates to 2001 (!) when GDI+
// was originally introduced. It is widely mentioned on Microsoft websites
// with some workarounds (not working for our case).
// The bug is not fixed and doesn't seem to be planned to be fixed.
// The code is based on wine (www.winehq.org) code
//////////////////////////////////////////////////////////////////////////

Status CXTPMarkupGdiPlusDeviceContext::GdipCreateBitmapFromHICON_Fixed(HICON hicon,
																	   GpBitmap** bitmap) const
{
	GpStatus returnStatus = Gdiplus::Ok;

	GpStatus stat;

	BITMAP bm;
	int width = 0, height = 0;
	Rect rect;
	BitmapData lockeddata;
	BOOL has_alpha;
	int x, y;

	BITMAPINFOHEADER bih;
	DWORD* src	  = NULL;
	BYTE* dst_row = NULL;
	DWORD* dst	  = NULL;

	ICONINFO iinfo;
	ZeroMemory(&iinfo, sizeof(ICONINFO));

	BYTE* pbBitmapBits = NULL;

	HDC hScreenDC = NULL;

	do
	{
		if (!bitmap || !::GetIconInfo(hicon, &iinfo))
		{
			returnStatus = InvalidParameter;
			break;
		}

		// get the size of the icon
		if (!GetObject(iinfo.hbmColor ? iinfo.hbmColor : iinfo.hbmMask, sizeof(bm), &bm))
		{
			returnStatus = GenericError;
			break;
		}

		width = bm.bmWidth;

		if (iinfo.hbmColor)
			height = abs(bm.bmHeight);
		else // combined bitmap + mask
			height = abs(bm.bmHeight) / 2;

		pbBitmapBits = (BYTE*)::HeapAlloc(::GetProcessHeap(), 0,
										  XTPToSizeTChecked(4 * width * height));

		if (!pbBitmapBits)
		{
			returnStatus = OutOfMemory;
			break;
		}

		stat = GdipCreateBitmapFromScan0(width, height, 0, PixelFormat32bppPARGB, NULL, bitmap);

		rect.X		= 0;
		rect.Y		= 0;
		rect.Width	= width;
		rect.Height = height;

		stat = GdipBitmapLockBits(*bitmap, &rect, ImageLockModeWrite, PixelFormat32bppPARGB,
								  &lockeddata);

		bih.biSize			= sizeof(bih);
		bih.biWidth			= width;
		bih.biHeight		= -height;
		bih.biPlanes		= 1;
		bih.biBitCount		= 32;
		bih.biCompression	= BI_RGB;
		bih.biSizeImage		= 0;
		bih.biXPelsPerMeter = 0;
		bih.biYPelsPerMeter = 0;
		bih.biClrUsed		= 0;
		bih.biClrImportant	= 0;

		hScreenDC = ::GetDC(NULL);
		if (iinfo.hbmColor)
		{
			::GetDIBits(hScreenDC, iinfo.hbmColor, 0, XTPToUIntChecked(height), pbBitmapBits,
						(BITMAPINFO*)&bih, DIB_RGB_COLORS);

			if (bm.bmBitsPixel == 32)
			{
				has_alpha = FALSE;

				// If any pixel has a non-zero alpha, ignore hbmMask
				src = (DWORD*)pbBitmapBits;
				for (x = 0; x < width && !has_alpha; x++)
					for (y = 0; y < height && !has_alpha; y++)
						if ((*src++ & 0xff000000) != 0)
							has_alpha = TRUE;
			}
			else
				has_alpha = FALSE;
		}
		else
		{
			::GetDIBits(hScreenDC, iinfo.hbmMask, 0, XTPToUIntChecked(height), pbBitmapBits,
						(BITMAPINFO*)&bih, DIB_RGB_COLORS);

			has_alpha = FALSE;
		}

		// copy the image data to the Bitmap
		src		= (DWORD*)pbBitmapBits;
		dst_row = (BYTE*)lockeddata.Scan0;
		for (y = 0; y < height; y++)
		{
			memcpy(dst_row, src, XTPToSizeTChecked(width * 4));
			src += width;
			dst_row += lockeddata.Stride;
		}

		if (!has_alpha)
		{
			if (iinfo.hbmMask)
			{
				// read alpha data from the mask
				if (iinfo.hbmColor)
					::GetDIBits(hScreenDC, iinfo.hbmMask, 0, XTPToUIntChecked(height), pbBitmapBits,
								(BITMAPINFO*)&bih, DIB_RGB_COLORS);
				else
					::GetDIBits(hScreenDC, iinfo.hbmMask, XTPToUIntChecked(height),
								XTPToUIntChecked(height), pbBitmapBits, (BITMAPINFO*)&bih,
								DIB_RGB_COLORS);

				src		= (DWORD*)pbBitmapBits;
				dst_row = (BYTE*)lockeddata.Scan0;
				for (y = 0; y < height; y++)
				{
					dst = (DWORD*)dst_row;
					for (x = 0; x < width; x++)
					{
						DWORD src_value = *src++;
						if (src_value)
							*dst++ = 0;
						else
							*dst++ |= 0xff000000;
					}
					dst_row += lockeddata.Stride;
				}
			}
			else
			{
				// set constant alpha of 255
				dst_row = pbBitmapBits;
				for (y = 0; y < height; y++)
				{
					dst = (DWORD*)dst_row;
					for (x = 0; x < width; x++)
						*dst++ |= 0xff000000;
					dst_row += lockeddata.Stride;
				}
			}
		}
	} while (false);

	if (hScreenDC)
		::ReleaseDC(0, hScreenDC);

	if (iinfo.hbmColor)
		::DeleteObject(iinfo.hbmColor);

	if (iinfo.hbmColor)
		::DeleteObject(iinfo.hbmMask);

	if (bitmap && *bitmap)
		stat = GdipBitmapUnlockBits(*bitmap, &lockeddata);

	if (pbBitmapBits)
		::HeapFree(GetProcessHeap(), 0, pbBitmapBits);

	return returnStatus;
}

GpBitmap* CXTPMarkupGdiPlusDeviceContext::IconToGdiplusBitmap(CXTPImageManagerIcon* pIcon,
															  CSize& szImage) const
{
	ASSERT(pIcon->IsRasterIcon());

	GpBitmap* pGpBitmap = NULL;

	// Copying icon handle is required, because initial CXTPImageManagerIconHandle
	// ICON/Bitmap members are always NULL, after copying it's OK
	CXTPImageManagerIconHandle iconHandle;
	iconHandle.CopyHandle(pIcon->GetIcon());

	if (NULL != pIcon->GetImageList())
	{
		HICON hIcon = pIcon->GetImageList()->GetIconHandle(pIcon->GetID());
		if (NULL != hIcon)
		{
			iconHandle.Attach(hIcon, TRUE);
		}
		else
		{
			HBITMAP hBmp = pIcon->GetImageList()->GetBitmapHandle(pIcon->GetID());
			if (NULL != hBmp)
			{
				iconHandle.Attach(hBmp, TRUE);
			}
		}
	}

	if (iconHandle.IsEmpty())
	{
		return NULL;
	}

	szImage = iconHandle.GetExtent();

	if (iconHandle.IsAlpha())
	{
		BITMAP bmpInfo;
		::GetObject(iconHandle.GetBitmap(), sizeof(BITMAP), &bmpInfo);
		int cxBitmap = bmpInfo.bmWidth;
		int cyBitmap = bmpInfo.bmHeight;

		if (Ok
			!= GdipCreateBitmapFromScan0(cxBitmap, cyBitmap, cxBitmap * 4, PixelFormat32bppPARGB,
										 (BYTE*)bmpInfo.bmBits, &pGpBitmap))
		{
			return NULL;
		}

		GdipImageRotateFlip(pGpBitmap, Rotate180FlipX);
		return pGpBitmap;
	}

	if (iconHandle.GetIcon())
	{
		GdipCreateBitmapFromHICON_Fixed(iconHandle.GetIcon(), &pGpBitmap);
		return pGpBitmap;
	}

	if (iconHandle.GetBitmap())
	{
		GdipCreateBitmapFromHBITMAP(iconHandle.GetBitmap(), 0, &pGpBitmap);
		return pGpBitmap;
	}

	return NULL;
}

void CXTPMarkupGdiPlusDeviceContext::DrawGpImage(Gdiplus::GpImage* pImage, CRect rc, CSize size,
												 CXTPMarkupContext* pContext /*= NULL*/) const
{
	ASSERT(NULL != pImage);

	GpImageAttributes* pAttr = NULL;
	GdipCreateImageAttributes(&pAttr);
	if (NULL != pAttr)
	{
		// For non-trasparent images a color key may be provided my
		// context's image manager. If none, the default (absolute green) is used.
		if (NULL != pContext)
		{
			COLORREF crColorKey = pContext->GetImageColorKey();
			if (COLORREF_NULL != crColorKey)
			{
				UINT flags = 0;
				if (Gdiplus::Ok == GdipGetImageFlags(pImage, &flags)
					&& (0 == (flags & (ImageFlagsHasAlpha | ImageFlagsHasTranslucent))))
				{
					ARGB crArgb = Color::MakeARGB(0, GetRValue(crColorKey), GetGValue(crColorKey),
												  GetBValue(crColorKey));
					GdipSetImageAttributesColorKeys(pAttr, ColorAdjustTypeBitmap, TRUE, crArgb,
													crArgb);
				}
			}
		}

		// Apply opacity
		// clang-format off
		Gdiplus::ColorMatrix colorMatrix =
		{{
			{ 1, 0, 0, 0, 0 },
			{ 0, 1, 0, 0, 0 },
			{ 0, 0, 1, 0, 0 },
			{ 0, 0, 0, static_cast<Gdiplus::REAL>(GetOpacity()), 0 },
			{ 0, 0, 0, 0, 1 }
		}};
		// clang-format on
		GdipSetImageAttributesColorMatrix(pAttr, ColorAdjustTypeBitmap, TRUE, &colorMatrix, NULL,
										  ColorMatrixFlagsDefault);
	}

	// Draw image
	GdipDrawImageRectRect(GetGraphics(), pImage, static_cast<REAL>(rc.left),
						  static_cast<REAL>(rc.top), static_cast<REAL>(rc.Width()),
						  static_cast<REAL>(rc.Height()), 0, 0, static_cast<REAL>(size.cx),
						  static_cast<REAL>(size.cy), UnitPixel, pAttr, NULL, NULL);

	if (NULL != pAttr)
	{
		GdipDisposeImageAttributes(pAttr);
	}
}

void CXTPMarkupGdiPlusDeviceContext::DrawImage(CXTPImageManagerIcon* pIcon, CRect rc)
{
	ASSERT_VALID(pIcon);

	// GDI-plus drawing must draw in its own context instead of calling
	// CXTPMarkupGdiPlusDeviceContext::GetDC because the DC returned by GdipGetDC() will
	// not respect the transformation applied to the GDI-plus context The solution is too convert
	// CXTPImageManagerIcon to GpBitmap FYI: CXTPMarkupDeviceContext::DrawImage is using a simple
	// drawing

	GpBitmap* pGpImage = NULL;
	CSize size;

	if (!pIcon->IsVectorIcon())
	{
		pGpImage = IconToGdiplusBitmap(pIcon, size);
	}

	if (NULL == pGpImage)
	{
		// If the conversion failed or impossible, we need to provide the old drawing code.
		HDC hDC = GetDC();
		pIcon->Draw(CDC::FromHandle(hDC), rc.TopLeft(), rc.Size());
		ReleaseDC(hDC);
		return;
	}

	DrawGpImage(pGpImage, rc, size);
	GdipDisposeImage(pGpImage);
}

void CXTPMarkupGdiPlusDeviceContext::DrawImage(CXTPMarkupDeviceDependentImage* pImage, CRect rc)
{
	ASSERT(NULL != pImage);
	ASSERT_KINDOF(CXTPMarkupGdiPlusImage, pImage);

	CXTPMarkupGdiPlusImage* pGdiPlusImage = static_cast<CXTPMarkupGdiPlusImage*>(pImage);
	GpImage* pGpImage					  = pGdiPlusImage->GetGpImage();
	if (NULL != pGpImage)
	{
		DrawGpImage(pGpImage, rc, pGdiPlusImage->GetSize(), pImage->GetSite()->GetMarkupContext());
	}
	else
	{
		CXTPImageManagerIcon* pIcon = pGdiPlusImage->GetIcon();
		if (NULL != pIcon)
		{
			DrawImage(pIcon, rc);
		}
	}
}

BOOL CXTPMarkupGdiPlusDeviceContext::GetFontMetrics(CXTPMarkupFont* pFont,
													XTPMarkupFontMetrics* pMetrics)
{
	BOOL bSuccess = FALSE;

	ASSERT(NULL != pFont);
	ASSERT(NULL != pMetrics);
	if (NULL != pFont && NULL != pMetrics)
	{
		LOGFONT lf;
		if (0 < GetObject(pFont->m_Font, sizeof(LOGFONT), &lf))
		{
			Gdiplus::GpFont* pGpFont = NULL;
			HDC hDC					 = GetDC();
#ifdef _UNICODE
			GdipCreateFontFromLogfontW(hDC, &lf, &pGpFont);
#else
			GdipCreateFontFromLogfontA(hDC, &lf, &pGpFont);
#endif
			ReleaseDC(hDC);

			if (NULL != pGpFont)
			{
				Gdiplus::GpFontFamily* pGpFontFamily = NULL;
				GdipGetFamily(pGpFont, &pGpFontFamily);
				if (NULL != pGpFontFamily)
				{
					do
					{
						int nFontStyle = 0;
						if (Gdiplus::Ok != GdipGetFontStyle(pGpFont, &nFontStyle))
							break;

						UINT16 nDescent = 0;
						if (Gdiplus::Ok != GdipGetCellDescent(pGpFontFamily, nFontStyle, &nDescent))
							break;

						UINT16 nAscent = 0;
						if (Gdiplus::Ok != GdipGetCellAscent(pGpFontFamily, nFontStyle, &nAscent))
							break;

						UINT16 nEmHeight = 0;
						if (Gdiplus::Ok != GdipGetEmHeight(pGpFontFamily, nFontStyle, &nEmHeight))
							break;

						REAL rFontSize = 0;
						if (Gdiplus::Ok != GdipGetFontSize(pGpFont, &rFontSize))
							break;

						REAL rDesignToPixels = rFontSize / static_cast<REAL>(nEmHeight);

						pMetrics->nAscent  = static_cast<int>(rDesignToPixels
															  * static_cast<REAL>(nAscent));
						pMetrics->nDescent = static_cast<int>(rDesignToPixels
															  * static_cast<REAL>(nDescent));

						bSuccess = TRUE;
					} while (false);

					GdipDeleteFontFamily(pGpFontFamily);
				}

				GdipDeleteFont(pGpFont);
			}
			else
			{
				// Fallback to GDI font handling it it could be a non-TrueType font,
				// so it's better to extract metrics that are close to actual than nothing.
				hDC				 = GetDC();
				HGDIOBJ hOldFont = ::SelectObject(hDC, pFont->m_Font);

				TEXTMETRIC tm;
				if (::GetTextMetrics(hDC, &tm))
				{
					pMetrics->nAscent  = tm.tmAscent;
					pMetrics->nDescent = tm.tmDescent;
					bSuccess		   = TRUE;
				}

				::SelectObject(hDC, hOldFont);
				ReleaseDC(hDC);
			}
		}
	}

	return bSuccess;
}

void CXTPMarkupGdiPlusDeviceContext::SetTextFont(CXTPMarkupFont* pFont)
{
	if (m_pGpFont)
	{
		GdipDeleteFont(m_pGpFont);
		m_pGpFont = NULL;
	}

	if (pFont)
	{
		HDC hDC = GetDC();

		LOGFONT lf;
		pFont->m_Font.GetLogFont(&lf);
#ifdef _UNICODE
		GdipCreateFontFromLogfontW(hDC, &lf, &m_pGpFont);
#else
		GdipCreateFontFromLogfontA(hDC, &lf, &m_pGpFont);
#endif

		ReleaseDC(hDC);
	}
}

void CXTPMarkupGdiPlusDeviceContext::SetTextColor(CXTPMarkupBrush* pBrush)
{
	if (m_pGpTextBrush)
	{
		GdipDeleteBrush(m_pGpTextBrush);
		m_pGpTextBrush = NULL;
	}

	m_pGpTextBrush = CreateGpBrush(pBrush, CRect(0, 0, 1, 1));
}

SIZE CXTPMarkupGdiPlusDeviceContext::MeasureString(LPCWSTR lpszText, int nCount) const
{
	SIZE sz = { 0 };

	RectF rc;
	RectF layoutRect(0, 0, 0.0f, 0.0f);

	if (Gdiplus::Ok
		== GdipMeasureString(GetGraphics(), lpszText, nCount, AccessFont(), &layoutRect,
							 m_pGpStringFormat, &rc, 0, 0))
	{
		sz.cx = static_cast<int>(CXTPMathUtils::Round(rc.Width));
		sz.cy = static_cast<int>(CXTPMathUtils::Round(rc.Height));
	}

	return sz;
}

void CXTPMarkupGdiPlusDeviceContext::MeasureString(LPCWSTR lpszText, int nCount,
												   CXTPSizeF& size) const
{
	CXTPSizeF sz;

	RectF rc;
	RectF layoutRect(0, 0, 0.0f, 0.0f);

	if (Gdiplus::Ok
		== GdipMeasureString(GetGraphics(), lpszText, nCount, AccessFont(), &layoutRect,
							 m_pGpStringFormat, &rc, 0, 0))
	{
		sz.Width  = rc.Width;
		sz.Height = rc.Height;
	}

	size = sz;
}

void CXTPMarkupGdiPlusDeviceContext::DrawString(LPCWSTR lpszString, UINT nCount, LPCRECT lpRect)
{
	ASSERT(NULL != lpszString);
	ASSERT(NULL != lpRect);

	// GDI+ often stretches text and fills the traling space
	// so in order to avoid it the trailing space should not be rendered.
	int nSpace = 0;
	if (0 < nCount)
	{
		nSpace = (iswspace(lpszString[nCount - 1]) ? 1 : 0);
	}

	if (lpRect->left < lpRect->right && lpRect->top < lpRect->bottom)
	{
		RectF rc(static_cast<REAL>(lpRect->left), static_cast<REAL>(lpRect->top),
				 static_cast<REAL>(lpRect->right - lpRect->left),
				 static_cast<REAL>(lpRect->bottom - lpRect->top));
		GdipDrawString(GetGraphics(), lpszString, XTPToIntChecked(nCount - nSpace), AccessFont(),
					   &rc, m_pGpStringFormat, m_pGpTextBrush);
	}
}

HDC CXTPMarkupGdiPlusDeviceContext::GetDC()
{
	HDC hDC = NULL;
	GdipGetDC(m_pGraphics, &hDC);
	return hDC;
}

void CXTPMarkupGdiPlusDeviceContext::ReleaseDC(HDC hDC)
{
	GdipReleaseDC(m_pGraphics, hDC);
}

void CXTPMarkupGdiPlusDeviceContext::DrawLine(int x1, int y1, int x2, int y2,
											  CXTPMarkupStrokeStyle* pStrokeStyle)
{
	GpPen* pGpPen = CreateGpPen(pStrokeStyle, CRect(x1, y1, x2, y2));
	GdipDrawLineI(GetGraphics(), pGpPen, x1, y1, x2, y2);
	GdipDeletePen(pGpPen);
}

GpBrush* CXTPMarkupGdiPlusDeviceContext::CreateGpBrush(CXTPMarkupBrush* pBrush,
													   LPCRECT lpRect) const
{
	if (IsSolidBrush(pBrush))
	{
		CXTPMarkupSolidColorBrush* pSolidColorBrush = (CXTPMarkupSolidColorBrush*)pBrush;
		CXTPMarkupColor* pColor						= pSolidColorBrush->GetColor();

		if (!pColor)
		{
			return NULL;
		}

		GpSolidFill* pGpBrush = NULL;
		GdipCreateSolidFill(SetARGBOpacity(pColor->GetARGB()), &pGpBrush);
		return pGpBrush;
	}

	if (IsLinearGradientBrush(pBrush))
	{
		CXTPMarkupLinearGradientBrush* pLinearGradientBrush = (CXTPMarkupLinearGradientBrush*)pBrush;

		PointF ptStartPoint(0, 0);
		CXTPMarkupPoint* pt = MARKUP_STATICCAST(
			CXTPMarkupPoint,
			pBrush->GetValue(CXTPMarkupLinearGradientBrush::m_pStartPointProperty));
		if (pt)
			ptStartPoint = PointF((REAL)pt->x, (REAL)pt->y);

		PointF ptEndPoint(1, 1);
		pt = MARKUP_STATICCAST(CXTPMarkupPoint,
							   pBrush->GetValue(
								   CXTPMarkupLinearGradientBrush::m_pEndPointProperty));
		if (pt)
			ptEndPoint = PointF((REAL)pt->x, (REAL)pt->y);

		CXTPMarkupGradientStops* pGradientStops = pLinearGradientBrush->GetGradientStops();
		int nCount								= pGradientStops->GetCount();
		if (nCount == 0)
		{
			return NULL;
		}

		GRADIENTSTOP* pStops = new GRADIENTSTOP[XTPToUIntChecked(nCount)];
		int i;

		for (i = 0; i < nCount; i++)
		{
			CXTPMarkupGradientStop* pItem = pGradientStops->GetItem(i);

			COLORREF clr			  = SetCOLORREFOpacity(pItem->GetColor());
			pStops[i].clr.rgbRed	  = GetRValue(clr);
			pStops[i].clr.rgbGreen	  = GetGValue(clr);
			pStops[i].clr.rgbBlue	  = GetBValue(clr);
			pStops[i].clr.rgbReserved = static_cast<BYTE>(clr >> 24);
			pStops[i].dOffset		  = pItem->GetOffset();
			pStops[i].nIndex		  = i;
		}

		qsort(pStops, XTPToSizeTChecked(nCount), sizeof(GRADIENTSTOP), _GradientStopCompare);

		GpLineGradient* pGpBrush = NULL;

		PointF ptStart(static_cast<REAL>(lpRect->left)
						   + ptStartPoint.X * static_cast<REAL>(lpRect->right - lpRect->left),
					   static_cast<REAL>(lpRect->top)
						   + ptStartPoint.Y * static_cast<REAL>(lpRect->bottom - lpRect->top));
		PointF ptEnd(static_cast<REAL>(lpRect->left)
						 + ptEndPoint.X * static_cast<REAL>(lpRect->right - lpRect->left),
					 static_cast<REAL>(lpRect->top)
						 + ptEndPoint.Y * static_cast<REAL>(lpRect->bottom - lpRect->top));

		GdipCreateLineBrush(&ptStart, &ptEnd, *(ARGB*)&pStops[0].clr,
							*(ARGB*)&pStops[nCount - 1].clr, WrapModeTileFlipXY, &pGpBrush);

		ARGB* pBlend	 = new ARGB[XTPToUIntChecked(nCount + 2)];
		REAL* pPositions = new REAL[XTPToUIntChecked(nCount + 2)];
		for (i = 0; i < nCount; i++)
		{
			pBlend[i + 1]	  = *(ARGB*)&pStops[i].clr;
			pPositions[i + 1] = (REAL)pStops[i].dOffset;
		}
		pBlend[0]	  = *(ARGB*)&pStops[0].clr;
		pPositions[0] = 0;

		pBlend[nCount + 1]	   = *(ARGB*)&pStops[nCount - 1].clr;
		pPositions[nCount + 1] = 1;

		GdipSetLinePresetBlend(pGpBrush, pBlend, pPositions, nCount + 2);

		delete[] pPositions;
		delete[] pBlend;
		delete[] pStops;

		return pGpBrush;
	}

	return NULL;
}

GpBrush* CXTPMarkupGdiPlusDeviceContext::CreateGpBrush(CXTPMarkupBrush* pBrush,
													   const Gdiplus::GpRectF& lpRect) const
{
	if (IsSolidBrush(pBrush))
	{
		CXTPMarkupSolidColorBrush* pSolidColorBrush = (CXTPMarkupSolidColorBrush*)pBrush;
		CXTPMarkupColor* pColor						= pSolidColorBrush->GetColor();

		if (!pColor)
		{
			return NULL;
		}

		GpSolidFill* pGpBrush = NULL;
		GdipCreateSolidFill(SetARGBOpacity(pColor->GetARGB()), &pGpBrush);
		return pGpBrush;
	}

	if (IsLinearGradientBrush(pBrush))
	{
		CXTPMarkupLinearGradientBrush* pLinearGradientBrush = (CXTPMarkupLinearGradientBrush*)pBrush;

		PointF ptStartPoint(0, 0);
		CXTPMarkupPoint* pt = MARKUP_STATICCAST(
			CXTPMarkupPoint,
			pBrush->GetValue(CXTPMarkupLinearGradientBrush::m_pStartPointProperty));
		if (pt)
			ptStartPoint = PointF((REAL)pt->x, (REAL)pt->y);

		PointF ptEndPoint(1, 1);
		pt = MARKUP_STATICCAST(CXTPMarkupPoint,
							   pBrush->GetValue(
								   CXTPMarkupLinearGradientBrush::m_pEndPointProperty));
		if (pt)
			ptEndPoint = PointF((REAL)pt->x, (REAL)pt->y);

		CXTPMarkupGradientStops* pGradientStops = pLinearGradientBrush->GetGradientStops();
		int nCount								= pGradientStops->GetCount();
		if (nCount == 0)
		{
			return NULL;
		}

		GRADIENTSTOP* pStops = new GRADIENTSTOP[XTPToUIntChecked(nCount)];
		int i;

		for (i = 0; i < nCount; i++)
		{
			CXTPMarkupGradientStop* pItem = pGradientStops->GetItem(i);

			COLORREF clr			  = SetCOLORREFOpacity(pItem->GetColor());
			pStops[i].clr.rgbRed	  = GetRValue(clr);
			pStops[i].clr.rgbGreen	  = GetGValue(clr);
			pStops[i].clr.rgbBlue	  = GetBValue(clr);
			pStops[i].clr.rgbReserved = static_cast<BYTE>(clr >> 24);
			pStops[i].dOffset		  = pItem->GetOffset();
			pStops[i].nIndex		  = i;
		}

		qsort(pStops, XTPToSizeTChecked(nCount), sizeof(GRADIENTSTOP), _GradientStopCompare);

		GpLineGradient* pGpBrush = NULL;

		PointF ptStart(lpRect.GetLeft() + ptStartPoint.X * (lpRect.GetRight() - lpRect.GetLeft()),
					   lpRect.GetTop() + ptStartPoint.Y * (lpRect.GetBottom() - lpRect.GetTop()));
		PointF ptEnd(lpRect.GetLeft() + ptEndPoint.X * (lpRect.GetRight() - lpRect.GetLeft()),
					 lpRect.GetTop() + ptEndPoint.Y * (lpRect.GetBottom() - lpRect.GetTop()));

		GdipCreateLineBrush(&ptStart, &ptEnd, *(ARGB*)&pStops[0].clr,
							*(ARGB*)&pStops[nCount - 1].clr, WrapModeTileFlipXY, &pGpBrush);

		ARGB* pBlend	 = new ARGB[XTPToUIntChecked(nCount + 2)];
		REAL* pPositions = new REAL[XTPToUIntChecked(nCount + 2)];
		for (i = 0; i < nCount; i++)
		{
			pBlend[i + 1]	  = *(ARGB*)&pStops[i].clr;
			pPositions[i + 1] = (REAL)pStops[i].dOffset;
		}
		pBlend[0]	  = *(ARGB*)&pStops[0].clr;
		pPositions[0] = 0;

		pBlend[nCount + 1]	   = *(ARGB*)&pStops[nCount - 1].clr;
		pPositions[nCount + 1] = 1;

		GdipSetLinePresetBlend(pGpBrush, pBlend, pPositions, nCount + 2);

		delete[] pPositions;
		delete[] pBlend;
		delete[] pStops;

		return pGpBrush;
	}

	return NULL;
}

GpPen* CXTPMarkupGdiPlusDeviceContext::CreateGpPen(CXTPMarkupStrokeStyle* pStrokeStyle,
												   LPCRECT lpRect) const
{
	if (!pStrokeStyle || !pStrokeStyle->pStrokeBrush)
	{
		return NULL;
	}

	CXTPMarkupBrush* pBrush = pStrokeStyle->pStrokeBrush;

	GpPen* pGpPen = 0;

	if (IsSolidBrush(pBrush))
	{
		CXTPMarkupSolidColorBrush* pSolidColorBrush = (CXTPMarkupSolidColorBrush*)pBrush;
		CXTPMarkupColor* pColor						= pSolidColorBrush->GetColor();

		if (!pColor)
		{
			return NULL;
		}

		GdipCreatePen1(SetARGBOpacity(pColor->GetARGB()), (REAL)pStrokeStyle->nStrokeThickness,
					   UnitWorld, &pGpPen);
	}
	else
	{
		GpBrush* pGpBrush = CreateGpBrush(pBrush, lpRect);

		if (pGpBrush)
		{
			GdipCreatePen2(pGpBrush, (REAL)pStrokeStyle->nStrokeThickness, UnitWorld, &pGpPen);
			GdipDeleteBrush(pGpBrush);
		}
	}

	if (!pGpPen)
	{
		return NULL;
	}

	if (pStrokeStyle->pStrokeDashArray)
	{
		if (pStrokeStyle->pStrokeDashArray->GetCount() == 1)
		{
			REAL dist		  = pStrokeStyle->pStrokeDashArray->GetAt(0);
			REAL rDashStyle[] = { dist, dist };
			GdipSetPenDashArray(pGpPen, rDashStyle, 2);
		}
		else
		{
			GdipSetPenDashArray(pGpPen, pStrokeStyle->pStrokeDashArray->GetData(),
								pStrokeStyle->pStrokeDashArray->GetCount());
		}
	}

	if (pStrokeStyle->nStrokeLineJoin != xtpMarkupLineJoinMiter)
	{
		GdipSetPenLineJoin(pGpPen, (LineJoin)pStrokeStyle->nStrokeLineJoin);
	}

	if (pStrokeStyle->nStrokeStartLineCap != xtpMarkupLineCapFlat)
	{
		GdipSetPenStartCap(pGpPen, (LineCap)pStrokeStyle->nStrokeStartLineCap);
	}

	if (pStrokeStyle->nStrokeEndLineCap != xtpMarkupLineCapFlat)
	{
		GdipSetPenEndCap(pGpPen, (LineCap)pStrokeStyle->nStrokeEndLineCap);
	}

	return pGpPen;
}

GpPen* CXTPMarkupGdiPlusDeviceContext::CreateGpPen(CXTPMarkupStrokeStyle* pStrokeStyle,
												   const Gdiplus::GpRectF& lpRect) const
{
	if (!pStrokeStyle || !pStrokeStyle->pStrokeBrush)
	{
		return NULL;
	}

	CXTPMarkupBrush* pBrush = pStrokeStyle->pStrokeBrush;

	GpPen* pGpPen = 0;

	if (IsSolidBrush(pBrush))
	{
		CXTPMarkupSolidColorBrush* pSolidColorBrush = (CXTPMarkupSolidColorBrush*)pBrush;
		CXTPMarkupColor* pColor						= pSolidColorBrush->GetColor();

		if (!pColor)
		{
			return NULL;
		}

		GdipCreatePen1(SetARGBOpacity(pColor->GetARGB()), (REAL)pStrokeStyle->nStrokeThickness,
					   UnitWorld, &pGpPen);
	}
	else
	{
		GpBrush* pGpBrush = CreateGpBrush(pBrush, lpRect);

		if (pGpBrush)
		{
			GdipCreatePen2(pGpBrush, (REAL)pStrokeStyle->nStrokeThickness, UnitWorld, &pGpPen);
			GdipDeleteBrush(pGpBrush);
		}
	}

	if (!pGpPen)
	{
		return NULL;
	}

	if (pStrokeStyle->pStrokeDashArray)
	{
		if (pStrokeStyle->pStrokeDashArray->GetCount() == 1)
		{
			REAL dist		  = pStrokeStyle->pStrokeDashArray->GetAt(0);
			REAL rDashStyle[] = { dist, dist };
			GdipSetPenDashArray(pGpPen, rDashStyle, 2);
		}
		else
		{
			GdipSetPenDashArray(pGpPen, pStrokeStyle->pStrokeDashArray->GetData(),
								pStrokeStyle->pStrokeDashArray->GetCount());
		}
	}

	if (pStrokeStyle->nStrokeLineJoin != xtpMarkupLineJoinMiter)
	{
		GdipSetPenLineJoin(pGpPen, (LineJoin)pStrokeStyle->nStrokeLineJoin);
	}

	if (pStrokeStyle->nStrokeStartLineCap != xtpMarkupLineCapFlat)
	{
		GdipSetPenStartCap(pGpPen, (LineCap)pStrokeStyle->nStrokeStartLineCap);
	}

	if (pStrokeStyle->nStrokeEndLineCap != xtpMarkupLineCapFlat)
	{
		GdipSetPenEndCap(pGpPen, (LineCap)pStrokeStyle->nStrokeEndLineCap);
	}

	return pGpPen;
}

void CXTPMarkupGdiPlusDeviceContext::FillRectangle(CRect rc, CXTPMarkupBrush* pBrush)
{
	if (m_hDC && IsGDIBrush(pBrush))
	{
		CXTPMarkupDeviceContext::FillRectangle(rc, pBrush);
		return;
	}

	GpBrush* pGpBrush = CreateGpBrush(pBrush, rc);
	if (pGpBrush)
	{
		GdipFillRectangleI(GetGraphics(), pGpBrush, rc.left, rc.top, rc.Width(), rc.Height());
		GdipDeleteBrush(pGpBrush);
	}
}

BOOL CXTPMarkupGdiPlusDeviceContext::IsGDIBrush(CXTPMarkupBrush* pBrush) const
{
	if (!pBrush)
		return TRUE;

	CXTPMarkupSolidColorBrush* pSolidColorBrush = MARKUP_DYNAMICCAST(CXTPMarkupSolidColorBrush,
																	 pBrush);
	if (!pSolidColorBrush)
		return FALSE;

	CXTPMarkupColor* pColor = pSolidColorBrush->GetColor();
	if (!pColor)
		return TRUE;

	return ((*pColor) & 0xFF000000) == 0xFF000000;
}

void CXTPMarkupGdiPlusDeviceContext::DrawRectangle(CRect rc, CXTPMarkupBrush* pBrush,
												   CXTPMarkupThickness* pThickness)
{
	if (!pThickness)
		return;

	if (m_hDC && IsGDIBrush(pBrush))
	{
		CXTPMarkupDeviceContext::DrawRectangle(rc, pBrush, pThickness);
		return;
	}

	GpBrush* pGpBrush = CreateGpBrush(pBrush, rc);

	if (pThickness->GetLeft() > 0)
		GdipFillRectangleI(GetGraphics(), pGpBrush, rc.left, rc.top, pThickness->GetLeft(),
						   rc.Height());

	if (pThickness->GetRight() > 0)
		GdipFillRectangleI(GetGraphics(), pGpBrush, rc.right - pThickness->GetRight(), rc.top,
						   pThickness->GetRight(), rc.Height());

	if (pThickness->GetTop() > 0)
		GdipFillRectangleI(GetGraphics(), pGpBrush, rc.left + pThickness->GetLeft(), rc.top,
						   rc.Width() - pThickness->GetLeft() - pThickness->GetRight(),
						   pThickness->GetTop());

	if (pThickness->GetBottom() > 0)
		GdipFillRectangleI(GetGraphics(), pGpBrush, rc.left + pThickness->GetLeft(),
						   rc.bottom - pThickness->GetBottom(),
						   rc.Width() - pThickness->GetLeft() - pThickness->GetRight(),
						   pThickness->GetBottom());

	GdipDeleteBrush(pGpBrush);
}
