// XTPMarkupContext.h: interface for the CXTPMarkupBorder class.
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
#if !defined(__XTPMARKUPCONTEXT_H__)
#	define __XTPMARKUPCONTEXT_H__
//}}AFX_CODEJOCK_PRIVATE

#	if _MSC_VER > 1000
#		pragma once
#	endif // _MSC_VER > 1000

#	include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

class CXTPDpi;
class CXTPMarkupObject;
class CXTPImageManager;
class CXTPMarkupUIElement;
class CXTPMarkupInputElement;
class CXTPMarkupContext;
class CXTPMarkupEventHandlerMap;
class CXTPMarkupRoutedEventArgs;
class CXTPMarkupRoutedEvent;
class CXTPMarkupDelegate;
class CXTPMarkupFont;
class CXTPMarkupType;
class CXTPMarkupKeyboardNavigation;
class CXTPToolTipContext;
class CXTPMarkupParser;
class CXTPMarkupBuilder;
class CXTPActiveScriptEngine;
class CXTPActiveScriptStdRuntime;
class CXTPMarkupDelegateMap;
class CXTPMarkupStaticExtension;
template<class TYPE>
class CXTPMarkupTypedSimpleStack;

enum XTPMarkupSmoothingMode
{
	xtpMarkupSmoothingDefault,
	xtpMarkupSmoothingNone,
	xtpMarkupSmoothingHighSpeed,
	xtpMarkupSmoothingHighQuality,
	xtpMarkupSmoothingAntiAlias,
};

#	ifdef _XTP_ACTIVEX
class _XTP_EXT_CLASS CXTPMarkupContext : public CXTPCmdTarget
#	else

//{{AFX_CODEJOCK_PRIVATE
class CXTPMarkupContextInternalDispatch;
//}}AFX_CODEJOCK_PRIVATE

//===========================================================================
// Summary:
//     CXTPMarkupContext is a standalone class used to provide common
//     settings for Markup objects.
//===========================================================================
class _XTP_EXT_CLASS CXTPMarkupContext
#	endif
{
protected:
	class CInputElementCollection;

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPMarkupContext object.
	//-----------------------------------------------------------------------
	CXTPMarkupContext();

protected:
	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPMarkupContext object, handles cleanup and deallocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPMarkupContext();

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Gets the default font properties of markup objects.
	// Returns:
	//     The default font properties of markup objects.
	//-----------------------------------------------------------------------
	LOGFONT* GetDefaultLogFont();

	//-----------------------------------------------------------------------
	// Summary:
	//     Gets the default text color of TextBlock objects.
	// Returns:
	//     The default text color of TextBlock objects.
	//-----------------------------------------------------------------------
	COLORREF GetDefaultForeground();

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets the default text color for TextBlock objects.
	// Parameters:
	//     clr - New default text color to set.
	//-----------------------------------------------------------------------
	void SetDefaultTextColor(COLORREF clr);

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets the default font for TextBlock objects.
	// Parameters:
	//     font - New default font to set.
	//-----------------------------------------------------------------------
	void SetDefaultFont(HFONT font);
	void SetDefaultFont(LOGFONT* font); // <combine CXTPMarkupContext::SetDefaultFont@HFONT>

	//-----------------------------------------------------------------------
	// Summary:
	//     Gets/sets the default smoothing mode to be used for any drawing
	//     context that uses the context. By default, smoothing mode is
	//     applied to all graphical elements except text. To enable text
	//     smoothing, refer to SetDefaultTextSmoothing.
	// Parameters:
	//     smoothingMode - New smoothing mode to set; must be one of the values
	//                     defined by the XTPMarkupSmoothingMode enumeration.
	// Returns:
	//     Current/previous smoothing mode; returns one of the values defined
	//     by the XTPMarkupSmoothingMode enumeration.
	// See also:
	//      GetDefaultTextSmoothing, SetDefaultTextSmoothing
	//-----------------------------------------------------------------------
	XTPMarkupSmoothingMode GetDefaultSmoothingMode() const;
	XTPMarkupSmoothingMode SetDefaultSmoothingMode(
		XTPMarkupSmoothingMode smoothingMode); // <combine
											   // CXTPMarkupContext::GetDefaultSmoothingMode>

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines/specifies if text smoothing is enabled for any drawing
	//     context that uses the context. By default, smoothing mode is
	//     applied to all graphical elements except text.
	// Parameters:
	//     bEnableTextSmoothing - TRUE to enable text smoothing according to
	//                            the overall smoothing mode.
	// Returns:
	//     Current/previous text smoothing setting. TRUE if text smoothing
	//     is/was enabled, FALSE if text smoothing is/was disabled.
	// See also:
	//     GetDefaultSmoothingMode, SetDefaultSmoothingMode
	//-----------------------------------------------------------------------
	BOOL GetDefaultTextSmoothing() const;
	BOOL SetDefaultTextSmoothing(
		BOOL bEnableTextSmoothing); // <combine CXTPMarkupContext::GetDefaultTextSmoothing>

	//-----------------------------------------------------------------------
	// Summary:
	//     Creates a Markup tree from its XML string.
	// Parameters:
	//     lpszBuffer - XML Markup string.
	// Returns:
	//     The top level Markup element.
	//-----------------------------------------------------------------------
	CXTPMarkupUIElement* Parse(LPCSTR lpszBuffer);
	CXTPMarkupUIElement* Parse(LPCWSTR lpszBuffer); // <combine CXTPMarkupContext::Parse@LPCSTR>

	//-----------------------------------------------------------------------
	// Summary:
	//     Validates XML correctness.
	// Parameters:
	//     lpszBuffer - XML data to parse and validate.
	// Returns:
	//     A validation status message.
	//-----------------------------------------------------------------------
	CString ValidateXML(LPCSTR lpszBuffer);
	CString ValidateXML(LPCWSTR lpszBuffer); // <combine CXTPMarkupContext::ValidateXML@LPCSTR>

	//-----------------------------------------------------------------------
	// Summary:
	//     Connects a Markup event with a class member or static method delegate.
	// Parameters:
	//     pEvent    - Markup event.
	//     pDelegate - Class member or static method delegate.
	// Example:
	// <code>
	// m_wndStatic.AddHandler(CXTPMarkupHyperlink::m_pClickEvent,
	//      CreateMarkupClassDelegate(this, &CMarkupSampleDlg::OnHyperlinkClick));
	// </code>
	//-----------------------------------------------------------------------
	void AddHandler(CXTPMarkupRoutedEvent* pEvent, CXTPMarkupDelegate* pDelegate);

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets a delegate for a Markup text event.
	// Parameters:
	//     lpszDelegate - Text event delegate.
	//     pDelegate    - Method delegate.
	// Example:
	//      <code><TextBlock><Hyperlink Click='OnClick'>FTSE 100</Hyperlink></TextBlock></code>
	//      <code>.... </code>
	//      <code>pPopup->GetMarkupContext()->SetDelegate(L"OnClick",
	//      CreateMarkupFunctionDelegate(&HyperlinkClick));</code>
	//-----------------------------------------------------------------------
	void SetDelegate(LPWSTR lpszDelegate, CXTPMarkupDelegate* pDelegate);

	//-----------------------------------------------------------------------
	// Summary:
	//     Enables/disables GDI+ usage.
	// Parameters:
	//     bEnable - TRUE to enable GDI+ usage, FALSE to disable.
	//-----------------------------------------------------------------------
	void EnableGdiPlus(BOOL bEnable);

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines if GDI+ is enabled/disabled.
	// Returns:
	//     TRUE if GDI+ usage is enabled,
	//     FALSE if GDI+ usage is disabled.
	//-----------------------------------------------------------------------
	BOOL IsGdiPlusEnabled() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to get a pointer to the tooltip context.
	// Returns:
	//     A pointer to the tooltip context.
	//-----------------------------------------------------------------------
	CXTPToolTipContext* GetToolTipContext() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines if custom positioning for tooltips is enabled/disabled.
	// Returns:
	//     TRUE if custom positioning for tooltips is enabled,
	//     FALSE if custom positioning for tooltips is disabled.
	// See also:
	//     SetUseCustomToolTipPosition, GetCustomToolTipPosition
	//-----------------------------------------------------------------------
	BOOL GetUseCustomToolTipPosition() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Enables/disables custom positioning for tooltips.
	// Parameters:
	//     bUse  - TRUE to enable custom positioning for tooltips, FALSE to disable.
	//     ptPos - CPoint that specifies the x- and y- coordinates of the tooltip
	//             window. These coordinates are always relative to the upper-left
	//             corner of the window.
	// Remarks:
	//      This feature can be used before calling CXTPToolTipContext::FilterToolTipMessage
	//      to place a tooltip window in a desirable location.
	// See also:
	//     GetUseCustomToolTipPosition, GetCustomToolTipPosition
	//-----------------------------------------------------------------------
	void SetUseCustomToolTipPosition(BOOL bUse, CPoint ptPos = CPoint(0, 0));

	//-----------------------------------------------------------------------
	// Summary:
	//     Gets the custom x- and y- coordinates of the tooltip window. These
	//     coordinates are always relative to the upper-left corner of the window.
	// Returns:
	//     A CPoint object containing the custom x- and y- coordinates of the tooltip window.
	// See also:
	//     GetUseCustomToolTipPosition, SetUseCustomToolTipPosition
	//-----------------------------------------------------------------------
	CPoint GetCustomToolTipPosition() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets the isolated image manager for elements inside the Markup context.
	// Parameters:
	//     pImageManager - New image manager to use.
	// Example:
	//     <code>m_pMarkupContext->SetImageManager(new CXTPImageManager());</code>
	//-----------------------------------------------------------------------
	void SetImageManager(CXTPImageManager* pImageManager);

	//-----------------------------------------------------------------------
	// Summary:
	//     Gets the isolated image manager for elements inside the Markup context.
	// Returns:
	//     The isolated image manager for elements inside the Markup context.
	// Remarks:
	//     Function returns XTPImageManager(); by default, call SetImageManager to
	//     set isolated images.
	//-----------------------------------------------------------------------
	CXTPImageManager* GetImageManager() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets the context specific color key for an image element.
	// Parameters:
	//     crColorKey - New color key value, or COLORREF_NULL.
	// Remarks:
	//     If the context specific color key is set to COLORREF_NULL, then
	//     the color mask of the assigned image manager will be used in
	//     place of the context specific color key. If the color mask of the
	//     assigned image manager is also COLORREF_NULL, then a default
	//     value of RGB(0, 0xff, 0) will be used instead.
	//-----------------------------------------------------------------------
	void SetImageColorKey(COLORREF crColorKey);

	//-----------------------------------------------------------------------
	// Summary:
	//     Gets the context specific color key for an image element.
	// Returns:
	//     If the context specific color key is not COLORREF_NULL, then the
	//     context specific color key will be returned. If the context specific
	//     color key is COLORREF_NULL, then the color mask of the assigned
	//     image manager will be returned. If the color mask of the assigned
	//     image manager is also COLORREF_NULL, then a default value of
	//     RGB(0, 0xff, 0) will be returned instead.
	//-----------------------------------------------------------------------
	COLORREF GetImageColorKey() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Gets a pointer to the extension root object used to add custom
	//     Markup extensions.
	// Returns:
	//     A pointer to the extension root object.
	//-----------------------------------------------------------------------
	CXTPMarkupStaticExtension* GetExtensionRoot();

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines if context DPI awareness is enabled
	//     (i.e. graphics are scaled according to active DPI settings).
	// Returns:
	//     TRUE if context DPI awareness is enabled.
	//-----------------------------------------------------------------------
	BOOL IsDpiAware() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Enables/disables context DPI awareness.
	// Parameters:
	//     bSet - TRUE to enable DPI awareness
	//            (i.e. make graphics scale according to active DPI settings).
	//     hWnd - If specified, then the window's device context information
	//            will be used for DPI translation.
	//     hDC  - If specified, then the device context information will be
	//            used for DPI translation.
	//-----------------------------------------------------------------------
	void SetDpiAware(BOOL bSet = TRUE);
	void SetDpiAware(BOOL bSet, HWND hWnd); // <combine CXTPMarkupContext::SetDpiAware@BOOL>
	void SetDpiAware(BOOL bSet, HDC hDC);	// <combine CXTPMarkupContext::SetDpiAware@BOOL>

	//-----------------------------------------------------------------------
	// Summary:
	//     Performs x- and y- coordinate scaling according to active DPI settings
	//     if the Markup context is DPI aware.
	// Parameters:
	//     x, y - Coordinates to scale according to active DPI settings.
	// Returns:
	//     If the Markup context is DPI aware, then a scaled value is returned.
	//     If the Markup context is not DPI aware, then an unchanged value is returned.
	// See also:
	//     SetDpiAware
	//-----------------------------------------------------------------------
	int ScaleX(int x) const;
	int ScaleY(int y) const;			 // <combine CXTPMarkupContext::ScaleX@int>
	float ScaleX(float x) const;		 // <combine CXTPMarkupContext::ScaleX@int>
	float ScaleY(float y) const;		 // <combine CXTPMarkupContext::ScaleX@int>
	double ScaleX(double x) const;		 // <combine CXTPMarkupContext::ScaleX@int>
	double ScaleY(double y) const;		 // <combine CXTPMarkupContext::ScaleX@int>
	CSize Scale(const SIZE& size) const; // <combine CXTPMarkupContext::ScaleX@int>

	//-----------------------------------------------------------------------
	// Summary:
	//     Loads a chunk of script code in a specified language.
	// Parameters:
	//     pBuilder     - Pointer to the Markup builder from which code is loaded.
	//     lpszLanguage - Script code language.
	//     lpszCode     - Script code.
	//-----------------------------------------------------------------------
	void LoadScriptCodeChunk(CXTPMarkupBuilder* pBuilder, LPCWSTR lpszLanguage, LPCWSTR lpszCode);

	//-----------------------------------------------------------------------
	// Summary:
	//     Runs script engine.
	// Parameters:
	//     pBuilder - Pointer to the Markup builder that runs script engine.
	//-----------------------------------------------------------------------
	void RunScriptEngine(CXTPMarkupBuilder* pBuilder);

	//{{AFX_CODEJOCK_PRIVATE
	// Internal methods
	void ResetScriptEngine();
	void RegisterScriptNamedObjects(CXTPMarkupBuilder* pBuilder, CXTPMarkupObject* pObject);
	void ThrowScriptEngineException(CXTPMarkupBuilder* pBuilder, HRESULT hr);

	CXTPMarkupFont* GetFont(LOGFONT* pLogFont);

	static BOOL AFX_CDECL CompareFont(LOGFONT* pLogFont1, LOGFONT* pLogFont2);

	virtual BOOL OnWndMsg(CXTPMarkupUIElement* pUIElement, UINT message, WPARAM wParam,
						  LPARAM lParam, LRESULT* pResult);

	void RaiseEvent(CXTPMarkupObject* pSource, CXTPMarkupRoutedEventArgs* pEventArgs);

	CXTPMarkupDelegate* LookupDelegate(LPCWSTR lpszDelegate) const;

	void CaptureMouse(CXTPMarkupInputElement* pUIElement);
	void ReleaseMouseCapture(CXTPMarkupInputElement* pUIElement);
	CXTPMarkupInputElement* GetMouseCapture() const;

	CXTPMarkupKeyboardNavigation* GetKeyboardNavigation() const;

	void AddRef();
	void Release();
	INT_PTR OnToolHitTest(CPoint point, TOOLINFO* pToolInfo);

public:
	CRect GetClientBoundingRect(CXTPMarkupInputElement* pUIElement) const;
	CRect GetUpdateRect(CXTPMarkupInputElement* pUIElement) const;
	BOOL IsVisualChild(CXTPMarkupObject* pParent, CXTPMarkupObject* pChild) const;

private:
	void HandleMouseMove(CXTPMarkupUIElement* pUIElement, CPoint point);
	void HandleMouseEnter(CXTPMarkupInputElement* pUIElement, CPoint point);
	BOOL HandleSetCursor();
	BOOL HandleMouseWheel(int nDelta);
	BOOL HandleMouseUpDown(UINT message, WPARAM wParam, LPARAM lParam);

	void BuildInputList(CXTPMarkupObject* pUIElement, CInputElementCollection* list);

	void Cleanup();

	void AddPendingUpdate(CXTPMarkupUIElement* pUIElement);

public:
	virtual void OnInvalidateVisual(CXTPMarkupUIElement* pUIElement);
	virtual void OnInvalidateArrange(CXTPMarkupUIElement* pUIElement);

public:
	CXTPMarkupObject* CreateMarkupObject(CXTPMarkupType* pRuntimeClass);
	void FinalizeMarkupObject(CXTPMarkupObject* pObject);
	static void AFX_CDECL RegisterClasses();

protected:
	virtual void GetScriptNamedItemNames(CList<LPCWSTR, LPCWSTR>& names);
	virtual HRESULT GetScriptNamedItem(LPCWSTR lpszName, IUnknown** ppiunkItem);

public:
	HWND m_hContextWnd;

protected:
	COLORREF m_clrForeground;
	COLORREF m_clrImgColorKey;
	LOGFONT m_logFont;
	XTPMarkupSmoothingMode m_DefaultSmoothingMode;
	BOOL m_bDefaultTextSmoothing;

	CXTPMarkupInputElement* m_pMouseOver;
	CXTPMarkupInputElement* m_pMouseCapture;
	CXTPMarkupUIElement* m_pActiveElement;

	CXTPMarkupTypedSimpleStack<CXTPMarkupFont>* m_pFonts;

	CXTPMarkupEventHandlerMap* m_pHandlers;
	CXTPMarkupDelegateMap* m_pDelegates;

	CXTPMarkupKeyboardNavigation* m_pKeyboardNavigation;

	CXTPToolTipContext* m_pToolTipContext;
	BOOL m_bUseCustomToolTipPos;
	CPoint m_ptCustomToolTipPos;

	friend class CXTPMarkupFont;
	class CTooltipContext;
	long m_dwRef;

	CXTPImageManager* m_pImageManager;
	CXTPMarkupStaticExtension* m_pExtensionRoot;

	LPWSTR m_lpszScriptEngineLanguage;
	CXTPActiveScriptEngine* m_pScriptEngine;
	CList<LPWSTR, LPWSTR> m_ScriptCodeChunks;
	CXTPActiveScriptStdRuntime* m_pScriptEngineStdRuntime;

	BOOL m_bEnableGdiPlus;
	CXTPDpi* m_pDpi;
	BOOL m_bUpdatesFrozen;
	BOOL m_bEntireUpdatePended;
	CList<CXTPMarkupUIElement*, CXTPMarkupUIElement*> m_PendingUpdateList;

protected:
#	ifdef _XTP_ACTIVEX
	DECLARE_INTERFACE_MAP()
#	endif

#	pragma warning(push)
// C4616: #pragma warning : warning number '...' out of range, must be between '4001' and '4999'
// C4619: #pragma warning : there is no warning number 'number'
// C5204: '...': class has virtual functions, but its trivial destructor is not virtual; instances
// of objects derived from this class may not be destructed correctly
#	pragma warning(disable : 4616 4619 5204)
	BEGIN_INTERFACE_PART(ActiveScriptSite, IActiveScriptSite)
	STDMETHOD(GetLCID)(LCID* plcid);
	STDMETHOD(GetItemInfo)
	(LPCOLESTR pstrName, DWORD dwReturnMask, IUnknown** ppiunkItem, ITypeInfo** ppti);
	STDMETHOD(GetDocVersionString)(BSTR* pbstrVersion);
	STDMETHOD(OnScriptTerminate)(const VARIANT* pvarResult, const EXCEPINFO* pexcepinfo);
	STDMETHOD(OnStateChange)(SCRIPTSTATE ssScriptState);
	STDMETHOD(OnScriptError)(IActiveScriptError* pScriptError);
	STDMETHOD(OnEnterScript)();
	STDMETHOD(OnLeaveScript)();

#	ifndef _XTP_ACTIVEX
	CXTPMarkupContext* pThis;
#	endif
	END_INTERFACE_PART(ActiveScriptSite);
#	pragma warning(pop)

#	ifndef _XTP_ACTIVEX
	friend class XActiveScriptSite;
#	endif

#	ifdef _XTP_ACTIVEX
public:
	DECLARE_DISPATCH_MAP()

	DECLARE_OLETYPELIB_EX(CXTPMarkupContext);

#	else
protected:
	friend class CXTPMarkupContextInternalDispatch;
	CCmdTarget* m_pInternalDispatch;
#	endif

	CCmdTarget* AccessInternalDispatch();

protected:
	void OleSetMethod(LPDISPATCH lpHandler, LPCOLESTR lpName);
	void OleSetHandler(LPDISPATCH lpHandler);

	LPDISPATCH OleCreateObject(LPCOLESTR lpName);
	LPDISPATCH OleCreateSolidBrush(COLORREF clr);
	LPDISPATCH OleCreateThickness(long nLeft, long nTop, long nRight, long nBottom);
	LPDISPATCH OleParse(LPCTSTR lpText);
	BOOL OleIsDpiAware();
	void OleSetDpiAware(BOOL bNewValue);
	LPDISPATCH OleGetToolTipContext();
	void OleSetIcons(LPDISPATCH lpDisp);
	LPDISPATCH OleGetIcons();

	BSTR OleValidateXML(LPCTSTR lpText);

	void OleFreezeUpdates();
	void OleUnfreezeUpdates();

	long OleGetDefaultSmoothingMode();
	void OleSetDefaultSmoothingMode(long lMode);

	BOOL OleGetDefaultTextSmoothing();
	void OleSetDefaultTextSmoothing(BOOL bEnableTextSmoothing);

	//}}AFX_CODEJOCK_PRIVATE

	friend class CXTPMarkupInputElement;
};

AFX_INLINE LOGFONT* CXTPMarkupContext::GetDefaultLogFont()
{
	return &m_logFont;
}

AFX_INLINE COLORREF CXTPMarkupContext::GetDefaultForeground()
{
	return m_clrForeground;
}

AFX_INLINE void CXTPMarkupContext::SetDefaultTextColor(COLORREF clr)
{
	m_clrForeground = clr;
}

AFX_INLINE void CXTPMarkupContext::SetDefaultFont(HFONT font)
{
	LOGFONT lf;
	GetObject(font, sizeof(LOGFONT), &lf);
	m_logFont = lf;
}

AFX_INLINE void CXTPMarkupContext::SetDefaultFont(LOGFONT* pLogFont)
{
	m_logFont = *pLogFont;
}

AFX_INLINE XTPMarkupSmoothingMode CXTPMarkupContext::GetDefaultSmoothingMode() const
{
	return m_DefaultSmoothingMode;
}

AFX_INLINE XTPMarkupSmoothingMode
	CXTPMarkupContext::SetDefaultSmoothingMode(XTPMarkupSmoothingMode smoothingMode)
{
	XTPMarkupSmoothingMode oldMode = m_DefaultSmoothingMode;
	m_DefaultSmoothingMode		   = smoothingMode;
	return oldMode;
}

AFX_INLINE BOOL CXTPMarkupContext::GetDefaultTextSmoothing() const
{
	return m_bDefaultTextSmoothing;
}

AFX_INLINE BOOL CXTPMarkupContext::SetDefaultTextSmoothing(BOOL bEnableTextSmoothing)
{
	BOOL bOldState			= m_bDefaultTextSmoothing;
	m_bDefaultTextSmoothing = bEnableTextSmoothing;
	return bOldState;
}

AFX_INLINE void CXTPMarkupContext::SetImageColorKey(COLORREF crColorKey)
{
	m_clrImgColorKey = crColorKey;
}

AFX_INLINE CXTPMarkupKeyboardNavigation* CXTPMarkupContext::GetKeyboardNavigation() const
{
	return m_pKeyboardNavigation;
}

AFX_INLINE CXTPMarkupInputElement* CXTPMarkupContext::GetMouseCapture() const
{
	return m_pMouseCapture;
}

AFX_INLINE BOOL CXTPMarkupContext::IsGdiPlusEnabled() const
{
	return m_bEnableGdiPlus;
}

AFX_INLINE void CXTPMarkupContext::EnableGdiPlus(BOOL bEnable)
{
	m_bEnableGdiPlus = bEnable;
}

AFX_INLINE CXTPToolTipContext* CXTPMarkupContext::GetToolTipContext() const
{
	return m_pToolTipContext;
}

AFX_INLINE BOOL CXTPMarkupContext::GetUseCustomToolTipPosition() const
{
	return m_bUseCustomToolTipPos;
}

AFX_INLINE void CXTPMarkupContext::SetUseCustomToolTipPosition(BOOL bUse,
															   CPoint ptPos /*= CPoint(0, 0)*/)
{
	m_bUseCustomToolTipPos = bUse;
	m_ptCustomToolTipPos   = ptPos;
}

AFX_INLINE CPoint CXTPMarkupContext::GetCustomToolTipPosition() const
{
	return m_ptCustomToolTipPos;
}

AFX_INLINE BOOL CXTPMarkupContext::IsDpiAware() const
{
	return NULL != m_pDpi;
}

AFX_INLINE void CXTPMarkupContext::SetDpiAware(BOOL bSet /*= TRUE*/)
{
	SetDpiAware(bSet, HWND_DESKTOP);
}

#	define MARKUP_CREATE(object_class, context)                                                   \
		((object_class*)context->CreateMarkupObject(MARKUP_TYPE(object_class)))

#	include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPMARKUPCONTEXT_H__)
