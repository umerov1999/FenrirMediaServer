// XTPSkinObjectToolBar.cpp: implementation of the CXTPSkinObjectToolBar class.
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
#include "Common/XTPCasting.h"

#include "Common/XTPFramework.h"
#include "Common/XTPSystemHelpers.h"
#include "Common/XTPSynchro.h"
#include "Common/XTPApplication.h"
#include "Common/XTPSingleton.h"
#include "Common/XTPGdiObjects.h"
#include "Common/XTPDrawHelpers.h"
#include "Common/XTPImageManager.h"
#include "Common/ScrollBar/XTPScrollBase.h"
#include "Common/XTPWinThemeWrapper.h"
#include "Common/XTPSynchro.h"

#include "SkinFramework/XTPSkinObject.h"
#include "SkinFramework/XTPSkinObjectFrame.h"
#include "SkinFramework/XTPSkinObjectToolBar.h"
#include "SkinFramework/XTPSkinManager.h"
#include "SkinFramework/XTPSkinDrawTools.h"

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

#ifdef _DEBUG
#	undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#	define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CXTPSkinObjectToolBar, CXTPSkinObjectFrame)

CXTPSkinObjectToolBar::CXTPSkinObjectToolBar()
{
	m_strClassName = _T("TOOLBAR");

	m_nSpecialToolbar = -1;
}

CXTPSkinObjectToolBar::~CXTPSkinObjectToolBar()
{
}

#include "Common/Base/Diagnostic/XTPBeginAfxMap.h"
BEGIN_MESSAGE_MAP(CXTPSkinObjectToolBar, CXTPSkinObjectFrame)
	//{{AFX_MSG_MAP(CXTPSkinObjectToolBar)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
#include "Common/Base/Diagnostic/XTPEndAfxMap.h"

/////////////////////////////////////////////////////////////////////////////
// CXTPSkinObjectToolBar message handlers

LRESULT CXTPSkinObjectToolBar::CustomDrawNotify(DWORD dwDrawStage, NMCUSTOMDRAW* pnmcd)
{
	ASSERT(NULL != pnmcd);

	pnmcd->dwDrawStage = dwDrawStage;

	pnmcd->hdr.code		= NM_CUSTOMDRAW;
	pnmcd->hdr.hwndFrom = GetSafeHwnd();
	pnmcd->hdr.idFrom	= XTPToUIntPtr(GetDlgCtrlID());

	return ::SendMessage(::GetParent(m_hWnd), WM_NOTIFY, (WPARAM)GetDlgCtrlID(), (LPARAM)pnmcd);
}

BOOL CXTPSkinObjectToolBar::CheckDrawingEnabled()
{
	BOOL bEnabled = CXTPSkinObjectFrame::CheckDrawingEnabled();

	CString strClassPath = GetWndClassTreePath(*this, TRUE);

	// Case 1: FileOpen dialog in Vista/Win7/Win8 when DWM is enabled and frame not skinned
	if (bEnabled && CXTPWinDwmWrapper().IsCompositionEnabled()
		&& !XTPSkinManager()->HasApplyOptions(xtpSkinApplyFrame))
	{
		bEnabled = (NULL == _tcsstr(strClassPath, _T("/#32770/WorkerW/ReBarWindow32")));
	}

	return bEnabled;
}

BOOL CXTPSkinObjectToolBar::OnEraseBkgnd(CDC* pDC)
{
	ASSERT_VALID(pDC);

	if (GetStyle() & (TBSTYLE_TRANSPARENT | TBSTYLE_CUSTOMERASE) || !IsDrawingEnabled())
	{
		return CXTPSkinObjectFrame::OnEraseBkgnd(pDC);
	}

	HBRUSH hbr	  = (HBRUSH)(DWORD_PTR)GetClassLongPtr(m_hWnd, GCLP_HBRBACKGROUND);
	BOOL bChanged = FALSE;

	if ((ULONG_PTR)hbr > 0 && (ULONG_PTR)hbr < (ULONG_PTR)XTP_SKINMETRICS_COLORTABLESIZE)
	{
		HBRUSH hbrTheme = GetMetrics()->m_brTheme[(ULONG_PTR)hbr - 1];
		SetClassLongPtr(m_hWnd, GCLP_HBRBACKGROUND, (LONG_PTR)hbrTheme);
		bChanged = TRUE;
	}

	BOOL bResult = (BOOL)::DefWindowProc(m_hWnd, WM_ERASEBKGND, (WPARAM)pDC->GetSafeHdc(), 0);

	if (bChanged)
	{
		SetClassLongPtr(m_hWnd, GCLP_HBRBACKGROUND, (LONG_PTR)hbr);
	}
	return bResult;
}

void CXTPSkinObjectToolBar::UpdateToolTipsPos()
{
	if ((GetStyle() & TBSTYLE_TOOLTIPS) == 0)
		return;

	HWND hwndToolTips = (HWND)SendMessage(TB_GETTOOLTIPS);
	if (!hwndToolTips)
		return;

	int nCount = GetToolbarButtonCount();

	TOOLINFO ti;
	ti.cbSize	= sizeof(ti);
	ti.hwnd		= m_hWnd;
	ti.lpszText = LPSTR_TEXTCALLBACK;

	for (int nIndex = 0; nIndex < nCount; nIndex++)
	{
		TBBUTTON tbb;
		if (!GetToolbarButton(nIndex, &tbb))
			continue;

		if (tbb.fsState & TBSTATE_HIDDEN)
			continue;

		if (tbb.fsStyle & TBSTYLE_SEP)
			continue;

		ti.uId = XTPToUIntPtr(tbb.idCommand);

		if (!GetToolbarItemRect(nIndex, &ti.rect))
		{
			ti.rect.left = ti.rect.right = ti.rect.top = ti.rect.bottom = 0;
		}

		::SendMessage(hwndToolTips, TTM_NEWTOOLRECT, 0, (LPARAM)((LPTOOLINFO)&ti));
	}
}

BOOL CXTPSkinObjectToolBar::IsSpecialSystemToolbar()
{
	if (m_nSpecialToolbar == -1)
	{
		m_nSpecialToolbar = 0;

		HWND hWndParent = ::GetParent(m_hWnd);

		TCHAR lpszClassName[XTP_MAX_CLASSNAME + 1];
		if (0 != ::GetClassName(hWndParent, lpszClassName, _countof(lpszClassName)))
		{
			if (_tcsicmp(lpszClassName, _T("Breadcrumb Parent")) == 0
				|| _tcsicmp(lpszClassName, _T("TravelBand")) == 0
				|| _tcsicmp(lpszClassName, _T("msctls_progress32")) == 0
				|| _tcsicmp(lpszClassName, _T("Search Control")) == 0)
			{
				m_nSpecialToolbar = 1;
			}
		}
	}

	return m_nSpecialToolbar;
}

void CXTPSkinObjectToolBar::OnPaint()
{
	if (!IsDrawingEnabled())
	{
		CXTPSkinObjectFrame::OnPaint();
		return;
	}

	CXTPSkinObjectPaintDC dc(this); // device context for painting

	UpdateToolTipsPos();

	CRect rcClient;
	GetClientRect(&rcClient);

	CXTPBufferDC dcMem(dc, rcClient);

	if (IsSpecialSystemToolbar())
	{
		DefWindowProc(WM_PAINT, (WPARAM)dcMem.GetSafeHdc(), 0);
		return;
	}

	dcMem.FillSolidRect(rcClient, GetColor(COLOR_3DFACE));

	HFONT hFont	   = (HFONT)SendMessage(WM_GETFONT);
	HFONT hOldFont = (HFONT)::SelectObject(dcMem, hFont);

	NMTBCUSTOMDRAW tbcd;
	ZeroMemory(&tbcd, sizeof(NMTBCUSTOMDRAW));
	tbcd.nmcd.hdc = dcMem.GetSafeHdc();
	tbcd.nmcd.rc  = rcClient;

	LRESULT dwCustom = CustomDrawNotify(CDDS_PREPAINT, &tbcd.nmcd);

	if (!(dwCustom & CDRF_SKIPDEFAULT))
	{
		if (IsHorizontal())
			DrawToolbarH(&dcMem);
		else
			DrawToolbarV(&dcMem);
	}

	if (dwCustom & CDRF_NOTIFYPOSTPAINT)
	{
		tbcd.nmcd.hdc		  = dcMem.GetSafeHdc();
		tbcd.nmcd.uItemState  = 0;
		tbcd.nmcd.lItemlParam = 0;
		CustomDrawNotify(CDDS_POSTPAINT, &tbcd.nmcd);
	}

	::SelectObject(dcMem, hOldFont);
}

#define HIML_NORMAL 0
#define HIML_HOT 1
#define HIML_DISABLED 2
#define I_IMAGENONE (-2)

HIMAGELIST CXTPSkinObjectToolBar::GetImageList(int iMode, int iIndex)
{
	return (HIMAGELIST)SendMessage(UINT(iMode == HIML_HOT		 ? TB_GETHOTIMAGELIST
										: iMode == HIML_DISABLED ? TB_GETDISABLEDIMAGELIST
																 : TB_GETIMAGELIST),
								   XTPToWPARAM(iIndex), 0);
}

#ifndef BTNS_WHOLEDROPDOWN
#	define BTNS_WHOLEDROPDOWN 0x80
#endif

BOOL CXTPSkinObjectToolBar::HasSplitDropDown(LPTBBUTTON ptbb)
{
	ASSERT(NULL != ptbb);

	DWORD dwExStyle = (DWORD)SendMessage(TB_GETEXTENDEDSTYLE);

	BOOL fRet = ((dwExStyle & TBSTYLE_EX_DRAWDDARROWS) && (ptbb->fsStyle & TBSTYLE_DROPDOWN)
				 && !(ptbb->fsStyle & BTNS_WHOLEDROPDOWN));

	return fRet;
}

BOOL CXTPSkinObjectToolBar::HasDropDownArrow(LPTBBUTTON ptbb)
{
	ASSERT(NULL != ptbb);

	DWORD dwExStyle = (DWORD)SendMessage(TB_GETEXTENDEDSTYLE);

	BOOL fRet = (((dwExStyle & TBSTYLE_EX_DRAWDDARROWS) && (ptbb->fsStyle & TBSTYLE_DROPDOWN))
				 || (ptbb->fsStyle & BTNS_WHOLEDROPDOWN));

	return fRet;
}

BOOL CXTPSkinObjectToolBar::HasButtonImage(LPTBBUTTON ptbb)
{
	ASSERT(NULL != ptbb);

	int iImage = ptbb->iBitmap;

	if ((GetStyle() & TBSTYLE_LIST) && (iImage == I_IMAGENONE) && ptbb->fsStyle & TBSTYLE_AUTOSIZE)
	{
		return FALSE;
	}
	return TRUE;
}

BOOL CXTPSkinObjectToolBar::IsHorizontal()
{
	HWND hWndParent = ::GetParent(m_hWnd);
	DWORD dwStyle	= (DWORD)::GetWindowLong(hWndParent, GWL_STYLE);
	return (dwStyle & CBRS_ORIENT_VERT) == 0;
}

#define PSDPxax 0x00B8074A

struct XTP_IMAGELISTDRAWPARAMS
{
	DWORD cbSize;
	HIMAGELIST himl;
	int i;
	HDC hdcDst;
	int x;
	int y;
	int cx;
	int cy;
	int xBitmap; // x offest from the upperleft of bitmap
	int yBitmap; // y offset from the upperleft of bitmap
	COLORREF rgbBk;
	COLORREF rgbFg;
	UINT fStyle;
	DWORD dwRop;
};

struct XTP_IMAGELISTDRAWPARAMSEX : public XTP_IMAGELISTDRAWPARAMS
{
	DWORD fState;
	DWORD Frame;
	COLORREF crEffect;
};

#define ILS_SATURATE 0x00000004

BOOL CXTPSkinObjectToolBar::IsAlphaImageList(HIMAGELIST himl)
{
	if (!GetSkinManager()->IsComCtlV6())
		return FALSE;

	BOOL bAlpha = FALSE;
	if (m_mapAlphaImageList.Lookup(himl, bAlpha))
		return bAlpha;

	m_mapAlphaImageList[himl] = FALSE;

	IMAGEINFO ii;
	if (!ImageList_GetImageInfo(himl, 0, &ii))
		return FALSE;

	BITMAP bmp;
	GetObject(ii.hbmImage, sizeof(bmp), &bmp);

	if (bmp.bmBitsPixel != 32)
		return FALSE;

	COLORREF clr = ImageList_GetBkColor(himl);

	if (clr != 0xFFFFFFFF)
		return FALSE;

	LPBYTE pBitsI = NULL;
	bAlpha		  = -1;

	HBITMAP hBitmap = CXTPImageManagerIcon::PreMultiplyAlphaBitmap(ii.hbmImage, &bAlpha, &pBitsI);

	BOOL bResult = (bAlpha == TRUE) && pBitsI;

	if (hBitmap)
		DeleteObject(hBitmap);

	m_mapAlphaImageList[himl] = bResult;
	return bResult;
}

void CXTPSkinObjectToolBar::DrawButtonImage(CDC* pDC, int x, int y, int nIndex)
{
	ASSERT_VALID(pDC);

	TBBUTTON tbb;
	GetToolbarButton(nIndex, &tbb);

	int state	   = tbb.fsState;
	int nHot	   = GetToolbarHotItem();
	BOOL fHotTrack = nHot == nIndex;
	int iIndex	   = 0;
	int iImage	   = tbb.iBitmap;

	HIMAGELIST himl	 = 0;
	BOOL bMonoBitmap = FALSE;

	if (fHotTrack || (state & TBSTATE_CHECKED))
	{
		himl = GetImageList(HIML_HOT, iIndex);
	}
	else if (!(state & TBSTATE_ENABLED))
	{
		himl		= GetImageList(HIML_DISABLED, iIndex);
		bMonoBitmap = himl == NULL;
	}

	if (!himl)
	{
		himl = GetImageList(HIML_NORMAL, iIndex);
	}

	if (himl && (iImage != -1))
	{
		if (bMonoBitmap && IsAlphaImageList(himl))
		{
			XTP_IMAGELISTDRAWPARAMSEX imldp;

			imldp.himl	  = himl;
			imldp.cbSize  = sizeof(imldp);
			imldp.i		  = iImage;
			imldp.hdcDst  = pDC->GetSafeHdc();
			imldp.x		  = x;
			imldp.y		  = y;
			imldp.cx	  = 0;
			imldp.cy	  = 0;
			imldp.xBitmap = 0;
			imldp.yBitmap = 0;
			imldp.rgbBk	  = GetColor(COLOR_3DFACE);
			imldp.rgbFg	  = CLR_DEFAULT;
			imldp.fStyle  = ILD_TRANSPARENT;

			imldp.fState   = ILS_SATURATE;
			imldp.Frame	   = 0;
			imldp.crEffect = 0;

			ImageList_DrawIndirect((IMAGELISTDRAWPARAMS*)&imldp);
		}
		else if (bMonoBitmap)
		{
			int iDxBitmap, iDyBitmap;
			ImageList_GetIconSize(himl, &iDxBitmap, &iDyBitmap);

			CDC dcMono;
			dcMono.CreateCompatibleDC(pDC);

			CBitmap bmp;
			bmp.Attach(CreateBitmap(iDxBitmap + 1, iDyBitmap + 1, 1, 1, 0));

			CBitmap* pOldBitmap = dcMono.SelectObject(&bmp);
			dcMono.SetTextColor(0L);

			XTP_IMAGELISTDRAWPARAMS imldp;

			PatBlt(dcMono, 0, 0, iDxBitmap + 1, iDyBitmap + 1, WHITENESS);

			imldp.cbSize  = sizeof(imldp);
			imldp.himl	  = himl;
			imldp.i		  = iImage;
			imldp.hdcDst  = dcMono.GetSafeHdc();
			imldp.x		  = 0;
			imldp.y		  = 0;
			imldp.cx	  = 0;
			imldp.cy	  = 0;
			imldp.xBitmap = 0;
			imldp.yBitmap = 0;
			imldp.rgbBk	  = GetColor(COLOR_BTNFACE);
			imldp.rgbFg	  = CLR_DEFAULT;
			imldp.fStyle  = ILD_ROP | ILD_MASK;
			imldp.dwRop	  = SRCCOPY;

			ImageList_DrawIndirect((IMAGELISTDRAWPARAMS*)&imldp);

			imldp.fStyle = ILD_ROP | ILD_IMAGE;
			imldp.rgbBk	 = GetColor(COLOR_3DHILIGHT);
			imldp.dwRop	 = SRCPAINT;

			ImageList_DrawIndirect((IMAGELISTDRAWPARAMS*)&imldp);

			pDC->SetTextColor(0L);
			pDC->SetBkColor(0x00FFFFFF);

			HBRUSH hbrOld = (HBRUSH)SelectObject(pDC->GetSafeHdc(),
												 GetMetrics()->m_brTheme[COLOR_3DHILIGHT]);
			BitBlt(pDC->GetSafeHdc(), x + XTP_DPI_X(1), y + XTP_DPI_Y(1), iDxBitmap, iDyBitmap,
				   dcMono, 0, 0, PSDPxax);
			SelectObject(pDC->GetSafeHdc(), hbrOld);

			hbrOld = (HBRUSH)SelectObject(pDC->GetSafeHdc(),
										  GetMetrics()->m_brTheme[COLOR_BTNSHADOW]);
			BitBlt(pDC->GetSafeHdc(), x, y, iDxBitmap, iDyBitmap, dcMono, 0, 0, PSDPxax);
			SelectObject(pDC->GetSafeHdc(), hbrOld);

			dcMono.SelectObject(pOldBitmap);
		}
		else
		{
			XTP_IMAGELISTDRAWPARAMS imldp;

			imldp.himl	  = himl;
			imldp.cbSize  = sizeof(imldp);
			imldp.i		  = iImage;
			imldp.hdcDst  = pDC->GetSafeHdc();
			imldp.x		  = x;
			imldp.y		  = y;
			imldp.cx	  = 0;
			imldp.cy	  = 0;
			imldp.xBitmap = 0;
			imldp.yBitmap = 0;
			imldp.rgbBk	  = GetColor(COLOR_3DFACE);
			imldp.rgbFg	  = CLR_DEFAULT;
			imldp.fStyle  = ILD_TRANSPARENT;

			ImageList_DrawIndirect((IMAGELISTDRAWPARAMS*)&imldp);
		}
	}
}

void CXTPSkinObjectToolBar::DrawButton(CDC* pDC, int nIndex)
{
	ASSERT_VALID(pDC);

	TBBUTTON tbb;
	GetToolbarButton(nIndex, &tbb);

	CRect rc;
	if (!GetToolbarItemRect(nIndex, rc))
		return;

	int dxText = rc.Width() - (2 * GetMetrics()->m_cxEdge);
	int dyText = rc.Height() - (2 * GetMetrics()->m_cyEdge);

	NMTBCUSTOMDRAW tbcd;
	ZeroMemory(&tbcd, sizeof(NMTBCUSTOMDRAW));
	tbcd.nmcd.hdc		  = pDC->GetSafeHdc();
	tbcd.nmcd.rc		  = rc;
	tbcd.nmcd.dwItemSpec  = XTPToULongPtr(tbb.idCommand);
	tbcd.nmcd.lItemlParam = XTPToLPARAM(tbb.dwData);

	SetRect(&tbcd.rcText, 0, 0, dxText, dyText);

	int nHot	   = GetToolbarHotItem();
	BOOL fHotTrack = nHot == nIndex;
	BOOL bPressed  = tbb.fsState & TBSTATE_PRESSED;
	BOOL bChecked  = tbb.fsState & TBSTATE_CHECKED;
	BOOL bEnabled  = tbb.fsState & TBSTATE_ENABLED;

	tbcd.nmcd.uItemState = UINT((fHotTrack ? CDIS_HOT : 0) | (bPressed ? CDIS_SELECTED : 0)
								| (bChecked ? CDIS_CHECKED : 0) | (bEnabled ? 0 : CDIS_DISABLED));

	LRESULT dwCustom = CustomDrawNotify(CDDS_ITEMPREPAINT, &tbcd.nmcd);

	fHotTrack = (0 != (tbcd.nmcd.uItemState & CDIS_HOT));
	bPressed  = (0 != (tbcd.nmcd.uItemState & CDIS_SELECTED));
	bChecked  = (0 != (tbcd.nmcd.uItemState & CDIS_CHECKED));
	bEnabled  = (0 == (tbcd.nmcd.uItemState & CDIS_DISABLED));

	if (!(dwCustom & CDRF_SKIPDEFAULT))
	{
		int dxFace = rc.Width() - 2 * GetMetrics()->m_cxEdge;

		dxText = tbcd.rcText.right - tbcd.rcText.left;
		dyText = tbcd.rcText.bottom - tbcd.rcText.top;

		int x = rc.left + GetMetrics()->m_cxEdge;
		int y = rc.top + GetMetrics()->m_cyEdge;

		BOOL bSplit		= HasSplitDropDown(&tbb);
		int cxMenuCheck = bSplit ? GetSystemMetrics(SM_CYMENUCHECK) : 0;

		if (HasDropDownArrow(&tbb))
		{
			dxFace -= 5;
			dxText -= bSplit ? cxMenuCheck : 5;
		}

		LRESULT lPad = SendMessage(TB_GETPADDING);
		int yPad	 = HIWORD(lPad);
		int xPad	 = LOWORD(lPad);

		int yOffset = (yPad - (2 * GetMetrics()->m_cyEdge)) / 2;
		if (yOffset < 0)
			yOffset = 0;

		HIMAGELIST himl = GetImageList(HIML_NORMAL, 0);

		int iDxBitmap = XTP_DPI_X(16), iDyBitmap = XTP_DPI_Y(16);

		if (himl)
		{
			ImageList_GetIconSize(himl, &iDxBitmap, &iDyBitmap);
		}

		int xCenterOffset = (dxFace - iDxBitmap) / 2;

		if (GetStyle() & TBSTYLE_LIST)
		{
			xCenterOffset = xPad / 2;
		}
		else if (bSplit)
		{
			xCenterOffset = (dxFace + GetMetrics()->m_cxEdge * 2 - (iDxBitmap + cxMenuCheck)) / 2;
		}

		if ((bPressed || bChecked) && (!(dwCustom & TBCDRF_NOOFFSET)))
		{
			xCenterOffset++;
			yOffset++;
		}

		CXTPSkinManagerClass* pClass = GetSkinClass();
		if (NULL != pClass)
		{
			if (!(dwCustom & TBCDRF_NOEDGES))
			{
				if (bSplit)
				{
					CRect rcSplit(rc), rcSplitDropDown(rc);
					rcSplit.right -= cxMenuCheck;
					rcSplitDropDown.left = rcSplit.right;

					pClass->DrawThemeBackground(pDC, TP_SPLITBUTTON,
												!bEnabled	? TS_DISABLED
												: bPressed	? TS_PRESSED
												: bChecked	? TS_CHECKED
												: fHotTrack ? TS_HOT
															: TS_NORMAL,
												&rcSplit);
					pClass->DrawThemeBackground(pDC, TP_SPLITBUTTONDROPDOWN,
												!bEnabled	? TS_DISABLED
												: bPressed	? TS_PRESSED
												: bChecked	? TS_CHECKED
												: fHotTrack ? TS_HOT
															: TS_NORMAL,
												&rcSplitDropDown);
				}
				else
				{
					pClass->DrawThemeBackground(pDC, TP_BUTTON,
												!bEnabled	? TS_DISABLED
												: bPressed	? TS_PRESSED
												: bChecked	? TS_CHECKED
												: fHotTrack ? TS_HOT
															: TS_NORMAL,
												&rc);
				}
			}
		}

		BOOL fImage = HasButtonImage(&tbb);

		if (fImage)
		{
			int yImage = y, xImage = x;

			if (GetStyle() & TBSTYLE_LIST)
			{
				if (iDyBitmap + yPad >= rc.Height())
					yImage -= GetMetrics()->m_cyEdge;

				if (iDxBitmap + xPad >= rc.Width())
					xImage -= GetMetrics()->m_cxEdge;
			}

			DrawButtonImage(pDC, xImage + xCenterOffset, yImage + yOffset, nIndex);
		}

		CString strText;

		SendMessage(TB_GETBUTTONTEXT, XTPToWPARAM(tbb.idCommand),
					(LPARAM)(LPTSTR)strText.GetBuffer(256));
		strText.ReleaseBuffer();

#ifdef _UNICODE
		if (strText.IsEmpty() && (int)SendMessage(WM_NOTIFYFORMAT, 0, NF_QUERY) == NFR_ANSI)
		{
			char tText[256];
			tText[0] = 0;
			SendMessage(TB_GETBUTTONTEXTA, XTPToWPARAM(tbb.idCommand), (LPARAM)tText);
			strText = tText;
		}
#endif

		if (!strText.IsEmpty())
		{
			if ((bPressed || bChecked) && (!(dwCustom & TBCDRF_NOOFFSET)))
			{
				x++;
				if (GetStyle() & TBSTYLE_LIST)
					y++;
			}

			if (GetStyle() & TBSTYLE_LIST)
			{
				int iListGap = (GetMetrics()->m_cxEdge * 2);

				if (fImage)
				{
					x += iDxBitmap + iListGap;
					dxText -= iDxBitmap + iListGap;
				}
			}
			else
			{
				y += yOffset + iDyBitmap;
				dyText -= yOffset + iDyBitmap;
			}

			DWORD uiStyle = DT_END_ELLIPSIS;

			int nTextRows = (int)SendMessage(TB_GETTEXTROWS);

			if (nTextRows > 1)
				uiStyle |= DT_WORDBREAK | DT_EDITCONTROL;
			else
				uiStyle |= DT_SINGLELINE;

			if (GetStyle() & TBSTYLE_LIST)
			{
				uiStyle |= DT_LEFT | DT_VCENTER | DT_SINGLELINE;
			}
			else
			{
				uiStyle |= DT_CENTER;
			}

			pDC->SetBkMode(TRANSPARENT);
			pDC->SetTextColor(bEnabled ? GetColor(COLOR_BTNTEXT) : GetColor(COLOR_BTNSHADOW));

			if (dxText > 0)
			{
				CRect rcText(x + XTP_DPI_X(1), y + XTP_DPI_Y(1), x + XTP_DPI_X(1) + dxText,
							 y + XTP_DPI_Y(1) + dyText);
				XTPSkinFrameworkDrawText(*pDC, strText, rcText, uiStyle);
			}
		}

		if (!bSplit && HasDropDownArrow(&tbb))
		{
			CRect rcGlyph = rc;
			rcGlyph.left  = rcGlyph.right - XTP_DPI_X(14);
			if (bPressed)
				rcGlyph.OffsetRect(XTP_DPI_X(1), XTP_DPI_Y(1));

			XTPPrimitiveDrawer()->DrawSymbol(pDC, xtpPrimitiveSymbolPlayDown, rcGlyph,
											 GetColor(COLOR_BTNTEXT));
		}
	}

	if (dwCustom & CDRF_NOTIFYPOSTPAINT)
		CustomDrawNotify(CDDS_ITEMPOSTPAINT, &tbcd.nmcd);
}

void CXTPSkinObjectToolBar::FillBackOfViewControlClass()
{
	CString strClassPath = GetWndClassTreePath(*this, TRUE);
	if (NULL != _tcsstr(strClassPath, _T("/#32770/ViewControlClass")))
	{
		CWnd* pOwnerWnd = GetSafeOwner();
		if (NULL != pOwnerWnd)
		{
			CDC* pDC = pOwnerWnd->GetDC();
			if (NULL != pDC)
			{
				CRect rc;
				pOwnerWnd->GetClientRect(&rc);
				pDC->FillSolidRect(rc, GetColor(COLOR_3DFACE));
			}
		}
	}
}

void CXTPSkinObjectToolBar::DrawToolbarH(CDC* pDC)
{
	ASSERT_VALID(pDC);

	FillBackOfViewControlClass();

	int nCount = GetToolbarButtonCount();
	for (int nIndex = 0; nIndex < nCount; nIndex++)
	{
		TBBUTTON tbb;
		if (!GetToolbarButton(nIndex, &tbb))
			continue;

		if (tbb.fsState & TBSTATE_HIDDEN)
			continue;

		if (tbb.fsStyle & TBSTYLE_SEP)
		{
			CXTPSkinManagerClass* pClass = GetSkinClass();
			if (NULL != pClass)
			{
				CRect rc;
				GetToolbarItemRect(nIndex, &rc);
				pClass->DrawThemeBackground(pDC, TP_SEPARATOR, 0, &rc);
			}
		}
		else
		{
			DrawButton(pDC, nIndex);
		}
	}
}

void CXTPSkinObjectToolBar::DrawToolbarV(CDC* pDC)
{
	ASSERT_VALID(pDC);

	FillBackOfViewControlClass();

	int nCount = GetToolbarButtonCount();
	for (int nIndex = 0; nIndex < nCount; nIndex++)
	{
		TBBUTTON tbb;
		if (!GetToolbarButton(nIndex, &tbb))
			continue;

		if (tbb.fsState & TBSTATE_HIDDEN)
			continue;

		if (tbb.fsStyle & TBSTYLE_SEP)
		{
			CXTPSkinManagerClass* pClass = GetSkinClass();
			if (NULL != pClass)
			{
				CRect rc2;
				GetToolbarItemRect(nIndex, &rc2);
				CRect rc(rc2.left, rc2.bottom - rc2.Width(), rc2.left + rc2.Height(), rc2.bottom);
				pClass->DrawThemeBackground(pDC, TP_SEPARATORVERT, 0, &rc);
			}
		}
		else
		{
			DrawButton(pDC, nIndex);
		}
	}
}

int CXTPSkinObjectToolBar::GetToolbarButtonCount() const
{
	return static_cast<int>(::SendMessage(m_hWnd, TB_BUTTONCOUNT, 0, 0));
}

BOOL CXTPSkinObjectToolBar::GetToolbarButton(int nIndex, LPTBBUTTON lpButton) const
{
	return static_cast<BOOL>(::SendMessage(m_hWnd, TB_GETBUTTON, XTPToWPARAM(nIndex),
										   reinterpret_cast<LPARAM>(lpButton)));
}

BOOL CXTPSkinObjectToolBar::GetToolbarItemRect(int nIndex, LPRECT lpRect) const
{
	return static_cast<BOOL>(::SendMessage(m_hWnd, TB_GETITEMRECT, XTPToWPARAM(nIndex),
										   reinterpret_cast<LPARAM>(lpRect)));
}

int CXTPSkinObjectToolBar::GetToolbarHotItem() const
{
	return static_cast<int>(::SendMessage(m_hWnd, TB_GETHOTITEM, 0, 0));
}
