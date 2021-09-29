// XTPTypeId.h:
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
#if !defined(_XTPTYPEID_H_)
#	define _XTPTYPEID_H_
//}}AFX_CODEJOCK_PRIVATE

#	if _MSC_VER > 1000
#		pragma once
#	endif // _MSC_VER > 1000

#	include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

class CXTPTypeId
{
public:
	explicit CXTPTypeId(DWORD_PTR id);
	CXTPTypeId(const CXTPTypeId& rhs);

	BOOL operator==(const CXTPTypeId& rhs) const;
	BOOL operator!=(const CXTPTypeId& rhs) const;

private:
	DWORD_PTR m_id;
};

AFX_INLINE CXTPTypeId::CXTPTypeId(DWORD_PTR id)
	: m_id(id)
{
}

AFX_INLINE CXTPTypeId::CXTPTypeId(const CXTPTypeId& rhs)
	: m_id(rhs.m_id)
{
}

AFX_INLINE BOOL CXTPTypeId::operator==(const CXTPTypeId& rhs) const
{
	return m_id == rhs.m_id;
}

AFX_INLINE BOOL CXTPTypeId::operator!=(const CXTPTypeId& rhs) const
{
	return m_id != rhs.m_id;
}

template<class This, class Parent>
class CXTPTypeIdProvider;

class _XTP_EXT_CLASS CXTPTypeIdProviderBase
{
#	if _MSC_VER > 1200
protected:
	template<class T, class U>
	friend class CXTPTypeIdProvider;
#	else
public:
#	endif

	struct DerivedList
	{
		DerivedList(CXTPTypeId derivedTypeId, CXTPTypeIdProviderBase* pTypeProvider,
					void* pInstance, DerivedList* pFLink);

		CXTPTypeId derivedTypeId;
		CXTPTypeIdProviderBase* pTypeProvider;
		void* pInstance;
		DerivedList* pFLink;
	};

	explicit CXTPTypeIdProviderBase(CXTPTypeId typeId);
	virtual ~CXTPTypeIdProviderBase();

	virtual CXTPTypeId GetTypeId() const;
	void AddDerived(CXTPTypeId typeId, CXTPTypeIdProviderBase* pTypeProvider, void* pInstance);

	template<class T>
	AFX_INLINE T* TryCast() const;

	void* TryCastTo(CXTPTypeId typeId) const;

	const CXTPTypeId m_typeId;
	DerivedList* m_pDerivedList;
};

template<class This, class Parent = This>
class CXTPTypeIdProvider
	:
#	if _MSC_VER > 1200
	private CXTPTypeIdProviderBase
{
	template<class T, class U>
	friend class CXTPTypeIdProvider;

protected:
#	else
	public CXTPTypeIdProviderBase
{
public:
#	endif

	typedef CXTPTypeIdProvider<This, Parent> XTPTypeIdProvider;

	CXTPTypeIdProvider();

public:
	using CXTPTypeIdProviderBase::GetTypeId;
	using CXTPTypeIdProviderBase::TryCastTo;
};

template<class T>
class CXTPTypeIdOf
{
public:
	static CXTPTypeId AFX_CDECL Id();
};

template<class T>
AFX_INLINE T* CXTPTypeIdProviderBase::TryCast() const
{
	return reinterpret_cast<T*>(TryCastTo(CXTPTypeIdOf<T>::Id()));
}

template<class This, class Parent>
AFX_INLINE CXTPTypeIdProvider<This, Parent>::CXTPTypeIdProvider()
	: CXTPTypeIdProviderBase(CXTPTypeIdOf<This>::Id())
{
	typename Parent::XTPTypeIdProvider* pParent = static_cast<typename Parent::XTPTypeIdProvider*>(
		static_cast<This*>(this));
	pParent->AddDerived(GetTypeId(), this, static_cast<This*>(this));
}

template<class T>
AFX_INLINE CXTPTypeId AFX_CDECL CXTPTypeIdOf<T>::Id()
{
	static const CXTPTypeId id(reinterpret_cast<DWORD_PTR>(&CXTPTypeIdOf<T>::Id));
	return id;
}

#	include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(_XTPTYPEID_H_)
