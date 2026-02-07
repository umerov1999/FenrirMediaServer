// CXTPScrollBarPaintManager.h: interface for the CXTPScrollBarPaintManager class.
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
#if !defined(__XTPSCROLLBARPAINTMANAGER_H__)
#	define __XTPSCROLLBARPAINTMANAGER_H__
//}}AFX_CODEJOCK_PRIVATE

#	if _MSC_VER > 1000
#		pragma once
#	endif // _MSC_VER > 1000

#	include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

class CXTPScrollBase;

//===========================================================================
// Summary:
//     CXTPScrollBarPaintManager is standalone class used to draw CXTPCommandBarScrollBarCtrl object
//===========================================================================
class _XTP_EXT_CLASS CXTPScrollBarPaintManager : public CObject
{
	DECLARE_DYNAMIC(CXTPScrollBarPaintManager);

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPScrollBarPaintManager object
	//-----------------------------------------------------------------------
	CXTPScrollBarPaintManager();

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to draw all scrollbar parts.
	// Parameters:
	//     pDC - Pointer to device context
	//     pScrollBar - ScrollBar to draw
	//-----------------------------------------------------------------------
	virtual void DrawScrollBar(CDC* pDC, CXTPScrollBase* pScrollBar);

	//-------------------------------------------------------------------------
	// Summary:
	//     Recalculates scrollbar metrics
	//-------------------------------------------------------------------------
	virtual void RefreshMetrics();

protected:
	struct XTPScrollMetricsV
	{
		BOOL bEnabled;
		CRect rcScrollBar;
		CRect rcArrowUp;
		CRect rcArrowDown;
		CRect rcTrack;
		CRect rcLowerTrack;
		CRect rcBtnTrack;
		CRect rcUpperTrack;
	};

	struct XTPScrollMetricsH
	{
		BOOL bEnabled;
		CRect rcScrollBar;
		CRect rcArrowLeft;
		CRect rcArrowRight;
		CRect rcTrack;
		CRect rcLowerTrack;
		CRect rcBtnTrack;
		CRect rcUpperTrack;
	};

	//-----------------------------------------------------------------------
	// Summary:
	//     Computes vertical scrollbar mertics.
	// Parameters:
	//     pScroll - Scroll bar object pointer
	//     pVScroll - Pointer to the output metrics information
	//-----------------------------------------------------------------------
	static void AFX_CDECL CalcVScroll(CXTPScrollBase* pScroll, XTPScrollMetricsV* pVScroll);

	//-----------------------------------------------------------------------
	// Summary:
	//     Computes horizontal scrollbar mertics.
	// Parameters:
	//     pScroll - Scroll bar object pointer
	//     pVScroll - Pointer to the output metrics information
	//-----------------------------------------------------------------------
	static void AFX_CDECL CalcHScroll(CXTPScrollBase* pScroll, XTPScrollMetricsH* pHScroll);

	//-----------------------------------------------------------------------
	// Summary:
	//     Draws verical scrollbar background only.
	// Parameters:
	//     pDC - Target device context pointer
	//     pScroll - Scroll bar object pointer
	//     pMetrics - Precomputed scrollbar metrics
	//-----------------------------------------------------------------------
	virtual void DrawBackground(CDC* pDC, CXTPScrollBase* pScroll, XTPScrollMetricsV* pMetrics);

	//-----------------------------------------------------------------------
	// Summary:
	//     Draws vertical scrollbar buttons only.
	// Parameters:
	//     pDC - Target device context pointer
	//     pScroll - Scroll bar object pointer
	//     pMetrics - Precomputed scrollbar metrics
	//-----------------------------------------------------------------------
	virtual void DrawButtons(CDC* pDC, CXTPScrollBase* pScroll, XTPScrollMetricsV* pMetrics);

	//-----------------------------------------------------------------------
	// Summary:
	//     Draws vertical scrollbar button arrows only.
	// Parameters:
	//     pDC - Target device context pointer
	//     pScroll - Scroll bar object pointer
	//     pMetrics - Precomputed scrollbar metrics
	//-----------------------------------------------------------------------
	virtual void DrawArrows(CDC* pDC, CXTPScrollBase* pScroll, XTPScrollMetricsV* pMetrics);

	//-----------------------------------------------------------------------
	// Summary:
	//     Draws vertical scrollbar trackers only.
	// Parameters:
	//     pDC - Target device context pointer
	//     pScroll - Scroll bar object pointer
	//     pMetrics - Precomputed scrollbar metrics
	//-----------------------------------------------------------------------
	virtual void DrawTracker(CDC* pDC, CXTPScrollBase* pScroll, XTPScrollMetricsV* pMetrics);

	//-----------------------------------------------------------------------
	// Summary:
	//     Draws vertical scrollbar thumb button only.
	// Parameters:
	//     pDC - Target device context pointer
	//     pScroll - Scroll bar object pointer
	//     pMetrics - Precomputed scrollbar metrics
	//-----------------------------------------------------------------------
	virtual void DrawThumbButton(CDC* pDC, CXTPScrollBase* pScroll, XTPScrollMetricsV* pMetrics);

	//-----------------------------------------------------------------------
	// Summary:
	//     Draws horizontal scrollbar background only.
	// Parameters:
	//     pDC - Target device context pointer
	//     pScroll - Scroll bar object pointer
	//     pMetrics - Precomputed scrollbar metrics
	//-----------------------------------------------------------------------
	virtual void DrawBackground(CDC* pDC, CXTPScrollBase* pScroll, XTPScrollMetricsH* pMetrics);

	//-----------------------------------------------------------------------
	// Summary:
	//     Draws horizontal scrollbar buttons only.
	// Parameters:
	//     pDC - Target device context pointer
	//     pScroll - Scroll bar object pointer
	//     pMetrics - Precomputed scrollbar metrics
	//-----------------------------------------------------------------------
	virtual void DrawButtons(CDC* pDC, CXTPScrollBase* pScroll, XTPScrollMetricsH* pMetrics);

	//-----------------------------------------------------------------------
	// Summary:
	//     Draws horizontal scrollbar button arrows only.
	// Parameters:
	//     pDC - Target device context pointer
	//     pScroll - Scroll bar object pointer
	//     pMetrics - Precomputed scrollbar metrics
	//-----------------------------------------------------------------------
	virtual void DrawArrows(CDC* pDC, CXTPScrollBase* pScroll, XTPScrollMetricsH* pMetrics);

	//-----------------------------------------------------------------------
	// Summary:
	//     Draws horizontal scrollbar trackers only.
	// Parameters:
	//     pDC - Target device context pointer
	//     pScroll - Scroll bar object pointer
	//     pMetrics - Precomputed scrollbar metrics
	//-----------------------------------------------------------------------
	virtual void DrawTracker(CDC* pDC, CXTPScrollBase* pScroll, XTPScrollMetricsH* pMetrics);

	//-----------------------------------------------------------------------
	// Summary:
	//     Draws horizontal scrollbar thumb button only.
	// Parameters:
	//     pDC - Target device context pointer
	//     pScroll - Scroll bar object pointer
	//     pMetrics - Precomputed scrollbar metrics
	//-----------------------------------------------------------------------
	virtual void DrawThumbButton(CDC* pDC, CXTPScrollBase* pScroll, XTPScrollMetricsH* pMetrics);

	//-----------------------------------------------------------------------
	// Summary:
	//     Draws scrollbar gripper only.
	// Parameters:
	//     pDC - Target device context pointer
	//     pScroll - Scroll bar object pointer
	//-----------------------------------------------------------------------
	virtual void DrawGripper(CDC* pDC, CXTPScrollBase* pScroll);

protected:
	enum XTPArrowGlyph
	{
		xtpArrowGlyphLeft,
		xtpArrowGlyphRight,
		xtpArrowGlyphUp,
		xtpArrowGlyphDown
	};

	//-----------------------------------------------------------------------
	// Summary:
	//     Draws scrollbar button arrow glyph.
	// Parameters:
	//     pDC - Target device context pointer.
	//     rcArrow - Target rectangle.
	//     glyph - Arrow glyph code, one of XTPArrowGlyph.
	//     bEnabled - Determines whether an enabled glyph should be drawn.
	//     clrText - Glyph color. If default COLORREF_NULL is used the enalbed glyph
	//               will be drawn in black color and disabled glyph will be drawn in
	//               COLOR_3DSHADOW.
	//-----------------------------------------------------------------------
	void DrawArrowGlyph(CDC* pDC, CRect rcArrow, XTPArrowGlyph glyph, BOOL bEnabled,
						COLORREF clrText = COLORREF_NULL);

public:
	int m_cxHScroll; // Width, in pixels, of the arrow bitmap on a horizontal scroll bar
	int m_cyHScroll; // Height, in pixels, of a horizontal scroll bar.

	int m_cxVScroll; // Width, in pixels, of a vertical scroll bar;
	int m_cyVScroll; // Height, in pixels, of the arrow bitmap on a vertical scroll bar.

	int m_cThumb; // Width of thumb button.

	COLORREF m_crBackPushed;   // Color when pushed.
	COLORREF m_crBackHilite;   // Color when border.
	COLORREF m_crBorderHilite; // Color of border when highlighted.
	COLORREF m_crBorder;	   // Color of border.
	COLORREF m_crBack;		   // Color of background.
};

#	include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif //#if !defined(__XTPSCROLLBARPAINTMANAGER_H__)
