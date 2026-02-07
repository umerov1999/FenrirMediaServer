// XTPMarkupMatrixTransform.h: interface for the CXTPMarkupMatrixTransform class.
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
#if !defined(__XTPMARKUPMATRIXTRANSFORM_H__)
#	define __XTPMARKUPMATRIXTRANSFORM_H__
//}}AFX_CODEJOCK_PRIVATE

#	if _MSC_VER > 1000
#		pragma once
#	endif // _MSC_VER > 1000

#	include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

class CXTPMarkupTransformationMatrix;
class CXTPMarkupDependencyProperty;

//=============================================================================
// Summary:
//     Default MatrixTransform implementation.
//=============================================================================
class _XTP_EXT_CLASS CXTPMarkupMatrixTransform : public CXTPMarkupTransform
{
	DECLARE_MARKUPCLASS(CXTPMarkupMatrixTransform);

public:
	CXTPMarkupDoubleCollection* GetMatrix() const;

	void SetMatrix(CXTPMarkupDoubleCollection* pMatrix);

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
	afx_msg LPDISPATCH OleGetMatrix();
	afx_msg void OleSetMatrix(LPDISPATCH lpValue);

public:
	static CXTPMarkupDependencyProperty* m_pPropertyMatrix;
};

#	include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPMARKUPMATRIXTRANSFORM_H__)
