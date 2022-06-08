// XTPArcBall.h
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
#if !defined(__XTPARCBALL_H__)
#	define __XTPARCBALL_H__
//}}AFX_CODEJOCK_PRIVATE

#	if _MSC_VER >= 1000
#		pragma once
#	endif // _MSC_VER >= 1000

#	include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

//-----------------------------------------------------------------------
// Summary:
//      Implements natural object rotation computation using quaternions.
//      An object is assumed to have bounds, and rotation begin and end points,
//      for which a rotation quaternion can be computed and used for actual
//      rotation of the view matrix.
// See also:
//      CXTPMatrix
//-----------------------------------------------------------------------
class _XTP_EXT_CLASS CXTPArcBall
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//      Constructs an empy arc-ball object.
	//-----------------------------------------------------------------------
	CXTPArcBall();

	//-----------------------------------------------------------------------
	// Summary:
	//      Sets object bounds.
	// Parameters:
	//      dWidth - The width of object bounds.
	//      dHeight - The height of object bounds.
	// See also:
	//      HasBounds
	//-----------------------------------------------------------------------
	void SetBounds(double dWidth, double dHeight);

	//-----------------------------------------------------------------------
	// Summary:
	//      Determines if object bounds have been set.
	// Returns:
	//      TRUE is object bounds have been set.
	// See also:
	//      SetBounds
	//-----------------------------------------------------------------------
	BOOL HasBounds() const;

	//-----------------------------------------------------------------------
	// Summary:
	//      Sets initial rotation point that is mapped from 2D space onto
	//      a 3D sphere determined by object bounds.
	// Parameters:
	//      x - X coordiante of the rotation starting point.
	//      y - Y coordiante of the rotation starting point.
	// See also:
	//      SetEnd, SetBounds
	//-----------------------------------------------------------------------
	void SetBegin(double x, double y);

	//-----------------------------------------------------------------------
	// Summary:
	//      Sets final rotation point that is mapped from 2D space onto
	//      a 3D sphere determined by object bounds.
	// Parameters:
	//      x - X coordiante of the rotation final point.
	//      y - Y coordiante of the rotation final point.
	// See also:
	//      SetBegin, SetBounds
	//-----------------------------------------------------------------------
	void SetEnd(double x, double y);

	//-----------------------------------------------------------------------
	// Summary:
	//      Obtains rotation quaternion parameters based on the mapped
	//      rotation starting and ending points.
	// Parameters:
	//      xQuat - X component of the rotation quaternion.
	//      yQuat - Y component of the rotation quaternion.
	//      zQuat - Z component of the rotation quaternion.
	//      wQuat - W component of the rotation quaternion.
	//-----------------------------------------------------------------------
	void GetRotationQuaternion(double& xQuat, double& yQuat, double& zQuat, double& wQuat);

private:
	void MapToSphere(double x, double y, CXTPPoint3d& vSphere);

private:
	CXTPPoint3d m_vBegin;
	CXTPPoint3d m_vEnd;
	double m_dWidthAdjustmentFactor;  // Mouse bounds width
	double m_dHeightAdjustmentFactor; // Mouse bounds height
};

#	include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif //#if !defined(__XTPARCBALL_H__)
