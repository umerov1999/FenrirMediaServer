// XTPThemeDLL.h: interface for the CXTPThemeDLL class.
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

#if !defined(__XTPTHEMEDLL_H__)
//{{AFX_CODEJOCK_PRIVATE
#	define __XTPTHEMEDLL_H__
//}}AFX_CODEJOCK_PRIVATE

#	if _MSC_VER > 1000
#		pragma once
#	endif // _MSC_VER > 1000

#	include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

//===========================================================================
// Summary:  CXTPThemeDLL is a helper class used to load Codejock theme
//           resource DLL's.  Use XTPThemeDLL() to access the meembers of
//           this class.
// See Also: XTPThemeDLL()
//===========================================================================
class _XTP_EXT_CLASS CXTPThemeDLL
{
	friend class CXTPSingleton<CXTPThemeDLL>;

	//-----------------------------------------------------------------------
	// Summary: Default constructor handles initialization.
	//-----------------------------------------------------------------------
	CXTPThemeDLL();

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Destructor, handles cleanup and deallocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPThemeDLL();

	//-----------------------------------------------------------------------
	// Summary: This member function is called to load the style DLL's from
	//          the specified path.  If 'lpszDllPath' is NULL then the application
	//          folder + \Styles path is used as a default.
	// Input:   hAppInstance - Instance of application who's path will be used to
	//          locate the 'Styles' folder.  Can be NULL when using 'lpszDllPath'.
	//          lpszDllPath  - Path where Codejock theme DLL's are located.
	// Returns: TRUE if one or more of the Codejock theme DLL's were located,
	//          otherwise FALSE.
	//-----------------------------------------------------------------------
	BOOL Init(HINSTANCE hAppInstance = NULL, LPCTSTR lpszDllPath = NULL);

	//-----------------------------------------------------------------------
	// Summary:  Sets the handle to the resource DLL used for Codejock themes.
	// Input:    strINI - Name of the INI file to be loaded from the
	//                    resource DLL.
	// Returns:  TRUE if the handle was successfully set, otherwise FALSE.
	// See Also: See XTPResourceImage.h for a list of constants you can use
	//           for strINI.
	//-----------------------------------------------------------------------
	BOOL SetHandle(CString strINI);

	//-----------------------------------------------------------------------
	// Summary:  Resets all properties of ResourceImages
	//-----------------------------------------------------------------------
	void Reset();

private:
	//-----------------------------------------------------------------------
	// Summary:
	//     Internal function used to load Codejock theme .DLLs.
	// Parameters:
	//     hModule     - Reference to a CXTPModuleHandle object containing the
	//                   handle of the loaded theme .DLL.
	//     lpszDLLPath - Path where the theme .DLLs are located.
	//     lpszDllName - Name of the theme .DLL to load.
	// Returns:
	//     TRUE if the theme .DLL was successfully loaded, otherwise FALSE.
	//-----------------------------------------------------------------------
	BOOL LoadThemeDll(CXTPModuleHandle& hModule, LPCTSTR lpszDLLPath, LPCTSTR lpszDllName);

	CXTPModuleHandle m_hOffice2007;		  // Handle to the Office 2007 theme DLL.
	CXTPModuleHandle m_hOffice2010;		  // Handle to the Office 2010 theme DLL.
	CXTPModuleHandle m_hOffice2013;		  // Handle to the Office 2013 theme DLL.
	CXTPModuleHandle m_hOffice2016;		  // Handle to the Office 2016 theme DLL.
	CXTPModuleHandle m_hVisualStudio2012; // Handle to the Visual Studio 2012 theme DLL.
	CXTPModuleHandle m_hVisualStudio2015; // Handle to the Visual Studio 2015 theme DLL.
	CXTPModuleHandle m_hWindows7;		  // Handle to the Windows 7 theme DLL.
	CXTPModuleHandle m_hWindows10;		  // Handle to the Windows 10 theme DLL.

	friend _XTP_EXT_CLASS CXTPThemeDLL* AFX_CDECL XTPThemeDLL();
};

//===========================================================================
// Summary:  XTPThemeDLL returns pointer to common CXTPThemeDLL class
// Returns:  Returns pointer to common CXTPThemeDLL class
// See Also: CXTPThemeDLL
//===========================================================================
_XTP_EXT_CLASS CXTPThemeDLL* AFX_CDECL XTPThemeDLL();

#	include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPTHEMEDLL_H__)
