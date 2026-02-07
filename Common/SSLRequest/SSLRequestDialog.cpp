#include "pch.h"
#include "SSLRequestDialog.h"
#include "iostream"
#include <vector>
#include <iomanip>
#include <sstream>
#include <string>
#include <Winsock2.h>
#include <ws2tcpip.h>
#include <Windows.h>
#include <Shlwapi.h>
#include "WSTRUtils.h"
#include "Map.h"
using namespace std;
using namespace WSTRUtils;
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "Shlwapi.lib")

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

#define UTF8START "\xef\xbb\xbf"
#define ENDL "\r\n"
#define WENDL L"\r\n"

#define SOCKTIMEOUT 25
#define POST_BODY_MAX 2 * 1024 * 1024

static vector<wstring> Files;
static int Port = 0;

class Connection {
public:
	Connection(SOCKET_C socket, const std::string& connection) {
		this->socket = socket;
		this->connection = connection;
	}
	std::string connection;
	SOCKET_C socket;
};

class wstringparam {
public:
	void add(const std::string& name, const std::wstring& value) {
		paramsdata[name] = value;
	}
	void add_decode(const std::string& name, const std::string& value) {
		paramsdata[name] = UTF8_to_wchar(url_decode(value));
	}
	void add(const std::string& name, const std::string& value) {
		paramsdata[name] = UTF8_to_wchar(value);
	}
	int count() const {
		return (int)paramsdata.size();
	}
	void parse(const std::string& paramstr, const std::string& separator, bool need_url_decode) {
		string paramstring = paramstr;
		while (!paramstring.empty())
		{
			size_t SepPos = paramstring.find(separator);
			string SubKeys = paramstring.substr(0, SepPos);
			string key = SubKeys.substr(0, SubKeys.find('='));
			string value = SubKeys.substr(SubKeys.find('=') + 1);
			if (need_url_decode)
				value = url_decode(value);
			add(key, UTF8_to_wchar(value));
			if (SepPos == string::npos)
				paramstring.clear();
			else
				paramstring = paramstring.substr(SepPos + separator.length());
		}
	}
	void parse(const std::wstring& paramstring, const std::string& separator) {
		parse(wchar_to_UTF8(paramstring), separator, false);
	}
	bool exist(const std::string& name) {
		if (paramsdata.exist(name))
			return true;
		return false;
	}
	void erase(const std::string& name) {
		if (paramsdata.exist(name))
			paramsdata.erase(name);
	}
	std::wstring operator[](const std::string& name) const {
		if (paramsdata.exist(name))
			return paramsdata.at(name);
		return L"";
	}
	std::string get_utf8(const std::string& name) const {
		if (paramsdata.exist(name))
			return wchar_to_UTF8(paramsdata.at(name));
		return "";
	}
	void set_utf8(const std::string& name, const std::string& value) {
		if (paramsdata.exist(name))
			paramsdata[name] = UTF8_to_wchar(value);
	}
public:
	Map::Map<Map::string_compare_no_case, std::wstring> paramsdata;
};

enum class HTTP_METHOD {
	HTTP_METHOD_GET,
	HTTP_METHOD_POST,
	HTTP_METHOD_PUT,
	HTTP_METHOD_DELETE,
	HTTP_METHOD_HEAD,
	HTTP_METHOD_CONNECT,
	HTTP_METHOD_OPTIONS,
	HTTP_METHOD_TRACE,
	HTTP_METHOD_PATCH,
	HTTP_METHOD_UNKNOW
};

enum class TARGET_DEVICES {
	DEVICE_ANDROID,
	DEVICE_IPHONE,
	DEVICE_IPAD,
	DEVICE_WINDOWS,
	DEVICE_WINDOWS_PHONE,
	DEVICE_UNKNOW,
};

class RequestParserStruct {
public:
	RequestParserStruct() {
		method = HTTP_METHOD::HTTP_METHOD_UNKNOW;
		Device = TARGET_DEVICES::DEVICE_UNKNOW;
		bodySize = 0;
	}
	bool HTTPParse(const std::string& HttpHeader, const std::string& Connection);
	void ParsePostUrlEncoded(const std::string& http_post_url_encoded) {
		http_post_param.parse(http_post_url_encoded, "&", true);
	}
	bool IsMobile() const {
		if (Device == TARGET_DEVICES::DEVICE_ANDROID || Device == TARGET_DEVICES::DEVICE_IPHONE || Device == TARGET_DEVICES::DEVICE_IPAD || Device == TARGET_DEVICES::DEVICE_WINDOWS_PHONE)
			return true;
		return false;
	}
	TARGET_DEVICES Device;
	HTTP_METHOD method;
	int64_t bodySize;
	std::string connection;
	wstringparam cookies;
	wstringparam http_post_param;
	wstringparam http_get_param;
	wstringparam http_request;
};

static SOCKET_C tcp_listen(int Port) {
	SOCKET_C sock;
	struct sockaddr_in sin {};
	const int qlen = 1;
	if ((sock = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) <= 0) {
		return SOCKET_ERROR;
	}

	int tr = 1;
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char*)&tr, sizeof(tr));

	sin.sin_addr.s_addr = INADDR_ANY;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(Port);
	if (::bind(sock, (struct sockaddr*)&sin, sizeof(sin)) < 0) {
		closesocket(sock);
		return SOCKET_ERROR;
	}
	if (::listen(sock, qlen) < 0) {
		closesocket(sock);
		return SOCKET_ERROR;
	}
	return sock;
}

static bool ReciveNetBinary(SOCKET_C Socket, const void* Buffer, int Len, int timeOutSec) {
#if defined(__linux__)
	struct timeval timeout {};
	timeout.tv_sec = timeOutSec;
	timeout.tv_usec = 0;
#else
	DWORD timeout = (DWORD)(timeOutSec * 1000);
#endif
	setsockopt(Socket, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));
	int rsz = recv(Socket, (char*)Buffer, Len, MSG_WAITALL);
	if (rsz != Len) {
		if (rsz >= 0 && rsz <= Len) {
			memset((char*)Buffer + rsz, 0, Len - rsz);
		}
		else {
			memset((char*)Buffer, 0, Len);
		}
		return false;
	}
	return true;
}

static bool SendNetBinary(SOCKET_C Socket, const void* Buffer, int Len, int timeOutSec) {
#if defined(__linux__)
	struct timeval timeout {};
	timeout.tv_sec = timeOutSec;
	timeout.tv_usec = 0;
#else
	DWORD timeout = (DWORD)(timeOutSec * 1000);
#endif
	setsockopt(Socket, SOL_SOCKET, SO_SNDTIMEO, (char*)&timeout, sizeof(timeout));
	int rsz = send(Socket, (char*)Buffer, Len, 0);
	if (rsz != Len)
		return false;
	return true;
}

bool SendHTTTPAnswerWithData(const Connection& client, const string& CodeAnsw, const string& ContentType, const string& Data, bool isHead) {
	ostringstream HttpAnswer;
	HttpAnswer << "HTTP/1.1 " << CodeAnsw << ENDL
		<< "Server: Apache" << ENDL
		<< "Date: " << GetTimeGMT(0) << ENDL
		<< "Content-Type: " << ContentType << ENDL;
	if (Data.size() > 0)
		HttpAnswer << "Content-Length: " << Data.size() << ENDL;
	HttpAnswer << "Connection: close" << ENDL;
	HttpAnswer << "Cache-Control: no-cache, no-store, must-revalidate" << ENDL
		<< ENDL;
	string header = HttpAnswer.str();

	if (!SendNetBinary(client.socket, header.data(), (int)header.size(), SOCKTIMEOUT)) {
		return false;
	}
	if (!isHead && !SendNetBinary(client.socket, Data.data(), (int)Data.size(), SOCKTIMEOUT)) {
		return false;
	}
	return true;
}

bool RequestParserStruct::HTTPParse(const string& HttpHeader, const string& Connection) {
	string HttpHdr = HttpHeader;
	connection = Connection;
	size_t Pos = HttpHdr.find(" ");
	if (Pos == string::npos) {
		return false;
	}
	string MtStr = HttpHdr.substr(0, Pos);
	if (MtStr == "GET")
		method = HTTP_METHOD::HTTP_METHOD_GET;
	else if (MtStr == "POST")
		method = HTTP_METHOD::HTTP_METHOD_POST;
	else if (MtStr == "PUT")
		method = HTTP_METHOD::HTTP_METHOD_PUT;
	else if (MtStr == "DELETE")
		method = HTTP_METHOD::HTTP_METHOD_DELETE;
	else if (MtStr == "HEAD")
		method = HTTP_METHOD::HTTP_METHOD_HEAD;
	else if (MtStr == "CONNECT")
		method = HTTP_METHOD::HTTP_METHOD_CONNECT;
	else if (MtStr == "OPTIONS")
		method = HTTP_METHOD::HTTP_METHOD_OPTIONS;
	else if (MtStr == "TRACE")
		method = HTTP_METHOD::HTTP_METHOD_TRACE;
	else if (MtStr == "PATCH")
		method = HTTP_METHOD::HTTP_METHOD_PATCH;
	else
		method = HTTP_METHOD::HTTP_METHOD_UNKNOW;

	if (Pos + 2 > HttpHdr.size() - 1) {
		return false;
	}
	HttpHdr = HttpHdr.substr(Pos + 2);
	Pos = HttpHdr.find(" ");
	if (Pos == string::npos) {
		return false;
	}
	string GetParams = HttpHdr.substr(0, Pos);
	size_t qw = GetParams.find("?");
	http_request.add_decode("OPath", GetParams);
	http_request.add_decode("Path", GetParams.substr(0, qw));
	if (qw != string::npos && qw + 1 < GetParams.size() - 1) {
		GetParams = GetParams.substr(GetParams.find("?") + 1);
		http_get_param.parse(GetParams, "&", true);
	}
	if (Pos + 6 > HttpHdr.size() - 1) {
		return false;
	}
	HttpHdr = HttpHdr.substr(Pos + 6);
	Pos = HttpHdr.find("\r\n");
	http_request.add("Version", HttpHdr.substr(0, Pos));
	if (Pos == string::npos || Pos + 2 > HttpHdr.size() - 1) {
		return false;
	}
	HttpHdr = HttpHdr.substr(Pos + 2);
	while ((Pos = HttpHdr.find(": ")) != string::npos) {
		size_t endl = HttpHdr.find("\r\n");
		if (endl == string::npos) {
			return false;
		}
		http_request.add(HttpHdr.substr(0, Pos), HttpHdr.substr(Pos + 2, endl - (Pos + 2)));
		HttpHdr = HttpHdr.substr(endl + 2);
	}

	if (http_request.exist("Cookie")) {
		wstring Cookiest = http_request["Cookie"];
		http_request.erase("Cookie");
		cookies.parse(wchar_to_UTF8(Cookiest), "; ", true);
	}

	if (search(http_request.get_utf8("User-Agent"), "iPad") != string::npos)
		Device = TARGET_DEVICES::DEVICE_IPAD;
	else if (search(http_request.get_utf8("User-Agent"), "iPhone") != string::npos)
		Device = TARGET_DEVICES::DEVICE_IPHONE;
	else if (search(http_request.get_utf8("User-Agent"), "Android") != string::npos)
		Device = TARGET_DEVICES::DEVICE_ANDROID;
	else if (search(http_request.get_utf8("User-Agent"), "Windows Phone") != string::npos)
		Device = TARGET_DEVICES::DEVICE_WINDOWS_PHONE;
	else if (search(http_request.get_utf8("User-Agent"), "Windows") != string::npos)
		Device = TARGET_DEVICES::DEVICE_WINDOWS;
	else
		Device = TARGET_DEVICES::DEVICE_ANDROID;

	if (method == HTTP_METHOD::HTTP_METHOD_POST) {
		string CntOf = http_request.get_utf8("Content-Length");
		if (CntOf.size() > 0) {
			try {
				bodySize = stoll(CntOf);
			}
			catch (...) {
				return false;
			}
		}
	}
	return true;
}

static bool ReciveHTTPRequest(const Connection& client, RequestParserStruct& Req) {
	string HttpRequest;
	string http_post_url_encoded;

	size_t nCurrentPos = 0;
	while (HttpRequest.find("\r\n\r\n") == string::npos) {
		char Tmp = 0;
		if (HttpRequest.size() > POST_BODY_MAX || !ReciveNetBinary(client.socket, &Tmp, 1, SOCKTIMEOUT)) {
			return false;
		}
		HttpRequest.push_back(Tmp);
	}
	if (!Req.HTTPParse(HttpRequest, client.connection)) {
		return false;
	}

	if (Req.method != HTTP_METHOD::HTTP_METHOD_GET
		&& Req.method != HTTP_METHOD::HTTP_METHOD_POST
		&& Req.method != HTTP_METHOD::HTTP_METHOD_HEAD) {
		return false;
	}
	if (Req.method == HTTP_METHOD::HTTP_METHOD_POST) {
		if (Req.bodySize > 0) {
			if (Req.bodySize > POST_BODY_MAX) {
				return false;
			}
			http_post_url_encoded.resize(Req.bodySize);
			if (!ReciveNetBinary(client.socket, http_post_url_encoded.data(), (int)Req.bodySize, SOCKTIMEOUT)) {
				return false;
			}
			Req.ParsePostUrlEncoded(http_post_url_encoded);
		}
	}
	return true;
}

std::string filename(const std::string& fname) {
	const char* FnS = "\\/";
	size_t pos = fname.find_last_of(FnS);
	return (std::string::npos == pos)
		? fname
		: fname.substr(pos + 1);
}

string GetFileData(const wstring &File) {
	FILE* fl = nullptr;
	if (_wfopen_s(&fl, File.c_str(), L"rb") != 0) {
		return "";
	}
	if (!fl) {
		return "";
	}
	fseek(fl, 0, SEEK_END);
	int Size = ftell(fl);
	if (Size <= 0) {
		fclose(fl);
		return "";
	}
	fseek(fl, 0, SEEK_SET);
	string ret;
	ret.resize(Size);
	fread((void*)ret.data(), 1, Size, fl);
	fclose(fl);
	return ret;
}


SSLRequestDialog::SSLRequestDialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(SSLRequestDialog::IDD, pParent) {
	m_hIcon = nullptr;
	memset(szBuf, 0, sizeof(szBuf));
}

SSLRequestDialog::~SSLRequestDialog() {
}

void SSLRequestDialog::DoDataExchange(CDataExchange* pDX) {
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MFCEDITBROWSE1, File1);
	DDX_Control(pDX, IDC_MFCEDITBROWSE2, File2);
	DDX_Control(pDX, IDC_MFCEDITBROWSE3, File3);
	DDX_Control(pDX, IDC_EDIT1, EPort);
	DDX_Control(pDX, IDC_EDIT2, EURL);
	DDX_Control(pDX, IDC_BUTTON1, BStart);
}

BEGIN_MESSAGE_MAP(SSLRequestDialog, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_SETCURSOR()
	ON_WM_HSCROLL()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON1, OnStart)
END_MESSAGE_MAP()

BOOL SSLRequestDialog::OnInitDialog() {
	CDialogEx::OnInitDialog();
	XTPSkinMgr()->AlphaEnableWindow(m_hWnd, 220);
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON1);
	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);

	EURL.SetWindowTextW(L"https://punchsalad.com/ssl-certificate-generator/");
	return TRUE;
}

void SSLRequestDialog::OnSysCommand(UINT nID, LPARAM lParam) {
	CDialogEx::OnSysCommand(nID, lParam);
}

void SSLRequestDialog::OnPaint() {
	if (IsIconic()) {
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
	else {
		CDialogEx::OnPaint();
	}

}

static void client_shutdown(Connection& client, int how) {
	shutdown(client.socket, how);
	closesocket(client.socket);
	client.socket = SOCKET_ERROR;
}

THREAD_IMPL(ClientConnected, arg) {
	Connection client = extractThreadParamAndDelete<Connection>(arg);

	RequestParserStruct Req;

	if (!ReciveHTTPRequest(client, Req)) {
		client_shutdown(client, SHUT_RDWR);
		return NULL_P;
	}

	wstring ResultFile;
	for (auto& i : Files) {
		if (Req.http_request.get_utf8("Path").find(filename(wchar_to_UTF8(i))) == string::npos)
			continue;
		ResultFile = i;
	}
	if (ResultFile.length() <= 0) {
		bool ret = SendHTTTPAnswerWithData(client, "404 NOT FOUND", "text/plain; charset=utf-8", "Error", Req.method == HTTP_METHOD::HTTP_METHOD_HEAD);
		client_shutdown(client, ret ? SHUT_WR : SHUT_RDWR);
		return NULL_P;
	}
	string ResultA = GetFileData(ResultFile);
	bool ret = SendHTTTPAnswerWithData(client, "200 OK", "text/plain; charset=utf-8", ResultA, Req.method == HTTP_METHOD::HTTP_METHOD_HEAD);
	client_shutdown(client, ret ? SHUT_WR : SHUT_RDWR);
	return NULL_P;
}

static void SetTimeOutsSocket(SOCKET_C Socket) {
#if defined(__linux__)
	struct timeval timeout {};
	timeout.tv_sec = globalConfig.socket_timeout_sec;
	timeout.tv_usec = 0;
#else
	DWORD timeout = (DWORD)(SOCKTIMEOUT * 1000);
#endif
	setsockopt(Socket, SOL_SOCKET, SO_SNDTIMEO, (char*)&timeout, sizeof(timeout));
	setsockopt(Socket, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));
}

static string GetConnectionAddr(const sockaddr_in& FromAddr) {
	string buf;
	buf.resize(260);
	inet_ntop(AF_INET, &FromAddr.sin_addr, (char*)buf.data(), 260);
	buf = buf.c_str();
	buf += string(":") + to_string(ntohs(FromAddr.sin_port));
	return buf;
}

THREAD_IMPL(Server, arg) {
	SOCKET_C server_sock = 0, client_sock = 0;
	server_sock = tcp_listen(Port);
	if (server_sock == INVALID_SOCKET) {
		(win_message().timeout(5).message_type(MSG_TYPE::TYPE_ERROR) << L"Сбой в инициализации сервера(socket bind)").show();
		return NULL_P;
	}
	sockaddr_in FromAddr;
	int len = sizeof(FromAddr);
	while (true) {
		client_sock = accept(server_sock, (struct sockaddr*)&FromAddr, &len);
		if (client_sock <= 0)
			continue;
		DWORD sockFlag = 1;
		setsockopt(client_sock, IPPROTO_TCP, TCP_NODELAY, (char*)&sockFlag, sizeof(sockFlag));
		SetTimeOutsSocket(client_sock);

		Connection* packet = new Connection(client_sock, GetConnectionAddr(FromAddr));
		if (!CreateThreadDetachedSimple(&ClientConnected, (void*)packet)) {
			shutdown(client_sock, SHUT_RDWR);
			closesocket(client_sock);
			delete packet;
			continue;
		}
	}
	shutdown(server_sock, SHUT_RDWR);
	closesocket(server_sock);
	(win_message().message_type(MSG_TYPE::TYPE_ERROR) << L"Сбой!").show();
	return NULL_P;
}

void SSLRequestDialog::OnStart() {
	win_message msg(m_hWnd);
	msg.message_type(MSG_TYPE::TYPE_ERROR);

	Files.clear();
	CString Temp;
	File1.GetWindowTextW(Temp);
	if (Temp.GetLength() <= 0) {
		(msg << L"Выберите 1 файл").show();
		return;
	}
	Files.push_back(Temp.GetString());
	File2.GetWindowTextW(Temp);
	if (Temp.GetLength() > 0) {
		Files.push_back(Temp.GetString());
	}
	File3.GetWindowTextW(Temp);
	if (Temp.GetLength() > 0) {
		Files.push_back(Temp.GetString());
	}

	EPort.GetWindowTextW(Temp);
	Port = _wtoi(Temp.GetString());
	if (Port <= 0 || Port > 65535) {
		(msg << L"Порт может быть только 1..65535").show();
		return;
	}
	BStart.SetWindowTextW(L"Запущен");
	BStart.EnableWindow(FALSE);
	File1.EnableWindow(FALSE);
	File2.EnableWindow(FALSE);
	File3.EnableWindow(FALSE);
	EPort.EnableWindow(FALSE);

	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		(win_message(m_hWnd).timeout(5).message_type(MSG_TYPE::TYPE_ERROR) << L"Ошибка инициализации сокетов").show();
		return;
	}
	CreateThreadDetachedSimple(&Server);
}

HCURSOR SSLRequestDialog::OnQueryDragIcon() {
	return (HCURSOR)m_hIcon;
}

void SSLRequestDialog::OnClose() {
	_exit(0);
}

BOOL SSLRequestDialog::PreTranslateMessage(MSG* pMsg) {
	if (pMsg->hwnd == File1.GetSafeHwnd() || pMsg->hwnd == File2.GetSafeHwnd() || pMsg->hwnd == File3.GetSafeHwnd()) {
		if (pMsg->message == WM_DROPFILES) {
			long ResultQueryFile = DragQueryFileW((HDROP)pMsg->wParam, 0xFFFFFFFF, NULL, 0);
			if (ResultQueryFile == 1) {
				ResultQueryFile = DragQueryFileW((HDROP)pMsg->wParam, 0, szBuf, sizeof(szBuf) / sizeof(wchar_t));
				if (pMsg->hwnd == File1.GetSafeHwnd())
					File1.SetWindowTextW(szBuf);
				else if (pMsg->hwnd == File2.GetSafeHwnd())
					File2.SetWindowTextW(szBuf);
				else
					File3.SetWindowTextW(szBuf);
			}
		}
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}