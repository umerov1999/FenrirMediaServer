#include "pch.h"
#include "Hex2BinDialog.h"
#include "Shlobj.h"
#pragma comment (lib, "psapi.lib")

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

static char * GetANSIFromUTF16(const wchar_t * pUTF16)
{
	const long nCharacters = pUTF16 ? (long)wcslen(pUTF16) : 0;
	int nANSICharacters = (sizeof(wchar_t) * nCharacters);
	char * pANSI = new char[nANSICharacters + 1];
	memset(pANSI, 0, (nANSICharacters + 1) * sizeof(char));
	if (pUTF16)
#ifdef _WIN32
		WideCharToMultiByte(CP_ACP, 0, pUTF16, -1, pANSI, nANSICharacters, NULL, NULL);
#else
	{
		setlocale(LC_CTYPE, "");
		wcstombs(pANSI, pUTF16, nANSICharacters);
	}
#endif

	return (char *)pANSI;
}

Hex2BinDialog::Hex2BinDialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(Hex2BinDialog::IDD, pParent)
{
	m_hIcon = nullptr;
	memset(szBuf, 0, sizeof(szBuf));
}

Hex2BinDialog::~Hex2BinDialog()
{
}

void Hex2BinDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, Edk);
}

BEGIN_MESSAGE_MAP(Hex2BinDialog, CDialogEx)

	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_SETCURSOR()
	ON_WM_HSCROLL()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON1, OnSave)
	ON_BN_CLICKED(IDC_BUTTON2, OnOpen)
	ON_BN_CLICKED(IDC_BUTTON3, OnCopy)
END_MESSAGE_MAP()

BOOL Hex2BinDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	XTPSkinMgr()->AlphaEnableWindow(m_hWnd, 220);
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON1);
	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);

	Edk.SetLimitText(0);
	return TRUE;
}

void Hex2BinDialog::OnSysCommand(UINT nID, LPARAM lParam)
{
	CDialogEx::OnSysCommand(nID, lParam);
}

void Hex2BinDialog::OnPaint()
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

HCURSOR Hex2BinDialog::OnQueryDragIcon()
{
	return (HCURSOR)m_hIcon;
}

void Hex2BinDialog::HextToBin(const wchar_t*fl)
{
	FILE* flt = _wfopen(fl, L"wb");
	if (!flt)
	{
		MessageBoxW(L"Запись файла в этой папке невозможна!", L"Внимание!", MB_ICONINFORMATION);
		return;
	}

	CString Bughf;
	Edk.GetWindowTextW(Bughf);
	unsigned char bytet = 0;
	int nibble_index = 1;

	char*strbf = GetANSIFromUTF16((wchar_t*)Bughf.GetString());
	strbf = _strlwr(strbf);

	char cc[2];

	for (int i = 0; i < Bughf.GetLength(); i+=2)
	{
		unsigned char c = strbf[i];
		if ((c >= 'a' && c <= 'f') || (c >= '0' && c <= '9'))
		{
			memcpy(cc, strbf + i, 2);
			fprintf(flt, "%c", (unsigned char)strtol(cc, NULL, 16));
		}
		else
		{
			i--;
		}
	}
	free(strbf);
	fclose(flt);
	MessageBoxW(L"Успешно!", L"Внимание!", MB_ICONINFORMATION);
}

void Hex2BinDialog::BinToHex(const wchar_t*fl)
{
	FILE* flt = _wfopen(fl, L"rb");
	if (!flt)
	{
		MessageBoxW(L"Не возможно открыть файл!", L"Внимание!", MB_ICONINFORMATION);
		return;
	}
	fseek(flt, 0, SEEK_END);
	long fz = ftell(flt);
	fseek(flt, 0, SEEK_SET);
	char*bv = new char[fz];
	fread(bv, sizeof(char), fz, flt);
	fclose(flt);
	CString out = "\"";

	const wchar_t hex[] = L"0123456789abcdef";
	int posr = 0;

	for (int i = 0; i < fz; i++)
	{
		unsigned char c = bv[i];
		if (posr > 15)
		{
			posr = 0;
			out += L"\"\r\n\"\\x";
		}
		else
		{
			out += L"\\x";
		}
		out += hex[c >> 4];
		out += hex[c & 0xf];
		posr++;
	}

	out += "\"";

	Edk.SetWindowTextW(out.GetString());
	free(bv);
}

void Hex2BinDialog::OnOpen()
{
#if (WINVER >= 0x0400)
	DWORD dwFlags = OFN_EXPLORER | OFN_OVERWRITEPROMPT;
#else
	DWORD dwFlags = OFN_OVERWRITEPROMPT;
#endif
	CFileDialog fileDialog(TRUE, L"", NULL, dwFlags, L"Все файлы|*.*|");
	int result = (int)fileDialog.DoModal();
	if (result != 1)
		return;
	else
		BinToHex((wchar_t*)fileDialog.GetPathName().GetString());
}

void Hex2BinDialog::OnCopy()
{
	CString nmm;
	Edk.GetWindowTextW(nmm);
	OpenClipboard();
	EmptyClipboard();
	HGLOBAL hStrMem = GlobalAlloc(GMEM_MOVEABLE, (nmm.GetLength() +3) * sizeof(wchar_t));
	void* pStrMem = GlobalLock(hStrMem);
	memcpy(pStrMem, nmm.GetString(), nmm.GetLength() * sizeof(wchar_t));
	GlobalUnlock(pStrMem);
	SetClipboardData(CF_UNICODETEXT, hStrMem);
	CloseClipboard();
}

void Hex2BinDialog::OnSave()
{
#if (WINVER >= 0x0400)
	DWORD dwFlags = OFN_EXPLORER | OFN_OVERWRITEPROMPT;
#else
	DWORD dwFlags = OFN_OVERWRITEPROMPT;
#endif
	CFileDialog fileDialog(FALSE, L"bin", NULL, dwFlags, L"Бинарный файл|*.bin|");
	int result = (int)fileDialog.DoModal();
	if (result != 1)
		return;
	else
		HextToBin((wchar_t*)fileDialog.GetPathName().GetString());
}

void Hex2BinDialog::OnClose()
{
	EndDialog(FALSE);
}

BOOL Hex2BinDialog::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->hwnd == GetSafeHwnd())
	{
		if (pMsg->message == WM_DROPFILES)
		{
			long ResultQueryFile = DragQueryFileW((HDROP)pMsg->wParam, 0xFFFFFFFF, NULL, 0);
			if (ResultQueryFile == 1)
			{
				ResultQueryFile = DragQueryFileW((HDROP)pMsg->wParam, 0, szBuf, sizeof(szBuf));
				FILE*tmp = _wfopen(szBuf, L"rb");
				if (!tmp)
				{
					MessageBoxW(L"Невозможно открыть файл!", L"Внимание!", MB_ICONINFORMATION);
					return CDialogEx::PreTranslateMessage(pMsg);
				}
				else
				{
					fclose(tmp);
					BinToHex(szBuf);
				}
			}
		}
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}