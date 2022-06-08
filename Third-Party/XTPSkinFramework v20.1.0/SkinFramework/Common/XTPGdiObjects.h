// XTPGdiObjects.h
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
#if !defined(__XTPGDIOBJECTCACHE_H__)
#	define __XTPGDIOBJECTCACHE_H__
//}}AFX_CODEJOCK_PRIVATE

#	if _MSC_VER > 1000
#		pragma once
#	endif // _MSC_VER > 1000

#	include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

//{{AFX_CODEJOCK_PRIVATE

//// Enable this in Debug mode and re-build ToolkitPro only for tracking unauthorized cached object
//// changes.
//#define XTP_ENABLE_UNAUTHORIZED_CACHED_GDI_OBJECT_CHANGE_FAULT

#	ifndef XTP_DISABLE_DEPRECATE_GDI_MEMBER
#		define XTP_DEPRECATE_GDI_MEMBER_IN_FAVOR_OF(memberName, inFavorOf)                        \
			_XTP_DEPRECATE("The member '" #memberName                                              \
						   "' has been deprecated, using it may lead to undefined behavior. "      \
						   "Use '" #inFavorOf                                                      \
						   "' instead. You can disable this message by defining "                  \
						   "XTP_DISABLE_DEPRECATE_GDI_MEMBER globally.")
#	else
#		define XTP_DEPRECATE_GDI_MEMBER_IN_FAVOR_OF(what)
#	endif

#	define XTP_SUBSTITUTE_GDI_MEMBER_WITH_CACHED_(oldClass, oldClassRef, oldClassCRef,            \
												   oldMemberName, cachedMemberName,                \
												   handleGetterName)                               \
		XTP_DEPRECATE_GDI_MEMBER_IN_FAVOR_OF(oldMemberName, cachedMemberName)                      \
		AFX_INLINE oldClassRef handleGetterName()                                                  \
		{                                                                                          \
			return *cachedMemberName.GetNativeGdiObject();                                         \
		}                                                                                          \
		XTP_DEPRECATE_GDI_MEMBER_IN_FAVOR_OF(oldMemberName, cachedMemberName)                      \
		AFX_INLINE oldClassCRef handleGetterName() const                                           \
		{                                                                                          \
			return *cachedMemberName.GetNativeGdiObject();                                         \
		}                                                                                          \
		__declspec(property(get = handleGetterName)) oldClassRef oldMemberName;

#	define XTP_SUBSTITUTE_GDI_MEMBER_WITH_CACHED(oldClass, oldMemberName, cachedMemberName,       \
												  handleGetterName)                                \
		XTP_SUBSTITUTE_GDI_MEMBER_WITH_CACHED_(oldClass, oldClass&, const oldClass&,               \
											   oldMemberName, cachedMemberName, handleGetterName)

template<class GdiObject, class XtpObject>
class CXTPGdiObjectPtrProxy;
class CXTPCachedGdiObjectReferenceBase;
template<class Descriptor, class GdiObject, class XtpCacheClass>
class CXTPCachedGdiObjectReference;
class CXTPFont;
class CXTPGdiObject;
class CXTPGdiObjectCacheBase;

template<class GdiObject, class XtpObject>
class CXTPGdiObjectPtrProxy
{
public:
	operator XtpObject*();
	operator const XtpObject*() const;
	operator GdiObject*();
	operator const GdiObject*() const;
	XtpObject& operator*();
	const XtpObject& operator*() const;

private:
	friend class CXTPFont;
	friend class CXTPBrush;
	friend class CXTPPen;
	friend class CXTPImageList;
	explicit CXTPGdiObjectPtrProxy(XtpObject* object);

	XtpObject* m_pObject;
};

template<class GdiObject, class XtpObject>
AFX_INLINE CXTPGdiObjectPtrProxy<GdiObject, XtpObject>::CXTPGdiObjectPtrProxy(XtpObject* pObject)
	: m_pObject(pObject)
{
}

template<class GdiObject, class XtpObject>
AFX_INLINE CXTPGdiObjectPtrProxy<GdiObject, XtpObject>::operator XtpObject*()
{
	return m_pObject;
}

template<class GdiObject, class XtpObject>
AFX_INLINE CXTPGdiObjectPtrProxy<GdiObject, XtpObject>::operator const XtpObject*() const
{
	return m_pObject;
}

template<class GdiObject, class XtpObject>
AFX_INLINE CXTPGdiObjectPtrProxy<GdiObject, XtpObject>::operator GdiObject*()
{
	return m_pObject->GetNativeGdiObject();
}

template<class GdiObject, class XtpObject>
AFX_INLINE CXTPGdiObjectPtrProxy<GdiObject, XtpObject>::operator const GdiObject*() const
{
	return m_pObject->GetNativeGdiObject();
}

template<class GdiObject, class XtpObject>
AFX_INLINE XtpObject& CXTPGdiObjectPtrProxy<GdiObject, XtpObject>::operator*()
{
	return *m_pObject;
}

template<class GdiObject, class XtpObject>
AFX_INLINE const XtpObject& CXTPGdiObjectPtrProxy<GdiObject, XtpObject>::operator*() const
{
	return *m_pObject;
}

struct XTPGDIDESCRIPTOR;

//}}AFX_CODEJOCK_PRIVATE

//=======================================================================
// Summary:
//    Implements common functionality for all XTP GDI object. Mimics
//    CGdiObject's interface from MFC however cannot be cast to
//    CGdiObject type.
// Remarks:
//    XTP GDI objects use cached GDI handles so one native GDI handle can be
//    shared across several XTP GDI object instances. The lifetime of a handle
//    is controlled by XTP GDI reference counter, meaning a handle is deleted
//    only when the last XTP GDI object that references a particular handle is
//    destroyed or DeleteObject is called.
//    Note, because an encapsulated GDI handle is cached and shared, passing
//    m_hObject to DeleteObject from Windows SDK or convering an XTP GDI
//    object to CGdiObject& and making a call to CGdiObject::DeleteObject
//    will delete a cached handle and leave all XTP GDI object that reference
//    the delete handle in an undetermined state. This will also affect every
//    place in a program where the cached handles are used. So use the encapsulated
//    GDI handle or CGdiObject very carefully and for read-only purposes.
// See also:
//    CXTPFont, CXTPBrush, CXTPPen
//=======================================================================
class _XTP_EXT_CLASS CXTPGdiObject : public CObject
{
	DECLARE_DYNAMIC(CXTPGdiObject);

protected:
	//-----------------------------------------------------------------------
	// Summary:
	//      Performs internal CXTPGdiObject construction logic.
	//-----------------------------------------------------------------------
	CXTPGdiObject();

public:
	//-----------------------------------------------------------------------
	// Summary:
	//      Handles CXTPGdiObject destruction. Makes a call to DeleteObject
	//      if it has not been done prior to object destruction.
	//-----------------------------------------------------------------------
	virtual ~CXTPGdiObject() = 0;

	//-----------------------------------------------------------------------
	// Summary:
	//      Casts an object to an encapsulated GDI handle.
	// Remarks:
	//      The returned handle must not be passed to DeleteObject
	//      and should be used for read-only purposes, i.e. GDI object
	//      state should not be modified as it will affect every place
	//      in a program where the cached hande is used.
	// Returns:
	//      Encapsulated GDI handle or NULL.
	//-----------------------------------------------------------------------
	operator HGDIOBJ() const;

	//-----------------------------------------------------------------------
	// Summary:
	//      Returns m_hObject unless this is NULL, in which case NULL is returned.
	// Remarks:
	//      The returned handle must not be passed to DeleteObject
	//      and should be used for read-only purposes, i.e. GDI object
	//      state should not be modified as it will affect every place
	//      in a program where the cached handle is used.
	// Returns:
	//      Encapsulated GDI handle or NULL.
	//-----------------------------------------------------------------------
	HGDIOBJ GetSafeHandle() const;

	//-----------------------------------------------------------------------
	// Summary:
	//      Decrements reference counter for a cached GDI object or delete
	//      the cached GDI object once the reference counter is equal to zero.
	// Returns:
	//      TRUE if the reference counter is decremented without deleting
	//      a cached GDI object or the GDI object is deleted successfully.
	//-----------------------------------------------------------------------
	BOOL DeleteObject();

#	pragma push_macro("GetObject")
#	undef GetObject
	//-----------------------------------------------------------------------
	// Summary:
	//      Fills a buffer with data that defines a specified object.
	// Parameters:
	//      nCount - Specifies the number of bytes to copy into the lpObject buffer.
	//      lpObject - Points to a user - supplied buffer that is to receive the information.
	// Returns:
	//      The number of bytes retrieved; otherwise 0 if an error occurs.
	//-----------------------------------------------------------------------
	int GetObject(int nCount, LPVOID lpObject) const;
#	pragma pop_macro("GetObject")

	//-----------------------------------------------------------------------
	// Summary:
	//      Retrieves the type of the GDI object.
	// Returns:
	//      The type of the object, if successful; otherwise 0. Refer to
	//      the Platform SDK documentation for the list of possible values
	//      returned.
	//-----------------------------------------------------------------------
	UINT GetObjectType() const;

	//-----------------------------------------------------------------------
	// Summary:
	//      Retrieves a handle to one of the predefined stock Windows GDI pens,
	//      brushes, or fonts, and attaches the GDI object to the CGdiObject object.
	// Parameters:
	//      nIndex - A constant specifying the type of stock object desired. Refer to
	//               the Platform SDK documentation for the list of possible values.
	// Returns:
	//      Nonzero if the function is successful; otherwise 0.
	// Remarks:
	//      Stock objects don't get cached and its handle becomes associated
	//      with one and only one instance of CXTPGdiObject or its derived class.
	//-----------------------------------------------------------------------
	BOOL CreateStockObject(int nIndex);

	//-----------------------------------------------------------------------
	// Summary:
	//      Determines if two GDI objects are logically equal.
	// Parameters:
	//      obj - A reference to the other CXTPGdiObject instance to compare to.
	// Returns:
	//      TRUE if the encapsulated GDI objects are logically equal (handles
	//      are the same) or FALSE otherwise.
	//-----------------------------------------------------------------------
	BOOL operator==(const CXTPGdiObject& obj) const;

	//-----------------------------------------------------------------------
	// Summary:
	//      Determines if two GDI objects are logically not equal.
	// Parameters:
	//      obj - A reference to the other CXTPGdiObject instance to compare to.
	// Returns:
	//      TRUE if the encapsulated GDI objects are logically not equal (handles
	//      are not the same) or FALSE otherwise.
	//-----------------------------------------------------------------------
	BOOL operator!=(const CXTPGdiObject& obj) const;

	//-----------------------------------------------------------------------
	// Summary:
	//      Converts an object to CGdiObject reference making it possible
	//      to use CXTPGdiObject with MFC classes and methods that use CGdiObject
	//      reference as an argument.
	// Returns:
	//      An encapsulated CGdiObject reference.
	// Remarks:
	//      The returned object must not be deleted or modified as it will affect
	//      every place in a program where the cached handle is used.
	//-----------------------------------------------------------------------
	operator CGdiObject&();

	//-----------------------------------------------------------------------
	// Summary:
	//      Converts an object to CGdiObject constance reference making it possible
	//      to use CXTPGdiObject with MFC classes and methods that use CGdiObject
	//      reference as an argument.
	// Returns:
	//      An encapsulated CGdiObject constant reference.
	//-----------------------------------------------------------------------
	operator const CGdiObject&() const;

	//-----------------------------------------------------------------------
	// Summary:
	//      A read-only encapsulated GDI handle value.
	// Remarks:
	//      The returned handle must not be passed to DeleteObject
	//      and should be used for read-only purposes, i.e. GDI object
	//      state should not be modified as it will affect every place
	//      in a program where the cached handle is used.
	// Returns:
	//      Encapsulated GDI handle or NULL.
	//-----------------------------------------------------------------------
	__declspec(property(get = GetSafeHandle)) HGDIOBJ m_hObject;

	//-----------------------------------------------------------------------
	// Summary:
	//      Sets and determines handle ownership guaranteed status. Guaranteed
	//      ownership implies full control over the cached encapsulated handle and thus
	//      guarantees that the handle is properly detached from a device context
	//      before being deleted. Non-guaranteed ownership can be set in scenarios
	//      when a handle is being passed to any API thay may or may not detach it
	//      from a device context. By default guaranteed ownership is implied.
	//      This status is used only as a diagnostic hint and does not provide any
	//      functional advantage. Changing this status affects all GDI object that
	//      use the same cached handle.
	// Returns:
	//      TRUE if handle ownership is guaranteed.
	//-----------------------------------------------------------------------
	void SetHandleOwnershipNotGuaranteed();
	BOOL IsHandleOwnershipGuaranteed() const; // <combine
											  // CXTPGdiObject::SetHandleOwnershipNotGuaranteed>

protected:
	//{{AFX_CODEJOCK_PRIVATE
	CGdiObject* GetGdiObject();
	const CGdiObject* GetGdiObject() const;

	void SetDummyGdiObject(CGdiObject& gdiObject);
	void SetCachedObject(CXTPCachedGdiObjectReferenceBase* pCachedObject);

	virtual XTPGDIDESCRIPTOR& AccessDescriptorData() = 0;
	const XTPGDIDESCRIPTOR& AccessDescriptorData() const;

	void* GetDescriptor() const;
	//}}AFX_CODEJOCK_PRIVATE

private:
	CXTPCachedGdiObjectReferenceBase* m_pCachedObject;
	CGdiObject* m_pDummyGdiObject;
	BOOL m_bHandleOwnershipViolated;
#	ifdef _DEBUG
	HGDIOBJ m_hDummyGdiObject;

	void AssertDummyUnaltered() const;
#	endif
};

AFX_INLINE const CGdiObject* CXTPGdiObject::GetGdiObject() const
{
	return const_cast<CXTPGdiObject*>(this)->GetGdiObject();
}

AFX_INLINE CXTPGdiObject::operator CGdiObject&()
{
	return *GetGdiObject();
}

AFX_INLINE CXTPGdiObject::operator const CGdiObject&() const
{
	return *GetGdiObject();
}

AFX_INLINE const XTPGDIDESCRIPTOR& CXTPGdiObject::AccessDescriptorData() const
{
	return const_cast<CXTPGdiObject*>(this)->AccessDescriptorData();
}

//=======================================================================
// Summary:
//    Provides a convenient object oriented interface for HFONT GDI object
//    and uses GDI handle caching. Mimics CFont's interface from MFC however
//    cannot be cast to CFont or CGdiObject types.
// Remarks:
//    XTP GDI objects use cached GDI handles so one native GDI handle can be
//    shared across several XTP GDI object instances. The lifetime of a handle
//    is controlled by XTP GDI reference counter, meaning a handle is deleted
//    only when the last XTP GDI object that references a particular handle is
//    destroyed or DeleteObject is called.
//    Note, because an encapsulated GDI handle is cached and shared, passing
//    m_hObject to DeleteObject from Windows SDK or convering an XTP GDI
//    object to CGdiObject& and making a call to CGdiObject::DeleteObject
//    will delete a cached handle and leave all XTP GDI object that reference
//    the delete handle in an undetermined state. This will also affect every
//    place in a program where the cached handles are used. So use the encapsulated
//    GDI handle or CGdiObject very carefully and for read-only purposes.
// See also:
//    CXTPGdiObject, CXTPBrush, CXTPPen
//=======================================================================
class _XTP_EXT_CLASS CXTPFont : public CXTPGdiObject
{
	DECLARE_DYNAMIC(CXTPFont);

public:
	//{{AFX_CODEJOCK_PRIVATE
	typedef CXTPGdiObjectPtrProxy<CFont, CXTPFont> XTPFontPtrProxy;
	//}}AFX_CODEJOCK_PRIVATE

	//-----------------------------------------------------------------------
	// Summary:
	//      Constructs empty CXTPFont object.
	//-----------------------------------------------------------------------
	CXTPFont();

	//-----------------------------------------------------------------------
	// Summary:
	//      Handles CXTPFont object destruction.
	//-----------------------------------------------------------------------
	~CXTPFont();

	//-----------------------------------------------------------------------
	// Summary:
	//      Initializes a CFont object with the characteristics given in a
	//      LOGFONTstructure.
	// Parameters:
	//      lpLogFont - Points to a LOGFONT structure that defines the
	//                  characteristics of the logical font.
	// Returns:
	//      Nonzero if successful; otherwise 0.
	// Remarks:
	//      If the requested font has already been created from another
	//      CXTPFont instance and not yet deleted the same font will be re-used
	//      and its reference counter will be incremented.
	//-----------------------------------------------------------------------
	BOOL CreateFontIndirect(const LOGFONT* lpLogFont);

	//-----------------------------------------------------------------------
	// Summary:
	//      Initializes a CFont object with the specified characteristics.
	// Parameters:
	//      nHeight - Specifies the desired height(in logical units) of the font.
	//      nWidth - Specifies the average width(in logical units) of characters in the font.
	//      nEscapement - Specifies the angle(in 0.1 - degree units) between the
	//                    escapement vector and the x - axis of the display surface.
	//      nOrientation - Specifies the angle(in 0.1 - degree units) between the
	//                     baseline of a character and the x - axis.
	//      nWeight - Specifies the font weight(in inked pixels per 1000).
	//      bItalic - Specifies whether the font is italic.
	//      bUnderline - Specifies whether the font is underlined.
	//      cStrikeOut - Specifies whether characters in the font are struck out.
	//      nCharSet - Specifies the font's character set.
	//      nOutPrecision - Specifies the desired output precision.
	//      nClipPrecision - Specifies the desired clipping precision.
	//      nQuality - Specifies the font's output quality.
	//      nPitchAndFamily - Specifies the pitch and family of the font.
	//      lpszFacename - A CString or pointer to a null - terminated string that
	//                     specifies the typeface name of the font.
	// Returns:
	//      Nonzero if successful; otherwise 0.
	// Remarks:
	//      If the requested font has already been created from another
	//      CXTPFont instance and not yet deleted the same font will be re-used
	//      and its reference counter will be incremented.
	//-----------------------------------------------------------------------
	BOOL CreateFont(int nHeight, int nWidth, int nEscapement, int nOrientation, int nWeight,
					BYTE bItalic, BYTE bUnderline, BYTE cStrikeOut, BYTE nCharSet,
					BYTE nOutPrecision, BYTE nClipPrecision, BYTE nQuality, BYTE nPitchAndFamily,
					LPCTSTR lpszFacename);

	//-----------------------------------------------------------------------
	// Summary:
	//      This function provides a simple way to create a font of a specified
	//      typeface and point size.
	// Parameters:
	//      nPointSize - Requested font height in tenths of a point.
	//      lpszFaceName - A CString or pointer to a null - terminated string
	//                     that specifies the typeface name of the font.
	//      pDC - Pointer to the CDC object to be used to convert the height in
	//            nPointSize to logical units.
	// Returns:
	//      Nonzero if successful, otherwise 0.
	// Remarks:
	//      If the requested font has already been created from another
	//      CXTPFont instance and not yet deleted the same font will be re-used
	//      and its reference counter will be incremented.
	//-----------------------------------------------------------------------
	BOOL CreatePointFont(int nPointSize, LPCTSTR lpszFaceName, CDC* pDC = NULL);

	//-----------------------------------------------------------------------
	// Summary:
	//      This function is the same as CreateFontIndirect except that the lfHeight
	//      member of the LOGFONT is interpreted in tenths of a point rather than device units.
	// Parameters:
	//      lpLogFont - Points to a LOGFONT structure that defines the
	//                  characteristics of the logical font.
	//      pDC - Pointer to the CDC object to be used to convert the height
	//            in lfHeight to logical units.
	// Returns:
	//      Nonzero if successful, otherwise 0.
	// Remarks:
	//      If the requested font has already been created from another
	//      CXTPFont instance and not yet deleted the same font will be re-used
	//      and its reference counter will be incremented.
	//-----------------------------------------------------------------------
	BOOL CreatePointFontIndirect(const LOGFONT* lpLogFont, CDC* pDC = NULL);

	//-----------------------------------------------------------------------
	// Summary:
	//      Call this function to retrieve a copy of the LOGFONT structure that
	//      describes the encapsulated GDI object.
	// Parameters:
	//      pLogFont - Pointer to the LOGFONT structure to receive the font information.
	// Returns:
	//      Nonzero if the function succeeds, otherwise 0.
	//-----------------------------------------------------------------------
	int GetLogFont(LOGFONT* pLogFont);

	//-----------------------------------------------------------------------
	// Summary:
	//      Obtains a pointer to the encapsulated GDI object as CFont type.
	//      The returned object must not be deleted or modified as it will affect
	//      every place in a program where the cached handle is used.
	// Returns:
	//      A pointer to the encapsulated GDI object as CFont type.
	//-----------------------------------------------------------------------
	CFont* GetNativeGdiObject();
	const CFont* GetNativeGdiObject() const; // <combine CXTPFont::GetNativeGdiObject>

	//-----------------------------------------------------------------------
	// Summary:
	//      Casts an object to an encapsulated GDI handle.
	// Remarks:
	//      The returned handle must not be passed to DeleteObject
	//      and should be used for read-only purposes, i.e. GDI object
	//      state should not be modified as it will affect every place
	//      in a program where the cached hande is used.
	// Returns:
	//      Encapsulated GDI handle or NULL.
	//-----------------------------------------------------------------------
	operator HFONT() const;

	//-----------------------------------------------------------------------
	// Summary:
	//      Converts an object to CFont reference making it possible
	//      to use CXTPFont with MFC classes and methods that use CFont or
	//      CGdiObject reference as an argument.
	// Returns:
	//      An encapsulated CFont reference.
	// Remarks:
	//      The returned object must not be deleted or modified as it will affect
	//      every place in a program where the cached handle is used.
	//-----------------------------------------------------------------------
	operator CFont&();

	//-----------------------------------------------------------------------
	// Summary:
	//      Converts an object to CFont reference making it possible
	//      to use CXTPFont with MFC classes and methods that use CFont or
	//      CGdiObject constant reference as an argument.
	// Returns:
	//      An encapsulated CFont constance reference.
	//-----------------------------------------------------------------------
	operator const CFont&() const;

	//{{AFX_CODEJOCK_PRIVATE
	XTPFontPtrProxy operator&();
	const XTPFontPtrProxy operator&() const;
	//}}AFX_CODEJOCK_PRIVATE

private:
	virtual XTPGDIDESCRIPTOR& AccessDescriptorData();

	CFont m_dummyFont;
	HGDIOBJ m_hLogFontHandle;
	LOGFONT m_logFont;
};

AFX_INLINE CFont* CXTPFont::GetNativeGdiObject()
{
	return static_cast<CFont*>(GetGdiObject());
}

AFX_INLINE const CFont* CXTPFont::GetNativeGdiObject() const
{
	return const_cast<CXTPFont*>(this)->GetNativeGdiObject();
}

AFX_INLINE CXTPFont::operator HFONT() const
{
	return reinterpret_cast<HFONT>(GetSafeHandle());
}

AFX_INLINE CXTPFont::operator CFont&()
{
	return *GetNativeGdiObject();
}

AFX_INLINE CXTPFont::operator const CFont&() const
{
	return *GetNativeGdiObject();
}

AFX_INLINE CXTPFont::XTPFontPtrProxy CXTPFont::operator&()
{
	return XTPFontPtrProxy(this);
}

AFX_INLINE const CXTPFont::XTPFontPtrProxy CXTPFont::operator&() const
{
	return XTPFontPtrProxy(const_cast<CXTPFont*>(this));
}

//=======================================================================
// Summary:
//    Provides a convenient object oriented interface for HBRUSH GDI object
//    and uses GDI handle caching. Mimics CBrush's interface from MFC however
//    cannot be cast to CBrush or CGdiObject types.
// Remarks:
//    XTP GDI objects use cached GDI handles so one native GDI handle can be
//    shared across several XTP GDI object instances. The lifetime of a handle
//    is controlled by XTP GDI reference counter, meaning a handle is deleted
//    only when the last XTP GDI object that references a particular handle is
//    destroyed or DeleteObject is called.
//    Note, because an encapsulated GDI handle is cached and shared, passing
//    m_hObject to DeleteObject from Windows SDK or convering an XTP GDI
//    object to CGdiObject& and making a call to CGdiObject::DeleteObject
//    will delete a cached handle and leave all XTP GDI object that reference
//    the delete handle in an undetermined state. This will also affect every
//    place in a program where the cached handles are used. So use the encapsulated
//    GDI handle or CGdiObject very carefully and for read-only purposes.
// See also:
//    CXTPGdiObject, CXTPFont, CXTPPen
//=======================================================================
class _XTP_EXT_CLASS CXTPBrush : public CXTPGdiObject
{
	DECLARE_DYNAMIC(CXTPBrush);

public:
	//{{AFX_CODEJOCK_PRIVATE
	typedef CXTPGdiObjectPtrProxy<CBrush, CXTPBrush> XTPBrushPtrProxy;
	//}}AFX_CODEJOCK_PRIVATE

	//-----------------------------------------------------------------------
	// Summary:
	//      Constructs an empty CXTPBrush object.
	//-----------------------------------------------------------------------
	CXTPBrush();

	//-----------------------------------------------------------------------
	// Summary:
	//      Initializes a brush with a specified solid color.
	// Parameters:
	//      crColor - A COLORREF structure that specifies the color of the brush.
	// Remarks:
	//      If the requested brush has already been created from another
	//      CXTPBrush instance and not yet deleted the same brush will be re-used
	//      and its reference counter will be incremented.
	//-----------------------------------------------------------------------
	CXTPBrush(COLORREF crColor);

	//-----------------------------------------------------------------------
	// Summary:
	//      Initializes a brush with the specified hatched pattern and color.
	// Parameters:
	//      nIndex - Specifies the hatch style of the brush.
	//      crColor - A COLORREF structure that specifies the color of the brush.
	// Remarks:
	//      If the requested brush has already been created from another
	//      CXTPBrush instance and not yet deleted the same brush will be re-used
	//      and its reference counter will be incremented.
	//-----------------------------------------------------------------------
	CXTPBrush(int nIndex, COLORREF crColor);

	//-----------------------------------------------------------------------
	// Summary:
	//      Initializes a brush with a pattern specified by a bitmap.
	// Parameters:
	//      pBitmap - Pattern bitmap pointer.
	// Remarks:
	//      If the requested brush has already been created from another
	//      CXTPBrush instance and not yet deleted the same brush will be re-used
	//      and its reference counter will be incremented.
	//-----------------------------------------------------------------------
	CXTPBrush(CBitmap* pBitmap);

	//-----------------------------------------------------------------------
	// Summary:
	//      Handles CXTPBrush object destruction.
	//-----------------------------------------------------------------------
	~CXTPBrush();

	//-----------------------------------------------------------------------
	// Summary:
	//      Initializes a brush with a specified solid color.
	// Parameters:
	//      crColor - A COLORREF structure that specifies the color of the brush.
	// Returns:
	//      Nonzero if successful; otherwise 0.
	// Remarks:
	//      If the requested brush has already been created from another
	//      CXTPBrush instance and not yet deleted the same brush will be re-used
	//      and its reference counter will be incremented.
	//-----------------------------------------------------------------------
	BOOL CreateSolidBrush(COLORREF crColor);

	//-----------------------------------------------------------------------
	// Summary:
	//      Initializes a brush with the specified hatched pattern and color.
	// Parameters:
	//      nIndex - Specifies the hatch style of the brush.
	//      crColor - A COLORREF structure that specifies the color of the brush.
	// Returns:
	//      Nonzero if successful; otherwise 0.
	// Remarks:
	//      If the requested brush has already been created from another
	//      CXTPBrush instance and not yet deleted the same brush will be re-used
	//      and its reference counter will be incremented.
	//-----------------------------------------------------------------------
	BOOL CreateHatchBrush(int nIndex, COLORREF crColor);

	//-----------------------------------------------------------------------
	// Summary:
	//      Initializes a brush with a style, color, and pattern specified in
	//      a LOGBRUSH structure.
	// Parameters:
	//      lpLogBrush - Points to a LOGBRUSH structure that contains
	//                   information about the brush.
	// Returns:
	//      Nonzero if successful; otherwise 0.
	// Remarks:
	//      If the requested brush has already been created from another
	//      CXTPBrush instance and not yet deleted the same brush will be re-used
	//      and its reference counter will be incremented.
	//-----------------------------------------------------------------------
	BOOL CreateBrushIndirect(const LOGBRUSH* lpLogBrush);

	//-----------------------------------------------------------------------
	// Summary:
	//      Initializes a brush with a pattern specified by a bitmap.
	// Parameters:
	//      pBitmap - Pattern bitmap pointer.
	// Returns:
	//      Nonzero if successful; otherwise 0.
	// Remarks:
	//      If the requested brush has already been created from another
	//      CXTPBrush instance and not yet deleted the same brush will be re-used
	//      and its reference counter will be incremented.
	//-----------------------------------------------------------------------
	BOOL CreatePatternBrush(CBitmap* pBitmap);

	//-----------------------------------------------------------------------
	// Summary:
	//      Initializes a brush with the pattern specified by a device-independent bitmap (DIB).
	// Parameters:
	//      hPackedDIB - Identifies a global - memory object containing a
	//                   packed device - independent bitmap(DIB).
	//      lpPackedDIB - Points to a packed DIB consisting of a BITMAPINFO
	//                    structure immediately followed by an array of bytes
	//                    defining the pixels of the bitmap.
	//      nUsage - Specifies whether the bmiColors[] fields of the BITMAPINFO
	//               data structure(a part of the "packed DIB") contain explicit
	//               RGB values or indices into the currently realized logical palette.
	// Returns:
	//      Nonzero if successful; otherwise 0.
	// Remarks:
	//      If the requested brush has already been created from another
	//      CXTPBrush instance and not yet deleted the same brush will be re-used
	//      and its reference counter will be incremented.
	//-----------------------------------------------------------------------
	BOOL CreateDIBPatternBrush(HGLOBAL hPackedDIB, UINT nUsage);
	BOOL CreateDIBPatternBrush(const void* lpPackedDIB,
							   UINT nUsage); // <combine
											 // CXTPBrush::CreateDIBPatternBrush@HGLOBAL@UINT>

	//-----------------------------------------------------------------------
	// Summary:
	//      Initializes a brush with a system color.
	// Parameters:
	//      nIndex - Specifies a color index.
	// Returns:
	//      Nonzero if successful; otherwise 0.
	// Remarks:
	//      If the requested brush has already been created from another
	//      CXTPBrush instance and not yet deleted the same brush will be re-used
	//      and its reference counter will be incremented.
	//-----------------------------------------------------------------------
	BOOL CreateSysColorBrush(int nIndex);

	//-----------------------------------------------------------------------
	// Summary:
	//      Call this function to retrieve a copy of the LOGBRUSH structure that
	//      describes the encapsulated GDI object.
	// Parameters:
	//      pLogBrush - Pointer to the LOGBRUSH structure to receive the brush information.
	// Returns:
	//      Nonzero if the function succeeds, otherwise 0.
	//-----------------------------------------------------------------------
	int GetLogBrush(LOGBRUSH* pLogBrush);

	//-----------------------------------------------------------------------
	// Summary:
	//      Obtains a pointer to the encapsulated GDI object as CBrush type.
	//      The returned object must not be deleted or modified as it will affect
	//      every place in a program where the cached handle is used.
	// Returns:
	//      A pointer to the encapsulated GDI object as CBrush type.
	//-----------------------------------------------------------------------
	CBrush* GetNativeGdiObject();
	const CBrush* GetNativeGdiObject() const; // <combine CXTPBrush::GetNativeGdiObject>

	//-----------------------------------------------------------------------
	// Summary:
	//      Casts an object to an encapsulated GDI handle.
	// Remarks:
	//      The returned handle must not be passed to DeleteObject
	//      and should be used for read-only purposes, i.e. GDI object
	//      state should not be modified as it will affect every place
	//      in a program where the cached hande is used.
	// Returns:
	//      Encapsulated GDI handle or NULL.
	//-----------------------------------------------------------------------
	operator HBRUSH() const;

	//-----------------------------------------------------------------------
	// Summary:
	//      Converts an object to CBrush reference making it possible
	//      to use CXTPBrush with MFC classes and methods that use CBrush or
	//      CGdiObject reference as an argument.
	// Returns:
	//      An encapsulated CBrush reference.
	// Remarks:
	//      The returned object must not be deleted or modified as it will affect
	//      every place in a program where the cached handle is used.
	//-----------------------------------------------------------------------
	operator CBrush&();

	//-----------------------------------------------------------------------
	// Summary:
	//      Converts an object to CBrush constant reference making it possible
	//      to use CXTPBrush with MFC classes and methods that use CBrush or
	//      CGdiObject constant reference as an argument.
	// Returns:
	//      An encapsulated CBrush constant reference.
	// Remarks:
	//      The returned object must not be deleted or modified as it will affect
	//      every place in a program where the cached handle is used.
	//-----------------------------------------------------------------------
	operator const CBrush&() const;

	//{{AFX_CODEJOCK_PRIVATE
	XTPBrushPtrProxy operator&();
	const XTPBrushPtrProxy operator&() const;
	//}}AFX_CODEJOCK_PRIVATE

private:
	virtual XTPGDIDESCRIPTOR& AccessDescriptorData();

	CBrush m_dummyBrush;
	HGDIOBJ m_hLogBrushHandle;
	LOGBRUSH m_logBrush;
};

AFX_INLINE CBrush* CXTPBrush::GetNativeGdiObject()
{
	return static_cast<CBrush*>(GetGdiObject());
}

AFX_INLINE const CBrush* CXTPBrush::GetNativeGdiObject() const
{
	return const_cast<CXTPBrush*>(this)->GetNativeGdiObject();
}

AFX_INLINE CXTPBrush::operator HBRUSH() const
{
	return reinterpret_cast<HBRUSH>(GetSafeHandle());
}

AFX_INLINE CXTPBrush::operator CBrush&()
{
	return *GetNativeGdiObject();
}

AFX_INLINE CXTPBrush::operator const CBrush&() const
{
	return *GetNativeGdiObject();
}

AFX_INLINE CXTPBrush::XTPBrushPtrProxy CXTPBrush::operator&()
{
	return XTPBrushPtrProxy(this);
}

AFX_INLINE const CXTPBrush::XTPBrushPtrProxy CXTPBrush::operator&() const
{
	return XTPBrushPtrProxy(const_cast<CXTPBrush*>(this));
}

//=======================================================================
// Summary:
//    Provides a convenient object oriented interface for HPEN GDI object
//    and uses GDI handle caching. Mimics CPen's interface from MFC however
//    cannot be cast to CPen or CGdiObject types.
// Remarks:
//    XTP GDI objects use cached GDI handles so one native GDI handle can be
//    shared across several XTP GDI object instances. The lifetime of a handle
//    is controlled by XTP GDI reference counter, meaning a handle is deleted
//    only when the last XTP GDI object that references a particular handle is
//    destroyed or DeleteObject is called.
//    Note, because an encapsulated GDI handle is cached and shared, passing
//    m_hObject to DeleteObject from Windows SDK or convering an XTP GDI
//    object to CGdiObject& and making a call to CGdiObject::DeleteObject
//    will delete a cached handle and leave all XTP GDI object that reference
//    the delete handle in an undetermined state. This will also affect every
//    place in a program where the cached handles are used. So use the encapsulated
//    GDI handle or CGdiObject very carefully and for read-only purposes.
// See also:
//    CXTPGdiObject, CXTPFont, CXTPBrush
//=======================================================================
class _XTP_EXT_CLASS CXTPPen : public CXTPGdiObject
{
	DECLARE_DYNAMIC(CXTPPen);

public:
	//{{AFX_CODEJOCK_PRIVATE
	typedef CXTPGdiObjectPtrProxy<CPen, CXTPPen> XTPPenPtrProxy;
	//}}AFX_CODEJOCK_PRIVATE

	//-----------------------------------------------------------------------
	// Summary:
	//      Constructs an empty CXTPPen object.
	//-----------------------------------------------------------------------
	CXTPPen();

	//-----------------------------------------------------------------------
	// Summary:
	//      Constructs a CXTPPen object.
	// Parameters:
	//      nPenStyle - Specifies the pen style.
	//      nWidth - Specifies the width of the pen.
	//      crColor - Contains an RGB color for the pen.
	//      pLogBrush - A pointer to LOGBRUSH structure used differently
	//                  depending on pen style.
	//      nStyleCount - The number of optional style parameters.
	//      lpStyle - An optional pointer to the array of style parameters.
	// Remarks:
	//      If the requested pen has already been created from another
	//      CXTPPen instance and not yet deleted the same pen will be re-used
	//      and its reference counter will be incremented.
	//-----------------------------------------------------------------------
	CXTPPen(int nPenStyle, int nWidth, COLORREF crColor);
	CXTPPen(int nPenStyle, int nWidth, const LOGBRUSH* pLogBrush, int nStyleCount = 0,
			const DWORD* lpStyle = NULL); // <combine CXTPPen::CXTPPen@int@int@COLORREF>

	//-----------------------------------------------------------------------
	// Summary:
	//      Handles CXTPPen object destruction.
	//-----------------------------------------------------------------------
	~CXTPPen();

	//-----------------------------------------------------------------------
	// Summary:
	//      Creates a pen with specified parameters.
	// Parameters:
	//      nPenStyle - Specifies the pen style.
	//      nWidth - Specifies the width of the pen.
	//      crColor - Contains an RGB color for the pen.
	//      pLogBrush - A pointer to LOGBRUSH structure used differently
	//                  depending on pen style.
	//      nStyleCount - The number of optional style parameters.
	//      lpStyle - An optional pointer to the array of style parameters.
	//      lpLogPen - A pointer to LOGPEN structu describing a pen
	//                 to be created.
	// Returns:
	//      TRUE if a pen is successfully created.
	// Remarks:
	//      If the requested pen has already been created from another
	//      CXTPPen instance and not yet deleted the same pen will be re-used
	//      and its reference counter will be incremented.
	//-----------------------------------------------------------------------
	BOOL CreatePen(int nPenStyle, int nWidth, COLORREF crColor);
	BOOL CreatePen(int nPenStyle, int nWidth, const LOGBRUSH* pLogBrush, int nStyleCount = 0,
				   const DWORD* lpStyle = NULL); // <combine CXTPPen::CreatePen@int@int@COLORREF>
	BOOL CreatePenIndirect(LPLOGPEN lpLogPen);	 // <combine CXTPPen::CreatePen@int@int@COLORREF>

	//-----------------------------------------------------------------------
	// Summary:
	//      Call this function to retrieve a copy of the LOGPEN structure that
	//      describes the encapsulated GDI object.
	// Parameters:
	//      pLogPen - Pointer to the LOGPEN structure to receive the pen information.
	// Returns:
	//      Nonzero if the function succeeds, otherwise 0.
	//-----------------------------------------------------------------------
	int GetLogPen(LOGPEN* pLogPen);

	//-----------------------------------------------------------------------
	// Summary:
	//      Call this function to retrieve a copy of the EXTLOGPEN structure that
	//      describes the encapsulated GDI object.
	// Parameters:
	//      pLogPen - Pointer to the EXTLOGPEN structure to receive the pen information.
	// Returns:
	//      Nonzero if the function succeeds, otherwise 0.
	//-----------------------------------------------------------------------
	int GetExtLogPen(EXTLOGPEN* pLogPen);

	//-----------------------------------------------------------------------
	// Summary:
	//      Obtains a pointer to the encapsulated GDI object as CPen type.
	//      The returned object must not be deleted or modified as it will affect
	//      every place in a program where the cached handle is used.
	// Returns:
	//      A pointer to the encapsulated GDI object as CPen type.
	//-----------------------------------------------------------------------
	CPen* GetNativeGdiObject();
	const CPen* GetNativeGdiObject() const; // <combine CXTPPen::GetNativeGdiObject>

	//-----------------------------------------------------------------------
	// Summary:
	//      Casts an object to an encapsulated GDI handle.
	// Remarks:
	//      The returned handle must not be passed to DeleteObject
	//      and should be used for read-only purposes, i.e. GDI object
	//      state should not be modified as it will affect every place
	//      in a program where the cached hande is used.
	// Returns:
	//      Encapsulated GDI handle or NULL.
	//-----------------------------------------------------------------------
	operator HPEN() const;

	//-----------------------------------------------------------------------
	// Summary:
	//      Converts an object to CPen reference making it possible
	//      to use CXTPPen with MFC classes and methods that use CPen or
	//      CGdiObject reference as an argument.
	// Returns:
	//      An encapsulated CPen reference.
	// Remarks:
	//      The returned object must not be deleted or modified as it will affect
	//      every place in a program where the cached handle is used.
	//-----------------------------------------------------------------------
	operator CPen&();

	//-----------------------------------------------------------------------
	// Summary:
	//      Converts an object to CPen constant reference making it possible
	//      to use CXTPPen with MFC classes and methods that use CPen or
	//      CGdiObject constant reference as an argument.
	// Returns:
	//      An encapsulated CPen constant reference.
	// Remarks:
	//      The returned object must not be deleted or modified as it will affect
	//      every place in a program where the cached handle is used.
	//-----------------------------------------------------------------------
	operator const CPen&() const;

	//{{AFX_CODEJOCK_PRIVATE
	XTPPenPtrProxy operator&();
	const XTPPenPtrProxy operator&() const;
	//}}AFX_CODEJOCK_PRIVATE

private:
	virtual XTPGDIDESCRIPTOR& AccessDescriptorData();

	CPen m_dummyPen;
	HGDIOBJ m_hLogPenHandle;
	LOGPEN m_logPen;
};

AFX_INLINE CPen* CXTPPen::GetNativeGdiObject()
{
	return static_cast<CPen*>(GetGdiObject());
}

AFX_INLINE const CPen* CXTPPen::GetNativeGdiObject() const
{
	return const_cast<CXTPPen*>(this)->GetNativeGdiObject();
}

AFX_INLINE CXTPPen::operator HPEN() const
{
	return reinterpret_cast<HPEN>(GetSafeHandle());
}

AFX_INLINE CXTPPen::operator CPen&()
{
	return *GetNativeGdiObject();
}

AFX_INLINE CXTPPen::operator const CPen&() const
{
	return *GetNativeGdiObject();
}

AFX_INLINE CXTPPen::XTPPenPtrProxy CXTPPen::operator&()
{
	return XTPPenPtrProxy(this);
}

AFX_INLINE const CXTPPen::XTPPenPtrProxy CXTPPen::operator&() const
{
	return XTPPenPtrProxy(const_cast<CXTPPen*>(this));
}

//{{AFX_CODEJOCK_PRIVATE

class CXTPGdiObjectCacheBase
	: public CObject
	, public CXTPSynchronized
{
	DECLARE_DYNAMIC(CXTPGdiObjectCacheBase);

protected:
	CXTPGdiObjectCacheBase()
	{
	}
	virtual ~CXTPGdiObjectCacheBase()
	{
	}

public:
	virtual void RemoveFromCache(const void* pDescriptor) = 0;
};

struct XTPGDIDESCRIPTOR
{
	XTPGDIDESCRIPTOR()
		: pReference(NULL)
		, nHandleOwnershipViolatorCount(0)
	{
	}

	CXTPCachedGdiObjectReferenceBase* pReference;
	LONG nHandleOwnershipViolatorCount;
};

struct XTPPENINFO;
class CXTPCachedPenReference;

template<>
UINT AFXAPI HashKey(XTPPENINFO* pPenInfo);
template<>
BOOL AFXAPI CompareElements(XTPPENINFO* const* pPenInfo1, XTPPENINFO* const* pPenInfo2);

class CXTPPenCache : private CXTPGdiObjectCacheBase
{
	DECLARE_DYNAMIC(CXTPPenCache);

	friend class CXTPSingleton<CXTPPenCache>;

public:
	static XTPGDIDESCRIPTOR& AccessDescriptorData(void* pDescriptor);

	CXTPCachedPenReference* CreatePen(int nPenStyle, int nWidth, COLORREF crColor);
	CXTPCachedPenReference* CreatePen(int nPenStyle, int nWidth, const LOGBRUSH* pLogBrush,
									  int nStyleCount = 0, const DWORD* lpStyle = NULL);
	CXTPCachedPenReference* CreatePenIndirect(LPLOGPEN lpLogPen);

private:
	CXTPPenCache();
	~CXTPPenCache();

	virtual void RemoveFromCache(const void* pDescriptor);

	friend CXTPPenCache* AFX_CDECL XTPPenCache();

	// LPVOID used instead of XTPCachedBrushReference* as a workaround for warning C4786.
	CMap<XTPPENINFO*, XTPPENINFO*, LPVOID, LPVOID&> m_penCache;
};

struct XTPLOGBRUSHDESCRIPTOR : XTPGDIDESCRIPTOR
{
	LOGBRUSH logBrush;
};

template<>
UINT AFXAPI HashKey(const LOGBRUSH& logBrush);
template<>
BOOL AFXAPI CompareElements(const LOGBRUSH* logBrush1, const LOGBRUSH* logBrush2);

class CXTPBrushCache : private CXTPGdiObjectCacheBase
{
	DECLARE_DYNAMIC(CXTPBrushCache);

	friend class CXTPSingleton<CXTPBrushCache>;
	friend class CXTPCachedGdiObjectReference<XTPLOGBRUSHDESCRIPTOR, CBrush, CXTPBrushCache>;

public:
	typedef CXTPCachedGdiObjectReference<XTPLOGBRUSHDESCRIPTOR, CBrush, CXTPBrushCache>
		XTPCachedBrushReference;

	static XTPGDIDESCRIPTOR& AccessDescriptorData(void* pDescriptor);

	XTPCachedBrushReference* CreateSolidBrush(COLORREF crColor);
	XTPCachedBrushReference* CreateHatchBrush(int nIndex, COLORREF crColor);
	XTPCachedBrushReference* CreateBrushIndirect(const LOGBRUSH* lpLogBrush);
	XTPCachedBrushReference* CreatePatternBrush(CBitmap* pBitmap);
	XTPCachedBrushReference* CreateDIBPatternBrush(HGLOBAL hPackedDIB, UINT nUsage);
	XTPCachedBrushReference* CreateDIBPatternBrush(const void* lpPackedDIB, UINT nUsage);
	XTPCachedBrushReference* CreateSysColorBrush(int nIndex);

private:
	CXTPBrushCache();
	~CXTPBrushCache();

	virtual void RemoveFromCache(const void* pDescriptor);

	friend CXTPBrushCache* AFX_CDECL XTPBrushCache();

	// LPVOID used instead of XTPCachedBrushReference* as a workaround for warning C4786.
	CMap<LOGBRUSH, const LOGBRUSH&, LPVOID, LPVOID&> m_brushCache;
};

struct XTPLOGFONTDESCRIPTOR : XTPGDIDESCRIPTOR
{
	LOGFONT logFont;
};

template<>
UINT AFXAPI HashKey(const LOGFONT& logFont);
template<>
BOOL AFXAPI CompareElements(const LOGFONT* logFont1, const LOGFONT* logFont2);

class CXTPFontCache : private CXTPGdiObjectCacheBase
{
	DECLARE_DYNAMIC(CXTPFontCache);

	friend class CXTPSingleton<CXTPFontCache>;
	friend class CXTPCachedGdiObjectReference<XTPLOGFONTDESCRIPTOR, CFont, CXTPFontCache>;

public:
	typedef CXTPCachedGdiObjectReference<XTPLOGFONTDESCRIPTOR, CFont, CXTPFontCache>
		XTPCachedFontReference;

	static XTPGDIDESCRIPTOR& AccessDescriptorData(void* pDescriptor);

	XTPCachedFontReference* CreateFontIndirect(const LOGFONT* lpLogFont);
	XTPCachedFontReference* CreateFont(int nHeight, int nWidth, int nEscapement, int nOrientation,
									   int nWeight, BYTE bItalic, BYTE bUnderline, BYTE cStrikeOut,
									   BYTE nCharSet, BYTE nOutPrecision, BYTE nClipPrecision,
									   BYTE nQuality, BYTE nPitchAndFamily, LPCTSTR lpszFaceName);
	XTPCachedFontReference* CreatePointFont(int nPointSize, LPCTSTR lpszFaceName, CDC* pDC = NULL);
	XTPCachedFontReference* CreatePointFontIndirect(const LOGFONT* lpLogFont, CDC* pDC = NULL);

private:
	CXTPFontCache();
	~CXTPFontCache();

	virtual void RemoveFromCache(const void* pDescriptor);

	friend CXTPFontCache* AFX_CDECL XTPFontCache();

	// LPVOID used instead of XTPCachedFontReference* as a workaround for warning C4786.
	CMap<LOGFONT, const LOGFONT&, LPVOID, LPVOID&> m_fontCache;
};

//}}AFX_CODEJOCK_PRIVATE

#	include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif /*__XTPGDIOBJECTCACHE_H__*/
