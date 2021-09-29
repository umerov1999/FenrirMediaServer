// XTPSkinManagerSchema.h: interface for the CXTPSkinManagerSchema class.
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
#if !defined(_XTPSKINMANAGERSCHEMA_H__)
#	define _XTPSKINMANAGERSCHEMA_H__
//}}AFX_CODEJOCK_PRIVATE

#	if _MSC_VER > 1000
#		pragma once
#	endif // _MSC_VER > 1000

#	include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

class CXTPSkinManagerResourceFile;
class CXTPSkinManagerClass;
class CXTPSkinManager;
class CXTPSkinObjectFrame;
class CXTPSkinImage;
class CXTPSkinManagerMetrics;
class CXTPSkinManagerSchema;
struct XTP_SKINSCROLLBAR_POSINFO;

//#define _OLD_METHOD_GET_TMS_CODES_VERIFY

//===========================================================================
// Summary:
//     CXTPSkinManagerSchemaProperty is a standalone class representing a
//     single property of a visual styles .ini file.
//===========================================================================
class _XTP_EXT_CLASS CXTPSkinManagerSchemaProperty : public CXTPSynchronized
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPSkinManagerSchemaProperty object.
	//-----------------------------------------------------------------------
	CXTPSkinManagerSchemaProperty();

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPSkinManagerSchemaProperty object, handles cleanup and deallocation.
	//-----------------------------------------------------------------------
	~CXTPSkinManagerSchemaProperty();

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Sets the color value of the property.
	// Remarks:
	//     This method converts a string to a color value.
	// Parameters:
	//     lpszValue - String to be converted.
	//-----------------------------------------------------------------------
	void SetPropertyColor(LPCTSTR lpszValue);

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets the string value of the property.
	// Parameters:
	//     lpszValue - String to be set.
	//-----------------------------------------------------------------------
	void SetPropertyString(LPCTSTR lpszValue);

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets the BOOL value of the property.
	// Remarks:
	//     This method converts a string to a BOOL value.
	// Parameters:
	//     lpszValue - String to be converted.
	//-----------------------------------------------------------------------
	void SetPropertyBool(LPCTSTR lpszValue);

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets the int value of the property.
	// Remarks:
	//     This method converts a string to an int value.
	// Parameters:
	//     lpszValue - String to be converted.
	//-----------------------------------------------------------------------
	void SetPropertyInt(LPCTSTR lpszValue);

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets the enum value of the property.
	// Parameters:
	//     nEnumValue - Enum value to be set.
	//-----------------------------------------------------------------------
	void SetPropertyEnum(int nEnumValue);

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets the size value of the property.
	// Remarks:
	//     This method converts a string to a size value.
	// Parameters:
	//     lpszValue - String to be converted.
	//-----------------------------------------------------------------------
	void SetPropertySize(LPCTSTR lpszValue);

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets the RECT value of the property.
	// Remarks:
	//     This method converts a string to a RECT value.
	// Parameters:
	//     lpszValue - String to be converted.
	//-----------------------------------------------------------------------
	void SetPropertyRect(LPCTSTR lpszValue);

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets the LOGFONT value of the property.
	// Remarks:
	//     This method converts a string to a LOGFONT value.
	// Parameters:
	//     lpszValue - String to be converted.
	//-----------------------------------------------------------------------
	void SetPropertyFont(LPCTSTR lpszValue);

	//-----------------------------------------------------------------------
	// Summary:
	//     Clears the property value.
	//-----------------------------------------------------------------------
	void ClearProperty();

	BOOL operator==(const CXTPSkinManagerSchemaProperty& obj) const;

public:
	XTPSkinManagerProperty propType; // Property type.
	int nPropertyCode;				 // Property code.

	union
	{
		COLORREF clrVal; // Color value of the property.
		LPTSTR lpszVal;	 // LPCTSTR value of the property.
		BOOL bVal;		 // BOOL value of the property.
		int iVal;		 // Integer value of the property.
		RECT rcVal;		 // Rect value of the property.
		SIZE szVal;		 // SIZE value of the property.
		PLOGFONT lfVal;	 // LOGFONT value of the property.
	};
};

//===========================================================================
// Summary:
//     CXTPSkinManagerSchema is a standalone class used to read a
//     visual styles schema file.
//===========================================================================
class _XTP_EXT_CLASS CXTPSkinManagerSchema
	: public CXTPCmdTarget
	, public CXTPSynchronized
{
	DECLARE_DYNAMIC(CXTPSkinManagerSchema)

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPSkinManagerSchema object.
	// Parameters:
	//     pResourceFile - Resource file to read.
	//-----------------------------------------------------------------------
	CXTPSkinManagerSchema(CXTPSkinManagerResourceFile* pResourceFile);

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPSkinManagerSchema object, handles cleanup and deallocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPSkinManagerSchema();

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Reads all properties from a specified resource file.
	// Parameters:
	//     pResourceFile - Resource file to read.
	// Returns:
	//     S_OK if successful.
	//-----------------------------------------------------------------------
	HRESULT ReadProperties();

	//-----------------------------------------------------------------------
	// Summary:
	//     Gets the hash code of a specified class.
	// Parameters:
	//     strClass - Class name.
	// Returns:
	//     The hash code of the specified class.
	//-----------------------------------------------------------------------
	static UINT AFX_CDECL GetClassCode(LPCTSTR strClass);

#	ifdef _OLD_METHOD_GET_TMS_CODES_VERIFY
	static UINT AFX_CDECL GetClassCodeOld(LPCTSTR strClass);
#	endif

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Gets the value of a specified string property.
	// Parameters:
	//     iClassId - Class hash code.
	//     iPartId  - Part number.
	//     iStateId - State number of the part.
	//     iPropId  - Property number to get the value of.
	//     strVal   - Reference to a string to receive the value of
	//                the specified string property.
	// Returns:
	//     S_OK if successful.
	//-----------------------------------------------------------------------
	HRESULT GetStringProperty(UINT iClassId, int iPartId, int iStateId, int iPropId,
							  CString& strVal);

	//-----------------------------------------------------------------------
	// Summary:
	//     Gets the value of a specified RECT property.
	// Parameters:
	//     iClassId - Class hash code.
	//     iPartId  - Part number.
	//     iStateId - State number of the part.
	//     iPropId  - Property number to get the value of.
	//     rcVal    - Reference to a CRect object to receive the value of
	//                the specified RECT property.
	// Returns:
	//     S_OK if successful.
	//-----------------------------------------------------------------------
	HRESULT GetRectProperty(UINT iClassId, int iPartId, int iStateId, int iPropId, CRect& rcVal);

	//-----------------------------------------------------------------------
	// Summary:
	//     Gets the value of a specified int property.
	// Parameters:
	//     iClassId - Class hash code.
	//     iPartId  - Part number.
	//     iStateId - State number of the part.
	//     iPropId  - Property number to get the value of.
	//     iVal     - Reference to an int to receive the value of
	//                the specified int property.
	// Returns:
	//     S_OK if successful.
	//-----------------------------------------------------------------------
	HRESULT GetIntProperty(UINT iClassId, int iPartId, int iStateId, int iPropId, int& iVal);

	//-----------------------------------------------------------------------
	// Summary:
	//     Gets the value of a specified BOOL property.
	// Parameters:
	//     iClassId - Class hash code.
	//     iPartId  - Part number.
	//     iStateId - State number of the part.
	//     iPropId  - Property number to get the value of.
	//     bVal     - Reference to a BOOL to receive the value of
	//                the specified BOOL property.
	// Returns:
	//     S_OK if successful.
	//-----------------------------------------------------------------------
	HRESULT GetBoolProperty(UINT iClassId, int iPartId, int iStateId, int iPropId, BOOL& bVal);

	//-----------------------------------------------------------------------
	// Summary:
	//     Gets the value of a specified COLORREF property.
	// Parameters:
	//     iClassId - Class hash code.
	//     iPartId  - Part number.
	//     iStateId - State number of the part.
	//     iPropId  - Property number to get the value of.
	//     clrVal   - Reference to a COLORREF to receive the value of
	//                the specified COLORREF property.
	// Returns:
	//     S_OK if successful.
	//-----------------------------------------------------------------------
	HRESULT GetColorProperty(UINT iClassId, int iPartId, int iStateId, int iPropId,
							 COLORREF& clrVal);

	//-----------------------------------------------------------------------
	// Summary:
	//     Gets the value of a specified enum property.
	// Parameters:
	//     iClassId - Class hash code.
	//     iPartId  - Part number.
	//     iStateId - State number of the part.
	//     iPropId  - Property number to get the value of.
	//     nVal     - Reference to an int to receive the value of
	//                the specified enum property.
	// Returns:
	//     S_OK if successful.
	//-----------------------------------------------------------------------
	HRESULT GetEnumProperty(UINT iClassId, int iPartId, int iStateId, int iPropId, int& nVal);

	//-----------------------------------------------------------------------
	// Summary:
	//     Gets the value of a specified LOGFONT property.
	// Parameters:
	//     iClassId - Class hash code.
	//     iPartId  - Part number.
	//     iStateId - State number of the part.
	//     iPropId  - Property number to get the value of.
	//     lfVal    - Reference to a LOGFONT to receive the value of
	//                the specified LOGFONT property.
	// Returns:
	//     S_OK if successful.
	//-----------------------------------------------------------------------
	HRESULT GetFontProperty(UINT iClassId, int iPartId, int iStateId, int iPropId, LOGFONT& lfVal);

	//-----------------------------------------------------------------------
	// Summary:
	//     Gets the value of a specified CSize property.
	// Parameters:
	//     iClassId - Class hash code.
	//     iPartId  - Part number.
	//     iStateId - State number of the part.
	//     iPropId  - Property number to get the value of.
	//     szVal    - Reference to a CSize to receive the value of
	//                the specified CSize property value.
	// Returns:
	//     S_OK if successful.
	//-----------------------------------------------------------------------
	HRESULT GetSizeProperty(UINT iClassId, int iPartId, int iStateId, int iPropId, CSize& szVal);

	//-----------------------------------------------------------------------
	// Summary:
	//     Gets the schema name.
	// Returns:
	//     The schema name.
	//-----------------------------------------------------------------------
	CString GetSchemaName() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Gets the parent skin manager object.
	// Returns:
	//     A pointer to the parent skin manager object.
	//-----------------------------------------------------------------------
	CXTPSkinManager* GetSkinManager() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Gets the resource file object.
	// Returns:
	//     A pointer to the resource file object.
	//-----------------------------------------------------------------------
	CXTPSkinManagerResourceFile* GetResourceFile() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Gets the metrics object.
	// Returns:
	//     A pointer to the metrics object.
	//-----------------------------------------------------------------------
	CXTPSkinManagerMetrics* GetMetrics() const;

	//{{AFX_CODEJOCK_PRIVATE

	void RefreshAllClasses();
	CXTPSkinManagerSchemaProperty* GetProperty(UINT iClassId, int iPartId, int iStateId,
											   int iPropId);
	CXTPSkinManagerClass* GetClass(const CString& strClassList);

protected:
	enum FRAMEPART
	{
		frameLeft,
		frameTop,
		frameRight,
		frameBottom
	};

	struct FRAMEREGIONPART
	{
		FRAMEREGIONPART()
		{
			nCenter = 0;
			rcSizingMargins.SetRectEmpty();
		}

		CRgn rgnLeft;
		CRgn rgnRight;
		CRect rcSizingMargins;
		int nCenter;
	};

	struct FRAMEREGION
	{
		int nCaptionHeight;
		int nBorderHeight;
		BOOL bToolWindow;
		BOOL bAutoDelete;

		FRAMEREGIONPART Part[4];
	};

public:
	HICON GetFrameSmIcon(HWND hWnd, BOOL bCheckStyle = TRUE) const;

public:
	static UINT AFX_CDECL GetSchemaPartCode(CString strSchemaPart);
	static UINT AFX_CDECL GetClassPartCode(LPCTSTR lpszClass, LPCTSTR lpszClassPart);

#	ifdef _OLD_METHOD_GET_TMS_CODES_VERIFY

	static UINT AFX_CDECL GetClassPartCodeOld(LPCTSTR lpszClass, LPCTSTR lpszClassPart);

#	endif // _OLD_METHOD_GET_TMS_CODES_VERIFY

	static UINT AFX_CDECL GetStateCode(LPCTSTR lpszClassPart, LPCTSTR lpszState);
	static int AFX_CDECL GetEnumCode(LPCTSTR lpszProperty, LPCTSTR lpszValue);

#	ifdef _OLD_METHOD_GET_TMS_CODES_VERIFY
	static UINT AFX_CDECL GetStateCodeOld(LPCTSTR lpszClassPart, LPCTSTR lpszState);
	static int AFX_CDECL GetEnumCodeOld(LPCTSTR lpszProperty, LPCTSTR lpszValue);
	static UINT AFX_CDECL GetPropertyCodeOld(LPCTSTR lpszProperty,
											 XTPSkinManagerProperty& nPropertyType);
#	endif // _OLD_METHOD_GET_TMS_CODES_VERIFY

	static UINT AFX_CDECL GetPropertyCode(LPCTSTR lpszProperty,
										  XTPSkinManagerProperty& nPropertyType);
	static CXTPSkinManagerSchemaProperty* AFX_CDECL CreateProperty(
		LPCTSTR lpszProperty, XTPSkinManagerProperty nPropertyType, LPCTSTR lpszValue);
	static UINT AFX_CDECL CalculatePropertyCode(UINT iClassId, int iPartId, int iStateId,
												int iPropId);

protected:
	void RemoveAllProperties();
	COLORREF GetColor(int nIndex) const;
	//}}AFX_CODEJOCK_PRIVATE

public:
	//{{AFX_CODEJOCK_PRIVATE
	virtual BOOL DrawThemeBackground(CDC* pDC, CXTPSkinManagerClass* pClass, int iPartId,
									 int iStateId, const RECT* pRect);
	virtual void DrawThemeFrame(CDC* pDC, CXTPSkinObjectFrame* pFrame);
	virtual CRect CalcFrameBorders(CXTPSkinObjectFrame* pFrame);
	virtual CRect CalcFrameBorders(DWORD dwStyle, DWORD dwExStyle);
	virtual HRGN CalcFrameRegion(CXTPSkinObjectFrame* pFrame, CSize sz);
	virtual void RefreshMetrcis();
	void DrawNonClientRect(CDC* pDC, CRect rcFrame, CXTPSkinObjectFrame* pFrame);
	void DrawClientEdge(CDC* pDC, const CRect& rcFrame, CXTPSkinObjectFrame* pFrame);
	virtual void DrawThemeScrollBar(CDC* pDC, CXTPSkinObjectFrame* pFrame,
									XTP_SKINSCROLLBAR_POSINFO* pSBInfo);
	virtual COLORREF GetScrollBarSizeBoxColor(CXTPSkinObjectFrame* pFrame);
	//}}AFX_CODEJOCK_PRIVATE

protected:
	//{{AFX_CODEJOCK_PRIVATE
	BOOL DrawThemeBackgroundGlyph(CDC* pDC, CXTPSkinManagerClass* pClass, int iPartId, int iStateId,
								  const RECT* pRect);
	BOOL DrawThemeBackgroundBorder(CDC* pDC, CXTPSkinManagerClass* pClass, int iPartId,
								   int iStateId, const RECT* pRect);
	int FindBestImageGlyphSize(CXTPSkinManagerClass* pClass, int iPartId, int iStateId,
							   const CRect& rcDest, int nImageCount, BOOL bHorizontalImageLayout);

#	ifdef _XTP_ACTIVEX
	COLORREF OleGetColor(long Index);
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
	DECLARE_OLETYPELIB_EX(CXTPSkinManagerSchema);
#	endif
	//}}AFX_CODEJOCK_PRIVATE

private:
	int GetWindowBorders(LONG lStyle, DWORD dwExStyle, BOOL fWindow);
	void FreeFrameRegions();
	void RegionFromBitmap(CBitmap* pBitmap, FRAMEPART nAlign, FRAMEREGIONPART* pPart,
						  const CRect& rc, COLORREF clrTransparent);

	FRAMEREGION* CreateFrameRegion(CXTPSkinObjectFrame* pFrame, CSize sz);
	CSize GetCaptionButtonSize(CXTPSkinManagerClass* pClass, int yButton);
	BOOL DrawWindowPart(CXTPSkinObjectFrame* pFrame, CBitmap& bmp, int iPartId, BOOL bVertical,
						int nBorderHeight, CRect& rcDest, CRect& rcSizingMargins,
						COLORREF& clrTransparent);
	int DrawThemeFrameButtons(CDC* pDC, CXTPSkinObjectFrame* pFrame);

	CRect CalcFrameBorders(DWORD dwStyle, DWORD dwExStyle, CXTPSkinObjectFrame* pFrame);

public:
	BOOL m_bPreMultiplyImages;

protected:
	CMap<UINT, UINT, CXTPSkinManagerSchemaProperty*, CXTPSkinManagerSchemaProperty*>
		m_mapProperties; // Properties collection.

	CXTPSkinManager* m_pManager;				  // Parent CXTPSkinManager class.
	CXTPSkinManagerResourceFile* m_pResourceFile; // Resources.
	CXTPSkinManagerMetrics* m_pMetrics;			  // Skin metrics.

	CString m_strSchemaName; // Schema name.
	CRITICAL_SECTION m_csDraw;

	CArray<FRAMEREGION*, FRAMEREGION*> m_arrFrameRegions; // Frame region.

protected:
	int m_nGlobalClassId;
	int m_nWindowClassId;

	CMapStringToPtr m_mapClasses; // Classes collection.
};

AFX_INLINE CString CXTPSkinManagerSchema::GetSchemaName() const
{
	return m_strSchemaName;
}
AFX_INLINE CXTPSkinManager* CXTPSkinManagerSchema::GetSkinManager() const
{
	return m_pManager;
}
AFX_INLINE CXTPSkinManagerResourceFile* CXTPSkinManagerSchema::GetResourceFile() const
{
	return m_pResourceFile;
}
AFX_INLINE CXTPSkinManagerMetrics* CXTPSkinManagerSchema::GetMetrics() const
{
	return m_pMetrics;
}

#	include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(AFX_SKINMANAGERSCHEMA_H__44A8B96F_B15B_43F7_A734_39C649784B29__INCLUDED_)
