// XTPScrollBarThemeResource.cpp
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
#include "Common/XTPResourceImage.h"
#include "Common/XTPDrawHelpers.h"
#include "Common/XTPWinThemeWrapper.h"
#include "Common/ScrollBar/XTPScrollBase.h"
#include "Common/ScrollBar/XTPScrollBarPaintManager.h"
#include "Common/ScrollBar/Themes/XTPScrollBarThemeResource.h"

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

#ifdef _DEBUG
#	define new DEBUG_NEW
#	undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CXTPResourceImage* CXTPScrollBarThemeResource::LoadImage(LPCTSTR lpszImageFile)
{
	CXTPResourceImages* pImages = XTPResourceImages();
	return pImages->LoadFile(lpszImageFile);
}

#define GETPARTSTATE2(ht)                                                                          \
	(!m_State.bEnabled                                                                             \
		 ? 0                                                                                       \
		 : m_State.nPressedHt == (ht)                                                              \
			   ? 3                                                                                 \
			   : m_State.nHotHt == (ht) ? 2                                                        \
										: m_State.nHotHt > 0 || m_State.nPressedHt > 0 ? 1 : 0)

void CXTPScrollBarThemeResource::DrawScrollBar(CDC* pDC, CXTPScrollBase* pScroll)
{
	if (!XTPResourceImages()->IsValid())
		return;

	XTP_SCROLLBAR_POSINFO* pSBInfo = pScroll->GetScrollBarPosInfo();
	if (!pSBInfo->fSizebox)
	{
		XTP_SCROLLBAR_TRACKINFO* pSBTrack = pScroll->GetScrollBarTrackInfo();

		int cWidth = (pSBInfo->pxRight - pSBInfo->pxLeft);
		if (cWidth <= 0)
		{
			return;
		}

		m_State.nPressedHt = pSBTrack ? pSBInfo->ht : -1;
		m_State.nHotHt	 = pSBTrack ? -1 : pSBInfo->ht;
		m_State.bLight	 = (pScroll->GetScrollBarStyle() != xtpScrollStyleOffice2007Dark);
		m_State.bEnabled   = (pSBInfo->posMax - pSBInfo->posMin - pSBInfo->page + 1 > 0)
						   && pScroll->IsScrollBarEnabled();
		m_State.nBtnTrackSize	 = pSBInfo->pxThumbBottom - pSBInfo->pxThumbTop;
		m_State.nBtnTrackPos	  = pSBInfo->pxThumbTop - pSBInfo->pxUpArrow;
		m_State.rcArrowGripperSrc = CRect(0, 0, 9, 9);
		m_State.rcScroll		  = pSBInfo->rc;
		m_State.pImageArrowGlyphs = NULL;
		m_State.pImageBackground  = NULL;
		if (!m_State.bEnabled || pSBInfo->pxThumbBottom > pSBInfo->pxDownArrow)
		{
			m_State.nBtnTrackPos  = 0;
			m_State.nBtnTrackSize = 0;
		}

		if (!m_State.bLight)
		{
			m_State.pImageArrowGlyphs = LoadImage(_T("CONTROLGALLERYSCROLLARROWGLYPHSDARK"));
		}
		if (NULL == m_State.pImageArrowGlyphs)
		{
			m_State.pImageArrowGlyphs = LoadImage(_T("CONTROLGALLERYSCROLLARROWGLYPHS"));
		}
		if (NULL == m_State.pImageArrowGlyphs)
			return;

		if (pSBInfo->fVert)
		{
			m_State.pImageBackground = LoadImage(m_State.bLight
													 ? _T("CONTROLGALLERYSCROLLVERTICALLIGHT")
													 : _T("CONTROLGALLERYSCROLLVERTICALDARK"));
			if (NULL == m_State.pImageBackground)
				return;

			m_State.rcScroll.DeflateRect(1, 0);
			m_State.rcArrowUp   = CRect(m_State.rcScroll.left, m_State.rcScroll.top,
										m_State.rcScroll.right, pSBInfo->pxUpArrow);
			m_State.rcArrowDown = CRect(m_State.rcScroll.left, pSBInfo->pxDownArrow,
										m_State.rcScroll.right, m_State.rcScroll.bottom);
		}
		else
		{
			m_State.pImageBackground = LoadImage(m_State.bLight
													 ? _T("CONTROLGALLERYSCROLLHORIZONTALLIGHT")
													 : _T("CONTROLGALLERYSCROLLHORIZONTALDARK"));
			if (NULL == m_State.pImageBackground)
				return;

			m_State.rcScroll.DeflateRect(0, XTP_DPI_Y(1));
			m_State.rcArrowLeft  = CRect(m_State.rcScroll.left, m_State.rcScroll.top,
										 pSBInfo->pxUpArrow, m_State.rcScroll.bottom);
			m_State.rcArrowRight = CRect(pSBInfo->pxDownArrow, m_State.rcScroll.top,
										 m_State.rcScroll.right, m_State.rcScroll.bottom);
		}
	}

	CXTPScrollBarPaintManager::DrawScrollBar(pDC, pScroll);
}

void CXTPScrollBarThemeResource::DrawBackground(CDC* pDC, CXTPScrollBase* pScroll,
												XTPScrollMetricsV* pMetrics)
{
	ASSERT(pDC);
	ASSERT(NULL != pScroll);
	ASSERT(NULL != pMetrics);

	UNREFERENCED_PARAMETER(pMetrics);

	XTP_SCROLLBAR_POSINFO* pSBInfo = pScroll->GetScrollBarPosInfo();
	m_State.pImageBackground->DrawImage(pDC, pSBInfo->rc, m_State.pImageBackground->GetSource(0, 2),
										CRect(1, 0, 1, 0));
}

void CXTPScrollBarThemeResource::DrawButtons(CDC* pDC, CXTPScrollBase* pScroll,
											 XTPScrollMetricsV* pMetrics)
{
	ASSERT(pDC);
	ASSERT(NULL != pScroll);
	ASSERT(NULL != pMetrics);

	UNREFERENCED_PARAMETER(pScroll);
	UNREFERENCED_PARAMETER(pMetrics);

	CXTPResourceImage* pImage = LoadImage(m_State.bLight
											  ? _T("CONTROLGALLERYSCROLLARROWSVERTICALLIGHT")
											  : _T("CONTROLGALLERYSCROLLARROWSVERTICALDARK"));
	if (NULL != pImage)
	{
		pImage->DrawImage(pDC, m_State.rcArrowUp,
						  pImage->GetSource(GETPARTSTATE2(XTP_HTSCROLLUP), 4), CRect(3, 3, 3, 3),
						  0xFF00FF);
		pImage->DrawImage(pDC, m_State.rcArrowDown,
						  pImage->GetSource(GETPARTSTATE2(XTP_HTSCROLLDOWN), 4), CRect(3, 3, 3, 3),
						  0xFF00FF);
	}
}

void CXTPScrollBarThemeResource::DrawArrows(CDC* pDC, CXTPScrollBase* pScroll,
											XTPScrollMetricsV* pMetrics)
{
	ASSERT(pDC);
	ASSERT(NULL != pScroll);
	ASSERT(NULL != pMetrics);

	UNREFERENCED_PARAMETER(pScroll);
	UNREFERENCED_PARAMETER(pMetrics);

	CRect rcArrowUpGripper(
		CPoint((m_State.rcArrowUp.left + m_State.rcArrowUp.right - XTP_DPI_X(9)) / 2,
			   (m_State.rcArrowUp.top + m_State.rcArrowUp.bottom - XTP_DPI_Y(9)) / 2),
		XTP_DPI(CSize(9, 9)));
	m_State.pImageArrowGlyphs->DrawImage(pDC, rcArrowUpGripper,
										 OffsetSourceRect(m_State.rcArrowGripperSrc,
														  !m_State.bEnabled
															  ? ABS_UPDISABLED
															  : GETPARTSTATE2(XTP_HTSCROLLUP) != 0
																	? ABS_UPHOT
																	: ABS_UPNORMAL),
										 CRect(0, 0, 0, 0), RGB(255, 0, 255));

	CRect rcArrowDownGripper(
		CPoint((m_State.rcArrowDown.left + m_State.rcArrowDown.right - XTP_DPI_X(9)) / 2,
			   (m_State.rcArrowDown.top + m_State.rcArrowDown.bottom - XTP_DPI_Y(9)) / 2),
		XTP_DPI(CSize(9, 9)));
	m_State.pImageArrowGlyphs->DrawImage(pDC, rcArrowDownGripper,
										 OffsetSourceRect(m_State.rcArrowGripperSrc,
														  !m_State.bEnabled
															  ? ABS_DOWNDISABLED
															  : GETPARTSTATE2(XTP_HTSCROLLDOWN) != 0
																	? ABS_DOWNHOT
																	: ABS_DOWNNORMAL),
										 CRect(0, 0, 0, 0), RGB(255, 0, 255));
}

void CXTPScrollBarThemeResource::DrawTracker(CDC* pDC, CXTPScrollBase* pScroll,
											 XTPScrollMetricsV* pMetrics)
{
	ASSERT(pDC);
	ASSERT(NULL != pScroll);
	ASSERT(NULL != pMetrics);

	UNREFERENCED_PARAMETER(pScroll);
	UNREFERENCED_PARAMETER(pMetrics);

	CRect rcTrack(m_State.rcScroll.left, m_State.rcArrowUp.bottom, m_State.rcScroll.right,
				  m_State.rcArrowDown.top);

	if (!rcTrack.IsRectEmpty())
	{
		CRect rcLowerTrack(rcTrack.left - XTP_DPI_X(1), rcTrack.top, rcTrack.right + XTP_DPI_X(1),
						   rcTrack.top + m_State.nBtnTrackPos);
		CRect rcBtnTrack(rcTrack.left, rcLowerTrack.bottom, rcTrack.right,
						 rcLowerTrack.bottom + m_State.nBtnTrackSize);
		CRect rcUpperTrack(rcTrack.left - XTP_DPI_X(1), rcBtnTrack.bottom,
						   rcTrack.right + XTP_DPI_X(1), rcTrack.bottom);

		if (!rcLowerTrack.IsRectEmpty() && (GETPARTSTATE2(XTP_HTSCROLLUPPAGE) == 3))
		{
			m_State.pImageBackground->DrawImage(pDC, rcLowerTrack,
												m_State.pImageBackground->GetSource(1, 2),
												CRect(1, 0, 1, 0));
		}

		if (!rcUpperTrack.IsRectEmpty() && (GETPARTSTATE2(XTP_HTSCROLLDOWNPAGE) == 3))
		{
			m_State.pImageBackground->DrawImage(pDC, rcUpperTrack,
												m_State.pImageBackground->GetSource(1, 2),
												CRect(1, 0, 1, 0));
		}
	}
}

void CXTPScrollBarThemeResource::DrawThumbButton(CDC* pDC, CXTPScrollBase* pScroll,
												 XTPScrollMetricsV* pMetrics)
{
	ASSERT(pDC);
	ASSERT(NULL != pScroll);
	ASSERT(NULL != pMetrics);

	UNREFERENCED_PARAMETER(pScroll);
	UNREFERENCED_PARAMETER(pMetrics);

	CRect rcTrack(m_State.rcScroll.left, m_State.rcArrowUp.bottom, m_State.rcScroll.right,
				  m_State.rcArrowDown.top);
	if (!rcTrack.IsRectEmpty())
	{
		CRect rcLowerTrack(rcTrack.left - XTP_DPI_X(1), rcTrack.top, rcTrack.right + XTP_DPI_X(1),
						   rcTrack.top + m_State.nBtnTrackPos);
		CRect rcBtnTrack(rcTrack.left, rcLowerTrack.bottom, rcTrack.right,
						 rcLowerTrack.bottom + m_State.nBtnTrackSize);

		if (!rcBtnTrack.IsRectEmpty())
		{
			int nState = GETPARTSTATE2(XTP_HTSCROLLTHUMB);
			if (nState > 0)
				nState--;

			CXTPResourceImage* pImage = NULL;
			if (!m_State.bLight)
			{
				pImage = LoadImage(_T("CONTROLGALLERYSCROLLTHUMBVERTICALDARK"));

				if (!pImage)
					pImage = LoadImage(_T("CONTROLGALLERYSCROLLTHUMBVERTICAL"));
			}
			else
			{
				pImage = LoadImage(_T("CONTROLGALLERYSCROLLTHUMBVERTICAL"));
			}
			if (NULL == pImage)
				return;

			pImage->DrawImage(pDC, rcBtnTrack, pImage->GetSource(nState, 3), CRect(5, 5, 5, 5));

			if (rcBtnTrack.Height() > XTP_DPI_Y(10))
			{
				pImage = LoadImage(_T("CONTROLGALLERYSCROLLTHUMBGRIPPERVERTICAL"));
				CRect rcGripper(CPoint(rcBtnTrack.CenterPoint().x - XTP_DPI_X(4),
									   rcBtnTrack.CenterPoint().y - XTP_DPI_Y(4)),
								XTP_DPI(CSize(8, 8)));

				pImage->DrawImage(pDC, rcGripper, pImage->GetSource(nState, 3), CRect(0, 0, 0, 0));
			}
		}
	}
}

void CXTPScrollBarThemeResource::DrawBackground(CDC* pDC, CXTPScrollBase* pScroll,
												XTPScrollMetricsH* pMetrics)
{
	ASSERT(pDC);
	ASSERT(NULL != pScroll);
	ASSERT(NULL != pMetrics);

	UNREFERENCED_PARAMETER(pMetrics);

	XTP_SCROLLBAR_POSINFO* pSBInfo = pScroll->GetScrollBarPosInfo();
	m_State.pImageBackground->DrawImage(pDC, pSBInfo->rc, m_State.pImageBackground->GetSource(0, 2),
										CRect(0, 1, 0, 1));
}

void CXTPScrollBarThemeResource::DrawButtons(CDC* pDC, CXTPScrollBase* pScroll,
											 XTPScrollMetricsH* pMetrics)
{
	ASSERT(pDC);
	ASSERT(NULL != pScroll);
	ASSERT(NULL != pMetrics);

	UNREFERENCED_PARAMETER(pScroll);
	UNREFERENCED_PARAMETER(pMetrics);

	CXTPResourceImage* pImage = LoadImage(m_State.bLight
											  ? _T("CONTROLGALLERYSCROLLARROWSHORIZONTALLIGHT")
											  : _T("CONTROLGALLERYSCROLLARROWSHORIZONTALDARK"));
	if (NULL != pImage)
	{
		pImage->DrawImage(pDC, m_State.rcArrowLeft,
						  pImage->GetSource(GETPARTSTATE2(XTP_HTSCROLLUP), 4), CRect(3, 3, 3, 3),
						  0xFF00FF);
		pImage->DrawImage(pDC, m_State.rcArrowRight,
						  pImage->GetSource(GETPARTSTATE2(XTP_HTSCROLLDOWN), 4), CRect(3, 3, 3, 3),
						  0xFF00FF);
	}
}

void CXTPScrollBarThemeResource::DrawArrows(CDC* pDC, CXTPScrollBase* pScroll,
											XTPScrollMetricsH* pMetrics)
{
	ASSERT(pDC);
	ASSERT(NULL != pScroll);
	ASSERT(NULL != pMetrics);

	UNREFERENCED_PARAMETER(pScroll);
	UNREFERENCED_PARAMETER(pMetrics);

	CRect rcArrowLeftGripper(
		CPoint((m_State.rcArrowLeft.left + m_State.rcArrowLeft.right - XTP_DPI_X(9)) / 2,
			   (m_State.rcArrowLeft.top + m_State.rcArrowLeft.bottom - XTP_DPI_Y(9)) / 2),
		XTP_DPI(CSize(9, 9)));
	m_State.pImageArrowGlyphs->DrawImage(pDC, rcArrowLeftGripper,
										 OffsetSourceRect(m_State.rcArrowGripperSrc,
														  !m_State.bEnabled
															  ? ABS_LEFTDISABLED
															  : GETPARTSTATE2(XTP_HTSCROLLUP) != 0
																	? ABS_LEFTHOT
																	: ABS_LEFTNORMAL),
										 CRect(0, 0, 0, 0), RGB(255, 0, 255));

	CRect rcArrowRightGripper(
		CPoint((m_State.rcArrowRight.left + m_State.rcArrowRight.right - XTP_DPI_X(9)) / 2,
			   (m_State.rcArrowRight.top + m_State.rcArrowRight.bottom - XTP_DPI_Y(9)) / 2),
		XTP_DPI(CSize(9, 9)));
	m_State.pImageArrowGlyphs->DrawImage(pDC, rcArrowRightGripper,
										 OffsetSourceRect(m_State.rcArrowGripperSrc,
														  !m_State.bEnabled
															  ? ABS_RIGHTDISABLED
															  : GETPARTSTATE2(XTP_HTSCROLLDOWN) != 0
																	? ABS_RIGHTHOT
																	: ABS_RIGHTNORMAL),
										 CRect(0, 0, 0, 0), RGB(255, 0, 255));
}

void CXTPScrollBarThemeResource::DrawTracker(CDC* pDC, CXTPScrollBase* pScroll,
											 XTPScrollMetricsH* pMetrics)
{
	ASSERT(pDC);
	ASSERT(NULL != pScroll);
	ASSERT(NULL != pMetrics);

	UNREFERENCED_PARAMETER(pScroll);
	UNREFERENCED_PARAMETER(pMetrics);

	CRect rcTrack(m_State.rcArrowLeft.right, m_State.rcScroll.top, m_State.rcArrowRight.left,
				  m_State.rcScroll.bottom);

	if (!rcTrack.IsRectEmpty())
	{
		CRect rcLowerTrack(rcTrack.left, rcTrack.top - XTP_DPI_Y(1),
						   rcTrack.left + m_State.nBtnTrackPos, rcTrack.bottom + XTP_DPI_Y(1));
		CRect rcBtnTrack(rcTrack.left, rcLowerTrack.bottom, rcTrack.right,
						 rcLowerTrack.bottom + m_State.nBtnTrackSize);
		CRect rcUpperTrack(rcBtnTrack.right, rcTrack.top - XTP_DPI_Y(1), rcTrack.right,
						   rcTrack.bottom + XTP_DPI_Y(1));

		if (!rcLowerTrack.IsRectEmpty() && (GETPARTSTATE2(XTP_HTSCROLLUPPAGE) == 3))
		{
			m_State.pImageBackground->DrawImage(pDC, rcLowerTrack,
												m_State.pImageBackground->GetSource(1, 2),
												CRect(0, 1, 0, 1));
		}

		if (!rcUpperTrack.IsRectEmpty() && (GETPARTSTATE2(XTP_HTSCROLLDOWNPAGE) == 3))
		{
			m_State.pImageBackground->DrawImage(pDC, rcUpperTrack,
												m_State.pImageBackground->GetSource(1, 2),
												CRect(0, 1, 0, 1));
		}
	}
}

void CXTPScrollBarThemeResource::DrawThumbButton(CDC* pDC, CXTPScrollBase* pScroll,
												 XTPScrollMetricsH* pMetrics)
{
	ASSERT(pDC);
	ASSERT(NULL != pScroll);
	ASSERT(NULL != pMetrics);

	UNREFERENCED_PARAMETER(pScroll);
	UNREFERENCED_PARAMETER(pMetrics);

	CRect rcTrack(m_State.rcArrowLeft.right, m_State.rcScroll.top, m_State.rcArrowRight.left,
				  m_State.rcScroll.bottom);

	if (!rcTrack.IsRectEmpty())
	{
		CRect rcLowerTrack(rcTrack.left, rcTrack.top - XTP_DPI_Y(1),
						   rcTrack.left + m_State.nBtnTrackPos, rcTrack.bottom + XTP_DPI_Y(1));
		CRect rcBtnTrack(rcLowerTrack.right, rcTrack.top,
						 rcLowerTrack.right + m_State.nBtnTrackSize, rcTrack.bottom);

		if (!rcBtnTrack.IsRectEmpty())
		{
			int nState = GETPARTSTATE2(XTP_HTSCROLLTHUMB);
			if (nState > 0)
				nState--;

			CXTPResourceImage* pImage = NULL;
			if (!m_State.bLight)
			{
				pImage = LoadImage(_T("CONTROLGALLERYSCROLLTHUMBHORIZONTALDARK"));

				if (!pImage)
					pImage = LoadImage(_T("CONTROLGALLERYSCROLLTHUMBHORIZONTAL"));
			}
			else
			{
				pImage = LoadImage(_T("CONTROLGALLERYSCROLLTHUMBHORIZONTAL"));
			}
			if (NULL == pImage)
				return;

			pImage->DrawImage(pDC, rcBtnTrack, pImage->GetSource(nState, 3), CRect(5, 5, 5, 5));

			if (rcBtnTrack.Width() > XTP_DPI_X(10))
			{
				pImage = LoadImage(_T("CONTROLGALLERYSCROLLTHUMBGRIPPERHORIZONTAL"));
				CRect rcGripper(CPoint(rcBtnTrack.CenterPoint().x - XTP_DPI_X(3),
									   rcBtnTrack.CenterPoint().y - XTP_DPI_Y(4)),
								XTP_DPI(CSize(8, 8)));

				pImage->DrawImage(pDC, rcGripper, pImage->GetSource(nState, 3), CRect(0, 0, 0, 0));
			}
		}
	}
}

void CXTPScrollBarThemeResource::DrawGripper(CDC* pDC, CXTPScrollBase* pScroll)
{
	ASSERT(pDC);
	ASSERT(NULL != pScroll);

	XTP_SCROLLBAR_POSINFO* pSBInfo = pScroll->GetScrollBarPosInfo();

	CXTPResourceImage* pSizeboxBackground = LoadImage(m_State.bLight
														  ? _T("CONTROLGALLERYSCROLLSIZEBOXLIGHT")
														  : _T("CONTROLGALLERYSCROLLSIZEBOXDARK"));
	ASSERT(NULL != pSizeboxBackground);
	if (NULL != pSizeboxBackground)
	{
		pSizeboxBackground->DrawImage(pDC, pSBInfo->rc, pSizeboxBackground->GetSource());

		CXTPResourceImage* pImageGripper = LoadImage(_T("STATUSBARGRIPPER"));
		if (NULL != pImageGripper)
		{
			CRect rcSrc(0, 0, pImageGripper->GetWidth(), pImageGripper->GetHeight());
			int cxDst = pSBInfo->rc.right - pSBInfo->rc.left;
			int cyDst = pSBInfo->rc.bottom - pSBInfo->rc.top;
			CRect rcDst(cxDst - rcSrc.Width(), cyDst - rcSrc.Height(), cxDst, cyDst);
			pImageGripper->DrawImage(pDC, rcDst, rcSrc, CRect(0, 0, 0, 0), RGB(0xFF, 0, 0xFF));
		}
	}
}
