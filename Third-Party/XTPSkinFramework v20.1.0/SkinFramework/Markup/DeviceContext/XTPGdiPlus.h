// XTPGdiPlus.h: declaration of CXTPGdiPlus class.
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

#if !defined(__XTPGDIPLUS_H__)
#	define __XTPGDIPLUS_H__

#	if _MSC_VER > 1000
#		pragma once
#	endif // _MSC_VER > 1000

#	include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

#	define DECLARE_GDIPLUS_METHOD_(return, rettype, calltype, proc, proc_param, call_param)       \
	public:                                                                                        \
		rettype(calltype* proc) proc_param;

#	define DECLARE_GDIPLUS_METHOD(calltype, proc, proc_param, call_param)                         \
		DECLARE_GDIPLUS_METHOD_(return, GpStatus, calltype, proc, proc_param, call_param)
#	define DECLARE_GDIPLUS_METHOD_VOID(calltype, proc, proc_param, call_param)                    \
		DECLARE_GDIPLUS_METHOD_(;, void, calltype, proc, proc_param, call_param)

_XTP_DEPRECATED
class CXTPGdiPlus
{
public:
	CXTPGdiPlus();

public:
	//-----------------------------------------------------------------------
	// Summary:
	//      Obsolete.
	//-----------------------------------------------------------------------
	void Register(BOOL bReserved);

	DECLARE_GDIPLUS_METHOD(WINAPI, GdiplusStartup,
						   (ULONG_PTR * token, const GdiplusStartupInput* input,
							GdiplusStartupOutput* output),
						   (token, input, output))
	DECLARE_GDIPLUS_METHOD_VOID(WINAPI, GdiplusShutdown, (ULONG_PTR token), (token))

	DECLARE_GDIPLUS_METHOD(WINGDIPAPI, GdipSetPixelOffsetMode,
						   (GpGraphics * graphics, PixelOffsetMode pixelOffsetMode),
						   (graphics, pixelOffsetMode))
	DECLARE_GDIPLUS_METHOD(WINGDIPAPI, GdipSetPageUnit, (GpGraphics * graphics, GpUnit unit),
						   (graphics, unit))
	DECLARE_GDIPLUS_METHOD(WINGDIPAPI, GdipSetSmoothingMode,
						   (GpGraphics * graphics, SmoothingMode smoothingMode),
						   (graphics, smoothingMode))
	DECLARE_GDIPLUS_METHOD(WINGDIPAPI, GdipSetLinePresetBlend,
						   (GpLineGradient * brush, GDIPCONST ARGB* blend,
							GDIPCONST REAL* positions, INT count),
						   (brush, blend, positions, count))
	DECLARE_GDIPLUS_METHOD(WINGDIPAPI, GdipSetInterpolationMode,
						   (GpGraphics * graphics, InterpolationMode interpolationMode),
						   (graphics, interpolationMode))

	DECLARE_GDIPLUS_METHOD(WINGDIPAPI, GdipCreateFromHDC2,
						   (HDC hdc, HANDLE hDevice, GpGraphics** graphics),
						   (hdc, hDevice, graphics))
	DECLARE_GDIPLUS_METHOD(WINGDIPAPI, GdipCreateFromHDC, (HDC hdc, GpGraphics** graphics),
						   (hdc, graphics))
	DECLARE_GDIPLUS_METHOD(WINGDIPAPI, GdipCreatePath2,
						   (GDIPCONST GpPointF * points, GDIPCONST BYTE* data, INT dataCount,
							GpFillMode fillMode, GpPath** path),
						   (points, data, dataCount, fillMode, path))
	DECLARE_GDIPLUS_METHOD(WINGDIPAPI, GdipCreateSolidFill, (ARGB color, GpSolidFill** brush),
						   (color, brush))

	DECLARE_GDIPLUS_METHOD(WINGDIPAPI, GdipCreateLineBrush,
						   (GDIPCONST GpPointF * point1, GDIPCONST GpPointF* point2, ARGB color1,
							ARGB color2, GpWrapMode wrapMode, GpLineGradient** lineGradient),
						   (point1, point2, color1, color2, wrapMode, lineGradient))
	DECLARE_GDIPLUS_METHOD(WINGDIPAPI, GdipCreatePen1,
						   (ARGB color, REAL width, GpUnit unit, GpPen** pen),
						   (color, width, unit, pen))
	DECLARE_GDIPLUS_METHOD(WINGDIPAPI, GdipCreatePen2,
						   (GpBrush * brush, REAL width, GpUnit unit, GpPen** pen),
						   (brush, width, unit, pen))

	DECLARE_GDIPLUS_METHOD(WINGDIPAPI, GdipDeleteGraphics, (GpGraphics * graphics), (graphics))
	DECLARE_GDIPLUS_METHOD(WINGDIPAPI, GdipDeleteBrush, (GpBrush * brush), (brush))
	DECLARE_GDIPLUS_METHOD(WINGDIPAPI, GdipDeletePen, (GpPen * pen), (pen))
	DECLARE_GDIPLUS_METHOD(WINGDIPAPI, GdipDeletePath, (GpPath * path), (path))

	DECLARE_GDIPLUS_METHOD(WINGDIPAPI, GdipDrawEllipse,
						   (GpGraphics * graphics, GpPen* pen, REAL x, REAL y, REAL width,
							REAL height),
						   (graphics, pen, x, y, width, height))
	DECLARE_GDIPLUS_METHOD(WINGDIPAPI, GdipDrawLines,
						   (GpGraphics * graphics, GpPen* pen, GDIPCONST GpPointF* points,
							INT count),
						   (graphics, pen, points, count))
	DECLARE_GDIPLUS_METHOD(WINGDIPAPI, GdipDrawLinesI,
						   (GpGraphics * graphics, GpPen* pen, GDIPCONST GpPoint* points,
							INT count),
						   (graphics, pen, points, count))
	DECLARE_GDIPLUS_METHOD(WINGDIPAPI, GdipDrawPolygon,
						   (GpGraphics * graphics, GpPen* pen, GDIPCONST GpPointF* points,
							INT count),
						   (graphics, pen, points, count))
	DECLARE_GDIPLUS_METHOD(WINGDIPAPI, GdipDrawPolygonI,
						   (GpGraphics * graphics, GpPen* pen, GDIPCONST GpPoint* points,
							INT count),
						   (graphics, pen, points, count))
	DECLARE_GDIPLUS_METHOD(WINGDIPAPI, GdipDrawPath,
						   (GpGraphics * graphics, GpPen* pen, GpPath* path), (graphics, pen, path))
	DECLARE_GDIPLUS_METHOD(WINGDIPAPI, GdipDrawLineI,
						   (GpGraphics * graphics, GpPen* pen, INT x1, INT y1, INT x2, INT y2),
						   (graphics, pen, x1, y1, x2, y2))

	DECLARE_GDIPLUS_METHOD(WINGDIPAPI, GdipFillEllipse,
						   (GpGraphics * graphics, GpBrush* brush, REAL x, REAL y, REAL width,
							REAL height),
						   (graphics, brush, x, y, width, height))
	DECLARE_GDIPLUS_METHOD(WINGDIPAPI, GdipFillPolygon,
						   (GpGraphics * graphics, GpBrush* brush, GDIPCONST GpPointF* points,
							INT count, GpFillMode fillMode),
						   (graphics, brush, points, count, fillMode))
	DECLARE_GDIPLUS_METHOD(WINGDIPAPI, GdipFillPolygonI,
						   (GpGraphics * graphics, GpBrush* brush, GDIPCONST GpPoint* points,
							INT count, GpFillMode fillMode),
						   (graphics, brush, points, count, fillMode))
	DECLARE_GDIPLUS_METHOD(WINGDIPAPI, GdipFillRectangleI,
						   (GpGraphics * graphics, GpBrush* brush, INT x, INT y, INT width,
							INT height),
						   (graphics, brush, x, y, width, height))
	DECLARE_GDIPLUS_METHOD(WINGDIPAPI, GdipFillPath,
						   (GpGraphics * graphics, GpBrush* brush, GpPath* path),
						   (graphics, brush, path))
	DECLARE_GDIPLUS_METHOD(WINGDIPAPI, GdipGetClipBoundsI, (GpGraphics * graphics, GpRect* rect),
						   (graphics, rect))

	DECLARE_GDIPLUS_METHOD(WINGDIPAPI, GdipCreateFontFromLogfontA,
						   (HDC hdc, const LOGFONTA* logfont, GpFont** font), (hdc, logfont, font))
	DECLARE_GDIPLUS_METHOD(WINGDIPAPI, GdipCreateFontFromLogfontW,
						   (HDC hdc, const LOGFONTW* logfont, GpFont** font), (hdc, logfont, font))
	DECLARE_GDIPLUS_METHOD(WINGDIPAPI, GdipGetFontStyle, (GpFont * font, INT* style), (font, style))
	DECLARE_GDIPLUS_METHOD(WINGDIPAPI, GdipGetFontSize, (GpFont * font, REAL* size), (font, size))
	DECLARE_GDIPLUS_METHOD(WINGDIPAPI, GdipDeleteFont, (GpFont * font), (font))

	DECLARE_GDIPLUS_METHOD(WINGDIPAPI, GdipGetFamily, (GpFont * font, GpFontFamily** family),
						   (font, family))
	DECLARE_GDIPLUS_METHOD(WINGDIPAPI, GdipGetEmHeight,
						   (GDIPCONST GpFontFamily * family, INT style, UINT16* EmHeight),
						   (family, style, EmHeight))
	DECLARE_GDIPLUS_METHOD(WINGDIPAPI, GdipGetCellAscent,
						   (GDIPCONST GpFontFamily * family, INT style, UINT16* CellAscent),
						   (family, style, CellAscent))
	DECLARE_GDIPLUS_METHOD(WINGDIPAPI, GdipGetCellDescent,
						   (GDIPCONST GpFontFamily * family, INT style, UINT16* CellDescent),
						   (family, style, CellDescent))
	DECLARE_GDIPLUS_METHOD(WINGDIPAPI, GdipDeleteFontFamily, (GpFontFamily * family), (family))

	DECLARE_GDIPLUS_METHOD(WINGDIPAPI, GdipMeasureString,
						   (GpGraphics * graphics, GDIPCONST WCHAR* string, INT length,
							GDIPCONST GpFont* font, GDIPCONST RectF* layoutRect,
							GDIPCONST GpStringFormat* stringFormat, RectF* boundingBox,
							INT* codepointsFitted, INT* linesFilled),
						   (graphics, string, length, font, layoutRect, stringFormat, boundingBox,
							codepointsFitted, linesFilled))

	DECLARE_GDIPLUS_METHOD(WINGDIPAPI, GdipTranslateWorldTransform,
						   (GpGraphics * graphics, REAL dx, REAL dy, GpMatrixOrder order),
						   (graphics, dx, dy, order))
	DECLARE_GDIPLUS_METHOD(WINGDIPAPI, GdipGetWorldTransform,
						   (GpGraphics * graphics, GpMatrix* matrix), (graphics, matrix))
	DECLARE_GDIPLUS_METHOD(WINGDIPAPI, GdipSetWorldTransform,
						   (GpGraphics * graphics, GpMatrix* matrix), (graphics, matrix))

	DECLARE_GDIPLUS_METHOD(WINGDIPAPI, GdipDeleteStringFormat, (GpStringFormat * format), (format))
	DECLARE_GDIPLUS_METHOD(WINGDIPAPI, GdipCreateStringFormat,
						   (INT formatAttributes, LANGID language, GpStringFormat** format),
						   (formatAttributes, language, format))

	DECLARE_GDIPLUS_METHOD(WINGDIPAPI, GdipGetDC, (GpGraphics * graphics, HDC* hdc),
						   (graphics, hdc))
	DECLARE_GDIPLUS_METHOD(WINGDIPAPI, GdipReleaseDC, (GpGraphics * graphics, HDC hdc),
						   (graphics, hdc))

	DECLARE_GDIPLUS_METHOD(WINGDIPAPI, GdipDrawString,
						   (GpGraphics * graphics, GDIPCONST WCHAR* string, INT length,
							GDIPCONST GpFont* font, GDIPCONST RectF* layoutRect,
							GDIPCONST GpStringFormat* stringFormat, GDIPCONST GpBrush* brush),
						   (graphics, string, length, font, layoutRect, stringFormat, brush))
	DECLARE_GDIPLUS_METHOD(WINGDIPAPI, GdipSetTextRenderingHint,
						   (GpGraphics * graphics, TextRenderingHint mode), (graphics, mode))

	DECLARE_GDIPLUS_METHOD(WINGDIPAPI, GdipStringFormatGetGenericTypographic,
						   (GpStringFormat * *format), (format))
	DECLARE_GDIPLUS_METHOD(WINGDIPAPI, GdipCloneStringFormat,
						   (GDIPCONST GpStringFormat * format, GpStringFormat** newFormat),
						   (format, newFormat))
	DECLARE_GDIPLUS_METHOD(WINGDIPAPI, GdipSetStringFormatFlags,
						   (GpStringFormat * format, INT flags), (format, flags))
	DECLARE_GDIPLUS_METHOD(WINGDIPAPI, GdipGetStringFormatFlags,
						   (GDIPCONST GpStringFormat * format, INT* flags), (format, flags))

	DECLARE_GDIPLUS_METHOD(WINGDIPAPI, GdipSetPenDashArray,
						   (GpPen * pen, GDIPCONST REAL* dash, INT count), (pen, dash, count))
	DECLARE_GDIPLUS_METHOD(WINGDIPAPI, GdipSetPenLineJoin, (GpPen * pen, GpLineJoin lineJoin),
						   (pen, lineJoin))
	DECLARE_GDIPLUS_METHOD(WINGDIPAPI, GdipSetPenStartCap, (GpPen * pen, GpLineCap startCap),
						   (pen, startCap))
	DECLARE_GDIPLUS_METHOD(WINGDIPAPI, GdipSetPenEndCap, (GpPen * pen, GpLineCap endCap),
						   (pen, endCap))

	DECLARE_GDIPLUS_METHOD(WINGDIPAPI, GdipCreateRegion, (GpRegion * *region), (region))
	DECLARE_GDIPLUS_METHOD(WINGDIPAPI, GdipGetClip, (GpGraphics * graphics, GpRegion* region),
						   (graphics, region))
	DECLARE_GDIPLUS_METHOD(WINGDIPAPI, GdipSetClipRegion,
						   (GpGraphics * graphics, GpRegion* region, CombineMode combineMode),
						   (graphics, region, combineMode))
	DECLARE_GDIPLUS_METHOD(WINGDIPAPI, GdipDeleteRegion, (GpRegion * region), (region))
	DECLARE_GDIPLUS_METHOD(WINGDIPAPI, GdipSetClipRectI,
						   (GpGraphics * graphics, INT x, INT y, INT width, INT height,
							CombineMode combineMode),
						   (graphics, x, y, width, height, combineMode))

	DECLARE_GDIPLUS_METHOD(WINGDIPAPI, GdipCreateImageAttributes, (GpImageAttributes * *imageattr),
						   (imageattr))
	DECLARE_GDIPLUS_METHOD(WINGDIPAPI, GdipDisposeImageAttributes, (GpImageAttributes * imageattr),
						   (imageattr))
	DECLARE_GDIPLUS_METHOD(WINGDIPAPI, GdipSetImageAttributesColorKeys,
						   (GpImageAttributes * imageattr, ColorAdjustType type, BOOL enableFlag,
							ARGB colorLow, ARGB colorHigh),
						   (imageattr, type, enableFlag, colorLow, colorHigh))

	DECLARE_GDIPLUS_METHOD(WINGDIPAPI, GdipDrawImage,
						   (GpGraphics * graphics, GpImage* image, REAL x, REAL y),
						   (graphics, image, x, y))
	DECLARE_GDIPLUS_METHOD(WINGDIPAPI, GdipDrawImageRect,
						   (GpGraphics * graphics, GpImage* image, REAL x, REAL y, REAL width,
							REAL height),
						   (graphics, image, x, y, width, height))
	DECLARE_GDIPLUS_METHOD(WINGDIPAPI, GdipDrawImageRectRect,
						   (GpGraphics * graphics, GpImage* image, REAL dstx, REAL dsty,
							REAL dstwidth, REAL dstheight, REAL srcx, REAL srcy, REAL srcwidth,
							REAL srcheight, GpUnit srcUnit,
							GDIPCONST GpImageAttributes* imageAttributes, DrawImageAbort callback,
							VOID* callbackData),
						   (graphics, image, dstx, dsty, dstwidth, dstheight, srcx, srcy, srcwidth,
							srcheight, srcUnit, imageAttributes, callback, callbackData))
	DECLARE_GDIPLUS_METHOD(WINGDIPAPI, GdipCreateBitmapFromHBITMAP,
						   (HBITMAP hbm, HPALETTE hpal, GpBitmap** bitmap), (hbm, hpal, bitmap))
	DECLARE_GDIPLUS_METHOD(WINGDIPAPI, GdipCreateBitmapFromScan0,
						   (INT width, INT height, INT stride, PixelFormat format, BYTE* scan0,
							GpBitmap** bitmap),
						   (width, height, stride, format, scan0, bitmap))
	DECLARE_GDIPLUS_METHOD(WINGDIPAPI, GdipLoadImageFromStream, (IStream * stream, GpImage** image),
						   (stream, image))
	DECLARE_GDIPLUS_METHOD(WINGDIPAPI, GdipGetImageWidth, (GpImage * image, UINT* width),
						   (image, width))
	DECLARE_GDIPLUS_METHOD(WINGDIPAPI, GdipGetImageHeight, (GpImage * image, UINT* height),
						   (image, height))
	DECLARE_GDIPLUS_METHOD(WINGDIPAPI, GdipImageRotateFlip,
						   (GpImage * image, RotateFlipType rfType), (image, rfType))
	DECLARE_GDIPLUS_METHOD(WINGDIPAPI, GdipGetImageGraphicsContext,
						   (GpImage * image, GpGraphics** graphics), (image, graphics))
	DECLARE_GDIPLUS_METHOD(WINGDIPAPI, GdipImageGetFrameDimensionsCount,
						   (GpImage * image, UINT* count), (image, count))
	DECLARE_GDIPLUS_METHOD(WINGDIPAPI, GdipImageGetFrameDimensionsList,
						   (GpImage * image, GUID* dimensionIDs, UINT count),
						   (image, dimensionIDs, count))
	DECLARE_GDIPLUS_METHOD(WINGDIPAPI, GdipImageGetFrameCount,
						   (GpImage * image, GDIPCONST GUID* dimensionID, UINT* count),
						   (image, dimensionID, count))
	DECLARE_GDIPLUS_METHOD(WINGDIPAPI, GdipGetPropertyItemSize,
						   (GpImage * image, PROPID propId, UINT* size), (image, propId, size))
	DECLARE_GDIPLUS_METHOD(WINGDIPAPI, GdipGetPropertyItem,
						   (GpImage * image, PROPID propId, UINT propSize, PropertyItem* buffer),
						   (image, propId, propSize, buffer))
	DECLARE_GDIPLUS_METHOD(WINGDIPAPI, GdipImageSelectActiveFrame,
						   (GpImage * image, GDIPCONST GUID* dimensionID, UINT frameIndex),
						   (image, dimensionID, frameIndex))
	DECLARE_GDIPLUS_METHOD(WINGDIPAPI, GdipDisposeImage, (GpImage * image), (image))
	DECLARE_GDIPLUS_METHOD(WINGDIPAPI, GdipBitmapLockBits,
						   (GpBitmap * bitmap, GDIPCONST GpRect* rect, UINT flags,
							PixelFormat format, BitmapData* lockedBitmapData),
						   (bitmap, rect, flags, format, lockedBitmapData))
	DECLARE_GDIPLUS_METHOD(WINGDIPAPI, GdipBitmapUnlockBits,
						   (GpBitmap * bitmap, BitmapData* lockedBitmapData),
						   (bitmap, lockedBitmapData))
	DECLARE_GDIPLUS_METHOD(WINGDIPAPI, GdipGetImageFlags, (GpImage * image, UINT* flags),
						   (image, flags));

	DECLARE_GDIPLUS_METHOD(WINGDIPAPI, GdipCreateMatrix, (GpMatrix * *matrix), (matrix))
	DECLARE_GDIPLUS_METHOD(WINGDIPAPI, GdipCloneMatrix, (GpMatrix * matrix, GpMatrix** cloneMatrix),
						   (matrix, cloneMatrix))
	DECLARE_GDIPLUS_METHOD(WINGDIPAPI, GdipDeleteMatrix, (GpMatrix * matrix), (matrix))

	DECLARE_GDIPLUS_METHOD(WINGDIPAPI, GdipInvertMatrix, (GpMatrix * matrix), (matrix))
	DECLARE_GDIPLUS_METHOD(WINGDIPAPI, GdipTransformMatrixPointsI,
						   (GpMatrix * matrix, GpPoint* pts, INT count), (matrix, pts, count))
	DECLARE_GDIPLUS_METHOD(WINGDIPAPI, GdipTransformMatrixPoints,
						   (GpMatrix * matrix, GpPointF* pts, INT count), (matrix, pts, count))
	DECLARE_GDIPLUS_METHOD(WINGDIPAPI, GdipTranslateMatrix,
						   (GpMatrix * matrix, REAL offsetX, REAL offsetY, GpMatrixOrder order),
						   (matrix, offsetX, offsetY, order))
	DECLARE_GDIPLUS_METHOD(WINGDIPAPI, GdipRotateMatrix,
						   (GpMatrix * matrix, REAL angle, GpMatrixOrder order),
						   (matrix, angle, order))
	DECLARE_GDIPLUS_METHOD(WINGDIPAPI, GdipScaleMatrix,
						   (GpMatrix * matrix, REAL scaleX, REAL scaleY, GpMatrixOrder order),
						   (matrix, scaleX, scaleY, order))
	DECLARE_GDIPLUS_METHOD(WINGDIPAPI, GdipShearMatrix,
						   (GpMatrix * matrix, REAL shearX, REAL shearY, GpMatrixOrder order),
						   (matrix, shearX, shearY, order))
	DECLARE_GDIPLUS_METHOD(WINGDIPAPI, GdipSetImageAttributesColorMatrix,
						   (GpImageAttributes * imageattr, ColorAdjustType type, BOOL enableFlag,
							GDIPCONST ColorMatrix* colorMatrix, GDIPCONST ColorMatrix* grayMatrix,
							ColorMatrixFlags flags),
						   (imageattr, type, enableFlag, colorMatrix, grayMatrix, flags));

public:
	_XTP_DEPRECATED HMODULE m_hModule;
	_XTP_DEPRECATED ULONG_PTR m_nGdiplusToken;
	_XTP_DEPRECATED int m_nCount;
};

AFX_INLINE void CXTPGdiPlus::Register(BOOL bReserved)
{
	UNREFERENCED_PARAMETER(bReserved);
}

#	include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPGDIPLUS_H__)

//}}AFX_CODEJOCK_PRIVATE
