// XTPTheme.h:
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
#if !defined(_XTPTHEME_H_)
#	define _XTPTHEME_H_
//}}AFX_CODEJOCK_PRIVATE

#	if _MSC_VER > 1000
#		pragma once
#	endif // _MSC_VER > 1000

#	include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

class CXTPResourceImages;

//===========================================================================
// Summary: Default theme caption button color set.
//===========================================================================
class _XTP_EXT_CLASS CXTPColorSetCaptionButton
{
public:
	//-----------------------------------------------------------------------
	// Handles object destruction
	//-----------------------------------------------------------------------
	virtual ~CXTPColorSetCaptionButton()
	{
	}

	//-----------------------------------------------------------------------
	// Summary:    Called to update caption button theme colors.
	// Parameters: strSectionName - Name of the INI section that contains
	//                              caption button color definitions.
	//-----------------------------------------------------------------------
	virtual void RefreshMetrics(CString strSectionName);

	CXTPPaintManagerColor m_clrBackground; // Background color
	CXTPPaintManagerColor m_clrOutline;	   // Outline color
	CXTPPaintManagerColor m_clrFill;	   // Fill color
};

#	include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(_XTPTHEME_H_)
