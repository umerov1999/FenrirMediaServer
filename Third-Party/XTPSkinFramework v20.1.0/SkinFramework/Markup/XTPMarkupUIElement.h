// XTPMarkupUIElement.h: interface for the CXTPMarkupUIElement class.
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
#if !defined(__XTPMARKUPUIELEMENT_H__)
#	define __XTPMARKUPUIELEMENT_H__
//}}AFX_CODEJOCK_PRIVATE

#	if _MSC_VER > 1000
#		pragma once
#	endif // _MSC_VER > 1000

#	include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

class CXTPMarkupContext;
class CXTPMarkupRenderTransform;
class CXTPMarkupTransformationMatrix;

//=============================================================================
// Summary:
//     Base class for XML Markup core level implementations.
//=============================================================================
class _XTP_EXT_CLASS CXTPMarkupUIElement : public CXTPMarkupVisual
{
	DECLARE_MARKUPCLASS(CXTPMarkupUIElement);

protected:
	//-------------------------------------------------------------------------
	// Summary:
	//     Initializes a new instance of the CXTPMarkupUIElement class.
	//-------------------------------------------------------------------------
	CXTPMarkupUIElement();

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPMarkupUIElement object, handles cleanup and deallocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPMarkupUIElement();

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Positions child elements and determines a size for a
	//     CXTPMarkupUIElement. Parent elements call this method from their
	//     ArrangeOverride implementation to form a recursive layout update.
	//     This method constitutes the second pass of a layout update.
	// Parameters:
	//     rcFinalRect - The final size that the parent computes for the child
	//                   element, provided as a CRect instance.
	//-----------------------------------------------------------------------
	void Arrange(CRect rcFinalRect);

	//-----------------------------------------------------------------------
	// Summary:
	//     Updates the DesiredSize of a CXTPMarkupUIElement. Parent elements
	//     call this method from their own MeasureOveride implementations
	//     to form a recursive layout update. Calling this method constitutes
	//     the first pass (the "Measure" pass) of a layout update.
	// Parameters:
	//     pDC             - Pointer to a CXTPMarkupDrawingContext.
	//     szAvailableSize - The available space that a parent element can
	//                       allocate to a child element. A child element can
	//                       request a larger space than what is available; the
	//                       provided size might be accommodated if scrolling is
	//                       possible in the content model for the current element.
	//-----------------------------------------------------------------------
	void Measure(CXTPMarkupDrawingContext* pDC, CSize szAvailableSize);

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Gets the size that this element computed during the measure pass of
	//     the layout process.
	// Returns:
	//     The size that this element computed during the measure pass of the
	//     layout process; this size becomes the DesiredSize for the arrange pass.
	// Remarks:
	//     DesiredSize is typically checked as one of the measurement
	//     factors when you implement layout behavior overrides such
	//     as ArrangeOverride, MeasureOverride, or OnRender.
	//-----------------------------------------------------------------------
	CSize GetDesiredSize() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Gets the final render size of this element.
	// Returns:
	//     The rendered size of this element.
	//-----------------------------------------------------------------------
	CRect GetFinalRect() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Gets the user interface (UI) visibility of this element. This
	//     is a dependency property.
	// Returns:
	//     An XTPMarkupVisibility enumerator value.
	//-----------------------------------------------------------------------
	XTPMarkupVisibility GetVisibility() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets the user interface (UI) visibility of this element. This
	//     is a dependency property.
	// Parameters:
	//     visibility - An XTPMarkupVisibility enumerator value;
	//                  the default value is visible.
	//-----------------------------------------------------------------------
	void SetVisibility(XTPMarkupVisibility visibility);

	//-----------------------------------------------------------------------
	// Summary:
	//     Gets or sets a value indicating whether this element can
	//     receive focus. This is a dependency property.
	// Returns:
	//     TRUE if this element is focusable, otherwise FALSE.
	//     The default is FALSE.
	//-----------------------------------------------------------------------
	BOOL IsFocusable() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Gets a value indicating whether this element is visible in
	//     the user interface (UI). This is a dependency property.
	// Returns:
	//     TRUE if this element is visible, otherwise FALSE.
	//-----------------------------------------------------------------------
	BOOL IsVisible() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Gets or sets a value indicating whether this element is enabled
	//     in the user interface (UI). This is a dependency property.
	// Returns:
	//     TRUE if the element is enabled, otherwise FALSE.
	//     The default value is TRUE.
	//-----------------------------------------------------------------------
	BOOL IsEnabled() const;

public:
	//-------------------------------------------------------------------------
	// Summary:
	//     Invalidates the arrange state (layout) for the element.
	// Remarks:
	//     Frequent calls to InvalidateArrange have significant performance
	//     consequences. Therefore, avoid calling this method unless you
	//     absolutely require a precise layout state for subsequent calls to
	//     other APIs in your code.
	//-------------------------------------------------------------------------
	virtual void InvalidateArrange();

	//-------------------------------------------------------------------------
	// Summary:
	//     Invalidates the rendering of the element, and forces a new complete
	//     layout pass. OnRender is called after the layout cycle is completed.
	// Remarks:
	//     This method calls InvalidateArrange internally.
	//-------------------------------------------------------------------------
	virtual void InvalidateVisual();

	//-------------------------------------------------------------------------
	// Summary:
	//     Invalidates the measurement state (layout) for the element.
	// Remarks:
	//     Calling this method also calls InvalidateArrange internally, there is
	//     no need to call InvalidateMeasure and InvalidateArrange in succession.
	//-------------------------------------------------------------------------
	virtual void InvalidateMeasure();

protected:
	//-----------------------------------------------------------------------
	// Summary:
	//     Obtains a pointer to the RenderTransform object.
	// Returns:
	//     A pointer to the RenderTransform object, or NULL if the RenderTransform
	//     object is not specified for the currect Markup element.
	//-----------------------------------------------------------------------
	CXTPMarkupRenderTransform* GetRenderTransform() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Defines the template for the core-level arrange layout definition.
	// Parameters:
	//     rcFinalRect - The final area within the parent that an element should
	//                   use to arrange itself and its child elements.
	//-----------------------------------------------------------------------
	virtual void ArrangeCore(CRect rcFinalRect);

	//-----------------------------------------------------------------------
	// Summary:
	//     When overridden in a derived class, provides measurement logic
	//     for sizing this element properly with consideration of the
	//     size of any child element content.
	// Parameters:
	//     pDC             - Device context.
	//     szAvailableSize - The available size that the parent element can
	//                       allocate for the child.
	// Returns:
	//     The desired size of this element in layout.
	//-----------------------------------------------------------------------
	virtual CSize MeasureCore(CXTPMarkupDrawingContext* pDC, CSize szAvailableSize);

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves an alternative clipping geometry that represents the region that
	//     would be clipped if child elements were clipped inside element bounds.
	// Parameters:
	//     rc - The available size provided by the element.
	// Returns:
	//     The potential clipping geometry.
	//-----------------------------------------------------------------------
	virtual BOOL GetLayoutClip(CRect& rc) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Override this method to provide a custom reaction to an InvalidateMeasure call.
	// Parameters:
	//     pDC - Device context to measure.
	//-----------------------------------------------------------------------
	virtual void InvalidateMeasureOverride(CXTPMarkupDrawingContext* pDC);

	//-----------------------------------------------------------------------
	// Summary:
	//     Applies render transformation if necessary.
	// Parameters:
	//     pContext - Drawing context to apply the render transformation for.
	// Returns:
	//     A structure that optionally describes the previous and the new
	//     transformation matrices.
	//-----------------------------------------------------------------------
	virtual TransformationMatrices ApplyRenderTransform(CXTPMarkupDrawingContext* pContext);

	//-----------------------------------------------------------------------
	// Summary:
	//     Restores a previously applied render transformation.
	// Parameters:
	//     pContext - Drawing context to restore the render transformation for.
	//     matrices - Transformation matrices previously returned by a call
	//                to ApplyRenderTransform.
	//-----------------------------------------------------------------------
	virtual void RestoreRenderTransform(CXTPMarkupDrawingContext* pContext,
										TransformationMatrices& matrices);

	//-----------------------------------------------------------------------
	// Summary:
	//     Gets either the default or explicitly set opacity factor value.
	// Returns:
	//     An opacity factor value in the range 0..1.
	//-----------------------------------------------------------------------
	virtual double GetOpacity() const;

	//{{AFX_CODEJOCK_PRIVATE
public:
	DECLARE_DISPATCH_MAP()
#	ifdef _XTP_ACTIVEX
	DECLARE_INTERFACE_MAP()
	DECLARE_OLETYPELIB_EX(CXTPMarkupUIElement);
#	endif

	afx_msg BOOL OleGetClipToBounds();
	afx_msg void OleSetClipToBounds(BOOL bValue);
	afx_msg long OleGetVisibility();
	afx_msg void OleSetVisibility(long nValue);
	afx_msg BOOL OleGetFocusable();
	afx_msg void OleSetFocusable(BOOL bValue);
	afx_msg BOOL OleGetIsEnabled();
	afx_msg void OleSetIsEnabled(BOOL bValue);
	afx_msg double OleGetOpacity();
	afx_msg void OleSetOpacity(double dValue);
	afx_msg LPDISPATCH OleGetRenderTransform();

	//}}AFX_CODEJOCK_PRIVATE

protected:
	CSize m_szDesiredSize; // Gets the size that this element computed during the measure pass of
						   // the layout process.
	CRect m_rcFinalRect;   // Final position of the visual element.
	BOOL m_bMeasureDirty;  // TRUE if object hasn't recalculated its size.
	BOOL m_bNeverMeasured; // TRUE if object has never ran Measure to update its size.
	CSize m_szPreviousAvailableSize; // Previous available size of measure call.
	BOOL m_bArrangeDirty;			 // TRUE if object position can be invalid.
	BOOL m_bNeverArranged;			 // TRUE if Arrange was never called for this object.
	BOOL m_bArrangeInProgress;		 // TRUE if Arrange is currently executed.
	BOOL m_bMeasureInProgress;		 // TRUE if Measure is currently executed.

public:
	static CXTPMarkupDependencyProperty* m_pClipToBoundsProperty; // Gets or sets a value indicating
																  // whether to clip the content of
																  // this element (or content coming
																  // from the child elements of this
																  // element) to fit into the size
																  // of the containing element. This
																  // is a dependency property.
	static CXTPMarkupDependencyProperty* m_pVisibilityProperty;	  // A value indicating whether this
																  // element is visible in the user
																  // interface (UI).
	static CXTPMarkupDependencyProperty* m_pFocusableProperty; // A value that indicates whether the
															   // element can receive focus.
	static CXTPMarkupDependencyProperty* m_pIsEnabledProperty; // A value indicating whether this
															   // element is enabled in the user
															   // interface (UI).
	static CXTPMarkupDependencyProperty* m_pRenderTransformProperty; // Gets or sets transform
																	 // information that affects the
																	 // rendering position of this
																	 // element.
	static CXTPMarkupDependencyProperty* m_pOpacityProperty; // Gets or sets the opacity factor
															 // applied to the entire UIElement when
															 // it is rendered in the user interface
															 // (UI).

	friend class CXTPMarkupThemePart;

private:
	static double NormalizeOpacityValue(double dValue);
};

AFX_INLINE CSize CXTPMarkupUIElement::GetDesiredSize() const
{
	return m_szDesiredSize;
}

AFX_INLINE CRect CXTPMarkupUIElement::GetFinalRect() const
{
	return m_rcFinalRect;
}

#	include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPMARKUPUIELEMENT_H__)
