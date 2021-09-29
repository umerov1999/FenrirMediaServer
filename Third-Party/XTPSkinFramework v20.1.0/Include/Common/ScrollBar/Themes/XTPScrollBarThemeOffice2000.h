// XTPScrollBarThemeOffice2000.h: interface for the CXTPScrollBarCtrl class.
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
#if !defined(__XTPSCROLLBARTHEMEOFFICE2000_H__)
#	define __XTPSCROLLBARTHEMEOFFICE2000_H__
//}}AFX_CODEJOCK_PRIVATE

#	if _MSC_VER >= 1000
#		pragma once
#	endif // _MSC_VER >= 1000

#	include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

//===========================================================================
// Summary:
//     Scrollbar paint manager used to draw Office 200 like scrollbars.
// See Also: CXTPPaintManager::SetTheme
//===========================================================================
class _XTP_EXT_CLASS CXTPScrollBarThemeOffice2000 : public CXTPScrollBarThemeWindowsUx
{
protected:
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
};

#	include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif /*__XTPSCROLLBARTHEMEOFFICE2000_H__*/
