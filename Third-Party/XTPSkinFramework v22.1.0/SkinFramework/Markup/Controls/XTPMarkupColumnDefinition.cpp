// XTPMarkupColumnDefinition.cpp: implementation of the CXTPMarkupColumnDefinition class.
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
#include "Markup/Controls/XTPMarkupColumnDefinition.h"
#include "Markup/XTPMarkupIIDs.h"

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

#ifdef _DEBUG
#	undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#	define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////////
// CXTPMarkupColumnDefinition

CXTPMarkupDependencyProperty* CXTPMarkupColumnDefinition::m_pWidthProperty	= NULL;
CXTPMarkupDependencyProperty* CXTPMarkupColumnDefinition::m_pMinWidthProperty = NULL;
CXTPMarkupDependencyProperty* CXTPMarkupColumnDefinition::m_pMaxWidthProperty = NULL;

IMPLEMENT_MARKUPCLASS(L"ColumnDefinition", CXTPMarkupColumnDefinition, CXTPMarkupDefinitionBase);

void CXTPMarkupColumnDefinition::RegisterMarkupClass()
{
	m_pWidthProperty = CXTPMarkupDependencyProperty::Register(
		L"Width", MARKUP_TYPE(CXTPMarkupGridLength), MARKUP_TYPE(CXTPMarkupColumnDefinition),
		new CXTPMarkupPropertyMetadata(new CXTPMarkupGridLength(1,
																CXTPMarkupGridLength::unitTypeStar),
									   &CXTPMarkupDefinitionBase::OnDefinitionPropertyChanged));

	m_pMinWidthProperty = CXTPMarkupDependencyProperty::Register(
		L"MinWidth", MARKUP_TYPE(CXTPMarkupInt), MARKUP_TYPE(CXTPMarkupColumnDefinition),
		new CXTPMarkupPropertyMetadata(NULL, &CXTPMarkupDefinitionBase::OnDefinitionPropertyChanged,
									   CXTPMarkupPropertyMetadata::flagHorzDpiSensible));

	m_pMaxWidthProperty = CXTPMarkupDependencyProperty::Register(
		L"MaxWidth", MARKUP_TYPE(CXTPMarkupInt), MARKUP_TYPE(CXTPMarkupColumnDefinition),
		new CXTPMarkupPropertyMetadata(NULL, &CXTPMarkupDefinitionBase::OnDefinitionPropertyChanged,
									   CXTPMarkupPropertyMetadata::flagHorzDpiSensible));
}

CXTPMarkupColumnDefinition::CXTPMarkupColumnDefinition()
	: CXTPMarkupDefinitionBase(TRUE)
{
}

void CXTPMarkupColumnDefinition::SetMinWidth(int nWidth)
{
	SetValue(m_pMinWidthProperty, new CXTPMarkupInt(nWidth));
}

void CXTPMarkupColumnDefinition::SetMaxWidth(int nWidth)
{
	SetValue(m_pMaxWidthProperty, new CXTPMarkupInt(nWidth));
}

void CXTPMarkupColumnDefinition::SetWidth(int nWidth)
{
	SetValue(m_pWidthProperty, new CXTPMarkupGridLength(nWidth));
}

void CXTPMarkupColumnDefinition::SetWidth(CXTPMarkupGridLength* pLength)
{
	SetValue(m_pWidthProperty, pLength);
}

//////////////////////////////////////////////////////////////////////////
// CXTPMarkupColumnDefinition

#include "Common/Base/Diagnostic/XTPBeginAfxMap.h"
BEGIN_DISPATCH_MAP(CXTPMarkupColumnDefinition, CXTPMarkupDefinitionBase)
	DISP_PROPERTY_EX_ID(CXTPMarkupColumnDefinition, "Width", 200, OleGetWidth, OleSetWidth, VT_I4)
	DISP_PROPERTY_EX_ID(CXTPMarkupColumnDefinition, "MinWidth", 201, OleGetMinWidth, OleSetMinWidth,
						VT_I4)
	DISP_PROPERTY_EX_ID(CXTPMarkupColumnDefinition, "MaxWidth", 202, OleGetMaxWidth, OleSetMaxWidth,
						VT_I4)
	DISP_PROPERTY_EX_ID(CXTPMarkupColumnDefinition, "UnitType", 203, OleGetUnitType, OleSetUnitType,
						VT_I4)
END_DISPATCH_MAP()
#include "Common/Base/Diagnostic/XTPEndAfxMap.h"

#ifdef _XTP_ACTIVEX

BEGIN_INTERFACE_MAP(CXTPMarkupColumnDefinition, CXTPMarkupDefinitionBase)
	INTERFACE_PART(CXTPMarkupColumnDefinition, XTPDIID_MarkupColumnDefinition, Dispatch)
END_INTERFACE_MAP()

IMPLEMENT_OLETYPELIB_EX(CXTPMarkupColumnDefinition, XTPDIID_MarkupColumnDefinition)
#endif

void CXTPMarkupColumnDefinition::OleSetMinWidth(int nWidth)
{
	SetMinWidth(nWidth);
}

void CXTPMarkupColumnDefinition::OleSetMaxWidth(int nWidth)
{
	SetMaxWidth(nWidth);
}

void CXTPMarkupColumnDefinition::OleSetWidth(int nWidth)
{
	SetWidth(nWidth);
}

int CXTPMarkupColumnDefinition::OleGetMinWidth()
{
	return GetUserMinSize();
}

int CXTPMarkupColumnDefinition::OleGetMaxWidth()
{
	return GetUserMaxSize();
}

int CXTPMarkupColumnDefinition::OleGetWidth()
{
	CXTPMarkupGridLength* pValue = GetUserSize();
	return (int)pValue->GetValue();
}

int CXTPMarkupColumnDefinition::OleGetUnitType()
{
	CXTPMarkupGridLength* pValue = GetUserSize();
	return pValue->GetUnitType();
}

void CXTPMarkupColumnDefinition::OleSetUnitType(int nType)
{
	SetWidth(new CXTPMarkupGridLength(OleGetWidth(), (CXTPMarkupGridLength::GridUnitType)nType));
}
