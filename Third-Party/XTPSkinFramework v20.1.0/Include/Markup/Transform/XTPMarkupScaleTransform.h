// XTPMarkupScaleTransform.h: interface for the CXTPMarkupScaleTransform class.
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
#if !defined(__XTPMARKUPSCALETRANSFORM_H__)
#	define __XTPMARKUPSCALETRANSFORM_H__
//}}AFX_CODEJOCK_PRIVATE

#	if _MSC_VER > 1000
#		pragma once
#	endif // _MSC_VER > 1000

#	include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

class CXTPMarkupTransformationMatrix;
class CXTPMarkupDependencyProperty;

//=============================================================================
// Summary:
//     Default ScaleTransform implementation.
//=============================================================================
class _XTP_EXT_CLASS CXTPMarkupScaleTransform : public CXTPMarkupTransform
{
	DECLARE_MARKUPCLASS(CXTPMarkupScaleTransform);

public:
	//-------------------------------------------------------------------------
	// Summary:
	//     Gets the scale X value.
	// Returns:
	//     The scale X value.
	//-------------------------------------------------------------------------
	double GetScaleX() const;

	//-------------------------------------------------------------------------
	// Summary:
	//     Sets the scale X value.
	// Parameters:
	//     scaleX - New scale X value.
	//-------------------------------------------------------------------------
	void SetScaleX(double scaleX);

	//-------------------------------------------------------------------------
	// Summary:
	//     Gets the scale Y value.
	// Returns:
	//     The scale Y value.
	//-------------------------------------------------------------------------
	double GetScaleY() const;

	//-------------------------------------------------------------------------
	// Summary:
	//     Sets the scale Y value.
	// Parameters:
	//     scaleY - New scale Y value.
	//-------------------------------------------------------------------------
	void SetScaleY(double scaleY);

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
	afx_msg double OleGetScaleX();
	afx_msg void OleSetScaleX(double scaleX);
	afx_msg double OleGetScaleY();
	afx_msg void OleSetScaleY(double scaleY);
	afx_msg double OleGetCenterX();
	afx_msg void OleSetCenterX(double centerX);
	afx_msg double OleGetCenterY();
	afx_msg void OleSetCenterY(double centerY);

public:
	static CXTPMarkupDependencyProperty* m_pPropertyScaleX;	 // ScaleX property.
	static CXTPMarkupDependencyProperty* m_pPropertyScaleY;	 // ScaleY property.
	static CXTPMarkupDependencyProperty* m_pPropertyCenterX; // CenterX property.
	static CXTPMarkupDependencyProperty* m_pPropertyCenterY; // CenterY property.
};

#	include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPMARKUPSCALETRANSFORM_H__)
