// XTPMarkupDefinitionBase.cpp: implementation of the CXTPMarkupDefinitionBase class.
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
#include "Common/Base/cxminmax.h"

#include "Common/XTPTypeId.h"
#include "Common/XTPCasting.h"
#include "Common/XTPFramework.h"
#include "Common/XTPSystemHelpers.h"

#include "Markup/XTPMarkupTools.h"
#include "Markup/XTPMarkupObject.h"
#include "Markup/XTPMarkupInputElement.h"
#include "Markup/XTPMarkupFrameworkContentElement.h"
#include "Markup/XTPMarkupUIElement.h"
#include "Markup/XTPMarkupFrameworkElement.h"
#include "Markup/Controls/XTPMarkupGridLength.h"
#include "Markup/Controls/XTPMarkupDefinitionBase.h"
#include "Markup/Controls/XTPMarkupColumnDefinition.h"
#include "Markup/Controls/XTPMarkupRowDefinition.h"
#include "Markup/Controls/XTPMarkupPanel.h"
#include "Markup/Controls/XTPMarkupGrid.h"
#include "Markup/XTPMarkupIIDs.h"

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

#ifdef _DEBUG
#	undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#	define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////////
// CXTPMarkupDefinitionBase

IMPLEMENT_MARKUPCLASS(L"DefinitionBase", CXTPMarkupDefinitionBase,
					  CXTPMarkupFrameworkContentElement);

void CXTPMarkupDefinitionBase::RegisterMarkupClass()
{
}

CXTPMarkupDefinitionBase::CXTPMarkupDefinitionBase(BOOL bIsColumnDefinition)
{
	m_bIsColumnDefinition = bIsColumnDefinition;
	m_nMinSize			  = 0;
	m_nSizeType			  = 0;
	m_nMeasureSize		  = 0;
}

void CXTPMarkupDefinitionBase::OnBeforeLayout()
{
	m_nSizeType	= 0;
	m_nMinSize	 = 0;
	m_nMeasureSize = 0;
}

void CXTPMarkupDefinitionBase::UpdateMinSize(int nMinSize)
{
	m_nMinSize = CXTP_max(m_nMinSize, nMinSize);
}

int CXTPMarkupDefinitionBase::GetUserMinSize() const
{
	CXTPMarkupInt* pMinValue = MARKUP_STATICCAST(
		CXTPMarkupInt,
		GetValue(m_bIsColumnDefinition ? CXTPMarkupColumnDefinition::m_pMinWidthProperty
									   : CXTPMarkupRowDefinition::m_pMinHeightProperty));
	return pMinValue != NULL ? (int)*pMinValue : 0;
}

double CXTPMarkupDefinitionBase::GetPreferredSize() const
{
	double nMinSize = m_nMinSize;
	if ((m_nSizeType != CXTPMarkupGrid::sizeTypeAuto) && (nMinSize < m_nMeasureSize))
	{
		nMinSize = m_nMeasureSize;
	}
	return nMinSize;
}

int CXTPMarkupDefinitionBase::GetUserMaxSize() const
{
	CXTPMarkupInt* pMaxValue = MARKUP_STATICCAST(
		CXTPMarkupInt,
		GetValue(m_bIsColumnDefinition ? CXTPMarkupColumnDefinition::m_pMaxWidthProperty
									   : CXTPMarkupRowDefinition::m_pMaxHeightProperty));
	return pMaxValue != NULL ? (int)*pMaxValue : INT_MAX;
}

CXTPMarkupGridLength* CXTPMarkupDefinitionBase::GetUserSize() const
{
	CXTPMarkupGridLength* pValue = MARKUP_STATICCAST(
		CXTPMarkupGridLength,
		GetValue(m_bIsColumnDefinition ? CXTPMarkupColumnDefinition::m_pWidthProperty
									   : CXTPMarkupRowDefinition::m_pHeightProperty));

	ASSERT(pValue);
	return pValue;
}

void CXTPMarkupDefinitionBase::OnDefinitionPropertyChanged(
	CXTPMarkupObject* d, CXTPMarkupPropertyChangedEventArgs* /*e*/)
{
	CXTPMarkupDefinitionBase* pDefinition = MARKUP_DYNAMICCAST(CXTPMarkupDefinitionBase, d);
	if (!pDefinition)
		return;

	CXTPMarkupGrid* pGrid =
		MARKUP_DYNAMICCAST(CXTPMarkupGrid, pDefinition->GetLogicalParent()
											   ? pDefinition->GetLogicalParent()->GetLogicalParent()
											   : NULL);
	if (!pGrid)
		return;

	pGrid->InvalidateMeasure();
}

//////////////////////////////////////////////////////////////////////////
// CXTPMarkupDefinitionBase

BEGIN_DISPATCH_MAP(CXTPMarkupDefinitionBase, CXTPMarkupFrameworkContentElement)
END_DISPATCH_MAP()

#ifdef _XTP_ACTIVEX

BEGIN_INTERFACE_MAP(CXTPMarkupDefinitionBase, CXTPMarkupFrameworkContentElement)
	INTERFACE_PART(CXTPMarkupDefinitionBase, XTPDIID_MarkupDefinitionBase, Dispatch)
END_INTERFACE_MAP()

IMPLEMENT_OLETYPELIB_EX(CXTPMarkupDefinitionBase, XTPDIID_MarkupDefinitionBase)

#endif
