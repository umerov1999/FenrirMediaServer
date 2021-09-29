// XTPSkinManagerDetoursApiHook.h: interface for the CXTPSkinManagerDetoursApiHook class.
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
#if !defined(__XTPSKINMANAGERDETOURAPIHOOK_H__) && defined(XTP_SKINFRAMEWORK_USE_DETOURS_API_HOOK)
#	define __XTPSKINMANAGERDETOURAPIHOOK_H__
//}}AFX_CODEJOCK_PRIVATE

#	if _MSC_VER > 1000
#		pragma once
#	endif // _MSC_VER > 1000

#	include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

//{{AFX_CODEJOCK_PRIVATE

enum XTPSkinFrameworkDetoursApiFunctionIndex
{
	xtpSkinDetoursApiGetProcAddress,
	xtpSkinDetoursApiGetModuleHandleA,
	xtpSkinDetoursApiGetModuleHandleW,
	xtpSkinDetoursApiGetModuleHandleExA,
	xtpSkinDetoursApiGetModuleHandleExW,
	xtpSkinDetoursApiCreateThread,
	xtpSkinDetoursApiRegisterClassA,
	xtpSkinDetoursApiRegisterClassW,
	xtpSkinDetoursApiGetSysColor,
	xtpSkinDetoursApiGetSysColorBrush,
	xtpSkinDetoursApiDrawEdge,
	xtpSkinDetoursApiDrawFrameControl,
	xtpSkinDetoursApiSetScrollInfo,
	xtpSkinDetoursApiSetScrollPos,
	xtpSkinDetoursApiGetScrollInfo,
	xtpSkinDetoursApiEnableScrollBar,
	xtpSkinDetoursApiSystemParametersInfoA,
	xtpSkinDetoursApiSystemParametersInfoW,
	xtpSkinDetoursApiDefWindowProcA,
	xtpSkinDetoursApiDefWindowProcW,
	xtpSkinDetoursApiDefMDIChildProcA,
	xtpSkinDetoursApiDefMDIChildProcW,
	xtpSkinDetoursApiDefFrameProcA,
	xtpSkinDetoursApiDefFrameProcW,
	xtpSkinDetoursApiDefDlgProcA,
	xtpSkinDetoursApiDefDlgProcW,
	xtpSkinDetoursApiCallWindowProcA,
	xtpSkinDetoursApiCallWindowProcW,
	xtpSkinDetoursApiFillRect,
	xtpSkinDetoursApiDeleteObject,
	xtpSkinDetoursApiAdjustWindowRectEx,
	xtpSkinDetoursApiAdjustWindowRect,
	xtpSkinDetoursApiGetSystemMetrics,
	xtpSkinDetoursApiTrackPopupMenu,
	xtpSkinDetoursApiTrackPopupMenuEx,
	xtpSkinDetoursApiNtUserTrackPopupMenuEx,
	xtpSkinDetoursApiDrawMenuBar,
	xtpSkinDetoursApiGetMenuItemRect,
	xtpSkinDetoursApiDwmExtendFrameIntoClientArea,
	xtpSkinDetoursApiOpenThemeData,
	xtpSkinDetoursApiOpenThemeDataForDpi,
	xtpSkinDetoursApiOpenThemeDataEx,
	xtpSkinDetoursApiDrawThemeBackground,
	xtpSkinDetoursApiCloseThemeData,
	xtpSkinDetoursApiGetThemeColor,
	xtpSkinDetoursApiGetThemeInt,
	xtpSkinDetoursApiIsAppThemed,
	xtpSkinDetoursApiIsThemeActive,
	xtpSkinDetoursApiGetCurrentThemeName,
	xtpSkinDetoursApiGetThemeSysBool,
	xtpSkinDetoursApiGetThemeSysColor,
	xtpSkinDetoursApiGetThemePartSize,

	XTP_SKIN_DETOURS_API_HOOK_COUNT
};

class CXTPSkinManagerDetoursApiHook;

//////////////////////////////////////////////////////////////////////////
// CXTPSkinManagerDetoursApiFunction

class _XTP_EXT_CLASS CXTPSkinManagerDetoursApiFunction : public CXTPSynchronized
{
public:
	CXTPSkinManagerDetoursApiFunction(CXTPSkinManagerDetoursApiHook* pApiHook, HMODULE hModule,
									  LPCSTR pszFuncName, PVOID pfnOrig, PVOID pfnHook);

	virtual ~CXTPSkinManagerDetoursApiFunction();

public:
	BOOL Hook();
	BOOL Unhook();
	BOOL IsHooked() const;

	HMODULE GetModule() const;
	LPVOID GetOriginal() const;
	LPVOID GetTrueOriginal() const;
	LPVOID GetHook() const;
	LPCSTR GetFunctionName() const;

	static PVOID GetAfxWndProc();

private:
	CXTPSkinManagerDetoursApiHook* m_pApiHook;
	HMODULE m_hModule;
	PVOID m_pfnOrig;
	PVOID m_pfnHook;
	PVOID m_pfnTarget;
	char m_szFuncName[50];
};

AFX_INLINE BOOL CXTPSkinManagerDetoursApiFunction::IsHooked() const
{
	return NULL != m_pfnTarget;
}

AFX_INLINE HMODULE CXTPSkinManagerDetoursApiFunction::GetModule() const
{
	return m_hModule;
}

AFX_INLINE LPCSTR CXTPSkinManagerDetoursApiFunction::GetFunctionName() const
{
	ASSERT('\0' != m_szFuncName[0]);
	return m_szFuncName;
}

AFX_INLINE LPVOID CXTPSkinManagerDetoursApiFunction::GetOriginal() const
{
	return NULL != m_pfnTarget ? m_pfnTarget : m_pfnOrig;
}

AFX_INLINE LPVOID CXTPSkinManagerDetoursApiFunction::GetTrueOriginal() const
{
	return m_pfnOrig;
}

AFX_INLINE LPVOID CXTPSkinManagerDetoursApiFunction::GetHook() const
{
	return m_pfnHook;
}

/////////////////////////////////////////////////////////////////////////////
// CXTPSkinManagerDetoursApiHook

class _XTP_EXT_CLASS CXTPSkinManagerDetoursApiHook : public CXTPSkinManagerApiHookBase
{
	DECLARE_DYNAMIC(CXTPSkinManagerDetoursApiHook);

	struct APIHOOKINFO
	{
		XTPSkinFrameworkDetoursApiFunctionIndex nIndex;
		LPCSTR lpModuleName;
		LPCSTR lpRoutineName;
		LPVOID pfnHook;
		int nApplyOptions;
		bool (CXTPSystemVersion::*pfnVersionCheck)() const;
	};

public:
	CXTPSkinManagerDetoursApiHook();
	virtual ~CXTPSkinManagerDetoursApiHook();

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
	BOOL IsModuleExcluded(HMODULE hModule) const;
	CXTPSkinManagerDetoursApiFunction*
		GetHookedFunction(XTPSkinFrameworkDetoursApiFunctionIndex nIndex);
	CXTPSkinManagerDetoursApiFunction* GetHookedFunction(HMODULE hModule, LPCSTR pszFuncName);

private:
	static CXTPSkinManagerDetoursApiHook* AFX_CDECL GetInstance();

	void UnhookAllFunctions(BOOL bFinal);
	CXTPSkinManagerDetoursApiFunction* Hook(XTPSkinFrameworkDetoursApiFunctionIndex nIndex,
											LPCSTR pszModuleName, LPCSTR pszFuncName,
											LPVOID pfnHook);

	CXTPSkinManagerDetoursApiFunction* AddHook(XTPSkinFrameworkDetoursApiFunctionIndex nIndex,
											   HMODULE hModule, LPCSTR pszFuncName, LPVOID pfnOrig,
											   LPVOID pfnHook);

	BOOL RemoveHook(LPCSTR pszFuncName);

	static DWORD WINAPI ThreadProcHook(LPVOID lpParameter);
	static CXTPSkinManagerDetoursApiFunction* AFX_CDECL EnterHookedCall(HMODULE hModule,
																		LPCSTR pszFuncName);
	static CXTPSkinManagerDetoursApiFunction* AFX_CDECL
		EnterHookedCall(XTPSkinFrameworkDetoursApiFunctionIndex nIndex);

	static FARPROC WINAPI GetProcAddressWindows(HMODULE hModule, LPCSTR pszProcName);

	static FARPROC WINAPI OnHookGetProcAddress(HMODULE hmod, PCSTR pszProcName);
	static HMODULE WINAPI OnHookGetModuleHandleA(LPCSTR lpModuleName);
	static HMODULE WINAPI OnHookGetModuleHandleW(LPCWSTR lpModuleName);
	static HMODULE WINAPI OnHookGetModuleHandleExA(DWORD dwFlags, LPCSTR lpModuleName,
												   HMODULE* phModule);
	static HMODULE WINAPI OnHookGetModuleHandleExW(DWORD dwFlags, LPCWSTR lpModuleName,
												   HMODULE* phModule);
	static HANDLE WINAPI OnHookCreateThread(LPSECURITY_ATTRIBUTES lpThreadAttributes,
											UINT_PTR dwStackSize,
											LPTHREAD_START_ROUTINE lpStartAddress,
											LPVOID lpParameter, DWORD dwCreationFlags,
											LPDWORD lpThreadId);

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
	static BOOL WINAPI OnHookTrackPopupMenuExImpl(
		XTPSkinFrameworkDetoursApiFunctionIndex nFunctionIndex, HMENU hMenu, UINT uFlags, int x,
		int y, HWND hWnd, LPTPMPARAMS lptpParams);
	static BOOL WINAPI OnHookTrackPopupMenuEx(HMENU hMenu, UINT uFlags, int x, int y, HWND hWnd,
											  LPTPMPARAMS lptpParams);
	static BOOL WINAPI OnHookNtUserTrackPopupMenuEx(HMENU hMenu, UINT uFlags, int x, int y,
													HWND hWnd, LPTPMPARAMS lptpParams);
	static BOOL WINAPI OnHookDrawMenuBar(HWND hWnd);
	static BOOL WINAPI OnHookGetMenuItemRect(HWND hWnd, HMENU hMenu, UINT uItem, LPRECT lprcItem);
	static HRESULT WINAPI OnHookDwmExtendFrameIntoClientArea(HWND hWnd, LPCVOID pMarInset);

	static HTHEME STDAPICALLTYPE OnHookOpenThemeData(HWND hWnd, LPCWSTR pszClassList);
	static HTHEME STDAPICALLTYPE OnHookOpenThemeDataForDpi(HWND hwnd, LPCWSTR pszClassList,
														   UINT dpi);
	static HTHEME STDAPICALLTYPE OnHookOpenThemeDataEx(HWND hwnd, LPCWSTR pszClassList,
													   DWORD dwFlags);
	static HRESULT STDAPICALLTYPE OnHookCloseThemeData(HTHEME);

	static HRESULT STDAPICALLTYPE OnHookGetCurrentThemeName(LPWSTR pszThemeFileName,
															int dwMaxNameChars, LPWSTR pszColorBuff,
															int cchMaxColorChars,
															LPWSTR pszSizeBuff,
															int cchMaxSizeChars);
	static BOOL STDAPICALLTYPE OnHookGetThemeSysBool(HTHEME hTheme, int iBoolId);
	static COLORREF STDAPICALLTYPE OnHookGetThemeSysColor(HTHEME hTheme, int iColorId);
	static HRESULT STDAPICALLTYPE OnHookGetThemePartSize(HTHEME hTheme, HDC, int iPartId,
														 int iStateId, RECT* pRect, THEMESIZE eSize,
														 SIZE* pSize);
	static BOOL STDAPICALLTYPE OnHookIsAppThemed();
	static BOOL STDAPICALLTYPE OnHookIsThemeActive();
	static HRESULT STDAPICALLTYPE OnHookGetThemeColor(HTHEME hTheme, int iPartId, int iStateId,
													  int iPropID, COLORREF* pColor);
	static HRESULT STDAPICALLTYPE OnHookGetThemeInt(HTHEME hTheme, int iPartId, int iStateId,
													int iPropID, int* piVal);
	static HRESULT STDAPICALLTYPE OnHookDrawThemeBackground(HTHEME hTheme, HDC hDC, int iPartId,
															int iStateId, const RECT* pRect,
															const RECT*);

private:
	CXTPSkinManagerDetoursApiFunction* m_arrFunctions[XTP_SKIN_DETOURS_API_HOOK_COUNT];
	static BOOL m_bInitialized;
	long m_nLockCondition;
	CMap<HMODULE, HMODULE, LONG, LONG> m_mapModuleRefCount;
	CXTPThreadLocal<int> m_HackModuleOnLoadCounter;
	static const APIHOOKINFO m_apiHookInfos[];
	static CXTPModuleHandle m_hUxTheme;

	friend class CXTPSkinManagerDetoursApiFunction;
};

//}}AFX_CODEJOCK_PRIVATE

#	include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPSKINMANAGERDETOURAPIHOOK_H__) &&
	   // defined(XTP_SKINFRAMEWORK_USE_DETOURS_API_HOOK)
