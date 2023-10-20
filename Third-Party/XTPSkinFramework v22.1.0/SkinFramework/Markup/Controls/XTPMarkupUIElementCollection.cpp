// XTPMarkupUIElementCollection.cpp: implementation of the CXTPMarkupUIElementCollection class.
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

#include "stdafx.h"

#include "Common/XTPFramework.h"

#include "Common/XTPCasting.h"
#include "Markup/XTPMarkupTools.h"
#include "Markup/XTPMarkupObject.h"
#include "Markup/XTPMarkupInputElement.h"
#include "Markup/XTPMarkupUIElement.h"
#include "Markup/Controls/XTPMarkupUIElementCollection.h"

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

#ifdef _DEBUG
#	undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#	define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////////
// CXTPMarkupUIElementCollection

CXTPMarkupUIElementCollection::CXTPMarkupUIElementCollection()
{
	m_pElementType = MARKUP_TYPE(CXTPMarkupUIElement);
}

CXTPMarkupUIElementCollection::~CXTPMarkupUIElementCollection()
{
}

void CXTPMarkupUIElementCollection::OnChanged()
{
	CXTPMarkupUIElement* pPanel = MARKUP_DYNAMICCAST(CXTPMarkupUIElement, GetLogicalParent());
	if (pPanel)
	{
		pPanel->InvalidateMeasure();
	}
}
