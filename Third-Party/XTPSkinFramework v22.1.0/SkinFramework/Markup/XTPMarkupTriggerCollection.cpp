// XTPMarkupTriggerCollection.cpp: implementation of the CXTPMarkupTriggerCollection class.
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
#include "Common/XTPCasting.h"
#include "Common/XTPFramework.h"
#include "Common/XTPSystemHelpers.h"

#include "Markup/XTPMarkupObject.h"
#include "Markup/XTPMarkupTrigger.h"
#include "Markup/XTPMarkupTriggerCollection.h"

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

#ifdef _DEBUG
#	define new DEBUG_NEW
#	undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////
// CXTPMarkupTriggerCollection

IMPLEMENT_MARKUPCLASS(NULL, CXTPMarkupTriggerCollection, CXTPMarkupCollection)

void CXTPMarkupTriggerCollection::RegisterMarkupClass()
{
}

CXTPMarkupTriggerCollection::CXTPMarkupTriggerCollection()
{
	m_pElementType = MARKUP_TYPE(CXTPMarkupTrigger);
}

CXTPMarkupTrigger* CXTPMarkupTriggerCollection::GetItem(int nIndex) const
{
	return nIndex >= 0 && nIndex < GetCount() ? (CXTPMarkupTrigger*)m_arrItems[nIndex] : NULL;
}
