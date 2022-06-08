// XTPSkinObjectComboBox.cpp: implementation of the CXTPSkinObjectComboBox class.
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
#define XTP_INTERNAL_UXTHEME_INCLUSION
#include "Common/Uxtheme.h"
#include "Common/Tmschema.h"
#include "Common/XTPSystemHelpers.h"
#include "Common/XTPSynchro.h"
#include "Common/XTPGdiObjects.h"
#include "Common/XTPApplication.h"
#include "Common/XTPSingleton.h"
#include "Common/XTPDrawHelpers.h"
#include "Common/ScrollBar/XTPScrollBase.h"
#include "Common/XTPSynchro.h"

#include "SkinFramework/XTPSkinObject.h"
#include "SkinFramework/XTPSkinObjectFrame.h"
#include "SkinFramework/XTPSkinObjectComboBox.h"
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

IMPLEMENT_DYNCREATE(CXTPSkinObjectComboBox, CXTPSkinObjectFrame)

CXTPSkinObjectComboBox::CXTPSkinObjectComboBox()
{
	m_bHot	  = FALSE;
	m_bPushed = FALSE;

	m_strClassName	   = _T("COMBOBOX");
	m_nCtlColorMessage = WM_CTLCOLORLISTBOX;
	m_bPaint		   = FALSE;
}

CXTPSkinObjectComboBox::~CXTPSkinObjectComboBox()
{
}

#include "Common/Base/Diagnostic/XTPBeginAfxMap.h"
BEGIN_MESSAGE_MAP(CXTPSkinObjectComboBox, CXTPSkinObjectFrame)
	//{{AFX_MSG_MAP(CXTPSkinObjectComboBox)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_MOUSEMOVE()
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_CAPTURECHANGED()
	//}}AFX_MSG_MAP
	ON_MESSAGE_VOID(WM_MOUSELEAVE, OnMouseLeave)
	ON_MESSAGE_VOID(BM_SETSTYLE, OnStyleChanged)
	ON_MESSAGE_VOID(WM_ENABLE, OnStyleChanged)
	ON_MESSAGE_VOID(WM_UPDATEUISTATE, OnStyleChanged)
	ON_MESSAGE(WM_PRINTCLIENT, OnPrintClient)

	ON_MESSAGE_VOID(WM_LBUTTONUP, OnStyleChanged)
END_MESSAGE_MAP()
#include "Common/Base/Diagnostic/XTPEndAfxMap.h"

/////////////////////////////////////////////////////////////////////////////
// CXTPSkinObjectComboBox message handlers

int CXTPSkinObjectComboBox::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CXTPSkinObject::OnCreate(lpCreateStruct) == -1)
		return -1;

	Invalidate(FALSE);

	ModifyStyle(WS_BORDER, 0);
	ModifyStyleEx(WS_EX_CLIENTEDGE | WS_EX_STATICEDGE, 0, SWP_FRAMECHANGED);

	return 0;
}

HWND AFX_CDECL CXTPSkinObjectComboBox::GetComboListBox(HWND hWnd)
{
	HWND hwndList = NULL;
	struct XTP_COMBOBOXINFO
	{
		DWORD cbSize;
		RECT rcItem;
		RECT rcButton;
		DWORD stateButton;
		HWND hwndCombo;
		HWND hwndItem;
		HWND hwndList;
	};

	static CXTPModuleHandle modUser32Dll;
	if (modUser32Dll.GetModuleHandle(_T("USER32.DLL")))
	{
		typedef BOOL(WINAPI * PFNGETCOMBOBOXINFO)(HWND hwndCombo, XTP_COMBOBOXINFO * pcbi);
		PFNGETCOMBOBOXINFO pfnGetComboBoxInfo = XTPToFunctionPtr<PFNGETCOMBOBOXINFO>(
			::GetProcAddress(modUser32Dll, "GetComboBoxInfo"));
		if (NULL != pfnGetComboBoxInfo)
		{
			XTP_COMBOBOXINFO ci;
			ZeroMemory(&ci, sizeof(ci));
			ci.cbSize = sizeof(ci);

			if ((*pfnGetComboBoxInfo)(hWnd, &ci))
			{
				hwndList = ci.hwndList;
			}
		}
	}

	return hwndList;
}

LRESULT CXTPSkinObjectComboBox::OnPrintClient(WPARAM wParam, LPARAM lParam)
{
	if (((lParam & PRF_CLIENT) == 0) || m_bPaint || !IsDrawingEnabled())
		return Default();

	CDC* pDC = CDC::FromHandle((HDC)wParam);
	if (pDC)
		OnDraw(pDC);
	return 1;
}

void CXTPSkinObjectComboBox::OnPaint()
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

void CXTPSkinObjectComboBox::OnDraw(CDC* pDC)
{
	DWORD dwCbsStyle = (GetStyle() & (CBS_SIMPLE | CBS_DROPDOWN));
	if (CBS_SIMPLE == dwCbsStyle || 0 == dwCbsStyle)
	{
		CXTPClientRect rcEdit(this);
		rcEdit.bottom = rcEdit.top + XTPToInt(SendMessage(CB_GETITEMHEIGHT, XTPToWPARAM(-1)))
						+ 2 * GetSystemMetrics(SM_CYFIXEDFRAME);

		CXTPSkinManagerClass* pClass	 = m_pManager->GetSkinClass(this, _T("EDIT"));
		CXTPSkinManagerMetrics* pMetrics = GetMetrics();

		HBRUSH hBrush = GetClientBrush(pDC);

		if (pClass->GetThemeEnumValue(0, 0, TMT_BGTYPE) == BT_BORDERFILL)
		{
			COLORREF clrBorderColor = pClass->GetThemeColor(0, 0, TMT_BORDERCOLOR);
			pDC->Draw3dRect(rcEdit, clrBorderColor, clrBorderColor);

			rcEdit.DeflateRect(XTP_DPI_X(1), XTP_DPI_Y(1));
			XTPSkinFrameworkDrawFrame(pDC->GetSafeHdc(), &rcEdit, XTP_DPI_X(1), hBrush);
		}
		else
		{
			pDC->Draw3dRect(rcEdit, pMetrics->m_clrEdgeShadow, pMetrics->m_clrEdgeHighLight);

			rcEdit.DeflateRect(XTP_DPI_X(1), XTP_DPI_Y(1));
			pDC->Draw3dRect(rcEdit, pMetrics->m_clrEdgeDkShadow, pMetrics->m_clrEdgeLight);
		}

		rcEdit.DeflateRect(XTP_DPI_X(1), XTP_DPI_Y(1));
		XTPSkinFrameworkDrawFrame(pDC->GetSafeHdc(), &rcEdit, XTP_DPI_X(1), hBrush);
		return;
	}

	CRect rc;
	GetClientRect(&rc);

	BOOL bPressed	 = (GetKeyState(VK_LBUTTON) < 0) && SendMessage(CB_GETDROPPEDSTATE);
	BOOL bHot		 = m_bHot || (::GetCapture() == m_hWnd);
	BOOL bEnabled	 = IsWindowEnabled();
	BOOL bRightAlign = (0 != (GetExStyle() & WS_EX_RIGHT));

	CXTPBufferDC memDC(*pDC, rc);

	int nMessage  = GetClientBrushMessage();
	HBRUSH hBrush = GetFillBackgroundBrush(&memDC, nMessage);

	m_bPaint = TRUE;
	DefWindowProc(WM_PRINT, XTPToWPARAM(memDC.m_hDC), PRF_CLIENT);
	m_bPaint = FALSE;

	CXTPSkinManagerClass* pClassComboBox = GetSkinManager()->GetSkinClass(this, _T("COMBOBOX"));

	memDC.SelectClipRgn(NULL);

	int nState = !bEnabled	? CBXS_DISABLED
				 : bPressed ? CBXS_PRESSED
				 : bHot		? CBXS_HOT
							: CBXS_NORMAL;

	int nBGType = pClassComboBox->GetThemeEnumValue(0, nState, TMT_BGTYPE);

	if (bRightAlign)
	{
		::FillRect(memDC.GetSafeHdc(),
				   CRect(rc.left, rc.top,
						 rc.left
							 + max(GetMetrics()->m_cxVScroll + XTP_DPI_X(1),
								   GetMetrics()->m_cxOsVScroll + XTP_DPI_X(3)),
						 rc.bottom),
				   hBrush);
	}
	else
	{
		::FillRect(memDC.GetSafeHdc(),
				   CRect(rc.right
							 - max(GetMetrics()->m_cxVScroll + XTP_DPI_X(1),
								   GetMetrics()->m_cxOsVScroll + XTP_DPI_X(3)),
						 rc.top, rc.right, rc.bottom),
				   hBrush);
	}

	int nThumbWidth = GetMetrics()->m_cxVScroll;

	if (nBGType == BT_IMAGEFILE)
	{
		CRect rectClient(rc.left + XTP_DPI_X(3), rc.top + XTP_DPI_Y(3),
						 rc.right - nThumbWidth - XTP_DPI_X(3), rc.bottom - XTP_DPI_Y(3));
		if (bRightAlign)
			rectClient.OffsetRect(nThumbWidth, 0);

		memDC.ExcludeClipRect(rectClient);

		pClassComboBox->DrawThemeBackground(&memDC, 0, nState, rc);
	}
	else if (nBGType == BT_BORDERFILL)
	{
		COLORREF clrBorderColor = pClassComboBox->GetThemeColor(0, nState, TMT_BORDERCOLOR);

		memDC.Draw3dRect(rc, clrBorderColor, clrBorderColor);
		rc.DeflateRect(XTP_DPI_X(1), XTP_DPI_Y(1));
		CRect rcClient(rc);
		if (bRightAlign)
			rcClient.left += nThumbWidth;
		else
			rcClient.right -= nThumbWidth;
		XTPSkinFrameworkDrawFrame(memDC, &rcClient, XTP_DPI_X(2), hBrush);
	}

	CRect rcDropDownButton(rc);
	if (bRightAlign)
		rcDropDownButton.right = rcDropDownButton.left + nThumbWidth;
	else
		rcDropDownButton.left = rcDropDownButton.right - nThumbWidth;

	pClassComboBox->DrawThemeBackground(&memDC, CP_DROPDOWNBUTTON, nState, rcDropDownButton);
}

BOOL CXTPSkinObjectComboBox::OnEraseBkgnd(CDC* pDC)
{
	BOOL bResult = TRUE;
	if (!IsDrawingEnabled())
	{
		bResult = CXTPSkinObjectFrame::OnEraseBkgnd(pDC);
	}
	return bResult;
}

void CXTPSkinObjectComboBox::OnMouseLeave()
{
	OnMouseMove(0, CPoint(-1, -1));
}

void CXTPSkinObjectComboBox::OnMouseMove(UINT /*nFlags*/, CPoint point)
{
	Default();

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

void CXTPSkinObjectComboBox::OnSetFocus(CWnd* pOldWnd)
{
	CXTPSkinObject::OnSetFocus(pOldWnd);
	Invalidate(FALSE);
}

void CXTPSkinObjectComboBox::OnKillFocus(CWnd* pNewWnd)
{
	CXTPSkinObject::OnKillFocus(pNewWnd);
	Invalidate(FALSE);
}

void CXTPSkinObjectComboBox::OnStyleChanged()
{
	Default();
	Invalidate(FALSE);
}

void CXTPSkinObjectComboBox::OnCaptureChanged(CWnd* pWnd)
{
	CXTPSkinObject::OnCaptureChanged(pWnd);
	Invalidate(FALSE);
}

int CXTPSkinObjectComboBox::GetClientBrushMessage()
{
	CWnd* pwndEdit = GetWindow(GW_CHILD);
	int nMessage   = !IsWindowEnabled() || (pwndEdit && pwndEdit->GetStyle() & ES_READONLY)
						 ? WM_CTLCOLORSTATIC
						 : WM_CTLCOLOREDIT;

	return nMessage;
}

//////////////////////////////////////////////////////////////////////////
// CXTPSkinObjectDateTime

IMPLEMENT_DYNCREATE(CXTPSkinObjectDateTime, CXTPSkinObjectComboBox)

CXTPSkinObjectDateTime::CXTPSkinObjectDateTime()
{
}

#include "Common/Base/Diagnostic/XTPBeginAfxMap.h"
BEGIN_MESSAGE_MAP(CXTPSkinObjectDateTime, CXTPSkinObjectComboBox)
	//{{AFX_MSG_MAP(CXTPSkinObjectDateTime)
	ON_WM_CREATE()
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP(CXTPSkinObjectDateTime)
END_MESSAGE_MAP()
#include "Common/Base/Diagnostic/XTPEndAfxMap.h"

int CXTPSkinObjectDateTime::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CXTPSkinObject::OnCreate(lpCreateStruct) == -1)
		return -1;

	Invalidate(FALSE);

	return 0;
}

void CXTPSkinObjectDateTime::OnSkinChanged(BOOL bPrevState, BOOL bNewState)
{
	CXTPSkinObjectFrame::OnSkinChanged(bPrevState, bNewState);
}

CRect CXTPSkinObjectDateTime::GetButtonRect()
{
	CXTPClientRect rc(this);
	rc.left = rc.right - GetSystemMetrics(SM_CXVSCROLL);

	return rc;
}

BOOL CXTPSkinObjectDateTime::IsVistaStyle() const
{
	DWORD dwVersion = XTPSystemVersion()->GetComCtlVersion();

	if (dwVersion >= MAKELONG(10, 6) || (HIWORD(dwVersion) == 5 && LOWORD(dwVersion) >= 52))
	{
		return (GetExStyle() & WS_EX_CLIENTEDGE) == 0;
	}

	return FALSE;
}

void CXTPSkinObjectDateTime::OnDraw(CDC* pDC)
{
	CRect rc;
	GetClientRect(&rc);

	BOOL bHot	  = m_bHot || (::GetCapture() == m_hWnd);
	BOOL bEnabled = IsWindowEnabled();

	CXTPBufferDC memDC(*pDC, rc);
	COLORREF clr = GetColor(IsWindowEnabled() ? COLOR_WINDOW : COLOR_3DFACE);
	memDC.FillSolidRect(rc, clr);

	if (m_bActiveX)
	{
		m_bPaint = TRUE;
		CXTPBufferDC dcMem2(*pDC, rc);
		DefWindowProc(WM_PAINT, (WPARAM)dcMem2.GetSafeHdc(), 0);
		dcMem2.Discard();
		m_bPaint = FALSE;
	}

	DefWindowProc(WM_PRINTCLIENT, (WPARAM)memDC.m_hDC, 0);

	CXTPSkinManagerClass* pClassComboBox = GetSkinManager()->GetSkinClass(this, _T("COMBOBOX"));

	int nState	= !bEnabled ? CBXS_DISABLED : bHot ? CBXS_HOT : CBXS_NORMAL;
	int nBGType = pClassComboBox->GetThemeEnumValue(0, nState, TMT_BGTYPE);

	int nThumbWidth = GetSystemMetrics(SM_CXVSCROLL);

	BOOL bVistaStyle = IsVistaStyle();

	if (!bVistaStyle)
	{
		memDC.FillSolidRect(CRect(rc.right - nThumbWidth - XTP_DPI_X(3), rc.top, rc.right,
								  rc.bottom),
							clr);
	}

	if (bVistaStyle)
	{
		if (nBGType == BT_IMAGEFILE)
		{
			CRect rectClient(rc.left + XTP_DPI_X(2), rc.top + XTP_DPI_Y(2), rc.right - XTP_DPI_X(2),
							 rc.bottom - XTP_DPI_Y(2));
			memDC.ExcludeClipRect(rectClient);

			pClassComboBox->DrawThemeBackground(&memDC, 0, nState, rc);
		}
		else if (nBGType == BT_BORDERFILL)
		{
			COLORREF clrBorderColor = pClassComboBox->GetThemeColor(0, nState, TMT_BORDERCOLOR);

			memDC.Draw3dRect(rc, clrBorderColor, clrBorderColor);
			rc.DeflateRect(XTP_DPI_X(1), XTP_DPI_Y(1));
			CRect rcClient(rc);
			memDC.Draw3dRect(&rcClient, clr, clr);
		}
	}

	memDC.SelectClipRgn(NULL);

	if (GetStyle() & DTS_SHOWNONE)
	{
		CRect rcCheck(rc);
		rcCheck.DeflateRect(XTP_DPI_X(1), XTP_DPI_Y(1));
		rcCheck.right = rcCheck.left + rcCheck.Height();

		memDC.FillSolidRect(rcCheck, clr);

		CXTPSkinManagerClass* pClassButton = GetSkinManager()->GetSkinClass(this, _T("BUTTON"));

		CSize size(13, 13);
		pClassButton->GetThemePartSize(BP_CHECKBOX, 0, NULL, TS_TRUE, &size);
		size = XTP_DPI(size);

		CRect rcMark(CPoint((rcCheck.left + rcCheck.right - size.cx) / 2,
							(rcCheck.top + rcCheck.bottom - size.cy) / 2),
					 size);

		int nStateButton = !bEnabled ? CBS_UNCHECKEDDISABLED : CBS_UNCHECKEDNORMAL;

		SYSTEMTIME st;
		BOOL bChecked = SendMessage(DTM_GETSYSTEMTIME, 0, (LPARAM)&st) != GDT_NONE;

		if (bChecked)
			nStateButton += 4;

		pClassButton->DrawThemeBackground(&memDC, BP_CHECKBOX, nStateButton, &rcMark);
	}

	if (((GetStyle() & DTS_UPDOWN) == 0) && !bVistaStyle)
	{
		CRect rcButton = rc;
		rcButton.left  = rcButton.right - GetSystemMetrics(SM_CXVSCROLL);

		memDC.FillSolidRect(rcButton.left, rcButton.top, rcButton.Width(), rcButton.Height(), clr);
		pClassComboBox->DrawThemeBackground(&memDC, CP_DROPDOWNBUTTON, nState, rcButton);
	}
}

int CXTPSkinObjectDateTime::GetClientBrushMessage()
{
#ifdef _XTP_ACTIVEX
	if (m_bActiveX)
	{
		CWnd* pwndEdit = GetWindow(GW_CHILD);
		int nMessage   = !IsWindowEnabled() || (pwndEdit && pwndEdit->GetStyle() & ES_READONLY)
							 ? WM_CTLCOLORSTATIC
							 : WM_CTLCOLORLISTBOX;

		return nMessage;
	}
#endif
	return CXTPSkinObjectComboBox::GetClientBrushMessage();
}

void CXTPSkinObjectDateTime::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (IsWindowEnabled() && ((GetStyle() & DTS_UPDOWN) == 0) && GetButtonRect().PtInRect(point))
	{
		SetFocus();
		DefWindowProc(WM_SYSKEYDOWN, VK_DOWN, 0);
		return;
	}

	CXTPSkinObjectComboBox::OnLButtonDown(nFlags, point);
}

void CXTPSkinObjectDateTime::RefreshMetrics()
{
	CXTPSkinObjectComboBox::RefreshMetrics();

	SendMessage(DTM_SETMCCOLOR, MCSC_BACKGROUND, (LPARAM)GetColor(COLOR_WINDOW));
	SendMessage(DTM_SETMCCOLOR, MCSC_MONTHBK, (LPARAM)GetColor(COLOR_WINDOW));
	SendMessage(DTM_SETMCCOLOR, MCSC_TEXT, (LPARAM)GetColor(COLOR_WINDOWTEXT));
	SendMessage(DTM_SETMCCOLOR, MCSC_TITLEBK, (LPARAM)GetColor(COLOR_ACTIVECAPTION));
	SendMessage(DTM_SETMCCOLOR, MCSC_TITLETEXT, (LPARAM)GetColor(COLOR_CAPTIONTEXT));
	SendMessage(DTM_SETMCCOLOR, MCSC_TRAILINGTEXT, (LPARAM)GetColor(COLOR_GRAYTEXT));
}

void CXTPSkinObjectDateTime::DrawFrame(CDC* pDC)
{
	if (GetExStyle() & WS_EX_CLIENTEDGE)
	{
		BOOL bHot	  = m_bHot || (::GetCapture() == m_hWnd);
		BOOL bEnabled = IsWindowEnabled();

		CXTPSkinManagerClass* pClassComboBox = GetSkinManager()->GetSkinClass(this, _T("COMBOBOX"));

		int nState	= !bEnabled ? CBXS_DISABLED : bHot ? CBXS_HOT : CBXS_NORMAL;
		int nBGType = pClassComboBox->GetThemeEnumValue(0, nState, TMT_BGTYPE);

		if (nBGType == BT_IMAGEFILE)
		{
			CXTPWindowRect rc(this);
			rc.OffsetRect(-rc.TopLeft());

			CRect rectClient(rc.left + XTP_DPI_X(2), rc.top + XTP_DPI_Y(2), rc.right - XTP_DPI_X(2),
							 rc.bottom - XTP_DPI_Y(2));
			pDC->ExcludeClipRect(rectClient);

			pClassComboBox->DrawThemeBackground(pDC, 0, nState, rc);
			pDC->SelectClipRgn(NULL);
			return;
		}
	}

	CXTPSkinObjectComboBox::DrawFrame(pDC);
}

//////////////////////////////////////////////////////////////////////////
// CXTPSkinObjectMonthCal

IMPLEMENT_DYNCREATE(CXTPSkinObjectMonthCal, CXTPSkinObjectFrame)

CXTPSkinObjectMonthCal::CXTPSkinObjectMonthCal()
{
}

void CXTPSkinObjectMonthCal::RefreshMetrics()
{
	CXTPSkinObjectFrame::RefreshMetrics();

	SendMessage(MCM_SETCOLOR, MCSC_BACKGROUND, (LPARAM)GetColor(COLOR_WINDOW));
	SendMessage(MCM_SETCOLOR, MCSC_MONTHBK, (LPARAM)GetColor(COLOR_WINDOW));
	SendMessage(MCM_SETCOLOR, MCSC_TEXT, (LPARAM)GetColor(COLOR_WINDOWTEXT));
	SendMessage(MCM_SETCOLOR, MCSC_TITLEBK, (LPARAM)GetColor(COLOR_ACTIVECAPTION));
	SendMessage(MCM_SETCOLOR, MCSC_TITLETEXT, (LPARAM)GetColor(COLOR_CAPTIONTEXT));
	SendMessage(MCM_SETCOLOR, MCSC_TRAILINGTEXT, (LPARAM)GetColor(COLOR_GRAYTEXT));
}
