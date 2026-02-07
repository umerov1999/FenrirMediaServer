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
#if !defined(__XTPMARKUP_INCLUDES_H__)
#	define __XTPMARKUP_INCLUDES_H__

#	if _MSC_VER >= 1000
#		pragma once
#	endif // _MSC_VER >= 1000

#	include "Common/Base/Diagnostic/XTPDisableAdvancedWarnings.h"
#	include <DispEx.h>
#	include "Common/Base/Diagnostic/XTPEnableAdvancedWarnings.h"

#	include "GraphicLibrary/GdiPlus/XTPGdiPlus.h"

#	include "Common/Base/Types/XTPSize.h"
#	include "Common/Base/Types/XTPRect.h"

#	include "Markup\XTPMarkupTools.h"
#	include "Markup\XTPMarkupBuilder.h"
#	include "Markup\XTPMarkupContext.h"
#	include "Markup\XTPMarkupObject.h"
#	include "Markup\XTPMarkupInputElement.h"
#	include "Markup\XTPMarkupUIElement.h"
#	include "Markup\XTPMarkupDelegate.h"
#	include "Markup\XTPMarkupDrawingContext.h"
#	include "Markup\XTPMarkupEventHandlerMap.h"
#	include "Markup\XTPMarkupFrameworkContentElement.h"
#	include "Markup\XTPMarkupFrameworkElement.h"
#	include "Markup\XTPMarkupRoutedEvent.h"
#	include "Markup\XTPMarkupRoutedEventArgs.h"
#	include "Markup\XTPMarkupKeyboardNavigation.h"
#	include "Markup\XTPMarkupMouseEventArgs.h"
#	include "Markup\XTPMarkupMouseButtonEventArgs.h"
#	include "Markup\XTPMarkupMouseWheelEventArgs.h"
#	include "Markup\XTPMarkupParser.h"
#	include "Markup\XTPMarkupQueryCursorEventArgs.h"
#	include "Markup\XTPMarkupResourceDictionary.h"
#	include "Markup\XTPMarkupSetter.h"
#	include "Markup\XTPMarkupSetterCollection.h"
#	include "Markup\XTPMarkupString.h"
#	include "Markup\XTPMarkupStyle.h"
#	include "Markup\XTPMarkupThickness.h"
#	include "Markup\XTPMarkupTrigger.h"
#	include "Markup\XTPMarkupTriggerCollection.h"
#	include "Markup\XTPMarkupTypedSimpleStack.h"
#	include "Markup\DeviceContext\XTPMarkupDeviceContext.h"
#	include "Markup\DeviceContext\XTPMarkupDeviceDependentImage.h"
#	include "Markup\Shapes\XTPMarkupShape.h"
#	include "Markup\Shapes\XTPMarkupEllipse.h"
#	include "Markup\Shapes\XTPMarkupLine.h"
#	include "Markup\Shapes\XTPMarkupPath.h"
#	include "Markup\Shapes\XTPMarkupPathData.h"
#	include "Markup\Shapes\XTPMarkupPathGeometry.h"
#	include "Markup\Shapes\XTPMarkupPathGeometryBuilder.h"
#	include "Markup\Shapes\XTPMarkupPathGeometryFigures.h"
#	include "Markup\Shapes\XTPMarkupPolygon.h"
#	include "Markup\Shapes\XTPMarkupPolyline.h"
#	include "Markup\Shapes\XTPMarkupRectangle.h"
#	include "Markup\Text\XTPMarkupTextElement.h"
#	include "Markup\Text\XTPMarkupInline.h"
#	include "Markup\Text\XTPMarkupSpan.h"
#	include "Markup\Text\XTPMarkupBold.h"
#	include "Markup\Text\XTPMarkupHyperlink.h"
#	include "Markup\Text\XTPMarkupInlineCollection.h"
#	include "Markup\Text\XTPMarkupInlineUIContainer.h"
#	include "Markup\Text\XTPMarkupItalic.h"
#	include "Markup\Text\XTPMarkupRun.h"
#	include "Markup\Text\XTPMarkupLineBreak.h"
#	include "Markup\Text\XTPMarkupUnderline.h"
#	include "Markup\Controls\XTPMarkupUIElementCollection.h"
#	include "Markup\Controls\XTPMarkupControl.h"
#	include "Markup\Controls\XTPMarkupContentControl.h"
#	include "Markup\Controls\XTPMarkupButtonBase.h"
#	include "Markup\Controls\XTPMarkupDecorator.h"
#	include "Markup\Controls\XTPMarkupDefinitionBase.h"
#	include "Markup\Controls\XTPMarkupBorder.h"
#	include "Markup\Controls\XTPMarkupButton.h"
#	include "Markup\Controls\XTPMarkupPanel.h"
#	include "Markup\Controls\XTPMarkupCanvas.h"
#	include "Markup\Controls\XTPMarkupToggleButton.h"
#	include "Markup\Controls\XTPMarkupCheckBox.h"
#	include "Markup\Controls\XTPMarkupColumnDefinition.h"
#	include "Markup\Controls\XTPMarkupDockPanel.h"
#	include "Markup\Controls\XTPMarkupGrid.h"
#	include "Markup\Controls\XTPMarkupGridLength.h"
#	include "Markup\Controls\XTPMarkupColumnDefinitionCollection.h"
#	include "Markup\Controls\XTPMarkupImage.h"
#	include "Markup\Controls\XTPMarkupPage.h"
#	include "Markup\Controls\XTPMarkupRadioButton.h"
#	include "Markup\Controls\XTPMarkupRowDefinition.h"
#	include "Markup\Controls\XTPMarkupRowDefinitionCollection.h"
#	include "Markup\Controls\XTPMarkupScrollBar.h"
#	include "Markup\Controls\XTPMarkupScrollViewer.h"
#	include "Markup\Controls\XTPMarkupStackPanel.h"
#	include "Markup\Controls\XTPMarkupTextBlock.h"
#	include "Markup\Controls\XTPMarkupUniformGrid.h"
#	include "Markup\Controls\XTPMarkupWrapPanel.h"
#	include "Markup\Transform\XTPMarkupTransform.h"
#	include "Markup\Transform\XTPMarkupRenderTransform.h"
#	include "Markup\Transform\XTPMarkupRotateTransform.h"
#	include "Markup\Transform\XTPMarkupScaleTransform.h"
#	include "Markup\Transform\XTPMarkupSkewTransform.h"
#	include "Markup\Transform\XTPMarkupTransformationMatrix.h"
#	include "Markup\Transform\XTPMarkupTranslateTransform.h"
#	include "Markup\Extensions\XTPMarkupExtension.h"
#	include "Markup\Extensions\XTPMarkupStaticExtension.h"
#	include "Markup\Extensions\XTPMarkupSystemColorsStaticExtension.h"
#	include "Markup\Extensions\XTPMarkupXtpScript.h"

#endif //#if !defined(__XTPMARKUP_INCLUDES_H__)
//}}AFX_CODEJOCK_PRIVATE
