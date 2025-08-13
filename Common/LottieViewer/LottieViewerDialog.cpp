#include "pch.h"
#include <memory> 
#include "LottieViewerDialog.h"
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

LottieViewerDialog::LottieViewerDialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(LottieViewerDialog::IDD, pParent)
{
	memset(szBuf, 0, sizeof(szBuf));
	m_hIcon = nullptr;
	m_hCursor = nullptr;

	int argcou = 0;
	std::wstring Path = L"";
	LPWSTR* CommandLns = CommandLineToArgvW(XTPSkinMgr()->GetCommandLineTool().GetString(), &argcou);
	if (argcou > 1) {
		Path = CommandLns[1];

		FILE* fl = nullptr;
		if (!_wfopen_s(&fl, Path.c_str(), L"rb")) {
			if (fl) {
				fseek(fl, 0, SEEK_END);
				size_t sz = ftell(fl);
				fseek(fl, 0, SEEK_SET);
				char* buf = new char[sz];
				fread(buf, 1, sz, fl);
				fclose(fl);
				Anim.load_animation(buf, sz);
				delete[] buf;
			}
		}
	}
}

LottieViewerDialog::~LottieViewerDialog() {

}

void LottieViewerDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LOTTIE1, Anim);
}

BEGIN_MESSAGE_MAP(LottieViewerDialog, CDialogEx)

	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_SETCURSOR()
	ON_WM_CLOSE()
END_MESSAGE_MAP()

BOOL LottieViewerDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	XTPSkinMgr()->AlphaEnableWindow(m_hWnd, CurrentAlpha);

	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON1);
	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);

	m_hCursor = AfxGetApp()->LoadCursor(MAKEINTRESOURCEW(IDC_CURSOR1));

	return TRUE;
}

BOOL LottieViewerDialog::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	::SetCursor(m_hCursor);
	return TRUE;
}

void LottieViewerDialog::OnSysCommand(UINT nID, LPARAM lParam)
{
	CDialogEx::OnSysCommand(nID, lParam);
}

void LottieViewerDialog::OnPaint()
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

HCURSOR LottieViewerDialog::OnQueryDragIcon()
{
	return (HCURSOR)m_hIcon;
}

void LottieViewerDialog::OnClose()
{
	_exit(0);
}

BOOL LottieViewerDialog::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->hwnd == GetSafeHwnd())
	{
		if (pMsg->message == WM_DROPFILES)
		{
			long ResultQueryFile = DragQueryFileW((HDROP)pMsg->wParam, 0xFFFFFFFF, NULL, 0);
			if (ResultQueryFile == 1)
			{
				ResultQueryFile = DragQueryFileW((HDROP)pMsg->wParam, 0, szBuf, sizeof(szBuf) / sizeof(wchar_t));
				FILE* fl = nullptr;
				if (!_wfopen_s(&fl, szBuf, L"rb")) {
					if (fl) {
						fseek(fl, 0, SEEK_END);
						size_t sz = ftell(fl);
						fseek(fl, 0, SEEK_SET);
						char* buf = new char[sz];
						fread(buf, 1, sz, fl);
						fclose(fl);

						//auto customColors = tvg::ColorReplace().setUseCustomColorsLottieOffset().registerCustomColorLottie(RGB(255, 129, 46), RGB(68, 138, 255));
						//Anim.load_animation(buf, sz, &customColors);

						auto customColors = tvg::ColorReplace().registerCustomColorLottie(RGB(0, 0, 0), RGB(68, 138, 255)).registerCustomColorLottie(RGB(119, 119, 119), RGB(30, 136, 229));
						Anim.load_animation(buf, sz, &customColors);
						delete[] buf;
					}
				}
			}
		}
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}