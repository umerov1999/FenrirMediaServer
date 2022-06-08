// XTPMarkupTextBlock.cpp: implementation of the CXTPMarkupTextBlock class.
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

#include "stdafx.h"

#include "Common/Base/Diagnostic/XTPDisableAdvancedWarnings.h"
#include <ActivScp.h>
#include "Common/Base/Diagnostic/XTPEnableAdvancedWarnings.h"

#include "GraphicLibrary/GdiPlus/XTPGdiPlus.h"

#include "Common/XTPTypeId.h"
#include "Common/Math/XTPMathUtils.h"
#include "Common/XTPCasting.h"
#include "Common/XTPFramework.h"
#include "Common/XTPSystemHelpers.h"
#include "Common/XTPSynchro.h"
#include "Common/XTPApplication.h"
#include "Common/XTPSingleton.h"
#include "Common/XTPGdiObjects.h"
#include "Common/XTPResourceManager.h"
#include "Common/Base/Types/XTPPoint2.h"
#include "Common/Base/Types/XTPSize.h"
#include "Common/Base/Types/XTPRect.h"
#include "Common/Math/XTPMathUtils.h"

#include "Markup/XTPMarkupContext.h"
#include "Markup/XTPMarkupTools.h"
#include "Markup/XTPMarkupObject.h"
#include "Markup/XTPMarkupString.h"
#include "Markup/XTPMarkupInputElement.h"
#include "Markup/XTPMarkupUIElement.h"
#include "Markup/XTPMarkupFrameworkElement.h"
#include "Markup/Controls/XTPMarkupTextBlock.h"
#include "Markup/XTPMarkupDrawingContext.h"
#include "Markup/XTPMarkupFrameworkContentElement.h"
#include "Markup/Text/XTPMarkupTextElement.h"
#include "Markup/Text/XTPMarkupInline.h"
#include "Markup/Text/XTPMarkupInlineCollection.h"
#include "Markup/Text/XTPMarkupRun.h"
#include "Markup/XTPMarkupBuilder.h"
#include "Markup/XTPMarkupThickness.h"
#include "Markup/XTPMarkupTypedSimpleStack.h"
#include "Markup/DeviceContext/XTPMarkupDeviceContext.h"
#include "Markup/XTPMarkupIIDs.h"

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

#ifdef _DEBUG
#	undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#	define new DEBUG_NEW
#endif

struct CXTPMarkupTextBlock::CLinePart
{
	CLinePart(CXTPMarkupInline* pInline, POSITION pos)
	{
		ZeroMemory(this, sizeof(CLinePart));

		this->pInline  = pInline;
		this->posStart = pos;
		this->posEnd   = pos;

		pInline->AddRef();

		bEmptyPart = FALSE;
	}

	~CLinePart()
	{
		pInline->Release();
	}

	void Calculate(CLineIterator& li);

	CXTPMarkupInline* pInline;
	POSITION posStart;
	POSITION posEnd;
	CXTPPoint2f ptTopLeft;
	CXTPPoint2f ptOffset;
	CLine* pLine;
	CXTPSizeF size;
	int nBaseline;
	XTPMarkupFontMetrics fontMetrics;
	CLinePart* m_pNextChain;
	BOOL bEmptyPart;
};

struct CXTPMarkupTextBlock::CLine
{
public:
	CLine()
	{
		m_nWidth = m_nHeight = m_nBaseline = 0;
		m_pNextChain					   = NULL;

		m_bAllowJustify = FALSE;
	}

	~CLine()
	{
		CLinePart* pos = m_arrParts.GetHead();
		while (pos)
		{
			CLinePart* posDelete = pos;
			pos					 = pos->m_pNextChain;

			delete posDelete;
		}

		m_arrParts.RemoveAll();
	}

public:
	BOOL IsEmpty() const
	{
		return m_arrParts.IsEmpty();
	}

	CXTPMarkupTypedSimpleStack<CLinePart> m_arrParts;
	FLOAT m_nWidth;
	FLOAT m_nHeight;
	FLOAT m_nBaseline;
	CLine* m_pNextChain;
	BOOL m_bAllowJustify;
};

struct CXTPMarkupTextBlock::CLineIterator
{
	CLineIterator()
	{
		x = y = nMaxWidth = 0;
		pLine			  = NULL;
		pLinePart		  = NULL;
		pDC				  = NULL;
		bTextJustify	  = FALSE;
	}

	FLOAT x;
	FLOAT y;
	FLOAT nMaxWidth;
	CLine* pLine;
	CLinePart* pLinePart;
	CXTPMarkupTypedSimpleStack<CLinePart> arrStack;
	CXTPMarkupDrawingContext* pDC;
	XTPMarkupTextWrapping bTextWrapping;
	XTPMarkupTextTrimming bTextTrimming;
	BOOL bTextJustify;
};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CXTPMarkupDependencyProperty* CXTPMarkupTextBlock::m_pBackgroundProperty	  = NULL;
CXTPMarkupDependencyProperty* CXTPMarkupTextBlock::m_pForegroundProperty	  = NULL;
CXTPMarkupDependencyProperty* CXTPMarkupTextBlock::m_pFontSizeProperty		  = NULL;
CXTPMarkupDependencyProperty* CXTPMarkupTextBlock::m_pFontWeightProperty	  = NULL;
CXTPMarkupDependencyProperty* CXTPMarkupTextBlock::m_pFontFamilyProperty	  = NULL;
CXTPMarkupDependencyProperty* CXTPMarkupTextBlock::m_pFontStyleProperty		  = NULL;
CXTPMarkupDependencyProperty* CXTPMarkupTextBlock::m_pTextDecorationsProperty = NULL;
CXTPMarkupDependencyProperty* CXTPMarkupTextBlock::m_pTextWrappingProperty	  = NULL;
CXTPMarkupDependencyProperty* CXTPMarkupTextBlock::m_pTextTrimmingProperty	  = NULL;
CXTPMarkupDependencyProperty* CXTPMarkupTextBlock::m_pTextAlignmentProperty	  = NULL;
CXTPMarkupDependencyProperty* CXTPMarkupTextBlock::m_pPaddingProperty		  = NULL;
CXTPMarkupDependencyProperty* CXTPMarkupTextBlock::m_pTextProperty			  = NULL;
CXTPMarkupDependencyProperty* CXTPMarkupTextBlock::m_pFontQualityProperty	  = NULL;
CXTPMarkupDependencyProperty* CXTPMarkupTextBlock::m_pFontCharsetProperty	  = NULL;

IMPLEMENT_MARKUPCLASS(L"TextBlock", CXTPMarkupTextBlock, CXTPMarkupFrameworkElement)

void CXTPMarkupTextBlock::RegisterMarkupClass()
{
	CXTPMarkupTextElement::RegisterType();

	m_pBackgroundProperty = CXTPMarkupTextElement::m_pBackgroundProperty->AddOwner(
		MARKUP_TYPE(CXTPMarkupTextBlock));

	m_pForegroundProperty = CXTPMarkupTextElement::m_pForegroundProperty->AddOwner(
		MARKUP_TYPE(CXTPMarkupTextBlock));

	m_pFontSizeProperty = CXTPMarkupTextElement::m_pFontSizeProperty->AddOwner(
		MARKUP_TYPE(CXTPMarkupTextBlock));

	m_pFontWeightProperty = CXTPMarkupTextElement::m_pFontWeightProperty->AddOwner(
		MARKUP_TYPE(CXTPMarkupTextBlock));

	m_pFontQualityProperty = CXTPMarkupTextElement::m_pFontQualityProperty->AddOwner(
		MARKUP_TYPE(CXTPMarkupTextBlock));

	m_pFontCharsetProperty = CXTPMarkupTextElement::m_pFontCharsetProperty->AddOwner(
		MARKUP_TYPE(CXTPMarkupTextBlock));

	m_pFontFamilyProperty = CXTPMarkupTextElement::m_pFontFamilyProperty->AddOwner(
		MARKUP_TYPE(CXTPMarkupTextBlock));

	m_pFontStyleProperty = CXTPMarkupTextElement::m_pFontStyleProperty->AddOwner(
		MARKUP_TYPE(CXTPMarkupTextBlock));

	m_pTextDecorationsProperty = CXTPMarkupTextElement::m_pTextDecorationsProperty->AddOwner(
		MARKUP_TYPE(CXTPMarkupTextBlock));

	m_pTextWrappingProperty = CXTPMarkupDependencyProperty::Register(
		L"TextWrapping", MARKUP_TYPE(CXTPMarkupEnum), MARKUP_TYPE(CXTPMarkupTextBlock),
		new CXTPMarkupPropertyMetadata(NULL, &CXTPMarkupBuilder::ConvertTextWrapping,
									   CXTPMarkupPropertyMetadata::flagAffectsMeasure));

	m_pTextTrimmingProperty = CXTPMarkupDependencyProperty::Register(
		L"TextTrimming", MARKUP_TYPE(CXTPMarkupEnum), MARKUP_TYPE(CXTPMarkupTextBlock),
		new CXTPMarkupPropertyMetadata(NULL, &CXTPMarkupBuilder::ConvertTextTrimming,
									   CXTPMarkupPropertyMetadata::flagAffectsMeasure));

	m_pTextAlignmentProperty = CXTPMarkupDependencyProperty::Register(
		L"TextAlignment", MARKUP_TYPE(CXTPMarkupEnum), MARKUP_TYPE(CXTPMarkupTextBlock),
		new CXTPMarkupPropertyMetadata(NULL, &CXTPMarkupBuilder::ConvertTextAlignment,
									   CXTPMarkupPropertyMetadata::flagAffectsArrange));

	m_pPaddingProperty = CXTPMarkupDependencyProperty::Register(
		L"Padding", MARKUP_TYPE(CXTPMarkupThickness), MARKUP_TYPE(CXTPMarkupTextBlock),
		new CXTPMarkupPropertyMetadata(CXTPMarkupThickness::CreateValue(),
									   CXTPMarkupPropertyMetadata::flagAffectsMeasure
										   | CXTPMarkupPropertyMetadata::flagHorzDpiSensible
										   | CXTPMarkupPropertyMetadata::flagVertDpiSensible));

	m_pTextProperty = CXTPMarkupDependencyProperty::Register(
		L"Text", MARKUP_TYPE(CXTPMarkupString), MARKUP_TYPE(CXTPMarkupTextBlock),
		new CXTPMarkupPropertyMetadata(NULL, CXTPMarkupPropertyMetadata::flagAffectsMeasure));
}

CXTPMarkupTextBlock::CXTPMarkupTextBlock()
{
	m_pInlines = new CXTPMarkupInlineCollection();
	m_pInlines->SetLogicalParent(this);

	m_nLastWidth  = -1;
	m_szBlockSize = CSize(0, 0);

	m_pLineList = new CXTPMarkupTypedSimpleStack<CLine>();
}

CXTPMarkupTextBlock::~CXTPMarkupTextBlock()
{
	RemoveAllLines();
	delete m_pLineList;

	if (m_pInlines)
	{
		m_pInlines->RemoveAll();
		m_pInlines->SetLogicalParent(NULL);
		MARKUP_RELEASE(m_pInlines);
	}
}

void CXTPMarkupTextBlock::SetTextAlignment(XTPMarkupTextAlignment alignment)
{
	SetValue(m_pTextAlignmentProperty, CXTPMarkupEnum::CreateValue(alignment));
}

XTPMarkupTextAlignment CXTPMarkupTextBlock::GetTextAlignment() const
{
	CXTPMarkupEnum* pAlign = MARKUP_STATICCAST(CXTPMarkupEnum, GetValue(m_pTextAlignmentProperty));
	return pAlign ? (XTPMarkupTextAlignment)(int)(*pAlign) : xtpMarkupTextAlignmentLeft;
}

CXTPMarkupBrush* CXTPMarkupTextBlock::GetBackground() const
{
	return MARKUP_STATICCAST(CXTPMarkupBrush, GetValue(m_pBackgroundProperty));
}

void CXTPMarkupTextBlock::SetBackground(CXTPMarkupBrush* brush)
{
	SetValue(m_pBackgroundProperty, brush);
}

CXTPMarkupBrush* CXTPMarkupTextBlock::GetForeground() const
{
	return MARKUP_STATICCAST(CXTPMarkupBrush, GetValue(m_pForegroundProperty));
}

void CXTPMarkupTextBlock::SetForeground(CXTPMarkupBrush* brush)
{
	SetValue(m_pForegroundProperty, brush);
}

void CXTPMarkupTextBlock::SetText(LPCWSTR lpszText)
{
	SetValue(m_pTextProperty, CXTPMarkupString::CreateValue(lpszText));
}

CString CXTPMarkupTextBlock::GetText() const
{
	if (m_pInlines->GetCount() != 1
		|| MARKUP_DYNAMICCAST(CXTPMarkupRun, m_pInlines->GetInline(0)) == 0)
		return _T("");

	return ((CXTPMarkupRun*)m_pInlines->GetInline(0))->GetText();
}

void CXTPMarkupTextBlock::OnPropertyChanged(CXTPMarkupDependencyProperty* pProperty,
											CXTPMarkupObject* pOldValue,
											CXTPMarkupObject* pNewValue)
{
	if (pProperty == m_pTextProperty)
	{
		ASSERT(m_pMarkupContext);
		m_pInlines->RemoveAll();

		CXTPMarkupRun* pRun = MARKUP_CREATE(CXTPMarkupRun, m_pMarkupContext);
		pRun->SetText(((CXTPMarkupString*)pNewValue));

		m_pInlines->Add(pRun);

		MARKUP_ADDREF(pNewValue);
	}

	CXTPMarkupFrameworkElement::OnPropertyChanged(pProperty, pOldValue, pNewValue);
}

void CXTPMarkupTextBlock::SetFontSize(int nFontSize)
{
	SetValue(m_pFontSizeProperty, new CXTPMarkupInt(nFontSize));
}

int CXTPMarkupTextBlock::GetFontSize() const
{
	CXTPMarkupInt* pFontSize = MARKUP_STATICCAST(CXTPMarkupInt, GetValue(m_pFontSizeProperty));
	if (!pFontSize)
		return -12;

	return *pFontSize;
}

void CXTPMarkupTextBlock::SetFontWeight(int nFontWeight)
{
	SetValue(m_pFontWeightProperty, CXTPMarkupEnum::CreateValue(nFontWeight));
}

int CXTPMarkupTextBlock::GetFontWeight() const
{
	CXTPMarkupEnum* pFontWeigh = MARKUP_STATICCAST(CXTPMarkupEnum, GetValue(m_pFontWeightProperty));
	if (!pFontWeigh)
		return FW_NORMAL;

	return *pFontWeigh;
}

void CXTPMarkupTextBlock::SetFontStyle(int nFontStyle)
{
	SetValue(m_pFontStyleProperty, CXTPMarkupEnum::CreateValue(nFontStyle));
}

int CXTPMarkupTextBlock::GetFontStyle() const
{
	CXTPMarkupEnum* pFontStyle = MARKUP_STATICCAST(CXTPMarkupEnum, GetValue(m_pFontStyleProperty));
	if (!pFontStyle)
		return 0;

	return *pFontStyle;
}

void CXTPMarkupTextBlock::SetTextDecorations(int nTextDecorations)
{
	SetValue(m_pTextDecorationsProperty, CXTPMarkupEnum::CreateValue(nTextDecorations));
}

int CXTPMarkupTextBlock::GetTextDecorations() const
{
	CXTPMarkupEnum* pTextDecorations = MARKUP_STATICCAST(CXTPMarkupEnum,
														 GetValue(m_pTextDecorationsProperty));
	if (!pTextDecorations)
		return 0;

	return *pTextDecorations;
}

void CXTPMarkupTextBlock::SetFontFamily(LPCWSTR lpszFontFamily)
{
	SetValue(m_pFontFamilyProperty, CXTPMarkupString::CreateValue(lpszFontFamily));
}
LPCWSTR CXTPMarkupTextBlock::GetFontFamily() const
{
	CXTPMarkupString* pFontFamily = MARKUP_STATICCAST(CXTPMarkupString,
													  GetValue(m_pFontFamilyProperty));
	if (!pFontFamily)
		return NULL;

	return *pFontFamily;
}

CXTPMarkupInline* CXTPMarkupTextBlock::GetFirstInline() const
{
	return m_pInlines->GetCount() > 0 ? m_pInlines->GetInline(0)->GetFirstInline() : NULL;
}

void CXTPMarkupTextBlock::RemoveAllLines()
{
	CLine* pLine = m_pLineList->GetHead();
	while (pLine)
	{
		CLine* pLineDelete = pLine;
		pLine			   = pLine->m_pNextChain;
		delete pLineDelete;
	}
	m_pLineList->RemoveAll();
}

void CXTPMarkupTextBlock::TrimLine(CLineIterator& li, int nWidth)
{
	CXTPMarkupRun* pRunEllipsis = MARKUP_CREATE(CXTPMarkupRun, m_pMarkupContext);
	pRunEllipsis->SetLogicalParent(this);
	pRunEllipsis->SetText(L"...");

	CLinePart* pPartEllipsis = new CLinePart(pRunEllipsis, pRunEllipsis->GetContentStartPosition());
	pRunEllipsis->GetContentNextPosition(pPartEllipsis->posEnd);
	pRunEllipsis->GetContentNextPosition(pPartEllipsis->posEnd);
	pRunEllipsis->GetContentNextPosition(pPartEllipsis->posEnd);

	pPartEllipsis->pInline->PrepareMeasure(li.pDC);
	pPartEllipsis->Calculate(li);

	nWidth -= static_cast<int>(pPartEllipsis->size.Width);

	FLOAT x			 = 0;
	CLinePart* pPart = li.pLine->m_arrParts.GetHead();
	while (pPart)
	{
		if (static_cast<int>(x + pPart->size.Width) > nWidth)
		{
			CXTPMarkupInline* pInline = pPart->pInline;
			pPart->posEnd			  = pPart->posStart;
			POSITION posChar = pPart->posEnd, posWord = (x == 0 ? NULL : pPart->posEnd);

			while (pPart->posEnd != MARKUP_POSITION_EOF)
			{
				pPart->Calculate(li);

				if (static_cast<int>(x + pPart->size.Width) > nWidth)
					break;

				posChar = pPart->posEnd;

				if (li.bTextTrimming == xtpMarkupTextTrimmingWordEllipsis
					&& pInline->IsWordBreakPosition(pPart->posEnd))
					posWord = posChar;

				pInline->GetContentNextPosition(pPart->posEnd);
			}
			pPart->posEnd = posWord && li.bTextTrimming == xtpMarkupTextTrimmingWordEllipsis
								? posWord
								: posChar;
			pPart->Calculate(li);

			x += pPart->size.Width;
			break;
		}
		x += pPart->size.Width;

		pPart = pPart->m_pNextChain;
	}

	if (pPart)
	{
		while (pPart->m_pNextChain)
		{
			CLinePart* pNextChain = pPart->m_pNextChain;
			li.pLine->m_arrParts.Remove(pNextChain);

			delete pNextChain;
		}
	}

	li.pLine->m_arrParts.AddTail(pPartEllipsis);
	pPartEllipsis->pLine = li.pLine;
	pPartEllipsis->ptTopLeft.SetPoint(x, li.y);

	pRunEllipsis->Release();
}

void CXTPMarkupTextBlock::CloseLine(CLineIterator& li, int nWidth)
{
	if (!li.pLine->m_arrParts.IsEmpty())
	{
		ASSERT(li.pLine->m_nHeight == 0);

		if (li.bTextTrimming != xtpMarkupTextTrimmingNone && static_cast<int>(li.x) > nWidth)
		{
			TrimLine(li, nWidth);
		}

		CLinePart* pPart = li.pLine->m_arrParts.GetHead();
		while (pPart)
		{
			li.pLine->m_nWidth += pPart->size.Width;
			li.pLine->m_nBaseline = max(li.pLine->m_nBaseline,
										static_cast<FLOAT>(pPart->nBaseline));
			pPart				  = pPart->m_pNextChain;
		}

		pPart = li.pLine->m_arrParts.GetHead();
		while (pPart)
		{
			li.pLine->m_nHeight = max(li.pLine->m_nHeight,
									  pPart->size.Height - static_cast<float>(pPart->nBaseline)
										  + li.pLine->m_nBaseline);
			pPart				= pPart->m_pNextChain;
		}

		li.nMaxWidth = max(li.nMaxWidth, li.pLine->m_nWidth);

		li.x = 0;
		li.y += li.pLine->m_nHeight;

		m_pLineList->AddTail(li.pLine);
		li.pLine = new CLine();
	}
}

void CXTPMarkupTextBlock::PushLinePart(CLineIterator& li, int nWidth)
{
	if (li.pLinePart
		&& ((li.pLinePart->posStart != li.pLinePart->posEnd) || li.pLinePart->bEmptyPart))
	{
		CLinePart* pLinePart = li.pLinePart;
		li.pLinePart		 = new CLinePart(pLinePart->pInline, pLinePart->posEnd);

		li.arrStack.AddTail(pLinePart);
	}

	if (li.bTextWrapping && li.x > 0 && static_cast<int>(li.x + GetStackWidth(li)) > nWidth)
	{
		li.pLine->m_bAllowJustify = TRUE;
		CloseLine(li, nWidth);
	}

	while (!li.arrStack.IsEmpty())
	{
		CLinePart* pPart = li.arrStack.RemoveHead();

		if (li.x == 0 && li.y > 0 && pPart->pInline->IsWhiteSpacePosition(pPart->posStart))
		{
			while (pPart->posStart != pPart->posEnd
				   && pPart->pInline->IsWhiteSpacePosition(pPart->posStart))
			{
				pPart->pInline->GetContentNextPosition(pPart->posStart);
			}

			if (pPart->posStart == pPart->posEnd)
			{
				delete pPart;
				continue;
			}
			pPart->pInline->Measure(li.pDC, pPart->posStart, pPart->posEnd, pPart->size);
		}

		if (li.bTextWrapping == xtpMarkupTextWrap
			&& static_cast<int>(li.x + pPart->size.Width) > nWidth
			&& pPart->posEnd != pPart->posStart)
		{
			POSITION posEnd = pPart->posEnd;
			POSITION pos	= pPart->posStart;
			pPart->pInline->GetContentNextPosition(pos);
			pPart->posEnd = pos;

			while (pos != posEnd)
			{
				CXTPSizeF size;
				pPart->pInline->Measure(li.pDC, pPart->posStart, pos, size);

				if (static_cast<int>(li.x + size.Width) > nWidth)
					break;

				pPart->posEnd = pos;

				pPart->pInline->GetContentNextPosition(pos);
			}

			pPart->pInline->Measure(li.pDC, pPart->posStart, pPart->posEnd, pPart->size);
			pPart->ptTopLeft = CXTPPoint2f(li.x, li.y);
			pPart->pLine	 = li.pLine;
			li.pLine->m_arrParts.AddTail(pPart);
			li.x += pPart->size.Width;

			li.pLine->m_bAllowJustify = TRUE;
			CloseLine(li, nWidth);

			CLinePart* pPartEnd = new CLinePart(pPart->pInline, pPart->posEnd);
			pPartEnd->posEnd	= posEnd;
			pPartEnd->Calculate(li);
			li.arrStack.AddHead(pPartEnd);
			continue;
		}

		CLinePart* pPartLast = !li.pLine->m_arrParts.IsEmpty() ? li.pLine->m_arrParts.GetTail()
															   : NULL;

		if (!li.bTextJustify && pPartLast && pPartLast->posEnd == pPart->posStart
			&& pPart->pInline == pPartLast->pInline && pPartLast->pInline && !pPart->bEmptyPart)
		{
			ASSERT(pPartLast->size.Height == pPart->size.Height);
			pPartLast->posEnd = pPart->posEnd;

			li.x -= pPartLast->size.Width;
			pPartLast->pInline->Measure(li.pDC, pPartLast->posStart, pPartLast->posEnd,
										pPartLast->size);
			li.x += pPartLast->size.Width;

			delete pPart;
		}
		else
		{
			pPart->ptTopLeft = CXTPPoint2f(li.x, li.y);
			pPart->pLine	 = li.pLine;
			li.pLine->m_arrParts.AddTail(pPart);
			li.x += pPart->size.Width;
		}
	}
}

void CXTPMarkupTextBlock::CLinePart::Calculate(CLineIterator& li)
{
	pInline->Measure(li.pDC, posStart, posEnd, size);
	nBaseline = pInline->GetBaseline();

	const XTPMarkupFontMetrics* pFontMetrics = pInline->GetFontMetrics();
	if (NULL != pFontMetrics)
	{
		fontMetrics = *pFontMetrics;
	}
}

FLOAT CXTPMarkupTextBlock::GetStackWidth(CLineIterator& li) const
{
	FLOAT nStackWidth = 0;

	for (CLinePart* part = li.arrStack.GetHead(); part != NULL; part = part->m_pNextChain)
	{
		nStackWidth += part->size.Width;
	}

	return nStackWidth;
}

void CXTPMarkupTextBlock::AddEmptyRun(CLineIterator& li)
{
	CXTPMarkupRun* pRun = MARKUP_CREATE(CXTPMarkupRun, m_pMarkupContext);
	pRun->SetLogicalParent(this);
	pRun->SetText(L" ");

	CLinePart* pLinePart = new CLinePart(pRun, pRun->GetContentStartPosition());

	pLinePart->pInline->PrepareMeasure(li.pDC);
	pLinePart->Calculate(li);

	li.arrStack.AddTail(pLinePart);
	pRun->Release();
}

void CXTPMarkupTextBlock::CalculateLines(CXTPMarkupDrawingContext* pDC, int nWidth)
{
	CLineIterator li;
	li.bTextWrapping = GetTextWrapping();
	li.bTextTrimming = li.bTextWrapping ? xtpMarkupTextTrimmingNone : GetTextTrimming();
	li.bTextJustify	 = li.bTextWrapping ? GetTextAlignment() == xtpMarkupTextAlignmentJustify
										: FALSE;

	if (((m_nLastWidth
		  == nWidth) /*|| (!li.bTextWrapping && !li.bTextTrimming && m_nLastWidth != -1)*/)
		&& !pDC->IsPrinting())
		return;

	m_nLastWidth = nWidth;

	RemoveAllLines();

	li.pLine = new CLine();

	li.pDC = pDC;

	CXTPMarkupInline* pInline = GetFirstInline();

	if (!pInline)
	{
		AddEmptyRun(li);
	}

	while (pInline)
	{
		pInline->PrepareMeasure(li.pDC);

		POSITION pos = pInline->GetContentStartPosition();

		li.pLinePart = new CLinePart(pInline, pos);

		while (pos != MARKUP_POSITION_EOF)
		{
			li.pLinePart->posEnd = pos;

			if (pInline->IsCaretReturnPosition(pos))
			{
				pInline->GetContentNextPosition(pos);
			}

			if (li.bTextWrapping && pInline->IsWordBreakPosition(pos))
			{
				li.pLinePart->Calculate(li);

				PushLinePart(li, nWidth);
			}

			if (pInline->IsLineBreakPosition(pos))
			{
				li.pLinePart->Calculate(li);

				PushLinePart(li, nWidth);

				if (li.pLine->m_arrParts.IsEmpty())
				{
					AddEmptyRun(li);
					PushLinePart(li, nWidth);
				}

				CloseLine(li, nWidth);

				pInline->GetContentNextPosition(pos);
				li.pLinePart->posStart	 = pos;
				li.pLinePart->posEnd	 = pos;
				li.pLinePart->bEmptyPart = TRUE;
				continue;
			}

			pInline->GetContentNextPosition(pos);
		}

		if (li.pLinePart->posStart == li.pLinePart->posEnd && !li.pLinePart->bEmptyPart)
		{
			delete li.pLinePart;
		}
		else
		{
			li.pLinePart->Calculate(li);

			li.arrStack.AddTail(li.pLinePart);
		}
		li.pLinePart = NULL;

		pInline = pInline->GetNextInline();
	}

	PushLinePart(li, nWidth);
	CloseLine(li, nWidth);

	delete li.pLine;
	ASSERT(!li.pLinePart);
	ASSERT(li.arrStack.IsEmpty());

	if (pDC->IsPrinting())
		m_nLastWidth = -1;

	m_szBlockSize = CSize(static_cast<int>(li.nMaxWidth), static_cast<int>(li.y));
};

void CXTPMarkupTextBlock::InvalidateMeasureOverride(CXTPMarkupDrawingContext* pDC)
{
	m_nLastWidth = -1;
	CXTPMarkupFrameworkElement::InvalidateMeasureOverride(pDC);
}

CSize CXTPMarkupTextBlock::MeasureOverride(CXTPMarkupDrawingContext* pDC, CSize szAvailableSize)
{
	CSize size3 = GetPadding()->GetSize();

	CSize availableSize(max(0, szAvailableSize.cx - size3.cx),
						max(0, szAvailableSize.cy - size3.cy));

	CalculateLines(pDC, availableSize.cx);

	return CSize(m_szBlockSize.cx + size3.cx, m_szBlockSize.cy + size3.cy);
}

CSize CXTPMarkupTextBlock::ArrangeOverride(CSize szFinalSize)
{
	CXTPMarkupThickness* pPadding = GetPadding();

	XTPMarkupTextAlignment textAlignment = GetTextAlignment();

	for (CLine* pLine = m_pLineList->GetHead(); pLine != NULL; pLine = pLine->m_pNextChain)
	{
		int nJustifyWidth = szFinalSize.cx - static_cast<int>(pLine->m_nWidth) - pPadding->GetLeft()
							- pPadding->GetRight();

		int xOffset = textAlignment == xtpMarkupTextAlignmentRight	  ? nJustifyWidth
					  : textAlignment == xtpMarkupTextAlignmentCenter ? nJustifyWidth / 2
																	  : 0;

		int nJustifyCount = -1;

		if (textAlignment == xtpMarkupTextAlignmentJustify && pLine->m_bAllowJustify
			&& szFinalSize.cx - static_cast<int>(pLine->m_nWidth) > 0)
		{
			for (CLinePart* part = pLine->m_arrParts.GetHead(); part != NULL;
				 part			 = part->m_pNextChain)
				   if (!part->bEmptyPart)
					   nJustifyCount++;
		}

		BOOL bFirstPart = TRUE;
		for (CLinePart* part = pLine->m_arrParts.GetHead(); part != NULL; part = part->m_pNextChain)
		{
			part->ptOffset.SetPoint(static_cast<FLOAT>(pPadding->GetLeft())
										+ static_cast<FLOAT>(xOffset),
									static_cast<FLOAT>(pPadding->GetTop()) + pLine->m_nHeight
										- part->size.Height);

			if (nJustifyCount > 0 && nJustifyWidth > 0 && !bFirstPart && !part->bEmptyPart)
			{
				int nOffset = nJustifyWidth / nJustifyCount;
				if (nOffset == 0)
					nOffset = 1;
				part->ptOffset.X += static_cast<float>(nOffset);
				nJustifyWidth -= nOffset;
				nJustifyCount -= 1;
				xOffset += nOffset;
			}

			if (!part->bEmptyPart)
				bFirstPart = FALSE;

			XTPMarkupBaselineAlignment baseline = part->pInline->GetBaselineAlignment();
			switch (baseline)
			{
				case xtpMarkupBaseline:
					part->ptOffset.Y += static_cast<FLOAT>(part->nBaseline) - pLine->m_nBaseline;
					break;
				case xtpMarkupBaselineSubscript:
					part->ptOffset.Y += static_cast<FLOAT>(part->nBaseline)
										- pLine->m_nBaseline / 2;
					break;

				case xtpMarkupBaselineSuperscript:
					part->ptOffset.Y += part->size.Height - pLine->m_nHeight
										+ pLine->m_nBaseline / 2
										- static_cast<float>(part->nBaseline);
					break;

				case xtpMarkupBaselineBottom:
				case xtpMarkupBaselineTextBottom: break;

				case xtpMarkupBaselineTop:
				case xtpMarkupBaselineTextTop:
					part->ptOffset.Y += part->size.Height - pLine->m_nHeight;
					break;

				case xtpMarkupBaselineCenter:
					part->ptOffset.Y += (part->size.Height - pLine->m_nHeight) / 2;
					break;
			}

			CXTPRectF rcF(part->ptTopLeft, part->size);
			rcF.Offset(part->ptOffset.X, part->ptOffset.Y);

			CRect rc(static_cast<int>(rcF.GetLeft()), static_cast<int>(rcF.GetTop()),
					 static_cast<int>(rcF.GetRight()), static_cast<int>(rcF.GetBottom()));

			part->pInline->Arrange(rc, part->posStart, part->posEnd);
		}
	}

	return szFinalSize;
}

void CXTPMarkupTextBlock::RenderTextDecorations(CXTPMarkupDrawingContext* pDC, CLinePart* part)
{
	CXTPMarkupEnum* pTextDecorations =
		MARKUP_STATICCAST(CXTPMarkupEnum, part->pInline->GetValue(m_pTextDecorationsProperty));
	if (!pTextDecorations || (int)*pTextDecorations == 0)
		return;

	CXTPRectF rc(part->ptTopLeft, part->size);
	rc.Offset(part->ptOffset.X, part->ptOffset.Y);

	int nTopDecoration = 0;
	int nLineHeight	   = max(1, part->nBaseline / 4);
	if (pTextDecorations->GetValue() == 1) // Underline
	{
		int nLineTop   = static_cast<int>(rc.GetBottom()) - part->nBaseline + nLineHeight;
		nTopDecoration = nLineTop + ((static_cast<int>(rc.GetBottom()) - nLineTop) % 2);
	}
	else if (pTextDecorations->GetValue() == 2) // Strikethrough
	{
		nTopDecoration = static_cast<int>(rc.GetBottom()) - part->nBaseline
						 - (part->fontMetrics.nAscent - part->fontMetrics.nDescent - nLineHeight)
							   / 2;
	}

	CXTPRectF rcDecorationF(rc.GetLeft(), static_cast<FLOAT>(nTopDecoration), rc.Width,
							static_cast<FLOAT>(nLineHeight));
	CRect rcDecoration(static_cast<int>(rcDecorationF.GetLeft()),
					   static_cast<int>(rcDecorationF.GetTop()),
					   static_cast<int>(rcDecorationF.GetRight()),
					   static_cast<int>(rcDecorationF.GetBottom()));

	CXTPMarkupObject* pDecorationsOwner = part->pInline->GetValueSource(m_pTextDecorationsProperty);
	if (!pDecorationsOwner)
	{
		ASSERT(FALSE);
		return;
	}

	CXTPMarkupBrush* pDecorationsBrush =
		MARKUP_STATICCAST(CXTPMarkupBrush, pDecorationsOwner->GetValue(m_pForegroundProperty));
	if (pDecorationsBrush)
	{
		pDC->FillRectangle(rcDecoration, pDecorationsBrush);
	}
	else
	{
		CXTPMarkupSolidColorBrush brush(GetMarkupContext()->GetDefaultForeground());
		pDC->FillRectangle(rcDecoration, &brush);
	}
}

void CXTPMarkupTextBlock::OnRender(CXTPMarkupDrawingContext* pDC)
{
	CXTPMarkupBrush* pTextBackground = GetBackground();

	if (pTextBackground != NULL)
	{
		CRect rcRender(CPoint(0, 0), GetActualSize());
		pDC->FillRectangle(rcRender, pTextBackground);
	}

	RECT rcClipBox = pDC->GetClipBox(), rcIntersect;

	CXTPMarkupSolidColorBrush brushDefaultForeground(m_pMarkupContext->GetDefaultForeground());
	CXTPMarkupBrush* pSelectedForeground = NULL;

	for (CLine* pLine = m_pLineList->GetHead(); pLine != NULL; pLine = pLine->m_pNextChain)
	{
		for (CLinePart* part = pLine->m_arrParts.GetHead(); part != NULL; part = part->m_pNextChain)
		{
			CXTPRectF rcF(part->ptTopLeft, part->size);
			rcF.Offset(part->ptOffset.X, part->ptOffset.Y);

			CRect rc(static_cast<int>(rcF.GetLeft()), static_cast<int>(rcF.GetTop()),
					 static_cast<int>(rcF.GetRight()), static_cast<int>(rcF.GetBottom()));

			if (!IntersectRect(&rcIntersect, &rc, &rcClipBox))
				continue;

			CXTPMarkupBrush* pForeground = part->pInline->GetForeground();
			CXTPMarkupBrush* pBackground = part->pInline->GetBackground();

			if (pBackground != NULL && pBackground != pTextBackground)
			{
				pDC->FillRectangle(rc, pBackground);
			}

			if (!pForeground)
				pForeground = &brushDefaultForeground;

			if (pForeground != pSelectedForeground)
			{
				pDC->SetTextColor(pForeground);
				pSelectedForeground = pForeground;
			}

			part->pInline->Render(pDC, rc, part->posStart, part->posEnd);

			RenderTextDecorations(pDC, part);
		}
	}

	pDC->SetTextColor(NULL);
}

void CXTPMarkupTextBlock::SetContentObject(CXTPMarkupBuilder* pBuilder, CXTPMarkupObject* pContent)
{
	m_pInlines->SetContentObject(pBuilder, pContent);
}

BOOL CXTPMarkupTextBlock::HasContentObject() const
{
	return m_pInlines->HasContentObject();
}

BOOL CXTPMarkupTextBlock::AllowWhiteSpaceContent() const
{
	return TRUE;
}

CXTPMarkupInputElement* CXTPMarkupTextBlock::InputHitTestOverride(CPoint point) const
{
	for (CLine* pLine = m_pLineList->GetHead(); pLine != NULL; pLine = pLine->m_pNextChain)
	{
		for (CLinePart* part = pLine->m_arrParts.GetHead(); part != NULL; part = part->m_pNextChain)
		{
			CXTPRectF rcF(part->ptTopLeft, part->size);
			rcF.Offset(part->ptOffset.X, part->ptOffset.Y);

			CRect rc(static_cast<int>(rcF.GetLeft()), static_cast<int>(rcF.GetTop()),
					 static_cast<int>(rcF.GetRight()), static_cast<int>(rcF.GetBottom()));

			if (PtInRectTransform(rc, point))
				return part->pInline->InputHitTest(point);
		}
	}

	return (CXTPMarkupInputElement*)this;
}

void CXTPMarkupTextBlock::SetPadding(int nLeft, int nTop, int nRight, int nBottom)
{
	SetValue(m_pPaddingProperty, new CXTPMarkupThickness(nLeft, nTop, nRight, nBottom));
}

void CXTPMarkupTextBlock::SetPadding(int padding)
{
	SetValue(m_pPaddingProperty, new CXTPMarkupThickness(padding));
}

CXTPMarkupThickness* CXTPMarkupTextBlock::GetPadding() const
{
	return MARKUP_STATICCAST(CXTPMarkupThickness, GetValue(m_pPaddingProperty));
}

XTPMarkupTextWrapping CXTPMarkupTextBlock::GetTextWrapping() const
{
	CXTPMarkupEnum* pTextWrapping = MARKUP_STATICCAST(CXTPMarkupEnum,
													  GetValue(m_pTextWrappingProperty));
	return pTextWrapping != NULL ? (XTPMarkupTextWrapping)(int)*pTextWrapping : xtpMarkupTextNoWrap;
}

XTPMarkupTextTrimming CXTPMarkupTextBlock::GetTextTrimming() const
{
	CXTPMarkupEnum* pTextTrimming = MARKUP_STATICCAST(CXTPMarkupEnum,
													  GetValue(m_pTextTrimmingProperty));
	return pTextTrimming != NULL ? (XTPMarkupTextTrimming)(int)*pTextTrimming
								 : (XTPMarkupTextTrimming)xtpMarkupTextTrimmingNone;
}

#include "Common/Base/Diagnostic/XTPBeginAfxMap.h"
BEGIN_DISPATCH_MAP(CXTPMarkupTextBlock, CXTPMarkupFrameworkElement)
	DISP_PROPERTY_EX_ID(CXTPMarkupTextBlock, "TextWrapping", 500, OleGetTextWrapping,
						OleSetTextWrapping, VT_I4)
	DISP_PROPERTY_EX_ID(CXTPMarkupTextBlock, "TextTrimming", 501, OleGetTextTrimming,
						OleSetTextTrimming, VT_I4)
	DISP_PROPERTY_EX_ID(CXTPMarkupTextBlock, "TextAlignment", 502, OleGetTextAlignment,
						OleSetTextAlignment, VT_I4)
	DISP_PROPERTY_EX_ID(CXTPMarkupTextBlock, "TextDecorations", 503, OleGetTextDecorations,
						OleSetTextDecorations, VT_I4)
	DISP_PROPERTY_EX_ID(CXTPMarkupTextBlock, "FontSize", 504, OleGetFontSize, OleSetFontSize, VT_I4)
	DISP_PROPERTY_EX_ID(CXTPMarkupTextBlock, "FontWeight", 505, OleGetFontWeight, OleSetFontWeight,
						VT_I4)
	DISP_PROPERTY_EX_ID(CXTPMarkupTextBlock, "FontFamily", 506, OleGetFontFamily, OleSetFontFamily,
						VT_BSTR)
	DISP_PROPERTY_EX_ID(CXTPMarkupTextBlock, "FontStyle", 507, OleGetFontStyle, OleSetFontStyle,
						VT_I4)
	DISP_PROPERTY_EX_ID(CXTPMarkupTextBlock, "Padding", 508, OleGetPadding, OleSetPadding,
						VT_DISPATCH)
	DISP_PROPERTY_EX_ID(CXTPMarkupTextBlock, "FontQuality", 509, OleGetFontQuality,
						OleSetFontQuality, VT_I4)
	DISP_PROPERTY_EX_ID(CXTPMarkupTextBlock, "Background", 510, OleGetBackground, OleSetBackground,
						VT_DISPATCH)
	DISP_PROPERTY_EX_ID(CXTPMarkupTextBlock, "Foreground", 511, OleGetForeground, OleSetForeground,
						VT_DISPATCH)
	DISP_PROPERTY_EX_ID(CXTPMarkupTextBlock, "Text", 512, OleGetText, OleSetText, VT_VARIANT)
	DISP_PROPERTY_EX_ID(CXTPMarkupTextBlock, "Inlines", 513, OleGetInlines, SetNotSupported,
						VT_DISPATCH)
END_DISPATCH_MAP()
#include "Common/Base/Diagnostic/XTPEndAfxMap.h"

#ifdef _XTP_ACTIVEX

BEGIN_INTERFACE_MAP(CXTPMarkupTextBlock, CXTPMarkupFrameworkElement)
	INTERFACE_PART(CXTPMarkupTextBlock, XTPDIID_MarkupTextBlock, Dispatch)
END_INTERFACE_MAP()

IMPLEMENT_OLETYPELIB_EX(CXTPMarkupTextBlock, XTPDIID_MarkupTextBlock)
#endif

VARIANT CXTPMarkupTextBlock::OleGetText()
{
	VARIANT v;
	v.vt = VT_BSTR;

	if (m_pInlines->GetCount() != 1
		|| MARKUP_DYNAMICCAST(CXTPMarkupRun, m_pInlines->GetInline(0)) == 0)
		v.bstrVal = ::SysAllocString(L"");
	else
		v.bstrVal = ::SysAllocString(((CXTPMarkupRun*)m_pInlines->GetInline(0))->GetTextW());

	return v;
}

void CXTPMarkupTextBlock::OleSetText(const VARIANT& lpCaption)
{
	VARIANT varCaption;
	VariantInit(&varCaption);
	VariantChangeType(&varCaption, (VARIANTARG*)&lpCaption, 0, VT_BSTR);

	if (varCaption.bstrVal != NULL)
	{
		SetText(varCaption.bstrVal);
	}
	else
	{
		SetText(L"");
	}

	VariantClear(&varCaption);
}

long CXTPMarkupTextBlock::OleGetTextWrapping()
{
	return GetTextWrapping();
}

void CXTPMarkupTextBlock::OleSetTextWrapping(long nValue)
{
	SetTextWrapping(nValue);
}

long CXTPMarkupTextBlock::OleGetTextTrimming()
{
	return GetTextTrimming();
}

void CXTPMarkupTextBlock::OleSetTextTrimming(long nValue)
{
	SetTextTrimming((XTPMarkupTextTrimming)nValue);
}

long CXTPMarkupTextBlock::OleGetTextAlignment()
{
	return GetTextAlignment();
}
void CXTPMarkupTextBlock::OleSetTextAlignment(long nValue)
{
	SetTextAlignment((XTPMarkupTextAlignment)nValue);
}

long CXTPMarkupTextBlock::OleGetTextDecorations()
{
	return GetTextDecorations();
}

void CXTPMarkupTextBlock::OleSetTextDecorations(long nValue)
{
	SetTextDecorations(nValue);
}

long CXTPMarkupTextBlock::OleGetFontSize()
{
	return GetFontSize();
}

void CXTPMarkupTextBlock::OleSetFontSize(long nValue)
{
	SetFontSize(nValue);
}

long CXTPMarkupTextBlock::OleGetFontWeight()
{
	return GetFontWeight();
}

void CXTPMarkupTextBlock::OleSetFontWeight(long nValue)
{
	SetFontWeight(nValue);
}

BSTR CXTPMarkupTextBlock::OleGetFontFamily()
{
	return SysAllocString(GetFontFamily());
}

void CXTPMarkupTextBlock::OleSetFontFamily(LPCTSTR lpszValue)
{
	SetFontFamily(XTP_CT2CW(lpszValue));
}

long CXTPMarkupTextBlock::OleGetFontStyle()
{
	return GetFontStyle();
}

void CXTPMarkupTextBlock::OleSetFontStyle(long nValue)
{
	SetFontStyle(nValue);
}

LPDISPATCH CXTPMarkupTextBlock::OleGetPadding()
{
	return XTPGetDispatch(GetPadding());
}

void CXTPMarkupTextBlock::OleSetPadding(LPDISPATCH lpPaddingDisp)
{
	SetValue(m_pPaddingProperty,
			 MARKUP_STATICCAST(CXTPMarkupThickness, FromDispatch(lpPaddingDisp)));
}

long CXTPMarkupTextBlock::OleGetFontQuality()
{
	CXTPMarkupEnum* pValue = MARKUP_STATICCAST(CXTPMarkupEnum, GetValue(m_pFontQualityProperty));
	return pValue ? (int)*pValue : 0;
}

void CXTPMarkupTextBlock::OleSetFontQuality(long nValue)
{
	SetValue(m_pFontQualityProperty, CXTPMarkupEnum::CreateValue(nValue));
}

LPDISPATCH CXTPMarkupTextBlock::OleGetBackground()
{
	return XTPGetDispatch(GetBackground());
}

void CXTPMarkupTextBlock::OleSetBackground(LPDISPATCH lpBackground)
{
	SetBackground(MARKUP_STATICCAST(CXTPMarkupBrush, FromDispatch(lpBackground)));
}

LPDISPATCH CXTPMarkupTextBlock::OleGetForeground()
{
	return XTPGetDispatch(GetForeground());
}

void CXTPMarkupTextBlock::OleSetForeground(LPDISPATCH lpBackground)
{
	SetForeground(MARKUP_STATICCAST(CXTPMarkupBrush, FromDispatch(lpBackground)));
}

LPDISPATCH CXTPMarkupTextBlock::OleGetInlines()
{
	return XTPGetDispatch(GetInlines());
}
