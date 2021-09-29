// XTPMarkupParser.h: interface for the CXTPMarkupParser class.
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
#if !defined(__XTPMARKUPPARSER_H__)
#	define __XTPMARKUPPARSER_H__
//}}AFX_CODEJOCK_PRIVATE

#	if _MSC_VER > 1000
#		pragma once
#	endif // _MSC_VER > 1000

#	include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

//{{AFX_CODEJOCK_PRIVATE

class CXTPComInitializer;

#	define XTP_MAX_TOKEN_SIZE 1024
#	define XTP_MAX_NAME_SIZE 128

//===========================================================================
// Summary:
//     Internal class used to parse XAML text to a Markup tree.
//===========================================================================
class _XTP_EXT_CLASS CXTPMarkupParser
{
public:
	CXTPMarkupParser();
	virtual ~CXTPMarkupParser();

public:
	//-------------------------------------------------------------------------
	// Summary:
	//     Sets ANSI XML data.
	// Parameters:
	//     lpszStart - Pointer to the first character of the sequence to parse.
	//     lpszEnd   - Pointer to the next after the last character of the sequence to parse.
	// Returns:
	//     TRUE if successful.
	//-------------------------------------------------------------------------
	BOOL SetBuffer(LPCSTR lpszStart, LPCSTR lpszEnd);

	//-------------------------------------------------------------------------
	// Summary:
	//     Sets Unicode XML data.
	// Parameters:
	//     lpszStart - Pointer to the first character of the sequence to parse.
	//     lpszEnd   - Pointer to the next after the last character of the sequence to parse.
	// Returns:
	//     TRUE if successful.
	//-------------------------------------------------------------------------
	BOOL SetBuffer(LPCWSTR lpszStart, LPCWSTR lpszEnd);

	//-------------------------------------------------------------------------
	// Summary:
	//     Reads XML data from a stream.
	// Parameters:
	//     pStream - Pointer to the stream to read XML data from.
	// Returns:
	//     TRUE if successful.
	//-------------------------------------------------------------------------
	BOOL SetDataStream(IStream* pStream);

	//-------------------------------------------------------------------------
	// Summary:
	//     Provides access to the XML document instance.
	// Returns:
	//     A valid pointer to the XML document instance, or an empty pointer
	//     if no XML document instance is loaded.
	//-------------------------------------------------------------------------
	XTPXML::IXMLDOMDocumentPtr GetXmlDocument();

	//-------------------------------------------------------------------------
	// Summary:
	//     Provides information about loading and parsing an XML error.
	//-------------------------------------------------------------------------
	struct ErrorInfo
	{
		HRESULT nCode;	   // Error code.
		BOOL bXml;		   // If TRUE, then the following fields contain valid values.
		long nLine;		   // Line number where an error occured.
		long nPosition;	   // Position in a line where an error occured.
		_bstr_t strReason; // Error description.
		_bstr_t strSource; // Source line where an error occured.
	};

	//-------------------------------------------------------------------------
	// Summary:
	//     Provides access to error information.
	// Returns:
	//     A reference to error information.
	//-------------------------------------------------------------------------
	const ErrorInfo& GetErrorInfo() const;

	//-------------------------------------------------------------------------
	// Summary:
	//     Formats a user friendly error message.
	// Returns:
	//     A user friendly error message.
	//-------------------------------------------------------------------------
	CString FormatErrorMessage() const;

	//-------------------------------------------------------------------------
	// Summary:
	//     Formats a user friendly markup error message.
	// Returns:
	//     A user friendly markup error message.
	//-------------------------------------------------------------------------
	CString FormatMarkupErrorMessage() const;

private:
	struct MEMORY_BLOB
	{
		SIZE_T cbSize;
		LPCVOID pBlobData;
	};

	BOOL CreateDataStream(LPCSTR lpString, SIZE_T cch, IStream** ppStream) const;
	BOOL CreateDataStream(LPCWSTR lpString, SIZE_T cch, IStream** ppStream) const;
	BOOL CreateDataStream(const MEMORY_BLOB* pBlobs, UINT nBlobs, IStream** ppStream) const;

	void ObtainComErrorInfo(HRESULT hr) const;
	void ObtainXmlErrorInfo(XTPXML::IXMLDOMDocument* pXmlDocument, HRESULT hr = S_OK);
	void ResetErrorInfo();

	void ReleaseXml();

	HGLOBAL ReadStreamData(IStream* pStream) const;
	LPWSTR ReadStreamDataAsUnicode(IStream* pStream, UINT* pchUnicodeText) const;

	// Backward compatibility tools.
	BOOL FixReferenceToUndeclaredNamespacePrefixX(IStream* pInputStream,
												  IStream** ppOutputStream) const;
	BOOL FixRequiredWhiteSpaceWasMissing(IStream* pInputStream, IStream** ppOutputStream) const;

private:
	CXTPComInitializer* m_pComInitializer;
	XTPXML::IXMLDOMDocumentPtr m_pXmlDocument;
	mutable ErrorInfo m_errorInfo;
	BOOL m_bInFixReferenceToUndeclaredNamespacePrefixX;
	static const BYTE UnicodeBOM[2];
};

AFX_INLINE const CXTPMarkupParser::ErrorInfo& CXTPMarkupParser::GetErrorInfo() const
{
	return m_errorInfo;
}

AFX_INLINE XTPXML::IXMLDOMDocumentPtr CXTPMarkupParser::GetXmlDocument()
{
	return m_pXmlDocument;
}

//}}AFX_CODEJOCK_PRIVATE

#	include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPMARKUPPARSER_H__)
