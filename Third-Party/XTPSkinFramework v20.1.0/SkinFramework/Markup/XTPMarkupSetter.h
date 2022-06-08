// XTPMarkupSetter.h: interface for the CXTPMarkupSetter class.
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
#if !defined(__XTPMARKUPSETTER_H__)
#	define __XTPMARKUPSETTER_H__
//}}AFX_CODEJOCK_PRIVATE

#	if _MSC_VER > 1000
#		pragma once
#	endif // _MSC_VER > 1000

#	include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

class CXTPMarkupObject;
class CXTPMarkupDependencyProperty;

class _XTP_EXT_CLASS CXTPMarkupSetter : public CXTPMarkupObject
{
	DECLARE_MARKUPCLASS(CXTPMarkupSetter);

public:
	CXTPMarkupSetter();
	CXTPMarkupSetter(CXTPMarkupDependencyProperty* pProperty, CXTPMarkupObject* pValue);

public:
	CXTPMarkupDependencyProperty* GetSetterProperty() const;
	CXTPMarkupObject* GetSetterValue() const;

public:
	static CXTPMarkupDependencyProperty* m_pPropertyProperty;
	static CXTPMarkupDependencyProperty* m_pValueProperty;
};

#	include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPMARKUPSETTER_H__)
