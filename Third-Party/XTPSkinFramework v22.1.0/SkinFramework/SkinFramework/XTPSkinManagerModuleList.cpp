// XTPSkinManagerModuleList.cpp: implementation of the CXTPSkinManagerModuleList class.
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

#include "Common/XTPFramework.h"
#include "Common/XTPVC80Helpers.h"
#include "Common/XTPSystemHelpers.h"
#include "Common/XTPSynchro.h"
#include "Common/XTPApplication.h"
#include "Common/XTPSingleton.h"
#include "Common/XTPGdiObjects.h"
#include "Common/XTPWinThemeWrapper.h"

#include "SkinFramework/XTPSkinManager.h"
#include "SkinFramework/XTPSkinManagerModuleList.h"
#include "SkinFramework/XTPSkinObject.h"
#include "SkinFramework/XTPSkinManagerApiHook.h"

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

#ifdef _DEBUG
#	undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#	define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////////
// CModuleEnumerator

class CXTPSkinManagerModuleList::CModuleEnumerator
{
public:
	CModuleEnumerator(DWORD dwProcessId, BOOL bUseCache);
	virtual ~CModuleEnumerator();

	void PreLoad(BOOL bThreadSafe);

	HMODULE GetFirstModule();
	HMODULE GetNextModule();
	virtual BOOL GetModuleInformation(HMODULE hModule, XTP_MODULEINFO* lpModInfo);

protected:
	void Refresh();
	void Reset(BOOL bIncludingPreLoaded = FALSE);
	virtual void CollectModules(CArray<XTP_MODULEINFO, const XTP_MODULEINFO&>& modules) = 0;

	void LockReferences(BOOL bLock = TRUE);

	DWORD GetProcessId() const;

private:
	CArray<XTP_MODULEINFO, const XTP_MODULEINFO&> m_arModules;
	INT_PTR m_nIndex;
	DWORD m_dwProcessId;
	BOOL m_bUseCache;
	BOOL m_bPreLoaded;
	int m_nReferenceLockCounter;

	typedef HMODULE(WINAPI* PFNLoadLibraryEx)(LPCTSTR, HANDLE, DWORD);
	PFNLoadLibraryEx m_pfnLoadLibraryExOrig;

	typedef BOOL(WINAPI* PFNFreeLibrary)(HMODULE);
	PFNFreeLibrary m_pfnFreeLibraryOrig;
};

CXTPSkinManagerModuleList::CModuleEnumerator::CModuleEnumerator(DWORD dwProcessId, BOOL bUseCache)
	: m_nIndex(0)
	, m_dwProcessId(dwProcessId)
	, m_bUseCache(bUseCache)
	, m_bPreLoaded(FALSE)
	, m_nReferenceLockCounter(0)
{
	XTP_SKINFRAMEWORK_MANAGE_STATE();

#ifndef _XTP_ACTIVEX
#	ifndef XTP_SKINFRAMEWORK_USE_DETOURS_API_HOOK

	// Obtain unhooked pointers to LoadLibraryEx and FreeLibrary functions.
	CXTPSkinManagerApiHookBase* pApiHookBase = XTPSkinManager()->GetApiHook();
	ASSERT(NULL != pApiHookBase);
	ASSERT_KINDOF(CXTPSkinManagerLegacyApiHook, pApiHookBase);
	CXTPSkinManagerLegacyApiHook* pApiHook = static_cast<CXTPSkinManagerLegacyApiHook*>(
		pApiHookBase);

	CXTPSkinManagerLegacyApiFunction* pFunction = pApiHook->GetHookedFunction(
#		ifdef _UNICODE
		xtpSkinLegacyApiLoadLibraryExW_KERNEL32
#		else
		xtpSkinLegacyApiLoadLibraryExA_KERNEL32
#		endif
	);

	m_pfnLoadLibraryExOrig = (NULL != pFunction && pFunction->IsHooked()
								  ? XTPToFunctionPtr<PFNLoadLibraryEx>(pFunction->GetOriginal())
								  : &::LoadLibraryEx);
	ASSERT(NULL != m_pfnLoadLibraryExOrig);

	pFunction			 = pApiHook->GetHookedFunction(xtpSkinLegacyApiFreeLibrary_KERNEL32);
	m_pfnFreeLibraryOrig = (NULL != pFunction && pFunction->IsHooked()
								? XTPToFunctionPtr<PFNFreeLibrary>(pFunction->GetOriginal())
								: &::FreeLibrary);
	ASSERT(NULL != m_pfnFreeLibraryOrig);

#	else // XTP_SKINFRAMEWORK_USE_DETOURS_API_HOOK

	m_pfnLoadLibraryExOrig = ::LoadLibraryEx;
	m_pfnFreeLibraryOrig   = ::FreeLibrary;

#	endif // !XTP_SKINFRAMEWORK_USE_DETOURS_API_HOOK

#else // _XTP_ACTIVEX

	CXTPSkinManagerApiHookBase* pApiHookBase = XTPSkinManager()->GetApiHook();
	ASSERT(NULL != pApiHookBase);

	if (pApiHookBase->IsLegacyImplementation())
	{
		ASSERT_KINDOF(CXTPSkinManagerLegacyApiHook, pApiHookBase);
		CXTPSkinManagerLegacyApiHook* pApiHook = static_cast<CXTPSkinManagerLegacyApiHook*>(
			pApiHookBase);

		CXTPSkinManagerLegacyApiFunction* pFunction = pApiHook->GetHookedFunction(
#	ifdef _UNICODE
			xtpSkinLegacyApiLoadLibraryExW_KERNEL32
#	else
			xtpSkinLegacyApiLoadLibraryExA_KERNEL32
#	endif
		);

		m_pfnLoadLibraryExOrig = (NULL != pFunction && pFunction->IsHooked()
									  ? reinterpret_cast<PFNLoadLibraryEx>(pFunction->GetOriginal())
									  : &::LoadLibraryEx);
		ASSERT(NULL != m_pfnLoadLibraryExOrig);

		pFunction			 = pApiHook->GetHookedFunction(xtpSkinLegacyApiFreeLibrary_KERNEL32);
		m_pfnFreeLibraryOrig = (NULL != pFunction && pFunction->IsHooked()
									? reinterpret_cast<PFNFreeLibrary>(pFunction->GetOriginal())
									: &::FreeLibrary);
		ASSERT(NULL != m_pfnFreeLibraryOrig);
	}
	else
	{
		m_pfnLoadLibraryExOrig = ::LoadLibraryEx;
		m_pfnFreeLibraryOrig   = ::FreeLibrary;
	}

#endif // !_XTP_ACTIVEX
}

CXTPSkinManagerModuleList::CModuleEnumerator::~CModuleEnumerator()
{
	Reset(TRUE);
}

void CXTPSkinManagerModuleList::CModuleEnumerator::PreLoad(BOOL bThreadSafe)
{
	if (bThreadSafe)
	{
		Refresh();
		LockReferences();
	}
	else
	{
		Refresh();
	}

	m_bPreLoaded = TRUE;
}

HMODULE CXTPSkinManagerModuleList::CModuleEnumerator::GetFirstModule()
{
	if (!m_bPreLoaded)
	{
		Refresh();
	}

	return GetNextModule();
}

static BOOL IsValidModule(HMODULE hModule)
{
	BOOL bValid				  = FALSE;
	PBYTE pModuleStart		  = reinterpret_cast<PBYTE>(hModule);
	PIMAGE_DOS_HEADER pDosHdr = reinterpret_cast<PIMAGE_DOS_HEADER>(pModuleStart);

	if (::AfxIsValidAddress(pDosHdr, sizeof(*pDosHdr), FALSE)
		&& pDosHdr->e_magic == IMAGE_DOS_SIGNATURE)
	{
		PIMAGE_NT_HEADERS pNtHdr = reinterpret_cast<PIMAGE_NT_HEADERS>(pModuleStart
																	   + pDosHdr->e_lfanew);
		if (::AfxIsValidAddress(pNtHdr, sizeof(*pNtHdr), FALSE)
			&& pNtHdr->Signature == IMAGE_NT_SIGNATURE)
		{
			bValid = TRUE;
		}
	}

	return bValid;
}

HMODULE CXTPSkinManagerModuleList::CModuleEnumerator::GetNextModule()
{
	HMODULE hModule = NULL;

	while (m_nIndex < m_arModules.GetSize())
	{
		XTP_MODULEINFO& info = m_arModules[m_nIndex++];
		if (!info.bUnacceptable && IsValidModule(info.hModule))
		{
			hModule = info.hModule;
			break;
		}
		else
		{
			hModule = NULL;
		}
	}

	if (NULL == hModule)
	{
		Reset(FALSE);
	}

	return hModule;
}

BOOL CXTPSkinManagerModuleList::CModuleEnumerator::GetModuleInformation(HMODULE hModule,
																		XTP_MODULEINFO* lpModInfo)
{
	ASSERT(NULL != lpModInfo);

	BOOL bFound = FALSE;

	if (IsValidModule(hModule))
	{
		CArray<XTP_MODULEINFO, const XTP_MODULEINFO&> modules;
		CArray<XTP_MODULEINFO, const XTP_MODULEINFO&>* pModules;

		if (m_bUseCache)
		{
			if (0 == m_arModules.GetSize())
			{
				Refresh();
			}

			pModules = &m_arModules;
		}
		else
		{
			CollectModules(modules);
			pModules = &modules;
		}

		for (INT_PTR i = 0; i < pModules->GetSize(); ++i)
		{
			XTP_MODULEINFO& info = (*pModules)[i];
			if (info.hModule == hModule)
			{
				*lpModInfo = info;
				bFound	 = TRUE;
				break;
			}
		}
	}

	return bFound;
}

void CXTPSkinManagerModuleList::CModuleEnumerator::Refresh()
{
	Reset(FALSE);

	if (!m_bUseCache || 0 == m_arModules.GetSize())
	{
		CollectModules(m_arModules);
	}
}

void CXTPSkinManagerModuleList::CModuleEnumerator::Reset(BOOL bIncludingPreLoaded /*= FALSE*/)
{
	if (bIncludingPreLoaded)
	{
		while (0 < m_nReferenceLockCounter)
		{
			LockReferences(FALSE);
		}

		m_bPreLoaded = FALSE;
	}

	if (!m_bUseCache)
	{
		m_arModules.RemoveAll();
	}

	m_nIndex = 0;
}

void CXTPSkinManagerModuleList::CModuleEnumerator::LockReferences(BOOL bLock /*= TRUE*/)
{
	if (bLock)
	{
		++m_nReferenceLockCounter;
	}
	else
	{
		ASSERT(0 < m_nReferenceLockCounter);
		--m_nReferenceLockCounter;
	}

	for (INT_PTR i = 0; i < m_arModules.GetSize(); ++i)
	{
		XTP_MODULEINFO& info = m_arModules[i];
		if (NULL != info.hModule && !info.bUnacceptable)
		{
			if (bLock)
			{
				info.bUnacceptable = TRUE;
				if (XTP_LDR_IS_RESOURCE(info.hModule))
				{
					continue;
				}

				TCHAR szPath[MAX_PATH * 4];
				if (0 == ::GetModuleFileName(info.hModule, szPath, _countof(szPath)))
				{
					continue;
				}

				HMODULE hModule = m_pfnLoadLibraryExOrig(szPath, NULL, LOAD_LIBRARY_AS_DATAFILE);
				if (NULL == hModule)
				{
					continue;
				}

				if (info.hModule != hModule || !IsValidModule(info.hModule))
				{
					m_pfnFreeLibraryOrig(hModule);
					continue;
				}

				info.bUnacceptable = FALSE;
				if (XTPSkinManager()->IsPersistentModule(info.hModule))
				{
					m_pfnFreeLibraryOrig(hModule);
					continue;
				}

				++info.nLockCount;
			}
			else if (0 < info.nLockCount)
			{
				if (m_pfnFreeLibraryOrig(info.hModule))
				{
					--info.nLockCount;
				}
			}
		}
	}
}

AFX_INLINE DWORD CXTPSkinManagerModuleList::CModuleEnumerator::GetProcessId() const
{
	return m_dwProcessId;
}

//////////////////////////////////////////////////////////////////////////
// CPsapiModuleEnumerator

class CXTPSkinManagerModuleList::CPsapiModuleEnumerator : public CModuleEnumerator
{
	typedef BOOL(WINAPI* PFNENUMPROCESSMODULES)(HANDLE hProcess, HMODULE* lphModule, DWORD cb,
												LPDWORD lpcbNeeded);
	typedef BOOL(WINAPI* PFNGETMODULEINFORMATION)(HANDLE hProcess, HMODULE hModule,
												  XTP_MODULEINFO* lpModInfo, DWORD cb);

public:
	CPsapiModuleEnumerator(DWORD dwProcessId, BOOL bUseCache, HMODULE hModPSAPI);

	virtual BOOL GetModuleInformation(HMODULE hModule, XTP_MODULEINFO* lpModInfo);

protected:
	virtual void CollectModules(CArray<XTP_MODULEINFO, const XTP_MODULEINFO&>& modules);

	PFNENUMPROCESSMODULES m_pfnEnumProcessModules;
	PFNGETMODULEINFORMATION m_pfnGetModuleInformation;
	HMODULE m_hModPSAPI;
};

CXTPSkinManagerModuleList::CPsapiModuleEnumerator::CPsapiModuleEnumerator(DWORD dwProcessId,
																		  BOOL bUseCache,
																		  HMODULE hModPSAPI)
	: CModuleEnumerator(dwProcessId, bUseCache)
	, m_hModPSAPI(hModPSAPI)
{
	ASSERT(NULL != m_hModPSAPI);

	if (NULL != m_hModPSAPI)
	{
		m_pfnEnumProcessModules = XTPToFunctionPtr<PFNENUMPROCESSMODULES>(
			::GetProcAddress(m_hModPSAPI, "EnumProcessModules"));
		m_pfnGetModuleInformation = XTPToFunctionPtr<PFNGETMODULEINFORMATION>(
			::GetProcAddress(m_hModPSAPI, "GetModuleInformation"));
	}
	else
	{
		m_pfnEnumProcessModules   = NULL;
		m_pfnGetModuleInformation = NULL;
	}
}

void CXTPSkinManagerModuleList::CPsapiModuleEnumerator::CollectModules(
	CArray<XTP_MODULEINFO, const XTP_MODULEINFO&>& modules)
{
	modules.RemoveAll();

	if (NULL != m_pfnEnumProcessModules)
	{
		HANDLE hProcess = ::OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE,
										GetProcessId());
		if (NULL != hProcess)
		{
			DWORD cbNeeded = 0;
			HMODULE hModule;

			if (m_pfnEnumProcessModules(hProcess, &hModule, sizeof(hModule), &cbNeeded)
				&& sizeof(HMODULE) <= cbNeeded)
			{
				int nCount = XTPToIntChecked(cbNeeded / sizeof(HMODULE));
				ASSERT(1 <= nCount);

				CArray<HMODULE, HMODULE> handles;
				handles.SetSize(nCount);

				if (m_pfnEnumProcessModules(hProcess, &handles[0], cbNeeded, &cbNeeded))
				{
					for (INT_PTR i = 0; i < handles.GetSize(); ++i)
					{
						XTP_MODULEINFO info = { 0 };
						if (m_pfnGetModuleInformation(hProcess, handles[i], &info,
													  XTP_WIN32SDK_MODULEINFO_SIZE))
						{
							info.hModule = handles[i];
							modules.Add(info);
						}
					}
				}
			}

			::CloseHandle(hProcess);
		}
	}
}

BOOL CXTPSkinManagerModuleList::CPsapiModuleEnumerator::GetModuleInformation(
	HMODULE hModule, XTP_MODULEINFO* lpModInfo)
{
	ASSERT(NULL != lpModInfo);

	BOOL bResult = FALSE;

	if (NULL != m_pfnGetModuleInformation && NULL != hModule && NULL != lpModInfo)
	{
		HANDLE hProcess = ::OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE,
										this->GetProcessId());
		if (NULL != hProcess)
		{
			bResult = m_pfnGetModuleInformation(hProcess, hModule, lpModInfo,
												XTP_WIN32SDK_MODULEINFO_SIZE);
			if (bResult)
			{
				lpModInfo->hModule		 = hModule;
				lpModInfo->nLockCount	= 0;
				lpModInfo->bUnacceptable = FALSE;
			}

			::CloseHandle(hProcess);
		}
	}

	return bResult;
}

//////////////////////////////////////////////////////////////////////////
// ToolHelp

class CXTPSkinManagerModuleList::CToolHelpModuleEnumerator : public CModuleEnumerator
{
public:
	CToolHelpModuleEnumerator(DWORD dwProcessId, BOOL bUseCache);

protected:
	virtual void CollectModules(CArray<XTP_MODULEINFO, const XTP_MODULEINFO&>& modules);
};

CXTPSkinManagerModuleList::CToolHelpModuleEnumerator::CToolHelpModuleEnumerator(DWORD dwProcessId,
																				BOOL bUseCache)
	: CModuleEnumerator(dwProcessId, bUseCache)
{
}

void CXTPSkinManagerModuleList::CToolHelpModuleEnumerator::CollectModules(
	CArray<XTP_MODULEINFO, const XTP_MODULEINFO&>& modules)
{
	modules.RemoveAll();

	CXTPToolhelpApi& toolhelpApi = CXTPToolhelpApi::Instance();
	if (toolhelpApi.IsInitialized())
	{
		HANDLE hSnapshot = toolhelpApi.CreateSnapshot(TH32CS_SNAPMODULE, GetProcessId());
		if (INVALID_HANDLE_VALUE != hSnapshot)
		{
			MODULEENTRY32 me;
			me.dwSize = sizeof(MODULEENTRY32);
			if (toolhelpApi.GetFirstModule(hSnapshot, &me))
			{
				do
				{
					if (NULL != me.hModule)
					{
						XTP_MODULEINFO info = { 0 };
						info.lpBaseOfDll	= me.modBaseAddr;
						info.SizeOfImage	= me.modBaseSize;
						info.hModule		= me.hModule;
						modules.Add(info);
					}
				} while (toolhelpApi.GetNextModule(hSnapshot, &me));
			}

			::CloseHandle(hSnapshot);
		}
	}
}

//////////////////////////////////////////////////////////////////////
// CXTPSkinManagerModuleList

CXTPSkinManagerModuleListSharedData::CXTPSkinManagerModuleListSharedData()
	: m_bExists(FALSE)
	, m_bInitialized(FALSE)
{
}

CXTPSkinManagerModuleListSharedData::~CXTPSkinManagerModuleListSharedData()
{
}

const CXTPModuleHandle& CXTPSkinManagerModuleListSharedData::GetPsapiDllHandle() const
{
	const_cast<CXTPSkinManagerModuleListSharedData*>(this)->Init();
	return m_hPsapiDll;
}

BOOL CXTPSkinManagerModuleListSharedData::Exists() const
{
	const_cast<CXTPSkinManagerModuleListSharedData*>(this)->Init();
	return m_bExists;
}

void CXTPSkinManagerModuleListSharedData::Init()
{
	if (!m_bInitialized)
	{
		m_bInitialized = TRUE;

		m_hPsapiDll.LoadLibrary(_T("PSAPI.DLL"));

		m_bExists = m_hPsapiDll != NULL;

		if (!m_bExists)
		{
			m_bExists = CXTPToolhelpApi::Instance().IsInitialized();
		}
	}
}

CXTPSkinManagerModuleListSharedData& AFX_CDECL CXTPSkinManagerModuleList::GetSharedData()
{
	return CXTPSingleton<CXTPSkinManagerModuleListSharedData>::Instance();
}

BOOL AFX_CDECL CXTPSkinManagerModuleList::DoesEnumeratorExist()
{
	return GetSharedData().Exists();
}

CXTPSkinManagerModuleList::CXTPSkinManagerModuleList(DWORD dwProcessId, BOOL bUseCache /*= FALSE*/)
{
	const CXTPModuleHandle& hPsapiDll = GetSharedData().GetPsapiDllHandle();
	if (hPsapiDll)
	{
		m_pEnumerator = new CPsapiModuleEnumerator(dwProcessId, bUseCache, hPsapiDll);
	}
	else if (GetSharedData().Exists())
	{
		m_pEnumerator = new CToolHelpModuleEnumerator(dwProcessId, bUseCache);
	}
	else
	{
		m_pEnumerator = NULL;
		ASSERT(!"Shared data is in unexpected state");
	}
}

CXTPSkinManagerModuleList::~CXTPSkinManagerModuleList()
{
	SAFE_DELETE(m_pEnumerator);
}

void CXTPSkinManagerModuleList::PreLoad(BOOL bThreadSafe)
{
	if (NULL != m_pEnumerator)
	{
		m_pEnumerator->PreLoad(bThreadSafe);
	}
}

HMODULE CXTPSkinManagerModuleList::GetFirstModule()
{
	return NULL != m_pEnumerator ? m_pEnumerator->GetFirstModule() : NULL;
}

HMODULE CXTPSkinManagerModuleList::GetNextModule()
{
	return NULL != m_pEnumerator ? m_pEnumerator->GetNextModule() : NULL;
}

BOOL CXTPSkinManagerModuleList::GetModuleInformation(HMODULE hModule, XTP_MODULEINFO* lpModInfo)
{
	return NULL != m_pEnumerator ? m_pEnumerator->GetModuleInformation(hModule, lpModInfo) : NULL;
}
