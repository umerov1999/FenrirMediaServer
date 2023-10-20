// XTPMarkupObject.cpp: implementation of the CXTPMarkupObject class.
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

#include "stdafx.h"

#include "Common/XTPTypeId.h"
#include "Common/XTPCasting.h"
#include "Common/XTPFramework.h"
#include "Common/XTPVc80Helpers.h"
#include "Common/XTPSystemHelpers.h"
#include "Common/XTPSynchro.h"
#include "Common/XTPApplication.h"
#include "Common/XTPSingleton.h"
#include "Common/XTPXMLHelpers.h"

#include "Markup/XTPMarkupObject.h"
#include "Markup/XTPMarkupContext.h"
#include "Markup/XTPMarkupString.h"
#include "Markup/XTPMarkupBuilder.h"
#include "Markup/Extensions/XTPMarkupExtension.h"
#include "Markup/XTPMarkupIIDs.h"

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

#ifdef _DEBUG
#	undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#	define new DEBUG_NEW
#endif

template<>
inline BOOL AFXAPI CompareElements(const LPCWSTR* pElement1, const LPCWSTR* pElement2)
{
	return wcscmp(*pElement1, *pElement2) == 0;
}

CXTPMarkupPropertyChangedEventArgs::CXTPMarkupPropertyChangedEventArgs(
	CXTPMarkupDependencyProperty* pProperty, CXTPMarkupObject* pOldValue,
	CXTPMarkupObject* pNewValue)
{
	m_pProperty = pProperty;
	m_pOldValue = pOldValue;
	m_pNewValue = pNewValue;
}

struct CXTPMarkupFromNameKey
{
public:
	CXTPMarkupFromNameKey(LPCWSTR lpszName = 0, CXTPMarkupType* pOwnerType = 0)
	{
		m_lpszName   = lpszName;
		m_pOwnerType = pOwnerType;
	}

	BOOL operator==(const CXTPMarkupFromNameKey& key) const
	{
		return m_pOwnerType == key.m_pOwnerType
			   && (m_lpszName == key.m_lpszName || wcscmp(m_lpszName, key.m_lpszName) == 0);
	}

	LPCWSTR m_lpszName;
	CXTPMarkupType* m_pOwnerType;
};

class CXTPMarkupPropertyFromNameMap
	: public CMap<CXTPMarkupFromNameKey, CXTPMarkupFromNameKey&, CXTPMarkupDependencyProperty*,
				  CXTPMarkupDependencyProperty*>
{
public:
	CXTPMarkupPropertyFromNameMap();
	~CXTPMarkupPropertyFromNameMap();
};

CXTPMarkupPropertyFromNameMap::CXTPMarkupPropertyFromNameMap()
{
}

CXTPMarkupPropertyFromNameMap::~CXTPMarkupPropertyFromNameMap()
{
	POSITION pos = GetStartPosition();
	while (pos)
	{
		CXTPMarkupFromNameKey key;
		CXTPMarkupDependencyProperty* dp;
		GetNextAssoc(pos, key, dp);
		dp->Release();
	}
	RemoveAll();
}

template<>
AFX_INLINE UINT AFXAPI HashKey<CXTPMarkupFromNameKey&>(CXTPMarkupFromNameKey& key)
{
	UINT nameHash = HashKey(key.m_lpszName);
	UINT typeHash = HashKey(key.m_pOwnerType);
	return (nameHash * typeHash) + (nameHash ^ typeHash);
}

CXTPMarkupPropertyFromNameMap* CXTPMarkupDependencyProperty::GetPropertyMap()
{
	return &CXTPSingleton<CXTPMarkupPropertyFromNameMap>::Instance();
}

//////////////////////////////////////////////////////////////////////////
// CXTPMarkupPropertyMetadata
CXTPMarkupPropertyMetadata::CXTPMarkupPropertyMetadata(CXTPMarkupObject* pDefaultValue,
													   DWORD dwFlags)
{
	m_pDefaultValue	= pDefaultValue;
	m_pConverter	   = NULL;
	m_dwFlags		   = dwFlags;
	m_pPropertyChanged = NULL;
}

CXTPMarkupPropertyMetadata::CXTPMarkupPropertyMetadata(CXTPMarkupObject* pDefaultValue,
													   PFNCONVERTFROM pConverter, DWORD dwFlags)
{
	m_pDefaultValue	= pDefaultValue;
	m_pConverter	   = pConverter;
	m_dwFlags		   = dwFlags;
	m_pPropertyChanged = NULL;
}

CXTPMarkupPropertyMetadata::CXTPMarkupPropertyMetadata(CXTPMarkupObject* pDefaultValue,
													   PFNPROPERTYCHANGED pPropertyChanged,
													   DWORD dwFlags)
{
	m_pDefaultValue	= pDefaultValue;
	m_pConverter	   = NULL;
	m_dwFlags		   = dwFlags;
	m_pPropertyChanged = pPropertyChanged;
}

CXTPMarkupPropertyMetadata::CXTPMarkupPropertyMetadata(CXTPMarkupObject* pDefaultValue,
													   PFNCONVERTFROM pConverter,
													   PFNPROPERTYCHANGED pPropertyChanged,
													   DWORD dwFlags)
{
	m_pDefaultValue	= pDefaultValue;
	m_pConverter	   = pConverter;
	m_dwFlags		   = dwFlags;
	m_pPropertyChanged = pPropertyChanged;
}

CXTPMarkupPropertyMetadata::~CXTPMarkupPropertyMetadata()
{
	if (m_pDefaultValue)
	{
		MARKUP_RELEASE(m_pDefaultValue);
	}
}

//////////////////////////////////////////////////////////////////////////
// CXTPMarkupDependencyProperty

int CXTPMarkupDependencyProperty::s_nCount = 0;

IMPLEMENT_MARKUPCLASS(NULL, CXTPMarkupDependencyProperty, CXTPMarkupObject);

void CXTPMarkupDependencyProperty::RegisterMarkupClass()
{
}

CXTPMarkupDependencyProperty::CXTPMarkupDependencyProperty()
	: CXTPMarkupObject(flagNoCom)
{
	m_lpszName	 = 0;
	m_pPropetyType = 0;
	m_pOwnerType   = 0;
	m_bAttached	= FALSE;
	m_pMetadata	= NULL;
	m_nIndex	   = -1;
}

CXTPMarkupDependencyProperty::~CXTPMarkupDependencyProperty()
{
	SAFE_DELETE(m_pMetadata);
}

CXTPMarkupDependencyProperty* AFX_CDECL CXTPMarkupDependencyProperty::RegisterCommon(
	CXTPMarkupDependencyProperty* dp, LPCWSTR lpszName, CXTPMarkupType* pPropetyType,
	CXTPMarkupType* pOwnerType, BOOL bAttached)
{
	CXTPMarkupFromNameKey key(lpszName, pOwnerType);

	dp->m_lpszName	 = lpszName;
	dp->m_pPropetyType = pPropetyType;
	dp->m_pOwnerType   = pOwnerType;
	dp->m_bAttached	= bAttached;

	CXTPMarkupPropertyFromNameMap* pPropertyFromNameMap = GetPropertyMap();

	if (!dp->IsEvent())
	{
		dp->m_nIndex = s_nCount++;
	}

	pPropertyFromNameMap->SetAt(key, dp);

	return dp;
}

CXTPMarkupDependencyProperty* AFX_CDECL CXTPMarkupDependencyProperty::RegisterAttached(
	LPCWSTR lpszName, CXTPMarkupType* pPropetyType, CXTPMarkupType* pOwnerType,
	CXTPMarkupPropertyMetadata* pMetadata)
{
	CXTPMarkupDependencyProperty* dp = RegisterCommon(new CXTPMarkupDependencyProperty(), lpszName,
													  pPropetyType, pOwnerType, TRUE);
	dp->m_pMetadata					 = pMetadata;
	return dp;
}

CXTPMarkupDependencyProperty* AFX_CDECL CXTPMarkupDependencyProperty::Register(
	LPCWSTR lpszName, CXTPMarkupType* pPropetyType, CXTPMarkupType* pOwnerType,
	CXTPMarkupPropertyMetadata* pMetadata)
{
	CXTPMarkupDependencyProperty* dp = RegisterCommon(new CXTPMarkupDependencyProperty(), lpszName,
													  pPropetyType, pOwnerType, FALSE);
	dp->m_pMetadata					 = pMetadata;
	return dp;
}

CXTPMarkupDependencyProperty* CXTPMarkupDependencyProperty::AddOwner(CXTPMarkupType* pOwnerType)
{
	CXTPMarkupFromNameKey key(m_lpszName, pOwnerType);

	CXTPMarkupPropertyFromNameMap* pPropertyFromNameMap = GetPropertyMap();
	pPropertyFromNameMap->SetAt(key, this);

	AddRef();

	return this;
}

CXTPMarkupDependencyProperty*
	CXTPMarkupDependencyProperty::FindProperty(CXTPMarkupType* pRuntimeClass, LPCWSTR lpszAttribute)
{
	if (!lpszAttribute)
		return NULL;

	if (lpszAttribute[0] == 'x' && lpszAttribute[1] == ':')
		lpszAttribute = lpszAttribute + 2;

	CXTPMarkupPropertyFromNameMap* pMap = CXTPMarkupDependencyProperty::GetPropertyMap();

	while (pRuntimeClass != NULL)
	{
		CXTPMarkupFromNameKey nameKey(lpszAttribute, pRuntimeClass);
		CXTPMarkupDependencyProperty* pProperty = NULL;

		if (pMap->Lookup(nameKey, pProperty))
			return pProperty;

		pRuntimeClass = pRuntimeClass->GetBaseType();
	}

	return NULL;
}

//////////////////////////////////////////////////////////////////////////
// CXTPMarkupType

class CXTPMarkupType::CClassList
{
public:
	~CClassList();

public:
	typedef CXTPMarkupType*(AFX_CDECL* PFNGetType)();
	class CTypeEntry
	{
	public:
		CTypeEntry();

		void SetGetType(PFNGetType pfnGetType);
		CXTPMarkupType* GetType() const;

	private:
		PFNGetType m_pfnGetType;
		mutable CXTPMarkupType* m_pType;
	};

	CTypeEntry* Register(LPCWSTR lpszTag, PFNGetType pfnGetType);
	CXTPMarkupType* Lookup(LPCWSTR lpszTag);
	void RegisterAll();

private:
	CMap<LPCWSTR, LPCWSTR, CTypeEntry, CTypeEntry> m_namedTypes;
	CList<CTypeEntry, CTypeEntry> m_unnamedTypes;

	friend class CXTPMarkupType;
};

CXTPMarkupType::CClassList::CTypeEntry::CTypeEntry()
	: m_pfnGetType(NULL)
	, m_pType(NULL)
{
}

void CXTPMarkupType::CClassList::CTypeEntry::SetGetType(PFNGetType pfnGetType)
{
	ASSERT(NULL != pfnGetType);
	ASSERT(NULL == m_pfnGetType);

	m_pfnGetType = pfnGetType;
}

CXTPMarkupType* CXTPMarkupType::CClassList::CTypeEntry::GetType() const
{
	if (NULL == m_pType)
	{
		ASSERT(NULL != m_pfnGetType);
		m_pType = m_pfnGetType();
	}

	return m_pType;
}

CXTPMarkupType::CClassList::~CClassList()
{
	POSITION pos = m_namedTypes.GetStartPosition();
	while (NULL != pos)
	{
		LPCWSTR key = NULL;
		CClassList::CTypeEntry typeEntry;
		m_namedTypes.GetNextAssoc(pos, key, typeEntry);

		CXTPMarkupType* pType = typeEntry.GetType();
		MARKUP_RELEASE(pType);
	}

	pos = m_unnamedTypes.GetHeadPosition();
	while (NULL != pos)
	{
		CXTPMarkupType* pType = m_unnamedTypes.GetNext(pos).GetType();
		MARKUP_RELEASE(pType);
	}
}

CXTPMarkupType::CClassList::CTypeEntry* CXTPMarkupType::CClassList::Register(LPCWSTR lpszTag,
																			 PFNGetType pfnGetType)
{
	CTypeEntry* pTypeEntry = NULL;

	CTypeEntry typeEntry;
	typeEntry.SetGetType(pfnGetType);
	if (NULL != lpszTag && L'\0' != lpszTag[0])
	{
		ASSERT(Lookup(lpszTag) == NULL);
		m_namedTypes.SetAt(lpszTag, typeEntry);
	}
	else
	{
		m_unnamedTypes.AddTail(typeEntry);
	}

	return pTypeEntry;
}

void CXTPMarkupType::CClassList::RegisterAll()
{
	POSITION pos = m_namedTypes.GetStartPosition();
	while (NULL != pos)
	{
		LPCWSTR key = NULL;
		CClassList::CTypeEntry typeEntry;
		m_namedTypes.GetNextAssoc(pos, key, typeEntry);
		typeEntry.GetType()->Register();
	}

	pos = m_unnamedTypes.GetHeadPosition();
	while (NULL != pos)
	{
		m_unnamedTypes.GetNext(pos).GetType()->Register();
	}
}

CXTPMarkupType* CXTPMarkupType::CClassList::Lookup(LPCWSTR lpszTag)
{
	ASSERT(NULL != lpszTag);

	CClassList::CTypeEntry typeEntry;
	if (m_namedTypes.Lookup(lpszTag, typeEntry))
	{
		return typeEntry.GetType();
	}

	return NULL;
}

CXTPMarkupType::CClassList* AFX_CDECL CXTPMarkupType::GetClassList()
{
	return &CXTPSingleton<CClassList>::Instance();
}

CXTPMarkupType* CXTPMarkupType::LookupTag(LPCWSTR lpszTag)
{
	return GetClassList()->Lookup(lpszTag);
}

IMPLEMENT_MARKUPCLASS(NULL, CXTPMarkupType, CXTPMarkupObject)

void CXTPMarkupType::RegisterMarkupClass()
{
}

CXTPMarkupType::CXTPMarkupType()
	: CXTPMarkupObject(CXTPMarkupObject::flagNoCom)
{
	m_lpszTag				 = NULL;
	m_lpszClassName			 = NULL;
	m_pfnCreateObject		 = NULL;
	m_pfnGetBaseType		 = NULL;
	m_pfnRegisterMarkupClass = NULL;
	m_bRegister				 = FALSE;
	m_pTypeStyle			 = NULL;
}

CXTPMarkupType::CXTPMarkupType(LPCWSTR lpszTag, LPCWSTR lpszClassName,
							   PFNCREATEOBJECT pfnCreateObject, PFNGETBASETYPE pfnGetBaseType,
							   PFNREGISTERMARKUPCLASS pfnRegisterMarkupClass)
	: CXTPMarkupObject(CXTPMarkupObject::flagNoCom)
{
	m_lpszTag				 = lpszTag;
	m_lpszClassName			 = lpszClassName;
	m_pfnCreateObject		 = pfnCreateObject;
	m_pfnGetBaseType		 = pfnGetBaseType;
	m_pfnRegisterMarkupClass = pfnRegisterMarkupClass;
	m_bRegister				 = FALSE;
	m_pTypeStyle			 = NULL;
}

CXTPMarkupObject* CXTPMarkupType::ConvertFrom(CXTPMarkupBuilder* /*pBuilder*/,
											  CXTPMarkupObject* pObject) const
{
	if (IsStringObject(pObject))
	{
		LPCWSTR lpszValue = *((CXTPMarkupString*)pObject);

		CXTPMarkupType* pType = GetClassList()->Lookup(lpszValue);
		if (pType)
		{
			pType->AddRef();
			return pType;
		}
	}

	return NULL;
}

CXTPMarkupType::~CXTPMarkupType()
{
	if (m_pTypeStyle)
	{
		((CXTPMarkupObject*)m_pTypeStyle)->Release();
	}
}

CXTPMarkupObject* CXTPMarkupType::CreateObject(CXTPMarkupContext* pContext) const
{
	return (*m_pfnCreateObject)(pContext);
}

CXTPMarkupType* CXTPMarkupType::GetBaseType() const
{
	return (*m_pfnGetBaseType)();
}

void CXTPMarkupType::Register()
{
	if (m_bRegister)
		return;

	CXTPMarkupType* pType = GetBaseType();
	if (pType && !pType->m_bRegister)
		pType->Register();

	m_bRegister = TRUE;
	(*m_pfnRegisterMarkupClass)();
}

void AFX_CDECL CXTPMarkupType::RegisterAll()
{
	GetClassList()->RegisterAll();
}

void* AFX_CDECL CXTPMarkupType::RegisterTypeEntry(LPCWSTR lpTag, PFNGETBASETYPE pfnGetType)
{
	return GetClassList()->Register(lpTag, pfnGetType);
}

void AFX_CDECL CXTPMarkupType::ResolveTypeEntry(void* pTypeEntry)
{
	if (NULL == pTypeEntry)
		return;

	reinterpret_cast<CXTPMarkupType::CClassList::CTypeEntry*>(pTypeEntry)->GetType();
}

BOOL CXTPMarkupType::IsDerivedFrom(const CXTPMarkupType* pBaseClass) const
{
	const CXTPMarkupType* pClassThis = this;
	while (pClassThis != NULL)
	{
		if (pClassThis == pBaseClass)
			return TRUE;

		pClassThis = pClassThis->GetBaseType();
	}
	return FALSE;
}

CXTPMarkupObject* CXTPMarkupType::DynamicDownCast(CXTPMarkupType* pType, CXTPMarkupObject* pObject)
{
	if (pObject != NULL && pObject->IsKindOf(pType))
		return pObject;
	else
		return NULL;
}

CXTPMarkupObject* CXTPMarkupType::StaticDownCast(CXTPMarkupType* pType, CXTPMarkupObject* pObject)
{
	pType;
	ASSERT(pObject == NULL || pObject->IsKindOf(pType));

	return pObject;
}

//////////////////////////////////////////////////////////////////////////
// CXTPMarkupProperties

CXTPMarkupProperties::CXTPMarkupProperties(CXTPMarkupObject* pOwner)
{
	m_pOwner	 = pOwner;
	m_nTableSize = CXTPMarkupDependencyProperty::s_nCount;
	m_pTable	 = new CXTPMarkupObject*[XTPToUIntChecked(m_nTableSize)];
	ZeroMemory(m_pTable, m_nTableSize * sizeof(CXTPMarkupObject*));
}

CXTPMarkupProperties::~CXTPMarkupProperties()
{
	for (int i = 0; i < m_nTableSize; i++)
	{
		MARKUP_RELEASE(m_pTable[i]);
	}

	delete[] m_pTable;
}

BOOL CXTPMarkupProperties::IsPropertyValid(CXTPMarkupDependencyProperty* pProperty) const
{
	if (pProperty->m_nIndex == -1)
	{
		ASSERT(FALSE);
		return FALSE;
	}
	if (pProperty->m_nIndex >= m_nTableSize)
	{
		ASSERT(FALSE);
		return FALSE;
	}
	return TRUE;
}

CXTPMarkupObject* CXTPMarkupProperties::Lookup(CXTPMarkupDependencyProperty* pProperty) const
{
	if (!IsPropertyValid(pProperty))
		return NULL;

	return m_pTable[pProperty->m_nIndex];
}

void CXTPMarkupProperties::Copy(CXTPMarkupProperties* pOwner)
{
	if (this != pOwner)
	{
		ASSERT(pOwner->m_nTableSize == m_nTableSize);

		for (int i = 0; i < pOwner->m_nTableSize; i++)
		{
			if (pOwner->m_pTable[i])
			{
				m_pTable[i] = pOwner->m_pTable[i];
				m_pTable[i]->AddRef();
			}
		}
	}
}

void CXTPMarkupProperties::Set(CXTPMarkupDependencyProperty* pProperty, CXTPMarkupObject* pValue)
{
	Remove(pProperty);

	if (NULL != pValue)
	{
		m_pTable[pProperty->m_nIndex] = pValue;
		if (NULL != m_pOwner)
		{
			pValue->SetLogicalParent(m_pOwner);
		}
	}
}

void CXTPMarkupProperties::Remove(CXTPMarkupDependencyProperty* pProperty)
{
	if (IsPropertyValid(pProperty))
	{
		int nIndex = pProperty->m_nIndex;

		if (NULL != m_pTable[nIndex])
		{
			if (NULL != m_pOwner)
			{
				m_pTable[nIndex]->SetLogicalParent(NULL);
			}

			MARKUP_RELEASE(m_pTable[nIndex]);
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// CXTPMarkupObject

IMPLEMENT_DYNAMIC(CXTPMarkupObject, CXTPCmdTarget);

CXTPMarkupDependencyProperty* CXTPMarkupObject::m_pNameProperty = NULL;
CXTPMarkupDependencyProperty* CXTPMarkupObject::m_pKeyProperty  = NULL;
void* CXTPMarkupObject::markupTypeEntryCXTPMarkupObject =
	CXTPMarkupType::RegisterTypeEntry(NULL, CXTPMarkupObject::GetCXTPMarkupObjectType);

CXTPMarkupObject* AFX_CDECL CXTPMarkupObject::CreateMarkupObject(CXTPMarkupContext* pContext)
{
	CXTPMarkupObject* pObject = new CXTPMarkupObject();
	if (NULL != pObject)
	{
		pObject->m_pMarkupContext = pContext;
	}

	return pObject;
}

CXTPMarkupType* CXTPMarkupObject::GetMarkupBaseType()
{
	return NULL;
}

static CXTPMarkupType* AFX_CDECL XTPCreateCXTPMarkupObjectType()
{
	CXTPMarkupType* pType = new CXTPMarkupType(NULL, L"CXTPMarkupObject",
											   &CXTPMarkupObject::CreateMarkupObject,
											   &CXTPMarkupObject::GetMarkupBaseType,
											   &CXTPMarkupObject::RegisterMarkupClass);
	pType->ExternalAddRef();
	return pType;
}

static void AFX_CDECL XTPDestroyCXTPMarkupObjectType(CXTPMarkupType* pType)
{
	ASSERT(NULL != pType);

	pType->ExternalRelease();
}

CXTPMarkupType* CXTPMarkupObject::GetCXTPMarkupObjectType()
{
	static CXTPThreadLocal<CXTPMarkupType> tlsType(XTPCreateCXTPMarkupObjectType,
												   XTPDestroyCXTPMarkupObjectType);
	return &*tlsType;
}

CXTPMarkupType* CXTPMarkupObject::GetType() const
{
	return MARKUP_TYPE(CXTPMarkupObject);
}

void CXTPMarkupObject::RegisterMarkupClass()
{
	CXTPMarkupType::ResolveTypeEntry(markupTypeEntryCXTPMarkupObject);

	m_pNameProperty = CXTPMarkupDependencyProperty::Register(L"Name", MARKUP_TYPE(CXTPMarkupString),
															 MARKUP_TYPE(CXTPMarkupObject));
	m_pKeyProperty  = CXTPMarkupDependencyProperty::Register(L"Key", MARKUP_TYPE(CXTPMarkupObject),
															 MARKUP_TYPE(CXTPMarkupObject));
}

CXTPMarkupObject::CXTPMarkupObject(int nFlags /*= 0*/)
	: m_pComInitializer(0 == (nFlags & flagNoCom) ? new CXTPComInitializer : NULL)
	, m_pLogicalParent(NULL)
	, m_pProperties(NULL)
	, m_pAssociatedProperty(NULL)
	, m_posExtension(NULL)
	, m_pMarkupContext(NULL)
	, m_pMarkupElement(NULL)
	, m_lpszMarkupTag(NULL)
	, m_dwRef(1)
	, m_bBeingLoaded(FALSE)
{
	if (0 == (nFlags & flagNoCom))
	{
		EnableAutomation();
#ifdef _XTP_ACTIVEX
		EnableTypeLib();
#endif
	}
}

CXTPMarkupObject::~CXTPMarkupObject()
{
	ASSERT(m_dwRef <= 1);

	if (NULL != m_pMarkupElement)
	{
		m_pMarkupElement->Release();
	}

	if (NULL != m_lpszMarkupTag)
	{
		free(m_lpszMarkupTag);
	}

	while (!m_Extensions.IsEmpty())
	{
		CXTPMarkupExtension* pExtension = m_Extensions.RemoveTail();
		MARKUP_RELEASE(pExtension);
	}

	SAFE_DELETE(m_pProperties);
	SAFE_DELETE(m_pComInitializer);
}

DWORD CXTPMarkupObject::AddRef()
{
	return InternalAddRef();
}

void CXTPMarkupObject::OnFinalRelease()
{
	if (NULL != m_pMarkupContext)
	{
		m_pMarkupContext->FinalizeMarkupObject(this);
		m_pMarkupContext = NULL;
	}

	delete this;
}

DWORD CXTPMarkupObject::Release()
{
	return InternalRelease();
}

CXTPMarkupObject* CXTPMarkupObject::ConvertFrom(CXTPMarkupBuilder* /*pBuilder*/,
												CXTPMarkupObject* /*pObject*/) const
{
	return NULL;
}

XTPXML::IXMLDOMElement* CXTPMarkupObject::GetSourceMarkupElement() const
{
	return m_pMarkupElement;
}

void CXTPMarkupObject::SetSourceMarkupElement(XTPXML::IXMLDOMElement* pElement)
{
	ASSERT(NULL != pElement);
	ASSERT(NULL == m_pMarkupElement);

	m_pMarkupElement = pElement;
	m_pMarkupElement->AddRef();
}

LPCWSTR CXTPMarkupObject::GetKey() const
{
	LPCWSTR lpszKey			 = NULL;
	CXTPMarkupObject* pValue = GetValue(m_pKeyProperty);
	if (NULL != pValue)
	{
		lpszKey = *MARKUP_STATICCAST(CXTPMarkupString, pValue);
	}

	return lpszKey;
}

void CXTPMarkupObject::SetKey(LPCWSTR lpszKey)
{
	SetValue(m_pKeyProperty, new CXTPMarkupString(lpszKey));
}

LPCWSTR CXTPMarkupObject::GetName() const
{
	LPCWSTR lpszName		 = NULL;
	CXTPMarkupObject* pValue = GetValue(m_pNameProperty);
	if (NULL != pValue)
	{
		lpszName = *MARKUP_STATICCAST(CXTPMarkupString, pValue);
	}

	return lpszName;
}

void CXTPMarkupObject::SetName(LPCWSTR lpszName)
{
	SetValue(m_pNameProperty, new CXTPMarkupString(lpszName));
}

void CXTPMarkupObject::SetValue(CXTPMarkupDependencyProperty* pProperty, CXTPMarkupObject* pValue)
{
	ASSERT_VALID(pProperty);

	if (!pProperty)
		return;

	if (pValue != NULL && !pValue->GetType()->IsDerivedFrom(pProperty->GetPropetyType()))
	{
		ASSERT(FALSE);
		return;
	}

	if (pValue && !m_pProperties)
	{
		m_pProperties = new CXTPMarkupProperties(this);
	}

	if (m_pProperties)
	{
		CXTPMarkupObject* pOldValue = GetValue(pProperty);
		BOOL bEqualValues			= ::IsEqual(pValue, pOldValue);

		m_pProperties->Set(pProperty, pValue);
		if (NULL != pValue)
		{
			pValue->OnSetAsProperty(pProperty, this);
		}

		if (!bEqualValues)
		{
			CXTPMarkupObject* pNewValue = GetValue(pProperty);

			OnPropertyChanged(pProperty, pOldValue, pNewValue);

			if (pProperty->GetFlags() & CXTPMarkupPropertyMetadata::flagInherited)
			{
				RecursePropertyChanged(pProperty, pOldValue, pNewValue);
			}
		}
	}
	else if (NULL != pValue)
	{
		MARKUP_RELEASE(pValue);
	}
}

void CXTPMarkupObject::RecursePropertyChanged(CXTPMarkupDependencyProperty* pProperty,
											  CXTPMarkupObject* pOldValue,
											  CXTPMarkupObject* pNewValue)
{
	int nCount = GetLogicalChildrenCount();

	for (int i = 0; i < nCount; i++)
	{
		CXTPMarkupObject* pObject = GetLogicalChild(i);
		if (!pObject)
			continue;

		if (pObject->IsKindOf(pProperty->GetOwnerType()))
		{
			pObject->OnPropertyChanged(pProperty, pOldValue, pNewValue);
		}

		pObject->RecursePropertyChanged(pProperty, pOldValue, pNewValue);
	}
}

void CXTPMarkupObject::OnPropertyChanged(CXTPMarkupDependencyProperty* pProperty,
										 CXTPMarkupObject* pOldValue, CXTPMarkupObject* pNewValue)
{
	if (pProperty->GetMetadata() && pProperty->GetMetadata()->m_pPropertyChanged != NULL)
	{
		CXTPMarkupPropertyChangedEventArgs e(pProperty, pOldValue, pNewValue);
		(*pProperty->GetMetadata()->m_pPropertyChanged)(this, &e);
	}
}

void CXTPMarkupObject::OnSetAsProperty(CXTPMarkupDependencyProperty* pProperty,
									   CXTPMarkupObject* pTargetObject)
{
	UNREFERENCED_PARAMETER(pProperty);
	UNREFERENCED_PARAMETER(pTargetObject);
}

void CXTPMarkupObject::OnLoaded(CXTPMarkupBuilder* pBuilder)
{
	UNREFERENCED_PARAMETER(pBuilder);
}

CXTPMarkupObject* CXTPMarkupObject::GetValueSource(CXTPMarkupDependencyProperty* pProperty) const
{
	CXTPMarkupObject* pValue = GetValueCore(pProperty);
	if (pValue)
	{
		return (CXTPMarkupObject*)this;
	}

	if (pProperty->GetFlags() & CXTPMarkupPropertyMetadata::flagInherited)
	{
		CXTPMarkupObject* pLogicalParent = GetLogicalParent();
		while (pLogicalParent)
		{
			pValue = pLogicalParent->GetValueCore(pProperty);

			if (pValue)
				return pLogicalParent;

			pLogicalParent = pLogicalParent->GetLogicalParent();
		}
	}

	return NULL;
}

CXTPMarkupObject* CXTPMarkupObject::GetValueCore(CXTPMarkupDependencyProperty* pProperty) const
{
	CXTPMarkupObject* pValue = m_pProperties ? m_pProperties->Lookup(pProperty) : NULL;

	return pValue;
}

CXTPMarkupObject* CXTPMarkupObject::GetValue(CXTPMarkupDependencyProperty* pProperty) const
{
	CXTPMarkupObject* pValue = GetValueCore(pProperty);
	if (pValue)
		return pValue;

	if (pProperty->GetFlags() & CXTPMarkupPropertyMetadata::flagInherited)
	{
		CXTPMarkupObject* pLogicalParent = GetLogicalParent();
		while (pLogicalParent)
		{
			pValue = pLogicalParent->GetValueCore(pProperty);
			if (pValue)
				return pValue;

			pLogicalParent = pLogicalParent->GetLogicalParent();
		}
	}

	if (pProperty->GetMetadata() && pProperty->GetMetadata()->m_pDefaultValue)
		return pProperty->GetMetadata()->m_pDefaultValue;

	return NULL;
}

void CXTPMarkupObject::SetLogicalParent(CXTPMarkupObject* pObject)
{
	m_pLogicalParent = pObject;
}

CXTPMarkupObject* CXTPMarkupObject::GetLogicalParent() const
{
	return m_pLogicalParent;
}

void CXTPMarkupObject::SetContentObject(CXTPMarkupBuilder* pBuilder, CXTPMarkupObject* pContent)
{
	ASSERT(NULL != pContent);

	if (IsStringObject(pContent))
	{
		pBuilder->ThrowBuilderException(
			CXTPMarkupBuilder::FormatString(_T("Cannot convert the string '%ls' into a '%ls' ")
											_T("object"),
											(LPCTSTR)(LPCWSTR) * ((CXTPMarkupString*)pContent),
											(LPCTSTR)GetType()->m_lpszClassName));
	}
	else
	{
		pBuilder->ThrowBuilderException(
			CXTPMarkupBuilder::FormatString(_T("Cannot add content to an object of type '%ls'"),
											(LPCTSTR)GetType()->m_lpszClassName));
	}
}

BOOL CXTPMarkupObject::HasContentObject() const
{
	return FALSE;
}

BOOL CXTPMarkupObject::AllowWhiteSpaceContent() const
{
	return FALSE;
}

void CXTPMarkupObject::SetPropertyObject(CXTPMarkupBuilder* pBuilder,
										 CXTPMarkupDependencyProperty* pProperty,
										 CXTPMarkupObject* pValue)
{
	ASSERT(NULL != pValue);

	if (pValue != NULL && !pValue->IsKindOf(pProperty->GetPropetyType()))
	{
		CXTPMarkupObject* pNewValue = pBuilder->ConvertValue(pProperty, pValue);
		SetValue(pProperty, pNewValue);
	}
	else
	{
		SetValue(pProperty, pValue);
		MARKUP_ADDREF(pValue);
	}
}

void CXTPMarkupObject::SetTagName(LPCWSTR lpszTagName)
{
	ASSERT(NULL != lpszTagName);
	if (NULL != m_lpszMarkupTag)
	{
		free(m_lpszMarkupTag);
		m_lpszMarkupTag = NULL;
	}

	if (NULL != lpszTagName)
	{
		m_lpszMarkupTag = _wcsdup(lpszTagName);
	}
}

LPCWSTR CXTPMarkupObject::GetTagName() const
{
	return m_lpszMarkupTag;
}

CXTPMarkupObject* CXTPMarkupObject::FindKey(LPCWSTR lpszKey)
{
	CXTPMarkupObject* pObject = NULL;

	CXTPMarkupString* pKey = MARKUP_STATICCAST(CXTPMarkupString, GetValue(m_pKeyProperty));

	if ((NULL != pKey) && (0 == ::wcscmp(*pKey, lpszKey)))
	{
		pObject = this;
	}
	else
	{
		const int nCount = GetLogicalChildrenCount();

		for (int i = 0; (i < nCount) && (NULL == pObject); i++)
		{
			pObject = GetLogicalChild(i)->FindKey(lpszKey);
		}
	}

	return pObject;
}

CXTPMarkupObject* CXTPMarkupObject::FindName(LPCWSTR lpszName)
{
	CXTPMarkupString* pName = MARKUP_STATICCAST(CXTPMarkupString, GetValue(m_pNameProperty));
	if (pName && (wcscmp(*pName, lpszName) == 0))
		return this;

	int nCount = GetLogicalChildrenCount();
	for (int i = 0; i < nCount; i++)
	{
		CXTPMarkupObject* pResult = GetLogicalChild(i)->FindName(lpszName);
		if (pResult)
		{
			return pResult;
		}
	}

	return NULL;
}

int CXTPMarkupObject::GetLogicalChildrenCount() const
{
	return 0;
}

CXTPMarkupObject* CXTPMarkupObject::GetLogicalChild(int /*nIndex*/) const
{
	ASSERT(FALSE);
	return NULL;
}

void CXTPMarkupObject::AddExtension(CXTPMarkupExtension* pExtension)
{
	ASSERT(NULL != pExtension);
	m_Extensions.AddTail(pExtension);
	MARKUP_ADDREF(pExtension);
}

CXTPMarkupExtension* CXTPMarkupObject::GetNextExtension(const CXTPMarkupType* pClass /*= NULL*/)
{
	CXTPMarkupExtension* pExtension = NULL;

	if (NULL == m_posExtension)
	{
		ResetExtensionEnumerator();
	}

	while (NULL != m_posExtension)
	{
		CXTPMarkupExtension* pExt = m_Extensions.GetNext(m_posExtension);
		if (NULL != pClass)
		{
			if (!pExt->IsKindOf(pClass))
			{
				continue;
			}
		}

		pExtension = pExt;
		break;
	}

	return pExtension;
}

const CXTPMarkupExtension*
	CXTPMarkupObject::GetNextExtension(const CXTPMarkupType* pClass /*= NULL*/) const
{
	return const_cast<CXTPMarkupObject*>(this)->GetNextExtension(pClass);
}

void CXTPMarkupObject::ResetExtensionEnumerator()
{
	m_posExtension = m_Extensions.GetHeadPosition();
}

BOOL CXTPMarkupObject::IsKindOf(const CXTPMarkupType* pClass) const
{
	CXTPMarkupType* pTypeThis = GetType();
	return pTypeThis->IsDerivedFrom(pClass);
}

//////////////////////////////////////////////////////////////////////////
// CXTPMarkupAutoPtr

CXTPMarkupAutoPtr::CXTPMarkupAutoPtr(CXTPMarkupObject* pObject /*= NULL*/)
{
	m_pObject = pObject;
}

CXTPMarkupAutoPtr::~CXTPMarkupAutoPtr()
{
	MARKUP_RELEASE(m_pObject);
}

void CXTPMarkupAutoPtr::Assign(CXTPMarkupObject* pObject)
{
	MARKUP_RELEASE(m_pObject);
	m_pObject = pObject;
}

CXTPMarkupObject* CXTPMarkupAutoPtr::AddRef()
{
	ASSERT(NULL != m_pObject);
	m_pObject->AddRef();
	return m_pObject;
}

CXTPMarkupObject* CXTPMarkupAutoPtr::operator->()
{
	ASSERT(NULL != m_pObject);
	return m_pObject;
}

const CXTPMarkupObject* CXTPMarkupAutoPtr::operator->() const
{
	ASSERT(NULL != m_pObject);
	return m_pObject;
}

//////////////////////////////////////////////////////////////////////////
// CXTPMarkupInt

IMPLEMENT_MARKUPCLASS(NULL, CXTPMarkupInt, CXTPMarkupObject);

void CXTPMarkupInt::RegisterMarkupClass()
{
}

CXTPMarkupInt::CXTPMarkupInt(int nValue)
{
	m_nValue = nValue;
}

CXTPMarkupObject* CXTPMarkupInt::ConvertFrom(CXTPMarkupBuilder* pBuilder,
											 CXTPMarkupObject* pObject) const
{
	ASSERT(NULL != pBuilder);
	ASSERT(NULL != pObject);

	CXTPMarkupInt* pValue = NULL;

	if (IsStringObject(pObject))
	{
		int nValue = _wtoi(*((CXTPMarkupString*)pObject));

		if (NULL != m_pAssociatedProperty)
		{
			if (0
				!= (m_pAssociatedProperty->GetFlags()
					& CXTPMarkupPropertyMetadata::flagHorzDpiSensible))
			{
				nValue = pBuilder->GetMarkupContext()->ScaleX(nValue);
			}
			else if (0
					 != (m_pAssociatedProperty->GetFlags()
						 & CXTPMarkupPropertyMetadata::flagVertDpiSensible))
			{
				nValue = pBuilder->GetMarkupContext()->ScaleY(nValue);
			}
		}

		pValue = new CXTPMarkupInt(nValue);
	}

	return pValue;
}

BOOL CXTPMarkupInt::IsEqual(const CXTPMarkupObject* pObject) const
{
	if (!pObject)
		return FALSE;

	if (pObject->GetType() != MARKUP_TYPE(CXTPMarkupInt))
		return FALSE;

	return m_nValue == ((CXTPMarkupInt*)pObject)->m_nValue;
}

//////////////////////////////////////////////////////////////////////////
// CXTPMarkupDouble

IMPLEMENT_MARKUPCLASS(NULL, CXTPMarkupDouble, CXTPMarkupObject);

void CXTPMarkupDouble::RegisterMarkupClass()
{
}

CXTPMarkupDouble::CXTPMarkupDouble(double dValue)
{
	m_dValue = dValue;
}

CXTPMarkupObject* CXTPMarkupDouble::ConvertFrom(CXTPMarkupBuilder* pBuilder,
												CXTPMarkupObject* pObject) const
{
	ASSERT(NULL != pBuilder);
	ASSERT(NULL != pObject);

	CXTPMarkupDouble* pValue = NULL;

	if (IsStringObject(pObject))
	{
		LPCWSTR lpszValue = *(CXTPMarkupString*)pObject;

		double nValue = 0;
		if (CXTPMarkupBuilder::ConvertDouble(lpszValue, nValue))
		{
			if (NULL != m_pAssociatedProperty)
			{
				if (0
					!= (m_pAssociatedProperty->GetFlags()
						& CXTPMarkupPropertyMetadata::flagHorzDpiSensible))
				{
					nValue = pBuilder->GetMarkupContext()->ScaleX(nValue);
				}
				else if (0
						 != (m_pAssociatedProperty->GetFlags()
							 & CXTPMarkupPropertyMetadata::flagVertDpiSensible))
				{
					nValue = pBuilder->GetMarkupContext()->ScaleY(nValue);
				}
			}

			pValue = new CXTPMarkupDouble(nValue);
		}
	}

	return pValue;
}

BOOL CXTPMarkupDouble::IsEqual(const CXTPMarkupObject* pObject) const
{
	if (!pObject)
		return FALSE;

	if (pObject->GetType() != MARKUP_TYPE(CXTPMarkupDouble))
		return FALSE;

	return m_dValue == ((CXTPMarkupDouble*)pObject)->m_dValue;
}

//////////////////////////////////////////////////////////////////////////
// CXTPMarkupEnum

IMPLEMENT_MARKUPCLASS(NULL, CXTPMarkupEnum, CXTPMarkupObject);

void CXTPMarkupEnum::RegisterMarkupClass()
{
}

CXTPMarkupEnum::CXTPMarkupEnum(int nValue)
{
	m_nValue = nValue;
}

CXTPMarkupObject* CXTPMarkupEnum::ConvertFrom(CXTPMarkupBuilder* /*pBuilder*/,
											  CXTPMarkupObject* /*pObject*/) const
{
	return NULL;
}

BOOL CXTPMarkupEnum::IsEqual(const CXTPMarkupObject* pObject) const
{
	if (!pObject)
		return FALSE;

	if (pObject->GetType() != MARKUP_TYPE(CXTPMarkupEnum))
		return FALSE;

	return m_nValue == ((CXTPMarkupEnum*)pObject)->m_nValue;
}

CXTPMarkupEnum* CXTPMarkupEnum::CreateValue(int nValue)
{
	return new CXTPMarkupEnum(nValue);
}

//////////////////////////////////////////////////////////////////////////
// CXTPMarkupBool

IMPLEMENT_MARKUPCLASS(NULL, CXTPMarkupBool, CXTPMarkupObject);

void CXTPMarkupBool::RegisterMarkupClass()
{
}

CXTPMarkupBool::CXTPMarkupBool(BOOL bValue)
{
	m_bValue = bValue;
}

CXTPMarkupBool* CXTPMarkupBool::CreateTrueValue()
{
	return CreateValue(TRUE);
}

CXTPMarkupBool* CXTPMarkupBool::CreateValue(BOOL bValue)
{
	return new CXTPMarkupBool(bValue);
}

CXTPMarkupBool* CXTPMarkupBool::CreateFalseValue()
{
	return CreateValue(FALSE);
}

CXTPMarkupObject* CXTPMarkupBool::ConvertFrom(CXTPMarkupBuilder* pBuilder,
											  CXTPMarkupObject* pObject) const
{
	return CXTPMarkupBuilder::ConvertBool(pBuilder, pObject);
}

UINT CXTPMarkupBool::GetHashKey() const
{
	return XTPToUInt(m_bValue);
}

BOOL CXTPMarkupBool::IsEqual(const CXTPMarkupObject* pObject) const
{
	if (!pObject)
		return FALSE;

	if (pObject->GetType() != MARKUP_TYPE(CXTPMarkupBool))
		return FALSE;

	if (m_bValue != ((CXTPMarkupBool*)pObject)->m_bValue)
		return FALSE;

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
// CXTPMarkupColor

IMPLEMENT_MARKUPCLASS(L"Color", CXTPMarkupColor, CXTPMarkupObject);

void CXTPMarkupColor::RegisterMarkupClass()
{
}

CXTPMarkupColor::CXTPMarkupColor(COLORREF nValue)
{
	m_nValue = nValue | 0xFF000000;
}

CXTPMarkupColor::CXTPMarkupColor(BYTE bAlpha, COLORREF nValue)
{
	m_nValue = (nValue & 0xFFFFFF) | (COLORREF(bAlpha) << 24);
}

BOOL CXTPMarkupColor::ConvertFromString(LPCWSTR lpszValue, COLORREF& clr)
{
	size_t nLength = wcslen(lpszValue);
	if (lpszValue[0] == '#' && nLength == 7)
	{
		UINT clrRed = 0, clrGreen = 0, clrBlue = 0;
		if (WSCANF_S(lpszValue, L"#%2x%2x%2x", &clrRed, &clrGreen, &clrBlue) != 3)
			return FALSE;

		clr = RGB(clrRed, clrGreen, clrBlue) | 0xFF000000;
		return TRUE;
	}

	if (lpszValue[0] == '#' && nLength == 9)
	{
		UINT clrRed = 0, clrGreen = 0, clrBlue = 0, clrAlpha = 0;
		if (WSCANF_S(lpszValue, L"#%2x%2x%2x%2x", &clrAlpha, &clrRed, &clrGreen, &clrBlue) != 4)
			return FALSE;

		clr = RGB(clrRed, clrGreen, clrBlue) | (clrAlpha << 24);
		return TRUE;
	}

	if (lpszValue[0] != '#')
	{
		if (CXTPMarkupBuilder::ColorStringToKnownColor(lpszValue, clr))
		{
			return TRUE;
		}
	}

	return FALSE;
}

CXTPMarkupObject* CXTPMarkupColor::ConvertFrom(CXTPMarkupBuilder* /*pBuilder*/,
											   CXTPMarkupObject* pObject) const
{
	if (IsStringObject(pObject))
	{
		COLORREF clr;
		if (ConvertFromString(*((CXTPMarkupString*)pObject), clr))
		{
			return new CXTPMarkupColor((BYTE)(clr >> 24), clr & 0xFFFFFF);
		}
	}

	return NULL;
}

BOOL CXTPMarkupColor::IsEqual(const CXTPMarkupObject* pObject) const
{
	if (!pObject)
		return FALSE;

	if (pObject->GetType() != MARKUP_TYPE(CXTPMarkupColor))
		return FALSE;

	return m_nValue == ((CXTPMarkupColor*)pObject)->m_nValue;
}

void CXTPMarkupColor::SetContentObject(CXTPMarkupBuilder* pBuilder, CXTPMarkupObject* pContent)
{
	if (IsStringObject(pContent))
	{
		COLORREF clr;
		if (ConvertFromString(*((CXTPMarkupString*)pContent), clr))
		{
			m_nValue = clr;
			return;
		}
	}

	CXTPMarkupObject::SetContentObject(pBuilder, pContent);
}

//////////////////////////////////////////////////////////////////////////
// CXTPMarkupColorKey

IMPLEMENT_MARKUPCLASS(NULL, CXTPMarkupColorKey, CXTPMarkupObject);

void CXTPMarkupColorKey::RegisterMarkupClass()
{
}

//////////////////////////////////////////////////////////////////////////
// CXTPMarkupCollection

IMPLEMENT_MARKUPCLASS(NULL, CXTPMarkupCollection, CXTPMarkupObject);

void CXTPMarkupCollection::RegisterMarkupClass()
{
}

CXTPMarkupCollection::CXTPMarkupCollection()
{
	m_pElementType   = NULL;
	m_bLogicalParent = TRUE;
}

CXTPMarkupCollection::~CXTPMarkupCollection()
{
	RemoveAll();
}

void CXTPMarkupCollection::RemoveAll()
{
	for (int i = 0; i < m_arrItems.GetSize(); i++)
	{
		if (m_bLogicalParent)
			m_arrItems[i]->SetLogicalParent(NULL);
		m_arrItems[i]->Release();
	}
	m_arrItems.RemoveAll();

	OnChanged();
}

void CXTPMarkupCollection::Remove(int nIndex)
{
	if (nIndex < m_arrItems.GetSize())
	{
		CXTPMarkupObject* pItem = m_arrItems[nIndex];
		m_arrItems.RemoveAt(nIndex);

		if (m_bLogicalParent)
			pItem->SetLogicalParent(NULL);
		pItem->Release();

		OnChanged();
	}
}

void CXTPMarkupCollection::Insert(int nIndex, CXTPMarkupObject* pElement)
{
	ASSERT(pElement);
	ASSERT(m_pElementType);
	ASSERT(pElement->IsKindOf(m_pElementType));

	if (!pElement || !pElement->IsKindOf(m_pElementType))
		return;

	if (m_bLogicalParent)
	{
		ASSERT(pElement->GetLogicalParent() == NULL);
		pElement->SetLogicalParent(this);
	}

	if (nIndex < 0 || nIndex > (int)m_arrItems.GetSize())
		nIndex = (int)m_arrItems.GetSize();

	m_arrItems.InsertAt(nIndex, pElement);

	OnItemAdded(pElement, nIndex);
	OnChanged();
}

void CXTPMarkupCollection::Add(CXTPMarkupObject* pElement)
{
	Insert((int)m_arrItems.GetSize(), pElement);
}

int CXTPMarkupCollection::IndexOf(CXTPMarkupObject* pElement) const
{
	for (int i = 0; i < m_arrItems.GetSize(); i++)
	{
		if (pElement == m_arrItems[i])
			return i;
	}
	return -1;
}

void CXTPMarkupCollection::OnItemAdded(CXTPMarkupObject* /*pItem*/, int /*nIndex*/)
{
}

void CXTPMarkupCollection::OnChanged()
{
}

void CXTPMarkupCollection::SetContentObject(CXTPMarkupBuilder* pBuilder, CXTPMarkupObject* pContent)
{
	ASSERT(m_pElementType);

	if (pContent->IsKindOf(m_pElementType))
	{
		Add(pContent);
		MARKUP_ADDREF(pContent);
	}
	else
	{
		pBuilder->ThrowBuilderException(CXTPMarkupBuilder::FormatString(
			_T("'%ls' object cannot be added to '%ls'. Object cannot be converted to type '%ls'"),
			(LPCTSTR)pContent->GetType()->m_lpszClassName, (LPCTSTR)GetType()->m_lpszClassName,
			(LPCTSTR)m_pElementType->m_lpszClassName));
	}
}

BOOL CXTPMarkupCollection::HasContentObject() const
{
	return GetCount() > 0;
}

int CXTPMarkupCollection::GetLogicalChildrenCount() const
{
	return (int)m_arrItems.GetSize();
}

CXTPMarkupObject* CXTPMarkupCollection::GetLogicalChild(int nIndex) const
{
	return m_arrItems[nIndex];
}

//////////////////////////////////////////////////////////////////////////
// CXTPMarkupDoubleCollection

IMPLEMENT_MARKUPCLASS(L"DoubleCollection", CXTPMarkupDoubleCollection, CXTPMarkupObject)

void CXTPMarkupDoubleCollection::RegisterMarkupClass()
{
}

CXTPMarkupDoubleCollection::CXTPMarkupDoubleCollection()
{
}

CXTPMarkupDoubleCollection::CXTPMarkupDoubleCollection(CDoubleArray& arr)
{
	m_arr.Copy(arr);
}

int CXTPMarkupDoubleCollection::GetCount() const
{
	return (int)m_arr.GetSize();
}

const float* CXTPMarkupDoubleCollection::GetData() const
{
	return m_arr.GetData();
}

float CXTPMarkupDoubleCollection::GetAt(int nIndex) const
{
	return m_arr.GetAt(nIndex);
}

void CXTPMarkupDoubleCollection::RemoveAll()
{
	m_arr.RemoveAll();
}

void CXTPMarkupDoubleCollection::Remove(int nIndex)
{
	m_arr.RemoveAt(nIndex);
}

void CXTPMarkupDoubleCollection::Add(float fValue)
{
	m_arr.Add(fValue);
}

BOOL CXTPMarkupDoubleCollection::GetNextValue(LPCWSTR& lpszValue, float& fValue)
{
	while (*lpszValue == ' ')
		lpszValue++;
	LPCWSTR lpszNext = lpszValue;

	fValue	= 0;
	int nSign = 1;
	if (*lpszNext == '-')
	{
		nSign = -1;
		lpszNext++;
	}

	if (*lpszNext == 0)
		return FALSE;

	int factor = 1;

	while (*lpszNext != 0)
	{
		WCHAR c = *lpszNext;

		if (c == ' ' || c == ',')
		{
			if (lpszNext == lpszValue)
				return FALSE;
			lpszNext++;
			break;
		}

		if (c == '.')
		{
			if (factor != 1)
				return FALSE;
			factor = 10;
			lpszNext++;
			continue;
		}

		if (c < '0' || c > '9')
			return FALSE;

		if (factor != 1)
		{
			fValue += static_cast<float>(c - '0') / static_cast<float>(factor);
			factor *= 10;
		}
		else
		{
			fValue = 10 * fValue + static_cast<float>(c - '0');
		}

		lpszNext++;
	}

	lpszValue = lpszNext;
	fValue *= static_cast<float>(nSign);

	return TRUE;
}

BOOL CXTPMarkupDoubleCollection::ConvertFromString(LPCWSTR lpszValue, CDoubleArray& arr)
{
	if (!lpszValue)
		return FALSE;

	while (*lpszValue != 0)
	{
		float x;
		if (!GetNextValue(lpszValue, x))
			return FALSE;

		while (*lpszValue == ' ')
			lpszValue++;

		arr.Add(x);
	}

	return TRUE;
}

CXTPMarkupObject* CXTPMarkupDoubleCollection::ConvertFrom(CXTPMarkupBuilder* /*pBuilder*/,
														  CXTPMarkupObject* pObject) const
{
	if (IsStringObject(pObject))
	{
		CDoubleArray arr;
		if (ConvertFromString(*((CXTPMarkupString*)pObject), arr))
		{
			return new CXTPMarkupDoubleCollection(arr);
		}
	}

	return NULL;
}

BEGIN_DISPATCH_MAP(CXTPMarkupObject, CXTPCmdTarget)
END_DISPATCH_MAP()

#ifdef _XTP_ACTIVEX

BEGIN_INTERFACE_MAP(CXTPMarkupObject, CXTPCmdTarget)
	INTERFACE_PART(CXTPMarkupObject, XTPDIID_MarkupObject, Dispatch)
END_INTERFACE_MAP()

IMPLEMENT_OLETYPELIB_EX(CXTPMarkupObject, XTPDIID_MarkupObject)
#endif

CXTPMarkupObject* CXTPMarkupObject::FromDispatch(LPDISPATCH lpElementDisp, BOOL bAddRef)
{
	if (!lpElementDisp)
	{
		return NULL;
	}

	CXTPMarkupObject* pObject = XTP_DYNAMIC_DOWNCAST_REMOTE_(CXTPMarkupObject,
															 CXTPMarkupObject::FromIDispatchSafe(
																 lpElementDisp));
	if (NULL == pObject)
	{
		AfxThrowOleException(E_INVALIDARG);
	}

	if (bAddRef)
	{
		pObject->InternalAddRef();
	}

	return pObject;
}

//////////////////////////////////////////////////////////////////////////
// CXTPMarkupDoubleCollection

#include "Common/Base/Diagnostic/XTPBeginAfxMap.h"
BEGIN_DISPATCH_MAP(CXTPMarkupDoubleCollection, CXTPMarkupObject)
	DISP_FUNCTION_ID(CXTPMarkupDoubleCollection, "Count", 1, OleGetItemCount, VT_I4, VTS_NONE)
	DISP_FUNCTION_ID(CXTPMarkupDoubleCollection, "Item", DISPID_VALUE, OleGetItem, VT_R8, VTS_I4)
	DISP_FUNCTION_ID(CXTPMarkupDoubleCollection, "Add", 2, OleAdd, VT_EMPTY, VTS_R8)
	DISP_FUNCTION_ID(CXTPMarkupDoubleCollection, "Clear", 4, RemoveAll, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION_ID(CXTPMarkupDoubleCollection, "RemoveAt", 5, Remove, VT_EMPTY, VTS_I4)
END_DISPATCH_MAP()
#include "Common/Base/Diagnostic/XTPEndAfxMap.h"

#ifdef _XTP_ACTIVEX

BEGIN_INTERFACE_MAP(CXTPMarkupDoubleCollection, CXTPMarkupObject)
	INTERFACE_PART(CXTPMarkupDoubleCollection, XTPDIID_MarkupDoubleCollection, Dispatch)
END_INTERFACE_MAP()

IMPLEMENT_OLETYPELIB_EX(CXTPMarkupDoubleCollection, XTPDIID_MarkupDoubleCollection)
#endif

long CXTPMarkupDoubleCollection::OleGetItemCount()
{
	return (long)GetCount();
}

double CXTPMarkupDoubleCollection::OleGetItem(long nIndex)
{
	return GetAt(nIndex);
}

void CXTPMarkupDoubleCollection::OleAdd(double dValue)
{
	Add((float)dValue);
}
