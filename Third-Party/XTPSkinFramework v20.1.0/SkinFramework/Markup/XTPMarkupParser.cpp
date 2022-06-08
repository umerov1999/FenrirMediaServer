// XTPMarkupParser.cpp: implementation of the CXTPMarkupParser class.
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

#include "Common/XTPTypeId.h"
#include "Common/XTPCasting.h"
#include "Common/XTPSystemHelpers.h"
#include "Common/XTPSynchro.h"
#include "Common/XTPApplication.h"
#include "Common/XTPSingleton.h"
#include "Common/XTPXMLHelpers.h"
#include "Common/XTPResourceManager.h"

#include "Markup/XTPMarkupParser.h"

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

#ifdef _DEBUG
#	undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#	define new DEBUG_NEW
#endif

const BYTE CXTPMarkupParser::UnicodeBOM[2] = { 0xff, 0xfe };

inline int IsSpace(WCHAR ch)
{
	return iswspace(ch);
}

inline int IsSpace(CHAR ch)
{
	return isspace(static_cast<unsigned char>(ch));
}

template<class Char>
static Char* SkipSpaces(Char* lpBegin, Char* lpEnd)
{
	ASSERT(NULL != lpBegin);
	ASSERT(lpBegin <= lpEnd);

	while (lpBegin < lpEnd && IsSpace(*lpBegin))
	{
		++lpBegin;
	}

	return lpBegin;
}

template<class Char>
static Char* SkipBom(Char* lpBegin, Char* lpEnd)
{
	ASSERT(NULL != lpBegin);
	ASSERT(lpBegin <= lpEnd);

	// All BOM's are assumed to have every byte value larger than 127 (0x7f)

	const BYTE* pbBegin = reinterpret_cast<const BYTE*>(lpBegin);
	const BYTE* pbEnd	= reinterpret_cast<const BYTE*>(lpEnd);
	const BYTE* pb		= pbBegin;

	while (pb != pbEnd && 0x7F < *pb)
	{
		++pb;
	}

	if (pb != pbBegin && 0 == (static_cast<UINT_PTR>(pb - pbBegin) % sizeof(Char)))
	{
		lpBegin = reinterpret_cast<Char*>(const_cast<BYTE*>(pb));
	}

	return lpBegin;
}

template<class Char>
static BOOL LooksLikeXml(const Char* lpBegin, const Char* lpEnd)
{
	lpBegin = SkipBom(lpBegin, lpEnd);
	lpBegin = SkipSpaces(lpBegin, lpEnd);

	// The very first character in XML, not including spaces, may be only '<'.
	BOOL bLikeXml = (lpBegin < lpEnd && Char('<') == *lpBegin);
	return bLikeXml;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CXTPMarkupParser::CXTPMarkupParser()
	: m_pComInitializer(new CXTPComInitializer)
	, m_bInFixReferenceToUndeclaredNamespacePrefixX(FALSE)
{
	ResetErrorInfo();
}

CXTPMarkupParser::~CXTPMarkupParser()
{
	ReleaseXml();
	SAFE_DELETE(m_pComInitializer);
}

BOOL CXTPMarkupParser::SetBuffer(LPCSTR lpszStart, LPCSTR lpszEnd)
{
	BOOL bSuccess = FALSE;

	if (LooksLikeXml(lpszStart, lpszEnd))
	{
		IStream* pStream = NULL;
		if (CreateDataStream(lpszStart, XTPToSizeTChecked(lpszEnd - lpszStart), &pStream))
		{
			bSuccess = SetDataStream(pStream);
			pStream->Release();
		}
	}

	return bSuccess;
}

BOOL CXTPMarkupParser::SetBuffer(LPCWSTR lpszStart, LPCWSTR lpszEnd)
{
	BOOL bSuccess = FALSE;

	if (LooksLikeXml(lpszStart, lpszEnd))
	{
		IStream* pStream = NULL;
		if (CreateDataStream(lpszStart, XTPToSizeTChecked(lpszEnd - lpszStart), &pStream))
		{
			bSuccess = SetDataStream(pStream);
			pStream->Release();
		}
	}

	return bSuccess;
}

BOOL CXTPMarkupParser::SetDataStream(IStream* pStream)
{
	ASSERT(NULL != pStream);

	BOOL bSuccess = FALSE;

	ReleaseXml();
	ResetErrorInfo();

	CXTPDOMDocumentPtr xmlDocument = XTPCreateXmlDocumentInstance();
	if (NULL != xmlDocument)
	{
		xmlDocument->put_async(VARIANT_FALSE);
		xmlDocument->put_preserveWhiteSpace(-1 /*VARIANT_TRUE*/); // produces warning in VC6
		xmlDocument->put_validateOnParse(0 /*VARIANT_FALSE*/);	  // produces warning in VC6
		xmlDocument->put_resolveExternals(0 /*VARIANT_FALSE*/);	  // produces warning in VC6

		VARIANT vtSource;
		vtSource.vt			   = VT_UNKNOWN;
		vtSource.punkVal	   = pStream;
		VARIANT_BOOL vfSuccess = VARIANT_FALSE;
		HRESULT hr			   = xmlDocument->raw_load(vtSource, &vfSuccess);
		if (SUCCEEDED(hr) && VARIANT_FALSE != vfSuccess)
		{
			m_pXmlDocument = xmlDocument;
			bSuccess	   = TRUE;
		}
		else
		{
			ObtainXmlErrorInfo(xmlDocument, hr);
		}
	}
	else
	{
		ObtainComErrorInfo(CLASS_E_CLASSNOTAVAILABLE);
	}

#ifdef _DEBUG
	if (!bSuccess)
	{
		OutputDebugString(FormatErrorMessage() + _T("\n"));
	}
#endif

	// Add errorenous behavior for backward compatibility.
	if (!bSuccess && !m_bInFixReferenceToUndeclaredNamespacePrefixX)
	{
		m_bInFixReferenceToUndeclaredNamespacePrefixX = TRUE;

		IStream* pFixedStream = NULL;
		if (FixReferenceToUndeclaredNamespacePrefixX(pStream, &pFixedStream))
		{
			bSuccess = SetDataStream(pFixedStream);
			pFixedStream->Release();
		}

		m_bInFixReferenceToUndeclaredNamespacePrefixX = FALSE;
	}

	return bSuccess;
}

static LPWSTR StripNewLines(LPCWSTR lpszText)
{
	ASSERT(NULL != lpszText);

	size_t nLength			= wcslen(lpszText);
	LPWSTR lpszStrippedText = new WCHAR[nLength + 1];
	LPCWSTR lpInCh			= lpszText;
	LPWSTR lpOutCh			= lpszStrippedText;
	do
	{
		if (L'\r' != *lpInCh && L'\n' != *lpInCh)
		{
			*lpOutCh++ = *lpInCh;
		}
	} while (L'\0' != *lpInCh++);
	return lpszStrippedText;
}

CString CXTPMarkupParser::FormatErrorMessage() const
{
	CString strErrorMessage;

	if (FAILED(m_errorInfo.nCode))
	{
		if (m_errorInfo.bXml)
		{
			LPWSTR lpszReason = NULL;
			if (0 < m_errorInfo.strReason.length())
			{
				lpszReason = StripNewLines(m_errorInfo.strReason);
			}

			LPWSTR lpszSourceLine = NULL;
			if (0 < m_errorInfo.strSource.length())
			{
				lpszSourceLine = StripNewLines(m_errorInfo.strSource);
			}

#ifdef _UNICODE
			strErrorMessage.Format(_T("XML Error [0x%08X] '%ls' at %i:%i '%ls'"), m_errorInfo.nCode,
								   NULL != lpszReason ? lpszReason : _T("{reason unknown}"),
								   m_errorInfo.nLine, m_errorInfo.nPosition,
								   NULL != lpszSourceLine ? lpszSourceLine
														  : _T("{source unknown}"));
#else
			strErrorMessage.Format(_T("XML Error [0x%08X] '%s' at %i:%i '%s'"), m_errorInfo.nCode,
								   NULL != lpszReason ? XTP_CW2CA(lpszReason) : "{reason unknown}",
								   m_errorInfo.nLine, m_errorInfo.nPosition,
								   NULL != lpszSourceLine ? XTP_CW2CA(lpszSourceLine)
														  : "{source unknown}");
#endif

			if (NULL != lpszSourceLine)
				delete[] lpszSourceLine;
			if (NULL != lpszReason)
				delete[] lpszReason;
		}
		else
		{
#ifdef _UNICODE
			strErrorMessage.Format(_T("%ls"), (LPCWSTR)m_errorInfo.strReason);
#else
			strErrorMessage.Format(_T("%s"), XTP_CW2CA(m_errorInfo.strReason));
#endif
		}
	}
	else
	{
		strErrorMessage = _T("No Error");
	}

	return strErrorMessage;
}

CString CXTPMarkupParser::FormatMarkupErrorMessage() const
{
	CString strMarkupErrorMessage;

	if (FAILED(m_errorInfo.nCode))
	{
		if (m_errorInfo.bXml)
		{
			CXTPDOMDocumentPtr xmlDoc = XTPCreateXmlDocumentInstance();
			if (NULL != xmlDoc)
			{
				VARIANT_BOOL vtSuccess;
				if (SUCCEEDED(xmlDoc->raw_loadXML(_bstr_t(L"<TextBlock>"
														  L"<Run></Run> "
														  L"<Hyperlink></Hyperlink> "
														  L"<Run></Run>"
														  L"</TextBlock>"),
												  &vtSuccess))
					&& VARIANT_FALSE != vtSuccess)
				{
					LPWSTR lpszReason = NULL;
					if (0 < m_errorInfo.strReason.length())
					{
						lpszReason = StripNewLines(m_errorInfo.strReason);
					}

					LPWSTR lpszSourceLine = NULL;
					if (0 < m_errorInfo.strSource.length())
					{
						lpszSourceLine = StripNewLines(m_errorInfo.strSource);
					}

					CString str;

					CString strMessage;
					strMessage.Format(_T("XML Error [0x%08X] '%ls'"), m_errorInfo.nCode,
									  NULL != lpszReason ? lpszReason : L"{unknown reason}");
					XTPXML::IXMLDOMNodePtr xmlNode = xmlDoc->documentElement->firstChild;
					xmlNode->text				   = (LPCTSTR)strMessage;

					xmlNode = xmlNode->nextSibling;

					CString strLocation;
					strLocation.Format(_T("at %u:%u"), m_errorInfo.nLine, m_errorInfo.nPosition);
					xmlNode->text = (LPCTSTR)strLocation;

					xmlNode = xmlNode->nextSibling;

					CString strSourceLine;
					if (NULL != lpszSourceLine)
					{
						strSourceLine.Format(_T("'%ls'"), lpszSourceLine);
					}

					xmlNode->text = (!strSourceLine.IsEmpty() ? (LPCTSTR)strSourceLine
															  : _T("{unknown source}"));

					if (NULL != lpszSourceLine)
						delete[] lpszSourceLine;
					if (NULL != lpszReason)
						delete[] lpszReason;

					strMarkupErrorMessage = (LPCTSTR)xmlDoc->xml;
				}
			}
		}
		else
		{
			strMarkupErrorMessage.Format(_T("%ls"), (LPCWSTR)m_errorInfo.strReason);
		}
	}
	else
	{
		strMarkupErrorMessage = _T("No Error");
	}

	return strMarkupErrorMessage;
}

BOOL CXTPMarkupParser::CreateDataStream(LPCSTR lpString, SIZE_T cch, IStream** ppStream) const
{
	const MEMORY_BLOB dataBlob = { cch, lpString };
	return CreateDataStream(&dataBlob, 1, ppStream);
}

BOOL CXTPMarkupParser::CreateDataStream(LPCWSTR lpString, SIZE_T cch, IStream** ppStream) const
{
	const MEMORY_BLOB dataBlobs[] = { { sizeof(UnicodeBOM), UnicodeBOM }, // Unicode lead bytes
									  { cch * sizeof(WCHAR), lpString } };

	return CreateDataStream(dataBlobs, _countof(dataBlobs), ppStream);
}

BOOL CXTPMarkupParser::CreateDataStream(const MEMORY_BLOB* pBlobs, UINT nBlobs,
										IStream** ppStream) const
{
	ASSERT(NULL != pBlobs);
	ASSERT(0 < nBlobs);
	ASSERT(NULL != ppStream);

	BOOL bSuccess = FALSE;

	SIZE_T cbTotal = 0;
	UINT nBlob;
	for (nBlob = 0; nBlob < nBlobs; ++nBlob)
	{
		cbTotal += pBlobs[nBlob].cbSize;
	}

	if (0 < cbTotal)
	{
		HGLOBAL hMem = GlobalAlloc(GMEM_FIXED, cbTotal);
		if (NULL != hMem)
		{
			PBYTE pMem = reinterpret_cast<PBYTE>(GlobalLock(hMem));
			if (NULL != pMem)
			{
				for (nBlob = 0; nBlob < nBlobs; ++nBlob)
				{
					if (0 < pBlobs[nBlob].cbSize)
					{
						CopyMemory(pMem, pBlobs[nBlob].pBlobData, pBlobs[nBlob].cbSize);
						pMem += pBlobs[nBlob].cbSize;
					}
				}

				GlobalUnlock(hMem);

				HRESULT hr;
				if (SUCCEEDED(hr = CreateStreamOnHGlobal(hMem, TRUE, ppStream)))
				{
					bSuccess = TRUE;
				}
				else
				{
					GlobalFree(hMem);
					ObtainComErrorInfo(hr);
				}
			}
		}
		else
		{
			ObtainComErrorInfo(HRESULT_FROM_WIN32(GetLastError()));
		}
	}

	return bSuccess;
}

void CXTPMarkupParser::ReleaseXml()
{
	m_pXmlDocument = NULL;
}

void CXTPMarkupParser::ObtainComErrorInfo(HRESULT hr) const
{
	m_errorInfo.nCode	  = hr;
	m_errorInfo.bXml	  = FALSE;
	m_errorInfo.nLine	  = 0;
	m_errorInfo.nPosition = 0;
	m_errorInfo.strSource = _bstr_t();
	m_errorInfo.strReason = _com_error(hr).ErrorMessage();
}

void CXTPMarkupParser::ObtainXmlErrorInfo(XTPXML::IXMLDOMDocument* pXmlDocument,
										  HRESULT hr /*= S_OK*/)
{
	ASSERT(NULL != pXmlDocument);

	HRESULT hrLoc;
	XTPXML::IXMLDOMParseErrorPtr pParseError;
	if (SUCCEEDED(hrLoc = pXmlDocument->get_parseError(&pParseError)))
	{
		if (pParseError)
		{
			m_errorInfo.nCode = pParseError->errorCode;
			if (FAILED(m_errorInfo.nCode) || S_OK == hr)
			{
				m_errorInfo.bXml	  = TRUE;
				m_errorInfo.nLine	  = pParseError->line;
				m_errorInfo.nPosition = pParseError->linepos;
				m_errorInfo.strReason = pParseError->reason;
				m_errorInfo.strSource = pParseError->srcText;

				TRACE3("Markup loading failed (%i:%i): '%ls'", m_errorInfo.nLine,
					   m_errorInfo.nPosition, (LPCWSTR)m_errorInfo.strReason);
			}
			else
			{
				ObtainComErrorInfo(hr);
			}
		}
	}
	else
	{
		ObtainComErrorInfo(hrLoc);
	}
}

void CXTPMarkupParser::ResetErrorInfo()
{
	m_errorInfo.nCode	  = S_OK;
	m_errorInfo.bXml	  = FALSE;
	m_errorInfo.nLine	  = 0;
	m_errorInfo.nPosition = 0;
	m_errorInfo.strSource = _bstr_t();
	m_errorInfo.strReason = _bstr_t();
}

HGLOBAL CXTPMarkupParser::ReadStreamData(IStream* pStream) const
{
	ASSERT(NULL != pStream);

	HGLOBAL hData = NULL;

	STATSTG streamStat;
	if (SUCCEEDED(pStream->Stat(&streamStat, STATFLAG_NONAME)) && 0 == streamStat.cbSize.HighPart)
	{
		hData = GlobalAlloc(GPTR, streamStat.cbSize.LowPart);
		if (NULL != hData)
		{
			static const LARGE_INTEGER startSeekPos = { 0 };
			pStream->Seek(startSeekPos, STREAM_SEEK_SET, NULL);

			LPVOID lpData = GlobalLock(hData);
			ULONG cbRead  = 0;
			if (SUCCEEDED(pStream->Read(lpData, streamStat.cbSize.LowPart, &cbRead)))
			{
				GlobalUnlock(hData);
			}
			else
			{
				GlobalUnlock(hData);
				GlobalFree(hData);
				hData = NULL;
			}
		}
	}

	return hData;
}

LPWSTR CXTPMarkupParser::ReadStreamDataAsUnicode(IStream* pStream, UINT* pchUnicodeText) const
{
	ASSERT(NULL != pchUnicodeText);

	LPWSTR lpUnicodeText = NULL;

	HGLOBAL hData = ReadStreamData(pStream);
	if (NULL != hData)
	{
		LPVOID lpData = GlobalLock(hData);
		SIZE_T cbData = GlobalSize(hData);

		lpUnicodeText = new WCHAR[cbData + 1];
		if (NULL != lpUnicodeText)
		{
			*pchUnicodeText = static_cast<UINT>(cbData) + 1;

			if (XTPLoadUnicodeText(lpData, static_cast<UINT>(cbData), lpUnicodeText,
								   pchUnicodeText))
			{
				lpUnicodeText[*pchUnicodeText] = L'\0';
			}
			else
			{
				delete[] lpUnicodeText;
				lpUnicodeText	= NULL;
				*pchUnicodeText = 0;
			}
		}

		GlobalUnlock(hData);
		GlobalFree(hData);
	}

	return lpUnicodeText;
}

BOOL CXTPMarkupParser::FixReferenceToUndeclaredNamespacePrefixX(IStream* pInputStream,
																IStream** ppOutputStream) const
{
	// Issue example:
	//	XML Error [0xC00CE01D] at 4:59 '<Style x:Key="{x:Type Hyperlink}" TargetType="Hyperlink">':
	// Reference to undeclared namespace prefix: 'x'.
	//	                                       ^

	ASSERT(NULL != pInputStream);
	ASSERT(NULL != ppOutputStream);

	*ppOutputStream = NULL;

	if (0xC00CE01D == m_errorInfo.nCode && NULL != wcsstr((LPCWSTR)m_errorInfo.strReason, L"'x'"))
	{
		TRACE(_T("Applying workaround for 'Reference to undeclared namespace prefix: 'x''\n"));

		UINT chXmlSource	 = 0;
		LPWSTR lpszXmlSource = ReadStreamDataAsUnicode(pInputStream, &chXmlSource);
		if (NULL != lpszXmlSource)
		{
			// Look for root tag entry.
			LPWSTR lpPos = wcschr(lpszXmlSource, L'<');
			if (NULL != lpPos)
			{
				if (L'?' == lpPos[1])
				{
					lpPos = wcschr(lpPos + 2, L'<');
				}

				if (NULL != lpPos)
				{
					// Root tag found. Skip tag name.
					LPCWSTR lpEnd = lpszXmlSource + chXmlSource;
					while (!iswspace(*lpPos) && L'>' != *lpPos && lpPos < lpEnd)
					{
						++lpPos;
					}

					if (lpPos < lpEnd)
					{
						// Split XML source for wedging 'x' namespace declaration.
						MEMORY_BLOB dataBlobs[5];
						dataBlobs[0].pBlobData = UnicodeBOM;
						dataBlobs[0].cbSize	   = sizeof(UnicodeBOM);

						dataBlobs[1].pBlobData = lpszXmlSource;
						dataBlobs[1].cbSize	   = (lpPos - lpszXmlSource) * sizeof(WCHAR);
						const BOOL bSrcUnicode = (sizeof(UnicodeBOM) <= dataBlobs[1].cbSize
												  && 0
														 == memcmp(dataBlobs[1].pBlobData,
																   UnicodeBOM, sizeof(UnicodeBOM)));

						dataBlobs[2].pBlobData = L" ";
						dataBlobs[2].cbSize	   = sizeof(WCHAR);

						static const WCHAR szXNamspaceDeclaration[] =
							L"xmlns:x='http://schemas.microsoft.com/winfx/2006/xaml'";
						dataBlobs[3].pBlobData = szXNamspaceDeclaration;
						dataBlobs[3].cbSize	   = sizeof(szXNamspaceDeclaration) - sizeof(WCHAR);

						dataBlobs[4].pBlobData = lpPos;
						dataBlobs[4].cbSize	   = (chXmlSource - (lpPos - lpszXmlSource))
											  * sizeof(WCHAR);

						CreateDataStream(dataBlobs + (bSrcUnicode ? 1 : 0),
										 _countof(dataBlobs) - (bSrcUnicode ? 1 : 0),
										 ppOutputStream);
					}
				}
			}

			delete[] lpszXmlSource;
		}
	}

	return NULL != *ppOutputStream;
}
