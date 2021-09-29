// XTPMarkupLine.h: interface for the CXTPMarkupLine class.
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
#if !defined(__XTPMARKUPLINE_H__)
#	define __XTPMARKUPLINE_H__
//}}AFX_CODEJOCK_PRIVATE

#	if _MSC_VER > 1000
#		pragma once
#	endif // _MSC_VER > 1000

#	include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

class CXTPMarkupDrawingContext;
class CXTPMarkupInputElement;

class _XTP_EXT_CLASS CXTPMarkupLine : public CXTPMarkupShape
{
	DECLARE_MARKUPCLASS(CXTPMarkupLine)

protected:
	CXTPMarkupLine();
	~CXTPMarkupLine();

public:
	int GetX1() const;
	int GetX2() const;
	int GetY1() const;
	int GetY2() const;

	void SetX1(int nValue);
	void SetX2(int nValue);
	void SetY1(int nValue);
	void SetY2(int nValue);

protected:
	virtual CSize MeasureOverride(CXTPMarkupDrawingContext* pDC, CSize szAvailableSize);
	virtual void OnRender(CXTPMarkupDrawingContext* pDC);
	virtual CXTPMarkupInputElement* InputHitTestOverride(CPoint point) const;

private:
	void GetPoints(int& x1, int& y1, int& x2, int& y2) const;

	//{{AFX_CODEJOCK_PRIVATE
public:
	DECLARE_DISPATCH_MAP()
#	ifdef _XTP_ACTIVEX
	DECLARE_INTERFACE_MAP()
	DECLARE_OLETYPELIB_EX(CXTPMarkupLine);
#	endif

	afx_msg int OleGetX1();
	afx_msg void OleSetX1(int nValue);
	afx_msg int OleGetX2();
	afx_msg void OleSetX2(int nValue);
	afx_msg int OleGetY1();
	afx_msg void OleSetY1(int nValue);
	afx_msg int OleGetY2();
	afx_msg void OleSetY2(int nValue);
	//}}AFX_CODEJOCK_PRIVATE

public:
	static CXTPMarkupDependencyProperty* m_pX1Property;
	static CXTPMarkupDependencyProperty* m_pY1Property;
	static CXTPMarkupDependencyProperty* m_pX2Property;
	static CXTPMarkupDependencyProperty* m_pY2Property;
};

AFX_INLINE void CXTPMarkupLine::SetX1(int nValue)
{
	SetValue(m_pX1Property, new CXTPMarkupInt(nValue));
}

AFX_INLINE void CXTPMarkupLine::SetX2(int nValue)
{
	SetValue(m_pX2Property, new CXTPMarkupInt(nValue));
}

AFX_INLINE void CXTPMarkupLine::SetY1(int nValue)
{
	SetValue(m_pY1Property, new CXTPMarkupInt(nValue));
}

AFX_INLINE void CXTPMarkupLine::SetY2(int nValue)
{
	SetValue(m_pY2Property, new CXTPMarkupInt(nValue));
}

#	include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPMARKUPLINE_H__)
