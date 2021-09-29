// XTPMarkupGdiPlusScaleTransform.h: interface for the CXTPMarkupGdiPlusScaleTransform class.
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
#if !defined(__XTPMARKUPGDIPLUSSCALETRANSFORM_H__)
#	define __XTPMARKUPGDIPLUSSCALETRANSFORM_H__
//}}AFX_CODEJOCK_PRIVATE

#	if _MSC_VER > 1000
#		pragma once
#	endif // _MSC_VER > 1000

#	include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

class CXTPGdiPlus;

//=============================================================================
// Summary:
//     GDI+ ScaleTransform implementation.
//=============================================================================
class _XTP_EXT_CLASS CXTPMarkupGdiPlusScaleTransform
	: public CXTPMarkupGdiPlusTransformObject<CXTPMarkupScaleTransform>
{
	DECLARE_MARKUPCLASS(CXTPMarkupGdiPlusScaleTransform);

public:
	CXTPMarkupGdiPlusScaleTransform();

	//-------------------------------------------------------------------------
	// Summary:
	//     Performs GDI+ matrix scaling.
	// Parameters:
	//     pReserved - Reserved. Not used.
	//     pMatrix   - Native GDI+ matrix to transform instance pointer.
	// Returns:
	//     TRUE if successful.
	//-------------------------------------------------------------------------
	virtual BOOL TransformMatrix(const CXTPGdiPlus* pReserved, GpMatrix* pMatrix);
};

#	include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPMARKUPGDIPLUSSCALETRANSFORM_H__)
