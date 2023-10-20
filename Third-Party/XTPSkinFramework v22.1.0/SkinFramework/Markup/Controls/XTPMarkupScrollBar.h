// XTPMarkupScrollBar.h: interface for the CXTPMarkupScrollBar class.
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

//{{AFX_CODEJOCK_PRIVATE
#if !defined(__XTPMARKUPSCROLLBAR_H__)
#	define __XTPMARKUPSCROLLBAR_H__
//}}AFX_CODEJOCK_PRIVATE

#	if _MSC_VER > 1000
#		pragma once
#	endif // _MSC_VER > 1000

#	include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

//{{AFX_CODEJOCK_PRIVATE
struct XTP_SCROLLBAR_POSINFO;
struct XTP_SCROLLBAR_TRACKINFO;
//}}AFX_CODEJOCK_PRIVATE

class CXTPMarkupMouseEventArgs;
class CXTPMarkupDrawingContext;
class CXTPMarkupMouseButtonEventArgs;

class _XTP_EXT_CLASS CXTPMarkupScrollBar : public CXTPMarkupControl
{
	DECLARE_MARKUPCLASS(CXTPMarkupScrollBar);

protected:
	CXTPMarkupScrollBar();

public:
	void SetOrientation(XTPMarkupOrientation orientation);
	void SetScrollInfo(SCROLLINFO* pScrollInfo);
	void GetScrollInfo(SCROLLINFO* pScrollInfo);

protected:
	CSize MeasureOverride(CXTPMarkupDrawingContext* pDC, CSize szAvailableSize);
	CSize ArrangeOverride(CSize szFinalSize);
	void OnRender(CXTPMarkupDrawingContext* pDC);

	void CalcScrollBarInfo(LPRECT lprc, XTP_SCROLLBAR_POSINFO* pSBInfo, SCROLLINFO* pSI);

	void OnMouseLeave(CXTPMarkupMouseEventArgs* e);
	void OnMouseEnter(CXTPMarkupMouseEventArgs* e);
	void OnMouseMove(CXTPMarkupMouseEventArgs* e);

	void OnMouseLeftButtonUp(CXTPMarkupMouseButtonEventArgs* e);
	void OnMouseLeftButtonDown(CXTPMarkupMouseButtonEventArgs* e);

	BOOL IsEnabled() const;

private:
	void PerformTrackInit(HWND hWnd, CPoint point, XTP_SCROLLBAR_POSINFO* pSBInfo, BOOL bDirect);
	void CalcTrackDragRect(XTP_SCROLLBAR_TRACKINFO* pSBTrack) const;

	void DoScroll(int cmd, int pos);
	int HitTestScrollBar(POINT pt) const;
	void EndScroll(BOOL fCancel);
	void RedrawScrollBar();
	void MoveThumb(int px);
	void TrackThumb(UINT message, CPoint pt);
	void TrackBox(UINT message, CPoint point);
	void ContScroll();
	void ScreenToClient(HWND hWnd, LPPOINT lpPoint);

protected:
	XTPMarkupOrientation m_orientation;

	XTP_SCROLLBAR_POSINFO m_spi;		 // Scrollbar position.
	XTP_SCROLLBAR_TRACKINFO* m_pSBTrack; // Scrollbar tracking
	SCROLLINFO m_si;
};

AFX_INLINE void CXTPMarkupScrollBar::SetOrientation(XTPMarkupOrientation orientation)
{
	m_orientation = orientation;
}

#	include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPMARKUPSCROLLBAR_H__)
