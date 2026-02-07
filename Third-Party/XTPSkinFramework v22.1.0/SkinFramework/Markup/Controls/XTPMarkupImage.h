// XTPMarkupImage.h: interface for the CXTPMarkupImage class.
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
#if !defined(__XTPMARKUPIMAGE_H__)
#	define __XTPMARKUPIMAGE_H__
//}}AFX_CODEJOCK_PRIVATE

#	if _MSC_VER > 1000
#		pragma once
#	endif // _MSC_VER > 1000

#	include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

class CXTPMarkupDeviceDependentImage;
class CXTPMarkupDependencyProperty;
class CXTPMarkupObject;
class CXTPMarkupDrawingContext;
class CXTPMarkupContext;

//===========================================================================
// Summary:
//     CXTPMarkupImage is a CXTPMarkupFrameworkElement derived class.
//     It implements the Image XAML Tag.
//===========================================================================
class _XTP_EXT_CLASS CXTPMarkupImage
	: public CXTPMarkupFrameworkElement
	, private IXTPMarkupDeviceDependentImageSite
{
	DECLARE_MARKUPCLASS(CXTPMarkupImage)

protected:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPMarkupImage object.
	//-----------------------------------------------------------------------
	CXTPMarkupImage();

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPMarkupImage object, handles cleanup and deallocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPMarkupImage();

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Sets the source of the image to load.
	// Parameters:
	//     lpszSource - String contained URI of the image to load.
	// Example:
	// <code>
	//     pImage->SetSource("file://c:\\image.bmp); // Load from file
	//     pImage->SetSource("res://#125"); // Load from resources
	//     pImage->SetSource("125");  // Use Image manager index
	// </code>
	//-----------------------------------------------------------------------
	void SetSource(LPCTSTR lpszSource);

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves the source of the image to load.
	// Returns:
	//     The string contained URI of the image to load.
	//-----------------------------------------------------------------------
	CString GetSource();

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets the stretch type of the image.
	// Parameters:
	//     stretch - New stretch type.
	//-----------------------------------------------------------------------
	void SetStretch(XTPMarkupStretch stretch);

	//-----------------------------------------------------------------------
	// Summary:
	//     Gets the current stretch type of the image.
	// Returns:
	//     The current stretch type.
	//-----------------------------------------------------------------------
	XTPMarkupStretch GetStretch() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves the original size of the image (not scaled).
	// Returns:
	//     A CSize object containing the original size of the image.
	//-----------------------------------------------------------------------
	CSize GetOriginalSize() const;

	//{{AFX_CODEJOCK_PRIVATE
	// Implementation
public:
	virtual CSize MeasureOverride(CXTPMarkupDrawingContext* pDC, CSize constraint);
	virtual CSize ArrangeOverride(CSize arrangeSize);
	virtual void OnPropertyChanged(CXTPMarkupDependencyProperty* pProperty,
								   CXTPMarkupObject* pOldValue, CXTPMarkupObject* pNewValue);
	virtual void OnRender(CXTPMarkupDrawingContext* pDC);

protected:
	void CreateImageInstance(CXTPMarkupDrawingContext* pDC);
	CSize MeasureArrangeHelper(CSize inputSize);

protected:
	// IXTPMarkupDeviceDependentImageSite overrides

	virtual CXTPMarkupContext* GetMarkupContext();
	virtual void OnImageUpdateRequired();

private:
	CXTPMarkupDeviceDependentImage* m_pDeviceImage;
	CSize m_ConstraintSize;

public:
	static CXTPMarkupDependencyProperty* m_pSourceProperty;
	static CXTPMarkupDependencyProperty* m_pStretchProperty;
	//}}AFX_CODEJOCK_PRIVATE

	//{{AFX_CODEJOCK_PRIVATE
public:
	DECLARE_DISPATCH_MAP()
#	ifdef _XTP_ACTIVEX
	DECLARE_INTERFACE_MAP()
	DECLARE_OLETYPELIB_EX(CXTPMarkupImage);
#	endif

	afx_msg void OleSetSource(LPCTSTR lpszSource);
	afx_msg BSTR OleGetSource();
	afx_msg long OleGetStretch();
	afx_msg void OleSetStretch(long stretch);
	//}}AFX_CODEJOCK_PRIVATE
};

AFX_INLINE void CXTPMarkupImage::SetStretch(XTPMarkupStretch stretch)
{
	SetValue(m_pStretchProperty, CXTPMarkupEnum::CreateValue(stretch));
}

#	include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPMARKUPIMAGE_H__)
