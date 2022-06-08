// XTPMarkupRoutedEventArgs.cpp : implementation file
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

#include "StdAfx.h"

#include "Common/XTPCasting.h"
#include "Common/XTPFramework.h"

#include "Markup/XTPMarkupTools.h"
#include "Markup/XTPMarkupObject.h"
#include "Markup/XTPMarkupRoutedEvent.h"
#include "Markup/XTPMarkupRoutedEventArgs.h"
#include "Markup/XTPMarkupIIDs.h"

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

#ifdef _DEBUG
#	define new DEBUG_NEW
#	undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////
// CXTPMarkupRoutedEventArgs

CXTPMarkupRoutedEventArgs::CXTPMarkupRoutedEventArgs(CXTPMarkupRoutedEvent* pEvent,
													 CXTPMarkupObject* pSource)
{
	m_pSource  = pSource;
	m_pEvent   = pEvent;
	m_bHandled = FALSE;

	EnableAutomation();
#ifdef _XTP_ACTIVEX
	EnableTypeLib();
#endif
}

CXTPMarkupRoutedEventArgs::CXTPMarkupRoutedEventArgs()
{
	m_pSource  = NULL;
	m_pEvent   = NULL;
	m_bHandled = FALSE;

	EnableAutomation();
#ifdef _XTP_ACTIVEX
	EnableTypeLib();
#endif
}

#include "Common/Base/Diagnostic/XTPBeginAfxMap.h"
BEGIN_DISPATCH_MAP(CXTPMarkupRoutedEventArgs, CXTPCmdTarget)
	DISP_PROPERTY_EX_ID(CXTPMarkupRoutedEventArgs, "Handled", 100, OleGetHandled, OleSetHandled,
						VT_BOOL)
	DISP_PROPERTY_EX_ID(CXTPMarkupRoutedEventArgs, "Event", 101, OleGetEvent, SetNotSupported,
						VT_DISPATCH)
	DISP_PROPERTY_EX_ID(CXTPMarkupRoutedEventArgs, "Source", 102, OleGetSource, SetNotSupported,
						VT_DISPATCH)
END_DISPATCH_MAP()
#include "Common/Base/Diagnostic/XTPEndAfxMap.h"

BOOL CXTPMarkupRoutedEventArgs::OleGetHandled()
{
	return IsHandled();
}

void CXTPMarkupRoutedEventArgs::OleSetHandled(BOOL bHandled)
{
	if (bHandled)
		SetHandled();
}

LPDISPATCH CXTPMarkupRoutedEventArgs::OleGetEvent()
{
	return XTPGetDispatch(GetEvent());
}

LPDISPATCH CXTPMarkupRoutedEventArgs::OleGetSource()
{
	return XTPGetDispatch(GetSource());
}

#ifdef _XTP_ACTIVEX

BEGIN_INTERFACE_MAP(CXTPMarkupRoutedEventArgs, CXTPCmdTarget)
	INTERFACE_PART(CXTPMarkupRoutedEventArgs, XTPDIID_MarkupRoutedEventArgs, Dispatch)
END_INTERFACE_MAP()

IMPLEMENT_OLETYPELIB_EX(CXTPMarkupRoutedEventArgs, XTPDIID_MarkupRoutedEventArgs)

#endif
