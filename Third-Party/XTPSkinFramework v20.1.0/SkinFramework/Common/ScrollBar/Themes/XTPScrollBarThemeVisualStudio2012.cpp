// XTPScrollBarThemeVisualStudio2012.cpp
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
#include "Common/ScrollBar/XTPScrollBase.h"
#include "Common/ScrollBar/XTPScrollBarPaintManager.h"
#include "Common/ScrollBar/Themes/XTPScrollBarThemeVisualStudio2012.h"

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

#ifdef _DEBUG
#	define new DEBUG_NEW
#	undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define XTP_SBVS2012_MARGIN XTP_DPI_X(5)

#define GETPARTSTATE(ht, pressed, hot, normal, disabled)                                           \
	(!m_State.bEnabled			? disabled                                                         \
	 : m_State.nPressedHt == ht ? pressed                                                          \
	 : m_State.nHotHt == ht		? hot                                                              \
								: normal)

void CXTPScrollBarThemeVisualStudio2012::DrawScrollBar(CDC* pDC, CXTPScrollBase* pScroll)
{
	ASSERT(NULL != pScroll);

	XTP_SCROLLBAR_TRACKINFO* pSBTrack = pScroll->GetScrollBarTrackInfo();
	XTP_SCROLLBAR_POSINFO* pSBInfo	  = pScroll->GetScrollBarPosInfo();

	int cWidth = (pSBInfo->pxRight - pSBInfo->pxLeft);
	if (cWidth <= 0)
	{
		return;
	}

	m_State.nPressedHt = pSBTrack ? (pSBTrack->bTrackThumb || pSBTrack->fHitOld ? pSBInfo->ht : -1)
								  : -1;
	m_State.nHotHt	   = pSBTrack ? -1 : pSBInfo->ht;
	m_State.bEnabled   = (pSBInfo->posMax - pSBInfo->posMin - pSBInfo->page + 1 > 0)
					   && pScroll->IsScrollBarEnabled();
	m_State.nBtnTrackSize = pSBInfo->pxThumbBottom - pSBInfo->pxThumbTop;
	m_State.nBtnTrackPos  = pSBInfo->pxThumbTop - pSBInfo->pxUpArrow;

	if (!m_State.bEnabled || pSBInfo->pxThumbBottom > pSBInfo->pxDownArrow)
		m_State.nBtnTrackPos = m_State.nBtnTrackSize = 0;

	switch (pScroll->GetScrollBarStyle())
	{
		case xtpScrollStyleVisualStudio2012Dark:
		{
			m_State.clrBackground = RGB(62, 62, 66);

			m_State.clrArrowDisabled = RGB(85, 85, 88);
			m_State.clrArrowEnabled	 = RGB(153, 153, 153);
			m_State.clrArrowHot		 = RGB(28, 151, 234);
			m_State.clrArrowPressed	 = RGB(0, 122, 204);

			m_State.clrBtnTrackEnabled = RGB(104, 104, 104);
			m_State.clrBtnTrackHot	   = RGB(158, 158, 158);
			m_State.clrBtnTrackPressed = RGB(239, 235, 239);

			break;
		}

		case xtpScrollStyleVisualStudio2012Light:
		case xtpScrollStyleDefault:
		case xtpScrollStyleStandard:
		case xtpScrollStyleFlat:
		case xtpScrollStyleSystem:
		case xtpScrollStyleOffice:
		case xtpScrollStyleOffice2007Light:
		case xtpScrollStyleOffice2007Dark:
		case xtpScrollStyleSlider:
		case xtpScrollStyleVisualStudio2015:
		default:
		{
			m_State.clrBackground = RGB(232, 232, 236);

			m_State.clrArrowDisabled = RGB(202, 203, 211);
			m_State.clrArrowEnabled	 = RGB(134, 137, 153);
			m_State.clrArrowHot		 = RGB(28, 151, 234);
			m_State.clrArrowPressed	 = RGB(0, 122, 204);

			m_State.clrBtnTrackEnabled = RGB(208, 209, 215);
			m_State.clrBtnTrackHot	   = RGB(136, 136, 136);
			m_State.clrBtnTrackPressed = RGB(106, 106, 106);

			break;
		}
	}

	CXTPScrollBarPaintManager::DrawScrollBar(pDC, pScroll);
}

void CXTPScrollBarThemeVisualStudio2012::DrawBackground(CDC* pDC, CXTPScrollBase* pScroll,
														XTPScrollMetricsV* pMetrics)
{
	ASSERT(pDC);
	ASSERT(NULL != pScroll);
	ASSERT(NULL != pMetrics);

	UNREFERENCED_PARAMETER(pScroll);

	pDC->FillSolidRect(pMetrics->rcTrack, m_State.clrBackground);
}

void CXTPScrollBarThemeVisualStudio2012::DrawButtons(CDC* pDC, CXTPScrollBase* pScroll,
													 XTPScrollMetricsV* pMetrics)
{
	ASSERT(pDC);
	ASSERT(NULL != pScroll);
	ASSERT(NULL != pMetrics);

	UNREFERENCED_PARAMETER(pScroll);

	pDC->FillSolidRect(pMetrics->rcArrowUp, m_State.clrBackground);
	pDC->FillSolidRect(pMetrics->rcArrowDown, m_State.clrBackground);
}

void CXTPScrollBarThemeVisualStudio2012::DrawArrows(CDC* pDC, CXTPScrollBase* pScroll,
													XTPScrollMetricsV* pMetrics)
{
	ASSERT(pDC);
	ASSERT(NULL != pScroll);
	ASSERT(NULL != pMetrics);

	UNREFERENCED_PARAMETER(pScroll);

	if (m_State.bEnabled)
	{
		COLORREF clrText;

		// drawing up button
		clrText = m_State.clrArrowEnabled;
		if (m_State.nHotHt == XTP_HTSCROLLUP)
			clrText = m_State.clrArrowHot;
		if (m_State.bEnabled && m_State.nPressedHt == XTP_HTSCROLLUP)
			clrText = m_State.clrArrowPressed;
		DrawArrowGlyph(pDC, pMetrics->rcArrowUp, xtpArrowGlyphUp, m_State.bEnabled, clrText);

		// drawing down button
		clrText = m_State.clrArrowEnabled;
		if (m_State.nHotHt == XTP_HTSCROLLDOWN)
			clrText = m_State.clrArrowHot;
		if (m_State.bEnabled && m_State.nPressedHt == XTP_HTSCROLLDOWN)
			clrText = m_State.clrArrowPressed;
		DrawArrowGlyph(pDC, pMetrics->rcArrowDown, xtpArrowGlyphDown, m_State.bEnabled, clrText);
	}
	else
	{
		DrawArrowGlyph(pDC, pMetrics->rcArrowUp, xtpArrowGlyphUp, m_State.bEnabled,
					   m_State.clrArrowDisabled);
		DrawArrowGlyph(pDC, pMetrics->rcArrowDown, xtpArrowGlyphDown, m_State.bEnabled,
					   m_State.clrArrowDisabled);
	}
}

void CXTPScrollBarThemeVisualStudio2012::DrawThumbButton(CDC* pDC, CXTPScrollBase* pScroll,
														 XTPScrollMetricsV* pMetrics)
{
	ASSERT(pDC);
	ASSERT(NULL != pScroll);
	ASSERT(NULL != pMetrics);

	UNREFERENCED_PARAMETER(pScroll);

	pMetrics->rcBtnTrack.DeflateRect(XTP_SBVS2012_MARGIN, 0, XTP_SBVS2012_MARGIN, 0);
	pDC->FillSolidRect(pMetrics->rcBtnTrack,
					   m_State.nPressedHt == XTP_HTSCROLLTHUMB ? m_State.clrBtnTrackPressed
					   : m_State.nHotHt == XTP_HTSCROLLTHUMB   ? m_State.clrBtnTrackHot
															   : m_State.clrBtnTrackEnabled);
}

void CXTPScrollBarThemeVisualStudio2012::DrawBackground(CDC* pDC, CXTPScrollBase* pScroll,
														XTPScrollMetricsH* pMetrics)
{
	ASSERT(pDC);
	ASSERT(NULL != pScroll);
	ASSERT(NULL != pMetrics);

	UNREFERENCED_PARAMETER(pMetrics);

	XTP_SCROLLBAR_POSINFO* pSBInfo = pScroll->GetScrollBarPosInfo();
	CRect rcArrowLeft(pSBInfo->rc.left, pSBInfo->rc.top, pSBInfo->pxUpArrow, pSBInfo->rc.bottom);
	CRect rcArrowRight(pSBInfo->pxDownArrow, pSBInfo->rc.top, pSBInfo->rc.right,
					   pSBInfo->rc.bottom);

	pDC->FillSolidRect(rcArrowLeft, m_State.clrBackground);
	pDC->FillSolidRect(rcArrowRight, m_State.clrBackground);
}

void CXTPScrollBarThemeVisualStudio2012::DrawButtons(CDC* pDC, CXTPScrollBase* pScroll,
													 XTPScrollMetricsH* pMetrics)
{
	ASSERT(pDC);
	ASSERT(NULL != pScroll);
	ASSERT(NULL != pMetrics);

	UNREFERENCED_PARAMETER(pMetrics);

	XTP_SCROLLBAR_POSINFO* pSBInfo = pScroll->GetScrollBarPosInfo();
	CRect rcArrowLeft(pSBInfo->rc.left, pSBInfo->rc.top, pSBInfo->pxUpArrow, pSBInfo->rc.bottom);
	CRect rcArrowRight(pSBInfo->pxDownArrow, pSBInfo->rc.top, pSBInfo->rc.right,
					   pSBInfo->rc.bottom);
	CRect rcTrack(rcArrowLeft.right, pSBInfo->rc.top, rcArrowRight.left, pSBInfo->rc.bottom);
	pDC->FillSolidRect(rcTrack, m_State.clrBackground);
}

void CXTPScrollBarThemeVisualStudio2012::DrawArrows(CDC* pDC, CXTPScrollBase* pScroll,
													XTPScrollMetricsH* pMetrics)
{
	ASSERT(pDC);
	ASSERT(NULL != pScroll);
	ASSERT(NULL != pMetrics);

	UNREFERENCED_PARAMETER(pMetrics);

	XTP_SCROLLBAR_POSINFO* pSBInfo = pScroll->GetScrollBarPosInfo();
	CRect rcArrowLeft(pSBInfo->rc.left, pSBInfo->rc.top, pSBInfo->pxUpArrow, pSBInfo->rc.bottom);
	CRect rcArrowRight(pSBInfo->pxDownArrow, pSBInfo->rc.top, pSBInfo->rc.right,
					   pSBInfo->rc.bottom);

	// drawing top and bottom buttons

	if (m_State.bEnabled)
	{
		COLORREF clrText;

		// drawing up button
		clrText = m_State.clrArrowEnabled;
		if (m_State.nHotHt == XTP_HTSCROLLUP)
			clrText = m_State.clrArrowHot;
		if (m_State.bEnabled && m_State.nPressedHt == XTP_HTSCROLLUP)
			clrText = m_State.clrArrowPressed;
		DrawArrowGlyph(pDC, rcArrowLeft, xtpArrowGlyphLeft, m_State.bEnabled, clrText);

		// drawing down button
		clrText = m_State.clrArrowEnabled;
		if (m_State.nHotHt == XTP_HTSCROLLDOWN)
			clrText = m_State.clrArrowHot;
		if (m_State.bEnabled && m_State.nPressedHt == XTP_HTSCROLLDOWN)
			clrText = m_State.clrArrowPressed;
		DrawArrowGlyph(pDC, rcArrowRight, xtpArrowGlyphRight, m_State.bEnabled, clrText);
	}
	else
	{
		DrawArrowGlyph(pDC, rcArrowLeft, xtpArrowGlyphLeft, m_State.bEnabled,
					   m_State.clrArrowDisabled);
		DrawArrowGlyph(pDC, rcArrowRight, xtpArrowGlyphRight, m_State.bEnabled,
					   m_State.clrArrowDisabled);
	}
}

void CXTPScrollBarThemeVisualStudio2012::DrawThumbButton(CDC* pDC, CXTPScrollBase* pScroll,
														 XTPScrollMetricsH* pMetrics)
{
	ASSERT(pDC);
	ASSERT(NULL != pScroll);
	ASSERT(NULL != pMetrics);

	UNREFERENCED_PARAMETER(pMetrics);

	XTP_SCROLLBAR_POSINFO* pSBInfo = pScroll->GetScrollBarPosInfo();
	CRect rcArrowLeft(pSBInfo->rc.left, pSBInfo->rc.top, pSBInfo->pxUpArrow, pSBInfo->rc.bottom);
	CRect rcArrowRight(pSBInfo->pxDownArrow, pSBInfo->rc.top, pSBInfo->rc.right,
					   pSBInfo->rc.bottom);
	CRect rcTrack(rcArrowLeft.right, pSBInfo->rc.top, rcArrowRight.left, pSBInfo->rc.bottom);
	CRect rcLowerTrack(rcTrack.left, rcTrack.top, rcTrack.left + m_State.nBtnTrackPos,
					   rcTrack.bottom);
	CRect rcBtnTrack(rcLowerTrack.right, rcTrack.top, rcLowerTrack.right + m_State.nBtnTrackSize,
					 rcTrack.bottom);

	rcBtnTrack.DeflateRect(0, XTP_SBVS2012_MARGIN, 0, XTP_SBVS2012_MARGIN);
	pDC->FillSolidRect(rcBtnTrack,
					   m_State.nPressedHt == XTP_HTSCROLLTHUMB ? m_State.clrBtnTrackPressed
					   : m_State.nHotHt == XTP_HTSCROLLTHUMB   ? m_State.clrBtnTrackHot
															   : m_State.clrBtnTrackEnabled);
}

void CXTPScrollBarThemeVisualStudio2012::DrawGripper(CDC* pDC, CXTPScrollBase* pScroll)
{
	ASSERT(pDC);
	ASSERT(NULL != pScroll);

	XTP_SCROLLBAR_POSINFO* pSBInfo = pScroll->GetScrollBarPosInfo();
	pDC->FillSolidRect(&pSBInfo->rc, m_State.clrBackground);
}
