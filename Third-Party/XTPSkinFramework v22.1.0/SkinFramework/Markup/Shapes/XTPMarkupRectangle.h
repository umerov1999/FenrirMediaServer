// XTPMarkupRectangle.h: interface for the CXTPMarkupRectangle class.
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
#if !defined(__XTPMARKUPRECTANGLE_H__)
#	define __XTPMARKUPRECTANGLE_H__
//}}AFX_CODEJOCK_PRIVATE

#	if _MSC_VER > 1000
#		pragma once
#	endif // _MSC_VER > 1000

#	include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

class CXTPMarkupDependencyProperty;
class CXTPMarkupDrawingContext;

class _XTP_EXT_CLASS CXTPMarkupRectangle : public CXTPMarkupShape
{
	DECLARE_MARKUPCLASS(CXTPMarkupRectangle)

protected:
	CXTPMarkupRectangle();
	virtual ~CXTPMarkupRectangle();

public:
	int GetRadiusX() const;
	int GetRadiusY() const;

	void SetRadiusX(int nRarius);
	void SetRadiusY(int nRarius);

protected:
	CSize MeasureOverride(CXTPMarkupDrawingContext* pDC, CSize szAvailableSize);
	void OnRender(CXTPMarkupDrawingContext* pDC);

public:
	static CXTPMarkupDependencyProperty* m_pRadiusXProperty;
	static CXTPMarkupDependencyProperty* m_pRadiusYProperty;

	//{{AFX_CODEJOCK_PRIVATE
public:
	DECLARE_DISPATCH_MAP()
#	ifdef _XTP_ACTIVEX
	DECLARE_INTERFACE_MAP()
	DECLARE_OLETYPELIB_EX(CXTPMarkupRectangle);
#	endif

	afx_msg int OleGetRadiusX();
	afx_msg void OleSetRadiusX(int nValue);
	afx_msg int OleGetRadiusY();
	afx_msg void OleSetRadiusY(int nValue);

	//}}AFX_CODEJOCK_PRIVATE
};

AFX_INLINE void CXTPMarkupRectangle::SetRadiusX(int nRarius)
{
	SetValue(m_pRadiusXProperty, new CXTPMarkupInt(nRarius));
}

AFX_INLINE void CXTPMarkupRectangle::SetRadiusY(int nRarius)
{
	SetValue(m_pRadiusYProperty, new CXTPMarkupInt(nRarius));
}

#	include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPMARKUPRECTANGLE_H__)
