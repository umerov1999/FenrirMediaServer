// XTPSkinObjectEdit.cpp: implementation of the CXTPSkinObjectEdit class.
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

#include "stdafx.h"

#include "Common/XTPTypeId.h"
#include "Common/XTPFramework.h"
#include "Common/XTPSystemHelpers.h"
#include "Common/XTPSynchro.h"
#include "Common/XTPApplication.h"
#include "Common/XTPSingleton.h"
#include "Common/XTPGdiObjects.h"
#include "Common/XTPDrawHelpers.h"
#include "Common/ScrollBar/XTPScrollBase.h"
#include "Common/XTPSynchro.h"

#include "SkinFramework/XTPSkinObject.h"
#include "SkinFramework/XTPSkinObjectFrame.h"
#include "SkinFramework/XTPSkinObjectEdit.h"

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

#ifdef _DEBUG
#	undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#	define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CXTPSkinObjectEdit, CXTPSkinObjectFrame)

CXTPSkinObjectEdit::CXTPSkinObjectEdit()
{
	m_strClassName	 = _T("EDIT");
	m_nCtlColorMessage = WM_CTLCOLOREDIT;
}

CXTPSkinObjectEdit::~CXTPSkinObjectEdit()
{
}

BEGIN_MESSAGE_MAP(CXTPSkinObjectEdit, CXTPSkinObjectUser32Control)
	//{{AFX_MSG_MAP(CXTPSkinObjectButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CXTPSkinObjectEdit::DrawFrame(CDC* pDC)
{
	CXTPWindowRect rcWindow(this);

	// Spin Button Check
	HWND hWndNext = ::GetWindow(m_hWnd, GW_HWNDNEXT);
	if (hWndNext && hWndNext != m_hWnd)
	{
		CXTPWindowRect rcWindowNext(hWndNext);
		CRect rcIntersect;
		if (rcIntersect.IntersectRect(rcWindow, rcWindowNext)
			&& rcIntersect.Width() == XTP_DPI_X(2))
		{
			rcIntersect.OffsetRect(-rcWindow.TopLeft());
			pDC->ExcludeClipRect(rcIntersect);
		}
	}

	CXTPSkinObjectFrame::DrawFrame(pDC);
}

int CXTPSkinObjectEdit::GetClientBrushMessage()
{
	return (!IsWindowEnabled()) || (GetStyle() & ES_READONLY) ? WM_CTLCOLORSTATIC : WM_CTLCOLOREDIT;
}
