// XTPMarkupRowDefinitionCollection.cpp : implementation file
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

#include "Common/XTPCasting.h"
#include "Markup/XTPMarkupTools.h"
#include "Markup/XTPMarkupObject.h"
#include "Markup/XTPMarkupInputElement.h"
#include "Markup/XTPMarkupFrameworkContentElement.h"
#include "Markup/XTPMarkupUIElement.h"
#include "Markup/XTPMarkupFrameworkElement.h"
#include "Markup/Controls/XTPMarkupPanel.h"
#include "Markup/Controls/XTPMarkupGrid.h"
#include "Markup/Controls/XTPMarkupDefinitionBase.h"
#include "Markup/Controls/XTPMarkupRowDefinition.h"
#include "Markup/Controls/XTPMarkupRowDefinitionCollection.h"

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

#ifdef _DEBUG
#	define new DEBUG_NEW
#	undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_MARKUPCLASS(NULL, CXTPMarkupRowDefinitionCollection, CXTPMarkupDefinitionCollection);

void CXTPMarkupRowDefinitionCollection::RegisterMarkupClass()
{
}

CXTPMarkupRowDefinitionCollection::CXTPMarkupRowDefinitionCollection()
{
	m_pElementType = MARKUP_TYPE(CXTPMarkupRowDefinition);
}
