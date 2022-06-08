// XTPScrollBarThemeOffice2013.h
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

//{{AFX_CODEJOCK_PRIVATE
#if !defined(__XTPSCROLLBARTHEMEOFFICE2013_H__)
#	define __XTPSCROLLBARTHEMEOFFICE2013_H__
//}}AFX_CODEJOCK_PRIVATE

#	if _MSC_VER >= 1000
#		pragma once
#	endif // _MSC_VER >= 1000

#	include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

//-----------------------------------------------------------------------
// Summary:
//    Used by paint manager to retrieve color settings from the loaded
//    resource INI file.
//-----------------------------------------------------------------------
struct XTP_SCROLLBARINICOLORS
{
	COLORREF clrBack;		  // Scrollbar background color.
	COLORREF clrScrollBack;	  // Scrollbar button background color.
	COLORREF clrScrollArrow;  // Scrollbar button arrow color.
	COLORREF clrScrollBorder; // Scrollbar button border color.
	COLORREF clrThumbBack;	  // Scrollbar thumb background color.
	COLORREF clrThumbBorder;  // Scrollbar thumb border color.
};

//-----------------------------------------------------------------------
// Summary:
//    Used by paint manager to retrieve the current state color settings
//    when drawing scrollbars.
//-----------------------------------------------------------------------
struct XTP_SCROLLBARDRAWCOLORS : public XTP_SCROLLBARINICOLORS
{
	COLORREF clrScrollDownBack;	  // Scrollbar down / right button background color.
	COLORREF clrScrollDownArrow;  // Scrollbar down / right button arrow color.
	COLORREF clrScrollDownBorder; // Scrollbar down / right button border color.
};

//===========================================================================
// Summary:
//     The CXTPScrollBarThemeOffice2013 class is used to enable a Gallery Office 2013 style theme
//     for Command Bars.
// See Also: CXTPPaintManager::SetTheme
//===========================================================================
class _XTP_EXT_CLASS CXTPScrollBarThemeOffice2013 : public CXTPScrollBarPaintManager
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//    Constructs a CXTPScrollBarThemeOffice2013 object.
	//-----------------------------------------------------------------------
	CXTPScrollBarThemeOffice2013();

	//-----------------------------------------------------------------------
	// Summary:
	//    Destructor, handles any cleanup / deallocation necessary.
	//-----------------------------------------------------------------------
	virtual ~CXTPScrollBarThemeOffice2013();

	//-----------------------------------------------------------------------
	// Summary:
	//    Used by the paint manager to retreive state colors used when
	//    drawing the scrollbar.
	// Parameters:
	//    sbc - Reference to a XTP_SCROLLBARDRAWCOLORS that recives state color values.
	//    nHitSelected - Contains selected hit test index value, see remarks for more details.
	//    nHitPressed - Contains pressed hit test index value, see remarks for more details.
	//    bEnabled - TRUE if the scrollbar is enabled, otherwise FALSE.
	// Remarks:
	//    The nHitSelected and nHitPressed values contain an index id which identifies
	//    the scroll button and thumb track state. Possible values are XTP_HTSCROLLUP,
	//    XTP_HTSCROLLLEFT, XTP_HTSCROLLDOWN, XTP_HTSCROLLRIGHT and XTP_HTSCROLLTHUMB. So
	//    for example if nHitPressed equals XTP_HTSCROLLDOWN, that would indicate the
	//    down scroll arrow has been pressed.
	// See Also:
	//    XTP_SCROLLBARDRAWCOLORS
	//-----------------------------------------------------------------------
	virtual void GetScrollBarColors(XTP_SCROLLBARDRAWCOLORS& sbc, int nHitSelected, int nHitPressed,
									BOOL bEnabled);

protected:
	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to draw all scrollbar parts.
	// Parameters:
	//     pDC - Pointer to device context
	//     pScrollBar - ScrollBar to draw
	//-----------------------------------------------------------------------
	virtual void DrawScrollBar(CDC* pDC, CXTPScrollBase* pGallery);

	//-------------------------------------------------------------------------
	// Summary:
	//     Recalculates scrollbar metrics
	//-------------------------------------------------------------------------
	virtual void RefreshMetrics();

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

	XTP_SCROLLBARINICOLORS m_clrNormal;	  // Holds scrollbar default state colors.
	XTP_SCROLLBARINICOLORS m_clrPressed;  // Holds scrollbar pressed state colors.
	XTP_SCROLLBARINICOLORS m_clrSelected; // Holds scrollbar selected state colors.
	XTP_SCROLLBARINICOLORS m_clrDisabled; // Holds scrollbar disabled state colors.
	XTP_SCROLLBARDRAWCOLORS m_clrCurrent; // Holds currently selected scrollbar colors.
};

#	include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPSCROLLBARTHEMEOFFICE2013_H__)
