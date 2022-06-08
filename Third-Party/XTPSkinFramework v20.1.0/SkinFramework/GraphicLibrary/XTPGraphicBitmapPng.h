// XTPGraphicBitmapPng.h interface for the XTPGraphicBitmapPng class.
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
#if !defined(__XTPGRAPHICBITMAPPNG_H__)
#	define __XTPGRAPHICBITMAPPNG_H__
//}}AFX_CODEJOCK_PRIVATE

#	if _MSC_VER > 1000
#		pragma once
#	endif // _MSC_VER > 1000

#	include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

//===========================================================================
// Summary:
//     CXTPGraphicBitmapPng is a CBitmap derived class used to load .PNG
//     files from files and resources.
//===========================================================================
class _XTP_EXT_CLASS CXTPGraphicBitmapPng : public CBitmap
{
	struct CCallback;

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Default constructor.
	//-----------------------------------------------------------------------
	CXTPGraphicBitmapPng();

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to load a .PNG file.
	// Parameters:
	//     lpszFileName - A string value that contains the name of a file
	//                    to be loaded.
	// Returns:
	//     TRUE if successful, otherwise 0.
	//-----------------------------------------------------------------------
	BOOL LoadFromFile(LPCTSTR lpszFileName);

	BOOL LoadFromMemory(const void* Buf, int Size);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to load a .PNG file from a specified
	//     resource.
	// Parameters:
	//     hModule - Module instance to load.
	//     hRes    - Resource handle.
	// Returns:
	//     TRUE if successful, otherwise 0.
	//-----------------------------------------------------------------------
	BOOL LoadFromResource(HMODULE hModule, HRSRC hRes);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to load a .PNG file.
	// Parameters:
	//     pFile - Pointer to the CFile to be loaded.
	// Returns:
	//     TRUE if successful, otherwise 0.
	//-----------------------------------------------------------------------
	BOOL LoadFromFile(CFile* pFile);

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines if the loaded .PNG file has alpha layer.
	// Returns:
	//     TRUE if the .PNG file has alpha layer.
	//-----------------------------------------------------------------------
	BOOL IsAlpha() const;

private:
	HBITMAP ConvertToBitmap(BYTE* pbImage, CSize szImage, int cImgChannels) const;

protected:
	BOOL m_bAlpha; // TRUE if the .PNG file has alpha layer.
};

AFX_INLINE BOOL CXTPGraphicBitmapPng::IsAlpha() const
{
	return m_bAlpha;
}

//---------------------------------------------------------------------------
// Summary:
//     Possible ZLib compression levels.
// See Also:
//     ZLibCompressWithLevel
//---------------------------------------------------------------------------
enum XTPZlibCompressionLevel
{
	xtpZLibNoCompression	  = 0,
	xtpZLibBestSpeed		  = 1,
	xtpZLibBestCompression	  = 9,
	xtpZLibDefaultCompression = -1
};

//===========================================================================
// Summary:
//     Compresses a source buffer into a target buffer using the ZLib library.
// Parameters:
//     dest      - Pointer to the target buffer.
//     destLen   - Size of the target buffer; must be at least 0.1% larger
//                 than source_size plus 12 bytes.
//     source    - Pointer to the source buffer.
//     sourceLen - Size of the source buffer.
//     level     - Optional compression level; one of the XTPZlibCompressionLevel
//                 constants.
// Returns:
//     The actual size of the compressed buffer, or 0 if an error occurred.
// See Also:
//     XTPZlibCompressionLevel
//===========================================================================
_XTP_EXT_CLASS int AFX_CDECL ZLibCompress(BYTE* dest, ULONG* destLen, const BYTE* source,
										  ULONG sourceLen,
										  XTPZlibCompressionLevel level = xtpZLibDefaultCompression);

//===========================================================================
// Summary:
//     Used before a ZLibCompress call to allocate the destination buffer.
// Parameters:
//     sourceLen - Size of the source buffer.
// Returns:
//     An upper bound on the compressed size after ZLibCompress on sourceLen bytes.
//===========================================================================
_XTP_EXT_CLASS ULONG AFX_CDECL ZLibCompressBound(ULONG sourceLen);

//===========================================================================
// Summary:
//     Decompresses a source buffer into a target buffer, using the ZLib library.
// Parameters:
//     dest      - Pointer to the target buffer.
//     destLen   - Size of the target buffer; must be at least 0.1% larger
//                 than source_size plus 12 bytes.
//     source    - Pointer to the source buffer.
//     sourceLen - Size of the source buffer.
// Returns:
//     The actual size of the uncompressed buffer, or 0 if an error occurred.
//===========================================================================
_XTP_EXT_CLASS int AFX_CDECL ZLibUncompress(BYTE* dest, ULONG* destLen, const BYTE* source,
											ULONG sourceLen);

#	include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // #if !defined(__XTPGRAPHICBITMAPPNG_H__)
