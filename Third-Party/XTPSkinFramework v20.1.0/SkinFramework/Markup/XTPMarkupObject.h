// XTPMarkupObject.h: interface for the CXTPMarkupObject class.
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
#if !defined(__XTPMARKUPOBJECT_H__)
#	define __XTPMARKUPOBJECT_H__
//}}AFX_CODEJOCK_PRIVATE

#	if _MSC_VER > 1000
#		pragma once
#	endif // _MSC_VER > 1000

#	include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

class CXTPMarkupDrawingContext;
class CXTPMarkupDependencyProperty;
class CXTPMarkupObject;
class CXTPMarkupBuilder;
class CXTPMarkupPropertyFromNameMap;
class CXTPMarkupPropertyChangedEventArgs;
class CXTPMarkupContext;
class CXTPMarkupProperties;
class CXTPMarkupType;
class CXTPMarkupStyle;
class CXTPMarkupBuilder;
class CXTPMarkupExtension;
class CXTPComInitializer;

namespace XTPXML
{
struct IXMLDOMElement;
}

enum XTPMarkupTextAlignment
{
	xtpMarkupTextAlignmentLeft,
	xtpMarkupTextAlignmentRight,
	xtpMarkupTextAlignmentCenter,
	xtpMarkupTextAlignmentJustify
};

enum XTPMarkupHorizontalAlignment
{
	xtpMarkupHorizontalAlignmentLeft,
	xtpMarkupHorizontalAlignmentCenter,
	xtpMarkupHorizontalAlignmentRight,
	xtpMarkupHorizontalAlignmentStretch
};

enum XTPMarkupVerticalAlignment
{
	xtpMarkupVerticalAlignmentTop,
	xtpMarkupVerticalAlignmentCenter,
	xtpMarkupVerticalAlignmentBottom,
	xtpMarkupVerticalAlignmentStretch
};

enum XTPMarkupOrientation
{
	xtpMarkupOrientationHorizontal,
	xtpMarkupOrientationVertical
};

enum XTPMarkupBaselineAlignment
{
	xtpMarkupBaselineTop,
	xtpMarkupBaselineCenter,
	xtpMarkupBaselineBottom,
	xtpMarkupBaseline,
	xtpMarkupBaselineTextTop,
	xtpMarkupBaselineTextBottom,
	xtpMarkupBaselineSubscript,
	xtpMarkupBaselineSuperscript
};

enum XTPMarkupVisibility
{
	xtpMarkupVisibilityVisible	 = 0,
	xtpMarkupVisibilityHidden	 = 1,
	xtpMarkupVisibilityCollapsed = 2
};

enum XTPMarkupLineCap
{
	xtpMarkupLineCapFlat,
	xtpMarkupLineCapSquare,
	xtpMarkupLineCapRound,
	xtpMarkupLineCapTriangle
};

enum XTPMarkupLineJoin
{
	xtpMarkupLineJoinMiter,
	xtpMarkupLineJoinBevel,
	xtpMarkupLineJoinRound
};

typedef CXTPMarkupObject* (*PFNCONVERTFROM)(
	CXTPMarkupBuilder* pBuilder, CXTPMarkupObject* pObject,
	CXTPMarkupDependencyProperty* pAssociatedProperty /*= NULL*/);
typedef void (*PFNPROPERTYCHANGED)(CXTPMarkupObject* d, CXTPMarkupPropertyChangedEventArgs* e);

class _XTP_EXT_CLASS CXTPMarkupPropertyMetadata
{
public:
	enum MetadataFlags
	{
		flagInherited			 = 1,
		flagAffectsArrange		 = 2,
		flagAffectsMeasure		 = 4,
		flagAffectsParentArrange = 8,
		flagAffectsParentMeasure = 16,
		flagAffectsRender		 = 32,
		flagDpiSensible			 = 64,
		flagHorzDpiSensible		 = flagDpiSensible,
		flagVertDpiSensible		 = 128,
	};

public:
	CXTPMarkupPropertyMetadata(CXTPMarkupObject* pDefaultValue, DWORD dwFlags = 0);
	CXTPMarkupPropertyMetadata(CXTPMarkupObject* pDefaultValue, PFNCONVERTFROM pConverter,
							   DWORD dwFlags = 0);
	CXTPMarkupPropertyMetadata(CXTPMarkupObject* pDefaultValue, PFNPROPERTYCHANGED pPropertyChanged,
							   DWORD dwFlags = 0);
	CXTPMarkupPropertyMetadata(CXTPMarkupObject* pDefaultValue, PFNCONVERTFROM pConverter,
							   PFNPROPERTYCHANGED pPropertyChanged, DWORD dwFlags = 0);
	virtual ~CXTPMarkupPropertyMetadata();

public:
	CXTPMarkupObject* m_pDefaultValue;
	PFNCONVERTFROM m_pConverter;
	PFNPROPERTYCHANGED m_pPropertyChanged;
	DWORD m_dwFlags;

	friend class CXTPMarkupObject;
};

#	define MARKUP_RELEASE(x)                                                                      \
		if (x)                                                                                     \
		{                                                                                          \
			x->Release();                                                                          \
			x = 0;                                                                                 \
		}
#	define MARKUP_ADDREF(x)                                                                       \
		if (x)                                                                                     \
		{                                                                                          \
			x->AddRef();                                                                           \
		}

#	define MARKUP_TYPE(class_name) ((CXTPMarkupType*)(class_name::Get##class_name##Type()))

#	define DECLARE_MARKUPCLASS(class_name)                                                        \
		DECLARE_DYNAMIC(class_name)                                                                \
	public:                                                                                        \
		static CXTPMarkupType* AFX_CDECL GetMarkupBaseType();                                      \
		static CXTPMarkupObject* AFX_CDECL CreateMarkupObject(CXTPMarkupContext* pContext);        \
		static void AFX_CDECL RegisterMarkupClass();                                               \
		virtual CXTPMarkupType* GetType() const;                                                   \
		static void AFX_CDECL RegisterType();                                                      \
		static CXTPMarkupType* Get##class_name##Type();                                            \
		static void* markupTypeEntry##class_name;

#	define IMPLEMENT_MARKUPCLASS(class_tag, class_name, base_class_name)                          \
		IMPLEMENT_DYNAMIC(class_name, base_class_name);                                            \
		void* class_name::markupTypeEntry##class_name =                                            \
			CXTPMarkupType::RegisterTypeEntry(class_tag, class_name::Get##class_name##Type);       \
		CXTPMarkupObject* AFX_CDECL class_name::CreateMarkupObject(CXTPMarkupContext* pContext)    \
		{                                                                                          \
			class_name* obj = new class_name;                                                      \
			if (obj)                                                                               \
				obj->m_pMarkupContext = pContext;                                                  \
			return obj;                                                                            \
		}                                                                                          \
		CXTPMarkupType* AFX_CDECL class_name::GetMarkupBaseType()                                  \
		{                                                                                          \
			return MARKUP_TYPE(base_class_name);                                                   \
		}                                                                                          \
		CXTPMarkupType* class_name::Get##class_name##Type()                                        \
		{                                                                                          \
			static CXTPMarkupType* type = new CXTPMarkupType(class_tag, L#class_name,              \
															 &class_name::CreateMarkupObject,      \
															 &class_name::GetMarkupBaseType,       \
															 &class_name::RegisterMarkupClass);    \
			return type;                                                                           \
		}                                                                                          \
		CXTPMarkupType* class_name::GetType() const                                                \
		{                                                                                          \
			return MARKUP_TYPE(class_name);                                                        \
		}                                                                                          \
		void AFX_CDECL class_name::RegisterType()                                                  \
		{                                                                                          \
			CXTPMarkupType::ResolveTypeEntry(markupTypeEntry##class_name);                         \
			MARKUP_TYPE(class_name)->Register();                                                   \
		}

#	ifdef _DEBUG
#		define MARKUP_STATICCAST(class_name, object)                                              \
			((class_name*)CXTPMarkupType::StaticDownCast(MARKUP_TYPE(class_name), object))
#	else
#		define MARKUP_STATICCAST(class_name, object) ((class_name*)object)
#	endif

#	undef MARKUP_DYNAMICCAST
#	define MARKUP_DYNAMICCAST(class_name, object)                                                 \
		(class_name*)CXTPMarkupType::DynamicDownCast(MARKUP_TYPE(class_name), object)

class _XTP_EXT_CLASS CXTPMarkupObject : public CXTPCmdTarget
{
	DECLARE_MARKUPCLASS(CXTPMarkupObject);

public:
	enum Flags
	{
		flagNoCom = 1
	};

	CXTPMarkupObject(int nFlags = 0);

protected:
	virtual ~CXTPMarkupObject();

public:
	CXTPMarkupContext* GetMarkupContext() const;

	XTPXML::IXMLDOMElement* GetSourceMarkupElement() const;
	void SetSourceMarkupElement(XTPXML::IXMLDOMElement* pElement);

	LPCWSTR GetKey() const;
	void SetKey(LPCWSTR lpszKey);

	LPCWSTR GetName() const;
	void SetName(LPCWSTR lpszName);

	void SetValue(CXTPMarkupDependencyProperty* pProperty, CXTPMarkupObject* pValue);
	CXTPMarkupObject* GetValue(CXTPMarkupDependencyProperty* pProperty) const;

	CXTPMarkupObject* GetValueSource(CXTPMarkupDependencyProperty* pProperty) const;

	virtual CXTPMarkupObject* GetValueCore(CXTPMarkupDependencyProperty* pProperty) const;

	CXTPMarkupDependencyProperty* GetAssociatedProperty() const;
	void SetAssociatedProperty(CXTPMarkupDependencyProperty* pAssociatedProperty);

	void SetLogicalParent(CXTPMarkupObject* pObject);
	CXTPMarkupObject* GetLogicalParent() const;

	void AddExtension(CXTPMarkupExtension* pExtension);
	CXTPMarkupExtension* GetNextExtension(const CXTPMarkupType* pClass = NULL);
	const CXTPMarkupExtension* GetNextExtension(const CXTPMarkupType* pClass = NULL) const;
	void ResetExtensionEnumerator();

	BOOL IsKindOf(const CXTPMarkupType* pClass) const;

public:
	DWORD AddRef();
	DWORD Release();

public:
	virtual UINT GetHashKey() const;
	virtual BOOL IsEqual(const CXTPMarkupObject* pObject) const;

	BOOL operator==(const CXTPMarkupObject* pObject) const;

public:
	void SetTagName(LPCWSTR lpszTagName);
	LPCWSTR GetTagName() const;

	CXTPMarkupObject* FindKey(LPCWSTR lpszKey);
	CXTPMarkupObject* FindName(LPCWSTR lpszName);

	virtual int GetLogicalChildrenCount() const;
	virtual CXTPMarkupObject* GetLogicalChild(int nIndex) const;

public:
	virtual void SetContentObject(CXTPMarkupBuilder* pBuilder, CXTPMarkupObject* pContent);
	virtual void SetPropertyObject(CXTPMarkupBuilder* pBuilder,
								   CXTPMarkupDependencyProperty* pProperty,
								   CXTPMarkupObject* pValue);
	virtual CXTPMarkupObject* ConvertFrom(CXTPMarkupBuilder* pBuilder,
										  CXTPMarkupObject* pObject) const;
	virtual BOOL HasContentObject() const;
	virtual BOOL AllowWhiteSpaceContent() const;

protected:
	virtual void OnPropertyChanged(CXTPMarkupDependencyProperty* pProperty,
								   CXTPMarkupObject* pOldValue, CXTPMarkupObject* pNewValue);
	virtual void OnSetAsProperty(CXTPMarkupDependencyProperty* pProperty,
								 CXTPMarkupObject* pTargetObject);
	virtual void OnLoaded(CXTPMarkupBuilder* pBuilder);

	void RecursePropertyChanged(CXTPMarkupDependencyProperty* pProperty,
								CXTPMarkupObject* pOldValue, CXTPMarkupObject* pNewValue);

protected:
	virtual void OnFinalRelease();

	//{{AFX_CODEJOCK_PRIVATE
public:
	DECLARE_DISPATCH_MAP()
#	ifdef _XTP_ACTIVEX
	DECLARE_INTERFACE_MAP()
	DECLARE_OLETYPELIB_EX(CXTPMarkupObject);
#	endif
	CXTPMarkupObject* FromDispatch(LPDISPATCH lpElementDisp, BOOL bAddRef = TRUE);

	void SetBeingLoaded(BOOL bBeingLoaded = TRUE);
	BOOL IsBeingLoaded() const;

	//}}AFX_CODEJOCK_PRIVATE

protected:
	CXTPComInitializer* m_pComInitializer;

	CXTPMarkupObject* m_pLogicalParent;

	CXTPMarkupProperties* m_pProperties;
	CXTPMarkupDependencyProperty* m_pAssociatedProperty;

	CList<CXTPMarkupExtension*, CXTPMarkupExtension*> m_Extensions;
	POSITION m_posExtension;

	CXTPMarkupContext* m_pMarkupContext;

	XTPXML::IXMLDOMElement* m_pMarkupElement;
	LPWSTR m_lpszMarkupTag;
	long m_dwRef;

private:
	BOOL m_bBeingLoaded;

protected:
	friend class CXTPMarkupBuilder;
	friend class CXTPMarkupStyle;
	static CXTPMarkupDependencyProperty* m_pNameProperty;
	static CXTPMarkupDependencyProperty* m_pKeyProperty;

	friend class CXTPMarkupProperties;
};

AFX_INLINE CXTPMarkupDependencyProperty* CXTPMarkupObject::GetAssociatedProperty() const
{
	return m_pAssociatedProperty;
}

AFX_INLINE void
	CXTPMarkupObject::SetAssociatedProperty(CXTPMarkupDependencyProperty* pAssociatedProperty)
{
	m_pAssociatedProperty = pAssociatedProperty;
}

AFX_INLINE void CXTPMarkupObject::SetBeingLoaded(BOOL bBeingLoaded /*= TRUE*/)
{
	m_bBeingLoaded = bBeingLoaded;
}

AFX_INLINE BOOL CXTPMarkupObject::IsBeingLoaded() const
{
	return m_bBeingLoaded;
}

typedef CXTPMarkupObject* CXTPMarkupObjectPtr;

class _XTP_EXT_CLASS CXTPMarkupDependencyProperty : public CXTPMarkupObject
{
	DECLARE_MARKUPCLASS(CXTPMarkupDependencyProperty);

protected:
	CXTPMarkupDependencyProperty();

public:
	virtual ~CXTPMarkupDependencyProperty();

public:
	static CXTPMarkupDependencyProperty* AFX_CDECL
		Register(LPCWSTR lpszName, CXTPMarkupType* pPropetyType, CXTPMarkupType* pOwnerType,
				 CXTPMarkupPropertyMetadata* pMetadata = NULL);
	static CXTPMarkupDependencyProperty* AFX_CDECL
		RegisterAttached(LPCWSTR lpszName, CXTPMarkupType* pPropetyType, CXTPMarkupType* pOwnerType,
						 CXTPMarkupPropertyMetadata* pMetadata = NULL);
	CXTPMarkupDependencyProperty* AddOwner(CXTPMarkupType* pOwnerType);

public:
	LPCWSTR GetName() const;
	CXTPMarkupType* GetPropetyType() const;
	CXTPMarkupType* GetOwnerType() const;
	CXTPMarkupPropertyMetadata* GetMetadata() const;
	BOOL IsAttached() const;
	virtual BOOL IsEvent() const;

	DWORD GetFlags() const;

public:
	static CXTPMarkupDependencyProperty* AFX_CDECL FindProperty(CXTPMarkupType* pRuntimeClass,
																LPCWSTR lpszAttribute);

protected:
	static CXTPMarkupPropertyFromNameMap* AFX_CDECL GetPropertyMap();
	static CXTPMarkupDependencyProperty* AFX_CDECL RegisterCommon(CXTPMarkupDependencyProperty* dp,
																  LPCWSTR lpszName,
																  CXTPMarkupType* pPropetyType,
																  CXTPMarkupType* pOwnerType,
																  BOOL bAttached);

private:
	LPCWSTR m_lpszName;
	CXTPMarkupType* m_pPropetyType;
	CXTPMarkupType* m_pOwnerType;
	CXTPMarkupPropertyMetadata* m_pMetadata;
	BOOL m_bAttached;
	int m_nIndex;

	static int s_nCount;

	friend class CXTPMarkupBuilder;
	friend class CXTPMarkupRoutedEvent;
	friend class CXTPMarkupProperties;
};

class _XTP_EXT_CLASS CXTPMarkupPropertyChangedEventArgs
{
public:
	CXTPMarkupPropertyChangedEventArgs(CXTPMarkupDependencyProperty* pProperty,
									   CXTPMarkupObject* pOldValue, CXTPMarkupObject* pNewValue);

public:
	CXTPMarkupDependencyProperty* m_pProperty;
	CXTPMarkupObject* m_pOldValue;
	CXTPMarkupObject* m_pNewValue;
};

class _XTP_EXT_CLASS CXTPMarkupInt : public CXTPMarkupObject
{
	DECLARE_MARKUPCLASS(CXTPMarkupInt);

public:
	CXTPMarkupInt(int nValue = 0);

	operator int() const
	{
		return m_nValue;
	}

	int GetValue() const
	{
		return m_nValue;
	}

public:
	CXTPMarkupObject* ConvertFrom(CXTPMarkupBuilder* pBuilder, CXTPMarkupObject* pObject) const;

public:
	virtual BOOL IsEqual(const CXTPMarkupObject* pObject) const;

protected:
	int m_nValue;
};

class _XTP_EXT_CLASS CXTPMarkupDouble : public CXTPMarkupObject
{
	DECLARE_MARKUPCLASS(CXTPMarkupDouble);

public:
	CXTPMarkupDouble(double dValue = 0);

	operator double() const
	{
		return m_dValue;
	}

public:
	CXTPMarkupObject* ConvertFrom(CXTPMarkupBuilder* pBuilder, CXTPMarkupObject* pObject) const;

public:
	virtual BOOL IsEqual(const CXTPMarkupObject* pObject) const;

protected:
	double m_dValue;
};

class _XTP_EXT_CLASS CXTPMarkupEnum : public CXTPMarkupObject
{
	DECLARE_MARKUPCLASS(CXTPMarkupEnum);

	struct EnumMapTag
	{
	};

public:
	CXTPMarkupEnum(int nValue = 0);

	operator int() const
	{
		return m_nValue;
	}

	int GetValue() const
	{
		return m_nValue;
	}

public:
	static CXTPMarkupEnum* AFX_CDECL CreateValue(int nValue);

public:
	CXTPMarkupObject* ConvertFrom(CXTPMarkupBuilder* pBuilder, CXTPMarkupObject* pObject) const;

public:
	virtual BOOL IsEqual(const CXTPMarkupObject* pObject) const;

protected:
	int m_nValue;
};

class _XTP_EXT_CLASS CXTPMarkupBool : public CXTPMarkupObject
{
	DECLARE_MARKUPCLASS(CXTPMarkupBool);

public:
	CXTPMarkupBool(BOOL bValue = FALSE);

	operator BOOL() const
	{
		return m_bValue;
	}

public:
	static CXTPMarkupBool* AFX_CDECL CreateTrueValue();
	static CXTPMarkupBool* AFX_CDECL CreateFalseValue();
	static CXTPMarkupBool* AFX_CDECL CreateValue(BOOL bValue);

public:
	CXTPMarkupObject* ConvertFrom(CXTPMarkupBuilder* pBuilder, CXTPMarkupObject* pObject) const;

public:
	virtual UINT GetHashKey() const;
	virtual BOOL IsEqual(const CXTPMarkupObject* pObject) const;

protected:
	BOOL m_bValue;
};

class _XTP_EXT_CLASS CXTPMarkupColor : public CXTPMarkupObject
{
	DECLARE_MARKUPCLASS(CXTPMarkupColor);

public:
	CXTPMarkupColor(COLORREF nValue = 0);
	CXTPMarkupColor(BYTE bAlpha, COLORREF nValue);

	operator COLORREF() const;

	COLORREF GetCOLORREF() const;
	DWORD GetARGB() const;
	BYTE GetAlpha() const;

	CXTPMarkupObject* ConvertFrom(CXTPMarkupBuilder* pBuilder, CXTPMarkupObject* pObject) const;

public:
	static BOOL AFX_CDECL ConvertFromString(LPCWSTR lpszValue, COLORREF& clr);

public:
	virtual BOOL IsEqual(const CXTPMarkupObject* pObject) const;

protected:
	void SetContentObject(CXTPMarkupBuilder* pBuilder, CXTPMarkupObject* /*pContent*/);

protected:
	COLORREF m_nValue;
};

AFX_INLINE CXTPMarkupColor::operator COLORREF() const
{
	return m_nValue;
}

AFX_INLINE COLORREF CXTPMarkupColor::GetCOLORREF() const
{
	return m_nValue & 0xFFFFFF;
}

AFX_INLINE DWORD CXTPMarkupColor::GetARGB() const
{
	return (m_nValue & 0xFF000000)
		   | RGB(GetBValue(m_nValue), GetGValue(m_nValue), GetRValue(m_nValue));
}

AFX_INLINE BYTE CXTPMarkupColor::GetAlpha() const
{
	return static_cast<BYTE>(m_nValue >> 24);
}

class _XTP_EXT_CLASS CXTPMarkupColorKey : public CXTPMarkupObject
{
	DECLARE_MARKUPCLASS(CXTPMarkupColorKey);

public:
	CXTPMarkupColorKey(int nIndex = 0)
		: m_nIndex(nIndex)
	{
	}

public:
	int m_nIndex;
};

class _XTP_EXT_CLASS CXTPMarkupType : public CXTPMarkupObject
{
	DECLARE_MARKUPCLASS(CXTPMarkupType);

protected:
	class CClassList;
	static CClassList* AFX_CDECL GetClassList();

public:
	typedef CXTPMarkupObject*(AFX_CDECL* PFNCREATEOBJECT)(CXTPMarkupContext*);
	typedef CXTPMarkupType*(AFX_CDECL* PFNGETBASETYPE)();
	typedef void(AFX_CDECL* PFNREGISTERMARKUPCLASS)();

	CXTPMarkupObject* CreateObject(CXTPMarkupContext* pContext) const;

	static void AFX_CDECL RegisterAll();

	//{{AFX_CODEJOCK_PRIVATE
	static void* AFX_CDECL RegisterTypeEntry(LPCWSTR lpTag, PFNGETBASETYPE pfnGetType);
	static void AFX_CDECL ResolveTypeEntry(void* pTypeEntry);
	//}}AFX_CODEJOCK_PRIVATE

public:
	CXTPMarkupType();
	CXTPMarkupType(LPCWSTR lpszTag, LPCWSTR lpszClassName, PFNCREATEOBJECT pfnCreateObject,
				   PFNGETBASETYPE pfnGetBaseType, PFNREGISTERMARKUPCLASS pfnRegisterMarkupClass);

private:
	virtual ~CXTPMarkupType();

public:
	BOOL IsDerivedFrom(const CXTPMarkupType* pBaseClass) const;
	static CXTPMarkupObject* AFX_CDECL DynamicDownCast(CXTPMarkupType* pType,
													   CXTPMarkupObject* pObject);
	static CXTPMarkupObject* AFX_CDECL StaticDownCast(CXTPMarkupType* pType,
													  CXTPMarkupObject* pObject);

	static CXTPMarkupType* AFX_CDECL LookupTag(LPCWSTR lpszTag);

	void Register();

	CXTPMarkupStyle* GetTypeStyle() const;
	void SetTypeStyle(CXTPMarkupStyle* pStyle);

	CXTPMarkupType* GetBaseType() const;

protected:
	CXTPMarkupObject* ConvertFrom(CXTPMarkupBuilder* pBuilder, CXTPMarkupObject* pObject) const;

public:
	LPCWSTR m_lpszTag;
	LPCWSTR m_lpszClassName;

	_XTP_DEPRECATED
	CXTPMarkupType* m_pNextType;

protected:
	BOOL m_bRegister;
	PFNCREATEOBJECT m_pfnCreateObject;
	PFNGETBASETYPE m_pfnGetBaseType;
	PFNREGISTERMARKUPCLASS m_pfnRegisterMarkupClass;
	CXTPMarkupStyle* m_pTypeStyle;
};

class _XTP_EXT_CLASS CXTPMarkupCollection : public CXTPMarkupObject
{
	DECLARE_MARKUPCLASS(CXTPMarkupCollection);

public:
	CXTPMarkupCollection();
	virtual ~CXTPMarkupCollection();

public:
	int GetCount() const;
	void Add(CXTPMarkupObject* pElement);
	void Insert(int nIndex, CXTPMarkupObject* pElement);
	void RemoveAll();
	void Remove(int nIndex);
	int IndexOf(CXTPMarkupObject* pElement) const;

public:
	void SetContentObject(CXTPMarkupBuilder* pBuilder, CXTPMarkupObject* pContent);
	BOOL HasContentObject() const;

	CXTPMarkupType* GetElementType() const;

	virtual int GetLogicalChildrenCount() const;
	virtual CXTPMarkupObject* GetLogicalChild(int nIndex) const;

protected:
	virtual void OnItemAdded(CXTPMarkupObject* pItem, int nIndex);
	virtual void OnChanged();

protected:
	CArray<CXTPMarkupObject*, CXTPMarkupObject*> m_arrItems;
	CXTPMarkupType* m_pElementType;
	BOOL m_bLogicalParent;
};

class _XTP_EXT_CLASS CXTPMarkupProperties : public CXTPMarkupObject
{
public:
	CXTPMarkupProperties(CXTPMarkupObject* pOwner);
	virtual ~CXTPMarkupProperties();

public:
	BOOL IsPropertyValid(CXTPMarkupDependencyProperty* pProperty) const;

	CXTPMarkupObject* Lookup(CXTPMarkupDependencyProperty* pProperty) const;

	void Copy(CXTPMarkupProperties* pOwner);

	void Set(CXTPMarkupDependencyProperty* pProperty, CXTPMarkupObject* pValue);

	void Remove(CXTPMarkupDependencyProperty* pProperty);

protected:
	CXTPMarkupObject** m_pTable;
	int m_nTableSize;
	CXTPMarkupObject* m_pOwner;
};

class _XTP_EXT_CLASS CXTPMarkupAutoPtr
{
public:
	CXTPMarkupAutoPtr(CXTPMarkupObject* pObject = NULL);
	virtual ~CXTPMarkupAutoPtr();

public:
	void Assign(CXTPMarkupObject* pObject);
	CXTPMarkupObject* AddRef();
	CXTPMarkupObject* operator->();
	const CXTPMarkupObject* operator->() const;

public:
	CXTPMarkupObject* m_pObject;
};

class _XTP_EXT_CLASS CXTPMarkupDoubleCollection : public CXTPMarkupObject
{
	DECLARE_MARKUPCLASS(CXTPMarkupDoubleCollection)
public:
	typedef CArray<float, float&> CDoubleArray;

public:
	CXTPMarkupDoubleCollection();
	CXTPMarkupDoubleCollection(CDoubleArray& arr);

public:
	int GetCount() const;
	const float* GetData() const;
	float GetAt(int nIndex) const;
	void RemoveAll();
	void Remove(int nIndex);
	void Add(float fValue);

protected:
	CXTPMarkupObject* ConvertFrom(CXTPMarkupBuilder* pBuilder, CXTPMarkupObject* pObject) const;
	static BOOL AFX_CDECL ConvertFromString(LPCWSTR lpszValue, CDoubleArray& arr);
	static BOOL AFX_CDECL GetNextValue(LPCWSTR& lpszValue, float& nValue);

protected:
	CDoubleArray m_arr;

	//{{AFX_CODEJOCK_PRIVATE
public:
	DECLARE_DISPATCH_MAP()
#	ifdef _XTP_ACTIVEX
	DECLARE_INTERFACE_MAP()
	DECLARE_OLETYPELIB_EX(CXTPMarkupDoubleCollection);
#	endif

	afx_msg long OleGetItemCount();
	afx_msg void OleAdd(double dValue);
	afx_msg double OleGetItem(long nIndex);
	//}}AFX_CODEJOCK_PRIVATE
};

AFX_INLINE CXTPMarkupStyle* CXTPMarkupType::GetTypeStyle() const
{
	return m_pTypeStyle;
}
AFX_INLINE int CXTPMarkupCollection::GetCount() const
{
	return (int)m_arrItems.GetSize();
}
AFX_INLINE CXTPMarkupType* CXTPMarkupCollection::GetElementType() const
{
	return m_pElementType;
}
AFX_INLINE LPCWSTR CXTPMarkupDependencyProperty::GetName() const
{
	return m_lpszName;
}
AFX_INLINE CXTPMarkupType* CXTPMarkupDependencyProperty::GetPropetyType() const
{
	return m_pPropetyType;
}
AFX_INLINE CXTPMarkupType* CXTPMarkupDependencyProperty::GetOwnerType() const
{
	return m_pOwnerType;
}
AFX_INLINE CXTPMarkupPropertyMetadata* CXTPMarkupDependencyProperty::GetMetadata() const
{
	return m_pMetadata;
}
AFX_INLINE BOOL CXTPMarkupDependencyProperty::IsAttached() const
{
	return m_bAttached;
}
AFX_INLINE BOOL CXTPMarkupDependencyProperty::IsEvent() const
{
	return FALSE;
}
AFX_INLINE DWORD CXTPMarkupDependencyProperty::GetFlags() const
{
	return m_pMetadata ? m_pMetadata->m_dwFlags : 0;
}
AFX_INLINE CXTPMarkupContext* CXTPMarkupObject::GetMarkupContext() const
{
	return m_pMarkupContext;
}
AFX_INLINE UINT CXTPMarkupObject::GetHashKey() const
{
	return (UINT)(UINT_PTR)this;
}
AFX_INLINE BOOL CXTPMarkupObject::IsEqual(const CXTPMarkupObject* pObject) const
{
	return pObject == this;
}
AFX_INLINE BOOL CXTPMarkupObject::operator==(const CXTPMarkupObject* pObject) const
{
	return IsEqual(pObject);
}
AFX_INLINE bool AFXAPI operator==(const CXTPMarkupObject& s1, const CXTPMarkupObject& s2)
{
	return s1.IsEqual(&s2) ? true : false;
}
template<>
AFX_INLINE UINT AFXAPI HashKey(CXTPMarkupObject* key)
{
	ASSERT(NULL != key);

	return key->GetHashKey();
}
template<>
AFX_INLINE BOOL AFXAPI CompareElements(const CXTPMarkupObjectPtr* pElement1,
									   const CXTPMarkupObjectPtr* pElement2)
{
	ASSERT(NULL != pElement1);
	ASSERT(NULL != *pElement1);
	ASSERT(NULL != pElement2);

	return (*pElement1)->IsEqual(*pElement2);
}

AFX_INLINE BOOL IsEqual(CXTPMarkupObject* pNewValue, CXTPMarkupObject* pOldValue)
{
	if (pNewValue == NULL && pOldValue == NULL)
		return TRUE;

	if (pNewValue != NULL && pOldValue != NULL)
		return pNewValue->IsEqual(pOldValue);

	return FALSE;
}

#	include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPMARKUPOBJECT_H__)
