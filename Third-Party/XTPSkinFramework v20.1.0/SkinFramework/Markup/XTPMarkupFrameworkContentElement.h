// XTPMarkupFrameworkContentElement.h: interface for the CXTPMarkupFrameworkContentElement class.
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
#if !defined(__XTPMARKUPFRAMEWORKCONTENTELEMENT_H__)
#	define __XTPMARKUPFRAMEWORKCONTENTELEMENT_H__
//}}AFX_CODEJOCK_PRIVATE

#	if _MSC_VER > 1000
#		pragma once
#	endif // _MSC_VER > 1000

#	include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

class CXTPMarkupFrameworkElement;

class _XTP_EXT_CLASS CXTPMarkupFrameworkContentElement : public CXTPMarkupInputElement
{
	DECLARE_MARKUPCLASS(CXTPMarkupFrameworkContentElement)

protected:
	CXTPMarkupFrameworkContentElement();

public:
	CXTPMarkupObject* GetTag() const;
	void SetTag(CXTPMarkupObject* pTag);

public:
	CXTPMarkupInputElement* InputHitTest(CPoint point) const;
	CRect GetBoundRect() const;
	CRect GetUpdateRect() const;

public:
	CXTPMarkupFrameworkElement* GetParent() const;

protected:
	virtual void OnPropertyChanged(CXTPMarkupDependencyProperty* pProperty,
								   CXTPMarkupObject* pOldValue, CXTPMarkupObject* pNewValue);

	static void AFX_CDECL OnStylePropertyChanged(CXTPMarkupObject* d,
												 CXTPMarkupPropertyChangedEventArgs* e);

public:
	static CXTPMarkupDependencyProperty* m_pTagProperty;
};

AFX_INLINE void CXTPMarkupFrameworkContentElement::SetTag(CXTPMarkupObject* pTag)
{
	SetValue(m_pTagProperty, pTag);
}

AFX_INLINE CXTPMarkupObject* CXTPMarkupFrameworkContentElement::GetTag() const
{
	return GetValue(m_pTagProperty);
}

#	include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPMARKUPFRAMEWORKCONTENTELEMENT_H__)
