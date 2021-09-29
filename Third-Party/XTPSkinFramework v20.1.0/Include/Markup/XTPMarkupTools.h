// XTPMarkupTools.h: markup helpers.
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
#if !defined(__XTPMARKUPTOOLS_H__)
#	define __XTPMARKUPTOOLS_H__
//}}AFX_CODEJOCK_PRIVATE

#	if _MSC_VER > 1000
#		pragma once
#	endif // _MSC_VER > 1000

#	include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

#	ifndef _XTP_ACTIVEX

#		ifndef DECLARE_ENUM_VARIANT
#			define DECLARE_ENUM_VARIANT(theClass)                                                 \
				afx_msg LPUNKNOWN OleNewEnum();                                                    \
				friend class CEnumVariantArray<theClass>;
#		endif /*!DECLARE_ENUM_VARIANT*/

#		ifndef IMPLEMENT_ENUM_VARIANT
#			define IMPLEMENT_ENUM_VARIANT(theClass)                                               \
				LPUNKNOWN theClass::OleNewEnum()                                                   \
				{                                                                                  \
					CEnumVariantArray<theClass>* pEnumVariant = new CEnumVariantArray<theClass>(   \
						this);                                                                     \
					return (LPUNKNOWN)pEnumVariant;                                                \
				}
#		endif /*!IMPLEMENT_ENUM_VARIANT*/

AFX_INLINE LPDISPATCH XTPGetDispatch(CCmdTarget* pCmdTarget, BOOL bAddRef = TRUE)
{
	LPDISPATCH pDisp = NULL;
	if (NULL != pCmdTarget)
	{
		pDisp = pCmdTarget->GetIDispatch(bAddRef);
	}

	return pDisp;
}

#		pragma warning(push)
// C4616: #pragma warning : warning number '...' out of range, must be between '4001' and '4999'
// C4619: #pragma warning : there is no warning number 'number'
// C5204: '...': class has virtual functions, but its trivial destructor is not virtual; instances
// of objects derived from this class may not be destructed correctly
#		pragma warning(disable : 4616 4619 5204)

template<class T>
class CEnumVariantArray : public IEnumVARIANT
{
public:
	CEnumVariantArray(T* pEnumable)
	{
		m_nCurrent	= 0;
		m_pEnumable = pEnumable;
		m_dwRef		= 1;
	}

	STDMETHOD_(ULONG, AddRef)();
	STDMETHOD_(ULONG, Release)();
	STDMETHOD(QueryInterface)(REFIID iid, LPVOID* ppvObj);

	HRESULT STDMETHODCALLTYPE Next(ULONG celt, VARIANT* rgVar, ULONG* pCeltFetched);

	HRESULT STDMETHODCALLTYPE Skip(ULONG celt);

	HRESULT STDMETHODCALLTYPE Reset();

	HRESULT STDMETHODCALLTYPE Clone(IEnumVARIANT** ppEnum);

	int m_nCurrent;
	T* m_pEnumable;
	long m_dwRef;
};

#		pragma warning(pop)

template<class T>
inline ULONG FAR EXPORT CEnumVariantArray<T>::AddRef()
{
	return XTPToULongChecked(InterlockedIncrement(&m_dwRef));
}

template<class T>
inline ULONG FAR EXPORT CEnumVariantArray<T>::Release()
{
	if (m_dwRef == 0)
		return 0;

	LONG lResult = InterlockedDecrement(&m_dwRef);
	if (lResult == 0)
	{
		delete this;
	}
	return XTPToULongChecked(lResult);
}

template<class T>
inline HRESULT FAR EXPORT CEnumVariantArray<T>::QueryInterface(REFIID iid, void FAR* FAR* ppvObj)
{
	if (iid == IID_IUnknown || iid == IID_IEnumVARIANT)
	{
		*ppvObj = this;
		AddRef();
		return S_OK;
	}

	*ppvObj = NULL;
	return E_NOINTERFACE;
}

template<class T>
inline HRESULT STDMETHODCALLTYPE CEnumVariantArray<T>::Next(ULONG celt, VARIANT* rgVar,
															ULONG* pceltFetched)
{
	AFX_MANAGE_STATE(m_pEnumable->m_pModuleState)

	ULONG nFetched = 0;
	while (nFetched < celt && m_nCurrent < m_pEnumable->OleGetItemCount())
	{
		rgVar[nFetched].vt		 = VT_DISPATCH;
		rgVar[nFetched].pdispVal = m_pEnumable->OleGetItem(m_nCurrent);
		nFetched++;
		m_nCurrent++;
	}

	if (pceltFetched)
		*pceltFetched = nFetched;
	return nFetched == celt ? S_OK : S_FALSE;
}

template<class T>
inline HRESULT STDMETHODCALLTYPE CEnumVariantArray<T>::Skip(ULONG celt)
{
	if (m_nCurrent + (int)celt < m_pEnumable->OleGetItemCount())
	{
		m_nCurrent += celt;
		return S_OK;
	}
	m_nCurrent = m_pEnumable->OleGetItemCount() - 1;
	return S_FALSE;
}

template<class T>
inline HRESULT STDMETHODCALLTYPE CEnumVariantArray<T>::Reset()
{
	m_nCurrent = 0;
	return S_OK;
}

template<class T>
inline HRESULT STDMETHODCALLTYPE CEnumVariantArray<T>::Clone(IEnumVARIANT** ppEnum)
{
	CEnumVariantArray<T>* pEnumVariant = new CEnumVariantArray<T>(m_pEnumable);
	pEnumVariant->m_nCurrent		   = m_nCurrent;

	*ppEnum = pEnumVariant;
	return S_OK;
}

#	endif /*!_XTP_ACTIVEX*/

#	include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // #if !defined(__XTPMARKUPTOOLS_H__)
