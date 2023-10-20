// XTPXMLHelpers.cpp
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
#include "Common/XTPVC80Helpers.h"
#include "Common/XTPSystemHelpers.h"
#include "Common/XTPResourceManager.h"
#include "Common/XTPXMLHelpers.h"

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

#ifdef _DEBUG
#	define new DEBUG_NEW
#	undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CString AFX_CDECL XTPEscapeXmlString(LPCTSTR lpString, LPCTSTR lpDefaultValue /*= _T("")*/)
{
	ASSERT(NULL != lpString);

	CXTPComInitializer initCOM;

	CString strEscapedString;
	BOOL bSuccess = FALSE;

	CXTPDOMDocumentPtr xmlDocument = XTPCreateXmlDocumentInstance();
	if (NULL != xmlDocument)
	{
		xmlDocument->put_async(VARIANT_FALSE);
		xmlDocument->put_preserveWhiteSpace(-1 /*VARIANT_TRUE*/); // produces warning in VC6

		if (VARIANT_FALSE != xmlDocument->loadXML(L"<X/>"))
		{
			xmlDocument->documentElement->text = lpString;
			_bstr_t xml						   = xmlDocument->documentElement->xml;
			LPCWSTR lpFrom					   = xml.operator LPCWSTR() + (sizeof("<X>") - 1);
			LPCWSTR lpTo = xml.operator LPCWSTR() + xml.length() - (sizeof("</X>") - 1);

			ASSERT(lpFrom <= lpTo);
			ASSERT(0 == wcsncmp(lpTo, L"</X>", sizeof("</X>") - 1));

			SIZE_T cch		= XTPToSizeTChecked(lpTo - lpFrom);
			WCHAR* pWBuffer = new WCHAR[cch + 1];
			memcpy(pWBuffer, lpFrom, cch * sizeof(WCHAR));
			pWBuffer[cch]	= L'\0';
			strEscapedString = XTP_CW2CT(pWBuffer);
			delete[] pWBuffer;

			bSuccess = TRUE;
		}
	}

	if (!bSuccess)
	{
		strEscapedString = (NULL != lpDefaultValue ? lpDefaultValue : _T(""));
	}

	return strEscapedString;
}

CXTPDOMDocumentPtr AFX_CDECL
	XTPCreateXmlDocumentInstance(BOOL fExact /*= FALSE*/, const CLSID* pClsid /*= NULL*/,
								 int fEnableProps /*= xtpXmlDomDefaultEnabled*/,
								 int fDisableProps /*= xtpXmlDomDefaultDisabled*/)
{
	ASSERT("XML DOM properties for enabling and disabling must not overlap"
		   && 0 == (fEnableProps & fDisableProps));
	ASSERT("XPath must not be combined with XSLPattern for enabling"
		   && (xtpXmlDomUseXPath | xtpXmlDomUseXSLPattern)
				  != (fEnableProps & (xtpXmlDomUseXPath | xtpXmlDomUseXSLPattern)));

	CXTPDOMDocumentPtr pDoc;
	const CLSID* clsid = (NULL != pClsid ? pClsid : &XTP_MSXML_CSLID_DOMDOCUMENT);

	if (fExact)
	{
		pDoc.CreateInstance(*clsid);
	}
	else
	{
		static const CLSID* arrKnownClasses[] = { &CLSID_XTPDOMDocument6, &CLSID_XTPDOMDocument5,
												  &CLSID_XTPDOMDocument3, &CLSID_XTPDOMDocument };

		for (int i = 0; i < _countof(arrKnownClasses); ++i)
		{
			if (SUCCEEDED(pDoc.CreateInstance(*arrKnownClasses[i])))
				break;

			if (0 == memcmp(&XTP_MSXML_CSLID_DOMDOCUMENT, arrKnownClasses[i], sizeof(CLSID)))
				break;
		}
	}

	if (NULL != pDoc.GetInterfacePtr())
	{
		XTPXML::IXMLDOMDocument2Ptr pDoc2;
		if (S_OK == pDoc->QueryInterface(&pDoc2))
		{
			// Enable requested properties.

			if (0 != (fEnableProps & xtpXmlDomAllowDocumentFunction))
				pDoc2->setProperty(_bstr_t(L"AllowDocumentFunction"), _variant_t(true));
			if (0 != (fEnableProps & xtpXmlDomAllowXsltScript))
				pDoc2->setProperty(_bstr_t(L"AllowXsltScript"), _variant_t(true));
			if (0 != (fEnableProps & xtpXmlDomForcedResync))
				pDoc2->setProperty(_bstr_t(L"ForcedResync"), _variant_t(true));
			if (0 != (fEnableProps & xtpXmlDomMultipleErrorMessages))
				pDoc2->setProperty(_bstr_t(L"MultipleErrorMessages"), _variant_t(true));
			if (0 != (fEnableProps & xtpXmlDomNewParser))
				pDoc2->setProperty(_bstr_t(L"NewParser"), _variant_t(true));
			if (0 != (fEnableProps & xtpXmlDomNormalizeAttributeValues))
				pDoc2->setProperty(_bstr_t(L"NormalizeAttributeValues"), _variant_t(true));
			if (0 != (fEnableProps & xtpXmlDomPopulateElementDefaultValues))
				pDoc2->setProperty(_bstr_t(L"PopulateElementDefaultValues"), _variant_t(true));
			if (0 != (fEnableProps & xtpXmlDomProhibitDTD))
				pDoc2->setProperty(_bstr_t(L"ProhibitDTD"), _variant_t(true));
			if (0 != (fEnableProps & xtpXmlDomResolveExternals))
				pDoc2->setProperty(_bstr_t(L"ResolveExternals"), _variant_t(true));
			if (0 != (fEnableProps & xtpXmlDomServerHTTPRequest))
				pDoc2->setProperty(_bstr_t(L"ServerHTTPRequest"), _variant_t(true));
			if (0 != (fEnableProps & xtpXmlDomUseInlineSchema))
				pDoc2->setProperty(_bstr_t(L"UseInlineSchema"), _variant_t(true));
			if (0 != (fEnableProps & xtpXmlDomValidateOnParse))
				pDoc2->setProperty(_bstr_t(L"ValidateOnParse"), _variant_t(true));
			if (0 != (fEnableProps & xtpXmlDomUseXPath))
				pDoc2->setProperty(_bstr_t(L"SelectionLanguage"), _variant_t(L"XPath"));
			if (0 != (fEnableProps & xtpXmlDomUseXSLPattern))
				pDoc2->setProperty(_bstr_t(L"SelectionLanguage"), _variant_t(L"XSLPattern"));

			// Disable requested properties.

			if (0 != (fDisableProps & xtpXmlDomAllowDocumentFunction))
				pDoc2->setProperty(_bstr_t(L"AllowDocumentFunction"), _variant_t(false));
			if (0 != (fDisableProps & xtpXmlDomAllowXsltScript))
				pDoc2->setProperty(_bstr_t(L"AllowXsltScript"), _variant_t(false));
			if (0 != (fDisableProps & xtpXmlDomForcedResync))
				pDoc2->setProperty(_bstr_t(L"ForcedResync"), _variant_t(false));
			if (0 != (fDisableProps & xtpXmlDomMultipleErrorMessages))
				pDoc2->setProperty(_bstr_t(L"MultipleErrorMessages"), _variant_t(false));
			if (0 != (fDisableProps & xtpXmlDomNewParser))
				pDoc2->setProperty(_bstr_t(L"NewParser"), _variant_t(false));
			if (0 != (fDisableProps & xtpXmlDomNormalizeAttributeValues))
				pDoc2->setProperty(_bstr_t(L"NormalizeAttributeValues"), _variant_t(false));
			if (0 != (fDisableProps & xtpXmlDomPopulateElementDefaultValues))
				pDoc2->setProperty(_bstr_t(L"PopulateElementDefaultValues"), _variant_t(false));
			if (0 != (fDisableProps & xtpXmlDomProhibitDTD))
				pDoc2->setProperty(_bstr_t(L"ProhibitDTD"), _variant_t(false));
			if (0 != (fDisableProps & xtpXmlDomResolveExternals))
				pDoc2->setProperty(_bstr_t(L"ResolveExternals"), _variant_t(false));
			if (0 != (fDisableProps & xtpXmlDomServerHTTPRequest))
				pDoc2->setProperty(_bstr_t(L"ServerHTTPRequest"), _variant_t(false));
			if (0 != (fDisableProps & xtpXmlDomUseInlineSchema))
				pDoc2->setProperty(_bstr_t(L"UseInlineSchema"), _variant_t(false));
			if (0 != (fDisableProps & xtpXmlDomValidateOnParse))
				pDoc2->setProperty(_bstr_t(L"ValidateOnParse"), _variant_t(false));
		}
	}
	else
	{
		TRACE(_T("Unable to create MSXML document instance of the requested class.\n"));
	}

	return pDoc;
}
