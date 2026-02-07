// XTPDrawHelpers.cpp: implementation of the CXTPDrawHelpers class.
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
#include "Common/Resource.h"
#include "Common/Base/cxminmax.h"

#include "Common/XTPTypeId.h"
#include "Common/XTPCasting.h"
#include "Common/XTPFramework.h"
#include "Common/XTPSystemHelpers.h"
#include "Common/XTPSynchro.h"
#include "Common/XTPApplication.h"
#include "Common/XTPSingleton.h"
#include "Common/XTPGdiObjects.h"
#include "Common/XTPColorManager.h"

#if defined(_XTP_INCLUDE_COMMANDBARS)
#	include "CommandBars/XTPCommandBarsDefines.h"
#	include "CommandBars/XTPPaintManager.h"
#endif

#include "Common/XTPDrawHelpers.h"
#include "Common/XTPVC80Helpers.h"
#include "Common/XTPResourceManager.h"
#include "Common/Math/XTPMathUtils.h"
#include "Common/IIDs/XTPPrintingIIDs.h"

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

#ifdef _DEBUG
#	undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#	define new DEBUG_NEW
#endif

HHOOK CXTPMouseMonitor::m_hHookMouse  = 0;
CWnd* CXTPMouseMonitor::m_pWndMonitor = 0;

typedef BOOL(WINAPI* PFNSETLAYEREDWINDOWATTRIBUTES)(HWND hwnd, COLORREF crKey, BYTE bAlpha,
													DWORD dwFlags);

#ifndef LWA_ALPHA
#	define LWA_ALPHA 0x00000002
#endif

#ifndef WS_EX_LAYERED
#	define WS_EX_LAYERED 0x00080000
#endif

#ifndef ANTIALIASED_QUALITY
#	define ANTIALIASED_QUALITY 4
#endif

#ifndef WM_DPICHANGED
#	define WM_DPICHANGED 0x02E0
#endif

#ifndef MONITOR_DEFAULTTONEAREST
#	define MONITOR_DEFAULTTONEAREST 0x00000002
#endif

//////////////////////////////////////////////////////////////////////////
// CXTPMouseMonitor
//////////////////////////////////////////////////////////////////////////

void CXTPMouseMonitor::SetupHook(CWnd* pWndMonitor)
{
	if (pWndMonitor && m_hHookMouse == 0)
	{
		m_hHookMouse = SetWindowsHookEx(WH_MOUSE, MouseProc, 0, GetCurrentThreadId());
	}
	if (!pWndMonitor && m_hHookMouse)
	{
		UnhookWindowsHookEx(m_hHookMouse);
		m_hHookMouse = 0;
	}
	m_pWndMonitor = pWndMonitor;
}

BOOL CXTPMouseMonitor::IsMouseHooked()
{
	return m_pWndMonitor != NULL;
}

LRESULT CALLBACK CXTPMouseMonitor::MouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode != HC_ACTION || !m_pWndMonitor)
		return CallNextHookEx(m_hHookMouse, nCode, wParam, lParam);

	CXTPWindowRect rc(m_pWndMonitor);

	if (!rc.PtInRect(((PMOUSEHOOKSTRUCT)lParam)->pt))
	{
		switch (wParam)
		{
			case WM_LBUTTONDOWN:
			case WM_NCLBUTTONDOWN:
			case WM_RBUTTONDOWN:
			case WM_NCRBUTTONDOWN:
			case WM_MBUTTONDOWN:
			case WM_NCMBUTTONDOWN: m_pWndMonitor->GetOwner()->SetFocus(); return TRUE;
		}
	}

	return CallNextHookEx(m_hHookMouse, nCode, wParam, lParam);
}

//===========================================================================
// CXTPClientCursorPos class
//===========================================================================

CXTPTransparentBitmap::CXTPTransparentBitmap(HBITMAP hBitmap)
	: m_hBitmap(hBitmap)
{
}

// Not foolproof, but works 99% of the time :).  Assumes the top
// left pixel is the transparent color.

COLORREF CXTPTransparentBitmap::GetTransparentColor() const
{
	CBitmap* pBitmap = CBitmap::FromHandle(m_hBitmap);
	if (pBitmap != NULL)
	{
		CXTPCompatibleDC dc(NULL, pBitmap);
		return dc.GetPixel(0, 0);
	}
	return (COLORREF)-1;
}

HICON CXTPTransparentBitmap::ConvertToIcon() const
{
	if (m_hBitmap == NULL)
		return NULL;

	COLORREF crTransparent = GetTransparentColor();

	BITMAP bmp;
	if (!::GetObject(m_hBitmap, sizeof(BITMAP), &bmp))
		return NULL;

	if (bmp.bmHeight == 0 || bmp.bmWidth == 0)
		return NULL;

	CImageList il;
	il.Create(bmp.bmWidth, bmp.bmHeight, ILC_COLOR24 | ILC_MASK, 0, 1);
	il.Add(CBitmap::FromHandle(m_hBitmap), crTransparent);

	ASSERT(il.GetImageCount() == 1);

	return il.ExtractIcon(0);
}

//===========================================================================
// CXTPClientCursorPos class
//===========================================================================

CXTPClientCursorPos::CXTPClientCursorPos(CWnd* pWnd)
{
	GetCursorPos(this);
	pWnd->ScreenToClient(this);
}

//===========================================================================
// CXTPEmptySize class
//===========================================================================

CXTPEmptySize::CXTPEmptySize()
{
	SetSizeEmpty();
}

void CXTPEmptySize::SetSizeEmpty()
{
	cx = 0;
	cy = 0;
}

const SIZE& CXTPEmptySize::operator=(const SIZE& srcSize)
{
	cx = srcSize.cx;
	cy = srcSize.cy;
	return *this;
}

//===========================================================================
// CXTPEmptyRect class
//===========================================================================

CXTPEmptyRect::CXTPEmptyRect()
{
	SetRectEmpty();
}

//===========================================================================
// CXTPWindowRect class
//===========================================================================

CXTPWindowRect::CXTPWindowRect(HWND hWnd)
{
	if (::IsWindow(hWnd))
		::GetWindowRect(hWnd, this);
	else
		SetRectEmpty();
}

CXTPWindowRect::CXTPWindowRect(const CWnd* pWnd)
{
	if (::IsWindow(pWnd->GetSafeHwnd()))
		::GetWindowRect(pWnd->GetSafeHwnd(), this);
	else
		SetRectEmpty();
}

//===========================================================================
// CXTPClientRect class
//===========================================================================

CXTPClientRect::CXTPClientRect(HWND hWnd)
{
	if (::IsWindow(hWnd))
		::GetClientRect(hWnd, this);
	else
		SetRectEmpty();
}

CXTPClientRect::CXTPClientRect(const CWnd* pWnd)
{
	if (::IsWindow(pWnd->GetSafeHwnd()))
		::GetClientRect(pWnd->GetSafeHwnd(), this);
	else
		SetRectEmpty();
}

//===========================================================================
// CXTPBufferDC class
//===========================================================================

CXTPBufferDC::CXTPBufferDC(HDC hDestDC, const CRect& rcPaint)
	: m_hDestDC(hDestDC)
	, m_hBitmap(NULL)
	, m_hOldBitmap(NULL)
{
	ASSERT(rcPaint.left <= rcPaint.right);
	ASSERT(rcPaint.top <= rcPaint.bottom);

	m_rect = rcPaint;
	Attach(::CreateCompatibleDC(m_hDestDC));
	if (NULL == m_hDC)
		return;

	m_hBitmap = ::CreateCompatibleBitmap(m_hDestDC, m_rect.right, m_rect.bottom);
	if (NULL == m_hBitmap)
		return;

	m_hOldBitmap = ::SelectObject(m_hDC, m_hBitmap);
}

CXTPBufferDC::CXTPBufferDC(HDC hDestDC, const CRect& rcPaint,
						   const CXTPPaintManagerColorGradient& clrBack,
						   const BOOL bHorz /*=FALSE*/)
	: m_hDestDC(hDestDC)
	, m_hBitmap(NULL)
	, m_hOldBitmap(NULL)
{
	ASSERT(rcPaint.left <= rcPaint.right);
	ASSERT(rcPaint.top <= rcPaint.bottom);

	m_rect = rcPaint;
	Attach(::CreateCompatibleDC(m_hDestDC));
	if (NULL == m_hDC)
		return;

	m_hBitmap = ::CreateCompatibleBitmap(m_hDestDC, m_rect.right, m_rect.bottom);
	if (NULL == m_hBitmap)
		return;

	m_hOldBitmap = ::SelectObject(m_hDC, m_hBitmap);

	if (!clrBack.IsNull())
		XTPDrawHelpers()->GradientFill(this, m_rect, clrBack, bHorz);
}

CXTPBufferDC::CXTPBufferDC(CPaintDC& paintDC)
	: m_hDestDC(paintDC.GetSafeHdc())
	, m_hBitmap(NULL)
	, m_rect(paintDC.m_ps.rcPaint)
	, m_hOldBitmap(NULL)
{
	ASSERT(m_rect.left <= m_rect.right);
	ASSERT(m_rect.top <= m_rect.bottom);

	Attach(::CreateCompatibleDC(m_hDestDC));
	if (NULL == m_hDC)
		return;

	m_hBitmap = ::CreateCompatibleBitmap(m_hDestDC, m_rect.right, m_rect.bottom);
	if (NULL == m_hBitmap)
		return;

	m_hOldBitmap = ::SelectObject(m_hDC, m_hBitmap);

	CRgn rgn;
	if (rgn.CreateRectRgnIndirect(&m_rect))
		SelectClipRgn(&rgn);
}

CXTPBufferDC::~CXTPBufferDC()
{
	if (NULL != m_hBitmap)
	{
		if (NULL != m_hDC)
		{
			if (NULL != m_hDestDC)
				::BitBlt(m_hDestDC, m_rect.left, m_rect.top, m_rect.Width(), m_rect.Height(), m_hDC,
						 m_rect.left, m_rect.top, SRCCOPY);

			VERIFY(
				(::SelectObject(m_hDC, m_hOldBitmap) == m_hBitmap)
				&& "Potential GDI leak detected: Another bitmap has been selected to the memory DC "
				   "and not released prior to buffer DC destruction.");
		}

		::DeleteObject(m_hBitmap);
	}
}

void CXTPBufferDC::Discard()
{
	m_hDestDC = NULL;
}

CDC* CXTPBufferDC::GetDestDC()
{
	ASSERT(NULL != m_hDestDC);
	return (NULL != m_hDestDC ? CDC::FromHandle(m_hDestDC) : NULL);
}

void CXTPBufferDC::TakeSnapshot()
{
	ASSERT(NULL != m_hDestDC);
	ASSERT(NULL != m_hDC);

	if (NULL != m_hDC && NULL != m_hDestDC)
		::BitBlt(m_hDC, 0, 0, m_rect.Width(), m_rect.Height(), m_hDestDC, m_rect.left, m_rect.top,
				 SRCCOPY);
}

//===========================================================================
// CXTPBufferDC class
//===========================================================================

CXTPBufferDCEx::CXTPBufferDCEx(HDC hDestDC, const CRect rcPaint)
	: m_hDestDC(hDestDC)
	, m_hBitmap(NULL)
	, m_hOldBitmap(NULL)
{
	ASSERT(rcPaint.left <= rcPaint.right);
	ASSERT(rcPaint.top <= rcPaint.bottom);

	m_rect = rcPaint;
	Attach(::CreateCompatibleDC(m_hDestDC));
	if (NULL == m_hDC)
		return;

	m_hBitmap = ::CreateCompatibleBitmap(m_hDestDC, m_rect.Width(), m_rect.Height());
	if (NULL == m_hBitmap)
		return;

	m_hOldBitmap = ::SelectObject(m_hDC, m_hBitmap);

	SetViewportOrg(-rcPaint.left, -rcPaint.top);
}

CXTPBufferDCEx::~CXTPBufferDCEx()
{
	if (NULL != m_hBitmap)
	{
		if (NULL != m_hDC)
		{
			SetViewportOrg(0, 0);

			if (NULL != m_hDestDC)
				::BitBlt(m_hDestDC, m_rect.left, m_rect.top, m_rect.Width(), m_rect.Height(), m_hDC,
						 0, 0, SRCCOPY);

			VERIFY((::SelectObject(m_hDC, m_hOldBitmap) == m_hBitmap)
				   && "Potential GDI leak detected: Another bitmap has been selected to the memory "
					  "DC and not "
					  "released.");
		}

		::DeleteObject(m_hBitmap);
	}
}

//===========================================================================
// CXTPBitmapDC class
//===========================================================================

CXTPBitmapDC::CXTPBitmapDC()
	: m_hOldBitmap(NULL)
	, m_hCurrentBitmap(NULL)
{
}

CXTPBitmapDC::CXTPBitmapDC(CDC* pDC, CBitmap* pBitmap)
	: m_hOldBitmap(NULL)
	, m_hCurrentBitmap(NULL)
{
	ASSERT_VALID(pDC);

	m_hDC = *pDC;

	if (NULL != pBitmap)
		SetBitmap(*pBitmap);
}

CXTPBitmapDC::CXTPBitmapDC(CDC* pDC, HBITMAP hBitmap)
	: m_hOldBitmap(NULL)
	, m_hCurrentBitmap(NULL)
{
	ASSERT_VALID(pDC);

	m_hDC = *pDC;
	SetBitmap(hBitmap);
}

CXTPBitmapDC::~CXTPBitmapDC()
{
	SelectOld();
}

void CXTPBitmapDC::SetBitmap(HBITMAP hBitmap)
{
	ASSERT(hBitmap != m_hOldBitmap);

	SelectOld();

	if (NULL != hBitmap)
	{
		m_hOldBitmap	 = HBITMAP(::SelectObject(m_hDC, hBitmap));
		m_hCurrentBitmap = hBitmap;
	}
}

void CXTPBitmapDC::SelectOld()
{
	if (NULL != m_hOldBitmap)
	{
		if (NULL != m_hDC)
		{
			VERIFY(
				(::SelectObject(m_hDC, m_hOldBitmap) == m_hCurrentBitmap)
				&& "Potential GDI leak detected: Another bitmap has been selected to the memory DC "
				   "and not released.");
		}
	}

	m_hOldBitmap	 = NULL;
	m_hCurrentBitmap = NULL;
}

//===========================================================================
// CXTPFontDC class
//===========================================================================

CXTPFontDC::CXTPFontDC(CDC* pDC, CFont* pFont)
	: m_pDC(pDC)
	, m_pOldFont(NULL)
	, m_pCurrentFont(NULL)
	, m_clrOldTextColor(COLORREF_NULL)
{
	ASSERT_VALID(pDC);

	SetFont(pFont);
}

CXTPFontDC::CXTPFontDC(CDC* pDC, CFont* pFont, COLORREF clrTextColor)
	: m_pDC(pDC)
	, m_pOldFont(NULL)
	, m_pCurrentFont(NULL)
	, m_clrOldTextColor(COLORREF_NULL)
{
	ASSERT_VALID(pDC);
	ASSERT(clrTextColor != COLORREF_NULL);

	SetFont(pFont);
	SetColor(clrTextColor);
}

CXTPFontDC::~CXTPFontDC()
{
	ReleaseFont();
	ReleaseColor();
}

void CXTPFontDC::SetFont(CFont* pFont)
{
	ReleaseFont();

	if (NULL != pFont)
	{
		m_pOldFont	 = m_pDC->SelectObject(pFont);
		m_pCurrentFont = pFont;
	}
}

void CXTPFontDC::SetColor(COLORREF clrTextColor)
{
	ASSERT(clrTextColor != COLORREF_NULL);

	if (COLORREF_NULL != clrTextColor)
	{
		COLORREF clrTextColorPrev = m_pDC->SetTextColor(clrTextColor);

		if (m_clrOldTextColor == COLORREF_NULL)
		{
			m_clrOldTextColor = clrTextColorPrev;
		}
	}
}

void CXTPFontDC::SetFontColor(CFont* pFont, COLORREF clrTextColor)
{
	SetFont(pFont);
	SetColor(clrTextColor);
}

void CXTPFontDC::ReleaseFont()
{
	if (NULL != m_pOldFont && NULL != m_pDC)
	{
#ifdef _DEBUG
		if (NULL != m_pCurrentFont)
		{
			VERIFY((m_pDC->SelectObject(m_pOldFont)->GetSafeHandle()
					== m_pCurrentFont->GetSafeHandle())
				   && "Potential GDI leak detected: Another font has been selected to the memory "
					  "DC and not released. In selected cases when this message has to be disabled "
					  "call CXTPFontDC::DisableHandleLeakDetection.");
		}
		else
		{
			m_pDC->SelectObject(m_pOldFont);
		}
#else
		m_pDC->SelectObject(m_pOldFont);
#endif
	}

	m_pOldFont	 = NULL;
	m_pCurrentFont = NULL;
}

void CXTPFontDC::ReleaseColor()
{
	if (COLORREF_NULL != m_clrOldTextColor)
	{
		m_pDC->SetTextColor(m_clrOldTextColor);
		m_clrOldTextColor = COLORREF_NULL;
	}
}

void CXTPFontDC::DisableHandleLeakDetection()
{
#ifdef _DEBUG
	m_pCurrentFont = NULL;
#endif
}

//===========================================================================
// CXTPPenDC class
//===========================================================================

CXTPPenDC::CXTPPenDC(CDC* pDC, CPen* pPen)
	: m_hOldPen(NULL)
	, m_hCurrentPen(NULL)
{
	ASSERT_VALID(pDC);

	m_hDC = *pDC;

	if (NULL != pPen)
	{
		m_hCurrentPen = *pPen;
		m_hOldPen	 = (HPEN)::SelectObject(m_hDC, m_hCurrentPen);
	}
}

CXTPPenDC::CXTPPenDC(HDC hDC, COLORREF crColor)
	: m_hOldPen(NULL)
	, m_hCurrentPen(NULL)
{
	Create(hDC, PS_SOLID, 1, crColor);
}

CXTPPenDC::CXTPPenDC(HDC hDC, int nStyle, COLORREF crColor)
	: m_hOldPen(NULL)
	, m_hCurrentPen(NULL)
{
	Create(hDC, nStyle, 1, crColor);
}

CXTPPenDC::CXTPPenDC(HDC hDC, int nStyle, int nWidth, COLORREF crColor)
	: m_hOldPen(NULL)
	, m_hCurrentPen(NULL)
{
	Create(hDC, nStyle, nWidth, crColor);
}

void CXTPPenDC::Create(HDC hDC, int nStyle, int nWidth, COLORREF crColor)
{
	ReleasePen();

	m_hDC	 = hDC;
	m_nStyle  = nStyle;
	m_nWidth  = nWidth;
	m_crColor = crColor;
	VERIFY(m_pen.CreatePen(m_nStyle, m_nWidth, m_crColor));
	m_hCurrentPen = m_pen;
	m_hOldPen	 = (HPEN)::SelectObject(m_hDC, m_hCurrentPen);
}

CXTPPenDC::~CXTPPenDC()
{
	ReleasePen();
}

void CXTPPenDC::ReleasePen()
{
	if (NULL != m_hOldPen && NULL != m_hDC)
	{
		VERIFY((::SelectObject(m_hDC, m_hOldPen) == m_hCurrentPen)
			   && "Potential GDI leak detected: Another pen has been selected to the memory DC and "
				  "not released.");
	}

	m_hOldPen	 = NULL;
	m_hCurrentPen = NULL;

	if (NULL != m_pen.m_hObject)
		VERIFY(m_pen.DeleteObject());
}

void CXTPPenDC::Color(COLORREF crColor)
{
	Create(m_hDC, m_nStyle, m_nWidth, crColor);
}

COLORREF CXTPPenDC::Color()
{
	LOGPEN logPen;
	m_pen.GetLogPen(&logPen);
	return logPen.lopnColor;
}

//===========================================================================
// CXTPBrushDC class
//===========================================================================

CXTPBrushDC::CXTPBrushDC(HDC hDC, COLORREF crColor)
	: m_hDC(hDC)
{
	VERIFY(m_brush.CreateSolidBrush(crColor));
	m_hOldBrush = (HBRUSH)::SelectObject(m_hDC, m_brush);
}

CXTPBrushDC::~CXTPBrushDC()
{
	ReleaseBrush();
}

void CXTPBrushDC::ReleaseBrush()
{
	if (NULL != m_hDC)
	{
		VERIFY((::SelectObject(m_hDC, m_hOldBrush) == m_brush.m_hObject)
			   && "Potential GDI leak detected: Another brush has been selected to the memory DC "
				  "and not "
				  "released.");
	}

	VERIFY(m_brush.DeleteObject());
}

void CXTPBrushDC::Color(COLORREF crColor)
{
	ReleaseBrush();

	VERIFY(m_brush.CreateSolidBrush(crColor));
	m_hOldBrush = (HBRUSH)::SelectObject(m_hDC, m_brush);
}

//===========================================================================
// CXTPDCSaveState class
//===========================================================================

CXTPDCSaveState::CXTPDCSaveState(CDC* pDC)
	: m_pDC(pDC)
	, m_nState(0)
{
	ASSERT(m_pDC);

	if (NULL != m_pDC)
		m_nState = m_pDC->SaveDC();
}

CXTPDCSaveState::~CXTPDCSaveState()
{
	if (NULL != m_pDC)
		m_pDC->RestoreDC(m_nState);
}

//===========================================================================
// CXTPCompatibleDC class
//===========================================================================

CXTPCompatibleDC::CXTPCompatibleDC(CDC* pDC, CBitmap* pBitmap)
	: m_hBitmap(reinterpret_cast<HBITMAP>(pBitmap->GetSafeHandle()))
{
	VERIFY(CreateCompatibleDC(pDC));
	m_hOldBitmap = reinterpret_cast<HBITMAP>(::SelectObject(GetSafeHdc(), m_hBitmap));
}

CXTPCompatibleDC::CXTPCompatibleDC(CDC* pDC, HBITMAP hBitmap)
	: m_hBitmap(hBitmap)
{
	VERIFY(CreateCompatibleDC(pDC));
	m_hOldBitmap = reinterpret_cast<HBITMAP>(::SelectObject(GetSafeHdc(), m_hBitmap));
}

CXTPCompatibleDC::~CXTPCompatibleDC()
{
	if (NULL != GetSafeHdc())
	{
		VERIFY((::SelectObject(GetSafeHdc(), m_hOldBitmap) == m_hBitmap)
			   && "Potential GDI leak detected: another bitmap has been selected into the device "
				  "context and not released prior to device context destruction");
		DeleteDC();
	}
}

//===========================================================================
// CXTPSplitterTracker class
//===========================================================================

CXTPSplitterTracker::CXTPSplitterTracker(BOOL bSolid /*= FALSE*/, BOOL bDesktopDC /*= TRUE*/)
{
	m_bSolid = bSolid;
	m_rcBoundRect.SetRectEmpty();
	m_pDC		   = 0;
	m_bDesktopDC   = bDesktopDC;
	m_pWnd		   = NULL;
	m_pSplitterWnd = NULL;

	m_pfnSetLayeredWindowAttributes = NULL;

	HMODULE hLib = GetModuleHandle(_T("USER32"));
	if (hLib)
	{
		m_pfnSetLayeredWindowAttributes = (PVOID)::GetProcAddress(hLib,
																  "SetLayeredWindowAttributes");
	}
}

void CXTPSplitterTracker::OnInvertTracker(CRect rect)
{
	ASSERT(!rect.IsRectEmpty());

	if (!m_bDesktopDC)
	{
		m_pWnd->ScreenToClient(rect);
	}

	if (m_pSplitterWnd)
	{
		m_pSplitterWnd->SetWindowPos(0, rect.left, rect.top, rect.Width(), rect.Height(),
									 SWP_SHOWWINDOW | SWP_NOZORDER | SWP_NOACTIVATE
										 | SWP_NOOWNERZORDER);
		return;
	}

	if (!m_pDC)
		return;

	if (m_bSolid)
	{
		m_pDC->InvertRect(rect);
	}
	else
	{
		CBrush* pDitherBrush = CDC::GetHalftoneBrush();
		CBrush* pBrush		 = (CBrush*)m_pDC->SelectObject(pDitherBrush);

		m_pDC->PatBlt(rect.left, rect.top, rect.Width(), rect.Height(), PATINVERT);
		m_pDC->SelectObject(pBrush);
	}
}

BOOL CXTPSplitterTracker::Track(CWnd* pTrackWnd, CRect rcAvail, CRect& rectTracker, CPoint point,
								BOOL bHoriz)
{
	pTrackWnd->SetCapture();
	m_pDC		   = 0;
	m_pSplitterWnd = NULL;

	if (m_rcBoundRect.IsRectEmpty() && m_bDesktopDC && XTPSystemVersion()->IsWinVistaOrGreater()
		&& m_pfnSetLayeredWindowAttributes && !XTPColorManager()->IsLowResolution())
	{
		m_pSplitterWnd = new CWnd();
		m_pSplitterWnd->CreateEx(WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TOOLWINDOW,
								 AfxRegisterWndClass(0, AfxGetApp()->LoadStandardCursor(IDC_ARROW),
													 (HBRUSH)GetStockObject(BLACK_BRUSH)),
								 0, WS_POPUP, CRect(0, 0, 0, 0), NULL, 0);

		((PFNSETLAYEREDWINDOWATTRIBUTES)m_pfnSetLayeredWindowAttributes)(m_pSplitterWnd->m_hWnd, 0,
																		 100, LWA_ALPHA);
	}
	else
	{
		if (m_bDesktopDC)
			m_pWnd = CWnd::GetDesktopWindow();
		else
			m_pWnd = pTrackWnd;

		if (m_pWnd->LockWindowUpdate())
			m_pDC = m_pWnd->GetDCEx(NULL, DCX_WINDOW | DCX_CACHE | DCX_LOCKWINDOWUPDATE);
		else
			m_pDC = m_pWnd->GetDCEx(NULL, DCX_WINDOW | DCX_CACHE);
		ASSERT(m_pDC != NULL);
	}

	CPoint ptOffset = bHoriz ? CPoint(rectTracker.left - point.x, 0)
							 : CPoint(0, rectTracker.top - point.y);

	OnInvertTracker(rectTracker);

	if (!m_rcBoundRect.IsRectEmpty())
		OnInvertTracker(m_rcBoundRect);

	BOOL bAccept = FALSE;
	while (CWnd::GetCapture() == pTrackWnd)
	{
		MSG msg;
		if (!GetMessage(&msg, NULL, 0, 0))
			break;

		if (msg.message == WM_MOUSEMOVE)
		{
			point = CPoint(msg.lParam);
			pTrackWnd->ClientToScreen(&point);
			point += ptOffset;

			point.x = CXTP_max(CXTP_min(point.x, rcAvail.right), rcAvail.left);
			point.y = CXTP_max(CXTP_min(point.y, rcAvail.bottom), rcAvail.top);

			if (bHoriz)
			{
				if (rectTracker.left != point.x)
				{
					OnInvertTracker(rectTracker);
					rectTracker.OffsetRect(point.x - rectTracker.left, 0);
					OnInvertTracker(rectTracker);
				}
			}
			else
			{
				if (rectTracker.top != point.y)
				{
					OnInvertTracker(rectTracker);
					rectTracker.OffsetRect(0, point.y - rectTracker.top);
					OnInvertTracker(rectTracker);
				}
			}
		}
		else if (msg.message == WM_KEYDOWN && msg.wParam == VK_ESCAPE)
			break;
		else if (msg.message == WM_LBUTTONUP)
		{
			bAccept = TRUE;
			break;
		}
		else
			::DispatchMessage(&msg);
	}

	if (!m_rcBoundRect.IsRectEmpty())
		OnInvertTracker(m_rcBoundRect);

	OnInvertTracker(rectTracker);

	if (CWnd::GetCapture() == pTrackWnd)
		ReleaseCapture();

	if (m_pSplitterWnd)
	{
		m_pSplitterWnd->DestroyWindow();

		delete m_pSplitterWnd;
		m_pSplitterWnd = NULL;
	}
	else
	{
		m_pWnd->UnlockWindowUpdate();
		if (m_pDC != NULL)
		{
			m_pWnd->ReleaseDC(m_pDC);
			m_pDC = NULL;
		}
	}

	return bAccept;
}

//===========================================================================
// CXTPDrawHelpers class
//===========================================================================

CXTPDrawHelpers::CXTPDrawHelpers()
{
	m_pfnFastGradientFill = 0;
	m_pfnAlphaBlend		  = 0;
	m_pfnTransparentBlt   = 0;

	// Don't use CXTPModuleHandle to reduce dependence between common source
	m_hMsImgDll = ::LoadLibrary(_T("msimg32.dll"));

	if (m_hMsImgDll)
	{
		m_pfnFastGradientFill = XTPToFunctionPtr<PFNGRADIENTFILL>(
			::GetProcAddress(m_hMsImgDll, "GradientFill"));
		m_pfnAlphaBlend = XTPToFunctionPtr<PFNALPHABLEND>(
			::GetProcAddress(m_hMsImgDll, "AlphaBlend"));
		m_pfnTransparentBlt = XTPToFunctionPtr<PFNTRANSPARENTBLT>(
			::GetProcAddress(m_hMsImgDll, "TransparentBlt"));
	}
}

CXTPDrawHelpers* AFX_CDECL XTPDrawHelpers()
{
	return &CXTPSingleton<CXTPDrawHelpers>::Instance();
}

CXTPDrawHelpers::~CXTPDrawHelpers()
{
	if (m_hMsImgDll != NULL)
	{
		//::FreeLibrary(m_hMsImgDll); Dangerous to call FreeLibrary in destructor of static object.
	}
}

void CXTPDrawHelpers::SolidRectangle(CDC* pDC, CRect rc, COLORREF clrRect, COLORREF clrFill)
{
	pDC->Draw3dRect(rc, clrRect, clrRect);
	rc.DeflateRect(1, 1, 1, 1);
	pDC->FillSolidRect(rc, clrFill);
}

BOOL CXTPDrawHelpers::GradientFill(HDC hdc, PTRIVERTEX pVertex, ULONG dwNumVertex, PVOID pMesh,
								   ULONG dwNumMesh, ULONG dwMode)
{
	if (m_pfnFastGradientFill)
	{
		return (*m_pfnFastGradientFill)(hdc, pVertex, dwNumVertex, pMesh, dwNumMesh, dwMode);
	}

	return FALSE;
}

void CXTPDrawHelpers::GradientFillSlow(CDC* pDC, LPCRECT lpRect, COLORREF crFrom, COLORREF crTo,
									   BOOL bHorz)
{
	int cx = CXTP_max(1, lpRect->right - lpRect->left);
	int cy = CXTP_max(1, lpRect->bottom - lpRect->top);

	CRect rc;
	pDC->GetClipBox(&rc);

	if (rc.IsRectEmpty())
		rc = *lpRect;
	else
		rc.IntersectRect(rc, lpRect);

	if (bHorz)
	{
		for (int nX = rc.left; nX < rc.right; nX++)
		{
			pDC->FillSolidRect(nX, rc.top, 1, rc.Height(),
							   BlendColors(crFrom, crTo,
										   (double)(1.0 - ((nX - lpRect->left) / (double)cx))));
		}
	}
	else
	{
		for (int nY = rc.top; nY < rc.bottom; nY++)
		{
			pDC->FillSolidRect(rc.left, nY, rc.Width(), 1,
							   BlendColors(crFrom, crTo,
										   (double)(1.0 - ((nY - lpRect->top)) / (double)cy)));
		}
	}
}

void CXTPDrawHelpers::GradientFillFast(CDC* pDC, LPCRECT lpRect, COLORREF crFrom, COLORREF crTo,
									   BOOL bHorz)
{
	TRIVERTEX vert[2];
	vert[0].x	 = lpRect->left;
	vert[0].y	 = lpRect->top;
	vert[0].Red   = (COLOR16)(GetRValue(crFrom) << 8);
	vert[0].Green = (COLOR16)(GetGValue(crFrom) << 8);
	vert[0].Blue  = (COLOR16)(GetBValue(crFrom) << 8);
	vert[0].Alpha = 0x0000;

	vert[1].x	 = lpRect->right;
	vert[1].y	 = lpRect->bottom;
	vert[1].Red   = (COLOR16)(GetRValue(crTo) << 8);
	vert[1].Green = (COLOR16)(GetGValue(crTo) << 8);
	vert[1].Blue  = (COLOR16)(GetBValue(crTo) << 8);
	vert[1].Alpha = 0x0000;

	GRADIENT_RECT gRect = { 0, 1 };

	GradientFill(*pDC, vert, 2, &gRect, 1,
				 static_cast<ULONG>(bHorz ? GRADIENT_FILL_RECT_H : GRADIENT_FILL_RECT_V));
}

void CXTPDrawHelpers::GradientFill(CDC* pDC, LPCRECT lpRect, COLORREF crFrom, COLORREF crTo,
								   BOOL bHorz)
{
	if (!lpRect)
		return;

	if (::IsRectEmpty(lpRect))
		return;

	if (IsLowResolution(pDC->GetSafeHdc()))
	{
		pDC->FillSolidRect(lpRect, crFrom);
	}
	else if (crFrom == crTo)
	{
		pDC->FillSolidRect(lpRect, crFrom);
	}
	else if ((m_pfnFastGradientFill == NULL)
			 || (IsContextRTL(pDC) && XTPSystemVersion()->IsWin9x()))
	{
		GradientFillSlow(pDC, lpRect, crFrom, crTo, bHorz);
	}
	else
	{
		GradientFillFast(pDC, lpRect, crFrom, crTo, bHorz);
	}
}

void CXTPDrawHelpers::GradientFill(CDC* pDC, LPCRECT lpRect, COLORREF crFrom, COLORREF crTo,
								   BOOL bHorz, LPCRECT lpRectClip)
{
	CRect rc(lpRect);

	if (lpRectClip == NULL)
	{
		GradientFill(pDC, lpRect, crFrom, crTo, bHorz);
		return;
	}

	COLORREF crFrom1 = crFrom;

	if (bHorz)
	{
		if (rc.top < lpRectClip->top)
		{
			rc.top = lpRectClip->top;
		}

		if (rc.bottom > lpRectClip->bottom)
		{
			rc.bottom = lpRectClip->bottom;
		}

		if ((rc.left > lpRectClip->right) || (rc.right < lpRectClip->left))
			return;

		if (rc.left < lpRectClip->left)
		{
			rc.left = lpRectClip->left;

			crFrom = BlendColors(crFrom, crTo,
								 (float)(lpRect->right - lpRectClip->left)
									 / (float)(lpRect->right - lpRect->left));
		}

		if (rc.right > lpRectClip->right)
		{
			rc.right = lpRectClip->right;

			crTo = BlendColors(crFrom1, crTo,
							   (float)(lpRect->right - lpRectClip->right)
								   / (float)(lpRect->right - lpRect->left));
		}

		GradientFill(pDC, rc, crFrom, crTo, bHorz);
	}
	else
	{
		if (rc.left < lpRectClip->left)
		{
			rc.left = lpRectClip->left;
		}

		if (rc.right > lpRectClip->right)
		{
			rc.right = lpRectClip->right;
		}

		if ((rc.top > lpRectClip->bottom) || (rc.bottom < lpRectClip->top))
			return;

		if (rc.top < lpRectClip->top)
		{
			rc.top = lpRectClip->top;

			crFrom = BlendColors(crFrom, crTo,
								 (float)(lpRect->bottom - lpRectClip->top)
									 / (float)(lpRect->bottom - lpRect->top));
		}

		if (rc.bottom > lpRectClip->bottom)
		{
			rc.bottom = lpRectClip->bottom;

			crTo = BlendColors(crFrom1, crTo,
							   (float)(lpRect->bottom - lpRectClip->bottom)
								   / (float)(lpRect->bottom - lpRect->top));
		}

		GradientFill(pDC, rc, crFrom, crTo, bHorz);
	}
}

void CXTPDrawHelpers::GradientFill(CDC* pDC, LPCRECT lpRect,
								   const CXTPPaintManagerColorGradient& grc, BOOL bHorz,
								   LPCRECT lpRectClip)
{
	// using gradient factor color gradient fill
	if (grc.fGradientFactor != 0.5f)
	{
		COLORREF clrMid = BlendColors(grc.clrLight, grc.clrDark, grc.fGradientFactor);

		if (bHorz)
		{
			CRect rcLeft(lpRect);
			rcLeft.right -= rcLeft.Width() / 2;
			GradientFill(pDC, &rcLeft, grc.clrLight, clrMid, bHorz, lpRectClip);

			CRect rcRight(lpRect);
			rcRight.left = rcLeft.right;
			GradientFill(pDC, &rcRight, clrMid, grc.clrDark, bHorz, lpRectClip);
		}
		else
		{
			CRect rcTop(lpRect);
			rcTop.bottom -= rcTop.Height() / 2;
			GradientFill(pDC, &rcTop, grc.clrLight, clrMid, bHorz, lpRectClip);

			CRect rcBottom(lpRect);
			rcBottom.top = rcTop.bottom;
			GradientFill(pDC, &rcBottom, clrMid, grc.clrDark, bHorz, lpRectClip);
		}
	}
	// using 2 color gradient fill
	else
	{
		GradientFill(pDC, lpRect, grc.clrLight, grc.clrDark, bHorz, lpRectClip);
	}
}

void AFX_CDECL CXTPDrawHelpers::DrawBorder(CDC* pDC, CRect rBorder, COLORREF* pOutsideClr,
										   COLORREF* pInsideClr /*=NULL*/)
{
	ASSERT(pDC && pOutsideClr);

	if (!pDC || !pOutsideClr)
		return;

	// draw outside border.
	pDC->FillSolidRect(rBorder.left, rBorder.top, XTP_DPI_X(1), rBorder.Height(),
					   pOutsideClr[0]); // left border
	pDC->FillSolidRect(rBorder.left + XTP_DPI_X(1), rBorder.top, rBorder.Width() - XTP_DPI_X(2),
					   XTP_DPI_Y(1), pOutsideClr[1]); // top border
	pDC->FillSolidRect(rBorder.right - XTP_DPI_X(1), rBorder.top, XTP_DPI_X(1), rBorder.Height(),
					   pOutsideClr[2]); // right border
	pDC->FillSolidRect(rBorder.left + XTP_DPI_X(1), rBorder.bottom - XTP_DPI_Y(1),
					   rBorder.Width() - XTP_DPI_X(2), XTP_DPI_Y(1),
					   pOutsideClr[3]); // bottom border

	if (!pInsideClr)
		return;

	rBorder.DeflateRect(XTP_DPI_X(1), XTP_DPI_Y(1));

	// draw inside border.
	pDC->FillSolidRect(rBorder.left, rBorder.top, XTP_DPI_X(1), rBorder.Height(),
					   pInsideClr[0]); // left border
	pDC->FillSolidRect(rBorder.left + XTP_DPI_X(1), rBorder.top, rBorder.Width() - XTP_DPI_X(2),
					   XTP_DPI_Y(1), pInsideClr[1]); // top border
	pDC->FillSolidRect(rBorder.right - XTP_DPI_X(1), rBorder.top, XTP_DPI_X(1), rBorder.Height(),
					   pInsideClr[2]); // right border
	pDC->FillSolidRect(rBorder.left + XTP_DPI_X(1), rBorder.bottom - XTP_DPI_Y(1),
					   rBorder.Width() - XTP_DPI_X(2), XTP_DPI_Y(1),
					   pInsideClr[3]); // bottom border
}

void CXTPDrawHelpers::ExcludeCorners(CDC* pDC, CRect rc, BOOL bTopCornersOnly /*= FALSE*/)
{
	pDC->ExcludeClipRect(rc.left, rc.top, rc.left + XTP_DPI_X(1), rc.top + XTP_DPI_Y(1));
	pDC->ExcludeClipRect(rc.right - XTP_DPI_X(1), rc.top, rc.right, rc.top + XTP_DPI_Y(1));

	if (bTopCornersOnly == FALSE)
	{
		pDC->ExcludeClipRect(rc.left, rc.bottom - XTP_DPI_Y(1), rc.left + XTP_DPI_X(1), rc.bottom);
		pDC->ExcludeClipRect(rc.right - XTP_DPI_X(1), rc.bottom - XTP_DPI_Y(1), rc.right,
							 rc.bottom);
	}
}

void CXTPDrawHelpers::StripMnemonics(CString& strClear)
{
	for (int i = 0; i < strClear.GetLength(); i++)
	{
		if (strClear[i] == _T('&')) // Converts "&&" to "&" and "&&&&" to "&&"
		{
			strClear.Delete(i);
		}
	}
}

void CXTPDrawHelpers::StripMnemonics(LPTSTR lpszClear)
{
	if (lpszClear == NULL || lpszClear == LPSTR_TEXTCALLBACK)
		return;

	LPTSTR lpszResult = lpszClear;

	while (*lpszClear)
	{
		if (*lpszClear == _T('&') && *(lpszClear + 1) != _T('&'))
		{
			lpszClear++;
			continue;
		}

		*lpszResult++ = *lpszClear++;
	}

	*lpszResult = 0;
}

void CXTPDrawHelpers::BlurPoints(CDC* pDC, LPPOINT pts, int nCount)
{
	for (int i = 0; i < nCount; i += 2)
	{
		CPoint ptBlur = pts[i];
		CPoint ptDirection(pts[i].x + pts[i + 1].x, pts[i].y + pts[i + 1].y);

		COLORREF clrBlur	  = pDC->GetPixel(ptDirection);
		COLORREF clrDirection = pDC->GetPixel(ptBlur);

		pDC->SetPixel(ptBlur, RGB((GetRValue(clrBlur) + GetRValue(clrDirection)) / 2,
								  (GetGValue(clrBlur) + GetGValue(clrDirection)) / 2,
								  (GetBValue(clrBlur) + GetBValue(clrDirection)) / 2));
	}
}

COLORREF CXTPDrawHelpers::BlendColors(COLORREF crA, COLORREF crB, double fAmountA)
{
	double fAmountB = (1.0 - fAmountA);
	int btR			= (int)(GetRValue(crA) * fAmountA + GetRValue(crB) * fAmountB);
	int btG			= (int)(GetGValue(crA) * fAmountA + GetGValue(crB) * fAmountB);
	int btB			= (int)(GetBValue(crA) * fAmountA + GetBValue(crB) * fAmountB);

	return RGB(CXTP_min(255, btR), (BYTE)CXTP_min(255, btG), (BYTE)CXTP_min(255, btB));
}

COLORREF CXTPDrawHelpers::DarkenColor(long lScale, COLORREF lColor)
{
	long red   = ::MulDiv(GetRValue(lColor), (255 - lScale), 255);
	long green = ::MulDiv(GetGValue(lColor), (255 - lScale), 255);
	long blue  = ::MulDiv(GetBValue(lColor), (255 - lScale), 255);

	return RGB(red, green, blue);
}

COLORREF CXTPDrawHelpers::LightenColor(long lScale, COLORREF lColor)
{
	long R = ::MulDiv(255 - GetRValue(lColor), lScale, 255) + GetRValue(lColor);
	long G = ::MulDiv(255 - GetGValue(lColor), lScale, 255) + GetGValue(lColor);
	long B = ::MulDiv(255 - GetBValue(lColor), lScale, 255) + GetBValue(lColor);

	return RGB(R, G, B);
}

CPoint CXTPDrawHelpers::Dlu2Pix(int dluX, int dluY)
{
	CPoint baseXY(::GetDialogBaseUnits());
	CPoint pixXY(0, 0);
	pixXY.x = ::MulDiv(dluX, baseXY.x, 4);
	pixXY.y = ::MulDiv(dluY, baseXY.y, 8);
	return pixXY;
}

COLORREF CXTPDrawHelpers::RGBtoHSL(COLORREF rgb)
{
	int delta, sum;
	int nH, nS, nL;
	int r	= GetRValue(rgb);
	int g	= GetGValue(rgb);
	int b	= GetBValue(rgb);
	int cmax = ((r) >= (g) ? ((r) >= (b) ? (r) : (b)) : (g) >= (b) ? (g) : (b));
	int cmin = ((r) <= (g) ? ((r) <= (b) ? (r) : (b)) : (g) <= (b) ? (g) : (b));

	nL = (cmax + cmin + 1) / 2;
	if (cmax == cmin)
	{
		nH = 255; // H is really undefined
		nS = 0;
	}
	else
	{
		delta = cmax - cmin;
		sum   = cmax + cmin;
		if (nL < 127)
			nS = ((delta + 1) * 256) / sum;
		else
			nS = (delta * 256) / ((2 * 256) - sum);
		if (r == cmax)
			nH = ((g - b) * 256) / delta;
		else if (g == cmax)
			nH = (2 * 256) + ((b - r) * 256) / delta;
		else
			nH = (4 * 256) + ((r - g) * 256) / delta;
		nH /= 6;
		if (nH < 0)
			nH += 256;
	}
	nH = nH * 239 / 255;
	nS = nS * 240 / 255;
	nL = nL * 240 / 255;

	return RGB((BYTE)CXTP_min(nH, 239), (BYTE)CXTP_min(nS, 240), (BYTE)CXTP_min(nL, 240));
}

void AFX_CDECL CXTPDrawHelpers::RGBtoHSL(COLORREF clr, double& h, double& s, double& l)
{
	double r = (double)GetRValue(clr) / 255.0;
	double g = (double)GetGValue(clr) / 255.0;
	double b = (double)GetBValue(clr) / 255.0;

	double maxcolor = __max(r, __max(g, b));
	double mincolor = __min(r, __min(g, b));

	l = (maxcolor + mincolor) / 2;

	if (maxcolor == mincolor)
	{
		h = 0;
		s = 0;
	}
	else
	{
		if (l < 0.5)
			s = (maxcolor - mincolor) / (maxcolor + mincolor);
		else
			s = (maxcolor - mincolor) / (2.0 - maxcolor - mincolor);

		if (r == maxcolor)
			h = (g - b) / (maxcolor - mincolor);
		else if (g == maxcolor)
			h = 2.0 + (b - r) / (maxcolor - mincolor);
		else
			h = 4.0 + (r - g) / (maxcolor - mincolor);

		h /= 6.0;
		if (h < 0.0)
			h += 1;
	}
}

double CXTPDrawHelpers::HueToRGB(double temp1, double temp2, double temp3)
{
	if (temp3 < 0)
		temp3 = temp3 + 1.0;
	if (temp3 > 1)
		temp3 = temp3 - 1.0;

	if (6.0 * temp3 < 1)
		return (temp1 + (temp2 - temp1) * temp3 * 6.0);

	else if (2.0 * temp3 < 1)
		return temp2;

	else if (3.0 * temp3 < 2.0)
		return (temp1 + (temp2 - temp1) * ((2.0 / 3.0) - temp3) * 6.0);

	return temp1;
}

int CXTPDrawHelpers::HueToRGB(int m1, int m2, int h)
{
	if (h < 0)
		h += 255;

	if (h > 255)
		h -= 255;

	if ((6 * h) < 255)
		return ((m1 + ((m2 - m1) / 255 * h * 6)) / 255);

	if ((2 * h) < 255)
		return m2 / 255;

	if ((3 * h) < (2 * 255))
		return ((m1 + (m2 - m1) / 255 * ((255 * 2 / 3) - h) * 6) / 255);

	return (m1 / 255);
}

COLORREF CXTPDrawHelpers::HSLtoRGB(COLORREF hsl)
{
	int r, g, b;
	int m1, m2;
	int nH = GetRValue(hsl) * 255 / 239;
	int nS = GetGValue(hsl) * 255 / 240;
	int nL = GetBValue(hsl) * 255 / 240;

	if (nS == 0)
		r = g = b = nL;
	else
	{
		if (nL <= 127)
			m2 = nL * (255 + nS);
		else
			m2 = (nL + nS - ((nL * nS) / 255)) * 255;
		m1 = (2 * 255 * nL) - m2;
		r  = HueToRGB(m1, m2, nH + (255 / 3));
		g  = HueToRGB(m1, m2, nH);
		b  = HueToRGB(m1, m2, nH - (255 / 3));
	}
	return RGB((BYTE)CXTP_min(r, 255), (BYTE)CXTP_min(g, 255), (BYTE)CXTP_min(b, 255));
}

COLORREF CXTPDrawHelpers::HSLtoRGB(double h, double s, double l)
{
	double r, g, b;
	double temp1, temp2;

	if (s == 0)
	{
		r = g = b = l;
	}
	else
	{
		if (l < 0.5)
			temp2 = l * (1.0 + s);
		else
			temp2 = l + s - l * s;

		temp1 = 2.0 * l - temp2;

		r = HueToRGB(temp1, temp2, h + 1.0 / 3.0);
		g = HueToRGB(temp1, temp2, h);
		b = HueToRGB(temp1, temp2, h - 1.0 / 3.0);
	}

	return RGB((BYTE)(r * 255), (BYTE)(g * 255), (BYTE)(b * 255));
}

COLORREF CXTPDrawHelpers::AdjustBrightness(COLORREF clr, int nPercent)
{
	ASSERT((nPercent >= -100) && (nPercent <= 100));

	// must be a value between -100 and +100.
	if (nPercent < -100 || nPercent > 100)
		return COLORREF_NULL;

	// if 0 no change is necessary.
	if (nPercent == 0)
		return clr;

	double dHue, dSat, dLum;
	RGBtoHSL(clr, dHue, dSat, dLum);

	return HSLtoRGB(dHue, dSat,
					(nPercent > 0) ? (dLum + (1.0 - dLum) * ((double)nPercent / 100.0))
								   : (dLum * (1.0 + ((double)nPercent / 100.0))));
}

COLORREF AFX_CDECL CXTPDrawHelpers::DarkenColorLum(COLORREF clr, double dPercent)
{
	ASSERT(dPercent > 0.0 && dPercent < 1.0);

	// convert RGB value to HSL.
	DWORD dwHSL = CXTPDrawHelpers::RGBtoHSL(clr);

	// get the hue, saturation and luminosity values.
	double dHue = (double)GetRValue(dwHSL);
	double dSat = (double)GetGValue(dwHSL);
	double dLum = (double)GetBValue(dwHSL) * dPercent;

	// adjust the luminosity to darken the color.
	dLum = CXTPMathUtils::Round(dLum);

	// convert the HSL value back to RGB.
	return CXTPDrawHelpers::HSLtoRGB(RGB((BYTE)dHue, (BYTE)dSat, (BYTE)dLum));
}

COLORREF AFX_CDECL CXTPDrawHelpers::GreyScaleColor(COLORREF clr)
{
	double r = GetRValue(clr) * 0.30;
	double g = GetGValue(clr) * 0.59;
	double b = GetBValue(clr) * 0.11;

	BYTE chGrey = BYTE(r + g + b);

	return RGB(chGrey, chGrey, chGrey);
}

static int CALLBACK XTPEnumFontFamExProc(const LOGFONT* lpelfe, const TEXTMETRIC* lpntme,
										 DWORD FontType, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lpntme);
	UNREFERENCED_PARAMETER(FontType);

	const ENUMLOGFONTEX* pelf = reinterpret_cast<const ENUMLOGFONTEX*>(lpelfe);
	ASSERT(NULL != pelf);

	LPCTSTR strFontName = reinterpret_cast<LPCTSTR>(lParam);
	ASSERT(NULL != strFontName);

	CString strFaceName = pelf->elfLogFont.lfFaceName;

	return (strFaceName.CompareNoCase(strFontName) == 0 ? 0 : 1);
}

BOOL CXTPDrawHelpers::FontExists(LPCTSTR strFaceName)
{
	// Enumerate all styles and charsets of all fonts:
	LOGFONT lfEnum;
	::ZeroMemory(&lfEnum, sizeof(LOGFONT));

	lfEnum.lfFaceName[0] = 0;
	lfEnum.lfCharSet	 = DEFAULT_CHARSET;

	CWindowDC dc(NULL);

	return ::EnumFontFamiliesEx(dc.m_hDC, &lfEnum, XTPEnumFontFamExProc, (LPARAM)strFaceName, 0)
		   == 0;
}

CString CXTPDrawHelpers::GetDefaultFontName()
{
	LOGFONT lfFont;
	ZeroMemory(&lfFont, sizeof(LOGFONT));
	::GetObject(::GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONT), &lfFont);

	return CString(lfFont.lfFaceName);
}

int AFX_CDECL CXTPDrawHelpers::DrawFontText(CDC* pDC, LPCTSTR lpszFontFaceName, int nFontHeight,
											LPCTSTR lpchText, int nCount, LPRECT lpRect,
											UINT uFormat, COLORREF crForeColor /*= COLORREF_NULL*/,
											COLORREF crBackColor /*= COLORREF_NULL*/)
{
	ASSERT(pDC);
	ASSERT(NULL != lpszFontFaceName);
	ASSERT(NULL != lpchText);

	int nResult = 0;

	if (0 != nFontHeight)
	{
		CXTPFont font;
		if (font.CreatePointFont(nFontHeight, lpszFontFaceName))
		{
			nResult = DrawFontText(pDC, &font, lpchText, nCount, lpRect, uFormat, crForeColor,
								   crBackColor);
		}
	}
	else
	{
		CXTPFont fntTest;
		if (fntTest.CreatePointFont(80, lpszFontFaceName))
		{
			int nTextLen  = (int)(0 <= nCount ? nCount : _tcslen(lpchText));
			int nFontSize = ComputeFittingFontHeight(pDC, lpchText, nTextLen, *lpRect, fntTest);
			if (0 != nFontSize)
			{
				LOGFONT fontData;
				if (0 < GetObject(fntTest, sizeof(fontData), &fontData))
				{
					fontData.lfHeight = nFontSize;
					CXTPFont font;
					if (font.CreateFontIndirect(&fontData))
					{
						nResult = DrawFontText(pDC, &font, lpchText, nTextLen, lpRect,
											   DT_CENTER | DT_VCENTER | DT_SINGLELINE, crForeColor,
											   crBackColor);
					}
				}
			}
		}
	}

	return nResult;
}

int AFX_CDECL CXTPDrawHelpers::DrawFontText(CDC* pDC, CFont* pFont, LPCTSTR lpchText, int nCount,
											LPRECT lpRect, UINT uFormat,
											COLORREF crForeColor /*= COLORREF_NULL*/,
											COLORREF crBackColor /*= COLORREF_NULL*/)
{
	ASSERT(pDC);
	ASSERT(pFont);
	ASSERT(NULL != lpchText);

	int nResult = 0;

	// Set attributes.
	COLORREF crOldForeColor = pDC->GetTextColor();
	if (COLORREF_NULL != crForeColor)
	{
		pDC->SetTextColor(crForeColor);
	}

	int nOldBkMode = nOldBkMode = pDC->GetBkMode();
	COLORREF crOldBackColor		= 0;
	if (COLORREF_NULL != crBackColor)
	{
		crOldBackColor = pDC->GetBkColor();
		pDC->SetBkColor(crBackColor);
		pDC->SetBkMode(OPAQUE);
	}
	else
	{
		pDC->SetBkMode(TRANSPARENT);
	}

	// Draw text using provided font.
	CFont* pOldFont = pDC->SelectObject(pFont);
	nResult			= pDC->DrawText(lpchText, nCount, lpRect, uFormat);
	pDC->SelectObject(pOldFont);

	// Restore attributes
	if (COLORREF_NULL != crBackColor)
	{
		pDC->SetBkColor(crOldBackColor);
	}
	pDC->SetBkMode(nOldBkMode);
	pDC->SetTextColor(crOldForeColor);

	return nResult;
}

CString AFX_CDECL CXTPDrawHelpers::GetVerticalFontName(BOOL bUseOfficeFont)
{
	LOGFONT lfFont;
	ZeroMemory(&lfFont, sizeof(LOGFONT));
	::GetObject(::GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONT), &lfFont);
	bool bUseSystemFont = lfFont.lfCharSet > SYMBOL_CHARSET;

	if (bUseSystemFont && !XTPSystemVersion()->IsWin2KOrGreater())
		bUseSystemFont = FALSE;

	if (bUseSystemFont && (_tcsicmp(lfFont.lfFaceName, _T("MS Shell Dlg")) == 0))
		bUseSystemFont = FALSE; // Can draw it vertically in Windows 2000.

	CString strVerticalFaceName = _T("Arial");
	LPCTSTR strOfficeFont		= _T("Tahoma");

	if (bUseSystemFont || !FontExists(strVerticalFaceName))
	{
		strVerticalFaceName = lfFont.lfFaceName;
	}
	else if (bUseOfficeFont && !bUseSystemFont && FontExists(strOfficeFont))
	{
		strVerticalFaceName = strOfficeFont;
	}

	return strVerticalFaceName;
}

DWORD CXTPDrawHelpers::GetLayout(CDC* pDC)
{
	DWORD dwLayout = 0;

	if (NULL != pDC)
	{
		dwLayout = GetLayout(pDC->GetSafeHdc());
	}

	return dwLayout;
}

DWORD CXTPDrawHelpers::GetLayout(HDC hDC)
{
	typedef DWORD(CALLBACK * PFNGDIGETLAYOUTPROC)(HDC);
	static PFNGDIGETLAYOUTPROC s_pfnGetLayout = NULL;

	if (NULL == s_pfnGetLayout)
	{
		HINSTANCE hInstance = ::GetModuleHandle(_T("GDI32.DLL"));
		s_pfnGetLayout		= XTPToFunctionPtr<PFNGDIGETLAYOUTPROC>(
			 ::GetProcAddress(hInstance, "GetLayout"));
		ASSERT(NULL != s_pfnGetLayout); // No entry point for GetLayout
	}

	DWORD dwLayout = 0;

	if (NULL != s_pfnGetLayout)
	{
		dwLayout = s_pfnGetLayout(hDC);
	}

	return dwLayout;
}

DWORD CXTPDrawHelpers::SetLayout(CDC* pDC, DWORD dwLayout)
{
	DWORD dwPrevLayout = 0;

	if (NULL != pDC)
	{
		dwPrevLayout = SetLayout(pDC->GetSafeHdc(), dwLayout);
	}

	return dwPrevLayout;
}

DWORD CXTPDrawHelpers::SetLayout(HDC hDC, DWORD dwLayout)
{
	typedef DWORD(CALLBACK * PFNGDISETLAYOUTPROC)(HDC, DWORD);
	static PFNGDISETLAYOUTPROC s_pfn = (PFNGDISETLAYOUTPROC)-1;

	if (s_pfn == (PFNGDISETLAYOUTPROC)-1)
	{
		HINSTANCE hInst = ::GetModuleHandleA("GDI32.DLL");

		s_pfn = (hInst ? XTPToFunctionPtr<PFNGDISETLAYOUTPROC>(::GetProcAddress(hInst, "SetLayout"))
					   : NULL);
	}

	DWORD dwPrevLayout = 0;

	if (s_pfn != NULL)
	{
		dwPrevLayout = (*s_pfn)(hDC, dwLayout);
	}

	return dwPrevLayout;
}

BOOL CXTPDrawHelpers::IsContextRTL(CDC* pDC)
{
	BOOL bIsContextRTL = FALSE;

	if (NULL != pDC)
	{
		bIsContextRTL = IsContextRTL(pDC->GetSafeHdc());
	}

	return bIsContextRTL;
}

BOOL CXTPDrawHelpers::IsContextRTL(HDC hDC)
{
	DWORD dwLayout = GetLayout(hDC);
	return (LAYOUT_RTL == dwLayout);
}

void CXTPDrawHelpers::SetContextRTL(CDC* pDC, DWORD dwLayoutRTL)
{
	ASSERT_VALID(pDC);

	if (NULL != pDC)
		SetContextRTL(pDC->GetSafeHdc(), dwLayoutRTL);
}

void CXTPDrawHelpers::SetContextRTL(HDC hDC, DWORD dwLayoutRTL)
{
	typedef DWORD(CALLBACK * PFNGDISETLAYOUTPROC)(HDC, DWORD);
	static PFNGDISETLAYOUTPROC s_pfn = (PFNGDISETLAYOUTPROC)-1;

	if (s_pfn == (PFNGDISETLAYOUTPROC)-1)
	{
		HINSTANCE hInst = ::GetModuleHandleA("GDI32.DLL");

		s_pfn = (hInst ? XTPToFunctionPtr<PFNGDISETLAYOUTPROC>(::GetProcAddress(hInst, "SetLayout"))
					   : NULL);
	}

	if (s_pfn != NULL)
	{
		(*s_pfn)(hDC, dwLayoutRTL);
	}
}

BOOL AFX_CDECL CXTPDrawHelpers::IsWindowRTL(CWnd* pWnd)
{
	ASSERT_VALID(pWnd);
	return IsWindowRTL(pWnd->GetSafeHwnd());
}

BOOL AFX_CDECL CXTPDrawHelpers::IsWindowRTL(HWND hWnd)
{
	// Accoring to: https://msdn.microsoft.com/en-us/library/windows/desktop/ee264314(v=vs.85).aspx
	LONG lExStyle	= ::GetWindowLong(hWnd, GWL_EXSTYLE);
	BOOL bRtlReading = (0 != (lExStyle & WS_EX_RTLREADING));
	BOOL bLayoutRtl  = (0 != (lExStyle & WS_EX_LAYOUTRTL));
	return (bRtlReading && !bLayoutRtl) || (!bRtlReading && bLayoutRtl);
}

BOOL AFX_CDECL CXTPDrawHelpers::IsVScrollOnTheLeft(CWnd* pWnd)
{
	ASSERT_VALID(pWnd);
	return IsVScrollOnTheLeft(pWnd->GetSafeHwnd());
}

BOOL AFX_CDECL CXTPDrawHelpers::IsVScrollOnTheLeft(HWND hWnd)
{
	LONG lExStyle = ::GetWindowLong(hWnd, GWL_EXSTYLE);
	return (0 != (lExStyle & (WS_EX_LAYOUTRTL | WS_EX_LEFTSCROLLBAR)));
}

void CXTPDrawHelpers::KeyToLayout(CWnd* pWnd, UINT& nChar)
{
	ASSERT(pWnd);
	if (!pWnd || !pWnd->GetSafeHwnd())
		return;

	if (nChar == VK_LEFT && pWnd->GetExStyle() & WS_EX_LAYOUTRTL)
		nChar = VK_RIGHT;
	else if (nChar == VK_RIGHT && pWnd->GetExStyle() & WS_EX_LAYOUTRTL)
		nChar = VK_LEFT;
}

void CXTPDrawHelpers::Triangle(CDC* pDC, CPoint pt0, CPoint pt1, CPoint pt2, COLORREF clrPen,
							   COLORREF clrBrush /*=COLORREF_NULL*/)
{
	if (clrBrush == COLORREF_NULL)
		clrBrush = clrPen;

	CXTPPenDC pen(*pDC, clrPen);
	CXTPBrushDC brush(*pDC, clrBrush);

	Triangle(pDC, pt0, pt1, pt2);
}

void CXTPDrawHelpers::DrawExpandTriangle(CDC* pDC, CPoint pt, BOOL bExpanded, COLORREF clrPen,
										 COLORREF clrBrush /*=COLORREF_NULL*/)
{
	if (bExpanded)
	{
		CPoint pt1(pt.x, pt.y + XTP_DPI_Y(5));
		CPoint pt2(pt.x + XTP_DPI_X(5), pt.y + XTP_DPI_Y(5));
		CPoint pt3(pt.x + XTP_DPI_X(5), pt.y);

		Triangle(pDC, pt1, pt2, pt3, clrPen, clrBrush);
	}
	else
	{
		CPoint pt1(pt.x, pt.y);
		CPoint pt2(pt.x, pt.y + XTP_DPI_Y(8));
		CPoint pt3(pt.x + XTP_DPI_X(4), pt.y + XTP_DPI_Y(4));

		Triangle(pDC, pt1, pt2, pt3, clrPen, clrBrush);
	}
}

void CXTPDrawHelpers::DrawExpandArrow(CDC* pDC, CRect rc, BOOL bExpanded, COLORREF clrArrow)
{
	ASSERT_VALID(pDC);

	const int width  = XTP_DPI_X(5);
	const int height = XTP_DPI_Y(7);

	const CPoint pt = rc.CenterPoint();
	int x			= pt.x;
	int y			= pt.y;

	int dx				= XTP_DPI_X(3);
	const int shortLine = height / 2 - XTP_DPI_X(1);

	CPen pen;
	if (pen.CreatePen(PS_SOLID, 1, clrArrow))
	{
		CPen* pOldPen = pDC->SelectObject(&pen);
		if (pOldPen)
		{
			if (bExpanded)
			{
				x -= width / 2;
				pDC->MoveTo(x, y);
				pDC->LineTo(x + XTP_DPI_X(3), y);
				x++;

				for (int dy = 1; dy <= height / 2; dy++)
				{
					if (dy > shortLine)
						dx = XTP_DPI_X(3) - (dy - shortLine);

					pDC->MoveTo(x, y + dy);
					pDC->LineTo(x + dx, y + dy);
					pDC->MoveTo(x, y - dy);
					pDC->LineTo(x + dx, y - dy);
					x++;
				}
			}
			else
			{
				pDC->MoveTo(x, y);
				pDC->LineTo(x + XTP_DPI_X(3), y);
				x--;

				for (int dy = 1; dy <= height / 2; dy++)
				{
					if (dy > shortLine)
					{
						dx = XTP_DPI_X(3) - (dy - shortLine);
						x++;
					}

					pDC->MoveTo(x, y + dy);
					pDC->LineTo(x + dx, y + dy);
					pDC->MoveTo(x, y - dy);
					pDC->LineTo(x + dx, y - dy);
					x--;
				}
			}

			pDC->SelectObject(pOldPen);
		}
		else
		{
			ASSERT(FALSE);
		}

		pen.DeleteObject();
	}
	else
	{
		ASSERT(FALSE);
	}
}

BOOL CXTPDrawHelpers::DrawLine(CDC* pDC, int x1, int y1, int x2, int y2, COLORREF crLine)
{
	if (pDC->GetSafeHdc())
	{
		CXTPPenDC penDC(*pDC, crLine);
		pDC->MoveTo(x1, y1);
		pDC->LineTo(x2, y2);
		return TRUE;
	}
	return FALSE;
}

DWORD AFX_CDECL CXTPDrawHelpers::GetComCtlVersion()
{
	return XTPSystemVersion()->GetComCtlVersion();
}

BOOL CXTPDrawHelpers::IsLowResolution(HDC hDC /* = 0*/)
{
	return XTPColorManager()->IsLowResolution(hDC);
}

CRect AFX_CDECL CXTPDrawHelpers::GetWorkArea(LPCRECT rect)
{
	return XTPMultiMonitor()->GetWorkArea(rect);
}

CRect AFX_CDECL CXTPDrawHelpers::GetWorkArea(const CWnd* pWnd)
{
	return XTPMultiMonitor()->GetWorkArea(pWnd);
}

CRect AFX_CDECL CXTPDrawHelpers::GetScreenArea(const CWnd* pWnd)
{
	return XTPMultiMonitor()->GetScreenArea(pWnd);
}

CRect AFX_CDECL CXTPDrawHelpers::GetWorkArea(const POINT& point)
{
	return XTPMultiMonitor()->GetWorkArea(point);
}

CRect AFX_CDECL CXTPDrawHelpers::GetWorkArea()
{
	return XTPMultiMonitor()->GetWorkArea();
}

BOOL CXTPDrawHelpers::TakeSnapShot(CWnd* pWnd, CBitmap& bmpSnapshot)
{
	if (!::IsWindow(pWnd->GetSafeHwnd()))
		return FALSE;

	CWnd* pWndParent = pWnd->GetParent();
	if (::IsWindow(pWndParent->GetSafeHwnd()))
	{
		if (bmpSnapshot.GetSafeHandle() != NULL)
			bmpSnapshot.DeleteObject();

		// convert our coordinates to our parent coordinates.
		CXTPWindowRect rc(pWnd);
		pWndParent->ScreenToClient(&rc);

		// copy what's on the parents background at this point
		CDC* pDC = pWndParent->GetDC();

		CDC memDC;
		memDC.CreateCompatibleDC(pDC);
		bmpSnapshot.CreateCompatibleBitmap(pDC, rc.Width(), rc.Height());

		CXTPBitmapDC bitmapDC(&memDC, &bmpSnapshot);
		memDC.BitBlt(0, 0, rc.Width(), rc.Height(), pDC, rc.left, rc.top, SRCCOPY);

		pWndParent->ReleaseDC(pDC);

		return TRUE;
	}

	return FALSE;
}

BOOL CXTPDrawHelpers::DrawTransparentBack(CDC* pDC, CWnd* pWnd, CBitmap& bmpSnapshot)
{
	if (!::IsWindow(pWnd->GetSafeHwnd()))
		return FALSE;

	if (::GetWindowLong(pWnd->GetSafeHwnd(), GWL_EXSTYLE) & WS_EX_TRANSPARENT)
	{
		// Get background.
		if (!TakeSnapShot(pWnd, bmpSnapshot))
			return FALSE;

		CXTPClientRect rc(pWnd);

		CDC memDC;
		memDC.CreateCompatibleDC(pDC);

		CXTPBitmapDC bitmapDC(&memDC, &bmpSnapshot);
		pDC->BitBlt(0, 0, rc.Width(), rc.Height(), &memDC, 0, 0, SRCCOPY);

		return TRUE;
	}

	return FALSE;
}

BOOL CXTPDrawHelpers::IsTopParentActive(HWND hWnd)
{
	HWND hwndForeground = ::GetForegroundWindow();

	HWND hWndT;
	while ((hWndT = AfxGetParentOwner(hWnd)) != NULL)
	{
#ifdef _XTP_ACTIVEX
		if (hwndForeground == hWndT)
			return TRUE;
#endif
		hWnd = hWndT;
	}

	HWND hwndActivePopup = ::GetLastActivePopup(hWnd);

	if (hwndForeground == hwndActivePopup)
		return TRUE;

#ifdef _XTP_ACTIVEX
	if (hwndActivePopup && ::GetWindowLong(hwndForeground, GWL_EXSTYLE) & WS_EX_TOOLWINDOW)
	{
		if (::GetParent(hwndForeground) == hwndActivePopup)
			return TRUE;
	}
#endif

	return FALSE;
}
void CXTPDrawHelpers::ScreenToWindow(CWnd* pWnd, LPPOINT lpPoint)
{
	RECT rc;
	::GetWindowRect(pWnd->GetSafeHwnd(), &rc);

	lpPoint->y -= rc.top;

	if (GetWindowLong(pWnd->GetSafeHwnd(), GWL_EXSTYLE) & WS_EX_LAYOUTRTL)
	{
		lpPoint->x = rc.right - lpPoint->x;
	}
	else
	{
		lpPoint->x -= rc.left;
	}
}

BOOL AFX_CDECL CXTPDrawHelpers::RegisterWndClass(HINSTANCE hInstance, LPCTSTR lpszClassName,
												 UINT style, HICON hIcon, HBRUSH hbrBackground)
{
	WNDCLASS wndcls;
	ZeroMemory(&wndcls, sizeof(wndcls));

	wndcls.style		 = style;
	wndcls.lpfnWndProc   = ::DefWindowProc;
	wndcls.hInstance	 = hInstance ? hInstance : XTPGetInstanceHandle();
	wndcls.hCursor		 = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
	wndcls.lpszClassName = lpszClassName;
	wndcls.hIcon		 = hIcon;
	wndcls.hbrBackground = hbrBackground;

	return AfxRegisterClass(&wndcls);
}

void CXTPDrawHelpers::GetWindowCaption(HWND hWnd, CString& strWindowText)
{
#ifdef _UNICODE
	int nLen = (int)::DefWindowProc(hWnd, WM_GETTEXTLENGTH, 0, 0);
	::DefWindowProc(hWnd, WM_GETTEXT, XTPToWPARAM(nLen + 1),
					XTPToLPARAM((LPCTSTR)strWindowText.GetBuffer(nLen)));
	strWindowText.ReleaseBuffer();
#else
	int nLen = ::GetWindowTextLength(hWnd);
	::GetWindowText(hWnd, strWindowText.GetBufferSetLength(nLen), nLen + 1);
	strWindowText.ReleaseBuffer();
#endif
}

void CXTPDrawHelpers::DrawColorRect(CDC* pDC, int l, int t, int cx, int cy, COLORREF clrFill,
									COLORREF clrBorder)
{
	CXTPDrawHelpers::DrawColorRect(pDC, CRect(l, t, l + cx, t + cy), clrFill, clrBorder);
}

void CXTPDrawHelpers::DrawColorRect(CDC* pDC, CRect rColor, COLORREF clrFill, COLORREF clrBorder)
{
	if (pDC->GetSafeHdc())
	{
		// draw border.
		CXTPPenDC pen(pDC->GetSafeHdc(), clrBorder);
		pDC->Rectangle(rColor);
		rColor.DeflateRect(1, 1, 1, 1);

		// draw color rect.
		pDC->FillSolidRect(rColor, clrFill);
	}
}

void CXTPDrawHelpers::DrawHatchRect(CDC* pDC, CRect rColor, COLORREF clrFill, COLORREF clrBorder)
{
	HDC hDC = pDC->GetSafeHdc();
	if (hDC)
	{
		// draw border.
		CXTPPenDC pen(hDC, clrBorder);
		pDC->Rectangle(rColor);
		rColor.DeflateRect(1, 1, 1, 1);

		// draw diagonal brush
		// using ::SelectObject instead of CDC::SelectObject because VC6 can assert in print preview
		HBRUSH hBrush	= ::CreateHatchBrush(HS_BDIAGONAL, clrBorder);
		HBRUSH hOldBrush = (HBRUSH)::SelectObject(hDC, hBrush);
		::SetBkColor(hDC, clrFill);
		::FillRect(hDC, rColor, hBrush);
		::SelectObject(hDC, hOldBrush);
		::DeleteObject(hBrush);
	}
}

void CXTPDrawHelpers::CreatePatternBrush(CBitmap& bmp, CBrush& brush, COLORREF clrFill,
										 const void* pBitsBrush)
{
	if (bmp.GetSafeHandle())
		bmp.DeleteObject();

	if (brush.GetSafeHandle())
		brush.DeleteObject();

	bmp.CreateBitmap(8, 8, 1, 1, pBitsBrush);

	LOGBRUSH lb;
	lb.lbStyle = BS_PATTERN;
	lb.lbHatch = (ULONG_PTR)(HBITMAP)bmp;
	lb.lbColor = clrFill;
	brush.CreateBrushIndirect(&lb);
}

void CXTPDrawHelpers::DrawBrushRect(CDC* pDC, CRect rColor, COLORREF clrFill, COLORREF clrBorder,
									const void* pBitsBrush)
{
	HDC hDC = pDC->GetSafeHdc();
	if (hDC)
	{
		// save dc state.
		CXTPDCSaveState saveDC(pDC);

		CXTPPenDC pen(hDC, clrBorder);

		CBitmap bmp;
		CBrush brush;
		CreatePatternBrush(bmp, brush, clrFill, pBitsBrush);

		// draw pattern brush, using ::SelectObject instead of
		// CDC::SelectObject because VC6 can assert in print preview.
		::SelectObject(hDC, (HBRUSH)brush);

		pDC->SetBkMode(TRANSPARENT);
		pDC->SetBkColor(clrFill);
		pDC->SetTextColor(clrBorder);
		pDC->Rectangle(&rColor);

		brush.DeleteObject();
	}
}

static int ComputeFittingSize(const CSize& szOuter, const CSize& szInner)
{
	int nSize = 0;
	if (0 < szOuter.cx && 0 < szOuter.cy)
	{
		// 1. wide outer, wide inner
		if (((szOuter.cy <= szOuter.cx) && (szInner.cy <= szInner.cx))
			// 3. tall outer, wide inner
			|| ((szOuter.cx <= szOuter.cy) && (szInner.cy <= szInner.cx)))
		{
			nSize = ::MulDiv(szInner.cy, szOuter.cx, szInner.cx);
		}
		// 2. wide outer, tall inner
		else if (((szOuter.cy <= szOuter.cx) && (szInner.cx <= szInner.cy))
				 // 4. tall outer, tall inner
				 || ((szOuter.cx <= szOuter.cy) && (szInner.cx <= szInner.cy)))
		{
			nSize = szOuter.cy;
		}
		else
		{
			ASSERT(!"Unexpected case");
		}
	}

	return nSize;
}

static CRect MakeEvenRect(CRect rc)
{
	ASSERT(rc.left <= rc.right);
	ASSERT(rc.top <= rc.bottom);

	CSize sz(rc.right - rc.left, rc.bottom - rc.top);
	bool oddX = (0 != sz.cx % 2);
	bool oddY = (0 != sz.cy % 2);

	if (oddX && !oddY)
	{
		if (sz.cy <= sz.cx)
		{
			--rc.right;
		}
		else
		{
			--rc.right;
			++rc.left;
			--rc.bottom;
		}
	}

	if (oddY && !oddX)
	{
		if (sz.cx <= sz.cy)
		{
			--rc.bottom;
		}
		else
		{
			++rc.top;
			--rc.bottom;
			--rc.right;
		}
	}

	return rc;
}

int CXTPDrawHelpers::ComputeFittingFontHeight(CDC* pDC, LPCTSTR lpStr, int cch, const CRect& rc,
											  CFont& fntTest)
{
	ASSERT(pDC);
	ASSERT(NULL != lpStr);
	ASSERT(0 <= cch);

	int nResultSize  = 0;
	CSize szRectSize = MakeEvenRect(rc).Size();
	if (0 < szRectSize.cx && 0 < szRectSize.cy)
	{
		// Obtain text extent for the test font.
		LOGFONT fontData;
		if (0 < GetObject(fntTest, sizeof(fontData), &fontData))
		{
			ASSERT(0 == fontData.lfWidth && "Non-default font width is not supported");

			CXTPFontDC fontDC(pDC, &fntTest);
			CSize szTextSize  = pDC->GetTextExtent(lpStr, cch);
			fontData.lfHeight = ComputeFittingSize(szRectSize, szTextSize);

			int nStep = 0;
			while (0 < fontData.lfHeight && 0 == nResultSize)
			{
				CXTPFont fnt;
				if (fnt.CreateFontIndirect(&fontData))
				{
					CXTPFontDC fontDC2(pDC, &fnt);
					szTextSize = pDC->GetTextExtent(lpStr, cch);

					switch (nStep)
					{
						case 0: // initial check
							if (szTextSize.cx < szRectSize.cx && szTextSize.cy < szRectSize.cy)
							{
								nStep = 1;
								++fontData.lfHeight;
							}
							else if (szRectSize.cx < szTextSize.cx || szRectSize.cy < szTextSize.cy)
							{
								nStep = -1;
								--fontData.lfHeight;
							}
							else
							{
								nResultSize = fontData.lfHeight;
							}
							break;
						case 1: // increment font height until text is smaller than a square
							if (szTextSize.cx < szRectSize.cx && szTextSize.cy < szRectSize.cy)
							{
								++fontData.lfHeight;
							}
							else
							{
								nResultSize = --fontData.lfHeight;
							}
							break;
						case -1: // decrement font height until text is larger than a square
							if (szTextSize.cx <= szRectSize.cx && szTextSize.cy <= szRectSize.cy)
							{
								nResultSize = fontData.lfHeight;
							}
							else
							{
								--fontData.lfHeight;
							}
							break;
					}
				}
				else
				{
					break;
				}
			}
		}
	}

	return nResultSize;
}

BOOL AFX_CDECL CXTPDrawHelpers::StretchImageListImage(CDC* pDC, CRect rcDest,
													  CImageList* pImageList, int nImage)
{
	ASSERT(pDC);
	ASSERT(pImageList);

	BOOL bSuccess = FALSE;

	HICON hIcon = pImageList->ExtractIcon(nImage);
	if (NULL != hIcon)
	{
		bSuccess = ::DrawIconEx(*pDC, rcDest.left, rcDest.top, hIcon, rcDest.Width(),
								rcDest.Height(), 0, NULL, DI_NORMAL);
		DestroyIcon(hIcon);
	}

	return bSuccess;
}

BOOL AFX_CDECL CXTPDrawHelpers::ProcessPendingPaintMessages(HWND hWnd /*= NULL*/)
{
	BOOL bResult = TRUE;
	MSG msg;

	while (::PeekMessage(&msg, hWnd, WM_PAINT, WM_PAINT, PM_NOREMOVE)
		   || ::PeekMessage(&msg, hWnd, WM_ERASEBKGND, WM_ERASEBKGND, PM_NOREMOVE)
		   || ::PeekMessage(&msg, hWnd, WM_SYNCPAINT, WM_SYNCPAINT, PM_NOREMOVE)
		   || ::PeekMessage(&msg, hWnd, WM_NCPAINT, WM_NCPAINT, PM_NOREMOVE))
	{
		if (WM_QUIT != msg.message)
		{
			if (::PeekMessage(&msg, hWnd, msg.message, msg.message, PM_REMOVE))
			{
				if (WM_QUIT != msg.message) // WM_QUIT should never happen here but still extra
											// verification won't harm
				{
					::DispatchMessage(&msg);

					// This ensures the window won't enter the second painting cycle.
					::ValidateRect(msg.hwnd, NULL);
				}
				else
				{
					// Re-post WM_QUIT as it's been removed.
					::PostQuitMessage(XTPToInt(msg.wParam));

					bResult = FALSE;
					break;
				}
			}
		}
		else
		{
			bResult = FALSE;
			break;
		}
	}

	return bResult;
}

BOOL AFX_CDECL XTPTrackMouseEvent(HWND hWndTrack, DWORD dwFlags /*= TME_LEAVE*/,
								  DWORD dwHoverTime /*= HOVER_DEFAULT*/)
{
	ASSERT(::IsWindow(hWndTrack));

	TRACKMOUSEEVENT tme;
	tme.cbSize		= sizeof(TRACKMOUSEEVENT);
	tme.dwFlags		= dwFlags;
	tme.dwHoverTime = dwHoverTime;
	tme.hwndTrack   = hWndTrack;

	return ::_TrackMouseEvent(&tme);
}

BOOL AFX_CDECL XTPGetPrinterDeviceDefaults(HGLOBAL& ref_hDevMode, HGLOBAL& ref_hDevNames)
{
	CWinApp* pApp = AfxGetApp();
	if (pApp)
	{
		PRINTDLG _pd;
		::ZeroMemory(&_pd, sizeof(_pd));
		if (pApp->GetPrinterDeviceDefaults(&_pd))
		{
			ref_hDevMode  = _pd.hDevMode;
			ref_hDevNames = _pd.hDevNames;
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CXTPDrawHelpers::GetIconLogFont(LOGFONT* pLogFont)
{
	ASSERT(pLogFont);

	if (!pLogFont)
		return FALSE;

	::ZeroMemory(pLogFont, sizeof(LOGFONT));

	if (::SystemParametersInfo(SPI_GETICONTITLELOGFONT, sizeof(LOGFONT), pLogFont, 0))
	{
		if (XTPSystemVersion()->IsClearTypeTextQualitySupported())
			pLogFont->lfQuality = CLEARTYPE_QUALITY;

		pLogFont->lfCharSet = XTPResourceManager()->GetFontCharset();

		return TRUE;
	}

	return FALSE;
}

BOOL CXTPDrawHelpers::GetDefaultLogFont(LOGFONT* pLogFont, BOOL bModernFont /*=TRUE*/,
										BOOL bVertFont /*=FALSE*/)
{
	ASSERT(pLogFont);

	if (!pLogFont)
		return FALSE;

	LOGFONT lf;

	// get the default icon title font for the system.
	if (GetIconLogFont(&lf))
	{
		if (bModernFont)
		{
			if (FontExists(_T("Segoe UI")))
				STRCPY_S(lf.lfFaceName, LF_FACESIZE, _T("Segoe UI"));

			else if (FontExists(_T("Tahoma")))
				STRCPY_S(lf.lfFaceName, LF_FACESIZE, _T("Tahoma"));
		}

		if (!XTPSystemVersion()->IsWin2KOrGreater())
		{
			if (FontExists(_T("Arial")) && bVertFont)
				STRCPY_S(lf.lfFaceName, LF_FACESIZE, _T("Arial"));
		}

		// copy the icon title log font to 'lf'.
		MEMCPY_S(pLogFont, &lf, sizeof(LOGFONT));

		return TRUE;
	}

	return FALSE;
}

void CXTPDrawHelpers::ReplaceColor(CDC* pDC, CRect rArea, COLORREF crFind, COLORREF crReplace)
{
	CXTPDCSaveState dcState(pDC);

	CDC dcMonoChrome;
	dcMonoChrome.CreateCompatibleDC(pDC);

	CBitmap bmpMonoChrome;
	bmpMonoChrome.CreateCompatibleBitmap(&dcMonoChrome, rArea.Width(), rArea.Height());
	dcMonoChrome.SelectObject(&bmpMonoChrome);

	CPoint pt = rArea.TopLeft();

	// blt to monochrome dc so the mask color will have 1 set and remaining colors 0.
	pDC->SetBkColor(crFind);
	dcMonoChrome.BitBlt(0, 0, rArea.Width(), rArea.Height(), pDC, pt.x, pt.y, SRCCOPY);

	CDC dcBuffer;
	dcBuffer.CreateCompatibleDC(pDC);

	CBitmap bmp;
	bmp.CreateCompatibleBitmap(pDC, rArea.Width(), rArea.Height());
	dcBuffer.SelectObject(&bmp);

	dcBuffer.SetBkColor(crReplace);
	dcBuffer.SetTextColor(RGB(255, 255, 255));

	// BitBlt to buffer dc so the monochrome white is set to fill color and the monochrome black is
	// set to white then BitBlt to pDC with buffer dc so that the replace color part is blackened
	// out and all other colors remains same
	dcBuffer.BitBlt(0, 0, rArea.Width(), rArea.Height(), &dcMonoChrome, 0, 0, SRCCOPY);
	pDC->BitBlt(pt.x, pt.y, rArea.Width(), rArea.Height(), &dcBuffer, 0, 0, SRCAND);

	// BitBlt to buffer dc so that the monochrome white is set to fill color and the monochrome
	// black is set to black then BitBlt to pDC with buffer dc so that all colors remains as they
	// where except the blackened out part receives the fill color
	dcBuffer.SetTextColor(RGB(0, 0, 0));
	dcBuffer.BitBlt(0, 0, rArea.Width(), rArea.Height(), &dcMonoChrome, 0, 0, SRCCOPY);
	pDC->BitBlt(pt.x, pt.y, rArea.Width(), rArea.Height(), &dcBuffer, 0, 0, SRCPAINT);
}

//////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CXTPPrinterInfo, CXTPCmdTarget)

CXTPPrinterInfo::CXTPPrinterInfo()
{
#ifdef _XTP_ACTIVEX
	EnableAutomation();
	EnableTypeLib();
#endif
}

CXTPPrinterInfo::~CXTPPrinterInfo()
{
}

CString CXTPPrinterInfo::GetName(XTPEnumDeviceName eNameID)
{
	CString strName;
	HGLOBAL hDevMode = NULL, hDevNames = NULL;

	if (XTPGetPrinterDeviceDefaults(hDevMode, hDevNames) && hDevNames != NULL)
	{
		LPDEVNAMES lpDev = (LPDEVNAMES)::GlobalLock(hDevNames);
		if (lpDev)
		{
			strName = (LPCTSTR)lpDev + _GetNameOffset(lpDev, eNameID);
		}
		::GlobalUnlock(hDevNames);
	}
	return strName;
}

WORD CXTPPrinterInfo::_GetNameOffset(LPDEVNAMES pDevNames, XTPEnumDeviceName eNameID)
{
	ASSERT(pDevNames);
	if (!pDevNames)
		return 0;

	switch (eNameID)
	{
		case xtpDevName_Driver: return pDevNames->wDriverOffset;
		case xtpDevName_Device: return pDevNames->wDeviceOffset;
		case xtpDevName_Port: return pDevNames->wOutputOffset;
	};

	ASSERT(FALSE);
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CXTPPrintPageHeaderFooter, CXTPCmdTarget)

CXTPPrintPageHeaderFooter::CXTPPrintPageHeaderFooter(CXTPPrintOptions* pOwner, BOOL bHeader)
{
	ASSERT(pOwner);

	m_bHeader = bHeader;
	m_pOwner  = pOwner;

	VERIFY(CXTPDrawHelpers::GetIconLogFont(&m_lfFont));
	m_clrColor = RGB(0, 0, 0);

#ifdef _XTP_ACTIVEX
	EnableAutomation();
	EnableTypeLib();
#endif
}

CXTPPrintPageHeaderFooter::~CXTPPrintPageHeaderFooter()
{
}

void CXTPPrintPageHeaderFooter::Set(const CXTPPrintPageHeaderFooter* pSrc)
{
	ASSERT(pSrc);
	if (!pSrc)
		return;

	m_lfFont   = pSrc->m_lfFont;
	m_clrColor = pSrc->m_clrColor;

	m_strFormatString = pSrc->m_strFormatString;

	m_strLeft   = pSrc->m_strLeft;
	m_strCenter = pSrc->m_strCenter;
	m_strRight  = pSrc->m_strRight;
}

void CXTPPrintPageHeaderFooter::Clear()
{
	VERIFY(CXTPDrawHelpers::GetIconLogFont(&m_lfFont));
	m_clrColor = RGB(0, 0, 0);
	m_strFormatString.Empty();

	m_strLeft.Empty();
	m_strCenter.Empty();
	m_strRight.Empty();
}

BOOL CXTPPrintPageHeaderFooter::IsEmpty()
{
	return m_strLeft.IsEmpty() && m_strCenter.IsEmpty() && m_strRight.IsEmpty();
}

//===========================================================================
//  &w      Window title
//
//  &d      Date in short format (as specified by Regional Settings in Control Panel)
//  &D      Date in long format (as specified by Regional Settings in Control Panel)
//
//  &t      Time in the format specified by Regional Settings in Control Panel
//  &T      Time in 24-hour format
//
//  &p      Current page number
//  &P      Total number of pages
//  &#      Ordinal page number (even for WYSIWYG mode)
//
//  &b      The text immediately following these characters as centered.
//  &b&b    The text immediately following the first "&b" as centered, and the text following the
//  second "&b" as right-justified.
//
//  &&      A single ampersand (&)
//===========================================================================
void CXTPPrintPageHeaderFooter::FormatTexts(CPrintInfo* pInfo, LPCTSTR pcszWndTitle,
											int nVirtualPage)
{
	ASSERT(pInfo);
	if (m_strFormatString.IsEmpty() || !pInfo)
		return;

	LCID lcidLocale = m_pOwner ? m_pOwner->GetActiveLCID() : LOCALE_USER_DEFAULT;

	CString strFormat = m_strFormatString;
	strFormat.Replace(_T("&&"), _T("\1"));

	CString str_p, str_P = _T("*");
	if (nVirtualPage > 0)
		str_p.Format(_T("%d-%d"), pInfo->m_nCurPage, nVirtualPage);
	else
		str_p.Format(_T("%d"), pInfo->m_nCurPage);

	if (pInfo->GetMaxPage() != 65535)
		str_P.Format(_T("%d"), pInfo->GetMaxPage());

	strFormat.Replace(_T("&p"), str_p);
	strFormat.Replace(_T("&P"), str_P);

	strFormat.Replace(_T("\\n"), _T("\n"));

	_SplitFormatLCR(strFormat);

	_FormatDateTime(m_strLeft, lcidLocale);
	_FormatDateTime(m_strCenter, lcidLocale);
	_FormatDateTime(m_strRight, lcidLocale);

	if (pcszWndTitle)
	{
		m_strLeft.Replace(_T("&w"), pcszWndTitle);
		m_strCenter.Replace(_T("&w"), pcszWndTitle);
		m_strRight.Replace(_T("&w"), pcszWndTitle);
	}

	m_strLeft.Replace(_T("\1"), _T("&"));
	m_strCenter.Replace(_T("\1"), _T("&"));
	m_strRight.Replace(_T("\1"), _T("&"));
}

CString AFX_CDECL CXTPPrintPageHeaderFooter::GetParentFrameTitle(CWnd* pWnd)
{
	CString strTitle;

	ASSERT(pWnd);
	if (!pWnd)
		return strTitle;

	pWnd->GetWindowText(strTitle);
	if (strTitle.IsEmpty() || !pWnd->IsFrameWnd())
	{
		if (pWnd->GetParentFrame())
			pWnd->GetParentFrame()->GetWindowText(strTitle);

		if (strTitle.IsEmpty())
		{
			if (pWnd->GetParentOwner())
				pWnd->GetParentOwner()->GetWindowText(strTitle);
		}
	}
	return strTitle;
}

void CXTPPrintPageHeaderFooter::_FormatDateTime(CString& strFormat, LCID lcidLocale)
{
	SYSTEMTIME stNow;
	::GetLocalTime(&stNow);

	const int cnBufferSize				  = 200;
	TCHAR szDateFormatShort[cnBufferSize] = { 0 };
	TCHAR szDateFormatLong[cnBufferSize]  = { 0 };

	TCHAR szTimeFormat[cnBufferSize]   = { 0 };
	TCHAR szTimeFormat24[cnBufferSize] = { 0 };

	::GetDateFormat(lcidLocale, DATE_SHORTDATE, &stNow, NULL, szDateFormatShort, cnBufferSize);
	::GetDateFormat(lcidLocale, DATE_LONGDATE, &stNow, NULL, szDateFormatLong, cnBufferSize);

	::GetTimeFormat(lcidLocale, 0, &stNow, NULL, szTimeFormat, cnBufferSize);
	DWORD dwFlags24h = TIME_FORCE24HOURFORMAT | TIME_NOTIMEMARKER;
	::GetTimeFormat(lcidLocale, dwFlags24h, &stNow, NULL, szTimeFormat24, cnBufferSize);

	strFormat.Replace(_T("&d"), szDateFormatShort); // &d   Date in short format (as specified by
													// Regional Settings in Control Panel)
	strFormat.Replace(_T("&D"), szDateFormatLong);  // &D   Date in long format (as specified by
													// Regional Settings in Control Panel)

	strFormat.Replace(_T("&t"), szTimeFormat);   // &t  Time in the format specified by Regional
												 // Settings in Control Panel
	strFormat.Replace(_T("&T"), szTimeFormat24); // &T  Time in 24-hour format
}

void CXTPPrintPageHeaderFooter::_SplitFormatLCR(CString strFormat)
{
	m_strLeft.Empty();
	m_strCenter.Empty();
	m_strRight.Empty();

	int nAmpBidx = strFormat.Find(_T("&b"));
	if (nAmpBidx < 0)
	{
		m_strLeft = strFormat;
	}
	else if (nAmpBidx >= 0)
	{
		if (nAmpBidx > 0)
			m_strLeft = strFormat.Left(nAmpBidx);

		strFormat.Delete(0, nAmpBidx + 2);

		nAmpBidx = strFormat.Find(_T("&b"));
		if (nAmpBidx < 0)
		{
			m_strCenter = strFormat;
		}
		else if (nAmpBidx >= 0)
		{
			if (nAmpBidx > 0)
				m_strCenter = strFormat.Left(nAmpBidx);
			strFormat.Delete(0, nAmpBidx + 2);

			m_strRight = strFormat;
		}
	}
}

int CXTPPrintPageHeaderFooter::fnYi(int Xi, int Wi)
{
	int nYi  = Wi / Xi;
	int nYi_ = Wi % Xi;
	return nYi + (nYi_ ? 1 : 0);
}

int CXTPPrintPageHeaderFooter::_Calc3ColSizesIfSingleCol(CDC* pDC, int nW, const CString& str1,
														 const CString& str2, const CString& str3,
														 CSize& rsz1, CSize& rsz2, CSize& rsz3)
{
	CString str1trim = str1;
	CString str2trim = str2;
	CString str3trim = str3;

	str1trim.TrimLeft();
	str2trim.TrimLeft();
	str3trim.TrimLeft();

	UINT uFlags = DT_TOP | DT_WORDBREAK | DT_CALCRECT;
	rsz1 = rsz2 = rsz3 = CSize(0, 0);

	if (!str1trim.IsEmpty() && str2trim.IsEmpty() && str3trim.IsEmpty())
	{
		CRect rcText1(0, 0, nW, 32000);
		rsz1.cx		   = nW;
		return rsz1.cy = pDC->DrawText(str1, &rcText1, uFlags | DT_LEFT);
	}

	if (str1trim.IsEmpty() && !str2trim.IsEmpty() && str3trim.IsEmpty())
	{
		CRect rcText2(0, 0, nW, 32000);
		rsz2.cx		   = nW;
		return rsz2.cy = pDC->DrawText(str2, &rcText2, uFlags | DT_CENTER);
	}

	if (str1trim.IsEmpty() && str2trim.IsEmpty() && !str3trim.IsEmpty())
	{
		CRect rcText3(0, 0, nW, 32000);
		rsz3.cx		   = nW;
		return rsz3.cy = pDC->DrawText(str3, &rcText3, uFlags | DT_RIGHT);
	}

	return -1;
}

int CXTPPrintPageHeaderFooter::Calc3ColSizes(CDC* pDC, int nW, const CString& str1,
											 const CString& str2, const CString& str3, CSize& rsz1,
											 CSize& rsz2, CSize& rsz3)
{
	ASSERT(pDC);
	if (!pDC)
		return 0;

	int nIfSingleCol = _Calc3ColSizesIfSingleCol(pDC, nW, str1, str2, str3, rsz1, rsz2, rsz3);
	if (nIfSingleCol >= 0)
		return nIfSingleCol;

	int nW1 = pDC->GetTextExtent(str1).cx;
	int nW2 = pDC->GetTextExtent(str2).cx;
	int nW3 = pDC->GetTextExtent(str3).cx;

	if (!nW || !nW1 && !nW2 && !nW3)
		return 0;

	nW1 = CXTP_max(nW1, 1);
	nW2 = CXTP_max(nW2, 1);
	nW3 = CXTP_max(nW3, 1);

	int nYmin		 = INT_MAX;
	int nYminMetric  = INT_MAX;
	int nXforYmin[4] = { 0, 1, 1, 1 };

	int nX[4]   = { 0, 0, 0, 0 };
	int nX_step = 15; // nW/50;

	int nX_start = nW / 9;
	int nX_stop  = nW / 2 - nW / 9;
	for (nX[1] = nX_start; nX[1] <= nX_stop; nX[1] += nX_step)
	{
		int nY[4];
		//************************
		nY[1] = fnYi(nX[1], nW1);

		int nX2_stop = nW - nX[1] - nW / 9;
		for (nX[2] = nX_start; nX[2] <= nX2_stop; nX[2] += nX_step)
		{
			nX[3] = nW - nX[1] - nX[2];

			//************************
			nY[2] = fnYi(nX[2], nW2);
			if (nY[2] > nYmin)
				continue;

			nY[3] = fnYi(nX[3], nW3);
			if (nY[3] > nYmin)
				break;

			int nY123 = CXTP_max(CXTP_max(nY[1], nY[2]), CXTP_max(nY[1], nY[3]));

			int nM			= nW / 3;
			int nY123Metric = abs(nM - nX[1]) + abs(nM - nX[2]) + abs(nM - nX[3]);

			if (nY123 < nYmin || nY123 == nYmin && nY123Metric < nYminMetric)
			{
				// TRACE(_T("-- =*=   %d <= %d, metrics(cur: %d, min: %d) \n"), nY123, nYmin,
				// nY123Metric, nYminMetric); TRACE(_T("-- prev (%d, %d, %d) \n"), nXforYmin[1],
				// nXforYmin[2], nXforYmin[3]); TRACE(_T("-- new  (%d, %d, %d) \n"), nX[1], nX[2],
				// nX[3]);

				nYmin		= nY123;
				nYminMetric = nY123Metric;

				nXforYmin[1] = nX[1];
				nXforYmin[2] = nX[2];
				nXforYmin[3] = nX[3];
			}
		}
	}

	ASSERT(nXforYmin[1] + nXforYmin[2] + nXforYmin[3] <= nW);

	UINT uFlags = DT_TOP | DT_WORDBREAK | DT_CALCRECT;
	CRect rcText1(0, 0, nXforYmin[1], 32000);

	rsz1.cx = nXforYmin[1];
	rsz1.cy = pDC->DrawText(str1, &rcText1, uFlags | DT_LEFT);

	CRect rcText2(0, 0, nXforYmin[2], 32000);
	rsz2.cx = nXforYmin[2];
	rsz2.cy = pDC->DrawText(str2, &rcText2, uFlags | DT_CENTER);

	CRect rcText3(0, 0, nXforYmin[3], 32000);
	rsz3.cx = nXforYmin[3];
	rsz3.cy = pDC->DrawText(str3, &rcText3, uFlags | DT_RIGHT);

	return CXTP_max(CXTP_max(rsz1.cy, rsz2.cy), CXTP_max(rsz1.cy, rsz3.cy));
}

void CXTPPrintPageHeaderFooter::Draw(CDC* pDC, CRect& rcRect, BOOL bCalculateOnly)
{
	CXTPFont fntHF;
	VERIFY(fntHF.CreateFontIndirect(&m_lfFont));
	CXTPFontDC autoFont(pDC, &fntHF, m_clrColor);

	int nWidth = rcRect.Width();

	CSize szLeft2, szCenter2, szRight2;

	int nHeight = Calc3ColSizes(pDC, nWidth, m_strLeft, m_strCenter, m_strRight, szLeft2, szCenter2,
								szRight2);

	int nBorderH = nHeight ? 3 : 0;
	if (m_bHeader)
	{
		rcRect.bottom = rcRect.top + nHeight + nBorderH;
	}
	else
	{
		rcRect.top = rcRect.bottom - nHeight - nBorderH;
	}

	if (bCalculateOnly)
	{
		return;
	}

	int nBkModePrev = pDC->SetBkMode(TRANSPARENT);

	UINT uFlags = DT_TOP | DT_WORDBREAK | DT_NOPREFIX;

	CRect rcLeft = rcRect;
	rcLeft.right = rcLeft.left + szLeft2.cx;
	pDC->DrawText(m_strLeft, &rcLeft, uFlags | DT_LEFT);

	CRect rcCenter = rcRect;
	rcCenter.left  = rcLeft.right;
	rcCenter.right = rcCenter.left + szCenter2.cx;
	pDC->DrawText(m_strCenter, &rcCenter, uFlags | DT_CENTER);

	CRect rcRight = rcRect;
	rcRight.left  = rcRight.right - szRight2.cx;
	pDC->DrawText(m_strRight, &rcRight, uFlags | DT_RIGHT);

	pDC->SetBkMode(nBkModePrev);
}

//////////////////////////////////////////////////////////////////////////
// CXTPDpi
//////////////////////////////////////////////////////////////////////////

CXTPDpi* XTPDpiHelper()
{
	return &CXTPSingleton<CXTPDpi>::Instance();
}

BOOL CXTPDpi::m_bDpiAwarenessInitialized								= FALSE;
CXTPDpi::PFNSetProcessDpiAwareness CXTPDpi::m_pfnSetProcessDpiAwareness = NULL;
CXTPDpi::PFNGetProcessDpiAwareness CXTPDpi::m_pfnGetProcessDpiAwareness = NULL;
CXTPDpi::PFNSetProcessDPIAware CXTPDpi::m_pfnSetProcessDPIAware			= NULL;
BOOL CXTPDpi::m_bSetProcessDPIAwareResult								= FALSE;
CXTPDpi::PFNMonitorFromWindow CXTPDpi::m_pfnMonitorFromWindow			= NULL;
CXTPDpi::PFNGetDpiForMonitor CXTPDpi::m_pfnGetDpiForMonitor				= NULL;

CXTPDpi::CXTPDpi()
{
	Construct();
}

CXTPDpi::CXTPDpi(HDC hDC)
{
	Construct(hDC);
}

CXTPDpi::CXTPDpi(HWND hWnd)
{
	Construct(NULL, hWnd);
}

CXTPDpi::~CXTPDpi()
{
	if (m_bReleaseDC)
	{
		::ReleaseDC(m_hWnd, m_hDC);
	}
}

void CXTPDpi::Construct(HDC hDC /*= NULL*/, HWND hWnd /*= NULL*/)
{
	ASSERT(!(NULL != hDC && NULL != hWnd));

	m_bInitialized		= FALSE;
	m_dpiX				= CXTPDpi::DefaultDpi;
	m_dpiY				= CXTPDpi::DefaultDpi;
	m_hWnd				= NULL;
	m_bDpiBitmapScaling = FALSE;

	if (NULL != hDC)
	{
		m_hDC				 = hDC;
		m_hWnd				 = NULL;
		m_bReleaseDC		 = FALSE;
		m_bSupportsDpiChange = FALSE;
	}
	else if (NULL != hWnd)
	{
		m_hWnd = hWnd;
		ASSERT(HWND_DESKTOP != m_hWnd ? ::IsWindow(m_hWnd) : TRUE);
		m_hDC				 = ::GetDC(m_hWnd);
		m_bReleaseDC		 = (NULL != m_hDC);
		m_bSupportsDpiChange = FALSE;
	}
	else
	{
		m_hDC				 = ::GetDC(HWND_DESKTOP);
		m_hWnd				 = HWND_DESKTOP;
		m_bReleaseDC		 = TRUE;
		m_bSupportsDpiChange = TRUE;
	}
}

void CXTPDpi::InitDpiAwareness()
{
	if (!m_bDpiAwarenessInitialized)
	{
		if (XTPSystemVersion()->IsWin81OrGreater())
		{
			HMODULE hShcoreDll = ::LoadLibrary(_T("shcore.dll"));
			if (NULL != hShcoreDll)
			{
				m_pfnSetProcessDpiAwareness = XTPToFunctionPtr<PFNSetProcessDpiAwareness>(
					::GetProcAddress(hShcoreDll, "SetProcessDpiAwareness"));
				m_pfnGetProcessDpiAwareness = XTPToFunctionPtr<PFNGetProcessDpiAwareness>(
					::GetProcAddress(hShcoreDll, "GetProcessDpiAwareness"));
				m_pfnGetDpiForMonitor = XTPToFunctionPtr<PFNGetDpiForMonitor>(
					::GetProcAddress(hShcoreDll, "GetDpiForMonitor"));
			}

			HMODULE hUser32Dll = ::GetModuleHandle(_T("user32.dll"));
			if (NULL != hUser32Dll)
			{
				m_pfnMonitorFromWindow = XTPToFunctionPtr<PFNMonitorFromWindow>(
					::GetProcAddress(hUser32Dll, "MonitorFromWindow"));
			}
		}
		else if (XTPSystemVersion()->IsWinVistaOrGreater())
		{
			HMODULE hUser32Dll = ::GetModuleHandle(_T("user32.dll"));
			if (NULL != hUser32Dll)
			{
				m_pfnSetProcessDPIAware = XTPToFunctionPtr<PFNSetProcessDPIAware>(
					::GetProcAddress(hUser32Dll, "SetProcessDPIAware"));
			}
		}

		m_bDpiAwarenessInitialized = TRUE;
	}
}

BOOL CXTPDpi::SetDpiAwareness(XTPDpiAwareness dpiAwareness)
{
	BOOL bResult = FALSE;

	InitDpiAwareness();

	if (NULL != m_pfnSetProcessDpiAwareness)
	{
		bResult = SUCCEEDED(m_pfnSetProcessDpiAwareness(dpiAwareness));
	}
	else if (NULL != m_pfnSetProcessDPIAware && xtpDpiUnaware != dpiAwareness)
	{
		bResult						= m_pfnSetProcessDPIAware();
		m_bSetProcessDPIAwareResult = bResult;
	}

	return bResult;
}

XTPDpiAwareness CXTPDpi::GetDpiAwareness() const
{
	XTPDpiAwareness dpiAwareness = xtpDpiUnaware;

	if (NULL != m_pfnSetProcessDpiAwareness)
	{
		m_pfnGetProcessDpiAwareness(NULL, &dpiAwareness);
	}
	else if (NULL != m_pfnSetProcessDPIAware)
	{
		dpiAwareness = m_bSetProcessDPIAwareResult ? xtpDpiSystemAware : xtpDpiUnaware;
	}

	return dpiAwareness;
}

LRESULT CXTPDpi::OnWmCreate(HWND hWnd, WPARAM /*wParam*/, LPARAM /*lParam*/)
{
	if (NULL != m_pfnMonitorFromWindow && NULL != m_pfnGetDpiForMonitor)
	{
		HANDLE hMonitor = m_pfnMonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
		if (NULL != hMonitor)
		{
			UINT dpiX = 0, dpiY = 0;
			if (SUCCEEDED(m_pfnGetDpiForMonitor(hMonitor, xtpEffectiveDpiType, &dpiX, &dpiY)))
			{
				Init(XTPToIntChecked(dpiX), XTPToIntChecked(dpiY), TRUE);
			}
		}
	}

	return 0;
}

LRESULT CXTPDpi::OnWmDpiChanged(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	if (m_bSupportsDpiChange)
	{
		Init(LOWORD(wParam), HIWORD(wParam), TRUE);

		const RECT* pRect = reinterpret_cast<const RECT*>(lParam);
		ASSERT(NULL != pRect);
		::SetWindowPos(hWnd, NULL, pRect->left, pRect->top, pRect->right - pRect->left,
					   pRect->bottom - pRect->top, SWP_NOZORDER | SWP_NOACTIVATE);

		::RedrawWindow(hWnd, NULL, NULL, RDW_INVALIDATE | RDW_ERASE | RDW_ALLCHILDREN | RDW_FRAME);
	}

	return 0;
}

BOOL CXTPDpi::OnWndMsg(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	ASSERT(NULL != pResult);

	BOOL bHandled = FALSE;

	ASSERT(m_bSupportsDpiChange
		   && "This CXTPDpi object must not be used for handling DPI window messages");

	switch (message)
	{
		case WM_DPICHANGED:
			*pResult = OnWmDpiChanged(hWnd, wParam, lParam);
			bHandled = TRUE;
			break;
		case WM_CREATE: OnWmCreate(hWnd, wParam, lParam); break;
	}

	return bHandled;
}

int CXTPDpi::GetDPIX() const
{
	const_cast<CXTPDpi*>(this)->Init();
	return m_dpiX;
}

int CXTPDpi::GetDPIY() const
{
	const_cast<CXTPDpi*>(this)->Init();
	return m_dpiY;
}

template<class T>
inline T ScaleDpi(T v, int dpi, int dpiScaleAbove)
{
	return dpiScaleAbove < dpi ? (v * static_cast<T>(dpi)) / static_cast<T>(CXTPDpi::DefaultDpi)
							   : v;
}

template<class T>
inline T UnscaleDpi(T v, int dpi, int dpiScaleAbove)
{
	return dpiScaleAbove < dpi ? (v * static_cast<T>(CXTPDpi::DefaultDpi)) / static_cast<T>(dpi)
							   : v;
}

template<class T>
inline T ScaleDpiX(T x, HDC hDC, int dpiScaleAbove)
{
	int dpiX = ::GetDeviceCaps(hDC, LOGPIXELSX);
	return dpiScaleAbove < dpiX ? (x * static_cast<T>(dpiX)) / static_cast<T>(CXTPDpi::DefaultDpi)
								: x;
}

template<class T>
inline T ScaleDpiY(T y, HDC hDC, int dpiScaleAbove)
{
	int dpiY = ::GetDeviceCaps(hDC, LOGPIXELSY);
	return dpiScaleAbove < dpiY ? (y * static_cast<T>(dpiY)) / static_cast<T>(CXTPDpi::DefaultDpi)
								: y;
}

template<class T>
inline T UnscaleDpiX(T x, HDC hDC, int dpiScaleAbove)
{
	int dpiX = ::GetDeviceCaps(hDC, LOGPIXELSX);
	return dpiScaleAbove < dpiX ? (x * static_cast<T>(CXTPDpi::DefaultDpi)) / static_cast<T>(dpiX)
								: x;
}

template<class T>
inline T UnscaleDpiY(T y, HDC hDC, int dpiScaleAbove)
{
	int dpiY = ::GetDeviceCaps(hDC, LOGPIXELSY);
	return dpiScaleAbove < dpiY ? (y * static_cast<T>(CXTPDpi::DefaultDpi)) / static_cast<T>(dpiY)
								: y;
}

// Convert between raw pixels and relative pixels.
int CXTPDpi::ScaleX(int x, int iDpiScaleAbove /*= CXTPDpi::DefaultDpi*/) const
{
	int nScaled;
	int dpiX = GetDPIX();

	if (iDpiScaleAbove < dpiX)
	{
		if (-DpiValueCount <= x && x <= DpiValueCount)
		{
			nScaled = (0 <= x ? m_nPositiveDpiValues[x].x : m_nNegativeDpiValues[-x].x);
		}
		else
		{
			nScaled = ::MulDiv(x, dpiX, CXTPDpi::DefaultDpi);
		}
	}
	else
	{
		nScaled = x;
	}

	return nScaled;
}

int CXTPDpi::ScaleY(int y, int iDpiScaleAbove /*= CXTPDpi::DefaultDpi*/) const
{
	int nScaled;
	int dpiY = GetDPIY();

	if (iDpiScaleAbove < dpiY)
	{
		if (-DpiValueCount <= y && y <= DpiValueCount)
		{
			nScaled = (0 <= y ? m_nPositiveDpiValues[y].y : m_nNegativeDpiValues[-y].y);
		}
		else
		{
			nScaled = ::MulDiv(y, dpiY, CXTPDpi::DefaultDpi);
		}
	}
	else
	{
		nScaled = y;
	}

	return nScaled;
}

int CXTPDpi::ScaledScreenWidth() const
{
	return ScaleSystemMetricX(SM_CXSCREEN);
}

int CXTPDpi::ScaledScreenHeight() const
{
	return ScaleSystemMetricY(SM_CYSCREEN);
}

bool CXTPDpi::IsResolutionAtLeast(int cxMin, int cyMin) const
{
	return (ScaledScreenWidth() >= cxMin) && (ScaledScreenHeight() >= cyMin);
}

int CXTPDpi::PointsToPixels(int pt) const
{
	return ::MulDiv(pt, GetDPIY(), 72);
}

void CXTPDpi::Invalidate()
{
	m_bInitialized = FALSE;
}

BOOL CXTPDpi::GetPhysicalWindowScaleFactors(HWND hWnd, double& xScale, double& yScale) const
{
	CRect rcScreen = XTPMultiMonitor()->GetScreenArea(hWnd);

	DEVMODE dm = { 0 };
	dm.dmSize  = sizeof(dm);
	if (!XTPMultiMonitor()->GetDisplaySettings(hWnd, &dm))
		return FALSE;

	xScale = static_cast<double>(dm.dmPelsWidth) / rcScreen.Width();
	yScale = static_cast<double>(dm.dmPelsHeight) / rcScreen.Height();

	return TRUE;
}

void CXTPDpi::Init(int dpiX /*= 0*/, int dpiY /*= 0*/, BOOL bUpdate /*= FALSE*/)
{
	if (!m_bInitialized || bUpdate)
	{
		ASSERT((0 == dpiX && 0 == dpiY) || (0 != dpiX && 0 != dpiY));
		if (m_dpiX != dpiX || m_dpiY != dpiY)
		{
			m_dpiX = (0 == dpiX ? (NULL != m_hDC ? ::GetDeviceCaps(m_hDC, LOGPIXELSX) : 0) : dpiX);
			m_dpiY = (0 == dpiY ? (NULL != m_hDC ? ::GetDeviceCaps(m_hDC, LOGPIXELSY) : 0) : dpiY);

			if (0 != m_dpiX && 0 != m_dpiY)
			{
				for (int i = 0; i <= DpiValueCount; ++i)
				{
					m_nPositiveDpiValues[i].x = ::MulDiv(i, m_dpiX, CXTPDpi::DefaultDpi);
					m_nPositiveDpiValues[i].y = ::MulDiv(i, m_dpiY, CXTPDpi::DefaultDpi);

					m_nNegativeDpiValues[i].x = ::MulDiv(-i, m_dpiX, CXTPDpi::DefaultDpi);
					m_nNegativeDpiValues[i].y = ::MulDiv(-i, m_dpiY, CXTPDpi::DefaultDpi);
				}
			}
			else
			{
				// Impossible to scale, use the same values
				for (int i = 0; i <= DpiValueCount; ++i)
				{
					m_nPositiveDpiValues[i].x = i;
					m_nPositiveDpiValues[i].y = i;

					m_nNegativeDpiValues[i].x = -i;
					m_nNegativeDpiValues[i].y = -i;
				}
			}

			m_bInitialized = true;
		}
	}
}

int CXTPDpi::ScaleSystemMetricX(int nIndex) const
{
	return ScaleX(::GetSystemMetrics(nIndex));
}

int CXTPDpi::ScaleSystemMetricY(int nIndex) const
{
	return ScaleY(::GetSystemMetrics(nIndex));
}

long CXTPDpi::ScaleX(long x, int iDpiScaleAbove /*= CXTPDpi::DefaultDpi*/) const
{
	return ScaleDpi(x, GetDPIX(), iDpiScaleAbove);
}

long CXTPDpi::ScaleY(long y, int iDpiScaleAbove /*= CXTPDpi::DefaultDpi*/) const
{
	return ScaleDpi(y, GetDPIY(), iDpiScaleAbove);
}

float CXTPDpi::ScaleX(float x, int iDpiScaleAbove /*= CXTPDpi::DefaultDpi*/) const
{
	return ScaleDpi(x, GetDPIX(), iDpiScaleAbove);
}

float CXTPDpi::ScaleY(float y, int iDpiScaleAbove /*= CXTPDpi::DefaultDpi*/) const
{
	return ScaleDpi(y, GetDPIY(), iDpiScaleAbove);
}

double CXTPDpi::ScaleX(double x, int iDpiScaleAbove /*= CXTPDpi::DefaultDpi*/) const
{
	return ScaleDpi(x, GetDPIX(), iDpiScaleAbove);
}

double CXTPDpi::ScaleY(double y, int iDpiScaleAbove /*= CXTPDpi::DefaultDpi*/) const
{
	return ScaleDpi(y, GetDPIY(), iDpiScaleAbove);
}

unsigned int CXTPDpi::ScaleX(unsigned int x, int iDpiScaleAbove /*= CXTPDpi::DefaultDpi*/) const
{
	return ScaleDpi(x, GetDPIX(), iDpiScaleAbove);
}

unsigned int CXTPDpi::ScaleY(unsigned int y, int iDpiScaleAbove /*= CXTPDpi::DefaultDpi*/) const
{
	return ScaleDpi(y, GetDPIY(), iDpiScaleAbove);
}

int CXTPDpi::UnscaleX(int x, int iDpiScaleAbove /*= CXTPDpi::DefaultDpi*/) const
{
	return UnscaleDpi(x, GetDPIX(), iDpiScaleAbove);
}

int CXTPDpi::UnscaleY(int y, int iDpiScaleAbove /*= CXTPDpi::DefaultDpi*/) const
{
	return UnscaleDpi(y, GetDPIY(), iDpiScaleAbove);
}

long CXTPDpi::UnscaleX(long x, int iDpiScaleAbove /*= CXTPDpi::DefaultDpi*/) const
{
	return UnscaleDpi(x, GetDPIX(), iDpiScaleAbove);
}

long CXTPDpi::UnscaleY(long y, int iDpiScaleAbove /*= CXTPDpi::DefaultDpi*/) const
{
	return UnscaleDpi(y, GetDPIY(), iDpiScaleAbove);
}

float CXTPDpi::UnscaleX(float x, int iDpiScaleAbove /*= CXTPDpi::DefaultDpi*/) const
{
	return UnscaleDpi(x, GetDPIX(), iDpiScaleAbove);
}

float CXTPDpi::UnscaleY(float y, int iDpiScaleAbove /*= CXTPDpi::DefaultDpi*/) const
{
	return UnscaleDpi(y, GetDPIY(), iDpiScaleAbove);
}

double CXTPDpi::UnscaleX(double x, int iDpiScaleAbove /*= CXTPDpi::DefaultDpi*/) const
{
	return UnscaleDpi(x, GetDPIX(), iDpiScaleAbove);
}

double CXTPDpi::UnscaleY(double y, int iDpiScaleAbove /*= CXTPDpi::DefaultDpi*/) const
{
	return UnscaleDpi(y, GetDPIY(), iDpiScaleAbove);
}

unsigned int CXTPDpi::UnscaleX(unsigned int x, int iDpiScaleAbove /*= CXTPDpi::DefaultDpi*/) const
{
	return UnscaleDpi(x, GetDPIX(), iDpiScaleAbove);
}

unsigned int CXTPDpi::UnscaleY(unsigned int y, int iDpiScaleAbove /*= CXTPDpi::DefaultDpi*/) const
{
	return UnscaleDpi(y, GetDPIY(), iDpiScaleAbove);
}

int CXTPDpi::ScaleX(HDC hDC, int x, int iDpiScaleAbove /*= CXTPDpi::DefaultDpi*/) const
{
	return ScaleDpiX(x, hDC, iDpiScaleAbove);
}

int CXTPDpi::ScaleY(HDC hDC, int y, int iDpiScaleAbove /*= CXTPDpi::DefaultDpi*/) const
{
	return ScaleDpiY(y, hDC, iDpiScaleAbove);
}

long CXTPDpi::ScaleX(HDC hDC, long x, int iDpiScaleAbove /*= CXTPDpi::DefaultDpi*/) const
{
	return ScaleDpiX(x, hDC, iDpiScaleAbove);
}

long CXTPDpi::ScaleY(HDC hDC, long y, int iDpiScaleAbove /*= CXTPDpi::DefaultDpi*/) const
{
	return ScaleDpiY(y, hDC, iDpiScaleAbove);
}

float CXTPDpi::ScaleX(HDC hDC, float x, int iDpiScaleAbove /*= CXTPDpi::DefaultDpi*/) const
{
	return ScaleDpiX(x, hDC, iDpiScaleAbove);
}

float CXTPDpi::ScaleY(HDC hDC, float y, int iDpiScaleAbove /*= CXTPDpi::DefaultDpi*/) const
{
	return ScaleDpiY(y, hDC, iDpiScaleAbove);
}

double CXTPDpi::ScaleX(HDC hDC, double x, int iDpiScaleAbove /*= CXTPDpi::DefaultDpi*/) const
{
	return ScaleDpiX(x, hDC, iDpiScaleAbove);
}

double CXTPDpi::ScaleY(HDC hDC, double y, int iDpiScaleAbove /*= CXTPDpi::DefaultDpi*/) const
{
	return ScaleDpiY(y, hDC, iDpiScaleAbove);
}

unsigned int CXTPDpi::ScaleX(HDC hDC, unsigned int x,
							 int iDpiScaleAbove /*= CXTPDpi::DefaultDpi*/) const
{
	return ScaleDpiX(x, hDC, iDpiScaleAbove);
}

unsigned int CXTPDpi::ScaleY(HDC hDC, unsigned int y,
							 int iDpiScaleAbove /*= CXTPDpi::DefaultDpi*/) const
{
	return ScaleDpiY(y, hDC, iDpiScaleAbove);
}

int CXTPDpi::UnscaleX(HDC hDC, int x, int iDpiScaleAbove /*= CXTPDpi::DefaultDpi*/) const
{
	return UnscaleDpiX(x, hDC, iDpiScaleAbove);
}

int CXTPDpi::UnscaleY(HDC hDC, int y, int iDpiScaleAbove /*= CXTPDpi::DefaultDpi*/) const
{
	return UnscaleDpiY(y, hDC, iDpiScaleAbove);
}

long CXTPDpi::UnscaleX(HDC hDC, long x, int iDpiScaleAbove /*= CXTPDpi::DefaultDpi*/) const
{
	return UnscaleDpiX(x, hDC, iDpiScaleAbove);
}

long CXTPDpi::UnscaleY(HDC hDC, long y, int iDpiScaleAbove /*= CXTPDpi::DefaultDpi*/) const
{
	return UnscaleDpiY(y, hDC, iDpiScaleAbove);
}

float CXTPDpi::UnscaleX(HDC hDC, float x, int iDpiScaleAbove /*= CXTPDpi::DefaultDpi*/) const
{
	return UnscaleDpiX(x, hDC, iDpiScaleAbove);
}

float CXTPDpi::UnscaleY(HDC hDC, float y, int iDpiScaleAbove /*= CXTPDpi::DefaultDpi*/) const
{
	return UnscaleDpiY(y, hDC, iDpiScaleAbove);
}

double CXTPDpi::UnscaleX(HDC hDC, double x, int iDpiScaleAbove /*= CXTPDpi::DefaultDpi*/) const
{
	return UnscaleDpiX(x, hDC, iDpiScaleAbove);
}

double CXTPDpi::UnscaleY(HDC hDC, double y, int iDpiScaleAbove /*= CXTPDpi::DefaultDpi*/) const
{
	return UnscaleDpiY(y, hDC, iDpiScaleAbove);
}

unsigned int CXTPDpi::UnscaleX(HDC hDC, unsigned int x,
							   int iDpiScaleAbove /*= CXTPDpi::DefaultDpi*/) const
{
	return UnscaleDpiX(x, hDC, iDpiScaleAbove);
}

unsigned int CXTPDpi::UnscaleY(HDC hDC, unsigned int y,
							   int iDpiScaleAbove /*= CXTPDpi::DefaultDpi*/) const
{
	return UnscaleDpiY(y, hDC, iDpiScaleAbove);
}

void CXTPDpi::ScaleRect(RECT* pRect, int iDpiScaleAbove /*= CXTPDpi::DefaultDpi*/) const
{
	ASSERT(NULL != pRect);

	pRect->left   = ScaleX(static_cast<int>(pRect->left), iDpiScaleAbove);
	pRect->right  = ScaleX(static_cast<int>(pRect->right), iDpiScaleAbove);
	pRect->top	= ScaleY(static_cast<int>(pRect->top), iDpiScaleAbove);
	pRect->bottom = ScaleY(static_cast<int>(pRect->bottom), iDpiScaleAbove);
}

RECT CXTPDpi::ScaleRect(const RECT& rc, int iDpiScaleAbove /*= CXTPDpi::DefaultDpi*/) const
{
	RECT rcScaled = rc;
	ScaleRect(&rcScaled, iDpiScaleAbove);
	return rcScaled;
}

void CXTPDpi::UnscaleRect(RECT* pRect, int iDpiScaleAbove /*= CXTPDpi::DefaultDpi*/) const
{
	ASSERT(NULL != pRect);

	pRect->left   = UnscaleX(static_cast<int>(pRect->left), iDpiScaleAbove);
	pRect->right  = UnscaleX(static_cast<int>(pRect->right), iDpiScaleAbove);
	pRect->top	= UnscaleY(static_cast<int>(pRect->top), iDpiScaleAbove);
	pRect->bottom = UnscaleY(static_cast<int>(pRect->bottom), iDpiScaleAbove);
}

RECT CXTPDpi::UnscaleRect(const RECT& rc, int iDpiScaleAbove /*= CXTPDpi::DefaultDpi*/) const
{
	RECT rcUnscaled = rc;
	UnscaleRect(&rcUnscaled, iDpiScaleAbove);
	return rcUnscaled;
}

void CXTPDpi::Scale(RECT* pRect, int iDpiScaleAbove /*= CXTPDpi::DefaultDpi*/) const
{
	ScaleRect(pRect, iDpiScaleAbove);
}

RECT CXTPDpi::Scale(const RECT& rc, int iDpiScaleAbove /*= CXTPDpi::DefaultDpi*/) const
{
	return ScaleRect(rc, iDpiScaleAbove);
}

void CXTPDpi::Unscale(RECT* pRect, int iDpiScaleAbove /*= CXTPDpi::DefaultDpi*/) const
{
	UnscaleRect(pRect, iDpiScaleAbove);
}

RECT CXTPDpi::Unscale(const RECT& rc, int iDpiScaleAbove /*= CXTPDpi::DefaultDpi*/) const
{
	return UnscaleRect(rc, iDpiScaleAbove);
}

void CXTPDpi::ScaleRect(CRect* pRect, int iDpiScaleAbove /*= CXTPDpi::DefaultDpi*/) const
{
	ScaleRect((RECT*)pRect, iDpiScaleAbove);
}

CRect CXTPDpi::ScaleRect(const CRect& rc, int iDpiScaleAbove /*= CXTPDpi::DefaultDpi*/) const
{
	return ScaleRect((const RECT&)rc, iDpiScaleAbove);
}

void CXTPDpi::UnscaleRect(CRect* pRect, int iDpiScaleAbove /*= CXTPDpi::DefaultDpi*/) const
{
	UnscaleRect((RECT*)pRect, iDpiScaleAbove);
}

CRect CXTPDpi::UnscaleRect(const CRect& rc, int iDpiScaleAbove /*= CXTPDpi::DefaultDpi*/) const
{
	return UnscaleRect((const RECT&)rc, iDpiScaleAbove);
}

void CXTPDpi::Scale(CRect* pRect, int iDpiScaleAbove /*= CXTPDpi::DefaultDpi*/) const
{
	ScaleRect(pRect, iDpiScaleAbove);
}

CRect CXTPDpi::Scale(const CRect& rc, int iDpiScaleAbove /*= CXTPDpi::DefaultDpi*/) const
{
	return ScaleRect(rc, iDpiScaleAbove);
}

void CXTPDpi::Unscale(CRect* pRect, int iDpiScaleAbove /*= CXTPDpi::DefaultDpi*/) const
{
	UnscaleRect(pRect, iDpiScaleAbove);
}

CRect CXTPDpi::Unscale(const CRect& rc, int iDpiScaleAbove /*= CXTPDpi::DefaultDpi*/) const
{
	return UnscaleRect(rc, iDpiScaleAbove);
}

void CXTPDpi::ScaleRect(HDC hDC, RECT* pRect, int iDpiScaleAbove /*= CXTPDpi::DefaultDpi*/) const
{
	ASSERT(NULL != pRect);

	pRect->left   = ScaleX(hDC, static_cast<int>(pRect->left), iDpiScaleAbove);
	pRect->right  = ScaleX(hDC, static_cast<int>(pRect->right), iDpiScaleAbove);
	pRect->top	= ScaleY(hDC, static_cast<int>(pRect->top), iDpiScaleAbove);
	pRect->bottom = ScaleY(hDC, static_cast<int>(pRect->bottom), iDpiScaleAbove);
}

RECT CXTPDpi::ScaleRect(HDC hDC, const RECT& rc, int iDpiScaleAbove /*= CXTPDpi::DefaultDpi*/) const
{
	RECT rcScaled = rc;
	ScaleRect(hDC, &rcScaled, iDpiScaleAbove);
	return rcScaled;
}

void CXTPDpi::UnscaleRect(HDC hDC, RECT* pRect, int iDpiScaleAbove /*= CXTPDpi::DefaultDpi*/) const
{
	ASSERT(NULL != pRect);

	pRect->left   = UnscaleX(hDC, static_cast<int>(pRect->left), iDpiScaleAbove);
	pRect->right  = UnscaleX(hDC, static_cast<int>(pRect->right), iDpiScaleAbove);
	pRect->top	= UnscaleY(hDC, static_cast<int>(pRect->top), iDpiScaleAbove);
	pRect->bottom = UnscaleY(hDC, static_cast<int>(pRect->bottom), iDpiScaleAbove);
}

RECT CXTPDpi::UnscaleRect(HDC hDC, const RECT& rc,
						  int iDpiScaleAbove /*= CXTPDpi::DefaultDpi*/) const
{
	RECT rcUnscaled = rc;
	UnscaleRect(hDC, &rcUnscaled, iDpiScaleAbove);
	return rcUnscaled;
}

void CXTPDpi::Scale(HDC hDC, RECT* pRect, int iDpiScaleAbove /*= CXTPDpi::DefaultDpi*/) const
{
	ScaleRect(hDC, pRect, iDpiScaleAbove);
}

RECT CXTPDpi::Scale(HDC hDC, const RECT& rc, int iDpiScaleAbove /*= CXTPDpi::DefaultDpi*/) const
{
	return ScaleRect(hDC, rc, iDpiScaleAbove);
}

void CXTPDpi::Unscale(HDC hDC, RECT* pRect, int iDpiScaleAbove /*= CXTPDpi::DefaultDpi*/) const
{
	UnscaleRect(hDC, pRect, iDpiScaleAbove);
}

RECT CXTPDpi::Unscale(HDC hDC, const RECT& rc, int iDpiScaleAbove /*= CXTPDpi::DefaultDpi*/) const
{
	return UnscaleRect(hDC, rc, iDpiScaleAbove);
}

void CXTPDpi::ScaleRect(HDC hDC, CRect* pRect, int iDpiScaleAbove /*= CXTPDpi::DefaultDpi*/) const
{
	ScaleRect(hDC, (RECT*)pRect, iDpiScaleAbove);
}

CRect CXTPDpi::ScaleRect(HDC hDC, const CRect& rc,
						 int iDpiScaleAbove /*= CXTPDpi::DefaultDpi*/) const
{
	return ScaleRect(hDC, (const RECT&)rc, iDpiScaleAbove);
}

void CXTPDpi::UnscaleRect(HDC hDC, CRect* pRect, int iDpiScaleAbove /*= CXTPDpi::DefaultDpi*/) const
{
	UnscaleRect(hDC, (RECT*)pRect, iDpiScaleAbove);
}

CRect CXTPDpi::UnscaleRect(HDC hDC, const CRect& rc,
						   int iDpiScaleAbove /*= CXTPDpi::DefaultDpi*/) const
{
	return UnscaleRect(hDC, (const RECT&)rc, iDpiScaleAbove);
}

void CXTPDpi::Scale(HDC hDC, CRect* pRect, int iDpiScaleAbove /*= CXTPDpi::DefaultDpi*/) const
{
	ScaleRect(hDC, pRect, iDpiScaleAbove);
}

CRect CXTPDpi::Scale(HDC hDC, const CRect& rc, int iDpiScaleAbove /*= CXTPDpi::DefaultDpi*/) const
{
	return ScaleRect(hDC, rc, iDpiScaleAbove);
}

void CXTPDpi::Unscale(HDC hDC, CRect* pRect, int iDpiScaleAbove /*= CXTPDpi::DefaultDpi*/) const
{
	UnscaleRect(hDC, pRect, iDpiScaleAbove);
}

CRect CXTPDpi::Unscale(HDC hDC, const CRect& rc, int iDpiScaleAbove /*= CXTPDpi::DefaultDpi*/) const
{
	return UnscaleRect(hDC, rc, iDpiScaleAbove);
}

void CXTPDpi::ScalePoint(POINT* pPoint, int iDpiScaleAbove /*= CXTPDpi::DefaultDpi*/) const
{
	ASSERT(NULL != pPoint);

	pPoint->x = ScaleX(static_cast<int>(pPoint->x), iDpiScaleAbove);
	pPoint->y = ScaleY(static_cast<int>(pPoint->y), iDpiScaleAbove);
}

POINT CXTPDpi::ScalePoint(const POINT& pt, int iDpiScaleAbove /*= CXTPDpi::DefaultDpi*/) const
{
	POINT ptScaled = pt;
	ScalePoint(&ptScaled, iDpiScaleAbove);
	return ptScaled;
}

void CXTPDpi::UnscalePoint(POINT* pPoint, int iDpiScaleAbove /*= CXTPDpi::DefaultDpi*/) const
{
	ASSERT(NULL != pPoint);

	pPoint->x = UnscaleX(static_cast<int>(pPoint->x), iDpiScaleAbove);
	pPoint->y = UnscaleY(static_cast<int>(pPoint->y), iDpiScaleAbove);
}

POINT CXTPDpi::UnscalePoint(const POINT& pt, int iDpiScaleAbove /*= CXTPDpi::DefaultDpi*/) const
{
	POINT ptUnscaled = pt;
	UnscalePoint(&ptUnscaled, iDpiScaleAbove);
	return ptUnscaled;
}

void CXTPDpi::Scale(POINT* pPoint, int iDpiScaleAbove /*= CXTPDpi::DefaultDpi*/) const
{
	ScalePoint(pPoint, iDpiScaleAbove);
}

POINT CXTPDpi::Scale(const POINT& pt, int iDpiScaleAbove /*= CXTPDpi::DefaultDpi*/) const
{
	return ScalePoint(pt, iDpiScaleAbove);
}

void CXTPDpi::Unscale(POINT* pPoint, int iDpiScaleAbove /*= CXTPDpi::DefaultDpi*/) const
{
	UnscalePoint(pPoint, iDpiScaleAbove);
}

POINT CXTPDpi::Unscale(const POINT& pt, int iDpiScaleAbove /*= CXTPDpi::DefaultDpi*/) const
{
	return UnscalePoint(pt, iDpiScaleAbove);
}

void CXTPDpi::ScalePoint(CPoint* pPoint, int iDpiScaleAbove /*= CXTPDpi::DefaultDpi*/) const
{
	ScalePoint((POINT*)pPoint, iDpiScaleAbove);
}

CPoint CXTPDpi::ScalePoint(const CPoint& pt, int iDpiScaleAbove /*= CXTPDpi::DefaultDpi*/) const
{
	return ScalePoint((const POINT&)pt, iDpiScaleAbove);
}

void CXTPDpi::UnscalePoint(CPoint* pPoint, int iDpiScaleAbove /*= CXTPDpi::DefaultDpi*/) const
{
	UnscalePoint((POINT*)pPoint, iDpiScaleAbove);
}

CPoint CXTPDpi::UnscalePoint(const CPoint& pt, int iDpiScaleAbove /*= CXTPDpi::DefaultDpi*/) const
{
	return UnscalePoint((const POINT&)pt, iDpiScaleAbove);
}

void CXTPDpi::Scale(CPoint* pPoint, int iDpiScaleAbove /*= CXTPDpi::DefaultDpi*/) const
{
	ScalePoint(pPoint, iDpiScaleAbove);
}

CPoint CXTPDpi::Scale(const CPoint& pt, int iDpiScaleAbove /*= CXTPDpi::DefaultDpi*/) const
{
	return ScalePoint(pt, iDpiScaleAbove);
}

void CXTPDpi::Unscale(CPoint* pPoint, int iDpiScaleAbove /*= CXTPDpi::DefaultDpi*/) const
{
	UnscalePoint(pPoint, iDpiScaleAbove);
}

CPoint CXTPDpi::Unscale(const CPoint& pt, int iDpiScaleAbove /*= CXTPDpi::DefaultDpi*/) const
{
	return UnscalePoint(pt, iDpiScaleAbove);
}

void CXTPDpi::ScalePoint(HDC hDC, POINT* pPoint, int iDpiScaleAbove /*= CXTPDpi::DefaultDpi*/) const
{
	ASSERT(NULL != pPoint);

	pPoint->x = ScaleX(hDC, static_cast<int>(pPoint->x), iDpiScaleAbove);
	pPoint->y = ScaleY(hDC, static_cast<int>(pPoint->y), iDpiScaleAbove);
}

POINT CXTPDpi::ScalePoint(HDC hDC, const POINT& pt,
						  int iDpiScaleAbove /*= CXTPDpi::DefaultDpi*/) const
{
	POINT ptScaled = pt;
	ScalePoint(hDC, &ptScaled, iDpiScaleAbove);
	return ptScaled;
}

void CXTPDpi::UnscalePoint(HDC hDC, POINT* pPoint,
						   int iDpiScaleAbove /*= CXTPDpi::DefaultDpi*/) const
{
	ASSERT(NULL != pPoint);

	pPoint->x = UnscaleX(hDC, static_cast<int>(pPoint->x), iDpiScaleAbove);
	pPoint->y = UnscaleY(hDC, static_cast<int>(pPoint->y), iDpiScaleAbove);
}

POINT CXTPDpi::UnscalePoint(HDC hDC, const POINT& pt,
							int iDpiScaleAbove /*= CXTPDpi::DefaultDpi*/) const
{
	POINT ptUnscaled = pt;
	UnscalePoint(hDC, &ptUnscaled, iDpiScaleAbove);
	return ptUnscaled;
}

void CXTPDpi::Scale(HDC hDC, POINT* pPoint, int iDpiScaleAbove /*= CXTPDpi::DefaultDpi*/) const
{
	ScalePoint(hDC, pPoint, iDpiScaleAbove);
}

POINT CXTPDpi::Scale(HDC hDC, const POINT& pt, int iDpiScaleAbove /*= CXTPDpi::DefaultDpi*/) const
{
	return ScalePoint(hDC, pt, iDpiScaleAbove);
}

void CXTPDpi::Unscale(HDC hDC, POINT* pPoint, int iDpiScaleAbove /*= CXTPDpi::DefaultDpi*/) const
{
	UnscalePoint(hDC, pPoint, iDpiScaleAbove);
}

POINT CXTPDpi::Unscale(HDC hDC, const POINT& pt, int iDpiScaleAbove /*= CXTPDpi::DefaultDpi*/) const
{
	return UnscalePoint(hDC, pt, iDpiScaleAbove);
}

void CXTPDpi::ScalePoint(HDC hDC, CPoint* pPoint,
						 int iDpiScaleAbove /*= CXTPDpi::DefaultDpi*/) const
{
	ScalePoint(hDC, (POINT*)pPoint, iDpiScaleAbove);
}

CPoint CXTPDpi::ScalePoint(HDC hDC, const CPoint& pt,
						   int iDpiScaleAbove /*= CXTPDpi::DefaultDpi*/) const
{
	return ScalePoint(hDC, (const POINT&)pt, iDpiScaleAbove);
}

void CXTPDpi::UnscalePoint(HDC hDC, CPoint* pPoint,
						   int iDpiScaleAbove /*= CXTPDpi::DefaultDpi*/) const
{
	UnscalePoint(hDC, (POINT*)pPoint, iDpiScaleAbove);
}

CPoint CXTPDpi::UnscalePoint(HDC hDC, const CPoint& pt,
							 int iDpiScaleAbove /*= CXTPDpi::DefaultDpi*/) const
{
	return UnscalePoint(hDC, (const POINT&)pt, iDpiScaleAbove);
}

void CXTPDpi::Scale(HDC hDC, CPoint* pPoint, int iDpiScaleAbove /*= CXTPDpi::DefaultDpi*/) const
{
	ScalePoint(hDC, pPoint, iDpiScaleAbove);
}

CPoint CXTPDpi::Scale(HDC hDC, const CPoint& pt, int iDpiScaleAbove /*= CXTPDpi::DefaultDpi*/) const
{
	return ScalePoint(hDC, pt, iDpiScaleAbove);
}

void CXTPDpi::Unscale(HDC hDC, CPoint* pPoint, int iDpiScaleAbove /*= CXTPDpi::DefaultDpi*/) const
{
	UnscalePoint(hDC, pPoint, iDpiScaleAbove);
}

CPoint CXTPDpi::Unscale(HDC hDC, const CPoint& pt,
						int iDpiScaleAbove /*= CXTPDpi::DefaultDpi*/) const
{
	return UnscalePoint(hDC, pt, iDpiScaleAbove);
}

void CXTPDpi::ScaleSize(SIZE* pSize, int iDpiScaleAbove /*= CXTPDpi::DefaultDpi*/) const
{
	ASSERT(NULL != pSize);

	pSize->cx = ScaleX(static_cast<int>(pSize->cx), iDpiScaleAbove);
	pSize->cy = ScaleY(static_cast<int>(pSize->cy), iDpiScaleAbove);
}

SIZE CXTPDpi::ScaleSize(const SIZE& sz, int iDpiScaleAbove /*= CXTPDpi::DefaultDpi*/) const
{
	SIZE szScaled = sz;
	ScaleSize(&szScaled, iDpiScaleAbove);
	return szScaled;
}

void CXTPDpi::UnscaleSize(SIZE* pSize, int iDpiScaleAbove /*= CXTPDpi::DefaultDpi*/) const
{
	ASSERT(NULL != pSize);

	pSize->cx = UnscaleX(static_cast<int>(pSize->cx), iDpiScaleAbove);
	pSize->cy = UnscaleY(static_cast<int>(pSize->cy), iDpiScaleAbove);
}

SIZE CXTPDpi::UnscaleSize(const SIZE& sz, int iDpiScaleAbove /*= CXTPDpi::DefaultDpi*/) const
{
	SIZE szUnscaled = sz;
	UnscaleSize(&szUnscaled, iDpiScaleAbove);
	return szUnscaled;
}

void CXTPDpi::Scale(SIZE* pSize, int iDpiScaleAbove /*= CXTPDpi::DefaultDpi*/) const
{
	ScaleSize(pSize, iDpiScaleAbove);
}

SIZE CXTPDpi::Scale(const SIZE& sz, int iDpiScaleAbove /*= CXTPDpi::DefaultDpi*/) const
{
	return ScaleSize(sz, iDpiScaleAbove);
}

void CXTPDpi::Unscale(SIZE* pSize, int iDpiScaleAbove /*= CXTPDpi::DefaultDpi*/) const
{
	UnscaleSize(pSize, iDpiScaleAbove);
}

SIZE CXTPDpi::Unscale(const SIZE& sz, int iDpiScaleAbove /*= CXTPDpi::DefaultDpi*/) const
{
	return UnscaleSize(sz, iDpiScaleAbove);
}

void CXTPDpi::ScaleSize(CSize* pSize, int iDpiScaleAbove /*= CXTPDpi::DefaultDpi*/) const
{
	ScaleSize((SIZE*)pSize, iDpiScaleAbove);
}

CSize CXTPDpi::ScaleSize(const CSize& sz, int iDpiScaleAbove /*= CXTPDpi::DefaultDpi*/) const
{
	return ScaleSize((const SIZE&)sz, iDpiScaleAbove);
}

void CXTPDpi::UnscaleSize(CSize* pSize, int iDpiScaleAbove /*= CXTPDpi::DefaultDpi*/) const
{
	UnscaleSize((SIZE*)pSize, iDpiScaleAbove);
}

CSize CXTPDpi::UnscaleSize(const CSize& sz, int iDpiScaleAbove /*= CXTPDpi::DefaultDpi*/) const
{
	return UnscaleSize((const SIZE&)sz, iDpiScaleAbove);
}

void CXTPDpi::Scale(CSize* pSize, int iDpiScaleAbove /*= CXTPDpi::DefaultDpi*/) const
{
	ScaleSize(pSize, iDpiScaleAbove);
}

CSize CXTPDpi::Scale(const CSize& sz, int iDpiScaleAbove /*= CXTPDpi::DefaultDpi*/) const
{
	return ScaleSize(sz, iDpiScaleAbove);
}

void CXTPDpi::Unscale(CSize* pSize, int iDpiScaleAbove /*= CXTPDpi::DefaultDpi*/) const
{
	UnscaleSize(pSize, iDpiScaleAbove);
}

CSize CXTPDpi::Unscale(const CSize& sz, int iDpiScaleAbove /*= CXTPDpi::DefaultDpi*/) const
{
	return UnscaleSize(sz, iDpiScaleAbove);
}

void CXTPDpi::ScaleSize(HDC hDC, SIZE* pSize, int iDpiScaleAbove /*= CXTPDpi::DefaultDpi*/) const
{
	ASSERT(NULL != pSize);

	pSize->cx = ScaleX(hDC, static_cast<int>(pSize->cx), iDpiScaleAbove);
	pSize->cy = ScaleY(hDC, static_cast<int>(pSize->cy), iDpiScaleAbove);
}

SIZE CXTPDpi::ScaleSize(HDC hDC, const SIZE& sz, int iDpiScaleAbove /*= CXTPDpi::DefaultDpi*/) const
{
	SIZE szScaled = sz;
	ScaleSize(hDC, &szScaled, iDpiScaleAbove);
	return szScaled;
}

void CXTPDpi::UnscaleSize(HDC hDC, SIZE* pSize, int iDpiScaleAbove /*= CXTPDpi::DefaultDpi*/) const
{
	ASSERT(NULL != pSize);

	pSize->cx = UnscaleX(hDC, static_cast<int>(pSize->cx), iDpiScaleAbove);
	pSize->cy = UnscaleY(hDC, static_cast<int>(pSize->cy), iDpiScaleAbove);
}

SIZE CXTPDpi::UnscaleSize(HDC hDC, const SIZE& sz,
						  int iDpiScaleAbove /*= CXTPDpi::DefaultDpi*/) const
{
	SIZE szUnscaled = sz;
	UnscaleSize(hDC, &szUnscaled, iDpiScaleAbove);
	return szUnscaled;
}

void CXTPDpi::Scale(HDC hDC, SIZE* pSize, int iDpiScaleAbove /*= CXTPDpi::DefaultDpi*/) const
{
	ScaleSize(hDC, pSize, iDpiScaleAbove);
}

SIZE CXTPDpi::Scale(HDC hDC, const SIZE& sz, int iDpiScaleAbove /*= CXTPDpi::DefaultDpi*/) const
{
	return ScaleSize(hDC, sz, iDpiScaleAbove);
}

void CXTPDpi::Unscale(HDC hDC, SIZE* pSize, int iDpiScaleAbove /*= CXTPDpi::DefaultDpi*/) const
{
	UnscaleSize(hDC, pSize, iDpiScaleAbove);
}

SIZE CXTPDpi::Unscale(HDC hDC, const SIZE& sz, int iDpiScaleAbove /*= CXTPDpi::DefaultDpi*/) const
{
	return UnscaleSize(hDC, sz, iDpiScaleAbove);
}

void CXTPDpi::ScaleSize(HDC hDC, CSize* pSize, int iDpiScaleAbove /*= CXTPDpi::DefaultDpi*/) const
{
	ScaleSize(hDC, (SIZE*)pSize, iDpiScaleAbove);
}

CSize CXTPDpi::ScaleSize(HDC hDC, const CSize& sz,
						 int iDpiScaleAbove /*= CXTPDpi::DefaultDpi*/) const
{
	return ScaleSize(hDC, (const SIZE&)sz, iDpiScaleAbove);
}

void CXTPDpi::UnscaleSize(HDC hDC, CSize* pSize, int iDpiScaleAbove /*= CXTPDpi::DefaultDpi*/) const
{
	UnscaleSize(hDC, (SIZE*)pSize, iDpiScaleAbove);
}

CSize CXTPDpi::UnscaleSize(HDC hDC, const CSize& sz,
						   int iDpiScaleAbove /*= CXTPDpi::DefaultDpi*/) const
{
	return UnscaleSize(hDC, (const SIZE&)sz, iDpiScaleAbove);
}

void CXTPDpi::Scale(HDC hDC, CSize* pSize, int iDpiScaleAbove /*= CXTPDpi::DefaultDpi*/) const
{
	ScaleSize(hDC, pSize, iDpiScaleAbove);
}

CSize CXTPDpi::Scale(HDC hDC, const CSize& sz, int iDpiScaleAbove /*= CXTPDpi::DefaultDpi*/) const
{
	return ScaleSize(hDC, sz, iDpiScaleAbove);
}

void CXTPDpi::Unscale(HDC hDC, CSize* pSize, int iDpiScaleAbove /*= CXTPDpi::DefaultDpi*/) const
{
	UnscaleSize(hDC, pSize, iDpiScaleAbove);
}

CSize CXTPDpi::Unscale(HDC hDC, const CSize& sz, int iDpiScaleAbove /*= CXTPDpi::DefaultDpi*/) const
{
	return UnscaleSize(hDC, sz, iDpiScaleAbove);
}

template<class T>
AFX_INLINE T SelectDpiSpecificImpl(T upToDpi120, T upToDpi144, T upToDpi192, T theLargest)
{
	int dpi = XTPDpiHelper()->GetDPIX();
	ASSERT(dpi == XTPDpiHelper()->GetDPIY());

	if (96 <= dpi && dpi < 120)
		return upToDpi120;
	else if (120 <= dpi && dpi < 144)
		return upToDpi144;
	else if (144 <= dpi && dpi < 192)
		return upToDpi192;

	return theLargest;
}

int CXTPDpi::SelectDpiSpecific(int upToDpi120, int upToDpi144, int upToDpi192, int theLargest) const
{
	return SelectDpiSpecificImpl(upToDpi120, upToDpi144, upToDpi192, theLargest);
}

LPCTSTR CXTPDpi::SelectDpiSpecific(LPCTSTR upToDpi120, LPCTSTR upToDpi144, LPCTSTR upToDpi192,
								   LPCTSTR theLargest) const
{
	return SelectDpiSpecificImpl(upToDpi120, upToDpi144, upToDpi192, theLargest);
}

void* CXTPDpi::SelectDpiSpecific(void* upToDpi120, void* upToDpi144, void* upToDpi192,
								 void* theLargest) const
{
	return SelectDpiSpecificImpl(upToDpi120, upToDpi144, upToDpi192, theLargest);
}

BOOL CXTPDpi::IsDefaultDpiX() const
{
	return ScaleX(DefaultDpi) == DefaultDpi;
}

BOOL CXTPDpi::IsDefaultDpiY() const
{
	return ScaleY(DefaultDpi) == DefaultDpi;
}

BOOL CXTPDpi::IsDefaultDpi() const
{
	return IsDefaultDpiX() && IsDefaultDpiY();
}

//////////////////////////////////////////////////////////////////////////
// CXTPPrimitiveDrawer

CXTPPrimitiveDrawer* AFX_CDECL XTPPrimitiveDrawer()
{
	return &CXTPSingleton<CXTPPrimitiveDrawer>::Instance();
}

CXTPPrimitiveDrawer::CXTPPrimitiveDrawer()
	: m_hCodejockMemFont(NULL)
	, m_bCodejockFontLoadingAttempted(FALSE)
{
	::InitializeCriticalSection(&m_fontAccess);

	HMODULE hGdi32			  = GetModuleHandle(_T("gdi32"));
	m_pfnAddFontMemResourceEx = XTPToFunctionPtr<PFNAddFontMemResourceEx>(
		::GetProcAddress(hGdi32, "AddFontMemResourceEx"));
	m_pfnRemoveFontMemResourceEx = XTPToFunctionPtr<PFNRemoveFontMemResourceEx>(
		::GetProcAddress(hGdi32, "RemoveFontMemResourceEx"));
}

CXTPPrimitiveDrawer::~CXTPPrimitiveDrawer()
{
	if (NULL != m_fntCodejock.m_hObject)
	{
		m_fntCodejock.DeleteObject();
	}

	if (NULL != m_hCodejockMemFont && NULL != m_pfnRemoveFontMemResourceEx)
	{
		m_pfnRemoveFontMemResourceEx(m_hCodejockMemFont);
	}

	::DeleteCriticalSection(&m_fontAccess);
}

void CXTPPrimitiveDrawer::InitFont()
{
	if (!m_bCodejockFontLoadingAttempted)
	{
		::EnterCriticalSection(&m_fontAccess);
		if (!m_bCodejockFontLoadingAttempted)
		{
			InitMemFont();

			BYTE bQuality = (BYTE)(XTPDpiHelper()->IsDefaultDpi()
										   && XTPSystemVersion()->IsWinNT4OrGreater()
									   ? ANTIALIASED_QUALITY
									   : DEFAULT_QUALITY);

			// Even if memory font is not initialized or blocked via Untrusted Font Blocking
			// the font can still be installed into %WINDIR%\Fonts so try to load it anyway.
			if (m_fntCodejock.CreateFont(0, 0, 0, 0, 0, 0, 0, 0, DEFAULT_CHARSET,
										 OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, bQuality,
										 FIXED_PITCH, _T("Codejock")))
			{
				m_fntCodejock.GetLogFont(&m_lfCodejock);
			}

			m_bCodejockFontLoadingAttempted = TRUE;
		}

		::LeaveCriticalSection(&m_fontAccess);
	}
}

void CXTPPrimitiveDrawer::InitMemFont()
{
	if (NULL == m_hCodejockMemFont && NULL != m_pfnAddFontMemResourceEx)
	{
		HMODULE hResModule = NULL;
		HRSRC hResource	= XTPResourceManager()->FindResource(hResModule,
																MAKEINTRESOURCE(XTP_IDC_FONT_CODEJOCK),
																RT_FONT);
		ASSERT(NULL != hResource);
		if (NULL != hResource)
		{
			HGLOBAL hGlobRes = ::LoadResource(hResModule, hResource);
			if (NULL != hGlobRes)
			{
				LPVOID pFontData = ::LockResource(hGlobRes);
				if (NULL != pFontData)
				{
					DWORD cbFontData   = ::SizeofResource(hResModule, hResource);
					DWORD nNumFonts	= 0;
					m_hCodejockMemFont = m_pfnAddFontMemResourceEx(pFontData, cbFontData, NULL,
																   &nNumFonts);

					::UnlockResource(hGlobRes);
				}
			}
		}
	}
}

void CXTPPrimitiveDrawer::DrawSymbol(CDC* pDC, XTPPrimitiveSymbol nSymbol, const CRect& rc,
									 COLORREF crForeColor /*= COLORREF_NULL*/,
									 COLORREF crBackColor /*= COLORREF_NULL*/,
									 BOOL bOutline /*= FALSE*/, HPEN hOutlinePen /*= NULL*/)
{
	ASSERT(pDC);
	ASSERT(xtpPrimitiveSymbolFirst <= nSymbol && nSymbol <= xtpPrimitiveSymbolLast);

	InitFont();

	if (NULL != m_fntCodejock.m_hObject)
	{
		const TCHAR strSymbol[] = { static_cast<TCHAR>(nSymbol), _T('\0') };
		int nFontSize			= XTPDrawHelpers()->ComputeFittingFontHeight(pDC, strSymbol, 1, rc,
																	 m_fntCodejock);
		if (0 != nFontSize)
		{
			LOGFONT fontData  = m_lfCodejock;
			fontData.lfHeight = nFontSize;

			CXTPFont font;
			if (font.CreateFontIndirect(&fontData))
			{
				CFont* pOldFont = pDC->SelectObject(&font);
				CSize szText	= pDC->GetTextExtent(strSymbol, 1);
				pDC->SelectObject(pOldFont);

				CRect rcOut = MakeEvenRect(rc);

				rcOut.OffsetRect((rcOut.Width() - szText.cx) / 2, (rcOut.Height() - szText.cy) / 2);

				if (!bOutline)
				{
					XTPDrawHelpers()->DrawFontText(pDC, &font, strSymbol, 1, rcOut,
												   DT_SINGLELINE | DT_NOCLIP, crForeColor,
												   crBackColor);
				}
				else
				{
					if (COLORREF_NULL == crForeColor)
					{
						crForeColor = pDC->GetTextColor();
					}

					CPen pen;
					if (NULL == hOutlinePen)
					{
						pen.CreatePen(PS_GEOMETRIC | PS_SOLID, XTP_DPI_X(1), crForeColor);
					}
					else
					{
						pen.Attach(hOutlinePen);
					}

					CPen* pOldPen = pDC->SelectObject(&pen);
					pDC->BeginPath();

					XTPDrawHelpers()->DrawFontText(pDC, &font, strSymbol, 1, rcOut,
												   DT_SINGLELINE | DT_NOCLIP, crForeColor,
												   crBackColor);

					pDC->EndPath();
					pDC->StrokePath();
					pDC->SelectObject(pOldPen);

					if (NULL != hOutlinePen)
					{
						pen.Detach();
					}
					else
					{
						pen.DeleteObject();
					}
				}
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////

//*************************************************************
IMPLEMENT_DYNAMIC(CXTPPrintOptions, CXTPCmdTarget)

LCID CXTPPrintOptions::GetActiveLCID()
{
	return LOCALE_USER_DEFAULT;
}

BOOL CXTPPrintOptions::IsMarginsMeasureInches()
{
	TCHAR szResult[5];

	int nResult = ::GetLocaleInfo(GetActiveLCID(), LOCALE_IMEASURE, szResult, 4);
	ASSERT(nResult == 2);
	UNUSED(nResult);
	long nIsInches = _ttoi(szResult);

	return nIsInches != 0;
}

CXTPPrintOptions::CXTPPrintOptions()
{
	BOOL bIsInches = IsMarginsMeasureInches();
	int nMargin	= bIsInches ? (1000 * 1 / 2) : (10 * 100);
	m_rcMargins.SetRect(nMargin, nMargin, nMargin, nMargin);

	m_pPageHeader = new CXTPPrintPageHeaderFooter(this, TRUE);
	m_pPageFooter = new CXTPPrintPageHeaderFooter(this, FALSE);

	m_bBlackWhitePrinting = FALSE;
	m_nBlackWhiteContrast = 0;

	//#ifdef _XTP_ACTIVEX
	//  EnableAutomation();
	//  EnableTypeLib();
	//#endif
}

CXTPPrintOptions::~CXTPPrintOptions()
{
	CMDTARGET_RELEASE(m_pPageHeader);
	CMDTARGET_RELEASE(m_pPageFooter);
}

void CXTPPrintOptions::Set(const CXTPPrintOptions* pSrc)
{
	ASSERT(pSrc);
	if (!pSrc)
		return;

	m_rcMargins = pSrc->m_rcMargins;

	if (m_pPageHeader)
		m_pPageHeader->Set(pSrc->m_pPageHeader);

	if (m_pPageFooter)
		m_pPageFooter->Set(pSrc->m_pPageFooter);

	m_bBlackWhitePrinting = pSrc->m_bBlackWhitePrinting;
	m_nBlackWhiteContrast = pSrc->m_nBlackWhiteContrast;
}

CRect CXTPPrintOptions::GetMarginsHimetric()
{
	BOOL bIsInches = IsMarginsMeasureInches();

	if (!bIsInches)
	{
		return m_rcMargins;
	}

	int nMul_i2m = 254;
	int nDiv_i2m = 100;

	CRect rcMarginsHimetric;
	rcMarginsHimetric.left   = m_rcMargins.left * nMul_i2m / nDiv_i2m;
	rcMarginsHimetric.top	= m_rcMargins.top * nMul_i2m / nDiv_i2m;
	rcMarginsHimetric.right  = m_rcMargins.right * nMul_i2m / nDiv_i2m;
	rcMarginsHimetric.bottom = m_rcMargins.bottom * nMul_i2m / nDiv_i2m;

	return rcMarginsHimetric;
}

CRect CXTPPrintOptions::GetMarginsLP(CDC* pDC)
{
	ASSERT(pDC);
	if (!pDC)
		return CRect(0, 0, 0, 0);

	CRect rcMarginsHI = GetMarginsHimetric();

	CSize szLT(rcMarginsHI.left, rcMarginsHI.top);
	CSize szRB(rcMarginsHI.right, rcMarginsHI.bottom);

	pDC->HIMETRICtoLP(&szLT);
	pDC->HIMETRICtoLP(&szRB);

	CRect rcLP(szLT.cx, szLT.cy, szRB.cx, szRB.cy);

	return rcLP;
}

CXTPPrintPageHeaderFooter* CXTPPrintOptions::GetPageHeader()
{
	return m_pPageHeader;
}

CXTPPrintPageHeaderFooter* CXTPPrintOptions::GetPageFooter()
{
	return m_pPageFooter;
}

/////////////////////////////////////////////////////////////////////////////
// Printing Dialog

class XTP_PRINT_STATE : public CNoTrackObject
{
public:
	// printing abort
	BOOL m_bUserAbort;
};

PROCESS_LOCAL(XTP_PRINT_STATE, g_xtpPrintState)

BOOL CALLBACK _XTPAbortProc(HDC, int)
{
	XTP_PRINT_STATE* pState = g_xtpPrintState;
	MSG msg;
	while (!pState->m_bUserAbort && ::PeekMessage(&msg, NULL, NULL, NULL, PM_NOREMOVE))
	{
		if (!XTPGetThread()->PumpMessage())
			return FALSE; // terminate if WM_QUIT received
	}
	return !pState->m_bUserAbort;
}

CXTPPrintingDialog::CXTPPrintingDialog(CWnd* pParent)
{
	Create(CXTPPrintingDialog::IDD, pParent); // modeless !
	g_xtpPrintState->m_bUserAbort = FALSE;
}

BOOL CXTPPrintingDialog::OnInitDialog()
{
	// SetWindowText(AfxGetAppName());
	CenterWindow();
	return CDialog::OnInitDialog();
}

void CXTPPrintingDialog::OnCancel()
{
	g_xtpPrintState->m_bUserAbort = TRUE; // flag that user aborted print
	CDialog::OnCancel();
}

void AFX_CDECL CXTPPrintPageHeaderFooter::DoInsertHFFormatSpecifierViaMenu(CWnd* pParent,
																		   CEdit* pEdit,
																		   CButton* pButton)
{
	CMap<UINT, UINT, CString, CString&> menuData;
	menuData[XTP_ID_HF_FORMAT_D1]  = _T("&d");
	menuData[XTP_ID_HF_FORMAT_D2]  = _T("&D");
	menuData[XTP_ID_HF_FORMAT_T1]  = _T("&t");
	menuData[XTP_ID_HF_FORMAT_T2]  = _T("&T");
	menuData[XTP_ID_HF_FORMAT_P1]  = _T("&p");
	menuData[XTP_ID_HF_FORMAT_P2]  = _T("&P");
	menuData[XTP_ID_HF_FORMAT_B]   = _T("&b");
	menuData[XTP_ID_HF_FORMAT_W]   = _T("&w");
	menuData[XTP_ID_HF_FORMAT_UMP] = _T("&&");
	menuData[XTP_ID_HF_FORMAT_N]   = _T("\\n");

	if (!pParent || !pEdit || !pButton)
	{
		ASSERT(FALSE);
		return;
	}
	CRect rcButton;
	pButton->GetWindowRect(&rcButton);

	CPoint ptMenu(rcButton.left, rcButton.bottom + 1);

	CMenu menu;
	CXTPResourceManager::AssertValid(
		XTPResourceManager()->LoadMenu(&menu, XTP_IDR_MENU_HEADERFOOTER_FORMATS));

	// track menu
	int nMenuResult = menu.GetSubMenu(0)->TrackPopupMenu(TPM_RETURNCMD | TPM_LEFTALIGN
															 | TPM_RIGHTBUTTON,
														 ptMenu.x, ptMenu.y, pParent, NULL);

	pEdit->SetFocus();

	CString strItem = menuData[(UINT)nMenuResult];
	if (!strItem.IsEmpty())
	{
		pEdit->ReplaceSel(strItem, TRUE);
	}
}

COLORREF XTPOfficeBorder_GetBorderColor(bool bTopLeft)
{
	UNREFERENCED_PARAMETER(bTopLeft);

#ifndef _XTP_ACTIVEX
#	if defined(_XTP_INCLUDE_COMMANDBARS)

	CXTPPaintManager* pPaintManager = CXTPPaintManager::GetInstanceAddRef();
	XTPPaintTheme nPaintTheme		= pPaintManager->BaseTheme();
	CXTPPaintManager::ReleaseInstance();

	switch (nPaintTheme)
	{
		case xtpThemeOfficeXP:
		case xtpThemeOffice2000:
			return GetXtremeColor(UINT(bTopLeft ? COLOR_3DSHADOW : COLOR_3DHIGHLIGHT));
		case xtpThemeOffice2003: return GetXtremeColor(XPCOLOR_FRAME);
		case xtpThemeNativeWinXP: return GetXtremeColor(XPCOLOR_STATICFRAME);
	}
#	endif
#endif

	return GetXtremeColor(XPCOLOR_3DSHADOW);
}

void XTPOfficeBorder_OnPaint(CWnd* pWnd, COLORREF clr1, COLORREF clr2)
{
	ASSERT_VALID(pWnd);

	CWindowDC dc(pWnd);

	CXTPWindowRect rWindow(pWnd);
	pWnd->ScreenToClient(rWindow);

	int nBorderWidth = -rWindow.top;

	rWindow.OffsetRect(-rWindow.left, -rWindow.top);

	// draw 1px. border.
	dc.Draw3dRect(&rWindow, clr1, clr2);

	if (nBorderWidth > 1)
	{
		rWindow.DeflateRect(1, 1);
		dc.Draw3dRect(&rWindow, XTPColorManager()->GetColor(COLOR_WINDOW),
					  XTPColorManager()->GetColor(COLOR_WINDOW));
	}
}

//////////////////////////////////////////////////////////////////////////
#ifdef _XTP_ACTIVEX

BEGIN_INTERFACE_MAP(CXTPPrinterInfo, CXTPCmdTarget)
	INTERFACE_PART(CXTPPrinterInfo, XTPDIID_XtremePrinterInfo, Dispatch)
END_INTERFACE_MAP()

IMPLEMENT_OLETYPELIB_EX(CXTPPrinterInfo, XTPDIID_XtremePrinterInfo)

BEGIN_DISPATCH_MAP(CXTPPrinterInfo, CXTPCmdTarget)
	DISP_FUNCTION_ID(CXTPPrinterInfo, "DeviceName", 1, OleGetDeviceName, VT_BSTR, VTS_NONE)
	DISP_FUNCTION_ID(CXTPPrinterInfo, "DriverName", 2, OleGetDriverName, VT_BSTR, VTS_NONE)
	DISP_FUNCTION_ID(CXTPPrinterInfo, "PortName", 3, OleGetPortName, VT_BSTR, VTS_NONE)
END_DISPATCH_MAP()

BSTR CXTPPrinterInfo::OleGetDeviceName()
{
	CString strName = GetName(xtpDevName_Device);
	return strName.AllocSysString();
}

BSTR CXTPPrinterInfo::OleGetDriverName()
{
	CString strName = GetName(xtpDevName_Driver);
	return strName.AllocSysString();
}

BSTR CXTPPrinterInfo::OleGetPortName()
{
	CString strName = GetName(xtpDevName_Port);
	return strName.AllocSysString();
}

BEGIN_INTERFACE_MAP(CXTPPrintPageHeaderFooter, CXTPCmdTarget)
	INTERFACE_PART(CXTPPrintPageHeaderFooter, XTPDIID_PrintPageHeaderFooter, Dispatch)
END_INTERFACE_MAP()

IMPLEMENT_OLETYPELIB_EX(CXTPPrintPageHeaderFooter, XTPDIID_PrintPageHeaderFooter)

BEGIN_DISPATCH_MAP(CXTPPrintPageHeaderFooter, CXTPCmdTarget)

	DISP_PROPERTY_EX_ID(CXTPPrintPageHeaderFooter, "Font", DISPID_FONT, OleGetFont, OleSetFont,
						VT_FONT)

	DISP_PROPERTY_ID(CXTPPrintPageHeaderFooter, "FormatString", 1, m_strFormatString, VT_BSTR)

	DISP_PROPERTY_ID(CXTPPrintPageHeaderFooter, "TextLeft", 2, m_strLeft, VT_BSTR)
	DISP_PROPERTY_ID(CXTPPrintPageHeaderFooter, "TextCenter", 3, m_strCenter, VT_BSTR)
	DISP_PROPERTY_ID(CXTPPrintPageHeaderFooter, "TextRight", 4, m_strRight, VT_BSTR)

	DISP_FUNCTION_ID(CXTPPrintPageHeaderFooter, "Clear", 50, OleClear, VT_EMPTY, VTS_NONE)

END_DISPATCH_MAP()

LPFONTDISP CXTPPrintPageHeaderFooter::OleGetFont()
{
	return AxCreateOleFont(m_lfFont, this, (LPFNFONTCHANGED)&CXTPPrintPageHeaderFooter::OleSetFont);
}

void CXTPPrintPageHeaderFooter::OleSetFont(LPFONTDISP pFontDisp)
{
	VERIFY(AxGetLogFont(m_lfFont, pFontDisp));
}

void CXTPPrintPageHeaderFooter::OleClear()
{
	Clear();
}

/////////////////////////////////////////////////////////////////////////////
BEGIN_DISPATCH_MAP(CXTPPrintOptions, CXTPCmdTarget)

	DISP_PROPERTY_ID(CXTPPrintOptions, "MarginLeft", 201, m_rcMargins.left, VT_I4)
	DISP_PROPERTY_ID(CXTPPrintOptions, "MarginRight", 202, m_rcMargins.right, VT_I4)
	DISP_PROPERTY_ID(CXTPPrintOptions, "MarginTop", 203, m_rcMargins.top, VT_I4)
	DISP_PROPERTY_ID(CXTPPrintOptions, "MarginBottom", 204, m_rcMargins.bottom, VT_I4)

	DISP_PROPERTY_ID(CXTPPrintOptions, "BlackWhitePrinting", 205, m_bBlackWhitePrinting, VT_BOOL)
	DISP_PROPERTY_ID(CXTPPrintOptions, "BlackWhiteContrast", 206, m_nBlackWhiteContrast, VT_I4)

	DISP_PROPERTY_EX_ID(CXTPPrintOptions, "Landscape", 207, OleGetLandscape, OleSetLandscape,
						VT_BOOL)

	DISP_FUNCTION_ID(CXTPPrintOptions, "MarginsMeasureMetrics", 250, OleGetMarginsMeasureMetrics,
					 VT_BOOL, VTS_NONE)

	DISP_FUNCTION_ID(CXTPPrintOptions, "Header", 251, OleGetHeader, VT_DISPATCH, VTS_NONE)
	DISP_FUNCTION_ID(CXTPPrintOptions, "Footer", 252, OleGetFooter, VT_DISPATCH, VTS_NONE)

	DISP_FUNCTION_ID(CXTPPrintOptions, "PrinterInfo", 253, OleGetPrinterInfo, VT_DISPATCH, VTS_NONE)
END_DISPATCH_MAP()

LPDISPATCH CXTPPrintOptions::OleGetHeader()
{
	return m_pPageHeader ? m_pPageHeader->GetIDispatch(TRUE) : NULL;
}

LPDISPATCH CXTPPrintOptions::OleGetFooter()
{
	return m_pPageFooter ? m_pPageFooter->GetIDispatch(TRUE) : NULL;
}

LPDISPATCH CXTPPrintOptions::OleGetPrinterInfo()
{
	CXTPPrinterInfo* pPrnIf = new CXTPPrinterInfo();
	if (!pPrnIf)
	{
		AfxThrowOleException(E_OUTOFMEMORY);
	}
	return pPrnIf->GetIDispatch(FALSE);
}

BOOL CXTPPrintOptions::OleGetMarginsMeasureMetrics()
{
	return !IsMarginsMeasureInches();
}

BOOL CXTPPrintOptions::OleGetLandscape()
{
	HGLOBAL hDevMode = NULL, hDevNames = NULL;

	if (!XTPGetPrinterDeviceDefaults(hDevMode, hDevNames) || !hDevMode)
	{
		ASSERT(FALSE);
		return FALSE;
	}

	LPDEVMODE pDevMode = (LPDEVMODE)::GlobalLock(hDevMode);
	ASSERT(pDevMode);
	if (!pDevMode)
		return FALSE;

	int nOrientation = pDevMode->dmOrientation;

	::GlobalUnlock(hDevMode);
	pDevMode = NULL;

	ASSERT(nOrientation == DMORIENT_LANDSCAPE || nOrientation == DMORIENT_PORTRAIT);

	return (nOrientation == DMORIENT_LANDSCAPE);
}

void CXTPPrintOptions::OleSetLandscape(BOOL bLandscape)
{
	HGLOBAL hDevMode = NULL, hDevNames = NULL;

	if (!XTPGetPrinterDeviceDefaults(hDevMode, hDevNames) || !hDevMode)
	{
		ASSERT(FALSE);
		return;
	}

	LPDEVMODE pDevMode = (LPDEVMODE)::GlobalLock(hDevMode);
	ASSERT(pDevMode);
	if (!pDevMode)
		return;

	ASSERT(pDevMode->dmOrientation == DMORIENT_LANDSCAPE
		   || pDevMode->dmOrientation == DMORIENT_PORTRAIT);

	pDevMode->dmOrientation = bLandscape ? DMORIENT_LANDSCAPE : DMORIENT_PORTRAIT;

	::GlobalUnlock(hDevMode);
	pDevMode = NULL;
}

#endif
