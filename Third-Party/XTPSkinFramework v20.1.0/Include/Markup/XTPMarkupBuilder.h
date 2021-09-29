// XTPMarkupBuilder.h: interface for the CXTPMarkupBuilder class.
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
#if !defined(__XTPMARKUPBUILDER_H__)
#	define __XTPMARKUPBUILDER_H__
//}}AFX_CODEJOCK_PRIVATE

#	if _MSC_VER > 1000
#		pragma once
#	endif // _MSC_VER > 1000

#	include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

//{{AFX_CODEJOCK_PRIVATE
// Internal Class

class CXTPMarkupUIElement;
class CXTPMarkupObject;
class CXTPMarkupDependencyProperty;
class CXTPMarkupContext;
class CXTPMarkupParser;
class CXTPMarkupClassList;
class CXTPMarkupType;
class CXTPComInitializer;

namespace XTPXML
{
struct IXMLDOMNode;
struct IXMLDOMElement;
}; // namespace XTPXML

class CXTPMarkupBuilderException : public CSimpleException
{
	DECLARE_DYNAMIC(CXTPMarkupBuilderException)

public:
	CXTPMarkupBuilderException(LPCTSTR lpszError);

public:
	BOOL GetErrorMessage(LPTSTR lpszError, UINT nMaxError, PUINT pnHelpContext = NULL);
	BOOL GetErrorMessage(LPTSTR lpszError, UINT nMaxError, PUINT pnHelpContext = NULL) const;

protected:
	TCHAR m_szMessage[256];
};

class _XTP_EXT_CLASS CXTPMarkupBuilder
{
	class CStaticExtension;
	class CStringBuilder;
	class CStaticResourceExtension;

public:
	CXTPMarkupBuilder(CXTPMarkupContext* pContext);
	virtual ~CXTPMarkupBuilder();

public:
	CXTPMarkupObject* CreateObject(LPCWSTR lpszTag, XTPXML::IXMLDOMElement* pElement,
								   CXTPMarkupObject* pParent = NULL);
	CXTPMarkupDependencyProperty* FindProperty(LPCWSTR lpszTag, LPCWSTR lpszAttribute);
	CXTPMarkupDependencyProperty* FindProperty(CXTPMarkupType* pRuntimeClass,
											   LPCWSTR lpszAttribute);

	CString GetLastError() const;

public:
	CXTPMarkupObject* ParseObject(CXTPMarkupParser* pParser);
	CXTPMarkupUIElement* Parse(CXTPMarkupParser* pParser);

	void ThrowBuilderException(LPCTSTR lpszError);
	void ThrowBuilderException(HRESULT hr);

	CXTPMarkupContext* GetMarkupContext() const;

public:
	static BOOL AFX_CDECL ColorStringToKnownColor(LPCWSTR lpszValue, COLORREF& clr);
	static CXTPMarkupObject* AFX_CDECL
		ConvertBool(CXTPMarkupBuilder* pBuilder, CXTPMarkupObject* pObject,
					CXTPMarkupDependencyProperty* pAssociatedProperty = NULL);
	static CXTPMarkupObject* AFX_CDECL
		ConvertTextDecorations(CXTPMarkupBuilder* pBuilder, CXTPMarkupObject* pObject,
							   CXTPMarkupDependencyProperty* pAssociatedProperty = NULL);
	static CXTPMarkupObject* AFX_CDECL
		ConvertFontWeight(CXTPMarkupBuilder* pBuilder, CXTPMarkupObject* pObject,
						  CXTPMarkupDependencyProperty* pAssociatedProperty = NULL);
	static CXTPMarkupObject* AFX_CDECL
		ConvertFontStyle(CXTPMarkupBuilder* pBuilder, CXTPMarkupObject* pObject,
						 CXTPMarkupDependencyProperty* pAssociatedProperty = NULL);
	static CXTPMarkupObject* AFX_CDECL
		ConvertLength(CXTPMarkupBuilder* pBuilder, CXTPMarkupObject* pObject,
					  CXTPMarkupDependencyProperty* pAssociatedProperty = NULL);
	static CXTPMarkupObject* AFX_CDECL
		ConvertVerticalAlignment(CXTPMarkupBuilder* pBuilder, CXTPMarkupObject* pObject,
								 CXTPMarkupDependencyProperty* pAssociatedProperty = NULL);
	static CXTPMarkupObject* AFX_CDECL
		ConvertHorizontalAlignment(CXTPMarkupBuilder* pBuilder, CXTPMarkupObject* pObject,
								   CXTPMarkupDependencyProperty* pAssociatedProperty = NULL);
	static CXTPMarkupObject* AFX_CDECL
		ConvertTextWrapping(CXTPMarkupBuilder* pBuilder, CXTPMarkupObject* pObject,
							CXTPMarkupDependencyProperty* pAssociatedProperty = NULL);
	static CXTPMarkupObject* AFX_CDECL
		ConvertTextTrimming(CXTPMarkupBuilder* pBuilder, CXTPMarkupObject* pObject,
							CXTPMarkupDependencyProperty* pAssociatedProperty = NULL);
	static CXTPMarkupObject* AFX_CDECL
		ConvertTextAlignment(CXTPMarkupBuilder* pBuilder, CXTPMarkupObject* pObject,
							 CXTPMarkupDependencyProperty* pAssociatedProperty = NULL);
	static CXTPMarkupObject* AFX_CDECL
		ConvertOrientation(CXTPMarkupBuilder* pBuilder, CXTPMarkupObject* pObject,
						   CXTPMarkupDependencyProperty* pAssociatedProperty = NULL);
	static CXTPMarkupObject* AFX_CDECL
		ConvertBaselineAlignment(CXTPMarkupBuilder* pBuilder, CXTPMarkupObject* pObject,
								 CXTPMarkupDependencyProperty* pAssociatedProperty = NULL);
	static CXTPMarkupObject* AFX_CDECL
		ConvertCursor(CXTPMarkupBuilder* pBuilder, CXTPMarkupObject* pObject,
					  CXTPMarkupDependencyProperty* pAssociatedProperty = NULL);
	static CXTPMarkupObject* AFX_CDECL
		ConvertVisibility(CXTPMarkupBuilder* pBuilder, CXTPMarkupObject* pObject,
						  CXTPMarkupDependencyProperty* pAssociatedProperty = NULL);
	static CXTPMarkupObject* AFX_CDECL
		ConvertFontQuality(CXTPMarkupBuilder* pBuilder, CXTPMarkupObject* pObject,
						   CXTPMarkupDependencyProperty* pAssociatedProperty = NULL);
	static CXTPMarkupObject* AFX_CDECL
		ConvertContent(CXTPMarkupBuilder* pBuilder, CXTPMarkupObject* pObject,
					   CXTPMarkupDependencyProperty* pAssociatedProperty = NULL);
	static CXTPMarkupObject* AFX_CDECL
		ConvertScrollBarVisibility(CXTPMarkupBuilder* pBuilder, CXTPMarkupObject* pObject,
								   CXTPMarkupDependencyProperty* pAssociatedProperty = NULL);
	static CXTPMarkupObject* AFX_CDECL
		ConvertStretch(CXTPMarkupBuilder* pBuilder, CXTPMarkupObject* pObject,
					   CXTPMarkupDependencyProperty* pAssociatedProperty = NULL);

public:
	static CString AFX_CDECL FormatString(LPCTSTR lpszFormat, LPCTSTR lpszParameter);
	static CString AFX_CDECL FormatString(LPCTSTR lpszFormat, LPCTSTR lpszParameter1,
										  LPCTSTR lpszParameter2);
	static CString AFX_CDECL FormatString(LPCTSTR lpszFormat, LPCTSTR lpszParameter1,
										  LPCTSTR lpszParameter2, LPCTSTR lpszParameter3);

	CXTPMarkupObject* ConvertValue(CXTPMarkupDependencyProperty* pProperty,
								   CXTPMarkupObject* pValue);

	static BOOL AFX_CDECL ConvertDouble(LPCWSTR& lpszValue, double& dValue, TCHAR cStop = 0,
										BOOL bCheckTail = TRUE);

private:
	BOOL NeedPreserveXmlNodeWhitespaces(XTPXML::IXMLDOMNode* pNode) const;
	void LoadMarkupObjectTextContent(CXTPMarkupObject* pObject, XTPXML::IXMLDOMNode* pContent,
									 BOOL bPreformatted);
	void LoadMarkupObjectChild(CXTPMarkupObject* pObject, XTPXML::IXMLDOMNode* pChild);
	void LoadMarkupObjectAttributes(CXTPMarkupObject* pObject, XTPXML::IXMLDOMElement* pElement);
	CXTPMarkupObject* LoadXmlElement(CXTPMarkupObject* pParent, XTPXML::IXMLDOMElement* pElement);

	CXTPMarkupObject* ResolveMarkupExtension(LPCWSTR lpszValue);
	CStaticExtension* GetStaticExtension();
	CStaticResourceExtension* GetStaticResourceExtension();
	CXTPMarkupObject* CreateTypeKey(LPCWSTR lpszType);

protected:
	CXTPComInitializer* m_pComInitializer;
	CString m_strLastError;
	CXTPMarkupContext* m_pMarkupContext;
	CStaticExtension* m_pStaticExtension;
	CStaticResourceExtension* m_pStaticResourceExtension;
	CList<CXTPMarkupObject*, CXTPMarkupObject*> m_arrObjects;

	friend class CStaticResourceExtension;
};

AFX_INLINE CString CXTPMarkupBuilder::GetLastError() const
{
	return m_strLastError;
}
AFX_INLINE CXTPMarkupContext* CXTPMarkupBuilder::GetMarkupContext() const
{
	return m_pMarkupContext;
}

//}}AFX_CODEJOCK_PRIVATE

#	include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPMARKUPBUILDER_H__)
