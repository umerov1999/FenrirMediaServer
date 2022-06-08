// XTPMarkupContentControl.h: interface for the CXTPMarkupContentControl class.
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
#if !defined(__XTPMARKUPCONTENTCONTROL_H__)
#	define __XTPMARKUPCONTENTCONTROL_H__
//}}AFX_CODEJOCK_PRIVATE

#	if _MSC_VER > 1000
#		pragma once
#	endif // _MSC_VER > 1000

#	include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

class CXTPMarkupDependencyProperty;
class CXTPMarkupUIElement;
class CXTPMarkupVisual;
class CXTPMarkupBuilder;
class CXTPMarkupObject;

//===========================================================================
// Summary:
//     CXTPMarkupContentControl is a CXTPMarkupControl derived class. It
//     implements a base class for all control elements with a single child.
//===========================================================================
class _XTP_EXT_CLASS CXTPMarkupContentControl : public CXTPMarkupControl
{
	DECLARE_MARKUPCLASS(CXTPMarkupContentControl);

protected:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPMarkupContentControl object.
	//-----------------------------------------------------------------------
	CXTPMarkupContentControl();

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPMarkupContentControl object, handles cleanup and deallocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPMarkupContentControl();

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Gets the child object of the ControlContent element.
	// Returns:
	//     The child CXTPMarkupUIElement object.
	// See Also:
	//     SetContent
	//-----------------------------------------------------------------------
	CXTPMarkupUIElement* GetContent() const;

	// -------------------------------------------------------------------
	// Summary:
	//     Sets the child object for the ControlContent element.
	// Parameters:
	//     pContent - New child CXTPMarkupUIElement object.
	// See Also:
	//     GetContent
	//-----------------------------------------------------------------------
	void SetContent(CXTPMarkupUIElement* pContent);

protected:
	//{{AFX_CODEJOCK_PRIVATE
	// Implementation
	virtual void SetContentObject(CXTPMarkupBuilder* pBuilder, CXTPMarkupObject* pContent);
	virtual BOOL HasContentObject() const;

public:
	virtual int GetVisualChildrenCount() const;
	virtual CXTPMarkupVisual* GetVisualChild(int nIndex) const;

protected:
	static CXTPMarkupDependencyProperty* m_pContentProperty;
	//}}AFX_CODEJOCK_PRIVATE

	//{{AFX_CODEJOCK_PRIVATE
public:
	DECLARE_DISPATCH_MAP()
#	ifdef _XTP_ACTIVEX
	DECLARE_INTERFACE_MAP()
	DECLARE_OLETYPELIB_EX(CXTPMarkupContentControl);
#	endif

	afx_msg LPDISPATCH OleGetContent();
	afx_msg void OleSetContent(LPDISPATCH lpContent);
	//}}AFX_CODEJOCK_PRIVATE
};

AFX_INLINE int CXTPMarkupContentControl::GetVisualChildrenCount() const
{
	return GetContent() != NULL ? 1 : 0;
}

AFX_INLINE CXTPMarkupVisual* CXTPMarkupContentControl::GetVisualChild(int nIndex) const
{
	return nIndex == 0 ? GetContent() : NULL;
}

#	include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPMARKUPCONTENTCONTROL_H__)
