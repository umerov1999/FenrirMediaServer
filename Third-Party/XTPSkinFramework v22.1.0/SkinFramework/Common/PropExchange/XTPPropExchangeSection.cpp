// XTPPropExchangeSection.cpp
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

#include "Common/XTPFramework.h"
#include "Common/PropExchange/XTPPropExchangeSection.h"
#include "Common/XTPPropExchange.h"
#include "Common/PropExchange/XTPPropExchangeRegistry.h"
#include "Common/PropExchange/XTPPropExchangeIniFile.h"

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

#ifdef _DEBUG
#	define new DEBUG_NEW
#	undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CXTPPropExchangeSection::CXTPPropExchangeSection(CXTPPropExchange* pPX)
	: m_pPX(pPX)
{
}

CXTPPropExchangeSection::CXTPPropExchangeSection(BOOL bLoading, LPCTSTR lpszSection)
{
	if (AfxGetApp()->m_pszRegistryKey)
	{
		m_pPX = new CXTPPropExchangeRegistry(bLoading, 0, lpszSection);
	}
	else
	{
		CXTPPropExchangeIniFile* pPX = new CXTPPropExchangeIniFile(bLoading, 0, lpszSection);
		pPX->AssignFile(AfxGetApp()->m_pszProfileName);
		m_pPX = pPX;
	}
}

CXTPPropExchangeSection::~CXTPPropExchangeSection()
{
	if (m_pPX)
	{
		m_pPX->InternalRelease();
	}
}

CXTPPropExchange& CXTPPropExchangeSection::operator*() const
{
	return *m_pPX;
}

CXTPPropExchange* CXTPPropExchangeSection::operator->() const
{
	return m_pPX;
}

CXTPPropExchange* CXTPPropExchangeSection::operator&() const
{
	return m_pPX;
}
