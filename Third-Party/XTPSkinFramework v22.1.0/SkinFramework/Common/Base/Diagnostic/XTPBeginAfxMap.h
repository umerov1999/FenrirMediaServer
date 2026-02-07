// XTPBeginAfxMap.h: Must be included before any AFX map, like message map,
// dispatch map, event map, interface part, etc.
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

#ifndef XTP_AFX_MAP_INCLUSION_CONTROL
#	define XTP_AFX_MAP_INCLUSION_CONTROL

// C4619: #pragma warning: there is no warning number '...'
// C4616: #pragma warning : warning number '...' out of range, must be between '4001' and '4999'
// C4191: 'type cast' : unsafe conversion from '...' to 'AFX_PMSGW'
// C5246: '...': the initialization of a subobject should be wrapped in braces
// C5204: '...': class has virtual functions, but its trivial destructor is not virtual; instances
// of objects derived from this class may not be destructed correctly
#	pragma warning(push)
#	pragma warning(disable : 4619 4616 4191 5246 5204)
#else
#	error "Common/Base/Diagnostic/XTPEndAfxMap.h" inclusion is missing
#endif

//}}AFX_CODEJOCK_PRIVATE
