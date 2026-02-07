// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
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

#if !defined(AFX_STDAFX_H__B6F4ABBF_7DA1_43C1_A82B_5EB4405A5135__INCLUDED_)
#	define AFX_STDAFX_H__B6F4ABBF_7DA1_43C1_A82B_5EB4405A5135__INCLUDED_

#	if _MSC_VER > 1000
#		pragma once
#	endif // _MSC_VER > 1000

// disable MBCS warning for Visual Studio 2013 and later
#	if _MSC_VER >= 1800 && !defined(NO_WARN_MBCS_MFC_DEPRECATION)
#		define NO_WARN_MBCS_MFC_DEPRECATION
#	endif

// By default SkinFramework must have Calendar MAPI support enabled
#	define _XTP_INCLUDE_CALENDAR_MAPI

#	include "Common/XTPDLLExports.h"

#endif // !defined(AFX_STDAFX_H__B6F4ABBF_7DA1_43C1_A82B_5EB4405A5135__INCLUDED_)
