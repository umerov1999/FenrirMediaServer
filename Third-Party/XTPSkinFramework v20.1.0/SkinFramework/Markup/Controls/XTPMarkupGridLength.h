// XTPMarkupGridLength.h: interface for the CXTPMarkupGridLength class.
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
#if !defined(__XTPMARKUPGRIDLENGTH_H__)
#	define __XTPMARKUPGRIDLENGTH_H__
//}}AFX_CODEJOCK_PRIVATE

#	if _MSC_VER > 1000
#		pragma once
#	endif // _MSC_VER > 1000

#	include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

class CXTPMarkupObject;
class CXTPMarkupBuilder;

//===========================================================================
// Summary:
//     CXTPMarkupGridLength is a class defined length of grid rows and columns.
//===========================================================================
class _XTP_EXT_CLASS CXTPMarkupGridLength : public CXTPMarkupObject
{
	DECLARE_MARKUPCLASS(CXTPMarkupGridLength);

public:
	//=======================================================================
	// Summary:
	//     Unit type of the length.
	//=======================================================================
	enum GridUnitType
	{
		unitTypeAuto,  // Automatically calculated length.
		unitTypePixel, // Length in pixels.
		unitTypeStar   // Length in percents.
	};

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPMarkupGridLength object.
	// Parameters:
	//     nValue - Length of the grid elements.
	//     type   - Unit type of the length; must be one of the values
	//              defined by the GridUnitType enumeration.
	//-----------------------------------------------------------------------
	CXTPMarkupGridLength(double nValue = 0, GridUnitType type = unitTypePixel);

	//-----------------------------------------------------------------------
	// Summary:
	//     Gets the unit type of the length.
	// Returns:
	//     The unit type of the length; returns one of the values
	//     defined by the GridUnitType enumeration.
	//     unitTypePixel will be returned if the length is calculated in pixels,
	//     unitTypeStar will be returned if the length is calculated in percents.
	//-----------------------------------------------------------------------
	GridUnitType GetUnitType() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Gets the value of the length.
	// Returns:
	//     The value of the length, in pixels or percents.
	//-----------------------------------------------------------------------
	double GetValue() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines if the length is calculated in percents (unitTypeStar).
	// Returns:
	//     TRUE if the unit type of the length is unitTypeStar.
	//-----------------------------------------------------------------------
	BOOL IsStar() const
	{
		return m_type == unitTypeStar;
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines if the length is calculated automatically (unitTypeAuto).
	// Returns:
	//     TRUE if the unit type of the length is unitTypeAuto.
	//-----------------------------------------------------------------------
	BOOL IsAuto() const
	{
		return m_type == unitTypeAuto;
	}

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Helper method used to convert string to length.
	// Parameters:
	//     pBuilder - Pointer to a builder object.
	//     pObject  - String representation of the length.
	// Returns
	//     A pointer to a new CXTPMarkupGridLength object.
	//-----------------------------------------------------------------------
	CXTPMarkupObject* ConvertFrom(CXTPMarkupBuilder* pBuilder, CXTPMarkupObject* pObject) const;

public:
	double m_nValue;	 // Value.
	GridUnitType m_type; // Unit type.
};

AFX_INLINE CXTPMarkupGridLength::GridUnitType CXTPMarkupGridLength::GetUnitType() const
{
	return m_type;
};

AFX_INLINE double CXTPMarkupGridLength::GetValue() const
{
	return m_nValue;
}

#	include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPMARKUPGRIDLENGTH_H__)
