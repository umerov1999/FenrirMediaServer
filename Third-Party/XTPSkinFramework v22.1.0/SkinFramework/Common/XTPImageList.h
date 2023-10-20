// XTPImageList.h interface
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
#if !defined(__XTPIMAGELIST_H__)
#	define __XTPIMAGELIST_H__
//}}AFX_CODEJOCK_PRIVATE

#	if _MSC_VER > 1000
#		pragma once
#	endif // _MSC_VER > 1000

#	include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

//===========================================================================
// Summary:
//     CXTPImageList extends CImageList with add additional functionality for
//     images resizing that depends on DPI.
//===========================================================================
class _XTP_EXT_CLASS CXTPImageList : public CObject
{
	DECLARE_DYNAMIC(CXTPImageList);

public:
	//{{AFX_CODEJOCK_PRIVATE
	typedef CXTPGdiObjectPtrProxy<CImageList, CXTPImageList> XTPImageListPtrProxy;
	//}}AFX_CODEJOCK_PRIVATE

private:
	CImageList* m_imageList;
	int m_imageCx;

	//-----------------------------------------------------------------------
	// Summary:
	//     Initializes an image list and attaches it to a CXTPImageList object.
	// Parameters:
	//     bitmap - Pointer to the bitmap containing the image or images.
	//     cx - Dimensions of each image, in pixels.
	//     nGrow - Number of images by which the image list can grow when the
	//             system needs to resize the list to make room for new images.
	//             This parameter represents the number of new images the resized
	//             image list can contain.
	//     crMask - Color used to generate a mask. Each pixel of this color
	//             in the specified bitmap is changed to black, and the
	//             corresponding bit in the mask is set to one.
	// Returns:
	//     Nonzero if successful; otherwise 0.
	// Remarks:
	//     If image has size 16x16 px and DPI settings set 150%, then image will be
	//     resized to 24x24 px and added to imagelist.
	//-----------------------------------------------------------------------
	BOOL Create(CBitmap* bitmap, int cx, int nGrow, COLORREF crMask);

	//-----------------------------------------------------------------------
	// Summary:
	//     Resizes image specified by the source bitmap and stores it into
	//     the destination bitmap.
	// Parameters:
	//     src - Pointer to the source bitmap.
	//     dst - Pointer to the destination bitmap while will contain the the
	//           resized image upon successful completion.
	// Returns:
	//     Nonzero if successful; otherwise 0.
	// Remarks:
	//     If image has size 16x16 px and DPI settings set 150%, then image will be
	//     resized to 24x24 px.
	//-----------------------------------------------------------------------
	BOOL Resize(CBitmap* src, CBitmap* dst);

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPImageList object
	//-----------------------------------------------------------------------
	CXTPImageList();

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPImageList object, handles cleanup and deallocation
	//-----------------------------------------------------------------------
	virtual ~CXTPImageList();

	//-----------------------------------------------------------------------
	// Summary:
	//     Initializes an image list and attaches it to a CXTPImageList object.
	// Parameters:
	//     cx - Dimensions of each image, in pixels.
	//     cy - Dimensions of each image, in pixels.
	//     nFlags - Specifies the type of image list to create. This parameter
	//              can be a combination of the following values, but it can
	//              include only one of the ILC_COLOR values.
	//     nInitial - Number of images that the image list initially contains.
	//     nGrow - Number of images by which the image list can grow when the
	//             system needs to resize the list to make room for new images.
	//             This parameter represents the number of new images the resized
	//             image list can contain.
	// Returns:
	//     Nonzero if successful; otherwise 0.
	//-----------------------------------------------------------------------
	BOOL Create(int cx, int cy, UINT nFlags, int nInitial, int nGrow);

	//-----------------------------------------------------------------------
	// Summary:
	//     Initializes an image list and attaches it to a CXTPImageList object.
	// Parameters:
	//     nBitmapID - Resource IDs of the bitmap to be associated with the image list.
	//     cx - Dimensions of each image, in pixels.
	//     nGrow - Number of images by which the image list can grow when the
	//             system needs to resize the list to make room for new images.
	//             This parameter represents the number of new images the resized
	//             image list can contain.
	//     crMask - Color used to generate a mask. Each pixel of this color
	//             in the specified bitmap is changed to black, and the
	//             corresponding bit in the mask is set to one.
	// Returns:
	//     Nonzero if successful; otherwise 0.
	//-----------------------------------------------------------------------
	BOOL Create(UINT nBitmapID, int cx, int nGrow, COLORREF crMask);

	//-----------------------------------------------------------------------
	// Summary:
	//     Initializes an image list and attaches it to a CXTPImageList object.
	// Parameters:
	//     lpszBitmapID - A string containing the resource IDs of the images.
	//     cx - Dimensions of each image, in pixels.
	//     nGrow - Number of images by which the image list can grow when the
	//             system needs to resize the list to make room for new images.
	//             This parameter represents the number of new images the resized
	//             image list can contain.
	//     crMask - Color used to generate a mask. Each pixel of this color
	//             in the specified bitmap is changed to black, and the
	//             corresponding bit in the mask is set to one.
	// Returns:
	//     Nonzero if successful; otherwise 0.
	//-----------------------------------------------------------------------
	BOOL Create(LPCTSTR lpszBitmapID, int cx, int nGrow, COLORREF crMask);

	//-----------------------------------------------------------------------
	// Summary:
	//     Initializes an image list and attaches it to a CXTPImageList object.
	// Parameters:
	//     imagelist1 - A reference to a CXTPImageList object.
	//     nImage1 - Index of the first existing image.
	//     imagelist2 - A reference to a CXTPImageList object.
	//     nImage2 - Index of the second existing image.
	//     dx - Offset of the x-axis of the second image in relationship to the first image, in
	//     pixels. dy - Offset of the y-axis of the second image in relationship to the first image,
	//     in pixels.
	// Returns:
	//     Nonzero if successful; otherwise 0.
	//-----------------------------------------------------------------------
	BOOL Create(CXTPImageList& imagelist1, int nImage1, CXTPImageList& imagelist2, int nImage2,
				int dx, int dy);

	//-----------------------------------------------------------------------
	// Summary:
	//     Initializes an image list and attaches it to a CXTPImageList object.
	// Parameters:
	//     pImageList - A pointer to a CXTPImageList object.
	// Returns:
	//     Nonzero if successful; otherwise 0.
	//-----------------------------------------------------------------------
	BOOL Create(CXTPImageList* pImageList);

	//-----------------------------------------------------------------------
	// Summary:
	//     Use this operator to get the attached handle of the CXTPImageList object.
	// Returns:
	//     If successful, a handle to the image list represented by the CXTPImageList object;
	//     otherwise NULL.
	//-----------------------------------------------------------------------
	operator HIMAGELIST() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this function to retrieve the m_hImageList data member.
	// Returns:
	//     A handle to the attached image list; otherwise NULL if no object is attached.
	//-----------------------------------------------------------------------
	HIMAGELIST GetSafeHandle() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Attaches an image list to a CXTPImageList object.
	// Parameters:
	//     hImageList - A handle to an image list object.
	// Returns:
	//     Nonzero if the attachment was successful; otherwise 0.
	//-----------------------------------------------------------------------
	BOOL Attach(HIMAGELIST hImageList);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this function to detach an image list object from a CXTPImageList object.
	// Returns:
	//     A handle to an image list object.
	//-----------------------------------------------------------------------
	HIMAGELIST Detach();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this function to retrieve the number of images in an image list.
	// Returns:
	//     The number of images.
	//-----------------------------------------------------------------------
	int GetImageCount() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this function to set the background color for an image list.
	// Parameters:
	//     cr - Background color to set. It can be CLR_NONE. In that case,
	//          images are drawn transparently using the mask.
	// Returns:
	//     The previous background color if successful; otherwise CLR_NONE.
	//-----------------------------------------------------------------------
	COLORREF SetBkColor(COLORREF cr);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this function to retrieve the current background color for an image list.
	// Returns:
	//     The RGB color value of the CXTPImageList object background color.
	//-----------------------------------------------------------------------
	COLORREF GetBkColor() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this function to retrieve information about an image.
	// Parameters:
	//     nImage - Zero-based index of the image.
	//     pImageInfo - Pointer to an IMAGEINFO structure that receives information
	//                  about the image. The information in this structure can be
	//                  used to directly manipulate the bitmaps for the image.
	// Returns:
	//     Nonzero if successful; otherwise 0.
	//-----------------------------------------------------------------------
	BOOL GetImageInfo(int nImage, IMAGEINFO* pImageInfo) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this function to delete an image list.
	// Returns:
	//     Nonzero if successful; otherwise 0.
	//-----------------------------------------------------------------------
	BOOL DeleteImageList();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to reset the number of images in a CXTPImageList object.
	// Parameters:
	//     uNewCount - The value specifying the new total number of images in the image list.
	// Returns:
	//     Nonzero if successful; otherwise zero.
	//-----------------------------------------------------------------------
	BOOL SetImageCount(UINT uNewCount);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this function to add one or more images or an icon to an image list.
	// Parameters:
	//     pbmImage - Pointer to the bitmap containing the image or images. The number
	//                of images is inferred from the width of the bitmap.
	//     pbmMask - Pointer to the bitmap containing the mask. If no mask is used
	//                with the image list, this parameter is ignored.
	// Returns:
	//     Zero-based index of the first new image if successful; otherwise -1.
	//-----------------------------------------------------------------------
	int Add(CBitmap* pbmImage, CBitmap* pbmMask);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this function to add one or more images or an icon to an image list.
	// Parameters:
	//     pbmImage - Pointer to the bitmap containing the image or images. The number
	//                of images is inferred from the width of the bitmap.
	//     crMask - Color used to generate the mask. Each pixel of this color in the
	//              given bitmap is changed to black and the corresponding bit in the
	//              mask is set to one.
	// Returns:
	//     Zero-based index of the first new image if successful; otherwise -1.
	//-----------------------------------------------------------------------
	int Add(CBitmap* pbmImage, COLORREF crMask);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this function to add one or more images or an icon to an image list.
	// Parameters:
	//     hIcon - Handle of the icon that contains the bitmap and mask for the new image.
	// Returns:
	//     Zero-based index of the first new image if successful; otherwise -1.
	//-----------------------------------------------------------------------
	int Add(HICON hIcon);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this function to remove an image from an image list object.
	// Parameters:
	//     nImage - Zero-based index of the image to remove.
	// Returns:
	//     Nonzero if successful; otherwise 0.
	//-----------------------------------------------------------------------
	BOOL Remove(int nImage);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this function to replace an image in an image list with a new image.
	// Parameters:
	//     nImage - Zero-based index of the image to replace.
	//     pbmImage - A pointer to the bitmap containing the image.
	//     pbmMask - A pointer to the bitmap containing the mask. If no mask is used
	//               with the image list, this parameter is ignored.
	// Returns:
	//     Nonzero if successful; otherwise 0.
	//-----------------------------------------------------------------------
	BOOL Replace(int nImage, CBitmap* pbmImage, CBitmap* pbmMask);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this function to replace an image in an image list with a new image.
	// Parameters:
	//     nImage - Zero-based index of the image to replace.
	//     hIcon - A handle to the icon that contains the bitmap and mask for the new image.
	// Returns:
	//     Zero-based index of the image if successful; otherwise -1.
	//-----------------------------------------------------------------------
	int Replace(int nImage, HICON hIcon);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this function to create an icon based on an image and its related mask in an image
	//     list.
	// Parameters:
	//     nImage - Zero-based index of the image.
	// Returns:
	//     Handle of the icon if successful; otherwise NULL.
	//-----------------------------------------------------------------------
	HICON ExtractIcon(int nImage);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this function to draw the image that is being dragged during a drag-and-drop
	//     operation.
	// Parameters:
	//     pDC - Pointer to the destination device context.
	//     nImage - Zero-based index of the image to draw.
	//     pt - Location at which to draw within the specified device context.
	//     nStyle - Flag specifying the drawing style. Refer to the platform SDK's documentation
	//              on ImageList_Draw function for ILD_* flags summary.
	// Returns:
	//     Nonzero if successful; otherwise 0.
	//-----------------------------------------------------------------------
	BOOL Draw(CDC* pDC, int nImage, POINT pt, UINT nStyle);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this function to add the zero-based index of an image to the list of images to be
	//     used as overlay masks.
	// Parameters:
	//     nImage - Zero-based index of the image to use as an overlay mask.
	//     nOverlay - One-based index of the overlay mask.
	// Returns:
	//     Nonzero if successful; otherwise 0.
	//-----------------------------------------------------------------------
	BOOL SetOverlayImage(int nImage, int nOverlay);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function implements the behavior of the Win32 function ImageList_Copy, as
	//     described in the Windows SDK.
	// Parameters:
	//     iDst - The zero-based index of the image to be used as the destination of the copy
	//            operation.
	//     iSrc - The zero-based index of the image to be used as the source of the copy
	//            operation.
	//     uFlags - The bit flag value that specifies the type of copy operation to be
	//              made.
	// Returns:
	//     Nonzero if successful; otherwise zero.
	//-----------------------------------------------------------------------
	BOOL Copy(int iDst, int iSrc, UINT uFlags = ILCF_MOVE);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function implements the behavior of the Win32 function ImageList_Copy, as
	//     described in the Windows SDK.
	// Parameters:
	//     iDst - The zero-based index of the image to be used as the destination of the copy
	//            operation.
	//     pSrc - A pointer to a CXTPImageList object that is the target of the copy
	//            operation.
	//     iSrc - The zero-based index of the image to be used as the source of the copy
	//            operation.
	//     uFlags - The bit flag value that specifies the type of copy operation to be
	//              made.
	// Returns:
	//     Nonzero if successful; otherwise zero.
	//-----------------------------------------------------------------------
	BOOL Copy(int iDst, CXTPImageList* pSrc, int iSrc, UINT uFlags = ILCF_MOVE);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to draw an image from an image list.
	// Parameters:
	//     pimldp - A pointer to an IMAGELISTDRAWPARAMS structure that contains information about
	//     the draw operation.
	// Returns:
	//     TRUE if the image is successfully drawn; otherwise FALSE.
	//-----------------------------------------------------------------------
	BOOL DrawIndirect(IMAGELISTDRAWPARAMS* pimldp);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this function to read an image list from an archive.
	// Parameters:
	//     pArchive - A pointer to a CArchive object from which the image list is to be read.
	// Returns:
	//     Nonzero if successful; otherwise 0.
	//-----------------------------------------------------------------------
	BOOL Read(CArchive* pArchive);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this function to write an image list object to an archive.
	// Parameters:
	//     pArchive - A pointer to a CArchive object in which the image list is to be stored.
	// Returns:
	//     Nonzero if successful; otherwise 0.
	//-----------------------------------------------------------------------
	BOOL Write(CArchive* pArchive);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this function to begin dragging an image.
	// Parameters:
	//     nImage - Zero-based index of the image to drag.
	//     ptHotSpot - Coordinates of the starting drag position (typically, the cursor position).
	//     The coordinates are relative to the upper left corner of the image.
	// Returns:
	//     Nonzero if successful; otherwise 0.
	//-----------------------------------------------------------------------
	BOOL BeginDrag(int nImage, CPoint ptHotSpot);

	//-----------------------------------------------------------------------
	// Summary:
	//     Creates a new drag image by combining the given image (typically a mouse cursor image)
	//     with the current drag image.
	// Parameters:
	//     nDrag - Index of the new image to be combined with the drag image.
	//     ptHotSpot - Position of the hot spot within the new image.
	// Returns:
	//     Nonzero if successful; otherwise 0.
	//-----------------------------------------------------------------------
	BOOL SetDragCursorImage(int nDrag, CPoint ptHotSpot);

	//{{AFX_CODEJOCK_PRIVATE
#	if defined(_DEBUG) || defined(_AFXDLL)
	virtual void Dump(CDumpContext& dc) const;
	virtual void AssertValid() const;
#	endif
	//}}AFX_CODEJOCK_PRIVATE

	//-----------------------------------------------------------------------
	// Summary:
	//     Obtains a pointer to the encapsulated CImageList object.
	// Returns:
	//     A pointer to the encapsulated CImageList object.
	//-----------------------------------------------------------------------
	CImageList* GetNativeGdiObject();
	const CImageList* GetNativeGdiObject() const; // <combine CXTPImageList::GetNativeGdiObject>

	//-----------------------------------------------------------------------
	// Summary:
	//     Converts an object to CImageList reference making it possible
	//     to use CXTPImageList with MFC classes and methods that use CImageList
	//     reference as an argument.
	// Returns:
	//     An encapsulated CImageList object reference.
	//-----------------------------------------------------------------------
	operator CImageList&();

	//-----------------------------------------------------------------------
	// Summary:
	//     Converts an object to CImageList constant reference making it possible
	//     to use CXTPImageList with MFC classes and methods that use CImageList or
	//     CGdiObject constant reference as an argument.
	// Returns:
	//     An encapsulated CImageList constant object reference.
	//-----------------------------------------------------------------------
	operator const CImageList&() const;

	//{{AFX_CODEJOCK_PRIVATE
	XTPImageListPtrProxy operator&();
	const XTPImageListPtrProxy operator&() const;
	//}}AFX_CODEJOCK_PRIVATE
};

AFX_INLINE CImageList* CXTPImageList::GetNativeGdiObject()
{
	return m_imageList;
}

AFX_INLINE const CImageList* CXTPImageList::GetNativeGdiObject() const
{
	return const_cast<CXTPImageList*>(this)->GetNativeGdiObject();
}

AFX_INLINE CXTPImageList::operator CImageList&()
{
	return *GetNativeGdiObject();
}

AFX_INLINE CXTPImageList::operator const CImageList&() const
{
	return *GetNativeGdiObject();
}

AFX_INLINE CXTPImageList::XTPImageListPtrProxy CXTPImageList::operator&()
{
	return XTPImageListPtrProxy(this);
}

AFX_INLINE const CXTPImageList::XTPImageListPtrProxy CXTPImageList::operator&() const
{
	return XTPImageListPtrProxy(const_cast<CXTPImageList*>(this));
}

#	include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPIMAGELIST_H__)
