// XTPSkinObjectFrame.cpp: implementation of the XTPSkinObjectFrame class.
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

#include "stdafx.h"
#include "Common/Base/cxminmax.h"

#include "Common/XTPTypeId.h"
#include "Common/XTPCasting.h"

#include "Common/XTPFramework.h"
#define XTP_INTERNAL_UXTHEME_INCLUSION
#include "Common/Uxtheme.h"
#include "Common/Tmschema.h"
#include "Common/XTPSystemHelpers.h"
#include "Common/XTPSynchro.h"
#include "Common/XTPApplication.h"
#include "Common/XTPSingleton.h"
#include "Common/XTPGdiObjects.h"
#include "Common/XTPDrawHelpers.h"
#include "Common/ScrollBar/XTPScrollBase.h"
#include "Common/XTPSynchro.h"

#include "SkinFramework/XTPSkinObject.h"
#include "SkinFramework/XTPSkinManager.h"
#include "SkinFramework/XTPSkinManagerSchema.h"
#include "SkinFramework/XTPSkinObjectFrame.h"
#include "SkinFramework/XTPSkinManagerApiHook.h"
#include "SkinFramework/XTPSkinImage.h"

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

#if defined(__VSSYM32_H__) && !defined(TMT_STOCKIMAGEFILE)
#	define TMT_STOCKIMAGEFILE 3007
#endif

#ifdef _DEBUG
#	define new DEBUG_NEW
#	undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CXTPSkinObjectFrame, CWnd)

CXTPSkinObjectFrame::CCaptionButton::CCaptionButton(int nCommand, CXTPSkinObjectFrame* pFrame,
													UINT nHTCode, int nClassPart)
	: m_nHTCode(nHTCode)
	, m_nClassPart(nClassPart)
	, m_pFrame(pFrame)
	, m_nCommand(nCommand)
	, m_bEnabled(TRUE)
{
	m_rcButton.SetRectEmpty();
}

CXTPSkinObjectFrame::CCaptionButton::~CCaptionButton()
{
	m_pFrame  = NULL;
	m_nHTCode = UINT(HTERROR);
}

BOOL CXTPSkinObjectFrame::CCaptionButton::IsPressed() const
{
	return m_pFrame->m_pButtonPressed == this;
}

BOOL CXTPSkinObjectFrame::CCaptionButton::IsHighlighted() const
{
	return m_pFrame->m_pButtonHot == this;
}

void CXTPSkinObjectFrame::CCaptionButton::Draw(CDC* pDC, BOOL bFrameActive)
{
	BOOL bHot	= IsHighlighted() && (IsPressed() || !m_pFrame->m_pButtonPressed);
	BOOL pRessed = bHot && IsPressed();

	int nState = !m_bEnabled ? SBS_DISABLED : pRessed ? SBS_PUSHED : bHot ? SBS_HOT : SBS_NORMAL;
	if (!bFrameActive)
		nState += 4;

	CXTPSkinManagerClass* pClassWindow = m_pFrame->GetSkinManager()->GetSkinClass(m_pFrame,
																				  _T("WINDOW"));
	pClassWindow->DrawThemeBackground(pDC, m_nClassPart, nState, m_rcButton);
}

/////////////////////////////////////////////////////////////////////////////
// CXTPSkinObjectFrame

CXTPThreadLocal<BOOL> CXTPSkinObjectFrame::m_bMenuStatus;

CXTPSkinObjectFrame::CXTPSkinObjectFrame()
	: m_bInUpdateRegion(FALSE)
	, m_bInWindowPosChanged(FALSE)
{
	*m_bMenuStatus = FALSE;

	m_rcBorders.SetRectEmpty();
	m_pSBTrack = NULL;

	m_strClassName	 = _T("WINDOW");
	m_nCtlColorMessage = WM_CTLCOLORDLG;

	m_bMDIClient = FALSE;

	ZeroMemory(&m_spi[SB_VERT], sizeof(XTP_SKINSCROLLBAR_POSINFO));
	ZeroMemory(&m_spi[SB_HORZ], sizeof(XTP_SKINSCROLLBAR_POSINFO));

	m_spi[SB_VERT].fVert	= TRUE;
	m_spi[SB_VERT].fSizebox = FALSE;
	m_spi[SB_VERT].nBar		= SB_VERT;

	m_spi[SB_HORZ].fVert	= FALSE;
	m_spi[SB_HORZ].fSizebox = FALSE;
	m_spi[SB_HORZ].nBar		= SB_HORZ;

	m_szFrameRegion = CSize(0, 0);

	m_pButtonHot	 = NULL;
	m_pButtonPressed = NULL;
	m_bActive		 = FALSE;

	m_dwExStyle = (DWORD)-1;
	m_dwStyle   = (DWORD)-1;

	m_bActiveX = FALSE;

	m_bRegionChanged = FALSE;
	m_bLockFrameDraw = FALSE;

	m_dwDialogTexture = ETDT_DISABLE;

	m_pMDIClient = NULL;
}

CXTPSkinObjectFrame::~CXTPSkinObjectFrame()
{
	m_pButtonHot	 = NULL;
	m_pButtonPressed = NULL;

	RemoveButtons();
}

void CXTPSkinObjectFrame::ScreenToFrame(LPPOINT lpPoint)
{
	CXTPDrawHelpers::ScreenToWindow(this, lpPoint);
}

void CXTPSkinObjectFrame::ClientToFrame(LPPOINT lpPoint)
{
	ClientToScreen(lpPoint);
	ScreenToFrame(lpPoint);
}

void CXTPSkinObjectFrame::AdjustFrame(CRect& rc)
{
	rc.DeflateRect(m_rcBorders);
}

#include "Common/Base/Diagnostic/XTPBeginAfxMap.h"
BEGIN_MESSAGE_MAP(CXTPSkinObjectFrame, CXTPSkinObject)
	//{{AFX_MSG_MAP(CXTPSkinObjectFrame)
	ON_WM_NCPAINT()
	ON_WM_NCCALCSIZE()
	ON_WM_NCMOUSEMOVE()
	ON_WM_NCHITTEST_EX()
	ON_WM_NCLBUTTONDOWN()
	ON_WM_NCLBUTTONDBLCLK()
	ON_WM_NCRBUTTONUP()
	ON_MESSAGE_VOID(WM_NCMOUSELEAVE, OnNcMouseLeave)
	ON_WM_NCACTIVATE()
	ON_MESSAGE(WM_SETTEXT, OnSetText)
	ON_WM_STYLECHANGED()
	ON_WM_CREATE()
	ON_WM_WINDOWPOSCHANGING()
	ON_WM_WINDOWPOSCHANGED()
	ON_WM_PAINT()
	ON_MESSAGE(WM_PRINT, OnPrint)
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_WM_GETMINMAXINFO()
	ON_WM_SYSCOMMAND()
	ON_WM_ERASEBKGND()

	ON_WM_SYSKEYDOWN()
	ON_WM_SYSKEYUP()
	ON_WM_KEYUP()
	ON_MESSAGE(WM_SETICON, OnSetIcon)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
#include "Common/Base/Diagnostic/XTPEndAfxMap.h"

/////////////////////////////////////////////////////////////////////////////
// CXTPSkinObjectFrame message handlers

BOOL CXTPSkinObjectFrame::OnEraseBkgnd(CDC* pDC)
{
	HBRUSH hbr	= (HBRUSH)(DWORD_PTR)GetClassLongPtr(m_hWnd, GCLP_HBRBACKGROUND);
	BOOL bChanged = FALSE;

	if ((ULONG_PTR)hbr > 0 && (ULONG_PTR)hbr < (ULONG_PTR)XTP_SKINMETRICS_COLORTABLESIZE)
	{
		HBRUSH hbrTheme = GetMetrics()->m_brTheme[(ULONG_PTR)hbr - 1];
		SetClassLongPtr(m_hWnd, GCLP_HBRBACKGROUND, (LONG_PTR)hbrTheme);
		bChanged = TRUE;
	}

	BOOL bResult = CXTPSkinObject::OnEraseBkgnd(pDC);

	if (bChanged)
	{
		SetClassLongPtr(m_hWnd, GCLP_HBRBACKGROUND, (LONG_PTR)hbr);
	}
	return bResult;
}

void CXTPSkinObjectFrame::OnNcPaint()
{
#if 0
	static _int64 nPerfomanceEnd;
	_int64 nPerfomanceStart;
	static _int64 nPerfomanceSum = 0;

	QueryPerformanceCounter((LARGE_INTEGER*)&nPerfomanceStart);
	if (nPerfomanceStart - nPerfomanceEnd  > 100000)
	{
		TRACE(_T("Reset Timer \n"));
		nPerfomanceSum = 0;
	}
#endif

	if (IsFlatScrollBarInitialized())
	{
		Default();
	}
	else
	{
		UpdateButtons();

		CWindowDC dc(this);
		DrawFrame(&dc);
	}

#if 0
	QueryPerformanceCounter((LARGE_INTEGER*)&nPerfomanceEnd);
	nPerfomanceSum += nPerfomanceEnd - nPerfomanceStart;
	TRACE(_T("TotalCounter = %i \n"), int(nPerfomanceSum));
#endif
}

void CXTPSkinObjectFrame::ResizeFrame(BOOL bUpdateRegion /*= TRUE*/)
{
	if (HasWindowBorder() && m_pManager->IsEnabled())
	{
		CXTPWindowRect rc(this);

		DWORD dwStyle   = GetStyle();
		DWORD dwExStyle = GetExStyle();

		if ((m_pManager->GetApplyOptions() & xtpSkinApplyFrame) && ((dwStyle & WS_CHILD) == 0))
		{
			CRect rcWindow(rc);
			AdjustWindowRectEx(rcWindow, dwStyle, FALSE, dwExStyle);

			CRect rcSysWindow(rc);
			XTPSkinManager()->GetApiHook()->AdjustWindowRectExOrig(rcSysWindow, dwStyle, FALSE,
																   dwExStyle);

			rc.bottom -= rcSysWindow.Height() - rcWindow.Height();
			rc.right -= rcSysWindow.Width() - rcWindow.Width();

			MoveWindow(rc, !bUpdateRegion);
		}

		if (bUpdateRegion)
			UpdateFrameRegion(rc.Size());
	}
}

void CXTPSkinObjectFrame::OnHookAttached(LPCREATESTRUCT lpcs, BOOL bAuto)
{
	CXTPSkinObject::OnHookAttached(lpcs, bAuto);

	if (!bAuto)
	{
		ResizeFrame();
	}
}

void CXTPSkinObjectFrame::RefreshFrameStyle()
{
	if ((GetSkinManager()->GetApplyOptions() & xtpSkinApplyFrame) == 0)
		return;

	m_bLockFrameDraw++;

	DWORD dwStyle		= GetStyle();
	DWORD dwStyleRemove = XTP_REFRESHFRAME_AFFECTED_STYLES;
	if (dwStyle & dwStyleRemove)
	{
		SetWindowLong(m_hWnd, GWL_STYLE, XTPToLong(dwStyle & ~dwStyleRemove));

		RECT rc = CXTPWindowRect(this);
		SendMessage(WM_NCCALCSIZE, FALSE, (LPARAM)&rc);

		SetWindowLong(m_hWnd, GWL_STYLE, XTPToLong(dwStyle));
	}

	m_bLockFrameDraw--;
}

void CXTPSkinObjectFrame::OnSkinChanged(BOOL bPrevState, BOOL bNewState)
{
	if (bNewState)
	{
		RefreshFrameStyle();
	}

	if (bNewState && !bPrevState)
	{
		m_bActive = SendMessage(0x0035) != 0;
	}

	SetWindowPos(0, 0, 0, 0, 0,
				 SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED | SWP_NOACTIVATE);

	RefreshMetrics();

	CXTPWindowRect rc(this);
	m_szFrameRegion = CSize(0);
	UpdateFrameRegion(rc.Size());

	Invalidate();
}

void CXTPSkinObjectFrame::OnHookDetached(BOOL bAuto)
{
	if (m_bRegionChanged && !bAuto && ::IsWindow(m_hWnd))
	{
		SetWindowRgn(NULL, TRUE);
		m_bRegionChanged = FALSE;
		SetWindowPos(0, 0, 0, 0, 0,
					 SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED | SWP_NOACTIVATE);
	}

	CXTPSkinObject::OnHookDetached(bAuto);
}

BOOL CXTPSkinObjectFrame::IsFrameScrollBars()
{
	HWND hWnd = m_hWnd;
	CXTPWindowRect rcChild(hWnd);

	int cxEdge = GetMetrics()->m_cxEdge;
	int cyEdge = GetMetrics()->m_cyEdge;

	while (hWnd)
	{
		if ((GetWindowLong(hWnd, GWL_STYLE) & WS_CAPTION) == WS_CAPTION)
		{
			CPoint ptParent = CXTPClientRect(hWnd).BottomRight();
			::ClientToScreen(hWnd, &ptParent);

			if ((rcChild.right + cxEdge < ptParent.x) || (rcChild.bottom + cyEdge < ptParent.y))
			{
				return FALSE;
			}

			return hWnd ? TRUE : FALSE;
		}

		if (((GetWindowLong(hWnd, GWL_STYLE) & WS_CHILD) == 0)
			|| ((GetWindowLong(hWnd, GWL_STYLE) & WS_CAPTION) == WS_CAPTION))
			break;

		hWnd = ::GetParent(hWnd);
	}

	return FALSE;
}

BOOL CXTPSkinObjectFrame::IsSizeBox()
{
	HWND hWnd = m_hWnd;
	CXTPWindowRect rcChild(hWnd);

	int cxEdge = GetMetrics()->m_cxEdge;
	int cyEdge = GetMetrics()->m_cyEdge;

	while (hWnd)
	{
		if (GetWindowLong(hWnd, GWL_STYLE) & WS_SIZEBOX)
		{
			if ((GetWindowLong(hWnd, GWL_STYLE) & WS_MAXIMIZE))
				return FALSE;

			CPoint ptParent = CXTPClientRect(hWnd).BottomRight();
			::ClientToScreen(hWnd, &ptParent);

			if ((rcChild.right + cxEdge < ptParent.x) || (rcChild.bottom + cyEdge < ptParent.y))
			{
				return FALSE;
			}

			return hWnd ? TRUE : FALSE;
		}

		if (((GetWindowLong(hWnd, GWL_STYLE) & WS_CHILD) == 0)
			|| ((GetWindowLong(hWnd, GWL_STYLE) & WS_CAPTION) == WS_CAPTION))
			break;

		hWnd = ::GetParent(hWnd);
	}

	return FALSE;
}

void CXTPSkinObjectFrame::RedrawFrame()
{
	if (m_pManager->GetApplyOptions() & xtpSkinApplyFrame)
	{
		CWindowDC dc(this);
		DrawFrame(&dc);
	}
	else
	{
		SendMessage(WM_NCPAINT);
	}
}

void CXTPSkinObjectFrame::RefreshScrollBarsData()
{
	DWORD dwStyle = GetStyle();

	m_spi[SB_VERT].fVisible = (0 != (dwStyle & WS_VSCROLL));
	m_spi[SB_HORZ].fVisible = (0 != (dwStyle & WS_HSCROLL));

	if (m_spi[SB_VERT].fVisible && (NULL == m_pSBTrack || !m_pSBTrack->bTrackThumb))
	{
		SetupScrollInfo(&m_spi[SB_VERT]);
	}

	if (m_spi[SB_HORZ].fVisible && (NULL == m_pSBTrack || !m_pSBTrack->bTrackThumb))
	{
		SetupScrollInfo(&m_spi[SB_HORZ]);
	}
}

void CXTPSkinObjectFrame::DrawFrame(CDC* pDC)
{
	if (m_bLockFrameDraw)
		return;

	CXTPWindowRect rc(this);
	rc.OffsetRect(-rc.TopLeft());

	GetSchema()->DrawThemeFrame(pDC, this);

	RefreshScrollBarsData();

	if (m_spi[SB_VERT].fVisible)
	{
		CXTPBufferDCEx dcMem(*pDC, m_spi[SB_VERT].rc);
		DrawScrollBar(&dcMem, &m_spi[SB_VERT]);
	}

	if (m_spi[SB_HORZ].fVisible)
	{
		CXTPBufferDCEx dcMem(*pDC, m_spi[SB_HORZ].rc);
		DrawScrollBar(&dcMem, &m_spi[SB_HORZ]);
	}

	if (m_spi[SB_HORZ].fVisible && m_spi[SB_VERT].fVisible)
	{
		CRect rcSizeGripper(m_spi[SB_HORZ].rc.right, m_spi[SB_VERT].rc.bottom,
							m_spi[SB_VERT].rc.right, m_spi[SB_HORZ].rc.bottom);

		if (XTPDrawHelpers()->IsVScrollOnTheLeft(this))
		{
			rcSizeGripper.left  = m_spi[SB_VERT].rc.left;
			rcSizeGripper.right = m_spi[SB_HORZ].rc.left;
		}

		pDC->FillSolidRect(rcSizeGripper, GetSchema()->GetScrollBarSizeBoxColor(this));

		if (IsSizeBox())
		{
			CXTPSkinManagerClass* pClassScrollBar = GetSkinManager()->GetSkinClass(this,
																				   _T("SCROLLBAR"));
			pClassScrollBar->DrawThemeBackground(pDC, SBP_SIZEBOX, SZB_RIGHTALIGN, rcSizeGripper);
		}
	}
}

BOOL CXTPSkinObjectFrame::IsFlatScrollBarInitialized() const
{
	INT nStyle = 0;
	if (FlatSB_GetScrollProp(m_hWnd, WSB_PROP_VSTYLE, &nStyle))
		return TRUE;

	return FALSE;
}

void CXTPSkinObjectFrame::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp)
{
	if (IsFlatScrollBarInitialized())
	{
		m_spi[SB_VERT].fVisible = FALSE;
		m_spi[SB_HORZ].fVisible = FALSE;
		CXTPSkinObject::OnNcCalcSize(bCalcValidRects, lpncsp);
		return;
	}

	m_rcBorders = GetSchema()->CalcFrameBorders(this);

	lpncsp[0].rgrc->top += m_rcBorders.top;
	lpncsp[0].rgrc->left += m_rcBorders.left;
	lpncsp[0].rgrc->right -= m_rcBorders.right;
	lpncsp[0].rgrc->bottom -= m_rcBorders.bottom;

	RefreshScrollBarsData();

	if (m_spi[SB_VERT].fVisible)
	{
		if (XTPDrawHelpers()->IsWindowRTL(this))
		{
			lpncsp[0].rgrc->left += GetMetrics()->m_cxVScroll;
		}
		else
		{
			lpncsp[0].rgrc->right -= GetMetrics()->m_cxVScroll;
		}
	}

	if (m_spi[SB_HORZ].fVisible)
	{
		lpncsp[0].rgrc->bottom -= GetMetrics()->m_cyHScroll;
	}

	if (lpncsp[0].rgrc->bottom < lpncsp[0].rgrc->top)
	{
		lpncsp[0].rgrc->bottom = lpncsp[0].rgrc->top;
	}

	if (lpncsp[0].rgrc->right < lpncsp[0].rgrc->left)
	{
		lpncsp[0].rgrc->right = lpncsp[0].rgrc->left;
	}
}

LRESULT CXTPSkinObjectFrame::HandleNcHitTest(CPoint point)
{
	ScreenToFrame(&point);

	RefreshScrollBarsData();

	if (m_spi[SB_VERT].fVisible && ::PtInRect(&m_spi[SB_VERT].rc, point))
		return (LRESULT)HTVSCROLL;

	if (m_spi[SB_HORZ].fVisible && ::PtInRect(&m_spi[SB_HORZ].rc, point))
		return (LRESULT)HTHSCROLL;

	if (m_spi[SB_VERT].fVisible && m_spi[SB_HORZ].fVisible && IsSizeBox())
	{
		CRect rcSizeGripper(m_spi[SB_HORZ].rc.right, m_spi[SB_VERT].rc.bottom,
							m_spi[SB_VERT].rc.right, m_spi[SB_HORZ].rc.bottom);

		if (rcSizeGripper.PtInRect(point))
			return XTPDrawHelpers()->IsWindowRTL(this) ? HTBOTTOMLEFT : HTBOTTOMRIGHT;
	}

	CCaptionButton* pButtonHot = HitTestButton(point);

	if (pButtonHot)
	{
		return XTPToLRESULT(pButtonHot->m_nHTCode);
	}

	return (LRESULT)HTNOWHERE;
}

LRESULT CXTPSkinObjectFrame::OnNcHitTest(CPoint point)
{
	LRESULT nHitCode = (LRESULT)CXTPSkinObject::OnNcHitTest(point);

	LRESULT nHitCode2 = HandleNcHitTest(point);
	if (nHitCode2 != HTNOWHERE)
		return nHitCode2;

	if (nHitCode == HTCLOSE || nHitCode == HTMAXBUTTON || nHitCode == HTMINBUTTON
		|| nHitCode == HTHELP || nHitCode == HTMENU)
		return (LRESULT)HTCAPTION;

	if ((nHitCode == HTVSCROLL || nHitCode == HTHSCROLL) && !IsFlatScrollBarInitialized())
		return (LRESULT)HTCLIENT;

	return nHitCode;
}

void CXTPSkinObjectFrame::CancelMouseLeaveTracking()
{
	KillTimer(XTP_TID_MOUSELEAVE);
}

void CXTPSkinObjectFrame::HandleTimer(UINT_PTR uTimerID)
{
	if (uTimerID == XTP_TID_REFRESHFRAME)
	{
		RefreshFrameStyle();
		KillTimer(XTP_TID_REFRESHFRAME);
		return;
	}

	if (uTimerID == XTP_TID_MOUSELEAVE)
	{
		RECT rect;
		POINT pt;

		GetWindowRect(&rect);
		::GetCursorPos(&pt);

		if (::GetCapture() != NULL)
			return;

		LRESULT lHitTest = HandleNcHitTest(MAKELPARAM(pt.x, pt.y));

		if ((lHitTest == HTCLIENT || lHitTest == HTNOWHERE || lHitTest == HTBOTTOMRIGHT
			 || lHitTest == HTBOTTOMLEFT)
			|| !::PtInRect(&rect, pt))
		{
			HandleMouseMove(CPoint(-1, -1));
			CancelMouseLeaveTracking();
		}
		return;
	}
}

void CALLBACK CXTPSkinObjectFrame::OnTimerInternal(HWND hWnd, UINT nMsg, UINT_PTR uTimerID,
												   DWORD /*dwTime*/)
{
	if (nMsg != WM_TIMER)
		return;

	XTP_SKINFRAMEWORK_MANAGE_STATE();
	XTP_GUARD_SHARED(CXTPSkinManager, XTPSkinManager())
	{
		CXTPSkinObjectFrame* pFrame = (CXTPSkinObjectFrame*)XTPSkinManager()->Lookup(hWnd);
		if (!pFrame)
		{
			::KillTimer(hWnd, uTimerID);
			return;
		}

		pFrame->HandleTimer(uTimerID);
	}
}

BOOL CXTPSkinObjectFrame::HandleMouseMove(CPoint point)
{
	CPoint ptClient(point);
	ScreenToFrame(&ptClient);

	CCaptionButton* pButton = HitTestButton(ptClient);

	if (m_pButtonHot != pButton)
	{
		if (m_pButtonHot && !pButton)
			CancelMouseLeaveTracking();

		m_pButtonHot = pButton;
		InvalidateButtons();

		if (m_pButtonHot)
		{
			SetTimer(XTP_TID_MOUSELEAVE, 50, &OnTimerInternal);
			return TRUE;
		}
	}

	if (m_pSBTrack)
		return TRUE;

	BOOL bResult = FALSE;
	for (int i = 0; i < 2; i++)
	{
		XTP_SKINSCROLLBAR_POSINFO* pSBInfo = &m_spi[i];
		int ht							   = HitTestScrollBar(pSBInfo, ptClient);

		if (ht != pSBInfo->ht && pSBInfo->fVisible)
		{
			if (pSBInfo->ht != HTNOWHERE && ht == HTNOWHERE && !bResult)
			{
				CancelMouseLeaveTracking();
			}

			pSBInfo->ht = ht;
			RedrawScrollBar(pSBInfo);
		}

		if (ht != HTNOWHERE)
		{
			SetTimer(XTP_TID_MOUSELEAVE, 50, &OnTimerInternal);
			bResult = TRUE;
		}
	}

	return bResult;
}

void CXTPSkinObjectFrame::OnNcMouseMove(UINT nHitTest, CPoint point)
{
	if (HandleMouseMove(point))
		return;

	CXTPSkinObject::OnNcMouseMove(nHitTest, point);
}

void CXTPSkinObjectFrame::OnNcMouseLeave()
{
	if (HandleMouseMove(CPoint(-1, -1)))
		return;

	Default();
}

void CXTPSkinObjectFrame::OnNcLButtonDblClk(UINT nHitTest, CPoint point)
{
	OnNcLButtonDown(nHitTest, point);
}

void CXTPSkinObjectFrame::OnNcRButtonUp(UINT nHitTest, CPoint point)
{
	CPoint ptClient(point);
	ScreenToFrame(&ptClient);

	for (int i = 0; i < 2; i++)
	{
		XTP_SKINSCROLLBAR_POSINFO* pSBInfo = &m_spi[i];
		int ht							   = HitTestScrollBar(pSBInfo, ptClient);

		if (ht != HTNOWHERE)
		{
			m_bLockFrameDraw++;
			RECT rc = CXTPWindowRect(this);
			SendMessage(WM_NCCALCSIZE, FALSE, (LPARAM)&rc);
			m_bLockFrameDraw--;

			SendMessage(WM_CONTEXTMENU, (WPARAM)m_hWnd, MAKELPARAM(point.x, point.y));

			RefreshFrameStyle();
			return;
		}
	}

	CXTPSkinObject::OnNcRButtonDown(nHitTest, point);
}

void CXTPSkinObjectFrame::OnNcLButtonDown(UINT nHitTest, CPoint point)
{
	if (m_pButtonHot)
	{
		TrackCaptionButton();
	}

	CPoint ptClient(point);
	ScreenToFrame(&ptClient);

	for (int i = 0; i < 2; i++)
	{
		XTP_SKINSCROLLBAR_POSINFO* pSBInfo = &m_spi[i];
		int ht							   = HitTestScrollBar(pSBInfo, ptClient);

		if (ht != HTNOWHERE)
		{
			TrackInit(ptClient, pSBInfo, (GetKeyState(VK_SHIFT) < 0) ? TRUE : FALSE);
			return;
		}
	}

	if (nHitTest == HTZOOM || nHitTest == HTREDUCE || nHitTest == HTCLOSE || nHitTest == HTHELP)
		return;

	CXTPSkinObject::OnNcLButtonDown(nHitTest, point);
}

BOOL CXTPSkinObjectFrame::OnCtlColor(UINT nMessage, WPARAM& wParam, LPARAM& lParam,
									 LRESULT& lResult, BOOL bConsiderDlgTexture /*= FALSE*/)
{
	if (!(WM_CTLCOLORBTN == nMessage || WM_CTLCOLORDLG == nMessage || WM_CTLCOLORSTATIC == nMessage
		  || WM_CTLCOLOREDIT == nMessage || WM_CTLCOLORLISTBOX == nMessage))
		return FALSE;

	if (m_strClassName == _T("COMBOBOX"))
		return FALSE;

	if (bConsiderDlgTexture && IsDefWindowProcAvail(XTPToInt(nMessage))
		&& (m_dwDialogTexture != ETDT_ENABLETAB || !m_bActiveX))
		return FALSE;

	HDC hCtlDC   = reinterpret_cast<HDC>(wParam);
	HWND hCtlWnd = reinterpret_cast<HWND>(lParam);
	if (nMessage == WM_CTLCOLOREDIT || nMessage == WM_CTLCOLORLISTBOX)
	{
		XTP_GUARD_SHARED_(CXTPSkinManager, XTPSkinManager(), pSkinManager)
		{
			CXTPSkinObject* pSkinObject = pSkinManager->Lookup(hCtlWnd);
			if (NULL != pSkinObject)
			{
				::SetBkColor(hCtlDC, pSkinObject->GetColor(COLOR_WINDOW));
				::SetTextColor(hCtlDC, pSkinObject->GetColor(COLOR_WINDOWTEXT));
				lResult = reinterpret_cast<LRESULT>(
					pSkinObject->GetMetrics()->m_xtpBrushWindow.GetSafeHandle());
				return TRUE;
			}
		}
	}

	UINT nCtlColorMsgId = nMessage - WM_CTLCOLORMSGBOX;
	if (CWnd::GrayCtlColor(hCtlDC, hCtlWnd, nCtlColorMsgId, GetMetrics()->m_xtpBrushDialog, 0))
	{
		lResult = reinterpret_cast<LRESULT>(GetClientBrush(hCtlDC, hCtlWnd, nCtlColorMsgId));
		return TRUE;
	}

	return FALSE;
}

BOOL CXTPSkinObjectFrame::OnHookDefWindowProc(UINT nMessage, WPARAM& wParam, LPARAM& lParam,
											  LRESULT& lResult)
{
	return !OnCtlColor(nMessage, wParam, lParam, lResult)
			   ? CXTPSkinObject::OnHookDefWindowProc(nMessage, wParam, lParam, lResult)
			   : TRUE;
}

BOOL CXTPSkinObjectFrame::IsDefWindowProcAvail(int nMessage) const
{
	if (!CXTPSkinObject::IsDefWindowProcAvail(nMessage))
		return FALSE;

	if (m_bActiveX && m_bSetWindowProc && nMessage == WM_GETMINMAXINFO)
		return FALSE;

	return TRUE;
}

BOOL CXTPSkinObjectFrame::PreHookMessage(UINT nMessage)
{
	if ((nMessage == WM_ENTERIDLE) && (m_pManager->GetApplyOptions() & xtpSkinApplyFrame)
		&& ((GetStyle() & (WS_CAPTION | WS_MINIMIZE | WS_VISIBLE))
			== (WS_CAPTION | WS_MINIMIZE | WS_VISIBLE)))
	{
		RedrawFrame();
	}

	if (nMessage >= WM_LBUTTONDOWN && nMessage <= WM_MBUTTONDBLCLK)
		*m_bMenuStatus = FALSE;

	if (m_bLockFrameDraw
		&& (nMessage == WM_NCCALCSIZE || nMessage == WM_STYLECHANGED
			|| nMessage == WM_STYLECHANGING))
		return TRUE;

	if ((m_pManager->GetApplyOptions() & xtpSkinApplyFrame) == 0)
	{
		if (nMessage == WM_NCPAINT || nMessage == WM_NCCALCSIZE || nMessage == WM_NCHITTEST
			|| nMessage == WM_NCLBUTTONDOWN || nMessage == WM_NCLBUTTONDBLCLK
			|| nMessage == WM_WINDOWPOSCHANGED || nMessage == WM_NCMOUSEMOVE
			|| nMessage == WM_NCMOUSELEAVE || nMessage == WM_NCACTIVATE
			|| nMessage == WM_WINDOWPOSCHANGING || nMessage == WM_PRINT)
		{
			return TRUE;
		}
	}

	return CXTPSkinObject::PreHookMessage(nMessage);
}

BOOL CXTPSkinObjectFrame::OnHookMessage(UINT nMessage, WPARAM& wParam, LPARAM& lParam,
										LRESULT& lResult)
{
	BOOL bHandled = OnCtlColor(nMessage, wParam, lParam, lResult, TRUE);

	if (!bHandled)
	{
		do
		{
#ifdef _XTP_ACTIVEX
			if (nMessage == WM_PAINT && m_dwDialogTexture == ETDT_ENABLETAB && m_bActiveX)
			{
				CPaintDC dc(this);
				::SetBrushOrgEx(dc, 0, 0, NULL);
				FillRect(dc, CXTPClientRect(this), GetClientBrush(dc, m_hWnd, CTLCOLOR_DLG));
				bHandled = TRUE;
				break;
			}
#endif
			if (nMessage == WM_ERASEBKGND && m_dwDialogTexture == ETDT_ENABLETAB)
			{
				::SetBrushOrgEx((HDC)wParam, 0, 0, NULL);
				FillRect((HDC)wParam, CXTPClientRect(this),
						 GetClientBrush((HDC)wParam, m_hWnd, CTLCOLOR_DLG));
				bHandled = TRUE;
				break;
			}
		} while (FALSE);
	}

	return !bHandled ? CXTPSkinObject::OnHookMessage(nMessage, wParam, lParam, lResult) : TRUE;
}

//////////////////////////////////////////////////////////////////////////
// CXTPSkinObjectFrame;

void CXTPSkinObjectFrame::RemoveButtons()
{
	for (int i = 0; i < m_arrButtons.GetSize(); i++)
	{
		delete m_arrButtons[i];
	}

	m_arrButtons.RemoveAll();

	ASSERT(NULL == m_pButtonHot);	 // Invalid after remove
	ASSERT(NULL == m_pButtonPressed); // Invalid after remove
}

void CXTPSkinObjectFrame::UpdateButton(int nCommand, BOOL bVisible, BOOL bEnabled, UINT htCode,
									   int nClassPart)
{
	if (bVisible)
	{
		CCaptionButton* pButton = new CCaptionButton(nCommand, this, htCode, nClassPart);
		pButton->m_bEnabled		= bEnabled;

		m_arrButtons.Add(pButton);
	}
}

void CXTPSkinObjectFrame::UpdateButtons()
{
	DWORD dwExStyle = GetExStyle();
	DWORD dwStyle   = GetStyle();

	if (m_dwStyle == dwStyle && m_dwExStyle == dwExStyle)
		return;

	m_dwExStyle = dwExStyle;
	m_dwStyle   = dwStyle;

	m_pButtonHot	 = NULL;
	m_pButtonPressed = NULL;

	RemoveButtons();

	if (HasCaption())
	{
		BOOL bToolWindow = (dwExStyle & WS_EX_TOOLWINDOW) == WS_EX_TOOLWINDOW;
		BOOL bMaximized  = (dwStyle & WS_MAXIMIZE) == WS_MAXIMIZE;
		BOOL bMinimized  = (dwStyle & WS_MINIMIZE) == WS_MINIMIZE;

		BOOL bSysMenu	 = (0 != (dwStyle & WS_SYSMENU));
		BOOL bDialogFrame = (0 != (dwStyle & WS_DLGFRAME))
							|| (0 != (dwExStyle & WS_EX_DLGMODALFRAME));

		BOOL bEnableClose		= TRUE;
		BOOL bEnabledMaximize   = ((dwStyle & WS_MAXIMIZEBOX) == WS_MAXIMIZEBOX);
		BOOL bEnabledMinimize   = ((dwStyle & WS_MINIMIZEBOX) == WS_MINIMIZEBOX);
		BOOL bShowMinMaxButtons = !bToolWindow && bSysMenu
								  && (bEnabledMaximize || bEnabledMinimize);

		if (bSysMenu && !bToolWindow && ((m_dwExStyle & WS_EX_MDICHILD) == 0))
		{
			HMENU hMenu = ::GetSystemMenu(m_hWnd, FALSE);
			if (::GetMenuState(hMenu, SC_CLOSE, MF_BYCOMMAND) & (MF_DISABLED | MF_GRAYED))
				bEnableClose = FALSE;
		}

		UpdateButton(SC_CLOSE, !bDialogFrame || bSysMenu, bEnableClose, HTCLOSE,
					 bToolWindow ? WP_SMALLCLOSEBUTTON : WP_CLOSEBUTTON);

		UpdateButton(SC_MAXIMIZE, !bMaximized && bShowMinMaxButtons, bEnabledMaximize, HTMAXBUTTON,
					 WP_MAXBUTTON);

		if (bMinimized)
		{
			UpdateButton(SC_RESTORE, bShowMinMaxButtons, bEnabledMinimize, HTMINBUTTON,
						 WP_RESTOREBUTTON);
		}
		else
		{
			UpdateButton(SC_RESTORE, bMaximized && bShowMinMaxButtons, bEnabledMaximize,
						 HTMAXBUTTON, WP_RESTOREBUTTON);

			UpdateButton(SC_MINIMIZE, bShowMinMaxButtons, bEnabledMinimize, HTMINBUTTON,
						 WP_MINBUTTON);
		}

		UpdateButton(SC_CONTEXTHELP,
					 ((dwExStyle & WS_EX_CONTEXTHELP) == WS_EX_CONTEXTHELP) && !bToolWindow
						 && bSysMenu,
					 TRUE, HTHELP, WP_HELPBUTTON);
	}
}

CXTPSkinObjectFrame::CCaptionButton* CXTPSkinObjectFrame::HitTestButton(CPoint pt)
{
	DWORD dwStyle = GetStyle();

	for (int i = 0; i < m_arrButtons.GetSize(); i++)
	{
		CCaptionButton* pButton = m_arrButtons[i];
		CRect rcButton(pButton->m_rcButton);

		if ((dwStyle & (WS_MAXIMIZE | WS_CHILD)) == WS_MAXIMIZE)
			rcButton.InflateRect(0, XTP_DPI_Y(2), i == 0 ? XTP_DPI_X(2) : 0, 0);

		if (rcButton.PtInRect(pt) && pButton->m_bEnabled)
			return pButton;
	}
	return NULL;
}

/////////////////////////////////////////////////////////////////////////////
// CXTPSkinObjectFrame message handlers

LRESULT CXTPSkinObjectFrame::OnSetText(WPARAM wParam, LPARAM lParam)
{
	if (((GetStyle() & WS_CAPTION) == WS_CAPTION)
		&& (GetSkinManager()->GetApplyOptions() & xtpSkinApplyFrame))
	{
		LRESULT lRet = DefWindowProc(WM_SETTEXT, wParam, lParam);

		RedrawFrame();

		if ((GetExStyle() & WS_EX_MDICHILD)
			&& (GetStyle() & (WS_MAXIMIZE | WS_CHILD)) == (WS_MAXIMIZE | WS_CHILD))
		{
			CWnd* pWnd = GetTopLevelParent();
			pWnd->SendMessage(WM_NCPAINT, 0, 0);
		}

		return lRet;
	}

	return Default();
}

HWND CXTPSkinObjectFrame::FindMDIClient()
{
	return m_pMDIClient ? m_pMDIClient->m_hWnd : NULL;
}

#if (_MSC_VER <= 1200) && !defined(_WIN64)
#	define GetWindowLongPtrW GetWindowLongW
#	define GetWindowLongPtrA GetWindowLongA
#endif

#ifndef DWLP_DLGPROC
#	define DWLP_DLGPROC 4
#endif

LRESULT CXTPSkinObjectFrame::CallDefDlgProc(UINT nMessage, WPARAM wParam, LPARAM lParam)
{
	if (m_dwDialogTexture == ETDT_ENABLE) // Dialog ?
	{
		int nNotifyFormat = (int)SendMessage(WM_NOTIFYFORMAT, 0, NF_QUERY);

		WNDPROC pDlgWndProc = (WNDPROC)(nNotifyFormat == NFR_UNICODE
											? GetWindowLongPtrW(m_hWnd, DWLP_DLGPROC)
											: GetWindowLongPtrA(m_hWnd, DWLP_DLGPROC));

		if (pDlgWndProc && (HIWORD((ULONG)(ULONG_PTR)pDlgWndProc) != 0xFFFF))
		{
			return (pDlgWndProc)(m_hWnd, nMessage, wParam, lParam);
		}
	}
	return 0;
}

BOOL CXTPSkinObjectFrame::OnNcActivate(BOOL bActive)
{
	if (!HasWindowBorder())
		return CXTPSkinObject::OnNcActivate(bActive);

	TCHAR szClassName[XTP_MAX_CLASSNAME + 1];
	if (0 < ::GetClassName(m_hWnd, szClassName, _countof(szClassName))
		&& 0 == _tcscmp(szClassName, _T("#32770")))
		CallDefDlgProc(WM_NCACTIVATE, (WPARAM)bActive, 0);
	else
		CXTPSkinObject::OnNcActivate(bActive);

	if (!bActive)
	{
		// Mimic MFC kludge to stay active if WF_STAYACTIVE bit is on
		CWnd* pWnd = CWnd::FromHandlePermanent(m_hWnd);
		if (pWnd && pWnd->m_nFlags & WF_STAYACTIVE)
			bActive = TRUE;

		if (!IsWindowEnabled())
			bActive = FALSE;
	}

	HWND hWndClient = FindMDIClient();
	HWND hWndActive = hWndClient ? (HWND)::SendMessage(hWndClient, WM_MDIGETACTIVE, 0, 0) : NULL;

	if (hWndActive != m_hWnd && hWndActive)
	{
		::SendMessage(hWndActive, WM_NCACTIVATE, XTPToWPARAM(bActive), 0);
		::SendMessage(hWndActive, WM_NCPAINT, 0, 0);
	}

	m_bActive = bActive; // update state

	RedrawFrame();
	return TRUE;
}

int CXTPSkinObjectFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CXTPSkinObject::OnCreate(lpCreateStruct) == -1)
		return -1;

	// UpdateFrameRegion must not be called at this point as it causes the dynamically added
	// controls behave in a strange way (see ticket 50447).
	ResizeFrame(FALSE);

	return 0;
}

BOOL CXTPSkinObjectFrame::HasCaption() const
{
	return (GetStyle() & WS_CAPTION) == WS_CAPTION;
}

BOOL CXTPSkinObjectFrame::HasWindowBorder() const
{
	return 0 != (GetStyle() & (WS_SIZEBOX | WS_DLGFRAME | WS_CAPTION));
}

void CXTPSkinObjectFrame::UpdateFrameRegion(CSize szFrameRegion)
{
	if (m_bInUpdateRegion)
		return;

	m_bInUpdateRegion = TRUE;

	if (HasWindowBorder() && (m_szFrameRegion != szFrameRegion))
	{
		if (GetSkinManager()->IsEnabled()
			&& GetSkinManager()->GetApplyOptions() & xtpSkinApplyFrame)
		{
			HRGN hRgn = GetSchema()->CalcFrameRegion(this, szFrameRegion);

			SetWindowRgn(hRgn, TRUE);
			m_bRegionChanged = TRUE;
		}
		else if (m_bRegionChanged)
		{
			SetWindowRgn(NULL, TRUE);
			m_bRegionChanged = FALSE;
		}

		m_szFrameRegion = szFrameRegion;
	}

	m_bInUpdateRegion = FALSE;
}

void CXTPSkinObjectFrame::OnWindowPosChanging(WINDOWPOS FAR* lpwndpos)
{
	CXTPSkinObject::OnWindowPosChanging(lpwndpos);

	if ((m_pManager->HasApplyOptions(xtpSkinApplyMetrics | xtpSkinApplyFrame))
		&& ((GetExStyle() & WS_EX_MDICHILD) == WS_EX_MDICHILD)
		&& ((GetStyle() & WS_MAXIMIZE) == WS_MAXIMIZE))
	{
		RECT rc;
		GetParent()->GetClientRect(&rc);

		int cx = rc.right - rc.left;
		int cy = rc.bottom - rc.top;

		if (lpwndpos->cx > cx + m_rcBorders.left + m_rcBorders.right)
		{
			lpwndpos->cx = cx + m_rcBorders.left + m_rcBorders.right;
			lpwndpos->cy = cy + m_rcBorders.top + m_rcBorders.bottom;
			lpwndpos->x  = -m_rcBorders.left;
			lpwndpos->y  = -m_rcBorders.top;
		}
	}

	CSize szFrameRegion(lpwndpos->cx, lpwndpos->cy);

	if (((lpwndpos->flags & SWP_NOSIZE) == 0) && (m_szFrameRegion != szFrameRegion))
	{
		if (!XTPDrawHelpers()->IsWindowRTL(this))
			UpdateFrameRegion(szFrameRegion);
	}
}

void CXTPSkinObjectFrame::OnWindowPosChanged(WINDOWPOS FAR* lpwndpos)
{
	if (m_bInWindowPosChanged)
	{
		m_bInWindowPosChanged = TRUE;
	}

	m_bInWindowPosChanged = TRUE;

	CXTPSkinObject::OnWindowPosChanged(lpwndpos);

	CSize szFrameRegion(lpwndpos->cx, lpwndpos->cy);

	if (((lpwndpos->flags & SWP_NOSIZE) == 0) && (m_szFrameRegion != szFrameRegion))
	{
		UpdateFrameRegion(szFrameRegion);
	}

	m_bInWindowPosChanged = FALSE;
}

void CXTPSkinObjectFrame::RefreshMetrics()
{
	CXTPSkinObject::RefreshMetrics();

	m_szFrameRegion = CSize(0, 0);

	PostMessage(WM_SYSCOLORCHANGE);
}

void CXTPSkinObjectFrame::InvalidateButtons()
{
	RedrawFrame();
}

void CXTPSkinObjectFrame::TrackCaptionButton()
{
	SetCapture();
	BOOL bAccept				   = FALSE;
	m_pButtonPressed			   = m_pButtonHot;
	CCaptionButton* pButtonPressed = m_pButtonHot;
	ASSERT(pButtonPressed);

	RedrawFrame();

	while (::GetCapture() == m_hWnd)
	{
		MSG msg;

		if (!::GetMessage(&msg, NULL, 0, 0))
		{
			AfxPostQuitMessage((int)msg.wParam);
			break;
		}

		if (msg.message == WM_LBUTTONUP)
		{
			bAccept = m_pButtonPressed == pButtonPressed;
			break;
		}
		else if (msg.message == WM_NCMOUSELEAVE)
		{
		}
		else if (msg.message == WM_MOUSEMOVE)
		{
			POINT point = msg.pt;
			ScreenToFrame(&point);

			CCaptionButton* pButton = HitTestButton(point) == pButtonPressed ? pButtonPressed
																			 : NULL;

			if (pButton != m_pButtonPressed)
			{
				m_pButtonPressed = pButton;
				RedrawFrame();
			}
		}
		else
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	m_pButtonPressed = NULL;
	m_pButtonHot	 = NULL;
	ReleaseCapture();
	RedrawFrame();

	if (bAccept)
	{
		SendMessage(WM_SYSCOMMAND, XTPToWPARAM(pButtonPressed->m_nCommand));
	}
}

void CXTPSkinObjectFrame::OnPaint()
{
	// Scroll bars must be excluded from update region so that they can be skinned.
	RefreshScrollBarsData();

	if (m_spi[SB_VERT].fVisible)
	{
		CXTPWindowRect rcWnd(this);
		ScreenToClient(&rcWnd);

		CRect rcScroll;
		int nWidth = GetMetrics()->m_cxVScroll;
		if (XTPDrawHelpers()->IsVScrollOnTheLeft(this))
		{
			rcScroll.SetRect(rcWnd.left, rcWnd.top, rcWnd.left + nWidth, rcWnd.bottom);
		}
		else
		{
			rcScroll.SetRect(rcWnd.right - nWidth, rcWnd.top, rcWnd.right, rcWnd.bottom);
		}

		ValidateRect(&rcScroll);
	}

	if (m_spi[SB_HORZ].fVisible)
	{
		CXTPWindowRect rcWnd(this);
		ScreenToClient(&rcWnd);
		CRect rcScroll(rcWnd.right, rcWnd.bottom - GetMetrics()->m_cyHScroll, rcWnd.right,
					   rcWnd.bottom);
		ValidateRect(&rcScroll);
	}

	CXTPSkinObject::OnPaint();

	// In some cases (like MSFTEDIT control) scrollbars can be first drawn in WM_PAINT,
	// o in order to ensure scrollbar is refreshed the frame has to be drawn.
	if (m_spi[SB_VERT].fVisible || m_spi[SB_HORZ].fVisible)
	{
		CWindowDC dc(this);
		DrawFrame(&dc);
	}
}

LRESULT CXTPSkinObjectFrame::OnPrint(WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult = TRUE;

	if (lParam != PRF_NONCLIENT)
	{
		m_bLockFrameDraw++;
		lResult = DefWindowProc(WM_PRINT, wParam, lParam);
		m_bLockFrameDraw--;
	}

	if (lParam & PRF_NONCLIENT)
	{
		CDC* pDC = CDC::FromHandle((HDC)wParam);

		if (XTPDrawHelpers()->IsWindowRTL(this) && !XTPDrawHelpers()->IsContextRTL(pDC))
			XTPDrawHelpers()->SetContextRTL(pDC, TRUE);

		DrawFrame(pDC);
	}

	return lResult;
}

void CXTPSkinObjectFrame::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	CXTPSkinObject::OnVScroll(nSBCode, nPos, pScrollBar);

	CWindowDC dc(this);
	DrawFrame(&dc);
}

void CXTPSkinObjectFrame::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	CXTPSkinObject::OnHScroll(nSBCode, nPos, pScrollBar);

	CWindowDC dc(this);
	DrawFrame(&dc);
}

BOOL CXTPSkinObjectFrame::HandleSysCommand(UINT nID, LPARAM lParam)
{
	if ((GetSkinManager()->GetApplyOptions() & xtpSkinApplyMenus) == 0)
		return FALSE;

	UINT nCmd = (nID & 0xFFF0);

	if (nCmd == SC_MOUSEMENU)
	{
		return TRUE;
	}

	if (nCmd == SC_KEYMENU)
	{
		if ((GetStyle() & WS_CHILD) == 0)
			return TRUE;

		HWND hWnd = m_hWnd;

		while (GetWindowLong(hWnd, GWL_STYLE) & WS_CHILD)
		{
			if (GetWindowLong(hWnd, GWL_STYLE) & WS_SYSMENU)
				break;

			hWnd = ::GetParent(hWnd);
		}

		if (hWnd != m_hWnd)
		{
			if (::GetMenu(hWnd) || (GetWindowLong(hWnd, GWL_STYLE) & WS_SYSMENU))
			{
				CXTPSkinObjectFrame* pFrame = (CXTPSkinObjectFrame*)GetSkinManager()->Lookup(hWnd);
				if (pFrame)
				{
					pFrame->HandleSysCommand(nID, lParam);
				}
			}

			return TRUE;
		}
	}

	return FALSE;
}

void CXTPSkinObjectFrame::OnSysCommand(UINT nID, LPARAM lParam)
{
	UINT nCmd = (nID & 0xFFF0);

	if ((nCmd == SC_MAXIMIZE)
		&& m_pManager->HasApplyOptions(xtpSkinApplyMetrics | xtpSkinApplyFrame)
		&& ((GetExStyle() & WS_EX_MDICHILD) == WS_EX_MDICHILD) && ((GetStyle() & WS_MAXIMIZE) == 0))
	{
		CXTPSkinObject::OnSysCommand(nID, lParam);

		RECT rc;
		GetParent()->GetClientRect(&rc);
		int cx = rc.right - rc.left;
		int cy = rc.bottom - rc.top;

		rc.left   = -m_rcBorders.left;
		rc.top	= -m_rcBorders.top;
		rc.right  = cx + m_rcBorders.right;
		rc.bottom = cy + m_rcBorders.bottom;

		MoveWindow(rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, TRUE);
		return;
	}

	if (HandleSysCommand(nID, lParam))
		return;

	if ((nCmd == SC_MOVE) && (GetStyle() & WS_MINIMIZE)
		&& (GetSkinManager()->GetApplyOptions() & xtpSkinApplyFrame))
	{
		DoDefWindowProc(WM_NCPAINT, 0, 0);
		RedrawFrame();
	}

	CXTPSkinObject::OnSysCommand(nID, lParam);

	if ((nCmd == SC_MINIMIZE) && (GetSkinManager()->GetApplyOptions() & xtpSkinApplyFrame))
	{
		RedrawFrame();
	}
}

HWND CXTPSkinObjectFrame::GetTopLevelWindow() const
{
	HWND hWnd = m_hWnd;
	while (GetWindowLong(hWnd, GWL_STYLE) & WS_CHILD)
		hWnd = ::GetParent(hWnd);
	return hWnd;
}

void CXTPSkinObjectFrame::OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar == VK_MENU && ((nFlags & KF_REPEAT) == 0))
	{
		HWND hWnd = GetTopLevelWindow();

		HMENU hMenu = ::GetMenu(hWnd);
		if (hMenu)
		{
			*m_bMenuStatus = TRUE;
			return;
		}
	}

	CXTPSkinObject::OnSysKeyDown(nChar, nRepCnt, nFlags);
}

void CXTPSkinObjectFrame::OnSysKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar == VK_MENU && *m_bMenuStatus)
	{
		*m_bMenuStatus = FALSE;

		HWND hWnd = GetTopLevelWindow();
		::SendMessage(hWnd, WM_SYSCOMMAND, SC_KEYMENU, 0);
		return;
	}

	CXTPSkinObject::OnSysKeyUp(nChar, nRepCnt, nFlags);
}

void CXTPSkinObjectFrame::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CXTPSkinObject::OnKeyUp(nChar, nRepCnt, nFlags);
}

void CXTPSkinObjectFrame::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	CXTPSkinObject::OnGetMinMaxInfo(lpMMI);

	if (m_pManager->HasApplyOptions(xtpSkinApplyFrame) && HasWindowBorder())
	{
		int yMin = m_rcBorders.top + m_rcBorders.bottom;
		int xMin = (int)m_arrButtons.GetSize() * m_rcBorders.top;

		xMin += GetSystemMetrics(SM_CYSIZE) + 2 * GetSystemMetrics(SM_CXEDGE);

		lpMMI->ptMinTrackSize.x = CXTP_max(lpMMI->ptMinTrackSize.x, xMin);
		lpMMI->ptMinTrackSize.y = CXTP_max(lpMMI->ptMinTrackSize.y, yMin);
	}

	if (m_pManager->HasApplyOptions(xtpSkinApplyFrame | xtpSkinApplyMetrics)
		&& ((GetExStyle() & WS_EX_MDICHILD) == WS_EX_MDICHILD))
	{
		int nDelta = m_rcBorders.top + lpMMI->ptMaxPosition.y;
		lpMMI->ptMaxPosition.y -= nDelta;
		lpMMI->ptMaxSize.y += nDelta;
	}
}

void CXTPSkinObjectFrame::OnStyleChanged(int nStyleType, LPSTYLESTRUCT lpStyleStruct)
{
	CXTPSkinObject::OnStyleChanged(nStyleType, lpStyleStruct);

	m_rcBorders = GetSchema()->CalcFrameBorders(this);

	if (((lpStyleStruct->styleNew ^ lpStyleStruct->styleOld)
		 & (WS_EX_TOOLWINDOW | WS_BORDER | WS_DLGFRAME | WS_THICKFRAME)))
	{
		CXTPWindowRect rc(this);
		m_szFrameRegion = CSize(0);
		UpdateFrameRegion(rc.Size());
	}

	if (HasWindowBorder())
	{
		PostMessage(WM_NCPAINT);
	}
}

int CXTPSkinObjectFrame::GetClientBrushMessage()
{
	return m_nCtlColorMessage;
}

HBRUSH CXTPSkinObjectFrame::GetClientBrush(CDC* pDC)
{
	return GetFillBackgroundBrush(pDC, GetClientBrushMessage());
}

HBRUSH CXTPSkinObjectFrame::GetFillBackgroundBrush(CDC* pDC, int nMessage)
{
	if (GetParent() == NULL)
		return GetMetrics()->m_brTheme[COLOR_3DFACE];

	HBRUSH hBrush = (HBRUSH)::SendMessage(GetParent()->GetSafeHwnd(), XTPToUInt(nMessage),
										  (WPARAM)pDC->GetSafeHdc(), (LPARAM)m_hWnd);
	if (hBrush)
	{
		if ((DWORD_PTR)hBrush < XTP_SKINMETRICS_COLORTABLESIZE)
		{
			hBrush = GetMetrics()->m_brTheme[(DWORD_PTR)hBrush - 1];
		}
		return hBrush;
	}

	return nMessage == WM_CTLCOLOREDIT || nMessage == WM_CTLCOLORLISTBOX
			   ? GetMetrics()->m_xtpBrushWindow
			   : GetMetrics()->m_brTheme[COLOR_3DFACE];
}

HBRUSH CXTPSkinObjectFrame::FillBackground(CDC* pDC, LPCRECT lprc,
										   int nMessage /*= WM_CTLCOLORSTATIC*/)
{
	HWND hWndParent = ::GetParent(m_hWnd);
	HBRUSH hBrush   = (NULL != hWndParent
						   ? (HBRUSH)::SendMessage(hWndParent, XTPToUInt(nMessage),
												   (WPARAM)pDC->GetSafeHdc(), (LRESULT)m_hWnd)
						   : NULL);
	if (NULL != hBrush)
	{
		if ((DWORD_PTR)hBrush < XTP_SKINMETRICS_COLORTABLESIZE)
		{
			hBrush = GetMetrics()->m_brTheme[(DWORD_PTR)hBrush - 1];
		}

		::FillRect(pDC->GetSafeHdc(), lprc, hBrush);
		return hBrush;
	}

	pDC->FillSolidRect(lprc, GetColor(COLOR_3DFACE));
	return GetMetrics()->m_brTheme[COLOR_3DFACE];
}

BOOL CXTPSkinObjectFrame::DrawMenuBar()
{
	m_dwStyle = m_dwExStyle = (DWORD)-1;

	// UpdateButtons();

	UpdateMenuBar();

	RedrawFrame();

	return TRUE;
}

LRESULT CXTPSkinObjectFrame::OnSetIcon(WPARAM, LPARAM)
{
	LRESULT lResult = Default();

	RedrawFrame();

	return lResult;
}

CRect CXTPSkinObjectFrame::GetScrollBarRect(int nBar)
{
	CRect rcClient;
	GetClientRect(&rcClient);

	if (nBar == SB_CTL)
	{
		return rcClient;
	}

	CRect rc;
	GetWindowRect(rc);
	ScreenToClient(rc);

	if (nBar == SB_VERT)
	{
		rc.bottom = rcClient.bottom - rc.top;
		rc.top	= rcClient.top - rc.top;

		if (XTPDrawHelpers()->IsVScrollOnTheLeft(this))
		{
			rc.left = rcClient.left - rc.left - GetMetrics()->m_cxVScroll;
		}
		else
		{
			rc.left = rcClient.right - rc.left;
		}
		rc.right = rc.left + GetMetrics()->m_cxVScroll;
	}

	if (nBar == SB_HORZ)
	{
		rc.right = rcClient.right - rc.left;
		rc.left  = rcClient.left - rc.left;

		rc.top	= rcClient.bottom - rc.top;
		rc.bottom = rc.top + GetMetrics()->m_cyHScroll;
	}

	return rc;
}

void CXTPSkinObjectFrame::SetupScrollInfo(XTP_SKINSCROLLBAR_POSINFO* pSBInfo)
{
	CRect rc = GetScrollBarRect(pSBInfo->nBar);

	SCROLLINFO si = { 0 };
	si.cbSize	 = sizeof(SCROLLINFO);

	si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;
	::GetScrollInfo(m_hWnd, pSBInfo->nBar, &si);

	CalcScrollBarInfo(&rc, pSBInfo, &si);
}

void CXTPSkinObjectFrame::RedrawScrollBar(int nBar)
{
	ASSERT(nBar == SB_VERT || nBar == SB_HORZ);
	RedrawScrollBar(&m_spi[nBar]);
}

void CXTPSkinObjectFrame::RedrawScrollBar(XTP_SKINSCROLLBAR_POSINFO* pSBInfo)
{
	if (!pSBInfo->fVisible)
		return;

	if (!IsWindow(m_hWnd))
		return;

	CDC* pDC = pSBInfo->nBar == SB_CTL ? (CDC*)new CClientDC(this) : (CDC*)new CWindowDC(this);

	if (pDC)
	{
		CRect rc = GetScrollBarRect(pSBInfo->nBar);

		CXTPBufferDCEx dcMem(*pDC, rc);

		if (!m_pSBTrack || !m_pSBTrack->bTrackThumb)
			SetupScrollInfo(pSBInfo);

		DrawScrollBar(&dcMem, pSBInfo);
	}

	delete pDC;
}

int CXTPSkinObjectFrame::HitTestScrollBar(XTP_SKINSCROLLBAR_POSINFO* pSBInfo, POINT pt)
{
	if (!pSBInfo->fVisible || pSBInfo->fSizebox)
		return HTNOWHERE;

	int px = pSBInfo->fVert ? pt.y : pt.x;

	if (!::PtInRect(&pSBInfo->rc, pt))
		return HTNOWHERE;

	if (px < pSBInfo->pxUpArrow)
		return XTP_HTSCROLLUP;

	if (px >= pSBInfo->pxDownArrow)
		return XTP_HTSCROLLDOWN;

	if (px < pSBInfo->pxThumbTop)
		return XTP_HTSCROLLUPPAGE;

	if (px < pSBInfo->pxThumbBottom)
		return XTP_HTSCROLLTHUMB;

	if (px < pSBInfo->pxDownArrow)
		return XTP_HTSCROLLDOWNPAGE;

	return HTERROR;
}

void CXTPSkinObjectFrame::CalcScrollBarInfo(LPRECT lprc, XTP_SKINSCROLLBAR_POSINFO* pSBInfo,
											SCROLLINFO* pSI)
{
	int cpx;
	DWORD dwRange;
	int denom;
	BOOL fVert = pSBInfo->fVert;

	pSBInfo->rc = *lprc;

	if (fVert)
	{
		pSBInfo->pxTop	= lprc->top;
		pSBInfo->pxBottom = lprc->bottom;
		pSBInfo->pxLeft   = lprc->left;
		pSBInfo->pxRight  = lprc->right;
		pSBInfo->cpxThumb = GetMetrics()->m_cyVScroll;
	}
	else
	{
		pSBInfo->pxTop	= lprc->left;
		pSBInfo->pxBottom = lprc->right;
		pSBInfo->pxLeft   = lprc->top;
		pSBInfo->pxRight  = lprc->bottom;
		pSBInfo->cpxThumb = GetMetrics()->m_cxHScroll;
	}

	pSBInfo->pos	= pSI->nPos;
	pSBInfo->page   = XTPToIntChecked(pSI->nPage);
	pSBInfo->posMin = pSI->nMin;
	pSBInfo->posMax = pSI->nMax;

	dwRange = ((DWORD)(pSBInfo->posMax - pSBInfo->posMin)) + 1;

	cpx = CXTP_min((pSBInfo->pxBottom - pSBInfo->pxTop) / 2, pSBInfo->cpxThumb);

	pSBInfo->pxUpArrow   = pSBInfo->pxTop + cpx;
	pSBInfo->pxDownArrow = pSBInfo->pxBottom - cpx;

	if ((pSBInfo->page != 0) && (dwRange != 0))
	{
		int i = MulDiv(pSBInfo->pxDownArrow - pSBInfo->pxUpArrow, pSBInfo->page,
					   XTPToIntChecked(dwRange));

		pSBInfo->cpxThumb = CXTP_max(CXTP_max(XTP_DPI_X(16), pSBInfo->cpxThumb / 2), i);
	}

	pSBInfo->pxMin = pSBInfo->pxTop + cpx;
	pSBInfo->cpx   = pSBInfo->pxBottom - cpx - pSBInfo->cpxThumb - pSBInfo->pxMin;
	if (pSBInfo->cpx < 0)
		pSBInfo->cpx = 0;

	denom = XTPToIntChecked(dwRange - (pSBInfo->page ? pSBInfo->page : 1));
	if (denom)
		pSBInfo->pxThumbTop = MulDiv(pSBInfo->pos - pSBInfo->posMin, pSBInfo->cpx, denom)
							  + pSBInfo->pxMin;
	else
		pSBInfo->pxThumbTop = pSBInfo->pxMin - 1;

	pSBInfo->pxThumbBottom = pSBInfo->pxThumbTop + pSBInfo->cpxThumb;
}

void XTPSkinCalcTrackDragRect(XTP_SKINSCROLLBAR_TRACKINFO* pSBTrack)
{
	int cx;
	int cy;
	LPINT pwX, pwY;

	pwX = pwY = (LPINT)&pSBTrack->rcTrack;

	if (pSBTrack->fTrackVert)
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

void CXTPSkinObjectFrame::DoScroll(HWND hwnd, HWND hWndSBNotify, int cmd, int pos, BOOL fVert)
{
	if (!hWndSBNotify)
		return;

	::SendMessage(hWndSBNotify, (UINT)(fVert ? WM_VSCROLL : WM_HSCROLL),
				  XTPToWPARAM(MAKELONG(cmd, pos)), (LPARAM)hwnd);
}

void CXTPSkinObjectFrame::EndScroll(BOOL fCancel)
{
	XTP_SKINSCROLLBAR_TRACKINFO* pSBTrack = m_pSBTrack;

	if (pSBTrack)
	{
		pSBTrack->cmdSB = 0;
		ReleaseCapture();

		if (pSBTrack->bTrackThumb)
		{
			if (fCancel)
			{
				pSBTrack->posOld = pSBTrack->pSBInfo->pos;
			}

			DoScroll(pSBTrack->hWndSB, pSBTrack->hWndSBNotify, SB_THUMBPOSITION, pSBTrack->posOld,
					 pSBTrack->fTrackVert);

			RedrawScrollBar(pSBTrack->pSBInfo);
		}
		else
		{
			if (pSBTrack->hTimerSB != 0)
			{
				KillTimer(pSBTrack->hTimerSB);
				pSBTrack->hTimerSB = 0;
			}
		}

		DoScroll(pSBTrack->hWndSB, pSBTrack->hWndSBNotify, SB_ENDSCROLL, 0, pSBTrack->fTrackVert);
	}

	pSBTrack->hWndSBNotify = NULL;
}

void CXTPSkinObjectFrame::MoveThumb(int px)
{
	XTP_SKINSCROLLBAR_TRACKINFO* pSBTrack = m_pSBTrack;

	if ((pSBTrack == NULL) || (px == pSBTrack->pxOld))
		return;

	XTP_SKINSCROLLBAR_POSINFO* pSBInfo = m_pSBTrack->pSBInfo;

pxReCalc:

	pSBTrack->posNew = SBPosFromPx(pSBInfo, px);

	if (pSBTrack->posNew != pSBTrack->posOld)
	{
		DoScroll(pSBTrack->hWndSB, pSBTrack->hWndSBNotify, SB_THUMBTRACK, pSBTrack->posNew,
				 pSBTrack->fTrackVert);

		pSBTrack->posOld = pSBTrack->posNew;

		if (px >= pSBInfo->pxMin + pSBInfo->cpx)
		{
			px = pSBInfo->pxMin + pSBInfo->cpx;
			goto pxReCalc;
		}
	}

	pSBInfo->pxThumbTop	= px;
	pSBInfo->pxThumbBottom = pSBInfo->pxThumbTop + pSBInfo->cpxThumb;
	pSBTrack->pxOld		   = px;

	RedrawScrollBar(pSBTrack->pSBInfo);
}

void CXTPSkinObjectFrame::TrackThumb(UINT message, CPoint pt)
{
	XTP_SKINSCROLLBAR_TRACKINFO* pSBTrack = m_pSBTrack;
	if (!pSBTrack)
		return;

	XTP_SKINSCROLLBAR_POSINFO* pSBInfo = pSBTrack->pSBInfo;

	if (HIBYTE(message) != HIBYTE(WM_MOUSEFIRST))
		return;

	if (pSBInfo == NULL)
		return;

	int px;

	if (!PtInRect(&pSBTrack->rcTrack, pt))
		px = pSBInfo->pxStart;
	else
	{
		px = (pSBTrack->fTrackVert ? pt.y : pt.x) + pSBTrack->dpxThumb;
		if (px < pSBInfo->pxMin)
			px = pSBInfo->pxMin;
		else if (px >= pSBInfo->pxMin + pSBInfo->cpx)
			px = pSBInfo->pxMin + pSBInfo->cpx;
	}

	MoveThumb(px);

	pSBTrack->fHitOld = TRUE;

	if (message == WM_LBUTTONUP || GetKeyState(VK_LBUTTON) >= 0)
	{
		EndScroll(FALSE);
	}
}

void CXTPSkinObjectFrame::TrackBox(UINT message, CPoint point)
{
	XTP_SKINSCROLLBAR_TRACKINFO* pSBTrack = m_pSBTrack;

	if (pSBTrack == NULL)
		return;

	if (message != WM_NULL && HIBYTE(message) != HIBYTE(WM_MOUSEFIRST))
		return;

	if ((pSBTrack->cmdSB == SB_PAGEUP || pSBTrack->cmdSB == SB_PAGEDOWN))
	{
		int* pLength = (int*)&pSBTrack->rcTrack;

		if (pSBTrack->fTrackVert)
			pLength++;

		if (pSBTrack->cmdSB == SB_PAGEUP)
			pLength[2] = pSBTrack->pSBInfo->pxThumbTop;
		else
			pLength[0] = pSBTrack->pSBInfo->pxThumbBottom;
	}

	BOOL fHit = PtInRect(&pSBTrack->rcTrack, point);

	BOOL fHitChanged = fHit != (BOOL)pSBTrack->fHitOld;

	if (fHitChanged)
	{
		pSBTrack->fHitOld = fHit;
		RedrawScrollBar(pSBTrack->pSBInfo);
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
				pSBTrack->hTimerSB = SetTimer(IDSYS_SCROLL, XTPToUIntChecked(cmsTimer), NULL);

				DoScroll(pSBTrack->hWndSB, pSBTrack->hWndSBNotify, XTPToIntChecked(pSBTrack->cmdSB),
						 0, pSBTrack->fTrackVert);
			}
	}
}

void CXTPSkinObjectFrame::TrackInit(CPoint point, XTP_SKINSCROLLBAR_POSINFO* pSBInfo, BOOL bDirect)
{
	int px = (pSBInfo->fVert ? point.y : point.x);

	XTP_SKINSCROLLBAR_TRACKINFO* pSBTrack = new XTP_SKINSCROLLBAR_TRACKINFO;
	ZeroMemory(pSBTrack, sizeof(XTP_SKINSCROLLBAR_TRACKINFO));

	pSBTrack->fTrackVert = pSBInfo->fVert;

	if (pSBInfo->nBar == SB_CTL)
	{
		pSBTrack->hWndSB	   = m_hWnd;
		pSBTrack->hWndSBNotify = ::GetParent(m_hWnd);
		pSBTrack->fNonClient   = FALSE;
	}
	else
	{
		pSBTrack->hWndSB	   = NULL;
		pSBTrack->hWndSBNotify = m_hWnd;
		pSBTrack->fNonClient   = TRUE;
	}

	pSBTrack->cmdSB		  = (UINT)-1;
	pSBTrack->nBar		  = pSBInfo->nBar;
	pSBTrack->bTrackThumb = FALSE;
	pSBTrack->pSBInfo	 = pSBInfo;

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

	if (px < pSBInfo->pxUpArrow)
	{
		pSBInfo->ht		= XTP_HTSCROLLUP;
		pSBTrack->cmdSB = SB_LINEUP;
		*(pwY + 2)		= pSBInfo->pxUpArrow;
	}
	else if (px >= pSBInfo->pxDownArrow)
	{
		pSBInfo->ht		= XTP_HTSCROLLDOWN;
		pSBTrack->cmdSB = SB_LINEDOWN;
		*(pwY + 0)		= pSBInfo->pxDownArrow;
	}
	else if (px < pSBInfo->pxThumbTop)
	{
		pSBInfo->ht		= XTP_HTSCROLLUPPAGE;
		pSBTrack->cmdSB = SB_PAGEUP;
		*(pwY + 0)		= pSBInfo->pxUpArrow;
		*(pwY + 2)		= pSBInfo->pxThumbTop;
	}
	else if (px < pSBInfo->pxThumbBottom)
	{
		pSBInfo->ht = XTP_HTSCROLLTHUMB;
	DoThumbPos:

		if (pSBInfo->pxDownArrow - pSBInfo->pxUpArrow <= pSBInfo->cpxThumb)
		{
			delete m_pSBTrack;
			m_pSBTrack = NULL;
			return;
		}

		pSBTrack->cmdSB = SB_THUMBPOSITION;
		XTPSkinCalcTrackDragRect(pSBTrack);

		pSBTrack->pxOld = pSBInfo->pxStart = pSBInfo->pxThumbTop;
		pSBTrack->posNew = pSBTrack->posOld = pSBInfo->pos;
		pSBTrack->dpxThumb					= pSBInfo->pxStart - px;

		pSBTrack->bTrackThumb = TRUE;

		SetCapture();

		DoScroll(pSBTrack->hWndSB, pSBTrack->hWndSBNotify, SB_THUMBTRACK, pSBTrack->posOld,
				 pSBTrack->fTrackVert);
	}
	else if (px < pSBInfo->pxDownArrow)
	{
		pSBInfo->ht		= XTP_HTSCROLLDOWNPAGE;
		pSBTrack->cmdSB = SB_PAGEDOWN;
		*(pwY + 0)		= pSBInfo->pxThumbBottom;
		*(pwY + 2)		= pSBInfo->pxDownArrow;
	}

	if ((bDirect && pSBTrack->cmdSB != SB_LINEUP && pSBTrack->cmdSB != SB_LINEDOWN))
	{
		if (pSBTrack->cmdSB != SB_THUMBPOSITION)
		{
			goto DoThumbPos;
		}
		pSBTrack->dpxThumb = -(pSBInfo->cpxThumb / 2);
	}

	SetCapture();

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

	while (::GetCapture() == m_hWnd && pSBTrack->hWndSBNotify != 0)
	{
		MSG msg;

		if (!::GetMessage(&msg, NULL, 0, 0))
		{
			AfxPostQuitMessage((int)msg.wParam);
			break;
		}

		UINT cmd = msg.message;

		if (cmd == WM_TIMER && msg.wParam == IDSYS_SCROLL)
		{
			ContScroll();
		}
		else if (cmd >= WM_MOUSEFIRST && cmd <= WM_MOUSELAST)
		{
			CPoint ptScreen = msg.pt;

			if (pSBTrack->fNonClient)
				ScreenToFrame(&ptScreen);
			else
				ScreenToClient(&ptScreen);

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
		KillTimer(pSBTrack->hTimerSB);
	}

	pSBInfo->ht = 0;

	delete m_pSBTrack;
	m_pSBTrack = NULL;

	RedrawScrollBar(pSBInfo);
}

void CXTPSkinObjectFrame::ContScroll()
{
	XTP_SKINSCROLLBAR_TRACKINFO* pSBTrack = m_pSBTrack;
	ASSERT(pSBTrack);

	if (pSBTrack == NULL)
		return;

	CPoint pt;
	GetCursorPos(&pt);

	if (pSBTrack->fNonClient)
		ScreenToFrame(&pt);
	else
		ScreenToClient(&pt);

	TrackBox(WM_NULL, pt);

	if (pSBTrack->fHitOld)
	{
		pSBTrack->hTimerSB = SetTimer(IDSYS_SCROLL, GetDoubleClickTime() / 10, NULL);

		DoScroll(pSBTrack->hWndSB, pSBTrack->hWndSBNotify, XTPToIntChecked(pSBTrack->cmdSB), 0,
				 pSBTrack->fTrackVert);
	}
}

void CXTPSkinObjectFrame::DrawScrollBar(CDC* pDC, XTP_SKINSCROLLBAR_POSINFO* pSBInfo)
{
	GetSchema()->DrawThemeScrollBar(pDC, this, pSBInfo);
}

HBRUSH CXTPSkinObjectFrame::GetClientBrush(HDC hDC, HWND hWnd, UINT nCtlColor)
{
	if (m_dwDialogTexture != ETDT_ENABLETAB)
	{
		GrayCtlColor(hDC, hWnd, nCtlColor, GetMetrics()->m_xtpBrushDialog, GetColor(COLOR_BTNTEXT));
		return GetMetrics()->m_xtpBrushDialog;
	}

	if (GetMetrics()->m_xtpBrushTabControl.GetSafeHandle() == NULL)
	{
		CWindowDC dcWindow(this);

		CXTPSkinManagerClass* pClass = GetSkinManager()->GetSkinClass(this, _T("TAB"));

		CString strImageFile = pClass->GetThemeString(TABP_BODY, 0, TMT_STOCKIMAGEFILE);
		if (strImageFile.IsEmpty())
		{
			return GetMetrics()->m_xtpBrushDialog;
		}

		CXTPSkinImage* pImage = pClass->GetImages()->LoadFile(m_pManager->GetResourceFile(),
															  strImageFile);

		if (!pImage)
		{
			return GetMetrics()->m_xtpBrushDialog;
		}

		CRect rc(0, 0, XTPToIntChecked(pImage->GetWidth()), XTPToIntChecked(pImage->GetHeight()));

		CBitmap bmp;
		bmp.CreateCompatibleBitmap(&dcWindow, rc.Width(), rc.Height());

		CXTPCompatibleDC dc(&dcWindow, bmp);

		pImage->DrawImage(&dc, rc, rc, CRect(0, 0, 0, 0), COLORREF_NULL, ST_TRUESIZE, FALSE);

		GetMetrics()->CreateTabControlBrush(&bmp);
	}

	if (hWnd != m_hWnd)
	{
		CXTPWindowRect rcPaint(hWnd);
		CXTPWindowRect rcBrush(m_hWnd);
		::SetBrushOrgEx(hDC, rcBrush.left - rcPaint.left, rcBrush.top - rcPaint.top, NULL);

		::SetBkMode(hDC, TRANSPARENT);
		::SetTextColor(hDC, GetColor(COLOR_BTNTEXT));
	}

	return GetMetrics()->m_xtpBrushTabControl;
}

//////////////////////////////////////////////////////////////////////////
// CXTPSkinObjectUser32Control

CXTPSkinObjectUser32Control::CXTPSkinObjectUser32Control()
{
	ZeroMemory(&m_si, sizeof(m_si));
}

void CXTPSkinObjectUser32Control::CheckScrollBarsDraw()
{
	if (m_pManager->IsComCtlV6())
		return;

	if (!IsWindow(m_hWnd))
		return;

	for (int i = 0; i < 2; i++)
	{
		if (m_spi[i].fVisible)
		{
			SCROLLINFO si = { 0 };
			si.cbSize	 = sizeof(SCROLLINFO);

			GetScrollInfo(i, &si, SIF_POS | SIF_RANGE);

			if (si.nPos != m_si[i].nPos || si.nMax != m_si[i].nMax || si.nMin != m_si[i].nMin)
			{
				m_si[i] = si;
				RedrawScrollBar(&m_spi[i]);
			}
		}
	}
}

#include "Common/Base/Diagnostic/XTPBeginAfxMap.h"
BEGIN_MESSAGE_MAP(CXTPSkinObjectUser32Control, CXTPSkinObjectFrame)
	//{{AFX_MSG_MAP(CXTPSkinObjectUser32Control)
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_WM_WINDOWPOSCHANGED()
	ON_WM_ERASEBKGND()
	ON_WM_KEYDOWN()
	ON_WM_CHAR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
#include "Common/Base/Diagnostic/XTPEndAfxMap.h"

BOOL CXTPSkinObjectUser32Control::OnEraseBkgnd(CDC* pDC)
{
	BOOL bResult = CXTPSkinObjectFrame::OnEraseBkgnd(pDC);

	CheckScrollBarsDraw();

	return bResult;
}

void CXTPSkinObjectUser32Control::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CXTPSkinObjectFrame::OnKeyDown(nChar, nRepCnt, nFlags);
	CheckScrollBarsDraw();
}

void CXTPSkinObjectUser32Control::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CXTPSkinObjectFrame::OnChar(nChar, nRepCnt, nFlags);
	CheckScrollBarsDraw();
}

void CXTPSkinObjectUser32Control::OnHScroll(UINT /*nSBCode*/, UINT /*nPos*/,
											CScrollBar* /*pScrollBar*/)
{
	Default();
	CheckScrollBarsDraw();
}

void CXTPSkinObjectUser32Control::OnVScroll(UINT /*nSBCode*/, UINT /*nPos*/,
											CScrollBar* /*pScrollBar*/)
{
	Default();
	CheckScrollBarsDraw();
}

BOOL CXTPSkinObjectUser32Control::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam,
										   LRESULT* pResult)
{
	BOOL bResult = CXTPSkinObjectFrame::OnWndMsg(message, wParam, lParam, pResult);

	if (message == WM_MOUSEWHEEL)
	{
		CheckScrollBarsDraw();
	}

	return bResult;
}

void CXTPSkinObjectUser32Control::OnWindowPosChanged(WINDOWPOS FAR* /*lpwndpos*/)
{
	Default();
	CheckScrollBarsDraw();
}

//////////////////////////////////////////////////////////////////////////
// CXTPSkinObjectComCtl32Control

CXTPSkinObjectComCtl32Control::CXTPSkinObjectComCtl32Control()
{
}

void CXTPSkinObjectComCtl32Control::CheckScrollBarsDraw()
{
	if (!m_pManager->IsWin9x())
		return;

	CXTPSkinObjectUser32Control::CheckScrollBarsDraw();
}

//////////////////////////////////////////////////////////////////////////
// CXTPSkinObjectSolidFilled
