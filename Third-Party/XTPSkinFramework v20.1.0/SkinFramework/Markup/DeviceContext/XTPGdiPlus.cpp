// XTPGdiPlus.cpp: implementation of the CXTPGdiPlus class.
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

#include "GraphicLibrary/GdiPlus/XTPGdiPlus.h"

using namespace Gdiplus;
using namespace Gdiplus::DllExports;

#include "Markup/DeviceContext/XTPGdiPlus.h"

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

#ifdef _DEBUG
#	undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#	define new DEBUG_NEW
#endif

CXTPGdiPlus::CXTPGdiPlus()
	: GdiplusStartup(Gdiplus::GdiplusStartup)
	, GdiplusShutdown(Gdiplus::GdiplusShutdown)
	, GdipSetPixelOffsetMode(Gdiplus::DllExports::GdipSetPixelOffsetMode)
	, GdipSetPageUnit(Gdiplus::DllExports::GdipSetPageUnit)
	, GdipSetSmoothingMode(Gdiplus::DllExports::GdipSetSmoothingMode)
	, GdipSetLinePresetBlend(Gdiplus::DllExports::GdipSetLinePresetBlend)
	, GdipSetInterpolationMode(Gdiplus::DllExports::GdipSetInterpolationMode)
	, GdipCreateFromHDC2(Gdiplus::DllExports::GdipCreateFromHDC2)
	, GdipCreateFromHDC(Gdiplus::DllExports::GdipCreateFromHDC)
	, GdipCreatePath2(Gdiplus::DllExports::GdipCreatePath2)
	, GdipCreateSolidFill(Gdiplus::DllExports::GdipCreateSolidFill)
	, GdipCreateLineBrush(Gdiplus::DllExports::GdipCreateLineBrush)
	, GdipCreatePen1(Gdiplus::DllExports::GdipCreatePen1)
	, GdipCreatePen2(Gdiplus::DllExports::GdipCreatePen2)
	, GdipDeleteGraphics(Gdiplus::DllExports::GdipDeleteGraphics)
	, GdipDeleteBrush(Gdiplus::DllExports::GdipDeleteBrush)
	, GdipDeletePen(Gdiplus::DllExports::GdipDeletePen)
	, GdipDeletePath(Gdiplus::DllExports::GdipDeletePath)
	, GdipDrawEllipse(Gdiplus::DllExports::GdipDrawEllipse)
	, GdipDrawLines(Gdiplus::DllExports::GdipDrawLines)
	, GdipDrawLinesI(Gdiplus::DllExports::GdipDrawLinesI)
	, GdipDrawPolygon(Gdiplus::DllExports::GdipDrawPolygon)
	, GdipDrawPolygonI(Gdiplus::DllExports::GdipDrawPolygonI)
	, GdipDrawPath(Gdiplus::DllExports::GdipDrawPath)
	, GdipDrawLineI(Gdiplus::DllExports::GdipDrawLineI)
	, GdipFillEllipse(Gdiplus::DllExports::GdipFillEllipse)
	, GdipFillPolygon(Gdiplus::DllExports::GdipFillPolygon)
	, GdipFillPolygonI(Gdiplus::DllExports::GdipFillPolygonI)
	, GdipFillRectangleI(Gdiplus::DllExports::GdipFillRectangleI)
	, GdipFillPath(Gdiplus::DllExports::GdipFillPath)
	, GdipGetClipBoundsI(Gdiplus::DllExports::GdipGetClipBoundsI)
	, GdipCreateFontFromLogfontA(Gdiplus::DllExports::GdipCreateFontFromLogfontA)
	, GdipCreateFontFromLogfontW(Gdiplus::DllExports::GdipCreateFontFromLogfontW)
	, GdipGetFontStyle(Gdiplus::DllExports::GdipGetFontStyle)
	, GdipGetFontSize(Gdiplus::DllExports::GdipGetFontSize)
	, GdipDeleteFont(Gdiplus::DllExports::GdipDeleteFont)
	, GdipGetFamily(Gdiplus::DllExports::GdipGetFamily)
	, GdipGetEmHeight(Gdiplus::DllExports::GdipGetEmHeight)
	, GdipGetCellAscent(Gdiplus::DllExports::GdipGetCellAscent)
	, GdipGetCellDescent(Gdiplus::DllExports::GdipGetCellDescent)
	, GdipDeleteFontFamily(Gdiplus::DllExports::GdipDeleteFontFamily)
	, GdipMeasureString(Gdiplus::DllExports::GdipMeasureString)
	, GdipTranslateWorldTransform(Gdiplus::DllExports::GdipTranslateWorldTransform)
	, GdipGetWorldTransform(Gdiplus::DllExports::GdipGetWorldTransform)
	, GdipSetWorldTransform(Gdiplus::DllExports::GdipSetWorldTransform)
	, GdipDeleteStringFormat(Gdiplus::DllExports::GdipDeleteStringFormat)
	, GdipCreateStringFormat(Gdiplus::DllExports::GdipCreateStringFormat)
	, GdipGetDC(Gdiplus::DllExports::GdipGetDC)
	, GdipReleaseDC(Gdiplus::DllExports::GdipReleaseDC)
	, GdipDrawString(Gdiplus::DllExports::GdipDrawString)
	, GdipSetTextRenderingHint(Gdiplus::DllExports::GdipSetTextRenderingHint)
	, GdipStringFormatGetGenericTypographic(
		  Gdiplus::DllExports::GdipStringFormatGetGenericTypographic)
	, GdipCloneStringFormat(Gdiplus::DllExports::GdipCloneStringFormat)
	, GdipSetStringFormatFlags(Gdiplus::DllExports::GdipSetStringFormatFlags)
	, GdipGetStringFormatFlags(Gdiplus::DllExports::GdipGetStringFormatFlags)
	, GdipSetPenDashArray(Gdiplus::DllExports::GdipSetPenDashArray)
	, GdipSetPenLineJoin(Gdiplus::DllExports::GdipSetPenLineJoin)
	, GdipSetPenStartCap(Gdiplus::DllExports::GdipSetPenStartCap)
	, GdipSetPenEndCap(Gdiplus::DllExports::GdipSetPenEndCap)
	, GdipCreateRegion(Gdiplus::DllExports::GdipCreateRegion)
	, GdipGetClip(Gdiplus::DllExports::GdipGetClip)
	, GdipSetClipRegion(Gdiplus::DllExports::GdipSetClipRegion)
	, GdipDeleteRegion(Gdiplus::DllExports::GdipDeleteRegion)
	, GdipSetClipRectI(Gdiplus::DllExports::GdipSetClipRectI)
	, GdipCreateImageAttributes(Gdiplus::DllExports::GdipCreateImageAttributes)
	, GdipDisposeImageAttributes(Gdiplus::DllExports::GdipDisposeImageAttributes)
	, GdipSetImageAttributesColorKeys(Gdiplus::DllExports::GdipSetImageAttributesColorKeys)
	, GdipDrawImage(Gdiplus::DllExports::GdipDrawImage)
	, GdipDrawImageRect(Gdiplus::DllExports::GdipDrawImageRect)
	, GdipDrawImageRectRect(Gdiplus::DllExports::GdipDrawImageRectRect)
	, GdipCreateBitmapFromHBITMAP(Gdiplus::DllExports::GdipCreateBitmapFromHBITMAP)
	, GdipCreateBitmapFromScan0(Gdiplus::DllExports::GdipCreateBitmapFromScan0)
	, GdipLoadImageFromStream(Gdiplus::DllExports::GdipLoadImageFromStream)
	, GdipGetImageWidth(Gdiplus::DllExports::GdipGetImageWidth)
	, GdipGetImageHeight(Gdiplus::DllExports::GdipGetImageHeight)
	, GdipImageRotateFlip(Gdiplus::DllExports::GdipImageRotateFlip)
	, GdipGetImageGraphicsContext(Gdiplus::DllExports::GdipGetImageGraphicsContext)
	, GdipImageGetFrameDimensionsCount(Gdiplus::DllExports::GdipImageGetFrameDimensionsCount)
	, GdipImageGetFrameDimensionsList(Gdiplus::DllExports::GdipImageGetFrameDimensionsList)
	, GdipImageGetFrameCount(Gdiplus::DllExports::GdipImageGetFrameCount)
	, GdipGetPropertyItemSize(Gdiplus::DllExports::GdipGetPropertyItemSize)
	, GdipGetPropertyItem(Gdiplus::DllExports::GdipGetPropertyItem)
	, GdipImageSelectActiveFrame(Gdiplus::DllExports::GdipImageSelectActiveFrame)
	, GdipDisposeImage(Gdiplus::DllExports::GdipDisposeImage)
	, GdipBitmapLockBits(Gdiplus::DllExports::GdipBitmapLockBits)
	, GdipBitmapUnlockBits(Gdiplus::DllExports::GdipBitmapUnlockBits)
	, GdipGetImageFlags(Gdiplus::DllExports::GdipGetImageFlags)
	, GdipCreateMatrix(Gdiplus::DllExports::GdipCreateMatrix)
	, GdipCloneMatrix(Gdiplus::DllExports::GdipCloneMatrix)
	, GdipDeleteMatrix(Gdiplus::DllExports::GdipDeleteMatrix)
	, GdipInvertMatrix(Gdiplus::DllExports::GdipInvertMatrix)
	, GdipTransformMatrixPointsI(Gdiplus::DllExports::GdipTransformMatrixPointsI)
	, GdipTransformMatrixPoints(Gdiplus::DllExports::GdipTransformMatrixPoints)
	, GdipTranslateMatrix(Gdiplus::DllExports::GdipTranslateMatrix)
	, GdipRotateMatrix(Gdiplus::DllExports::GdipRotateMatrix)
	, GdipScaleMatrix(Gdiplus::DllExports::GdipScaleMatrix)
	, GdipShearMatrix(Gdiplus::DllExports::GdipShearMatrix)
	, GdipSetImageAttributesColorMatrix(Gdiplus::DllExports::GdipSetImageAttributesColorMatrix)
{
}
