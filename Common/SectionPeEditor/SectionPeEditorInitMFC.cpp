#include "pch.h"
#include "SectionPeEditorInitMFC.h"
#include "SectionPeEditorDialog.h"

#ifdef _WIN64
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif _WIN32
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif

extern int APP_STYLE_SIZE;
extern const char* APP_STYLE_DATA;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void SectionPeEditor::CreateDialogMFC()
{
	SectionPeEditorDialog dlgS;

	m_pMainWnd = &dlgS;
	dlgS.DoModal();
	return;
}

void SectionPeEditor::LoadStyle()
{
	if (XTPSkinMgr()->SkinLoadMemory(APP_STYLE_DATA, APP_STYLE_SIZE) == FALSE)
	{
		MessageBoxW(0, L"Стиль повреждён!", L"Внимание!", MB_ICONINFORMATION);
		return;
	}
}

BEGIN_MESSAGE_MAP(SectionPeEditor, CXTPWinApp)

END_MESSAGE_MAP()

SectionPeEditor theApp;

BOOL SectionPeEditor::InitInstance()
{
	AfxEnableControlContainer();

	LoadStyle();
	CreateDialogMFC();
	return FALSE;
}