// XTPScrollBarThemeWindowsClassic.cpp
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
#include "Common/XTPCasting.h"
#include "Common/XTPFramework.h"
#include "Common/XTPSystemHelpers.h"
#include "Common/XTPSynchro.h"
#include "Common/XTPApplication.h"
#include "Common/XTPSingleton.h"
#include "Common/XTPColorManager.h"
#include "Common/ScrollBar/XTPScrollBase.h"
#include "Common/ScrollBar/XTPScrollBarPaintManager.h"
#include "Common/ScrollBar/Themes/XTPScrollBarThemeWindowsClassic.h"

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

#ifdef _DEBUG
#	define new DEBUG_NEW
#	undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void CXTPScrollBarThemeWindowsClassic::DrawButtons(CDC* pDC, CXTPScrollBase* pScroll,
												   XTPScrollMetricsV* pMetrics)
{
	ASSERT(pDC);
	ASSERT(NULL != pScroll);
	ASSERT(NULL != pMetrics);

	XTP_SCROLLBAR_TRACKINFO* pSBTrack = pScroll->GetScrollBarTrackInfo();
	XTP_SCROLLBAR_POSINFO* pSBInfo	  = pScroll->GetScrollBarPosInfo();
	int nHtPressed					  = (NULL != pSBTrack) ? pSBInfo->ht : -1;

	::DrawFrameControl(pDC->GetSafeHdc(), &pMetrics->rcArrowUp, XTPToUInt(DFC_SCROLL),
					   XTPToUInt(DFCS_SCROLLUP | (!pMetrics->bEnabled ? DFCS_INACTIVE : 0)
								 | (nHtPressed == XTP_HTSCROLLUP ? DFCS_PUSHED : 0)));
	::DrawFrameControl(pDC->GetSafeHdc(), &pMetrics->rcArrowDown, XTPToUInt(DFC_SCROLL),
					   XTPToUInt(DFCS_SCROLLDOWN | (!pMetrics->bEnabled ? DFCS_INACTIVE : 0)
								 | (nHtPressed == XTP_HTSCROLLDOWN ? DFCS_PUSHED : 0)));
}

void CXTPScrollBarThemeWindowsClassic::DrawTracker(CDC* pDC, CXTPScrollBase* pScroll,
												   XTPScrollMetricsV* pMetrics)
{
	ASSERT(pDC);
	ASSERT(NULL != pScroll);
	ASSERT(NULL != pMetrics);

	XTP_SCROLLBAR_TRACKINFO* pSBTrack = pScroll->GetScrollBarTrackInfo();
	XTP_SCROLLBAR_POSINFO* pSBInfo	  = pScroll->GetScrollBarPosInfo();

	int nHtPressed = (NULL != pSBTrack) ? pSBInfo->ht : -1;

	CWnd* pParent = pScroll->GetParentWindow();
	HBRUSH hbrRet = (HBRUSH)DefWindowProc(pParent->GetSafeHwnd(), WM_CTLCOLORSCROLLBAR,
										  (WPARAM)pDC->GetSafeHdc(),
										  (LPARAM)pParent->GetSafeHwnd());

	::FillRect(pDC->GetSafeHdc(), &pMetrics->rcTrack, hbrRet);

	if (nHtPressed == XTP_HTSCROLLUPPAGE)
	{
		::InvertRect(pDC->GetSafeHdc(), &pMetrics->rcLowerTrack);
	}

	if (!pMetrics->rcTrack.IsRectEmpty() && !pMetrics->rcBtnTrack.IsRectEmpty())
	{
		pDC->FillSolidRect(pMetrics->rcBtnTrack, GetXtremeColor(COLOR_3DFACE));
		DrawEdge(pDC->GetSafeHdc(), &pMetrics->rcBtnTrack, EDGE_RAISED,
				 (UINT)(BF_ADJUST | BF_RECT));
	}
}

void CXTPScrollBarThemeWindowsClassic::DrawButtons(CDC* pDC, CXTPScrollBase* pScroll,
												   XTPScrollMetricsH* pMetrics)
{
	ASSERT(pDC);
	ASSERT(NULL != pScroll);
	ASSERT(NULL != pMetrics);

	XTP_SCROLLBAR_TRACKINFO* pSBTrack = pScroll->GetScrollBarTrackInfo();
	XTP_SCROLLBAR_POSINFO* pSBInfo	  = pScroll->GetScrollBarPosInfo();
	int nHtPressed					  = (NULL != pSBTrack) ? pSBInfo->ht : -1;

	::DrawFrameControl(pDC->GetSafeHdc(), &pMetrics->rcArrowLeft, XTPToUInt(DFC_SCROLL),
					   XTPToUInt(DFCS_SCROLLLEFT | (!pMetrics->bEnabled ? DFCS_INACTIVE : 0)
								 | (nHtPressed == XTP_HTSCROLLUP ? DFCS_PUSHED : 0)));
	::DrawFrameControl(pDC->GetSafeHdc(), &pMetrics->rcArrowRight, XTPToUInt(DFC_SCROLL),
					   XTPToUInt(DFCS_SCROLLRIGHT | (!pMetrics->bEnabled ? DFCS_INACTIVE : 0)
								 | (nHtPressed == XTP_HTSCROLLDOWN ? DFCS_PUSHED : 0)));
}

void CXTPScrollBarThemeWindowsClassic::DrawTracker(CDC* pDC, CXTPScrollBase* pScroll,
												   XTPScrollMetricsH* pMetrics)
{
	ASSERT(pDC);
	ASSERT(NULL != pScroll);
	ASSERT(NULL != pMetrics);

	XTP_SCROLLBAR_TRACKINFO* pSBTrack = pScroll->GetScrollBarTrackInfo();
	XTP_SCROLLBAR_POSINFO* pSBInfo	  = pScroll->GetScrollBarPosInfo();

	int nHtPressed = (NULL != pSBTrack) ? pSBInfo->ht : -1;

	CWnd* pParent = pScroll->GetParentWindow();
	HBRUSH hbrRet = (HBRUSH)DefWindowProc(pParent->GetSafeHwnd(), WM_CTLCOLORSCROLLBAR,
										  (WPARAM)pDC->GetSafeHdc(),
										  (LPARAM)pParent->GetSafeHwnd());

	::FillRect(pDC->GetSafeHdc(), &pMetrics->rcTrack, hbrRet);

	if (nHtPressed == XTP_HTSCROLLUPPAGE)
	{
		::InvertRect(pDC->GetSafeHdc(), &pMetrics->rcLowerTrack);
	}

	if (!pMetrics->rcTrack.IsRectEmpty() && !pMetrics->rcBtnTrack.IsRectEmpty())
	{
		pDC->FillSolidRect(pMetrics->rcBtnTrack, GetXtremeColor(COLOR_3DFACE));
		DrawEdge(pDC->GetSafeHdc(), &pMetrics->rcBtnTrack, EDGE_RAISED,
				 (UINT)(BF_ADJUST | BF_RECT));
	}
}

void CXTPScrollBarThemeWindowsClassic::DrawGripper(CDC* pDC, CXTPScrollBase* pScroll)
{
	ASSERT(pDC);
	ASSERT(NULL != pScroll);

	XTP_SCROLLBAR_POSINFO* pSBInfo = pScroll->GetScrollBarPosInfo();
	::DrawFrameControl(pDC->GetSafeHdc(), &pSBInfo->rc, DFC_SCROLL, DFCS_SCROLLSIZEGRIP);
}
