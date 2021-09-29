// XTPResourceImageList.h
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
#if !defined(__XTPRESOURCEIMAGELIST_H__)
#	define __XTPRESOURCEIMAGELIST_H__
//}}AFX_CODEJOCK_PRIVATE

#	if _MSC_VER > 1000
#		pragma once
#	endif // _MSC_VER > 1000

#	include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

class _XTP_EXT_CLASS CXTPResourceImageList : public CXTPResourceImages
{
	DECLARE_DYNCREATE(CXTPResourceImageList)
	friend class CXTPResourceImage;

public:
	CXTPResourceImageList();
	virtual ~CXTPResourceImageList();

	CXTPResourceImage* GetBitmap(UINT nID);

	CXTPResourceImage* SetBitmap(HBITMAP hBitmap, UINT nID, BOOL bAlptha, BOOL bCopyBitmap = TRUE);

	BOOL SetBitmap(CXTPResourceImage* pImage, UINT nID, BOOL bCallAddRef);

	BOOL LoadBitmap(LPCTSTR lpcszPath, UINT nID);

	BOOL Remove(UINT nID);
	void RemoveAll();

protected:
	CMap<UINT, UINT, CXTPResourceImage*, CXTPResourceImage*> m_mapID2Image;

protected:
#	ifdef _XTP_ACTIVEX
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()

	DECLARE_OLETYPELIB_EX(CXTPResourceImageList);

	void OnFinalRelease();

	// Only for Calendar package
#		ifdef XTP_CALENDAR_SITENOTIFY_KEY
	DECLARE_OLECREATE_EX(CXTPResourceImageList)
#		endif

	BOOL OleAddBitmap(OLE_HANDLE ohBitmap, UINT nID);
#	endif
};

#	include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPRESOURCEIMAGELIST_H__)
