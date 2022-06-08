// XTPMarkupInlineUIContainer.cpp: implementation of the CXTPMarkupInlineUIContainer class.
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

#include "StdAfx.h"

#include "Common/XTPCasting.h"
#include "Common/XTPFramework.h"
#include "Common/Base/Types/XTPSize.h"

#include "Markup/XTPMarkupTools.h"
#include "Markup/XTPMarkupObject.h"
#include "Markup/XTPMarkupInputElement.h"
#include "Markup/XTPMarkupUIElement.h"
#include "Markup/XTPMarkupBuilder.h"

#include "Markup/XTPMarkupFrameworkContentElement.h"
#include "Markup/Text/XTPMarkupTextElement.h"
#include "Markup/Text/XTPMarkupInline.h"
#include "Markup/Text/XTPMarkupInlineUIContainer.h"

#include "Markup/XTPMarkupIIDs.h"

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

#ifdef _DEBUG
#	undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#	define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////////
// CXTPMarkupInlineUIContainer

IMPLEMENT_MARKUPCLASS(L"InlineUIContainer", CXTPMarkupInlineUIContainer, CXTPMarkupInline);

void CXTPMarkupInlineUIContainer::RegisterMarkupClass()
{
}

CXTPMarkupInlineUIContainer::CXTPMarkupInlineUIContainer()
{
	m_pChild = NULL;
}

CXTPMarkupInlineUIContainer::CXTPMarkupInlineUIContainer(CXTPMarkupUIElement* pElement)
{
	m_pChild = NULL;
	SetChild(pElement);
}

CXTPMarkupInlineUIContainer::~CXTPMarkupInlineUIContainer()
{
	MARKUP_RELEASE(m_pChild);
}

CXTPMarkupUIElement* CXTPMarkupInlineUIContainer::GetChild() const
{
	return m_pChild;
}

void CXTPMarkupInlineUIContainer::SetChild(CXTPMarkupUIElement* pChild)
{
	if (m_pChild)
	{
		m_pChild->SetLogicalParent(NULL);
		MARKUP_RELEASE(m_pChild);
	}

	m_pChild = pChild;

	if (m_pChild)
	{
		m_pChild->SetLogicalParent(this);
	}
}

void CXTPMarkupInlineUIContainer::SetContentObject(CXTPMarkupBuilder* pBuilder,
												   CXTPMarkupObject* pContent)
{
	if (!pContent->IsKindOf(MARKUP_TYPE(CXTPMarkupUIElement)))
	{
		pBuilder->ThrowBuilderException(
			CXTPMarkupBuilder::FormatString(_T("'%ls' object cannot be added to '%ls'. Object ")
											_T("cannot be converted to type 'CXTPMarkupUIElement'"),
											(LPCTSTR)pContent->GetType()->m_lpszClassName,
											(LPCTSTR)GetType()->m_lpszClassName));
	}

	SetChild((CXTPMarkupUIElement*)pContent);
	MARKUP_ADDREF(pContent);
}

BOOL CXTPMarkupInlineUIContainer::HasContentObject() const
{
	return m_pChild != NULL;
}

POSITION CXTPMarkupInlineUIContainer::GetContentStartPosition() const
{
	return m_pChild ? MARKUP_POSITION_START : MARKUP_POSITION_EOF;
};

void CXTPMarkupInlineUIContainer::GetContentNextPosition(POSITION& pos) const
{
	pos = (pos == MARKUP_POSITION_START ? MARKUP_POSITION_END : MARKUP_POSITION_EOF);
}

BOOL CXTPMarkupInlineUIContainer::IsWordBreakPosition(POSITION pos) const
{
	return pos == MARKUP_POSITION_START || pos == MARKUP_POSITION_END;
}

CSize CXTPMarkupInlineUIContainer::Measure(CXTPMarkupDrawingContext* pDC, POSITION posStart,
										   POSITION /*posEnd*/)
{
	if (!m_pChild || posStart != MARKUP_POSITION_START)
		return CSize(0, 0);

	m_pChild->Measure(pDC, CSize(32000, 32000));

	return m_pChild->GetDesiredSize();
}

void CXTPMarkupInlineUIContainer::Measure(CXTPMarkupDrawingContext* pDC, POSITION posStart,
										  POSITION /*posEnd*/, CXTPSizeF& size)
{
	if (!m_pChild || posStart != MARKUP_POSITION_START)
	{
		size = CXTPSizeF(0, 0);
		return;
	}

	m_pChild->Measure(pDC, CSize(32000, 32000));

	CSize sz = m_pChild->GetDesiredSize();

	size.Width	= static_cast<FLOAT>(sz.cx);
	size.Height = static_cast<FLOAT>(sz.cy);
}

void CXTPMarkupInlineUIContainer::Arrange(CRect rcFinalRect, POSITION posStart, POSITION /*posEnd*/)
{
	if (!m_pChild || posStart != MARKUP_POSITION_START)
		return;

	m_pChild->Arrange(rcFinalRect);
}

void CXTPMarkupInlineUIContainer::Render(CXTPMarkupDrawingContext* pDC, CRect /*rc*/,
										 POSITION posStart, POSITION /*posEnd*/)
{
	if (!m_pChild || posStart != MARKUP_POSITION_START)
		return;

	m_pChild->Render(pDC);
}

CXTPMarkupInputElement* CXTPMarkupInlineUIContainer::InputHitTest(CPoint point) const
{
	CXTPMarkupInputElement* pObject = m_pChild ? m_pChild->InputHitTest(point)
											   : (CXTPMarkupInputElement*)this;

	if (pObject)
		return pObject;

	return (CXTPMarkupInputElement*)this;
}

/////////////////////////////////////////////////////////////////////////
// CXTPMarkupInlineUIContainer

#include "Common/Base/Diagnostic/XTPBeginAfxMap.h"
BEGIN_DISPATCH_MAP(CXTPMarkupInlineUIContainer, CXTPMarkupInline)
	DISP_PROPERTY_EX_ID(CXTPMarkupInlineUIContainer, "Child", 700, OleGetChild, OleSetChild,
						VT_DISPATCH)
END_DISPATCH_MAP()
#include "Common/Base/Diagnostic/XTPEndAfxMap.h"

#ifdef _XTP_ACTIVEX

BEGIN_INTERFACE_MAP(CXTPMarkupInlineUIContainer, CXTPMarkupInline)
	INTERFACE_PART(CXTPMarkupInlineUIContainer, XTPDIID_MarkupInlineUIContainer, Dispatch)
END_INTERFACE_MAP()

IMPLEMENT_OLETYPELIB_EX(CXTPMarkupInlineUIContainer, XTPDIID_MarkupInlineUIContainer)
#endif

LPDISPATCH CXTPMarkupInlineUIContainer::OleGetChild()
{
	return XTPGetDispatch(GetChild());
}

void CXTPMarkupInlineUIContainer::OleSetChild(LPDISPATCH lpDisp)
{
	SetChild(MARKUP_STATICCAST(CXTPMarkupUIElement, FromDispatch(lpDisp)));
}
