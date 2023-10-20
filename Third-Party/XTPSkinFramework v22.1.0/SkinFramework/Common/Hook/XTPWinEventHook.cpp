// XTPWinEventHook.cpp
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

#include "Common/XTPCasting.h"
#include "Common/Hook/XTPWinEventHook.h"

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

#ifdef _DEBUG
#	define new DEBUG_NEW
#	undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CXTPWinEventHook::CXTPWinEventHook()
	: m_hWinEventHook(NULL)
	, m_pSetWinEventHook(NULL)
	, m_pUnhookWinEvent(NULL)
{
	HMODULE hLibrary = GetModuleHandle(_T("USER32"));

	if (hLibrary)
	{
		m_pSetWinEventHook = XTPToFunctionPtr<LPFNSETWINEVENTHOOK>(
			::GetProcAddress(hLibrary, "SetWinEventHook"));
		m_pUnhookWinEvent = XTPToFunctionPtr<LPFNUNHOOKWINEVENT>(
			::GetProcAddress(hLibrary, "UnhookWinEvent"));
	}
}

CXTPWinEventHook::~CXTPWinEventHook()
{
}

void CXTPWinEventHook::SetEventHook(UINT eventMin, UINT eventMax, WINEVENTPROC pfnEventProc)
{
	if (m_pSetWinEventHook && !m_hWinEventHook)
	{
		m_hWinEventHook = m_pSetWinEventHook(eventMin, eventMax, NULL, pfnEventProc,
											 ::GetCurrentProcessId(), 0, 0);
	}
}

void CXTPWinEventHook::RemoveEventHook()
{
	if (m_pUnhookWinEvent && m_hWinEventHook)
	{
		m_pUnhookWinEvent(m_hWinEventHook);
		m_hWinEventHook = NULL;
	}
}
