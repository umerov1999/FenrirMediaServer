// XTPSkinImage.h: interface for the CXTPSkinImage class.
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
#if !defined(__XTPSKINIMAGE_H__)
#	define __XTPSKINIMAGE_H__
//}}AFX_CODEJOCK_PRIVATE

#	if _MSC_VER > 1000
#		pragma once
#	endif // _MSC_VER > 1000

#	include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

class CXTPSkinManagerResourceFile;

//===========================================================================
// Summary:
//     The CXTPSkinImage class represents a simple bitmap holder and
//     draw operations for Skin Framework.
//===========================================================================
class _XTP_EXT_CLASS CXTPSkinImage : public CXTPSynchronized
{
private:
	struct SOLIDRECT
	{
		RECT rc;
		COLORREF clr;
	};

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPSkinImage object.
	//-----------------------------------------------------------------------
	CXTPSkinImage();

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPSkinImage object, handles cleanup and deallocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPSkinImage();

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Loads a specified bitmap from a .dll using its name identifier.
	// Parameters:
	//     hModule            - Module handle.
	//     lpszBitmapFileName - Bitmap file name.
	//     lpszFileName       - File name on disk to load.
	// Returns:
	//     TRUE if successful, otherwise FALSE.
	//-----------------------------------------------------------------------
	BOOL LoadFile(LPCTSTR lpszFileName);
	BOOL LoadFile(HMODULE hModule,
				  LPCTSTR lpszBitmapFileName); //<COMBINE CXTPSkinImage::LoadFile@LPCTSTR>
	BOOL LoadMemory(const void* Buf, int Size);

	//-----------------------------------------------------------------------
	// Summary:
	//     Assigns a bitmap handle to the CXTPSkinImage class.
	// Parameters:
	//     hBitmap - Bitmap handle.
	//     bAlpha  - TRUE if the bitmap contains alpha pixels.
	//-----------------------------------------------------------------------
	void SetBitmap(HBITMAP hBitmap, BOOL bAlpha = FALSE);

	//-----------------------------------------------------------------------
	// Summary:
	//     Draws the image in a specified location.
	// Parameters:
	//     pDC             - Destination device context.
	//     rcDest          - Reference to the location of the destination rectangle.
	//     rcSrc           - Reference to the location of the source rectangle.
	//     rcSizingMargins - Reference to sizing margins.
	//     nSizingType     - Stretch type.
	//     bBorderOnly     - TRUE to only draw borders.
	//     clrTransparent  - Transparent color.
	//-----------------------------------------------------------------------
	void DrawImage(CDC* pDC, const CRect& rcDest, const CRect& rcSrc, const CRect& rcSizingMargins,
				   int nSizingType, BOOL bBorderOnly);
	void DrawImage(CDC* pDC, const CRect& rcDest, const CRect& rcSrc, const CRect& rcSizingMargins,
				   COLORREF clrTransparent, int nSizingType,
				   BOOL bBorderOnly); // <combine CXTPSkinImage::DrawImage@CDC*@const CRect&@const
									  // CRect&@const CRect&@int@BOOL>

	//-----------------------------------------------------------------------
	// Summary:
	//     Gets the height of the bitmap.
	// Returns:
	//     The height of the bitmap.
	//-----------------------------------------------------------------------
	DWORD GetHeight() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Gets the width of the bitmap.
	// Returns:
	//     The width of the bitmap.
	//-----------------------------------------------------------------------
	DWORD GetWidth() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Gets the extent of the bitmap.
	// Returns:
	//     The extent of the bitmap.
	//-----------------------------------------------------------------------
	CSize GetExtent() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines if the bitmap has alpha pixels.
	// Returns:
	//     TRUE if the bitmap has alpha pixels, otherwise FALSE.
	//-----------------------------------------------------------------------
	BOOL IsAlphaImage() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Calculates the position of a specified image.
	// Parameters:
	//     nState - Index of the image.
	//     nCount - Total image count.
	// Returns:
	//     The position of the specified image.
	//-----------------------------------------------------------------------
	CRect GetSource(int nState = 0, int nCount = 1) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Helper method that splits an image into solid rectangles for faster drawing.
	// Parameters:
	//     nBitmaps         - Image identifier.
	//     bHorizontalImage - TRUE if the images are arranged horizontally.
	//     rcSizingMargins  - Reference to sizing margins.
	//-----------------------------------------------------------------------
	void CreateSolidRectArray(int nBitmaps, BOOL bHorizontalImage, const CRect& rcSizingMargins);

private:
	void DrawImageInternal(CDC* pDC, const CRect& rcDest, const CRect& rcSrc,
						   const CRect& rcSizingMargins, int nSizingType, BOOL bBorderOnly,
						   COLORREF clrTransparent = COLORREF_NULL);
	BOOL DrawImagePart(CDC* pDCDest, const CRect& rcDest, CDC* pDCSrc, const CRect& rcSrc) const;

	BOOL DrawImageTile(CDC* pDCDest, const CRect& rcDest, CDC* pDCSrc, const CRect& rcSrc,
					   BOOL bTile, COLORREF clrTransparent = COLORREF_NULL) const;

	void FilterImage(COLORREF clrTransparent);
	void InvertBitmap();
	BOOL FindSolidRect(const CRect& rcSrc, SOLIDRECT& sr) const;
	CSize _GetExtent() const;
	BOOL CheckBitmapRect(LPBYTE pBits, CRect rcCheck, CSize sz);

public:
	BOOL m_bMirrorImage; // TRUE to invert image in RTL mode.

protected:
	HBITMAP m_hBitmap; // Bitmap handle.
	BOOL m_bAlpha;	   // TRUE if the bitmap has alpha pixels.
	BOOL m_bFiltered;  // TRUE if the image is filtered.
	BOOL m_bInvert;	   // TRUE if the image is inverted for RTL mode.
	BOOL m_bOptimized; // TRUE if the image is split into solid rectangles.
	CSize m_szBitmap;  // Bitmap size of the image.
	CArray<SOLIDRECT, SOLIDRECT&> m_arrSolidRects; // Solid rectangles of the image.
};

//===========================================================================
// Summary:
//     CXTPSkinImages represents a cached collection of CXTPSkinImage classes.
//===========================================================================
class _XTP_EXT_CLASS CXTPSkinImages : public CXTPSynchronized
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPSkinImages object.
	//-----------------------------------------------------------------------
	CXTPSkinImages();

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPSkinImages object, handles cleanup and deallocation.
	//-----------------------------------------------------------------------
	~CXTPSkinImages();

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Removes all images from the collection.
	//-----------------------------------------------------------------------
	void RemoveAll();

	//-----------------------------------------------------------------------
	// Summary:
	//     Loads a specified image from a resource file.
	// Parameters:
	//     pResourceFile - Pointer to the resource file.
	//     lpszImageFile - Path to the image.
	// Returns:
	//     A pointer to the newly loaded CXTPSkinImage object if successful,
	//     otherwise NULL.
	//-----------------------------------------------------------------------
	CXTPSkinImage* LoadFile(CXTPSkinManagerResourceFile* pResourceFile, LPCTSTR lpszImageFile);

	//-----------------------------------------------------------------------
	// Summary:
	//     Gets the extent of a specified image located within a resource file.
	// Parameters:
	//     pResourceFile - Pointer to the resource file.
	//     lpszImageFile - Path to the image.
	// Returns:
	//     The extent of the image.
	//-----------------------------------------------------------------------
	CSize GetExtent(CXTPSkinManagerResourceFile* pResourceFile, LPCTSTR lpszImageFile);

protected:
	CMapStringToPtr m_mapImages; // Collection of images.
};

#	include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPSKINIMAGE_H__)
