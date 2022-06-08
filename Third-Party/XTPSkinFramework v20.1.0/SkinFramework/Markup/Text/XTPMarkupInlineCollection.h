// XTPMarkupInlineCollection.h: interface for the CXTPMarkupInlineCollection class.
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
#if !defined(__XTPMARKUPINLINECOLLECTION_H__)
#	define __XTPMARKUPINLINECOLLECTION_H__
//}}AFX_CODEJOCK_PRIVATE

#	if _MSC_VER > 1000
#		pragma once
#	endif // _MSC_VER > 1000

#	include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

class CXTPMarkupInline;
class CXTPMarkupSpan;
class CXTPMarkupTextBlock;
class CXTPMarkupObject;
class CXTPMarkupBuilder;

class _XTP_EXT_CLASS CXTPMarkupInlineCollection : public CXTPMarkupCollection
{
	DECLARE_MARKUPCLASS(CXTPMarkupInlineCollection);

public:
	CXTPMarkupInlineCollection();
	~CXTPMarkupInlineCollection();

public:
	CXTPMarkupInline* GetInline(int nIndex) const;

protected:
	virtual void SetContentObject(CXTPMarkupBuilder* pBuilder, CXTPMarkupObject* pContent);
	virtual void OnItemAdded(CXTPMarkupObject* pItem, int nIndex);

protected:
	//{{AFX_CODEJOCK_PRIVATE
public:
	DECLARE_DISPATCH_MAP()
#	ifdef _XTP_ACTIVEX
	DECLARE_INTERFACE_MAP()
	DECLARE_OLETYPELIB_EX(CXTPMarkupInlineCollection);
#	endif
	DECLARE_ENUM_VARIANT(CXTPMarkupInlineCollection);

	long OleGetItemCount();
	LPDISPATCH OleGetItem(long nIndex);
	void OleAdd(LPDISPATCH lpElementDisp);
	void OleInsert(long Index, LPDISPATCH lpElementDisp);
	//}}AFX_CODEJOCK_PRIVATE

private:
	friend class CXTPMarkupInline;
	friend class CXTPMarkupSpan;
	friend class CXTPMarkupTextBlock;
};

AFX_INLINE CXTPMarkupInline* CXTPMarkupInlineCollection::GetInline(int nIndex) const
{
	return nIndex >= 0 && nIndex < m_arrItems.GetSize() ? (CXTPMarkupInline*)m_arrItems[nIndex]
														: NULL;
}

#	include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPMARKUPINLINECOLLECTION_H__)
