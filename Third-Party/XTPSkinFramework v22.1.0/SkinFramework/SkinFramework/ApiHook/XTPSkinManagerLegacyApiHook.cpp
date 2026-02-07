// XTPSkinManagerLegacyApiHook.cpp: Legacy implementation of the CXTPSkinManagerLegacyApiHook class.
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

#ifdef XTP_SKINFRAMEWORK_USE_LEGACY_API_HOOK

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

#	include "Common/Base/Diagnostic/XTPEnableAdvancedWarnings.h"

#	if _MSC_VER >= 1900
#		pragma warning(pop)
#	endif

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
#	include "Common/XTPSynchro.h"

#	include "SkinFramework/XTPSkinManager.h"
#	include "SkinFramework/XTPSkinManagerSchema.h"
#	include "SkinFramework/XTPSkinObject.h"
#	include "SkinFramework/XTPSkinObjectFrame.h"
#	include "SkinFramework/XTPSkinObjectMenu.h"
#	include "SkinFramework/XTPSkinDrawTools.h"
#	include "SkinFramework/XTPSkinManagerModuleList.h"
#	include "SkinFramework/XTPSkinManagerResource.h"
#	include "SkinFramework/ApiHook/XTPSkinManagerApiHookBase.h"
#	include "SkinFramework/ApiHook/XTPSkinManagerLegacyApiHook.h"

#	include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

#	ifdef _DEBUG
#		define new DEBUG_NEW
#		undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#	endif

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

static int XTPCompareStringNoCase(const TCHAR* dst, const TCHAR* src, int len)
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

		len--;
	} while (f && (f == l) && len != 0);

	return (f - l);
}

//
// The PUSH opcode on x86 platforms
//
const BYTE cPushOpCode = 0x68;

PVOID CXTPSkinManagerLegacyApiFunction::sm_pvMaxAppAddr = NULL;

//////////////////////////////////////////////////////////////////////////
// CXTPSkinManagerLegacyApiFunction

CXTPSkinManagerLegacyApiFunction::CXTPSkinManagerLegacyApiFunction(
	CXTPSkinManagerLegacyApiHook* pApiHook, LPCSTR pszCalleeModName, LPCSTR pszFuncName,
	PROC pfnOrig, PROC pfnHook, PROC pfnAltOrig /*= NULL*/)
	: m_pApiHook(pApiHook)
	, m_bHooked(FALSE)
	, m_pfnOrig(pfnOrig)
	, m_pfnHook(pfnHook)
	, m_pfnAltOrig(pfnAltOrig)
{
#	if (_MSC_VER > 1310) // VS2005
	strcpy_s(m_szCalleeModName, _countof(m_szCalleeModName), pszCalleeModName);
	strcpy_s(m_szFuncName, _countof(m_szFuncName), pszFuncName);
#	else
	strcpy(m_szCalleeModName, pszCalleeModName);
	strcpy(m_szFuncName, pszFuncName);
#	endif

	if (sm_pvMaxAppAddr == NULL)
	{
		//
		// Functions with address above lpMaximumApplicationAddress require
		// special processing (Windows 9x only)
		//
		SYSTEM_INFO si;
		GetSystemInfo(&si);
		sm_pvMaxAppAddr = si.lpMaximumApplicationAddress;
	}

	if (m_pfnOrig > sm_pvMaxAppAddr)
	{
		//
		// The address is in a shared DLL; the address needs fixing up
		//
		PBYTE pb = (PBYTE)m_pfnOrig;
		if (pb[0] == cPushOpCode)
		{
			//
			// Skip over the PUSH op code and grab the real address
			//
			PVOID pv  = *(PVOID*)&pb[1];
			m_pfnOrig = (PROC)pv;
		}
	}
}

CXTPSkinManagerLegacyApiFunction::~CXTPSkinManagerLegacyApiFunction()
{
	ASSERT("The hook must be released prior to function destruction" && !m_bHooked);
}

BOOL CXTPSkinManagerLegacyApiFunction::HookImport(CXTPSkinManagerModuleList* pModuleList)
{
	XTP_GUARD_SHARED(CXTPSkinManagerLegacyApiFunction, this)
	{
		m_bHooked = ReplaceInAllModules(m_szCalleeModName, m_pfnOrig, m_pfnHook, pModuleList,
										m_pfnAltOrig);
		return m_bHooked;
	}
}

BOOL CXTPSkinManagerLegacyApiFunction::UnhookImport(CXTPSkinManagerModuleList* pModuleList)
{
	XTP_GUARD_SHARED(CXTPSkinManagerLegacyApiFunction, this)
	{
		if (m_bHooked)
		{
			ReplaceInAllModules(m_szCalleeModName, m_pfnHook, m_pfnOrig, pModuleList, m_pfnAltOrig);
			m_bHooked = FALSE;
		}

		return !m_bHooked;
	}
}

BOOL CXTPSkinManagerLegacyApiFunction::UnhookImport(HMODULE hmod)
{
	XTP_GUARD_SHARED(CXTPSkinManagerLegacyApiFunction, this)
	{
		if (m_bHooked)
		{
			ReplaceInOneModule(m_szCalleeModName, m_pfnHook, m_pfnOrig, hmod, m_pfnAltOrig);
		}

		return !m_bHooked;
	}
}

BOOL CXTPSkinManagerLegacyApiFunction::ReplaceInAllModules(LPCSTR pszCalleeModName, PROC pfnCurrent,
														   PROC pfnNew,
														   CXTPSkinManagerModuleList* pModuleList,
														   PROC pfnAltCurrent /*= NULL*/)
{
	ASSERT(NULL != pModuleList);

	BOOL bResult = FALSE;

	if ((NULL != pfnCurrent) && (NULL != pfnNew))
	{
		HINSTANCE hInstance = XTPGetInstanceHandle();

		HMODULE hModule = pModuleList->GetFirstModule();
		while (NULL != hModule)
		{
			if ((hModule != hInstance) && !XTPAccessShared(m_pApiHook)->IsModuleExcluded(hModule))
			{
				if (ReplaceInOneModule(pszCalleeModName, pfnCurrent, pfnNew, hModule,
									   pfnAltCurrent))
				{
					bResult = TRUE;
				}
			}

			hModule = pModuleList->GetNextModule();
		}

		if (ReplaceInOneModule(pszCalleeModName, pfnCurrent, pfnNew, hInstance, pfnAltCurrent))
		{
			bResult = TRUE;
		}
	}

	return bResult;
}

BOOL CXTPSkinManagerLegacyApiFunction::ReplaceInOneModule(LPCSTR pszCalleeModName, PROC pfnCurrent,
														  PROC pfnNew, HMODULE hmodCaller,
														  PROC pfnAltCurrent /*= NULL*/)
{
	BOOL bAtLeastOneReplaced = FALSE;

	if (pfnCurrent != NULL && pfnNew != NULL)
	{
		try
		{
			// Get the address of the module's import section
			ULONG ulSize;
			PIMAGE_IMPORT_DESCRIPTOR pImportDesc = (PIMAGE_IMPORT_DESCRIPTOR)
				ImageDirectoryEntryToData(hmodCaller, TRUE, IMAGE_DIRECTORY_ENTRY_IMPORT, &ulSize);

			// Does this module has import section ?
			while (NULL != pImportDesc && 0 != pImportDesc->Name)
			{
				// Loop through all descriptors and
				// find the import descriptor containing references to callee's functions
				while (0 != pImportDesc->Name)
				{
					LPCSTR lpszName = ((LPCSTR)((PBYTE)hmodCaller + pImportDesc->Name));
					if (XTPCompareStringNoCase(lpszName, pszCalleeModName) == 0)
						break; // Found
					pImportDesc++;
				}

				// Does this module import any functions from this callee ?
				if (0 == pImportDesc->Name)
					break;

				PIMAGE_THUNK_DATA pThunk = (PIMAGE_THUNK_DATA)((PBYTE)hmodCaller
															   + (UINT_PTR)pImportDesc->FirstThunk);

				while (pThunk->u1.Function)
				{
					PROC* ppfn	 = (PROC*)&pThunk->u1.Function;
					BOOL bFound	= (*ppfn == pfnCurrent);
					BOOL bReplaced = (*ppfn == pfnNew);

					if (!bReplaced && !bFound && (*ppfn > sm_pvMaxAppAddr))
					{
						PBYTE pbInFunc = (PBYTE)*ppfn;

						// Is this a wrapper (debug thunk) represented by PUSH instruction?
						if (pbInFunc[0] == cPushOpCode)
						{
							ppfn	  = (PROC*)&pbInFunc[1];
							bFound	= (*ppfn == pfnCurrent);
							bReplaced = (*ppfn == pfnNew);
						}
					}

					if (!bReplaced && !bFound && NULL != pfnAltCurrent)
					{
						bFound = (*ppfn == pfnAltCurrent);
						if (!bFound && (*ppfn > sm_pvMaxAppAddr))
						{
							PBYTE pbInFunc = (PBYTE)*ppfn;

							// Is this a wrapper (debug thunk) represented by PUSH instruction?
							if (pbInFunc[0] == cPushOpCode)
							{
								ppfn	  = (PROC*)&pbInFunc[1];
								bFound	= (*ppfn == pfnAltCurrent);
								bReplaced = (*ppfn == pfnNew);
							}
						}
					}

					if (bFound)
					{
						// Some useful info can be found on
						// http://www.gunsmoker.ru/2012/09/task-15-answer.html.
						// In order to provide writable access to this part of the
						// memory we need to change the memory protection

						MEMORY_BASIC_INFORMATION mbi;
						::VirtualQuery(ppfn, &mbi, sizeof(MEMORY_BASIC_INFORMATION));
						if (::VirtualProtect(mbi.BaseAddress, mbi.RegionSize,
											 PAGE_EXECUTE_READWRITE, &mbi.Protect))
						{
							// Hook the function.
							*ppfn = *pfnNew;

							// Restore the protection back
							DWORD dwOldProtect;
							::VirtualProtect(mbi.BaseAddress, mbi.RegionSize, mbi.Protect,
											 &dwOldProtect);

							::FlushInstructionCache(GetCurrentProcess(), mbi.BaseAddress,
													mbi.RegionSize);

							bAtLeastOneReplaced = TRUE;
						}
					}
					else if (bReplaced)
					{
						bAtLeastOneReplaced = TRUE;
					}

					pThunk++;
				}

				pImportDesc++;
			}
		}
		catch (...)
		{
			// Do nothing, as not much can be done.
		}
	}

	return bAtLeastOneReplaced;
}

/////////////////////////////////////////////////////////////////////////////
// CXTPSkinManagerLegacyApiHook

IMPLEMENT_DYNAMIC(CXTPSkinManagerLegacyApiHook, CXTPSkinManagerApiHookBase)

BOOL CXTPSkinManagerLegacyApiHook::m_bInitialized			   = FALSE;
BOOL CXTPSkinManagerLegacyApiHook::m_bInitializationInProgress = FALSE;

CXTPSkinManagerLegacyApiHook::CXTPSkinManagerLegacyApiHook()
	: CXTPSkinManagerApiHookBase(TRUE)
	, m_nLockCondition(0)
{
	ZeroMemory(&m_arrFunctions, sizeof(m_arrFunctions));

	m_bInAppCompatMode = (XTPSystemVersion()->IsWin7OrGreater()
						  && NULL != GetModuleHandle(_T("APPHELP.DLL")));

	ExcludeModule(_T("SHLWAPI.DLL"), TRUE);
	ExcludeModule(_T("COMCTL32.DLL"), TRUE);
	ExcludeModule(_T("KERNEL32.DLL"), FALSE);
	ExcludeModule(_T("USER32.DLL"), FALSE);
	ExcludeModule(_T("GDI32.DLL"), FALSE);
	ExcludeModule(_T("WININET.DLL"), FALSE);
	ExcludeModule(_T("MSCTF.DLL"), FALSE);
	ExcludeModule(_T("ACLAYERS.DLL"), FALSE);
}

CXTPSkinManagerLegacyApiHook::~CXTPSkinManagerLegacyApiHook()
{
	FinalizeHookManagement();
	UnhookAllFunctions(TRUE);
}

const CXTPSkinManagerLegacyApiHook::APIHOOKINFO CXTPSkinManagerLegacyApiHook::m_apiHookInfos[] = {
#	define XTPSFHOOKINFOEX(module, name, suffix, options, version)                                \
		{                                                                                          \
			xtpSkinLegacyApi##name##_##suffix, module, #name,                                      \
				reinterpret_cast<PROC>(&CXTPSkinManagerLegacyApiHook::OnHook##name##_##suffix),    \
				options, version                                                                   \
		}
#	define XTPSFHOOKINFO(module, name, options, version)                                          \
		{                                                                                          \
			xtpSkinLegacyApi##name, module, #name,                                                 \
				reinterpret_cast<PROC>(&CXTPSkinManagerLegacyApiHook::OnHook##name), options,      \
				version                                                                            \
		}

	// TODO: Consider dynamic approach for Win7 and above with all API Set modules extracted from
	// %SystemRoot%\System32\ApiSetSchema.dll. Read more on
	// http://www.geoffchappell.com/studies/windows/win32/apisetschema/index.htm.

	XTPSFHOOKINFOEX("KERNEL32.DLL", LoadLibraryA, KERNEL32, 0, NULL),
	XTPSFHOOKINFOEX("API-MS-WIN-CORE-LIBRARYLOADER-L1-1-0.DLL", LoadLibraryA,
					API_MS_WIN_CORE_LIBRARYLOADER_L1_1_0, 0, &CXTPSystemVersion::IsWin7OrGreater),
	XTPSFHOOKINFOEX("API-MS-WIN-CORE-LIBRARYLOADER-L1-1-1.DLL", LoadLibraryA,
					API_MS_WIN_CORE_LIBRARYLOADER_L1_1_1, 0, &CXTPSystemVersion::IsWin8OrGreater),
	XTPSFHOOKINFOEX("API-MS-WIN-CORE-LIBRARYLOADER-L1-2-0.DLL", LoadLibraryA,
					API_MS_WIN_CORE_LIBRARYLOADER_L1_2_0, 0, &CXTPSystemVersion::IsWin81OrGreater),
	XTPSFHOOKINFOEX("API-MS-WIN-CORE-LIBRARYLOADER-PRIVATE-L1-1-0.DLL", LoadLibraryA,
					API_MS_WIN_CORE_LIBRARYLOADER_PRIVATE_L1_1_0, 0,
					&CXTPSystemVersion::IsWin81OrGreater),
	XTPSFHOOKINFOEX("API-MS-WIN-CORE-LIBRARYLOADER-L1-2-1.DLL", LoadLibraryA,
					API_MS_WIN_CORE_LIBRARYLOADER_L1_2_1, 0, &CXTPSystemVersion::IsWin10OrGreater),
	XTPSFHOOKINFOEX("API-MS-WIN-CORE-LIBRARYLOADER-L1-2-2.DLL", LoadLibraryA,
					API_MS_WIN_CORE_LIBRARYLOADER_L1_2_2, 0, &CXTPSystemVersion::IsWin10OrGreater),
	XTPSFHOOKINFOEX("API-MS-WIN-CORE-LIBRARYLOADER-L2-1-0.DLL", LoadLibraryA,
					API_MS_WIN_CORE_LIBRARYLOADER_L2_1_0, 0, &CXTPSystemVersion::IsWin10OrGreater),
	XTPSFHOOKINFOEX("KERNEL32.DLL", LoadLibraryW, KERNEL32, 0, NULL),
	XTPSFHOOKINFOEX("API-MS-WIN-CORE-LIBRARYLOADER-L1-1-0.DLL", LoadLibraryW,
					API_MS_WIN_CORE_LIBRARYLOADER_L1_1_0, 0, &CXTPSystemVersion::IsWin7OrGreater),
	XTPSFHOOKINFOEX("API-MS-WIN-CORE-LIBRARYLOADER-L1-1-1.DLL", LoadLibraryW,
					API_MS_WIN_CORE_LIBRARYLOADER_L1_1_1, 0, &CXTPSystemVersion::IsWin8OrGreater),
	XTPSFHOOKINFOEX("API-MS-WIN-CORE-LIBRARYLOADER-L1-2-0.DLL", LoadLibraryW,
					API_MS_WIN_CORE_LIBRARYLOADER_L1_2_0, 0, &CXTPSystemVersion::IsWin81OrGreater),
	XTPSFHOOKINFOEX("API-MS-WIN-CORE-LIBRARYLOADER-PRIVATE-L1-1-0.DLL", LoadLibraryW,
					API_MS_WIN_CORE_LIBRARYLOADER_PRIVATE_L1_1_0, 0,
					&CXTPSystemVersion::IsWin81OrGreater),
	XTPSFHOOKINFOEX("API-MS-WIN-CORE-LIBRARYLOADER-L1-2-1.DLL", LoadLibraryW,
					API_MS_WIN_CORE_LIBRARYLOADER_L1_2_1, 0, &CXTPSystemVersion::IsWin10OrGreater),
	XTPSFHOOKINFOEX("API-MS-WIN-CORE-LIBRARYLOADER-L1-2-2.DLL", LoadLibraryW,
					API_MS_WIN_CORE_LIBRARYLOADER_L1_2_2, 0, &CXTPSystemVersion::IsWin10OrGreater),
	XTPSFHOOKINFOEX("API-MS-WIN-CORE-LIBRARYLOADER-L2-1-0.DLL", LoadLibraryW,
					API_MS_WIN_CORE_LIBRARYLOADER_L2_1_0, 0, &CXTPSystemVersion::IsWin10OrGreater),
	XTPSFHOOKINFOEX("KERNEL32.DLL", LoadLibraryExA, KERNEL32, 0, NULL),
	XTPSFHOOKINFOEX("API-MS-WIN-CORE-LIBRARYLOADER-L1-1-0.DLL", LoadLibraryExA,
					API_MS_WIN_CORE_LIBRARYLOADER_L1_1_0, 0, &CXTPSystemVersion::IsWin7OrGreater),
	XTPSFHOOKINFOEX("API-MS-WIN-CORE-LIBRARYLOADER-L1-1-1.DLL", LoadLibraryExA,
					API_MS_WIN_CORE_LIBRARYLOADER_L1_1_1, 0, &CXTPSystemVersion::IsWin8OrGreater),
	XTPSFHOOKINFOEX("API-MS-WIN-CORE-LIBRARYLOADER-L1-2-0.DLL", LoadLibraryExA,
					API_MS_WIN_CORE_LIBRARYLOADER_L1_2_0, 0, &CXTPSystemVersion::IsWin81OrGreater),
	XTPSFHOOKINFOEX("API-MS-WIN-CORE-LIBRARYLOADER-PRIVATE-L1-1-0.DLL", LoadLibraryExA,
					API_MS_WIN_CORE_LIBRARYLOADER_PRIVATE_L1_1_0, 0,
					&CXTPSystemVersion::IsWin81OrGreater),
	XTPSFHOOKINFOEX("API-MS-WIN-CORE-LIBRARYLOADER-L1-2-1.DLL", LoadLibraryExA,
					API_MS_WIN_CORE_LIBRARYLOADER_L1_2_1, 0, &CXTPSystemVersion::IsWin10OrGreater),
	XTPSFHOOKINFOEX("API-MS-WIN-CORE-LIBRARYLOADER-L1-2-2.DLL", LoadLibraryExA,
					API_MS_WIN_CORE_LIBRARYLOADER_L1_2_2, 0, &CXTPSystemVersion::IsWin10OrGreater),
	XTPSFHOOKINFOEX("API-MS-WIN-CORE-LIBRARYLOADER-L2-1-0.DLL", LoadLibraryExA,
					API_MS_WIN_CORE_LIBRARYLOADER_L2_1_0, 0, &CXTPSystemVersion::IsWin10OrGreater),
	XTPSFHOOKINFOEX("KERNEL32.DLL", LoadLibraryExW, KERNEL32, 0, NULL),
	XTPSFHOOKINFOEX("API-MS-WIN-CORE-LIBRARYLOADER-L1-1-0.DLL", LoadLibraryExW,
					API_MS_WIN_CORE_LIBRARYLOADER_L1_1_0, 0, &CXTPSystemVersion::IsWin7OrGreater),
	XTPSFHOOKINFOEX("API-MS-WIN-CORE-LIBRARYLOADER-L1-1-1.DLL", LoadLibraryExW,
					API_MS_WIN_CORE_LIBRARYLOADER_L1_1_1, 0, &CXTPSystemVersion::IsWin8OrGreater),
	XTPSFHOOKINFOEX("API-MS-WIN-CORE-LIBRARYLOADER-L1-2-0.DLL", LoadLibraryExW,
					API_MS_WIN_CORE_LIBRARYLOADER_L1_2_0, 0, &CXTPSystemVersion::IsWin81OrGreater),
	XTPSFHOOKINFOEX("API-MS-WIN-CORE-LIBRARYLOADER-PRIVATE-L1-1-0.DLL", LoadLibraryExW,
					API_MS_WIN_CORE_LIBRARYLOADER_PRIVATE_L1_1_0, 0,
					&CXTPSystemVersion::IsWin81OrGreater),
	XTPSFHOOKINFOEX("API-MS-WIN-CORE-LIBRARYLOADER-L1-2-1.DLL", LoadLibraryExW,
					API_MS_WIN_CORE_LIBRARYLOADER_L1_2_1, 0, &CXTPSystemVersion::IsWin10OrGreater),
	XTPSFHOOKINFOEX("API-MS-WIN-CORE-LIBRARYLOADER-L1-2-2.DLL", LoadLibraryExW,
					API_MS_WIN_CORE_LIBRARYLOADER_L1_2_2, 0, &CXTPSystemVersion::IsWin10OrGreater),
	XTPSFHOOKINFOEX("API-MS-WIN-CORE-LIBRARYLOADER-L2-1-0.DLL", LoadLibraryExW,
					API_MS_WIN_CORE_LIBRARYLOADER_L2_1_0, 0, &CXTPSystemVersion::IsWin10OrGreater),
	XTPSFHOOKINFOEX("KERNEL32.DLL", FreeLibrary, KERNEL32, 0, NULL),
	XTPSFHOOKINFOEX("API-MS-WIN-CORE-LIBRARYLOADER-L1-1-0.DLL", FreeLibrary,
					API_MS_WIN_CORE_LIBRARYLOADER_L1_1_0, 0, &CXTPSystemVersion::IsWin7OrGreater),
	XTPSFHOOKINFOEX("API-MS-WIN-CORE-LIBRARYLOADER-L1-1-1.DLL", FreeLibrary,
					API_MS_WIN_CORE_LIBRARYLOADER_L1_1_1, 0, &CXTPSystemVersion::IsWin8OrGreater),
	XTPSFHOOKINFOEX("API-MS-WIN-CORE-LIBRARYLOADER-L1-2-0.DLL", FreeLibrary,
					API_MS_WIN_CORE_LIBRARYLOADER_L1_2_0, 0, &CXTPSystemVersion::IsWin81OrGreater),
	XTPSFHOOKINFOEX("API-MS-WIN-CORE-LIBRARYLOADER-PRIVATE-L1-1-0.DLL", FreeLibrary,
					API_MS_WIN_CORE_LIBRARYLOADER_PRIVATE_L1_1_0, 0,
					&CXTPSystemVersion::IsWin81OrGreater),
	XTPSFHOOKINFOEX("API-MS-WIN-CORE-LIBRARYLOADER-L1-2-1.DLL", FreeLibrary,
					API_MS_WIN_CORE_LIBRARYLOADER_L1_2_1, 0, &CXTPSystemVersion::IsWin10OrGreater),
	XTPSFHOOKINFOEX("API-MS-WIN-CORE-LIBRARYLOADER-L1-2-2.DLL", FreeLibrary,
					API_MS_WIN_CORE_LIBRARYLOADER_L1_2_2, 0, &CXTPSystemVersion::IsWin10OrGreater),
	XTPSFHOOKINFOEX("API-MS-WIN-CORE-LIBRARYLOADER-L2-1-0.DLL", FreeLibrary,
					API_MS_WIN_CORE_LIBRARYLOADER_L2_1_0, 0, &CXTPSystemVersion::IsWin10OrGreater),
	XTPSFHOOKINFOEX("KERNEL32.DLL", FreeLibraryAndExitThread, KERNEL32, 0, NULL),
	XTPSFHOOKINFOEX("API-MS-WIN-CORE-LIBRARYLOADER-L1-1-0.DLL", FreeLibraryAndExitThread,
					API_MS_WIN_CORE_LIBRARYLOADER_L1_1_0, 0, &CXTPSystemVersion::IsWin7OrGreater),
	XTPSFHOOKINFOEX("API-MS-WIN-CORE-LIBRARYLOADER-L1-1-1.DLL", FreeLibraryAndExitThread,
					API_MS_WIN_CORE_LIBRARYLOADER_L1_1_1, 0, &CXTPSystemVersion::IsWin8OrGreater),
	XTPSFHOOKINFOEX("API-MS-WIN-CORE-LIBRARYLOADER-L1-2-0.DLL", FreeLibraryAndExitThread,
					API_MS_WIN_CORE_LIBRARYLOADER_L1_2_0, 0, &CXTPSystemVersion::IsWin81OrGreater),
	XTPSFHOOKINFOEX("API-MS-WIN-CORE-LIBRARYLOADER-PRIVATE-L1-1-0.DLL", FreeLibraryAndExitThread,
					API_MS_WIN_CORE_LIBRARYLOADER_PRIVATE_L1_1_0, 0,
					&CXTPSystemVersion::IsWin81OrGreater),
	XTPSFHOOKINFOEX("API-MS-WIN-CORE-LIBRARYLOADER-L1-2-1.DLL", FreeLibraryAndExitThread,
					API_MS_WIN_CORE_LIBRARYLOADER_L1_2_1, 0, &CXTPSystemVersion::IsWin10OrGreater),
	XTPSFHOOKINFOEX("API-MS-WIN-CORE-LIBRARYLOADER-L1-2-2.DLL", FreeLibraryAndExitThread,
					API_MS_WIN_CORE_LIBRARYLOADER_L1_2_2, 0, &CXTPSystemVersion::IsWin10OrGreater),
	XTPSFHOOKINFOEX("API-MS-WIN-CORE-LIBRARYLOADER-L2-1-0.DLL", FreeLibraryAndExitThread,
					API_MS_WIN_CORE_LIBRARYLOADER_L2_1_0, 0, &CXTPSystemVersion::IsWin10OrGreater),
	XTPSFHOOKINFOEX("KERNEL32.DLL", GetProcAddress, KERNEL32, 0, NULL),
	XTPSFHOOKINFOEX("API-MS-WIN-CORE-LIBRARYLOADER-L1-1-0.DLL", GetProcAddress,
					API_MS_WIN_CORE_LIBRARYLOADER_L1_1_0, 0, &CXTPSystemVersion::IsWin7OrGreater),
	XTPSFHOOKINFOEX("API-MS-WIN-CORE-LIBRARYLOADER-L1-1-1.DLL", GetProcAddress,
					API_MS_WIN_CORE_LIBRARYLOADER_L1_1_1, 0, &CXTPSystemVersion::IsWin8OrGreater),
	XTPSFHOOKINFOEX("API-MS-WIN-CORE-LIBRARYLOADER-L1-2-0.DLL", GetProcAddress,
					API_MS_WIN_CORE_LIBRARYLOADER_L1_2_0, 0, &CXTPSystemVersion::IsWin81OrGreater),
	XTPSFHOOKINFOEX("API-MS-WIN-CORE-LIBRARYLOADER-PRIVATE-L1-1-0.DLL", GetProcAddress,
					API_MS_WIN_CORE_LIBRARYLOADER_PRIVATE_L1_1_0, 0,
					&CXTPSystemVersion::IsWin81OrGreater),
	XTPSFHOOKINFOEX("API-MS-WIN-CORE-LIBRARYLOADER-L1-2-1.DLL", GetProcAddress,
					API_MS_WIN_CORE_LIBRARYLOADER_L1_2_1, 0, &CXTPSystemVersion::IsWin10OrGreater),
	XTPSFHOOKINFOEX("API-MS-WIN-CORE-LIBRARYLOADER-L1-2-2.DLL", GetProcAddress,
					API_MS_WIN_CORE_LIBRARYLOADER_L1_2_2, 0, &CXTPSystemVersion::IsWin10OrGreater),
	XTPSFHOOKINFOEX("API-MS-WIN-CORE-LIBRARYLOADER-L2-1-0.DLL", GetProcAddress,
					API_MS_WIN_CORE_LIBRARYLOADER_L2_1_0, 0, &CXTPSystemVersion::IsWin10OrGreater),
	XTPSFHOOKINFOEX("KERNEL32.DLL", GetModuleHandleA, KERNEL32, 0, NULL),
	XTPSFHOOKINFOEX("API-MS-WIN-CORE-LIBRARYLOADER-L1-1-0.DLL", GetModuleHandleA,
					API_MS_WIN_CORE_LIBRARYLOADER_L1_1_0, 0, &CXTPSystemVersion::IsWin7OrGreater),
	XTPSFHOOKINFOEX("API-MS-WIN-CORE-LIBRARYLOADER-L1-1-1.DLL", GetModuleHandleA,
					API_MS_WIN_CORE_LIBRARYLOADER_L1_1_1, 0, &CXTPSystemVersion::IsWin8OrGreater),
	XTPSFHOOKINFOEX("API-MS-WIN-CORE-LIBRARYLOADER-L1-2-0.DLL", GetModuleHandleA,
					API_MS_WIN_CORE_LIBRARYLOADER_L1_2_0, 0, &CXTPSystemVersion::IsWin81OrGreater),
	XTPSFHOOKINFOEX("API-MS-WIN-CORE-LIBRARYLOADER-PRIVATE-L1-1-0.DLL", GetModuleHandleA,
					API_MS_WIN_CORE_LIBRARYLOADER_PRIVATE_L1_1_0, 0,
					&CXTPSystemVersion::IsWin81OrGreater),
	XTPSFHOOKINFOEX("API-MS-WIN-CORE-LIBRARYLOADER-L1-2-1.DLL", GetModuleHandleA,
					API_MS_WIN_CORE_LIBRARYLOADER_L1_2_1, 0, &CXTPSystemVersion::IsWin10OrGreater),
	XTPSFHOOKINFOEX("API-MS-WIN-CORE-LIBRARYLOADER-L1-2-2.DLL", GetModuleHandleA,
					API_MS_WIN_CORE_LIBRARYLOADER_L1_2_2, 0, &CXTPSystemVersion::IsWin10OrGreater),
	XTPSFHOOKINFOEX("API-MS-WIN-CORE-LIBRARYLOADER-L2-1-0.DLL", GetModuleHandleA,
					API_MS_WIN_CORE_LIBRARYLOADER_L2_1_0, 0, &CXTPSystemVersion::IsWin10OrGreater),
	XTPSFHOOKINFOEX("KERNEL32.DLL", GetModuleHandleW, KERNEL32, 0, NULL),
	XTPSFHOOKINFOEX("API-MS-WIN-CORE-LIBRARYLOADER-L1-1-0.DLL", GetModuleHandleW,
					API_MS_WIN_CORE_LIBRARYLOADER_L1_1_0, 0, &CXTPSystemVersion::IsWin7OrGreater),
	XTPSFHOOKINFOEX("API-MS-WIN-CORE-LIBRARYLOADER-L1-1-1.DLL", GetModuleHandleW,
					API_MS_WIN_CORE_LIBRARYLOADER_L1_1_1, 0, &CXTPSystemVersion::IsWin8OrGreater),
	XTPSFHOOKINFOEX("API-MS-WIN-CORE-LIBRARYLOADER-L1-2-0.DLL", GetModuleHandleW,
					API_MS_WIN_CORE_LIBRARYLOADER_L1_2_0, 0, &CXTPSystemVersion::IsWin81OrGreater),
	XTPSFHOOKINFOEX("API-MS-WIN-CORE-LIBRARYLOADER-PRIVATE-L1-1-0.DLL", GetModuleHandleW,
					API_MS_WIN_CORE_LIBRARYLOADER_PRIVATE_L1_1_0, 0,
					&CXTPSystemVersion::IsWin81OrGreater),
	XTPSFHOOKINFOEX("API-MS-WIN-CORE-LIBRARYLOADER-L1-2-1.DLL", GetModuleHandleW,
					API_MS_WIN_CORE_LIBRARYLOADER_L1_2_1, 0, &CXTPSystemVersion::IsWin10OrGreater),
	XTPSFHOOKINFOEX("API-MS-WIN-CORE-LIBRARYLOADER-L1-2-2.DLL", GetModuleHandleW,
					API_MS_WIN_CORE_LIBRARYLOADER_L1_2_2, 0, &CXTPSystemVersion::IsWin10OrGreater),
	XTPSFHOOKINFOEX("API-MS-WIN-CORE-LIBRARYLOADER-L2-1-0.DLL", GetModuleHandleW,
					API_MS_WIN_CORE_LIBRARYLOADER_L2_1_0, 0, &CXTPSystemVersion::IsWin10OrGreater),
	XTPSFHOOKINFOEX("KERNEL32.DLL", GetModuleHandleExA, KERNEL32, 0, NULL),
	XTPSFHOOKINFOEX("API-MS-WIN-CORE-LIBRARYLOADER-L1-1-0.DLL", GetModuleHandleExA,
					API_MS_WIN_CORE_LIBRARYLOADER_L1_1_0, 0, &CXTPSystemVersion::IsWin7OrGreater),
	XTPSFHOOKINFOEX("API-MS-WIN-CORE-LIBRARYLOADER-L1-1-1.DLL", GetModuleHandleExA,
					API_MS_WIN_CORE_LIBRARYLOADER_L1_1_1, 0, &CXTPSystemVersion::IsWin8OrGreater),
	XTPSFHOOKINFOEX("API-MS-WIN-CORE-LIBRARYLOADER-L1-2-0.DLL", GetModuleHandleExA,
					API_MS_WIN_CORE_LIBRARYLOADER_L1_2_0, 0, &CXTPSystemVersion::IsWin81OrGreater),
	XTPSFHOOKINFOEX("API-MS-WIN-CORE-LIBRARYLOADER-PRIVATE-L1-1-0.DLL", GetModuleHandleExA,
					API_MS_WIN_CORE_LIBRARYLOADER_PRIVATE_L1_1_0, 0,
					&CXTPSystemVersion::IsWin81OrGreater),
	XTPSFHOOKINFOEX("API-MS-WIN-CORE-LIBRARYLOADER-L1-2-1.DLL", GetModuleHandleExA,
					API_MS_WIN_CORE_LIBRARYLOADER_L1_2_1, 0, &CXTPSystemVersion::IsWin10OrGreater),
	XTPSFHOOKINFOEX("API-MS-WIN-CORE-LIBRARYLOADER-L1-2-2.DLL", GetModuleHandleExA,
					API_MS_WIN_CORE_LIBRARYLOADER_L1_2_2, 0, &CXTPSystemVersion::IsWin10OrGreater),
	XTPSFHOOKINFOEX("API-MS-WIN-CORE-LIBRARYLOADER-L2-1-0.DLL", GetModuleHandleExA,
					API_MS_WIN_CORE_LIBRARYLOADER_L2_1_0, 0, &CXTPSystemVersion::IsWin10OrGreater),
	XTPSFHOOKINFOEX("KERNEL32.DLL", GetModuleHandleExW, KERNEL32, 0, NULL),
	XTPSFHOOKINFOEX("API-MS-WIN-CORE-LIBRARYLOADER-L1-1-0.DLL", GetModuleHandleExW,
					API_MS_WIN_CORE_LIBRARYLOADER_L1_1_0, 0, &CXTPSystemVersion::IsWin7OrGreater),
	XTPSFHOOKINFOEX("API-MS-WIN-CORE-LIBRARYLOADER-L1-1-1.DLL", GetModuleHandleExW,
					API_MS_WIN_CORE_LIBRARYLOADER_L1_1_1, 0, &CXTPSystemVersion::IsWin8OrGreater),
	XTPSFHOOKINFOEX("API-MS-WIN-CORE-LIBRARYLOADER-L1-2-0.DLL", GetModuleHandleExW,
					API_MS_WIN_CORE_LIBRARYLOADER_L1_2_0, 0, &CXTPSystemVersion::IsWin81OrGreater),
	XTPSFHOOKINFOEX("API-MS-WIN-CORE-LIBRARYLOADER-PRIVATE-L1-1-0.DLL", GetModuleHandleExW,
					API_MS_WIN_CORE_LIBRARYLOADER_PRIVATE_L1_1_0, 0,
					&CXTPSystemVersion::IsWin81OrGreater),
	XTPSFHOOKINFOEX("API-MS-WIN-CORE-LIBRARYLOADER-L1-2-1.DLL", GetModuleHandleExW,
					API_MS_WIN_CORE_LIBRARYLOADER_L1_2_1, 0, &CXTPSystemVersion::IsWin10OrGreater),
	XTPSFHOOKINFOEX("API-MS-WIN-CORE-LIBRARYLOADER-L1-2-2.DLL", GetModuleHandleExW,
					API_MS_WIN_CORE_LIBRARYLOADER_L1_2_2, 0, &CXTPSystemVersion::IsWin10OrGreater),
	XTPSFHOOKINFOEX("API-MS-WIN-CORE-LIBRARYLOADER-L2-1-0.DLL", GetModuleHandleExW,
					API_MS_WIN_CORE_LIBRARYLOADER_L2_1_0, 0, &CXTPSystemVersion::IsWin10OrGreater),
	XTPSFHOOKINFOEX("KERNEL32.DLL", CreateThread, KERNEL32, 0, NULL),
	XTPSFHOOKINFOEX("API-MS-WIN-CORE-PROCESSTHREADS-L1-1-1.DLL", CreateThread,
					API_MS_WIN_CORE_PROCESSTHREADS_L1_1_1, 0, &CXTPSystemVersion::IsWin8OrGreater),
	XTPSFHOOKINFOEX("API-MS-WIN-CORE-PROCESSTHREADS-L1-1-2.DLL", CreateThread,
					API_MS_WIN_CORE_PROCESSTHREADS_L1_1_2, 0, &CXTPSystemVersion::IsWin81OrGreater),

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
	XTPSFHOOKINFOEX("USER32.DLL", TrackPopupMenuEx, USER32, xtpSkinApplyMenus, NULL),
	XTPSFHOOKINFOEX("WIN32U.DLL", NtUserTrackPopupMenuEx, WIN32U, xtpSkinApplyMenus,
					&CXTPSystemVersion::IsWin7OrGreater),
	XTPSFHOOKINFO("USER32.DLL", GetMenuItemRect, xtpSkinApplyMenus, NULL)

#	undef XTPSFHOOKINFO
#	undef XTPSFHOOKINFOEX
};

void CXTPSkinManagerLegacyApiHook::InitializeHookManagement()
{
	XTP_GUARD_EXCLUSIVE(CXTPSkinManagerLegacyApiHook, this)
	{
		ASSERT(!m_bInitializationInProgress);

		if (m_bInitialized || m_bInitializationInProgress)
			return;

		m_bInitializationInProgress = TRUE;

		XTP_GUARD_SHARED(CXTPSkinManager, XTPSkinManager())
		{
			ProcessExcludedModules();

			CXTPSkinManagerModuleList moduleList(::GetCurrentProcessId(), TRUE);
			moduleList.PreLoad(TRUE);

			for (int i = 0; i < _countof(m_apiHookInfos); ++i)
			{
				const APIHOOKINFO& hookInfo = m_apiHookInfos[i];

				if (NULL != hookInfo.pfnVersionCheck
					&& !(XTPSystemVersion()->*hookInfo.pfnVersionCheck)())
					continue;

				if (0 != hookInfo.nApplyOptions
					&& 0 == (XTPSkinManager()->GetApplyOptions() & hookInfo.nApplyOptions))
					continue;

				HookImport(hookInfo.nIndex, hookInfo.lpModuleName, hookInfo.lpRoutineName,
						   hookInfo.pfnHook, &moduleList);
			}

			IncrementAllModulesReferenceCounters(&moduleList);

			m_bInitializationInProgress = FALSE;
			m_bInitialized				= TRUE;
		}
	}
}

void CXTPSkinManagerLegacyApiHook::FinalizeHookManagement()
{
	XTP_GUARD_EXCLUSIVE(CXTPSkinManagerLegacyApiHook, this)
	{
		if (!m_bInitialized)
			return;

		UnhookAllFunctions(FALSE);

		m_bInitialized = FALSE;
	}
}

void CXTPSkinManagerLegacyApiHook::ExcludeModule(LPCTSTR lpszModule, BOOL bWin9x)
{
	EXCLUDEDMODULE em;
	em.bWin9x	= bWin9x;
	em.strModule = lpszModule;
	em.hModule   = 0;
	XTPAccessExclusive(this)->m_arrExcludedModules.Add(em);
}

LPVOID CXTPSkinManagerLegacyApiHook::GetOriginalFunctionAddress(int nFunction) const
{
	CXTPSkinManagerLegacyApiFunction* pFunction =
		const_cast<CXTPSkinManagerLegacyApiHook*>(this)->GetHookedFunction(
			static_cast<XTPSkinFrameworkLegacyApiFunctionIndex>(nFunction));
	return (NULL != pFunction ? pFunction->GetOriginal() : NULL);
}

BOOL CXTPSkinManagerLegacyApiHook::AdjustWindowRectExOrig(LPRECT lpRect, DWORD dwStyle, BOOL bMenu,
														  DWORD dwExStyle)
{
	CXTPSkinManagerLegacyApiFunction* pFunction = EnterHookedCall(
		xtpSkinLegacyApiAdjustWindowRectEx);
	if (NULL == pFunction)
		return FALSE;

	typedef BOOL(WINAPI * LPFNADJUSTWINDOWRECTEX)(LPRECT lpRect, DWORD dwStyle, BOOL bMenu,
												  DWORD dwExStyle);
	return ((LPFNADJUSTWINDOWRECTEX)pFunction->GetOriginal())(lpRect, dwStyle, bMenu, dwExStyle);
}

LRESULT CXTPSkinManagerLegacyApiHook::CallWindowProcOrig(WNDPROC lpPrevWndFunc, HWND hWnd, UINT Msg,
														 WPARAM wParam, LPARAM lParam)
{
	XTP_SKINFRAMEWORK_ASSERT_WINDOW_THREAD(hWnd);

#	ifdef _UNICODE
	LPVOID pfnOrig = GetOriginalFunctionAddress(xtpSkinLegacyApiCallWindowProcW);
#	else
	LPVOID pfnOrig = GetOriginalFunctionAddress(xtpSkinLegacyApiCallWindowProcA);
#	endif

	if (pfnOrig)
	{
		typedef LRESULT(WINAPI * LPFNCALLWINDOWPROC)(WNDPROC lpPrevWndFunc, HWND hWnd, UINT Msg,
													 WPARAM wParam, LPARAM lParam);
		return ((LPFNCALLWINDOWPROC)pfnOrig)(lpPrevWndFunc, hWnd, Msg, wParam, lParam);
	}

	return ::CallWindowProc(lpPrevWndFunc, hWnd, Msg, wParam, lParam);
}

LPVOID CXTPSkinManagerLegacyApiHook::GetOpenThemeDataOriginalPtr() const
{
	return GetOriginalFunctionAddress(xtpSkinLegacyApiOpenThemeData);
}

LPVOID CXTPSkinManagerLegacyApiHook::GetCloseThemeDataOriginalPtr() const
{
	return GetOriginalFunctionAddress(xtpSkinLegacyApiCloseThemeData);
}

void CXTPSkinManagerLegacyApiHook::ConnectWrapper(CXTPWinThemeWrapper* pThemeWrapper)
{
	pThemeWrapper->CreateSharedData();
	pThemeWrapper->SetThemeWrapperPtr(xtpWrapperOpenThemeData,
									  &CXTPSkinManagerLegacyApiHook::OnHookOpenThemeData);
	pThemeWrapper->SetThemeWrapperPtr(xtpWrapperDrawThemeBackground,
									  &CXTPSkinManagerLegacyApiHook::OnHookDrawThemeBackground);
	pThemeWrapper->SetThemeWrapperPtr(xtpWrapperCloseThemeData,
									  &CXTPSkinManagerLegacyApiHook::OnHookCloseThemeData);
	pThemeWrapper->SetThemeWrapperPtr(xtpWrapperGetThemeColor,
									  &CXTPSkinManagerLegacyApiHook::OnHookGetThemeColor);
	pThemeWrapper->SetThemeWrapperPtr(xtpWrapperGetThemeInt,
									  &CXTPSkinManagerLegacyApiHook::OnHookGetThemeInt);
	pThemeWrapper->SetThemeWrapperPtr(xtpWrapperIsAppThemed,
									  &CXTPSkinManagerLegacyApiHook::OnHookIsAppThemed);
	pThemeWrapper->SetThemeWrapperPtr(xtpWrapperIsThemeActive,
									  &CXTPSkinManagerLegacyApiHook::OnHookIsThemeActive);
	pThemeWrapper->SetThemeWrapperPtr(xtpWrapperGetCurrentThemeName,
									  &CXTPSkinManagerLegacyApiHook::OnHookGetCurrentThemeName);
	pThemeWrapper->SetThemeWrapperPtr(xtpWrapperGetThemeSysBool,
									  &CXTPSkinManagerLegacyApiHook::OnHookGetThemeSysBool);
	pThemeWrapper->SetThemeWrapperPtr(xtpWrapperGetThemeSysColor,
									  &CXTPSkinManagerLegacyApiHook::OnHookGetThemeSysColor);
	pThemeWrapper->SetThemeWrapperPtr(xtpWrapperGetThemePartSize,
									  &CXTPSkinManagerLegacyApiHook::OnHookGetThemePartSize);

	{
		CXTPSkinManagerModuleList moduleList(::GetCurrentProcessId(), TRUE);
		moduleList.PreLoad(TRUE);

		HookImport(xtpSkinLegacyApiOpenThemeData, "UXTHEME.DLL", "OpenThemeData",
				   (PROC)&CXTPSkinManagerLegacyApiHook::OnHookOpenThemeData, &moduleList);
		HookImport(xtpSkinLegacyApiDrawThemeBackground, "UXTHEME.DLL", "DrawThemeBackground",
				   (PROC)&CXTPSkinManagerLegacyApiHook::OnHookDrawThemeBackground, &moduleList);
		HookImport(xtpSkinLegacyApiCloseThemeData, "UXTHEME.DLL", "CloseThemeData",
				   (PROC)&CXTPSkinManagerLegacyApiHook::OnHookCloseThemeData, &moduleList);
		HookImport(xtpSkinLegacyApiGetThemeColor, "UXTHEME.DLL", "GetThemeColor",
				   (PROC)&CXTPSkinManagerLegacyApiHook::OnHookGetThemeColor, &moduleList);
		HookImport(xtpSkinLegacyApiGetThemeInt, "UXTHEME.DLL", "GetThemeInt",
				   (PROC)&CXTPSkinManagerLegacyApiHook::OnHookGetThemeInt, &moduleList);
		HookImport(xtpSkinLegacyApiIsAppThemed, "UXTHEME.DLL", "IsAppThemed",
				   (PROC)&CXTPSkinManagerLegacyApiHook::OnHookIsAppThemed, &moduleList);
		HookImport(xtpSkinLegacyApiIsThemeActive, "UXTHEME.DLL", "IsThemeActive",
				   (PROC)&CXTPSkinManagerLegacyApiHook::OnHookIsThemeActive, &moduleList);
		HookImport(xtpSkinLegacyApiGetCurrentThemeName, "UXTHEME.DLL", "GetCurrentThemeName",
				   (PROC)&CXTPSkinManagerLegacyApiHook::OnHookGetCurrentThemeName, &moduleList);
		HookImport(xtpSkinLegacyApiGetThemeSysBool, "UXTHEME.DLL", "GetThemeSysBool",
				   (PROC)&CXTPSkinManagerLegacyApiHook::OnHookGetThemeSysBool, &moduleList);
		HookImport(xtpSkinLegacyApiGetThemeSysColor, "UXTHEME.DLL", "GetThemeSysColor",
				   (PROC)&CXTPSkinManagerLegacyApiHook::OnHookGetThemeSysColor, &moduleList);
		HookImport(xtpSkinLegacyApiGetThemePartSize, "UXTHEME.DLL", "GetThemePartSize",
				   (PROC)&CXTPSkinManagerLegacyApiHook::OnHookGetThemePartSize, &moduleList);
	}
}

CXTPSkinManagerLegacyApiHook* CXTPSkinManagerLegacyApiHook::GetInstance()
{
	CXTPSkinManagerApiHookBase* pBase = XTPSkinManager()->GetApiHook();
	ASSERT_KINDOF(CXTPSkinManagerLegacyApiHook, pBase);
	return static_cast<CXTPSkinManagerLegacyApiHook*>(pBase);
}

BOOL CXTPSkinManagerLegacyApiHook::IsModuleExcluded(HMODULE hModule) const
{
	XTP_GUARD_SHARED_CONST(CXTPSkinManagerLegacyApiHook, this)
	{
		BOOL bWin9x = XTPSkinManager()->IsWin9x();

		for (int i = 0; i < (int)m_arrExcludedModules.GetSize(); i++)
		{
			const EXCLUDEDMODULE& em = m_arrExcludedModules[i];

			if (em.bWin9x && !bWin9x)
				continue;

			if (em.hModule == hModule)
				return TRUE;

			if (em.strModule[em.strModule.GetLength() - 1] == '*')
			{
				TCHAR fn[MAX_PATH];
				GetModuleFileName(hModule, fn, MAX_PATH);

				TCHAR* lpszName = _tcsrchr(fn, '\\');
				if (lpszName == NULL)
					lpszName = fn;
				else
					lpszName++;

				if (XTPCompareStringNoCase(lpszName, em.strModule, em.strModule.GetLength() - 1)
					== 0)
					return TRUE;
			}
		}
	}

	return FALSE;
}

void CXTPSkinManagerLegacyApiHook::UnhookAllFunctions(BOOL bFinal)
{
	XTP_GUARD_EXCLUSIVE(CXTPSkinManagerLegacyApiHook, this)
	{
		// Unhoook all functions
		{
			CXTPSkinManagerModuleList moduleList(::GetCurrentProcessId(), TRUE);
			moduleList.PreLoad(TRUE);
			IncrementAllModulesReferenceCounters(&moduleList, FALSE);

			for (int i = 0; i < XTP_SKIN_LEGACY_API_HOOK_COUNT; i++)
			{
				CXTPSkinManagerLegacyApiFunction*& pHook = m_arrFunctions[i];
				if (NULL != pHook)
				{
					if (pHook->IsHooked())
					{
						pHook->UnhookImport(&moduleList);
					}
				}
			}
		}

		// Delete all functions' instances on final release.
		if (bFinal)
		{
			for (int i = 0; i < XTP_SKIN_LEGACY_API_HOOK_COUNT; i++)
			{
				CXTPSkinManagerLegacyApiFunction*& pHook = m_arrFunctions[i];
				if (NULL != pHook)
				{
					delete pHook;
					pHook = NULL;
				}
			}
		}
	}
}

void CXTPSkinManagerLegacyApiHook::UnhookAllFunctions(HMODULE hMod)
{
	XTP_GUARD_EXCLUSIVE(CXTPSkinManagerLegacyApiHook, this)
	{
		for (int i = 0; i < XTP_SKIN_LEGACY_API_HOOK_COUNT; i++)
		{
			CXTPSkinManagerLegacyApiFunction*& pHook = m_arrFunctions[i];
			if (NULL != pHook)
			{
				XTP_GUARD_EXCLUSIVE(CXTPSkinManagerLegacyApiFunction, pHook)
				{
					if (pHook->IsHooked())
					{
						pHook->UnhookImport(hMod);
					}
				}
			}
		}
	}
}

CXTPSkinManagerLegacyApiFunction* CXTPSkinManagerLegacyApiHook::HookImport(
	XTPSkinFrameworkLegacyApiFunctionIndex nIndex, LPCSTR pszCalleeModName, LPCSTR pszFuncName,
	PROC pfnHook, CXTPSkinManagerModuleList* pModuleList)
{
	CXTPSkinManagerLegacyApiFunction* pResult = NULL;

	try
	{
		PROC pfnOrig = GetProcAddressWindows(GetModuleHandleAWindows(pszCalleeModName),
											 pszFuncName);

		if (NULL == pfnOrig)
		{
			HMODULE hmod = ::LoadLibraryA(pszCalleeModName);
			if (NULL != hmod)
			{
				pfnOrig = GetProcAddressWindows(GetModuleHandleAWindows(pszCalleeModName),
												pszFuncName);
				if (NULL == pfnOrig)
				{
					::FreeLibrary(hmod);
				}
			}
		}

		if (NULL != pfnOrig)
		{
			pResult = AddHook(nIndex, pszCalleeModName, pszFuncName, pfnOrig, pfnHook, pModuleList);
		}
	}
	catch (...)
	{
	}

	return pResult;
}

CXTPSkinManagerLegacyApiFunction* CXTPSkinManagerLegacyApiHook::AddHook(
	XTPSkinFrameworkLegacyApiFunctionIndex nIndex, LPCSTR pszCalleeModName, LPCSTR pszFuncName,
	PROC pfnOrig, PROC pfnHook, CXTPSkinManagerModuleList* pModuleList)
{
	ASSERT(nIndex < XTP_SKIN_LEGACY_API_HOOK_COUNT);
	if (nIndex >= XTP_SKIN_LEGACY_API_HOOK_COUNT)
		return NULL;

	XTP_GUARD_EXCLUSIVE(CXTPSkinManagerLegacyApiHook, this)
	{
		CXTPSkinManagerLegacyApiFunction* pHook = m_arrFunctions[nIndex];
		if (NULL == pHook)
		{
			// Starting from Win7 some functions can be already hooked by
			// apphelp.dll but not everywhere, in this case we need to find
			// initial function address manually and use it as an alternative
			// original function address.
			PROC pfnAltOrig = NULL;
			if (m_bInAppCompatMode && IsHookedByAppHelpDll(pfnOrig))
			{
				HMODULE hColleeModule = GetModuleHandleA(pszCalleeModName);
				if (NULL != hColleeModule)
				{
					pfnAltOrig = GetProcAddressManual(hColleeModule, pszFuncName);
				}
			}

			pHook = new CXTPSkinManagerLegacyApiFunction(this, pszCalleeModName, pszFuncName,
														 pfnOrig, pfnHook, pfnAltOrig);
			m_arrFunctions[nIndex] = pHook;
		}

		pHook->HookImport(pModuleList);

		return pHook;
	}
}

FARPROC WINAPI CXTPSkinManagerLegacyApiHook::GetProcAddressWindows(HMODULE hModule,
																   LPCSTR pszProcName)
{
	LPVOID pfnOrig = XTPSkinManager()->GetApiHook()->GetOriginalFunctionAddress(
		xtpSkinLegacyApiGetProcAddress_KERNEL32);

	if (NULL != pfnOrig)
	{
		typedef FARPROC(WINAPI * LPFNGETPROCADDRESS)(HMODULE hModule, LPCSTR pszProcName);
		return ((LPFNGETPROCADDRESS)pfnOrig)(hModule, pszProcName);
	}

	return ::GetProcAddress(hModule, pszProcName);
}

FARPROC WINAPI CXTPSkinManagerLegacyApiHook::GetProcAddressManual(HMODULE hModule,
																  LPCSTR pszProcName)
{
	LPVOID pFunction   = NULL;
	PBYTE pModuleStart = reinterpret_cast<PBYTE>(hModule);

	PIMAGE_DOS_HEADER pDosHdr = reinterpret_cast<PIMAGE_DOS_HEADER>(pModuleStart);
	ASSERT(::AfxIsValidAddress(pDosHdr, sizeof(*pDosHdr), FALSE));

	if (pDosHdr->e_magic == IMAGE_DOS_SIGNATURE)
	{
		PIMAGE_NT_HEADERS pNtHdr = reinterpret_cast<PIMAGE_NT_HEADERS>(pModuleStart
																	   + pDosHdr->e_lfanew);
		ASSERT(::AfxIsValidAddress(pNtHdr, sizeof(*pNtHdr), FALSE));

		if (pNtHdr->Signature == IMAGE_NT_SIGNATURE)
		{
			PIMAGE_DATA_DIRECTORY pExportDataDir =
				&pNtHdr->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];
			if (0 != pExportDataDir->VirtualAddress)
			{
				PIMAGE_EXPORT_DIRECTORY pExportDir = reinterpret_cast<PIMAGE_EXPORT_DIRECTORY>(
					pModuleStart + pExportDataDir->VirtualAddress);
				for (DWORD i = 0; i < pExportDir->NumberOfNames - 1; ++i)
				{
					DWORD_PTR address = reinterpret_cast<DWORD_PTR>(
						pModuleStart + (pExportDir->AddressOfNames + (i * sizeof(DWORD))));
					LPCSTR name = reinterpret_cast<LPCSTR>(pModuleStart
														   + *reinterpret_cast<PDWORD>(address));
					if (0 == strcmp(name, pszProcName))
					{
						address		  = pExportDir->AddressOfNameOrdinals + (i * sizeof(WORD));
						DWORD ordinal = *reinterpret_cast<PDWORD>(pModuleStart + address) & 0xffff;
						address		  = pExportDir->AddressOfFunctions + (ordinal * sizeof(DWORD));
						pFunction	 = pModuleStart
									+ *reinterpret_cast<PDWORD>(pModuleStart + address);
						if ((pModuleStart + pExportDataDir->VirtualAddress) < pFunction)
						{
							if (pFunction < (pModuleStart + pExportDataDir->VirtualAddress
											 + pExportDataDir->Size))
							{
								// Routine is forwarded
								LPCSTR functionName  = reinterpret_cast<LPCSTR>(pFunction);
								LPCSTR forwardedName = strstr(functionName, ".") + 1;
								DWORD_PTR diffSize   = forwardedName - functionName;
								char* libName		 = new char[diffSize];
								lstrcpynA(libName, functionName, static_cast<int>(diffSize));
								HMODULE library = GetModuleHandleA(libName);
								if (NULL != library)
								{
									pFunction = GetProcAddressManual(library, forwardedName);
								}
								delete[] libName;
							}
						}
					}
				}
			}
		}
	}

	return reinterpret_cast<FARPROC>(pFunction);
}

HMODULE WINAPI CXTPSkinManagerLegacyApiHook::GetModuleHandleAWindows(LPCSTR pszModuleName)
{
	ASSERT(NULL != pszModuleName);

	LPVOID pfnOrig = XTPSkinManager()->GetApiHook()->GetOriginalFunctionAddress(
		xtpSkinLegacyApiGetModuleHandleA_KERNEL32);

	if (NULL != pfnOrig)
	{
		typedef HMODULE(WINAPI * LPFNGETMODULEHANDLEA)(LPCSTR pszModuleName);
		return ((LPFNGETMODULEHANDLEA)pfnOrig)(pszModuleName);
	}

	return ::GetModuleHandleA(pszModuleName);
}

CXTPSkinManagerLegacyApiFunction*
	CXTPSkinManagerLegacyApiHook::GetHookedFunction(XTPSkinFrameworkLegacyApiFunctionIndex nIndex)
{
	ASSERT(nIndex < XTP_SKIN_LEGACY_API_HOOK_COUNT);
	if (nIndex < 0 || nIndex >= XTP_SKIN_LEGACY_API_HOOK_COUNT)
		return NULL;

	return (!m_bInitializationInProgress
				? XTPAccessShared(this, m_nLockCondition)->m_arrFunctions[nIndex]
				: m_arrFunctions[nIndex]);
}

CXTPSkinManagerLegacyApiFunction*
	CXTPSkinManagerLegacyApiHook::GetHookedFunction(LPCSTR pszCalleeModName, LPCSTR pszFuncName)
{
	XTP_GUARD_SHARED(CXTPSkinManagerLegacyApiHook, this)
	{
		for (int i = 0; i < XTP_SKIN_LEGACY_API_HOOK_COUNT; i++)
		{
			CXTPSkinManagerLegacyApiFunction* pHook = m_arrFunctions[i];
			if (!pHook)
			{
				continue;
			}

			XTP_GUARD_SHARED(CXTPSkinManagerLegacyApiFunction, pHook)
			{
				if (XTPCompareStringNoCase(pHook->m_szCalleeModName, pszCalleeModName) == 0
					&& XTPCompareStringNoCase(pHook->m_szFuncName, pszFuncName) == 0)
				{
					return pHook;
				}
			}
		}
	}

	return NULL;
}

void CXTPSkinManagerLegacyApiHook::ProcessExcludedModules()
{
	for (INT_PTR i = 0; i < m_arrExcludedModules.GetSize(); ++i)
	{
		CString& strModule = m_arrExcludedModules[i].strModule;

		if (strModule[strModule.GetLength() - 1] != _T('*'))
		{
			m_arrExcludedModules[i].hModule = GetModuleHandle(strModule);
		}
	}
}

void CXTPSkinManagerLegacyApiHook::IncrementAllModulesReferenceCounters(
	CXTPSkinManagerModuleList* pModuleList, BOOL bIncrement /*= TRUE*/)
{
	ASSERT(NULL != pModuleList);

	HMODULE hModule = pModuleList->GetFirstModule();
	while (NULL != hModule)
	{
		if (bIncrement)
		{
			IncrementModuleReferenceCounter(hModule);
		}
		else
		{
			DecrementModuleReferenceCounter(hModule);
		}

		hModule = pModuleList->GetNextModule();
	}
}

void CXTPSkinManagerLegacyApiHook::HackModuleOnLoad(HMODULE hmod, DWORD dwFlags)
{
	XTP_GUARD_SHARED(CXTPSkinManagerLegacyApiHook, this)
	{
		for (int i = 0; i < (int)m_arrExcludedModules.GetSize(); i++)
		{
			EXCLUDEDMODULE& em = m_arrExcludedModules[i];

			if (em.hModule == NULL && em.strModule[em.strModule.GetLength() - 1] != '*')
			{
				em.hModule = GetModuleHandle(em.strModule);
			}
		}

		if (IsModuleExcluded(hmod))
			return;

		if ((hmod != NULL) && ((dwFlags & LOAD_LIBRARY_AS_DATAFILE) == 0))
		{
			for (int i = 0; i < XTP_SKIN_LEGACY_API_HOOK_COUNT; i++)
			{
				CXTPSkinManagerLegacyApiFunction* pHook = m_arrFunctions[i];
				if (pHook)
				{
					XTP_GUARD_EXCLUSIVE(CXTPSkinManagerLegacyApiFunction, pHook)
					{
						if (pHook->ReplaceInOneModule(pHook->m_szCalleeModName, pHook->m_pfnOrig,
													  pHook->m_pfnHook, hmod, pHook->m_pfnAltOrig))
						{
							pHook->m_bHooked = TRUE;
						}
					}
				}
			}
		}
	}
}

CXTPSkinManagerLegacyApiFunction* AFX_CDECL
	CXTPSkinManagerLegacyApiHook::EnterHookedCall(LPCSTR pszCalleeModName, LPCSTR pszFuncName)
{
	CXTPSkinManagerLegacyApiFunction* pFunction = GetInstance()->GetHookedFunction(pszCalleeModName,
																				   pszFuncName);
	if (!(NULL != pFunction && NULL != pFunction->GetOriginal()))
	{
		SetLastError(ERROR_PROC_NOT_FOUND);
		pFunction = NULL;
	}

	return pFunction;
}

CXTPSkinManagerLegacyApiFunction* AFX_CDECL
	CXTPSkinManagerLegacyApiHook::EnterHookedCall(XTPSkinFrameworkLegacyApiFunctionIndex nIndex)
{
	CXTPSkinManagerLegacyApiFunction* pFunction = GetInstance()->GetHookedFunction(nIndex);
	if (!(NULL != pFunction && NULL != pFunction->GetOriginal()))
	{
		SetLastError(ERROR_PROC_NOT_FOUND);
		pFunction = NULL;
	}

	return pFunction;
}

DWORD WINAPI CXTPSkinManagerLegacyApiHook::OnHookGetSysColor(int nIndex)
{
	XTP_SKINFRAMEWORK_MANAGE_STATE();

	DWORD crColor = 0;

	CXTPSkinManagerLegacyApiFunction* pFunction = EnterHookedCall(xtpSkinLegacyApiGetSysColor);
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

BOOL WINAPI CXTPSkinManagerLegacyApiHook::OnHookDeleteObject(HGDIOBJ hObject)
{
	XTP_SKINFRAMEWORK_MANAGE_STATE();

	CXTPSkinManagerLegacyApiFunction* pFunction = EnterHookedCall(xtpSkinLegacyApiDeleteObject);
	if (NULL == pFunction)
	{
		return FALSE;
	}

	if (m_bInitialized && pFunction->IsHooked())
	{
		CXTPSkinManagerMetrics* pMetrics = XTPSkinManager()->GetMetrics();

		if (NULL != pMetrics && pMetrics->IsMetricObject(hObject))
			return FALSE;
	}

	typedef BOOL(WINAPI * LPFNDELETEOBJECT)(HGDIOBJ);
	return ((LPFNDELETEOBJECT)pFunction->GetOriginal())(hObject);
}

HBRUSH WINAPI CXTPSkinManagerLegacyApiHook::OnHookGetSysColorBrush(int nIndex)
{
	XTP_SKINFRAMEWORK_MANAGE_STATE();

	HBRUSH hBrush = NULL;

	CXTPSkinManagerLegacyApiFunction* pFunction = EnterHookedCall(xtpSkinLegacyApiGetSysColorBrush);
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

int WINAPI CXTPSkinManagerLegacyApiHook::OnHookSetScrollPos(HWND hwnd, int nBar, int nPos,
															BOOL redraw)
{
	int nResult = 0;

	XTP_SKINFRAMEWORK_MANAGE_STATE();

	CXTPSkinManagerLegacyApiFunction* pFunction = EnterHookedCall(xtpSkinLegacyApiSetScrollPos);
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

int WINAPI CXTPSkinManagerLegacyApiHook::OnHookSetScrollInfo(HWND hwnd, int nBar,
															 LPCSCROLLINFO lpsi, BOOL redraw)
{
	int nResult = 0;

	XTP_SKINFRAMEWORK_MANAGE_STATE();

	CXTPSkinManagerLegacyApiFunction* pFunction = EnterHookedCall(xtpSkinLegacyApiSetScrollInfo);
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

BOOL WINAPI CXTPSkinManagerLegacyApiHook::OnHookEnableScrollBar(HWND hWnd, UINT wSBflags,
																UINT wArrows)
{
	BOOL bResult = FALSE;

	XTP_SKINFRAMEWORK_MANAGE_STATE();

	CXTPSkinManagerLegacyApiFunction* pFunction = EnterHookedCall(xtpSkinLegacyApiEnableScrollBar);
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

BOOL WINAPI CXTPSkinManagerLegacyApiHook::OnHookGetScrollInfo(HWND hWnd, int nBar,
															  LPSCROLLINFO lpsi)
{
	BOOL bResult = FALSE;

	XTP_SKINFRAMEWORK_MANAGE_STATE();

	CXTPSkinManagerLegacyApiFunction* pFunction = EnterHookedCall(xtpSkinLegacyApiGetScrollInfo);
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

LRESULT WINAPI CXTPSkinManagerLegacyApiHook::OnHookCallWindowProcW(WNDPROC lpPrevWndFunc, HWND hWnd,
																   UINT Msg, WPARAM wParam,
																   LPARAM lParam)
{
	XTP_SKINFRAMEWORK_MANAGE_STATE();

	CXTPSkinManagerLegacyApiFunction* pFunction = EnterHookedCall(xtpSkinLegacyApiCallWindowProcW);
	if (NULL == pFunction)
	{
		return 0;
	}

	if (m_bInitialized && pFunction->IsHooked())
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

LRESULT WINAPI CXTPSkinManagerLegacyApiHook::OnHookCallWindowProcA(WNDPROC lpPrevWndFunc, HWND hWnd,
																   UINT Msg, WPARAM wParam,
																   LPARAM lParam)
{
	XTP_SKINFRAMEWORK_MANAGE_STATE();

	CXTPSkinManagerLegacyApiFunction* pFunction = EnterHookedCall(xtpSkinLegacyApiCallWindowProcA);
	if (NULL == pFunction)
	{
		return 0;
	}

	if (m_bInitialized && pFunction->IsHooked())
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

LRESULT WINAPI CXTPSkinManagerLegacyApiHook::OnHookDefWindowProcA(HWND hWnd, UINT Msg,
																  WPARAM wParam, LPARAM lParam)
{
	XTP_SKINFRAMEWORK_MANAGE_STATE();

	CXTPSkinManagerLegacyApiFunction* pFunction = EnterHookedCall(xtpSkinLegacyApiDefWindowProcA);
	if (NULL == pFunction)
	{
		return 0;
	}

	if (m_bInitialized && pFunction->IsHooked())
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

LRESULT WINAPI CXTPSkinManagerLegacyApiHook::OnHookDefWindowProcW(HWND hWnd, UINT Msg,
																  WPARAM wParam, LPARAM lParam)
{
	XTP_SKINFRAMEWORK_MANAGE_STATE();

	CXTPSkinManagerLegacyApiFunction* pFunction = EnterHookedCall(xtpSkinLegacyApiDefWindowProcW);
	if (NULL == pFunction)
	{
		return 0;
	}

	if (m_bInitialized && pFunction->IsHooked())
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

LRESULT WINAPI CXTPSkinManagerLegacyApiHook::OnHookDefFrameProcA(HWND hWnd, HWND hWndMDIClient,
																 UINT Msg, WPARAM wParam,
																 LPARAM lParam)
{
	XTP_SKINFRAMEWORK_MANAGE_STATE();

	CXTPSkinManagerLegacyApiFunction* pFunction = EnterHookedCall(xtpSkinLegacyApiDefFrameProcA);
	if (NULL == pFunction)
	{
		return 0;
	}

	if (m_bInitialized && pFunction->IsHooked())
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

LRESULT WINAPI CXTPSkinManagerLegacyApiHook::OnHookDefFrameProcW(HWND hWnd, HWND hWndMDIClient,
																 UINT Msg, WPARAM wParam,
																 LPARAM lParam)
{
	XTP_SKINFRAMEWORK_MANAGE_STATE();

	CXTPSkinManagerLegacyApiFunction* pFunction = EnterHookedCall(xtpSkinLegacyApiDefFrameProcW);
	if (NULL == pFunction)
	{
		return 0;
	}

	if (m_bInitialized && pFunction->IsHooked())
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

LRESULT WINAPI CXTPSkinManagerLegacyApiHook::OnHookDefDlgProcA(HWND hWnd, UINT Msg, WPARAM wParam,
															   LPARAM lParam)
{
	XTP_SKINFRAMEWORK_MANAGE_STATE();

	CXTPSkinManagerLegacyApiFunction* pFunction = EnterHookedCall(xtpSkinLegacyApiDefDlgProcA);
	if (NULL == pFunction)
	{
		return 0;
	}

	if (m_bInitialized && pFunction->IsHooked())
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

LRESULT WINAPI CXTPSkinManagerLegacyApiHook::OnHookDefDlgProcW(HWND hWnd, UINT Msg, WPARAM wParam,
															   LPARAM lParam)
{
	XTP_SKINFRAMEWORK_MANAGE_STATE();

	CXTPSkinManagerLegacyApiFunction* pFunction = EnterHookedCall(xtpSkinLegacyApiDefDlgProcW);
	if (NULL == pFunction)
	{
		return 0;
	}

	if (m_bInitialized && pFunction->IsHooked())
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

LRESULT WINAPI CXTPSkinManagerLegacyApiHook::OnHookDefMDIChildProcA(HWND hWnd, UINT Msg,
																	WPARAM wParam, LPARAM lParam)
{
	XTP_SKINFRAMEWORK_MANAGE_STATE();

	CXTPSkinManagerLegacyApiFunction* pFunction = EnterHookedCall(xtpSkinLegacyApiDefMDIChildProcA);
	if (NULL == pFunction)
	{
		return 0;
	}

	if (m_bInitialized && pFunction->IsHooked())
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

LRESULT WINAPI CXTPSkinManagerLegacyApiHook::OnHookDefMDIChildProcW(HWND hWnd, UINT Msg,
																	WPARAM wParam, LPARAM lParam)
{
	XTP_SKINFRAMEWORK_MANAGE_STATE();

	CXTPSkinManagerLegacyApiFunction* pFunction = EnterHookedCall(xtpSkinLegacyApiDefMDIChildProcW);
	if (NULL == pFunction)
	{
		return 0;
	}

	if (m_bInitialized && pFunction->IsHooked())
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

struct XTP_SKINFRAMEWORK_THREADPROCPARAMETER
{
	LPTHREAD_START_ROUTINE lpStartAddress;
	LPVOID lpParameter;
};

DWORD WINAPI CXTPSkinManagerLegacyApiHook::ThreadProcHook(LPVOID lpThreadData)
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

HRESULT WINAPI CXTPSkinManagerLegacyApiHook::OnHookDwmExtendFrameIntoClientArea(HWND hWnd,
																				LPCVOID pMarInset)
{
	HRESULT hr = E_NOTIMPL;

	XTP_SKINFRAMEWORK_MANAGE_STATE();

	CXTPSkinManagerLegacyApiFunction* pFunction = EnterHookedCall(
		xtpSkinLegacyApiDwmExtendFrameIntoClientArea);
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

HANDLE STDAPICALLTYPE CXTPSkinManagerLegacyApiHook::OnHookCreateThread(
	XTPSkinFrameworkLegacyApiFunctionIndex nFunctionIndex, LPSECURITY_ATTRIBUTES lpThreadAttributes,
	UINT_PTR dwStackSize, LPTHREAD_START_ROUTINE lpStartAddress, LPVOID lpParameter,
	DWORD dwCreationFlags, LPDWORD lpThreadId)
{
	XTP_SKINFRAMEWORK_MANAGE_STATE();

	CXTPSkinManagerLegacyApiFunction* pFunction = EnterHookedCall(nFunctionIndex);
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
				pParam->lpParameter	= lpParameter;
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

ATOM WINAPI CXTPSkinManagerLegacyApiHook::OnHookRegisterClassA(const WNDCLASSA* lpWndClass)
{
	XTP_SKINFRAMEWORK_MANAGE_STATE();

	CXTPSkinManagerLegacyApiFunction* pFunction = EnterHookedCall(xtpSkinLegacyApiRegisterClassA);
	if (NULL == pFunction)
		return 0;

	typedef ATOM(WINAPI * LPFNREGISTERCLASSA)(const WNDCLASSA* lpWndClass);

	if (m_bInitialized && pFunction->IsHooked())
	{
		XTP_GUARD_SHARED_(CXTPSkinManagerLegacyApiHook, GetInstance(), pInstance)
		{
			CXTPSkinManagerLegacyApiFunction* pDefWindowFunction = NULL;
			if (lpWndClass
				&& lpWndClass->lpfnWndProc
					   == XTPToFunctionPtr<WNDPROC>(
							  &CXTPSkinManagerLegacyApiHook::OnHookDefWindowProcA))
			{
				pDefWindowFunction = pInstance->GetHookedFunction(xtpSkinLegacyApiDefWindowProcA);
			}
			if (lpWndClass
				&& lpWndClass->lpfnWndProc
					   == XTPToFunctionPtr<WNDPROC>(
							  &CXTPSkinManagerLegacyApiHook::OnHookDefMDIChildProcA))
			{
				pDefWindowFunction = pInstance->GetHookedFunction(xtpSkinLegacyApiDefFrameProcA);
			}
			if (lpWndClass
				&& lpWndClass->lpfnWndProc
					   == XTPToFunctionPtr<WNDPROC>(
							  &CXTPSkinManagerLegacyApiHook::OnHookDefDlgProcA))
			{
				pDefWindowFunction = pInstance->GetHookedFunction(xtpSkinLegacyApiDefDlgProcA);
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

ATOM WINAPI CXTPSkinManagerLegacyApiHook::OnHookRegisterClassW(const WNDCLASSW* lpWndClass)
{
	XTP_SKINFRAMEWORK_MANAGE_STATE();

	CXTPSkinManagerLegacyApiFunction* pFunction = EnterHookedCall(xtpSkinLegacyApiRegisterClassW);
	if (NULL == pFunction)
		return 0;

	typedef ATOM(WINAPI * LPFNREGISTERCLASSW)(const WNDCLASSW* lpWndClass);

	if (m_bInitialized && pFunction->IsHooked())
	{
		XTP_GUARD_SHARED_(CXTPSkinManagerLegacyApiHook, GetInstance(), pInstance)
		{
			CXTPSkinManagerLegacyApiFunction* pDefWindowFunction = NULL;
			if (lpWndClass
				&& lpWndClass->lpfnWndProc
					   == XTPToFunctionPtr<WNDPROC>(
							  &CXTPSkinManagerLegacyApiHook::OnHookDefWindowProcW))
			{
				pDefWindowFunction = pInstance->GetHookedFunction(xtpSkinLegacyApiDefWindowProcW);
			}
			if (lpWndClass
				&& lpWndClass->lpfnWndProc
					   == XTPToFunctionPtr<WNDPROC>(
							  &CXTPSkinManagerLegacyApiHook::OnHookDefMDIChildProcW))
			{
				pDefWindowFunction = pInstance->GetHookedFunction(xtpSkinLegacyApiDefMDIChildProcW);
			}

			if (lpWndClass
				&& lpWndClass->lpfnWndProc
					   == XTPToFunctionPtr<WNDPROC>(
							  &CXTPSkinManagerLegacyApiHook::OnHookDefDlgProcW))
			{
				pDefWindowFunction = pInstance->GetHookedFunction(xtpSkinLegacyApiDefDlgProcW);
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

int WINAPI CXTPSkinManagerLegacyApiHook::OnHookGetSystemMetrics(int nIndex)
{
	XTP_SKINFRAMEWORK_MANAGE_STATE();

	CXTPSkinManagerLegacyApiFunction* pFunction = EnterHookedCall(xtpSkinLegacyApiGetSystemMetrics);
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

BOOL WINAPI CXTPSkinManagerLegacyApiHook::OnHookSystemParametersInfoA(UINT uiAction, UINT uiParam,
																	  PVOID pvParam, UINT fWinIni)
{
	XTP_SKINFRAMEWORK_MANAGE_STATE();

	CXTPSkinManagerLegacyApiFunction* pFunction = EnterHookedCall(
		xtpSkinLegacyApiSystemParametersInfoA);
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

BOOL WINAPI CXTPSkinManagerLegacyApiHook::OnHookSystemParametersInfoW(UINT uiAction, UINT uiParam,
																	  PVOID pvParam, UINT fWinIni)
{
	XTP_SKINFRAMEWORK_MANAGE_STATE();

	CXTPSkinManagerLegacyApiFunction* pFunction = EnterHookedCall(
		xtpSkinLegacyApiSystemParametersInfoW);
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

BOOL WINAPI CXTPSkinManagerLegacyApiHook::OnHookAdjustWindowRectEx(LPRECT lpRect, DWORD dwStyle,
																   BOOL bMenu, DWORD dwExStyle)
{
	XTP_SKINFRAMEWORK_MANAGE_STATE();

	BOOL bResult = FALSE;

	CXTPSkinManagerLegacyApiFunction* pFunction = EnterHookedCall(
		xtpSkinLegacyApiAdjustWindowRectEx);
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

BOOL WINAPI CXTPSkinManagerLegacyApiHook::OnHookAdjustWindowRect(LPRECT lpRect, DWORD dwStyle,
																 BOOL bMenu)
{
	XTP_SKINFRAMEWORK_MANAGE_STATE();

	BOOL bResult = FALSE;

	CXTPSkinManagerLegacyApiFunction* pFunction = EnterHookedCall(xtpSkinLegacyApiAdjustWindowRect);
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

int WINAPI CXTPSkinManagerLegacyApiHook::OnHookFillRect(HDC hDC, CONST RECT* lprc, HBRUSH hbr)
{
	XTP_SKINFRAMEWORK_MANAGE_STATE();

	CXTPSkinManagerLegacyApiFunction* pFunction = EnterHookedCall(xtpSkinLegacyApiFillRect);
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

BOOL WINAPI CXTPSkinManagerLegacyApiHook::OnHookDrawEdge(HDC hdc, LPRECT lprc, UINT edge,
														 UINT flags)
{
	XTP_SKINFRAMEWORK_MANAGE_STATE();

	CXTPSkinManagerLegacyApiFunction* pFunction = EnterHookedCall(xtpSkinLegacyApiDrawEdge);
	if (NULL == pFunction)
		return 0;

	if (pFunction->IsHooked() && XTPSkinManager()->IsEnabled())
		return XTPSkinFrameworkDrawEdge(hdc, lprc, edge, flags);

	typedef BOOL(WINAPI * LPFNDRAWEDGE)(HDC, LPRECT, UINT, UINT);
	return ((LPFNDRAWEDGE)pFunction->GetOriginal())(hdc, lprc, edge, flags);
}

BOOL WINAPI CXTPSkinManagerLegacyApiHook::OnHookDrawFrameControl(HDC hdc, LPRECT lprc, UINT uType,
																 UINT uState)
{
	XTP_SKINFRAMEWORK_MANAGE_STATE();

	CXTPSkinManagerLegacyApiFunction* pFunction = EnterHookedCall(xtpSkinLegacyApiDrawFrameControl);
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

BOOL WINAPI CXTPSkinManagerLegacyApiHook::OnHookDrawMenuBar(HWND hWnd)
{
	XTP_SKINFRAMEWORK_MANAGE_STATE();

	CXTPSkinManagerLegacyApiFunction* pFunction = EnterHookedCall(xtpSkinLegacyApiDrawMenuBar);
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

BOOL WINAPI CXTPSkinManagerLegacyApiHook::OnHookGetMenuItemRect(HWND hWnd, HMENU hMenu, UINT uItem,
																LPRECT lprcItem)
{
	XTP_SKINFRAMEWORK_MANAGE_STATE();

	BOOL bResult = FALSE;

	CXTPSkinManagerLegacyApiFunction* pFunction = EnterHookedCall(xtpSkinLegacyApiGetMenuItemRect);
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

BOOL WINAPI CXTPSkinManagerLegacyApiHook::OnHookTrackPopupMenu(HMENU hMenu, UINT uFlags, int x,
															   int y, int nReserved, HWND hWnd,
															   CONST RECT* prcRect)
{
	XTP_SKINFRAMEWORK_MANAGE_STATE();

	BOOL bResult = FALSE;

	CXTPSkinManagerLegacyApiFunction* pFunction = EnterHookedCall(xtpSkinLegacyApiTrackPopupMenu);
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

BOOL WINAPI CXTPSkinManagerLegacyApiHook::OnHookTrackPopupMenuEx(
	XTPSkinFrameworkLegacyApiFunctionIndex nFunctionIndex, HMENU hMenu, UINT uFlags, int x, int y,
	HWND hWnd, LPTPMPARAMS lptpParams)
{
	XTP_SKINFRAMEWORK_MANAGE_STATE();

	BOOL bResult = FALSE;

	CXTPSkinManagerLegacyApiFunction* pFunction = EnterHookedCall(nFunctionIndex);
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

BOOL WINAPI CXTPSkinManagerLegacyApiHook::OnHookTrackPopupMenuEx_USER32(HMENU hMenu, UINT uFlags,
																		int x, int y, HWND hWnd,
																		LPTPMPARAMS lptpParams)
{
	return OnHookTrackPopupMenuEx(xtpSkinLegacyApiTrackPopupMenuEx_USER32, hMenu, uFlags, x, y,
								  hWnd, lptpParams);
}
BOOL WINAPI CXTPSkinManagerLegacyApiHook::OnHookNtUserTrackPopupMenuEx_WIN32U(
	HMENU hMenu, UINT uFlags, int x, int y, HWND hWnd, LPTPMPARAMS lptpParams)
{
	return OnHookTrackPopupMenuEx(xtpSkinLegacyApiNtUserTrackPopupMenuEx_WIN32U, hMenu, uFlags, x,
								  y, hWnd, lptpParams);
}

HMODULE WINAPI CXTPSkinManagerLegacyApiHook::OnHookLoadLibraryA_KERNEL32(LPCSTR pszModuleName)
{
	return OnHookLoadLibraryA(xtpSkinLegacyApiLoadLibraryA_KERNEL32, pszModuleName);
}
HMODULE WINAPI CXTPSkinManagerLegacyApiHook::OnHookLoadLibraryA_API_MS_WIN_CORE_LIBRARYLOADER_L1_1_0(
	LPCSTR pszModuleName)
{
	return OnHookLoadLibraryA(xtpSkinLegacyApiLoadLibraryA_API_MS_WIN_CORE_LIBRARYLOADER_L1_1_0,
							  pszModuleName);
}
HMODULE WINAPI CXTPSkinManagerLegacyApiHook::OnHookLoadLibraryA_API_MS_WIN_CORE_LIBRARYLOADER_L1_1_1(
	LPCSTR pszModuleName)
{
	return OnHookLoadLibraryA(xtpSkinLegacyApiLoadLibraryA_API_MS_WIN_CORE_LIBRARYLOADER_L1_1_1,
							  pszModuleName);
}
HMODULE WINAPI CXTPSkinManagerLegacyApiHook::OnHookLoadLibraryA_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_0(
	LPCSTR pszModuleName)
{
	return OnHookLoadLibraryA(xtpSkinLegacyApiLoadLibraryA_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_0,
							  pszModuleName);
}
HMODULE WINAPI
	CXTPSkinManagerLegacyApiHook::OnHookLoadLibraryA_API_MS_WIN_CORE_LIBRARYLOADER_PRIVATE_L1_1_0(
		LPCSTR pszModuleName)
{
	return OnHookLoadLibraryA(
		xtpSkinLegacyApiLoadLibraryA_API_MS_WIN_CORE_LIBRARYLOADER_PRIVATE_L1_1_0, pszModuleName);
}
HMODULE WINAPI CXTPSkinManagerLegacyApiHook::OnHookLoadLibraryA_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_1(
	LPCSTR pszModuleName)
{
	return OnHookLoadLibraryA(xtpSkinLegacyApiLoadLibraryA_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_1,
							  pszModuleName);
}
HMODULE WINAPI CXTPSkinManagerLegacyApiHook::OnHookLoadLibraryA_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_2(
	LPCSTR pszModuleName)
{
	return OnHookLoadLibraryA(xtpSkinLegacyApiLoadLibraryA_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_2,
							  pszModuleName);
}
HMODULE WINAPI CXTPSkinManagerLegacyApiHook::OnHookLoadLibraryA_API_MS_WIN_CORE_LIBRARYLOADER_L2_1_0(
	LPCSTR pszModuleName)
{
	return OnHookLoadLibraryA(xtpSkinLegacyApiLoadLibraryA_API_MS_WIN_CORE_LIBRARYLOADER_L2_1_0,
							  pszModuleName);
}
HMODULE WINAPI CXTPSkinManagerLegacyApiHook::OnHookLoadLibraryW_KERNEL32(PCWSTR pszModuleName)
{
	return OnHookLoadLibraryW(xtpSkinLegacyApiLoadLibraryW_KERNEL32, pszModuleName);
}
HMODULE WINAPI CXTPSkinManagerLegacyApiHook::OnHookLoadLibraryW_API_MS_WIN_CORE_LIBRARYLOADER_L1_1_0(
	PCWSTR pszModuleName)
{
	return OnHookLoadLibraryW(xtpSkinLegacyApiLoadLibraryW_API_MS_WIN_CORE_LIBRARYLOADER_L1_1_0,
							  pszModuleName);
}
HMODULE WINAPI CXTPSkinManagerLegacyApiHook::OnHookLoadLibraryW_API_MS_WIN_CORE_LIBRARYLOADER_L1_1_1(
	PCWSTR pszModuleName)
{
	return OnHookLoadLibraryW(xtpSkinLegacyApiLoadLibraryW_API_MS_WIN_CORE_LIBRARYLOADER_L1_1_1,
							  pszModuleName);
}
HMODULE WINAPI CXTPSkinManagerLegacyApiHook::OnHookLoadLibraryW_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_0(
	PCWSTR pszModuleName)
{
	return OnHookLoadLibraryW(xtpSkinLegacyApiLoadLibraryW_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_0,
							  pszModuleName);
}
HMODULE WINAPI
	CXTPSkinManagerLegacyApiHook::OnHookLoadLibraryW_API_MS_WIN_CORE_LIBRARYLOADER_PRIVATE_L1_1_0(
		PCWSTR pszModuleName)
{
	return OnHookLoadLibraryW(
		xtpSkinLegacyApiLoadLibraryW_API_MS_WIN_CORE_LIBRARYLOADER_PRIVATE_L1_1_0, pszModuleName);
}
HMODULE WINAPI CXTPSkinManagerLegacyApiHook::OnHookLoadLibraryW_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_1(
	PCWSTR pszModuleName)
{
	return OnHookLoadLibraryW(xtpSkinLegacyApiLoadLibraryW_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_1,
							  pszModuleName);
}
HMODULE WINAPI CXTPSkinManagerLegacyApiHook::OnHookLoadLibraryW_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_2(
	PCWSTR pszModuleName)
{
	return OnHookLoadLibraryW(xtpSkinLegacyApiLoadLibraryW_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_2,
							  pszModuleName);
}
HMODULE WINAPI CXTPSkinManagerLegacyApiHook::OnHookLoadLibraryW_API_MS_WIN_CORE_LIBRARYLOADER_L2_1_0(
	PCWSTR pszModuleName)
{
	return OnHookLoadLibraryW(xtpSkinLegacyApiLoadLibraryW_API_MS_WIN_CORE_LIBRARYLOADER_L2_1_0,
							  pszModuleName);
}
HMODULE WINAPI CXTPSkinManagerLegacyApiHook::OnHookLoadLibraryExA_KERNEL32(LPCSTR pszModuleName,
																		   HANDLE hFile,
																		   DWORD dwFlags)
{
	return OnHookLoadLibraryExA(xtpSkinLegacyApiLoadLibraryExA_KERNEL32, pszModuleName, hFile,
								dwFlags);
}
HMODULE WINAPI
	CXTPSkinManagerLegacyApiHook::OnHookLoadLibraryExA_API_MS_WIN_CORE_LIBRARYLOADER_L1_1_0(
		LPCSTR pszModuleName, HANDLE hFile, DWORD dwFlags)
{
	return OnHookLoadLibraryExA(xtpSkinLegacyApiLoadLibraryExA_API_MS_WIN_CORE_LIBRARYLOADER_L1_1_0,
								pszModuleName, hFile, dwFlags);
}
HMODULE WINAPI
	CXTPSkinManagerLegacyApiHook::OnHookLoadLibraryExA_API_MS_WIN_CORE_LIBRARYLOADER_L1_1_1(
		LPCSTR pszModuleName, HANDLE hFile, DWORD dwFlags)
{
	return OnHookLoadLibraryExA(xtpSkinLegacyApiLoadLibraryExA_API_MS_WIN_CORE_LIBRARYLOADER_L1_1_1,
								pszModuleName, hFile, dwFlags);
}
HMODULE WINAPI
	CXTPSkinManagerLegacyApiHook::OnHookLoadLibraryExA_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_0(
		LPCSTR pszModuleName, HANDLE hFile, DWORD dwFlags)
{
	return OnHookLoadLibraryExA(xtpSkinLegacyApiLoadLibraryExA_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_0,
								pszModuleName, hFile, dwFlags);
}
HMODULE WINAPI
	CXTPSkinManagerLegacyApiHook::OnHookLoadLibraryExA_API_MS_WIN_CORE_LIBRARYLOADER_PRIVATE_L1_1_0(
		LPCSTR pszModuleName, HANDLE hFile, DWORD dwFlags)
{
	return OnHookLoadLibraryExA(
		xtpSkinLegacyApiLoadLibraryExA_API_MS_WIN_CORE_LIBRARYLOADER_PRIVATE_L1_1_0, pszModuleName,
		hFile, dwFlags);
}
HMODULE WINAPI
	CXTPSkinManagerLegacyApiHook::OnHookLoadLibraryExA_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_1(
		LPCSTR pszModuleName, HANDLE hFile, DWORD dwFlags)
{
	return OnHookLoadLibraryExA(xtpSkinLegacyApiLoadLibraryExA_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_1,
								pszModuleName, hFile, dwFlags);
}
HMODULE WINAPI
	CXTPSkinManagerLegacyApiHook::OnHookLoadLibraryExA_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_2(
		LPCSTR pszModuleName, HANDLE hFile, DWORD dwFlags)
{
	return OnHookLoadLibraryExA(xtpSkinLegacyApiLoadLibraryExA_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_2,
								pszModuleName, hFile, dwFlags);
}
HMODULE WINAPI
	CXTPSkinManagerLegacyApiHook::OnHookLoadLibraryExA_API_MS_WIN_CORE_LIBRARYLOADER_L2_1_0(
		LPCSTR pszModuleName, HANDLE hFile, DWORD dwFlags)
{
	return OnHookLoadLibraryExA(xtpSkinLegacyApiLoadLibraryExA_API_MS_WIN_CORE_LIBRARYLOADER_L2_1_0,
								pszModuleName, hFile, dwFlags);
}
HMODULE WINAPI CXTPSkinManagerLegacyApiHook::OnHookLoadLibraryExW_KERNEL32(PCWSTR pszModuleName,
																		   HANDLE hFile,
																		   DWORD dwFlags)
{
	return OnHookLoadLibraryExW(xtpSkinLegacyApiLoadLibraryExW_KERNEL32, pszModuleName, hFile,
								dwFlags);
}
HMODULE WINAPI
	CXTPSkinManagerLegacyApiHook::OnHookLoadLibraryExW_API_MS_WIN_CORE_LIBRARYLOADER_L1_1_0(
		PCWSTR pszModuleName, HANDLE hFile, DWORD dwFlags)
{
	return OnHookLoadLibraryExW(xtpSkinLegacyApiLoadLibraryExW_API_MS_WIN_CORE_LIBRARYLOADER_L1_1_0,
								pszModuleName, hFile, dwFlags);
}
HMODULE WINAPI
	CXTPSkinManagerLegacyApiHook::OnHookLoadLibraryExW_API_MS_WIN_CORE_LIBRARYLOADER_L1_1_1(
		PCWSTR pszModuleName, HANDLE hFile, DWORD dwFlags)
{
	return OnHookLoadLibraryExW(xtpSkinLegacyApiLoadLibraryExW_API_MS_WIN_CORE_LIBRARYLOADER_L1_1_1,
								pszModuleName, hFile, dwFlags);
}
HMODULE WINAPI
	CXTPSkinManagerLegacyApiHook::OnHookLoadLibraryExW_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_0(
		PCWSTR pszModuleName, HANDLE hFile, DWORD dwFlags)
{
	return OnHookLoadLibraryExW(xtpSkinLegacyApiLoadLibraryExW_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_0,
								pszModuleName, hFile, dwFlags);
}
HMODULE WINAPI
	CXTPSkinManagerLegacyApiHook::OnHookLoadLibraryExW_API_MS_WIN_CORE_LIBRARYLOADER_PRIVATE_L1_1_0(
		PCWSTR pszModuleName, HANDLE hFile, DWORD dwFlags)
{
	return OnHookLoadLibraryExW(
		xtpSkinLegacyApiLoadLibraryExW_API_MS_WIN_CORE_LIBRARYLOADER_PRIVATE_L1_1_0, pszModuleName,
		hFile, dwFlags);
}
HMODULE WINAPI
	CXTPSkinManagerLegacyApiHook::OnHookLoadLibraryExW_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_1(
		PCWSTR pszModuleName, HANDLE hFile, DWORD dwFlags)
{
	return OnHookLoadLibraryExW(xtpSkinLegacyApiLoadLibraryExW_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_1,
								pszModuleName, hFile, dwFlags);
}
HMODULE WINAPI
	CXTPSkinManagerLegacyApiHook::OnHookLoadLibraryExW_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_2(
		PCWSTR pszModuleName, HANDLE hFile, DWORD dwFlags)
{
	return OnHookLoadLibraryExW(xtpSkinLegacyApiLoadLibraryExW_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_2,
								pszModuleName, hFile, dwFlags);
}
HMODULE WINAPI
	CXTPSkinManagerLegacyApiHook::OnHookLoadLibraryExW_API_MS_WIN_CORE_LIBRARYLOADER_L2_1_0(
		PCWSTR pszModuleName, HANDLE hFile, DWORD dwFlags)
{
	return OnHookLoadLibraryExW(xtpSkinLegacyApiLoadLibraryExW_API_MS_WIN_CORE_LIBRARYLOADER_L2_1_0,
								pszModuleName, hFile, dwFlags);
}
BOOL WINAPI CXTPSkinManagerLegacyApiHook::OnHookFreeLibrary_KERNEL32(HMODULE hmod)
{
	return OnHookFreeLibrary(xtpSkinLegacyApiFreeLibrary_KERNEL32, hmod);
}
BOOL WINAPI CXTPSkinManagerLegacyApiHook::OnHookFreeLibrary_API_MS_WIN_CORE_LIBRARYLOADER_L1_1_0(
	HMODULE hmod)
{
	return OnHookFreeLibrary(xtpSkinLegacyApiFreeLibrary_API_MS_WIN_CORE_LIBRARYLOADER_L1_1_0,
							 hmod);
}
BOOL WINAPI CXTPSkinManagerLegacyApiHook::OnHookFreeLibrary_API_MS_WIN_CORE_LIBRARYLOADER_L1_1_1(
	HMODULE hmod)
{
	return OnHookFreeLibrary(xtpSkinLegacyApiFreeLibrary_API_MS_WIN_CORE_LIBRARYLOADER_L1_1_1,
							 hmod);
}
BOOL WINAPI CXTPSkinManagerLegacyApiHook::OnHookFreeLibrary_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_0(
	HMODULE hmod)
{
	return OnHookFreeLibrary(xtpSkinLegacyApiFreeLibrary_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_0,
							 hmod);
}
BOOL WINAPI
	CXTPSkinManagerLegacyApiHook::OnHookFreeLibrary_API_MS_WIN_CORE_LIBRARYLOADER_PRIVATE_L1_1_0(
		HMODULE hmod)
{
	return OnHookFreeLibrary(
		xtpSkinLegacyApiFreeLibrary_API_MS_WIN_CORE_LIBRARYLOADER_PRIVATE_L1_1_0, hmod);
}
BOOL WINAPI CXTPSkinManagerLegacyApiHook::OnHookFreeLibrary_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_1(
	HMODULE hmod)
{
	return OnHookFreeLibrary(xtpSkinLegacyApiFreeLibrary_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_1,
							 hmod);
}
BOOL WINAPI CXTPSkinManagerLegacyApiHook::OnHookFreeLibrary_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_2(
	HMODULE hmod)
{
	return OnHookFreeLibrary(xtpSkinLegacyApiFreeLibrary_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_2,
							 hmod);
}
BOOL WINAPI CXTPSkinManagerLegacyApiHook::OnHookFreeLibrary_API_MS_WIN_CORE_LIBRARYLOADER_L2_1_0(
	HMODULE hmod)
{
	return OnHookFreeLibrary(xtpSkinLegacyApiFreeLibrary_API_MS_WIN_CORE_LIBRARYLOADER_L2_1_0,
							 hmod);
}
BOOL WINAPI CXTPSkinManagerLegacyApiHook::OnHookFreeLibraryAndExitThread_KERNEL32(HMODULE hmod,
																				  DWORD dwExitCode)
{
	return OnHookFreeLibraryAndExitThread(xtpSkinLegacyApiFreeLibraryAndExitThread_KERNEL32, hmod,
										  dwExitCode);
}
BOOL WINAPI CXTPSkinManagerLegacyApiHook::
	OnHookFreeLibraryAndExitThread_API_MS_WIN_CORE_LIBRARYLOADER_L1_1_0(HMODULE hmod,
																		DWORD dwExitCode)
{
	return OnHookFreeLibraryAndExitThread(
		xtpSkinLegacyApiFreeLibraryAndExitThread_API_MS_WIN_CORE_LIBRARYLOADER_L1_1_0, hmod,
		dwExitCode);
}
BOOL WINAPI CXTPSkinManagerLegacyApiHook::
	OnHookFreeLibraryAndExitThread_API_MS_WIN_CORE_LIBRARYLOADER_L1_1_1(HMODULE hmod,
																		DWORD dwExitCode)
{
	return OnHookFreeLibraryAndExitThread(
		xtpSkinLegacyApiFreeLibraryAndExitThread_API_MS_WIN_CORE_LIBRARYLOADER_L1_1_1, hmod,
		dwExitCode);
}
BOOL WINAPI CXTPSkinManagerLegacyApiHook::
	OnHookFreeLibraryAndExitThread_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_0(HMODULE hmod,
																		DWORD dwExitCode)
{
	return OnHookFreeLibraryAndExitThread(
		xtpSkinLegacyApiFreeLibraryAndExitThread_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_0, hmod,
		dwExitCode);
}
BOOL WINAPI CXTPSkinManagerLegacyApiHook::
	OnHookFreeLibraryAndExitThread_API_MS_WIN_CORE_LIBRARYLOADER_PRIVATE_L1_1_0(HMODULE hmod,
																				DWORD dwExitCode)
{
	return OnHookFreeLibraryAndExitThread(
		xtpSkinLegacyApiFreeLibraryAndExitThread_API_MS_WIN_CORE_LIBRARYLOADER_PRIVATE_L1_1_0, hmod,
		dwExitCode);
}
BOOL WINAPI CXTPSkinManagerLegacyApiHook::
	OnHookFreeLibraryAndExitThread_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_1(HMODULE hmod,
																		DWORD dwExitCode)
{
	return OnHookFreeLibraryAndExitThread(
		xtpSkinLegacyApiFreeLibraryAndExitThread_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_1, hmod,
		dwExitCode);
}
BOOL WINAPI CXTPSkinManagerLegacyApiHook::
	OnHookFreeLibraryAndExitThread_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_2(HMODULE hmod,
																		DWORD dwExitCode)
{
	return OnHookFreeLibraryAndExitThread(
		xtpSkinLegacyApiFreeLibraryAndExitThread_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_2, hmod,
		dwExitCode);
}
BOOL WINAPI CXTPSkinManagerLegacyApiHook::
	OnHookFreeLibraryAndExitThread_API_MS_WIN_CORE_LIBRARYLOADER_L2_1_0(HMODULE hmod,
																		DWORD dwExitCode)
{
	return OnHookFreeLibraryAndExitThread(
		xtpSkinLegacyApiFreeLibraryAndExitThread_API_MS_WIN_CORE_LIBRARYLOADER_L2_1_0, hmod,
		dwExitCode);
}
FARPROC WINAPI CXTPSkinManagerLegacyApiHook::OnHookGetProcAddress_KERNEL32(HMODULE hmod,
																		   PCSTR pszProcName)
{
	return OnHookGetProcAddress(xtpSkinLegacyApiGetProcAddress_KERNEL32, hmod, pszProcName);
}
FARPROC WINAPI
	CXTPSkinManagerLegacyApiHook::OnHookGetProcAddress_API_MS_WIN_CORE_LIBRARYLOADER_L1_1_0(
		HMODULE hmod, PCSTR pszProcName)
{
	return OnHookGetProcAddress(xtpSkinLegacyApiGetProcAddress_API_MS_WIN_CORE_LIBRARYLOADER_L1_1_0,
								hmod, pszProcName);
}
FARPROC WINAPI
	CXTPSkinManagerLegacyApiHook::OnHookGetProcAddress_API_MS_WIN_CORE_LIBRARYLOADER_L1_1_1(
		HMODULE hmod, PCSTR pszProcName)
{
	return OnHookGetProcAddress(xtpSkinLegacyApiGetProcAddress_API_MS_WIN_CORE_LIBRARYLOADER_L1_1_1,
								hmod, pszProcName);
}
FARPROC WINAPI
	CXTPSkinManagerLegacyApiHook::OnHookGetProcAddress_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_0(
		HMODULE hmod, PCSTR pszProcName)
{
	return OnHookGetProcAddress(xtpSkinLegacyApiGetProcAddress_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_0,
								hmod, pszProcName);
}
FARPROC WINAPI
	CXTPSkinManagerLegacyApiHook::OnHookGetProcAddress_API_MS_WIN_CORE_LIBRARYLOADER_PRIVATE_L1_1_0(
		HMODULE hmod, PCSTR pszProcName)
{
	return OnHookGetProcAddress(
		xtpSkinLegacyApiGetProcAddress_API_MS_WIN_CORE_LIBRARYLOADER_PRIVATE_L1_1_0, hmod,
		pszProcName);
}
FARPROC WINAPI
	CXTPSkinManagerLegacyApiHook::OnHookGetProcAddress_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_1(
		HMODULE hmod, PCSTR pszProcName)
{
	return OnHookGetProcAddress(xtpSkinLegacyApiGetProcAddress_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_1,
								hmod, pszProcName);
}
FARPROC WINAPI
	CXTPSkinManagerLegacyApiHook::OnHookGetProcAddress_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_2(
		HMODULE hmod, PCSTR pszProcName)
{
	return OnHookGetProcAddress(xtpSkinLegacyApiGetProcAddress_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_2,
								hmod, pszProcName);
}
FARPROC WINAPI
	CXTPSkinManagerLegacyApiHook::OnHookGetProcAddress_API_MS_WIN_CORE_LIBRARYLOADER_L2_1_0(
		HMODULE hmod, PCSTR pszProcName)
{
	return OnHookGetProcAddress(xtpSkinLegacyApiGetProcAddress_API_MS_WIN_CORE_LIBRARYLOADER_L2_1_0,
								hmod, pszProcName);
}
HMODULE WINAPI CXTPSkinManagerLegacyApiHook::OnHookGetModuleHandleA_KERNEL32(LPCSTR lpModuleName)
{
	return OnHookGetModuleHandleA(xtpSkinLegacyApiGetModuleHandleA_KERNEL32, lpModuleName);
}
HMODULE WINAPI
	CXTPSkinManagerLegacyApiHook::OnHookGetModuleHandleA_API_MS_WIN_CORE_LIBRARYLOADER_L1_1_0(
		LPCSTR lpModuleName)
{
	return OnHookGetModuleHandleA(
		xtpSkinLegacyApiGetModuleHandleA_API_MS_WIN_CORE_LIBRARYLOADER_L1_1_0, lpModuleName);
}
HMODULE WINAPI
	CXTPSkinManagerLegacyApiHook::OnHookGetModuleHandleA_API_MS_WIN_CORE_LIBRARYLOADER_L1_1_1(
		LPCSTR lpModuleName)
{
	return OnHookGetModuleHandleA(
		xtpSkinLegacyApiGetModuleHandleA_API_MS_WIN_CORE_LIBRARYLOADER_L1_1_1, lpModuleName);
}
HMODULE WINAPI
	CXTPSkinManagerLegacyApiHook::OnHookGetModuleHandleA_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_0(
		LPCSTR lpModuleName)
{
	return OnHookGetModuleHandleA(
		xtpSkinLegacyApiGetModuleHandleA_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_0, lpModuleName);
}
HMODULE WINAPI CXTPSkinManagerLegacyApiHook::
	OnHookGetModuleHandleA_API_MS_WIN_CORE_LIBRARYLOADER_PRIVATE_L1_1_0(LPCSTR lpModuleName)
{
	return OnHookGetModuleHandleA(
		xtpSkinLegacyApiGetModuleHandleA_API_MS_WIN_CORE_LIBRARYLOADER_PRIVATE_L1_1_0,
		lpModuleName);
}
HMODULE WINAPI
	CXTPSkinManagerLegacyApiHook::OnHookGetModuleHandleA_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_1(
		LPCSTR lpModuleName)
{
	return OnHookGetModuleHandleA(
		xtpSkinLegacyApiGetModuleHandleA_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_1, lpModuleName);
}
HMODULE WINAPI
	CXTPSkinManagerLegacyApiHook::OnHookGetModuleHandleA_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_2(
		LPCSTR lpModuleName)
{
	return OnHookGetModuleHandleA(
		xtpSkinLegacyApiGetModuleHandleA_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_2, lpModuleName);
}
HMODULE WINAPI
	CXTPSkinManagerLegacyApiHook::OnHookGetModuleHandleA_API_MS_WIN_CORE_LIBRARYLOADER_L2_1_0(
		LPCSTR lpModuleName)
{
	return OnHookGetModuleHandleA(
		xtpSkinLegacyApiGetModuleHandleA_API_MS_WIN_CORE_LIBRARYLOADER_L2_1_0, lpModuleName);
}
HMODULE WINAPI CXTPSkinManagerLegacyApiHook::OnHookGetModuleHandleW_KERNEL32(LPCWSTR lpModuleName)
{
	return OnHookGetModuleHandleW(xtpSkinLegacyApiGetModuleHandleW_KERNEL32, lpModuleName);
}
HMODULE WINAPI
	CXTPSkinManagerLegacyApiHook::OnHookGetModuleHandleW_API_MS_WIN_CORE_LIBRARYLOADER_L1_1_0(
		LPCWSTR lpModuleName)
{
	return OnHookGetModuleHandleW(
		xtpSkinLegacyApiGetModuleHandleW_API_MS_WIN_CORE_LIBRARYLOADER_L1_1_0, lpModuleName);
}
HMODULE WINAPI
	CXTPSkinManagerLegacyApiHook::OnHookGetModuleHandleW_API_MS_WIN_CORE_LIBRARYLOADER_L1_1_1(
		LPCWSTR lpModuleName)
{
	return OnHookGetModuleHandleW(
		xtpSkinLegacyApiGetModuleHandleW_API_MS_WIN_CORE_LIBRARYLOADER_L1_1_1, lpModuleName);
}
HMODULE WINAPI
	CXTPSkinManagerLegacyApiHook::OnHookGetModuleHandleW_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_0(
		LPCWSTR lpModuleName)
{
	return OnHookGetModuleHandleW(
		xtpSkinLegacyApiGetModuleHandleW_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_0, lpModuleName);
}
HMODULE WINAPI CXTPSkinManagerLegacyApiHook::
	OnHookGetModuleHandleW_API_MS_WIN_CORE_LIBRARYLOADER_PRIVATE_L1_1_0(LPCWSTR lpModuleName)
{
	return OnHookGetModuleHandleW(
		xtpSkinLegacyApiGetModuleHandleW_API_MS_WIN_CORE_LIBRARYLOADER_PRIVATE_L1_1_0,
		lpModuleName);
}
HMODULE WINAPI
	CXTPSkinManagerLegacyApiHook::OnHookGetModuleHandleW_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_1(
		LPCWSTR lpModuleName)
{
	return OnHookGetModuleHandleW(
		xtpSkinLegacyApiGetModuleHandleW_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_1, lpModuleName);
}
HMODULE WINAPI
	CXTPSkinManagerLegacyApiHook::OnHookGetModuleHandleW_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_2(
		LPCWSTR lpModuleName)
{
	return OnHookGetModuleHandleW(
		xtpSkinLegacyApiGetModuleHandleW_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_2, lpModuleName);
}
HMODULE WINAPI
	CXTPSkinManagerLegacyApiHook::OnHookGetModuleHandleW_API_MS_WIN_CORE_LIBRARYLOADER_L2_1_0(
		LPCWSTR lpModuleName)
{
	return OnHookGetModuleHandleW(
		xtpSkinLegacyApiGetModuleHandleW_API_MS_WIN_CORE_LIBRARYLOADER_L2_1_0, lpModuleName);
}
HMODULE WINAPI CXTPSkinManagerLegacyApiHook::OnHookGetModuleHandleExA_KERNEL32(DWORD dwFlags,
																			   LPCSTR lpModuleName,
																			   HMODULE* phModule)
{
	return OnHookGetModuleHandleExA(xtpSkinLegacyApiGetModuleHandleExA_KERNEL32, dwFlags,
									lpModuleName, phModule);
}
HMODULE WINAPI
	CXTPSkinManagerLegacyApiHook::OnHookGetModuleHandleExA_API_MS_WIN_CORE_LIBRARYLOADER_L1_1_0(
		DWORD dwFlags, LPCSTR lpModuleName, HMODULE* phModule)
{
	return OnHookGetModuleHandleExA(
		xtpSkinLegacyApiGetModuleHandleExA_API_MS_WIN_CORE_LIBRARYLOADER_L1_1_0, dwFlags,
		lpModuleName, phModule);
}
HMODULE WINAPI
	CXTPSkinManagerLegacyApiHook::OnHookGetModuleHandleExA_API_MS_WIN_CORE_LIBRARYLOADER_L1_1_1(
		DWORD dwFlags, LPCSTR lpModuleName, HMODULE* phModule)
{
	return OnHookGetModuleHandleExA(
		xtpSkinLegacyApiGetModuleHandleExA_API_MS_WIN_CORE_LIBRARYLOADER_L1_1_1, dwFlags,
		lpModuleName, phModule);
}
HMODULE WINAPI
	CXTPSkinManagerLegacyApiHook::OnHookGetModuleHandleExA_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_0(
		DWORD dwFlags, LPCSTR lpModuleName, HMODULE* phModule)
{
	return OnHookGetModuleHandleExA(
		xtpSkinLegacyApiGetModuleHandleExA_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_0, dwFlags,
		lpModuleName, phModule);
}
HMODULE WINAPI CXTPSkinManagerLegacyApiHook::
	OnHookGetModuleHandleExA_API_MS_WIN_CORE_LIBRARYLOADER_PRIVATE_L1_1_0(DWORD dwFlags,
																		  LPCSTR lpModuleName,
																		  HMODULE* phModule)
{
	return OnHookGetModuleHandleExA(
		xtpSkinLegacyApiGetModuleHandleExA_API_MS_WIN_CORE_LIBRARYLOADER_PRIVATE_L1_1_0, dwFlags,
		lpModuleName, phModule);
}
HMODULE WINAPI
	CXTPSkinManagerLegacyApiHook::OnHookGetModuleHandleExA_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_1(
		DWORD dwFlags, LPCSTR lpModuleName, HMODULE* phModule)
{
	return OnHookGetModuleHandleExA(
		xtpSkinLegacyApiGetModuleHandleExA_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_1, dwFlags,
		lpModuleName, phModule);
}
HMODULE WINAPI
	CXTPSkinManagerLegacyApiHook::OnHookGetModuleHandleExA_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_2(
		DWORD dwFlags, LPCSTR lpModuleName, HMODULE* phModule)
{
	return OnHookGetModuleHandleExA(
		xtpSkinLegacyApiGetModuleHandleExA_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_2, dwFlags,
		lpModuleName, phModule);
}
HMODULE WINAPI
	CXTPSkinManagerLegacyApiHook::OnHookGetModuleHandleExA_API_MS_WIN_CORE_LIBRARYLOADER_L2_1_0(
		DWORD dwFlags, LPCSTR lpModuleName, HMODULE* phModule)
{
	return OnHookGetModuleHandleExA(
		xtpSkinLegacyApiGetModuleHandleExA_API_MS_WIN_CORE_LIBRARYLOADER_L2_1_0, dwFlags,
		lpModuleName, phModule);
}
HMODULE WINAPI CXTPSkinManagerLegacyApiHook::OnHookGetModuleHandleExW_KERNEL32(DWORD dwFlags,
																			   LPCWSTR lpModuleName,
																			   HMODULE* phModule)
{
	return OnHookGetModuleHandleExW(xtpSkinLegacyApiGetModuleHandleExW_KERNEL32, dwFlags,
									lpModuleName, phModule);
}
HMODULE WINAPI
	CXTPSkinManagerLegacyApiHook::OnHookGetModuleHandleExW_API_MS_WIN_CORE_LIBRARYLOADER_L1_1_0(
		DWORD dwFlags, LPCWSTR lpModuleName, HMODULE* phModule)
{
	return OnHookGetModuleHandleExW(
		xtpSkinLegacyApiGetModuleHandleExW_API_MS_WIN_CORE_LIBRARYLOADER_L1_1_0, dwFlags,
		lpModuleName, phModule);
}
HMODULE WINAPI
	CXTPSkinManagerLegacyApiHook::OnHookGetModuleHandleExW_API_MS_WIN_CORE_LIBRARYLOADER_L1_1_1(
		DWORD dwFlags, LPCWSTR lpModuleName, HMODULE* phModule)
{
	return OnHookGetModuleHandleExW(
		xtpSkinLegacyApiGetModuleHandleExW_API_MS_WIN_CORE_LIBRARYLOADER_L1_1_1, dwFlags,
		lpModuleName, phModule);
}
HMODULE WINAPI
	CXTPSkinManagerLegacyApiHook::OnHookGetModuleHandleExW_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_0(
		DWORD dwFlags, LPCWSTR lpModuleName, HMODULE* phModule)
{
	return OnHookGetModuleHandleExW(
		xtpSkinLegacyApiGetModuleHandleExW_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_0, dwFlags,
		lpModuleName, phModule);
}
HMODULE WINAPI CXTPSkinManagerLegacyApiHook::
	OnHookGetModuleHandleExW_API_MS_WIN_CORE_LIBRARYLOADER_PRIVATE_L1_1_0(DWORD dwFlags,
																		  LPCWSTR lpModuleName,
																		  HMODULE* phModule)
{
	return OnHookGetModuleHandleExW(
		xtpSkinLegacyApiGetModuleHandleExW_API_MS_WIN_CORE_LIBRARYLOADER_PRIVATE_L1_1_0, dwFlags,
		lpModuleName, phModule);
}
HMODULE WINAPI
	CXTPSkinManagerLegacyApiHook::OnHookGetModuleHandleExW_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_1(
		DWORD dwFlags, LPCWSTR lpModuleName, HMODULE* phModule)
{
	return OnHookGetModuleHandleExW(
		xtpSkinLegacyApiGetModuleHandleExW_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_1, dwFlags,
		lpModuleName, phModule);
}
HMODULE WINAPI
	CXTPSkinManagerLegacyApiHook::OnHookGetModuleHandleExW_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_2(
		DWORD dwFlags, LPCWSTR lpModuleName, HMODULE* phModule)
{
	return OnHookGetModuleHandleExW(
		xtpSkinLegacyApiGetModuleHandleExW_API_MS_WIN_CORE_LIBRARYLOADER_L1_2_2, dwFlags,
		lpModuleName, phModule);
}
HMODULE WINAPI
	CXTPSkinManagerLegacyApiHook::OnHookGetModuleHandleExW_API_MS_WIN_CORE_LIBRARYLOADER_L2_1_0(
		DWORD dwFlags, LPCWSTR lpModuleName, HMODULE* phModule)
{
	return OnHookGetModuleHandleExW(
		xtpSkinLegacyApiGetModuleHandleExW_API_MS_WIN_CORE_LIBRARYLOADER_L2_1_0, dwFlags,
		lpModuleName, phModule);
}
HANDLE WINAPI CXTPSkinManagerLegacyApiHook::OnHookCreateThread_KERNEL32(
	LPSECURITY_ATTRIBUTES lpThreadAttributes, UINT_PTR dwStackSize,
	LPTHREAD_START_ROUTINE lpStartAddress, LPVOID lpParameter, DWORD dwCreationFlags,
	LPDWORD lpThreadId)
{
	return OnHookCreateThread(xtpSkinLegacyApiCreateThread_KERNEL32, lpThreadAttributes,
							  dwStackSize, lpStartAddress, lpParameter, dwCreationFlags,
							  lpThreadId);
}
HANDLE WINAPI CXTPSkinManagerLegacyApiHook::OnHookCreateThread_API_MS_WIN_CORE_PROCESSTHREADS_L1_1_1(
	LPSECURITY_ATTRIBUTES lpThreadAttributes, UINT_PTR dwStackSize,
	LPTHREAD_START_ROUTINE lpStartAddress, LPVOID lpParameter, DWORD dwCreationFlags,
	LPDWORD lpThreadId)
{
	return OnHookCreateThread(xtpSkinLegacyApiCreateThread_API_MS_WIN_CORE_PROCESSTHREADS_L1_1_1,
							  lpThreadAttributes, dwStackSize, lpStartAddress, lpParameter,
							  dwCreationFlags, lpThreadId);
}
HANDLE WINAPI CXTPSkinManagerLegacyApiHook::OnHookCreateThread_API_MS_WIN_CORE_PROCESSTHREADS_L1_1_2(
	LPSECURITY_ATTRIBUTES lpThreadAttributes, UINT_PTR dwStackSize,
	LPTHREAD_START_ROUTINE lpStartAddress, LPVOID lpParameter, DWORD dwCreationFlags,
	LPDWORD lpThreadId)
{
	return OnHookCreateThread(xtpSkinLegacyApiCreateThread_API_MS_WIN_CORE_PROCESSTHREADS_L1_1_2,
							  lpThreadAttributes, dwStackSize, lpStartAddress, lpParameter,
							  dwCreationFlags, lpThreadId);
}

HMODULE WINAPI CXTPSkinManagerLegacyApiHook::OnHookLoadLibraryA(
	XTPSkinFrameworkLegacyApiFunctionIndex nFunctionIndex, LPCSTR pszModuleName)
{
	XTP_SKINFRAMEWORK_MANAGE_STATE();

	HMODULE hModule = NULL;

	CXTPSkinManagerLegacyApiFunction* pFunction = EnterHookedCall(nFunctionIndex);
	if (NULL != pFunction)
	{
		int* pnHackModuleOnLoadCounter = NULL;
		try
		{
			CXTPSkinManagerLegacyApiHook* pApiHook = GetInstance();
			pnHackModuleOnLoadCounter			   = &*pApiHook->m_HackModuleOnLoadCounter;
			++*pnHackModuleOnLoadCounter;

			typedef HMODULE(WINAPI * LPFNLOADLIBRARYA)(LPCSTR pszModuleName);
			hModule = ((LPFNLOADLIBRARYA)pFunction->GetOriginal())(pszModuleName);

			if (NULL != hModule && !XTP_LDR_IS_RESOURCE(hModule) && 1 == *pnHackModuleOnLoadCounter)
			{
				if (1 == pApiHook->IncrementModuleReferenceCounter(hModule))
				{
					pApiHook->HackModuleOnLoad(hModule, 0);
				}
			}

			--*pnHackModuleOnLoadCounter;
		}
		catch (...)
		{
			if (NULL != pnHackModuleOnLoadCounter)
				--*pnHackModuleOnLoadCounter;
			throw;
		}
	}

	return hModule;
}

HMODULE WINAPI CXTPSkinManagerLegacyApiHook::OnHookLoadLibraryW(
	XTPSkinFrameworkLegacyApiFunctionIndex nFunctionIndex, PCWSTR pszModuleName)
{
	XTP_SKINFRAMEWORK_MANAGE_STATE();

	HMODULE hModule = NULL;

	CXTPSkinManagerLegacyApiFunction* pFunction = EnterHookedCall(nFunctionIndex);
	if (NULL != pFunction)
	{
		int* pnHackModuleOnLoadCounter = NULL;
		try
		{
			CXTPSkinManagerLegacyApiHook* pApiHook = GetInstance();
			pnHackModuleOnLoadCounter			   = &*pApiHook->m_HackModuleOnLoadCounter;
			++*pnHackModuleOnLoadCounter;

			typedef HMODULE(WINAPI * LPFNLOADLIBRARYW)(PCWSTR pszModuleName);
			hModule = ((LPFNLOADLIBRARYW)pFunction->GetOriginal())(pszModuleName);

			if (NULL != hModule && !XTP_LDR_IS_RESOURCE(hModule) && 1 == *pnHackModuleOnLoadCounter)
			{
				if (1 == pApiHook->IncrementModuleReferenceCounter(hModule))
				{
					pApiHook->HackModuleOnLoad(hModule, 0);
				}
			}

			--*pnHackModuleOnLoadCounter;
		}
		catch (...)
		{
			if (NULL != pnHackModuleOnLoadCounter)
				--*pnHackModuleOnLoadCounter;
			throw;
		}
	}

	return hModule;
}

HMODULE WINAPI CXTPSkinManagerLegacyApiHook::OnHookLoadLibraryExA(
	XTPSkinFrameworkLegacyApiFunctionIndex nFunctionIndex, LPCSTR pszModuleName, HANDLE hFile,
	DWORD dwFlags)
{
	XTP_SKINFRAMEWORK_MANAGE_STATE();

	HMODULE hModule = NULL;

	CXTPSkinManagerLegacyApiFunction* pFunction = EnterHookedCall(nFunctionIndex);
	if (NULL != pFunction)
	{
		int* pnHackModuleOnLoadCounter = NULL;
		try
		{
			CXTPSkinManagerLegacyApiHook* pApiHook = GetInstance();
			pnHackModuleOnLoadCounter			   = &*pApiHook->m_HackModuleOnLoadCounter;
			++*pnHackModuleOnLoadCounter;

			typedef HMODULE(WINAPI * LPFNLOADLIBRARYEXA)(LPCSTR pszModuleName, HANDLE hFile,
														 DWORD dwFlags);
			hModule = ((LPFNLOADLIBRARYEXA)pFunction->GetOriginal())(pszModuleName, hFile, dwFlags);

			if (NULL != hModule && !XTP_LDR_IS_RESOURCE(hModule) && 1 == *pnHackModuleOnLoadCounter)
			{
				if (1 == pApiHook->IncrementModuleReferenceCounter(hModule))
				{
					pApiHook->HackModuleOnLoad(hModule, 0);
				}
			}

			--*pnHackModuleOnLoadCounter;
		}
		catch (...)
		{
			if (NULL != pnHackModuleOnLoadCounter)
				--*pnHackModuleOnLoadCounter;
			throw;
		}
	}

	return hModule;
}

HMODULE WINAPI CXTPSkinManagerLegacyApiHook::OnHookLoadLibraryExW(
	XTPSkinFrameworkLegacyApiFunctionIndex nFunctionIndex, PCWSTR pszModuleName, HANDLE hFile,
	DWORD dwFlags)
{
	XTP_SKINFRAMEWORK_MANAGE_STATE();

	HMODULE hModule = NULL;

	CXTPSkinManagerLegacyApiFunction* pFunction = EnterHookedCall(nFunctionIndex);
	if (NULL != pFunction)
	{
		int* pnHackModuleOnLoadCounter = NULL;
		try
		{
			CXTPSkinManagerLegacyApiHook* pApiHook = GetInstance();
			pnHackModuleOnLoadCounter			   = &*pApiHook->m_HackModuleOnLoadCounter;
			++*pnHackModuleOnLoadCounter;

			typedef HMODULE(WINAPI * LPFNLOADLIBRARYEXW)(PCWSTR pszModuleName, HANDLE hFile,
														 DWORD dwFlags);
			hModule = ((LPFNLOADLIBRARYEXW)pFunction->GetOriginal())(pszModuleName, hFile, dwFlags);

			if (NULL != hModule && !XTP_LDR_IS_RESOURCE(hModule) && 1 == *pnHackModuleOnLoadCounter)
			{
				if (1 == pApiHook->IncrementModuleReferenceCounter(hModule))
				{
					pApiHook->HackModuleOnLoad(hModule, 0);
				}
			}

			--*pnHackModuleOnLoadCounter;
		}
		catch (...)
		{
			if (NULL != pnHackModuleOnLoadCounter)
				--*pnHackModuleOnLoadCounter;
			throw;
		}
	}

	return hModule;
}

BOOL WINAPI CXTPSkinManagerLegacyApiHook::OnHookFreeLibrary(
	XTPSkinFrameworkLegacyApiFunctionIndex nFunctionIndex, HMODULE hModule)
{
	XTP_SKINFRAMEWORK_MANAGE_STATE();

	BOOL bResult = FALSE;

	CXTPSkinManagerLegacyApiFunction* pFunction = EnterHookedCall(nFunctionIndex);
	if (NULL != pFunction)
	{
		if (NULL != hModule && !XTP_LDR_IS_RESOURCE(hModule))
		{
			CXTPSkinManagerLegacyApiHook* pApiHook = GetInstance();
			if (0 == pApiHook->DecrementModuleReferenceCounter(hModule))
			{
				pApiHook->UnhookAllFunctions(hModule);
			}
		}

		typedef BOOL(WINAPI * LPFNFREELIBRARY)(HMODULE);
		bResult = ((LPFNFREELIBRARY)pFunction->GetOriginal())(hModule);
	}

	return bResult;
}

BOOL WINAPI CXTPSkinManagerLegacyApiHook::OnHookFreeLibraryAndExitThread(
	XTPSkinFrameworkLegacyApiFunctionIndex nFunctionIndex, HMODULE hModule, DWORD dwExitCode)
{
	XTP_SKINFRAMEWORK_MANAGE_STATE();

	BOOL bResult = FALSE;

	CXTPSkinManagerLegacyApiFunction* pFunction = EnterHookedCall(nFunctionIndex);
	if (NULL != pFunction)
	{
		if (NULL != hModule && !XTP_LDR_IS_RESOURCE(hModule))
		{
			CXTPSkinManagerLegacyApiHook* pApiHook = GetInstance();
			if (0 == pApiHook->DecrementModuleReferenceCounter(hModule))
			{
				pApiHook->UnhookAllFunctions(hModule);
			}
		}

		typedef BOOL(WINAPI * LPFNFREELIBRARYANDEXITTHREAD)(HMODULE, DWORD);
		bResult = ((LPFNFREELIBRARYANDEXITTHREAD)pFunction->GetOriginal())(hModule, dwExitCode);
	}

	return bResult;
}

HMODULE WINAPI CXTPSkinManagerLegacyApiHook::OnHookGetModuleHandleA(
	XTPSkinFrameworkLegacyApiFunctionIndex nFunctionIndex, LPCSTR lpModuleName)
{
	XTP_SKINFRAMEWORK_MANAGE_STATE();

	CXTPSkinManagerLegacyApiFunction* pFunction = EnterHookedCall(nFunctionIndex);
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

HMODULE WINAPI CXTPSkinManagerLegacyApiHook::OnHookGetModuleHandleW(
	XTPSkinFrameworkLegacyApiFunctionIndex nFunctionIndex, LPCWSTR lpModuleName)
{
	XTP_SKINFRAMEWORK_MANAGE_STATE();

	CXTPSkinManagerLegacyApiFunction* pFunction = EnterHookedCall(nFunctionIndex);
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

HMODULE WINAPI CXTPSkinManagerLegacyApiHook::OnHookGetModuleHandleExA(
	XTPSkinFrameworkLegacyApiFunctionIndex nFunctionIndex, DWORD dwFlags, LPCSTR lpModuleName,
	HMODULE* phModule)
{
	XTP_SKINFRAMEWORK_MANAGE_STATE();

	CXTPSkinManagerLegacyApiFunction* pFunction = EnterHookedCall(nFunctionIndex);
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

HMODULE WINAPI CXTPSkinManagerLegacyApiHook::OnHookGetModuleHandleExW(
	XTPSkinFrameworkLegacyApiFunctionIndex nFunctionIndex, DWORD dwFlags, LPCWSTR lpModuleName,
	HMODULE* phModule)
{
	XTP_SKINFRAMEWORK_MANAGE_STATE();

	CXTPSkinManagerLegacyApiFunction* pFunction = EnterHookedCall(nFunctionIndex);
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

FARPROC WINAPI CXTPSkinManagerLegacyApiHook::OnHookGetProcAddress(
	XTPSkinFrameworkLegacyApiFunctionIndex nFunctionIndex, HMODULE hModule, PCSTR pszProcName)
{
	UNREFERENCED_PARAMETER(nFunctionIndex);

	XTP_SKINFRAMEWORK_MANAGE_STATE();

	if (m_bInitializationInProgress || (DWORD_PTR)pszProcName < 0xFFFF)
		return GetProcAddressWindows(hModule, pszProcName);

	CXTPSkinManagerLegacyApiFunction* pFuncHook = NULL;
	if (XTP_UXTHEME_HANDLE == hModule)
	{
		pFuncHook = EnterHookedCall("UXTHEME.DLL", pszProcName);
	}
	else
	{
		char szFullFileName[MAX_PATH];
		if (0 != ::GetModuleFileNameA(hModule, szFullFileName, MAX_PATH))
		{
			char* szFileName = ::PathFindFileNameA(szFullFileName);

			if (XTPCompareStringNoCase(szFileName, "UxTheme.dll") == 0)
			{
				return GetProcAddressWindows(hModule, pszProcName);
			}

			pFuncHook = EnterHookedCall(szFileName, pszProcName);
		}
	}

	if (pFuncHook)
	{
		return pFuncHook->m_pfnHook;
	}

	return GetProcAddressWindows(hModule, pszProcName);
}

HTHEME STDAPICALLTYPE CXTPSkinManagerLegacyApiHook::OnHookOpenThemeData(HWND hWnd,
																		LPCWSTR pszClassList)
{
	XTP_SKINFRAMEWORK_MANAGE_STATE();

	CXTPSkinManagerLegacyApiFunction* pFunction = EnterHookedCall(xtpSkinLegacyApiOpenThemeData);
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

HTHEME STDAPICALLTYPE CXTPSkinManagerLegacyApiHook::OnHookOpenThemeDataForDpi(HWND hWnd,
																			  LPCWSTR pszClassList,
																			  UINT dpi)
{
	XTP_SKINFRAMEWORK_MANAGE_STATE();

	CXTPSkinManagerLegacyApiFunction* pFunction = EnterHookedCall(
		xtpSkinLegacyApiOpenThemeDataForDpi);
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

HTHEME STDAPICALLTYPE CXTPSkinManagerLegacyApiHook::OnHookOpenThemeDataEx(HWND hWnd,
																		  LPCWSTR pszClassList,
																		  DWORD dwFlags)
{
	XTP_SKINFRAMEWORK_MANAGE_STATE();

	CXTPSkinManagerLegacyApiFunction* pFunction = EnterHookedCall(xtpSkinLegacyApiOpenThemeDataEx);
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

HRESULT STDAPICALLTYPE CXTPSkinManagerLegacyApiHook::OnHookCloseThemeData(HTHEME hTheme)
{
	XTP_SKINFRAMEWORK_MANAGE_STATE();

	CXTPSkinManagerLegacyApiFunction* pFunction = EnterHookedCall(xtpSkinLegacyApiCloseThemeData);
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

HRESULT STDAPICALLTYPE CXTPSkinManagerLegacyApiHook::OnHookDrawThemeBackground(
	HTHEME hTheme, HDC hDC, int iPartId, int iStateId, const RECT* pRect, const RECT* pClipRect)
{
	XTP_SKINFRAMEWORK_MANAGE_STATE();

	CXTPSkinManagerLegacyApiFunction* pFunction = EnterHookedCall(
		xtpSkinLegacyApiDrawThemeBackground);
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

HRESULT STDAPICALLTYPE CXTPSkinManagerLegacyApiHook::OnHookGetThemeColor(HTHEME hTheme, int iPartId,
																		 int iStateId, int iPropID,
																		 COLORREF* pColor)
{
	XTP_SKINFRAMEWORK_MANAGE_STATE();

	CXTPSkinManagerLegacyApiFunction* pFunction = EnterHookedCall(xtpSkinLegacyApiGetThemeColor);
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

HRESULT STDAPICALLTYPE CXTPSkinManagerLegacyApiHook::OnHookGetThemeInt(HTHEME hTheme, int iPartId,
																	   int iStateId, int iPropID,
																	   int* piVal)
{
	XTP_SKINFRAMEWORK_MANAGE_STATE();

	CXTPSkinManagerLegacyApiFunction* pFunction = EnterHookedCall(xtpSkinLegacyApiGetThemeInt);
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

BOOL STDAPICALLTYPE CXTPSkinManagerLegacyApiHook::OnHookIsThemeActive()
{
	BOOL bResult = FALSE;

	XTP_SKINFRAMEWORK_MANAGE_STATE();

	CXTPSkinManagerLegacyApiFunction* pFunction = EnterHookedCall(xtpSkinLegacyApiIsThemeActive);
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

BOOL STDAPICALLTYPE CXTPSkinManagerLegacyApiHook::OnHookIsAppThemed()
{
	BOOL bResult = FALSE;

	XTP_SKINFRAMEWORK_MANAGE_STATE();

	CXTPSkinManagerLegacyApiFunction* pFunction = EnterHookedCall(xtpSkinLegacyApiIsAppThemed);
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

HRESULT STDAPICALLTYPE CXTPSkinManagerLegacyApiHook::OnHookGetThemePartSize(
	HTHEME hTheme, HDC hDC, int iPartId, int iStateId, RECT* pRect, THEMESIZE eSize, SIZE* pSize)
{
	XTP_SKINFRAMEWORK_MANAGE_STATE();

	CXTPSkinManagerLegacyApiFunction* pFunction = EnterHookedCall(xtpSkinLegacyApiGetThemePartSize);
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

BOOL STDAPICALLTYPE CXTPSkinManagerLegacyApiHook::OnHookGetThemeSysBool(HTHEME hTheme, int iBoolId)
{
	if (NULL == hTheme)
		return FALSE;

	XTP_SKINFRAMEWORK_MANAGE_STATE();

	CXTPSkinManagerLegacyApiFunction* pFunction = EnterHookedCall(xtpSkinLegacyApiGetThemeSysBool);
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

COLORREF STDAPICALLTYPE CXTPSkinManagerLegacyApiHook::OnHookGetThemeSysColor(HTHEME hTheme,
																			 int iColorId)
{
	XTP_SKINFRAMEWORK_MANAGE_STATE();

	CXTPSkinManagerLegacyApiFunction* pFunction = EnterHookedCall(xtpSkinLegacyApiGetThemeSysColor);
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

HRESULT STDAPICALLTYPE CXTPSkinManagerLegacyApiHook::OnHookGetCurrentThemeName(
	LPWSTR pszThemeFileName, int dwMaxNameChars, LPWSTR pszColorBuff, int cchMaxColorChars,
	LPWSTR pszSizeBuff, int cchMaxSizeChars)
{
	HRESULT hResult = E_FAIL;

	XTP_SKINFRAMEWORK_MANAGE_STATE();

	CXTPSkinManagerLegacyApiFunction* pFunction = EnterHookedCall(
		xtpSkinLegacyApiGetCurrentThemeName);
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
							TCHAR szFileName[MAX_PATH + 1];
							CString strThemeFileName;
							if (GetModuleFileName(NULL, szFileName, MAX_PATH) >= 0)
								strThemeFileName = szFileName;
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

LONG CXTPSkinManagerLegacyApiHook::IncrementModuleReferenceCounter(HMODULE hModule)
{
	LONG lRefCount = -1;

	if (!XTPSkinManager()->IsPersistentModule(hModule))
	{
		XTP_GUARD_EXCLUSIVE(CXTPSkinManagerLegacyApiHook, this)
		{
			if (m_mapModuleRefCount.Lookup(hModule, lRefCount))
			{
				++lRefCount;
				m_mapModuleRefCount[hModule] = lRefCount;
			}
			else
			{
				lRefCount					 = 1;
				m_mapModuleRefCount[hModule] = lRefCount;
			}
		}
	}
	else
	{
		// An imported module is always loaded
		lRefCount = 1;
	}

	return lRefCount;
}

LONG CXTPSkinManagerLegacyApiHook::DecrementModuleReferenceCounter(HMODULE hModule)
{
	LONG lRefCount = -1;

	if (!XTPSkinManager()->IsPersistentModule(hModule))
	{
		XTP_GUARD_EXCLUSIVE(CXTPSkinManagerLegacyApiHook, this)
		{
			if (m_mapModuleRefCount.Lookup(hModule, lRefCount))
			{
				if (0 == --lRefCount)
				{
					m_mapModuleRefCount.RemoveKey(hModule);
				}
				else
				{
					m_mapModuleRefCount[hModule] = lRefCount;
				}
			}
		}
	}
	else
	{
		// An imported module is always loaded
		lRefCount = 1;
	}

	return lRefCount;
}

#endif // XTP_SKINFRAMEWORK_USE_LEGACY_API_HOOK
