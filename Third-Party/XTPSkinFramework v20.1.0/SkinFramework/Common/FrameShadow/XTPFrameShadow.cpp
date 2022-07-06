// XTPFrameShadow.cpp : implementation file
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
#include "GraphicLibrary/GdiPlus/XTPGdiPlus.h"

#include "Common/XTPTypeId.h"
#include "Common/XTPCasting.h"
#include "Common/XTPFramework.h"
#include "Common/XTPSystemHelpers.h"
#include "Common/XTPSynchro.h"
#include "Common/XTPApplication.h"
#include "Common/XTPSingleton.h"
#include "Common/XTPGdiObjects.h"
#include "Common/XTPDrawHelpers.h"

#include "Common/FrameShadow/XTPFrameShadowManager.h"
#include "Common/FrameShadow/XTPFrameShadow.h"
#include "Common/IIDs/XTPFrameShadowManagerIIDs.h"

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

#ifdef _DEBUG
#	define new DEBUG_NEW
#	undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace Gdiplus;
using namespace Gdiplus::DllExports;

#ifndef WS_EX_LAYERED
#	define WS_EX_LAYERED 0x00080000
#endif

#ifndef AC_SRC_ALPHA
#	define AC_SRC_ALPHA 0x01
#endif

#ifndef ULW_ALPHA
#	define ULW_ALPHA 0x00000002
#endif

#ifndef WS_EX_NOACTIVATE
#	define WS_EX_NOACTIVATE 0x08000000L
#endif

#ifndef CS_DROPSHADOW
#	define CS_DROPSHADOW 0x00020000
#endif

#define ID_UPDATEPOSITION_TIMER 1

// CXTPFrameShadow

CXTPFrameShadow::PFNUpdateLayeredWindow CXTPFrameShadow::m_pfnUpdateLayeredWindow = NULL;
BOOL CXTPFrameShadow::m_bSupportTested											  = FALSE;

struct CXTPFrameShadow::PreComputedData
{
	PreComputedData();
	~PreComputedData();

	CSize size;
	CRect clientRect;

	Color startColor;
	Color endColor;

	Rect leftRect;
	LinearGradientBrush* leftBrush;

	Rect topRect;
	LinearGradientBrush* topBrush;

	Rect rightRect;
	LinearGradientBrush* rightBrush;

	Rect bottomRect;
	LinearGradientBrush* bottomBrush;

	Rect backgroundRect;
	LinearGradientBrush* backgroundBrush;

	Rect leftTopRect;
	GraphicsPath leftTopPath;
	PathGradientBrush* leftTopBrush;

	Rect rightTopRect;
	GraphicsPath rightTopPath;
	PathGradientBrush* rightTopBrush;

	Rect rightBottomRect;
	GraphicsPath rightBottomPath;
	PathGradientBrush* rightBottomBrush;

	Rect leftBottomRect;
	GraphicsPath leftBottomPath;
	PathGradientBrush* leftBottomBrush;
};

CXTPFrameShadow::PreComputedData::PreComputedData()
{
	leftBrush		 = NULL;
	topBrush		 = NULL;
	rightBrush		 = NULL;
	bottomBrush		 = NULL;
	backgroundBrush	 = NULL;
	leftTopBrush	 = NULL;
	rightTopBrush	 = NULL;
	rightBottomBrush = NULL;
	leftBottomBrush	 = NULL;
}

CXTPFrameShadow::PreComputedData::~PreComputedData()
{
	ASSERT(NULL != leftBrush);
	delete leftBrush;

	ASSERT(NULL != topBrush);
	delete topBrush;

	ASSERT(NULL != rightBrush);
	delete rightBrush;

	ASSERT(NULL != bottomBrush);
	delete bottomBrush;

	ASSERT(NULL != backgroundBrush);
	delete backgroundBrush;

	ASSERT(NULL != leftTopBrush);
	delete leftTopBrush;

	ASSERT(NULL != rightTopBrush);
	delete rightTopBrush;

	ASSERT(NULL != rightBottomBrush);
	delete rightBottomBrush;

	ASSERT(NULL != leftBottomBrush);
	delete leftBottomBrush;
}

IMPLEMENT_DYNAMIC(CXTPFrameShadow, CWnd)

CXTPFrameShadow::CXTPFrameShadow(HWND hOwner)
	: m_hOwner(hOwner)
	, m_bCreated(FALSE)
	, m_bVisible(FALSE)
	, m_bPendingUpdate(FALSE)
	, m_bIsIconic(FALSE)
	, m_bFreezeUpdatesCounter(0)
	, m_bSizingAllowed(-1)
	, m_bBeingUpdated(FALSE)
	, m_pPCD(NULL)
{
	ASSERT(::IsWindow(m_hOwner));

	LoadDefaults();

	XTPFrameShadowManager()->Subscribe(this);
#ifdef _XTP_ACTIVEX
	EnableAutomation();
	EnableTypeLib();
#endif
}

CXTPFrameShadow::~CXTPFrameShadow()
{
	XTPFrameShadowManager()->Unsubscribe(this);

	if (NULL != m_pPCD)
		delete m_pPCD;
}

BOOL AFX_CDECL CXTPFrameShadow::IsShadowSupported()
{
	if (!m_bSupportTested && NULL == m_pfnUpdateLayeredWindow)
	{
		HMODULE hUser32			 = GetModuleHandle(_T("user32"));
		m_pfnUpdateLayeredWindow = XTPToFunctionPtr<PFNUpdateLayeredWindow>(
			::GetProcAddress(hUser32, "UpdateLayeredWindow"));
		m_bSupportTested = TRUE;
	}

	return NULL != m_pfnUpdateLayeredWindow;
}

BOOL CXTPFrameShadow::Create(COLORREF crColor, UINT nOuterSize, UINT nInnerSize /*= 0*/,
							 CPoint ptOffset /*= CPoint(0, 0)*/, BYTE bTransparency /*= 0x80*/)
{
	ASSERT(!m_bCreated);

	BOOL bSuccess = FALSE;

	if (IsShadowSupported())
	{
		DWORD exStyle = WS_EX_TOOLWINDOW | WS_EX_LAYERED | WS_EX_NOACTIVATE;
		if (!IsSizingAllowed())
		{
			exStyle |= WS_EX_TRANSPARENT;
		}

		bSuccess = CWnd::CreateEx(exStyle, AfxRegisterWndClass(0), _T("XTPFrameShadow"), WS_POPUP,
								  CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL,
								  NULL);
		if (bSuccess)
		{
			m_bCreated		= TRUE;
			m_crColor		= crColor;
			m_nOuterSize	= nOuterSize;
			m_nInnerSize	= nInnerSize;
			m_ptOffset		= ptOffset;
			m_bTransparency = bTransparency;
			SetDefaultBackgroundBlendingOptions();

			if (::IsWindowVisible(m_hOwner))
			{
				Show();
			}
		}
	}

	return bSuccess;
}

void CXTPFrameShadow::FreezeUpdates(BOOL bFreeze /*= TRUE*/)
{
	m_bFreezeUpdatesCounter += (bFreeze ? 1 : -1);

	if (m_bFreezeUpdatesCounter < 0)
	{
		ASSERT(!"Extra freeze counter release");
		m_bFreezeUpdatesCounter = 0;
	}

	if (0 == m_bFreezeUpdatesCounter && m_bPendingUpdate)
	{
		m_bPendingUpdate = FALSE;

		ComputeData();
		SendMessage(WM_PAINT);

		Update(TRUE);
	}
}

void CXTPFrameShadow::SetOuterSize(UINT nSize)
{
	if (nSize != m_nOuterSize)
	{
		m_nOuterSize = nSize;
		Update(TRUE);
	}
}

void CXTPFrameShadow::SetInnerSize(UINT nSize)
{
	if (nSize != m_nInnerSize)
	{
		m_nInnerSize = nSize;
		Update(TRUE);
	}
}

void CXTPFrameShadow::SetOffset(CPoint ptOffset)
{
	if (m_ptOffset != ptOffset)
	{
		m_ptOffset = ptOffset;
		Update(TRUE);
	}
}

void CXTPFrameShadow::SetColor(COLORREF crColor)
{
	if (m_crColor != crColor)
	{
		m_crColor = crColor;
		Update(FALSE);
	}
}

void CXTPFrameShadow::SetTransparency(BYTE bTransparency)
{
	if (m_bTransparency != bTransparency)
	{
		m_bTransparency = bTransparency;
		Update(FALSE);
	}
}

UINT CXTPFrameShadow::GetBlendingOptions(OUT OPTIONAL float* pFactors,
										 OUT OPTIONAL float* pPositions) const
{
	UINT nCount = (UINT)m_BlendingOptions.Factors.GetSize();
	if (0 < nCount)
	{
		if (NULL != pFactors)
		{
			memcpy(pFactors, m_BlendingOptions.Factors.GetData(), nCount * sizeof(float));
		}

		if (NULL != pPositions)
		{
			memcpy(pPositions, m_BlendingOptions.Positions.GetData(), nCount * sizeof(float));
		}
	}

	return nCount;
}

void CXTPFrameShadow::SetBlendingOptions(UINT nCount, const float* pFactors,
										 const float* pPositions)
{
	if (0 < nCount)
	{
		ASSERT(1 < nCount);
		ASSERT(NULL != pFactors);
		ASSERT(NULL != pPositions);

		m_BlendingOptions.Factors.SetSize(XTPToIntPtrChecked(nCount));
		m_BlendingOptions.Positions.SetSize(XTPToIntPtrChecked(nCount));
		m_BlendingOptions.CornerFactors.SetSize(XTPToIntPtrChecked(nCount));
		m_BlendingOptions.CornerPositions.SetSize(XTPToIntPtrChecked(nCount));

		for (INT_PTR i = 0; i < XTPToIntPtrChecked(nCount); ++i)
		{
			m_BlendingOptions.Factors.SetAt(i, 1 - pFactors[i]);
			m_BlendingOptions.Positions.SetAt(i, pPositions[i]);

			m_BlendingOptions.CornerFactors.SetAt(XTPToIntPtrChecked(nCount) - i - 1, pFactors[i]);
			float fltCornerPosition = (0 < pPositions[i] && pPositions[i] < 1 ? 1 - pPositions[i]
																			  : pPositions[i]);
			m_BlendingOptions.CornerPositions.SetAt(i, fltCornerPosition);
		}

		m_BlendingOptions.BackgroundFactors.SetSize(2);

		float nInnerFactor = m_BlendingOptions.Factors.GetAt(0);
		m_BlendingOptions.BackgroundFactors.SetAt(0, nInnerFactor);
		m_BlendingOptions.BackgroundFactors.SetAt(1, nInnerFactor);

		m_BlendingOptions.BackgroundPositions.SetSize(2);
		m_BlendingOptions.BackgroundPositions.SetAt(0, 0.f);
		m_BlendingOptions.BackgroundPositions.SetAt(1, 1.f);
	}
	else
	{
		m_BlendingOptions.Factors.RemoveAll();
		m_BlendingOptions.Positions.RemoveAll();
		m_BlendingOptions.CornerFactors.RemoveAll();
		m_BlendingOptions.CornerPositions.RemoveAll();
		SetDefaultBackgroundBlendingOptions();
	}

	Update(FALSE);
}

void CXTPFrameShadow::SetClipOwner(BOOL bClipOwner)
{
	m_bClipOwner = bClipOwner;
	Update(FALSE);
}

void CXTPFrameShadow::Show()
{
	if (!m_bVisible)
	{
		m_bVisible = TRUE;
		ShowWindow(SW_SHOWNOACTIVATE);
		Update(TRUE);
	}
}

void CXTPFrameShadow::Hide()
{
	ShowWindow(SW_HIDE);
	m_bVisible = FALSE;
}

void CXTPFrameShadow::UpdatePosition()
{
	Update(TRUE, FALSE);
}

void CXTPFrameShadow::OnRestore()
{
	SetTimer(ID_UPDATEPOSITION_TIMER, 250, NULL);
}

void CXTPFrameShadow::OnOwnersStyleChanged(DWORD oldStyle, DWORD newStyle)
{
	BOOL bSizingEnabled	 = (0 == (oldStyle & WS_SIZEBOX) && 0 != (newStyle & WS_SIZEBOX));
	BOOL bSizingDisabled = (0 != (oldStyle & WS_SIZEBOX) && 0 == (newStyle & WS_SIZEBOX));
	BOOL bTransparent	 = (0 != (GetExStyle() & WS_EX_TRANSPARENT));

	if (bSizingEnabled && bTransparent)
	{
		ModifyStyleEx(WS_EX_TRANSPARENT, 0);
	}
	else if (bSizingDisabled && !bTransparent)
	{
		ModifyStyleEx(0, WS_EX_TRANSPARENT);
	}
}

#include "Common/Base/Diagnostic/XTPBeginAfxMap.h"
BEGIN_MESSAGE_MAP(CXTPFrameShadow, CWnd)
	ON_WM_PAINT()
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_WM_SETCURSOR()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEACTIVATE()
END_MESSAGE_MAP()
#include "Common/Base/Diagnostic/XTPEndAfxMap.h"

void CXTPFrameShadow::OnPaint()
{
	if (NULL == m_pPCD)
	{
		ComputeData();
	}

	CPaintDC dc(this);

	BOOL bInvisible	 = (0 != (XTPFrameShadowManager()->GetFlags() & xtpFrameShadowsInvisible));
	BOOL bResizeable = (0 == (XTPFrameShadowManager()->GetFlags() & xtpFrameShadowsNotResizeable));

	CSize clientSize = m_pPCD->clientRect.Size();
	HDC hdcMemory	 = CreateCompatibleDC(dc);
	HBITMAP hBitmap	 = CreateCompatibleBitmap(dc, clientSize.cx, clientSize.cy);
	HGDIOBJ hbmpOld	 = SelectObject(hdcMemory, hBitmap);

	if (bInvisible ? bResizeable : true)
	{
		Graphics graph(hdcMemory);
		graph.SetSmoothingMode(Gdiplus::SmoothingModeNone);

		if (m_bClipOwner)
		{
			CRect rcClip = GetClipRect();
			graph.ExcludeClip(Rect(rcClip.left, rcClip.top, rcClip.Width(), rcClip.Height()));
		}

		graph.FillRectangle(m_pPCD->leftBrush, m_pPCD->leftRect);
		graph.FillRectangle(m_pPCD->topBrush, m_pPCD->topRect);
		graph.FillRectangle(m_pPCD->rightBrush, m_pPCD->rightRect);
		graph.FillRectangle(m_pPCD->bottomBrush, m_pPCD->bottomRect);
		graph.FillRectangle(m_pPCD->backgroundBrush, m_pPCD->backgroundRect);

		graph.FillPie(m_pPCD->leftTopBrush, m_pPCD->leftTopRect, 270, -90);
		graph.FillPie(m_pPCD->rightTopBrush, m_pPCD->rightTopRect, 0, -90);
		graph.FillPie(m_pPCD->rightBottomBrush, m_pPCD->rightBottomRect, 90, -90);
		graph.FillPie(m_pPCD->leftBottomBrush, m_pPCD->leftBottomRect, 180, -90);
	}

	CXTPWindowRect rcWindow(this);
	CPoint ptWinPos = rcWindow.TopLeft();

	POINT ptSrc = { 0, 0 };
	BLENDFUNCTION blendFunction;
	blendFunction.BlendOp	  = 0;
	blendFunction.BlendFlags  = 0;
	blendFunction.AlphaFormat = AC_SRC_ALPHA;
	// If invisible but resizeable, made impossible to see but actually visible.
	blendFunction.SourceConstantAlpha = (bInvisible ? BYTE(bResizeable ? 2 : 0) : m_bTransparency);
	m_pfnUpdateLayeredWindow(m_hWnd, dc, &ptWinPos, &clientSize, hdcMemory, &ptSrc, 0,
							 &blendFunction, ULW_ALPHA);

	SelectObject(hdcMemory, hbmpOld);
	DeleteObject(hBitmap);
	::DeleteDC(hdcMemory);
}

void CXTPFrameShadow::OnDestroy()
{
	if (2 == InternalAddRef()) // The first call to OnDestroy
	{
		XTPFrameShadowManager()->DisableShadow(CWnd::FromHandle(m_hOwner));
		CWnd::OnDestroy();
	}

	InternalRelease();
}

void CXTPFrameShadow::OnTimer(UINT_PTR nIDEvent)
{
	if (ID_UPDATEPOSITION_TIMER == nIDEvent)
	{
		m_bIsIconic = FALSE;
		UpdatePosition();
		KillTimer(nIDEvent);
	}
}

void CXTPFrameShadow::OnFinalRelease()
{
	delete this;
}

void CXTPFrameShadow::OnFlagsChanged(CXTPFrameShadowManager* /*pManager*/, int nOldEffectiveFlags,
									 int nNewEffectiveFlags)
{
	if (m_bCreated)
	{
		// Enabling/disabling shadow's ability to resize the owner window.
		if ((nOldEffectiveFlags & xtpFrameShadowsNotResizeable)
			!= (nNewEffectiveFlags & xtpFrameShadowsNotResizeable))
		{
			BOOL bSizingAllowed = IsSizingAllowed();
			if (bSizingAllowed != m_bSizingAllowed)
			{
				m_bSizingAllowed = bSizingAllowed;
				if (m_bSizingAllowed)
				{
					ModifyStyleEx(WS_EX_TRANSPARENT, 0, SWP_FRAMECHANGED);
				}
				else
				{
					ModifyStyleEx(0, WS_EX_TRANSPARENT, SWP_FRAMECHANGED);
				}
			}
		}

		// Enabling/disabling shadow invisibility.
		if ((nOldEffectiveFlags & xtpFrameShadowsInvisible)
			!= (nNewEffectiveFlags & xtpFrameShadowsInvisible))
		{
			InvalidateRect(NULL);
			UpdateWindow();
		}
	}
}

void CXTPFrameShadow::LoadDefaults()
{
	CXTPFrameShadowManager* pManager = XTPFrameShadowManager();
	m_ptOffset						 = pManager->GetDefaultOffset();
	m_nOuterSize					 = pManager->GetDefaultOuterSize();
	m_nInnerSize					 = pManager->GetDefaultInnerSize();
	m_crColor						 = pManager->GetDefaultColor();
	m_bTransparency					 = pManager->GetDefaultTransparency();
	m_bClipOwner					 = pManager->GetDefaultClipOwner();

	UINT nCount = pManager->GetDefaultBlendingOptions(NULL, NULL);
	if (0 < nCount)
	{
		CArray<float, float> arrFactors, arrPositions;
		arrFactors.SetSize(XTPToIntPtrChecked(nCount));
		arrPositions.SetSize(XTPToIntPtrChecked(nCount));
		pManager->GetDefaultBlendingOptions(arrFactors.GetData(), arrPositions.GetData());
		SetBlendingOptions(nCount, arrFactors.GetData(), arrPositions.GetData());
	}
}

void CXTPFrameShadow::ComputeData()
{
	if (NULL != m_pPCD)
	{
		delete m_pPCD;
		m_pPCD = NULL;
	}

	m_pPCD = new PreComputedData;

	const REAL BrushCorrection = 0.1f; // LinearGradientBrush artifact issues workaround

	GetClientRect(m_pPCD->clientRect);
	CSize clientSize = m_pPCD->clientRect.Size();

	UINT nSize = m_nOuterSize + CXTP_min(m_nInnerSize, UINT((clientSize.cx - m_nOuterSize * 2) / 2));
	m_pPCD->size.cx = XTPToLongChecked(nSize);

	nSize = m_nOuterSize + CXTP_min(m_nInnerSize, UINT((clientSize.cy - m_nOuterSize * 2) / 2));
	m_pPCD->size.cy = XTPToLongChecked(nSize);

	m_pPCD->startColor = Color(0xff, GetRValue(m_crColor), GetGValue(m_crColor),
							   GetBValue(m_crColor));
	m_pPCD->endColor   = Color(0, GetRValue(m_crColor), GetGValue(m_crColor), GetBValue(m_crColor));

	// Left side
	m_pPCD->leftRect = Rect(0, m_pPCD->size.cy, m_pPCD->size.cx,
							clientSize.cy - m_pPCD->size.cy * 2);
#ifdef _DEBUG
#	pragma push_macro("new")
#	undef new
#endif
	m_pPCD->leftBrush = new LinearGradientBrush(
		PointF(static_cast<REAL>(m_pPCD->leftRect.Width) + static_cast<REAL>(BrushCorrection), 0),
		PointF(-BrushCorrection, 0.f), m_pPCD->startColor, m_pPCD->endColor);
#ifdef _DEBUG
#	pragma pop_macro("new")
#endif
	if (0 < m_BlendingOptions.Factors.GetSize())
	{
		m_pPCD->leftBrush->SetBlend(m_BlendingOptions.Factors.GetData(),
									m_BlendingOptions.Positions.GetData(),
									(INT)m_BlendingOptions.Factors.GetSize());
	}

	// Top side
	m_pPCD->topRect = Rect(m_pPCD->size.cx, 0, clientSize.cx - m_pPCD->size.cx * 2,
						   m_pPCD->size.cy);
#ifdef _DEBUG
#	pragma push_macro("new")
#	undef new
#endif
	m_pPCD->topBrush = new LinearGradientBrush(PointF(0, static_cast<REAL>(m_pPCD->topRect.Height)
															 + static_cast<REAL>(BrushCorrection)),
											   PointF(0, -BrushCorrection), m_pPCD->startColor,
											   m_pPCD->endColor);
#ifdef _DEBUG
#	pragma pop_macro("new")
#endif
	if (0 < m_BlendingOptions.Factors.GetSize())
	{
		m_pPCD->topBrush->SetBlend(m_BlendingOptions.Factors.GetData(),
								   m_BlendingOptions.Positions.GetData(),
								   (INT)m_BlendingOptions.Factors.GetSize());
	}

	// Right side
	m_pPCD->rightRect = Rect(clientSize.cx - m_pPCD->size.cx, m_pPCD->size.cy, m_pPCD->size.cx,
							 clientSize.cy - m_pPCD->size.cy * 2);
#ifdef _DEBUG
#	pragma push_macro("new")
#	undef new
#endif
	m_pPCD->rightBrush = new LinearGradientBrush(
		PointF(static_cast<REAL>(m_pPCD->rightRect.X) - static_cast<REAL>(BrushCorrection), 0),
		PointF(static_cast<REAL>(m_pPCD->rightRect.X) + static_cast<REAL>(m_pPCD->rightRect.Width)
				   + static_cast<REAL>(BrushCorrection),
			   0),
		m_pPCD->startColor, m_pPCD->endColor);
#ifdef _DEBUG
#	pragma pop_macro("new")
#endif
	if (0 < m_BlendingOptions.Factors.GetSize())
	{
		m_pPCD->rightBrush->SetBlend(m_BlendingOptions.Factors.GetData(),
									 m_BlendingOptions.Positions.GetData(),
									 (INT)m_BlendingOptions.Factors.GetSize());
	}

	// Bottom side
	m_pPCD->bottomRect = Rect(m_pPCD->size.cx, clientSize.cy - m_pPCD->size.cy,
							  clientSize.cx - m_pPCD->size.cx * 2, m_pPCD->size.cy);
#ifdef _DEBUG
#	pragma push_macro("new")
#	undef new
#endif
	m_pPCD->bottomBrush = new LinearGradientBrush(
		PointF(0, static_cast<REAL>(m_pPCD->bottomRect.Y) - static_cast<REAL>(BrushCorrection)),
		PointF(0, static_cast<REAL>(m_pPCD->bottomRect.Y)
					  + static_cast<REAL>(m_pPCD->bottomRect.Height)
					  + static_cast<REAL>(BrushCorrection)),
		m_pPCD->startColor, m_pPCD->endColor);
#ifdef _DEBUG
#	pragma pop_macro("new")
#endif
	if (0 < m_BlendingOptions.Factors.GetSize())
	{
		m_pPCD->bottomBrush->SetBlend(m_BlendingOptions.Factors.GetData(),
									  m_BlendingOptions.Positions.GetData(),
									  (INT)m_BlendingOptions.Factors.GetSize());
	}

	// Background
	m_pPCD->backgroundRect = Rect(m_pPCD->size.cx, m_pPCD->size.cy,
								  clientSize.cx - m_pPCD->size.cx * 2,
								  clientSize.cy - m_pPCD->size.cy * 2);
#ifdef _DEBUG
#	pragma push_macro("new")
#	undef new
#endif
	m_pPCD->backgroundBrush = new LinearGradientBrush(Point(0, 0),
													  Point(m_pPCD->backgroundRect.Width, 0),
													  m_pPCD->startColor, m_pPCD->endColor);
#ifdef _DEBUG
#	pragma pop_macro("new")
#endif
	if (0 < m_BlendingOptions.BackgroundFactors.GetSize())
	{
		m_pPCD->backgroundBrush->SetBlend(m_BlendingOptions.BackgroundFactors.GetData(),
										  m_BlendingOptions.BackgroundPositions.GetData(),
										  (INT)m_BlendingOptions.BackgroundFactors.GetSize());
	}

	// Left top corner
	int one				= 1;
	m_pPCD->leftTopRect = Rect(0, 0, m_pPCD->size.cx * 2, m_pPCD->size.cy * 2);
	m_pPCD->leftTopPath.AddEllipse(m_pPCD->leftTopRect);
#ifdef _DEBUG
#	pragma push_macro("new")
#	undef new
#endif
	m_pPCD->leftTopBrush = new PathGradientBrush(&m_pPCD->leftTopPath);
#ifdef _DEBUG
#	pragma pop_macro("new")
#endif
	m_pPCD->leftTopBrush->SetCenterColor(m_pPCD->startColor);
	m_pPCD->leftTopBrush->SetCenterPoint(Point(m_pPCD->size.cx, m_pPCD->size.cy));
	m_pPCD->leftTopBrush->SetSurroundColors(&m_pPCD->endColor, &one);
	if (0 < m_BlendingOptions.CornerFactors.GetSize())
	{
		m_pPCD->leftTopBrush->SetBlend(m_BlendingOptions.CornerFactors.GetData(),
									   m_BlendingOptions.CornerPositions.GetData(),
									   (INT)m_BlendingOptions.CornerFactors.GetSize());
	}

	// Right top corner
	m_pPCD->rightTopRect = Rect(clientSize.cx - m_pPCD->size.cx * 2, 0, m_pPCD->size.cx * 2,
								m_pPCD->size.cy * 2);
	m_pPCD->rightTopPath.AddEllipse(m_pPCD->rightTopRect);
#ifdef _DEBUG
#	pragma push_macro("new")
#	undef new
#endif
	m_pPCD->rightTopBrush = new PathGradientBrush(&m_pPCD->rightTopPath);
#ifdef _DEBUG
#	pragma pop_macro("new")
#endif
	m_pPCD->rightTopBrush->SetCenterColor(m_pPCD->startColor);
	m_pPCD->rightTopBrush->SetCenterPoint(
		Point(m_pPCD->size.cx + m_pPCD->rightTopRect.X, m_pPCD->size.cy));
	m_pPCD->rightTopBrush->SetSurroundColors(&m_pPCD->endColor, &one);
	if (0 < m_BlendingOptions.CornerFactors.GetSize())
	{
		m_pPCD->rightTopBrush->SetBlend(m_BlendingOptions.CornerFactors.GetData(),
										m_BlendingOptions.CornerPositions.GetData(),
										(INT)m_BlendingOptions.CornerFactors.GetSize());
	}

	// Right bottom corner
	m_pPCD->rightBottomRect = Rect(clientSize.cx - m_pPCD->size.cx * 2,
								   clientSize.cy - m_pPCD->size.cy * 2, m_pPCD->size.cx * 2,
								   m_pPCD->size.cy * 2);
	m_pPCD->rightBottomPath.AddEllipse(m_pPCD->rightBottomRect);
#ifdef _DEBUG
#	pragma push_macro("new")
#	undef new
#endif
	m_pPCD->rightBottomBrush = new PathGradientBrush(&m_pPCD->rightBottomPath);
#ifdef _DEBUG
#	pragma pop_macro("new")
#endif
	m_pPCD->rightBottomBrush->SetCenterColor(m_pPCD->startColor);
	m_pPCD->rightBottomBrush->SetCenterPoint(Point(m_pPCD->size.cx + m_pPCD->rightBottomRect.X,
												   m_pPCD->size.cy + m_pPCD->rightBottomRect.Y));
	m_pPCD->rightBottomBrush->SetSurroundColors(&m_pPCD->endColor, &one);
	if (0 < m_BlendingOptions.CornerFactors.GetSize())
	{
		m_pPCD->rightBottomBrush->SetBlend(m_BlendingOptions.CornerFactors.GetData(),
										   m_BlendingOptions.CornerPositions.GetData(),
										   (INT)m_BlendingOptions.CornerFactors.GetSize());
	}

	// Left bottom corner
	m_pPCD->leftBottomRect = Rect(0, clientSize.cy - m_pPCD->size.cy * 2, m_pPCD->size.cx * 2,
								  m_pPCD->size.cy * 2);
	m_pPCD->leftBottomPath.AddEllipse(m_pPCD->leftBottomRect);
#ifdef _DEBUG
#	pragma push_macro("new")
#	undef new
#endif
	m_pPCD->leftBottomBrush = new PathGradientBrush(&m_pPCD->leftBottomPath);
#ifdef _DEBUG
#	pragma pop_macro("new")
#endif
	m_pPCD->leftBottomBrush->SetCenterColor(m_pPCD->startColor);
	m_pPCD->leftBottomBrush->SetCenterPoint(
		Point(m_pPCD->size.cx, m_pPCD->size.cy + m_pPCD->leftBottomRect.Y));
	m_pPCD->leftBottomBrush->SetSurroundColors(&m_pPCD->endColor, &one);
	if (0 < m_BlendingOptions.CornerFactors.GetSize())
	{
		m_pPCD->leftBottomBrush->SetBlend(m_BlendingOptions.CornerFactors.GetData(),
										  m_BlendingOptions.CornerPositions.GetData(),
										  (INT)m_BlendingOptions.CornerFactors.GetSize());
	}
}

void CXTPFrameShadow::SetDefaultBackgroundBlendingOptions()
{
	m_BlendingOptions.BackgroundFactors.SetSize(2);
	m_BlendingOptions.BackgroundFactors.SetAt(0, 0.f);
	m_BlendingOptions.BackgroundFactors.SetAt(1, 0.f);

	m_BlendingOptions.BackgroundPositions.SetSize(2);
	m_BlendingOptions.BackgroundPositions.SetAt(0, 0.f);
	m_BlendingOptions.BackgroundPositions.SetAt(1, 1.f);
}

void CXTPFrameShadow::Update(BOOL bReposition, BOOL bRedraw /*= TRUE*/)
{
	if (m_bCreated && m_bVisible && !m_bBeingUpdated)
	{
		if (0 == m_bFreezeUpdatesCounter && !m_bIsIconic && ::IsWindowVisible(m_hOwner))
		{
			m_bBeingUpdated = TRUE;

			m_bIsIconic	  = ::IsIconic(m_hOwner);
			DWORD dwStyle = (DWORD)GetWindowLong(m_hOwner, GWL_STYLE);
			if ((0 == (dwStyle & WS_MAXIMIZE)) && !m_bIsIconic)
			{
				if (bReposition)
				{
					CXTPWindowRect rcWnd(m_hOwner);
					CXTPClientRect rcShadow(this);
					CSize szNewSize(rcWnd.Width() + XTPToIntChecked(m_nOuterSize) * 2,
									rcWnd.Height() + XTPToIntChecked(m_nOuterSize) * 2);

					UINT flags = SWP_NOREDRAW | SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOCOPYBITS;
					if (rcShadow.Size() != szNewSize)
					{
						bRedraw = TRUE;
					}
					else
					{
						flags |= SWP_NOSIZE;
					}

					if (!IsWindowVisible())
					{
						ASSERT(m_bVisible);
						flags |= SWP_SHOWWINDOW;
					}

					::SetWindowPos(m_hWnd, m_hOwner,
								   rcWnd.left - XTPToIntChecked(m_nOuterSize) + m_ptOffset.x,
								   rcWnd.top - XTPToIntChecked(m_nOuterSize) + m_ptOffset.y,
								   szNewSize.cx, szNewSize.cy, flags);
				}

				if (bRedraw)
				{
					ComputeData();

					InvalidateRect(NULL);
					UpdateWindow();
				}
			}
			else
			{
				// Do not show shadow if its owner is maximized or iconic.
				ShowWindow(SW_HIDE);
			}

			m_bBeingUpdated = FALSE;
		}
		else
		{
			m_bPendingUpdate = TRUE;
		}
	}
}

BOOL CXTPFrameShadow::IsSizingAllowed() const
{
	BOOL bOwnerSizeable = (0 != (::GetWindowLong(m_hOwner, GWL_STYLE) & WS_SIZEBOX));
	BOOL bResizeable = (0 == (XTPFrameShadowManager()->GetFlags() & xtpFrameShadowsNotResizeable));
	return bOwnerSizeable && bResizeable;
}

CXTPFrameShadow::HitTestResult CXTPFrameShadow::HitTest(CPoint point) const
{
	HitTestResult nHitTest = NoHit;

	CRect rcShadow;
	GetWindowRect(&rcShadow);
	if (rcShadow.PtInRect(point))
	{
		CRect rcOwner;
		::GetWindowRect(m_hOwner, &rcOwner);

		if (!rcOwner.PtInRect(point))
		{
			if (point.y < rcOwner.top)
			{
				if (point.x < rcOwner.left)
				{
					nHitTest = HitTopLeft;
				}
				else if (rcOwner.right <= point.x)
				{
					nHitTest = HitTopRight;
				}
				else
				{
					nHitTest = HitTop;
				}
			}
			else if (rcOwner.bottom <= point.y)
			{
				if (point.x < rcOwner.left)
				{
					nHitTest = HitBottomLeft;
				}
				else if (rcOwner.right <= point.x)
				{
					nHitTest = HitBottomRight;
				}
				else
				{
					nHitTest = HitBottom;
				}
			}
			else if (point.x < rcOwner.left)
			{
				nHitTest = HitLeft;
			}
			else if (rcOwner.right <= point.x)
			{
				nHitTest = HitRight;
			}
			else
			{
				// should never happen
			}
		}
	}

	return nHitTest;
}

BOOL CXTPFrameShadow::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	BOOL bHandled = CWnd::OnSetCursor(pWnd, nHitTest, message);

	HCURSOR hCursor = NULL;

	if (!bHandled)
	{
		if (m_bSizingAllowed)
		{
			CPoint point;
			GetCursorPos(&point);

			switch (HitTest(point))
			{
				case HitTop:
				case HitBottom: hCursor = AfxGetApp()->LoadStandardCursor(IDC_SIZENS); break;
				case HitLeft:
				case HitRight: hCursor = AfxGetApp()->LoadStandardCursor(IDC_SIZEWE); break;
				case HitTopLeft:
				case HitBottomRight: hCursor = AfxGetApp()->LoadStandardCursor(IDC_SIZENWSE); break;
				case HitTopRight:
				case HitBottomLeft: hCursor = AfxGetApp()->LoadStandardCursor(IDC_SIZENESW); break;
				case NoHit:
				default: hCursor = AfxGetApp()->LoadStandardCursor(IDC_ARROW); break;
			}
		}
		else
		{
			hCursor = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
		}
	}

	if (NULL != hCursor)
	{
		::SetCursor(hCursor);
		bHandled = TRUE;
	}

	return bHandled;
}

CSize CXTPFrameShadow::GetOwnerMinTrackSize() const
{
	MINMAXINFO mmi = { { 0 } };
	::SendMessage(m_hOwner, WM_GETMINMAXINFO, 0, reinterpret_cast<LPARAM>(&mmi));

	CSize szMinSize(mmi.ptMinTrackSize.x, mmi.ptMinTrackSize.y);
	if (0 == szMinSize.cx)
	{
		szMinSize.cx = ::GetSystemMetrics(SM_CXMIN);
	}

	if (0 == szMinSize.cy)
	{
		szMinSize.cy = ::GetSystemMetrics(SM_CYMIN);
	}

	return szMinSize;
}

CPoint CXTPFrameShadow::GetDraggingInitialOffset(HitTestResult nHitTest, CPoint ptStart) const
{
	CPoint ptOffset;
	CRect rcWnd;
	::GetWindowRect(m_hOwner, &rcWnd);

	switch (nHitTest)
	{
		case HitTop: ptOffset.y = ptStart.y - rcWnd.top; break;
		case HitTopRight:
			ptOffset.y = ptStart.y - rcWnd.top;
			ptOffset.x = ptStart.x - rcWnd.right;
			break;
		case HitRight: ptOffset.x = ptStart.x - rcWnd.right; break;
		case HitBottomRight:
			ptOffset.x = ptStart.x - rcWnd.right;
			ptOffset.y = ptStart.y - rcWnd.bottom;
			break;
		case HitBottom: ptOffset.y = ptStart.y - rcWnd.bottom; break;
		case HitBottomLeft:
			ptOffset.y = ptStart.y - rcWnd.bottom;
			ptOffset.x = ptStart.x - rcWnd.left;
			break;
		case HitLeft: ptOffset.x = ptStart.x - rcWnd.left; break;
		case HitTopLeft:
			ptOffset.y = ptStart.y - rcWnd.top;
			ptOffset.x = ptStart.x - rcWnd.left;
			break;
		case NoHit:
			// Do nothing
			break;
	}

	return ptOffset;
}

void CXTPFrameShadow::Drag(HitTestResult nHitTest, CPoint ptOffset, CSize szMinSize)
{
	CPoint pt;
	::GetCursorPos(&pt);

	CRect rcWnd;
	::GetWindowRect(m_hOwner, &rcWnd);

	BOOL bProceed = TRUE;
	switch (nHitTest)
	{
		case HitTop: rcWnd.top = __min(rcWnd.bottom - szMinSize.cy, pt.y - ptOffset.y); break;
		case HitTopRight:
			rcWnd.top	= __min(rcWnd.bottom - szMinSize.cy, pt.y - ptOffset.y);
			rcWnd.right = __max(rcWnd.left + szMinSize.cx, pt.x - ptOffset.x);
			break;
		case HitRight: rcWnd.right = __max(rcWnd.left + szMinSize.cx, pt.x - ptOffset.x); break;
		case HitBottomRight:
			rcWnd.right	 = __max(rcWnd.left + szMinSize.cx, pt.x - ptOffset.x);
			rcWnd.bottom = __max(rcWnd.top + szMinSize.cy, pt.y - ptOffset.y);
			break;
		case HitBottom: rcWnd.bottom = __max(rcWnd.top + szMinSize.cy, pt.y - ptOffset.y); break;
		case HitBottomLeft:
			rcWnd.bottom = __max(rcWnd.top + szMinSize.cy, pt.y - ptOffset.y);
			rcWnd.left	 = __min(rcWnd.right - szMinSize.cx, pt.x - ptOffset.x);
			break;
		case HitLeft: rcWnd.left = __min(rcWnd.right - szMinSize.cx, pt.x - ptOffset.x); break;
		case HitTopLeft:
			rcWnd.top  = __min(rcWnd.bottom - szMinSize.cy, pt.y - ptOffset.y);
			rcWnd.left = __min(rcWnd.right - szMinSize.cx, pt.x - ptOffset.x);
			break;
		case NoHit: bProceed = FALSE; break;
	}

	if (bProceed)
	{
		::SetWindowPos(m_hOwner, HWND_TOP, rcWnd.left, rcWnd.top, rcWnd.Width(), rcWnd.Height(),
					   SWP_NOZORDER | SWP_DEFERERASE);
		::UpdateWindow(m_hOwner);

		Update(TRUE);
	}
}

CRect CXTPFrameShadow::GetClipRect() const
{
	ASSERT(::IsWindow(m_hOwner));
	CRect rcClip;
	::GetWindowRect(m_hOwner, &rcClip);

	if (XTPSystemVersion()->IsWinVistaOrGreater())
	{
		typedef HRESULT(STDAPICALLTYPE * PFNDwmGetWindowAttribute)(HWND hwnd, DWORD dwAttribute,
																   PVOID pvAttribute,
																   DWORD cbAttribute);
		static PFNDwmGetWindowAttribute pfnDwmGetWindowAttribute = NULL;

		if (NULL == pfnDwmGetWindowAttribute)
		{
			static CXTPModuleHandle hDwmApi(_T("dwmapi.dll"));
			if (NULL != hDwmApi.GetHandle())
			{
				pfnDwmGetWindowAttribute = XTPToFunctionPtr<PFNDwmGetWindowAttribute>(
					::GetProcAddress(hDwmApi, "DwmGetWindowAttribute"));
			}
		}

		if (NULL != pfnDwmGetWindowAttribute)
		{
			CRect rcFrameBounds;
			if (S_OK
				== pfnDwmGetWindowAttribute(m_hOwner, 9 /*DWMWA_EXTENDED_FRAME_BOUNDS*/,
											&rcFrameBounds, sizeof(rcFrameBounds)))
			{
				double xScale, yScale;
				if (XTPDpiHelper()->GetPhysicalWindowScaleFactors(m_hWnd, xScale, yScale))
				{
					DEVMODE dm = { 0 };
					dm.dmSize  = sizeof(dm);
					if (XTPMultiMonitor()->GetDisplaySettings(m_hWnd, &dm))
					{
						CRect rcScreen = XTPMultiMonitor()->GetScreenArea(m_hOwner);

						// Compute frame bounds relatively to its own monitor
						rcFrameBounds.left = rcScreen.left + (rcFrameBounds.left - dm.dmPosition.x);
						rcFrameBounds.top  = rcScreen.top + (rcFrameBounds.top - dm.dmPosition.y);
						rcFrameBounds.right = rcScreen.left
											  + (rcFrameBounds.right - dm.dmPosition.x);
						rcFrameBounds.bottom = rcScreen.top
											   + (rcFrameBounds.bottom - dm.dmPosition.y);

						rcClip.left = static_cast<LONG>(
							static_cast<double>(rcFrameBounds.left - rcScreen.left) / xScale
							+ rcScreen.left);
						rcClip.right = static_cast<LONG>(
							static_cast<double>(rcFrameBounds.right - rcScreen.left) / xScale
							+ rcScreen.left);
						rcClip.top = static_cast<LONG>(
							static_cast<double>(rcFrameBounds.top - rcScreen.top) / yScale
							+ rcScreen.top);
						rcClip.bottom = static_cast<LONG>(
							static_cast<double>(rcFrameBounds.bottom - rcScreen.top) / yScale
							+ rcScreen.top);
					}
				}
			}
		}
	}

	ScreenToClient(rcClip);

	return rcClip;
}

void CXTPFrameShadow::OnLButtonDown(UINT nFlags, CPoint point)
{
	CWnd::OnLButtonDown(nFlags, point);

	BOOL bActivateOwner = TRUE;

	if (IsSizingAllowed() && ::IsWindowEnabled(m_hOwner))
	{
		CPoint ptStart;
		::GetCursorPos(&ptStart);

		HitTestResult nHitTest = HitTest(ptStart);
		if (NoHit != nHitTest)
		{
			CRect rcWndInitial;
			::GetWindowRect(m_hOwner, &rcWndInitial);

			CSize szMinSize = GetOwnerMinTrackSize();
			CPoint ptOffset = GetDraggingInitialOffset(nHitTest, ptStart);

			::SetFocus(m_hOwner);
			::SetCapture(m_hOwner);

			::PostMessage(m_hOwner, WM_ENTERSIZEMOVE, 0, 0);

			MSG msg;
			BOOL bRes;
			BOOL bDoLoop = TRUE;
			while (bDoLoop)
			{
				CXTPDrawHelpers::ProcessPendingPaintMessages();

				if (0 < (bRes = ::GetMessage(&msg, NULL, 0, 0)))
				{
					switch (msg.message)
					{
						case WM_KEYDOWN: bDoLoop = (VK_ESCAPE != msg.wParam); break;
						case WM_KILLFOCUS:
						case WM_LBUTTONUP: bDoLoop = FALSE; break;
						case WM_MOUSEMOVE: Drag(nHitTest, ptOffset, szMinSize); break;
						default: DispatchMessage(&msg); break;
					}
				}
				else
				{
					break;
				}

				if (::GetCapture() != m_hOwner)
				{
					// Owner lost capture. Must be window switched.
					bActivateOwner = FALSE;
					break;
				}
			}

			::PostMessage(m_hOwner, WM_EXITSIZEMOVE, 0, 0);

			::ReleaseCapture();
		}
	}

	if (bActivateOwner)
	{
		::SetFocus(m_hOwner);
	}

	::UpdateWindow(m_hOwner);
}

int CXTPFrameShadow::OnMouseActivate(CWnd* /*pDesktopWnd*/, UINT /*nHitTest*/, UINT message)
{
	int nResult = MA_NOACTIVATE;

	if (WM_LBUTTONDOWN == message && m_bSizingAllowed)
	{
		nResult = MA_ACTIVATE;
	}

	return nResult;
}

#ifdef _XTP_ACTIVEX

BEGIN_DISPATCH_MAP(CXTPFrameShadow, CCmdTarget)
	DISP_PROPERTY_EX_ID(CXTPFrameShadow, "OuterSize", 1, OleGetOuterSize, OleSetOuterSize, VT_I4)
	DISP_PROPERTY_EX_ID(CXTPFrameShadow, "InnerSize", 2, OleGetInnerSize, OleSetInnerSize, VT_I4)
	DISP_PROPERTY_EX_ID(CXTPFrameShadow, "OffsetX", 3, OleGetOffsetX, OleSetOffsetX,
						VT_XSIZE_PIXELS)
	DISP_PROPERTY_EX_ID(CXTPFrameShadow, "OffsetY", 4, OleGetOffsetY, OleSetOffsetY,
						VT_YSIZE_PIXELS)
	DISP_PROPERTY_EX_ID(CXTPFrameShadow, "Color", 5, OleGetColor, OleSetColor, VT_COLOR)
	DISP_PROPERTY_EX_ID(CXTPFrameShadow, "Transparency", 6, OleGetTransparency, OleSetTransparency,
						VT_I2)
	DISP_PROPERTY_EX_ID(CXTPFrameShadow, "ClipOwner", 15, OleGetClipOwner, OleSetClipOwner, VT_BOOL)
	DISP_PROPERTY_EX_ID(CXTPFrameShadow, "BlendingFactors", 7, OleGetBlendingFactors,
						SetNotSupported, VT_VARIANT)
	DISP_PROPERTY_EX_ID(CXTPFrameShadow, "BlendingPositions", 8, OleGetBlendingPositions,
						SetNotSupported, VT_VARIANT)
	DISP_PROPERTY_EX_ID(CXTPFrameShadow, "Owner", 9, OleGetOwner, SetNotSupported, VT_HANDLE)
	DISP_FUNCTION_ID(CXTPFrameShadow, "FreezeUpdates", 10, OleFreezeUpdates, VT_EMPTY, VTS_BOOL)
	DISP_FUNCTION_ID(CXTPFrameShadow, "Show", 11, OleShow, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION_ID(CXTPFrameShadow, "Hide", 12, OleHide, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION_ID(CXTPFrameShadow, "UpdatePosition", 13, OleUpdatePosition, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION_ID(CXTPFrameShadow, "SetBlendingOptions", 14, OleSetBlendingOptions, VT_EMPTY,
					 VTS_VARIANT VTS_VARIANT)
END_DISPATCH_MAP()

BEGIN_INTERFACE_MAP(CXTPFrameShadow, CCmdTarget)
	INTERFACE_PART(CXTPFrameShadow, XTPDIID_IFrameShadow, Dispatch)
	XTP_DECLARE_CMDTARGETPROVIDER_INTERFACE_PART(CXTPFrameShadow)
END_INTERFACE_MAP()

IMPLEMENT_OLETYPELIB_EX(CXTPFrameShadow, XTPDIID_IFrameShadow)
XTP_IMPLEMENT_CMDTARGETPROVIDER_INTERFACE(CXTPFrameShadow)

OLE_HANDLE CXTPFrameShadow::OleGetOwner()
{
	return (OLE_HANDLE) reinterpret_cast<LONG_PTR>(GetOwner());
}

void CXTPFrameShadow::OleSetOuterSize(long size)
{
	SetOuterSize((UINT)size);
}

long CXTPFrameShadow::OleGetOuterSize()
{
	return (long)GetOuterSize();
}

void CXTPFrameShadow::OleSetInnerSize(long size)
{
	SetInnerSize((UINT)size);
}

long CXTPFrameShadow::OleGetInnerSize()
{
	return (long)GetInnerSize();
}

OLE_XSIZE_PIXELS CXTPFrameShadow::OleGetOffsetX()
{
	CPoint p = GetOffset();
	return p.x;
}

void CXTPFrameShadow::OleSetOffsetX(OLE_XSIZE_PIXELS offset)
{
	CPoint p = GetOffset();
	p.x		 = offset;
	SetOffset(p);
}

OLE_YSIZE_PIXELS CXTPFrameShadow::OleGetOffsetY()
{
	CPoint p = GetOffset();
	return p.y;
}

void CXTPFrameShadow::OleSetOffsetY(OLE_YSIZE_PIXELS offset)
{
	CPoint p = GetOffset();
	p.y		 = offset;
	SetOffset(p);
}

OLE_COLOR CXTPFrameShadow::OleGetColor()
{
	return GetColor();
}

void CXTPFrameShadow::OleSetColor(OLE_COLOR color)
{
	SetColor(color);
}

short CXTPFrameShadow::OleGetTransparency()
{
	return (short)GetTransparency();
}

void CXTPFrameShadow::OleSetTransparency(short val)
{
	SetTransparency((BYTE)val);
}

BOOL CXTPFrameShadow::OleGetClipOwner()
{
	return GetClipOwner();
}

void CXTPFrameShadow::OleSetClipOwner(BOOL bClip)
{
	SetClipOwner(bClip);
}

void CXTPFrameShadow::OleFreezeUpdates(VARIANT_BOOL bFreeze)
{
	FreezeUpdates(bFreeze);
}

void CXTPFrameShadow::OleShow()
{
	Show();
}

void CXTPFrameShadow::OleHide()
{
	Hide();
}

void CXTPFrameShadow::OleUpdatePosition()
{
	UpdatePosition();
}

void CXTPFrameShadow::OleSetBlendingOptions(VARIANT factors, VARIANT positions)
{
	if ((factors.vt & VT_ARRAY) == 0)
		AfxThrowOleException(E_INVALIDARG);
	if ((factors.vt & VT_R4) == 0)
		AfxThrowOleException(E_INVALIDARG);
	if ((positions.vt & VT_ARRAY) == 0)
		AfxThrowOleException(E_INVALIDARG);
	if ((positions.vt & VT_R4) == 0)
		AfxThrowOleException(E_INVALIDARG);
	COleSafeArray saFac(factors), saPos(positions);

	long upper, low, count, count2, low2, upper2, i;
	saFac.GetUBound(1, &upper);
	saFac.GetLBound(1, &low);
	count = upper - low + 1;

	saPos.GetUBound(1, &upper2);
	saPos.GetLBound(1, &low2);

	count2 = upper - low + 1;
	if (count != count2)
		AfxThrowOleException(E_INVALIDARG);
	CArray<float, float&> facs, poss;
	facs.SetSize(count);
	poss.SetSize(count);
	int fType = factors.vt & (~VT_ARRAY);
	int pType = positions.vt & (~VT_ARRAY);
	for (i = low; i <= upper; i++)
	{
		float val;
		if (fType == VT_VARIANT)
		{
			VARIANT var;
			saFac.GetElement(&i, &var);
			val = var.fltVal;
		}
		else
			saFac.GetElement(&i, &val);
		facs[i - low] = val;
	}

	for (i = low2; i <= upper2; i++)
	{
		float val;
		if (pType == VT_VARIANT)
		{
			VARIANT var;
			saPos.GetElement(&i, &var);
			val = var.fltVal;
		}
		else
			saPos.GetElement(&i, &val);
		poss[i - low] = val;
	}
	SetBlendingOptions((UINT)count, facs.GetData(), poss.GetData());
}

VARIANT CXTPFrameShadow::OleGetBlendingFactors()
{
	COleSafeArray retVal;
	DWORD nElems[]			   = { 0 };
	UINT nBlendingOptionsCount = GetBlendingOptions(NULL, NULL);
	if (nBlendingOptionsCount)
		nElems[0] = nBlendingOptionsCount;
	retVal.Create(VT_R4, 1, nElems);
	if (!nBlendingOptionsCount)
		return retVal.Detach();
	CArray<float, float&> arrPositions;
	arrPositions.SetSize(nBlendingOptionsCount);
	GetBlendingOptions(NULL, arrPositions.GetData());
	long i;
	for (i = 0; i < (long)nBlendingOptionsCount; i++)
	{
		float fac = arrPositions.GetAt((UINT)i);
		retVal.PutElement(&i, &fac);
	}
	return retVal.Detach();
}

VARIANT CXTPFrameShadow::OleGetBlendingPositions()
{
	COleSafeArray retVal;
	DWORD nElems[]			   = { 0 };
	UINT nBlendingOptionsCount = GetBlendingOptions(NULL, NULL);
	if (nBlendingOptionsCount)
		nElems[0] = nBlendingOptionsCount;
	retVal.Create(VT_R4, 1, nElems);
	if (!nBlendingOptionsCount)
		return retVal.Detach();
	CArray<float, float&> arrFactors;
	arrFactors.SetSize(nBlendingOptionsCount);
	GetBlendingOptions(arrFactors.GetData(), NULL);
	long i;
	for (i = 0; i < (long)nBlendingOptionsCount; i++)
	{
		float fac = arrFactors.GetAt((UINT)i);
		retVal.PutElement(&i, &fac);
	}
	return retVal.Detach();
}

#endif
