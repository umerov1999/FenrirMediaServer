// XTPPropExchangeEnumeratorPtr.h
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
#if !defined(__XTPPROPEXCHANGEENUMERATORPTR_H__)
#	define __XTPPROPEXCHANGEENUMERATORPTR_H__
//}}AFX_CODEJOCK_PRIVATE

#	if _MSC_VER > 1000
#		pragma once
#	endif // _MSC_VER > 1000

#	include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

class _XTP_EXT_CLASS CXTPPropExchangeEnumeratorPtr
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs CXTPPropExchangeEnumeratorPtr a object.
	// Parameters:
	//     pPX -  pointer to the CXTPPropExchangeEnumerator object that is the ultimate source or
	//     destination of the persistent data.
	//-----------------------------------------------------------------------
	explicit CXTPPropExchangeEnumeratorPtr(CXTPPropExchangeEnumerator* pPX);

	//-------------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPPropExchangeEnumeratorPtr object, handles cleanup and
	//     deallocation.
	//-------------------------------------------------------------------------
	virtual ~CXTPPropExchangeEnumeratorPtr();

	CXTPPropExchangeEnumerator& operator*() const;
	CXTPPropExchangeEnumerator* operator->() const;
	CXTPPropExchangeEnumerator* operator&() const;

private:
	CXTPPropExchangeEnumerator* m_pPX;
};

#	include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPPROPEXCHANGEENUMERATORPTR_H__)
