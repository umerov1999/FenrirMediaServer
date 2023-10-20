// XTPSkinObjectButton.cpp: implementation of the CXTPSkinObjectButton class.
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
#include "SkinFramework/XTPSkinObjectFrame.h"
#include "SkinFramework/XTPSkinObjectButton.h"
#include "SkinFramework/XTPSkinDrawTools.h"
#include "SkinFramework/XTPSkinManager.h"

#define CBR_CLIENTRECT 0
#define CBR_CHECKBOX 1
#define CBR_RADIOBOX 2
#define CBR_CHECKTEXT 3
#define CBR_RADIOTEXT 4
#define CBR_GROUPTEXT 5
#define CBR_GROUPFRAME 6
#define CBR_PUSHBUTTON 7

#ifndef BS_SPLITBUTTON
#	define BS_SPLITBUTTON 0x0000000CL
#	define BS_DEFSPLITBUTTON 0x0000000DL
#endif

#define BS_HORZMASK (BS_LEFT | BS_RIGHT | BS_CENTER)
#define BS_VERTMASK (BS_TOP | BS_BOTTOM | BS_VCENTER)

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

#ifdef _DEBUG
#	undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#	define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CXTPSkinObjectButton, CXTPSkinObjectFrame)

CXTPSkinObjectButton::CXTPSkinObjectButton()
{
	m_bHot	= FALSE;
	m_bPushed = FALSE;

	m_bStyle = (BYTE)0xFF;

	m_strClassName	 = _T("BUTTON");
	m_nCtlColorMessage = WM_CTLCOLORBTN;
}

CXTPSkinObjectButton::~CXTPSkinObjectButton()
{
}

#include "Common/Base/Diagnostic/XTPBeginAfxMap.h"
BEGIN_MESSAGE_MAP(CXTPSkinObjectButton, CXTPSkinObjectFrame)
	//{{AFX_MSG_MAP(CXTPSkinObjectButton)
	ON_WM_ERASEBKGND()
	ON_WM_MOUSEMOVE()
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_PAINT()
	ON_MESSAGE(WM_PRINTCLIENT, OnPrintClient)
	//}}AFX_MSG_MAP
	ON_MESSAGE_VOID(WM_MOUSELEAVE, OnMouseLeave)
	ON_MESSAGE_VOID(BM_SETSTYLE, OnDefaultAndInvalidate)
	ON_MESSAGE_VOID(BM_SETCHECK, OnDefaultAndInvalidate)
	ON_MESSAGE_VOID(WM_CAPTURECHANGED, OnDefaultAndInvalidate)
	ON_MESSAGE_VOID(WM_ENABLE, OnInvalidate)
	ON_MESSAGE(WM_UPDATEUISTATE, OnUpdateUIState)
	ON_MESSAGE(BM_SETSTATE, OnSetState)
	ON_MESSAGE(WM_SETTEXT, OnSetText)
END_MESSAGE_MAP()
#include "Common/Base/Diagnostic/XTPEndAfxMap.h"

/////////////////////////////////////////////////////////////////////////////
// CXTPSkinObjectButton message handlers

int CXTPSkinObjectButton::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CXTPSkinObjectFrame::OnCreate(lpCreateStruct) == -1)
		return -1;

	Invalidate(FALSE);

	return 0;
}

void CXTPSkinObjectButton::OnSkinChanged(BOOL bPrevState, BOOL bNewState)
{
	CXTPSkinObjectFrame::OnSkinChanged(bPrevState, bNewState);

	m_bHot	= FALSE;
	m_bPushed = FALSE;

	if (!bPrevState)
	{
		m_bPushed = FALSE;
	}
}

BOOL CXTPSkinObjectButton::OnEraseBkgnd(CDC* pDC)
{
	BOOL bResult = TRUE;
	if (!IsDrawingEnabled())
	{
		bResult = CXTPSkinObjectFrame::OnEraseBkgnd(pDC);
	}
	return bResult;
}

void CXTPSkinObjectButton::OnMouseLeave()
{
	OnMouseMove(0, CPoint(-1, -1));
}

void CXTPSkinObjectButton::OnMouseMove(UINT nFlags, CPoint point)
{
	if (GetButtonStyle() == BS_GROUPBOX)
		return;

	CXTPSkinObjectFrame::OnMouseMove(nFlags, point);

	CRect rc;
	GetClientRect(&rc);

	BOOL bHot = rc.PtInRect(point);
	if (bHot != m_bHot)
	{
		m_bHot = bHot;
		Invalidate(FALSE);

		if (m_bHot)
		{
			TRACKMOUSEEVENT tme = { sizeof(TRACKMOUSEEVENT), TME_LEAVE, m_hWnd, HOVER_DEFAULT };
			_TrackMouseEvent(&tme);
		}
	}
}

BOOL CXTPSkinObjectButton::IsDefWindowProcAvail(int nMessage) const
{
	if (m_bWindowsForms && (nMessage < WM_CTLCOLORMSGBOX || nMessage > WM_CTLCOLORSTATIC))
		return FALSE;

	if (nMessage == WM_TIMER)
		return FALSE;

	if (m_bActiveX)
		return TRUE;

	return CXTPSkinObjectFrame::IsDefWindowProcAvail(nMessage);
}

LRESULT CXTPSkinObjectButton::OnSetState(WPARAM wParam, LPARAM /*lParam*/)
{
	BOOL bVisible = (0 != (GetStyle() & WS_VISIBLE));
	if (bVisible)
		ModifyStyle(WS_VISIBLE, 0);
	Default();
	if (bVisible)
		ModifyStyle(0, WS_VISIBLE);

	m_bPushed = (wParam != 0);

	Invalidate(FALSE);

	return 0;
}

void CXTPSkinObjectButton::OnSetFocus(CWnd* pOldWnd)
{
	CXTPSkinObjectFrame::OnSetFocus(pOldWnd);
	Invalidate(FALSE);
}

void CXTPSkinObjectButton::OnKillFocus(CWnd* pNewWnd)
{
	CXTPSkinObjectFrame::OnKillFocus(pNewWnd);
	Invalidate(FALSE);
}

void CXTPSkinObjectButton::OnDefaultAndInvalidate()
{
	Default();
	Invalidate(FALSE);
}

void CXTPSkinObjectButton::OnInvalidate()
{
	Invalidate(FALSE);
}

LRESULT CXTPSkinObjectButton::OnPrintClient(WPARAM wParam, LPARAM lParam)
{
	if ((lParam & PRF_CLIENT) == 0 || !IsDrawingEnabled())
		return Default();

	CDC* pDC = CDC::FromHandle((HDC)wParam);
	if (pDC)
		OnDraw(pDC);
	return 1;
}

void CXTPSkinObjectButton::OnPaint()
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

void CXTPSkinObjectButton::OnDraw(CDC* pDC)
{
	CRect rc;
	GetClientRect(&rc);

	if ((GetButtonStyle() == BS_GROUPBOX) && !m_bActiveX && !m_bWindowsForms)
	{
		CFont* pOldFont = pDC->SelectObject(GetFont());
		pDC->SetBkColor(GetColor(COLOR_3DFACE));

		OnDraw(pDC, rc);

		pDC->SelectObject(pOldFont);
	}
	else
	{
		CXTPBufferDC memDC(*pDC, rc);

		if (FillBackground(&memDC, rc, WM_CTLCOLORBTN) == GetStockObject(HOLLOW_BRUSH))
		{
			memDC.Discard();

			CFont* pOldFont = pDC->SelectObject(GetFont());
			pDC->SetBkColor(GetColor(COLOR_3DFACE));

			OnDraw(pDC, rc);

			pDC->SelectObject(pOldFont);
		}
		else
		{
			CFont* pOldFont = memDC.SelectObject(GetFont());

			OnDraw(&memDC, rc);

			memDC.SelectObject(pOldFont);
		}
	}
}

BOOL CXTPSkinObjectButton::IsPushButton()
{
	DWORD dwStyle = GetStyle();

	switch (GetButtonStyle())
	{
		case LOBYTE(BS_PUSHBUTTON):
		case LOBYTE(BS_DEFPUSHBUTTON):
		case LOBYTE(BS_OWNERDRAW):
		case LOBYTE(BS_SPLITBUTTON):
		case LOBYTE(BS_DEFSPLITBUTTON): return TRUE;

		default:
			if ((dwStyle & BS_PUSHLIKE) == BS_PUSHLIKE)
				return TRUE;
			break;
	}

	return FALSE;
}

DWORD CXTPSkinObjectButton::GetAlignment()
{
	DWORD bHorz;
	DWORD bVert;

	DWORD dwStyle = GetStyle();

	bHorz = dwStyle & (BS_LEFT | BS_RIGHT | BS_CENTER);
	bVert = dwStyle & (BS_TOP | BS_BOTTOM | BS_VCENTER);

	if (!bHorz || !bVert)
	{
		if (IsPushButton())
		{
			if (!bHorz)
				bHorz = BS_CENTER;
		}
		else
		{
			if (!bHorz)
				bHorz = BS_LEFT;
		}

		if (!bVert)
			bVert = BS_VCENTER;
	}

	if (m_bActiveX)
		bVert = BS_VCENTER;

	return bHorz | bVert;
}

CSize CXTPSkinObjectButton::GetGlyphSize(BOOL bCheckBox)
{
	CXTPSkinManagerClass* pClassButton = GetSkinClass();
	CRect rcButton(0, 0, XTP_DPI_X(13), XTP_DPI_Y(13));
	CSize sz(13, 13);

	pClassButton->GetThemePartSize(bCheckBox ? BP_CHECKBOX : BP_RADIOBUTTON, 1, rcButton, TS_DRAW,
								   &sz);

	return XTP_DPI(sz);
}

void CXTPSkinObjectButton::CalcRect(CDC* pDC, LPRECT lprc, int code)
{
	CString strText;
	GetWindowText(strText);
	int dy;
	UINT align;
	CSize extent;

	GetClientRect(lprc);

	align = GetAlignment();

	switch (code)
	{
		case CBR_PUSHBUTTON:
			// Subtract out raised edge all around
			InflateRect(lprc, -GetMetrics()->m_cxEdge, -GetMetrics()->m_cyEdge);
			break;

		case CBR_CHECKBOX:
		case CBR_RADIOBOX:
		{
			CSize szGlyph = GetGlyphSize(code == CBR_CHECKBOX);

			switch (align & BS_VERTMASK)
			{
				case BS_VCENTER: lprc->top = (lprc->top + lprc->bottom - szGlyph.cy) / 2; break;

				case BS_TOP:
				case BS_BOTTOM:
				{
					extent = pDC->GetTextExtent(_T(" "), 1);
					dy	 = extent.cy + extent.cy / 4;

					// Save vertical extent
					extent.cx = dy;

					// Get centered amount

					dy = (dy - szGlyph.cy) / 2;
					if ((align & BS_VERTMASK) == BS_TOP)
						lprc->top += dy;
					else
						lprc->top = lprc->bottom - extent.cx + dy;
					break;
				}
			}
			lprc->bottom = lprc->top + szGlyph.cy;

			if (GetStyle() & BS_RIGHTBUTTON)
				lprc->left = lprc->right - szGlyph.cx;
			else
				lprc->right = lprc->left + szGlyph.cx;
		}
		break;

		case CBR_CHECKTEXT:
		case CBR_RADIOTEXT:
		{
			int nOffset = GetGlyphSize(code == CBR_CHECKBOX).cx + XTP_DPI_X(3);
			if (GetStyle() & BS_RIGHTBUTTON)
			{
				lprc->right -= nOffset;
			}
			else
			{
				lprc->left += nOffset;
			}
		}
		break;

		case CBR_GROUPTEXT:
			if (strText.IsEmpty())
			{
				lprc->left = lprc->right = 0;
				lprc->bottom			 = lprc->top + pDC->GetTextExtent(_T("x"), 1).cy;
				break;
			}

			extent = pDC->GetTextExtent(strText);
			extent.cx += GetMetrics()->m_cxEdge * 2;

			switch (align & BS_HORZMASK)
			{
				case BS_RIGHT:
					lprc->right -= XTP_DPI_X(6) + GetMetrics()->m_cxBorder;
					lprc->left += lprc->right - (int)(extent.cx);
					break;

				case BS_CENTER:
					lprc->left += GetMetrics()->m_cxBorder
								  + (lprc->left + lprc->right - extent.cx) / 2;
					lprc->right = lprc->left + (int)(extent.cx);
					break;

				default:
					lprc->left += XTP_DPI_X(8) + GetMetrics()->m_cxBorder;
					lprc->right = lprc->left + (int)(extent.cx);
			}

			lprc->bottom = lprc->top + extent.cy;
			break;

		case CBR_GROUPFRAME:
			extent = pDC->GetTextExtent(_T(" "), 1);
			lprc->top += extent.cy / 2;
			break;
	}
}

#define BitmapWidth(width, bitsPerPixel) (((width * bitsPerPixel + 31) & ~31) >> 3)

static HBITMAP CreateDIB(HDC hdc, int aWidth, int aHeight, int aPixelBit, unsigned char*& BBits)
{
	HBITMAP hBitmap = NULL;

	BITMAPINFO      pBitmapInfo = { 0 };

	pBitmapInfo.bmiHeader.biSize = sizeof(pBitmapInfo.bmiHeader);
	pBitmapInfo.bmiHeader.biBitCount = (WORD)aPixelBit;
	pBitmapInfo.bmiHeader.biCompression = BI_RGB;
	pBitmapInfo.bmiHeader.biWidth = aWidth;
	pBitmapInfo.bmiHeader.biHeight = -aHeight;
	pBitmapInfo.bmiHeader.biPlanes = 1;
	pBitmapInfo.bmiHeader.biSizeImage = BitmapWidth(pBitmapInfo.bmiHeader.biWidth, pBitmapInfo.bmiHeader.biBitCount) * pBitmapInfo.bmiHeader.biHeight;
	hBitmap = CreateDIBSection(hdc, &pBitmapInfo, DIB_RGB_COLORS, (void**)&BBits, 0, 0);

	return hBitmap;
}

static HBITMAP GrayBitmap(HBITMAP hSRCBitmap)
{
	if (!hSRCBitmap) {
		return hSRCBitmap;
	}

	BITMAP bmpInfo;
	if (!GetObjectW(hSRCBitmap, sizeof(BITMAP), &bmpInfo)) {
		return hSRCBitmap;
	}

	HDC srcDC, destDC;
	srcDC = CreateCompatibleDC(NULL);
	destDC = CreateCompatibleDC(NULL);
	if (!srcDC || !destDC) {
		if (srcDC) {
			DeleteDC(srcDC);
		}
		if (destDC) {
			DeleteDC(destDC);
		}
		return hSRCBitmap;
	}
	SetStretchBltMode(destDC, COLORONCOLOR);
	SelectObject(srcDC, hSRCBitmap);
	unsigned char* BBits = NULL;
	HBITMAP dst = CreateDIB(srcDC, bmpInfo.bmWidth, bmpInfo.bmHeight, bmpInfo.bmBitsPixel, BBits);
	if (!dst) {
		DeleteDC(srcDC);
		DeleteDC(destDC);
		return hSRCBitmap;
	}
	SelectObject(destDC, dst);
	if (!BitBlt(destDC, 0, 0, bmpInfo.bmWidth, bmpInfo.bmHeight, srcDC, 0, 0, SRCCOPY)) {
		DeleteDC(srcDC);
		DeleteDC(destDC);
		DeleteObject(dst);
		return hSRCBitmap;
	}
	DeleteDC(srcDC);
	DeleteDC(destDC);
	for (int i = 0; i < bmpInfo.bmHeight; i++) {
		for (int s = 0; s < bmpInfo.bmWidth; s++) {
			int pos = i * BitmapWidth(bmpInfo.bmWidth, bmpInfo.bmBitsPixel) + s * bmpInfo.bmBitsPixel / 8;
			if (bmpInfo.bmBitsPixel == 24 && BBits[pos] == 255 && BBits[pos + 1] == 0 && BBits[pos + 2] == 255) {
				continue;
			}
			int result = (BBits[pos] + BBits[pos + 1] + BBits[pos + 2]) / 3;
			BBits[pos] = (unsigned char)result;
			BBits[pos + 1] = (unsigned char)result;
			BBits[pos + 2] = (unsigned char)result;
		}
	}
	return dst;
}

void CXTPSkinObjectButton::DrawButtonText(CDC* pDC, int nPart, int nState)
{
	ASSERT_VALID(pDC);

	static const BYTE buttonsStyles[] = {
		CBR_PUSHBUTTON, CBR_PUSHBUTTON, CBR_CHECKTEXT,  CBR_CHECKTEXT,
		CBR_RADIOTEXT,  CBR_CHECKTEXT,  CBR_CHECKTEXT,  CBR_GROUPTEXT,
		CBR_CLIENTRECT, CBR_RADIOTEXT,  CBR_CLIENTRECT, CBR_PUSHBUTTON,
	};

	CXTPSkinManagerClass* pClass = GetSkinClass();

	RECT rc;
	int x, y, cx = 0, cy = 0;

	CString strText;
	UINT dsFlags = DT_SINGLELINE | DT_LEFT | DT_TOP | DT_NOCLIP;
	BYTE bStyle  = GetButtonStyle();

	BOOL pbfPush = IsPushButton();
	if (pbfPush)
	{
		CalcRect(pDC, &rc, CBR_PUSHBUTTON);
	}
	else
	{
		CalcRect(pDC, &rc, buttonsStyles[bStyle]);
	}

	if (bStyle == BS_SPLITBUTTON || bStyle == BS_DEFSPLITBUTTON)
	{
		rc.right -= XTP_DPI_X(12);
	}

	// Alignment
	GetWindowText(strText);
	UINT wAlignment = GetAlignment();

	HGDIOBJ hImage = 0;

	bool is32BitBitmap = false;
	bool is24BitBitmap = false;

	if (GetStyle() & BS_BITMAP)
	{
		hImage = (HGDIOBJ)SendMessage(BM_GETIMAGE, IMAGE_BITMAP);
		if (!hImage)
			return;

		BITMAP bmp;
		GetObject(hImage, sizeof(BITMAP), &bmp);
		cx = bmp.bmWidth;
		cy = bmp.bmHeight;
		is32BitBitmap = bmp.bmBitsPixel == 32;
		is24BitBitmap = bmp.bmBitsPixel == 24;
	}
	else if (GetStyle() & BS_ICON)
	{
		hImage = (HGDIOBJ)SendMessage(BM_GETIMAGE, IMAGE_ICON);
		if (!hImage)
			return;

		XTPSkinFrameworkGetIconSize((HICON)hImage, &cx, &cy);
	}
	else
	{
		// Text button
		if (strText.IsEmpty())
			return;

		if (GetStyle() & BS_MULTILINE)
		{
			dsFlags |= DT_WORDBREAK | DT_EDITCONTROL;
			dsFlags &= ~DT_SINGLELINE;
		}

		CRect rcText(0, 0, rc.right - rc.left, 0);
		XTPSkinFrameworkDrawText(*pDC, strText, rcText, dsFlags | DT_CALCRECT);

		cx = rcText.Width();
		cy = rcText.Height();
	}

	CRect rcText(rc);
	if (pbfPush && ((GetStyle() & BS_MULTILINE) == 0))
	{
		rcText.DeflateRect(GetMetrics()->m_cxBorder, GetMetrics()->m_cyBorder);
	}

	// Horizontal
	switch (wAlignment & BS_HORZMASK)
	{
		case BS_LEFT: x = rc.left + (pbfPush ? GetMetrics()->m_cxBorder : 0); break;

		case BS_RIGHT:
			x = rc.right - cx - (pbfPush ? GetMetrics()->m_cxBorder : 0);
			dsFlags |= DT_RIGHT;
			break;

		default:
			x = (rc.left + rc.right - cx) / 2;
			dsFlags |= DT_CENTER;
			break;
	}

	// Vertical
	switch (wAlignment & BS_VERTMASK)
	{
		case BS_TOP: y = rc.top + (pbfPush ? GetMetrics()->m_cyBorder : 0); break;

		case BS_BOTTOM:
			y = rc.bottom - cy - (pbfPush ? GetMetrics()->m_cyBorder : 0);
			dsFlags |= DT_BOTTOM;
			break;

		default:
			y = (rc.top + rc.bottom - cy) / 2;
			dsFlags |= DT_VCENTER;
			break;
	}

	if (GetStyle() & BS_BITMAP && (nState != PBS_DISABLED || !is32BitBitmap && !is24BitBitmap))
	{
		if (is32BitBitmap) {
			BLENDFUNCTION bf = { 0 };
			bf.BlendOp = AC_SRC_OVER;
			bf.BlendFlags = 0;
			bf.SourceConstantAlpha = 0xff;
			bf.AlphaFormat = AC_SRC_ALPHA;

			CDC dcMem;
			dcMem.CreateCompatibleDC(pDC);
			dcMem.SelectObject(hImage);
			pDC->AlphaBlend(x, y, cx, cy, &dcMem, 0, 0, cx, cy, bf);
			DeleteDC(dcMem);
		}
		else {
			CDC dcMem;
			dcMem.CreateCompatibleDC(pDC);
			dcMem.SelectObject(hImage);
			pDC->BitBlt(x, y, cx, cy, &dcMem, 0, 0, SRCCOPY);
			DeleteDC(dcMem);
		}
	}
	else if (GetStyle() & BS_BITMAP && nState == PBS_DISABLED)
	{
		HBITMAP gray = GrayBitmap((HBITMAP)hImage);
		if (is32BitBitmap) {
			BLENDFUNCTION bf = { 0 };
			bf.BlendOp = AC_SRC_OVER;
			bf.BlendFlags = 0;
			bf.SourceConstantAlpha = 0xff;
			bf.AlphaFormat = AC_SRC_ALPHA;

			CDC dcMem;
			dcMem.CreateCompatibleDC(pDC);
			dcMem.SelectObject(gray);
			pDC->AlphaBlend(x, y, cx, cy, &dcMem, 0, 0, cx, cy, bf);
			DeleteDC(dcMem);
		}
		else {
			CDC dcMem;
			dcMem.CreateCompatibleDC(pDC);
			dcMem.SelectObject(gray);
			pDC->BitBlt(x, y, cx, cy, &dcMem, 0, 0, SRCCOPY);
			DeleteDC(dcMem);
		}
		if (gray != (HBITMAP)hImage) {
			DeleteObject(gray);
		}
	}
	else if (GetStyle() & BS_ICON)
	{
		pDC->DrawState(CPoint(x, y), CSize(cx, cy), (HICON)hImage, DSS_NORMAL, (HBRUSH)0);
	}
	else if (bStyle != LOBYTE(BS_USERBUTTON))
	{
		if (SendMessage(WM_QUERYUISTATE) & UISF_HIDEACCEL)
		{
			dsFlags |= DT_HIDEPREFIX;
		}

		if (GetExStyle() & WS_EX_RTLREADING)
			dsFlags |= DT_RTLREADING;

		pDC->SetTextColor(GetColor(COLOR_BTNTEXT));

		if (GetStyle() & BS_MULTILINE)
		{
			pClass->DrawThemeText(pDC, nPart, nState, strText, dsFlags | DT_NOCLIP,
								  CRect(x, y, x + cx, y + cy));
		}
		else
		{
			pClass->DrawThemeText(pDC, nPart, nState, strText, dsFlags, &rcText);
		}
	}

	if (::GetFocus() == m_hWnd)
	{
		if ((SendMessage(WM_QUERYUISTATE) & UISF_HIDEFOCUS) == 0)
		{
			if (!pbfPush)
			{
				RECT rcClient;
				GetClientRect(&rcClient);

				if (bStyle == LOBYTE(BS_USERBUTTON))
					CopyRect(&rc, &rcClient);
				else
				{
					// Try to leave a border all around text.  That causes
					// focus to hug text.
					rc.top	  = CXTP_max(rcClient.top, y - GetMetrics()->m_cyBorder);
					rc.bottom = CXTP_min(rcClient.bottom, rc.top + GetMetrics()->m_cyEdge + cy);

					rc.left	 = CXTP_max(rcClient.left, x - GetMetrics()->m_cxBorder);
					rc.right = CXTP_min(rcClient.right, rc.left + GetMetrics()->m_cxEdge + cx);
				}
			}
			else
				InflateRect(&rc, -GetMetrics()->m_cxBorder, -GetMetrics()->m_cyBorder);

			pDC->SetTextColor(0);
			pDC->SetBkColor(0xFFFFFF);
			// Are back & fore colors set properly?
			::DrawFocusRect(pDC->GetSafeHdc(), &rc);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CXTPSkinObjectButton message handlers

BYTE CXTPSkinObjectButton::GetButtonStyle()
{
	if (m_bStyle != 0xFF)
		return m_bStyle;

	BYTE bStyle = BYTE(GetStyle() & 0xF);
	return bStyle;
}

void CXTPSkinObjectButton::OnDraw(CDC* pDC, CRect rc)
{
	ASSERT_VALID(pDC);

	BOOL bPressed = m_bPushed;
	BOOL bHot	 = m_bHot || (::GetCapture() == m_hWnd);
	BYTE bStyle   = GetButtonStyle();
	BOOL bDefault = bStyle == BS_DEFPUSHBUTTON || bStyle == BS_DEFSPLITBUTTON;
	BOOL bEnabled = IsWindowEnabled();
	BOOL bChecked = (BOOL)SendMessage(BM_GETCHECK);

	CXTPSkinManagerClass* pClassButton = GetSkinClass();
	int nPart						   = 0;
	int nState						   = 0;

	if (IsPushButton())
	{
		nState = !bEnabled ? PBS_DISABLED
						   : bPressed ? PBS_PRESSED
									  : bHot ? PBS_HOT
											 : bChecked ? PBS_PRESSED
														: bDefault ? PBS_DEFAULTED : PBS_NORMAL;
		nPart = BP_PUSHBUTTON;

		pClassButton->DrawThemeBackground(pDC, nPart, nState, rc);

		if (bStyle == BS_SPLITBUTTON || bStyle == BS_DEFSPLITBUTTON)
		{
			CRect rcItem(rc);
			rcItem.DeflateRect(XTP_DPI_X(4), XTP_DPI_Y(4));
			rcItem.left = rcItem.right - XTP_DPI_X(12);

			COLORREF clr = GetColor(!bEnabled ? COLOR_GRAYTEXT : COLOR_BTNTEXT);
			XTPPrimitiveDrawer()->DrawSymbol(pDC, xtpPrimitiveSymbolPlayDown, rcItem, clr);

			rcItem.left -= XTP_DPI_X(4);
			pDC->FillSolidRect(rcItem.left, rcItem.top, XTP_DPI_X(1), rcItem.Height(),
							   GetColor(!bEnabled ? COLOR_GRAYTEXT : COLOR_BTNSHADOW));
			pDC->FillSolidRect(rcItem.left + XTP_DPI_X(1), rcItem.top, XTP_DPI_X(1),
							   rcItem.Height(), GetColor(COLOR_BTNHIGHLIGHT));
		}
	}
	else if (bStyle == BS_RADIOBUTTON || bStyle == BS_AUTORADIOBUTTON)
	{
		nState = !bEnabled ? RBS_UNCHECKEDDISABLED
						   : bPressed ? RBS_UNCHECKEDPRESSED
									  : bHot ? RBS_UNCHECKEDHOT : RBS_UNCHECKEDNORMAL;
		if (bChecked)
			nState += 4;
		nPart = BP_RADIOBUTTON;

		CRect rcRadio;
		CalcRect(pDC, &rcRadio, CBR_RADIOBOX);
		pClassButton->DrawThemeBackground(pDC, nPart, nState, rcRadio);
	}
	else if (bStyle == BS_CHECKBOX || bStyle == BS_AUTOCHECKBOX || bStyle == BS_AUTO3STATE
			 || bStyle == BS_3STATE)
	{
		nState = !bEnabled ? RBS_UNCHECKEDDISABLED
						   : bPressed ? RBS_UNCHECKEDPRESSED
									  : bHot ? RBS_UNCHECKEDHOT : RBS_UNCHECKEDNORMAL;
		if (bChecked)
			nState += 4;
		if ((bChecked == BST_INDETERMINATE) && (bStyle == BS_AUTO3STATE || bStyle == BS_3STATE))
			nState += 4;

		nPart = BP_CHECKBOX;

		CRect rcRadio;
		CalcRect(pDC, &rcRadio, CBR_CHECKBOX);

		pClassButton->DrawThemeBackground(pDC, nPart, nState, rcRadio);

		pDC->SetTextColor(pClassButton->GetThemeColor(BP_CHECKBOX, nState, TMT_TEXTCOLOR,
													  GetColor(COLOR_BTNTEXT)));
	}
	else if (bStyle == BS_GROUPBOX)
	{
		nState = !bEnabled ? GBS_DISABLED : GBS_NORMAL;
		nPart  = BP_GROUPBOX;

		CRect rcGroupBox;
		CalcRect(pDC, &rcGroupBox, CBR_GROUPFRAME);

		pClassButton->DrawThemeBackground(pDC, nPart, nState, rcGroupBox);

		CRect rcGrouText;
		CalcRect(pDC, &rcGrouText, CBR_GROUPTEXT);

		rcGrouText.left -= GetMetrics()->m_cxEdge;

		FillBackground(pDC, rcGrouText, WM_CTLCOLORBTN);
	}
	else
	{
	}

	DrawButtonText(pDC, nPart, nState);
}

LRESULT CXTPSkinObjectButton::OnSetText(WPARAM wParam, LPARAM lParam)
{
	BOOL bVisible = (0 != (GetStyle() & WS_VISIBLE));
	if (bVisible)
		ModifyStyle(WS_VISIBLE, 0);

	LRESULT lResult = CXTPSkinObjectFrame::OnSetText(wParam, lParam);

	if (bVisible)
		ModifyStyle(0, WS_VISIBLE);

	if (GetButtonStyle() == BS_GROUPBOX) // Special clean for Group Box.
	{
		CXTPClientRect rc(this);
		CClientDC dc(this);

		CRect rcGrouText;
		CalcRect(&dc, &rcGrouText, CBR_GROUPTEXT);

		rcGrouText.left  = 0;
		rcGrouText.right = rc.right;

		FillBackground(&dc, rcGrouText, WM_CTLCOLORBTN);
	}

	Invalidate(FALSE);

	return lResult;
}

BOOL CXTPSkinObjectButton::OnHookMessage(UINT nMessage, WPARAM& wParam, LPARAM& lParam,
										 LRESULT& lResult)
{
	int bStyle = GetButtonStyle();
	if (bStyle >= BS_OWNERDRAW && bStyle != BS_SPLITBUTTON && bStyle != BS_DEFSPLITBUTTON)
	{
		return FALSE;
	}
	return CXTPSkinObjectFrame::OnHookMessage(nMessage, wParam, lParam, lResult);
}

BOOL CXTPSkinObjectButton::OnHookDefWindowProc(UINT nMessage, WPARAM& wParam, LPARAM& lParam,
											   LRESULT& lResult)
{
	int bStyle = GetButtonStyle();
	if (bStyle >= BS_OWNERDRAW && bStyle != BS_SPLITBUTTON && bStyle != BS_DEFSPLITBUTTON)
	{
		return FALSE;
	}

	if (!m_bActiveX && wParam && (nMessage == WM_PAINT || nMessage == WM_PRINTCLIENT))
	{
		OnDraw(CDC::FromHandle((HDC)wParam));
		return TRUE;
	}

	return CXTPSkinObjectFrame::OnHookDefWindowProc(nMessage, wParam, lParam, lResult);
}

LRESULT CXTPSkinObjectButton::OnUpdateUIState(WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult = ::DefWindowProc(m_hWnd, WM_UPDATEUISTATE, wParam, lParam);
	Invalidate(FALSE);

	return lResult;
}
