// XTPFramework.h
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
#if !defined(__XTPFRAMEWORK_H__)
#	define __XTPFRAMEWORK_H__
//}}AFX_CODEJOCK_PRIVATE

#	if _MSC_VER > 1000
#		pragma once
#	endif // _MSC_VER > 1000

#	include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

// IXTPCmdTargetProvider interface identifier.
EXTERN_C const IID IID_IXTPCmdTargetProvider;

#	pragma warning(push)
// C4616: #pragma warning : warning number '...' out of range, must be between '4001' and '4999'
// C4619: #pragma warning : there is no warning number 'number'
// C5204: '...': class has virtual functions, but its trivial destructor is not virtual; instances
// of objects derived from this class may not be destructed correctly
#	pragma warning(disable : 4616 4619 5204)

//===========================================================================
// Summary: A class that implements IXTPCmdTargetProvider interface can be
//          used when guaranteed casting from IDispatch to CCmdTarget is
//          required using CXTPCmdTarget::FromIDispatchSafe.
//===========================================================================
struct __declspec(uuid("1965CF4C-7BE1-4E09-94B4-DCEF78B16D04")) IXTPCmdTargetProvider
	: public IUnknown
{
	//-----------------------------------------------------------------------
	// Summary:
	//     The implementation must always return a valid CCmdTarget pointer
	//     of the current implementation object.
	//  Returns:
	//     Obtained CCmdTarget pointer value.
	//-----------------------------------------------------------------------
	virtual CCmdTarget* GetCmdTarget() = 0;
};

#	pragma warning(pop)

// clang-format off
//{{AFX_CODEJOCK_PRIVATE
#	if defined(_MSC_VER) && (1900 <= _MSC_VER)
#		define XTP_DECLARE_CMDTARGETPROVIDER_INTERFACE_()                                              \
			__pragma(warning(push))                                                                    \
			__pragma(warning(disable : 4616 4619 5204))                                                \
			BEGIN_INTERFACE_PART(CmdTargetProvider, IXTPCmdTargetProvider)                             \
			virtual CCmdTarget* GetCmdTarget();                                                        \
			END_INTERFACE_PART(CmdTargetProvider);                                                     \
			__pragma(warning(pop))
#	else
#		define XTP_DECLARE_CMDTARGETPROVIDER_INTERFACE_()                                              \
			BEGIN_INTERFACE_PART(CmdTargetProvider, IXTPCmdTargetProvider)                             \
			virtual CCmdTarget* GetCmdTarget();                                                        \
			END_INTERFACE_PART(CmdTargetProvider);
#	endif
//}}AFX_CODEJOCK_PRIVATE
// clang-format on

//-----------------------------------------------------------------------
// Summary:
//     When deriving from CXTPCmdTarget is not possible this macro can
//     be used in addition to standard DECLARE_INTERFACE_MAP for
//     declaring IXTPCmdTargetProvider interface.
//-----------------------------------------------------------------------
#	define XTP_DECLARE_CMDTARGETPROVIDER_INTERFACE() XTP_DECLARE_CMDTARGETPROVIDER_INTERFACE_()

//-----------------------------------------------------------------------
// Summary:
//     When deriving from CXTPCmdTarget is not possible this macro can
//     be used in addition to standard DECLARE_INTERFACE_MAP for
//     declaring IXTPCmdTargetProvider interface part.
// Parameters:
//     className - Class name that implements IXTPCmdTargetProvider interface.
//-----------------------------------------------------------------------
#	define XTP_DECLARE_CMDTARGETPROVIDER_INTERFACE_PART(className)                                \
		INTERFACE_PART(className, IID_IXTPCmdTargetProvider, CmdTargetProvider)

//-----------------------------------------------------------------------
// Summary:
//     When deriving from CXTPCmdTarget is not possible this macro can
//     be used in addition to standard DECLARE_INTERFACE_MAP for
//     implementing IXTPCmdTargetProvider interface.
// Parameters:
//     className - Class name that implements IXTPCmdTargetProvider interface.
//-----------------------------------------------------------------------
#	define XTP_IMPLEMENT_CMDTARGETPROVIDER_INTERFACE(className)                                   \
		XTP_IMPLEMENT_IUNKNOWN(className, CmdTargetProvider)                                       \
		CCmdTarget* className::XCmdTargetProvider::GetCmdTarget()                                  \
		{                                                                                          \
			METHOD_PROLOGUE(className, CmdTargetProvider);                                         \
			return pThis;                                                                          \
		}

//===========================================================================
// Summary: The CXTPCmdTarget class extends functionality of CCmdTarget
//          and introduces workarounds for known issues and limitations of CCmdTarget.
//===========================================================================
class _XTP_EXT_CLASS CXTPCmdTarget : public CCmdTarget
{
	DECLARE_DYNAMIC(CXTPCmdTarget);

public:
	//-----------------------------------------------------------------------
	// Summary: Constructs command target instance.
	//-----------------------------------------------------------------------
	CXTPCmdTarget();

	//-----------------------------------------------------------------------
	// Summary:
	//     Tries to cast IDispatch pointer to its CCmdTarget object pointer
	//     if it is provided by CCmdTarget.
	//     Unlike CCmdTarget::FromIDispatch
	//     that has side-effects in certain configurations FromIDispatchSafe
	//     guaranties correct CCmdTarget pointer as a result value if an
	//     object that provides IDispatch interface is either derived from
	//     CXTPCmdTarget or only exposes IXTPCmdTargetProvider interface.
	//     If the object does not expose IXTPCmdTargetProvider interface
	//     the implementation fall back to CCmdTarget::FromIDispatch.
	//  Parameters:
	//     pDisp - IDispatch pointer for which CCmdTarget pointer is to be obtained.
	//  Returns:
	//     Obtained CCmdTarget pointer value or NULL when it cannot be obtained.
	//-----------------------------------------------------------------------
	static CCmdTarget* AFX_CDECL FromIDispatchSafe(IDispatch* pDisp);

	//{{AFX_CODEJOCK_PRIVATE
protected:
	DECLARE_INTERFACE_MAP()
	XTP_DECLARE_CMDTARGETPROVIDER_INTERFACE()
	//}}AFX_CODEJOCK_PRIVATE
};

#	include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPFRAMEWORK_H__)
