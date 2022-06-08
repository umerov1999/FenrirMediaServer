// XTPMarkupRowDefinitionCollection.h: interface for the CXTPMarkupRowDefinitionCollection class.
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
#if !defined(__XTPMARKUPROWDEFINITIONCOLLECTION_H__)
#	define __XTPMARKUPROWDEFINITIONCOLLECTION_H__
//}}AFX_CODEJOCK_PRIVATE

#	if _MSC_VER > 1000
#		pragma once
#	endif // _MSC_VER > 1000

#	include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

//===========================================================================
// Summary:
//     CXTPMarkupRowDefinitionCollection is a CXTPMarkupDefinitionCollection derived
//     class. It implements a collection of CXTPMarkupRowDefinition classes.
//===========================================================================
class _XTP_EXT_CLASS CXTPMarkupRowDefinitionCollection : public CXTPMarkupDefinitionCollection
{
	DECLARE_MARKUPCLASS(CXTPMarkupRowDefinitionCollection);

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPMarkupRowDefinitionCollection object.
	//-----------------------------------------------------------------------
	CXTPMarkupRowDefinitionCollection();
};

#	include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPMARKUPROWDEFINITIONCOLLECTION_H__)
