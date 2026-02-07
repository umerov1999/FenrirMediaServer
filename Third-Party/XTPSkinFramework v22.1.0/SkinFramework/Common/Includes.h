// Includes.h : header file
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
#if !defined(__XTPCOMMON_INCLUDES_H__)
#	define __XTPCOMMON_INCLUDES_H__

#	if _MSC_VER >= 1000
#		pragma once
#	endif // _MSC_VER >= 1000

#	if defined(_XTP_USE_WINAPPEX) && (_MFC_VER >= 0x0900)
#		include "Common/Base/Diagnostic/XTPDisableAdvancedWarnings.h"
#		include <afxwinappex.h>
#		include "Common/Base/Diagnostic/XTPEnableAdvancedWarnings.h"
#	endif

#	include "Common/XTPMacros.h"
#	include "Common/XTPCasting.h"

#	include "GraphicLibrary/GdiPlus/XTPGdiPlus.h"

#	include "Common/Resource.h"
#	include "Tmschema.h"

//{{AFX_CODEJOCK_PRIVATE
#	define XTP_INTERNAL_UXTHEME_INCLUSION
//}}AFX_CODEJOCK_PRIVATE
#	include "UxTheme.h"

#	include "Common/XTPVersion.h"
#	include "Common/XTPTypeId.h"
#	include "Common/XTPFramework.h"
#	include "Common/XTPVC80Helpers.h"
#	include "Common/XTPSystemHelpers.h"
#	include "Common/XTPSynchro.h"
#	include "Common/XTPApplication.h"
#	include "Common/XTPSingleton.h"
#	include "Common/XTPMathUtils.h"
#	include "Common/XTPGdiObjects.h"
#	include "Common/XTPResourceManager.h"
#	include "Common/XTPFontManager.h"
#	include "Common/XTPColorManager.h"
#	include "Common/XTPWinThemeWrapper.h"
#	include "Common/XTPDrawHelpers.h"
#	include "Common/XTPImageManager.h"
#	include "Common/XTPMaskEditT.h"
#	include "Common/PropExchange/XTPPropExchangeSection.h"
#	include "Common/XTPPropExchange.h"
#	include "Common/XTPToolTipContext.h"
#	include "Common/XTPResourceImage.h"
#	include "Common/XTPNotifyConnection.h"
#	include "Common/XTPSmartPtrInternalT.h"
#	include "Common/XTPRichRender.h"
#	include "Common/XTPHookManager.h"
#	include "Common/XTPMarkupRender.h"
#	include "Common/XTPCustomHeap.h"
#	include "Common/XTPXMLHelpers.h"
#	include "Common/XTPThemeDLL.h"
#	include "Common/XTPImageList.h"

#	include "Common/Hook/XTPWinEventHook.h"

#	include "Common/Controls/Progress/XTPProgressBase.h"
#	include "Common/Base/XTPWinApp.h"
#	include "Common/Base/Types/XTPPoint2.h"
#	include "Common/Base/Types/XTPPoint3.h"
#	include "Common/Base/Types/XTPSize.h"
#	include "Common/Base/Types/XTPRect.h"

#	include "Common/ScrollBar/XTPScrollInfo.h"
#	include "Common/ScrollBar/XTPScrollBase.h"
#	include "Common/ScrollBar/XTPScrollBarCtrl.h"
#	include "Common/ScrollBar/XTPScrollBarPaintManager.h"
#	include "Common/ScrollBar/XTPWnd.h"
#	include "Common/ScrollBar/XTPScrollBarContainer.h"
#	include "Common/ScrollBar/XTPScrollable.h"

#	include "Common/FrameShadow/XTPFrameShadowManager.h"
#	include "Common/FrameShadow/XTPFrameShadow.h"

#	include "Common/PropExchange/XTPPropExchangeArchive.h"
#	include "Common/PropExchange/XTPPropExchangeEnumerator.h"
#	include "Common/PropExchange/XTPPropExchangeEnumeratorPtr.h"
#	include "Common/PropExchange/XTPPropExchangeIniFile.h"
#	include "Common/PropExchange/XTPPropExchangeRegistry.h"
#	include "Common/PropExchange/XTPPropExchangeSection.h"
#	include "Common/PropExchange/XTPPropExchangeXMLNode.h"

#endif // !defined(__XTPCOMMON_INCLUDES_H__)
//}}AFX_CODEJOCK_PRIVATE
