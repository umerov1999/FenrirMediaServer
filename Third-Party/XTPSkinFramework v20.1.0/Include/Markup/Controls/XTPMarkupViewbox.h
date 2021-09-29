// XTPMarkupViewbox.h: interface for the CXTPMarkupViewbox class.
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
#if !defined(__XTPMARKUPVIEWBOX_H__)
#	define __XTPMARKUPVIEWBOX_H__
//}}AFX_CODEJOCK_PRIVATE

#	if _MSC_VER > 1000
#		pragma once
#	endif // _MSC_VER > 1000

#	include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

class CXTPMarkupBrush;
class CXTPMarkupBuilder;

//===========================================================================
// Summary:
//     CXTPMarkupViewbox is a CXTPMarkupDecorator derived class.
//     It implements the Border XAML Tag.
//===========================================================================
class _XTP_EXT_CLASS CXTPMarkupViewbox : public CXTPMarkupDecorator
{
	DECLARE_MARKUPCLASS(CXTPMarkupViewbox);

protected:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPMarkupViewbox object.
	//-----------------------------------------------------------------------
	CXTPMarkupViewbox();

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPMarkupViewbox object, handles cleanup and deallocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPMarkupViewbox();

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Sets the stretch type of the image.
	// Parameters:
	//     stretch - New stretch type.
	//-----------------------------------------------------------------------
	void SetStretch(XTPMarkupStretch stretch);

	//-----------------------------------------------------------------------
	// Summary:
	//     Gets the current stretch type of the image.
	// Returns:
	//     The current stretch type.
	//-----------------------------------------------------------------------
	XTPMarkupStretch GetStretch() const;

protected:
	//{{AFX_CODEJOCK_PRIVATE
	// Implementation
	virtual CSize MeasureOverride(CXTPMarkupDrawingContext* pDC, CSize szAvailableSize);
	virtual CSize ArrangeOverride(CSize szFinalSize);

	//-----------------------------------------------------------------------
	// Summary: Applies render transformation if necessary.
	// Parameters: pContext - Drawing context to apply render transformation for.
	// Returns: A structure that optionally describes the previous and the new
	//          transformation matrices.
	//-----------------------------------------------------------------------
	virtual TransformationMatrices ApplyRenderTransform(CXTPMarkupDrawingContext* pContext);
	CXTPMarkupTransformationMatrix* ApplyTransform(const CXTPMarkupTransformationMatrix* pMatrix);

	BOOL HasElementSize();
	CSize GetElementSize();
	BOOL ChildHasElementSize();
	CSize ChildGetElementSize();

protected:
	float m_scaleX;
	float m_scaleY;
	float m_centerX;
	float m_centerY;

public:
	static CXTPMarkupDependencyProperty* m_pStretchProperty;
	//}}AFX_CODEJOCK_PRIVATE

	//{{AFX_CODEJOCK_PRIVATE
public:
	DECLARE_DISPATCH_MAP()
#	ifdef _XTP_ACTIVEX
	DECLARE_INTERFACE_MAP()
	DECLARE_OLETYPELIB_EX(CXTPMarkupViewbox);
#	endif

	afx_msg long OleGetStretch();
	afx_msg void OleSetStretch(long stretch);
	afx_msg LPDISPATCH OleGetChild();
	afx_msg void OleSetChild(LPDISPATCH lpDisp);

	//}}AFX_CODEJOCK_PRIVATE
};

#	include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPMARKUPVIEWBOX_H__)
