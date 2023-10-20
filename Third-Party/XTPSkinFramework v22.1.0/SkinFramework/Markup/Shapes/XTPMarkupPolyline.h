// XTPMarkupPolyline.h: interface for the CXTPMarkupPolyline class.
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

//{{AFX_CODEJOCK_PRIVATE
#if !defined(__XTPMARKUPPOLYLINE_H__)
#	define __XTPMARKUPPOLYLINE_H__
//}}AFX_CODEJOCK_PRIVATE

#	if _MSC_VER > 1000
#		pragma once
#	endif // _MSC_VER > 1000

#	include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

class CXTPMarkupPointCollection;
class CXTPMarkupDependencyProperty;
class CXTPMarkupDrawingContext;

class _XTP_EXT_CLASS CXTPMarkupPolyline : public CXTPMarkupShape
{
	DECLARE_MARKUPCLASS(CXTPMarkupPolyline)

protected:
	CXTPMarkupPolyline();
	virtual ~CXTPMarkupPolyline();

public:
	void SetPoints(CXTPMarkupPointCollection* pPoints);
	CXTPMarkupPointCollection* GetPoints() const;

protected:
	CSize MeasureOverride(CXTPMarkupDrawingContext* pDC, CSize szAvailableSize);
	void OnRender(CXTPMarkupDrawingContext* pDC);

	//{{AFX_CODEJOCK_PRIVATE
public:
	DECLARE_DISPATCH_MAP()
#	ifdef _XTP_ACTIVEX
	DECLARE_INTERFACE_MAP()
	DECLARE_OLETYPELIB_EX(CXTPMarkupPolyline);
#	endif

	//}}AFX_CODEJOCK_PRIVATE

public:
	static CXTPMarkupDependencyProperty* m_pPointsProperty;
};

#	include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPMARKUPPOLYLINE_H__)
