// XTPSkinManagerModuleList.h: interface for the CXTPSkinManagerModuleList class.
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
#if !defined(__XTPSKINMANAGERMODULELIST_H__)
#	define __XTPSKINMANAGERMODULELIST_H__
//}}AFX_CODEJOCK_PRIVATE

#	if _MSC_VER > 1000
#		pragma once
#	endif // _MSC_VER > 1000

#	include "Common/Base/Diagnostic/XTPDisableAdvancedWarnings.h"
#	include <stddef.h>
#	include "Common/Base/Diagnostic/XTPEnableAdvancedWarnings.h"

#	include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

//{{AFX_CODEJOCK_PRIVATE

struct XTP_MODULEINFO
{
	LPVOID lpBaseOfDll;
	DWORD SizeOfImage;
	LPVOID EntryPoint;
	HMODULE hModule;
	UINT nLockCount;
	BOOL bUnacceptable;
};

class CXTPSkinManagerModuleListSharedData
{
	friend class CXTPSingleton<CXTPSkinManagerModuleListSharedData>;

private:
	CXTPSkinManagerModuleListSharedData();

public:
	~CXTPSkinManagerModuleListSharedData();

	const CXTPModuleHandle& GetPsapiDllHandle() const;
	BOOL Exists() const;

private:
	void Init();

	CXTPModuleHandle m_hPsapiDll;
	BOOL m_bExists;
	BOOL m_bInitialized;
};

#	define XTP_WIN32SDK_MODULEINFO_SIZE offsetof(XTP_MODULEINFO, hModule)

class _XTP_EXT_CLASS CXTPSkinManagerModuleList
{
	class CModuleEnumerator;
	class CPsapiModuleEnumerator;
	class CToolHelpModuleEnumerator;

public:
	CXTPSkinManagerModuleList(DWORD dwProcessId, BOOL bUseCache = FALSE);
	virtual ~CXTPSkinManagerModuleList();

public:
	_XTP_DEPRECATED_IN_FAVOR(CXTPSkinManagerModuleListSharedData)
	typedef CXTPSkinManagerModuleListSharedData CSharedData;

	static BOOL AFX_CDECL DoesEnumeratorExist();
	static CXTPSkinManagerModuleListSharedData& AFX_CDECL GetSharedData();

public:
	void PreLoad(BOOL bThreadSafe);

	HMODULE GetFirstModule();
	HMODULE GetNextModule();

	BOOL GetModuleInformation(HMODULE hModule, XTP_MODULEINFO* lpmodinfo);

protected:
	CModuleEnumerator* m_pEnumerator;
};

//}}AFX_CODEJOCK_PRIVATE

#	include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPSKINMANAGERMODULELIST_H__)
