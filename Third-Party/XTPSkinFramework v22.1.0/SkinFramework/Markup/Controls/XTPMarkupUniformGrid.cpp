// XTPMarkupUniformGrid.cpp: implementation of the CXTPMarkupUniformGrid class.
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

#include "Common/Base/Diagnostic/XTPDisableAdvancedWarnings.h"
#include <math.h>
#include "Common/Base/Diagnostic/XTPEnableAdvancedWarnings.h"

#include "Common/XTPTypeId.h"
#include "Common/XTPCasting.h"
#include "Common/XTPFramework.h"
#include "Common/XTPSystemHelpers.h"

#include "Markup/XTPMarkupTools.h"
#include "Markup/XTPMarkupObject.h"
#include "Markup/XTPMarkupInputElement.h"
#include "Markup/XTPMarkupUIElement.h"
#include "Markup/XTPMarkupFrameworkElement.h"
#include "Markup/Controls/XTPMarkupUIElementCollection.h"
#include "Markup/Controls/XTPMarkupPanel.h"
#include "Markup/Controls/XTPMarkupUniformGrid.h"
#include "Markup/XTPMarkupIIDs.h"

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

#ifdef _DEBUG
#	undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#	define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////////
// CXTPMarkupUniformGrid

CXTPMarkupDependencyProperty* CXTPMarkupUniformGrid::m_pFirstColumnProperty = NULL;
CXTPMarkupDependencyProperty* CXTPMarkupUniformGrid::m_pColumnsProperty		= NULL;
CXTPMarkupDependencyProperty* CXTPMarkupUniformGrid::m_pRowsProperty		= NULL;

IMPLEMENT_MARKUPCLASS(L"UniformGrid", CXTPMarkupUniformGrid, CXTPMarkupPanel)

void CXTPMarkupUniformGrid::RegisterMarkupClass()
{
	m_pFirstColumnProperty = CXTPMarkupDependencyProperty::Register(
		L"FirstColumn", MARKUP_TYPE(CXTPMarkupInt), MARKUP_TYPE(CXTPMarkupUniformGrid),
		new CXTPMarkupPropertyMetadata(NULL, CXTPMarkupPropertyMetadata::flagAffectsMeasure));

	m_pColumnsProperty = CXTPMarkupDependencyProperty::Register(
		L"Columns", MARKUP_TYPE(CXTPMarkupInt), MARKUP_TYPE(CXTPMarkupUniformGrid),
		new CXTPMarkupPropertyMetadata(NULL, CXTPMarkupPropertyMetadata::flagAffectsMeasure));

	m_pRowsProperty = CXTPMarkupDependencyProperty::Register(
		L"Rows", MARKUP_TYPE(CXTPMarkupInt), MARKUP_TYPE(CXTPMarkupUniformGrid),
		new CXTPMarkupPropertyMetadata(NULL, CXTPMarkupPropertyMetadata::flagAffectsMeasure));
}

CXTPMarkupUniformGrid::CXTPMarkupUniformGrid()
{
	m_nColumns = m_nRows = 1;
	m_nFirstColumn		 = 0;
}

CXTPMarkupUniformGrid::~CXTPMarkupUniformGrid()
{
}

void CXTPMarkupUniformGrid::UpdateComputedValues()
{
	m_nColumns	 = GetColumns();
	m_nRows		   = GetRows();
	m_nFirstColumn = GetFirstColumn();

	if (m_nFirstColumn >= m_nColumns)
	{
		m_nFirstColumn = 0;
	}

	if ((m_nRows == 0) || (m_nColumns == 0))
	{
		int num	= 0;
		int nCount = m_pChildren->GetCount();

		for (int i = 0; i < nCount; i++)
		{
			CXTPMarkupUIElement* pElement = m_pChildren->GetItem(i);
			if (pElement && pElement->GetVisibility() != xtpMarkupVisibilityCollapsed)
			{
				num++;
			}
		}

		if (num == 0)
		{
			num = 1;
		}

		if (m_nRows == 0)
		{
			if (m_nColumns > 0)
			{
				m_nRows = ((num + m_nFirstColumn) + (m_nColumns - 1)) / m_nColumns;
			}
			else
			{
				m_nRows = (int)sqrt((double)num);
				if ((m_nRows * m_nRows) < num)
				{
					m_nRows++;
				}
				m_nColumns = m_nRows;
			}
		}
		else if (m_nColumns == 0)
		{
			m_nColumns = (num + (m_nRows - 1)) / m_nRows;
		}
	}
}

CSize CXTPMarkupUniformGrid::MeasureOverride(CXTPMarkupDrawingContext* pDC, CSize constraint)
{
	UpdateComputedValues();

	CSize availableSize(constraint.cx / m_nColumns, constraint.cy / m_nRows);

	int nWidth = 0, nHeight = 0;

	int nCount = m_pChildren->GetCount();
	for (int i = 0; i < nCount; i++)
	{
		CXTPMarkupUIElement* pElement = m_pChildren->GetItem(i);
		if (pElement == NULL)
			continue;

		pElement->Measure(pDC, availableSize);

		CSize desiredSize = pElement->GetDesiredSize();

		if (nWidth < desiredSize.cx)
		{
			nWidth = desiredSize.cx;
		}
		if (nHeight < desiredSize.cy)
		{
			nHeight = desiredSize.cy;
		}
	}
	return CSize(nWidth * m_nColumns, nHeight * m_nRows);
}

CSize CXTPMarkupUniformGrid::ArrangeOverride(CSize arrangeSize)
{
	CRect rcFinalRect(0, 0, arrangeSize.cx / m_nColumns, arrangeSize.cy / m_nRows);

	int nWidth = rcFinalRect.Width();

	rcFinalRect.OffsetRect(nWidth * m_nFirstColumn, 0);

	int nCount = m_pChildren->GetCount();
	for (int i = 0; i < nCount; i++)
	{
		CXTPMarkupUIElement* pElement = m_pChildren->GetItem(i);
		if (pElement == NULL)
			continue;

		pElement->Arrange(rcFinalRect);

		if (pElement->GetVisibility() != xtpMarkupVisibilityCollapsed)
		{
			rcFinalRect.OffsetRect(nWidth, 0);

			if (rcFinalRect.right > arrangeSize.cx)
			{
				rcFinalRect = CRect(0, rcFinalRect.top + rcFinalRect.Height(), nWidth,
									rcFinalRect.bottom + rcFinalRect.Height());
			}
		}
	}
	return arrangeSize;
}

int CXTPMarkupUniformGrid::GetColumns() const
{
	CXTPMarkupInt* pColumns = MARKUP_STATICCAST(CXTPMarkupInt, GetValue(m_pColumnsProperty));
	return pColumns != NULL ? (int)*pColumns : 0;
}

int CXTPMarkupUniformGrid::GetRows() const
{
	CXTPMarkupInt* pRows = MARKUP_STATICCAST(CXTPMarkupInt, GetValue(m_pRowsProperty));
	return pRows != NULL ? (int)*pRows : 0;
}

int CXTPMarkupUniformGrid::GetFirstColumn() const
{
	CXTPMarkupInt* pFirstColumn = MARKUP_STATICCAST(CXTPMarkupInt,
													GetValue(m_pFirstColumnProperty));
	return pFirstColumn != NULL ? (int)*pFirstColumn : 0;
}

void CXTPMarkupUniformGrid::SetFirstColumn(int nFirstColumn)
{
	SetValue(m_pFirstColumnProperty, new CXTPMarkupInt(nFirstColumn));
}

void CXTPMarkupUniformGrid::SetColumns(int nColumns)
{
	SetValue(m_pColumnsProperty, new CXTPMarkupInt(nColumns));
}

void CXTPMarkupUniformGrid::SetRows(int nRows)
{
	SetValue(m_pRowsProperty, new CXTPMarkupInt(nRows));
}

//////////////////////////////////////////////////////////////////////////
// CXTPMarkupUniformGrid

#include "Common/Base/Diagnostic/XTPBeginAfxMap.h"
BEGIN_DISPATCH_MAP(CXTPMarkupUniformGrid, CXTPMarkupPanel)
	DISP_PROPERTY_EX_ID(CXTPMarkupUniformGrid, "Columns", 600, OleGetColumns, OleSetColumns, VT_I4)
	DISP_PROPERTY_EX_ID(CXTPMarkupUniformGrid, "Rows", 601, OleGetRows, OleSetRows, VT_I4)
	DISP_PROPERTY_EX_ID(CXTPMarkupUniformGrid, "FirstColumn", 602, OleGetFirstColumn,
						OleSetFirstColumn, VT_I4)
END_DISPATCH_MAP()
#include "Common/Base/Diagnostic/XTPEndAfxMap.h"

#ifdef _XTP_ACTIVEX

BEGIN_INTERFACE_MAP(CXTPMarkupUniformGrid, CXTPMarkupPanel)
	INTERFACE_PART(CXTPMarkupUniformGrid, XTPDIID_MarkupUniformGrid, Dispatch)
END_INTERFACE_MAP()

IMPLEMENT_OLETYPELIB_EX(CXTPMarkupUniformGrid, XTPDIID_MarkupUniformGrid)
#endif

long CXTPMarkupUniformGrid::OleGetColumns()
{
	return GetColumns();
}

void CXTPMarkupUniformGrid::OleSetColumns(long nValue)
{
	SetColumns(nValue);
}

long CXTPMarkupUniformGrid::OleGetRows()
{
	return GetRows();
}

void CXTPMarkupUniformGrid::OleSetRows(long nValue)
{
	SetRows(nValue);
}

long CXTPMarkupUniformGrid::OleGetFirstColumn()
{
	return GetFirstColumn();
}

void CXTPMarkupUniformGrid::OleSetFirstColumn(long nValue)
{
	SetFirstColumn(nValue);
}
