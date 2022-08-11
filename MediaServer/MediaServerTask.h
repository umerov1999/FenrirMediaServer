#pragma once
#include <iostream>
#include <vector>
#include <ctime>
#include <regex>
#include "json.hpp"
#include "Map.h"
#include "sha1.hpp"
#include "crc32.h"
#include "WSTRUtils.h"
#define SERVER_NAME "Fenrir"
#define CACHE_DIR L"Thumbs"
#define SOCKTIMEOUT 60 * 60
#define BUFFERSIZE 8196
#define POST_BODY_MAX 2 * 1024 * 1024
#define POST_BODY_MULTIPART_MAX 50 * 1024 * 1024
//#define CRITICAL_DEBUG 1
//#define PRINT_SSL_ACCEPT_ERROR 1
//#define PRINT_REDIRECT 1

#define minu(a,b)(((a) < (b)) ? (a) : (b))
#define maxu(a,b)(((a) > (b)) ? (a) : (b))

#define UTF8START "\xef\xbb\xbf"
#define ENDL "\r\n"
#define WENDL L"\r\n"

static std::wstring GetAppName()
{
	const wchar_t* FnS = L"\\/";
	TCHAR szFileName[MAX_PATH];
	GetModuleFileNameW(0, szFileName, MAX_PATH);

	std::wstring fname = szFileName;
	size_t pos = fname.find_last_of(FnS);
	return (std::wstring::npos == pos)
		? fname
		: fname.substr(pos + 1);
}
#define SETTINGS (GetAppName() + L".settings.json")

class InitServer
{
public:
	InitServer() {
		HttpsPort = 0;
		HttpPort = 0;
		isSsl = false;
		isDebug = false;
		canEdit = false;
		ffmpeg_proc_count = 8;
	}
	std::string password;
	int HttpsPort;
	int HttpPort;
	bool isSsl;
	bool isDebug;
	bool canEdit;
	int ffmpeg_proc_count;
};

#define USSL_CERTPART_CERT 0
#define USSL_CERTPART_BUNDLE 1
#define USSL_CERTPART_KEY 2

struct USSL_CERT
{
	USSL_CERT()
	{
		Inited = false;
	}
	bool Inited;
	std::string Name;
	std::string AddTime;
	Map::Map<int, std::string>CertPart;
};

class ACCEPT_STRUCT
{
public:
	ACCEPT_STRUCT() {
		isSSL = false;
		ssl_ctx = NULL;
		client_sock = NULL;
	}
	bool isSSL;
	std::string connection;
	void* ssl_ctx;
	SOCKET client_sock;
	InitServer server_config;
};

class wstringparam
{
public:
	void add(const std::string& name, const std::wstring& value);
	void add_decode(const std::string& name, const std::string& value);
	void add(const std::string& name, const std::string& value);
	int count() const;
	void parse(const std::string& paramstring, const std::string& separator, bool need_url_decode);
	void parse(const std::wstring& paramstring, const std::string& separator);
	bool exist(const std::string& name);
	void erase(const std::string& name);
	std::wstring operator[](const std::string& name) const;
	std::string get_utf8(const std::string& name) const;
	void set_utf8(const std::string& name, const std::string& value);
public:
	Map::Map<std::string, std::wstring> paramsdata;
};

class multipart
{
public:
	struct multipart_data
	{
		multipart_data() {

		}
		multipart_data(const std::string& filename, const std::string& data) {
			this->filename = filename;
			this->data = data;
		}

		bool isFile() const {
			return filename.length() > 0;
		}

		std::wstring get_Wfilename() {
			return WSTRUtils::UTF8_to_wchar(filename);
		}

		std::wstring get_Wvalue() {
			return WSTRUtils::UTF8_to_wchar(data);
		}

		std::string filename;
		std::string data;
	};

	int count() const {
		return (int)paramsdata.size();
	}
	bool exist(const std::string &name) const {
		return paramsdata.exist(name);
	}
	void erase(const std::string &name) {
		paramsdata.erase(name);
	}
	multipart_data get_data(const std::string &name) const {
		return paramsdata[name];
	}

	size_t parse(const std::string& body, size_t offset, const std::string& boundary);
public:
	Map::Map<std::string, multipart_data> paramsdata;
};

enum class HTTP_METHOD
{
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

enum class TARGET_DEVICES
{
	DEVICE_ANDROID,
	DEVICE_IPHONE,
	DEVICE_IPAD,
	DEVICE_WINDOWS,
	DEVICE_WINDOWS_PHONE,
	DEVICE_UNKNOW,
};
void InitMediaServer();

struct RequestParserStruct
{
	TARGET_DEVICES Device;
	HTTP_METHOD method;
	std::string connection;
	wstringparam cookies;
	wstringparam http_post_param;
	wstringparam http_get_param;
	wstringparam http_request;
	multipart multi_part;
};

class UTF8Stream
{
public:
	UTF8Stream & operator<<(const std::wstring &String);
	UTF8Stream &operator<<(const std::string &String);
	UTF8Stream &operator<<(size_t value);
	UTF8Stream &operator<<(int value);
	const std::string &GetString() const;
	int GetSize();
	void Clear();
private:
	std::string Data;
};

class Media {
public:
	enum class TYPE {
		TYPE_NULL,
		TYPE_AUDIO,
		TYPE_VIDEO,
		TYPE_PHOTO
	};
	Media() {
		type = TYPE::TYPE_NULL;
		id = 0;
		owner_id = 0;
		create_time = 0;
		mod_time = 0;
		file_size = 0;
	}
	Media(TYPE type, std::wstring path_name, std::wstring file_name, time_t mod_time, time_t create_time, unsigned long long file_size) {
		this->type = type;
		this->create_time = create_time;
		this->mod_time = mod_time;
		this->file_size = file_size;
		orig_dir = path_name;
		orig_name = file_name;
		fileName = WSTRUtils::wchar_to_UTF8(file_name);
		path = WSTRUtils::combine_path(path_name, file_name);
		id = crc32(WSTRUtils::wchar_to_UTF8(file_name));
		owner_id = crc32(WSTRUtils::wchar_to_UTF8(path_name));
		hash = std::to_string(mod_time) + SHA1::from_string(WSTRUtils::wchar_to_UTF8(path)) + "." + WSTRUtils::wchar_to_UTF8(WSTRUtils::toLowerW(get_ext(file_name)));
	}
	void operator=(const Media& media) {
		this->type = media.type;
		this->hash = media.hash;
		this->path = media.path;
		this->id = media.id;
		this->owner_id = media.owner_id;
		this->create_time = media.create_time;
		this->mod_time = media.mod_time;
		this->file_size = media.file_size;
		this->fileName = media.fileName;
		this->orig_dir = media.orig_dir;
		this->orig_name = media.orig_name;
	}
public:
	Media(const Media& media) {
		this->type = media.type;
		this->hash = media.hash;
		this->path = media.path;
		this->id = media.id;
		this->owner_id = media.owner_id;
		this->create_time = media.create_time;
		this->mod_time = media.mod_time;
		this->file_size = media.file_size;
		this->fileName = media.fileName;
		this->orig_dir = media.orig_dir;
		this->orig_name = media.orig_name;
	}
	const std::string& update_hash(time_t mod_time, time_t create_time) {
		hash = std::to_string(mod_time) + SHA1::from_string(WSTRUtils::wchar_to_UTF8(path)) + "." + WSTRUtils::wchar_to_UTF8(WSTRUtils::toLowerW(get_ext(WSTRUtils::UTF8_to_wchar(fileName))));
		return hash;
	}
	const std::string& update_full_hash(std::wstring path_name, std::wstring file_name, time_t mod_time, time_t create_time) {
		this->create_time = create_time;
		this->mod_time = mod_time;
		this->file_size = file_size;
		orig_dir = path_name;
		orig_name = file_name;
		fileName = WSTRUtils::wchar_to_UTF8(file_name);
		path = WSTRUtils::combine_path(path_name, file_name);
		id = crc32(WSTRUtils::wchar_to_UTF8(file_name));
		owner_id = crc32(WSTRUtils::wchar_to_UTF8(path_name));
		hash = std::to_string(mod_time) + SHA1::from_string(WSTRUtils::wchar_to_UTF8(path)) + "." + WSTRUtils::wchar_to_UTF8(WSTRUtils::toLowerW(get_ext(file_name)));
		return hash;
	}
	const std::string &get_hash() const {
		return hash;
	}

	std::wstring get_cover() const;

	const TYPE &get_type() const {
		return type;
	}
	const std::string getMimeType() const {
		switch (type) {
		case TYPE::TYPE_AUDIO:
			return "audio/" + getMimeExt();
		case TYPE::TYPE_VIDEO:
			return "video/" + getMimeExt();
		case TYPE::TYPE_PHOTO:
			return "image/" + getMimeExt();
		}
		return "*/*";
	}
	const int& getId() const {
		return id;
	}
	const int& getOwnerId() const {
		return owner_id;
	}
	const std::string& get_file_name() const {
		return fileName;
	}
	const std::wstring& get_path() const {
		return path;
	}
	const std::wstring& get_orig_dir() const {
		return orig_dir;
	}
	const std::wstring& get_orig_name() const {
		return orig_name;
	}
	const time_t& get_create_time() const {
		return create_time;
	}
	const time_t& get_mod_time() const {
		return mod_time;
	}
	const unsigned long long& get_file_size() const {
		return file_size;
	}
	bool operator==(const Media &media) const {
		return this->hash == media.hash;
	}
	bool operator!=(const Media& media) const {
		return this->hash != media.hash;
	}
	static std::wstring get_ext(const std::wstring& file_name) {
		std::wstring tmp = file_name;
		size_t n = tmp.find_last_of(L'.');
		if (n != std::wstring::npos && n != tmp.size() - 1) {
			return tmp.erase(0, n + 1);
		}
		return tmp;
	}
protected:
	std::string getMimeExt() const {
		std::wstring ex = get_ext(get_orig_name());
		if (WSTRUtils::toLowerW(ex) == L"jpg" || WSTRUtils::toLowerW(ex) == L"jpeg") {
			return "jpeg";
		}
		if (WSTRUtils::toLowerW(ex) == L"tiff") {
			return "tiff";
		}
		if (WSTRUtils::toLowerW(ex) == L"png") {
			return "png";
		}
		if (WSTRUtils::toLowerW(ex) == L"mp3") {
			return "mpeg";
		}
		if (WSTRUtils::toLowerW(ex) == L"ogg") {
			return "ogg";
		}
		if (WSTRUtils::toLowerW(ex) == L"mpg" || WSTRUtils::toLowerW(ex) == L"mpeg") {
			return "mpeg";
		}
		if (WSTRUtils::toLowerW(ex) == L"mp4") {
			return "mp4";
		}
		if (WSTRUtils::toLowerW(ex) == L"webm") {
			return "webm";
		}
		if (WSTRUtils::toLowerW(ex) == L"avi") {
			return "x-msvideo";
		}

		return "*";
	}
	static std::wstring strip_ext(const std::wstring& file_name) {
		std::wstring tmp = file_name;
		size_t n = tmp.find_last_of(L'.');
		if (n != std::wstring::npos) {
			return tmp.erase(n, tmp.size() - n);
		}
		return tmp;
	}
	TYPE type;
	std::string hash;
	std::string fileName;
	std::wstring path;
	std::wstring orig_dir;
	std::wstring orig_name;
	int id;
	int owner_id;
	time_t mod_time;
	time_t create_time;
	unsigned long long file_size;
};

static std::vector<std::string> split(const std::string& input, const std::string& regex) {
	std::regex re(regex);
	std::sregex_token_iterator
		first{ input.begin(), input.end(), re, -1 },
		last;
	return { first, last };
}

static std::vector<std::wstring> wsplit(const std::wstring& input, const std::wstring& regex) {
	std::wregex re(regex);
	std::wsregex_token_iterator
		first{ input.begin(), input.end(), re, -1 },
		last;
	return { first, last };
}

inline std::string trim(const std::string& s)
{
	auto  wsfront = std::find_if_not(s.begin(), s.end(), [](int c) {return std::isspace(c); });
	return std::string(wsfront, std::find_if_not(s.rbegin(), std::string::const_reverse_iterator(wsfront), [](int c) {return std::isspace(c); }).base());
}

static inline std::wstring sreplace(const std::wstring& s, const std::wstring& find, const std::wstring& replace) {
	std::wstring ret = s;
	size_t l = ret.find(find);
	if (l != std::wstring::npos) {
		ret.replace(l, find.length(), replace);
	}
	return ret;
}

class Audio: public Media {
public:
	Audio() : Media() {
	}
	Audio(const Audio& media) : Media(media) {
		this->title = media.title;
		this->artist = media.artist;
	}
	void operator=(const Audio& media) {
		this->title = media.title;
		this->artist = media.artist;
		Media::operator=(media);
	}
	Audio(std::wstring path_name, std::wstring file_name, time_t mod_time, time_t create_time, unsigned long long file_size) : Media(Media::TYPE::TYPE_AUDIO, path_name, file_name, mod_time, create_time, file_size) {
		std::wstring tmp = Media::strip_ext(file_name);
		std::wstring Artist;
		std::vector<std::wstring> arr = wsplit(tmp, L" - ");
		if (arr.size() > 1) {
			Artist = arr[0];
			title = WSTRUtils::wchar_to_UTF8(sreplace(tmp, Artist + L" - ", L""));
			artist = WSTRUtils::wchar_to_UTF8(Artist);
		}
		else {
			title = WSTRUtils::wchar_to_UTF8(tmp);
			std::wstring pth = path_name;
			size_t hh = pth.find_last_of(L"\\/");
			if (hh != std::wstring::npos) {
				pth = pth.erase(0, hh + 1);
			}
			artist = WSTRUtils::wchar_to_UTF8(pth);
		}
	}
	const std::string& get_title() const {
		return title;
	}

	const std::string& get_artist() const {
		return artist;
	}
private:
	std::string title;
	std::string artist;
};

class Video : public Media {
public:
	Video() : Media() {
	}
	Video(const Video& media) : Media(media) {
		this->title = media.title;
		this->description = media.description;
	}
	void operator=(const Video& media) {
		this->title = media.title;
		this->description = media.description;
		Media::operator=(media);
	}
	Video(std::wstring path_name, std::wstring file_name, time_t mod_time, time_t create_time, unsigned long long file_size) : Media(Media::TYPE::TYPE_VIDEO, path_name, file_name, mod_time, create_time, file_size) {
		title = WSTRUtils::wchar_to_UTF8(Media::strip_ext(file_name));
		std::wstring pth = path_name;
		size_t hh = pth.find_last_of(L"\\/");
		if (hh != std::wstring::npos) {
			pth = pth.erase(0, hh + 1);
		}
		description = WSTRUtils::wchar_to_UTF8(pth);
	}
	const std::string& get_title() const {
		return title;
	}
	const std::string& get_description() const {
		return description;
	}
private:
	std::string title;
	std::string description;
};

class Photo : public Media {
public:
	Photo() : Media() {
	}
	Photo(const Photo& media) : Media(media) {
		this->text = media.text;
	}
	void operator=(const Photo& media) {
		this->text = media.text;
		Media::operator=(media);
	}
	Photo(std::wstring path_name, std::wstring file_name, time_t mod_time, time_t create_time, unsigned long long file_size) : Media(Media::TYPE::TYPE_PHOTO, path_name, file_name, mod_time, create_time, file_size) {
		std::wstring pth = path_name;
		size_t hh = pth.find_last_of(L"\\/");
		if (hh != std::wstring::npos) {
			pth = pth.erase(0, hh + 1);
		}
		text = WSTRUtils::wchar_to_UTF8(pth) + "/" + WSTRUtils::wchar_to_UTF8(Media::strip_ext(file_name));
	}
	const std::string& get_text() const {
		return text;
	}
private:
	std::string text;
};

class ServerMethodInfo
{
public:
	ServerMethodInfo() {
		Func = NULL;
		NeedHost = false;
		NeedAuth = false;
	}
	ServerMethodInfo(void* Func, bool NeedHost, bool NeedAuth)
	{
		this->Func = Func;
		this->NeedHost = NeedHost;
		this->NeedAuth = NeedAuth;
	}
	void* Func;
	bool NeedHost;
	bool NeedAuth;
};

class ServerMethods
{
public:
	ServerMethods()
	{
		Inited = false;
	}
	void SetInited()
	{
		Inited = true;
	}
	bool GetInited() const
	{
		return Inited;
	}
	void RegisterMethod(const std::string& UTF8Path, void* Func, bool NeedHost = false, bool NeedAuth = false);
	void Execute(const std::string& UTF8Path, RequestParserStruct& Req, void* client) const;
private:
	Map::Map<std::string, ServerMethodInfo> Methods;
	bool Inited;
};

class Inode {
public:
	static bool greator(const Inode& lhs, const Inode& rhs)
	{
		return lhs.modification_time > rhs.modification_time || lhs.type == InodeType::INODE_FOLDER && rhs.type != InodeType::INODE_FOLDER;
	}
	enum class InodeType {
		INODE_ERROR = -1,
		INODE_FOLDER,
		INODE_PHOTO,
		INODE_VIDEO,
		INODE_AUDIO
	};
	Inode() {
		type = InodeType::INODE_FOLDER;
		size = 0;
		modification_time = 0;
		id = 1;
		owner_id = 1;
		parent = nullptr;
	}
	Inode(const std::wstring name) {
		this->name = name;
		type = InodeType::INODE_ERROR;
		size = 0;
		modification_time = 0;
		id = 0;
		owner_id = 0;
		parent = nullptr;
	}
	static Inode* make_subRoot(const std::wstring path, Inode& parent) {
		auto w = wsplit(path, L"\\\\|\\/");
		if (w.size() > 0) {
			std::wstring h = w[w.size() - 1];
			if (parent.find_by_path(h) == nullptr) {
				Inode sRt;
				sRt.name = h;
				parent.entries.push_back(sRt);
				return &parent.entries.back();
			}
			return parent.find_by_path(h);
		}
		return nullptr;
	}
	Inode& folder() {
		type = InodeType::INODE_FOLDER;
		size = 0;
		modification_time = 0;
		return *this;
	}
	Inode& audio(long long size, time_t modification_time) {
		type = InodeType::INODE_AUDIO;
		this->size = size;
		this->modification_time = modification_time;
		return *this;
	}
	Inode& video(long long size, time_t modification_time) {
		type = InodeType::INODE_VIDEO;
		this->size = size;
		this->modification_time = modification_time;
		return *this;
	}
	Inode& photo(long long size, time_t modification_time) {
		type = InodeType::INODE_PHOTO;
		this->size = size;
		this->modification_time = modification_time;
		return *this;
	}
	static std::wstring get_ext(const std::wstring& file_name) {
		std::wstring tmp = file_name;
		size_t n = tmp.find_last_of(L'.');
		if (n != std::wstring::npos && n != tmp.size() - 1) {
			return tmp.erase(0, n + 1);
		}
		return tmp;
	}
	std::wstring getFullPath() {
		std::wstring ret;
		Inode* p = parent;
		bool fst = true;
		while (p != nullptr && (p->id != 1 && p->owner_id != 1)) {
			if (fst) {
				fst = false;
				ret = p->name;
			}
			else {
				ret = p->name + L"\\" + ret;
			}
			p = p->parent;
		}
		return ret;
	}
	static std::wstring getName(Inode* inode) {
		if (inode == nullptr) {
			return L"";
		}
		return inode->name;
	}
	static void updateDir(Inode* inode) {
		if (inode == nullptr) {
			return;
		}
		inode->updateDir();
	}
	void updateDir() {
		if (type != InodeType::INODE_FOLDER) {
			return;
		}
		entries.sort(greator);
		for (auto& i : entries) {
			if (i.type != InodeType::INODE_FOLDER) {
				preview_hash = i.hash;
				break;
			}
		}
		if (entries.size() > 0) {
			modification_time = (*entries.begin()).modification_time;
		}
		else {
			modification_time = time(0);
		}
	}
	Inode* attachTo(Inode& parentNode, const std::wstring&root) {
		parent = &parentNode;
		id = crc32(WSTRUtils::wchar_to_UTF8(name));
		owner_id = crc32(WSTRUtils::wchar_to_UTF8(WSTRUtils::combine_root_path(root, getFullPath(), parentNode.name)));
		if (type != InodeType::INODE_FOLDER) {
			hash = std::to_string(modification_time) + SHA1::from_string(WSTRUtils::wchar_to_UTF8(WSTRUtils::combine_root_path(root, getFullPath(), name))) + "." + WSTRUtils::wchar_to_UTF8(WSTRUtils::toLowerW(get_ext(name)));
			preview_hash = hash;
		}
		else {
			entries.sort(greator);
			for (auto& i : entries) {
				if (i.type != InodeType::INODE_FOLDER) {
					preview_hash = i.preview_hash;
					modification_time = i.modification_time;
					break;
				}
			}
			if (modification_time <= 0) {
				modification_time = time(0);
			}
		}
		parentNode.entries.push_back(*this);
		return &parentNode.entries.back();
	}
	Inode* find_by_path(const std::wstring& path) {
		if (path.empty()) {
			return this;
		}
		Inode* ret = this;
		Inode* tp = this;
		auto bb = wsplit(path, L"\\\\|\\/");
		for (auto& i :bb) {
			for (auto& s : tp->entries) {
				if (s.name == i) {
					tp = &s;
					break;
				}
			}
			if (tp == ret) {
				return nullptr;
			}
			ret = tp;
		}
		return ret;
	}

	void clear() {
		entries.clear();
	}
	nlohmann::json serialize(const RequestParserStruct& Req, bool isSSL, size_t &count);
private:
	std::wstring name;
	std::string preview_hash;
	std::string hash;
	time_t modification_time;
	InodeType type;
	long long size;
	Inode* parent;
	std::list<Inode> entries;

	int id;
	int owner_id;
};
