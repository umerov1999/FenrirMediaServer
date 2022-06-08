// XTPCasting.h: Type casting and conversion helpers
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
#if !defined(__XTPCASTING_H__)
#	define __XTPCASTING_H__
//}}AFX_CODEJOCK_PRIVATE

#	include <afxdisp.h>

#	if _MSC_VER > 1000
#		pragma once
#	endif // _MSC_VER > 1000

#	include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

#	ifndef XTP_CASTING_ASSERTIONS_ENABLED
#		define XTP_CASTING_ASSERTIONS_ENABLED TRUE
#	endif

//{{AFX_CODEJOCK_PRIVATE
#	define XTP_IMPLEMENT_CHECKED_CASTING(fromType, toType, value, predicate, casting,             \
										  defaultValue)                                            \
		BOOL bChecked = (predicate);                                                               \
		if (XTP_CASTING_ASSERTIONS_ENABLED)                                                        \
			ASSERT("'" #fromType "' value truncated to '" #toType "'.\n\n"                         \
				   "To disable noisy assertions and take the risk of possible data loss "          \
				   "re-compile with XTP_CASTING_ASSERTIONS_ENABLED defined to FALSE or 0."         \
				   && bChecked);                                                                   \
		return (bChecked ? (casting) : (toType(defaultValue)));

#	define XTP_IMPLEMENT_CHECKED_STATIC_CASTING(fromType, toType, value, predicate, defaultValue) \
		XTP_IMPLEMENT_CHECKED_CASTING(fromType, toType, value, predicate,                          \
									  (static_cast<toType>(value)), defaultValue)

#	define XTP_CASTING_T_(e) _T(#	 e)
#	define XTP_CASTING_T(e) XTP_CASTING_T_(e)

#	define XTP_IMPLEMENT_STRICT_CASTING(fromType, toType, value, format, predicate, casting)      \
		if (!(predicate))                                                                          \
		{                                                                                          \
			if (XTP_CASTING_ASSERTIONS_ENABLED)                                                    \
				ASSERT("'" #fromType "' value cannot be cast to '" #toType "'.\n\n"                \
					   "To disable noisy assertions and take the risk of possible data loss "      \
					   "re-compile with XTP_CASTING_ASSERTIONS_ENABLED defined to FALSE or 0."     \
					   && FALSE);                                                                  \
			TRACE(XTP_CASTING_T(fromType) _T("(") _T(                                              \
					  format) _T(") cannot be cast to '") XTP_CASTING_T(toType) _T("'"),           \
				  toType(value));                                                                  \
		}                                                                                          \
		return (casting);

#	define XTP_IMPLEMENT_STRICT_STATIC_CASTING(fromType, toType, value, format, predicate)        \
		XTP_IMPLEMENT_STRICT_CASTING(fromType, toType, value, format, predicate,                   \
									 (static_cast<toType>(value)))

#	define XTP_IMPLEMENT_SELF_CASTING(name, type)                                                 \
		AFX_INLINE type XTPTo##name(type value)                                                    \
		{                                                                                          \
			return value;                                                                          \
		}                                                                                          \
		AFX_INLINE type XTPTo##name##Checked(type value)                                           \
		{                                                                                          \
			return value;                                                                          \
		}                                                                                          \
		AFX_INLINE type XTPTo##name##Strict(type value)                                            \
		{                                                                                          \
			return value;                                                                          \
		}

#	define XTP_IMPLEMENT_LARGER_TO_SMALLER_CASTING(name, fromType, toType, toMax, format)         \
		AFX_INLINE toType XTPTo##name(fromType value)                                              \
		{                                                                                          \
			return static_cast<toType>(value);                                                     \
		}                                                                                          \
		AFX_INLINE toType XTPTo##name##Checked(fromType value)                                     \
		{                                                                                          \
			XTP_IMPLEMENT_CHECKED_STATIC_CASTING(fromType, toType, value, ((value) <= (toMax)),    \
												 toMax);                                           \
		}                                                                                          \
		AFX_INLINE toType XTPTo##name##Strict(fromType value)                                      \
		{                                                                                          \
			XTP_IMPLEMENT_STRICT_STATIC_CASTING(fromType, toType, value, format,                   \
												((value) <= (toMax)));                             \
		}

#	define XTP_IMPLEMENT_SIGNED_TO_UNSIGNED_CASTING(name, fromType, toType, format)               \
		AFX_INLINE toType XTPTo##name(fromType value)                                              \
		{                                                                                          \
			return static_cast<toType>(value);                                                     \
		}                                                                                          \
		AFX_INLINE toType XTPTo##name##Checked(fromType value)                                     \
		{                                                                                          \
			XTP_IMPLEMENT_CHECKED_STATIC_CASTING(fromType, toType, value,                          \
												 (fromType(0) <= (value)), 0);                     \
		}                                                                                          \
		AFX_INLINE toType XTPTo##name##Strict(fromType value)                                      \
		{                                                                                          \
			XTP_IMPLEMENT_STRICT_STATIC_CASTING(fromType, toType, value, format,                   \
												(fromType(0) <= (value)));                         \
		}

#	define XTP_IMPLEMENT_INTERPRETED_CASTING(name, fromType, toType)                              \
		AFX_INLINE toType XTPTo##name(fromType value)                                              \
		{                                                                                          \
			return reinterpret_cast<toType>(value);                                                \
		}

#	define XTPToDWORD XTPToUInt
#	define XTPToDWORDChecked XTPToUIntChecked
#	define XTPToDWORDStrict XTPToUIntStrict

// These type do not require checked or strict conversion as they are always
// used for bit interpretation:
#	define XTPToWPARAM XTPToUIntPtr
#	define XTPToLPARAM XTPToLongPtr
#	define XTPToLRESULT XTPToLongPtr

///////////////////////////////////////////////////////////////////////////////
// ToInt

XTP_IMPLEMENT_SELF_CASTING(Int, int);
XTP_IMPLEMENT_LARGER_TO_SMALLER_CASTING(Int, LONG, int, INT_MAX, "%u");
XTP_IMPLEMENT_LARGER_TO_SMALLER_CASTING(Int, UINT, int, INT_MAX, "%u");
XTP_IMPLEMENT_LARGER_TO_SMALLER_CASTING(Int, ULONG, int, INT_MAX, "%lu");
#	ifdef _WIN64
XTP_IMPLEMENT_LARGER_TO_SMALLER_CASTING(Int, LONG_PTR, int, INT_MAX, "0x%p");
XTP_IMPLEMENT_LARGER_TO_SMALLER_CASTING(Int, ULONG_PTR, int, INT_MAX, "0x%p");
#	endif

///////////////////////////////////////////////////////////////////////////////
// ToUInt

XTP_IMPLEMENT_SELF_CASTING(UInt, UINT);
XTP_IMPLEMENT_SIGNED_TO_UNSIGNED_CASTING(UInt, int, UINT, "%i");
XTP_IMPLEMENT_SIGNED_TO_UNSIGNED_CASTING(UInt, long, UINT, "%li");
XTP_IMPLEMENT_SIGNED_TO_UNSIGNED_CASTING(UInt, double, UINT, "%g");
XTP_IMPLEMENT_LARGER_TO_SMALLER_CASTING(UInt, ULONG, UINT, UINT_MAX, "%lu");
#	ifdef _WIN64
XTP_IMPLEMENT_SIGNED_TO_UNSIGNED_CASTING(UInt, LONG_PTR, UINT, "0x%p");
XTP_IMPLEMENT_LARGER_TO_SMALLER_CASTING(UInt, ULONG_PTR, UINT, UINT_MAX, "0x%p");
#	endif

///////////////////////////////////////////////////////////////////////////////
// ToIntPtr

XTP_IMPLEMENT_SELF_CASTING(IntPtr, INT_PTR);
XTP_IMPLEMENT_LARGER_TO_SMALLER_CASTING(IntPtr, UINT, INT_PTR, MAXINT_PTR, "%u");
XTP_IMPLEMENT_LARGER_TO_SMALLER_CASTING(IntPtr, ULONG, INT_PTR, MAXINT_PTR, "%lu");
XTP_IMPLEMENT_INTERPRETED_CASTING(IntPtr, void*, INT_PTR);
XTP_IMPLEMENT_INTERPRETED_CASTING(IntPtr, const void*, INT_PTR);
#	ifdef _WIN64
XTP_IMPLEMENT_LARGER_TO_SMALLER_CASTING(IntPtr, ULONG_PTR, INT_PTR, MAXINT_PTR, "0x%p");
#	endif

///////////////////////////////////////////////////////////////////////////////
// ToUIntPtr

XTP_IMPLEMENT_SELF_CASTING(UIntPtr, UINT_PTR);
XTP_IMPLEMENT_SIGNED_TO_UNSIGNED_CASTING(UIntPtr, int, UINT_PTR, "%i");
XTP_IMPLEMENT_SIGNED_TO_UNSIGNED_CASTING(UIntPtr, long, UINT_PTR, "%li");
XTP_IMPLEMENT_INTERPRETED_CASTING(UIntPtr, void*, UINT_PTR);
XTP_IMPLEMENT_INTERPRETED_CASTING(UIntPtr, const void*, UINT_PTR);
#	ifdef _WIN64
XTP_IMPLEMENT_LARGER_TO_SMALLER_CASTING(UIntPtr, LONG_PTR, UINT_PTR, MAXUINT_PTR, "0x%p");
#	endif

///////////////////////////////////////////////////////////////////////////////
// ToLong

XTP_IMPLEMENT_SELF_CASTING(Long, long);
XTP_IMPLEMENT_LARGER_TO_SMALLER_CASTING(Long, UINT, long, LONG_MAX, "%u");
XTP_IMPLEMENT_LARGER_TO_SMALLER_CASTING(Long, ULONG, long, LONG_MAX, "%lu");
#	ifdef _WIN64
XTP_IMPLEMENT_LARGER_TO_SMALLER_CASTING(Long, LONG_PTR, long, LONG_MAX, "0x%p");
XTP_IMPLEMENT_LARGER_TO_SMALLER_CASTING(Long, ULONG_PTR, long, LONG_MAX, "0x%p");
#	endif

///////////////////////////////////////////////////////////////////////////////
// ToULong

XTP_IMPLEMENT_SELF_CASTING(ULong, ULONG);
XTP_IMPLEMENT_SIGNED_TO_UNSIGNED_CASTING(ULong, int, ULONG, "%i");
XTP_IMPLEMENT_SIGNED_TO_UNSIGNED_CASTING(ULong, long, ULONG, "%li");
#	ifdef _WIN64
XTP_IMPLEMENT_SIGNED_TO_UNSIGNED_CASTING(ULong, LONG_PTR, ULONG, "0x%p");
XTP_IMPLEMENT_LARGER_TO_SMALLER_CASTING(ULong, ULONG_PTR, ULONG, ULONG_MAX, "0x%p");
#	endif

///////////////////////////////////////////////////////////////////////////////
// ToLongPtr

XTP_IMPLEMENT_SELF_CASTING(LongPtr, LONG_PTR);
#	ifdef MAXLONG_PTR
XTP_IMPLEMENT_LARGER_TO_SMALLER_CASTING(LongPtr, UINT, LONG_PTR, MAXLONG_PTR, "%u");
XTP_IMPLEMENT_LARGER_TO_SMALLER_CASTING(LongPtr, ULONG, LONG_PTR, MAXLONG_PTR, "%lu");
#	else
XTP_IMPLEMENT_LARGER_TO_SMALLER_CASTING(LongPtr, UINT, LONG_PTR, MAXINT_PTR, "%u");
XTP_IMPLEMENT_LARGER_TO_SMALLER_CASTING(LongPtr, ULONG, LONG_PTR, MAXINT_PTR, "%lu");
#	endif
XTP_IMPLEMENT_INTERPRETED_CASTING(LongPtr, void*, LONG_PTR);
XTP_IMPLEMENT_INTERPRETED_CASTING(LongPtr, const void*, LONG_PTR);
#	ifdef _WIN64
XTP_IMPLEMENT_LARGER_TO_SMALLER_CASTING(LongPtr, ULONG_PTR, LONG_PTR, MAXLONG_PTR, "0x%p");
#	endif

///////////////////////////////////////////////////////////////////////////////
// ToULongPtr

XTP_IMPLEMENT_SELF_CASTING(ULongPtr, ULONG_PTR);
XTP_IMPLEMENT_SIGNED_TO_UNSIGNED_CASTING(ULongPtr, int, ULONG_PTR, "%i");
XTP_IMPLEMENT_SIGNED_TO_UNSIGNED_CASTING(ULongPtr, long, ULONG_PTR, "%li");
XTP_IMPLEMENT_INTERPRETED_CASTING(ULongPtr, void*, ULONG_PTR);
XTP_IMPLEMENT_INTERPRETED_CASTING(ULongPtr, const void*, ULONG_PTR);

///////////////////////////////////////////////////////////////////////////////
// ToSizeT

XTP_IMPLEMENT_SELF_CASTING(SizeT, SIZE_T);
XTP_IMPLEMENT_SIGNED_TO_UNSIGNED_CASTING(SizeT, int, SIZE_T, "%i");
XTP_IMPLEMENT_SIGNED_TO_UNSIGNED_CASTING(SizeT, long, SIZE_T, "%li");

#	ifdef _WIN64
XTP_IMPLEMENT_SIGNED_TO_UNSIGNED_CASTING(SizeT, INT_PTR, SIZE_T, "0x%p");
#	endif

///////////////////////////////////////////////////////////////////////////////
// ToFunctionPtr

template<typename FunctionPtr>
AFX_INLINE FunctionPtr XTPToFunctionPtr(FARPROC pfn)
{
	return reinterpret_cast<FunctionPtr>(reinterpret_cast<LPVOID>(pfn));
}

template<typename FunctionPtr>
AFX_INLINE FunctionPtr XTPToFunctionPtr(LPVOID pfn)
{
	return reinterpret_cast<FunctionPtr>(pfn);
}

//}}AFX_CODEJOCK_PRIVATE

#	include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPCASTING_H__)
