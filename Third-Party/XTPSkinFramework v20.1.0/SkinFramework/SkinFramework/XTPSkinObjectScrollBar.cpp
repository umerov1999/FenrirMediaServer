// XTPSkinObjectScrollBar.cpp: implementation of the CXTPSkinObjectScrollBar class.
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

#include "Common/XTPTypeId.h"
#include "Common/XTPFramework.h"
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
#include "SkinFramework/XTPSkinObjectFrame.h"
#include "SkinFramework/XTPSkinManager.h"
#include "SkinFramework/XTPSkinManagerSchema.h"
#include "SkinFramework/XTPSkinObjectScrollBar.h"

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

#ifdef _DEBUG
#	undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#	define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////////
// CXTPSkinObjectScrollBar

IMPLEMENT_DYNCREATE(CXTPSkinObjectScrollBar, CXTPSkinObjectFrame)

CXTPSkinObjectScrollBar::CXTPSkinObjectScrollBar()
{
	ZeroMemory(&m_spiCtl, sizeof(XTP_SKINSCROLLBAR_POSINFO));

	m_strClassName = _T("SCROLLBAR");
}

CXTPSkinObjectScrollBar::~CXTPSkinObjectScrollBar()
{
}

#include "Common/Base/Diagnostic/XTPBeginAfxMap.h"
BEGIN_MESSAGE_MAP(CXTPSkinObjectScrollBar, CXTPSkinObjectFrame)
	//{{AFX_MSG_MAP(CXTPSkinObjectScrollBar)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_MESSAGE(WM_PRINTCLIENT, OnPrintClient)
	ON_WM_ENABLE()
	ON_WM_MOUSEMOVE()
	ON_MESSAGE_VOID(WM_MOUSELEAVE, OnMouseLeave)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_MESSAGE(SBM_SETSCROLLINFO, OnSetScrollInfo)
	ON_MESSAGE(SBM_GETSCROLLINFO, OnGetScrollInfo)
	ON_MESSAGE(SBM_SETPOS, OnSetScrollPos)
	ON_WM_SETFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
#include "Common/Base/Diagnostic/XTPEndAfxMap.h"

/////////////////////////////////////////////////////////////////////////////
// CXTPSkinObjectScrollBar message handlers

int CXTPSkinObjectScrollBar::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CXTPSkinObjectFrame::OnCreate(lpCreateStruct) == -1)
		return -1;

	Invalidate(FALSE);

	return 0;
}

LRESULT CXTPSkinObjectScrollBar::OnPrintClient(WPARAM wParam, LPARAM /*lParam*/)
{
	if (!IsDrawingEnabled())
	{
		return Default();
	}

	CDC* pDC = CDC::FromHandle((HDC)wParam);
	if (pDC)
		OnDraw(pDC);
	return 1;
}

void CXTPSkinObjectScrollBar::OnPaint()
{
	if (IsDrawingEnabled())
	{
		CXTPSkinObjectPaintDC dc(this); // device context for painting
		OnDraw(&dc);
	}
	else
	{
		CXTPSkinObjectFrame::OnPaint();
	}
}

void CXTPSkinObjectScrollBar::OnDraw(CDC* pDC)
{
	CRect rc;
	GetClientRect(&rc);

	BOOL fSize = ((GetStyle() & (SBS_SIZEBOX | SBS_SIZEGRIP)) != 0);

	CXTPBufferDC dcMem(*pDC, rc);

	if (!fSize)
	{
		m_spiCtl.fVert	  = (0 != (GetStyle() & SBS_VERT));
		m_spiCtl.fSizebox = fSize;
		m_spiCtl.fVisible = TRUE;
		m_spiCtl.nBar	  = SB_CTL;

		if (!m_pSBTrack || !m_pSBTrack->bTrackThumb)
			SetupScrollInfo(&m_spiCtl);

		DrawScrollBar(&dcMem, &m_spiCtl);
	}
	else
	{
		CXTPSkinManagerClass* pClassScrollBar = GetSkinManager()->GetSkinClass(this,
																			   _T("SCROLLBAR"));
		FillBackground(&dcMem, rc);

		if (GetStyle() & SBS_SIZEGRIP)
		{
			pClassScrollBar->DrawThemeBackground(&dcMem, SBP_SIZEBOX, SZB_RIGHTALIGN, rc);
		}
	}
}

void CXTPSkinObjectScrollBar::OnEnable(BOOL bEnable)
{
	CXTPSkinObjectFrame::OnEnable(bEnable);
	Invalidate(FALSE);
}

void CXTPSkinObjectScrollBar::OnMouseLeave()
{
	OnMouseMove(0, CPoint(-1, -1));
}

void CXTPSkinObjectScrollBar::OnMouseMove(UINT /*nFlags*/, CPoint point)
{
	BOOL fSize = ((GetStyle() & (SBS_SIZEBOX | SBS_SIZEGRIP)) != 0);

	if (fSize)
		return;

	int ht = HitTestScrollBar(&m_spiCtl, point);

	if (ht != m_spiCtl.ht)
	{
		m_spiCtl.ht = ht;
		Invalidate(FALSE);

		if (m_spiCtl.ht != HTNOWHERE)
		{
			TRACKMOUSEEVENT tme = { sizeof(TRACKMOUSEEVENT), TME_LEAVE, m_hWnd, HOVER_DEFAULT };
			_TrackMouseEvent(&tme);
		}
	}
}

void CXTPSkinObjectScrollBar::OnLButtonDown(UINT nFlags, CPoint point)
{
	BOOL fSize = ((GetStyle() & (SBS_SIZEBOX | SBS_SIZEGRIP)) != 0);

	if (fSize)
	{
		CXTPSkinObjectFrame::OnLButtonDown(nFlags, point);
		return;
	}

	TrackInit(point, &m_spiCtl, (GetKeyState(VK_SHIFT) < 0) ? TRUE : FALSE);
}

void CXTPSkinObjectScrollBar::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	BOOL fSize = ((GetStyle() & (SBS_SIZEBOX | SBS_SIZEGRIP)) != 0);
	if (fSize)
	{
		CXTPSkinObjectFrame::OnLButtonDblClk(nFlags, point);
		return;
	}

	OnLButtonDown(nFlags, point);
}

LRESULT CXTPSkinObjectScrollBar::OnSetScrollInfo(WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult = DefWindowProc(SBM_SETSCROLLINFO, FALSE, lParam);

	if (wParam && IsWindowVisible())
	{
		CClientDC dc(this);
		OnDraw(&dc);
	}

	return lResult;
}

LRESULT CXTPSkinObjectScrollBar::OnGetScrollInfo(WPARAM wParam, LPARAM lParam)
{
	LRESULT bResult = DefWindowProc(SBM_GETSCROLLINFO, wParam, lParam);

	LPSCROLLINFO lpsi = (LPSCROLLINFO)lParam;

	if (lpsi && (lpsi->fMask & SIF_TRACKPOS) && m_pSBTrack)
	{
		lpsi->nTrackPos = m_pSBTrack->posNew;
	}

	return bResult;
}

LRESULT CXTPSkinObjectScrollBar::OnSetScrollPos(WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult = DefWindowProc(SBM_SETPOS, wParam, FALSE);

	if (lParam && IsWindowVisible())
	{
		CClientDC dc(this);
		OnDraw(&dc);
	}

	return lResult;
}

void CXTPSkinObjectScrollBar::OnSetFocus(CWnd* pOldWnd)
{
	pOldWnd;
}
