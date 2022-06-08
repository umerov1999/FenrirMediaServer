// XTPFramework.cpp: Framework extension and helper classes
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

#include "StdAfx.h"

#include "Common/XTPFramework.h"

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

#ifdef _DEBUG
#	define new DEBUG_NEW
#	undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CXTPCmdTarget, CCmdTarget);

// { 1965CF4C-7BE1-4E09-94B4-DCEF78B16D04 }
const IID IID_IXTPCmdTargetProvider = { 0x1965cf4c,
										0x7be1,
										0x4e09,
										{ 0x94, 0xb4, 0xdc, 0xef, 0x78, 0xb1, 0x6d, 0x4 } };

CCmdTarget* AFX_CDECL CXTPCmdTarget::FromIDispatchSafe(IDispatch* pDisp)
{
	if (NULL == pDisp)
	{
		return NULL;
	}

	// Try to obtain CCmdTarget pointer from IXTPCmdTargetProvider.
	IXTPCmdTargetProvider* pCmdTargetProvider = NULL;
	if (SUCCEEDED(pDisp->QueryInterface(&pCmdTargetProvider)) && NULL != pCmdTargetProvider)
	{
		CCmdTarget* pCmdTarget = pCmdTargetProvider->GetCmdTarget();
		pCmdTargetProvider->Release();
		return pCmdTarget;
	}

	// Try MFC way.
	IDispatch* pObjectRoot = NULL;
	if (SUCCEEDED(pDisp->QueryInterface(&pObjectRoot)) && NULL != pObjectRoot)
	{
		CCmdTarget* pCmdTarget = CCmdTarget::FromIDispatch(pObjectRoot);
		pObjectRoot->Release();
		return pCmdTarget;
	}

	return NULL;
}

CXTPCmdTarget::CXTPCmdTarget()
{
}

BEGIN_INTERFACE_MAP(CXTPCmdTarget, CCmdTarget)
	XTP_DECLARE_CMDTARGETPROVIDER_INTERFACE_PART(CXTPCmdTarget)
END_INTERFACE_MAP()

XTP_IMPLEMENT_CMDTARGETPROVIDER_INTERFACE(CXTPCmdTarget)
