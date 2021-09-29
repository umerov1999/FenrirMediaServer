// XTPVersion.h Xtreme Toolkit version definitions file
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

// Xtreme Skin Framework version number
#define _XTPLIB_VERSION_ MAKELONG(1, 20)

// Xtreme Skin Framework Major version number
#define _XTPLIB_VERSION_MAJOR 20

// Xtreme Skin Framework Minor version number
#define _XTPLIB_VERSION_MINOR 1

#define _XTPLIB_VERSION_REVISION 0

#define _XTPLIB_VERSION_BUILD 0

// Xtreme Skin Framework version prefix
#define _XTPLIB_VERSION_PREFIX 2010

//#define _XTP_DEMOMODE
#define _XTP_RETAIL

#if !defined(_XTP_DEMOMODE) && !defined(_XTP_RETAIL)
#	define _XTP_DEVMODE
#endif

#define _XTP_PACKAGE_NAME_T _T("Xtreme Skin Framework")
#define _XTP_PACKAGE_NAME_W L "Xtreme Skin Framework"
#define _XTP_PACKAGE_NAME_A "Xtreme Skin Framework"
#define _XTP_PACKAGE_NAME _XTP_PACKAGE_NAME_T

#define _XTP_COPYRIGHT_INFO _T("Copyright \xA9 1998-2021 Codejock Software")

//////////////////////////////////////////////////////////////////////
// Xtreme Toolkit Package
//////////////////////////////////////////////////////////////////////
#if !defined(_XTP_EXCLUDE_ALL) && !defined(_XTP_EXCLUDE_CALENDAR)
#endif

#if !defined(_XTP_EXCLUDE_ALL) && !defined(_XTP_EXCLUDE_CHART)
#endif

#if !defined(_XTP_EXCLUDE_ALL) && !defined(_XTP_EXCLUDE_COMMANDBARS)
#endif

#if !defined(_XTP_EXCLUDE_ALL) && !defined(_XTP_EXCLUDE_COMMON)
#	define _XTP_INCLUDE_COMMON
#endif

#if !defined(_XTP_EXCLUDE_ALL) && !defined(_XTP_EXCLUDE_CONTROLS)
#endif

#if !defined(_XTP_EXCLUDE_ALL) && !defined(_XTP_EXCLUDE_DOCKINGPANE)
#endif

#if !defined(_XTP_EXCLUDE_ALL) && !defined(_XTP_EXCLUDE_FLOWGRAPH)
#endif

#if !defined(_XTP_EXCLUDE_ALL) && !defined(_XTP_EXCLUDE_GRAPHICLIBRARY)
#	define _XTP_INCLUDE_GRAPHICLIBRARY
#endif

#if !defined(_XTP_EXCLUDE_ALL) && !defined(_XTP_EXCLUDE_PROPERTYGRID)
#endif

#if !defined(_XTP_EXCLUDE_ALL) && !defined(_XTP_EXCLUDE_REPORTCONTROL)
#endif

#if !defined(_XTP_EXCLUDE_ALL) && !defined(_XTP_EXCLUDE_RIBBON)
// Requires: TabManager, Controls, CommandBars
#endif

#if !defined(_XTP_EXCLUDE_ALL) && !defined(_XTP_EXCLUDE_SHORTCUTBAR)
#endif

#if !defined(_XTP_EXCLUDE_ALL) && !defined(_XTP_EXCLUDE_SKINFRAMEWORK)
#	define _XTP_INCLUDE_SKINFRAMEWORK
#endif

#if !defined(_XTP_EXCLUDE_ALL) && !defined(_XTP_EXCLUDE_SYNTAXEDIT)
#endif

#if !defined(_XTP_EXCLUDE_ALL) && !defined(_XTP_EXCLUDE_TABMANAGER)
#endif

#if !defined(_XTP_EXCLUDE_ALL) && !defined(_XTP_EXCLUDE_TASKPANEL)
#endif

#if !defined(_XTP_EXCLUDE_ALL) && !defined(_XTP_EXCLUDE_MARKUP)
#	define _XTP_INCLUDE_MARKUP
#endif
