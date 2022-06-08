// XTPMarkupScrollBar.cpp: implementation of the CXTPMarkupScrollBar class.
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

#include "stdafx.h"

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
#include "Common/XTPWinThemeWrapper.h"
#include "Common/Base/Types/XTPSize.h"
#include "Common/ScrollBar/XTPScrollBase.h"

#include "Markup/XTPMarkupObject.h"
#include "Markup/XTPMarkupInputElement.h"
#include "Markup/XTPMarkupUIElement.h"
#include "Markup/XTPMarkupFrameworkElement.h"
#include "Markup/Controls/XTPMarkupControl.h"

#include "Markup/Controls/XTPMarkupContentControl.h"
#include "Markup/Controls/XTPMarkupScrollViewer.h"
#include "Markup/Controls/XTPMarkupScrollBar.h"
#include "Markup/XTPMarkupContext.h"
#include "Markup/XTPMarkupDrawingContext.h"
#include "Markup/DeviceContext/XTPMarkupDeviceContext.h"
#include "Markup/XTPMarkupRoutedEventArgs.h"
#include "Markup/XTPMarkupMouseEventArgs.h"
#include "Markup/XTPMarkupMouseButtonEventArgs.h"

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

#ifdef _DEBUG
#	undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#	define new DEBUG_NEW
#endif

IMPLEMENT_MARKUPCLASS(NULL, CXTPMarkupScrollBar, CXTPMarkupControl)

void CXTPMarkupScrollBar::RegisterMarkupClass()
{
}

CXTPMarkupScrollBar::CXTPMarkupScrollBar()
{
	m_orientation = xtpMarkupOrientationVertical;

	m_pSBTrack = NULL;
	ZeroMemory(&m_spi, sizeof(m_spi));
	ZeroMemory(&m_si, sizeof(m_si));
}

void CXTPMarkupScrollBar::SetScrollInfo(SCROLLINFO* pScrollInfo)
{
	ASSERT(NULL != pScrollInfo);

	if (m_si.nMax != pScrollInfo->nMax || m_si.nPage != pScrollInfo->nPage
		|| m_si.nPos != pScrollInfo->nPos)
	{
		m_si			= *pScrollInfo;
		m_bArrangeDirty = TRUE;
	}
}

void CXTPMarkupScrollBar::GetScrollInfo(SCROLLINFO* pScrollInfo)
{
	ASSERT(NULL != pScrollInfo);
	*pScrollInfo = m_si;
}

int CXTPMarkupScrollBar::HitTestScrollBar(POINT pt) const
{
	int htResult = HTERROR;

	do
	{
		if (!IsEnabled())
		{
			htResult = HTNOWHERE;
			break;
		}

		if (!PtInRectTransform(m_spi.rc, pt))
		{
			htResult = HTNOWHERE;
			break;
		}

		if (m_spi.fVert)
		{
			CRect rcPart(m_spi.rc.left, m_spi.rc.top, m_spi.rc.right,
						 m_spi.rc.top + m_spi.pxUpArrow);
			if (PtInRectTransform(rcPart, pt))
			{
				htResult = XTP_HTSCROLLUP;
				break;
			}

			if (m_spi.pxPopup > 0)
			{
				rcPart.top	  = m_spi.rc.top + m_spi.pxPopup;
				rcPart.bottom = m_spi.rc.bottom;
				if (PtInRectTransform(rcPart, pt))
				{
					htResult = XTP_HTSCROLLPOPUP;
					break;
				}
			}

			rcPart.SetRect(m_spi.rc.left, m_spi.rc.top + m_spi.pxDownArrow, m_spi.rc.right,
						   m_spi.rc.bottom);
			if (PtInRectTransform(rcPart, pt))
			{
				htResult = XTP_HTSCROLLDOWN;
				break;
			}

			rcPart.SetRect(m_spi.rc.left, m_spi.rc.top, m_spi.rc.right,
						   m_spi.rc.top + m_spi.pxThumbTop);
			if (PtInRectTransform(rcPart, pt))
			{
				htResult = XTP_HTSCROLLUPPAGE;
				break;
			}

			rcPart.bottom = m_spi.rc.top + m_spi.pxThumbBottom;
			if (PtInRectTransform(rcPart, pt))
			{
				htResult = XTP_HTSCROLLTHUMB;
				break;
			}

			rcPart.bottom = m_spi.rc.top + m_spi.pxDownArrow;
			if (PtInRectTransform(rcPart, pt))
			{
				htResult = XTP_HTSCROLLDOWNPAGE;
				break;
			}
		}
		else
		{
			CRect rcPart(m_spi.rc.left, m_spi.rc.top, m_spi.rc.left + m_spi.pxUpArrow,
						 m_spi.rc.bottom);
			if (PtInRectTransform(rcPart, pt))
			{
				htResult = XTP_HTSCROLLUP;
				break;
			}

			if (m_spi.pxPopup > 0)
			{
				rcPart.left	 = m_spi.rc.left + m_spi.pxPopup;
				rcPart.right = m_spi.rc.right;
				if (PtInRectTransform(rcPart, pt))
				{
					htResult = XTP_HTSCROLLPOPUP;
					break;
				}
			}

			rcPart.SetRect(m_spi.rc.left + m_spi.pxDownArrow, m_spi.rc.top, m_spi.rc.right,
						   m_spi.rc.bottom);
			if (PtInRectTransform(rcPart, pt))
			{
				htResult = XTP_HTSCROLLDOWN;
				break;
			}

			rcPart.SetRect(m_spi.rc.left, m_spi.rc.top, m_spi.rc.left + m_spi.pxThumbTop,
						   m_spi.rc.bottom);
			if (PtInRectTransform(rcPart, pt))
			{
				htResult = XTP_HTSCROLLUPPAGE;
				break;
			}

			rcPart.right = m_spi.rc.left + m_spi.pxThumbBottom;
			if (PtInRectTransform(rcPart, pt))
			{
				htResult = XTP_HTSCROLLTHUMB;
				break;
			}

			rcPart.right = m_spi.rc.left + m_spi.pxDownArrow;
			if (PtInRectTransform(rcPart, pt))
			{
				htResult = XTP_HTSCROLLDOWNPAGE;
				break;
			}
		}
	} while (FALSE);

	return htResult;
}

//////////////////////////////////////////////////////////////////////////
// Scrolling

void CXTPMarkupScrollBar::EndScroll(BOOL fCancel)
{
	XTP_SCROLLBAR_TRACKINFO* pSBTrack = m_pSBTrack;

	if (pSBTrack)
	{
		pSBTrack->cmdSB = 0;
		GetMarkupContext()->ReleaseMouseCapture(this);

		if (pSBTrack->bTrackThumb)
		{
			if (fCancel)
			{
				pSBTrack->posOld = pSBTrack->pSBInfo->pos;
			}

			DoScroll(SB_THUMBPOSITION, pSBTrack->posOld);

			RedrawScrollBar();
		}
		else
		{
			if (pSBTrack->hTimerSB != 0)
			{
				::KillTimer(pSBTrack->hWndTrack, pSBTrack->hTimerSB);
				pSBTrack->hTimerSB = 0;
			}
		}

		DoScroll(SB_ENDSCROLL, 0);
	}
}

void CXTPMarkupScrollBar::MoveThumb(int px)
{
	XTP_SCROLLBAR_TRACKINFO* pSBTrack = m_pSBTrack;

	if ((pSBTrack == NULL) || (px == pSBTrack->pxOld))
		return;

	XTP_SCROLLBAR_POSINFO* pSBInfo = m_pSBTrack->pSBInfo;

pxReCalc:

	pSBTrack->posNew = SBPosFromPx(pSBInfo, px);

	if (pSBTrack->posNew != pSBTrack->posOld)
	{
		DoScroll(SB_THUMBTRACK, pSBTrack->posNew);

		pSBTrack->posOld = pSBTrack->posNew;

		if (px >= pSBInfo->pxMin + pSBInfo->cpx)
		{
			px = pSBInfo->pxMin + pSBInfo->cpx;
			goto pxReCalc;
		}
	}

	pSBInfo->pxThumbTop	   = px;
	pSBInfo->pxThumbBottom = pSBInfo->pxThumbTop + pSBInfo->cpxThumb;
	pSBTrack->pxOld		   = px;

	RedrawScrollBar();
}

void CXTPMarkupScrollBar::TrackThumb(UINT message, CPoint pt)
{
	XTP_SCROLLBAR_TRACKINFO* pSBTrack = m_pSBTrack;
	if (!pSBTrack)
		return;

	XTP_SCROLLBAR_POSINFO* pSBInfo = pSBTrack->pSBInfo;

	if (HIBYTE(message) != HIBYTE(WM_MOUSEFIRST))
		return;

	if (pSBInfo == NULL)
		return;

	int px;

	if (!PtInRectTransform(pSBTrack->rcTrack, pt))
		px = pSBInfo->pxStart;
	else
	{
		px = (pSBTrack->pSBInfo->fVert ? pt.y : pt.x) + pSBTrack->dpxThumb;
		if (px < pSBInfo->pxMin)
			px = pSBInfo->pxMin;
		else if (px >= pSBInfo->pxMin + pSBInfo->cpx)
			px = pSBInfo->pxMin + pSBInfo->cpx;
	}

	MoveThumb(px);

	if (message == WM_LBUTTONUP || GetKeyState(VK_LBUTTON) >= 0)
	{
		EndScroll(FALSE);
	}
}

void CXTPMarkupScrollBar::TrackBox(UINT message, CPoint point)
{
	XTP_SCROLLBAR_TRACKINFO* pSBTrack = m_pSBTrack;

	if (pSBTrack == NULL)
		return;

	if (message != WM_NULL && HIBYTE(message) != HIBYTE(WM_MOUSEFIRST))
		return;

	if ((pSBTrack->cmdSB == SB_PAGEUP || pSBTrack->cmdSB == SB_PAGEDOWN))
	{
		int* pLength = (int*)&pSBTrack->rcTrack;

		if (pSBTrack->pSBInfo->fVert)
			pLength++;

		if (pSBTrack->cmdSB == SB_PAGEUP)
			pLength[2] = pSBTrack->pSBInfo->pxThumbTop;
		else
			pLength[0] = pSBTrack->pSBInfo->pxThumbBottom;
	}

	BOOL fHit = PtInRectTransform(pSBTrack->rcTrack, point);

	BOOL fHitChanged = fHit != (BOOL)pSBTrack->fHitOld;

	if (fHitChanged)
	{
		pSBTrack->fHitOld = fHit;
		RedrawScrollBar();
	}

	int cmsTimer = XTPToIntChecked(GetDoubleClickTime() / 10);

	switch (message)
	{
		case WM_LBUTTONUP: EndScroll(FALSE); break;

		case WM_LBUTTONDOWN:
			pSBTrack->hTimerSB = 0;
			cmsTimer		   = XTPToIntChecked(GetDoubleClickTime() * 4 / 5);

			/*
			*** FALL THRU **
			*/

		case WM_MOUSEMOVE:
			if (fHit && fHitChanged)
			{
				pSBTrack->hTimerSB = ::SetTimer(m_pSBTrack->hWndTrack, IDSYS_SCROLL,
												XTPToUInt(cmsTimer), NULL);

				DoScroll(XTPToInt(pSBTrack->cmdSB), 0);
			}
	}
}

void CXTPMarkupScrollBar::ContScroll()
{
	XTP_SCROLLBAR_TRACKINFO* pSBTrack = m_pSBTrack;
	ASSERT(pSBTrack);

	if (pSBTrack == NULL)
		return;

	CPoint pt;
	GetCursorPos(&pt);

	ScreenToClient(m_pSBTrack->hWndTrack, &pt);

	TrackBox(WM_NULL, pt);

	if (pSBTrack->fHitOld)
	{
		pSBTrack->hTimerSB = ::SetTimer(m_pSBTrack->hWndTrack, IDSYS_SCROLL,
										GetDoubleClickTime() / 10, NULL);

		DoScroll(XTPToInt(pSBTrack->cmdSB), 0);
	}
}

void CXTPMarkupScrollBar::CalcTrackDragRect(XTP_SCROLLBAR_TRACKINFO* pSBTrack) const
{
	int cx;
	int cy;
	LPINT pwX, pwY;

	pwX = pwY = (LPINT)&pSBTrack->rcTrack;

	if (pSBTrack->pSBInfo->fVert)
	{
		cy = GetSystemMetrics(SM_CYVTHUMB);
		pwY++;
	}
	else
	{
		cy = GetSystemMetrics(SM_CXHTHUMB);
		pwX++;
	}

	cx = (pSBTrack->pSBInfo->pxRight - pSBTrack->pSBInfo->pxLeft) * 8;
	cy *= 2;

	*(pwX + 0) = pSBTrack->pSBInfo->pxLeft - cx;
	*(pwY + 0) = pSBTrack->pSBInfo->pxTop - cy;
	*(pwX + 2) = pSBTrack->pSBInfo->pxRight + cx;
	*(pwY + 2) = pSBTrack->pSBInfo->pxBottom + cy;
}

void CXTPMarkupScrollBar::DoScroll(int cmd, int pos)
{
	CXTPMarkupScrollViewer* pViewer = MARKUP_DYNAMICCAST(CXTPMarkupScrollViewer,
														 GetLogicalParent());
	if (pViewer)
	{
		pViewer->Scroll(m_spi.fVert ? SB_VERT : SB_HORZ, cmd, pos);
	}
}

void CXTPMarkupScrollBar::ScreenToClient(HWND hWnd, LPPOINT lpPoint)
{
	::ScreenToClient(hWnd, lpPoint);

	CXTPMarkupMouseEventArgs e(0);
	e.m_hWnd  = hWnd;
	e.m_point = *lpPoint;

	*lpPoint = e.GetPosition(this);
}

void CXTPMarkupScrollBar::PerformTrackInit(HWND hWnd, CPoint point, XTP_SCROLLBAR_POSINFO* pSBInfo,
										   BOOL bDirect)
{
	CXTPMarkupContext* pMarkupContext = GetMarkupContext();

	int px = m_spi.fVert ? point.y : point.x;

	XTP_SCROLLBAR_TRACKINFO* pSBTrack = new XTP_SCROLLBAR_TRACKINFO;
	ZeroMemory(pSBTrack, sizeof(XTP_SCROLLBAR_TRACKINFO));

	pSBTrack->cmdSB		  = (UINT)-1;
	pSBTrack->bTrackThumb = FALSE;
	pSBTrack->pSBInfo	  = pSBInfo;
	pSBTrack->hWndTrack	  = hWnd;

	m_pSBTrack = pSBTrack;

	RECT rcSB;
	LPINT pwX = (LPINT)&rcSB;
	LPINT pwY = pwX + 1;
	if (!pSBInfo->fVert)
		pwX = pwY--;

	*(pwX + 0) = pSBInfo->pxLeft;
	*(pwY + 0) = pSBInfo->pxTop;
	*(pwX + 2) = pSBInfo->pxRight;
	*(pwY + 2) = pSBInfo->pxBottom;

	pSBInfo->ht = HitTestScrollBar(point);
	switch (pSBInfo->ht)
	{
		case XTP_HTSCROLLUP:
			pSBTrack->cmdSB = SB_LINEUP;
			*(pwY + 2)		= pSBInfo->pxUpArrow;
			break;
		case XTP_HTSCROLLDOWN:
			pSBTrack->cmdSB = SB_LINEDOWN;
			*(pwY + 0)		= pSBInfo->pxDownArrow;
			break;
		case XTP_HTSCROLLUPPAGE:
			pSBTrack->cmdSB = SB_PAGEUP;
			*(pwY + 0)		= pSBInfo->pxUpArrow;
			*(pwY + 2)		= pSBInfo->pxThumbTop;
			break;
		case XTP_HTSCROLLTHUMB:
		DoThumbPos:
			if (pSBInfo->pxDownArrow - pSBInfo->pxUpArrow <= pSBInfo->cpxThumb)
			{
				delete m_pSBTrack;
				m_pSBTrack = NULL;
				return;
			}

			pSBTrack->cmdSB = SB_THUMBPOSITION;
			CalcTrackDragRect(pSBTrack);

			pSBTrack->pxOld = pSBInfo->pxStart = pSBInfo->pxThumbTop;
			pSBTrack->posNew = pSBTrack->posOld = pSBInfo->pos;
			pSBTrack->dpxThumb					= pSBInfo->pxStart - px;

			pSBTrack->bTrackThumb = TRUE;

			pMarkupContext->CaptureMouse(this);

			DoScroll(SB_THUMBTRACK, pSBTrack->posOld);
			break;
		case XTP_HTSCROLLDOWNPAGE:
			pSBTrack->cmdSB = SB_PAGEDOWN;
			*(pwY + 0)		= pSBInfo->pxThumbBottom;
			*(pwY + 2)		= pSBInfo->pxDownArrow;
			break;
	}

	if ((bDirect && pSBTrack->cmdSB != SB_LINEUP && pSBTrack->cmdSB != SB_LINEDOWN))
	{
		if (pSBTrack->cmdSB != SB_THUMBPOSITION)
		{
			goto DoThumbPos;
		}
		pSBTrack->dpxThumb = -(pSBInfo->cpxThumb / 2);
	}

	pMarkupContext->CaptureMouse(this);

	if (pSBTrack->cmdSB != SB_THUMBPOSITION)
	{
		CopyRect(&pSBTrack->rcTrack, &rcSB);
	}

	if (!pSBTrack->bTrackThumb)
	{
		TrackBox(WM_LBUTTONDOWN, point);
	}
	else
	{
		TrackThumb(WM_LBUTTONDOWN, point);
	}

	while (pMarkupContext->GetMouseCapture() == this)
	{
		MSG msg;

		if (!::GetMessage(&msg, NULL, 0, 0))
		{
			AfxPostQuitMessage((int)msg.wParam);
			break;
		}

		if (!IsWindow(hWnd))
			break;

		UINT cmd = msg.message;

		if (cmd == WM_TIMER && msg.wParam == IDSYS_SCROLL)
		{
			ContScroll();
		}
		else if (cmd >= WM_MOUSEFIRST && cmd <= WM_MOUSELAST)
		{
			CPoint ptScreen = msg.pt;

			ScreenToClient(hWnd, &ptScreen);

			if (!pSBTrack->bTrackThumb)
			{
				TrackBox(cmd, ptScreen);
			}
			else
			{
				TrackThumb(cmd, ptScreen);
			}
		}
		else
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	if (pSBTrack->hTimerSB != 0)
	{
		::KillTimer(hWnd, pSBTrack->hTimerSB);
	}

	delete m_pSBTrack;
	m_pSBTrack = NULL;

	if (IsWindow(hWnd))
	{
		GetCursorPos(&point);
		ScreenToClient(hWnd, &point);
		m_spi.ht = HitTestScrollBar(point);

		RedrawScrollBar();
	}
}

void CXTPMarkupScrollBar::OnMouseLeave(CXTPMarkupMouseEventArgs* e)
{
	if (m_spi.ht != HTNOWHERE)
	{
		m_spi.ht = HTNOWHERE;
		RedrawScrollBar();
	}

	CXTPMarkupControl::OnMouseLeave(e);
}

void CXTPMarkupScrollBar::OnMouseEnter(CXTPMarkupMouseEventArgs* e)
{
	CXTPMarkupControl::OnMouseEnter(e);
}

void CXTPMarkupScrollBar::OnMouseMove(CXTPMarkupMouseEventArgs* e)
{
	int ht = HitTestScrollBar(e->GetPosition(this));

	if (ht != m_spi.ht)
	{
		m_spi.ht = ht;
		RedrawScrollBar();
	}

	e->SetHandled();
}

void CXTPMarkupScrollBar::OnMouseLeftButtonUp(CXTPMarkupMouseButtonEventArgs* e)
{
	CXTPMarkupControl::OnMouseLeftButtonUp(e);
}

void CXTPMarkupScrollBar::OnMouseLeftButtonDown(CXTPMarkupMouseButtonEventArgs* e)
{
	e->SetHandled();

	if (IsEnabled())
	{
		PerformTrackInit(e->m_hWnd, e->GetPosition(this), &m_spi,
						 (GetKeyState(VK_SHIFT) < 0) ? TRUE : FALSE);
	}
}

void CXTPMarkupScrollBar::RedrawScrollBar()
{
	InvalidateVisual();
}

void CXTPMarkupScrollBar::CalcScrollBarInfo(LPRECT lprc, XTP_SCROLLBAR_POSINFO* pSBInfo,
											SCROLLINFO* pSI)
{
	ASSERT(NULL != pSBInfo);
	ASSERT(NULL != pSI);

	BOOL fVert = pSBInfo->fVert = (m_orientation == xtpMarkupOrientationVertical);

	pSBInfo->rc		 = *lprc;
	pSBInfo->pxPopup = 0;

	if (fVert)
	{
		pSBInfo->pxTop	  = lprc->top;
		pSBInfo->pxBottom = lprc->bottom;
		pSBInfo->pxLeft	  = lprc->left;
		pSBInfo->pxRight  = lprc->right;
		pSBInfo->cpxThumb = GetSystemMetrics(SM_CYVSCROLL);
	}
	else
	{
		pSBInfo->pxTop	  = lprc->left;
		pSBInfo->pxBottom = lprc->right;
		pSBInfo->pxLeft	  = lprc->top;
		pSBInfo->pxRight  = lprc->bottom;
		pSBInfo->cpxThumb = GetSystemMetrics(SM_CXHSCROLL);
	}

	pSBInfo->pos	= pSI->nPos;
	pSBInfo->page	= XTPToIntChecked(pSI->nPage);
	pSBInfo->posMin = pSI->nMin;
	pSBInfo->posMax = pSI->nMax;

	ASSERT(pSBInfo->posMin <= pSBInfo->posMax);
	int nRange = (pSBInfo->posMax - pSBInfo->posMin) + 1;
	ASSERT(0 < nRange);

	int cpx = min((pSBInfo->pxBottom - pSBInfo->pxTop) / 2, pSBInfo->cpxThumb);

	pSBInfo->pxUpArrow	 = pSBInfo->pxTop + cpx;
	pSBInfo->pxDownArrow = pSBInfo->pxBottom - cpx;

	if (pSBInfo->page != 0)
	{
		int i = MulDiv(pSBInfo->pxDownArrow - pSBInfo->pxUpArrow, pSBInfo->page, nRange);
		pSBInfo->cpxThumb = max(max(16, pSBInfo->cpxThumb / 2), i);
	}

	pSBInfo->pxMin = pSBInfo->pxTop + cpx;
	pSBInfo->cpx   = pSBInfo->pxBottom - cpx - pSBInfo->cpxThumb - pSBInfo->pxMin;

	int nDenom = 0;

	if (pSBInfo->page < nRange)
		nDenom = nRange - (pSBInfo->page ? pSBInfo->page : 1);

	if (0 != nDenom)
		pSBInfo->pxThumbTop = MulDiv(pSBInfo->pos - pSBInfo->posMin, pSBInfo->cpx, nDenom)
							  + pSBInfo->pxMin;
	else
		pSBInfo->pxThumbTop = pSBInfo->pxMin;

	pSBInfo->pxThumbBottom = pSBInfo->pxThumbTop + pSBInfo->cpxThumb;
}

CSize CXTPMarkupScrollBar::ArrangeOverride(CSize szFinalSize)
{
	CalcScrollBarInfo(CRect(0, 0, szFinalSize.cx, szFinalSize.cy), &m_spi, &m_si);

	return szFinalSize;
}

CSize CXTPMarkupScrollBar::MeasureOverride(CXTPMarkupDrawingContext* /*pDC*/,
										   CSize /*szAvailableSize*/)
{
	BOOL bHorizontal = (m_orientation == xtpMarkupOrientationHorizontal);

	if (bHorizontal)
		return CSize(0, GetSystemMetrics(SM_CYHSCROLL));

	return CSize(GetSystemMetrics(SM_CXVSCROLL), 0);
}

BOOL CXTPMarkupScrollBar::IsEnabled() const
{
	return m_spi.posMax - m_spi.posMin - m_spi.page + 1 > 0;
}

void CXTPMarkupScrollBar::OnRender(CXTPMarkupDrawingContext* pDC)
{
#define GETPARTSTATE(ht, pressed, hot, normal, disabled)                                           \
	(!bEnabled ? disabled : nPressedHt == ht ? pressed : nHotHt == ht ? hot : normal)

	XTP_SCROLLBAR_TRACKINFO* pSBTrack = m_pSBTrack;
	XTP_SCROLLBAR_POSINFO* pSBInfo	  = &m_spi;

	int cWidth = (pSBInfo->pxRight - pSBInfo->pxLeft);
	if (0 < cWidth)
	{
		BOOL nPressedHt = pSBTrack ? (pSBTrack->bTrackThumb || pSBTrack->fHitOld ? pSBInfo->ht : -1)
								   : -1;
		BOOL nHotHt		= pSBTrack ? -1 : pSBInfo->ht;

		BOOL bEnabled = IsEnabled();

		int nBtnTrackSize = pSBInfo->pxThumbBottom - pSBInfo->pxThumbTop;
		int nBtnTrackPos  = pSBInfo->pxThumbTop - pSBInfo->pxUpArrow;

		if (!bEnabled || pSBInfo->pxThumbBottom > pSBInfo->pxDownArrow)
			nBtnTrackPos = nBtnTrackSize = 0;

		BOOL bUseVisualStyle = TRUE;

		CXTPWinThemeWrapper m_themeScrollBar;
		m_themeScrollBar.OpenThemeData(NULL, L"SCROLLBAR");

		COLORREF clrFace		= GetSysColor(COLOR_3DFACE);
		COLORREF m_crBackHilite = clrFace, m_crBackPushed = clrFace, m_crBack = clrFace;

		CRect rcScrollBar(pSBInfo->rc);

		CXTPMarkupDedicatedDC* pDDC = pDC->GetDeviceContext()->GetDedicatedDC(rcScrollBar);
		rcScrollBar.OffsetRect(pDDC->GetCorrectionOffset());
		HDC hDC = pDDC->GetDC();

		if (!pSBInfo->fSizebox)
		{
			if (pSBInfo->fVert)
			{
				CRect rcArrowUp(rcScrollBar.left, rcScrollBar.top, rcScrollBar.right,
								pSBInfo->pxUpArrow);
				CRect rcArrowDown(rcScrollBar.left, pSBInfo->pxDownArrow, rcScrollBar.right,
								  rcScrollBar.bottom);
				CRect rcTrack(rcScrollBar.left, rcArrowUp.bottom, rcScrollBar.right,
							  rcArrowDown.top);

				CRect rcLowerTrack(rcTrack.left, rcTrack.top, rcTrack.right,
								   rcTrack.top + nBtnTrackPos);
				CRect rcBtnTrack(rcTrack.left, rcLowerTrack.bottom, rcTrack.right,
								 rcLowerTrack.bottom + nBtnTrackSize);
				CRect rcUpperTrack(rcTrack.left, rcBtnTrack.bottom, rcTrack.right, rcTrack.bottom);

				if (bUseVisualStyle && m_themeScrollBar.IsAppThemeActive())
				{
					m_themeScrollBar.DrawThemeBackground(hDC, SBP_ARROWBTN,
														 GETPARTSTATE(XTP_HTSCROLLUP, ABS_UPPRESSED,
																	  ABS_UPHOT, ABS_UPNORMAL,
																	  ABS_UPDISABLED),
														 rcArrowUp, NULL);
					m_themeScrollBar.DrawThemeBackground(hDC, SBP_ARROWBTN,
														 GETPARTSTATE(XTP_HTSCROLLDOWN,
																	  ABS_DOWNPRESSED, ABS_DOWNHOT,
																	  ABS_DOWNNORMAL,
																	  ABS_DOWNDISABLED),
														 rcArrowDown, NULL);

					if (!rcTrack.IsRectEmpty())
					{
						if (!rcLowerTrack.IsRectEmpty())
							m_themeScrollBar.DrawThemeBackground(
								hDC, SBP_LOWERTRACKVERT,
								GETPARTSTATE(XTP_HTSCROLLUPPAGE, SCRBS_PRESSED, SCRBS_HOT,
											 SCRBS_NORMAL, SCRBS_DISABLED),
								rcLowerTrack, NULL);

						if (!rcBtnTrack.IsRectEmpty())
						{
							m_themeScrollBar.DrawThemeBackground(
								hDC, SBP_THUMBBTNVERT,
								GETPARTSTATE(XTP_HTSCROLLTHUMB, SCRBS_PRESSED, SCRBS_HOT,
											 SCRBS_NORMAL, SCRBS_DISABLED),
								rcBtnTrack, NULL);
							if (rcBtnTrack.Height() > 13)
								m_themeScrollBar.DrawThemeBackground(
									hDC, SBP_GRIPPERVERT,
									GETPARTSTATE(XTP_HTSCROLLTHUMB, SCRBS_PRESSED, SCRBS_HOT,
												 SCRBS_NORMAL, SCRBS_DISABLED),
									rcBtnTrack, NULL);
						}

						if (!rcUpperTrack.IsRectEmpty())
							m_themeScrollBar.DrawThemeBackground(
								hDC, SBP_UPPERTRACKVERT,
								GETPARTSTATE(XTP_HTSCROLLDOWNPAGE, SCRBS_PRESSED, SCRBS_HOT,
											 SCRBS_NORMAL, SCRBS_DISABLED),
								rcUpperTrack, NULL);
					}
				}
				else
				{
					DrawFrameControl(hDC, &rcArrowUp, DFC_SCROLL,
									 UINT(DFCS_SCROLLUP | (!bEnabled ? DFCS_INACTIVE : 0)
										  | (nPressedHt == XTP_HTSCROLLUP ? DFCS_PUSHED : 0)));
					DrawFrameControl(hDC, &rcArrowDown, DFC_SCROLL,
									 UINT(DFCS_SCROLLDOWN | (!bEnabled ? DFCS_INACTIVE : 0)
										  | (nPressedHt == XTP_HTSCROLLDOWN ? DFCS_PUSHED : 0)));

					HBRUSH hbrRet = (HBRUSH)SendMessage(::GetDesktopWindow(), WM_CTLCOLORSCROLLBAR,
														(WPARAM)hDC, 0);
					::SetBkColor(hDC, GetSysColor(COLOR_3DHILIGHT));
					::SetTextColor(hDC, GetSysColor(COLOR_BTNFACE));
					::FillRect(hDC, &rcTrack, hbrRet);

					if (nPressedHt == XTP_HTSCROLLUPPAGE)
					{
						::InvertRect(hDC, &rcLowerTrack);
					}

					if (!rcTrack.IsRectEmpty() && !rcBtnTrack.IsRectEmpty())
					{
						::SetBkColor(hDC, nPressedHt == XTP_HTSCROLLTHUMB ? m_crBackPushed
										  : nHotHt == XTP_HTSCROLLTHUMB	  ? m_crBackHilite
																		  : m_crBack);
						::ExtTextOut(hDC, 0, 0, ETO_OPAQUE, rcBtnTrack, NULL, 0, NULL);

						::DrawEdge(hDC, &rcBtnTrack, EDGE_RAISED, (UINT)(BF_ADJUST | BF_RECT));
					}

					if (nPressedHt == XTP_HTSCROLLDOWNPAGE)
					{
						::InvertRect(hDC, &rcUpperTrack);
					}
				}
			}
			else
			{
				CRect rcArrowLeft(rcScrollBar.left, rcScrollBar.top, pSBInfo->pxUpArrow,
								  rcScrollBar.bottom);
				CRect rcArrowRight(pSBInfo->pxDownArrow, rcScrollBar.top, rcScrollBar.right,
								   rcScrollBar.bottom);
				CRect rcTrack(rcArrowLeft.right, rcScrollBar.top, rcArrowRight.left,
							  rcScrollBar.bottom);

				CRect rcLowerTrack(rcTrack.left, rcTrack.top, rcTrack.left + nBtnTrackPos,
								   rcTrack.bottom);
				CRect rcBtnTrack(rcLowerTrack.right, rcTrack.top,
								 rcLowerTrack.right + nBtnTrackSize, rcTrack.bottom);
				CRect rcUpperTrack(rcBtnTrack.right, rcTrack.top, rcTrack.right, rcTrack.bottom);

				if (bUseVisualStyle && m_themeScrollBar.IsAppThemeActive())
				{
					m_themeScrollBar.DrawThemeBackground(hDC, SBP_ARROWBTN,
														 GETPARTSTATE(XTP_HTSCROLLUP,
																	  ABS_LEFTPRESSED, ABS_LEFTHOT,
																	  ABS_LEFTNORMAL,
																	  ABS_LEFTDISABLED),
														 rcArrowLeft, NULL);
					m_themeScrollBar.DrawThemeBackground(hDC, SBP_ARROWBTN,
														 GETPARTSTATE(XTP_HTSCROLLDOWN,
																	  ABS_RIGHTPRESSED,
																	  ABS_RIGHTHOT, ABS_RIGHTNORMAL,
																	  ABS_RIGHTDISABLED),
														 rcArrowRight, NULL);

					if (!rcTrack.IsRectEmpty())
					{
						if (!rcLowerTrack.IsRectEmpty())
							m_themeScrollBar.DrawThemeBackground(
								hDC, SBP_LOWERTRACKHORZ,
								GETPARTSTATE(XTP_HTSCROLLUPPAGE, SCRBS_PRESSED, SCRBS_HOT,
											 SCRBS_NORMAL, SCRBS_DISABLED),
								rcLowerTrack, NULL);

						if (!rcBtnTrack.IsRectEmpty())
						{
							m_themeScrollBar.DrawThemeBackground(
								hDC, SBP_THUMBBTNHORZ,
								GETPARTSTATE(XTP_HTSCROLLTHUMB, SCRBS_PRESSED, SCRBS_HOT,
											 SCRBS_NORMAL, SCRBS_DISABLED),
								rcBtnTrack, NULL);
							if (rcBtnTrack.Width() > 13)
								m_themeScrollBar.DrawThemeBackground(
									hDC, SBP_GRIPPERHORZ,
									GETPARTSTATE(XTP_HTSCROLLTHUMB, SCRBS_PRESSED, SCRBS_HOT,
												 SCRBS_NORMAL, SCRBS_DISABLED),
									rcBtnTrack, NULL);
						}

						if (!rcUpperTrack.IsRectEmpty())
							m_themeScrollBar.DrawThemeBackground(
								hDC, SBP_UPPERTRACKHORZ,
								GETPARTSTATE(XTP_HTSCROLLDOWNPAGE, SCRBS_PRESSED, SCRBS_HOT,
											 SCRBS_NORMAL, SCRBS_DISABLED),
								rcUpperTrack, NULL);
					}
				}
				else
				{
					DrawFrameControl(hDC, &rcArrowLeft, DFC_SCROLL,
									 UINT(DFCS_SCROLLLEFT | (!bEnabled ? DFCS_INACTIVE : 0)
										  | (nPressedHt == XTP_HTSCROLLUP ? DFCS_PUSHED : 0)));
					DrawFrameControl(hDC, &rcArrowRight, DFC_SCROLL,
									 UINT(DFCS_SCROLLRIGHT | (!bEnabled ? DFCS_INACTIVE : 0)
										  | (nPressedHt == XTP_HTSCROLLDOWN ? DFCS_PUSHED : 0)));

					HBRUSH hbrRet = (HBRUSH)SendMessage(::GetDesktopWindow(), WM_CTLCOLORSCROLLBAR,
														(WPARAM)hDC, 0);
					::SetBkColor(hDC, GetSysColor(COLOR_3DHILIGHT));
					::SetTextColor(hDC, GetSysColor(COLOR_BTNFACE));
					::FillRect(hDC, &rcTrack, hbrRet);

					if (nPressedHt == XTP_HTSCROLLUPPAGE)
					{
						::InvertRect(hDC, &rcLowerTrack);
					}

					if (!rcTrack.IsRectEmpty() && !rcBtnTrack.IsRectEmpty())
					{
						::SetBkColor(hDC, nPressedHt == XTP_HTSCROLLTHUMB ? m_crBackPushed
										  : nHotHt == XTP_HTSCROLLTHUMB	  ? m_crBackHilite
																		  : m_crBack);
						::ExtTextOut(hDC, 0, 0, ETO_OPAQUE, rcBtnTrack, NULL, 0, NULL);

						::DrawEdge(hDC, &rcBtnTrack, EDGE_RAISED, (UINT)(BF_ADJUST | BF_RECT));
					}

					if (nPressedHt == XTP_HTSCROLLDOWNPAGE)
					{
						::InvertRect(hDC, &rcUpperTrack);
					}
				}
			}
		}
		else
		{
			if (bUseVisualStyle && m_themeScrollBar.IsAppThemeActive())
			{
				m_themeScrollBar.DrawThemeBackground(hDC, SBP_SIZEBOX, 0, &pSBInfo->rc, NULL);
			}
			else
			{
				DrawFrameControl(hDC, &pSBInfo->rc, DFC_SCROLL, DFCS_SCROLLSIZEGRIP);
			}
		}

		pDDC->Commit();
		delete pDDC;
	}
}
