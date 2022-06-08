// XTPScrollBarThemeOffice2013.cpp
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
#include "Common/XTPGdiObjects.h"
#include "Common/XTPDrawHelpers.h"
#include "Common/XTPResourceImage.h"
#include "Common/ScrollBar/XTPScrollBase.h"
#include "Common/ScrollBar/XTPScrollBarPaintManager.h"
#include "Common/ScrollBar/Themes/XTPScrollBarThemeOffice2013.h"

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

#ifdef _DEBUG
#	define new DEBUG_NEW
#	undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////
// CXTPScrollBarThemeOffice2013

CXTPScrollBarThemeOffice2013::CXTPScrollBarThemeOffice2013()
	: CXTPScrollBarPaintManager()
{
}

CXTPScrollBarThemeOffice2013::~CXTPScrollBarThemeOffice2013()
{
}

void CXTPScrollBarThemeOffice2013::RefreshMetrics()
{
	CXTPScrollBarPaintManager::RefreshMetrics();

	// Normal
	m_clrNormal.clrBack = XTPIniColor(_T("Common.ScrollBar"), _T("NormalBack"), RGB(243, 243, 243));
	m_clrNormal.clrScrollBorder = XTPIniColor(_T("Common.ScrollBar"), _T("NormalScrollBorder"),
											  RGB(171, 171, 171));
	m_clrNormal.clrScrollBack	= XTPIniColor(_T("Common.ScrollBar"), _T("NormalScrollBack"),
											  RGB(255, 255, 255));
	m_clrNormal.clrScrollArrow	= XTPIniColor(_T("Common.ScrollBar"), _T("NormalScrollArrow"),
											  RGB(119, 119, 119));
	m_clrNormal.clrThumbBorder	= XTPIniColor(_T("Common.ScrollBar"), _T("NormalThumbBorder"),
											  RGB(171, 171, 171));
	m_clrNormal.clrThumbBack	= XTPIniColor(_T("Common.ScrollBar"), _T("NormalThumbBack"),
											  RGB(255, 255, 255));

	// Pressed
	m_clrPressed.clrBack		 = XTPIniColor(_T("Common.ScrollBar"), _T("PressedBack"),
									   RGB(243, 243, 243));
	m_clrPressed.clrScrollBorder = XTPIniColor(_T("Common.ScrollBar"), _T("PressedScrollBorder"),
											   RGB(119, 119, 119));
	m_clrPressed.clrScrollBack	 = XTPIniColor(_T("Common.ScrollBar"), _T("PressedScrollBack"),
											   RGB(240, 240, 240));
	m_clrPressed.clrScrollArrow	 = XTPIniColor(_T("Common.ScrollBar"), _T("PressedScrollArrow"),
											   RGB(119, 119, 119));
	m_clrPressed.clrThumbBorder	 = XTPIniColor(_T("Common.ScrollBar"), _T("PressedThumbBorder"),
											   RGB(119, 119, 119));
	m_clrPressed.clrThumbBack	 = XTPIniColor(_T("Common.ScrollBar"), _T("PressedThumbBack"),
											   RGB(240, 240, 240));

	// Selected
	m_clrSelected.clrBack		  = XTPIniColor(_T("Common.ScrollBar"), _T("SelectedBack"),
										RGB(243, 243, 243));
	m_clrSelected.clrScrollBorder = XTPIniColor(_T("Common.ScrollBar"), _T("SelectedScrollBorder"),
												RGB(119, 119, 119));
	m_clrSelected.clrScrollBack	  = XTPIniColor(_T("Common.ScrollBar"), _T("SelectedScrollBack"),
												RGB(255, 255, 255));
	m_clrSelected.clrScrollArrow  = XTPIniColor(_T("Common.ScrollBar"), _T("SelectedScrollArrow"),
												RGB(119, 119, 119));
	m_clrSelected.clrThumbBorder  = XTPIniColor(_T("Common.ScrollBar"), _T("SelectedThumbBorder"),
												RGB(171, 171, 171));
	m_clrSelected.clrThumbBack	  = XTPIniColor(_T("Common.ScrollBar"), _T("SelectedThumbBack"),
												RGB(240, 240, 240));

	// Disabled
	m_clrDisabled.clrBack		  = XTPIniColor(_T("Common.ScrollBar"), _T("DisabledBack"),
										RGB(243, 243, 243));
	m_clrDisabled.clrScrollBorder = XTPIniColor(_T("Common.ScrollBar"), _T("DisabledScrollBorder"),
												RGB(243, 243, 243));
	m_clrDisabled.clrScrollBack	  = XTPIniColor(_T("Common.ScrollBar"), _T("DisabledScrollBack"),
												RGB(243, 243, 243));
	m_clrDisabled.clrScrollArrow  = XTPIniColor(_T("Common.ScrollBar"), _T("DisabledScrollArrow"),
												RGB(171, 171, 171));
	m_clrDisabled.clrThumbBorder  = XTPIniColor(_T("Common.ScrollBar"), _T("DisabledThumbBorder"),
												RGB(243, 243, 243));
	m_clrDisabled.clrThumbBack	  = XTPIniColor(_T("Common.ScrollBar"), _T("DisabledThumbBack"),
												RGB(243, 243, 243));
}

void CXTPScrollBarThemeOffice2013::GetScrollBarColors(XTP_SCROLLBARDRAWCOLORS& sbc,
													  int nHitSelected, int nHitPressed,
													  BOOL bEnabled)
{
	if (!bEnabled)
	{
		sbc.clrBack				= m_clrDisabled.clrBack;
		sbc.clrScrollArrow		= m_clrDisabled.clrScrollArrow;
		sbc.clrScrollBorder		= m_clrDisabled.clrScrollBorder;
		sbc.clrScrollBack		= m_clrDisabled.clrScrollBack;
		sbc.clrThumbBorder		= m_clrDisabled.clrThumbBorder;
		sbc.clrThumbBack		= m_clrDisabled.clrThumbBack;
		sbc.clrScrollDownArrow	= m_clrDisabled.clrScrollArrow;
		sbc.clrScrollDownBorder = m_clrDisabled.clrScrollBorder;
		sbc.clrScrollDownBack	= m_clrDisabled.clrScrollBack;
	}
	else
	{
		// get the Up / Left scroll button colors.
		if (nHitSelected == XTP_HTSCROLLUP || nHitSelected == XTP_HTSCROLLLEFT)
		{
			sbc.clrScrollArrow	= m_clrSelected.clrScrollArrow;
			sbc.clrScrollBorder = m_clrSelected.clrScrollBorder;
			sbc.clrScrollBack	= m_clrSelected.clrScrollBack;
		}
		else if (nHitPressed == XTP_HTSCROLLUP || nHitPressed == XTP_HTSCROLLLEFT)
		{
			sbc.clrScrollArrow	= m_clrPressed.clrScrollArrow;
			sbc.clrScrollBorder = m_clrPressed.clrScrollBorder;
			sbc.clrScrollBack	= m_clrPressed.clrScrollBack;
		}
		else
		{
			sbc.clrScrollArrow	= m_clrNormal.clrScrollArrow;
			sbc.clrScrollBorder = m_clrNormal.clrScrollBorder;
			sbc.clrScrollBack	= m_clrNormal.clrScrollBack;
		}

		// Get the Down / Right scroll button colors.
		if (nHitSelected == XTP_HTSCROLLDOWN || nHitSelected == XTP_HTSCROLLRIGHT)
		{
			sbc.clrScrollDownArrow	= m_clrSelected.clrScrollArrow;
			sbc.clrScrollDownBorder = m_clrSelected.clrScrollBorder;
			sbc.clrScrollDownBack	= m_clrSelected.clrScrollBack;
		}
		else if (nHitPressed == XTP_HTSCROLLDOWN || nHitPressed == XTP_HTSCROLLRIGHT)
		{
			sbc.clrScrollDownArrow	= m_clrPressed.clrScrollArrow;
			sbc.clrScrollDownBorder = m_clrPressed.clrScrollBorder;
			sbc.clrScrollDownBack	= m_clrPressed.clrScrollBack;
		}
		else
		{
			sbc.clrScrollDownArrow	= m_clrNormal.clrScrollArrow;
			sbc.clrScrollDownBorder = m_clrNormal.clrScrollBorder;
			sbc.clrScrollDownBack	= m_clrNormal.clrScrollBack;
		}

		// Get the Thumb button colors.
		if (nHitSelected == XTP_HTSCROLLTHUMB)
		{
			sbc.clrBack		   = m_clrSelected.clrBack;
			sbc.clrThumbBorder = m_clrSelected.clrThumbBorder;
			sbc.clrThumbBack   = m_clrSelected.clrThumbBack;
		}
		else if (nHitPressed == XTP_HTSCROLLTHUMB)
		{
			sbc.clrBack		   = m_clrPressed.clrBack;
			sbc.clrThumbBorder = m_clrPressed.clrThumbBorder;
			sbc.clrThumbBack   = m_clrPressed.clrThumbBack;
		}
		else
		{
			sbc.clrBack		   = m_clrNormal.clrBack;
			sbc.clrThumbBorder = m_clrNormal.clrThumbBorder;
			sbc.clrThumbBack   = m_clrNormal.clrThumbBack;
		}
	}
}

void CXTPScrollBarThemeOffice2013::DrawScrollBar(CDC* pDC, CXTPScrollBase* pScroll)
{
	ASSERT(NULL != pScroll);

	XTP_SCROLLBAR_POSINFO* pSBInfo = pScroll->GetScrollBarPosInfo();

	if (!pSBInfo->fSizebox)
	{
		XTP_SCROLLBAR_TRACKINFO* pSBTrack = pScroll->GetScrollBarTrackInfo();
		int nHitSelected				  = (NULL != pSBTrack) ? -1 : pSBInfo->ht;
		int nHitPressed					  = (NULL != pSBTrack) ? pSBInfo->ht : -1;

		if (pSBInfo->fVert)
		{
			XTPScrollMetricsV scroll;
			CalcVScroll(pScroll, &scroll);
			GetScrollBarColors(m_clrCurrent, nHitSelected, nHitPressed, scroll.bEnabled);
		}
		else
		{
			XTPScrollMetricsH scroll;
			CalcHScroll(pScroll, &scroll);
			GetScrollBarColors(m_clrCurrent, nHitSelected, nHitPressed, scroll.bEnabled);
		}
	}

	CXTPScrollBarPaintManager::DrawScrollBar(pDC, pScroll);
}

void CXTPScrollBarThemeOffice2013::DrawBackground(CDC* pDC, CXTPScrollBase* pScroll,
												  XTPScrollMetricsV* pMetrics)
{
	ASSERT(pDC);
	ASSERT(NULL != pScroll);
	ASSERT(NULL != pMetrics);

	UNREFERENCED_PARAMETER(pScroll);

	pDC->FillSolidRect(pMetrics->rcScrollBar, m_clrCurrent.clrBack);
}

void CXTPScrollBarThemeOffice2013::DrawButtons(CDC* pDC, CXTPScrollBase* pScroll,
											   XTPScrollMetricsV* pMetrics)
{
	ASSERT(pDC);
	ASSERT(NULL != pScroll);
	ASSERT(NULL != pMetrics);

	UNREFERENCED_PARAMETER(pScroll);

	XTPDrawHelpers()->SolidRectangle(pDC, pMetrics->rcArrowUp, m_clrCurrent.clrScrollBorder,
									 m_clrCurrent.clrScrollBack);
	XTPDrawHelpers()->SolidRectangle(pDC, pMetrics->rcArrowDown, m_clrCurrent.clrScrollDownBorder,
									 m_clrCurrent.clrScrollDownBack);
}

void CXTPScrollBarThemeOffice2013::DrawArrows(CDC* pDC, CXTPScrollBase* pScroll,
											  XTPScrollMetricsV* pMetrics)
{
	ASSERT(pDC);
	ASSERT(NULL != pScroll);
	ASSERT(NULL != pMetrics);

	UNREFERENCED_PARAMETER(pScroll);

	DrawArrowGlyph(pDC, pMetrics->rcArrowUp, xtpArrowGlyphUp, pMetrics->bEnabled,
				   m_clrCurrent.clrScrollArrow);
	DrawArrowGlyph(pDC, pMetrics->rcArrowDown, xtpArrowGlyphDown, pMetrics->bEnabled,
				   m_clrCurrent.clrScrollDownArrow);
}

void CXTPScrollBarThemeOffice2013::DrawThumbButton(CDC* pDC, CXTPScrollBase* pScroll,
												   XTPScrollMetricsV* pMetrics)
{
	ASSERT(pDC);
	ASSERT(NULL != pScroll);
	ASSERT(NULL != pMetrics);

	UNREFERENCED_PARAMETER(pScroll);

	if (pMetrics->bEnabled)
	{
		XTPDrawHelpers()->SolidRectangle(pDC, pMetrics->rcBtnTrack, m_clrCurrent.clrThumbBorder,
										 m_clrCurrent.clrThumbBack);
	}
}

void CXTPScrollBarThemeOffice2013::DrawBackground(CDC* pDC, CXTPScrollBase* pScroll,
												  XTPScrollMetricsH* pMetrics)
{
	ASSERT(pDC);
	ASSERT(NULL != pScroll);
	ASSERT(NULL != pMetrics);

	UNREFERENCED_PARAMETER(pScroll);

	pDC->FillSolidRect(pMetrics->rcScrollBar, m_clrCurrent.clrBack);
}

void CXTPScrollBarThemeOffice2013::DrawButtons(CDC* pDC, CXTPScrollBase* pScroll,
											   XTPScrollMetricsH* pMetrics)
{
	ASSERT(pDC);
	ASSERT(NULL != pScroll);
	ASSERT(NULL != pMetrics);

	UNREFERENCED_PARAMETER(pScroll);

	XTPDrawHelpers()->SolidRectangle(pDC, pMetrics->rcArrowLeft, m_clrCurrent.clrScrollBorder,
									 m_clrCurrent.clrScrollBack);
	XTPDrawHelpers()->SolidRectangle(pDC, pMetrics->rcArrowRight, m_clrCurrent.clrScrollDownBorder,
									 m_clrCurrent.clrScrollDownBack);
}

void CXTPScrollBarThemeOffice2013::DrawArrows(CDC* pDC, CXTPScrollBase* pScroll,
											  XTPScrollMetricsH* pMetrics)
{
	ASSERT(pDC);
	ASSERT(NULL != pScroll);
	ASSERT(NULL != pMetrics);

	UNREFERENCED_PARAMETER(pScroll);

	DrawArrowGlyph(pDC, pMetrics->rcArrowLeft, xtpArrowGlyphLeft, pMetrics->bEnabled,
				   m_clrCurrent.clrScrollArrow);
	DrawArrowGlyph(pDC, pMetrics->rcArrowRight, xtpArrowGlyphRight, pMetrics->bEnabled,
				   m_clrCurrent.clrScrollDownArrow);
}

void CXTPScrollBarThemeOffice2013::DrawThumbButton(CDC* pDC, CXTPScrollBase* pScroll,
												   XTPScrollMetricsH* pMetrics)
{
	ASSERT(pDC);
	ASSERT(NULL != pScroll);
	ASSERT(NULL != pMetrics);

	UNREFERENCED_PARAMETER(pScroll);

	if (pMetrics->bEnabled)
	{
		XTPDrawHelpers()->SolidRectangle(pDC, pMetrics->rcBtnTrack, m_clrCurrent.clrThumbBorder,
										 m_clrCurrent.clrThumbBack);
	}
}

void CXTPScrollBarThemeOffice2013::DrawGripper(CDC* pDC, CXTPScrollBase* pScroll)
{
	ASSERT(pDC);
	ASSERT(NULL != pScroll);

	XTP_SCROLLBAR_POSINFO* pSBInfo = pScroll->GetScrollBarPosInfo();
	pDC->FillSolidRect(&pSBInfo->rc, m_clrNormal.clrBack);
}
