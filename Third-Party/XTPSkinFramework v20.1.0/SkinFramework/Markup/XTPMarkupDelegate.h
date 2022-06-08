// XTPMarkupDelegate.h: interface for the CXTPMarkupDelegate class.
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
#if !defined(__XTPMARKUPDELEGATE_H__)
#	define __XTPMARKUPDELEGATE_H__
//}}AFX_CODEJOCK_PRIVATE

#	if _MSC_VER > 1000
#		pragma once
#	endif // _MSC_VER > 1000

#	include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

class CXTPMarkupRoutedEventArgs;

class _XTP_EXT_CLASS CXTPMarkupDelegate : public CXTPMarkupObject
{
public:
	CXTPMarkupDelegate()
		: CXTPMarkupObject(CXTPMarkupObject::flagNoCom)
	{
	}

	virtual void Execute(CXTPMarkupObject* /*pSender*/, CXTPMarkupRoutedEventArgs* /*pArgs*/)
	{
		ASSERT(FALSE);
	}
};

template<class T>
class CXTPMarkupClassDelegate : public CXTPMarkupDelegate
{
public:
	typedef void (T::*ROUTEDEVENTHANDLER)(CXTPMarkupObject* pSender,
										  CXTPMarkupRoutedEventArgs* pArgs);

public:
	CXTPMarkupClassDelegate(T* pObject, ROUTEDEVENTHANDLER pHandler)
	{
		m_pObject  = pObject;
		m_pHandler = pHandler;
	}

	virtual void Execute(CXTPMarkupObject* pSender, CXTPMarkupRoutedEventArgs* pArgs)
	{
		(m_pObject->*m_pHandler)(pSender, pArgs);
	}

protected:
	T* m_pObject;
	ROUTEDEVENTHANDLER m_pHandler;
};

class _XTP_EXT_CLASS CXTPMarkupFunctionDelegate : public CXTPMarkupDelegate
{
public:
	typedef void(AFX_CDECL* ROUTEDEVENTHANDLER)(CXTPMarkupObject* pSender,
												CXTPMarkupRoutedEventArgs* pArgs);

public:
	CXTPMarkupFunctionDelegate(ROUTEDEVENTHANDLER pHandler)
	{
		m_pHandler = pHandler;
	}

	virtual void Execute(CXTPMarkupObject* pSender, CXTPMarkupRoutedEventArgs* pArgs)
	{
		(*m_pHandler)(pSender, pArgs);
	}

protected:
	ROUTEDEVENTHANDLER m_pHandler;
};

template<class T, typename EVENTHANDLER>
AFX_INLINE CXTPMarkupDelegate* CreateMarkupClassDelegate(T* pClass, EVENTHANDLER pfnDelegate)
{
	return new CXTPMarkupClassDelegate<T>(
		pClass, (typename CXTPMarkupClassDelegate<T>::ROUTEDEVENTHANDLER)pfnDelegate);
}

template<typename EVENTHANDLER>
AFX_INLINE CXTPMarkupDelegate* CreateMarkupFunctionDelegate(EVENTHANDLER pfnDelegate)
{
	return new CXTPMarkupFunctionDelegate(
		(CXTPMarkupFunctionDelegate::ROUTEDEVENTHANDLER)pfnDelegate);
}

#	include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPMARKUPDELEGATE_H__)
