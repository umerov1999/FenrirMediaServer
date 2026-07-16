#include "pch.h"
#include <iostream>
#include <vector>
#include <iomanip>
#include <sstream>
#include "Map.h"
#include <regex>
#include <Winsock2.h>
#include <ws2tcpip.h>
#include <Windows.h>
#include <Shlwapi.h>
#include <openssl/rsa.h>
#include <openssl/crypto.h>
#include <openssl/asn1.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include "FlBin.h"
#include "resource.h"
#include "MediaServerTask.h"
#include "MediaServerDialog.h"
#include <io.h>
#include <fcntl.h>
#include "json.hpp"
#include "WSTRUtils.h"
#include "zstd.h"
#include "win_api_utils.h"
#include "do_curl.h"
#include "../../Third-Party/USound/src/USound.h"
#include "vk_api_interface.h"
using namespace std;
using namespace nlohmann;
using namespace WSTRUtils;

SOCKET HTTPSserver_sock = 0;
USSL_CERT CertificateData;
static ServerMethods serverMethods;

static USound player;

extern std::wstring ExtractAppPath();
extern MediaServerDialog dlgS;
extern void PrintMessage(const wstring &Msg, URGB Color = URGB(255, 255, 255));
extern void ClearMessages();
extern void ClearLine();

static Map::Map<std::string, Audio> mDiscography;
static Map::Map<std::string, Audio> mAudios;
static Map::Map<std::string, Video> mVideos;
static Map::Map<std::string, Photo> mPhotos;

static int PasswordWrongCount = 0;

list<wstring> Discography_Dirs;
list<wstring> Audio_Dirs;
list<wstring> Video_Dirs;
list<wstring> Photo_Video_Dirs;
void* template_ff = nullptr;

static Inode RootDir;

EXTERN_THREAD_ACCESS_GUARD_OBJECT

InitServer Startinit;

class Connection {
public:
	Connection(SOCKET_C socket, SSL* ssl, const std::string& connection, const InitServer& server_config) {
		this->socket = socket;
		this->ssl = ssl;
		this->connection = connection;
		this->server_config = server_config;
	}
	bool isSSL() const {
		return ssl != nullptr;
	}
	std::string connection;
	SSL* ssl;
	SOCKET_C socket;
	InitServer server_config;
};

typedef bool(*method_t)(RequestParserStruct& Req, const Connection& client);

void wstringparam::add(const string& name, const wstring& value) {
	paramsdata[name] = value;
}
void wstringparam::add_decode(const string& name, const string& value) {
	paramsdata[name] = UTF8_to_wchar(url_decode(value));
}
void wstringparam::add(const string& name, const string& value) {
	paramsdata[name] = UTF8_to_wchar(value);
}
void wstringparam::parse(const string& paramstr, const string& separator, bool need_url_decode) {
	string paramstring = paramstr;
	while (!paramstring.empty()) {
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
void wstringparam::parse(const wstring& paramstring, const string& separator) {
	parse(wchar_to_UTF8(paramstring), separator, false);
}
int wstringparam::count() const {
	return (int)paramsdata.size();
}
wstring wstringparam::operator[](const string& name) const {
	if (paramsdata.exist(name))
		return paramsdata.at(name);
	return L"";
}
bool wstringparam::exist(const string& name) {
	if (paramsdata.exist(name))
		return true;
	return false;
}
void wstringparam::erase(const string& name) {
	if (paramsdata.exist(name))
		paramsdata.erase(name);
}
string wstringparam::get_utf8(const string& name) const {
	if (paramsdata.exist(name))
		return wchar_to_UTF8(paramsdata.at(name));
	return "";
}
void wstringparam::set_utf8(const string& name, const string& value) {
	if (paramsdata.exist(name))
		paramsdata[name] = UTF8_to_wchar(value);
}

static wstring GetDublicates(int count, const wstring &str) {
	wstring out = L"";
	for (int i = 0; i < count; i++)
		out += str;
	return out;
}

UTF8Stream& UTF8Stream::operator<<(const wstring &String) {
	if (String.length() > 0) {
		string OT = wchar_to_UTF8(String);
		Data += OT;
	}
	return *this;
}
UTF8Stream& UTF8Stream::operator<<(const string &String) {
	if (String.length() > 0)
		Data += String;
	return *this;
}
UTF8Stream& UTF8Stream::operator<<(size_t value) {
	Data += to_string(value);
	return *this;
}
UTF8Stream& UTF8Stream::operator<<(int value) {
	Data += to_string(value);
	return *this;
}
const string &UTF8Stream::GetString() const {
	return Data;
}
int UTF8Stream::GetSize() {
	return (int)Data.length();
}
void UTF8Stream::Clear() {
	Data.clear();
}

static string GetConnectionAddr(const sockaddr_in& FromAddr) {
	string buf;
	buf.resize(260);
	inet_ntop(AF_INET, &FromAddr.sin_addr, (char*)buf.data(), 260);
	buf = buf.c_str();
	buf += string(":") + to_string(ntohs(FromAddr.sin_port));
	return buf;
}

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

static time_t asn1_time_to_time_t(const ASN1_TIME* time) {
	struct tm tm;
	if (ASN1_TIME_to_tm(time, &tm) != 1) {
		return (time_t)-1; // error
	}

	return mktime(&tm);
}

inline void PrintSertValid(X509* Sert, const wstring& Name) {
	PrintMessage(L"Сертификат " + Name + L" Действителен с " + GetTimeAT(asn1_time_to_time_t(X509_get_notBefore(Sert))) + L" по " + GetTimeAT(asn1_time_to_time_t(X509_get_notAfter(Sert))), URGB(140, 140, 140));
}

static SSL_CTX* initialize_ctx(bool print_sert, bool server = true) {
	auto CA_CERT = std::shared_ptr<BIO>(BIO_new_mem_buf(CertificateData.CertPart[USSL_CERTPART_CERT].data(), (int)CertificateData.CertPart[USSL_CERTPART_CERT].size()), BIO_free);
	auto CA_KEY = std::shared_ptr<BIO>(BIO_new_mem_buf(CertificateData.CertPart[USSL_CERTPART_KEY].data(), (int)CertificateData.CertPart[USSL_CERTPART_KEY].size()), BIO_free);
	SSL_CTX* ctx = SSL_CTX_new(server ? TLS_server_method() : TLS_client_method());
	BIO* CA_BUNDLE = nullptr;
	X509* bundle = nullptr;
	X509* cert = nullptr;
	EVP_PKEY* rsa = nullptr;
	X509* p = nullptr;
	bool first = true;
	int bundle_part = 0;

	if (!ctx) {
		goto error;
	}
	
	while ((p = PEM_read_bio_X509(CA_CERT.get(), nullptr, 0, nullptr))) {
		if (first) {
			if (SSL_CTX_use_certificate(ctx, p) <= 0) {
				goto error;
			}
			first = false;
			if (print_sert) {
				PrintSertValid(p, L"Certificate.crt");
			}
		}
		else {
			if (SSL_CTX_add_extra_chain_cert(ctx, p) <= 0)
				goto error;
			if (print_sert) {
				PrintSertValid(p, L"Ca_Bundle[" + to_wstring(++bundle_part) + L"].crt");
			}
		}
	}
	rsa = PEM_read_bio_PrivateKey(CA_KEY.get(), nullptr, 0, nullptr);

	if (first || !rsa)
		goto error;
	if (CertificateData.CertPart.exist(USSL_CERTPART_BUNDLE) && !CertificateData.CertPart[USSL_CERTPART_BUNDLE].empty()) {
		CA_BUNDLE = BIO_new_mem_buf(CertificateData.CertPart[USSL_CERTPART_BUNDLE].data(), (int)CertificateData.CertPart[USSL_CERTPART_BUNDLE].size());
		if (!CA_BUNDLE) {
			goto error;
		}
		while ((p = PEM_read_bio_X509(CA_BUNDLE, nullptr, 0, nullptr))) {
			if (SSL_CTX_add_extra_chain_cert(ctx, p) <= 0) {
				BIO_free(CA_BUNDLE);
				goto error;
			}

			if (print_sert) {
				PrintSertValid(p, L"Ca_Bundle[" + to_wstring(++bundle_part) + L"].crt");
			}
		}
		BIO_free(CA_BUNDLE);
	}
	if (SSL_CTX_use_PrivateKey(ctx, rsa) <= 0) {
		goto error;
	}
	if (print_sert) {
		PrintMessage(L"    #Домен " + UTF8_to_wchar(CertificateData.Name), URGB(180, 0, 0));
	}
	return ctx;
error:
	(win_message().timeout(5).message_type(MSG_TYPE::TYPE_ERROR) << L"OpenSSL Key Error : Невозможно создать Server CTX").show();
	return nullptr;
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


static bool SSL_SendData(const Connection& client, const void* Buffer, int Len, int timeOutSec) {
	if (!client.ssl) {
		return SendNetBinary(client.socket, Buffer, Len, timeOutSec);
	}
#if defined(__linux__)
	struct timeval timeout {};
	timeout.tv_sec = timeOutSec;
	timeout.tv_usec = 0;
#else
	DWORD timeout = (DWORD)(timeOutSec * 1000);
#endif
	setsockopt(client.socket, SOL_SOCKET, SO_SNDTIMEO, (char*)&timeout, sizeof(timeout));
	setsockopt(client.socket, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));

	int nCurrentPos = 0;
	int err = 0;
	while (nCurrentPos < Len) {
		err = SSL_write(client.ssl, (char*)Buffer + nCurrentPos, Len - nCurrentPos);
		if (err > 0) {
			nCurrentPos += err;
			continue;
		}
		const int nCode = SSL_get_error(client.ssl, err);
		if ((nCode != SSL_ERROR_WANT_READ) && (nCode != SSL_ERROR_WANT_WRITE)) {
			return false;
		}
	}
	return true;
}

static bool SSL_SendData(const Connection& client, const string &Buffer, int timeOutSec) {
	return SSL_SendData(client, Buffer.data(), (int)Buffer.size(), SOCKTIMEOUT);
}

static bool SSL_RecvData(const Connection& client, const void* Buffer, int Len, int timeOutSec) {
	if (!client.ssl) {
		return ReciveNetBinary(client.socket, Buffer, Len, timeOutSec);
	}
#if defined(__linux__)
	struct timeval timeout {};
	timeout.tv_sec = timeOutSec;
	timeout.tv_usec = 0;
#else
	DWORD timeout = (DWORD)(timeOutSec * 1000);
#endif
	setsockopt(client.socket, SOL_SOCKET, SO_SNDTIMEO, (char*)&timeout, sizeof(timeout));
	setsockopt(client.socket, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));

	int nCurrentPos = 0;
	int err = 0;
	while (nCurrentPos < Len) {
		err = SSL_read(client.ssl, (char*)Buffer + nCurrentPos, Len - nCurrentPos);
		if (err > 0) {
			nCurrentPos += err;
			continue;
		}
		const int nCode = SSL_get_error(client.ssl, err);
		if ((nCode != SSL_ERROR_WANT_READ) && (nCode != SSL_ERROR_WANT_WRITE)) {
			return false;
		}
	}
	return true;
}

static void client_shutdown(Connection& client, int how) {
	if (client.ssl) {
		SSL_shutdown(client.ssl);
		SSL_free(client.ssl);
		client.ssl = nullptr;
	}
	shutdown(client.socket, how);
	closesocket(client.socket);
	client.socket = SOCKET_ERROR;
}

static bool SSL_RecvDataString(const Connection& client, string& BinData, int Size, int timeOutSec) {
	BinData.resize((size_t)Size);
	return SSL_RecvData(client, BinData.data(), Size, timeOutSec);
}

static bool exist_get_post(RequestParserStruct& Req, const string& str) {
	return Req.http_get_param.exist(str) || Req.http_post_param.exist(str);
}

static wstring get_post(RequestParserStruct& Req, const string& str) {
	if (Req.http_get_param.exist(str)) {
		return Req.http_get_param[str];
	}
	return Req.http_post_param[str];
}

static string get_postUTF8(RequestParserStruct& Req, const string& str) {
	if (Req.http_get_param.exist(str)) {
		return Req.http_get_param.get_utf8(str);
	}
	return Req.http_post_param.get_utf8(str);
}

static bool SendHTTTPAnswerWithData(const Connection& client, const string &CodeAnsw, const string &ContentType, const string& Data) {
	ostringstream HttpAnswer;
	HttpAnswer << "HTTP/1.1 " << CodeAnsw << ENDL
		<< "Server: " << SERVER_NAME << ENDL
		<< "Date: " << GetTimeGMT(0) << ENDL
		<< "Content-Type: " << ContentType << ENDL;
	if (Data.size() > 0)
		HttpAnswer << "Content-Length: " << Data.size() << ENDL;
	HttpAnswer << "Connection: close" << ENDL;
	HttpAnswer << ENDL;
	if (!SSL_SendData(client, HttpAnswer.str(), SOCKTIMEOUT) || !SSL_SendData(client, Data, SOCKTIMEOUT))
		return false;
	return true;
}

static bool SendHTTTPAnswerWithTree(const Connection &client, RequestParserStruct &Req, const string &CodeAnsw, const json &tree) {
	bool isMessagePack = Req.http_request.exist("X-Response-Format") && Req.http_request.get_utf8("X-Response-Format") == "msgpack";
	bool supportCompressZstd = Req.http_request.exist("Accept-Encoding") && Req.http_request.get_utf8("Accept-Encoding").find("zstd") != string::npos;
	ostringstream HttpAnswer;
	HttpAnswer << "HTTP/1.1 " << CodeAnsw << ENDL
		<< "Server: " << SERVER_NAME << ENDL
		<< "Date: " << GetTimeGMT(0) << ENDL;

	string src;
	string dst;
	if (isMessagePack) {
		auto v = json::to_msgpack(tree);
		src.resize(v.size());
		memcpy(src.data(), v.data(), v.size());
		HttpAnswer << "Content-Type: " << "application/x-msgpack; charset=utf-8" << ENDL;
	}
	else {
		src = tree.dump();
		HttpAnswer << "Content-Type: " << "application/json; charset=utf-8" << ENDL;
	}
	bool compressed = false;
	if (supportCompressZstd) {
		size_t const cBuffSize = ZSTD_compressBound(src.size());
		dst.resize(cBuffSize);
		auto dstp = const_cast<void*>(static_cast<const void*>(dst.c_str()));
		auto srcp = static_cast<const void*>(src.c_str());
		size_t const cSize = ZSTD_compress(dstp, cBuffSize, srcp, src.size(), ZSTD_fast);
		auto code = ZSTD_isError(cSize);
		if (!code) {
			dst.resize(cSize);
		}
		compressed = true;
		HttpAnswer << "Content-Encoding: zstd" << ENDL;
	}
	if (compressed) {
		if (dst.size() > 0)
			HttpAnswer << "Content-Length: " << dst.size() << ENDL;
		HttpAnswer << "Connection: close" << ENDL
			<< "Cache-Control: no-cache, no-store, must-revalidate" << ENDL
			<< ENDL;
		if (!SSL_SendData(client, HttpAnswer.str(), SOCKTIMEOUT) || !SSL_SendData(client, dst, SOCKTIMEOUT))
			return false;
	}
	else {
		if (src.size() > 0)
			HttpAnswer << "Content-Length: " << src.size() << ENDL;
		HttpAnswer << "Connection: close" << ENDL
			<< "Cache-Control: no-cache, no-store, must-revalidate" << ENDL
			<< ENDL;
		if (!SSL_SendData(client, HttpAnswer.str(), SOCKTIMEOUT) || !SSL_SendData(client, src, SOCKTIMEOUT))
			return false;
	}
	return true;
}

static int from_get_post_int(RequestParserStruct& Req, const string& str, int default_param = 0) {
	if (!exist_get_post(Req, str)) {
		return default_param;
	}
	try {
		return stoi(get_post(Req, str));
	}
	catch (...) {
		return default_param;
	}
}

static int64_t from_get_post_int64(RequestParserStruct& Req, const string& str, int64_t default_param = 0) {
	if (!exist_get_post(Req, str)) {
		return default_param;
	}
	try {
		return stoll(get_post(Req, str));
	}
	catch (...) {
		return default_param;
	}
}

static size_t from_get_post_size_t(RequestParserStruct& Req, const string& str, size_t default_param = 0) {
	if (!exist_get_post(Req, str)) {
		return default_param;
	}
	try {
		long long v = stoll(get_post(Req, str));
		if (v < 0) {
			return default_param;
		}
		return (size_t)v;
	}
	catch (...) {
		return default_param;
	}
}

static bool from_get_post_bool(RequestParserStruct& Req, const string& str, bool default_param = false) {
	if (!exist_get_post(Req, str)) {
		return default_param;
	}
	try {
		return stoi(get_post(Req, str)) != 0;
	}
	catch (...) {
		return default_param;
	}
}

template<typename T>
static bool SendSingleResponseJSON(const Connection &client, RequestParserStruct& Req, T status) {
	json response = json::object();
	response.emplace("response", status);
	return SendHTTTPAnswerWithTree(client, Req, "200 OK", response);
}

template<typename T>
static bool SendPendingSingleResponseJSON(const Connection& client, RequestParserStruct& Req, T status) {
	json response = json::object();
	response.emplace("response", status);
	response.emplace("requested", 1);
	return SendHTTTPAnswerWithTree(client, Req, "200 OK", response);
}

static bool SendItemsJSON(const Connection& client, RequestParserStruct& Req, size_t elements, const json& tree) {
	json response = json::object();
	json info = json::object();
	info.emplace("count", elements);
	info.emplace("items", tree);
	response.emplace("response", info);
	return SendHTTTPAnswerWithTree(client, Req, "200 OK", response);
}

static bool SendErrorJSON(const Connection& client, RequestParserStruct& Req, const wstring& error) {
	json response = json::object();
	json error_t = json::object();
	error_t.emplace("error_code", 3);
	error_t.emplace("error_msg", wchar_to_UTF8(error));
	response.emplace("error", error_t);
	return SendHTTTPAnswerWithTree(client, Req, "200 OK", response);
}

wstring Media::get_cover() const {
	return wstring(CACHE_DIR) + L"\\" + UTF8_to_wchar(get_hash()) + L".jpg";
}

static wstring get_cover(const std::string &hash) {
	return wstring(CACHE_DIR) + L"\\" + UTF8_to_wchar(hash) + L".jpg";
}

static json makeAudio(const Audio &a, const RequestParserStruct& Req, bool isSSL) {
	json data = json(json::value_t::object);
	data.emplace("id", a.getId());
	data.emplace("owner_id", a.getOwnerId());
	data.emplace("title", a.get_title());
	data.emplace("artist", a.get_artist());
	data.emplace("url", (string)(isSSL ? "https" : "http") + "://" + Req.http_request.get_utf8("Host") + "/method/get_media?hash=" + a.get_hash());
	data.emplace("duration", a.get_file_size());

	if (PathFileExistsW(a.get_cover().c_str())) {
		json cover = json(json::value_t::object);
		cover.emplace("photo_600", (string)(isSSL ? "https" : "http") + "://" + Req.http_request.get_utf8("Host") + "/method/get_cover?hash=" + a.get_hash());
		cover.emplace("photo_135", (string)(isSSL ? "https" : "http") + "://" + Req.http_request.get_utf8("Host") + "/method/get_cover?hash=" + a.get_hash());
		json album = json(json::value_t::object);
		album.emplace("thumb", cover);
		data.emplace("album", album);
	}
	return data;
}

static json makeVideo(const Video& v, const RequestParserStruct& Req, bool isSSL) {
	json data = json(json::value_t::object);
	data.emplace("id", v.getId());
	data.emplace("owner_id", v.getOwnerId());
	data.emplace("title", v.get_title());
	data.emplace("description", v.get_description());
	data.emplace("duration", v.get_file_size());

	json files = json(json::value_t::object);
	files.emplace("mp4_720", (string)(isSSL ? "https" : "http") + "://" + Req.http_request.get_utf8("Host") + "/method/get_media?hash=" + v.get_hash());
	data.emplace("files", files);

	if (PathFileExistsW(v.get_cover().c_str())) {
		json cover = json(json::value_t::object);
		cover.emplace("width", 900);
		cover.emplace("height", 450);
		cover.emplace("url", (string)(isSSL ? "https" : "http") + "://" + Req.http_request.get_utf8("Host") + "/method/get_cover?hash=" + v.get_hash());
		json album = json(json::value_t::array);
		album.push_back(cover);
		data.emplace("image", album);
	}
	return data;
}

static json makePhoto(const Photo& p, const RequestParserStruct& Req, bool isSSL) {
	json data = json(json::value_t::object);
	data.emplace("id", p.getId());
	data.emplace("owner_id", p.getOwnerId());
	data.emplace("album_id", -311);
	data.emplace("date", p.get_mod_time());
	data.emplace("text", p.get_text());

	json sizes = json(json::value_t::array);
	json W = json(json::value_t::object);
	W.emplace("type", "w");
	W.emplace("width", "2080");
	W.emplace("height", "1080");
	W.emplace("url", (string)(isSSL ? "https" : "http") + "://" + Req.http_request.get_utf8("Host") + "/method/get_media?hash=" + p.get_hash());
	sizes.push_back(W);

	json S = json(json::value_t::object);
	S.emplace("type", "s");
	S.emplace("width", "512");
	S.emplace("height", "512");
	if (PathFileExistsW(p.get_cover().c_str())) {
		S.emplace("url", (string)(isSSL ? "https" : "http") + "://" + Req.http_request.get_utf8("Host") + "/method/get_cover?hash=" + p.get_hash());
	}

	sizes.push_back(S);

	data.emplace("sizes", sizes);
	return data;
}

static bool AudioGet(RequestParserStruct& Req, const Connection& client) {
	THREAD_ACCESS_LOCK(DEFAULT_GUARD_NAME, &mAudios);
	size_t count = from_get_post_size_t(Req, "count", 0);
	size_t offset = from_get_post_size_t(Req, "offset", 0);
	bool reverse = from_get_post_bool(Req, "reverse", false);
	if (count < 0 || count > mAudios.size()) {
		count = 0;
	}
	if (offset < 0) {
		offset = 0;
	}
	json arr = json(json::value_t::array);

	if (offset <= mAudios.size() - 1) {
		if (reverse) {
			auto it = mAudios().rbegin();
			for (size_t i = 0; i < offset; i++) {
				it++;
			}
			for (size_t i = 0; (it != mAudios().rend() && (count > 0 ? i < count : true)); i++, it++) {
				arr.push_back(makeAudio((*it).get_value(), Req, client.isSSL()));
			}
		}
		else {
			auto it = mAudios().begin();
			for (size_t i = 0; i < offset; i++) {
				it++;
			}
			for (size_t i = 0; (it != mAudios().end() && (count > 0 ? i < count : true)); i++, it++) {
				arr.push_back(makeAudio((*it).get_value(), Req, client.isSSL()));
			}
		}
	}
	auto z_count = mAudios.size();
	THREAD_ACCESS_LOCK(DEFAULT_GUARD_NAME, &mAudios);
	return SendItemsJSON(client, Req, z_count, arr);
}

static bool AudioList(RequestParserStruct& Req, const Connection& client) {
	THREAD_ACCESS_LOCK(DEFAULT_GUARD_NAME, &mAudios);

	json arr = json(json::value_t::array);

	for (auto &i : mAudios) {
		arr.push_back(i.get_value().get_artist() + " - " + i.get_value().get_title() + "." + wchar_to_UTF8(Media::get_ext(i.get_value().get_orig_name())));
	}
	THREAD_ACCESS_LOCK(DEFAULT_GUARD_NAME, &mAudios);
	return SendHTTTPAnswerWithData(client, "200 OK", "application/json; charset=utf-8", arr.dump());
}

static bool AudioRemotePlay(RequestParserStruct& Req, const Connection& client) {
	bool ret = false;
	THREAD_ACCESS_LOCK(DEFAULT_GUARD_NAME, &player);
	if (Req.multi_part.exist("audio")) {
		player.Stop();
		player.ClearMemorySound();
		player.RegisterResourceMP3Sounds(Req.multi_part.get_data("audio").data);
		player.PlayMemorySound(false, false);
		dlgS.ToggleStopAudio(true);
		ret = SendSingleResponseJSON(client, Req, 1);
	} else {
		player.Stop();
		player.ClearMemorySound();
		dlgS.ToggleStopAudio(false);
		ret = SendSingleResponseJSON(client, Req, 0);
	}
	THREAD_ACCESS_UNLOCK(DEFAULT_GUARD_NAME, &player);
	return ret;
}

static bool DiscographyGet(RequestParserStruct& Req, const Connection& client) {
	THREAD_ACCESS_LOCK(DEFAULT_GUARD_NAME, &mDiscography);
	size_t count = from_get_post_size_t(Req, "count", 0);
	size_t offset = from_get_post_size_t(Req, "offset", 0);
	bool reverse = from_get_post_bool(Req, "reverse", false);
	if (count < 0 || count > mDiscography.size()) {
		count = 0;
	}
	if (offset < 0) {
		offset = 0;
	}
	json arr = json(json::value_t::array);

	if (offset <= mDiscography.size() - 1) {
		if (reverse) {
			auto it = mDiscography().rbegin();
			for (size_t i = 0; i < offset; i++) {
				it++;
			}
			for (size_t i = 0; (it != mDiscography().rend() && (count > 0 ? i < count : true)); i++, it++) {
				arr.push_back(makeAudio((*it).get_value(), Req, client.isSSL()));
			}
		}
		else {
			auto it = mDiscography().begin();
			for (size_t i = 0; i < offset; i++) {
				it++;
			}
			for (size_t i = 0; (it != mDiscography().end() && (count > 0 ? i < count : true)); i++, it++) {
				arr.push_back(makeAudio((*it).get_value(), Req, client.isSSL()));
			}
		}
	}
	auto z_count = mDiscography.size();
	THREAD_ACCESS_LOCK(DEFAULT_GUARD_NAME, &mDiscography);
	return SendItemsJSON(client, Req, z_count, arr);
}

static bool PhotosGet(RequestParserStruct& Req, const Connection& client) {
	THREAD_ACCESS_LOCK(DEFAULT_GUARD_NAME, &mPhotos);
	size_t count = from_get_post_size_t(Req, "count", 0);
	size_t offset = from_get_post_size_t(Req, "offset", 0);
	bool reverse = from_get_post_bool(Req, "reverse", false);
	if (count < 0 || count > mPhotos.size()) {
		count = 0;
	}
	if (offset < 0) {
		offset = 0;
	}
	json arr = json(json::value_t::array);

	if (offset <= mPhotos.size() - 1) {
		if (reverse) {
			auto it = mPhotos().rbegin();
			for (size_t i = 0; i < offset; i++) {
				it++;
			}
			for (size_t i = 0; (it != mPhotos().rend() && (count > 0 ? i < count : true)); i++, it++) {
				arr.push_back(makePhoto((*it).get_value(), Req, client.isSSL()));
			}
		}
		else {
			auto it = mPhotos().begin();
			for (size_t i = 0; i < offset; i++) {
				it++;
			}
			for (size_t i = 0; (it != mPhotos().end() && (count > 0 ? i < count : true)); i++, it++) {
				arr.push_back(makePhoto((*it).get_value(), Req, client.isSSL()));
			}
		}
	}
	auto z_count = mPhotos.size();
	THREAD_ACCESS_LOCK(DEFAULT_GUARD_NAME, &mPhotos);
	return SendItemsJSON(client, Req, z_count, arr);
}

static bool AudioSearch(RequestParserStruct& Req, const Connection& client) {
	string q = trim(get_postUTF8(Req, "q"));
	if (q.length() <= 0) {
		return AudioGet(Req, client);
	}
	bool reverse = from_get_post_bool(Req, "reverse", false);
	list<Audio> result;
	THREAD_ACCESS_LOCK(DEFAULT_GUARD_NAME, &mAudios);
	if (reverse) {
		for (auto it = mAudios().rbegin(); it != mAudios().rend(); it++) {
			if (WSTRUtils::wsearch(WSTRUtils::UTF8_to_wchar(it->get_value().get_artist()), WSTRUtils::UTF8_to_wchar(q)) != string::npos || WSTRUtils::wsearch(WSTRUtils::UTF8_to_wchar(it->get_value().get_title()), WSTRUtils::UTF8_to_wchar(q)) != string::npos) {
				result.push_back(it->get_value());
			}
		}
	}
	else {
		for (auto& i : mAudios) {
			if (WSTRUtils::wsearch(WSTRUtils::UTF8_to_wchar(i.get_value().get_artist()), WSTRUtils::UTF8_to_wchar(q)) != string::npos || WSTRUtils::wsearch(WSTRUtils::UTF8_to_wchar(i.get_value().get_title()), WSTRUtils::UTF8_to_wchar(q)) != string::npos) {
				result.push_back(i.get_value());
			}
		}
	}
	THREAD_ACCESS_UNLOCK(DEFAULT_GUARD_NAME, &mAudios);

	size_t count = from_get_post_size_t(Req, "count", 0);
	size_t offset = from_get_post_size_t(Req, "offset", 0);
	if (count < 0 || count > result.size()) {
		count = 0;
	}
	if (offset < 0) {
		offset = 0;
	}
	json arr = json(json::value_t::array);

	if (offset <= result.size() - 1) {
		auto it = result.begin();
		for (size_t i = 0; i < offset; i++) {
			it++;
		}
		for (size_t i = 0; (it != result.end() && (count > 0 ? i < count : true)); i++, it++) {
			arr.push_back(makeAudio(*it, Req, client.isSSL()));
		}
	}
	return SendItemsJSON(client, Req, result.size(), arr);
}

static bool DiscographySearch(RequestParserStruct& Req, const Connection& client) {
	string q = trim(get_postUTF8(Req, "q"));
	if (q.length() <= 0) {
		return DiscographyGet(Req, client);
	}
	bool reverse = from_get_post_bool(Req, "reverse", false);
	list<Audio> result;
	THREAD_ACCESS_LOCK(DEFAULT_GUARD_NAME, &mDiscography);
	if (reverse) {
		for (auto it = mDiscography().rbegin(); it != mDiscography().rend(); it++) {
			if (WSTRUtils::wsearch(WSTRUtils::UTF8_to_wchar(it->get_value().get_artist()), WSTRUtils::UTF8_to_wchar(q)) != string::npos || WSTRUtils::wsearch(WSTRUtils::UTF8_to_wchar(it->get_value().get_title()), WSTRUtils::UTF8_to_wchar(q)) != string::npos) {
				result.push_back(it->get_value());
			}
		}
	}
	else {
		for (auto& i : mDiscography) {
			if (WSTRUtils::wsearch(WSTRUtils::UTF8_to_wchar(i.get_value().get_artist()), WSTRUtils::UTF8_to_wchar(q)) != string::npos || WSTRUtils::wsearch(WSTRUtils::UTF8_to_wchar(i.get_value().get_title()), WSTRUtils::UTF8_to_wchar(q)) != string::npos) {
				result.push_back(i.get_value());
			}
		}
	}
	THREAD_ACCESS_UNLOCK(DEFAULT_GUARD_NAME, &mDiscography);

	size_t count = from_get_post_size_t(Req, "count", 0);
	size_t offset = from_get_post_size_t(Req, "offset", 0);
	if (count < 0 || count > result.size()) {
		count = 0;
	}
	if (offset < 0) {
		offset = 0;
	}
	json arr = json(json::value_t::array);

	if (offset <= result.size() - 1) {
		auto it = result.begin();
		for (size_t i = 0; i < offset; i++) {
			it++;
		}
		for (size_t i = 0; (it != result.end() && (count > 0 ? i < count : true)); i++, it++) {
			arr.push_back(makeAudio(*it, Req, client.isSSL()));
		}
	}
	return SendItemsJSON(client, Req, result.size(), arr);
}

static bool PhotosSearch(RequestParserStruct& Req, const Connection& client) {
	string q = trim(get_postUTF8(Req, "q"));
	if (q.length() <= 0) {
		return PhotosGet(Req, client);
	}
	bool reverse = from_get_post_bool(Req, "reverse", false);
	list<Photo> result;
	THREAD_ACCESS_LOCK(DEFAULT_GUARD_NAME, &mPhotos);
	if (reverse) {
		for (auto it = mPhotos().rbegin(); it != mPhotos().rend(); it++) {
			if (WSTRUtils::wsearch(it->get_value().get_path(), WSTRUtils::UTF8_to_wchar(q)) != string::npos) {
				result.push_back(it->get_value());
			}
		}
	}
	else {
		for (auto& i : mPhotos) {
			if (WSTRUtils::wsearch(i.get_value().get_path(), WSTRUtils::UTF8_to_wchar(q)) != string::npos) {
				result.push_back(i.get_value());
			}
		}
	}
	THREAD_ACCESS_UNLOCK(DEFAULT_GUARD_NAME, &mPhotos);

	size_t count = from_get_post_size_t(Req, "count", 0);
	size_t offset = from_get_post_size_t(Req, "offset", 0);
	if (count < 0 || count > result.size()) {
		count = 0;
	}
	if (offset < 0) {
		offset = 0;
	}
	json arr = json(json::value_t::array);

	if (offset <= result.size() - 1) {
		auto it = result.begin();
		for (size_t i = 0; i < offset; i++) {
			it++;
		}
		for (size_t i = 0; (it != result.end() && (count > 0 ? i < count : true)); i++, it++) {
			arr.push_back(makePhoto(*it, Req, client.isSSL()));
		}
	}
	return SendItemsJSON(client, Req, result.size(), arr);
}

static bool VideoGet(RequestParserStruct& Req, const Connection& client) {
	THREAD_ACCESS_LOCK(DEFAULT_GUARD_NAME, &mVideos);
	size_t count = from_get_post_size_t(Req, "count", 0);
	size_t offset = from_get_post_size_t(Req, "offset", 0);
	bool reverse = from_get_post_bool(Req, "reverse", false);
	if (count < 0 || count > mVideos.size()) {
		count = 0;
	}
	if (offset < 0) {
		offset = 0;
	}
	json arr = json(json::value_t::array);

	if (offset <= mVideos.size() - 1) {
		if (reverse) {
			auto it = mVideos().rbegin();
			for (size_t i = 0; i < offset; i++) {
				it++;
			}
			for (size_t i = 0; (it != mVideos().rend() && (count > 0 ? i < count : true)); i++, it++) {
				arr.push_back(makeVideo((*it).get_value(), Req, client.isSSL()));
			}
		}
		else {
			auto it = mVideos().begin();
			for (size_t i = 0; i < offset; i++) {
				it++;
			}
			for (size_t i = 0; (it != mVideos().end() && (count > 0 ? i < count : true)); i++, it++) {
				arr.push_back(makeVideo((*it).get_value(), Req, client.isSSL()));
			}
		}
	}
	auto z_count = mVideos.size();
	THREAD_ACCESS_UNLOCK(DEFAULT_GUARD_NAME, &mVideos);
	return SendItemsJSON(client, Req, z_count, arr);
}

json Inode::serialize(const RequestParserStruct& Req, bool isSSL, size_t& count) {
	json arr = json(json::value_t::array);
	count = entries.size();
	for (auto& i : entries) {
		if (i.type == InodeType::INODE_FOLDER && i.entries.size() <= 0) {
			continue;
		}
		json data = json(json::value_t::object);
		data.emplace("id", i.id);
		data.emplace("owner_id", i.owner_id);
		data.emplace("file_name", wchar_to_UTF8(i.name));
		data.emplace("type", i.type);
		data.emplace("modification_time", i.modification_time);
		data.emplace("size", i.type == InodeType::INODE_FOLDER ? i.entries.size() : i.size);
		if (i.type != InodeType::INODE_FOLDER) {
			data.emplace("url", (string)(isSSL ? "https" : "http") + "://" + Req.http_request.get_utf8("Host") + "/method/get_media?hash=" + i.hash);
		}

		if (PathFileExistsW((wstring(CACHE_DIR) + L"\\" + UTF8_to_wchar(i.preview_hash) + L".jpg").c_str())) {
			data.emplace("preview_url", (string)(isSSL ? "https" : "http") + "://" + Req.http_request.get_utf8("Host") + "/method/get_cover?hash=" + i.preview_hash);
		}
		arr.push_back(data);
	}
	return arr;
}

static bool FSGet(RequestParserStruct& Req, const Connection& client) {
	THREAD_ACCESS_LOCK(DEFAULT_GUARD_NAME, &RootDir);
	string dir = trim(get_postUTF8(Req, "dir"));
	auto p = RootDir.find_by_path(UTF8_to_wchar(dir));
	if (p == nullptr) {
		THREAD_ACCESS_UNLOCK(DEFAULT_GUARD_NAME, &RootDir);
		return SendErrorJSON(client, Req, L"Неверный путь");
	}
	size_t z_count = 0;
	json payload = p->serialize(Req, client.isSSL(), z_count);
	THREAD_ACCESS_UNLOCK(DEFAULT_GUARD_NAME, &RootDir);
	return SendItemsJSON(client, Req, z_count, payload);
}

static bool rebootPC(RequestParserStruct& Req, const Connection& client) {
	string type = trim(get_postUTF8(Req, "type"));
	bool ret = SendSingleResponseJSON(client, Req, 1);
	if (type == "win") {
		system("shutdown /r");
		dlgS.OnClose();
	}
	else if (type == "linux") {
		system("start /unix /sbin/reboot");
		dlgS.OnClose();
	}
	return ret;
}

static bool VideoSearch(RequestParserStruct& Req, const Connection& client) {
	string q = trim(get_postUTF8(Req, "q"));
	if (q.length() <= 0) {
		return VideoGet(Req, client);
	}
	bool reverse = from_get_post_bool(Req, "reverse", false);
	list<Video> result;
	THREAD_ACCESS_LOCK(DEFAULT_GUARD_NAME, &mVideos);
	if (reverse) {
		for (auto it = mVideos().rbegin(); it != mVideos().rend(); it++) {
			if (WSTRUtils::wsearch(WSTRUtils::UTF8_to_wchar(it->get_value().get_description()), WSTRUtils::UTF8_to_wchar(q)) != string::npos || WSTRUtils::wsearch(WSTRUtils::UTF8_to_wchar(it->get_value().get_title()), WSTRUtils::UTF8_to_wchar(q)) != string::npos) {
				result.push_back(it->get_value());
			}
		}
	}
	else {
		for (auto& i : mVideos) {
			if (WSTRUtils::wsearch(WSTRUtils::UTF8_to_wchar(i.get_value().get_description()), WSTRUtils::UTF8_to_wchar(q)) != string::npos || WSTRUtils::wsearch(WSTRUtils::UTF8_to_wchar(i.get_value().get_title()), WSTRUtils::UTF8_to_wchar(q)) != string::npos) {
				result.push_back(i.get_value());
			}
		}
	}
	THREAD_ACCESS_UNLOCK(DEFAULT_GUARD_NAME, &mVideos);
	size_t count = from_get_post_size_t(Req, "count", 0);
	size_t offset = from_get_post_size_t(Req, "offset", 0);
	if (count < 0 || count > result.size()) {
		count = 0;
	}
	if (offset < 0) {
		offset = 0;
	}
	json arr = json(json::value_t::array);

	if (offset <= result.size() - 1) {
		auto it = result.begin();
		for (size_t i = 0; i < offset; i++) {
			it++;
		}
		for (size_t i = 0; (it != result.end() && (count > 0 ? i < count : true)); i++, it++) {
			arr.push_back(makeVideo(*it, Req, client.isSSL()));
		}
	}
	return SendItemsJSON(client, Req, result.size(), arr);
}

static bool hasRange(const Connection& client, const Media &media, RequestParserStruct& Req, long long& from, long long& to, long long size) {
	from = 0;
	to = 0;
	if (!Req.http_request.exist("Range")) {
		return false;
	}
	wstring range = Req.http_request["Range"];
	size_t rr = range.find(L"bytes=");
	if (rr == wstring::npos) {
		return false;
	}
#ifdef CRITICAL_DEBUG
	if (client.server_config.isDebug) {
		PrintMessage(L"DEBUG: Partal Content " + media.get_path(), URGB(0, 200, 255));
		PrintMessage(L"         " + range + L", Connection: " + UTF8_to_wchar(Req.connection), URGB(0, 200, 255));
	}
#endif
	range.erase(0, 6);
	vector<wstring> res = wsplit(range, L"-");
	bool isLast = (range[0] == '-');
	if (res.size() != 1 && res.size() != 2) {
		return false;
	}
	if (res.size() == 1) {
		if (isLast) {
			long long tmp = stoll(res[0]);
			from = size - 1 - tmp;
			to = size - 1;
			if (from < 0) {
				return false;
			}
			return true;
		}
		else {
			from = stoll(res[0]);
			if (from >= size - 1) {
				return false;
			}
			to = size - 1;
			return true;
		}
	}
	from = stoll(res[0]);
	to = stoll(res[1]);
	if (to > size - 1) {
		to = size - 1;
	}
	return true;
}

static Media* look_MediaByHash(const string& hash) {
	if (mAudios.exist(hash)) {
		return &mAudios[hash];
	}
	else if (mVideos.exist(hash)) {
		return &mVideos[hash];
	}
	else if (mDiscography.exist(hash)) {
		return &mDiscography[hash];
	}
	else if (mPhotos.exist(hash)) {
		return &mPhotos[hash];
	}
	return nullptr;
}

bool VK_APIMETHOD::VK_APIMETHOD_doCapcha(const std::string& captcha_img, const std::string& user_agent, std::string& code) {
	return false;
}

void VK_APIMETHOD::VK_APIMETHOD_doLimit(const std::string& Method) {
}

bool do_convert_mp3(const wstring& in, const wstring& out) {
	PrintMessage(L"Преобразование " + in + L" в " + out);
	PROCESS_INFORMATION piProcInfo;
	STARTUPINFOW siStartInfo;
	ZeroMemory(&piProcInfo, sizeof(PROCESS_INFORMATION));
	ZeroMemory(&siStartInfo, sizeof(STARTUPINFOW));
	siStartInfo.cb = sizeof(STARTUPINFOW);
	wstring crdir;
	crdir.resize(MAX_PATH + 1);
	GetCurrentDirectoryW(MAX_PATH, (LPWSTR)crdir.c_str());
	wstring cmd = L"-loglevel panic -n -i \"" + in + L"\" -ab 320k -acodec libmp3lame -q 1 -map a -id3v2_version 4 \"" + out + L"\"";
	PrintMessage(cmd);
	if (!CreateProcessSimple(ExtractAppPath() + L"\\ffmpeg.exe", cmd,
		&siStartInfo,
		&piProcInfo,
		crdir,
		CREATE_NO_WINDOW))
	{
		return false;
	}
	WaitForSingleObject(piProcInfo.hProcess, INFINITE);
	CloseHandle(piProcInfo.hProcess);
	CloseHandle(piProcInfo.hThread);
	if (!PathFileExistsW(out.c_str())) {
		return false;
	}
	FILE* test;
	if (_wfopen_s(&test, out.c_str(), L"rb") != 0) {
		return false;
	}
	if (!test) {
		return false;
	}
	fseek(test, 0, SEEK_END);
	if (ftell(test) <= 0) {
		fclose(test);
		return false;
	}
	fclose(test);
	return true;
}

static bool AudioUpload(RequestParserStruct& Req, const Connection& client) {
	if (!exist_get_post(Req, "hash")) {
		return SendErrorJSON(client, Req, L"Не передан hash");
	}
	if (!exist_get_post(Req, "access_token")) {
		return SendErrorJSON(client, Req, L"Не передан access_token");
	}
	if (!exist_get_post(Req, "user_agent")) {
		return SendErrorJSON(client, Req, L"Не передан user_agent");
	}
	string hash = get_postUTF8(Req, "hash");
	THREAD_ACCESS_LOCK(DEFAULT_GUARD_NAME, &mDiscography, &mAudios);
	Media* md = look_MediaByHash(hash);
	if (!md) {
		THREAD_ACCESS_UNLOCK(DEFAULT_GUARD_NAME, &mDiscography, &mAudios);
		return SendErrorJSON(client, Req, L"Hash не найден");
	}
	Audio l = (*(Audio*)md);
	wstring pth = (*md).get_path();
	wstring cn = ExtractAppPath() + L"\\converted.mp3";
	THREAD_ACCESS_UNLOCK(DEFAULT_GUARD_NAME, &mDiscography, &mAudios);
	bool ret = SendSingleResponseJSON(client, Req, 1);

	THREAD_ACCESS_LOCK(DEFAULT_GUARD_NAME, &template_ff);
	PrintMessage(L"Загрузка: " + pth, URGB(120, 0, 255));
	DeleteFileW(cn.c_str());
	if (pth.find(L".mp3") == wstring::npos) {
		if (!do_convert_mp3(pth, cn)) {
			DeleteFileW(cn.c_str());
			THREAD_ACCESS_UNLOCK(DEFAULT_GUARD_NAME, &template_ff);
			return ret;
		}
		pth = cn;
	}

	VK_APIMETHOD Akkount(get_postUTF8(Req, "access_token"), get_postUTF8(Req, "user_agent"));
	VKAPI_ANSWER Answer = Akkount["audio.getUploadServer"]();
	if (Answer.IsError || Answer.Object.find("response") == Answer.Object.end() || Answer.Object.at("response").find("upload_url") == Answer.Object.at("response").end()) {
		PrintMessage(L"ERROR: " + UTF8_to_wchar(Answer.Object), URGB(255, 255, 150));
		DeleteFileW(cn.c_str());
		THREAD_ACCESS_UNLOCK(DEFAULT_GUARD_NAME, &template_ff);
		return ret;
	}
	string url = Answer.Object.at("response").at("upload_url").get<string>();
	string rt;
	if (DoCurlMultipart(url, pth, "file", "audio.mp3", get_postUTF8(Req, "user_agent"), rt, true) <= 0) {
		PrintMessage(L"ERROR: " + UTF8_to_wchar(rt), URGB(255, 255, 150));
		DeleteFileW(cn.c_str());
		THREAD_ACCESS_UNLOCK(DEFAULT_GUARD_NAME, &template_ff);
		return ret;
	}
	DeleteFileW(cn.c_str());
	THREAD_ACCESS_UNLOCK(DEFAULT_GUARD_NAME, &template_ff);
	try {
		auto v = json::parse(rt);
		VKAPI_ANSWER P = (Akkount["audio.save"] << VK("audio", v.at("audio").get<string>()) << VK("hash", v.at("hash").get<string>()) << VKI("server", v.at("server").get<int64_t>()) << VK("artist", l.get_artist()) << VK("title", l.get_title()))();
		if (P.IsError) {
			PrintMessage(L"ERROR: " + UTF8_to_wchar(Answer.Object), URGB(255, 255, 150));
			return ret;
		}
		PrintMessage(L"Успешно загружен: " + pth, URGB(0, 255, 0));
	}
	catch (json::exception e) {
		PrintMessage(L"ERROR: " + UTF8_to_wchar(e.what()), URGB(255, 255, 150));
		return ret;
	}
	return ret;
}

static bool GetMedia(RequestParserStruct& Req, const Connection& client) {
	if (!exist_get_post(Req, "hash")) {
		return false;
	}
	std::string hash = get_postUTF8(Req, "hash");
	THREAD_ACCESS_LOCK(DEFAULT_GUARD_NAME, &mDiscography, &mAudios, &mVideos, &mPhotos);
	Media *md = look_MediaByHash(hash);
	if (!md) {
		THREAD_ACCESS_RETURN(DEFAULT_GUARD_NAME, false, &mDiscography, &mAudios, &mVideos, &mPhotos)
	}
	Media media = *md;
	THREAD_ACCESS_UNLOCK(DEFAULT_GUARD_NAME, &mDiscography, &mAudios, &mVideos, &mPhotos);

	FILE* fl;
	if (_wfopen_s(&fl, media.get_path().c_str(), L"rb") != 0) {
		return false;
	}
	if (!fl) {
		return false;
	}
	_fseeki64(fl, 0, SEEK_END);
	long long sz = _ftelli64(fl);
	_fseeki64(fl, 0, SEEK_SET);
	if (sz <= 0) {
		fclose(fl);
		return false;
	}
	long long from, to;
	if (hasRange(client, media, Req, from, to, sz)) {
		ostringstream HttpAnswer;
		long long off = (to - from) + 1;
		HttpAnswer << "HTTP/1.1 " << "206 Partial Content" << ENDL
			<< "Server: " << SERVER_NAME << ENDL
			<< "Date: " << GetTimeGMT(0) << ENDL
			<< "Content-Type: " << media.getMimeType() << ENDL
			<< "Content-Disposition: inline; filename=\"" << media.get_file_name() << "\"" << ENDL;
		if (sz > 0)
			HttpAnswer << "Content-Length: " << off << ENDL;
		HttpAnswer << "Connection: close" << ENDL;
		HttpAnswer << "Accept-Ranges: bytes" << ENDL;
		HttpAnswer << "Content-Range: bytes " << from << "-" << to << "/" << sz << ENDL;
		HttpAnswer << ENDL;
		if (!SSL_SendData(client, HttpAnswer.str(), SOCKTIMEOUT)) {
			fclose(fl);
#ifdef CRITICAL_DEBUG
			if (client.server_config.isDebug) {
				PrintMessage(L"DEBUG: Closed " + media.get_path(), URGB(255, 255, 150));
			}
#endif
			return false;
		}
		_fseeki64(fl, from, SEEK_SET);
		string data;
		data.resize(BUFFERSIZE);
		long long ccnt = off / BUFFERSIZE;
		long long ost = off - (ccnt * BUFFERSIZE);
		for (long long i = 0; i < ccnt; i++) {
			fread(data.data(), 1, BUFFERSIZE, fl);
			if (!SSL_SendData(client, data, SOCKTIMEOUT)) {
				fclose(fl);
#ifdef CRITICAL_DEBUG
				if (client.server_config.isDebug) {
					PrintMessage(L"DEBUG: Closed " + media.get_path(), URGB(255, 255, 150));
				}
#endif
				return false;
			}
		}
		if (ost > 0) {
			data.resize((size_t)ost);
			fread(data.data(), 1, (size_t)ost, fl);
			if (!SSL_SendData(client, data, SOCKTIMEOUT)) {
				fclose(fl);
				return false;
			}
		}
		fclose(fl);
#ifdef CRITICAL_DEBUG
		if (client.server_config.isDebug) {
			PrintMessage(L"DEBUG: Closed FULL " + media.get_path(), URGB(0, 255, 150));
		}
#endif
	}
	else {
#ifdef CRITICAL_DEBUG
		if (client.server_config.isDebug) {
			PrintMessage(L"DEBUG: FULL Content " + media.get_path(), URGB(0, 255, 150));
			PrintMessage(L"         " + wprintBytesCount(media.get_file_size()) + L", Connection: " + UTF8_to_wchar(Req.connection), URGB(0, 255, 150));
		}
#endif

		ostringstream HttpAnswer;
		HttpAnswer << "HTTP/1.1 " << "200 OK" << ENDL
			<< "Server: " << SERVER_NAME << ENDL
			<< "Date: " << GetTimeGMT(0) << ENDL
			<< "Content-Type: " << media.getMimeType() << ENDL
			<< "Content-Disposition: inline; filename=\"" << media.get_file_name() << "\"" << ENDL;
		if (media.get_type() == Media::TYPE::TYPE_PHOTO) {
			HttpAnswer << "Cache-Control: no-transform,public,max-age=345600,s-maxage=345600" << ENDL
				<< "X-Cache: HIT" << ENDL;
		}
		if (sz > 0)
			HttpAnswer << "Content-Length: " << sz << ENDL;
		HttpAnswer << "Connection: close" << ENDL;
		HttpAnswer << "Accept-Ranges: bytes" << ENDL;
		HttpAnswer << ENDL;
		if (!SSL_SendData(client, HttpAnswer.str(), SOCKTIMEOUT)) {
			fclose(fl);
#ifdef CRITICAL_DEBUG
			if (client.server_config.isDebug) {
				PrintMessage(L"DEBUG: Closed " + media.get_path(), URGB(255, 255, 150));
			}
#endif
			return false;
		}
		string data;
		data.resize(BUFFERSIZE);
		size_t n = 0;
		while ((n = fread(data.data(), 1, BUFFERSIZE, fl)) >= BUFFERSIZE) {
			if (!SSL_SendData(client, data, SOCKTIMEOUT)) {
				fclose(fl);
#ifdef CRITICAL_DEBUG
				if (client.server_config.isDebug) {
					PrintMessage(L"DEBUG: Closed " + media.get_path(), URGB(255, 255, 150));
				}
#endif
				return false;
			}
		}
		if (n > 0) {
			data.resize(n);
			fread(data.data(), 1, n, fl);
			if (!SSL_SendData(client, data, SOCKTIMEOUT)) {
				fclose(fl);
				return false;
			}
		}
		fclose(fl);
#ifdef CRITICAL_DEBUG
		if (client.server_config.isDebug) {
			PrintMessage(L"DEBUG: Closed FULL " + media.get_path(), URGB(0, 255, 150));
		}
#endif
	}
	return true;
}

static bool GetCover(RequestParserStruct& Req, const Connection& client) {
	if (!exist_get_post(Req, "hash")) {
		return false;
	}
	string hash = get_postUTF8(Req, "hash");
	std::wstring cover = get_cover(hash);
	if (!PathFileExistsW(cover.c_str())) {
		return false;
	}

	FILE* fl;
	if (_wfopen_s(&fl, cover.c_str(), L"rb") != 0) {
		return false;
	}
	if (!fl) {
		return false;
	}
	_fseeki64(fl, 0, SEEK_END);
	long long sz = _ftelli64(fl);
	_fseeki64(fl, 0, SEEK_SET);
	if (sz <= 0) {
		fclose(fl);
		return false;
	}
	ostringstream HttpAnswer;
	HttpAnswer << "HTTP/1.1 " << "200 OK" << ENDL
		<< "Server: " << SERVER_NAME << ENDL
		<< "Date: " << GetTimeGMT(0) << ENDL
		<< "Content-Type: " << "image/jpeg" << ENDL
		<< "Cache-Control: no-transform,public,max-age=345600,s-maxage=345600" << ENDL
		<< "X-Cache: HIT" << ENDL
		<< "Content-Disposition: inline; filename=\"" << hash + "_cover.jpg" << "\"" << ENDL;
	if (sz > 0)
		HttpAnswer << "Content-Length: " << sz << ENDL;
	HttpAnswer << "Connection: close" << ENDL;
	HttpAnswer << ENDL;
	if (!SSL_SendData(client, HttpAnswer.str(), SOCKTIMEOUT)) {
		fclose(fl);
		return false;
	}
	string data;
	data.resize(BUFFERSIZE);
	size_t n = 0;
	while ((n = fread(data.data(), 1, BUFFERSIZE, fl)) >= BUFFERSIZE) {
		if (!SSL_SendData(client, data, SOCKTIMEOUT)) {
			fclose(fl);
			return false;
		}
	}
	if (n > 0) {
		data.resize(n);
		fread(data.data(), 1, n, fl);
		if (!SSL_SendData(client, data, SOCKTIMEOUT)) {
			fclose(fl);
			return false;
		}
	}
	fclose(fl);
	return true;
}

static void UnixTimeToFileTime(const time_t& t, LPFILETIME pft) {
	LONGLONG ll;
	ll = Int32x32To64(t, 10000000) + 116444736000000000;
	pft->dwLowDateTime = (DWORD)ll;
	pft->dwHighDateTime = ll >> 32;
}

static bool AcceptSSL(SSL* ssl, bool server = true) {
	int nErrCode = 0;
	do {
		int error = server ? SSL_accept(ssl) : SSL_connect(ssl);
		if (error == 1) {
			return true;
		}
		nErrCode = SSL_get_error(ssl, error);
	} while (nErrCode == SSL_ERROR_WANT_READ || nErrCode == SSL_ERROR_WANT_WRITE);
	return false;
}

size_t multipart::parse(const string& body, size_t offset, const std::string& boundary) {
	size_t to = body.find("\r\n\r\n", offset);
	if (to == string::npos)
		return to;

	size_t end = body.find(boundary, to + 4);
	if (end == string::npos)
		return end;

	string hdr = body.substr(offset, to - offset);
	std::regex e1("Content-Disposition: form-data; name=\"([^\\r\\n\\t\\f\\v]*)\"", std::regex_constants::ECMAScript);
	std::regex e2("Content-Disposition: form-data; name=\"([^\\r\\n\\t\\f\\v]*)\"; filename=\"([^\\r\\n\\t\\f\\v]*)\"", std::regex_constants::ECMAScript);
	std::smatch matches1;
	std::smatch matches2;
	int status = 0;
	if (std::regex_search(hdr, matches2, e2))
		status = 2;
	else if (std::regex_search(hdr, matches1, e1))
		status = 1;
	if (status == 0) {
		return string::npos;
	}

	multipart_data dt;
	to += 4;
	dt.data.resize(end - to);
	memcpy((char*)dt.data.data(), &body.data()[to], end - to);

	string name;
	if (status == 1) {
		name = matches1[1];
	}

	if (status == 2) {
		//name = SHA3_512::sha3(dt.data);
		name = matches2[1];
		dt.filename = matches2[2];
	}
	paramsdata[name] = dt;

	return end;
}

static std::string decompress_zstd(const std::string& from) {
	std::unique_ptr<ZSTD_DCtx, decltype(&ZSTD_freeDCtx)> ctx{ ZSTD_createDCtx(),
															 &ZSTD_freeDCtx };
	std::string frame_buffer(ZSTD_DStreamOutSize(), 0);
	std::string decompressed;

	ZSTD_inBuffer in_ref = { .src = from.data(), .size = from.size(), .pos = 0 };
	std::size_t decompression_result = 0;
	while (in_ref.pos != in_ref.size) {
		ZSTD_outBuffer out_ref = {
			.dst = frame_buffer.data(), .size = frame_buffer.size(), .pos = 0 };
		decompression_result = ZSTD_decompressStream(ctx.get(), &out_ref, &in_ref);
		if (ZSTD_isError(decompression_result)) {
			throw(std::runtime_error("ZSTD_decompressStream failed while decompressing."));
		}
		decompressed.append(frame_buffer.data(), out_ref.pos);
	}
	while (decompression_result) {
		ZSTD_outBuffer out_ref = {
			.dst = frame_buffer.data(), .size = frame_buffer.size(), .pos = 0 };
		decompression_result = ZSTD_decompressStream(ctx.get(), &out_ref, &in_ref);
		if (ZSTD_isError(decompression_result)) {
			throw(std::runtime_error("ZSTD_decompressStream failed while decompressing."));
		}
		decompressed.append(frame_buffer.data(), out_ref.pos);
	}
	return decompressed;
}

static int parse_post_url_encoded_multipart(const Connection& client, RequestParserStruct &Req, int content_length, multipart& payload) {
	string contentType = Req.http_request.get_utf8("Content-Type");
	std::regex e("multipart/form-data; boundary=([^\\r\\n\\t\\f\\v]*)", std::regex_constants::ECMAScript);
	std::smatch matches;
	if (std::regex_search(contentType, matches, e)) {
		string boundary = matches[1];
		boundary = "--" + boundary;

		string subdata;
		subdata.resize(content_length);
		if (!SSL_RecvData(client, subdata.data(), content_length, SOCKTIMEOUT)) {
			return 3;
		}

		if (Req.http_request.exist("Content-Encoding") && Req.http_request["Content-Encoding"] == L"zstd") {
			subdata = decompress_zstd(subdata);
			Req.http_request.add("Content-Length", to_string(subdata.size()));
			Req.bodySize = subdata.size();
		}

		bool isFirst = true;
		size_t offset = 0;

		while ((offset = subdata.find(boundary, offset)) != string::npos) {
			offset += boundary.length();
			if (isFirst) {
				isFirst = false;
				boundary = "\r\n" + boundary;
			}
			if (subdata[offset] == '-' && subdata[offset + 1] == '-')
				break;
			offset = payload.parse(subdata, offset + 2, boundary);
		}
		if (offset == string::npos || payload.count() <= 0) {
			return 3;
		}
		return 2;
	}
	return 0;
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

static bool ReciveHTTPRequest(const Connection& client, RequestParserStruct &Req) {
	string HttpRequest;
	string http_post_url_encoded;
	multipart mpart;

	size_t nCurrentPos = 0;
	while (HttpRequest.find("\r\n\r\n") == string::npos) {
		char Tmp = 0;
		if (HttpRequest.size() > POST_BODY_MAX || !SSL_RecvData(client, &Tmp, 1, SOCKTIMEOUT)) {
			return false;
		}
		HttpRequest.push_back(Tmp);
	}
	if (!Req.HTTPParse(HttpRequest, client.connection)) {
		return false;
	}

	if (Req.method != HTTP_METHOD::HTTP_METHOD_GET
		&& Req.method != HTTP_METHOD::HTTP_METHOD_POST) {
		return false;
	}
	if (Req.method == HTTP_METHOD::HTTP_METHOD_POST) {
		if (Req.bodySize > 0) {
			if (Req.bodySize > POST_BODY_MULTIPART_MAX) {
				return false;
			}
			int status = parse_post_url_encoded_multipart(client, Req, (int)Req.bodySize, mpart);
			if (status == 0) {
				if (Req.bodySize > POST_BODY_MAX) {
					return false;
				}
				http_post_url_encoded.resize(Req.bodySize);
				if (!SSL_RecvData(client, http_post_url_encoded.data(), (int)Req.bodySize, SOCKTIMEOUT)) {
					return false;
				}
				Req.ParsePostUrlEncoded(http_post_url_encoded);
			}
			else if (status == 3) {
				return false;
			}
			Req.multi_part = mpart;
		}
	}
	return true;
}

THREAD_IMPL(HTTPS_ServerThread, arg) {
	Connection client = extractThreadParamAndDelete<Connection>(arg);

	RequestParserStruct Req;

	bool NeedRedirect = false;
	bool needFullClose = false;
	if (client.isSSL()) {
		if (!AcceptSSL(client.ssl, true)) {
			needFullClose = true;
			goto out;
		}
	}

	if (!ReciveHTTPRequest(client, Req)) {
		needFullClose = true;
		goto out;
	}

	needFullClose = !serverMethods.Execute(Req.http_request.get_utf8("Path"), Req, &client);
	goto out;
out:
	client_shutdown(client, needFullClose ? SHUT_RDWR : SHUT_WR);
	return NULL_P;
}

void ServerMethods::RegisterMethod(const string& UTF8Path, void* Func, bool NeedHost, bool NeedAuth) {
	if (Methods.exist(UTF8Path))
		throw runtime_error("Method Exist");

	Methods[UTF8Path] = ServerMethodInfo(Func, NeedHost, NeedAuth);
}

bool ServerMethods::Execute(const string& UTF8Path, RequestParserStruct& Req, void* pClient) const {
	Connection* client = (Connection*)pClient;
	if (!Methods.exist(UTF8Path)) {
		return false;
	}
	ServerMethodInfo info = Methods.at(UTF8Path);
	if (info.NeedHost && !Req.http_request.exist("Host")) {
		if (client->server_config.isDebug) {
			PrintMessage(L"DEBUG: метод " + UTF8_to_wchar(UTF8Path) + L" требует http заголовок Host", URGB(255, 0, 0));
		}
		return SendErrorJSON(*client, Req, L"DEBUG: метод " + UTF8_to_wchar(UTF8Path) + L" требует http заголовок Host");
	}
	if (info.NeedAuth && exist_get_post(Req, "password")) {
		string password = get_postUTF8(Req, "password");
		if (PasswordWrongCount >= 2 || client->server_config.password != password) {
			wstring error = L"Метод " + UTF8_to_wchar(UTF8Path) + L" неверный пароль, IP " + WSTRUtils::UTF8_to_wchar(Req.connection);
			if (PasswordWrongCount >= 2) {
				FILE* fll = nullptr;
				if (_wfopen_s(&fll, (ExtractAppPath() + L"\\password-errors").c_str(), L"wb") == 0) {
					if (fll) {
						string herror = WSTRUtils::wchar_to_UTF8(error);
						fwrite(UTF8START, 1, strlen(UTF8START), fll);
						fwrite(herror.c_str(), 1, herror.size(), fll);
						fclose(fll);
					}
				}
				_exit(0);
				return false;
			}
			else {
				PasswordWrongCount++;
			}
			PrintMessage(error, URGB(255, 0, 0));
			return SendErrorJSON(*client, Req, L"Доступ запрещён!");
		}
	}
	else if (info.NeedAuth) {
		wstring error = L"Метод " + UTF8_to_wchar(UTF8Path) + L" требует авторизацию, IP " + WSTRUtils::UTF8_to_wchar(Req.connection);
		if (PasswordWrongCount >= 2) {
			FILE* fll = nullptr;
			if (_wfopen_s(&fll, (ExtractAppPath() + L"\\password-errors").c_str(), L"wb") == 0) {
				if (fll) {
					string herror = WSTRUtils::wchar_to_UTF8(error);
					fwrite(UTF8START, 1, strlen(UTF8START), fll);
					fwrite(herror.c_str(), 1, herror.size(), fll);
					fclose(fll);
				}
			}
			_exit(0);
			return false;
		}
		else {
			PasswordWrongCount++;
		}
		PrintMessage(error, URGB(255, 0, 0));
		return SendErrorJSON(*client, Req, L"Доступ запрещён!");
	}
	return ((method_t)Methods.at(UTF8Path).Func)(Req, *client);
}

static std::time_t to_time_t(const ULARGE_INTEGER& ull) {
	// Преобразовать в time_t
	std::time_t ret = (ull.QuadPart / 10000000ULL - 11644473600ULL);
	// Правильное оуркгление
	ULONGLONG QuadPartMod = ull.QuadPart % 10000000ULL;
	if (QuadPartMod >= 5000000ULL) {
		ret++;
	}
	//
	return ret;
}
static std::time_t to_time_t(const FILETIME& ft) {
	std::time_t ret = 0;
	// Преобразовать в ULARGE_INTEGER
	ULARGE_INTEGER ull;
	ull.LowPart = ft.dwLowDateTime;
	ull.HighPart = ft.dwHighDateTime;
	// Преобразовать в time_t
	return to_time_t(ull);
}
template <typename TP> std::time_t to_time_t(TP tp) {
	// Преобразовать в ULARGE_INTEGER
	ULARGE_INTEGER ull;
	ull.QuadPart = tp.time_since_epoch().count();
	// Преобразовать в time_t
	return to_time_t(ull);
}

static void DeleteThumbs() {
	WIN32_FIND_DATAW data;
	HANDLE hFind = FindFirstFileW((wstring(CACHE_DIR) + L"\\*.*").c_str(), &data);

	if (hFind != INVALID_HANDLE_VALUE) {
		do {
			if (~data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				DeleteFileW(((wstring)CACHE_DIR + L"\\" + data.cFileName).c_str());
			}
		} while (FindNextFileW(hFind, &data));
		FindClose(hFind);
	}
}

enum class TYPE_SCAN {
	TYPE_SCAN_AUDIO,
	TYPE_SCAN_DISCOGRAPHY,
	TYPE_SCAN_VIDEO,
	TYPE_SCAN_PHOTO
};

static bool isBadExt(const wstring& str, TYPE_SCAN type) {
	vector<wstring> skip = { L"jpg", L"jpeg", L"png", L"gif", L"txt", L"pdf", L"doc", L"docx", L"ppt", L"pptx", L"ini", L"xml", L"html", L"log", L"cue", L"zip", L"tar", L"rar", L"7z", L"gz", L"lzma" };
	if (type == TYPE_SCAN::TYPE_SCAN_PHOTO) {
		skip = { L"gif", L"txt", L"pdf", L"doc", L"docx", L"ppt", L"pptx", L"ini", L"xml", L"html", L"log", L"cue", L"zip", L"tar", L"rar", L"7z", L"gz", L"lzma" };
	}
	for (auto& i : skip) {
		size_t ll = str.find_last_of(L'.');
		if (ll != wstring::npos && WSTRUtils::Wcompare(str.substr(ll + 1), i)) {
			return true;
		}
	}
	return false;
}

static void cleanLoaded(TYPE_SCAN type) {
	switch (type)
	{
	case TYPE_SCAN::TYPE_SCAN_AUDIO:
		mAudios().clear();
		break;
	case TYPE_SCAN::TYPE_SCAN_DISCOGRAPHY:
		mDiscography.clear();
		break;
	case TYPE_SCAN::TYPE_SCAN_VIDEO:
		mVideos.clear();
		break;
	case TYPE_SCAN::TYPE_SCAN_PHOTO:
		mPhotos.clear();
		break;
	}
}

static inline long long getFindDataSize(const WIN32_FIND_DATAW& dt) {
	LARGE_INTEGER sz;
	sz.LowPart = dt.nFileSizeLow;
	sz.HighPart = dt.nFileSizeHigh;

	return sz.QuadPart;
}

static void scanFiles(const wstring &root, const wstring& offset, const wstring& dir_entry, bool scanFS, TYPE_SCAN type, bool is_main = true) {
	WIN32_FIND_DATAW data;
	HANDLE hFind = FindFirstFileW(combine_root_path(root, offset, L"*.*").c_str(), &data);

	if (hFind != INVALID_HANDLE_VALUE) {
		do {
			if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				wstring test = data.cFileName;
				if (test == L"." || test == L"..") {
					continue;
				}
				if (scanFS) {
					auto dirPtr = RootDir.find_by_path(combine_root_path(dir_entry, offset, test));
					if (!dirPtr) {
						dirPtr = Inode(test).folder().attachTo(*RootDir.find_by_path(combine_path(dir_entry, offset)), root);
					}
					scanFiles(root, combine_path(offset, test), dir_entry, scanFS, type, false);
					dirPtr->updateDir();
				}
				else {
					scanFiles(root, combine_path(offset, test), dir_entry, scanFS, type, false);
				}
			}
			else {
				if (isBadExt(data.cFileName, type)) {
					continue;
				}
				Audio a;
				Video v;
				Photo p;
				switch (type)
				{
				case TYPE_SCAN::TYPE_SCAN_AUDIO:
					a = Audio(combine_path(root, offset), data.cFileName, to_time_t(data.ftLastWriteTime), to_time_t(data.ftCreationTime), getFindDataSize(data));
					mAudios[a.get_hash()] = a;
					if (scanFS) {
						auto dirPtr = RootDir.find_by_path(combine_path(dir_entry, offset));
						Inode(data.cFileName).audio(getFindDataSize(data), to_time_t(data.ftLastWriteTime)).attachTo(*dirPtr, root);
					}
					break;
				case TYPE_SCAN::TYPE_SCAN_DISCOGRAPHY:
					a = Audio(combine_path(root, offset), data.cFileName, to_time_t(data.ftLastWriteTime), to_time_t(data.ftCreationTime), getFindDataSize(data));
					mDiscography[a.get_hash()] = a;
					if (scanFS) {
						auto dirPtr = RootDir.find_by_path(combine_path(dir_entry, offset));
						Inode(data.cFileName).audio(getFindDataSize(data), to_time_t(data.ftLastWriteTime)).attachTo(*dirPtr, root);
					}
					break;
				case TYPE_SCAN::TYPE_SCAN_VIDEO:
					v = Video(combine_path(root, offset), data.cFileName, to_time_t(data.ftLastWriteTime), to_time_t(data.ftCreationTime), getFindDataSize(data));
					mVideos[v.get_hash()] = v;
					if (scanFS) {
						auto dirPtr = RootDir.find_by_path(combine_path(dir_entry, offset));
						Inode(data.cFileName).video(getFindDataSize(data), to_time_t(data.ftLastWriteTime)).attachTo(*dirPtr, root);
					}
					break;
				case TYPE_SCAN::TYPE_SCAN_PHOTO:
					wstring fl = data.cFileName;
					if (WSTRUtils::wsearch(fl, L".jpg") != wstring::npos || WSTRUtils::wsearch(fl, L".jpeg") != wstring::npos || WSTRUtils::wsearch(fl, L".png") != wstring::npos || WSTRUtils::wsearch(fl, L".tiff") != wstring::npos || WSTRUtils::wsearch(fl, L".webp") != wstring::npos) {
						p = Photo(combine_path(root, offset), data.cFileName, to_time_t(data.ftLastWriteTime), to_time_t(data.ftCreationTime), getFindDataSize(data));
						mPhotos[p.get_hash()] = p;
						if (scanFS) {
							auto dirPtr = RootDir.find_by_path(combine_path(dir_entry, offset));
							Inode(data.cFileName).photo(getFindDataSize(data), to_time_t(data.ftLastWriteTime)).attachTo(*dirPtr, root);
						}
					}
					else {
						v = Video(combine_path(root, offset), data.cFileName, to_time_t(data.ftLastWriteTime), to_time_t(data.ftCreationTime), getFindDataSize(data));
						mVideos[v.get_hash()] = v;
						if (scanFS) {
							auto dirPtr = RootDir.find_by_path(combine_path(dir_entry, offset));
							Inode(data.cFileName).video(getFindDataSize(data), to_time_t(data.ftLastWriteTime)).attachTo(*dirPtr, root);
						}
					}
					break;
				}
			}
		} while (FindNextFileW(hFind, &data));
		FindClose(hFind);
	}
	if (is_main) {
		switch (type)
		{
		case TYPE_SCAN::TYPE_SCAN_AUDIO:
			PrintMessage(to_wstring(mAudios.size()) + L" Аудио.");
			break;
		case TYPE_SCAN::TYPE_SCAN_DISCOGRAPHY:
			PrintMessage(to_wstring(mDiscography.size()) + L" Дискографий.");
			break;
		case TYPE_SCAN::TYPE_SCAN_VIDEO:
			PrintMessage(to_wstring(mVideos.size()) + L" Видео.");
			break;
		case TYPE_SCAN::TYPE_SCAN_PHOTO:
			PrintMessage(to_wstring(mPhotos.size()) + L" Фото.");
			break;
		}
	}
}

template<typename T>
static bool greator(const Map::Pair<string, T>& lhs, const Map::Pair<string, T>& rhs) {
	return lhs.get_key() > rhs.get_key();
}

static void sortFiles() {
	mAudios.sort(greator<Audio>);
	mDiscography.sort(greator<Audio>);
	mVideos.sort(greator<Video>);
	mPhotos.sort(greator<Photo>);
	RootDir.updateDir();
}

static void doAfterFFMpegEnded(Map::Map<Media, PROCESS_INFORMATION> &handles) {
	for (auto& s : handles) {
		WaitForSingleObject(s.get_value().hProcess, INFINITE);
		CloseHandle(s.get_value().hProcess);
		CloseHandle(s.get_value().hThread);
		if (!PathFileExistsW((wstring(CACHE_DIR) + L"\\" + UTF8_to_wchar(s.get_key().get_hash()) + L".jpg").c_str()))
		{
			FlBin o;
			if (o.wopen(wstring(CACHE_DIR) + L"\\" + UTF8_to_wchar(s.get_key().get_hash()) + L".err", OPENType::OPENWrite)) {
				o << UTF8START << s.get_key().get_path();
				o.Close();
			}
		}
	}
	handles.clear();
}

void genVideoThumbs(bool is_OnlyNews) {
	if (!PathFileExistsW(CACHE_DIR)) {
		if (!CreateDirectoryW(CACHE_DIR, NULL)) {
			return;
		}
	}
	cleanLoaded(TYPE_SCAN::TYPE_SCAN_VIDEO);
	cleanLoaded(TYPE_SCAN::TYPE_SCAN_PHOTO);
	for (auto& i : Photo_Video_Dirs) {
		scanFiles(i, L"", L"", false, TYPE_SCAN::TYPE_SCAN_PHOTO);
	}
	for (auto& i : Video_Dirs) {
		scanFiles(i, L"", L"", false, TYPE_SCAN::TYPE_SCAN_VIDEO);
	}
	Map::Map<std::string, Video> tVideos = mVideos;
	wstring crdir;
	crdir.resize(MAX_PATH + 1);
	GetCurrentDirectoryW(MAX_PATH, (LPWSTR)crdir.c_str());
	PrintMessage(L"Сканирование обложек у видео : " + to_wstring(tVideos.size()));
	size_t ccn = 0;
	Map::Map<Media, PROCESS_INFORMATION> handles;
	for (auto& i : tVideos) {
		if (is_OnlyNews && (PathFileExistsW((wstring(CACHE_DIR) + L"\\" + UTF8_to_wchar(i.get_value().get_hash()) + L".jpg").c_str()) || PathFileExistsW((wstring(CACHE_DIR) + L"\\" + UTF8_to_wchar(i.get_value().get_hash()) + L".err").c_str()))) {
			ccn++;
			continue;
		}
		PROCESS_INFORMATION piProcInfo;
		STARTUPINFOW siStartInfo;
		ZeroMemory(&piProcInfo, sizeof(PROCESS_INFORMATION));
		ZeroMemory(&siStartInfo, sizeof(STARTUPINFOW));
		siStartInfo.cb = sizeof(STARTUPINFOW);
		unsigned long long flsz = i.get_value().get_file_size();
		wstring timestamp = L"00:00:00";
		if (flsz >= 20 * 1024 * 1024 && flsz < 50 * 1024 * 1024) {
			timestamp = L"00:00:04";
		}
		else if (flsz >= 50 * 1024 * 1024 && flsz < 70 * 1024 * 1024) {
			timestamp = L"00:00:15";
		}
		else if (flsz >= 70 * 1024 * 1024 && flsz < 100 * 1024 * 1024) {
			timestamp = L"00:00:25";
		}
		else if (flsz >= 100 * 1024 * 1024 && flsz < 150 * 1024 * 1024) {
			timestamp = L"00:00:45";
		}
		else if (flsz >= 150 * 1024 * 1024 && flsz < 200 * 1024 * 1024) {
			timestamp = L"00:01:00";
		}
		else if (flsz >= 200 * 1024 * 1024 && flsz < 250 * 1024 * 1024) {
			timestamp = L"00:01:30";
		}
		else if (flsz >= 250 * 1024 * 1024) {
			timestamp = L"00:01:45";
		}
		wstring cmd = L"-loglevel panic -n -ss " + timestamp + L" -i \"" + i.get_value().get_path() + L"\" -vf scale=\"\'if(gt(a,1/1),960,-1)\':\'if(gt(a,1/1),-1,960)\'\" -frames 1 " + CACHE_DIR + L"\\" + UTF8_to_wchar(i.get_value().get_hash()) + L".jpg";
		if (Startinit.isDebug) {
			PrintMessage(L"(" + to_wstring(++ccn) + L" из " + to_wstring(tVideos.size()) + L") " + cmd);
		}
		else {
			PrintMessage(L"(" + to_wstring(++ccn) + L" из " + to_wstring(tVideos.size()) + L") " + i.get_value().get_path());
		}
		if (!CreateProcessSimple(ExtractAppPath() + L"\\ffmpeg.exe", cmd,
			&siStartInfo,
			&piProcInfo,
			crdir,
			CREATE_NO_WINDOW))
		{
			continue;
		}
		handles[i.get_value()] = piProcInfo;
		if (handles.size() % Startinit.ffmpeg_proc_count == 0) {
			doAfterFFMpegEnded(handles);
			for (int i = 0; i < Startinit.ffmpeg_proc_count; i++) {
				ClearLine();
			}
		}
	}
	auto enr = handles.size();
	if (handles.size() > 0)
		doAfterFFMpegEnded(handles);
	for (size_t i = 0; i < enr; i++) {
		ClearLine();
	}
}

void genPhotoThumbs(bool is_OnlyNews) {
	if (!PathFileExistsW(CACHE_DIR)) {
		if (!CreateDirectoryW(CACHE_DIR, NULL)) {
			return;
		}
	}
	cleanLoaded(TYPE_SCAN::TYPE_SCAN_VIDEO);
	cleanLoaded(TYPE_SCAN::TYPE_SCAN_PHOTO);
	for (auto& i : Photo_Video_Dirs) {
		scanFiles(i, L"", L"", false, TYPE_SCAN::TYPE_SCAN_PHOTO);
	}
	for (auto& i : Video_Dirs) {
		scanFiles(i, L"", L"", false, TYPE_SCAN::TYPE_SCAN_VIDEO);
	}
	Map::Map<std::string, Photo> tPhotos = mPhotos;
	wstring crdir;
	crdir.resize(MAX_PATH + 1);
	GetCurrentDirectoryW(MAX_PATH, (LPWSTR)crdir.c_str());
	PrintMessage(L"Сканирование обложек у фото : " + to_wstring(tPhotos.size()));
	size_t ccn = 0;
	Map::Map<Media, PROCESS_INFORMATION> handles;
	for (auto& i : tPhotos) {
		if (is_OnlyNews && (PathFileExistsW((wstring(CACHE_DIR) + L"\\" + UTF8_to_wchar(i.get_value().get_hash()) + L".jpg").c_str()) || PathFileExistsW((wstring(CACHE_DIR) + L"\\" + UTF8_to_wchar(i.get_value().get_hash()) + L".err").c_str()))) {
			ccn++;
			continue;
		}
		PROCESS_INFORMATION piProcInfo;
		STARTUPINFOW siStartInfo;
		ZeroMemory(&piProcInfo, sizeof(PROCESS_INFORMATION));
		ZeroMemory(&siStartInfo, sizeof(STARTUPINFOW));
		siStartInfo.cb = sizeof(STARTUPINFOW);
		wstring cmd = L"-loglevel panic -n -i \"" + i.get_value().get_path() + L"\" -vf scale=\"\'if(gt(a,1/1),512,-1)\':\'if(gt(a,1/1),-1,512)\'\" " + CACHE_DIR + L"\\" + UTF8_to_wchar(i.get_value().get_hash()) + L".jpg";
		if (Startinit.isDebug) {
			PrintMessage(L"(" + to_wstring(++ccn) + L" из " + to_wstring(tPhotos.size()) + L") " + cmd);
		}
		else {
			PrintMessage(L"(" + to_wstring(++ccn) + L" из " + to_wstring(tPhotos.size()) + L") " + i.get_value().get_path());
		}
		if (!CreateProcessSimple(ExtractAppPath() + L"\\ffmpeg.exe", cmd,
			&siStartInfo,
			&piProcInfo,
			crdir,
			CREATE_NO_WINDOW))
		{
			continue;
		}
		handles[i.get_value()] = piProcInfo;
		if (handles.size() % Startinit.ffmpeg_proc_count == 0) {
			doAfterFFMpegEnded(handles);
			for (int i = 0; i < Startinit.ffmpeg_proc_count; i++) {
				ClearLine();
			}
		}
	}
	auto enr = handles.size();
	if (handles.size() > 0)
		doAfterFFMpegEnded(handles);
	for (size_t i = 0; i < enr; i++) {
		ClearLine();
	}
}

void genAudioThumbs(bool is_OnlyNews) {
	if (!PathFileExistsW(CACHE_DIR)) {
		if (!CreateDirectoryW(CACHE_DIR, NULL)) {
			return;
		}
	}
	cleanLoaded(TYPE_SCAN::TYPE_SCAN_AUDIO);
	for (auto& i : Audio_Dirs) {
		scanFiles(i, L"", L"", false, TYPE_SCAN::TYPE_SCAN_AUDIO);
	}
	Map::Map<std::string, Audio> tAudios = mAudios;
	wstring crdir;
	crdir.resize(MAX_PATH + 1);
	GetCurrentDirectoryW(MAX_PATH, (LPWSTR)crdir.c_str());
	PrintMessage(L"Сканирование обложек у аудио : " + to_wstring(tAudios.size()));
	size_t ccn = 0;
	size_t thr = 0;
	Map::Map<Media, PROCESS_INFORMATION> handles;
	for (auto& i : tAudios) {
		if (is_OnlyNews && (PathFileExistsW((wstring(CACHE_DIR) + L"\\" + UTF8_to_wchar(i.get_value().get_hash()) + L".jpg").c_str()) || PathFileExistsW((wstring(CACHE_DIR) + L"\\" + UTF8_to_wchar(i.get_value().get_hash()) + L".err").c_str()))) {
			ccn++;
			continue;
		}
		PROCESS_INFORMATION piProcInfo;
		STARTUPINFOW siStartInfo;
		ZeroMemory(&piProcInfo, sizeof(PROCESS_INFORMATION));
		ZeroMemory(&siStartInfo, sizeof(STARTUPINFOW));
		siStartInfo.cb = sizeof(STARTUPINFOW);
		wstring cmd = L"-loglevel panic -n -i \"" + i.get_value().get_path() + L"\" " + CACHE_DIR + L"\\" + UTF8_to_wchar(i.get_value().get_hash()) + L".jpg";
		if (Startinit.isDebug) {
			PrintMessage(L"(" + to_wstring(++ccn) + L" из " + to_wstring(tAudios.size()) + L") " + cmd);
		}
		else {
			PrintMessage(L"(" + to_wstring(++ccn) + L" из " + to_wstring(tAudios.size()) + L") " + i.get_value().get_path());
		}
		if (!CreateProcessSimple(ExtractAppPath() + L"\\ffmpeg.exe", cmd,
			&siStartInfo,
			&piProcInfo,
			crdir,
			CREATE_NO_WINDOW))
		{
			continue;
		}
		handles[i.get_value()] = piProcInfo;
		if (handles.size() % Startinit.ffmpeg_proc_count == 0) {
			doAfterFFMpegEnded(handles);
			for (int i = 0; i < Startinit.ffmpeg_proc_count; i++) {
				ClearLine();
			}
		}
	}
	auto enr = handles.size();
	if (handles.size() > 0)
		doAfterFFMpegEnded(handles);

	for (size_t i = 0; i < enr; i++) {
		ClearLine();
	}
}

THREAD_IMPL(doScanCovers, arg) {
	dlgS.StartBT.EnableWindow(FALSE);
	dlgS.ScanCovers.EnableWindow(FALSE);
	dlgS.MediaFolders.EnableWindow(FALSE);
	dlgS.PhotosThumb.EnableWindow(FALSE);
	dlgS.ServerPassword.ShowWindow(SW_HIDE);
	bool is_OnlyNews = dlgS.OnlyNews.GetCheck();
	if (!is_OnlyNews) {
		DeleteThumbs();
	}
	genAudioThumbs(is_OnlyNews);
	//genDiscographyThumbs(is_OnlyNews);
	genVideoThumbs(is_OnlyNews);
	if (dlgS.PhotosThumb.GetCheck() == TRUE) {
		genPhotoThumbs(is_OnlyNews);
	}
	dlgS.StartBT.EnableWindow(TRUE);
	dlgS.ScanCovers.EnableWindow(TRUE);
	dlgS.MediaFolders.EnableWindow(TRUE);
	dlgS.PhotosThumb.EnableWindow(TRUE);
	dlgS.Edk.ClearSpecialPatternOnce();
	if (dlgS.StartAfterScan.GetCheck()) {
		dlgS.OnStart();
	}
	dlgS.ServerPassword.ShowWindow(SW_SHOW);
	return 0;
}

static bool UpdateTime(RequestParserStruct& Req, const Connection& client) {
	if (!exist_get_post(Req, "hash")) {
		return SendErrorJSON(client, Req, L"Не передан hash");
	}
	string hash = get_postUTF8(Req, "hash");
	THREAD_ACCESS_LOCK(DEFAULT_GUARD_NAME, &mDiscography, &mAudios, &mVideos, &mPhotos, &RootDir);
	Media* md = look_MediaByHash(hash);
	if (!md) {
		THREAD_ACCESS_UNLOCK(DEFAULT_GUARD_NAME, &mDiscography, &mAudios, &mVideos, &mPhotos, &RootDir);
		return SendErrorJSON(client, Req, L"Hash не найден");
	}
	Media media = *md;
	THREAD_ACCESS_UNLOCK(DEFAULT_GUARD_NAME, &mDiscography, &mAudios, &mVideos, &mPhotos, &RootDir);

	bool needAnswer = true;
	bool retConf = false;
	if (!Startinit.canEdit) {
		retConf = SendPendingSingleResponseJSON(client, Req, 1);
		needAnswer = false;
		wstring pth = media.get_path();
		if ((win_message().message_type(MSG_TYPE::TYPE_QUESTION).button_type(MSG_BUTTON::BUTTON_YESNO) << L"Обновить дату модификации " << pth << L"?").show() != IDYES) {
			return retConf;
		}
	}

	HANDLE file_handle =
		::CreateFileW(media.get_path().c_str(), FILE_WRITE_ATTRIBUTES,
			NULL, NULL, OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL, 0);
	if (file_handle != NULL) {
		time_t tm = time(0);
		FILETIME FlTime;
		UnixTimeToFileTime(tm, &FlTime);
		::SetFileTime(file_handle, &FlTime, &FlTime, &FlTime);
		CloseHandle(file_handle);

		std::wstring cover = get_cover(media.get_hash());
		if (PathFileExistsW(cover.c_str())) {
			MoveFileW(cover.c_str(), get_cover(media.update_hash(tm, tm)).c_str());
		}
	}
	THREAD_ACCESS_LOCK(DEFAULT_GUARD_NAME, &mDiscography, &mAudios, &mVideos, &mPhotos, &RootDir);
	PrintMessage(L"Пересканирование контента!", URGB(255, 200, 0));

	cleanLoaded(TYPE_SCAN::TYPE_SCAN_AUDIO);
	cleanLoaded(TYPE_SCAN::TYPE_SCAN_DISCOGRAPHY);
	cleanLoaded(TYPE_SCAN::TYPE_SCAN_VIDEO);
	cleanLoaded(TYPE_SCAN::TYPE_SCAN_PHOTO);
	RootDir.clear();
	for (auto& i : Audio_Dirs) {
		auto pp = Inode::make_subRoot(i, RootDir);
		scanFiles(i, L"", Inode::getName(pp), true, TYPE_SCAN::TYPE_SCAN_AUDIO);
		Inode::updateDir(pp);
	}
	for (auto& i : Discography_Dirs) {
		auto pp = Inode::make_subRoot(i, RootDir);
		scanFiles(i, L"", Inode::getName(pp), true, TYPE_SCAN::TYPE_SCAN_DISCOGRAPHY);
		Inode::updateDir(pp);
	}
	for (auto& i : Photo_Video_Dirs) {
		auto pp = Inode::make_subRoot(i, RootDir);
		scanFiles(i, L"", Inode::getName(pp), true, TYPE_SCAN::TYPE_SCAN_PHOTO);
		Inode::updateDir(pp);
	}
	for (auto& i : Video_Dirs) {
		auto pp = Inode::make_subRoot(i, RootDir);
		scanFiles(i, L"", Inode::getName(pp), true, TYPE_SCAN::TYPE_SCAN_VIDEO);
		Inode::updateDir(pp);
	}
	PrintMessage(L" ");
	sortFiles();

	THREAD_ACCESS_UNLOCK(DEFAULT_GUARD_NAME, &mDiscography, &mAudios, &mVideos, &mPhotos, &RootDir);
	if (needAnswer) {
		return SendSingleResponseJSON(client, Req, 1);
	}
	else {
		return retConf;
	}
}

static bool UpdateFileName(RequestParserStruct& Req, const Connection& client) {
	if (!exist_get_post(Req, "hash") || !exist_get_post(Req, "name")) {
		return SendErrorJSON(client, Req, L"Не передан hash или name");
	}
	string hash = get_postUTF8(Req, "hash");
	wstring new_fl = UTF8_to_wchar(trim(get_postUTF8(Req, "name")));
	if (new_fl.empty()) {
		return SendErrorJSON(client, Req, L"name пустой");
	}
	new_fl = FixFileName(new_fl);
	if (PathFileExistsW(new_fl.c_str())) {
		return SendErrorJSON(client, Req, L"Файл существует с таким именем!");
	}
	THREAD_ACCESS_LOCK(DEFAULT_GUARD_NAME, &mDiscography, &mAudios, &mVideos, &mPhotos, &RootDir);
	Media* md = look_MediaByHash(hash);
	if (!md) {
		THREAD_ACCESS_UNLOCK(DEFAULT_GUARD_NAME, &mDiscography, &mAudios, &mVideos, &mPhotos, &RootDir);
		return SendErrorJSON(client, Req, L"Hash не найден");
	}
	Media media = *md;
	THREAD_ACCESS_UNLOCK(DEFAULT_GUARD_NAME, &mDiscography, &mAudios, &mVideos, &mPhotos, &RootDir);

	bool needAnswer = true;
	bool retConf = false;
	if (!Startinit.canEdit) {
		retConf = SendPendingSingleResponseJSON(client, Req, 1);
		needAnswer = false;
		wstring pth = media.get_path();
		if ((win_message().message_type(MSG_TYPE::TYPE_QUESTION).button_type(MSG_BUTTON::BUTTON_YESNO) << L"Переменововать " << pth << L" в" << combine_path(media.get_orig_dir(), new_fl) << L"?").show() != IDYES)
			return retConf;
	}

	if (!MoveFileW(media.get_path().c_str(), combine_path(media.get_orig_dir(), new_fl).c_str())) {
		if (needAnswer) {
			return SendErrorJSON(client, Req, L"Переименовать не удалось!");
		}
		else {
			PrintMessage(L"Переименовать не удалось!", URGB(255, 200, 0));
			return retConf;
		}
	}
	HANDLE file_handle =
		::CreateFileW(combine_path(media.get_orig_dir(), new_fl).c_str(), FILE_WRITE_ATTRIBUTES,
			NULL, NULL, OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL, 0);
	if (file_handle != NULL) {
		time_t tm = time(0);
		FILETIME FlTime;
		UnixTimeToFileTime(tm, &FlTime);
		::SetFileTime(file_handle, &FlTime, &FlTime, &FlTime);
		CloseHandle(file_handle);

		std::wstring cover = get_cover(media.get_hash());
		if (PathFileExistsW(cover.c_str())) {
			MoveFileW(cover.c_str(), get_cover(media.update_full_hash(media.get_orig_dir(), new_fl, tm, tm)).c_str());
		}
	}
	THREAD_ACCESS_LOCK(DEFAULT_GUARD_NAME, &mDiscography, &mAudios, &mVideos, &mPhotos, &RootDir);
	PrintMessage(L"Пересканирование контента!", URGB(255, 200, 0));

	cleanLoaded(TYPE_SCAN::TYPE_SCAN_AUDIO);
	cleanLoaded(TYPE_SCAN::TYPE_SCAN_DISCOGRAPHY);
	cleanLoaded(TYPE_SCAN::TYPE_SCAN_VIDEO);
	cleanLoaded(TYPE_SCAN::TYPE_SCAN_PHOTO);
	RootDir.clear();
	for (auto& i : Audio_Dirs) {
		auto pp = Inode::make_subRoot(i, RootDir);
		scanFiles(i, L"", Inode::getName(pp), true, TYPE_SCAN::TYPE_SCAN_AUDIO);
		Inode::updateDir(pp);
	}
	for (auto& i : Discography_Dirs) {
		auto pp = Inode::make_subRoot(i, RootDir);
		scanFiles(i, L"", Inode::getName(pp), true, TYPE_SCAN::TYPE_SCAN_DISCOGRAPHY);
		Inode::updateDir(pp);
	}
	for (auto& i : Photo_Video_Dirs) {
		auto pp = Inode::make_subRoot(i, RootDir);
		scanFiles(i, L"", Inode::getName(pp), true, TYPE_SCAN::TYPE_SCAN_PHOTO);
		Inode::updateDir(pp);
	}
	for (auto& i : Video_Dirs) {
		auto pp = Inode::make_subRoot(i, RootDir);
		scanFiles(i, L"", Inode::getName(pp), true, TYPE_SCAN::TYPE_SCAN_VIDEO);
		Inode::updateDir(pp);
	}
	PrintMessage(L" ");
	sortFiles();

	THREAD_ACCESS_UNLOCK(DEFAULT_GUARD_NAME, &mDiscography, &mAudios, &mVideos, &mPhotos, &RootDir);

	if (needAnswer) {
		return SendSingleResponseJSON(client, Req, 1);
	}
	else {
		return retConf;
	}
}

static bool DeleteFileSrv(RequestParserStruct& Req, const Connection& client) {
	if (!exist_get_post(Req, "hash")) {
		return SendErrorJSON(client, Req, L"Не передан hash");
	}
	string hash = get_postUTF8(Req, "hash");
	THREAD_ACCESS_LOCK(DEFAULT_GUARD_NAME, &mDiscography, &mAudios, &mVideos, &mPhotos, &RootDir);
	Media* md = look_MediaByHash(hash);
	if (!md) {
		THREAD_ACCESS_UNLOCK(DEFAULT_GUARD_NAME, &mDiscography, &mAudios, &mVideos, &mPhotos, &RootDir);
		return SendErrorJSON(client, Req, L"Hash не найден");
	}
	Media media = *md;
	std::wstring cover = get_cover(media.get_hash());

	bool needAnswer = true;
	bool retConf = false;
	if (!Startinit.canEdit) {
		retConf = SendPendingSingleResponseJSON(client, Req, 1);
		needAnswer = false;
		wstring pth = media.get_path();
		THREAD_ACCESS_UNLOCK(DEFAULT_GUARD_NAME, &mDiscography, &mAudios, &mVideos, &mPhotos, &RootDir);
		if ((win_message().message_type(MSG_TYPE::TYPE_QUESTION).button_type(MSG_BUTTON::BUTTON_YESNO) << L"Удалить файл " << pth << L"?").show() != IDYES)
			return retConf;
		THREAD_ACCESS_LOCK(DEFAULT_GUARD_NAME, &mDiscography, &mAudios, &mVideos, &mPhotos, &RootDir);
	}

	if (!DeleteFileW(media.get_path().c_str())) {
		THREAD_ACCESS_UNLOCK(DEFAULT_GUARD_NAME, &mDiscography, &mAudios, &mVideos, &mPhotos, &RootDir);
		if (needAnswer) {
			return SendErrorJSON(client, Req, L"Файл не удалён");
		}
		else {
			PrintMessage(L"Файл не удалён", URGB(255, 200, 0));
			return retConf;
		}
	}
	if (PathFileExistsW(cover.c_str())) {
		DeleteFileW(cover.c_str());
	}

	PrintMessage(L"Пересканирование контента!", URGB(255, 200, 0));

	cleanLoaded(TYPE_SCAN::TYPE_SCAN_AUDIO);
	cleanLoaded(TYPE_SCAN::TYPE_SCAN_DISCOGRAPHY);
	cleanLoaded(TYPE_SCAN::TYPE_SCAN_VIDEO);
	cleanLoaded(TYPE_SCAN::TYPE_SCAN_PHOTO);
	RootDir.clear();
	for (auto& i : Audio_Dirs) {
		auto pp = Inode::make_subRoot(i, RootDir);
		scanFiles(i, L"", Inode::getName(pp), true, TYPE_SCAN::TYPE_SCAN_AUDIO);
		Inode::updateDir(pp);
	}
	for (auto& i : Discography_Dirs) {
		auto pp = Inode::make_subRoot(i, RootDir);
		scanFiles(i, L"", Inode::getName(pp), true, TYPE_SCAN::TYPE_SCAN_DISCOGRAPHY);
		Inode::updateDir(pp);
	}
	for (auto& i : Photo_Video_Dirs) {
		auto pp = Inode::make_subRoot(i, RootDir);
		scanFiles(i, L"", Inode::getName(pp), true, TYPE_SCAN::TYPE_SCAN_PHOTO);
		Inode::updateDir(pp);
	}
	for (auto& i : Video_Dirs) {
		auto pp = Inode::make_subRoot(i, RootDir);
		scanFiles(i, L"", Inode::getName(pp), true, TYPE_SCAN::TYPE_SCAN_VIDEO);
		Inode::updateDir(pp);
	}
	PrintMessage(L" ");
	sortFiles();

	THREAD_ACCESS_UNLOCK(DEFAULT_GUARD_NAME, &mDiscography, &mAudios, &mVideos, &mPhotos, &RootDir);

	if (needAnswer) {
		return SendSingleResponseJSON(client, Req, 1);
	}
	else {
		return retConf;
	}
}

static bool GetFileName(RequestParserStruct& Req, const Connection& client) {
	if (!exist_get_post(Req, "hash")) {
		return SendErrorJSON(client, Req, L"Не передан hash");
	}
	string hash = get_postUTF8(Req, "hash");
	THREAD_ACCESS_LOCK(DEFAULT_GUARD_NAME, &mDiscography, &mAudios, &mVideos, &mPhotos);
	Media* md = look_MediaByHash(hash);
	if (!md) {
		THREAD_ACCESS_UNLOCK(DEFAULT_GUARD_NAME, &mDiscography, &mAudios, &mVideos, &mPhotos);
		return SendErrorJSON(client, Req, L"Hash не найден");
	}
	Media media = *md;
	THREAD_ACCESS_UNLOCK(DEFAULT_GUARD_NAME, &mDiscography, &mAudios, &mVideos, &mPhotos);

	return SendSingleResponseJSON(client, Req, WSTRUtils::wchar_to_UTF8(media.get_orig_name()));
}

void stopAudioPlay() {
	THREAD_ACCESS_LOCK(DEFAULT_GUARD_NAME, &player);
	player.Stop();
	player.ClearMemorySound();
	THREAD_ACCESS_UNLOCK(DEFAULT_GUARD_NAME, &player);
}

void InitMediaServer() {
	if (Startinit.ffmpeg_proc_count <= 0) {
		Startinit.ffmpeg_proc_count = 1;
	}
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		(win_message().timeout(5).message_type(MSG_TYPE::TYPE_ERROR) << L"Ошибка инициализации сокетов").show();
		return;
	}

	if (!serverMethods.GetInited()) {
		serverMethods.RegisterMethod("method/audio.dumplist", AudioList, true, true);

		serverMethods.RegisterMethod("method/audio.remoteplay", AudioRemotePlay, true, true);

		serverMethods.RegisterMethod("method/audio.get", AudioGet, true, true);
		serverMethods.RegisterMethod("method/video.get", VideoGet, true, true);
		serverMethods.RegisterMethod("method/audio.search", AudioSearch, true, true);
		serverMethods.RegisterMethod("method/video.search", VideoSearch, true, true);
		serverMethods.RegisterMethod("method/fs.get", FSGet, true, true);
		serverMethods.RegisterMethod("method/rebootPC", rebootPC, false, true);
		serverMethods.RegisterMethod("method/discography.get", DiscographyGet, true, true);
		serverMethods.RegisterMethod("method/discography.search", DiscographySearch, true, true);
		serverMethods.RegisterMethod("method/photos.get", PhotosGet, true, true);
		serverMethods.RegisterMethod("method/photos.search", PhotosSearch, true, true);

		serverMethods.RegisterMethod("method/delete_media", DeleteFileSrv, false, true);
		serverMethods.RegisterMethod("method/update_time", UpdateTime, false, true);
		serverMethods.RegisterMethod("method/update_file_name", UpdateFileName, false, true);
		serverMethods.RegisterMethod("method/get_file_name", GetFileName, false, true);

		serverMethods.RegisterMethod("method/get_media", GetMedia);
		serverMethods.RegisterMethod("method/get_cover", GetCover);
		if (PathFileExistsW((ExtractAppPath() + L"\\" + L"ffmpeg.exe").c_str())) {
			serverMethods.RegisterMethod("method/audio.upload", AudioUpload, true, true);
		}
		serverMethods.SetInited();
	}

	THREAD_ACCESS_REGISTER_POINTERS(DEFAULT_GUARD_NAME, &mDiscography, &mAudios, &mVideos, &mPhotos, &player, &RootDir, &template_ff);

	cleanLoaded(TYPE_SCAN::TYPE_SCAN_AUDIO);
	cleanLoaded(TYPE_SCAN::TYPE_SCAN_DISCOGRAPHY);
	cleanLoaded(TYPE_SCAN::TYPE_SCAN_VIDEO);
	cleanLoaded(TYPE_SCAN::TYPE_SCAN_PHOTO);
	RootDir.clear();
	for (auto& i : Audio_Dirs) {
		auto pp = Inode::make_subRoot(i, RootDir);
		scanFiles(i, L"", Inode::getName(pp), true, TYPE_SCAN::TYPE_SCAN_AUDIO);
		Inode::updateDir(pp);
	}
	for (auto& i : Discography_Dirs) {
		auto pp = Inode::make_subRoot(i, RootDir);
		scanFiles(i, L"", Inode::getName(pp), true, TYPE_SCAN::TYPE_SCAN_DISCOGRAPHY);
		Inode::updateDir(pp);
	}
	for (auto& i : Photo_Video_Dirs) {
		auto pp = Inode::make_subRoot(i, RootDir);
		scanFiles(i, L"", Inode::getName(pp), true, TYPE_SCAN::TYPE_SCAN_PHOTO);
		Inode::updateDir(pp);
	}
	for (auto& i : Video_Dirs) {
		auto pp = Inode::make_subRoot(i, RootDir);
		scanFiles(i, L"", Inode::getName(pp), true, TYPE_SCAN::TYPE_SCAN_VIDEO);
		Inode::updateDir(pp);
	}
	PrintMessage(L" ");
	sortFiles();
	
	SSL_CTX* ctx = nullptr;
	if (Startinit.isSsl) {
		ctx = initialize_ctx(true, true);
		if (!ctx) {
			return;
		}
	}
	HTTPSserver_sock = 0;
	SOCKET client_sock = 0;
	HTTPSserver_sock = tcp_listen(Startinit.isSsl ? Startinit.HttpsPort : Startinit.HttpPort);
	if (HTTPSserver_sock == SOCKET_ERROR) {
		(win_message().timeout(5).message_type(MSG_TYPE::TYPE_ERROR) << L"Сбой в инициализации сервера(socket bind)").show();
		return;
	}
	if (Startinit.isSsl) {
		PrintMessage(L"[HTTPS инициализирован, порт " + to_wstring(Startinit.HttpsPort) + L"]", URGB(0, 140, 255));
	}
	else {
		PrintMessage(L"[HTTP инициализирован, порт " + to_wstring(Startinit.HttpPort) + L"]", URGB(0, 140, 255));
	}
	sockaddr_in FromAddr;
	int len = sizeof(FromAddr);
	while (true) {
		client_sock = accept(HTTPSserver_sock, (struct sockaddr*)&FromAddr, &len);
		if (client_sock <= 0)
			continue;
		DWORD sockFlag = 1;
		setsockopt(client_sock, IPPROTO_TCP, TCP_NODELAY, (char*)&sockFlag, sizeof(sockFlag));
		SetTimeOutsSocket(client_sock);

		SSL* ssl = nullptr;

		if (Startinit.isSsl) {
			ssl = SSL_new(ctx);
			if (!ssl) {
				shutdown(client_sock, SHUT_RDWR);
				closesocket(client_sock);
				continue;
			}
			if (SSL_set_fd(ssl, (int)client_sock) != 1) {
				SSL_free(ssl);
				shutdown(client_sock, SHUT_RDWR);
				closesocket(client_sock);
				continue;
			}
		}

		Connection* packet = new Connection(client_sock, ssl, GetConnectionAddr(FromAddr), Startinit);
		if (!CreateThreadDetachedSimple(&HTTPS_ServerThread, (void*)packet)) {
			if (ssl) {
				SSL_free(ssl);
			}
			shutdown(client_sock, SHUT_RDWR);
			closesocket(client_sock);
			delete packet;
			continue;
		}
	}
	shutdown(HTTPSserver_sock, SHUT_RDWR);
	closesocket(HTTPSserver_sock);
	if (ctx) {
		SSL_CTX_free(ctx);
	}
}
