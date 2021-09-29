// XTPMarkupFrameworkElement.h: interface for the CXTPMarkupFrameworkElement class.
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
#if !defined(__XTPMARKUPFRAMEWORKELEMENT_H__)
#	define __XTPMARKUPFRAMEWORKELEMENT_H__
//}}AFX_CODEJOCK_PRIVATE

#	if _MSC_VER > 1000
#		pragma once
#	endif // _MSC_VER > 1000

#	include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

class CXTPMarkupResourceDictionary;
class CXTPMarkupStyle;
class CXTPMarkupTriggerCollection;
class CXTPMarkupThickness;

//=============================================================================
// Summary:
//     This class represents the provided framework-level implementation
//     built on CXTPMarkupUIElement.
//=============================================================================
class _XTP_EXT_CLASS CXTPMarkupFrameworkElement : public CXTPMarkupUIElement
{
private:
	struct MINMAX
	{
	public:
		MINMAX();
		void Update(const CXTPMarkupFrameworkElement* pElement);

	public:
		int nMaxHeight;
		int nMinHeight;
		int nMaxWidth;
		int nMinWidth;
	};

private:
	DECLARE_MARKUPCLASS(CXTPMarkupFrameworkElement)

protected:
	//-----------------------------------------------------------------------
	// Summary:
	//     Initializes a new instance of the CXTPMarkupFrameworkElement class.
	//-----------------------------------------------------------------------
	CXTPMarkupFrameworkElement();

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPMarkupFrameworkElement object, handles cleanup and deallocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPMarkupFrameworkElement();

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to set the outer margin of an element.
	//  Parameters:
	//     nLeft   - Left margin.
	//     nTop    - Top margin.
	//     nRight  - Right margin.
	//     nBottom - Bottom margin.
	//-----------------------------------------------------------------------
	void SetMargin(int nLeft, int nTop, int nRight, int nBottom);
	void SetMargin(int nMargin);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to get the outer margin of an element
	// Returns:
	//     A pointer to a CXTPMarkupThickness object containing outer margin values.
	//-----------------------------------------------------------------------
	CXTPMarkupThickness* GetMargin() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets the minimum width constraint for the element.
	// Parameters:
	//     nWidth - New minimum width constraint.
	//-----------------------------------------------------------------------
	void SetMinWidth(int nWidth);

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets the maximum width constraint for the element.
	// Parameters:
	//     nWidth - New maximum width constraint.
	//-----------------------------------------------------------------------
	void SetMaxWidth(int nWidth);

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets the width for the element.
	// Parameters:
	//     nWidth - New width for the element.
	//-----------------------------------------------------------------------
	void SetWidth(int nWidth);

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets the minimum height constraint for the element.
	// Parameters:
	//     nHeight - New minimum height constraint.
	//-----------------------------------------------------------------------
	void SetMinHeight(int nHeight);

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets the maximum height constraint for the element.
	// Parameters:
	//     nHeight - New maximum height constraint.
	//-----------------------------------------------------------------------
	void SetMaxHeight(int nHeight);

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets the height for the element.
	// Parameters:
	//     nHeight - New height for the element.
	//-----------------------------------------------------------------------
	void SetHeight(int nHeight);

	//-----------------------------------------------------------------------
	// Summary:
	//     Gets the minimum width constraint of the element.
	//  Returns:
	//     The minimum width constraint.
	//-----------------------------------------------------------------------
	int GetMinWidth() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Gets the maximum width constraint of the element.
	//  Returns:
	//     The maximum width constraint.
	//-----------------------------------------------------------------------
	int GetMaxWidth() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Gets the width of the element.
	//  Returns:
	//     The width of the element.
	//-----------------------------------------------------------------------
	int GetWidth() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Gets the minimum height constraint of the element.
	//  Returns:
	//     The minimum height constraint.
	//-----------------------------------------------------------------------
	int GetMinHeight() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Gets the maximum height constraint of the element.
	//  Returns:
	//     The maximum height constraint.
	//-----------------------------------------------------------------------
	int GetMaxHeight() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Gets the height of the element.
	//  Returns:
	//     The height of the element.
	//-----------------------------------------------------------------------
	int GetHeight() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Gets the tag object of the element.
	//  Returns:
	//     The tag object.
	//-----------------------------------------------------------------------
	CXTPMarkupObject* GetTag() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets the tag object for the element.
	//  Parameters:
	//     pTag - New tag object to set.
	//-----------------------------------------------------------------------
	void SetTag(CXTPMarkupObject* pTag);

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets the horizontal alignment characteristics applied to this element when
	//     it is composed within a parent element (e.g. a panel or items control).
	//  Parameters:
	//     alignment - New XTPMarkupHorizontalAlignment enumeration value.
	//-----------------------------------------------------------------------
	void SetHorizontalAlignment(XTPMarkupHorizontalAlignment alignment);

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets the vertical alignment characteristics applied to this element when
	//     it is composed within a parent element (e.g. a panel or items control).
	//  Parameters:
	//     alignment - New XTPMarkupVerticalAlignment enumeration value.
	//-----------------------------------------------------------------------
	void SetVerticalAlignment(XTPMarkupVerticalAlignment alignment);

	//-----------------------------------------------------------------------
	// Summary:
	//     Gets the horizontal alignment characteristics applied to this element when
	//     it is composed within a parent element (e.g. a panel or items control).
	//  Returns:
	//     An XTPMarkupHorizontalAlignment enumeration value.
	//-----------------------------------------------------------------------
	XTPMarkupHorizontalAlignment GetHorizontalAlignment() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Gets the vertical alignment characteristics applied to this element when
	//     it is composed within a parent element (e.g. a panel or items control).
	//  Returns:
	//     An XTPMarkupVerticalAlignment enumeration value.
	//-----------------------------------------------------------------------
	XTPMarkupVerticalAlignment GetVerticalAlignment() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves the actual size of the element after it is measured and
	//     arranged within a parent element (e.g. a panel or items control).
	// Returns:
	//     A CSize object containing the actual size of the element.
	//-----------------------------------------------------------------------
	CSize GetActualSize() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Specifies if child elements should be clipped inside element bounds.
	// Parameters:
	//     bClipToBounds - TRUE to clip child elements inside element bounds.
	//-----------------------------------------------------------------------
	void SetClipToBounds(BOOL bClipToBounds);

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines if child elements are clipped inside element bounds.
	// Returns:
	//     TRUE if child elements are clipped inside element bounds.
	//-----------------------------------------------------------------------
	BOOL GetClipToBounds() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Gets the attached style of the element.
	// Returns:
	//     The attached style of the element.
	//-----------------------------------------------------------------------
	CXTPMarkupStyle* GetStyle() const;

protected:
	//{{AFX_CODEJOCK_PRIVATE
	// Implementation

	virtual CSize MeasureCore(CXTPMarkupDrawingContext* pDC, CSize szAvailableSize);
	virtual CSize MeasureOverride(CXTPMarkupDrawingContext* pDC, CSize szAvailableSize);

	virtual void ArrangeCore(CRect rcFinalRect);
	virtual CSize ArrangeOverride(CSize szFinalSize);

	BOOL GetLayoutClip(CRect& rc) const;
	virtual void OnPropertyChanged(CXTPMarkupDependencyProperty* pProperty,
								   CXTPMarkupObject* pOldValue, CXTPMarkupObject* pNewValue);

private:
	CPoint ComputeAlignmentOffset(CSize clientSize, CSize inkSize) const;

protected:
	BOOL m_bNeedsClipBounds;

	CSize m_szUnclippedDesiredSize;
	BOOL m_bUnclippedDesiredSize;

	MINMAX m_mmBounds;

public:
	static CXTPMarkupDependencyProperty* m_pMarginProperty;
	static CXTPMarkupDependencyProperty* m_pHorizontalAlignmentProperty;
	static CXTPMarkupDependencyProperty* m_pVerticalAlignmentProperty;
	static CXTPMarkupDependencyProperty* m_pWidthProperty;
	static CXTPMarkupDependencyProperty* m_pHeightProperty;
	static CXTPMarkupDependencyProperty* m_pMinWidthProperty;
	static CXTPMarkupDependencyProperty* m_pMinHeightProperty;
	static CXTPMarkupDependencyProperty* m_pMaxWidthProperty;
	static CXTPMarkupDependencyProperty* m_pMaxHeightProperty;
	static CXTPMarkupDependencyProperty* m_pTagProperty;
	//}}AFX_CODEJOCK_PRIVATE

	//{{AFX_CODEJOCK_PRIVATE
public:
	DECLARE_DISPATCH_MAP()
#	ifdef _XTP_ACTIVEX
	DECLARE_INTERFACE_MAP()
	DECLARE_OLETYPELIB_EX(CXTPMarkupFrameworkElement);
#	endif

	afx_msg long OleGetMinWidth();
	afx_msg long OleGetWidth();
	afx_msg long OleGetMaxWidth();
	afx_msg void OleSetMinWidth(int nWidth);
	afx_msg void OleSetWidth(int nWidth);
	afx_msg void OleSetMaxWidth(int nWidth);
	afx_msg long OleGetMinHeight();
	afx_msg long OleGetHeight();
	afx_msg long OleGetMaxHeight();
	afx_msg void OleSetMinHeight(int nHeight);
	afx_msg void OleSetHeight(int nHeight);
	afx_msg void OleSetMaxHeight(int nHeight);
	afx_msg LPDISPATCH OleGetMargin();
	afx_msg void OleSetMargin(LPDISPATCH lpMarginDisp);
	afx_msg long OleGetHorizontalAlignment();
	afx_msg long OleGetVerticalAlignment();
	afx_msg void OleSetHorizontalAlignment(long nValue);
	afx_msg void OleSetVerticalAlignment(long nValue);
	afx_msg VARIANT OleGetTag();
	afx_msg void OleSetTag(const VARIANT& va);

	//}}AFX_CODEJOCK_PRIVATE
};

AFX_INLINE void CXTPMarkupFrameworkElement::SetMinWidth(int nWidth)
{
	SetValue(m_pMinWidthProperty, new CXTPMarkupInt(nWidth));
}
AFX_INLINE void CXTPMarkupFrameworkElement::SetMaxWidth(int nWidth)
{
	SetValue(m_pMaxWidthProperty, new CXTPMarkupInt(nWidth));
}
AFX_INLINE void CXTPMarkupFrameworkElement::SetWidth(int nWidth)
{
	SetValue(m_pWidthProperty, new CXTPMarkupInt(nWidth));
}
AFX_INLINE void CXTPMarkupFrameworkElement::SetMinHeight(int nHeight)
{
	SetValue(m_pMinHeightProperty, new CXTPMarkupInt(nHeight));
}
AFX_INLINE void CXTPMarkupFrameworkElement::SetMaxHeight(int nHeight)
{
	SetValue(m_pMaxHeightProperty, new CXTPMarkupInt(nHeight));
}
AFX_INLINE void CXTPMarkupFrameworkElement::SetHeight(int nHeight)
{
	SetValue(m_pHeightProperty, new CXTPMarkupInt(nHeight));
}
AFX_INLINE void
	CXTPMarkupFrameworkElement::SetHorizontalAlignment(XTPMarkupHorizontalAlignment alignment)
{
	SetValue(m_pHorizontalAlignmentProperty, CXTPMarkupEnum::CreateValue(alignment));
}
AFX_INLINE void
	CXTPMarkupFrameworkElement::SetVerticalAlignment(XTPMarkupVerticalAlignment alignment)
{
	SetValue(m_pVerticalAlignmentProperty, CXTPMarkupEnum::CreateValue(alignment));
}
AFX_INLINE CSize CXTPMarkupFrameworkElement::GetActualSize() const
{
	return m_szRenderSize;
}
AFX_INLINE int CXTPMarkupFrameworkElement::GetMinWidth() const
{
	CXTPMarkupInt* pValue = MARKUP_STATICCAST(CXTPMarkupInt, GetValue(m_pMinWidthProperty));
	return pValue != NULL ? (int)*pValue : 0;
}
AFX_INLINE int CXTPMarkupFrameworkElement::GetMaxWidth() const
{
	CXTPMarkupInt* pValue = MARKUP_STATICCAST(CXTPMarkupInt, GetValue(m_pMaxWidthProperty));
	return pValue != NULL ? (int)*pValue : INT_MAX;
}
AFX_INLINE int CXTPMarkupFrameworkElement::GetWidth() const
{
	CXTPMarkupInt* pValue = MARKUP_STATICCAST(CXTPMarkupInt, GetValue(m_pWidthProperty));
	return pValue != NULL ? (int)*pValue : 0;
}
AFX_INLINE int CXTPMarkupFrameworkElement::GetMinHeight() const
{
	CXTPMarkupInt* pValue = MARKUP_STATICCAST(CXTPMarkupInt, GetValue(m_pMinHeightProperty));
	return pValue != NULL ? (int)*pValue : 0;
}
AFX_INLINE int CXTPMarkupFrameworkElement::GetMaxHeight() const
{
	CXTPMarkupInt* pValue = MARKUP_STATICCAST(CXTPMarkupInt, GetValue(m_pMaxHeightProperty));
	return pValue != NULL ? (int)*pValue : INT_MAX;
}
AFX_INLINE int CXTPMarkupFrameworkElement::GetHeight() const
{
	CXTPMarkupInt* pValue = MARKUP_STATICCAST(CXTPMarkupInt, GetValue(m_pHeightProperty));
	return pValue != NULL ? (int)*pValue : 0;
}
AFX_INLINE void CXTPMarkupFrameworkElement::SetTag(CXTPMarkupObject* pTag)
{
	SetValue(m_pTagProperty, pTag);
}
AFX_INLINE CXTPMarkupObject* CXTPMarkupFrameworkElement::GetTag() const
{
	return GetValue(m_pTagProperty);
}
AFX_INLINE CXTPMarkupStyle* CXTPMarkupFrameworkElement::GetStyle() const
{
	return m_pStyleCache;
}
AFX_INLINE void CXTPMarkupFrameworkElement::SetClipToBounds(BOOL bClipToBounds)
{
	SetValue(m_pClipToBoundsProperty, CXTPMarkupBool::CreateValue(bClipToBounds));
}

#	include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPMARKUPFRAMEWORKELEMENT_H__)
