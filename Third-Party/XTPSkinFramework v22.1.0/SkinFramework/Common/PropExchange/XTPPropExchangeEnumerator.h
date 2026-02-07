// XTPPropExchangeEnumerator.h
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
#if !defined(__XTPPROPEXCHANGEENUMERATOR_H__)
#	define __XTPPROPEXCHANGEENUMERATOR_H__
//}}AFX_CODEJOCK_PRIVATE

#	if _MSC_VER > 1000
#		pragma once
#	endif // _MSC_VER > 1000

#	include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

class CXTPPropExchange;

//===========================================================================
// Summary:
//     CXTPPropExchangeEnumerator is standalone class. It used by CXTPPropExchange to enumerate
//     child sections.
//
// Example:
// <code>
// extern CXTPPropExchange* pPX;
//
// CXTPPropExchangeEnumeratorPtr pEnumerator(pPX->GetEnumerator(_T("Icon")));
// POSITION posEnum = pEnumerator->GetPosition();
//
// while (posEnum)
// {
//     CXTPPropExchangeSection secIcon(pEnumerator->GetNext(posEnum));
// }
// </code>
// See Also: CXTPPropExchange, CXTPPropExchange::GetEnumerator
//===========================================================================
class _XTP_EXT_CLASS CXTPPropExchangeEnumerator
{
public:
	//-------------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPPropExchangeEnumerator object.
	// Parameters:
	//     pContainer - Parent section of enumerator
	//     lpszSection - Enumerator section name
	//-------------------------------------------------------------------------
	CXTPPropExchangeEnumerator(CXTPPropExchange* pContainer, LPCTSTR lpszSection);

	//-------------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPPropExchangeEnumerator object, handles cleanup and
	//     deallocation.
	//-------------------------------------------------------------------------
	virtual ~CXTPPropExchangeEnumerator();

public:
	//-------------------------------------------------------------------------
	// Summary:
	//     Call this method to init enumerable section
	// Parameters:
	//     dwCount - Count of items to be stored.
	//     bCompactMode - TRUE to use "compact mode" of XML nodes.
	// Returns:
	//     A POSITION value that can be used for iteration; NULL if the section is empty.
	//-------------------------------------------------------------------------
	virtual POSITION GetPosition(DWORD dwCount = 0, BOOL bCompactMode = TRUE);

	//-------------------------------------------------------------------------
	// Summary:
	//     Call this method to get next enumerable section to load.
	// Parameters:
	//     pos - A reference to a POSITION value returned by a previous GetNext, GetPosition, or
	//     other member function call.
	//-------------------------------------------------------------------------
	virtual CXTPPropExchange* GetNext(POSITION& pos);

	CXTPPropExchange* GetContainer() const;
	virtual int GetCount() const;

protected:
	CXTPPropExchange* m_pContainer; // Parent section of enumerator
	CString m_strSectionName;		// Enumerator section name
	DWORD m_nCount;					// Count of items to load.
};

AFX_INLINE CXTPPropExchange* CXTPPropExchangeEnumerator::GetContainer() const
{
	return m_pContainer;
}

#	include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPPROPEXCHANGEENUMERATOR_H__)
