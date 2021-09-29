// XTPMarkupShape.h: interface for the CXTPMarkupShape class.
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
#if !defined(__XTPMARKUPSHAPE_H__)
#	define __XTPMARKUPSHAPE_H__
//}}AFX_CODEJOCK_PRIVATE

#	if _MSC_VER > 1000
#		pragma once
#	endif // _MSC_VER > 1000

#	include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

class CXTPMarkupBrush;
class CXTPMarkupStrokeStyle;
class CXTPMarkupDoubleCollection;
class CXTPMarkupBuilder;
class CXTPMarkupObject;
class CXTPMarkupDependencyProperty;

enum XTPMarkupStretch
{
	xtpMarkupStretchNone,
	xtpMarkupStretchFill,
	xtpMarkupStretchUniform,
	xtpMarkupStretchUniformToFill
};

class _XTP_EXT_CLASS CXTPMarkupShape : public CXTPMarkupFrameworkElement
{
	DECLARE_MARKUPCLASS(CXTPMarkupShape)

protected:
	CXTPMarkupShape();
	virtual ~CXTPMarkupShape();

public:
	void SetFill(CXTPMarkupBrush* brush);
	CXTPMarkupBrush* GetFill() const;

	void SetStroke(CXTPMarkupBrush* brush);
	CXTPMarkupBrush* GetStroke() const;

	void SetStrokeThickness(int nThickness);
	int GetStrokeThickness() const;

	XTPMarkupStretch GetStretch() const;

	CXTPMarkupDoubleCollection* GetStrokeDashArray() const;
	void SetStrokeDashArray(CXTPMarkupDoubleCollection* pStrokeDashArray);

	void GetStrokeStyle(CXTPMarkupStrokeStyle* pStrokeStyle) const;

	XTPMarkupLineCap GetStrokeStartLineCap() const;
	void SetStrokeStartLineCap(XTPMarkupLineCap nLineCap);

	XTPMarkupLineCap GetStrokeEndLineCap() const;
	void SetStrokeEndLineCap(XTPMarkupLineCap nLineCap);

	XTPMarkupLineJoin GetStrokeLineJoin() const;
	void SetStrokeLineJoin(XTPMarkupLineJoin nLineJoin);

	XTPMarkupSmoothingMode GetSmoothingMode() const;
	void SetSmoothingMode(XTPMarkupSmoothingMode smoothingMode);

public:
	static CXTPMarkupObject* AFX_CDECL
		ConvertLineCap(CXTPMarkupBuilder* pBuilder, CXTPMarkupObject* pObject,
					   CXTPMarkupDependencyProperty* pAssociatedProperty = NULL);
	static CXTPMarkupObject* AFX_CDECL
		ConvertLineJoin(CXTPMarkupBuilder* pBuilder, CXTPMarkupObject* pObject,
						CXTPMarkupDependencyProperty* pAssociatedProperty = NULL);
	static CXTPMarkupObject* AFX_CDECL
		ConvertSmoothing(CXTPMarkupBuilder* pBuilder, CXTPMarkupObject* pObject,
						 CXTPMarkupDependencyProperty* pAssociatedProperty = NULL);
	//{{AFX_CODEJOCK_PRIVATE
public:
	DECLARE_DISPATCH_MAP()
#	ifdef _XTP_ACTIVEX
	DECLARE_INTERFACE_MAP()
	DECLARE_OLETYPELIB_EX(CXTPMarkupShape);
#	endif

	afx_msg LPDISPATCH OleGetFill();
	afx_msg void OleSetFill(LPDISPATCH lpBackground);
	afx_msg LPDISPATCH OleGetStroke();
	afx_msg void OleSetStroke(LPDISPATCH lpBackground);
	afx_msg int OleGetStrokeThickness();
	afx_msg void OleSetStrokeThickness(int lpThickness);
	afx_msg int OleGetStrokeStartLineCap();
	afx_msg void OleSetStrokeStartLineCap(int nValue);
	afx_msg int OleGetStrokeEndLineCap();
	afx_msg void OleSetStrokeEndLineCap(int nValue);
	afx_msg int OleGetStrokeLineJoin();
	afx_msg void OleSetStrokeLineJoin(int nValue);
	afx_msg LPDISPATCH OleGetStrokeDashArray();
	afx_msg void OleSetStrokeDashArray(LPDISPATCH lpValue);

	//}}AFX_CODEJOCK_PRIVATE

public:
	static CXTPMarkupDependencyProperty* m_pStrokeProperty;
	static CXTPMarkupDependencyProperty* m_pFillProperty;
	static CXTPMarkupDependencyProperty* m_pStrokeThicknessProperty;
	static CXTPMarkupDependencyProperty* m_pStretchProperty;

	static CXTPMarkupDependencyProperty* m_pStrokeDashArrayProperty;
	static CXTPMarkupDependencyProperty* m_pStrokeStartLineCapProperty;
	static CXTPMarkupDependencyProperty* m_pStrokeEndLineCapProperty;
	static CXTPMarkupDependencyProperty* m_pStrokeLineJoinProperty;
	static CXTPMarkupDependencyProperty* m_pSmoothingMode;

	static CXTPMarkupDependencyProperty* m_pPropertyStrokeMiterLimit;
};

#	include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPMARKUPSHAPE_H__)
