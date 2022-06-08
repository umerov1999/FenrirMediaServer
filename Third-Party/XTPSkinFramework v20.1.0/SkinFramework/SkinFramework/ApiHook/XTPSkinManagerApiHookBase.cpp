// XTPSkinManagerApiHookBase.cpp: implementation of the CXTPSkinManagerApiHookBase class.
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

#include "stdafx.h"

#if _MSC_VER >= 1900
// warning C4091 : 'typedef ' : ignored on left of '' when no variable is declared
#	pragma warning(push)
#	pragma warning(disable : 4091)
#endif

#if _MSC_VER >= 1900
#	pragma warning(pop)
#endif

#include "Common/XTPTypeId.h"
#include "Common/XTPFramework.h"
#include "Common/XTPVC80Helpers.h"
#include "Common/XTPSystemHelpers.h"
#include "Common/XTPSynchro.h"
#include "Common/XTPApplication.h"
#include "Common/XTPSingleton.h"
#include "Common/XTPGdiObjects.h"

#include "SkinFramework/XTPSkinManager.h"
#include "SkinFramework/XTPSkinObject.h"
#include "SkinFramework/ApiHook/XTPSkinManagerApiHookBase.h"
#include "SkinFramework/XTPSkinManagerModuleList.h"

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

#ifdef _DEBUG
#	define new DEBUG_NEW
#	undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////////////
// CXTPSkinManagerApiHookBase

IMPLEMENT_DYNAMIC(CXTPSkinManagerApiHookBase, CObject)

PVOID CXTPSkinManagerApiHookBase::sm_pfnAfxWndProc = NULL;

CXTPSkinManagerApiHookBase::CXTPSkinManagerApiHookBase(BOOL bLegacy)
	: m_bLegacy(bLegacy)
{
	if (sm_pfnAfxWndProc == NULL)
	{
		sm_pfnAfxWndProc = reinterpret_cast<LPVOID>(AfxGetAfxWndProc());
	}
}

CXTPSkinManagerApiHookBase::~CXTPSkinManagerApiHookBase()
{
}

static BOOL GetModuleChecksum(LPVOID lpModuleStart, WORD& wChecksum)
{
	PIMAGE_DOS_HEADER pDosHdr = reinterpret_cast<PIMAGE_DOS_HEADER>(lpModuleStart);
	if (!::AfxIsValidAddress(pDosHdr, sizeof(*pDosHdr), FALSE))
		return FALSE;

	if (pDosHdr->e_magic != IMAGE_DOS_SIGNATURE)
		return FALSE;

	wChecksum = pDosHdr->e_csum;

	return TRUE;
}

struct XTP_SYSTEMWINDOWMODULEINFO
{
	explicit XTP_SYSTEMWINDOWMODULEINFO(LPCTSTR lpName)
		: lpName(lpName)
		, wChecksum(0)
	{
		memset(&info, 0, sizeof(info));
	}

	LPCTSTR lpName;
	XTP_MODULEINFO info;
	CXTPModuleHandle hmod;
	WORD wChecksum;
};

static BOOL IsImplementedInSystemWindowModule(LPVOID lpProc, XTP_SYSTEMWINDOWMODULEINFO& swmi,
											  CXTPSkinManagerModuleList** pModuleList)
{
	ASSERT(NULL != lpProc);
	ASSERT(NULL != pModuleList);

	if (NULL != swmi.info.lpBaseOfDll)
	{
		WORD wChecksum = 0;
		if (GetModuleChecksum(swmi.info.lpBaseOfDll, wChecksum))
		{
			if (swmi.wChecksum != wChecksum)
			{
				swmi.info.lpBaseOfDll = NULL;
				swmi.info.hModule	  = NULL;
			}

			swmi.wChecksum = wChecksum;
		}
		else
		{
			swmi.info.lpBaseOfDll = NULL;
			swmi.info.hModule	  = NULL;
		}
	}

	if (NULL == swmi.info.lpBaseOfDll && swmi.hmod.GetModuleHandle(swmi.lpName))
	{
		if (NULL == *pModuleList)
		{
			*pModuleList = new CXTPSkinManagerModuleList(::GetCurrentProcessId());
		}

		(*pModuleList)->GetModuleInformation(swmi.hmod, &swmi.info);

		ASSERT(swmi.info.hModule == swmi.hmod);
		ASSERT(NULL != swmi.info.lpBaseOfDll);

		// For those rare cases when GetModuleInformation failed to pull out module info.
		if (NULL == swmi.info.hModule)
			swmi.info.hModule = swmi.hmod;
		if (NULL == swmi.info.lpBaseOfDll)
			swmi.info.lpBaseOfDll = reinterpret_cast<LPVOID>(swmi.info.hModule);
	}

	return (NULL != swmi.info.lpBaseOfDll
			&& reinterpret_cast<LPBYTE>(lpProc) > reinterpret_cast<LPBYTE>(swmi.info.lpBaseOfDll)
			&& reinterpret_cast<LPBYTE>(lpProc)
				   < reinterpret_cast<LPBYTE>(swmi.info.lpBaseOfDll) + swmi.info.SizeOfImage);
}

BOOL AFX_CDECL CXTPSkinManagerApiHookBase::IsSystemWindowModule(WNDPROC lpWndProc)
{
	if ((HIWORD((ULONG)(ULONG_PTR)lpWndProc)) == 0xFFFF)
		return TRUE;

	CXTPSkinManagerModuleList* pModuleList = NULL;

	static XTP_SYSTEMWINDOWMODULEINFO swmiUser32(_T("USER32.DLL"));
	BOOL bResult = IsImplementedInSystemWindowModule(lpWndProc, swmiUser32, &pModuleList);

	if (!bResult)
	{
		static XTP_SYSTEMWINDOWMODULEINFO swmiComCtl32(_T("COMCTL32.DLL"));
		bResult = IsImplementedInSystemWindowModule(lpWndProc, swmiComCtl32, &pModuleList);
	}

	if (!bResult && XTPSystemVersion()->IsWinXPOrGreater())
	{
		static XTP_SYSTEMWINDOWMODULEINFO swmiNtdll(_T("NTDLL.DLL"));
		bResult = IsImplementedInSystemWindowModule(lpWndProc, swmiNtdll, &pModuleList);
	}

	if (!bResult && XTPSystemVersion()->IsWin7OrGreater())
	{
		static XTP_SYSTEMWINDOWMODULEINFO swmiAppHelp(_T("APPHELP.DLL"));
		bResult = IsImplementedInSystemWindowModule(lpWndProc, swmiAppHelp, &pModuleList);
	}

	if (NULL != pModuleList)
		delete pModuleList;

	return bResult;
}

BOOL AFX_CDECL CXTPSkinManagerApiHookBase::IsHookedByAppHelpDll(LPVOID lpProc)
{
	if (!XTPSystemVersion()->IsWin7OrGreater())
		return FALSE;

	CXTPSkinManagerModuleList* pModuleList = NULL;

	static XTP_SYSTEMWINDOWMODULEINFO swmiAppHelp(_T("APPHELP.DLL"));
	BOOL bResult = IsImplementedInSystemWindowModule(lpProc, swmiAppHelp, &pModuleList);

	if (NULL != pModuleList)
		delete pModuleList;

	return bResult;
}

BOOL CXTPSkinManagerApiHookBase::CallHookDefWindowProc(HWND hWnd, LPVOID pfnOrig,
													   XTPSkinDefaultProc defProc, LPVOID lpPrev,
													   UINT nMessage, WPARAM& wParam,
													   LPARAM& lParam, LRESULT& lResult)
{
	XTP_SKINFRAMEWORK_ASSERT_WINDOW_THREAD(hWnd);

	DWORD dwWindowThreadID = 0;
	DWORD dwCurrThreadID   = 0;

	if (::IsWindow(hWnd))
	{
		dwWindowThreadID = GetWindowThreadProcessId(hWnd, NULL);
		dwCurrThreadID	 = GetCurrentThreadId();
		if (dwWindowThreadID != dwCurrThreadID)
			return FALSE;
	}

	CXTPAsyncGuard<CXTPSkinObject, CXTPRWCriticalSection::CSharedLock>* pSkinObjectGuard = NULL;
	XTP_GUARD_SHARED_(CXTPSkinManager, XTPSkinManager(), pSkinManager)
	{
		if (!pSkinManager->IsEnabled())
			return FALSE;

		CXTPSkinObject* pSkinObject = pSkinManager->Lookup(hWnd);

		if (!pSkinObject)
			return FALSE;

		BOOL bWindowProcAttached = FALSE;

		XTP_GUARD_SHARED(CXTPSkinObject, pSkinObject)
		{
			if (pSkinObject->m_bCustomDraw)
				return FALSE;

			if (nMessage == pSkinObject->GetHeadMessage())
				return FALSE;

			bWindowProcAttached = pSkinObject->m_bWindowProcAttached;
		}

		if (nMessage == WM_NCDESTROY && !bWindowProcAttached)
		{
			XTPAccessExclusive(XTPSkinManager())->Remove(hWnd, TRUE);
			return FALSE;
		}

		// Move skin object guard outside skin manager guard's scope
		pSkinObjectGuard = new CXTPAsyncGuard<CXTPSkinObject, CXTPRWCriticalSection::CSharedLock>(
			XTPAccessShared(pSkinObject));
	}

	MSG& curMsg	   = AfxGetThreadState()->m_lastSentMsg;
	MSG oldMsg	   = curMsg;
	curMsg.hwnd	   = hWnd;
	curMsg.message = nMessage;
	curMsg.wParam  = wParam;
	curMsg.lParam  = lParam;

	CXTPSkinObject* pSkinObject = *pSkinObjectGuard;
	pSkinObject->OnBeginHook(nMessage, defProc, reinterpret_cast<PROC>(pfnOrig), lpPrev);
	BOOL bResult = pSkinObject->OnHookDefWindowProc(nMessage, wParam, lParam, lResult);
	curMsg		 = oldMsg;
	pSkinObject->InternalAddRef(); // Holding a reference is required prior to releasing the
								   // guard
	pSkinObject->OnEndHook();

	delete pSkinObjectGuard;
	pSkinObject->InternalRelease();

	return bResult;
}
