// XTPMarkupBuilder.cpp: implementation of the CXTPMarkupBuilder class.
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
#include "Common/Base/cxminmax.h"

#include "GraphicLibrary/GdiPlus/XTPGdiPlus.h"

#include "Common/XTPTypeId.h"
#include "Common/Math/XTPMathUtils.h"
#include "Common/XTPCasting.h"
#include "Common/XTPFramework.h"
#include "Common/XTPVc80Helpers.h"
#include "Common/XTPSystemHelpers.h"
#include "Common/XTPSynchro.h"
#include "Common/XTPApplication.h"
#include "Common/XTPSingleton.h"
#include "Common/XTPGdiObjects.h"
#include "Common/XTPDrawHelpers.h"
#include "Common/XTPXMLHelpers.h"
#include "Common/XTPResourceManager.h"
#include "Common/Base/Types/XTPPoint2.h"
#include "Common/Base/Types/XTPSize.h"
#include "Common/Base/Types/XTPRect.h"

#include "Markup/XTPMarkupContext.h"
#include "Markup/XTPMarkupTools.h"
#include "Markup/XTPMarkupBuilder.h"
#include "Markup/XTPMarkupParser.h"
#include "Markup/XTPMarkupObject.h"
#include "Markup/XTPMarkupStyle.h"
#include "Markup/XTPMarkupString.h"
#include "Markup/XTPMarkupInputElement.h"
#include "Markup/XTPMarkupUIElement.h"
#include "Markup/XTPMarkupFrameworkElement.h"
#include "Markup/Extensions/XTPMarkupExtension.h"

#include "Markup/XTPMarkupDrawingContext.h"
#include "Markup/Controls/XTPMarkupControl.h"
#include "Markup/Controls/XTPMarkupContentControl.h"
#include "Markup/Controls/XTPMarkupTextBlock.h"
#include "Markup/Controls/XTPMarkupScrollViewer.h"
#include "Markup/XTPMarkupFrameworkContentElement.h"
#include "Markup/Text/XTPMarkupTextElement.h"
#include "Markup/Text/XTPMarkupInline.h"
#include "Markup/Text/XTPMarkupInlineCollection.h"
#include "Markup/Text/XTPMarkupRun.h"
#include "Markup/XTPMarkupResourceDictionary.h"
#include "Markup/Shapes/XTPMarkupShape.h"
#include "Markup/Extensions/XTPMarkupStaticExtension.h"
#include "Markup/Extensions/XTPMarkupSystemColorsStaticExtension.h"

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

#ifdef _DEBUG
#	undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#	define new DEBUG_NEW
#endif

#define XTP_MARKUP_STRING_CONSTANT_NAME(Name) g_xtpMarkupString##Name
#define XTP_MARKUP_STRING_CONSTANT(Name) (*XTP_MARKUP_STRING_CONSTANT_NAME(Name))
#define XTP_DECLARE_MARKUP_STRING_CONSTANT(Name)                                                   \
	static CXTPMarkupString* AFX_CDECL XTPInitMarkupString__##Name()                               \
	{                                                                                              \
		return new CXTPMarkupString(XTP_WIDE(#Name), _countof(XTP_WIDE(#Name)) - 1,                \
									CXTPMarkupObject::flagNoCom);                                  \
	}                                                                                              \
	static void AFX_CDECL XTPUninitMarkupString__##Name(CXTPMarkupString* pStr)                    \
	{                                                                                              \
		SAFE_RELEASE(pStr);                                                                        \
	}                                                                                              \
	static CXTPThreadLocal<CXTPMarkupString> XTP_MARKUP_STRING_CONSTANT_NAME(                      \
		Name)(XTPInitMarkupString__##Name, XTPUninitMarkupString__##Name);

XTP_DECLARE_MARKUP_STRING_CONSTANT(AppStarting);
XTP_DECLARE_MARKUP_STRING_CONSTANT(Arrow);
XTP_DECLARE_MARKUP_STRING_CONSTANT(Auto);
XTP_DECLARE_MARKUP_STRING_CONSTANT(Baseline);
XTP_DECLARE_MARKUP_STRING_CONSTANT(Black);
XTP_DECLARE_MARKUP_STRING_CONSTANT(Bold);
XTP_DECLARE_MARKUP_STRING_CONSTANT(Bottom);
XTP_DECLARE_MARKUP_STRING_CONSTANT(Center);
XTP_DECLARE_MARKUP_STRING_CONSTANT(CharacterEllipsis);
XTP_DECLARE_MARKUP_STRING_CONSTANT(ClearType);
XTP_DECLARE_MARKUP_STRING_CONSTANT(Collapsed);
XTP_DECLARE_MARKUP_STRING_CONSTANT(Cross);
XTP_DECLARE_MARKUP_STRING_CONSTANT(DemiBold);
XTP_DECLARE_MARKUP_STRING_CONSTANT(Disabled);
XTP_DECLARE_MARKUP_STRING_CONSTANT(ExtraBlack);
XTP_DECLARE_MARKUP_STRING_CONSTANT(ExtraBold);
XTP_DECLARE_MARKUP_STRING_CONSTANT(ExtraLight);
XTP_DECLARE_MARKUP_STRING_CONSTANT(False);
XTP_DECLARE_MARKUP_STRING_CONSTANT(Fill);
XTP_DECLARE_MARKUP_STRING_CONSTANT(Hand);
XTP_DECLARE_MARKUP_STRING_CONSTANT(Heavy);
XTP_DECLARE_MARKUP_STRING_CONSTANT(Hidden);
XTP_DECLARE_MARKUP_STRING_CONSTANT(Horizontal);
XTP_DECLARE_MARKUP_STRING_CONSTANT(IBeam);
XTP_DECLARE_MARKUP_STRING_CONSTANT(Italic);
XTP_DECLARE_MARKUP_STRING_CONSTANT(Justify);
XTP_DECLARE_MARKUP_STRING_CONSTANT(Left);
XTP_DECLARE_MARKUP_STRING_CONSTANT(Light);
XTP_DECLARE_MARKUP_STRING_CONSTANT(Medium);
XTP_DECLARE_MARKUP_STRING_CONSTANT(No);
XTP_DECLARE_MARKUP_STRING_CONSTANT(None);
XTP_DECLARE_MARKUP_STRING_CONSTANT(Normal);
XTP_DECLARE_MARKUP_STRING_CONSTANT(NoWrap);
XTP_DECLARE_MARKUP_STRING_CONSTANT(Regular);
XTP_DECLARE_MARKUP_STRING_CONSTANT(Right);
XTP_DECLARE_MARKUP_STRING_CONSTANT(SemiBold);
XTP_DECLARE_MARKUP_STRING_CONSTANT(SizeAll);
XTP_DECLARE_MARKUP_STRING_CONSTANT(SizeNESW);
XTP_DECLARE_MARKUP_STRING_CONSTANT(SizeNS);
XTP_DECLARE_MARKUP_STRING_CONSTANT(SizeNWSE);
XTP_DECLARE_MARKUP_STRING_CONSTANT(SizeWE);
XTP_DECLARE_MARKUP_STRING_CONSTANT(Stretch);
XTP_DECLARE_MARKUP_STRING_CONSTANT(Strikethrough);
XTP_DECLARE_MARKUP_STRING_CONSTANT(Subscript);
XTP_DECLARE_MARKUP_STRING_CONSTANT(Superscript);
XTP_DECLARE_MARKUP_STRING_CONSTANT(TextBottom);
XTP_DECLARE_MARKUP_STRING_CONSTANT(TextTop);
XTP_DECLARE_MARKUP_STRING_CONSTANT(Thin);
XTP_DECLARE_MARKUP_STRING_CONSTANT(Top);
XTP_DECLARE_MARKUP_STRING_CONSTANT(True);
XTP_DECLARE_MARKUP_STRING_CONSTANT(UltraBold);
XTP_DECLARE_MARKUP_STRING_CONSTANT(Underline);
XTP_DECLARE_MARKUP_STRING_CONSTANT(Uniform);
XTP_DECLARE_MARKUP_STRING_CONSTANT(UniformToFill);
XTP_DECLARE_MARKUP_STRING_CONSTANT(UpArrow);
XTP_DECLARE_MARKUP_STRING_CONSTANT(Vertical);
XTP_DECLARE_MARKUP_STRING_CONSTANT(Visible);
XTP_DECLARE_MARKUP_STRING_CONSTANT(Wait);
XTP_DECLARE_MARKUP_STRING_CONSTANT(WordEllipsis);
XTP_DECLARE_MARKUP_STRING_CONSTANT(Wrap);
XTP_DECLARE_MARKUP_STRING_CONSTANT(WrapWithOverflow);

//////////////////////////////////////////////////////////////////////////
// CXTPMarkupObjectProperty

class CXTPMarkupObjectProperty : public CXTPMarkupObject
{
	DECLARE_MARKUPCLASS(CXTPMarkupObjectProperty)
public:
	CXTPMarkupObjectProperty(CXTPMarkupDependencyProperty* pProperty = 0)
		: CXTPMarkupObject(CXTPMarkupObject::flagNoCom)
	{
		m_pProperty   = pProperty;
		m_pContent	= NULL;
		m_pCollection = NULL;
	}

	~CXTPMarkupObjectProperty()
	{
		MARKUP_RELEASE(m_pContent);
		MARKUP_RELEASE(m_pCollection);
	}

	virtual void SetContentObject(CXTPMarkupBuilder* pBuilder, CXTPMarkupObject* pContent)
	{
		CXTPMarkupType* pContentType = m_pProperty->GetPropetyType();

		if (pContent->IsKindOf(pContentType))
		{
			ASSERT(m_pContent == NULL);
			MARKUP_RELEASE(m_pContent);
			m_pContent = pContent;
			MARKUP_ADDREF(m_pContent);
		}
		else if (IsStringObject(pContent))
		{
			ASSERT(m_pContent == NULL);
			MARKUP_RELEASE(m_pContent);
			m_pContent = pContent;
			MARKUP_ADDREF(m_pContent);
		}
		else
		{
			if (m_pContent == NULL)
			{
				m_pCollection = pBuilder->GetMarkupContext()->CreateMarkupObject(
					m_pProperty->GetPropetyType());
				if (m_pCollection)
				{
					m_pCollection->SetAssociatedProperty(m_pProperty);
					m_pCollection->SetContentObject(pBuilder, pContent);
				}

				m_pContent	= m_pCollection;
				m_pCollection = NULL;
			}
			else
			{
				m_pContent->SetContentObject(pBuilder, pContent);
			}
		}
	}

	CXTPMarkupObject* FindResource(const CXTPMarkupObject* pKey) const
	{
		if (m_pContent)
			return CXTPMarkupResourceDictionary::FindResource(m_pContent, pKey);

		return NULL;
	}

	CXTPMarkupObject* m_pContent;
	CXTPMarkupObject* m_pCollection;
	CXTPMarkupDependencyProperty* m_pProperty;
};

IMPLEMENT_MARKUPCLASS(NULL, CXTPMarkupObjectProperty, CXTPMarkupObject)

void CXTPMarkupObjectProperty::RegisterMarkupClass()
{
}

//////////////////////////////////////////////////////////////////////////
// CXTPMarkupBuilder::CStaticExtension

class CXTPMarkupBuilder::CStaticExtension
{
public:
	CXTPMarkupObject* ProvideValue(CXTPMarkupBuilder* pBuilder, LPCWSTR lpszValue);
};

CXTPMarkupObject* CXTPMarkupBuilder::CStaticExtension::ProvideValue(CXTPMarkupBuilder* pBuilder,
																	LPCWSTR lpszValue)
{
	ASSERT(NULL != pBuilder);
	CXTPMarkupObject* pObject = pBuilder->GetMarkupContext()->GetExtensionRoot()->ProvideValue(
		lpszValue);

	if (NULL == pObject)
	{
		pBuilder->ThrowBuilderException(
			pBuilder->FormatString(_T("Cannot find the static extension '%ls'"),
								   reinterpret_cast<LPCTSTR>(lpszValue)));
	}

	return pObject;
}

//////////////////////////////////////////////////////////////////////////
// CXTPMarkupBuilder::CStaticResourceExtension

class CXTPMarkupBuilder::CStaticResourceExtension
{
public:
	CStaticResourceExtension();
	~CStaticResourceExtension();

public:
	CXTPMarkupObject* ProvideValue(CXTPMarkupBuilder* pBuilder, LPCWSTR lpszValue);
	CXTPMarkupObject* FindResources(CXTPMarkupObject* pElement, CXTPMarkupObject* pValue);
	CXTPMarkupObject* ResolveKey(CXTPMarkupBuilder* pBuilder, LPCWSTR lpszValue);
};

CXTPMarkupBuilder::CStaticResourceExtension::CStaticResourceExtension()
{
}

CXTPMarkupBuilder::CStaticResourceExtension::~CStaticResourceExtension()
{
}

CXTPMarkupObject* CXTPMarkupBuilder::CStaticResourceExtension::FindResources(
	CXTPMarkupObject* pElement, CXTPMarkupObject* pKey)
{
	if (pElement->IsKindOf(MARKUP_TYPE(CXTPMarkupFrameworkElement)))
	{
		CXTPMarkupObject* pValue = ((CXTPMarkupFrameworkElement*)pElement)->FindResource(pKey);
		if (pValue)
			return pValue;
	}

	if (pElement->IsKindOf(MARKUP_TYPE(CXTPMarkupFrameworkContentElement)))
	{
		CXTPMarkupObject* pValue =
			((CXTPMarkupFrameworkContentElement*)pElement)->FindResource(pKey);
		if (pValue)
			return pValue;
	}

	if (pElement->IsKindOf(MARKUP_TYPE(CXTPMarkupStyle)))
	{
		CXTPMarkupObject* pValue = ((CXTPMarkupStyle*)pElement)->FindResource(pKey);
		if (pValue)
			return pValue;
	}

	if (pElement->IsKindOf(MARKUP_TYPE(CXTPMarkupObjectProperty)))
	{
		CXTPMarkupObject* pValue = ((CXTPMarkupObjectProperty*)pElement)->FindResource(pKey);
		if (pValue)
			return pValue;
	}

	return NULL;
}

CXTPMarkupObject* CXTPMarkupBuilder::CStaticResourceExtension::ResolveKey(
	CXTPMarkupBuilder* pBuilder, LPCWSTR lpszValue)
{
	while (*lpszValue == ' ' || *lpszValue == '\n' || *lpszValue == '\r')
		lpszValue++;

	if (lpszValue[0] == '{')
	{
		if (lpszValue[1] == '}')
		{
			lpszValue = lpszValue + 2;
		}
		else
		{
			return pBuilder->ResolveMarkupExtension(lpszValue + 1);
		}
	}

	return CXTPMarkupString::CreateValue(lpszValue);
}

CXTPMarkupObject* CXTPMarkupBuilder::CStaticResourceExtension::ProvideValue(
	CXTPMarkupBuilder* pBuilder, LPCWSTR lpszValue)
{
	CXTPMarkupObject* pKey = ResolveKey(pBuilder, lpszValue);
	if (!pKey)
		return NULL;

	POSITION pos = pBuilder->m_arrObjects.GetTailPosition();
	while (pos)
	{
		CXTPMarkupObject* pElement = pBuilder->m_arrObjects.GetPrev(pos);

		CXTPMarkupObject* pValue = FindResources(pElement, pKey);

		if (pValue)
		{
			MARKUP_RELEASE(pKey);
			MARKUP_ADDREF(pValue);
			return pValue;
		}
	}

	if (pKey->IsKindOf(MARKUP_TYPE(CXTPMarkupColorKey)))
	{
		CXTPMarkupObject* pValue = CXTPMarkupSystemColorStaticExtension::GetSystemColor(
			((CXTPMarkupColorKey*)pKey)->m_nIndex);
		return pValue;
	}

	if (pKey->IsKindOf(MARKUP_TYPE(CXTPMarkupBrushKey)))
	{
		CXTPMarkupObject* pValue = CXTPMarkupSystemBrushStaticExtension::GetSystemBrush(
			((CXTPMarkupBrushKey*)pKey)->m_nIndex);
		return pValue;
	}

	MARKUP_RELEASE(pKey);

	pBuilder->ThrowBuilderException(pBuilder->FormatString(_T("Cannot find resource named '%ls'. ")
														   _T("Resource names are case sensitive"),
														   reinterpret_cast<LPCTSTR>(lpszValue)));

	return 0;
}

//////////////////////////////////////////////////////////////////////////
// CXTPMarkupBuilder

CXTPMarkupBuilder::CStaticExtension* CXTPMarkupBuilder::GetStaticExtension()
{
	if (m_pStaticExtension == NULL)
	{
		m_pStaticExtension = new CStaticExtension;
	}

	return m_pStaticExtension;
}

CXTPMarkupBuilder::CStaticResourceExtension* CXTPMarkupBuilder::GetStaticResourceExtension()
{
	if (m_pStaticResourceExtension == NULL)
	{
		m_pStaticResourceExtension = new CStaticResourceExtension();
	}

	return m_pStaticResourceExtension;
}

//////////////////////////////////////////////////////////////////////////
// CXTPMarkupBuilderException

IMPLEMENT_DYNAMIC(CXTPMarkupBuilderException, CException)

CXTPMarkupBuilderException::CXTPMarkupBuilderException(LPCTSTR lpszError)
{
	m_bInitialized = TRUE;
	m_bLoaded	  = TRUE;

	m_szMessage[_countof(m_szMessage) - 1] = 0;
	STRNCPY_S(m_szMessage, _countof(m_szMessage), lpszError, _countof(m_szMessage) - 1);
}

BOOL CXTPMarkupBuilderException::GetErrorMessage(LPTSTR lpszError, UINT nMaxError,
												 PUINT pnHelpContext)
{
	ASSERT(lpszError != NULL && AfxIsValidString(lpszError, XTPToIntChecked(nMaxError)));
	if (!lpszError)
		return FALSE;

	if (pnHelpContext != NULL)
		*pnHelpContext = 0;

	// if we didn't load our string (eg, we're a console app)
	lstrcpyn(lpszError, m_szMessage, XTPToIntChecked(nMaxError));

	return TRUE;
}

BOOL CXTPMarkupBuilderException::GetErrorMessage(LPTSTR lpszError, UINT nMaxError,
												 PUINT pnHelpContext) const
{
	return const_cast<CXTPMarkupBuilderException*>(this)->GetErrorMessage(lpszError, nMaxError,
																		  pnHelpContext);
}

//////////////////////////////////////////////////////////////////////////
// CStringBuilder

class CXTPMarkupBuilder::CStringBuilder
{
public:
	CStringBuilder();
	~CStringBuilder();

public:
	void Add(LPCWSTR lpszWord);
	void Empty();

	int GetLength() const
	{
		return m_nLength;
	}

	LPCWSTR GetBuffer()
	{
		ASSERT(m_lpszData);
		ASSERT(m_nLength < m_nAlloc);
		m_lpszData[m_nLength] = 0;

		return m_lpszData;
	}

protected:
	int m_nAlloc;
	int m_nLength;
	LPWSTR m_lpszData;
};

CXTPMarkupBuilder::CStringBuilder::CStringBuilder()
{
	m_nAlloc   = 0;
	m_nLength  = 0;
	m_lpszData = NULL;
}

CXTPMarkupBuilder::CStringBuilder::~CStringBuilder()
{
	if (m_lpszData)
	{
		delete[] m_lpszData;
	}
}

void CXTPMarkupBuilder::CStringBuilder::Add(LPCWSTR lpszWord)
{
	int nWordLength = (int)wcslen(lpszWord);

	if (m_nAlloc < m_nLength + nWordLength + 1)
	{
		int nAlloc		= m_nLength + CXTP_max(256, nWordLength + 1);
		LPWSTR lpszData = new WCHAR[XTPToUIntChecked(nAlloc)];

		if (m_nLength > 0)
			memcpy(lpszData, m_lpszData, m_nLength * sizeof(WCHAR));
		if (m_lpszData)
			delete[] m_lpszData;

		m_lpszData = lpszData;
		m_nAlloc   = nAlloc;
	}

	memcpy(m_lpszData + m_nLength, lpszWord, nWordLength * sizeof(WCHAR));
	m_nLength += nWordLength;
}

void CXTPMarkupBuilder::CStringBuilder::Empty()
{
	m_nLength = 0;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CXTPMarkupBuilder::CXTPMarkupBuilder(CXTPMarkupContext* pContext)
	: m_pComInitializer(new CXTPComInitializer)
	, m_pMarkupContext(pContext)
	, m_pStaticExtension(NULL)
	, m_pStaticResourceExtension(NULL)
{
	ASSERT(m_pMarkupContext);
}

CXTPMarkupBuilder::~CXTPMarkupBuilder()
{
	SAFE_DELETE(m_pStaticExtension);
	SAFE_DELETE(m_pStaticResourceExtension);
	SAFE_DELETE(m_pComInitializer);
}

inline BOOL IsXmlWhitespace(WCHAR ch)
{
	return (ch < 0x80 && iswspace(ch));
}

inline BOOL IsXmlWhitespaceString(LPCWSTR lpszString)
{
	ASSERT(NULL != lpszString);

	BOOL bWhitespaceString = FALSE;
	if (L'\0' != *lpszString)
	{
		bWhitespaceString = TRUE;
		LPCWSTR lpCh	  = lpszString;
		while (L'\0' != *lpCh)
		{
			if (!IsXmlWhitespace(*lpCh))
			{
				bWhitespaceString = FALSE;
				break;
			}

			++lpCh;
		}
	}
	return bWhitespaceString;
}

inline LPCWSTR NormalizeXmlContentText(LPCWSTR lpszString, CArray<WCHAR, WCHAR>& strBuffer,
									   int nTextLen, BOOL bForceSpaceOnLeft,
									   BOOL bForceSpaceOnRight)
{
	ASSERT(NULL != lpszString);

	LPCWSTR lpLeft  = lpszString;
	LPCWSTR lpRight = lpLeft + nTextLen;

	BOOL bLeftSpaceFound  = FALSE;
	BOOL bRightSpaceFound = FALSE;

	while (lpLeft < lpRight && IsXmlWhitespace(*lpLeft))
	{
		++lpLeft;
		bLeftSpaceFound = TRUE;
	}

	if (lpLeft < lpRight)
	{
		--lpRight;

		while (lpLeft < lpRight && IsXmlWhitespace(*lpRight))
		{
			--lpRight;
			bRightSpaceFound = TRUE;
		}

		++lpRight;
	}

	SIZE_T nLen = XTPToSizeTChecked(lpRight - lpLeft);
	ASSERT(0 <= (lpRight - lpLeft));

	strBuffer.SetSize(XTPToIntPtrChecked(nLen + 2 + 1));
	LPWSTR lpStr = strBuffer.GetData();
	if (bForceSpaceOnLeft && bLeftSpaceFound)
	{
		*lpStr++ = L' ';
	}

	BOOL bSpaceFound = FALSE;
	while (lpLeft < lpRight)
	{
		if (!IsXmlWhitespace(*lpLeft))
		{
			if (bSpaceFound)
			{
				*lpStr++	= L' ';
				bSpaceFound = FALSE;
			}

			*lpStr++ = *lpLeft++;
		}
		else
		{
			bSpaceFound = TRUE;
			++lpLeft;
		}
	}

	if (bForceSpaceOnRight && bRightSpaceFound)
	{
		*lpStr++ = L' ';
	}
	*lpStr = L'\0';

	return strBuffer.GetData();
}

inline LPCWSTR FindXmlNamespaceEnd(LPCWSTR lpszName)
{
	ASSERT(NULL != lpszName);

	LPCWSTR lpNsEnd = NULL;
	LPCWSTR lpCh	= lpszName;
	do
	{
		if (L':' == *lpCh)
		{
			lpNsEnd = lpCh;
			break;
		}
	} while (L'\0' != *lpCh++);

	return lpNsEnd;
}

inline BOOL IsXmlNamespaceSupported(LPCWSTR lpszTagOrAttrName)
{
	ASSERT(NULL != lpszTagOrAttrName);

	BOOL bSupported = TRUE;
	LPCWSTR lpNsEnd = FindXmlNamespaceEnd(lpszTagOrAttrName);
	if (NULL != lpNsEnd)
	{
		SIZE_T cch = XTPToSizeTChecked(lpNsEnd - lpszTagOrAttrName);

		// Note: For now it is hardcoded and always assumed that
		// all default namespace is 'http://schemas.microsoft.com/winfx/2006/xaml/presentation'
		// and x='http://schemas.microsoft.com/winfx/2006/xaml', but
		// that's not right and has to be changed in future so that:
		//  1. users would be able to choose whatever prefixes for XML
		//     namespaces they like, the only thing that matters has
		//     to remain XML namespace name itself, e.g:
		//       <Grid x:Name='A' xmlns='http://schemas.microsoft.com/winfx/2006/xaml/presentation'
		//       xmlns:x='http://schemas.microsoft.com/winfx/2006/xaml' />
		//     has to be to equal to
		//       <xaml:Grid winfx:Name='A'
		//       xmlns:xaml='http://schemas.microsoft.com/winfx/2006/xaml/presentation'
		//       xmlns:winfx='http://schemas.microsoft.com/winfx/2006/xaml' />
		//  2. user would be able to add their own namespaces with
		//     custom tags and attributes without conflicting with
		//     this implementation.
		bSupported = (0 == wcsncmp(lpszTagOrAttrName, L"x:", cch))
					 || (0 == wcsncmp(lpszTagOrAttrName, L"xtp:", cch));
	}

	return bSupported;
}

CXTPMarkupObject* CXTPMarkupBuilder::CreateObject(LPCWSTR lpszTag, XTPXML::IXMLDOMElement* pElement,
												  CXTPMarkupObject* pParent /*= NULL*/)
{
	ASSERT(NULL != lpszTag);

	CXTPMarkupObject* pMarkupObject = NULL;

	LPCWSTR lpszAttributeDot = wcschr(lpszTag, L'.');
	if (NULL != lpszAttributeDot)
	{
		if (NULL != pParent)
		{
			_bstr_t tagFullName		 = lpszTag;
			LPWSTR lpszTagPart		 = tagFullName;
			LPWSTR lpszAttributePart = lpszTagPart + (lpszAttributeDot - lpszTag + 1);
			lpszAttributePart[-1]	= L'\0';
			CXTPMarkupDependencyProperty* pProperty = FindProperty(lpszTagPart, lpszAttributePart);

			if (NULL != pProperty)
			{
				BOOL bValidUse = FALSE;
				if (!pProperty->IsAttached())
				{
					// A dependency property can only be an immediate
					// child of tah parent tag.
					bValidUse = (0 == wcscmp(pParent->GetTagName(), lpszTagPart));
				}
				else
				{
					// An attached property requries a parent to be on
					// the tree up to the root element.
					do
					{
						bValidUse = (0 == wcscmp(pParent->GetTagName(), lpszTagPart));
						if (bValidUse)
						{
							break;
						}

						pParent = pParent->GetLogicalParent();
					} while (NULL != pParent);
				}

				if (!bValidUse)
				{
					ThrowBuilderException(FormatString(_T("Invalid use of dependency property %ls"),
													   reinterpret_cast<LPCTSTR>(lpszTag)));
				}

				pMarkupObject = new CXTPMarkupObjectProperty(pProperty);
			}
		}
	}
	else
	{
		CXTPMarkupType* pType = CXTPMarkupType::LookupTag(lpszTag);
		if (NULL != pType)
		{
			pMarkupObject = m_pMarkupContext->CreateMarkupObject(pType);
		}
	}

	if (NULL != pMarkupObject)
	{
		pMarkupObject->SetTagName(lpszTag);
		pMarkupObject->SetSourceMarkupElement(pElement);
	}

	return pMarkupObject;
}

CString CXTPMarkupBuilder::FormatString(LPCTSTR lpszFormat, LPCTSTR lpszParameter)
{
	CString str;
	str.Format(lpszFormat, lpszParameter);
	return str;
}

CString CXTPMarkupBuilder::FormatString(LPCTSTR lpszFormat, LPCTSTR lpszParameter1,
										LPCTSTR lpszParameter2)
{
	CString str;
	str.Format(lpszFormat, lpszParameter1, lpszParameter2);
	return str;
}

CString CXTPMarkupBuilder::FormatString(LPCTSTR lpszFormat, LPCTSTR lpszParameter1,
										LPCTSTR lpszParameter2, LPCTSTR lpszParameter3)
{
	CString str;
	str.Format(lpszFormat, lpszParameter1, lpszParameter2, lpszParameter3);
	return str;
}

CXTPMarkupDependencyProperty* CXTPMarkupBuilder::FindProperty(LPCWSTR lpszTag,
															  LPCWSTR lpszAttribute)
{
	CXTPMarkupType* pType = CXTPMarkupType::LookupTag(lpszTag);
	if (!pType)
		return NULL;

	return CXTPMarkupDependencyProperty::FindProperty(pType, lpszAttribute);
}

CXTPMarkupDependencyProperty* CXTPMarkupBuilder::FindProperty(CXTPMarkupType* pRuntimeClass,
															  LPCWSTR lpszAttribute)
{
	LPCWSTR lpszAttributeDot = wcschr(lpszAttribute, L'.');

	if (lpszAttributeDot != NULL)
	{
		*(LPWSTR)lpszAttributeDot				= 0;
		CXTPMarkupDependencyProperty* pProperty = FindProperty(lpszAttribute, lpszAttributeDot + 1);
		*(LPWSTR)lpszAttributeDot				= '.';

		return pProperty;
	}

	if (!pRuntimeClass)
		return NULL;

	return CXTPMarkupDependencyProperty::FindProperty(pRuntimeClass, lpszAttribute);
}

CXTPMarkupObject* CXTPMarkupBuilder::ConvertValue(CXTPMarkupDependencyProperty* pProperty,
												  CXTPMarkupObject* pValue)
{
	CXTPMarkupObject* pNewValue = NULL;

	if (pProperty->GetMetadata() && pProperty->GetMetadata()->m_pConverter)
	{
		pNewValue = (*pProperty->GetMetadata()->m_pConverter)(this, pValue, pProperty);
	}

	if (pNewValue == NULL)
	{
		CXTPMarkupAutoPtr pConverter(
			GetMarkupContext()->CreateMarkupObject(pProperty->GetPropetyType()));
		pConverter->SetAssociatedProperty(pProperty);
		pNewValue = pConverter->ConvertFrom(this, pValue);
	}

	if (!pNewValue)
	{
		ThrowBuilderException(CXTPMarkupBuilder::FormatString(
			_T("Cannot convert string in attribute '%ls' to object of type '%ls'"),
			reinterpret_cast<LPCTSTR>(pProperty->GetName()),
			reinterpret_cast<LPCTSTR>(pProperty->GetPropetyType()->m_lpszClassName)));
	}

	return pNewValue;
}

CXTPMarkupObject* CXTPMarkupBuilder::CreateTypeKey(LPCWSTR lpszType)
{
	CXTPMarkupType* pType = CXTPMarkupType::LookupTag(lpszType);
	if (!pType)
	{
		ThrowBuilderException(
			FormatString(_T("Cannot find type '%ls'"), reinterpret_cast<LPCTSTR>(lpszType)));
	}

	pType->AddRef();
	return pType;
}

#pragma warning(push)
// C4702: unreachable code
#pragma warning(disable : 4702)

CXTPMarkupObject* CXTPMarkupBuilder::ResolveMarkupExtension(LPCWSTR lpszValue)
{
	int nLength = (int)wcslen(lpszValue);

	if (lpszValue[nLength - 1] != '}')
	{
		ThrowBuilderException(_T("Markup extension expressions must end with '}'"));
	}

	*((LPWSTR)lpszValue + nLength - 1) = '\0';

	if (wcsncmp(lpszValue, L"x:Static ", 9) == 0)
	{
		return GetStaticExtension()->ProvideValue(this, lpszValue + 9);
	}

	if (wcsncmp(lpszValue, L"StaticResource ", 15) == 0)
	{
		return GetStaticResourceExtension()->ProvideValue(this, lpszValue + 15);
	}

	if (wcsncmp(lpszValue, L"DynamicResource ", 16) == 0)
	{
		return GetStaticResourceExtension()->ProvideValue(this, lpszValue + 16);
	}

	if (wcsncmp(lpszValue, L"x:Type ", 7) == 0)
	{
		return CreateTypeKey(lpszValue + 7);
	}

	ThrowBuilderException(FormatString(_T("The tag '%ls' does not exist in XML namespace"),
									   reinterpret_cast<LPCTSTR>(lpszValue)));

	return NULL;
}

#pragma warning(pop)

void CXTPMarkupBuilder::LoadMarkupObjectAttributes(CXTPMarkupObject* pObject,
												   XTPXML::IXMLDOMElement* pElement)
{
	ASSERT(NULL != pObject);
	ASSERT(NULL != pElement);

	XTPXML::IXMLDOMNamedNodeMapPtr pAttributes = pElement->attributes;
	XTPXML::IXMLDOMNodePtr pAttrNode;
	while (pAttrNode = pAttributes->nextNode())
	{
		// Find attribute property object.
		_bstr_t attrName	 = pAttrNode->nodeName;
		LPCWSTR lpszAttrName = attrName;

		// Exclude 'xmlns' and all attibutes that prefixed with
		// namespace different from 'x:'.
		if (0 == wcscmp(lpszAttrName, L"xmlns") || !IsXmlNamespaceSupported(lpszAttrName))
		{
			continue;
		}

		CXTPMarkupDependencyProperty* pProperty = FindProperty(pObject->GetType(), attrName);
		if (NULL == pProperty)
		{
			ThrowBuilderException(
				FormatString(_T("The property '%ls' does not exist in XML namespace"), attrName));
		}

		// Load attribute content.
		_bstr_t attrValue = pAttrNode->text;

		CXTPMarkupAutoPtr pContentObject;
		LPCWSTR lpszAttrValue = NULL;
		if (0 < attrValue.length())
		{
			lpszAttrValue = attrValue;
			if (L'{' == lpszAttrValue[0])
			{
				if (L'}' == lpszAttrValue[1])
				{
					lpszAttrValue += 2;
				}
				else
				{
					pContentObject.Assign(ResolveMarkupExtension(lpszAttrValue + 1));
				}
			}
		}

		BOOL bNotifyOnLoad = FALSE;
		if (NULL == pContentObject.m_pObject)
		{
			pContentObject.Assign(CXTPMarkupString::CreateValue(lpszAttrValue));
			bNotifyOnLoad = TRUE;
		}

		pObject->SetPropertyObject(this, pProperty, pContentObject.m_pObject);

		if (bNotifyOnLoad)
		{
			pContentObject.m_pObject->OnLoaded(this);
		}
	}
}

BOOL CXTPMarkupBuilder::NeedPreserveXmlNodeWhitespaces(XTPXML::IXMLDOMNode* pNode) const
{
	ASSERT(NULL != pNode);

	BOOL bPreserve = FALSE;

	// Traversing gives better performance on avarage then getNamedItem
	XTPXML::IXMLDOMNamedNodeMapPtr pAttributes = pNode->attributes;
	XTPXML::IXMLDOMNodePtr pAttribute;
	while (pAttribute = pAttributes->nextNode())
	{
		if (0 == wcscmp(pAttribute->nodeName, L"xml:space"))
		{
			bPreserve = (0 == wcscmp(pAttribute->text, L"preserve"));
			break;
		}
	}

	return bPreserve;
}

void CXTPMarkupBuilder::LoadMarkupObjectTextContent(CXTPMarkupObject* pObject,
													XTPXML::IXMLDOMNode* pContent,
													BOOL bPreformatted)
{
	ASSERT(NULL != pObject);
	ASSERT(NULL != pContent);

	XTPXML::IXMLDOMTextPtr pText = pContent;
	_bstr_t nodeText			 = pText->data;
	if (0 < nodeText.length())
	{
		LPCWSTR lpszContent = nodeText;
		CArray<WCHAR, WCHAR> textBuffer;
		if (!bPreformatted)
		{
			BOOL bPreserveWhitespaces = NeedPreserveXmlNodeWhitespaces(pContent->parentNode);
			if (!bPreserveWhitespaces)
			{
				lpszContent = NormalizeXmlContentText(lpszContent, textBuffer,
													  XTPToIntChecked(nodeText.length()),
													  pContent->previousSibling,
													  pContent->nextSibling);
			}
		}

		BOOL bAddContent = (L'\0' != *lpszContent);

		if (bAddContent && IsXmlWhitespaceString(lpszContent))
		{
			if (pObject->HasContentObject() && pObject->AllowWhiteSpaceContent())
			{
				lpszContent = L" ";
			}
			else
			{
				bAddContent = FALSE;
			}
		}

		if (bAddContent)
		{
			CXTPMarkupAutoPtr pContentObject = CXTPMarkupString::CreateValue(lpszContent);
			if (NULL == pContentObject.m_pObject)
			{
				ThrowBuilderException(
					FormatString(_T("Failed to load '%ls' object content"),
								 reinterpret_cast<LPCTSTR>(pObject->GetTagName())));
			}

			pObject->SetContentObject(this, pContentObject.m_pObject);
			pContentObject.m_pObject->OnLoaded(this);
		}
	}
}

void CXTPMarkupBuilder::LoadMarkupObjectChild(CXTPMarkupObject* pObject,
											  XTPXML::IXMLDOMNode* pChild)
{
	ASSERT(NULL != pObject);
	ASSERT(NULL != pChild);

	XTPXML::IXMLDOMElementPtr pChildElement = pChild;
	CXTPMarkupAutoPtr pContentObject		= LoadXmlElement(pObject, pChildElement);
	if (NULL != pContentObject.m_pObject)
	{
		if (pContentObject->IsKindOf(MARKUP_TYPE(CXTPMarkupObjectProperty)))
		{
			CXTPMarkupObjectProperty* pObjectProperty = static_cast<CXTPMarkupObjectProperty*>(
				pContentObject.m_pObject);
			if (NULL == pObjectProperty->m_pContent)
			{
				ThrowBuilderException(FormatString(_T("'%ls' property does not have a value"),
												   reinterpret_cast<LPCTSTR>(
													   pObjectProperty->m_pProperty->GetName())));
			}

			pObject->SetPropertyObject(this, pObjectProperty->m_pProperty,
									   pObjectProperty->m_pContent);
		}
		else if (pContentObject->IsKindOf(MARKUP_TYPE(CXTPMarkupExtension)))
		{
			pObject->AddExtension(MARKUP_STATICCAST(CXTPMarkupExtension, pContentObject.m_pObject));
		}
		else
		{
			pObject->SetContentObject(this, pContentObject.m_pObject);
		}
	}
}

CXTPMarkupObject* CXTPMarkupBuilder::LoadXmlElement(CXTPMarkupObject* pParent,
													XTPXML::IXMLDOMElement* pElement)
{
	ASSERT(NULL != pElement);

	CXTPMarkupAutoPtr pObject = NULL;
	_bstr_t tagName			  = pElement->tagName;
	if (IsXmlNamespaceSupported(tagName))
	{
		pObject.Assign(CreateObject(tagName, pElement, pParent));
		if (NULL == pObject.m_pObject)
		{
#ifdef UNICODE
			ThrowBuilderException(FormatString(_T("The tag '%ls' does not exist in XML namespace"),
											   (LPCWSTR)tagName));
#else
			ThrowBuilderException(FormatString(_T("The tag '%s' does not exist in XML namespace"),
											   XTP_CW2CA(tagName)));
#endif
		}

		m_arrObjects.AddTail(pObject.m_pObject);

		try
		{
			pObject.m_pObject->SetBeingLoaded();

			LoadMarkupObjectAttributes(pObject.m_pObject, pElement);

			XTPXML::IXMLDOMNodeListPtr pChildNodes = pElement->childNodes;
			XTPXML::IXMLDOMNodePtr pChildNode;
			while (pChildNode = pChildNodes->nextNode())
			{
				XTPXML::DOMNodeType nodeType = pChildNode->nodeType;
				switch (nodeType)
				{
					case XTPXML::NODE_CDATA_SECTION:
						LoadMarkupObjectTextContent(pObject.m_pObject, pChildNode, TRUE);
						break;
					case XTPXML::NODE_TEXT:
						LoadMarkupObjectTextContent(pObject.m_pObject, pChildNode, FALSE);
						break;
					case XTPXML::NODE_ELEMENT:
						LoadMarkupObjectChild(pObject.m_pObject, pChildNode);
						break;
				}
			}

			pObject.m_pObject->SetBeingLoaded(FALSE);
			pObject.m_pObject->OnLoaded(this);

			m_arrObjects.RemoveTail();
		}
		catch (...)
		{
			pObject.m_pObject->SetBeingLoaded(FALSE);
			m_arrObjects.RemoveTail();
			throw;
		}

		pObject.AddRef();
	}
	else
	{
#ifdef UNICODE
		TRACE1("Markup tag skipped: %ls\n", (LPCWSTR)tagName);
#else
		TRACE1("Markup tag skipped: %s\n", XTP_CW2CA(tagName));
#endif
	}

	return pObject.m_pObject;
}

CXTPMarkupObject* CXTPMarkupBuilder::ParseObject(CXTPMarkupParser* pParser)
{
	ASSERT(NULL != pParser);

	CXTPMarkupObject* pRootObject = NULL;

	m_strLastError.Empty();

	if (pParser->GetXmlDocument())
	{
		try
		{
			TRY
			{
				XTPXML::IXMLDOMElementPtr pDocumentElement =
					pParser->GetXmlDocument()->documentElement;
				pRootObject = LoadXmlElement(NULL, pDocumentElement);
			}
			CATCH(CXTPMarkupBuilderException, e)
			{
				TCHAR szErrorMessage[512];
				UINT nHelpContext;
				if (e->GetErrorMessage(szErrorMessage, 512, &nHelpContext))
				{
					m_strLastError = szErrorMessage;
					TRACE(m_strLastError + _T("\n"));
				}

				MARKUP_RELEASE(pRootObject);
			}
			END_CATCH
		}
		catch (const _com_error& ex)
		{
			m_strLastError.Format(_T("COM Error [0x%08X]: %s"), ex.Error(), ex.ErrorMessage());
			TRACE(m_strLastError + _T("\n"));
			MARKUP_RELEASE(pRootObject);
		}
		catch (...)
		{
			MARKUP_RELEASE(pRootObject);
			throw;
		}
	}
	else
	{
		m_strLastError = pParser->FormatErrorMessage();
		TRACE(m_strLastError + _T("\n"));
	}

	return pRootObject;
}

CXTPMarkupUIElement* CXTPMarkupBuilder::Parse(CXTPMarkupParser* pParser)
{
	ASSERT(NULL != pParser);

	CXTPMarkupUIElement* pRootUIElement = NULL;
	m_strLastError.Empty();

	if (pParser->GetXmlDocument())
	{
		try
		{
			TRY
			{
				m_pMarkupContext->ResetScriptEngine();

				XTPXML::IXMLDOMElementPtr pDocumentElement =
					pParser->GetXmlDocument()->documentElement;
				CXTPMarkupObject* pRootObject = LoadXmlElement(NULL, pDocumentElement);
				if (NULL == pRootObject || !pRootObject->IsKindOf(MARKUP_TYPE(CXTPMarkupUIElement)))
				{
					MARKUP_RELEASE(pRootObject);
					ThrowBuilderException(_T("Root element have to be 'CXTPMarkupUIElement' type"));
				}

				pRootUIElement = static_cast<CXTPMarkupUIElement*>(pRootObject);
				pRootUIElement->Apply();

				m_pMarkupContext->RegisterScriptNamedObjects(this, pRootUIElement);
				m_pMarkupContext->RunScriptEngine(this);
			}
			CATCH(CXTPMarkupBuilderException, e)
			{
				TCHAR szErrorMessage[512];
				UINT nHelpContext;
				if (e->GetErrorMessage(szErrorMessage, 512, &nHelpContext))
				{
					m_strLastError = szErrorMessage;
					TRACE(m_strLastError + _T("\n"));
				}

				MARKUP_RELEASE(pRootUIElement);
			}
			END_CATCH
		}
		catch (const _com_error& ex)
		{
			m_strLastError.Format(_T("COM Error [0x%08X]: %s"), ex.Error(), ex.ErrorMessage());
			TRACE(m_strLastError + _T("\n"));
			MARKUP_RELEASE(pRootUIElement);
		}
		catch (...)
		{
			MARKUP_RELEASE(pRootUIElement);
			throw;
		}
	}
	else if (S_OK != pParser->GetErrorInfo().nCode)
	{
		m_strLastError = pParser->FormatErrorMessage();
		TRACE(m_strLastError + _T("\n"));
	}

	return pRootUIElement;
}

BOOL CXTPMarkupBuilder::ColorStringToKnownColor(LPCWSTR lpszValue, COLORREF& clr)
{
	ASSERT(NULL != lpszValue);

	switch (XTP_CHARTOUPPERW(*lpszValue++))
	{
		case L'A':
			switch (XTP_CHARTOUPPERW(*lpszValue++))
			{
				case L'L':
					if (_wcsicmp(lpszValue, L"ICEBLUE") == 0)
					{
						clr = 0xfffff8f0;
						return TRUE;
					}
					break;
				case L'N':
					if (_wcsicmp(lpszValue, L"TIQUEWHITE") == 0)
					{
						clr = 0xffd7ebfa;
						return TRUE;
					}
					break;
				case L'Q':
					if (_wcsicmp(lpszValue, L"UA") == 0)
					{
						clr = 0xffffff00;
						return TRUE;
					}
					if (_wcsicmp(lpszValue, L"UAMARINE") == 0)
					{
						clr = 0xffd4ff7f;
						return TRUE;
					}
					break;
				case L'Z':
					if (_wcsicmp(lpszValue, L"URE") == 0)
					{
						clr = 0xfffffff0;
						return TRUE;
					}
					break;
			}
			break;
		case L'B':
			switch (XTP_CHARTOUPPERW(*lpszValue++))
			{
				case L'E':
					if (_wcsicmp(lpszValue, L"IGE") == 0)
					{
						clr = 0xffdcf5f5;
						return TRUE;
					}
					break;
				case L'I':
					if (_wcsicmp(lpszValue, L"SQUE") == 0)
					{
						clr = 0xffc4e4ff;
						return TRUE;
					}
					break;
				case L'L':
					if (_wcsicmp(lpszValue, L"ACK") == 0)
					{
						clr = 0xff000000;
						return TRUE;
					}
					if (_wcsicmp(lpszValue, L"ANCHEDALMOND") == 0)
					{
						clr = 0xffcdebff;
						return TRUE;
					}
					if (_wcsicmp(lpszValue, L"UE") == 0)
					{
						clr = 0xffff0000;
						return TRUE;
					}
					if (_wcsicmp(lpszValue, L"UEVIOLET") == 0)
					{
						clr = 0xffe22b8a;
						return TRUE;
					}
					break;
				case L'R':
					if (_wcsicmp(lpszValue, L"OWN") == 0)
					{
						clr = 0xff2a2aa5;
						return TRUE;
					}
					break;
				case L'U':
					if (_wcsicmp(lpszValue, L"RLYWOOD") == 0)
					{
						clr = 0xff87b8de;
						return TRUE;
					}
					break;
			}
			break;
		case L'C':
			switch (XTP_CHARTOUPPERW(*lpszValue++))
			{
				case L'A':
					if (_wcsicmp(lpszValue, L"DETBLUE") == 0)
					{
						clr = 0xffa09e5f;
						return TRUE;
					}
					break;
				case L'H':
					if (_wcsicmp(lpszValue, L"ARTREUSE") == 0)
					{
						clr = 0xff00ff7f;
						return TRUE;
					}
					if (_wcsicmp(lpszValue, L"OCOLATE") == 0)
					{
						clr = 0xff1e69d2;
						return TRUE;
					}
					break;
				case L'O':
					if (_wcsicmp(lpszValue, L"RAL") == 0)
					{
						clr = 0xff507fff;
						return TRUE;
					}
					if (_wcsicmp(lpszValue, L"RNFLOWERBLUE") == 0)
					{
						clr = 0xffed9564;
						return TRUE;
					}
					if (_wcsicmp(lpszValue, L"RNSILK") == 0)
					{
						clr = 0xffdcf8ff;
						return TRUE;
					}
					break;
				case L'R':
					if (_wcsicmp(lpszValue, L"IMSON") == 0)
					{
						clr = 0xff3c14dc;
						return TRUE;
					}
					break;
				case L'Y':
					if (_wcsicmp(lpszValue, L"AN") == 0)
					{
						clr = 0xffffff00;
						return TRUE;
					}
					break;
			}
			break;
		case L'D':
			if ((L'A' == lpszValue[0] || L'a' == lpszValue[0])
				&& (L'R' == lpszValue[1] || L'r' == lpszValue[1])
				&& (L'K' == lpszValue[2] || L'k' == lpszValue[2]))
			{
				lpszValue += 3;
				switch (XTP_CHARTOUPPERW(*lpszValue++))
				{
					case L'B':
						if (_wcsicmp(lpszValue, L"LUE") == 0)
						{
							clr = 0xff8b0000;
							return TRUE;
						}
						break;
					case L'C':
						if (_wcsicmp(lpszValue, L"YAN") == 0)
						{
							clr = 0xff8b8b00;
							return TRUE;
						}
						break;
					case L'G':
						if (_wcsicmp(lpszValue, L"OLDENROD") == 0)
						{
							clr = 0xff0b86b8;
							return TRUE;
						}
						if (_wcsicmp(lpszValue, L"RAY") == 0)
						{
							clr = 0xffa9a9a9;
							return TRUE;
						}
						if (_wcsicmp(lpszValue, L"REEN") == 0)
						{
							clr = 0xff006400;
							return TRUE;
						}
						break;
					case L'K':
						if (_wcsicmp(lpszValue, L"HAKI") == 0)
						{
							clr = 0xff6bb7bd;
							return TRUE;
						}
						break;
					case L'M':
						if (_wcsicmp(lpszValue, L"AGENTA") == 0)
						{
							clr = 0xff8b008b;
							return TRUE;
						}
						break;
					case L'O':
						if (_wcsicmp(lpszValue, L"LIVEGREEN") == 0)
						{
							clr = 0xff2f6b55;
							return TRUE;
						}
						if (_wcsicmp(lpszValue, L"RANGE") == 0)
						{
							clr = 0xff008cff;
							return TRUE;
						}
						if (_wcsicmp(lpszValue, L"RCHID") == 0)
						{
							clr = 0xffcc3299;
							return TRUE;
						}
						break;
					case L'R':
						if (_wcsicmp(lpszValue, L"ED") == 0)
						{
							clr = 0xff00008b;
							return TRUE;
						}
						break;
					case L'S':
						if (_wcsicmp(lpszValue, L"ALMON") == 0)
						{
							clr = 0xff7a96e9;
							return TRUE;
						}
						if (_wcsicmp(lpszValue, L"EAGREEN") == 0)
						{
							clr = 0xff8fbc8f;
							return TRUE;
						}
						if (_wcsicmp(lpszValue, L"LATEBLUE") == 0)
						{
							clr = 0xff8b3d48;
							return TRUE;
						}
						if (_wcsicmp(lpszValue, L"LATEGRAY") == 0)
						{
							clr = 0xff4f4f2f;
							return TRUE;
						}
						break;
					case L'T':
						if (_wcsicmp(lpszValue, L"URQUOISE") == 0)
						{
							clr = 0xffd1ce00;
							return TRUE;
						}
						break;
					case L'V':
						if (_wcsicmp(lpszValue, L"IOLET") == 0)
						{
							clr = 0xffd30094;
							return TRUE;
						}
						break;
				}
				break;
			}

			switch (XTP_CHARTOUPPERW(*lpszValue++))
			{
				case L'E':
					if (_wcsicmp(lpszValue, L"EPPINK") == 0)
					{
						clr = 0xff9314ff;
						return TRUE;
					}
					if (_wcsicmp(lpszValue, L"EPSKYBLUE") == 0)
					{
						clr = 0xffffbf00;
						return TRUE;
					}
					break;
				case L'I':
					if (_wcsicmp(lpszValue, L"MGRAY") == 0)
					{
						clr = 0xff696969;
						return TRUE;
					}
					break;
				case L'O':
					if (_wcsicmp(lpszValue, L"DGERBLUE") == 0)
					{
						clr = 0xffff901e;
						return TRUE;
					}
					break;
			}
			break;
		case L'F':
			switch (XTP_CHARTOUPPERW(*lpszValue++))
			{
				case L'I':
					if (_wcsicmp(lpszValue, L"REBRICK") == 0)
					{
						clr = 0xff2222b2;
						return TRUE;
					}
					break;
				case L'L':
					if (_wcsicmp(lpszValue, L"ORALWHITE") == 0)
					{
						clr = 0xfff0faff;
						return TRUE;
					}
					break;
				case L'O':
					if (_wcsicmp(lpszValue, L"RESTGREEN") == 0)
					{
						clr = 0xff228b22;
						return TRUE;
					}
					break;
				case L'U':
					if (_wcsicmp(lpszValue, L"CHSIA") == 0)
					{
						clr = 0xffff00ff;
						return TRUE;
					}
					break;
			}
			break;
		case L'G':
			switch (XTP_CHARTOUPPERW(*lpszValue++))
			{
				case L'A':
					if (_wcsicmp(lpszValue, L"INSBORO") == 0)
					{
						clr = 0xffdcdcdc;
						return TRUE;
					}
					break;
				case L'H':
					if (_wcsicmp(lpszValue, L"OSTWHITE") == 0)
					{
						clr = 0xfffff8f8;
						return TRUE;
					}
					break;
				case L'O':
					if (_wcsicmp(lpszValue, L"LD") == 0)
					{
						clr = 0xff00d7ff;
						return TRUE;
					}
					if (_wcsicmp(lpszValue, L"LDENROD") == 0)
					{
						clr = 0xff20a5da;
						return TRUE;
					}
					break;
				case L'R':
					if (_wcsicmp(lpszValue, L"AY") == 0)
					{
						clr = 0xff808080;
						return TRUE;
					}
					if (_wcsicmp(lpszValue, L"EEN") == 0)
					{
						clr = 0xff008000;
						return TRUE;
					}
					if (_wcsicmp(lpszValue, L"EENYELLOW") == 0)
					{
						clr = 0xff2fffad;
						return TRUE;
					}
					break;
			}
			break;
		case L'H':
			if (_wcsicmp(lpszValue, L"ONEYDEW") == 0)
			{
				clr = 0xfff0fff0;
				return TRUE;
			}
			if (_wcsicmp(lpszValue, L"OTPINK") == 0)
			{
				clr = 0xffb469ff;
				return TRUE;
			}
			break;
		case L'I':
			if (_wcsicmp(lpszValue, L"NDIANRED") == 0)
			{
				clr = 0xff5c5ccd;
				return TRUE;
			}
			if (_wcsicmp(lpszValue, L"NDIGO") == 0)
			{
				clr = 0xff82004b;
				return TRUE;
			}
			if (_wcsicmp(lpszValue, L"VORY") == 0)
			{
				clr = 0xfff0ffff;
				return TRUE;
			}
			break;
		case L'K':
			if (_wcsicmp(lpszValue, L"HAKI") == 0)
			{
				clr = 0xff8ce6f0;
				return TRUE;
			}
			break;
		case L'L':
			if ((L'I' == lpszValue[0] || L'i' == lpszValue[0])
				&& (L'G' == lpszValue[1] || L'g' == lpszValue[1])
				&& (L'H' == lpszValue[2] || L'h' == lpszValue[2])
				&& (L'T' == lpszValue[3] || L't' == lpszValue[3]))
			{
				lpszValue += 4;

				switch (XTP_CHARTOUPPERW(*lpszValue++))
				{
					case L'B':
						if (_wcsicmp(lpszValue, L"LUE") == 0)
						{
							clr = 0xffe6d8ad;
							return TRUE;
						}
						break;
					case L'C':
						if (_wcsicmp(lpszValue, L"ORAL") == 0)
						{
							clr = 0xff8080f0;
							return TRUE;
						}
						if (_wcsicmp(lpszValue, L"YAN") == 0)
						{
							clr = 0xffffffe0;
							return TRUE;
						}
						break;
					case L'G':
						if (_wcsicmp(lpszValue, L"OLDENRODYELLOW") == 0)
						{
							clr = 0xffd2fafa;
							return TRUE;
						}
						if (_wcsicmp(lpszValue, L"RAY") == 0)
						{
							clr = 0xffd3d3d3;
							return TRUE;
						}
						if (_wcsicmp(lpszValue, L"REEN") == 0)
						{
							clr = 0xff90ee90;
							return TRUE;
						}
						break;
					case L'P':
						if (_wcsicmp(lpszValue, L"INK") == 0)
						{
							clr = 0xffc1b6ff;
							return TRUE;
						}
						break;
					case L'S':
						if (_wcsicmp(lpszValue, L"ALMON") == 0)
						{
							clr = 0xff7aa0ff;
							return TRUE;
						}
						if (_wcsicmp(lpszValue, L"EAGREEN") == 0)
						{
							clr = 0xffaab220;
							return TRUE;
						}
						if (_wcsicmp(lpszValue, L"KYBLUE") == 0)
						{
							clr = 0xffface87;
							return TRUE;
						}
						if (_wcsicmp(lpszValue, L"LATEGRAY") == 0)
						{
							clr = 0xff998877;
							return TRUE;
						}
						if (_wcsicmp(lpszValue, L"TEELBLUE") == 0)
						{
							clr = 0xffdec4b0;
							return TRUE;
						}
						break;
					case L'Y':
						if (_wcsicmp(lpszValue, L"ELLOW") == 0)
						{
							clr = 0xffe0ffff;
							return TRUE;
						}
						break;
				}
				break;
			}

			switch (XTP_CHARTOUPPERW(*lpszValue++))
			{
				case L'A':
					if (_wcsicmp(lpszValue, L"VENDER") == 0)
					{
						clr = 0xfffae6e6;
						return TRUE;
					}
					if (_wcsicmp(lpszValue, L"VENDERBLUSH") == 0)
					{
						clr = 0xfff5f0ff;
						return TRUE;
					}
					if (_wcsicmp(lpszValue, L"WNGREEN") == 0)
					{
						clr = 0xff00fc7c;
						return TRUE;
					}
					break;
				case L'E':
					if (_wcsicmp(lpszValue, L"MONCHIFFON") == 0)
					{
						clr = 0xffcdfaff;
						return TRUE;
					}
					break;
				case L'I':
					if (_wcsicmp(lpszValue, L"ME") == 0)
					{
						clr = 0xff00ff00;
						return TRUE;
					}
					if (_wcsicmp(lpszValue, L"MEGREEN") == 0)
					{
						clr = 0xff32cd32;
						return TRUE;
					}
					if (_wcsicmp(lpszValue, L"NEN") == 0)
					{
						clr = 0xffe6f0fa;
						return TRUE;
					}
					break;
			}
			break;
		case L'M':
			if ((L'E' == lpszValue[0] || L'e' == lpszValue[0])
				&& (L'D' == lpszValue[1] || L'd' == lpszValue[1])
				&& (L'I' == lpszValue[2] || L'i' == lpszValue[2])
				&& (L'U' == lpszValue[3] || L'u' == lpszValue[3])
				&& (L'M' == lpszValue[4] || L'm' == lpszValue[4]))
			{
				lpszValue += 5;

				switch (XTP_CHARTOUPPERW(*lpszValue++))
				{
					case L'A':
						if (_wcsicmp(lpszValue, L"QUAMARINE") == 0)
						{
							clr = 0xffaacd66;
							return TRUE;
						}
						break;
					case L'B':
						if (_wcsicmp(lpszValue, L"LUE") == 0)
						{
							clr = 0xffcd0000;
							return TRUE;
						}
						break;
					case L'O':
						if (_wcsicmp(lpszValue, L"RCHID") == 0)
						{
							clr = 0xffd355ba;
							return TRUE;
						}
						break;
					case L'P':
						if (_wcsicmp(lpszValue, L"URPLE") == 0)
						{
							clr = 0xffdb7093;
							return TRUE;
						}
						break;
					case L'S':
						if (_wcsicmp(lpszValue, L"EAGREEN") == 0)
						{
							clr = 0xff71b33c;
							return TRUE;
						}
						if (_wcsicmp(lpszValue, L"LATEBLUE") == 0)
						{
							clr = 0xffee687b;
							return TRUE;
						}
						if (_wcsicmp(lpszValue, L"PRINGGREEN") == 0)
						{
							clr = 0xff9afa00;
							return TRUE;
						}
						break;
					case L'T':
						if (_wcsicmp(lpszValue, L"URQUOISE") == 0)
						{
							clr = 0xffccd148;
							return TRUE;
						}
						break;
					case L'V':
						if (_wcsicmp(lpszValue, L"IOLETRED") == 0)
						{
							clr = 0xff8515c7;
							return TRUE;
						}
						break;
				}
				break;
			}

			switch (XTP_CHARTOUPPERW(*lpszValue++))
			{
				case L'A':
					if (_wcsicmp(lpszValue, L"GENTA") == 0)
					{
						clr = 0xffff00ff;
						return TRUE;
					}
					if (_wcsicmp(lpszValue, L"ROON") == 0)
					{
						clr = 0xff000080;
						return TRUE;
					}
					break;
				case L'I':
					if (_wcsicmp(lpszValue, L"DNIGHTBLUE") == 0)
					{
						clr = 0xff701919;
						return TRUE;
					}
					if (_wcsicmp(lpszValue, L"NTCREAM") == 0)
					{
						clr = 0xfffafff5;
						return TRUE;
					}
					if (_wcsicmp(lpszValue, L"STYROSE") == 0)
					{
						clr = 0xffe1e4ff;
						return TRUE;
					}
					break;
				case L'O':
					if (_wcsicmp(lpszValue, L"CCASIN") == 0)
					{
						clr = 0xffb5e4ff;
						return TRUE;
					}
					break;
			}
			break;
		case L'N':
			if (_wcsicmp(lpszValue, L"AVAJOWHITE") == 0)
			{
				clr = 0xffaddeff;
				return TRUE;
			}
			if (_wcsicmp(lpszValue, L"AVY") == 0)
			{
				clr = 0xff800000;
				return TRUE;
			}
			break;
		case L'O':
			switch (XTP_CHARTOUPPERW(*lpszValue++))
			{
				case L'L':
					if (_wcsicmp(lpszValue, L"DLACE") == 0)
					{
						clr = 0xffe6f5fd;
						return TRUE;
					}
					if (_wcsicmp(lpszValue, L"IVE") == 0)
					{
						clr = 0xff008080;
						return TRUE;
					}
					if (_wcsicmp(lpszValue, L"IVEDRAB") == 0)
					{
						clr = 0xff238e6b;
						return TRUE;
					}
					break;
				case L'R':
					if (_wcsicmp(lpszValue, L"ANGE") == 0)
					{
						clr = 0xff00a5ff;
						return TRUE;
					}
					if (_wcsicmp(lpszValue, L"ANGERED") == 0)
					{
						clr = 0xff0045ff;
						return TRUE;
					}
					if (_wcsicmp(lpszValue, L"CHID") == 0)
					{
						clr = 0xffd670da;
						return TRUE;
					}
					break;
			}
			break;
		case L'P':
			switch (XTP_CHARTOUPPERW(*lpszValue++))
			{
				case L'A':
					if (_wcsicmp(lpszValue, L"LEGOLDENROD") == 0)
					{
						clr = 0xffaae8ee;
						return TRUE;
					}
					if (_wcsicmp(lpszValue, L"LEGREEN") == 0)
					{
						clr = 0xff98fb98;
						return TRUE;
					}
					if (_wcsicmp(lpszValue, L"LETURQUOISE") == 0)
					{
						clr = 0xffeeeeaf;
						return TRUE;
					}
					if (_wcsicmp(lpszValue, L"LEVIOLETRED") == 0)
					{
						clr = 0xff9370db;
						return TRUE;
					}
					if (_wcsicmp(lpszValue, L"PAYAWHIP") == 0)
					{
						clr = 0xffd5efff;
						return TRUE;
					}
					break;
				case L'E':
					if (_wcsicmp(lpszValue, L"ACHPUFF") == 0)
					{
						clr = 0xffb9daff;
						return TRUE;
					}
					if (_wcsicmp(lpszValue, L"RU") == 0)
					{
						clr = 0xff3f85cd;
						return TRUE;
					}
					break;
				case L'I':
					if (_wcsicmp(lpszValue, L"NK") == 0)
					{
						clr = 0xffcbc0ff;
						return TRUE;
					}
					break;
				case L'L':
					if (_wcsicmp(lpszValue, L"UM") == 0)
					{
						clr = 0xffdda0dd;
						return TRUE;
					}
					break;
				case L'O':
					if (_wcsicmp(lpszValue, L"WDERBLUE") == 0)
					{
						clr = 0xffe6e0b0;
						return TRUE;
					}
					break;
				case L'U':
					if (_wcsicmp(lpszValue, L"RPLE") == 0)
					{
						clr = 0xff800080;
						return TRUE;
					}
					break;
			}
			break;
		case L'R':
			if (_wcsicmp(lpszValue, L"ED") == 0)
			{
				clr = 0xff0000ff;
				return TRUE;
			}
			if (_wcsicmp(lpszValue, L"OSYBROWN") == 0)
			{
				clr = 0xff8f8fbc;
				return TRUE;
			}
			if (_wcsicmp(lpszValue, L"OYALBLUE") == 0)
			{
				clr = 0xffe16941;
				return TRUE;
			}
			break;
		case L'S':
			switch (XTP_CHARTOUPPERW(*lpszValue++))
			{
				case L'A':
					if (_wcsicmp(lpszValue, L"DDLEBROWN") == 0)
					{
						clr = 0xff13458b;
						return TRUE;
					}
					if (_wcsicmp(lpszValue, L"LMON") == 0)
					{
						clr = 0xff7280fa;
						return TRUE;
					}
					if (_wcsicmp(lpszValue, L"NDYBROWN") == 0)
					{
						clr = 0xff60a4f4;
						return TRUE;
					}
				case L'E':
					if (_wcsicmp(lpszValue, L"AGREEN") == 0)
					{
						clr = 0xff578b2e;
						return TRUE;
					}
					if (_wcsicmp(lpszValue, L"ASHELL") == 0)
					{
						clr = 0xffeef5ff;
						return TRUE;
					}
					break;
				case L'I':
					if (_wcsicmp(lpszValue, L"ENNA") == 0)
					{
						clr = 0xff2d52a0;
						return TRUE;
					}
					if (_wcsicmp(lpszValue, L"LVER") == 0)
					{
						clr = 0xffc0c0c0;
						return TRUE;
					}
					break;
				case L'K':
					if (_wcsicmp(lpszValue, L"YBLUE") == 0)
					{
						clr = 0xffebce87;
						return TRUE;
					}
					break;
				case L'L':
					if (_wcsicmp(lpszValue, L"ATEBLUE") == 0)
					{
						clr = 0xffcd5a6a;
						return TRUE;
					}
					if (_wcsicmp(lpszValue, L"ATEGRAY") == 0)
					{
						clr = 0xff908070;
						return TRUE;
					}
					break;
				case L'N':
					if (_wcsicmp(lpszValue, L"OW") == 0)
					{
						clr = 0xfffafaff;
						return TRUE;
					}
					break;
				case L'P':
					if (_wcsicmp(lpszValue, L"RINGGREEN") == 0)
					{
						clr = 0xff7fff00;
						return TRUE;
					}
					break;
				case L'T':
					if (_wcsicmp(lpszValue, L"EELBLUE") == 0)
					{
						clr = 0xffb48246;
						return TRUE;
					}
					break;
			}
			break;
		case L'T':
			switch (XTP_CHARTOUPPERW(*lpszValue++))
			{
				case L'A':
					if (_wcsicmp(lpszValue, L"N") == 0)
					{
						clr = 0xff8cb4d2;
						return TRUE;
					}
					break;
				case L'E':
					if (_wcsicmp(lpszValue, L"AL") == 0)
					{
						clr = 0xff808000;
						return TRUE;
					}
					break;
				case L'H':
					if (_wcsicmp(lpszValue, L"ISTLE") == 0)
					{
						clr = 0xffd8bfd8;
						return TRUE;
					}
					break;
				case L'O':
					if (_wcsicmp(lpszValue, L"MATO") == 0)
					{
						clr = 0xff4763ff;
						return TRUE;
					}
					break;
				case L'R':
					if (_wcsicmp(lpszValue, L"ANSPARENT") == 0)
					{
						clr = 0x00ffffff;
						return TRUE;
					}
					break;
				case L'U':
					if (_wcsicmp(lpszValue, L"RQUOISE") == 0)
					{
						clr = 0xffd0e040;
						return TRUE;
					}
					break;
			}
			break;
		case L'V':
			if (_wcsicmp(lpszValue, L"IOLET") == 0)
			{
				clr = 0xffee82ee;
				return TRUE;
			}
			break;
		case L'W':
			if (_wcsicmp(lpszValue, L"HEAT") == 0)
			{
				clr = 0xffb3def5;
				return TRUE;
			}
			if (_wcsicmp(lpszValue, L"HITE") == 0)
			{
				clr = 0xffffffff;
				return TRUE;
			}
			if (_wcsicmp(lpszValue, L"HITESMOKE") == 0)
			{
				clr = 0xfff5f5f5;
				return TRUE;
			}
			break;
		case L'Y':
			if (_wcsicmp(lpszValue, L"ELLOW") == 0)
			{
				clr = 0xff00ffff;
				return TRUE;
			}
			if (_wcsicmp(lpszValue, L"ELLOWGREEN") == 0)
			{
				clr = 0xff32cd9a;
				return TRUE;
			}
			break;
	}

	return FALSE;
}

CXTPMarkupObject*
	CXTPMarkupBuilder::ConvertBool(CXTPMarkupBuilder* /*pBuilder*/, CXTPMarkupObject* pObject,
								   CXTPMarkupDependencyProperty* /*pAssociatedProperty = NULL*/)
{
	if (IsStringObject(pObject))
	{
		const CXTPMarkupString* pString = static_cast<const CXTPMarkupString*>(pObject);

		if (XTP_MARKUP_STRING_CONSTANT(True).IsEqual(pString, FALSE))
			return CXTPMarkupBool::CreateTrueValue();

		if (XTP_MARKUP_STRING_CONSTANT(False).IsEqual(pString, FALSE))
			return CXTPMarkupBool::CreateFalseValue();
	}

	return NULL;
}

CXTPMarkupObject* CXTPMarkupBuilder::ConvertTextDecorations(
	CXTPMarkupBuilder* /*pBuilder*/, CXTPMarkupObject* pObject,
	CXTPMarkupDependencyProperty* /*pAssociatedProperty = NULL*/)
{
	if (IsStringObject(pObject))
	{
		const CXTPMarkupString* pString = static_cast<const CXTPMarkupString*>(pObject);

		if (XTP_MARKUP_STRING_CONSTANT(Underline).IsEqual(pString, FALSE))
			return CXTPMarkupEnum::CreateValue(1);

		if (XTP_MARKUP_STRING_CONSTANT(Strikethrough).IsEqual(pString, FALSE))
			return CXTPMarkupEnum::CreateValue(2);

		if (0 == pString->GetLength())
			return CXTPMarkupEnum::CreateValue(0);
	}

	return NULL;
}

CXTPMarkupObject* CXTPMarkupBuilder::ConvertFontWeight(
	CXTPMarkupBuilder* /*pBuilder*/, CXTPMarkupObject* pObject,
	CXTPMarkupDependencyProperty* /*pAssociatedProperty = NULL*/)
{
	if (IsStringObject(pObject))
	{
		const CXTPMarkupString* pString = static_cast<const CXTPMarkupString*>(pObject);
		switch (pString->GetLength())
		{
			case 4:
				if (XTP_MARKUP_STRING_CONSTANT(Bold).IsEqual(pString, FALSE))
					return CXTPMarkupEnum::CreateValue(FW_BOLD);

				if (XTP_MARKUP_STRING_CONSTANT(Thin).IsEqual(pString, FALSE))
					return CXTPMarkupEnum::CreateValue(FW_THIN);

				return NULL;

			case 5:
				if (XTP_MARKUP_STRING_CONSTANT(Light).IsEqual(pString, FALSE))
					return CXTPMarkupEnum::CreateValue(FW_LIGHT);

				if (XTP_MARKUP_STRING_CONSTANT(Black).IsEqual(pString, FALSE))
					return CXTPMarkupEnum::CreateValue(FW_BLACK);

				if (XTP_MARKUP_STRING_CONSTANT(Heavy).IsEqual(pString, FALSE))
					return CXTPMarkupEnum::CreateValue(FW_HEAVY);

				return NULL;

			case 6:
				if (XTP_MARKUP_STRING_CONSTANT(Normal).IsEqual(pString, FALSE))
					return CXTPMarkupEnum::CreateValue(FW_NORMAL);

				if (XTP_MARKUP_STRING_CONSTANT(Medium).IsEqual(pString, FALSE))
					return CXTPMarkupEnum::CreateValue(FW_MEDIUM);

				return NULL;

			case 7:
				if (XTP_MARKUP_STRING_CONSTANT(Regular).IsEqual(pString, FALSE))
					return CXTPMarkupEnum::CreateValue(FW_REGULAR);

				return NULL;

			case 8:
				if (XTP_MARKUP_STRING_CONSTANT(SemiBold).IsEqual(pString, FALSE))
					return CXTPMarkupEnum::CreateValue(FW_SEMIBOLD);

				if (XTP_MARKUP_STRING_CONSTANT(DemiBold).IsEqual(pString, FALSE))
					return CXTPMarkupEnum::CreateValue(FW_DEMIBOLD);

				return NULL;

			case 9:
				if (XTP_MARKUP_STRING_CONSTANT(ExtraBold).IsEqual(pString, FALSE))
					return CXTPMarkupEnum::CreateValue(FW_EXTRABOLD);

				if (XTP_MARKUP_STRING_CONSTANT(UltraBold).IsEqual(pString, FALSE))
					return CXTPMarkupEnum::CreateValue(FW_ULTRABOLD);

				return NULL;

			case 10:
				if (XTP_MARKUP_STRING_CONSTANT(ExtraLight).IsEqual(pString, FALSE))
					return CXTPMarkupEnum::CreateValue(FW_EXTRALIGHT);

				if (XTP_MARKUP_STRING_CONSTANT(ExtraBlack).IsEqual(pString, FALSE))
					return CXTPMarkupEnum::CreateValue(950);

				return NULL;
		}
	}

	return NULL;
}

CXTPMarkupObject* CXTPMarkupBuilder::ConvertFontQuality(
	CXTPMarkupBuilder* /*pBuilder*/, CXTPMarkupObject* pObject,
	CXTPMarkupDependencyProperty* /*pAssociatedProperty = NULL*/)
{
	if (IsStringObject(pObject))
	{
		const CXTPMarkupString* pString = static_cast<const CXTPMarkupString*>(pObject);

		if (XTP_MARKUP_STRING_CONSTANT(ClearType).IsEqual(pString, FALSE))
			return CXTPMarkupEnum::CreateValue(5);
	}

	return NULL;
}

CXTPMarkupObject*
	CXTPMarkupBuilder::ConvertLength(CXTPMarkupBuilder* pBuilder, CXTPMarkupObject* pObject,
									 CXTPMarkupDependencyProperty* pAssociatedProperty /*= NULL*/)
{
	ASSERT(NULL != pBuilder);
	ASSERT(NULL != pObject);

	if (IsStringObject(pObject))
	{
		const CXTPMarkupString* pString = static_cast<const CXTPMarkupString*>(pObject);

		if (XTP_MARKUP_STRING_CONSTANT(Auto).IsEqual(pString, FALSE))
			return new CXTPMarkupInt(INT_MAX);

		LPCWSTR lpszValue = *pString;
		int nLength		  = pString->GetLength();

		int nValue = _wtoi(lpszValue);
		if (nValue == 0)
			return new CXTPMarkupInt(nValue);

		if ((nLength > 2) && _wcsicmp(lpszValue + nLength - 2, L"pt") == 0)
		{
			nValue = MulDiv(nValue, CXTPDpi::DefaultDpi, 72);
		}
		else if ((nLength > 2) && _wcsicmp(lpszValue + nLength - 2, L"in") == 0)
		{
			nValue *= CXTPDpi::DefaultDpi;
		}
		else if ((nLength > 2) && _wcsicmp(lpszValue + nLength - 2, L"cm") == 0)
		{
			nValue = int((double)nValue * 37.79528);
		}
		else if ((nLength > 2) && _wcsicmp(lpszValue + nLength - 2, L"mm") == 0)
		{
			nValue = int((double)nValue * 3.779528);
		}

		if (NULL != pAssociatedProperty)
		{
			if (0
				!= (pAssociatedProperty->GetFlags()
					& CXTPMarkupPropertyMetadata::flagHorzDpiSensible))
			{
				nValue = pBuilder->GetMarkupContext()->ScaleX(nValue);
			}
			else if (0
					 != (pAssociatedProperty->GetFlags()
						 & CXTPMarkupPropertyMetadata::flagVertDpiSensible))
			{
				nValue = pBuilder->GetMarkupContext()->ScaleY(nValue);
			}
		}

		return new CXTPMarkupInt(nValue);
	}

	return NULL;
}

CXTPMarkupObject* CXTPMarkupBuilder::ConvertFontStyle(
	CXTPMarkupBuilder* /*pBuilder*/, CXTPMarkupObject* pObject,
	CXTPMarkupDependencyProperty* /*pAssociatedProperty = NULL*/)
{
	if (IsStringObject(pObject))
	{
		const CXTPMarkupString* pString = static_cast<const CXTPMarkupString*>(pObject);

		if (XTP_MARKUP_STRING_CONSTANT(Normal).IsEqual(pString, FALSE))
			return CXTPMarkupEnum::CreateValue(0);

		if (XTP_MARKUP_STRING_CONSTANT(Italic).IsEqual(pString, FALSE))
			return CXTPMarkupEnum::CreateValue(1);
	}

	return NULL;
}

//////////////////////////////////////////////////////////////////////////

CXTPMarkupObject* CXTPMarkupBuilder::ConvertVerticalAlignment(
	CXTPMarkupBuilder* /*pBuilder*/, CXTPMarkupObject* pObject,
	CXTPMarkupDependencyProperty* /*pAssociatedProperty = NULL*/)
{
	if (IsStringObject(pObject))
	{
		const CXTPMarkupString* pString = static_cast<const CXTPMarkupString*>(pObject);

		if (XTP_MARKUP_STRING_CONSTANT(Top).IsEqual(pString, FALSE))
			return CXTPMarkupEnum::CreateValue(xtpMarkupVerticalAlignmentTop);

		if (XTP_MARKUP_STRING_CONSTANT(Center).IsEqual(pString, FALSE))
			return CXTPMarkupEnum::CreateValue(xtpMarkupVerticalAlignmentCenter);

		if (XTP_MARKUP_STRING_CONSTANT(Bottom).IsEqual(pString, FALSE))
			return CXTPMarkupEnum::CreateValue(xtpMarkupVerticalAlignmentBottom);

		if (XTP_MARKUP_STRING_CONSTANT(Stretch).IsEqual(pString, FALSE))
			return CXTPMarkupEnum::CreateValue(xtpMarkupVerticalAlignmentStretch);
	}

	return NULL;
}

CXTPMarkupObject* CXTPMarkupBuilder::ConvertHorizontalAlignment(
	CXTPMarkupBuilder* /*pBuilder*/, CXTPMarkupObject* pObject,
	CXTPMarkupDependencyProperty* /*pAssociatedProperty = NULL*/)
{
	if (IsStringObject(pObject))
	{
		const CXTPMarkupString* pString = static_cast<const CXTPMarkupString*>(pObject);

		if (XTP_MARKUP_STRING_CONSTANT(Left).IsEqual(pString, FALSE))
			return CXTPMarkupEnum::CreateValue(xtpMarkupHorizontalAlignmentLeft);

		if (XTP_MARKUP_STRING_CONSTANT(Center).IsEqual(pString, FALSE))
			return CXTPMarkupEnum::CreateValue(xtpMarkupHorizontalAlignmentCenter);

		if (XTP_MARKUP_STRING_CONSTANT(Right).IsEqual(pString, FALSE))
			return CXTPMarkupEnum::CreateValue(xtpMarkupHorizontalAlignmentRight);

		if (XTP_MARKUP_STRING_CONSTANT(Stretch).IsEqual(pString, FALSE))
			return CXTPMarkupEnum::CreateValue(xtpMarkupHorizontalAlignmentStretch);
	}

	return NULL;
}

CXTPMarkupObject* CXTPMarkupBuilder::ConvertTextWrapping(
	CXTPMarkupBuilder* /*pBuilder*/, CXTPMarkupObject* pObject,
	CXTPMarkupDependencyProperty* /*pAssociatedProperty = NULL*/)
{
	if (IsStringObject(pObject))
	{
		const CXTPMarkupString* pString = static_cast<const CXTPMarkupString*>(pObject);

		if (XTP_MARKUP_STRING_CONSTANT(Wrap).IsEqual(pString, FALSE))
			return CXTPMarkupEnum::CreateValue(xtpMarkupTextWrap);

		if (XTP_MARKUP_STRING_CONSTANT(WrapWithOverflow).IsEqual(pString, FALSE))
			return CXTPMarkupEnum::CreateValue(xtpMarkupTextWrapWithOverflow);

		if (XTP_MARKUP_STRING_CONSTANT(NoWrap).IsEqual(pString, FALSE))
			return CXTPMarkupEnum::CreateValue(xtpMarkupTextNoWrap);
	}

	return NULL;
}

CXTPMarkupObject* CXTPMarkupBuilder::ConvertTextTrimming(
	CXTPMarkupBuilder* /*pBuilder*/, CXTPMarkupObject* pObject,
	CXTPMarkupDependencyProperty* /*pAssociatedProperty = NULL*/)
{
	if (IsStringObject(pObject))
	{
		const CXTPMarkupString* pString = static_cast<const CXTPMarkupString*>(pObject);

		if (XTP_MARKUP_STRING_CONSTANT(None).IsEqual(pString, FALSE))
			return CXTPMarkupEnum::CreateValue(xtpMarkupTextTrimmingNone);

		if (XTP_MARKUP_STRING_CONSTANT(CharacterEllipsis).IsEqual(pString, FALSE))
			return CXTPMarkupEnum::CreateValue(xtpMarkupTextTrimmingCharacterEllipsis);

		if (XTP_MARKUP_STRING_CONSTANT(WordEllipsis).IsEqual(pString, FALSE))
			return CXTPMarkupEnum::CreateValue(xtpMarkupTextTrimmingWordEllipsis);
	}

	return NULL;
}

CXTPMarkupObject* CXTPMarkupBuilder::ConvertTextAlignment(
	CXTPMarkupBuilder* /*pBuilder*/, CXTPMarkupObject* pObject,
	CXTPMarkupDependencyProperty* /*pAssociatedProperty = NULL*/)
{
	if (IsStringObject(pObject))
	{
		const CXTPMarkupString* pString = static_cast<const CXTPMarkupString*>(pObject);

		if (XTP_MARKUP_STRING_CONSTANT(Center).IsEqual(pString, FALSE))
			return CXTPMarkupEnum::CreateValue(xtpMarkupTextAlignmentCenter);

		if (XTP_MARKUP_STRING_CONSTANT(Left).IsEqual(pString, FALSE))
			return CXTPMarkupEnum::CreateValue(xtpMarkupTextAlignmentLeft);

		if (XTP_MARKUP_STRING_CONSTANT(Right).IsEqual(pString, FALSE))
			return CXTPMarkupEnum::CreateValue(xtpMarkupTextAlignmentRight);

		if (XTP_MARKUP_STRING_CONSTANT(Justify).IsEqual(pString, FALSE))
			return CXTPMarkupEnum::CreateValue(xtpMarkupTextAlignmentJustify);
	}

	return NULL;
}

CXTPMarkupObject* CXTPMarkupBuilder::ConvertOrientation(
	CXTPMarkupBuilder* /*pBuilder*/, CXTPMarkupObject* pObject,
	CXTPMarkupDependencyProperty* /*pAssociatedProperty = NULL*/)
{
	if (IsStringObject(pObject))
	{
		const CXTPMarkupString* pString = static_cast<const CXTPMarkupString*>(pObject);

		if (XTP_MARKUP_STRING_CONSTANT(Horizontal).IsEqual(pString, FALSE))
			return CXTPMarkupEnum::CreateValue(xtpMarkupOrientationHorizontal);

		if (XTP_MARKUP_STRING_CONSTANT(Vertical).IsEqual(pString, FALSE))
			return CXTPMarkupEnum::CreateValue(xtpMarkupOrientationVertical);
	}

	return NULL;
}

CXTPMarkupObject* CXTPMarkupBuilder::ConvertBaselineAlignment(
	CXTPMarkupBuilder* /*pBuilder*/, CXTPMarkupObject* pObject,
	CXTPMarkupDependencyProperty* /*pAssociatedProperty = NULL*/)
{
	if (IsStringObject(pObject))
	{
		const CXTPMarkupString* pString = static_cast<const CXTPMarkupString*>(pObject);

		if (XTP_MARKUP_STRING_CONSTANT(Top).IsEqual(pString, FALSE))
			return CXTPMarkupEnum::CreateValue(xtpMarkupBaselineTop);

		if (XTP_MARKUP_STRING_CONSTANT(Center).IsEqual(pString, FALSE))
			return CXTPMarkupEnum::CreateValue(xtpMarkupBaselineCenter);

		if (XTP_MARKUP_STRING_CONSTANT(Bottom).IsEqual(pString, FALSE))
			return CXTPMarkupEnum::CreateValue(xtpMarkupBaselineBottom);

		if (XTP_MARKUP_STRING_CONSTANT(Baseline).IsEqual(pString, FALSE))
			return CXTPMarkupEnum::CreateValue(xtpMarkupBaseline);

		if (XTP_MARKUP_STRING_CONSTANT(TextTop).IsEqual(pString, FALSE))
			return CXTPMarkupEnum::CreateValue(xtpMarkupBaselineTextTop);

		if (XTP_MARKUP_STRING_CONSTANT(TextBottom).IsEqual(pString, FALSE))
			return CXTPMarkupEnum::CreateValue(xtpMarkupBaselineTextBottom);

		if (XTP_MARKUP_STRING_CONSTANT(Subscript).IsEqual(pString, FALSE))
			return CXTPMarkupEnum::CreateValue(xtpMarkupBaselineSubscript);

		if (XTP_MARKUP_STRING_CONSTANT(Superscript).IsEqual(pString, FALSE))
			return CXTPMarkupEnum::CreateValue(xtpMarkupBaselineSuperscript);
	}

	return NULL;
}

CXTPMarkupObject*
	CXTPMarkupBuilder::ConvertCursor(CXTPMarkupBuilder* /*pBuilder*/, CXTPMarkupObject* pObject,
									 CXTPMarkupDependencyProperty* /*pAssociatedProperty = NULL*/)
{
	if (IsStringObject(pObject))
	{
		const CXTPMarkupString* pString = static_cast<const CXTPMarkupString*>(pObject);

		if (XTP_MARKUP_STRING_CONSTANT(None).IsEqual(pString, FALSE))
			return new CXTPMarkupInt(0);

		if (XTP_MARKUP_STRING_CONSTANT(No).IsEqual(pString, FALSE))
			return new CXTPMarkupInt((DWORD)(DWORD_PTR)IDC_NO);

		if (XTP_MARKUP_STRING_CONSTANT(Arrow).IsEqual(pString, FALSE))
			return new CXTPMarkupInt((DWORD)(DWORD_PTR)IDC_ARROW);

		if (XTP_MARKUP_STRING_CONSTANT(AppStarting).IsEqual(pString, FALSE))
			return new CXTPMarkupInt((DWORD)(DWORD_PTR)IDC_APPSTARTING);

		if (XTP_MARKUP_STRING_CONSTANT(Cross).IsEqual(pString, FALSE))
			return new CXTPMarkupInt((DWORD)(DWORD_PTR)IDC_CROSS);

		if (XTP_MARKUP_STRING_CONSTANT(IBeam).IsEqual(pString, FALSE))
			return new CXTPMarkupInt((DWORD)(DWORD_PTR)IDC_IBEAM);

		if (XTP_MARKUP_STRING_CONSTANT(SizeAll).IsEqual(pString, FALSE))
			return new CXTPMarkupInt((DWORD)(DWORD_PTR)IDC_SIZEALL);

		if (XTP_MARKUP_STRING_CONSTANT(SizeNESW).IsEqual(pString, FALSE))
			return new CXTPMarkupInt((DWORD)(DWORD_PTR)IDC_SIZENESW);

		if (XTP_MARKUP_STRING_CONSTANT(SizeNS).IsEqual(pString, FALSE))
			return new CXTPMarkupInt((DWORD)(DWORD_PTR)IDC_SIZENS);

		if (XTP_MARKUP_STRING_CONSTANT(SizeNWSE).IsEqual(pString, FALSE))
			return new CXTPMarkupInt((DWORD)(DWORD_PTR)IDC_SIZENWSE);

		if (XTP_MARKUP_STRING_CONSTANT(SizeWE).IsEqual(pString, FALSE))
			return new CXTPMarkupInt((DWORD)(DWORD_PTR)IDC_SIZEWE);

		if (XTP_MARKUP_STRING_CONSTANT(UpArrow).IsEqual(pString, FALSE))
			return new CXTPMarkupInt((DWORD)(DWORD_PTR)IDC_UPARROW);

		if (XTP_MARKUP_STRING_CONSTANT(Wait).IsEqual(pString, FALSE))
			return new CXTPMarkupInt((DWORD)(DWORD_PTR)IDC_WAIT);

		if (XTP_MARKUP_STRING_CONSTANT(Hand).IsEqual(pString, FALSE))
			return new CXTPMarkupInt((DWORD)32649);
	}

	return NULL;
}

CXTPMarkupObject* CXTPMarkupBuilder::ConvertVisibility(
	CXTPMarkupBuilder* /*pBuilder*/, CXTPMarkupObject* pObject,
	CXTPMarkupDependencyProperty* /*pAssociatedProperty = NULL*/)
{
	if (IsStringObject(pObject))
	{
		const CXTPMarkupString* pString = static_cast<const CXTPMarkupString*>(pObject);

		if (XTP_MARKUP_STRING_CONSTANT(Visible).IsEqual(pString, FALSE))
			return CXTPMarkupEnum::CreateValue(xtpMarkupVisibilityVisible);

		if (XTP_MARKUP_STRING_CONSTANT(Hidden).IsEqual(pString, FALSE))
			return CXTPMarkupEnum::CreateValue(xtpMarkupVisibilityHidden);

		if (XTP_MARKUP_STRING_CONSTANT(Collapsed).IsEqual(pString, FALSE))
			return CXTPMarkupEnum::CreateValue(xtpMarkupVisibilityCollapsed);
	}

	return NULL;
}

CXTPMarkupObject*
	CXTPMarkupBuilder::ConvertContent(CXTPMarkupBuilder* pBuilder, CXTPMarkupObject* pObject,
									  CXTPMarkupDependencyProperty* /*pAssociatedProperty = NULL*/)
{
	if (IsStringObject(pObject))
	{
		CXTPMarkupRun* pRun = MARKUP_CREATE(CXTPMarkupRun, pBuilder->GetMarkupContext());
		pRun->SetText((CXTPMarkupString*)pObject);
		MARKUP_ADDREF(pObject);

		CXTPMarkupTextBlock* pTextBlock = MARKUP_CREATE(CXTPMarkupTextBlock,
														pBuilder->GetMarkupContext());
		pTextBlock->GetInlines()->Add(pRun);

		return pTextBlock;
	}

	return pObject;
}

CXTPMarkupObject* CXTPMarkupBuilder::ConvertScrollBarVisibility(
	CXTPMarkupBuilder* /*pBuilder*/, CXTPMarkupObject* pObject,
	CXTPMarkupDependencyProperty* /*pAssociatedProperty = NULL*/)
{
	if (IsStringObject(pObject))
	{
		const CXTPMarkupString* pString = static_cast<const CXTPMarkupString*>(pObject);

		if (XTP_MARKUP_STRING_CONSTANT(Auto).IsEqual(pString, FALSE))
			return CXTPMarkupEnum::CreateValue(xtpMarkupScrollBarAuto);

		if (XTP_MARKUP_STRING_CONSTANT(Visible).IsEqual(pString, FALSE))
			return CXTPMarkupEnum::CreateValue(xtpMarkupScrollBarVisible);

		if (XTP_MARKUP_STRING_CONSTANT(Disabled).IsEqual(pString, FALSE))
			return CXTPMarkupEnum::CreateValue(xtpMarkupScrollBarDisabled);
	}

	return NULL;
}

CXTPMarkupObject*
	CXTPMarkupBuilder::ConvertStretch(CXTPMarkupBuilder* /*pBuilder*/, CXTPMarkupObject* pObject,
									  CXTPMarkupDependencyProperty* /*pAssociatedProperty = NULL*/)
{
	if (IsStringObject(pObject))
	{
		const CXTPMarkupString* pString = static_cast<const CXTPMarkupString*>(pObject);

		if (XTP_MARKUP_STRING_CONSTANT(None).IsEqual(pString, FALSE))
			return CXTPMarkupEnum::CreateValue(xtpMarkupStretchNone);

		if (XTP_MARKUP_STRING_CONSTANT(Fill).IsEqual(pString, FALSE))
			return CXTPMarkupEnum::CreateValue(xtpMarkupStretchFill);

		if (XTP_MARKUP_STRING_CONSTANT(Uniform).IsEqual(pString, FALSE))
			return CXTPMarkupEnum::CreateValue(xtpMarkupStretchUniform);

		if (XTP_MARKUP_STRING_CONSTANT(UniformToFill).IsEqual(pString, FALSE))
			return CXTPMarkupEnum::CreateValue(xtpMarkupStretchUniformToFill);
	}

	return NULL;
}

BOOL AFX_CDECL CXTPMarkupBuilder::ConvertDouble(LPCWSTR& lpszValue, double& dValue, TCHAR cStop,
												BOOL bCheckTail)
{
	if (!lpszValue)
		return FALSE;

	while (*lpszValue == ' ')
		lpszValue++;

	dValue	 = 0;
	BOOL bSign = FALSE;
	if (*lpszValue == '-')
	{
		bSign = TRUE;
		lpszValue++;
	}
	else if (*lpszValue == '+')
	{
		lpszValue++;
	}

	LPCWSTR lpszNext = lpszValue;

	if (*lpszNext == 0)
		return FALSE;

	BOOL bDecimal   = 0;
	double dDecimal = 0;

	while (*lpszNext != 0)
	{
		WCHAR c = *lpszNext;

		if (c == ' ' || c == cStop)
		{
			if (lpszNext == lpszValue)
				return FALSE;

			if (c == ' ')
			{
				while (*lpszNext == ' ')
					lpszNext++;
			}

			if (cStop != 0 && *lpszNext == cStop)
				lpszNext++;
			break;
		}

		if (c == '.')
		{
			if (bDecimal)
				return FALSE;
			bDecimal = TRUE;
			dDecimal = 1;

			if (lpszValue == lpszNext)
				lpszValue = ++lpszNext;
			else
				lpszNext++;

			continue;
		}

		if (c < '0' || c > '9')
			return FALSE;

		if (!bDecimal)
		{
			dValue = 10 * dValue + (c - '0');
		}
		else
		{
			dDecimal /= 10;
			dValue += dDecimal * double(c - '0');
		}

		lpszNext++;
	}

	if (lpszNext == lpszValue)
		return FALSE;

	lpszValue = lpszNext;
	while (*lpszValue == ' ')
		lpszValue++;

	if (bCheckTail && lpszValue[0] != 0)
		return FALSE;

	if (bSign)
		dValue = -dValue;

	return TRUE;
}

void CXTPMarkupBuilder::ThrowBuilderException(LPCTSTR lpszError)
{
	THROW(new CXTPMarkupBuilderException(lpszError));
}

void CXTPMarkupBuilder::ThrowBuilderException(HRESULT hr)
{
	ThrowBuilderException(_com_error(hr).ErrorMessage());
}
