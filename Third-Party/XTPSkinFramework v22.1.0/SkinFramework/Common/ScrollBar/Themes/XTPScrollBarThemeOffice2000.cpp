// XTPScrollBarThemeOffice2000.cpp
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
#include "Common/XTPColorManager.h"
#include "Common/ScrollBar/XTPScrollBase.h"
#include "Common/ScrollBar/XTPScrollBarPaintManager.h"
#include "Common/ScrollBar/Themes/XTPScrollBarThemeWindowsClassic.h"
#include "Common/ScrollBar/Themes/XTPScrollBarThemeWindowsUx.h"
#include "Common/ScrollBar/Themes/XTPScrollBarThemeOffice2000.h"

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

#ifdef _DEBUG
#	define new DEBUG_NEW
#	undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void CXTPScrollBarThemeOffice2000::DrawButtons(CDC* pDC, CXTPScrollBase* pScroll,
											   XTPScrollMetricsV* pMetrics)
{
	ASSERT(pDC);
	ASSERT(NULL != pScroll);
	ASSERT(NULL != pMetrics);

	CXTPScrollBarThemeWindowsClassic::DrawButtons(pDC, pScroll, pMetrics);

	XTP_SCROLLBAR_TRACKINFO* pSBTrack = pScroll->GetScrollBarTrackInfo();
	XTP_SCROLLBAR_POSINFO* pSBInfo	= pScroll->GetScrollBarPosInfo();
	int nHtPressed					  = (NULL != pSBTrack) ? pSBInfo->ht : -1;

	// Draw up button
	if (pMetrics->bEnabled && nHtPressed == XTP_HTSCROLLUP)
	{
		pDC->Draw3dRect(pMetrics->rcArrowUp, GetXtremeColor(COLOR_3DSHADOW),
						GetXtremeColor(COLOR_3DHIGHLIGHT));
	}
	else
	{
		pDC->Draw3dRect(pMetrics->rcArrowUp, GetXtremeColor(COLOR_3DHIGHLIGHT),
						GetXtremeColor(COLOR_3DSHADOW));
	}

	// Drawing down button
	if (pMetrics->bEnabled && nHtPressed == XTP_HTSCROLLDOWN)
	{
		pDC->Draw3dRect(pMetrics->rcArrowDown, GetXtremeColor(COLOR_3DSHADOW),
						GetXtremeColor(COLOR_3DHIGHLIGHT));
	}
	else
	{
		pDC->Draw3dRect(pMetrics->rcArrowDown, GetXtremeColor(COLOR_3DHIGHLIGHT),
						GetXtremeColor(COLOR_3DSHADOW));
	}
}

void CXTPScrollBarThemeOffice2000::DrawArrows(CDC* pDC, CXTPScrollBase* pScroll,
											  XTPScrollMetricsV* pMetrics)
{
	UNREFERENCED_PARAMETER(pDC);
	UNREFERENCED_PARAMETER(pScroll);
	UNREFERENCED_PARAMETER(pMetrics);
}

void CXTPScrollBarThemeOffice2000::DrawButtons(CDC* pDC, CXTPScrollBase* pScroll,
											   XTPScrollMetricsH* pMetrics)
{
	ASSERT(pDC);
	ASSERT(NULL != pScroll);
	ASSERT(NULL != pMetrics);

	CXTPScrollBarThemeWindowsClassic::DrawButtons(pDC, pScroll, pMetrics);

	XTP_SCROLLBAR_TRACKINFO* pSBTrack = pScroll->GetScrollBarTrackInfo();
	XTP_SCROLLBAR_POSINFO* pSBInfo	= pScroll->GetScrollBarPosInfo();
	int nHtPressed					  = (NULL != pSBTrack) ? pSBInfo->ht : -1;

	// Draw left button
	if (pMetrics->bEnabled && nHtPressed == XTP_HTSCROLLLEFT)
	{
		pDC->Draw3dRect(pMetrics->rcArrowLeft, GetXtremeColor(COLOR_3DSHADOW),
						GetXtremeColor(COLOR_3DHIGHLIGHT));
	}
	else
	{
		pDC->Draw3dRect(pMetrics->rcArrowLeft, GetXtremeColor(COLOR_3DHIGHLIGHT),
						GetXtremeColor(COLOR_3DSHADOW));
	}

	// Draw right button
	if (pMetrics->bEnabled && nHtPressed == XTP_HTSCROLLRIGHT)
	{
		pDC->Draw3dRect(pMetrics->rcArrowRight, GetXtremeColor(COLOR_3DSHADOW),
						GetXtremeColor(COLOR_3DHIGHLIGHT));
	}
	else
	{
		pDC->Draw3dRect(pMetrics->rcArrowRight, GetXtremeColor(COLOR_3DHIGHLIGHT),
						GetXtremeColor(COLOR_3DSHADOW));
	}
}

void CXTPScrollBarThemeOffice2000::DrawArrows(CDC* pDC, CXTPScrollBase* pScroll,
											  XTPScrollMetricsH* pMetrics)
{
	UNREFERENCED_PARAMETER(pDC);
	UNREFERENCED_PARAMETER(pScroll);
	UNREFERENCED_PARAMETER(pMetrics);
}
