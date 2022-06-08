// XTPScrollBarThemeFlat.cpp
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

#include "Common/XTPTypeId.h"
#include "Common/XTPFramework.h"
#include "Common/XTPSystemHelpers.h"
#include "Common/XTPSynchro.h"
#include "Common/XTPApplication.h"
#include "Common/XTPSingleton.h"
#include "Common/XTPColorManager.h"
#include "Common/ScrollBar/XTPScrollBase.h"
#include "Common/ScrollBar/XTPScrollBarPaintManager.h"
#include "Common/ScrollBar/Themes/XTPScrollBarThemeWindowsClassic.h"
#include "Common/ScrollBar/Themes/XTPScrollBarThemeWindowsUx.h"
#include "Common/ScrollBar/Themes/XTPScrollBarThemeFlat.h"

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

#ifdef _DEBUG
#	define new DEBUG_NEW
#	undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void CXTPScrollBarThemeFlat::RefreshMetrics()
{
	CXTPScrollBarThemeWindowsUx::RefreshMetrics();
	m_crBorderHilite = GetXtremeColor(COLOR_3DHIGHLIGHT);
	m_crBorder		 = GetXtremeColor(COLOR_3DSHADOW);
}

void CXTPScrollBarThemeFlat::DrawButtons(CDC* pDC, CXTPScrollBase* pScroll,
										 XTPScrollMetricsV* pMetrics)
{
	ASSERT(pDC);
	ASSERT(NULL != pScroll);
	ASSERT(NULL != pMetrics);

	XTP_SCROLLBAR_TRACKINFO* pSBTrack = pScroll->GetScrollBarTrackInfo();
	XTP_SCROLLBAR_POSINFO* pSBInfo	  = pScroll->GetScrollBarPosInfo();
	int nHtPressed					  = (NULL != pSBTrack) ? pSBInfo->ht : -1;

	pDC->FillSolidRect(pMetrics->rcArrowUp, m_crBack);
	if (pMetrics->bEnabled && nHtPressed == XTP_HTSCROLLUP)
		pDC->Draw3dRect(pMetrics->rcArrowUp, m_crBorder, m_crBorderHilite);
	else
		pDC->Draw3dRect(pMetrics->rcArrowUp, m_crBorderHilite, m_crBorder);

	pDC->FillSolidRect(pMetrics->rcArrowDown, m_crBack);
	if (pMetrics->bEnabled && nHtPressed == XTP_HTSCROLLDOWN)
		pDC->Draw3dRect(pMetrics->rcArrowDown, m_crBorder, m_crBorderHilite);
	else
		pDC->Draw3dRect(pMetrics->rcArrowDown, m_crBorderHilite, m_crBorder);
}

void CXTPScrollBarThemeFlat::DrawArrows(CDC* pDC, CXTPScrollBase* pScroll,
										XTPScrollMetricsV* pMetrics)
{
	ASSERT(pDC);
	ASSERT(NULL != pScroll);
	ASSERT(NULL != pMetrics);

	UNREFERENCED_PARAMETER(pScroll);

	DrawArrowGlyph(pDC, pMetrics->rcArrowUp, xtpArrowGlyphUp, pMetrics->bEnabled);
	DrawArrowGlyph(pDC, pMetrics->rcArrowDown, xtpArrowGlyphDown, pMetrics->bEnabled);
}

void CXTPScrollBarThemeFlat::DrawTracker(CDC* pDC, CXTPScrollBase* pScroll,
										 XTPScrollMetricsV* pMetrics)
{
	ASSERT(pDC);
	ASSERT(NULL != pScroll);
	ASSERT(NULL != pMetrics);

	XTP_SCROLLBAR_TRACKINFO* pSBTrack = pScroll->GetScrollBarTrackInfo();
	XTP_SCROLLBAR_POSINFO* pSBInfo	  = pScroll->GetScrollBarPosInfo();
	int nHtPressed					  = (NULL != pSBTrack) ? pSBInfo->ht : -1;

	CWnd* pParent = pScroll->GetParentWindow();
	HBRUSH hbrRet = (HBRUSH)DefWindowProc(pParent->GetSafeHwnd(), WM_CTLCOLORSCROLLBAR,
										  (WPARAM)pDC->GetSafeHdc(),
										  (LPARAM)pParent->GetSafeHwnd());

	::FillRect(pDC->GetSafeHdc(), &pMetrics->rcTrack, hbrRet);

	if (nHtPressed == XTP_HTSCROLLUPPAGE)
	{
		::InvertRect(pDC->GetSafeHdc(), &pMetrics->rcLowerTrack);
	}
}

void CXTPScrollBarThemeFlat::DrawThumbButton(CDC* pDC, CXTPScrollBase* pScroll,
											 XTPScrollMetricsV* pMetrics)
{
	ASSERT(pDC);
	ASSERT(NULL != pScroll);
	ASSERT(NULL != pMetrics);

	UNREFERENCED_PARAMETER(pScroll);

	if (!pMetrics->rcTrack.IsRectEmpty() && !pMetrics->rcBtnTrack.IsRectEmpty())
	{
		pDC->FillSolidRect(pMetrics->rcBtnTrack, m_crBack);
		pDC->Draw3dRect(pMetrics->rcBtnTrack, m_crBorderHilite, m_crBorder);
	}
}

void CXTPScrollBarThemeFlat::DrawButtons(CDC* pDC, CXTPScrollBase* pScroll,
										 XTPScrollMetricsH* pMetrics)
{
	ASSERT(pDC);
	ASSERT(NULL != pScroll);
	ASSERT(NULL != pMetrics);

	XTP_SCROLLBAR_TRACKINFO* pSBTrack = pScroll->GetScrollBarTrackInfo();
	XTP_SCROLLBAR_POSINFO* pSBInfo	  = pScroll->GetScrollBarPosInfo();
	int nHtPressed					  = (NULL != pSBTrack) ? pSBInfo->ht : -1;

	pDC->FillSolidRect(pMetrics->rcArrowLeft, m_crBack);
	if (pMetrics->bEnabled && nHtPressed == XTP_HTSCROLLUP)
	{
		pDC->Draw3dRect(pMetrics->rcArrowLeft, m_crBorder, m_crBorderHilite);
	}
	else
	{
		pDC->Draw3dRect(pMetrics->rcArrowLeft, m_crBorderHilite, m_crBorder);
	}

	pDC->FillSolidRect(pMetrics->rcArrowRight, m_crBack);
	if (pMetrics->bEnabled && nHtPressed == XTP_HTSCROLLDOWN)
	{
		pDC->Draw3dRect(pMetrics->rcArrowRight, m_crBorder, m_crBorderHilite);
	}
	else
	{
		pDC->Draw3dRect(pMetrics->rcArrowRight, m_crBorderHilite, m_crBorder);
	}
}

void CXTPScrollBarThemeFlat::DrawArrows(CDC* pDC, CXTPScrollBase* pScroll,
										XTPScrollMetricsH* pMetrics)
{
	ASSERT(pDC);
	ASSERT(NULL != pScroll);
	ASSERT(NULL != pMetrics);

	UNREFERENCED_PARAMETER(pScroll);

	DrawArrowGlyph(pDC, pMetrics->rcArrowLeft, xtpArrowGlyphLeft, pMetrics->bEnabled);

	DrawArrowGlyph(pDC, pMetrics->rcArrowRight, xtpArrowGlyphRight, pMetrics->bEnabled);
}

void CXTPScrollBarThemeFlat::DrawTracker(CDC* pDC, CXTPScrollBase* pScroll,
										 XTPScrollMetricsH* pMetrics)
{
	ASSERT(pDC);
	ASSERT(NULL != pScroll);
	ASSERT(NULL != pMetrics);

	XTP_SCROLLBAR_TRACKINFO* pSBTrack = pScroll->GetScrollBarTrackInfo();
	XTP_SCROLLBAR_POSINFO* pSBInfo	  = pScroll->GetScrollBarPosInfo();
	int nHtPressed					  = (NULL != pSBTrack) ? pSBInfo->ht : -1;

	CWnd* pParent = pScroll->GetParentWindow();
	HBRUSH hbrRet = (HBRUSH)DefWindowProc(pParent->GetSafeHwnd(), WM_CTLCOLORSCROLLBAR,
										  (WPARAM)pDC->GetSafeHdc(),
										  (LPARAM)pParent->GetSafeHwnd());

	::FillRect(pDC->GetSafeHdc(), &pMetrics->rcTrack, hbrRet);

	if (nHtPressed == XTP_HTSCROLLUPPAGE)
	{
		::InvertRect(pDC->GetSafeHdc(), &pMetrics->rcLowerTrack);
	}
}

void CXTPScrollBarThemeFlat::DrawThumbButton(CDC* pDC, CXTPScrollBase* pScroll,
											 XTPScrollMetricsH* pMetrics)
{
	ASSERT(pDC);
	ASSERT(NULL != pScroll);
	ASSERT(NULL != pMetrics);

	UNREFERENCED_PARAMETER(pScroll);

	if (!pMetrics->rcTrack.IsRectEmpty() && !pMetrics->rcBtnTrack.IsRectEmpty())
	{
		pDC->FillSolidRect(pMetrics->rcBtnTrack, m_crBack);
		pDC->Draw3dRect(pMetrics->rcBtnTrack, m_crBorderHilite, m_crBorder);
	}
}

void CXTPScrollBarThemeFlat::DrawGripper(CDC* pDC, CXTPScrollBase* pScroll)
{
	ASSERT(pDC);
	ASSERT(NULL != pScroll);

	XTP_SCROLLBAR_POSINFO* pSBInfo = pScroll->GetScrollBarPosInfo();
	pDC->FillSolidRect(&pSBInfo->rc, GetXtremeColor(COLOR_3DFACE));
}
