// XTPMathUtils.h
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
#if !defined(__XTPMATHUTILS_H__)
#	define __XTPMATHUTILS_H__
//}}AFX_CODEJOCK_PRIVATE

#	if _MSC_VER >= 1000
#		pragma once
#	endif // _MSC_VER >= 1000

#	include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

//===========================================================================
// Summary:
//     This class provide some mathematical utility functions.
// Remarks:
//===========================================================================
class _XTP_EXT_CLASS CXTPMathUtils
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Call this function to normalize angle in radian i.e. bring the value
	//     within the range of 0 - 2PI.
	// Parameters:
	//     angleRadian - Angle in radians.
	// Returns:
	//     A double value denoting the normalized radian.
	// Remarks:
	//     It is a static function.
	//-----------------------------------------------------------------------
	static double AFX_CDECL NormalizeRadian(double angleRadian);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this function to normalize angle in degree i.e. bring the value
	//     within the range of 0 and 360.
	// Parameters:
	//     angle - Angle in degree.
	// Returns:
	//     A double value denoting the normalized degree.
	// Remarks:
	//     It is a static function.
	//-----------------------------------------------------------------------
	static double AFX_CDECL NormalizeDegree(double angle);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this function to check the sign of a double value.
	// Parameters:
	//     value - The double value.
	// Returns:
	//     +1 if the value is positive and -1 if the value negative.
	// Remarks:
	//     It is a static function.
	//-----------------------------------------------------------------------
	static int AFX_CDECL Sign(double value);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this function to compare two double values.It also checks
	//     whether they are equal or almost equal.
	// Parameters:
	//     value1  - The first value.
	//     value2  - The second value.
	// Returns:
	//     +1 if the value1 is greater than value2, -1 if vice versa.
	//     0 if they are equal or almost equal.
	// Remarks:
	//     It is a static function.
	//-----------------------------------------------------------------------
	static int AFX_CDECL Compare(double value1, double value2);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this function to compare two double values.It also checks
	//     whether they are equal or almost equal based on a 3rd value.
	// Parameters:
	//     value1  - The first value.
	//     value2  - The second value.
	//     epsilon - The equality threshold.
	// Returns:
	//     +1 if the value1 is greater than value2, -1 if vice versa.
	//     0 if they are equal or almost equal.
	// Remarks:
	//     It is a static function.
	//-----------------------------------------------------------------------
	static int AFX_CDECL Compare(double value1, double value2, double epsilon);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this function to convert radian angle to degree.
	// Parameters:
	//     angleRadian  - The angle in radian.
	// Returns:
	//     A double value denoting the angle in degree.
	// Remarks:
	//     It is a static function.
	//-----------------------------------------------------------------------
	static double AFX_CDECL Radian2Degree(double angleRadian);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this function to convert degree angle to radian.
	// Parameters:
	//     angleDegree  - The angle in degree.
	// Returns:
	//     A double value denoting the angle in radian.
	// Remarks:
	//     It is a static function.
	//-----------------------------------------------------------------------
	static double AFX_CDECL Degree2Radian(double angleDegree);

	//-----------------------------------------------------------------------
	// Summary:
	//      Rounds a real value taking into account its sign and returns
	//      result as integer.
	// Parameters:
	//      dValue - A value to be rounded.
	// Returns:
	//      A rounded value casted to integer type.
	//-----------------------------------------------------------------------
	static int AFX_CDECL Round(double dValue);

	//-----------------------------------------------------------------------
	// Summary:
	//      Obtains Not-a-Number (NaN) value for double type.
	// Returns:
	//      Double NaN value.
	// See also:
	//      IsNan
	//-----------------------------------------------------------------------
	static double AFX_CDECL GetNaN();

	//-----------------------------------------------------------------------
	// Summary:
	//      Checks if the value provided is Not-a-Number (NaN) value.
	// Parameters:
	//      x - A value to check.
	// Returns:
	//      TRUE if the value provided is NaN value.
	// See also:
	//      GetNaN
	//-----------------------------------------------------------------------
	static BOOL AFX_CDECL IsNan(double x);

	//-----------------------------------------------------------------------
	// Summary:
	//      Computes a normal for a triangle specified.
	// Parameters:
	//      x1 - 1st triangle X coordinate.
	//      y1 - 1st triangle Y coordinate.
	//      z1 - 1st triangle Z coordinate.
	//      x2 - 2nd triangle X coordinate.
	//      y2 - 2nd triangle Y coordinate.
	//      z2 - 2nd triangle Z coordinate.
	//      x3 - 3rd triangle X coordinate.
	//      y3 - 3rd triangle Y coordinate.
	//      z3 - 3rd triangle Z coordinate.
	//      nx - Result normal X value.
	//      ny - Result normal Y value.
	//      nz - Result normal Z value.
	//      normalize - If true the result normal vector is clamped to 0..1 range.
	//      ccw - Determines which side of the triangle the normal vector will
	//            be directed from. By default triangle points are assumed to
	//            be in close-wise order which determines the front face of
	//            the triangle for which the normal is computed. In order
	//            to reverse normal vector this value has to be false which means
	//            counter clock wise vertex unwinding.
	//-----------------------------------------------------------------------
	static void AFX_CDECL ComputeTriangleNormal(double x1, double y1, double z1, double x2,
												double y2, double z2, double x3, double y3,
												double z4, double& nx, double& ny, double& nz,
												bool normalize = true, bool ccw = false);

	//-----------------------------------------------------------------------
	// Summary:
	//      Computes 2D line segment length.
	// Parameters:
	//      x1 - 1st point X coordinate.
	//      y1 - 1st point Y coordinate.
	//      x2 - 2nd point X coordinate.
	//      y2 - 2nd point Y coordinate.
	// Returns:
	//      The distance between points provided.
	//-----------------------------------------------------------------------
	static double AFX_CDECL GetSegmentLength(double x1, double y1, double x2, double y2);

	//-----------------------------------------------------------------------
	// Summary:
	//      Computes 3D line segment length.
	// Parameters:
	//      x1 - 1st point X coordinate.
	//      y1 - 1st point Y coordinate.
	//      z1 - 1st point Z coordinate.
	//      x2 - 2nd point X coordinate.
	//      y2 - 2nd point Y coordinate.
	//      z2 - 2nd point Z coordinate.
	// Returns:
	//      The distance between points provided.
	//-----------------------------------------------------------------------
	static double AFX_CDECL GetSegmentLength(double x1, double y1, double z1, double x2, double y2,
											 double z2);

	//-----------------------------------------------------------------------
	// Summary:
	//      Tests if the point specified hits a line segment determined
	//      by two points.
	// Parameters:
	//      x1 - 1st point X coordinate.
	//      y1 - 1st point Y coordinate.
	//      x2 - 2nd point X coordinate.
	//      y2 - 2nd point Y coordinate.
	//      xPoint - X coordinate of the point being tested.
	//      yPoint - Y coordinate of the point being tested.
	//      nLineThickness - Line thickness.
	// Returns:
	//      TRUE if the point provided hits the line segment.
	//-----------------------------------------------------------------------
	static BOOL AFX_CDECL HitTestLineSegment(double x1, double y1, double x2, double y2,
											 double xPoint, double yPoint, int nLineThickness);

	//-----------------------------------------------------------------------
	// Summary:
	//      Computes a point where two lines detemined by four points intersect.
	// Parameters:
	//      x11 - Line's one 1st point X coordinate.
	//      y11 - Line's one 1st point Y coordinate.
	//      x12 - Line's one 2nd point X coordinate.
	//      y12 - Line's one 2nd point Y coordinate.
	//      x21 - Line's two 1st point X coordinate.
	//      y21 - Line's two 1st point Y coordinate.
	//      x22 - Line's two 2nd point X coordinate.
	//      y22 - Line's two 2nd point Y coordinate.
	//      xi - On successful completion takes a value of X coordinate of
	//           line intersection position.
	//      yi - On successful completion takes a value of Y coordinate of
	//           line intersection position.
	// Returns:
	//      TRUE if lines intersect and intersection point is found.
	//-----------------------------------------------------------------------
	static BOOL AFX_CDECL GetLinesIntersection(double x11, double y11, double x12, double y12,
											   double x21, double y21, double x22, double y22,
											   double& xi, double& yi);

	//-----------------------------------------------------------------------
	// Summary:
	//      Computes  vector cross product.
	// Parameters:
	//      x1 - 1st vector X coordinate.
	//      y1 - 1st vector Y coordinate.
	//      z1 - 1st vector Z coordinate.
	//      x2 - 2nd vector X coordinate.
	//      y2 - 2nd vector Y coordinate.
	//      z2 - 2nd vector Z coordinate.
	//      rx - Result vector X coordinate.
	//      ry - Result vector Y coordinate.
	//      rz - Result vector Z coordinate.
	// See also:
	//      ComputeVectorDotProduct, ComputeVectorLength
	//-----------------------------------------------------------------------
	static void AFX_CDECL ComputeVectorCrossProduct(double x1, double y1, double z1, double x2,
													double y2, double z2, double& rx, double& ry,
													double& rz);

	//-----------------------------------------------------------------------
	// Summary:
	//      Computes  vector cross product.
	// Parameters:
	//      x1 - 1st vector X coordinate.
	//      y1 - 1st vector Y coordinate.
	//      z1 - 1st vector Z coordinate.
	//      x2 - 2nd vector X coordinate.
	//      y2 - 2nd vector Y coordinate.
	//      z2 - 2nd vector Z coordinate.
	// Returns:
	//      Vector dot product value.
	// See also:
	//      ComputeVectorCrossProduct, ComputeVectorLength
	//-----------------------------------------------------------------------
	static double AFX_CDECL ComputeVectorDotProduct(double x1, double y1, double z1, double x2,
													double y2, double z2);

	//-----------------------------------------------------------------------
	// Summary:
	//      Computes vector length.
	// Parameters:
	//      x - Vector X coordinate.
	//      y - Vector Y coordinate.
	//      z - Vector Z coordinate.
	// Returns:
	//      Vector length value.
	// See also:
	//      ComputeVectorCrossProduct, ComputeVectorDotProduct
	//-----------------------------------------------------------------------
	static double AFX_CDECL ComputeVectorLength(double x, double y, double z);

public:
	static const double m_dPI;	// Pi value
	static const double m_dEPS; // The smallest possible double value, or epsilon.
};

#	include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif //#if !defined(__XTPMATHUTILS_H__)
