// XTPMarkupRoutedEvent.h: interface for the CXTPMarkupRoutedEvent class.
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
#if !defined(__XTPMARKUPROUTEDEVENT_H__)
#	define __XTPMARKUPROUTEDEVENT_H__
//}}AFX_CODEJOCK_PRIVATE

#	if _MSC_VER > 1000
#		pragma once
#	endif // _MSC_VER > 1000

#	include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

class CXTPMarkupType;

class _XTP_EXT_CLASS CXTPMarkupRoutedEvent : public CXTPMarkupDependencyProperty
{
public:
	enum RoutingStrategy
	{
		routingDirect = 0,
		routingBubble = 1
	};

protected:
	CXTPMarkupRoutedEvent();

public:
	virtual BOOL IsEvent() const;

public:
	static CXTPMarkupRoutedEvent* AFX_CDECL RegisterRoutedEvent(LPCWSTR lpszName,
																RoutingStrategy routingStrategy,
																CXTPMarkupType* pOwnerType);

	//{{AFX_CODEJOCK_PRIVATE
public:
	DECLARE_DISPATCH_MAP()
#	ifdef _XTP_ACTIVEX
	DECLARE_INTERFACE_MAP()
	DECLARE_OLETYPELIB_EX(CXTPMarkupRoutedEvent);
#	endif

	afx_msg BSTR OleGetName();

	//}}AFX_CODEJOCK_PRIVATE
};

AFX_INLINE BOOL CXTPMarkupRoutedEvent::IsEvent() const
{
	return TRUE;
}

#	include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPMARKUPROUTEDEVENT_H__)
