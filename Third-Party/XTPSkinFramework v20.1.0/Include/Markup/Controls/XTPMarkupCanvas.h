// XTPMarkupCanvas.h: interface for the CXTPMarkupCanvas class.
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
#if !defined(__XTPMARKUPCANVAS_H__)
#	define __XTPMARKUPCANVAS_H__
//}}AFX_CODEJOCK_PRIVATE

#	if _MSC_VER > 1000
#		pragma once
#	endif // _MSC_VER > 1000

#	include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

class CXTPMarkupUIElement;
class CXTPMarkupDrawingContext;
class CXTPMarkupDependencyProperty;

class _XTP_EXT_CLASS CXTPMarkupCanvas : public CXTPMarkupPanel
{
	DECLARE_MARKUPCLASS(CXTPMarkupCanvas)
protected:
	CXTPMarkupCanvas();
	virtual ~CXTPMarkupCanvas();

public:
	static void AFX_CDECL SetLeft(CXTPMarkupUIElement* pElement, int nLeft);
	static void AFX_CDECL SetTop(CXTPMarkupUIElement* pElement, int nTop);
	static void AFX_CDECL SetRight(CXTPMarkupUIElement* pElement, int nRight);
	static void AFX_CDECL SetBottom(CXTPMarkupUIElement* pElement, int nBottom);

	static int AFX_CDECL GetLeft(CXTPMarkupUIElement* pElement);
	static int AFX_CDECL GetTop(CXTPMarkupUIElement* pElement);
	static int AFX_CDECL GetRight(CXTPMarkupUIElement* pElement);
	static int AFX_CDECL GetBottom(CXTPMarkupUIElement* pElement);

protected:
	//-----------------------------------------------------------------------
	// Summary:
	//     Determines the size required for this element during layout, based
	//     on the size of its child elements.
	// Parameters:
	//     pDC           - Pointer to a valid device context.
	//     availableSize - Available size for this element.
	// Returns:
	//     The size required for this element during layout, based on the size
	//     of its child elements.
	//-----------------------------------------------------------------------
	virtual CSize MeasureOverride(CXTPMarkupDrawingContext* pDC, CSize availableSize);
	virtual CSize ArrangeOverride(CSize arrangeSize);

	//{{AFX_CODEJOCK_PRIVATE
public:
	DECLARE_DISPATCH_MAP()
#	ifdef _XTP_ACTIVEX
	DECLARE_INTERFACE_MAP()
	DECLARE_OLETYPELIB_EX(CXTPMarkupTextBlock);
#	endif

	afx_msg void OleSetLeft(LPDISPATCH lpElementDisp, int nValue);
	afx_msg void OleSetRight(LPDISPATCH lpElementDisp, int nValue);
	afx_msg void OleSetTop(LPDISPATCH lpElementDisp, int nValue);
	afx_msg void OleSetBottom(LPDISPATCH lpElementDisp, int nValue);
	afx_msg int OleGetLeft(LPDISPATCH lpElementDisp);
	afx_msg int OleGetTop(LPDISPATCH lpElementDisp);
	afx_msg int OleGetRight(LPDISPATCH lpElementDisp);
	afx_msg int OleGetBottom(LPDISPATCH lpElementDisp);
	//}}AFX_CODEJOCK_PRIVATE

public:
	static CXTPMarkupDependencyProperty* m_pTopProperty;
	static CXTPMarkupDependencyProperty* m_pLeftProperty;
	static CXTPMarkupDependencyProperty* m_pRightProperty;
	static CXTPMarkupDependencyProperty* m_pBottomProperty;
};

#	include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPMARKUPCANVAS_H__)
