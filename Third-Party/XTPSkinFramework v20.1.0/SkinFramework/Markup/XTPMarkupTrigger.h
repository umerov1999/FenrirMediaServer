// XTPMarkupTrigger.h: interface for the CXTPMarkupTrigger class.
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
#if !defined(__XTPMARKUPTRIGGER_H__)
#	define __XTPMARKUPTRIGGER_H__
//}}AFX_CODEJOCK_PRIVATE

#	if _MSC_VER > 1000
#		pragma once
#	endif // _MSC_VER > 1000

#	include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

class CXTPMarkupObject;
class CXTPMarkupDependencyProperty;
class CXTPMarkupSetterCollection;

class _XTP_EXT_CLASS CXTPMarkupTrigger : public CXTPMarkupObject
{
	DECLARE_MARKUPCLASS(CXTPMarkupTrigger);

public:
	CXTPMarkupTrigger();
	CXTPMarkupTrigger(CXTPMarkupDependencyProperty* pProperty, CXTPMarkupObject* pValue);

public:
	CXTPMarkupSetterCollection* GetSetters() const;
	void SetSetters(CXTPMarkupSetterCollection* pSetters);

public:
	CXTPMarkupDependencyProperty* GetTriggerProperty() const;
	CXTPMarkupObject* GetTriggerValue() const;

public:
	static CXTPMarkupDependencyProperty* m_pSettersProperty;
	static CXTPMarkupDependencyProperty* m_pPropertyProperty;
	static CXTPMarkupDependencyProperty* m_pValueProperty;
};

#	include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPMARKUPTRIGGER_H__)
