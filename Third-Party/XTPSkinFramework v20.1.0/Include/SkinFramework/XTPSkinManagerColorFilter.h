// XTPSkinManagerColorFilter.h: interface for the CXTPSkinManagerColorFilter class.
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
#if !defined(__XTPSKINIMAGECOLORFILTER_H__)
#	define __XTPSKINIMAGECOLORFILTER_H__
//}}AFX_CODEJOCK_PRIVATE

#	if _MSC_VER > 1000
#		pragma once
#	endif // _MSC_VER > 1000

#	include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

//===========================================================================
// Summary:
//     CXTPSkinManagerColorFilter represents the parent class for all color
//     filters of skin manager. Skin manager calls the ApplyColorFilter method
//     for all filters of each color and image used to draw.
//===========================================================================
class _XTP_EXT_CLASS CXTPSkinManagerColorFilter
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Handles color filter object destruction.
	//-----------------------------------------------------------------------
	virtual ~CXTPSkinManagerColorFilter()
	{
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     Modifies the color of the skin manager before drawing the skin.
	// Parameters:
	//     clr - Reference to the color to filter.
	//-----------------------------------------------------------------------
	virtual void ApplyColorFilter(COLORREF& clr) = 0;
};

//{{AFX_CODEJOCK_PRIVATE

class _XTP_EXT_CLASS CXTPSkinManagerColorFilterColorize : public CXTPSkinManagerColorFilter
{
public:
	CXTPSkinManagerColorFilterColorize(BYTE hue, BYTE sat, float blend);

public:
	virtual void ApplyColorFilter(COLORREF& clr);

protected:
	BYTE m_hue;
	BYTE m_sat;
	float m_blend;
};

class _XTP_EXT_CLASS CXTPSkinManagerColorFilterShiftRGB : public CXTPSkinManagerColorFilter
{
public:
	CXTPSkinManagerColorFilterShiftRGB(long r, long g, long b);

public:
	virtual void ApplyColorFilter(COLORREF& clr);

protected:
	long m_r;
	long m_g;
	long m_b;
};
//}}AFX_CODEJOCK_PRIVATE

#	include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPSKINIMAGECOLORFILTER_H__)
