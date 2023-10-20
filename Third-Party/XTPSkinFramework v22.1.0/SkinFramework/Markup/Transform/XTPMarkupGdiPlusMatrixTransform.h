// XTPMarkupGdiPlusMatrixTransform.h: interface for the CXTPMarkupGdiPlusMatrixTransform class.
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
#if !defined(__XTPMARKUPGDIPLUSMATRIXTRANSFORM_H__)
#	define __XTPMARKUPGDIPLUSMATRIXTRANSFORM_H__
//}}AFX_CODEJOCK_PRIVATE

#	if _MSC_VER > 1000
#		pragma once
#	endif // _MSC_VER > 1000

#	include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

class CXTPGdiPlus;

//=============================================================================
// Summary:
//     GDI+ MatrixTransform implementation.
//=============================================================================
class _XTP_EXT_CLASS CXTPMarkupGdiPlusMatrixTransform
	: public CXTPMarkupGdiPlusTransformObject<CXTPMarkupMatrixTransform>
{
	DECLARE_MARKUPCLASS(CXTPMarkupGdiPlusMatrixTransform);

public:
	//-------------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPMarkupGdiPlusMatrixTransform object.
	//-------------------------------------------------------------------------
	CXTPMarkupGdiPlusMatrixTransform();

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
#endif // !defined(__XTPMARKUPGDIPLUSMATRIXTRANSFORM_H__)
