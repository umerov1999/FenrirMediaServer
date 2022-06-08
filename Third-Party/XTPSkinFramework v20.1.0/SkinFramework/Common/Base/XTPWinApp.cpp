// XTPWinApp.cpp : implementation file
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

#include "stdafx.h"

#if defined(_XTP_USE_WINAPPEX) && (_MFC_VER >= 0x0900)
#	include "Common/Base/Diagnostic/XTPDisableAdvancedWarnings.h"
#	include <afxwinappex.h>
#	include "Common/Base/Diagnostic/XTPEnableAdvancedWarnings.h"
#endif

#include "Common/XTPTypeId.h"
#include "Common/XTPSynchro.h"
#include "Common/XTPSystemHelpers.h"
#include "Common/XTPApplication.h"
#include "Common/Base/XTPWinApp.h"

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

#ifdef _DEBUG
#	define new DEBUG_NEW
#	undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CXTPWinApp

IMPLEMENT_DYNAMIC(CXTPWinApp, CWinAppBase);

CXTPWinApp::CXTPWinApp()
{
}

CXTPWinApp::~CXTPWinApp()
{
}

BOOL CXTPWinApp::InitInstance()
{
	BOOL bInit = CWinAppBase::InitInstance();
	if (!bInit)
		return FALSE;

	return XTPInitialize();
}

//-----------------------------------------------------------------------
//  Perform XTP application shut down by calling XTPShutdown() to release
//  all internal XTP pointers and resources.
//-----------------------------------------------------------------------

int CXTPWinApp::ExitInstance()
{
	XTPShutdown();
	return CWinAppBase::ExitInstance();
}

BEGIN_MESSAGE_MAP(CXTPWinApp, CWinAppBase)
	//{{AFX_MSG_MAP(CXTPWinApp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CXTPWinApp message handlers
