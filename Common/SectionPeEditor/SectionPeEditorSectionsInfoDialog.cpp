#include "pch.h"
#include "SectionPeEditorSectionsInfoDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifndef WS_EX_LAYERED 
#define WS_EX_LAYERED 0x00080000 
#define LWA_COLORKEY 0x00000001 
#define LWA_ALPHA 0x00000002 
#endif // WS_EX_LAYERED

SectionPeEditorSectionsInfoDialog::SectionPeEditorSectionsInfoDialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(SectionPeEditorSectionsInfoDialog::IDD, pParent)
{
}

SectionPeEditorSectionsInfoDialog::~SectionPeEditorSectionsInfoDialog()
{
}

void SectionPeEditorSectionsInfoDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, ResultsEd);
}

BEGIN_MESSAGE_MAP(SectionPeEditorSectionsInfoDialog, CDialogEx)

	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_SETCURSOR()
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_BUTTON1, ToWinBuf)
END_MESSAGE_MAP()

BOOL SectionPeEditorSectionsInfoDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	XTPSkinMgr()->AlphaEnableWindow(m_hWnd, 220);
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON1);
	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);
	ResultsEd.SetWindowTextW(Resu);
	return TRUE;
}

void SectionPeEditorSectionsInfoDialog::OnSysCommand(UINT nID, LPARAM lParam)
{
	CDialogEx::OnSysCommand(nID, lParam);
}

void SectionPeEditorSectionsInfoDialog::OnPaint()
{

	if (IsIconic())
	{
		CPaintDC dc(this);

		SendMessage(WM_ICONERASEBKGND, (WPARAM)dc.GetSafeHdc(), 0);

		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}

}

HCURSOR SectionPeEditorSectionsInfoDialog::OnQueryDragIcon()
{
	return (HCURSOR)m_hIcon;
}


void SectionPeEditorSectionsInfoDialog::ToWinBuf()
{
	CString nmm;
	ResultsEd.GetWindowTextW(nmm);
	OpenClipboard();
	EmptyClipboard();
	HGLOBAL hStrMem = GlobalAlloc(GMEM_MOVEABLE, nmm.GetLength() * sizeof(wchar_t));
	void* pStrMem = GlobalLock(hStrMem);
	memcpy(pStrMem, nmm.GetString(), nmm.GetLength() * sizeof(wchar_t));
	GlobalUnlock(pStrMem);
	SetClipboardData(CF_UNICODETEXT, hStrMem);
	CloseClipboard();
}

void SectionPeEditorSectionsInfoDialog::OnClose()
{
	EndDialog(0);
}