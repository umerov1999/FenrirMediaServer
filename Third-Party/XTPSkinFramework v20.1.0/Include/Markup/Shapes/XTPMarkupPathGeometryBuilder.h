// XTPMarkupPathGeometryBuilder.h: interface for the CXTPMarkupPathGeometryBuilder class.
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
#if !defined(__XTPMARKUPGEOMETRYBUILDER_H__)
#	define __XTPMARKUPGEOMETRYBUILDER_H__
//}}AFX_CODEJOCK_PRIVATE

#	if _MSC_VER > 1000
#		pragma once
#	endif // _MSC_VER > 1000

#	include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

enum XTPMarkupPathPointType
{
	xtpMarkupPathPointTypeStart		   = 0,	   // move
	xtpMarkupPathPointTypeLine		   = 1,	   // line
	xtpMarkupPathPointTypeBezier	   = 3,	   // default Bezier (= cubic Bezier)
	xtpMarkupPathPointTypeCloseSubpath = 0x80, // closed flag
};

struct MARKUP_POINTF
{
	float x;
	float y;
};

class CXTPMarkupPathData;
class CXTPMarkupBuilder;

class _XTP_EXT_CLASS CXTPMarkupPathGeometryBuilder
{
public:
	CXTPMarkupPathGeometryBuilder();

public:
	void Parse(CXTPMarkupBuilder* pBuilder, LPCWSTR lpszValue, int nLength);
	CXTPMarkupPathData* CreateData() const;

public:
	void BeginFigure(float x, float y);
	void BezierTo(float x1, float y1, float x2, float y2, float x3,
				  float y3);							   // draw bicubic bezier curve
	void BezierTo(float x1, float y1, float x2, float y2); // draw quadratic bezier curve
	void LineTo(float x, float y);

	void ArcSegment(double xc, double yc, double th0, double th1, double rx, double ry,
					double xAxisRotation);
	void ArcTo(float x, float y, float rotationAngle, double xRadius, double yRadius,
			   BOOL isLargeArcFlag, BOOL sweepDirectionFlag);

	void CloseFigure();

private:
	BOOL ReadToken();
	BOOL SkipWhiteSpace(BOOL allowComma);
	BOOL More() const;

	void ThrowBadToken();

	MARKUP_POINTF ReadSize(BOOL allowcomma);

	MARKUP_POINTF ReadPoint(WCHAR cmd, BOOL allowcomma);
	float ReadNumber(BOOL allowComma);
	BOOL IsNumber(BOOL allowComma);

	void EnsureFigure();
	MARKUP_POINTF Reflect();

private:
	LPCWSTR m_lpszValue;
	int m_nLength;
	int m_nIndex;
	BOOL m_bFigureStarted;
	int m_nFillRule;

	CArray<MARKUP_POINTF, MARKUP_POINTF&> m_arrPoints;
	CArray<BYTE, BYTE> m_arrTypes;
	WCHAR m_cToken;
	CXTPMarkupBuilder* m_pBuilder;

	MARKUP_POINTF m_secondLastPoint;
	MARKUP_POINTF m_lastPoint;
	MARKUP_POINTF m_lastStart;
};

#	include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPMARKUPGEOMETRYBUILDER_H__)
