// XTPMarkupStaticExtension.h: interface for the CXTPMarkupStaticExtension class.
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
#if !defined(__XTPMARKUPSTATICEXTENSION_H__)
#	define __XTPMARKUPSTATICEXTENSION_H__
//}}AFX_CODEJOCK_PRIVATE

#	if _MSC_VER > 1000
#		pragma once
#	endif // _MSC_VER > 1000

#	include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

class CXTPMarkupObject;
class CXTPMarkupColor;

//===========================================================================
// Summary:
//     A base class for a markup static extension composite object.
//===========================================================================
class _XTP_EXT_CLASS CXTPMarkupStaticExtension
{
protected:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a composite or property CXTPMarkupStaticExtension instance.
	// Parameters:
	//     lpzName   - Pointer to an extension name.
	//     bProperty - TRUE if the object is a property object
	//                 (i.e. it cannot be extended with other extensions
	//                 and it provides a final value of an extension).
	//-----------------------------------------------------------------------
	CXTPMarkupStaticExtension(LPCWSTR lpzName, BOOL bProperty);

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a composite CXTPMarkupStaticExtension instance.
	// Parameters:
	//     lpzName - Pointer to an extension name.
	//-----------------------------------------------------------------------
	CXTPMarkupStaticExtension(LPCWSTR lpzName);

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPMarkupStaticExtension object, handles cleanup and deallocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPMarkupStaticExtension();

	//-----------------------------------------------------------------------
	// Summary:
	//     Parses a static extension expression of form Name.Value[.Value[...]].
	//     where both name and value has to be of form [a-zA-Z\_]+.
	// Parameters:
	//     lpzValue    - Entire static extension value.
	//     lpzSubValue - On successful completion, contains a pointer to a
	//                   sub-value of the extension object.
	// Returns:
	//     TRUE if parsed successfully.
	//-----------------------------------------------------------------------
	static BOOL AFX_CDECL Parse(LPCWSTR lpzValue, LPCWSTR& lpzSubValue);

	//-----------------------------------------------------------------------
	// Summary:
	//     Gets the name of the current extension.
	// Returns:
	//     A reference to the name of the current extension.
	//-----------------------------------------------------------------------
	LPCWSTR GetName() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines if the current extension is a property extension.
	// Returns:
	//     TRUE if the current extension is a property extension.
	//-----------------------------------------------------------------------
	BOOL IsProperty() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Extends the current extension object with another sub-extension.
	// Parameters:
	//     pExtension - Pointer to a sub-extension object to be added to the
	//                  current extension. The sub-extension object will be
	//                  deallocated during parent extension destruction.
	//-----------------------------------------------------------------------
	void Extend(CXTPMarkupStaticExtension* pExtension);

	//-----------------------------------------------------------------------
	// Summary:
	//     Tries to find an extension by its name or relative path.
	// Parameters:
	//     lpzName - Pointer to an extension name or relative path of
	//               form Name[.Name[...]].
	// Returns:
	//     A pointer to an extension if such an extension is found,
	//     otherwise NULL.
	//-----------------------------------------------------------------------
	CXTPMarkupStaticExtension* FindExtension(LPCWSTR lpzName) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Provides the value of a composite extension object.
	// Parameters:
	//     lpzValue - Composite extension value of form Value[.Value[...]].
	// Returns:
	//     A pointer to a markup object that represents the value of the
	//     composite extension object if successful, or NULL if no value is
	//     found. It is the responsibility of the caller to release the object.
	//-----------------------------------------------------------------------
	virtual CXTPMarkupObject* ProvideValue(LPCWSTR lpzValue);

	//-----------------------------------------------------------------------
	// Summary:
	//     Provides the value of a property extension object.
	// Returns:
	//     A pointer to a markup object that represents the value of the
	//     property extension object if successful, or NULL if no value is
	//     found. It is the responsibility of the caller to release the object.
	//-----------------------------------------------------------------------
	virtual CXTPMarkupObject* ProvideValue();

private:
	static BOOL AFX_CDECL ValidateExtensionName(LPCWSTR lpzBegin, LPCWSTR lpzEnd);

private:
	BOOL m_bProperty;
	LPWSTR m_lpzName;
	CMap<LPCWSTR, LPCWSTR, CXTPMarkupStaticExtension*, CXTPMarkupStaticExtension*> m_mapExtensions;
};

AFX_INLINE LPCWSTR CXTPMarkupStaticExtension::GetName() const
{
	return m_lpzName;
}

AFX_INLINE BOOL CXTPMarkupStaticExtension::IsProperty() const
{
	return m_bProperty;
}

//===========================================================================
// Summary:
//     A convenience base class for all static extension properties.
//===========================================================================
class _XTP_EXT_CLASS CXTPMarkupStaticExtensionProperty : public CXTPMarkupStaticExtension
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPMarkupStaticExtensionProperty with an optional
	//     initial property value.
	// Parameters:
	//     lpzName - Static extension property name.
	//     pObject - Optional pointer to an initial property value. It is the
	//               responsibility of the class to release the value object instance.
	//-----------------------------------------------------------------------
	CXTPMarkupStaticExtensionProperty(LPCWSTR lpzName, CXTPMarkupObject* pObject = NULL);

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPMarkupStaticExtensionProperty object, handles cleanup
	//     and deallocation.
	//-----------------------------------------------------------------------
	~CXTPMarkupStaticExtensionProperty();

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets the property value.
	// Parameters:
	//     pObject - Pointer to the property value or NULL. It is the
	//               responsibility of the class to release the value object instance.
	//-----------------------------------------------------------------------
	void SetValue(CXTPMarkupObject* pObject);

	//-----------------------------------------------------------------------
	// Summary:
	//     Provides the value of a property extension object.
	// Returns:
	//     A pointer to a markup object that represents the value of the
	//     property extension object if successful, or NULL if no value is
	//     found. It is the responsibility of the caller to release the object.
	//-----------------------------------------------------------------------
	virtual CXTPMarkupObject* ProvideValue();

	//-----------------------------------------------------------------------
	// Summary:
	//     Provides the value of a composite extension object.
	// Parameters:
	//     lpzValue - Composite extension value of form Value[.Value[...]].
	// Returns:
	//     A pointer to a markup object that represents the value of the
	//     composite extension object if successful, or NULL if no value is
	//     found. It is the responsibility of the caller to release the object.
	//-----------------------------------------------------------------------
	virtual CXTPMarkupObject* ProvideValue(LPCWSTR lpzValue);

protected:
	CXTPMarkupObject* m_pObject;
};

//===========================================================================
// Summary:
//     A convenience class that represents a string static extension property.
//===========================================================================
class _XTP_EXT_CLASS CXTPMarkupStaticExtensionStringProperty
	: public CXTPMarkupStaticExtensionProperty
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPMarkupStaticExtensionStringProperty with an optional
	//     initial property value.
	// Parameters:
	//     lpzName  - Static extension property name.
	//     lpzValue - Optional pointer to an initial property value.
	//-----------------------------------------------------------------------
	CXTPMarkupStaticExtensionStringProperty(LPCWSTR lpzName, LPCWSTR lpzValue = NULL);

	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPMarkupStaticExtensionStringProperty with an optional
	//     initial property value.
	// Parameters:
	//     lpzName  - Static extension property name.
	//     lpzValue - Optional pointer to an initial property value.
	//-----------------------------------------------------------------------
	CXTPMarkupStaticExtensionStringProperty(LPCWSTR lpzName, LPCSTR lpzValue = NULL);

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets the property value.
	// Parameters:
	//     lpzValue - Pointer to the property value or NULL. The string will be duplicated.
	//-----------------------------------------------------------------------
	void SetValue(LPCWSTR lpzValue);

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets the property value.
	// Parameters:
	//     lpzValue - Pointer to the property value or NULL. The string will be duplicated.
	//-----------------------------------------------------------------------
	void SetValue(LPCSTR lpzValue);

	//-----------------------------------------------------------------------
	// Summary:
	//     Obtains a pointer to a raw object value.
	// Returns:
	//     A pointer to a raw object value, or NULL if no value is assigned.
	//-----------------------------------------------------------------------
	LPCWSTR GetRawValue() const;
};

//===========================================================================
// Summary:
//     A convenience class that represents an integer static extension property.
//===========================================================================
class _XTP_EXT_CLASS CXTPMarkupStaticExtensionIntegerProperty
	: public CXTPMarkupStaticExtensionProperty
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPMarkupStaticExtensionIntegerProperty with an optional
	//     initial property value.
	// Parameters:
	//     lpzName - Static extension property name.
	//     nValue  - Optional initial property value.
	//-----------------------------------------------------------------------
	CXTPMarkupStaticExtensionIntegerProperty(LPCWSTR lpzName, int nValue = 0);

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets the property value.
	// Parameters:
	//     nValue - New property value.
	//-----------------------------------------------------------------------
	void SetValue(int nValue);

	//-----------------------------------------------------------------------
	// Summary:
	//     Gets the property value.
	// Returns:
	//     The property value.
	//-----------------------------------------------------------------------
	int GetRawValue() const;
};

//===========================================================================
// Summary:
//     A convenience class that represents a boolean static extension property.
//===========================================================================
class _XTP_EXT_CLASS CXTPMarkupStaticExtensionBooleanProperty
	: public CXTPMarkupStaticExtensionProperty
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPMarkupStaticExtensionBooleanProperty with an optional
	//     initial property value.
	// Parameters:
	//     lpzName - Static extension property name.
	//     bValue  - Optional initial property value.
	//-----------------------------------------------------------------------
	CXTPMarkupStaticExtensionBooleanProperty(LPCWSTR lpzName, BOOL bValue = FALSE);

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets the property value.
	// Parameters:
	//     bValue - New property value.
	//-----------------------------------------------------------------------
	void SetValue(BOOL bValue);

	//-----------------------------------------------------------------------
	// Summary:
	//     Gets the property value.
	// Returns:
	//     The property value.
	//-----------------------------------------------------------------------
	BOOL GetRawValue() const;
};

//===========================================================================
// Summary:
//     A convenience class that represents a double static extension property.
//===========================================================================
class _XTP_EXT_CLASS CXTPMarkupStaticExtensionDoubleProperty
	: public CXTPMarkupStaticExtensionProperty
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPMarkupStaticExtensionDoubleProperty with an optional
	//     initial property value.
	// Parameters:
	//     lpzName  - Static extension property name.
	//     dblValue - Optional initial property value.
	//-----------------------------------------------------------------------
	CXTPMarkupStaticExtensionDoubleProperty(LPCWSTR lpzName, double dblValue = 0);

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets the property value.
	// Parameters:
	//     dblValue - New property value.
	//-----------------------------------------------------------------------
	void SetValue(double dblValue);

	//-----------------------------------------------------------------------
	// Summary:
	//     Gets the property value.
	// Returns:
	//     The property value.
	//-----------------------------------------------------------------------
	double GetRawValue() const;
};

//===========================================================================
// Summary:
//     A convenience class that represents an RGB color static extension property.
//===========================================================================
class _XTP_EXT_CLASS CXTPMarkupStaticExtensionColorProperty
	: public CXTPMarkupStaticExtensionProperty
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPMarkupStaticExtensionColorProperty with an optional
	//     initial property value.
	// Parameters:
	//     lpzName - Static extension property name.
	//     crValue - Optional initial property value.
	//-----------------------------------------------------------------------
	CXTPMarkupStaticExtensionColorProperty(LPCWSTR lpzName, COLORREF crValue = RGB(0, 0, 0));

	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPMarkupStaticExtensionColorProperty with an optional
	//     initial property value.
	// Parameters:
	//     lpzName - Static extension property name.
	//     crValue - Initial property color value.
	//     bAlpha  - Initial property color alpha-channel value.
	//-----------------------------------------------------------------------
	CXTPMarkupStaticExtensionColorProperty(LPCWSTR lpzName, COLORREF crValue, BYTE bAlpha);

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets the property value.
	// Parameters:
	//     crValue - New property value.
	//-----------------------------------------------------------------------
	void SetValue(COLORREF crValue);

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets the property value.
	// Parameters:
	//     crValue - Initial property color value.
	//     bAlpha  - Initial property color alpha-channel value.
	//-----------------------------------------------------------------------
	void SetValue(COLORREF crValue, BYTE bAlpha);

	//-----------------------------------------------------------------------
	// Summary:
	//     Gets the property value.
	// Returns:
	//     The property value.
	//-----------------------------------------------------------------------
	COLORREF GetRawValue() const;
};

//===========================================================================
// Summary:
//     A convenience class that represents a brush static extension property.
//===========================================================================
class _XTP_EXT_CLASS CXTPMarkupStaticExtensionBrushProperty
	: public CXTPMarkupStaticExtensionProperty
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPMarkupStaticExtensionBrushProperty with an optional
	//     initial property value.
	// Parameters:
	//     lpzName - Static extension property name.
	//     crValue - Optional initial property value.
	//-----------------------------------------------------------------------
	CXTPMarkupStaticExtensionBrushProperty(LPCWSTR lpzName, COLORREF crValue = RGB(0, 0, 0));

	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPMarkupStaticExtensionBrushProperty with an optional
	//     initial property value.
	// Parameters:
	//     lpzName - Static extension property name.
	//     crValue - Initial property color value.
	//     bAlpha  - Initial property color alpha-channel value.
	//-----------------------------------------------------------------------
	CXTPMarkupStaticExtensionBrushProperty(LPCWSTR lpzName, COLORREF crValue, BYTE bAlpha);

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets the property value.
	// Parameters:
	//     crValue - New property value.
	//-----------------------------------------------------------------------
	void SetValue(COLORREF crValue);

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets the property value.
	// Parameters:
	//     crValue - Initial property color value.
	//     bAlpha  - Initial property color alpha-channel value.
	//-----------------------------------------------------------------------
	void SetValue(COLORREF crValue, BYTE bAlpha);

	//-----------------------------------------------------------------------
	// Summary:
	//     Gets the property value.
	// Returns:
	//     The property value.
	//-----------------------------------------------------------------------
	COLORREF GetRawValue() const;
};

#	include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPMARKUPSTATICEXTENSION_H__)
