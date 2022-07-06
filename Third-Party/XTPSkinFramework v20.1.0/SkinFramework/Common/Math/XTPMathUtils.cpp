// XTPMathUtils.cpp
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
#include "Common/Base/cxminmax.h"
#include "Common/Base/Diagnostic/XTPDisableAdvancedWarnings.h"
#include <math.h>
#include <float.h>
#include "Common/Base/Diagnostic/XTPEnableAdvancedWarnings.h"

#include "Common/Math/XTPMathUtils.h"

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

#ifdef _DEBUG
#	define new DEBUG_NEW
#	undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const double CXTPMathUtils::m_dPI  = acos(-1.);
const double CXTPMathUtils::m_dEPS = DBL_EPSILON;

double AFX_CDECL CXTPMathUtils::NormalizeRadian(double angleRadian)
{
	double x = fabs(angleRadian) / (m_dPI * 2);
	x		 = m_dPI * 2 * (x - floor(x));
	return x * Sign(angleRadian);
}

double AFX_CDECL CXTPMathUtils::NormalizeDegree(double angle)
{
	double result = angle - 360 * ((int)angle / 360);
	if (result < 0)
		result += 360;
	return result;
}

int AFX_CDECL CXTPMathUtils::Compare(double value1, double value2, double epsilon)
{
	if (fabs(value1 - value2) <= epsilon)
		return 0;

	if (value1 > value2)
		return 1;

	return -1;
}

int AFX_CDECL CXTPMathUtils::Compare(double value1, double value2)
{
	return Compare(value1, value2, m_dEPS);
}

int AFX_CDECL CXTPMathUtils::Sign(double value)
{
	if (fabs(value) < m_dEPS)
		return 0;

	return value > 0 ? +1 : -1;
}

double AFX_CDECL CXTPMathUtils::Radian2Degree(double angleRadian)
{
	return angleRadian * 180 / m_dPI;
}

double AFX_CDECL CXTPMathUtils::Degree2Radian(double angleDegree)
{
	return angleDegree * m_dPI / 180.0;
}

int AFX_CDECL CXTPMathUtils::Round(double dValue)
{
	double dRound = (dValue < 0.0) ? ceil(dValue - 0.5) : floor(dValue + 0.5);
	return (int)dRound;
}

double AFX_CDECL CXTPMathUtils::GetNaN()
{
	unsigned long nan[2] = { 0xffffffff, 0x7fffffff };
	double g			 = *(double*)nan;

	return g;
}

BOOL AFX_CDECL CXTPMathUtils::IsNan(double x)
{
	return _isnan(x);
}

void AFX_CDECL CXTPMathUtils::ComputeTriangleNormal(double x1, double y1, double z1, double x2,
													double y2, double z2, double x3, double y3,
													double z3, double& nx, double& ny, double& nz,
													bool normalize /*= true*/, bool ccw /*= false*/)
{
	nx = y1 * z3 - y2 * z3 - y1 * z2 - z1 * y3 + z2 * y3 + z1 * y2;
	ny = z1 * x3 - z2 * x3 - z1 * x2 - x1 * z3 + x2 * z3 + x1 * z2;
	nz = x1 * y3 - x2 * y3 - x1 * y2 - y1 * x3 + y2 * x3 + y1 * x2;

	if (normalize)
	{
		// clamp to [0,1]
		double l = sqrt(nx * nx + ny * ny + nz * nz);
		if (0 != l)
		{
			nx /= l;
			ny /= l;
			nz /= l;
		}
	}

	if (ccw)
	{
		// Reverse normal for counter-clockwise
		nx *= -1;
		ny *= -1;
		nz *= -1;
	}
}

double AFX_CDECL CXTPMathUtils::GetSegmentLength(double x1, double y1, double x2, double y2)
{
	double dx	= x1 - x2;
	double dy	= y1 - y2;
	double dist = sqrt(dx * dx + dy * dy);
	return dist;
}

double AFX_CDECL CXTPMathUtils::GetSegmentLength(double x1, double y1, double z1, double x2,
												 double y2, double z2)
{
	double dx	= x1 - x2;
	double dy	= y1 - y2;
	double dz	= z1 - z2;
	double dist = sqrt(dx * dx + dy * dy + dz * dz);
	return dist;
}

BOOL AFX_CDECL CXTPMathUtils::HitTestLineSegment(double x1, double y1, double x2, double y2,
												 double xPoint, double yPoint, int nLineThickness)
{
	BOOL bHit = FALSE;

	do
	{
		if (xPoint < CXTP_min(x1, x2) - nLineThickness)
			break;
		if (xPoint > CXTP_max(x1, x2) + nLineThickness)
			break;

		if (yPoint < CXTP_min(y1, y2) - nLineThickness)
			break;
		if (yPoint > CXTP_max(y1, y2) + nLineThickness)
			break;

		double d = sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
		if (d < nLineThickness)
		{
			bHit = TRUE;
			break;
		}

		double fArea = fabs((x2 - x1) * (yPoint - y1) - (y2 - y1) * (xPoint - x1));
		if (fArea / d <= nLineThickness)
		{
			bHit = TRUE;
			break;
		}
	} while (FALSE);

	return bHit;
}

BOOL AFX_CDECL CXTPMathUtils::GetLinesIntersection(double x11, double y11, double x12, double y12,
												   double x21, double y21, double x22, double y22,
												   double& xi, double& yi)
{
	BOOL bIntersect = FALSE;

	// Store the values for fast access and easy
	// equations-to-code conversion
	double x1 = x11, x2 = x12, x3 = x21, x4 = x22;
	double y1 = y11, y2 = y12, y3 = y21, y4 = y22;

	double d = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
	// If d is zero, there is no intersection
	if (0. != d)
	{
		// Get the x and y
		double pre = (x1 * y2 - y1 * x2), post = (x3 * y4 - y3 * x4);
		double x = (pre * (x3 - x4) - (x1 - x2) * post) / d;
		double y = (pre * (y3 - y4) - (y1 - y2) * post) / d;

		// Check if the x and y coordinates are within both lines
		if (!(x < __min(x1, x2) || x > __max(x1, x2) || x < __min(x3, x4) || x > __max(x3, x4)
			  || y < __min(y1, y2) || y > __max(y1, y2) || y < __min(y3, y4) || y > __max(y3, y4)))
		{
			xi		   = x;
			yi		   = y;
			bIntersect = TRUE;
		}
	}

	return bIntersect;
}

void AFX_CDECL CXTPMathUtils::ComputeVectorCrossProduct(double x1, double y1, double z1, double x2,
														double y2, double z2, double& rx,
														double& ry, double& rz)
{
	rx = (y1 * z2) - (z1 * y2);
	ry = (z1 * x2) - (x1 * z2);
	rz = (x1 * y2) - (y1 * x2);
}

double AFX_CDECL CXTPMathUtils::ComputeVectorDotProduct(double x1, double y1, double z1, double x2,
														double y2, double z2)
{
	return (x1 * x2) + (y1 * y2) + (z1 * z2);
}

double AFX_CDECL CXTPMathUtils::ComputeVectorLength(double x, double y, double z)
{
	return sqrt(x * x + y * y + z * z);
}
