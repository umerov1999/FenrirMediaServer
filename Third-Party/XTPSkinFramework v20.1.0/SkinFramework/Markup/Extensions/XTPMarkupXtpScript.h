// XTPMarkupXtpScript.h: interface for the CXTPMarkupXtpScript class.
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
#if !defined(__XTPMARKUPXTPSCRIPT_H__)
#	define __XTPMARKUPXTPSCRIPT_H__
//}}AFX_CODEJOCK_PRIVATE

#	if _MSC_VER > 1000
#		pragma once
#	endif // _MSC_VER > 1000

#	include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

class CXTPMarkupDependencyProperty;
class CXTPMarkupBuilder;
class CXTPMarkupObject;

//===========================================================================
// Summary:
//     Contains a code in the script language chosen.
//===========================================================================
class _XTP_EXT_CLASS CXTPMarkupXtpScript : public CXTPMarkupExtension
{
	DECLARE_MARKUPCLASS(CXTPMarkupXtpScript);

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Obtains the script language name, which is the same as the script
	//     language AppID.
	// Returns:
	//     The script language name.
	//-----------------------------------------------------------------------
	LPCWSTR GetLanguage() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Obtains the referenced script location.
	// Returns:
	//     The referenced script location.
	//-----------------------------------------------------------------------
	LPCWSTR GetSource() const;

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Sets the script code.
	// Parameters:
	//     pBuilder - Pointer to a CXTPMarkupBuilder object that runs script engine.
	//     pContent - Pointer to a CXTPMarkupObject object that represents the code.
	//-----------------------------------------------------------------------
	virtual void SetContentObject(CXTPMarkupBuilder* pBuilder, CXTPMarkupObject* pContent);

private:
	virtual void OnLoaded(CXTPMarkupBuilder* pBuilder);

	HRESULT LoadFileContents(LPCTSTR lpszPath, CArray<WCHAR, WCHAR>* pData) const;
	HRESULT LoadResourceContents(LPCTSTR lpszModule, LPCTSTR lpName, LPCTSTR lpType,
								 CArray<WCHAR, WCHAR>* pData) const;
	HRESULT LoadCodeContent(const void* pFileData, UINT cbFileData,
							CArray<WCHAR, WCHAR>* pData) const;

private:
	static CXTPMarkupDependencyProperty* m_pPropertyLanguage; // Language property.
	static CXTPMarkupDependencyProperty* m_pPropertySource;	  // Source property.
};

#	include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPMARKUPXTPSCRIPT_H__)
