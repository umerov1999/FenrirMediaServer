// XTPDLLExports.h : common include file for all dll projects.
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

//{{AFX_CODEJOCK_PRIVATE
#if !defined(__XTPDLLEXPORTS_H__)
#	define __XTPDLLEXPORTS_H__

#	if _MSC_VER >= 1000
#		pragma once
#	endif // _MSC_VER >= 1000

#	define VC_EXTRALEAN // Exclude rarely-used stuff from Windows headers

#	pragma warning(disable : 4786) // Identifier was truncated to '255' characters

#	if _MSC_VER > 1200

#		if _MSC_VER > 1600
#			include <sdkddkver.h>
#		else

// Modify the following defines if you have to target a platform prior to the ones specified below.
// Refer to MSDN for the latest info on corresponding values for different platforms.
#			ifndef WINVER // Allow use of features specific to Windows 95 and Windows NT 4 or
						   // later.
#				if _MSC_VER >= 1800
#					define WINVER 0x0502
#				elif _MSC_VER > 1500
#					define WINVER 0x0500
#				else
#					define WINVER                                                                 \
						0x0400 // Change this to the appropriate value to target Windows 98 and
							   // Windows 2000 or later.
#				endif
#			endif // WINVER

#			ifndef _WIN32_WINNT // Allow use of features specific to Windows NT 4 or later.
#				if _MSC_VER >= 1800
#					define _WIN32_WINNT 0x0502
#				elif _MSC_VER > 1500
#					define _WIN32_WINNT 0x0500
#				else
#					define _WIN32_WINNT                                                           \
						0x0400 // Change this to the appropriate value to target Windows 98 and
							   // Windows 2000 or later.
#				endif
#			endif // _WIN32_WINNT

#			ifndef _WIN32_WINDOWS // Allow use of features specific to Windows 98 or later.
#				define _WIN32_WINDOWS                                                             \
					0x0410 // Change this to the appropriate value to target Windows Me or later.
#			endif

#			ifndef _WIN32_IE // Allow use of features specific to IE 4.0 or later.
#				if _MSC_VER >= 1800
#					define _WIN32_IE 0x0603
#				else
#					define _WIN32_IE                                                              \
						0x0500 // Change this to the appropriate value to target IE 5.0 or later.
#				endif
#			endif // _WIN32_IE

#		endif // !(_MSC_VER > 1600)
#	endif	 // _MSC_VER > 1200

#	include "Common/Base/Diagnostic/XTPDisableAdvancedWarnings.h"

#	include <afxwin.h>  // MFC core and standard components
#	include <afxext.h>  // MFC extensions
#	include <afxole.h>  // MFC OLE classes
#	include <afxdisp.h> // MFC Automation classes
#	include <afxcmn.h>  // MFC support for Windows Common Controls
#	if (_MSC_VER > 1200)
#		include <afxdhtml.h> // MFC HTML Dialogs
#	endif

//-----------------------------------------------------------------------------
// Supporting MFC include files
//-----------------------------------------------------------------------------

#	ifdef _MFC_OVERRIDES_NEW
#		define _INC_MALLOC
#	endif

#	include <afxpriv.h>  // MFC support for docking windows
#	include <afxtempl.h> // MFC template classes
#	include <afxcview.h> // MFC ListView / TreeView support
#	include <afxmt.h>	// MFC Multithreaded Extensions (Syncronization Objects)

#	include "Common/Base/Diagnostic/XTPEnableAdvancedWarnings.h"

#	ifndef AFX_STATIC
#		define AFX_STATIC extern
#		define AFX_STATIC_DATA extern __declspec(selectany)
#	endif

#	ifdef _LIB
#		define _XTP_EXTRN
#	else
#		define _XTP_EXTRN __declspec(dllexport)
#	endif

#	define _XTP_EXT_CLASS _XTP_EXTRN

#	include "Common/XTPVersion.h"
#	include "Common/XTPMacros.h"

#endif // #if !defined(__XTPDLLEXPORTS_H__)
//}}AFX_CODEJOCK_PRIVATE
