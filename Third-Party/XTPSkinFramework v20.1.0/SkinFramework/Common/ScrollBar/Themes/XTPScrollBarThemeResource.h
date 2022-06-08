// XTPScrollBarThemeResource.h: interface for the CXTPScrollBarCtrl class.
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
#if !defined(__XTPSCROLLBARTHEMERESOURCE_H__)
#	define __XTPSCROLLBARTHEMERESOURCE_H__
//}}AFX_CODEJOCK_PRIVATE

#	if _MSC_VER >= 1000
#		pragma once
#	endif // _MSC_VER >= 1000

#	include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

//===========================================================================
// Summary:
//     Scrollbar paint manager used to draw scrollbars from the currently loaded
//     resource image data.
// See Also: CXTPPaintManager::SetTheme
//===========================================================================
class _XTP_EXT_CLASS CXTPScrollBarThemeResource : public CXTPScrollBarPaintManager
{
protected:
	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to draw all scrollbar parts.
	// Parameters:
	//     pDC - Pointer to device context
	//     pScrollBar - ScrollBar to draw
	//-----------------------------------------------------------------------
	virtual void DrawScrollBar(CDC* pDC, CXTPScrollBase* pScrollBar);

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

	//-----------------------------------------------------------------------
	// Summary:
	//     Loads image resource by its name.
	// Parameters:
	//     lpszImageFile - Image resource name to load.
	// Returns:
	//     A pointer to a loaded resource image or NULL if it's not found.
	//-----------------------------------------------------------------------
	CXTPResourceImage* LoadImage(LPCTSTR lpszImageFile);

private:
	struct DRAWING_STATE
	{
		BOOL bLight;
		BOOL nPressedHt;
		BOOL nHotHt;
		BOOL bEnabled;
		int nBtnTrackSize;
		int nBtnTrackPos;
		CRect rcArrowGripperSrc;
		CRect rcScroll;
		CRect rcArrowUp;
		CRect rcArrowDown;
		CRect rcArrowLeft;
		CRect rcArrowRight;

		CXTPResourceImage* pImageArrowGlyphs;
		CXTPResourceImage* pImageBackground;
	};

	DRAWING_STATE m_State;
};

#	include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif /*__XTPSCROLLBARTHEMERESOURCE_H__*/
