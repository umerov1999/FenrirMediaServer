// XTPSkinManagerApiHookConfig.h : API hook configuration file
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
#if !defined(__XTPSKINMANAGERAPIHOOKCONFIG_H__)
#	define __XTPSKINMANAGERAPIHOOKCONFIG_H__
//}}AFX_CODEJOCK_PRIVATE

#	if _MSC_VER > 1000
#		pragma once
#	endif // _MSC_VER > 1000

#	if !defined(XTP_SKINFRAMEWORK_USE_LEGACY_API_HOOK)
#		if defined(_WIN32_WINNT) && (0x400 <= _WIN32_WINNT)
#			define XTP_SKINFRAMEWORK_USE_DETOURS_API_HOOK
#		else
#			define XTP_SKINFRAMEWORK_USE_LEGACY_API_HOOK
#		endif
#	endif

#	if (defined(_XTP_ACTIVEX) && !defined(XTP_SKINFRAMEWORK_USE_LEGACY_API_HOOK))
#		define XTP_SKINFRAMEWORK_USE_LEGACY_API_HOOK
#	endif

#	if !defined(_M_IX86) && !defined(_M_X64)
#		error Unsupported target processor architecture. Only x86 and x64 are supported.
#	endif

#endif // !defined(__XTPSKINMANAGERAPIHOOKCONFIG_H__)
