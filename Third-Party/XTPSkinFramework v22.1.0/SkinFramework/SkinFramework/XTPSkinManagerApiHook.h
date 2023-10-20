// XTPSkinManagerApiHook.h: API hook configuration and inclusion directives
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
#if !defined(__XTPSKINMANAGERAPIHOOK_H__)
#	define __XTPSKINMANAGERAPIHOOK_H__

#	if _MSC_VER > 1000
#		pragma once
#	endif // _MSC_VER > 1000

#	include "SkinFramework/ApiHook/XTPSkinManagerApiHookConfig.h"

#	if !defined(_XTP_ACTIVEX) && defined(XTP_SKINFRAMEWORK_USE_DETOURS_API_HOOK)                  \
		&& defined(XTP_SKINFRAMEWORK_USE_LEGACY_API_HOOK)
#		error Only one implementation of SkinManager API Hook is allowed.
#	endif

#	include "SkinFramework/ApiHook/XTPSkinManagerApiHookBase.h"

#	ifdef XTP_SKINFRAMEWORK_USE_DETOURS_API_HOOK
#		include "SkinFramework/ApiHook/XTPSkinManagerDetoursApiHook.h"
#	endif
#	ifdef XTP_SKINFRAMEWORK_USE_LEGACY_API_HOOK
#		include "SkinFramework/ApiHook/XTPSkinManagerLegacyApiHook.h"
#	endif

#endif // !defined(__XTPSKINMANAGERAPIHOOK_H__)

//}}AFX_CODEJOCK_PRIVATE
