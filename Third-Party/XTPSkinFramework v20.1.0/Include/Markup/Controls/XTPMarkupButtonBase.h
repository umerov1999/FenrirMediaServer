// XTPMarkupButtonBase.h: interface for the CXTPMarkupButtonBase class.
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
#if !defined(__XTPMARKUPBUTTONBASE_H__)
#	define __XTPMARKUPBUTTONBASE_H__
//}}AFX_CODEJOCK_PRIVATE

#	if _MSC_VER > 1000
#		pragma once
#	endif // _MSC_VER > 1000

#	include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

class CXTPWinThemeWrapper;
class CXTPMarkupDependencyProperty;
class CXTPMarkupRoutedEvent;
class CXTPMarkupMouseEventArgs;
class CXTPMarkupMouseButtonEventArgs;

//===========================================================================
// Summary:
//     CXTPMarkupButtonBase is a CXTPMarkupContentControl derived class.
//     It implements a base class for the Button element.
//===========================================================================
class _XTP_EXT_CLASS CXTPMarkupButtonBase : public CXTPMarkupContentControl
{
	DECLARE_MARKUPCLASS(CXTPMarkupButtonBase);

protected:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPMarkupButtonBase object.
	//-----------------------------------------------------------------------
	CXTPMarkupButtonBase();

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPMarkupButtonBase object, handles cleanup and deallocation.
	//-----------------------------------------------------------------------
	~CXTPMarkupButtonBase();

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Determines if the button is currently pressed.
	// Returns:
	//     TRUE if the button is currently pressed.
	//-----------------------------------------------------------------------
	BOOL IsPressed() const;

protected:
	//-----------------------------------------------------------------------
	// Summary:
	//     Virtual method called when the user clicks the button.
	//-----------------------------------------------------------------------
	virtual void OnClick();

	//{{AFX_CODEJOCK_PRIVATE
	// Implementation
protected:
	void OnMouseLeave(CXTPMarkupMouseEventArgs* e);
	void OnMouseEnter(CXTPMarkupMouseEventArgs* e);

	void OnMouseLeftButtonUp(CXTPMarkupMouseButtonEventArgs* e);
	void OnMouseLeftButtonDown(CXTPMarkupMouseButtonEventArgs* e);

	void OnLostMouseCapture(CXTPMarkupMouseEventArgs* e);

protected:
	void SetPressed(BOOL bPressed);

	void UseParentBackground(HDC destHdc, HDC srcHdc, CRect srcRect) const;

protected:
	BOOL m_bPushed;
	BOOL m_bMouseOver;
	CXTPWinThemeWrapper* m_themeButton;

public:
	static CXTPMarkupDependencyProperty* m_pIsPressedProperty;
	static CXTPMarkupRoutedEvent* m_pClickEvent;

	//}}AFX_CODEJOCK_PRIVATE

	//{{AFX_CODEJOCK_PRIVATE
public:
	DECLARE_DISPATCH_MAP()
#	ifdef _XTP_ACTIVEX
	DECLARE_INTERFACE_MAP()
	DECLARE_OLETYPELIB_EX(CXTPMarkupButtonBase);
#	endif

	afx_msg BOOL OleGetIsPressed();
	afx_msg void OleSetIsPressed(BOOL bValue);
	afx_msg LPDISPATCH OleGetClickEvent();

	//}}AFX_CODEJOCK_PRIVATE
};

#	include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPMARKUPBUTTONBASE_H__)
