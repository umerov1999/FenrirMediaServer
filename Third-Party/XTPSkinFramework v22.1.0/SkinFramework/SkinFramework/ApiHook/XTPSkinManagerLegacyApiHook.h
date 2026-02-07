// XTPSkinManagerLegacyApiHook.h: interface for the CXTPSkinManagerLegacyApiHook class.
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
#if !defined(__XTPSKINMANAGERLEGACYAPIHOOK_H__) && defined(XTP_SKINFRAMEWORK_USE_LEGACY_API_HOOK)
#	define __XTPSKINMANAGERLEGACYAPIHOOK_H__
//}}AFX_CODEJOCK_PRIVATE

#	if _MSC_VER > 1000
#		pragma once
#	endif // _MSC_VER > 1000

#	include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

class CXTPWinThemeWrapper;

//{{AFX_CODEJOCK_PRIVATE

enum XTPSkinFrameworkLegacyApiFunctionIndex
{
	xtpSkinLegacyApiLoadLibraryA_KERNEL32,
	xtpSkinLegacyApiLoadLibraryA_API_MS_WIN_CORE_LIBRARYLOADER_L1_1_0,
	xtpSkinLegacyApiLoadLibraryA_API_MS_WIN_CORE_LIBRARYLOADER_L1_1_1,
	xtpSkinLegacyApiLoadLibraryA_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_0,
	xtpSkinLegacyApiLoadLibraryA_API_MS_WIN_CORE_LIBRARYLOADER_PRIVATE_L1_1_0,
	xtpSkinLegacyApiLoadLibraryA_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_1,
	xtpSkinLegacyApiLoadLibraryA_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_2,
	xtpSkinLegacyApiLoadLibraryA_API_MS_WIN_CORE_LIBRARYLOADER_L2_1_0,
	xtpSkinLegacyApiLoadLibraryW_KERNEL32,
	xtpSkinLegacyApiLoadLibraryW_API_MS_WIN_CORE_LIBRARYLOADER_L1_1_0,
	xtpSkinLegacyApiLoadLibraryW_API_MS_WIN_CORE_LIBRARYLOADER_L1_1_1,
	xtpSkinLegacyApiLoadLibraryW_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_0,
	xtpSkinLegacyApiLoadLibraryW_API_MS_WIN_CORE_LIBRARYLOADER_PRIVATE_L1_1_0,
	xtpSkinLegacyApiLoadLibraryW_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_1,
	xtpSkinLegacyApiLoadLibraryW_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_2,
	xtpSkinLegacyApiLoadLibraryW_API_MS_WIN_CORE_LIBRARYLOADER_L2_1_0,
	xtpSkinLegacyApiLoadLibraryExA_KERNEL32,
	xtpSkinLegacyApiLoadLibraryExA_API_MS_WIN_CORE_LIBRARYLOADER_L1_1_0,
	xtpSkinLegacyApiLoadLibraryExA_API_MS_WIN_CORE_LIBRARYLOADER_L1_1_1,
	xtpSkinLegacyApiLoadLibraryExA_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_0,
	xtpSkinLegacyApiLoadLibraryExA_API_MS_WIN_CORE_LIBRARYLOADER_PRIVATE_L1_1_0,
	xtpSkinLegacyApiLoadLibraryExA_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_1,
	xtpSkinLegacyApiLoadLibraryExA_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_2,
	xtpSkinLegacyApiLoadLibraryExA_API_MS_WIN_CORE_LIBRARYLOADER_L2_1_0,
	xtpSkinLegacyApiLoadLibraryExW_KERNEL32,
	xtpSkinLegacyApiLoadLibraryExW_API_MS_WIN_CORE_LIBRARYLOADER_L1_1_0,
	xtpSkinLegacyApiLoadLibraryExW_API_MS_WIN_CORE_LIBRARYLOADER_L1_1_1,
	xtpSkinLegacyApiLoadLibraryExW_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_0,
	xtpSkinLegacyApiLoadLibraryExW_API_MS_WIN_CORE_LIBRARYLOADER_PRIVATE_L1_1_0,
	xtpSkinLegacyApiLoadLibraryExW_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_1,
	xtpSkinLegacyApiLoadLibraryExW_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_2,
	xtpSkinLegacyApiLoadLibraryExW_API_MS_WIN_CORE_LIBRARYLOADER_L2_1_0,
	xtpSkinLegacyApiFreeLibrary_KERNEL32,
	xtpSkinLegacyApiFreeLibrary_API_MS_WIN_CORE_LIBRARYLOADER_L1_1_0,
	xtpSkinLegacyApiFreeLibrary_API_MS_WIN_CORE_LIBRARYLOADER_L1_1_1,
	xtpSkinLegacyApiFreeLibrary_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_0,
	xtpSkinLegacyApiFreeLibrary_API_MS_WIN_CORE_LIBRARYLOADER_PRIVATE_L1_1_0,
	xtpSkinLegacyApiFreeLibrary_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_1,
	xtpSkinLegacyApiFreeLibrary_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_2,
	xtpSkinLegacyApiFreeLibrary_API_MS_WIN_CORE_LIBRARYLOADER_L2_1_0,
	xtpSkinLegacyApiFreeLibraryAndExitThread_KERNEL32,
	xtpSkinLegacyApiFreeLibraryAndExitThread_API_MS_WIN_CORE_LIBRARYLOADER_L1_1_0,
	xtpSkinLegacyApiFreeLibraryAndExitThread_API_MS_WIN_CORE_LIBRARYLOADER_L1_1_1,
	xtpSkinLegacyApiFreeLibraryAndExitThread_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_0,
	xtpSkinLegacyApiFreeLibraryAndExitThread_API_MS_WIN_CORE_LIBRARYLOADER_PRIVATE_L1_1_0,
	xtpSkinLegacyApiFreeLibraryAndExitThread_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_1,
	xtpSkinLegacyApiFreeLibraryAndExitThread_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_2,
	xtpSkinLegacyApiFreeLibraryAndExitThread_API_MS_WIN_CORE_LIBRARYLOADER_L2_1_0,
	xtpSkinLegacyApiGetProcAddress_KERNEL32,
	xtpSkinLegacyApiGetProcAddress_API_MS_WIN_CORE_LIBRARYLOADER_L1_1_0,
	xtpSkinLegacyApiGetProcAddress_API_MS_WIN_CORE_LIBRARYLOADER_L1_1_1,
	xtpSkinLegacyApiGetProcAddress_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_0,
	xtpSkinLegacyApiGetProcAddress_API_MS_WIN_CORE_LIBRARYLOADER_PRIVATE_L1_1_0,
	xtpSkinLegacyApiGetProcAddress_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_1,
	xtpSkinLegacyApiGetProcAddress_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_2,
	xtpSkinLegacyApiGetProcAddress_API_MS_WIN_CORE_LIBRARYLOADER_L2_1_0,
	xtpSkinLegacyApiGetModuleHandleA_KERNEL32,
	xtpSkinLegacyApiGetModuleHandleA_API_MS_WIN_CORE_LIBRARYLOADER_L1_1_0,
	xtpSkinLegacyApiGetModuleHandleA_API_MS_WIN_CORE_LIBRARYLOADER_L1_1_1,
	xtpSkinLegacyApiGetModuleHandleA_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_0,
	xtpSkinLegacyApiGetModuleHandleA_API_MS_WIN_CORE_LIBRARYLOADER_PRIVATE_L1_1_0,
	xtpSkinLegacyApiGetModuleHandleA_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_1,
	xtpSkinLegacyApiGetModuleHandleA_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_2,
	xtpSkinLegacyApiGetModuleHandleA_API_MS_WIN_CORE_LIBRARYLOADER_L2_1_0,
	xtpSkinLegacyApiGetModuleHandleW_KERNEL32,
	xtpSkinLegacyApiGetModuleHandleW_API_MS_WIN_CORE_LIBRARYLOADER_L1_1_0,
	xtpSkinLegacyApiGetModuleHandleW_API_MS_WIN_CORE_LIBRARYLOADER_L1_1_1,
	xtpSkinLegacyApiGetModuleHandleW_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_0,
	xtpSkinLegacyApiGetModuleHandleW_API_MS_WIN_CORE_LIBRARYLOADER_PRIVATE_L1_1_0,
	xtpSkinLegacyApiGetModuleHandleW_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_1,
	xtpSkinLegacyApiGetModuleHandleW_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_2,
	xtpSkinLegacyApiGetModuleHandleW_API_MS_WIN_CORE_LIBRARYLOADER_L2_1_0,
	xtpSkinLegacyApiGetModuleHandleExA_KERNEL32,
	xtpSkinLegacyApiGetModuleHandleExA_API_MS_WIN_CORE_LIBRARYLOADER_L1_1_0,
	xtpSkinLegacyApiGetModuleHandleExA_API_MS_WIN_CORE_LIBRARYLOADER_L1_1_1,
	xtpSkinLegacyApiGetModuleHandleExA_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_0,
	xtpSkinLegacyApiGetModuleHandleExA_API_MS_WIN_CORE_LIBRARYLOADER_PRIVATE_L1_1_0,
	xtpSkinLegacyApiGetModuleHandleExA_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_1,
	xtpSkinLegacyApiGetModuleHandleExA_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_2,
	xtpSkinLegacyApiGetModuleHandleExA_API_MS_WIN_CORE_LIBRARYLOADER_L2_1_0,
	xtpSkinLegacyApiGetModuleHandleExW_KERNEL32,
	xtpSkinLegacyApiGetModuleHandleExW_API_MS_WIN_CORE_LIBRARYLOADER_L1_1_0,
	xtpSkinLegacyApiGetModuleHandleExW_API_MS_WIN_CORE_LIBRARYLOADER_L1_1_1,
	xtpSkinLegacyApiGetModuleHandleExW_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_0,
	xtpSkinLegacyApiGetModuleHandleExW_API_MS_WIN_CORE_LIBRARYLOADER_PRIVATE_L1_1_0,
	xtpSkinLegacyApiGetModuleHandleExW_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_1,
	xtpSkinLegacyApiGetModuleHandleExW_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_2,
	xtpSkinLegacyApiGetModuleHandleExW_API_MS_WIN_CORE_LIBRARYLOADER_L2_1_0,
	xtpSkinLegacyApiCreateThread_KERNEL32,
	xtpSkinLegacyApiCreateThread_API_MS_WIN_CORE_PROCESSTHREADS_L1_1_1,
	xtpSkinLegacyApiCreateThread_API_MS_WIN_CORE_PROCESSTHREADS_L1_1_2,
	xtpSkinLegacyApiRegisterClassA,
	xtpSkinLegacyApiRegisterClassW,
	xtpSkinLegacyApiGetSysColor,
	xtpSkinLegacyApiGetSysColorBrush,
	xtpSkinLegacyApiDrawEdge,
	xtpSkinLegacyApiDrawFrameControl,
	xtpSkinLegacyApiSetScrollInfo,
	xtpSkinLegacyApiSetScrollPos,
	xtpSkinLegacyApiGetScrollInfo,
	xtpSkinLegacyApiEnableScrollBar,
	xtpSkinLegacyApiOpenThemeData,
	xtpSkinLegacyApiOpenThemeDataForDpi,
	xtpSkinLegacyApiOpenThemeDataEx,
	xtpSkinLegacyApiDrawThemeBackground,
	xtpSkinLegacyApiCloseThemeData,
	xtpSkinLegacyApiGetThemeColor,
	xtpSkinLegacyApiGetThemeInt,
	xtpSkinLegacyApiIsAppThemed,
	xtpSkinLegacyApiIsThemeActive,
	xtpSkinLegacyApiGetCurrentThemeName,
	xtpSkinLegacyApiGetThemeSysBool,
	xtpSkinLegacyApiGetThemeSysColor,
	xtpSkinLegacyApiGetThemePartSize,
	xtpSkinLegacyApiSystemParametersInfoA,
	xtpSkinLegacyApiSystemParametersInfoW,
	xtpSkinLegacyApiDefWindowProcA,
	xtpSkinLegacyApiDefWindowProcW,
	xtpSkinLegacyApiDefMDIChildProcA,
	xtpSkinLegacyApiDefMDIChildProcW,
	xtpSkinLegacyApiDefFrameProcA,
	xtpSkinLegacyApiDefFrameProcW,
	xtpSkinLegacyApiDefDlgProcA,
	xtpSkinLegacyApiDefDlgProcW,
	xtpSkinLegacyApiCallWindowProcA,
	xtpSkinLegacyApiCallWindowProcW,
	xtpSkinLegacyApiFillRect,
	xtpSkinLegacyApiDeleteObject,
	xtpSkinLegacyApiAdjustWindowRectEx,
	xtpSkinLegacyApiAdjustWindowRect,
	xtpSkinLegacyApiGetSystemMetrics,
	xtpSkinLegacyApiTrackPopupMenu,
	xtpSkinLegacyApiTrackPopupMenuEx_USER32,
	xtpSkinLegacyApiNtUserTrackPopupMenuEx_WIN32U,
	xtpSkinLegacyApiDrawMenuBar,
	xtpSkinLegacyApiGetMenuItemRect,
	xtpSkinLegacyApiDwmExtendFrameIntoClientArea,
	XTP_SKIN_LEGACY_API_HOOK_COUNT
};

class CXTPSkinManagerLegacyApiHook;
class CXTPSkinManagerModuleList;

class _XTP_EXT_CLASS CXTPSkinManagerLegacyApiFunction : public CXTPSynchronized
{
public:
	CXTPSkinManagerLegacyApiFunction(CXTPSkinManagerLegacyApiHook* pApiHook,
									 LPCSTR pszCalleeModName, LPCSTR pszFuncName, PROC pfnOrig,
									 PROC pfnHook, PROC pfnAltOrig = NULL);

	virtual ~CXTPSkinManagerLegacyApiFunction();

public:
	BOOL HookImport(CXTPSkinManagerModuleList* pModuleList);
	BOOL UnhookImport(CXTPSkinManagerModuleList* pModuleList);
	BOOL UnhookImport(HMODULE hmod);
	BOOL IsHooked() const;
	PROC GetOriginal() const;

	static PVOID GetAfxWndProc();

private:
	BOOL ReplaceInOneModule(LPCSTR pszCalleeModName, PROC pfnCurrent, PROC pfnNew,
							HMODULE hmodCaller, PROC pfnAltCurrent = NULL);
	BOOL ReplaceInAllModules(LPCSTR pszCalleeModName, PROC pfnCurrent, PROC pfnNew,
							 CXTPSkinManagerModuleList* pModuleList, PROC pfnAltCurrent = NULL);

private:
	CXTPSkinManagerLegacyApiHook* m_pApiHook;
	BOOL m_bHooked;
	char m_szCalleeModName[50];
	PROC m_pfnOrig;
	PROC m_pfnAltOrig;
	PROC m_pfnHook;
	char m_szFuncName[30];

	static PVOID sm_pvMaxAppAddr;

	friend class CXTPSkinManagerLegacyApiHook;
	friend class CXTPSkinObject;
};

AFX_INLINE BOOL CXTPSkinManagerLegacyApiFunction::IsHooked() const
{
	return m_bHooked;
}

AFX_INLINE PROC CXTPSkinManagerLegacyApiFunction::GetOriginal() const
{
	return m_pfnOrig;
}

class _XTP_EXT_CLASS CXTPSkinManagerLegacyApiHook : public CXTPSkinManagerApiHookBase
{
	DECLARE_DYNAMIC(CXTPSkinManagerLegacyApiHook);

	struct EXCLUDEDMODULE
	{
		HMODULE hModule;
		CString strModule;
		BOOL bWin9x;
	};

	struct APIHOOKINFO
	{
		XTPSkinFrameworkLegacyApiFunctionIndex nIndex;
		LPCSTR lpModuleName;
		LPCSTR lpRoutineName;
		PROC pfnHook;
		int nApplyOptions;
		bool (CXTPSystemVersion::*pfnVersionCheck)() const;
	};

public:
	CXTPSkinManagerLegacyApiHook();
	virtual ~CXTPSkinManagerLegacyApiHook();

	virtual void InitializeHookManagement();
	virtual void FinalizeHookManagement();
	virtual void ExcludeModule(LPCTSTR lpszModule, BOOL bWin9x);
	virtual LPVOID GetOriginalFunctionAddress(int nFunction) const;
	virtual BOOL AdjustWindowRectExOrig(LPRECT lpRect, DWORD dwStyle, BOOL bMenu, DWORD dwExStyle);
	virtual LRESULT CallWindowProcOrig(WNDPROC lpPrevWndFunc, HWND hWnd, UINT Msg, WPARAM wParam,
									   LPARAM lParam);
	virtual LPVOID GetOpenThemeDataOriginalPtr() const;
	virtual LPVOID GetCloseThemeDataOriginalPtr() const;
	virtual void ConnectWrapper(CXTPWinThemeWrapper* pThemeWrapper);

public:
	static CXTPSkinManagerLegacyApiHook* AFX_CDECL GetInstance();

	BOOL IsModuleExcluded(HMODULE hModule) const;

	CXTPSkinManagerLegacyApiFunction* GetHookedFunction(LPCSTR pszCalleeModName,
														LPCSTR pszFuncName);
	CXTPSkinManagerLegacyApiFunction*
		GetHookedFunction(XTPSkinFrameworkLegacyApiFunctionIndex nIndex);

	static FARPROC WINAPI GetProcAddressWindows(HMODULE hmod, LPCSTR pszProcName);
	static FARPROC WINAPI GetProcAddressManual(HMODULE hmod, LPCSTR pszProcName);
	static HMODULE WINAPI GetModuleHandleAWindows(LPCSTR pszModuleName);

protected:
	void UnhookAllFunctions(BOOL bFinal);
	void UnhookAllFunctions(HMODULE hModule);
	CXTPSkinManagerLegacyApiFunction* HookImport(XTPSkinFrameworkLegacyApiFunctionIndex nIndex,
												 LPCSTR pszCalleeModName, LPCSTR pszFuncName,
												 PROC pfnHook,
												 CXTPSkinManagerModuleList* pModuleList);

private:
	void ProcessExcludedModules();
	void IncrementAllModulesReferenceCounters(CXTPSkinManagerModuleList* pModuleList,
											  BOOL bIncrement = TRUE);

	CXTPSkinManagerLegacyApiFunction* AddHook(XTPSkinFrameworkLegacyApiFunctionIndex nIndex,
											  LPCSTR pszCalleeModName, LPCSTR pszFuncName,
											  PROC pfnOrig, PROC pfnHook,
											  CXTPSkinManagerModuleList* pModuleList);

	BOOL RemoveHook(LPCSTR pszCalleeModName, LPCSTR pszFuncName);

	void HackModuleOnLoad(HMODULE hmod, DWORD dwFlags);

	static CXTPSkinManagerLegacyApiFunction* AFX_CDECL EnterHookedCall(LPCSTR pszCalleeModName,
																	   LPCSTR pszFuncName);
	static CXTPSkinManagerLegacyApiFunction* AFX_CDECL
		EnterHookedCall(XTPSkinFrameworkLegacyApiFunctionIndex nIndex);

	static HMODULE WINAPI OnHookLoadLibraryA_KERNEL32(LPCSTR pszModuleName);
	static HMODULE WINAPI
		OnHookLoadLibraryA_API_MS_WIN_CORE_LIBRARYLOADER_L1_1_0(LPCSTR pszModuleName);
	static HMODULE WINAPI
		OnHookLoadLibraryA_API_MS_WIN_CORE_LIBRARYLOADER_L1_1_1(LPCSTR pszModuleName);
	static HMODULE WINAPI
		OnHookLoadLibraryA_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_0(LPCSTR pszModuleName);
	static HMODULE WINAPI
		OnHookLoadLibraryA_API_MS_WIN_CORE_LIBRARYLOADER_PRIVATE_L1_1_0(LPCSTR pszModuleName);
	static HMODULE WINAPI
		OnHookLoadLibraryA_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_1(LPCSTR pszModuleName);
	static HMODULE WINAPI
		OnHookLoadLibraryA_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_2(LPCSTR pszModuleName);
	static HMODULE WINAPI
		OnHookLoadLibraryA_API_MS_WIN_CORE_LIBRARYLOADER_L2_1_0(LPCSTR pszModuleName);
	static HMODULE WINAPI OnHookLoadLibraryW_KERNEL32(PCWSTR pszModuleName);
	static HMODULE WINAPI
		OnHookLoadLibraryW_API_MS_WIN_CORE_LIBRARYLOADER_L1_1_0(PCWSTR pszModuleName);
	static HMODULE WINAPI
		OnHookLoadLibraryW_API_MS_WIN_CORE_LIBRARYLOADER_L1_1_1(PCWSTR pszModuleName);
	static HMODULE WINAPI
		OnHookLoadLibraryW_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_0(PCWSTR pszModuleName);
	static HMODULE WINAPI
		OnHookLoadLibraryW_API_MS_WIN_CORE_LIBRARYLOADER_PRIVATE_L1_1_0(PCWSTR pszModuleName);
	static HMODULE WINAPI
		OnHookLoadLibraryW_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_1(PCWSTR pszModuleName);
	static HMODULE WINAPI
		OnHookLoadLibraryW_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_2(PCWSTR pszModuleName);
	static HMODULE WINAPI
		OnHookLoadLibraryW_API_MS_WIN_CORE_LIBRARYLOADER_L2_1_0(PCWSTR pszModuleName);
	static HMODULE WINAPI OnHookLoadLibraryExA_KERNEL32(LPCSTR pszModuleName, HANDLE hFile,
														DWORD dwFlags);
	static HMODULE WINAPI OnHookLoadLibraryExA_API_MS_WIN_CORE_LIBRARYLOADER_L1_1_0(
		LPCSTR pszModuleName, HANDLE hFile, DWORD dwFlags);
	static HMODULE WINAPI OnHookLoadLibraryExA_API_MS_WIN_CORE_LIBRARYLOADER_L1_1_1(
		LPCSTR pszModuleName, HANDLE hFile, DWORD dwFlags);
	static HMODULE WINAPI OnHookLoadLibraryExA_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_0(
		LPCSTR pszModuleName, HANDLE hFile, DWORD dwFlags);
	static HMODULE WINAPI OnHookLoadLibraryExA_API_MS_WIN_CORE_LIBRARYLOADER_PRIVATE_L1_1_0(
		LPCSTR pszModuleName, HANDLE hFile, DWORD dwFlags);
	static HMODULE WINAPI OnHookLoadLibraryExA_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_1(
		LPCSTR pszModuleName, HANDLE hFile, DWORD dwFlags);
	static HMODULE WINAPI OnHookLoadLibraryExA_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_2(
		LPCSTR pszModuleName, HANDLE hFile, DWORD dwFlags);
	static HMODULE WINAPI OnHookLoadLibraryExA_API_MS_WIN_CORE_LIBRARYLOADER_L2_1_0(
		LPCSTR pszModuleName, HANDLE hFile, DWORD dwFlags);
	static HMODULE WINAPI OnHookLoadLibraryExW_KERNEL32(PCWSTR pszModuleName, HANDLE hFile,
														DWORD dwFlags);
	static HMODULE WINAPI OnHookLoadLibraryExW_API_MS_WIN_CORE_LIBRARYLOADER_L1_1_0(
		PCWSTR pszModuleName, HANDLE hFile, DWORD dwFlags);
	static HMODULE WINAPI OnHookLoadLibraryExW_API_MS_WIN_CORE_LIBRARYLOADER_L1_1_1(
		PCWSTR pszModuleName, HANDLE hFile, DWORD dwFlags);
	static HMODULE WINAPI OnHookLoadLibraryExW_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_0(
		PCWSTR pszModuleName, HANDLE hFile, DWORD dwFlags);
	static HMODULE WINAPI OnHookLoadLibraryExW_API_MS_WIN_CORE_LIBRARYLOADER_PRIVATE_L1_1_0(
		PCWSTR pszModuleName, HANDLE hFile, DWORD dwFlags);
	static HMODULE WINAPI OnHookLoadLibraryExW_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_1(
		PCWSTR pszModuleName, HANDLE hFile, DWORD dwFlags);
	static HMODULE WINAPI OnHookLoadLibraryExW_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_2(
		PCWSTR pszModuleName, HANDLE hFile, DWORD dwFlags);
	static HMODULE WINAPI OnHookLoadLibraryExW_API_MS_WIN_CORE_LIBRARYLOADER_L2_1_0(
		PCWSTR pszModuleName, HANDLE hFile, DWORD dwFlags);
	static BOOL WINAPI OnHookFreeLibrary_KERNEL32(HMODULE hmod);
	static BOOL WINAPI OnHookFreeLibrary_API_MS_WIN_CORE_LIBRARYLOADER_L1_1_0(HMODULE hmod);
	static BOOL WINAPI OnHookFreeLibrary_API_MS_WIN_CORE_LIBRARYLOADER_L1_1_1(HMODULE hmod);
	static BOOL WINAPI OnHookFreeLibrary_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_0(HMODULE hmod);
	static BOOL WINAPI OnHookFreeLibrary_API_MS_WIN_CORE_LIBRARYLOADER_PRIVATE_L1_1_0(HMODULE hmod);
	static BOOL WINAPI OnHookFreeLibrary_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_1(HMODULE hmod);
	static BOOL WINAPI OnHookFreeLibrary_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_2(HMODULE hmod);
	static BOOL WINAPI OnHookFreeLibrary_API_MS_WIN_CORE_LIBRARYLOADER_L2_1_0(HMODULE hmod);
	static BOOL WINAPI OnHookFreeLibraryAndExitThread_KERNEL32(HMODULE hmod, DWORD dwExitCode);
	static BOOL WINAPI OnHookFreeLibraryAndExitThread_API_MS_WIN_CORE_LIBRARYLOADER_L1_1_0(
		HMODULE hmod, DWORD dwExitCode);
	static BOOL WINAPI OnHookFreeLibraryAndExitThread_API_MS_WIN_CORE_LIBRARYLOADER_L1_1_1(
		HMODULE hmod, DWORD dwExitCode);
	static BOOL WINAPI OnHookFreeLibraryAndExitThread_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_0(
		HMODULE hmod, DWORD dwExitCode);
	static BOOL WINAPI OnHookFreeLibraryAndExitThread_API_MS_WIN_CORE_LIBRARYLOADER_PRIVATE_L1_1_0(
		HMODULE hmod, DWORD dwExitCode);
	static BOOL WINAPI OnHookFreeLibraryAndExitThread_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_1(
		HMODULE hmod, DWORD dwExitCode);
	static BOOL WINAPI OnHookFreeLibraryAndExitThread_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_2(
		HMODULE hmod, DWORD dwExitCode);
	static BOOL WINAPI OnHookFreeLibraryAndExitThread_API_MS_WIN_CORE_LIBRARYLOADER_L2_1_0(
		HMODULE hmod, DWORD dwExitCode);
	static FARPROC WINAPI OnHookGetProcAddress_KERNEL32(HMODULE hmod, PCSTR pszProcName);
	static FARPROC WINAPI
		OnHookGetProcAddress_API_MS_WIN_CORE_LIBRARYLOADER_L1_1_0(HMODULE hmod, PCSTR pszProcName);
	static FARPROC WINAPI
		OnHookGetProcAddress_API_MS_WIN_CORE_LIBRARYLOADER_L1_1_1(HMODULE hmod, PCSTR pszProcName);
	static FARPROC WINAPI
		OnHookGetProcAddress_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_0(HMODULE hmod, PCSTR pszProcName);
	static FARPROC WINAPI OnHookGetProcAddress_API_MS_WIN_CORE_LIBRARYLOADER_PRIVATE_L1_1_0(
		HMODULE hmod, PCSTR pszProcName);
	static FARPROC WINAPI
		OnHookGetProcAddress_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_1(HMODULE hmod, PCSTR pszProcName);
	static FARPROC WINAPI
		OnHookGetProcAddress_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_2(HMODULE hmod, PCSTR pszProcName);
	static FARPROC WINAPI
		OnHookGetProcAddress_API_MS_WIN_CORE_LIBRARYLOADER_L2_1_0(HMODULE hmod, PCSTR pszProcName);
	static HMODULE WINAPI OnHookGetModuleHandleA_KERNEL32(LPCSTR lpModuleName);
	static HMODULE WINAPI
		OnHookGetModuleHandleA_API_MS_WIN_CORE_LIBRARYLOADER_L1_1_0(LPCSTR lpModuleName);
	static HMODULE WINAPI
		OnHookGetModuleHandleA_API_MS_WIN_CORE_LIBRARYLOADER_L1_1_1(LPCSTR lpModuleName);
	static HMODULE WINAPI
		OnHookGetModuleHandleA_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_0(LPCSTR lpModuleName);
	static HMODULE WINAPI
		OnHookGetModuleHandleA_API_MS_WIN_CORE_LIBRARYLOADER_PRIVATE_L1_1_0(LPCSTR lpModuleName);
	static HMODULE WINAPI
		OnHookGetModuleHandleA_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_1(LPCSTR lpModuleName);
	static HMODULE WINAPI
		OnHookGetModuleHandleA_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_2(LPCSTR lpModuleName);
	static HMODULE WINAPI
		OnHookGetModuleHandleA_API_MS_WIN_CORE_LIBRARYLOADER_L2_1_0(LPCSTR lpModuleName);
	static HMODULE WINAPI OnHookGetModuleHandleW_KERNEL32(LPCWSTR lpModuleName);
	static HMODULE WINAPI
		OnHookGetModuleHandleW_API_MS_WIN_CORE_LIBRARYLOADER_L1_1_0(LPCWSTR lpModuleName);
	static HMODULE WINAPI
		OnHookGetModuleHandleW_API_MS_WIN_CORE_LIBRARYLOADER_L1_1_1(LPCWSTR lpModuleName);
	static HMODULE WINAPI
		OnHookGetModuleHandleW_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_0(LPCWSTR lpModuleName);
	static HMODULE WINAPI
		OnHookGetModuleHandleW_API_MS_WIN_CORE_LIBRARYLOADER_PRIVATE_L1_1_0(LPCWSTR lpModuleName);
	static HMODULE WINAPI
		OnHookGetModuleHandleW_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_1(LPCWSTR lpModuleName);
	static HMODULE WINAPI
		OnHookGetModuleHandleW_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_2(LPCWSTR lpModuleName);
	static HMODULE WINAPI
		OnHookGetModuleHandleW_API_MS_WIN_CORE_LIBRARYLOADER_L2_1_0(LPCWSTR lpModuleName);
	static HMODULE WINAPI OnHookGetModuleHandleExA_KERNEL32(DWORD dwFlags, LPCSTR lpModuleName,
															HMODULE* phModule);
	static HMODULE WINAPI OnHookGetModuleHandleExA_API_MS_WIN_CORE_LIBRARYLOADER_L1_1_0(
		DWORD dwFlags, LPCSTR lpModuleName, HMODULE* phModule);
	static HMODULE WINAPI OnHookGetModuleHandleExA_API_MS_WIN_CORE_LIBRARYLOADER_L1_1_1(
		DWORD dwFlags, LPCSTR lpModuleName, HMODULE* phModule);
	static HMODULE WINAPI OnHookGetModuleHandleExA_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_0(
		DWORD dwFlags, LPCSTR lpModuleName, HMODULE* phModule);
	static HMODULE WINAPI OnHookGetModuleHandleExA_API_MS_WIN_CORE_LIBRARYLOADER_PRIVATE_L1_1_0(
		DWORD dwFlags, LPCSTR lpModuleName, HMODULE* phModule);
	static HMODULE WINAPI OnHookGetModuleHandleExA_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_1(
		DWORD dwFlags, LPCSTR lpModuleName, HMODULE* phModule);
	static HMODULE WINAPI OnHookGetModuleHandleExA_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_2(
		DWORD dwFlags, LPCSTR lpModuleName, HMODULE* phModule);
	static HMODULE WINAPI OnHookGetModuleHandleExA_API_MS_WIN_CORE_LIBRARYLOADER_L2_1_0(
		DWORD dwFlags, LPCSTR lpModuleName, HMODULE* phModule);
	static HMODULE WINAPI OnHookGetModuleHandleExW_KERNEL32(DWORD dwFlags, LPCWSTR lpModuleName,
															HMODULE* phModule);
	static HMODULE WINAPI OnHookGetModuleHandleExW_API_MS_WIN_CORE_LIBRARYLOADER_L1_1_0(
		DWORD dwFlags, LPCWSTR lpModuleName, HMODULE* phModule);
	static HMODULE WINAPI OnHookGetModuleHandleExW_API_MS_WIN_CORE_LIBRARYLOADER_L1_1_1(
		DWORD dwFlags, LPCWSTR lpModuleName, HMODULE* phModule);
	static HMODULE WINAPI OnHookGetModuleHandleExW_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_0(
		DWORD dwFlags, LPCWSTR lpModuleName, HMODULE* phModule);
	static HMODULE WINAPI OnHookGetModuleHandleExW_API_MS_WIN_CORE_LIBRARYLOADER_PRIVATE_L1_1_0(
		DWORD dwFlags, LPCWSTR lpModuleName, HMODULE* phModule);
	static HMODULE WINAPI OnHookGetModuleHandleExW_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_1(
		DWORD dwFlags, LPCWSTR lpModuleName, HMODULE* phModule);
	static HMODULE WINAPI OnHookGetModuleHandleExW_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_2(
		DWORD dwFlags, LPCWSTR lpModuleName, HMODULE* phModule);
	static HMODULE WINAPI OnHookGetModuleHandleExW_API_MS_WIN_CORE_LIBRARYLOADER_L2_1_0(
		DWORD dwFlags, LPCWSTR lpModuleName, HMODULE* phModule);
	static HANDLE WINAPI OnHookCreateThread_KERNEL32(LPSECURITY_ATTRIBUTES lpThreadAttributes,
													 UINT_PTR dwStackSize,
													 LPTHREAD_START_ROUTINE lpStartAddress,
													 LPVOID lpParameter, DWORD dwCreationFlags,
													 LPDWORD lpThreadId);
	static HANDLE WINAPI OnHookCreateThread_API_MS_WIN_CORE_PROCESSTHREADS_L1_1_1(
		LPSECURITY_ATTRIBUTES lpThreadAttributes, UINT_PTR dwStackSize,
		LPTHREAD_START_ROUTINE lpStartAddress, LPVOID lpParameter, DWORD dwCreationFlags,
		LPDWORD lpThreadId);
	static HANDLE WINAPI OnHookCreateThread_API_MS_WIN_CORE_PROCESSTHREADS_L1_1_2(
		LPSECURITY_ATTRIBUTES lpThreadAttributes, UINT_PTR dwStackSize,
		LPTHREAD_START_ROUTINE lpStartAddress, LPVOID lpParameter, DWORD dwCreationFlags,
		LPDWORD lpThreadId);

	static HMODULE WINAPI OnHookLoadLibraryA(XTPSkinFrameworkLegacyApiFunctionIndex nFunctionIndex,
											 LPCSTR pszModuleName);
	static HMODULE WINAPI OnHookLoadLibraryW(XTPSkinFrameworkLegacyApiFunctionIndex nFunctionIndex,
											 PCWSTR pszModuleName);
	static HMODULE WINAPI OnHookLoadLibraryExA(XTPSkinFrameworkLegacyApiFunctionIndex nFunctionIndex,
											   LPCSTR pszModuleName, HANDLE hFile, DWORD dwFlags);
	static HMODULE WINAPI OnHookLoadLibraryExW(XTPSkinFrameworkLegacyApiFunctionIndex nFunctionIndex,
											   PCWSTR pszModuleName, HANDLE hFile, DWORD dwFlags);
	static FARPROC WINAPI OnHookGetProcAddress(
		XTPSkinFrameworkLegacyApiFunctionIndex nFunctionIndex, HMODULE hmod, PCSTR pszProcName);
	static BOOL WINAPI OnHookFreeLibrary(XTPSkinFrameworkLegacyApiFunctionIndex nFunctionIndex,
										 HMODULE hmod);
	static BOOL WINAPI OnHookFreeLibraryAndExitThread(
		XTPSkinFrameworkLegacyApiFunctionIndex nFunctionIndex, HMODULE hmod, DWORD dwExitCode);
	static HMODULE WINAPI OnHookGetModuleHandleA(
		XTPSkinFrameworkLegacyApiFunctionIndex nFunctionIndex, LPCSTR lpModuleName);
	static HMODULE WINAPI OnHookGetModuleHandleW(
		XTPSkinFrameworkLegacyApiFunctionIndex nFunctionIndex, LPCWSTR lpModuleName);
	static HMODULE WINAPI
		OnHookGetModuleHandleExA(XTPSkinFrameworkLegacyApiFunctionIndex nFunctionIndex,
								 DWORD dwFlags, LPCSTR lpModuleName, HMODULE* phModule);
	static HMODULE WINAPI
		OnHookGetModuleHandleExW(XTPSkinFrameworkLegacyApiFunctionIndex nFunctionIndex,
								 DWORD dwFlags, LPCWSTR lpModuleName, HMODULE* phModule);
	static HANDLE STDAPICALLTYPE
		OnHookCreateThread(XTPSkinFrameworkLegacyApiFunctionIndex nFunctionIndex,
						   LPSECURITY_ATTRIBUTES lpThreadAttributes, UINT_PTR dwStackSize,
						   LPTHREAD_START_ROUTINE lpStartAddress, LPVOID lpParameter,
						   DWORD dwCreationFlags, LPDWORD lpThreadId);

	static ATOM WINAPI OnHookRegisterClassA(const WNDCLASSA* lpWndClass);
	static ATOM WINAPI OnHookRegisterClassW(const WNDCLASSW* lpWndClass);
	static int WINAPI OnHookSetScrollInfo(HWND hwnd, int fnBar, LPCSCROLLINFO lpsi, BOOL fRedraw);
	static int WINAPI OnHookSetScrollPos(HWND hwnd, int nBar, int nPos, BOOL redraw);
	static BOOL WINAPI OnHookAdjustWindowRectEx(LPRECT lpRect, DWORD dwStyle, BOOL bMenu,
												DWORD dwExStyle);
	static BOOL WINAPI OnHookAdjustWindowRect(LPRECT lpRect, DWORD dwStyle, BOOL bMenu);
	static int WINAPI OnHookGetSystemMetrics(int nIndex);
	static BOOL WINAPI OnHookEnableScrollBar(HWND hWnd, UINT wSBflags, UINT wArrows);
	static BOOL WINAPI OnHookGetScrollInfo(HWND hwnd, int nBar, LPSCROLLINFO lpsi);
	static DWORD WINAPI OnHookGetSysColor(int nIndex);
	static BOOL WINAPI OnHookDeleteObject(HGDIOBJ hObject);
	static BOOL WINAPI OnHookDrawEdge(HDC hdc, LPRECT qrc, UINT edge, UINT grfFlags);
	static BOOL WINAPI OnHookDrawFrameControl(HDC hDC, LPRECT rect, UINT, UINT);
	static HBRUSH WINAPI OnHookGetSysColorBrush(int nIndex);
	static BOOL WINAPI OnHookSystemParametersInfoA(UINT uiAction, UINT uiParam, PVOID pvParam,
												   UINT fWinIni);
	static BOOL WINAPI OnHookSystemParametersInfoW(UINT uiAction, UINT uiParam, PVOID pvParam,
												   UINT fWinIni);
	static LRESULT WINAPI OnHookDefWindowProcA(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
	static LRESULT WINAPI OnHookDefWindowProcW(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
	static LRESULT WINAPI OnHookDefFrameProcA(HWND hWnd, HWND hWndMDIClient, UINT Msg,
											  WPARAM wParam, LPARAM lParam);
	static LRESULT WINAPI OnHookDefFrameProcW(HWND hWnd, HWND hWndMDIClient, UINT Msg,
											  WPARAM wParam, LPARAM lParam);
	static LRESULT WINAPI OnHookDefDlgProcA(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
	static LRESULT WINAPI OnHookDefDlgProcW(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
	static LRESULT WINAPI OnHookDefMDIChildProcA(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
	static LRESULT WINAPI OnHookDefMDIChildProcW(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
	static LRESULT WINAPI OnHookCallWindowProcA(WNDPROC lpPrevWndFunc, HWND hWnd, UINT Msg,
												WPARAM wParam, LPARAM lParam);
	static LRESULT WINAPI OnHookCallWindowProcW(WNDPROC lpPrevWndFunc, HWND hWnd, UINT Msg,
												WPARAM wParam, LPARAM lParam);
	static int WINAPI OnHookFillRect(HDC hDC, CONST RECT* lprc, HBRUSH hbr);
	static BOOL WINAPI OnHookTrackPopupMenu(HMENU hMenu, UINT uFlags, int x, int y, int nReserved,
											HWND hWnd, CONST RECT* prcRect);
	static BOOL WINAPI OnHookTrackPopupMenuEx(XTPSkinFrameworkLegacyApiFunctionIndex nFunctionIndex,
											  HMENU hMenu, UINT uFlags, int x, int y, HWND hWnd,
											  LPTPMPARAMS lptpParams);
	static BOOL WINAPI OnHookTrackPopupMenuEx_USER32(HMENU hMenu, UINT uFlags, int x, int y,
													 HWND hWnd, LPTPMPARAMS lptpParams);
	static BOOL WINAPI OnHookNtUserTrackPopupMenuEx_WIN32U(HMENU hMenu, UINT uFlags, int x, int y,
														   HWND hWnd, LPTPMPARAMS lptpParams);
	static BOOL WINAPI OnHookDrawMenuBar(HWND hWnd);
	static BOOL WINAPI OnHookGetMenuItemRect(HWND hWnd, HMENU hMenu, UINT uItem, LPRECT lprcItem);
	static HRESULT STDAPICALLTYPE OnHookGetCurrentThemeName(LPWSTR pszThemeFileName,
															int dwMaxNameChars, LPWSTR pszColorBuff,
															int cchMaxColorChars,
															LPWSTR /*pszSizeBuff*/,
															int /*cchMaxSizeChars*/);
	static BOOL STDAPICALLTYPE OnHookGetThemeSysBool(HTHEME /*hTheme*/, int iBoolId);
	static COLORREF STDAPICALLTYPE OnHookGetThemeSysColor(HTHEME /*hTheme*/, int iColorId);
	static HRESULT STDAPICALLTYPE OnHookGetThemePartSize(HTHEME hTheme, HDC, int iPartId,
														 int iStateId, RECT* pRect, THEMESIZE eSize,
														 SIZE* pSize);
	static BOOL STDAPICALLTYPE OnHookIsAppThemed();
	static BOOL STDAPICALLTYPE OnHookIsThemeActive();
	static HRESULT STDAPICALLTYPE OnHookGetThemeColor(HTHEME hTheme, int iPartId, int iStateId,
													  int iPropID, COLORREF* pColor);
	static HRESULT STDAPICALLTYPE OnHookGetThemeInt(HTHEME hTheme, int iPartId, int iStateId,
													int iPropID, int* piVal);
	static HRESULT STDAPICALLTYPE OnHookCloseThemeData(HTHEME);
	static HRESULT STDAPICALLTYPE OnHookDrawThemeBackground(HTHEME hTheme, HDC hDC, int iPartId,
															int iStateId, const RECT* pRect,
															const RECT*);
	static HTHEME STDAPICALLTYPE OnHookOpenThemeData(HWND /*hWnd*/, LPCWSTR pszClassList);
	static HTHEME STDAPICALLTYPE OnHookOpenThemeDataForDpi(HWND hwnd, LPCWSTR pszClassList,
														   UINT dpi);
	static HTHEME STDAPICALLTYPE OnHookOpenThemeDataEx(HWND hwnd, LPCWSTR pszClassList,
													   DWORD dwFlags);
	static DWORD WINAPI ThreadProcHook(LPVOID lpParameter);
	static HRESULT WINAPI OnHookDwmExtendFrameIntoClientArea(HWND hWnd, LPCVOID pMarInset);

	LONG IncrementModuleReferenceCounter(HMODULE hModule);
	LONG DecrementModuleReferenceCounter(HMODULE hModule);

protected:
	CXTPSkinManagerLegacyApiFunction* m_arrFunctions[XTP_SKIN_LEGACY_API_HOOK_COUNT];
	static BOOL m_bInitialized;
	static BOOL m_bInitializationInProgress;
	CArray<EXCLUDEDMODULE, EXCLUDEDMODULE&> m_arrExcludedModules;
	BOOL m_bInAppCompatMode; // Win7+ only.
	long m_nLockCondition;
	CMap<HMODULE, HMODULE, LONG, LONG> m_mapModuleRefCount;
	CXTPThreadLocal<int> m_HackModuleOnLoadCounter;
	static const APIHOOKINFO m_apiHookInfos[];

	friend class CXTPSkinManagerLegacyApiFunction;
	friend class CXTPSkinManager;
};

//}}AFX_CODEJOCK_PRIVATE

#	include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPSKINMANAGERLEGACYAPIHOOK_H__) &&
	   // defined(XTP_SKINFRAMEWORK_USE_LEGACY_API_HOOK)
