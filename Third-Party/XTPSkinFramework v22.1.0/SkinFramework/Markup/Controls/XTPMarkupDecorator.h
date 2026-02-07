// XTPMarkupDecorator.h: interface for the CXTPMarkupDecorator class.
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
#if !defined(__XTPMARKUPDECORATOR_H__)
#	define __XTPMARKUPDECORATOR_H__
//}}AFX_CODEJOCK_PRIVATE

#	if _MSC_VER > 1000
#		pragma once
#	endif // _MSC_VER > 1000

#	include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

class CXTPMarkupUIElement;
class CXTPMarkupBuilder;
class CXTPMarkupObject;
class CXTPMarkupVisual;

//===========================================================================
// Summary:
//     CXTPMarkupDecorator is a CXTPMarkupFrameworkElement derived class.
//     It implements a basic framework element with a single child.
//===========================================================================
class _XTP_EXT_CLASS CXTPMarkupDecorator : public CXTPMarkupFrameworkElement
{
protected:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPMarkupDecorator object.
	//-----------------------------------------------------------------------
	CXTPMarkupDecorator();

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPMarkupDecorator object, handles cleanup and deallocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPMarkupDecorator();

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Sets a new child object for the decorator element.
	// Parameters:
	//     pChild - New child CXTPMarkupUIElement object.
	// See Also:
	//     GetChild
	//-----------------------------------------------------------------------
	void SetChild(CXTPMarkupUIElement* pChild);

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves the child object of the decorator element.
	// Returns:
	//     The child CXTPMarkupUIElement object.
	// See Also:
	//     SetChild
	//-----------------------------------------------------------------------
	CXTPMarkupUIElement* GetChild() const;

protected:
	//{{AFX_CODEJOCK_PRIVATE

	// Implementation
	virtual void SetContentObject(CXTPMarkupBuilder* pBuilder, CXTPMarkupObject* pContent);
	virtual BOOL HasContentObject() const;

public:
	virtual int GetVisualChildrenCount() const;
	virtual CXTPMarkupVisual* GetVisualChild(int nIndex) const;

protected:
	CXTPMarkupUIElement* m_pChild;
	//}}AFX_CODEJOCK_PRIVATE
};

AFX_INLINE int CXTPMarkupDecorator::GetVisualChildrenCount() const
{
	return m_pChild != NULL ? 1 : 0;
}
AFX_INLINE CXTPMarkupVisual* CXTPMarkupDecorator::GetVisualChild(int nIndex) const
{
	return nIndex == 0 ? m_pChild : NULL;
}

#	include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPMARKUPDECORATOR_H__)
