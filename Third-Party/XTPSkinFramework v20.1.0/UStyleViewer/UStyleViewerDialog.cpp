#include "pch.h"
#include "UStyleViewerDialog.h"
//--noskin Disable SkinFramework
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifndef WS_EX_LAYERED 
#define WS_EX_LAYERED 0x00080000 
#define LWA_COLORKEY 0x00000001 
#define LWA_ALPHA 0x00000002 
#endif

CProgressCtrl Cprg;
CButton IsEn;
static BYTE CurrentAlpha = 240;

UStyleViewerDialog::UStyleViewerDialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(UStyleViewerDialog::IDD, pParent)
{
	XTPSkinMgr()->SetApplyOptions(XTPSkinManager()->GetApplyOptions() | xtpSkinApplyMetrics);
	int argcou = 0;
	std::wstring Path = L"";
	LPWSTR* CommandLns = CommandLineToArgvW(XTPSkinMgr()->GetCommandLineTool().GetString(), &argcou);
	if (argcou > 1)
	{
		Path = CommandLns[1];
		if (XTPSkinMgr()->SkinLoadFile(CommandLns[1]) == FALSE)
		{
			Path = L"";
			MessageBoxW(L"Стиль повреждён!", L"Внимание!", MB_ICONINFORMATION);
		}
	}
	size_t pos = Path.find_last_of(L"\\/");
	if (pos != std::wstring::npos)
		Path = Path.substr(pos + 1);
	if (Path.length() > 0)
		Header = Path.c_str();
}

UStyleViewerDialog::~UStyleViewerDialog()
{
	XTPSkinMgr()->SkinRelease();
}

void UStyleViewerDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, Ed);
	DDX_Control(pDX, IDC_LIST1, Lc);
	DDX_Control(pDX, IDC_SLIDER1, Sldr);
	DDX_Control(pDX, IDC_COMBO1, Cmb);
	DDX_Control(pDX, IDC_PROGRESS1, Cprg);
	DDX_Control(pDX, IDC_CHECK2, IsEn);
	DDX_Control(pDX, IDC_RADIO1, Rd);
	DDX_Control(pDX, IDC_COMBO2, wSchms);
	DDX_Control(pDX, IDC_CHECK3, Test);
	DDX_Control(pDX, IDC_RADIO3, TestRd);
	DDX_Control(pDX, IDC_EDIT2, POEd);
}

BEGIN_MESSAGE_MAP(UStyleViewerDialog, CDialogEx)

	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_SETCURSOR()
	ON_WM_HSCROLL()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON1, OnCloseStyle)
	ON_BN_CLICKED(IDC_BUTTON2, OnBrowseFolder)
	ON_LBN_SELCHANGE(IDC_COMBO2, OnSwitchShem)
END_MESSAGE_MAP()

void UStyleViewerDialog::OnBrowseFolder()
{
	TCHAR path[MAX_PATH];
	BROWSEINFOW bi = { 0 };
	bi.lpszTitle = L"Выберите папку...";
	bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;
	LPITEMIDLIST pidl = SHBrowseForFolderW(&bi);
	if (pidl != 0)
	{
		SHGetPathFromIDListW(pidl, path);
		IMalloc* imalloc = 0;
		if (SUCCEEDED(SHGetMalloc(&imalloc)))
		{
			imalloc->Free(pidl);
			imalloc->Release();
		}
		return;
	}
}

DWORD WINAPI Progress(LPVOID lp)
{
	int Pos = 0;
	while (true)
	{
		Sleep(50);
		if (IsEn.GetCheck() > 0)
		{
			if (Pos > 100)
				Pos = 0;
			Cprg.SetPos(Pos);
			Pos++;
		}
	}
	return 0;
}

BOOL UStyleViewerDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	XTPSkinMgr()->AlphaEnableWindow(m_hWnd, CurrentAlpha);

	if (Header.GetLength() > 0)
		SetWindowTextW(Header.GetString());

	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON1);
	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);

	m_hCursor = AfxGetApp()->LoadCursor(MAKEINTRESOURCEW(IDC_CURSOR1));

	Sldr.SetRange(100, 255);
	Sldr.SetPos(CurrentAlpha);

	Test.SetCheck(1);
	Test.EnableWindow(FALSE);
	POEd.SetWindowTextW(L"1234567890");

	Lc.AddString(L"1");
	Lc.AddString(L"2");
	Lc.AddString(L"3");
	Lc.AddString(L"4");
	Lc.AddString(L"5");
	Lc.AddString(L"6");
	Lc.AddString(L"7");
	Lc.AddString(L"8");
	Lc.AddString(L"9");
	Lc.AddString(L"10");
	Lc.AddString(L"End");

	Cmb.AddString(L"1");
	Cmb.AddString(L"2");
	Cmb.AddString(L"End");
	Cmb.SelectString(0, L"2");

	Ed.SetWindowTextW(L"Текст");

	Cprg.SetPos(75);
	TestRd.SetCheck(1);
	TestRd.EnableWindow(FALSE);
	Rd.SetCheck(1);

	CreateThread(NULL, NULL, &Progress, NULL, NULL, NULL);

	UpdateShems();

	return TRUE;
}

BOOL UStyleViewerDialog::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	::SetCursor(m_hCursor);
	return TRUE;
}

void UStyleViewerDialog::OnSysCommand(UINT nID, LPARAM lParam)
{
	CDialogEx::OnSysCommand(nID, lParam);
}

void UStyleViewerDialog::OnPaint()
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

HCURSOR UStyleViewerDialog::OnQueryDragIcon()
{
	return (HCURSOR)m_hIcon;
}

void UStyleViewerDialog::OnCloseStyle()
{
	SetWindowTextW(L"UStyleViewer");
	XTPSkinMgr()->SkinRelease();
	wSchms.ResetContent();
}

void UStyleViewerDialog::OnSwitchShem()
{
	if (wSchms.GetCurSel() < 0)
		return;
	CString tmxt;
	wSchms.GetWindowTextW(tmxt);
	XTPSkinMgr()->SkinSwitchSchema(tmxt.GetString());
}

void UStyleViewerDialog::UpdateShems()
{
	wSchms.ResetContent();
	std::vector<CString> List = XTPSkinMgr()->SkinListSchemes();
	if (List.size() <= 0)
		return;
	for (auto&i : List)
		wSchms.AddString(i.GetString());
	wSchms.SelectString(0, XTPSkinMgr()->SkinGetDefaultSchema().GetString());
}

void UStyleViewerDialog::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	CSliderCtrl *ACSliderCtrl = (CSliderCtrl *)pScrollBar;
	int nID = ACSliderCtrl->GetDlgCtrlID();
	BYTE NewPos = (BYTE)((CSliderCtrl *)pScrollBar)->GetPos();
	CWnd *ACWnd = GetDlgItem(nID);


	switch (nID)
	{
	default:
		break;

	case IDC_SLIDER1:
		CurrentAlpha = NewPos;
		XTPSkinMgr()->AlphaChengeWindow(m_hWnd, CurrentAlpha);
		break;
	}

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void UStyleViewerDialog::OnClose()
{
	XTPSkinMgr()->ExitProgramm();
}

BOOL UStyleViewerDialog::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->hwnd == GetSafeHwnd())
	{
		if (pMsg->message == WM_DROPFILES)
		{
			long ResultQueryFile = DragQueryFileW((HDROP)pMsg->wParam, 0xFFFFFFFF, NULL, 0);
			if (ResultQueryFile == 1)
			{
				ResultQueryFile = DragQueryFileW((HDROP)pMsg->wParam, 0, szBuf, sizeof(szBuf));

				if (XTPSkinMgr()->SkinLoadFile(szBuf) == FALSE)
				{
					MessageBoxW(L"Стиль повреждён!", L"Внимание!", MB_ICONINFORMATION);
					return CDialogEx::PreTranslateMessage(pMsg);
				}
				else
				{
					std::wstring SkinPath = szBuf;
					size_t pos = SkinPath.find_last_of(L"\\/");
					if (pos != std::wstring::npos)
						SkinPath = SkinPath.substr(pos + 1);
					if (SkinPath.length() > 0)
						SetWindowTextW(SkinPath.c_str());
				}
				UpdateShems();
			}
		}
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}