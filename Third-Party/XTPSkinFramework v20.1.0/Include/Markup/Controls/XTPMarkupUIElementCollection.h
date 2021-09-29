// XTPMarkupUIElementCollection.h: interface for the CXTPMarkupUIElementCollection class.
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
#if !defined(__XTPMARKUPUIELEMENTCOLLECTION_H__)
#	define __XTPMARKUPUIELEMENTCOLLECTION_H__
//}}AFX_CODEJOCK_PRIVATE

#	if _MSC_VER > 1000
#		pragma once
#	endif // _MSC_VER > 1000

#	include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

class CXTPMarkupUIElement;

//===========================================================================
// Summary:
//     CXTPMarkupUIElementCollection is a CXTPMarkupCollection derived class.
//     It implements a basic collection of CXTPMarkupUIElement elements.
//===========================================================================
class _XTP_EXT_CLASS CXTPMarkupUIElementCollection : public CXTPMarkupCollection
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPMarkupUIElementCollection object.
	//-----------------------------------------------------------------------
	CXTPMarkupUIElementCollection();

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPMarkupUIElementCollection object, handles cleanup and deallocation.
	//-----------------------------------------------------------------------
	~CXTPMarkupUIElementCollection();

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves a CXTPMarkupUIElement object by its index.
	// Parameters:
	//     nIndex - Index of the CXTPMarkupUIElement object to retrieve.
	// Returns:
	//     A pointer to the CXTPMarkupUIElement object.
	//-----------------------------------------------------------------------
	CXTPMarkupUIElement* GetItem(int nIndex) const;

protected:
	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called when the collection is changed.
	//-----------------------------------------------------------------------
	void OnChanged();

protected:
	//{{AFX_CODEJOCK_PRIVATE
public:
	DECLARE_DISPATCH_MAP()
#	ifdef _XTP_ACTIVEX
	DECLARE_INTERFACE_MAP()
	DECLARE_OLETYPELIB_EX(CXTPMarkupUIElementCollection);
#	endif

	DECLARE_ENUM_VARIANT(CXTPMarkupUIElementCollection);

	long OleGetItemCount();
	LPDISPATCH OleGetItem(long nIndex);
	void OleAdd(LPDISPATCH lpElementDisp);
	void OleInsert(long Index, LPDISPATCH lpElementDisp);
	//}}AFX_CODEJOCK_PRIVATE
};

AFX_INLINE CXTPMarkupUIElement* CXTPMarkupUIElementCollection::GetItem(int nIndex) const
{
	return nIndex >= 0 && nIndex < m_arrItems.GetSize() ? (CXTPMarkupUIElement*)m_arrItems[nIndex]
														: NULL;
}

#	include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPMARKUPUIELEMENTCOLLECTION_H__)
