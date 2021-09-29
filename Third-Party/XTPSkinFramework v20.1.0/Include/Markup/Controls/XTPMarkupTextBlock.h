// XTPMarkupTextBlock.h: interface for the CXTPMarkupTextBlock class.
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
#if !defined(__XTPMARKUPTEXTBLOCK_H__)
#	define __XTPMARKUPTEXTBLOCK_H__
//}}AFX_CODEJOCK_PRIVATE

#	if _MSC_VER > 1000
#		pragma once
#	endif // _MSC_VER > 1000

#	include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

class CXTPMarkupBrush;
class CXTPMarkupInlineCollection;
class CXTPMarkupInline;
class CXTPMarkupDrawingContext;
class CXTPMarkupRenderTransform;
template<class TYPE>
class CXTPMarkupTypedSimpleStack;

//-----------------------------------------------------------------------
// Summary:
//     Text trimming type.
// Example:
//     <code>pTexBlock->SetTextTrimming(xtpMarkupTextTrimmingCharacterEllipsis)</code>
// See Also:
//     CXTPMarkupTextBlock
//-----------------------------------------------------------------------
enum XTPMarkupTextTrimming
{
	xtpMarkupTextTrimmingNone,				// Drawing without trimming.
	xtpMarkupTextTrimmingCharacterEllipsis, // Replaces part of the given string with ellipses, if
											// necessary.
	xtpMarkupTextTrimmingWordEllipsis, // Replaces last word of the given string with ellipses, if
									   // necessary.
};

//-----------------------------------------------------------------------
// Summary:
//     Text wrapping type.
// Example:
//     <code>pTexBlock->SetTextWrapping(xtpMarkupTextWrap)</code>
// See Also:
//     CXTPMarkupTextBlock
//-----------------------------------------------------------------------
enum XTPMarkupTextWrapping
{
	xtpMarkupTextNoWrap, // No line wrapping is performed.
	xtpMarkupTextWrap,	 // Line-breaking occurs if the line overflows beyond the available block
						 // width, even if the standard line breaking algorithm cannot determine any
						 // line break opportunity.
	xtpMarkupTextWrapWithOverflow // Line-breaking occurs if the line overflows beyond the available
								  // block width. However, a line may overflow beyond the block
								  // width if the line breaking algorithm cannot determine a line
								  // break opportunity.
};

//===========================================================================
// Summary:
//     CXTPMarkupTextBlock is a CXTPMarkupFrameworkElement derived class.
//     It implements the TextBlock XAML Tag.
//===========================================================================
class _XTP_EXT_CLASS CXTPMarkupTextBlock : public CXTPMarkupFrameworkElement
{
	DECLARE_MARKUPCLASS(CXTPMarkupTextBlock)

private:
	struct CLine;
	struct CLineIterator;
	struct CLinePart;

protected:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPMarkupTextBlock object.
	//-----------------------------------------------------------------------
	CXTPMarkupTextBlock();

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPMarkupTextBlock object, handles cleanup and deallocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPMarkupTextBlock();

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Sets the text for the TextBlock object.
	// Parameters:
	//     lpszText - New text to be set.
	//-----------------------------------------------------------------------
	void SetText(LPCWSTR lpszText);

	//-----------------------------------------------------------------------
	// Summary:
	//     Gets the current text of the TextBlock object.
	// Returns:
	//     A CString object containing the current text of the TextBlock object.
	//-----------------------------------------------------------------------
	CString GetText() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Gets a pointer to the CXTPMarkupInlineCollection object containing
	//     the TextBlock object's inline blocks.
	// Returns:
	//     A pointer to the CXTPMarkupInlineCollection object containing
	//     the TextBlock object's inline blocks.
	//-----------------------------------------------------------------------
	CXTPMarkupInlineCollection* GetInlines() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets the background brush for the TextBlock object.
	// Parameters:
	//     brush - New background brush for the TextBlock object.
	//-----------------------------------------------------------------------
	void SetBackground(CXTPMarkupBrush* brush);

	//-----------------------------------------------------------------------
	// Summary:
	//     Gets the background brush of the TextBlock object.
	// Returns:
	//     A pointer to the background brush of the TextBlock object.
	//-----------------------------------------------------------------------
	CXTPMarkupBrush* GetBackground() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets the default text brush for the TextBlock object.
	// Parameters:
	//     brush - New default text brush for the TextBlock object.
	//-----------------------------------------------------------------------
	void SetForeground(CXTPMarkupBrush* brush);

	//-----------------------------------------------------------------------
	// Summary:
	//     Gets the default text brush of the TextBlock object.
	// Returns:
	//     A pointer to the default text brush of the TextBlock object.
	//-----------------------------------------------------------------------
	CXTPMarkupBrush* GetForeground() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets the text alignment for the TextBlock object.
	// Parameters:
	//     alignment - New alignment to be set; must be one of the values
	//                 defined by the XTPMarkupTextAlignment enumeration.
	// See Also:
	//     XTPMarkupTextAlignment, GetTextAlignment
	//-----------------------------------------------------------------------
	void SetTextAlignment(XTPMarkupTextAlignment alignment);

	//-----------------------------------------------------------------------
	// Summary:
	//     Gets the text alignment of the TextBlock object.
	// Returns:
	//     The text alignment of the TextBlock object; returns one of the
	//     values defined by the XTPMarkupTextAlignment enumeration.
	// See Also:
	//     XTPMarkupTextAlignment, SetTextAlignment
	//-----------------------------------------------------------------------
	XTPMarkupTextAlignment GetTextAlignment() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets the text wrapping type for the bounding rectangle
	//     (how text will be wrapped inside the bounding rectangle).
	// Parameters:
	//     nTextWrapping - New text wrapping type to be set; must be one of the values
	//                     defined by the XTPMarkupTextWrapping enumeration.
	// See Also:
	//     XTPMarkupTextWrapping, SetTextWrapping
	//-----------------------------------------------------------------------
	void SetTextWrapping(XTPMarkupTextWrapping nTextWrapping);
	void SetTextWrapping(BOOL bTextWrapping);

	//-----------------------------------------------------------------------
	// Summary:
	//     Gets the text wrapping type of the bounding rectangle
	//     (how text is wrapped inside the bounding rectangle).
	// Returns:
	//     The text wrapping type; returns one of the values defined by
	//     the XTPMarkupTextWrapping enumeration.
	// See Also:
	//     XTPMarkupTextWrapping, SetTextWrapping
	//-----------------------------------------------------------------------
	XTPMarkupTextWrapping GetTextWrapping() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets the text trimming style
	//     (how text will be trimmed when content overflows the content area).
	// Parameters:
	//     bTextTrimming - New text trimming style to be set; must be one of the values
	//                     defined by the XTPMarkupTextTrimming enumeration.
	// Remarks:
	//     The TextTrimming property has no effect unless the TextWrapping
	//     property is set to NoWrap.
	//-----------------------------------------------------------------------
	void SetTextTrimming(XTPMarkupTextTrimming bTextTrimming);

	//-----------------------------------------------------------------------
	// Summary:
	//     Gets the current text trimming style
	//     (how text is trimmed when content overflows the content area).
	// Returns:
	//     The current text trimming style; returns one of the values defined by
	//     the XTPMarkupTextTrimming enumeration.
	//-----------------------------------------------------------------------
	XTPMarkupTextTrimming GetTextTrimming() const;

	// ---------------------------------------------------------------------
	// Summary:
	//     Sets a value that indicates the thickness of the padding space
	//     between the boundaries of the content area and the content
	//     displayed by a TextBlock.
	// Parameters:
	//     nLeft    - Left space.
	//     nTop     - Top space.
	//     nRight   - Right space.
	//     nBottom  - Bottom space.
	//     nPadding - All borders.
	//-----------------------------------------------------------------------
	void SetPadding(int nLeft, int nTop, int nRight, int nBottom);
	void SetPadding(int nPadding); // <Combine CXTPMarkupTextBlock::SetPadding@int@int@int@int>

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves a value that indicates the thickness of the padding space
	//     between the boundaries of the content area and the content
	//     displayed by a TextBlock.
	// Returns:
	//     A pointer to a CXTPMarkupThickness object that indicates the thickness
	//     of the padding space between the boundaries of the content area
	//     and the content displayed by a TextBlock.
	// See Also:
	//     SetPadding
	//-----------------------------------------------------------------------
	CXTPMarkupThickness* GetPadding() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets the font size for the TextBlock object.
	// Parameters:
	//     nFontSize - New font size to be set.
	//-----------------------------------------------------------------------
	void SetFontSize(int nFontSize);

	//-----------------------------------------------------------------------
	// Summary:
	//     Gets the font size of the TextBlock object.
	// Returns:
	//     The font size of the TextBlock object if it was set.
	//-----------------------------------------------------------------------
	int GetFontSize() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets the font weight for the TextBlock object.
	// Parameters:
	//     nFontWeight - New font weight to be set.
	//-----------------------------------------------------------------------
	void SetFontWeight(int nFontWeight);

	//-----------------------------------------------------------------------
	// Summary:
	//     Gets the font weight of the TextBlock object.
	// Returns:
	//     The font weight of the TextBlock object if it was set.
	//-----------------------------------------------------------------------
	int GetFontWeight() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets the typeface name for the font.
	// Parameters:
	//     lpszFontFamily - New typeface name to be set.
	//-----------------------------------------------------------------------
	void SetFontFamily(LPCWSTR lpszFontFamily);

	//-----------------------------------------------------------------------
	// Summary:
	//     Gets the typeface name of the font.
	// Returns:
	//     The typeface name of the font if it was set, otherwise NULL.
	//-----------------------------------------------------------------------
	LPCWSTR GetFontFamily() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets the font style for the TextBlock object.
	// Parameters:
	//     nFontStyle - New font style to be set.
	// Remarks:
	//     Italic (nFontStyle = 1) and Normal (FontStyle = 0) are supported.
	//-----------------------------------------------------------------------
	void SetFontStyle(int nFontStyle);

	//-----------------------------------------------------------------------
	// Summary:
	//     Gets the font style of the TextBlock object.
	// Returns:
	//     The font style of the TextBlock object if it was set.
	//-----------------------------------------------------------------------
	int GetFontStyle() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets the font text decoration for the TextBlock object.
	// Parameters
	//     nTextDecorations - New font text decoration to be set.
	// Remarks:
	//     Underline (1) and Strikethrough(2) are supported.
	//-----------------------------------------------------------------------
	void SetTextDecorations(int nTextDecorations);

	//-----------------------------------------------------------------------
	// Summary:
	//     Gets the font text decoration of the TextBlock object.
	// Returns
	//     The font text decoration of the TextBlock object if it was set.
	//-----------------------------------------------------------------------
	int GetTextDecorations() const;

	//{{AFX_CODEJOCK_PRIVATE
	// Internal methods

public:
	CXTPMarkupInline* GetFirstInline() const;

public:
	virtual void SetContentObject(CXTPMarkupBuilder* pBuilder, CXTPMarkupObject* pContent);
	virtual BOOL HasContentObject() const;

protected:
	virtual CSize MeasureOverride(CXTPMarkupDrawingContext* pDC, CSize szAvailableSize);
	virtual CSize ArrangeOverride(CSize szFinal);
	virtual void OnRender(CXTPMarkupDrawingContext* pDC);

	virtual BOOL AllowWhiteSpaceContent() const;
	virtual void OnPropertyChanged(CXTPMarkupDependencyProperty* pProperty,
								   CXTPMarkupObject* pOldValue, CXTPMarkupObject* pNewValue);

	virtual int GetLogicalChildrenCount() const;
	virtual CXTPMarkupObject* GetLogicalChild(int nIndex) const;

	CXTPMarkupInputElement* InputHitTestOverride(CPoint point) const;

	void InvalidateMeasureOverride(CXTPMarkupDrawingContext* pDC);

	//}}AFX_CODEJOCK_PRIVATE

private:
	void RemoveAllLines();

	void CloseLine(CLineIterator& li, int nWidth);
	void TrimLine(CLineIterator& li, int nWidth);
	void PushLinePart(CLineIterator& li, int nWidth);

	void CalculateLines(CXTPMarkupDrawingContext* pDC, int nWidth);

	void RenderTextDecorations(CXTPMarkupDrawingContext* pDC, CLinePart* part);
	FLOAT GetStackWidth(CLineIterator& li) const;

	void AddEmptyRun(CLineIterator& li);

	//{{AFX_CODEJOCK_PRIVATE
public:
	DECLARE_DISPATCH_MAP()
#	ifdef _XTP_ACTIVEX
	DECLARE_INTERFACE_MAP()
	DECLARE_OLETYPELIB_EX(CXTPMarkupTextBlock);
#	endif

	afx_msg VARIANT OleGetText();
	afx_msg void OleSetText(const VARIANT& lpCaption);
	afx_msg long OleGetTextWrapping();
	afx_msg void OleSetTextWrapping(long nValue);
	afx_msg long OleGetTextTrimming();
	afx_msg void OleSetTextTrimming(long nValue);
	afx_msg long OleGetTextAlignment();
	afx_msg void OleSetTextAlignment(long nValue);
	afx_msg long OleGetTextDecorations();
	afx_msg void OleSetTextDecorations(long nValue);
	afx_msg long OleGetFontSize();
	afx_msg void OleSetFontSize(long nValue);
	afx_msg long OleGetFontWeight();
	afx_msg void OleSetFontWeight(long nValue);
	afx_msg BSTR OleGetFontFamily();
	afx_msg void OleSetFontFamily(LPCTSTR lpszValue);
	afx_msg long OleGetFontStyle();
	afx_msg void OleSetFontStyle(long nValue);
	afx_msg LPDISPATCH OleGetPadding();
	afx_msg void OleSetPadding(LPDISPATCH lpPaddingDisp);
	afx_msg long OleGetFontQuality();
	afx_msg void OleSetFontQuality(long nValue);
	afx_msg LPDISPATCH OleGetBackground();
	afx_msg void OleSetBackground(LPDISPATCH lpBackground);
	afx_msg LPDISPATCH OleGetForeground();
	afx_msg void OleSetForeground(LPDISPATCH lpBackground);
	afx_msg LPDISPATCH OleGetInlines();
	//}}AFX_CODEJOCK_PRIVATE

protected:
	CXTPMarkupInlineCollection* m_pInlines; // Inline collection of TextBlock.

public:
	static CXTPMarkupDependencyProperty* m_pBackgroundProperty;
	static CXTPMarkupDependencyProperty* m_pForegroundProperty;
	static CXTPMarkupDependencyProperty* m_pTextWrappingProperty;
	static CXTPMarkupDependencyProperty* m_pTextTrimmingProperty;
	static CXTPMarkupDependencyProperty* m_pTextAlignmentProperty;
	static CXTPMarkupDependencyProperty* m_pTextDecorationsProperty;
	static CXTPMarkupDependencyProperty* m_pFontSizeProperty;
	static CXTPMarkupDependencyProperty* m_pFontWeightProperty;
	static CXTPMarkupDependencyProperty* m_pFontFamilyProperty;
	static CXTPMarkupDependencyProperty* m_pFontStyleProperty;
	static CXTPMarkupDependencyProperty* m_pPaddingProperty;
	static CXTPMarkupDependencyProperty* m_pTextProperty;
	static CXTPMarkupDependencyProperty* m_pFontQualityProperty;
	static CXTPMarkupDependencyProperty* m_pFontCharsetProperty;

private:
	CXTPMarkupTypedSimpleStack<CLine>* m_pLineList;
	int m_nLastWidth;
	CSize m_szBlockSize;
};

AFX_INLINE CXTPMarkupInlineCollection* CXTPMarkupTextBlock::GetInlines() const
{
	return m_pInlines;
}
AFX_INLINE void CXTPMarkupTextBlock::SetTextWrapping(XTPMarkupTextWrapping nTextWrapping)
{
	SetValue(m_pTextWrappingProperty, CXTPMarkupEnum::CreateValue(nTextWrapping));
}
AFX_INLINE void CXTPMarkupTextBlock::SetTextWrapping(BOOL bTextWrapping)
{
	SetValue(m_pTextWrappingProperty,
			 CXTPMarkupEnum::CreateValue(bTextWrapping ? xtpMarkupTextWrapWithOverflow
													   : xtpMarkupTextNoWrap));
}
AFX_INLINE void CXTPMarkupTextBlock::SetTextTrimming(XTPMarkupTextTrimming bTextTrimming)
{
	SetValue(m_pTextTrimmingProperty, CXTPMarkupEnum::CreateValue(bTextTrimming));
}
AFX_INLINE int CXTPMarkupTextBlock::GetLogicalChildrenCount() const
{
	return m_pInlines != NULL ? 1 : 0;
}
AFX_INLINE CXTPMarkupObject* CXTPMarkupTextBlock::GetLogicalChild(int nIndex) const
{
	return nIndex == 0 ? (CXTPMarkupObject*)m_pInlines : NULL;
}

#	include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPMARKUPTEXTBLOCK_H__)
