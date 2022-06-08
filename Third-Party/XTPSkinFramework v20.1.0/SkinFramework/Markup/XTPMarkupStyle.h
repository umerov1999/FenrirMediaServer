// XTPMarkupStyle.h: interface for the CXTPMarkupStyle class.
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
#if !defined(__XTPMARKUPSTYLE_H__)
#	define __XTPMARKUPSTYLE_H__
//}}AFX_CODEJOCK_PRIVATE

#	if _MSC_VER > 1000
#		pragma once
#	endif // _MSC_VER > 1000

#	include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

class CXTPMarkupObject;
class CXTPMarkupBuilder;
class CXTPMarkupTrigger;
class CXTPMarkupTriggerCollection;
class CXTPMarkupSetterCollection;
class CXTPMarkupDependencyProperty;

class _XTP_EXT_CLASS CXTPMarkupStyle : public CXTPMarkupObject
{
	DECLARE_MARKUPCLASS(CXTPMarkupStyle);

public:
	CXTPMarkupStyle();
	virtual ~CXTPMarkupStyle();

public:
	void Seal();

	CXTPMarkupSetterCollection* GetSetters() const;
	CXTPMarkupTriggerCollection* GetTriggers() const;
	void SetTriggers(CXTPMarkupTriggerCollection* pTriggers);

	CXTPMarkupStyle* GetBasedStyle() const;
	void SetBasedStyle(CXTPMarkupStyle* pStyle);

	CXTPMarkupObject* GetStyleValue(CXTPMarkupDependencyProperty* pProperty) const;

	CXTPMarkupObject* FindResource(const CXTPMarkupObject* pKey) const;

protected:
	void SetContentObject(CXTPMarkupBuilder* pBuilder, CXTPMarkupObject* pContent);
	void ResolveSetterProperty(CXTPMarkupBuilder* pBuilder, CXTPMarkupObject* pSetter);
	void ResolveTriggerProperty(CXTPMarkupBuilder* pBuilder, CXTPMarkupTrigger* pTrigger);
	void SetPropertyObject(CXTPMarkupBuilder* pBuilder, CXTPMarkupDependencyProperty* pProperty,
						   CXTPMarkupObject* pValue);

protected:
	CXTPMarkupSetterCollection* m_pSetters;
	BOOL m_bSealed;
	CXTPMarkupProperties* m_pProperties;

public:
	static CXTPMarkupDependencyProperty* m_pTargetTypeProperty;
	static CXTPMarkupDependencyProperty* m_pBasedOnProperty;
	static CXTPMarkupDependencyProperty* m_pResourcesProperty;
	static CXTPMarkupDependencyProperty* m_pTriggersProperty;
};

#	include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPMARKUPSTYLE_H__)
