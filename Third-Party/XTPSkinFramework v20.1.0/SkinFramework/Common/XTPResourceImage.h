// XTPResourceImage.h: interface for the CXTPResourceImage class.
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
#if !defined(__XTPRESOURCEIMAGE_H__)
#	define __XTPRESOURCEIMAGE_H__
//}}AFX_CODEJOCK_PRIVATE

#	if _MSC_VER > 1000
#		pragma once
#	endif // _MSC_VER > 1000

#	include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

// Composite image flag for CXTPResourceImage::DrawImage
#	define XTP_DI_COMPOSITE 1

class CXTPResourceImages;
class CXTPResourceImageList;
class CXTPNotifyConnection;
class CXTPBitmapDC;

//=======================================================================
// Summary:
//      Represent a resource image and provides basic images functions.
// See also:
//      CXTPResourceImages
//=======================================================================
class _XTP_EXT_CLASS CXTPResourceImage : public CXTPCmdTarget
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//      Constructs resource image which is a part of image collection.
	// Parameters:
	//      pImages - A valid pointer to image collection the image belongs to.
	//-----------------------------------------------------------------------
	CXTPResourceImage(CXTPResourceImages* pImages);

	//-----------------------------------------------------------------------
	// Summary:
	//      Handles resource image deallocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPResourceImage();

public:
	//-----------------------------------------------------------------------
	// Summary:
	//      Loads image from the resource specified.
	// Parameters:
	//      hModule - Resource module handle.
	//      lpszBitmapFileName - PNG or bitmap resource identifier to load from
	//                           "PNG" or RT_BITMAP resource directories respectively.
	// Returns:
	//      TRUE if an image is load, FALSE otherwise.
	// See also:
	//      SetBitmap
	//-----------------------------------------------------------------------
	BOOL LoadFile(HMODULE hModule, LPCTSTR lpszBitmapFileName);

	//-----------------------------------------------------------------------
	// Summary:
	//      Loads image bits from the bitmap provided. The bitmap handle is
	//      destroyed upon successful completion.
	// Parameters:
	//      hBitmap - Bitmap handle to load image bits from.
	//      bAlpha - TRUE causes the bitmap to be processed as a 32-bit image with alpha channel.
	// See also:
	//      LoadFile
	//-----------------------------------------------------------------------
	void SetBitmap(HBITMAP hBitmap, BOOL bAlpha = FALSE);

	//-----------------------------------------------------------------------
	// Summary:
	//      Draws an image to the device context provided.
	// Parameters:
	//      pDC - A valid pointer to the target device context.
	//      rcDest - Destination rectangle.
	//      rcSrc - Source rectangle.
	//      rcSizingMargins - Margins.
	//      clrTransparent - A color key to be treated as transparent color.
	//      dwFlags - If contains XTP_DI_COMPOSITE bit, the image will be drawn as composited.
	//-----------------------------------------------------------------------
	void DrawImage(CDC* pDC, const CRect& rcDest, CRect rcSrc);
	void DrawImage(CDC* pDC, const CRect& rcDest, CRect rcSrc,
				   CRect rcSizingMargins); // <combine CXTPResourceImage::DrawImage@CDC*@const
										   // CRect&@CRect>
	void DrawImage(CDC* pDC, const CRect& rcDest, CRect rcSrc, const CRect& rcSizingMargins,
				   COLORREF clrTransparent); // <combine CXTPResourceImage::DrawImage@CDC*@const
											 // CRect&@CRect>
	void DrawImage(CDC* pDC, const CRect& rcDest, CRect rcSrc, const CRect& rcSizingMargins,
				   COLORREF clrTransparent,
				   DWORD dwFlags); // <combine CXTPResourceImage::DrawImage@CDC*@const CRect&@CRect>

	//-----------------------------------------------------------------------
	// Summary:
	//      Determine image dimensinsions.
	// Returns:
	//      The width, height or combined size of the image.
	//-----------------------------------------------------------------------
	int GetHeight() const;
	int GetWidth() const;	 // <combine CXTPResourceImage::GetHeight>
	CSize GetExtent() const; // <combine CXTPResourceImage::GetHeight>

	//-----------------------------------------------------------------------
	// Summary:
	//      Determines whether the image is a 32-bit image with alpha channel.
	// Returns:
	//      TRUE if the image is a 32-bit image with alpha channel.
	//-----------------------------------------------------------------------
	BOOL IsAlphaImage() const;

	//-----------------------------------------------------------------------
	// Summary:
	//      Enables preserved image orientation for RTL contexts and inverted drawing.
	// See also:
	//      DrawImageFlipped, CXTPDrawHelpers::IsContextRTL
	//-----------------------------------------------------------------------
	void PreserveOrientation();

	//-----------------------------------------------------------------------
	// Summary:
	//      Obtains a source rectable for an image in image list.
	// Parameters:
	//      nState - Stage/image index in the image list.
	//      nCount - Total number of images in the image list.
	// Returns:
	//      The computed image rectangle.
	//-----------------------------------------------------------------------
	CRect GetSource(int nState = 0, int nCount = 1) const;

	//-----------------------------------------------------------------------
	// Summary:
	//      Obtains pixel color value at the point specified.
	// Parameters:
	//      pt - Pixel coordinates.
	//      clrPixel - The output pixel color value.
	// Returns:
	//      TRUE if the color is successfully determined, FALSE otherwise.
	//-----------------------------------------------------------------------
	BOOL GetBitmapPixel(CPoint pt, COLORREF& clrPixel);

	//-----------------------------------------------------------------------
	// Summary:
	//      Pre-multiplies alpha image bits.
	//-----------------------------------------------------------------------
	void Premultiply();

	//-----------------------------------------------------------------------
	// Summary:
	//      Enables/disable image flipping.
	// Parameters:
	//      bFlip - TRUE inverts the image, FALSE restores original image.
	//-----------------------------------------------------------------------
	void DrawImageFlipped(BOOL bFlip);

protected:
	//-----------------------------------------------------------------------
	// Summary:
	//      Draws image part.
	// Parameters:
	//      pDCDest - A valid pointer to the target device context.
	//      rcDest - Target rectangle.
	//      pDCSrc - A valid pointer to the source device context.
	//      rcSrc - Source rectangle.
	// Returns:
	//      TRUE if drawing is successful, FALSE otherwise.
	//-----------------------------------------------------------------------
	BOOL DrawImagePart(CDC* pDCDest, CRect rcDest, CDC* pDCSrc, CRect rcSrc) const;

	//-----------------------------------------------------------------------
	// Summary:
	//      Inverts image bits.
	//-----------------------------------------------------------------------
	void InvertBitmap();

	//-----------------------------------------------------------------------
	// Summary:
	//      Obtains bitmap bits and uses them as image bits for the current image.
	// Parameters:
	//      hBitmap - Source bitmap handle.
	// Returns:
	//      TRUE if successfully converted, FALSE otherwise.
	//-----------------------------------------------------------------------
	BOOL ConvertToBits(HBITMAP hBitmap);

protected:
	LPVOID m_pBits;					  // Image bits
	BOOL m_bAlpha;					  // TRUE for 32-bit alpha images.
	BOOL m_bMultiply;				  // TRUE if an alpha image bits were multiplied.
	BOOL m_bImageOrientaionPreserved; // TRUE if image orientation is to be preserved for RTL or
									  // flipped drawing.
	BOOL m_bInvert;					  // TRUE indicates the images bits are currently inverted.
	CSize m_szExtent;				  // Image size.
	BOOL m_bFlipped;				  // TRUE m_bFlipped the image bits are currently flipped.
	CXTPResourceImages* m_pImages;	  // Parent image list pointer.

	friend class CXTPResourceImages;
	friend class CXTPResourceImageList;
};

//=======================================================================
// Summary:
//      Implements resource image collection and its operations.
// See also:
//      CXTPResourceImage
//=======================================================================
class _XTP_EXT_CLASS CXTPResourceImages : public CXTPCmdTarget
{
	friend class CXTPSingleton<CXTPResourceImages>;

public:
	//-----------------------------------------------------------------------
	// Summary:
	//      Construct an empty resource image collection.
	//-----------------------------------------------------------------------
	CXTPResourceImages();

	//-----------------------------------------------------------------------
	// Summary:
	//      Handles resource image list deallocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPResourceImages();

public:
	//-----------------------------------------------------------------------
	// Summary:
	//      Removes all loaded images and clears all loaded properties.
	// See also:
	//      RemoveAll
	//-----------------------------------------------------------------------
	void Reset();

	//-----------------------------------------------------------------------
	// Summary:
	//      Removes all loaded images only. Loaded properties remain valid.
	// See also:
	//      Reset
	//-----------------------------------------------------------------------
	void RemoveAll();

	//-----------------------------------------------------------------------
	// Summary:
	//      Loads image from resources and adds it to the collection.
	// Parameters:
	//      lpszImageFile - Image file name as used in resources. Symbols '.' and '\'
	//                      get replaced with '_'.
	// Returns:
	//      A loaded image pointer or NULL if unble to load an image.
	// See also:
	//      LoadImage
	//-----------------------------------------------------------------------
	CXTPResourceImage* LoadFile(LPCTSTR lpszImageFile);

	//-----------------------------------------------------------------------
	// Summary:
	//      Loads image from resource.
	// Parameters:
	//      lpszImageFile - Image file name as used in resources. Symbols '.' and '\'
	//                      get replaced with '_'.
	// Returns:
	//      A loaded image pointer or NULL if unble to load an image.
	// See also:
	//      LoadFile
	//-----------------------------------------------------------------------
	CXTPResourceImage* LoadImage(LPCTSTR lpszImageFile);

	//-----------------------------------------------------------------------
	// Summary:
	//      Obtains resource handle value.
	// Returns:
	//      Resource handle value.
	// See also:
	//      SetHandle
	//-----------------------------------------------------------------------
	HMODULE GetHandle() const;

	//-----------------------------------------------------------------------
	// Summary:
	//      Sets image collection resource handle.
	// Parameters:
	//      lpszDllFileName   - DLL file name.
	//      hResource         - Module handle to be used for resources.
	//      lpszIniFileName   - Optional INI resource file name from TEXTFILE
	//                          resource directory to be used for loading
	//                          file specific properties.
	//      lpszThemeFileName - Name of initialization file of the theme
	//      lpszAppFileName   - Name of initialization file of the application
	//      bFreeOnRelease    - If TRUE, the module specified by hResource handle
	//                          will be unloaded on releasing.
	// Returns:
	//      TRUE if resource handle is successfully set and resource loaded.
	// See also:
	//      SetIniData
	//-----------------------------------------------------------------------
	BOOL SetHandle(LPCTSTR lpszDllFileName, LPCTSTR lpszIniFileName = NULL);
	BOOL SetHandle(HMODULE hResource, LPCTSTR lpszIniFileName = NULL,
				   BOOL bFreeOnRelease = FALSE); // <combine
												 // CXTPResourceImages::SetHandle@LPCTSTR@LPCTSTR>
	BOOL SetHandle(HMODULE hResource, LPCTSTR lpszThemeFileName, LPCTSTR lpszAppFileName,
				   BOOL bFreeOnRelease = FALSE); // <combine
												 // CXTPResourceImages::SetHandle@LPCTSTR@LPCTSTR>

	//-----------------------------------------------------------------------
	// Summary:
	//      Loads INI file properties from the provided input.
	// Parameters:
	//      lpIniData - INI file contents to load.
	// See also:
	//      SetHandle
	//-----------------------------------------------------------------------
	void SetIniData(LPCSTR lpIniData);

	//-----------------------------------------------------------------------
	// Summary:
	//      Searches for a module that contains the resource specified and
	//      uses it as a resource module.
	// Parameters:
	//      lpszTestImageFile - Test image resource identifier.
	//      lpResType         - Test image resource type.
	// Returns:
	//      TRUE if the resource specified is found and its module is used.
	// See also:
	//      SetIniData, SetHandle
	//-----------------------------------------------------------------------
	BOOL InitResourceHandle(LPCTSTR lpszTestImageFile, LPCTSTR lpResType = RT_BITMAP);

	//-----------------------------------------------------------------------
	// Summary:
	//      Obtains color value from the specific key of the currently loaded INI file.
	// Parameters:
	//      lpszSectionName - INI section name.
	//      lpszKeyName     - INI key name that contains color value.
	//      clrDefault      - Default color value to be used.
	// Returns:
	//      The obtained or default color value.
	// See also:
	//      GetImageInt, GetImageRect, GetImageSize, GetImageString, GetImageValue
	//-----------------------------------------------------------------------
	COLORREF GetImageColor(LPCTSTR lpszSectionName, LPCTSTR lpszKeyName,
						   COLORREF clrDefault = (COLORREF)-1) const;

	//-----------------------------------------------------------------------
	// Summary:
	//      Obtains integer value from the specific key of the currently loaded INI file.
	// Parameters:
	//      lpszSectionName - INI section name.
	//      lpszKeyName     - INI key name that contains integer value.
	//      nDefault        - Default integer value to be used.
	// Returns:
	//      The obtained or default integer value.
	// See also:
	//      GetImageColor, GetImageRect, GetImageSize, GetImageString, GetImageValue
	//-----------------------------------------------------------------------
	int GetImageInt(LPCTSTR lpszSectionName, LPCTSTR lpszKeyName, int nDefault = -1) const;

	//-----------------------------------------------------------------------
	// Summary:
	//      Obtains rectangle value from the specific key of the currently loaded INI file.
	// Parameters:
	//      lpszSectionName - INI section name.
	//      lpszKeyName     - INI key name that contains rectangle value.
	//      rcDefault       - Default rectangle value to be used.
	// Returns:
	//      The obtained or default rectangle value.
	// See also:
	//      GetImageColor, GetImageInt, GetImageSize, GetImageString, GetImageValue
	//-----------------------------------------------------------------------
	CRect GetImageRect(LPCTSTR lpszSectionName, LPCTSTR lpszKeyName,
					   CRect rcDefault = CRect(0, 0, 0, 0)) const;

	//-----------------------------------------------------------------------
	// Summary:
	//      Obtains size value from the specific key of the currently loaded INI file.
	// Parameters:
	//      lpszSectionName - INI section name.
	//      lpszKeyName     - INI key name that contains size value.
	//      szDefault       - Default size value to be used.
	// Returns:
	//      The obtained or default size value.
	// See also:
	//      GetImageColor, GetImageInt, GetImageRect, GetImageString, GetImageValue
	//-----------------------------------------------------------------------
	CSize GetImageSize(LPCTSTR lpszSectionName, LPCTSTR lpszKeyName,
					   CSize szDefault = CSize(0, 0)) const;

	//-----------------------------------------------------------------------
	// Summary:
	//      Obtains string value from the specific key of the currently loaded INI file.
	// Parameters:
	//      lpszSectionName - INI section name.
	//      lpszKeyName     - INI key name that contains string value.
	//      lpszDef         - Default string value to be used.
	// Returns:
	//      The obtained or default string value.
	// See also:
	//      GetImageColor, GetImageInt, GetImageRect, GetImageSize, GetImageValue
	//-----------------------------------------------------------------------
	CString GetImageString(LPCTSTR lpszSectionName, LPCTSTR lpszKeyName,
						   LPCTSTR lpszDef = NULL) const;

	//-----------------------------------------------------------------------
	// Summary:
	//      Obtains string value from the specific key of any type of the currently loaded INI file.
	// Parameters:
	//      lpszSectionName - INI section name.
	//      lpszKeyName     - INI key name that contains string value.
	// Returns:
	//      The obtained or default string value.
	// See also:
	//      GetImageColor, GetImageInt, GetImageRect, GetImageString, GetImageSize
	//-----------------------------------------------------------------------
	CString GetImageValue(LPCTSTR lpszSectionName, LPCTSTR lpszKeyName) const;

	//-----------------------------------------------------------------------
	// Summary:
	//      Determines if image collection state is valid, i.e. a resource module
	//      is used and all properties loaded.
	//      loaded.
	// Returns:
	//      TRUE if collection is in a valid state.
	// See also:
	//      SetHandle
	//-----------------------------------------------------------------------
	BOOL IsValid() const;

	//{{AFX_CODEJOCK_PRIVATE
	// Retained for backward compatibility
	void AssertValid() const;
	//}}AFX_CODEJOCK_PRIVATE

	//-----------------------------------------------------------------------
	// Summary:
	//      Loads a bitmap from the currently assigned resource module.
	// Parameters:
	//      pBitmap - A valid pointer to the bitmap object that will be loaded
	//                upon successful completion.
	//      lpszResourceName - Bitmap resource identifier to load.
	// Returns:
	//      TRUE if loaded successfully, FALSE otherwise.
	//-----------------------------------------------------------------------
	BOOL LoadBitmap(CBitmap* pBitmap, LPCTSTR lpszResourceName);

	//-----------------------------------------------------------------------
	// Summary:
	//      Return connection object pointer that can be used for subscribing
	//      to collection events.
	// Returns:
	//      Return connection object pointer.
	//-----------------------------------------------------------------------
	CXTPNotifyConnection* GetConnection();

	CString m_strDllFileName; // Loaded DLL file name
	CString m_strIniFileName; // Loaded INI file name

public:
	//{{AFX_CODEJOCK_PRIVATE
	BOOL m_bLoadedNothing;
	//}}AFX_CODEJOCK_PRIVATE

protected:
	//-----------------------------------------------------------------------
	// Summary:
	//      Converts file name to resource file name, Symbols '.' and '\'
	//      get replaced with '_'.
	// Parameters:
	//      strImageFile - File name pointer to convert.
	// Returns:
	//      Converted resource file name value.
	//-----------------------------------------------------------------------
	CString _ImageNameToResourceName(LPCTSTR strImageFile);

	//-----------------------------------------------------------------------
	// Summary:
	//      Clears all loaded INI properties.
	//-----------------------------------------------------------------------
	void ClearProperties();

	//-----------------------------------------------------------------------
	// Summary:
	//      Loads INI properties from the INI text range.
	// Parameters:
	//      lpTextFile      - INI data start pointer.
	//      lpTextFileEnd   - INI data end pointer.
	//      bAllowOverwrite - TRUE if it is allowed to overwrite existing properties.
	//-----------------------------------------------------------------------
	void LoadProperties(LPSTR lpTextFile, LPSTR lpTextFileEnd, BOOL bAllowOverwrite);

private:
	BOOL ReadString(CString& str, LPSTR& lpTextFile, LPSTR lpTextFileEnd);

	BOOL LoadResources(LPCTSTR lpszIniFileName);
	BOOL LoadResources(HRSRC hRsrc, BOOL bAllowOverwrite);

	static BOOL CALLBACK EnumResNameProc(HMODULE hModule, LPCTSTR lpszType, LPTSTR lpszName,
										 LONG_PTR lParam);
	HRSRC FindResourceIniFile(HMODULE hResource);

	UINT GetKeyHash(LPCTSTR lpszSection, LPCTSTR lpszKeyName) const;
	HBITMAP AllocateLayerBitmap(UINT cx, UINT cy, LPDWORD* lpBits = NULL, SIZE* pSize = NULL);

protected:
	HMODULE m_hResource;   // Associated resource module handle.
	BOOL m_bFreeOnRelease; // If TRUE, the module determined by m_hResource will be unloaded upon
						   // release.
	CRITICAL_SECTION m_cs; // Shared access critical section.
	BOOL m_bPremultiplyImages; // If TRUE, the bits of alpha images are to be pre-multiplied.
	CXTPBitmapDC* m_pdc; // An off-screen device context pointer used for internal image processing.
	HBITMAP m_hbmLayer;	 // Bitmap layer handle.
	LPDWORD m_pLayerBits;							   // Bitmap layer bits pointer.
	CXTPNotifyConnection* m_pConnection;			   // Connection sink object pointer.
	CMap<UINT, UINT, CString, CString> m_mapPropeties; // Loaded INI properties.
	CMapStringToPtr m_mapImages;					   // Loaded image collection.

#	ifdef _XTP_ACTIVEX
	//{{AFX_CODEJOCK_PRIVATE

	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()

	DECLARE_OLETYPELIB_EX(CXTPResourceImages);

	BOOL OleLoadFromFile(LPCTSTR lpszPath, LPCTSTR lpszIniFileName);
	virtual void OnFinalRelease();
	BSTR OleGetIniFileName();
	BSTR OleGetDllFileName();
	OLE_COLOR OleGetGetColor(LPCTSTR lpszSection, LPCTSTR lpszEntry);

	//}}AFX_CODEJOCK_PRIVATE
#	endif

	friend class CXTPResourceImage;
};

//-----------------------------------------------------------------------
// Summary: Call to access members of the CXTPResourceImages class.
// Returns: Returns the a pointer to the single instance of the
//          CXTPResourceImages class.
//-----------------------------------------------------------------------
_XTP_EXT_CLASS CXTPResourceImages* AFX_CDECL XTPResourceImages();

//-----------------------------------------------------------------------
// Summary:    Call to access members of the CXTPResourceImage class.
// Parameters: lpszImageFile - Name of the image file to load specified in the theme INI.
// Returns:    Returns the a pointer to the single instance of the
//             CXTPResourceImage class.
//-----------------------------------------------------------------------
_XTP_EXT_CLASS CXTPResourceImage* AFX_CDECL XTPLoadImage(LPCTSTR lpszImageFile);

//-----------------------------------------------------------------------
// Summary:    Call to get the color value specified by lpszKey from the
//             current theme.
// Parameters: lpszSection - Name of the section in the theme INI file.
//             lpszKey     - Name of the value that defines the specified value.
//             clrDef      - Default value returned if lpszKey was not found.
// Returns:    A value represting the color defined by lpszKey.
//-----------------------------------------------------------------------
_XTP_EXT_CLASS COLORREF AFX_CDECL XTPIniColor(LPCTSTR lpszSection, LPCTSTR lpszKey,
											  COLORREF clrDef);

//-----------------------------------------------------------------------
// Summary:    Call to get the int value specified by lpszKey from the
//             current theme.
// Parameters: lpszSection - Name of the section in the theme INI file.
//             lpszKey     - Name of the value that defines the specified value.
//             nDef        - Default value returned if lpszKey was not found.
// Returns:    A value represting the int defined by lpszKey.
//-----------------------------------------------------------------------
_XTP_EXT_CLASS int AFX_CDECL XTPIniInt(LPCTSTR lpszSection, LPCTSTR lpszKey, int nDef);

//-----------------------------------------------------------------------
// Summary:    Call to get the CRect value specified by lpszKey from the
//             current theme.
// Parameters: lpszSection - Name of the section in the theme INI file.
//             lpszKey     - Name of the value that defines the specified value.
//             rcDef       - Default value returned if lpszKey was not found.
// Returns:    A value represting the CRect defined by lpszKey.
//-----------------------------------------------------------------------
_XTP_EXT_CLASS CRect AFX_CDECL XTPIniRect(LPCTSTR lpszSection, LPCTSTR lpszKey, CRect rcDef);

//-----------------------------------------------------------------------
// Summary:    Call to get the CSize value specified by lpszKey from the
//             current theme.
// Parameters: lpszSection - Name of the section in the theme INI file.
//             lpszKey     - Name of the value that defines the specified value.
//             szDef       - Default value returned if lpszKey was not found.
// Returns:    A value represting the CSize defined by lpszKey.
//-----------------------------------------------------------------------
_XTP_EXT_CLASS CSize AFX_CDECL XTPIniSize(LPCTSTR lpszSection, LPCTSTR lpszKey, CSize szDef);

//-----------------------------------------------------------------------
// Summary:    Call to get the CString value specified by lpszKey from the
//             current theme.
// Parameters: lpszSection - Name of the section in the theme INI file.
//             lpszKey     - Name of the value that defines the specified value.
//             lpszDef     - Default value returned if lpszKey was not found.
// Returns:    A value represting the CString defined by lpszKey.
//-----------------------------------------------------------------------
_XTP_EXT_CLASS CString AFX_CDECL XTPIniString(LPCTSTR lpszSection, LPCTSTR lpszKey,
											  LPCTSTR lpszDef);

// -----------------------------------------
// Use with CXTPResourceImages::SetHandle().
// -----------------------------------------

// Office2007.dll
#	define xtpIniOffice2007Aqua                                                                   \
		_T("OFFICE2007AQUA_INI") // Specifies Office 2007 Aqua settings file.
#	define xtpIniOffice2007Blue                                                                   \
		_T("OFFICE2007BLUE_INI") // Specifies Office 2007 Blue settings file.
#	define xtpIniOffice2007Black                                                                  \
		_T("OFFICE2007BLACK_INI") // Specifies Office 2007 Black settings file.
#	define xtpIniOffice2007Silver                                                                 \
		_T("OFFICE2007SILVER_INI") // Specifies Office 2007 Silver settings file.

// Office2010.dll
#	define xtpIniOffice2010Blue                                                                   \
		_T("OFFICE2010BLUE_INI") // Specifies Office 2010 Blue settings file.
#	define xtpIniOffice2010Black                                                                  \
		_T("OFFICE2010BLACK_INI") // Specifies Office 2010 Black settings file.
#	define xtpIniOffice2010Silver                                                                 \
		_T("OFFICE2010SILVER_INI") // Specifies Office 2010 Silver settings file.

// Office2013.dll
#	define xtpIniOffice2013Access                                                                 \
		_T("OFFICE2013ACCESS_INI") // Specifies Office 2013 Access settings file.
#	define xtpIniOffice2013AccessGrayDark                                                         \
		_T("OFFICE2013ACCESSGRAYDARK_INI") // Specifies Office 2013 Access Dark Gray settings file.
#	define xtpIniOffice2013AccessGrayLight                                                        \
		_T("OFFICE2013ACCESSGRAYLIGHT_INI") // Specifies Office 2013 Access Light Gray settings
											// file.
#	define xtpIniOffice2013Excel                                                                  \
		_T("OFFICE2013EXCEL_INI") // Specifies Office 2013 Excel settings file.
#	define xtpIniOffice2013ExcelGrayDark                                                          \
		_T("OFFICE2013EXCELGRAYDARK_INI") // Specifies Office 2013 Excel Dark Gray settings file.
#	define xtpIniOffice2013ExcelGrayLight                                                         \
		_T("OFFICE2013EXCELGRAYLIGHT_INI") // Specifies Office 2013 Excel Light Gray settings file.
#	define xtpIniOffice2013OneNote                                                                \
		_T("OFFICE2013ONENOTE_INI") // Specifies Office 2013 OneNote settings file.
#	define xtpIniOffice2013OneNoteGrayDark                                                        \
		_T("OFFICE2013ONENOTEGRAYDARK_INI") // Specifies Office 2013 OneNote Dark Gray settings
											// file.
#	define xtpIniOffice2013OneNoteGrayLight                                                       \
		_T("OFFICE2013ONENOTEGRAYLIGHT_INI") // Specifies Office 2013 OneNote Light Gray settings
											 // file.
#	define xtpIniOffice2013Outlook                                                                \
		_T("OFFICE2013OUTLOOK_INI") // Specifies Office 2013 Outlook settings file.
#	define xtpIniOffice2013OutlookGrayDark                                                        \
		_T("OFFICE2013OUTLOOKGRAYDARK_INI") // Specifies Office 2013 Outlook Dark Gray settings
											// file.
#	define xtpIniOffice2013OutlookGrayLight                                                       \
		_T("OFFICE2013OUTLOOKGRAYLIGHT_INI") // Specifies Office 2013 Outlook Light Gray settings
											 // file.
#	define xtpIniOffice2013Publisher                                                              \
		_T("OFFICE2013PUBLISHER_INI") // Specifies Office 2013 Publisher settings file.
#	define xtpIniOffice2013PublisherGrayDark                                                      \
		_T("OFFICE2013PUBLISHERGRAYDARK_INI") // Specifies Office 2013 Publisher Dark Gray settings
											  // file.
#	define xtpIniOffice2013PublisherGrayLight                                                     \
		_T("OFFICE2013PUBLISHERGRAYLIGHT_INI") // Specifies Office 2013 Publisher Light Gray
											   // settings file.
#	define xtpIniOffice2013PowerPoint                                                             \
		_T("OFFICE2013POWERPOINT_INI") // Specifies Office 2013 PowerPoint settings file.
#	define xtpIniOffice2013PowerPointGrayDark                                                     \
		_T("OFFICE2013POWERPOINTGRAYDARK_INI") // Specifies Office 2013 PowerPoint Dark Gray
											   // settings file.
#	define xtpIniOffice2013PowerPointGrayLight                                                    \
		_T("OFFICE2013POWERPOINTGRAYLIGHT_INI") // Specifies Office 2013 PowerPoint Light Gray
												// settings file.
#	define xtpIniOffice2013Word                                                                   \
		_T("OFFICE2013WORD_INI") // Specifies Office 2013 Word settings file.
#	define xtpIniOffice2013WordGrayDark                                                           \
		_T("OFFICE2013WORDGRAYDARK_INI") // Specifies Office 2013 Word Dark Gray settings file.
#	define xtpIniOffice2013WordGrayLight                                                          \
		_T("OFFICE2013WORDGRAYLIGHT_INI") // Specifies Office 2013 Word Light Gray settings file.

// Office2016.dll
#	define xtpIniOffice2016AccessBlack                                                            \
		_T("OFFICE2016ACCESSBLACK_INI") // Specifies Office 2016 Access Black settings file.
#	define xtpIniOffice2016AccessColorful                                                         \
		_T("OFFICE2016ACCESSCOLORFUL_INI") // Specifies Office 2016 Access Colorful settings file.
#	define xtpIniOffice2016AccessDarkGray                                                         \
		_T("OFFICE2016ACCESSDARKGRAY_INI") // Specifies Office 2016 Access Dark Gray settings file.
#	define xtpIniOffice2016AccessWhite                                                            \
		_T("OFFICE2016ACCESSWHITE_INI") // Specifies Office 2016 Access White settings file.
#	define xtpIniOffice2016ExcelBlack                                                             \
		_T("OFFICE2016EXCELBLACK_INI") // Specifies Office 2016 Excel Black settings file.
#	define xtpIniOffice2016ExcelColorful                                                          \
		_T("OFFICE2016EXCELCOLORFUL_INI") // Specifies Office 2016 Excel Colorful settings file.
#	define xtpIniOffice2016ExcelDarkGray                                                          \
		_T("OFFICE2016EXCELDARKGRAY_INI") // Specifies Office 2016 Excel Dark Gray settings file.
#	define xtpIniOffice2016ExcelWhite                                                             \
		_T("OFFICE2016EXCELWHITE_INI") // Specifies Office 2016 Excel White settings file.
#	define xtpIniOffice2016OneNoteBlack                                                           \
		_T("OFFICE2016ONENOTEBLACK_INI") // Specifies Office 2016 OneNote Black settings file.
#	define xtpIniOffice2016OneNoteColorful                                                        \
		_T("OFFICE2016ONENOTECOLORFUL_INI") // Specifies Office 2016 OneNote Colorful settings file.
#	define xtpIniOffice2016OneNoteDarkGray                                                        \
		_T("OFFICE2016ONENOTEDARKGRAY_INI") // Specifies Office 2016 OneNote Dark Gray settings
											// file.
#	define xtpIniOffice2016OneNoteWhite                                                           \
		_T("OFFICE2016ONENOTEWHITE_INI") // Specifies Office 2016 OneNote White settings file.
#	define xtpIniOffice2016OutlookBlack                                                           \
		_T("OFFICE2016OUTLOOKBLACK_INI") // Specifies Office 2016 Outlook Black settings file.
#	define xtpIniOffice2016OutlookColorful                                                        \
		_T("OFFICE2016OUTLOOKCOLORFUL_INI") // Specifies Office 2016 Outlook Colorful settings file.
#	define xtpIniOffice2016OutlookDarkGray                                                        \
		_T("OFFICE2016OUTLOOKDARKGRAY_INI") // Specifies Office 2016 Outlook Dark Gray settings
											// file.
#	define xtpIniOffice2016OutlookWhite                                                           \
		_T("OFFICE2016OUTLOOKWHITE_INI") // Specifies Office 2016 Outlook White settings file.
#	define xtpIniOffice2016PublisherBlack                                                         \
		_T("OFFICE2016PUBLISHERBLACK_INI") // Specifies Office 2016 Publisher Black settings file.
#	define xtpIniOffice2016PublisherColorful                                                      \
		_T("OFFICE2016PUBLISHERCOLORFUL_INI") // Specifies Office 2016 Publisher Colorful settings
											  // file.
#	define xtpIniOffice2016PublisherDarkGray                                                      \
		_T("OFFICE2016PUBLISHERDARKGRAY_INI") // Specifies Office 2016 Publisher Dark Gray settings
											  // file.
#	define xtpIniOffice2016PublisherWhite                                                         \
		_T("OFFICE2016PUBLISHERWHITE_INI") // Specifies Office 2016 Publisher White settings file.
#	define xtpIniOffice2016PowerPointBlack                                                        \
		_T("OFFICE2016POWERPOINTBLACK_INI") // Specifies Office 2016 PowerPoint Black settings file.
#	define xtpIniOffice2016PowerPointColorful                                                     \
		_T("OFFICE2016POWERPOINTCOLORFUL_INI") // Specifies Office 2016 PowerPoint Colorful settings
											   // file.
#	define xtpIniOffice2016PowerPointDarkGray                                                     \
		_T("OFFICE2016POWERPOINTDARKGRAY_INI") // Specifies Office 2016 PowerPoint Dark Gray
											   // settings file.
#	define xtpIniOffice2016PowerPointWhite                                                        \
		_T("OFFICE2016POWERPOINTWHITE_INI") // Specifies Office 2016 PowerPoint White settings file.
#	define xtpIniOffice2016WordBlack                                                              \
		_T("OFFICE2016WORDBLACK_INI") // Specifies Office 2016 Word Black settings file.
#	define xtpIniOffice2016WordColorful                                                           \
		_T("OFFICE2016WORDCOLORFUL_INI") // Specifies Office 2016 Word Colorful settings file.
#	define xtpIniOffice2016WordDarkGray                                                           \
		_T("OFFICE2016WORDDARKGRAY_INI") // Specifies Office 2016 Word Dark Gray settings file.
#	define xtpIniOffice2016WordWhite                                                              \
		_T("OFFICE2016WORDWHITE_INI") // Specifies Office 2016 Word White settings file.

// VisualStudio2012.dll
#	define xtpIniVisualStudio2012Dark                                                             \
		_T("VISUALSTUDIO2012DARK_INI") // Specifies Visual Studio 2012 Dark settings file.
#	define xtpIniVisualStudio2012Light                                                            \
		_T("VISUALSTUDIO2012LIGHT_INI") // Specifies Visual Studio 2012 Light settings file.

// VisualStudio2015.dll
#	define xtpIniVisualStudio2015Blue                                                             \
		_T("VISUALSTUDIO2015BLUE_INI") // Specifies Visual Studio 2015 Blue settings file.
#	define xtpIniVisualStudio2015Dark                                                             \
		_T("VISUALSTUDIO2015DARK_INI") // Specifies Visual Studio 2015 Dark settings file.
#	define xtpIniVisualStudio2015Light                                                            \
		_T("VISUALSTUDIO2015LIGHT_INI") // Specifies Visual Studio 2015 Light settings file.

// Windows7.dll
#	define xtpIniWindows7Blue _T("WINDOWS7BLUE_INI") // Specifies Windows 7 Blue settings file.

// Windows10.dll
#	define xtpIniWindows10Light                                                                   \
		_T("WINDOWS10LIGHT_INI") // Specifies Windows 10 Light settings file.
#	define xtpIniWindows10Dark _T("WINDOWS10DARK_INI") // Specifies Windows 10 Dark settings file.

#	include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPRESOURCEIMAGE_H__)
