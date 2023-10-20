// XTPMarkupRowDefinition.cpp: implementation of the CXTPMarkupRowDefinition class.
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
#include "Markup/XTPMarkupInputElement.h"
#include "Markup/XTPMarkupFrameworkContentElement.h"
#include "Markup/Controls/XTPMarkupGridLength.h"
#include "Markup/Controls/XTPMarkupDefinitionBase.h"
#include "Markup/Controls/XTPMarkupRowDefinition.h"
#include "Markup/XTPMarkupIIDs.h"

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

#ifdef _DEBUG
#	undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#	define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////////
// CXTPMarkupRowDefinition

CXTPMarkupDependencyProperty* CXTPMarkupRowDefinition::m_pHeightProperty	= NULL;
CXTPMarkupDependencyProperty* CXTPMarkupRowDefinition::m_pMinHeightProperty = NULL;
CXTPMarkupDependencyProperty* CXTPMarkupRowDefinition::m_pMaxHeightProperty = NULL;

IMPLEMENT_MARKUPCLASS(L"RowDefinition", CXTPMarkupRowDefinition, CXTPMarkupDefinitionBase);

void CXTPMarkupRowDefinition::RegisterMarkupClass()
{
	m_pHeightProperty = CXTPMarkupDependencyProperty::Register(
		L"Height", MARKUP_TYPE(CXTPMarkupGridLength), MARKUP_TYPE(CXTPMarkupRowDefinition),
		new CXTPMarkupPropertyMetadata(new CXTPMarkupGridLength(1,
																CXTPMarkupGridLength::unitTypeStar),
									   &CXTPMarkupDefinitionBase::OnDefinitionPropertyChanged));

	m_pMinHeightProperty = CXTPMarkupDependencyProperty::Register(
		L"MinHeight", MARKUP_TYPE(CXTPMarkupInt), MARKUP_TYPE(CXTPMarkupRowDefinition),
		new CXTPMarkupPropertyMetadata(NULL, &CXTPMarkupDefinitionBase::OnDefinitionPropertyChanged,
									   CXTPMarkupPropertyMetadata::flagVertDpiSensible));

	m_pMaxHeightProperty = CXTPMarkupDependencyProperty::Register(
		L"MaxHeight", MARKUP_TYPE(CXTPMarkupInt), MARKUP_TYPE(CXTPMarkupRowDefinition),
		new CXTPMarkupPropertyMetadata(NULL, &CXTPMarkupDefinitionBase::OnDefinitionPropertyChanged,
									   CXTPMarkupPropertyMetadata::flagVertDpiSensible));
}

CXTPMarkupRowDefinition::CXTPMarkupRowDefinition()
	: CXTPMarkupDefinitionBase(FALSE)
{
}

void CXTPMarkupRowDefinition::SetMinHeight(int nHeight)
{
	SetValue(m_pMinHeightProperty, new CXTPMarkupInt(nHeight));
}

void CXTPMarkupRowDefinition::SetMaxHeight(int nHeight)
{
	SetValue(m_pMaxHeightProperty, new CXTPMarkupInt(nHeight));
}

void CXTPMarkupRowDefinition::SetHeight(int nHeight)
{
	SetValue(m_pHeightProperty, new CXTPMarkupGridLength(nHeight));
}

void CXTPMarkupRowDefinition::SetHeight(CXTPMarkupGridLength* pLength)
{
	SetValue(m_pHeightProperty, pLength);
}

//////////////////////////////////////////////////////////////////////////
// CXTPMarkupRowDefinition

#include "Common/Base/Diagnostic/XTPBeginAfxMap.h"
BEGIN_DISPATCH_MAP(CXTPMarkupRowDefinition, CXTPMarkupDefinitionBase)
	DISP_PROPERTY_EX_ID(CXTPMarkupRowDefinition, "Height", 200, OleGetHeight, OleSetHeight, VT_I4)
	DISP_PROPERTY_EX_ID(CXTPMarkupRowDefinition, "MinHeight", 201, OleGetMinHeight, OleSetMinHeight,
						VT_I4)
	DISP_PROPERTY_EX_ID(CXTPMarkupRowDefinition, "MaxHeight", 202, OleGetMaxHeight, OleSetMaxHeight,
						VT_I4)
	DISP_PROPERTY_EX_ID(CXTPMarkupRowDefinition, "UnitType", 203, OleGetUnitType, OleSetUnitType,
						VT_I4)
END_DISPATCH_MAP()
#include "Common/Base/Diagnostic/XTPEndAfxMap.h"

#ifdef _XTP_ACTIVEX

BEGIN_INTERFACE_MAP(CXTPMarkupRowDefinition, CXTPMarkupDefinitionBase)
	INTERFACE_PART(CXTPMarkupRowDefinition, XTPDIID_MarkupRowDefinition, Dispatch)
END_INTERFACE_MAP()

IMPLEMENT_OLETYPELIB_EX(CXTPMarkupRowDefinition, XTPDIID_MarkupRowDefinition)
#endif

void CXTPMarkupRowDefinition::OleSetMinHeight(int nHeight)
{
	SetMinHeight(nHeight);
}

void CXTPMarkupRowDefinition::OleSetMaxHeight(int nHeight)
{
	SetMinHeight(nHeight);
}

void CXTPMarkupRowDefinition::OleSetHeight(int nHeight)
{
	SetHeight(nHeight);
}

int CXTPMarkupRowDefinition::OleGetMinHeight()
{
	return GetUserMinSize();
}

int CXTPMarkupRowDefinition::OleGetMaxHeight()
{
	return GetUserMaxSize();
}

int CXTPMarkupRowDefinition::OleGetHeight()
{
	CXTPMarkupGridLength* pValue = GetUserSize();
	return (int)pValue->GetValue();
}

int CXTPMarkupRowDefinition::OleGetUnitType()
{
	CXTPMarkupGridLength* pValue = GetUserSize();
	return pValue->GetUnitType();
}
void CXTPMarkupRowDefinition::OleSetUnitType(int nType)
{
	SetHeight(new CXTPMarkupGridLength(OleGetHeight(), (CXTPMarkupGridLength::GridUnitType)nType));
}
