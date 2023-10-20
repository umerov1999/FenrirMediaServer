// XTPSkinObjectToolBar.h: interface for the CXTPSkinObjectToolBar class.
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
#if !defined(__XTPSKINOBJECTTOOLBAR_H__)
#	define __XTPSKINOBJECTTOOLBAR_H__
//}}AFX_CODEJOCK_PRIVATE

#	if _MSC_VER > 1000
#		pragma once
#	endif // _MSC_VER > 1000

#	include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

//===========================================================================
// Summary:
//     CXTPSkinObjectToolBar is a CXTPSkinObjectFrame derived class.
//     It implements a standard toolbar control.
//===========================================================================
class _XTP_EXT_CLASS CXTPSkinObjectToolBar : public CXTPSkinObjectFrame
{
	DECLARE_DYNCREATE(CXTPSkinObjectToolBar)
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPSkinObjectToolBar object.
	//-----------------------------------------------------------------------
	CXTPSkinObjectToolBar();

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPSkinObjectToolBar object, handles cleanup and deallocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPSkinObjectToolBar();

protected:
	void FillBackOfViewControlClass();
	void DrawToolbarH(CDC* pDC);
	void DrawToolbarV(CDC* pDC);
	void DrawButton(CDC* pDC, int nIndex);
	HIMAGELIST GetImageList(int iMode, int iIndex);

	BOOL HasDropDownArrow(LPTBBUTTON ptbb);
	BOOL HasSplitDropDown(LPTBBUTTON ptbb);
	void DrawButtonImage(CDC* pDC, int x, int y, int nIndex);
	BOOL HasButtonImage(LPTBBUTTON ptbb);
	BOOL IsHorizontal();
	BOOL IsAlphaImageList(HIMAGELIST himl);
	void UpdateToolTipsPos();

private:
	int GetToolbarButtonCount() const;
	BOOL GetToolbarButton(int nIndex, LPTBBUTTON lpButton) const;
	BOOL GetToolbarItemRect(int nIndex, LPRECT lpRect) const;
	int GetToolbarHotItem() const;

protected:
	//{{AFX_CODEJOCK_PRIVATE
	DECLARE_MESSAGE_MAP()

	LRESULT CustomDrawNotify(DWORD dwDrawStage, NMCUSTOMDRAW* pnmcd);
	virtual BOOL CheckDrawingEnabled();

	//{{AFX_VIRTUAL(CXTPSkinObjectToolBar)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CXTPSkinObjectToolBar)
	void OnPaint();
	BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	BOOL IsSpecialSystemToolbar();
	int m_nSpecialToolbar;

	CMap<HIMAGELIST, HIMAGELIST, BOOL, BOOL> m_mapAlphaImageList;

	//}}AFX_CODEJOCK_PRIVATE
};

#	include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPSKINOBJECTTOOLBAR_H__)
