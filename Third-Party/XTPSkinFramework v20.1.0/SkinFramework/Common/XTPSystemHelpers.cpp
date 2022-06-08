// XTPSystemHelpers.cpp
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

#include "Common/Base/Diagnostic/XTPDisableAdvancedWarnings.h"
#include <shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")
#pragma warning(disable : 4201)
#include "Common/Base/Diagnostic/XTPEnableAdvancedWarnings.h"

#include "Common/XTPTypeId.h"
#include "Common/XTPCasting.h"
#include "Common/XTPSystemHelpers.h"
#include "Common/XTPSynchro.h"
#include "Common/XTPApplication.h"
#include "Common/XTPSingleton.h"
#include "Common/XTPResourceManager.h"

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

#ifdef _DEBUG
#	define new DEBUG_NEW
#	undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define TRACE_ACCESSIBLE(x)

#ifndef VER_EQUAL
#	define VER_EQUAL 1
#endif

#ifndef VER_GREATER_EQUAL
#	define VER_GREATER_EQUAL 3
#endif

#ifndef VER_MINORVERSION
#	define VER_MINORVERSION 0x0000001
#endif

#ifndef VER_MAJORVERSION
#	define VER_MAJORVERSION 0x0000002
#endif

#ifndef VER_PLATFORMID
#	define VER_PLATFORMID 0x0000008
#endif

#ifndef VER_SERVICEPACKMINOR
#	define VER_SERVICEPACKMINOR 0x0000010
#endif

#ifndef VER_SERVICEPACKMAJOR
#	define VER_SERVICEPACKMAJOR 0x0000020
#endif

#define VER_SET_CONDITION_MASK(_m_, _t_, _c_)                                                      \
	((_m_) = m_pfnVerSetConditionMask((_m_), (_t_), (_c_)))

#ifndef LOAD_LIBRARY_AS_IMAGE_RESOURCE
#	define LOAD_LIBRARY_AS_IMAGE_RESOURCE 0x00000020
#endif

struct XTP_SHLWAPI
{
	typedef IStream*(WINAPI* PFNSHCreateMemStream)(const void* pInit, UINT cbInit);
	PFNSHCreateMemStream pfnSHCreateMemStream;

	typedef HRESULT(STDAPICALLTYPE* PFNSHCreateStreamOnFileA)(LPCSTR pszFile, DWORD grfMode,
															  IStream** ppstm);
	PFNSHCreateStreamOnFileA pfnSHCreateStreamOnFileA;

	typedef HRESULT(STDAPICALLTYPE* PFNSHCreateStreamOnFileW)(LPCWSTR pszFile, DWORD grfMode,
															  IStream** ppstm);
	PFNSHCreateStreamOnFileW pfnSHCreateStreamOnFileW;
};

static const XTP_SHLWAPI& GetShlwapi()
{
	static XTP_SHLWAPI api = { NULL };

	static BOOL bInitialized = FALSE;
	if (!bInitialized)
	{
		bInitialized = TRUE;

		CXTPModuleHandle hmodShlwapi(_T("Shlwapi.dll"));
		if (NULL != hmodShlwapi.GetHandle())
		{
			api.pfnSHCreateMemStream = XTPToFunctionPtr<XTP_SHLWAPI::PFNSHCreateMemStream>(
				::GetProcAddress(hmodShlwapi, "SHCreateMemStream"));
			api.pfnSHCreateStreamOnFileA = XTPToFunctionPtr<XTP_SHLWAPI::PFNSHCreateStreamOnFileA>(
				::GetProcAddress(hmodShlwapi, "SHCreateStreamOnFileA"));
			api.pfnSHCreateStreamOnFileW = XTPToFunctionPtr<XTP_SHLWAPI::PFNSHCreateStreamOnFileW>(
				::GetProcAddress(hmodShlwapi, "SHCreateStreamOnFileW"));
		}
	}

	return api;
}

//===========================================================================
// CReadOnlyStreamWrapper class
//===========================================================================

class CReadOnlyStreamWrapper : public IStream
{
	IStream* m_pStream;
	ULONG m_nRefs;

public:
	explicit CReadOnlyStreamWrapper(IStream* pStream)
		: m_pStream(pStream)
		, m_nRefs(1)
	{
		ASSERT(NULL != m_pStream);
	}

	virtual ~CReadOnlyStreamWrapper()
	{
		m_pStream->Release();
	}

	//
	// IUnknown
	//

	STDMETHODIMP QueryInterface(REFIID riid, LPVOID* ppvObject)
	{
		if (NULL == ppvObject)
			return E_POINTER;

		*ppvObject = NULL;

		if (0 == memcmp(&riid, &IID_IStream, sizeof(GUID)))
		{
			*ppvObject = static_cast<IStream*>(this);
			AddRef();
			return S_OK;
		}

		if (0 == memcmp(&riid, &IID_ISequentialStream, sizeof(GUID)))
		{
			*ppvObject = static_cast<ISequentialStream*>(this);
			AddRef();
			return S_OK;
		}

		return m_pStream->QueryInterface(riid, ppvObject);
	}

	STDMETHODIMP_(ULONG) AddRef()
	{
		return ++m_nRefs;
	}

	STDMETHODIMP_(ULONG) Release()
	{
		ULONG nRefs = --m_nRefs;
		if (0 == nRefs)
		{
			delete this;
		}

		return nRefs;
	}

	//
	// ISequentialStream
	//

	STDMETHODIMP Read(void* pv, ULONG cb, ULONG* pcbRead)
	{
		return m_pStream->Read(pv, cb, pcbRead);
	}

	STDMETHODIMP Write(const void* /*pv*/, ULONG /*cb*/, ULONG* /*pcbWritten*/)
	{
		return E_ACCESSDENIED;
	}

	//
	// IStream
	//

	STDMETHODIMP Seek(LARGE_INTEGER dlibMove, DWORD dwOrigin, ULARGE_INTEGER* plibNewPosition)
	{
		return m_pStream->Seek(dlibMove, dwOrigin, plibNewPosition);
	}

	STDMETHODIMP SetSize(ULARGE_INTEGER /*libNewSize*/)
	{
		return E_ACCESSDENIED;
	}

	STDMETHODIMP CopyTo(IStream* pstm, ULARGE_INTEGER cb, ULARGE_INTEGER* pcbRead,
						ULARGE_INTEGER* pcbWritten)
	{
		return m_pStream->CopyTo(pstm, cb, pcbRead, pcbWritten);
	}

	STDMETHODIMP Commit(DWORD /*grfCommitFlags*/)
	{
		return E_ACCESSDENIED;
	}

	STDMETHODIMP Revert()
	{
		return E_ACCESSDENIED;
	}

	STDMETHODIMP LockRegion(ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType)
	{
		return m_pStream->LockRegion(libOffset, cb, dwLockType);
	}

	STDMETHODIMP UnlockRegion(ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType)
	{
		return m_pStream->UnlockRegion(libOffset, cb, dwLockType);
	}

	STDMETHODIMP Stat(STATSTG* pstatstg, DWORD grfStatFlag)
	{
		return m_pStream->Stat(pstatstg, grfStatFlag);
	}

	STDMETHODIMP Clone(IStream** ppstm)
	{
		HRESULT hr = E_FAIL;
		if (NULL != ppstm)
		{
			IStream* pStreamClone = NULL;
			hr					  = m_pStream->Clone(&pStreamClone);
			if (SUCCEEDED(hr))
			{
				*ppstm = new CReadOnlyStreamWrapper(pStreamClone);
				hr	   = S_OK;
			}
		}
		else
		{
			hr = E_POINTER;
		}

		return hr;
	}
};

//===========================================================================
// CXTPSystemVersion class
//===========================================================================

CXTPSystemVersion::CXTPSystemVersion()
	: m_pfnVerifyVersionInfo(NULL)
	, m_pfnVerSetConditionMask(NULL)
{
	// load Kernel32.dll and check for VerifyVersionInfo API existence.
	m_modKernel32.Init(TEXT("Kernel32.dll"));
	if (m_modKernel32)
	{
#ifdef UNICODE
		m_modKernel32.GetProcAddress((FARPROC*)&m_pfnVerifyVersionInfo, "VerifyVersionInfoW");
		m_modKernel32.GetProcAddress((FARPROC*)&m_pfnVerSetConditionMask, "VerSetConditionMask");
#else
		m_modKernel32.GetProcAddress((FARPROC*)&m_pfnVerifyVersionInfo, "VerifyVersionInfoA");
		m_modKernel32.GetProcAddress((FARPROC*)&m_pfnVerSetConditionMask, "VerSetConditionMask");
#endif
	}

	// Obtain ntdll.dll exports
	m_modNtdll.Init(TEXT("ntdll.dll"));
	if (m_modNtdll)
	{
		m_modNtdll.GetProcAddress((FARPROC*)&m_pfnRtlGetVersion, "RtlGetVersion");
	}
}

LRESULT CXTPSystemVersion::VerifyVersion(BYTE dwConditionMask, DWORD dwMajorVersion,
										 DWORD dwMinorVersion, DWORD dwPlatformId) const
{
	if (m_pfnVerifyVersionInfo && m_pfnVerSetConditionMask)
	{
		// Initialize the OSVERSIONINFOEX structure.
		OSVERSIONINFOEX osvi;
		ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
		osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
		osvi.dwMajorVersion		 = dwMajorVersion;
		osvi.dwMinorVersion		 = dwMinorVersion;
		osvi.dwPlatformId		 = dwPlatformId;

		// Initialize the condition mask.
		DWORDLONG dwlConditionMask = 0;
		VER_SET_CONDITION_MASK(dwlConditionMask, VER_MAJORVERSION, dwConditionMask);
		VER_SET_CONDITION_MASK(dwlConditionMask, VER_MINORVERSION, dwConditionMask);
		VER_SET_CONDITION_MASK(dwlConditionMask, VER_PLATFORMID, dwConditionMask);

		// Perform the test.
		if (m_pfnVerifyVersionInfo(&osvi, VER_MAJORVERSION | VER_MINORVERSION | VER_PLATFORMID,
								   dwlConditionMask)
			!= 0)
		{
			return S_OK;
		}

		return S_FALSE;
	}

	return E_NOTIMPL;
}

LRESULT CXTPSystemVersion::GetVersion(BYTE dwConditionMask, DWORD dwMajorVersion,
									  DWORD dwMinorVersion, DWORD dwPlatformId) const
{
	// Initialize the OSVERSIONINFOEX structure.
	OSVERSIONINFO osvi;
	ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

	// C4996: 'GetVersionExA': was declared deprecated
#pragma warning(push)
#pragma warning(disable : 4996)
	// get the Windows OS version information.
	::GetVersionEx(&osvi);
#pragma warning(pop)

	if (dwConditionMask == VER_EQUAL)
	{
		if (osvi.dwPlatformId == dwPlatformId)
		{
			if (osvi.dwMajorVersion == dwMajorVersion && osvi.dwMinorVersion == dwMinorVersion)
			{
				return S_OK;
			}
		}
	}

	if (dwConditionMask == VER_GREATER_EQUAL)
	{
		if (osvi.dwPlatformId >= dwPlatformId)
		{
			if (osvi.dwMajorVersion > dwMajorVersion
				|| osvi.dwMajorVersion == dwMajorVersion && osvi.dwMinorVersion >= dwMinorVersion)
			{
				return S_OK;
			}
		}
	}

	return S_FALSE;
}

bool CXTPSystemVersion::VerifyWinVer(BYTE dwConditionMask, DWORD dwMajorVersion,
									 DWORD dwMinorVersion, DWORD dwPlatformId) const
{
	LRESULT lResult = VerifyVersion(dwConditionMask, dwMajorVersion, dwMinorVersion, dwPlatformId);

	if (lResult == E_NOTIMPL)
	{
		lResult = GetVersion(dwConditionMask, dwMajorVersion, dwMinorVersion, dwPlatformId);
	}

	if (lResult == S_OK)
	{
		return true;
	}

	return false;
}

_XTP_EXT_CLASS CXTPSystemVersion* AFXAPI XTPSystemVersion()
{
	return &CXTPSingleton<CXTPSystemVersion>::Instance();
}

bool CXTPSystemVersion::IsWin31() const
{
	static bool bResult = VerifyWinVer(VER_EQUAL, 3, 10, VER_PLATFORM_WIN32s);
	return bResult;
}

bool CXTPSystemVersion::IsWin95() const
{
	static bool bResult = VerifyWinVer(VER_EQUAL, 4, 0, VER_PLATFORM_WIN32_WINDOWS);
	return bResult;
}

bool CXTPSystemVersion::IsWin98() const
{
	static bool bResult = VerifyWinVer(VER_EQUAL, 4, 10, VER_PLATFORM_WIN32_WINDOWS);
	return bResult;
}

bool CXTPSystemVersion::IsWinME() const
{
	static bool bResult = VerifyWinVer(VER_EQUAL, 4, 90, VER_PLATFORM_WIN32_WINDOWS);
	return bResult;
}

bool CXTPSystemVersion::IsWin9x() const
{
	static bool bResult = IsWin95() || IsWin98() || IsWinME();
	return bResult;
}

bool CXTPSystemVersion::IsWinNT4() const
{
	static bool bResult = VerifyWinVer(VER_EQUAL, 4, 0, VER_PLATFORM_WIN32_NT);
	return bResult;
}

bool CXTPSystemVersion::IsWin2K() const
{
	static bool bResult = VerifyWinVer(VER_EQUAL, 5, 0, VER_PLATFORM_WIN32_NT);
	return bResult;
}

bool CXTPSystemVersion::IsWinXP() const
{
	static bool bResult = VerifyWinVer(VER_EQUAL, 5, 1, VER_PLATFORM_WIN32_NT);
	return bResult;
}

bool CXTPSystemVersion::IsWin95OrGreater() const
{
	static bool bResult = VerifyWinVer(VER_GREATER_EQUAL, 4, 0, VER_PLATFORM_WIN32_WINDOWS);
	return bResult;
}

bool CXTPSystemVersion::IsWin98OrGreater() const
{
	static bool bResult = VerifyWinVer(VER_GREATER_EQUAL, 4, 10, VER_PLATFORM_WIN32_WINDOWS);
	return bResult;
}

bool CXTPSystemVersion::IsWinMEOrGreater() const
{
	static bool bResult = VerifyWinVer(VER_GREATER_EQUAL, 4, 90, VER_PLATFORM_WIN32_WINDOWS);
	return bResult;
}

bool CXTPSystemVersion::IsWinNT4OrGreater() const
{
	static bool bResult = VerifyWinVer(VER_GREATER_EQUAL, 4, 0, VER_PLATFORM_WIN32_NT);
	return bResult;
}

bool CXTPSystemVersion::IsWin2KOrGreater() const
{
	static bool bResult = VerifyWinVer(VER_GREATER_EQUAL, 5, 0, VER_PLATFORM_WIN32_NT);
	return bResult;
}

bool CXTPSystemVersion::IsWinXPOrGreater() const
{
	static bool bResult = VerifyWinVer(VER_GREATER_EQUAL, 5, 1, VER_PLATFORM_WIN32_NT);
	return bResult;
}

bool CXTPSystemVersion::IsWinVistaOrGreater() const
{
	static bool bResult = VerifyWinVer(VER_GREATER_EQUAL, 6, 0, VER_PLATFORM_WIN32_NT);
	return bResult;
}

bool CXTPSystemVersion::IsWin7OrGreater() const
{
	static bool bResult = VerifyWinVer(VER_GREATER_EQUAL, 6, 1, VER_PLATFORM_WIN32_NT);
	return bResult;
}

bool CXTPSystemVersion::IsWin8OrGreater() const
{
	static bool bResult = VerifyWinVer(VER_GREATER_EQUAL, 6, 2, VER_PLATFORM_WIN32_NT);
	return bResult;
}

bool CXTPSystemVersion::IsWin81OrGreater() const
{
	static bool bResult = VerifyWinVer(VER_GREATER_EQUAL, 6, 3, VER_PLATFORM_WIN32_NT)
						  || IsWin10OrGreater();
	return bResult;
}

bool CXTPSystemVersion::IsWin10OrGreater() const
{
	static bool bResult = false, bChecked = false;
	if (!bChecked && NULL != m_pfnRtlGetVersion)
	{
		OSVERSIONINFOEXW vi;
		vi.dwOSVersionInfoSize = sizeof(vi);
		if (0 == m_pfnRtlGetVersion(&vi))
		{
			bResult	 = vi.dwMajorVersion >= 10;
			bChecked = true;
		}
	}

	return bResult;
}

DWORD CXTPSystemVersion::GetComCtlVersion() const
{
	static DWORD dwVersion = 0;
	if (0 == dwVersion)
	{
		dwVersion = CXTPModuleHandle(_T("comctl32.dll")).GetVersion();

		if (0 == dwVersion)
			dwVersion = MAKELONG(0, 4); // Old ComCtl32 had version 4.0
	}

	return dwVersion;
}

BOOL CXTPSystemVersion::IsLayoutRTLSupported() const
{
	return IsWin2KOrGreater() || GetSystemMetrics(SM_MIDEASTENABLED);
}

BOOL CXTPSystemVersion::IsClearTypeTextQualitySupported() const
{
	return IsWinXPOrGreater();
}

int CXTPSystemVersion::GetMaxCharSize() const
{
#ifdef _UNICODE
	return 1;
#else
	static int nMaxCharSize = -1;
	if (nMaxCharSize == -1)
	{
		CPINFO info;
		GetCPInfo(GetOEMCP(), &info);

		nMaxCharSize = XTPToIntChecked(info.MaxCharSize);
	}
	return nMaxCharSize;
#endif
}

//=============================================================================
// Multi-Monitor API
//=============================================================================

_XTP_EXT_CLASS CXTPMultiMonitor* AFX_CDECL XTPMultiMonitor()
{
	return &CXTPSingleton<CXTPMultiMonitor>::Instance();
}

CXTPMultiMonitor::CXTPMultiMonitor()
{
	ASSERT(sizeof(XTP_MONITORINFO) == 40);

	m_modUser32.Init(TEXT("User32.dll"));
	if (!m_modUser32)
	{
		TRACE(_T("WARNING: Could not locate User32.dll.\n"));
	}

	if (!m_modUser32
		|| !m_modUser32.GetProcAddress((FARPROC*)&m_pfnGetSystemMetrics, "GetSystemMetrics")
		|| !m_modUser32.GetProcAddress((FARPROC*)&m_pfnMonitorFromWindow, "MonitorFromWindow")
		|| !m_modUser32.GetProcAddress((FARPROC*)&m_pfnMonitorFromRect, "MonitorFromRect")
		|| !m_modUser32.GetProcAddress((FARPROC*)&m_pfnMonitorFromPoint, "MonitorFromPoint") ||
#ifndef UNICODE
		!m_modUser32.GetProcAddress((FARPROC*)&m_pfnGetMonitorInfo, "GetMonitorInfoA")
		|| !m_modUser32.GetProcAddress((FARPROC*)&m_pfnEnumDisplaySettingsEx,
									   "EnumDisplaySettingsExA"))
#else
		!m_modUser32.GetProcAddress((FARPROC*)&m_pfnGetMonitorInfo, "GetMonitorInfoW")
		|| !m_modUser32.GetProcAddress((FARPROC*)&m_pfnEnumDisplaySettingsEx,
									   "EnumDisplaySettingsExW"))
#endif
	{
		m_pfnGetSystemMetrics	   = NULL;
		m_pfnMonitorFromWindow	   = NULL;
		m_pfnMonitorFromRect	   = NULL;
		m_pfnMonitorFromPoint	   = NULL;
		m_pfnGetMonitorInfo		   = NULL;
		m_pfnEnumDisplaySettingsEx = NULL;
	}
}

BOOL CXTPMultiMonitor::GetMonitorInfo(XTP_HMONITOR hMonitor, XTP_MONITORINFO* lpMonitorInfo)
{
	return (hMonitor && m_pfnGetMonitorInfo ? m_pfnGetMonitorInfo(hMonitor, lpMonitorInfo) : FALSE);
}

CRect CXTPMultiMonitor::GetWorkArea(XTP_HMONITOR hMonitor)
{
	XTP_MONITORINFO info;
	info.cbSize = sizeof(info);
	if (GetMonitorInfo(hMonitor, &info))
	{
		return info.rcWork;
	}

	RECT rcWork;
	::SystemParametersInfo(SPI_GETWORKAREA, NULL, &rcWork, 0);
	return rcWork;
}

CRect CXTPMultiMonitor::GetScreenArea(XTP_HMONITOR hMonitor)
{
	XTP_MONITORINFO info;
	info.cbSize = sizeof(info);
	if (GetMonitorInfo(hMonitor, &info))
	{
		return info.rcMonitor;
	}

	return CRect(0, 0, ::GetSystemMetrics(SM_CXSCREEN), ::GetSystemMetrics(SM_CYSCREEN));
}

CRect CXTPMultiMonitor::GetScreenArea(HWND hWnd)
{
	return GetScreenArea(m_pfnMonitorFromWindow ? m_pfnMonitorFromWindow(hWnd, 0) : 0);
}

CRect CXTPMultiMonitor::GetScreenArea(const CWnd* pWnd)
{
	return GetScreenArea(pWnd->GetSafeHwnd());
}

CRect CXTPMultiMonitor::GetScreenArea(const POINT& ptScreenCoords)
{
	return GetScreenArea(m_pfnMonitorFromPoint ? m_pfnMonitorFromPoint(ptScreenCoords, 0) : 0);
}

CRect CXTPMultiMonitor::GetScreenArea(LPCRECT lprcScreenCoords)
{
	return GetScreenArea(m_pfnMonitorFromRect ? m_pfnMonitorFromRect(lprcScreenCoords, 0) : 0);
}

CRect CXTPMultiMonitor::GetWorkArea(HWND hWnd)
{
	return GetWorkArea(m_pfnMonitorFromWindow ? m_pfnMonitorFromWindow(hWnd, 0) : 0);
}

CRect CXTPMultiMonitor::GetWorkArea(const POINT& ptScreenCoords)
{
	return GetWorkArea(m_pfnMonitorFromPoint ? m_pfnMonitorFromPoint(ptScreenCoords, 0) : 0);
}

CRect CXTPMultiMonitor::GetWorkArea(LPCRECT lprcScreenCoords)
{
	return GetWorkArea(m_pfnMonitorFromRect ? m_pfnMonitorFromRect(lprcScreenCoords, 0) : 0);
}

CRect CXTPMultiMonitor::GetWorkArea(const CWnd* pWnd)
{
	return GetWorkArea(pWnd->GetSafeHwnd());
}

CRect CXTPMultiMonitor::GetWorkArea()
{
	POINT point;
	::GetCursorPos(&point);

	return GetWorkArea(point);
}

BOOL CXTPMultiMonitor::GetDisplaySettings(const CWnd* pWnd, DEVMODE* lpDevMode)
{
	return GetDisplaySettings(pWnd->GetSafeHwnd(), lpDevMode);
}

BOOL CXTPMultiMonitor::GetDisplaySettings(HWND hWnd, DEVMODE* lpDevMode)
{
	ASSERT(NULL != lpDevMode);

	if (NULL == m_pfnGetMonitorInfo || NULL == m_pfnEnumDisplaySettingsEx
		|| NULL == m_pfnMonitorFromWindow)
		return FALSE;

	XTP_HMONITOR hMonitor = m_pfnMonitorFromWindow(hWnd, 0 /*MONITOR_DEFAULTTONULL*/);
	if (NULL == hMonitor)
		return FALSE;

	XTP_MONITORINFOEX info;
	info.cbSize = sizeof(info);
	if (!GetMonitorInfo(hMonitor, &info))
		return FALSE;

	return m_pfnEnumDisplaySettingsEx(info.szDevice, ENUM_CURRENT_SETTINGS, lpDevMode, 0);
}

//=============================================================================
// CXTPModuleHandle
//=============================================================================

CXTPModuleHandle::CXTPModuleHandle()
	: m_hModule(0)
	, m_eModuleState(xtpModNone)
	, m_pfnDllGetVersion(NULL)
{
	::ZeroMemory(&m_dvInfo, sizeof(m_dvInfo));
}

CXTPModuleHandle::CXTPModuleHandle(LPCTSTR lpszModuleName)
	: m_hModule(0)
	, m_eModuleState(xtpModNone)
	, m_pfnDllGetVersion(NULL)
{
	::ZeroMemory(&m_dvInfo, sizeof(m_dvInfo));

	Init(lpszModuleName);
}

CXTPModuleHandle::~CXTPModuleHandle()
{
	VERIFY(FreeLibrary());
}

BOOL CXTPModuleHandle::Init(LPCTSTR lpszModuleName)
{
	// first, try to obtain the module that was mapped into
	// the address space of the calling process, if not found,
	// attempt to load the library.

	return (GetModuleHandle(lpszModuleName) || LoadLibrary(lpszModuleName));
}

BOOL CXTPModuleHandle::GetModuleHandle(LPCTSTR lpszModuleName)
{
	ASSERT(NULL != lpszModuleName);

	BOOL bObtained = (xtpModNone != m_eModuleState && IsModule(lpszModuleName)
					  && EnsureModuleState());
	if (!bObtained)
	{
		if (xtpModNone != m_eModuleState)
		{
			FreeLibrary();
		}

		if ((m_hModule = ::GetModuleHandle(lpszModuleName)) != NULL)
		{
			m_eModuleState	= xtpModMapped;
			m_strModuleName = lpszModuleName;
			bObtained		= TRUE;
		}
	}

	return bObtained;
}

BOOL CXTPModuleHandle::LoadLibrary(LPCTSTR lpszModuleName)
{
	ASSERT(NULL != lpszModuleName);

	BOOL bLoaded = (xtpModLoaded == m_eModuleState && IsModule(lpszModuleName));
	if (!bLoaded)
	{
		if (xtpModNone != m_eModuleState)
		{
			FreeLibrary();
		}

		if ((m_hModule = ::LoadLibrary(lpszModuleName)) != NULL)
		{
			m_eModuleState	= xtpModLoaded;
			m_strModuleName = lpszModuleName;
			bLoaded			= TRUE;
		}
	}

	return bLoaded;
}

BOOL CXTPModuleHandle::FreeLibrary()
{
	BOOL bRet = TRUE;

	if (NULL != m_hModule)
	{
		if (m_eModuleState == xtpModLoaded)
		{
			bRet = ::FreeLibrary(m_hModule);
		}

		m_hModule	   = NULL;
		m_eModuleState = xtpModNone;
		m_strModuleName.Empty();
		memset(&m_dvInfo, 0, sizeof(m_dvInfo));
	}

	return bRet;
}

BOOL CXTPModuleHandle::GetVersionInfo()
{
	BOOL bSuccess = FALSE;

	if (EnsureModuleState())
	{
		m_dvInfo.cbSize = sizeof(m_dvInfo);

		if (NULL == m_pfnDllGetVersion)
		{
			m_pfnDllGetVersion = XTPToFunctionPtr<PFNDLLVERSIONINFO>(
				::GetProcAddress(m_hModule, "DllGetVersion"));
		}

		if (NULL != m_pfnDllGetVersion)
		{
			bSuccess = (m_pfnDllGetVersion(&m_dvInfo) != NOERROR);
		}
	}

	return bSuccess;
}

DWORD CXTPModuleHandle::GetVersion()
{
	DWORD dwModVer = 0;

	if (EnsureModuleState())
	{
		if (m_dvInfo.cbSize == 0)
		{
			GetVersionInfo();
		}

		dwModVer = static_cast<DWORD>(MAKELONG(m_dvInfo.dwMinorVersion, m_dvInfo.dwMajorVersion));
		ASSERT(HIWORD(dwModVer) <= 0xFFFF);
		ASSERT(LOWORD(dwModVer) <= 0xFFFF);
	}

	return dwModVer;
}

BOOL CXTPModuleHandle::GetProcAddress(FARPROC* ppFnPtr, LPCSTR lpProcName,
									  DWORD dwMinVer /*= NULL*/)
{
	ASSERT(NULL != ppFnPtr);
	ASSERT(NULL != lpProcName);

	BOOL bSuccess = FALSE;

	if (EnsureModuleState())
	{
		bSuccess = TRUE;
		if (dwMinVer != NULL)
		{
			DWORD dwModVer = GetVersion();

			if ((HIWORD(dwMinVer) > HIWORD(dwModVer))
				|| (HIWORD(dwMinVer) == HIWORD(dwModVer)) && (LOWORD(dwMinVer) > LOWORD(dwModVer)))
			{
				*ppFnPtr = NULL;
				bSuccess = FALSE;
			}
		}

		if (bSuccess)
		{
			*ppFnPtr = ::GetProcAddress(m_hModule, lpProcName);
			bSuccess = (*ppFnPtr != NULL);
		}
	}

	return bSuccess;
}

BOOL CXTPModuleHandle::EnsureModuleState()
{
	BOOL bEnsured = FALSE;

	switch (m_eModuleState)
	{
		case xtpModMapped:
		{
			// Make sure the module is still loaded.
			bEnsured = IsModule(m_strModuleName, m_strModuleName.GetLength());
			if (!bEnsured)
			{
				// Apparently the library has been unloaded so the handle must be detached.
				FreeLibrary();
			}
			break;
		}
		case xtpModLoaded:
			// Nothing can happen to the loaded module until it's unloaded.
			bEnsured = TRUE;
			break;
		case xtpModNone:
			// No state
			break;
	}

	return bEnsured;
}

BOOL CXTPModuleHandle::IsModule(LPCTSTR lpName, int nLen /*= -1*/) const
{
	ASSERT(NULL != lpName);

	BOOL bConfirmed = FALSE;

	if (NULL != m_hModule)
	{
		TCHAR szModulePath[MAX_PATH + 1];
		DWORD cchPath = ::GetModuleFileName(m_hModule, szModulePath, _countof(szModulePath));
		if (0 < cchPath)
		{
			DWORD cchName = (0 <= nLen ? nLen : XTPToUIntChecked(_tcslen(lpName)));
			if (cchName <= cchPath)
			{
				bConfirmed = (0 == _tcsicmp(lpName, &szModulePath[cchPath] - cchName));
			}
		}
	}

	return bConfirmed;
}

//=============================================================================
// CXTPCriticalSection
//=============================================================================

CXTPCriticalSection::CXTPCriticalSection()
{
	::InitializeCriticalSection(&m_csMutex);
}

CXTPCriticalSection::~CXTPCriticalSection()
{
	::DeleteCriticalSection(&m_csMutex);
}

void CXTPCriticalSection::EnterCritical()
{
	::EnterCriticalSection(&m_csMutex);
}

void CXTPCriticalSection::LeaveCritical()
{
	::LeaveCriticalSection(&m_csMutex);
}

//=============================================================================
// CXTPLockGuard
//=============================================================================

CXTPLockGuard::CXTPLockGuard(CRITICAL_SECTION& key)
	: m_key(key)
{
	LockThread();
}

CXTPLockGuard::~CXTPLockGuard()
{
	UnLockThread();
}

void CXTPLockGuard::LockThread()
{
	::EnterCriticalSection(&m_key);
}

void CXTPLockGuard::UnLockThread()
{
	::LeaveCriticalSection(&m_key);
}

//////////////////////////////////////////////////////////////////////////
// CXTPAccessible

CXTPAccessible::CXTPAccessible()
{
	m_pNotifyWinEvent			  = NULL;
	m_pLresultFromObject		  = NULL;
	m_pAccessibleObjectFromWindow = NULL;

	m_hOleAcc = NULL;
	m_hUser32 = NULL;
}

CXTPAccessible::~CXTPAccessible()
{
	if (m_hOleAcc && m_hOleAcc != INVALID_HANDLE_VALUE)
	{
		FreeLibrary(m_hOleAcc);
	}
}

void CXTPAccessible::AccessibleNotifyWinEvent(DWORD event, HWND hwnd, LONG idObject, LONG idChild)
{
	if (m_hUser32 == INVALID_HANDLE_VALUE)
		return;

	if (m_hUser32 == NULL)
	{
		m_hUser32 = GetModuleHandle(_T("user32.dll"));
	}

	if (m_pNotifyWinEvent == NULL && m_hUser32)
	{
		m_pNotifyWinEvent = XTPToFunctionPtr<LPFNNOTIFYWINEVENT>(
			::GetProcAddress(m_hUser32, "NotifyWinEvent"));

		if (!m_pNotifyWinEvent)
		{
			m_hUser32 = (HMODULE)INVALID_HANDLE_VALUE;
		}
	}

	if (m_pNotifyWinEvent != NULL)
	{
		m_pNotifyWinEvent(event, hwnd, idObject, idChild);
	}
}

HRESULT CXTPAccessible::AccessibleObjectFromWindow(HWND hwnd, DWORD dwId, REFIID riid,
												   void** ppvObject)
{
	if (m_hOleAcc == INVALID_HANDLE_VALUE)
		return E_FAIL;

	if (m_hOleAcc == NULL)
	{
		m_hOleAcc = LoadLibrary(_T("oleacc.dll"));
	}

	if (m_hOleAcc != NULL && m_pAccessibleObjectFromWindow == NULL)
	{
		m_pAccessibleObjectFromWindow = XTPToFunctionPtr<LPFNACCESSIBLEOBJECTFROMWINDOW>(
			::GetProcAddress(m_hOleAcc, "AccessibleObjectFromWindow"));

		if (!m_pAccessibleObjectFromWindow)
		{
			FreeLibrary(m_hOleAcc);

			m_hOleAcc					  = (HMODULE)INVALID_HANDLE_VALUE;
			m_pLresultFromObject		  = NULL;
			m_pAccessibleObjectFromWindow = NULL;
		}
	}

	if (m_pAccessibleObjectFromWindow)
	{
		return m_pAccessibleObjectFromWindow(hwnd, dwId, riid, ppvObject);
	}
	return E_FAIL;
}

LRESULT CXTPAccessible::LresultFromObject(REFIID riid, WPARAM wParam, LPUNKNOWN punk)
{
	if (m_hOleAcc == INVALID_HANDLE_VALUE)
		return E_FAIL;

	if (m_hOleAcc == NULL)
	{
		m_hOleAcc = LoadLibrary(_T("oleacc.dll"));
	}

	if (m_hOleAcc != NULL && m_pLresultFromObject == NULL)
	{
		m_pLresultFromObject = XTPToFunctionPtr<LPFNLRESULTFROMOBJECT>(
			::GetProcAddress(m_hOleAcc, "LresultFromObject"));

		if (!m_pLresultFromObject)
		{
			FreeLibrary(m_hOleAcc);

			m_hOleAcc					  = (HMODULE)INVALID_HANDLE_VALUE;
			m_pLresultFromObject		  = NULL;
			m_pAccessibleObjectFromWindow = NULL;
		}
	}

	if (m_pLresultFromObject)
	{
		return m_pLresultFromObject(riid, wParam, punk);
	}

	return E_FAIL;
}

long CXTPAccessible::GetChildIndex(VARIANT* varChild)
{
	if (!varChild)
		return -1;
	if (varChild->vt == VT_EMPTY)
		return CHILDID_SELF;

	if (varChild->vt == VT_I4)
		return V_I4(varChild);
	if (varChild->vt == VT_I2)
		return V_I2(varChild);

	if (varChild->vt == (VT_I4 | VT_BYREF) && V_I4REF(varChild))
		return *V_I4REF(varChild);
	if (varChild->vt == (VT_I2 | VT_BYREF) && V_I2REF(varChild))
		return *V_I2REF(varChild);

	if (varChild->vt == (VT_VARIANT | VT_BYREF))
		return GetChildIndex(varChild->pvarVal);
	return -1;
}

XTP_STDMETHODIMP_(ULONG) CXTPAccessible::XAccessible::AddRef()
{
	METHOD_PROLOGUE_EX_(CXTPAccessible, ExternalAccessible)
	return (ULONG)pThis->GetAccessible()->ExternalAddRef();
}

XTP_STDMETHODIMP_(ULONG) CXTPAccessible::XAccessible::Release()
{
	METHOD_PROLOGUE_EX_(CXTPAccessible, ExternalAccessible)
	return (ULONG)pThis->GetAccessible()->ExternalRelease();
}

XTP_STDMETHODIMP CXTPAccessible::XAccessible::QueryInterface(REFIID iid, LPVOID* ppvObj)
{
	METHOD_PROLOGUE_EX_(CXTPAccessible, ExternalAccessible)
	return (HRESULT)pThis->GetAccessible()->ExternalQueryInterface(&iid, ppvObj);
}

XTP_STDMETHODIMP CXTPAccessible::XAccessible::GetTypeInfoCount(unsigned int*)
{
	return E_NOTIMPL;
}

XTP_STDMETHODIMP CXTPAccessible::XAccessible::GetTypeInfo(unsigned int, LCID, ITypeInfo**)
{
	return E_NOTIMPL;
}

XTP_STDMETHODIMP CXTPAccessible::XAccessible::GetIDsOfNames(REFIID, LPOLESTR*, unsigned int, LCID,
															DISPID*)
{
	return E_NOTIMPL;
}

XTP_STDMETHODIMP CXTPAccessible::XAccessible::get_accParent(IDispatch* FAR* ppdispParent)
{
	TRACE_ACCESSIBLE(_T("get_accParent\n"));
	METHOD_PROLOGUE_EX_(CXTPAccessible, ExternalAccessible);

	return pThis->GetAccessibleParent(ppdispParent);
}

XTP_STDMETHODIMP CXTPAccessible::XAccessible::get_accChildCount(long FAR* pChildCount)
{
	TRACE_ACCESSIBLE(_T("get_accChildCount\n"));
	METHOD_PROLOGUE_EX_(CXTPAccessible, ExternalAccessible);

	return pThis->GetAccessibleChildCount(pChildCount);
}

XTP_STDMETHODIMP CXTPAccessible::XAccessible::get_accChild(VARIANT varChild,
														   IDispatch* FAR* ppdispChild)
{
	TRACE_ACCESSIBLE(_T("get_accChild\n"));
	METHOD_PROLOGUE_EX_(CXTPAccessible, ExternalAccessible);

	return pThis->GetAccessibleChild(varChild, ppdispChild);
}

XTP_STDMETHODIMP CXTPAccessible::XAccessible::get_accName(VARIANT varChild, BSTR* pszName)
{
	TRACE_ACCESSIBLE(_T("get_accName\n"));
	METHOD_PROLOGUE_EX_(CXTPAccessible, ExternalAccessible);

	return pThis->GetAccessibleName(varChild, pszName);
}

XTP_STDMETHODIMP CXTPAccessible::XAccessible::get_accValue(VARIANT varChild, BSTR* pszValue)
{
	TRACE_ACCESSIBLE(_T("get_accValue\n"));
	METHOD_PROLOGUE_EX_(CXTPAccessible, ExternalAccessible);

	return pThis->GetAccessibleValue(varChild, pszValue);
}

XTP_STDMETHODIMP CXTPAccessible::XAccessible::get_accDescription(VARIANT varChild,
																 BSTR FAR* pszDescription)
{
	TRACE_ACCESSIBLE(_T("get_accDescription\n"));
	METHOD_PROLOGUE_EX_(CXTPAccessible, ExternalAccessible);

	return pThis->GetAccessibleDescription(varChild, pszDescription);
}

XTP_STDMETHODIMP CXTPAccessible::XAccessible::get_accRole(VARIANT varChild, VARIANT* pvarRole)
{
	TRACE_ACCESSIBLE(_T("get_accRole\n"));
	METHOD_PROLOGUE_EX_(CXTPAccessible, ExternalAccessible);

	return pThis->GetAccessibleRole(varChild, pvarRole);
}

XTP_STDMETHODIMP CXTPAccessible::XAccessible::get_accState(VARIANT varChild, VARIANT* pvarState)
{
	TRACE_ACCESSIBLE(_T("get_accState\n"));
	METHOD_PROLOGUE_EX_(CXTPAccessible, ExternalAccessible);

	return pThis->GetAccessibleState(varChild, pvarState);
}

XTP_STDMETHODIMP CXTPAccessible::XAccessible::get_accHelp(VARIANT varChild, BSTR* pszHelp)
{
	TRACE_ACCESSIBLE(_T("get_accHelp\n"));
	METHOD_PROLOGUE_EX_(CXTPAccessible, ExternalAccessible);

	return pThis->GetAccessibleHelp(varChild, pszHelp);
}

XTP_STDMETHODIMP CXTPAccessible::XAccessible::get_accHelpTopic(BSTR* pszHelpFile, VARIANT varChild,
															   long* pidTopic)
{
	TRACE_ACCESSIBLE(_T("get_accHelpTopic\n"));
	METHOD_PROLOGUE_EX_(CXTPAccessible, ExternalAccessible);

	return pThis->GetAccessibleHelpTopic(pszHelpFile, varChild, pidTopic);
}

XTP_STDMETHODIMP CXTPAccessible::XAccessible::get_accKeyboardShortcut(VARIANT varChild,
																	  BSTR* pszKeyboardShortcut)
{
	TRACE_ACCESSIBLE(_T("get_accKeyboardShortcut\n"));
	METHOD_PROLOGUE_EX_(CXTPAccessible, ExternalAccessible);

	return pThis->GetAccessibleKeyboardShortcut(varChild, pszKeyboardShortcut);
}

XTP_STDMETHODIMP CXTPAccessible::XAccessible::get_accFocus(VARIANT FAR* pvarFocusChild)
{
	TRACE_ACCESSIBLE(_T("get_accFocus\n"));
	METHOD_PROLOGUE_EX_(CXTPAccessible, ExternalAccessible);

	return pThis->GetAccessibleFocus(pvarFocusChild);
}

XTP_STDMETHODIMP CXTPAccessible::XAccessible::get_accSelection(VARIANT FAR* pvarSelectedChildren)
{
	TRACE_ACCESSIBLE(_T("get_accSelection\n"));
	METHOD_PROLOGUE_EX_(CXTPAccessible, ExternalAccessible);

	return pThis->GetAccessibleSelection(pvarSelectedChildren);
}

XTP_STDMETHODIMP CXTPAccessible::XAccessible::get_accDefaultAction(VARIANT varChild,
																   BSTR* pszDefaultAction)
{
	TRACE_ACCESSIBLE(_T("get_accDefaultAction\n"));
	METHOD_PROLOGUE_EX_(CXTPAccessible, ExternalAccessible);

	return pThis->GetAccessibleDefaultAction(varChild, pszDefaultAction);
}

XTP_STDMETHODIMP CXTPAccessible::XAccessible::accSelect(long flagsSelect, VARIANT varChild)
{
	TRACE_ACCESSIBLE(_T("accSelect\n"));
	METHOD_PROLOGUE_EX_(CXTPAccessible, ExternalAccessible);

	return pThis->AccessibleSelect(flagsSelect, varChild);
}

XTP_STDMETHODIMP CXTPAccessible::XAccessible::accLocation(long* pxLeft, long* pyTop, long* pcxWidth,
														  long* pcyHeight, VARIANT varChild)
{
	TRACE_ACCESSIBLE(_T("accLocation\n"));
	METHOD_PROLOGUE_EX_(CXTPAccessible, ExternalAccessible);

	return pThis->AccessibleLocation(pxLeft, pyTop, pcxWidth, pcyHeight, varChild);
}

XTP_STDMETHODIMP CXTPAccessible::XAccessible::accNavigate(long navDir, VARIANT varStart,
														  VARIANT* pvarEndUpAt)
{
	TRACE_ACCESSIBLE(_T("accNavigate\n"));
	METHOD_PROLOGUE_EX_(CXTPAccessible, ExternalAccessible);

	return pThis->AccessibleNavigate(navDir, varStart, pvarEndUpAt);
}

XTP_STDMETHODIMP CXTPAccessible::XAccessible::accHitTest(long xLeft, long yTop, VARIANT* pvarID)
{
	TRACE_ACCESSIBLE(_T("accHitTest\n"));
	METHOD_PROLOGUE_EX_(CXTPAccessible, ExternalAccessible);

	return pThis->AccessibleHitTest(xLeft, yTop, pvarID);
}

XTP_STDMETHODIMP CXTPAccessible::XAccessible::accDoDefaultAction(VARIANT varChild)
{
	TRACE_ACCESSIBLE(_T("accDoDefaultAction\n"));
	METHOD_PROLOGUE_EX_(CXTPAccessible, ExternalAccessible);

	return pThis->AccessibleDoDefaultAction(varChild);
}

XTP_STDMETHODIMP CXTPAccessible::XAccessible::put_accName(VARIANT varChild, BSTR szName)
{
	TRACE_ACCESSIBLE(_T("put_accName\n"));
	METHOD_PROLOGUE_EX_(CXTPAccessible, ExternalAccessible);

	return pThis->PutAccessibleName(varChild, szName);
}

XTP_STDMETHODIMP CXTPAccessible::XAccessible::put_accValue(VARIANT varChild, BSTR pszValue)
{
	TRACE_ACCESSIBLE(_T("put_accValue\n"));
	METHOD_PROLOGUE_EX_(CXTPAccessible, ExternalAccessible);

	return pThis->PutAccessibleValue(varChild, pszValue);
}

XTP_STDMETHODIMP CXTPAccessible::XAccessible::Invoke(DISPID /*dispid*/, REFIID, LCID,
													 unsigned short /*wFlags*/,
													 DISPPARAMS* /*pDispParams*/,
													 VARIANT* /*pvarResult*/, EXCEPINFO*,
													 unsigned int*)
{
	return E_NOTIMPL;
}

HRESULT CXTPAccessible::GetAccessibleParent(IDispatch** /*ppdispParent*/)
{
	return E_NOTIMPL;
}
HRESULT CXTPAccessible::GetAccessibleChildCount(long* /*pcountChildren*/)
{
	return E_NOTIMPL;
}
HRESULT CXTPAccessible::GetAccessibleChild(VARIANT /*varChild*/, IDispatch** /*ppdispChild*/)
{
	return E_NOTIMPL;
}
HRESULT CXTPAccessible::GetAccessibleName(VARIANT /*varChild*/, BSTR* /*pszName*/)
{
	return E_NOTIMPL;
}
HRESULT CXTPAccessible::GetAccessibleValue(VARIANT /*varChild*/, BSTR* /*pszValue*/)
{
	return E_NOTIMPL;
}
HRESULT CXTPAccessible::GetAccessibleDescription(VARIANT /*varChild*/, BSTR* /*pszDescription*/)
{
	return E_NOTIMPL;
}
HRESULT CXTPAccessible::GetAccessibleRole(VARIANT /*varChild*/, VARIANT* /*pvarRole*/)
{
	return E_NOTIMPL;
}
HRESULT CXTPAccessible::GetAccessibleState(VARIANT /*varChild*/, VARIANT* /*pvarState*/)
{
	return E_NOTIMPL;
}
HRESULT CXTPAccessible::GetAccessibleHelp(VARIANT /*varChild*/, BSTR* /*pszHelp*/)
{
	return E_NOTIMPL;
}
HRESULT CXTPAccessible::GetAccessibleHelpTopic(BSTR* /*pszHelpFile*/, VARIANT /*varChild*/,
											   long* /*pidTopic*/)
{
	return E_NOTIMPL;
}
HRESULT CXTPAccessible::GetAccessibleKeyboardShortcut(VARIANT /*varChild*/,
													  BSTR* /*pszKeyboardShortcut*/)
{
	return E_NOTIMPL;
}
HRESULT CXTPAccessible::GetAccessibleFocus(VARIANT* /*pvarChild*/)
{
	return S_FALSE;
}
HRESULT CXTPAccessible::GetAccessibleSelection(VARIANT* /*pvarChildren*/)
{
	return S_FALSE;
}
HRESULT CXTPAccessible::GetAccessibleDefaultAction(VARIANT /*varChild*/, BSTR* /*pszDefaultAction*/)
{
	return E_NOTIMPL;
}
HRESULT CXTPAccessible::AccessibleSelect(long /*flagsSelect*/, VARIANT /*varChild*/)
{
	return E_NOTIMPL;
}
HRESULT CXTPAccessible::AccessibleLocation(long* /*pxLeft*/, long* /*pyTop*/, long* /*pcxWidth*/,
										   long* /*pcyHeight*/, VARIANT /*varChild*/)
{
	return E_NOTIMPL;
}
HRESULT CXTPAccessible::AccessibleNavigate(long /*navDir*/, VARIANT /*varStart*/,
										   VARIANT* /*pvarEndUpAt*/)
{
	return E_NOTIMPL;
}
HRESULT CXTPAccessible::AccessibleHitTest(long /*xLeft*/, long /*yTop*/, VARIANT* /*pvarChild*/)
{
	return E_NOTIMPL;
}
HRESULT CXTPAccessible::AccessibleDoDefaultAction(VARIANT /*varChild*/)
{
	return E_NOTIMPL;
}
HRESULT CXTPAccessible::PutAccessibleName(VARIANT /*varChild*/, BSTR /*szName*/)
{
	return E_NOTIMPL;
}
HRESULT CXTPAccessible::PutAccessibleValue(VARIANT /*varChild*/, BSTR /*szValue*/)
{
	return E_NOTIMPL;
}
CCmdTarget* CXTPAccessible::GetAccessible()
{
	ASSERT(FALSE);
	return NULL;
}

//////////////////////////////////////////////////////////////////////////
// XTPGetThread

CWinThread* AFX_CDECL XTPGetThread()
{
	CWinThread* pThread = AfxGetThread();
	if (pThread != NULL)
		return pThread;

	return AfxGetApp();
}

//////////////////////////////////////////////////////////////////////////
// CXTPZipMemFile

#define ZIP_HEADER ('Z' + ('I' << 8) + ('P' << 16) + ('!' << 24))

CXTPZipMemFile::CXTPZipMemFile()
{
}

CXTPZipMemFile::CXTPZipMemFile(BYTE* lpBuffer, UINT nBufferSize)
{
	AttachCompressedBuffer(lpBuffer, nBufferSize);
}

CXTPZipMemFile::~CXTPZipMemFile()
{
}

void CXTPZipMemFile::AttachCompressedBuffer(BYTE* lpBuffer, UINT nBufferSize, BOOL bAutoDelete)
{
#ifdef ZLIB_H
	ZIPBUFFERINFO* pbi = (ZIPBUFFERINFO*)lpBuffer;

	if (nBufferSize < sizeof(ZIPBUFFERINFO) || pbi->bh.dwType != ZIP_HEADER)
	{
		Attach(lpBuffer, nBufferSize);
		m_bAutoDelete = bAutoDelete;
		return;
	}

	DWORD dwDestCount = pbi->bh.dwSize;

	BYTE* lpBufferDest = (BYTE*)malloc(dwDestCount);
	uncompress(lpBufferDest, &dwDestCount, (LPBYTE)&pbi->bBuffer,
			   nBufferSize - sizeof(ZIPBUFFERHEADER));

	ASSERT(dwDestCount == pbi->bh.dwSize);

	Attach(lpBufferDest, dwDestCount);
	m_bAutoDelete = TRUE;

	if (bAutoDelete)
	{
		free(lpBuffer);
	}
#else
	Attach(lpBuffer, nBufferSize);
	m_bAutoDelete = bAutoDelete;
#endif
}

BOOL CXTPZipMemFile::OpenCompressedFile(LPCTSTR lpszFileName)
{
	CFile file;
	if (!file.Open(lpszFileName, CFile::modeRead))
		return FALSE;

	UINT nSize		= (UINT)file.GetLength();
	LPBYTE lpBuffer = (LPBYTE)malloc(nSize);
	file.Read(lpBuffer, nSize);

	file.Close();

	AttachCompressedBuffer(lpBuffer, nSize, TRUE);

	return TRUE;
}

void CXTPZipMemFile::DetachCompressedBuffer(BYTE*& lpBuffer, DWORD& dwCount)
{
	dwCount	 = (DWORD)GetPosition();
	lpBuffer = Detach();

#ifdef ZLIB_H
	if (dwCount < 20)
		return;

	ZIPBUFFERINFO* pbi = (ZIPBUFFERINFO*)malloc(dwCount + sizeof(ZIPBUFFERHEADER));
	if (!pbi)
		return;

	DWORD dwDestCount = dwCount;

	if (compress((LPBYTE)&pbi->bBuffer, &dwDestCount, lpBuffer, dwCount) != Z_OK)
	{
		free(pbi);
		return;
	}

	pbi->bh.dwType = ZIP_HEADER;
	pbi->bh.dwSize = dwCount;

	free(lpBuffer);
	dwCount	 = dwDestCount + sizeof(ZIPBUFFERHEADER);
	lpBuffer = (LPBYTE)pbi;
#endif
}

#ifdef _UNICODE
#	define XTP_AW_WNDCLASSNAME(name) _T(name) _T("W")
#else
#	define XTP_AW_WNDCLASSNAME(name) _T(name) _T("A")
#endif

BOOL XTPLoadWindowClass_RichEdit(XTPWindowClassVersionSelectionVerb nVerb, int* pnVersion,
								 LPWNDCLASS pWndClass = NULL)
{
	ASSERT(NULL != pnVersion);
	ASSERT(xtpRichEditWindowClassVersion10 <= *pnVersion
		   && *pnVersion <= xtpRichEditWindowClassVersion70);

	BOOL bResult = FALSE;

	// Rich Edit versions:
	//	http://blogs.msdn.com/b/murrays/archive/2006/10/14/richedit-versions.aspx
	//	http://blogs.msdn.com/b/murrays/archive/2009/09/05/richedit-versions-update-to-7-0.aspx

	static struct MODULE_INFO
	{
		HINSTANCE hInst;
		LPCTSTR lpModuleName;
	} modules[] = {
		{ NULL, _T("riched32.dll") },
		{ NULL, _T("riched20.dll") },
		{ NULL, _T("msftedit.dll") },
	};

	static struct VERSION_INFO
	{
		XTPRichEditWindowClassVersion nVersion;
		LPCTSTR lpClassName;
		MODULE_INFO* module;

		BOOL Load(LPWNDCLASS pWndClass)
		{
			ASSERT(NULL != module);

			BOOL bLoaded = FALSE;
			if (NULL == module->hInst)
			{
				module->hInst = ::LoadLibrary(module->lpModuleName);
			}

			if (NULL != module->hInst)
			{
				WNDCLASS wc;
				bLoaded = ::GetClassInfo(NULL, lpClassName, &wc);
				if (bLoaded && NULL != pWndClass)
				{
					*pWndClass			 = wc;
					pWndClass->hInstance = module->hInst;
				}
			}

			return bLoaded;
		}
	} versions[] = {
		{ xtpRichEditWindowClassVersion10, XTP_AW_WNDCLASSNAME("RichEdit10"), &modules[0] },
		{ xtpRichEditWindowClassVersion20, XTP_AW_WNDCLASSNAME("RichEdit20"), &modules[1] },
		{ xtpRichEditWindowClassVersion30, XTP_AW_WNDCLASSNAME("RichEdit30"), &modules[1] },
		{ xtpRichEditWindowClassVersion31, XTP_AW_WNDCLASSNAME("RichEdit31"), &modules[1] },
		{ xtpRichEditWindowClassVersion40, XTP_AW_WNDCLASSNAME("RichEdit40"), &modules[1] },
		{ xtpRichEditWindowClassVersion41, XTP_AW_WNDCLASSNAME("RichEdit41"), &modules[2] },
		{ xtpRichEditWindowClassVersion50, XTP_AW_WNDCLASSNAME("RichEdit50"), &modules[2] },
		{ xtpRichEditWindowClassVersion60, XTP_AW_WNDCLASSNAME("RichEdit60"), &modules[2] },
		{ xtpRichEditWindowClassVersion70, XTP_AW_WNDCLASSNAME("RichEdit70"), &modules[2] },
	};

	int i = 0;
	switch (nVerb)
	{
		case xtpWindowClassVersionHighest:
			*pnVersion = xtpRichEditWindowClassVersion10;
			// no break
		case xtpWindowClassVersionAtLeast:
			for (i = _countof(versions) - 1; *pnVersion <= i; --i)
			{
				if (versions[i].Load(pWndClass))
				{
					bResult	   = TRUE;
					*pnVersion = i;
					break;
				}
			}
			break;
		case xtpWindowClassVersionExact: bResult = versions[*pnVersion].Load(pWndClass); break;
	}

	return bResult;
}

BOOL AFX_CDECL XTPLoadWindowClass(XTPWindowClass nClass, XTPWindowClassVersionSelectionVerb nVerb,
								  int* pnVersion, LPWNDCLASS pWndClass /*= NULL*/)
{
	BOOL bResult = FALSE;
	switch (nClass)
	{
		case xtpWindowClassRichEdit:
			bResult = XTPLoadWindowClass_RichEdit(nVerb, pnVersion, pWndClass);
			break;
			// TODO: Add more
	}

	return bResult;
}

///////////////////////////////////////////////////////////////////////////////
// XTPParseUri implementation

static HRESULT ResolveRelativePath(LPCTSTR lpszRelativePath, LPTSTR lpFullPath, DWORD cchFulPath)
{
	ASSERT(NULL != lpszRelativePath);
	ASSERT(NULL != lpFullPath);

	HRESULT hr = S_OK;

	if (0 != GetModuleFileName(NULL, lpFullPath, cchFulPath))
	{
		PathRemoveFileSpec(lpFullPath);
		size_t cchTotal = _tcslen(lpszRelativePath) + _tcslen(lpFullPath);
		if (cchTotal < cchFulPath)
		{
			if (!PathCombine(lpFullPath, lpFullPath, lpszRelativePath))
			{
				hr = HRESULT_FROM_WIN32(ERROR_FILENAME_EXCED_RANGE);
			}
		}
		else
		{
			hr = HRESULT_FROM_WIN32(ERROR_FILENAME_EXCED_RANGE);
		}
	}
	else
	{
		hr = HRESULT_FROM_WIN32(GetLastError());
	}

	return hr;
}

inline LPCTSTR GetUriResourceType(LPCTSTR lpszSource, const XTP_URI_DEFAULTS* pDefaults = NULL)
{
	LPCTSTR lpType = (NULL != pDefaults && NULL != pDefaults->res.lpType
						  ? pDefaults->res.lpType
						  : XTPGetDefaultResourceTypeFromPath(lpszSource));

	return lpType;
}

static HRESULT ParseResUri(LPCTSTR lpszResPath, XTP_URI* pUri,
						   const XTP_URI_DEFAULTS* pDefaults = NULL)
{
	ASSERT(NULL != lpszResPath);
	ASSERT(NULL != pUri);

	HRESULT hr = S_OK;

	do
	{
		static const TCHAR szResProtocol[] = _T("res://");
		ASSERT(lpszResPath == _tcsstr(lpszResPath, szResProtocol));
		lpszResPath += _countof(szResProtocol) - 1;

		// Parse ID part.
		LPCTSTR lpID	 = lpszResPath + _tcslen(lpszResPath);
		BOOL bSlashFound = FALSE;
		while (lpszResPath < lpID)
		{
			--lpID;
			if (_T('/') == *lpID)
			{
				++lpID;
				break;
			}
			else if (_T('\\') == *lpID)
			{
				bSlashFound = TRUE;
			}
		}

		if (lpszResPath == lpID)
		{
			if (!bSlashFound)
			{
				// No ID and slash found, assumed that the entire name is an ID.
				if (0
					== GetModuleFileName(NULL, pUri->res.szFilePath,
										 _countof(pUri->res.szFilePath)))
				{
					hr = HRESULT_FROM_WIN32(GetLastError());
					break;
				}

				pUri->res.lpType = GetUriResourceType(lpszResPath, pDefaults);

				if (_T('#') == *lpID)
				{
					pUri->res.lpName = MAKEINTRESOURCE(_ttoi(lpID + 1));
				}
				else
				{
					size_t cchID = _tcslen(lpID);
					if ((cchID + 1) < _countof(pUri->res.reserved.szName))
					{
						lstrcpyn(pUri->res.reserved.szName, lpID, XTPToIntChecked(cchID) + 1);
						pUri->res.reserved.szName[cchID] = _T('\0');
						pUri->res.lpName				 = pUri->res.reserved.szName;
					}
					else
					{
						hr = HRESULT_FROM_WIN32(ERROR_BUFFER_OVERFLOW);
						break;
					}
				}
			}
			else
			{
				hr = HRESULT_FROM_WIN32(ERROR_PATH_NOT_FOUND);
			}

			break;
		}

		long nID = _ttol(lpID);
		if (0 < nID)
		{
			pUri->res.lpName = MAKEINTRESOURCE(nID);
		}
		else
		{
			pUri->res.lpName = lpID;
		}

		// Part TYPE part.
		LPCTSTR lpType = NULL;
		if (lpszResPath < lpID)
		{
			lpType = lpID - 1;
			while (lpszResPath < lpType)
			{
				if (_T('/') == *--lpType)
				{
					++lpType;
					break;
				}
			}

			if (lpszResPath == lpType)
			{
				lpType = NULL;
			}
		}

		if (NULL != lpType)
		{
			if (lpszResPath == lpType)
			{
				hr = HRESULT_FROM_WIN32(ERROR_PATH_NOT_FOUND);
				break;
			}

			INT_PTR cchType = lpID - lpType;
			if (_countof(pUri->res.reserved.szType) < cchType)
			{
				hr = HRESULT_FROM_WIN32(ERROR_BUFFER_OVERFLOW);
				break;
			}

			lstrcpyn(pUri->res.reserved.szType, lpType, XTPToIntChecked(cchType));
			pUri->res.reserved.szType[cchType - 1] = _T('\0');

			long nType = _ttol(pUri->res.reserved.szType);
			if (0 != nType)
			{
				pUri->res.lpType = MAKEINTRESOURCE(nType);
			}
			else
			{
				pUri->res.lpType = pUri->res.reserved.szType;
			}
		}
		else
		{
			pUri->res.lpType = GetUriResourceType(lpszResPath, pDefaults);
		}

		// Parse FILE part.
		if (_T('.') == *lpszResPath && ((lpszResPath + 2) == lpType || (lpszResPath + 2) == lpID))
		{
			if (0 == GetModuleFileName(NULL, pUri->res.szFilePath, _countof(pUri->res.szFilePath)))
			{
				hr = HRESULT_FROM_WIN32(GetLastError());
				break;
			}
		}
		else
		{
			TCHAR szRelativePath[MAX_PATH + 1];
			LPCTSTR lpPathEnd = (NULL != lpType ? lpType : lpID);
			INT_PTR cchPath	  = lpPathEnd - lpszResPath;
			if (_countof(szRelativePath) < cchPath)
			{
				hr = HRESULT_FROM_WIN32(ERROR_BUFFER_OVERFLOW);
				break;
			}

			lstrcpyn(szRelativePath, lpszResPath, XTPToIntChecked(cchPath));
			szRelativePath[cchPath - 1] = _T('\0');

			hr = ResolveRelativePath(szRelativePath, pUri->res.szFilePath,
									 _countof(pUri->res.szFilePath));
			if (FAILED(hr))
			{
				break;
			}
		}
	} while (FALSE);

	return hr;
}

HRESULT AFX_CDECL XTPParseUri(LPCTSTR lpszUri, XTP_URI* pUri,
							  const XTP_URI_DEFAULTS* pDefaults /*= NULL*/)
{
	ASSERT(NULL != lpszUri);
	ASSERT(NULL != pUri);

	HRESULT hr = S_OK;

	if (NULL != lpszUri && NULL != pUri)
	{
		static const TCHAR szFileProtocol[] = _T("file://");
		static const TCHAR szResProtocol[]	= _T("res://");
		if (0 == _tcsncmp(lpszUri, szFileProtocol, _countof(szFileProtocol) - 1))
		{
			pUri->type = xtpUriFile;
			hr		   = ResolveRelativePath(lpszUri + _countof(szFileProtocol) - 1, pUri->file,
									 _countof(pUri->file));
		}
		else if (0 == _tcsncmp(lpszUri, szResProtocol, _countof(szResProtocol) - 1))
		{
			pUri->type = xtpUriRes;
			hr		   = ParseResUri(lpszUri, pUri, pDefaults);
		}
		else
		{
			pUri->type = xtpUriFile;
			hr		   = ResolveRelativePath(lpszUri, pUri->file, _countof(pUri->file));
		}
	}
	else
	{
		hr = E_POINTER;
	}

	return hr;
}

static HRESULT CreateStreamOnResourceUri(const XTP_URI* pUri, IStream** ppStream)
{
	ASSERT(NULL != pUri);
	ASSERT(NULL != ppStream);

	HMODULE hModule = LoadLibraryEx(pUri->res.szFilePath, NULL,
									LOAD_LIBRARY_AS_DATAFILE | LOAD_LIBRARY_AS_IMAGE_RESOURCE);
	if (NULL == hModule)
		return HRESULT_FROM_WIN32(GetLastError());

	LPCTSTR lpType = pUri->res.lpType;
	CString strType;
	if (NULL == lpType)
	{
		strType = XTPFindResourceType(hModule, pUri->res.lpName);
		lpType	= strType;
	}

	HRESULT hr = XTPCreateReadOnlyResourceStream(hModule, pUri->res.lpName, lpType, TRUE, ppStream);

	::FreeLibrary(hModule);

	return hr;
}

HRESULT AFX_CDECL XTPCreateReadOnlyStreamOnUri(const XTP_URI* pUri, IStream** ppStream)
{
	ASSERT(NULL != pUri);
	ASSERT(NULL != ppStream);

	HRESULT hr = E_FAIL;

	switch (pUri->type)
	{
		case xtpUriFile: hr = XTPCreateReadOnlyFileStream(pUri->file, ppStream); break;
		case xtpUriRes: hr = CreateStreamOnResourceUri(pUri, ppStream); break;
		default: hr = E_INVALIDARG; break;
	}

	return hr;
}

HRESULT AFX_CDECL XTPCreateReadOnlyStreamOnUri(LPCTSTR lpszUri, IStream** ppStream,
											   const XTP_URI_DEFAULTS* pDefaults /*= NULL*/)
{
	XTP_URI uri;
	HRESULT hr = XTPParseUri(lpszUri, &uri, pDefaults);
	if (SUCCEEDED(hr))
	{
		hr = XTPCreateReadOnlyStreamOnUri(&uri, ppStream);
	}

	return hr;
}

///////////////////////////////////////////////////////////////////////////////
// XTPLoadUnicodeText implementation

inline UINT StrNLen(LPCSTR lpString, UINT nMaxLen)
{
	ASSERT(NULL != lpString);

	UINT cch = 0;
	while ('\0' != *lpString++ && cch < nMaxLen)
	{
		++cch;
	}

	return cch;
}

inline BOOL WcsCchCopyN(LPWSTR lpDest, UINT& cchDest, LPCWSTR lpSrc, UINT cchSrc)
{
	ASSERT(NULL != lpDest);
	ASSERT(NULL != lpSrc);

	BOOL bSuccess = FALSE;
	UINT cchMax	  = cchDest - 1;
	if (cchSrc <= cchMax)
	{
		UINT cch = 0;
		while (L'\0' != *lpSrc && cch < cchSrc)
		{
			lpDest[cch++] = *lpSrc++;
		}

		if (cch <= cchMax)
		{
			cchDest		= cch;
			lpDest[cch] = L'\0';
			bSuccess	= TRUE;
		}
	}

	return bSuccess;
}

BOOL AFX_CDECL XTPLoadUnicodeText(const void* pRawData, UINT cbRawData, LPWSTR lpUnicodeText,
								  UINT* pchUnicodeText)
{
	ASSERT(NULL != pRawData);
	ASSERT(NULL != lpUnicodeText);
	ASSERT(NULL != pchUnicodeText);

	BOOL bSuccess = FALSE;

	if (NULL != pRawData && NULL != lpUnicodeText && NULL != pchUnicodeText)
	{
		LPCCH lpMbString = reinterpret_cast<LPCCH>(pRawData);
		const BYTE* pBOM = reinterpret_cast<const BYTE*>(pRawData);
		if (2 < cbRawData && 0xff == pBOM[0] && 0xfe == pBOM[1])
		{
			// Already in Unicode.
			lpMbString += 2;
			bSuccess = WcsCchCopyN(lpUnicodeText, *pchUnicodeText,
								   reinterpret_cast<LPCWSTR>(lpMbString),
								   (cbRawData - 2) / sizeof(WCHAR));
		}
		else if (3 < cbRawData && 0xef == pBOM[0] && 0xbb == pBOM[1] && 0xbf == pBOM[2])
		{
			// Convert from UTF-8
			lpMbString += 3;
			UINT cchRaw = StrNLen(lpMbString, cbRawData - 3);
			int cch		= MultiByteToWideChar(CP_UTF8, 0, lpMbString, XTPToIntChecked(cchRaw),
										  lpUnicodeText, XTPToIntChecked(*pchUnicodeText) - 1);
			if (0 < cch)
			{
				*pchUnicodeText	   = XTPToUInt(cch);
				lpUnicodeText[cch] = L'\0';
				bSuccess		   = TRUE;
			}
		}
		else if (0 < cbRawData
				 && IsTextUnicode(const_cast<void*>(pRawData), XTPToIntChecked(cbRawData), NULL))
		{
			// Seems like already in Unicode.
			bSuccess = WcsCchCopyN(lpUnicodeText, *pchUnicodeText,
								   reinterpret_cast<LPCWSTR>(lpMbString),
								   cbRawData / sizeof(WCHAR));
		}
		else if (0 < cbRawData)
		{
			// Try to convert as from ANSI.
			UINT cchRaw = StrNLen(lpMbString, cbRawData);
			int cch		= MultiByteToWideChar(CP_ACP, 0, lpMbString, XTPToIntChecked(cchRaw),
										  lpUnicodeText, XTPToIntChecked(*pchUnicodeText) - 1);
			if (0 < cch)
			{
				*pchUnicodeText	   = XTPToUInt(cch);
				lpUnicodeText[cch] = L'\0';
				bSuccess		   = TRUE;
			}
		}
		else if (0 < *pchUnicodeText)
		{
			*pchUnicodeText	 = 0;
			lpUnicodeText[0] = L'\0';
			bSuccess		 = TRUE;
		}
	}

	return bSuccess;
}

HRESULT AFX_CDECL XTPCreateReadOnlyMemoryStream(const void* pData, UINT cbData, BOOL bClone,
												IStream** ppStream)
{
	ASSERT(NULL != ppStream);
	*ppStream = NULL;

	if (!bClone)
	{
		static const XTP_SHLWAPI& api = GetShlwapi();
		if (NULL != api.pfnSHCreateMemStream)
		{
			*ppStream = api.pfnSHCreateMemStream(pData, cbData);
			if (NULL != *ppStream)
			{
				*ppStream = new CReadOnlyStreamWrapper(*ppStream);
				return S_OK;
			}

			return E_FAIL;
		}
	}

	HGLOBAL hGlob = ::GlobalAlloc(GMEM_FIXED, cbData);
	if (NULL == hGlob)
		return E_OUTOFMEMORY;

	LPVOID pBuff = ::GlobalLock(hGlob);
	if (NULL == pBuff)
	{
		::GlobalFree(hGlob);
		return E_FAIL;
	}

	::CopyMemory(pBuff, pData, cbData);
	::GlobalUnlock(pBuff);

	HRESULT hr = ::CreateStreamOnHGlobal(hGlob, TRUE, ppStream);
	if (SUCCEEDED(hr))
	{
		*ppStream = new CReadOnlyStreamWrapper(*ppStream);
	}

	return hr;
}

_XTP_EXT_CLASS HRESULT AFX_CDECL XTPCreateFileStream(LPCTSTR lpPath, DWORD grfMode,
													 IStream** ppStream)
{
	ASSERT(NULL != lpPath);
	ASSERT(NULL != ppStream);

	const XTP_SHLWAPI& api = GetShlwapi();

#ifdef _UNICODE
	XTP_SHLWAPI::PFNSHCreateStreamOnFileW pfnSHCreateStreamOnFile = api.pfnSHCreateStreamOnFileW;
#else
	XTP_SHLWAPI::PFNSHCreateStreamOnFileA pfnSHCreateStreamOnFile = api.pfnSHCreateStreamOnFileA;
#endif

	if (NULL == pfnSHCreateStreamOnFile)
		return E_NOTIMPL;

	HRESULT hr = pfnSHCreateStreamOnFile(lpPath, grfMode, ppStream);
	if (FAILED(hr))
		return hr;

	if (NULL == *ppStream)
		return E_FAIL;

	return hr;
}

HRESULT AFX_CDECL XTPCreateReadOnlyFileStream(LPCTSTR lpPath, IStream** ppStream)
{
	ASSERT(NULL != lpPath);
	ASSERT(NULL != ppStream);
	*ppStream = NULL;

	HRESULT hr = XTPCreateFileStream(lpPath, STGM_READ | STGM_SHARE_DENY_WRITE, ppStream);
	if (SUCCEEDED(hr))
	{
		*ppStream = new CReadOnlyStreamWrapper(*ppStream);
		return hr;
	}

	HANDLE hFile  = INVALID_HANDLE_VALUE;
	HGLOBAL hMem  = NULL;
	LPVOID lpData = NULL;

	do
	{
		hFile = ::CreateFile(lpPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
							 FILE_ATTRIBUTE_NORMAL, NULL);
		if (INVALID_HANDLE_VALUE == hFile)
		{
			hr = HRESULT_FROM_WIN32(GetLastError());
			break;
		}

		ULARGE_INTEGER fileSize;
		fileSize.LowPart = ::GetFileSize(hFile, &fileSize.HighPart);
		if (0 < fileSize.HighPart)
		{
			hr = E_OUTOFMEMORY;
			break;
		}

		if (0 == fileSize.LowPart)
		{
			hr = HRESULT_FROM_WIN32(ERROR_EMPTY);
			break;
		}

		hMem = ::GlobalAlloc(GMEM_FIXED, fileSize.LowPart);
		if (NULL == hMem)
		{
			hr = HRESULT_FROM_WIN32(GetLastError());
			break;
		}

		lpData = ::GlobalLock(hMem);
		if (NULL == lpData)
		{
			hr = HRESULT_FROM_WIN32(GetLastError());
			break;
		}

		DWORD cbIO = 0;
		if (!::ReadFile(hFile, lpData, fileSize.LowPart, &cbIO, NULL))
		{
			hr = HRESULT_FROM_WIN32(GetLastError());
			break;
		}

		::GlobalUnlock(lpData);
		lpData = NULL;

		hr = ::CreateStreamOnHGlobal(hMem, TRUE, ppStream);
		if (SUCCEEDED(hr))
		{
			hMem	  = NULL;
			*ppStream = new CReadOnlyStreamWrapper(*ppStream);
		}
	} while (FALSE);

	if (INVALID_HANDLE_VALUE != hFile)
	{
		::CloseHandle(hFile);
	}

	if (NULL != lpData)
	{
		::GlobalUnlock(lpData);
	}

	if (NULL != hMem)
	{
		::GlobalFree(hMem);
	}

	return hr;
}

HRESULT AFX_CDECL XTPCreateReadOnlyResourceStream(HMODULE hModule, LPCTSTR lpResource,
												  LPCTSTR lpResourceType, BOOL bClone,
												  IStream** ppStream)
{
	ASSERT(NULL != ppStream);
	*ppStream = NULL;

	HRSRC hRsrc = XTPResourceManager()->FindResource(hModule, lpResource, lpResourceType);
	if (NULL == hRsrc)
		return E_FAIL;

	HGLOBAL hGlobal = ::LoadResource(hModule, hRsrc);
	if (NULL == hGlobal)
		return E_FAIL;

	LPVOID pData = ::LockResource(hGlobal);
	if (NULL == pData)
		return E_FAIL;

	DWORD dwSize = ::SizeofResource(hModule, hRsrc);
	if (0 == dwSize)
	{
		::UnlockResource(hGlobal);
		return E_FAIL;
	}

	HRESULT hr = XTPCreateReadOnlyMemoryStream(pData, dwSize, bClone, ppStream);
	::UnlockResource(hGlobal);

	return hr;
}

BOOL AFX_CDECL XTPIsDerivedFromRemote(const CRuntimeClass* pBaseClass,
									  const CRuntimeClass* pClassThis)
{
	ASSERT(NULL != pClassThis);
	ASSERT(AfxIsValidAddress(pClassThis, sizeof(CRuntimeClass), FALSE));
	ASSERT(NULL != pBaseClass);
	ASSERT(AfxIsValidAddress(pBaseClass, sizeof(CRuntimeClass), FALSE));

#ifdef _AFXDLL
	for (;;)
#else
	while (pClassThis != NULL)
#endif
	{
		if (pClassThis->m_nObjectSize == pBaseClass->m_nObjectSize
			&& 0 == strcmp(pClassThis->m_lpszClassName, pBaseClass->m_lpszClassName))
		{
			return TRUE;
		}

#ifdef _AFXDLL
		if (pClassThis->m_pfnGetBaseClass == NULL)
			break;
		pClassThis = (*pClassThis->m_pfnGetBaseClass)();
#else
		pClassThis = pClassThis->m_pBaseClass;
#endif
	}

	return FALSE; // walked to the top, no match
}

BOOL AFX_CDECL XTPIsKindOfRemote(const CRuntimeClass* pClass, const CObject* pObject)
{
	ASSERT(NULL != pObject);

	// it better be in valid memory, at least for CObject size
	ASSERT(AfxIsValidAddress(pObject, sizeof(CObject)));

	// simple SI case
	CRuntimeClass* pClassThis = pObject->GetRuntimeClass();
	ASSERT(NULL != pClassThis);
	return XTPIsDerivedFromRemote(pClass, pClassThis);
}

CObject* AFX_CDECL XTPDynamicDownCastRemote(CRuntimeClass* pClass, CObject* pObject)
{
	return (pObject != NULL && XTPIsKindOfRemote(pClass, pObject) ? pObject : NULL);
}

///////////////////////////////////////////////////////////////////////////////
// CXTPNonClientMetrics

CXTPNonClientMetrics::CXTPNonClientMetrics()
{
	::ZeroMemory(this, sizeof(NONCLIENTMETRICS));
	cbSize = sizeof(NONCLIENTMETRICS);

#if (WINVER >= 0x0600)
	// The workaround for the major known issue with SPI_GETNONCLIENTMETRICS.
	OSVERSIONINFO osvi = { sizeof(OSVERSIONINFO), 0 };
	// C4996: 'GetVersionExA': was declared deprecated
#	pragma warning(push)
#	pragma warning(disable : 4996)
	VERIFY(::GetVersionEx(&osvi));
#	pragma warning(pop)
	if (osvi.dwMajorVersion < 6)
	{
		cbSize -= sizeof(int);
	}
#endif

	VERIFY(::SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), this, 0));
}

///////////////////////////////////////////////////////////////////////////////
// CXTPToolhelpApi

CXTPToolhelpApi& AFX_CDECL CXTPToolhelpApi::Instance()
{
	return CXTPSingleton<CXTPToolhelpApi>::Instance();
}

HANDLE CXTPToolhelpApi::CreateSnapshot(DWORD dwFlags, DWORD th32ProcessID)
{
	ASSERT(m_bInitialized);
	return m_bInitialized ? m_pfnCreateToolhelp32Snapshot(dwFlags, th32ProcessID) : NULL;
}

BOOL CXTPToolhelpApi::GetFirstThread(HANDLE hSnapshot, LPTHREADENTRY32 lpte)
{
	ASSERT(m_bInitialized);
	return m_bInitialized ? m_pfnThread32First(hSnapshot, lpte) : FALSE;
}

BOOL CXTPToolhelpApi::GetNextThread(HANDLE hSnapshot, LPTHREADENTRY32 lpte)
{
	ASSERT(m_bInitialized);
	return m_bInitialized ? m_pfnThread32Next(hSnapshot, lpte) : FALSE;
}

BOOL CXTPToolhelpApi::GetFirstModule(HANDLE hSnapshot, LPMODULEENTRY32 lpme)
{
	ASSERT(m_bInitialized);
	return m_bInitialized ? m_pfnModule32First(hSnapshot, lpme) : FALSE;
}

BOOL CXTPToolhelpApi::GetNextModule(HANDLE hSnapshot, LPMODULEENTRY32 lpme)
{
	ASSERT(m_bInitialized);
	return m_bInitialized ? m_pfnModule32Next(hSnapshot, lpme) : FALSE;
}

CXTPToolhelpApi::CXTPToolhelpApi()
	: m_bInitialized(FALSE)
	, m_pfnCreateToolhelp32Snapshot(NULL)
	, m_pfnThread32First(NULL)
	, m_pfnThread32Next(NULL)
	, m_pfnModule32First(NULL)
	, m_pfnModule32Next(NULL)
{
	HMODULE hKernel32 = ::LoadLibrary(_T("KERNEL32"));
	if (NULL != hKernel32)
	{
		m_pfnCreateToolhelp32Snapshot = XTPToFunctionPtr<PFNCREATETOOLHELP32SNAPSHOT>(
			::GetProcAddress(hKernel32, "CreateToolhelp32Snapshot"));
		m_pfnThread32First = XTPToFunctionPtr<PFNTHREAD32FIRSTNEXT>(
			::GetProcAddress(hKernel32, "Thread32First"));
		m_pfnThread32Next = XTPToFunctionPtr<PFNTHREAD32FIRSTNEXT>(
			::GetProcAddress(hKernel32, "Thread32Next"));
		m_pfnModule32First = XTPToFunctionPtr<PFNMODULE32FIRSTNEXT>(::GetProcAddress(hKernel32,
#ifndef _UNICODE
																					 "Module32First"
#else
																					 "Module32First"
																					 "W"
#endif
																					 ));
		m_pfnModule32Next = XTPToFunctionPtr<PFNMODULE32FIRSTNEXT>(::GetProcAddress(hKernel32,
#ifndef _UNICODE
																					"Module32Next"
#else
																					"Module32NextW"
#endif
																					));

		m_bInitialized = (NULL != m_pfnCreateToolhelp32Snapshot && NULL != m_pfnThread32First
						  && NULL != m_pfnThread32Next && NULL != m_pfnModule32First
						  && NULL != m_pfnModule32Next);

		::FreeLibrary(hKernel32);
	}
}

CXTPToolhelpApi::~CXTPToolhelpApi()
{
}

//===========================================================================
// CXTPGlobalAtom class
//===========================================================================

CXTPGlobalAtom::CXTPGlobalAtom(LPCTSTR lpAtomName)
	: m_strName(lpAtomName)
{
	ASSERT(NULL != lpAtomName);
	m_atom = ::GlobalAddAtom(lpAtomName);
	VERIFY(0 != m_atom);
}

CXTPGlobalAtom::~CXTPGlobalAtom()
{
	if (0 != m_atom)
	{
		::GlobalDeleteAtom(m_atom);
	}
}
