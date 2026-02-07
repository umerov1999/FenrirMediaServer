// XTPSystemMetrics.cpp
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

#include "StdAfx.h"

#include "Common/XTPTypeId.h"
#include "Common/XTPSystemHelpers.h"
#include "Common/XTPSynchro.h"
#include "Common/XTPApplication.h"
#include "Common/XTPSingleton.h"
#include "Common/XTPSystemMetrics.h"

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

#ifdef _DEBUG
#	define new DEBUG_NEW
#	undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////////////
// CXTPSystemMetrics

CXTPSystemMetrics* AFX_CDECL XTPSystemMetrics()
{
	return &CXTPSingleton<CXTPSystemMetrics>::Instance();
}

CXTPSystemMetrics::CXTPSystemMetrics()
	: m_nBorderY(0)
	, m_nCaptionY(0)
	, m_nSizeFrameY(0)
{
	RefreshMetrics();
}

void CXTPSystemMetrics::RefreshMetrics()
{
	CXTPSystemMetrics systemMetricsOld(*this);

	m_nBorderY	= ::GetSystemMetrics(SM_CYBORDER);
	m_nCaptionY   = ::GetSystemMetrics(SM_CYCAPTION);
	m_nSizeFrameY = ::GetSystemMetrics(SM_CYSIZEFRAME);

	m_nSmallIconX = ::GetSystemMetrics(SM_CXSMICON);
	m_nSmallIconY = ::GetSystemMetrics(SM_CYSMICON);

	m_nIconX = ::GetSystemMetrics(SM_CXICON);
	m_nIconY = ::GetSystemMetrics(SM_CYICON);
}

///////////////////////////////////////////////////////////////////////////////
// CXTPDeviceCaps

CXTPDeviceCaps* AFX_CDECL XTPDeviceCaps()
{
	return &CXTPSingleton<CXTPDeviceCaps>::Instance();
}

CXTPDeviceCaps::CXTPDeviceCaps()
	: m_nLogPixelsX(0)
	, m_nLogPixelsY(0)
{
	RefreshMetrics();
}

void CXTPDeviceCaps::RefreshMetrics()
{
	HDC hDC = ::GetDC(NULL);

	// Log pixels
	m_nLogPixelsX = ::GetDeviceCaps(hDC, LOGPIXELSX);
	m_nLogPixelsY = ::GetDeviceCaps(hDC, LOGPIXELSY);

	::ReleaseDC(NULL, hDC);
}
