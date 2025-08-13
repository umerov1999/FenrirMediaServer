#include "pch.h"
#include "MakeSSLDialog.h"
#include <vector>
#define UMagic "UCERT"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
using namespace WSTRUtils;

#ifndef WS_EX_LAYERED 
#define WS_EX_LAYERED 0x00080000 
#define LWA_COLORKEY 0x00000001 
#define LWA_ALPHA 0x00000002 
#endif

#pragma pack(push,1)
struct UCERTHeader
{
	char Magic[10];
	time_t created;
	unsigned short utf8_domaine_name_length;
	unsigned int certificate_size;
	unsigned int ca_bundle_size;
	unsigned int private_key_size;
};
#pragma pack(pop)

MakeSSLDialog::MakeSSLDialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(MakeSSLDialog::IDD, pParent)
{
	memset(szBuf, 0, sizeof(szBuf));
	m_hIcon = nullptr;
}

MakeSSLDialog::~MakeSSLDialog()
{
}

void MakeSSLDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MFCEDITBROWSE1, File1);
	DDX_Control(pDX, IDC_MFCEDITBROWSE2, File2);
	DDX_Control(pDX, IDC_MFCEDITBROWSE3, File3);
	DDX_Control(pDX, IDC_EDIT1, EDomaine);
}

BEGIN_MESSAGE_MAP(MakeSSLDialog, CDialogEx)

	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_SETCURSOR()
	ON_WM_HSCROLL()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON1, OnGen)
END_MESSAGE_MAP()

BOOL MakeSSLDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	XTPSkinMgr()->AlphaEnableWindow(m_hWnd, 220);
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON1);
	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);

	return TRUE;
}

void MakeSSLDialog::OnSysCommand(UINT nID, LPARAM lParam)
{
	CDialogEx::OnSysCommand(nID, lParam);
}

void MakeSSLDialog::OnPaint()
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

void MakeSSLDialog::OnGen()
{
	win_message msg(m_hWnd);
	msg.message_type(MSG_TYPE::TYPE_ERROR);

	CString Cert1Str;
	CString Cert2Str;
	CString Cert3Str;
	CString DomaineNameStr;

	File1.GetWindowTextW(Cert1Str);
	File2.GetWindowTextW(Cert2Str);
	File3.GetWindowTextW(Cert3Str);
	EDomaine.GetWindowTextW(DomaineNameStr);
	if (Cert1Str.GetLength() <= 0 || Cert3Str.GetLength() <= 0 || DomaineNameStr.GetLength() <= 0)
	{
		(msg << L"Не все поля заполнены!").show();
		return;
	}
	std::wstring FlNm = DomaineNameStr.GetString();
	size_t Fnd = 0;
	while ((Fnd = FlNm.find('.')) != std::string::npos)
		FlNm.replace(Fnd, 1, L"_");
#if (WINVER >= 0x0400)
	DWORD dwFlags = OFN_EXPLORER | OFN_OVERWRITEPROMPT;
#else
	DWORD dwFlags = OFN_OVERWRITEPROMPT;
#endif
	CFileDialog fileDialog(FALSE, L"ucert", FlNm.c_str(), dwFlags, L"SSL UCERT|*.ucert|");
	int result = (int)fileDialog.DoModal();
	if (result != 1)
		return;
	FILE*ucert = NULL, *cert1 = NULL, *cert2 = NULL, *cert3 = NULL;
	if (_wfopen_s(&ucert, fileDialog.GetPathName().GetString(), L"wb") != 0 || _wfopen_s(&cert1, Cert1Str.GetString(), L"rb") != 0 || _wfopen_s(&cert3, Cert3Str.GetString(), L"rb") != 0)
	{
		if (ucert)
			fclose(ucert);
		if (cert1)
			fclose(cert1);
		if (cert3)
			fclose(cert3);
		(msg << L"Нет доступа к файлам!").show();
		return;
	}
	_wfopen_s(&cert2, Cert2Str.GetString(), L"rb");

	fseek(cert1, 0, SEEK_END);
	if (cert2 != nullptr) {
		fseek(cert2, 0, SEEK_END);
	}
	fseek(cert3, 0, SEEK_END);
	std::vector<char>Cer1Data(ftell(cert1)), Cer3Data(ftell(cert3));
	std::vector<char>Cer2Data;
	if (cert2 != nullptr) {
		Cer2Data.resize(ftell(cert2));
		fseek(cert2, 0, SEEK_SET);
	}
	fseek(cert1, 0, SEEK_SET);
	fseek(cert3, 0, SEEK_SET);

	fread(Cer1Data.data(), 1, Cer1Data.size(), cert1);
	if (cert2 != nullptr) {
		fread(Cer2Data.data(), 1, Cer2Data.size(), cert2);
	}
	fread(Cer3Data.data(), 1, Cer3Data.size(), cert3);

	fclose(cert1);
	if (cert2 != nullptr) {
		fclose(cert2);
	}
	fclose(cert3);

	UCERTHeader hdr;
	memset(&hdr, 0, sizeof(UCERTHeader));
	memcpy(hdr.Magic, UMagic, strlen(UMagic));
	std::string UName = wchar_to_UTF8(DomaineNameStr.GetString());
	hdr.created = time(0);
	hdr.utf8_domaine_name_length = (unsigned short)UName.length();
	hdr.certificate_size = (unsigned short)Cer1Data.size();
	hdr.ca_bundle_size = (unsigned short)Cer2Data.size();
	hdr.private_key_size = (unsigned short)Cer3Data.size();
	fwrite(&hdr, sizeof(UCERTHeader), 1, ucert);
	fwrite(UName.data(), 1, UName.length(), ucert);
	fwrite(Cer1Data.data(), 1, Cer1Data.size(), ucert);
	if (!Cer2Data.empty()) {
		fwrite(Cer2Data.data(), 1, Cer2Data.size(), ucert);
	}
	fwrite(Cer3Data.data(), 1, Cer3Data.size(), ucert);
	fclose(ucert);
	(msg.message_type(MSG_TYPE::TYPE_INFO) << L"Успешно!").show();
}

HCURSOR MakeSSLDialog::OnQueryDragIcon()
{
	return (HCURSOR)m_hIcon;
}

void MakeSSLDialog::OnClose()
{
	_exit(0);
}

BOOL MakeSSLDialog::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->hwnd == File1.GetSafeHwnd() || pMsg->hwnd == File2.GetSafeHwnd() || pMsg->hwnd == File3.GetSafeHwnd())
	{
		if (pMsg->message == WM_DROPFILES)
		{
			long ResultQueryFile = DragQueryFileW((HDROP)pMsg->wParam, 0xFFFFFFFF, NULL, 0);
			if (ResultQueryFile == 1)
			{
				ResultQueryFile = DragQueryFileW((HDROP)pMsg->wParam, 0, szBuf, sizeof(szBuf) / sizeof(wchar_t));
				if (pMsg->hwnd == File1.GetSafeHwnd())
					File1.SetWindowTextW(szBuf);
				else if(pMsg->hwnd == File2.GetSafeHwnd())
					File2.SetWindowTextW(szBuf);
				else
					File3.SetWindowTextW(szBuf);
			}
		}
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}