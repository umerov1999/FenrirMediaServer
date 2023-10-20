// XTPMarkupThickness.h: interface for the CXTPMarkupThickness class.
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
#if !defined(__XTPMARKUPTHICKNESS_H__)
#	define __XTPMARKUPTHICKNESS_H__
//}}AFX_CODEJOCK_PRIVATE

#	if _MSC_VER > 1000
#		pragma once
#	endif // _MSC_VER > 1000

#	include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

class CXTPMarkupObject;
class CXTPMarkupBuilder;

//===========================================================================
// Summary:
//     The CXTPMarkupThickness class describes the thickness of the
//     left, top, right, and bottom sides of a rectangle.
//===========================================================================
class _XTP_EXT_CLASS CXTPMarkupThickness : public CXTPMarkupObject
{
	DECLARE_MARKUPCLASS(CXTPMarkupThickness);

	struct DefaultThicknessTag
	{
	};

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPMarkupThickness object and sets
	//     the left, top, right, and bottom thickness to 0.
	//-----------------------------------------------------------------------
	CXTPMarkupThickness();

	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPMarkupThickness object and sets
	//     the left, top, right, and bottom thickness.
	// Parameters:
	//     other - The left, top, right, and bottom thickness.
	//-----------------------------------------------------------------------
	CXTPMarkupThickness(const CXTPMarkupThickness& other);

	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPMarkupThickness object and sets
	//     the left, top, right, and bottom thickness.
	// Parameters:
	//     nThickness - The left, top, right, and bottom thickness.
	//-----------------------------------------------------------------------
	CXTPMarkupThickness(long nThickness);

	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPMarkupThickness object and sets
	//     the left/right and top/bottom thickness.
	// Parameters:
	//     nXThickness - The left and right thickness.
	//     nYThickness - The top and bottom thickness.
	//-----------------------------------------------------------------------
	CXTPMarkupThickness(long nXThickness, long nYThickness);

	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPMarkupThickness object and sets
	//     the left, top, right, and bottom thickness.
	// Parameters:
	//     nLeft   - The left thickness.
	//     nTop    - The top thickness.
	//     nRight  - The right thickness.
	//     nBottom - The bottom thickness.
	//-----------------------------------------------------------------------
	CXTPMarkupThickness(long nLeft, long nTop, long nRight, long nBottom);

public:
	static CXTPMarkupThickness* AFX_CDECL CreateValue();

	CSize GetSize() const;

	BOOL IsZero() const;

	static CRect AFX_CDECL HelperDeflateRect(CRect rt, CXTPMarkupThickness* thick);

	CXTPMarkupObject* ConvertFrom(CXTPMarkupBuilder* pBuilder, CXTPMarkupObject* pObject) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Gets the left thickness.
	// Returns:
	//     The left thickness.
	//-----------------------------------------------------------------------
	long GetLeft() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets the left thickness.
	// Parameters:
	//     nThickness - New left thickness.
	//-----------------------------------------------------------------------
	void SetLeft(long nThickness);

	//-----------------------------------------------------------------------
	// Summary:
	//     Gets the top thickness.
	// Returns:
	//     The top thickness.
	//-----------------------------------------------------------------------
	long GetTop() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Gets the right thickness.
	// Returns:
	//     The right thickness.
	//-----------------------------------------------------------------------
	long GetRight() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Gets the bottom thickness.
	// Returns:
	//     The bottom thickness.
	//-----------------------------------------------------------------------
	long GetBottom() const;

	virtual BOOL IsEqual(const CXTPMarkupObject* pObject) const;

	//{{AFX_CODEJOCK_PRIVATE
public:
	DECLARE_DISPATCH_MAP()
#	ifdef _XTP_ACTIVEX
	DECLARE_INTERFACE_MAP()
	DECLARE_OLETYPELIB_EX(CXTPMarkupThickness);
#	endif

	//}}AFX_CODEJOCK_PRIVATE

protected:
	long m_left;
	long m_top;
	long m_right;
	long m_bottom;
};

AFX_INLINE long CXTPMarkupThickness::GetLeft() const
{
	return m_left;
}
AFX_INLINE long CXTPMarkupThickness::GetTop() const
{
	return m_top;
}
AFX_INLINE long CXTPMarkupThickness::GetRight() const
{
	return m_right;
}
AFX_INLINE long CXTPMarkupThickness::GetBottom() const
{
	return m_bottom;
}
AFX_INLINE CSize CXTPMarkupThickness::GetSize() const
{
	return CSize(m_left + m_right, m_top + m_bottom);
}
AFX_INLINE BOOL CXTPMarkupThickness::IsZero() const
{
	return m_left == 0 && m_right == 0 && m_top == 0 && m_bottom == 0;
}

#	include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // #if !defined(__XTPMARKUPTHICKNESS_H__)
