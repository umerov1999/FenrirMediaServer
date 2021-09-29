// XTPWinEventHook.h
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
#if !defined(__XTPWINEVENTHOOK_H__)
#	define __XTPWINEVENTHOOK_H__
//}}AFX_CODEJOCK_PRIVATE

#	if _MSC_VER > 1000
#		pragma once
#	endif // _MSC_VER > 1000

#	include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

class _XTP_EXT_CLASS CXTPWinEventHook
{
public:
	DECLARE_HANDLE(HWINEVENTHOOK);
	typedef void(CALLBACK* WINEVENTPROC)(HWINEVENTHOOK hWinEventHook, DWORD event, HWND hwnd,
										 LONG idObject, LONG idChild, DWORD idEventThread,
										 DWORD dwmsEventTime);

public:
	CXTPWinEventHook();
	virtual ~CXTPWinEventHook();

public:
	void SetEventHook(UINT eventMin, UINT eventMax, WINEVENTPROC pfnEventProc);
	void RemoveEventHook();

protected:
	typedef HWINEVENTHOOK(WINAPI* LPFNSETWINEVENTHOOK)(UINT eventMin, UINT eventMax,
													   HMODULE hmodWinEventProc,
													   WINEVENTPROC lpfnWinEventProc,
													   DWORD idProcess, DWORD idThread,
													   UINT dwflags);
	typedef BOOL(WINAPI* LPFNUNHOOKWINEVENT)(HWINEVENTHOOK hWinEventHook);

	static void CALLBACK WinEventProc(HWINEVENTHOOK hWinEventHook, DWORD event, HWND hWnd,
									  LONG idObject, LONG idChild, DWORD dwEventThread,
									  DWORD dwmsEventTime);

protected:
	HWINEVENTHOOK m_hWinEventHook; // HWINEVENTHOOK value that identifies event hook instance
	LPFNSETWINEVENTHOOK m_pSetWinEventHook;
	LPFNUNHOOKWINEVENT m_pUnhookWinEvent;
};

#	include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPWINEVENTHOOK_H__)
