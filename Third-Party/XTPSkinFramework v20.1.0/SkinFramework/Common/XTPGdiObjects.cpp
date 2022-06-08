// XTPGdiObjects.cpp: Framework extension and helper classes
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

#include "stdafx.h"

#include "Common/XTPTypeId.h"
#include "Common/XTPCasting.h"
#include "Common/XTPSystemHelpers.h"
#include "Common/XTPSynchro.h"
#include "Common/XTPApplication.h"
#include "Common/XTPSingleton.h"
#include "Common/XTPGdiObjects.h"
#include "Common/XTPVC80Helpers.h"

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

#ifdef XTP_ENABLE_UNAUTHORIZED_CACHED_GDI_OBJECT_CHANGE_FAULT

#	ifndef _DEBUG
#	error XTP_ENABLE_UNAUTHORIZED_CACHED_GDI_OBJECT_CHANGE_FAULT must be defined in Debug mode only.
#	endif

#	pragma XTP_MESSAGE(                                                                           \
		"XTP_ENABLE_UNAUTHORIZED_CACHED_GDI_OBJECT_CHANGE_FAULT is defined, the application "      \
		"may crash with page access violation during debugging if unauthorized cached object changes are detected")

#endif // XTP_ENABLE_UNAUTHORIZED_CACHED_GDI_OBJECT_CHANGE_FAULT

#ifdef _DEBUG
#	undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#	define new DEBUG_NEW
#endif

class CXTPCachedGdiObjectReferenceBase;

template<class T>
inline UINT QuickSequenceHashFunction(T value, UINT pos)
{
	return (value + pos) * (pos + 1) + ~value * (pos + 2);
}

template<class T>
inline UINT QuickSequenceHash(const T* pSequence, UINT nSizeBytes)
{
	ASSERT(NULL != pSequence);
	ASSERT((0 == nSizeBytes % sizeof(T)) && (sizeof(T) <= nSizeBytes));

	UINT nHash = 0;
	UINT n	   = nSizeBytes / sizeof(T);
	for (UINT i = 0; i < n; ++i)
	{
		nHash += QuickSequenceHashFunction(pSequence[i], i);
	}

	return nHash;
}

template<class T>
inline T* AllocateReferenceObject()
{
#ifdef XTP_ENABLE_UNAUTHORIZED_CACHED_GDI_OBJECT_CHANGE_FAULT
	T* pObj = reinterpret_cast<T*>(::VirtualAlloc(NULL, sizeof(T), MEM_COMMIT, PAGE_READWRITE));
	ASSERT(NULL != pObj);
	return pObj;
#else
	return reinterpret_cast<T*>(malloc(sizeof(T)));
#endif
}

template<class T>
inline void DestroyReferenceObject(T* pObj)
{
	ASSERT(NULL != pObj);

#ifdef XTP_ENABLE_UNAUTHORIZED_CACHED_GDI_OBJECT_CHANGE_FAULT
	MEMORY_BASIC_INFORMATION memInfo;
	ASSERT(0 <= ::VirtualQuery(pObj, &memInfo, sizeof(memInfo))
		   && memInfo.Protect == PAGE_READWRITE);

	pObj->~T();
	::VirtualFree(pObj, 0, MEM_RELEASE);
#else
	pObj->~T();
	free(pObj);
#endif
}

///////////////////////////////////////////////////////////////////////////////
// CXTPGdiObjectCacheBase

IMPLEMENT_DYNAMIC(CXTPGdiObjectCacheBase, CObject);

///////////////////////////////////////////////////////////////////////////////
// CXTPCachedGdiObjectReferenceBase

class CXTPCachedGdiObjectReferenceBase : public CObject
{
	DECLARE_DYNAMIC(CXTPCachedGdiObjectReferenceBase);

public:
	virtual ~CXTPCachedGdiObjectReferenceBase();

	void AddRef();
	BOOL Release();

	void UnbindFromCache();

	CGdiObject* GetGdiObject();
	const CGdiObject* GetGdiObject() const;

	virtual void* GetDescriptor() const = 0;

protected:
	CXTPCachedGdiObjectReferenceBase();

	void Init(CXTPGdiObjectCacheBase* pCache, CGdiObject* pObject);

	virtual BOOL DeleteObject()							   = 0;
	virtual XTPGDIDESCRIPTOR& AccessDescriptorData() const = 0;

	CXTPGdiObjectCacheBase* m_pCache;
	CGdiObject* m_pObject;
	LONG m_cRefs;

#ifdef _DEBUG
	HGDIOBJ m_hObject;

public:
	void AssertUnaltered();

#endif

#ifdef XTP_ENABLE_UNAUTHORIZED_CACHED_GDI_OBJECT_CHANGE_FAULT
public:
	virtual void EnableUnauthorizedCachedGdiHandleChangeFault(BOOL bEnable = TRUE) = 0;

protected:
	LONG* m_pnUnauthorizedCachedGdiHandleChangeFaultCounter;

#endif // XTP_ENABLE_UNAUTHORIZED_CACHED_GDI_OBJECT_CHANGE_FAULT
};

IMPLEMENT_DYNAMIC(CXTPCachedGdiObjectReferenceBase, CObject);

AFX_INLINE CGdiObject* CXTPCachedGdiObjectReferenceBase::GetGdiObject()
{
	return m_pObject;
}

AFX_INLINE const CGdiObject* CXTPCachedGdiObjectReferenceBase::GetGdiObject() const
{
	return m_pObject;
}

AFX_INLINE void CXTPCachedGdiObjectReferenceBase::AddRef()
{
#ifdef XTP_ENABLE_UNAUTHORIZED_CACHED_GDI_OBJECT_CHANGE_FAULT
	EnableUnauthorizedCachedGdiHandleChangeFault(FALSE);
#endif

	InterlockedIncrement(&m_cRefs);

#ifdef XTP_ENABLE_UNAUTHORIZED_CACHED_GDI_OBJECT_CHANGE_FAULT
	EnableUnauthorizedCachedGdiHandleChangeFault();
#endif
}

BOOL CXTPCachedGdiObjectReferenceBase::Release()
{
#ifdef XTP_ENABLE_UNAUTHORIZED_CACHED_GDI_OBJECT_CHANGE_FAULT
	EnableUnauthorizedCachedGdiHandleChangeFault(FALSE);
#endif

	ASSERT(0 < m_cRefs);

	BOOL bResult	 = TRUE;
	BOOL bSelfDelete = FALSE;

	if (NULL != m_pCache)
	{
		XTP_GUARD_EXCLUSIVE(CXTPGdiObjectCacheBase, m_pCache)
		{
			if (0 == InterlockedDecrement(&m_cRefs))
			{
				m_pCache->RemoveFromCache(&AccessDescriptorData());
				bResult		= DeleteObject();
				bSelfDelete = TRUE;
			}
		}
	}
	else
	{
		if (0 == InterlockedDecrement(&m_cRefs))
		{
			bResult		= DeleteObject();
			bSelfDelete = TRUE;
		}
	}

	if (bSelfDelete)
	{
		DestroyReferenceObject(this);
	}
#ifdef XTP_ENABLE_UNAUTHORIZED_CACHED_GDI_OBJECT_CHANGE_FAULT
	else
	{
		EnableUnauthorizedCachedGdiHandleChangeFault();
	}
#endif

	return bResult;
}

AFX_INLINE void CXTPCachedGdiObjectReferenceBase::UnbindFromCache()
{
#ifdef XTP_ENABLE_UNAUTHORIZED_CACHED_GDI_OBJECT_CHANGE_FAULT
	EnableUnauthorizedCachedGdiHandleChangeFault(FALSE);
#endif

	m_pCache = NULL;

#ifdef XTP_ENABLE_UNAUTHORIZED_CACHED_GDI_OBJECT_CHANGE_FAULT
	EnableUnauthorizedCachedGdiHandleChangeFault();
#endif
}

AFX_INLINE CXTPCachedGdiObjectReferenceBase::CXTPCachedGdiObjectReferenceBase()
	: m_cRefs(1)
#ifdef XTP_ENABLE_UNAUTHORIZED_CACHED_GDI_OBJECT_CHANGE_FAULT
	, m_pnUnauthorizedCachedGdiHandleChangeFaultCounter(new LONG(0))
#endif
{
}

AFX_INLINE CXTPCachedGdiObjectReferenceBase::~CXTPCachedGdiObjectReferenceBase()
{
#ifdef XTP_ENABLE_UNAUTHORIZED_CACHED_GDI_OBJECT_CHANGE_FAULT
	delete m_pnUnauthorizedCachedGdiHandleChangeFaultCounter;
#endif
}

void CXTPCachedGdiObjectReferenceBase::Init(CXTPGdiObjectCacheBase* pCache, CGdiObject* pObject)
{
	ASSERT_VALID(pCache);
	ASSERT_VALID(pObject);
	ASSERT(NULL != pObject->m_hObject);

	m_pCache  = pCache;
	m_pObject = pObject;

	AccessDescriptorData().pReference = this;

#ifdef _DEBUG
	m_hObject = pObject->m_hObject;

#	ifdef XTP_ENABLE_UNAUTHORIZED_CACHED_GDI_OBJECT_CHANGE_FAULT
	EnableUnauthorizedCachedGdiHandleChangeFault();
#	endif

#endif
}

#ifdef _DEBUG

AFX_INLINE void CXTPCachedGdiObjectReferenceBase::AssertUnaltered()
{
	if (m_hObject != m_pObject->m_hObject)
	{
		TRACE(_T("Cached GDI object has been changed. Expected handle - 0x%pX, actual handle - ")
			  _T("0x%pX"),
			  m_hObject, m_pObject->m_hObject);
		ASSERT(!"Cached GDI object has been changed which can cause undetermined behavior "
				"in locations where the cached object is re-used.\n"
				"IMPORTANT: The issue can be both in the ToolkitPro code "
				"and consumer code. In order to determine the root cause of the issue re-compile "
				"ToolkitPro "
				"with XTP_ENABLE_UNAUTHORIZED_CACHED_GDI_OBJECT_CHANGE_FAULT macro defined in "
				"Deubg mode and try "
				"to re-produce it again when running in debugger. When "
				"XTP_ENABLE_UNAUTHORIZED_CACHED_GDI_OBJECT_CHANGE_FAULT is defined "
				"any attempt to alter a cached GDI object will result in memory access violation. "
				"Once you get "
				"this exception please report it with the copy of full stack trace and output "
				"window to support@codejock.com.");
	}
}

#endif

///////////////////////////////////////////////////////////////////////////////
// CXTPCachedGdiObjectReference

template<class Descriptor, class GdiObject, class XtpCache>
class CXTPCachedGdiObjectReference : public CXTPCachedGdiObjectReferenceBase
{
public:
	CXTPCachedGdiObjectReference(CXTPGdiObjectCacheBase* pCache, HGDIOBJ hObject,
								 Descriptor& descriptor);
	~CXTPCachedGdiObjectReference();

	GdiObject* Get();
	const GdiObject* Get() const;

	virtual void* GetDescriptor() const;

protected:
	virtual BOOL DeleteObject();
	virtual XTPGDIDESCRIPTOR& AccessDescriptorData() const;

#ifdef XTP_ENABLE_UNAUTHORIZED_CACHED_GDI_OBJECT_CHANGE_FAULT
	virtual void EnableUnauthorizedCachedGdiHandleChangeFault(BOOL bEnable = TRUE);
#endif

	GdiObject m_gdiObject;
	Descriptor m_descriptor;
};

template<class Descriptor, class GdiObject, class XtpCache>
AFX_INLINE
	CXTPCachedGdiObjectReference<Descriptor, GdiObject, XtpCache>::CXTPCachedGdiObjectReference(
		CXTPGdiObjectCacheBase* pCache, HGDIOBJ hObject, Descriptor& descriptor)
	: m_descriptor(descriptor)
{
	m_gdiObject.Attach(hObject);
	Init(pCache, &m_gdiObject);
}

template<class Descriptor, class GdiObject, class XtpCache>
AFX_INLINE
	CXTPCachedGdiObjectReference<Descriptor, GdiObject, XtpCache>::~CXTPCachedGdiObjectReference()
{
	CXTPCachedGdiObjectReference::DeleteObject();
}

template<class Descriptor, class GdiObject, class XtpCache>
AFX_INLINE void* CXTPCachedGdiObjectReference<Descriptor, GdiObject, XtpCache>::GetDescriptor() const
{
	return &const_cast<CXTPCachedGdiObjectReference*>(this)->m_descriptor;
}

template<class Descriptor, class GdiObject, class XtpCache>
BOOL CXTPCachedGdiObjectReference<Descriptor, GdiObject, XtpCache>::DeleteObject()
{
#ifdef XTP_ENABLE_UNAUTHORIZED_CACHED_GDI_OBJECT_CHANGE_FAULT
	EnableUnauthorizedCachedGdiHandleChangeFault(FALSE);
#endif

	BOOL bResult = TRUE;

	if (NULL != m_gdiObject.m_hObject)
	{
		bResult = m_gdiObject.DeleteObject();
#ifdef _DEBUG
		if (0 == AccessDescriptorData().nHandleOwnershipViolatorCount)
		{
			ASSERT("GDI handle is not released properly prior to being deleted" && bResult);
		}
#endif
	}

#ifdef XTP_ENABLE_UNAUTHORIZED_CACHED_GDI_OBJECT_CHANGE_FAULT
	EnableUnauthorizedCachedGdiHandleChangeFault();
#endif

	return bResult;
}

template<class Descriptor, class GdiObject, class XtpCache>
AFX_INLINE XTPGDIDESCRIPTOR&
	CXTPCachedGdiObjectReference<Descriptor, GdiObject, XtpCache>::AccessDescriptorData() const
{
	return XtpCache::AccessDescriptorData(GetDescriptor());
}

template<class Descriptor, class GdiObject, class XtpCache>
AFX_INLINE GdiObject* CXTPCachedGdiObjectReference<Descriptor, GdiObject, XtpCache>::Get()
{
	return static_cast<GdiObject*>(m_pObject);
}

template<class Descriptor, class GdiObject, class XtpCache>
AFX_INLINE const GdiObject*
	CXTPCachedGdiObjectReference<Descriptor, GdiObject, XtpCache>::Get() const
{
	return static_cast<GdiObject*>(m_pObject);
}

#ifdef XTP_ENABLE_UNAUTHORIZED_CACHED_GDI_OBJECT_CHANGE_FAULT

template<class Descriptor, class GdiObject, class XtpCache>
void CXTPCachedGdiObjectReference<Descriptor, GdiObject, XtpCache>::
	EnableUnauthorizedCachedGdiHandleChangeFault(BOOL bEnable /*= TRUE*/)
{
	BOOL bProtect = (bEnable ? 0 == (*m_pnUnauthorizedCachedGdiHandleChangeFaultCounter)++
							 : 0 == --(*m_pnUnauthorizedCachedGdiHandleChangeFaultCounter));

	if (bProtect)
	{
		DWORD dwOldProtection = 0;
		if (!::VirtualProtect(this, sizeof(*this), bEnable ? PAGE_READONLY : PAGE_READWRITE,
							  &dwOldProtection))
		{
			TRACE(_T("VirtualProtect failed, code %i\n"), ::GetLastError());
			ASSERT(!"Unable to enable page protection");
		}
	}
}

#endif

///////////////////////////////////////////////////////////////////////////////
// CXTPGdiObject

IMPLEMENT_DYNAMIC(CXTPGdiObject, CObject);

CXTPGdiObject::CXTPGdiObject()
	: m_pCachedObject(NULL)
	, m_pDummyGdiObject(NULL)
	, m_bHandleOwnershipViolated(FALSE)
#ifdef _DEBUG
	, m_hDummyGdiObject(NULL)
#endif
{
}

CXTPGdiObject::~CXTPGdiObject()
{
#ifdef _DEBUG
	AssertDummyUnaltered();
	ASSERT("Implementation must call DeleteObject in its destructor." && NULL == m_hDummyGdiObject);
#endif

	ASSERT("Implementation must call DeleteObject in its destructor." && NULL == m_pCachedObject
		   && (NULL != m_pDummyGdiObject ? NULL == m_pDummyGdiObject->m_hObject : TRUE));
}

CXTPGdiObject::operator HGDIOBJ() const
{
	return (HGDIOBJ)*GetGdiObject();
}

BOOL CXTPGdiObject::operator==(const CXTPGdiObject& obj) const
{
	return (HGDIOBJ) * this == obj;
}

BOOL CXTPGdiObject::operator!=(const CXTPGdiObject& obj) const
{
	return (HGDIOBJ) * this != obj;
}

HGDIOBJ CXTPGdiObject::GetSafeHandle() const
{
	return (NULL != this ? (HGDIOBJ)*GetGdiObject() : NULL);
}

BOOL CXTPGdiObject::DeleteObject()
{
	BOOL bResult = TRUE;

	if (NULL != m_pCachedObject)
	{
		bResult			= m_pCachedObject->Release();
		m_pCachedObject = NULL;
	}

#ifdef _DEBUG
	AssertDummyUnaltered();
#endif

	if (NULL != m_pDummyGdiObject->m_hObject)
	{
		bResult = m_pDummyGdiObject->DeleteObject();
#ifdef _DEBUG
		m_hDummyGdiObject = NULL;
#endif
	}

	return bResult;
}

// Operations
#pragma push_macro("GetObject")
#undef GetObject
int CXTPGdiObject::GetObject(int nCount, LPVOID lpObject) const
#pragma pop_macro("GetObject")
{
	return GetGdiObject()->GetObject(nCount, lpObject);
}

UINT CXTPGdiObject::GetObjectType() const
{
	return GetGdiObject()->GetObjectType();
}

BOOL CXTPGdiObject::CreateStockObject(int nIndex)
{
	ASSERT(NULL != m_pDummyGdiObject);
	DeleteObject();

	BOOL bResult = m_pDummyGdiObject->CreateStockObject(nIndex);
#ifdef _DEBUG
	if (bResult)
	{
		m_hDummyGdiObject = m_pDummyGdiObject->m_hObject;
	}
#endif

	return bResult;
}

void CXTPGdiObject::SetHandleOwnershipNotGuaranteed()
{
	if (!m_bHandleOwnershipViolated)
	{
		XTPGDIDESCRIPTOR& descriptor = AccessDescriptorData();

#ifdef XTP_ENABLE_UNAUTHORIZED_CACHED_GDI_OBJECT_CHANGE_FAULT
		ASSERT(NULL != descriptor.pReference);
		descriptor.pReference->EnableUnauthorizedCachedGdiHandleChangeFault(FALSE);
#endif

		InterlockedIncrement(&descriptor.nHandleOwnershipViolatorCount);

#ifdef XTP_ENABLE_UNAUTHORIZED_CACHED_GDI_OBJECT_CHANGE_FAULT
		descriptor.pReference->EnableUnauthorizedCachedGdiHandleChangeFault();
#endif

		m_bHandleOwnershipViolated = TRUE;
	}
}

BOOL CXTPGdiObject::IsHandleOwnershipGuaranteed() const
{
	return 0 == AccessDescriptorData().nHandleOwnershipViolatorCount;
}

CGdiObject* CXTPGdiObject::GetGdiObject()
{
	return NULL != m_pCachedObject ? m_pCachedObject->GetGdiObject() : m_pDummyGdiObject;
}

void CXTPGdiObject::SetDummyGdiObject(CGdiObject& gdiObject)
{
	ASSERT(NULL == m_pDummyGdiObject);
	m_pDummyGdiObject = &gdiObject;
#ifdef _DEBUG
	m_hDummyGdiObject = m_pDummyGdiObject->m_hObject;
#endif
}

void CXTPGdiObject::SetCachedObject(CXTPCachedGdiObjectReferenceBase* pCachedObject)
{
	DeleteObject();
	m_pCachedObject = pCachedObject;
}

#ifdef _DEBUG

void CXTPGdiObject::AssertDummyUnaltered() const
{
	ASSERT(NULL != m_pDummyGdiObject);
	ASSERT(
		"Internal GDI object has been changed"
		&& (NULL != m_pDummyGdiObject ? m_hDummyGdiObject == m_pDummyGdiObject->m_hObject : TRUE));
}

#endif

void* CXTPGdiObject::GetDescriptor() const
{
	return (NULL != m_pCachedObject ? m_pCachedObject->GetDescriptor() : NULL);
}

//////////////////////////////////////////////////////////////////////
// CXTPFontCache

template<>
AFX_INLINE BOOL AFXAPI CompareElements(const LOGFONT* logFont1, const LOGFONT* logFont2)
{
	ASSERT(NULL != logFont1);
	ASSERT(NULL != logFont2);

	BOOL bResult = FALSE;

	if (logFont1->lfHeight == logFont2->lfHeight && logFont1->lfWidth == logFont2->lfWidth
		&& logFont1->lfEscapement == logFont2->lfEscapement
		&& logFont1->lfOrientation == logFont2->lfOrientation
		&& logFont1->lfWeight == logFont2->lfWeight && logFont1->lfItalic == logFont2->lfItalic
		&& logFont1->lfUnderline == logFont2->lfUnderline
		&& logFont1->lfStrikeOut == logFont2->lfStrikeOut
		&& logFont1->lfCharSet == logFont2->lfCharSet
		&& logFont1->lfOutPrecision == logFont2->lfOutPrecision
		&& logFont1->lfClipPrecision == logFont2->lfClipPrecision
		&& logFont1->lfQuality == logFont2->lfQuality
		&& logFont1->lfPitchAndFamily == logFont2->lfPitchAndFamily)
	{
		bResult = (0 == _tcscmp(logFont1->lfFaceName, logFont2->lfFaceName));
	}

	return bResult;
}

template<>
UINT AFXAPI HashKey(const LOGFONT& logFont)
{
	UINT nHash = QuickSequenceHash(reinterpret_cast<const UINT*>(&logFont),
								   offsetof(LOGFONT, lfFaceName));

	for (UINT i = 0; logFont.lfFaceName[i] != _T('\0'); ++i)
	{
		nHash += QuickSequenceHashFunction(logFont.lfFaceName[i], i);
	}

	return nHash;
}

AFX_INLINE CXTPFontCache* AFX_CDECL XTPFontCache()
{
	return &CXTPSingleton<CXTPFontCache>::Instance();
}

IMPLEMENT_DYNAMIC(CXTPFontCache, CXTPGdiObjectCacheBase);

CXTPFontCache::CXTPFontCache()
{
}

CXTPFontCache::~CXTPFontCache()
{
	POSITION pos = m_fontCache.GetStartPosition();
	while (NULL != pos)
	{
		XTPCachedFontReference* pFontRef = NULL;
		LOGFONT logFont;
		m_fontCache.GetNextAssoc(pos, logFont, (LPVOID&)pFontRef);
		pFontRef->UnbindFromCache();
	}
}

AFX_INLINE XTPGDIDESCRIPTOR& CXTPFontCache::AccessDescriptorData(void* pDescriptor)
{
	ASSERT(NULL != pDescriptor);
	return *reinterpret_cast<XTPLOGFONTDESCRIPTOR*>(pDescriptor);
}

CXTPFontCache::XTPCachedFontReference* CXTPFontCache::CreateFontIndirect(const LOGFONT* lpLogFont)
{
	ASSERT(AfxIsValidAddress(lpLogFont, sizeof(LOGFONT), FALSE));

	LOGFONT lf = *lpLogFont;
	TCSLWR_S(lf.lfFaceName, _countof(lf.lfFaceName)); // Ensure case-insinsitive hashing

	XTPCachedFontReference* pFontRef = NULL;
	XTP_GUARD_EXCLUSIVE(CXTPGdiObjectCacheBase, static_cast<CXTPGdiObjectCacheBase*>(this))
	{
		if (!m_fontCache.Lookup(lf, (LPVOID&)pFontRef))
		{
			HFONT hFont = ::CreateFontIndirect(&lf);
			if (NULL != hFont)
			{
				XTPLOGFONTDESCRIPTOR descriptor;
				descriptor.logFont = lf;
				pFontRef		   = AllocateReferenceObject<XTPCachedFontReference>();
#ifdef _DEBUG
#	pragma push_macro("new")
#	undef new
#endif
				new (pFontRef) XTPCachedFontReference(this, hFont, descriptor);
#ifdef _DEBUG
#	pragma pop_macro("new")
#endif
				m_fontCache.SetAt(lf, (LPVOID&)pFontRef);
			}
		}
		else
		{
			pFontRef->AddRef();

#ifdef _DEBUG
			// Make sure the cached font has not been altered.
			pFontRef->AssertUnaltered();
#endif
		}
	}

	return pFontRef;
}

CXTPFontCache::XTPCachedFontReference* CXTPFontCache::CreateFont(
	int nHeight, int nWidth, int nEscapement, int nOrientation, int nWeight, BYTE bItalic,
	BYTE bUnderline, BYTE cStrikeOut, BYTE nCharSet, BYTE nOutPrecision, BYTE nClipPrecision,
	BYTE nQuality, BYTE nPitchAndFamily, LPCTSTR lpszFaceName)
{
	LOGFONT logFont			 = { 0 };
	logFont.lfHeight		 = nHeight;
	logFont.lfWidth			 = nWidth;
	logFont.lfEscapement	 = nEscapement;
	logFont.lfOrientation	 = nOrientation;
	logFont.lfWeight		 = nWeight;
	logFont.lfItalic		 = bItalic;
	logFont.lfUnderline		 = bUnderline;
	logFont.lfStrikeOut		 = cStrikeOut;
	logFont.lfCharSet		 = nCharSet;
	logFont.lfOutPrecision	 = nOutPrecision;
	logFont.lfClipPrecision	 = nClipPrecision;
	logFont.lfQuality		 = nQuality;
	logFont.lfPitchAndFamily = nPitchAndFamily;
	lstrcpyn(logFont.lfFaceName, lpszFaceName, _countof(logFont.lfFaceName));
	return CreateFontIndirect(&logFont);
}

CXTPFontCache::XTPCachedFontReference* CXTPFontCache::CreatePointFont(int nPointSize,
																	  LPCTSTR lpszFaceName,
																	  CDC* pDC /*= NULL*/)
{
	// Default MFC implementation (from WINGDIX.CPP).
	ASSERT(AfxIsValidString(lpszFaceName));

	LOGFONT logFont	  = { 0 };
	logFont.lfCharSet = DEFAULT_CHARSET;
	logFont.lfHeight  = nPointSize;
	lstrcpyn(logFont.lfFaceName, lpszFaceName, _countof(logFont.lfFaceName));

	return CreatePointFontIndirect(&logFont, pDC);
}

CXTPFontCache::XTPCachedFontReference*
	CXTPFontCache::CreatePointFontIndirect(const LOGFONT* lpLogFont, CDC* pDC /*= NULL*/)
{
	// Default MFC implementation (from WINGDIX.CPP).
	ASSERT(AfxIsValidAddress(lpLogFont, sizeof(LOGFONT), FALSE));
	HDC hDC;
	if (pDC != NULL)
	{
		ASSERT_VALID(pDC);
		ASSERT(pDC->m_hAttribDC != NULL);
		hDC = pDC->m_hAttribDC;
	}
	else
	{
		hDC = ::GetDC(NULL);
	}

	// convert nPointSize to logical units based on pDC
	LOGFONT logFont = *lpLogFont;
	POINT pt;
	pt.y = ::GetDeviceCaps(hDC, LOGPIXELSY) * logFont.lfHeight;
	pt.y /= 720; // 72 points/inch, 10 decipoints/point
	::DPtoLP(hDC, &pt, 1);
	POINT ptOrg = { 0, 0 };
	::DPtoLP(hDC, &ptOrg, 1);
	logFont.lfHeight = -abs(pt.y - ptOrg.y);

	if (pDC == NULL)
	{
		ReleaseDC(NULL, hDC);
	}

	return CreateFontIndirect(&logFont);
}

void CXTPFontCache::RemoveFromCache(const void* pDescriptor)
{
	ASSERT(NULL != pDescriptor);

	XTP_GUARD_EXCLUSIVE(CXTPGdiObjectCacheBase, static_cast<CXTPGdiObjectCacheBase*>(this))
	{
		const XTPLOGFONTDESCRIPTOR* pLogFontDescriptor =
			reinterpret_cast<const XTPLOGFONTDESCRIPTOR*>(pDescriptor);
		m_fontCache.RemoveKey(pLogFontDescriptor->logFont);
	}
}

///////////////////////////////////////////////////////////////////////////////
// CXTPFont

IMPLEMENT_DYNAMIC(CXTPFont, CXTPGdiObject);

CXTPFont::CXTPFont()
	: m_hLogFontHandle(NULL)
{
	SetDummyGdiObject(m_dummyFont);
}

CXTPFont::~CXTPFont()
{
	DeleteObject();
}

BOOL CXTPFont::CreateFontIndirect(const LOGFONT* lpLogFont)
{
	CXTPFontCache::XTPCachedFontReference* pCachedFont = XTPFontCache()->CreateFontIndirect(
		lpLogFont);
	SetCachedObject(pCachedFont);
	return NULL != pCachedFont;
}

BOOL CXTPFont::CreateFont(int nHeight, int nWidth, int nEscapement, int nOrientation, int nWeight,
						  BYTE bItalic, BYTE bUnderline, BYTE cStrikeOut, BYTE nCharSet,
						  BYTE nOutPrecision, BYTE nClipPrecision, BYTE nQuality,
						  BYTE nPitchAndFamily, LPCTSTR lpszFacename)
{
	CXTPFontCache::XTPCachedFontReference* pCachedFont = XTPFontCache()->CreateFont(
		nHeight, nWidth, nEscapement, nOrientation, nWeight, bItalic, bUnderline, cStrikeOut,
		nCharSet, nOutPrecision, nClipPrecision, nQuality, nPitchAndFamily, lpszFacename);
	SetCachedObject(pCachedFont);
	return NULL != pCachedFont;
}

BOOL CXTPFont::CreatePointFont(int nPointSize, LPCTSTR lpszFaceName, CDC* pDC /*= NULL*/)
{
	CXTPFontCache::XTPCachedFontReference* pCachedFont =
		XTPFontCache()->CreatePointFont(nPointSize, lpszFaceName, pDC);
	SetCachedObject(pCachedFont);
	return NULL != pCachedFont;
}

BOOL CXTPFont::CreatePointFontIndirect(const LOGFONT* lpLogFont, CDC* pDC /*= NULL*/)
{
	CXTPFontCache::XTPCachedFontReference* pCachedFont =
		XTPFontCache()->CreatePointFontIndirect(lpLogFont, pDC);
	SetCachedObject(pCachedFont);
	return NULL != pCachedFont;
}

int CXTPFont::GetLogFont(LOGFONT* pLogFont)
{
	int nResult = 0;

	HGDIOBJ hCurrent = GetSafeHandle();
	if (hCurrent == m_hLogFontHandle && NULL != m_hLogFontHandle)
	{
		if (XTP_ASSERT_CHECK(NULL != pLogFont))
		{
			*pLogFont = m_logFont;
			nResult	  = sizeof(m_logFont);
		}
	}
	else
	{
		nResult = GetNativeGdiObject()->GetLogFont(pLogFont);
		if (0 != nResult)
		{
			m_hLogFontHandle = hCurrent;
			m_logFont		 = *pLogFont;
		}
	}

	return nResult;
}

XTPGDIDESCRIPTOR& CXTPFont::AccessDescriptorData()
{
	void* pDescriptor = GetDescriptor();
	ASSERT("No handle is attached, descriptor data unavailable" && NULL != pDescriptor);

	return CXTPFontCache::AccessDescriptorData(pDescriptor);
}

///////////////////////////////////////////////////////////////////////////////
// CXTPBrushCache

template<>
AFX_INLINE BOOL AFXAPI CompareElements(const LOGBRUSH* logBrush1, const LOGBRUSH* logBrush2)
{
	ASSERT(NULL != logBrush1);
	ASSERT(NULL != logBrush2);

	return (logBrush1->lbStyle == logBrush2->lbStyle && logBrush1->lbColor == logBrush2->lbColor
			&& logBrush1->lbHatch == logBrush2->lbHatch);
}

template<>
UINT AFXAPI HashKey(const LOGBRUSH& logBrush)
{
	return QuickSequenceHash(reinterpret_cast<const UINT*>(&logBrush), sizeof(LOGBRUSH));
}

AFX_INLINE CXTPBrushCache* AFX_CDECL XTPBrushCache()
{
	return &CXTPSingleton<CXTPBrushCache>::Instance();
}

IMPLEMENT_DYNAMIC(CXTPBrushCache, CXTPGdiObjectCacheBase);

AFX_INLINE XTPGDIDESCRIPTOR& CXTPBrushCache::AccessDescriptorData(void* pDescriptor)
{
	ASSERT(NULL != pDescriptor);
	return *reinterpret_cast<XTPLOGBRUSHDESCRIPTOR*>(pDescriptor);
}

CXTPBrushCache::CXTPBrushCache()
{
}

CXTPBrushCache::~CXTPBrushCache()
{
	POSITION pos = m_brushCache.GetStartPosition();
	while (NULL != pos)
	{
		XTPCachedBrushReference* pBrushRef = NULL;
		LOGBRUSH logBrush;
		m_brushCache.GetNextAssoc(pos, logBrush, (LPVOID&)pBrushRef);
		pBrushRef->UnbindFromCache();
	}
}

CXTPBrushCache::XTPCachedBrushReference* CXTPBrushCache::CreateSolidBrush(COLORREF crColor)
{
	LOGBRUSH lbr;
	lbr.lbStyle = BS_SOLID;
	lbr.lbColor = crColor;
	lbr.lbHatch = 0;
	return CreateBrushIndirect(&lbr);
}

CXTPBrushCache::XTPCachedBrushReference* CXTPBrushCache::CreateHatchBrush(int nIndex,
																		  COLORREF crColor)
{
	LOGBRUSH lbr;
	lbr.lbStyle = BS_HATCHED;
	lbr.lbColor = crColor;
	lbr.lbHatch = static_cast<ULONG_PTR>(nIndex);
	return CreateBrushIndirect(&lbr);
}

CXTPBrushCache::XTPCachedBrushReference*
	CXTPBrushCache::CreateBrushIndirect(const LOGBRUSH* lpLogBrush)
{
	ASSERT(AfxIsValidAddress(lpLogBrush, sizeof(LOGFONT), FALSE));

	XTPCachedBrushReference* pBrushRef = NULL;
	XTP_GUARD_EXCLUSIVE(CXTPGdiObjectCacheBase, static_cast<CXTPGdiObjectCacheBase*>(this))
	{
		if (!m_brushCache.Lookup(*lpLogBrush, (LPVOID&)pBrushRef))
		{
			HBRUSH hBrush = ::CreateBrushIndirect(&*lpLogBrush);
			if (NULL != hBrush)
			{
				XTPLOGBRUSHDESCRIPTOR logBrushDescriptor;
				logBrushDescriptor.logBrush = *lpLogBrush;
				pBrushRef					= AllocateReferenceObject<XTPCachedBrushReference>();
#ifdef _DEBUG
#	pragma push_macro("new")
#	undef new
#endif
				new (pBrushRef) XTPCachedBrushReference(this, hBrush, logBrushDescriptor);
#ifdef _DEBUG
#	pragma pop_macro("new")
#endif

				m_brushCache.SetAt(*lpLogBrush, (LPVOID&)pBrushRef);
			}
		}
		else
		{
			pBrushRef->AddRef();

#ifdef _DEBUG
			// Make sure the cached font has not been altered.
			pBrushRef->AssertUnaltered();
#endif
		}
	}

	return pBrushRef;
}

CXTPBrushCache::XTPCachedBrushReference* CXTPBrushCache::CreatePatternBrush(CBitmap* pBitmap)
{
	ASSERT_VALID(pBitmap);

	XTPCachedBrushReference* pRef = NULL;
	HGDIOBJ hBmp				  = pBitmap->GetSafeHandle();
	if (NULL != hBmp)
	{
		LOGBRUSH lbr;
		lbr.lbStyle = BS_PATTERN;
		lbr.lbColor = 0;
		lbr.lbHatch = reinterpret_cast<ULONG_PTR>(hBmp);
		pRef		= CreateBrushIndirect(&lbr);
	}

	return pRef;
}

CXTPBrushCache::XTPCachedBrushReference* CXTPBrushCache::CreateDIBPatternBrush(HGLOBAL hPackedDIB,
																			   UINT nUsage)
{
	ASSERT(hPackedDIB != NULL);

	XTPCachedBrushReference* pRef = NULL;
	if (NULL != hPackedDIB)
	{
		const void* lpPackedDIB = ::GlobalLock(hPackedDIB);
		ASSERT(lpPackedDIB != NULL);

		if (NULL != lpPackedDIB)
		{
			pRef = CreateDIBPatternBrush(lpPackedDIB, nUsage);
		}

		::GlobalUnlock(hPackedDIB);
	}

	return pRef;
}

CXTPBrushCache::XTPCachedBrushReference*
	CXTPBrushCache::CreateDIBPatternBrush(const void* lpPackedDIB, UINT nUsage)
{
	ASSERT(NULL != lpPackedDIB);

	XTPCachedBrushReference* pRef = NULL;
	if (NULL != lpPackedDIB)
	{
		LOGBRUSH lbr;
		lbr.lbStyle = BS_DIBPATTERN;
		lbr.lbColor = nUsage;
		lbr.lbHatch = reinterpret_cast<ULONG_PTR>(lpPackedDIB);
		pRef		= CreateBrushIndirect(&lbr);
	}

	return pRef;
}

CXTPBrushCache::XTPCachedBrushReference* CXTPBrushCache::CreateSysColorBrush(int nIndex)
{
	COLORREF crColor = ::GetSysColor(nIndex);
	return CreateSolidBrush(crColor);
}

void CXTPBrushCache::RemoveFromCache(const void* pDescriptor)
{
	ASSERT(NULL != pDescriptor);

	XTP_GUARD_EXCLUSIVE(CXTPGdiObjectCacheBase, static_cast<CXTPGdiObjectCacheBase*>(this))
	{
		const XTPLOGBRUSHDESCRIPTOR* pLogBrushDescriptor =
			reinterpret_cast<const XTPLOGBRUSHDESCRIPTOR*>(pDescriptor);
		m_brushCache.RemoveKey(pLogBrushDescriptor->logBrush);
	}
}

///////////////////////////////////////////////////////////////////////////////
// CXTPBrush

IMPLEMENT_DYNAMIC(CXTPBrush, CXTPGdiObject);

CXTPBrush::CXTPBrush()
	: m_hLogBrushHandle(NULL)
{
	SetDummyGdiObject(m_dummyBrush);
}

CXTPBrush::CXTPBrush(COLORREF crColor)
	: m_hLogBrushHandle(NULL)
{
	SetDummyGdiObject(m_dummyBrush);
	CreateSolidBrush(crColor);
}

CXTPBrush::CXTPBrush(int nIndex, COLORREF crColor)
	: m_hLogBrushHandle(NULL)
{
	SetDummyGdiObject(m_dummyBrush);
	CreateHatchBrush(nIndex, crColor);
}

CXTPBrush::CXTPBrush(CBitmap* pBitmap)
	: m_hLogBrushHandle(NULL)
{
	SetDummyGdiObject(m_dummyBrush);
	CreatePatternBrush(pBitmap);
}

CXTPBrush::~CXTPBrush()
{
	DeleteObject();
}

BOOL CXTPBrush::CreateSolidBrush(COLORREF crColor)
{
	CXTPBrushCache::XTPCachedBrushReference* pCachedBrush = XTPBrushCache()->CreateSolidBrush(
		crColor);
	SetCachedObject(pCachedBrush);
	return NULL != pCachedBrush;
}

BOOL CXTPBrush::CreateHatchBrush(int nIndex, COLORREF crColor)
{
	CXTPBrushCache::XTPCachedBrushReference* pCachedBrush =
		XTPBrushCache()->CreateHatchBrush(nIndex, crColor);
	SetCachedObject(pCachedBrush);
	return NULL != pCachedBrush;
}

BOOL CXTPBrush::CreateBrushIndirect(const LOGBRUSH* lpLogBrush)
{
	CXTPBrushCache::XTPCachedBrushReference* pCachedBrush = XTPBrushCache()->CreateBrushIndirect(
		lpLogBrush);
	SetCachedObject(pCachedBrush);
	return NULL != pCachedBrush;
}

BOOL CXTPBrush::CreatePatternBrush(CBitmap* pBitmap)
{
	CXTPBrushCache::XTPCachedBrushReference* pCachedBrush = XTPBrushCache()->CreatePatternBrush(
		pBitmap);
	SetCachedObject(pCachedBrush);
	return NULL != pCachedBrush;
}

BOOL CXTPBrush::CreateDIBPatternBrush(HGLOBAL hPackedDIB, UINT nUsage)
{
	CXTPBrushCache::XTPCachedBrushReference* pCachedBrush =
		XTPBrushCache()->CreateDIBPatternBrush(hPackedDIB, nUsage);
	SetCachedObject(pCachedBrush);
	return NULL != pCachedBrush;
}

BOOL CXTPBrush::CreateDIBPatternBrush(const void* lpPackedDIB, UINT nUsage)
{
	CXTPBrushCache::XTPCachedBrushReference* pCachedBrush =
		XTPBrushCache()->CreateDIBPatternBrush(lpPackedDIB, nUsage);
	SetCachedObject(pCachedBrush);
	return NULL != pCachedBrush;
}

BOOL CXTPBrush::CreateSysColorBrush(int nIndex)
{
	CXTPBrushCache::XTPCachedBrushReference* pCachedBrush = XTPBrushCache()->CreateSysColorBrush(
		nIndex);
	SetCachedObject(pCachedBrush);
	return NULL != pCachedBrush;
}

int CXTPBrush::GetLogBrush(LOGBRUSH* pLogBrush)
{
	int nResult = 0;

	HGDIOBJ hCurrent = GetSafeHandle();
	if (hCurrent == m_hLogBrushHandle && NULL != m_hLogBrushHandle)
	{
		if (XTP_ASSERT_CHECK(NULL != pLogBrush))
		{
			*pLogBrush = m_logBrush;
			nResult	   = sizeof(m_logBrush);
		}
	}
	else
	{
		nResult = GetNativeGdiObject()->GetLogBrush(pLogBrush);
		if (0 != nResult)
		{
			m_hLogBrushHandle = hCurrent;
			m_logBrush		  = *pLogBrush;
		}
	}

	return nResult;
}

XTPGDIDESCRIPTOR& CXTPBrush::AccessDescriptorData()
{
	void* pDescriptor = GetDescriptor();
	ASSERT("No handle is attached, descriptor data unavailable" && NULL != pDescriptor);

	return CXTPBrushCache::AccessDescriptorData(pDescriptor);
}

///////////////////////////////////////////////////////////////////////////////
// CXTPPenCache

AFX_INLINE UINT ComputeExtLogPenSize(UINT nStyleCount)
{
	return sizeof(EXTLOGPEN) + sizeof(DWORD) * (__min(nStyleCount, 1) - 1);
}

struct XTPPENINFO : XTPGDIDESCRIPTOR
{
	BOOL bLogPen;
	union
	{
		LOGPEN logPen;
		LPEXTLOGPEN pExtLogPen;
	};

	XTPPENINFO()
		: bLogPen(FALSE)
	{
		memset(&logPen, 0, sizeof(logPen));
	}

	void Release();
	void Set(const LOGPEN& logPen);
	void Set(int nPenStyle, int nWidth, const LOGBRUSH* pLogBrush, int nStyleCount = 0,
			 const DWORD* lpStyle = NULL);
};

void XTPPENINFO::Release()
{
	if (!bLogPen && NULL != pExtLogPen)
	{
		free(pExtLogPen);
		pExtLogPen = NULL;
	}
}

void XTPPENINFO::Set(const LOGPEN& lpen)
{
	Release();
	bLogPen = TRUE;
	logPen	= lpen;
}

void XTPPENINFO::Set(int nPenStyle, int nWidth, const LOGBRUSH* pLogBrush, int nStyleCount /*= 0*/,
					 const DWORD* lpStyle /*= NULL*/)
{
	ASSERT(NULL != pLogBrush);
	ASSERT(0 < nStyleCount ? NULL != nStyleCount : TRUE);

	Release();
	bLogPen = FALSE;

	UINT cbStruct			  = ComputeExtLogPenSize(XTPToUIntChecked(nStyleCount));
	pExtLogPen				  = reinterpret_cast<LPEXTLOGPEN>(malloc(cbStruct));
	pExtLogPen->elpPenStyle	  = static_cast<DWORD>(nPenStyle);
	pExtLogPen->elpWidth	  = static_cast<DWORD>(nWidth);
	pExtLogPen->elpBrushStyle = pLogBrush->lbStyle;
	pExtLogPen->elpColor	  = pLogBrush->lbColor;
	pExtLogPen->elpHatch	  = pLogBrush->lbHatch;
	pExtLogPen->elpNumEntries = static_cast<DWORD>(nStyleCount);
	for (int i = 0; i < nStyleCount; ++i)
	{
		pExtLogPen->elpStyleEntry[i] = lpStyle[i];
	}
}

class CXTPCachedPenReference : public CXTPCachedGdiObjectReference<XTPPENINFO*, CPen, CXTPPenCache>
{
public:
	CXTPCachedPenReference(CXTPGdiObjectCacheBase* pCache, HGDIOBJ hObject, XTPPENINFO* pPenInfo)
		: CXTPCachedGdiObjectReference<XTPPENINFO*, CPen, CXTPPenCache>(pCache, hObject, pPenInfo)
	{
	}

	~CXTPCachedPenReference()
	{
		m_descriptor->Release();
		delete m_descriptor;
	}
};

template<>
AFX_INLINE BOOL AFXAPI CompareElements(XTPPENINFO* const* pPenInfo1, XTPPENINFO* const* pPenInfo2)
{
	ASSERT(NULL != pPenInfo1 && NULL != *pPenInfo1);
	ASSERT(NULL != pPenInfo2 && NULL != *pPenInfo2);

	const XTPPENINFO& penInfo1 = **pPenInfo1;
	const XTPPENINFO& penInfo2 = **pPenInfo2;

	BOOL bEqual = FALSE;
	if (penInfo1.bLogPen == penInfo2.bLogPen)
	{
		if (penInfo1.bLogPen)
		{
			bEqual = (penInfo1.logPen.lopnStyle == penInfo2.logPen.lopnStyle
					  && penInfo1.logPen.lopnWidth.x == penInfo2.logPen.lopnWidth.x
					  && penInfo1.logPen.lopnColor == penInfo2.logPen.lopnColor);
		}
		else if (penInfo1.pExtLogPen->elpNumEntries == penInfo2.pExtLogPen->elpNumEntries)
		{
			UINT cbStruct = ComputeExtLogPenSize(penInfo1.pExtLogPen->elpNumEntries);
			bEqual		  = (0 == memcmp(penInfo1.pExtLogPen, penInfo2.pExtLogPen, cbStruct));
		}
	}

	return bEqual;
}

template<>
UINT AFXAPI HashKey(XTPPENINFO* pPenInfo)
{
	ASSERT(NULL != pPenInfo);

	UINT nHash = 0;

	if (pPenInfo->bLogPen)
	{
		nHash = QuickSequenceHash(reinterpret_cast<const UINT*>(&pPenInfo->logPen), sizeof(LOGPEN));
	}
	else
	{
		UINT cbStruct = ComputeExtLogPenSize(pPenInfo->pExtLogPen->elpNumEntries);
		nHash = QuickSequenceHash(reinterpret_cast<const UINT*>(pPenInfo->pExtLogPen), cbStruct);
	}

	return nHash;
}

AFX_INLINE CXTPPenCache* AFX_CDECL XTPPenCache()
{
	return &CXTPSingleton<CXTPPenCache>::Instance();
}

IMPLEMENT_DYNAMIC(CXTPPenCache, CXTPGdiObjectCacheBase);

AFX_INLINE XTPGDIDESCRIPTOR& CXTPPenCache::AccessDescriptorData(void* pDescriptor)
{
	ASSERT(NULL != pDescriptor);
	return **reinterpret_cast<XTPPENINFO**>(pDescriptor);
}

CXTPPenCache::CXTPPenCache()
{
}

CXTPPenCache::~CXTPPenCache()
{
	POSITION pos = m_penCache.GetStartPosition();
	while (NULL != pos)
	{
		CXTPCachedPenReference* pPenRef = NULL;
		XTPPENINFO* pPenInfo;
		m_penCache.GetNextAssoc(pos, pPenInfo, (LPVOID&)pPenRef);
		pPenRef->UnbindFromCache();
	}
}

CXTPCachedPenReference* CXTPPenCache::CreatePen(int nPenStyle, int nWidth, COLORREF crColor)
{
	LOGPEN logPen;
	logPen.lopnStyle   = XTPToUIntChecked(nPenStyle);
	logPen.lopnWidth.x = nWidth;
	logPen.lopnWidth.y = 0; // not used
	logPen.lopnColor   = crColor;
	return CreatePenIndirect(&logPen);
}

CXTPCachedPenReference* CXTPPenCache::CreatePen(int nPenStyle, int nWidth,
												const LOGBRUSH* pLogBrush, int nStyleCount /*= 0*/,
												const DWORD* lpStyle /*= NULL*/)
{
	CXTPCachedPenReference* pPenRef = NULL;
	XTP_GUARD_EXCLUSIVE(CXTPGdiObjectCacheBase, static_cast<CXTPGdiObjectCacheBase*>(this))
	{
		XTPPENINFO penInfo;
		penInfo.Set(nPenStyle, nWidth, pLogBrush, nStyleCount, lpStyle);
		if (!m_penCache.Lookup(&penInfo, (LPVOID&)pPenRef))
		{
			HPEN hPen = ::ExtCreatePen(static_cast<DWORD>(nPenStyle), static_cast<DWORD>(nWidth),
									   pLogBrush, static_cast<DWORD>(nStyleCount), lpStyle);
			if (NULL != hPen)
			{
				XTPPENINFO* pPenInfo = new XTPPENINFO(penInfo);
				pPenRef				 = AllocateReferenceObject<CXTPCachedPenReference>();
#ifdef _DEBUG
#	pragma push_macro("new")
#	undef new
#endif
				new (pPenRef) CXTPCachedPenReference(this, hPen, pPenInfo);
#ifdef _DEBUG
#	pragma pop_macro("new")
#endif
				m_penCache.SetAt(pPenInfo, (LPVOID&)pPenRef);
			}
			else
			{
				penInfo.Release();
			}
		}
		else
		{
			penInfo.Release();
			pPenRef->AddRef();

#ifdef _DEBUG
			// Make sure the cached font has not been altered.
			pPenRef->AssertUnaltered();
#endif
		}
	}

	return pPenRef;
}

CXTPCachedPenReference* CXTPPenCache::CreatePenIndirect(LPLOGPEN lpLogPen)
{
	ASSERT(AfxIsValidAddress(lpLogPen, sizeof(LOGPEN), FALSE));

	CXTPCachedPenReference* pPenRef = NULL;
	XTP_GUARD_EXCLUSIVE(CXTPGdiObjectCacheBase, static_cast<CXTPGdiObjectCacheBase*>(this))
	{
		XTPPENINFO penInfo;
		penInfo.Set(*lpLogPen);
		if (!m_penCache.Lookup(&penInfo, (LPVOID&)pPenRef))
		{
			HPEN hPen = ::CreatePenIndirect(lpLogPen);
			if (NULL != hPen)
			{
				XTPPENINFO* pPenInfo = new XTPPENINFO(penInfo);
				pPenRef				 = AllocateReferenceObject<CXTPCachedPenReference>();
#ifdef _DEBUG
#	pragma push_macro("new")
#	undef new
#endif
				new (pPenRef) CXTPCachedPenReference(this, hPen, pPenInfo);
#ifdef _DEBUG
#	pragma pop_macro("new")
#endif
				m_penCache.SetAt(pPenInfo, (LPVOID&)pPenRef);
			}
		}
		else
		{
			pPenRef->AddRef();

#ifdef _DEBUG
			// Make sure the cached font has not been altered.
			pPenRef->AssertUnaltered();
#endif
		}
	}

	return pPenRef;
}

void CXTPPenCache::RemoveFromCache(const void* pDescriptor)
{
	ASSERT(NULL != pDescriptor);

	XTP_GUARD_EXCLUSIVE(CXTPGdiObjectCacheBase, static_cast<CXTPGdiObjectCacheBase*>(this))
	{
		XTPPENINFO** pPenInfo = reinterpret_cast<XTPPENINFO**>(const_cast<void*>(pDescriptor));
		m_penCache.RemoveKey(*pPenInfo);
	}
}

///////////////////////////////////////////////////////////////////////////////
// CXTPPen

IMPLEMENT_DYNAMIC(CXTPPen, CXTPGdiObject);

CXTPPen::CXTPPen()
	: m_hLogPenHandle(NULL)
{
	SetDummyGdiObject(m_dummyPen);
}

CXTPPen::CXTPPen(int nPenStyle, int nWidth, COLORREF crColor)
	: m_hLogPenHandle(NULL)
{
	SetDummyGdiObject(m_dummyPen);
	CreatePen(nPenStyle, nWidth, crColor);
}

CXTPPen::CXTPPen(int nPenStyle, int nWidth, const LOGBRUSH* pLogBrush, int nStyleCount /*= 0*/,
				 const DWORD* lpStyle /*= NULL*/)
	: m_hLogPenHandle(NULL)
{
	SetDummyGdiObject(m_dummyPen);
	CreatePen(nPenStyle, nWidth, pLogBrush, nStyleCount, lpStyle);
}

CXTPPen::~CXTPPen()
{
	DeleteObject();
}

BOOL CXTPPen::CreatePen(int nPenStyle, int nWidth, COLORREF crColor)
{
	CXTPCachedPenReference* pCachedPen = XTPPenCache()->CreatePen(nPenStyle, nWidth, crColor);
	SetCachedObject(pCachedPen);
	return NULL != pCachedPen;
}

BOOL CXTPPen::CreatePen(int nPenStyle, int nWidth, const LOGBRUSH* pLogBrush,
						int nStyleCount /*= 0*/, const DWORD* lpStyle /*= NULL*/)
{
	CXTPCachedPenReference* pCachedPen = XTPPenCache()->CreatePen(nPenStyle, nWidth, pLogBrush,
																  nStyleCount, lpStyle);
	SetCachedObject(pCachedPen);
	return NULL != pCachedPen;
}

BOOL CXTPPen::CreatePenIndirect(LPLOGPEN lpLogPen)
{
	CXTPCachedPenReference* pCachedPen = XTPPenCache()->CreatePenIndirect(lpLogPen);
	SetCachedObject(pCachedPen);
	return NULL != pCachedPen;
}

int CXTPPen::GetLogPen(LOGPEN* pLogPen)
{
	int nResult = 0;

	HGDIOBJ hCurrent = GetSafeHandle();
	if (hCurrent == m_hLogPenHandle && NULL != m_hLogPenHandle)
	{
		if (XTP_ASSERT_CHECK(NULL != pLogPen))
		{
			*pLogPen = m_logPen;
			nResult	 = sizeof(m_logPen);
		}
	}
	else
	{
		nResult = GetNativeGdiObject()->GetLogPen(pLogPen);
		if (0 != nResult)
		{
			m_hLogPenHandle = hCurrent;
			m_logPen		= *pLogPen;
		}
	}

	return nResult;
}

int CXTPPen::GetExtLogPen(EXTLOGPEN* pLogPen)
{
	return GetNativeGdiObject()->GetExtLogPen(pLogPen);
}

XTPGDIDESCRIPTOR& CXTPPen::AccessDescriptorData()
{
	void* pDescriptor = GetDescriptor();
	ASSERT("No handle is attached, descriptor data unavailable" && NULL != pDescriptor);

	return CXTPPenCache::AccessDescriptorData(pDescriptor);
}
