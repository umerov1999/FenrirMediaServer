// XTPMarkupSkewTransform.h: interface for the CXTPMarkupSkewTransform class.
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
#if !defined(__XTPMARKUPSKEWTRANSFORM_H__)
#	define __XTPMARKUPSKEWTRANSFORM_H__
//}}AFX_CODEJOCK_PRIVATE

#	if _MSC_VER > 1000
#		pragma once
#	endif // _MSC_VER > 1000

#	include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

class CXTPMarkupTransformationMatrix;
class CXTPMarkupDependencyProperty;

//=============================================================================
// Summary:
//     Default SkewTransform implementation.
//=============================================================================
class _XTP_EXT_CLASS CXTPMarkupSkewTransform : public CXTPMarkupTransform
{
	DECLARE_MARKUPCLASS(CXTPMarkupSkewTransform);

public:
	//-------------------------------------------------------------------------
	// Summary:
	//     Gets the angle X value.
	// Returns:
	//     The angle X value.
	//-------------------------------------------------------------------------
	double GetAngleX() const;

	//-------------------------------------------------------------------------
	// Summary:
	//     Sets the angle X value.
	// Parameters:
	//     angleX - New angle X value.
	//-------------------------------------------------------------------------
	void SetAngleX(double angleX);

	//-------------------------------------------------------------------------
	// Summary:
	//     Gets the angle Y value.
	// Returns:
	//     The angle Y value.
	//-------------------------------------------------------------------------
	double GetAngleY() const;

	//-------------------------------------------------------------------------
	// Summary:
	//     Sets the angle Y value.
	// Parameters:
	//     angleY - New angle Y value.
	//-------------------------------------------------------------------------
	void SetAngleY(double angleY);

	//-------------------------------------------------------------------------
	// Summary:
	//     Gets the center X value.
	// Returns:
	//     The center X value.
	//-------------------------------------------------------------------------
	double GetCenterX() const;

	//-------------------------------------------------------------------------
	// Summary:
	//     Sets the center X value.
	// Paramteres:
	//     centerX - New center X value.
	//-------------------------------------------------------------------------
	void SetCenterX(double centerX);

	//-------------------------------------------------------------------------
	// Summary:
	//     Gets the center Y value.
	// Returns:
	//     The center Y value.
	//-------------------------------------------------------------------------
	double GetCenterY() const;

	//-------------------------------------------------------------------------
	// Summary:
	//     Sets the center Y value.
	// Returns:
	//     centerY - New center Y value.
	//-------------------------------------------------------------------------
	void SetCenterY(double centerY);

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
	afx_msg double OleGetAngleX();
	afx_msg void OleSetAngleX(double angleX);
	afx_msg double OleGetAngleY();
	afx_msg void OleSetAngleY(double angleY);
	afx_msg double OleGetCenterX();
	afx_msg void OleSetCenterX(double centerX);
	afx_msg double OleGetCenterY();
	afx_msg void OleSetCenterY(double centerY);

public:
	static CXTPMarkupDependencyProperty* m_pPropertyAngleX;	 // AngleX property.
	static CXTPMarkupDependencyProperty* m_pPropertyAngleY;	 // AngleY property.
	static CXTPMarkupDependencyProperty* m_pPropertyCenterX; // CenterX property.
	static CXTPMarkupDependencyProperty* m_pPropertyCenterY; // CenterY property.
};

#	include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPMARKUPSKEWTRANSFORM_H__)
