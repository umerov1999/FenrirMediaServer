// XTPSkinObjectFrame.h: interface for the CXTPSkinObjectFrame class.
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
#if !defined(__XTPSKINOBJECTFRAME_H__)
#	define __XTPSKINOBJECTFRAME_H__
//}}AFX_CODEJOCK_PRIVATE

#	if _MSC_VER > 1000
#		pragma once
#	endif // _MSC_VER > 1000

#	include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

//{{AFX_CODEJOCK_PRIVATE
#	ifndef WM_NCMOUSELEAVE
#		define WM_NCMOUSELEAVE 0x02A2
#	endif

#	ifndef WM_QUERYUISTATE
#		define WM_UPDATEUISTATE 0x0128
#		define WM_QUERYUISTATE 0x0129

#		define UISF_HIDEACCEL 0x2
#		define UISF_HIDEFOCUS 0x1
#	endif

#	define XTP_TID_MOUSELEAVE 0xACB
#	define XTP_TID_REFRESHFRAME 0xACD
#	define XTP_REFRESHFRAME_AFFECTED_STYLES WS_DLGFRAME
#	define XTP_REFRESHFRAME_DURATION 1000

struct _XTP_EXT_CLASS XTP_SKINSCROLLBAR_POSINFO : public XTP_SCROLLBAR_POSINFO
{
	int nBar;
	BOOL fVisible;
};

struct _XTP_EXT_CLASS XTP_SKINSCROLLBAR_TRACKINFO : public XTP_SCROLLBAR_TRACKINFO
{
	BOOL fTrackVert;
	HWND hWndSB;
	HWND hWndSBNotify;
	int nBar;
	BOOL fNonClient;
	XTP_SKINSCROLLBAR_POSINFO* pSBInfo; // Overwrite XTP_SCROLLBAR_TRACKINFO::pSBInfo
};

//}}AFX_CODEJOCK_PRIVATE

//===========================================================================
// Summary:
//     CXTPSkinObjectFrame is a CXTPSkinObject derived class. It is the
//     parent control for all implementations that have borders or scrollbars.
//===========================================================================
class _XTP_EXT_CLASS CXTPSkinObjectFrame : public CXTPSkinObject
{
	DECLARE_DYNCREATE(CXTPSkinObjectFrame)
public:
	//===========================================================================
	// Summary:
	//     Caption button class.
	//===========================================================================
	class _XTP_EXT_CLASS CCaptionButton
	{
	public:
		//-----------------------------------------------------------------------
		// Summary:
		//     Constructs a CCaptionButton object.
		// Parameters:
		//     nCommand     - Command ID of the button.
		//     pFrame       - Pointer to the parent frame object.
		//     nHTCode      - Hit test code of the button.
		//     m_nClassPart - Class part.
		//-----------------------------------------------------------------------
		CCaptionButton(int nCommand, CXTPSkinObjectFrame* pFrame, UINT nHTCode, int m_nClassPart);

		//-----------------------------------------------------------------------
		// Summary:
		//     Destroys a CCaptionButton object.
		//-----------------------------------------------------------------------
		virtual ~CCaptionButton();

		//-----------------------------------------------------------------------
		// Summary:
		//     This method is called to draw the caption button.
		// Parameters:
		//     pDC          - Pointer to the device context.
		//     bFrameActive - TRUE to set the frame to active.
		//-----------------------------------------------------------------------
		void Draw(CDC* pDC, BOOL bFrameActive);

		//-----------------------------------------------------------------------
		// Summary:
		//     Determines the pressed state of the button.
		// Returns:
		//     TRUE if the button is pressed, otherwise FALSE.
		//-----------------------------------------------------------------------
		BOOL IsPressed() const;

		//-----------------------------------------------------------------------
		// Summary:
		//     Determines the highlighted state of the button.
		// Returns:
		//     TRUE if the button is highlighted, otherwise FALSE.
		//-----------------------------------------------------------------------
		BOOL IsHighlighted() const;

	public:
		UINT m_nHTCode;				   // Hit test code.
		CRect m_rcButton;			   // Button bounding rectangle.
		int m_nClassPart;			   // Class part.
		CXTPSkinObjectFrame* m_pFrame; // Parent frame object.
		int m_nCommand;				   // Button Command.
		BOOL m_bEnabled;
	};

	typedef CArray<CCaptionButton*, CCaptionButton*> CCaptionButtons;

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPSkinObjectFrame object.
	//-----------------------------------------------------------------------
	CXTPSkinObjectFrame();

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPSkinObjectFrame object, handles cleanup and deallocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPSkinObjectFrame();

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Redraws the scrollbar of the frame.
	// Parameters:
	//     nBar - Orientation of the scrollbar;
	//            1 for vertical, 0 for horizontal.
	//-----------------------------------------------------------------------
	void RedrawScrollBar(int nBar);

	//-----------------------------------------------------------------------
	// Summary:
	//     Gets the borders of the window.
	// Returns:
	//     The borders of the window.
	//-----------------------------------------------------------------------
	CRect GetBorders() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines if the caption is active.
	// Returns:
	//     TRUE if the caption is active, otherwise FALSE.
	//-----------------------------------------------------------------------
	BOOL IsActive() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Gets the collection of caption buttons.
	// Returns:
	//     A pointer to the collection of caption buttons.
	//-----------------------------------------------------------------------
	CCaptionButtons* GetCaptionButtons();

	//-----------------------------------------------------------------------
	// Summary:
	//     Gets the client brush.
	// Parameters:
	//     pDC - Pointer to the device context.
	// Returns:
	//     The client brush.
	//-----------------------------------------------------------------------
	virtual HBRUSH GetClientBrush(CDC* pDC);

public:
	//{{AFX_CODEJOCK_PRIVATE
	BOOL IsFrameScrollBars();
	HBRUSH GetClientBrush(HDC hDC, HWND hWnd, UINT nCtlColor);
	XTP_SKINSCROLLBAR_TRACKINFO* GetScrollBarTrackInfo() const;

	BOOL DrawMenuBar();
	virtual void UpdateMenuBar();
	//}}AFX_CODEJOCK_PRIVATE

protected:
	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called when the skin is changed.
	// Parameters:
	//     bPrevState - TRUE if the previous skin was installed.
	//     bNewState  - TRUE if the current skin is installed.
	//-----------------------------------------------------------------------
	virtual void OnSkinChanged(BOOL bPrevState, BOOL bNewState);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called when visual metrics are changed.
	//-----------------------------------------------------------------------
	virtual void RefreshMetrics();

	// ---------------------------------------------------------------------
	// Summary:
	//     This method is called when the hook is attached.
	// Parameters:
	//     lpcs  - Pointer to a LPCREATESTRUCT structure that contains
	//             information about the CWnd object being created.
	//     bAuto - TRUE to automatically skin the window when a new
	//             window is created or when the user manually skins
	//             the existing window.
	// ---------------------------------------------------------------------
	virtual void OnHookAttached(LPCREATESTRUCT lpcs, BOOL bAuto);

	// ---------------------------------------------------------------------
	// Summary:
	//     This method is called when the hook is detached.
	// Parameters:
	//     bAuto - TRUE to automatically skin the window when a new
	//             window is created or when the user manually skins
	//             the existing window.
	// ---------------------------------------------------------------------
	void OnHookDetached(BOOL bAuto);

	//{{AFX_CODEJOCK_PRIVATE
public:
	void RedrawFrame();
	BOOL HasCaption() const;
	BOOL HasWindowBorder() const;
	virtual int GetClientBrushMessage();

protected:
	void UpdateFrameRegion(CSize szFrameRegion);
	void UpdateButtons();
	void InvalidateButtons();
	CCaptionButton* HitTestButton(CPoint pt);
	void UpdateButton(int nID, BOOL bVisible, BOOL bEnabled, UINT htCode, int nClassPart);
	void RemoveButtons();
	void RefreshFrameStyle();
	HBRUSH FillBackground(CDC* pDC, LPCRECT lprc, int nMessage = WM_CTLCOLORSTATIC);
	HBRUSH GetFillBackgroundBrush(CDC* pDC, int nMessage);
	void ResizeFrame(BOOL bUpdateRegion = TRUE);

protected:
	virtual void DrawFrame(CDC* pDC);
	void ScreenToFrame(LPPOINT lpPoint);
	void ClientToFrame(LPPOINT lpPoint);
	void AdjustFrame(CRect& rc);
	BOOL OnHookMessage(UINT nMessage, WPARAM& wParam, LPARAM& lParam, LRESULT& lResult);
	BOOL OnHookDefWindowProc(UINT nMessage, WPARAM& wParam, LPARAM& lParam, LRESULT& lResult);

	using CXTPSkinObject::OnCtlColor;
	BOOL OnCtlColor(UINT nMessage, WPARAM& wParam, LPARAM& lParam, LRESULT& lResult,
					BOOL bConsiderDlgTexture = FALSE);

protected:
	void DrawScrollBar(CDC* pDC, XTP_SKINSCROLLBAR_POSINFO* pSBInfo);
	void TrackInit(CPoint point, XTP_SKINSCROLLBAR_POSINFO* pSBInfo, BOOL bDirect);
	void ContScroll();
	void DoScroll(HWND hwnd, HWND hwndNotify, int cmd, int pos, BOOL fVert);
	void TrackBox(UINT message, CPoint point);
	void TrackThumb(UINT message, CPoint point);
	void MoveThumb(int px);
	int HitTestScrollBar(XTP_SKINSCROLLBAR_POSINFO* pSBInfo, POINT pt);
	void EndScroll(BOOL fCancel);
	void CalcScrollBarInfo(LPRECT lprc, XTP_SKINSCROLLBAR_POSINFO* pSBInfo, SCROLLINFO* pSI);
	void RedrawScrollBar(XTP_SKINSCROLLBAR_POSINFO* pSBInfo);
	void SetupScrollInfo(XTP_SKINSCROLLBAR_POSINFO* pSBInfo);
	CRect GetScrollBarRect(int sbCode);
	void TrackCaptionButton();
	BOOL IsSizeBox();
	virtual BOOL IsDefWindowProcAvail(int nMessage) const;
	BOOL PreHookMessage(UINT nMessage);
	BOOL IsFlatScrollBarInitialized() const;
	HWND FindMDIClient();
	virtual BOOL HandleSysCommand(UINT nID, LPARAM lParam);
	//}}AFX_CODEJOCK_PRIVATE

protected:
	//{{AFX_CODEJOCK_PRIVATE
	DECLARE_MESSAGE_MAP()

	//{{AFX_VIRTUAL(CXTPSkinObjectFrame)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CXTPSkinObjectFrame)
	afx_msg void OnNcPaint();
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp);
	afx_msg void OnNcMouseMove(UINT nHitTest, CPoint point);
	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg void OnNcLButtonDown(UINT nHitTest, CPoint point);
	afx_msg void OnNcRButtonUp(UINT nHitTest, CPoint point);
	afx_msg void OnNcLButtonDblClk(UINT nHitTest, CPoint point);
	afx_msg void HandleTimer(UINT_PTR nIDEvent);
	afx_msg void OnNcMouseLeave();
	afx_msg LRESULT OnSetText(WPARAM wParam, LPARAM lParam);
	afx_msg BOOL OnNcActivate(BOOL bActive);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnWindowPosChanging(WINDOWPOS FAR* lpwndpos);
	afx_msg void OnPaint();
	afx_msg LRESULT OnPrint(WPARAM, LPARAM);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	afx_msg void OnStyleChanged(int nStyleType, LPSTYLESTRUCT lpStyleStruct);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSysKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual BOOL HandleMouseMove(CPoint point);
	virtual LRESULT HandleNcHitTest(CPoint point);
	afx_msg void OnWindowPosChanged(WINDOWPOS FAR* lpwndpos);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	LRESULT CallDefDlgProc(UINT nMessage, WPARAM wParam, LPARAM lParam);
	static void CALLBACK OnTimerInternal(HWND hWnd, UINT nMsg, UINT_PTR nIDEvent, DWORD dwTime);
	afx_msg LRESULT OnSetIcon(WPARAM, LPARAM);
	//}}AFX_MSG
	//}}AFX_CODEJOCK_PRIVATE

protected:
	void CancelMouseLeaveTracking();
	HWND GetTopLevelWindow() const;

private:
	void RefreshScrollBarsData();

public:
	BOOL m_bLockFrameDraw;   // TRUE to lock draw.
	DWORD m_dwDialogTexture; // Dialog texture.
	CXTPSkinObjectFrame* m_pMDIClient;

protected:
	CCaptionButtons m_arrButtons;			 // Caption buttons.
	CRect m_rcBorders;						 // Frame borders.
	CCaptionButton* m_pButtonHot;			 // Hot caption button.
	CCaptionButton* m_pButtonPressed;		 // Pressed caption button.
	BOOL m_bRegionChanged;					 // TRUE if region was changed.
	XTP_SKINSCROLLBAR_TRACKINFO* m_pSBTrack; // Track info of scrollbars.
	XTP_SKINSCROLLBAR_POSINFO m_spi[2];		 // Scrollbar position info.
	CSize m_szFrameRegion;					 // Previous frame region.
	BOOL m_bMDIClient;						 // TRUE if the frame is a MDI client.
	BOOL m_bActive;							 // TRUE if the caption is active.
	DWORD m_dwStyle;						 // Styles of window.
	DWORD m_dwExStyle;						 // ExStyles of window.

	BOOL m_bInUpdateRegion;
	BOOL m_bInWindowPosChanged;

	int m_nCtlColorMessage;
	static CXTPThreadLocal<BOOL> m_bMenuStatus;

private:
	friend class CXTPSkinManager;
	friend class CCaptionButton;
};

AFX_INLINE BOOL CXTPSkinObjectFrame::IsActive() const
{
	return m_bActive;
}

AFX_INLINE CRect CXTPSkinObjectFrame::GetBorders() const
{
	return m_rcBorders;
}

AFX_INLINE XTP_SKINSCROLLBAR_TRACKINFO* CXTPSkinObjectFrame::GetScrollBarTrackInfo() const
{
	return m_pSBTrack;
}

AFX_INLINE CXTPSkinObjectFrame::CCaptionButtons* CXTPSkinObjectFrame::GetCaptionButtons()
{
	return &m_arrButtons;
}
AFX_INLINE void CXTPSkinObjectFrame::UpdateMenuBar()
{
}

//===========================================================================
// Summary:
//     CXTPSkinObjectUser32Control is a CXTPSkinObjectFrame derived class.
//     It is the base class for all User32 defined controls.
//===========================================================================
class _XTP_EXT_CLASS CXTPSkinObjectUser32Control : public CXTPSkinObjectFrame
{
protected:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPSkinObjectUser32Control object.
	//-----------------------------------------------------------------------
	CXTPSkinObjectUser32Control();

protected:
	//-----------------------------------------------------------------------
	// Summary:
	//     Determines if the scrollbar position was changed.
	//-----------------------------------------------------------------------
	virtual void CheckScrollBarsDraw();

protected:
	//{{AFX_CODEJOCK_PRIVATE
	DECLARE_MESSAGE_MAP()

	// Overrides
	//{{AFX_VIRTUAL(CXTPSkinObjectUser32Control)
	BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CXTPSkinObjectUser32Control)
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnWindowPosChanged(WINDOWPOS FAR* lpwndpos);
	//}}AFX_MSG

protected:
	SCROLLINFO m_si[2];
	//}}AFX_CODEJOCK_PRIVATE
};

//===========================================================================
// Summary:
//     CXTPSkinObjectComCtl32Control is a CXTPSkinObjectUser32Control derived class.
//     It is the base class for all ComCtl32 defined controls.
//===========================================================================
class _XTP_EXT_CLASS CXTPSkinObjectComCtl32Control : public CXTPSkinObjectUser32Control
{
protected:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPSkinObjectComCtl32Control object.
	//-----------------------------------------------------------------------
	CXTPSkinObjectComCtl32Control();

protected:
	//-----------------------------------------------------------------------
	// Summary:
	//     Determines if the scrollbar position was changed.
	//-----------------------------------------------------------------------
	virtual void CheckScrollBarsDraw();
};

//===========================================================================
// Summary:
//     CXTPSkinObjectSolidFilled is a CXTPSkinObjectFrame derived class.
//     It is a helper class for unknown window classes that must be filled
//     with a solid color or brush.
//===========================================================================
class _XTP_EXT_CLASS CXTPSkinObjectSolidFilled : public CXTPSkinObjectFrame
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPSkinObjectSolidFilled object.
	// Parameters:
	//     crFill - Color to fill the window background with.
	//-----------------------------------------------------------------------
	explicit CXTPSkinObjectSolidFilled(COLORREF crFill);

	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPSkinObjectSolidFilled object.
	// Parameters:
	//     hbrFill - Handle of a brush to fill the window background with.
	//-----------------------------------------------------------------------
	explicit CXTPSkinObjectSolidFilled(HBRUSH hbrFill);

protected:
	//{{AFX_CODEJOCK_PRIVATE
	DECLARE_MESSAGE_MAP()

	//{{AFX_MSG(CXTPSkinObjectUser32Control)
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG

	//}}AFX_CODEJOCK_PRIVATE
};

#	include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPSKINOBJECTFRAME_H__)
