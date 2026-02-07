// XTPMarkupGdiPlusTransformationMatrix.h: declaration of CXTPMarkupGdiPlusTransformationMatrix
// class.
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
#if !defined(__XTPMARKUPGDIPLUSTRANSFORMATIONMATRIX_H__)
#	define __XTPMARKUPGDIPLUSTRANSFORMATIONMATRIX_H__
//}}AFX_CODEJOCK_PRIVATE

#	if _MSC_VER > 1000
#		pragma once
#	endif // _MSC_VER > 1000

#	include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

class CXTPGdiPlus;

//===========================================================================
// Summary:
//     Encapsulates GDI+ transformation matrix access.
//===========================================================================
class _XTP_EXT_CLASS CXTPMarkupGdiPlusTransformationMatrix : public CXTPMarkupTransformationMatrix
{
	DECLARE_DYNAMIC(CXTPMarkupGdiPlusTransformationMatrix);

public:
	//-------------------------------------------------------------------------
	// Summary:
	//     Constructs a GDI+ markup matrix.
	// Parameters:
	//    pReserved - Reserved. Not used.
	//    pMatrix   - A pointer to a native GDI+ matrix. The wrapper takes
	//                exclusive ownership of the native GDI+ matrix passed.
	//-------------------------------------------------------------------------
	CXTPMarkupGdiPlusTransformationMatrix(CXTPGdiPlus* pReserved, GpMatrix* pMatrix);

	//-------------------------------------------------------------------------
	// Summary:
	//     Cleans up resources held.
	//-------------------------------------------------------------------------
	~CXTPMarkupGdiPlusTransformationMatrix();

	//{{AFX_CODEJOCK_PRIVATE
	_XTP_DEPRECATED CXTPGdiPlus* GetGdiPlus();
	_XTP_DEPRECATED const CXTPGdiPlus* GetGdiPlus() const;
	//}}AFX_CODEJOCK_PRIVATE

	//-------------------------------------------------------------------------
	// Summary:
	//     Provides access to the native GDI+ matrix instance.
	// Returns:
	//     A pointer to the native GDI+ matrix instance.
	//-------------------------------------------------------------------------
	GpMatrix* GetMatrix();

	//-------------------------------------------------------------------------
	// Summary:
	//     Provides access to the native GDI+ matrix instance.
	// Returns:
	//     A pointer to the native GDI+ matrix instance.
	//-------------------------------------------------------------------------
	const GpMatrix* GetMatrix() const;

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Clones a matrix object.
	// Returns:
	//     A pointer to the cloned matrix. It is the responsibility
	//     of the caller to delete the returned object.
	//-----------------------------------------------------------------------
	virtual CXTPMarkupTransformationMatrix* Clone() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Adds offset to the current matrix.
	// Parameters:
	//     point - Offset to add.
	//-----------------------------------------------------------------------
	virtual void Offset(const POINT& offset);

	//-----------------------------------------------------------------------
	// Summary:
	//     Checks if a point is inside a rectangle with transformation applied.
	// Parameters:
	//     rect           - Rectangle in relative coordinates to test in.
	//     absoluteOffset - Absolute offset of the rectangle.
	//     point          - Absolute point to test.
	// Returns:
	//     TRUE if the point hits the specified rectangle.
	//-----------------------------------------------------------------------
	virtual BOOL PtInRect(const RECT& rect, const POINT& absoluteOffset, const POINT& point) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Computes the bounding rectangle after transformation.
	// Parameters:
	//     rect           - Rectangle in relative coordinates to transform.
	//     absoluteOffset - Absolute offset of the rectangle.
	// Returns:
	//     A CRect object containing the bounding rectangle after transformation.
	//-----------------------------------------------------------------------
	virtual CRect GetBoundRect(const RECT& rect, const POINT& absoluteOffset) const;

private:
	BOOL TransformRect(const RECT& rect, const POINT& absoluteOffset,
					   PointF (&transformedRectPoints)[4]) const;

private:
	GpMatrix* m_pMatrix; // Pointer to the native GDI+ matrix instance.
};

AFX_INLINE CXTPGdiPlus* CXTPMarkupGdiPlusTransformationMatrix::GetGdiPlus()
{
	return NULL;
}

AFX_INLINE const CXTPGdiPlus* CXTPMarkupGdiPlusTransformationMatrix::GetGdiPlus() const
{
	return NULL;
}

AFX_INLINE GpMatrix* CXTPMarkupGdiPlusTransformationMatrix::GetMatrix()
{
	return m_pMatrix;
}

AFX_INLINE const GpMatrix* CXTPMarkupGdiPlusTransformationMatrix::GetMatrix() const
{
	return m_pMatrix;
}

#	include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPMARKUPGDIPLUSTRANSFORMATIONMATRIX_H__)
