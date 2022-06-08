// XTPMarkupToggleButton.h: interface for the CXTPMarkupToggleButton class.
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
#if !defined(__XTPMARKUPTOGGLEBUTTON_H__)
#	define __XTPMARKUPTOGGLEBUTTON_H__
//}}AFX_CODEJOCK_PRIVATE

#	if _MSC_VER > 1000
#		pragma once
#	endif // _MSC_VER > 1000

#	include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

class CXTPMarkupDependencyProperty;
class CXTPMarkupRoutedEvent;
class CXTPMarkupObject;
class CXTPMarkupBuilder;
class CXTPMarkupRoutedEventArgs;

//===========================================================================
// Summary:
//     CXTPMarkupToggleButton is a CXTPMarkupButtonBase derived class.
//     It implements a base class for Radio and CheckBox buttons.
//===========================================================================
class _XTP_EXT_CLASS CXTPMarkupToggleButton : public CXTPMarkupButtonBase
{
	DECLARE_MARKUPCLASS(CXTPMarkupToggleButton);

public:
	// ----------------------------------------------------------------------
	// Summary:
	//     Call this method to set the checked state for a button.
	// Parameters:
	//     bChecked - TRUE to set the button to checked, FALSE otherwise.
	//-----------------------------------------------------------------------
	void SetChecked(BOOL bChecked);

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines the checked state of a button.
	// Returns:
	//     TRUE if the button is checked, otherwise FALSE.
	//-----------------------------------------------------------------------
	BOOL GetChecked() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Specifies if the button supports two or three states.
	// Parameters:
	//     bThreeState - TRUE to set the button to support three states
	//                   (Checked, Normal, and Indeterminate).
	//-----------------------------------------------------------------------
	void SetThreeState(BOOL bThreeState);

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines if the button supports two or three states.
	// Returns:
	//     TRUE if the button supports three states
	//     (Checked, Normal, and Indeterminate).
	//-----------------------------------------------------------------------
	BOOL IsThreeState() const;

protected:
	//-----------------------------------------------------------------------
	// Summary:
	//     Virtual method called when the user clicks the button.
	//-----------------------------------------------------------------------
	virtual void OnClick();

	//-----------------------------------------------------------------------
	// Summary:
	//     Virtual method called when the state is toggled by the user.
	//-----------------------------------------------------------------------
	virtual void OnToggle();

	//-----------------------------------------------------------------------
	// Summary:
	//     Virtual method called when the user sets the state to checked.
	// Parameters:
	//     e - Mouse pointer information.
	//-----------------------------------------------------------------------
	virtual void OnChecked(CXTPMarkupRoutedEventArgs* e);

	//-----------------------------------------------------------------------
	// Summary:
	//     Virtual method called when the user sets the state to unchecked.
	// Parameters:
	//     e - Mouse pointer information.
	//-----------------------------------------------------------------------
	virtual void OnUnchecked(CXTPMarkupRoutedEventArgs* e);

	//-----------------------------------------------------------------------
	// Summary:
	//     Virtual method called when the user sets the state to indeterminate.
	// Parameters:
	//     e - Mouse pointer information.
	//-----------------------------------------------------------------------
	virtual void OnIndeterminate(CXTPMarkupRoutedEventArgs* e);

	//{{AFX_CODEJOCK_PRIVATE
	// Implementation
protected:
	static void AFX_CDECL OnIsCheckedChanged(CXTPMarkupObject* d,
											 CXTPMarkupPropertyChangedEventArgs* e);
	static CXTPMarkupObject* AFX_CDECL
		ConvertIsChecked(CXTPMarkupBuilder* pBuilder, CXTPMarkupObject* pObject,
						 CXTPMarkupDependencyProperty* pAssociatedProperty = NULL);

public:
	static CXTPMarkupDependencyProperty* m_pIsThreeStateProperty;
	static CXTPMarkupDependencyProperty* m_pIsCheckedProperty;
	static CXTPMarkupRoutedEvent* m_pCheckedEvent;
	static CXTPMarkupRoutedEvent* m_pUncheckedEvent;
	static CXTPMarkupRoutedEvent* m_pIndeterminateEvent;
	//}}AFX_CODEJOCK_PRIVATE

	//{{AFX_CODEJOCK_PRIVATE
public:
	DECLARE_DISPATCH_MAP()
#	ifdef _XTP_ACTIVEX
	DECLARE_INTERFACE_MAP()
	DECLARE_OLETYPELIB_EX(CXTPMarkupToggleButton);
#	endif

	afx_msg long OleGetIsChecked();
	afx_msg void OleSetIsChecked(long bValue);

	afx_msg BOOL OleGetIsThreeState();
	afx_msg void OleSetIsThreeState(BOOL bValue);

	afx_msg LPDISPATCH OleGetCheckedEvent();
	afx_msg LPDISPATCH OleGetIndeterminateEvent();
	afx_msg LPDISPATCH OleGetUncheckedEvent();

	//}}AFX_CODEJOCK_PRIVATE
};

AFX_INLINE void CXTPMarkupToggleButton::SetChecked(BOOL bChecked)
{
	SetValue(m_pIsCheckedProperty, CXTPMarkupEnum::CreateValue(bChecked));
}

AFX_INLINE void CXTPMarkupToggleButton::SetThreeState(BOOL bThreeState)
{
	SetValue(m_pIsThreeStateProperty, CXTPMarkupBool::CreateValue(bThreeState));
}

#	include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPMARKUPTOGGLEBUTTON_H__)
