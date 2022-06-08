// XTPCustomHeap.cpp : Implementation of custom heap related classes.
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

#include "Common/XTPCasting.h"
#include "Common/XTPCustomHeap.h"
#include "Common/XTPVC80Helpers.h"

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

#ifdef _DEBUG
#	define new DEBUG_NEW
#	undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BOOL AFX_CDECL XTPHeapSetCompatibilityInformation(HANDLE hHeapHandle, PVOID pHeapInformation,
												  ULONG_PTR nHeapInformationLength)
{
#ifdef _WIN32_WCE
	return TRUE; // Not supported;
#else
	typedef BOOL(WINAPI
				 * PFNHEAPSETINFORMATION)(HANDLE HeapHandle, XTP_HEAP_INFORMATION_CLASS eHICls,
										  PVOID HeapInformation, ULONG_PTR HeapInformationLength);

	HMODULE hKernel = ::GetModuleHandle(_T("KERNEL32.DLL"));
	ASSERT(NULL != hKernel);

	if (NULL != hKernel)
	{
		PFNHEAPSETINFORMATION pfHeapSetInformation = XTPToFunctionPtr<PFNHEAPSETINFORMATION>(
			::GetProcAddress(hKernel, "HeapSetInformation"));
		if (NULL != pfHeapSetInformation)
		{
			return pfHeapSetInformation(hHeapHandle, xtpHeapCompatibilityInformation,
										pHeapInformation, nHeapInformationLength);
		}
	}
	return TRUE; // Not supported;
#endif
}

BOOL AFX_CDECL XTPHeapSetLowFragmentation(HANDLE hHeapHandle)
{
	// LFH mode does not enabled under debugger,
	// only under 'clear' run.
	ULONG uHI = 2;
	return XTPHeapSetCompatibilityInformation(hHeapHandle, &uHI, sizeof(uHI));
}

///////////////////////////////////////////////////////////////////////////////
// CXTPHeapString

CXTPHeapString::CXTPHeapString()
{
	m_nStrLen	 = 0;
	m_pcszString = NULL;
}

CXTPHeapString::CXTPHeapString(LPCTSTR pcszString)
{
	m_nStrLen	 = 0;
	m_pcszString = NULL;

	SetString(pcszString);
}

CXTPHeapString::~CXTPHeapString()
{
	_FreeStringData();
}

BOOL CXTPHeapString::IsEmpty() const
{
	return !m_pcszString || m_nStrLen == 0;
}

LPCTSTR CXTPHeapString::operator=(LPCTSTR pcszString)
{
	SetString(pcszString);
	return m_pcszString ? m_pcszString : _T("");
}

BOOL CXTPHeapString::operator==(LPCTSTR pcszString) const
{
	return 0 == _tcscmp((LPCTSTR) * this, pcszString);
}

BOOL CXTPHeapString::operator!=(LPCTSTR pcszString) const
{
	return 0 != _tcscmp((LPCTSTR) * this, pcszString);
}

CXTPHeapString::operator LPCTSTR() const
{
	return m_pcszString ? m_pcszString : _T("");
}

CXTPHeapString::operator CString() const
{
	return CString(m_pcszString ? m_pcszString : _T(""));
}

BSTR CXTPHeapString::AllocSysString() const
{
	CString str(m_pcszString ? m_pcszString : _T(""));
	return str.AllocSysString();
}

void CXTPHeapString::SetString(LPCTSTR pcszString)
{
	if (!pcszString || *pcszString == 0)
	{
		_FreeStringData();
		return;
	}

	int nNewStrLen = (int)_tcslen(pcszString);

	if (nNewStrLen <= m_nStrLen && m_pcszString)
	{
		STRCPY_S(m_pcszString, XTPToSizeTChecked(m_nStrLen + 1), pcszString);
	}
	else
	{
		_FreeStringData();

		_AllocStringData(nNewStrLen);

		if (m_pcszString)
			STRCPY_S(m_pcszString, XTPToSizeTChecked(m_nStrLen + 1), pcszString);
	}
}

void CXTPHeapString::_AllocStringData(int nNewStrLen)
{
	ASSERT(m_pcszString == NULL);

	m_nStrLen	 = nNewStrLen;
	m_pcszString = new TCHAR[XTPToUIntChecked(nNewStrLen + 1)];
}

void CXTPHeapString::_FreeStringData()
{
	if (m_pcszString)
		delete m_pcszString;

	m_pcszString = NULL;
	m_nStrLen	 = 0;
}
