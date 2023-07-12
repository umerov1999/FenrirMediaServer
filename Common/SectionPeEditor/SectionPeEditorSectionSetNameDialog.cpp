#include "pch.h"
#include "SectionPeEditorSectionSetNameDialog.h"

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

SectionPeEditorSectionSetNameDialog::SectionPeEditorSectionSetNameDialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(SectionPeEditorSectionSetNameDialog::IDD, pParent)
{
}

SectionPeEditorSectionSetNameDialog::~SectionPeEditorSectionSetNameDialog()
{
}

void SectionPeEditorSectionSetNameDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, NameEd);
}

BEGIN_MESSAGE_MAP(SectionPeEditorSectionSetNameDialog, CDialogEx)

	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_SETCURSOR()
	ON_WM_HSCROLL()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON1, OnOkT)
END_MESSAGE_MAP()

BOOL SectionPeEditorSectionSetNameDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	XTPSkinMgr()->AlphaEnableWindow(m_hWnd, 220);
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON1);
	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);

	NameEd.SetLimitText(7);

	return TRUE;
}

void SectionPeEditorSectionSetNameDialog::OnSysCommand(UINT nID, LPARAM lParam)
{
	CDialogEx::OnSysCommand(nID, lParam);
}

void SectionPeEditorSectionSetNameDialog::OnPaint()
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

HCURSOR SectionPeEditorSectionSetNameDialog::OnQueryDragIcon()
{
	return (HCURSOR)m_hIcon;
}

void SectionPeEditorSectionSetNameDialog::OnOkT()
{
	if (NameEd.GetWindowTextLengthW() <= 0)
	{
		MessageBoxW(L"Введите название секции. Максимально 7 символов", L"Внимание!", MB_ICONINFORMATION);
		return;
	}
	NameEd.GetWindowTextW(NmStr);
	Ret = true;
	EndDialog(0);
}

void SectionPeEditorSectionSetNameDialog::OnClose()
{
	Ret = false;
	EndDialog(0);
}