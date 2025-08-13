#include "pch.h"
#include "ExchengeTokenToolDialog.h"
#include "win_api_utils.h"
#include "WSTRUtils.h"
#include "config_json_parse.h"
#include "json.hpp"
#include "vk_api_interface.h"
#include <regex>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
using namespace std;
using namespace WSTRUtils;
using namespace nlohmann;

extern std::wstring ExtractAppPath();

#define UTF8START "\xef\xbb\xbf"

#ifndef WS_EX_LAYERED 
#define WS_EX_LAYERED 0x00080000 
#define LWA_COLORKEY 0x00000001 
#define LWA_ALPHA 0x00000002 
#endif

ExchengeTokenToolDialog::ExchengeTokenToolDialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(ExchengeTokenToolDialog::IDD, pParent)
{
	m_hIcon = nullptr;
}

ExchengeTokenToolDialog::~ExchengeTokenToolDialog()
{
}

void ExchengeTokenToolDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, AccessToken);
	DDX_Control(pDX, IDC_EDIT2, ExchengeToken);
	DDX_Control(pDX, IDC_EDIT3, DeviceName);
	DDX_Control(pDX, IDC_EDIT4, DeviceId);
	DDX_Control(pDX, IDC_CHECK1, UseDeviceName);
	DDX_Control(pDX, IDC_CHECK2, UseDeviceId);
}

BEGIN_MESSAGE_MAP(ExchengeTokenToolDialog, CDialogEx)

	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_SETCURSOR()
	ON_WM_HSCROLL()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON1, OnToken2Exchenge)
	ON_BN_CLICKED(IDC_BUTTON2, OnExchenge2Token)
	ON_BN_CLICKED(IDC_CHECK1, OnCheckBox)
	ON_BN_CLICKED(IDC_CHECK2, OnCheckBox)
END_MESSAGE_MAP()

#define SERIALIZE_CONTROL(settings,object) settings.object = settings.GetComponent(object)
#define DESERIALIZE_CONTROL(settings,object) settings.SetComponent(object, settings.object)
#define JSON_SERIALIZE(jsont,object) serialize(jsont, #object, object)
#define JSON_DESERIALIZE(jsont,object) deserialize(jsont, #object, object)

class SettingsClass
{
public:
	SettingsClass()
	{
		UseDeviceName = false;
		UseDeviceId = false;
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

			JSON_SERIALIZE(settings, AccessToken);
			JSON_SERIALIZE(settings, ExchengeToken);
			JSON_SERIALIZE(settings, DeviceName);
			JSON_SERIALIZE(settings, DeviceId);
			JSON_SERIALIZE(settings, UseDeviceName);
			JSON_SERIALIZE(settings, UseDeviceId);

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

				JSON_DESERIALIZE(settings, AccessToken);
				JSON_DESERIALIZE(settings, ExchengeToken);
				JSON_DESERIALIZE(settings, DeviceName);
				JSON_DESERIALIZE(settings, DeviceId);
				JSON_DESERIALIZE(settings, UseDeviceName);
				JSON_DESERIALIZE(settings, UseDeviceId);
			}
		}
		catch (json::exception e) {
			(win_message().message_type(MSG_TYPE::TYPE_ERROR).timeout(10) << e.what()).show();
			return;
		}
	}
private:
	template<typename T1>
	static void serialize(json& obj, const string& key, T1 value)
	{
		obj.emplace(key, value);
	}
	template<typename T1>
	static void serialize(json& obj, const string& key, const vector<T1>& value)
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
	template<typename T1>
	static void deserialize(const json& obj, const string& key, T1& value)
	{
		if (obj.find(key) != obj.end())
			value = obj.at(key).get<T1>();
	}

	template<typename T1>
	static void deserialize(const json& obj, const string& key, const vector<T1>& value)
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
public:
	string AccessToken;
	string ExchengeToken;

	string DeviceName;
	string DeviceId;

	bool UseDeviceName;
	bool UseDeviceId;
};
static SettingsClass pSettings;

BOOL ExchengeTokenToolDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	XTPSkinMgr()->AlphaEnableWindow(m_hWnd, 220);
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON1);
	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);

	pSettings.DeSerializeSettings();

	DESERIALIZE_CONTROL(pSettings, AccessToken);
	DESERIALIZE_CONTROL(pSettings, ExchengeToken);
	DESERIALIZE_CONTROL(pSettings, DeviceName);
	DESERIALIZE_CONTROL(pSettings, DeviceId);
	DESERIALIZE_CONTROL(pSettings, UseDeviceName);
	DESERIALIZE_CONTROL(pSettings, UseDeviceId);

	OnCheckBox();
	return TRUE;
}

void ExchengeTokenToolDialog::OnSysCommand(UINT nID, LPARAM lParam)
{
	CDialogEx::OnSysCommand(nID, lParam);
}

void ExchengeTokenToolDialog::OnPaint()
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

HCURSOR ExchengeTokenToolDialog::OnQueryDragIcon()
{
	return (HCURSOR)m_hIcon;
}

void ExchengeTokenToolDialog::OnClose()
{
	SERIALIZE_CONTROL(pSettings, AccessToken);
	SERIALIZE_CONTROL(pSettings, ExchengeToken);
	SERIALIZE_CONTROL(pSettings, DeviceName);
	SERIALIZE_CONTROL(pSettings, DeviceId);
	SERIALIZE_CONTROL(pSettings, UseDeviceName);
	SERIALIZE_CONTROL(pSettings, UseDeviceId);
	pSettings.SerializeSettings();

	_exit(0);
}

bool VK_APIMETHOD::VK_APIMETHOD_doCapcha(const std::string& captcha_img, const std::string& user_agent, std::string& code) {
	win_message msg(NULL);
	msg.message_type(MSG_TYPE::TYPE_ERROR);
	(msg << "Требуется капча!").show();

	return false;
}

void VK_APIMETHOD::VK_APIMETHOD_doLimit(const std::string& Method) {
	win_message msg(NULL);
	msg.message_type(MSG_TYPE::TYPE_ERROR);
	(msg << "    #VK_APIMETHOD[error] " + wchar_to_UTF8(WSTRUtils::GetTimeLocal()) + " " + Method + (const char*)u8" Ошибка 29: лимит на использование").show();
}

void ExchengeTokenToolDialog::OnToken2Exchenge()
{
	win_message msg(m_hWnd);
	msg.message_type(MSG_TYPE::TYPE_ERROR);

	ExchengeToken.SetWindowTextW(L"");
	string accessToken = SettingsClass::GetComponent(AccessToken);

	string deviceName = SettingsClass::GetComponent(DeviceName);
	string deviceId = SettingsClass::GetComponent(DeviceId);

	bool useDeviceName = SettingsClass::GetComponent(UseDeviceName);
	bool useDeviceId = SettingsClass::GetComponent(UseDeviceId);

	if (accessToken.empty()) {
		(msg << L"Ошибка: Введите access_token").show();
		return;
	}

	if (useDeviceName && deviceName.empty()) {
		(msg << L"Ошибка: Введите Имя устройства").show();
		return;
	}

	if (useDeviceId && deviceId.empty()) {
		(msg << L"Ошибка: Введите ID устройства").show();
		return;
	}

	string userAgent = useDeviceName ? ANDROID_USERAGENT_DEVICED(deviceName) : ANDROID_USERAGENT;

	VK_APIMETHOD Akkount(accessToken, userAgent);
	VK_APIMETHOD& point = Akkount["auth.getExchangeToken"];
	if (useDeviceId) {
		point << VK("device_id", deviceId);
	}
	VKAPI_ANSWER Answer = point();
	if (Answer.IsError == true || Answer.Object.find("response") == Answer.Object.end()) {
		(msg << (const char*)u8"Ошибка: " << Answer.Object.dump(4)).show();
		return;
	}
	try {
		json info = Answer.Object.at("response").get<json>();
		if (info.find("token") != info.end()) {
			SettingsClass::SetComponent(ExchengeToken, info.at("token").get<string>());
		}
		else {
			(msg << (const char*)u8"Ошибка: " << Answer.Object.dump(4)).show();
			return;
		}
	}
	catch (json::exception e) {
		(msg << (const char*)u8"Ошибка: " << e.what()).show();
		return;
	}
}

void ExchengeTokenToolDialog::OnCheckBox() {
	DeviceName.EnableWindow(UseDeviceName.GetCheck());
	DeviceId.EnableWindow(UseDeviceId.GetCheck());
}

void ExchengeTokenToolDialog::OnExchenge2Token()
{
	win_message msg(m_hWnd);
	msg.message_type(MSG_TYPE::TYPE_ERROR);

	AccessToken.SetWindowTextW(L"");
	string exchengeToken = SettingsClass::GetComponent(ExchengeToken);

	string deviceName = SettingsClass::GetComponent(DeviceName);
	string deviceId = SettingsClass::GetComponent(DeviceId);

	bool useDeviceName = SettingsClass::GetComponent(UseDeviceName);
	bool useDeviceId = SettingsClass::GetComponent(UseDeviceId);

	if (exchengeToken.empty()) {
		(msg << L"Ошибка: Введите exchenge_token").show();
		return;
	}

	if (useDeviceName && deviceName.empty()) {
		(msg << L"Ошибка: Введите Имя устройства").show();
		return;
	}

	if (useDeviceId && deviceId.empty()) {
		(msg << L"Ошибка: Введите ID устройства").show();
		return;
	}

	string userAgent = useDeviceName ? ANDROID_USERAGENT_DEVICED(deviceName) : ANDROID_USERAGENT;

	stringstream Link;
	Link << "https://oauth.vk.com/auth_by_exchange_token?initiator=expired_token&client_id=" << "2274003" << "&api_id=" << "2274003" << "&client_secret=" << "hHbZxrka2uZ6jB1inYsH" << "&scope=all&v=" << VKAPI_VERSION << "&lang=ru" << "&exchange_token=" << exchengeToken;
	if (useDeviceId) {
		Link << "&device_id=" << deviceId;
	}
	string AnswerVK;
	int res = DoCurlGetAndReturnUrl(Link.str(), KATE_USERAGENT, AnswerVK, 20);
	if (res != 0) {
		(msg << L"Ошибка: " << AnswerVK).show();
		return;
	}

	std::regex rgx("access_token=(.*?)&");
	std::smatch match;
	if (std::regex_search(AnswerVK, match, rgx)) {
		SettingsClass::SetComponent(AccessToken, match[1]);
	}
	else {
		(msg << L"Ошибка: " << AnswerVK).show();
		return;
	}
}