#include "pch.h"
#include <iostream>
#include <vector>
#include <list>
#include <regex>
#include <algorithm>
#include <random>
#include <Windows.h>
#include <Tlhelp32.h>
#include <shlobj.h>
#include <shlwapi.h>
#include "WSTRUtils.h"
#include "win_api_utils.h"
#include "config_json_parse.h"
#include "vk_api_interface.h"
#include "json.hpp"
using namespace std;
using namespace nlohmann;
#define SERVICENAME L"VKLoveSpamService"
#define SERVICETITLE L"VKLoveSpamService"
SERVICE_STATUS serviceStatus;
SERVICE_STATUS_HANDLE serviceStatusHandle;

#ifdef _WIN64
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif _WIN32
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif

static std::wstring ExtractAppPath()
{
	const wchar_t* FnS = L"\\/";
	TCHAR szFileName[MAX_PATH];
	GetModuleFileNameW(0, szFileName, MAX_PATH);

	std::wstring fname = szFileName;
	size_t pos = fname.find_last_of(FnS);
	return (std::wstring::npos == pos)
		? fname
		: fname.substr(0, pos);
}

static std::wstring GetFullExePath()
{
	TCHAR szFileName[MAX_PATH];
	GetModuleFileNameW(0, szFileName, MAX_PATH);

	return szFileName;
}

void addLogMessage(const wstring& msg, bool error = true) {
	if (error) {
		MessageBoxW(0, msg.c_str(), L"Ошибка!", MB_ICONERROR);
	}
	else {
		MessageBoxW(0, msg.c_str(), L"Внимание!", MB_ICONINFORMATION);
	}
}

bool InstallServiceSC() {
	SC_HANDLE hSCManager = OpenSCManagerW(NULL, NULL, SC_MANAGER_CREATE_SERVICE);
	if (!hSCManager) {
		addLogMessage(L"Ошибка: не удается открыть диспетчер управления службами!");
		return false;
	}

	SC_HANDLE hService = CreateServiceW(
		hSCManager,
		SERVICENAME,
		SERVICETITLE,
		SERVICE_ALL_ACCESS,
		SERVICE_WIN32_OWN_PROCESS,
		SERVICE_AUTO_START,
		SERVICE_ERROR_NORMAL,
		(L"\"" + GetFullExePath() + L"\" --service").c_str(),
		NULL, NULL, NULL, NULL, NULL
	);

	if (!hService) {
		int err = GetLastError();
		switch (err) {
		case ERROR_ACCESS_DENIED:
			addLogMessage(L"Ошибка: В ДОСТУПЕ ОТКАЗАНО");
			break;
		case ERROR_CIRCULAR_DEPENDENCY:
			addLogMessage(L"Ошибка: ЦИКЛИЧЕСКАЯ ЗАВИСИМОСТЬ");
			break;
		case ERROR_DUPLICATE_SERVICE_NAME:
			addLogMessage(L"Ошибка: СЛУЖБА С ДАННЫМ ИМЕНЕМ СУЩЕСТВУЕТ");
			break;
		case ERROR_INVALID_HANDLE:
			addLogMessage(L"Ошибка: НЕВАЛИДНЫЙ ДЕСКРИПТОР");
			break;
		case ERROR_INVALID_NAME:
			addLogMessage(L"Ошибка: НЕВАЛИДНОЕ ИМЯ");
			break;
		case ERROR_INVALID_PARAMETER:
			addLogMessage(L"Ошибка: НЕВАЛИДНЫЙ ПАРАМЕТР");
			break;
		case ERROR_INVALID_SERVICE_ACCOUNT:
			addLogMessage(L"Ошибка: НЕВАЛИДНЫЙ АККАУНТ СЛУЖБЫ");
			break;
		case ERROR_SERVICE_EXISTS:
			addLogMessage(L"Ошибка: СЛУЖБА УЖЕ СУЩЕСТВУЕТ");
			break;
		default:
			addLogMessage(L"Ошибка: НЕИЗВЕСТНО...");
		}
		CloseServiceHandle(hSCManager);
		return false;
	}
	CloseServiceHandle(hService);

	CloseServiceHandle(hSCManager);
	addLogMessage(L"Успех!", false);
	return true;
}

bool RemoveServiceSC() {
	SC_HANDLE hSCManager = OpenSCManagerW(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (!hSCManager) {
		addLogMessage(L"Ошибка: не удается открыть диспетчер управления службами!");
		return false;
	}
	SC_HANDLE hService = OpenServiceW(hSCManager, SERVICENAME, SERVICE_STOP | DELETE);
	if (!hService) {
		addLogMessage(L"Error: Can't open service");
		CloseServiceHandle(hSCManager);
		return false;
	}
	ControlService(hService, SERVICE_CONTROL_STOP, (LPSERVICE_STATUS)&serviceStatus);
	Sleep(5000);
	if (!DeleteService(hService)) {
		addLogMessage(L"Ошибка: не могу удалить службу");
		CloseServiceHandle(hService);
		CloseServiceHandle(hSCManager);
		return false;
	}
	CloseServiceHandle(hService);
	CloseServiceHandle(hSCManager);
	addLogMessage(L"OK!", false);
	return true;
}

bool StartServiceSC() {
	SC_HANDLE hSCManager = OpenSCManagerW(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	SC_HANDLE hService = OpenServiceW(hSCManager, SERVICENAME, SERVICE_START);
	if (!StartServiceW(hService, 0, NULL)) {
		CloseServiceHandle(hSCManager);
		addLogMessage(L"Ошибка: Не могу запустить сервис");
		return false;
	}

	CloseServiceHandle(hService);
	CloseServiceHandle(hSCManager);
	return true;
}

bool CheckServiceSC() {
	SC_HANDLE hSCManager = OpenSCManagerW(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (!hSCManager) {
		addLogMessage(L"Ошибка: не удается открыть диспетчер управления службами!");
		return false;
	}
	SC_HANDLE hService = OpenServiceW(hSCManager, SERVICENAME, SERVICE_QUERY_CONFIG);
	if (!hService) {
		CloseServiceHandle(hSCManager);
		return false;
	}
	CloseServiceHandle(hService);
	CloseServiceHandle(hSCManager);
	return true;
}

void ControlHandler(DWORD request) {
	switch (request)
	{
	case SERVICE_CONTROL_STOP:
		serviceStatus.dwWin32ExitCode = 0;
		serviceStatus.dwCurrentState = SERVICE_STOPPED;
		SetServiceStatus(serviceStatusHandle, &serviceStatus);
		return;

	case SERVICE_CONTROL_SHUTDOWN:
		serviceStatus.dwWin32ExitCode = 0;
		serviceStatus.dwCurrentState = SERVICE_STOPPED;
		SetServiceStatus(serviceStatusHandle, &serviceStatus);
		return;

	default:
		break;
	}

	SetServiceStatus(serviceStatusHandle, &serviceStatus);
	return;
}

template<typename T>
static inline T optVarJSon(json& st, const string& name, T defaultValue) {
	if (!st.contains(name)) {
		return defaultValue;
	}
	return st.at(name).get<T>();
}

template<typename T>
static inline vector<T> getListJSon(json& st, const string& name, vector<T> defaultValue = vector<T>()) {
	vector<T> ret;
	for (auto& i : st.at(name)) {
		ret.push_back(i.get<T>());
	}
	return ret;
}

template<typename T>
static inline json makeFromListJSon(const vector<T> &values) {
	json ret = json::array();
	for (auto& i : values) {
		ret.push_back(i);
	}
	return ret;
}

class PTime {
public:
	PTime() {
		hour = 0;
		minut = 0;
	}
	PTime& from_string(const string& str) {
		regex p("^([0-1]?[0-9]|2?[0-3]|[0-9])[:\\-\\/]([0-5][0-9]|[0-9])$");
		std::smatch matches;
		int status = 0;
		if (!std::regex_search(str, matches, p))
			throw runtime_error("time");
		hour = stoi(matches[1]);
		minut = stoi(matches[2]);
		return *this;
	}
	int to_sec() const {
		return (hour * 60 + minut) * 60;
	}
	string to_str() const {
		return (hour <= 9 ? "0" : "") + to_string(hour) + ":" + (minut <= 9 ? "0" : "") + to_string(minut);
	}
	bool operator==(const PTime& v) const {
		return hour == v.hour && minut == v.minut;
	}
	bool operator!=(const PTime& v) const {
		return hour != v.hour || minut != v.minut;
	}
	bool operator<(const PTime& v) const {
		return to_sec() < v.to_sec();
	}
	int hour;
	int minut;
};

class BuildInMessage {
public:
	BuildInMessage() {
		time = 0;
		need_online = false;
	}
	BuildInMessage(time_t snap, int delta, const PTime& p, const string& msg, bool need_online) {
		message = msg;
		this->need_online = need_online;
		tm t_m;
		memset(&t_m, 0, sizeof(tm));
		if (localtime_s(&t_m, &snap)) {
			throw runtime_error("localtime_s");
		}
		t_m.tm_hour = 0;
		t_m.tm_min = 0;
		t_m.tm_sec = 0;

		time = mktime(&t_m);
		time += (24 * 60 * 60 + p.to_sec() + (rand() % delta) * 60);
	}
	bool operator==(const BuildInMessage& v) const {
		return time == v.time;
	}
	bool operator!=(const BuildInMessage& v) const {
		return time != v.time;
	}
	bool operator<(const BuildInMessage& v) const {
		return time < v.time;
	}
	BuildInMessage& deserialize(json& pref) {
		time = pref.at("time").get<time_t>();
		message = pref.at("message").get<string>();
		need_online = pref.at("need_online").get<bool>();
		return *this;
	}
	json serialize() const {
		json pref = json::object();
		pref.emplace("time", time);
		pref.emplace("description_time", WSTRUtils::wchar_to_UTF8(WSTRUtils::GetTimeAT(time)));
		pref.emplace("message", message);
		pref.emplace("need_online", need_online);
		return pref;
	}
	time_t time;
	bool need_online;
	string message;
};

class PMSGType {
public:
	PMSGType() {
		need_online = false;
	}
	bool operator==(const PMSGType& v) const {
		return time == v.time;
	}
	bool operator!=(const PMSGType& v) const {
		return time != v.time;
	}
	bool operator<(const PMSGType& v) const {
		return time < v.time;
	}
	PMSGType &deserialize(json& pref) {
		id = pref.at("id").get<string>();
		time.from_string(pref.at("time").get<string>());
		need_online = optVarJSon(pref, "need_online", false);
		return *this;
	}
	json serialize() const {
		json pref = json::object();
		pref.emplace("id", id);
		pref.emplace("time", time.to_str());
		pref.emplace("need_online", need_online);
		return pref;
	}
	string id;
	PTime time;
	bool need_online;
};

class PConfig {
public:
	PConfig() {
		peer_id = 0;
		run_status = true;
		delta_time_minutes = 0;
	}
	void deserialize(json& pref) {
		access_token = pref.at("access_token").get<string>();
		user_agent = pref.at("user_agent").get<string>();
		peer_id = pref.at("peer_id").get<int64_t>();
		run_status = optVarJSon(pref, "run_status", true);
		delta_time_minutes = optVarJSon(pref, "delta_time_minutes", 1);

		list<PMSGType> types;
		for (auto& i : pref.at("msg_types")) {
			types.push_back(PMSGType().deserialize(i));
		}
		for (auto& i : types) {
			data[i] = getListJSon<string>(pref, i.id);
		}
		if (pref.contains("built_messages")) {
			for (auto& i : pref.at("built_messages")) {
				built_messages.push_back(BuildInMessage().deserialize(i));
			}
		}
	}
	json serialize() const {
		json pref = json::object();
		pref.emplace("access_token", access_token);
		pref.emplace("user_agent", user_agent);
		pref.emplace("peer_id", peer_id);
		pref.emplace("run_status", run_status);
		pref.emplace("delta_time_minutes", delta_time_minutes);

		json typesJ = json::array();
		for (auto& i : data) {
			typesJ.push_back(i.first.serialize());
		}
		pref.emplace("msg_types", typesJ);
		for (auto& i : data) {
			if (!pref.contains(i.first.id)) {
				pref.emplace(i.first.id, makeFromListJSon(i.second));
			}
		}
		json msgss = json::array();
		for (auto& i : built_messages) {
			msgss.push_back(i.serialize());
		}
		pref.emplace("built_messages", msgss);
		return pref;
	}
	string access_token;
	string user_agent;
	int64_t peer_id;
	bool run_status;
	int delta_time_minutes;
	map<PMSGType, vector<string>> data;
	list<BuildInMessage> built_messages;
};

void genBuiltIns(PConfig& config) {
	time_t dt = time(0);
	int day_inc = 0;
	map<PMSGType, vector<string>> vt = config.data;

	std::random_device rd;
	std::mt19937 rng(rd());
	for (auto& i : vt) {
		std::ranges::shuffle(i.second, rng);
	}
	for (size_t o = 0; o < (*vt.begin()).second.size(); o++) {
		for (auto& i : vt) {
			config.built_messages.push_back(BuildInMessage(dt + (day_inc * 24 * 60 * 60), config.delta_time_minutes, i.first.time, i.second[o], i.first.need_online));
		}
		day_inc++;
	}
}

bool VK_APIMETHOD::VK_APIMETHOD_doCapcha(const std::string& captcha_img, const std::string& user_agent, std::string& code) {
	return false;
}

void VK_APIMETHOD::VK_APIMETHOD_doLimit(const std::string& Method) {
}

bool ProcMain(bool is_service) {
	if (PathFileExistsW((ExtractAppPath() + L"\\" + PREFS_NAME).c_str()) == FALSE) {
		return false;
	}
	PConfig config;

	try {
		json settings;
		if (parseJsonConfig(ExtractAppPath() + L"\\" + PREFS_NAME, settings)) {
			config.deserialize(settings);
			if (!config.peer_id || !config.run_status || config.access_token.empty() || config.data.empty()) {
				return false;
			}
			size_t count = (*config.data.begin()).second.size();
			for (auto& i : config.data) {
				if (i.second.empty() || i.second.size() != count) {
					return false;
				}
			}
			if (config.built_messages.empty()) {
				genBuiltIns(config);
			}
		}
		else {
			return false;
		}
	}
	catch (...) {
		return false;
	}
	writeJsonConfig(ExtractAppPath() + L"\\" + PREFS_NAME, config.serialize());

	VK_APIMETHOD VkReq(config.access_token, config.user_agent);

	while (is_service && serviceStatus.dwCurrentState == SERVICE_RUNNING || !is_service)
	{
		Sleep(10000);

		if (config.built_messages.empty()) {
			genBuiltIns(config);
			writeJsonConfig(ExtractAppPath() + L"\\" + PREFS_NAME, config.serialize());
		}
		time_t snap = time(0);
		BuildInMessage ms = config.built_messages.front();
		if ((long long)snap - (long long)ms.time >= 0 && (long long)snap - (long long)ms.time < config.delta_time_minutes * 60) {
			VkReq["users.get"] << VKI("user_ids", config.peer_id) << VK("fields", "blacklisted,can_write_private_message,online,last_seen");
			VKAPI_ANSWER GDe = VkReq();
			if (GDe.IsError == true || GDe.Object.find("response") == GDe.Object.end()) {
				continue;
			}
			try {
				json info = GDe.Object.at("response").get<json>();
				info = info.begin().value();
				if (info.contains("blacklisted") && info.at("blacklisted").get<int>() != 0 || info.contains("can_write_private_message") && !info.at("can_write_private_message").get<int>()) {
					config.run_status = false;
					writeJsonConfig(ExtractAppPath() + L"\\" + PREFS_NAME, config.serialize());
					return false;
				}
				if (ms.need_online && (!info.contains("online") || info.at("online").get<int>() == 0)) {
					if (!info.contains("last_seen") ||
						!info.at("last_seen").contains("time") ||
						!info.at("last_seen").at("time").is_number() ||
						snap - info.at("last_seen").at("time").get<time_t>() > config.delta_time_minutes * 60) {
						config.built_messages.pop_front();
						writeJsonConfig(ExtractAppPath() + L"\\" + PREFS_NAME, config.serialize());
						continue;
					}
				}
				VkReq["messages.send"] << VKI("peer_id", config.peer_id) << VKI("random_id", time(0)) << VK("message", ms.message);
				if (VkReq().IsError) {
					continue;
				}
			}
			catch (json::exception e) {
				cout << e.what();
				continue;
			}

			config.built_messages.pop_front();
			writeJsonConfig(ExtractAppPath() + L"\\" + PREFS_NAME, config.serialize());
		}
		else if ((long long)snap - (long long)ms.time > config.delta_time_minutes * 60) {
			config.built_messages.pop_front();
			writeJsonConfig(ExtractAppPath() + L"\\" + PREFS_NAME, config.serialize());
		}
	}
	return true;
}

void ServiceMain() {
	serviceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
	serviceStatus.dwCurrentState = SERVICE_START_PENDING;
	serviceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN;
	serviceStatus.dwWin32ExitCode = 0;
	serviceStatus.dwServiceSpecificExitCode = 0;
	serviceStatus.dwCheckPoint = 0;
	serviceStatus.dwWaitHint = 0;

	serviceStatusHandle = RegisterServiceCtrlHandlerW(SERVICENAME, (LPHANDLER_FUNCTION)ControlHandler);
	if (serviceStatusHandle == (SERVICE_STATUS_HANDLE)0) {
		return;
	}
	serviceStatus.dwCurrentState = SERVICE_RUNNING;
	SetServiceStatus(serviceStatusHandle, &serviceStatus);

	bool ret = ProcMain(true);
	serviceStatus.dwCurrentState = SERVICE_STOPPED;
	serviceStatus.dwWin32ExitCode = ret ? 0 : 1;
	SetServiceStatus(serviceStatusHandle, &serviceStatus);
}
/*
int main() {
	ProcMain(false);
	return 0;
}
*/

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR szCmdLine, int iCmdShow)
{
	srand((unsigned int)time(0));
	if (wstring(szCmdLine).find(L"--service") != wstring::npos) {
		SERVICE_TABLE_ENTRYW ServiceTable[1];
		ServiceTable[0].lpServiceName = (LPWSTR)SERVICENAME;
		ServiceTable[0].lpServiceProc = (LPSERVICE_MAIN_FUNCTION)ServiceMain;

		StartServiceCtrlDispatcherW(ServiceTable);
	}
	else {
		bool v = CheckServiceSC();
		if (v && MessageBoxW(0, L"Удалить службу?", L"Внимание", MB_ICONQUESTION | MB_YESNO) == IDYES) {
			RemoveServiceSC();
		}
		else if (!v) {
			int vb = MessageBoxW(0, L"Установить службу?", L"Внимание", MB_ICONQUESTION | MB_YESNOCANCEL);
			if (vb == IDYES && InstallServiceSC()) {
				StartServiceSC();
			}
			else if (vb == IDNO) {
				ProcMain(false);
			}
		}
	}
	return 0;
}