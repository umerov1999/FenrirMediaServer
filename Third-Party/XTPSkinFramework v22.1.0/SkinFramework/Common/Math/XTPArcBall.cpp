// XTPArcBall.cpp
//
// (c)1998-2023 Codejock Software, All Rights Reserved.
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

#include "Common/Math/XTPMathUtils.h"
#include "Common/Base/Types/XTPPoint2.h"
#include "Common/Base/Types/XTPPoint3.h"
#include "Common/Math/XTPArcBall.h"
#include "Common/Math/XTPMathUtils.h"

#include "Common/Base/Diagnostic/XTPDisableAdvancedWarnings.h"
#include <math.h>
#include "Common/Base/Diagnostic/XTPEnableAdvancedWarnings.h"

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

#ifdef _DEBUG
#	define new DEBUG_NEW
#	undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CXTPArcBall

CXTPArcBall::CXTPArcBall()
	: m_dWidthAdjustmentFactor(0.)
	, m_dHeightAdjustmentFactor(0.)
{
}

void CXTPArcBall::SetBounds(double dWidth, double dHeight)
{
	ASSERT(0 < dWidth);
	ASSERT(0 < dHeight);

	if (dWidth < 1.)
		dWidth = 1.;
	if (dHeight < 1.)
		dHeight = 1.;

	m_dWidthAdjustmentFactor  = 1. / ((dWidth - 1.) * .5);
	m_dHeightAdjustmentFactor = 1. / ((dHeight - 1.) * .5);
}

BOOL CXTPArcBall::HasBounds() const
{
	return 0. < m_dWidthAdjustmentFactor && 0. < m_dHeightAdjustmentFactor;
}

void CXTPArcBall::SetBegin(double x, double y)
{
	MapToSphere(x, y, m_vBegin);
}

void CXTPArcBall::SetEnd(double x, double y)
{
	MapToSphere(x, y, m_vEnd);
}

void CXTPArcBall::GetRotationQuaternion(double& xQuat, double& yQuat, double& zQuat, double& wQuat)
{
	// Compute the perpendicular vector to the begin and end vectors
	CXTPPoint3d vPerp;
	CXTPMathUtils::ComputeVectorCrossProduct(m_vBegin.X, m_vBegin.Y, m_vBegin.Z, m_vEnd.X, m_vEnd.Y,
											 m_vEnd.Z, vPerp.X, vPerp.Y, vPerp.Z);

	// Compute the length of the perpendicular vector
	double dLength = CXTPMathUtils::ComputeVectorLength(vPerp.X, vPerp.Y, vPerp.Z);
	if (CXTPMathUtils::m_dEPS < dLength)
	{
		// Build quaternion from perpendicular vector
		xQuat = vPerp.X;
		yQuat = vPerp.Y;
		zQuat = vPerp.Z;
		wQuat = CXTPMathUtils::ComputeVectorDotProduct(m_vBegin.X, m_vBegin.Y, m_vBegin.Z, m_vEnd.X,
													   m_vEnd.Y, m_vEnd.Z);
	}
	else
	{
		// Use identity quaternion for zero-length perpendicular vector
		xQuat = yQuat = zQuat = wQuat = 0.;
	}
}

void CXTPArcBall::MapToSphere(double x, double y, CXTPPoint3d& vSphere)
{
	ASSERT(HasBounds() && "Arcball bounds have not been set");

	// Scale coordinates tp to range of [-1 ... 1]
	x = (x * m_dWidthAdjustmentFactor) - 1.;
	y = 1. - (y * m_dHeightAdjustmentFactor);

	// Compute the square length from begin to end
	double dSquareLength = (x * x) + (y * y);
	if (1. < dSquareLength)
	{
		// The point is outside the sphere. Compute a mapped point on the sphere.
		double dNorm = 1. / sqrt(dSquareLength);
		vSphere.X	= x * dNorm;
		vSphere.Y	= y * dNorm;
		vSphere.Z	= 0.;
	}
	else
	{
		// The point is inside the sphere. . Compute a mapped point on the sphere.
		vSphere.X = x;
		vSphere.Y = y;
		vSphere.Z = sqrt(1. - dSquareLength);
	}
}
