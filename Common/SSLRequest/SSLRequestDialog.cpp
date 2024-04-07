#include "pch.h"
#include "SSLRequestDialog.h"
#include "iostream"
#include <vector>
#include <iomanip>
#include <sstream>
#include <string>
#include <map>
#include <Winsock2.h>
#include <Windows.h>
#include <Shlwapi.h>
#include "WSTRUtils.h"
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


static vector<wstring> Files;
static int Port = 0;

class wstringparam
{
public:
	void add(string name, wstring value);
	void add_decode(string name, string value);
	void add(string name, string value);
	int count();
	void parse(string paramstring, string separator, bool need_url_decode);
	void parse(wstring paramstring, string separator);
	bool exist(string name);
	void erase(string name);
	wstring operator[](string name);
	string get_utf8(string name);
	void set_utf8(string name, string value);
public:
	map<string, wstring> paramsdata;
};

void wstringparam::add(string name, wstring value)
{
	paramsdata.insert(pair<string, wstring>(name, value));
}
void wstringparam::add_decode(string name, string value)
{
	paramsdata.insert(pair<string, wstring>(name, UTF8_to_wchar(url_decode(value))));
}
void wstringparam::add(string name, string value)
{
	paramsdata.insert(pair<string, wstring>(name, UTF8_to_wchar(value)));
}
void wstringparam::parse(string paramstring, string separator, bool need_url_decode)
{
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
void wstringparam::parse(wstring paramstring, string separator)
{
	parse(wchar_to_UTF8(paramstring), separator, false);
}
int wstringparam::count()
{
	return (int)paramsdata.size();
}
wstring wstringparam::operator[](string name)
{
	if (paramsdata.find(name) != paramsdata.end())
		return paramsdata[name];
	return L"";
}
bool wstringparam::exist(string name)
{
	if (paramsdata.find(name) != paramsdata.end())
		return true;
	return false;
}
void wstringparam::erase(string name)
{
	if (paramsdata.find(name) != paramsdata.end())
		paramsdata.erase(name);
}
string wstringparam::get_utf8(string name)
{
	if (paramsdata.find(name) != paramsdata.end())
		return wchar_to_UTF8(paramsdata[name]);
	return "";
}
void wstringparam::set_utf8(string name, string value)
{
	if (paramsdata.find(name) != paramsdata.end())
		paramsdata[name] = UTF8_to_wchar(value);
}

enum HTTP_METHOD
{
	HTTP_METHOD_GET = 0,
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

class RequestParserStruct
{
public:
	RequestParserStruct() {
		is_mobile = false;
		method = HTTP_METHOD::HTTP_METHOD_UNKNOW;
	}

	bool is_mobile;
	HTTP_METHOD method;
	string connection;
	wstringparam cookies;
	wstringparam http_post_param;
	wstringparam http_get_param;
	wstringparam http_request;
};

SOCKET tcp_listen(int Port)
{
	SOCKET sock;
	struct sockaddr_in sin {};
	const int qlen = 1;
	if ((sock = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) <= 0) {
		return SOCKET_ERROR;
	}

	DWORD tr = 1;
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

void HTTPParse(string HttpHeader, RequestParserStruct& hdr, string Connection)
{
	hdr.connection = Connection;
	size_t Pos = HttpHeader.find(" ");
	string MtStr = HttpHeader.substr(0, Pos);
	if (MtStr == "GET")
		hdr.method = HTTP_METHOD_GET;
	else if (MtStr == "POST")
		hdr.method = HTTP_METHOD_POST;
	else if (MtStr == "PUT")
		hdr.method = HTTP_METHOD_PUT;
	else if (MtStr == "DELETE")
		hdr.method = HTTP_METHOD_DELETE;
	else if (MtStr == "HEAD")
		hdr.method = HTTP_METHOD_HEAD;
	else if (MtStr == "CONNECT")
		hdr.method = HTTP_METHOD_CONNECT;
	else if (MtStr == "OPTIONS")
		hdr.method = HTTP_METHOD_OPTIONS;
	else if (MtStr == "TRACE")
		hdr.method = HTTP_METHOD_TRACE;
	else if (MtStr == "PATCH")
		hdr.method = HTTP_METHOD_PATCH;
	else
		hdr.method = HTTP_METHOD_UNKNOW;

	HttpHeader = HttpHeader.substr(Pos + 2);
	Pos = HttpHeader.find(" ");
	string GetParams = HttpHeader.substr(0, Pos);
	hdr.http_request.add_decode("OPath", GetParams);
	hdr.http_request.add_decode("Path", GetParams.substr(0, GetParams.find("?")));
	GetParams = GetParams.substr(GetParams.find("?") + 1);
	hdr.http_get_param.parse(GetParams, "&", true);
	HttpHeader = HttpHeader.substr(Pos + 6);
	Pos = HttpHeader.find("\r\n");
	hdr.http_request.add("Version", HttpHeader.substr(0, Pos));
	HttpHeader = HttpHeader.substr(Pos + 2);
	while ((Pos = HttpHeader.find(": ")) != string::npos)
	{
		hdr.http_request.add(HttpHeader.substr(0, Pos), HttpHeader.substr(Pos + 2, HttpHeader.find("\r\n") - (Pos + 2)));
		HttpHeader = HttpHeader.substr(HttpHeader.find("\r\n") + 2);
	}
	HttpHeader = HttpHeader.substr(HttpHeader.find("\r\n") + 2);
	hdr.http_post_param.parse(HttpHeader, "&", true);
	if (hdr.http_request.exist("Cookie"))
	{
		wstring Cookiest = hdr.http_request["Cookie"];
		hdr.http_request.erase("Cookie");
		hdr.cookies.parse(Cookiest, "; ");
	}
	if (search(hdr.http_request.get_utf8("User-Agent"), "iPad") != string::npos || search(hdr.http_request.get_utf8("User-Agent"), "iPhone") != string::npos || search(hdr.http_request.get_utf8("User-Agent"), "Android") != string::npos)
		hdr.is_mobile = true;
	else
		hdr.is_mobile = false;
}

bool ReciveHTTPRequest(SOCKET sock, string& RequestHTTP)
{
	int err = 0;
	bool IsSuccess = true;
	std::vector<unsigned char> HttpRequest(4096);
	memset(&HttpRequest[0], 0, HttpRequest.size());
	size_t nCurrentPos = 0;
	while (nCurrentPos < HttpRequest.size() - 1)
	{
		err = recv(sock, (char*)&HttpRequest[nCurrentPos], (int)(HttpRequest.size() - nCurrentPos - 1), NULL);
		if (err > 0)
		{
			nCurrentPos += err;
			const string strInputString((const char*)&HttpRequest[0]);
			if (strInputString.find("\r\n\r\n") != string::npos)
				break;
			continue;
		}
	}
	if (IsSuccess == false || (string((const char*)&HttpRequest[0]).find("GET") == string::npos && string((const char*)&HttpRequest[0]).find("POST") == string::npos))
		return false;

	RequestHTTP = string((const char*)&HttpRequest[0]);
	return true;
}

bool SendHTTTPAnswerWithData(SOCKET sock, string Data)
{
	ostringstream HttpAnswer;
	HttpAnswer << "HTTP/1.1 200 OK\r\n"
		<< "Server: Apache\r\n"
		<< "Date: " << GetTimeGMT(0) << "\r\n"
		<< "Content-Type: text/html; charset=utf-8\r\n"
		<< "Content-Length: " << Data.size() << "\r\n"
		<< "Connection: keep-alive\r\n"
		<< "X-Powered-By: PHP/3.15174\r\n"
		<< "\r\n";
	send(sock, HttpAnswer.str().c_str(), (int)HttpAnswer.str().length(), 0);
	send(sock, (char*)(Data.data()), (int)Data.size(), 0);
	return true;
}

std::string filename(const std::string& fname)
{
	const char* FnS = "\\/";
	size_t pos = fname.find_last_of(FnS);
	return (std::string::npos == pos)
		? fname
		: fname.substr(pos + 1);
}

string GetFileData(wstring File)
{
	FILE* fl = NULL;
	if (_wfopen_s(&fl, File.c_str(), L"rb") != 0) {
		return "";
	}
	if (!fl) {
		return "";
	}
	fseek(fl, 0, SEEK_END);
	int Size = ftell(fl);
	if (Size <= 0)
	{
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
	: CDialogEx(SSLRequestDialog::IDD, pParent)
{
	m_hIcon = nullptr;
	memset(szBuf, 0, sizeof(szBuf));
}

SSLRequestDialog::~SSLRequestDialog()
{
}

void SSLRequestDialog::DoDataExchange(CDataExchange* pDX)
{
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

BOOL SSLRequestDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	XTPSkinMgr()->AlphaEnableWindow(m_hWnd, 220);
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON1);
	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);

	EURL.SetWindowTextW(L"https://punchsalad.com/ssl-certificate-generator/");
	return TRUE;
}

void SSLRequestDialog::OnSysCommand(UINT nID, LPARAM lParam)
{
	CDialogEx::OnSysCommand(nID, lParam);
}

void SSLRequestDialog::OnPaint()
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

void SetTimeOutsSocket(SOCKET Socket)
{
	DWORD timeout = 5000;
	setsockopt(Socket, SOL_SOCKET, SO_SNDTIMEO, (char*)&timeout, sizeof(timeout));
	setsockopt(Socket, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));
}

DWORD WINAPI ClientConnected(LPVOID arg)
{
	SOCKET client_sock = extractThreadParamAndDelete<SOCKET>(arg);
	SetTimeOutsSocket(client_sock);
	string HttpRequest;
	ReciveHTTPRequest(client_sock, HttpRequest);
	RequestParserStruct Req;
	HTTPParse(HttpRequest, Req, "");
	wstring ResultFile;
	for (auto& i : Files)
	{
		if (Req.http_request.get_utf8("Path").find(filename(wchar_to_UTF8(i))) == string::npos)
			continue;
		ResultFile = i;
	}
	if (ResultFile.length() <= 0)
	{
		SendHTTTPAnswerWithData(client_sock, "Error");
		shutdown(client_sock, 1);
		closesocket(client_sock);
		return -1;
	}
	string ResultA = GetFileData(ResultFile);
	SendHTTTPAnswerWithData(client_sock, ResultA);
	shutdown(client_sock, 1);
	closesocket(client_sock);
	return 0;
}

DWORD WINAPI Server(LPVOID)
{
	SOCKET server_sock = 0, client_sock = 0;
	server_sock = tcp_listen(Port);
	if (server_sock == INVALID_SOCKET) {
		(win_message().timeout(5).message_type(MSG_TYPE::TYPE_ERROR) << L"Сбой в инициализации сервера(socket bind)").show();
		return -1;
	}
	sockaddr_in FromAddr;
	int len = sizeof(FromAddr);
	while (true)
	{
		client_sock = accept(server_sock, (struct sockaddr*)&FromAddr, &len);
		if (client_sock <= 0)
			continue;
		SOCKET* tmp = new SOCKET();
		*tmp = client_sock;
		CreateThreadDetachedSimple(&ClientConnected, tmp);
	}
	closesocket(server_sock);
	(win_message().message_type(MSG_TYPE::TYPE_ERROR) << L"Сбой!").show();
	return 0;
}

void SSLRequestDialog::OnStart()
{
	win_message msg(m_hWnd);
	msg.message_type(MSG_TYPE::TYPE_ERROR);

	Files.clear();
	CString Temp;
	File1.GetWindowTextW(Temp);
	if (Temp.GetLength() <= 0)
	{
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
	if (Port <= 0 || Port > 65535)
	{
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

HCURSOR SSLRequestDialog::OnQueryDragIcon()
{
	return (HCURSOR)m_hIcon;
}

void SSLRequestDialog::OnClose()
{
	XTPSkinMgr()->ExitProgramm();
}

BOOL SSLRequestDialog::PreTranslateMessage(MSG* pMsg)
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
				else if (pMsg->hwnd == File2.GetSafeHwnd())
					File2.SetWindowTextW(szBuf);
				else
					File3.SetWindowTextW(szBuf);
			}
		}
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}