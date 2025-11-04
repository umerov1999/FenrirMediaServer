#include <iostream>
#include <list>
#include <vector>
#include <cstring>
#include <tchar.h>
#include <filesystem>
#include <fstream>
#include <Windows.h>
#include "WSTRUtils.h"

using namespace std;
using namespace WSTRUtils;
namespace fs = std::filesystem;
#define BUF_SIZE 10485760

class Files {
public:
	Files(const wstring& second_dir, const wstring& out_dir) {
		temp_buf1 = vector<unsigned char>(BUF_SIZE);
		temp_buf2 = vector<unsigned char>(BUF_SIZE);
		new_dir = FixSlashs(second_dir);
        this->out_dir = FixSlashs(out_dir);
        while (new_dir.back() == L'\\') {
            new_dir.pop_back();
        }
        while (this->out_dir.back() == L'\\') {
            this->out_dir.pop_back();
        }
        new_dir += L"\\";
        this->out_dir += L"\\";
	}

	void add_ignore_dir(const wstring& dir) {
		ignore_folders.push_back(dir);
	}

	void load_files(const wstring& first_dir) {
        root_dir = FixSlashs(first_dir);
        while (root_dir.back() == L'\\') {
            root_dir.pop_back();
        }
        root_dir = root_dir + L"\\";
        listdir(first_dir);
	}

	void check_empty_files(const wstring& type) {
		cout << endl << "Check Empty Files " << wchar_to_UTF8(type) << "..." << endl;
		size_t count = 0;
		for (auto& i : files) {
			cout << "Checking: [" << ++count << "\\" << files.size() << "] "
				<< wchar_to_UTF8(build_first(i)) << endl;
			if (get_file_stat(build_first(i)).st_size == 0)
			{
				cout << "Info: Empty File: " << wchar_to_UTF8(build_first(i)) << endl;
				out_log.push_back(build_first(i) + L"\r\n");
			}

		}
		ofstream file;
		file.open(build_out(L"empty_" + type + L".txt"), ios::out);
		if (file) {
			for (auto& i : out_log) {
				file << wchar_to_UTF8(i);
			}
			file.close();
		}
		out_log.clear();
	}

	void check_lost_files() {
		cout << endl << "Working..." << endl;
		size_t count = 0;
		size_t copied = 0;
		for (auto& i : files) {
			cout << "Checking: [" << ++count << "\\" << files.size() << " >> " << copied << "] "
				<< wchar_to_UTF8(build_first(i)) << endl;
			try {
				if (!fs::exists(build_new(i))) {
					do_copy_file(build_first(i), build_out(i), COPY_STATUS::NOT_EXIST);
					copied++;
					continue;
				}
			}
			catch (fs::filesystem_error e) {
				cout << endl << "Error check exist: " << wchar_to_UTF8(build_new(i)) << " " << e.what() << endl;
				cout << "Abort" << endl;
				exit(1);
			}

			long long sz = get_file_stat(build_first(i)).st_size;
			if (sz != get_file_stat(build_new(i)).st_size) {
				do_copy_file(build_first(i), build_out(i), COPY_STATUS::SIZE_NOT_EQUAL);
				copied++;
				continue;
			}
			if (sz == 0)
				continue;
			FILE* flold;
			if (_wfopen_s(&flold, build_first(i).c_str(), L"rb") != 0) {
				cout << endl << "Error fopen: " << wchar_to_UTF8(build_first(i)) << endl;
				cout << "Abort" << endl;
				exit(1);
			}
			FILE* flnew;
			if (_wfopen_s(&flnew, build_new(i).c_str(), L"rb") != 0) {
				fclose(flold);
				cout << endl << "Error fopen: " << wchar_to_UTF8(build_new(i)) << endl;
				cout << "Abort" << endl;
				exit(1);
			}
			long long it = sz / BUF_SIZE;
			bool gout = false;
			for (long long s = 0; s < it; s++) {
				read_bin(temp_buf1, BUF_SIZE, flold);
				read_bin(temp_buf2, BUF_SIZE, flnew);
				if (memcmp(temp_buf1.data(), temp_buf2.data(), BUF_SIZE) != 0) {
					fclose(flold);
					fclose(flnew);
					do_copy_file(build_first(i), build_out(i), COPY_STATUS::NOT_COMAPRE);
					copied++;
					gout = true;
					break;
				}
			}
			if (gout) {
				continue;
			}
			sz -= it * BUF_SIZE;
			if (sz > 0) {
				read_bin(temp_buf1, sz, flold);
				read_bin(temp_buf2, sz, flnew);
				if (memcmp(temp_buf1.data(), temp_buf2.data(), (size_t)sz) != 0) {
					do_copy_file(build_first(i), build_out(i), COPY_STATUS::NOT_COMAPRE);
					copied++;
				}
			}
			fclose(flold);
			fclose(flnew);
		}
		ofstream file;
		file.open(build_out(L"out.txt"), ios::out);
		if (file) {
			for (auto& i : out_log) {
				file << wchar_to_UTF8(i);
			}
			file.close();
		}
		out_log.clear();
	}

private:
	enum class COPY_STATUS {
		NOT_EXIST,
		SIZE_NOT_EQUAL,
		NOT_COMAPRE
	};

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
	wstring build_first(const wstring& file) const {
		return root_dir + file;
	}

	wstring build_new(const wstring& file) const {
		return new_dir + file;
	}

	wstring build_out(const wstring& file) const {
		mkFolders(out_dir + file);
		return out_dir + file;
	}

	void do_copy_file(const wstring& first, const wstring& second, COPY_STATUS status) {
		wstring lg;
		switch (status) {
		case COPY_STATUS::NOT_EXIST:
			lg += L"!!![Not Found] ";
			break;
		case COPY_STATUS::SIZE_NOT_EQUAL:
			lg += L"[Size not equal] ";
			break;
		case COPY_STATUS::NOT_COMAPRE:
			lg += L"[Compare false] ";
			break;
		}
		lg += first;
        out_log.push_back(lg + L"\r\n");
		cout << "       ->|Copy| " << wchar_to_UTF8(lg) << endl;
		try {
			if (!fs::copy_file(first, second)) {
				cout << endl << "Error copy " << wchar_to_UTF8(first) << endl;
				out_log.push_back(L"Error!: " + lg + L"\r\n");
                cout << "Abort" << endl;
                exit(1);
			}
		}
		catch (fs::filesystem_error e) {
			cout << endl << "Error copy " << wchar_to_UTF8(first) << " " << e.what() << endl;
			out_log.push_back(L"Error!: " + lg + L"\r\n");
            cout << "Abort" << endl;
            exit(1);
		}
	}

	static struct _stat64 get_file_stat(const wstring& file) {
		struct _stat64 buf;
		if (_wstat64(file.c_str(), &buf) != 0) {
			cout << endl << "Error stat: " << wchar_to_UTF8(file) << endl;
			cout << "Abort" << endl;
			exit(1);
		}
		return buf;
	}

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

	void insert_file(wstring filename) {
		size_t pos = filename.find(root_dir);
		if (pos != string::npos)
			filename.erase(pos, root_dir.length());
		files.push_back(filename);
	}

	static void read_bin(vector<unsigned char>& buf, long long size, FILE* fl) {
		if (fread(buf.data(), 1, (size_t)size, fl) != size) {
			cout << endl << "Error read size " << size << endl;
			cout << "Abort" << endl;
			exit(1);
		}
	}

	void listdir(const wstring &lpFolder)
	{
		for (auto& i : ignore_folders) {
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
					insert_file(lpFolder + L"\\" + FindFileData.cFileName);
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

	wstring root_dir;
	wstring new_dir;
	wstring out_dir;
	list<wstring> files;
	list<wstring> ignore_folders;
	list<wstring> out_log;
	vector<unsigned char> temp_buf1;
	vector<unsigned char> temp_buf2;
};

int _tmain(int argc, _TCHAR* argv[]) {
	locale::global(locale("ru_RU.UTF-8"));
	if (argc < 4) {
		cout << "\ndiff_folders - usage is: \n\n      diff_folders <old> <new> <out>\n\n";
		return 1;
	}
	Files comp(argv[2], argv[3]);
	//comp.add_ignore_dir("media/0/Android");
	//comp.add_ignore_dir("media/0/WhatsApp");
	//comp.add_ignore_dir("media/0/Telegram");
	comp.add_ignore_dir(L"System Volume Information");
	comp.add_ignore_dir(L"$Recycle.Bin");
	comp.add_ignore_dir(L"FOUND.000");
	comp.add_ignore_dir(L"found.000");

	comp.load_files(argv[1]);
	comp.check_lost_files();
	comp.check_empty_files(L"old");

	Files second(argv[2], argv[3]);
	//second.add_ignore_dir("media/0/Android");
	//second.add_ignore_dir("media/0/WhatsApp");
	//second.add_ignore_dir("media/0/Telegram");
	second.add_ignore_dir(L"System Volume Information");
	second.add_ignore_dir(L"$Recycle.Bin");
	second.add_ignore_dir(L"FOUND.000");
	second.add_ignore_dir(L"found.000");

	second.load_files(argv[2]);
	second.check_empty_files(L"new");
	printf("Copyright (c) Umerov Artem, 2026\n");
	return 0;
}
