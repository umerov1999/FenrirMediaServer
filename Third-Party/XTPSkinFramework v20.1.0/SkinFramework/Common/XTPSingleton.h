// XTPSingleton.h: CXTPSingleton template class definition.
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
#if !defined(__XTPSINGLETON_H__)
#	define __XTPSINGLETON_H__
//}}AFX_CODEJOCK_PRIVATE

#	if _MSC_VER > 1000
#		pragma once
#	endif // _MSC_VER > 1000

#	include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

#	ifndef __XTPSYSTEMHELPERS_H__
class CXTPDummySingletonDependency;

template<
	class Singleton1 = CXTPDummySingletonDependency,
	class Singleton2 = CXTPDummySingletonDependency, class Singleton3 = CXTPDummySingletonDependency,
	class Singleton4 = CXTPDummySingletonDependency, class Singleton5 = CXTPDummySingletonDependency,
	class Singleton6 = CXTPDummySingletonDependency, class Singleton7 = CXTPDummySingletonDependency,
	class Singleton8 = CXTPDummySingletonDependency, class Singleton9 = CXTPDummySingletonDependency,
	class Singleton10 = CXTPDummySingletonDependency>
class CXTPSingletonDependencies;

template<class T, class Dependencies = CXTPSingletonDependencies<> >
class CXTPSingleton;
#	endif /*__XTPSYSTEMHELPERS_H__*/

//{{AFX_CODEJOCK_PRIVATE

//===========================================================================
// Summary: Implements Singleton design pattern for a pointer of unspecified
//          type that is to be allocated, created and destroyed by a client.
//          Respects thread safety for instance allocation.
//===========================================================================
class _XTP_EXT_CLASS CXTPSingletonPointer : IXTPApplicationEvents
{
public:
	// Creator callback type.
	typedef BOOL(AFX_CDECL* Creator)(void*);

	// Destroyer callback type.
	typedef void(AFX_CDECL* Destroyer)(void*);

	// Initializer callback type.
	typedef void(AFX_CDECL* Initializer)();

	//-----------------------------------------------------------------------
	// Summary: Handles object construction.
	// Parameters:
	//          cbObjectSize - Singleton object size in bytes.
	//          pfnCreator - Creater callback function pointer.
	//          pfnDestroyer - Destroyer callback function pointer.
	//          pfnDestroyer - Initializer callback function pointer.
	//-----------------------------------------------------------------------
	CXTPSingletonPointer(SIZE_T cbObjectSize, Creator pfnCreator, Destroyer pfnDestroyer,
						 Initializer pfnInitializer = NULL);

	//-----------------------------------------------------------------------
	// Summary: Handles object destruction.
	//-----------------------------------------------------------------------
	virtual ~CXTPSingletonPointer();

	//-----------------------------------------------------------------------
	// Summary: Obtains encapsulated singleton instance pointer. The instance is
	//          allocated and created during the first call. The function guaranties
	//          returning a valid pointer value. If it cannot return it an exception
	//          will be issued.
	// Returns: A pointer to singleton instance.
	//-----------------------------------------------------------------------
	void* GetInstance();

	//-----------------------------------------------------------------------
	// Summary: Destroys a previously created and allocated instance and
	//          invalidate the instance pointer.
	//-----------------------------------------------------------------------
	void Destroy();

	//-----------------------------------------------------------------------
	// Summary: Obtains reference to singleton access critical secton.
	// Returns: A reference to singleton access critical secton.
	//-----------------------------------------------------------------------
	::CRITICAL_SECTION& GetAccess() const
	{
		return m_access;
	}

private:
	virtual void OnApplicationShutdown(CXTPApplication* pApplication);

private:
	void* m_pInstance;
	mutable ::CRITICAL_SECTION m_access;
	const SIZE_T m_cbObjectSize;
	const Creator m_pfnCreator;
	const Destroyer m_pfnDestroyer;
	BOOL m_bDestroyed;
};

class CXTPDummySingletonDependency
{
};

template<class Singleton>
AFX_INLINE void XTPInitSingletonInstance(Singleton*)
{
	Singleton::Instance();
}

AFX_INLINE void XTPInitSingletonInstance(CXTPDummySingletonDependency*)
{
	// Do nothing.
}

//}}AFX_CODEJOCK_PRIVATE

//=======================================================================
// Summary:
//      Describes the list of up to 10 optional singleton dependencies.
//      If specified, the dependencies gets initialized prior to the
//      singleton instance initialization in the order of listing.
// Parameters:
//      Singleton1 - Optional 1st dependency.
//      Singleton2 - Optional 2nd dependency.
//      Singleton3 - Optional 3rd dependency.
//      Singleton4 - Optional 4th dependency.
//      Singleton5 - Optional 5th dependency.
//      Singleton6 - Optional 6th dependency.
//      Singleton7 - Optional 7th dependency.
//      Singleton8 - Optional 8th dependency.
//      Singleton9 - Optional 9th dependency.
//      Singleton10 - Optional 10th dependency.
// See also:
//      CXTPSingleton
//=======================================================================
template<class Singleton1 /*= CXTPDummySingletonDependency*/,
		 class Singleton2 /*= CXTPDummySingletonDependency*/,
		 class Singleton3 /*= CXTPDummySingletonDependency*/,
		 class Singleton4 /*= CXTPDummySingletonDependency*/,
		 class Singleton5 /*= CXTPDummySingletonDependency*/,
		 class Singleton6 /*= CXTPDummySingletonDependency*/,
		 class Singleton7 /*= CXTPDummySingletonDependency*/,
		 class Singleton8 /*= CXTPDummySingletonDependency*/,
		 class Singleton9 /*= CXTPDummySingletonDependency*/,
		 class Singleton10 /*= CXTPDummySingletonDependency*/>
class CXTPSingletonDependencies
{
public:
	//{{AFX_CODEJOCK_PRIVATE
	static void AFX_CDECL InitDependencies()
	{
		// Template overloading approach doesn't work in VC6,
		// that's why function overloading with dummy pointers is used.
		XTPInitSingletonInstance(reinterpret_cast<Singleton1*>(NULL));
		XTPInitSingletonInstance(reinterpret_cast<Singleton2*>(NULL));
		XTPInitSingletonInstance(reinterpret_cast<Singleton3*>(NULL));
		XTPInitSingletonInstance(reinterpret_cast<Singleton4*>(NULL));
		XTPInitSingletonInstance(reinterpret_cast<Singleton5*>(NULL));
		XTPInitSingletonInstance(reinterpret_cast<Singleton6*>(NULL));
		XTPInitSingletonInstance(reinterpret_cast<Singleton7*>(NULL));
		XTPInitSingletonInstance(reinterpret_cast<Singleton8*>(NULL));
		XTPInitSingletonInstance(reinterpret_cast<Singleton9*>(NULL));
		XTPInitSingletonInstance(reinterpret_cast<Singleton10*>(NULL));
	}
	//}}AFX_CODEJOCK_PRIVATE
};

//===========================================================================
// Summary: Implements Singleton design pattern. Can be used both as an
//          adaptor or a base class for a class the is not derived from
//          any other class. Respects thread safety for instance allocation.
// Parameters:
//          T - Type name of singleton instance.
//          Dependencies - Optional list of singleton types listed as
//                         CXTPSingletonDependencies template arguments
//                         on which the current signleton type depends.
//                         If not initialized any of the listed dependencies
//                         will be initialized in the order of appearance in
//                         the list before this singleton instance is created.
//===========================================================================
template<class T, class Dependencies /*= CXTPSingletonDependencies<>*/>
class CXTPSingleton
{
public:
	//-----------------------------------------------------------------------
	// Summary: Obtains encapsulated singleton instance reference. The instance is
	//          allocated and created during the first call.
	// Returns: A reference to singleton instance.
	//-----------------------------------------------------------------------
	static T& AFX_CDECL Instance()
	{
		void* pInstance = InstancePointer().GetInstance();
		return *reinterpret_cast<T*>(pInstance);
	}

private:
	static BOOL AFX_CDECL Create(void* ptr)
	{
		new (ptr) T();
		return TRUE;
	}

	static void AFX_CDECL Destroy(void* ptr)
	{
		reinterpret_cast<T*>(ptr)->T::~T();
	}

protected:
	//{{AFX_CODEJOCK_PRIVATE

	CXTPSingleton()
	{
	}

	static ::CRITICAL_SECTION& AFX_CDECL GetInstanceAccess()
	{
		return InstancePointer().GetAccess();
	}

	static CXTPSingletonPointer& AFX_CDECL InstancePointer()
	{
		static CXTPSingletonPointer instanceHolder(sizeof(T), Create, Destroy,
												   Dependencies::InitDependencies);
		return instanceHolder;
	}

	//}}AFX_CODEJOCK_PRIVATE
};

//{{AFX_CODEJOCK_PRIVATE
template<class Parent>
class CXTPDerive : public Parent
{
};
template<>
class CXTPDerive<void>
{
};
//}}AFX_CODEJOCK_PRIVATE

//===========================================================================
// Summary: Implements Singleton design pattern for a derived class with
//          possibility of providing a parent class.
// Parameters:
//          Derived - Type name of singleton instance.
//          Parent - Type of a parent class, can be 'void' which means
//                   no parent class is used.
//===========================================================================
template<class Derived, class Parent>
class CXTPSingletonBase
	: public CXTPDerive<Parent>
	, public CXTPSingleton<Derived>
{
protected:
	//===========================================================================
	// Summary: Handles object construction.
	//===========================================================================
	CXTPSingletonBase()
	{
	}
};

template<class Value, class Tag>
struct XTPSingletonValueAdaptor
{
	Value Value;
};

#	include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif /*__XTPSINGLETON_H__*/
