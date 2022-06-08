// XTPMarkupContext.cpp: implementation of the CXTPMarkupContext class.
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

#include "Common/Base/Diagnostic/XTPDisableAdvancedWarnings.h"
#include <atlbase.h>
#include <ActivScp.h>
#include "Common/Base/Diagnostic/XTPEnableAdvancedWarnings.h"

#include "GraphicLibrary/GdiPlus/XTPGdiPlus.h"

#include "Common/XTPTypeId.h"
#include "Common/Math/XTPMathUtils.h"
#include "Common/XTPCasting.h"
#include "Common/XTPFramework.h"
#include "Common/XTPSystemHelpers.h"
#include "Common/XTPSynchro.h"
#include "Common/XTPApplication.h"
#include "Common/XTPSingleton.h"
#include "Common/XTPGdiObjects.h"
#include "Common/XTPActiveScriptEngine.h"
#include "Common/XTPToolTipContext.h"
#include "Common/XTPResourceManager.h"
#include "Common/XTPMarkupRender.h"
#include "Common/XTPImageManager.h"
#include "Common/XTPXMLHelpers.h"
#include "Common/XTPDrawHelpers.h"
#include "Common/Base/Types/XTPPoint2.h"
#include "Common/Base/Types/XTPSize.h"
#include "Common/Base/Types/XTPRect.h"

using namespace Gdiplus;
using namespace Gdiplus::DllExports;

#include "Markup/XTPMarkupTools.h"
#include "Markup/XTPMarkupObject.h"
#include "Markup/XTPMarkupStyle.h"
#include "Markup/XTPMarkupSetter.h"
#include "Markup/XTPMarkupString.h"
#include "Markup/XTPMarkupContext.h"
#include "Markup/XTPMarkupInputElement.h"
#include "Markup/XTPMarkupUIElement.h"
#include "Markup/XTPMarkupFrameworkElement.h"
#include "Markup/XTPMarkupTypedSimpleStack.h"
#include "Markup/XTPMarkupBuilder.h"
#include "Markup/XTPMarkupParser.h"
#include "Markup/XTPMarkupDrawingContext.h"
#include "Markup/XTPMarkupKeyboardNavigation.h"
#include "Markup/XTPMarkupDelegate.h"
#include "Markup/XTPMarkupEventHandlerMap.h"
#include "Markup/XTPMarkupRoutedEventArgs.h"
#include "Markup/XTPMarkupMouseEventArgs.h"
#include "Markup/XTPMarkupMouseButtonEventArgs.h"
#include "Markup/XTPMarkupMouseWheelEventArgs.h"
#include "Markup/XTPMarkupQueryCursorEventArgs.h"
#include "Markup/XTPMarkupFrameworkContentElement.h"
#include "Markup/XTPMarkupResourceDictionary.h"
#include "Markup/XTPMarkupThickness.h"
#include "Markup/XTPMarkupTrigger.h"
#include "Markup/DeviceContext/XTPMarkupDeviceDependentImage.h"

#include "Markup/Transform/XTPMarkupTransform.h"
#include "Markup/Transform/XTPMarkupRotateTransform.h"
#include "Markup/Transform/XTPMarkupSkewTransform.h"
#include "Markup/Transform/XTPMarkupScaleTransform.h"
#include "Markup/Transform/XTPMarkupTranslateTransform.h"
#include "Markup/Transform/XTPMarkupGdiPlusTransformObject.h"
#include "Markup/Transform/XTPMarkupGdiPlusRotateTransform.h"
#include "Markup/Transform/XTPMarkupGdiPlusScaleTransform.h"
#include "Markup/Transform/XTPMarkupGdiPlusSkewTransform.h"
#include "Markup/Transform/XTPMarkupGdiPlusTranslateTransform.h"
#include "Markup/Transform/XTPMarkupRenderTransform.h"

#include "Markup/Shapes/XTPMarkupShape.h"
#include "Markup/Shapes/XTPMarkupPath.h"
#include "Markup/Shapes/XTPMarkupPolygon.h"
#include "Markup/Shapes/XTPMarkupPolyline.h"
#include "Markup/Shapes/XTPMarkupLine.h"
#include "Markup/Shapes/XTPMarkupEllipse.h"
#include "Markup/Shapes/XTPMarkupRectangle.h"
#include "Markup/Shapes/XTPMarkupPathGeometry.h"
#include "Markup/Shapes/XTPMarkupPathGeometryFigures.h"
#include "Markup/Shapes/XTPMarkupPathData.h"

#include "Markup/Controls/XTPMarkupDecorator.h"
#include "Markup/Controls/XTPMarkupControl.h"
#include "Markup/Controls/XTPMarkupPanel.h"
#include "Markup/Controls/XTPMarkupBorder.h"
#include "Markup/Controls/XTPMarkupViewbox.h"
#include "Markup/Controls/XTPMarkupPage.h"
#include "Markup/Controls/XTPMarkupImage.h"
#include "Markup/Controls/XTPMarkupContentControl.h"
#include "Markup/Controls/XTPMarkupButtonBase.h"
#include "Markup/Controls/XTPMarkupDockPanel.h"
#include "Markup/Controls/XTPMarkupWrapPanel.h"
#include "Markup/Controls/XTPMarkupDefinitionBase.h"
#include "Markup/Controls/XTPMarkupColumnDefinition.h"
#include "Markup/Controls/XTPMarkupRowDefinition.h"
#include "Markup/Controls/XTPMarkupStackPanel.h"
#include "Markup/Controls/XTPMarkupGrid.h"
#include "Markup/Controls/XTPMarkupCanvas.h"
#include "Markup/Controls/XTPMarkupTextBlock.h"
#include "Markup/Controls/XTPMarkupToggleButton.h"
#include "Markup/Controls/XTPMarkupButton.h"
#include "Markup/Controls/XTPMarkupScrollViewer.h"
#include "Markup/Controls/XTPMarkupUniformGrid.h"
#include "Markup/Controls/XTPMarkupCheckbox.h"
#include "Markup/Controls/XTPMarkupRadioButton.h"

#include "Markup/Text/XTPMarkupTextElement.h"
#include "Markup/Text/XTPMarkupInline.h"
#include "Markup/Text/XTPMarkupSpan.h"
#include "Markup/Text/XTPMarkupRun.h"
#include "Markup/Text/XTPMarkupBold.h"
#include "Markup/Text/XTPMarkupItalic.h"
#include "Markup/Text/XTPMarkupLineBreak.h"
#include "Markup/Text/XTPMarkupUnderline.h"
#include "Markup/Text/XTPMarkupHyperlink.h"
#include "Markup/Text/XTPMarkupInlineUIContainer.h"

#include "Markup/Extensions/XTPMarkupStaticExtension.h"
#include "Markup/Extensions/XTPMarkupSystemColorsStaticExtension.h"
#include "Markup/Extensions/XTPMarkupAmbientColorsStaticExtension.h"
#include "Markup/Extensions/XTPMarkupExtension.h"
#include "Markup/Extensions/XTPMarkupXtpScript.h"

#include "Markup/XTPMarkupIIDs.h"

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

#ifdef _DEBUG
#	undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#	define new DEBUG_NEW
#endif

template<>
inline BOOL AFXAPI CompareElements(const LPCWSTR* pElement1, const LPCWSTR* pElement2)
{
	return wcscmp(*pElement1, *pElement2) == 0;
}

//////////////////////////////////////////////////////////////////////////
// CXTPMarkupDelegateMap

class CXTPMarkupDelegateMap
{
public:
	CXTPMarkupDelegateMap();
	~CXTPMarkupDelegateMap();

public:
	void RemoveAll();
	void Add(LPCWSTR lpszDelegate, CXTPMarkupDelegate* pDelegate);
	CXTPMarkupDelegate* Lookup(LPCWSTR lpszDelegate) const;

protected:
	CMap<LPCWSTR, LPCWSTR, CXTPMarkupDelegate*, CXTPMarkupDelegate*> m_mapDelegates;

public:
	LPDISPATCH m_lpHandler;
};

class CXTPMarkupOleAutoDelegate : public CXTPMarkupDelegate
{
public:
	CXTPMarkupOleAutoDelegate(LPWSTR lpName, const CXTPMarkupDelegateMap* pMap)
	{
		m_lpName = lpName;
		m_pMap	 = pMap;
	}

	~CXTPMarkupOleAutoDelegate()
	{
		delete[] m_lpName;
	}

	virtual void Execute(CXTPMarkupObject* pSender, CXTPMarkupRoutedEventArgs* pArgs);

protected:
	const CXTPMarkupDelegateMap* m_pMap;
	LPWSTR m_lpName;
};

CXTPMarkupDelegateMap::CXTPMarkupDelegateMap()
{
	m_lpHandler = NULL;
}

CXTPMarkupDelegateMap::~CXTPMarkupDelegateMap()
{
	RemoveAll();

	m_lpHandler = NULL;
}

void CXTPMarkupDelegateMap::RemoveAll()
{
	POSITION pos = m_mapDelegates.GetStartPosition();
	while (pos)
	{
		LPCWSTR lpszDelegate;
		CXTPMarkupDelegate* pDelegate;
		m_mapDelegates.GetNextAssoc(pos, lpszDelegate, pDelegate);

		MARKUP_RELEASE(pDelegate);
	}
	m_mapDelegates.RemoveAll();
}

void CXTPMarkupDelegateMap::Add(LPCWSTR lpszDelegate, CXTPMarkupDelegate* pDelegate)
{
	m_mapDelegates.SetAt(lpszDelegate, pDelegate);
}

CXTPMarkupDelegate* CXTPMarkupDelegateMap::Lookup(LPCWSTR lpszDelegate) const
{
	CXTPMarkupDelegate* pDelegate = NULL;
	if (!m_mapDelegates.Lookup(lpszDelegate, pDelegate))
		return NULL;

	return pDelegate;
}

class CXTPMarkupContext::CInputElementCollection : public CXTPMarkupCollection
{
public:
	CInputElementCollection()
	{
		m_pElementType	 = MARKUP_TYPE(CXTPMarkupInputElement);
		m_bLogicalParent = FALSE;
	}

public:
	CXTPMarkupInputElement* GetItem(int nIndex) const
	{
		return (CXTPMarkupInputElement*)m_arrItems[nIndex];
	}
	CXTPMarkupInputElement* GetTailItem() const
	{
		return (CXTPMarkupInputElement*)m_arrItems[GetCount() - 1];
	}
};

class CXTPMarkupContext::CTooltipContext : public CXTPToolTipContext
{
public:
	CTooltipContext(CXTPMarkupContext* pMarkupContext)
	{
		m_pMarkupContext = pMarkupContext;
	}

	INT_PTR OnToolHitTest(CWnd* /*pWnd*/, CPoint point, TOOLINFO* pToolInfo)
	{
		return m_pMarkupContext->OnToolHitTest(point, pToolInfo);
	}

protected:
	CXTPMarkupContext* m_pMarkupContext;
};

//////////////////////////////////////////////////////////////////////////
// CXTPMarkupContext

CXTPMarkupContext::CXTPMarkupContext()
{
	CXTPNonClientMetrics ncm;
	ncm.lfMessageFont.lfCharSet = XTPResourceManager()->GetFontCharset();

	m_dwRef	  = 1;
	m_logFont = ncm.lfMessageFont;

	// Some fonts defaults have to be overridden.
	m_logFont.lfWidth		= 0;
	m_logFont.lfEscapement	= 0;
	m_logFont.lfOrientation = 0;
	m_logFont.lfWeight		= FW_DONTCARE;
	m_logFont.lfItalic		= FALSE;
	m_logFont.lfStrikeOut	= FALSE;
	m_logFont.lfUnderline	= FALSE;

	m_DefaultSmoothingMode	= xtpMarkupSmoothingDefault;
	m_bDefaultTextSmoothing = FALSE;

	m_clrForeground	 = 0;
	m_clrImgColorKey = COLORREF_NULL;
	m_pImageManager	 = NULL;

	m_pMouseOver	 = NULL;
	m_pMouseCapture	 = NULL;
	m_pActiveElement = NULL;
	m_hContextWnd	 = NULL;

	m_pHandlers	 = NULL;
	m_pDelegates = NULL;

	m_pFonts			  = NULL;
	m_pKeyboardNavigation = new CXTPMarkupKeyboardNavigation(this);

	m_pToolTipContext = new CTooltipContext(this);
	m_pToolTipContext->SetStyle(xtpToolTipMarkup);

	m_bUseCustomToolTipPos = FALSE;
	m_ptCustomToolTipPos   = CPoint(0, 0);

	m_lpszScriptEngineLanguage = NULL;
	m_pScriptEngine			   = NULL;
	m_pScriptEngineStdRuntime  = NULL;

	m_bEnableGdiPlus = TRUE;
	m_pDpi			 = NULL;

	m_pExtensionRoot = NULL;

	m_bUpdatesFrozen	  = FALSE;
	m_bEntireUpdatePended = FALSE;

#ifdef _XTP_ACTIVEX
	EnableAutomation();
	EnableTypeLib();
#else
	m_xActiveScriptSite.pThis = this;
	m_pInternalDispatch		  = NULL;
#endif

	m_pDelegates = new CXTPMarkupDelegateMap();

	RegisterClasses();

	CXTPMarkupDrawingContext::Register(TRUE);
}

void CXTPMarkupContext::RegisterClasses()
{
	static BOOL bRegistered = FALSE;
	if (bRegistered)
		return;

	bRegistered = TRUE;

	CXTPMarkupType::RegisterAll();

	CXTPMarkupBorder::RegisterType();
	CXTPMarkupViewbox::RegisterType();
	CXTPMarkupButton::RegisterType();
	CXTPMarkupCanvas::RegisterType();
	CXTPMarkupCheckBox::RegisterType();
	CXTPMarkupColumnDefinition::RegisterType();
	CXTPMarkupControl::RegisterType();
	CXTPMarkupDefinitionBase::RegisterType();
	CXTPMarkupDockPanel::RegisterType();
	CXTPMarkupGrid::RegisterType();
	CXTPMarkupImage::RegisterType();
	CXTPMarkupPage::RegisterType();
	CXTPMarkupWindowContainer::RegisterType();
	CXTPMarkupPanel::RegisterType();
	CXTPMarkupRadioButton::RegisterType();
	CXTPMarkupRowDefinition::RegisterType();
	CXTPMarkupScrollViewer::RegisterType();
	CXTPMarkupStackPanel::RegisterType();
	CXTPMarkupTextBlock::RegisterType();
	CXTPMarkupUniformGrid::RegisterType();
	CXTPMarkupWrapPanel::RegisterType();
	CXTPMarkupXtpScript::RegisterType();
	CXTPMarkupEllipse::RegisterType();
	CXTPMarkupLine::RegisterType();
	CXTPMarkupPath::RegisterType();
	CXTPMarkupPathData::RegisterType();
	CXTPMarkupPathGeometry::RegisterType();
	CXTPMarkupPathGeometryFigures::RegisterType();
	CXTPMarkupPolygon::RegisterType();
	CXTPMarkupPolyline::RegisterType();
	CXTPMarkupRectangle::RegisterType();
	CXTPMarkupShape::RegisterType();
	CXTPMarkupBold::RegisterType();
	CXTPMarkupHyperlink::RegisterType();
	CXTPMarkupInlineUIContainer::RegisterType();
	CXTPMarkupItalic::RegisterType();
	CXTPMarkupLineBreak::RegisterType();
	CXTPMarkupRun::RegisterType();
	CXTPMarkupSpan::RegisterType();
	CXTPMarkupTextElement::RegisterType();
	CXTPMarkupUnderline::RegisterType();
	CXTPMarkupGdiPlusRotateTransform::RegisterType();
	CXTPMarkupGdiPlusScaleTransform::RegisterType();
	CXTPMarkupGdiPlusSkewTransform::RegisterType();
	CXTPMarkupGdiPlusTranslateTransform::RegisterType();
	CXTPMarkupRenderTransform::RegisterType();
	CXTPMarkupGradientStop::RegisterType();
	CXTPMarkupGradientStops::RegisterType();
	CXTPMarkupLinearGradientBrush::RegisterType();
	CXTPMarkupSolidColorBrush::RegisterType();
	CXTPMarkupFrameworkContentElement::RegisterType();
	CXTPMarkupFrameworkElement::RegisterType();
	CXTPMarkupKeyboardNavigation::RegisterType();
	CXTPMarkupColor::RegisterType();
	CXTPMarkupDoubleCollection::RegisterType();
	CXTPMarkupResourceDictionary::RegisterType();
	CXTPMarkupSetter::RegisterType();
	CXTPMarkupStyle::RegisterType();
	CXTPMarkupTrigger::RegisterType();
}

CXTPMarkupContext::~CXTPMarkupContext()
{
	ResetScriptEngine();

	if (NULL != m_pExtensionRoot)
	{
		delete m_pExtensionRoot;
		m_pExtensionRoot = NULL;
	}

	CMDTARGET_RELEASE(m_pImageManager);

	m_pMouseOver = NULL;
	MARKUP_RELEASE(m_pKeyboardNavigation);

	ASSERT(m_pFonts == NULL || m_pFonts->IsEmpty());

	OleUnfreezeUpdates();

	SAFE_DELETE(m_pHandlers);
	SAFE_DELETE(m_pDelegates);
	SAFE_DELETE(m_pFonts);

	CMDTARGET_RELEASE(m_pToolTipContext);

#ifndef _XTP_ACTIVEX
	CMDTARGET_RELEASE(m_pInternalDispatch);
#endif

	ASSERT(m_dwRef <= 1);

	CXTPMarkupDrawingContext::Register(FALSE);

	if (NULL != m_pDpi)
	{
		delete m_pDpi;
	}
}

CXTPMarkupStaticExtension* CXTPMarkupContext::GetExtensionRoot()
{
	if (NULL == m_pExtensionRoot)
	{
		class CRootStaticExtension : public CXTPMarkupStaticExtension
		{
		public:
			CRootStaticExtension()
				: CXTPMarkupStaticExtension(L"__root__", FALSE)
			{
			}
		};

		m_pExtensionRoot = new CRootStaticExtension();
		m_pExtensionRoot->Extend(new CXTPMarkupSystemColorsStaticExtension());
		m_pExtensionRoot->Extend(new CXTPMarkupAmbientColorsStaticExtension());
	}

	return m_pExtensionRoot;
}

int CXTPMarkupContext::ScaleX(int x) const
{
	return NULL != m_pDpi ? m_pDpi->ScaleX(x) : x;
}

int CXTPMarkupContext::ScaleY(int y) const
{
	return NULL != m_pDpi ? m_pDpi->ScaleY(y) : y;
}

float CXTPMarkupContext::ScaleX(float x) const
{
	return NULL != m_pDpi ? m_pDpi->ScaleX(x) : x;
}

float CXTPMarkupContext::ScaleY(float y) const
{
	return NULL != m_pDpi ? m_pDpi->ScaleY(y) : y;
}

double CXTPMarkupContext::ScaleX(double x) const
{
	return NULL != m_pDpi ? m_pDpi->ScaleX(x) : x;
}

double CXTPMarkupContext::ScaleY(double y) const
{
	return NULL != m_pDpi ? m_pDpi->ScaleY(y) : y;
}

CSize CXTPMarkupContext::Scale(const SIZE& size) const
{
	return NULL != m_pDpi
			   ? CSize(ScaleX(static_cast<int>(size.cx)), ScaleY(static_cast<int>(size.cy)))
			   : size;
}

CXTPMarkupObject* CXTPMarkupContext::CreateMarkupObject(CXTPMarkupType* pType)
{
	CXTPMarkupObject* pObject = pType->CreateObject(this);
	if (NULL != pObject)
	{
#ifdef _XTP_ACTIVEX
		InternalAddRef();
#else
		AddRef();
#endif
	}

	return (CXTPMarkupObject*)pObject;
}

void CXTPMarkupContext::Cleanup()
{
	m_pMouseOver	 = NULL;
	m_pActiveElement = NULL;

	if (m_pMouseCapture)
	{
		m_pMouseCapture = NULL;
		ReleaseCapture();
	}
}

void CXTPMarkupContext::FinalizeMarkupObject(CXTPMarkupObject* pObject)
{
	if (pObject == m_pMouseOver)
		m_pMouseOver = NULL;

	if (pObject == m_pActiveElement)
		m_pActiveElement = NULL;

	if (pObject == m_pMouseCapture)
	{
		m_pMouseCapture = NULL;
		ReleaseCapture();
	}

	Release();
}

CXTPMarkupUIElement* CXTPMarkupContext::Parse(LPCSTR lpszBuffer)
{
	Cleanup();

	if (!lpszBuffer)
		return NULL;

	CXTPMarkupUIElement* pRootElement = NULL;

	CXTPMarkupParser sc;
	if (sc.SetBuffer(lpszBuffer, lpszBuffer + strlen(lpszBuffer)))
	{
		CXTPMarkupBuilder builder(this);
		pRootElement = builder.Parse(&sc);
	}

	return pRootElement;
}

CXTPMarkupUIElement* CXTPMarkupContext::Parse(LPCWSTR lpszBuffer)
{
	Cleanup();

	if (!lpszBuffer)
		return NULL;

	CXTPMarkupUIElement* pRootElement = NULL;

	CXTPMarkupParser sc;
	if (sc.SetBuffer(lpszBuffer, lpszBuffer + wcslen(lpszBuffer)))
	{
		CXTPMarkupBuilder builder(this);
		pRootElement = builder.Parse(&sc);
	}

	return pRootElement;
}

template<class CharPtr>
static CString DoValidateXML(CharPtr lpszBuffer, SIZE_T cch, CXTPMarkupContext* pCtx)
{
	ASSERT(NULL != pCtx);

	if (NULL == lpszBuffer)
	{
		return _T("Done.");
	}

	CString strMessage;

	CXTPMarkupParser sc;
	if (!sc.SetBuffer(lpszBuffer, lpszBuffer + cch))
	{
		return sc.FormatErrorMessage();
	}

	CXTPMarkupBuilder builder(pCtx);
	CXTPMarkupUIElement* pUIElement = builder.Parse(&sc);
	if (NULL == pUIElement)
	{
		return builder.GetLastError();
	}

	MARKUP_RELEASE(pUIElement);

	return _T("Done.");
}

CString CXTPMarkupContext::ValidateXML(LPCSTR lpszBuffer)
{
	Cleanup();
	return DoValidateXML<LPCSTR>(lpszBuffer, (NULL != lpszBuffer ? strlen(lpszBuffer) : 0), this);
}

CString CXTPMarkupContext::ValidateXML(LPCWSTR lpszBuffer)
{
	Cleanup();
	return DoValidateXML<LPCWSTR>(lpszBuffer, (NULL != lpszBuffer ? wcslen(lpszBuffer) : 0), this);
}

BOOL CXTPMarkupContext::CompareFont(LOGFONT* pLogFont1, LOGFONT* pLogFont2)
{
	if (pLogFont1->lfHeight != pLogFont2->lfHeight)
		return FALSE;

	if (pLogFont1->lfWeight != pLogFont2->lfWeight)
		return FALSE;

	if (pLogFont1->lfItalic != pLogFont2->lfItalic)
		return FALSE;

	if (pLogFont1->lfQuality != pLogFont2->lfQuality)
		return FALSE;

	if (pLogFont1->lfEscapement != pLogFont2->lfEscapement)
		return FALSE;

	if (pLogFont1->lfOrientation != pLogFont2->lfOrientation)
		return FALSE;

	if (_tcscmp(pLogFont1->lfFaceName, pLogFont2->lfFaceName) != 0)
		return FALSE;

	return TRUE;
}

void CXTPMarkupContext::AddPendingUpdate(CXTPMarkupUIElement* pUIElement)
{
	ASSERT(NULL != pUIElement);
	if (NULL != pUIElement)
	{
		MARKUP_ADDREF(pUIElement);
		m_PendingUpdateList.AddTail(pUIElement);
	}
}

void CXTPMarkupContext::LoadScriptCodeChunk(CXTPMarkupBuilder* pBuilder, LPCWSTR lpszLanguage,
											LPCWSTR lpszCode)
{
	ASSERT(NULL != pBuilder);
	ASSERT(NULL != lpszLanguage);
	ASSERT(NULL != lpszCode);

	if (NULL == m_pScriptEngine)
	{
		// Initialize scripting engine.
		m_pScriptEngine = new CXTPActiveScriptEngine();
		HRESULT hr		= m_pScriptEngine->Initialize(lpszLanguage, &m_xActiveScriptSite);
		if (FAILED(hr))
		{
			ResetScriptEngine();
			pBuilder->ThrowBuilderException(hr);
		}

		// Add standard runtime extension.
		hr = m_pScriptEngine->AddNamedItem(L"__XTPActiveScriptStdRuntime", NULL,
										   SCRIPTITEM_GLOBALMEMBERS);
		if (FAILED(hr))
		{
			ResetScriptEngine();
			pBuilder->ThrowBuilderException(hr);
		}

		// Add custom clobal objects.
		CList<LPCWSTR, LPCWSTR> namedItems;
		GetScriptNamedItemNames(namedItems);
		POSITION posNamedItem = namedItems.GetHeadPosition();
		while (NULL != posNamedItem)
		{
			LPCWSTR lpszName = namedItems.GetNext(posNamedItem);
			hr				 = m_pScriptEngine->AddNamedItem(lpszName, NULL, SCRIPTITEM_ISVISIBLE);
			if (FAILED(hr))
			{
				ResetScriptEngine();
				pBuilder->ThrowBuilderException(hr);
			}
		}

		m_lpszScriptEngineLanguage = _wcsdup(lpszLanguage);
	}
	else if (0 != wcscmp(m_lpszScriptEngineLanguage, lpszLanguage))
	{
		CString message;
		message.Format(_T("'%ls' script engine is not allowed to use when '%ls' is already used."),
					   lpszLanguage, m_lpszScriptEngineLanguage);
		pBuilder->ThrowBuilderException(message);
	}

	m_ScriptCodeChunks.AddTail(_wcsdup(lpszCode));
}

void CXTPMarkupContext::RunScriptEngine(CXTPMarkupBuilder* pBuilder)
{
	ASSERT(NULL != pBuilder);

	if (NULL != m_pScriptEngine)
	{
		// Load code chunks.
		POSITION posChunk = m_ScriptCodeChunks.GetHeadPosition();
		while (NULL != posChunk)
		{
			LPCWSTR lpszChunk = m_ScriptCodeChunks.GetNext(posChunk);
			HRESULT hr		  = m_pScriptEngine->LoadCode(lpszChunk);
			if (FAILED(hr))
			{
				ThrowScriptEngineException(pBuilder, hr);
			}
		}

		// Run engine script.
		HRESULT hr = m_pScriptEngine->Run();
		if (FAILED(hr))
		{
			ThrowScriptEngineException(pBuilder, hr);
		}
	}
}

void CXTPMarkupContext::ResetScriptEngine()
{
	while (!m_ScriptCodeChunks.IsEmpty())
	{
		free(m_ScriptCodeChunks.RemoveTail());
	}

	if (NULL != m_lpszScriptEngineLanguage)
	{
		free(m_lpszScriptEngineLanguage);
		m_lpszScriptEngineLanguage = NULL;
	}

	if (NULL != m_pScriptEngine)
	{
		m_pScriptEngine->Uninitialize();
		CMDTARGET_RELEASE(m_pScriptEngine);
	}

	CMDTARGET_RELEASE(m_pScriptEngineStdRuntime);
}

void CXTPMarkupContext::RegisterScriptNamedObjects(CXTPMarkupBuilder* pBuilder,
												   CXTPMarkupObject* pObject)
{
	ASSERT(NULL != pBuilder);
	ASSERT(NULL != pObject);

	if (NULL != m_pScriptEngine)
	{
		LPCWSTR lpszName = pObject->GetName();
		if (NULL != lpszName)
		{
			HRESULT hr = m_pScriptEngine->AddNamedItem(lpszName, pObject);
			if (FAILED(hr))
			{
				pBuilder->ThrowBuilderException(hr);
			}
		}

		int nChildrenCount = pObject->GetLogicalChildrenCount();
		for (int i = 0; i < nChildrenCount; i++)
		{
			CXTPMarkupObject* pChildObject = pObject->GetLogicalChild(i);
			RegisterScriptNamedObjects(pBuilder, pChildObject);
		}
	}
}

void CXTPMarkupContext::GetScriptNamedItemNames(CList<LPCWSTR, LPCWSTR>& names)
{
	names.AddTail(L"MarkupContext");
}

HRESULT CXTPMarkupContext::GetScriptNamedItem(LPCWSTR lpszName, IUnknown** ppiunkItem)
{
	ASSERT(NULL != lpszName);
	ASSERT(NULL != ppiunkItem);

	HRESULT hr = E_NOINTERFACE;

	if (0 == wcscmp(lpszName, L"MarkupContext"))
	{
		hr = AccessInternalDispatch()->GetControllingUnknown()->QueryInterface(ppiunkItem);
	}

	return hr;
}

void CXTPMarkupContext::ThrowScriptEngineException(CXTPMarkupBuilder* pBuilder, HRESULT hr)
{
	ASSERT(NULL != pBuilder);
	ASSERT(FAILED(hr));
	ASSERT(NULL != m_pScriptEngine);

	const EXCEPINFO* pExcepInfo = m_pScriptEngine->GetExceptionInfo();
	if (NULL != pExcepInfo)
	{
		CString message;
		message.Format(_T("Script error [0x%08X]: %ls"),
					   (0 != pExcepInfo->wCode ? pExcepInfo->wCode : pExcepInfo->scode),
					   (NULL != pExcepInfo->bstrDescription ? pExcepInfo->bstrDescription
															: L"<unknown>"));
		pBuilder->ThrowBuilderException(message);
	}
	else
	{
		pBuilder->ThrowBuilderException(hr);
	}
}

CXTPMarkupFont* CXTPMarkupContext::GetFont(LOGFONT* pLogFont)
{
	if (m_pFonts == NULL)
		m_pFonts = new CXTPMarkupTypedSimpleStack<CXTPMarkupFont>();

	CXTPMarkupFont* pFont;

	for (pFont = m_pFonts->GetHead(); pFont != NULL; pFont = pFont->m_pNextChain)
	{
		if (NULL != pFont->m_Font.GetSafeHandle())
		{
			LOGFONT lf;
			pFont->m_Font.GetLogFont(&lf);
			if (CompareFont(&lf, pLogFont))
			{
				pFont->AddRef();
				return pFont;
			}
		}
	}

	pFont = new CXTPMarkupFont();
	m_pFonts->AddTail(pFont);
	pFont->m_Font.CreateFontIndirect(pLogFont);
	pFont->m_pMarkupContext = this;

	return pFont;
}

BOOL CXTPMarkupContext::IsVisualChild(CXTPMarkupObject* pParent, CXTPMarkupObject* pChild) const
{
	while (pChild)
	{
		if (pChild == pParent)
			return TRUE;

		pChild = pChild->GetLogicalParent();
	}
	return FALSE;
}

void CXTPMarkupContext::BuildInputList(CXTPMarkupObject* pUIElement,
									   CInputElementCollection* arrObjects)
{
	CXTPMarkupObject* pParent = pUIElement;
	while (pParent)
	{
		if (pParent->IsKindOf(MARKUP_TYPE(CXTPMarkupInputElement)))
		{
			arrObjects->Add((CXTPMarkupInputElement*)pParent);
			pParent->AddRef();
		}

		pParent = pParent->GetLogicalParent();
	}
}

INT_PTR CXTPMarkupContext::OnToolHitTest(CPoint /*point*/, TOOLINFO* pToolInfo)
{
	CInputElementCollection listMouseOver;
	BuildInputList(m_pMouseOver, &listMouseOver);

	for (int i = 0; i < listMouseOver.GetCount(); i++)
	{
		CXTPMarkupInputElement* pInputElement = listMouseOver.GetItem(i);

		CXTPMarkupObject* pToolTip = pInputElement->GetToolTip();
		if (pToolTip && IsStringObject(pToolTip))
		{
			CXTPToolTipContext::FillInToolInfo(pToolInfo, m_hContextWnd,
											   GetClientBoundingRect(pInputElement),
											   (INT_PTR)pInputElement,
											   (LPCWSTR) * ((CXTPMarkupString*)pToolTip));

			if (GetUseCustomToolTipPosition())
			{
				pToolInfo->rect.left = m_ptCustomToolTipPos.x;
				if (pToolInfo->cbSize == sizeof(XTP_TOOLTIP_TOOLINFO_EX))
				{
					RECT* pRC	= &(((XTP_TOOLTIP_TOOLINFO_EX*)pToolInfo)->pToolInfo->rcExclude);
					pRC->bottom = m_ptCustomToolTipPos.y;
					// ensure that rcExclude non empty
					pRC->top   = pRC->bottom - 10;
					pRC->right = m_ptCustomToolTipPos.x;
					pRC->left  = pRC->right - 10;
				}
			}

			return XTPToIntPtr(pToolInfo->uId);
		}
	}

	return -1;
}

void CXTPMarkupContext::HandleMouseEnter(CXTPMarkupInputElement* pMouseOver, CPoint point)
{
	if (m_pMouseOver == pMouseOver)
		return;

	if (!pMouseOver)
		m_pActiveElement = NULL;

	CInputElementCollection listOldMouseOver;
	CInputElementCollection listNewMouseOver;

	BuildInputList(m_pMouseOver, &listOldMouseOver);
	BuildInputList(pMouseOver, &listNewMouseOver);

	while (listOldMouseOver.GetCount() > 0 && listNewMouseOver.GetCount() > 0
		   && listOldMouseOver.GetTailItem() == listNewMouseOver.GetTailItem())
	{
		listNewMouseOver.Remove(listNewMouseOver.GetCount() - 1);
		listOldMouseOver.Remove(listOldMouseOver.GetCount() - 1);
	}

	CXTPMarkupMouseEventArgs* eMouseLeaveEventArgs = new CXTPMarkupMouseEventArgs(
		CXTPMarkupInputElement::m_pMouseLeaveEvent);
	CXTPMarkupMouseEventArgs* eMouseEnterEventArgs = new CXTPMarkupMouseEventArgs(
		CXTPMarkupInputElement::m_pMouseEnterEvent);

	eMouseLeaveEventArgs->SetSource(m_pMouseOver);
	eMouseLeaveEventArgs->m_hWnd  = m_hContextWnd;
	eMouseLeaveEventArgs->m_point = point;

	eMouseEnterEventArgs->SetSource(pMouseOver);
	eMouseEnterEventArgs->m_hWnd  = m_hContextWnd;
	eMouseEnterEventArgs->m_point = point;

	m_pMouseOver = pMouseOver;

	int i;
	for (i = 0; i < listOldMouseOver.GetCount(); i++)
	{
		CXTPMarkupInputElement* pInputElement = listOldMouseOver.GetItem(i);

		pInputElement->RaiseEvent(eMouseLeaveEventArgs);
		if (eMouseLeaveEventArgs->IsHandled())
			break;

		pInputElement->OnMouseLeave(eMouseLeaveEventArgs);
		if (eMouseLeaveEventArgs->IsHandled())
			break;
	}

	for (i = 0; i < listNewMouseOver.GetCount(); i++)
	{
		CXTPMarkupInputElement* pInputElement = listNewMouseOver.GetItem(i);

		pInputElement->RaiseEvent(eMouseEnterEventArgs);
		if (eMouseEnterEventArgs->IsHandled())
			break;

		pInputElement->OnMouseEnter(eMouseEnterEventArgs);
		if (eMouseEnterEventArgs->IsHandled())
			break;
	}

	eMouseLeaveEventArgs->Release();
	eMouseEnterEventArgs->Release();
}

void CXTPMarkupContext::HandleMouseMove(CXTPMarkupUIElement* pUIElement, CPoint point)
{
	CXTPMarkupInputElement* pObject = pUIElement->InputHitTest(point);
	if (m_pMouseCapture)
	{
		if ((pObject == NULL) || !IsVisualChild(m_pMouseCapture, pObject))
			pObject = NULL;
		else
			pObject = m_pMouseCapture;
	}

	if (m_pMouseOver != pObject)
	{
		HandleMouseEnter(pObject, point);

		if (m_pMouseOver)
		{
			TRACKMOUSEEVENT tme = { sizeof(TRACKMOUSEEVENT), TME_LEAVE, m_hContextWnd, 0 };
			_TrackMouseEvent(&tme);
		}

		if (!HandleSetCursor())
		{
			SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
		}
	}

	m_pActiveElement = m_pMouseOver ? pUIElement : NULL;

	if (m_pMouseOver)
	{
		CInputElementCollection listNewMouseOver;
		BuildInputList(m_pMouseOver, &listNewMouseOver);

		CXTPMarkupMouseEventArgs* eMouseMoveEventArgs = new CXTPMarkupMouseEventArgs(
			CXTPMarkupInputElement::m_pMouseMoveEvent);
		eMouseMoveEventArgs->SetSource(m_pMouseOver);
		eMouseMoveEventArgs->m_hWnd	 = m_hContextWnd;
		eMouseMoveEventArgs->m_point = point;

		for (int i = 0; i < listNewMouseOver.GetCount(); i++)
		{
			CXTPMarkupInputElement* pInputElement = listNewMouseOver.GetItem(i);

			pInputElement->RaiseEvent(eMouseMoveEventArgs);
			if (eMouseMoveEventArgs->IsHandled())
				break;

			pInputElement->OnMouseMove(eMouseMoveEventArgs);
			if (eMouseMoveEventArgs->IsHandled())
				break;
		}

		eMouseMoveEventArgs->Release();
	}
}

BOOL CXTPMarkupContext::HandleSetCursor()
{
	if (m_pMouseOver)
	{
		CInputElementCollection listNewMouseOver;
		BuildInputList(m_pMouseOver, &listNewMouseOver);

		CXTPMarkupQueryCursorEventArgs eQueryCursorEventArgs;
		eQueryCursorEventArgs.SetSource(m_pMouseOver);

		for (int i = 0; i < listNewMouseOver.GetCount(); i++)
		{
			CXTPMarkupInputElement* pInputElement = listNewMouseOver.GetItem(i);

			pInputElement->OnQueryCursor(&eQueryCursorEventArgs);
			if (!eQueryCursorEventArgs.IsHandled())
				continue;

			if (eQueryCursorEventArgs.m_hCursor)
			{
				::SetCursor(eQueryCursorEventArgs.m_hCursor);
				return TRUE;
			}

			return FALSE;
		}
	}
	return FALSE;
}

BOOL CXTPMarkupContext::HandleMouseWheel(int nDelta)
{
	if (m_pMouseOver)
	{
		CInputElementCollection listNewMouseOver;
		BuildInputList(m_pMouseOver, &listNewMouseOver);

		CXTPMarkupMouseWheelEventArgs eMouseWheelEventArgs;
		eMouseWheelEventArgs.m_nDelta = nDelta;
		eMouseWheelEventArgs.SetSource(m_pMouseOver);

		for (int i = 0; i < listNewMouseOver.GetCount(); i++)
		{
			CXTPMarkupInputElement* pInputElement = listNewMouseOver.GetItem(i);

			pInputElement->OnMouseWheel(&eMouseWheelEventArgs);
			if (eMouseWheelEventArgs.IsHandled())
				return TRUE;
		}
	}
	return FALSE;
}

BOOL CXTPMarkupContext::HandleMouseUpDown(UINT message, WPARAM /*wParam*/, LPARAM lParam)
{
	CXTPMarkupInputElement* pMouseOver = m_pMouseOver;
	if (m_pMouseCapture)
		pMouseOver = m_pMouseCapture;

	if (!pMouseOver)
		return FALSE;

	CInputElementCollection listNewMouseOver;
	BuildInputList(pMouseOver, &listNewMouseOver);

	CXTPMarkupMouseButtonEventArgs* eMouseButtonEventArgs = new CXTPMarkupMouseButtonEventArgs(
		message == WM_LBUTTONDOWN || message == WM_LBUTTONDBLCLK
			? CXTPMarkupInputElement::m_pMouseLeftButtonDownEvent
		: message == WM_LBUTTONUP	? CXTPMarkupInputElement::m_pMouseLeftButtonUpEvent
		: message == WM_RBUTTONDOWN ? CXTPMarkupInputElement::m_pMouseRightButtonDownEvent
		: message == WM_RBUTTONUP	? CXTPMarkupInputElement::m_pMouseRightButtonUpEvent
									: NULL);

	eMouseButtonEventArgs->SetSource(pMouseOver);
	eMouseButtonEventArgs->m_hWnd  = m_hContextWnd;
	eMouseButtonEventArgs->m_point = lParam;

	for (int i = 0; i < listNewMouseOver.GetCount(); i++)
	{
		CXTPMarkupInputElement* pInputElement = listNewMouseOver.GetItem(i);

		pInputElement->RaiseEvent(eMouseButtonEventArgs);
		if (eMouseButtonEventArgs->IsHandled())
			break;

		if (message == WM_LBUTTONDOWN || message == WM_LBUTTONDBLCLK)
		{
			pInputElement->OnMouseLeftButtonDown(eMouseButtonEventArgs);
		}
		else if (message == WM_LBUTTONUP)
		{
			pInputElement->OnMouseLeftButtonUp(eMouseButtonEventArgs);
		}
		else if (message == WM_RBUTTONDOWN)
		{
			pInputElement->OnMouseRightButtonDown(eMouseButtonEventArgs);
		}
		else if (message == WM_RBUTTONUP)
		{
			pInputElement->OnMouseRightButtonUp(eMouseButtonEventArgs);
		}

		if (eMouseButtonEventArgs->IsHandled())
			break;
	}

	BOOL bHandled = eMouseButtonEventArgs->IsHandled();

	eMouseButtonEventArgs->Release();

	return bHandled;
}

void CXTPMarkupContext::CaptureMouse(CXTPMarkupInputElement* pUIElement)
{
	if (m_pMouseCapture == pUIElement)
		return;

	::SetCapture(m_hContextWnd);
	m_pMouseCapture = pUIElement;
}

void CXTPMarkupContext::ReleaseMouseCapture(CXTPMarkupInputElement* /*pUIElement*/)
{
	m_pMouseCapture = NULL;
	::ReleaseCapture();
}

BOOL CXTPMarkupContext::OnWndMsg(CXTPMarkupUIElement* pUIElement, UINT message, WPARAM wParam,
								 LPARAM lParam, LRESULT* pResult)
{
	if (!m_hContextWnd)
		return FALSE;

	if (!pUIElement)
	{
		if (m_pActiveElement)
		{
			HandleMouseEnter(NULL, CPoint(-1, -1));
		}
		return FALSE;
	}

	if (message == WM_MOUSEMOVE)
	{
		HandleMouseMove(pUIElement, CPoint(lParam));
	}

	if (message == WM_DESTROY || message == WM_MOUSELEAVE)
	{
		HandleMouseEnter(NULL, CPoint(-1, -1));
	}

	if ((message >= WM_MOUSEFIRST && message <= WM_MOUSELAST) || (message == WM_MOUSELEAVE))
	{
		m_pToolTipContext->FilterToolTipMessage(CWnd::FromHandle(m_hContextWnd), message, wParam,
												lParam);
	}

	if (message == WM_SETCURSOR)
	{
		if (HandleSetCursor())
		{
			*pResult = TRUE;
			return TRUE;
		}
	}

	if (message == WM_MOUSEWHEEL)
	{
		if (HandleMouseWheel((short)HIWORD(wParam)))
		{
			*pResult = TRUE;
			return TRUE;
		}
	}

	if (message == WM_LBUTTONDOWN || message == WM_RBUTTONDOWN || message == WM_LBUTTONUP
		|| message == WM_RBUTTONUP || message == WM_LBUTTONDBLCLK)
	{
		if (HandleMouseUpDown(message, wParam, lParam))
			return TRUE;
	}

	if (message == WM_CAPTURECHANGED && m_pMouseCapture)
	{
		CXTPMarkupMouseEventArgs* eLostMouseCaptureEventArgs = new CXTPMarkupMouseEventArgs(
			CXTPMarkupInputElement::m_pLostMouseCaptureEvent);
		eLostMouseCaptureEventArgs->SetSource(m_pMouseCapture);

		CXTPMarkupInputElement* pInputElement = m_pMouseCapture;

		pInputElement->RaiseEvent(eLostMouseCaptureEventArgs);

		if (!eLostMouseCaptureEventArgs->IsHandled())
		{
			pInputElement->OnLostMouseCapture(eLostMouseCaptureEventArgs);
		}
		eLostMouseCaptureEventArgs->Release();

		m_pMouseCapture = NULL;
	}

	if (message == WM_KEYDOWN || message == WM_SETFOCUS || message == WM_KILLFOCUS)
	{
		if (m_pKeyboardNavigation->OnWndMsg(message, wParam, lParam, pResult))
			return TRUE;
	}

	return FALSE;
}

void CXTPMarkupContext::RaiseEvent(CXTPMarkupObject* pSource, CXTPMarkupRoutedEventArgs* pEventArgs)
{
	if (m_pHandlers)
		m_pHandlers->Raise(pSource, pEventArgs);
}

void CXTPMarkupContext::AddHandler(CXTPMarkupRoutedEvent* pEvent, CXTPMarkupDelegate* pDelegate)
{
	if (m_pHandlers == NULL)
		m_pHandlers = new CXTPMarkupEventHandlerMap();

	m_pHandlers->Add(pEvent, pDelegate);
}

void CXTPMarkupContext::SetDelegate(LPWSTR lpszHandler, CXTPMarkupDelegate* pDelegate)
{
	if (m_pDelegates == NULL)
		m_pDelegates = new CXTPMarkupDelegateMap();

	m_pDelegates->Add(lpszHandler, pDelegate);
}

CXTPMarkupDelegate* CXTPMarkupContext::LookupDelegate(LPCWSTR lpszDelegate) const
{
	if (m_pDelegates == NULL)
		return NULL;

	CXTPMarkupDelegate* pDelegate = m_pDelegates->Lookup(lpszDelegate);

	if (pDelegate)
		return pDelegate;

	int nLen = (int)wcslen(lpszDelegate);

	LPWSTR lpMethodName = new WCHAR[XTPToUIntChecked(nLen + 1)];
	memcpy(lpMethodName, lpszDelegate, (nLen + 1) * sizeof(WCHAR));

	pDelegate = new CXTPMarkupOleAutoDelegate(lpMethodName, m_pDelegates);

	m_pDelegates->Add(lpMethodName, pDelegate);

	return pDelegate;
}

CRect CXTPMarkupContext::GetClientBoundingRect(CXTPMarkupInputElement* pInputElement) const
{
	CRect rc(0, 0, 0, 0);

	if (!pInputElement)
		return rc;

	CXTPMarkupVisual* pVisual = NULL;
	if (pInputElement->IsKindOf(MARKUP_TYPE(CXTPMarkupVisual)))
	{
		pVisual = (CXTPMarkupVisual*)pInputElement;
		rc		= pVisual->GetBoundRect();
	}
	else if (pInputElement->IsKindOf(MARKUP_TYPE(CXTPMarkupFrameworkContentElement)))
	{
		pVisual = ((CXTPMarkupFrameworkContentElement*)pInputElement)->GetParent();
		rc		= ((CXTPMarkupFrameworkContentElement*)pInputElement)->GetBoundRect();
	}

	if (!pVisual)
		return rc;

	while (pVisual != 0)
	{
		rc.OffsetRect(pVisual->GetVisualOffset());
		pVisual = pVisual->GetVisualParent();
	}

	return rc;
}

CRect CXTPMarkupContext::GetUpdateRect(CXTPMarkupInputElement* pInputElement) const
{
	CRect rc(0, 0, 0, 0);

	if (!pInputElement)
		return rc;

	CXTPMarkupVisual* pVisual = NULL;
	if (pInputElement->IsKindOf(MARKUP_TYPE(CXTPMarkupVisual)))
	{
		pVisual = (CXTPMarkupVisual*)pInputElement;
		rc		= pVisual->GetUpdateRect();
	}
	else if (pInputElement->IsKindOf(MARKUP_TYPE(CXTPMarkupFrameworkContentElement)))
	{
		pVisual = ((CXTPMarkupFrameworkContentElement*)pInputElement)->GetParent();
		rc		= ((CXTPMarkupFrameworkContentElement*)pInputElement)->GetUpdateRect();
	}

	if (!pVisual)
		return rc;

	while (pVisual != 0)
	{
		rc.OffsetRect(pVisual->GetVisualOffset());
		pVisual = pVisual->GetVisualParent();
	}

	return rc;
}

void CXTPMarkupContext::OnInvalidateVisual(CXTPMarkupUIElement* pUIElement)
{
	if (m_hContextWnd)
	{
		if (!m_bUpdatesFrozen)
		{
			CRect rcUpdate = GetUpdateRect(pUIElement);
			InvalidateRect(m_hContextWnd, rcUpdate, FALSE);
			::SendMessage(m_hContextWnd, XTP_WM_MARKUPREDRAW, 0, 0);
		}
		else
		{
			AddPendingUpdate(pUIElement);
		}
	}
}

void CXTPMarkupContext::OnInvalidateArrange(CXTPMarkupUIElement* /*pUIElement*/)
{
	if (m_hContextWnd)
	{
		if (!m_bUpdatesFrozen)
		{
			InvalidateRect(m_hContextWnd, NULL, FALSE);
			::SendMessage(m_hContextWnd, XTP_WM_MARKUPREDRAW, 0, 0);
		}
		else
		{
			m_bEntireUpdatePended = TRUE;
		}
	}
}

void CXTPMarkupContext::AddRef()
{
	InterlockedIncrement(&m_dwRef);
}

void CXTPMarkupContext::Release()
{
#ifdef _XTP_ACTIVEX
	InternalRelease();
#else
	if (m_dwRef == 0)
		return;

	LONG lResult = InterlockedDecrement(&m_dwRef);
	if (lResult == 0)
	{
		delete this;
	}
#endif
}

void CXTPMarkupContext::SetImageManager(CXTPImageManager* pImageManager)
{
	CMDTARGET_RELEASE(m_pImageManager);

	m_pImageManager = pImageManager;
}

CXTPImageManager* CXTPMarkupContext::GetImageManager() const
{
	if (m_pImageManager)
		return m_pImageManager;

	return XTPImageManager();
}

COLORREF CXTPMarkupContext::GetImageColorKey() const
{
	COLORREF clr = m_clrImgColorKey;
	if (COLORREF_NULL == clr)
	{
		clr = GetImageManager()->GetMaskColor();
		if (COLORREF_NULL == clr)
		{
			// Use a default one for backward compatibility.
			clr = RGB(0, 0xff, 0);
		}
	}

	return clr;
}

#ifdef _XTP_ACTIVEX

IMPLEMENT_OLETYPELIB_EX(CXTPMarkupContext, XTPDIID_MarkupContext)

BEGIN_INTERFACE_MAP(CXTPMarkupContext, CXTPCmdTarget)
	INTERFACE_PART(CXTPMarkupContext, IID_IActiveScriptSite, ActiveScriptSite)
	INTERFACE_PART(CXTPMarkupContext, XTPDIID_MarkupContext, Dispatch)
END_INTERFACE_MAP()
#endif

#ifdef _XTP_ACTIVEX

XTP_IMPLEMENT_IUNKNOWN(CXTPMarkupContext, ActiveScriptSite)

#else /*!_XTP_ACTIVEX*/

STDMETHODIMP_(ULONG) CXTPMarkupContext::XActiveScriptSite::AddRef()
{
	pThis->AddRef();
	return XTPToULongChecked(pThis->m_dwRef);
}

STDMETHODIMP_(ULONG) CXTPMarkupContext::XActiveScriptSite::Release()
{
	DWORD dwRefs = XTPToDWORDChecked(pThis->m_dwRef);
	pThis->Release();
	return dwRefs - 1;
}

STDMETHODIMP CXTPMarkupContext::XActiveScriptSite::QueryInterface(REFIID riid, void** ppvObj)
{
	HRESULT hr = E_NOINTERFACE;
	if (NULL != ppvObj)
	{
		*ppvObj = NULL;
		if (IID_IActiveScriptSite == riid || IID_IUnknown == riid)
		{
			*ppvObj = pThis;
			AddRef();
			hr = S_OK;
		}
	}
	else
	{
		hr = E_POINTER;
	}

	return hr;
}

#endif /*!_XTP_ACTIVEX*/

STDMETHODIMP CXTPMarkupContext::XActiveScriptSite::GetLCID(LCID* plcid)
{
	UNREFERENCED_PARAMETER(plcid);
	return E_NOTIMPL;
}

STDMETHODIMP CXTPMarkupContext::XActiveScriptSite::GetItemInfo(LPCOLESTR pstrName,
															   DWORD dwReturnMask,
															   IUnknown** ppiunkItem,
															   ITypeInfo** ppti)
{
#ifdef _XTP_ACTIVEX
	METHOD_PROLOGUE(CXTPMarkupContext, ActiveScriptSite);
#endif

	HRESULT hr = TYPE_E_ELEMENTNOTFOUND;

	if (0 != (dwReturnMask & SCRIPTINFO_IUNKNOWN))
	{
		if (NULL != ppiunkItem)
		{
			*ppiunkItem = NULL;

			// Check if standard runtime instance is requested.
			if (0 == wcscmp(pstrName, L"__XTPActiveScriptStdRuntime"))
			{
				if (NULL == pThis->m_pScriptEngineStdRuntime)
				{
					pThis->m_pScriptEngineStdRuntime = new CXTPActiveScriptStdRuntime();
				}

				hr = pThis->m_pScriptEngineStdRuntime->GetControllingUnknown()->QueryInterface(
					ppiunkItem);
			}
			else
			{
				// If nothing found, try custom global objects.
				hr = pThis->GetScriptNamedItem(pstrName, ppiunkItem);
			}

			// If nothing found, forward to script engine.
			if (NULL == *ppiunkItem)
			{
				LPVOID lpItemData = pThis->m_pScriptEngine->GetNamedItemData(pstrName);
				if (NULL != lpItemData)
				{
					CXTPMarkupObject* pObject = reinterpret_cast<CXTPMarkupObject*>(lpItemData);
					hr = pObject->GetControllingUnknown()->QueryInterface(ppiunkItem);
				}
			}
		}
		else
		{
			hr = E_POINTER;
		}
	}

	if (0 != (dwReturnMask & SCRIPTINFO_ITYPEINFO) && NULL != ppti)
	{
		*ppti = NULL;
	}

	return hr;
}

STDMETHODIMP CXTPMarkupContext::XActiveScriptSite::GetDocVersionString(BSTR* pbstrVersion)
{
	UNREFERENCED_PARAMETER(pbstrVersion);
	return E_NOTIMPL;
}

STDMETHODIMP CXTPMarkupContext::XActiveScriptSite::OnScriptTerminate(const VARIANT* pvarResult,
																	 const EXCEPINFO* pexcepinfo)
{
	UNREFERENCED_PARAMETER(pvarResult);
	UNREFERENCED_PARAMETER(pexcepinfo);
	return S_OK;
}

STDMETHODIMP CXTPMarkupContext::XActiveScriptSite::OnStateChange(SCRIPTSTATE ssScriptState)
{
	UNREFERENCED_PARAMETER(ssScriptState);
	return S_OK;
}

STDMETHODIMP CXTPMarkupContext::XActiveScriptSite::OnScriptError(IActiveScriptError* pScriptError)
{
	UNREFERENCED_PARAMETER(pScriptError);
	return E_NOTIMPL;
}

STDMETHODIMP CXTPMarkupContext::XActiveScriptSite::OnEnterScript()
{
	return S_OK;
}

STDMETHODIMP CXTPMarkupContext::XActiveScriptSite::OnLeaveScript()
{
	return S_OK;
}

#ifdef _XTP_ACTIVEX

BEGIN_DISPATCH_MAP(CXTPMarkupContext, CXTPCmdTarget)
	DISP_FUNCTION_ID(CXTPMarkupContext, "SetMethod", 1, OleSetMethod, VT_EMPTY,
					 VTS_DISPATCH VTS_WBSTR)
	DISP_FUNCTION_ID(CXTPMarkupContext, "SetHandler", 6, OleSetHandler, VT_EMPTY, VTS_DISPATCH)
	DISP_FUNCTION_ID(CXTPMarkupContext, "CreateObject", 2, OleCreateObject, VT_DISPATCH, VTS_WBSTR)
	DISP_FUNCTION_ID(CXTPMarkupContext, "CreateSolidBrush", 3, OleCreateSolidBrush, VT_DISPATCH,
					 VTS_COLOR)
	DISP_FUNCTION_ID(CXTPMarkupContext, "CreateThickness", 4, OleCreateThickness, VT_DISPATCH,
					 VTS_I4 VTS_I4 VTS_I4 VTS_I4)
	DISP_FUNCTION_ID(CXTPMarkupContext, "Parse", 5, OleParse, VT_DISPATCH, VTS_BSTR)
	DISP_PROPERTY_ID(CXTPMarkupContext, "EnableGdiPlus", 7, m_bEnableGdiPlus, VT_BOOL)
	DISP_PROPERTY_EX_ID(CXTPMarkupContext, "DpiAware", 43, OleIsDpiAware, OleSetDpiAware, VT_BOOL)
	DISP_PROPERTY_EX_ID(CXTPMarkupContext, "ToolTipContext", 39, OleGetToolTipContext,
						SetNotSupported, VT_DISPATCH)
	DISP_PROPERTY_EX_ID(CXTPMarkupContext, "Icons", 40, OleGetIcons, OleSetIcons, VT_DISPATCH)
	DISP_FUNCTION_ID(CXTPMarkupContext, "ValidateXML", 8, OleValidateXML, VT_BSTR, VTS_BSTR)
	DISP_FUNCTION_ID(CXTPMarkupContext, "FreezeUpdates", 41, OleFreezeUpdates, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION_ID(CXTPMarkupContext, "UnfreezeUpdates", 42, OleUnfreezeUpdates, VT_EMPTY,
					 VTS_NONE)
	DISP_PROPERTY_EX_ID(CXTPMarkupContext, "DefaultSmoothingMode", 44, OleGetDefaultSmoothingMode,
						OleSetDefaultSmoothingMode, VT_I4)
	DISP_PROPERTY_EX_ID(CXTPMarkupContext, "DefaultTextSmoothingEnabled", 45,
						OleGetDefaultTextSmoothing, OleSetDefaultTextSmoothing, VT_BOOL)
END_DISPATCH_MAP()

#endif /*!_XTP_ACTIVEX*/

class CXTPMarkupOleDelegate : public CXTPMarkupDelegate
{
public:
	CXTPMarkupOleDelegate(LPDISPATCH lpHandler, LPWSTR lpName)
	{
		m_lpName	= lpName;
		m_lpHandler = lpHandler;
	}

	~CXTPMarkupOleDelegate()
	{
		delete[] m_lpName;
	}

	virtual void Execute(CXTPMarkupObject* pSender, CXTPMarkupRoutedEventArgs* pArgs)
	{
		COleDispatchDriver dispDriverBinded(m_lpHandler, FALSE);

		OLECHAR* szMember = (OLECHAR*)m_lpName;

		DISPID dispidBinded = 0;

		if (FAILED(m_lpHandler->GetIDsOfNames(IID_NULL, &szMember, 1, LOCALE_SYSTEM_DEFAULT,
											  &dispidBinded)))
			return;

		static BYTE parms[] = VTS_DISPATCH VTS_DISPATCH;
		dispDriverBinded.InvokeHelper(dispidBinded, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
									  XTPGetDispatch(pSender, FALSE), XTPGetDispatch(pArgs, FALSE));
	}

protected:
	LPDISPATCH m_lpHandler;
	LPWSTR m_lpName;
};

void CXTPMarkupOleAutoDelegate::Execute(CXTPMarkupObject* pSender, CXTPMarkupRoutedEventArgs* pArgs)
{
	if (!m_pMap->m_lpHandler)
		return;

	COleDispatchDriver dispDriverBinded(m_pMap->m_lpHandler, FALSE);

	OLECHAR* szMember = (OLECHAR*)m_lpName;

	DISPID dispidBinded = 0;

	if (FAILED(m_pMap->m_lpHandler->GetIDsOfNames(IID_NULL, &szMember, 1, LOCALE_SYSTEM_DEFAULT,
												  &dispidBinded)))
		return;

	static BYTE parms[] = VTS_DISPATCH VTS_DISPATCH;
	dispDriverBinded.InvokeHelper(dispidBinded, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
								  XTPGetDispatch(pSender, FALSE), XTPGetDispatch(pArgs, FALSE));
}

void CXTPMarkupInputElement::OleAddHandler(LPDISPATCH lpEventDisp, LPCOLESTR lpszDelegate)
{
	CXTPMarkupRoutedEvent* pEvent = (CXTPMarkupRoutedEvent*)FromDispatch(lpEventDisp, FALSE);
	if (!pEvent)
		return;

	int nLen = (int)wcslen(lpszDelegate);

	LPWSTR lpMethodName = new WCHAR[XTPToUIntChecked(nLen + 1)];
	memcpy(lpMethodName, lpszDelegate, (nLen + 1) * sizeof(WCHAR));

	AddHandler(pEvent,
			   new CXTPMarkupOleAutoDelegate(lpMethodName, GetMarkupContext()->m_pDelegates));
}

#ifndef _XTP_ACTIVEX

///////////////////////////////////////////////////////////////////////////////
// OLE methods implementation

class CXTPMarkupContextInternalDispatch : public CXTPCmdTarget
{
	DECLARE_DYNAMIC(CXTPMarkupContextInternalDispatch);

public:
	explicit CXTPMarkupContextInternalDispatch(CXTPMarkupContext* pContext)
		: m_pContext(pContext)
	{
		ASSERT(NULL != m_pContext);
		EnableAutomation();
	}

private:
	DECLARE_DISPATCH_MAP();

	void OleSetMethod(LPDISPATCH lpHandler, LPCOLESTR lpName)
	{
		m_pContext->OleSetMethod(lpHandler, lpName);
	}

	void OleSetHandler(LPDISPATCH lpHandler)
	{
		m_pContext->OleSetHandler(lpHandler);
	}

	LPDISPATCH OleCreateObject(LPCOLESTR lpName)
	{
		return m_pContext->OleCreateObject(lpName);
	}

	LPDISPATCH OleCreateSolidBrush(COLORREF clr)
	{
		return m_pContext->OleCreateSolidBrush(clr);
	}

	LPDISPATCH OleCreateThickness(long nLeft, long nTop, long nRight, long nBottom)
	{
		return m_pContext->OleCreateThickness(nLeft, nTop, nRight, nBottom);
	}

	LPDISPATCH OleParse(LPCTSTR lpText)
	{
		return m_pContext->OleParse(lpText);
	}

	LPDISPATCH OleGetToolTipContext()
	{
		return m_pContext->OleGetToolTipContext();
	}

	void OleSetIcons(LPDISPATCH lpDisp)
	{
		m_pContext->OleSetIcons(lpDisp);
	}

	LPDISPATCH OleGetIcons()
	{
		return m_pContext->OleGetIcons();
	}

	BSTR OleValidateXML(LPCTSTR lpText)
	{
		return m_pContext->OleValidateXML(lpText);
	}

	void OleSetEnableGdiPlus(BOOL bEnable)
	{
		m_pContext->m_bEnableGdiPlus = bEnable;
	}

	BOOL OleGetEnableGdiPlus()
	{
		return m_pContext->m_bEnableGdiPlus;
	}

	void OleFreezeUpdates()
	{
		m_pContext->OleFreezeUpdates();
	}

	void OleUnfreezeUpdates()
	{
		m_pContext->OleUnfreezeUpdates();
	}

	long OleGetDefaultSmoothingMode()
	{
		return m_pContext->OleGetDefaultSmoothingMode();
	}

	void OleSetDefaultSmoothingMode(long lMode)
	{
		m_pContext->OleSetDefaultSmoothingMode(lMode);
	}

	BOOL OleGetDefaultTextSmoothing()
	{
		return m_pContext->OleGetDefaultTextSmoothing();
	}

	void OleSetDefaultTextSmoothing(BOOL bEnableTextSmoothing)
	{
		m_pContext->OleSetDefaultTextSmoothing(bEnableTextSmoothing);
	}

	BOOL OleIsDpiAware()
	{
		return m_pContext->OleIsDpiAware();
	}

	void OleSetDpiAware(BOOL bNewValue)
	{
		m_pContext->OleSetDpiAware(bNewValue);
	}

private:
	CXTPMarkupContext* m_pContext;
};

IMPLEMENT_DYNAMIC(CXTPMarkupContextInternalDispatch, CXTPCmdTarget);

#	include "Common/Base/Diagnostic/XTPBeginAfxMap.h"
BEGIN_DISPATCH_MAP(CXTPMarkupContextInternalDispatch, CXTPCmdTarget)
	DISP_FUNCTION_ID(CXTPMarkupContextInternalDispatch, "SetMethod", 1, OleSetMethod, VT_EMPTY,
					 VTS_DISPATCH VTS_WBSTR)
	DISP_FUNCTION_ID(CXTPMarkupContextInternalDispatch, "SetHandler", 6, OleSetHandler, VT_EMPTY,
					 VTS_DISPATCH)
	DISP_FUNCTION_ID(CXTPMarkupContextInternalDispatch, "CreateObject", 2, OleCreateObject,
					 VT_DISPATCH, VTS_WBSTR)
	DISP_FUNCTION_ID(CXTPMarkupContextInternalDispatch, "CreateSolidBrush", 3, OleCreateSolidBrush,
					 VT_DISPATCH, VTS_COLOR)
	DISP_FUNCTION_ID(CXTPMarkupContextInternalDispatch, "CreateThickness", 4, OleCreateThickness,
					 VT_DISPATCH, VTS_I4 VTS_I4 VTS_I4 VTS_I4)
	DISP_FUNCTION_ID(CXTPMarkupContextInternalDispatch, "Parse", 5, OleParse, VT_DISPATCH,
					 VTS_WBSTR)
	DISP_PROPERTY_EX_ID(CXTPMarkupContextInternalDispatch, "EnableGdiPlus", 7, OleGetEnableGdiPlus,
						OleSetEnableGdiPlus, VT_BOOL)
	DISP_PROPERTY_EX_ID(CXTPMarkupContextInternalDispatch, "DpiAware", 43, OleIsDpiAware,
						OleSetDpiAware, VT_BOOL)
	DISP_PROPERTY_EX_ID(CXTPMarkupContextInternalDispatch, "ToolTipContext", 39,
						OleGetToolTipContext, SetNotSupported, VT_DISPATCH)
	DISP_PROPERTY_EX_ID(CXTPMarkupContextInternalDispatch, "Icons", 40, OleGetIcons, OleSetIcons,
						VT_DISPATCH)
	DISP_FUNCTION_ID(CXTPMarkupContextInternalDispatch, "ValidateXML", 8, OleValidateXML, VT_BSTR,
					 VTS_WBSTR)
	DISP_FUNCTION_ID(CXTPMarkupContextInternalDispatch, "FreezeUpdates", 41, OleFreezeUpdates,
					 VT_EMPTY, VTS_NONE)
	DISP_FUNCTION_ID(CXTPMarkupContextInternalDispatch, "UnfreezeUpdates", 42, OleUnfreezeUpdates,
					 VT_EMPTY, VTS_NONE)
	DISP_PROPERTY_EX_ID(CXTPMarkupContextInternalDispatch, "DefaultSmoothingMode", 44,
						OleGetDefaultSmoothingMode, OleSetDefaultSmoothingMode, VT_I4)
	DISP_PROPERTY_EX_ID(CXTPMarkupContextInternalDispatch, "DefaultTextSmoothingEnabled", 45,
						OleGetDefaultTextSmoothing, OleSetDefaultTextSmoothing, VT_BOOL)
END_DISPATCH_MAP()
#	include "Common/Base/Diagnostic/XTPEndAfxMap.h"

#endif // !_XTP_ACTIVEX

CCmdTarget* CXTPMarkupContext::AccessInternalDispatch()
{
	CCmdTarget* pDispatcher;

#ifdef _XTP_ACTIVEX
	pDispatcher = this;
#else
	if (NULL == m_pInternalDispatch)
	{
		m_pInternalDispatch = new CXTPMarkupContextInternalDispatch(this);
	}

	pDispatcher = m_pInternalDispatch;
#endif

	return pDispatcher;
}

void CXTPMarkupContext::OleSetMethod(LPDISPATCH lpHandler, LPCOLESTR lpName)
{
	if (!lpName || !lpHandler)
		return;

	int nLen = (int)wcslen(lpName);

	LPWSTR lpMethodName = new WCHAR[XTPToUIntChecked(nLen + 1)];
	memcpy(lpMethodName, lpName, (nLen + 1) * sizeof(WCHAR));

	SetDelegate(lpMethodName, new CXTPMarkupOleDelegate(lpHandler, lpMethodName));
}

void CXTPMarkupContext::OleSetHandler(LPDISPATCH lpHandler)
{
	if (m_pDelegates == NULL)
		m_pDelegates = new CXTPMarkupDelegateMap();

	if (m_pDelegates->m_lpHandler != NULL)
	{
		m_pDelegates->m_lpHandler = NULL;
	}

	if (lpHandler)
	{
		m_pDelegates->m_lpHandler = lpHandler;
	}
}

LPDISPATCH CXTPMarkupContext::OleCreateObject(LPCOLESTR lpName)
{
	CXTPMarkupType* pType = CXTPMarkupType::LookupTag(lpName);
	if (!pType)
		return NULL;

	return XTPGetDispatch(CreateMarkupObject(pType), FALSE);
}

LPDISPATCH CXTPMarkupContext::OleCreateSolidBrush(COLORREF clr)
{
	BYTE nAlpha = static_cast<BYTE>(clr >> 24);
	if (nAlpha == 0)
		nAlpha = 0xFF;

	return XTPGetDispatch(new CXTPMarkupSolidColorBrush(nAlpha, clr), FALSE);
}

LPDISPATCH CXTPMarkupContext::OleCreateThickness(long nLeft, long nTop, long nRight, long nBottom)
{
	return XTPGetDispatch(new CXTPMarkupThickness(nLeft, nTop, nRight, nBottom), FALSE);
}

LPDISPATCH CXTPMarkupContext::OleParse(LPCTSTR lpText)
{
	return XTPGetDispatch(Parse(lpText), FALSE);
}

BSTR CXTPMarkupContext::OleValidateXML(LPCTSTR lpText)
{
	return ValidateXML(lpText).AllocSysString();
}

BOOL CXTPMarkupContext::OleIsDpiAware()
{
	return IsDpiAware();
}

void CXTPMarkupContext::OleSetDpiAware(BOOL bNewValue)
{
	SetDpiAware(bNewValue);
}

LPDISPATCH CXTPMarkupContext::OleGetToolTipContext()
{
	return GetToolTipContext()->GetIDispatch(TRUE);
}

LPDISPATCH CXTPMarkupContext::OleGetIcons()
{
	return XTPGetDispatch(GetImageManager());
}

void CXTPMarkupContext::OleSetIcons(LPDISPATCH lpDispatch)
{
	CMDTARGET_RELEASE(m_pImageManager);

	if (lpDispatch)
	{
		m_pImageManager = CXTPImageManager::FromDispatch(lpDispatch);
		lpDispatch->AddRef();
	}
}

void CXTPMarkupContext::OleFreezeUpdates()
{
	m_bUpdatesFrozen = TRUE;
}

void CXTPMarkupContext::OleUnfreezeUpdates()
{
	if (m_bUpdatesFrozen)
	{
		POSITION position = m_PendingUpdateList.GetHeadPosition();
		while (NULL != position)
		{
			CXTPMarkupUIElement* pUIElement = m_PendingUpdateList.GetNext(position);
			if (NULL != pUIElement)
			{
				if (!m_bEntireUpdatePended && NULL != m_hContextWnd)
				{
					CRect rcUpdate = GetUpdateRect(pUIElement);
					InvalidateRect(m_hContextWnd, rcUpdate, FALSE);
					UpdateWindow(m_hContextWnd);
				}

				MARKUP_RELEASE(pUIElement);
			}
		}

		if (NULL != m_hContextWnd && !m_PendingUpdateList.IsEmpty())
		{
			if (m_bEntireUpdatePended)
			{
				InvalidateRect(m_hContextWnd, NULL, FALSE);
			}

			::SendMessage(m_hContextWnd, XTP_WM_MARKUPREDRAW, 0, 0);
		}

		m_PendingUpdateList.RemoveAll();
		m_bUpdatesFrozen	  = FALSE;
		m_bEntireUpdatePended = FALSE;
	}
}

long CXTPMarkupContext::OleGetDefaultSmoothingMode()
{
	return static_cast<long>(GetDefaultSmoothingMode());
}

void CXTPMarkupContext::OleSetDefaultSmoothingMode(long lMode)
{
	SetDefaultSmoothingMode(static_cast<XTPMarkupSmoothingMode>(lMode));
}

BOOL CXTPMarkupContext::OleGetDefaultTextSmoothing()
{
	return GetDefaultTextSmoothing();
}

void CXTPMarkupContext::OleSetDefaultTextSmoothing(BOOL bEnableTextSmoothing)
{
	SetDefaultTextSmoothing(bEnableTextSmoothing);
}

void CXTPMarkupContext::SetDpiAware(BOOL bSet, HWND hWnd)
{
	if (NULL != m_pDpi)
	{
		delete m_pDpi;
		m_pDpi = NULL;
	}

	if (bSet)
	{
		m_pDpi = new CXTPDpi(hWnd);
	}
}

void CXTPMarkupContext::SetDpiAware(BOOL bSet, HDC hDC)
{
	if (NULL != m_pDpi)
	{
		delete m_pDpi;
		m_pDpi = NULL;
	}

	if (bSet)
	{
		m_pDpi = new CXTPDpi(hDC);
	}
}
