// XTPMarkupSetterCollection.cpp: implementation of the CXTPMarkupSetterCollection class.
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

#include "Markup/XTPMarkupObject.h"
#include "Markup/XTPMarkupSetter.h"
#include "Markup/XTPMarkupSetterCollection.h"

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

#ifdef _DEBUG
#	define new DEBUG_NEW
#	undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////
// CXTPMarkupSetterCollection

IMPLEMENT_MARKUPCLASS(NULL, CXTPMarkupSetterCollection, CXTPMarkupCollection);

void CXTPMarkupSetterCollection::RegisterMarkupClass()
{
}

CXTPMarkupSetterCollection::CXTPMarkupSetterCollection()
{
	m_pElementType = MARKUP_TYPE(CXTPMarkupSetter);
}

CXTPMarkupSetter* CXTPMarkupSetterCollection::GetItem(int nIndex) const
{
	CXTPMarkupSetter* pItem = NULL;

	if (nIndex >= 0 && nIndex < m_arrItems.GetSize())
	{
		pItem = (CXTPMarkupSetter*)m_arrItems[nIndex];
	}

	return pItem;
}
