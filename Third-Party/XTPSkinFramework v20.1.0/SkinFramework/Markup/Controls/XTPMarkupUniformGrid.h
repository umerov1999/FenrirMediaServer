// XTPMarkupUniformGrid.h: interface for the CXTPMarkupUniformGrid class.
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

//{{AFX_CODEJOCK_PRIVATE
#if !defined(__XTPMARKUPUNIFORMGRID_H__)
#	define __XTPMARKUPUNIFORMGRID_H__
//}}AFX_CODEJOCK_PRIVATE

#	if _MSC_VER > 1000
#		pragma once
#	endif // _MSC_VER > 1000

#	include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

class CXTPMarkupDependencyProperty;
class CXTPMarkupDrawingContext;

//===========================================================================
// Summary:
//     CXTPMarkupUniformGrid is a CXTPMarkupPanel derived class.
//     It implements the UniformGrid XAML Tag.
//===========================================================================
class _XTP_EXT_CLASS CXTPMarkupUniformGrid : public CXTPMarkupPanel
{
	DECLARE_MARKUPCLASS(CXTPMarkupUniformGrid)
protected:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPMarkupUniformGrid object.
	//-----------------------------------------------------------------------
	CXTPMarkupUniformGrid();

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPMarkupUniformGrid object, handles cleanup and deallocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPMarkupUniformGrid();

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Sets the number of columns in the grid.
	// Parameters:
	//     nColumns - Number of columns to set.
	//-----------------------------------------------------------------------
	void SetColumns(int nColumns);

	//-----------------------------------------------------------------------
	// Summary:
	//     Gets the number of columns in the grid.
	// Returns:
	//     The number of columns in the grid.
	//-----------------------------------------------------------------------
	int GetColumns() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets the number of rows in the grid.
	// Parameters:
	//     nRows - Number of rows to set.
	//-----------------------------------------------------------------------
	void SetRows(int nRows);

	//-----------------------------------------------------------------------
	// Summary:
	//     Gets the number of rows in the grid.
	// Returns:
	//     The number of rows in the grid.
	//-----------------------------------------------------------------------
	int GetRows() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets the position for the first column of the first row of the grid.
	// Parameters:
	//     nFirstColumn - Position to be set.
	//-----------------------------------------------------------------------
	void SetFirstColumn(int nFirstColumn);

	//-----------------------------------------------------------------------
	// Summary:
	//     Gets the position of the first column of the first row of the grid.
	// Parameters:
	//     The position of the first column of the first row of the grid.
	//-----------------------------------------------------------------------
	int GetFirstColumn() const;

protected:
	//{{AFX_CODEJOCK_PRIVATE
	// Implementation
	virtual CSize MeasureOverride(CXTPMarkupDrawingContext* pDC, CSize szAvailableSize);
	virtual CSize ArrangeOverride(CSize szFinalSize);

private:
	void UpdateComputedValues();

private:
	int m_nColumns;
	int m_nRows;
	int m_nFirstColumn;

public:
	static CXTPMarkupDependencyProperty* m_pFirstColumnProperty;
	static CXTPMarkupDependencyProperty* m_pColumnsProperty;
	static CXTPMarkupDependencyProperty* m_pRowsProperty;
	//}}AFX_CODEJOCK_PRIVATE

	//{{AFX_CODEJOCK_PRIVATE
public:
	DECLARE_DISPATCH_MAP()
#	ifdef _XTP_ACTIVEX
	DECLARE_INTERFACE_MAP()
	DECLARE_OLETYPELIB_EX(CXTPMarkupUniformGrid);
#	endif

	afx_msg long OleGetColumns();
	afx_msg long OleGetRows();
	afx_msg long OleGetFirstColumn();
	afx_msg void OleSetColumns(long);
	afx_msg void OleSetRows(long);
	afx_msg void OleSetFirstColumn(long);
	//}}AFX_CODEJOCK_PRIVATE
};

#	include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPMARKUPUNIFORMGRID_H__)
