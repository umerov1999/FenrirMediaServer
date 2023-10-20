// XTPMarkupTranslateTransform.h: interface for the CXTPMarkupTranslateTransform class.
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
#if !defined(__XTPMARKUPTRANSLATETRANSFORM_H__)
#	define __XTPMARKUPTRANSLATETRANSFORM_H__
//}}AFX_CODEJOCK_PRIVATE

#	if _MSC_VER > 1000
#		pragma once
#	endif // _MSC_VER > 1000

#	include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

class CXTPMarkupTransformationMatrix;
class CXTPMarkupDependencyProperty;

//===========================================================================
// Summary:
//     Default TranslateTransform implementation.
//===========================================================================
class _XTP_EXT_CLASS CXTPMarkupTranslateTransform : public CXTPMarkupTransform
{
	DECLARE_MARKUPCLASS(CXTPMarkupTranslateTransform);

public:
	//-------------------------------------------------------------------------
	// Summary:
	//     Gets the X value.
	// Returns:
	//     The X value.
	//-------------------------------------------------------------------------
	double GetX() const;

	//-------------------------------------------------------------------------
	// Summary:
	//     Sets the X value.
	// Parameters:
	//     x - New X value.
	//-------------------------------------------------------------------------
	void SetX(double x);

	//-------------------------------------------------------------------------
	// Summary:
	//     Gets the Y value.
	// Returns:
	//     The Y value.
	//-------------------------------------------------------------------------
	double GetY() const;

	//-------------------------------------------------------------------------
	// Summary:
	//     Sets the Y value.
	// Parameters:
	//     y - New Y value.
	//-------------------------------------------------------------------------
	void SetY(double y);

	//-------------------------------------------------------------------------
	// Summary:
	//     Does nothing in default implementation.
	// Parameters:
	//     pMatrix - Pointer to a markup transformation matrix to be transformed.
	// Returns:
	//     NULL in default implementation.
	//-------------------------------------------------------------------------
	virtual CXTPMarkupTransformationMatrix*
		ApplyTransform(const CXTPMarkupTransformationMatrix* pMatrix);

private:
	DECLARE_DISPATCH_MAP();
	afx_msg double OleGetX();
	afx_msg void OleSetX(double x);
	afx_msg double OleGetY();
	afx_msg void OleSetY(double y);

public:
	static CXTPMarkupDependencyProperty* m_pPropertyX; // X property.
	static CXTPMarkupDependencyProperty* m_pPropertyY; // Y property.
};

#	include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPMARKUPTRANSLATETRANSFORM_H__)
