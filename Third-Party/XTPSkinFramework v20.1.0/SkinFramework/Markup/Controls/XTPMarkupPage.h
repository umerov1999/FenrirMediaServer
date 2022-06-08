// XTPMarkupPage.h: interface for the CXTPMarkupPage class.
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
#if !defined(__XTPMARKUPPAGE_H__)
#	define __XTPMARKUPPAGE_H__
//}}AFX_CODEJOCK_PRIVATE

#	if _MSC_VER > 1000
#		pragma once
#	endif // _MSC_VER > 1000

#	include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

class CXTPMarkupUIElement;
class CXTPMarkupVisual;
class CXTPMarkupDrawingContext;
class CXTPMarkupBuilder;
class CXTPMarkupObject;
class CXTPMarkupInputElement;
class CXTPMarkupUIElement;

//===========================================================================
// Summary:
//     CXTPMarkupPage is a CXTPMarkupFrameworkElement derived class.
//     It implements the Page XAML Tag.
//===========================================================================
class _XTP_EXT_CLASS CXTPMarkupPage : public CXTPMarkupFrameworkElement
{
	DECLARE_MARKUPCLASS(CXTPMarkupPage);

protected:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPMarkupPage object.
	//-----------------------------------------------------------------------
	CXTPMarkupPage();

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPMarkupPage object, handles cleanup and deallocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPMarkupPage();

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Sets the child object for the page element.
	// Parameters:
	//     pContent - Pointer to a CXTPMarkupUIElement object.
	// See Also:
	//     GetContent
	//-----------------------------------------------------------------------
	void SetContent(CXTPMarkupUIElement* pContent);

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves the child object for the page element.
	// Returns:
	//     A pointer to a CXTPMarkupUIElement object.
	// See Also:
	//     SetContent
	//-----------------------------------------------------------------------
	CXTPMarkupUIElement* GetContent() const;

public:
	//{{AFX_CODEJOCK_PRIVATE

	// Implementation
	virtual int GetVisualChildrenCount() const;
	virtual CXTPMarkupVisual* GetVisualChild(int nIndex) const;

protected:
	virtual CSize MeasureOverride(CXTPMarkupDrawingContext* pDC, CSize szAvailableSize);
	virtual CSize ArrangeOverride(CSize szFinalSize);
	virtual void SetContentObject(CXTPMarkupBuilder* pBuilder, CXTPMarkupObject* pContent);
	virtual BOOL HasContentObject() const;

	CXTPMarkupInputElement* InputHitTestOverride(CPoint /*point*/) const;

protected:
	CXTPMarkupUIElement* m_pContent;
	//}}AFX_CODEJOCK_PRIVATE

	//{{AFX_CODEJOCK_PRIVATE
public:
	DECLARE_DISPATCH_MAP()
#	ifdef _XTP_ACTIVEX
	DECLARE_INTERFACE_MAP()
	DECLARE_OLETYPELIB_EX(CXTPMarkupPage);
#	endif

	afx_msg LPDISPATCH OleGetChild();
	afx_msg void OleSetChild(LPDISPATCH lpDisp);
	//}}AFX_CODEJOCK_PRIVATE
};

class _XTP_EXT_CLASS CXTPMarkupWindowContainer : public CXTPMarkupFrameworkElement
{
	DECLARE_MARKUPCLASS(CXTPMarkupWindowContainer);

protected:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPMarkupWindowContainer object.
	//-----------------------------------------------------------------------
	CXTPMarkupWindowContainer();

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPMarkupWindowContainer object, handles cleanup and deallocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPMarkupWindowContainer();

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Sets the child window for the page element.
	// Parameters:
	//     hwndContent - Child window to be set.
	// See Also:
	//     SetContent
	//-----------------------------------------------------------------------
	void SetChild(HWND hwndContent);

	//-----------------------------------------------------------------------
	// Summary:
	//     Gets the child window of the page element.
	// Returns:
	//     The child window of the page element.
	// See Also:
	//     SetContent
	//-----------------------------------------------------------------------
	HWND GetChild() const;

public:
	//{{AFX_CODEJOCK_PRIVATE

	// Implementation

protected:
	virtual CSize MeasureOverride(CXTPMarkupDrawingContext* pDC, CSize szAvailableSize);
	virtual CSize ArrangeOverride(CSize szFinalSize);
	void OnRender(CXTPMarkupDrawingContext* pDC);
	void RenderClipped(CXTPMarkupDrawingContext* pDC);

protected:
	HWND m_hWndChild;
	CSize m_szChild;
	//}}AFX_CODEJOCK_PRIVATE

	//{{AFX_CODEJOCK_PRIVATE
public:
	DECLARE_DISPATCH_MAP()
#	ifdef _XTP_ACTIVEX
	DECLARE_INTERFACE_MAP()
	DECLARE_OLETYPELIB_EX(CXTPMarkupWindowContainer);
#	endif

	afx_msg HWND OleGetChild();
	afx_msg void OleSetChild(HWND hWnd);
	//}}AFX_CODEJOCK_PRIVATE
};

AFX_INLINE int CXTPMarkupPage::GetVisualChildrenCount() const
{
	return m_pContent != NULL ? 1 : 0;
}
AFX_INLINE CXTPMarkupVisual* CXTPMarkupPage::GetVisualChild(int nIndex) const
{
	return nIndex == 0 ? m_pContent : NULL;
}

#	include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPMARKUPPAGE_H__)
