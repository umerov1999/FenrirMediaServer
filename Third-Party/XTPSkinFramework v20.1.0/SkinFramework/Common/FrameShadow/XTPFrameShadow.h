// XTPFrameShadow.h: interface for the CXTPFrameShadow class.
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
#if !defined(__XTPFRAMESHADOW_H__)
#	define __XTPFRAMESHADOW_H__
//}}AFX_CODEJOCK_PRIVATE

#	if _MSC_VER > 1000
#		pragma once
#	endif // _MSC_VER > 1000

#	include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

class CXTPFrameShadowManager;

//===========================================================================
// Summary:
//     The class represents frame shadow object. Frame shadow is a rounded
//     rectangle beyond a window to which it is assigned. The thickness of
//     shadow area and the radius of the corners are controlled by outer
//     and inner shadow size, that is  a size between window border and the
//     outermost shadow edge (outer size), and a size between window border
//     and the innermost shadow edge (inner size).
//===========================================================================
class _XTP_EXT_CLASS CXTPFrameShadow
	: public CWnd
	, IXTPFrameShadowManagerEvents
{
	DECLARE_DYNAMIC(CXTPFrameShadow)
	friend class CXTPFrameShadowManager;

	using CWnd::Create;

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs frame shadow object.
	//-----------------------------------------------------------------------
	explicit CXTPFrameShadow(HWND hOwner);

	//-----------------------------------------------------------------------
	// Summary:
	//     Handles frame shadow deallocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPFrameShadow();

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines if frame shadows are supported by the platform.
	// Returns:
	//     TRUE if frame shadows are supported, otherwise FALSE.
	//-----------------------------------------------------------------------
	static BOOL AFX_CDECL IsShadowSupported();

	//-----------------------------------------------------------------------
	// Summary:
	//     Creates frame shadow and initializes it with initial parameters.
	// Parameters:
	//     crColor - Shadow color.
	//     nOuterSize - The outer size of a shadow, i.e. the size between window
	//                  border and shadow edge outside the window.
	//     nInnerSize - The inner size of a shadow, i.e. the size between window
	//                  border and shadow edge under the window.
	//     ptOffset - Shadow offset.
	//     bTransparency - Shadow transparency, 0 means fully transparent,
	//                     255 means fully opaque.
	// Returns:
	//     TRUE if frame shadows are supported, otherwise FALSE.
	// See Also:
	//     IsCreated
	//-----------------------------------------------------------------------
	BOOL Create(COLORREF crColor, UINT nOuterSize, UINT nInnerSize = 0,
				CPoint ptOffset = CPoint(0, 0), BYTE bTransparency = 0x80);

	//-----------------------------------------------------------------------
	// Summary:
	//     Freezes/unfreezes updates on changing shadow settings, as a result
	//     shadow blinking is reduced.
	// Parameters:
	//     bFreeze - If TRUE the update freeze counter is incremented. Otherwise
	//               the update freeze counter is decremented.
	//-----------------------------------------------------------------------
	void FreezeUpdates(BOOL bFreeze = TRUE);

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines if the shadows has been created.
	// Returns:
	//     TRUE if the shadow has been created.
	// See Also:
	//     Create
	//-----------------------------------------------------------------------
	BOOL IsCreated() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Obtains an owning window pointer.
	// Returns:
	//     An owning window handle.
	//-----------------------------------------------------------------------
	HWND GetOwner() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Obtains the value of the outer size.
	// Returns:
	//     The value of the outer size.
	// See Also:
	//     SetOuterSize
	//-----------------------------------------------------------------------
	UINT GetOuterSize() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets the value of the outer size.
	// Parameters:
	//     nSize - The value of the outer size.
	// See Also:
	//     GetOuterSize
	//-----------------------------------------------------------------------
	void SetOuterSize(UINT nSize);

	//-----------------------------------------------------------------------
	// Summary:
	//     Obtains the value of the inner size.
	// Returns:
	//     The value of the inner size.
	// See Also:
	//     SetInnerSize
	//-----------------------------------------------------------------------
	UINT GetInnerSize() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets the value of the inner size.
	// Parameters:
	//     nSize - The value of the inner size.
	// See Also:
	//     GetInnerSize
	//-----------------------------------------------------------------------
	void SetInnerSize(UINT nSize);

	//-----------------------------------------------------------------------
	// Summary:
	//     Obtains offset value.
	// Returns:
	//     Offset value.
	// See Also:
	//     SetOffset
	//-----------------------------------------------------------------------
	CPoint GetOffset() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets offset value.
	// Parameters:
	//     ptOffset - Offset value.
	// See Also:
	//     GetOffset
	//-----------------------------------------------------------------------
	void SetOffset(CPoint ptOffset);

	//-----------------------------------------------------------------------
	// Summary:
	//     Obtains color value.
	// Returns:
	//     Color value.
	// See Also:
	//     SetColor
	//-----------------------------------------------------------------------
	COLORREF GetColor() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets color value.
	// Parameters:
	//     crColor - Color value.
	// See Also:
	//     GetColor
	//-----------------------------------------------------------------------
	void SetColor(COLORREF crColor);

	//-----------------------------------------------------------------------
	// Summary:
	//     Obtains transparency value. 0 means fully transparent, 255 means fully opaque.
	// Returns:
	//     Transparency value.
	// See Also:
	//     SetTransparency
	//-----------------------------------------------------------------------
	BYTE GetTransparency() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets transparency value. 0 means fully transparent, 255 means fully opaque.
	// Parameters:
	//     bTransparency - Transparency value.
	// See Also:
	//     GetTransparency
	//-----------------------------------------------------------------------
	void SetTransparency(BYTE bTransparency);

	//-----------------------------------------------------------------------
	// Summary:
	//     Obtains shadow blending options. The pFactors and pPositions arrays are of
	//     the same size where each factor is applied at the appropriate position
	//     specified. A factor value has to be in a range [0..1], where 0 means fully
	//     transparent shadow and 1 means fully opaque shadow at the specified position.
	//     The position array has to be either empty or has at least two values { 0, 1 }.
	//     The first position has always be equal to 0, and the last - to 1, positions in
	//     between have to be sorted in the ascending order left-to-right and should
	//     be in a range (0, 1).
	// Parameters:
	//     pFactors - Optional. If not NULL it should a pointer to an array of factors,
	//                large enough to hold the number of elements returned by the function.
	//     pPositions - Optional. If not NULL it should a pointer to an array of positions,
	//                  large enough to hold the number of elements returned by the function.
	// Returns:
	//     A number of blenging elements or 0 if no blending options applied.
	// See Also:
	//     SetBlendingOptions
	//-----------------------------------------------------------------------
	UINT GetBlendingOptions(OUT OPTIONAL float* pFactors, OUT OPTIONAL float* pPositions) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets shadow blending options. The pFactors and pPositions arrays are of
	//     the same size where each factor is applied at the appropriate position
	//     specified. A factor value has to be in a range [0..1], where 0 means fully
	//     transparent shadow and 1 means fully opaque shadow at the specified position.
	//     The position array has to be either empty or has at least two values { 0, 1 }.
	//     The first position has always be equal to 0, and the last - to 1, positions in
	//     between have to be sorted in the ascending order left-to-right and should
	//     be in a range (0, 1).
	// Parameters:
	//     nCount - Number of elements in pFactors and pPositions arrays. If equals to 0
	//              the pFactors and pPositions values are ignored and blending options are
	//              disabled.
	//     pFactors - A pointer to an array of factors of nCount elements.
	//     pPositions - A pointer to an array of positions of nCount elements.
	// See Also:
	//     GetBlendingOptions
	//-----------------------------------------------------------------------
	void SetBlendingOptions(UINT nCount, const float* pFactors, const float* pPositions);

	//-----------------------------------------------------------------------
	// Summary:
	//     Obtains or sets owner clipping for the current shadow only.
	// Remarks:
	//     By default owner clipping is enabled in order to reduce flickering
	//     and improve rendering speed in slow environments. Disabling owner
	//     clipping may be necessary for owner windows that non-rectangular shape
	//     as the clipping box is always rectangular.
	//-----------------------------------------------------------------------
	BOOL GetClipOwner() const;
	void SetClipOwner(BOOL bClipOwner); // <combine CXTPFrameShadow::GetClipOwner>

	//-----------------------------------------------------------------------
	// Summary:
	//     Shows frame shadow if not visible.
	// Remarks:
	//     Note that showing and hiding a shadow is not the same as enabling
	//     and disabling the shadow when the it's actually created and destroyed.
	//     It's just about making an enabled shadow visible.
	// See Also:
	//     Hide
	//-----------------------------------------------------------------------
	void Show();

	//-----------------------------------------------------------------------
	// Summary:
	//     Hides frame shadow if not visible.
	// Remarks:
	//     Note that showing and hiding a shadow is not the same as enabling
	//     and disabling the shadow when the it's actually created and destroyed.
	//     It's just about making an enabled shadow hidden.
	// See Also:
	//     Show
	//-----------------------------------------------------------------------
	void Hide();

	//-----------------------------------------------------------------------
	// Summary:
	//     Updates shadow position and redraws it if necessary.
	//-----------------------------------------------------------------------
	void UpdatePosition();

	//{{AFX_CODEJOCK_PRIVATE
	BOOL IsIconic() const;
	void OnRestore();
	void OnOwnersStyleChanged(DWORD oldStyle, DWORD newStyle);
	//}}AFX_CODEJOCK_PRIVATE

protected:
	DECLARE_MESSAGE_MAP();
	afx_msg void OnPaint();
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);

	virtual void OnFinalRelease();

	virtual void OnFlagsChanged(CXTPFrameShadowManager* pManager, int nOldEffectiveFlags,
								int nNewEffectiveFlags);

	enum HitTestResult
	{
		NoHit,
		HitTop,
		HitTopRight,
		HitRight,
		HitBottomRight,
		HitBottom,
		HitBottomLeft,
		HitLeft,
		HitTopLeft
	};

	void LoadDefaults();
	void ComputeData();
	void SetDefaultBackgroundBlendingOptions();
	void Update(BOOL bReposition, BOOL bRedraw = TRUE);
	BOOL IsSizingAllowed() const;
	CSize GetOwnerMinTrackSize() const;
	CPoint GetDraggingInitialOffset(HitTestResult nHitTest, CPoint ptStart) const;
	HitTestResult HitTest(CPoint point) const;
	void Drag(HitTestResult nHitTest, CPoint ptOffset, CSize szMinSize);
	CRect GetClipRect() const;

	typedef BOOL(WINAPI* PFNUpdateLayeredWindow)(HWND hWnd, HDC hdcDst, POINT* pptDst, SIZE* psize,
												 HDC hdcSrc, POINT* pptSrc, COLORREF crKey,
												 BLENDFUNCTION* pBlend, DWORD dwFlags);

	static PFNUpdateLayeredWindow m_pfnUpdateLayeredWindow;
	static BOOL m_bSupportTested;

	HWND m_hOwner;
	BOOL m_bCreated;
	BOOL m_bVisible;
	UINT m_nOuterSize;
	UINT m_nInnerSize;
	CPoint m_ptOffset;
	COLORREF m_crColor;
	BYTE m_bTransparency;
	BOOL m_bPendingUpdate;
	BOOL m_bIsIconic;
	int m_bFreezeUpdatesCounter;
	BOOL m_bSizingAllowed;
	BOOL m_bClipOwner;
	BOOL m_bBeingUpdated;

	struct BlendingOptions
	{
		CArray<float, const float&> Factors;
		CArray<float, const float&> Positions;
		CArray<float, const float&> CornerFactors;
		CArray<float, const float&> CornerPositions;
		CArray<float, const float&> BackgroundFactors;
		CArray<float, const float&> BackgroundPositions;
	} m_BlendingOptions;

	struct PreComputedData;
	PreComputedData* m_pPCD;

private:
#	ifdef _XTP_ACTIVEX
	//{{AFX_CODEJOCK_PRIVATE

	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
	XTP_DECLARE_CMDTARGETPROVIDER_INTERFACE()

	DECLARE_OLETYPELIB_EX(CXTPFrameShadow);

	afx_msg OLE_HANDLE OleGetOwner();
	afx_msg void OleSetOuterSize(long size);
	afx_msg long OleGetOuterSize();
	afx_msg void OleSetInnerSize(long size);
	afx_msg long OleGetInnerSize();
	afx_msg OLE_XSIZE_PIXELS OleGetOffsetX();
	afx_msg void OleSetOffsetX(OLE_XSIZE_PIXELS offset);
	afx_msg OLE_YSIZE_PIXELS OleGetOffsetY();
	afx_msg void OleSetOffsetY(OLE_YSIZE_PIXELS offset);
	afx_msg OLE_COLOR OleGetColor();
	afx_msg void OleSetColor(OLE_COLOR color);
	afx_msg short OleGetTransparency();
	afx_msg void OleSetTransparency(short val);
	afx_msg BOOL OleGetClipOwner();
	afx_msg void OleSetClipOwner(BOOL bClip);
	afx_msg VARIANT OleGetBlendingFactors();
	afx_msg VARIANT OleGetBlendingPositions();
	afx_msg void OleFreezeUpdates(VARIANT_BOOL bFreeze);
	afx_msg void OleShow();
	afx_msg void OleHide();
	afx_msg void OleUpdatePosition();
	afx_msg void OleSetBlendingOptions(VARIANT factors, VARIANT positions);
	//}}AFX_CODEJOCK_PRIVATE
#	endif
};

AFX_INLINE BOOL CXTPFrameShadow::IsCreated() const
{
	return m_bCreated;
}

AFX_INLINE HWND CXTPFrameShadow::GetOwner() const
{
	return m_hOwner;
}

AFX_INLINE UINT CXTPFrameShadow::GetInnerSize() const
{
	return m_nInnerSize;
}

AFX_INLINE UINT CXTPFrameShadow::GetOuterSize() const
{
	return m_nOuterSize;
}

AFX_INLINE CPoint CXTPFrameShadow::GetOffset() const
{
	return m_ptOffset;
}

AFX_INLINE COLORREF CXTPFrameShadow::GetColor() const
{
	return m_crColor;
}

AFX_INLINE BYTE CXTPFrameShadow::GetTransparency() const
{
	return m_bTransparency;
}

AFX_INLINE BOOL CXTPFrameShadow::IsIconic() const
{
	return m_bIsIconic;
}

AFX_INLINE BOOL CXTPFrameShadow::GetClipOwner() const
{
	return m_bClipOwner;
}

#	include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPFRAMESHADOW_H__)
