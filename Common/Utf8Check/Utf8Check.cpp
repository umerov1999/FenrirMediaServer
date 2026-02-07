#include <iostream>
#include <string>
#include <sstream>
#include <map>
#include <list>
#include <iomanip>
#include <tchar.h>
using namespace std;

static std::wstring get_ext(const std::wstring& file_name) {
	std::wstring tmp = file_name;
	size_t n = tmp.find_last_of(L'.');
	if (n != std::wstring::npos && n != tmp.size() - 1) {
		return tmp.erase(0, n + 1);
	}
	return L"";
}

int _tmain(int argc, _TCHAR* argv[])
{
	locale::global(locale("ru_RU.UTF-8"));

	if (argc <= 1) {
		cout << "Utf8Check.exe [filename]" << endl;
		return 0;
	}

	wstring ext = get_ext(argv[1]);
	if (ext == L"svg" || ext == L"ico" || ext == L"cur" || ext == L"png" || ext == L"bmp" || ext == L"jpg" || ext == L"zip" || ext == L"7z" || ext == L"tar" || ext == L"gz") {
		return 0;
	}

	FILE* fl = NULL;
	if (_wfopen_s(&fl, argv[1], L"rb") != 0) {
		return 0;
	}

	if (!fl) {
		return 0;
	}

	char magic[20];
	memset(magic, 0, 20);
	fread(magic, sizeof(char), 20, fl);
	fclose(fl);

	if (magic[0] == '\xef' && magic[1] == '\xbb' && magic[2] == '\xbf') {
		wcout << argv[1] << L" | UTF-8 BOM" << endl;
	} else if (magic[0] == '\xfe' && magic[1] == '\xff') {
		wcout << argv[1] << L" | UTF-16 BE" << endl;
	} else if (magic[0] == '\xff' && magic[1] == '\xfe' && (magic[2] != '\0' || magic[3] != '\0')) {
		wcout << argv[1] << L" | UTF-16 LE" << endl;
	} else if (magic[0] == '\0' && magic[1] == '\0' && magic[2] == '\xfe' && magic[3] == '\xff') {
		wcout << argv[1] << L" | UTF-32 BE" << endl;
	} else if (magic[0] == '\xff' && magic[1] == '\xfe' && magic[2] == '\0' && magic[3] == '\0') {
		wcout << argv[1] << L" | UTF-32 LE" << endl;
	}
	else {
		wcout << argv[1] << L" | UNKNOW" << endl;
	}
}