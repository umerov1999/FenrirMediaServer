// XTPScrollable.cpp : CXTPScrollable implementation file
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
#include "Common/XTPMacros.h"
#include "Common/XTPCasting.h"
#include "Common/XTPFramework.h"
#include "Common/XTPSystemHelpers.h"
#include "Common/XTPSynchro.h"
#include "Common/XTPApplication.h"
#include "Common/XTPSingleton.h"
#include "Common/XTPColorManager.h"
#include "Common/XTPResourceImage.h"
#include "Common/XTPGdiObjects.h"
#include "Common/XTPDrawHelpers.h"

#include "Common/ScrollBar/XTPScrollBase.h"
#include "Common/ScrollBar/XTPScrollBarCtrl.h"
#include "Common/ScrollBar/XTPScrollable.h"
#include "Common/ScrollBar/XTPScrollBarPaintManager.h"

#ifdef _XTP_INCLUDE_CONTROLS
#	include "Controls/Defines.h"
#	include "Controls/Util/XTPControlTheme.h"
#endif

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"
#ifdef _DEBUG
#	define new DEBUG_NEW
#	undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifndef WS_EX_CLIENTEDGE
#	define WS_EX_CLIENTEDGE 0x00000200L
#endif

#ifndef WS_EX_STATICEDGE
#	define WS_EX_STATICEDGE 0x00020000L
#endif

#ifndef WS_EX_DLGMODALFRAME
#	define WS_EX_DLGMODALFRAME 0x00000001L
#endif

#ifndef WS_EX_WINDOWEDGE
#	define WS_EX_WINDOWEDGE 0x00000100L
#endif

#ifndef WS_EX_MDICHILD
#	define WS_EX_MDICHILD 0x00000040L
#endif

#ifndef WS_EX_APPWINDOW
#	define WS_EX_APPWINDOW 0x00040000L
#endif

#ifndef WS_EX_NOACTIVATE
#	define WS_EX_NOACTIVATE 0x08000000L
#endif

#ifndef WS_EX_COMPOSITED
#	define WS_EX_COMPOSITED 0x02000000L
#endif

#ifndef WS_EX_NOREDIRECTIONBITMAP
#	define WS_EX_NOREDIRECTIONBITMAP 0x00200000L
#endif

#ifndef WM_NCXBUTTONDOWN
#	define WM_NCXBUTTONDOWN 0x00AB
#endif

#ifndef WM_NCXBUTTONUP
#	define WM_NCXBUTTONUP 0x00AC
#endif

#ifndef WM_NCXBUTTONDBLCLK
#	define WM_NCXBUTTONDBLCLK 0x00AD
#endif

#ifndef WM_MOUSEWHEEL
#	define WM_MOUSEWHEEL 0x020A
#endif

#ifndef WM_MOUSEHWHEEL
#	define WM_MOUSEHWHEEL 0x020E
#endif

#ifndef WM_CHANGEUISTATE
#	define WM_CHANGEUISTATE 0x0127
#endif

#ifndef WM_INPUT
#	define WM_INPUT 0x00FF
#endif

#ifndef WM_POINTERDEVICEOUTOFRANGE
#	define WM_POINTERDEVICEOUTOFRANGE 0x23A
#endif

#ifndef WM_GETDPISCALEDSIZE
#	define WM_GETDPISCALEDSIZE 0x02E4
#endif

#ifndef WM_POINTERACTIVATE
#	define WM_POINTERACTIVATE 0x024B
#endif

#ifndef WM_GESTURENOTIFY
#	define WM_GESTURENOTIFY 0x011A
#endif

#ifndef WM_GESTURE
#	define WM_GESTURE 0x0119
#endif

#ifndef GET_WHEEL_DELTA_WPARAM
#	define GET_WHEEL_DELTA_WPARAM(wParam) ((short)HIWORD(wParam))
#endif

#define XTP_SCROLLABLE_HOST_PRIVATE_STYLES                                                         \
	(WS_OVERLAPPED | WS_POPUP | WS_MINIMIZE | WS_VISIBLE | WS_MAXIMIZE | WS_CAPTION | WS_BORDER    \
	 | WS_DLGFRAME | WS_SYSMENU | WS_THICKFRAME | WS_GROUP | WS_MINIMIZEBOX | WS_MAXIMIZEBOX)

#define XTP_SCROLLABLE_SHARED_STYLES                                                               \
	(WS_DISABLED | WS_TABSTOP | WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN)

#define XTP_SCROLLABLE_CTRL_PRIVATE_STYLES                                                         \
	~(XTP_SCROLLABLE_HOST_PRIVATE_STYLES | XTP_SCROLLABLE_SHARED_STYLES)

#define XTP_SCROLLABLE_HOST_PRIVATE_EXSTYLES                                                       \
	(WS_EX_DLGMODALFRAME | WS_EX_TOPMOST | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE                     \
	 | WS_EX_DLGMODALFRAME | WS_EX_WINDOWEDGE | WS_EX_MDICHILD | WS_EX_APPWINDOW)

#define XTP_SCROLLABLE_SHARED_EXSTYLES                                                             \
	(WS_EX_CONTROLPARENT | WS_EX_NOPARENTNOTIFY | WS_EX_TRANSPARENT | WS_EX_NOACTIVATE             \
	 | WS_EX_COMPOSITED | WS_EX_NOREDIRECTIONBITMAP)

#define XTP_SCROLLABLE_CTRL_PRIVATE_EXSTYLES                                                       \
	~(XTP_SCROLLABLE_HOST_PRIVATE_EXSTYLES | XTP_SCROLLABLE_SHARED_EXSTYLES)

/////////////////////////////////////////////////////////////////////////////
// CXTPScrollHostGapCtrl

class CXTPScrollHostGapCtrl : public CWnd
{
protected:
	DECLARE_MESSAGE_MAP();
	afx_msg void OnPaint();
};

BEGIN_MESSAGE_MAP(CXTPScrollHostGapCtrl, CWnd)
	ON_WM_PAINT()
END_MESSAGE_MAP()

void CXTPScrollHostGapCtrl::OnPaint()
{
	CPaintDC dc(this);
	CXTPClientRect rcClient(this);
	COLORREF clr = XTPIniColor(_T("Common.ScrollBar"), _T("Base"),
							   XTPColorManager()->GetColor(XPCOLOR_BASE));
	dc.FillSolidRect(&rcClient, clr);
}

/////////////////////////////////////////////////////////////////////////////
// CXTPScrollHost

static const UINT g_WM_XTPScrollHost_Layout = ::RegisterWindowMessage(
	_T("{EDB75CCC-CC32-4537-8CF9-75BF561A6CF7}"));
CFont CXTPScrollHost::m_Font;
CXTPGlobalAtom CXTPScrollHost::m_propAtom(_T("{1B27B7F7-A640-40C3-AD59-2721359018C3}"));

CXTPScrollHost::CXTPScrollHost()
	: m_pScrollable(NULL)
	, m_bLeftBar(FALSE)
	, m_bInitialized(FALSE)
	, m_pControl(NULL)
	, m_pfnCtrlProc(NULL)
	, m_pGapCtrl(NULL)
	, m_bEnableDefaultBorder(TRUE)
	, m_nAccumulatedWheelDelta(0)
	, m_nAccumulatedHWheelDelta(0)
	, m_bInLayout(FALSE)
	, m_dwCurrentCtlStyle(0)
	, m_dwCurrentCtlExStyle(0)
{
	m_bars[0] = NULL;
	m_bars[1] = NULL;

	memset(&m_scrollInfo, 0, sizeof(m_scrollInfo));

	if (NULL == m_Font.m_hObject)
		m_Font.CreateStockObject(DEFAULT_GUI_FONT);
}

CXTPScrollHost::~CXTPScrollHost()
{
	SAFE_DELETE(m_pControl);
	SAFE_DELETE(m_bars[SB_VERT]);
	SAFE_DELETE(m_bars[SB_HORZ]);
}

static BOOL IsForwardableMessage(UINT message)
{
	static const UINT nonForwardableMessages[] = { WM_CREATE,
												   WM_DESTROY,
												   WM_MOVE,
												   WM_SIZE,
												   WM_ACTIVATE,
												   WM_SETFOCUS,
												   WM_KILLFOCUS,
												   WM_PAINT,
												   WM_CLOSE,
												   WM_QUIT,
												   WM_ERASEBKGND,
												   WM_SHOWWINDOW,
												   WM_GETMINMAXINFO,
												   WM_NEXTDLGCTL,
												   WM_WINDOWPOSCHANGING,
												   WM_WINDOWPOSCHANGED,
												   WM_STYLECHANGING,
												   WM_NOTIFY,
												   WM_NCCREATE,
												   WM_NCDESTROY,
												   WM_NCCALCSIZE,
												   WM_NCHITTEST,
												   WM_NCPAINT,
												   WM_NCACTIVATE,
												   WM_GETDLGCODE,
												   WM_SYNCPAINT,
												   WM_NCMOUSEMOVE,
												   WM_NCLBUTTONDOWN,
												   WM_NCLBUTTONUP,
												   WM_NCLBUTTONDBLCLK,
												   WM_NCRBUTTONDOWN,
												   WM_NCRBUTTONUP,
												   WM_NCRBUTTONDBLCLK,
												   WM_NCMBUTTONDOWN,
												   WM_NCMBUTTONUP,
												   WM_NCMBUTTONDBLCLK,
												   WM_NCXBUTTONDOWN,
												   WM_NCXBUTTONUP,
												   WM_NCXBUTTONDBLCLK,
												   WM_SIZING,
												   WM_MOVING,
												   WM_MOUSEHOVER,
												   WM_MOUSELEAVE,
												   WM_MOUSEWHEEL,
												   WM_MOUSEHWHEEL,
												   WM_PRINT,
												   WM_PRINTCLIENT,
												   WM_MOUSEACTIVATE,
												   WM_CHILDACTIVATE,
												   WM_CHANGEUISTATE,
												   WM_POINTERACTIVATE,
												   WM_GESTURENOTIFY,
												   WM_GESTURE };

	for (int i = 0; i < _countof(nonForwardableMessages); ++i)
	{
		if (message == nonForwardableMessages[i])
			return FALSE;
	}

	return TRUE;
}

static BOOL RequiresScrollBarsSynchronization(UINT message)
{
	static const struct
	{
		UINT msgStart;
		UINT msgEnd;
	} ignoreRanges[] = {
		{ WM_NULL, WM_NULL },
		{ WM_DESTROY, WM_DESTROY },
		{ WM_KILLFOCUS, WM_SETREDRAW },
		{ WM_GETTEXT, WM_GETTEXTLENGTH },
		{ WM_CLOSE, WM_QUIT },
		{ WM_SYSCOLORCHANGE, WM_SYSCOLORCHANGE },
		{ WM_WININICHANGE, WM_ACTIVATEAPP },
		{ WM_TIMECHANGE, WM_SETCURSOR },
		{ WM_QUEUESYNC, WM_CHARTOITEM },
		{ WM_GETFONT, WM_COMMNOTIFY },
		{ WM_POWER, WM_CONTEXTMENU },
		{ WM_DISPLAYCHANGE, WM_INPUT },
		{ WM_IME_STARTCOMPOSITION, WM_IME_KEYLAST },
		{ WM_TIMER, WM_TIMER },
		{ WM_INITMENU, WM_MOUSEMOVE },
		{ WM_PARENTNOTIFY, WM_NEXTMENU },
		{ WM_CAPTURECHANGED, WM_CAPTURECHANGED },
		{ WM_POWERBROADCAST, WM_POINTERDEVICEOUTOFRANGE },
		{ WM_IME_SETCONTEXT, WM_GETDPISCALEDSIZE },
		{ WM_COPY, WM_COPY },
		{ WM_RENDERFORMAT, WM_PAINTCLIPBOARD },
		{ WM_ASKCBFORMATNAME, WM_CHANGECBCHAIN },
		{ WM_QUERYNEWPALETTE, WM_HANDHELDLAST },
		{ WM_PENWINFIRST, WM_PENWINLAST },
		{ WM_PENWINFIRST, WM_PENWINLAST },
	};

	for (int i = 0; i < _countof(ignoreRanges); ++i)
	{
		if (ignoreRanges[i].msgStart <= message && message <= ignoreRanges[i].msgEnd)
			return FALSE;
	}

	return TRUE;
}

BOOL CXTPScrollHost::InitializeScrollHost(IXTPScrollable* pScrollable, DWORD dwInitialStyle,
										  DWORD dwInitialExStyle)
{
	ASSERT(!m_bInitialized);

	ASSERT(NULL != pScrollable);
	m_pScrollable = pScrollable;

	CWnd& wnd = m_pScrollable->GetWnd();
	ASSERT(::IsWindow(wnd));

	CWnd* parent = wnd.GetParent();
	ASSERT(NULL != parent);

	BOOL bHasVScroll = m_pScrollable->HasVScroll(dwInitialStyle, dwInitialExStyle);
	BOOL bHasHScroll = m_pScrollable->HasHScroll(dwInitialStyle, dwInitialExStyle);
	int id			 = wnd.GetDlgCtrlID();
	m_bLeftBar		 = m_pScrollable->HasLeftScrollbar(dwInitialStyle, dwInitialExStyle);

	m_pScrollable->DisableScrollbars();

	LPCTSTR lpClass = ::AfxRegisterWndClass(CS_CLASSDC | CS_DBLCLKS, ::LoadCursor(NULL, IDC_ARROW),
											reinterpret_cast<HBRUSH>(COLOR_BTNFACE + 1));
	CRect rc;
	wnd.GetWindowRect(&rc);
	parent->ScreenToClient(&rc);

	m_pControl = m_pScrollable->CreateControl();
	ASSERT_VALID(m_pControl);

	DWORD style = dwInitialStyle
				  & (XTP_SCROLLABLE_HOST_PRIVATE_STYLES | XTP_SCROLLABLE_SHARED_STYLES);
	DWORD exStyle = wnd.GetExStyle()
					& (XTP_SCROLLABLE_HOST_PRIVATE_EXSTYLES | XTP_SCROLLABLE_SHARED_EXSTYLES);
	if (!m_pControl->CreateEx(exStyle, lpClass, NULL, style, rc, parent, XTPToUInt(id)))
		return FALSE;

	if (bHasVScroll && bHasHScroll)
	{
		m_pGapCtrl = new CXTPScrollHostGapCtrl();
		CRect rcGap(0, 0, 0, 0);
		VERIFY(m_pGapCtrl->Create(lpClass, NULL, WS_CHILD, rcGap, m_pControl, 0));
	}

	style = (dwInitialStyle & (XTP_SCROLLABLE_CTRL_PRIVATE_STYLES | XTP_SCROLLABLE_SHARED_STYLES))
			| WS_VISIBLE | WS_CHILD;
	exStyle = wnd.GetExStyle()
			  & (XTP_SCROLLABLE_CTRL_PRIVATE_EXSTYLES | XTP_SCROLLABLE_SHARED_EXSTYLES);

	if (bHasVScroll)
	{
		m_bars[SB_VERT] = CreateBar(SBS_VERT, m_pControl);
		if (NULL == m_bars[SB_VERT])
			return FALSE;
	}

	if (bHasHScroll)
	{
		m_bars[SB_HORZ] = CreateBar(SBS_HORZ, m_pControl);
		if (NULL == m_bars[SB_HORZ])
			return FALSE;
	}

	CString strText;
	wnd.GetWindowText(strText);

	TCHAR szClassName[0x100];
	::GetClassName(wnd, szClassName, _countof(szClassName));

	m_oldControl.Attach(wnd.Detach());
	m_oldControl.ShowWindow(SW_HIDE);
	m_oldControl.PostMessage(WM_DESTROY);

	wnd.Attach(m_pControl->Detach());

	if (!m_pControl->CreateEx(exStyle, szClassName, strText, style, rc, &wnd, 0))
		return FALSE;

	VERIFY(::SetProp(*m_pControl, m_propAtom.GetAtomString(), reinterpret_cast<HANDLE>(this)));
	m_pfnCtrlProc = reinterpret_cast<WNDPROC>(::GetWindowLongPtr(*m_pControl,
#ifdef _WIN64
																 GWLP_WNDPROC
#else
																 GWL_WNDPROC
#endif
																 ));
	::SetWindowLongPtr(*m_pControl,
#ifdef _WIN64
					   GWLP_WNDPROC,
#else
					   GWL_WNDPROC,
#endif

					   reinterpret_cast<LONG_PTR>(CtrlWndProc));

	if (NULL != m_Font.m_hObject)
		m_pControl->SetFont(&m_Font);

	m_bInitialized = TRUE;

	SynchronizeScrollBar(SB_VERT, NULL, TRUE);
	SynchronizeScrollBar(SB_HORZ, NULL, TRUE);

	Layout();
	wnd.PostMessage(g_WM_XTPScrollHost_Layout);

	return TRUE;
}

void CXTPScrollHost::UninitializeScrollHost()
{
	if (!m_bInitialized)
		return;

	if (NULL != m_pControl && ::IsWindow(m_pControl->GetSafeHwnd()))
	{
		m_pControl->DestroyWindow();
		delete m_pControl;
		m_pControl = NULL;
	}

	if (NULL != m_pGapCtrl)
	{
		m_pGapCtrl->DestroyWindow();
		delete m_pGapCtrl;
		m_pGapCtrl = NULL;
	}

	if (NULL != m_bars[SB_VERT] && ::IsWindow(m_bars[SB_VERT]->GetSafeHwnd()))
	{
		m_bars[SB_VERT]->DestroyWindow();
		delete m_bars[SB_VERT];
		m_bars[SB_VERT] = NULL;
	}

	if (NULL != m_bars[SB_HORZ] && ::IsWindow(m_bars[SB_HORZ]->GetSafeHwnd()))
	{
		m_bars[SB_HORZ]->DestroyWindow();
		delete m_bars[SB_HORZ];
		m_bars[SB_HORZ] = NULL;
	}

	m_pfnCtrlProc  = NULL;
	m_pScrollable  = NULL;
	m_bLeftBar	   = FALSE;
	m_bInitialized = FALSE;
}

void CXTPScrollHost::SetScrollBarTheme(XTPScrollBarTheme nTheme)
{
	if (NULL != m_bars[SB_VERT])
	{
		CXTPScrollBarCtrl* pHScroll = DYNAMIC_DOWNCAST(CXTPScrollBarCtrl, m_bars[SB_VERT]);
		if (NULL != pHScroll)
			pHScroll->SetTheme(nTheme);
	}

	if (NULL != m_bars[SB_HORZ])
	{
		CXTPScrollBarCtrl* pVScroll = DYNAMIC_DOWNCAST(CXTPScrollBarCtrl, m_bars[SB_HORZ]);
		if (NULL != pVScroll)
			pVScroll->SetTheme(nTheme);
	}
}

void CXTPScrollHost::EnableDefaultBorderDrawing(BOOL bEnable)
{
	if (bEnable == m_bEnableDefaultBorder)
		return;

	m_bEnableDefaultBorder = bEnable;

	if (NULL == m_pScrollable)
		return;

	HWND hWnd = m_pScrollable->GetWnd();
	if (!::IsWindow(hWnd))
		return;

	::SetWindowPos(hWnd, NULL, 0, 0, 0, 0,
				   SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
}

BOOL CXTPScrollHost::OnHostWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	if (!m_bInitialized)
		return FALSE;

	switch (message)
	{
		case WM_NCCALCSIZE:
		{
			if (wParam && !m_bEnableDefaultBorder)
			{
				*pResult = 0;
				return TRUE;
			}
			break;
		}

		case WM_NOTIFY:
		{
			if (OnNotify(wParam, lParam, pResult))
				return TRUE;
			break;
		}

#ifdef _XTP_INCLUDE_CONTROLS
		case WM_XTP_SETCONTROLTHEME:
		{
			if (OnSetTheme(wParam, lParam, pResult))
				return TRUE;

			break;
		}
#endif

		default:
			if (g_WM_XTPScrollHost_Layout == message)
			{
				Layout();
			}
			break;
	}

	return FALSE;
}

BOOL CXTPScrollHost::OnHostDefWindowProc(UINT message, WPARAM wParam, LPARAM lParam,
										 LRESULT* pResult)
{
	ASSERT(NULL != pResult);

	if (!m_bInitialized)
	{
		if (WM_CREATE == message)
		{
			*pResult = ::DefWindowProc(m_pScrollable->GetWnd(), message, wParam, lParam);
			return TRUE;
		}

		return FALSE;
	}

	switch (message)
	{
		case WM_CLOSE: return TRUE;

		case WM_STYLECHANGING:
		{
			if (OnStyleChanging(wParam, lParam, pResult))
				return TRUE;
			break;
		}

		case WM_WINDOWPOSCHANGED:
		case WM_WINDOWPOSCHANGING:
		{
			Layout();
			break;
		}

		case WM_DESTROY:
		{
			UninitializeScrollHost();
			break;
		}

		case WM_NCDESTROY:
		{
			m_pScrollable->GetWnd().Detach();
			break;
		}

		case WM_VSCROLL:
		case WM_HSCROLL:
		{
			if (OnScroll(message, wParam, lParam, pResult))
				return TRUE;
			break;
		}
	}

	if (::IsWindow(m_pControl->GetSafeHwnd()) && IsForwardableMessage(message))
	{
		*pResult = m_pControl->SendMessage(message, wParam, lParam);
		return TRUE;
	}

	return FALSE;
}

LRESULT CXTPScrollHost::SendControlMessage(UINT message, WPARAM wParam, LPARAM lParam)
{
	ASSERT(!m_bInitialized);

	if (!::IsWindow(m_pControl->GetSafeHwnd()))
		return 0;

	return ::SendMessage(m_pControl->GetSafeHwnd(), message, wParam, lParam);
}

CScrollBar* CXTPScrollHost::CreateBar(DWORD style, CWnd* pParent)
{
	CScrollBar* pBar = m_pScrollable->CreateBar();
	ASSERT(NULL != pBar);

	CRect rc(0, 0, 1, 1);
	if (!pBar->Create(WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | style, rc, pParent, 0))
	{
		delete pBar;
		return NULL;
	}

	return pBar;
}

void CXTPScrollHost::Layout()
{
	if (m_bInLayout)
		return;

	m_bInLayout = TRUE;

	CWnd& wndHost = m_pScrollable->GetWnd();
	CXTPClientRect rc(wndHost);

	if (NULL == m_bars[SB_VERT] && NULL == m_bars[SB_HORZ])
	{
		m_pControl->MoveWindow(&rc);
		m_bInLayout = FALSE;
		return;
	}

	CSize sScroll(::GetSystemMetrics(SM_CXVSCROLL), ::GetSystemMetrics(SM_CYHSCROLL));
	CRect rcCtrl = rc;
	CRect rcVBar;
	CRect rcHBar;
	CRect rcGap;

	BOOL bVertBar = (NULL != m_bars[SB_VERT] && m_bars[SB_VERT]->IsWindowVisible());
	BOOL bHorzBar = (NULL != m_bars[SB_HORZ] && m_bars[SB_HORZ]->IsWindowVisible());

	if (bVertBar)
	{
		BOOL bRtl = (0 != (m_pControl->GetExStyle() & WS_EX_LAYOUTRTL));

		if (bRtl ? !m_bLeftBar : m_bLeftBar)
		{
			rcVBar.SetRect(rc.left, rc.top, rc.left + sScroll.cx, rc.bottom);
			rcCtrl.left = rcVBar.right;
		}
		else
		{
			rcVBar.SetRect(rc.right - sScroll.cx, rc.top, rc.right, rc.bottom);
			rcCtrl.right = rcVBar.left;
		}

		rcGap = rcVBar;
	}

	if (bHorzBar)
	{
		rcHBar		  = rcCtrl;
		rcHBar.top	  = rcHBar.bottom - sScroll.cy;
		rcCtrl.bottom = rcHBar.top;
		rcGap.top	  = rcHBar.top;
		rcVBar.bottom = rcHBar.top;
	}

	CWnd* arUpdate[4] = { m_pControl };
	int nUpdateIdx	  = 1;

	HDWP dwp			= ::BeginDeferWindowPos(0);
	const UINT dwpFlags = SWP_NOZORDER | SWP_SHOWWINDOW | SWP_NOACTIVATE;

	dwp = ::DeferWindowPos(dwp, *m_pControl, NULL, rcCtrl.left, rcCtrl.top, rcCtrl.Width(),
						   rcCtrl.Height(), dwpFlags);

	if (bVertBar)
	{
		dwp = ::DeferWindowPos(dwp, *m_bars[SB_VERT], NULL, rcVBar.left, rcVBar.top, rcVBar.Width(),
							   rcVBar.Height(), dwpFlags);
		arUpdate[nUpdateIdx++] = m_bars[SB_VERT];
	}

	if (bHorzBar)
	{
		dwp = ::DeferWindowPos(dwp, *m_bars[SB_HORZ], NULL, rcHBar.left, rcHBar.top, rcHBar.Width(),
							   rcHBar.Height(), dwpFlags);
		arUpdate[nUpdateIdx++] = m_bars[SB_HORZ];
	}

	if (NULL != m_pGapCtrl)
	{
		if (bHorzBar && bVertBar)
		{
			dwp = ::DeferWindowPos(dwp, *m_pGapCtrl, NULL, rcGap.left, rcGap.top, rcGap.Width(),
								   rcGap.Height(), dwpFlags);
		}
		else
		{
			dwp = ::DeferWindowPos(dwp, *m_pGapCtrl, NULL, 0, 0, 0, 0,
								   SWP_HIDEWINDOW | SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER);
		}

		arUpdate[nUpdateIdx++] = m_pGapCtrl;
	}

	::EndDeferWindowPos(dwp);

	::UpdateWindow(wndHost);

	for (int i = 0; i < nUpdateIdx; ++i)
		arUpdate[i]->UpdateWindow();

	m_bInLayout = FALSE;
}

void CXTPScrollHost::SynchronizeScrollBar(int nBar, PBOOL pbRequiresLayout /*= NULL*/,
										  BOOL bInInit /*= FALSE*/)
{
	ASSERT(SB_VERT == nBar || SB_HORZ == nBar);

	if (NULL != m_bars[nBar])
	{
		ASSERT(NULL != m_pControl);

		SCROLLINFO si = { 0 };
		si.cbSize	  = sizeof(SCROLLINFO);
		si.fMask	  = SIF_ALL;
		m_pControl->GetScrollInfo(nBar, &si);

		SCROLLINFO& cachedInfo	= m_scrollInfo[nBar];
		BOOL bScrollInfoChanged = (cachedInfo.nMin != si.nMin || cachedInfo.nMax != si.nMax
								   || cachedInfo.nPage != si.nPage || cachedInfo.nPos != si.nPos);
		BOOL bNeedShowScroll	= (si.nPage <= static_cast<UINT>(si.nMax - si.nMin)
								   && (bInInit && SB_VERT == nBar ? !(0 == si.nPage && 100 == si.nMax)
																  : 0 < si.nPage));

		if (bScrollInfoChanged || (bNeedShowScroll != m_bars[nBar]->IsWindowVisible()))
		{
			cachedInfo = si;

			m_bars[nBar]->SetScrollInfo(&si);

			if (bNeedShowScroll)
			{
				BOOL bUpdate = TRUE;

				if (!m_bars[nBar]->IsWindowVisible() || CXTPWindowRect(m_bars[nBar]).Width() == 1)
				{
					m_bars[nBar]->ShowWindow(SW_SHOW);

					if (NULL != pbRequiresLayout)
					{
						*pbRequiresLayout = TRUE;
						bUpdate			  = FALSE;
					}
				}

				if (bUpdate)
				{
					m_bars[nBar]->Invalidate();
					m_bars[nBar]->UpdateWindow();
				}
			}
			else
			{
				if (m_bars[nBar]->IsWindowVisible())
				{
					m_bars[nBar]->ShowWindow(SW_HIDE);
					if (NULL != pbRequiresLayout)
					{
						*pbRequiresLayout = TRUE;
					}
				}
			}
		}
	}
}

LRESULT CALLBACK CXTPScrollHost::CtrlWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	CXTPScrollHost* pHost = reinterpret_cast<CXTPScrollHost*>(
		::GetProp(hwnd, m_propAtom.GetAtomString()));
	ASSERT(NULL != pHost);
	return pHost->OnCtrlWndProc(hwnd, message, wParam, lParam);
}

LRESULT CXTPScrollHost::OnCtrlWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	ASSERT(NULL != m_pControl);
	ASSERT(m_pControl->m_hWnd == hwnd);

	LRESULT lResult = 0;
	BOOL bProcessed = FALSE;
	BOOL bSyncBars	= m_bInitialized && !m_bInLayout && RequiresScrollBarsSynchronization(message);

	FilterStyles();

	switch (message)
	{
		case WM_NCHITTEST: return HTCLIENT;

		case WM_NCPAINT: return FALSE;

		case WM_DESTROY:
		{
			::SetWindowLongPtr(hwnd,
#ifdef _WIN64
							   GWLP_WNDPROC,
#else
							   GWL_WNDPROC,
#endif

							   reinterpret_cast<LONG_PTR>(m_pfnCtrlProc));
			break;
		}

		case WM_NCCALCSIZE:
			if (wParam)
				return 0;
			break;

		case WM_MOUSEWHEEL:
			if (m_pScrollable->RequiresMouseWheelOverriding())
			{
				bProcessed = OnCtrlMouseWheel(wParam, lParam, &lResult);
			}
			break;

		case WM_MOUSEHWHEEL:
			if (m_pScrollable->RequiresMouseWheelOverriding())
			{
				bProcessed = OnCtrlMouseHWheel(wParam, lParam, &lResult);
			}
			break;
	}

	if (!bProcessed)
		lResult = m_pfnCtrlProc(hwnd, message, wParam, lParam);

	if (bSyncBars)
	{
		BOOL bRequiresLayout = FALSE;
		SynchronizeScrollBar(SB_HORZ, &bRequiresLayout);
		SynchronizeScrollBar(SB_VERT, &bRequiresLayout);
		if (bRequiresLayout)
		{
			Layout();
		}
	}

	return lResult;
}

BOOL CXTPScrollHost::OnStyleChanging(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	UNREFERENCED_PARAMETER(pResult);

	LPSTYLESTRUCT lpStyle = reinterpret_cast<LPSTYLESTRUCT>(lParam);
	ASSERT(NULL != lpStyle);

	switch (wParam)
	{
		case GWL_STYLE:
		{
			DWORD ctrlStyle = (lpStyle->styleNew
							   & (XTP_SCROLLABLE_CTRL_PRIVATE_STYLES
								  | XTP_SCROLLABLE_SHARED_STYLES))
							  | WS_VISIBLE | WS_CHILD;
			if (0 != ctrlStyle)
			{
				lpStyle->styleNew &= ~XTP_SCROLLABLE_CTRL_PRIVATE_STYLES;
				if (NULL != m_pControl)
				{
					DWORD dwRemove = (lpStyle->styleOld ^ lpStyle->styleNew)
									 & (XTP_SCROLLABLE_CTRL_PRIVATE_STYLES
										| XTP_SCROLLABLE_SHARED_STYLES);
					m_pControl->ModifyStyle(dwRemove, ctrlStyle);
					m_pControl->SetWindowPos(NULL, 0, 0, 0, 0,
											 SWP_NOZORDER | SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE
												 | SWP_FRAMECHANGED);
				}
			}
			break;
		}

		case GWL_EXSTYLE:
		{
			DWORD ctrlStyle = lpStyle->styleNew
							  & (XTP_SCROLLABLE_CTRL_PRIVATE_EXSTYLES
								 | XTP_SCROLLABLE_SHARED_EXSTYLES);
			if (0 != ctrlStyle)
			{
				lpStyle->styleNew &= ~XTP_SCROLLABLE_CTRL_PRIVATE_EXSTYLES;
				if (NULL != m_pControl)
				{
					DWORD dwRemove = (lpStyle->styleOld ^ lpStyle->styleNew)
									 & (XTP_SCROLLABLE_CTRL_PRIVATE_EXSTYLES
										| XTP_SCROLLABLE_SHARED_EXSTYLES);
					m_pControl->ModifyStyleEx(dwRemove, ctrlStyle);
					m_pControl->SetWindowPos(NULL, 0, 0, 0, 0,
											 SWP_NOZORDER | SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE
												 | SWP_FRAMECHANGED);
				}
			}
			break;
		}
	}

	return FALSE;
}

BOOL CXTPScrollHost::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	ASSERT(NULL != pResult);
	*pResult = m_pScrollable->GetWnd().GetParent()->SendMessage(WM_NOTIFY, wParam, lParam);

	return TRUE;
}

BOOL CXTPScrollHost::OnScroll(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	UNREFERENCED_PARAMETER(lParam);

	ASSERT(NULL != pResult);
	ASSERT(WM_VSCROLL == message || WM_HSCROLL == message);

	int nBar = (WM_VSCROLL == message ? SB_VERT : SB_HORZ);

	SCROLLINFO si;
	si.cbSize = sizeof(si);
	si.fMask  = SIF_ALL;
	ASSERT(NULL != m_pControl);
	m_pControl->GetScrollInfo(nBar, &si);

	switch (LOWORD(wParam))
	{
		case SB_TOP: si.nPos = si.nMin; break;
		case SB_BOTTOM: si.nPos = si.nMax; break;
		case SB_LINEUP: si.nPos -= 1; break;
		case SB_LINEDOWN: si.nPos += 1; break;
		case SB_PAGEUP: si.nPos -= si.nPage; break;
		case SB_PAGEDOWN: si.nPos += si.nPage; break;
		case SB_THUMBTRACK: si.nPos = HIWORD(wParam); break;
		case SB_THUMBPOSITION: si.nPos = HIWORD(wParam); break;
	}

	si.fMask = SIF_POS;
	m_pControl->SetScrollInfo(nBar, &si, TRUE);

	ASSERT(NULL != m_bars[nBar]);
	m_bars[nBar]->SetScrollInfo(&si, TRUE);

	*pResult = m_pControl->SendMessage(message, wParam, 0);

	m_pControl->UpdateWindow();
	m_pScrollable->GetWnd().UpdateWindow();

	return TRUE;
}

void CXTPScrollHost::FilterStyles()
{
	ASSERT(NULL != m_pControl);

	DWORD dwStyle = m_pControl->GetStyle();
	if (m_dwCurrentCtlStyle != dwStyle)
	{
		DWORD dwModifiedStyle = m_pScrollable->FilterStyle(dwStyle);
		m_dwCurrentCtlStyle	  = dwModifiedStyle;
		if (dwModifiedStyle != dwStyle)
		{
			m_pControl->ModifyStyle(dwStyle & ~dwModifiedStyle, dwModifiedStyle & ~dwStyle);

			// This call is necessary as ModifyStyle does not guarantee style changing.
			m_dwCurrentCtlStyle = m_pControl->GetStyle();
		}
	}

	DWORD dwExStyle = m_pControl->GetExStyle();
	if (m_dwCurrentCtlExStyle != dwExStyle)
	{
		DWORD dwModifiedExStyle = m_pScrollable->FilterExStyle(dwExStyle);
		m_dwCurrentCtlExStyle	= dwModifiedExStyle;
		if (dwModifiedExStyle != dwExStyle)
		{
			m_pControl->ModifyStyleEx(dwExStyle & ~dwModifiedExStyle,
									  dwModifiedExStyle & ~dwExStyle);

			// This call is necessary as ModifyStyleEx does not guarantee style changing.
			m_dwCurrentCtlExStyle = m_pControl->GetExStyle();
		}
	}
}

#ifdef _XTP_INCLUDE_CONTROLS

BOOL CXTPScrollHost::OnSetTheme(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	UNREFERENCED_PARAMETER(lParam);
	UNREFERENCED_PARAMETER(pResult);

	XTPControlTheme nTheme = static_cast<XTPControlTheme>(wParam);
	switch (nTheme)
	{
		case xtpControlThemeDefault: SetScrollBarTheme(xtpScrollBarThemeWindowsDefault); break;
		case xtpControlThemeFlat: SetScrollBarTheme(xtpScrollBarThemeFlat); break;
		case xtpControlThemeUltraFlat: SetScrollBarTheme(xtpScrollBarThemeUltraFlat); break;
		case xtpControlThemeOffice2000: SetScrollBarTheme(xtpScrollBarThemeOffice2000); break;
		case xtpControlThemeOfficeXP: SetScrollBarTheme(xtpScrollBarThemeOfficeXP); break;
		case xtpControlThemeOffice2003: SetScrollBarTheme(xtpScrollBarThemeOffice2003); break;
		case xtpControlThemeNativeWinXP: SetScrollBarTheme(xtpScrollBarThemeWindowsDefault); break;
		case xtpControlThemeResource: SetScrollBarTheme(xtpScrollBarThemeResource); break;
		case xtpControlThemeVisualStudio2005:
			SetScrollBarTheme(xtpScrollBarThemeVisualStudio2005);
			break;
		case xtpControlThemeVisualStudio2008:
			SetScrollBarTheme(xtpScrollBarThemeVisualStudio2008);
			break;
		case xtpControlThemeVisualStudio2010:
			SetScrollBarTheme(xtpScrollBarThemeVisualStudio2010);
			break;
		case xtpControlThemeCustom: SetScrollBarTheme(xtpScrollBarThemeWindowsDefault); break;
		case xtpControlThemeOffice2013: SetScrollBarTheme(xtpScrollBarThemeOffice2013); break;
		case xtpControlThemeVisualStudio2012:
		case xtpControlThemeVisualStudio2012Light:
			SetScrollBarTheme(xtpScrollBarThemeVisualStudio2012Light);
			break;
		case xtpControlThemeVisualStudio2012Dark:
			SetScrollBarTheme(xtpScrollBarThemeVisualStudio2012Dark);
			break;
		case xtpControlThemeVisualStudio2015:
			SetScrollBarTheme(xtpScrollBarThemeVisualStudio2015);
			break;
		case xtpControlThemeNativeWindows10:
			SetScrollBarTheme(xtpScrollBarThemeNativeWindows10);
			break;
	}

	return FALSE;
}

#endif

BOOL CXTPScrollHost::OnCtrlMouseWheel(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	ASSERT(NULL != pResult);
	UNREFERENCED_PARAMETER(lParam);

	*pResult = 0;

	if (NULL == m_bars[SB_VERT])
		return FALSE;

	SCROLLINFO si = { 0 };
	si.cbSize	  = sizeof(SCROLLINFO);
	si.fMask	  = SIF_ALL;
	m_pControl->GetScrollInfo(SB_VERT, &si);

	int nLines = si.nMax - si.nMin;
	if (0 == nLines)
		return FALSE;

	UINT nScrollLines = 0;
	::SystemParametersInfo(SPI_GETWHEELSCROLLLINES, 0, &nScrollLines, 0);
	if (0 == nScrollLines)
		nScrollLines = 3;

	short nDelta = GET_WHEEL_DELTA_WPARAM(wParam);

	if (0 < (m_nAccumulatedWheelDelta * nDelta))
	{
		m_nAccumulatedWheelDelta += nDelta;
	}
	else
	{
		m_nAccumulatedWheelDelta = nDelta;
	}

	int nDisplacement = ::MulDiv(-m_nAccumulatedWheelDelta, XTPToIntChecked(nScrollLines),
								 WHEEL_DELTA);
	if (0 != nDisplacement)
	{
		si.fMask = SIF_POS;
		si.nPos += nDisplacement;
		if (si.nPos < si.nMin)
			si.nPos = si.nMin;
		if (si.nPos > si.nMax)
			si.nPos = si.nMax;
		VERIFY(m_pControl->SetScrollInfo(SB_VERT, &si));

		m_pControl->SendMessage(WM_VSCROLL, MAKEWPARAM(SB_THUMBPOSITION, si.nPos));

		m_pControl->UpdateWindow();
		m_pScrollable->GetWnd().UpdateWindow();

		m_nAccumulatedWheelDelta = 0;
	}

	return TRUE;
}

BOOL CXTPScrollHost::OnCtrlMouseHWheel(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	ASSERT(NULL != pResult);
	UNREFERENCED_PARAMETER(lParam);

	*pResult = 0;

	if (NULL == m_bars[SB_HORZ])
		return FALSE;

	SCROLLINFO si = { 0 };
	si.cbSize	  = sizeof(SCROLLINFO);
	si.fMask	  = SIF_ALL;
	m_pControl->GetScrollInfo(SB_HORZ, &si);

	int nCols = si.nMax - si.nMin;
	if (0 == nCols)
		return FALSE;

	UINT nScrollCols = 0;
	::SystemParametersInfo(SPI_GETWHEELSCROLLLINES, 0, &nScrollCols, 0);
	if (0 == nScrollCols)
		nScrollCols = 3;

	short nDelta = GET_WHEEL_DELTA_WPARAM(wParam);

	if (0 < (m_nAccumulatedHWheelDelta * nDelta))
	{
		m_nAccumulatedHWheelDelta += nDelta;
	}
	else
	{
		m_nAccumulatedHWheelDelta = nDelta;
	}

	int nDisplacement = ::MulDiv(-m_nAccumulatedHWheelDelta, XTPToIntChecked(nScrollCols),
								 WHEEL_DELTA);
	if (0 != nDisplacement)
	{
		si.fMask = SIF_POS;
		si.nPos += nDisplacement;
		if (si.nPos < si.nMin)
			si.nPos = si.nMin;
		if (si.nPos > si.nMax)
			si.nPos = si.nMax;
		VERIFY(m_pControl->SetScrollInfo(SB_HORZ, &si));

		m_pControl->SendMessage(WM_HSCROLL, MAKEWPARAM(SB_THUMBPOSITION, si.nPos));

		m_pControl->UpdateWindow();
		m_pScrollable->GetWnd().UpdateWindow();

		m_nAccumulatedHWheelDelta = 0;
	}

	return TRUE;
}
