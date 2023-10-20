// XTPScrollBarThemeWindowsUx.h: interface for the CXTPScrollBarCtrl class.
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
#if !defined(__XTPSCROLLBARTHEMEWINDOWSUX_H__)
#	define __XTPSCROLLBARTHEMEWINDOWSUX_H__
//}}AFX_CODEJOCK_PRIVATE

#	if _MSC_VER >= 1000
#		pragma once
#	endif // _MSC_VER >= 1000

#	include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

//===========================================================================
// Summary:
//     Scrollbar paint manager used to draw scrollbars that use current Windows theme.
// See Also: CXTPPaintManager::SetTheme
//===========================================================================
class _XTP_EXT_CLASS CXTPScrollBarThemeWindowsUx : public CXTPScrollBarThemeWindowsClassic
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPScrollBarThemeWindowsUx object
	//-----------------------------------------------------------------------
	CXTPScrollBarThemeWindowsUx();

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPScrollBarThemeWindowsUx object, handles cleanup and deallocation
	//-----------------------------------------------------------------------
	virtual ~CXTPScrollBarThemeWindowsUx();

protected:
	//-------------------------------------------------------------------------
	// Summary:
	//     Recalculates scrollbar metrics
	//-------------------------------------------------------------------------
	virtual void RefreshMetrics();

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
	//     Draws horizontal scrollbar buttons only.
	// Parameters:
	//     pDC - Target device context pointer
	//     pScroll - Scroll bar object pointer
	//     pMetrics - Precomputed scrollbar metrics
	//-----------------------------------------------------------------------
	virtual void DrawButtons(CDC* pDC, CXTPScrollBase* pScroll, XTPScrollMetricsH* pMetrics);

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

private:
	CXTPWinThemeWrapper* m_themeScrollBar; // Scrollbar theme.
};

#	include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif /*__XTPSCROLLBARTHEMEWINDOWSUX_H__*/
