// XTPDLLExports.h : common include file for all dll projects.
//
// This file is a part of the XTREME TOOLKIT PRO MFC class library.
// (c)1998-2013 Codejock Software, All Rights Reserved.
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
#define __XTPDLLEXPORTS_H__

#pragma once
#define VC_EXTRALEAN        // Exclude rarely-used stuff from Windows headers

#pragma warning(disable : 4786) // Identifier was truncated to '255' characters

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxole.h>         // MFC OLE classes
#include <afxdisp.h>        // MFC Automation classes
#include <afxcmn.h>         // MFC support for Windows Common Controls


//-----------------------------------------------------------------------------
// Supporting MFC include files
//-----------------------------------------------------------------------------

#ifdef _MFC_OVERRIDES_NEW
#define _INC_MALLOC
#endif

#include <afxpriv.h>            // MFC support for docking windows
#include <afxtempl.h>           // MFC template classes
#include <afxcview.h>           // MFC ListView / TreeView support
#include <afxmt.h>              // MFC Multithreaded Extensions (Syncronization Objects)

#ifndef AFX_STATIC
#define AFX_STATIC        extern
#define AFX_STATIC_DATA   extern __declspec(selectany)
#endif

#ifdef _LIB
	#define _XTP_EXTRN
#else
	#define _XTP_EXTRN    __declspec(dllexport)
#endif

#define _XTP_EXT_CLASS _XTP_EXTRN

#include "XTPMacros.h"
#include "XTPVersion.h"

//}}AFX_CODEJOCK_PRIVATE

//////////////////////////////////////////////////////////////////////

#endif // #if !defined(__XTPDLLEXPORTS_H__)
