// XTPMarkupXtpSource.cpp: implementation of the CXTPMarkupXtpScript class.
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

#include "StdAfx.h"

#include "Common/XTPTypeId.h"
#include "Common/XTPCasting.h"
#include "Common/XTPFramework.h"
#include "Common/XTPSystemHelpers.h"
#include "Common/XTPSynchro.h"
#include "Common/XTPApplication.h"
#include "Common/XTPSingleton.h"
#include "Common/XTPResourceManager.h"

#include "Markup/XTPMarkupObject.h"
#include "Markup/XTPMarkupString.h"
#include "Markup/XTPMarkupContext.h"
#include "Markup/Extensions/XTPMarkupExtension.h"
#include "Markup/XTPMarkupBuilder.h"
#include "Markup/Extensions/XTPMarkupXtpScript.h"

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

#ifdef _DEBUG
#	undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#	define new DEBUG_NEW
#endif

CXTPMarkupDependencyProperty* CXTPMarkupXtpScript::m_pPropertyLanguage = NULL;
CXTPMarkupDependencyProperty* CXTPMarkupXtpScript::m_pPropertySource   = NULL;

IMPLEMENT_MARKUPCLASS(L"xtp:Script", CXTPMarkupXtpScript, CXTPMarkupExtension);

void CXTPMarkupXtpScript::RegisterMarkupClass()
{
	m_pPropertyLanguage = CXTPMarkupDependencyProperty::Register(L"Language",
																 MARKUP_TYPE(CXTPMarkupString),
																 MARKUP_TYPE(CXTPMarkupXtpScript));

	m_pPropertySource = CXTPMarkupDependencyProperty::Register(L"Source",
															   MARKUP_TYPE(CXTPMarkupString),
															   MARKUP_TYPE(CXTPMarkupXtpScript));
}

LPCWSTR CXTPMarkupXtpScript::GetLanguage() const
{
	CXTPMarkupString* pLanguage = MARKUP_STATICCAST(CXTPMarkupString,
													GetValue(m_pPropertyLanguage));
	LPCWSTR lpszLanguage		= (NULL != pLanguage ? (LPCWSTR)*pLanguage : L"JScript");
	return lpszLanguage;
}

LPCWSTR CXTPMarkupXtpScript::GetSource() const
{
	CXTPMarkupString* pSource = MARKUP_STATICCAST(CXTPMarkupString, GetValue(m_pPropertySource));
	LPCWSTR lpszSource		  = NULL;
	if (NULL != pSource)
	{
		lpszSource = (LPCWSTR)*pSource;
	}

	return lpszSource;
}

void CXTPMarkupXtpScript::SetContentObject(CXTPMarkupBuilder* pBuilder, CXTPMarkupObject* pContent)
{
	ASSERT(NULL != pBuilder);
	ASSERT(NULL != pContent);

	if (IsStringObject(pContent))
	{
		if (NULL == GetSource())
		{
			CXTPMarkupString* pCode = static_cast<CXTPMarkupString*>(pContent);
			m_pMarkupContext->LoadScriptCodeChunk(pBuilder, GetLanguage(), (LPCWSTR)*pCode);
		}
		else
		{
			pBuilder->ThrowBuilderException(
				_T("xtp:Script does not allow combining script source and content"));
		}
	}
	else
	{
		CXTPMarkupExtension::SetContentObject(pBuilder, pContent);
	}
}

void CXTPMarkupXtpScript::OnLoaded(CXTPMarkupBuilder* pBuilder)
{
	ASSERT(NULL != pBuilder);

	LPCWSTR lpszLangauge = GetLanguage();
	LPCWSTR lpszSource   = GetSource();
	if (NULL != lpszSource)
	{
		CArray<WCHAR, WCHAR> data;

		XTP_URI uri;
		HRESULT hr = XTPParseUri(XTP_CW2CT(lpszSource), &uri);
		if (SUCCEEDED(hr))
		{
			switch (uri.type)
			{
				case xtpUriFile: hr = LoadFileContents(uri.file, &data); break;
				case xtpUriRes:
					hr = LoadResourceContents(uri.res.szFilePath, uri.res.lpName, uri.res.lpType,
											  &data);
					break;
				default: ASSERT(!"Unexpected URI type"); break;
			}
		}

		if (SUCCEEDED(hr))
		{
			if (0 < data.GetSize())
			{
				m_pMarkupContext->LoadScriptCodeChunk(pBuilder, lpszLangauge, data.GetData());
			}
		}
		else
		{
			pBuilder->ThrowBuilderException(hr);
		}
	}
}

HRESULT CXTPMarkupXtpScript::LoadFileContents(LPCTSTR lpszPath, CArray<WCHAR, WCHAR>* pData) const
{
	ASSERT(NULL != lpszPath);
	ASSERT(NULL != pData);

	HRESULT hr = S_OK;

	HANDLE hFile	 = INVALID_HANDLE_VALUE;
	HANDLE hMapping  = NULL;
	LPVOID pFileData = NULL;
	do
	{
		hFile = CreateFile(lpszPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
						   FILE_ATTRIBUTE_NORMAL, NULL);
		if (INVALID_HANDLE_VALUE == hFile)
		{
			hr = HRESULT_FROM_WIN32(GetLastError());
			break;
		}

		ULARGE_INTEGER cbFileSize;
		ZeroMemory(&cbFileSize, sizeof(cbFileSize));

		cbFileSize.LowPart = GetFileSize(hFile, &cbFileSize.HighPart);
		if (0 != cbFileSize.HighPart)
		{
			hr = HRESULT_FROM_WIN32(ERROR_BUFFER_OVERFLOW);
			break;
		}

		hMapping = CreateFileMapping(hFile, NULL, PAGE_READONLY, cbFileSize.HighPart,
									 cbFileSize.LowPart, NULL);
		if (NULL == hMapping)
		{
			hr = HRESULT_FROM_WIN32(GetLastError());
			break;
		}

		pFileData = MapViewOfFile(hMapping, FILE_MAP_READ, 0, 0, 0);
		if (NULL == pFileData)
		{
			hr = HRESULT_FROM_WIN32(GetLastError());
			break;
		}

		hr = LoadCodeContent(pFileData, cbFileSize.LowPart, pData);
	} while (FALSE);

	if (NULL != pFileData)
	{
		UnmapViewOfFile(pFileData);
	}

	if (NULL != hMapping)
	{
		CloseHandle(hMapping);
	}

	if (INVALID_HANDLE_VALUE != hFile)
	{
		CloseHandle(hFile);
	}

	return hr;
}

HRESULT CXTPMarkupXtpScript::LoadResourceContents(LPCTSTR lpszModule, LPCTSTR lpName,
												  LPCTSTR lpType, CArray<WCHAR, WCHAR>* pData) const
{
	ASSERT(NULL != lpszModule);
	ASSERT(NULL != lpName);
	ASSERT(NULL != lpType);
	ASSERT(NULL != pData);

	HRESULT hr = S_OK;

	HMODULE hModule = NULL;

	do
	{
		// Load resource data.
		DWORD dwLoadFlags = LOAD_LIBRARY_AS_DATAFILE;
#ifdef LOAD_LIBRARY_AS_IMAGE_RESOURCE
		dwLoadFlags |= LOAD_LIBRARY_AS_IMAGE_RESOURCE;
#endif
		hModule = LoadLibraryEx(lpszModule, NULL, dwLoadFlags);
		if (NULL == hModule)
		{
			hr = HRESULT_FROM_WIN32(GetLastError());
			break;
		}

		HRSRC hRes = FindResource(hModule, lpName, lpType);
		if (NULL == hRes)
		{
			hr = HRESULT_FROM_WIN32(GetLastError());
			break;
		}

		HGLOBAL hGlob = LoadResource(hModule, hRes);
		if (NULL == hGlob)
		{
			hr = HRESULT_FROM_WIN32(GetLastError());
			break;
		}

		LPVOID lpData = LockResource(hGlob);
		if (NULL == lpData)
		{
			hr = HRESULT_FROM_WIN32(GetLastError());
			break;
		}

		DWORD dwSize = SizeofResource(hModule, hRes);
		hr			 = LoadCodeContent(lpData, dwSize, pData);
	} while (FALSE);

	if (NULL != hModule)
	{
		FreeLibrary(hModule);
	}

	return hr;
}

HRESULT CXTPMarkupXtpScript::LoadCodeContent(const void* pFileData, UINT cbFileData,
											 CArray<WCHAR, WCHAR>* pData) const
{
	ASSERT(NULL != pFileData);
	ASSERT(NULL != pData);

	HRESULT hr = S_OK;

	pData->RemoveAll();
	if (0 < cbFileData)
	{
		if (cbFileData < INT_MAX)
		{
			UINT ccBuffer = cbFileData + 1;
			pData->SetSize(XTPToIntPtrChecked(ccBuffer));
			if (!XTPLoadUnicodeText(pFileData, cbFileData, pData->GetData(), &ccBuffer))
			{
				hr = HRESULT_FROM_WIN32(ERROR_INVALID_DATA);
			}
		}
		else
		{
			hr = HRESULT_FROM_WIN32(ERROR_BUFFER_OVERFLOW);
		}
	}

	return hr;
}
