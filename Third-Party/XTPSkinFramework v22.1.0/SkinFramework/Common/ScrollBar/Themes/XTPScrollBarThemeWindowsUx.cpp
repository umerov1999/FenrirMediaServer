// XTPScrollBarThemeWindowsUx.cpp
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

#include "Common/XTPTypeId.h"
#include "Common/XTPFramework.h"
#include "Common/XTPSystemHelpers.h"
#include "Common/XTPSynchro.h"
#include "Common/XTPApplication.h"
#include "Common/XTPSingleton.h"
#include "Common/XTPGdiObjects.h"
#include "Common/XTPWinThemeWrapper.h"
#include "Common/XTPDrawHelpers.h"
#include "Common/ScrollBar/XTPScrollBase.h"
#include "Common/ScrollBar/XTPScrollBarPaintManager.h"
#include "Common/ScrollBar/Themes/XTPScrollBarThemeWindowsClassic.h"
#include "Common/ScrollBar/Themes/XTPScrollBarThemeWindowsUx.h"

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

#ifdef _DEBUG
#	define new DEBUG_NEW
#	undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CXTPScrollBarThemeWindowsUx::CXTPScrollBarThemeWindowsUx()
{
	m_themeScrollBar = new CXTPWinThemeWrapper();
}

CXTPScrollBarThemeWindowsUx::~CXTPScrollBarThemeWindowsUx()
{
	SAFE_DELETE(m_themeScrollBar);
}

void CXTPScrollBarThemeWindowsUx::RefreshMetrics()
{
	CXTPScrollBarPaintManager::RefreshMetrics();

	m_themeScrollBar->CloseThemeData();
	m_themeScrollBar->OpenThemeData(NULL, L"SCROLLBAR");

	if (!m_themeScrollBar->IsAppThemeReady())
	{
		CXTPScrollBarThemeWindowsClassic::RefreshMetrics();
	}
}

#define GETPARTSTATE(ht, pressed, hot, normal, disabled)                                           \
	(!pMetrics->bEnabled ? disabled : nHtPressed == ht ? pressed : nHtHot == ht ? hot : normal)

void CXTPScrollBarThemeWindowsUx::DrawButtons(CDC* pDC, CXTPScrollBase* pScroll,
											  XTPScrollMetricsV* pMetrics)
{
	ASSERT(pDC);
	ASSERT(NULL != pScroll);
	ASSERT(NULL != pMetrics);

	if (m_themeScrollBar->IsAppThemeReady())
	{
		XTP_SCROLLBAR_TRACKINFO* pSBTrack = pScroll->GetScrollBarTrackInfo();
		XTP_SCROLLBAR_POSINFO* pSBInfo	= pScroll->GetScrollBarPosInfo();
		int nHtHot						  = (NULL != pSBTrack) ? -1 : pSBInfo->ht;
		int nHtPressed					  = (NULL != pSBTrack) ? pSBInfo->ht : -1;

		m_themeScrollBar->DrawThemeBackground(pDC->GetSafeHdc(), SBP_ARROWBTN,
											  GETPARTSTATE(XTP_HTSCROLLUP, ABS_UPPRESSED, ABS_UPHOT,
														   ABS_UPNORMAL, ABS_UPDISABLED),
											  pMetrics->rcArrowUp, NULL);
		m_themeScrollBar->DrawThemeBackground(pDC->GetSafeHdc(), SBP_ARROWBTN,
											  GETPARTSTATE(XTP_HTSCROLLDOWN, ABS_DOWNPRESSED,
														   ABS_DOWNHOT, ABS_DOWNNORMAL,
														   ABS_DOWNDISABLED),
											  pMetrics->rcArrowDown, NULL);
	}
	else
	{
		CXTPScrollBarThemeWindowsClassic::DrawButtons(pDC, pScroll, pMetrics);
	}
}

void CXTPScrollBarThemeWindowsUx::DrawTracker(CDC* pDC, CXTPScrollBase* pScroll,
											  XTPScrollMetricsV* pMetrics)
{
	ASSERT(pDC);
	ASSERT(NULL != pScroll);
	ASSERT(NULL != pMetrics);

	if (m_themeScrollBar->IsAppThemeReady())
	{
		XTP_SCROLLBAR_TRACKINFO* pSBTrack = pScroll->GetScrollBarTrackInfo();
		XTP_SCROLLBAR_POSINFO* pSBInfo	= pScroll->GetScrollBarPosInfo();
		int nHtHot						  = (NULL != pSBTrack) ? -1 : pSBInfo->ht;
		int nHtPressed					  = (NULL != pSBTrack) ? pSBInfo->ht : -1;

		if (!pMetrics->rcTrack.IsRectEmpty())
		{
			if (!pMetrics->rcLowerTrack.IsRectEmpty())
				m_themeScrollBar->DrawThemeBackground(pDC->GetSafeHdc(), SBP_LOWERTRACKVERT,
													  GETPARTSTATE(XTP_HTSCROLLUPPAGE,
																   SCRBS_PRESSED, SCRBS_HOT,
																   SCRBS_NORMAL, SCRBS_DISABLED),
													  pMetrics->rcLowerTrack, NULL);

			if (!pMetrics->rcUpperTrack.IsRectEmpty())
				m_themeScrollBar->DrawThemeBackground(pDC->GetSafeHdc(), SBP_UPPERTRACKVERT,
													  GETPARTSTATE(XTP_HTSCROLLDOWNPAGE,
																   SCRBS_PRESSED, SCRBS_HOT,
																   SCRBS_NORMAL, SCRBS_DISABLED),
													  pMetrics->rcUpperTrack, NULL);
		}
	}
	else
	{
		CXTPScrollBarThemeWindowsClassic::DrawTracker(pDC, pScroll, pMetrics);
	}
}

void CXTPScrollBarThemeWindowsUx::DrawThumbButton(CDC* pDC, CXTPScrollBase* pScroll,
												  XTPScrollMetricsV* pMetrics)
{
	ASSERT(pDC);
	ASSERT(NULL != pScroll);
	ASSERT(NULL != pMetrics);

	if (m_themeScrollBar->IsAppThemeReady())
	{
		if (!pMetrics->rcTrack.IsRectEmpty() && !pMetrics->rcBtnTrack.IsRectEmpty())
		{
			XTP_SCROLLBAR_TRACKINFO* pSBTrack = pScroll->GetScrollBarTrackInfo();
			XTP_SCROLLBAR_POSINFO* pSBInfo	= pScroll->GetScrollBarPosInfo();
			int nHtHot						  = (NULL != pSBTrack) ? -1 : pSBInfo->ht;
			int nHtPressed					  = (NULL != pSBTrack) ? pSBInfo->ht : -1;

			m_themeScrollBar->DrawThemeBackground(pDC->GetSafeHdc(), SBP_THUMBBTNVERT,
												  GETPARTSTATE(XTP_HTSCROLLTHUMB, SCRBS_PRESSED,
															   SCRBS_HOT, SCRBS_NORMAL,
															   SCRBS_DISABLED),
												  pMetrics->rcBtnTrack, NULL);
			if (pMetrics->rcBtnTrack.Height() > XTP_DPI_Y(13))
				m_themeScrollBar->DrawThemeBackground(pDC->GetSafeHdc(), SBP_GRIPPERVERT,
													  GETPARTSTATE(XTP_HTSCROLLTHUMB, SCRBS_PRESSED,
																   SCRBS_HOT, SCRBS_NORMAL,
																   SCRBS_DISABLED),
													  pMetrics->rcBtnTrack, NULL);
		}
	}
	else
	{
		CXTPScrollBarThemeWindowsClassic::DrawThumbButton(pDC, pScroll, pMetrics);
	}
}

void CXTPScrollBarThemeWindowsUx::DrawButtons(CDC* pDC, CXTPScrollBase* pScroll,
											  XTPScrollMetricsH* pMetrics)
{
	ASSERT(pDC);
	ASSERT(NULL != pScroll);
	ASSERT(NULL != pMetrics);

	if (m_themeScrollBar->IsAppThemeReady())
	{
		XTP_SCROLLBAR_TRACKINFO* pSBTrack = pScroll->GetScrollBarTrackInfo();
		XTP_SCROLLBAR_POSINFO* pSBInfo	= pScroll->GetScrollBarPosInfo();
		int nHtHot						  = (NULL != pSBTrack) ? -1 : pSBInfo->ht;
		int nHtPressed					  = (NULL != pSBTrack) ? pSBInfo->ht : -1;

		m_themeScrollBar->DrawThemeBackground(pDC->GetSafeHdc(), SBP_ARROWBTN,
											  GETPARTSTATE(XTP_HTSCROLLUP, ABS_LEFTPRESSED,
														   ABS_LEFTHOT, ABS_LEFTNORMAL,
														   ABS_LEFTDISABLED),
											  pMetrics->rcArrowLeft, NULL);
		m_themeScrollBar->DrawThemeBackground(pDC->GetSafeHdc(), SBP_ARROWBTN,
											  GETPARTSTATE(XTP_HTSCROLLDOWN, ABS_RIGHTPRESSED,
														   ABS_RIGHTHOT, ABS_RIGHTNORMAL,
														   ABS_RIGHTDISABLED),
											  pMetrics->rcArrowRight, NULL);
	}
	else
	{
		CXTPScrollBarThemeWindowsClassic::DrawButtons(pDC, pScroll, pMetrics);
	}
}

void CXTPScrollBarThemeWindowsUx::DrawTracker(CDC* pDC, CXTPScrollBase* pScroll,
											  XTPScrollMetricsH* pMetrics)
{
	ASSERT(pDC);
	ASSERT(NULL != pScroll);
	ASSERT(NULL != pMetrics);

	if (m_themeScrollBar->IsAppThemeReady())
	{
		XTP_SCROLLBAR_TRACKINFO* pSBTrack = pScroll->GetScrollBarTrackInfo();
		XTP_SCROLLBAR_POSINFO* pSBInfo	= pScroll->GetScrollBarPosInfo();
		int nHtHot						  = (NULL != pSBTrack) ? -1 : pSBInfo->ht;
		int nHtPressed					  = (NULL != pSBTrack) ? pSBInfo->ht : -1;

		if (!pMetrics->rcTrack.IsRectEmpty())
		{
			if (!pMetrics->rcLowerTrack.IsRectEmpty())
				m_themeScrollBar->DrawThemeBackground(pDC->GetSafeHdc(), SBP_LOWERTRACKHORZ,
													  GETPARTSTATE(XTP_HTSCROLLUPPAGE,
																   SCRBS_PRESSED, SCRBS_HOT,
																   SCRBS_NORMAL, SCRBS_DISABLED),
													  pMetrics->rcLowerTrack, NULL);

			if (!pMetrics->rcUpperTrack.IsRectEmpty())
			{
				m_themeScrollBar->DrawThemeBackground(pDC->GetSafeHdc(), SBP_UPPERTRACKHORZ,
													  GETPARTSTATE(XTP_HTSCROLLDOWNPAGE,
																   SCRBS_PRESSED, SCRBS_HOT,
																   SCRBS_NORMAL, SCRBS_DISABLED),
													  pMetrics->rcUpperTrack, NULL);
			}
		}
	}
	else
	{
		CXTPScrollBarThemeWindowsClassic::DrawTracker(pDC, pScroll, pMetrics);
	}
}

void CXTPScrollBarThemeWindowsUx::DrawThumbButton(CDC* pDC, CXTPScrollBase* pScroll,
												  XTPScrollMetricsH* pMetrics)
{
	ASSERT(pDC);
	ASSERT(NULL != pScroll);
	ASSERT(NULL != pMetrics);

	if (m_themeScrollBar->IsAppThemeReady())
	{
		if (!pMetrics->rcTrack.IsRectEmpty() && !pMetrics->rcBtnTrack.IsRectEmpty())
		{
			XTP_SCROLLBAR_TRACKINFO* pSBTrack = pScroll->GetScrollBarTrackInfo();
			XTP_SCROLLBAR_POSINFO* pSBInfo	= pScroll->GetScrollBarPosInfo();
			int nHtHot						  = (NULL != pSBTrack) ? -1 : pSBInfo->ht;
			int nHtPressed					  = (NULL != pSBTrack) ? pSBInfo->ht : -1;

			m_themeScrollBar->DrawThemeBackground(pDC->GetSafeHdc(), SBP_THUMBBTNHORZ,
												  GETPARTSTATE(XTP_HTSCROLLTHUMB, SCRBS_PRESSED,
															   SCRBS_HOT, SCRBS_NORMAL,
															   SCRBS_DISABLED),
												  pMetrics->rcBtnTrack, NULL);
			if (pMetrics->rcBtnTrack.Width() > XTP_DPI_Y(13))
			{
				m_themeScrollBar->DrawThemeBackground(pDC->GetSafeHdc(), SBP_GRIPPERHORZ,
													  GETPARTSTATE(XTP_HTSCROLLTHUMB, SCRBS_PRESSED,
																   SCRBS_HOT, SCRBS_NORMAL,
																   SCRBS_DISABLED),
													  pMetrics->rcBtnTrack, NULL);
			}
		}
	}
	else
	{
		CXTPScrollBarThemeWindowsClassic::DrawThumbButton(pDC, pScroll, pMetrics);
	}
}

void CXTPScrollBarThemeWindowsUx::DrawGripper(CDC* pDC, CXTPScrollBase* pScroll)
{
	ASSERT(pDC);
	ASSERT(NULL != pScroll);

	if (m_themeScrollBar->IsAppThemeReady())
	{
		XTP_SCROLLBAR_POSINFO* pSBInfo = pScroll->GetScrollBarPosInfo();

		COLORREF crBackground = m_themeScrollBar->GetThemeSysColor(COLOR_BTNFACE);
		pDC->FillSolidRect(&pSBInfo->rc, crBackground);
		m_themeScrollBar->DrawThemeBackground(pDC->GetSafeHdc(), SBP_SIZEBOX, 0, &pSBInfo->rc,
											  NULL);
	}
	else
	{
		CXTPScrollBarThemeWindowsClassic::DrawGripper(pDC, pScroll);
	}
}
