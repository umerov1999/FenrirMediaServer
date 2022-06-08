// XTPMarkupEventHandlerMap.h: interface for the CXTPMarkupEventHandlerMap class.
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
#if !defined(__XTPMARKUPEVENTHANDLERMAP_H__)
#	define __XTPMARKUPEVENTHANDLERMAP_H__
//}}AFX_CODEJOCK_PRIVATE

#	if _MSC_VER > 1000
#		pragma once
#	endif // _MSC_VER > 1000

#	include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

class _XTP_EXT_CLASS CXTPMarkupEventHandlerMap
{
public:
	CXTPMarkupEventHandlerMap();
	~CXTPMarkupEventHandlerMap();

public:
	void Add(CXTPMarkupRoutedEvent* pEvent, CXTPMarkupDelegate* pDelegate);
	void RemoveAll();
	void Raise(CXTPMarkupObject* pSender, CXTPMarkupRoutedEventArgs* pEventArgs);

protected:
	CMap<CXTPMarkupRoutedEvent*, CXTPMarkupRoutedEvent*, CPtrList*, CPtrList*> m_mapHandlers;
};

#	include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPMARKUPEVENTHANDLERMAP_H__)
