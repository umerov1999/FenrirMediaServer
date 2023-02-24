#include "pch.h"
#include "VKApiToolsDialogCapcha.h"
#include "do_curl.h"
#include "WSTRUtils.h"
#include "libimage.h"
using namespace LIB_IMAGE;
using namespace std;
using namespace WSTRUtils;
bool IsIWantEntryCapcha = false;

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

VKApiToolsDialogCapcha::VKApiToolsDialogCapcha(CString Link, CString UserAgent, CWnd* pParent /*=NULL*/)
	: CDialogEx(VKApiToolsDialogCapcha::IDD, pParent)
{
	LinkStr = Link;
	UserAgentStr = UserAgent;
	NeedCapcha = true;
}

VKApiToolsDialogCapcha::~VKApiToolsDialogCapcha()
{
}

void VKApiToolsDialogCapcha::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CAPCHA, HBitmapCapcha);
	DDX_Control(pDX, IDC_EDIT1, LinkEd);
	DDX_Control(pDX, IDC_EDIT2, CodeEd);
	DDX_Control(pDX, IDC_BUTTON1, RunBtn);
}

BEGIN_MESSAGE_MAP(VKApiToolsDialogCapcha, CDialogEx)

	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_HSCROLL()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON1, OnRun)
END_MESSAGE_MAP()

static DWORD WINAPI RunThr(LPVOID dlg)
{
	VKApiToolsDialogCapcha *th = (VKApiToolsDialogCapcha*)dlg;
	for (size_t i = 0; i < 6 && th->NeedCapcha == true; i++)
		Sleep(1000);
	if (th->NeedCapcha == true)
		::PostMessage(th->GetSafeHwnd(), WM_CLOSE, 0, 0);
	return 0;
}

BOOL VKApiToolsDialogCapcha::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	XTPSkinMgr()->AlphaEnableWindow(m_hWnd, 220);
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON1);
	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);

	string Data;
	DoCurlCapcha(wchar_to_UTF8(LinkStr.GetString()), wchar_to_UTF8(UserAgentStr.GetString()), Data, false);
	CRect rt;
	HBitmapCapcha.GetClientRect(rt);
	CapchaBitmap = PrepareImageFromBufferAutoType(HBitmapCapcha.m_hWnd, Data.data(), (int)Data.size(), false);
	if (CapchaBitmap.is_has_image() && (rt.Width() < CapchaBitmap.get_size().size_x || rt.Height() < CapchaBitmap.get_size().size_y))
		CapchaBitmap.resize(rt.Width(), rt.Height());
	if (!CapchaBitmap.is_has_image()) {
		std::string DataPic = GetDataFromResourceUtil(L"SVG", IDR_SVG2);
		CapchaBitmap = PrepareImageFromSVG(HBitmapCapcha.m_hWnd, rt.Width(), rt.Height(), DataPic.data(), (int)DataPic.size());
	}
	HBitmapCapcha.SetBitmap(CapchaBitmap.get_hBitmap());
	LinkEd.SetWindowTextW(LinkStr);
	if (IsIWantEntryCapcha == true)
	{
		NeedCapcha = false;
		RunBtn.SetWindowTextW(L"Готово");
	}
	else
	{
		NeedCapcha = true;
		CreateThreadSimple(&RunThr, this);
	}
	return TRUE;
}

void VKApiToolsDialogCapcha::OnSysCommand(UINT nID, LPARAM lParam)
{
	CDialogEx::OnSysCommand(nID, lParam);
}

void VKApiToolsDialogCapcha::OnPaint()
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

HCURSOR VKApiToolsDialogCapcha::OnQueryDragIcon()
{
	return (HCURSOR)m_hIcon;
}

void VKApiToolsDialogCapcha::OnRun()
{
	if (NeedCapcha == true)
	{
		NeedCapcha = false;
		RunBtn.SetWindowTextW(L"Готово");
		IsIWantEntryCapcha = true;
		return;
	}
	CodeEd.GetWindowTextW(CapchaCode);
	EndDialog(TRUE);
}

void VKApiToolsDialogCapcha::OnClose()
{
	EndDialog(FALSE);
}