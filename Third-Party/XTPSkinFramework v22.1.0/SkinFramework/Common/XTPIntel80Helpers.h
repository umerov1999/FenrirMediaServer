// XTPIntel80Helpers.h : Intel(R) C++ 8.0 helpers
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

//{{AFX_CODEJOCK_PRIVATE
#if !defined(__XTPINTEL80HELPERS_H__)
#	define __XTPINTEL80HELPERS_H__

#	if (_MSC_VER >= 1000)
#		pragma once
#	endif // _MSC_VER >= 1000

#	include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

#	ifdef __INTEL_COMPILER

#		define _XTP_USES_PROTECTED_ACCESS(theFiend, theClass, theType, theMember)                 \
			class CAccess##theClass : public theClass                                              \
			{                                                                                      \
				CAccess##theClass()                                                                \
				{                                                                                  \
				}                                                                                  \
                                                                                                   \
			public:                                                                                \
				inline theType Get##theMember()                                                    \
				{                                                                                  \
					return theMember;                                                              \
				}                                                                                  \
				friend class theFiend;                                                             \
			};

#		define _XTP_USES_PROTECTED_ACCESS_ARGS(theFiend, theClass, ctorArgs, theType, theMember)  \
			class CAccess##theClass : public theClass                                              \
			{                                                                                      \
				CAccess##theClass ctorArgs : theClass ctorArgs                                     \
				{                                                                                  \
				}                                                                                  \
                                                                                                   \
			public:                                                                                \
				inline theType Get##theMember()                                                    \
				{                                                                                  \
					return theMember;                                                              \
				}                                                                                  \
				friend class theFiend;                                                             \
			};

#		define _XTP_PROTECTED_ACCESS(theClass, thePointer, theMember)                             \
			(((CAccess##theClass*)thePointer)->Get##theMember())

#		define _XTP_PROTECTED_DEFWINDOWPROC_CALL(theFiend, thePonter, m, w, l)                    \
			class CAccessWnd : public CWnd                                                         \
			{                                                                                      \
				CAccessWnd()                                                                       \
				{                                                                                  \
				}                                                                                  \
                                                                                                   \
			public:                                                                                \
				inline void DefWindowProcBase(UINT message, WPARAM wParam, LPARAM lParam)          \
				{                                                                                  \
					CWnd::DefWindowProc(message, wParam, lParam);                                  \
				}                                                                                  \
				friend class theFiend;                                                             \
			};                                                                                     \
			((CAccessWnd*)(thePonter))->DefWindowProcBase(m, w, l);

#	else

#		define _XTP_USES_PROTECTED_ACCESS(theFiend, theClass, theType, theMember)                 \
			class CAccess##theClass : public theClass                                              \
			{                                                                                      \
				CAccess##theClass()                                                                \
				{                                                                                  \
				}                                                                                  \
				friend class theFiend;                                                             \
			};

#		define _XTP_USES_PROTECTED_ACCESS_ARGS(theFiend, theClass, ctorArgs, ctorArgs2, theType,  \
												theMember)                                         \
			class CAccess##theClass : public theClass                                              \
			{                                                                                      \
				CAccess##theClass##ctorArgs : theClass##ctorArgs2                                  \
				{                                                                                  \
				}                                                                                  \
				friend class theFiend;                                                             \
			};

#		define _XTP_PROTECTED_ACCESS(theClass, thePointer, theMember)                             \
			(((CAccess##theClass*)thePointer)->theMember)

#		define _XTP_PROTECTED_DEFWINDOWPROC_CALL(theFiend, thePonter, m, w, l)                    \
			class CAccessWnd : public CWnd                                                         \
			{                                                                                      \
				CAccessWnd()                                                                       \
				{                                                                                  \
				}                                                                                  \
				friend class theFiend;                                                             \
			};                                                                                     \
			((CAccessWnd*)thePonter)->DefWindowProc(m, w, l);

#	endif

//}}AFX_CODEJOCK_PRIVATE

//////////////////////////////////////////////////////////////////////

#	include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // #if !defined(__XTPINTEL80HELPERS_H__)
