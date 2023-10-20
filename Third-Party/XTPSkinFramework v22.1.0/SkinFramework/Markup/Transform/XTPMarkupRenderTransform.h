// XTPMarkupRenderTransform.h: interface for the CXTPMarkupRenderTransform class.
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

//{{AFX_CODEJOCK_PRIVATE
#if !defined(__XTPMARKUPRENDERTRANSFORM_H__)
#	define __XTPMARKUPRENDERTRANSFORM_H__
//}}AFX_CODEJOCK_PRIVATE

#	if _MSC_VER > 1000
#		pragma once
#	endif // _MSC_VER > 1000

#	include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

class CXTPMarkupRotateTransform;
class CXTPMarkupTransform;
class CXTPMarkupObject;
class CXTPMarkupDependencyProperty;
class CXTPMarkupBuilder;

class _XTP_EXT_CLASS CXTPMarkupRenderTransform : public CXTPMarkupObject
{
	DECLARE_MARKUPCLASS(CXTPMarkupRenderTransform);

public:
	CXTPMarkupRenderTransform();
	~CXTPMarkupRenderTransform();

	CXTPMarkupTransform* GetTransformObject() const;
	CXTPMarkupObject* GetTargetObject() const;

protected:
	virtual void SetContentObject(CXTPMarkupBuilder* pBuilder, CXTPMarkupObject* pContent);
	virtual void OnSetAsProperty(CXTPMarkupDependencyProperty* pProperty,
								 CXTPMarkupObject* pTargetObject);

private:
	CXTPMarkupDependencyProperty* m_pContentProperty;
	CXTPMarkupObject* m_pTargetObject;

public:
	static CXTPMarkupDependencyProperty* m_pPropertyRotateTransform;
	static CXTPMarkupDependencyProperty* m_pPropertyScaleTransform;
	static CXTPMarkupDependencyProperty* m_pPropertyTranslateTransform;
	static CXTPMarkupDependencyProperty* m_pPropertySkewTransform;
	static CXTPMarkupDependencyProperty* m_pPropertyMatrixTransform;

protected:
	double m_originX;
	double m_originY;

public:
	void SetOrigin(double x, double y);
	double GetOriginX();
	double GetOriginY();
};

AFX_INLINE CXTPMarkupObject* CXTPMarkupRenderTransform::GetTargetObject() const
{
	return m_pTargetObject;
}

AFX_INLINE void CXTPMarkupRenderTransform::SetOrigin(double x, double y)
{
	m_originX = x;
	m_originY = y;
}

AFX_INLINE double CXTPMarkupRenderTransform::GetOriginX()
{
	return m_originX;
}

AFX_INLINE double CXTPMarkupRenderTransform::GetOriginY()
{
	return m_originY;
}

#	include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPMARKUPRENDERTRANSFORM_H__)
