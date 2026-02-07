// XTPSkinObjectTab.cpp: implementation of the CXTPSkinObjectTab class.
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
#include "SkinFramework/XTPSkinObjectFrame.h"
#include "SkinFramework/XTPSkinObjectTab.h"
#include "SkinFramework/XTPSkinManager.h"

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

#ifdef _DEBUG
#	undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#	define new DEBUG_NEW
#endif

AFX_INLINE BOOL IsLeft(DWORD dwStyle)
{
	return ((dwStyle & (TCS_VERTICAL | TCS_RIGHT | TCS_BOTTOM)) == TCS_VERTICAL);
}
AFX_INLINE BOOL IsRight(DWORD dwStyle)
{
	return ((dwStyle & (TCS_VERTICAL | TCS_RIGHT | TCS_BOTTOM)) == (TCS_VERTICAL | TCS_RIGHT));
}
AFX_INLINE BOOL IsTop(DWORD dwStyle)
{
	return ((dwStyle & (TCS_VERTICAL | TCS_RIGHT | TCS_BOTTOM)) == 0);
}
AFX_INLINE BOOL IsBottom(DWORD dwStyle)
{
	return ((dwStyle & (TCS_VERTICAL | TCS_RIGHT | TCS_BOTTOM)) == TCS_BOTTOM);
}
AFX_INLINE BOOL IsVert(DWORD dwStyle)
{
	return (IsRight(dwStyle) || IsLeft(dwStyle));
}
AFX_INLINE BOOL IsHorz(DWORD dwStyle)
{
	return (IsTop(dwStyle) || IsBottom(dwStyle));
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CXTPSkinObjectTab, CXTPSkinObjectFrame)

CXTPSkinObjectTab::CXTPSkinObjectTab()
{
	m_strClassName = _T("TAB");
	m_nHotItem	 = -1;
	m_bPaint	   = FALSE;
}

CXTPSkinObjectTab::~CXTPSkinObjectTab()
{
}

#include "Common/Base/Diagnostic/XTPBeginAfxMap.h"
BEGIN_MESSAGE_MAP(CXTPSkinObjectTab, CXTPSkinObjectFrame)
	//{{AFX_MSG_MAP(CXTPSkinObjectTab)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_MESSAGE(WM_PRINTCLIENT, OnPrintClient)
	ON_WM_MOUSEMOVE()
	ON_MESSAGE_VOID(WM_MOUSELEAVE, OnMouseLeave)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
#include "Common/Base/Diagnostic/XTPEndAfxMap.h"

/////////////////////////////////////////////////////////////////////////////
// CXTPSkinObjectTab message handlers

BOOL CXTPSkinObjectTab::OnEraseBkgnd(CDC* pDC)
{
	BOOL bResult = TRUE;
	if (IsDrawingEnabled())
	{
		CXTPClientRect rc(this);
		CXTPBufferDC dcMem(pDC->GetSafeHdc(), rc);
		FillClient(&dcMem, rc);
	}
	else
	{
		bResult = CXTPSkinObjectFrame::OnEraseBkgnd(pDC);
	}
	return bResult;
}

CRect CXTPSkinObjectTab::GetHeaderRect()
{
	CRect rcHeader;
	GetClientRect(&rcHeader);

	CTabCtrl* pTabCtrl = (CTabCtrl*)this;

	// get the selected tab item rect.
	CRect rcItem;
	pTabCtrl->GetItemRect(pTabCtrl->GetCurSel(), &rcItem);

	DWORD dwStyle = GetStyle();

	if (IsRight(dwStyle))
	{
		rcHeader.left = rcItem.left;
	}
	else if (IsLeft(dwStyle))
	{
		rcHeader.right = rcItem.right;
	}
	else if (IsBottom(dwStyle))
	{
		rcHeader.top = rcItem.top;
	}
	else
	{
		rcHeader.bottom = (rcHeader.top + rcItem.Height()) * pTabCtrl->GetRowCount()
						  + GetMetrics()->m_cyEdge;
	}
	return rcHeader;
}

void CXTPSkinObjectTab::FillClient(CDC* pDC, CRect rc)
{
	FillBackground(pDC, rc);

	if (GetStyle() & TCS_BUTTONS)
		return;

	CXTPSkinManagerClass* pClass = GetSkinClass();

	CRect rcClient(rc);
	CRect rcHeader = GetHeaderRect();

	DWORD dwStyle = GetStyle();

	if (IsRight(dwStyle))
	{
		rcClient.right = rcHeader.left;
	}
	else if (IsLeft(dwStyle))
	{
		rcClient.left = rcHeader.right;
	}
	else if (IsBottom(dwStyle))
	{
		rcClient.bottom = rcHeader.top;
	}
	else
	{
		rcClient.top = rcHeader.bottom;
	}

	pClass->DrawThemeBackground(pDC, TABP_PANE, 0, &rcClient);
}

void CXTPSkinObjectTab::FillTabFace(CDC* pDC, CRect rcItem, int iItem, int iCount, BOOL bSelected,
									BOOL bFocused)
{
	if (GetStyle() & TCS_BUTTONS)
	{
		CXTPSkinManagerClass* pClassButton = GetSkinManager()->GetSkinClass(this, _T("BUTTON"));
		int nState						   = bSelected ? PBS_PRESSED : PBS_NORMAL;
		pClassButton->DrawThemeBackground(pDC, BP_PUSHBUTTON, nState, rcItem);
		return;
	}

	if (bSelected)
	{
		rcItem.InflateRect(2, 2, 2, 2); // should not be rescaled
	}

	CXTPSkinManagerClass* pClass = GetSkinClass();

	int nStateId = (bFocused ? TIS_FOCUSED
							 : (bSelected ? (m_nHotItem == iItem) ? TIS_FOCUSED : TIS_SELECTED
										  : (m_nHotItem == iItem ? TIS_HOT : TIS_NORMAL)));

	int nPartId = iItem == 0
					  ? TABP_TOPTABITEMLEFTEDGE
					  : (iItem == iCount - 1 && !bSelected && !bFocused) ? TABP_TOPTABITEMRIGHTEDGE
																		 : TABP_TOPTABITEM;

	pClass->DrawThemeBackground(pDC, nPartId, nStateId, &rcItem);
}

void CXTPSkinObjectTab::DrawTabIcon(CDC* pDC, CRect& rcItem, int iItem)
{
	CTabCtrl* pTabCtrl = (CTabCtrl*)this;

	TC_ITEM tci;
	tci.mask = TCIF_IMAGE;

	BOOL bResult = pTabCtrl->GetItem(iItem, &tci);

#ifdef _UNICODE
	if (!bResult)
	{
		TC_ITEMA tcia;
		::ZeroMemory(&tcia, sizeof(TC_ITEMA));

		tcia.mask = TCIF_IMAGE;

		bResult	= (BOOL)::SendMessage(pTabCtrl->m_hWnd, TCM_GETITEMA, XTPToWPARAM(iItem),
										 (LPARAM)&tcia);
		tci.iImage = tcia.iImage;
	}
#endif
	if (!bResult)
		return;

	CImageList* pImageList = pTabCtrl->GetImageList();
	if (!pImageList || tci.iImage < 0)
		return;

	CPoint point = rcItem.TopLeft();

	DWORD dwStyle = GetStyle();

	if (IsHorz(dwStyle))
	{
		point.Offset(GetMetrics()->m_cxEdge * 2,
					 IsTop(dwStyle) ? GetMetrics()->m_cyEdge : XTP_DPI_Y(1));
	}
	else
	{
		point.Offset(IsLeft(dwStyle) ? GetMetrics()->m_cxEdge : XTP_DPI_X(1),
					 GetMetrics()->m_cyEdge * 2);
	}

	// Draw any associated icons.
	pImageList->Draw(pDC, tci.iImage, point, ILD_TRANSPARENT);

	IMAGEINFO info;
	::ZeroMemory(&info, sizeof(info));

	pImageList->GetImageInfo(tci.iImage, &info);
	CRect rcImage(info.rcImage);

	if (IsHorz(dwStyle))
	{
		rcItem.left += rcImage.Width() + GetMetrics()->m_cxEdge;
	}
	else
	{
		rcItem.top += rcImage.Height() + GetMetrics()->m_cyEdge;
	}
}

void CXTPSkinObjectTab::DrawTabText(CDC* pDC, CRect& rcItem, int iItem, int iCount, BOOL bSelected,
									BOOL bFocused)
{
	CTabCtrl* pTabCtrl = (CTabCtrl*)this;

	CString strCaption;
	LPTSTR pszText = strCaption.GetBuffer(256);
	pszText[0]	 = 0;

	TC_ITEM tci;
	tci.mask	   = TCIF_TEXT;
	tci.pszText	= pszText;
	tci.cchTextMax = 255;

	BOOL bResult = pTabCtrl->GetItem(iItem, &tci);
	strCaption.ReleaseBuffer();

#ifdef _UNICODE
	if (!bResult)
	{
		char tText[256];
		tText[0] = 0;

		TC_ITEMA tcia;
		::ZeroMemory(&tcia, sizeof(TC_ITEMA));

		tcia.mask = TCIF_TEXT;

		// Get the header item text and format
		tcia.pszText	= tText;
		tcia.cchTextMax = 255;

		::SendMessage(pTabCtrl->m_hWnd, TCM_GETITEMA, XTPToWPARAM(iItem), (LPARAM)&tcia);

		strCaption = tText;
	}
#else
	if (!bResult)
		return;
#endif

	if (strCaption.GetLength() == 0)
		return;

	pDC->SetTextColor(GetColor(COLOR_BTNTEXT));

	// Set the font for the tab label.
	DWORD dwStyle = GetStyle();

	CXTPSkinManagerClass* pClass = GetSkinClass();

	int nStateId = (bFocused ? TIS_FOCUSED
							 : (bSelected ? (m_nHotItem == iItem) ? TIS_FOCUSED : TIS_SELECTED
										  : (m_nHotItem == iItem ? TIS_HOT : TIS_NORMAL)));
	int nPartId  = iItem == 0
					  ? TABP_TOPTABITEMLEFTEDGE
					  : iItem == iCount - 1 && !bSelected && !bFocused ? TABP_TOPTABITEMRIGHTEDGE
																	   : TABP_TOPTABITEM;

	// Draw the tab label.
	if (IsHorz(dwStyle))
	{
		UINT dsFlags = DT_SINGLELINE | DT_VCENTER | DT_CENTER | DT_END_ELLIPSIS;

#ifndef _XTP_ACTIVEX
		if (SendMessage(WM_QUERYUISTATE) & UISF_HIDEACCEL)
		{
			dsFlags |= DT_HIDEPREFIX;
		}
#endif

		if (!(GetStyle() & TCS_BUTTONS))
		{
			rcItem.top += XTP_DPI_Y(bSelected ? -2 : 1);
			rcItem.right -= iItem == 0 ? XTP_DPI_X(3) : 0;
		}
		else
		{
			rcItem.top += bSelected ? 0 : -XTP_DPI_Y(1);
		}
		pClass->DrawThemeText(pDC, nPartId, nStateId, strCaption, dsFlags, &rcItem);
	}
	else
	{
		CSize sz	= pDC->GetTextExtent(strCaption);
		rcItem.left = rcItem.right - (rcItem.Width() - sz.cy + 1) / 2;
		rcItem.top  = rcItem.top + (rcItem.Height() - sz.cx + 1) / 2;

		pClass->DrawThemeText(pDC, nPartId, nStateId, strCaption, DT_SINGLELINE | DT_NOCLIP,
							  &rcItem);
	}
}

void CXTPSkinObjectTab::DrawTab(CDC* pDC, int iItem)
{
	CXTPClientRect rc(this);
	CTabCtrl* pTabCtrl = (CTabCtrl*)this;

	// Get the current tab selection.
	int iCurSel = pTabCtrl->GetCurSel();

	// Get the tab item size.
	CRect rcItem;
	pTabCtrl->GetItemRect(iItem, &rcItem);

	if (!CRect().IntersectRect(rcItem, rc))
		return;

	CRect rcItemFocus(rcItem);

	// Draw the tab item.
	BOOL bSelected = (iItem == iCurSel);
	// BOOL bFocused = (pTabCtrl->GetCurFocus() == iItem);	//doesn't work properly
	BOOL bFocused = bSelected && ::GetFocus() == m_hWnd
					&& (SendMessage(WM_QUERYUISTATE) & UISF_HIDEFOCUS) == 0;
	FillTabFace(pDC, rcItem, iItem, pTabCtrl->GetItemCount(), bSelected, bFocused);

	if (GetStyle() & TCS_OWNERDRAWFIXED)
	{
		DRAWITEMSTRUCT dis;
		UINT wID = (UINT)GetDlgCtrlID();

		dis.CtlType	= ODT_TAB;
		dis.CtlID	  = wID;
		dis.itemID	 = XTPToUInt(iItem);
		dis.itemAction = ODA_DRAWENTIRE;

		if (iCurSel == iItem)
			dis.itemState = ODS_SELECTED;
		else
			dis.itemState = 0;

		dis.hwndItem = m_hWnd;
		dis.hDC		 = pDC->GetSafeHdc();
		dis.rcItem   = rcItem;
		dis.itemData = 0;

		::SendMessage(::GetParent(m_hWnd), WM_DRAWITEM, wID, (LPARAM)&dis);
	}
	else
	{
		DrawTabIcon(pDC, rcItem, iItem);
		DrawTabText(pDC, rcItem, iItem, iItem, bSelected, bFocused);
	}

	// draw the focus rect
	if (bFocused)
	{
		rcItemFocus.DeflateRect(GetMetrics()->m_cxEdge / 2, GetMetrics()->m_cyEdge / 2);
		pDC->DrawFocusRect(&rcItemFocus);
	}
}

void CXTPSkinObjectTab::OnPaint()
{
	if (IsDrawingEnabled())
	{
		CXTPSkinObjectPaintDC dc(this);

		if (dc.m_ps.hdc == 0)
		{
			FillClient(&dc, CXTPClientRect(this));
		}

		OnDraw(&dc);
	}
	else
	{
		CXTPSkinObjectFrame::OnPaint();
	}
}

LRESULT CXTPSkinObjectTab::OnPrintClient(WPARAM wParam, LPARAM /*lParam*/)
{
	if (m_bPaint || !IsDrawingEnabled())
	{
		return Default();
	}

	CDC* pDC = CDC::FromHandle((HDC)wParam);
	if (pDC)
	{
		OnDraw(pDC);
	}
	return 1;
}

void CXTPSkinObjectTab::OnDraw(CDC* pDC)
{
	CRect rc	  = GetHeaderRect();
	DWORD dwStyle = GetStyle();

	if (IsRight(dwStyle))
	{
		rc.left -= XTP_DPI_X(2);
	}
	else if (IsLeft(dwStyle))
	{
		rc.right += XTP_DPI_X(2);
	}
	else if (IsBottom(dwStyle))
	{
		rc.top -= XTP_DPI_Y(2);
	}
	else
	{
		rc.bottom += XTP_DPI_Y(2);
	}

	CXTPBufferDC dcMem(*pDC, rc);
	FillClient(&dcMem, CXTPClientRect(this));

	if (m_bActiveX)
	{
		m_bPaint = TRUE;
		CXTPBufferDC dcMem2(*pDC, rc);
		DefWindowProc(WM_PAINT, (WPARAM)dcMem2.GetSafeHdc(), 0);
		dcMem2.Discard();
		m_bPaint = FALSE;
	}

	CTabCtrl* pTabCtrl = (CTabCtrl*)this;

	CXTPFontDC font(&dcMem, GetFont());
	dcMem.SetBkMode(TRANSPARENT);

	for (int iItem = 0; iItem < pTabCtrl->GetItemCount(); ++iItem)
	{
		DrawTab(&dcMem, iItem);
	}

	DrawTab(&dcMem, pTabCtrl->GetCurSel());
}

void CXTPSkinObjectTab::OnMouseLeave()
{
	OnMouseMove(0, CPoint(-1, -1));
}

void CXTPSkinObjectTab::OnMouseMove(UINT /*nFlags*/, CPoint point)
{
	int nHotItem	   = -1;
	CTabCtrl* pTabCtrl = (CTabCtrl*)this;

	for (int iItem = 0; iItem < pTabCtrl->GetItemCount(); ++iItem)
	{
		CRect rcItem;
		pTabCtrl->GetItemRect(iItem, &rcItem);
		if (rcItem.PtInRect(point))
		{
			nHotItem = iItem;
			break;
		}
	}

	if (nHotItem != m_nHotItem)
	{
		m_nHotItem = nHotItem;

		CRect rcHeader = GetHeaderRect();
		rcHeader.bottom += 2; // to update selected tab  which inflated in FillTabFace()
		InvalidateRect(rcHeader, FALSE);

		if (m_nHotItem != -1)
		{
			TRACKMOUSEEVENT tme = { sizeof(TRACKMOUSEEVENT), TME_LEAVE, m_hWnd, HOVER_DEFAULT };
			_TrackMouseEvent(&tme);
		}
	}
}
