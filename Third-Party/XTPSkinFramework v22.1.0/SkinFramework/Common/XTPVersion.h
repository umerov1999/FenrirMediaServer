// XTPVersion.h Xtreme Toolkit version definitions file
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

#undef _XTPLIB_VERSION_
#undef _XTPLIB_VERSION_MAJOR
#undef _XTPLIB_VERSION_MINOR
#undef _XTPLIB_VERSION_REVISION
#undef _XTPLIB_VERSION_PREFIX
#undef _XTPLIB_VERSION_BUILD
#undef _XTP_PACKAGE_NAME_T
#undef _XTP_PACKAGE_NAME_W
#undef _XTP_PACKAGE_NAME_A
#undef _XTP_PACKAGE_NAME
#undef _XTP_COPYRIGHT_INFO

//////////////////////////////////////////////////////////////////////
// Xtreme Toolkit version definitions:
//////////////////////////////////////////////////////////////////////

// Xtreme Toolkit Pro version number
#define _XTPLIB_VERSION_ MAKELONG(1, 22)

// Xtreme Toolkit Pro Major version number
#define _XTPLIB_VERSION_MAJOR 22

// Xtreme Toolkit Pro Minor version number
#define _XTPLIB_VERSION_MINOR 1

#define _XTPLIB_VERSION_REVISION 0

#define _XTPLIB_VERSION_BUILD 0

// Xtreme Toolkit Pro version prefix
#define _XTPLIB_VERSION_PREFIX 2210

//#define _XTP_DEMOMODE
#define _XTP_RETAIL

#if !defined(_XTP_DEMOMODE) && !defined(_XTP_RETAIL)
#	define _XTP_DEVMODE
#endif

#define _XTP_PACKAGE_NAME_T _T("Xtreme Toolkit Pro")
#define _XTP_PACKAGE_NAME_W L "Xtreme Toolkit Pro"
#define _XTP_PACKAGE_NAME_A "Xtreme Toolkit Pro"
#define _XTP_PACKAGE_NAME _XTP_PACKAGE_NAME_T

#define _XTP_COPYRIGHT_INFO _T("Copyright \xA9 1998-2023 Codejock Software")

//////////////////////////////////////////////////////////////////////
// Xtreme Toolkit Package
//////////////////////////////////////////////////////////////////////

#if !defined(_XTP_EXCLUDE_ALL) && !defined(_XTP_EXCLUDE_COMMON)
#	define _XTP_INCLUDE_COMMON
#endif

#if !defined(_XTP_EXCLUDE_ALL) && !defined(_XTP_EXCLUDE_GRAPHICLIBRARY)
#	define _XTP_INCLUDE_GRAPHICLIBRARY
#endif

#if !defined(_XTP_EXCLUDE_ALL) && !defined(_XTP_EXCLUDE_SKINFRAMEWORK)
#	define _XTP_INCLUDE_SKINFRAMEWORK
#endif

#if !defined(_XTP_EXCLUDE_ALL) && !defined(_XTP_EXCLUDE_MARKUP)
#	define _XTP_INCLUDE_MARKUP
#endif

#define _XTP_USE_WINAPPEX