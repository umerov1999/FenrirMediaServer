// XTPMarkupScrollViewer.cpp: implementation of the CXTPMarkupScrollViewer class.
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
#include "Common/Base/cxminmax.h"
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
#include "Common/Base/Types/XTPSize.h"
#include "Common/ScrollBar/XTPScrollBase.h"

#include "Markup/XTPMarkupObject.h"
#include "Markup/XTPMarkupInputElement.h"
#include "Markup/XTPMarkupUIElement.h"
#include "Markup/XTPMarkupFrameworkElement.h"
#include "Markup/Controls/XTPMarkupControl.h"
#include "Markup/XTPMarkupContext.h"
#include "Markup/XTPMarkupDrawingContext.h"
#include "Markup/XTPMarkupBuilder.h"

#include "Markup/Controls/XTPMarkupContentControl.h"
#include "Markup/Controls/XTPMarkupScrollViewer.h"
#include "Markup/Controls/XTPMarkupScrollBar.h"

#include "Markup/XTPMarkupRoutedEventArgs.h"
#include "Markup/XTPMarkupMouseEventArgs.h"
#include "Markup/XTPMarkupMouseWheelEventArgs.h"

#include "Markup/XTPMarkupIIDs.h"

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

#ifdef _DEBUG
#	undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#	define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////////
// CXTPMarkupScrollBarGripper

class CXTPMarkupScrollBarGripper : public CXTPMarkupUIElement
{
	DECLARE_MARKUPCLASS(CXTPMarkupScrollBarGripper)

public:
	void OnRender(CXTPMarkupDrawingContext* pDC);
};

IMPLEMENT_MARKUPCLASS(NULL, CXTPMarkupScrollBarGripper, CXTPMarkupUIElement)

void CXTPMarkupScrollBarGripper::RegisterMarkupClass()
{
}

void CXTPMarkupScrollBarGripper::OnRender(CXTPMarkupDrawingContext* pDC)
{
	CRect rc(GetFinalRect());
	pDC->FillSolidRect(rc, GetSysColor(COLOR_BTNFACE));
}

//////////////////////////////////////////////////////////////////////////
// CXTPMarkupScrollViewer

CXTPMarkupDependencyProperty* CXTPMarkupScrollViewer::m_pVerticalScrollBarVisibilityProperty = NULL;
CXTPMarkupDependencyProperty* CXTPMarkupScrollViewer::m_pHorizontalScrollBarVisibilityProperty =
	NULL;

IMPLEMENT_MARKUPCLASS(L"ScrollViewer", CXTPMarkupScrollViewer, CXTPMarkupContentControl)

void CXTPMarkupScrollViewer::RegisterMarkupClass()
{
	m_pVerticalScrollBarVisibilityProperty = CXTPMarkupDependencyProperty::Register(
		L"VerticalScrollBarVisibility", MARKUP_TYPE(CXTPMarkupEnum),
		MARKUP_TYPE(CXTPMarkupScrollViewer),
		new CXTPMarkupPropertyMetadata(NULL, &CXTPMarkupBuilder::ConvertScrollBarVisibility,
									   CXTPMarkupPropertyMetadata::flagAffectsMeasure));

	m_pHorizontalScrollBarVisibilityProperty = CXTPMarkupDependencyProperty::Register(
		L"HorizontalScrollBarVisibility", MARKUP_TYPE(CXTPMarkupEnum),
		MARKUP_TYPE(CXTPMarkupScrollViewer),
		new CXTPMarkupPropertyMetadata(NULL, &CXTPMarkupBuilder::ConvertScrollBarVisibility,
									   CXTPMarkupPropertyMetadata::flagAffectsMeasure));
}

CXTPMarkupScrollViewer::CXTPMarkupScrollViewer()
{
	m_pVerticalScrollBar   = NULL;
	m_pHorizontalScrollBar = NULL;
	m_pScrollBarGripper	   = NULL;

	m_xOffset = m_yOffset = 0;
	SetValue(m_pClipToBoundsProperty, CXTPMarkupBool::CreateTrueValue());
}

CXTPMarkupScrollViewer::~CXTPMarkupScrollViewer()
{
	MARKUP_RELEASE(m_pVerticalScrollBar);
	MARKUP_RELEASE(m_pHorizontalScrollBar);
	MARKUP_RELEASE(m_pScrollBarGripper);
}

int CXTPMarkupScrollViewer::GetVisualChildrenCount() const
{
	return (GetContent() != NULL ? 1 : 0) + (m_pVerticalScrollBar ? 1 : 0)
		   + (m_pHorizontalScrollBar ? 1 : 0) + (m_pScrollBarGripper ? 1 : 0);
}

CXTPMarkupVisual* CXTPMarkupScrollViewer::GetVisualChild(int nIndex) const
{
	CXTPMarkupVisual* pContent = GetContent();
	if (pContent)
		if (nIndex == 0)
			return pContent;
		else
			nIndex--;
	if (m_pVerticalScrollBar)
		if (nIndex == 0)
			return m_pVerticalScrollBar;
		else
			nIndex--;
	if (m_pHorizontalScrollBar)
		if (nIndex == 0)
			return m_pHorizontalScrollBar;
		else
			nIndex--;
	return m_pScrollBarGripper;
}

CSize CXTPMarkupScrollViewer::ArrangeOverride(CSize szFinalSize)
{
	CXTPMarkupUIElement* pContent = GetContent();
	if (!pContent)
		return szFinalSize;

	CSize szViewport(szFinalSize);

	if (m_pVerticalScrollBar)
		szViewport.cx -= m_pVerticalScrollBar->GetDesiredSize().cx;
	if (m_pHorizontalScrollBar)
		szViewport.cy -= m_pHorizontalScrollBar->GetDesiredSize().cy;
	;

	if (m_pVerticalScrollBar)
	{
		if (m_yOffset > 0 && pContent->GetDesiredSize().cy < m_yOffset + szViewport.cy)
		{
			m_yOffset = CXTP_max(0, pContent->GetDesiredSize().cy - szViewport.cy);
		}

		SCROLLINFO si = { 0 };
		si.nMax		  = pContent->GetDesiredSize().cy - 1;
		si.nPage	  = XTPToUIntChecked(szViewport.cy);
		si.nPos		  = m_yOffset;
		m_pVerticalScrollBar->SetScrollInfo(&si);

		m_pVerticalScrollBar->Arrange(
			CRect(szFinalSize.cx - m_pVerticalScrollBar->GetDesiredSize().cx, 0, szFinalSize.cx,
				  szViewport.cy));
	}

	if (m_pHorizontalScrollBar)
	{
		if (m_xOffset > 0 && pContent->GetDesiredSize().cx < m_xOffset + szViewport.cx)
		{
			m_xOffset = CXTP_max(0, pContent->GetDesiredSize().cx - szViewport.cx);
		}

		SCROLLINFO si = { 0 };
		si.nMax		  = pContent->GetDesiredSize().cx - 1;
		si.nPage	  = XTPToUIntChecked(szViewport.cx);
		si.nPos		  = m_xOffset;
		m_pHorizontalScrollBar->SetScrollInfo(&si);

		m_pHorizontalScrollBar->Arrange(
			CRect(0, szFinalSize.cy - m_pHorizontalScrollBar->GetDesiredSize().cy, szViewport.cx,
				  szFinalSize.cy));
	}

	if (m_pScrollBarGripper && m_pHorizontalScrollBar && m_pVerticalScrollBar)
	{
		m_pScrollBarGripper->Arrange(
			CRect(szFinalSize.cx - m_pVerticalScrollBar->GetDesiredSize().cx,
				  szFinalSize.cy - m_pHorizontalScrollBar->GetDesiredSize().cy, szFinalSize.cx,
				  szFinalSize.cy));
	}

	CRect rcContent(0, 0, CXTP_max(szViewport.cx, pContent->GetDesiredSize().cx),
					CXTP_max(szViewport.cy, pContent->GetDesiredSize().cy));
	rcContent.OffsetRect(-m_xOffset, -m_yOffset);
	pContent->Arrange(rcContent);

	return szFinalSize;
}

void CXTPMarkupScrollViewer::CreateScrollBar(int nBar)
{
	if (nBar == SB_VERT && m_pVerticalScrollBar == NULL)
	{
		m_pVerticalScrollBar = (CXTPMarkupScrollBar*)m_pMarkupContext->CreateMarkupObject(
			MARKUP_TYPE(CXTPMarkupScrollBar));
		m_pVerticalScrollBar->SetOrientation(xtpMarkupOrientationVertical);
		m_pVerticalScrollBar->SetLogicalParent(this);
	}

	if (nBar == SB_HORZ && m_pHorizontalScrollBar == NULL)
	{
		m_pHorizontalScrollBar = (CXTPMarkupScrollBar*)m_pMarkupContext->CreateMarkupObject(
			MARKUP_TYPE(CXTPMarkupScrollBar));
		m_pHorizontalScrollBar->SetOrientation(xtpMarkupOrientationHorizontal);
		m_pHorizontalScrollBar->SetLogicalParent(this);
	}
}

CSize CXTPMarkupScrollViewer::MeasureOverride(CXTPMarkupDrawingContext* pDC, CSize szAvailableSize)
{
	CXTPMarkupUIElement* pContent = GetContent();
	if (!pContent)
		return CSize(0, 0);

	CSize szScroll(0, 0);

	XTPMarkupScrollBarVisibility verticalBarVisibility	 = GetVerticalScrollBarVisibility();
	XTPMarkupScrollBarVisibility horizontalBarVisibility = GetHorizontalScrollBarVisibility();

	CSize szViewportSize(INT_MAX, INT_MAX);

	if (horizontalBarVisibility == xtpMarkupScrollBarDisabled)
		szViewportSize.cx = szAvailableSize.cx;
	if (verticalBarVisibility == xtpMarkupScrollBarDisabled)
		szViewportSize.cy = szAvailableSize.cy;

	if (verticalBarVisibility == xtpMarkupScrollBarAuto)
	{
		pContent->Measure(pDC, szViewportSize);

		verticalBarVisibility = (pContent->GetDesiredSize().cy > szAvailableSize.cy)
									? xtpMarkupScrollBarVisible
									: xtpMarkupScrollBarDisabled;
	}

	if (verticalBarVisibility == xtpMarkupScrollBarVisible)
	{
		CreateScrollBar(SB_VERT);
		m_pVerticalScrollBar->Measure(pDC, szAvailableSize);
		szScroll.cx = m_pVerticalScrollBar->GetDesiredSize().cx;

		if (szViewportSize.cx != INT_MAX)
			szViewportSize.cx = CXTP_max(0, szViewportSize.cx - szScroll.cx);
	}
	else
	{
		MARKUP_RELEASE(m_pVerticalScrollBar);
		m_yOffset = 0;
	}

	if (horizontalBarVisibility == xtpMarkupScrollBarAuto)
	{
		pContent->Measure(pDC, szViewportSize);

		horizontalBarVisibility = (pContent->GetDesiredSize().cx > szAvailableSize.cx - szScroll.cx)
									  ? xtpMarkupScrollBarVisible
									  : xtpMarkupScrollBarDisabled;
	}

	if (horizontalBarVisibility == xtpMarkupScrollBarVisible)
	{
		CreateScrollBar(SB_HORZ);
		m_pHorizontalScrollBar->Measure(pDC, szAvailableSize);
		szScroll.cy = m_pHorizontalScrollBar->GetDesiredSize().cy;
		if (szViewportSize.cy != INT_MAX)
			szViewportSize.cy = CXTP_max(0, szViewportSize.cy - szScroll.cy);
	}
	else
	{
		MARKUP_RELEASE(m_pHorizontalScrollBar);
		m_xOffset = 0;
	}

	if (m_pVerticalScrollBar && m_pHorizontalScrollBar)
	{
		if (m_pScrollBarGripper == NULL)
		{
			m_pScrollBarGripper = (CXTPMarkupScrollBarGripper*)m_pMarkupContext->CreateMarkupObject(
				MARKUP_TYPE(CXTPMarkupScrollBarGripper));
			m_pScrollBarGripper->SetLogicalParent(this);
		}
	}
	else
	{
		MARKUP_RELEASE(m_pScrollBarGripper);
	}

	pContent->Measure(pDC, szViewportSize);
	return CSize(CXTP_min(szAvailableSize.cx, pContent->GetDesiredSize().cx + szScroll.cx),
		CXTP_min(szAvailableSize.cy, pContent->GetDesiredSize().cy + szScroll.cy));
}

void CXTPMarkupScrollViewer::Scroll(int nBar, int nSBCode, int pos)
{
	CXTPMarkupScrollBar* pScrollBar = nBar == SB_VERT ? m_pVerticalScrollBar
													  : m_pHorizontalScrollBar;
	if (!pScrollBar)
		return;

	SCROLLINFO si = { 0 };
	pScrollBar->GetScrollInfo(&si);

	int nCurPos = si.nPos;
	int nPage	= (int)si.nPage;
	int nLimit	= CXTP_max(0, si.nMax - CXTP_max(nPage - 1, 0));

	// decide what to do for each different scroll event
	switch (nSBCode)
	{
		case SB_TOP: nCurPos = 0; break;
		case SB_BOTTOM: nCurPos = nLimit; break;
		case SB_LINEUP: nCurPos = CXTP_max(nCurPos - 16, 0); break;
		case SB_LINEDOWN: nCurPos = CXTP_min(nCurPos + 16, nLimit); break;
		case SB_PAGEUP: nCurPos = CXTP_max(nCurPos - nPage, 0); break;
		case SB_PAGEDOWN: nCurPos = CXTP_min(nCurPos + nPage, nLimit); break;
		case SB_THUMBTRACK:
		case SB_THUMBPOSITION: nCurPos = pos; break;
	}

	SetScrollPos(nBar, nCurPos);
}

int CXTPMarkupScrollViewer::GetScrollPos(int nBar)
{
	return (nBar == SB_VERT) ? m_yOffset : m_xOffset;
}

int CXTPMarkupScrollViewer::GetScrollLimit(int nBar)
{
	CXTPMarkupScrollBar* pScrollBar = nBar == SB_VERT ? m_pVerticalScrollBar
													  : m_pHorizontalScrollBar;
	if (!pScrollBar)
		return 0;

	SCROLLINFO si = { 0 };
	pScrollBar->GetScrollInfo(&si);

	int nPage  = (int)si.nPage;
	int nLimit = CXTP_max(0, si.nMax - CXTP_max(nPage - 1, 0));

	return nLimit;
}

void CXTPMarkupScrollViewer::SetScrollPos(int nBar, int pos)
{
	int& nOffset = (nBar == SB_VERT) ? m_yOffset : m_xOffset;

	if (!m_bNeverArranged && !m_bArrangeDirty)
	{
		int nLimit = GetScrollLimit(nBar);
		if (pos > nLimit)
			pos = nLimit;
	}

	if (pos < 0)
		pos = 0;

	if (nOffset != pos)
	{
		nOffset = pos;
		InvalidateArrange();
	}
}

void CXTPMarkupScrollViewer::OnMouseWheel(CXTPMarkupMouseWheelEventArgs* e)
{
	SetScrollPos(SB_VERT,
				 e->m_nDelta < 0 ? GetScrollPos(SB_VERT) + 48 : GetScrollPos(SB_VERT) - 48);

	e->SetHandled();
}

XTPMarkupScrollBarVisibility CXTPMarkupScrollViewer::GetVerticalScrollBarVisibility() const
{
	CXTPMarkupEnum* pValue = MARKUP_STATICCAST(CXTPMarkupEnum,
											   GetValue(m_pVerticalScrollBarVisibilityProperty));
	return pValue ? (XTPMarkupScrollBarVisibility)(int)(*pValue) : xtpMarkupScrollBarVisible;
}

XTPMarkupScrollBarVisibility CXTPMarkupScrollViewer::GetHorizontalScrollBarVisibility() const
{
	CXTPMarkupEnum* pValue = MARKUP_STATICCAST(CXTPMarkupEnum,
											   GetValue(m_pHorizontalScrollBarVisibilityProperty));
	return pValue ? (XTPMarkupScrollBarVisibility)(int)(*pValue) : xtpMarkupScrollBarDisabled;
}

#include "Common/Base/Diagnostic/XTPBeginAfxMap.h"
BEGIN_DISPATCH_MAP(CXTPMarkupScrollViewer, CXTPMarkupContentControl)
	DISP_PROPERTY_EX_ID(CXTPMarkupScrollViewer, "VerticalOffset", 700, OleGetVerticalOffset,
						OleSetVerticalOffset, VT_I4)
	DISP_PROPERTY_EX_ID(CXTPMarkupScrollViewer, "HorizontalOffset", 701, OleGetHorizontalOffset,
						OleSetHorizontalOffset, VT_I4)
END_DISPATCH_MAP()
#include "Common/Base/Diagnostic/XTPEndAfxMap.h"

#ifdef _XTP_ACTIVEX

BEGIN_INTERFACE_MAP(CXTPMarkupScrollViewer, CXTPMarkupContentControl)
	INTERFACE_PART(CXTPMarkupScrollViewer, XTPDIID_MarkupScrollViewer, Dispatch)
END_INTERFACE_MAP()

IMPLEMENT_OLETYPELIB_EX(CXTPMarkupScrollViewer, XTPDIID_MarkupScrollViewer)
#endif

long CXTPMarkupScrollViewer::OleGetVerticalOffset()
{
	return m_yOffset;
}

long CXTPMarkupScrollViewer::OleGetHorizontalOffset()
{
	return m_xOffset;
}

void CXTPMarkupScrollViewer::OleSetVerticalOffset(int nOffset)
{
	SetScrollPos(SB_VERT, nOffset);
}

void CXTPMarkupScrollViewer::OleSetHorizontalOffset(int nOffset)
{
	SetScrollPos(SB_HORZ, nOffset);
}
