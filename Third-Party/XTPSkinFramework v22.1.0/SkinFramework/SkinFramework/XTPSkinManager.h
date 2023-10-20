// XTPSkinManager.h: interface for the CXTPSkinManager class.
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
#if !defined(__XTPSKINMANAGER_H__)
#	define __XTPSKINMANAGER_H__
//}}AFX_CODEJOCK_PRIVATE

#	if _MSC_VER > 1000
#		pragma once
#	endif // _MSC_VER > 1000

#	include "Common/Base/Diagnostic/XTPDisableAdvancedWarnings.h"
#	include <comdef.h>
#	include "Common/Base/Diagnostic/XTPEnableAdvancedWarnings.h"

#	include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

#include <vector>

class CXTPSkinManagerSchemaProperty;
class CXTPSkinManagerResourceFile;
class CXTPSkinManagerColorFilter;
class CXTPSkinObjectClassMap;
class CXTPSkinManagerApiHookBase;
class CXTPSkinManagerModuleListSharedData;
class CXTPSkinManagerSchemaCodeManager;
typedef HANDLE HTHEME;

//-----------------------------------------------------------------------
// Summary:
//     Applied options of the selected skin.
// Example:
// <code>
// XTPSkinManager()->SetApplyOptions(xtpSkinApplyMetrics | xtpSkinApplyFrame | xtpSkinApplyColors);
// </code>
// See Also:
//     CXTPSkinManager, CXTPSkinManager::SetApplyOptions
//
// <KEYWORDS xtpSkinApplyMetrics, xtpSkinApplyFrame, xtpSkinApplyColors>
//-----------------------------------------------------------------------
enum XTPSkinApplyOptions
{
	xtpSkinApplyMetrics = 1, // TRUE to use skin provided metrics.
	xtpSkinApplyFrame   = 2, // TRUE to apply frame skins.
	xtpSkinApplyColors  = 4, // TRUE to use skin provided colors
	xtpSkinApplyMenus   = 8  // TRUE to apply skin for pop-up menus.
};

//-----------------------------------------------------------------------
// Summary:
//     Available properties for the CXTPSkinManagerSchemaProperty class.
// Example:
// <code>
// XTPSkinManager()->GetProperty(XTP_SKINPROPERTY_BOOL, iPartId, iStateId, TMT_TRANSPARENT);
// </code>
// See Also:
//     CXTPSkinManagerSchemaProperty
//-----------------------------------------------------------------------
enum XTPSkinManagerProperty
{
	XTP_SKINPROPERTY_UNKNOWN,							 // Not defined.
	XTP_SKINPROPERTY_STRING,							 // CString property.
	XTP_SKINPROPERTY_INT,								 // int property.
	XTP_SKINPROPERTY_BOOL,								 // BOOL property.
	XTP_SKINPROPERTY_COLOR,								 // COLORREF property.
	XTP_SKINPROPERTY_RECT,								 // CRect property.
	XTP_SKINPROPERTY_FONT,								 // LOGFONT property.
	XTP_SKINPROPERTY_INTLIST,							 // CIntArray property.
	XTP_SKINPROPERTY_ENUM,								 // enum property.
	XTP_SKINPROPERTY_POSITION,							 // int property.
	XTP_SKINPROPERTY_FILENAME = XTP_SKINPROPERTY_STRING, // FileName property.
	XTP_SKINPROPERTY_MARGINS  = XTP_SKINPROPERTY_RECT,   // Margins property.
	XTP_SKINPROPERTY_SIZE	 = XTP_SKINPROPERTY_INT	 // Size property.
};

class CXTPSkinManager;
class CXTPSkinManagerSchema;
class CXTPSkinImages;
class CXTPSkinObject;

//===========================================================================
// Summary:
//     CXTPSkinManagerClass is a standalone class that represents a
//     single visual class of the skin manager.
//===========================================================================
class _XTP_EXT_CLASS CXTPSkinManagerClass : public CXTPSynchronized
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPSkinManagerClass object.
	// Parameters:
	//     pSchema      - Pointer to the parent skin manager class.
	//     pszClassList - Class name.
	//     nClassCode   - Hash value of the class name.
	//-----------------------------------------------------------------------
	CXTPSkinManagerClass(CXTPSkinManagerSchema* pSchema, CString pszClassList, int nClassCode);

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPSkinManagerClass object, handles cleanup and deallocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPSkinManagerClass();

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Gets the value of a specified string property.
	// Parameters:
	//     iPartId     - Part number.
	//     iStateId    - State number of the part.
	//     iPropId     - Property number to get the value of.
	//     lpszDefault - Default value.
	// Returns:
	//     The value of the specified string property.
	//-----------------------------------------------------------------------
	CString GetThemeString(int iPartId, int iStateId, int iPropId, LPCTSTR lpszDefault = NULL);

	//-----------------------------------------------------------------------
	// Summary:
	//     Gets the value of a specified RECT property.
	// Parameters:
	//     iPartId   - Part number.
	//     iStateId  - State number of the part.
	//     iPropId   - Property number to get the value of.
	//     rcDefault - Default value.
	// Returns:
	//     The value of the specified RECT property.
	//-----------------------------------------------------------------------
	CRect GetThemeRect(int iPartId, int iStateId, int iPropId, CRect rcDefault = CRect(0, 0, 0, 0));

	//-----------------------------------------------------------------------
	// Summary:
	//     Gets the value of a specified int property.
	// Parameters:
	//     iPartId  - Part number.
	//     iStateId - State number of the part.
	//     iPropId  - Property number to get the value of.
	//     nDefault - Default value.
	// Returns:
	//     The value of the specified int property.
	//-----------------------------------------------------------------------
	int GetThemeInt(int iPartId, int iStateId, int iPropId, int nDefault = 0);

	//-----------------------------------------------------------------------
	// Summary:
	//     Gets the value of a specified size property.
	// Parameters:
	//     iPartId   - Part number.
	//     iStateId  - State number of the part.
	//     iPropId   - Property number to get the value of.
	//     szDefault - Default value.
	// Returns:
	//     The value of the specified size property.
	//-----------------------------------------------------------------------
	CSize GetThemeSize(int iPartId, int iStateId, int iPropId, CSize szDefault = CSize(0));

	//-----------------------------------------------------------------------
	// Summary:
	//     Gets the value of a specified BOOL property.
	// Parameters:
	//     iPartId  - Part number.
	//     iStateId - State number of the part.
	//     iPropId  - Property number to get the value of.
	//     bDefault - Default value.
	// Returns:
	//     The value of the specified BOOL property.
	//-----------------------------------------------------------------------
	BOOL GetThemeBool(int iPartId, int iStateId, int iPropId, BOOL bDefault = FALSE);

	//-----------------------------------------------------------------------
	// Summary:
	//     Gets the value of a specified color property.
	// Parameters:
	//     iPartId    - Part number.
	//     iStateId   - State number of the part.
	//     iPropId    - Property number to get the value of.
	//     clrDefault - Default value.
	// Returns:
	//     The value of the specified color property.
	//-----------------------------------------------------------------------
	COLORREF GetThemeColor(int iPartId, int iStateId, int iPropId,
						   COLORREF clrDefault = (COLORREF)-1);

	//-----------------------------------------------------------------------
	// Summary:
	//     Gets the value of a specified enum property.
	// Parameters:
	//     iPartId  - Part number.
	//     iStateId - State number of the part.
	//     iPropId  - Property number to get the value of.
	//     nDefault - Default value.
	// Returns:
	//     The value of the specified enum property.
	//-----------------------------------------------------------------------
	int GetThemeEnumValue(int iPartId, int iStateId, int iPropId, int nDefault = 0);

	//-----------------------------------------------------------------------
	// Summary:
	//     Gets the size of a specified theme part.
	// Parameters:
	//     iPartId  - Part number.
	//     iStateId - State number of the part.
	//     pRect    - Pointer to a RECT for the part drawing destination.
	//     eSize    - Type of size to be retrieved.
	//     pSize    - Pointer to a SIZE that receives the size of the part.
	// Returns:
	//     TRUE if successful, otherwise FALSE.
	//-----------------------------------------------------------------------
	BOOL GetThemePartSize(int iPartId, int iStateId, RECT* pRect, int eSize, SIZE* pSize);

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Draws the background image defined by the visual style for a
	//     specified control part.
	// Parameters:
	//     pDC      - Pointer to a valid device context.
	//     iPartId  - Specifies the part to draw.
	//     iStateId - Specifies the state of the part to draw.
	//     pRect    - Pointer to a RECT structure that contains the rectangle,
	//                in logical coordinates, in which the background image is to be drawn.
	// Returns:
	//     TRUE if successful, otherwise FALSE.
	//-----------------------------------------------------------------------
	BOOL DrawThemeBackground(CDC* pDC, int iPartId, int iStateId, const RECT* pRect);

	//-----------------------------------------------------------------------
	// Summary:
	//     Draws text using the color and font defined by the visual style.
	// Parameters:
	//     pDC      - Pointer to a valid device context.
	//     iPartId  - Specifies the part that contains the text.
	//     iStateId - Specifies the state of the part to draw.
	//     strText  - Reference to a string that contains the text to draw.
	//     dwFormat - One or more values that specify the string's formatting.
	//     pRect    - Pointer to a RECT structure that contains the rectangle,
	//                in logical coordinates, in which the text is to be drawn.
	// Returns:
	//     TRUE if successful, otherwise FALSE.
	//-----------------------------------------------------------------------
	BOOL DrawThemeText(CDC* pDC, int iPartId, int iStateId, const CString& strText, DWORD dwFormat,
					   const RECT* pRect);

	//-----------------------------------------------------------------------
	// Summary:
	//     Gets the image collection of the class.
	// Returns:
	//     A pointer to the image collection of the class.
	//-----------------------------------------------------------------------
	CXTPSkinImages* GetImages() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Gets the hash value of the class name.
	// Returns:
	//     The hash value of the class name.
	//-----------------------------------------------------------------------
	int GetClassCode() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Gets the class name.
	// Returns:
	//     The class name.
	//-----------------------------------------------------------------------
	CString GetClassName() const;

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Gets the property value of a specified part and state.
	// Parameters:
	//     propType - Property type.
	//     iPartId  - Part number.
	//     iStateId - State number of the part.
	//     iPropId  - Property number to get the value of.
	// Returns:
	//     A pointer to the CXTPSkinManagerSchemaPropertyClass containing
	//     the value of the property.
	// See Also:
	//     XTPSkinManagerProperty, CXTPSkinManagerSchemaProperty
	//-----------------------------------------------------------------------
	CXTPSkinManagerSchemaProperty* GetProperty(XTPSkinManagerProperty propType, int iPartId,
											   int iStateId, int iPropId);

	//{{AFX_CODEJOCK_PRIVATE
	_XTP_DEPRECATED
	HTHEME GetSystemTheme();
	static CXTPSkinManagerClass* FromHandle(HTHEME hTheme);
	void BindTheme(HTHEME hTheme);
	void UnbindTheme(HTHEME hTheme);
	//}}AFX_CODEJOCK_PRIVATE

private:
	void UnbindTheme(HTHEME hTheme, BOOL bFinal);
	void UnbindAllThemes();

protected:
	CXTPSkinManagerSchema* m_pSchema; // Parent CXTPSkinManager object.

	_XTP_DEPRECATED HTHEME m_hSystemTheme;

	CString m_strClass; // Class name.
	UINT m_nClassCode;  // Hashed value of the class name.

	CXTPSkinImages* m_pImages; // Image collection.
	CMap<UINT, UINT, CXTPSkinManagerSchemaProperty*, CXTPSkinManagerSchemaProperty*>
		m_mapCachedProperties; // Cached collection of class properties.

private:
	struct ThemeClassAssoc
	{
		ThemeClassAssoc()
			: nRefs(0)
			, pClass(NULL)
		{
		}

		LONG nRefs;
		CXTPSkinManagerClass* pClass;
	};

	static CCriticalSection m_themeClassAccess;
	static CMap<HTHEME, HTHEME, ThemeClassAssoc*, ThemeClassAssoc*> m_themeClasses;
};

#	define XTP_SKINMETRICS_COLORTABLESIZE 31

//===========================================================================
// Summary:
//     Implement this interface in order to provide custom configured
//     window filter functionality. The filtered out windows will not be
//     skinned. If you find yourself frequently filtering out some system
//     window in order to prevent application crashes, please report to us
//     its class name and, ideally, the contents of the WNDCLASS structure
//     filled out by a call to GetWindowClass for this window handle.
//     Use CXTPSkinManager::SetWindowFilter for setting or disabling a
//     custom provided window filter implementation.
//===========================================================================
struct IXTPSkinManagerWindowFilter
{
	//-----------------------------------------------------------------------
	// Handles object destruction
	//-----------------------------------------------------------------------
	virtual ~IXTPSkinManagerWindowFilter()
	{
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     The implementation must make a decision as to whether a window
	//     class must be filtered out (not skinned) or not, depending on certain
	//     window class characteristics.
	// Parameters:
	//     lpszClassName - Window class name.
	// Returns:
	//     TRUE if the window class has to be filtered out, otherwise FALSE (default).
	//-----------------------------------------------------------------------
	virtual BOOL FilterWindowClass(LPCSTR lpszClassName) = 0;
	virtual BOOL FilterWindowClass(
		LPCWSTR lpszClassName) = 0; // <combine
									// IXTPSkinManagerWindowFilter::FilterWindowClass@LPCSTR>

	//-----------------------------------------------------------------------
	// Summary:
	//     The implementation must make a decision as to whether a window
	//     must be filtered out (not skinned) or not depending on certain
	//     window characteristics.
	// Parameters:
	//     hWnd          - Window handle.
	//     lpszClassName - Window class name if known, otherwise NULL.
	//     lpcs          - Pointer to the window's CREATESTRUCT structure if known, otherwise NULL.
	// Returns:
	//     TRUE if the window has to be filtered out, otherwise FALSE (default).
	//-----------------------------------------------------------------------
	virtual BOOL FilterWindow(HWND hWnd, LPCTSTR lpszClassName, LPCREATESTRUCT lpcs) = 0;
};

//===========================================================================
// Summary:
//     CXTPSkinManagerMetrics is a standalone class that represents a
//     collection of metrics of the current skin.
//===========================================================================
class _XTP_EXT_CLASS CXTPSkinManagerMetrics : public CXTPSynchronized
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPSkinManagerMetrics object.
	// Parameters:
	//     pSchema - Pointer to the parent skin manager class.
	//-----------------------------------------------------------------------
	CXTPSkinManagerMetrics(CXTPSkinManagerSchema* pSchema);

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPSkinManagerMetrics object, handles cleanup and deallocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPSkinManagerMetrics();

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Refreshes visual metrics.
	//-----------------------------------------------------------------------
	virtual void RefreshMetrics();

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines if a specified handle belongs to metrics.
	// Parameters:
	//     hObject - Handle to be tested.
	// Returns:
	//     TRUE if the specified handle belongs to metrics, otherwise FALSE.
	//-----------------------------------------------------------------------
	BOOL IsMetricObject(HGDIOBJ hObject) const;

	//{{AFX_CODEJOCK_PRIVATE
	//-----------------------------------------------------------------------
	// Summary: Determines if handle belongs to any existing metrics.
	// Input:   hObject - Handle to test
	//-----------------------------------------------------------------------
	_XTP_DEPRECATED_IN_FAVOR(IsMetricObject)
	static BOOL AFX_CDECL IsKnownMetricObject(HGDIOBJ hObject);
	//}}AFX_CODEJOCK_PRIVATE

	//-------------------------------------------------------------------------
	// Summary:
	//     Destroys all GDI objects of metrics.
	//-------------------------------------------------------------------------
	void DestroyMetrics();

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves the skinned color of a specified index.
	// Parameters:
	//     nIndex - Index of the skinned color to retrieve.
	// Returns:
	//     The skinned color of the specified index, otherwise COLORREF_NULL.
	//-----------------------------------------------------------------------
	COLORREF GetColor(int nIndex) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves the skinned brush of a specified index.
	// Parameters:
	//     nIndex - Index of the skinned brush to retrieve.
	// Returns:
	//     The skinned brush of the specified index, otherwise NULL.
	//-----------------------------------------------------------------------
	HBRUSH GetBrush(int nIndex) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Creates a tab control brush.
	// Parameters:
	//     pBitmap - Pointer to the bitmap to be used
	//               as the tab control brush pattern.
	// Returns:
	//     TRUE if the tab control brush was successfully created.
	//-----------------------------------------------------------------------
	BOOL CreateTabControlBrush(CBitmap* pBitmap);

public:
	mutable CXTPCriticalSection m_csAccess;
	BOOL m_bAllowHandleDestroy;

	CXTPFont m_xtpFontCaption;   // Caption font.
	CXTPFont m_xtpFontMenu;		 // Menu font.
	CXTPFont m_xtpFontSmCaption; // Small caption font.

	XTP_SUBSTITUTE_GDI_MEMBER_WITH_CACHED(CFont, m_fntCaption, m_xtpFontCaption,
										  GetCaptionFontHandle);
	XTP_SUBSTITUTE_GDI_MEMBER_WITH_CACHED(CFont, m_fntMenu, m_xtpFontMenu, GetMenuFontHandle);
	XTP_SUBSTITUTE_GDI_MEMBER_WITH_CACHED(CFont, m_fntSmCaption, m_xtpFontSmCaption,
										  GetSmCaptionFontHandle);

	CXTPSkinManagerSchema* m_pSchema; // Parent CXTPSkinManager object.

	CXTPBrush m_xtpBrushDialog;		// Dialog brush.
	CXTPBrush m_xtpBrushTabControl; // Tab control brush.
	CXTPBrush m_xtpBrushWindow;		// Window brush.

	XTP_SUBSTITUTE_GDI_MEMBER_WITH_CACHED_(HBRUSH, HBRUSH, HBRUSH, m_brushDialog, m_xtpBrushDialog,
										   GetDialogBrushHandle);
	XTP_SUBSTITUTE_GDI_MEMBER_WITH_CACHED_(HBRUSH, HBRUSH, HBRUSH, m_brushTabControl,
										   m_xtpBrushTabControl, GetTabControlBrushHandle);
	XTP_SUBSTITUTE_GDI_MEMBER_WITH_CACHED_(HBRUSH, HBRUSH, HBRUSH, m_brushWindow, m_xtpBrushWindow,
										   GetWindowBrushHandle);

	COLORREF m_clrEdgeLight;	 // Light edge color.
	COLORREF m_clrEdgeHighLight; // HighLight edge color.
	COLORREF m_clrEdgeShadow;	// Shadow edge color.
	COLORREF m_clrEdgeDkShadow;  // Dark shadow edge color.

	COLORREF m_clrTheme[XTP_SKINMETRICS_COLORTABLESIZE]; // Colors of the theme.

	HBRUSH m_brTheme[XTP_SKINMETRICS_COLORTABLESIZE]; // Brushes of the theme.

	int m_nBorderSize; // Border size.
	int m_cxBorder;	// SM_CXBORDER system metric.
	int m_cyBorder;	// SM_CYBORDER system metric.
	int m_cxEdge;	  // SM_CXEDGE system metric.
	int m_cyEdge;	  // SM_CYEDGE system metric.

	int m_cxHScroll; // SM_CXHSCROLL system metric.
	int m_cyHScroll; // SM_CYHSCROLL system metric.
	int m_cxVScroll; // SM_CXVSCROLL system metric.
	int m_cyVScroll; // SM_CYVSCROLL system metric.

	int m_cyCaption;	  // Height of captions.
	int m_cySmallCaption; // Height of small captions.

	int m_cyOsCaption;		// System caption height.
	int m_cyOsSmallCaption; // System small caption height.
	int m_cxOsVScroll;		// System SM_CXVSCROLL metric.

	BOOL m_bRefreshMetrics; // TRUE if refresh metrics is currently executed.
};

//===========================================================================
// Summary:
//     CXTPSkinManager is a standalone class that represents a
//     global object that manages skins.
//===========================================================================
class _XTP_EXT_CLASS CXTPSkinManager
	: public CXTPSynchronized
	, IXTPApplicationEvents
{
	friend class CXTPSingleton<
		CXTPSkinManager,
		CXTPSingletonDependencies<CXTPSingleton<CXTPSkinManagerModuleListSharedData>,
								  CXTPSingleton<CXTPSkinManagerSchemaCodeManager>,
								  CXTPSingleton<CXTPBrushCache> > >;

private:
	DECLARE_HANDLE(HWINEVENTHOOK);

	typedef void(CALLBACK* WINEVENTPROC)(HWINEVENTHOOK hWinEventHook, DWORD event, HWND hwnd,
										 LONG idObject, LONG idChild, DWORD idEventThread,
										 DWORD dwmsEventTime);
	typedef HWINEVENTHOOK(WINAPI* LPFNSETWINEVENTHOOK)(UINT eventMin, UINT eventMax,
													   HMODULE hmodWinEventProc,
													   WINEVENTPROC lpfnWinEventProc,
													   DWORD idProcess, DWORD idThread,
													   UINT dwflags);
	typedef BOOL(WINAPI* LPFNUNHOOKWINEVENT)(HWINEVENTHOOK hWinEventHook);

protected:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPSkinManager object.
	//-----------------------------------------------------------------------
	CXTPSkinManager();

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPSkinManager object, handles cleanup and deallocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPSkinManager();

	//-----------------------------------------------------------------------
	// Summary:
	//     Loads a skin from a cjstyles or msstyles file.
	// Parameters:
	//     lpszResourcePath - Path to the skin file.
	//     lpszIniFileName  - .ini file of the skin to load.
	// Returns:
	//     TRUE if successful, otherwise FALSE.
	//-----------------------------------------------------------------------
	BOOL SkinLoadMemory(const void* pSkinLibrary, int Size, LPCTSTR lpszIniFileName = NULL, int mode = 0 /*mode = 0 - not clean, mode = 1 - need clean, mode = 2 - clone memory*/);
	BOOL SkinLoadFile(LPCTSTR lpszSkinFileName, LPCTSTR lpszIniFileName = NULL);
	BOOL SkinSwitchSchema(LPCTSTR lpszIniFileName);
	void SkinRelease();
	std::vector<CString> SkinListSchemes();
	CString SkinGetDefaultSchema();
	BOOL GetDefaultThemeIsSet();

	void SetDefaultThemeFont(CString pFontName);
	CString GetDefaultThemeFont();

	CXTPSkinManagerSchema* CreateSchema(const void* pSkinLibrary, int Size, LPCTSTR lpszIniFileName = NULL);

	void AlphaEnableWindow(HWND hwnd, BYTE Alpha);
	void AlphaDisableWindow(HWND hwnd);
	void AlphaChengeWindow(HWND hwnd, BYTE Alpha);

	void ExitProgramm();

	CString GetCommandLineTool();

	//-----------------------------------------------------------------------
	// Summary:
	//     Creates a new schema instance from a provided resource.
	// Parameters:
	//     lpszResourcePath - Path to the styles file to load the schema from.
	//     lpszIniFileName  - Optional embedded .ini file name to use from the styles file.
	// Returns:
	//     A pointer to the newly created schema, otherwise NULL in the case of an error.
	//-----------------------------------------------------------------------
	CXTPSkinManagerSchema* CreateSchema(LPCTSTR lpszResourcePath, LPCTSTR lpszIniFileName = NULL);

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Determines which parts of the application skin are applied.
	// Returns:
	//     One or more XTPSkinApplyOptions values.
	// See Also:
	//     XTPSkinApplyOptions, SetApplyOptions
	//-----------------------------------------------------------------------
	DWORD GetApplyOptions() const;
	BOOL HasApplyOptions(DWORD dwOptions) const; // <combine CXTPSkinManager::GetApplyOptions@const>

	//-----------------------------------------------------------------------
	// Summary:
	//     Specifies which parts of the application skin should be applied.
	// Parameters:
	//     dwOptions - Can be combined by using the bitwise OR (|) operator:
	//                 * <b>xtpSkinApplyMetrics</b>: To apply skin metrics
	//                                               (caption height, scrollbar width, etc.).
	//                 * <b>xtpSkinApplyFrame</b>: To apply window frames.
	// See Also:
	//     XTPSkinApplyOptions, GetApplyOptions
	//-----------------------------------------------------------------------
	void SetApplyOptions(DWORD dwOptions);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to manually set the window whose appearance must be skinned.
	// Parameters:
	//     hWnd           - Handle of the window whose appearance must be skinned.
	//     bApplyChildren - TRUE to skin all child windows of the specified window
	//                      recursively, FALSE to only skin the specified window.
	//-----------------------------------------------------------------------
	void ApplyWindow(HWND hWnd);
	void ApplyWindow(HWND hWnd, BOOL bApplyChildren); // <combine CXTPSkinManager::ApplyWindow@HWND>

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets the window theme.
	// Parameters:
	//     hWnd    - Valid window handle for which a new theme is to be set.
	//     pSchema - Valid pointer to a schema describing the theme to be set.
	// See Also:
	//     CreateSchema
	//-----------------------------------------------------------------------
	void SetWindowTheme(HWND hWnd, CXTPSkinManagerSchema* pSchema);

	//-----------------------------------------------------------------------
	// Summary:
	//     Specifies if new windows should be automatically skinned.
	// Parameters:
	//     bAutoApply - TRUE to automatically skin new windows, FALSE otherwise.
	// Remarks:
	//     The current skin is used if new windows are automatically skinned.
	//-----------------------------------------------------------------------
	void SetAutoApplyNewWindows(BOOL bAutoApply);

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines if new windows are automatically skinned.
	// Remarks:
	//     The current skin is used if new windows are automatically skinned.
	// Returns:
	//     TRUE if new windows are automatically skinned, otherwise FALSE.
	//-----------------------------------------------------------------------
	BOOL GetAutoApplyNewWindows() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets the skin manager.
	// Parameters:
	//     pSkinManager - New skin manager to be set.
	//     bDelete      - TRUE to delete the previous skin manager.
	// Returns:
	//     If the bDelete parameter is set to TRUE, then a NULL value will be returned.
	//     If the bDelete parameter is set to FALSE, then a pointer to the previous
	//     skin manager will be returned.
	// Example:
	// <code>CXTPSkinManager::SetSkinManager(new CMySkinManager());</code>
	//-----------------------------------------------------------------------
	static CXTPSkinManager* AFX_CDECL SetSkinManager(CXTPSkinManager* pSkinManager,
													 BOOL bDelete = TRUE);

	//-----------------------------------------------------------------------
	// Summary:
	//     Excludes a specified .dll file from API hooking.
	// Parameters:
	//     lpszModule - Name of the .dll file to exclude.
	//-----------------------------------------------------------------------
#	if defined(XTP_SKINFRAMEWORK_USE_DETOURS_API_HOOK) && !defined(_XTP_ACTIVEX)
	_XTP_DEPRECATE("The method is disabled for compilation with "
				   "XTP_SKINFRAMEWORK_USE_DETOURS_API_HOOK and "
				   "its empty implementation is added for backward compatibility only. In order to "
				   "you the legacy implementation of API hooking re-compile ToolkitPro with "
				   "XTP_SKINFRAMEWORK_USE_LEGACY_API_HOOK macro defined.")
#	endif
	void ExcludeModule(LPCTSTR lpszModule);

	// -------------------------------------------------------------------
	// Summary:
	//     Gets a specified skin class by its name.
	// Parameters:
	//     strClassList - Name of the skin class.
	//     pObject      - Reference to the skin object to apply the skin.
	// Returns:
	//     A pointer to the specified CXTPSkinManagerClass object.
	// -------------------------------------------------------------------
	virtual CXTPSkinManagerClass* GetSkinClass(const CXTPSkinObject* pObject, CString strClassList);

	//-----------------------------------------------------------------------
	// Summary:
	//     Gets the skin object used to draw a specified window.
	// Parameters:
	//     hWnd - Handle of the skinned window.
	// Returns:
	//     A pointer to the CXTPSkinObject used to draw the window.
	//-----------------------------------------------------------------------
	CXTPSkinObject* Lookup(HWND hWnd);

	//-----------------------------------------------------------------------
	// Summary:
	//     Gets the skin metrics.
	// Returns:
	//     A pointer to the CXTPSkinManagerMetrics object containing the skin metrics.
	//-----------------------------------------------------------------------
	CXTPSkinManagerMetrics* GetMetrics() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines if skinning is enabled/disabled.
	// Returns:
	//     TRUE if skinning is enabled, FALSE if skinning is disabled.
	//-----------------------------------------------------------------------
	BOOL IsEnabled() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Gets the skin schema.
	// Returns:
	//     A pointer to the CXTPSkinManagerSchema object used to draw window parts.
	//-----------------------------------------------------------------------
	CXTPSkinManagerSchema* GetSchema() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Gets the resource file.
	// Returns:
	//     A pointer to the CXTPSkinManagerResouceFile used to manage skin resources.
	// See Also:
	//     SetResourceFile
	//-----------------------------------------------------------------------
	CXTPSkinManagerResourceFile* GetResourceFile() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets the resource file.
	// Parameters:
	//     pResourceFile - Pointer to a CXTPSkinManagerResouceFile object.
	// See Also:
	//     GetResourceFile
	// Example:
	//     <code>XTPSkinManager()->SetResourceFile(new CMySkinManagerResourceFile);</code>
	//-----------------------------------------------------------------------
	void SetResourceFile(CXTPSkinManagerResourceFile* pResourceFile);

	//-----------------------------------------------------------------------
	// Summary:
	//     Enables/disables dialog background theme.
	// Parameters:
	//     hWnd    - Window handle of the target dialog.
	//     dwFlags - One of the following:
	///              * ETDT_ENABLE: Enables theme-defined dialog background texturing.
	//               * ETDT_DISABLE: Disables background texturing.
	//               * ETDT_ENABLETAB: Enables theme-defined background texturing
	//                                 using the tab texture.
	// Remarks:
	//     This method can be used to tailor dialog compatibility with child windows
	//     and controls that may or may not coordinate the rendering of their
	//     client area backgrounds with that of their parent dialog in a manner that
	//     supports seamless background texturing.
	// Returns:
	//     S_OK if successful.
	//-----------------------------------------------------------------------
	HRESULT EnableThemeDialogTexture(HWND hWnd, DWORD dwFlags);

	//-----------------------------------------------------------------------
	// Summary:
	//     Gets the value of a specified system color.
	// Parameters:
	//     iColorId - System color index (defined in winuser.h).
	// Returns:
	//     The value of the specified system color.
	//-----------------------------------------------------------------------
	COLORREF GetThemeSysColor(int iColorId);

	//-----------------------------------------------------------------------
	// Summary:
	//     Gets the value of a specified system font.
	// Parameters:
	//     iFontId - TMT_XXX font number (first font is TMT_CAPTIONFONT).
	//     plf     - Pointer to a LOGFONT to receive the value of the specified
	//               system font (scaled for the current logical screen dpi).
	// Returns:
	//     S_OK if successful.
	//-----------------------------------------------------------------------
	HRESULT GetThemeSysFont(int iFontId, LOGFONT* plf);

	//-----------------------------------------------------------------------
	// Summary:
	//     Gets the size value of a specified system metric.
	// Parameters:
	//     iSizeId  - TMT_XXX size value.
	// Returns:
	//     The size value of the specified system metric
	//     (scaled for the current logical screen dpi).
	//-----------------------------------------------------------------------
	int GetThemeSysSize(int iSizeId);

	//-----------------------------------------------------------------------
	// Summary:
	//     Gets the BOOL value of a specified system metric.
	// Parameters:
	//     iBoolId - TMT_XXX BOOL number (first BOOL is TMT_FLATMENUS).
	// Returns:
	//     The BOOL value of the specified system metric.
	//-----------------------------------------------------------------------
	BOOL GetThemeSysBool(int iBoolId);

	//-----------------------------------------------------------------------
	// Summary:
	//     Gets the name of the current theme.
	// Parameters:
	//     strThemeFileName - Reference to a string to receive the
	//                        theme path and file name.
	//     strColorBuff     - Reference to a string to receive the
	//                        canonical color scheme name.
	//-----------------------------------------------------------------------
	void GetCurrentThemeName(CString& strThemeFileName, CString& strColorBuff);

	//-----------------------------------------------------------------------
	// Summary:
	//     Gets the CXTPSkinManagerClass class from a specified HTHEME handle.
	// Parameters:
	//     hTheme - HTHEME handle.
	// Returns:
	//     A pointer to the CXTPSkinManagerClass class.
	//-----------------------------------------------------------------------
	CXTPSkinManagerClass* FromHandle(HTHEME hTheme);

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines if ComCtl32.dll is of version 6.0 or above.
	// Returns:
	//     TRUE if ComCtl32.dll is of version 6.0 or above.
	// See Also:
	//     IsWin9x
	//-----------------------------------------------------------------------
	BOOL IsComCtlV6() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines if the operating system is of the Windows 9x family
	//     and if it is Windows 95 or a later version.
	// Returns:
	//     TRUE if the operating system is of the Windows 9x family
	//     and if it is Windows 95 or a later version, otherwise FALSE.
	// See Also:
	//     IsComCtlV6
	//-----------------------------------------------------------------------
	BOOL IsWin9x() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Redraws all controls with the current skin.
	//-----------------------------------------------------------------------
	void RedrawAllControls();

	//-----------------------------------------------------------------------
	// Summary:
	//     Enables skin framework for the current thread.
	// Remarks:
	//     This method can be called for each thread in a multi-threaded application
	//     to enable skin framework for all threads.
	//-----------------------------------------------------------------------
	void EnableCurrentThread();

	//-----------------------------------------------------------------------
	// Summary:
	//     Specifies if new threads should be automatically skinned.
	// Parameters:
	//     bAutoApply - TRUE to automatically skin new threads, otherwise FALSE.
	//-----------------------------------------------------------------------
	void SetAutoApplyNewThreads(BOOL bAutoApply);

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines if new threads are automatically skinned.
	// Returns:
	//     TRUE if new threads are automatically skinned, otherwise FALSE.
	//-----------------------------------------------------------------------
	BOOL GetAutoApplyNewThreads() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Gets the class map object whose purpose is to map
	//     window classes to skin framework classes.
	// Returns:
	//     A pointer to the class map object.
	//-----------------------------------------------------------------------
	CXTPSkinObjectClassMap* GetClassMap() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Removes the hook from a specified window.
	// Parameters:
	//     hWnd - Handle of the window.
	// See Also:
	//     RemoveAll
	//-----------------------------------------------------------------------
	void Remove(HWND hWnd);

	//-------------------------------------------------------------------------
	// Summary:
	//     Removes all hooks.
	// Parameters:
	//     bDetach - TRUE to detach hooks before they are removed.
	// See Also:
	//     Remove
	//-------------------------------------------------------------------------
	void RemoveAll(BOOL bDetach = TRUE);

	//-------------------------------------------------------------------------
	// Summary:
	//     Determines if any color filter is installed.
	// Returns:
	//     TRUE if any color filter is installed, otherwise FALSE.
	//-------------------------------------------------------------------------
	BOOL IsColorFilterExists() const;

	//-------------------------------------------------------------------------
	// Summary:
	//     This method is called internally to transform a specified color
	//     using installed color filters.
	// Parameters:
	//     clr - Reference to the color to transform.
	//-------------------------------------------------------------------------
	void ApplyColorFilter(COLORREF& clr);

	//-------------------------------------------------------------------------
	// Summary:
	//     Adds a color filter.
	// Parameters:
	//     pFilter - Pointer to the color filter to add.
	//-------------------------------------------------------------------------
	void AddColorFilter(CXTPSkinManagerColorFilter* pFilter);

	//-------------------------------------------------------------------------
	// Summary:
	//     Destroys all added color filters.
	//-------------------------------------------------------------------------
	void RemoveColorFilters();

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
	//     Determines if a specified module is persistent.
	// Parameters:
	//     hModule - Handle of the module to check.
	// Remarks:
	//     A module is persistent if it will remain loaded into the process address
	//     space for the entire lifetime of the skin manager and cannot be unloaded.
	//     The current and/or executable modules are always considered persistent.
	// Returns:
	//     TRUE if the specified module is persistent, otherwise FALSE.
	//-----------------------------------------------------------------------
	BOOL IsPersistentModule(HMODULE hModule);

	//{{AFX_CODEJOCK_PRIVATE
	CXTPSkinManagerApiHookBase* GetApiHook() const;
	//}}AFX_CODEJOCK_PRIVATE

protected:
	//-----------------------------------------------------------------------
	// Summary:
	//     Parses a visual styles .ini file.
	// Parameters:
	//     lpszResourcePath - Path to the resource file.
	//     lpszIniFileName  - .ini file name.
	// Returns:
	//     TRUE if successful, otherwise FALSE.
	// See Also:
	//     FreeSkinData
	//-----------------------------------------------------------------------
	BOOL ReadSkinData(const void* pSkinLibrary, int Size, LPCTSTR lpszIniFileName, bool needClean);

	//-------------------------------------------------------------------------
	// Summary:
	//     Frees skin resources.
	// See Also:
	//     ReadSkinData
	//-------------------------------------------------------------------------
	void FreeSkinData();

	// ----------------------------------------------------------------------
	// Summary:
	//     Creates a hook class for a specified window.
	// Parameters:
	//     hWnd          - Handle to the window to hook.
	//     lpszClassName - Class name of the specified window.
	//     lpcs          - Pointer to a CREATESTRUCT structure containing
	//                     information about the CWnd object being created.
	//     bAuto         - TRUE if the window was automatically skinned when it
	//                     was created, FALSE if the window was manually skinned.
	// Returns:
	//     A pointer to the CXTPSkinObject hooked to the specified window.
	// See Also:
	//     CXTPSkinObject
	// ----------------------------------------------------------------------
	virtual CXTPSkinObject* SetHook(HWND hWnd, LPCTSTR lpszClassName, LPCREATESTRUCT lpcs,
									BOOL bAuto);

	//-------------------------------------------------------------------------
	// Summary:
	//     Sets the window filter implementation.
	// Parameters:
	//     pFilter - Pointer to the window filter implementation, or NULL to
	//               disable any previously set window filter implementation.
	// See Also:
	//     IXTPSkinManagerWindowFilter
	//-------------------------------------------------------------------------
	void SetWindowFilter(IXTPSkinManagerWindowFilter* pFilter);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called by the framework before an application shuts down.
	// Parameters:
	//     pApplication - Pointer to the application that will be shut down.
	//-----------------------------------------------------------------------
	virtual void OnBeforeApplicationShutdown(CXTPApplication* pApplication);

public:
	//{{AFX_CODEJOCK_PRIVATE
	BOOL IsWindowClassFilteredOut(LPCSTR lpszClassName);
	BOOL IsWindowClassFilteredOut(LPCWSTR lpszClassName);
	BOOL IsWindowFilteredOut(HWND hWnd, LPCTSTR lpszClassName, LPCREATESTRUCT lpcs,
							 BOOL bRecursive);
	BOOL IsDCFilteredOut(HDC hDC, BOOL bRecursive);
	//}}AFX_CODEJOCK_PRIVATE

protected:
	//{{AFX_CODEJOCK_PRIVATE
	static BOOL AFX_CDECL IsClassKnownIgnoredClass(LPCTSTR lpszClassName);
	static LRESULT CALLBACK HookWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK CbtFilterHook(int code, WPARAM wParam, LPARAM lParam);
	static void CALLBACK WinEventProc(HWINEVENTHOOK hWinEventHook, DWORD event, HWND hWnd,
									  LONG idObject, LONG idChild, DWORD dwEventThread,
									  DWORD dwmsEventTime);
	static LRESULT CALLBACK DoCallWindowProc(WNDPROC lpPrevWndFunc, HWND hWnd, UINT Msg,
											 WPARAM wParam, LPARAM lParam);
	static BOOL CALLBACK EnumWindowsProcNetBroadcast(HWND hwnd, LPARAM lParam);
	static BOOL CALLBACK EnumNonRegistredWindows(HWND hwnd, LPARAM lParam);
	static BOOL AFX_CDECL AreAppVisualThemesDisabled();
	void Remove(HWND hWnd, BOOL bAuto);
	BOOL IsWindowFilteredOut(HWND hWnd, LPCTSTR lpszClassName, LPCREATESTRUCT lpcs);
	//}}AFX_CODEJOCK_PRIVATE

protected:
	DWORD m_dwComCtrl; // Version of ComCtl.dll.
	const BOOL m_bAppVisualThemesDisabled;
	BOOL DefaultThemeIsSet;
	CString DefaultFont;
	bool SkinEnableDefault;
	CString m_CommandLine;
	BOOL m_bWin9x;			  // TRUE if OS is Win9x.
	BOOL m_bEnabled;		  // TRIE if skinning is enabled.
	BOOL m_bAutoApplyWindows; // TRUE to auto-skin all new windows.
	BOOL m_bAutoApplyThreads; // TRUE to auto-skin all new threads.

	CXTPSkinManagerSchema* m_pSchema;			  // Current schema.
	CXTPSkinManagerResourceFile* m_pResourceFile; // Resource file.

	DWORD m_dwApplyOptions; // Apply options.

	HWINEVENTHOOK m_hWinEventHook; // HWINEVENTHOOK value that identifies event hook instance.
	CMap<HWND, HWND, CXTPSkinObject*, CXTPSkinObject*> m_mapObjects; // Collection of CXTPSkinObject
																	 // classes.
	CRITICAL_SECTION m_csObjects; // Helper critical section object.

	LPFNSETWINEVENTHOOK m_pSetWinEventHook; // Pointer to SetWinEventHook API.
	LPFNUNHOOKWINEVENT m_pUnhookWinEvent;   // Pointer to UnhookWinEvent API.

	CArray<CXTPSkinManagerColorFilter*, CXTPSkinManagerColorFilter*> m_arrFilters; // Array of
																				   // filters to
																				   // apply.

	CXTPSkinObjectClassMap* m_pClassMap; // Class map helper.

	IXTPSkinManagerWindowFilter* m_pWindowFilter; // Custom provided window filter implementation.

	mutable CXTPSkinManagerApiHookBase* m_pApiHook;  // API hook instance.
	static CXTPSkinManager* s_pInstance;			 // Instance of SkinManager.
	CXTPRWCriticalSection m_csPersistentModuleCache; // Synchronization primitive for persistent
													 // module info cache access.

	struct PERSISTENT_IMPORT_INFO
	{
		HMODULE hModule;
		BOOL bPersistent;
	};

	CArray<PERSISTENT_IMPORT_INFO, PERSISTENT_IMPORT_INFO&>
		m_arPersistentModuleCache; // Persistent module info cache.

public:
	//{{AFX_CODEJOCK_PRIVATE
	AFX_MODULE_STATE* m_pModuleState;

#	ifdef _XTP_ACTIVEX
	static BOOL s_bUserLegacyApiHook;
#	endif
	//}}AFX_CODEJOCK_PRIVATE

private:
	class CDestructor;
	friend _XTP_EXT_CLASS CXTPSkinManager* AFX_CDECL XTPSkinManager();
	friend _XTP_EXT_CLASS CXTPSkinManager* AFX_CDECL XTPSkinMgr();
	friend class CXTPSkinObject;
	friend class CDestructor;
	friend class CXTPSkinManagerApiHookBase;
};

AFX_INLINE CXTPSkinImages* CXTPSkinManagerClass::GetImages() const
{
	return m_pImages;
}
AFX_INLINE int CXTPSkinManagerClass::GetClassCode() const
{
	return static_cast<int>(m_nClassCode);
}
AFX_INLINE CString CXTPSkinManagerClass::GetClassName() const
{
	return m_strClass;
}
AFX_INLINE BOOL CXTPSkinManager::IsEnabled() const
{
	return m_bEnabled;
}
AFX_INLINE CXTPSkinManagerResourceFile* CXTPSkinManager::GetResourceFile() const
{
	return m_pResourceFile;
}
AFX_INLINE CXTPSkinManagerSchema* CXTPSkinManager::GetSchema() const
{
	return m_pSchema;
}
AFX_INLINE DWORD CXTPSkinManager::GetApplyOptions() const
{
	return m_dwApplyOptions;
}
AFX_INLINE BOOL CXTPSkinManager::HasApplyOptions(DWORD dwOptions) const
{
	return (m_dwApplyOptions & dwOptions) == dwOptions;
}
AFX_INLINE BOOL CXTPSkinManager::IsComCtlV6() const
{
	return m_dwComCtrl >= MAKELONG(0, 6);
}
AFX_INLINE BOOL CXTPSkinManager::IsWin9x() const
{
	return m_bWin9x;
}
AFX_INLINE CXTPSkinObjectClassMap* CXTPSkinManager::GetClassMap() const
{
	return m_pClassMap;
}
AFX_INLINE BOOL CXTPSkinManager::GetAutoApplyNewWindows() const
{
	return m_bAutoApplyWindows;
}
AFX_INLINE BOOL CXTPSkinManager::GetAutoApplyNewThreads() const
{
	return m_bAutoApplyThreads;
}
AFX_INLINE void CXTPSkinManager::SetAutoApplyNewWindows(BOOL bAutoApply)
{
	m_bAutoApplyWindows = bAutoApply;
}
AFX_INLINE void CXTPSkinManager::SetAutoApplyNewThreads(BOOL bAutoApply)
{
	m_bAutoApplyThreads = bAutoApply;
}
AFX_INLINE void CXTPSkinManager::SetWindowFilter(IXTPSkinManagerWindowFilter* pFilter)
{
	m_pWindowFilter = pFilter;
}

#	ifndef _XTP_ACTIVEX

AFX_INLINE CXTPSkinManagerApiHookBase* CXTPSkinManager::GetApiHook() const
{
	return m_pApiHook;
}

#	endif

//---------------------------------------------------------------------------
// Summary:
//     Call this function to access CXTPSkinManager members.
//     Since this class is designed as a single instance object, you can
//     only access version info through this method. You <b>cannot</b>
//     directly instantiate an object of type CXTPSkinManager.
// Example:
// <code>
// XTPSkinManager()->SetApplyOptions(xtpSkinApplyMetrics | xtpSkinApplyFrame | xtpSkinApplyColors);
// </code>
//---------------------------------------------------------------------------
_XTP_EXT_CLASS CXTPSkinManager* AFX_CDECL XTPSkinManager();
_XTP_EXT_CLASS CXTPSkinManager* AFX_CDECL XTPSkinMgr();

//{{AFX_CODEJOCK_PRIVATE

_XTP_EXT_CLASS CXTPRWCriticalSection* AFX_CDECL XTPSkinGlobalAccess();

#	ifdef _AFXDLL

class _XTP_EXT_CLASS CXTPSkinScopeModuleStateHolder
{
	AFX_MODULE_STATE* m_pPrevState;

	CXTPSkinScopeModuleStateHolder(const CXTPSkinScopeModuleStateHolder&);
	CXTPSkinScopeModuleStateHolder& operator=(const CXTPSkinScopeModuleStateHolder&);

public:
	CXTPSkinScopeModuleStateHolder(AFX_MODULE_STATE* pNewState)
	{
		m_pPrevState = AfxSetModuleState(pNewState);
	}

	~CXTPSkinScopeModuleStateHolder()
	{
		AfxSetModuleState(m_pPrevState);
	}

	operator int()
	{
		return 0;
	}
};

#	endif /*_AFXDLL*/

#	define XTP_SKINFRAMEWORK_GLOBALLOCK_SHARED_SCOPE()                                            \
		XTP_RWCS_LOCK_SHARED_SCOPE(*XTPSkinGlobalAccess())
#	define XTP_SKINFRAMEWORK_GLOBALLOCK_EXCLUSIVE_SCOPE()                                         \
		XTP_RWCS_LOCK_EXCLUSIVE_SCOPE(*XTPSkinGlobalAccess())

#	define XTP_SKINFRAMEWORK_MANAGE_STATE()                                                       \
		AFX_MODULE_STATE* $__xtpSkinOrigModuleState = AfxGetModuleState();                         \
		UNREFERENCED_PARAMETER($__xtpSkinOrigModuleState);                                         \
		SAFE_MANAGE_STATE(XTPSkinManager()->m_pModuleState);

#	ifdef _AFXDLL
#		define XTP_SKINFRAMEWORK_ORIGINAL_STATE()                                                 \
			XTP_BLOCKSTATEMENT(CXTPSkinScopeModuleStateHolder, $__xtpSkinScopeModuleStateHolder,   \
							   $__xtpSkinOrigModuleState)
#	else
#		define XTP_SKINFRAMEWORK_ORIGINAL_STATE()
#	endif /*_AFXDLL*/

#	define XTP_SKINFRAMEWORK_ASSERT_WINDOW_THREAD(hWnd)                                           \
		ASSERT(::IsWindow(hWnd) ? GetWindowThreadProcessId(hWnd, NULL) == GetCurrentThreadId()     \
								: TRUE)

//}}AFX_CODEJOCK_PRIVATE

#	include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPSKINMANAGER_H__)
