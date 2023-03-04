#include "pch.h"
#include "MediaServerMediaFoldersDialog.h"
#include "MediaServerTask.h"
using namespace std;
using namespace WSTRUtils;
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
extern list<wstring> Discography_Dirs;
extern list<wstring> Audio_Dirs;
extern list<wstring> Video_Dirs;
extern list<wstring> Photo_Video_Dirs;
extern std::wstring ExtractAppPath();

MediaServerMediaFoldersDialog::MediaServerMediaFoldersDialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(MediaServerMediaFoldersDialog::IDD, pParent)
{
	m_hIcon = nullptr;
	m_hCursor = nullptr;
}

MediaServerMediaFoldersDialog::~MediaServerMediaFoldersDialog()
{
}

void MediaServerMediaFoldersDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, ListAudios);
	DDX_Control(pDX, IDC_LIST2, ListVideos);
	DDX_Control(pDX, IDC_LIST3, ListDiscography);
	DDX_Control(pDX, IDC_LIST4, ListPhotoVideos);
	DDX_Control(pDX, IDC_BUTTON2, EraseAudios);
	DDX_Control(pDX, IDC_BUTTON7, EraseVideos);
	DDX_Control(pDX, IDC_BUTTON9, EraseDiscography);
	DDX_Control(pDX, IDC_BUTTON11, ErasePhotoVideos);
	DDX_Control(pDX, IDC_BUTTON1, AddAudios);
	DDX_Control(pDX, IDC_BUTTON3, AddVideos);
	DDX_Control(pDX, IDC_BUTTON8, AddDiscography);
	DDX_Control(pDX, IDC_BUTTON10, AddPhotoVideos);
}

BEGIN_MESSAGE_MAP(MediaServerMediaFoldersDialog, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_SETCURSOR()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON1, OnAddAudio)
	ON_BN_CLICKED(IDC_BUTTON2, OnDeleteAudio)
	ON_BN_CLICKED(IDC_BUTTON3, OnAddVideo)
	ON_BN_CLICKED(IDC_BUTTON10, OnAddPhotoVideo)
	ON_BN_CLICKED(IDC_BUTTON7, OnDeleteVideo)
	ON_BN_CLICKED(IDC_BUTTON8, OnAddDiscography)
	ON_BN_CLICKED(IDC_BUTTON9, OnDeleteDiscography)
	ON_BN_CLICKED(IDC_BUTTON11, OnDeletePhotoVideo)
END_MESSAGE_MAP()

BOOL MediaServerMediaFoldersDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	XTPSkinMgr()->AlphaEnableWindow(m_hWnd, 220);
	ListAudios.Init();
	ListVideos.Init();
	ListDiscography.Init();
	ListPhotoVideos.Init();
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON1);
	m_hCursor = AfxGetApp()->LoadCursor(MAKEINTRESOURCEW(IDC_CURSOR1));

	Pdelete = LIB_IMAGE::PrepareImageFromSVGResource(EraseAudios.m_hWnd, IDI_SVG2);
	PAdd = LIB_IMAGE::PrepareImageFromSVGResource(AddAudios.m_hWnd, IDI_SVG3);

	EraseAudios.SetBitmap(Pdelete.get_hBitmap());
	EraseVideos.SetBitmap(Pdelete.get_hBitmap());
	EraseDiscography.SetBitmap(Pdelete.get_hBitmap());
	ErasePhotoVideos.SetBitmap(Pdelete.get_hBitmap());

	AddAudios.SetBitmap(PAdd.get_hBitmap());
	AddVideos.SetBitmap(PAdd.get_hBitmap());
	AddDiscography.SetBitmap(PAdd.get_hBitmap());
	AddPhotoVideos.SetBitmap(PAdd.get_hBitmap());

	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);
	ReloadContent();

	return TRUE;
}

void MediaServerMediaFoldersDialog::OnSysCommand(UINT nID, LPARAM lParam)
{
	CDialogEx::OnSysCommand(nID, lParam);
}

void MediaServerMediaFoldersDialog::OnPaint()
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
		CDialogEx::OnPaint();
}

BOOL MediaServerMediaFoldersDialog::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	::SetCursor(m_hCursor);
	return TRUE;
}

HCURSOR MediaServerMediaFoldersDialog::OnQueryDragIcon()
{
	return (HCURSOR)m_hIcon;
}

void MediaServerMediaFoldersDialog::ReloadContent()
{
	ListAudios.Clear();
	ListVideos.Clear();
	ListDiscography.Clear();
	ListPhotoVideos.Clear();
	for (auto &i : Audio_Dirs)
		ListAudios.AddLine(i.c_str());
	for (auto& i : Video_Dirs)
		ListVideos.AddLine(i.c_str());
	for (auto& i : Discography_Dirs)
		ListDiscography.AddLine(i.c_str());
	for (auto& i : Photo_Video_Dirs)
		ListPhotoVideos.AddLine(i.c_str());
}

static int CALLBACK BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
	if (uMsg == BFFM_INITIALIZED)
		SendMessageW(hwnd, BFFM_SETSELECTIONW, TRUE, lpData);
	return 0;
}

static bool BrowseFolder(CString& saved_path)
{
	TCHAR path[MAX_PATH];
	BROWSEINFOW bi = { 0 };
	bi.lpszTitle = L"Выберите папку для трансляции контента...";
	bi.ulFlags = BIF_EDITBOX | BIF_VALIDATE | BIF_BROWSEINCLUDEFILES;
#if (WINVER >= 0x0501)
	bi.ulFlags |= BIF_USENEWUI;
#endif
	bi.lpfn = BrowseCallbackProc;
	bi.lParam = (LPARAM)saved_path.GetString();
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
		saved_path = path;
		return true;
	}
	return false;
}

void MediaServerMediaFoldersDialog::OnAddAudio()
{
	CString res = ExtractAppPath().c_str();
	if (BrowseFolder(res) == true)
	{
		if (res.GetLength() > 0 && PathFileExistsW(res.GetString()) == TRUE)
		{
			bool exist = false;
			for (auto& i : Audio_Dirs) {
				if (Wcompare(i, res.GetString())) {
					exist = true;
					break;
				}
			}
			if (!exist) {
				Audio_Dirs.push_front(res.GetString());
			}
		}
	}
	ReloadContent();
}

void MediaServerMediaFoldersDialog::OnAddDiscography()
{
	CString res = ExtractAppPath().c_str();
	if (BrowseFolder(res) == true)
	{
		if (res.GetLength() > 0 && PathFileExistsW(res.GetString()) == TRUE)
		{
			bool exist = false;
			for (auto& i : Discography_Dirs) {
				if (Wcompare(i, res.GetString())) {
					exist = true;
					break;
				}
			}
			if (!exist) {
				Discography_Dirs.push_front(res.GetString());
			}
		}
	}
	ReloadContent();
}

void MediaServerMediaFoldersDialog::OnAddVideo()
{
	CString res = ExtractAppPath().c_str();
	if (BrowseFolder(res) == true)
	{
		if (res.GetLength() > 0 && PathFileExistsW(res.GetString()) == TRUE)
		{
			bool exist = false;
			for (auto& i : Video_Dirs) {
				if (Wcompare(i, res.GetString())) {
					exist = true;
					break;
				}
			}
			if (!exist) {
				Video_Dirs.push_front(res.GetString());
			}
		}
	}
	ReloadContent();
}

void MediaServerMediaFoldersDialog::OnAddPhotoVideo()
{
	CString res = ExtractAppPath().c_str();
	if (BrowseFolder(res) == true)
	{
		if (res.GetLength() > 0 && PathFileExistsW(res.GetString()) == TRUE)
		{
			bool exist = false;
			for (auto& i : Photo_Video_Dirs) {
				if (Wcompare(i, res.GetString())) {
					exist = true;
					break;
				}
			}
			if (!exist) {
				Photo_Video_Dirs.push_front(res.GetString());
			}
		}
	}
	ReloadContent();
}

void MediaServerMediaFoldersDialog::OnDeleteAudio()
{
	int Sel = ListAudios.GetSelected();
	if (Sel < 0)
		return;
	int HT = 0;
	for (auto i = Audio_Dirs.begin(); i != Audio_Dirs.end(); i++, HT++)
	{
		if (HT == Sel)
		{
			Audio_Dirs.erase(i);
			ReloadContent();
			return;
		}
	}
}

void MediaServerMediaFoldersDialog::OnDeleteDiscography()
{
	int Sel = ListDiscography.GetSelected();
	if (Sel < 0)
		return;
	int HT = 0;
	for (auto i = Discography_Dirs.begin(); i != Discography_Dirs.end(); i++, HT++)
	{
		if (HT == Sel)
		{
			Discography_Dirs.erase(i);
			ReloadContent();
			return;
		}
	}
}

void MediaServerMediaFoldersDialog::OnDeleteVideo()
{
	int Sel = ListVideos.GetSelected();
	if (Sel < 0)
		return;
	int HT = 0;
	for (auto i = Video_Dirs.begin(); i != Video_Dirs.end(); i++, HT++)
	{
		if (HT == Sel)
		{
			Video_Dirs.erase(i);
			ReloadContent();
			return;
		}
	}
}

void MediaServerMediaFoldersDialog::OnDeletePhotoVideo()
{
	int Sel = ListPhotoVideos.GetSelected();
	if (Sel < 0)
		return;
	int HT = 0;
	for (auto i = Photo_Video_Dirs.begin(); i != Photo_Video_Dirs.end(); i++, HT++)
	{
		if (HT == Sel)
		{
			Photo_Video_Dirs.erase(i);
			ReloadContent();
			return;
		}
	}
}

BOOL MediaServerMediaFoldersDialog::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->hwnd == ListAudios.GetSafeHwnd() || pMsg->hwnd == ListVideos.GetSafeHwnd() || pMsg->hwnd == ListDiscography.GetSafeHwnd() || pMsg->hwnd == ListPhotoVideos.GetSafeHwnd())
	{
		if (pMsg->message == WM_DROPFILES)
		{
			long ResultQueryFile = DragQueryFileW((HDROP)pMsg->wParam, 0xFFFFFFFF, NULL, 0);
			for (long i = 0; i < ResultQueryFile; i++) {
				{
					ResultQueryFile = DragQueryFileW((HDROP)pMsg->wParam, i, szBuf, sizeof(szBuf) / sizeof(wchar_t));
					if (pMsg->hwnd == ListAudios.GetSafeHwnd()) {
						if (PathFileExistsW(szBuf) == TRUE)
						{
							bool exist = false;
							for (auto& i : Audio_Dirs) {
								if (Wcompare(i, szBuf)) {
									exist = true;
									break;
								}
							}
							if (!exist) {
								Audio_Dirs.push_front(szBuf);
							}
						}
					}
					else if (pMsg->hwnd == ListVideos.GetSafeHwnd()) {
						if (PathFileExistsW(szBuf) == TRUE)
						{
							bool exist = false;
							for (auto& i : Video_Dirs) {
								if (Wcompare(i, szBuf)) {
									exist = true;
									break;
								}
							}
							if (!exist) {
								Video_Dirs.push_front(szBuf);
							}
						}
					}
					else if (pMsg->hwnd == ListDiscography.GetSafeHwnd()) {
						if (PathFileExistsW(szBuf) == TRUE)
						{
							bool exist = false;
							for (auto& i : Discography_Dirs) {
								if (Wcompare(i, szBuf)) {
									exist = true;
									break;
								}
							}
							if (!exist) {
								Discography_Dirs.push_front(szBuf);
							}
						}
					}
					else if (pMsg->hwnd == ListPhotoVideos.GetSafeHwnd()) {
						if (PathFileExistsW(szBuf) == TRUE)
						{
							bool exist = false;
							for (auto& i : Photo_Video_Dirs) {
								if (Wcompare(i, szBuf)) {
									exist = true;
									break;
								}
							}
							if (!exist) {
								Photo_Video_Dirs.push_front(szBuf);
							}
						}
					}
				}
			}
			ReloadContent();
		}
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}

void MediaServerMediaFoldersDialog::OnClose()
{
	EndDialog(0);
}