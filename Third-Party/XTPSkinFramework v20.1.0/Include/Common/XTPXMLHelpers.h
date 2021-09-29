// XTPXMLhelpers.h : XML Helpers
// Created by Microsoft (R) C/C++ Compiler Version 11.00.50727.1 (fbac001d).
//
// This file is a part of the XTREME TOOLKIT PRO MFC class library.
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
#if !defined(__XTPXMLHELPERS_H__)
#	define __XTPXMLHELPERS_H__
//}}AFX_CODEJOCK_PRIVATE

#	if _MSC_VER >= 1000
#		pragma once
#	endif // _MSC_VER >= 1000

// Uncomment _XTP_EXCLUDE_XML to disable all XML used functions.
//#define _XTP_EXCLUDE_XML

#	ifndef _XTP_EXCLUDE_XML

#		ifndef XTP_MSXML_VERSION
// Determines which MSXML version by default will be used by the entire framework:
// 0 - indicates a default MSXML version available in the system (default).
// 3 - force to use MSXML v3
// 5 - force to use MSXML v5
// 6 - force to use MSXML v6
#			define XTP_MSXML_VERSION 0
#		endif

//{{AFX_CODEJOCK_PRIVATE
#		include "Common/Base/Diagnostic/XTPDisableAdvancedWarnings.h"
// C4365: 'initializing': conversion from '...' to '...', signed/unsigned mismatch
#		pragma warning(push)
#		pragma warning(disable : 4365)
#		include <comdef.h>
#		pragma warning(pop)
#		include "Common/Base/Diagnostic/XTPEnableAdvancedWarnings.h"

#		include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

#		pragma pack(push, 8)

#		if (_MSC_VER > 1100)
#			pragma warning(push)
#		endif

// C4616: #pragma warning : warning number '...' out of range, must be between '4001' and '4999'
// C4619: #pragma warning : there is no warning number 'number'
// C5204: '...': class has virtual functions, but its trivial destructor is not virtual; instances
// of objects derived from this class may not be destructed correctly
#		pragma warning(disable : 4616 4619 5204)

namespace XTPXML
{
//
// Forward references and typedefs
//

struct __declspec(uuid("d63e0ce2-a0a2-11d0-9c02-00c04fc99c8e"))
	/* LIBID */ __MSXML;
struct __declspec(uuid("2933bf8f-7b36-11d2-b20e-00c04f983e60"))
	/* dual interface */ IXMLDOMImplementation;
struct __declspec(uuid("2933bf80-7b36-11d2-b20e-00c04f983e60"))
	/* dual interface */ IXMLDOMNode;
XTP_FORWARD_ENUM(tagDOMNodeType);
struct __declspec(uuid("2933bf82-7b36-11d2-b20e-00c04f983e60"))
	/* dual interface */ IXMLDOMNodeList;
struct __declspec(uuid("2933bf83-7b36-11d2-b20e-00c04f983e60"))
	/* dual interface */ IXMLDOMNamedNodeMap;
struct __declspec(uuid("373984c8-b845-449b-91e7-45ac83036ade"))
	/* dual interface */ IXMLDOMSchemaCollection;
struct __declspec(uuid("2933bf81-7b36-11d2-b20e-00c04f983e60"))
	/* dual interface */ IXMLDOMDocument;
struct __declspec(uuid("2933bf95-7b36-11d2-b20e-00c04f983e60"))
	/* dual interface */ IXMLDOMDocument2;
struct __declspec(uuid("2933bf8b-7b36-11d2-b20e-00c04f983e60"))
	/* dual interface */ IXMLDOMDocumentType;
struct __declspec(uuid("2933bf86-7b36-11d2-b20e-00c04f983e60"))
	/* dual interface */ IXMLDOMElement;
struct __declspec(uuid("2933bf85-7b36-11d2-b20e-00c04f983e60"))
	/* dual interface */ IXMLDOMAttribute;
struct __declspec(uuid("3efaa413-272f-11d2-836f-0000f87a7782"))
	/* dual interface */ IXMLDOMDocumentFragment;
struct __declspec(uuid("2933bf87-7b36-11d2-b20e-00c04f983e60"))
	/* dual interface */ IXMLDOMText;
struct __declspec(uuid("2933bf84-7b36-11d2-b20e-00c04f983e60"))
	/* dual interface */ IXMLDOMCharacterData;
struct __declspec(uuid("2933bf88-7b36-11d2-b20e-00c04f983e60"))
	/* dual interface */ IXMLDOMComment;
struct __declspec(uuid("2933bf8a-7b36-11d2-b20e-00c04f983e60"))
	/* dual interface */ IXMLDOMCDATASection;
struct __declspec(uuid("2933bf89-7b36-11d2-b20e-00c04f983e60"))
	/* dual interface */ IXMLDOMProcessingInstruction;
struct __declspec(uuid("2933bf8e-7b36-11d2-b20e-00c04f983e60"))
	/* dual interface */ IXMLDOMEntityReference;
struct __declspec(uuid("3efaa426-272f-11d2-836f-0000f87a7782"))
	/* dual interface */ IXMLDOMParseError;
struct __declspec(uuid("2933bf8c-7b36-11d2-b20e-00c04f983e60"))
	/* dual interface */ IXMLDOMNotation;
struct __declspec(uuid("2933bf8d-7b36-11d2-b20e-00c04f983e60"))
	/* dual interface */ IXMLDOMEntity;
struct __declspec(uuid("3efaa425-272f-11d2-836f-0000f87a7782"))
	/* dual interface */ IXTLRuntime;
struct __declspec(uuid("3efaa427-272f-11d2-836f-0000f87a7782"))
	/* dispinterface */ XMLDOMDocumentEvents;
struct /* coclass */ DOMDocument;
struct /* coclass */ DOMFreeThreadedDocument;
struct __declspec(uuid("ed8c108d-4349-11d2-91a4-00c04f7969e8"))
	/* dual interface */ IXMLHttpRequest;
struct /* coclass */ XMLHTTPRequest;
struct __declspec(uuid("310afa62-0575-11d2-9ca9-0060b0ec3d39"))
	/* dual interface */ IXMLDSOControl;
struct /* coclass */ XMLDSOControl;
struct __declspec(uuid("65725580-9b5d-11d0-9bfe-00c04fc99c8e"))
	/* dual interface */ IXMLElementCollection;
struct __declspec(uuid("f52e2b61-18a1-11d1-b105-00805f49916b"))
	/* dual interface */ IXMLDocument;
struct __declspec(uuid("3f7f31ac-e15f-11d0-9c25-00c04fc99c8e"))
	/* dual interface */ IXMLElement;
struct __declspec(uuid("2b8de2fe-8d2d-11d1-b2fc-00c04fd915a9"))
	/* interface */ IXMLDocument2;
struct __declspec(uuid("2b8de2ff-8d2d-11d1-b2fc-00c04fd915a9"))
	/* dual interface */ IXMLElement2;
struct __declspec(uuid("d4d4a0fc-3b73-11d1-b2b4-00c04fb92596"))
	/* dual interface */ IXMLAttribute;
struct __declspec(uuid("948c5ad3-c58d-11d0-9c0b-00c04fc99c8e"))
	/* interface */ IXMLError;
struct _xml_error;
XTP_FORWARD_ENUM(tagXMLEMEM_TYPE);
struct /* coclass */ XMLDocument;
typedef enum tagDOMNodeType DOMNodeType;
typedef enum tagXMLEMEM_TYPE XMLELEM_TYPE;

//
// Smart pointer typedef declarations
//

_COM_SMARTPTR_TYPEDEF(IXMLDOMImplementation, __uuidof(IXMLDOMImplementation));
_COM_SMARTPTR_TYPEDEF(IXMLDOMParseError, __uuidof(IXMLDOMParseError));
_COM_SMARTPTR_TYPEDEF(XMLDOMDocumentEvents, __uuidof(XMLDOMDocumentEvents));
_COM_SMARTPTR_TYPEDEF(IXMLHttpRequest, __uuidof(IXMLHttpRequest));
_COM_SMARTPTR_TYPEDEF(IXMLElementCollection, __uuidof(IXMLElementCollection));
_COM_SMARTPTR_TYPEDEF(IXMLElement, __uuidof(IXMLElement));
_COM_SMARTPTR_TYPEDEF(IXMLDocument, __uuidof(IXMLDocument));
_COM_SMARTPTR_TYPEDEF(IXMLElement2, __uuidof(IXMLElement2));
_COM_SMARTPTR_TYPEDEF(IXMLDocument2, __uuidof(IXMLDocument2));
_COM_SMARTPTR_TYPEDEF(IXMLAttribute, __uuidof(IXMLAttribute));
_COM_SMARTPTR_TYPEDEF(IXMLError, __uuidof(IXMLError));
_COM_SMARTPTR_TYPEDEF(IXMLDOMNode, __uuidof(IXMLDOMNode));
_COM_SMARTPTR_TYPEDEF(IXMLDOMNodeList, __uuidof(IXMLDOMNodeList));
_COM_SMARTPTR_TYPEDEF(IXMLDOMNamedNodeMap, __uuidof(IXMLDOMNamedNodeMap));
_COM_SMARTPTR_TYPEDEF(IXMLDOMDocumentType, __uuidof(IXMLDOMDocumentType));
_COM_SMARTPTR_TYPEDEF(IXMLDOMAttribute, __uuidof(IXMLDOMAttribute));
_COM_SMARTPTR_TYPEDEF(IXMLDOMElement, __uuidof(IXMLDOMElement));
_COM_SMARTPTR_TYPEDEF(IXMLDOMDocumentFragment, __uuidof(IXMLDOMDocumentFragment));
_COM_SMARTPTR_TYPEDEF(IXMLDOMCharacterData, __uuidof(IXMLDOMCharacterData));
_COM_SMARTPTR_TYPEDEF(IXMLDOMText, __uuidof(IXMLDOMText));
_COM_SMARTPTR_TYPEDEF(IXMLDOMComment, __uuidof(IXMLDOMComment));
_COM_SMARTPTR_TYPEDEF(IXMLDOMCDATASection, __uuidof(IXMLDOMCDATASection));
_COM_SMARTPTR_TYPEDEF(IXMLDOMProcessingInstruction, __uuidof(IXMLDOMProcessingInstruction));
_COM_SMARTPTR_TYPEDEF(IXMLDOMEntityReference, __uuidof(IXMLDOMEntityReference));
_COM_SMARTPTR_TYPEDEF(IXMLDOMSchemaCollection, __uuidof(IXMLDOMSchemaCollection));
_COM_SMARTPTR_TYPEDEF(IXMLDOMDocument, __uuidof(IXMLDOMDocument));
_COM_SMARTPTR_TYPEDEF(IXMLDOMDocument2, __uuidof(IXMLDOMDocument2));
_COM_SMARTPTR_TYPEDEF(IXMLDOMNotation, __uuidof(IXMLDOMNotation));
_COM_SMARTPTR_TYPEDEF(IXMLDOMEntity, __uuidof(IXMLDOMEntity));
_COM_SMARTPTR_TYPEDEF(IXTLRuntime, __uuidof(IXTLRuntime));
_COM_SMARTPTR_TYPEDEF(IXMLDSOControl, __uuidof(IXMLDSOControl));

//
// Type library items
//

struct __declspec(uuid("2933bf8f-7b36-11d2-b20e-00c04f983e60")) IXMLDOMImplementation : IDispatch
{
	//
	// Wrapper methods for error-handling
	//

	VARIANT_BOOL hasFeature(_bstr_t feature, _bstr_t version);

	//
	// Raw methods provided by interface
	//

	virtual HRESULT __stdcall raw_hasFeature(
		/*[in]*/ BSTR feature,
		/*[in]*/ BSTR version,
		/*[out,retval]*/ VARIANT_BOOL* hasFeature) = 0;
};

enum tagDOMNodeType
{
	NODE_INVALID				= 0,
	NODE_ELEMENT				= 1,
	NODE_ATTRIBUTE				= 2,
	NODE_TEXT					= 3,
	NODE_CDATA_SECTION			= 4,
	NODE_ENTITY_REFERENCE		= 5,
	NODE_ENTITY					= 6,
	NODE_PROCESSING_INSTRUCTION = 7,
	NODE_COMMENT				= 8,
	NODE_DOCUMENT				= 9,
	NODE_DOCUMENT_TYPE			= 10,
	NODE_DOCUMENT_FRAGMENT		= 11,
	NODE_NOTATION				= 12
};

struct __declspec(uuid("3efaa426-272f-11d2-836f-0000f87a7782")) IXMLDOMParseError : IDispatch
{
	//
	// Property data
	//

	__declspec(property(get = GeterrorCode)) long errorCode;
	__declspec(property(get = Geturl)) _bstr_t url;
	__declspec(property(get = Getreason)) _bstr_t reason;
	__declspec(property(get = GetsrcText)) _bstr_t srcText;
	__declspec(property(get = Getline)) long line;
	__declspec(property(get = Getlinepos)) long linepos;
	__declspec(property(get = Getfilepos)) long filepos;

	//
	// Wrapper methods for error-handling
	//

	long GeterrorCode();
	_bstr_t Geturl();
	_bstr_t Getreason();
	_bstr_t GetsrcText();
	long Getline();
	long Getlinepos();
	long Getfilepos();

	//
	// Raw methods provided by interface
	//

	virtual HRESULT __stdcall get_errorCode(
		/*[out,retval]*/ long* errorCode) = 0;
	virtual HRESULT __stdcall get_url(
		/*[out,retval]*/ BSTR* urlString) = 0;
	virtual HRESULT __stdcall get_reason(
		/*[out,retval]*/ BSTR* reasonString) = 0;
	virtual HRESULT __stdcall get_srcText(
		/*[out,retval]*/ BSTR* sourceString) = 0;
	virtual HRESULT __stdcall get_line(
		/*[out,retval]*/ long* lineNumber) = 0;
	virtual HRESULT __stdcall get_linepos(
		/*[out,retval]*/ long* linePosition) = 0;
	virtual HRESULT __stdcall get_filepos(
		/*[out,retval]*/ long* filePosition) = 0;
};

struct __declspec(uuid("3efaa427-272f-11d2-836f-0000f87a7782")) XMLDOMDocumentEvents : IDispatch
{
	//
	// Wrapper methods for error-handling
	//

	// Methods:
	HRESULT ondataavailable();
	HRESULT onreadystatechange();
};

struct __declspec(uuid("2933bf90-7b36-11d2-b20e-00c04f983e60")) DOMDocument;
// [ default ] interface IXMLDOMDocument
// [ default, source ] dispinterface XMLDOMDocumentEvents

struct __declspec(uuid("2933bf91-7b36-11d2-b20e-00c04f983e60")) DOMFreeThreadedDocument;
// [ default ] interface IXMLDOMDocument
// [ default, source ] dispinterface XMLDOMDocumentEvents

struct __declspec(uuid("ed8c108d-4349-11d2-91a4-00c04f7969e8")) IXMLHttpRequest : IDispatch
{
	//
	// Property data
	//

	__declspec(property(get = Getstatus)) long status;
	__declspec(property(get = GetstatusText)) _bstr_t statusText;
	__declspec(property(get = GetresponseXML)) IDispatchPtr responseXML;
	__declspec(property(get = GetresponseText)) _bstr_t responseText;
	__declspec(property(get = GetresponseBody)) _variant_t responseBody;
	__declspec(property(get = GetresponseStream)) _variant_t responseStream;
	__declspec(property(get = GetreadyState)) long readyState;
	__declspec(property(put = Putonreadystatechange)) IDispatchPtr onreadystatechange;

	//
	// Wrapper methods for error-handling
	//

	HRESULT open(_bstr_t bstrMethod, _bstr_t bstrUrl, const _variant_t& varAsync = vtMissing,
				 const _variant_t& bstrUser		= vtMissing,
				 const _variant_t& bstrPassword = vtMissing);
	HRESULT setRequestHeader(_bstr_t bstrHeader, _bstr_t bstrValue);
	_bstr_t getResponseHeader(_bstr_t bstrHeader);
	_bstr_t getAllResponseHeaders();
	HRESULT send(const _variant_t& varBody = vtMissing);
	HRESULT abort();
	long Getstatus();
	_bstr_t GetstatusText();
	IDispatchPtr GetresponseXML();
	_bstr_t GetresponseText();
	_variant_t GetresponseBody();
	_variant_t GetresponseStream();
	long GetreadyState();
	void Putonreadystatechange(IDispatch* _arg1);

	//
	// Raw methods provided by interface
	//

	virtual HRESULT __stdcall raw_open(
		/*[in]*/ BSTR bstrMethod,
		/*[in]*/ BSTR bstrUrl,
		/*[in]*/ VARIANT varAsync	  = vtMissing,
		/*[in]*/ VARIANT bstrUser	  = vtMissing,
		/*[in]*/ VARIANT bstrPassword = vtMissing) = 0;
	virtual HRESULT __stdcall raw_setRequestHeader(
		/*[in]*/ BSTR bstrHeader,
		/*[in]*/ BSTR bstrValue) = 0;
	virtual HRESULT __stdcall raw_getResponseHeader(
		/*[in]*/ BSTR bstrHeader,
		/*[out,retval]*/ BSTR* pbstrValue) = 0;
	virtual HRESULT __stdcall raw_getAllResponseHeaders(
		/*[out,retval]*/ BSTR* pbstrHeaders) = 0;
	virtual HRESULT __stdcall raw_send(
		/*[in]*/ VARIANT varBody = vtMissing) = 0;
	virtual HRESULT __stdcall raw_abort()	  = 0;
	virtual HRESULT __stdcall get_status(
		/*[out,retval]*/ long* plStatus) = 0;
	virtual HRESULT __stdcall get_statusText(
		/*[out,retval]*/ BSTR* pbstrStatus) = 0;
	virtual HRESULT __stdcall get_responseXML(
		/*[out,retval]*/ IDispatch** ppBody) = 0;
	virtual HRESULT __stdcall get_responseText(
		/*[out,retval]*/ BSTR* pbstrBody) = 0;
	virtual HRESULT __stdcall get_responseBody(
		/*[out,retval]*/ VARIANT* pvarBody) = 0;
	virtual HRESULT __stdcall get_responseStream(
		/*[out,retval]*/ VARIANT* pvarBody) = 0;
	virtual HRESULT __stdcall get_readyState(
		/*[out,retval]*/ long* plState) = 0;
	virtual HRESULT __stdcall put_onreadystatechange(
		/*[in]*/ IDispatch* _arg1) = 0;
};

struct __declspec(uuid("ed8c108e-4349-11d2-91a4-00c04f7969e8")) XMLHTTPRequest;
// [ default ] interface IXMLHttpRequest

struct __declspec(uuid("550dda30-0541-11d2-9ca9-0060b0ec3d39")) XMLDSOControl;
// [ default ] interface IXMLDSOControl

struct __declspec(uuid("65725580-9b5d-11d0-9bfe-00c04fc99c8e")) IXMLElementCollection : IDispatch
{
	//
	// Property data
	//

	__declspec(property(get = Getlength, put = Putlength)) long length;
	__declspec(property(get = Get_newEnum)) IUnknownPtr _newEnum;

	//
	// Wrapper methods for error-handling
	//

	void Putlength(long p);
	long Getlength();
	IUnknownPtr Get_newEnum();
	IDispatchPtr item(const _variant_t& var1 = vtMissing, const _variant_t& var2 = vtMissing);

	//
	// Raw methods provided by interface
	//

	virtual HRESULT __stdcall put_length(
		/*[in]*/ long p) = 0;
	virtual HRESULT __stdcall get_length(
		/*[out,retval]*/ long* p) = 0;
	virtual HRESULT __stdcall get__newEnum(
		/*[out,retval]*/ IUnknown** ppUnk) = 0;
	virtual HRESULT __stdcall raw_item(
		/*[in]*/ VARIANT var1,
		/*[in]*/ VARIANT var2,
		/*[out,retval]*/ IDispatch** ppDisp) = 0;
};

struct __declspec(uuid("3f7f31ac-e15f-11d0-9c25-00c04fc99c8e")) IXMLElement : IDispatch
{
	//
	// Property data
	//

	__declspec(property(get = GettagName, put = PuttagName)) _bstr_t tagName;
	__declspec(property(get = Getparent)) IXMLElementPtr parent;
	__declspec(property(get = Getchildren)) IXMLElementCollectionPtr children;
	__declspec(property(get = Gettype)) long type;
	__declspec(property(get = Gettext, put = Puttext)) _bstr_t text;

	//
	// Wrapper methods for error-handling
	//

	_bstr_t GettagName();
	void PuttagName(_bstr_t p);
	IXMLElementPtr Getparent();
	HRESULT setAttribute(_bstr_t strPropertyName, const _variant_t& PropertyValue);
	_variant_t getAttribute(_bstr_t strPropertyName);
	HRESULT removeAttribute(_bstr_t strPropertyName);
	IXMLElementCollectionPtr Getchildren();
	long Gettype();
	_bstr_t Gettext();
	void Puttext(_bstr_t p);
	HRESULT addChild(struct IXMLElement* pChildElem, long lIndex, long lReserved);
	HRESULT removeChild(struct IXMLElement* pChildElem);

	//
	// Raw methods provided by interface
	//

	virtual HRESULT __stdcall get_tagName(
		/*[out,retval]*/ BSTR* p) = 0;
	virtual HRESULT __stdcall put_tagName(
		/*[in]*/ BSTR p) = 0;
	virtual HRESULT __stdcall get_parent(
		/*[out,retval]*/ struct IXMLElement** ppParent) = 0;
	virtual HRESULT __stdcall raw_setAttribute(
		/*[in]*/ BSTR strPropertyName,
		/*[in]*/ VARIANT PropertyValue) = 0;
	virtual HRESULT __stdcall raw_getAttribute(
		/*[in]*/ BSTR strPropertyName,
		/*[out,retval]*/ VARIANT* PropertyValue) = 0;
	virtual HRESULT __stdcall raw_removeAttribute(
		/*[in]*/ BSTR strPropertyName) = 0;
	virtual HRESULT __stdcall get_children(
		/*[out,retval]*/ struct IXMLElementCollection** pp) = 0;
	virtual HRESULT __stdcall get_type(
		/*[out,retval]*/ long* plType) = 0;
	virtual HRESULT __stdcall get_text(
		/*[out,retval]*/ BSTR* p) = 0;
	virtual HRESULT __stdcall put_text(
		/*[in]*/ BSTR p) = 0;
	virtual HRESULT __stdcall raw_addChild(
		/*[in]*/ struct IXMLElement* pChildElem, long lIndex, long lReserved) = 0;
	virtual HRESULT __stdcall raw_removeChild(
		/*[in]*/ struct IXMLElement* pChildElem) = 0;
};

struct __declspec(uuid("f52e2b61-18a1-11d1-b105-00805f49916b")) IXMLDocument : IDispatch
{
	//
	// Property data
	//

	__declspec(property(get = Getroot)) IXMLElementPtr root;
	__declspec(property(get = GetfileSize)) _bstr_t fileSize;
	__declspec(property(get = GetfileModifiedDate)) _bstr_t fileModifiedDate;
	__declspec(property(get = GetfileUpdatedDate)) _bstr_t fileUpdatedDate;
	__declspec(property(get = Geturl, put = Puturl)) _bstr_t url;
	__declspec(property(get = GetmimeType)) _bstr_t mimeType;
	__declspec(property(get = GetreadyState)) long readyState;
	__declspec(property(get = Getcharset, put = Putcharset)) _bstr_t charset;
	__declspec(property(get = Getversion)) _bstr_t version;
	__declspec(property(get = Getdoctype)) _bstr_t doctype;
	__declspec(property(get = GetdtdURL)) _bstr_t dtdURL;

	//
	// Wrapper methods for error-handling
	//

	IXMLElementPtr Getroot();
	_bstr_t GetfileSize();
	_bstr_t GetfileModifiedDate();
	_bstr_t GetfileUpdatedDate();
	_bstr_t Geturl();
	void Puturl(_bstr_t p);
	_bstr_t GetmimeType();
	long GetreadyState();
	_bstr_t Getcharset();
	void Putcharset(_bstr_t p);
	_bstr_t Getversion();
	_bstr_t Getdoctype();
	_bstr_t GetdtdURL();
	IXMLElementPtr createElement(const _variant_t& vType, const _variant_t& var1 = vtMissing);

	//
	// Raw methods provided by interface
	//

	virtual HRESULT __stdcall get_root(
		/*[out,retval]*/ struct IXMLElement** p) = 0;
	virtual HRESULT __stdcall get_fileSize(
		/*[out,retval]*/ BSTR* p) = 0;
	virtual HRESULT __stdcall get_fileModifiedDate(
		/*[out,retval]*/ BSTR* p) = 0;
	virtual HRESULT __stdcall get_fileUpdatedDate(
		/*[out,retval]*/ BSTR* p) = 0;
	virtual HRESULT __stdcall get_url(
		/*[out,retval]*/ BSTR* p) = 0;
	virtual HRESULT __stdcall put_url(
		/*[in]*/ BSTR p) = 0;
	virtual HRESULT __stdcall get_mimeType(
		/*[out,retval]*/ BSTR* p) = 0;
	virtual HRESULT __stdcall get_readyState(
		/*[out,retval]*/ long* pl) = 0;
	virtual HRESULT __stdcall get_charset(
		/*[out,retval]*/ BSTR* p) = 0;
	virtual HRESULT __stdcall put_charset(
		/*[in]*/ BSTR p) = 0;
	virtual HRESULT __stdcall get_version(
		/*[out,retval]*/ BSTR* p) = 0;
	virtual HRESULT __stdcall get_doctype(
		/*[out,retval]*/ BSTR* p) = 0;
	virtual HRESULT __stdcall get_dtdURL(
		/*[out,retval]*/ BSTR* p) = 0;
	virtual HRESULT __stdcall raw_createElement(
		/*[in]*/ VARIANT vType,
		/*[in]*/ VARIANT var1,
		/*[out,retval]*/ struct IXMLElement** ppElem) = 0;
};

struct __declspec(uuid("2b8de2ff-8d2d-11d1-b2fc-00c04fd915a9")) IXMLElement2 : IDispatch
{
	//
	// Property data
	//

	__declspec(property(get = GettagName, put = PuttagName)) _bstr_t tagName;
	__declspec(property(get = Getparent)) IXMLElement2Ptr parent;
	__declspec(property(get = Getchildren)) IXMLElementCollectionPtr children;
	__declspec(property(get = Gettype)) long type;
	__declspec(property(get = Gettext, put = Puttext)) _bstr_t text;
	__declspec(property(get = Getattributes)) IXMLElementCollectionPtr attributes;

	//
	// Wrapper methods for error-handling
	//

	_bstr_t GettagName();
	void PuttagName(_bstr_t p);
	IXMLElement2Ptr Getparent();
	HRESULT setAttribute(_bstr_t strPropertyName, const _variant_t& PropertyValue);
	_variant_t getAttribute(_bstr_t strPropertyName);
	HRESULT removeAttribute(_bstr_t strPropertyName);
	IXMLElementCollectionPtr Getchildren();
	long Gettype();
	_bstr_t Gettext();
	void Puttext(_bstr_t p);
	HRESULT addChild(struct IXMLElement2* pChildElem, long lIndex, long lReserved);
	HRESULT removeChild(struct IXMLElement2* pChildElem);
	IXMLElementCollectionPtr Getattributes();

	//
	// Raw methods provided by interface
	//

	virtual HRESULT __stdcall get_tagName(
		/*[out,retval]*/ BSTR* p) = 0;
	virtual HRESULT __stdcall put_tagName(
		/*[in]*/ BSTR p) = 0;
	virtual HRESULT __stdcall get_parent(
		/*[out,retval]*/ struct IXMLElement2** ppParent) = 0;
	virtual HRESULT __stdcall raw_setAttribute(
		/*[in]*/ BSTR strPropertyName,
		/*[in]*/ VARIANT PropertyValue) = 0;
	virtual HRESULT __stdcall raw_getAttribute(
		/*[in]*/ BSTR strPropertyName,
		/*[out,retval]*/ VARIANT* PropertyValue) = 0;
	virtual HRESULT __stdcall raw_removeAttribute(
		/*[in]*/ BSTR strPropertyName) = 0;
	virtual HRESULT __stdcall get_children(
		/*[out,retval]*/ struct IXMLElementCollection** pp) = 0;
	virtual HRESULT __stdcall get_type(
		/*[out,retval]*/ long* plType) = 0;
	virtual HRESULT __stdcall get_text(
		/*[out,retval]*/ BSTR* p) = 0;
	virtual HRESULT __stdcall put_text(
		/*[in]*/ BSTR p) = 0;
	virtual HRESULT __stdcall raw_addChild(
		/*[in]*/ struct IXMLElement2* pChildElem, long lIndex, long lReserved) = 0;
	virtual HRESULT __stdcall raw_removeChild(
		/*[in]*/ struct IXMLElement2* pChildElem) = 0;
	virtual HRESULT __stdcall get_attributes(
		/*[out,retval]*/ struct IXMLElementCollection** pp) = 0;
};

struct __declspec(uuid("2b8de2fe-8d2d-11d1-b2fc-00c04fd915a9")) IXMLDocument2 : IDispatch
{
	//
	// Property data
	//

	__declspec(property(get = Getroot)) IXMLElement2Ptr root;
	__declspec(property(get = GetfileSize)) _bstr_t fileSize;
	__declspec(property(get = GetfileModifiedDate)) _bstr_t fileModifiedDate;
	__declspec(property(get = GetfileUpdatedDate)) _bstr_t fileUpdatedDate;
	__declspec(property(get = Geturl, put = Puturl)) _bstr_t url;
	__declspec(property(get = GetmimeType)) _bstr_t mimeType;
	__declspec(property(get = GetreadyState)) long readyState;
	__declspec(property(get = Getcharset, put = Putcharset)) _bstr_t charset;
	__declspec(property(get = Getversion)) _bstr_t version;
	__declspec(property(get = Getdoctype)) _bstr_t doctype;
	__declspec(property(get = GetdtdURL)) _bstr_t dtdURL;
	__declspec(property(get = Getasync, put = Putasync)) VARIANT_BOOL async;

	//
	// Wrapper methods for error-handling
	//

	IXMLElement2Ptr Getroot();
	_bstr_t GetfileSize();
	_bstr_t GetfileModifiedDate();
	_bstr_t GetfileUpdatedDate();
	_bstr_t Geturl();
	void Puturl(_bstr_t p);
	_bstr_t GetmimeType();
	long GetreadyState();
	_bstr_t Getcharset();
	void Putcharset(_bstr_t p);
	_bstr_t Getversion();
	_bstr_t Getdoctype();
	_bstr_t GetdtdURL();
	IXMLElement2Ptr createElement(const _variant_t& vType, const _variant_t& var1 = vtMissing);
	VARIANT_BOOL Getasync();
	void Putasync(VARIANT_BOOL pf);

	//
	// Raw methods provided by interface
	//

	virtual HRESULT __stdcall get_root(
		/*[out,retval]*/ struct IXMLElement2** p) = 0;
	virtual HRESULT __stdcall get_fileSize(
		/*[out,retval]*/ BSTR* p) = 0;
	virtual HRESULT __stdcall get_fileModifiedDate(
		/*[out,retval]*/ BSTR* p) = 0;
	virtual HRESULT __stdcall get_fileUpdatedDate(
		/*[out,retval]*/ BSTR* p) = 0;
	virtual HRESULT __stdcall get_url(
		/*[out,retval]*/ BSTR* p) = 0;
	virtual HRESULT __stdcall put_url(
		/*[in]*/ BSTR p) = 0;
	virtual HRESULT __stdcall get_mimeType(
		/*[out,retval]*/ BSTR* p) = 0;
	virtual HRESULT __stdcall get_readyState(
		/*[out,retval]*/ long* pl) = 0;
	virtual HRESULT __stdcall get_charset(
		/*[out,retval]*/ BSTR* p) = 0;
	virtual HRESULT __stdcall put_charset(
		/*[in]*/ BSTR p) = 0;
	virtual HRESULT __stdcall get_version(
		/*[out,retval]*/ BSTR* p) = 0;
	virtual HRESULT __stdcall get_doctype(
		/*[out,retval]*/ BSTR* p) = 0;
	virtual HRESULT __stdcall get_dtdURL(
		/*[out,retval]*/ BSTR* p) = 0;
	virtual HRESULT __stdcall raw_createElement(
		/*[in]*/ VARIANT vType,
		/*[in]*/ VARIANT var1,
		/*[out,retval]*/ struct IXMLElement2** ppElem) = 0;
	virtual HRESULT __stdcall get_async(
		/*[out,retval]*/ VARIANT_BOOL* pf) = 0;
	virtual HRESULT __stdcall put_async(
		/*[in]*/ VARIANT_BOOL pf) = 0;
};

struct __declspec(uuid("d4d4a0fc-3b73-11d1-b2b4-00c04fb92596")) IXMLAttribute : IDispatch
{
	//
	// Property data
	//

	__declspec(property(get = Getname)) _bstr_t name;
	__declspec(property(get = Getvalue)) _bstr_t value;

	//
	// Wrapper methods for error-handling
	//

	_bstr_t Getname();
	_bstr_t Getvalue();

	//
	// Raw methods provided by interface
	//

	virtual HRESULT __stdcall get_name(
		/*[out,retval]*/ BSTR* n) = 0;
	virtual HRESULT __stdcall get_value(
		/*[out,retval]*/ BSTR* v) = 0;
};

#		pragma pack(push, 4)

struct _xml_error
{
	unsigned int _nLine;
	BSTR _pchBuf;
	unsigned int _cchBuf;
	unsigned int _ich;
	BSTR _pszFound;
	BSTR _pszExpected;
	unsigned long _reserved1;
	unsigned long _reserved2;
};

#		pragma pack(pop)

struct __declspec(uuid("948c5ad3-c58d-11d0-9c0b-00c04fc99c8e")) IXMLError : IUnknown
{
	//
	// Wrapper methods for error-handling
	//

	HRESULT GetErrorInfo(struct _xml_error* pErrorReturn);

	//
	// Raw methods provided by interface
	//

	virtual HRESULT __stdcall raw_GetErrorInfo(struct _xml_error* pErrorReturn) = 0;
};

enum tagXMLEMEM_TYPE
{
	XMLELEMTYPE_ELEMENT	 = 0,
	XMLELEMTYPE_TEXT	 = 1,
	XMLELEMTYPE_COMMENT	 = 2,
	XMLELEMTYPE_DOCUMENT = 3,
	XMLELEMTYPE_DTD		 = 4,
	XMLELEMTYPE_PI		 = 5,
	XMLELEMTYPE_OTHER	 = 6
};

struct __declspec(uuid("cfc399af-d876-11d0-9c10-00c04fc99c8e")) XMLDocument;
// [ default ] interface IXMLDocument2

struct __declspec(uuid("2933bf80-7b36-11d2-b20e-00c04f983e60")) IXMLDOMNode : IDispatch
{
	//
	// Property data
	//

	__declspec(property(get = GetnodeName)) _bstr_t nodeName;
	__declspec(property(get = GetnodeValue, put = PutnodeValue)) _variant_t nodeValue;
	__declspec(property(get = GetnodeType)) DOMNodeType nodeType;
	__declspec(property(get = GetparentNode)) IXMLDOMNodePtr parentNode;
	__declspec(property(get = GetchildNodes)) IXMLDOMNodeListPtr childNodes;
	__declspec(property(get = GetfirstChild)) IXMLDOMNodePtr firstChild;
	__declspec(property(get = GetlastChild)) IXMLDOMNodePtr lastChild;
	__declspec(property(get = GetpreviousSibling)) IXMLDOMNodePtr previousSibling;
	__declspec(property(get = GetnextSibling)) IXMLDOMNodePtr nextSibling;
	__declspec(property(get = Getattributes)) IXMLDOMNamedNodeMapPtr attributes;
	__declspec(property(get = GetownerDocument)) IXMLDOMDocumentPtr ownerDocument;
	__declspec(property(get = GetnodeTypeString)) _bstr_t nodeTypeString;
	__declspec(property(get = Getspecified)) VARIANT_BOOL specified;
	__declspec(property(get = Getdefinition)) IXMLDOMNodePtr definition;
	__declspec(property(get = Gettext, put = Puttext)) _bstr_t text;
	__declspec(property(get = GetnodeTypedValue,
						put = PutnodeTypedValue)) _variant_t nodeTypedValue;
	__declspec(property(get = Getxml)) _bstr_t xml;
	__declspec(property(get = Getparsed)) VARIANT_BOOL parsed;
	__declspec(property(get = GetnamespaceURI)) _bstr_t namespaceURI;
	__declspec(property(get = Getprefix)) _bstr_t prefix;
	__declspec(property(get = GetbaseName)) _bstr_t baseName;

	//
	// Wrapper methods for error-handling
	//

	_bstr_t GetnodeName();
	_variant_t GetnodeValue();
	void PutnodeValue(const _variant_t& value);
	DOMNodeType GetnodeType();
	IXMLDOMNodePtr GetparentNode();
	IXMLDOMNodeListPtr GetchildNodes();
	IXMLDOMNodePtr GetfirstChild();
	IXMLDOMNodePtr GetlastChild();
	IXMLDOMNodePtr GetpreviousSibling();
	IXMLDOMNodePtr GetnextSibling();
	IXMLDOMNamedNodeMapPtr Getattributes();
	IXMLDOMNodePtr insertBefore(struct IXMLDOMNode* newChild, const _variant_t& refChild);
	IXMLDOMNodePtr replaceChild(struct IXMLDOMNode* newChild, struct IXMLDOMNode* oldChild);
	IXMLDOMNodePtr removeChild(struct IXMLDOMNode* childNode);
	IXMLDOMNodePtr appendChild(struct IXMLDOMNode* newChild);
	VARIANT_BOOL hasChildNodes();
	IXMLDOMDocumentPtr GetownerDocument();
	IXMLDOMNodePtr cloneNode(VARIANT_BOOL deep);
	_bstr_t GetnodeTypeString();
	_bstr_t Gettext();
	void Puttext(_bstr_t text);
	VARIANT_BOOL Getspecified();
	IXMLDOMNodePtr Getdefinition();
	_variant_t GetnodeTypedValue();
	void PutnodeTypedValue(const _variant_t& typedValue);
	_variant_t GetdataType();
	void PutdataType(_bstr_t dataTypeName);
	_bstr_t Getxml();
	_bstr_t transformNode(struct IXMLDOMNode* stylesheet);
	IXMLDOMNodeListPtr selectNodes(_bstr_t queryString);
	IXMLDOMNodePtr selectSingleNode(_bstr_t queryString);
	VARIANT_BOOL Getparsed();
	_bstr_t GetnamespaceURI();
	_bstr_t Getprefix();
	_bstr_t GetbaseName();
	HRESULT transformNodeToObject(struct IXMLDOMNode* stylesheet, const _variant_t& outputObject);

	//
	// Raw methods provided by interface
	//

	virtual HRESULT __stdcall get_nodeName(
		/*[out,retval]*/ BSTR* name) = 0;
	virtual HRESULT __stdcall get_nodeValue(
		/*[out,retval]*/ VARIANT* value) = 0;
	virtual HRESULT __stdcall put_nodeValue(
		/*[in]*/ VARIANT value) = 0;
	virtual HRESULT __stdcall get_nodeType(
		/*[out,retval]*/ DOMNodeType* type) = 0;
	virtual HRESULT __stdcall get_parentNode(
		/*[out,retval]*/ struct IXMLDOMNode** parent) = 0;
	virtual HRESULT __stdcall get_childNodes(
		/*[out,retval]*/ struct IXMLDOMNodeList** childList) = 0;
	virtual HRESULT __stdcall get_firstChild(
		/*[out,retval]*/ struct IXMLDOMNode** firstChild) = 0;
	virtual HRESULT __stdcall get_lastChild(
		/*[out,retval]*/ struct IXMLDOMNode** lastChild) = 0;
	virtual HRESULT __stdcall get_previousSibling(
		/*[out,retval]*/ struct IXMLDOMNode** previousSibling) = 0;
	virtual HRESULT __stdcall get_nextSibling(
		/*[out,retval]*/ struct IXMLDOMNode** nextSibling) = 0;
	virtual HRESULT __stdcall get_attributes(
		/*[out,retval]*/ struct IXMLDOMNamedNodeMap** attributeMap) = 0;
	virtual HRESULT __stdcall raw_insertBefore(
		/*[in]*/ struct IXMLDOMNode* newChild,
		/*[in]*/ VARIANT refChild,
		/*[out,retval]*/ struct IXMLDOMNode** outNewChild) = 0;
	virtual HRESULT __stdcall raw_replaceChild(
		/*[in]*/ struct IXMLDOMNode* newChild,
		/*[in]*/ struct IXMLDOMNode* oldChild,
		/*[out,retval]*/ struct IXMLDOMNode** outOldChild) = 0;
	virtual HRESULT __stdcall raw_removeChild(
		/*[in]*/ struct IXMLDOMNode* childNode,
		/*[out,retval]*/ struct IXMLDOMNode** oldChild) = 0;
	virtual HRESULT __stdcall raw_appendChild(
		/*[in]*/ struct IXMLDOMNode* newChild,
		/*[out,retval]*/ struct IXMLDOMNode** outNewChild) = 0;
	virtual HRESULT __stdcall raw_hasChildNodes(
		/*[out,retval]*/ VARIANT_BOOL* hasChild) = 0;
	virtual HRESULT __stdcall get_ownerDocument(
		/*[out,retval]*/ struct IXMLDOMDocument** DOMDocument) = 0;
	virtual HRESULT __stdcall raw_cloneNode(
		/*[in]*/ VARIANT_BOOL deep,
		/*[out,retval]*/ struct IXMLDOMNode** cloneRoot) = 0;
	virtual HRESULT __stdcall get_nodeTypeString(
		/*[out,retval]*/ BSTR* nodeType) = 0;
	virtual HRESULT __stdcall get_text(
		/*[out,retval]*/ BSTR* text) = 0;
	virtual HRESULT __stdcall put_text(
		/*[in]*/ BSTR text) = 0;
	virtual HRESULT __stdcall get_specified(
		/*[out,retval]*/ VARIANT_BOOL* isSpecified) = 0;
	virtual HRESULT __stdcall get_definition(
		/*[out,retval]*/ struct IXMLDOMNode** definitionNode) = 0;
	virtual HRESULT __stdcall get_nodeTypedValue(
		/*[out,retval]*/ VARIANT* typedValue) = 0;
	virtual HRESULT __stdcall put_nodeTypedValue(
		/*[in]*/ VARIANT typedValue) = 0;
	virtual HRESULT __stdcall get_dataType(
		/*[out,retval]*/ VARIANT* dataTypeName) = 0;
	virtual HRESULT __stdcall put_dataType(
		/*[in]*/ BSTR dataTypeName) = 0;
	virtual HRESULT __stdcall get_xml(
		/*[out,retval]*/ BSTR* xmlString) = 0;
	virtual HRESULT __stdcall raw_transformNode(
		/*[in]*/ struct IXMLDOMNode* stylesheet,
		/*[out,retval]*/ BSTR* xmlString) = 0;
	virtual HRESULT __stdcall raw_selectNodes(
		/*[in]*/ BSTR queryString,
		/*[out,retval]*/ struct IXMLDOMNodeList** resultList) = 0;
	virtual HRESULT __stdcall raw_selectSingleNode(
		/*[in]*/ BSTR queryString,
		/*[out,retval]*/ struct IXMLDOMNode** resultNode) = 0;
	virtual HRESULT __stdcall get_parsed(
		/*[out,retval]*/ VARIANT_BOOL* isParsed) = 0;
	virtual HRESULT __stdcall get_namespaceURI(
		/*[out,retval]*/ BSTR* namespaceURI) = 0;
	virtual HRESULT __stdcall get_prefix(
		/*[out,retval]*/ BSTR* prefixString) = 0;
	virtual HRESULT __stdcall get_baseName(
		/*[out,retval]*/ BSTR* nameString) = 0;
	virtual HRESULT __stdcall raw_transformNodeToObject(
		/*[in]*/ struct IXMLDOMNode* stylesheet,
		/*[in]*/ VARIANT outputObject) = 0;
};

struct __declspec(uuid("2933bf82-7b36-11d2-b20e-00c04f983e60")) IXMLDOMNodeList : IDispatch
{
	//
	// Property data
	//

	__declspec(property(get = Getitem)) IXMLDOMNodePtr item[];
	__declspec(property(get = Getlength)) long length;
	__declspec(property(get = Get_newEnum)) IUnknownPtr _newEnum;

	//
	// Wrapper methods for error-handling
	//

	IXMLDOMNodePtr Getitem(long index);
	long Getlength();
	IXMLDOMNodePtr nextNode();
	HRESULT reset();
	IUnknownPtr Get_newEnum();

	//
	// Raw methods provided by interface
	//

	virtual HRESULT __stdcall get_item(
		/*[in]*/ long index,
		/*[out,retval]*/ struct IXMLDOMNode** listItem) = 0;
	virtual HRESULT __stdcall get_length(
		/*[out,retval]*/ long* listLength) = 0;
	virtual HRESULT __stdcall raw_nextNode(
		/*[out,retval]*/ struct IXMLDOMNode** nextItem) = 0;
	virtual HRESULT __stdcall raw_reset()				= 0;
	virtual HRESULT __stdcall get__newEnum(
		/*[out,retval]*/ IUnknown** ppUnk) = 0;
};

struct __declspec(uuid("2933bf83-7b36-11d2-b20e-00c04f983e60")) IXMLDOMNamedNodeMap : IDispatch
{
	//
	// Property data
	//

	__declspec(property(get = Getitem)) IXMLDOMNodePtr item[];
	__declspec(property(get = Getlength)) long length;
	__declspec(property(get = Get_newEnum)) IUnknownPtr _newEnum;

	//
	// Wrapper methods for error-handling
	//

	IXMLDOMNodePtr getNamedItem(_bstr_t name);
	IXMLDOMNodePtr setNamedItem(struct IXMLDOMNode* newItem);
	IXMLDOMNodePtr removeNamedItem(_bstr_t name);
	IXMLDOMNodePtr Getitem(long index);
	long Getlength();
	IXMLDOMNodePtr getQualifiedItem(_bstr_t baseName, _bstr_t namespaceURI);
	IXMLDOMNodePtr removeQualifiedItem(_bstr_t baseName, _bstr_t namespaceURI);
	IXMLDOMNodePtr nextNode();
	HRESULT reset();
	IUnknownPtr Get_newEnum();

	//
	// Raw methods provided by interface
	//

	virtual HRESULT __stdcall raw_getNamedItem(
		/*[in]*/ BSTR name,
		/*[out,retval]*/ struct IXMLDOMNode** namedItem) = 0;
	virtual HRESULT __stdcall raw_setNamedItem(
		/*[in]*/ struct IXMLDOMNode* newItem,
		/*[out,retval]*/ struct IXMLDOMNode** nameItem) = 0;
	virtual HRESULT __stdcall raw_removeNamedItem(
		/*[in]*/ BSTR name,
		/*[out,retval]*/ struct IXMLDOMNode** namedItem) = 0;
	virtual HRESULT __stdcall get_item(
		/*[in]*/ long index,
		/*[out,retval]*/ struct IXMLDOMNode** listItem) = 0;
	virtual HRESULT __stdcall get_length(
		/*[out,retval]*/ long* listLength) = 0;
	virtual HRESULT __stdcall raw_getQualifiedItem(
		/*[in]*/ BSTR baseName,
		/*[in]*/ BSTR namespaceURI,
		/*[out,retval]*/ struct IXMLDOMNode** qualifiedItem) = 0;
	virtual HRESULT __stdcall raw_removeQualifiedItem(
		/*[in]*/ BSTR baseName,
		/*[in]*/ BSTR namespaceURI,
		/*[out,retval]*/ struct IXMLDOMNode** qualifiedItem) = 0;
	virtual HRESULT __stdcall raw_nextNode(
		/*[out,retval]*/ struct IXMLDOMNode** nextItem) = 0;
	virtual HRESULT __stdcall raw_reset()				= 0;
	virtual HRESULT __stdcall get__newEnum(
		/*[out,retval]*/ IUnknown** ppUnk) = 0;
};

struct __declspec(uuid("2933bf8b-7b36-11d2-b20e-00c04f983e60")) IXMLDOMDocumentType : IXMLDOMNode
{
	//
	// Property data
	//

	__declspec(property(get = Getname)) _bstr_t name;
	__declspec(property(get = Getentities)) IXMLDOMNamedNodeMapPtr entities;
	__declspec(property(get = Getnotations)) IXMLDOMNamedNodeMapPtr notations;

	//
	// Wrapper methods for error-handling
	//

	_bstr_t Getname();
	IXMLDOMNamedNodeMapPtr Getentities();
	IXMLDOMNamedNodeMapPtr Getnotations();

	//
	// Raw methods provided by interface
	//

	virtual HRESULT __stdcall get_name(
		/*[out,retval]*/ BSTR* rootName) = 0;
	virtual HRESULT __stdcall get_entities(
		/*[out,retval]*/ struct IXMLDOMNamedNodeMap** entityMap) = 0;
	virtual HRESULT __stdcall get_notations(
		/*[out,retval]*/ struct IXMLDOMNamedNodeMap** notationMap) = 0;
};

struct __declspec(uuid("2933bf85-7b36-11d2-b20e-00c04f983e60")) IXMLDOMAttribute : IXMLDOMNode
{
	//
	// Property data
	//

	__declspec(property(get = Getname)) _bstr_t name;
	__declspec(property(get = Getvalue, put = Putvalue)) _variant_t value;

	//
	// Wrapper methods for error-handling
	//

	_bstr_t Getname();
	_variant_t Getvalue();
	void Putvalue(const _variant_t& attributeValue);

	//
	// Raw methods provided by interface
	//

	virtual HRESULT __stdcall get_name(
		/*[out,retval]*/ BSTR* attributeName) = 0;
	virtual HRESULT __stdcall get_value(
		/*[out,retval]*/ VARIANT* attributeValue) = 0;
	virtual HRESULT __stdcall put_value(
		/*[in]*/ VARIANT attributeValue) = 0;
};

struct __declspec(uuid("2933bf86-7b36-11d2-b20e-00c04f983e60")) IXMLDOMElement : IXMLDOMNode
{
	//
	// Property data
	//

	__declspec(property(get = GettagName)) _bstr_t tagName;

	//
	// Wrapper methods for error-handling
	//

	_bstr_t GettagName();
	_variant_t getAttribute(_bstr_t name);
	HRESULT setAttribute(_bstr_t name, const _variant_t& value);
	HRESULT removeAttribute(_bstr_t name);
	IXMLDOMAttributePtr getAttributeNode(_bstr_t name);
	IXMLDOMAttributePtr setAttributeNode(struct IXMLDOMAttribute* DOMAttribute);
	IXMLDOMAttributePtr removeAttributeNode(struct IXMLDOMAttribute* DOMAttribute);
	IXMLDOMNodeListPtr getElementsByTagName(_bstr_t tagName);
	HRESULT normalize();

	//
	// Raw methods provided by interface
	//

	virtual HRESULT __stdcall get_tagName(
		/*[out,retval]*/ BSTR* tagName) = 0;
	virtual HRESULT __stdcall raw_getAttribute(
		/*[in]*/ BSTR name,
		/*[out,retval]*/ VARIANT* value) = 0;
	virtual HRESULT __stdcall raw_setAttribute(
		/*[in]*/ BSTR name,
		/*[in]*/ VARIANT value) = 0;
	virtual HRESULT __stdcall raw_removeAttribute(
		/*[in]*/ BSTR name) = 0;
	virtual HRESULT __stdcall raw_getAttributeNode(
		/*[in]*/ BSTR name,
		/*[out,retval]*/ struct IXMLDOMAttribute** attributeNode) = 0;
	virtual HRESULT __stdcall raw_setAttributeNode(
		/*[in]*/ struct IXMLDOMAttribute* DOMAttribute,
		/*[out,retval]*/ struct IXMLDOMAttribute** attributeNode) = 0;
	virtual HRESULT __stdcall raw_removeAttributeNode(
		/*[in]*/ struct IXMLDOMAttribute* DOMAttribute,
		/*[out,retval]*/ struct IXMLDOMAttribute** attributeNode) = 0;
	virtual HRESULT __stdcall raw_getElementsByTagName(
		/*[in]*/ BSTR tagName,
		/*[out,retval]*/ struct IXMLDOMNodeList** resultList) = 0;
	virtual HRESULT __stdcall raw_normalize()				  = 0;
};

struct __declspec(uuid("3efaa413-272f-11d2-836f-0000f87a7782")) IXMLDOMDocumentFragment
	: IXMLDOMNode
{
};

struct __declspec(uuid("2933bf84-7b36-11d2-b20e-00c04f983e60")) IXMLDOMCharacterData : IXMLDOMNode
{
	//
	// Property data
	//

	__declspec(property(get = Getdata, put = Putdata)) _bstr_t data;
	__declspec(property(get = Getlength)) long length;

	//
	// Wrapper methods for error-handling
	//

	_bstr_t Getdata();
	void Putdata(_bstr_t data);
	long Getlength();
	_bstr_t substringData(long offset, long count);
	HRESULT appendData(_bstr_t data);
	HRESULT insertData(long offset, _bstr_t data);
	HRESULT deleteData(long offset, long count);
	HRESULT replaceData(long offset, long count, _bstr_t data);

	//
	// Raw methods provided by interface
	//

	virtual HRESULT __stdcall get_data(
		/*[out,retval]*/ BSTR* data) = 0;
	virtual HRESULT __stdcall put_data(
		/*[in]*/ BSTR data) = 0;
	virtual HRESULT __stdcall get_length(
		/*[out,retval]*/ long* dataLength) = 0;
	virtual HRESULT __stdcall raw_substringData(
		/*[in]*/ long offset,
		/*[in]*/ long count,
		/*[out,retval]*/ BSTR* data) = 0;
	virtual HRESULT __stdcall raw_appendData(
		/*[in]*/ BSTR data) = 0;
	virtual HRESULT __stdcall raw_insertData(
		/*[in]*/ long offset,
		/*[in]*/ BSTR data) = 0;
	virtual HRESULT __stdcall raw_deleteData(
		/*[in]*/ long offset,
		/*[in]*/ long count) = 0;
	virtual HRESULT __stdcall raw_replaceData(
		/*[in]*/ long offset,
		/*[in]*/ long count,
		/*[in]*/ BSTR data) = 0;
};

struct __declspec(uuid("2933bf87-7b36-11d2-b20e-00c04f983e60")) IXMLDOMText : IXMLDOMCharacterData
{
	//
	// Wrapper methods for error-handling
	//

	IXMLDOMTextPtr splitText(long offset);

	//
	// Raw methods provided by interface
	//

	virtual HRESULT __stdcall raw_splitText(
		/*[in]*/ long offset,
		/*[out,retval]*/ struct IXMLDOMText** rightHandTextNode) = 0;
};

struct __declspec(uuid("2933bf88-7b36-11d2-b20e-00c04f983e60")) IXMLDOMComment
	: IXMLDOMCharacterData
{
};

struct __declspec(uuid("2933bf8a-7b36-11d2-b20e-00c04f983e60")) IXMLDOMCDATASection : IXMLDOMText
{
};

struct __declspec(uuid("2933bf89-7b36-11d2-b20e-00c04f983e60")) IXMLDOMProcessingInstruction
	: IXMLDOMNode
{
	//
	// Property data
	//

	__declspec(property(get = Getdata, put = Putdata)) _bstr_t data;
	__declspec(property(get = Gettarget)) _bstr_t target;

	//
	// Wrapper methods for error-handling
	//

	_bstr_t Gettarget();
	_bstr_t Getdata();
	void Putdata(_bstr_t value);

	//
	// Raw methods provided by interface
	//

	virtual HRESULT __stdcall get_target(
		/*[out,retval]*/ BSTR* name) = 0;
	virtual HRESULT __stdcall get_data(
		/*[out,retval]*/ BSTR* value) = 0;
	virtual HRESULT __stdcall put_data(
		/*[in]*/ BSTR value) = 0;
};

struct __declspec(uuid("2933bf8e-7b36-11d2-b20e-00c04f983e60")) IXMLDOMEntityReference : IXMLDOMNode
{
};

struct __declspec(uuid("373984c8-b845-449b-91e7-45ac83036ade")) IXMLDOMSchemaCollection
	: public IDispatch
{
	virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE add(
		/* [in] */ BSTR namespaceURI,
		/* [in] */ VARIANT var) = 0;

	virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE get(
		/* [in] */ BSTR namespaceURI,
		/* [retval][out] */ IXMLDOMNode** schemaNode) = 0;

	virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE remove(
		/* [in] */ BSTR namespaceURI) = 0;

	virtual /* [propget][helpstring][id] */ HRESULT STDMETHODCALLTYPE get_length(
		/* [retval][out] */ long* length) = 0;

	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_namespaceURI(
		/* [in] */ long index,
		/* [retval][out] */ BSTR* length) = 0;

	virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE addCollection(
		/* [in] */ IXMLDOMSchemaCollection* otherCollection) = 0;

	virtual /* [id][hidden][restricted][propget] */ HRESULT STDMETHODCALLTYPE get__newEnum(
		/* [out][retval] */ IUnknown** ppUnk) = 0;
};

struct __declspec(uuid("2933bf81-7b36-11d2-b20e-00c04f983e60")) IXMLDOMDocument : IXMLDOMNode
{
	//
	// Property data
	//

	__declspec(property(get = GetvalidateOnParse,
						put = PutvalidateOnParse)) VARIANT_BOOL validateOnParse;
	__declspec(property(get = GetresolveExternals,
						put = PutresolveExternals)) VARIANT_BOOL resolveExternals;
	__declspec(property(get = GetpreserveWhiteSpace,
						put = PutpreserveWhiteSpace)) VARIANT_BOOL preserveWhiteSpace;
	__declspec(property(put = Putonreadystatechange)) _variant_t onreadystatechange;
	__declspec(property(put = Putondataavailable)) _variant_t ondataavailable;
	__declspec(property(get = Getdoctype)) IXMLDOMDocumentTypePtr doctype;
	__declspec(property(get = Getimplementation)) IXMLDOMImplementationPtr implementation;
	__declspec(property(get = GetdocumentElement,
						put = PutRefdocumentElement)) IXMLDOMElementPtr documentElement;
	__declspec(property(put = Putontransformnode)) _variant_t ontransformnode;
	__declspec(property(get = GetreadyState)) long readyState;
	__declspec(property(get = GetparseError)) IXMLDOMParseErrorPtr parseError;
	__declspec(property(get = Geturl)) _bstr_t url;
	__declspec(property(get = Getasync, put = Putasync)) VARIANT_BOOL async;

	//
	// Wrapper methods for error-handling
	//

	IXMLDOMDocumentTypePtr Getdoctype();
	IXMLDOMImplementationPtr Getimplementation();
	IXMLDOMElementPtr GetdocumentElement();
	void PutRefdocumentElement(struct IXMLDOMElement* DOMElement);
	IXMLDOMElementPtr createElement(_bstr_t tagName);
	IXMLDOMDocumentFragmentPtr createDocumentFragment();
	IXMLDOMTextPtr createTextNode(_bstr_t data);
	IXMLDOMCommentPtr createComment(_bstr_t data);
	IXMLDOMCDATASectionPtr createCDATASection(_bstr_t data);
	IXMLDOMProcessingInstructionPtr createProcessingInstruction(_bstr_t target, _bstr_t data);
	IXMLDOMAttributePtr createAttribute(_bstr_t name);
	IXMLDOMEntityReferencePtr createEntityReference(_bstr_t name);
	IXMLDOMNodeListPtr getElementsByTagName(_bstr_t tagName);
	IXMLDOMNodePtr createNode(const _variant_t& type, _bstr_t name, _bstr_t namespaceURI);
	IXMLDOMNodePtr nodeFromID(_bstr_t idString);
	VARIANT_BOOL load(const _variant_t& xmlSource);
	long GetreadyState();
	IXMLDOMParseErrorPtr GetparseError();
	_bstr_t Geturl();
	VARIANT_BOOL Getasync();
	void Putasync(VARIANT_BOOL isAsync);
	HRESULT abort();
	VARIANT_BOOL loadXML(_bstr_t bstrXML);
	HRESULT save(const _variant_t& destination);
	VARIANT_BOOL GetvalidateOnParse();
	void PutvalidateOnParse(VARIANT_BOOL isValidating);
	VARIANT_BOOL GetresolveExternals();
	void PutresolveExternals(VARIANT_BOOL isResolving);
	VARIANT_BOOL GetpreserveWhiteSpace();
	void PutpreserveWhiteSpace(VARIANT_BOOL isPreserving);
	void Putonreadystatechange(const _variant_t& _arg1);
	void Putondataavailable(const _variant_t& _arg1);
	void Putontransformnode(const _variant_t& _arg1);

	//
	// Raw methods provided by interface
	//

	virtual HRESULT __stdcall get_doctype(
		/*[out,retval]*/ struct IXMLDOMDocumentType** documentType) = 0;
	virtual HRESULT __stdcall get_implementation(
		/*[out,retval]*/ struct IXMLDOMImplementation** impl) = 0;
	virtual HRESULT __stdcall get_documentElement(
		/*[out,retval]*/ struct IXMLDOMElement** DOMElement) = 0;
	virtual HRESULT __stdcall putref_documentElement(
		/*[in]*/ struct IXMLDOMElement* DOMElement) = 0;
	virtual HRESULT __stdcall raw_createElement(
		/*[in]*/ BSTR tagName,
		/*[out,retval]*/ struct IXMLDOMElement** element) = 0;
	virtual HRESULT __stdcall raw_createDocumentFragment(
		/*[out,retval]*/ struct IXMLDOMDocumentFragment** docFrag) = 0;
	virtual HRESULT __stdcall raw_createTextNode(
		/*[in]*/ BSTR data,
		/*[out,retval]*/ struct IXMLDOMText** text) = 0;
	virtual HRESULT __stdcall raw_createComment(
		/*[in]*/ BSTR data,
		/*[out,retval]*/ struct IXMLDOMComment** comment) = 0;
	virtual HRESULT __stdcall raw_createCDATASection(
		/*[in]*/ BSTR data,
		/*[out,retval]*/ struct IXMLDOMCDATASection** cdata) = 0;
	virtual HRESULT __stdcall raw_createProcessingInstruction(
		/*[in]*/ BSTR target,
		/*[in]*/ BSTR data,
		/*[out,retval]*/ struct IXMLDOMProcessingInstruction** pi) = 0;
	virtual HRESULT __stdcall raw_createAttribute(
		/*[in]*/ BSTR name,
		/*[out,retval]*/ struct IXMLDOMAttribute** attribute) = 0;
	virtual HRESULT __stdcall raw_createEntityReference(
		/*[in]*/ BSTR name,
		/*[out,retval]*/ struct IXMLDOMEntityReference** entityRef) = 0;
	virtual HRESULT __stdcall raw_getElementsByTagName(
		/*[in]*/ BSTR tagName,
		/*[out,retval]*/ struct IXMLDOMNodeList** resultList) = 0;
	virtual HRESULT __stdcall raw_createNode(
		/*[in]*/ VARIANT type,
		/*[in]*/ BSTR name,
		/*[in]*/ BSTR namespaceURI,
		/*[out,retval]*/ struct IXMLDOMNode** node) = 0;
	virtual HRESULT __stdcall raw_nodeFromID(
		/*[in]*/ BSTR idString,
		/*[out,retval]*/ struct IXMLDOMNode** node) = 0;
	virtual HRESULT __stdcall raw_load(
		/*[in]*/ VARIANT xmlSource,
		/*[out,retval]*/ VARIANT_BOOL* isSuccessful) = 0;
	virtual HRESULT __stdcall get_readyState(
		/*[out,retval]*/ long* value) = 0;
	virtual HRESULT __stdcall get_parseError(
		/*[out,retval]*/ struct IXMLDOMParseError** errorObj) = 0;
	virtual HRESULT __stdcall get_url(
		/*[out,retval]*/ BSTR* urlString) = 0;
	virtual HRESULT __stdcall get_async(
		/*[out,retval]*/ VARIANT_BOOL* isAsync) = 0;
	virtual HRESULT __stdcall put_async(
		/*[in]*/ VARIANT_BOOL isAsync)	  = 0;
	virtual HRESULT __stdcall raw_abort() = 0;
	virtual HRESULT __stdcall raw_loadXML(
		/*[in]*/ BSTR bstrXML,
		/*[out,retval]*/ VARIANT_BOOL* isSuccessful) = 0;
	virtual HRESULT __stdcall raw_save(
		/*[in]*/ VARIANT destination) = 0;
	virtual HRESULT __stdcall get_validateOnParse(
		/*[out,retval]*/ VARIANT_BOOL* isValidating) = 0;
	virtual HRESULT __stdcall put_validateOnParse(
		/*[in]*/ VARIANT_BOOL isValidating) = 0;
	virtual HRESULT __stdcall get_resolveExternals(
		/*[out,retval]*/ VARIANT_BOOL* isResolving) = 0;
	virtual HRESULT __stdcall put_resolveExternals(
		/*[in]*/ VARIANT_BOOL isResolving) = 0;
	virtual HRESULT __stdcall get_preserveWhiteSpace(
		/*[out,retval]*/ VARIANT_BOOL* isPreserving) = 0;
	virtual HRESULT __stdcall put_preserveWhiteSpace(
		/*[in]*/ VARIANT_BOOL isPreserving) = 0;
	virtual HRESULT __stdcall put_onreadystatechange(
		/*[in]*/ VARIANT _arg1) = 0;
	virtual HRESULT __stdcall put_ondataavailable(
		/*[in]*/ VARIANT _arg1) = 0;
	virtual HRESULT __stdcall put_ontransformnode(
		/*[in]*/ VARIANT _arg1) = 0;
};

struct __declspec(uuid("2933bf95-7b36-11d2-b20e-00c04f983e60")) IXMLDOMDocument2 : IXMLDOMDocument
{
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_namespaces(
		/* [retval][out] */ IXMLDOMSchemaCollection** namespaceCollection) = 0;

	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_schemas(
		/* [retval][out] */ VARIANT* otherCollection) = 0;

	virtual /* [helpstring][id][propputref] */ HRESULT STDMETHODCALLTYPE putref_schemas(
		/* [in] */ VARIANT otherCollection) = 0;

	virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE validate(
		/* [out][retval] */ IXMLDOMParseError** errorObj) = 0;

	virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE setProperty(
		/* [in] */ BSTR name,
		/* [in] */ VARIANT value) = 0;

	virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE getProperty(
		/* [in] */ BSTR name,
		/* [retval][out] */ VARIANT* value) = 0;
};

struct __declspec(uuid("2933bf8c-7b36-11d2-b20e-00c04f983e60")) IXMLDOMNotation : IXMLDOMNode
{
	//
	// Property data
	//

	__declspec(property(get = GetpublicId)) _variant_t publicId;
	__declspec(property(get = GetsystemId)) _variant_t systemId;

	//
	// Wrapper methods for error-handling
	//

	_variant_t GetpublicId();
	_variant_t GetsystemId();

	//
	// Raw methods provided by interface
	//

	virtual HRESULT __stdcall get_publicId(
		/*[out,retval]*/ VARIANT* publicId) = 0;
	virtual HRESULT __stdcall get_systemId(
		/*[out,retval]*/ VARIANT* systemId) = 0;
};

struct __declspec(uuid("2933bf8d-7b36-11d2-b20e-00c04f983e60")) IXMLDOMEntity : IXMLDOMNode
{
	//
	// Property data
	//

	__declspec(property(get = GetpublicId)) _variant_t publicId;
	__declspec(property(get = GetsystemId)) _variant_t systemId;
	__declspec(property(get = GetnotationName)) _bstr_t notationName;

	//
	// Wrapper methods for error-handling
	//

	_variant_t GetpublicId();
	_variant_t GetsystemId();
	_bstr_t GetnotationName();

	//
	// Raw methods provided by interface
	//

	virtual HRESULT __stdcall get_publicId(
		/*[out,retval]*/ VARIANT* publicId) = 0;
	virtual HRESULT __stdcall get_systemId(
		/*[out,retval]*/ VARIANT* systemId) = 0;
	virtual HRESULT __stdcall get_notationName(
		/*[out,retval]*/ BSTR* name) = 0;
};

struct __declspec(uuid("3efaa425-272f-11d2-836f-0000f87a7782")) IXTLRuntime : IXMLDOMNode
{
	//
	// Wrapper methods for error-handling
	//

	long uniqueID(struct IXMLDOMNode* pNode);
	long depth(struct IXMLDOMNode* pNode);
	long childNumber(struct IXMLDOMNode* pNode);
	long ancestorChildNumber(_bstr_t bstrNodeName, struct IXMLDOMNode* pNode);
	long absoluteChildNumber(struct IXMLDOMNode* pNode);
	_bstr_t formatIndex(long lIndex, _bstr_t bstrFormat);
	_bstr_t formatNumber(double dblNumber, _bstr_t bstrFormat);
	_bstr_t formatDate(const _variant_t& varDate, _bstr_t bstrFormat,
					   const _variant_t& varDestLocale = vtMissing);
	_bstr_t formatTime(const _variant_t& varTime, _bstr_t bstrFormat,
					   const _variant_t& varDestLocale = vtMissing);

	//
	// Raw methods provided by interface
	//

	virtual HRESULT __stdcall raw_uniqueID(
		/*[in]*/ struct IXMLDOMNode* pNode,
		/*[out,retval]*/ long* pID) = 0;
	virtual HRESULT __stdcall raw_depth(
		/*[in]*/ struct IXMLDOMNode* pNode,
		/*[out,retval]*/ long* pDepth) = 0;
	virtual HRESULT __stdcall raw_childNumber(
		/*[in]*/ struct IXMLDOMNode* pNode,
		/*[out,retval]*/ long* pNumber) = 0;
	virtual HRESULT __stdcall raw_ancestorChildNumber(
		/*[in]*/ BSTR bstrNodeName,
		/*[in]*/ struct IXMLDOMNode* pNode,
		/*[out,retval]*/ long* pNumber) = 0;
	virtual HRESULT __stdcall raw_absoluteChildNumber(
		/*[in]*/ struct IXMLDOMNode* pNode,
		/*[out,retval]*/ long* pNumber) = 0;
	virtual HRESULT __stdcall raw_formatIndex(
		/*[in]*/ long lIndex,
		/*[in]*/ BSTR bstrFormat,
		/*[out,retval]*/ BSTR* pbstrFormattedString) = 0;
	virtual HRESULT __stdcall raw_formatNumber(
		/*[in]*/ double dblNumber,
		/*[in]*/ BSTR bstrFormat,
		/*[out,retval]*/ BSTR* pbstrFormattedString) = 0;
	virtual HRESULT __stdcall raw_formatDate(
		/*[in]*/ VARIANT varDate,
		/*[in]*/ BSTR bstrFormat,
		/*[in]*/ VARIANT varDestLocale,
		/*[out,retval]*/ BSTR* pbstrFormattedString) = 0;
	virtual HRESULT __stdcall raw_formatTime(
		/*[in]*/ VARIANT varTime,
		/*[in]*/ BSTR bstrFormat,
		/*[in]*/ VARIANT varDestLocale,
		/*[out,retval]*/ BSTR* pbstrFormattedString) = 0;
};

struct __declspec(uuid("310afa62-0575-11d2-9ca9-0060b0ec3d39")) IXMLDSOControl : IDispatch
{
	//
	// Property data
	//

	__declspec(property(get = GetXMLDocument, put = PutXMLDocument)) IXMLDOMDocumentPtr XMLDocument;
	__declspec(property(get = GetJavaDSOCompatible,
						put = PutJavaDSOCompatible)) long JavaDSOCompatible;
	__declspec(property(get = GetreadyState)) long readyState;

	//
	// Wrapper methods for error-handling
	//

	IXMLDOMDocumentPtr GetXMLDocument();
	void PutXMLDocument(struct IXMLDOMDocument* ppDoc);
	long GetJavaDSOCompatible();
	void PutJavaDSOCompatible(long fJavaDSOCompatible);
	long GetreadyState();

	//
	// Raw methods provided by interface
	//

	virtual HRESULT __stdcall get_XMLDocument(
		/*[out,retval]*/ struct IXMLDOMDocument** ppDoc) = 0;
	virtual HRESULT __stdcall put_XMLDocument(
		/*[in]*/ struct IXMLDOMDocument* ppDoc) = 0;
	virtual HRESULT __stdcall get_JavaDSOCompatible(
		/*[out,retval]*/ long* fJavaDSOCompatible) = 0;
	virtual HRESULT __stdcall put_JavaDSOCompatible(
		/*[in]*/ long fJavaDSOCompatible) = 0;
	virtual HRESULT __stdcall get_readyState(
		/*[out,retval]*/ long* state) = 0;
};

//
// Wrapper method implementations
//

#		include "Common/XTPXMLHelpers.inl"

} // namespace XTPXML

// {2933BF90-7B36-11D2-B20E-00C04F983E60}
extern "C" const GUID __declspec(selectany) CLSID_XTPDOMDocument = { 0x2933bf90,
																	 0x7b36,
																	 0x11d2,
																	 { 0xb2, 0x0e, 0x00, 0xc0, 0x4f,
																	   0x98, 0x3e, 0x60 } };

// {F5078F32-C551-11D3-89B9-0000F81FE221}
extern "C" const GUID __declspec(selectany) CLSID_XTPDOMDocument3 = { 0xF5078F32,
																	  0xC551,
																	  0x11D3,
																	  { 0x89, 0xB9, 0x00, 0x00,
																		0xF8, 0x1F, 0xE2, 0x21 } };

// {88D969E5-F192-11D4-A65F-0040963251E5}
extern "C" const GUID __declspec(selectany) CLSID_XTPDOMDocument5 = { 0x88D969E5,
																	  0xF192,
																	  0x11D4,
																	  { 0xA6, 0x5F, 0x00, 0x40,
																		0x96, 0x32, 0x51, 0xE5 } };

// {88d96a05-f192-11d4-a65f-0040963251e5}
extern "C" const GUID __declspec(selectany) CLSID_XTPDOMDocument6 = { 0x88d96a05,
																	  0xf192,
																	  0x11d4,
																	  { 0xa6, 0x5f, 0x00, 0x40,
																		0x96, 0x32, 0x51, 0xe5 } };

#		if (XTP_MSXML_VERSION == 0)
#			define XTP_MSXML_CSLID_DOMDOCUMENT CLSID_XTPDOMDocument
#		elif (XTP_MSXML_VERSION == 3)
#			define XTP_MSXML_CSLID_DOMDOCUMENT CLSID_XTPDOMDocument3
#		elif (XTP_MSXML_VERSION == 5)
#			define XTP_MSXML_CSLID_DOMDOCUMENT CLSID_XTPDOMDocument5
#		elif (XTP_MSXML_VERSION == 6)
#			define XTP_MSXML_CSLID_DOMDOCUMENT CLSID_XTPDOMDocument6
#		else
#			error XTP_MSXML_VERSION has an unknown value. Supported values include: 0, 3, 5, 6
#		endif

//}}AFX_CODEJOCK_PRIVATE

#		if (_MSC_VER > 1100)
#			pragma warning(pop)
#		endif

#		pragma pack(pop)

typedef XTPXML::IXMLDOMNodePtr CXTPDOMNodePtr; // Wrapper to manage IXMLDOMNodePtr interface.
typedef XTPXML::IXMLDOMDocumentPtr CXTPDOMDocumentPtr; // Wrapper to manage IXMLDOMDocumentPtr
													   // interface.
typedef XTPXML::IXMLDOMElementPtr CXTPDOMElementPtr;   // Wrapper to manage IXMLDOMElementPtr
													   // interface.
typedef XTPXML::IXMLDOMNamedNodeMapPtr CXTPDOMNamedNodeMapPtr; // Wrapper to manage
															   // IXMLDOMNamedNodeMapPtr interface.

//-----------------------------------------------------------------------
// Summary:
//      Replaces all reserved characters in the provided string accoring
//      to XML escape rules so that the returned string can be used in
//      XML concatenation operations.
// Parameters:
//      lpString - A pointer to the raw string to be escaped.
//      lpDefaultValue - A pointer to default string to be used as a result
//                       value in case of internal error.
// Returns:
//      Escaped string according to XML rules.
//-----------------------------------------------------------------------
_XTP_EXT_CLASS CString AFX_CDECL XTPEscapeXmlString(LPCTSTR lpString,
													LPCTSTR lpDefaultValue = _T(""));

//-----------------------------------------------------------------------
// Summary:
//		The enumeration defines common XML DOM Level2 properties to be changed
//		in a document. The default values of those properties depend on XML
//		version being used and are described in details at
//		https://docs.microsoft.com/en-us/previous-versions/windows/desktop/ms766391(v=vs.85).
//		xtpXmlDomUseXPath and xtpXmlDomUseXSLPattern are the shortcuts for
//		the 'SelectionLanguage' XML property, those two flags must not be set together.
// See Also:
//		XTPCreateXmlDocumentInstance, XTP_MSXML_VERSION
//-----------------------------------------------------------------------
enum XTPXmlDomProperties
{
	// Enables (true) or disables (false) the document function in XLST. Attempts to call the
	// disabled document function will result in an "Access Denied" error.
	xtpXmlDomAllowDocumentFunction = 0x1,

	// Enables (true) or disables (false) the <msxsl:script> element functionality in XLST. If this
	// property is set to false, attempts to use a stylesheet that uses script will result in a
	// "Security settings do not allow the execution of script code within this stylesheet" error.
	xtpXmlDomAllowXsltScript = 0x2,

	// When you load and parse an XML document from a URL that is not local (i.e. is on a different
	// web site), that document may be cached by any number of servers on the line. If you are
	// parsing an XML document that changes often, and it is important that your parser always
	// retrieves the latest version of the document, you can set this property to true. When you set
	// this property to true, it causes the XML document to not be cached; instead it is retrieved
	// from its source server each time that you reparse the document.
	xtpXmlDomForcedResync = 0x4,

	// Specifies whether or not a ParseError object can return multiple validation errors through
	// the IXMLDOMParseError2 interface. To get all validation errors, set this property to true.
	// Otherwise, only a single validation error is returned.
	xtpXmlDomMultipleErrorMessages = 0x8,

	// Specifies whether to enable (true) or disable (false) the use of the new parser, which was
	// introduced in MSXML 4.0, to load a DOM document. Setting the NewParser property to false
	// causes subsequent DOM documents to be loaded using the old parser. Setting this property to
	// true causes DOM documents to be loaded using the new parser.
	xtpXmlDomNewParser = 0x10,

	// Specifies whether to enable (true) or disable (false) the normalization of attribute values.
	// Setting the NormalizeAttributeValues property to true causes the DOM document to report
	// normalized attribute values, and to serialize per the specification. Setting this property to
	// false causes attribute values to have the existing, non-conformant behavior.
	xtpXmlDomNormalizeAttributeValues = 0x20,

	// Enables (true) or disables (false) population of default values when the 'validate' method is
	// called.
	xtpXmlDomPopulateElementDefaultValues = 0x40,

	// Specifies whether to prohibit (true) or allow (false) the inclusion of a DTD in the XML DOM
	// document.
	xtpXmlDomProhibitDTD = 0x80,

	// Specifies whether external definitions, resolvable namespaces, external subsets of document
	// type definitions (DTDs), and external entity references are to be resolved at the time the
	// document is parsed.
	xtpXmlDomResolveExternals = 0x100,

	// Specifies whether to enable (true) or disable (false) the use of the ServerHTTPRequest object
	// in a server application. Setting the property to false causes the DOM object not to use the
	// HTTPRequest object. Setting this property to true causes DOM documents to use
	// ServerHTTPRequest.
	xtpXmlDomServerHTTPRequest = 0x200,

	// Specifies whether inline schemas should be processed (true) or not (false). When this
	// property is set to true, inline schemas are used for validation. When this property is set to
	// false, inline schemas are treated like any other XML fragments.
	xtpXmlDomUseInlineSchema = 0x400,

	// This property is used to specify whether the XML parser should validate (true) this document
	// against document type definition (DTD), schema, or schema cache on loading.
	xtpXmlDomValidateOnParse = 0x800,

	// Sets XPath as document's selection language. Must not be combined with
	// xtpXmlDomUseXSLPattern.
	xtpXmlDomUseXPath = 0x1000,

	// Sets XSLPattern as document's selection language. Must not be combined with
	// xtpXmlDomUseXPath.
	xtpXmlDomUseXSLPattern = 0x2000,

	// A set of enabled by default flags for the best performance, compliance and security.
	xtpXmlDomDefaultEnabled = xtpXmlDomForcedResync | xtpXmlDomNewParser | xtpXmlDomProhibitDTD
							  | xtpXmlDomUseXPath,

	// A set of disabled by default flags for the best performance, compliance and security.
	xtpXmlDomDefaultDisabled = xtpXmlDomAllowDocumentFunction | xtpXmlDomAllowXsltScript
							   | xtpXmlDomMultipleErrorMessages | xtpXmlDomNormalizeAttributeValues
							   | xtpXmlDomPopulateElementDefaultValues | xtpXmlDomResolveExternals
							   | xtpXmlDomValidateOnParse | xtpXmlDomServerHTTPRequest
};

//-----------------------------------------------------------------------
// Summary:
//      Creates an XML document instance of a various available MSXML document class type.
// Parameters:
//      fExact - If TRUE it'll attempt to create the document of the specified class type only.
//               If FALSE it'll attempt creating document instance with the highest available
//               know MSXML class version but not lower than XTP default MSXML class version.
//      pClsid - Specified exact or the least acceptable MSXML class version value.
//               If no class ID is provided the XTP default MSXML class id will be used.
//      fEnableProps - A set of XTPXmlDomProperties flags for properties to be explicitly enabled
//				regadless their default value. The default values are set for the best performance
//				and security.
//      fDisableProps - A set of XTPXmlDomProperties flags for properties to be explicitly disabled
//				regadless their default value. The default values are set for the best performance
//				and security. xtpXmlDomUseXPath and xtpXmlDomUseXSLPattern cannot be disabled
//				so they are ignored.
// Returns:
//      MSXML document instance pointer.
// See also:
//		XTP_MSXML_VERSION, XTPXmlDomProperties
//-----------------------------------------------------------------------
_XTP_EXT_CLASS CXTPDOMDocumentPtr AFX_CDECL XTPCreateXmlDocumentInstance(
	BOOL fExact = FALSE, const CLSID* pClsid = NULL, int fEnableProps = xtpXmlDomDefaultEnabled,
	int fDisableProps = xtpXmlDomDefaultDisabled);

#		include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#	endif //_XTP_EXCLUDE_XML

#endif //#if !defined(__XTPXMLHELPERS_H__)
