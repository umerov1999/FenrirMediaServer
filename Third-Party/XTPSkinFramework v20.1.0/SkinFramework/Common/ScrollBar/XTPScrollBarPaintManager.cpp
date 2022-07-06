// XTPScrollBarPaintManager.cpp : implementation of the CXTPScrollBarPaintManager class.
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
#include "Common/Base/cxminmax.h"
#include "Common/XTPTypeId.h"
#include "Common/XTPFramework.h"
#include "Common/XTPSystemHelpers.h"
#include "Common/XTPSynchro.h"
#include "Common/XTPApplication.h"
#include "Common/XTPSingleton.h"
#include "Common/XTPGdiObjects.h"
#include "Common/XTPDrawHelpers.h"
#include "Common/XTPColorManager.h"
#include "Common/ScrollBar/XTPScrollBase.h"
#include "Common/ScrollBar/XTPScrollBarPaintManager.h"

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

#ifdef _DEBUG
#	define new DEBUG_NEW
#	undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////
// CXTPScrollBarPaintManager

IMPLEMENT_DYNAMIC(CXTPScrollBarPaintManager, CObject);

CXTPScrollBarPaintManager::CXTPScrollBarPaintManager()
	: m_cxHScroll(0)
	, m_cyHScroll(0)
	, m_cxVScroll(0)
	, m_cyVScroll(0)
{
	m_cThumb = 0;

	RefreshMetrics();
}

void CXTPScrollBarPaintManager::RefreshMetrics()
{
	RefreshXtremeColors(FALSE);
	m_cxHScroll = ::GetSystemMetrics(SM_CXHSCROLL);
	m_cyHScroll = ::GetSystemMetrics(SM_CYHSCROLL);
	m_cxVScroll = ::GetSystemMetrics(SM_CXVSCROLL);
	m_cyVScroll = ::GetSystemMetrics(SM_CYVSCROLL);

	// COLORREF clrWindow = GetXtremeColor(COLOR_WINDOW);
	COLORREF clrFace = GetXtremeColor(COLOR_3DFACE);

	m_crBack		 = clrFace;
	m_crBackHilite	 = clrFace;
	m_crBackPushed	 = clrFace;
	m_crBorderHilite = GetXtremeColor(COLOR_3DSHADOW);
	m_crBorder		 = GetXtremeColor(COLOR_3DSHADOW);
}

void CXTPScrollBarPaintManager::DrawArrowGlyph(CDC* pDC, CRect rcArrow, XTPArrowGlyph glyph,
											   BOOL bEnabled, COLORREF clrText)
{
	int nHeight = CXTP_min(rcArrow.Width(), rcArrow.Height());
	if (XTP_DPI_Y(6) <= nHeight)
	{
		COLORREF clr;
		if (clrText == COLORREF_NULL)
		{
			clr = bEnabled ? RGB(0, 0, 0) : GetSysColor(COLOR_3DSHADOW);
		}
		else
		{
			clr = clrText;
		}

		XTPPrimitiveSymbol nSymbol = xtpPrimitiveSymbolInvalid;
		switch (glyph)
		{
			case xtpArrowGlyphLeft: nSymbol = xtpPrimitiveSymbolPlayLeft; break;
			case xtpArrowGlyphRight: nSymbol = xtpPrimitiveSymbolPlayRight; break;
			case xtpArrowGlyphUp: nSymbol = xtpPrimitiveSymbolPlayUp; break;
			case xtpArrowGlyphDown: nSymbol = xtpPrimitiveSymbolPlayDown; break;
		}

		XTPPrimitiveDrawer()->DrawSymbol(pDC, nSymbol, rcArrow, clr);
	}
}

void CXTPScrollBarPaintManager::CalcVScroll(CXTPScrollBase* pScroll, XTPScrollMetricsV* pVScroll)
{
	XTP_SCROLLBAR_POSINFO* pSBInfo = pScroll->GetScrollBarPosInfo();

	BOOL bEnabled = (pSBInfo->posMax - pSBInfo->posMin - pSBInfo->page + 1 > 0)
					&& pScroll->IsScrollBarEnabled();

	int nBtnTrackSize = pSBInfo->pxThumbBottom - pSBInfo->pxThumbTop;
	int nBtnTrackPos  = pSBInfo->pxThumbTop - pSBInfo->pxUpArrow;

	if (!bEnabled || pSBInfo->pxThumbBottom > pSBInfo->pxDownArrow)
	{
		nBtnTrackPos  = 0;
		nBtnTrackSize = 0;
	}

	CRect rcScrollBar(pSBInfo->rc);
	CRect rcArrowUp(rcScrollBar.left, rcScrollBar.top, rcScrollBar.right, pSBInfo->pxUpArrow);
	CRect rcArrowDown(rcScrollBar.left, pSBInfo->pxDownArrow, rcScrollBar.right,
					  rcScrollBar.bottom);
	CRect rcTrack(rcScrollBar.left, rcArrowUp.bottom, rcScrollBar.right, rcArrowDown.top);
	CRect rcLowerTrack(rcTrack.left, rcTrack.top, rcTrack.right, rcTrack.top + nBtnTrackPos);
	CRect rcBtnTrack(rcTrack.left, rcLowerTrack.bottom, rcTrack.right,
					 rcLowerTrack.bottom + nBtnTrackSize);
	CRect rcUpperTrack(rcTrack.left, rcBtnTrack.bottom, rcTrack.right, rcTrack.bottom);

	pVScroll->bEnabled	   = bEnabled;
	pVScroll->rcScrollBar  = rcScrollBar;
	pVScroll->rcArrowUp	   = rcArrowUp;
	pVScroll->rcArrowDown  = rcArrowDown;
	pVScroll->rcTrack	   = rcTrack;
	pVScroll->rcBtnTrack   = rcBtnTrack;
	pVScroll->rcUpperTrack = rcUpperTrack;
	pVScroll->rcLowerTrack = rcLowerTrack;
}

void CXTPScrollBarPaintManager::CalcHScroll(CXTPScrollBase* pScroll, XTPScrollMetricsH* pHScroll)
{
	XTP_SCROLLBAR_POSINFO* pSBInfo = pScroll->GetScrollBarPosInfo();

	BOOL bEnabled = (pSBInfo->posMax - pSBInfo->posMin - pSBInfo->page + 1 > 0)
					&& pScroll->IsScrollBarEnabled();

	int nBtnTrackSize = pSBInfo->pxThumbBottom - pSBInfo->pxThumbTop;
	int nBtnTrackPos  = pSBInfo->pxThumbTop - pSBInfo->pxUpArrow;

	if (!bEnabled || pSBInfo->pxThumbBottom > pSBInfo->pxDownArrow)
	{
		nBtnTrackPos  = 0;
		nBtnTrackSize = 0;
	}

	CRect rcHScroll(pSBInfo->rc);
	CRect rcArrowLeft(rcHScroll.left, rcHScroll.top, pSBInfo->pxUpArrow, rcHScroll.bottom);
	CRect rcArrowRight(pSBInfo->pxDownArrow, rcHScroll.top, rcHScroll.right, rcHScroll.bottom);
	CRect rcTrack(rcArrowLeft.right, rcHScroll.top, rcArrowRight.left, rcHScroll.bottom);
	CRect rcLowerTrack(rcTrack.left, rcTrack.top, rcTrack.left + nBtnTrackPos, rcTrack.bottom);
	CRect rcBtnTrack(rcLowerTrack.right, rcTrack.top, rcLowerTrack.right + nBtnTrackSize,
					 rcTrack.bottom);
	CRect rcUpperTrack(rcBtnTrack.right, rcTrack.top, rcTrack.right, rcTrack.bottom);

	pHScroll->bEnabled	   = bEnabled;
	pHScroll->rcScrollBar  = rcHScroll;
	pHScroll->rcArrowLeft  = rcArrowLeft;
	pHScroll->rcArrowRight = rcArrowRight;
	pHScroll->rcTrack	   = rcTrack;
	pHScroll->rcLowerTrack = rcLowerTrack;
	pHScroll->rcBtnTrack   = rcBtnTrack;
	pHScroll->rcUpperTrack = rcUpperTrack;
}

void CXTPScrollBarPaintManager::DrawBackground(CDC* pDC, CXTPScrollBase* pScroll,
											   XTPScrollMetricsV* pMetrics)
{
	UNREFERENCED_PARAMETER(pDC);
	UNREFERENCED_PARAMETER(pScroll);
	UNREFERENCED_PARAMETER(pMetrics);
}

void CXTPScrollBarPaintManager::DrawButtons(CDC* pDC, CXTPScrollBase* pScroll,
											XTPScrollMetricsV* pMetrics)
{
	UNREFERENCED_PARAMETER(pDC);
	UNREFERENCED_PARAMETER(pScroll);
	UNREFERENCED_PARAMETER(pMetrics);
}

void CXTPScrollBarPaintManager::DrawArrows(CDC* pDC, CXTPScrollBase* pScroll,
										   XTPScrollMetricsV* pMetrics)
{
	UNREFERENCED_PARAMETER(pDC);
	UNREFERENCED_PARAMETER(pScroll);
	UNREFERENCED_PARAMETER(pMetrics);
}

void CXTPScrollBarPaintManager::DrawTracker(CDC* pDC, CXTPScrollBase* pScroll,
											XTPScrollMetricsV* pMetrics)
{
	UNREFERENCED_PARAMETER(pDC);
	UNREFERENCED_PARAMETER(pScroll);
	UNREFERENCED_PARAMETER(pMetrics);
}

void CXTPScrollBarPaintManager::DrawThumbButton(CDC* pDC, CXTPScrollBase* pScroll,
												XTPScrollMetricsV* pMetrics)
{
	UNREFERENCED_PARAMETER(pDC);
	UNREFERENCED_PARAMETER(pScroll);
	UNREFERENCED_PARAMETER(pMetrics);
}

void CXTPScrollBarPaintManager::DrawBackground(CDC* pDC, CXTPScrollBase* pScroll,
											   XTPScrollMetricsH* pMetrics)
{
	UNREFERENCED_PARAMETER(pDC);
	UNREFERENCED_PARAMETER(pScroll);
	UNREFERENCED_PARAMETER(pMetrics);
}

void CXTPScrollBarPaintManager::DrawButtons(CDC* pDC, CXTPScrollBase* pScroll,
											XTPScrollMetricsH* pMetrics)
{
	UNREFERENCED_PARAMETER(pDC);
	UNREFERENCED_PARAMETER(pScroll);
	UNREFERENCED_PARAMETER(pMetrics);
}

void CXTPScrollBarPaintManager::DrawArrows(CDC* pDC, CXTPScrollBase* pScroll,
										   XTPScrollMetricsH* pMetrics)
{
	UNREFERENCED_PARAMETER(pDC);
	UNREFERENCED_PARAMETER(pScroll);
	UNREFERENCED_PARAMETER(pMetrics);
}

void CXTPScrollBarPaintManager::DrawTracker(CDC* pDC, CXTPScrollBase* pScroll,
											XTPScrollMetricsH* pMetrics)
{
	UNREFERENCED_PARAMETER(pDC);
	UNREFERENCED_PARAMETER(pScroll);
	UNREFERENCED_PARAMETER(pMetrics);
}

void CXTPScrollBarPaintManager::DrawThumbButton(CDC* pDC, CXTPScrollBase* pScroll,
												XTPScrollMetricsH* pMetrics)
{
	UNREFERENCED_PARAMETER(pDC);
	UNREFERENCED_PARAMETER(pScroll);
	UNREFERENCED_PARAMETER(pMetrics);
}

void CXTPScrollBarPaintManager::DrawGripper(CDC* pDC, CXTPScrollBase* pScroll)
{
	UNREFERENCED_PARAMETER(pDC);
	UNREFERENCED_PARAMETER(pScroll);
}

void CXTPScrollBarPaintManager::DrawScrollBar(CDC* pDC, CXTPScrollBase* pScroll)
{
	XTP_SCROLLBAR_POSINFO* pSBInfo = pScroll->GetScrollBarPosInfo();

	if (!pSBInfo->fSizebox)
	{
		if (pSBInfo->fVert)
		{
			XTPScrollMetricsV scroll;
			CalcVScroll(pScroll, &scroll);

			DrawBackground(pDC, pScroll, &scroll);
			DrawButtons(pDC, pScroll, &scroll);
			DrawArrows(pDC, pScroll, &scroll);
			DrawTracker(pDC, pScroll, &scroll);
			DrawThumbButton(pDC, pScroll, &scroll);
		}
		else
		{
			XTPScrollMetricsH scroll;
			CalcHScroll(pScroll, &scroll);

			DrawBackground(pDC, pScroll, &scroll);
			DrawButtons(pDC, pScroll, &scroll);
			DrawArrows(pDC, pScroll, &scroll);
			DrawTracker(pDC, pScroll, &scroll);
			DrawThumbButton(pDC, pScroll, &scroll);
		}
	}
	else
	{
		DrawGripper(pDC, pScroll);
	}
}
