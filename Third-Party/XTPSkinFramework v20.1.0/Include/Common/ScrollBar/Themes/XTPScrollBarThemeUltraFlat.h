// XTPScrollBarThemeUltraFlat.h: interface for the CXTPScrollBarCtrl class.
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
#if !defined(__XTPSCROLLBARTHEMEULTRAFLAT_H__)
#	define __XTPSCROLLBARTHEMEULTRAFLAT_H__
//}}AFX_CODEJOCK_PRIVATE

#	if _MSC_VER >= 1000
#		pragma once
#	endif // _MSC_VER >= 1000

#	include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

//===========================================================================
// Summary:
//     Scrollbar paint manager used to draw ultra flat scrollbars.
// See Also: CXTPPaintManager::SetTheme
//===========================================================================
class _XTP_EXT_CLASS CXTPScrollBarThemeUltraFlat : public CXTPScrollBarThemeFlat
{
protected:
	//-------------------------------------------------------------------------
	// Summary:
	//     Recalculates scrollbar metrics
	//-------------------------------------------------------------------------
	virtual void RefreshMetrics();
};

#	include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif /*__XTPSCROLLBARTHEMEULTRAFLAT_H__*/
