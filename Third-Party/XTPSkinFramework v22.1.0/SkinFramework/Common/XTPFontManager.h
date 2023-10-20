// XTPFontManager.h: interface for the CXTPFontManager class.
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
#if !defined(__XTPFONTMANAGER_H__)
#	define __XTPFONTMANAGER_H__
//}}AFX_CODEJOCK_PRIVATE

#	if _MSC_VER > 1000
#		pragma once
#	endif // _MSC_VER > 1000

#	include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

//===========================================================================
// Summary:
//     Singleton class to manage fonts used by library windows and
//     controls.  You can access members of this class by using the
//     function XTPFontManager().
//===========================================================================
class _XTP_EXT_CLASS CXTPFontManager
{
	//-----------------------------------------------------------------------
	// Summary:
	//     Private singleton constructor.
	//-----------------------------------------------------------------------
	CXTPFontManager();

	//-----------------------------------------------------------------------
	// Summary:
	//     Used to access the CXTPFontManager object.
	//-----------------------------------------------------------------------
	friend _XTP_EXT_CLASS CXTPFontManager* AFX_CDECL XTPFontManager();

	//-----------------------------------------------------------------------
	// Summary:
	//     Base class singleton template.
	//-----------------------------------------------------------------------
	friend class CXTPSingleton<CXTPFontManager>;

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to determine if the specified
	//     font exists.
	// Parameters:
	//     lpszFaceName - A NULL-terminated string that represents the
	//                    font face name.
	// Returns:
	//     TRUE if the font was found, otherwise FALSE.
	//-----------------------------------------------------------------------
	BOOL FontExists(LPCTSTR lpszFaceName);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to create system fonts using the specified
	//     LOGFONT structure.
	// Parameters:
	//     lf - Font structure used to initialize all system fonts.
	//-----------------------------------------------------------------------
	void CreateFonts(LOGFONT lf);

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves a reference to the standard library font managed
	//     by CXTPFontManager.
	// Returns:
	//     A reference to a CFont object representing the library
	//     standard font.
	//-----------------------------------------------------------------------
	CFont& GetFont();

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves a reference to the bold library font managed
	//     by CXTPFontManager.
	// Returns:
	//     A reference to a CFont object representing the library
	//     bold font.
	//-----------------------------------------------------------------------
	CFont& GetFontBold();

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves a reference to the italic library font managed
	//     by CXTPFontManager.
	// Returns:
	//     A reference to a CFont object representing the library
	//     italic font.
	//-----------------------------------------------------------------------
	CFont& GetFontItalic();

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves a reference to the italic standard library font
	//     managed by CXTPFontManager.
	// Returns:
	//     A reference to a CFont object representing the library
	//     italic standard font.
	//-----------------------------------------------------------------------
	CFont& GetFontItalicBold();

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves a reference to the vertical library font managed
	//     by CXTPFontManager.
	// Returns:
	//     A reference to a CFont object representing the library
	//     vertical font.
	//-----------------------------------------------------------------------
	CFont& GetFontVertical();

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves a reference to the vertical bold library font
	//     managed by CXTPFontManager.
	// Returns:
	//     A reference to a CFont object representing the library
	//     vertical bold font.
	//-----------------------------------------------------------------------
	CFont& GetFontVerticalBold();

protected:
	CXTPFont m_xtpFont;				// Default font.
	CXTPFont m_xtpFontBold;			// Default bold font.
	CXTPFont m_xtpFontItalic;		// Default italic font.
	CXTPFont m_xtpFontItalicBold;   // Default italic bold font.
	CXTPFont m_xtpFontVertical;		// Default vertical font.
	CXTPFont m_xtpFontVerticalBold; // Default vertical bold font.

	XTP_SUBSTITUTE_GDI_MEMBER_WITH_CACHED(CFont, m_font, m_xtpFont, GetFontHandle);
	XTP_SUBSTITUTE_GDI_MEMBER_WITH_CACHED(CFont, m_fontBold, m_xtpFontBold, GetBoldFontHandle);
	XTP_SUBSTITUTE_GDI_MEMBER_WITH_CACHED(CFont, m_fontItalic, m_xtpFontItalic,
										  GetItalicFontHandle);
	XTP_SUBSTITUTE_GDI_MEMBER_WITH_CACHED(CFont, m_fontItalicBold, m_xtpFontItalicBold,
										  GetItalicBoldFontHandle);
	XTP_SUBSTITUTE_GDI_MEMBER_WITH_CACHED(CFont, m_fontVertical, m_xtpFontVertical,
										  GetVerticalFontHandle);
	XTP_SUBSTITUTE_GDI_MEMBER_WITH_CACHED(CFont, m_fontVerticalBold, m_xtpFontVerticalBold,
										  GetVerticalBoldFontHandle);
};

AFX_INLINE CFont& CXTPFontManager::GetFont()
{
	return m_xtpFont;
}
AFX_INLINE CFont& CXTPFontManager::GetFontBold()
{
	return m_xtpFontBold;
}
AFX_INLINE CFont& CXTPFontManager::GetFontItalic()
{
	return m_xtpFontItalic;
}
AFX_INLINE CFont& CXTPFontManager::GetFontItalicBold()
{
	return m_xtpFontItalicBold;
}
AFX_INLINE CFont& CXTPFontManager::GetFontVertical()
{
	return m_xtpFontVertical;
}
AFX_INLINE CFont& CXTPFontManager::GetFontVerticalBold()
{
	return m_xtpFontVerticalBold;
}

//---------------------------------------------------------------------------
// Summary:
//     Call this function to access CXTPFontManager members. Since this
//     class is designed as a single instance object you can only access
//     its members through this method. You <b>cannot</b> directly
//     instantiate an object of type CXTPFontManager.
// Example:
//     <code>SetFont(&CXTPFontManager()->GetNormalFont());</code>
//---------------------------------------------------------------------------
_XTP_EXT_CLASS CXTPFontManager* AFX_CDECL XTPFontManager();

#	include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPFONTMANAGER_H__)
