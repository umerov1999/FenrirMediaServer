// XTPPropExchangeEnumeratorPtr.cpp
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
#include "Common/PropExchange/XTPPropExchangeEnumerator.h"
#include "Common/PropExchange/XTPPropExchangeEnumeratorPtr.h"

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

#ifdef _DEBUG
#	define new DEBUG_NEW
#	undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CXTPPropExchangeEnumeratorPtr::CXTPPropExchangeEnumeratorPtr(CXTPPropExchangeEnumerator* pPX)
	: m_pPX(pPX)
{
}

CXTPPropExchangeEnumeratorPtr::~CXTPPropExchangeEnumeratorPtr()
{
	delete m_pPX;
}

CXTPPropExchangeEnumerator& CXTPPropExchangeEnumeratorPtr::operator*() const
{
	return *m_pPX;
}

CXTPPropExchangeEnumerator* CXTPPropExchangeEnumeratorPtr::operator->() const
{
	return m_pPX;
}

CXTPPropExchangeEnumerator* CXTPPropExchangeEnumeratorPtr::operator&() const
{
	return m_pPX;
}
