#include <iostream>
#include <list>
#include <vector>
#include <cstring>
#include <tchar.h>
#include <filesystem>
#include <fstream>
#include <Windows.h>
#include "sha3.hpp"
#include "WSTRUtils.h"
#include "json.hpp"
#include "config_json_parse.h"

using namespace std;
using namespace WSTRUtils;
using namespace nlohmann;
namespace fs = std::filesystem;

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

class HashFile {
public:
	HashFile() {

	}
	HashFile(const wstring &file_name, const string &hash) {
		this->file_name = file_name;
		this->hash = hash;
	}
	bool operator==(const HashFile& file) {
		return this->file_name == file.file_name;
	}
	bool operator!=(const HashFile& file) {
		return this->file_name != file.file_name;
	}
	bool operator>(const HashFile& file) {
		return this->file_name > file.file_name;
	}
	bool operator<(const HashFile& file) {
		return this->file_name < file.file_name;
	}
	HashFile& deserialize(json& pref) {
		file_name = UTF8_to_wchar(pref.at("file_name").get<string>());
		hash = pref.at("hash").get<string>();
		return *this;
	}
	json serialize() const {
		json pref = json::object();
		pref.emplace("file_name", wchar_to_UTF8(file_name));
		pref.emplace("hash", hash);
		return pref;
	}
	wstring fix_to_out_path() const {
		wstring tmp = file_name;
		size_t fdir = tmp.find_first_of(L"\\/");
		if (fdir != wstring::npos) {
			tmp = tmp.substr(fdir + 1);
		}
		for (auto& i : tmp) {
			if (i == L':') {
				i = L'_';
			}
		}
		tmp = ExtractAppPath() + L"\\out\\" + tmp;
		return tmp;
	}
	wstring file_name;
	string hash;
};

class PHasher {
public:
	PHasher(const wstring& path, const list<wstring> &ignore_dirs) {
		this->ignore_dirs = ignore_dirs;
		this->path = FixSlashs(path);
		while (this->path.back() == L'\\') {
			this->path.pop_back();
		}
	}
	json makeHashs() {
		count = 0;
		listdir(path);
		files.sort();
		json pref = json::array();
		for (auto& i : files) {
			pref.push_back(i.serialize());
		}
		return pref;
	}
private:
	std::wstring FixSlashs(const std::wstring& Path)
	{
		std::wstring ret = Path;
		for (auto& i : ret)
		{
			if (i == L'/')
				i = L'\\';
		}
		return ret;
	}
	void listdir(const wstring& lpFolder)
	{
		for (auto& i : ignore_dirs) {
			if (wsearch(lpFolder, i) != wstring::npos) {
				cout << "Info: Ignore dir: " << wchar_to_UTF8(lpFolder) << " " << wchar_to_UTF8(i) << endl;
				return;
			}
		}
		WIN32_FIND_DATAW FindFileData;
		HANDLE hFindFile;
		hFindFile = FindFirstFileW((lpFolder + L"\\*").c_str(), &FindFileData);
		if (hFindFile != INVALID_HANDLE_VALUE)
		{
			do
			{
				if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{
					if (wcscmp(FindFileData.cFileName, L".") != 0 && wcscmp(FindFileData.cFileName, L"..") != 0) {
						listdir(lpFolder + L"\\" + FindFileData.cFileName);
					}
				}
				else {
					cout << "Checking: " << ++count << ") " << wchar_to_UTF8(lpFolder + L"\\" + FindFileData.cFileName) << endl;
					string hash = SHA3_512::sha3_file(lpFolder + L"\\" + FindFileData.cFileName);
					if (hash == "error") {
						cout << endl << "Error open: " << wchar_to_UTF8(lpFolder + L"\\" + FindFileData.cFileName) << endl;
						cout << "Abort (Y : N)?" << endl;
						char vr;
						cin >> vr;
						if (vr == 'Y' || vr == 'y')
							exit(1);
						return;
					}
					else {
						files.push_back(HashFile(lpFolder + L"\\" + FindFileData.cFileName, hash));
					}
				}
			} while (FindNextFile(hFindFile, &FindFileData));
			FindClose(hFindFile);
		}
		else {
			cout << endl << "Error open: " << wchar_to_UTF8(lpFolder) << endl;
			cout << "Abort (Y : N)?" << endl;
			char vr;
			cin >> vr;
			if (vr == 'Y' || vr == 'y')
				exit(1);
			return;
		}
	}
	long long count;
	list<wstring> ignore_dirs;
	list<HashFile> files;
	wstring path;
};

static void mkFolders(const wstring& file) {
	try {
		fs::path p = file;
		if (!fs::exists(p.parent_path())) {
			if (!fs::create_directories(p.parent_path())) {
				cout << endl << "Error create dir: " << wchar_to_UTF8(file) << endl;
				cout << "Abort" << endl;
				exit(1);
			}
		}
	}
	catch (fs::filesystem_error e) {
		cout << endl << "Error create dir: " << wchar_to_UTF8(file) << " " << e.what() << endl;
		cout << "Abort" << endl;
		exit(1);
	}
}

void do_copy_file(const wstring& first, const wstring& second) {
	wstring lg;
	lg += first;
	cout << "       ->|Copy| " << wchar_to_UTF8(lg) << endl;
	try {
		if (!fs::copy_file(first, second)) {
			cout << endl << "Error copy " << wchar_to_UTF8(first) << endl;
			exit(1);
			return;
		}
	}
	catch (fs::filesystem_error e) {
		cout << endl << "Error copy " << wchar_to_UTF8(first) << " " << e.what() << endl;
		exit(1);
		return;
	}
}


int _tmain(int argc, _TCHAR* argv[]) {
	locale::global(locale("ru_RU.UTF-8"));
	if (argc < 3) {
		cout << "\nDiskHasher - usage is: \n\n      DiskHasher --hash <path>\n\n      DiskHasher --check <file_new> <file_old>\n\n      DiskHasher --restore <file_new> <file_old>\n\n";
		return 1;
	}
	if (wstring(argv[1]) == L"--hash") {
		//PHasher hasher(argv[2], { L"System Volume Information", L"$Recycle.Bin", L"FOUND.000", L"found.000", L"Backup\\Android" });
		PHasher hasher(argv[2], { L"System Volume Information", L"$Recycle.Bin", L"FOUND.000", L"found.000" });
		auto v = hasher.makeHashs();
		writeJsonConfig(ExtractAppPath() + L"\\" + FixFileNameAudio(argv[2]) + L".json", v);
	}
	else if (wstring(argv[1]) == L"--check" || wstring(argv[1]) == L"--restore") {
		bool isRestore = wstring(argv[1]) == L"--restore";
		if (argc < 4) {
			cout << "\nDiskHasher - usage is: \n\n      DiskHasher --hash <path>\n\n      DiskHasher --check <file_new> <file_old>\n\n      DiskHasher --restore <file_new> <file_old>\n\n";
			return 1;
		}
		json file_new;
		json file_old;
		if (!parseJsonConfig(argv[2], file_new) || !parseJsonConfig(argv[3], file_old)) {
			cout << "Error: reading configs!" << endl;
			return 1;
		}
		list<HashFile> files_new;
		list<HashFile> files_old;
		try {
			for (auto& i : file_new) {
				files_new.push_back(HashFile().deserialize(i));
			}
			for (auto& i : file_old) {
				files_old.push_back(HashFile().deserialize(i));
			}
		}
		catch (...) {
			cout << "Error: parse configs!" << endl;
			return 1;
		}
		long long count = 0;
		for (auto&i : files_old) {
			bool has = false;
			for (auto& s : files_new) {
				if (i.hash == s.hash) {
					has = true;
					break;
				}
			}
			if (!has) {
				if (isRestore) {
					wstring tt = i.fix_to_out_path();
					mkFolders(tt);
					do_copy_file(i.file_name, tt);
				}
				else {
					cout << "Found: " << ++count << ") " << wchar_to_UTF8(i.file_name) << endl;
				}
			}
		}
	}
	else {
		cout << "\nDiskHasher - usage is: \n\n      DiskHasher --hash <path>\n\n      DiskHasher --check <file_new> <file_old>\n\n      DiskHasher --restore <file_new> <file_old>\n\n";
		return 1;
	}
	cout << "Copyright (c) Umerov Artem, 2022" << endl;
	return 0;
}