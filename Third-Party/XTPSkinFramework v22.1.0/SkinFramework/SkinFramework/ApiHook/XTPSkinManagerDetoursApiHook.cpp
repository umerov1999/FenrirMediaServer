// XTPSkinManagerDetoursApiHook.cpp: implementation of the CXTPSkinManagerDetoursApiHook class.
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

#include "stdafx.h"

#include "Common/XTPTypeId.h"
#include "Common/XTPCasting.h"

#include "XTPSkinManagerApiHookConfig.h"

#ifdef XTP_SKINFRAMEWORK_USE_DETOURS_API_HOOK

#	if _MSC_VER >= 1900
// warning C4091 : 'typedef ' : ignored on left of '' when no variable is declared
#		pragma warning(push)
#		pragma warning(disable : 4091)
#	endif

#	include "Common/Base/Diagnostic/XTPDisableAdvancedWarnings.h"

#	include <imagehlp.h>
#	pragma comment(lib, "imagehlp.lib")

#	include <shlwapi.h>
#	pragma comment(lib, "shlwapi.lib")

#	include "SkinFramework/ApiHook/Detours/detours.h"
using namespace XTPDetours;

#	include "Common/Base/Diagnostic/XTPEnableAdvancedWarnings.h"

#	if _MSC_VER >= 1900
#		pragma warning(pop)
#	endif

#	include "Common/XTPCasting.h"
#	include "Common/XTPFramework.h"
#	include "Common/XTPVC80Helpers.h"
#	include "Common/XTPSystemHelpers.h"
#	include "Common/XTPSynchro.h"
#	include "Common/XTPApplication.h"
#	include "Common/XTPSingleton.h"
#	include "Common/XTPGdiObjects.h"
#	include "Common/XTPWinThemeWrapper.h"
#	include "Common/XTPDrawHelpers.h"
#	include "Common/ScrollBar/XTPScrollInfo.h"
#	include "Common/ScrollBar/XTPScrollBase.h"
#	include "Common/XTPResourceManager.h"
#	include "Common/XTPMacros.h"

#	include "SkinFramework/XTPSkinManager.h"
#	include "SkinFramework/XTPSkinManagerSchema.h"
#	include "SkinFramework/XTPSkinObject.h"
#	include "SkinFramework/XTPSkinObjectFrame.h"
#	include "SkinFramework/XTPSkinObjectMenu.h"
#	include "SkinFramework/XTPSkinDrawTools.h"
#	include "SkinFramework/XTPSkinManagerModuleList.h"
#	include "SkinFramework/XTPSkinManagerResource.h"
#	include "SkinFramework/ApiHook/XTPSkinManagerApiHookBase.h"
#	include "SkinFramework/ApiHook/XTPSkinManagerDetoursApiHook.h"

#	include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

#	ifdef _DEBUG
#		define new DEBUG_NEW
#		undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#	endif

EXTERN_C BOOL WINAPI XTPIsWow64Process(HANDLE hProcess, BOOL* pfIsWow64)
{
	typedef BOOL(WINAPI * LPFN_ISWOW64PROCESS)(HANDLE, PBOOL);

	static LPFN_ISWOW64PROCESS pfnIsWow64Process = NULL;
	if (NULL != pfnIsWow64Process)
	{
		CXTPModuleHandle hKernel32(_T("kernel32.dll"));
		if (NULL == hKernel32.GetHandle())
			return FALSE;

		pfnIsWow64Process = XTPToFunctionPtr<LPFN_ISWOW64PROCESS>(
			::GetProcAddress(hKernel32, "IsWow64Process"));
	}

	if (NULL == pfnIsWow64Process)
		return FALSE;

	return pfnIsWow64Process(hProcess, pfIsWow64);
}

template<class Char>
static int XTPCompareStringNoCase(const Char* dst, const Char* src)
{
	int f, l;
	do
	{
		f = (WORD)(*(dst++));
		if ((f >= 'A') && (f <= 'Z'))
			f -= ('A' - 'a');

		l = (WORD)(*(src++));
		if ((l >= 'A') && (l <= 'Z'))
			l -= ('A' - 'a');
	} while (f && (f == l));

	return (f - l);
}

//////////////////////////////////////////////////////////////////////////
// CXTPSkinManagerDetoursApiFunction

CXTPSkinManagerDetoursApiFunction::CXTPSkinManagerDetoursApiFunction(
	CXTPSkinManagerDetoursApiHook* pApiHook, HMODULE hModule, LPCSTR pszFuncName, PVOID pfnOrig,
	PVOID pfnHook)
	: m_pApiHook(pApiHook)
	, m_hModule(hModule)
	, m_pfnOrig(pfnOrig)
	, m_pfnHook(pfnHook)
	, m_pfnTarget(NULL)
{
	ASSERT(NULL != pApiHook);
	ASSERT(NULL != pszFuncName);
	ASSERT(NULL != pfnOrig);
	ASSERT(NULL != pfnHook);
	ASSERT((strlen(pszFuncName) + 1) < _countof(m_szFuncName));

#	if (_MSC_VER > 1310) // VS2005
	strcpy_s(m_szFuncName, _countof(m_szFuncName), pszFuncName);
#	else
	strcpy(m_szFuncName, pszFuncName);
#	endif
}

CXTPSkinManagerDetoursApiFunction::~CXTPSkinManagerDetoursApiFunction()
{
	ASSERT("The hook must be released prior to function destruction" && !IsHooked());
}

BOOL CXTPSkinManagerDetoursApiFunction::Hook()
{
	ASSERT(!IsHooked());

	XTP_GUARD_SHARED(CXTPSkinManagerDetoursApiFunction, this)
	{
		VERIFY(NO_ERROR == DetourTransactionBegin());
		VERIFY(NO_ERROR == DetourUpdateThread(GetCurrentThread()));

		m_pfnTarget = m_pfnOrig;

		if (ERROR_SUCCESS != DetourAttach(&m_pfnTarget, m_pfnHook))
		{
			m_pfnTarget = NULL;
			TRACE(_T("Unable to hook function '%s'\n"), m_szFuncName);
			ASSERT(!"Unable to hook function");
		}

		VERIFY(NO_ERROR == DetourTransactionCommit());
	}

	return IsHooked();
}

BOOL CXTPSkinManagerDetoursApiFunction::Unhook()
{
	XTP_GUARD_SHARED(CXTPSkinManagerDetoursApiFunction, this)
	{
		if (IsHooked())
		{
			VERIFY(NO_ERROR == DetourTransactionBegin());
			VERIFY(NO_ERROR == DetourUpdateThread(GetCurrentThread()));
			VERIFY(NO_ERROR == DetourDetach(&m_pfnTarget, m_pfnHook));
			VERIFY(NO_ERROR == DetourTransactionCommit());

			m_pfnTarget = NULL;
		}

		return !IsHooked();
	}
}

/////////////////////////////////////////////////////////////////////////////
// CXTPSkinManagerDetoursApiHook

IMPLEMENT_DYNAMIC(CXTPSkinManagerDetoursApiHook, CXTPSkinManagerApiHookBase)

BOOL CXTPSkinManagerDetoursApiHook::m_bInitialized = FALSE;

const CXTPSkinManagerDetoursApiHook::APIHOOKINFO CXTPSkinManagerDetoursApiHook::m_apiHookInfos[] = {
#	define XTPSFHOOKINFO_(module, name, options, version, depends)                                \
		{                                                                                          \
			xtpSkinDetoursApi##name, module, #name,                                                \
				reinterpret_cast<LPVOID>(&CXTPSkinManagerDetoursApiHook::OnHook##name), options,   \
				version, xtpSkinDetoursApi##depends                                                \
		}
#	define XTPSFHOOKINFO(module, name, options, version)                                          \
		{                                                                                          \
			xtpSkinDetoursApi##name, module, #name,                                                \
				reinterpret_cast<LPVOID>(&CXTPSkinManagerDetoursApiHook::OnHook##name), options,   \
				version, XTP_SKIN_DETOURS_API_HOOK_COUNT                                           \
		}

	XTPSFHOOKINFO("KERNEL32.DLL", GetProcAddress, 0, NULL),
	XTPSFHOOKINFO("KERNEL32.DLL", GetModuleHandleA, 0, NULL),
	XTPSFHOOKINFO("KERNEL32.DLL", GetModuleHandleW, 0, NULL),
	XTPSFHOOKINFO("KERNEL32.DLL", GetModuleHandleExA, 0, NULL),
	XTPSFHOOKINFO("KERNEL32.DLL", GetModuleHandleExW, 0, NULL),
	XTPSFHOOKINFO("KERNEL32.DLL", CreateThread, 0, NULL),

	XTPSFHOOKINFO("USER32.DLL", RegisterClassA, 0, NULL),
	XTPSFHOOKINFO("USER32.DLL", RegisterClassW, 0, NULL),
	XTPSFHOOKINFO("USER32.DLL", DefWindowProcA, 0, NULL),
	XTPSFHOOKINFO("USER32.DLL", DefWindowProcW, 0, NULL),
	XTPSFHOOKINFO("USER32.DLL", DefFrameProcA, 0, NULL),
	XTPSFHOOKINFO("USER32.DLL", DefFrameProcW, 0, NULL),
	XTPSFHOOKINFO("USER32.DLL", DefDlgProcA, 0, NULL),
	XTPSFHOOKINFO("USER32.DLL", DefDlgProcW, 0, NULL),
	XTPSFHOOKINFO("USER32.DLL", DefMDIChildProcA, 0, NULL),
	XTPSFHOOKINFO("USER32.DLL", DefMDIChildProcW, 0, NULL),
	XTPSFHOOKINFO("USER32.DLL", CallWindowProcA, 0, NULL),
	XTPSFHOOKINFO("USER32.DLL", CallWindowProcW, 0, NULL),

	XTPSFHOOKINFO("DWMAPI.DLL", DwmExtendFrameIntoClientArea, 0,
				  &CXTPSystemVersion::IsWin7OrGreater),

	XTPSFHOOKINFO("USER32.DLL", GetSysColor, xtpSkinApplyColors, NULL),
	XTPSFHOOKINFO("USER32.DLL", GetSysColorBrush, xtpSkinApplyColors, NULL),
	XTPSFHOOKINFO("USER32.DLL", DrawEdge, xtpSkinApplyColors, NULL),
	XTPSFHOOKINFO("USER32.DLL", FillRect, xtpSkinApplyColors, NULL),
	XTPSFHOOKINFO("GDI32.DLL", DeleteObject, xtpSkinApplyColors, NULL),

	XTPSFHOOKINFO("UXTHEME.DLL", OpenThemeData, xtpSkinApplyColors, NULL),
	XTPSFHOOKINFO("UXTHEME.DLL", OpenThemeDataForDpi, xtpSkinApplyColors, NULL),
	XTPSFHOOKINFO("UXTHEME.DLL", OpenThemeDataEx, xtpSkinApplyColors, NULL),
	XTPSFHOOKINFO("UXTHEME.DLL", DrawThemeBackground, xtpSkinApplyColors, NULL),
	XTPSFHOOKINFO("UXTHEME.DLL", CloseThemeData, xtpSkinApplyColors, NULL),
	XTPSFHOOKINFO("UXTHEME.DLL", GetThemeColor, xtpSkinApplyColors, NULL),
	XTPSFHOOKINFO("UXTHEME.DLL", GetThemeInt, xtpSkinApplyColors, NULL),
	XTPSFHOOKINFO("UXTHEME.DLL", IsAppThemed, xtpSkinApplyColors, NULL),
	XTPSFHOOKINFO("UXTHEME.DLL", IsThemeActive, xtpSkinApplyColors, NULL),
	XTPSFHOOKINFO("UXTHEME.DLL", GetCurrentThemeName, xtpSkinApplyColors, NULL),
	XTPSFHOOKINFO("UXTHEME.DLL", GetThemeSysBool, xtpSkinApplyColors, NULL),
	XTPSFHOOKINFO("UXTHEME.DLL", GetThemeSysColor, xtpSkinApplyColors, NULL),
	XTPSFHOOKINFO("UXTHEME.DLL", GetThemePartSize, xtpSkinApplyColors, NULL),

	XTPSFHOOKINFO("USER32.DLL", DrawFrameControl, xtpSkinApplyColors, NULL),
	XTPSFHOOKINFO("USER32.DLL", SetScrollInfo, xtpSkinApplyFrame, NULL),
	XTPSFHOOKINFO("USER32.DLL", SetScrollPos, xtpSkinApplyFrame, NULL),
	XTPSFHOOKINFO("USER32.DLL", GetScrollInfo, xtpSkinApplyFrame, NULL),
	XTPSFHOOKINFO("USER32.DLL", EnableScrollBar, xtpSkinApplyFrame, NULL),
	XTPSFHOOKINFO("USER32.DLL", DrawMenuBar, xtpSkinApplyFrame, NULL),

	XTPSFHOOKINFO("USER32.DLL", SystemParametersInfoA, xtpSkinApplyMetrics | xtpSkinApplyFrame,
				  NULL),
	XTPSFHOOKINFO("USER32.DLL", SystemParametersInfoW, xtpSkinApplyMetrics | xtpSkinApplyFrame,
				  NULL),
	XTPSFHOOKINFO("USER32.DLL", AdjustWindowRectEx, xtpSkinApplyMetrics | xtpSkinApplyFrame, NULL),
	XTPSFHOOKINFO("USER32.DLL", AdjustWindowRect, xtpSkinApplyMetrics | xtpSkinApplyFrame, NULL),
	XTPSFHOOKINFO("USER32.DLL", GetSystemMetrics, xtpSkinApplyMetrics | xtpSkinApplyFrame, NULL),

	XTPSFHOOKINFO("USER32.DLL", TrackPopupMenu, xtpSkinApplyMenus, NULL),
	XTPSFHOOKINFO("WIN32U.DLL", NtUserTrackPopupMenuEx, xtpSkinApplyMenus,
				  &CXTPSystemVersion::IsWin7OrGreater),
	XTPSFHOOKINFO_("USER32.DLL", TrackPopupMenuEx, xtpSkinApplyMenus, NULL, NtUserTrackPopupMenuEx),
	XTPSFHOOKINFO("USER32.DLL", GetMenuItemRect, xtpSkinApplyMenus, NULL)

#	undef XTPSFHOOKINFO
#	undef XTPSFHOOKINFOEX
};

CXTPModuleHandle CXTPSkinManagerDetoursApiHook::m_hUxTheme(_T("UXTHEME.DLL"));

CXTPSkinManagerDetoursApiHook::CXTPSkinManagerDetoursApiHook()
	: CXTPSkinManagerApiHookBase(FALSE)
	, m_nLockCondition(0)
{
	ZeroMemory(&m_arrFunctions, sizeof(m_arrFunctions));
}

CXTPSkinManagerDetoursApiHook::~CXTPSkinManagerDetoursApiHook()
{
	FinalizeHookManagement();
	UnhookAllFunctions(TRUE);
}

void CXTPSkinManagerDetoursApiHook::InitializeHookManagement()
{
	XTP_GUARD_EXCLUSIVE(CXTPSkinManagerDetoursApiHook, this)
	{
		if (m_bInitialized)
			return;

		XTP_GUARD_SHARED(CXTPSkinManager, XTPSkinManager())
		{
			for (int i = 0; i < _countof(m_apiHookInfos); ++i)
			{
				const APIHOOKINFO& hookInfo = m_apiHookInfos[i];

				if (NULL != hookInfo.pfnVersionCheck
					&& !(XTPSystemVersion()->*hookInfo.pfnVersionCheck)())
					continue;

				if (0 != hookInfo.nApplyOptions
					&& 0 == (XTPSkinManager()->GetApplyOptions() & hookInfo.nApplyOptions))
					continue;

				if (XTP_SKIN_DETOURS_API_HOOK_COUNT != hookInfo.nDependsOn)
				{
					CXTPSkinManagerDetoursApiFunction* pDependsOnHook =
						m_arrFunctions[hookInfo.nDependsOn];
					if (NULL != pDependsOnHook && pDependsOnHook->IsHooked())
						continue;
				}

				Hook(hookInfo.nIndex, hookInfo.lpModuleName, hookInfo.lpRoutineName,
					 hookInfo.pfnHook);
			}

			m_bInitialized = TRUE;
		}
	}
}

void CXTPSkinManagerDetoursApiHook::FinalizeHookManagement()
{
	XTP_GUARD_EXCLUSIVE(CXTPSkinManagerDetoursApiHook, this)
	{
		if (!m_bInitialized)
			return;

		UnhookAllFunctions(FALSE);

		m_bInitialized = FALSE;
	}
}

void CXTPSkinManagerDetoursApiHook::ExcludeModule(LPCTSTR lpszModule, BOOL bWin9x)
{
	UNREFERENCED_PARAMETER(lpszModule);
	UNREFERENCED_PARAMETER(bWin9x);
}

LPVOID CXTPSkinManagerDetoursApiHook::GetOriginalFunctionAddress(int nFunction) const
{
	CXTPSkinManagerDetoursApiFunction* pFunction =
		const_cast<CXTPSkinManagerDetoursApiHook*>(this)->GetHookedFunction(
			static_cast<XTPSkinFrameworkDetoursApiFunctionIndex>(nFunction));
	return (NULL != pFunction ? pFunction->GetOriginal() : NULL);
}

BOOL CXTPSkinManagerDetoursApiHook::AdjustWindowRectExOrig(LPRECT lpRect, DWORD dwStyle, BOOL bMenu,
														   DWORD dwExStyle)
{
	CXTPSkinManagerDetoursApiFunction* pFunction = EnterHookedCall(
		xtpSkinDetoursApiAdjustWindowRectEx);

	if (NULL == pFunction)
		return FALSE;

	typedef BOOL(WINAPI * LPFNADJUSTWINDOWRECTEX)(LPRECT lpRect, DWORD dwStyle, BOOL bMenu,
												  DWORD dwExStyle);
	return ((LPFNADJUSTWINDOWRECTEX)pFunction->GetOriginal())(lpRect, dwStyle, bMenu, dwExStyle);
}

LRESULT CXTPSkinManagerDetoursApiHook::CallWindowProcOrig(WNDPROC lpPrevWndFunc, HWND hWnd,
														  UINT Msg, WPARAM wParam, LPARAM lParam)
{
	XTP_SKINFRAMEWORK_ASSERT_WINDOW_THREAD(hWnd);

#	ifdef _UNICODE
	LPVOID pfnOrig = GetOriginalFunctionAddress(xtpSkinDetoursApiCallWindowProcW);
#	else
	LPVOID pfnOrig = GetOriginalFunctionAddress(xtpSkinDetoursApiCallWindowProcA);
#	endif

	if (NULL != pfnOrig)
	{
		typedef LRESULT(WINAPI * LPFNCALLWINDOWPROC)(WNDPROC lpPrevWndFunc, HWND hWnd, UINT Msg,
													 WPARAM wParam, LPARAM lParam);
		return ((LPFNCALLWINDOWPROC)pfnOrig)(lpPrevWndFunc, hWnd, Msg, wParam, lParam);
	}

	return ::CallWindowProc(lpPrevWndFunc, hWnd, Msg, wParam, lParam);
}

LPVOID CXTPSkinManagerDetoursApiHook::GetOpenThemeDataOriginalPtr() const
{
	return GetOriginalFunctionAddress(xtpSkinDetoursApiOpenThemeData);
}

LPVOID CXTPSkinManagerDetoursApiHook::GetCloseThemeDataOriginalPtr() const
{
	return GetOriginalFunctionAddress(xtpSkinDetoursApiCloseThemeData);
}

void CXTPSkinManagerDetoursApiHook::ConnectWrapper(CXTPWinThemeWrapper* pThemeWrapper)
{
	pThemeWrapper->CreateSharedData();
	pThemeWrapper->SetThemeWrapperPtr(xtpWrapperOpenThemeData,
									  &CXTPSkinManagerDetoursApiHook::OnHookOpenThemeData);
	pThemeWrapper->SetThemeWrapperPtr(xtpWrapperDrawThemeBackground,
									  &CXTPSkinManagerDetoursApiHook::OnHookDrawThemeBackground);
	pThemeWrapper->SetThemeWrapperPtr(xtpWrapperCloseThemeData,
									  &CXTPSkinManagerDetoursApiHook::OnHookCloseThemeData);
	pThemeWrapper->SetThemeWrapperPtr(xtpWrapperGetThemeColor,
									  &CXTPSkinManagerDetoursApiHook::OnHookGetThemeColor);
	pThemeWrapper->SetThemeWrapperPtr(xtpWrapperGetThemeInt,
									  &CXTPSkinManagerDetoursApiHook::OnHookGetThemeInt);
	pThemeWrapper->SetThemeWrapperPtr(xtpWrapperIsAppThemed,
									  &CXTPSkinManagerDetoursApiHook::OnHookIsAppThemed);
	pThemeWrapper->SetThemeWrapperPtr(xtpWrapperIsThemeActive,
									  &CXTPSkinManagerDetoursApiHook::OnHookIsThemeActive);
	pThemeWrapper->SetThemeWrapperPtr(xtpWrapperGetCurrentThemeName,
									  &CXTPSkinManagerDetoursApiHook::OnHookGetCurrentThemeName);
	pThemeWrapper->SetThemeWrapperPtr(xtpWrapperGetThemeSysBool,
									  &CXTPSkinManagerDetoursApiHook::OnHookGetThemeSysBool);
	pThemeWrapper->SetThemeWrapperPtr(xtpWrapperGetThemeSysColor,
									  &CXTPSkinManagerDetoursApiHook::OnHookGetThemeSysColor);
	pThemeWrapper->SetThemeWrapperPtr(xtpWrapperGetThemePartSize,
									  &CXTPSkinManagerDetoursApiHook::OnHookGetThemePartSize);
}

BOOL CXTPSkinManagerDetoursApiHook::IsModuleExcluded(HMODULE hModule) const
{
	UNREFERENCED_PARAMETER(hModule);
	return FALSE;
}

CXTPSkinManagerDetoursApiHook* CXTPSkinManagerDetoursApiHook::GetInstance()
{
	CXTPSkinManagerApiHookBase* pBase = XTPSkinManager()->GetApiHook();
	ASSERT_KINDOF(CXTPSkinManagerDetoursApiHook, pBase);
	return static_cast<CXTPSkinManagerDetoursApiHook*>(pBase);
}

void CXTPSkinManagerDetoursApiHook::UnhookAllFunctions(BOOL bFinal)
{
	XTP_GUARD_EXCLUSIVE(CXTPSkinManagerDetoursApiHook, this)
	{
		// Unhoook all functions
		for (int i = 0; i < XTP_SKIN_DETOURS_API_HOOK_COUNT; i++)
		{
			CXTPSkinManagerDetoursApiFunction*& pHook = m_arrFunctions[i];
			if (NULL != pHook && pHook->IsHooked())
			{
				pHook->Unhook();
				ASSERT(!pHook->IsHooked());
			}
		}

		// Delete all functions' instances on final release.
		if (bFinal)
		{
			for (int i = 0; i < XTP_SKIN_DETOURS_API_HOOK_COUNT; i++)
			{
				CXTPSkinManagerDetoursApiFunction*& pHook = m_arrFunctions[i];
				if (NULL != pHook)
				{
					delete pHook;
					pHook = NULL;
				}
			}
		}
	}
}

CXTPSkinManagerDetoursApiFunction*
	CXTPSkinManagerDetoursApiHook::Hook(XTPSkinFrameworkDetoursApiFunctionIndex nIndex,
										LPCSTR pszModuleName, LPCSTR pszFuncName, LPVOID pfnHook)
{
	HMODULE hModule = ::GetModuleHandleA(pszModuleName);
	LPVOID pfnOrig	= NULL;
	BOOL bDllLoaded = FALSE;

	if (NULL == hModule)
	{
		hModule = ::LoadLibraryA(pszModuleName);
		if (NULL == hModule)
			return NULL;

		bDllLoaded = TRUE;
	}

	pfnOrig = ::GetProcAddress(hModule, pszFuncName);

	if (NULL == pfnOrig)
	{
		if (bDllLoaded)
			::FreeLibrary(hModule);

		return NULL;
	}

	return AddHook(nIndex, hModule, pszFuncName, pfnOrig, pfnHook);
}

CXTPSkinManagerDetoursApiFunction* CXTPSkinManagerDetoursApiHook::AddHook(
	XTPSkinFrameworkDetoursApiFunctionIndex nIndex, HMODULE hModule, LPCSTR pszFuncName,
	LPVOID pfnOrig, LPVOID pfnHook)
{
	ASSERT(nIndex < XTP_SKIN_DETOURS_API_HOOK_COUNT);
	if (nIndex >= XTP_SKIN_DETOURS_API_HOOK_COUNT)
		return NULL;

	XTP_GUARD_EXCLUSIVE(CXTPSkinManagerDetoursApiHook, this)
	{
		CXTPSkinManagerDetoursApiFunction* pHook = m_arrFunctions[nIndex];
		if (NULL == pHook)
		{
			pHook = new CXTPSkinManagerDetoursApiFunction(this, hModule, pszFuncName, pfnOrig,
														  pfnHook);
			m_arrFunctions[nIndex] = pHook;
		}

		for (int i = 0; i < nIndex; ++i)
		{
			CXTPSkinManagerDetoursApiFunction* pExistingHook = m_arrFunctions[i];
			if (NULL != pExistingHook && pExistingHook->GetTrueOriginal() == pHook->GetOriginal())
			{
				// Another function already hooks the same location.
				// Leave the current hook unhooked.
				return pHook;
			}
		}

		pHook->Hook();

		return pHook;
	}
}

CXTPSkinManagerDetoursApiFunction*
	CXTPSkinManagerDetoursApiHook::GetHookedFunction(XTPSkinFrameworkDetoursApiFunctionIndex nIndex)
{
	ASSERT(nIndex < XTP_SKIN_DETOURS_API_HOOK_COUNT);

	if (nIndex < 0 || nIndex >= XTP_SKIN_DETOURS_API_HOOK_COUNT)
		return NULL;

	return XTPAccessShared(this, m_nLockCondition)->m_arrFunctions[nIndex];
}

CXTPSkinManagerDetoursApiFunction*
	CXTPSkinManagerDetoursApiHook::GetHookedFunction(HMODULE hModule, LPCSTR pszFuncName)
{
	XTP_GUARD_SHARED(CXTPSkinManagerDetoursApiHook, this)
	{
		for (int i = 0; i < XTP_SKIN_DETOURS_API_HOOK_COUNT; i++)
		{
			CXTPSkinManagerDetoursApiFunction* pHook = m_arrFunctions[i];
			if (NULL == pHook)
				continue;

			XTP_GUARD_SHARED(CXTPSkinManagerDetoursApiFunction, pHook)
			{
				if (pHook->GetModule() == hModule
					&& XTPCompareStringNoCase(pHook->GetFunctionName(), pszFuncName) == 0)
				{
					return pHook;
				}
			}
		}
	}

	return NULL;
}

struct XTP_SKINFRAMEWORK_THREADPROCPARAMETER
{
	LPTHREAD_START_ROUTINE lpStartAddress;
	LPVOID lpParameter;
};

DWORD WINAPI CXTPSkinManagerDetoursApiHook::ThreadProcHook(LPVOID lpThreadData)
{
	XTP_SKINFRAMEWORK_MANAGE_STATE();

	XTP_SKINFRAMEWORK_THREADPROCPARAMETER* parameter = (XTP_SKINFRAMEWORK_THREADPROCPARAMETER*)
		lpThreadData;
	LPTHREAD_START_ROUTINE lpStartAddress = parameter->lpStartAddress;
	LPVOID lpParam						  = parameter->lpParameter;
	delete lpThreadData;

	XTPAccessShared(XTPSkinManager())->EnableCurrentThread();

	return (lpStartAddress)(lpParam);
}

CXTPSkinManagerDetoursApiFunction* AFX_CDECL
	CXTPSkinManagerDetoursApiHook::EnterHookedCall(HMODULE hModule, LPCSTR pszFuncName)
{
	CXTPSkinManagerDetoursApiFunction* pFunction = GetInstance()->GetHookedFunction(hModule,
																					pszFuncName);
	if (!(NULL != pFunction && NULL != pFunction->GetOriginal()))
	{
		SetLastError(ERROR_PROC_NOT_FOUND);
		pFunction = NULL;
	}

	return pFunction;
}

CXTPSkinManagerDetoursApiFunction* AFX_CDECL
	CXTPSkinManagerDetoursApiHook::EnterHookedCall(XTPSkinFrameworkDetoursApiFunctionIndex nIndex)
{
	CXTPSkinManagerDetoursApiFunction* pFunction = GetInstance()->GetHookedFunction(nIndex);
	if (!(NULL != pFunction && NULL != pFunction->GetOriginal()))
	{
		SetLastError(ERROR_PROC_NOT_FOUND);
		pFunction = NULL;
	}

	return pFunction;
}

DWORD WINAPI CXTPSkinManagerDetoursApiHook::OnHookGetSysColor(int nIndex)
{
	XTP_SKINFRAMEWORK_MANAGE_STATE();

	DWORD crColor = 0;

	CXTPSkinManagerDetoursApiFunction* pFunction = EnterHookedCall(xtpSkinDetoursApiGetSysColor);
	if (NULL != pFunction)
	{
		BOOL bDefaultProcessing = TRUE;

		if (m_bInitialized && pFunction->IsHooked())
		{
			XTP_GUARD_SHARED_(CXTPSkinManager, XTPSkinManager(), pSkinManager)
			{
				CXTPSkinManagerMetrics* pMetrics = XTPSkinManager()->GetMetrics();
				if (NULL != pMetrics && pSkinManager->IsEnabled())
				{
					crColor			   = pMetrics->GetColor(nIndex);
					bDefaultProcessing = (COLORREF_NULL == crColor);
				}
			}
		}

		if (bDefaultProcessing)
		{
			typedef int(WINAPI * LPFNGETSYSCOLOR)(int);
			crColor = XTPToDWORD(((LPFNGETSYSCOLOR)pFunction->GetOriginal())(nIndex));
		}
	}

	return crColor;
}

BOOL WINAPI CXTPSkinManagerDetoursApiHook::OnHookDeleteObject(HGDIOBJ hObject)
{
	XTP_SKINFRAMEWORK_MANAGE_STATE();

	CXTPSkinManagerDetoursApiFunction* pFunction = EnterHookedCall(xtpSkinDetoursApiDeleteObject);
	if (NULL == pFunction)
		return FALSE;

	if (m_bInitialized && pFunction->IsHooked())
	{
		CXTPSkinManagerMetrics* pMetrics = XTPSkinManager()->GetMetrics();

		if (NULL != pMetrics && pMetrics->IsMetricObject(hObject))
			return FALSE;
	}

	typedef BOOL(WINAPI * LPFNDELETEOBJECT)(HGDIOBJ);
	return ((LPFNDELETEOBJECT)pFunction->GetOriginal())(hObject);
}

HBRUSH WINAPI CXTPSkinManagerDetoursApiHook::OnHookGetSysColorBrush(int nIndex)
{
	XTP_SKINFRAMEWORK_MANAGE_STATE();

	HBRUSH hBrush = NULL;

	CXTPSkinManagerDetoursApiFunction* pFunction = EnterHookedCall(
		xtpSkinDetoursApiGetSysColorBrush);
	if (NULL != pFunction)
	{
		BOOL bDefaultProcessing = TRUE;

		if (m_bInitialized && pFunction->IsHooked())
		{
			XTP_GUARD_SHARED_(CXTPSkinManager, XTPSkinManager(), pSkinManager)
			{
				CXTPSkinManagerMetrics* pMetrics = XTPSkinManager()->GetMetrics();
				if (NULL != pMetrics && pSkinManager->IsEnabled())
				{
					hBrush			   = pMetrics->GetBrush(nIndex);
					bDefaultProcessing = (NULL == hBrush);
				}
			}
		}

		if (bDefaultProcessing)
		{
			typedef HBRUSH(WINAPI * LPFNGETSYSCOLORBRUSH)(int);
			hBrush = ((LPFNGETSYSCOLORBRUSH)pFunction->GetOriginal())(nIndex);
		}
	}

	return hBrush;
}

int WINAPI CXTPSkinManagerDetoursApiHook::OnHookSetScrollPos(HWND hwnd, int nBar, int nPos,
															 BOOL redraw)
{
	int nResult = 0;

	XTP_SKINFRAMEWORK_MANAGE_STATE();

	CXTPSkinManagerDetoursApiFunction* pFunction = EnterHookedCall(xtpSkinDetoursApiSetScrollPos);
	if (NULL != pFunction)
	{
		typedef int(WINAPI * LPFNSETSCROLLPOS)(HWND hWnd, int nBar, int nPos, BOOL bRedraw);
		BOOL bDefaultProcessing = TRUE;

		if (m_bInitialized && pFunction->IsHooked())
		{
			XTP_GUARD_SHARED_(CXTPSkinManager, XTPSkinManager(), pSkinManager)
			{
				if (pSkinManager->IsEnabled())
				{
					CXTPSkinObjectFrame* pFrame = (CXTPSkinObjectFrame*)pSkinManager->Lookup(hwnd);

					if (NULL != pFrame)
					{
						XTP_GUARD_SHARED(CXTPSkinObjectFrame, pFrame)
						{
							nResult = ((LPFNSETSCROLLPOS)pFunction->GetOriginal())(hwnd, nBar, nPos,
																				   FALSE);
							bDefaultProcessing = FALSE;

							if (redraw)
							{
								if (nBar == SB_CTL)
									InvalidateRect(hwnd, NULL, FALSE);
								else
									pFrame->RedrawScrollBar(nBar);
							}
						}
					}
				}
			}
		}

		if (bDefaultProcessing)
			nResult = ((LPFNSETSCROLLPOS)pFunction->GetOriginal())(hwnd, nBar, nPos, redraw);
	}

	return nResult;
}

int WINAPI CXTPSkinManagerDetoursApiHook::OnHookSetScrollInfo(HWND hwnd, int nBar,
															  LPCSCROLLINFO lpsi, BOOL redraw)
{
	int nResult = 0;

	XTP_SKINFRAMEWORK_MANAGE_STATE();

	CXTPSkinManagerDetoursApiFunction* pFunction = EnterHookedCall(xtpSkinDetoursApiSetScrollInfo);
	if (NULL != pFunction)
	{
		typedef int(WINAPI * LPFNSETSCROLLINFO)(HWND hwnd, int fnBar, LPCSCROLLINFO lpsi,
												BOOL fRedraw);
		BOOL bDefaultProcessing = TRUE;

		if (m_bInitialized && pFunction->IsHooked())
		{
			XTP_GUARD_SHARED_(CXTPSkinManager, XTPSkinManager(), pSkinManager)
			{
				if (pSkinManager->IsEnabled())
				{
					CXTPSkinObjectFrame* pFrame = (CXTPSkinObjectFrame*)XTPSkinManager()->Lookup(
						hwnd);

					if (!(!pFrame || (nBar == SB_CTL) || !redraw))
					{
						XTP_GUARD_SHARED(CXTPSkinObjectFrame, pFrame)
						{
							nResult = ((LPFNSETSCROLLINFO)pFunction->GetOriginal())(hwnd, nBar,
																					lpsi, FALSE);
							pFrame->RedrawScrollBar(nBar);
							bDefaultProcessing = FALSE;
						}
					}
				}
			}
		}

		if (bDefaultProcessing)
			nResult = ((LPFNSETSCROLLINFO)pFunction->GetOriginal())(hwnd, nBar, lpsi, redraw);
	}

	return nResult;
}

BOOL WINAPI CXTPSkinManagerDetoursApiHook::OnHookEnableScrollBar(HWND hWnd, UINT wSBflags,
																 UINT wArrows)
{
	BOOL bResult = FALSE;

	XTP_SKINFRAMEWORK_MANAGE_STATE();

	CXTPSkinManagerDetoursApiFunction* pFunction = EnterHookedCall(
		xtpSkinDetoursApiEnableScrollBar);
	if (NULL != pFunction)
	{
		typedef BOOL(WINAPI * LPFNENABLESCROLLBAR)(HWND hWnd, UINT wSBflags, UINT wArrows);
		BOOL bDefaultProcessing = TRUE;

		if (m_bInitialized && pFunction->IsHooked())
		{
			XTP_GUARD_SHARED_(CXTPSkinManager, XTPSkinManager(), pSkinManager)
			{
				if (pSkinManager->IsEnabled())
				{
					CXTPSkinObjectFrame* pFrame = (CXTPSkinObjectFrame*)pSkinManager->Lookup(hWnd);
					if (NULL != pFrame)
					{
						XTP_GUARD_SHARED(CXTPSkinObjectFrame, pFrame)
						{
							bResult = ((LPFNENABLESCROLLBAR)pFunction->GetOriginal())(hWnd,
																					  wSBflags,
																					  wArrows);
							bDefaultProcessing = FALSE;

							if (wSBflags == SB_BOTH || wSBflags == SB_HORZ)
								pFrame->RedrawScrollBar(SB_HORZ);

							if (wSBflags == SB_BOTH || wSBflags == SB_VERT)
								pFrame->RedrawScrollBar(SB_VERT);
						}
					}
				}
			}
		}

		if (bDefaultProcessing)
			bResult = ((LPFNENABLESCROLLBAR)pFunction->GetOriginal())(hWnd, wSBflags, wArrows);
	}

	return bResult;
}

BOOL WINAPI CXTPSkinManagerDetoursApiHook::OnHookGetScrollInfo(HWND hWnd, int nBar,
															   LPSCROLLINFO lpsi)
{
	BOOL bResult = FALSE;

	XTP_SKINFRAMEWORK_MANAGE_STATE();

	CXTPSkinManagerDetoursApiFunction* pFunction = EnterHookedCall(xtpSkinDetoursApiGetScrollInfo);
	if (NULL != pFunction)
	{
		typedef BOOL(WINAPI * LPFNGETSCROLLINFO)(HWND hwnd, int nBar, LPCSCROLLINFO lpsi);
		bResult = ((LPFNGETSCROLLINFO)pFunction->GetOriginal())(hWnd, nBar, lpsi);

		if (m_bInitialized && pFunction->IsHooked())
		{
			if (NULL != lpsi && (lpsi->fMask & SIF_TRACKPOS))
			{
				XTP_GUARD_SHARED_(CXTPSkinManager, XTPSkinManager(), pSkinManager)
				{
					if (pSkinManager->IsEnabled())
					{
						CXTPSkinObject* pSink = XTPSkinManager()->Lookup(hWnd);

						if (NULL != pSink)
						{
							XTP_GUARD_SHARED(CXTPSkinObject, pSink)
							{
								XTP_SKINSCROLLBAR_TRACKINFO* pSBTrack =
									((CXTPSkinObjectFrame*)pSink)->GetScrollBarTrackInfo();

								if (NULL != pSBTrack && (pSBTrack->nBar == nBar))
								{
									// posNew is in the context of psbiSB's window and bar code
									lpsi->nTrackPos = pSBTrack->posNew;
								}
							}
						}
					}
				}
			}
		}
	}

	return bResult;
}

LRESULT WINAPI CXTPSkinManagerDetoursApiHook::OnHookCallWindowProcW(WNDPROC lpPrevWndFunc,
																	HWND hWnd, UINT Msg,
																	WPARAM wParam, LPARAM lParam)
{
	XTP_SKINFRAMEWORK_MANAGE_STATE();

	CXTPSkinManagerDetoursApiFunction* pFunction = EnterHookedCall(
		xtpSkinDetoursApiCallWindowProcW);
	if (NULL == pFunction)
		return 0;

	if (m_bInitialized && pFunction->IsHooked()
		&& !XTPSkinManager()->IsWindowFilteredOut(hWnd, NULL, NULL, TRUE))
	{
		LRESULT lResult = 0;

		if (NULL != lpPrevWndFunc
			&& GetInstance()->CallHookDefWindowProc(hWnd, pFunction->GetOriginal(),
													xtpSkinDefaultCallWindowProc, lpPrevWndFunc,
													Msg, wParam, lParam, lResult))
		{
			return lResult;
		}
	}

	XTP_SKINFRAMEWORK_ORIGINAL_STATE()
	{
		typedef LRESULT(WINAPI * LPFNCALLWINDOWPROC)(WNDPROC lpPrevWndFunc, HWND hWnd, UINT Msg,
													 WPARAM wParam, LPARAM lParam);
		return ((LPFNCALLWINDOWPROC)pFunction->GetOriginal())(lpPrevWndFunc, hWnd, Msg, wParam,
															  lParam);
	}
}

LRESULT WINAPI CXTPSkinManagerDetoursApiHook::OnHookCallWindowProcA(WNDPROC lpPrevWndFunc,
																	HWND hWnd, UINT Msg,
																	WPARAM wParam, LPARAM lParam)
{
	XTP_SKINFRAMEWORK_MANAGE_STATE();

	CXTPSkinManagerDetoursApiFunction* pFunction = EnterHookedCall(
		xtpSkinDetoursApiCallWindowProcA);
	if (NULL == pFunction)
		return 0;

	if (m_bInitialized && pFunction->IsHooked()
		&& !XTPSkinManager()->IsWindowFilteredOut(hWnd, NULL, NULL, TRUE))
	{
		LRESULT lResult = 0;

		if (NULL != lpPrevWndFunc
			&& GetInstance()->CallHookDefWindowProc(hWnd, pFunction->GetOriginal(),
													xtpSkinDefaultCallWindowProc, lpPrevWndFunc,
													Msg, wParam, lParam, lResult))
		{
			return lResult;
		}
	}

	XTP_SKINFRAMEWORK_ORIGINAL_STATE()
	{
		typedef LRESULT(WINAPI * LPFNCALLWINDOWPROC)(WNDPROC lpPrevWndFunc, HWND hWnd, UINT Msg,
													 WPARAM wParam, LPARAM lParam);
		return ((LPFNCALLWINDOWPROC)pFunction->GetOriginal())(lpPrevWndFunc, hWnd, Msg, wParam,
															  lParam);
	}
}

LRESULT WINAPI CXTPSkinManagerDetoursApiHook::OnHookDefWindowProcA(HWND hWnd, UINT Msg,
																   WPARAM wParam, LPARAM lParam)
{
	XTP_SKINFRAMEWORK_MANAGE_STATE();

	CXTPSkinManagerDetoursApiFunction* pFunction = EnterHookedCall(xtpSkinDetoursApiDefWindowProcA);
	if (NULL == pFunction)
		return 0;

	if (m_bInitialized && pFunction->IsHooked()
		&& !XTPSkinManager()->IsWindowFilteredOut(hWnd, NULL, NULL, TRUE))
	{
		LRESULT lResult = 0;
		if (GetInstance()->CallHookDefWindowProc(hWnd, pFunction->GetOriginal(),
												 xtpSkinDefaultDefWindowProc, NULL, Msg, wParam,
												 lParam, lResult))
		{
			return lResult;
		}
	}

	XTP_SKINFRAMEWORK_ORIGINAL_STATE()
	{
		typedef LRESULT(WINAPI * LPFNDEFWINDOWPROC)(HWND hWnd, UINT Msg, WPARAM wParam,
													LPARAM lParam);
		return ((LPFNDEFWINDOWPROC)pFunction->GetOriginal())(hWnd, Msg, wParam, lParam);
	}
}

LRESULT WINAPI CXTPSkinManagerDetoursApiHook::OnHookDefWindowProcW(HWND hWnd, UINT Msg,
																   WPARAM wParam, LPARAM lParam)
{
	XTP_SKINFRAMEWORK_MANAGE_STATE();

	CXTPSkinManagerDetoursApiFunction* pFunction = EnterHookedCall(xtpSkinDetoursApiDefWindowProcW);
	if (NULL == pFunction)
		return 0;

	if (m_bInitialized && pFunction->IsHooked()
		&& !XTPSkinManager()->IsWindowFilteredOut(hWnd, NULL, NULL, TRUE))
	{
		LRESULT lResult = 0;
		if (GetInstance()->CallHookDefWindowProc(hWnd, pFunction->GetOriginal(),
												 xtpSkinDefaultDefWindowProc, NULL, Msg, wParam,
												 lParam, lResult))
		{
			return lResult;
		}
	}

	XTP_SKINFRAMEWORK_ORIGINAL_STATE()
	{
		typedef LRESULT(WINAPI * LPFNDEFWINDOWPROC)(HWND hWnd, UINT Msg, WPARAM wParam,
													LPARAM lParam);
		return ((LPFNDEFWINDOWPROC)pFunction->GetOriginal())(hWnd, Msg, wParam, lParam);
	}
}

LRESULT WINAPI CXTPSkinManagerDetoursApiHook::OnHookDefFrameProcA(HWND hWnd, HWND hWndMDIClient,
																  UINT Msg, WPARAM wParam,
																  LPARAM lParam)
{
	XTP_SKINFRAMEWORK_MANAGE_STATE();

	CXTPSkinManagerDetoursApiFunction* pFunction = EnterHookedCall(xtpSkinDetoursApiDefFrameProcA);
	if (NULL == pFunction)
		return 0;

	if (m_bInitialized && pFunction->IsHooked()
		&& !XTPSkinManager()->IsWindowFilteredOut(hWnd, NULL, NULL, TRUE))
	{
		LRESULT lResult = 0;
		if (GetInstance()->CallHookDefWindowProc(hWnd, pFunction->GetOriginal(),
												 xtpSkinDefaultDefFrameProc, (LPVOID)hWndMDIClient,
												 Msg, wParam, lParam, lResult))
		{
			return lResult;
		}
	}

	XTP_SKINFRAMEWORK_ORIGINAL_STATE()
	{
		typedef LRESULT(WINAPI * LPFNDEFWINDOWPROC)(HWND hWnd, HWND hWndMDIClient, UINT Msg,
													WPARAM wParam, LPARAM lParam);
		return ((LPFNDEFWINDOWPROC)pFunction->GetOriginal())(hWnd, hWndMDIClient, Msg, wParam,
															 lParam);
	}
}

LRESULT WINAPI CXTPSkinManagerDetoursApiHook::OnHookDefFrameProcW(HWND hWnd, HWND hWndMDIClient,
																  UINT Msg, WPARAM wParam,
																  LPARAM lParam)
{
	XTP_SKINFRAMEWORK_MANAGE_STATE();

	CXTPSkinManagerDetoursApiFunction* pFunction = EnterHookedCall(xtpSkinDetoursApiDefFrameProcW);
	if (NULL == pFunction)
		return 0;

	if (m_bInitialized && pFunction->IsHooked()
		&& !XTPSkinManager()->IsWindowFilteredOut(hWnd, NULL, NULL, TRUE))
	{
		LRESULT lResult = 0;
		if (GetInstance()->CallHookDefWindowProc(hWnd, pFunction->GetOriginal(),
												 xtpSkinDefaultDefFrameProc, (LPVOID)hWndMDIClient,
												 Msg, wParam, lParam, lResult))
		{
			return lResult;
		}
	}

	XTP_SKINFRAMEWORK_ORIGINAL_STATE()
	{
		typedef LRESULT(WINAPI * LPFNDEFWINDOWPROC)(HWND hWnd, HWND hWndMDIClient, UINT Msg,
													WPARAM wParam, LPARAM lParam);
		return ((LPFNDEFWINDOWPROC)pFunction->GetOriginal())(hWnd, hWndMDIClient, Msg, wParam,
															 lParam);
	}
}

LRESULT WINAPI CXTPSkinManagerDetoursApiHook::OnHookDefDlgProcA(HWND hWnd, UINT Msg, WPARAM wParam,
																LPARAM lParam)
{
	XTP_SKINFRAMEWORK_MANAGE_STATE();

	CXTPSkinManagerDetoursApiFunction* pFunction = EnterHookedCall(xtpSkinDetoursApiDefDlgProcA);
	if (NULL == pFunction)
		return 0;

	if (m_bInitialized && pFunction->IsHooked()
		&& !XTPSkinManager()->IsWindowFilteredOut(hWnd, NULL, NULL, TRUE))
	{
		LRESULT lResult = 0;
		if (GetInstance()->CallHookDefWindowProc(hWnd, pFunction->GetOriginal(),
												 xtpSkinDefaultDefDlgProc, NULL, Msg, wParam,
												 lParam, lResult))
		{
			return lResult;
		}
	}

	XTP_SKINFRAMEWORK_ORIGINAL_STATE()
	{
		typedef LRESULT(WINAPI * LPFNDEFWINDOWPROC)(HWND hWnd, UINT Msg, WPARAM wParam,
													LPARAM lParam);
		return ((LPFNDEFWINDOWPROC)pFunction->GetOriginal())(hWnd, Msg, wParam, lParam);
	}
}

LRESULT WINAPI CXTPSkinManagerDetoursApiHook::OnHookDefDlgProcW(HWND hWnd, UINT Msg, WPARAM wParam,
																LPARAM lParam)
{
	XTP_SKINFRAMEWORK_MANAGE_STATE();

	CXTPSkinManagerDetoursApiFunction* pFunction = EnterHookedCall(xtpSkinDetoursApiDefDlgProcW);
	if (NULL == pFunction)
		return 0;

	if (m_bInitialized && pFunction->IsHooked()
		&& !XTPSkinManager()->IsWindowFilteredOut(hWnd, NULL, NULL, TRUE))
	{
		LRESULT lResult = 0;
		if (GetInstance()->CallHookDefWindowProc(hWnd, pFunction->GetOriginal(),
												 xtpSkinDefaultDefDlgProc, NULL, Msg, wParam,
												 lParam, lResult))
		{
			return lResult;
		}
	}

	XTP_SKINFRAMEWORK_ORIGINAL_STATE()
	{
		typedef LRESULT(WINAPI * LPFNDEFWINDOWPROC)(HWND hWnd, UINT Msg, WPARAM wParam,
													LPARAM lParam);
		return ((LPFNDEFWINDOWPROC)pFunction->GetOriginal())(hWnd, Msg, wParam, lParam);
	}
}

LRESULT WINAPI CXTPSkinManagerDetoursApiHook::OnHookDefMDIChildProcA(HWND hWnd, UINT Msg,
																	 WPARAM wParam, LPARAM lParam)
{
	XTP_SKINFRAMEWORK_MANAGE_STATE();

	CXTPSkinManagerDetoursApiFunction* pFunction = EnterHookedCall(
		xtpSkinDetoursApiDefMDIChildProcA);
	if (NULL == pFunction)
		return 0;

	if (m_bInitialized && pFunction->IsHooked()
		&& !XTPSkinManager()->IsWindowFilteredOut(hWnd, NULL, NULL, TRUE))
	{
		LRESULT lResult = 0;
		if (GetInstance()->CallHookDefWindowProc(hWnd, pFunction->GetOriginal(),
												 xtpSkinDefaultDefMDIChildProc, NULL, Msg, wParam,
												 lParam, lResult))
		{
			return lResult;
		}
	}

	XTP_SKINFRAMEWORK_ORIGINAL_STATE()
	{
		typedef LRESULT(WINAPI * LPFNDEFWINDOWPROC)(HWND hWnd, UINT Msg, WPARAM wParam,
													LPARAM lParam);
		return ((LPFNDEFWINDOWPROC)pFunction->GetOriginal())(hWnd, Msg, wParam, lParam);
	}
}

LRESULT WINAPI CXTPSkinManagerDetoursApiHook::OnHookDefMDIChildProcW(HWND hWnd, UINT Msg,
																	 WPARAM wParam, LPARAM lParam)
{
	XTP_SKINFRAMEWORK_MANAGE_STATE();

	CXTPSkinManagerDetoursApiFunction* pFunction = EnterHookedCall(
		xtpSkinDetoursApiDefMDIChildProcW);
	if (NULL == pFunction)
		return 0;

	if (m_bInitialized && pFunction->IsHooked()
		&& !XTPSkinManager()->IsWindowFilteredOut(hWnd, NULL, NULL, TRUE))
	{
		LRESULT lResult = 0;
		if (GetInstance()->CallHookDefWindowProc(hWnd, pFunction->GetOriginal(),
												 xtpSkinDefaultDefMDIChildProc, NULL, Msg, wParam,
												 lParam, lResult))
		{
			return lResult;
		}
	}

	XTP_SKINFRAMEWORK_ORIGINAL_STATE()
	{
		typedef LRESULT(WINAPI * LPFNDEFWINDOWPROC)(HWND hWnd, UINT Msg, WPARAM wParam,
													LPARAM lParam);
		return ((LPFNDEFWINDOWPROC)pFunction->GetOriginal())(hWnd, Msg, wParam, lParam);
	}
}

HRESULT WINAPI CXTPSkinManagerDetoursApiHook::OnHookDwmExtendFrameIntoClientArea(HWND hWnd,
																				 LPCVOID pMarInset)
{
	HRESULT hr = E_NOTIMPL;

	XTP_SKINFRAMEWORK_MANAGE_STATE();

	CXTPSkinManagerDetoursApiFunction* pFunction = EnterHookedCall(
		xtpSkinDetoursApiDwmExtendFrameIntoClientArea);
	if (NULL != pFunction)
	{
		BOOL bSkinObjectFound = FALSE;

		if (m_bInitialized && pFunction->IsHooked())
		{
			XTP_GUARD_SHARED_(CXTPSkinManager, XTPSkinManager(), pSkinManager)
			{
				if (pSkinManager->IsEnabled())
					bSkinObjectFound = (NULL != pSkinManager->Lookup(hWnd));
			}
		}

		if (!bSkinObjectFound)
		{
			typedef HRESULT(WINAPI * LPFNDWMEXTENDFRAMEINTOCLIENTAREA)(HWND, LPCVOID);
			hr = ((LPFNDWMEXTENDFRAMEINTOCLIENTAREA)pFunction->GetOriginal())(hWnd, pMarInset);
		}
	}

	return hr;
}

HANDLE STDAPICALLTYPE CXTPSkinManagerDetoursApiHook::OnHookCreateThread(
	LPSECURITY_ATTRIBUTES lpThreadAttributes, UINT_PTR dwStackSize,
	LPTHREAD_START_ROUTINE lpStartAddress, LPVOID lpParameter, DWORD dwCreationFlags,
	LPDWORD lpThreadId)
{
	XTP_SKINFRAMEWORK_MANAGE_STATE();

	CXTPSkinManagerDetoursApiFunction* pFunction = EnterHookedCall(xtpSkinDetoursApiCreateThread);
	if (NULL == pFunction)
		return NULL;

	typedef HANDLE(
		WINAPI * LPFNCREATETHREAD)(LPSECURITY_ATTRIBUTES lpThreadAttributes, UINT_PTR dwStackSize,
								   LPTHREAD_START_ROUTINE lpStartAddress, LPVOID lpParameter,
								   DWORD dwCreationFlags, LPDWORD lpThreadId);

	if (m_bInitialized && pFunction->IsHooked())
	{
		XTP_GUARD_SHARED_(CXTPSkinManager, XTPSkinManager(), pSkinManager)
		{
			if (pSkinManager->IsEnabled() && pSkinManager->GetAutoApplyNewThreads())
			{
				XTP_SKINFRAMEWORK_THREADPROCPARAMETER* pParam =
					new XTP_SKINFRAMEWORK_THREADPROCPARAMETER;
				pParam->lpParameter	   = lpParameter;
				pParam->lpStartAddress = lpStartAddress;

				return ((LPFNCREATETHREAD)pFunction->GetOriginal())(lpThreadAttributes, dwStackSize,
																	ThreadProcHook, pParam,
																	dwCreationFlags, lpThreadId);
			}
		}
	}

	return ((LPFNCREATETHREAD)pFunction->GetOriginal())(lpThreadAttributes, dwStackSize,
														lpStartAddress, lpParameter,
														dwCreationFlags, lpThreadId);
}

ATOM WINAPI CXTPSkinManagerDetoursApiHook::OnHookRegisterClassA(const WNDCLASSA* lpWndClass)
{
	XTP_SKINFRAMEWORK_MANAGE_STATE();

	CXTPSkinManagerDetoursApiFunction* pFunction = EnterHookedCall(xtpSkinDetoursApiRegisterClassA);
	if (NULL == pFunction)
		return 0;

	typedef ATOM(WINAPI * LPFNREGISTERCLASSA)(const WNDCLASSA* lpWndClass);

	if (m_bInitialized && pFunction->IsHooked())
	{
		XTP_GUARD_SHARED_(CXTPSkinManagerDetoursApiHook, GetInstance(), pInstance)
		{
			CXTPSkinManagerDetoursApiFunction* pDefWindowFunction = NULL;
			if (lpWndClass
				&& lpWndClass->lpfnWndProc
					   == XTPToFunctionPtr<WNDPROC>(
						   &CXTPSkinManagerDetoursApiHook::OnHookDefWindowProcA))
			{
				pDefWindowFunction = pInstance->GetHookedFunction(xtpSkinDetoursApiDefWindowProcA);
			}

			if (lpWndClass
				&& lpWndClass->lpfnWndProc
					   == XTPToFunctionPtr<WNDPROC>(
						   &CXTPSkinManagerDetoursApiHook::OnHookDefMDIChildProcA))
			{
				pDefWindowFunction = pInstance->GetHookedFunction(
					xtpSkinDetoursApiDefMDIChildProcA);
			}

			if (lpWndClass
				&& lpWndClass->lpfnWndProc
					   == XTPToFunctionPtr<WNDPROC>(
						   &CXTPSkinManagerDetoursApiHook::OnHookDefDlgProcA))
			{
				pDefWindowFunction = pInstance->GetHookedFunction(xtpSkinDetoursApiDefDlgProcA);
			}

			if (pDefWindowFunction)
			{
				WNDCLASSA wc;
				wc			   = *lpWndClass;
				wc.lpfnWndProc = XTPToFunctionPtr<WNDPROC>(
					XTPAccessShared(pDefWindowFunction)->GetOriginal());

				XTP_SKINFRAMEWORK_ORIGINAL_STATE()
				{
					return reinterpret_cast<LPFNREGISTERCLASSA>(pFunction->GetOriginal())(&wc);
				}
			}
		}
	}

	XTP_SKINFRAMEWORK_ORIGINAL_STATE()
	{
		return XTPToFunctionPtr<LPFNREGISTERCLASSA>(pFunction->GetOriginal())(lpWndClass);
	}
}

ATOM WINAPI CXTPSkinManagerDetoursApiHook::OnHookRegisterClassW(const WNDCLASSW* lpWndClass)
{
	XTP_SKINFRAMEWORK_MANAGE_STATE();

	CXTPSkinManagerDetoursApiFunction* pFunction = EnterHookedCall(xtpSkinDetoursApiRegisterClassW);
	if (NULL == pFunction)
		return 0;

	typedef ATOM(WINAPI * LPFNREGISTERCLASSW)(const WNDCLASSW* lpWndClass);

	if (m_bInitialized && pFunction->IsHooked())
	{
		XTP_GUARD_SHARED_(CXTPSkinManagerDetoursApiHook, GetInstance(), pInstance)
		{
			CXTPSkinManagerDetoursApiFunction* pDefWindowFunction = NULL;
			if (lpWndClass
				&& lpWndClass->lpfnWndProc
					   == XTPToFunctionPtr<WNDPROC>(
						   &CXTPSkinManagerDetoursApiHook::OnHookDefWindowProcW))
			{
				pDefWindowFunction = pInstance->GetHookedFunction(xtpSkinDetoursApiDefWindowProcW);
			}

			if (lpWndClass
				&& lpWndClass->lpfnWndProc
					   == XTPToFunctionPtr<WNDPROC>(
						   &CXTPSkinManagerDetoursApiHook::OnHookDefMDIChildProcW))
			{
				pDefWindowFunction = pInstance->GetHookedFunction(
					xtpSkinDetoursApiDefMDIChildProcW);
			}

			if (lpWndClass
				&& lpWndClass->lpfnWndProc
					   == XTPToFunctionPtr<WNDPROC>(
						   &CXTPSkinManagerDetoursApiHook::OnHookDefDlgProcW))
			{
				pDefWindowFunction = pInstance->GetHookedFunction(xtpSkinDetoursApiDefDlgProcW);
			}

			if (pDefWindowFunction)
			{
				WNDCLASSW wc;
				wc			   = *lpWndClass;
				wc.lpfnWndProc = XTPToFunctionPtr<WNDPROC>(
					XTPAccessShared(pDefWindowFunction)->GetOriginal());

				XTP_SKINFRAMEWORK_ORIGINAL_STATE()
				{
					return XTPToFunctionPtr<LPFNREGISTERCLASSW>(pFunction->GetOriginal())(&wc);
				}
			}
		}
	}

	XTP_SKINFRAMEWORK_ORIGINAL_STATE()
	{
		return XTPToFunctionPtr<LPFNREGISTERCLASSW>(pFunction->GetOriginal())(lpWndClass);
	}
}

int WINAPI CXTPSkinManagerDetoursApiHook::OnHookGetSystemMetrics(int nIndex)
{
	XTP_SKINFRAMEWORK_MANAGE_STATE();

	CXTPSkinManagerDetoursApiFunction* pFunction = EnterHookedCall(
		xtpSkinDetoursApiGetSystemMetrics);
	if (NULL == pFunction)
		return 0;

	typedef int(WINAPI * LPFNGETSYSTEMMETRICS)(int nIndex);
	int nResult = ((LPFNGETSYSTEMMETRICS)pFunction->GetOriginal())(nIndex);

	if (m_bInitialized && pFunction->IsHooked())
	{
		XTP_GUARD_SHARED_(CXTPSkinManager, XTPSkinManager(), pSkinManager)
		{
			if (pSkinManager->IsEnabled())
			{
				XTP_GUARD_SHARED_(CXTPSkinManagerMetrics, pSkinManager->GetMetrics(), pMetrics)
				{
					if (pSkinManager->IsEnabled() || pMetrics == NULL
						|| pMetrics->m_bRefreshMetrics)
						return nResult;

					if (!pSkinManager->HasApplyOptions(xtpSkinApplyMetrics | xtpSkinApplyColors
													   | xtpSkinApplyFrame))
						return nResult;

					switch (nIndex)
					{
						case SM_CYCAPTION: return pMetrics->m_cyCaption;
						case SM_CYSMCAPTION: return pMetrics->m_cySmallCaption;

						case SM_CXBORDER: return pMetrics->m_cxBorder;
						case SM_CYBORDER: return pMetrics->m_cyBorder;

						case SM_CXHSCROLL: return pMetrics->m_cxHScroll;
						case SM_CYHSCROLL: return pMetrics->m_cyHScroll;
						case SM_CXVSCROLL: return pMetrics->m_cxVScroll;
						case SM_CYVSCROLL: return pMetrics->m_cyVScroll;

						case SM_CXEDGE: return pMetrics->m_cxEdge;
						case SM_CYEDGE: return pMetrics->m_cyEdge;
					}
				}
			}
		}
	}

	return nResult;
}

BOOL WINAPI CXTPSkinManagerDetoursApiHook::OnHookSystemParametersInfoA(UINT uiAction, UINT uiParam,
																	   PVOID pvParam, UINT fWinIni)
{
	XTP_SKINFRAMEWORK_MANAGE_STATE();

	CXTPSkinManagerDetoursApiFunction* pFunction = EnterHookedCall(
		xtpSkinDetoursApiSystemParametersInfoA);
	if (NULL == pFunction)
		return FALSE;

	typedef BOOL(WINAPI * LPFNSYSTEMPARAMETERSINFO)(UINT uiAction, UINT uiParam, PVOID pvParam,
													UINT fWinIni);
	BOOL bResult = ((LPFNSYSTEMPARAMETERSINFO)pFunction->GetOriginal())(uiAction, uiParam, pvParam,
																		fWinIni);

	if (m_bInitialized && pFunction->IsHooked())
	{
		XTP_GUARD_SHARED_(CXTPSkinManager, XTPSkinManager(), pSkinManager)
		{
			if (!pSkinManager->IsEnabled() || pSkinManager->GetMetrics()->m_bRefreshMetrics)
				return bResult;

			if (!pSkinManager->HasApplyOptions(xtpSkinApplyMetrics | xtpSkinApplyColors
											   | xtpSkinApplyFrame))
				return bResult;

			if (uiAction == SPI_GETICONTITLELOGFONT && uiParam == sizeof(LOGFONTA))
			{
#	ifndef _UNICODE
				LOGFONTA* pLogFont = reinterpret_cast<LOGFONTA*>(pvParam);
				ASSERT(NULL != pLogFont);
#	else
				LOGFONTW logFont;
				LOGFONTW* pLogFont = &logFont;
#	endif

				XTP_GUARD_SHARED_(CXTPSkinManagerSchema, XTPSkinManager()->GetSchema(), pSchema)
				{
					UINT nSysMetrics = pSchema->GetClassCode(_T("SYSMETRICS"));

					pSchema->GetFontProperty(nSysMetrics, 0, 0, TMT_ICONTITLEFONT, *pLogFont);

#	ifdef _UNICODE
					LOGFONTA* pLogFontA = reinterpret_cast<LOGFONTA*>(pvParam);
					ASSERT(NULL != pLogFontA);

					memcpy(pLogFontA, pLogFont, offsetof(LOGFONTA, lfFaceName));
					WCSTOMBS_S(pLogFontA->lfFaceName, pLogFont->lfFaceName,
							   _countof(pLogFontA->lfFaceName));
#	endif
				}
			}

			if (uiAction == SPI_GETNONCLIENTMETRICS && uiParam == sizeof(NONCLIENTMETRICSA))
			{
				XTP_GUARD_SHARED_(CXTPSkinManagerSchema, XTPSkinManager()->GetSchema(), pSchema)
				{
					UINT nSysMetrics = pSchema->GetClassCode(_T("SYSMETRICS"));

					LOGFONT lfMenuFont, lfStatusFont, lfCaptionFont, lfSmCaptionFont;
					pSchema->GetFontProperty(nSysMetrics, 0, 0, TMT_MENUFONT, lfMenuFont);
					pSchema->GetFontProperty(nSysMetrics, 0, 0, TMT_STATUSFONT, lfStatusFont);
					pSchema->GetFontProperty(nSysMetrics, 0, 0, TMT_CAPTIONFONT, lfCaptionFont);
					pSchema->GetFontProperty(nSysMetrics, 0, 0, TMT_SMALLCAPTIONFONT,
											 lfSmCaptionFont);

					NONCLIENTMETRICSA* pncm = reinterpret_cast<NONCLIENTMETRICSA*>(pvParam);
					ASSERT(NULL != pncm);

#	ifndef _UNICODE
					memcpy(&pncm->lfMenuFont, &lfMenuFont, sizeof(LOGFONTA));
					memcpy(&pncm->lfStatusFont, &lfStatusFont, sizeof(LOGFONTA));
					memcpy(&pncm->lfCaptionFont, &lfCaptionFont, sizeof(LOGFONTA));
					memcpy(&pncm->lfSmCaptionFont, &lfSmCaptionFont, sizeof(LOGFONTA));

#	else
					memcpy(&pncm->lfMenuFont, &lfMenuFont, offsetof(LOGFONTA, lfFaceName));
					WCSTOMBS_S(pncm->lfMenuFont.lfFaceName, lfMenuFont.lfFaceName,
							   _countof(pncm->lfMenuFont.lfFaceName));

					memcpy(&pncm->lfStatusFont, &lfStatusFont, offsetof(LOGFONTA, lfFaceName));
					WCSTOMBS_S(pncm->lfStatusFont.lfFaceName, lfStatusFont.lfFaceName,
							   _countof(pncm->lfStatusFont.lfFaceName));

					memcpy(&pncm->lfCaptionFont, &lfCaptionFont, offsetof(LOGFONTA, lfFaceName));
					WCSTOMBS_S(pncm->lfCaptionFont.lfFaceName, lfCaptionFont.lfFaceName,
							   _countof(pncm->lfCaptionFont.lfFaceName));

					memcpy(&pncm->lfSmCaptionFont, &lfSmCaptionFont,
						   offsetof(LOGFONTA, lfFaceName));
					WCSTOMBS_S(pncm->lfSmCaptionFont.lfFaceName, lfSmCaptionFont.lfFaceName,
							   _countof(pncm->lfSmCaptionFont.lfFaceName));
#	endif
				}
			}
		}
	}

	return bResult;
}

BOOL WINAPI CXTPSkinManagerDetoursApiHook::OnHookSystemParametersInfoW(UINT uiAction, UINT uiParam,
																	   PVOID pvParam, UINT fWinIni)
{
	XTP_SKINFRAMEWORK_MANAGE_STATE();

	CXTPSkinManagerDetoursApiFunction* pFunction = EnterHookedCall(
		xtpSkinDetoursApiSystemParametersInfoW);
	if (NULL == pFunction)
		return FALSE;

	typedef BOOL(WINAPI * LPFNSYSTEMPARAMETERSINFO)(UINT uiAction, UINT uiParam, PVOID pvParam,
													UINT fWinIni);
	BOOL bResult = ((LPFNSYSTEMPARAMETERSINFO)pFunction->GetOriginal())(uiAction, uiParam, pvParam,
																		fWinIni);

	if (m_bInitialized && pFunction->IsHooked())
	{
		XTP_GUARD_SHARED_(CXTPSkinManager, XTPSkinManager(), pSkinManager)
		{
			if (!pSkinManager->IsEnabled() || pSkinManager->GetMetrics()->m_bRefreshMetrics)
				return bResult;

			if (!pSkinManager->HasApplyOptions(xtpSkinApplyMetrics | xtpSkinApplyColors
											   | xtpSkinApplyFrame))
				return bResult;

			if (uiAction == SPI_GETICONTITLELOGFONT && uiParam == sizeof(LOGFONTW))
			{
#	ifdef _UNICODE
				LOGFONTW* pLogFont = reinterpret_cast<LOGFONTW*>(pvParam);
				ASSERT(NULL != pLogFont);
#	else
				LOGFONTA logFont;
				LOGFONTA* pLogFont = &logFont;
#	endif

				XTP_GUARD_SHARED_(CXTPSkinManagerSchema, XTPSkinManager()->GetSchema(), pSchema)
				{
					UINT nSysMetrics = pSchema->GetClassCode(_T("SYSMETRICS"));

					pSchema->GetFontProperty(nSysMetrics, 0, 0, TMT_ICONTITLEFONT, *pLogFont);

#	ifndef _UNICODE
					LOGFONTW* pLogFontW = reinterpret_cast<LOGFONTW*>(pvParam);
					ASSERT(NULL != pLogFontW);

					memcpy(pLogFontW, pLogFont, offsetof(LOGFONT, lfFaceName));
					MBSTOWCS_S(pLogFontW->lfFaceName, pLogFont->lfFaceName,
							   _countof(pLogFontW->lfFaceName));
#	endif
				}
			}

			if (uiAction == SPI_GETNONCLIENTMETRICS && uiParam == sizeof(NONCLIENTMETRICSW))
			{
				XTP_GUARD_SHARED_(CXTPSkinManagerSchema, XTPSkinManager()->GetSchema(), pSchema)
				{
					UINT nSysMetrics = pSchema->GetClassCode(_T("SYSMETRICS"));

					LOGFONT lfMenuFont, lfStatusFont, lfCaptionFont, lfSmCaptionFont;
					pSchema->GetFontProperty(nSysMetrics, 0, 0, TMT_MENUFONT, lfMenuFont);
					pSchema->GetFontProperty(nSysMetrics, 0, 0, TMT_STATUSFONT, lfStatusFont);
					pSchema->GetFontProperty(nSysMetrics, 0, 0, TMT_CAPTIONFONT, lfCaptionFont);
					pSchema->GetFontProperty(nSysMetrics, 0, 0, TMT_SMALLCAPTIONFONT,
											 lfSmCaptionFont);

					NONCLIENTMETRICSW* pncm = reinterpret_cast<NONCLIENTMETRICSW*>(pvParam);
					ASSERT(NULL != pncm);

#	ifdef _UNICODE
					memcpy(&pncm->lfMenuFont, &lfMenuFont, sizeof(LOGFONTW));
					memcpy(&pncm->lfStatusFont, &lfStatusFont, sizeof(LOGFONTW));
					memcpy(&pncm->lfCaptionFont, &lfCaptionFont, sizeof(LOGFONTW));
					memcpy(&pncm->lfSmCaptionFont, &lfSmCaptionFont, sizeof(LOGFONTW));

#	else
					memcpy(&pncm->lfMenuFont, &lfMenuFont, offsetof(LOGFONTW, lfFaceName));
					MBSTOWCS_S(pncm->lfMenuFont.lfFaceName, lfMenuFont.lfFaceName,
							   _countof(pncm->lfMenuFont.lfFaceName));

					memcpy(&pncm->lfStatusFont, &lfStatusFont, offsetof(LOGFONTW, lfFaceName));
					MBSTOWCS_S(pncm->lfStatusFont.lfFaceName, lfStatusFont.lfFaceName,
							   _countof(pncm->lfStatusFont.lfFaceName));

					memcpy(&pncm->lfCaptionFont, &lfCaptionFont, offsetof(LOGFONTW, lfFaceName));
					MBSTOWCS_S(pncm->lfCaptionFont.lfFaceName, lfCaptionFont.lfFaceName,
							   _countof(pncm->lfCaptionFont.lfFaceName));

					memcpy(&pncm->lfSmCaptionFont, &lfSmCaptionFont,
						   offsetof(LOGFONTW, lfFaceName));
					MBSTOWCS_S(pncm->lfSmCaptionFont.lfFaceName, lfSmCaptionFont.lfFaceName,
							   _countof(pncm->lfSmCaptionFont.lfFaceName));
#	endif
				}
			}
		}
	}

	return bResult;
}

BOOL WINAPI CXTPSkinManagerDetoursApiHook::OnHookAdjustWindowRectEx(LPRECT lpRect, DWORD dwStyle,
																	BOOL bMenu, DWORD dwExStyle)
{
	XTP_SKINFRAMEWORK_MANAGE_STATE();

	BOOL bResult = FALSE;

	CXTPSkinManagerDetoursApiFunction* pFunction = EnterHookedCall(
		xtpSkinDetoursApiAdjustWindowRectEx);
	if (NULL != pFunction)
	{
		BOOL bDefaultProcessing = TRUE;

		if (m_bInitialized && pFunction->IsHooked())
		{
			XTP_GUARD_SHARED_(CXTPSkinManager, XTPSkinManager(), pSkinManager)
			{
				if (pSkinManager->IsEnabled() && pSkinManager->HasApplyOptions(xtpSkinApplyFrame)
					&& (0 == (dwStyle & WS_MAXIMIZE)) && NULL != lpRect && !bMenu)
				{
					CRect rcBorders = XTPAccessShared(pSkinManager->GetSchema())
										  ->CalcFrameBorders(dwStyle, dwExStyle);
					lpRect->top -= rcBorders.top;
					lpRect->left -= rcBorders.left;
					lpRect->right += rcBorders.right;
					lpRect->bottom += rcBorders.bottom;

					bResult			   = TRUE;
					bDefaultProcessing = FALSE;
				}
			}
		}

		if (bDefaultProcessing)
		{
			typedef BOOL(WINAPI * LPFNADJUSTWINDOWRECTEX)(LPRECT lpRect, DWORD dwStyle, BOOL bMenu,
														  DWORD dwExStyle);
			bResult = ((LPFNADJUSTWINDOWRECTEX)pFunction->GetOriginal())(lpRect, dwStyle, bMenu,
																		 dwExStyle);
		}
	}

	return bResult;
}

BOOL WINAPI CXTPSkinManagerDetoursApiHook::OnHookAdjustWindowRect(LPRECT lpRect, DWORD dwStyle,
																  BOOL bMenu)
{
	XTP_SKINFRAMEWORK_MANAGE_STATE();

	BOOL bResult = FALSE;

	CXTPSkinManagerDetoursApiFunction* pFunction = EnterHookedCall(
		xtpSkinDetoursApiAdjustWindowRect);
	if (NULL != pFunction)
	{
		BOOL bDefaultProcessing = TRUE;

		if (m_bInitialized && pFunction->IsHooked())
		{
			XTP_GUARD_SHARED_(CXTPSkinManager, XTPSkinManager(), pSkinManager)
			{
				if (pSkinManager->IsEnabled() && pSkinManager->HasApplyOptions(xtpSkinApplyFrame)
					&& (0 == (dwStyle & WS_MAXIMIZE)) && NULL != lpRect && !bMenu)
				{
					DWORD dwExStyle = 0;

					if ((dwStyle & WS_THICKFRAME))
						dwExStyle = WS_EX_WINDOWEDGE;

					if (((dwStyle & WS_CAPTION) == WS_DLGFRAME)
						|| ((dwStyle & WS_CAPTION) == WS_CAPTION))
						dwExStyle = WS_EX_WINDOWEDGE;

					CRect rcBorders = XTPAccessShared(pSkinManager->GetSchema())
										  ->CalcFrameBorders(dwStyle, dwExStyle);
					lpRect->top -= rcBorders.top;
					lpRect->left -= rcBorders.left;
					lpRect->right += rcBorders.right;
					lpRect->bottom += rcBorders.bottom;

					bResult			   = TRUE;
					bDefaultProcessing = FALSE;
				}
			}
		}

		if (bDefaultProcessing)
		{
			typedef BOOL(WINAPI * LPFNADJUSTWINDOWRECT)(LPRECT lpRect, DWORD dwStyle, BOOL bMenu);
			bResult = ((LPFNADJUSTWINDOWRECT)pFunction->GetOriginal())(lpRect, dwStyle, bMenu);
		}
	}

	return bResult;
}

int WINAPI CXTPSkinManagerDetoursApiHook::OnHookFillRect(HDC hDC, CONST RECT* lprc, HBRUSH hbr)
{
	XTP_SKINFRAMEWORK_MANAGE_STATE();

	CXTPSkinManagerDetoursApiFunction* pFunction = EnterHookedCall(xtpSkinDetoursApiFillRect);
	if (NULL == pFunction)
		return 0;

	if (m_bInitialized && pFunction->IsHooked())
	{
		XTP_GUARD_SHARED_(CXTPSkinManager, XTPSkinManager(), pSkinManager)
		{
			if (pSkinManager->IsEnabled() && pSkinManager->GetMetrics() && (DWORD_PTR)hbr > 0
				&& (DWORD_PTR)hbr < XTP_SKINMETRICS_COLORTABLESIZE)
			{
				hbr = pSkinManager->GetMetrics()->m_brTheme[(DWORD_PTR)hbr - 1];
			}
		}
	}

	typedef int(WINAPI * LPFNFILLRECT)(HDC hDC, CONST RECT * lprc, HBRUSH hbr);
	return ((LPFNFILLRECT)pFunction->GetOriginal())(hDC, lprc, hbr);
}

BOOL WINAPI CXTPSkinManagerDetoursApiHook::OnHookDrawEdge(HDC hdc, LPRECT lprc, UINT edge,
														  UINT flags)
{
	XTP_SKINFRAMEWORK_MANAGE_STATE();

	CXTPSkinManagerDetoursApiFunction* pFunction = EnterHookedCall(xtpSkinDetoursApiDrawEdge);
	if (NULL == pFunction)
		return 0;

	if (pFunction->IsHooked() && XTPSkinManager()->IsEnabled())
		return XTPSkinFrameworkDrawEdge(hdc, lprc, edge, flags);

	typedef BOOL(WINAPI * LPFNDRAWEDGE)(HDC, LPRECT, UINT, UINT);
	return ((LPFNDRAWEDGE)pFunction->GetOriginal())(hdc, lprc, edge, flags);
}

BOOL WINAPI CXTPSkinManagerDetoursApiHook::OnHookDrawFrameControl(HDC hdc, LPRECT lprc, UINT uType,
																  UINT uState)
{
	XTP_SKINFRAMEWORK_MANAGE_STATE();

	CXTPSkinManagerDetoursApiFunction* pFunction = EnterHookedCall(
		xtpSkinDetoursApiDrawFrameControl);
	if (NULL == pFunction)
		return 0;

	if (pFunction->IsHooked() && XTPSkinManager()->IsEnabled())
	{
		if (XTPSkinFrameworkDrawFrameControl(hdc, lprc, uType, uState))
			return TRUE;
	}

	typedef BOOL(WINAPI * LPFNDRAWFRAMECONTROL)(HDC, LPRECT, UINT, UINT);
	return ((LPFNDRAWFRAMECONTROL)pFunction->GetOriginal())(hdc, lprc, uType, uState);
}

BOOL WINAPI CXTPSkinManagerDetoursApiHook::OnHookDrawMenuBar(HWND hWnd)
{
	XTP_SKINFRAMEWORK_MANAGE_STATE();

	CXTPSkinManagerDetoursApiFunction* pFunction = EnterHookedCall(xtpSkinDetoursApiDrawMenuBar);
	if (NULL == pFunction)
		return 0;

	if (m_bInitialized && pFunction->IsHooked())
	{
		XTP_GUARD_SHARED_(CXTPSkinManager, XTPSkinManager(), pSkinManager)
		{
			if (pSkinManager->IsEnabled() && pSkinManager->HasApplyOptions(xtpSkinApplyFrame))
			{
				CXTPSkinObjectFrame* pFrame = (CXTPSkinObjectFrame*)pSkinManager->Lookup(hWnd);
				if (pFrame)
					return XTPAccessShared(pFrame)->DrawMenuBar();
			}
		}
	}

	typedef BOOL(WINAPI * LPFNDRAWMENUBAR)(HWND hWnd);
	return ((LPFNDRAWMENUBAR)pFunction->GetOriginal())(hWnd);
}

BOOL WINAPI CXTPSkinManagerDetoursApiHook::OnHookGetMenuItemRect(HWND hWnd, HMENU hMenu, UINT uItem,
																 LPRECT lprcItem)
{
	XTP_SKINFRAMEWORK_MANAGE_STATE();

	BOOL bResult = FALSE;

	CXTPSkinManagerDetoursApiFunction* pFunction = EnterHookedCall(
		xtpSkinDetoursApiGetMenuItemRect);
	if (NULL != pFunction)
	{
		BOOL bDefaultProcessing = TRUE;

		if (m_bInitialized && pFunction->IsHooked())
		{
			XTP_GUARD_SHARED_(CXTPSkinManager, XTPSkinManager(), pSkinManager)
			{
				bDefaultProcessing = !(pSkinManager->IsEnabled()
									   && (pSkinManager->GetApplyOptions() & xtpSkinApplyMenus));
			}
		}

		if (bDefaultProcessing)
		{
			typedef BOOL(WINAPI * LPFNGETMENUITEMRECT)(HWND hWnd, HMENU hMenu, UINT uItem,
													   LPRECT lprcItem);
			bResult = (LPFNGETMENUITEMRECT(pFunction->GetOriginal()))(hWnd, hMenu, uItem, lprcItem);
		}
	}

	return bResult;
}

BOOL WINAPI CXTPSkinManagerDetoursApiHook::OnHookTrackPopupMenu(HMENU hMenu, UINT uFlags, int x,
																int y, int nReserved, HWND hWnd,
																CONST RECT* prcRect)
{
	XTP_SKINFRAMEWORK_MANAGE_STATE();

	BOOL bResult = FALSE;

	CXTPSkinManagerDetoursApiFunction* pFunction = EnterHookedCall(xtpSkinDetoursApiTrackPopupMenu);
	if (NULL != pFunction)
	{
		BOOL bDefaultProcessing = TRUE;

		if (m_bInitialized && pFunction->IsHooked())
		{
			XTP_GUARD_SHARED_(CXTPSkinManager, XTPSkinManager(), pSkinManager)
			{
				if (pSkinManager->IsEnabled()
					&& 0 != (pSkinManager->GetApplyOptions() & xtpSkinApplyMenus))
				{
					bResult = CXTPSkinObjectApplicationFrame::TrackPopupMenu(hMenu, uFlags, x, y,
																			 hWnd, prcRect);
					bDefaultProcessing = FALSE;
				}
			}
		}

		if (bDefaultProcessing)
		{
			typedef BOOL(WINAPI * LPFNTRACKPOPUPMENU)(HMENU hMenu, UINT uFlags, int x, int y,
													  int nReserved, HWND hWnd,
													  CONST RECT* prcRect);
			bResult = (LPFNTRACKPOPUPMENU(pFunction->GetOriginal()))(hMenu, uFlags, x, y, nReserved,
																	 hWnd, prcRect);
		}
	}

	return bResult;
}

BOOL WINAPI CXTPSkinManagerDetoursApiHook::OnHookTrackPopupMenuExImpl(
	XTPSkinFrameworkDetoursApiFunctionIndex nFunctionIndex, HMENU hMenu, UINT uFlags, int x, int y,
	HWND hWnd, LPTPMPARAMS lptpParams)
{
	XTP_SKINFRAMEWORK_MANAGE_STATE();

	BOOL bResult = FALSE;

	CXTPSkinManagerDetoursApiFunction* pFunction = EnterHookedCall(nFunctionIndex);
	if (NULL != pFunction)
	{
		BOOL bDefaultProcessing = TRUE;

		if (m_bInitialized && pFunction->IsHooked())
		{
			XTP_GUARD_SHARED_(CXTPSkinManager, XTPSkinManager(), pSkinManager)
			{
				if (pSkinManager->IsEnabled()
					&& 0 != (pSkinManager->GetApplyOptions() & xtpSkinApplyMenus))
				{
					bResult = CXTPSkinObjectApplicationFrame::TrackPopupMenu(
						hMenu, uFlags, x, y, hWnd, lptpParams ? &lptpParams->rcExclude : NULL);
					bDefaultProcessing = FALSE;
				}
			}
		}

		if (bDefaultProcessing)
		{
			typedef BOOL(WINAPI * LPFNTRACKPOPUPMENUEX)(HMENU hMenu, UINT uFlags, int x, int y,
														HWND hWnd, LPTPMPARAMS lptpParams);
			bResult = ((LPFNTRACKPOPUPMENUEX)pFunction->GetOriginal())(hMenu, uFlags, x, y, hWnd,
																	   lptpParams);
		}
	}

	return bResult;
}

BOOL WINAPI CXTPSkinManagerDetoursApiHook::OnHookTrackPopupMenuEx(HMENU hMenu, UINT uFlags, int x,
																  int y, HWND hWnd,
																  LPTPMPARAMS lptpParams)
{
	return OnHookTrackPopupMenuExImpl(xtpSkinDetoursApiTrackPopupMenuEx, hMenu, uFlags, x, y, hWnd,
									  lptpParams);
}
BOOL WINAPI CXTPSkinManagerDetoursApiHook::OnHookNtUserTrackPopupMenuEx(HMENU hMenu, UINT uFlags,
																		int x, int y, HWND hWnd,
																		LPTPMPARAMS lptpParams)
{
	return OnHookTrackPopupMenuExImpl(xtpSkinDetoursApiNtUserTrackPopupMenuEx, hMenu, uFlags, x, y,
									  hWnd, lptpParams);
}

FARPROC WINAPI CXTPSkinManagerDetoursApiHook::GetProcAddressWindows(HMODULE hModule,
																	LPCSTR pszProcName)
{
	LPVOID pfnOrig = GetInstance()->GetOriginalFunctionAddress(xtpSkinDetoursApiGetProcAddress);

	if (NULL != pfnOrig)
	{
		typedef FARPROC(WINAPI * LPFNGETPROCADDRESS)(HMODULE hModule, LPCSTR pszProcName);
		return ((LPFNGETPROCADDRESS)pfnOrig)(hModule, pszProcName);
	}

	return ::GetProcAddress(hModule, pszProcName);
}

FARPROC WINAPI CXTPSkinManagerDetoursApiHook::OnHookGetProcAddress(HMODULE hModule,
																   PCSTR pszProcName)
{
	XTP_SKINFRAMEWORK_MANAGE_STATE();

	if ((DWORD_PTR)pszProcName < 0xFFFF)
		return GetProcAddressWindows(hModule, pszProcName);

	CXTPSkinManagerDetoursApiFunction* pFuncHook = NULL;
	if (XTP_UXTHEME_HANDLE == hModule || hModule == m_hUxTheme)
		pFuncHook = EnterHookedCall(m_hUxTheme, pszProcName);
	else
		pFuncHook = EnterHookedCall(hModule, pszProcName);

	if (NULL != pFuncHook && pFuncHook->IsHooked())
		return reinterpret_cast<FARPROC>(pFuncHook->GetHook());

	return GetProcAddressWindows(hModule, pszProcName);
}

HMODULE WINAPI CXTPSkinManagerDetoursApiHook::OnHookGetModuleHandleA(LPCSTR lpModuleName)
{
	XTP_SKINFRAMEWORK_MANAGE_STATE();

	CXTPSkinManagerDetoursApiFunction* pFunction = EnterHookedCall(
		xtpSkinDetoursApiGetModuleHandleA);
	if (NULL == pFunction)
		return NULL;

	if (m_bInitialized && pFunction->IsHooked())
	{
		if (lpModuleName && XTPCompareStringNoCase(lpModuleName, "UxTheme.dll") == 0)
		{
			XTP_GUARD_SHARED_(CXTPSkinManager, XTPSkinManager(), pSkinManager)
			{
				if (pSkinManager->IsEnabled()
					&& pSkinManager->GetApplyOptions() & xtpSkinApplyColors)
					return XTP_UXTHEME_HANDLE;
			}
		}
	}

	typedef HMODULE(WINAPI * LPFNGETMODULEHANDLEA)(LPCSTR lpModuleName);
	return ((LPFNGETMODULEHANDLEA)pFunction->GetOriginal())(lpModuleName);
}

HMODULE WINAPI CXTPSkinManagerDetoursApiHook::OnHookGetModuleHandleW(LPCWSTR lpModuleName)
{
	XTP_SKINFRAMEWORK_MANAGE_STATE();

	CXTPSkinManagerDetoursApiFunction* pFunction = EnterHookedCall(
		xtpSkinDetoursApiGetModuleHandleW);
	if (NULL == pFunction)
		return NULL;

	if (m_bInitialized && pFunction->IsHooked())
	{
		if (lpModuleName && XTPCompareStringNoCase(lpModuleName, L"UxTheme.dll") == 0)
		{
			XTP_GUARD_SHARED_(CXTPSkinManager, XTPSkinManager(), pSkinManager)
			{
				if (pSkinManager->IsEnabled()
					&& pSkinManager->GetApplyOptions() & xtpSkinApplyColors)
					return XTP_UXTHEME_HANDLE;
			}
		}
	}

	typedef HMODULE(WINAPI * LPFNGETMODULEHANDLEW)(LPCWSTR lpModuleName);
	return ((LPFNGETMODULEHANDLEW)pFunction->GetOriginal())(lpModuleName);
}

HMODULE WINAPI CXTPSkinManagerDetoursApiHook::OnHookGetModuleHandleExA(DWORD dwFlags,
																	   LPCSTR lpModuleName,
																	   HMODULE* phModule)
{
	XTP_SKINFRAMEWORK_MANAGE_STATE();

	CXTPSkinManagerDetoursApiFunction* pFunction = EnterHookedCall(
		xtpSkinDetoursApiGetModuleHandleExA);
	if (NULL == pFunction)
		return NULL;

	if (m_bInitialized && pFunction->IsHooked())
	{
		if (lpModuleName && XTPCompareStringNoCase(lpModuleName, "UxTheme.dll") == 0)
		{
			XTP_GUARD_SHARED_(CXTPSkinManager, XTPSkinManager(), pSkinManager)
			{
				if (pSkinManager->IsEnabled()
					&& pSkinManager->GetApplyOptions() & xtpSkinApplyColors)
					return XTP_UXTHEME_HANDLE;
			}
		}
	}

	typedef HMODULE(WINAPI * LPFNGETMODULEHANDLEEXA)(DWORD dwFlags, LPCSTR lpModuleName,
													 HMODULE * phModule);
	return ((LPFNGETMODULEHANDLEEXA)pFunction->GetOriginal())(dwFlags, lpModuleName, phModule);
}

HMODULE WINAPI CXTPSkinManagerDetoursApiHook::OnHookGetModuleHandleExW(DWORD dwFlags,
																	   LPCWSTR lpModuleName,
																	   HMODULE* phModule)
{
	XTP_SKINFRAMEWORK_MANAGE_STATE();

	CXTPSkinManagerDetoursApiFunction* pFunction = EnterHookedCall(
		xtpSkinDetoursApiGetModuleHandleExW);
	if (NULL == pFunction)
		return NULL;

	if (m_bInitialized && pFunction->IsHooked())
	{
		if (lpModuleName && XTPCompareStringNoCase(lpModuleName, L"UxTheme.dll") == 0)
		{
			XTP_GUARD_SHARED_(CXTPSkinManager, XTPSkinManager(), pSkinManager)
			{
				if (pSkinManager->IsEnabled()
					&& pSkinManager->GetApplyOptions() & xtpSkinApplyColors)
					return XTP_UXTHEME_HANDLE;
			}
		}
	}

	typedef HMODULE(WINAPI * LPFNGETMODULEHANDLEEXW)(DWORD dwFlags, LPCWSTR lpModuleName,
													 HMODULE * phModule);
	return ((LPFNGETMODULEHANDLEEXW)pFunction->GetOriginal())(dwFlags, lpModuleName, phModule);
}

HTHEME STDAPICALLTYPE CXTPSkinManagerDetoursApiHook::OnHookOpenThemeData(HWND hWnd,
																		 LPCWSTR pszClassList)
{
	XTP_SKINFRAMEWORK_MANAGE_STATE();

	CXTPSkinManagerDetoursApiFunction* pFunction = EnterHookedCall(xtpSkinDetoursApiOpenThemeData);
	if (NULL == pFunction)
		return NULL;

	typedef HTHEME(STDAPICALLTYPE * PFNOpenThemeData)(HWND, LPCWSTR);
	HTHEME hTheme = reinterpret_cast<PFNOpenThemeData>(pFunction->GetOriginal())(hWnd,
																				 pszClassList);

	if (!m_bInitialized)
		return hTheme;

	CXTPSkinManager* pSkinManager = XTPSkinManager();
	if (!pSkinManager->IsEnabled())
		return hTheme;

	XTP_GUARD_SHARED(CXTPSkinManager, pSkinManager)
	{
		CXTPSkinManagerClass* pClass = pSkinManager->GetSkinClass(pSkinManager->Lookup(hWnd),
																  pszClassList);
		if (NULL != pClass)
			pClass->BindTheme(hTheme);
	}

	return hTheme;
}

HTHEME STDAPICALLTYPE CXTPSkinManagerDetoursApiHook::OnHookOpenThemeDataForDpi(HWND hWnd,
																			   LPCWSTR pszClassList,
																			   UINT dpi)
{
	XTP_SKINFRAMEWORK_MANAGE_STATE();

	CXTPSkinManagerDetoursApiFunction* pFunction = EnterHookedCall(
		xtpSkinDetoursApiOpenThemeDataForDpi);
	if (NULL == pFunction)
		return NULL;

	typedef HTHEME(STDAPICALLTYPE * PFNOpenThemeDataForDpi)(HWND, LPCWSTR, UINT);
	HTHEME hTheme = reinterpret_cast<PFNOpenThemeDataForDpi>(
		pFunction->GetOriginal())(hWnd, pszClassList, dpi);

	if (!m_bInitialized)
		return hTheme;

	CXTPSkinManager* pSkinManager = XTPSkinManager();
	if (!pSkinManager->IsEnabled())
		return hTheme;

	XTP_GUARD_SHARED(CXTPSkinManager, pSkinManager)
	{
		CXTPSkinManagerClass* pClass = pSkinManager->GetSkinClass(pSkinManager->Lookup(hWnd),
																  pszClassList);
		if (NULL != pClass)
			pClass->BindTheme(hTheme);
	}

	return hTheme;
}

HTHEME STDAPICALLTYPE CXTPSkinManagerDetoursApiHook::OnHookOpenThemeDataEx(HWND hWnd,
																		   LPCWSTR pszClassList,
																		   DWORD dwFlags)
{
	XTP_SKINFRAMEWORK_MANAGE_STATE();

	CXTPSkinManagerDetoursApiFunction* pFunction = EnterHookedCall(
		xtpSkinDetoursApiOpenThemeDataEx);
	if (NULL == pFunction)
		return NULL;

	typedef HTHEME(STDAPICALLTYPE * PFNOpenThemeDataEx)(HWND, LPCWSTR, DWORD);
	HTHEME hTheme = reinterpret_cast<PFNOpenThemeDataEx>(
		pFunction->GetOriginal())(hWnd, pszClassList, dwFlags);

	if (!m_bInitialized)
		return hTheme;

	CXTPSkinManager* pSkinManager = XTPSkinManager();
	if (!pSkinManager->IsEnabled())
		return hTheme;

	XTP_GUARD_SHARED(CXTPSkinManager, pSkinManager)
	{
		CXTPSkinManagerClass* pClass = pSkinManager->GetSkinClass(pSkinManager->Lookup(hWnd),
																  pszClassList);
		if (NULL != pClass)
			pClass->BindTheme(hTheme);
	}

	return hTheme;
}

HRESULT STDAPICALLTYPE CXTPSkinManagerDetoursApiHook::OnHookCloseThemeData(HTHEME hTheme)
{
	XTP_SKINFRAMEWORK_MANAGE_STATE();

	CXTPSkinManagerDetoursApiFunction* pFunction = EnterHookedCall(xtpSkinDetoursApiCloseThemeData);
	if (NULL == pFunction)
		return E_FAIL;

	if (m_bInitialized)
	{
		XTP_GUARD_SHARED_(CXTPSkinManager, XTPSkinManager(), pSkinManager)
		{
			CXTPSkinManagerClass* pClass = pSkinManager->FromHandle(hTheme);
			if (NULL != pClass)
				pClass->UnbindTheme(hTheme);
		}
	}

	typedef HRESULT(STDAPICALLTYPE * PFNCloseThemeData)(HTHEME);
	return reinterpret_cast<PFNCloseThemeData>(pFunction->GetOriginal())(hTheme);
}

HRESULT STDAPICALLTYPE CXTPSkinManagerDetoursApiHook::OnHookDrawThemeBackground(
	HTHEME hTheme, HDC hDC, int iPartId, int iStateId, const RECT* pRect, const RECT* pClipRect)
{
	XTP_SKINFRAMEWORK_MANAGE_STATE();

	CXTPSkinManagerDetoursApiFunction* pFunction = EnterHookedCall(
		xtpSkinDetoursApiDrawThemeBackground);
	if (NULL == pFunction)
		return E_FAIL;

	if (m_bInitialized)
	{
		XTP_GUARD_SHARED_(CXTPSkinManager, XTPSkinManager(), pSkinManager)
		{
			CXTPSkinManagerClass* pClass = pSkinManager->FromHandle(hTheme);
			if (NULL != pClass)
			{
				if (pSkinManager->IsEnabled())
				{
					XTP_GUARD_SHARED(CXTPSkinManagerClass, pClass)
					{
						if (pClass->DrawThemeBackground(CDC::FromHandle(hDC), iPartId, iStateId,
														pRect))
						{
							HRGN hrgnClip = NULL;

							if (pClipRect && pRect && !EqualRect(pClipRect, pRect))
							{
								hrgnClip = CreateRectRgn(0, 0, 0, 0);
								if (hrgnClip != NULL)
								{
									if (GetClipRgn(hDC, hrgnClip) != 1)
									{
										DeleteObject(hrgnClip);
										hrgnClip = (HRGN)-1;
									}

									::IntersectClipRect(hDC, pClipRect->left, pClipRect->top,
														pClipRect->right, pClipRect->bottom);
								}
							}

							if (hrgnClip != NULL)
							{
								if (hrgnClip == (HRGN)-1)
								{
									ExtSelectClipRgn(hDC, NULL, RGN_COPY);
								}
								else
								{
									ExtSelectClipRgn(hDC, hrgnClip, RGN_COPY);
									DeleteObject(hrgnClip);
								}
							}

							return S_OK;
						}
					}
				}
			}
		}
	}

	typedef HRESULT(STDAPICALLTYPE * PFNDrawThemeBackground)(HTHEME, HDC, int, int, const RECT*,
															 const RECT*);
	return reinterpret_cast<PFNDrawThemeBackground>(
		pFunction->GetOriginal())(hTheme, hDC, iPartId, iStateId, pRect, pClipRect);
}

HRESULT STDAPICALLTYPE CXTPSkinManagerDetoursApiHook::OnHookGetThemeColor(HTHEME hTheme,
																		  int iPartId, int iStateId,
																		  int iPropID,
																		  COLORREF* pColor)
{
	XTP_SKINFRAMEWORK_MANAGE_STATE();

	CXTPSkinManagerDetoursApiFunction* pFunction = EnterHookedCall(xtpSkinDetoursApiGetThemeColor);
	if (NULL == pFunction)
		return E_FAIL;

	if (m_bInitialized && pFunction->IsHooked())
	{
		XTP_GUARD_SHARED_(CXTPSkinManager, XTPSkinManager(), pSkinManager)
		{
			CXTPSkinManagerClass* pClass = pSkinManager->FromHandle(hTheme);
			if (NULL != pClass)
			{
				if (pSkinManager->IsEnabled())
				{
					COLORREF clr = XTPAccessShared(pClass)->GetThemeColor(iPartId, iStateId,
																		  iPropID);
					if (clr != (COLORREF)-1)
					{
						*pColor = clr;
						return S_OK;
					}
				}
			}
		}
	}

	typedef HRESULT(STDAPICALLTYPE * PFNOnHookGetThemeColor)(HTHEME, int, int, int, COLORREF*);
	return reinterpret_cast<PFNOnHookGetThemeColor>(
		pFunction->GetOriginal())(hTheme, iPartId, iStateId, iPropID, pColor);
}

HRESULT STDAPICALLTYPE CXTPSkinManagerDetoursApiHook::OnHookGetThemeInt(HTHEME hTheme, int iPartId,
																		int iStateId, int iPropID,
																		int* piVal)
{
	XTP_SKINFRAMEWORK_MANAGE_STATE();

	CXTPSkinManagerDetoursApiFunction* pFunction = EnterHookedCall(xtpSkinDetoursApiGetThemeInt);
	if (NULL == pFunction)
		return E_FAIL;

	if (m_bInitialized && pFunction->IsHooked())
	{
		XTP_GUARD_SHARED_(CXTPSkinManager, XTPSkinManager(), pSkinManager)
		{
			CXTPSkinManagerClass* pClass = pSkinManager->FromHandle(hTheme);
			if (NULL != pClass)
			{
				if (pSkinManager->IsEnabled())
				{
					CXTPSkinManagerSchemaProperty* pProperty = XTPAccessShared(pClass)->GetProperty(
						XTP_SKINPROPERTY_INT, iPartId, iStateId, iPropID);
					if (NULL != pProperty)
					{
						if (NULL != piVal)
							*piVal = pProperty->iVal;

						return S_OK;
					}
				}
			}
		}
	}

	typedef HRESULT(STDAPICALLTYPE * PFNOnHookGetThemeInt)(HTHEME, int, int, int, int*);
	return reinterpret_cast<PFNOnHookGetThemeInt>(
		pFunction->GetOriginal())(hTheme, iPartId, iStateId, iPropID, piVal);
}

BOOL STDAPICALLTYPE CXTPSkinManagerDetoursApiHook::OnHookIsThemeActive()
{
	BOOL bResult = FALSE;

	XTP_SKINFRAMEWORK_MANAGE_STATE();

	CXTPSkinManagerDetoursApiFunction* pFunction = EnterHookedCall(xtpSkinDetoursApiIsThemeActive);
	if (NULL != pFunction)
	{
		if (m_bInitialized && pFunction->IsHooked())
		{
			// If enabled, no action required. It is assumed an application is always
			// themed if SkinFramework is enabled.
			bResult = XTPAccessShared(XTPSkinManager())->IsEnabled();
		}

		if (!bResult)
		{
			typedef BOOL(STDAPICALLTYPE * PFNOnHookIsThemeActive)();
			bResult = reinterpret_cast<PFNOnHookIsThemeActive>(pFunction->GetOriginal())();
		}
	}

	return bResult;
}

BOOL STDAPICALLTYPE CXTPSkinManagerDetoursApiHook::OnHookIsAppThemed()
{
	BOOL bResult = FALSE;

	XTP_SKINFRAMEWORK_MANAGE_STATE();

	CXTPSkinManagerDetoursApiFunction* pFunction = EnterHookedCall(xtpSkinDetoursApiIsAppThemed);
	if (NULL != pFunction)
	{
		if (m_bInitialized && pFunction->IsHooked())
		{
			// If enabled, no action required. It is assumed an application is always
			// themed if SkinFramework is enabled.
			bResult = XTPAccessShared(XTPSkinManager())->IsEnabled();
		}

		if (!bResult)
		{
			typedef BOOL(STDAPICALLTYPE * PFNOnHookIsAppThemed)();
			bResult = reinterpret_cast<PFNOnHookIsAppThemed>(pFunction->GetOriginal())();
		}
	}

	return bResult;
}

HRESULT STDAPICALLTYPE CXTPSkinManagerDetoursApiHook::OnHookGetThemePartSize(
	HTHEME hTheme, HDC hDC, int iPartId, int iStateId, RECT* pRect, THEMESIZE eSize, SIZE* pSize)
{
	XTP_SKINFRAMEWORK_MANAGE_STATE();

	CXTPSkinManagerDetoursApiFunction* pFunction = EnterHookedCall(
		xtpSkinDetoursApiGetThemePartSize);
	if (NULL == pFunction)
		return E_FAIL;

	if (m_bInitialized && pFunction->IsHooked())
	{
		XTP_GUARD_SHARED_(CXTPSkinManager, XTPSkinManager(), pSkinManager)
		{
			CXTPSkinManagerClass* pClass = pSkinManager->FromHandle(hTheme);
			if (NULL != pClass)
			{
				if (pSkinManager->IsEnabled())
				{
					if (XTPAccessShared(pClass)->GetThemePartSize(iPartId, iStateId, pRect, eSize,
																  pSize))
					{
						*pSize = XTP_DPI(*pSize);
						return S_OK;
					}
				}
			}
		}
	}

	typedef HRESULT(STDAPICALLTYPE * PFNOnHookGetThemePartSize)(HTHEME, HDC, int, int, RECT*,
																THEMESIZE, SIZE*);
	return reinterpret_cast<PFNOnHookGetThemePartSize>(
		pFunction->GetOriginal())(hTheme, hDC, iPartId, iStateId, pRect, eSize, pSize);
}

BOOL STDAPICALLTYPE CXTPSkinManagerDetoursApiHook::OnHookGetThemeSysBool(HTHEME hTheme, int iBoolId)
{
	if (NULL == hTheme)
		return FALSE;

	XTP_SKINFRAMEWORK_MANAGE_STATE();

	CXTPSkinManagerDetoursApiFunction* pFunction = EnterHookedCall(
		xtpSkinDetoursApiGetThemeSysBool);
	if (NULL == pFunction)
		return FALSE;

	if (m_bInitialized && pFunction->IsHooked())
	{
		XTP_GUARD_SHARED_(CXTPSkinManager, XTPSkinManager(), pSkinManager)
		{
			CXTPSkinManagerClass* pClass = pSkinManager->FromHandle(hTheme);
			if (NULL != pClass)
			{
				if (pSkinManager->IsEnabled())
					return pSkinManager->GetThemeSysBool(iBoolId);
			}
		}
	}

	typedef BOOL(STDAPICALLTYPE * PFNOnHookGetThemeSysBool)(HTHEME, int);
	return reinterpret_cast<PFNOnHookGetThemeSysBool>(pFunction->GetOriginal())(hTheme, iBoolId);
}

COLORREF STDAPICALLTYPE CXTPSkinManagerDetoursApiHook::OnHookGetThemeSysColor(HTHEME hTheme,
																			  int iColorId)
{
	XTP_SKINFRAMEWORK_MANAGE_STATE();

	CXTPSkinManagerDetoursApiFunction* pFunction = EnterHookedCall(
		xtpSkinDetoursApiGetThemeSysColor);
	if (NULL == pFunction)
		return 0;

	if (m_bInitialized && pFunction->IsHooked())
	{
		XTP_GUARD_SHARED_(CXTPSkinManager, XTPSkinManager(), pSkinManager)
		{
			CXTPSkinManagerClass* pClass = pSkinManager->FromHandle(hTheme);
			if (NULL != pClass && pSkinManager->IsEnabled())
				return pSkinManager->GetThemeSysColor(iColorId);
		}
	}

	typedef COLORREF(STDAPICALLTYPE * PFNOnHookGetThemeSysColor)(HTHEME, int);
	return reinterpret_cast<PFNOnHookGetThemeSysColor>(pFunction->GetOriginal())(hTheme, iColorId);
}

HRESULT STDAPICALLTYPE CXTPSkinManagerDetoursApiHook::OnHookGetCurrentThemeName(
	LPWSTR pszThemeFileName, int dwMaxNameChars, LPWSTR pszColorBuff, int cchMaxColorChars,
	LPWSTR pszSizeBuff, int cchMaxSizeChars)
{
	HRESULT hResult = E_FAIL;

	XTP_SKINFRAMEWORK_MANAGE_STATE();

	CXTPSkinManagerDetoursApiFunction* pFunction = EnterHookedCall(
		xtpSkinDetoursApiGetCurrentThemeName);
	if (NULL != pFunction)
	{
		BOOL bDefaultProcessing = TRUE;

		if (m_bInitialized && pFunction->IsHooked())
		{
			XTP_GUARD_SHARED_(CXTPSkinManager, XTPSkinManager(), pSkinManager)
			{
				if (pSkinManager->IsEnabled())
				{
					if (pSkinManager->GetResourceFile())
					{
						if (pszThemeFileName)
						{
							TCHAR szAppPath[MAX_PATH * 4];
							CString strThemeFileName;
							if (GetModuleFileName(NULL, szAppPath, _countof(szAppPath)) >= 0)
								strThemeFileName = szAppPath;
							else
								strThemeFileName = _T("In Memory");
							int nLength = strThemeFileName.GetLength();
							if (nLength < dwMaxNameChars)
								MBSTOWCS_S(pszThemeFileName, strThemeFileName,
										   XTPToSizeTChecked(nLength + 1));
						}

						if (pszColorBuff)
						{
							CString strColorBuff =
								XTPAccessShared(pSkinManager->GetResourceFile())->GetIniFileName();
							strColorBuff.MakeUpper();

							if (strColorBuff.Find(_T("METALLIC")) > 0)
								strColorBuff = _T("metallic");
							if (strColorBuff.Find(_T("BLUE")) > 0)
								strColorBuff = _T("normalcolor");
							if (strColorBuff.Find(_T("HOMESTEAD")) > 0)
								strColorBuff = _T("homestead");

							int nLength = strColorBuff.GetLength();
							if (nLength < cchMaxColorChars)
								MBSTOWCS_S(pszColorBuff, strColorBuff,
										   XTPToSizeTChecked(nLength + 1));
						}

						hResult = S_OK;
					}

					bDefaultProcessing = FALSE;
				}
			}
		}

		if (bDefaultProcessing)
		{
			typedef HRESULT(STDAPICALLTYPE * PFNOnHookGetCurrentThemeName)(LPWSTR, int, LPWSTR, int,
																		   LPWSTR, int);
			hResult = reinterpret_cast<PFNOnHookGetCurrentThemeName>(
				pFunction->GetOriginal())(pszThemeFileName, dwMaxNameChars, pszColorBuff,
										  cchMaxColorChars, pszSizeBuff, cchMaxSizeChars);
		}
	}

	return hResult;
}

#endif // XTP_SKINFRAMEWORK_USE_DETOURS_API_HOOK
