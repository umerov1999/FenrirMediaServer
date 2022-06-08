// XTPPropExchangeEnumerator.cpp
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

#include "Common/XTPCasting.h"
#include "Common/XTPFramework.h"
#include "Common/PropExchange/XTPPropExchangeSection.h"
#include "Common/XTPPropExchange.h"
#include "Common/PropExchange/XTPPropExchangeEnumerator.h"

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

#ifdef _DEBUG
#	define new DEBUG_NEW
#	undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////
// CXTPPropExchangeEnumerator

CXTPPropExchangeEnumerator::CXTPPropExchangeEnumerator(CXTPPropExchange* pContainer,
													   LPCTSTR lpszSection)
{
	m_pContainer	 = pContainer;
	m_strSectionName = lpszSection;
	m_nCount		 = 0;
}

CXTPPropExchangeEnumerator::~CXTPPropExchangeEnumerator()
{
}

POSITION CXTPPropExchangeEnumerator::GetPosition(DWORD dwCount /*= 0*/,
												 BOOL bCompactMode /*= TRUE*/)
{
	m_nCount = dwCount;

	if (!m_pContainer)
		return NULL;

	if (!bCompactMode)
	{
		PX_DWord(m_pContainer, _T("Count"), m_nCount, 0);
	}
	else
	{
		if (m_pContainer->IsStoring())
			m_pContainer->WriteCount(m_nCount);
		else
			m_nCount = m_pContainer->ReadCount();
	}

	return m_nCount == 0 ? NULL : (POSITION)1;
}

CXTPPropExchange* CXTPPropExchangeEnumerator::GetNext(POSITION& pos)
{
	CString strSection;
	strSection.Format(_T("%s%i"), (LPCTSTR)m_strSectionName, (DWORD)(DWORD_PTR)pos - 1);

	CXTPPropExchange* pSection = m_pContainer->GetSection(strSection);

	pos++;
	if ((DWORD)(DWORD_PTR)pos > m_nCount)
		pos = NULL;

	return pSection;
}

int CXTPPropExchangeEnumerator::GetCount() const
{
	return XTPToIntChecked(m_nCount);
}
