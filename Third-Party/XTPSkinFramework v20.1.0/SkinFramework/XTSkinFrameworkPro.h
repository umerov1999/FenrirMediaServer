// XTSkinFramework.h
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

#if !defined(XT_SKINFRAMEWORK_PRO_H__75B714AE_60B3_4FFE_A6F2_1880D09F1561__INCLUDED_)
#	define XT_SKINFRAMEWORK_PRO_H__75B714AE_60B3_4FFE_A6F2_1880D09F1561__INCLUDED_

#	if _MSC_VER > 1000
#		pragma once
#	endif // _MSC_VER > 1000

#	pragma warning(                                                                               \
		disable : 4786) // Identifier was truncated to '255' characters in the debug information

#	if (_MSC_VER > 1100)
#		pragma warning(push)
#		pragma pack(push, 8)
#	endif

#	include "Common/Base/Diagnostic/XTPDisableAdvancedWarnings.h"

#	ifndef _AFX_NO_OLE_SUPPORT
#		include <afxole.h>
#	endif // _AFX_NO_OLE_SUPPORT

#	ifdef _MFC_OVERRIDES_NEW
#		define _INC_MALLOC
#	endif

#	include <afxpriv.h>  // MFC support for docking windows
#	include <afxtempl.h> // MFC template classes
#	include <afxcview.h> // MFC ListView / TreeView support
#	include <afxmt.h>	  // MFC Multithreaded Extensions (Syncronization Objects)
#	if (_MSC_VER > 1200)
#		include <afxdhtml.h> // MFC HTML Dialogs
#	endif

#	include "Common/Base/Diagnostic/XTPEnableAdvancedWarnings.h"

#	ifndef AFX_STATIC
#		define AFX_STATIC static
#	endif

//------------------------------------------------------------------------
// NOTE: If using the Xtreme Skin Framework as a static library linked to an
// application that is dynamically linked with MFC, you will need to do the
// following:
//
// Open the XTPToolkit_Lib project workspace and select one of the
// Win32 Dynamic build settings and build the library. Add the following
// lines of code to your stdafx.h file:
//
// #define _XTP_STATICLINK
// #include "XTSkinFramework.h"
//
// Add the following line of code to your *.rc2 file after the comment:
// "Add manually edited resources here...":
//
// #include "XTSkinFramework.rc"
//------------------------------------------------------------------------

#	undef _XTP_EXTRN

#	if !defined(_AFXDLL) || defined(_XTP_STATICLINK)
#		define _XTP_EXTRN
#	else
#		define _XTP_EXTRN __declspec(dllimport)
#	endif // !defined(_AFXDLL) || defined(_XTP_STATICLINK)

#	undef _XTP_EXT_CLASS
#	define _XTP_EXT_CLASS _XTP_EXTRN

#	include "Common/XTPVersion.h"

///////////////////////////////////////////////////////////////////////////////////////////////////

#	pragma message(                                                                               \
		"--------------------------------------------------------------------------------")
#	pragma message("Including " _XTP_PACKAGE_NAME_A)

///////////////////////////////////////////////////////////////////////////////////////////////////

#	if defined(_XTP_INCLUDE_COMMON)
#		include "Common/Includes.h"
#	endif

#	if defined(_XTP_INCLUDE_MARKUP)
#		ifdef _XTP_EXCLUDE_XML
#error Markup requires XML support. Undefine _XTP_EXCLUDE_XML if Markup is planned to be used or define _XTP_INCLUDE_MARKUP otheriwse.
#		endif
#		pragma message(                                                                           \
			"Including Markup         (Use _XTP_EXCLUDE_MARKUP         to exclude from build)")
#		include "Markup/Includes.h"
#	endif

///////////////////////////////////////////////////////////////////////////////////////////////////

#	if defined(_XTP_INCLUDE_GRAPHICLIBRARY)
#		pragma message(                                                                           \
			"Including GraphicLibrary (Use _XTP_EXCLUDE_GRAPHICLIBRARY to exclude from build)")
#		include "GraphicLibrary/Includes.h"
#	endif

#	if defined(_XTP_INCLUDE_SKINFRAMEWORK)
#		pragma message(                                                                           \
			"Including SkinFramework  (Use _XTP_EXCLUDE_SKINFRAMEWORK  to exclude from build)")
#		include "SkinFramework/Includes.h"
#	endif

#	pragma message(                                                                               \
		"--------------------------------------------------------------------------------")

///////////////////////////////////////////////////////////////////////////////////////////////////

#	undef _XTPLIB_FILE_VERSION
#	undef _XTPLIB_PROJECT_CONFIGURATION
#	undef _XTPLIB_PROJECT_EVAL
#	undef _XTPLIB_LINK_TYPE
#	undef _XTPLIB_LINK_IS_DLL

#	if (_MSC_VER > 1100)
#		pragma pack(pop)
#		pragma warning(pop)
#	endif

/////////////////////////////////////////////////////////////////////////////

#endif // !defined(XT_SKINFRAMEWORK_PRO_H__75B714AE_60B3_4FFE_A6F2_1880D09F1561__INCLUDED_)
