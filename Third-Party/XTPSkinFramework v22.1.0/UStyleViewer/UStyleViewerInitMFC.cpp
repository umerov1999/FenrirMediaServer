#include "pch.h"
#include "UStyleViewerInitMFC.h"
#include "UStyleViewerDialog.h"

#ifdef _WIN64
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif _WIN32
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void UStyleViewer::CreateDialogMFC()
{
	UStyleViewerDialog dlgS;

	m_pMainWnd = &dlgS;
	dlgS.DoModal();
	return;
}

BEGIN_MESSAGE_MAP(UStyleViewer, CXTPWinApp)

END_MESSAGE_MAP()

UStyleViewer::UStyleViewer()
{

}

UStyleViewer theApp;

BOOL UStyleViewer::InitInstance()
{
	AfxEnableControlContainer();

	CreateDialogMFC();
	return FALSE;
}