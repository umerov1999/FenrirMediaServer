// XTPWinApp.h : header file
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
#if !defined(__XTPWINAPP_H__)
#	define __XTPWINAPP_H__
//}}AFX_CODEJOCK_PRIVATE

#	if _MSC_VER > 1000
#		pragma once
#	endif // _MSC_VER > 1000

#	include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

#	ifndef _XTP_ACTIVEX
#		if defined(_XTP_USE_WINAPPEX) && (_MFC_VER >= 0x0900)
#			define CWinAppBase CWinAppEx
#		else
#			define CWinAppBase CWinApp
#		endif
#	else
#		define CWinAppBase COleControlModule
#	endif

//===========================================================================
// Summary:
//     All XTP projects should derive from this CXTPWinApp instead of
//     CWinApp or CWinAppEx. This class handles cleanup and releases all internal XTP
//     pointers and resources. In order to use CXTPWinApp derived from
//     CWinAppEx in Visual Studio 2008 with MFC Feature Pack installed or in
//     higher Visual Studio versions ToolkitPro must be re-compiled with
//     _XTP_USE_WINAPPEX macro defined and the referencing projects must
//     have this macro defined.
//===========================================================================
class _XTP_EXT_CLASS CXTPWinApp : public CWinAppBase
{
	DECLARE_DYNAMIC(CXTPWinApp);

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Default constructor.
	//-----------------------------------------------------------------------
	CXTPWinApp();

	//-----------------------------------------------------------------------
	// Summary:
	//     Default destructor, handles cleanup / deallocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPWinApp();

	//{{AFX_CODEJOCK_PRIVATE
	//{{AFX_VIRTUAL(CXTPWinApp)
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL
	//}}AFX_CODEJOCK_PRIVATE

protected:
	//{{AFX_CODEJOCK_PRIVATE
	//{{AFX_MSG(CXTPWinApp)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	//}}AFX_CODEJOCK_PRIVATE
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#	include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPWINAPP_H__)
