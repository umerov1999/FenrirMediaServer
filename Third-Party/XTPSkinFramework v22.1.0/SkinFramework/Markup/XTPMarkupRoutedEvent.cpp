// XTPMarkupRoutedEvent.cpp : implementation file
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

#include "StdAfx.h"

#include "Common/XTPFramework.h"

#include "Markup/XTPMarkupObject.h"
#include "Markup/XTPMarkupString.h"
#include "Markup/XTPMarkupRoutedEvent.h"

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

#ifdef _DEBUG
#	define new DEBUG_NEW
#	undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////
// CXTPMarkupRoutedEvent

CXTPMarkupRoutedEvent::CXTPMarkupRoutedEvent()
{
}

CXTPMarkupRoutedEvent* AFX_CDECL CXTPMarkupRoutedEvent::RegisterRoutedEvent(
	LPCWSTR lpszName, RoutingStrategy routingStrategy, CXTPMarkupType* pOwnerType)
{
	CXTPMarkupRoutedEvent* pEvent = (CXTPMarkupRoutedEvent*)RegisterCommon(
		new CXTPMarkupRoutedEvent(), lpszName, MARKUP_TYPE(CXTPMarkupString), pOwnerType, FALSE);

	if (routingStrategy != routingDirect)
	{
		pEvent->m_pMetadata = new CXTPMarkupPropertyMetadata(NULL, routingStrategy);
	}

	return pEvent;
}
