// XTPMatrix.h
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
#if !defined(__XTPMATRIX_H__)
#	define __XTPMATRIX_H__
//}}AFX_CODEJOCK_PRIVATE

#	if _MSC_VER >= 1000
#		pragma once
#	endif // _MSC_VER >= 1000

#	include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

//-----------------------------------------------------------------------
// Summary:
//      Represents a matrix object built upon a continuos array of elements
//      and provides basic algorithms for it.
//-----------------------------------------------------------------------
template<class T>
class CXTPMatrix
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//      Constructs a matrix object of the specified size where each element
	//      is set to zero. If either side is equal to 0 then the matrix is
	//      considered to be a valid empty matrix.
	// Parameters:
	//      nWidth - The width of the matrix.
	//      nHeight - The height of the matrix.
	//-----------------------------------------------------------------------
	CXTPMatrix(UINT nWidth, UINT nHeight);

	//-----------------------------------------------------------------------
	// Summary:
	//      Handles matrix deallocation.
	//-----------------------------------------------------------------------
	~CXTPMatrix();

public:
	//-----------------------------------------------------------------------
	// Summary:
	//      Obtains the width of the matrix.
	// Returns:
	//      The width of the matrix.
	// See also:
	//      GetHeight
	//-----------------------------------------------------------------------
	UINT GetWidth() const;

	//-----------------------------------------------------------------------
	// Summary:
	//      Obtains the height of the matrix.
	// Returns:
	//      The height of the matrix.
	// See also:
	//      GetWidth
	//-----------------------------------------------------------------------
	UINT GetHeight() const;

	//-----------------------------------------------------------------------
	// Summary:
	//      Obtains a point to the first element of the continuous array
	//      upon which the matrix is built.
	// Returns:
	//      The value of the pointer of the first element in the array
	//      or NULL if matrix is empty.
	//-----------------------------------------------------------------------
	T* GetData();
	const T* GetData() const; // <combine CXTPMatrix::GetData>

	//-----------------------------------------------------------------------
	// Summary:
	//      Clones the matrix.
	// Returns:
	//      A pointer to the new cloned matrix object. The caller is responsible
	//      for releasing resources using delete operator.
	//-----------------------------------------------------------------------
	CXTPMatrix<T>* Clone() const;

	//-----------------------------------------------------------------------
	// Summary:
	//      Compares the current matrix to the matrix oject specified.
	// Parameters:
	//      m - A matrix to compare to.
	// Returns:
	//      TRUE if the dimensions and all emlements of both matrices are identical.
	//-----------------------------------------------------------------------
	BOOL Equals(const CXTPMatrix<T>& m) const;

	//-----------------------------------------------------------------------
	// Summary:
	//      Obtains an element value at the specified location.
	// Parameters:
	//      nCol - Zero-based column index.
	//      nRow - Zero-based row index.
	// Returns:
	//      Element value at the specified location.
	// See also:
	//      SetAt
	//-----------------------------------------------------------------------
	T GetAt(UINT nCol, UINT nRow) const;

	//-----------------------------------------------------------------------
	// Summary:
	//      Sets element value at the specified location.
	// Parameters:
	//      nCol - Zero-based column index.
	//      nRow - Zero-based row index.
	//      value - A new element value.
	// See also:
	//      GetAt
	//-----------------------------------------------------------------------
	void SetAt(UINT nCol, UINT nRow, T value);

	//-----------------------------------------------------------------------
	// Summary:
	//      Makes the current matrix identical to the matrix provided.
	// Parameters:
	//      m - A matrix object to make the current matrix identical to.
	//-----------------------------------------------------------------------
	void Set(const CXTPMatrix<T>& m);

	//-----------------------------------------------------------------------
	// Summary:
	//      Sets all matrix elements to 0.
	// See also:
	//      SetIdentity
	//-----------------------------------------------------------------------
	void SetZero();

	//-----------------------------------------------------------------------
	// Summary:
	//      Make the current matrix an identity matrix, i.e. all elements
	//      are set to 0 and the main diagonal elements are set to 1. Requires
	//      the matrix to be a square matrix.
	// See also:
	//      SetZero
	//-----------------------------------------------------------------------
	void SetIdentity();

	//-----------------------------------------------------------------------
	// Summary:
	//      Adds the matrix provided to the current matrix. Both matrixes
	//      have to be of the same size.
	// Parameters:
	//      m - A matrix to add to the current matrix.
	// See also:
	//      Subtract, Multiply, Rotate, Transpose
	//-----------------------------------------------------------------------
	void Add(const CXTPMatrix<T>& m);

	//-----------------------------------------------------------------------
	// Summary:
	//      Subtracts the matrix provided from the current matrix. Both matrixes
	//      have to be of the same size.
	// Parameters:
	//      m - A matrix to subtract from the current matrix.
	// See also:
	//      Add, Multiply, Rotate, Transpose
	//-----------------------------------------------------------------------
	void Subtract(const CXTPMatrix<T>& m);

	//-----------------------------------------------------------------------
	// Summary:
	//      Rotates 4x4 matrix using quaternion parameters.
	// Parameters:
	//      xQuat - X coordinate of the rotation vector.
	//      yQuat - Y coordinate of the rotation vector.
	//      zQuat - Z coordinate of the rotation vector.
	//      wQuat - Rotation angle.
	// See also:
	//      Add, Subtract, Multiply, Transpose
	//-----------------------------------------------------------------------
	void Rotate(T xQuat, T yQuat, T zQuat, T wQuat);

	//-----------------------------------------------------------------------
	// Summary:
	//      Multiplies the current matrix of size MxN to the provided matrix of size LxM.
	//      The current matrix is on the left hand side of the multiplication expression.
	// Parameters:
	//      m - A matrix to be multiplied with. A caller is responsible for releasing
	//          allocation resources using delete operator.
	// Returns:
	//      A new matrix of size LxN which contains the result of matrix multiplication.
	// See also:
	//      Add, Subtract, Transpose, Rotate
	//-----------------------------------------------------------------------
	CXTPMatrix<T>* Multiply(const CXTPMatrix<T>& m) const;

	//-----------------------------------------------------------------------
	// Summary:
	//      Creates a transposed matrix, i.e. rows and columns are swapped.
	// Returns:
	//      A new transposed matrix object. A caller is responsible for releasing
	//          allocation resources using delete operator.
	// See also:
	//      Add, Subtract, Multiply, Rotate
	//-----------------------------------------------------------------------
	CXTPMatrix<T>* Transpose() const;

private:
	CXTPMatrix(const CXTPMatrix&);			  // non-copyable
	CXTPMatrix& operator=(const CXTPMatrix&); // non-copyable

	const UINT m_nWidth;
	const UINT m_nHeight;
	T* m_pData;
};

template<class T>
AFX_INLINE CXTPMatrix<T>::CXTPMatrix(UINT nWidth, UINT nHeight)
	: m_nWidth(nWidth)
	, m_nHeight(nHeight)
{
	if (0 < m_nWidth && 0 < m_nHeight)
	{
		m_pData = new T[m_nWidth * m_nHeight];
		SetZero();
	}
	else
	{
		m_pData = NULL;
	}
}

template<class T>
AFX_INLINE CXTPMatrix<T>::~CXTPMatrix()
{
	if (NULL != m_pData)
	{
		delete[] m_pData;
	}
}

template<class T>
AFX_INLINE UINT CXTPMatrix<T>::GetWidth() const
{
	return m_nWidth;
}

template<class T>
AFX_INLINE UINT CXTPMatrix<T>::GetHeight() const
{
	return m_nHeight;
}

template<class T>
AFX_INLINE T* CXTPMatrix<T>::GetData()
{
	return m_pData;
}

template<class T>
AFX_INLINE const T* CXTPMatrix<T>::GetData() const
{
	return m_pData;
}

template<class T>
AFX_INLINE CXTPMatrix<T>* CXTPMatrix<T>::Clone() const
{
	CXTPMatrix<T>* pClone = new CXTPMatrix<T>(m_nWidth, m_nHeight);
	for (UINT i = 0; i < (m_nWidth * m_nHeight); ++i)
	{
		pClone->GetData()[i] = m_pData[i];
	}

	return pClone;
}

template<class T>
AFX_INLINE BOOL CXTPMatrix<T>::Equals(const CXTPMatrix<T>& m) const
{
	BOOL bEquals = FALSE;

	if (GetWidth() == m.GetWidth() && GetHeight() == m.GetHeight())
	{
		bEquals = TRUE;

		for (UINT i = 0; i < (m_nWidth * m_nHeight); ++i)
		{
			if (m_pData[i] != m.GetData()[i])
			{
				bEquals = FALSE;
				break;
			}
		}
	}

	return bEquals;
}

template<class T>
AFX_INLINE T CXTPMatrix<T>::GetAt(UINT nCol, UINT nRow) const
{
	ASSERT(nCol < m_nWidth);
	ASSERT(nRow < m_nHeight);

	UINT nIndex = nRow * m_nWidth + nCol;
	return m_pData[nIndex];
}

template<class T>
AFX_INLINE void CXTPMatrix<T>::SetAt(UINT nCol, UINT nRow, T value)
{
	ASSERT(nCol < m_nWidth);
	ASSERT(nRow < m_nHeight);

	UINT nIndex		= nRow * m_nWidth + nCol;
	m_pData[nIndex] = value;
}

template<class T>
AFX_INLINE void CXTPMatrix<T>::Set(const CXTPMatrix<T>& m)
{
	if (GetWidth() == m.GetWidth() && GetHeight() == m.GetHeight())
	{
		for (UINT i = 0; i < (m_nWidth * m_nHeight); ++i)
		{
			m_pData[i] = m.GetData()[i];
		}
	}
	else
	{
		ASSERT(!"Matrices must have the same size");
	}
}

template<class T>
AFX_INLINE void CXTPMatrix<T>::SetZero()
{
	for (UINT i = 0; i < (m_nWidth * m_nHeight); ++i)
	{
		m_pData[i] = T();
	}
}

template<class T>
AFX_INLINE void CXTPMatrix<T>::SetIdentity()
{
	if (m_nWidth == m_nHeight)
	{
		SetZero();

		for (UINT i = 0; i < m_nWidth; ++i)
		{
			SetAt(i, i, T(1));
		}
	}
	else
	{
		ASSERT(!"Only square matrix with odd sizes can be set to identity");
	}
}

template<class T>
AFX_INLINE void CXTPMatrix<T>::Add(const CXTPMatrix<T>& m)
{
	if (GetWidth() == m.GetWidth() && GetHeight() == m.GetHeight())
	{
		for (UINT i = 0; i < (m_nWidth * m_nHeight); ++i)
		{
			m_pData[i] += m.GetData()[i];
		}
	}
	else
	{
		ASSERT(!"Matrices must have the same size");
	}
}

template<class T>
AFX_INLINE void CXTPMatrix<T>::Subtract(const CXTPMatrix<T>& m)
{
	if (GetWidth() == m.GetWidth() && GetHeight() == m.GetHeight())
	{
		for (UINT i = 0; i < (m_nWidth * m_nHeight); ++i)
		{
			m_pData[i] -= m.GetData()[i];
		}
	}
	else
	{
		ASSERT(!"Matrices must have the same size");
	}
}

template<class T>
AFX_INLINE void CXTPMatrix<T>::Rotate(T xQuat, T yQuat, T zQuat, T wQuat)
{
	if (4 == m_nWidth && 4 == m_nHeight)
	{
		T n = (xQuat * xQuat) + (yQuat * yQuat) + (zQuat * zQuat) + (wQuat * wQuat);
		T s = (T(0) < n) ? (T(2) / n) : T(0);

		T xs = xQuat * s;
		T ys = yQuat * s;
		T zs = zQuat * s;
		T wx = wQuat * xs;
		T wy = wQuat * ys;
		T wz = wQuat * zs;
		T xx = xQuat * xs;
		T xy = xQuat * ys;
		T xz = xQuat * zs;
		T yy = yQuat * ys;
		T yz = yQuat * zs;
		T zz = zQuat * zs;

		SetAt(0, 0, T(1) - (yy + zz));
		SetAt(0, 1, xy - wz);
		SetAt(0, 2, xz + wy);
		SetAt(0, 3, T(0));
		SetAt(1, 0, xy + wz);
		SetAt(1, 1, T(1) - (xx + zz));
		SetAt(1, 2, yz - wx);
		SetAt(1, 3, T(0));
		SetAt(2, 0, xz - wy);
		SetAt(2, 1, yz + wx);
		SetAt(2, 2, T(1) - (xx + yy));
		SetAt(2, 3, T(0));
		SetAt(3, 0, T(0));
		SetAt(3, 1, T(0));
		SetAt(3, 2, T(0));
		SetAt(3, 3, T(1));
	}
	else
	{
		ASSERT(!"Only 4x4 matrix can be rotated using a quaternion");
	}
}

template<class T>
AFX_INLINE CXTPMatrix<T>* CXTPMatrix<T>::Multiply(const CXTPMatrix<T>& m) const
{
	CXTPMatrix<T>* pResult = NULL;

	if (GetWidth() == m.GetHeight())
	{
		pResult = new CXTPMatrix<T>(m.GetWidth(), GetHeight());

		for (UINT nRow = 0; nRow < GetHeight(); ++nRow)
		{
			for (UINT nCol = 0; nCol < m.GetWidth(); ++nCol)
			{
				T& value = pResult->GetData()[nCol * GetHeight() + nRow];
				value	 = T(0);

				for (UINT nRowLeft = 0; nRowLeft < GetWidth(); ++nRowLeft)
				{
					value += GetData()[nRowLeft * GetHeight() + nRow]
							 * m.GetData()[nCol * GetWidth() + nRowLeft];
				}
			}
		}
	}
	else
	{
		ASSERT(!"Matrices cannot be multiplied");
	}

	return pResult;
}

template<class T>
AFX_INLINE CXTPMatrix<T>* CXTPMatrix<T>::Transpose() const
{
	CXTPMatrix<T>* pResult = new CXTPMatrix<T>(GetHeight(), GetWidth());
	for (UINT nRow = 0; nRow < GetHeight(); ++nRow)
	{
		for (UINT nCol = 0; nCol < GetWidth(); ++nCol)
		{
			pResult->SetAt(nRow, nCol, GetAt(nCol, nRow));
		}
	}

	return pResult;
}

#	include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif //#if !defined(__XTPMATRIX_H__)
