// XTPSkinObjectReBar.cpp: implementation of the CXTPSkinObjectReBar class.
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
#include "Common/XTPSystemHelpers.h"
#include "Common/XTPSynchro.h"
#include "Common/XTPApplication.h"
#include "Common/XTPSingleton.h"
#include "Common/XTPGdiObjects.h"
#include "Common/XTPDrawHelpers.h"
#include "Common/ScrollBar/XTPScrollBase.h"
#include "Common/XTPWinThemeWrapper.h"
#include "Common/XTPSynchro.h"

#include "SkinFramework/XTPSkinObject.h"
#include "SkinFramework/XTPSkinObjectFrame.h"
#include "SkinFramework/XTPSkinObjectReBar.h"
#include "SkinFramework/XTPSkinManager.h"
#include "SkinFramework/XTPSkinDrawTools.h"

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

#ifdef _DEBUG
#	undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#	define new DEBUG_NEW
#endif

#if !defined(RP_BACKGROUND) && !defined(VSCLASS_REBAR)
#	define RP_BACKGROUND 6
#endif

#ifndef RBBS_USECHEVRON
#	define RBBS_USECHEVRON                                                                        \
		0x00000200 // display drop-down button for this band if it's sized smaller than ideal width
#endif

#ifndef RBBS_HIDETITLE
#	define RBBS_HIDETITLE 0x00000400 // keep band title hidden
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CXTPSkinObjectReBar, CXTPSkinObjectFrame)

CXTPSkinObjectReBar::CXTPSkinObjectReBar()
{
	m_strClassName = _T("REBAR");
}

CXTPSkinObjectReBar::~CXTPSkinObjectReBar()
{
}

#include "Common/Base/Diagnostic/XTPBeginAfxMap.h"
BEGIN_MESSAGE_MAP(CXTPSkinObjectReBar, CXTPSkinObjectFrame)
	//{{AFX_MSG_MAP(CXTPSkinObjectReBar)
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_MESSAGE(WM_PRINTCLIENT, OnPrintClient)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
#include "Common/Base/Diagnostic/XTPEndAfxMap.h"

/////////////////////////////////////////////////////////////////////////////
// CXTPSkinObjectReBar message handlers

int CXTPSkinObjectReBar::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CXTPSkinObjectFrame::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}

BOOL CXTPSkinObjectReBar::OnEraseBkgnd(CDC* pDC)
{
	BOOL bResult = TRUE;
	if (!IsDrawingEnabled())
	{
		bResult = CXTPSkinObjectFrame::OnEraseBkgnd(pDC);
	}
	return bResult;
}

LRESULT CXTPSkinObjectReBar::OnPrintClient(WPARAM wParam, LPARAM lParam)
{
	if ((lParam & PRF_CLIENT) == 0 || !IsDrawingEnabled())
		return Default();

	CDC* pDC = CDC::FromHandle((HDC)wParam);
	if (pDC)
		OnDraw(pDC);
	return 1;
}

void CXTPSkinObjectReBar::OnPaint()
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

void CXTPSkinObjectReBar::OnDraw(CDC* pDC)
{
	ASSERT(NULL != pDC);

	CXTPClientRect rcClient(this);
	CXTPBufferDC dcMem(*pDC, rcClient);
	CXTPSkinManagerClass* pClassReBar = GetSkinClass();
	DWORD dwStyle					  = GetStyle();
	BOOL bVertical					  = (0 != (dwStyle & (CCS_VERT)));

	// Draw background.
	pClassReBar->DrawThemeBackground(&dcMem, RP_BACKGROUND, 0, &rcClient);

	if (!bVertical)
	{
		// Draw bands.
		int nBands = static_cast<int>(SendMessage(RB_GETBANDCOUNT));
		for (int nBand = 0; nBand < nBands; ++nBand)
		{
			TCHAR szBandText[0x100];
			szBandText[0] = _T('\0');

			REBARBANDINFO rbbi = { 0 };
			rbbi.cbSize		   = sizeof(REBARBANDINFO);
			rbbi.fMask		   = RBBIM_STYLE | RBBIM_COLORS | RBBIM_TEXT | RBBIM_IMAGE | RBBIM_CHILD
						 | RBBIM_CHILDSIZE | RBBIM_SIZE | RBBIM_BACKGROUND | RBBIM_ID
						 | RBBIM_IDEALSIZE | RBBIM_LPARAM | RBBIM_HEADERSIZE;
#ifdef RBBIM_CHEVRONLOCATION
			rbbi.fMask |= RBBIM_CHEVRONLOCATION;
#endif
#ifdef RBBIM_CHEVRONSTATE
			rbbi.fMask |= RBBIM_CHEVRONSTATE;
#endif
			rbbi.lpText = szBandText;
			rbbi.cch	= _countof(szBandText);
			if (0
				!= SendMessage(RB_GETBANDINFO, XTPToWPARAM(nBand), reinterpret_cast<LPARAM>(&rbbi)))
			{
				CRect rcBand;
				if (0
					!= SendMessage(RB_GETRECT, XTPToWPARAM(nBand),
								   reinterpret_cast<LPARAM>(&rcBand)))
				{
					DrawBand(&dcMem, pClassReBar, rcBand, rbbi);
				}
			}
		}

		DrawBandBorders(&dcMem);
	}
	else
	{
		TRACE(_T("WARNING: Vertical ReBar drawing not supported due to native limitations for ")
			  _T("vertical ReBar."));
	}
}

void CXTPSkinObjectReBar::DrawBandBorders(CDC* pDC)
{
	ASSERT(NULL != pDC);

	DWORD dwStyle = GetStyle();
	int nBands	= static_cast<int>(SendMessage(RB_GETBANDCOUNT));

	// Draw band borders if enabled.
	if (0 != (dwStyle & RBS_BANDBORDERS) && 0 < nBands)
	{
		CXTPClientRect rcClient(this);
		CXTPSkinManagerMetrics* pMetrics = XTPSkinManager()->GetMetrics();

		COLORREF clrBorderColorLight = pMetrics->GetColor(COLOR_3DHIGHLIGHT);
		COLORREF clrBorderColorDark  = pMetrics->GetColor(COLOR_3DSHADOW);

		// Determine vertical spacing.
		CRect rcBand, rcPrevBand;
		int nVertSpacing = 0;
		int nBand;
		for (nBand = 1; nBand < nBands; ++nBand)
		{
			if (0 != SendMessage(RB_GETRECT, XTPToWPARAM(nBand), reinterpret_cast<LPARAM>(&rcBand))
				&& 0
					   != SendMessage(RB_GETRECT, XTPToWPARAM(nBand - 1),
									  reinterpret_cast<LPARAM>(&rcPrevBand)))
			{
				if (rcPrevBand.bottom < rcBand.top)
				{
					nVertSpacing = rcBand.top - rcPrevBand.bottom;
					break;
				}
			}
		}

		// Draw inner borders.
		for (nBand = 1; nBand < nBands; ++nBand)
		{
			if (0 != SendMessage(RB_GETRECT, XTPToWPARAM(nBand), reinterpret_cast<LPARAM>(&rcBand))
				&& 0
					   != SendMessage(RB_GETRECT, XTPToWPARAM(nBand - 1),
									  reinterpret_cast<LPARAM>(&rcPrevBand)))
			{
				// Draw verical border.
				int nBorderX = (rcBand.left + rcPrevBand.right) / 2;
				pDC->Draw3dRect(nBorderX - XTP_DPI_X(1), rcBand.top - nVertSpacing / 2,
								XTP_DPI_X(2), rcBand.Height() + nVertSpacing, clrBorderColorDark,
								clrBorderColorLight);

				// Draw horizontal borders.
				if (rcPrevBand.bottom < rcBand.top)
				{
					int nBorderY = (rcBand.top + rcPrevBand.bottom) / 2;
					pDC->Draw3dRect(0, nBorderY - XTP_DPI_Y(1), rcClient.right, XTP_DPI_Y(2),
									clrBorderColorDark, clrBorderColorLight);
				}
			}
		}

		// Draw the very last horizontal border.
		pDC->Draw3dRect(0, rcBand.bottom, rcClient.right, XTP_DPI_X(1), clrBorderColorDark,
						clrBorderColorLight);
	}
}

void CXTPSkinObjectReBar::DrawBand(CDC* pDC, CXTPSkinManagerClass* pClassReBar, const CRect& rcBand,
								   const REBARBANDINFO& rbbi)
{
	ASSERT(NULL != pDC);
	ASSERT(NULL != pClassReBar);

	const int GripperWidth = 6;
	const int Spacing	  = 3;

	CXTPSkinManagerMetrics* pMetrics = XTPSkinManager()->GetMetrics();
	int nOffsetFromBeginning		 = 0;

	// Draw custom band background if enabled.
	if (NULL != rbbi.hbmBack)
	{
		CBitmap bmp;
		bmp.Attach(rbbi.hbmBack);
		CBrush brush(&bmp);
		pDC->FillRect(&rcBand, &brush);
		brush.DeleteObject();
		bmp.Detach();
	}

	// Draw gripper if enabled.
	BOOL bGripperEnabled = (0 == (rbbi.fStyle & RBBS_NOGRIPPER));
	if (bGripperEnabled && 0 == (rbbi.fStyle & RBBS_GRIPPERALWAYS))
	{
		int nBandCount  = static_cast<int>(SendMessage(RB_GETBANDCOUNT));
		bGripperEnabled = (1 < nBandCount);
	}

	if (bGripperEnabled)
	{
		CRect rcGripper = rcBand;
		rcGripper.right = rcGripper.left + GripperWidth;
		pClassReBar->DrawThemeBackground(pDC, RP_GRIPPER, 0, rcGripper);
		nOffsetFromBeginning += GripperWidth + Spacing;
	}

	// Draw image if enabled.
	if (0 <= rbbi.iImage)
	{
		REBARINFO barInfo = { 0 };
		barInfo.cbSize	= sizeof(barInfo);
		barInfo.fMask	 = RBIM_IMAGELIST;
		if (0 < SendMessage(RB_GETBARINFO, 0, reinterpret_cast<LPARAM>(&barInfo)))
		{
			if (NULL != barInfo.himl)
			{
				int cx = 0, cy = 0;
				if (ImageList_GetIconSize(barInfo.himl, &cx, &cy))
				{
					CPoint imagePos;
					imagePos.x = rcBand.left + nOffsetFromBeginning;
					nOffsetFromBeginning += cx + Spacing;
					imagePos.y = ((rcBand.top + rcBand.bottom) - cy) / 2;

					CRect rcImage(imagePos, CSize(cx, cy));
					CXTPDrawHelpers::StretchImageListImage(pDC, rcImage,
														   CImageList::FromHandle(barInfo.himl),
														   rbbi.iImage);
				}
			}
		}
	}

	// Draw label if specified.
	if (NULL != rbbi.lpText && _T('\0') != *rbbi.lpText && 0 == (rbbi.fStyle & RBBS_HIDETITLE))
	{
		CFont* pOldFont = pDC->SelectObject(GetFont());

		int nLength	= static_cast<int>(_tcslen(rbbi.lpText));
		CSize textSize = pDC->GetTextExtent(rbbi.lpText, nLength);
		CRect rcText;
		rcText.left   = rcBand.left + nOffsetFromBeginning;
		rcText.top	= rcBand.top;
		rcText.right  = rcText.left + textSize.cx;
		rcText.bottom = rcBand.bottom;
		nOffsetFromBeginning += textSize.cx + Spacing;

		COLORREF clrTextColor = pMetrics->GetColor(COLOR_WINDOWTEXT);
		COLORREF crOldColor   = pDC->SetTextColor(clrTextColor);
		int nOldBkMode		  = pDC->SetBkMode(TRANSPARENT);
		XTPSkinFrameworkDrawText(*pDC, rbbi.lpText, nLength, &rcText, DT_SINGLELINE | DT_VCENTER);
		pDC->SetBkMode(nOldBkMode);
		pDC->SetTextColor(crOldColor);
		pDC->SelectObject(pOldFont);
	}

	// Draw chevron if enabled.
	if (0 != (rbbi.fStyle & RBBS_USECHEVRON))
	{
		CRect rcChevron;
		int nChevronState = CHEVS_NORMAL;

#if defined(RBBIM_CHEVRONLOCATION) && defined(RBBIM_CHEVRONSTATE)
		rcChevron = rbbi.rcChevronLocation;
		switch (rbbi.uChevronState)
		{
			case STATE_SYSTEM_PRESSED: nChevronState = CHEVS_PRESSED; break;
			case STATE_SYSTEM_HOTTRACKED: nChevronState = CHEVS_HOT; break;
		}
#else
		const int ChevronSize = XTP_DPI_X(16);
		rcChevron			  = rcBand;
		rcChevron.left		  = rcChevron.right - ChevronSize;

		BOOL bLKeyDown = (0 != (GetKeyState(VK_LBUTTON) & 0x80));
		CPoint ptCursor;
		GetCursorPos(&ptCursor);
		ScreenToClient(&ptCursor);

		if (rcChevron.PtInRect(ptCursor))
		{
			nChevronState = bLKeyDown ? CHEVS_PRESSED : CHEVS_HOT;
		}

#endif

		BOOL bNoPlaceForChevron = !((rcBand.left + nOffsetFromBeginning) < rcChevron.left);
		if (!bNoPlaceForChevron)
		{
			pClassReBar->DrawThemeBackground(pDC, RP_CHEVRON, nChevronState, &rcChevron);
		}
	}
}
