// XTPMarkupCheckBox.h: interface for the CXTPMarkupCheckBox class.
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
#if !defined(__XTPMARKUPCHECKBOX_H__)
#	define __XTPMARKUPCHECKBOX_H__
//}}AFX_CODEJOCK_PRIVATE

#	if _MSC_VER > 1000
#		pragma once
#	endif // _MSC_VER > 1000

#	include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

class CXTPMarkupDrawingContext;

//===========================================================================
// Summary:
//     CXTPMarkupCheckBox is a CXTPMarkupToggleButton derived class.
//     It implements the CheckBox XAML Tag.
//===========================================================================
class _XTP_EXT_CLASS CXTPMarkupCheckBox : public CXTPMarkupToggleButton
{
	DECLARE_MARKUPCLASS(CXTPMarkupCheckBox);

protected:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPMarkupCheckBox object.
	//-----------------------------------------------------------------------
	CXTPMarkupCheckBox();

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPMarkupCheckBox object, handles cleanup and deallocation.
	//-----------------------------------------------------------------------
	~CXTPMarkupCheckBox();

protected:
	//{{AFX_CODEJOCK_PRIVATE
	// Implementation
	void OnRender(CXTPMarkupDrawingContext* pDC);
	void OnRenderFocusVisual(CXTPMarkupDrawingContext* drawingContext);
	CSize ArrangeOverride(CSize szFinalSize);
	CSize MeasureOverride(CXTPMarkupDrawingContext* pDC, CSize szAvailableSize);
	//}}AFX_CODEJOCK_PRIVATE

	//{{AFX_CODEJOCK_PRIVATE
public:
	DECLARE_DISPATCH_MAP()
#	ifdef _XTP_ACTIVEX
	DECLARE_INTERFACE_MAP()
	DECLARE_OLETYPELIB_EX(CXTPMarkupCheckBox);
#	endif

	//}}AFX_CODEJOCK_PRIVATE
};

#	include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPMARKUPCHECKBOX_H__)
