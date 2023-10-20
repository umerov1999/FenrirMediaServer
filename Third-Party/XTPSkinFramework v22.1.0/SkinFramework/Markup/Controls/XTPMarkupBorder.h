// XTPMarkupBorder.h: interface for the CXTPMarkupBorder class.
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
#if !defined(__XTPMARKUPBORDER_H__)
#	define __XTPMARKUPBORDER_H__
//}}AFX_CODEJOCK_PRIVATE

#	if _MSC_VER > 1000
#		pragma once
#	endif // _MSC_VER > 1000

#	include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

class CXTPMarkupBrush;
class CXTPMarkupBuilder;

//===========================================================================
// Summary:
//     CXTPMarkupBorder is a CXTPMarkupDecorator derived class.
//     It implements the Border XAML Tag.
//===========================================================================
class _XTP_EXT_CLASS CXTPMarkupBorder : public CXTPMarkupDecorator
{
	DECLARE_MARKUPCLASS(CXTPMarkupBorder);

protected:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPMarkupBorder object.
	//-----------------------------------------------------------------------
	CXTPMarkupBorder();

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPMarkupBorder object, handles cleanup and deallocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPMarkupBorder();

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Sets a value that indicates the thickness of the padding space
	//     between the boundaries of the content area and the content
	//     displayed by a border.
	// Parameters
	//     nLeft    - Left space.
	//     nTop     - Top space.
	//     nRight   - Right space.
	//     nBottom  - Bottom space.
	//     nPadding - All borders.
	//-----------------------------------------------------------------------
	void SetPadding(int nLeft, int nTop, int nRight, int nBottom);
	void SetPadding(int nPadding); // <Combine CXTPMarkupBorder::SetPadding@int@int@int@int>

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves a value that indicates the thickness of the padding space
	//     between the boundaries of the content area and the content
	//     displayed by a border.
	// Returns:
	//     A pointer to a CXTPMarkupThickness object that indicates the thickness
	//     of the padding space between the boundaries of the content area
	//     and the content displayed by a border.
	// See Also:
	//     SetPadding
	//-----------------------------------------------------------------------
	CXTPMarkupThickness* GetPadding() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets the thickness of the border.
	// Parameters:
	//     nLeft            - Left border width.
	//     nTop             - Top border width.
	//     nRight           - Right border width.
	//     nBottom          - Bottom border width.
	//     nBorderThickness - Thickness for all borders.
	//-----------------------------------------------------------------------
	void SetBorderThickness(int nLeft, int nTop, int nRight, int nBottom);
	void SetBorderThickness(
		int nBorderThickness); // <Combine CXTPMarkupBorder::SetBorderThickness@int@int@int@int>

	//-----------------------------------------------------------------------
	// Summary:
	//     Gets the thickness of the border.
	// Returns:
	//     A pointer to a CXTPMarkupThickness object containing border widths.
	// See Also:
	//     SetBorderThickness
	//-----------------------------------------------------------------------
	CXTPMarkupThickness* GetBorderThickness() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets the background brush for the border object.
	// Parameters:
	//     brush - New background brush for the border object.
	//-----------------------------------------------------------------------
	void SetBackground(CXTPMarkupBrush* brush);

	//-----------------------------------------------------------------------
	// Summary:
	//     Gets the background brush of the border object.
	// Returns:
	//     A pointer to the background brush of the border object.
	//-----------------------------------------------------------------------
	CXTPMarkupBrush* GetBackground() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets the border brush for the border object.
	// Parameters:
	//     brush - New border brush for the border object.
	//-----------------------------------------------------------------------
	void SetBorderBrush(CXTPMarkupBrush* brush);

	//-----------------------------------------------------------------------
	// Summary:
	//     Gets the border brush of the border object.
	// Returns:
	//     A pointer to the border brush of the border object.
	//-----------------------------------------------------------------------
	CXTPMarkupBrush* GetBorderBrush() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets the corner radius for the border.
	// Parameters
	//     nCornerRadius - New corner radius for the border.
	//-----------------------------------------------------------------------
	void SetCornerRadius(int nCornerRadius);

	//-----------------------------------------------------------------------
	// Summary:
	//     Gets the corner radius of the border.
	// Returns:
	//     A pointer to a CXTPMarkupThickness object containing the corner
	//     radius of the border.
	// See Also:
	//     SetCornerRadius
	//-----------------------------------------------------------------------
	CXTPMarkupThickness* GetCornerRadius() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Gets the total horizontal and vertical size of the padding,
	//     border, and margin.
	// Returns:
	//     A CSize object containing the total horizontal and vertical
	//     size of the padding, border, and margin.
	//-----------------------------------------------------------------------
	CSize GetSize() const;

protected:
	//{{AFX_CODEJOCK_PRIVATE
	// Implementation
	virtual CSize MeasureOverride(CXTPMarkupDrawingContext* pDC, CSize szAvailableSize);
	virtual CSize ArrangeOverride(CSize szFinalSize);
	virtual void OnRender(CXTPMarkupDrawingContext* drawingContext);
	virtual CXTPMarkupInputElement* InputHitTestOverride(CPoint point) const;

public:
	static CXTPMarkupDependencyProperty* m_pBackgroundProperty;
	static CXTPMarkupDependencyProperty* m_pBorderThicknessProperty;
	static CXTPMarkupDependencyProperty* m_pPaddingProperty;
	static CXTPMarkupDependencyProperty* m_pBorderBrushProperty;
	static CXTPMarkupDependencyProperty* m_pCornerRadiusProperty;
	//}}AFX_CODEJOCK_PRIVATE

	//{{AFX_CODEJOCK_PRIVATE
public:
	DECLARE_DISPATCH_MAP()
#	ifdef _XTP_ACTIVEX
	DECLARE_INTERFACE_MAP()
	DECLARE_OLETYPELIB_EX(CXTPMarkupBorder);
#	endif

	afx_msg LPDISPATCH OleGetBackground();
	afx_msg void OleSetBackground(LPDISPATCH lpBackground);
	afx_msg LPDISPATCH OleGetBorderBrush();
	afx_msg void OleSetBorderBrush(LPDISPATCH lpBackground);
	afx_msg LPDISPATCH OleGetBorderThickness();
	afx_msg void OleSetBorderThickness(LPDISPATCH lpThickness);
	afx_msg LPDISPATCH OleGetPadding();
	afx_msg void OleSetPadding(LPDISPATCH lpThickness);
	afx_msg LPDISPATCH OleGetCornerRadius();
	afx_msg void OleSetCornerRadius(LPDISPATCH lpThickness);
	afx_msg LPDISPATCH OleGetChild();
	afx_msg void OleSetChild(LPDISPATCH lpDisp);

	//}}AFX_CODEJOCK_PRIVATE
};

#	include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPMARKUPBORDER_H__)
