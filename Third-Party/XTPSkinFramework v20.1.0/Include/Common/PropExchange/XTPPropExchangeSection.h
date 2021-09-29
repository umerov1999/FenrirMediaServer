// XTPPropExchangeSection.h
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
#if !defined(__XTPPROPEXCHANGESECTION_H__)
#	define __XTPPROPEXCHANGESECTION_H__
//}}AFX_CODEJOCK_PRIVATE

#	if _MSC_VER > 1000
#		pragma once
#	endif // _MSC_VER > 1000

#	include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

class CXTPPropExchange;

//===========================================================================
// Summary:
//     CXTPPropExchangeSection is standalone class. It is used to delete memory allocated by
//     CXTPPropExchange::GetSection.
// Example:
// <code>
// CXTPPropExchangeSection pxTaskPanel(px.GetSection(_T("TaskPanel")));
// </code>
//===========================================================================
class _XTP_EXT_CLASS CXTPPropExchangeSection
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs CXTPPropExchangeSection a object.
	// Parameters:
	//     pPX -  pointer to the CXTPPropExchange object that is the ultimate source or destination
	//     of the persistent data.
	//-----------------------------------------------------------------------
	explicit CXTPPropExchangeSection(CXTPPropExchange* pPX);

	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs CXTPPropExchangeSection object and automatically assign m_pPX .
	// Parameters:
	//     bLoading - TRUE to load
	//     lpszSection - Section of registry
	//-----------------------------------------------------------------------
	CXTPPropExchangeSection(BOOL bLoading, LPCTSTR lpszSection);

	//-------------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPPropExchangeSection object, handles cleanup and
	//     deallocation.
	//-------------------------------------------------------------------------
	virtual ~CXTPPropExchangeSection();

	CXTPPropExchange& operator*() const;
	CXTPPropExchange* operator->() const;
	CXTPPropExchange* operator&() const;

private:
	CXTPPropExchange* m_pPX;
};

#	include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPPROPEXCHANGESECTION_H__)
