// XTPMarkupDrawingContext.h: interface for the CXTPMarkupDrawingContext class.
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
#if !defined(__XTPMARKUPDRAWINGCONTEXT_H__)
#	define __XTPMARKUPDRAWINGCONTEXT_H__
//}}AFX_CODEJOCK_PRIVATE

#	if _MSC_VER > 1000
#		pragma once
#	endif // _MSC_VER > 1000

#	include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

class CXTPMarkupDeviceContext;
class CXTPMarkupPathData;
class CXTPMarkupDoubleCollection;
class CXTPMarkupContext;
class CXTPMarkupBrush;
class CXTPMarkupThickness;
class CXTPMarkupTransformationMatrix;
class CXTPMarkupDeviceDependentImage;
class CXTPImageManagerIcon;
struct XTPMarkupFontMetrics;

namespace Gdiplus
{
class GpGraphics;
class Graphics;
} // namespace Gdiplus

class _XTP_EXT_CLASS CXTPMarkupStrokeStyle
{
public:
	int nStrokeThickness;
	CXTPMarkupBrush* pStrokeBrush;
	CXTPMarkupDoubleCollection* pStrokeDashArray;
	XTPMarkupLineCap nStrokeStartLineCap;
	XTPMarkupLineCap nStrokeEndLineCap;
	XTPMarkupLineJoin nStrokeLineJoin;
};

class _XTP_EXT_CLASS CXTPMarkupBrush : public CXTPMarkupObject
{
	DECLARE_MARKUPCLASS(CXTPMarkupBrush);

public:
	CXTPMarkupObject* ConvertFrom(CXTPMarkupBuilder* pBuilder, CXTPMarkupObject* pObject) const;

public:
	virtual COLORREF GetHintColor() const;

	//{{AFX_CODEJOCK_PRIVATE
public:
	DECLARE_DISPATCH_MAP()
#	ifdef _XTP_ACTIVEX
	DECLARE_INTERFACE_MAP()
	DECLARE_OLETYPELIB_EX(CXTPMarkupBrush);
#	endif

	//}}AFX_CODEJOCK_PRIVATE
};

class _XTP_EXT_CLASS CXTPMarkupBrushKey : public CXTPMarkupObject
{
	DECLARE_MARKUPCLASS(CXTPMarkupBrushKey);

public:
	CXTPMarkupBrushKey(int nIndex = 0)
		: CXTPMarkupObject(CXTPMarkupObject::flagNoCom)
		, m_nIndex(nIndex)
	{
	}

public:
	int m_nIndex;
};

class _XTP_EXT_CLASS CXTPMarkupSolidColorBrush : public CXTPMarkupBrush
{
	DECLARE_MARKUPCLASS(CXTPMarkupSolidColorBrush);

public:
	CXTPMarkupSolidColorBrush();
	CXTPMarkupSolidColorBrush(COLORREF clr);
	CXTPMarkupSolidColorBrush(BYTE bAlpha, COLORREF clr);

public:
	COLORREF GetHintColor() const;
	CXTPMarkupColor* GetColor() const;

public:
	BOOL IsEqual(const CXTPMarkupObject* pObject) const;

	//{{AFX_CODEJOCK_PRIVATE
public:
	DECLARE_DISPATCH_MAP()
#	ifdef _XTP_ACTIVEX
	DECLARE_INTERFACE_MAP()
	DECLARE_OLETYPELIB_EX(CXTPMarkupSolidColorBrush);
#	endif

	afx_msg COLORREF OleGetColor();
	afx_msg void OleSetColor(COLORREF clr);
	//}}AFX_CODEJOCK_PRIVATE

protected:
	static CXTPMarkupDependencyProperty* m_pColorProperty;
};

class _XTP_EXT_CLASS CXTPMarkupGradientStop : public CXTPMarkupObject
{
	DECLARE_MARKUPCLASS(CXTPMarkupGradientStop);

public:
	CXTPMarkupGradientStop();
	CXTPMarkupGradientStop(COLORREF clr, double dOffset = 0);

public:
	COLORREF GetColor() const;
	double GetOffset() const;

protected:
protected:
	static CXTPMarkupDependencyProperty* m_pColorProperty;
	static CXTPMarkupDependencyProperty* m_pOffsetProperty;
};

class _XTP_EXT_CLASS CXTPMarkupGradientStops : public CXTPMarkupCollection
{
	DECLARE_MARKUPCLASS(CXTPMarkupGradientStops);

public:
	CXTPMarkupGradientStops();

public:
	CXTPMarkupGradientStop* GetItem(int nIndex) const;
};

class _XTP_EXT_CLASS CXTPMarkupPoint : public CXTPMarkupObject
{
	DECLARE_MARKUPCLASS(CXTPMarkupPoint);

public:
	CXTPMarkupPoint();
	CXTPMarkupPoint(double x, double y);

public:
	CXTPMarkupPoint& operator=(const CXTPMarkupPoint& srcPoint);

protected:
	CXTPMarkupObject* ConvertFrom(CXTPMarkupBuilder* pBuilder, CXTPMarkupObject* pObject) const;

public:
	double x;
	double y;
};

class _XTP_EXT_CLASS CXTPMarkupPointCollection : public CXTPMarkupObject
{
	DECLARE_MARKUPCLASS(CXTPMarkupPointCollection)
public:
	typedef CArray<Gdiplus::GpPointF, Gdiplus::GpPointF&> CPointArray;

public:
	CXTPMarkupPointCollection();
	CXTPMarkupPointCollection(CPointArray& arr);

public:
	CRect GetBounds() const;
	const CPointArray& GetPoints() const;

	void Stretch(CPointArray& arr, CSize sz);

protected:
	CXTPMarkupObject* ConvertFrom(CXTPMarkupBuilder* pBuilder, CXTPMarkupObject* pObject) const;
	static BOOL AFX_CDECL ConvertFromString(CXTPMarkupBuilder* pBuilder, LPCWSTR lpszValue,
											CPointArray& arr);
	static BOOL AFX_CDECL GetNextValue(LPCWSTR& lpszValue, float& dValue);

protected:
	CPointArray m_arrPoints;
	CRect m_rcBounds;
};

class _XTP_EXT_CLASS CXTPMarkupLinearGradientBrush : public CXTPMarkupBrush
{
public:
	DECLARE_MARKUPCLASS(CXTPMarkupLinearGradientBrush);

public:
	CXTPMarkupLinearGradientBrush();
	CXTPMarkupLinearGradientBrush(CXTPMarkupGradientStops* pGradientStops);
	~CXTPMarkupLinearGradientBrush();

public:
	CXTPMarkupGradientStops* GetGradientStops() const;
	void SetStartPoint(double x, double y);
	void SetEndPoint(double x, double y);

	COLORREF GetHintColor() const;

protected:
	void SetContentObject(CXTPMarkupBuilder* pBuilder, CXTPMarkupObject* pContent);

public:
	static CXTPMarkupDependencyProperty* m_pStartPointProperty;
	static CXTPMarkupDependencyProperty* m_pEndPointProperty;
	static CXTPMarkupDependencyProperty* m_pGradientStopsProperty;
};

class _XTP_EXT_CLASS CXTPMarkupFont : public CXTPMarkupObject
{
public:
	CXTPMarkupFont();
	~CXTPMarkupFont();

public:
	CXTPFont m_Font;
	CXTPMarkupFont* m_pNextChain;
	CXTPMarkupContext* m_pMarkupContext;
};

class _XTP_EXT_CLASS CXTPMarkupDrawingContext
{
public:
	CXTPMarkupDrawingContext(CXTPMarkupContext* pMarkupContext, HDC hDC, HDC hAttribDC = 0);
	CXTPMarkupDrawingContext(CXTPMarkupContext* pMarkupContext);
	CXTPMarkupDrawingContext(CXTPMarkupContext* pMarkupContext, Gdiplus::GpGraphics* pGpGraphics);
	CXTPMarkupDrawingContext(CXTPMarkupContext* pMarkupContext, Gdiplus::Graphics* pGraphics);
	CXTPMarkupDrawingContext(HDC hDC);
	CXTPMarkupDrawingContext();

public:
	virtual ~CXTPMarkupDrawingContext();

protected:
	void Init(CXTPMarkupContext* pMarkupContext);

public:
	void DrawFrame(CRect rc, CXTPMarkupBrush* pBrush);
	void FillSolidRect(CRect rc, COLORREF clr);

	void SetFont(CXTPMarkupFont* pFont);
	void SetTextColor(CXTPMarkupBrush* pBrush);

	double GetOpacity() const;
	void SetOpacity(double dOpacity) const;

	HDC Detach();

	CXTPMarkupTransformationMatrix* GetTransformationMatrix() const;
	void SetTransformationMatrix(CXTPMarkupTransformationMatrix* pMatrix);

	void OffsetViewport(const POINT& ptViewortOrg);
	XTPMarkupSmoothingMode SetSmoothingMode(XTPMarkupSmoothingMode smoothingMode);
	XTPMarkupSmoothingMode SetSmoothingMode(XTPMarkupSmoothingMode smoothingMode,
											BOOL bUseTextSmoothing);

	void SetUseTextSmoothing(BOOL bUseTextSmoothing);
	BOOL GetUseTextSmoothing() const;

	//{{AFX_CODEJOCK_PRIVATE
	_XTP_DEPRECATED_IN_FAVOR(SetUseTextSmoothing)
	void SetUseTextSmooting(BOOL bUseTextSmoothing);

	_XTP_DEPRECATED_IN_FAVOR(GetUseTextSmoothing)
	BOOL GetUseTextSmooting() const;
	//}}AFX_CODEJOCK_PRIVATE

	RECT GetClipBox() const;
	HRGN SaveClipRegion();
	void RestoreClipRegion(HRGN hrgnClip);
	void IntersectClipRect(const RECT& rcLayoutClip);

	virtual BOOL IsPrinting() const;

	BOOL GetFontMetrics(CXTPMarkupFont* pFont, XTPMarkupFontMetrics* pMetrics);
	SIZE GetTextExtent(LPCWSTR lpszText, int nCount) const;
	void GetTextExtent(LPCWSTR lpszText, int nCount, CXTPSizeF& size) const;
	virtual void DrawTextLine(LPCWSTR lpszText, UINT nCount, LPCRECT lpRect);

	CXTPMarkupDeviceContext* GetDeviceContext();
	HDC GetDC();
	void ReleaseDC(HDC hDC);

public:
	static void AFX_CDECL Register(BOOL bInit);

public:
	void Ellipse(CRect rc, CXTPMarkupStrokeStyle* pStrokeStyle, CXTPMarkupBrush* pFillBrush);
	void DrawRectangle(CRect rc, CXTPMarkupBrush* pBrush, CXTPMarkupThickness* pThickness,
					   double* pCornerRadius = 0);
	void FillRectangle(CRect rc, CXTPMarkupBrush* pBrush, double* pCornerRadius = 0);
	void DrawLine(int x1, int y1, int x2, int y2, CXTPMarkupStrokeStyle* pStrokeStyle);
	void Polyline(const POINT* points, int nCount, CXTPMarkupStrokeStyle* pStrokeStyle);
	void Polyline(const void* points, int nCount, CXTPMarkupStrokeStyle* pStrokeStyle);
	void Polygon(const POINT* points, int nCount, CXTPMarkupStrokeStyle* pStrokeStyle,
				 CXTPMarkupBrush* pFillBrush);
	void Polygon(const void* points, int nCount, CXTPMarkupStrokeStyle* pStrokeStyle,
				 CXTPMarkupBrush* pFillBrush);
	void DrawPath(CXTPMarkupPathData* pData, CXTPMarkupStrokeStyle* pStrokeStyle,
				  CXTPMarkupBrush* pFillBrush);

	void DrawImage(CXTPImageManagerIcon* pIcon, CRect rc);
	void DrawImage(CXTPMarkupDeviceDependentImage* pImage, CRect rc);

public:
	HDWP m_hDWP;

protected:
	HDC m_hDC;
	HDC m_hAttribDC;

	CXTPMarkupFont* m_pSelectedFont;
	HFONT m_hOldFont;
	CRect m_rcClipBox;
	BOOL m_bDeleteDC;

	CXTPMarkupDeviceContext* m_pDeviceContext;

	CXTPMarkupContext* m_pMarkupContext;
	BOOL m_bUseTextSmoothing;
};

class _XTP_EXT_CLASS CXTPMarkupPrintingContext : public CXTPMarkupDrawingContext
{
public:
	CXTPMarkupPrintingContext(CXTPMarkupContext* pMarkupContext, HDC hDC, HDC hAttribDC);
	CXTPMarkupPrintingContext(HDC hDC, HDC hAttribDC);

public:
	virtual BOOL IsPrinting() const;

protected:
};

AFX_INLINE BOOL CXTPMarkupDrawingContext::IsPrinting() const
{
	return FALSE;
}

AFX_INLINE BOOL CXTPMarkupPrintingContext::IsPrinting() const
{
	return TRUE;
}

AFX_INLINE RECT CXTPMarkupDrawingContext::GetClipBox() const
{
	return m_rcClipBox;
}

AFX_INLINE CXTPMarkupDeviceContext* CXTPMarkupDrawingContext::GetDeviceContext()
{
	return m_pDeviceContext;
}

AFX_INLINE void CXTPMarkupDrawingContext::SetUseTextSmoothing(BOOL bUseTextSmoothing)
{
	m_bUseTextSmoothing = bUseTextSmoothing;
}

AFX_INLINE BOOL CXTPMarkupDrawingContext::GetUseTextSmoothing() const
{
	return m_bUseTextSmoothing;
}

AFX_INLINE void CXTPMarkupDrawingContext::SetUseTextSmooting(BOOL bUseTextSmoothing)
{
	SetUseTextSmoothing(bUseTextSmoothing);
}

AFX_INLINE BOOL CXTPMarkupDrawingContext::GetUseTextSmooting() const
{
	return GetUseTextSmoothing();
}

AFX_INLINE CXTPMarkupGradientStops* CXTPMarkupLinearGradientBrush::GetGradientStops() const
{
	return MARKUP_STATICCAST(CXTPMarkupGradientStops, GetValue(m_pGradientStopsProperty));
}
AFX_INLINE CXTPMarkupGradientStop* CXTPMarkupGradientStops::GetItem(int nIndex) const
{
	return nIndex >= 0 && nIndex < m_arrItems.GetSize()
			   ? (CXTPMarkupGradientStop*)m_arrItems[nIndex]
			   : NULL;
}

#	include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPMARKUPDRAWINGCONTEXT_H__)
