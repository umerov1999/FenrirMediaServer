// XTPMarkupPanel.h: interface for the CXTPMarkupPanel class.
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
#if !defined(__XTPMARKUPPANEL_H__)
#	define __XTPMARKUPPANEL_H__
//}}AFX_CODEJOCK_PRIVATE

#	if _MSC_VER > 1000
#		pragma once
#	endif // _MSC_VER > 1000

#	include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

class CXTPMarkupBrush;
class CXTPMarkupUIElementCollection;
class CXTPMarkupVisual;
class CXTPMarkupBuilder;
class CXTPMarkupObject;
class CXTPMarkupDrawingContext;
class CXTPMarkupInputElement;
class CXTPMarkupDependencyProperty;

//===========================================================================
// Summary:
//     CXTPMarkupPanel is a CXTPMarkupFrameworkElement derived class. It
//     implements a base class for all container objects with multiple children.
//===========================================================================
class _XTP_EXT_CLASS CXTPMarkupPanel : public CXTPMarkupFrameworkElement
{
	DECLARE_MARKUPCLASS(CXTPMarkupPanel)

protected:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPMarkupPanel object.
	//-----------------------------------------------------------------------
	CXTPMarkupPanel();

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPMarkupPanel object, handles cleanup and deallocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPMarkupPanel();

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves the collection of child elements.
	// Returns:
	//     A pointer to a CXTPMarkupUIElementCollection object.
	//-----------------------------------------------------------------------
	CXTPMarkupUIElementCollection* GetChildren() const;

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Sets the background brush for the panel object.
	// Parameters:
	//     brush - New background brush for the panel object.
	//-----------------------------------------------------------------------
	void SetBackground(CXTPMarkupBrush* brush);

	//-----------------------------------------------------------------------
	// Summary:
	//     Gets the background brush of the panel object.
	// Returns:
	//     A pointer to the background brush of the panel object.
	//-----------------------------------------------------------------------
	CXTPMarkupBrush* GetBackground() const;

protected:
	//{{AFX_CODEJOCK_PRIVATE
	// Implementation
	virtual int GetVisualChildrenCount() const;
	virtual CXTPMarkupVisual* GetVisualChild(int nIndex) const;

protected:
	virtual void SetContentObject(CXTPMarkupBuilder* pBuilder, CXTPMarkupObject* pContent);
	virtual BOOL HasContentObject() const;
	virtual void OnRender(CXTPMarkupDrawingContext* drawingContext);
	virtual CXTPMarkupInputElement* InputHitTestOverride(CPoint point) const;

protected:
	CXTPMarkupUIElementCollection* m_pChildren;

public:
	static CXTPMarkupDependencyProperty* m_pBackgroundProperty;

	//}}AFX_CODEJOCK_PRIVATE

	//{{AFX_CODEJOCK_PRIVATE
public:
	DECLARE_DISPATCH_MAP()
#	ifdef _XTP_ACTIVEX
	DECLARE_INTERFACE_MAP()
	DECLARE_OLETYPELIB_EX(CXTPMarkupPanel);
#	endif

	afx_msg LPDISPATCH OleGetChildren();
	afx_msg LPDISPATCH OleGetBackground();
	afx_msg void OleSetBackground(LPDISPATCH lpBackground);

	//}}AFX_CODEJOCK_PRIVATE
};

AFX_INLINE CXTPMarkupUIElementCollection* CXTPMarkupPanel::GetChildren() const
{
	return m_pChildren;
}

#	include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPMARKUPPANEL_H__)
