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
#include "ThreadAccessGuard.h"
#include "../../Third-Party/USound/src/USound.h"
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

static Map::Map<std::string, Audio> mDiscography;
static Map::Map<std::string, Audio> mAudios;
static Map::Map<std::string, Video> mVideos;
static Map::Map<std::string, Photo> mPhotos;

list<wstring> Discography_Dirs;
list<wstring> Audio_Dirs;
list<wstring> Video_Dirs;
list<wstring> Photo_Video_Dirs;

static Inode RootDir;

EXTERN_THREAD_ACCESS_GUARD_OBJECT

InitServer Startinit;

class CLIENT_CONNECTION
{
public:
	CLIENT_CONNECTION() {
		isSSL = false;
		ssl = NULL;
		client_sock = NULL;
	}
	bool isSSL;
	SSL* ssl;
	SOCKET client_sock;
	InitServer server_config;
};

typedef void(*method_t)(RequestParserStruct& Req, CLIENT_CONNECTION* client);

bool IsMobile(TARGET_DEVICES Device)
{
	if (Device == TARGET_DEVICES::DEVICE_ANDROID || Device == TARGET_DEVICES::DEVICE_IPHONE || Device == TARGET_DEVICES::DEVICE_IPAD || Device == TARGET_DEVICES::DEVICE_WINDOWS_PHONE)
		return true;
	return false;
}

void wstringparam::add(const string& name, const wstring& value)
{
	paramsdata[name] = value;
}
void wstringparam::add_decode(const string& name, const string& value)
{
	paramsdata[name] = UTF8_to_wchar(url_decode(value));
}
void wstringparam::add(const string& name, const string& value)
{
	paramsdata[name] = UTF8_to_wchar(value);
}
void wstringparam::parse(const string& paramstr, const string& separator, bool need_url_decode)
{
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
void wstringparam::parse(const wstring& paramstring, const string& separator)
{
	parse(wchar_to_UTF8(paramstring), separator, false);
}
int wstringparam::count() const
{
	return (int)paramsdata.size();
}
wstring wstringparam::operator[](const string& name) const
{
	if (paramsdata.exist(name) == true)
		return paramsdata.at(name);
	return L"";
}
bool wstringparam::exist(const string& name)
{
	if (paramsdata.exist(name) == true)
		return true;
	return false;
}
void wstringparam::erase(const string& name)
{
	if (paramsdata.exist(name) == true)
		paramsdata.erase(name);
}
string wstringparam::get_utf8(const string& name) const
{
	if (paramsdata.exist(name) == true)
		return wchar_to_UTF8(paramsdata.at(name));
	return "";
}
void wstringparam::set_utf8(const string& name, const string& value)
{
	if (paramsdata.exist(name) == true)
		paramsdata[name] = UTF8_to_wchar(value);
}

static wstring GetDublicates(int count, const wstring &str)
{
	wstring out = L"";
	for (int i = 0; i < count; i++)
		out += str;
	return out;
}

UTF8Stream& UTF8Stream::operator<<(const wstring &String)
{
	if (String.length() > 0)
	{
		string OT = wchar_to_UTF8(String);
		Data += OT;
	}
	return *this;
}
UTF8Stream& UTF8Stream::operator<<(const string &String)
{
	if (String.length() > 0)
		Data += String;
	return *this;
}
UTF8Stream& UTF8Stream::operator<<(size_t value)
{
	Data += to_string(value);
	return *this;
}
UTF8Stream& UTF8Stream::operator<<(int value)
{
	Data += to_string(value);
	return *this;
}
const string &UTF8Stream::GetString() const
{
	return Data;
}
int UTF8Stream::GetSize()
{
	return (int)Data.length();
}
void UTF8Stream::Clear()
{
	Data.clear();
}

SOCKET tcp_listen(int Port)
{
	SOCKET sock;
	struct sockaddr_in sin;
	const int qlen = 1;
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		return SOCKET_ERROR;

	sin.sin_addr.s_addr = INADDR_ANY;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(Port);
	if (::bind(sock, (struct sockaddr*) & sin, sizeof(sin)) < 0)
		return SOCKET_ERROR;
	if (listen(sock, qlen) == SOCKET_ERROR)
		return SOCKET_ERROR;
	return sock;
}

void SetTimeOutsSocket(SOCKET Socket)
{
	DWORD timeout = SOCKTIMEOUT * 1000;
	setsockopt(Socket, SOL_SOCKET, SO_SNDTIMEO, (char*)&timeout, sizeof(timeout));
	setsockopt(Socket, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));
}

void InitOpenSSL()
{
	/*
	SSL_library_init();
	OpenSSL_add_all_algorithms();
	ERR_load_BIO_strings();
	ERR_load_crypto_strings();
	SSL_load_error_strings();
	*/
}

static time_t ASN1_GetTimeT(ASN1_TIME* time) {
	struct tm t;
	const char* str = (const char*)time->data;
	size_t i = 0;

	memset(&t, 0, sizeof(t));

	if (time->type == V_ASN1_UTCTIME) {
		t.tm_year = (str[i++] - '0') * 10;
		t.tm_year += (str[i++] - '0');
		if (t.tm_year < 70)
			t.tm_year += 100;
	}
	else if (time->type == V_ASN1_GENERALIZEDTIME) {
		t.tm_year = (str[i++] - '0') * 1000;
		t.tm_year += (str[i++] - '0') * 100;
		t.tm_year += (str[i++] - '0') * 10;
		t.tm_year += (str[i++] - '0');
		t.tm_year -= 1900;
	}
	t.tm_mon = (str[i++] - '0') * 10;
	t.tm_mon += (str[i++] - '0') - 1;
	t.tm_mday = (str[i++] - '0') * 10;
	t.tm_mday += (str[i++] - '0');
	t.tm_hour = (str[i++] - '0') * 10;
	t.tm_hour += (str[i++] - '0');
	t.tm_min = (str[i++] - '0') * 10;
	t.tm_min += (str[i++] - '0');
	t.tm_sec = (str[i++] - '0') * 10;
	t.tm_sec += (str[i++] - '0');

	return mktime(&t);
}

inline void PrintSertValid(X509* Sert, const wstring& Name)
{
	PrintMessage(L"Сертификат " + Name + L" Действителен с " + GetTimeAT(ASN1_GetTimeT(X509_get_notBefore(Sert))) + L" по " + GetTimeAT(ASN1_GetTimeT(X509_get_notAfter(Sert))), URGB(140, 140, 140));
}

SSL_CTX* initialize_ctx(bool print_sert)
{
	auto CA_CERT = std::shared_ptr<BIO>(BIO_new_mem_buf(CertificateData.CertPart[USSL_CERTPART_CERT].data(), (int)CertificateData.CertPart[USSL_CERTPART_CERT].size()), BIO_free);
	auto CA_KEY = std::shared_ptr<BIO>(BIO_new_mem_buf(CertificateData.CertPart[USSL_CERTPART_KEY].data(), (int)CertificateData.CertPart[USSL_CERTPART_KEY].size()), BIO_free);
	SSL_CTX* ctx = SSL_CTX_new(TLS_server_method());
	BIO* CA_BUNDLE = NULL;
	X509* bundle = NULL;
	X509* cert = NULL;
	EVP_PKEY* rsa = NULL;
	X509* p = NULL;
	bool first = true;
	int bundle_part = 0;

	if (ctx == NULL)
		goto error;

	
	while ((p = PEM_read_bio_X509(CA_CERT.get(), NULL, 0, NULL)) != NULL) {
		if (first) {
			if (SSL_CTX_use_certificate(ctx, p) <= 0)
				goto error;
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
	rsa = PEM_read_bio_PrivateKey(CA_KEY.get(), NULL, 0, NULL);

	if (first || rsa == NULL)
		goto error;
	if (CertificateData.CertPart.exist(USSL_CERTPART_BUNDLE) && !CertificateData.CertPart[USSL_CERTPART_BUNDLE].empty()) {
		CA_BUNDLE = BIO_new_mem_buf(CertificateData.CertPart[USSL_CERTPART_BUNDLE].data(), (int)CertificateData.CertPart[USSL_CERTPART_BUNDLE].size());
		if (!CA_BUNDLE) {
			goto error;
		}
		while ((p = PEM_read_bio_X509(CA_BUNDLE, NULL, 0, NULL)) != NULL) {
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
	if (SSL_CTX_use_PrivateKey(ctx, rsa) <= 0)
		goto error;
	if (print_sert) {
		PrintMessage(L"    #Домен " + UTF8_to_wchar(CertificateData.Name), URGB(180, 0, 0));
	}
	return ctx;
error:
	(win_message().timeout(5).message_type(MSG_TYPE::TYPE_ERROR) << L"OpenSSL Key Error : Невозможно создать Server CTX").show();
	return NULL;
}

bool ReciveNetBinary(SOCKET Socket, const void* Buffer, int Len, int Timeout)
{
	memset((void*)Buffer, 0, Len);
	DWORD timeout = Timeout * 1000;
	setsockopt(Socket, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));
	int rsz = recv(Socket, (char*)Buffer, Len, 0);
	if (rsz != Len)
		return false;
	return true;
}

bool SendNetBinary(SOCKET Socket, const string& BinData, int Timeout)
{
	DWORD timeout = Timeout * 1000;
	setsockopt(Socket, SOL_SOCKET, SO_SNDTIMEO, (char*)&timeout, sizeof(timeout));
	int rsz = send(Socket, (char*)BinData.data(), (int)BinData.size(), 0);
	if (rsz != BinData.size())
		return false;
	return true;
}

bool SSL_SendData(const CLIENT_CONNECTION &client, const string& BinData)
{
	if (!client.isSSL) {
		return SendNetBinary(client.client_sock, BinData, SOCKTIMEOUT);
	}
	size_t nCurrentPos = 0;
	int err = 0;
	while (nCurrentPos < BinData.size())
	{
		err = SSL_write(client.ssl, &BinData[nCurrentPos], (int)BinData.size());
		if (err > 0)
		{
			nCurrentPos += err;
			continue;
		}
		const int nCode = SSL_get_error(client.ssl, err);
		if ((nCode != SSL_ERROR_WANT_READ) && (nCode != SSL_ERROR_WANT_WRITE))
			return false;
	}
	return true;
}

bool SSL_RecvData(const CLIENT_CONNECTION& client, const void* BinData, int Size)
{
	if (!client.isSSL) {
		return ReciveNetBinary(client.client_sock, BinData, Size, SOCKTIMEOUT);
	}
	size_t nCurrentPos = 0;
	int err = 0;
	while (nCurrentPos < (size_t)Size)
	{
		err = SSL_read(client.ssl, &((char*)BinData)[nCurrentPos], Size);
		if (err > 0)
		{
			nCurrentPos += err;
			continue;
		}
		const int nCode = SSL_get_error(client.ssl, err);
		if ((nCode != SSL_ERROR_WANT_READ) && (nCode != SSL_ERROR_WANT_WRITE))
			return false;
	}
	return true;
}

bool SSL_RecvDataString(const CLIENT_CONNECTION& client, string& BinData, int Size)
{
	BinData.resize((size_t)Size);
	return SSL_RecvData(client, BinData.data(), Size);
}

bool SendHTTTPAnswerWithData(const CLIENT_CONNECTION& client, const string &CodeAnsw, const string &ContentType, const string& Data)
{
	ostringstream HttpAnswer;
	HttpAnswer << "HTTP/1.1 " << CodeAnsw << ENDL
		<< "Server: " << SERVER_NAME << ENDL
		<< "Date: " << GetTimeGMT(0) << ENDL
		<< "Content-Type: " << ContentType << ENDL;
	if (Data.size() > 0)
		HttpAnswer << "Content-Length: " << Data.size() << ENDL;
	HttpAnswer << ENDL;
	if (SSL_SendData(client, HttpAnswer.str()) == false || SSL_SendData(client, Data) == false)
		return false;
	return true;
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

static void SendErrorJSON(CLIENT_CONNECTION* client, const wstring& error) {
	string error_t = "{ \"error\": {\"error_code\": 3,\"error_msg\" : \"" + wchar_to_UTF8(error) + "\"} }";
	SendHTTTPAnswerWithData(*client, "200 OK", "application/json; charset=utf-8", error_t);
}

wstring Media::get_cover() const {
	return wstring(CACHE_DIR) + L"\\" + UTF8_to_wchar(get_hash()) + L".jpg";
}

static wstring get_cover(const std::string &hash) {
	return wstring(CACHE_DIR) + L"\\" + UTF8_to_wchar(hash) + L".jpg";
}

static string BytesToSize(long long Bytes) {
	long long tb = 1099511627776;
	long long gb = 1073741824;
	long long mb = 1048576;
	long long kb = 1024;

	char returnSize[512];

	if (Bytes >= tb)
		sprintf_s(returnSize, "%.2lf TB", (double)Bytes / tb);
	else if (Bytes >= gb && Bytes < tb)
		sprintf_s(returnSize, "%.2lf GB", (double)Bytes / gb);
	else if (Bytes >= mb && Bytes < gb)
		sprintf_s(returnSize, "%.2lf MB", (double)Bytes / mb);
	else if (Bytes >= kb && Bytes < mb)
		sprintf_s(returnSize, "%.2lf KB", (double)Bytes / kb);
	else if (Bytes < kb)
		sprintf_s(returnSize, "%.2llu Bytes", Bytes);
	else
		sprintf_s(returnSize, "%.2llu Bytes", Bytes);
	return returnSize;
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

static void AudioGet(RequestParserStruct& Req, CLIENT_CONNECTION* client)
{
	THREAD_ACCESS_LOCK(DEFAULT_GUARD_NAME, &mAudios);
	int count = from_get_post_int(Req, "count", 0);
	int offset = from_get_post_int(Req, "offset", 0);
	bool reverse = from_get_post_bool(Req, "reverse", false);
	if (count < 0 || count > mAudios.size()) {
		count = 0;
	}
	if (offset < 0) {
		offset = 0;
	}
	string payload = "{ \"response\": { \"count\": " + to_string(mAudios.size()) + ",\"items\": ";

	json arr = json(json::value_t::array);

	if (offset <= mAudios.size() - 1) {
		if (reverse) {
			auto it = mAudios().rbegin();
			for (size_t i = 0; i < offset; i++) {
				it++;
			}
			for (size_t i = 0; (it != mAudios().rend() && (count > 0 ? i < count : true)); i++, it++) {
				arr.push_back(makeAudio((*it).get_value(), Req, client->isSSL));
			}
		}
		else {
			auto it = mAudios().begin();
			for (size_t i = 0; i < offset; i++) {
				it++;
			}
			for (size_t i = 0; (it != mAudios().end() && (count > 0 ? i < count : true)); i++, it++) {
				arr.push_back(makeAudio((*it).get_value(), Req, client->isSSL));
			}
		}
	}
	payload += arr.dump();
	payload += "} }";
	THREAD_ACCESS_LOCK(DEFAULT_GUARD_NAME, &mAudios);
	SendHTTTPAnswerWithData(*client, "200 OK", "application/json; charset=utf-8", payload);
}

static void AudioList(RequestParserStruct& Req, CLIENT_CONNECTION* client)
{
	THREAD_ACCESS_LOCK(DEFAULT_GUARD_NAME, &mAudios);

	json arr = json(json::value_t::array);

	for (auto &i : mAudios) {
		arr.push_back(i.get_value().get_artist() + " - " + i.get_value().get_title() + "." + wchar_to_UTF8(Media::get_ext(i.get_value().get_orig_name())));
	}
	THREAD_ACCESS_LOCK(DEFAULT_GUARD_NAME, &mAudios);
	SendHTTTPAnswerWithData(*client, "200 OK", "application/json; charset=utf-8", arr.dump(4));
}

static void AudioRemotePlay(RequestParserStruct& Req, CLIENT_CONNECTION* client)
{
	THREAD_ACCESS_LOCK(DEFAULT_GUARD_NAME, &player);
	if (Req.multi_part.exist("audio")) {
		player.Stop();
		player.RegisterResourceMP3Sounds(Req.multi_part.get_data("audio").data);
		player.PlayMemorySound(false, false);
		dlgS.ToggleStopAudio(true);
		SendHTTTPAnswerWithData(*client, "200 OK", "application/json; charset=utf-8", "{ \"response\": 1 }");
	} else {
		player.Stop();
		dlgS.ToggleStopAudio(false);
		SendHTTTPAnswerWithData(*client, "200 OK", "application/json; charset=utf-8", "{ \"response\": 0 }");
	}
	THREAD_ACCESS_UNLOCK(DEFAULT_GUARD_NAME, &player);
}

static void DiscographyGet(RequestParserStruct& Req, CLIENT_CONNECTION* client)
{
	THREAD_ACCESS_LOCK(DEFAULT_GUARD_NAME, &mDiscography);
	int count = from_get_post_int(Req, "count", 0);
	int offset = from_get_post_int(Req, "offset", 0);
	bool reverse = from_get_post_bool(Req, "reverse", false);
	if (count < 0 || count > mDiscography.size()) {
		count = 0;
	}
	if (offset < 0) {
		offset = 0;
	}
	string payload = "{ \"response\": { \"count\": " + to_string(mDiscography.size()) + ",\"items\": ";

	json arr = json(json::value_t::array);

	if (offset <= mDiscography.size() - 1) {
		if (reverse) {
			auto it = mDiscography().rbegin();
			for (size_t i = 0; i < offset; i++) {
				it++;
			}
			for (size_t i = 0; (it != mDiscography().rend() && (count > 0 ? i < count : true)); i++, it++) {
				arr.push_back(makeAudio((*it).get_value(), Req, client->isSSL));
			}
		}
		else {
			auto it = mDiscography().begin();
			for (size_t i = 0; i < offset; i++) {
				it++;
			}
			for (size_t i = 0; (it != mDiscography().end() && (count > 0 ? i < count : true)); i++, it++) {
				arr.push_back(makeAudio((*it).get_value(), Req, client->isSSL));
			}
		}
	}
	payload += arr.dump();
	payload += "} }";
	THREAD_ACCESS_LOCK(DEFAULT_GUARD_NAME, &mDiscography);
	SendHTTTPAnswerWithData(*client, "200 OK", "application/json; charset=utf-8", payload);
}

static void PhotosGet(RequestParserStruct& Req, CLIENT_CONNECTION* client)
{
	THREAD_ACCESS_LOCK(DEFAULT_GUARD_NAME, &mPhotos);
	int count = from_get_post_int(Req, "count", 0);
	int offset = from_get_post_int(Req, "offset", 0);
	bool reverse = from_get_post_bool(Req, "reverse", false);
	if (count < 0 || count > mPhotos.size()) {
		count = 0;
	}
	if (offset < 0) {
		offset = 0;
	}
	string payload = "{ \"response\": { \"count\": " + to_string(mPhotos.size()) + ",\"items\": ";

	json arr = json(json::value_t::array);

	if (offset <= mPhotos.size() - 1) {
		if (reverse) {
			auto it = mPhotos().rbegin();
			for (size_t i = 0; i < offset; i++) {
				it++;
			}
			for (size_t i = 0; (it != mPhotos().rend() && (count > 0 ? i < count : true)); i++, it++) {
				arr.push_back(makePhoto((*it).get_value(), Req, client->isSSL));
			}
		}
		else {
			auto it = mPhotos().begin();
			for (size_t i = 0; i < offset; i++) {
				it++;
			}
			for (size_t i = 0; (it != mPhotos().end() && (count > 0 ? i < count : true)); i++, it++) {
				arr.push_back(makePhoto((*it).get_value(), Req, client->isSSL));
			}
		}
	}
	payload += arr.dump();
	payload += "} }";
	THREAD_ACCESS_LOCK(DEFAULT_GUARD_NAME, &mPhotos);
	SendHTTTPAnswerWithData(*client, "200 OK", "application/json; charset=utf-8", payload);
}

static void AudioSearch(RequestParserStruct& Req, CLIENT_CONNECTION* client)
{
	string q = trim(get_postUTF8(Req, "q"));
	if (q.length() <= 0) {
		AudioGet(Req, client);
		return;
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

	int count = from_get_post_int(Req, "count", 0);
	int offset = from_get_post_int(Req, "offset", 0);
	if (count < 0 || count > result.size()) {
		count = 0;
	}
	if (offset < 0) {
		offset = 0;
	}
	string payload = "{ \"response\": { \"count\": " + to_string(result.size()) + ",\"items\": ";

	json arr = json(json::value_t::array);

	if (offset <= result.size() - 1) {
		auto it = result.begin();
		for (size_t i = 0; i < offset; i++) {
			it++;
		}
		for (size_t i = 0; (it != result.end() && (count > 0 ? i < count : true)); i++, it++) {
			arr.push_back(makeAudio(*it, Req, client->isSSL));
		}
	}
	payload += arr.dump();
	payload += "} }";
	SendHTTTPAnswerWithData(*client, "200 OK", "application/json; charset=utf-8", payload);
}

static void DiscographySearch(RequestParserStruct& Req, CLIENT_CONNECTION* client)
{
	string q = trim(get_postUTF8(Req, "q"));
	if (q.length() <= 0) {
		DiscographyGet(Req, client);
		return;
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

	int count = from_get_post_int(Req, "count", 0);
	int offset = from_get_post_int(Req, "offset", 0);
	if (count < 0 || count > result.size()) {
		count = 0;
	}
	if (offset < 0) {
		offset = 0;
	}
	string payload = "{ \"response\": { \"count\": " + to_string(result.size()) + ",\"items\": ";

	json arr = json(json::value_t::array);

	if (offset <= result.size() - 1) {
		auto it = result.begin();
		for (size_t i = 0; i < offset; i++) {
			it++;
		}
		for (size_t i = 0; (it != result.end() && (count > 0 ? i < count : true)); i++, it++) {
			arr.push_back(makeAudio(*it, Req, client->isSSL));
		}
	}
	payload += arr.dump();
	payload += "} }";
	SendHTTTPAnswerWithData(*client, "200 OK", "application/json; charset=utf-8", payload);
}

static void PhotosSearch(RequestParserStruct& Req, CLIENT_CONNECTION* client)
{
	string q = trim(get_postUTF8(Req, "q"));
	if (q.length() <= 0) {
		PhotosGet(Req, client);
		return;
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

	int count = from_get_post_int(Req, "count", 0);
	int offset = from_get_post_int(Req, "offset", 0);
	if (count < 0 || count > result.size()) {
		count = 0;
	}
	if (offset < 0) {
		offset = 0;
	}
	string payload = "{ \"response\": { \"count\": " + to_string(result.size()) + ",\"items\": ";

	json arr = json(json::value_t::array);

	if (offset <= result.size() - 1) {
		auto it = result.begin();
		for (size_t i = 0; i < offset; i++) {
			it++;
		}
		for (size_t i = 0; (it != result.end() && (count > 0 ? i < count : true)); i++, it++) {
			arr.push_back(makePhoto(*it, Req, client->isSSL));
		}
	}
	payload += arr.dump();
	payload += "} }";
	SendHTTTPAnswerWithData(*client, "200 OK", "application/json; charset=utf-8", payload);
}

static void VideoGet(RequestParserStruct& Req, CLIENT_CONNECTION* client)
{
	THREAD_ACCESS_LOCK(DEFAULT_GUARD_NAME, &mVideos);
	int count = from_get_post_int(Req, "count", 0);
	int offset = from_get_post_int(Req, "offset", 0);
	bool reverse = from_get_post_bool(Req, "reverse", false);
	if (count < 0 || count > mVideos.size()) {
		count = 0;
	}
	if (offset < 0) {
		offset = 0;
	}
	string payload = "{ \"response\": { \"count\": " + to_string(mVideos.size()) + ",\"items\": ";

	json arr = json(json::value_t::array);

	if (offset <= mVideos.size() - 1) {
		if (reverse) {
			auto it = mVideos().rbegin();
			for (size_t i = 0; i < offset; i++) {
				it++;
			}
			for (size_t i = 0; (it != mVideos().rend() && (count > 0 ? i < count : true)); i++, it++) {
				arr.push_back(makeVideo((*it).get_value(), Req, client->isSSL));
			}
		}
		else {
			auto it = mVideos().begin();
			for (size_t i = 0; i < offset; i++) {
				it++;
			}
			for (size_t i = 0; (it != mVideos().end() && (count > 0 ? i < count : true)); i++, it++) {
				arr.push_back(makeVideo((*it).get_value(), Req, client->isSSL));
			}
		}
	}
	payload += arr.dump();
	payload += "} }";
	THREAD_ACCESS_UNLOCK(DEFAULT_GUARD_NAME, &mVideos);
	SendHTTTPAnswerWithData(*client, "200 OK", "application/json; charset=utf-8", payload);
}

std::string Inode::serialize(const RequestParserStruct& Req, bool isSSL) {
	json arr = json(json::value_t::array);
	string payload = "{ \"response\": { \"count\": " + to_string(entries.size()) + ",\"items\": ";
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
	payload += arr.dump();
	payload += "} }";
	return payload;
}

static void FSGet(RequestParserStruct& Req, CLIENT_CONNECTION* client)
{
	THREAD_ACCESS_LOCK(DEFAULT_GUARD_NAME, &RootDir);
	string dir = trim(get_postUTF8(Req, "dir"));
	auto p = RootDir.find_by_path(UTF8_to_wchar(dir));
	if (p == nullptr) {
		THREAD_ACCESS_UNLOCK(DEFAULT_GUARD_NAME, &RootDir);
		SendErrorJSON(client, L"Неверный путь");
		return;
	}
	string payload = p->serialize(Req, client->isSSL);
	THREAD_ACCESS_UNLOCK(DEFAULT_GUARD_NAME, &RootDir);
	SendHTTTPAnswerWithData(*client, "200 OK", "application/json; charset=utf-8", payload);
}

static void rebootPC(RequestParserStruct& Req, CLIENT_CONNECTION* client)
{
	string type = trim(get_postUTF8(Req, "type"));
	SendHTTTPAnswerWithData(*client, "200 OK", "application/json; charset=utf-8", "{ \"response\": 1 }");
	if (type == "win") {
		system("shutdown /r");
		dlgS.OnClose();
	}
	else if (type == "linux") {
		system("start /unix /sbin/reboot");
		dlgS.OnClose();
	}
}

static void VideoSearch(RequestParserStruct& Req, CLIENT_CONNECTION* client)
{
	string q = trim(get_postUTF8(Req, "q"));
	if (q.length() <= 0) {
		VideoGet(Req, client);
		return;
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
	int count = from_get_post_int(Req, "count", 0);
	int offset = from_get_post_int(Req, "offset", 0);
	if (count < 0 || count > result.size()) {
		count = 0;
	}
	if (offset < 0) {
		offset = 0;
	}
	string payload = "{ \"response\": { \"count\": " + to_string(result.size()) + ",\"items\": ";

	json arr = json(json::value_t::array);

	if (offset <= result.size() - 1) {
		auto it = result.begin();
		for (size_t i = 0; i < offset; i++) {
			it++;
		}
		for (size_t i = 0; (it != result.end() && (count > 0 ? i < count : true)); i++, it++) {
			arr.push_back(makeVideo(*it, Req, client->isSSL));
		}
	}
	payload += arr.dump();
	payload += "} }";
	SendHTTTPAnswerWithData(*client, "200 OK", "application/json; charset=utf-8", payload);
}

static bool hasRange(CLIENT_CONNECTION* client, Media media, RequestParserStruct& Req, long long& from, long long& to, long long size) {
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
	if (client->server_config.isDebug) {
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
	return NULL;
}

static void GetMedia(RequestParserStruct& Req, CLIENT_CONNECTION* client) {
	if (!exist_get_post(Req, "hash")) {
		return;
	}
	std::string hash = get_postUTF8(Req, "hash");
	THREAD_ACCESS_LOCK(DEFAULT_GUARD_NAME, &mDiscography, &mAudios, &mVideos, &mPhotos);
	Media *md = look_MediaByHash(hash);
	if (md == NULL) {
		THREAD_ACCESS_RETURN(DEFAULT_GUARD_NAME, , &mDiscography, &mAudios, &mVideos, &mPhotos)
	}
	Media media = *md;
	THREAD_ACCESS_UNLOCK(DEFAULT_GUARD_NAME, &mDiscography, &mAudios, &mVideos, &mPhotos);

	FILE* fl;
	if (_wfopen_s(&fl, media.get_path().c_str(), L"rb") != 0 || fl == NULL) {
		return;
	}
	_fseeki64(fl, 0, SEEK_END);
	long long sz = _ftelli64(fl);
	_fseeki64(fl, 0, SEEK_SET);
	if (sz <= 0) {
		fclose(fl);
		return;
	}
	long long from, to;
	if (hasRange(client, media, Req, from, to, sz)) {
		ostringstream HttpAnswer;
		long long off = (to - from) + 1;
		HttpAnswer << "HTTP/1.1 " << "206 Partial Content" << ENDL
			<< "Server: " << SERVER_NAME << ENDL
			<< "Date: " << GetTimeGMT(0) << ENDL
			<< "Content-Type: " << media.getMimeType() << ENDL
			<< "Content-Disposition: attachment; filename=\"" << media.get_file_name() << "\"" << ENDL;
		if (sz > 0)
			HttpAnswer << "Content-Length: " << off << ENDL;
		HttpAnswer << "Connection: keep-alive" << ENDL;
		HttpAnswer << "Accept-Ranges: bytes" << ENDL;
		HttpAnswer << "Content-Range: bytes " << from << "-" << to << "/" << sz << ENDL;
		HttpAnswer << ENDL;
		if (SSL_SendData(*client, HttpAnswer.str()) == false) {
			fclose(fl);
#ifdef CRITICAL_DEBUG
			if (client->server_config.isDebug) {
				PrintMessage(L"DEBUG: Closed " + media.get_path(), URGB(255, 255, 150));
			}
#endif
			return;
		}
		_fseeki64(fl, from, SEEK_SET);
		string data;
		data.resize(BUFFERSIZE);
		long long ccnt = off / BUFFERSIZE;
		long long ost = off - (ccnt * BUFFERSIZE);
		for (long long i = 0; i < ccnt; i++) {
			fread(data.data(), 1, BUFFERSIZE, fl);
			if (SSL_SendData(*client, data) == false) {
				fclose(fl);
#ifdef CRITICAL_DEBUG
				if (client->server_config.isDebug) {
					PrintMessage(L"DEBUG: Closed " + media.get_path(), URGB(255, 255, 150));
				}
#endif
				return;
			}
		}
		if (ost > 0) {
			data.resize(ost);
			fread(data.data(), 1, ost, fl);
			SSL_SendData(*client, data);
		}
		fclose(fl);
#ifdef CRITICAL_DEBUG
		if (client->server_config.isDebug) {
			PrintMessage(L"DEBUG: Closed FULL " + media.get_path(), URGB(0, 255, 150));
		}
#endif
	}
	else {
#ifdef CRITICAL_DEBUG
		if (client->server_config.isDebug) {
			PrintMessage(L"DEBUG: FULL Content " + media.get_path(), URGB(0, 255, 150));
			PrintMessage(L"         " + UTF8_to_wchar(BytesToSize(media.get_file_size())) + L", Connection: " + UTF8_to_wchar(Req.connection), URGB(0, 255, 150));
		}
#endif

		ostringstream HttpAnswer;
		HttpAnswer << "HTTP/1.1 " << "200 OK" << ENDL
			<< "Server: " << SERVER_NAME << ENDL
			<< "Date: " << GetTimeGMT(0) << ENDL
			<< "Content-Type: " << media.getMimeType() << ENDL
			<< "Content-Disposition: attachment; filename=\"" << media.get_file_name() << "\"" << ENDL;
		if (media.get_type() == Media::TYPE::TYPE_PHOTO) {
			HttpAnswer << "Cache-Control: no-transform,public,max-age=345600,s-maxage=345600" << ENDL
				<< "X-Cache: HIT" << ENDL;
		}
		if (sz > 0)
			HttpAnswer << "Content-Length: " << sz << ENDL;
		HttpAnswer << "Connection: keep-alive" << ENDL;
		HttpAnswer << "Accept-Ranges: bytes" << ENDL;
		HttpAnswer << ENDL;
		if (SSL_SendData(*client, HttpAnswer.str()) == false) {
			fclose(fl);
#ifdef CRITICAL_DEBUG
			if (client->server_config.isDebug) {
				PrintMessage(L"DEBUG: Closed " + media.get_path(), URGB(255, 255, 150));
			}
#endif
			return;
		}
		string data;
		data.resize(BUFFERSIZE);
		size_t n = 0;
		while ((n = fread(data.data(), 1, BUFFERSIZE, fl)) >= BUFFERSIZE) {
			if (SSL_SendData(*client, data) == false) {
				fclose(fl);
#ifdef CRITICAL_DEBUG
				if (client->server_config.isDebug) {
					PrintMessage(L"DEBUG: Closed " + media.get_path(), URGB(255, 255, 150));
				}
#endif
				return;
			}
		}
		if (n > 0) {
			data.resize(n);
			fread(data.data(), 1, n, fl);
			SSL_SendData(*client, data);
		}
		fclose(fl);
#ifdef CRITICAL_DEBUG
		if (client->server_config.isDebug) {
			PrintMessage(L"DEBUG: Closed FULL " + media.get_path(), URGB(0, 255, 150));
		}
#endif
	}
}

static void GetCover(RequestParserStruct& Req, CLIENT_CONNECTION* client) {
	if (!exist_get_post(Req, "hash")) {
		return;
	}
	string hash = get_postUTF8(Req, "hash");
	std::wstring cover = get_cover(hash);
	if (!PathFileExistsW(cover.c_str())) {
		return;
	}

	FILE* fl;
	if (_wfopen_s(&fl, cover.c_str(), L"rb") != 0 || fl == NULL) {
		return;
	}
	_fseeki64(fl, 0, SEEK_END);
	long long sz = _ftelli64(fl);
	_fseeki64(fl, 0, SEEK_SET);
	if (sz <= 0) {
		fclose(fl);
		return;
	}
	ostringstream HttpAnswer;
	HttpAnswer << "HTTP/1.1 " << "200 OK" << ENDL
		<< "Server: " << SERVER_NAME << ENDL
		<< "Date: " << GetTimeGMT(0) << ENDL
		<< "Content-Type: " << "image/jpeg" << ENDL
		<< "Cache-Control: no-transform,public,max-age=345600,s-maxage=345600" << ENDL
		<< "X-Cache: HIT" << ENDL
		<< "Content-Disposition: attachment; filename=\"" << hash + "_cover.jpg" << "\"" << ENDL;
	if (sz > 0)
		HttpAnswer << "Content-Length: " << sz << ENDL;
	HttpAnswer << ENDL;
	if (SSL_SendData(*client, HttpAnswer.str()) == false) {
		fclose(fl);
		return;
	}
	string data;
	data.resize(BUFFERSIZE);
	size_t n = 0;
	while ((n = fread(data.data(), 1, BUFFERSIZE, fl)) >= BUFFERSIZE) {
		if (SSL_SendData(*client, data) == false) {
			fclose(fl);
			return;
		}
	}
	if (n > 0) {
		data.resize(n);
		fread(data.data(), 1, n, fl);
		SSL_SendData(*client, data);
	}
	fclose(fl);
}

static void UnixTimeToFileTime(const time_t& t, LPFILETIME pft)
{
	LONGLONG ll;
	ll = Int32x32To64(t, 10000000) + 116444736000000000;
	pft->dwLowDateTime = (DWORD)ll;
	pft->dwHighDateTime = ll >> 32;
}

int AcceptSSL(SSL* ssl)
{
	int nErrCode = 0;
	do
	{
		int error = SSL_accept(ssl);
		if (error == 1)
			return 1;
		nErrCode = SSL_get_error(ssl, error);
	} while (nErrCode == SSL_ERROR_WANT_READ || nErrCode == SSL_ERROR_WANT_WRITE);
	return -1;
}

size_t multipart::parse(const string& body, size_t offset, const std::string& boundary)
{
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
		//name = sha512(dt.data);
		name = matches2[1];
		dt.filename = matches2[2];
	}
	paramsdata[name] = dt;

	return end;
}

int parse_post_url_encoded_multipart(const CLIENT_CONNECTION& client, const string& RequestHTTP, int content_length, multipart& payload) {
	std::regex e("Content-Type: multipart/form-data; boundary=([^\\r\\n\\t\\f\\v]*)", std::regex_constants::ECMAScript);
	std::smatch matches;
	if (std::regex_search(RequestHTTP, matches, e)) {
		if (content_length >= POST_BODY_MULTIPART_MAX) {
			return 3;
		}
		string boundary = matches[1];
		boundary = "--" + boundary;

		string subdata;
		subdata.resize(content_length);
		if (SSL_RecvData(client, subdata.data(), content_length) == false)
			return 3;

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

void HTTPParse(const string& HttpHeader, RequestParserStruct& hdr, const string& Connection, const multipart& mpart, const string& PostUrlEncoded)
{
	string HttpHdr = HttpHeader;
	hdr.multi_part = mpart;
	hdr.connection = Connection;
	size_t Pos = HttpHdr.find(" ");
	string MtStr = HttpHdr.substr(0, Pos);
	if (MtStr == "GET")
		hdr.method = HTTP_METHOD::HTTP_METHOD_GET;
	else if (MtStr == "POST")
		hdr.method = HTTP_METHOD::HTTP_METHOD_POST;
	else if (MtStr == "PUT")
		hdr.method = HTTP_METHOD::HTTP_METHOD_PUT;
	else if (MtStr == "DELETE")
		hdr.method = HTTP_METHOD::HTTP_METHOD_DELETE;
	else if (MtStr == "HEAD")
		hdr.method = HTTP_METHOD::HTTP_METHOD_HEAD;
	else if (MtStr == "CONNECT")
		hdr.method = HTTP_METHOD::HTTP_METHOD_CONNECT;
	else if (MtStr == "OPTIONS")
		hdr.method = HTTP_METHOD::HTTP_METHOD_OPTIONS;
	else if (MtStr == "TRACE")
		hdr.method = HTTP_METHOD::HTTP_METHOD_TRACE;
	else if (MtStr == "PATCH")
		hdr.method = HTTP_METHOD::HTTP_METHOD_PATCH;
	else
		hdr.method = HTTP_METHOD::HTTP_METHOD_UNKNOW;

	HttpHdr = HttpHdr.substr(Pos + 2);
	Pos = HttpHdr.find(" ");
	string GetParams = HttpHdr.substr(0, Pos);
	hdr.http_request.add_decode("OPath", GetParams);
	hdr.http_request.add_decode("Path", GetParams.substr(0, GetParams.find("?")));
	GetParams = GetParams.substr(GetParams.find("?") + 1);
	hdr.http_get_param.parse(GetParams, "&", true);
	HttpHdr = HttpHdr.substr(Pos + 6);
	Pos = HttpHdr.find("\r\n");
	hdr.http_request.add("Version", HttpHdr.substr(0, Pos));
	HttpHdr = HttpHdr.substr(Pos + 2);
	while ((Pos = HttpHdr.find(": ")) != string::npos)
	{
		hdr.http_request.add(HttpHdr.substr(0, Pos), HttpHdr.substr(Pos + 2, HttpHdr.find("\r\n") - (Pos + 2)));
		HttpHdr = HttpHdr.substr(HttpHdr.find("\r\n") + 2);
	}
	//HttpHdr = HttpHdr.substr(HttpHdr.find("\r\n") + 2);
	hdr.http_post_param.parse(PostUrlEncoded, "&", true);
	if (hdr.http_request.exist("Cookie"))
	{
		wstring Cookiest = hdr.http_request["Cookie"];
		hdr.http_request.erase("Cookie");
		hdr.cookies.parse(wchar_to_UTF8(Cookiest), "; ", true);
	}
	if (search(hdr.http_request.get_utf8("User-Agent"), "iPad") != string::npos)
		hdr.Device = TARGET_DEVICES::DEVICE_IPAD;
	else if (search(hdr.http_request.get_utf8("User-Agent"), "iPhone") != string::npos)
		hdr.Device = TARGET_DEVICES::DEVICE_IPHONE;
	else if (search(hdr.http_request.get_utf8("User-Agent"), "Android") != string::npos)
		hdr.Device = TARGET_DEVICES::DEVICE_ANDROID;
	else if (search(hdr.http_request.get_utf8("User-Agent"), "Windows Phone") != string::npos)
		hdr.Device = TARGET_DEVICES::DEVICE_WINDOWS_PHONE;
	else if (search(hdr.http_request.get_utf8("User-Agent"), "Windows") != string::npos)
		hdr.Device = TARGET_DEVICES::DEVICE_WINDOWS;
	else
		hdr.Device = TARGET_DEVICES::DEVICE_ANDROID;
}

bool ReciveHTTPRequest(const CLIENT_CONNECTION& client, string& RequestHTTP, multipart& mpart, string& PostUrlEncoded)
{
	size_t nCurrentPos = 0;
	while (RequestHTTP.find("\r\n\r\n") == string::npos)
	{
		char Tmp = 0;
		if (SSL_RecvData(client, &Tmp, 1) == false)
			return false;
		RequestHTTP.push_back(Tmp);
	}
	bool IsGet = RequestHTTP.find("GET") != string::npos;
	bool IsPost = RequestHTTP.find("POST") != string::npos;
	if (IsGet == false && IsPost == false)
		return false;
	if (IsPost == true)
	{
		int PaySize = 0;
		size_t CntOf = RequestHTTP.find("Content-Length: ");
		if (CntOf != string::npos) {
			try {
				PaySize = stoi(RequestHTTP.substr(CntOf + 16, RequestHTTP.substr(CntOf + 16).find("\r\n")));
			}
			catch (std::exception e) {
				return false;
			}
		}
		if (PaySize > 0)
		{
			int status = parse_post_url_encoded_multipart(client, RequestHTTP, PaySize, mpart);
			if (status == 0)
			{
				if (PaySize >= POST_BODY_MAX) {
					return false;
				}
				PostUrlEncoded.resize(PaySize);
				if (SSL_RecvData(client, PostUrlEncoded.data(), PaySize) == false)
					return false;
			}
			else if (status == 3) {
				return false;
			}
		}
	}
	return true;
}

DWORD WINAPI HTTPS_ServerThread(LPVOID arg)
{
	ACCEPT_STRUCT satr = *(ACCEPT_STRUCT*)arg;
	SSL_CTX* ctx = (SSL_CTX*)satr.ssl_ctx;
	delete (ACCEPT_STRUCT*)arg;

	CLIENT_CONNECTION client;
	client.client_sock = satr.client_sock;
	client.isSSL = satr.isSSL;
	client.ssl = NULL;
	client.server_config = satr.server_config;

	if (satr.isSSL) {
		client.ssl = SSL_new(ctx);
		if (client.ssl == NULL)
		{
			closesocket(satr.client_sock);
			return 0;
		}
		BIO* soc = BIO_new_socket((int)satr.client_sock, BIO_NOCLOSE);
		SSL_set_bio(client.ssl, soc, soc);
	}
	RequestParserStruct Req;
	string HttpRequest;
	string http_post_url_encoded;
	multipart mpart;

	bool NeedRedirect = false;
	if (satr.isSSL) {
		int sslacceptcode = AcceptSSL(client.ssl);
		if (sslacceptcode != 1)
		{
#ifdef PRINT_SSL_ACCEPT_ERROR
			PrintMessage(L"       |SSL_accept " + UTF8_to_wchar(connection) + L" вернул " + to_wstring(sslacceptcode) + L"; " + UTF8_to_wchar(ERR_error_string(ERR_get_error(), NULL)) + L"|", CLRV(255, 0, 0));
#endif
			goto out;
		}
	}

	if (ReciveHTTPRequest(client, HttpRequest, mpart, http_post_url_encoded) == false)
		goto out;

	HTTPParse(HttpRequest, Req, satr.connection, mpart, http_post_url_encoded);

	serverMethods.Execute(Req.http_request.get_utf8("Path"), Req, &client);
	goto out;
out:
	if (satr.isSSL) {
		SSL_shutdown(client.ssl);
		SSL_free(client.ssl);
	}
	shutdown(satr.client_sock, 1);
	closesocket(satr.client_sock);
	Sleep(1000);
	return 0;
}

static string GetConnectionIP(SOCKADDR_IN& FromAddr)
{
	string buf;
	buf.resize(260);
	inet_ntop(AF_INET, &FromAddr.sin_addr, (char*)buf.data(), 260);
	buf = buf.c_str();
	return buf;
}

void ServerMethods::RegisterMethod(const string& UTF8Path, void* Func, bool NeedHost, bool NeedAuth)
{
	if (Methods.exist(UTF8Path) == true)
		throw runtime_error("Method Exist");

	Methods[UTF8Path] = ServerMethodInfo(Func, NeedHost, NeedAuth);
}

void ServerMethods::Execute(const string& UTF8Path, RequestParserStruct& Req, void* ssl) const
{
	CLIENT_CONNECTION* client = (CLIENT_CONNECTION*)ssl;
	if (Methods.exist(UTF8Path) == false) {
		return;
	}
	ServerMethodInfo info = Methods.at(UTF8Path);
	if (info.NeedHost && !Req.http_request.exist("Host")) {
		if (client->server_config.isDebug) {
			PrintMessage(L"DEBUG: метод " + UTF8_to_wchar(UTF8Path) + L" требует http заголовок Host", URGB(255, 0, 0));
		}
		SendErrorJSON(client, L"DEBUG: метод " + UTF8_to_wchar(UTF8Path) + L" требует http заголовок Host");
		return;
	}
	if (info.NeedAuth && exist_get_post(Req, "password")) {
		string password = get_postUTF8(Req, "password");
		if (client->server_config.password != password) {
			if (client->server_config.isDebug) {
				PrintMessage(L"DEBUG: метод " + UTF8_to_wchar(UTF8Path) + L" неверный пароль", URGB(255, 0, 0));
			}
			SendErrorJSON(client, L"Неверный пароль");
			return;
		}
	}
	else if (info.NeedAuth) {
		if (client->server_config.isDebug) {
			PrintMessage(L"DEBUG: метод " + UTF8_to_wchar(UTF8Path) + L" требует авторизацию", URGB(255, 0, 0));
		}
		SendErrorJSON(client, L"DEBUG: метод " + UTF8_to_wchar(UTF8Path) + L" требует авторизацию");
		return;
	}
	((method_t)Methods.at(UTF8Path).Func)(Req, (CLIENT_CONNECTION*)ssl);
}

static std::time_t to_time_t(const ULARGE_INTEGER& ull)
{
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
static std::time_t to_time_t(const FILETIME& ft)
{
	std::time_t ret = 0;
	// Преобразовать в ULARGE_INTEGER
	ULARGE_INTEGER ull;
	ull.LowPart = ft.dwLowDateTime;
	ull.HighPart = ft.dwHighDateTime;
	// Преобразовать в time_t
	return to_time_t(ull);
}
template <typename TP> std::time_t to_time_t(TP tp)
{
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
		} while (FindNextFile(hFind, &data));
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
	vector<wstring> skip = { L"jpg", L"jpeg", L"png", L"gif", L"txt", L"pdf", L"doc", L"docx", L"ppt", L"pptx", L"ini", L"xml", L"html" };
	if (type == TYPE_SCAN::TYPE_SCAN_PHOTO) {
		skip = { L"gif", L"txt", L"pdf", L"doc", L"docx", L"ppt", L"pptx", L"ini", L"xml", L"html" };
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
					a = Audio(combine_path(root, offset), data.cFileName, to_time_t(data.ftLastWriteTime), to_time_t(data.ftCreationTime), (data.nFileSizeHigh * (MAXDWORD + 1)) + data.nFileSizeLow);
					mAudios[a.get_hash()] = a;
					if (scanFS) {
						auto dirPtr = RootDir.find_by_path(combine_path(dir_entry, offset));
						Inode(data.cFileName).audio((data.nFileSizeHigh * (MAXDWORD + 1)) + data.nFileSizeLow, to_time_t(data.ftLastWriteTime)).attachTo(*dirPtr, root);
					}
					break;
				case TYPE_SCAN::TYPE_SCAN_DISCOGRAPHY:
					a = Audio(combine_path(root, offset), data.cFileName, to_time_t(data.ftLastWriteTime), to_time_t(data.ftCreationTime), (data.nFileSizeHigh * (MAXDWORD + 1)) + data.nFileSizeLow);
					mDiscography[a.get_hash()] = a;
					if (scanFS) {
						auto dirPtr = RootDir.find_by_path(combine_path(dir_entry, offset));
						Inode(data.cFileName).audio((data.nFileSizeHigh * (MAXDWORD + 1)) + data.nFileSizeLow, to_time_t(data.ftLastWriteTime)).attachTo(*dirPtr, root);
					}
					break;
				case TYPE_SCAN::TYPE_SCAN_VIDEO:
					v = Video(combine_path(root, offset), data.cFileName, to_time_t(data.ftLastWriteTime), to_time_t(data.ftCreationTime), (data.nFileSizeHigh * (MAXDWORD + 1)) + data.nFileSizeLow);
					mVideos[v.get_hash()] = v;
					if (scanFS) {
						auto dirPtr = RootDir.find_by_path(combine_path(dir_entry, offset));
						Inode(data.cFileName).video((data.nFileSizeHigh * (MAXDWORD + 1)) + data.nFileSizeLow, to_time_t(data.ftLastWriteTime)).attachTo(*dirPtr, root);
					}
					break;
				case TYPE_SCAN::TYPE_SCAN_PHOTO:
					wstring fl = data.cFileName;
					if (WSTRUtils::wsearch(fl, L".jpg") != wstring::npos || WSTRUtils::wsearch(fl, L".jpeg") != wstring::npos || WSTRUtils::wsearch(fl, L".png") != wstring::npos || WSTRUtils::wsearch(fl, L".tiff") != wstring::npos || WSTRUtils::wsearch(fl, L".webp") != wstring::npos) {
						p = Photo(combine_path(root, offset), data.cFileName, to_time_t(data.ftLastWriteTime), to_time_t(data.ftCreationTime), (data.nFileSizeHigh * (MAXDWORD + 1)) + data.nFileSizeLow);
						mPhotos[p.get_hash()] = p;
						if (scanFS) {
							auto dirPtr = RootDir.find_by_path(combine_path(dir_entry, offset));
							Inode(data.cFileName).photo((data.nFileSizeHigh * (MAXDWORD + 1)) + data.nFileSizeLow, to_time_t(data.ftLastWriteTime)).attachTo(*dirPtr, root);
						}
					}
					else {
						v = Video(combine_path(root, offset), data.cFileName, to_time_t(data.ftLastWriteTime), to_time_t(data.ftCreationTime), (data.nFileSizeHigh * (MAXDWORD + 1)) + data.nFileSizeLow);
						mVideos[v.get_hash()] = v;
						if (scanFS) {
							auto dirPtr = RootDir.find_by_path(combine_path(dir_entry, offset));
							Inode(data.cFileName).video((data.nFileSizeHigh * (MAXDWORD + 1)) + data.nFileSizeLow, to_time_t(data.ftLastWriteTime)).attachTo(*dirPtr, root);
						}
					}
					break;
				}
			}
		} while (FindNextFile(hFind, &data));
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
static bool greator(const Map::Pair<string, T>& lhs, const Map::Pair<string, T>& rhs)
{
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
		wstring cmd = (L"\"" + ExtractAppPath() + L"\\ffmpeg.exe\"" + (L" -loglevel panic -n -i \"" + i.get_value().get_path() + L"\" -ss 00:00:04 -frames 1 -q:v 1 -vf scale=960:-2 -q:v 3 " + CACHE_DIR + L"\\" + UTF8_to_wchar(i.get_value().get_hash()) + L".jpg"));
		PrintMessage(L"(" + to_wstring(++ccn) + L" из " + to_wstring(tVideos.size()) + L") " + cmd);
		if (!CreateProcessW((ExtractAppPath() + L"\\ffmpeg.exe").c_str(), (LPWSTR)cmd.c_str(),
			NULL,
			NULL,
			FALSE,                 // handles are inherited 
			CREATE_NO_WINDOW,                    // creation flags 
			NULL,                 // use parent's environment 
			crdir.c_str(),                 // use parent's current directory 
			&siStartInfo,         // STARTUPINFO pointer 
			&piProcInfo))
		{
			CloseHandle(piProcInfo.hProcess);
			CloseHandle(piProcInfo.hThread);
			continue;
		}
		handles[i.get_value()] = piProcInfo;
		if (handles.size() % Startinit.ffmpeg_proc_count == 0) {
			doAfterFFMpegEnded(handles);
		}
	}
	if (handles.size() > 0)
		doAfterFFMpegEnded(handles);
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
		wstring cmd = (L"\"" + ExtractAppPath() + L"\\ffmpeg.exe\"" + (L" -loglevel panic -n -i \"" + i.get_value().get_path() + L"\" -vf scale=\"\'if(gt(a,1/1),512,-1)\':\'if(gt(a,1/1),-1,512)\'\" " + CACHE_DIR + L"\\" + UTF8_to_wchar(i.get_value().get_hash()) + L".jpg"));
		PrintMessage(L"(" + to_wstring(++ccn) + L" из " + to_wstring(tPhotos.size()) + L") " + cmd);
		if (!CreateProcessW((ExtractAppPath() + L"\\ffmpeg.exe").c_str(), (LPWSTR)cmd.c_str(),
			NULL,
			NULL,
			FALSE,                 // handles are inherited 
			CREATE_NO_WINDOW,                    // creation flags 
			NULL,                 // use parent's environment 
			crdir.c_str(),                 // use parent's current directory 
			&siStartInfo,         // STARTUPINFO pointer 
			&piProcInfo))
		{
			CloseHandle(piProcInfo.hProcess);
			CloseHandle(piProcInfo.hThread);
			continue;
		}
		handles[i.get_value()] = piProcInfo;
		if (handles.size() % Startinit.ffmpeg_proc_count == 0) {
			doAfterFFMpegEnded(handles);
		}
	}
	if (handles.size() > 0)
		doAfterFFMpegEnded(handles);
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
		wstring cmd = (L"\"" + ExtractAppPath() + L"\\ffmpeg.exe\"" + (L" -loglevel panic -n -i \"" + i.get_value().get_path() + L"\" " + CACHE_DIR + L"\\" + UTF8_to_wchar(i.get_value().get_hash()) + L".jpg"));
		PrintMessage(L"(" + to_wstring(++ccn) + L" из " + to_wstring(tAudios.size()) + L") " + cmd);
		if (!CreateProcessW((ExtractAppPath() + L"\\ffmpeg.exe").c_str(), (LPWSTR)cmd.c_str(),
			NULL,
			NULL,
			FALSE,                 // handles are inherited 
			CREATE_NO_WINDOW,                    // creation flags 
			NULL,                 // use parent's environment 
			crdir.c_str(),                 // use parent's current directory 
			&siStartInfo,         // STARTUPINFO pointer 
			&piProcInfo))
		{
			CloseHandle(piProcInfo.hProcess);
			CloseHandle(piProcInfo.hThread);
			continue;
		}
		handles[i.get_value()] = piProcInfo;
		if (handles.size() % Startinit.ffmpeg_proc_count == 0) {
			doAfterFFMpegEnded(handles);
		}
	}
	if (handles.size() > 0)
		doAfterFFMpegEnded(handles);
}

/*
void genDiscographyThumbs(bool is_OnlyNews) {
	if (!PathFileExistsW(CACHE_DIR)) {
		if (!CreateDirectoryW(CACHE_DIR, NULL)) {
			return;
		}
	}
	cleanLoaded(TYPE_SCAN::TYPE_SCAN_DISCOGRAPHY);
	for (auto& i : Discography_Dirs) {
		scanFiles(i, TYPE_SCAN::TYPE_SCAN_DISCOGRAPHY);
	}
	Map::Map<std::string, Audio> tAudios = mDiscography;
	wstring crdir;
	crdir.resize(MAX_PATH + 1);
	GetCurrentDirectoryW(MAX_PATH, (LPWSTR)crdir.c_str());
	PrintMessage(L"Сканирование обложек у дискографии : " + to_wstring(tAudios.size()));
	size_t ccn = 0;
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
		wstring cmd = (L"\"" + ExtractAppPath() + L"\\ffmpeg.exe\"" + (L" -loglevel panic -n -i \"" + i.get_value().get_path() + L"\" " + CACHE_DIR + L"\\" + UTF8_to_wchar(i.get_value().get_hash()) + L".jpg"));
		PrintMessage(L"(" + to_wstring(++ccn) + L" из " + to_wstring(tAudios.size()) + L") " + cmd);
		if (!CreateProcessW((ExtractAppPath() + L"\\ffmpeg.exe").c_str(), (LPWSTR)cmd.c_str(),
			NULL,
			NULL,
			FALSE,                 // handles are inherited 
			CREATE_NO_WINDOW,                    // creation flags 
			NULL,                 // use parent's environment 
			crdir.c_str(),                 // use parent's current directory 
			&siStartInfo,         // STARTUPINFO pointer 
			&piProcInfo))
		{
			CloseHandle(piProcInfo.hProcess);
			CloseHandle(piProcInfo.hThread);
			continue;
		}
		handles[i.get_value()] = piProcInfo;
		if (handles.size() % Startinit.ffmpeg_proc_count == 0) {
			doAfterFFMpegEnded(handles);
		}
	}
	if(handles.size() > 0)
		doAfterFFMpegEnded(handles);
}
*/

DWORD WINAPI doScanCovers(LPVOID) {
	dlgS.StartBT.EnableWindow(FALSE);
	dlgS.ScanCovers.EnableWindow(FALSE);
	dlgS.MediaFolders.EnableWindow(FALSE);
	dlgS.PhotosThumb.EnableWindow(FALSE);
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
	return 0;
}

static void UpdateTime(RequestParserStruct& Req, CLIENT_CONNECTION* client) {
	if (!exist_get_post(Req, "hash")) {
		SendErrorJSON(client, L"Не передан hash");
		return;
	}
	string hash = get_postUTF8(Req, "hash");
	THREAD_ACCESS_LOCK(DEFAULT_GUARD_NAME, &mDiscography, &mAudios, &mVideos, &mPhotos, &RootDir);
	Media* md = look_MediaByHash(hash);
	if (md == NULL) {
		THREAD_ACCESS_UNLOCK(DEFAULT_GUARD_NAME, &mDiscography, &mAudios, &mVideos, &mPhotos, &RootDir);
		SendErrorJSON(client, L"Hash не найден");
		return;
	}
	Media media = *md;
	THREAD_ACCESS_UNLOCK(DEFAULT_GUARD_NAME, &mDiscography, &mAudios, &mVideos, &mPhotos, &RootDir);

	bool needAnswer = true;
	if (!Startinit.canEdit) {
		SendHTTTPAnswerWithData(*client, "200 OK", "application/json; charset=utf-8", "{ \"response\": 1, \"requested\": 1 }");
		needAnswer = false;
		wstring pth = media.get_path();
		if ((win_message().message_type(MSG_TYPE::TYPE_QUESTION).button_type(MSG_BUTTON::BUTTON_YESNO) << L"Обновить дату модификации " << pth << L"?").show() != IDYES)
			return;
	}

	HANDLE file_handle =
		::CreateFileW(media.get_path().c_str(), FILE_WRITE_ATTRIBUTES,
			NULL, NULL, OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL, 0);
	if (file_handle != NULL)
	{
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
		SendHTTTPAnswerWithData(*client, "200 OK", "application/json; charset=utf-8", "{ \"response\": 1 }");
	}
}

static void UpdateFileName(RequestParserStruct& Req, CLIENT_CONNECTION* client) {
	if (!exist_get_post(Req, "hash") || !exist_get_post(Req, "name")) {
		SendErrorJSON(client, L"Не передан hash или name");
		return;
	}
	string hash = get_postUTF8(Req, "hash");
	wstring new_fl = UTF8_to_wchar(trim(get_postUTF8(Req, "name")));
	if (new_fl.empty()) {
		SendErrorJSON(client, L"name пустой");
		return;
	}
	new_fl = FixFileName(new_fl);
	if (PathFileExistsW(new_fl.c_str())) {
		SendErrorJSON(client, L"Файл существует с таким именем!");
		return;
	}
	THREAD_ACCESS_LOCK(DEFAULT_GUARD_NAME, &mDiscography, &mAudios, &mVideos, &mPhotos, &RootDir);
	Media* md = look_MediaByHash(hash);
	if (md == NULL) {
		THREAD_ACCESS_UNLOCK(DEFAULT_GUARD_NAME, &mDiscography, &mAudios, &mVideos, &mPhotos, &RootDir);
		SendErrorJSON(client, L"Hash не найден");
		return;
	}
	Media media = *md;
	THREAD_ACCESS_UNLOCK(DEFAULT_GUARD_NAME, &mDiscography, &mAudios, &mVideos, &mPhotos, &RootDir);

	bool needAnswer = true;
	if (!Startinit.canEdit) {
		SendHTTTPAnswerWithData(*client, "200 OK", "application/json; charset=utf-8", "{ \"response\": 1, \"requested\": 1 }");
		needAnswer = false;
		wstring pth = media.get_path();
		if ((win_message().message_type(MSG_TYPE::TYPE_QUESTION).button_type(MSG_BUTTON::BUTTON_YESNO) << L"Переменововать " << pth << L" в" << combine_path(media.get_orig_dir(), new_fl) << L"?").show() != IDYES)
			return;
	}

	if (!MoveFileW(media.get_path().c_str(), combine_path(media.get_orig_dir(), new_fl).c_str())) {
		if (needAnswer) {
			SendErrorJSON(client, L"Переименовать не удалось!");
		}
		else {
			PrintMessage(L"Переименовать не удалось!", URGB(255, 200, 0));
		}
		return;
	}
	HANDLE file_handle =
		::CreateFileW(combine_path(media.get_orig_dir(), new_fl).c_str(), FILE_WRITE_ATTRIBUTES,
			NULL, NULL, OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL, 0);
	if (file_handle != NULL)
	{
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
		SendHTTTPAnswerWithData(*client, "200 OK", "application/json; charset=utf-8", "{ \"response\": 1 }");
	}
}

static void DeleteFileSrv(RequestParserStruct& Req, CLIENT_CONNECTION* client) {
	if (!exist_get_post(Req, "hash")) {
		SendErrorJSON(client, L"Не передан hash");
		return;
	}
	string hash = get_postUTF8(Req, "hash");
	THREAD_ACCESS_LOCK(DEFAULT_GUARD_NAME, &mDiscography, &mAudios, &mVideos, &mPhotos, &RootDir);
	Media* md = look_MediaByHash(hash);
	if (md == NULL) {
		THREAD_ACCESS_UNLOCK(DEFAULT_GUARD_NAME, &mDiscography, &mAudios, &mVideos, &mPhotos, &RootDir);
		SendErrorJSON(client, L"Hash не найден");
		return;
	}
	Media media = *md;
	std::wstring cover = get_cover(media.get_hash());

	bool needAnswer = true;
	if (!Startinit.canEdit) {
		SendHTTTPAnswerWithData(*client, "200 OK", "application/json; charset=utf-8", "{ \"response\": 1, \"requested\": 1 }");
		needAnswer = false;
		wstring pth = media.get_path();
		THREAD_ACCESS_UNLOCK(DEFAULT_GUARD_NAME, &mDiscography, &mAudios, &mVideos, &mPhotos, &RootDir);
		if ((win_message().message_type(MSG_TYPE::TYPE_QUESTION).button_type(MSG_BUTTON::BUTTON_YESNO) << L"Удалить файл " << pth << L"?").show() != IDYES)
			return;
		THREAD_ACCESS_LOCK(DEFAULT_GUARD_NAME, &mDiscography, &mAudios, &mVideos, &mPhotos, &RootDir);
	}

	if (!DeleteFileW(media.get_path().c_str())) {
		THREAD_ACCESS_UNLOCK(DEFAULT_GUARD_NAME, &mDiscography, &mAudios, &mVideos, &mPhotos, &RootDir);
		if (needAnswer) {
			SendErrorJSON(client, L"Файл не удалён");
		}
		else {
			PrintMessage(L"Файл не удалён", URGB(255, 200, 0));
		}
		return;
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
		SendHTTTPAnswerWithData(*client, "200 OK", "application/json; charset=utf-8", "{ \"response\": 1 }");
	}
}

static void GetFileName(RequestParserStruct& Req, CLIENT_CONNECTION* client) {
	if (!exist_get_post(Req, "hash")) {
		SendErrorJSON(client, L"Не передан hash");
		return;
	}
	string hash = get_postUTF8(Req, "hash");
	THREAD_ACCESS_LOCK(DEFAULT_GUARD_NAME, &mDiscography, &mAudios, &mVideos, &mPhotos);
	Media* md = look_MediaByHash(hash);
	if (md == NULL) {
		THREAD_ACCESS_UNLOCK(DEFAULT_GUARD_NAME, &mDiscography, &mAudios, &mVideos, &mPhotos);
		SendErrorJSON(client, L"Hash не найден");
		return;
	}
	Media media = *md;
	THREAD_ACCESS_UNLOCK(DEFAULT_GUARD_NAME, &mDiscography, &mAudios, &mVideos, &mPhotos);
	json data = json(json::value_t::object);
	data.emplace("response", WSTRUtils::wchar_to_UTF8(media.get_orig_name()));

	SendHTTTPAnswerWithData(*client, "200 OK", "application/json; charset=utf-8", data.dump());
}

void stopAudioPlay() {
	THREAD_ACCESS_LOCK(DEFAULT_GUARD_NAME, &player);
	player.Stop();
	THREAD_ACCESS_UNLOCK(DEFAULT_GUARD_NAME, &player);
}

void InitMediaServer()
{
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		(win_message(dlgS.m_hWnd).timeout(5).message_type(MSG_TYPE::TYPE_ERROR) << L"Ошибка инициализации сокетов").show();
		return;
	}

	if (serverMethods.GetInited() == false)
	{
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
		serverMethods.SetInited();
	}

	THREAD_ACCESS_REGISTER_POINTERS(DEFAULT_GUARD_NAME, &mDiscography, &mAudios, &mVideos, &mPhotos, &player, &RootDir);

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
	
	SSL_CTX* ctx = NULL;
	if (Startinit.isSsl) {
		ctx = initialize_ctx(Startinit.isDebug);
		if (ctx == NULL)
			return;
	}
	HTTPSserver_sock = 0;
	SOCKET client_sock = 0;
	HTTPSserver_sock = tcp_listen(Startinit.isSsl ? Startinit.HttpsPort : Startinit.HttpPort);
	if (HTTPSserver_sock == SOCKET_ERROR)
	{
		(win_message().timeout(5).message_type(MSG_TYPE::TYPE_ERROR) << L"Сбой в инициализации сервера(socket bind)").show();
		return;
	}
	if (Startinit.isSsl) {
		PrintMessage(L"[HTTPS инициализирован, порт " + to_wstring(Startinit.HttpsPort) + L"]", URGB(0, 140, 255));
	}
	else {
		PrintMessage(L"[HTTP инициализирован, порт " + to_wstring(Startinit.HttpPort) + L"]", URGB(0, 140, 255));
	}
	SOCKADDR_IN FromAddr;
	int len = sizeof(SOCKADDR_IN);
	while (true)
	{
		client_sock = accept(HTTPSserver_sock, (SOCKADDR*)&FromAddr, &len);
		if (client_sock == SOCKET_ERROR)
			continue;
		SetTimeOutsSocket(client_sock);
		ACCEPT_STRUCT* satr = new ACCEPT_STRUCT;
		satr->connection = GetConnectionIP(FromAddr) + string(":") + to_string(ntohs(FromAddr.sin_port));
		satr->client_sock = client_sock;
		satr->ssl_ctx = ctx;
		satr->isSSL = Startinit.isSsl;
		satr->server_config = Startinit;
		CreateThread(NULL, NULL, &HTTPS_ServerThread, satr, NULL, NULL);
	}
	closesocket(HTTPSserver_sock);
	if (ctx != NULL) {
		SSL_CTX_free(ctx);
	}
}
