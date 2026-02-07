// XTPMarkupColumnDefinition.h: interface for the CXTPMarkupColumnDefinition class.
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
#if !defined(__XTPMARKUPCOLUMNDEFINITION_H__)
#	define __XTPMARKUPCOLUMNDEFINITION_H__
//}}AFX_CODEJOCK_PRIVATE

#	if _MSC_VER > 1000
#		pragma once
#	endif // _MSC_VER > 1000

#	include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

class CXTPMarkupDependencyProperty;
class CXTPMarkupGridLength;

//===========================================================================
// Summary:
//     CXTPMarkupColumnDefinition is a CXTPMarkupDefinitionBase derived class.
//     It implements the ColumnDefinition XAML Tag.
//===========================================================================
class _XTP_EXT_CLASS CXTPMarkupColumnDefinition : public CXTPMarkupDefinitionBase
{
	DECLARE_MARKUPCLASS(CXTPMarkupColumnDefinition);

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPMarkupColumnDefinition object.
	//-----------------------------------------------------------------------
	CXTPMarkupColumnDefinition();

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Sets the minimum width constraint for the column.
	// Parameters:
	//     nWidth - New minimum width constraint.
	//-----------------------------------------------------------------------
	void SetMinWidth(int nWidth);

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets the maximum width constraint for the column.
	// Parameters:
	//     nWidth - New maximum width constraint.
	//-----------------------------------------------------------------------
	void SetMaxWidth(int nWidth);

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets the width for the column.
	// Parameters:
	//     nWidth - New width for the column.
	//-----------------------------------------------------------------------
	void SetWidth(int nWidth);

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets the width for the column.
	// Parameters:
	//     pLength - New width for the column.
	//-----------------------------------------------------------------------
	void SetWidth(CXTPMarkupGridLength* pLength);

	//{{AFX_CODEJOCK_PRIVATE
public:
	DECLARE_DISPATCH_MAP()
#	ifdef _XTP_ACTIVEX
	DECLARE_INTERFACE_MAP()
	DECLARE_OLETYPELIB_EX(CXTPMarkupColumnDefinition);
#	endif&

	afx_msg void OleSetMinWidth(int nWidth);
	afx_msg void OleSetMaxWidth(int nWidth);
	afx_msg void OleSetWidth(int nWidth);
	afx_msg int OleGetMinWidth();
	afx_msg int OleGetMaxWidth();
	afx_msg int OleGetWidth();
	afx_msg int OleGetUnitType();
	afx_msg void OleSetUnitType(int nType);
	//}}AFX_CODEJOCK_PRIVATE

public:
	static CXTPMarkupDependencyProperty* m_pWidthProperty;
	static CXTPMarkupDependencyProperty* m_pMinWidthProperty;
	static CXTPMarkupDependencyProperty* m_pMaxWidthProperty;
};

#	include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPMARKUPCOLUMNDEFINITION_H__)
