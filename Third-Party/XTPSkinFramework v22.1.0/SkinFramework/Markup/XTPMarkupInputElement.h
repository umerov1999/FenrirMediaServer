// XTPMarkupInputElement.h: interface for the CXTPMarkupInputElement class.
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
#if !defined(__XTPMARKUPINPUTELEMENT_H__)
#	define __XTPMARKUPINPUTELEMENT_H__
//}}AFX_CODEJOCK_PRIVATE

#	if _MSC_VER > 1000
#		pragma once
#	endif // _MSC_VER > 1000

#	include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

class CXTPMarkupString;
class CXTPMarkupContext;
class CXTPMarkupTriggerCollection;

class CXTPMarkupEventHandlerMap;
class CXTPMarkupDelegate;
class CXTPMarkupRoutedEventArgs;
class CXTPMarkupMouseEventArgs;
class CXTPMarkupMouseWheelEventArgs;
class CXTPMarkupQueryCursorEventArgs;
class CXTPMarkupMouseButtonEventArgs;
class CXTPMarkupTransformationMatrix;

class _XTP_EXT_CLASS CXTPMarkupInputElement : public CXTPMarkupObject
{
	DECLARE_MARKUPCLASS(CXTPMarkupInputElement);

protected:
	CXTPMarkupInputElement();
	virtual ~CXTPMarkupInputElement();

public:
	virtual void OnMouseLeave(CXTPMarkupMouseEventArgs* e);
	virtual void OnMouseEnter(CXTPMarkupMouseEventArgs* e);
	virtual void OnMouseMove(CXTPMarkupMouseEventArgs* e);
	virtual void OnQueryCursor(CXTPMarkupQueryCursorEventArgs* e);

	virtual void OnMouseLeftButtonUp(CXTPMarkupMouseButtonEventArgs* e);
	virtual void OnMouseRightButtonUp(CXTPMarkupMouseButtonEventArgs* e);
	virtual void OnMouseLeftButtonDown(CXTPMarkupMouseButtonEventArgs* e);
	virtual void OnMouseRightButtonDown(CXTPMarkupMouseButtonEventArgs* e);

	virtual void OnLostMouseCapture(CXTPMarkupMouseEventArgs* e);
	CXTPMarkupContext* GetMarkupContext() const;

	virtual void OnMouseWheel(CXTPMarkupMouseWheelEventArgs* e);

	void Apply();

public:
	virtual CXTPMarkupInputElement* InputHitTest(CPoint point) const;

	BOOL IsMouseOver() const;
	CXTPMarkupObject* GetToolTip() const;
	void SetToolTip(LPCTSTR lpszTooltip);

	void AddHandler(CXTPMarkupRoutedEvent* pEvent, CXTPMarkupDelegate* pDelegate);

	CXTPMarkupObject* FindResource(const CXTPMarkupObject* pKey) const;

	void Focus();
	BOOL IsKeyboardFocused() const;

	CXTPMarkupString* GetName() const;
	void SetName(LPCTSTR lpszName);

protected:
	void RaiseEvent(CXTPMarkupRoutedEventArgs* pEventArgs);

protected:
	void CaptureMouse();
	void ReleaseMouseCapture();

protected:
	void FireTriggers(CXTPMarkupDependencyProperty* pProperty, CXTPMarkupObject* pNewValue);
	BOOL AddStyleTriggers(CXTPMarkupStyle* pStyle, CXTPMarkupDependencyProperty* pProperty,
						  CXTPMarkupObject* pNewValue);

protected:
	void SetPropertyObject(CXTPMarkupBuilder* pBuilder, CXTPMarkupDependencyProperty* pProperty,
						   CXTPMarkupObject* pValue);
	virtual void OnFinalRelease();
	virtual void OnApply();
	void ApplyTree(CXTPMarkupObject* pChild);

	static void AFX_CDECL OnStylePropertyChanged(CXTPMarkupObject* d,
												 CXTPMarkupPropertyChangedEventArgs* /*e*/);
	static void AFX_CDECL OnCursorPropertyChanged(CXTPMarkupObject* d,
												  CXTPMarkupPropertyChangedEventArgs* e);
	virtual CXTPMarkupObject* GetValueCore(CXTPMarkupDependencyProperty* pProperty) const;

protected:
	//{{AFX_CODEJOCK_PRIVATE
public:
	DECLARE_DISPATCH_MAP()
#	ifdef _XTP_ACTIVEX
	DECLARE_INTERFACE_MAP()
	DECLARE_OLETYPELIB_EX(CXTPMarkupInputElement);
#	endif

	afx_msg long OleGetCursor();
	afx_msg void OleSetCursor(long nValue);
	afx_msg VARIANT OleGetToolTip();
	afx_msg void OleSetToolTip(const VARIANT& va);
	afx_msg BOOL OleGetIsMouseOver();
	afx_msg void OleAddHandler(LPDISPATCH lpEventDisp, LPCOLESTR Method);
	afx_msg LPDISPATCH OleFindName(LPCOLESTR lpszName);
	void OleSetName(LPCTSTR lpszName);
	BSTR OleGetName();

	afx_msg LPDISPATCH OleGetMouseLeaveEvent();
	afx_msg LPDISPATCH OleGetMouseEnterEvent();
	afx_msg LPDISPATCH OleGetMouseLeftButtonUpEvent();
	afx_msg LPDISPATCH OleGetMouseLeftButtonDownEvent();
	afx_msg LPDISPATCH OleGetMouseRightButtonUpEvent();
	afx_msg LPDISPATCH OleGetMouseRightButtonDownEvent();
	afx_msg LPDISPATCH OleGetMouseMoveEvent();
	afx_msg LPDISPATCH OleGetLostMouseCaptureEvent();

	//}}AFX_CODEJOCK_PRIVATE

protected:
	CXTPMarkupEventHandlerMap* m_pHandlers;
	CXTPMarkupContext* m_pMarkupContext;
	CXTPMarkupTriggerCollection* m_pActiveTriggers;
	CXTPMarkupProperties* m_pTriggerProperties;
	CXTPMarkupStyle* m_pStyleCache;

public:
	static CXTPMarkupDependencyProperty* m_pCursorProperty;
	static CXTPMarkupDependencyProperty* m_pIsMouseOverProperty;
	static CXTPMarkupDependencyProperty* m_pStyleProperty;
	static CXTPMarkupDependencyProperty* m_pResourcesProperty;
	static CXTPMarkupDependencyProperty* m_pToolTipProperty;

	static CXTPMarkupDependencyProperty* m_pIsKeyboardFocusedProperty;
	static CXTPMarkupDependencyProperty* m_pIsFocusableProperty;

	static CXTPMarkupRoutedEvent* m_pMouseLeaveEvent;
	static CXTPMarkupRoutedEvent* m_pMouseEnterEvent;
	static CXTPMarkupRoutedEvent* m_pMouseLeftButtonUpEvent;
	static CXTPMarkupRoutedEvent* m_pMouseLeftButtonDownEvent;
	static CXTPMarkupRoutedEvent* m_pMouseRightButtonUpEvent;
	static CXTPMarkupRoutedEvent* m_pMouseRightButtonDownEvent;
	static CXTPMarkupRoutedEvent* m_pMouseMoveEvent;
	static CXTPMarkupRoutedEvent* m_pLostMouseCaptureEvent;

private:
	friend class CXTPMarkupContext;
};

AFX_INLINE CXTPMarkupContext* CXTPMarkupInputElement::GetMarkupContext() const
{
	return m_pMarkupContext;
}

//===========================================================================
// Summary:
//     Provides rendering support for XML Markup, which includes hit testing,
//     coordinate transformation, and bounding box calculations.
//===========================================================================
class _XTP_EXT_CLASS CXTPMarkupVisual : public CXTPMarkupInputElement
{
	DECLARE_MARKUPCLASS(CXTPMarkupVisual);

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Provides the base initialization for objects derived from the
	//     CXTPMarkupVisual class.
	//-----------------------------------------------------------------------
	CXTPMarkupVisual();

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPMarkupVisual object, handles cleanup and deallocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPMarkupVisual();

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to draw a Visual object inside a drawing context.
	// Parameters:
	//     drawingContext - Drawing context to draw in.
	//-----------------------------------------------------------------------
	void Render(CXTPMarkupDrawingContext* drawingContext);

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines if a point belongs to this object.
	// Input:
	//     point - Point to test.
	// Returns:
	//     The CXTPMarkupInputElement at the specified point, if any,
	//     otherwise NULL.
	//-----------------------------------------------------------------------
	CXTPMarkupInputElement* InputHitTest(CPoint point) const;

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Gets the number of child elements for the Visual.
	// Remarks:
	//     By default, a Visual has no children. Therefore, the default
	//     implementation always returns 0.
	// Returns:
	//     The number of child elements for the Visual.
	//-----------------------------------------------------------------------
	virtual int GetVisualChildrenCount() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves the child Visual object located at a specified index within
	//     the parent VisualCollection.
	// Parameters:
	//     nIndex - Specified index within the parent VisualCollection.
	// Returns:
	//     The child Visual object located at the specified index within
	//     the parent VisualCollection.
	//-----------------------------------------------------------------------
	virtual CXTPMarkupVisual* GetVisualChild(int nIndex) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves the number of logical child elements of this object.
	// Returns:
	//     The number of logical child elements of this object.
	//-----------------------------------------------------------------------
	virtual int GetLogicalChildrenCount() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves the logical child element located at a specified index
	//     within the parent collection.
	// Parameters:
	//     nIndex - Specified index within the parent collection.
	// Returns:
	//     The logical child element located at the specified index within
	//     the parent collection.
	// Remarks:
	//     Logical children allow you to iterate over child elements. This
	//     is useful for elements that may not have a defined, dedicated
	//     collection, but still contain more than one child element.
	//-----------------------------------------------------------------------
	virtual CXTPMarkupObject* GetLogicalChild(int nIndex) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves the bounding rectangle of the element.
	// Returns:
	//     A CRect object containing the bounding rectangle of the element.
	//-----------------------------------------------------------------------
	CRect GetBoundRect() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Computes the update rectangle.
	// Returns:
	//     The update rectangle.
	//-----------------------------------------------------------------------
	CRect GetUpdateRect() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Gets the final render size of this element.
	// Returns:
	//     The final render size of this element.
	//-----------------------------------------------------------------------
	CSize GetRenderSize() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Gets or sets the offset value of the visual object.
	// Returns:
	//     The offset value of the visual object.
	//-----------------------------------------------------------------------
	CPoint GetVisualOffset() const;

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
	//     Gets the Visual tree parent of the Visual object.
	// Returns:
	//     The Visual tree parent of the Visual object.
	//-----------------------------------------------------------------------
	CXTPMarkupVisual* GetVisualParent() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     A structure that optionally describes the previous and the
	//     new transformation matrices.
	//-----------------------------------------------------------------------
	struct TransformationMatrices
	{
		//-------------------------------------------------------------------
		// Summary:
		//     Constructs an empty object.
		//-------------------------------------------------------------------
		TransformationMatrices();

		//-------------------------------------------------------------------
		// Summary:
		//     Constructs an object with specified old and new tranformation matrices.
		// Parameters:
		//    pOldMatrix - A pointer to an old tranformation matrix.
		//    pNewMatrix - A pointer to a new tranformation matrix.
		//-------------------------------------------------------------------
		TransformationMatrices(CXTPMarkupTransformationMatrix* pOldMatrix,
							   CXTPMarkupTransformationMatrix* pNewMatrix);

		CXTPMarkupTransformationMatrix* pOldMatrix; // A pointer to an old tranformation matrix.
		CXTPMarkupTransformationMatrix* pNewMatrix; // A pointer to a new tranformation matrix.
	};

protected:
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
	//     Checks if a point is inside a rectangle with transformation applied.
	// Parameters:
	//     rect  - Rectangle relative to object position.
	//     point - Point to check.
	// Returns:
	//     TRUE if the point is inside the specified rectangle.
	//-----------------------------------------------------------------------
	BOOL PtInRectTransform(const RECT& rect, const POINT& point) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Gets the absolute element visual offset.
	// Return:
	//     The absolute element visual offset.
	//-----------------------------------------------------------------------
	POINT GetAbsoluteVisualOffset() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     When overridden in a derived class, participates in rendering
	//     operations that are directed by the layout system.
	// Parameters:
	//     drawingContext - Drawing context to draw in.
	//-----------------------------------------------------------------------
	virtual void OnRender(CXTPMarkupDrawingContext* drawingContext);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to render the focus rectangle of a
	//     focused object.
	// Parameters:
	//     drawingContext - Drawing context to draw in.
	//-----------------------------------------------------------------------
	virtual void OnRenderFocusVisual(CXTPMarkupDrawingContext* drawingContext);

	//-----------------------------------------------------------------------
	// Summary:
	//     Override this method in the derived class to provide visual hit
	//     testing of a child object.
	// Parameters:
	//     point - Position of the cursor to test.
	// Returns:
	//     The CXTPMarkupInputElement at the specified position, if any,
	//     otherwise NULL.
	//-----------------------------------------------------------------------
	virtual CXTPMarkupInputElement* InputHitTestOverride(CPoint point) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Called by the framework when an element is clipped and no rendering will occur.
	// Parameters:
	//     drawingContext - Pointer to the markup drawing context.
	//-----------------------------------------------------------------------
	virtual void RenderClipped(CXTPMarkupDrawingContext* drawingContext);

	//-----------------------------------------------------------------------
	// Summary:
	//     Gets either the default or explicitly set opacity factor value.
	// Returns:
	//     An opacity factor value in the range 0..1.
	//-----------------------------------------------------------------------
	virtual double GetOpacity() const;

	//{{AFX_CODEJOCK_PRIVATE
	// Internal methods
protected:
	void UpdateBoundRect();
	CXTPMarkupTransformationMatrix* GetAbsoluteTransformationMatrix() const;
	void SetCurrentTransformationMatrix(CXTPMarkupTransformationMatrix* pMatrix);
	//}}AFX_CODEJOCK_PRIVATE

	//{{AFX_CODEJOCK_PRIVATE
public:
	DECLARE_DISPATCH_MAP()
#	ifdef _XTP_ACTIVEX
	DECLARE_INTERFACE_MAP()
	DECLARE_OLETYPELIB_EX(CXTPMarkupVisual);
#	endif

	afx_msg LPDISPATCH OleGetVisualParent();
	afx_msg void OleGetBoundingRect(int* left, int* top, int* right, int* bottom);
	afx_msg int OleGetActualWidth();
	afx_msg int OleGetActualHeight();
	//}}AFX_CODEJOCK_PRIVATE

protected:
	CPoint m_ptVisualOffset;  // The offset value of the visual object.
	CSize m_szRenderSize;	 // The rendered size for this element.
	CRect m_rcBoundRect;	  // Bounding rectangle of the element
	CRect m_rcUpdateRect;	 // Update rectangle.
	CRect m_rcPrevUpdateRect; // Previous update rectangle.
	CXTPMarkupTransformationMatrix* m_pCurrentTransformationMatrix;
};

AFX_INLINE CXTPMarkupVisual::TransformationMatrices::TransformationMatrices()
	: pOldMatrix(NULL)
	, pNewMatrix(NULL)
{
}

AFX_INLINE CXTPMarkupVisual::TransformationMatrices::TransformationMatrices(
	CXTPMarkupTransformationMatrix* pOldMatrix, CXTPMarkupTransformationMatrix* pNewMatrix)
	: pOldMatrix(pOldMatrix)
	, pNewMatrix(pNewMatrix)
{
}

AFX_INLINE void CXTPMarkupVisual::OnRender(CXTPMarkupDrawingContext* /*drawingContext*/)
{
}

AFX_INLINE void CXTPMarkupVisual::OnRenderFocusVisual(CXTPMarkupDrawingContext* /*drawingContext*/)
{
}

AFX_INLINE CXTPMarkupVisual::TransformationMatrices
	CXTPMarkupVisual::ApplyRenderTransform(CXTPMarkupDrawingContext* pContext)
{
	UNREFERENCED_PARAMETER(pContext);
	return TransformationMatrices();
}

AFX_INLINE void CXTPMarkupVisual::RestoreRenderTransform(CXTPMarkupDrawingContext* pContext,
														 TransformationMatrices& matrices)
{
	UNREFERENCED_PARAMETER(pContext);
	UNREFERENCED_PARAMETER(matrices);
}

AFX_INLINE CSize CXTPMarkupVisual::GetRenderSize() const
{
	return m_szRenderSize;
}

AFX_INLINE CRect CXTPMarkupVisual::GetBoundRect() const
{
	return m_rcBoundRect;
}

AFX_INLINE CPoint CXTPMarkupVisual::GetVisualOffset() const
{
	return m_ptVisualOffset;
}

#	include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPMARKUPINPUTELEMENT_H__)
