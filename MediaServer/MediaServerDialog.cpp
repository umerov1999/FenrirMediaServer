#include "pch.h"
#include "MediaServerDialog.h"
#include "MediaServerMediaFoldersDialog.h"
#include "MediaServerServer.h"
#include "ThreadAccessGuard.h"
#include "WSTRUtils.h"
#include "libimage.h"
#include "json.hpp"
#include <openssl/ssl.h>
#include <list>
#include "base.hpp"
#include "Map.h"
using namespace std;
using namespace WSTRUtils;
using namespace LIB_IMAGE;
using namespace nlohmann;

extern InitServer Startinit;
extern std::wstring ExtractAppPath();
extern MediaServerDialog dlgS;
extern DWORD WINAPI doScanCovers(LPVOID);

extern list<wstring> Discography_Dirs;
extern list<wstring> Audio_Dirs;
extern list<wstring> Video_Dirs;
extern list<wstring> Photo_Video_Dirs;

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
	unsigned short utf8_domaine_name_length;
	unsigned int ca_bundle_size;
	unsigned int certificate_size;
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

void ClearMessages()
{
	WaitForSingleObject(hMessageMutex, INFINITE);
	dlgS.Edk.RemoveAllLines();
	ReleaseMutex(hMessageMutex);
}

static list<string> createUTF8List(const list<wstring> &data) {
	list<string>ret;
	for (auto& i : data) {
		ret.push_back(wchar_to_UTF8(i));
	}
	return ret;
}

static list<wstring> createWcharList(const list<string>& data) {
	list<wstring>ret;
	for (auto& i : data) {
		ret.push_back(UTF8_to_wchar(i));
	}
	return ret;
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
		isSsl = false;
		BIsDebug = true;
		OnlyNews = true;
		PhotosThumb = false;
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

			JSON_SERIALIZE(settings, isSsl);
			JSON_SERIALIZE(settings, BIsDebug);
			JSON_SERIALIZE(settings, OnlyNews);
			JSON_SERIALIZE(settings, PhotosThumb);
			JSON_SERIALIZE(settings, HTTPSPort);
			JSON_SERIALIZE(settings, HTTPPort);
			JSON_SERIALIZE(settings, ServerPassword);
			JSON_SERIALIZE(settings, CurrentDir);
			JSON_SERIALIZE(settings, CertificateContent);

			serialize_list(settings, "Discography_Dirs", createUTF8List(Discography_Dirs));
			serialize_list(settings, "Audio_Dirs", createUTF8List(Audio_Dirs));
			serialize_list(settings, "Video_Dirs", createUTF8List(Video_Dirs));
			serialize_list(settings, "Photo_Video_Dirs", createUTF8List(Photo_Video_Dirs));

			json fin(json::value_t::object);
			fin.emplace("app_settings", settings);

			FILE* save = NULL;
			if (_wfopen_s(&save, (ExtractAppPath() + L"\\" + SETTINGS).c_str(), L"wb") != 0)
				return;
			fwrite(UTF8START, 1, strlen(UTF8START), save);
			string sett = fin.dump(4);
			fwrite(sett.c_str(), 1, sett.length(), save);
			fclose(save);
		}
		catch (json::exception e) {
			(win_message().message_type(MSG_TYPE::TYPE_ERROR).timeout(10) << e.what()).show();
			return;
		}
	}

	void DeSerializeSettings()
	{
		if (PathFileExistsW((ExtractAppPath() + L"\\" + SETTINGS).c_str()) == FALSE)
			return;
		FILE* fl = NULL;
		if (_wfopen_s(&fl, (ExtractAppPath() + L"\\" + SETTINGS).c_str(), L"rb") == 0)
		{
			fseek(fl, 0, SEEK_END);
			int fsz = ftell(fl) - (int)strlen(UTF8START);
			fseek(fl, (int)strlen(UTF8START), SEEK_SET);
			string jsonna;
			jsonna.resize(fsz + 1);
			fread((char*)jsonna.data(), 1, fsz, fl);
			fclose(fl);
			json settings;
			try {
				settings = json::parse(jsonna);
				settings = settings.at("app_settings").get<json>();

				JSON_DESERIALIZE(settings, isSsl);
				JSON_DESERIALIZE(settings, BIsDebug);
				JSON_DESERIALIZE(settings, OnlyNews);
				JSON_DESERIALIZE(settings, PhotosThumb);
				JSON_DESERIALIZE(settings, HTTPSPort);
				JSON_DESERIALIZE(settings, HTTPPort);
				JSON_DESERIALIZE(settings, ServerPassword);
				JSON_DESERIALIZE(settings, CurrentDir);
				JSON_DESERIALIZE(settings, CertificateContent);

				list<string> dt;
				deserialize_list(settings, "Audio_Dirs", dt);
				Audio_Dirs = createWcharList(dt);

				list<string> udt;
				deserialize_list(settings, "Discography_Dirs", udt);
				Discography_Dirs = createWcharList(udt);

				list<string> dtv;
				deserialize_list(settings, "Video_Dirs", dtv);
				Video_Dirs = createWcharList(dtv);

				list<string> dtp;
				deserialize_list(settings, "Photo_Video_Dirs", dtp);
				Photo_Video_Dirs = createWcharList(dtp);
			}
			catch (json::exception e) {
				(win_message().message_type(MSG_TYPE::TYPE_ERROR).timeout(10) << e.what()).show();
				return;
			}
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
	static void serialize_list(json& obj, const string& key, const list<string>& value)
	{
		json arr = json(json::value_t::array);
		for (auto& i : value) {
			arr.push_back(i);
		}
		obj.emplace(key, arr);
	}
	static void serialize(json& obj, const string &key, const USSL_CERT &value)
	{
		json elem = json(json::value_t::object);
		if (value.Inited)
		{
			serialize(elem, "time_created", value.AddTime);
			serialize(elem, "is_inited", value.Inited);
			serialize(elem, "domain_name", value.Name);
			serialize(elem, "ssl_ca_cert_base64", base64::encode(value.CertPart[USSL_CERTPART_CERT]));
			serialize(elem, "ssl_ca_bundle_base64", base64::encode(value.CertPart[USSL_CERTPART_BUNDLE]));
			serialize(elem, "ssl_key_base64", base64::encode(value.CertPart[USSL_CERTPART_KEY]));
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
	static void deserialize(const json& obj, const string &key, vector<T1> &value)
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
	static void deserialize_list(const json& obj, const string& key, list<string>& value)
	{
		if (obj.find(key) != obj.end() && obj.at(key).is_array())
		{
			json arr = obj.at(key);
			for (json::iterator it = arr.begin(); it != arr.end(); ++it)
			{
				value.push_back(it.value().get<string>());
			}
		}
	}
	static void deserialize(const json& obj, const string &key, USSL_CERT& value)
	{
		if (obj.find(key) != obj.end())
		{
			json vl = obj.at(key);
			value.CertPart.resize(3);
			deserialize(vl, "time_created", value.AddTime);
			deserialize(vl, "is_inited", value.Inited);
			deserialize(vl, "domain_name", value.Name);
			deserialize(vl, "ssl_ca_cert_base64", value.CertPart[USSL_CERTPART_CERT]);
			deserialize(vl, "ssl_ca_bundle_base64", value.CertPart[USSL_CERTPART_BUNDLE]);
			deserialize(vl, "ssl_key_base64", value.CertPart[USSL_CERTPART_KEY]);

			value.CertPart[USSL_CERTPART_CERT] = base64::decode(value.CertPart[USSL_CERTPART_CERT]);
			value.CertPart[USSL_CERTPART_BUNDLE] = base64::decode(value.CertPart[USSL_CERTPART_BUNDLE]);
			value.CertPart[USSL_CERTPART_KEY] = base64::decode(value.CertPart[USSL_CERTPART_KEY]);
		}
	}
public:
	string HTTPSPort;
	string HTTPPort;
	string CurrentDir;
	string ServerPassword;
	bool isSsl;
	bool BIsDebug;
	bool OnlyNews;
	bool PhotosThumb;
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
	CertificateData.AddTime = wchar_to_UTF8(GetTimeLocal());
	string TempBuf;
	TempBuf.resize(hdr.ca_bundle_size);
	fread((void*)TempBuf.data(), 1, hdr.ca_bundle_size, ucert);
	CertificateData.CertPart.push_back(TempBuf);

	TempBuf.resize(hdr.certificate_size);
	fread((void*)TempBuf.data(), 1, hdr.certificate_size, ucert);
	CertificateData.CertPart.push_back(TempBuf);

	TempBuf.resize(hdr.private_key_size);
	fread((void*)TempBuf.data(), 1, hdr.private_key_size, ucert);
	CertificateData.CertPart.push_back(TempBuf);
	CertificateData.Inited = true;
	fclose(ucert);
	pSettings.CertificateContent = CertificateData;
	dlgS.SSLPathEd.SetWindowTextW(UTF8_to_wchar(CertificateData.Name + " (" + CertificateData.AddTime + ")").c_str());
	pSettings.SerializeSettings();
}

bool RegSetSettingsSTR(const wstring& Patch, const wstring& PatchReg, const wstring& NameReg)
{
	DWORD pcbData = (DWORD)Patch.length() * sizeof(wchar_t);
	HKEY hKey;

	if (RegCreateKeyW(HKEY_CURRENT_USER, PatchReg.c_str(), &hKey) == ERROR_SUCCESS)
	{
		if (RegSetValueExW(hKey, NameReg.c_str(), NULL, REG_SZ, (BYTE*)Patch.c_str(), pcbData) == ERROR_SUCCESS)
		{
			RegCloseKey(hKey);
			return true;
		}
		RegCloseKey(hKey);
	}
	return false;
}

wstring RegGetSettingsSTR(const wstring& NameReg, const wstring& PatchReg)
{
	HKEY hKey;
	const DWORD pcbData = MAX_PATH * sizeof(wchar_t);
	wchar_t Patch[pcbData];
	memset(Patch, 0, pcbData * sizeof(wchar_t));
	if (RegOpenKeyW(HKEY_CURRENT_USER, PatchReg.c_str(), &hKey) == ERROR_SUCCESS)
	{
		if (RegQueryValueExW(hKey, NameReg.data(), NULL, NULL, (LPBYTE)Patch, (LPDWORD)&pcbData) == ERROR_SUCCESS)
		{
			RegCloseKey(hKey);
			return Patch;
		}
	}
	RegCloseKey(hKey);
	return L"";
}

void RegDelKeyValue(const wstring& NameReg, const wstring& PatchReg)
{
	HKEY hKey;
	LSTATUS up;
	up = RegOpenKeyW(HKEY_CURRENT_USER, PatchReg.c_str(), &hKey);
	if (up == ERROR_SUCCESS)
	{
		RegDeleteValueW(hKey, NameReg.c_str());
		RegCloseKey(hKey);
	}
}

MediaServerDialog::MediaServerDialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(MediaServerDialog::IDD, pParent)
{
}

MediaServerDialog::~MediaServerDialog()
{
}

void MediaServerDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, HTTPSPort);
	DDX_Control(pDX, IDC_EDIT2, CurrentDir);
	DDX_Control(pDX, IDC_EDIT3, Edk);
	DDX_Control(pDX, IDC_EDIT4, SSLPathEd);
	DDX_Control(pDX, IDC_EDIT5, HTTPPort);
	DDX_Control(pDX, IDC_EDIT6, ServerPassword);
	DDX_Control(pDX, IDC_BUTTON1, StartBT);
	DDX_Control(pDX, IDC_BUTTON2, ButFolder);
	DDX_Control(pDX, IDC_BUTTON3, SelectSSL);
	DDX_Control(pDX, IDC_BUTTON4, ScanCovers);
	DDX_Control(pDX, IDC_BUTTON5, MediaFolders);
	DDX_Control(pDX, IDC_BUTTON6, BAutoStart);
	DDX_Control(pDX, IDC_RADIO1, BUseHttps);
	DDX_Control(pDX, IDC_RADIO2, BUseHttp);
	DDX_Control(pDX, IDC_CHECK1, BIsDebug);
	DDX_Control(pDX, IDC_CHECK2, OnlyNews);
	DDX_Control(pDX, IDC_CHECK3, CanEdit);
	DDX_Control(pDX, IDC_CHECK4, PhotosThumb);
}

BEGIN_MESSAGE_MAP(MediaServerDialog, CDialogEx)
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
	ON_BN_CLICKED(IDC_BUTTON4, OnScanCovers)
	ON_BN_CLICKED(IDC_BUTTON5, OnMediaFolders)
	ON_BN_CLICKED(IDC_BUTTON6, OnAutostart)
	ON_BN_CLICKED(IDC_RADIO1, OnProtocol)
	ON_BN_CLICKED(IDC_RADIO2, OnProtocol)
END_MESSAGE_MAP()

DWORD WINAPI InitMediaServerThread(LPVOID)
{
	win_message msg(dlgS.m_hWnd);
	msg.message_type(MSG_TYPE::TYPE_ERROR);

	dlgS.StartBT.EnableWindow(FALSE);
	dlgS.CurrentDir.SetReadOnly(TRUE);
	dlgS.HTTPSPort.SetReadOnly(TRUE);
	dlgS.HTTPPort.SetReadOnly(TRUE);
	dlgS.ButFolder.EnableWindow(FALSE);
	dlgS.ButSSL.EnableWindow(FALSE);
	dlgS.StartBT.ShowWindow(FALSE);
	dlgS.SelectSSL.ShowWindow(FALSE);
	dlgS.BUseHttp.EnableWindow(FALSE);
	dlgS.BUseHttps.EnableWindow(FALSE);
	dlgS.ServerPassword.SetWindowTextW(L"******************");
	dlgS.ServerPassword.EnableWindow(FALSE);
	dlgS.ScanCovers.EnableWindow(FALSE);
	dlgS.MediaFolders.EnableWindow(FALSE);
	dlgS.BIsDebug.EnableWindow(FALSE);
	dlgS.OnlyNews.EnableWindow(FALSE);
	dlgS.PhotosThumb.EnableWindow(FALSE);
	dlgS.CanEdit.EnableWindow(FALSE);
	dlgS.IsStart = true;

	ClearMessages();

	PrintMessage(L"[Запуск: (" + GetTimeLocal() + L")]", URGB(0, 140, 255));
	InitMediaServer();
	dlgS.StartBT.SetWindowTextW(L"Остановлен");
	PrintMessage(L"|Fatal Error: (" + GetTimeLocal() + L")|", URGB(255, 0, 0));
	(msg << L"Из-за ошибки сервер остановлен!").show();
	goto out;
out:
	dlgS.IsStart = false;
	dlgS.OnClose();
	return TRUE;
}

void MediaServerDialog::OnAutostart()
{
	if (IsAutostart == false)
	{
		BAutoStart.SetWindowTextW(L"Отключить");
		IsAutostart = true;

		wchar_t des[MAX_PATH];
		GetModuleFileNameW(NULL, des, MAX_PATH);
		wchar_t tmpcov[MAX_PATH];
		swprintf_s(tmpcov, L"\"%s\" --autostart", des);
		RegSetSettingsSTR(tmpcov, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", L"MediaServer");
	}
	else
	{
		RegDelKeyValue(L"MediaServer", L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run");
		BAutoStart.SetWindowTextW(L"Автозапуск");
		IsAutostart = false;
	}
}

DWORD WINAPI AutoStartServer(LPVOID)
{
	dlgS.StartBT.EnableWindow(FALSE);
	for (int i = 6; i >= 0; i--)
	{
		dlgS.StartBT.SetWindowTextW((L"(" + to_wstring(i) + L")").c_str());
		Sleep(1000);
	}
	dlgS.OnStart();
	dlgS.StartBT.EnableWindow(TRUE);
	return 0;
}

BOOL MediaServerDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CanEdit.SetCheck(FALSE);

	NeedAutoStart = false;
	int argcou = 0;
	LPWSTR* CommandLns = CommandLineToArgvW(XTPSkinMgr()->GetCommandLineTool().GetString(), &argcou);
	if (argcou > 1)
	{
		wchar_t* argvar = CommandLns[1];
		if (wcscmp(argvar, L"--autostart") == 0)
			NeedAutoStart = true;
	}

	ScanCovers.EnableWindow(PathFileExistsW((ExtractAppPath() + L"\\" + L"ffmpeg.exe").c_str()));

	hMessageMutex = CreateMutexW(NULL, FALSE, NULL);
	XTPSkinMgr()->AlphaEnableWindow(m_hWnd, 230);
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON1);
	m_hCursor = AfxGetApp()->LoadCursor(MAKEINTRESOURCEW(IDC_CURSOR1));
	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);
	THREAD_ACCESS_REGISTER_POINTERS(DEFAULT_GUARD_NAME, &Startinit);

	std::string DataPic = GetDataFromResourceUtil(L"PNG", IDB_PNG1);
	Edk.Init(Align::LEFT_ALIGN, PrepareImageFromBufferAutoType(Edk.m_hWnd, DataPic.data(), (int)DataPic.size()).get_hBitmap(true));

#ifdef _WIN64
	const wchar_t* ARCH = L"x64";
#elif _WIN32
	const wchar_t* ARCH = L"x86";
#else
#pragma error("Unknow Arch!")
#endif
	SetWindowTextW((wstring(L"Медиа Сервер ") + L"[" + UTF8_to_wchar(SERVER_NAME) + L"] " + ARCH).c_str());

	pSettings.DeSerializeSettings();
	IsStart = false;
	CertificateData = pSettings.CertificateContent;
	if (CertificateData.Inited == true)
		SSLPathEd.SetWindowTextW(UTF8_to_wchar(CertificateData.Name + " (" + CertificateData.AddTime + ")").c_str());

	pSettings.isSsl ? (BUseHttps.SetCheck(TRUE), BUseHttp.SetCheck(FALSE)) : (BUseHttps.SetCheck(FALSE), BUseHttp.SetCheck(TRUE));
	OnProtocol();
	DESERIALIZE_CONTROL(pSettings, BIsDebug);
	DESERIALIZE_CONTROL(pSettings, OnlyNews);
	DESERIALIZE_CONTROL(pSettings, PhotosThumb);
	DESERIALIZE_CONTROL(pSettings, HTTPSPort);
	DESERIALIZE_CONTROL(pSettings, HTTPPort);
	DESERIALIZE_CONTROL(pSettings, ServerPassword);

	wstring CurrentPatch = UTF8_to_wchar(pSettings.CurrentDir);
	if (CurrentPatch.length() > 0 && PathFileExistsW(CurrentPatch.c_str()) == TRUE)
	{
		CurrentDir.SetWindowTextW(CurrentPatch.c_str());
		SetCurrentDirectoryW(CurrentPatch.c_str());
	}

	if (RegGetSettingsSTR(L"MediaServer", L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run").length() > 0)
	{
		BAutoStart.SetWindowTextW(L"Отключить");
		IsAutostart = true;
	}
	else
		IsAutostart = false;

	if (NeedAutoStart == true)
		CreateThread(NULL, NULL, &AutoStartServer, NULL, NULL, NULL);

	return TRUE;
}

void MediaServerDialog::OnProtocol() {
	if (BUseHttp.GetCheck() == TRUE) {
		HTTPSPort.EnableWindow(FALSE);
		SSLPathEd.EnableWindow(FALSE);
		HTTPPort.EnableWindow(TRUE);
		SelectSSL.EnableWindow(FALSE);
	}
	else {
		HTTPSPort.EnableWindow(TRUE);
		SSLPathEd.EnableWindow(TRUE);
		HTTPPort.EnableWindow(FALSE);
		SelectSSL.EnableWindow(TRUE);
	}
}

void MediaServerDialog::OnSysCommand(UINT nID, LPARAM lParam)
{
	CDialogEx::OnSysCommand(nID, lParam);
}

void MediaServerDialog::OnPaint()
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

BOOL MediaServerDialog::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	::SetCursor(m_hCursor);
	return TRUE;
}

HCURSOR MediaServerDialog::OnQueryDragIcon()
{
	return (HCURSOR)m_hIcon;
}

static int CALLBACK BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
	if (uMsg == BFFM_INITIALIZED)
		SendMessageW(hwnd, BFFM_SETSELECTIONW, TRUE, lpData);
	return 0;
}

static bool BrowseFolder(CString &saved_path)
{
	TCHAR path[MAX_PATH];
	BROWSEINFOW bi = { 0 };
	bi.lpszTitle = L"Выберите папку для хранения контента...";
	bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;
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

void MediaServerDialog::ListPatch()
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

void MediaServerDialog::OnInitializeCustoms()
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

static void cleanNonExist() {
	for (auto i = Audio_Dirs.begin(); i != Audio_Dirs.end(); i++) {
		if (!PathFileExistsW((*i).c_str())) {
			auto tmp = i;
			i--;
			Audio_Dirs.erase(tmp);
		}
	}

	for (auto i = Discography_Dirs.begin(); i != Discography_Dirs.end(); i++) {
		if (!PathFileExistsW((*i).c_str())) {
			auto tmp = i;
			i--;
			Discography_Dirs.erase(tmp);
		}
	}

	for (auto i = Video_Dirs.begin(); i != Video_Dirs.end(); i++) {
		if (!PathFileExistsW((*i).c_str())) {
			auto tmp = i;
			i--;
			Video_Dirs.erase(tmp);
		}
	}

	for (auto i = Photo_Video_Dirs.begin(); i != Photo_Video_Dirs.end(); i++) {
		if (!PathFileExistsW((*i).c_str())) {
			auto tmp = i;
			i--;
			Photo_Video_Dirs.erase(tmp);
		}
	}
}

void MediaServerDialog::OnStart()
{
	cleanNonExist();
	if (Audio_Dirs.empty() && Video_Dirs.empty() && Discography_Dirs.empty() && Photo_Video_Dirs.empty()) {
		OnMediaFolders();
		return;
	}
	win_message msg(m_hWnd);
	msg.message_type(MSG_TYPE::TYPE_ERROR);
	OnInitializeCustoms();

	if (BUseHttp.GetCheck() == TRUE) {
		pSettings.isSsl = false;
	}
	else if (BUseHttps.GetCheck() == TRUE) {
		pSettings.isSsl = true;
	}
	Startinit.canEdit = CanEdit.GetCheck() == TRUE;
	Startinit.isSsl = pSettings.isSsl;
	SERIALIZE_CONTROL(pSettings, ServerPassword);
	SERIALIZE_CONTROL(pSettings, BIsDebug);
	SERIALIZE_CONTROL(pSettings, OnlyNews);
	SERIALIZE_CONTROL(pSettings, PhotosThumb);
	Startinit.isDebug = pSettings.BIsDebug;
	pSettings.ServerPassword = trim(pSettings.ServerPassword);
	if (pSettings.ServerPassword.length() <= 0)
	{
		(msg << L"Ошибка: Придумайте пароль для сервера").show();
		return;
	}
	Startinit.password = pSettings.ServerPassword;
	if (pSettings.isSsl) {
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
	}
	else {
		SERIALIZE_CONTROL(pSettings, HTTPPort);
		if (pSettings.HTTPPort.length() <= 0)
			Startinit.HttpPort = 0;
		else
			Startinit.HttpPort = stoi(pSettings.HTTPPort);
		if (Startinit.HttpPort <= 0 || Startinit.HttpPort > 65535)
		{
			(msg << L"Ошибка: Порт имеет значение (1:65535)").show();
			return;
		}
	}
	pSettings.SerializeSettings();
	CreateThread(NULL, NULL, &InitMediaServerThread, NULL, NULL, NULL);
}

void MediaServerDialog::OnMediaFolders() {
	cleanNonExist();
	MediaServerMediaFoldersDialog mdf;
	mdf.DoModal();
}

void MediaServerDialog::OnScanCovers() {
	if (!PathFileExistsW((ExtractAppPath() + L"\\" + L"ffmpeg.exe").c_str())) {
		(win_message(m_hWnd).message_type(MSG_TYPE::TYPE_ERROR) << L"Отсутствует ffmpeg.exe").show();
		return;
	}
	cleanNonExist();
	if (Audio_Dirs.empty() && Video_Dirs.empty() && Discography_Dirs.empty() && Photo_Video_Dirs.empty()) {
		OnMediaFolders();
		return;
	}
	CreateThread(NULL, NULL, &doScanCovers, NULL, NULL, NULL);
}

void MediaServerDialog::OnSelectSSL()
{
	CFileDialog fileDialog(TRUE, L"ucert", NULL, NULL, L"SSL UCERT|*.ucert|");
	int result = (int)fileDialog.DoModal();
	if (result != 1)
		return;
	ReadCert(fileDialog.GetPathName().GetString());
}

void MediaServerDialog::OnClose()
{
	PrintMessage(L"[Выход: (" + GetTimeLocal() + L")]", URGB(255, 0, 0));
	WaitForSingleObject(hMessageMutex, INFINITE);
	if(HTTPSserver_sock != 0)
		closesocket(HTTPSserver_sock);
	XTPSkinMgr()->ExitProgramm();
}

BOOL MediaServerDialog::PreTranslateMessage(MSG* pMsg)
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