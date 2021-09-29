// XTPMarkupInlineUIContainer.h: interface for the CXTPMarkupInlineUIContainer class.
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
#if !defined(__XTPMARKUPINLINEUICONTAINER_H__)
#	define __XTPMARKUPINLINEUICONTAINER_H__
//}}AFX_CODEJOCK_PRIVATE

#	if _MSC_VER > 1000
#		pragma once
#	endif // _MSC_VER > 1000

#	include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

class CXTPMarkupObject;
class CXTPMarkupUIElement;
class CXTPMarkupBuilder;
class CXTPMarkupDrawingContext;

class _XTP_EXT_CLASS CXTPMarkupInlineUIContainer : public CXTPMarkupInline
{
	DECLARE_MARKUPCLASS(CXTPMarkupInlineUIContainer);

protected:
	CXTPMarkupInlineUIContainer();
	CXTPMarkupInlineUIContainer(CXTPMarkupUIElement* pElement);
	~CXTPMarkupInlineUIContainer();

public:
	void SetChild(CXTPMarkupUIElement* pElement);
	CXTPMarkupUIElement* GetChild() const;

public:
	virtual POSITION GetContentStartPosition() const;
	virtual void GetContentNextPosition(POSITION& pos) const;
	virtual BOOL IsWordBreakPosition(POSITION pos) const;
	virtual void Render(CXTPMarkupDrawingContext* pDC, CRect rc, POSITION posStart,
						POSITION posEnd);
	virtual CSize Measure(CXTPMarkupDrawingContext* pDC, POSITION posStart, POSITION posEnd);
	virtual void Measure(CXTPMarkupDrawingContext* pDC, POSITION posStart, POSITION posEnd,
						 CXTPSizeF& size);
	virtual void Arrange(CRect rcFinalRect, POSITION posStart, POSITION posEnd);

protected:
	virtual BOOL HasContentObject() const;
	virtual void SetContentObject(CXTPMarkupBuilder* pBuilder, CXTPMarkupObject* pContent);
	virtual int GetLogicalChildrenCount() const;
	virtual CXTPMarkupObject* GetLogicalChild(int nIndex) const;

	CXTPMarkupInputElement* InputHitTest(CPoint point) const;

	//{{AFX_CODEJOCK_PRIVATE
public:
	DECLARE_DISPATCH_MAP()
#	ifdef _XTP_ACTIVEX
	DECLARE_INTERFACE_MAP()
	DECLARE_OLETYPELIB_EX(CXTPMarkupInlineUIContainer);
#	endif

	afx_msg LPDISPATCH OleGetChild();
	afx_msg void OleSetChild(LPDISPATCH lpDisp);

	//}}AFX_CODEJOCK_PRIVATE

protected:
	CXTPMarkupUIElement* m_pChild;
};

AFX_INLINE int CXTPMarkupInlineUIContainer::GetLogicalChildrenCount() const
{
	return m_pChild ? 1 : 0;
}

AFX_INLINE CXTPMarkupObject* CXTPMarkupInlineUIContainer::GetLogicalChild(int nIndex) const
{
	return nIndex == 0 ? (CXTPMarkupObject*)m_pChild : NULL;
}

#	include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPMARKUPINLINEUICONTAINER_H__)
