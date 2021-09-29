// XTPImageManager.h : interface for the CXTPImageManager class.
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
#if !defined(__XTPIMAGEMANAGER_H__)
#	define __XTPIMAGEMANAGER_H__
//}}AFX_CODEJOCK_PRIVATE

#	if _MSC_VER >= 1000
#		pragma once
#	endif // _MSC_VER >= 1000

#	include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

//-----------------------------------------------------------------------
// Summary:
//     Image state enumerators.
// Example:
// <code>
// // Set Icons
// pCommandBars->GetImageManager()->SetIcons(IDR_MAINFRAME, xtpImageNormal);
// pCommandBars->GetImageManager()->SetIcons(IDR_MAINFRAME, IDR_MAINFRAME_HOT, xtpImageHot);
// </code>
// See Also: CXTPImageManager::SetIcon, CXTPImageManager::SetIcons
//
// <KEYWORDS xtpImageNormal, xtpImageDisabled, xtpImageHot, xtpImageChecked>
//-----------------------------------------------------------------------
enum XTPImageState
{
	xtpImageNormal,	  // Normal image.
	xtpImageDisabled, // Disabled image.
	xtpImageHot,	  // Hot image.
	xtpImageChecked,  // Checked (selected) image.
	xtpImagePressed	  // Pressed image.
};

enum XTPIconSizeMatching
{
	xtpIconSizeExact,	// Requires an icon to have size the same as requested.
	xtpIconSizeAtLeast, // Requires an icon to have size the same or larger than requested.
	xtpIconSizeAtMost,	// Requires an icon to have size no more than requested.
};

class CXTPImageManager;
class CXTPImageManagerIconSet;
class CXTPImageManagerImageList;
class CXTPImageManagerIconHandle;
class CXTPImageManagerResource;
class CXTPPropExchange;
class CXTPMarkupContext;
class CXTPMarkupUIElement;

//===========================================================================
// Summary:
//      Represents a generic interface for a vector image of any possible
//      format to be used with CXTPImageManager.
//===========================================================================
class _XTP_EXT_CLASS CXTPImageManagerVectorImageHandle
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Construct an empty vector image handle object.
	//-----------------------------------------------------------------------
	CXTPImageManagerVectorImageHandle();

	//-----------------------------------------------------------------------
	// Summary:
	//     Handles object destruction, releases vector image resources.
	//-----------------------------------------------------------------------
	virtual ~CXTPImageManagerVectorImageHandle();

	//-----------------------------------------------------------------------
	// Summary:
	//     Construct a copy of the vector image handle object.
	// Parameters:
	//     rhs - a source vector image handle.
	//-----------------------------------------------------------------------
	CXTPImageManagerVectorImageHandle(const CXTPImageManagerVectorImageHandle& rhs);

	//-----------------------------------------------------------------------
	// Summary:
	//     Make the current vector image handle identical to the provided image handle.
	// Parameters:
	//     rhs - a source vector image handle.
	// Returns:
	//     Self reference.
	//-----------------------------------------------------------------------
	CXTPImageManagerVectorImageHandle& operator=(const CXTPImageManagerVectorImageHandle& rhs);

	//-----------------------------------------------------------------------
	// Summary:
	//     Checks if the current vector image handle has a valid image associated with it.
	// Returns:
	//     TRUE if the handle is a valid image.
	//-----------------------------------------------------------------------
	operator BOOL() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines if the handle is associated with Markup image.
	// Returns:
	//     TRUE if the handle is associated with Markup image.
	//-----------------------------------------------------------------------
	BOOL IsMarkupImage() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Draws the image associated with the handle.
	// Parameters:
	//      pDC - Target device context pointer.
	//      pt - Target top-left corner location.
	//      szIcon - Target image size.
	//-----------------------------------------------------------------------
	void Draw(CDC* pDC, CPoint pt, CSize szIcon);

	//-----------------------------------------------------------------------
	// Summary:
	//      Obtains a handle to image specific serialization data that can be used
	//      storing and re-storing an image from.
	// Returns:
	//      Serialization data handle. The handle is managed by the object
	//      so the caller must not deallocate or change it.
	//-----------------------------------------------------------------------
	HGLOBAL GetSerializationData() const;

	//-----------------------------------------------------------------------
	// Summary:
	//      Creates a markup image handle.
	// Parameters:
	//      pContext - User provided markup context pointer.
	//      bSharedContext - If TRUE, the user provided markup context pointer
	//                       is assumed to be shared, i.e. used and managed by the caller.
	//                       FALSE gives the image handle exclusize rights to manage and
	//                       release the context once it is no longer needed.
	//      SerializationData - Markup image serialization data handle.
	//      pStream - Markup image data stream pointer.
	//      lpszCode - Markup image code pointer.
	//      nLength - Optional markup image code length in characters.
	// Returns:
	//      Serialization data handle. The handle is managed by the object
	//      so the caller must not deallocate or change it.
	//-----------------------------------------------------------------------
	static CXTPImageManagerVectorImageHandle CreateMarkupHandle(CXTPMarkupContext* pContext,
																BOOL bSharedContext,
																HGLOBAL SerializationData);
	//-----------------------------------------------------------------------
	// <combine
	// CXTPImageManagerVectorImageHandle::CreateMarkupHandle@CXTPMarkupContext*@BOOL@HGLOBAL>
	//-----------------------------------------------------------------------
	static CXTPImageManagerVectorImageHandle CreateMarkupHandle(CXTPMarkupContext* pContext,
																BOOL bSharedContext,
																IStream* pStream);
	//-----------------------------------------------------------------------
	// <combine
	// CXTPImageManagerVectorImageHandle::CreateMarkupHandle@CXTPMarkupContext*@BOOL@HGLOBAL>
	//-----------------------------------------------------------------------
	static CXTPImageManagerVectorImageHandle CreateMarkupHandle(CXTPMarkupContext* pContext,
																BOOL bSharedContext,
																LPCSTR lpszCode,
																INT_PTR nLength = -1);
	//-----------------------------------------------------------------------
	// <combine
	// CXTPImageManagerVectorImageHandle::CreateMarkupHandle@CXTPMarkupContext*@BOOL@HGLOBAL>
	//-----------------------------------------------------------------------
	static CXTPImageManagerVectorImageHandle CreateMarkupHandle(CXTPMarkupContext* pContext,
																BOOL bSharedContext,
																LPCWSTR lpszCode,
																INT_PTR nLength = -1);

private:
	CXTPImageManagerVectorImageHandle(CXTPMarkupContext* pContext, BOOL bSharedContext,
									  CXTPMarkupUIElement* pMarkupElement,
									  HGLOBAL SerializationData);
	CXTPImageManagerVectorImageHandle(CXTPMarkupContext* pContext, BOOL bSharedContext,
									  CXTPMarkupUIElement* pMarkupElement,
									  IStream* pSerializationData);

	void Reset();

private:
	struct IMAGE_RESOURCE
	{
		ULONG cRefs;
		enum
		{
			MarkupType
		} nType;
		HGLOBAL SerializationData;
		IStream* pSerializationDataStream;
		union
		{
			struct
			{
				CXTPMarkupContext* pContext;
				BOOL bSharedContext;
				CXTPMarkupUIElement* pMarkupElement;
			} markup;
			// more can be added
		} u;
	};

	mutable IMAGE_RESOURCE* m_pResource;
};

//-----------------------------------------------------------------------
// Summary:
//     This structure is used to define the position and style of the icon in resources.
//-----------------------------------------------------------------------
struct XTP_IMAGERESOURCE_INFO
{
	BOOL bAlpha; // TRUE if the icon has alpha layer, otherwise m_hbmpMask used to draw its mask.
	CRect rc;	 // Position of the icon inside resources' m_hbmpImage bitmap.
	CXTPImageManagerResource* pResource; // Pointer to owner resources.
};

//===========================================================================
// Summary:
//     CXTPImageManagerIconHandle is a HICON wrapper class.
//===========================================================================
class _XTP_EXT_CLASS CXTPImageManagerIconHandle
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPImageManagerIconHandle object.
	// Parameters:
	//     hIcon - Icon handle.
	//     hBitmap - Bitmap handle.
	//     hHandle - CXTPImageManagerIconHandle reference.
	//     bSharedHandle - If FALSE, the resource will be destroyed
	//                     when it is no longer in use.
	//-----------------------------------------------------------------------
	CXTPImageManagerIconHandle();
	//-----------------------------------------------------------------------
	// <combine CXTPImageManagerIconHandle::CXTPImageManagerIconHandle>
	//-----------------------------------------------------------------------
	CXTPImageManagerIconHandle(HICON hIcon, BOOL bUseResources = TRUE, BOOL bSharedHandle = TRUE);
	//-----------------------------------------------------------------------
	// <combine CXTPImageManagerIconHandle::CXTPImageManagerIconHandle>
	//-----------------------------------------------------------------------
	CXTPImageManagerIconHandle(HBITMAP hBitmap, BOOL bSharedHandle = TRUE);
	//-----------------------------------------------------------------------
	// <combine CXTPImageManagerIconHandle::CXTPImageManagerIconHandle>
	//-----------------------------------------------------------------------
	CXTPImageManagerIconHandle(const CXTPImageManagerIconHandle& hHandle);

	//-------------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPImageManagerIconHandle object, handles cleanup and deallocation.
	//-------------------------------------------------------------------------
	~CXTPImageManagerIconHandle();

	//-----------------------------------------------------------------------
	// Summary:
	//     This operator returns a HICON object whose value is copied from this
	//     CXTPImageManagerIconHandle object.
	// Returns:
	//     HICON handle.
	//-----------------------------------------------------------------------
	operator HICON() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Copies data from an HBITMAP or CXTPImageManagerIconHandle object.
	// Parameters:
	//     handle - CXTPImageManagerIconHandle object.
	//     hBitmap - Alpha bitmap handler.
	//-----------------------------------------------------------------------
	void CopyHandle(HBITMAP hBitmap);
	//-----------------------------------------------------------------------
	// <combine CXTPImageManagerIconHandle::CopyHandle@HBITMAP>
	//-----------------------------------------------------------------------
	void CopyHandle(const CXTPImageManagerIconHandle& handle);

	//-----------------------------------------------------------------------
	// Summary:
	//     Checks if the image has alpha channel.
	// Returns:
	//     TRUE if the image has alpha channel, otherwise FALSE.
	//-----------------------------------------------------------------------
	BOOL IsAlpha() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Checks if the class has no icon.
	// Returns:
	//     TRUE if the m_hIcon member is NULL.
	//-----------------------------------------------------------------------
	BOOL IsEmpty() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves bitmap handle.
	// Returns:
	//     32bit alpha bitmap.
	//-----------------------------------------------------------------------
	HBITMAP GetBitmap() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves the icon handle.
	// Returns:
	//     Icon handle.
	//-----------------------------------------------------------------------
	HICON GetIcon() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves the size of the icon.
	// Returns:
	//     Size of the icon.
	//-----------------------------------------------------------------------
	CSize GetExtent() const;

	//---------------------------------------------------------------
	// Summary:
	//      Call this method to draw icons.
	// Parameters:
	//      pDC - Pointer to the device context to draw into.
	//      pt - Positioning of the icon to draw.
	//      szIcon - Size of the icon to draw.
	//      bDrawComposited - TRUE to draw inside Vista Composited area.
	//---------------------------------------------------------------
	void Draw(CDC* pDC, CPoint pt, CSize szIcon, BOOL bDrawComposited = FALSE);

	//---------------------------------------------------------------
	// Summary:
	//      Obtains a new icon handle with the icon image scaled to the specified DPI.
	// Parameters:
	//      nDdpi - Custom DPI level or 0 for the current DPI value.
	// Returns:
	//      A new scaled icon handle. The caller is responsible for destroying the handle.
	//---------------------------------------------------------------
	HICON GetDpiScaled(int nDpi = 0) const;

	//-------------------------------------------------------------------------
	// Summary:
	//     Release bitmap and icon handlers.
	//-------------------------------------------------------------------------
	void Clear();

	//-------------------------------------------------------------------------
	// Summary:
	//     Creates an icon from resource bits describing the icon.
	// Parameters:
	//     hInst - Application instance handler.
	//     lpszResourceName - Resource name identifier.
	//     szIcon - Specifies the desired width and height, in pixels, of the icon.
	//     bGroupResource - TRUE to check if the RT_GROUP_ICON resource exists.
	//     nSizeMatching - Specifies how icon size is matched. By default, exact matching
	//                     is specified. Used only if bGroupResource is TRUE.
	// Returns:
	//     TRUE if successful, otherwise FALSE.
	//-------------------------------------------------------------------------
	BOOL CreateIconFromResource(LPCTSTR lpszResourceName, CSize szIcon, BOOL bGroupResource = TRUE,
								XTPIconSizeMatching nSizeMatching = xtpIconSizeExact);
	//-------------------------------------------------------------------------
	// <combine CXTPImageManagerIconHandle::CreateIconFromResource@LPCTSTR@CSize@BOOL>
	//-------------------------------------------------------------------------
	BOOL CreateIconFromResource(HINSTANCE hInst, LPCTSTR lpszResourceName, CSize szIcon,
								BOOL bGroupResource,
								XTPIconSizeMatching nSizeMatching = xtpIconSizeExact);

	//-----------------------------------------------------------------------
	// Summary:
	//     Attaches an object to an existing icon handle.
	// Parameters:
	//     hIcon - Source icon handler.
	//     bSharedHandle - If FALSE, the resource will be destroyed
	//                     when it is no longer in use.
	//-----------------------------------------------------------------------
	void Attach(HICON hIcon, BOOL bSharedHandle);

	//-----------------------------------------------------------------------
	// Summary:
	//     Attaches an object to an existing bitmap handle.
	// Parameters:
	//     hBitmap - Alpha bitmap handler.
	//     bSharedHandle - If FALSE, the resource will be destroyed
	//                     when it is no longer in use.
	//-----------------------------------------------------------------------
	void Attach(HBITMAP hBitmap, BOOL bSharedHandle);

	//-----------------------------------------------------------------------
	// Summary:
	//     This operator assigns the icon handler to a
	//     CXTPImageManagerIconHandle object.
	// Parameters:
	//     hIcon - Source icon handler.
	//-----------------------------------------------------------------------
	_XTP_DEPRECATED_IN_FAVOR2(CXTPImageManagerIconHandle, Attach)
	const CXTPImageManagerIconHandle& operator=(const HICON hIcon);

	//-----------------------------------------------------------------------
	// Summary:
	//     This operator assigns the alpha bitmap handler to a
	//     CXTPImageManagerIconHandle object.
	// Parameters:
	//     hBitmap - Alpha bitmap handler
	//-----------------------------------------------------------------------
	_XTP_DEPRECATED_IN_FAVOR2(CXTPImageManagerIconHandle, Attach)
	const CXTPImageManagerIconHandle& operator=(const HBITMAP hBitmap);

	//-----------------------------------------------------------------------
	// Summary:
	//     Creates an alpha bitmap with a valid alpha layer.
	// Returns:
	//     TRUE if successful, otherwise FALSE.
	//-----------------------------------------------------------------------
	LPBYTE PreMultiply();

	//-----------------------------------------------------------------------
	// Summary:
	//     Detaches the handle from the associated icon handle without
	//     releasing the resource.
	//-----------------------------------------------------------------------
	void Detach();

protected:
	//---------------------------------------------------------------
	// Summary:
	//      Initializes icons.
	//---------------------------------------------------------------
	void Init();

private:
	const CXTPImageManagerIconHandle& operator=(const CXTPImageManagerIconHandle&);
	CSize _GetExtent() const;

protected:
	HICON m_hIcon;	   // The underlying HICON handler for this CXTPImageManagerIconHandle object.
	HBITMAP m_hBitmap; // The underlying Alpha bitmap handler for this CXTPImageManagerIconHandle
					   // object.
	LPBYTE m_pBits;	   // Alpha bitmap bits.
	BOOL m_bClearHandles;			   // TRUE if object must destroy handlers.
	CXTPImageManager* m_pImageManager; // Pointer to parent Image Manager.
	XTP_IMAGERESOURCE_INFO* m_pRInfo;  // Pointer to resource info if the icon is part of common
									   // resources.
	CSize m_szIcon;					   // Size of the icon.
	BOOL m_bUseResources;			   // TRUE to use common resources.

private:
	friend class CXTPImageManagerIcon;
	friend class CXTPImageManagerResource;
};

//===========================================================================
// Summary:
//     Icons holder class.
//===========================================================================
class _XTP_EXT_CLASS CXTPImageManagerIcon : public CXTPCmdTarget
{
	DECLARE_DYNAMIC(CXTPImageManagerIcon)

private:
	struct ICONDIRENTRY;
	struct ICONDIRHEADER;
	struct ICONIMAGE;
	struct GRPICONDIRENTRY;
	struct GRPICONDIR;

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPImageManagerIcon object.
	// Parameters:
	//     nID - Icon identifier.
	//     nWidth - Initial width for the icon.
	//     nHeight - Initial height for the icon.
	//     pIconSet - Parent icon set icon will belong.
	//     pImageList - Parent icon set icon will belong.
	//-----------------------------------------------------------------------
	CXTPImageManagerIcon(UINT nID, int nWidth, int nHeight,
						 CXTPImageManagerIconSet* pIconSet = NULL);
	//-----------------------------------------------------------------------
	// <combine CXTPImageManagerIcon::CXTPImageManagerIcon@UINT@int@int@CXTPImageManagerIconSet*>
	//-----------------------------------------------------------------------
	CXTPImageManagerIcon(UINT nID, int nWidth, int nHeight, CXTPImageManagerImageList* pImageList);
	//-----------------------------------------------------------------------
	// <combine CXTPImageManagerIcon::CXTPImageManagerIcon@UINT@int@int@CXTPImageManagerIconSet*>
	//-----------------------------------------------------------------------
	CXTPImageManagerIcon(UINT nID, CXTPImageManagerIconSet* pIconSet = NULL);

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPImageManagerIcon object, handles cleanup and deallocation.
	//-----------------------------------------------------------------------
	~CXTPImageManagerIcon();

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves the image identifier.
	// Returns:
	//     Identifier of the image.
	//-----------------------------------------------------------------------
	UINT GetID() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Checks if the image has alpha channel.
	// Returns:
	//     TRUE if the image has alpha channel, otherwise FALSE.
	//-----------------------------------------------------------------------
	BOOL IsAlpha() const;

	//-------------------------------------------------------------------------
	// Summary:
	//     Checks if icons were scaled from another CXTPImageManagerIcon object.
	//-------------------------------------------------------------------------
	BOOL IsScaled() const;

	//-------------------------------------------------------------------------
	// Summary:
	//     Obtains a pointer to an associated image list if the icon object is an
	//     icon in an image list. If the returned pointer is not NULL, then the
	//     icon ID should be used as an icon index in the returned image list.
	// Returns:
	//     Pointer value to an associated image list, or NULL.
	//-------------------------------------------------------------------------
	CXTPImageManagerImageList* GetImageList();
	//-----------------------------------------------------------------------
	// <combine CXTPImageManagerIcon::GetImageList>
	//-----------------------------------------------------------------------
	const CXTPImageManagerImageList* GetImageList() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves the icon handle.
	// Parameters:
	//     imageState - Image state for the icon to retrieve.
	// Returns:
	//     Icon handle.
	// See Also:
	//     XTPImageState
	//-----------------------------------------------------------------------
	CXTPImageManagerIconHandle& GetIcon();
	//-----------------------------------------------------------------------
	// <combine CXTPImageManagerIcon::GetIcon>
	//-----------------------------------------------------------------------
	CXTPImageManagerIconHandle& GetIcon(XTPImageState imageState);

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves the faded icon.
	// Returns:
	//     Icon handle.
	//-----------------------------------------------------------------------
	CXTPImageManagerIconHandle& GetFadedIcon();

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves the shadow for the icon.
	// Returns:
	//     Icon handle.
	//-----------------------------------------------------------------------
	CXTPImageManagerIconHandle& GetShadowIcon();

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves the checked icon.
	// Returns:
	//     Icon handle.
	//-----------------------------------------------------------------------
	CXTPImageManagerIconHandle& GetCheckedIcon();

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves the pressed icon.
	// Returns:
	//     Icon handle.
	//-----------------------------------------------------------------------
	CXTPImageManagerIconHandle& GetPressedIcon();

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves hot icon.
	// Returns:
	//     Icon handle.
	//-----------------------------------------------------------------------
	CXTPImageManagerIconHandle& GetHotIcon();

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves the disabled icon.
	// Parameters:
	//     bCreateIfNotExists - TRUE to create the disabled icon if the image does not
	//                          exist. By default, this parameter is TRUE.
	// Returns:
	//     Icon handle.
	//-----------------------------------------------------------------------
	CXTPImageManagerIconHandle& GetDisabledIcon(BOOL bCreateIfNotExists = TRUE);

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines if a state specific icon exists.
	// Parameters:
	//     imageState - Image state for which to check icon existence.
	// Returns:
	//     TRUE if the state specific icon exists.
	//-----------------------------------------------------------------------
	BOOL HasIcon(XTPImageState imageState) const;
	// -----------------------------------------------------------------------
	// <combine CXTPImageManagerIcon::HasIcon@XTPImageState>
	// -----------------------------------------------------------------------
	BOOL HasNormalIcon() const;
	// -----------------------------------------------------------------------
	// <combine CXTPImageManagerIcon::HasIcon@XTPImageState>
	// -----------------------------------------------------------------------
	BOOL HasDisabledIcon() const;
	// -----------------------------------------------------------------------
	// <combine CXTPImageManagerIcon::HasIcon@XTPImageState>
	// -----------------------------------------------------------------------
	BOOL HasCheckedIcon() const;
	// -----------------------------------------------------------------------
	// <combine CXTPImageManagerIcon::HasIcon@XTPImageState>
	// -----------------------------------------------------------------------
	BOOL HasPressedIcon() const;
	// -----------------------------------------------------------------------
	// <combine CXTPImageManagerIcon::HasIcon@XTPImageState>
	// -----------------------------------------------------------------------
	BOOL HasHotIcon() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Creates a disabled icon.
	// Parameters:
	//     clrDisabledLight - The lightest color of icon to generate.
	//     clrDisabledDark -  The darkest color of icon to generate.
	//-----------------------------------------------------------------------
	void CreateDisabledIcon(COLORREF clrDisabledLight = (COLORREF)-1,
							COLORREF clrDisabledDark  = (COLORREF)-1);

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets the icon.
	// Parameters:
	//     hIcon - Icon handle to set.
	//     imageState - State of the icon.
	//     nWidth - Width of the icon.
	//     nHeight - Height of the icon.
	//     nIDResourceIcon - Icon resource identifier.
	//     nSizeMatching - Specifies how icon size is matched. By default, exact matching
	//                     is specified. Used only if group icon resources.
	// Returns:
	//     TRUE if successful, otherwise FALSE.
	//-----------------------------------------------------------------------
	BOOL SetIcon(const CXTPImageManagerIconHandle& hIcon);
	// -----------------------------------------------------------------------
	// <combine CXTPImageManagerIcon::SetIcon@CXTPImageManagerIconHandle&amp;>
	// -----------------------------------------------------------------------
	BOOL SetIcon(const CXTPImageManagerIconHandle& hIcon, XTPImageState imageState);
	// -----------------------------------------------------------------------
	// <combine CXTPImageManagerIcon::SetIcon@CXTPImageManagerIconHandle&amp;>
	// -----------------------------------------------------------------------
	BOOL SetIcon(UINT nIDResourceIcon, int nWidth, int nHeight,
				 XTPIconSizeMatching nSizeMatching = xtpIconSizeExact);

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets the disabled icon.
	// Parameters:
	//     hIcon - Icon handle to set.
	//-----------------------------------------------------------------------
	void SetDisabledIcon(const CXTPImageManagerIconHandle& hIcon);

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets the hot icon.
	// Parameters:
	//     hIcon - Icon handle to set.
	//-----------------------------------------------------------------------
	void SetHotIcon(const CXTPImageManagerIconHandle& hIcon);

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets the checked icon.
	// Parameters:
	//     hIcon - Icon handle to set.
	//-----------------------------------------------------------------------
	void SetCheckedIcon(const CXTPImageManagerIconHandle& hIcon);

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets the icon.
	// Parameters:
	//     hIcon - Icon handle to set.
	//-----------------------------------------------------------------------
	void SetNormalIcon(const CXTPImageManagerIconHandle& hIcon);

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets the pressed icon.
	// Parameters:
	//     hIcon - Icon handle to set.
	//-----------------------------------------------------------------------
	void SetPressedIcon(const CXTPImageManagerIconHandle& hIcon);

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves the size of the icon.
	// Parameters:
	//     hIcon - Icon handle.
	// Returns:
	//     Size of the icon.
	//-----------------------------------------------------------------------
	CSize GetExtent() const;
	static CSize AFX_CDECL GetExtent(HICON hIcon);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method tests if the file is an alpha bitmap.
	// Parameters:
	//     pszFileName - File path.
	// Returns:
	//     TRUE if the file is an alpha bitmap.
	//-----------------------------------------------------------------------
	static BOOL AFX_CDECL IsAlphaBitmapFile(LPCTSTR pszFileName);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method tests if the file is a .png image.
	// Parameters:
	//     pszFileName - File path.
	// Returns:
	//     TRUE if the file is a .png image.
	//-----------------------------------------------------------------------
	static BOOL AFX_CDECL IsPngBitmapFile(LPCTSTR pszFileName);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method loads an alpha bitmap from an icon file.
	// Parameters:
	//     pszFileName - File path.
	//     nWidth - Icon width to load.
	// Returns:
	//     Alpha bitmap if an icon with the specified width was found, otherwise NULL.
	//-----------------------------------------------------------------------
	static HBITMAP AFX_CDECL LoadAlphaIcon(LPCTSTR pszFileName, int nWidth);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method loads an alpha bitmap from a resource file.
	// Parameters:
	//     nIDResource - Resource identifier.
	//     hModule - Handle to the instance of the module whose executable
	//               file contains the bitmap to be loaded.
	//     lpszResource - Long pointer to a null-terminated string that contains the
	//                    name of the bitmap resource to be loaded.
	// Remarks:
	//     Do not use LoadImage to load an alpha bitmap. It loses alpha channel
	//     in some Windows.
	// Returns:
	//     Alpha bitmap handler.
	//-----------------------------------------------------------------------
	static HBITMAP AFX_CDECL LoadAlphaBitmap(UINT nIDResource);
	//-----------------------------------------------------------------------
	// <combine CXTPImageManagerIcon::LoadAlphaBitmap@UINT>
	//-----------------------------------------------------------------------
	static HBITMAP AFX_CDECL LoadAlphaBitmap(HMODULE hModule, LPCTSTR lpszResource);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method loads a bitmap or .png from a file on disk.
	// Parameters:
	//     lpszFileName - File name of a bitmap or .png file.
	//     lbAlphaBitmap - Determines if a bitmap contains an alpha layer.
	// Returns:
	//     Bitmap handler.
	//-----------------------------------------------------------------------
	static HBITMAP AFX_CDECL LoadBitmapFromFile(LPCTSTR lpszFileName, BOOL* lbAlphaBitmap = NULL);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method loads an alpha bitmap or .png from a resource file.
	// Parameters:
	//     hModule - Handle to the instance of the module whose executable
	//               file contains the bitmap to be loaded.
	//     lpszResource - Long pointer to a null-terminated string that contains the
	//                    name of the bitmap resource to be loaded.
	//     lbAlphaBitmap - Determines if a bitmap contains an alpha layer.
	// Returns:
	//     Alpha bitmap handler.
	//-----------------------------------------------------------------------
	static HBITMAP AFX_CDECL LoadBitmapFromResource(LPCTSTR lpszResource, BOOL* lbAlphaBitmap);
	//-----------------------------------------------------------------------
	// <combine CXTPImageManagerIcon::LoadBitmapFromResource@LPCTSTR@BOOL*>
	//-----------------------------------------------------------------------
	static HBITMAP AFX_CDECL LoadBitmapFromResource(HMODULE hModule, LPCTSTR lpszResource,
													BOOL* lbAlphaBitmap);

	static HBITMAP AFX_CDECL LoadPNGFromMemory(const void* Buf, int Size, BOOL* lbAlphaBitmap);

	//-----------------------------------------------------------------------
	// Summary:
	//     Checks if a bitmap with a specified resource has alpha channel.
	// Parameters:
	//     hModule - Module of the bitmap to check.
	//     lpBitmapName - Resource name of the bitmap to check.
	// Returns:
	//     TRUE if the bitmap has alpha channel, otherwise FALSE.
	//-----------------------------------------------------------------------
	static BOOL AFX_CDECL IsAlphaBitmapResource(HMODULE hModule, LPCTSTR lpBitmapName);

	//-----------------------------------------------------------------------
	// Summary:
	//     Checks if a bitmap with a specified resource is a .png resource.
	// Parameters:
	//     hModule - Module of the bitmap to check.
	//     lpBitmapName - Resource name of the bitmap to check.
	// Returns:
	//     TRUE if the bitmap is a png resource, otherwise FALSE.
	//-----------------------------------------------------------------------
	static BOOL AFX_CDECL IsPngBitmapResource(HMODULE hModule, LPCTSTR lpBitmapName);

	static BOOL AFX_CDECL IsPngBitmapMemory(const void* Buf, int Size);

	//-----------------------------------------------------------------------
	// Summary:
	//     The DrawAlphaBitmap function displays bitmaps that have transparent
	//     or semitransparent pixels.
	// Parameters:
	//     pDC     - Pointer to destination device context.
	//     ptDest  - Upper-left corner of the destination rectangle.
	//     ptSrc   - Upper-left corner of the source rectangle.
	//     hBitmap - Alpha bitmap handler.
	//     szDest  - Destination size.
	//     szSrc   - Source size.
	//-----------------------------------------------------------------------
	static void AFX_CDECL DrawAlphaBitmap(CDC* pDC, HBITMAP hBitmap, CPoint ptDest, CSize szDest,
										  CPoint ptSrc = 0, CSize szSrc = 0);

	//-----------------------------------------------------------------------
	// Summary:
	//     Creates valid alpha bits to use in AlphaBlend.
	// Parameters:
	//     hBitmap - Alpha bitmap handler.
	// Returns:
	//     Valid alpha bits.
	// See Also:
	//     BLENDFUNCTION, AlphaBlend
	//-----------------------------------------------------------------------
	static HBITMAP AFX_CDECL PreMultiplyAlphaBitmap(HBITMAP hBitmap, BOOL* pbAlpha = NULL,
													LPBYTE* lpBits = NULL);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to create a mirror copy of a bitmap.
	// Parameters:
	//     hBitmap - Bitmap handle to be copied.
	// Returns:
	//     New mirror bitmap
	//-----------------------------------------------------------------------
	static HBITMAP AFX_CDECL InvertAlphaBitmap(HBITMAP hBitmap);

	//-----------------------------------------------------------------------
	// Summary:
	//     This helper method retrieves information about an alpha bitmap
	//     and its alpha bits.
	// Parameters:
	//     dcSrc   - Reference to CDC.
	//     hBitmap - Alpha bitmap.
	//     pbmi    - Bitmap information.
	//     pBits   - Bitmap bits.
	//     nSize   - Size of bits array.
	// Returns:
	//     TRUE if successful, otherwise FALSE.
	//-----------------------------------------------------------------------
	static BOOL AFX_CDECL GetBitmapBits(CDC& dcSrc, HBITMAP hBitmap, PBITMAPINFO& pbmi,
										LPVOID& pBits, UINT& nSize);

	//-----------------------------------------------------------------------
	// Summary:
	//     Copies an alpha bitmap.
	// Parameters:
	//     hBitmap - Bitmap handler to be copied.
	// Returns:
	//     New alpha bitmap handler.
	//-----------------------------------------------------------------------
	static HBITMAP AFX_CDECL CopyAlphaBitmap(HBITMAP hBitmap, LPBYTE* lpBits = NULL);

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines whether the provided icon is translucent or not.
	// Parameters:
	//     hicon - Icon handle to check.
	// Returns:
	//     TRUE if the icon provided is translucent, otherwise FALSE.
	//-----------------------------------------------------------------------
	static BOOL AFX_CDECL IsAlphaIconHandle(HICON hIcon);

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves the icon height.
	// Returns:
	//     Height of the icon.
	//-----------------------------------------------------------------------
	int GetHeight() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves the icon width.
	// Returns:
	//     Width of the icon.
	//-----------------------------------------------------------------------
	int GetWidth() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Either reads this object from or writes this object to an archive.
	// Parameters:
	//     ar - A CArchive object to serialize to or from.
	//-----------------------------------------------------------------------
	void Serialize(CArchive& ar);

	//-----------------------------------------------------------------------
	// Summary:
	//     Makes the current icon a copy of a provided icon.
	// Parameters:
	//     pIcon - An icon pointer to copy from.
	//-----------------------------------------------------------------------
	void Copy(CXTPImageManagerIcon* pIcon);

	//-----------------------------------------------------------------------
	// Summary:
	//     Either reads this object from or writes this object to an archive.
	// Parameters:
	//     hIcon - Icon to be serialized.
	//     ar - A CArchive object to serialize to or from.
	//-----------------------------------------------------------------------
	void SerializeIcon(CXTPImageManagerIconHandle& hIcon, CArchive& ar, long nSchema);
	void SerializeIcon(CXTPImageManagerVectorImageHandle& hIcon, CArchive& ar, long nSchema);

	//-----------------------------------------------------------------------
	// Summary:
	//     Draws the image at a specified location.
	// Parameters:
	//     pDC    - Destination device context.
	//     pt     - Specifies the location of the image.
	//     hIcon  - A handle to an icon.
	//     szIcon - Specifies the size of the image.
	//     imageState - State specific image to draw.
	//-----------------------------------------------------------------------
	void Draw(CDC* pDC, CPoint pt);
	void Draw(CDC* pDC, CPoint pt, CSize szIcon);
	void Draw(CDC* pDC, CPoint pt, XTPImageState imageState);
	void Draw(CDC* pDC, CPoint pt, XTPImageState imageState, CSize szIcon);
	void Draw(CDC* pDC, CPoint pt, CXTPImageManagerIconHandle& hIcon, CSize szIcon = 0,
			  COLORREF clrBK = CLR_NONE, COLORREF clrFG = CLR_NONE, UINT uiFlags = 0);

	//-----------------------------------------------------------------------
	// Summary:
	//     Draws the image using the brush specified by the clrBrush parameter.
	// Parameters:
	//     pDC      - Destination device context.
	//     pt       - Specifies the location of the image.
	//     hIcon    - A handle to an icon.
	//     clrBrush - Color of the brush used to draw the image.
	//-----------------------------------------------------------------------
	void DrawMono(CDC* pDC, CPoint pt, CXTPImageManagerIconHandle& hIcon, CSize szIcon,
				  COLORREF clrBrush);

	//-------------------------------------------------------------------------
	// Summary:
	//     Redraws Fade and Shadow icons.
	//-------------------------------------------------------------------------
	void Refresh();

	//-------------------------------------------------------------------------
	// Summary:
	//     Creates a markup context if it is necessary for the icon and
	//     has not already been created.
	// Returns:
	//     A pointer to the created or existing markup context, or NULL if such
	//     a context is not required for the icon type.
	//-------------------------------------------------------------------------
	CXTPMarkupContext* CreateMarkupContext();

	//-------------------------------------------------------------------------
	// Summary:
	//     Retrieves the markup context for the icon.
	// Returns:
	//     The markup context for the icon.
	//-------------------------------------------------------------------------
	CXTPMarkupContext* GetMarkupContext();

	//-------------------------------------------------------------------------
	// Summary:
	//     Sets the icon for a specified state.
	// Parameters:
	//     hIcon - Icon handle to use for the specified state.
	//     imageState - Image state identifier to set the icon for.
	// Returns:
	//     TRUE if the icon was successfully set.
	//-------------------------------------------------------------------------
	BOOL SetVectorIcon(CXTPImageManagerVectorImageHandle hIcon);
	BOOL SetVectorIcon(CXTPImageManagerVectorImageHandle hIcon, XTPImageState imageState);
	BOOL SetVectorDisabledIcon(CXTPImageManagerVectorImageHandle hIcon);
	BOOL SetVectorHotIcon(CXTPImageManagerVectorImageHandle hIcon);
	BOOL SetVectorCheckedIcon(CXTPImageManagerVectorImageHandle hIcon);
	BOOL SetVectorPressedIcon(CXTPImageManagerVectorImageHandle hIcon);

	//-------------------------------------------------------------------------
	// Summary:
	//     Obtains the vector icon handle for a specified state.
	// Parameters:
	//     imageState - Image state identifier to obtain the icon for.
	// Returns:
	//     State specific icon handle.
	//-------------------------------------------------------------------------
	CXTPImageManagerVectorImageHandle GetVectorIcon() const;
	CXTPImageManagerVectorImageHandle GetVectorIcon(XTPImageState imageState) const;
	CXTPImageManagerVectorImageHandle GetVectorCheckedIcon() const;
	CXTPImageManagerVectorImageHandle GetVectorPressedIcon() const;
	CXTPImageManagerVectorImageHandle GetVectorHotIcon() const;
	CXTPImageManagerVectorImageHandle GetVectorDisabledIcon() const;

	//-------------------------------------------------------------------------
	// Summary:
	//     Draws a specified vector icon to a specified device context
	//     at a specific location.
	// Parameters:
	//     pDC - Target device context pointer.
	//     pt - Target output position.
	//     hIcon - Vector icon handle to draw.
	//     szIcon - Target icon size to draw.
	//-------------------------------------------------------------------------
	void Draw(CDC* pDC, CPoint pt, CXTPImageManagerVectorImageHandle hIcon, CSize szIcon = 0);

	//-------------------------------------------------------------------------
	// Summary:
	//     Checks if the current icon is a raster icon.
	// Returns:
	//     TRUE if the current icon is a raster icon.
	//-------------------------------------------------------------------------
	BOOL IsRasterIcon() const;

	//-------------------------------------------------------------------------
	// Summary:
	//     Checks if the current icon is a vector icon.
	// Returns:
	//     TRUE if the current icon is a vector icon.
	//-------------------------------------------------------------------------
	BOOL IsVectorIcon() const;

	//-------------------------------------------------------------------------
	// Summary:
	//     Checks if the current icon has an underfined type,
	//     i.e. no icon is associated with it.
	// Returns:
	//     TRUE if the current icon is an undefined icon.
	//-------------------------------------------------------------------------
	BOOL IsUndefinedIcon() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Use this method to scale an icon.
	// Parameters:
	//     hIcon - Icon to be scaled.
	//     szExtent - Source icon size.
	//     nWidth - Width of the result icon.
	//-----------------------------------------------------------------------
	static HICON AFX_CDECL ScaleToFit(HICON hIcon, CSize szExtent, int nWidth);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to draw a non-alpha icon in a composited Vista Glass rectangle.
	// Parameters:
	//     pDC - Pointer to device context.
	//     pt - Pointer to draw icon.
	//     szIcon - Size of the icon.
	//     hIcon - Icon handle to draw.
	//-----------------------------------------------------------------------
	static void AFX_CDECL DrawIconComposited(CDC* pDC, CPoint pt, CSize szIcon, HICON hIcon);

private:
	void CreateFadedIcon();
	void CreateShadowIcon();

	void Clear(BOOL bIcon = FALSE);
	COLORREF LightenColor(COLORREF clr, double factor);

	BOOL GetDIBBitmap(HBITMAP hBitmap, PBYTE& pBits, UINT& nBitsSize, PBITMAPINFO& pBitmapInfo,
					  UINT& nBitmapInfoSize);
	HBITMAP ReadDIBBitmap(CArchive& ar, LPBYTE* pBits1);
	void WriteDIBBitmap(CArchive& ar, HBITMAP hBitmap);

	CXTPImageManager* GetImageManager() const;

private:
	CXTPImageManagerIcon(const CXTPImageManagerIcon&)
		: m_nID(0)
		, m_nType(RasterIcon)
		, m_pMarkupContext(NULL)
	{
	}

	const CXTPImageManagerIcon& operator=(const CXTPImageManagerIcon&)
	{
		return *this;
	}

public:
	BOOL m_bDrawComposited; // Draw on Vista composited rects.

private:
	const UINT m_nID;
	enum IconType
	{
		UndefinedIcon,
		RasterIcon,
		VectorIcon
	} m_nType;

	int m_nWidth;
	int m_nHeight;
	CXTPImageManagerIconHandle m_hIcon;
	CXTPImageManagerIconHandle m_hFaded;
	CXTPImageManagerIconHandle m_hShadow;
	CXTPImageManagerIconHandle m_hHot;
	CXTPImageManagerIconHandle m_hChecked;
	CXTPImageManagerIconHandle m_hPressed;
	CXTPImageManagerIconHandle m_hDisabled;
	CXTPImageManagerIconHandle m_hDisabledAuto;
	CXTPImageManagerIconSet* m_pIconSet;
	CXTPImageManagerImageList* m_pImageList;

	CXTPMarkupContext* m_pMarkupContext;

	CXTPImageManagerVectorImageHandle m_VectorImageNormal;
	CXTPImageManagerVectorImageHandle m_VectorImageHot;
	CXTPImageManagerVectorImageHandle m_VectorImageChecked;
	CXTPImageManagerVectorImageHandle m_VectorImagePressed;
	CXTPImageManagerVectorImageHandle m_VectorImageDisabled;

private:
	friend class CXTPImageManager;
	friend class CXTPImageManagerIconSet;
	friend class CXTPImageManagerIconHandle;

#	ifdef _XTP_ACTIVEX
	//{{AFX_CODEJOCK_PRIVATE
public:
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()

	DECLARE_OLETYPELIB_EX(CXTPImageManagerIcon);
	afx_msg long OleGetHandle();
	afx_msg void OleDraw(long hDC, int x, int y, int cx, int cy, int nState);
	afx_msg LPDISPATCH OleCreatePicture(int nState);
	afx_msg LPDISPATCH OleScale(long nWidth);
//}}AFX_CODEJOCK_PRIVATE
#	endif
};

//===========================================================================
// Summary:
//     CXTPImageManagerIconSet represents the collection of icons for a specified identifier.
//===========================================================================
class _XTP_EXT_CLASS CXTPImageManagerIconSet : public CXTPCmdTarget
{
public:
	typedef CMap<UINT, UINT, CXTPImageManagerIcon*, CXTPImageManagerIcon*>
		CIconSetMap; // Icons collection.

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPImageManagerIconSet object.
	// Parameters:
	//     nID - Icon set identifier.
	//     pImageManager - Parent image manager to which CXTPImageManagerIconSet belongs.
	//-----------------------------------------------------------------------
	CXTPImageManagerIconSet(UINT nID, CXTPImageManager* pImageManager);

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPImageManagerIconSet object, handles cleanup and deallocation.
	//-----------------------------------------------------------------------
	~CXTPImageManagerIconSet();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to get the icon with the specified width.
	// Parameters:
	//     nWidth - Width of the icon to retrieve.
	//     bScaled - TRUE to scale the icon.
	// Returns:
	//     A pointer to a CXTPImageManagerIcon object.
	//-----------------------------------------------------------------------
	CXTPImageManagerIcon* GetIcon(UINT nWidth, BOOL bScaled = TRUE);

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves the collection of icons.
	// Returns:
	//     A pointer to a CIconSetMap object.
	//-----------------------------------------------------------------------
	CIconSetMap* GetIcons();

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves the identifier of the collection set.
	// Returns:
	//     Identifier of icons.
	//-----------------------------------------------------------------------
	UINT GetID() const;

	//-------------------------------------------------------------------------
	// Summary:
	//     Removes all icons.
	//-------------------------------------------------------------------------
	void RemoveAll();

	//-------------------------------------------------------------------------
	// Summary:
	//     Redraws all icons.
	//-------------------------------------------------------------------------
	void RefreshAll();

private:
	CXTPImageManagerIcon* CreateIcon(UINT nWidth);
	void SetIcon(const CXTPImageManagerIconHandle& hIcon, UINT nWidth);
	void SetDisabledIcon(const CXTPImageManagerIconHandle& hIcon, UINT nWidth);
	void SetHotIcon(const CXTPImageManagerIconHandle& hIcon, UINT nWidth);
	void SetCheckedIcon(const CXTPImageManagerIconHandle& hIcon, UINT nWidth);
	void SetPressedIcon(const CXTPImageManagerIconHandle& hIcon, UINT nWidth);

	CXTPImageManagerIcon* CreateVectorIcon(UINT nWidth);
	BOOL SetVectorIcon(IStream* pStream, UINT nWidth);
	BOOL SetVectorDisabledIcon(IStream* pStream, UINT nWidth);
	BOOL SetVectorHotIcon(IStream* pStream, UINT nWidth);
	BOOL SetVectorCheckedIcon(IStream* pStream, UINT nWidth);
	BOOL SetVectorPressedIcon(IStream* pStream, UINT nWidth);

	BOOL SetVectorIcon(CXTPImageManagerVectorImageHandle hIcon, UINT nWidth);
	BOOL SetVectorDisabledIcon(CXTPImageManagerVectorImageHandle hIcon, UINT nWidth);
	BOOL SetVectorHotIcon(CXTPImageManagerVectorImageHandle hIcon, UINT nWidth);
	BOOL SetVectorCheckedIcon(CXTPImageManagerVectorImageHandle hIcon, UINT nWidth);
	BOOL SetVectorPressedIcon(CXTPImageManagerVectorImageHandle hIcon, UINT nWidth);

#	ifdef _XTP_ACTIVEX
	//{{AFX_CODEJOCK_PRIVATE
public:
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
	DECLARE_OLETYPELIB_EX(CXTPImageManagerIconSet);
	DECLARE_ENUM_VARIANTLIST(CXTPImageManagerIconSet)

	afx_msg long OleGetCount();
	afx_msg long OleGetID();
	LPDISPATCH OleGetImage(long Width);

	//}}AFX_CODEJOCK_PRIVATE
#	endif

private:
	CXTPImageManagerIconSet(const CXTPImageManagerIconSet&)
		: m_nID(0)
	{
	}

	const CXTPImageManagerIconSet& operator=(const CXTPImageManagerIconSet&)
	{
		return *this;
	}

private:
	CIconSetMap m_mapIcons;
	const UINT m_nID;
	CXTPImageManager* m_pImageManager;

private:
	friend class CXTPImageManager;
	friend class CXTPImageManagerIcon;
};

//===========================================================================
// Summary:
//     This class is used to connect ImageManager with specified HIMAGELIST
//===========================================================================
class _XTP_EXT_CLASS CXTPImageManagerImageList
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//      Constructs a CXTPImageManagerImageList class.
	//-----------------------------------------------------------------------
	CXTPImageManagerImageList();

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPImageManagerImageList object, handles cleanup and deallocation.
	//-----------------------------------------------------------------------
	~CXTPImageManagerImageList();

public:
	//-----------------------------------------------------------------------
	// Summary:
	//      Call this method to determine if ImageList handles a command
	//      with a specified command ID.
	// Parameters:
	//      nCommand - Command ID to check.
	// Returns:
	//      TRUE if ImageList handles the command with the specified command ID.
	//-----------------------------------------------------------------------
	BOOL Lookup(UINT nCommand);

	//-----------------------------------------------------------------------
	// Summary:
	//      Draws the icon for a command with a specified command ID.
	// Parameters:
	//      nCommand - Command ID.
	// Returns:
	//      A pointer to a CXTPImageManagerIcon object that can be used to
	//      draw the icon for the command with the specified command ID.
	//-----------------------------------------------------------------------
	CXTPImageManagerIcon* GetIcon(UINT nCommand);

	//-----------------------------------------------------------------------
	// Summary:
	//      Call this method to draw a specified icon.
	// Parameters:
	//      pDC - Device context to draw into.
	//      pt - Position of the icon to draw.
	//      pIcon - Pointer to the icon to draw.
	//      szIcon - Size of the icon.
	//      clrBK - Background color of the image.
	//      clrFG - Foreground color of the image.
	//      uiFlags - Drawing style and, optionally, the overlay image.
	//-----------------------------------------------------------------------
	void Draw(CDC* pDC, CPoint pt, CXTPImageManagerIcon* pIcon, CSize szIcon,
			  COLORREF clrBK = CLR_NONE, COLORREF clrFG = CLR_NONE, UINT uiFlags = 0);

	//-----------------------------------------------------------------------
	// Summary:
	//     Cleans all temporary icons.
	//-----------------------------------------------------------------------
	void RemoveAll();

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves the size of images inside HIMAGELIST.
	// Returns:
	//     The size of images inside HIMAGELIST.
	//-----------------------------------------------------------------------
	CSize GetIconSize() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves the icon handle for an image specified by a command ID.
	// Parameters:
	//     nCommand - Command ID.
	// Returns:
	//     Icon handle, or NULL.
	//-----------------------------------------------------------------------
	HICON GetIconHandle(UINT nCommand) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves the bitmap handle for an image specified by a command ID.
	// Parameters:
	//     nCommand - Command ID.
	// Returns:
	//     Bitmap handle, or NULL.
	//-----------------------------------------------------------------------
	HBITMAP GetBitmapHandle(UINT nCommand) const;

private:
	HIMAGELIST m_hImageList;
	BOOL m_bDestroyImageList;
	UINT m_nBaseCommand;
	CXTPImageManager* m_pImageManager;
	CMap<UINT, UINT, CXTPImageManagerIcon*, CXTPImageManagerIcon*> m_mapIcons;

	friend class CXTPImageManager;
};

//===========================================================================
// Summary:
//     CXTPImageManager is a standalone class. It is used to manipulate icons.
//===========================================================================
class _XTP_EXT_CLASS CXTPImageManager : public CXTPCmdTarget
{
	friend class CXTPSingleton<CXTPImageManager>;

	DECLARE_DYNAMIC(CXTPImageManager);

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPImageManager object.
	//-----------------------------------------------------------------------
	CXTPImageManager();

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPImageManager object, handles cleanup and deallocation.
	//-----------------------------------------------------------------------
	~CXTPImageManager();

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Adds the icons.
	// Parameters:
	//     nIDResourceBitmap  - Bitmap resource identifier.
	//     lpszResourceBitmap - Bitmap resource identifier.
	//     nIDResourceToolBar - Toolbar resource identifier.
	//     bmpIcons           - Bitmap containing the icons to be added.
	//     imlIcons           - Image list containing the icons to be added.
	//     pCommands          - Pointer to an array of IDs.
	//     nCount             - Number of elements in the array pointed to by lpIDArray.
	//     szIcon             - Size of the icons.
	//     imageState         - Images state.
	//     bAlpha             - TRUE if the bitmap has alpha channel.
	// Returns:
	//     TRUE if successful, otherwise FALSE.
	// Example:
	// <code>
	//
	// static UINT buttons[] =
	// {
	//     ID_BACK,
	//     ID_FORWARD,
	//     ID_STOP,
	//     ID_REFRESH,
	//     ID_HOME
	// }
	//
	// XTPImageManager()->SetMaskColor(RGB(0, 0xFF, 0));
	//
	// XTPImageManager()->SetIcons(IDR_TOOLBAR_NORMAL_SMALL, buttons, _countof(buttons), CSize(16,
	// 16), xtpImageNormal); XTPImageManager()->SetIcons(IDR_TOOLBAR_HOT_SMALL, buttons,
	// _countof(buttons), CSize(16, 16), xtpImageHot);
	//
	// XTPImageManager()->SetIcons(IDR_TOOLBAR_NORMAL_LARGE, buttons, _countof(buttons), CSize(24,
	// 24), xtpImageNormal); XTPImageManager()->SetIcons(IDR_TOOLBAR_HOT_LARGE, buttons,
	// _countof(buttons), CSize(24, 24), xtpImageHot);
	// </code>
	// See Also:
	//     XTPImageState, SetIcon
	//-----------------------------------------------------------------------
	BOOL SetIcons(UINT nIDResourceToolBar, XTPImageState imageState = xtpImageNormal);
	BOOL SetIcons(UINT nIDResourceToolBar, UINT nIDResourceBitmap,
				  XTPImageState imageState = xtpImageNormal);
	BOOL SetIcons(CBitmap& bmpIcons, UINT* pCommands, int nCount, CSize szIcon,
				  XTPImageState imageState = xtpImageNormal, BOOL bAlpha = FALSE);
	BOOL SetIcons(UINT nIDResourceToolBar, CBitmap& bmpIcons,
				  XTPImageState imageState = xtpImageNormal);
	BOOL SetIcons(UINT nIDResourceBitmap, UINT* pCommands, int nCount, CSize szIcon,
				  XTPImageState imageState = xtpImageNormal);
	BOOL SetIcons(LPCTSTR lpszResourceBitmap, UINT* pCommands, int nCount, CSize szIcon,
				  XTPImageState imageState = xtpImageNormal);
	BOOL SetIcons(CImageList& imlIcons, UINT* pCommands, int nCount, CSize szIcon,
				  XTPImageState imageState = xtpImageNormal);
	BOOL SetIcons(UINT nIDResourceToolBar, CImageList& imlIcons,
				  XTPImageState imageState = xtpImageNormal);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method loads an icon from an .ico file.
	// Parameters:
	//     lpszFileName - Path to .ico file.
	//     nIDCommand   - Icon command.
	//     szIcon       - Size of the icons.
	//     imageState   - Images state.
	// Returns:
	//     TRUE if successful, otherwise FALSE.
	//-----------------------------------------------------------------------
	BOOL SetIconFromIcoFile(LPCTSTR lpszFileName, UINT nIDCommand, CSize szIcon,
							XTPImageState imageState);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to attach images from an HIMAGELIST control to
	//     the image manager.
	// Parameters:
	//     hImageList - Pointer to an image list that holds icons.
	//     nBaseCommand - ID to start numbering icons from the image list.
	//     bDestroyImageList - TRUE to destroy the image list control after the
	//                         icons are copied to the image manager.
	//                         By default, this parameter is FALSE.
	// Returns:
	//     TRUE if successful, otherwise FALSE.
	//-----------------------------------------------------------------------
	BOOL SetImageList(HIMAGELIST hImageList, int nBaseCommand, BOOL bDestroyImageList = FALSE);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to create a system HIMAGELIST from a bitmap
	//     and add it to the image manager.
	// Parameters:
	//     nIDResourceBitmap - Bitmap resource identifier.
	//     cx - Width of the icons.
	//     nBaseCommand - ID to start numbering icons from the image list.
	//     clrMask - Mask color for bitmap.
	// Returns:
	//     TRUE if successful, otherwise FALSE.
	// Remarks:
	//     The SetIcons method loads bitmaps and creates a HICON for each icon in the
	//     bitmap. SetImageList creates only one HIMAGELIST. Use SetImageList
	//     to create only one GDI object for all icons.
	// See Also:
	//     SetIcons
	//-----------------------------------------------------------------------
	BOOL SetImageList(UINT nIDResourceBitmap, int cx, int nBaseCommand = 0,
					  COLORREF clrMask = (COLORREF)-1);

	//-----------------------------------------------------------------------
	// Summary:
	//     Adds the icons from another image manager.
	// Parameters:
	//     pImageManager - A pointer to another image manager. The icons in
	//                     pImageManager will be added to this image manager.
	//-----------------------------------------------------------------------
	void AddIcons(CXTPImageManager* pImageManager);
	void AddIcons(CXTPImageManagerIconSet* pIconSet);

	//-----------------------------------------------------------------------
	// Summary:
	//     Adds the icon.
	// Parameters:
	//     hIcon           - Icon handle to add.
	//     nIDResourceIcon - Icon resource identifier.
	//     lpszResourceIcon - Icon resource identifier.
	//     nIDCommand      - Icon identifier.
	//     szIcon          - Size of the icons.
	//     imageState      - Images state.
	// Returns:
	//     TRUE if successful, otherwise FALSE.
	// See Also:
	//     XTPImageState, SetIcons
	//-----------------------------------------------------------------------
	BOOL SetIcon(const CXTPImageManagerIconHandle& hIcon, UINT nIDCommand, CSize szIcon = 0,
				 XTPImageState imageState = xtpImageNormal);
	BOOL SetIcon(UINT nIDResourceIcon, UINT nIDCommand, CSize szIcon = 0,
				 XTPImageState imageState = xtpImageNormal);
	BOOL SetIcon(LPCTSTR lpszResourceIcon, UINT nIDCommand, CSize szIcon = 0,
				 XTPImageState imageState = xtpImageNormal);

	// --------------------------------------------------------------------------------------------
	// Summary:
	//      Sets a vector icon for the specific image state and command identifier.
	//  Parameters:
	//      hModule - Module's instance handle where to look for the icon resource.
	//      lpRsourceType - Vector icon resource type name.
	//      nIDResource - Vector icon resource identifier.
	//      lpIDResource - Vector icon resource identifier.
	//      nIDCommand - Associated command identifier.
	//      imageState - Associated image state.
	//      hImage - Pre-loaded vector image handle.
	//      nWidth - Preferred width of the icon.
	//      nIDResourceNormal - Normal state vector icon resource identifier.
	//      nIDResourceDisabled - Disabled state vector icon resource identifier.
	//      nIDResourceHot - Hot state vector icon resource identifier.
	//      nIDResourceChecked - Checked state vector icon resource identifier.
	//      nIDResourcePressed - Pressed state vector icon resource identifier.
	//      lpIDResourceNormal - Normal state vector icon resource identifier.
	//      lpIDResourceDisabled - Disabled state vector icon resource identifier.
	//      lpIDResourceHot - Hot state vector icon resource identifier.
	//      lpIDResourceChecked - Checked state vector icon resource identifier.
	//      lpIDResourcePressed - Pressed state vector icon resource identifier.
	//      hImageNormal - Pre-loaded normal state vector image handle.
	//      hImageDisabled - Pre-loaded disabled state vector image handle.
	//      hImageHot - Pre-loaded hot state vector image handle.
	//      hImageChecked - Pre-loaded checked state vector image handle.
	//      hImagePressed - Pre-loaded pressed state vector image handle.
	// Returns:
	//      TRUE if the vector icon image has been successfully added.
	// --------------------------------------------------------------------------------------------
	BOOL SetVectorIcon(LPCTSTR lpResourceType, UINT nIDResource, UINT nIDCommand, UINT nWidth,
					   XTPImageState imageState = xtpImageNormal);
	BOOL SetVectorIcon(LPCTSTR lpResourceType, LPCTSTR lpResource, UINT nIDCommand, UINT nWidth,
					   XTPImageState imageState = xtpImageNormal);
	BOOL SetVectorIcon(HMODULE hModule, LPCTSTR lpResourceType, UINT nIDResource, UINT nIDCommand,
					   UINT nWidth, XTPImageState imageState = xtpImageNormal);
	BOOL SetVectorIcon(HMODULE hModule, LPCTSTR lpResourceType, LPCTSTR lpResource, UINT nIDCommand,
					   UINT nWidth, XTPImageState imageState = xtpImageNormal);
	BOOL SetVectorIcon(CXTPImageManagerVectorImageHandle hImage, UINT nIDCommand, UINT nWidth,
					   XTPImageState imageState = xtpImageNormal);
	BOOL SetVectorIcon(LPCTSTR lpResourceType, UINT nIDResourceNormal, UINT nIDResourceDisabled,
					   UINT nIDResourceHot, UINT nIDResourceChecked, UINT nIDResourcePressed,
					   UINT nIDCommand, UINT nWidth);
	BOOL SetVectorIcon(LPCTSTR lpResourceType, LPCTSTR lpIDResourceNormal,
					   LPCTSTR lpIDResourceDisabled, LPCTSTR lpIDResourceHot,
					   LPCTSTR lpIDResourceChecked, LPCTSTR lpIDResourcePressed, UINT nIDCommand,
					   UINT nWidth);
	BOOL SetVectorIcon(HMODULE hModule, LPCTSTR lpResourceType, UINT nIDResourceNormal,
					   UINT nIDResourceDisabled, UINT nIDResourceHot, UINT nIDResourceChecked,
					   UINT nIDResourcePressed, UINT nIDCommand, UINT nWidth);
	BOOL SetVectorIcon(HMODULE hModule, LPCTSTR lpResourceType, LPCTSTR lpIDResourceNormal,
					   LPCTSTR lpIDResourceDisabled, LPCTSTR lpIDResourceHot,
					   LPCTSTR lpIDResourceChecked, LPCTSTR lpIDResourcePressed, UINT nIDCommand,
					   UINT nWidth);
	BOOL SetVectorIcon(CXTPImageManagerVectorImageHandle hImageNormal,
					   CXTPImageManagerVectorImageHandle hImageDisabled,
					   CXTPImageManagerVectorImageHandle hImageHot,
					   CXTPImageManagerVectorImageHandle hImageChecked,
					   CXTPImageManagerVectorImageHandle hImagePressed, UINT nIDCommand,
					   UINT nWidth);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to set the mask color of the bitmaps that will be added.
	// Parameters:
	//     clrMask - Mask color to be set.
	// Returns:
	//     Previous mask color.
	//-----------------------------------------------------------------------
	COLORREF SetMaskColor(COLORREF clrMask);

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves mask color.
	// Returns:
	//     Current mask color.
	//-----------------------------------------------------------------------
	COLORREF GetMaskColor() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Mirrors images around the y-axis, useful in RTL (Right-To-Left) Languages.
	// Parameters:
	//     bDrawReverted - If TRUE, images are "flipped" or Mirrored.
	//                     This should be used when displaying images in an
	//                     application that uses RTL (Right-To-Left) Languages.
	//-----------------------------------------------------------------------
	void DrawReverted(BOOL bDrawReverted);

	//-----------------------------------------------------------------------
	// Summary:
	//     Removes all icons.
	//-----------------------------------------------------------------------
	void RemoveAll();

	//-----------------------------------------------------------------------
	// Summary:
	//     Removes the specified icon.
	// Parameters:
	//     nIDCommand - ID of the icon to be removed.
	//-----------------------------------------------------------------------
	void RemoveIcon(UINT nIDCommand);

	//-----------------------------------------------------------------------
	// Summary:
	//     Redraws all icons.
	//-----------------------------------------------------------------------
	void RefreshAll();

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is used by CommandBars to add custom icons.
	// Parameters:
	//     hIcon - Icon handler to be added.
	// Returns:
	//     Identifier of the newly added icon.
	//-----------------------------------------------------------------------
	UINT AddCustomIcon(const CXTPImageManagerIconHandle& hIcon);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to get the icon with the specified ID and width.
	// Parameters:
	//     nCommand - ID of the icon to be retrieved.
	//     nWidth - Width of the icon to be retrieved.
	// Returns:
	//     A pointer to a CXTPImageManagerIcon object.
	//-----------------------------------------------------------------------
	CXTPImageManagerIcon* GetImage(UINT nCommand, int nWidth = 16) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to determine if a non-scaled image exists in the image manager.
	// Parameters:
	//     nCommand - Image identifier to check.
	//     nWidth - Image width to check.
	// Returns:
	//     TRUE if a non-scaled image exists in the image manager.
	// See Also:
	//     GetImage
	//-----------------------------------------------------------------------
	BOOL IsPrimaryImageExists(UINT nCommand, int nWidth) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to get the icon set with the specified ID.
	// Parameters:
	//     nCommand - ID of the icon set to be retrieved.
	// Returns:
	//     A pointer to a CXTPImageManagerIconSet object.
	//-----------------------------------------------------------------------
	CXTPImageManagerIconSet* GetIconSet(UINT nCommand) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to get the icon set with the specified ID from an image list.
	// Parameters:
	//     nCommand - ID of the icon set to be retrieved.
	// Returns:
	//     A pointer to a CXTPImageManagerImageList object.
	//-----------------------------------------------------------------------
	CXTPImageManagerImageList* GetImageList(UINT nCommand) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves a collection of images.
	// Returns:
	//     A CMap object.
	//-----------------------------------------------------------------------
	CMap<UINT, UINT, CXTPImageManagerIconSet*, CXTPImageManagerIconSet*>* GetImages();

	//-----------------------------------------------------------------------
	// Summary:
	//     Checks if an icon set exists with a specified ID.
	// Parameters:
	//     nCommand - ID of the icon set to check for.
	// Returns:
	//     TRUE if successful, otherwise FALSE.
	//-----------------------------------------------------------------------
	BOOL Lookup(UINT nCommand) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Checks if the icons are drawn mirrored/reverted.
	// Parameters:
	//     pDC - Pointer to the device context to check.
	// Returns:
	//     TRUE if the icons are drawn mirrored/reverted,
	//     FALSE if icons are drawn normal.
	//-----------------------------------------------------------------------
	BOOL IsDrawReverted() const;
	BOOL IsDrawReverted(CDC* pDC) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves resources of a specified size.
	// Parameters:
	//     szIcon - Size of resources to retrieve.
	// Returns:
	//     Pointer to a CXTPImageManagerResource object containing all
	//     icons of the specified size.
	//-----------------------------------------------------------------------
	CXTPImageManagerResource* GetResource(CSize szIcon);

	//-----------------------------------------------------------------------
	// Summary:
	//     Either reads icons from or writes icons to an archive.
	// Parameters:
	//     nCommand - Specified command to serialize.
	//     ar - A CArchive object to serialize to or from.
	//     pPX - A CXTPPropExchange object to serialize to or from.
	//-----------------------------------------------------------------------
	void Serialize(CArchive& ar);
	void Serialize(UINT nCommand, CArchive& ar);

	//-----------------------------------------------------------------------
	// Summary:
	//     Either reads icons from or writes icons to an archive.
	// Parameters:
	//     nCommand - Specified command to serialize.
	//     ar - A CArchive object to serialize to or from.
	//     pPX - A CXTPPropExchange object to serialize to or from.
	//-----------------------------------------------------------------------
	void DoPropExchange(CXTPPropExchange* pPX);
	void DoPropExchange(UINT nCommand, CXTPPropExchange* pPX);

	//-----------------------------------------------------------------------
	// Summary:
	//     Checks if a bitmap with a specified resource has alpha channel.
	// Parameters:
	//     nIDResourceBitmap - Resource identifier of the bitmap to check.
	// Returns:
	//     TRUE if the bitmap has alpha channel, otherwise FALSE.
	//-----------------------------------------------------------------------
	static BOOL AFX_CDECL IsAlphaBitmapResource(UINT nIDResourceBitmap);

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines if a resource contains a .PNG image.
	// Parameters:
	//     nIDResourceBitmap - ID of the resource to check.
	// Returns:
	//     TRUE if the resource contains a .PNG image.
	//-----------------------------------------------------------------------
	static BOOL AFX_CDECL IsPngBitmapResource(UINT nIDResourceBitmap);

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines if the OS supports alpha icons.
	// Returns:
	//     TRUE if successful; otherwise returns FALSE.
	// See Also:
	//     IsAlphaIconsImageListSupported
	//-----------------------------------------------------------------------
	BOOL IsAlphaIconsSupported() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines if the OS supports alpha image list.
	// Returns:
	//     TRUE if successful; otherwise returns FALSE
	// See Also:
	//     IsAlphaIconsSupported
	//-----------------------------------------------------------------------
	static BOOL AFX_CDECL IsAlphaIconsImageListSupported();

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves the color of the top-left pixel.
	// Parameters:
	//     bmp       - CBitmap reference.
	//     nIDBitmap - Bitmap identifier.
	//     pt        - Location of the pixel to retrieve.
	// Returns:
	//     A COLORREF value that will be used as a transparent color.
	//-----------------------------------------------------------------------
	static COLORREF AFX_CDECL GetBitmapMaskColor(CBitmap& bmp, CPoint pt = 0);
	static COLORREF AFX_CDECL GetBitmapMaskColor(UINT nIDBitmap, CPoint pt = 0);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method creates a 24bit bitmap from a 32bit alpha bitmap
	//     to draw it in old OSs.
	// Parameters:
	//     bmpAlpha - Resource bitmap identifier.
	//     clrMask  - Mask color to be used.
	//     nIDBitmap - Resource bitmap identifier.
	// Returns:
	//     Reduced bitmap handler.
	//-----------------------------------------------------------------------
	static HBITMAP AFX_CDECL ResampleAlphaLayer(HBITMAP bmpAlpha, COLORREF clrMask);
	static HBITMAP AFX_CDECL ResampleAlphaLayer(UINT nIDBitmap, COLORREF clrMask);

	//-----------------------------------------------------------------------
	// Summary:
	//     The TransparentBlt function performs a bit-block transfer of the color
	//     data corresponding to a rectangle of pixels from a specified source
	//     device context into a destination device context.
	// Parameters:
	//     hdcDest - Handle to destination DC.
	//     rcDest - Destination rectangle.
	//     hdcSrc - Handle to source DC.
	//     rcSrc - Source rectangle.
	//     crTransparent - Color to make transparent.
	// Returns:
	//     TRUE if successful.
	// See Also:
	//     AlphaBlend
	//-----------------------------------------------------------------------
	BOOL TransparentBlt(HDC hdcDest, const CRect& rcDest, HDC hdcSrc, const CRect& rcSrc,
						UINT crTransparent) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     The AlphaBlend function displays bitmaps that have transparent or
	//     semi-transparent pixels.
	// Parameters:
	//     hdcDest - Handle to destination DC.
	//     rcDest - Destination rectangle.
	//     hdcSrc - Handle to source DC.
	//     rcSrc - Source rectangle.
	// Returns:
	//     TRUE if successful.
	// See Also:
	//     TransparentBlt
	//-----------------------------------------------------------------------
	BOOL AlphaBlend(HDC hdcDest, const CRect& rcDest, HDC hdcSrc, const CRect& rcSrc) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     The AlphaBlend function displays bitmaps that have transparent or
	//     semi-transparent pixels.
	// Parameters:
	//     hdcDest - Handle to destination DC.
	//     rcDest - Destination rectangle.
	//     hdcSrc - Handle to source DC.
	//     rcSrc - Source rectangle.
	//     Transparency - Transparency for whole image to draw.
	// Returns:
	//     TRUE if successful.
	// See Also:
	//     TransparentBlt
	//-----------------------------------------------------------------------
	BOOL AlphaBlend2(HDC hdcDest, const CRect& rcDest, HDC hdcSrc, const CRect& rcSrc,
					 BYTE Transparency) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This function makes a rectangle in a specified device context disabled.
	// Parameters:
	//     hDC              - A handle to DC.
	//     rcRect           - A rectangle to be disabled.
	//     clrDisabledLight - The lightest color of icon to generate.
	//     clrDisabledDark  - The darkest color of icon to generate.
	// Returns:
	//     TRUE if successful.
	// See Also:
	//     BlackWhiteBitmap
	//-----------------------------------------------------------------------
	BOOL DisableBitmap(HDC hDC, const CRect& rcRect, COLORREF clrDisabledLight = (COLORREF)-1,
					   COLORREF clrDisabledDark = (COLORREF)-1);

	//-----------------------------------------------------------------------
	// Summary:
	//     This function makes a rectangle in a specified device context gray scale.
	// Parameters:
	//     hDC                    - A handle to DC.
	//     rcRect                 - A rectangle to be made gray scale.
	//     nBlackAndWhiteContrast - Black-white contrast as integer from 0 to 255.
	// Returns:
	//     TRUE if successful.
	// See Also:
	//     DisableBitmap
	//-----------------------------------------------------------------------
	BOOL BlackWhiteBitmap(HDC hDC, const CRect& rcRect, int nBlackAndWhiteContrast = 0);

	//-----------------------------------------------------------------------
	// Summary:
	//     This helper method creates a 32bit bitmap.
	// Parameters:
	//     hDC - Pointer to a valid context (can be NULL).
	//     iWidth - Width of bitmap.
	//     iHeight - Height of bitmap.
	//     lpBits - Pointer to bits to return.
	// Returns:
	//     The newly created bitmap handle.
	//-----------------------------------------------------------------------
	static HBITMAP AFX_CDECL Create32BPPDIBSection(HDC hDC, int iWidth, int iHeight,
												   LPBYTE* lpBits = NULL);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method adds all icons of a specified size from resources.
	// Parameters:
	//     hInst - Module instance to load icons from.
	//     lpszResourceName - Icon resource identifier.
	//     nIDCommand - Command to assign for the icon.
	//     szIcon - Size of the icon to load (0 to load all formats).
	//     imageState - Image state to assign for the icon.
	// Returns:
	//     TRUE if successful.
	//-----------------------------------------------------------------------
	BOOL SetIconFromResource(HINSTANCE hInst, LPCTSTR lpszResourceName, UINT nIDCommand,
							 CSize szIcon, XTPImageState imageState);

	//-----------------------------------------------------------------------
	// Summary:
	//     Fall back for systems without MSIMG32.DLL.
	//-----------------------------------------------------------------------
	static BOOL AFX_CDECL McTransparentBlt(HDC hdcDest, int nXOriginDest, int nYOriginDest,
										   int nWidthDest, int nHeightDest, HDC hdcSrc,
										   int nXOriginSrc, int nYOriginSrc, int nWidthSrc,
										   int nHeightSrc, UINT crTransparent);

private:
	BOOL LoadToolbar(UINT nIDResourceToolBar, UINT*& pItems, int& nCount, CSize& szIcon) const;

	UINT PasteCustomImage(COleDataObject& data);
	void CopyImage(UINT nCommand);
	BOOL SplitBitmap(HBITMAP hbmSource, int nCount, HBITMAP* pDest) const;
	BOOL IsWindow2000() const;
	BOOL BitmapsCompatible(LPBITMAP lpbm1, LPBITMAP lpbm2) const;

	BOOL BlendImages(HBITMAP hbmSrc1, BOOL bRTL1, HBITMAP hbmSrc2, BOOL bRTL2,
					 HBITMAP hbmDst) const;
	BOOL DoAlphaBlend(HDC hdcDest, int nXOriginDest, int nYOriginDest, int nWidthDest,
					  int nHeightDest, HDC hdcSrc, int nXOriginSrc, int nYOriginSrc, int nWidthSrc,
					  int nHeightSrc) const;
	BOOL DoDisableBitmap(HBITMAP hBmp, COLORREF clrDisabledLight = (COLORREF)-1,
						 COLORREF clrDisabledDark = (COLORREF)-1, int nBlackAndWhiteContrast = -1);

	BOOL ResourceToStream(HMODULE hModule, LPCTSTR lpResource, LPCTSTR lpResourceType,
						  IStream** ppStream);
	BOOL FileToStream(LPCTSTR path, IStream** ppStream);
	BOOL SetVectorIcon(IStream* pStream, UINT nIDCommand, UINT nWidth, XTPImageState imageState);

public:
	static double m_dDisabledBrightnessFactor; // Brightness factor of disabled icons.
	static double m_dDisabledAlphaFactor;	   // Alpha factor of disabled icons.
	static BOOL m_bAutoResample; // TRUE to automatically re-sample alpha bitmaps if msimg32 is not
								 // found.
	static CLIPFORMAT m_nImageClipFormat; // Clipboard format of icons.
	static CLIPFORMAT m_nAlphaClipFormat; // Clipboard format of icons.
	BOOL m_bUseResources;				  // TRUE to create a single bitmap for all icons.

private:
	COLORREF m_clrMask;
	CMap<UINT, UINT, CXTPImageManagerIconSet*, CXTPImageManagerIconSet*> m_mapImages;
	CArray<CXTPImageManagerImageList*, CXTPImageManagerImageList*> m_arrImageList;
	CMap<DWORD, DWORD, CXTPImageManagerResource*, CXTPImageManagerResource*> m_mapResources;

	UINT m_nCustomID;
	BOOL m_bDrawReverted;

private:
	friend _XTP_EXT_CLASS CXTPImageManager* AFX_CDECL XTPImageManager();
	friend class CXTPCustomizeSheet;
	friend class CXTPImageManagerIcon;
	friend class CXTPImageManagerIconHandle;

	//{{AFX_CODEJOCK_PRIVATE
public:
	static CXTPImageManager* AFX_CDECL FromDispatch(LPDISPATCH pDisp);

#	ifdef _XTP_ACTIVEX
	/*  x64 #ifdef below is a fix for OLE_HANDLE. While HBITMAP/HICON are still 32-bit on 64-bit
	   Windows, OLE_HANDLE (long) can still be used. But OLE_HANDLE cannot be used as HMODULE which
	   is always 64-bit
	*/

#		ifndef _WIN64
#			define MODULE_HANDLE OLE_HANDLE
#			define MODULE_HANDLE_TYPE VTS_I4
#			define GDI_HANDLE long
#			define GDI_HANDLE_TYPE VTS_I4
#		else
#			define MODULE_HANDLE LONG64
#			define MODULE_HANDLE_TYPE VTS_I8
#			define GDI_HANDLE LONG64
#			define GDI_HANDLE_TYPE VTS_I8
#		endif

	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()

	DECLARE_OLETYPELIB_EX(CXTPImageManager);
	DECLARE_ENUM_VARIANTLIST(CXTPImageManager)

	afx_msg long OleGetCount();
	afx_msg void OleAddIcon(GDI_HANDLE Handle, long Command, long imageState);
	afx_msg void OleLoadIcon(LPCTSTR Path, long Command, long imageState);
	afx_msg void OleLoadBitmap(LPCTSTR Path, const VARIANT& Commands, long imageState);
	afx_msg void OleAddBitmap(GDI_HANDLE Handle, const VARIANT& Commands, long imageState,
							  BOOL bAlphaBitmap);
	afx_msg void OleAddIcons(LPDISPATCH lpImageManagerIcons);
	afx_msg LPDISPATCH OleGetImage(long Command, long Width);
	afx_msg void OleLoadBitmapFromResource(MODULE_HANDLE Module, long Resource,
										   const VARIANT& Commands, long imageState);
	afx_msg void OleLoadIconFromResource(MODULE_HANDLE Module, long Resource, long Command,
										 long imageState);
	afx_msg void OleDoPropExchange(LPDISPATCH lpPropExchage);

	afx_msg BOOL OleLoadVectorIcon(LPCTSTR Path, long nIDCommand, long nWidth,
								   XTPImageState imageState);
	afx_msg BOOL OleLoadVectorIconCode(LPCTSTR Code, long nIDCommand, long nWidth,
									   XTPImageState imageState);
	afx_msg BOOL OleLoadVectorIconFromResource(MODULE_HANDLE Module, LPCTSTR ResourceType,
											   long Resource, long nIDCommand, long nWidth,
											   XTPImageState imageState);

//}}AFX_CODEJOCK_PRIVATE
#	endif
};

//===========================================================================
// Summary:
//    The CXTPTempColorMask class can be used to temporarily set the color mask
//    used by XTPImageManager when using images with different color masks.
//    When the destructor is called, the default mask is reset for XTPImageManager.
// Example:
//    The following example demonstrates using CXTPTempColorMask:
// <code>
//    // temporarily change the mask color.
//    CXTPTempColorMask mask(RGB(0,255,0));
//    XTPImageManager()->SetIcons(IDB_FAVORITES,
//        icons, _countof(icons) CSize(16,16));
//
//    // set another mask color, CXTPTempColorMask will handle resetting
//    // the default color mask when it is destroyed.
//    XTPImageManager()->SetMaskColor(RGB(255,0,255));
//    XTPImageManager()->SetIcons(IDR_TOOLBAR_MASKED,
//        buttons, _countof(buttons), CSize(16,16), xtpImageNormal);
// </code>
// See Also:
//    CXTPImageManager::GetMaskColor, CXTPImageManager::SetMaskColor,
//    CXTPPaintManagerColor::GetStandardColor
//========================================================================
class _XTP_EXT_CLASS CXTPTempColorMask
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPTempColorMask object, initializes the application
	//     defined mask color, and saves the default mask color
	//     used by XTPImageManager.
	// Parameters:
	//     crNewMask - An RGB value that represents the new color mask value
	//                 used by XTPImageManager.
	//-----------------------------------------------------------------------
	CXTPTempColorMask(COLORREF crNewMask);
	CXTPTempColorMask(CXTPImageManager* pImageManager, COLORREF crNewMask);

	//-----------------------------------------------------------------------
	// Summary:
	//     Destructor for CXTPTempColorMask. Resets the default mask color
	//     used by XTPImageManager back to its original color value.
	//-----------------------------------------------------------------------
	~CXTPTempColorMask();

private:
	COLORREF m_crMask;
	CXTPImageManager* m_pImageManager;
};

//===========================================================================
// Summary:
//     XTPImageManager returns a pointer to the common CXTPImageManager class.
// Returns:
//     A pointer to the common CXTPImageManager class.
// See Also:
//    CXTPImageManager
//===========================================================================
_XTP_EXT_CLASS CXTPImageManager* AFX_CDECL XTPImageManager();

//////////////////////////////////////////////////////////////////////////

AFX_INLINE CXTPImageManagerIconHandle::operator HICON() const
{
	return m_hIcon;
}

AFX_INLINE CXTPImageManagerImageList* CXTPImageManagerIcon::GetImageList()
{
	return m_pImageList;
}

AFX_INLINE const CXTPImageManagerImageList* CXTPImageManagerIcon::GetImageList() const
{
	return m_pImageList;
}

AFX_INLINE UINT CXTPImageManagerIconSet::GetID() const
{
	return m_nID;
}

AFX_INLINE BOOL CXTPImageManagerIcon::IsScaled() const
{
	return FALSE;
}

AFX_INLINE CXTPImageManagerVectorImageHandle CXTPImageManagerIcon::GetVectorCheckedIcon() const
{
	return GetVectorIcon(xtpImageChecked);
}

AFX_INLINE CXTPImageManagerVectorImageHandle CXTPImageManagerIcon::GetVectorPressedIcon() const
{
	return GetVectorIcon(xtpImagePressed);
}

AFX_INLINE CXTPImageManagerVectorImageHandle CXTPImageManagerIcon::GetVectorHotIcon() const
{
	return GetVectorIcon(xtpImageHot);
}

AFX_INLINE CXTPImageManagerVectorImageHandle CXTPImageManagerIcon::GetVectorDisabledIcon() const
{
	return GetVectorIcon(xtpImageDisabled);
}

AFX_INLINE BOOL CXTPImageManagerIcon::SetVectorDisabledIcon(CXTPImageManagerVectorImageHandle hIcon)
{
	return SetVectorIcon(hIcon, xtpImageDisabled);
}

AFX_INLINE BOOL CXTPImageManagerIcon::SetVectorHotIcon(CXTPImageManagerVectorImageHandle hIcon)
{
	return SetVectorIcon(hIcon, xtpImageHot);
}

AFX_INLINE BOOL CXTPImageManagerIcon::SetVectorCheckedIcon(CXTPImageManagerVectorImageHandle hIcon)
{
	return SetVectorIcon(hIcon, xtpImageChecked);
}

AFX_INLINE BOOL CXTPImageManagerIcon::SetVectorPressedIcon(CXTPImageManagerVectorImageHandle hIcon)
{
	return SetVectorIcon(hIcon, xtpImagePressed);
}

AFX_INLINE BOOL CXTPImageManagerIcon::IsRasterIcon() const
{
	return RasterIcon == m_nType;
}

AFX_INLINE BOOL CXTPImageManagerIcon::IsVectorIcon() const
{
	return VectorIcon == m_nType;
}

AFX_INLINE BOOL CXTPImageManagerIcon::IsUndefinedIcon() const
{
	return UndefinedIcon == m_nType;
}

AFX_INLINE CXTPImageManagerIconSet::CIconSetMap* CXTPImageManagerIconSet::GetIcons()
{
	return &m_mapIcons;
}

AFX_INLINE CMap<UINT, UINT, CXTPImageManagerIconSet*, CXTPImageManagerIconSet*>*
	CXTPImageManager::GetImages()
{
	return &m_mapImages;
}

AFX_INLINE void CXTPImageManager::DrawReverted(BOOL bDrawReverted)
{
	m_bDrawReverted = bDrawReverted;
}

AFX_INLINE BOOL CXTPImageManager::IsDrawReverted() const
{
	return m_bDrawReverted;
}

AFX_INLINE CXTPTempColorMask::CXTPTempColorMask(COLORREF crNewMask)
{
	m_pImageManager = XTPImageManager();
	m_crMask		= m_pImageManager->SetMaskColor(crNewMask);
}

AFX_INLINE CXTPTempColorMask::CXTPTempColorMask(CXTPImageManager* pImageManager, COLORREF crNewMask)
{
	m_pImageManager = pImageManager;
	m_crMask		= m_pImageManager->SetMaskColor(crNewMask);
}

AFX_INLINE CXTPTempColorMask::~CXTPTempColorMask()
{
	m_pImageManager->SetMaskColor(m_crMask);
}

//-----------------------------------------------------------------------
// Summary:
//      Scales bitmap image to the current DPI level.
// Parameters:
//      src - Source bitmap pointer.
//      dst - Destination bitmap pointer.
// Returns:
//      TRUE if the source bitmap was successfully scaled to the destination bitmap object.
//-----------------------------------------------------------------------
_XTP_EXT_CLASS BOOL AFX_CDECL XTPScaleBitmap(CBitmap* src, CBitmap* dst);

//{{AFX_CODEJOCK_PRIVATE
_XTP_DEPRECATED_IN_FAVOR(XTPScaleBitmap)
AFX_INLINE BOOL ScaleBitmap(CBitmap* src, CBitmap* dst)
{
	return XTPScaleBitmap(src, dst);
}
//}}AFX_CODEJOCK_PRIVATE

#	include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif //#if !defined(__XTPIMAGEMANAGER_H__)
