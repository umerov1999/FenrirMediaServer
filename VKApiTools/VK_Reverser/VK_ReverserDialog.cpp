#include "pch.h"
#include "VK_ReverserDialog.h"
#include "VK_ReverserServer.h"
#include "win_api_utils.h"
#include "WSTRUtils.h"
#include "config_json_parse.h"
#include "libimage.h"
#include "json.hpp"
#include <openssl/ssl.h>
#include "base.hpp"
#include "Map.h"
using namespace std;
using namespace WSTRUtils;
using namespace LIB_IMAGE;
using namespace nlohmann;

extern InitServer Startinit;
extern std::wstring ExtractAppPath();
extern VK_ReverserDialog dlgS;

extern SSL_CTX *initialize_ctx(bool print_sert);

THREAD_ACCESS_GUARD_OBJECT

static HANDLE hMessageMutex = NULL;
extern SOCKET HTTPSserver_sock;

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
extern USSL_CERT CertificateData;

#define UMagic "UCERT"
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

std::string tw_tostring(const TextWrite &input)
{
	std::string inp;
	inp.resize(sizeof(TextWrite));
	memcpy((void*)inp.data(), &input, sizeof(TextWrite));
	return inp;
}

void PrintMessage(const string &Msg, URGB Color)
{
	if (Msg.length() <= 0)
		return;
	WaitForSingleObject(hMessageMutex, INFINITE);
	TextWrite LineData(Color, (unsigned short)Msg.length());
	dlgS.Edk.AddLines(base91::encode(tw_tostring(LineData)) + " " + Msg);
	ReleaseMutex(hMessageMutex);
}

void PrintMessage(const wstring &Msg, URGB Color)
{
	if (Msg.length() <= 0)
		return;
	PrintMessage(wchar_to_UTF8(Msg), Color);
}

#define SERIALIZE_CONTROL(settings,object) settings.object = settings.GetComponent(object)
#define DESERIALIZE_CONTROL(settings,object) settings.SetComponent(object, settings.object)
#define JSON_SERIALIZE(jsont,object) serialize(jsont, #object, object)
#define JSON_DESERIALIZE(jsont,object) deserialize(jsont, #object, object)

class SettingsClass
{
public:
	SettingsClass()
	{

	}
	static bool GetComponent(CButton& checkbox)
	{
		return checkbox.GetCheck() > 0 ? true : false;
	}
	static string GetComponent(CEdit& edit)
	{
		CString ret;
		edit.GetWindowTextW(ret);
		return wchar_to_UTF8(ret.GetString());
	}
	static string GetComponent(CComboBox& cmb)
	{
		CString ret;
		cmb.GetWindowTextW(ret);
		return wchar_to_UTF8(ret.GetString());
	}
	static void SetComponent(CButton& checkbox, bool State)
	{
		checkbox.SetCheck(State == true ? TRUE : FALSE);
	}
	static void SetComponent(CEdit& edit, string State)
	{
		edit.SetWindowTextW(UTF8_to_wchar(State).c_str());
	}
	static void SetComponent(CComboBox& cmb, string State)
	{
		if (State.length() <= 0)
			return;
		cmb.SelectString(0, UTF8_to_wchar(State).c_str());
	}
	void SerializeSettings()
	{
		try {
			json settings(json::value_t::object);

			JSON_SERIALIZE(settings, HTTPSPort);
			JSON_SERIALIZE(settings, CurrentDir);
			JSON_SERIALIZE(settings, CertificateContent);

			json fin(json::value_t::object);
			fin.emplace("app_settings", settings);
			writeJsonConfig(ExtractAppPath() + L"\\" + PREFS_NAME, fin);
		}
		catch (json::exception e) {
			(win_message().message_type(MSG_TYPE::TYPE_ERROR).timeout(10) << e.what()).show();
			return;
		}
	}

	void DeSerializeSettings()
	{
		if (PathFileExistsW((ExtractAppPath() + L"\\" + PREFS_NAME).c_str()) == FALSE)
			return;
		try {
			json settings;
			if (parseJsonConfig(ExtractAppPath() + L"\\" + PREFS_NAME, settings)) {
				settings = settings.at("app_settings").get<json>();

				JSON_DESERIALIZE(settings, HTTPSPort);
				JSON_DESERIALIZE(settings, CurrentDir);
				JSON_DESERIALIZE(settings, CertificateContent);
			}
		}
		catch (json::exception e) {
			(win_message().message_type(MSG_TYPE::TYPE_ERROR).timeout(10) << e.what()).show();
			return;
		}
	}
private:
	template<typename T1>
	static void serialize(json& obj, const string &key, T1 value)
	{
		obj.emplace(key, value);
	}
	template<typename T1>
	static void serialize(json& obj, const string &key, const vector<T1> &value)
	{
		json arr = json(json::value_t::array);
		for (auto& i : value)
		{
			json elem = json(json::value_t::object);
			serialize(elem, i);
			arr.push_back(elem);
		}
		obj.emplace(key, arr);
	}
	static void serialize(json& obj, const string& key, const USSL_CERT& value)
	{
		json elem = json(json::value_t::object);
		if (value.Inited)
		{
			serialize(elem, "time_created", value.AddTime);
			serialize(elem, "is_inited", value.Inited);
			serialize(elem, "domain_name", value.Name);
			if (value.CertPart.exist(USSL_CERTPART_CERT) && !value.CertPart[USSL_CERTPART_CERT].empty()) {
				serialize(elem, "ssl_cert_base64", base64::encode(value.CertPart[USSL_CERTPART_CERT]));
			}
			if (value.CertPart.exist(USSL_CERTPART_BUNDLE) && !value.CertPart[USSL_CERTPART_BUNDLE].empty()) {
				serialize(elem, "ssl_ca_bundle_base64", base64::encode(value.CertPart[USSL_CERTPART_BUNDLE]));
			}
			if (value.CertPart.exist(USSL_CERTPART_KEY) && !value.CertPart[USSL_CERTPART_KEY].empty()) {
				serialize(elem, "ssl_key_base64", base64::encode(value.CertPart[USSL_CERTPART_KEY]));
			}
		}
		obj.emplace(key, elem);
	}
	template<typename T1>
	static void deserialize(const json& obj, const string &key, T1&value)
	{
		if (obj.find(key) != obj.end())
			value = obj.at(key).get<T1>();
	}

	template<typename T1>
	static void deserialize(const json& obj, const string &key, const vector<T1> &value)
	{
		if (obj.find(key) != obj.end() && obj.at(key).is_array())
		{
			json arr = obj.at(key);
			for (json::iterator it = arr.begin(); it != arr.end(); ++it)
			{
				json vll = it.value();
				T1 temp;
				deserialize(vll, temp);
				value.push_back(temp);
			}
		}
	}
	static void deserialize(const json& obj, const string &key, USSL_CERT& value)
	{
		if (obj.find(key) != obj.end())
		{
			json vl = obj.at(key);
			deserialize(vl, "time_created", value.AddTime);
			deserialize(vl, "is_inited", value.Inited);
			deserialize(vl, "domain_name", value.Name);
			deserialize(vl, "ssl_cert_base64", value.CertPart[USSL_CERTPART_CERT]);
			deserialize(vl, "ssl_ca_bundle_base64", value.CertPart[USSL_CERTPART_BUNDLE]);
			deserialize(vl, "ssl_key_base64", value.CertPart[USSL_CERTPART_KEY]);

			if (!value.CertPart[USSL_CERTPART_CERT].empty()) {
				value.CertPart[USSL_CERTPART_CERT] = base64::decode(value.CertPart[USSL_CERTPART_CERT]);
			}
			else {
				value.CertPart.erase(USSL_CERTPART_CERT);
			}
			if (!value.CertPart[USSL_CERTPART_BUNDLE].empty()) {
				value.CertPart[USSL_CERTPART_BUNDLE] = base64::decode(value.CertPart[USSL_CERTPART_BUNDLE]);
			}
			else {
				value.CertPart.erase(USSL_CERTPART_BUNDLE);
			}
			if (!value.CertPart[USSL_CERTPART_KEY].empty()) {
				value.CertPart[USSL_CERTPART_KEY] = base64::decode(value.CertPart[USSL_CERTPART_KEY]);
			}
			else {
				value.CertPart.erase(USSL_CERTPART_KEY);
			}
		}
	}
public:
	string HTTPSPort;
	string CurrentDir;
	USSL_CERT CertificateContent;
};
static SettingsClass pSettings;

static void ReadCert(const wstring &FilePath)
{
	win_message msg(dlgS.m_hWnd);
	msg.message_type(MSG_TYPE::TYPE_ERROR);
	FILE* ucert = NULL;
	if (_wfopen_s(&ucert, FilePath.c_str(), L"rb") != 0)
	{
		(msg << L"Ошибка: Невозможно открыть сертификат " << FilePath).show();
		return;
	}
	if (!ucert) {
		(msg << L"Ошибка: Невозможно открыть сертификат " << FilePath).show();
		return;
	}
	UCERTHeader hdr;
	fread(&hdr, sizeof(UCERTHeader), 1, ucert);
	if (memcmp(hdr.Magic, UMagic, strlen(UMagic)) != 0)
	{
		fclose(ucert);
		(msg << L"Повреждён сертификат " << FilePath).show();
		return;
	}
	CertificateData.CertPart.clear();
	string UTF8CertName;
	UTF8CertName.resize(hdr.utf8_domaine_name_length);
	fread((void*)UTF8CertName.data(), 1, hdr.utf8_domaine_name_length, ucert);

	CertificateData.Name = UTF8CertName;
	CertificateData.AddTime = wchar_to_UTF8(GetTimeAT(hdr.created));
	string TempBuf;
	
	TempBuf.resize(hdr.certificate_size);
	fread((void*)TempBuf.data(), 1, hdr.certificate_size, ucert);
	CertificateData.CertPart[USSL_CERTPART_CERT] = TempBuf;
	 
	if (hdr.ca_bundle_size > 0) {
		TempBuf.resize(hdr.ca_bundle_size);
		fread((void*)TempBuf.data(), 1, hdr.ca_bundle_size, ucert);
		CertificateData.CertPart[USSL_CERTPART_BUNDLE] = TempBuf;
	}

	TempBuf.resize(hdr.private_key_size);
	fread((void*)TempBuf.data(), 1, hdr.private_key_size, ucert);
	CertificateData.CertPart[USSL_CERTPART_KEY] = TempBuf;
	CertificateData.Inited = true;
	fclose(ucert);
	pSettings.CertificateContent = CertificateData;
	dlgS.SSLPathEd.SetWindowTextW(UTF8_to_wchar(CertificateData.Name + " (" + CertificateData.AddTime + ")").c_str());
	pSettings.SerializeSettings();
}

VK_ReverserDialog::VK_ReverserDialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(VK_ReverserDialog::IDD, pParent)
{
	memset(szBuf, 0, sizeof(szBuf));
	IsStart = false;
	m_hIcon = nullptr;
	m_hCursor = nullptr;
}

VK_ReverserDialog::~VK_ReverserDialog()
{
}

void VK_ReverserDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, HTTPSPort);
	DDX_Control(pDX, IDC_EDIT3, Edk);
	DDX_Control(pDX, IDC_EDIT2, CurrentDir);
	DDX_Control(pDX, IDC_EDIT4, SSLPathEd);
	DDX_Control(pDX, IDC_BUTTON1, StartBT);
	DDX_Control(pDX, IDC_BUTTON2, ButFolder);
}

BEGIN_MESSAGE_MAP(VK_ReverserDialog, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_SETCURSOR()
	ON_WM_QUERYDRAGICON()
	ON_WM_SETCURSOR()
	ON_WM_HSCROLL()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON1, OnStart)
	ON_BN_CLICKED(IDC_BUTTON2, ListPatch)
	ON_BN_CLICKED(IDC_BUTTON3, OnSelectSSL)
END_MESSAGE_MAP()

DWORD WINAPI InitVK_ReverserThread(LPVOID)
{
	win_message msg(dlgS.m_hWnd);
	msg.message_type(MSG_TYPE::TYPE_ERROR);

	dlgS.StartBT.EnableWindow(FALSE);
	dlgS.CurrentDir.SetReadOnly(TRUE);
	dlgS.HTTPSPort.SetReadOnly(TRUE);
	dlgS.ButFolder.EnableWindow(FALSE);
	dlgS.StartBT.ShowWindow(FALSE);
	dlgS.IsStart = true;

	PrintMessage(L"[Запуск: (" + GetTimeLocal() + L")]", URGB(0, 140, 255));
	InitVK_Reverser();
	dlgS.StartBT.SetWindowTextW(L"Остановлен");
	PrintMessage(L"|Fatal Error: (" + GetTimeLocal() + L")|", URGB(255, 0, 0));
	(msg << L"Из-за ошибки сервер остановлен!").show();
	goto out;
out:
	dlgS.IsStart = false;
	dlgS.OnClose();
	return TRUE;
}

BOOL VK_ReverserDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	hMessageMutex = CreateMutexW(NULL, FALSE, NULL);
	XTPSkinMgr()->AlphaEnableWindow(m_hWnd, 230);
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON1);
	m_hCursor = AfxGetApp()->LoadCursor(MAKEINTRESOURCEW(IDC_CURSOR1));
	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);
	THREAD_ACCESS_REGISTER_POINTERS(DEFAULT_GUARD_NAME, &Startinit);

	std::string DataPic = GetDataFromResourceUtil(L"SVG", IDB_SVG1);
	auto mk = Edk.getRect();
	Edk.Init(Align::LEFT_ALIGN, PrepareImageFromSVG(Edk.m_hWnd, mk.Width(), mk.Height(), DataPic.data(), (int)DataPic.size(), GetSysColor(COLOR_3DFACE)).get_hBitmap(true));

#ifdef _WIN64
	const wchar_t* ARCH = L"x64";
#elif _WIN32
	const wchar_t* ARCH = L"x86";
#else
#pragma error("Unknow Arch!")
#endif
	SetWindowTextW((wstring(L"ВК-Реверсер ") + L"[" + UTF8_to_wchar(SERVER_NAME) + L"] " + ARCH).c_str());

	pSettings.DeSerializeSettings();
	IsStart = false;
	CertificateData = pSettings.CertificateContent;
	if (CertificateData.Inited == true)
		SSLPathEd.SetWindowTextW(UTF8_to_wchar(CertificateData.Name + " (" + CertificateData.AddTime + ")").c_str());

	DESERIALIZE_CONTROL(pSettings, HTTPSPort);

	wstring CurrentPatch = UTF8_to_wchar(pSettings.CurrentDir);
	if (CurrentPatch.length() > 0 && PathFileExistsW(CurrentPatch.c_str()) == TRUE)
	{
		CurrentDir.SetWindowTextW(CurrentPatch.c_str());
		SetCurrentDirectoryW(CurrentPatch.c_str());
	}

	return TRUE;
}

void VK_ReverserDialog::OnSysCommand(UINT nID, LPARAM lParam)
{
	CDialogEx::OnSysCommand(nID, lParam);
}

void VK_ReverserDialog::OnPaint()
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

BOOL VK_ReverserDialog::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	::SetCursor(m_hCursor);
	return TRUE;
}

HCURSOR VK_ReverserDialog::OnQueryDragIcon()
{
	return (HCURSOR)m_hIcon;
}

static int CALLBACK BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
	if (uMsg == BFFM_INITIALIZED)
		SendMessageW(hwnd, BFFM_SETSELECTIONW, TRUE, lpData);
	return 0;
}

bool BrowseFolder(CString &saved_path)
{
	TCHAR path[MAX_PATH];
	BROWSEINFOW bi = { 0 };
	bi.lpszTitle = L"Выберите папку для хранения контента...";
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
		IMalloc * imalloc = 0;
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

void VK_ReverserDialog::ListPatch()
{
	CString Dri;
	CurrentDir.GetWindowTextW(Dri);
	if (BrowseFolder(Dri) == true)
	{
		if (Dri.GetLength() <= 0 || PathFileExistsW(Dri.GetString()) == FALSE)
		{
			SetCurrentDirectoryW(ExtractAppPath().c_str());
			CurrentDir.SetWindowTextW(L"");
		}
		else
		{
			SetCurrentDirectoryW(Dri.GetString());
			CurrentDir.SetWindowTextW(Dri.GetString());
		}
		SERIALIZE_CONTROL(pSettings, CurrentDir);
		pSettings.SerializeSettings();
	}
}

void VK_ReverserDialog::OnInitializeCustoms()
{
	SERIALIZE_CONTROL(pSettings, CurrentDir);
	if (pSettings.CurrentDir.length() <= 0 || PathFileExistsW(UTF8_to_wchar(pSettings.CurrentDir).c_str()) == FALSE)
	{
		SetCurrentDirectoryW(ExtractAppPath().c_str());
		CurrentDir.SetWindowTextW(L"");
	}
	else
		SetCurrentDirectoryW(UTF8_to_wchar(pSettings.CurrentDir).c_str());
	SERIALIZE_CONTROL(pSettings, CurrentDir);
}

void VK_ReverserDialog::OnStart()
{
	win_message msg(m_hWnd);
	msg.message_type(MSG_TYPE::TYPE_ERROR);
	OnInitializeCustoms();


	if (pSettings.CertificateContent.Inited == false)
	{
		(msg << L"Ошибка: Добавьте SSL сертификат").show();
		return;
	}
	SERIALIZE_CONTROL(pSettings, HTTPSPort);
	if (pSettings.HTTPSPort.length() <= 0)
		Startinit.HttpsPort = 0;
	else
		Startinit.HttpsPort = stoi(pSettings.HTTPSPort);
	if (Startinit.HttpsPort <= 0 || Startinit.HttpsPort > 65535)
	{
		(msg << L"Ошибка: Порт имеет значение (1:65535)").show();
		return;
	}
	pSettings.SerializeSettings();
	CreateThreadDetachedSimple(&InitVK_ReverserThread);
}

void VK_ReverserDialog::OnSelectSSL()
{
#if (WINVER >= 0x0400)
	DWORD dwFlags = OFN_EXPLORER | OFN_OVERWRITEPROMPT;
#else
	DWORD dwFlags = OFN_OVERWRITEPROMPT;
#endif
	CFileDialog fileDialog(TRUE, L"ucert", NULL, dwFlags, L"SSL UCERT|*.ucert|");
	int result = (int)fileDialog.DoModal();
	if (result != 1)
		return;
	ReadCert(fileDialog.GetPathName().GetString());
}

void VK_ReverserDialog::OnClose()
{
	if (HTTPSserver_sock != 0) {
		shutdown(HTTPSserver_sock, SD_BOTH);
		closesocket(HTTPSserver_sock);
	}
	XTPSkinMgr()->ExitProgramm();
}

BOOL VK_ReverserDialog::PreTranslateMessage(MSG* pMsg)
{
	if ((pMsg->hwnd == CurrentDir.GetSafeHwnd() || pMsg->hwnd == SSLPathEd.GetSafeHwnd()) && IsStart == false)
	{
		if (pMsg->message == WM_DROPFILES)
		{
			long ResultQueryFile = DragQueryFileW((HDROP)pMsg->wParam, 0xFFFFFFFF, NULL, 0);
			if (ResultQueryFile == 1)
			{
				ResultQueryFile = DragQueryFileW((HDROP)pMsg->wParam, 0, szBuf, sizeof(szBuf) / sizeof(wchar_t));
				if (pMsg->hwnd == CurrentDir.GetSafeHwnd())
					CurrentDir.SetWindowTextW(szBuf);
				else
					ReadCert(szBuf);
			}
		}
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}